
all: multi myfile

myfile: myfile.o
	gcc -m32 -o myfile myfile.o
myfile.o: myfile.asm
	nasm -f elf32 myfile.asm -o myfile.o
multi: multi.o
	gcc -m32 -o multi multi.o
multi.o: multi.s
	nasm -f elf32 multi.s -o multi.o
.PHONY: clean
clean:
	rm -f multi.o multi myfile