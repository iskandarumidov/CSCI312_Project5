#include <stdio.h>
void swap(int *n1, int *n2);
void str_manip(char *str);
void no_manip(char str[]);

int main()
{
    int num1 = 5, num2 = 10;

    // address of num1 and num2 is passed
    swap(&num1, &num2);

    printf("num1 = %d\n", num1);
    printf("num2 = %d\n", num2);
    char str[] = "Hello";
    str_manip(str);
    printf("got back: %s\n", str);

    no_manip(str);
    printf("got back no manip: %s\n", str);
    return 0;
}

void swap(int *n1, int *n2)
{
    int temp;
    temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

void str_manip(char *str)
{
    printf("received: %s\n", str);
    str[0] = 'A';
    printf("changed: %s\n", str);
}

void no_manip(char str[])
{
    printf("received no manip: %s\n", str);
    str[0] = 'B';
    printf("changed no manip: %s\n", str);
}