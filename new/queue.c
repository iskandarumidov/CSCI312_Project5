// #include <stdio.h>
// #include <stdlib.h>
#include "soc.h"

// #define print_log(f_, ...) printf("[%s] COORDIN: %d ", timestamp(), id), printf((f_), ##__VA_ARGS__), printf("") // Redefine macro, set philosopher ID

#define QUEUE_SIZE 30

void enqueue(int insert_item);
int dequeue();
void display_queue();
int queue_size();
int queue_is_empty();
int peek();

void wrong_pring_array();

int queue_array[QUEUE_SIZE];
int queue_rear = -1;
int queue_front = -1;

// int main()
// {
//     dequeue();
//     printf("EMPTY?: %d\n", queue_is_empty());
//     printf("SIZE: %d\n", queue_size());
//     enqueue(1);
//     printf("EMPTY?: %d\n", queue_is_empty());
//     enqueue(2);
//     enqueue(3);
//     printf("SIZE: %d\n", queue_size());
//     display_queue();
//     wrong_pring_array();
//     dequeue();
//     printf("SIZE: %d\n", queue_size());
//     display_queue();
//     wrong_pring_array();
//     dequeue();
//     printf("SIZE: %d\n", queue_size());
//     display_queue();
//     wrong_pring_array();
//     dequeue();
//     printf("SIZE: %d\n", queue_size());
//     printf("EMPTY?: %d\n", queue_is_empty());
// }

int queue_size()
{
    if (queue_rear == -1 && queue_front == -1)
    {
        return 0;
    }
    return queue_rear - queue_front + 1;
}

int queue_is_empty()
{
    return (queue_size() == 0);
}

void enqueue(int insert_item)
{
    if (queue_rear == QUEUE_SIZE - 1)
        printf("Overflow \n");
    else
    {
        if (queue_front == -1)
            queue_front = 0;
        queue_rear = queue_rear + 1;
        queue_array[queue_rear] = insert_item;
    }
}

int dequeue()
{
    int res = -1;
    if (queue_front == -1 || queue_front > queue_rear)
    {
        printf("Underflow \n");
        // return;
    }
    else
    {
        res = queue_array[queue_front];
        // printf("Element deleted from the Queue: %d\n", queue_array[queue_front]);
        queue_front = queue_front + 1;
    }
    return res;
}

int peek()
{
    if (queue_front == -1 || queue_front > queue_rear)
    {
        printf("Underflow \n");
        return -1;
    }
    else
    {
        return queue_array[queue_front];
    }
}

void display_queue()
{

    if (queue_front == -1)
        printf("Empty Queue \n");
    else
    {
        printf("Queue: ");
        for (int i = queue_front; i <= queue_rear; i++)
            printf("%d ", queue_array[i]);
        printf("\n");
    }
}

void wrong_pring_array()
{
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        printf("%d ", queue_array[i]);
    }
    printf("\n");
}