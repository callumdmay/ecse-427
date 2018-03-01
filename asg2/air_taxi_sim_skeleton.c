/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------
 */

//Please enter your name and McGill ID below
//Name: Callum May
//McGill ID: 260564523



#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>


int BUFFER_SIZE = 100; //size of queue



// A structure to represent a queue
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int *array;
};

// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue *createQueue(unsigned capacity) {
    struct Queue *queue = (struct Queue *) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int *) malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue *queue) {
    return ((queue->size) >= queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue *queue) {
    return (queue->size == 0);
}

// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue *queue, int item) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue *queue) {
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue *queue) {
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue *queue) {
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void print(struct Queue *queue) {
    if (queue->size == 0) {
        return;
    }

    for (int i = queue->front; i < queue->front + queue->size; i++) {
        printf(" Element at position %d is %d \n ", i % (queue->capacity), queue->array[i % (queue->capacity)]);
    }

}

struct Queue *queue;

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void *cl_id) {
    printf("In thread");
    int airplane_id = *((int *) cl_id);
    free(cl_id);
    printf("Created airplane with id %d\n", airplane_id);
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void *pr_id) {
    int taxi_id = *((int *) pr_id);
    free(pr_id);
}

int main(int argc, char *argv[]) {

    int num_airplanes;
    int num_taxis;

    num_airplanes = atoi(argv[1]);
    num_taxis = atoi(argv[2]);

    printf("You entered: %d airplanes per hour\n", num_airplanes);
    printf("You entered: %d taxis\n", num_taxis);


    //initialize queue
    queue = createQueue(BUFFER_SIZE);

    //declare arrays of threads and initialize semaphore(s)
    pthread_t *airplane_threads = malloc(sizeof(pthread_t) * num_airplanes);
    pthread_t *taxi_threads = malloc(sizeof(pthread_t) * num_taxis);

    //create arrays of integer pointers to ids for taxi / airplane threads
    int *taxi_ids[num_taxis];
    int *airplane_ids[num_airplanes];

    int i = 0, ret = -1;
    //create threads for airplanes

    for (i = 0; i < num_airplanes; i++) {
        printf("Creating airplane thread %d\n", i);

        //Allocate the memory for thread argument
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }
        //Set the thread argument to an integer
        *arg = i;
        ret = pthread_create(&airplane_threads[i], NULL, &FnAirplane, i);
        if (ret != 0) {
            printf("Create pthread error!\n");
            exit(1);
        }
    }

    //create threads for taxis
    for (i = 0; i < num_taxis; i++) {
        printf("Creating taxi thread %d\n", i);
        //Allocate the memory for thread argument
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }
        //Set the thread argument to an integer
        *arg = i;
        ret = pthread_create(&airplane_threads[i], NULL, &FnTaxi, i);
        if (ret != 0) {
            printf("Create pthread error!\n");
            exit(1);
        }
    }
    pthread_exit(NULL);
}
