#include "gestionStruct.h"

//Fonction concernant la liste de bot
void ajout_tete(ptr_liste* l, unsigned char* id, unsigned char* life, char* hote, char* service){
    ptr_liste new = (ptr_liste)malloc(sizeof(Liste));
    new->id = malloc(T_ID);
    memcpy(new->id,id,T_ID);
    new->life = malloc(T_L);
    memcpy(new->life,life,T_L);
    new->hote = malloc(T_H);
    memcpy(new->hote,hote,T_H);
    new->service=malloc(T_S);
    memcpy(new->service,service,T_S);
    new->next = *l;
    *l = new;
}

int inserer_sans_doublon(ptr_liste* l, unsigned char* id, unsigned char* life, char* hote, char* service){
    if(*l==NULL){
        ajout_tete(l,id,life,hote,service);
        return 0;
    }
    else if(memcmp(id,(*l)->id,6)==0){
        memcpy((*l)->life,life,4);
        return 0;
    }
    else inserer_sans_doublon((&(*l)->next),id,life,hote,service);

    return 0;
}

void supprimer_tete(ptr_liste* l){
    ptr_liste stock=*l;
    *l=(*l)->next;
    free (stock);
}

void supp_liste(ptr_liste* l){
    while(*l!=NULL)
        supprimer_tete(l);

}

char* print_liste(ptr_liste l){
    char *list=malloc(MAX_BUFFER);
    char id[T_ID*2+6];
    //char life[T_L*2+4];
    char life[T_L+2];
    int li;
    memcpy(&li,l->life,T_L);
    sprintf(life,"%d",li);
    strcat(life,"s\n");
    int taille_life=strlen(life);

    memcpy(list,"\n\nID: ",T_CHAINE_ID);
    for(int i=0;i<T_ID;i++) sprintf(id+i*3,"%02x ",l->id[i]);
    memcpy(list+T_CHAINE_ID,id,T_ID*3);
    memcpy(list+T_CHAINE_ID+T_ID*3,"\nLIFE: ",T_CHAINE_L);
    memcpy(list+T_CHAINE_ID+T_ID*3+T_CHAINE_L,life,taille_life);
    memcpy(list+T_CHAINE_ID+T_ID*3+T_CHAINE_L+taille_life,"Hote: ",T_CHAINE_H);
    memcpy(list+T_CHAINE_ID+T_ID*3+T_CHAINE_L+taille_life+T_CHAINE_H,l->hote,T_H);
    memcpy(list+T_CHAINE_ID+T_ID*3+T_CHAINE_L+taille_life+T_CHAINE_H+T_H,"\n\n",2);
    return list;
}

char* print_list_compact(ptr_liste l){
    char *list=malloc(MAX_BUFFER);
    char id[T_ID*2];
    char life[T_L*2];
    int li;
    memcpy(&li,l->life,T_L);
    sprintf(life,"%d",li);
    int taille_life=strlen(life);

    for(int i=0;i<T_ID;i++) sprintf(id+i*2,"%02x",l->id[i]);
    memcpy(list,id,T_ID*2);
    memcpy(list+T_ID*2,",",1);
    memcpy(list+T_ID*2+1,life,taille_life);
    memcpy(list+T_ID*2+1+taille_life,",",1);
    memcpy(list+T_ID*2+1+taille_life+1,l->hote,T_H);
    memcpy(list+T_ID*2+1+taille_life+1+T_H,";",1);
    return list;
}

//Fonction concernant la liste de socket de connexion TCP
void ajout_socket(ptr_liste_sock* l, char* hote, FILE* dialogue){
  ptr_liste_sock new = (ptr_liste_sock)malloc(sizeof(Sock));
  new->hote = malloc(T_H);
  memcpy(new->hote,hote,T_H);
  new->dialogue=dialogue;
  new->next = *l;
  *l = new;
}

void supprimer_socket(ptr_liste_sock* l){
    ptr_liste_sock stock=*l;
    fclose((*l)->dialogue);
    *l=(*l)->next;
    free (stock);
}
