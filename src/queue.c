#include "queue.h"
#include <stdlib.h>

node_t* head = NULL;
node_t* tail = NULL;

void enqueue(int *conn_fd) {
	node_t *nouv_node = malloc(sizeof(node_t));
	nouv_node->conn_fd = conn_fd;
	nouv_node->next = NULL;
	if (tail == NULL){
		head = nouv_node;
	}
	else {
		tail->next = nouv_node;
	}
	tail = nouv_node;
}

//returns NULL if the queue is empty
//Returns the pointer to a conn_fd if there is one to get

int* dequeue(){
	if (head == NULL){
		return NULL;
	}
	else {
		int* result = head->conn_fd;
		node_t* temp = head;
		head = head->next;
		if (head == NULL) {tail = NULL;}
		free(temp);
		return result;
	}
}
