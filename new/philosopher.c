#include "soc.h"

void set_coordinator_next(char str[]);
void setup_server();
void setup_client();
// void setup_client(int port);
void setup_client_with_port(int port);
void append_cur_id();

int id = -1;
int coordinator = -1;

int sock_read;
int new_sock_read;
// int pid;
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

char election_message[BUFFER_LEN] = "E;";
char coordinator_message[BUFFER_LEN] = "C;";
// char coordinator_message[BUFFER_LEN] = "C;123;45;6;78;1;";

#define print_log(f_, ...) printf("[%s] PHIL ID: %d ", timestamp(), id), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro, set philosopher ID

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
        setup_client();
        append_cur_id();
        // print_log("Appended String to send: %s\n", election_message);
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
        setup_client();
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
        print_log("DONE WITH ELECTION STARTER\n");
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
        // print_log("Appended String to send: %s\n", election_message);
        close(sock_read);
        close(new_sock_read);

        // Done listening. Need to send election message to next
        sleep(1);
        setup_client();
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
        setup_client();
        coordinator_message[0] = 'C'; // BUG - so somewhere here . P0 gets only letter C from P5
        err = write(sock_write, coordinator_message, sizeof(coordinator_message));
        check_syscall_err(err, "write error");
        close(sock_write);
        // print_log("DONE WITH NOT ELECTION STARTER\n");

        // Now that I have full coordinator message, I am done communicating with peers

        // TODO - now think of logic to cut out the coordinator and rearrange the ring
    }

    set_coordinator_next(coordinator_message);
    print_log("COORDINATOR: %d, NEXT PORT: %d\n", coordinator, next_write_port);

    // BUG - important - need to find port from message

    // detect if I am the coordinator
    if (coordinator == id)
    {
        print_log("I AM COORDINATOR\n");
        char phil_id_char[BUFFER_LEN];
        sprintf(phil_id_char, "%d", id);
        char self_read_port_char[BUFFER_LEN];
        sprintf(self_read_port_char, "%d", self_read_port);
        err = execl("./coordinator", "coordinator", phil_id_char, self_read_port_char, (char *)NULL);
        check_syscall_err(err, "Execl coordinator failed");
    }

    // If I'm here, it means I'm not the coordinator
    // Need to replace next port by coordinator's port?

    // Send request to coordinator
    sleep(10);
    setup_client_with_port(coordinator_port);
    char test_to_send[BUFFER_LEN];
    sprintf(test_to_send, "%d", 1);
    err = write(sock_write, test_to_send, sizeof(test_to_send));
    check_syscall_err(err, "write error after coord decided");
    close(sock_write);
    // print_log("DONE WITH NOT ELECTION STARTER\n");

    // TODO - might need to use threads like Hamnes said - on thread listens for messages

    return EXIT_SUCCESS;
}

void append_cur_id()
{
    char id_char[100];
    sprintf(id_char, "%d", id);
    char semicolon_char[100];
    sprintf(semicolon_char, "%c", ';');

    strncat(election_message, id_char, str_length(id_char));
    strncat(election_message, semicolon_char, str_length(semicolon_char));
}

void set_coordinator_next(char str[]) // TODO - strs old, do I even need to detect next PHIL ID? Since I have port?
{
    print_log("========INSIDE SET COORD\n");
    int max = id;
    char *token = strtok(str, SEPARATORS);
    int id_ints[PHILOSOPHER_COUNT];
    int i = 0;
    int my_index = -1;
    int coordinator_index = -1;
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
    print_log("========DONE WITH WHILE. MAX: %d, COORD_INDEX: %d, MY_INDEX: %d\n", max, coordinator_index, my_index);
    // BUG - here to determine if next port is to be changed. ALSO - need to include if self is coordinator?
    if (my_index + 1 == coordinator_index && coordinator_index == PHILOSOPHER_COUNT - 1) // BUG - PHILOSOPHER_COUNT - off-by-one errors!
    {
        print_log("========FIRST_IF, %d\n", read_ports[0]);
        next_write_port = read_ports[0];
    }
    else if (my_index == PHILOSOPHER_COUNT - 1 && coordinator_index == 0)
    {
        print_log("========SECOND_IF, %d\n", read_ports[1]);
        next_write_port = read_ports[1];
    }
    else if (my_index + 1 == coordinator_index)
    {
        print_log("========THIRD_IF, %d\n", read_ports[my_index + 2]);
        next_write_port = read_ports[my_index + 2];
    }

    coordinator = max;
    coordinator_port = read_ports[coordinator_index];
}

void setup_server()
{
    sock_read = socket(AF_INET, SOCK_STREAM, 0);
    check_syscall_err(sock_read, "Socket opening failed");

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(self_read_port);

    check_syscall_err(bind(sock_read, (struct sockaddr *)&serv_adr, sizeof(serv_adr)), "Binding to socket failed");
    check_syscall_err(listen(sock_read, MAX_CLIENT_QUEUE), "Listening to socket failed");
    // print_log("Node listening on port: %d\n", self_read_port);

    clientLength = sizeof(client_adr);
    new_sock_read = accept(sock_read, (struct sockaddr *)&client_adr, &clientLength);
    check_syscall_err(new_sock_read, "Socket accept failed");
}

void setup_client()
{
    sock_write = socket(AF_INET, SOCK_STREAM, 0);
    check_syscall_err(sock_write, "Socket opening failed");

    read_adr.sin_family = AF_INET;
    read_adr.sin_port = htons(next_write_port);
    read_adr.sin_addr.s_addr = inet_addr(SERVERIP);

    check_syscall_err(connect(sock_write, (struct sockaddr *)&read_adr, sizeof(read_adr)), "Error connecting");
    sleep(1);
    print_log("Writer created\n");
}

// TODO - delete the other version and refactor all?
void setup_client_with_port(int port)
{
    sock_write = socket(AF_INET, SOCK_STREAM, 0);
    check_syscall_err(sock_write, "Socket opening failed");

    read_adr.sin_family = AF_INET;
    read_adr.sin_port = htons(port);
    read_adr.sin_addr.s_addr = inet_addr(SERVERIP);

    check_syscall_err(connect(sock_write, (struct sockaddr *)&read_adr, sizeof(read_adr)), "Error connecting");
    sleep(1);
    // print_log("Writer created\n");
}