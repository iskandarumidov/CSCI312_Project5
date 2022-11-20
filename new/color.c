#include <stdio.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

int main()
{
    printf("%sred%s\n", KRED, KNRM);
    printf("%sgreen%s\n", KGRN, KNRM);
    printf("%syellow%s\n", KYEL, KNRM);
    printf("%sblue%s\n", KBLU, KNRM);
    printf("%smagenta%s\n", KMAG, KNRM);
    printf("%scyan%s\n", KCYN, KNRM);
    printf("%swhite%s\n", KWHT, KNRM);
    printf("%snormal%s\n", KNRM, KNRM);
    printf("%sred%s\n", KRED, KNRM);
    printf("normal\n");

    return 0;
}