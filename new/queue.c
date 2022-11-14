// #include <stdio.h>
// #include <stdlib.h>

#define QUEUE_SIZE 100

void enqueue(int insert_item);
void dequeue();
void display_queue();
int queue_size();
int queue_is_empty();

int queue_array[QUEUE_SIZE];
int queue_rear = -1;
int queue_front = -1;

// int main()
// {
// dequeue();
// printf("EMPTY?: %d\n", queue_is_empty());
// printf("SIZE: %d\n", queue_size());
// enqueue(1);
// printf("EMPTY?: %d\n", queue_is_empty());
// enqueue(2);
// enqueue(3);
// printf("SIZE: %d\n", queue_size());
// display_queue();
// dequeue();
// printf("SIZE: %d\n", queue_size());
// display_queue();
// dequeue();
// printf("SIZE: %d\n", queue_size());
// display_queue();
// dequeue();
// printf("SIZE: %d\n", queue_size());
// printf("EMPTY?: %d\n", queue_is_empty());
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

void dequeue()
{
    if (queue_front == -1 || queue_front > queue_rear)
    {
        printf("Underflow \n");
        return;
    }
    else
    {
        printf("Element deleted from the Queue: %d\n", queue_array[queue_front]);
        queue_front = queue_front + 1;
    }
}

void display_queue()
{

    if (queue_front == -1)
        printf("Empty Queue \n");
    else
    {
        printf("Queue: \n");
        for (int i = queue_front; i <= queue_rear; i++)
            printf("%d ", queue_array[i]);
        printf("\n");
    }
}