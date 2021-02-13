#include "libipc.h"

//Création du masque pour SIGINT
void initSIGINT(sigset_t ens){
  sigemptyset(&ens); //vide ensemble
  sigfillset(&ens); //ajoute tous les signaux
  sigdelset(&ens,SIGINT); //enlève SIGINT de la liste
  sigprocmask(SIG_SETMASK,&ens,NULL); //crée le mask
	sigaction(SIGINT,&action,NULL);
}

/*************/
/*****IPC*****/
/*************/

//Création file de message
int FileIPC_create(key_t key){
  int msgid;

  if((msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0660)) == -1) {
    if(errno == EEXIST){
      perror("IPC thread already exist\n");
      return -1;
    }
    else{
      perror("Message thread creation failed\n");
      return -1;
    }
  }
  return msgid;
}

//Récupération file de message
int FileIPC_get(key_t key){
  int msgid;

  if((msgid = msgget(key, 0)) == -1) {
      perror("Message thread recuperation failed\n");
      return -1;
  }
  return msgid;
}

//Destruction file de message
void destructionFileIPC(int msgid){
  msgctl(msgid, IPC_RMID, 0);
  printf("\nIPC thread (id : %d) destroyed\n", msgid);
}

//Envoi d'un message
void envoiIPC(int msgid, long type, char * text){
  data_t command;
  command.type = type;
  strcpy(command.buf, text);

  if(msgsnd(msgid, &command, SIZE_BUF, 0) == -1){
    printf("Message not sent, error\n");
    exit(1);
  }
  printf("Message sent (type = %ld) : %s\n", command.type, command.buf);
}

//Réception d'un message
data_t receptionIPC(int msgid, long type){
  data_t msg;
  if(msgrcv(msgid, &msg, SIZE_BUF, type,MSG_NOERROR) == SIZE_BUF ){
    printf("Message received (type = %ld) : %s\n", msg.type, msg.buf);
  }
  return msg;
}

/***********************/
/*****Shared memory*****/
/***********************/

//faire un script shell pour supprimer lors des tests

int createSHM(key_t key, size_t length){
  int shmid;
  if((shmid = shmget(key, length, IPC_CREAT|IPC_EXCL|0666)) == -1) perror("createSHM.shmget");
  return shmid;
}

void* attachSHM(int shmid){
  void* addr = NULL;
  if((addr = shmat(shmid, 0, 0)) == (void*)(-1)) perror("attachSHM.shmat");
  return addr;
}

int detachSHM(void* addr){
  int det;
  if((det = shmdt(addr)) == -1) perror("detachSHM.shmdt");
  return det;
}

int deleteSHM(int shmid){
  int del;
  if((del = shmctl(shmid, IPC_RMID, NULL)) == -1) perror("deleteSHM.shmctl");
  return del;
}

void writeSHM_unsigned_char(key_t key, unsigned char* data, int length){
  int shmid;
  if((shmid = shmget(key, length, 0666)) == -1){
    perror("writeSHM.shmget");
  }
  else{
    unsigned char* addr;
    addr = (unsigned char*)attachSHM(shmid);
    for(int i = 0 ; i<length ; i++)
      *(addr+i) = *(data+i);
    detachSHM(addr);
  }
}

unsigned char* readSHM_unsigned_char(key_t key, int length){
  int shmid;
  unsigned char* pt = NULL;
  if((shmid = shmget(key, length, 0666)) == -1){
    perror("readSHM.shmget");
    printf("%d\n", key);
  }
  else{
    pt = (unsigned char*)attachSHM(shmid);
  }
  return pt;
}

void writeSHM_int(key_t key, int* data, int length){
  int shmid;
  if((shmid = shmget(key, length, 0666)) == -1){
    perror("writeSHM.shmget");
  }
  else{
    int* addr;
    addr = (int*)attachSHM(shmid);
    for(int i = 0 ; i<length ; i++)
      *(addr+i) = *(data+i);
    detachSHM(addr);
  }
}

int* readSHM_int(key_t key, int length){
  int shmid;
  int* pt = NULL;
  if((shmid = shmget(key, length, 0666)) == -1){
    perror("readSHM.shmget");
    printf("%d\n", key);
  }
  else{
    pt = (int*)attachSHM(shmid);
  }
  return pt;
}
