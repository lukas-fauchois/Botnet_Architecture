#ifndef __COMMUNICATION_IPC_H
#define __COMMUNICATION_IPC_H

//Definition des fichiers d'inclusions
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

//Constantes pour obtenir les clés
#define FICHIER_COMMANDE "/etc/passwd"
#define PROJ_FTOK 'a'


//Clé file admin
#define KEY_ADMIN (key_t)1000

//Clés segments de mémoire partagée
#define KEY_NAME_LENGTH (key_t)3130
#define KEY_NAME (key_t)3131
#define KEY_CONTENT_LENGTH (key_t)3132
#define KEY_CONTENT (key_t)3133
#define KEY_SERVER_PID (key_t)3134
#define KEY_COMMAND_ID (key_t)3135
#define KEY_HOST_LENGTH (key_t)3136
#define KEY_HOST (key_t)3137
#define KEY_SERVICE_LENGTH (key_t)3138
#define KEY_SERVICE (key_t)3139
#define KEY_CANDC_PID (key_t)3140
#define KEY_CHARGE_INDEX (key_t)3141
#define KEY_BOT_LIST_LENGTH (key_t)3142
#define KEY_BOT_LIST (key_t)3143
#define KEY_INSTALLATION (key_t)3144
#define KEY_RUNNING (key_t)3145
#define KEY_DISCONNECT (key_t)3146
#define KEY_DELETE (key_t)3147
#define KEY_RESULT_LENGTH (key_t)3148
#define KEY_RESULT (key_t)3149
#define KEY_STATUS_LENGTH (key_t)3150
#define KEY_STATUS (key_t)3151

//ID des commandes à exécuter
#define ID_LIST 1
#define ID_INSTALL 2
#define ID_RUN 3
#define ID_DELETE 4
#define ID_RESULT 5
#define ID_STAT 6
#define ID_QUIT 7
#define ID_FORM 8

//Taille buf d'un message
#define SIZE_BUF 256

//Structure d'un message
struct data_s{
	long type; //type du message
	char buf[SIZE_BUF]; //message
};

typedef struct data_s data_t;

int msgid;

struct sigaction action;

void initSIGINT(sigset_t);
int FileIPC_create(key_t);
int FileIPC_get(key_t);
void envoiIPC(int, long, char*);
data_t receptionIPC(int, long);
void destructionFileIPC(int);

#endif
