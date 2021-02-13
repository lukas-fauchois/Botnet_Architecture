#include <sys/ipc.h>
#include "../ipc_lib/libipc.h"
#include <string.h>

#define T_NAME 30
int msgid1, msgid2;
key_t key_candc = 1000;
key_t key_admin = 2000;

//File du C&C
#define ADMIN_CONNECTED         1
#define LIST                    2
#define INSTALL                 3
#define RUN                     4
#define RESULT                  5
#define DELETE                  6
#define QUIT                    7
#define STAT                    8

//File de l'ADMIN
#define CANCC_CONNECTED         9
#define BOTS_LIST               10
#define INSTALLATION_DONE       11
#define RUNNING_DONE            12
#define RESULT_DONE             13
#define DELETE_DONE             14
#define QUIT_DONE               15
#define STAT_DONE               16

#define SIZE_HOST               19
#define SIZE_LIB                20
