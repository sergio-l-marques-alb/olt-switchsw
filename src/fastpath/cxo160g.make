##############################################
#                                            #
# FastPath Makefile for the CXO160G card     #
#                                            #
#	Daniel Figueira, 2013                #
#	daniel-f-figueira@ext.ptinovacao.pt  #
#                                            #
##############################################

RM    =	@-rm
MV    =	mv
CP    =	cp
TAR   = tar

NUM_CPUS	= 2
#$(shell grep -c 'model name' /proc/cpuinfo)

CURRENT_PATH= $(shell pwd)
FP_FOLDER	= $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR		= $(subst /$(FP_FOLDER),,$(shell pwd))
USER_NAME	= $(shell whoami)

TMP_FILE	= /tmp/$(USER_NAME)_fp_compiled_$(FP_FOLDER)_$(CPU)_$(BOARD)
KO_PATH		= $(CCVIEWS_HOME)/$(OUTPATH)/target
BIN_PATH	= $(CCVIEWS_HOME)/$(OUTPATH)/ipl
BIN_FILE	= switchdrvr
DEVSHSYM_FILE	= devshell_symbols.gz

#export LD_LIBRARY_PATH=/opt/fsl/1.2/sysroots/i686-fslsdk-linux/lib

export COMPILER 	= /opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr/bin/ppce500mc-fsl-linux/powerpc-fsl-linux-
export KERNEL_PATH	= /home/olt/svnrepo/olt-switchsw/trunk/lib/kernel/linux-3.0.51-smp-e500mc
#/home/devtools/dev-QorIQ/dev-P204x/cxo160g/kernel_3_0_51
#export KERNEL_PATH      = /home/tiago/kernel/linux-3.0.51
#/home/devtools/dev-QorIQ/dev-P204x/cxo160g/
#$(OLT_DIR)/../lib/kernel/official/kernel_3_0_51

#CC='${COMPILER}gcc'
#AS='${COMPILER}as'  
#LD='${COMPILER}ld' 
#AR='${COMPILER}ar' 
#RANLIB='${COMPILER}ranlib'
#CXX='${COMPILER}g++'
#GDB='${COMPILER}gdb'  
#NM='${COMPILER}nm'
#ARCH=powerpc

#export CC AS LD AR RANLIB CXX GDB NM ARCH

#CONFIGURE_FLAGS="--target=powerpc-fsl-linux --host=powerpc-fsl-linux --build=i686-linux --with-libtool-sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
#CFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
#CFGFLAGS=" fewfewfew"
#CFGFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
#CXXFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
#LDFLAGS=" --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
#CPPFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"

#export CC AS CPPFLAGS LD  CROSS_COMPILE ARCH RANLIB NM CONFIGURE_FLAGS CFLAGS LDFLAGS CXXFLAGS

#export CFGFLAGS

CARD_FOLDER 	= FastPath-Ent-esw-xgs4-e500mc-LR-CSxw-IQH_CXO160G
CARD		= $(word 2,$(subst _, ,$(CARD_FOLDER)))
CPU		= $(word 5,$(subst -, ,$(CARD_FOLDER)))

export OUTPATH		:= output/$(CARD_FOLDER)
export FP_VIEWNAME	:= .
export CROSS_COMPILE	:= $(COMPILER)
export KERNEL_SRC	:= $(KERNEL_PATH)
export CCVIEWS_HOME	:= $(OLT_DIR)/$(FP_FOLDER)

export SDK_LINK := vendor/broadcom
export SDK_PATH := $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.3.7/broadcom
#export SDK_PATH := /home/olt/svnrepo/olt-switchsw/trunk/lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.3.7/broadcom

export FP_CLI_PATH   := ../fastpath.cli
export FP_SHELL_PATH := ../fastpath.shell

export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

.PHONY: welcome all install clean cleanall help h kernel cli shell cli_clean shell_clean

all: welcome setsdk mgmdconfig cli_clean shell_clean cli shell
	$(RM) -f $(BIN_PATH)/$(BIN_FILE)
	@if [ -f $(TMP_FILE) ]; then\
		echo "Replacing package.cfg with the one without xweb and snmp compilation...";\
		cd $(CCVIEWS_HOME)/$(OUTPATH) && $(CP) package.cfg_woXweb package.cfg;\
		echo "";\
	fi;
	#@$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) andl
	@$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH)
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

setsdk:
	rm -f $(SDK_LINK)
	ln -s $(SDK_PATH) $(SDK_LINK)

mgmdconfig:
	@if [ ! -d src/application/switching/mgmd ]; then\
		echo "MGMD source-code not found! Please update your working copy.";\
		false;\
	fi;
	@sh mgmd_config_$(CARD).sh

kernel:
	cd $(KERNEL_PATH) && ./build_cxo160g.sh

install:
	sh cxo160g.install

help h:
	@echo ""
	@echo "Makefile Help"
	@echo "	make     		"
	@echo "	make clean		"
	@echo "	make cleanall	        "
	@echo " make install            "
	@echo "	make kernel		"
	@echo " make cli                "
	@echo " make shell              "
	@echo " make cli_clean          "
	@echo " make shell_clean        "
	@echo ""

welcome: 
	@echo ""
	@echo "############################################"
	@echo "#                                          #"
	@echo "#  FastPath Makefile for the CXO160G card  #"
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

andl: welcome setsdk
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

cli:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-cxo160g.make

shell:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-cxo160g.make

cli_clean:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-cxo160g.make clean

shell_clean:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-cxo160g.make clean

clean cleanall: welcome setsdk cli_clean shell_clean
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)

clean-platform: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-binds clean-plat_bsp clean-cpu_bsp clean-base
	$(RM) -f $(TMP_FILE)

clean-ptin clean-switching clean-routing clean-base clean-andl: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)

