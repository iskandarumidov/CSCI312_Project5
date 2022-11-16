
#include <stdbool.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/select.h>
// #include <linux/in.h>

#define NUMPHILOSOPHERS 5

int chopsticks[NUMPHILOSOPHERS]; // 0 if available, 1 if used
// int queue[NUMPHILOSOPHERS];
int nextSpot = 0;
int queueSize = 0;
pthread_mutex_t chopstickLock;
// pthread_mutex_t enterQueueLock;
// pthread_mutex_t removeFromQueueLock;

typedef struct
{
    int sock;
    struct sockaddr address;
    int addr_len;
} connection_t;

int valread;
void *process(void *ptr);
bool canEat(int Philosopher);
void enterQueue(int Philosopher);
void removeFromQueue(int position);
void doneEating(int Philosopher);
void createPhilosophers(int port, int i);

void *process(void *ptr)
{ // This is our server C
    char *buffer;
    char *buffer2;
    int len;
    connection_t *conn;
    long addr = 0;
    int err;
    if (!ptr)
        pthread_exit(0);
    conn = (connection_t *)ptr;
    int p_number;

    int i, j;

    char *Eat = "Can Eat \n";
    char *ok = "Ok \n";

    for (i = 0; i < 3; i++)
    {
        /* read length of message */
        read(conn->sock, &len, sizeof(int));
        if (len > 0)
        {
            addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
            buffer = (char *)malloc((len + 1) * sizeof(char));
            buffer[len] = 0;

            /* read message */
            read(conn->sock, buffer, len);
            p_number = buffer[0] - '0';
            printf("Controller: P%d is requesting to eat\n", p_number);
            if (canEat(p_number))
            {
                // Send them an ok to eat
                printf("Controller: Telling P%d to eat\n", p_number);
                len = strlen(Eat);
                write(conn->sock, &len, sizeof(int));
                write(conn->sock, Eat, len);
            }
            else
            {
                printf("Controller: P%d is in wait queue\n", p_number);
                while (!canEat(p_number))
                {
                    // sleep(1);
                }
                // Send them an ok to eat
                printf("Controller: Telling P%d to eat\n", p_number);
                len = strlen(Eat);
                write(conn->sock, &len, sizeof(int));
                write(conn->sock, Eat, len);
            }
            free(buffer);
        }

        /* read length of message */
        read(conn->sock, &len, sizeof(int));
        if (len > 0)
        {
            addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
            buffer = (char *)malloc((len + 1) * sizeof(char));
            buffer[len] = 0;

            /* read message */
            read(conn->sock, buffer, len);
            p_number = buffer[0] - '0';
            printf("Controller: P%d is done eating\n", p_number);
            doneEating(p_number);
            // Send them acknowledgment
            len = strlen(ok);
            write(conn->sock, &len, sizeof(int));
            write(conn->sock, ok, len);
            free(buffer);
        }
    }
}

// critical process
bool canEat(int Philosopher)
{
    pthread_mutex_lock(&chopstickLock);
    int i;
    // for(i=0; i<NUMPHILOSOPHERS; i++){
    // printf(" %d", chopsticks[i]);
    // }
    // printf("\n");
    if ((chopsticks[Philosopher] == 0) && (chopsticks[(Philosopher + 1) % NUMPHILOSOPHERS] == 0))
    { // Right chopstick and left available
        chopsticks[Philosopher] = 1;
        chopsticks[(Philosopher + 1) % 5] = 1;
        pthread_mutex_unlock(&chopstickLock);
        return true;
    }
    else
    {
        pthread_mutex_unlock(&chopstickLock);
        return false;
    }
}
// critical process
void doneEating(int Philosopher)
{
    pthread_mutex_lock(&chopstickLock);

    // int i;
    chopsticks[Philosopher] = 0;
    chopsticks[(Philosopher + 1) % NUMPHILOSOPHERS] = 0;
    // for(i=0; i<NUMPHILOSOPHERS; i++){
    // printf(" %d", chopsticks[i]);
    // }
    // printf("\n");
    pthread_mutex_unlock(&chopstickLock);
}

void createPhilosophers(int port, int i)
{
    if (fork() == 0)
    {
        char toSend[15];
        sprintf(toSend, "./philosopher localhost %d %d", port, i);
        // create a process for a philosopher, and connect the sockets
        printf("Coordinator: creating philosopher %d\n", i);

        system(toSend);
    }
    else if (i > 0)
        createPhilosophers(port, i - 1);
}

int main(int argc, char **argv)
{
    int sock = -1;
    struct sockaddr_in address;
    int port;
    connection_t *connection;
    pthread_t thread;
    int err;
    /* check for command line arguments */
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        return -1;
    }

    /* obtain port number */
    if (sscanf(argv[1], "%d", &port) <= 0)
    {
        fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
        return -2;
    }

    /* create socket */
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock <= 0)
    {
        fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
        return -3;
    }

    /* bind socket to port */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
    {
        fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
        return -4;
    }

    /* listen on port */
    if (listen(sock, 5) < 0)
    {
        fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
        return -5;
    }

    printf("%s: ready and listening\n", argv[0]);

    createPhilosophers(port, 4);

    while (1)
    {
        /* accept incoming connections */
        connection = (connection_t *)malloc(sizeof(connection_t));
        connection->sock = accept(sock, &connection->address, &connection->addr_len);
        if (connection->sock <= 0)
        {
            free(connection);
        }
        else
        {
            /* start a new thread but do not wait for it */
            pthread_create(&thread, 0, process, (void *)connection);
            pthread_detach(thread);
        }
    }

    return 0;
}
