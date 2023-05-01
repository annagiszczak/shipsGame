CFLAGS=-Wall -pedantic -std=gnu11 -lpthread

PLIK=$1

all:	
	gcc $(CFLAGS) -c $(PLIK).c	#kompiluje 
	gcc $(CFLAGS) -o $(PLIK) $(PLIK).o #linkuje obiekty

clean:
	rm -f $(PLIK).o
