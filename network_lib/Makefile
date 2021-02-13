CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = 
EXEC = libnetwork.a

all : $(EXEC)

libnetwork.a : libnetwork.o
	ar r $@ $<

libnetwork.o : libnetwork.c libnetwork.h
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
