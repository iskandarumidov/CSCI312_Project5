#include "soc.h"
#include "queue.c"

int id = -1;
int self_read_port = -1;

int main(int argc, char *argv[])
{
    id = atoi(argv[1]);
    self_read_port = atoi(argv[2]);

    printf("NOW I AM COORDINATOR!\n");
    dequeue();

    return 0;
}
