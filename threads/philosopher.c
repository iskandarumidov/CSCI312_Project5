#include "soc.h"
#include <pthread.h>

#define NULL 0

int sock_fd;                  // Original Socket in Server
int newsockfd;                // New socket for serverG
int user_input;               // User Input
struct sockaddr_in serv_addr; // Server Address for Socket
socklen_t clientLength;
struct sockaddr_in serv_adr, client_adr;
char buffer[BUFFER_LEN]; // Character Buffer
int err;
int conn_success = -1;

// pthread_t tid[2];
pthread_t thread;
int isThinking = 0;
pthread_mutex_t lock;

void *trythis(void *arg)
{
    pthread_mutex_lock(&lock);
    printf("Job has started\n");

    err = read(sock_fd, buffer, sizeof(buffer));
    printf("FROM COORDINATOR (IN THREAD): %s\n", buffer);

    printf("Job has finished\n");
    pthread_mutex_unlock(&lock);

    return NULL;
}

void setup_client()
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // fcntl(sock_fd, F_SETFL, O_NONBLOCK);
    if (sock_fd < 0)
    {
        print_log("ERROR: Error opening the socket.");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVERPORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVERIP);

    print_log("Setting up Connection...\n");

    int resp = connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (resp < 0)
    {
        print_log("ERROR connecting\n");
        conn_success = -1;
        return;
        // exit(2);
    }
    // sleep(1);
    print_log("Connection Established to Server\n");
    conn_success = 1;
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
    bzero((char *)&serv_adr, sizeof(serv_adr));
    memset(&serv_adr, 0, sizeof(struct sockaddr_in));

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

    clientLength = sizeof(client_adr);
    newsockfd = accept(sock_fd, (struct sockaddr *)&client_adr, &clientLength);
    if (newsockfd < 0)
    {
        printf("SERVERC: Socket accept failed\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init has failed\n");
        return 1;
    }
    setup_client();
    sleep(1);

    while (conn_success < 0)
    {
        setup_client();
        sleep(1);
    }
    char str[100] = "R;1;";
    sprintf(buffer, "%s", str);
    // sprintf(buffer, "%d", user_input);
    err = write(sock_fd, buffer, sizeof(buffer));
    if (err < 0)
    {
        print_log("ERROR!\n");
        exit(EXIT_FAILURE);
    }

    // err = read(sock_fd, buffer, sizeof(buffer));
    // printf("FROM COORDINATOR: %s\n", buffer);

    err = pthread_create(&thread, NULL, &trythis, NULL);
    if (err != 0)
        printf("\nThread can't be created :[%s]", strerror(err));
    pthread_detach(thread);

    usleep(5000000); // sleep 1 sec
    printf("FROM COORDINATOR (MAIN THREAD): %s\n", buffer);

    print_log("Closing the Connection...\n");
    close(sock_fd);

    // setup_server();
    // err = read(newsockfd, buffer, sizeof(buffer));
    // printf("FROM COORDINATOR: %s\n", buffer);
    // close(sock_fd);
    // close(newsockfd);
    pthread_mutex_destroy(&lock);
    return 0;
}

// TODO - on coordinator side - close listener and open sender.
// TODO - on philosopher side - if failed, try again 10 times.
