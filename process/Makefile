
KER_DIR ?= $(KERNEL)
PWD=$(shell pwd)

obj-m := sp.o
all:
	make -C $(KER_DIR) M=$(PWD) modules
