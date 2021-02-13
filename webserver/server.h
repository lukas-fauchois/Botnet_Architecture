#define _GNU_SOURCE //doit être avant les includes

/** Include files **/

#include <fcntl.h>
#include <sys/stat.h>

#include "../network_lib/libnetwork.h"
#include "../thread_lib/libthread.h"
#include "../ipc_lib/libipc.h"

/** Some constants **/

#define WEB_DIR  "./www"
#define PAGE_NOTFOUND "error.html"

#define CODE_OK  200
#define CODE_NOTFOUND 404

#define MAX_CONNEXIONS 10 //pour le serveur serverTCP
#define MAX_BUFFER 100

//fonctions de la libnetwork
int initialisationServeur(char*, int);
int boucleServeurTCP(int, int (*traitement)(int));

//fonctions de la libipc
int createSHM(key_t, size_t);
int detachSHM(void*);
int deleteSHM(int);
void writeSHM_unsigned_char(key_t, unsigned char*, int);
unsigned char* readSHM_unsigned_char(key_t, int);
void writeSHM_int(key_t, int*, int);
int* readSHM_int(key_t, int);

//Déclaration des SHMID des différents segments de mémoire partagée
int shmid_name_length;
int shmid_name;
int shmid_content_length;
int shmid_content;
int shmid_server_pid;
int shmid_command_id;
int shmid_host_length;
int shmid_host;
int shmid_service_length;
int shmid_service;
int shmid_charge_id_length;
int shmid_charge_id;
