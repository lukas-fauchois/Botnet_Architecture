export CC=gcc
export CFLAGS=-W -Wall -ansi -pedantic
export LDFLAGS=

ADMIN_DIR=admin
BOTS_DIR=bots
CANDC_DIR=candc
DYNAMIC_LIB_DIR=dynamic_lib
IPC_LIB_DIR=ipc_lib
NETWORK_LIB_DIR=network_lib
THREAD_LIB_DIR=thread_lib
WEBSERVER_DIR=webserver

EXEC1=$(DYNAMIC_LIB_DIR)/libcharge.so
EXEC2=$(IPC_LIB_DIR)/libipc.a
EXEC3=$(NETWORK_LIB_DIR)/libnetwork.a
EXEC4=$(THREAD_LIB_DIR)/libthread.a
EXEC5=$(BOTS_DIR)/bots
EXEC6=$(CANDC_DIR)/candc
EXEC7=$(ADMIN_DIR)/admin
EXEC8=$(WEBSERVER_DIR)/server


all: $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4) $(EXEC5) $(EXEC6) $(EXEC7) $(EXEC8)
 
$(EXEC1):
	@(cd $(DYNAMIC_LIB_DIR) && $(MAKE))

$(EXEC2):
	@(cd $(IPC_LIB_DIR) && $(MAKE))

$(EXEC3):
	@(cd $(NETWORK_LIB_DIR) && $(MAKE))

$(EXEC4):
	@(cd $(THREAD_LIB_DIR) && $(MAKE))

$(EXEC5):
	@(cd $(BOTS_DIR) && $(MAKE))

$(EXEC6):
	@(cd $(CANDC_DIR) && $(MAKE))

$(EXEC7):
	@(cd $(ADMIN_DIR) && $(MAKE))

$(EXEC8):
	@(cd $(WEBSERVER_DIR) && $(MAKE))


.PHONY: clean mrproper $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4) $(EXEC5) $(EXEC6) $(EXEC7) $(EXEC8)


clean:
	@(cd $(ADMIN_DIR) && $(MAKE) $@)
	@(cd $(BOTS_DIR) && $(MAKE) $@)
	@(cd $(CANDC_DIR) && $(MAKE) $@)
	@(cd $(DYNAMIC_LIB_DIR) && $(MAKE) $@)
	@(cd $(IPC_LIB_DIR) && $(MAKE) $@)
	@(cd $(NETWORK_LIB_DIR) && $(MAKE) $@)
	@(cd $(THREAD_LIB_DIR) && $(MAKE) $@)
	@(cd $(WEBSERVER_DIR) && $(MAKE) $@)



mrproper: clean
	@(cd $(ADMIN_DIR) && $(MAKE) $@)
	@(cd $(BOTS_DIR) && $(MAKE) $@)
	@(cd $(CANDC_DIR) && $(MAKE) $@)
	@(cd $(DYNAMIC_LIB_DIR) && $(MAKE) $@)
	@(cd $(IPC_LIB_DIR) && $(MAKE) $@)
	@(cd $(NETWORK_LIB_DIR) && $(MAKE) $@)
	@(cd $(THREAD_LIB_DIR) && $(MAKE) $@)
	@(cd $(WEBSERVER_DIR) && $(MAKE) $@)

