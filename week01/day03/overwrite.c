#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv){
    
    int src_fd;
    ssize_t ret;
	char *src_name;
	char *buf;
	
	if(argc != 3){
		printf("Usage: %s <source> <overwriting data>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	src_name = argv[1];
    buf = argv[2];
	
	src_fd = open(src_name, O_RDWR);
	if(src_fd == -1){
		printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
		return EXIT_FAILURE;
	}

    off_t pos = lseek(src_fd, 4, SEEK_SET);     
    if (pos == (off_t)-1) {
        perror("lseek");
        close(src_fd);
        return EXIT_FAILURE;
    }

    ret = write(src_fd, buf, strlen(buf));
    if(ret == -1){
        close(src_fd);

        printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
        return EXIT_FAILURE;
    }
    printf("byte_writen: %zd\n", ret);

    close(src_fd);

}