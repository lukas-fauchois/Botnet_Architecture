#include "libnetwork.h"

/**********************/
/*****Emission UDP*****/
/**********************/

int creationSocketUDP(char *hote,char *service, void** handle){
	struct addrinfo precisions,*resultat,*origine;
	int statut;
	int s;

  /* Creation de l'adresse de socket */
	memset(&precisions,0,sizeof precisions);

	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_DGRAM;

	statut=getaddrinfo(hote,service,&precisions,&origine);
	if(statut<0){ perror("messageUDPgenerique.getaddrinfo"); exit(EXIT_FAILURE); }

	struct addrinfo *p;

	for(p=origine,resultat=origine ; p!=NULL ; p=p->ai_next)
		if(p->ai_family==AF_INET6){resultat=p; break;}

  /* Creation d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
  if(s<0){ perror("messageUDPgenerique.socket"); exit(EXIT_FAILURE); }

  *handle=malloc(sizeof(struct addrinfo));
  *(struct addrinfo*)*handle=*resultat;

  /* Option sur la socket */
  int vrai=1;

  if(setsockopt(s,SOL_SOCKET,SO_BROADCAST,&vrai,sizeof(vrai))<0){
    perror("initialisationServeurUDPgenerique.setsockopt (BROADCAST)");
    exit(-1);

  	/* Liberation de la structure d'informations */
    freeaddrinfo(origine);
  }

  *handle = malloc(sizeof(struct addrinfo));
  *(struct addrinfo*)*handle=*resultat;

	return s;
}

void envoieMessageUDP(int s,unsigned char *message,int taille, void * handle){
	/* Envoi du message */
	struct addrinfo * resultat = handle;
	int nboctets = sendto(s,message,taille,0,resultat->ai_addr,resultat->ai_addrlen);
	if(nboctets<0){ perror("messageUDPgenerique.sento"); exit(EXIT_FAILURE); }
}

void destructionSocket(int s,void * handle){
	/* Fermeture de la socket d'envoi */
	free(handle);
	close(s);
}


/***********************/
/*****Réception UDP*****/
/***********************/

/*********Fonction pour initialiser le serveur UDP*********/
int initialisationSocketUDP(char *service){
	struct addrinfo precisions,*resultat,*origine;
	int statut;
	int s;

	/* Construction de la structure adresse */
	memset(&precisions,0,sizeof precisions);

	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_DGRAM;
	precisions.ai_flags=AI_PASSIVE;

	statut=getaddrinfo(NULL,service,&precisions,&origine);
	if(statut<0){ perror("initialisationSocketUDP.getaddrinfo"); exit(EXIT_FAILURE); }

	struct addrinfo *p;

	for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
  	if(p->ai_family==AF_INET6){ resultat=p; break; }

	/* Creation d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
	if(s<0){ perror("initialisationSocketUDP.socket"); exit(EXIT_FAILURE); }

	/* Options utiles */
	int vrai=1;
	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
  	perror("initialisationServeurUDPgenerique.setsockopt (REUSEADDR)");
  	exit(-1);
  }

	/* Specification de l'adresse de la socket */
	statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
	if(statut<0){ perror("initialisationServeurUDP.bind"); exit(-1); }

	/* Liberation de la structure d'informations */
	freeaddrinfo(origine);

	return s;
}

/********** Fonction pour ecouter en UDP *********/
int boucleServeurUDP(int s,int (*traitement)(unsigned char *,char*,char*)){
	while(1){
  		struct sockaddr_storage adresse;
  		socklen_t taille=sizeof(adresse);
  		unsigned char message[MAX_UDP_MESSAGE];

  		int nboctets=recvfrom(s,message,MAX_UDP_MESSAGE,0,(struct sockaddr *)&adresse,&taille);

	   	if(nboctets<0) return -1;

		char* hote = malloc(NI_MAXHOST);
  		char* service = malloc(NI_MAXSERV);

		if(hote==NULL || service==NULL){ perror("socketToClient.malloc"); exit(EXIT_FAILURE); }

		getnameinfo((struct sockaddr*)&adresse,taille,hote,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICHOST|NI_NUMERICSERV);
		if(traitement(message,hote,service)<0){
			free(hote);
			free(service);
			break;
	}

  	free(hote);
  	free(service);
	}

	return 0;
}




/*********************/
/*****Serveur TCP*****/
/*********************/

/**********Fonction pour initialiser le serveur**********/
int initialisationServeur(char *service,int connexions){
	struct addrinfo precisions,*resultat,*origine;
	int statut;
	int s;

	/* Construction de la structure adresse */
	memset(&precisions,0,sizeof precisions);

	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_STREAM;
	precisions.ai_flags=AI_PASSIVE;

	statut=getaddrinfo(NULL,service,&precisions,&origine);
	if(statut<0){ perror("initialisationServeur.getaddrinfo"); exit(EXIT_FAILURE); }

	struct addrinfo *p;

	for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
  	if(p->ai_family==AF_INET6){ resultat=p; break; }

	/* Creation d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
	if(s<0){ perror("initialisationServeur.socket"); exit(EXIT_FAILURE); }

	/* Options utiles */
	int vrai=1;
	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
		perror("initialisationServeur.setsockopt (REUSEADDR)");
		exit(EXIT_FAILURE);
	}

	if(setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai))<0){
		perror("initialisationServeur.setsockopt (NODELAY)");
		exit(EXIT_FAILURE);
	}

	/* Specification de l'adresse de la socket */
	statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
	if(statut<0) return -1;

	/* Liberation de la structure d'informations */
	freeaddrinfo(origine);

	/* Taille de la queue d'attente */
	statut=listen(s,connexions);
	if(statut<0) return -1;

	return s;
}

/********** Fonction pour ecouter en TCP ***********/
int boucleServeurTCP(int ecoute,int (*traitement)(int)){
	int dialogue;

	while(1){
  	/* Attente d'une connexion */
    if((dialogue=accept(ecoute,NULL,NULL))<0) return -1;

    /* Passage de la socket de dialogue a la fonction de traitement */
    if(traitement(dialogue)<0){ shutdown(ecoute,SHUT_RDWR); return 0;}
	}
}



/********************/
/*****Client TCP*****/
/********************/

/*********Fonction pour initialiser le client********/
int connexionServeurTCP(char *hote,char *service){
	struct addrinfo precisions,*resultat,*origine;
	int statut;
	int s;

	/* Creation de l'adresse de socket */
	memset(&precisions,0,sizeof precisions);

	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_STREAM;

	statut=getaddrinfo(hote,service,&precisions,&origine);
	if(statut<0){ perror("connexionServeur.getaddrinfo"); return -1; }

	struct addrinfo *p;

	for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
  	if(p->ai_family==AF_INET6){ resultat=p; break; }

	/* Creation d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
	if(s<0){ perror("connexionServeur.socket"); return -1; }

	/* Connection de la socket a l'hote */
	if(connect(s,resultat->ai_addr,resultat->ai_addrlen)<0) return -1;

	/* Liberation de la structure d'informations */
	freeaddrinfo(origine);

	return s;
}
