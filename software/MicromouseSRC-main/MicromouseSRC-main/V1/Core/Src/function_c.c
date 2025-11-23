/*
 * function_c.c
 *
 *  Created on: Oct 24, 2025
 *      Author: kinhz
 */

#include "function_c.h"

Vector mouse_state;

queue queue_create() {
	queue q = (queue) malloc(sizeof(struct _queue));
	if (q == NULL) {
		abort();
	}
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	return q;
}

void queue_destroy(queue q) {
	if (q == NULL) {
		abort();
	}
	queue_clear(q);
	free(q);
}

void queue_push(queue q, int elem) {
	struct node *n;
	n = (struct node*) malloc(sizeof(struct node));
	if (n == NULL) {
		abort();
	}
	n->data = elem;
	n->next = NULL;
	if (q->head == NULL) {
		q->head = q->tail = n;
	} else {
		q->tail->next = n;
		q->tail = n;
	}
	q->size += 1;
}

int queue_pop(queue q) {
	if (queue_is_empty(q)) {
		abort();
	}
	struct node *head = q->head;
	if (q->head == q->tail) {
		q->head = NULL;
		q->tail = NULL;
	} else {
		q->head = q->head->next;
	}
	q->size -= 1;
	int data = head->data;
	free(head);
	return data;
}

int queue_first(queue q) {
	if (queue_is_empty(q)) {
		abort();
	}
	return q->head->data;
}

int queue_is_empty(queue q) {
	if (q == NULL) {
		abort();
	}
	return q->head == NULL;
}

int queue_size(queue q) {
	if (q == NULL) {
		abort();
	}
	return q->size;
}

void queue_clear(queue q) {
	if (q == NULL) {
		abort();
	}
	while (q->head != NULL) {
		struct node *tmp = q->head;
		q->head = q->head->next;
		free(tmp);
	}
	q->tail = NULL;
	q->size = 0;
}

void initVector(Vector *vec, size_t initialCapacity) {
	vec->data = (int*) malloc(initialCapacity * sizeof(int));
	vec->size = 0;
	vec->capacity = initialCapacity;
	vec->index = 0;
}

void resizeVector(Vector *vec) {
	vec->capacity *= 2;
	vec->data = (int*) realloc(vec->data, vec->capacity * sizeof(int));
}

void pushBack(Vector *vec, int value) {
	if (vec->size == vec->capacity) {
		resizeVector(vec);
	}
	vec->data[vec->size++] = value;
}

int get(Vector *vec, size_t index) {
	if (index < vec->size) {
		return vec->data[index];
	}
	exit(EXIT_FAILURE);
}

void freeVector(Vector *vec) {
	free(vec->data);
	vec->data = NULL;
	vec->size = 0;
	vec->capacity = 0;
}
