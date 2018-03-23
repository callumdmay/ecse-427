#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc

#define LOW 0
#define HIGH 199
#define START 53

//compare function for qsort
//you might have to sort the request array
//use the qsort function 
// an argument to qsort function is a function that compares 2 quantities
//use this there.
int cmpfunc (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

//function to swap 2 integers
void swap(int *a, int *b)
{
    if (*a != *b)
    {
        *a = (*a ^ *b);
        *b = (*a ^ *b);
        *a = (*a ^ *b);
        return;
    }
}

//Prints the sequence and the performance metric
void printSeqNPerformance(int *request, int numRequest)
{
    int i, last, acc = 0;
    last = START;
    printf("\n");
    printf("%d", START);
    for (i = 0; i < numRequest; i++)
    {
        printf(" -> %d", request[i]);
        acc += abs(last - request[i]);
        last = request[i];
    }
    printf("\nPerformance : %d\n", acc);
    return;
}

//access the disk location in FCFS
void accessFCFS(int *request, int numRequest)
{
    //simplest part of assignment
    printf("\n----------------\n");
    printf("FCFS :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SSTF
void accessSSTF(int *request, int numRequest)
{
    int current_head = START;
    int MAX = HIGH + 1;

    qsort(request, numRequest, sizeof(int), &cmpfunc);
    int i = 0;
    int left = -1;
    int right = numRequest + 1;
    int min_distance = MAX;
    while(left == -1) {
        if (i == numRequest) {
            left = numRequest - 1;
            right = numRequest;
        } else {
            int current_distance = abs(request[i] - START);
            if (current_distance <= min_distance) {
                min_distance = current_distance;
            } else {
                left = i -1;
                right = i;
            }
        }
        i++;
    }

    int *newRequest = malloc(numRequest * sizeof(int));
    int queue_index = 0;

    while (left!=-1 || right!= numRequest) {
        int left_distance = MAX, right_distance = MAX;
        if(left != -1) {
            left_distance = abs(request[left] - current_head);
        }

        if(right != numRequest) {
            right_distance = abs(request[right] - current_head);
        }

        if (left_distance < right_distance) {
            newRequest[queue_index] = request[left];
            current_head = request[left];
            queue_index++;
            left--;
        } else {
            newRequest[queue_index] = request[right];
            current_head = request[right];
            queue_index++;
            right++;
        }
    }

    printf("\n----------------\n");
    printf("SSTF :");
    printSeqNPerformance(newRequest, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SCAN
void accessSCAN(int *request, int numRequest)
{
    qsort(request, numRequest, sizeof(int), &cmpfunc);
    int start = START;
    int i = 0;
    int left = -1;
    int right = numRequest;
    while(left == -1 && right == numRequest) {
        if (i == numRequest) {
            left = numRequest - 1;
        } else {
            if (start < 100) {
                if (request[i] > start) {
                    left = i - 1;
                    right = i;
                }
            } else {
                if (request[i] >= start) {
                    left = i - 1;
                    right = i;
                }
            }
        }
        i++;
    }

    //Increment numRequest to account for SCAN till end

    int *newRequest;
    int queue_index = 0;

    if (start < 100) {
        //going left first

        if (right != numRequest) {
            numRequest++;
        }

        *newRequest = malloc(numRequest * sizeof(int));

        while (left!=-1) {
            newRequest[queue_index] = request[left];
            left--;
            queue_index++;
        }


        //If we have to go right, move all the way left, then start going right
        if (right != numRequest) {
            newRequest[queue_index++] = 0;

            while(right != numRequest) {
                newRequest[queue_index] = request[right];
                right++;
                queue_index++;
            }
        }
    } else {
        //going right first

        if (left != -1)
            numRequest++;

        *newRequest = malloc(numRequest * sizeof(int));

        // -1 accounts for the addition of the 199 to the queue
        while(right != numRequest - 1) {
            newRequest[queue_index] = request[right];
            right++;
            queue_index++;
        }

        //If we have to go left, move all the way right, then start going left
        if (left != -1) {
            newRequest[queue_index++] = 199;

            while (left!=-1) {
                newRequest[queue_index] = request[left];
                left--;
                queue_index++;
            }
        }
    }

    printf("\n----------------\n");
    printf("SCAN :");
    printSeqNPerformance(newRequest, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int *request, int numRequest)
{
    qsort(request, numRequest, sizeof(int), &cmpfunc);
    int start = START;
    int i = 0;
    int left = -1;
    int right = numRequest;
    while(left == -1 && right == numRequest) {
        if (i == numRequest) {
            left = numRequest - 1;
        } else {
            if (start < 100) {
                if (request[i] > start) {
                    left = i - 1;
                    right = i;
                }
            } else {
                if (request[i] >= start) {
                    left = i - 1;
                    right = i;
                }
            }
        }
        i++;
    }

    //Increment numRequest twice to account for CSCAN till end, then wrap
    int *newRequest;

    int queue_index = 0;
    int starting_left = left;
    int starting_right = right;
    if (start < 100) {
        //Going left first

        if (right != numRequest)
            numRequest+= 2;

        newRequest = malloc(numRequest * sizeof(int));

        while (left!=-1) {
            newRequest[queue_index] = request[left];
            left--;
            queue_index++;
        }

        if (right != numRequest) {
            newRequest[queue_index++] = 0;
            newRequest[queue_index++] = 199;

            right = numRequest - 3;
            while(right != starting_left) {
                newRequest[queue_index] = request[right];
                right--;
                queue_index++;
            }
        }
    } else {
        //Going right first

        if (left != -1)
            numRequest+= 2;

        newRequest = malloc(numRequest * sizeof(int));

        // -1 accounts for the addition of the 199 to the queue
        while(right != numRequest - 2) {
            newRequest[queue_index] = request[right];
            right++;
            queue_index++;
        }

        if (left != -1) {
            newRequest[queue_index++] = 199;
            newRequest[queue_index++] = 0;

            left = 0;
            while (left!= starting_right) {
                newRequest[queue_index] = request[left];
                left++;
                queue_index++;
            }
        }
    }
    printf("\n----------------\n");
    printf("CSCAN :");
    printSeqNPerformance(newRequest, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in LOOK
void accessLOOK(int *request, int numRequest)
{
    qsort(request, numRequest, sizeof(int), &cmpfunc);
    int start = START;
    int i = 0;
    int left = -1;
    int right = numRequest;
    while(left == -1 && right == numRequest) {
        if (i == numRequest) {
            left = numRequest - 1;
        } else {
            if (start < 100) {
                if (request[i] > start) {
                    left = i - 1;
                    right = i;
                }
            } else {
                if (request[i] >= start) {
                    left = i - 1;
                    right = i;
                }
            }
        }
        i++;
    }

    int *newRequest = malloc(numRequest * sizeof(int));

    int queue_index = 0;

    if (start < 100) {
        while (left!=-1) {
            newRequest[queue_index] = request[left];
            left--;
            queue_index++;
        }

        while(right != numRequest) {
            newRequest[queue_index] = request[right];
            right++;
            queue_index++;
        }
    } else {
        while(right != numRequest) {
            newRequest[queue_index] = request[right];
            right++;
            queue_index++;
        }

        while (left!=-1) {
            newRequest[queue_index] = request[left];
            left--;
            queue_index++;
        }
    }
    //write your logic here
    printf("\n----------------\n");
    printf("LOOK :");
    printSeqNPerformance(newRequest, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in CLOOK
void accessCLOOK(int *request, int numRequest)
{
    //write your logic here
    printf("\n----------------\n");
    printf("CLOOK :");
    printSeqNPerformance(newRequest,newCnt);
    printf("----------------\n");
    return;
}

int main()
{
    int *request, numRequest, i,ans;

    //allocate memory to store requests
    printf("Enter the number of disk access requests : ");
    scanf("%d", &numRequest);
    request = malloc(numRequest * sizeof(int));

    printf("Enter the requests ranging between %d and %d\n", LOW, HIGH);
    for (i = 0; i < numRequest; i++)
    {
        scanf("%d", &request[i]);
    }

    printf("\nSelect the policy : \n");
    printf("----------------\n");
    printf("1\t FCFS\n");
    printf("2\t SSTF\n");
    printf("3\t SCAN\n");
    printf("4\t CSCAN\n");
    printf("5\t LOOK\n");
    printf("6\t CLOOK\n");
    printf("----------------\n");
    scanf("%d",&ans);

    switch (ans)
    {
        //access the disk location in FCFS
        case 1: accessFCFS(request, numRequest);
            break;

            //access the disk location in SSTF
        case 2: accessSSTF(request, numRequest);
            break;

            //access the disk location in SCAN
        case 3: accessSCAN(request, numRequest);
            break;

            //access the disk location in CSCAN
        case 4: accessCSCAN(request,numRequest);
            break;

            //access the disk location in LOOK
        case 5: accessLOOK(request,numRequest);
            break;

            //access the disk location in CLOOK
        case 6: accessCLOOK(request,numRequest);
            break;

        default:
            break;
    }
    return 0;
}