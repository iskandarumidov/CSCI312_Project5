#include "soc.h"
#include "philosopher.h"

void printAvailable(int c[], int ID);
int packToken(int c[], int thisID);

int main(int argc, char *argv[])
{
	/* init */
	int err;												// Errors handler
	int i;													// Counter
	int thisSocket;											// This philosopher's socket
	int nextSocket;											// Next philosopher's socket
	int prevSocket;											// Previous philosopher's socket
	struct sockaddr_in thisAddr;							// This philosopher's address
	struct sockaddr_in nextAddr;							// Next philosopher's address
	struct sockaddr_in prevAddr;							// Previous philosopher's address
	socklen_t socLen;										// Socket length
	char err_msg[BUFFLEN];									// Error message
	char send_msg[BUFFLEN];									// Sending message
	char recv_msg[BUFFLEN];									// Receiving message
	int thisID = atoi(argv[1]);								// Philosopher's ID
	int nextID = ((thisID + 1) < N + 1) ? (thisID + 1) : 1; // Next philosopher's ID
	int prevID = ((thisID - 1) > 0) ? (thisID - 1) : N;		// Previous philosopher's ID
	int ADDPORT = atoi(argv[2]);
	int status;			 // Child status
	time_t t0 = time(0); // Start time
	int nfd;
	fd_set RFD;
	struct timeval waitTime; // wait time

	/* Set up this philosopher socket */
	thisSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET
	if (thisSocket == -1)
	{
		sprintf(err_msg, "P[%d]: socket creation failed", thisID);
		perror(err_msg);
		exit(1);
	}
	setAddr(&thisAddr, thisID, ADDPORT);
	err = bind(thisSocket, (struct sockaddr *)&thisAddr, sizeof(struct sockaddr_in));
	if (err == -1)
	{
		sprintf(err_msg, "P[%d]: bind address to socket failed", thisID);
		perror(err_msg);
		exit(1);
	}
	err = listen(thisSocket, 5);
	if (err == -1)
	{
		sprintf(err_msg, "P[%d]: listen failed", thisID);
		perror(err_msg);
		exit(1);
	}
	printf("ID[%d]: Port %d is ready\n", thisID, ntohs(thisAddr.sin_port));
	sleep(1); // wait for everybody to be ready

	/* Connect to the next philosopher socket */
	setAddr(&nextAddr, nextID, ADDPORT);
	nextSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET
	if (nextSocket == -1)
	{
		sprintf(err_msg, "P[%d]: socket creation for P[%d] failed", thisID, nextID);
		perror(err_msg);
		exit(2);
	}
	err = connect(nextSocket, (struct sockaddr *)&nextAddr, sizeof(struct sockaddr_in));
	printf("ID[%d]: Connects port %d\n", thisID, ntohs(nextAddr.sin_port));

	/* Accept the previous philosopher's connection */
	prevSocket = accept(thisSocket, (struct sockaddr *)&prevAddr, &socLen);
	sleep(1); // wait for everybody to be ready

	/* At this point, the ring is created */
	///// PHILOSOPHER TOKEN RING //////
	/* INIT */
	int leftToken = thisID;
	int rightToken = (thisID > 1) ? (thisID - 1) : 5;
	Philosopher p = {thisID, leftToken, rightToken, 0, 0};
	int haveToken = (thisID == 1) ? 1 : 0;
	int tokens[N + 1];
	for (i = 1; i < N + 1; i++)
	{
		tokens[i] = i;
	}

	if (haveToken == 1)
	{
		printAvailable(tokens, thisID);
		printf("\nTOKEN RING ALGORITHM\n");
	}
	int isRecv = 0;
	// START SIMULATING
	for (;;)
	{
		if (haveToken == 1)
		{
			// If the philosopher holds a token
			// Update tokens' status
			if (isRecv > 0)
			{
				for (i = 1; i < N + 1; i++)
				{
					tokens[i] = recv_msg[i] - '0';
				}
			}

			if (p.isEat == 0 &&
				p.isThink == 0 &&
				tokens[p.leftToken] == p.leftToken &&
				tokens[p.rightToken] == p.rightToken)
			{
				// And this philosopher can eat
				// Start eating, picking up forks
				printf("TS%.0f. P[%d]: Has token. Ready to eat.\n", difftime(time(0), t0), thisID);
				p.isEat = 1;
				tokens[p.leftToken] = 0;
				tokens[p.rightToken] = 0;
				printf("P[%d]: Pick up %d and %d\n", thisID, p.leftToken, p.rightToken);
				printAvailable(tokens, thisID);

				// Eating
				err = fork();
				if (err == -1)
				{
					sprintf(err_msg, "P[%d]: fork failed", thisID);
					perror(err_msg);
					exit(2);
				}
				else if (err == 0)
				{
					// Child process
					srand(time(NULL) + thisID);
					int r = rand() % 3 + 2;
					printf("TS%.0f. P[%d] is eating in %ds.\n", difftime(time(0), t0), thisID, r);
					sleep(r);
					exit(0);
				}
			}

			else if (p.isThink == 1)
			{
				// Check done thinking
				err = waitpid(-1, &status, WNOHANG);
				if (err > 0)
				{ // Done thinking
					printf("TS%.0f. P[%d] has done thinking.\n", difftime(time(0), t0), thisID);
					p.isThink = 0;
				}
			}

			else if (p.isEat == 1)
			{ // If the philosopher is eating
				// Check done eating
				err = waitpid(-1, &status, WNOHANG);
				if (err > 0)
				{ // Done eating
					printf("TS%.0f. P[%d] has done eating.\n", difftime(time(0), t0), thisID);
					// Release tokens
					printf("P[%d]: Release %d and %d\n", thisID, p.leftToken, p.rightToken);
					tokens[p.leftToken] = p.leftToken;
					tokens[p.rightToken] = p.rightToken;
					printAvailable(tokens, thisID);
					p.isEat = 0;
					p.isThink = 1;
					err = fork();
					if (err == -1)
					{
						sprintf(err_msg, "P[%d]: fork failed", thisID);
						perror(err_msg);
						exit(2);
					}
					else if (err == 0)
					{
						// Child process
						srand(time(NULL) + thisID);
						int r = rand() % 3 + 2;
						printf("TS%.0f. P[%d] is thinking in %ds.\n", difftime(time(0), t0), thisID, r);
						sleep(r);
						exit(0);
					}
				}
			}

			// Send token to next process
			int packedToken = packToken(tokens, thisID);

			sprintf(send_msg, "%d", packedToken);
			printf("PACKED TOKEN: %d\n", packedToken);
			usleep(100);
			err = send(nextSocket, send_msg, 8, 0);
			haveToken = 0;
		}

		/* Select receive wait */
		FD_ZERO(&RFD);
		FD_SET(prevSocket, &RFD);
		nfd = prevSocket + 1;
		waitTime.tv_sec = 0; // select return immediately (no wait).
		waitTime.tv_usec = 10;
		err = select(nfd, &RFD, NULL, NULL, &waitTime);
		if (err > 0)
		{ // If recv_msg came
			isRecv = recv(prevSocket, recv_msg, 8, 0);
			haveToken = 1;
		}
	}
}

void printAvailable(int c[], int ID)
{
	int i;
	printf("P[%d]: Available chopsticks: ", ID);
	for (i = 1; i < N + 1; i++)
	{
		printf("%d ", c[i]);
	}
	printf("\n");
}

int packToken(int c[], int thisID)
{
	int i;
	int result = thisID;
	for (i = 1; i < N + 1; i++)
	{
		result = result * 10 + c[i];
	}
	return result;
}
