#include "soc.h"
#include "philosopher.h"

// main program
int main(int argc, char *argv[])
{
	/* initiate necessary variables */
	int err;				  // err indicators
	int cSocket;			  // philosopher socket
	struct sockaddr_in sAddr; // server address
	struct sockaddr_in cAddr; // client address
	int cSocLen;			  // client socket length
	char msg[BUFFLEN];		  // message
	int thisID = atoi(argv[1]);
	int ADDPORT = atoi(argv[2]);
	int t0 = atoi(argv[3]);

	/* set up philosopher's socket */
	setAddr(&cAddr, 0, ADDPORT);
	cSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET
	if (cSocket == -1)
	{
		perror("Philosopher: socket creation failed");
		exit(1);
	}
	// connect to coordinator
	err = connect(cSocket, (struct sockaddr *)&cAddr,
				  sizeof(struct sockaddr_in));
	if (err == -1)
	{
		perror("Philosopher: connect failed");
		exit(2);
	}
	sleep(1); // waot for all connect

	// initialize philosopher
	int leftToken = thisID;
	int rightToken = (thisID > 1) ? (thisID - 1) : 5;
	Philosopher p = {thisID, leftToken, rightToken, 0, 0};

	// simulating
	for (;;)
	{
		err = -1;
		if (p.isEat == 0 && p.isThink == 0)
		{
			printf("TS%.0f. P[%d] wants to eat\n",
				   difftime(time(NULL), t0), thisID);
			sprintf(msg, "q%d%d%d", thisID, leftToken, rightToken);
			err = send(cSocket, msg, 8, 0);
		}

		do
		{
			err = recv(cSocket, msg, 8, 0);
		} while (err == -1 && errno == EINTR && msg[1] == 'o');

		// Eat
		printf("TS%.0f. P[%d] picks up %d and %d\n",
			   difftime(time(NULL), t0), thisID, (leftToken == 0) ? 5 : leftToken, (rightToken == 0) ? 5 : rightToken);
		p.isEat = 1;
		srand(time(NULL) + thisID);
		int t = rand() % 3 + 2;
		printf("TS%.0f. P[%d] is eating in %ds\n",
			   difftime(time(NULL), t0), thisID, t);
		sleep(t);
		// Done eat
		printf("TS%.0f. P[%d] releases %d and %d\n",
			   difftime(time(NULL), t0), thisID, leftToken, rightToken);
		p.isEat = 0;
		sprintf(msg, "r%d%d%d", thisID, leftToken, rightToken);
		err = send(cSocket, msg, 8, 0);

		// Think
		p.isThink = 1;
		srand(time(NULL) + thisID);
		t = rand() % 3 + 2;
		printf("TS%.0f. P[%d] is thinking in %ds\n",
			   difftime(time(NULL), t0), thisID, t);
		sleep(t);
		// Done think
		p.isThink = 0;
	}
}
