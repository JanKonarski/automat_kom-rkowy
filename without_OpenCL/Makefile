CC = gcc
CFLAGS = -g -O2
LIBS = -lpng -lz

INCLUDE =\
	engine.h \
	strs.h

OBJ =\
	aCelular.o \
	engine.o \
	strs.o


all: main

clean:
	@rm -f *.o
	@rm -f *.gch

.c.o:
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

main: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o aCelular