//
// Created by callum on 25/03/18.
//

#include <printf.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

int *available;
int *total_resources;
int **max_resources;
int **alloc_resources;
int num_resources;
int num_processes;

pthread_mutex_t resource_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_resources;

int * create_resource_request(int process_id) {
    int *request = malloc(num_resources * sizeof(int));

    int i;
    for (i = 0; i < num_resources; i++) {
        int remaining = max_resources[process_id][i] - alloc_resources[process_id][i];
        request[i] = rand() % (remaining + 1);
    }

    return request;
}

int can_finish(int process_id) {
    int i;
    pthread_mutex_lock(&resource_mutex);
    for (i = 0; i < num_resources; i++) {
        if (alloc_resources[process_id][i] != max_resources[process_id][i]) {
            pthread_mutex_unlock(&resource_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&resource_mutex);
    return 1;
}

void determine_state(int **need, int *finish, int *work, int **temp_alloc) {
    int i, j;
    for (i = 0; i < num_processes; i++) {
        if (!finish[i]) {
            int safe = 1;
            for(j=0; j < num_resources; j++) {
                if(need[i][j] > work[j]) {
                    safe = 0;
                }
            }
            if(safe) {
                for(j=0; j < num_resources; j++) {
                    work[j] += temp_alloc[i][j];
                }
                finish[i] = 1;
                determine_state(need, finish, work, temp_alloc);
            }
        }
    }
}

int safety(int *request, int process_id) {
    int i, j;

    //init work matrix
    int *work = malloc(num_resources * sizeof(int));
    int *finish = malloc(num_processes * sizeof(int));

    int **need = malloc(num_processes * sizeof(int *));

    int **temp_alloc = malloc(num_processes * sizeof(int *));

    for (i=0; i< num_processes; i++){
        need[i] = malloc(num_resources * sizeof(int));
        temp_alloc[i] = malloc(num_resources * sizeof(int));
        for (j = 0; j < num_resources; j++) {
            temp_alloc[i][j] = i == process_id ? alloc_resources[i][j] + request[j] : alloc_resources[i][j];
            need[i][j] = max_resources[i][j] - temp_alloc[i][j];
        }
    }

    for (i=0; i< num_resources; i++)
        work[i] = available[i] - request[i];

    for (i=0; i< num_processes; i++)
        finish[i] = 0;

    determine_state(need, finish, work, temp_alloc);

    //Free all arrays
    free(work);

    for (i=0; i< num_processes; i++){
        free(need[i]);
        free(temp_alloc[i]);
    }

    free(need);
    free(temp_alloc);

    for(i=0; i<num_processes; i++) {
        if (!finish[i]){
            free(finish);
            return 0;
        }
    }

    free(finish);
    return 1;

}

int bankers(int *request, int process_id) {
    int i;
    pthread_mutex_lock(&resource_mutex);

    //Check to make sure request is not greater than need (otherwise process is violating resource contract)
    for (i = 0; i < num_resources; i++) {
        if (request[i] > max_resources[process_id][i] - alloc_resources[process_id][i]) {
            pthread_mutex_unlock(&resource_mutex);
            return 0;
        }
    }

    //Deny request if requesting more than available
    for (i = 0; i < num_resources; i++) {
        if (request[i] > available[i]) {
            pthread_mutex_unlock(&resource_mutex);
            return 0;
        }
    }

    int safety_result = safety(request, process_id);
    //If the resource allocation is safe, allocate the resources
    if(safety_result) {
        for(i = 0 ; i < num_resources; i++) {
            alloc_resources[process_id][i] += request[i];
            available[i] -= request[i];
        }
    }

    pthread_mutex_unlock(&resource_mutex);

    return safety_result;
}

void free_resources(int process_id) {
    int i;
    printf("Process %d completed\n", process_id);
    pthread_mutex_lock(&resource_mutex);
    for (i = 0; i < num_resources; i++) {
        available[i] += alloc_resources[process_id][i];
        alloc_resources[process_id][i] = 0;
    }
    pthread_mutex_unlock(&resource_mutex);
    sem_post(&sem_resources);

}

void print_resource_request_granted(int *request, int process_id) {
    int i;
    printf("Process %d resource request was accepted: ", process_id);
    for (i = 0; i < num_resources; i++) {
        printf("%d ", request[i]);
    }
    printf("\n");
}

void print_resource_request_rejected(int *request, int process_id) {
    int i;
    printf("Process %d  resource request was rejected: ", process_id);
    for (i = 0; i < num_resources; i++) {
        printf("%d ", request[i]);
    }
    printf("\n");
}

void print_resource_request(int *request, int process_id) {
    int i;
    printf("Process %d requesting resources: ", process_id);
    for (i = 0; i < num_resources; i++) {
        printf("%d ", request[i]);
    }
    printf("\n");
}

void *fn_process(void *arg_process_id) {
    int process_id = *((int *) arg_process_id);
    printf("Process %d started\n", process_id);
    int true = 1;
    while(true) {
        int *request  = create_resource_request(process_id);
        print_resource_request(request, process_id);
        if (bankers(request, process_id)) {
            print_resource_request_granted(request, process_id);
            if (can_finish(process_id)) {
                break;
            }
            sleep(3);
        } else {
            print_resource_request_rejected(request, process_id);
            sem_wait(&sem_resources);
        }
        free(request);
    }

    free_resources(process_id);
}

int main(int argc, char *argv[]) {
    int i, j, ret;

    //allocate memory to store requests

    printf("Enter the number of processes: \n");
    scanf("%d", &num_processes);

    printf("Enter the number of different resources: \n");
    scanf("%d", &num_resources);

    printf("Enter the total amount of each resource in the system, separated by spaces: \n");
    total_resources = malloc(num_resources * sizeof(int));
    available = malloc(num_resources * sizeof(int));
    int resource;
    for (i=0; i < num_resources ; i++) {
        scanf("%d", &resource);
        total_resources[i] = resource;
        available[i] = resource;
    }

    //Create process resource matrix
    max_resources = malloc(num_processes * sizeof(int));
    alloc_resources = malloc(num_processes * sizeof(int));
    for (i=0; i< num_processes; i++){
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

    sem_init(&sem_resources, 0, 0);
    time_t t;
    srand((unsigned) time(&t));
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