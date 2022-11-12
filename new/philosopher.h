#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#define N 5

typedef struct Philosopher
{
	int ID;
	int leftToken;
	int rightToken;
	int isEat;
	int isThink;
} Philosopher;

#endif
