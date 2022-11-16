#include "soc.h"

int sock_fd;                  // Original Socket in Server
int user_input;               // User Input
struct sockaddr_in serv_addr; // Server Address for Socket
char buffer[256];             // Character Buffer
int err;

void setup_client()
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
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
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        print_log("ERROR connecting");
        exit(2);
    }
    sleep(1);
    print_log("Connection Established to Server\n");

    bzero(buffer, sizeof(buffer));
}

int main(int argc, char *argv[])
{
    setup_client();
    char str[100] = "C;123;45;6;78;1;";
    sprintf(buffer, "%s", str);
    // sprintf(buffer, "%d", user_input);
    err = write(sock_fd, buffer, sizeof(buffer));
    if (err < 0)
    {
        print_log("ERROR!\n");
        exit(EXIT_FAILURE);
    }

    err = read(sock_fd, buffer, sizeof(buffer));
    printf("FROM SERVER: %s\n", buffer);

    print_log("Closing the Connection...\n");
    close(sock_fd);

    return 0;
}
