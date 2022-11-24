#include "soc.h"

void set_coordinator_next(char str[]);
void setup_server();
void setup_client_with_port(int port);
void append_cur_id();
void setup_chopsticks();
void think();
void eat();
int request_chopstick(int chopstick);
void release_chopstick(int chopstick);
void send_i_message();
int receive_x_from_coordinator();
void release_both_chopsticks(int left, int right);

int id = -1;
int coordinator = -1;

int sock_read;
int new_sock_read;
int sock_write;
char buffer[BUFFER_LEN];
socklen_t clientLength;
struct sockaddr_in serv_adr, client_adr;
struct sockaddr_in read_adr;
int err;

int self_read_port;
int next_write_port;
int coordinator_port;
int should_start_election;

int my_index = -1;
int coordinator_index = -1;
int left_chopstick = -1;
int right_chopstick = -1;
int has_left_chopstick = 0;
int has_right_chopstick = 0;

char election_message[BUFFER_LEN] = "E;";
char coordinator_message[BUFFER_LEN] = "C;";
char central_message[BUFFER_LEN] = "Q;";

int is_eating = 0;
int is_thinking = 0;

int got_x_from_coordinator = 0;
char buffer_x[BUFFER_LEN];

#define print_log(f_, ...) printf("[%s] PHIL ID %d: ", timestamp(), id), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro, set philosopher ID

int main(int argc, char *argv[])
{
    id = atoi(argv[1]);
    self_read_port = atoi(argv[2]);
    next_write_port = atoi(argv[3]);
    should_start_election = atoi(argv[4]);

    if (should_start_election)
    {
        // Starting election. Sending out first election message
        sleep(1); // Wait until readers are ready; might need more than 1 sec
        setup_client_with_port(next_write_port);
        append_cur_id();
        err = write(sock_write, election_message, sizeof(election_message));
        check_syscall_err(err, "write error");
        close(sock_write);

        // Done sending out first election message, now listen for incoming election message
        setup_server();
        err = read(new_sock_read, buffer, sizeof(buffer));
        check_syscall_err(err, "read error");
        sprintf(election_message, "%s", buffer);
        print_log("FROM CLIENT: %s\n", election_message);
        close(sock_read);
        close(new_sock_read);

        // Done with election. Now send out Coordinator message
        sleep(1);
        setup_client_with_port(next_write_port);
        sprintf(coordinator_message, "%s", election_message);
        coordinator_message[0] = 'C';
        err = write(sock_write, coordinator_message, sizeof(coordinator_message));
        check_syscall_err(err, "write error");
        close(sock_write);

        // Listen for Coordinator message to come back
        setup_server();
        err = read(new_sock_read, buffer, sizeof(buffer));
        check_syscall_err(err, "read error");
        print_log("FROM CLIENT: %s\n", buffer);
        sprintf(coordinator_message, "%s", buffer);
        close(sock_read);
        close(new_sock_read);
    }
    else
    {
        // I am not to start election. Listen for incoming requests with election message
        setup_server();
        err = read(new_sock_read, buffer, sizeof(buffer));
        check_syscall_err(err, "read error");

        print_log("FROM CLIENT: %s\n", buffer);
        sprintf(election_message, "%s", buffer);
        append_cur_id();
        close(sock_read);
        close(new_sock_read);

        // Done listening. Need to send election message to next
        sleep(1);
        setup_client_with_port(next_write_port);
        err = write(sock_write, election_message, sizeof(election_message));
        check_syscall_err(err, "write error");
        close(sock_write);

        // Done sending election message, now listen for Coordinator message
        setup_server();
        err = read(new_sock_read, buffer, sizeof(buffer));
        check_syscall_err(err, "read error");
        print_log("FROM CLIENT: %s\n", buffer);
        sprintf(coordinator_message, "%s", buffer);
        close(sock_read);
        close(new_sock_read);

        // Send Coordinator message to next node
        sleep(1);
        setup_client_with_port(next_write_port);
        coordinator_message[0] = 'C';
        err = write(sock_write, coordinator_message, sizeof(coordinator_message));
        check_syscall_err(err, "write error");
        close(sock_write);

        // Now that I have full coordinator message, I am done communicating with peers
    }

    set_coordinator_next(coordinator_message);

    // Detect if I am the coordinator
    if (coordinator == id)
    {
        char phil_id_char[BUFFER_LEN];
        sprintf(phil_id_char, "%d", id);
        char self_read_port_char[BUFFER_LEN];
        sprintf(self_read_port_char, "%d", self_read_port);
        err = execl("./bin/coordinator", "coordinator", phil_id_char, self_read_port_char, buffer, (char *)NULL);
        check_syscall_err(err, "Execl coordinator failed");
    }

    // If I'm here, it means I'm not the coordinator
    setup_chopsticks();

    sleep(10);
    // Start communication with coordinator only after ring algo completely done
    setup_client_with_port(coordinator_port);

    send_i_message();
    sleep(1);
    get_random_in_range(1200000, 5000000);

    int i = 0;
    while (1)
    {
        if (is_eating == 0 && is_thinking == 0)
        {
            print_log("I want to eat\n");
            has_left_chopstick = request_chopstick(left_chopstick);
            if (has_left_chopstick)
            {
                print_log("Got left chopstick\n");
                has_right_chopstick = request_chopstick(right_chopstick);
                if (has_right_chopstick)
                {
                    print_log("Got right chopstick\n");
                    eat();
                    release_both_chopsticks(left_chopstick, right_chopstick);
                }
                else
                {
                    print_log("Failed to get right chopstick\n");
                    release_chopstick(left_chopstick);
                    // DO NOT THINK/REQUEST NEW CHOPSTICK IF YOU FAILED. you are added to coordinator queue, just wait for X
                    int got_x = receive_x_from_coordinator();
                    if (got_x)
                    {
                        print_log("Got X from coordinator\n");
                        eat();
                        release_both_chopsticks(left_chopstick, right_chopstick);
                    }
                }
            }
            else
            {

                print_log("Failed to get left chopstick\n");
                // DO NOT THINK/REQUEST NEW CHOPSTICK IF YOU FAILED. you are added to coordinator queue, just wait for X
                int got_x = receive_x_from_coordinator();
                if (got_x)
                {
                    print_log("Got X from coordinator\n");
                    eat();
                    release_both_chopsticks(left_chopstick, right_chopstick);
                }
            }
        }
        think();
    }

    return EXIT_SUCCESS;
}

