#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//define des samaphores
#define LISTE_RESULTAT 0
#define ID_RESULTAT 1
#define REDIRECTION 2
#define NB_MUTEX 10

typedef struct{
	void(*fonction)(void*);
	void* parametre;
} strct_intermediaire;

void* lancementFonction(void*);

void lancerFlux(void(*fonction)(void*), void*, int);

void P(int);

void V(int);
