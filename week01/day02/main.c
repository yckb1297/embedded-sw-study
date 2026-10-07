#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv){

	int src_fd, dst_fd, byte_read, byte_write;
	char *src_name, *dst_name;
	char buf[1024];

	if(argc != 3){
		printf("Usage: ./copy <source> <destination>\n");
		return EXIT_FAILURE;
	}
	
	src_name = argv[1];
	dst_name = argv[2];

	src_fd = open(src_name, O_RDONLY);
	if(src_fd == -1){
		printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
		return EXIT_FAILURE;
	}

	dst_fd = open(dst_name, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(dst_fd == -1){
		close(src_fd);
		printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
		return EXIT_FAILURE;
	}

	for(;;){
	
		byte_read = read(src_fd, buf, sizeof(buf));
	
		if(byte_read > 0){
	
			byte_write = write(dst_fd, buf, byte_read);
	
			if(byte_write == -1){
	
				close(src_fd);
				close(dst_fd);
	
				printf("ERROR: %s (%d)\n",strerror(errno),__LINE__);
	
				return EXIT_FAILURE;
			}
		}
		else if(byte_read == 0){
	
			break;
		}
		else{
	
			close(src_fd);
			close(dst_fd);
	
			printf("ERROR: %s (%d)\n",strerror(errno),__LINE__);
	
			return EXIT_FAILURE;
		}
	}

	close(src_fd);
	close(dst_fd);

	return EXIT_SUCCESS;
}