void append_cur_id() // Append current phil ID to message
{
    char id_char[100];
    sprintf(id_char, "%d;", id);
    strncat(election_message, id_char, str_length(id_char));
}

void setup_chopsticks() // Determine which chopstick a philosopher will use
{
    if (coordinator_index == 0)
    {
        if (my_index == 1)
        {
            left_chopstick = 1;
            right_chopstick = 2;
        }
        else if (my_index == 2)
        {
            left_chopstick = 2;
            right_chopstick = 3;
        }
        else if (my_index == 3)
        {
            left_chopstick = 3;
            right_chopstick = 4;
        }
        else if (my_index == 4)
        {
            left_chopstick = 4;
            right_chopstick = 5;
        }
        else if (my_index == 5)
        {
            left_chopstick = 1;
            right_chopstick = 5;
        }
    }
    else if (coordinator_index == 1)
    {
        if (my_index == 0)
        {
            left_chopstick = 1;
            right_chopstick = 2;
        }
        else if (my_index == 2)
        {
            left_chopstick = 2;
            right_chopstick = 3;
        }
        else if (my_index == 3)
        {
            left_chopstick = 3;
            right_chopstick = 4;
        }
        else if (my_index == 4)
        {
            left_chopstick = 4;
            right_chopstick = 5;
        }
        else if (my_index == 5)
        {
            left_chopstick = 1;
            right_chopstick = 5;
        }
    }
    else if (coordinator_index == 2)
    {
        if (my_index == 0)
        {
            left_chopstick = 1;
            right_chopstick = 2;
        }
        else if (my_index == 1)
        {
            left_chopstick = 2;
            right_chopstick = 3;
        }
        else if (my_index == 3)
        {
            left_chopstick = 3;
            right_chopstick = 4;
        }
        else if (my_index == 4)
        {
            left_chopstick = 4;
            right_chopstick = 5;
        }
        else if (my_index == 5)
        {
            left_chopstick = 1;
            right_chopstick = 5;
        }
    }
    else if (coordinator_index == 3)
    {
        if (my_index == 0)
        {
            left_chopstick = 1;
            right_chopstick = 2;
        }
        else if (my_index == 1)
        {
            left_chopstick = 2;
            right_chopstick = 3;
        }
        else if (my_index == 2)
        {
            left_chopstick = 3;
            right_chopstick = 4;
        }
        else if (my_index == 4)
        {
            left_chopstick = 4;
            right_chopstick = 5;
        }
        else if (my_index == 5)
        {
            left_chopstick = 1;
            right_chopstick = 5;
        }
    }
    else if (coordinator_index == 4)
    {
        if (my_index == 0)
        {
            left_chopstick = 1;
            right_chopstick = 2;
        }
        else if (my_index == 1)
        {
            left_chopstick = 2;
            right_chopstick = 3;
        }
        else if (my_index == 2)
        {
            left_chopstick = 3;
            right_chopstick = 4;
        }
        else if (my_index == 3)
        {
            left_chopstick = 4;
            right_chopstick = 5;
        }
        else if (my_index == 5)
        {
            left_chopstick = 1;
            right_chopstick = 5;
        }
    }
    else if (coordinator_index == 5)
    {
        if (my_index == 0)
        {
            left_chopstick = 1;
            right_chopstick = 2;
        }
        else if (my_index == 1)
        {
            left_chopstick = 2;
            right_chopstick = 3;
        }
        else if (my_index == 2)
        {
            left_chopstick = 3;
            right_chopstick = 4;
        }
        else if (my_index == 3)
        {
            left_chopstick = 4;
            right_chopstick = 5;
        }
        else if (my_index == 4)
        {
            left_chopstick = 1;
            right_chopstick = 5;
        }
    }
}

