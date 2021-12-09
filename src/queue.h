#ifndef MYQUEUE_H_
#define MYQUEUE_H_

struct node {
	struct node* next;
	int *conn_fd;
	};

typedef struct node node_t;

void enqueue(int *conn_fd);
int* dequeue();

#endif
