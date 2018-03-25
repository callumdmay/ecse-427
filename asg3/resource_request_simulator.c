//
// Created by callum on 25/03/18.
//

#include <printf.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

int *cur_avail_resources, *total_resources;
int **max_resources;
int **alloc_resources;
int num_resources;

pthread_mutex_t resource_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_resources;

int * generate_resource_request(int process_id) {
    int *request = malloc(num_resources * sizeof(int));

    int i;
    for (i = 0; i < num_resources; i++) {
        int remaining = max_resources[process_id][i] - alloc_resources[process_id][i];
        request[i] = rand() % remaining;
    }

    return request;
}

int can_finish(int process_id) {
    int i;
    pthread_mutex_lock(&resource_mutex);
    for (i = 0; i < num_resources; i++) {
        if (alloc_resources[process_id][i] != max_resources[process_id][i]) {
            return 0;
        }
    }
    pthread_mutex_unlock(&resource_mutex);
    return 1;
}

int bankers(int *request, int process_id) {

    return 1;
}

void free_resources(int process_id) {
    int i;
    pthread_mutex_lock(&resource_mutex);
    for (i = 0; i < num_resources; i++) {
        cur_avail_resources[i] += alloc_resources[process_id][i];
        alloc_resources[process_id][i] = 0;
    }
    pthread_mutex_unlock(&resource_mutex);
    sem_post(&sem_resources);
}

void *fn_process(void *arg_process_id) {
    int process_id = *((int *) arg_process_id);

    while(!can_finish(process_id)) {
        int *request  = generate_resource_request(process_id);
        if (bankers(request, process_id)) {
            sleep(3);
        } else {
            sem_wait(&sem_resources);
        }
    }

    free_resources(process_id);
}

int main(int argc, char *argv[]) {
    int num_processes, i, j, ret;

    //allocate memory to store requests
    printf("Enter the number of processes: \n");
    scanf("%d", &num_processes);

    printf("Enter the number of distinct resources: \n");
    scanf("%d", &num_resources);

    printf("Enter the total amount of each resource in the system, separated by spaces: \n");
    total_resources = malloc(num_resources * sizeof(int));
    cur_avail_resources = malloc(num_resources * sizeof(int));
    int resource;
    for (i=0; i < num_resources ; i++) {
        scanf("%d", &resource);
        total_resources[i] = resource;
        cur_avail_resources[i] = resource;
    }

    //Create process resource matrix
    max_resources = malloc(num_processes * sizeof(int));
    alloc_resources = malloc(num_processes * sizeof(int));
    for (i=0; i< num_resources; i++){
        alloc_resources[i] = (int *)malloc(num_resources * sizeof(int));
        max_resources[i] = (int *)malloc(num_resources * sizeof(int));
    }

    for (i=0; i < num_processes ; i++) {
        printf("Enter the max resource claim by process %d, separated by spaces: \n", i);
        for (j=0; j < num_resources ; j++) {
            scanf("%d", &resource);
            //Get max values while also initializing alloc matrix to 0
            max_resources[i][j] = resource;
            alloc_resources[i][j] = 0;
        }
    }

    sem_init(&sem_resources, 0, 1);

    pthread_t *process_threads = malloc(sizeof(pthread_t) * num_processes);

    for (i = 0; i < num_processes; i++) {
        printf("Creating process thread %d\n", i);
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }
        //Set the thread argument to an integer
        *arg = i;
        ret = pthread_create(&process_threads[i], NULL, &fn_process, arg);
        if (ret != 0) {
            printf("Create pthread error!\n");
            exit(1);
        }
    }

    //Use pthread join so as to wait on the process threads to finish
    for (i = 0; i < num_processes; i++) {
        pthread_join(process_threads[i], NULL);
    }

    return 0;

}