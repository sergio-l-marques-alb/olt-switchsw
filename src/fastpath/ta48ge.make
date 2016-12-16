#############################################
#                                           #
# FastPath Makefile for the TA48GE card     #
#                                           #
#  Daniel Figueira, 2013                    #
#  daniel-f-figueira@ext.ptinovacao.pt      #
#                                           #
#############################################

RM    =	@-rm
MV    =	mv
CP    =	cp
TAR   = tar

INSTALL_DIR     = ../../../PR1003/builds_olt360/apps/TA48GE
BACKUP_DIR      = ../../../PR1003/builds_olt360/apps_backup/TA48GE

NUM_CPUS = 2
#NUM_CPUS = $(shell grep -c 'model name' /proc/cpuinfo)

CURRENT_PATH= $(shell pwd)
FP_FOLDER	= $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR		= $(subst /$(FP_FOLDER),,$(shell pwd))
USER_NAME	= $(shell whoami)

TMP_FILE	= /tmp/$(USER_NAME)_fp_compiled_$(FP_FOLDER)_$(CPU)_$(BOARD)
KO_PATH		= $(CCVIEWS_HOME)/$(OUTPATH)/target
BIN_PATH	= $(CCVIEWS_HOME)/$(OUTPATH)/ipl
BIN_FILE	= switchdrvr
DEVSHSYM_FILE	= devshell_symbols.gz

export COMPILER         = /opt/eldk/usr/bin/ppc_85xxDP-
export KERNEL_PATH      = $(OLT_DIR)/../lib/kernel/linux-2.6.35

CARD_FOLDER = FastPath-Ent-esw-xgs4-e500-LR-CSxw-IQH_TA48GE
CARD		= $(word 2,$(subst _, ,$(CARD_FOLDER)))
CPU		= $(word 5,$(subst -, ,$(CARD_FOLDER)))

export OUTPATH		:= output/$(CARD_FOLDER)
export FP_VIEWNAME	:= .
export CROSS_COMPILE:= $(COMPILER)
export KERNEL_SRC	:= $(KERNEL_PATH)
export CCVIEWS_HOME	:= $(OLT_DIR)/$(FP_FOLDER)

export SDK_LINK := vendor/broadcom
export SDK_PATH := $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-switchdrvr-6.5.7/broadcom
#export SDK_PATH := /home/olt/svnrepo/olt-switchsw/trunk/lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.3.7/broadcom

export FP_CLI_PATH   := ../fastpath.cli
export FP_SHELL_PATH := ../fastpath.shell

export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

.PHONY: welcome all install clean cleanall help h kernel cli cli_clean shell shell_clean

all: welcome setsdk cli_clean shell_clean cli shell mgmdconfig
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	@$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH)
	@if [ -f $(BIN_PATH)/$(BIN_FILE) ]; then\
		echo "Saving original $(BIN_FILE) binary...";\
		$(CP) $(BIN_PATH)/$(BIN_FILE) $(BIN_PATH)/$(BIN_FILE).unstripped;\
		echo "Stripping $(BIN_FILE) binary...";\
		$(CROSS_COMPILE)strip $(BIN_PATH)/$(BIN_FILE);\
	fi;
	@echo "Copying mgmd.cli to ipl directory..."
	@$(CP) src/application/switching/mgmd/rfs/usr/local/ptin/sbin/mgmd.cli $(OUTPATH)/ipl/mgmd.cli
	@$(CP) src/application/switching/mgmd/rfs/usr/local/ptin/lib/libmgmd.so $(OUTPATH)/ipl/
	@echo ""

setsdk:
	rm -f $(SDK_LINK)
	ln -s $(SDK_PATH) $(SDK_LINK)

mgmdconfig:
	@if [ ! -d src/application/switching/mgmd ]; then\
		@echo "MGMD source-code not found! Please update your working copy.";\
		false;\
	fi;
	@echo "Compiling MGMD..."
	@sh mgmd_config_$(CARD).sh
	@sh src/application/switching/make/mgmd_compile.sh $(CURRENT_PATH) switching
	@echo "...MGMD compiled!"

kernel:
	cd $(KERNEL_PATH) && ./build_ta48ge.sh

install:
	sh ta48ge.install

help h:
	@echo ""
	@echo "Makefile Help"
	@echo "	make     		"
	@echo "	make clean		"
	@echo "	make cleanall           "
	@echo "	make kernel		"
	@echo " make cli                "
	@echo " make shell              "
	@echo " make cli_clean          "
	@echo " make shell_clean        "
	@echo ""

welcome: 
	@echo ""
	@echo "###########################################"
	@echo "#                                         #"
	@echo "#  FastPath Makefile for the TA48GE card  #"
	@echo "#                                         #"
	@echo "###########################################"
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

cli:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-ta48ge.make

shell:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-ta48ge.make

cli_clean:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-ta48ge.make clean

shell_clean:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-ta48ge.make clean

clean cleanall: welcome setsdk cli_clean shell_clean
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)

clean-platform: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-binds clean-plat_bsp clean-cpu_bsp clean-base
	$(RM) -f $(TMP_FILE)

clean-ptin clean-switching clean-routing clean-base clean-andl: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)

