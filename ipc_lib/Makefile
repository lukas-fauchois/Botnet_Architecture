CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
EXEC = libipc.a

all : $(EXEC)

libipc.a : libipc.o
	ar r $@ $<

libipc.o : libipc.c libipc.h
	gcc -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)    
