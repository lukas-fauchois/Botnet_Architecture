CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
EXEC = bots
LIB = -L../thread_lib -L../network_lib -lnetwork -lthread -lpthread
HNET = ../network_lib
HTHR = ../thread_lib
OPT1 = -rdynamic
OPT2 = -ldl


all : $(EXEC)

bots : bots.o structure.o gestionCandc.o
	$(CC) $(OPT1) $^ -o $@ $(LIB) $(OPT2)

bots.o : bots.c bots.h
	$(CC) -o $@ -c $< $(CFLAGS)

stucture.o : structure.c structure.h
	$(CC) -o $@ -c $< $(CFLAGS)

gestionCandc.o : gestionCandc.c gestionCandc.h
	$(CC) -o $@ -c $< $(CFLAGS)



.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)



