CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
EXEC = candc
LIB = -L../thread_lib -L../network_lib -L../ipc_lib -lipc -lnetwork -lthread -lpthread
HNET = ../network_lib
HTHR = ../thread_lib
HIPC = ../ipc_lib

all : $(EXEC)

candc : candc.o gestionStruct.o gestionBot.o
	$(CC) $^ -o $@ $(LIB) $(CFLAGS)

candc.o : candc.c candc.h $(HNET)/libnetwork.h $(HNET)/libnetwork.a $(HIPC)/libipc.h $(HIPC)/libipc.a
	$(CC) -o $@ -c $< $(CFLAGS)

gestionStruct.o: gestionStruct.c gestionStruct.h
	$(CC) -o $@ -c $< $(CFLAGS)

gestionBot.o: gestionBot.c gestionBot.h
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
