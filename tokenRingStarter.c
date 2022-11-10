#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "soc.h"
#include "philosopher.h"

int main(int argc, char *argv[])
{
	int i;
	int pid;

	printf("\nAbout to run %d processes...\n", N);

	for (i = 1; i < N + 1; i++)
	{
		// printf("\nAbout to run %d processes...\n", N);
		char id[1];
		id[0] = i + '0';

		pid = fork();
		if (pid < 0)
		{ /* fork failed */
			perror("Starter: Fork failed");
			exit(5);
		}
		else if (pid == 0)
		{ /* in child process */
			execl("./tokenRingAlgorithm", "./tokenRingAlgorithm", id, argv[1]);
		}
	}

	for (;;)
	{
	}
}
