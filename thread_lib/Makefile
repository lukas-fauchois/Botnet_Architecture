CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
EXEC = libthread.a

all : $(EXEC)

libthread.a : libthread.o
	ar r $@ $<

libthread.o : libthread.c libthread.h
	gcc -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

