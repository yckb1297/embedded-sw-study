#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
    printf("Before fork\n");

    pid_t pid = fork();

    if(pid == -1)       //error
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    else if(pid == 0)   //child
    {
        printf("[Child]\n");
        printf("PID: %ld\n", (long)getpid());
        printf("PPID: %ld\n", (long)getppid());
        printf("Fork return = %ld\n", (long)pid);
    }
    else                //parent
    {
        printf("[PARENT]\n");
        printf("PID: %ld\n", (long)getpid());
        printf("PPID: %ld\n", (long)getppid());
        printf("Fork return = %ld\n", (long)pid);
    }

    printf("After fork\n");
    return EXIT_SUCCESS;
}
