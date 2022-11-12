#include "soc.h"
#include "philosopher.h"

#define SEPS "EC;"
#define PHILOSOPHER_COUNT 5
// #define NULL 0 // TODO - bad practice?
// TODO - in common header - know all addresses/ports?
// TODO - know just the next one?
// TODO - highest philosopher ID will be always different node because it's random
// TODO - So can set one node to run algo first always?
// TODO - need to remember to cut out coordinator from ring

// typedef struct Next
// {
//     int ID;
//     int leftToken;
//     int rightToken;
//     int isEat;
//     int isThink;
// } Next;

int get_random_in_range(int low, int high);
int str_length(char str[]);
void set_coordinator_next(char str[]);

int id = 123;
int next_id = -1;
int coordinator = -1;

int main(int argc, char *argv[])
{
    char str[100] = "C;123;45;6;78;1;";

    if (str[0] == 'E')
    {
        printf("ELECTION MESSAGE DETECTED\n");
        // here I append current ID and send to next
        char id_char[100];
        sprintf(id_char, "%d", id);

        printf("Original String: %s\n", str);
        strncat(str, id_char, str_length(id_char));
        printf("Appended String: %s\n", str);
    }
    else
    {
        printf("COORDINATOR MESSAGE DETECTED\n");
        // here I set coordinator and next
        set_coordinator_next(str);
        printf("COORDINATOR: %d\n", coordinator);
        printf("NEXT: %d\n", next_id);
        // TODO - if coordinator detected, stop listening for other messages
    }
    printf("RAND: %d\n", get_random_in_range(0, 10)); // TODO - needs to move to launcher
    // detect if I am the coordinator
    if (coordinator == id)
    {
        // do execl
        execl("./coordinator", "coordinator", (char *)NULL); // TODO - need err check?
    }

    return 0;
}

int get_random_in_range(int low, int high)
{
    srand(time(NULL));
    return (rand() % (high - low + 1)) + low;
}

int str_length(char str[])
{
    int count;
    for (count = 0; str[count] != '\0'; ++count)
        ;
    return count;
}

void set_coordinator_next(char str[])
{
    int max = id;
    char *token = strtok(str, SEPS);
    int id_ints[PHILOSOPHER_COUNT];
    int i = 0;
    while (token != NULL)
    {
        int token_int = atoi(token);
        id_ints[i] = token_int;
        if (token_int > max)
        {
            max = token_int;
        }
        token = strtok(NULL, SEPS);
        i++;
    }

    for (i = 0; i < PHILOSOPHER_COUNT; ++i)
    {
        if (id_ints[i] == id)
        {
            next_id = ((i == PHILOSOPHER_COUNT - 1) ? id_ints[0] : id_ints[i + 1]);
            break;
        }
    }

    coordinator = max;
}