#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct res* ptr_cellule;
typedef struct res{int id_result; char* name_result;int size_result;ptr_cellule suivant;}Resultat;

void ajout_tete(ptr_cellule* ,int, char*, int );
int inserer(ptr_cellule*, int, char*, int);
void supprimer_tete(ptr_cellule*);
void supp_liste(ptr_cellule*);

#endif
