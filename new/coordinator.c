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
int chopsticks[6] = {-1, 1, 2, 3, 4, 5}; // TODO - change to only 0s and 1s? 0-based?
char incoming_msg[BUFFER_LEN];           // TODO - need to define left and right chopsticks at each philosopher???? What about coord? Should prob decide who gets what chopstick after coord was decided, acc to C_MSG?

#define print_log(f_, ...) printf("[%s] COORDIN: %d ", timestamp(), id), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro, set philosopher ID
void setup_server();
void setup_client();

int main(int argc, char *argv[])
{
    id = atoi(argv[1]);
    self_read_port = atoi(argv[2]);

    print_log("INSIDE COORDINATOR!\n");
    // dequeue();
    setup_server();
    int i;
    for (i = 0; i < 5; i++)
    {
        clientLength = sizeof(client_adr);
        new_sock_read = accept(sock_read, (struct sockaddr *)&client_adr, &clientLength);
        check_syscall_err(new_sock_read, "Socket accept failed");

        err = read(new_sock_read, buffer, sizeof(buffer));
        check_syscall_err(err, "read coord error");
        sprintf(incoming_msg, "%s", buffer);
        print_log("FROM CLIENT: %s\n", incoming_msg);
    }

    close(sock_read);
    close(new_sock_read);

    // incoming_msg

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

    // clientLength = sizeof(client_adr);
    // new_sock_read = accept(sock_read, (struct sockaddr *)&client_adr, &clientLength);
    // check_syscall_err(new_sock_read, "Socket accept failed");
}

void setup_client(int remote_port_to_write)
{
    sock_write = socket(AF_INET, SOCK_STREAM, 0);
    check_syscall_err(sock_write, "Socket opening failed");

    read_adr.sin_family = AF_INET;
    read_adr.sin_port = htons(remote_port_to_write);
    read_adr.sin_addr.s_addr = inet_addr(SERVERIP);

    check_syscall_err(connect(sock_write, (struct sockaddr *)&read_adr, sizeof(read_adr)), "Error connecting");
    sleep(1);
    print_log("Writer created\n");
}