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
        else{   //자식의 종료 상태 확인
            if(WIFEXITED(status)){  //일반적인 종료인지 확인하는 매크로
                int exit_status = WEXITSTATUS(status);    //종료 코드 추출
                printf("Child process exited. exit code: %d\n", exit_status);
                return exit_status;     //자식의 종료코드 반환
            }
            else if(WIFSIGNALED(status)){   //시그널에 의한 종료인지 확인하는 매크로
                int signal_num = WTERMSIG(status);   //종료 원인 시그널 번호 추출
                printf("Child process terminated by signal: %d\n", signal_num);
                return 128 + signal_num;    //시그널에 의한 종료이므로 쉘 관례에 따라 128 + 시그널 번호 반환
            }
        }
        // printf("[%ld] Child process(%ld) closed!!!\n", (long)getpid(), (long)pid);
    }
    // return EXIT_SUCCESS; 자식의 종료 상태와 관계없이 성공상태로 종료됨 종료상태와 함께 자식의 종료 코드를 반환하는 방식으로 수정.
}