################################################
#                                              #
#   FastPath Makefile for the TG16GF card      #
#                                              #
#   Joao Mateiro, 2014                         #
#   joao-v-mateiro@ptinovacao.pt               #
#                                              #
################################################

RM    =	@-rm
MV    =	mv
CP    =	cp
TAR   = tar

export vend_sdk		:= vendor/broadcom/esw_sdk
export BROADCOM_SDK	:= $(vend_sdk)

#NUM_CPUS = 1
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
export TOOLCHAIN_BASE_DIR = /opt/broadcom_kt2_hx4/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH	  = $(TOOLCHAIN_BASE_DIR)/lib

export COMPILER           = $(TOOLCHAIN_BIN_DIR)/arm-linux-
export KERNEL_PATH        = $(OLT_DIR)/../lib/kernel/linux-3.6.5-kt2arm_LDK-3.4.7-RC4
#export KERNEL_PATH        = /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5

BOARD	= TG16GF
CPU		= katanaarm
CARD_FOLDER = FastPath-Ent-esw-xgs4-$(CPU)-LR-CSxw-IQH_$(BOARD)
CARD	= $(word 2,$(subst _, ,$(CARD_FOLDER)))
#CPU	= $(word 5,$(subst -, ,$(CARD_FOLDER)))

export OUTPATH		:= output/$(CARD_FOLDER)
export FP_VIEWNAME	:= .
export CROSS_COMPILE    := $(COMPILER)
export KERNEL_SRC	:= $(KERNEL_PATH)
export CCVIEWS_HOME	:= $(OLT_DIR)/$(FP_FOLDER)

export SDK_LINK := vendor/broadcom
export SDK_PATH := $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-switchdrvr-6.5.7/broadcom
#export SDK_PATH := /home/olt/svnrepo/olt-switchsw/trunk/lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom

export FP_CLI_PATH   := ../fastpath.cli
export FP_SHELL_PATH := ../fastpath.shell

export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

.PHONY: welcome all clean cleanall help h kernel transfer

all: welcome setsdk cli shell mgmd
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH)
	@if [ -f $(BIN_PATH)/$(BIN_FILE) ]; then\
		echo "Saving original $(BIN_FILE) binary...";\
		$(CP) $(BIN_PATH)/$(BIN_FILE) $(BIN_PATH)/$(BIN_FILE).unstripped;\
		echo "Stripping $(BIN_FILE) binary...";\
		$(CROSS_COMPILE)strip $(BIN_PATH)/$(BIN_FILE);\
	fi;
	@echo ""

andl os: welcome setsdk
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
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
#	rm -f $(SDK_LINK)
#	ln -s $(SDK_PATH) $(SDK_LINK)

mgmd:
	@if [ ! -d src/application/switching/mgmd ]; then\
		@echo "MGMD source-code not found! Please update your working copy.";\
		false;\
	fi;
	@sh mgmd_compile.sh $(BOARD)

kernel:
	cd $(KERNEL_PATH) && ./build_kernel_katana2_le.sh

install:
	sh tg16gf.install

help h:
	@echo ""
	@echo "Makefile Help"
	@echo " make                    "
	@echo " make clean              "
	@echo " make cleanall           "
	@echo " make kernel             "
	@echo " make cli                "
	@echo " make shell              "
	@echo " make clean-cli          "
	@echo " make clean-shell        "
	@echo " make mgmd               "
	@echo " make clean-mgmd         "
	@echo ""

welcome: 
	@echo ""
	@echo "##############################################"
	@echo "#                                            #"
	@echo "#  FastPath Makefile for the TG16GF card     #"
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
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-tg16gf.make

shell:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-tg16gf.make

clean-cli:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-tg16gf.make clean

clean-shell:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-tg16gf.make clean

clean cleanall: welcome setsdk clean-cli clean-shell clean-mgmd
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	#$(RM) -f $(TMP_FILE)

clean-platform: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-binds clean-plat_bsp clean-cpu_bsp clean-base
	#$(RM) -f $(TMP_FILE)

clean-mgmd:
	@sh mgmd_compile.sh $(BOARD) clean

clean-ptin clean-switching clean-routing clean-base clean-andl clean-os: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	#$(RM) -f $(TMP_FILE)

clean-xui: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-snmp clean-openssl clean-cli clean-modb clean-xlib clean-xweb clean-emweb
	#$(RM) -f $(TMP_FILE)

