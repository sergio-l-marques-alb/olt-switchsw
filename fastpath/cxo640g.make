##############################################
#                                            #
# FastPath Makefile for the CXO640G card     #
#                                            #
#	Daniel Figueira, 2013                #
#	daniel-f-figueira@ext.ptinovacao.pt  #
#                                            #
##############################################

RM    =	@-rm
MV    =	mv
CP    =	cp
TAR   = tar

INSTALL_DIR	= ../../../PR1003/builds_olt360/apps/CXO640G
BACKUP_DIR	= ../../../PR1003/builds_olt360/apps_backup/CXO640G

NUM_CPUS	= $(shell grep -c 'model name' /proc/cpuinfo)

CURRENT_PATH= $(shell pwd)
FP_FOLDER	= $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR		= $(subst /$(FP_FOLDER),,$(shell pwd))
USER_NAME	= $(shell whoami)

TMP_FILE	= /tmp/$(USER_NAME)_fp_compiled_$(FP_FOLDER)_$(CPU)_$(BOARD)
KO_PATH		= $(CCVIEWS_HOME)/$(OUTPATH)/target
BIN_PATH	= $(CCVIEWS_HOME)/$(OUTPATH)/ipl
BIN_FILE	= switchdrvr
DEVSHSYM_FILE	= devshell_symbols.gz

export COMPILER		= /opt/eldk/usr/bin/ppc_85xxDP-
export KERNEL_PATH	= $(OLT_DIR)/../lib/tmp/linux-2.6.27.56_1G

CARD_FOLDER 	= FastPath-Ent-esw-xgs4-pq3-LR-CSxw-IQH_CXO640G
CARD		= $(word 2,$(subst _, ,$(CARD_FOLDER)))
CPU		= $(word 5,$(subst -, ,$(CARD_FOLDER)))

export OUTPATH		:= output/$(CARD_FOLDER)
export FP_VIEWNAME	:= .
export CROSS_COMPILE:= $(COMPILER)
export KERNEL_SRC	:= $(KERNEL_PATH)
export CCVIEWS_HOME	:= $(OLT_DIR)/$(FP_FOLDER)

.PHONY: welcome all install clean cleanall help h kernel

all: welcome
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	@if [ -f $(TMP_FILE) ]; then\
		echo "Replacing package.cfg with the one without xweb and snmp compilation...";\
		cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_woXweb package.cfg;\
		echo "";\
	fi;
	@$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH)
	@touch $(TMP_FILE);\
	cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_original package.cfg
	@if [ -f $(BIN_PATH)/$(BIN_FILE) ]; then\
		echo "Saving original $(BIN_FILE) binary...";\
		$(CP) $(BIN_PATH)/$(BIN_FILE) $(BIN_PATH)/$(BIN_FILE).unstripped;\
		echo "Stripping $(BIN_FILE) binary...";\
		$(CROSS_COMPILE)strip $(BIN_PATH)/$(BIN_FILE);\
	fi;
	@echo ""

kernel:
	cd $(KERNEL_PATH) && ./build_ppc_cxo360g.sh

install:
	sh cxo640g.install

help h:
	@echo ""
	@echo "Makefile Help"
	@echo "	make     		"
	@echo "	make clean		"
	@echo "	make cleanall	        "
	@echo " make install            "
	@echo "	make kernel		"
	@echo ""

welcome: 
	@echo ""
	@echo "############################################"
	@echo "#                                          #"
	@echo "#  FastPath Makefile for the CXO640G card  #"
	@echo "#                                          #"
	@echo "############################################"
	@echo ""
	@echo "FP_FOLDER = $(FP_FOLDER)"
	@echo "OLT_DIR = $(OLT_DIR)"
	@echo "CCVIEWS HOME = $(CCVIEWS_HOME)"
	@echo "COMPILER = $(COMPILER)"
	@echo "KERNEL_SRC = $(KERNEL_SRC)"
	@echo "CARD = $(CARD)"
	@echo "CARD FOLDER = $(OUTPATH)"
	@echo "CPU = $(CPU)"
	@echo ""
	
clean cleanall: welcome
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)
