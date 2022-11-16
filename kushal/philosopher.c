
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

#define NUMPHILOSOPHERS 5

int port;
int i, j;
int sock = -1;
struct sockaddr_in address;
struct hostent *host;
int len;
char buffer[1024] = {0};
char gameBuffer[1024] = {0};
char wonBuffer[1024] = {0};
char looper = 'E';

int chopsticks[NUMPHILOSOPHERS]; // 0 if available, 1 if used

void eating(char p)
{
    chopsticks[p - '0'] = 1;
    chopsticks[((p - '0') + 1) % NUMPHILOSOPHERS] = 1;
    printf("Philosopher %c :eating\n", p);
    int eat_time = rand() % 3;
    sleep(eat_time);
    chopsticks[p - '0'] = 0;
    chopsticks[((p - '0') + 1) % NUMPHILOSOPHERS] = 0;
}

void thinking(char p)
{
    printf("Philosopher %c :thinking\n", p);
    int think_time = rand() % 3;
    sleep(think_time);
}

int main(int argc, char **argv)
{

    /* checking commandline parameter */
    if (argc != 4)
    {
        printf("usage: %s hostname port text\n", argv[0]);
        return -1;
    }

    /* obtain port number */
    if (sscanf(argv[2], "%d", &port) <= 0)
    {
        fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
        return -2;
    }

    /* obtain process number */
    char number = *argv[3];
    // printf("In philosopher %c\n", number);
    int p_number = number - '0';

    /* create socket */
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock <= 0)
    {
        fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
        return -3;
    }

    /* connect to server */
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    host = gethostbyname(argv[1]);
    if (!host)
    {
        fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
        return -4;
    }
    memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
    if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
    {
        fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
        return -5;
    }

    /* send text to server */
    // len = strlen(argv[3]);
    // write(sock, &len, sizeof(int));
    // write(sock, argv[3], len);

    char requestEat[1];
    strcat(requestEat, argv[3]);
    // strcat(requestEat, i);

    for (i = 0; i < 3; i++)
    {
        // request to eat
        /* send text to server */
        len = strlen(argv[3]);
        write(sock, &len, sizeof(int));
        write(sock, argv[3], len);
        read(sock, buffer, 1024);
        eating(number);

        // Notify controller that done eating
        char doneEat[1];
        strcat(doneEat, argv[3]);

        // done eating
        /* send text to server */
        len = strlen(argv[3]);
        write(sock, &len, sizeof(int));
        write(sock, argv[3], len);
        read(sock, buffer, 1024);

        thinking(number);
    }

    printf("Philosopher: P%c is closing. Goodbye.\n", number);

    /* close socket */
    close(sock);

    return 0;
}
