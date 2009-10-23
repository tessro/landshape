CFLAGS	=	-g
CC = gcc

default:  all

all:  shp2kml dbf2csv

shape.o: shape.h shape.c
	$(CC) $(CFLAGS) -c shape.c

dbf.o: dbf.h dbf.c
	$(CC) $(CFLAGS) -c dbf.c

shapefile.o: shape.o shapefile.c shapefile.h
	$(CC) $(CFLAGS) -c shapefile.c

mainfile.o: shape.o mainfile.c mainfile.h
	$(CC) $(CFLAGS) -c mainfile.c

indexfile.o: shape.o indexfile.c indexfile.h
	$(CC) $(CFLAGS) -c indexfile.c

dbf2csv: dbf2csv.c dbf.o
	$(CC) $(CFLAGS) dbf2csv.c dbf.o $(LINKOPT) -o dbf2csv

shp2kml: shape.o mainfile.o shp2kml.c
	$(CC) $(CFLAGS) shp2kml.c shape.o mainfile.o $(LINKOPT) -o shp2kml

clean:
	rm -f *.o *.exe *.gch
