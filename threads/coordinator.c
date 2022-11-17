#include "soc.h"

int sock_fd,   // Original Socket for serverC
    newsockfd, // New socket for serverG
    pid;       // PID for Fork
char buf[256];
socklen_t clientLength;
struct sockaddr_in serv_adr, client_adr;
struct sockaddr_in serv_addr; // Server Address for Socket
int err;
int conn_success = -1;

void setup_client()
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // fcntl(sock_fd, F_SETFL, O_NONBLOCK);
    if (sock_fd < 0)
    {
        print_log("ERROR: Error opening the socket.");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));

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

    // bzero(buffer, sizeof(buffer));
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
    // TODO - have to use threads in coordinator too? one for each accept?

    setup_server();

    err = read(newsockfd, buf, sizeof(buf));
    if (err < 0)
    {
        printf("READ ERR\n");
        exit(EXIT_FAILURE);
    }
    printf("FROM PHILOSOPHER: %s\n", buf);
    printf("CHANGING BUF\n");
    buf[0] = 'Y';

    err = write(newsockfd, buf, sizeof(buf));
    if (err < 0)
    {
        printf("WRITE ERR\n");
        exit(EXIT_FAILURE);
    }

    close(sock_fd); // close old socket file descriptor
    close(newsockfd);

    // setup_client();
    // close(sock_fd);

    printf("SERVERC: Terminating serverC\n");

    return EXIT_SUCCESS;
}
