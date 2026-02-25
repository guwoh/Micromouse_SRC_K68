#include <stdbool.h>
#include <stdio.h>

// define the maximum capacity of the queue
#define MAX 100

// define a struct for the queue
typedef struct {
  int arr[MAX]; // array, size of arr
  int front;    // index of front element
  int rear;     // index of rear element
  int size;     // current number of elements
} Queue;

// function to initialize the queue
void init(Queue *queue) {
  printf("\ninit Queue!");
  queue->front = 0;
  queue->rear = -1;
  queue->size = 0;
}

// function to check if the queue is empty
bool isEmpty(Queue *queue) { return queue->size == 0; }

// function to check if the queue is full
bool isFull(Queue *queue) { return queue->size >= MAX; }

// function to add an element to the rear of the queue
void enqueue(Queue *queue, int value) {
  if (isFull(queue)) {
    printf("Queue overflow\n");
    return;
  }
  queue->rear = (queue->rear + 1) % MAX; // circular wrap-around
  queue->arr[queue->rear] = value;
  queue->size++;
  printf("enqueued %d into the queue\n", value);
}

// function to remove an element from the front of the queue
int dequeue(Queue *queue) {
  if (isEmpty(queue)) {
    printf("Queue underflow\n");
    return -1;
  }
  int removed = queue->arr[queue->front];
  queue->front = (queue->front + 1) % MAX; // circular wrap-around
  queue->size--;
  printf("dequeued %d from the queue\n", removed);
  return removed;
}

// function to peek the front element of the queue
int peek(Queue *queue) {
  if (isEmpty(queue)) {
    printf("Queue underflow\n");
    return -1;
  }
  return queue->arr[queue->front];
}

int main() {
  Queue queue;
  init(&queue);
  printf("successfull!!\n");

  enqueue(&queue, 2);
  printf("front element is %d\n", peek(&queue));

  enqueue(&queue, 3);
  printf("front element is %d\n", peek(&queue));

  enqueue(&queue, 4);
  printf("front element is %d\n", peek(&queue));

  enqueue(&queue, 5);
  printf("front element is %d\n", peek(&queue));

  enqueue(&queue, 6);
  printf("front element is %d\n", peek(&queue));

  while (!isEmpty(&queue)) {
    printf("front element is %d\n", peek(&queue));
    printf("dequeued element %d\n", dequeue(&queue));
  }

  return 0;
}
