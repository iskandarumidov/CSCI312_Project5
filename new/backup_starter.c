#include "soc.h"

int main(int argc, char *argv[])
{
    int read_ports[6] = {31200, 31202, 31204, 31206, 31208, 31210};
    int write_ports[6] = {31201, 31203, 31205, 31207, 31209, 31211};

    print_log("Running %d processes...\n", PHILOSOPHER_COUNT + 1);

    int i = 0;
    int pid;
    int err;
    time_t t;

    for (i = 0; i < PHILOSOPHER_COUNT + 1; i++)
    {
        srand(time(NULL));
        int phil_id = get_random_in_range(1, 1000);
        pid = fork();
        if (pid < 0)
        {
            perror("Starter: Fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            char phil_id_char[BUFFER_LEN];
            sprintf(phil_id_char, "%d", phil_id);
            char read_port_char[BUFFER_LEN];
            sprintf(read_port_char, "%d", read_ports[i]);
            char write_port_char[BUFFER_LEN];
            sprintf(write_port_char, "%d", write_ports[i]);

            int next = ((i == PHILOSOPHER_COUNT) ? 0 : (i + 1));
            char next_read_port_char[BUFFER_LEN];
            sprintf(next_read_port_char, "%d", read_ports[next]);
            char next_write_port_char[BUFFER_LEN];
            sprintf(next_write_port_char, "%d", write_ports[next]);
            print_log("Exec: Phil ID: %d, Read port: %d, Write port: %d, Next read port: %d, Next write port: %d\n", phil_id, read_ports[i], write_ports[i], read_ports[next], write_ports[next]);
            err = execl("./exAppend", "exAppend", phil_id_char, read_port_char, write_port_char, next_read_port_char, next_write_port_char, (char *)NULL);
            // err = execl("./exAppend", "exAppend", phil_id_char, read_port_char, write_port_char, next_read_port_char, next_write_port_char, (char *)NULL);
            // execl("./exAppend", "exAppend", "PHIL_ID", "SELF_READ_PORT", "SELF_WRITE_PORT", "NEXT_READ_PORT", "NEXT_WRITE_PORT", (char *)NULL);
        }
        sleep(1);
    }

    for (;;)
    {
    }

    return EXIT_SUCCESS;
}
