#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid;
    int ret, status;

    pid = fork();
    if(pid == -1){
        perror("fork");
        return EXIT_FAILURE;
    }
    else if(pid == 0){      //child
        char *args[] = {"ls", "-l", NULL};
        ret = execvp("ls", args);   //execvp는 성공시 반환X, 리턴값을 받지 않고 에러처리 가능.
        if(ret == -1){
            // printf("ERROR: %s(%d)\n", strerror(errno), __LINE__); 버퍼에 남아있다가 _exit() 때문에 출력되지 않을 수 있음. 표준 오류로 출력하는 perror()가 적절
            perror("execvp");
            _exit(127); // 127: 명령 실패를 나타내는 값(관례에 따름)
        }
    }
    else{                   //parent: fork의 반환값으로 자식의 pid를 받음.
        pid_t waited_pid = waitpid(pid, &status, 0);    //자식 프로세스(pid)의 종료를 대기(옵션 0), status에 종료 정보 저장
        if(waited_pid == -1){
            printf("ERROR: %s(%d)\n", strerror(errno), __LINE__);
            return EXIT_FAILURE;
        }
        printf("[%ld]Child process(%ld) finished!!\n", (long)getpid(), (long)waited_pid);
    }
    return EXIT_SUCCESS;
}