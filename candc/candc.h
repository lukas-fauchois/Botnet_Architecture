#ifndef CANDC_H
#define CANDC_H

//Definition des fichiers d'inclusions
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "../network_lib/libnetwork.h"
#include "../thread_lib/libthread.h"
#include "../ipc_lib/libipc.h"
#include "gestionStruct.h"
#include "gestionBot.h"

//Définitions de constantes
#define MAX_LIGNE 10
#define MAX_CONNEXIONS 20
#define MAX_TAMPON 200
#define T_CHARGE 30
#define T_IP 16
#define T_NAME_RESULT 30
#define T_CHEMIN 50
#define MAX_LIST 1024

//Fonctions de la librairie IPC
int createSHM(key_t, size_t);
int detachSHM(void*);
int deleteSHM(int);
void writeSHM_unsigned_char(key_t, unsigned char*, int);
unsigned char* readSHM_unsigned_char(key_t, int);
void writeSHM_int(key_t, int*, int);
int* readSHM_int(key_t, int);

//Fonctions internes
int traitement(unsigned char *, char*, char*);
void manageSHM();

//Files IPC (admin  et c&c)
int msgid2, msgid1;

//SHM
int shmid_candc_pid;
int shmid_bot_list_length;
int shmid_bot_list;
int shmid_installation;
int shmid_running;
int shmid_disconnect;
int shmid_delete;
int shmid_result_length;
int shmid_result;
int shmid_status_length;
int shmid_status;

int received_SHM;
pid_t* server_pid;

struct sigaction action;

struct arg{char ip[T_IP]; char charge[T_CHARGE];};


#endif
