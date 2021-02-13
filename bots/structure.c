#include "structure.h"

/**************************************************/
/***Fonction pour manipuler la liste de résultat***/
/**************************************************/

void ajout_tete(ptr_cellule* l, int id, char* name, int size){
    ptr_cellule new = (ptr_cellule)malloc(sizeof(Resultat));
    new->id_result = id;
    char* n = malloc(50*sizeof(char));
    strcpy(n,name);
    new->name_result = n;
    new->size_result = size;
    new->suivant = *l;
    *l = new;
}

int inserer(ptr_cellule* l, int id, char* name, int size){
    if(*l==NULL){
        ajout_tete(l,id,name,size);
        return 0;
    }
    else if(id<(*l)->id_result){
        ajout_tete(l,id,name,size);
        return 0;
    }
    else inserer((&(*l)->suivant),id,name,size);

    return 0;

}

void supprimer_tete(ptr_cellule* l){
    ptr_cellule stock=*l;
    remove((*l)->name_result);
    *l=(*l)->suivant;
    free (stock);

}

void supp_liste(ptr_cellule* l){
    while(*l!=NULL)
        supprimer_tete(l);

}
