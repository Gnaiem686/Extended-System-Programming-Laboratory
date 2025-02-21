all: task2


task2: start.o startup.o
	gcc -m32 -c task2.c -o task2.o
	ld -o task2 task2.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

start.o:
	nasm -g -f elf32 -o start.o start.s

startup.o:
	nasm -g -f elf32 -o startup.o startup.s

clean:
	rm -f *.o task2