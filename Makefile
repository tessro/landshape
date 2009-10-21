CFLAGS	=	-g
CC = gcc

default:  all

all:  test

shape.o: shape.h
	$(CC) $(CFLAGS) -c shape.c

shapefile.o: shape.o
	$(CC) $(CFLAGS) -c shapefile.c

test: shape.o shapefile.o
	$(CC) $(CFLAGS) main.c shape.o shapefile.o $(LINKOPT) -o test

clean:
	rm -f *.o *.exe *.gch
