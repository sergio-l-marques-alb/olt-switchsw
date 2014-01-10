################################################
#                                              #
#   FastPath Makefile for the OLT7-8CHB card   #
#                                              #
#	Joao Mateiro, 2014                     #
#	joao-v-mateiro@ptinovacao.pt           #
#                                              #
################################################

RM    =	@-rm
MV    =	mv
CP    =	cp
TAR   = tar

NUM_CPUS	= 1 #$(shell grep -c 'model name' /proc/cpuinfo)

CURRENT_PATH= $(shell pwd)
FP_FOLDER	= $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR		= $(subst /$(FP_FOLDER),,$(shell pwd))
USER_NAME	= $(shell whoami)

TMP_FILE	= /tmp/$(USER_NAME)_fp_compiled_$(FP_FOLDER)_$(CPU)_$(BOARD)
KO_PATH		= $(CCVIEWS_HOME)/$(OUTPATH)/target
BIN_PATH	= $(CCVIEWS_HOME)/$(OUTPATH)/ipl
BIN_FILE	= switchdrvr

export COMPILER		= /opt/eb-broadcom/usr/bin/armeb-linux-
export KERNEL_PATH      = /home/devtools/dev-ARM/dev-BCM56340/ldk/3.4.0/iproc/kernel/linux-3.6.5
#export KERNEL_PATH	= $(OLT_DIR)/../lib/kernel/linux-3.6.5
export LD_LIBRARY_PATH	= /opt/eb-broadcom/usr/lib

BOARD = OLT7-8CH_B
CARD_FOLDER = FastPath-Ent-esw-xgs4-helixarm-LR-CSxw-IQH_$(BOARD)
CARD	= $(word 2,$(subst _, ,$(CARD_FOLDER)))
CPU	= $(word 5,$(subst -, ,$(CARD_FOLDER)))

export OUTPATH		:= output/$(CARD_FOLDER)
export FP_VIEWNAME	:= .
export CROSS_COMPILE:= $(COMPILER)
export KERNEL_SRC	:= $(KERNEL_PATH)
export CCVIEWS_HOME	:= $(OLT_DIR)/$(FP_FOLDER)

.PHONY: welcome all clean cleanall help h kernel transfer

all: welcome
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

andl: welcome
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	@if [ -f $(TMP_FILE) ]; then\
		echo "Replacing package.cfg with the one without xweb and snmp compilation...";\
		cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_woXweb package.cfg;\
	echo "";\
	fi;
	@$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) andl
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

kernel:
	cd $(KERNEL_PATH) && ./build-olt7_8ch.sh

install:
	@echo "Installation not defined for OLT7-8CH-B"

help h:
	@echo ""
	@echo "Makefile Help"
	@echo "	make     		"
	@echo "	make clean		"
	@echo "	make cleanall	"
	@echo "	make kernel		"
	@echo ""

welcome: 
	@echo ""
	@echo "##############################################"
	@echo "#                                            #"
	@echo "#  FastPath Makefile for the OLT7-8CHB card  #"
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
	
clean cleanall: welcome
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)

clean-emweb:
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-emweb
	$(RM) -f $(TMP_FILE)
