CC = gcc
CFLAGS = -Wall -Wextra
EXEC = libcharge.so
OPT = -shared

all : $(EXEC)

libcharge.so : libcharge.o
	$(CC) -o $@ $(OPT) $<

libcharge.o : libcharge.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o *.so

mrproper: clean
	rm -rf $(EXEC)
