
KER_DIR ?= /lib/modules/$(shell uname -r)/build

obj-m := kp.o
all:
	make -C $(KER_DIR) M=$(shell pwd) modules
