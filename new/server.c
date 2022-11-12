// ServerC is responsible for receiving a request from client and forking a child, serverG,
// to handle the tic-tac-toe game with the client

#include "soc.h"

int sock_fd,   // Original Socket for serverC
    newsockfd, // New socket for serverG
    pid;       // PID for Fork
char buf[256];
socklen_t clientLength;
struct sockaddr_in serv_adr, client_adr;
int err;

void setup_server()
{
    printf("SERVERC: Creating socket\n");
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    setup_server();

    err = read(newsockfd, buf, sizeof(buf));
    printf("FROM CLIENT: %s\n", buf);

    err = write(sock_fd, buf, sizeof(buf));

    close(sock_fd); // close old socket file descriptor
    close(newsockfd);

    printf("SERVERC: Terminating serverC\n");

    return EXIT_SUCCESS;
}
