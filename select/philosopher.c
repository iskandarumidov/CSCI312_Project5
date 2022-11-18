#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define DATA_BUFFER "Mona Lisa was painted by Leonardo da Vinci"

int main()
{
    struct sockaddr_in saddr;
    int fd, ret_val;
    struct hostent *local_host; /* need netdb.h for this */

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1)
    {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Let us initialize the server address structure */
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(31200);
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);

    /* Step2: connect to the TCP server socket */
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1)
    {
        fprintf(stderr, "connect failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    printf("The Socket is now connected\n");

    printf("Let us sleep before we start sending data\n");
    sleep(5);

    /* Next step: send some data */
    ret_val = send(fd, DATA_BUFFER, sizeof(DATA_BUFFER), 0);
    printf("Successfully sent data (len %d bytes): %s\n", ret_val, DATA_BUFFER);

    char buf[5000];
    ret_val = recv(fd, buf, 5000, 0);
    printf("FROM SERVER: %s\n", buf);

    /* Last step: close the socket */
    close(fd);
    return 0;
}