#include <stdbool.h>
#include <stdio.h>

// define the maximim capacity of the stack
#define MAX 100

// define a struct for the stack
typedef struct {
  int arr[MAX]; // array
  int top;      // index
} Stack;

// if use array it would be arr[]

// function to initialize the stack
void init(Stack *stack) {
  printf("\ninit Stack!");
  stack->top = -1;
}

// function to check if the stack is empty
bool isEmpty(Stack *stack) { return stack->top == -1; }

// function to check if the stack is full
bool isFull(Stack *stack) { return stack->top >= MAX - 1; }

// function to push an element onto the stack
void push(Stack *stack, int value) {
  if (isFull(stack)) {
    printf("\nStack overflow\n");
    return;
  }
  stack->arr[++stack->top] = value;
  printf("pushed %d onto the stack \n", value);
}

// function to pop an element from the stack
int pop(Stack *stack) {
  if (isEmpty(stack)) {
    printf("Stack underflow\n");
    return -1;
  }
  int popped = stack->arr[stack->top--];
  printf("popped %d from the stack\n", popped);
  return popped;
}

// function to peek the top element of the stack
int peek(Stack *stack) {
  if (isEmpty(stack)) {
    printf("Stack underflow\n");
    return -1;
  }
  return stack->arr[stack->top];
}

int main() {
  Stack stack;
  init(&stack);
  printf("successfull!!\n");

  push(&stack, 2);
  printf("top element is %d\n", peek(&stack));

  push(&stack, 3);
  printf("top element is %d\n", peek(&stack));

  push(&stack, 4);
  printf("top element is %d\n", peek(&stack));

  push(&stack, 5);
  printf("top element is %d\n", peek(&stack));

  push(&stack, 6);
  printf("top element is %d\n", peek(&stack));

  while (!isEmpty(&stack)) {
    printf("top element is %d\n", peek(&stack));
    printf("popped element %d\n", pop(&stack));
  }

  return 0;
}