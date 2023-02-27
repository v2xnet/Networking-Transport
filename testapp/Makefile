all: itsnet_test

itsnet_test: main.o
	gcc -o itsnet_test main.o -lpthread

main.o: main.c
	gcc -o main.o -c main.c -lpthread -W -Wall -ansi -pedantic  -lpthread

clean:
	rm -rf *.o

mrproper: clean
	rm -rf itsnet_test

