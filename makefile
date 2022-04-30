# Makefile
all: shell-app diskimage inject bootloader stdlib fs_utils bin move shell kernel

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
	bcc -ansi -c -o out/fs_utils.o src/c/fs_utils.c

move:
	mv src/c/bin/*.o out/

shell:
	bcc -ansi -c -o out/shell_kernel.o src/c/shell_kernel.c
# out/kernel.o out/kernel_asm.o out/std_lib.o out/bin.o

injector:
	gcc src/injector.c tc_gen/tc_lib -o out/injector

inject: injector
	cd out && ./injector system.img

inject-a:
	mv out/system.img tc_gen
	cd tc_gen && \
	./tester A && \
	cd ..
	mv tc_gen/system.img out

inject-b:
	mv out/system.img tc_gen
	cd tc_gen && \
	./tester B && \
	cd ..
	mv tc_gen/system.img out

inject-c:
	mv out/system.img tc_gen
	cd tc_gen && \
	./tester C && \
	cd ..
	mv tc_gen/system.img out

inject-d:
	mv out/system.img tc_gen
	cd tc_gen && \
	./tester D && \
	cd ..
	mv tc_gen/system.img out

run:
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	nasm -f as86 src/asm/interrupt.asm -o out/lib_interrupt.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o out/lib_interrupt.o out/shell_kernel.o out/mv.o out/cat.o out/cd.o out/ls.o out/mkdir.o out/printCWD.o out/args_split.o out/fs_utils.o out/std_lib.o
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1
	bochs -f src/config/if2230.config

build-run: all run