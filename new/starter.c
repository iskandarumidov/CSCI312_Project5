#include "soc.h"
#include "philosopher.h"

int main(int argc, char *argv[])
{
    int read_ports[6] = {31200, 31202, 31204, 31206, 31208, 31210};
    int write_ports[6] = {31201, 31203, 31205, 31207, 31209, 31211};
    // int phil0_read_port = 31200;
    // int phil0_write_port = 31201;
    // int phil1_read_port = 31202;
    // int phil1_write_port = 31203;
    // int phil2_read_port = 31204;
    // int phil2_write_port = 31205;
    // int phil3_read_port = 31206;
    // int phil3_write_port = 31207;
    // int phil4_read_port = 31208;
    // int phil4_write_port = 31209;
    // int phil5_read_port = 31210;
    // int phil5_write_port = 31211;

    print_log("Running %d processes...\n", PHILOSOPHER_COUNT + 1);

    int i = 0;
    int pid;
    int err;
    time_t t;

    for (i = 0; i < PHILOSOPHER_COUNT + 1; i++)
    {
        srand(time(NULL));
        int phil_id = get_random_in_range(1, 1000);
        // pid = fork();
        // if (pid < 0)
        // {
        //     perror("Starter: Fork failed");
        //     exit(EXIT_FAILURE);
        // }
        // else if (pid == 0)
        // {
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
        // err = execl("./exAppend", "exAppend", phil_id_char, read_port_char, write_port_char, next_read_port_char, next_write_port_char, (char *)NULL); // TODO - change program name, err handle
        // execl("./exAppend", "exAppend", "PHIL_ID", "SELF_READ_PORT", "SELF_WRITE_PORT", "NEXT_READ_PORT", "NEXT_WRITE_PORT", (char *)NULL);
        // }
        sleep(1);
        // usleep(1000);
    }

    // for (;;)
    // {
    // }

    return EXIT_SUCCESS;
}
