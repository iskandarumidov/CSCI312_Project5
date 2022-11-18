#include "soc.h"
#include "queue.c"

int id = -1;
int self_read_port = -1;

int sock_read;
int new_sock_read;
int sock_write;
char buffer[BUFFER_LEN];
socklen_t clientLength;
struct sockaddr_in serv_adr, client_adr;
struct sockaddr_in read_adr;
int err;
int chopsticks[6] = {-1, 1, 1, 1, 1, 1};
char incoming_msg[BUFFER_LEN];
int philosopher_ids[PHILOSOPHER_COUNT];
char philosopher_ids_string[BUFFER_LEN];

#define print_log(f_, ...) printf("[%s] COORDIN: %d ", timestamp(), id), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro, set philosopher ID
void setup_server();
void extract_ids();
void *process(void *arg);

pthread_t thread;
pthread_mutex_t lock;

int main(int argc, char *argv[])
{
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        print_log("mutex init has failed\n");
        return EXIT_FAILURE;
    }

    id = atoi(argv[1]);
    self_read_port = atoi(argv[2]);
    sprintf(philosopher_ids_string, "%s", argv[3]);

    print_log("INSIDE COORDINATOR! Coord msg: %s\n", philosopher_ids_string);
    extract_ids();

    // dequeue();
    setup_server();
    // while (1)
    int i = 0;
    for (i = 0; i < PHILOSOPHER_COUNT - 1; i++)
    // while (1)
    {
        print_log("INCOMING CONNETION #%d\n", i);
        clientLength = sizeof(client_adr);
        new_sock_read = accept(sock_read, (struct sockaddr *)&client_adr, &clientLength);
        check_syscall_err(new_sock_read, "Socket accept failed");

        err = pthread_create(&thread, NULL, &process, (void *)&new_sock_read);
        if (err != 0)
            printf("\nThread can't be created :[%s]", strerror(err));
        pthread_detach(thread);

        // err = read(new_sock_read, buffer, sizeof(buffer));
        // check_syscall_err(err, "read coord error");
        // sprintf(incoming_msg, "%s", buffer);
        // print_log("FROM CLIENT: %s\n", incoming_msg);

        // err = write(new_sock_read, "1", sizeof("1"));
        // check_syscall_err(err, "read coord error");
    }
    while (1)
    {
    }

    close(sock_read);
    close(new_sock_read);
    pthread_mutex_destroy(&lock);
    print_log("Terminating...\n");

    return EXIT_SUCCESS;
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
    print_log("Node listening on port: %d\n", self_read_port);
}

void extract_ids()
{
    char *token = strtok(philosopher_ids_string, SEPARATORS);
    int i = 0;
    while (token != NULL)
    {
        philosopher_ids[i] = (id == atoi(token) ? -1 : atoi(token));
        token = strtok(NULL, SEPARATORS);
        i++;
    }
}

void *process(void *arg)
{
    unsigned int newsocket_in_thread = *((unsigned int *)arg);
    while (1)
    {
        // pthread_mutex_lock(&lock);
        err = read(newsocket_in_thread, buffer, sizeof(buffer));
        if (err == 0)
        {
            print_log("Client disconnected\n");
            break;
            // exit(EXIT_SUCCESS);
        }
        check_syscall_err(err, "thread read failed");
        print_log("FROM PHILOSOPHER (IN THREAD): %s\n", buffer);
        // pthread_mutex_unlock(&lock);

        err = write(new_sock_read, "1", sizeof("1"));
        check_syscall_err(err, "write coord error");
    }

    // pthread_mutex_lock(&lock);
    // print_log("Job has started\n");

    // printf("Job has finished\n");
    // pthread_mutex_unlock(&lock);
    return NULL;
}