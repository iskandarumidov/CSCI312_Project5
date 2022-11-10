#ifndef SOC_H
#define SOC_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define SERVERPORT 		31200
#define SERVERIP		"199.17.28.80"
#define SERVERNAME		"csci4"
#define BUFFLEN 50

typedef struct sockaddr_in sockaddr_in;

void setAddr(sockaddr_in *thisAddr, int thisID, int ADDPORT) {
	memset (&(*thisAddr), 0, sizeof(struct sockaddr_in));
	(*thisAddr).sin_family = AF_INET;
	(*thisAddr).sin_port = htons(SERVERPORT + thisID + ADDPORT);
	(*thisAddr).sin_addr.s_addr = inet_addr(SERVERIP);
}

#endif
