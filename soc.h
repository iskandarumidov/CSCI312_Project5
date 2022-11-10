#ifndef SOC_H
#define SOC_H

#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
// #include <pthread.h>
#include <sys/syscall.h>

#define SERVERPORT 31200
#define SERVERIP "127.0.0.1"
#define SERVERNAME "csci4"
#define BUFFLEN 50

typedef struct sockaddr_in sockaddr_in;

void setAddr(sockaddr_in *thisAddr, int thisID, int ADDPORT)
{
	memset(&(*thisAddr), 0, sizeof(struct sockaddr_in));
	(*thisAddr).sin_family = AF_INET;
	(*thisAddr).sin_port = htons(SERVERPORT + thisID + ADDPORT);
	(*thisAddr).sin_addr.s_addr = inet_addr(SERVERIP);
}

#endif
