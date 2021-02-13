#ifndef GESTION_STRUCT_H
#define GESTION_STRUCT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_BUFFER 200
#define T_M 10
#define T_ID 6
#define T_L 4
#define T_H 19
#define T_S 4
#define T_CHAINE_ID 6
#define T_CHAINE_L 7
#define T_CHAINE_H 6
#define T_CHAINE_S 10

/*****Structure*****/
typedef struct bot* ptr_liste;
typedef struct bot{unsigned char* id; unsigned char* life; char* hote; char* service; ptr_liste next;}Liste;

void ajout_tete(ptr_liste*,unsigned char*,unsigned char*, char*, char*);
void supp_liste(ptr_liste*);
void supprimer_tete(ptr_liste*);
int inserer_sans_doublon(ptr_liste*, unsigned char*,unsigned  char*, char*, char*);
char* print_liste(ptr_liste);
char* print_list_compact(ptr_liste);


typedef struct sock* ptr_liste_sock;
typedef struct sock{char* hote; FILE* dialogue; ptr_liste_sock next;}Sock;

void ajout_socket(ptr_liste_sock*,char*,FILE*);
void supprimer_socket(ptr_liste_sock*);


#endif
