#include <string.h>
#include <stdio.h>

void extract_incoming_id(char str[])
{
    char *token = strtok(str, "-");

    /* walk through other tokens */
    while (token != NULL)
    {
        printf(" %s\n", token);

        token = strtok(NULL, "-");
    }
}

int main()
{
    char str[80] = "This is - www.tutorialspoint.com - website";
    char str2[80];
    sprintf(str2, "%s", str);
    const char s[2] = "-";
    char *token;
    extract_incoming_id(str2);

    /* get the first token */
    // token = strtok(str, s);

    /* walk through other tokens */
    // while (token != NULL)
    // {
    //     printf(" %s\n", token);

    //     token = strtok(NULL, s);
    // }
    printf(" %s\n", str);

    return (0);
}