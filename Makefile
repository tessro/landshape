CFLAGS	=	-g
CC = gcc

default:  all

all:  test

shape.o: shape.h shape.c
	$(CC) $(CFLAGS) -c shape.c

shapefile.o: shape.o shapefile.c shapefile.h
	$(CC) $(CFLAGS) -c shapefile.c

test: shape.o shapefile.o main.c
	$(CC) $(CFLAGS) main.c shape.o shapefile.o $(LINKOPT) -o test

clean:
	rm -f *.o *.exe *.gch
