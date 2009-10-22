CFLAGS	=	-g
CC = gcc

default:  all

all:  shp2kml

shape.o: shape.h shape.c
	$(CC) $(CFLAGS) -c shape.c

shapefile.o: shape.o shapefile.c shapefile.h
	$(CC) $(CFLAGS) -c shapefile.c

shp2kml: shape.o shapefile.o shp2kml.c
	$(CC) $(CFLAGS) shp2kml.c shape.o shapefile.o $(LINKOPT) -o shp2kml

clean:
	rm -f *.o *.exe *.gch
