#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv){
    pid_t pid;
    int status;

    if(argc < 2){
        printf("Usage: ./run_command <command> [args...]\n");
        return EXIT_FAILURE;
    }

    pid = fork();
    if(pid == -1){
        perror("fork");
        return EXIT_FAILURE;
    }
    else if(pid == 0){
        execvp(argv[1], &argv[1]);

        perror("execvp");
        _exit(127);
    }
    else{
        pid_t waited_pid = waitpid(pid, &status, 0);
        if(waited_pid == -1){
            perror("waitpid");
            return EXIT_FAILURE;
        }
        printf("[%ld] Child process(%ld) closed!!!\n", (long)getpid(), (long)pid);
    }
    return EXIT_SUCCESS;
}