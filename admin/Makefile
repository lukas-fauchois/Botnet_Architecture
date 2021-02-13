CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
EXEC = admin
LIB =  -L../ipc_lib -lipc

all : $(EXEC)

admin : admin.o
	$(CC) $< -o $@ $(LIB) $(CFLAGS)

admin.o : admin.c
	$(CC) -o $@ -c $< $(CFLAGS)


.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)       
