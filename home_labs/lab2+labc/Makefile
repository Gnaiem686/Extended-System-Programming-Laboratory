
all:  myshell looper mypipeline 

myshell : myshell.o LineParser.o
	gcc -m32 -g -Wall -o myshell myshell.o LineParser.o

myshell.o: myshell.c
	gcc -m32 -g -Wall -c -o myshell.o myshell.c
mypipeline : mypipeline.o 
	gcc -m32 -g -Wall -o mypipeline mypipeline.o 

mypipeline.o: mypipeline.c
	gcc -m32 -g -Wall -c -o mypipeline.o mypipeline.c

LineParser.o : LineParser.c
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 

looper: looper.c
	gcc -g -m32 looper.c -o looper

.PHONY: clean

clean:
	rm -f *.o myshell looper mypipeline

