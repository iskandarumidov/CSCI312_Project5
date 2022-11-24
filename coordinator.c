#include "soc.h"

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
int fd_to_chopstick[9][2] = {
    {-2, -2},
    {-2, -2},
    {-2, -2},
    {-2, -2},
    {-1, -1},
    {-1, -1},
    {-1, -1},
    {-1, -1},
    {-1, -1},
};

// 3 4 5 6 7 8

#define DATA_BUFFER 5000
#define MAX_CONNECTIONS 1000
int all_connections[MAX_CONNECTIONS];

#define print_log(f_, ...) printf("[%s] COORDINAT:", timestamp()), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro, set philosopher ID

#include "queue.c"

void extract_ids();
int create_tcp_server_socket();

int incoming_id;
int incoming_chopstick;
void extract_incoming_id(char str[]);

int main(int argc, char *argv[])
{
    id = atoi(argv[1]);
    self_read_port = atoi(argv[2]);
    sprintf(philosopher_ids_string, "%s", argv[3]);
    print_log("Coordinator started. Coord ID: %d\n", id);
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
        ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

        /* select() woke up. Identify the fd that has events */
        if (ret_val >= 0)
        {
            /* Check if the fd with event is the server fd */
            if (FD_ISSET(server_fd, &read_fd_set))
            {
                /* accept the new connection */
                new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
                if (new_fd >= 0)
                {
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
                    ret_val = recv(all_connections[i], buf, DATA_BUFFER, 0);
                    if (ret_val == 0)
                    {
                        close(all_connections[i]);
                        all_connections[i] = -1; /* Connection is now closed */
                    }
                    if (ret_val > 0)
                    {
                        sprintf(buf2, "%s", buf);
                        if (buf[0] == 'I')
                        {
                            print_log("RECEIVED I MESSAGE: %s\n", buf);
                            char *token = strtok(buf2, SEPARATORS);
                            incoming_id = atoi(token);
                            token = strtok(NULL, SEPARATORS);
                            fd_to_chopstick[all_connections[i]][0] = atoi(token);
                            token = strtok(NULL, SEPARATORS);
                            fd_to_chopstick[all_connections[i]][1] = atoi(token);
                            // print_log("FD TO CHOP: l - %d, r - %d\n", fd_to_chopstick[all_connections[i]][0], fd_to_chopstick[all_connections[i]][1]);
                        }
                        else if (buf[0] == 'Q')
                        {
                            print_log("RECEIVED Q MESSAGE: %s\n", buf);
                            char *token = strtok(buf2, SEPARATORS);
                            incoming_id = atoi(token);
                            token = strtok(NULL, SEPARATORS);
                            incoming_chopstick = atoi(token);

                            if (chopsticks[incoming_chopstick] == 1)
                            {
                                chopsticks[incoming_chopstick] = 0;
                                print_log("GIVING CHOPSTICK %d TO PHILOSOPHER %d\n", incoming_chopstick, incoming_id);
                                int wr_len = send(all_connections[i], "Y", 1, 0);
                                check_syscall_err(wr_len, "Failed to send 1 to philosopher");
                            }
                            else
                            {
                                print_log("CHOPSTICK %d IS BUSY, ADDING PHILOSOPHER %d TO QUEUE\n", incoming_chopstick, incoming_id);
                                int wr_len = send(all_connections[i], "N", 1, 0);
                                check_syscall_err(wr_len, "Failed to send 0 to philosopher");
                                enqueue(all_connections[i]);
                                display_queue();
                            }
                            print_log("CHOPSTICKS: %d %d %d %d %d\n", chopsticks[1], chopsticks[2], chopsticks[3], chopsticks[4], chopsticks[5]);
                            display_queue();
                        }
                        else if (buf[0] == 'R')
                        {
                            print_log("RECEIVED R MESSAGE: %s\n", buf);
                            char *token = strtok(buf2, SEPARATORS);
                            incoming_id = atoi(token);
                            token = strtok(NULL, SEPARATORS);
                            incoming_chopstick = atoi(token);

                            chopsticks[incoming_chopstick] = 1;

                            // Send X after one chopstick was released
                            int fd_to_run = peek();
                            if (fd_to_run > 0)
                            {
                                int my_left_chopstick = fd_to_chopstick[fd_to_run][0];
                                int my_right_chopstick = fd_to_chopstick[fd_to_run][1];
                                print_log("CHECK IF BOTH CHOPSTICKS ARE READY FOR FD: %d, left - %d, right - %d\n", fd_to_run, my_left_chopstick, my_right_chopstick);
                                if (chopsticks[my_left_chopstick] == 1 && chopsticks[my_right_chopstick] == 1)
                                {
                                    chopsticks[my_left_chopstick] = 0;
                                    chopsticks[my_right_chopstick] = 0;
                                    print_log("SENDING X - GIVING CHOPSTICKS %d AND %d TO PHILOSOPHER WITH FD %d\n", my_left_chopstick, my_right_chopstick, fd_to_run);

                                    char msg[BUFFER_LEN];
                                    err = send(fd_to_run, "X", 1, 0);
                                    check_syscall_err(err, "Failed to send X to philosopher");
                                    dequeue();
                                    display_queue();
                                }
                                else
                                {
                                    print_log("BOTH CHOPSTICKS NOT READY\n");
                                }
                            }

                            print_log("CHOPSTICKS: %d %d %d %d %d\n", chopsticks[1], chopsticks[2], chopsticks[3], chopsticks[4], chopsticks[5]);
                            display_queue();
                        }
                        else if (buf[0] == 'W')
                        {
                            print_log("RECEIVED W MESSAGE: %s\n", buf);
                            char *token = strtok(buf2, SEPARATORS);
                            incoming_id = atoi(token);
                            token = strtok(NULL, SEPARATORS);
                            int incoming_left_chopstick = atoi(token);
                            token = strtok(NULL, SEPARATORS);
                            int incoming_right_chopstick = atoi(token);

                            print_log("CHOPSTICKS BEFORE W: %d %d %d %d %d\n", chopsticks[1], chopsticks[2], chopsticks[3], chopsticks[4], chopsticks[5]);
                            chopsticks[incoming_left_chopstick] = 1;
                            chopsticks[incoming_right_chopstick] = 1;
                            print_log("CHOPSTICKS AFTER W: %d %d %d %d %d\n", chopsticks[1], chopsticks[2], chopsticks[3], chopsticks[4], chopsticks[5]);

                            // Send X after both chopsticks were released
                            int fd_to_run = peek();
                            if (fd_to_run > 0)
                            {
                                int my_left_chopstick = fd_to_chopstick[fd_to_run][0];
                                int my_right_chopstick = fd_to_chopstick[fd_to_run][1];
                                print_log("CHECK IF BOTH CHOPSTICKS ARE READY FOR FD: %d, left - %d, right - %d\n", fd_to_run, my_left_chopstick, my_right_chopstick);
                                if (chopsticks[my_left_chopstick] == 1 && chopsticks[my_right_chopstick] == 1)
                                {
                                    chopsticks[my_left_chopstick] = 0;
                                    chopsticks[my_right_chopstick] = 0;
                                    print_log("SENDING X - GIVING CHOPSTICKS %d AND %d TO PHILOSOPHER %d\n", my_left_chopstick, my_right_chopstick, fd_to_run);

                                    char msg[BUFFER_LEN];
                                    err = send(fd_to_run, "X", 1, 0);
                                    check_syscall_err(err, "Failed to send X to philosopher");
                                    dequeue();
                                    display_queue();
                                }
                                else
                                {
                                    print_log("BOTH CHOPSTICKS NOT READY\n");
                                }
                            }

                            print_log("CHOPSTICKS: %d %d %d %d %d\n", chopsticks[1], chopsticks[2], chopsticks[3], chopsticks[4], chopsticks[5]);
                            display_queue();
                        }
                    }
                    if (ret_val == -1)
                    {
                        print_log("recv() failed for fd: %d\n", all_connections[i]);
                        break;
                    }
                }
                ret_val--;
                if (!ret_val)
                    continue;
            }
        }
    }

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
