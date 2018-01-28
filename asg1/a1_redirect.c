//
// Created by Callum May on 2018-01-27.
//

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main() {
    int fd = open("redirect_out.txt", O_RDWR| O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP);
    int std_out = dup(1);
    printf("First: Print to stdout\n");
    dup2(fd, 1);
    printf("Second: Print to redirect_out.txt\n");
    close(fd);
    dup2(std_out, 1);
    printf("Third: Print to stdout\n");
    return 0;
}