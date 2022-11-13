#include "soc.h"

// TODO - highest philosopher ID will be always different node because it's random
// TODO - So can set one node to run algo first always?
// TODO - need to remember to cut out coordinator from ring

int str_length(char str[]);
void set_coordinator_next(char str[]);
void setup_server();
void setup_client();

int id = 123;
int next_id = -1;
int coordinator = -1;

int sock_read;
int new_sock_read;
int pid;
int sock_write;
char buffer[BUFFER_LEN];
socklen_t clientLength;
struct sockaddr_in serv_adr, client_adr;
struct sockaddr_in read_adr;
int err;

int self_read_port;
int next_write_port;
int should_start_election;

int main(int argc, char *argv[])
{
    id = atoi(argv[1]);
    self_read_port = atoi(argv[2]);
    next_write_port = atoi(argv[3]);
    should_start_election = atoi(argv[4]);
    printf("SHOULD START ELECTION: %d\n", should_start_election);

    char str[100] = "C;123;45;6;78;1;";

    // setup_server();
    // setup_client();

    if (should_start_election)
    {
        sleep(1);
        // Start election
        setup_client();
        err = write(sock_write, str, sizeof(str));
        if (err == -1)
        {
            print_log("WRITE ERR\n");
            exit(EXIT_FAILURE);
        }
    }
    if (!should_start_election)
    {
        setup_server();
        err = read(new_sock_read, buffer, sizeof(buffer));
        if (err == -1)
        {
            print_log("READ ERR\n");
            exit(EXIT_FAILURE);
        }
        printf("FROM CLIENT: %s\n", buffer);
    }
    // TODO - need to think of a mechanism of switching from reading to writing mode?

    // err = read(new_sock_read, buffer, sizeof(buffer));
    // printf("FROM CLIENT: %s\n", buffer);

    close(sock_read);
    close(new_sock_read);
    close(sock_write);

    exit(0); // TODO - need to remove once IO is fixed

    if (str[0] == 'E')
    {
        printf("ELECTION MESSAGE DETECTED\n");
        // here I append current ID and send to next
        char id_char[100];
        sprintf(id_char, "%d", id);

        printf("Original String: %s\n", str);
        strncat(str, id_char, str_length(id_char));
        printf("Appended String: %s\n", str);
    }
    else
    {
        printf("COORDINATOR MESSAGE DETECTED\n");
        // here I set coordinator and next
        set_coordinator_next(str);
        printf("COORDINATOR: %d\n", coordinator);
        printf("NEXT: %d\n", next_id);
        // TODO - if coordinator detected, stop listening for other messages
    }
    printf("RAND: %d\n", get_random_in_range(0, 10)); // TODO - needs to move to launcher
    // detect if I am the coordinator
    if (coordinator == id)
    {
        // do execl
        execl("./coordinator", "coordinator", (char *)NULL); // TODO - need err check?
    }

    return 0;
}

int str_length(char str[])
{
    int count;
    for (count = 0; str[count] != '\0'; ++count)
        ;
    return count;
}

void set_coordinator_next(char str[])
{
    int max = id;
    char *token = strtok(str, SEPS);
    int id_ints[PHILOSOPHER_COUNT];
    int i = 0;
    while (token != NULL)
    {
        int token_int = atoi(token);
        id_ints[i] = token_int;
        if (token_int > max)
        {
            max = token_int;
        }
        token = strtok(NULL, SEPS);
        i++;
    }

    for (i = 0; i < PHILOSOPHER_COUNT; ++i)
    {
        if (id_ints[i] == id)
        {
            next_id = ((i == PHILOSOPHER_COUNT - 1) ? id_ints[0] : id_ints[i + 1]);
            break;
        }
    }

    coordinator = max;
}

void setup_server()
{
    printf("SERVERC: Creating socket\n");
    sock_read = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_read < 0)
    {
        printf("SERVERC: Socket opening failed\n");
        exit(EXIT_FAILURE);
    }
    bzero((char *)&serv_adr, sizeof(serv_adr));
    memset(&serv_adr, 0, sizeof(struct sockaddr_in));

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(self_read_port);

    printf("SERVERC: Binding the Socket\n");

    if (bind(sock_read, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0)
    {
        printf("SERVERC: Binding to socket failed\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sock_read, MAX_CLIENT_QUEUE) == -1)
    {
        printf("SERVERC: Binding to socket failed\n");
        exit(EXIT_FAILURE);
    }
    printf("SERVERC: New Server created on IP: %s | Port: %d\n", SERVERIP, self_read_port);

    clientLength = sizeof(client_adr);
    new_sock_read = accept(sock_read, (struct sockaddr *)&client_adr, &clientLength);
    if (new_sock_read < 0)
    {
        printf("SERVERC: Socket accept failed\n");
        exit(EXIT_FAILURE);
    }
}

void setup_client()
{
    sock_write = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_write < 0)
    {
        print_log("ERROR: Error opening the socket.");
        exit(1);
    }

    bzero((char *)&read_adr, sizeof(read_adr));
    memset(&read_adr, 0, sizeof(struct sockaddr_in));

    read_adr.sin_family = AF_INET;
    read_adr.sin_port = htons(next_write_port);
    read_adr.sin_addr.s_addr = inet_addr(SERVERIP);

    print_log("Setting up Connection...\n");
    if (connect(sock_write, (struct sockaddr *)&read_adr, sizeof(read_adr)) < 0)
    {
        print_log("ERROR connecting");
        exit(2);
    }
    sleep(1);
    print_log("Connection Established to Server\n");

    // bzero(buffer, sizeof(buffer));
}