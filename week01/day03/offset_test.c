#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define BLOCK_SIZE 300

int main(int argc, char **argv){

	int src_fd;
    int ret;
	char *src_name;
	char buf[4] = {0};
	
	if(argc != 2){
		printf("Usage: ./offset_test <source>\n");
		return EXIT_FAILURE;
	}
	
	src_name = argv[1];
	
	src_fd = open(src_name, O_RDONLY);
	if(src_fd == -1){
		printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
		return EXIT_FAILURE;
	}

    //3바이트 읽기
    ret = read(src_fd, buf, 3);
    if(ret == -1){
		printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
        close (src_fd);
		return EXIT_FAILURE;
	}
    buf[ret] = '\0';
    printf("data: %s\n", buf);
    
    //현재 위치 가져오기
    off_t pos = lseek(src_fd, 0, SEEK_CUR);     
    if (pos == (off_t)-1) {
    perror("lseek");
    }
    printf("offset: %ld\n", (long)pos);

    //2바이트 앞으로 위치 이동, 2바이트 읽기
    pos = lseek(src_fd, 2, SEEK_CUR);           
    if (pos == (off_t)-1) {
    perror("lseek");
    }
    printf("offset: %ld\n", (long)pos);

    ret = read(src_fd, buf, 2);
    if(ret == -1){
		printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
        close (src_fd);
		return EXIT_FAILURE;
	}
    buf[ret] = '\0';
    printf("data: %s\n", buf);

    //2파일 끝에서 3바이트 앞으로 위치 이동, 3바이트 읽기
    pos = lseek(src_fd, -3, SEEK_END);           
    if (pos == (off_t)-1) {
    perror("lseek");
    }
    printf("offset: %ld\n", (long)pos);

    ret = read(src_fd, buf, 3);
    if(ret == -1){
		printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
        close (src_fd);
		return EXIT_FAILURE;
	}
    buf[ret] = '\0';
    printf("data: %s\n", buf);

	close(src_fd);

	return EXIT_SUCCESS;
}