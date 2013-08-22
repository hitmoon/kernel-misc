#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


/* 
   * map a file into memory and sort the data
   *
   */


// comparison function
int my_cmp(const void *a, const void *b)
{
	return *((int*)a) > *((int*)b);

}


int main(int argc, char **argv)
{

	int *buf;
	int fd;
	int i, size, num;

	if (--argc < 1) { // receive the data file to be sort
		printf("Usage: mapsort <data file name>\n");
		exit(-1);
	}

	if ((fd = open(argv[1], O_RDWR)) > 0) { // open ok, continue
		size = lseek(fd, 0, SEEK_END);
		num = size / sizeof(int); // get the number of integers 

		lseek(fd, 0, SEEK_SET);
		if(buf = mmap(0, size, PROT_WRITE, MAP_PRIVATE, fd, 0)) { // do a mmap
			qsort((void*)buf, num, sizeof(int), my_cmp); // use qsort perform a sort
			for(i = 0; i < num; i++)
				printf("%d ", buf[i]);
			printf("\n");
			return 0;

		}
		return -1;

	}
	else { // exit
		printf("can not open binary data %s\n", argv[1]);
		exit(0);
	}

}
