#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include <pthread.h>
#include "queue.h"
#include <string.h>

char default_root[] = ".";

#define PORT 10000
#define THREAD_POOL_SIZE 10
#define BUFFER_SIZE 100

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = PORT;
    int nThreads = THREAD_POOL_SIZE;
    int bufferMax = BUFFER_SIZE;
   while ((c = getopt(argc, argv, "d:p:t:b:")) != -1)
        switch (c) {
        case 'd':
            root_dir = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 't':
            nThreads = atoi(optarg);
            break;
        case 'b':
            bufferMax = atoi(optarg);
            break;
        default:
            fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t nThreads] [-b bufferMax] \n");
            exit(1);
        }

    int buffer[bufferMax];
    int buff_nb = 0;
	

    pthread_t thread_pool[nThreads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
    
    void* connection_thread_handler(void* p){
	int id_con = *((int*) p);
	request_handle(id_con);
	close_or_die(id_con);
	printf("Thread id %ld has finished ... for now\n",pthread_self());
	return NULL;
	 
    }
    

    void* thread_worker(void* arg){
        while (1){
	    //on s'assure qu'un seul thread modifie la queue à la fois
	    pthread_mutex_lock(&mutex);
	    int* conn_fd;
	    //if we don't check this the thread might wait while the queue is filled and there is nothing to signal anymore
	    if ((conn_fd = dequeue()) == NULL){
	    	pthread_cond_wait(&condition_var,&mutex);
	        }
	    pthread_mutex_unlock(&mutex);
	    if (conn_fd != NULL){
		    //there is a connection
		    printf("Thread %ld",pthread_self());
		    printf(" found connexion %d\n",*conn_fd);
		    connection_thread_handler((void *) conn_fd);
	    }
    }
}

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);


    //let's create some threads
    
    for (int i=0; i < THREAD_POOL_SIZE; i++){
	pthread_create(&thread_pool[i],NULL, thread_worker,NULL);
    }
    
    

    while (1) {
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
	//On assigne de la place dans la mémoire pour créer un pointeur vers notre identifiant de connexion
	int* p_conn_fd = malloc(sizeof(int));
	*p_conn_fd = conn_fd;
	//On s'assure qu'un seul thread modifie la queue à la fois
	pthread_mutex_lock(&mutex);
	enqueue(p_conn_fd);
	printf("\nEnqueuing %d\n",*p_conn_fd);
	//On signale que la queue a été remplie
	pthread_cond_signal(&condition_var);
	pthread_mutex_unlock(&mutex);

    }
    
    
    return 0;
}


    


 
