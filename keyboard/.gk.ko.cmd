cmd_/root/kernel-misc/keyboard/gk.ko := ld -r -m elf_x86_64 -T /work1/linux-3.8.6.test/scripts/module-common.lds --build-id  -o /root/kernel-misc/keyboard/gk.ko /root/kernel-misc/keyboard/gk.o /root/kernel-misc/keyboard/gk.mod.o