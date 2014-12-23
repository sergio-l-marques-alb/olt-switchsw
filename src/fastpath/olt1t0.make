################################################
#                                              #
#   FastPath Makefile for the OLT1T0 card      #
#                                              #
#	Joao Mateiro, 2014                     #
#	joao-v-mateiro@ptinovacao.pt           #
#                                              #
################################################

RM    =	@-rm
MV    =	mv
CP    =	cp
TAR   = tar

#NUM_CPUS = 2
NUM_CPUS = $(shell grep -c 'model name' /proc/cpuinfo)

CURRENT_PATH= $(shell pwd)
FP_FOLDER	= $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR		= $(subst /$(FP_FOLDER),,$(shell pwd))
USER_NAME	= $(shell whoami)

TMP_FILE	= /tmp/$(USER_NAME)_fp_compiled_$(FP_FOLDER)_$(CPU)_$(BOARD)
KO_PATH		= $(CCVIEWS_HOME)/$(OUTPATH)/target
BIN_PATH	= $(CCVIEWS_HOME)/$(OUTPATH)/ipl
BIN_FILE	= switchdrvr
DEVSHSYM_FILE	= devshell_symbols.gz

#export TOOLCHAIN_BASE_DIR = /opt/broadcom
export TOOLCHAIN_BASE_DIR = /opt/broadcom
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH	  = $(TOOLCHAIN_BASE_DIR)/lib

export COMPILER           = $(TOOLCHAIN_BIN_DIR)/arm-linux-
export KERNEL_PATH        = $(OLT_DIR)/../lib/kernel/linux-3.6.5-arm
#export KERNEL_PATH        = /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5

BOARD = OLT1T0
CARD_FOLDER = FastPath-Ent-esw-xgs4-helixarm-LR-CSxw-IQH_$(BOARD)
CARD	= $(word 2,$(subst _, ,$(CARD_FOLDER)))
CPU	= $(word 5,$(subst -, ,$(CARD_FOLDER)))

export OUTPATH		:= output/$(CARD_FOLDER)
export FP_VIEWNAME	:= .
export CROSS_COMPILE:= $(COMPILER)
export KERNEL_SRC	:= $(KERNEL_PATH)
export CCVIEWS_HOME	:= $(OLT_DIR)/$(FP_FOLDER)

export SDK_LINK := vendor/broadcom
export SDK_PATH := $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.3.10/broadcom
#export SDK_PATH := /home/olt/svnrepo/olt-switchsw/trunk/lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.3.10/broadcom

export FP_CLI_PATH   := ../fastpath.cli
export FP_SHELL_PATH := ../fastpath.shell

export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

.PHONY: welcome all clean cleanall help h kernel transfer

all: welcome setsdk mgmdconfig cli_clean shell_clean cli shell
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	@if [ -f $(TMP_FILE) ]; then\
		echo "Replacing package.cfg with the one without xweb and snmp compilation...";\
		cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_woXweb package.cfg;\
		echo "";\
	else\
		echo "Replacing package.cfg with the one with xweb and snmp compilation...";\
		cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_original package.cfg;\
		echo "";\
	fi;
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH)
	@touch $(TMP_FILE);\
	cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_original package.cfg
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

andl os: welcome setsdk
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	@if [ -f $(TMP_FILE) ]; then\
		echo "Replacing package.cfg with the one without xweb and snmp compilation...";\
		cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_woXweb package.cfg;\
	echo "";\
	fi;
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	@touch $(TMP_FILE);\
	cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_original package.cfg
	@if [ -f $(BIN_PATH)/$(BIN_FILE) ]; then\
		echo "Saving original $(BIN_FILE) binary...";\
		$(CP) $(BIN_PATH)/$(BIN_FILE) $(BIN_PATH)/$(BIN_FILE).unstripped;\
		echo "Stripping $(BIN_FILE) binary...";\
		$(CROSS_COMPILE)strip $(BIN_PATH)/$(BIN_FILE);\
	fi;
	@echo ""

transfer:
	cd $(OUTPATH) && ./transfer_paulo.sh bin

setsdk:
	rm -f $(SDK_LINK)
	ln -s $(SDK_PATH) $(SDK_LINK)

mgmdconfig:
	@sh mgmd_config_$(CARD).sh

kernel:
	cd $(KERNEL_PATH) && ./build-olt7_8ch.sh

install:
	sh olt1t0.install

help h:
	@echo ""
	@echo "Makefile Help"
	@echo " make                    "
	@echo " make clean              "
	@echo " make cleanall           "
	@echo " make kernel             "
	@echo " make cli                "
	@echo " make shell              "
	@echo " make cli_clean          "
	@echo " make shell_clean        "
	@echo ""

welcome: 
	@echo ""
	@echo "##############################################"
	@echo "#                                            #"
	@echo "#  FastPath Makefile for the OLT1T0 card     #"
	@echo "#                                            #"
	@echo "##############################################"
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
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-olt1t0.make

shell:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-olt1t0.make

cli_clean:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-olt1t0.make clean

shell_clean:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-olt1t0.make clean

clean cleanall: welcome setsdk cli_clean shell_clean
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)

clean-platform: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-binds clean-plat_bsp clean-cpu_bsp clean-base
	$(RM) -f $(TMP_FILE)

clean-ptin clean-switching clean-routing clean-base clean-andl: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)

