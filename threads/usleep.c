#include "soc.h"

int main()
{
    // 1 second = 1000000 microseconds
    // sigset_t sigset;
    // sigset_t oldset;
    // sigfillset(&sigset);
    // pthread_sigmask(SIG_BLOCK, &sigset, &oldset);
    printf("BEFORE USLEEP!\n");
    usleep(1000000);
    // pthread_sigmask(SIG_SETMASK, &oldset, NULL);
    printf("AFTER USLEEP!\n");
}
