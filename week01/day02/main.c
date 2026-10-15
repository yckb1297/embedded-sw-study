#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define BLOCK_SIZE 300

int main(int argc, char **argv){

	int src_fd, dst_fd;
	ssize_t byte_read, byte_write;
	ssize_t total_written = 0;
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
	
	dst_fd = open(dst_name, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if(dst_fd == -1){
		close(src_fd);
		printf("ERROR: %s (%d)\n", strerror(errno), __LINE__);
		return EXIT_FAILURE;
	}
	
	while(1){
		byte_read = read(src_fd, buf, sizeof(buf));
		if (byte_read > 0) {
			printf("read: %zd bytes\n",  byte_read);
			total_written = 0;
			
			while(total_written < byte_read){
				ssize_t remaining = byte_read - total_written;
				
				if(BLOCK_SIZE <= remaining){
					//byte_write = write(dst_fd, buf + total_written, byte_read - total_written);
					byte_write = write(dst_fd, buf + total_written, BLOCK_SIZE);
					
					if(byte_write == -1){
						close(src_fd);
						close(dst_fd);
						
						printf("WRITE ERROR: %s (%d)\n",strerror(errno),__LINE__);
						return EXIT_FAILURE;
					}
					else{
						printf("write: %zd bytes\n", byte_write);
						total_written += byte_write;
					}
				}
				else{
					byte_write = write(dst_fd, buf + total_written, remaining);
					if(byte_write == -1){
						close(src_fd);
						close(dst_fd);
						
						printf("WRITE ERROR: %s (%d)\n",strerror(errno),__LINE__);
						return EXIT_FAILURE;
					}
					else{
						printf("write: %zd bytes\n", byte_write);
						total_written += byte_write;
					}
				}
			}
			printf("total written: %zd\n", total_written);
		}
		else if(byte_read == 0){	//EOF
			close(src_fd);
			close(dst_fd);
			return EXIT_FAILURE;		
		}
		else{	//ERROR
			printf("READ ERROR: %s (%d)\n",strerror(errno),__LINE__);
			break;
		}
	}
	close(src_fd);
	close(dst_fd);
	return EXIT_SUCCESS;
}