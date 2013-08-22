#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/unistd.h>
#include <sys/mman.h>


void confirm(unsigned long addr, unsigned len)
{
	int fd, i;
	unsigned char *buf;

	if (addr < 0 || len < 0 || len > (~0)) {
		printf("invalid address!\n");
		return ;
	}

	fd = open("/dev/mem", O_RDONLY);
	if (fd > 0) {
		buf = mmap(0, len, PROT_READ, MAP_SHARED, fd, addr & ~0xfff);
		if (buf) {
			buf += addr & 0xfff;
			printf("data start at %p:\n", (void*)addr);
			for (i = 0; i < len; i++) {
				printf("0x%0.2x ", buf[i]);
			}
			printf("\n");
		}
	}
	close(fd);
	return;
}
