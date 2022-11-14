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
int get_random_in_range(int low, int high);
// void setAddr(sockaddr_in *thisAddr, int thisID, int ADDPORT);

#define print_log(f_, ...) printf("[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("")
// #define SERVERPORT 31200 // TODO - should I set a dedicated known port for controller or use old phil port?
#define SERVERIP "127.0.0.1"
#define NULL 0
#define MAX_CLIENT_QUEUE 5
#define BUFFER_LEN 256
#define SEPARATORS "EC;"
#define PHILOSOPHER_COUNT 6

int read_ports[6] = {31200, 31201, 31202, 31203, 31204, 31205};
int write_ports[6] = {31201, 31202, 31203, 31204, 31205, 31200};
// int read_ports[6] = {31200, 31201};
// int write_ports[6] = {31201, 31200};

typedef struct sockaddr_in sockaddr_in;

// void setAddr(sockaddr_in *thisAddr, int thisID, int ADDPORT)
// {
// 	memset(&(*thisAddr), 0, sizeof(struct sockaddr_in));
// 	(*thisAddr).sin_family = AF_INET;
// 	(*thisAddr).sin_port = htons(SERVERPORT + thisID + ADDPORT);
// 	(*thisAddr).sin_addr.s_addr = inet_addr(SERVERIP);
// }

char *timestamp()
{
	time_t now = time(NULL);
	char *time_arr = asctime(gmtime(&now));
	time_arr[strlen(time_arr) - 1] = '\0';
	return time_arr;
}

// int get_random_in_range(int low, int high)
// {
// 	srand(time(NULL));
// 	return (rand() % (high - low + 1)) + low;
// }

int not_random = 0;
// BUG - not so random!
int get_random_in_range(int low, int high)
{
	return not_random++;
	// srand(time(NULL));
	// return (rand() % (high - low + 1)) + low;
}

#endif
