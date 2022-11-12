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
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/select.h>

char *timestamp();
#define print_log(f_, ...) printf("[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("")
#define SERVERPORT 31200
#define SERVERIP "127.0.0.1"
#define BUFFLEN 50
#define NULL 0 // TODO - bad practice?
#define MAX_CLIENT_QUEUE 5
#define BUFFER_LEN 256

typedef struct sockaddr_in sockaddr_in;

void setAddr(sockaddr_in *thisAddr, int thisID, int ADDPORT)
{
	memset(&(*thisAddr), 0, sizeof(struct sockaddr_in));
	(*thisAddr).sin_family = AF_INET;
	(*thisAddr).sin_port = htons(SERVERPORT + thisID + ADDPORT);
	(*thisAddr).sin_addr.s_addr = inet_addr(SERVERIP);
}

char *timestamp()
{
	time_t now = time(NULL);
	char *time = asctime(gmtime(&now));
	time[strlen(time) - 1] = '\0';
	return time;
}

#endif
