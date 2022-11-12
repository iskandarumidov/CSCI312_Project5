// ServerC is responsible for receiving a request from client and forking a child, serverG,
// to handle the tic-tac-toe game with the client

#include "soc.h"

int main(int argc, char *argv[])
{
    int sock_fd; // Original Socket in Server
    // int user_input;               // User Input
    struct sockaddr_in serv_addr; // Server Address for Socket
    char buf[256];                // Character Buffer
    int err;

    // srand(time(NULL));

    // Setting the Socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        printf("ERROR: Error opening the socket.");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVERPORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVERIP);

    // Setting the Connection
    printf("Setting up Connection...\n");
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting");
        exit(2);
    }
    sleep(1);
    printf("Connection Established to Server\n");
    // int user_input = 999;
    // sprintf(buf, "%d", user_input);
    // err = write(sock_fd, buf, sizeof(buf));
    err = write(sock_fd, "ASDF", sizeof("ASDF") + 1);

    close(sock_fd); // close old socket file descriptor

    printf("SERVERC: Terminating serverC\n");

    return EXIT_SUCCESS;
}
