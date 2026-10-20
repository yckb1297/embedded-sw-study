#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int value = 100;

int main(void)
{
    //int value = 100;

    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        return EXIT_FAILURE;
    }
    else if(pid == 0){  //child
        value += 10;
        printf("[child] pid: %ld value: %d\n", (long)getpid(), value);
    }
    else{       //parent
        value += 20;
        printf("[parent] pid: %ld value: %d\n", (long)getpid(), value);
    }

    return EXIT_SUCCESS;
}