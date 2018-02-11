/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/

//Please enter your name and McGill ID below
//Name: Callum May
//McGill ID: 260564523

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