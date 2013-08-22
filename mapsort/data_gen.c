
/*
  * generate some integer for other programs in binary
  *
  */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


int main(int argc, char **argv)
{	
	int i, num = 10; // default number of integer is 10
	int fd;
	int data;
	if (--argc > 0) // if user give a number , accept it 
		num = atoi(argv[1]);
	if (fd = open("sort_data", O_RDWR | O_CREAT | O_TRUNC)) { 
		srand(time(NULL));
		for (i = num; i > 0; i--) {
			data = rand() % 100; // fill random number between 0 and 99
			if(write(fd, &data, sizeof(int)))
				;
		}
		printf("%d integer written to file!\n", num - i);
		close(fd);
		return 0;
	}
	return -1;
}
