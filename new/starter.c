#include "soc.h"
#define print_log(f_, ...) printf("[%s] STARTER: ", timestamp()), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro to STARTER
int main(int argc, char *argv[])
{
    // // int read_ports[6] = {31200, 31201, 31202, 31203, 31204, 31205};
    // // int write_ports[6] = {31201, 31202, 31203, 31204, 31205, 31200};
    // int read_ports[6] = {31200, 31201};
    // int write_ports[6] = {31201, 31200};

    print_log("Running %d processes...\n", PHILOSOPHER_COUNT + 1);

    int i = 0;
    int pid;
    int err;
    time_t t;

    for (i = 0; i < PHILOSOPHER_COUNT + 1; i++)
    // for (i = 0; i < 2; i++)
    {
        srand(time(NULL));
        int phil_id = get_random_in_range(1, 1000);
        pid = fork();
        if (pid < 0)
        {
            print_log("Fork failed\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            char phil_id_char[BUFFER_LEN];
            sprintf(phil_id_char, "%d", phil_id);
            char self_read_port_char[BUFFER_LEN];
            sprintf(self_read_port_char, "%d", read_ports[i]);
            char next_write_port_char[BUFFER_LEN];
            sprintf(next_write_port_char, "%d", write_ports[i]);

            char should_start_election_char[BUFFER_LEN];
            sprintf(should_start_election_char, "%d", 1);
            char should_not_start_election_char[BUFFER_LEN];
            sprintf(should_not_start_election_char, "%d", 0);
            print_log("Exec phil ID: %d, Self read port: %d, Next write port: %d\n", phil_id, read_ports[i], write_ports[i]);
            if (i == 0) // This one should start election
            {
                err = execl("./philosopher", "philosopher", phil_id_char, self_read_port_char, next_write_port_char, should_start_election_char, (char *)NULL);
            }
            else
            {
                err = execl("./philosopher", "philosopher", phil_id_char, self_read_port_char, next_write_port_char, should_not_start_election_char, (char *)NULL);
            }

            if (err == -1)
            {
                print_log("Execl failed\n");
                exit(EXIT_FAILURE);
            }
        }
        sleep(1);
    }

    for (;;)
    {
    }

    return EXIT_SUCCESS;
}
