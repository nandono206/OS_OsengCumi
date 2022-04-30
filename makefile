# Makefile
all: shell-app diskimage bootloader stdlib fs_utils bin move shell kernel

# Recipes
clear: 
	rm out/*.o

diskimage:
	dd if=/dev/zero of=out/system.img bs=512 count=2880

bootloader:
	nasm src/asm/bootloader.asm -o out/bootloader
	dd if=out/bootloader of=out/system.img bs=512 count=1 conv=notrunc

shell-app:
	bcc -ansi -c -o out/shell.o src/c/shell.c
	bcc -ansi -c -o out/textio.o src/c/textio.c
	nasm -f as86 src/asm/interrupt.asm -o out/lib_interrupt.o
	ld86 -o out/shell -d out/shell.o out/lib_interrupt.o out/textio.o


kernel:
	bcc -ansi -c -o out/kernel.o src/c/kernel.c

libraries:
	bcc -ansi -c -o out/string.o src/c/string.c
	bcc -ansi -c -o out/fileio.o src/c/fileio.c
	bcc -ansi -c -o out/program.o src/c/program.c
	bcc -ansi -c -o out/textio.o src/c/textio.c

stdlib:
	bcc -ansi -c -o out/std_lib.o src/c/std_lib.c

bin:
	bcc -ansi -c src/c/bin/*.c

fs_utils:
	bcc -ansi -c -o out/filesystem_utils.o src/c/filesystem_utils.c

move:
	mv src/c/bin/*.o out/

shell:
	bcc -ansi -c -o out/shell_control.o src/c/shell_control.c
# out/kernel.o out/kernel_asm.o out/std_lib.o out/bin.o

run:
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	nasm -f as86 src/asm/interrupt.asm -o out/lib_interrupt.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o out/lib_interrupt.o out/shell_control.o out/mv.o out/cat.o out/cd.o out/ls.o out/mkdir.o out/printCWD.o out/args_split.o out/fs_utils.o out/std_lib.o
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1
	bochs -f src/config/if2230.config

build-run: all run
