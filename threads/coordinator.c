#include "soc.h"
#include <pthread.h>

int sock_fd,   // Original Socket for serverC
    newsockfd, // New socket for serverG
    pid;       // PID for Fork
char buffer[256];
socklen_t clientLength;
struct sockaddr_in serv_adr, client_adr;
struct sockaddr_in serv_addr; // Server Address for Socket
int err;
int conn_success = -1;

pthread_t thread;
pthread_mutex_t lock;

char philosopher_ids_string[BUFFER_LEN] = "C;123;45;6;78;1;17;";
int philosopher_ids[PHILOSOPHER_COUNT - 1];
int id = 123;

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
    for (int i = 0; i < PHILOSOPHER_COUNT; i++)
    {
        printf("Philosopher ID: %d\n", philosopher_ids[i]);
    }
}

void *trythis(void *arg)
{
    unsigned int newsocket_in_thread = *((unsigned int *)arg);
    pthread_mutex_lock(&lock);
    printf("Job has started\n");

    err = read(newsocket_in_thread, buffer, sizeof(buffer));
    if (err < 0)
    {
        printf("READ ERR\n");
        exit(EXIT_FAILURE);
    }
    printf("FROM PHILOSOPHER (IN THREAD): %s\n", buffer);

    printf("Job has finished\n");
    pthread_mutex_unlock(&lock);

    return NULL;
}

void setup_server()
{
    printf("SERVERC: Creating socket\n");
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // fcntl(sock_fd, F_SETFL, O_NONBLOCK);
    if (sock_fd < 0)
    {
        printf("SERVERC: Socket opening failed\n");
        exit(EXIT_FAILURE);
    }
    // bzero((char *)&serv_adr, sizeof(serv_adr));
    // memset(&serv_adr, 0, sizeof(struct sockaddr_in));

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(SERVERPORT);

    const int enable = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed\n");

    printf("SERVERC: Binding the Socket\n");

    if (bind(sock_fd, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0)
    {
        printf("SERVERC: Binding to socket failed\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sock_fd, MAX_CLIENT_QUEUE) == -1)
    {
        printf("SERVERC: Binding to socket failed\n");
        exit(EXIT_FAILURE);
    }
    printf("SERVERC: New Server created on IP: %s | Port: %d\n", SERVERIP, SERVERPORT);

    // clientLength = sizeof(client_adr);
    // newsockfd = accept(sock_fd, (struct sockaddr *)&client_adr, &clientLength);
    // if (newsockfd < 0)
    // {
    //     printf("SERVERC: Socket accept failed\n");
    //     exit(EXIT_FAILURE);
    // }
}

int main(int argc, char *argv[])
{
    extract_ids();
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("mutex init has failed\n");
        return 1;
    }

    setup_server();

    while (1)
    {

        clientLength = sizeof(client_adr);
        newsockfd = accept(sock_fd, (struct sockaddr *)&client_adr, &clientLength);
        if (newsockfd < 0)
        {
            printf("SERVERC: Socket accept failed\n");
            exit(EXIT_FAILURE);
        }

        err = pthread_create(&thread, NULL, &trythis, (void *)&newsockfd);
        if (err != 0)
            printf("\nThread can't be created :[%s]", strerror(err));
        pthread_detach(thread);
        printf("FROM PHILOSOPHER (IN MAIN): %s\n", buffer);
        printf("CHANGING BUF\n");
        buffer[0] = 'Y';

        err = write(newsockfd, buffer, sizeof(buffer));
        if (err < 0)
        {
            printf("WRITE ERR\n");
            exit(EXIT_FAILURE);
        }
    }

    close(sock_fd);
    close(newsockfd);
    pthread_mutex_destroy(&lock);
    printf("SERVERC: Terminating serverC\n");

    return EXIT_SUCCESS;
}
