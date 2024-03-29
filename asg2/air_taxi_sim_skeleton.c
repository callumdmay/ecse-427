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
sem_t sem_fill_count;
sem_t sem_empty_count;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void *arg_plane_id) {
    //Get the plane ID as int and free the void argument memory
    int plane_id = *((int *) arg_plane_id);
    free(arg_plane_id);

    // Intializes random number generator
    time_t t;
    srand((unsigned) time(&t));

    //Need this to disable endless loop warning in CLion
    int true = 1;
    while(true) {
        //Create the passenger count
        int passenger_count = 5 + rand() % 6;
        printf("Airplane %d arrives with %d passengers\n", plane_id, passenger_count);
        int i = 0;
        for(i = 0; i < passenger_count; i++) {
            //Create passenger ID
            int passenger_id = 1000000 + 1000*(plane_id) + i;
            if(sem_trywait(&sem_empty_count) != 0) {
                //Queue is full, break loop
                printf("Platform is full: Rest of passengers of plane %d take the bus\n", plane_id);
                break;
            }
            //Acquire the lock to mutate the queue
            pthread_mutex_lock(&queue_mutex);
            //Print and enqueue
            printf("Passenger %d of airplane %d arrives to platform\n", passenger_id, plane_id);
            enqueue(queue, passenger_id);
            //Release lock and increment fill semaphore
            pthread_mutex_unlock(&queue_mutex);
            sem_post(&sem_fill_count);
        }
        sleep(1);
    };

}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void *arg_taxi_id) {
    //Get the taxi ID as int and free the void argument memory
    int taxi_id = *((int *) arg_taxi_id);
    free(arg_taxi_id);

    //Need this to disable endless loop warning in CLion
    int true = 1;
    while(true) {
        printf("Taxi driver %d arrives\n", taxi_id);
        if(sem_trywait(&sem_fill_count)!= 0) {
            //The queue is empty, wait for the semaphore
            printf("Taxi driver %d waits for passengers to enter the platform\n", taxi_id);
            sem_wait(&sem_fill_count);
        }
        //Acquire the lock to mutate the queue
        pthread_mutex_lock(&queue_mutex);
        int passenger = dequeue(queue);
        printf("Taxi driver %d picked up client %d from the platform\n", taxi_id, passenger);
        //Release lock and increment empty semaphore
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&sem_empty_count);
        //Delay a random amount of time between 10 and 30 (fake) seconds
        int delay = ((10 + rand()%20)*1000000)/60;
        usleep(delay);
    }
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
    sem_init(&sem_empty_count, 0, BUFFER_SIZE);
    sem_init(&sem_fill_count, 0, 0);

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
        ret = pthread_create(&airplane_threads[i], NULL, &FnAirplane, arg);
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
        ret = pthread_create(&taxi_threads[i], NULL, &FnTaxi, arg);
        if (ret != 0) {
            printf("Create pthread error!\n");
            exit(1);
        }
    }

    //Use pthread join so as to wait on the taxi and airplane threads to finish
    for (i = 0; i < num_taxis; i++) {
        pthread_join(taxi_threads[i], NULL);
    }

    for (i = 0; i < num_airplanes; i++) {
        pthread_join(airplane_threads[i], NULL);
    }

    pthread_exit(NULL);
}
