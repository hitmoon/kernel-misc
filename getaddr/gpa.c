#define __KERNEL__
#include <linux/unistd.h>
#include <linux/types.h>
#include <errno.h>
#include <stdio.h>

_syscall2(long,getphyaddr,pid_t,pid,void*,p)

// convert a hexadecimal string to unsigned long
unsigned long hex_to_long(char *p)
{
	unsigned long ret = 0;
	char tmp;

	while(*p) {
		tmp = *p;
		if (tmp >= 'a' && tmp <= 'f')
			tmp = 10 + tmp - 'a';
		else if (tmp >= 'A' && tmp <='F')
			tmp = 10 + tmp - 'A';
		else 
			tmp = tmp - '0';
		ret = ret * 16 + tmp;
		p++;
	}

	return ret;

}

int main(int argc, char **argv)
{
	unsigned long addr, phyaddr;
	unsigned long pid;
	int data = 0x1f13; // data for verify the physcial address

	if (argc < 3) { // need arguments
		printf("Usage: getphyaddr <pid> <vaddr>\n");
		return -1;
	}
	pid = atol(argv[1]);
	addr = hex_to_long(argv[2]);

	// get the physcial address through our system call: getphyaddr
	phyaddr = getphyaddr(pid, (void*)addr);

	printf("linear address: %p ---> physicial address: %p\n", addr, phyaddr);
	
	// verify the phyaddress by check the content 
	printf("confirm address:\n");
	phyaddr = getphyaddr(getpid(), &data); // data's physcial address
	printf("linear address: %p ---> physicial address: %p, data is %x\n", &data, phyaddr, data);

	confirm(phyaddr, sizeof(int));

	return 0;
}
