#ifndef LIB_NETWORK_H
#define LIB_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <time.h>
#define MAX_UDP_MESSAGE 50



/*****UDP*****/
int creationSocketUDP(char*, char*, void**);
void envoieMessageUDP(int,unsigned char*, int, void*);
void destructionSocket(int, void*);
int initialisationSocketUDP(char *);
int boucleServeurUDP(int, int (*traitement)(unsigned char *, char*, char*));

/*****TCP*****/
int initialisationServeur(char*, int);
int boucleServeurTCP(int, int (*traitement)(int));
int connexionServeurTCP(char*, char*);



#endif
