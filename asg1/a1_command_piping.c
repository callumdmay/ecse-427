//
// Created by Callum May on 2018-01-28.
//

#include<stdio.h>
#include<unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
    int fds[2];
    pipe(fds);
    if (fork()==0) {
        //Child : execute ls using execvp
        //Close reading part of pipe in child
        close(fds[0]);
        //Redirect stdout to pipe write end
        dup2(fds[1], 1);
        char *argv[2];
        argv[0] = "ls";
        argv[1] = NULL;

        close(fds[1]);
        execvp("ls", argv);
        exit(-1);
    } else {
        //Parent : print output from ls here
        //close writing part of pipe in parent
        close(fds[1]);
        //read from pipe and output to stdout (console)
        char str[4096];
        read(fds[0], str, sizeof(str));
        printf(str);
    }
    return 0;
}
