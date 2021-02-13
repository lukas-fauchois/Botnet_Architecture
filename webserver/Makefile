CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
EXEC = server
LIB = -L../thread_lib -L../network_lib -L../ipc_lib -lipc -lnetwork -lthread -lpthread
HNET = ../network_lib
HTHR = ../thread_lib
HIPC = ../ipc_lib

all : $(EXEC)

server : server.o
	$(CC) $< -o $@ $(LIB) $(CFLAGS)

server.o : server.c $(HNET)/libnetwork.h $(HNET)/libnetwork.a $(HIPC)/libipc.h $(HIPC)/libipc.a server.h 
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
