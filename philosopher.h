#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#define N 5

typedef struct Philosopher {
	int ID;
	int leftToken;
	int rightToken;
	int isEat;
	int isThink;
} Philosopher;

#endif
