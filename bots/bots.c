#include "bots.h"

int idResultat = 0;
int diffTemps = 0;
ptr_cellule listResult = NULL;
struct sigaction action;

void handler(int sig){
  if(sig == SIGINT){
		//ici on supprime tous les resultats
		if(listResult!=NULL)
			supp_liste(&listResult);
  	exit(EXIT_SUCCESS);

	}
}


int gestionClient(int s){
/* recuperation de la socket de dialogue et lancement thread */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

 	lancerFlux(charge,(void*)&dialogue,sizeof(dialogue));

	return 0;
}

void sendUDP(){
	clock_t t1,t2;
  t1=clock();
	void *handle;
	char *hote="255.255.255.255";
	unsigned char message[T_M]={0x48,0xa4,0x72,0xc1,0x6e,0x5c,0,0,0,0};
	char *service="4242";

	int socket = creationSocketUDP(hote,service,&handle);

	while(1){
		sleep(1);
		t2=clock();
		diffTemps=(int)(t2-t1);
		diffTemps/=CLOCKS_PER_SEC;
		memcpy(message+T_ID,&diffTemps,T_L);
		envoieMessageUDP(socket,(unsigned char*)message,sizeof(message),handle);
	}
}

void ecouteTCP(){
	int s;
  char *service="4242";

  /* Initialisation du serveur */
  s=initialisationServeur(service,MAX_CONNEXIONS);

  /* Lancement de la boucle d'ecoute */
  boucleServeurTCP(s,gestionClient);
}

int main(){

	lancerFlux(sendUDP,NULL,0);
	lancerFlux(ecouteTCP,NULL,0);
	action.sa_handler = handler;
	sigaction(SIGINT, &action, NULL);
	while(1);


	return 0;
}