void set_coordinator_next(char str[]) // After election is done, coordinator has to be chosen - the highest Phil ID
{
    int max = id;
    char *token = strtok(str, SEPARATORS);
    int id_ints[PHILOSOPHER_COUNT];
    int i = 0;
    while (token != NULL)
    {
        int token_int = atoi(token);
        id_ints[i] = token_int;
        if (token_int == id) // found myself
        {
            my_index = i;
        }
        if (token_int > max)
        {
            max = token_int;
            coordinator_index = i;
        }
        token = strtok(NULL, SEPARATORS);
        i++;
    }

    if (my_index + 1 == coordinator_index && coordinator_index == PHILOSOPHER_COUNT - 1)
    {
        next_write_port = read_ports[0];
    }
    else if (my_index == PHILOSOPHER_COUNT - 1 && coordinator_index == 0)
    {
        next_write_port = read_ports[1];
    }
    else if (my_index + 1 == coordinator_index)
    {
        next_write_port = read_ports[my_index + 2];
    }

    coordinator = max;
    coordinator_port = read_ports[coordinator_index];
}

void setup_server() // Setup server on known port
{
    sock_read = socket(AF_INET, SOCK_STREAM, 0);
    check_syscall_err(sock_read, "Socket opening failed");

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(self_read_port);

    check_syscall_err(bind(sock_read, (struct sockaddr *)&serv_adr, sizeof(serv_adr)), "Binding to socket failed");
    check_syscall_err(listen(sock_read, MAX_CLIENT_QUEUE), "Listening to socket failed");

    clientLength = sizeof(client_adr);
    new_sock_read = accept(sock_read, (struct sockaddr *)&client_adr, &clientLength);
    check_syscall_err(new_sock_read, "Socket accept failed");
}

void setup_client_with_port(int port) // Setup client listening on port
{
    sock_write = socket(AF_INET, SOCK_STREAM, 0);
    check_syscall_err(sock_write, "Socket opening failed");

    read_adr.sin_family = AF_INET;
    read_adr.sin_port = htons(port);
    read_adr.sin_addr.s_addr = inet_addr(SERVERIP);

    check_syscall_err(connect(sock_write, (struct sockaddr *)&read_adr, sizeof(read_adr)), "Error connecting");
    sleep(1);
}

void think()
{
    int think_time = get_random_in_range(1200000, 5000000); // 1.2s - 5s
    print_log("Thinking for %.2f\n", think_time / (float)1000000);
    is_thinking = 1;
    usleep(think_time);
    is_thinking = 0;
    print_log("=== DONE THINKING ===\n");
}

void eat()
{
    int eat_time = get_random_in_range(1200000, 5000000); // 1.2s - 5s
    print_log("Eating for %.2f\n", eat_time / (float)1000000);
    is_eating = 1;
    usleep(eat_time);
    is_eating = 0;
    print_log("=== DONE EATING ===\n");
}

int request_chopstick(int chopstick) // Q message - request 1 chopstick from coordinator
{
    char msg[BUFFER_LEN];
    sprintf(msg, "Q;%d;%d;", id, chopstick);
    print_log("Requesting chopstick: %s\n", msg);
    err = send(sock_write, msg, sizeof(msg), 0);
    check_syscall_err(err, "chopstick err");

    char recv_buffer[2];
    err = recv(sock_write, recv_buffer, 2, 0);
    check_syscall_err(err, "get_response_chopstick err");
    print_log("Got response for chopstick: %c\n", recv_buffer[0]);

    if (recv_buffer[0] == 'Y')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void release_chopstick(int chopstick) // R message - one chopstick is released when philosopher can't get the other one
{
    char msg[BUFFER_LEN];
    sprintf(msg, "R;%d;%d;", id, chopstick);
    print_log("Releasing chopstick: %s\n", msg);
    err = send(sock_write, msg, sizeof(msg), 0);
    check_syscall_err(err, "release chopstick err");
}

void release_both_chopsticks(int left, int right) // W message - both chopsticks can be released at the same time when philosopher is done eating
{
    char msg[BUFFER_LEN];
    sprintf(msg, "W;%d;%d;%d;", id, left, right);
    print_log("Releasing BOTH chopstick: %s\n", msg);
    err = send(sock_write, msg, sizeof(msg), 0);
    check_syscall_err(err, "release both chopsticks err");
}

int receive_x_from_coordinator() // X type message comes only from coordinator. It indicates that philosopher can start eating
{
    char recv_buffer[2];
    err = recv(sock_write, recv_buffer, 2, 0);
    check_syscall_err(err, "get_response_chopstick err");
    print_log("Got response for chopstick: %c\n", recv_buffer[0]);

    if (recv_buffer[0] == 'X')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void send_i_message() // I message is used to signal to coordinator which chopsticks a Philosopher uses
{
    char msg[BUFFER_LEN];
    sprintf(msg, "I;%d;%d;%d;", id, left_chopstick, right_chopstick);
    print_log("Sending I message: %s\n", msg);
    err = send(sock_write, msg, sizeof(msg), 0);
    check_syscall_err(err, "send i message err");
}
