#ifndef BOTS_H
#define BOTS_H

#include "../network_lib/libnetwork.h"
#include "../thread_lib/libthread.h"
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <dirent.h>
#include "gestionCandc.h"
#include "structure.h"

#define MAX_CONNEXIONS 10
#define MAX_LIGNE 200
#define T_M 10
#define T_ID 6
#define T_L 4
#define T_CHARGE 50
#define T_CHEMIN 50

void charge(void *);
void (*start)(void); //fonction start de a lib dynamique

#endif
