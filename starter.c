#include "soc.h"
#define print_log(f_, ...) printf("[%s] STARTER  : ", timestamp()), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro to print STARTER

int main(int argc, char *argv[]) // Startup 6 philosophers. One of them will be coordinator later on
{
    print_log("Running %d processes...\n", PHILOSOPHER_COUNT);

    int i = 0;
    int err;

    for (i = 0; i < PHILOSOPHER_COUNT; i++)
    {
        srand(time(NULL));
        int phil_id = get_random_in_range(1, 1000);
        int pid = fork();
        check_syscall_err(pid, "Fork failed");
        if (pid == 0)
        {
            char phil_id_char[BUFFER_LEN];
            sprintf(phil_id_char, "%d", phil_id);
            char self_read_port_char[BUFFER_LEN]; // Philosopher listens on this port
            sprintf(self_read_port_char, "%d", read_ports[i]);
            char next_write_port_char[BUFFER_LEN]; // Philosopher writes to next philosopher on this port
            sprintf(next_write_port_char, "%d", write_ports[i]);

            print_log("Exec phil ID: %d, Self read port: %d, Next write port: %d\n", phil_id, read_ports[i], write_ports[i]);
            if (i == 0) // First id always starts election
            {
                err = execl("./bin/philosopher", "philosopher", phil_id_char, self_read_port_char, next_write_port_char, "1", (char *)NULL);
            }
            else
            {
                err = execl("./bin/philosopher", "philosopher", phil_id_char, self_read_port_char, next_write_port_char, "0", (char *)NULL);
            }
            check_syscall_err(err, "Execl failed");
        }
        sleep(1);
    }

    return EXIT_SUCCESS;
}
