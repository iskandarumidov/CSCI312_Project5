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
int chopsticks[6] = {-1, 1, 1, 1, 1, 1};
char incoming_msg[BUFFER_LEN];
int philosopher_ids[PHILOSOPHER_COUNT];
char philosopher_ids_string[BUFFER_LEN];

#define DATA_BUFFER 5000
#define MAX_CONNECTIONS 1000
int all_connections[MAX_CONNECTIONS];

#define print_log(f_, ...) printf("[%s] COORDIN: %d ", timestamp(), id), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro, set philosopher ID

void extract_ids();
int create_tcp_server_socket();
void print_conn_arr();

int incoming_id;
int incoming_chopstick;
void extract_incoming_id(char str[]);

// TODO - still haven't touched queue and shared var

int main(int argc, char *argv[])
{
    id = atoi(argv[1]);
    self_read_port = atoi(argv[2]);
    sprintf(philosopher_ids_string, "%s", argv[3]);
    print_log("INSIDE COORDINATOR! Coord msg: %s\n", philosopher_ids_string);
    extract_ids();

    fd_set read_fd_set;
    struct sockaddr_in new_addr;
    int server_fd, new_fd, ret_val, i;
    socklen_t addrlen;
    char buf[DATA_BUFFER];
    char buf2[DATA_BUFFER];

    server_fd = create_tcp_server_socket();
    check_syscall_err(server_fd, "Failed to create a server");

    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;

    while (1)
    {
        FD_ZERO(&read_fd_set);
        /* Set the fd_set before passing it to the select call */
        for (i = 0; i < MAX_CONNECTIONS; i++)
        {
            if (all_connections[i] >= 0)
            {
                FD_SET(all_connections[i], &read_fd_set);
            }
        }

        /* Invoke select() and then wait! */
        printf("\nUsing select() to listen for incoming events\n");
        ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

        /* select() woke up. Identify the fd that has events */
        if (ret_val >= 0)
        {
            printf("Select returned with %d\n", ret_val);
            /* Check if the fd with event is the server fd */
            if (FD_ISSET(server_fd, &read_fd_set))
            {
                /* accept the new connection */
                printf("Returned fd is %d (server's fd)\n", server_fd);
                new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
                if (new_fd >= 0)
                {
                    printf("Accepted a new connection with fd: %d\n", new_fd);
                    for (i = 0; i < MAX_CONNECTIONS; i++)
                    {
                        if (all_connections[i] < 0)
                        {
                            all_connections[i] = new_fd;
                            break;
                        }
                    }
                }
                else
                {
                    fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                }
                ret_val--;
                if (!ret_val)
                    continue;
            }

            /* Check if the fd with event is a non-server fd */
            for (i = 1; i < MAX_CONNECTIONS; i++)
            {
                if ((all_connections[i] > 0) && (FD_ISSET(all_connections[i], &read_fd_set)))
                {
                    /* read incoming data */
                    printf("Returned fd is %d [index, i: %d]\n", all_connections[i], i);
                    ret_val = recv(all_connections[i], buf, DATA_BUFFER, 0);
                    if (ret_val == 0)
                    {
                        printf("Closing connection for fd:%d\n", all_connections[i]);
                        close(all_connections[i]);
                        all_connections[i] = -1; /* Connection is now closed */
                    }
                    if (ret_val > 0)
                    {
                        // printf("Received data (len %d bytes, fd: %d): %s\n", ret_val, all_connections[i], buf);
                        sprintf(buf2, "%s", buf);
                        extract_incoming_id(buf2);
                        if (buf[0] == 'Q')
                        {
                            // if(queue_is_empty()){

                            // }
                            int wr_len = send(all_connections[i], "1", DATA_BUFFER, 0);
                            print_conn_arr();
                        }
                        else
                        {
                        }
                    }
                    if (ret_val == -1)
                    {
                        printf("recv() failed for fd: %d [%s]\n", all_connections[i], strerror(errno));
                        break;
                    }
                }
                ret_val--;
                if (!ret_val)
                    continue;
            }
        }
    }

    /* Last step: Close all the sockets */
    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (all_connections[i] > 0)
        {
            close(all_connections[i]);
        }
    }

    print_log("Terminating...\n");
    return EXIT_SUCCESS;
}

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
}

void extract_incoming_id(char str[])
{
    char *token = strtok(str, SEPARATORS);
    incoming_id = atoi(token);
    token = strtok(NULL, SEPARATORS);
    incoming_chopstick = atoi(token);
}

int create_tcp_server_socket()
{
    struct sockaddr_in saddr;
    int fd, ret_val;

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    check_syscall_err(fd, "Socket opening failed");

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(self_read_port);
    saddr.sin_addr.s_addr = INADDR_ANY;

    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    check_syscall_err(ret_val, "Bind failed");

    ret_val = listen(fd, 5);
    check_syscall_err(ret_val, "Listen failed");
    return fd;
}

void print_conn_arr()
{
    int i;
    for (i = 0; i < 20; i++)
    {
        printf("%d ", all_connections[i]);
    }
    printf("\n");
}