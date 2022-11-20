#include "soc.h"
#define print_log(f_, ...) printf("[%s] COORDIN: ", timestamp()), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro, set philosopher ID

void print_chopstick_arr();
int chopsticks[6] = {-1, 1, 1, 1, 1, 1};

int main()
{
    // char str[80] = "This is - www.tutorialspoint.com - website";
    // char str2[80];
    // sprintf(str2, "%s", str);
    // const char s[2] = "-";
    // char *token;
    // extract_incoming_id(str2);

    /* get the first token */
    // token = strtok(str, s);

    /* walk through other tokens */
    // while (token != NULL)
    // {
    //     printf(" %s\n", token);

    //     token = strtok(NULL, s);
    // }
    // printf(" %s\n", str);

    print_chopstick_arr();

    return (0);
}

void print_chopstick_arr()
{
    char chopsticks_char[40];
    int i;
    // sprintf(chopsticks_char, "%d %d %d %d %d", chopsticks[1], chopsticks[2], chopsticks[3], chopsticks[4], chopsticks[5]);
    // print_log("CHOPSTICKS: %s\n", chopsticks_char);
    print_log("CHOPSTICKS: %d %d %d %d %d\n", chopsticks[1], chopsticks[2], chopsticks[3], chopsticks[4], chopsticks[5]);
}
