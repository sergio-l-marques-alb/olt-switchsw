################################################
#                                              #
#   Generic Switchdrvr Makefile                #
#                                              #
#   Milton Ruas, 2017                          #
#   milton-r-silva@alticelabs.com              #
#                                              #
################################################

RM    =	@-rm
MV    =	mv
CP    =	cp
TAR   = tar

unexport CFLAGS
unexport CPPFLAGS
unexport LDFLAGS

#By default use all available CPUs
NUM_CPUS ?= $(shell grep -c 'model name' /proc/cpuinfo)

CURRENT_PATH ?= $(shell pwd)
FP_FOLDER    ?= $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      ?= $(subst /$(FP_FOLDER),,$(shell pwd))

CARD_FOLDER ?= FastPath-Ent-esw-xgs4-$(CPU)-LR-CSxw-IQH_$(BOARD)
CARD        ?= $(word 2,$(subst _, ,$(CARD_FOLDER)))

export OUTPATH       ?= output/$(CARD_FOLDER)
export FP_VIEWNAME   ?= .
export CROSS_COMPILE ?= $(COMPILER)
export KERNEL_SRC    ?= $(KERNEL_PATH)
export CCVIEWS_HOME  ?= $(OLT_DIR)/$(FP_FOLDER)
export BUILDIR       ?= $(DESTDIR)
export BUILDIR_LOCAL ?= $(DESTDIR)
export CROSS_COMPILE := $(TOOLCHAIN_BIN_DIR)/$(CROSS_COMPILE)

KO_PATH	      = $(CCVIEWS_HOME)/$(OUTPATH)/target
BIN_PATH      = $(CCVIEWS_HOME)/$(OUTPATH)/ipl
BIN_FILE      = switchdrvr
DEVSHSYM_FILE = devshell_symbols.gz

SDK_LINK := vendor/broadcom_$(BOARD)
SDK_PATH ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-switchdrvr-6.5.7/broadcom

export vend_sdk	    := vendor/broadcom_$(BOARD)/esw_sdk
export BROADCOM_SDK := $(vend_sdk)

FP_CLI_PATH   := ../fastpath.cli
FP_SHELL_PATH := ../fastpath.shell

export LVL7_MAKEFILE_LOGGING ?= N
export LVL7_MAKEFILE_DISPLAY_MODE ?= S


.PHONY: welcome all clean cleanall install help h

all: welcome setsdk cli shell mgmd
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	@echo "Compiling switchdrvr for $(BOARD) with $(NUM_CPUS) cores... [BUILDIR=$(BUILDIR)]"
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

setsdk:
	rm -f $(SDK_LINK)
	ln -s $(SDK_PATH) $(SDK_LINK)

mgmd:
	@if [ ! -d src/application/switching/mgmd ]; then\
		@echo "MGMD source-code not found! Please update your working copy.";\
		false;\
	fi;
	@sh mgmd_compile.sh $(BOARD)

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
	@echo "   FastPath Makefile for the $(BOARD) card    "
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

install:
	@sh $(INSTALL_SCRIPT)

cli:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-common.make

shell:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-common.make

clean-cli:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-common.make clean

clean-shell:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-common.make clean

clean cleanall: welcome setsdk clean-cli clean-shell clean-mgmd
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	@$(RM) $(SDK_LINK)

clean-platform: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-binds clean-plat_bsp clean-cpu_bsp clean-base

clean-mgmd:
	@sh mgmd_compile.sh $(BOARD) clean

clean-ptin clean-switching clean-routing clean-base clean-andl clean-os: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@

clean-xui: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-snmp clean-openssl clean-cli clean-modb clean-xlib clean-xweb clean-emweb

