#include "soc.h"

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
    if (queue_front == -1 || queue_front > queue_rear)
    {
        printf("Underflow \n");
        return -1;
    }
    int res = queue_array[queue_front];
    queue_front = queue_front + 1;
    return res;
}

int peek()
{
    if (queue_front == -1 || queue_front > queue_rear)
    {
        printf("Underflow \n");
        return -1;
    }
    return queue_array[queue_front];
}

void display_queue()
{

    if (queue_front == -1)
        print_log("Empty Queue\n");
    else
    {
        char msg[100];
        sprintf(msg, "Queue is:");
        for (int i = queue_front; i <= queue_rear; i++)
            sprintf(msg, "%s %d", msg, queue_array[i]);
        sprintf(msg, "%s\n", msg);
        print_log("%s", msg);
    }
}
