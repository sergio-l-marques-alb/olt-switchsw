
###########################################################
### Alterações feitas nos módulos de kernel da broadcom ###
###########################################################

Index: ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/make/Makefile.linux-pq3-2_6
===================================================================
--- ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/make/Makefile.linux-pq3-2_6      (revision 3994)
+++ ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/make/Makefile.linux-pq3-2_6      (working copy)
@@ -198,7 +198,10 @@

 # Default Linux include directory
 ifeq (,$(LINUX_INCLUDE))
-LINUX_INCLUDE := $(KERNDIR)/include
+#LINUX_INCLUDE := $(KERNDIR)/include
+LINUX_INCLUDE := $(KERNDIR)/linux-inc/include
+else
+LINUX_INCLUDE := $(KERNDIR)/linux-inc/include
 endif

 CFGFLAGS += -DSYS_BE_PIO=1 -DSYS_BE_PACKET=0 -DSYS_BE_OTHER=1
@@ -251,7 +254,7 @@
 endif

 ifeq (,$(KFLAGS))
-KFLAGS := -D__KERNEL__ -m32 -Wp, -nostdinc -isystem $(KFLAG_INCLD) -I$(LINUX_INCLUDE) -include $(LINUX_INCLUDE)/linux/version.h -include $(LINUX_INCLUDE)/linux/autoconf.h -I$(KERNDIR)/arch/powerpc -I$(KERNDIR)/arch/powerpc/include -I$(KERNDIR)/include/asm-generic -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -msoft-float -pipe -ffixed-r2 -mmultiple -mno-altivec -funit-at-a-time -Wa,-me500 -fomit-frame-pointer  -Wdeclaration-after-statement -Wno-pointer-sign
+KFLAGS := -D__KERNEL__ -m32 -Wp, -nostdinc -isystem $(KFLAG_INCLD) -I$(KERNDIR)/include -include $(LINUX_INCLUDE)/../linux-inc/include/linux/version.h -include $(LINUX_INCLUDE)/generated/autoconf.h -I$(KERNDIR)/arch/powerpc -I$(KERNDIR)/arch/powerpc/include -I$(KERNDIR)/arch/powerpc/include/generated -I$(KERNDIR)/include/asm-generic -I$(KERNDIR)/arch/powerpc/include/asm -I$(LINUX_INCLUDE)/../linux-inc/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -msoft-float -pipe -ffixed-r2 -mmultiple -mno-altivec -funit-at-a-time -Wa,-me500 -fomit-frame-pointer  -Wdeclaration-after-statement -Wno-pointer-sign
 endif

 # Use MSI interrupts if kernel is compiled with MSI support.
Index: ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/src/sal/core/linux/thread.c
===================================================================
--- ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/src/sal/core/linux/thread.c      (revision 3994)
+++ ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/src/sal/core/linux/thread.c      (working copy)
@@ -615,10 +615,19 @@
 sal_sleep(int sec)
 {
     wait_queue_head_t queue;
+    DEFINE_WAIT(wait);

     assert(!in_interrupt());
     init_waitqueue_head(&queue);
-    interruptible_sleep_on_timeout(&queue, sec * HZ);
+    #if 0
+       Desde o kernel 3.15 que esta funcao deixou de existir
+       interruptible_sleep_on_timeout(&queue, sec * HZ);
+    #else
+//     timeout = interruptible_sleep_on_timeout(&w1_control_wait, timeout);
+       prepare_to_wait(&queue, &wait, TASK_INTERRUPTIBLE);
+       schedule_timeout(sec * HZ);
+       finish_wait(&queue, &wait);
+    #endif
     thread_check_signals();
 #ifndef LKM_2_6
     mb();
@@ -646,6 +655,7 @@
 {
     sal_usecs_t start_usec;
     wait_queue_head_t queue;
+    DEFINE_WAIT(wait);

     if (in_interrupt()) {
         assert(usec < 1000);
@@ -661,7 +671,15 @@
             } while ((sal_time_usecs() - start_usec) < usec);
         } else {
             init_waitqueue_head(&queue);
+            #if 0
+            Desde o kernel 3.15 que esta funcao deixou de existir
             interruptible_sleep_on_timeout(&queue, USEC_TO_JIFFIES(usec));
+            #else
+//          timeout = interruptible_sleep_on_timeout(&w1_control_wait, timeout);
+            prepare_to_wait(&queue, &wait, TASK_INTERRUPTIBLE);
+            schedule_timeout(USEC_TO_JIFFIES(usec));
+            finish_wait(&queue, &wait);
+            #endif
             thread_check_signals();
         }
     }
Index: ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/systems/bde/linux/kernel/linux-kernel-bde.c
===================================================================
--- ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/systems/bde/linux/kernel/linux-kernel-bde.c      (revision 3994)
+++ ../../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom/esw_sdk/systems/bde/linux/kernel/linux-kernel-bde.c      (working copy)
@@ -240,7 +240,9 @@

 /* Select SPI device revision (cannot be probed) */
 static uint32_t spi_revid = 1;
-LKM_MOD_PARAM(spi_revid, "i", uint, 1);
+//LKM_MOD_PARAM(spi_revid, "i", uint, 1);
+//#warning "### 3.18.25 kernel does not accept a non zero priority!!!###"
+LKM_MOD_PARAM(spi_revid, "i", uint, 0);
 MODULE_PARM_DESC(spi_revid,
 "Select device revision for SPI slave device");

mruas@vxcomp-ubuntu:~/repositorio/svn/trunk/fastpath$

#################################
### Alteracoes ao file system ###
#################################
sed -i 's/dmasize=16M/dmasize=16M himem=1/g' /usr/local/ptin/scripts/fp.insmods.sh
mknod /dev/linux-kernel-bde c 127 0


######################################################
### Makefile utilizada cxo640g.make-kernel-3.18.25 ###
######################################################

##############################################
#                                            #
# FastPath Makefile for the CXO640G card     #
#                                            #
#  Daniel Figueira, 2013                     #
#  daniel-f-figueira@ext.ptinovacao.pt       #
#                                            #
##############################################

RM    = @-rm
MV    = mv
CP    = cp
TAR   = tar

INSTALL_DIR     = ../../../PR1003/builds_olt360/apps/CXO640G
BACKUP_DIR      = ../../../PR1003/builds_olt360/apps_backup/CXO640G

NUM_CPUS = 2
#NUM_CPUS = $(shell grep -c 'model name' /proc/cpuinfo)

CURRENT_PATH= $(shell pwd)
FP_FOLDER       = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR         = $(subst /$(FP_FOLDER),,$(shell pwd))
USER_NAME       = $(shell whoami)

TMP_FILE        = /tmp/$(USER_NAME)_fp_compiled_$(FP_FOLDER)_$(CPU)_$(BOARD)
KO_PATH         = $(CCVIEWS_HOME)/$(OUTPATH)/target
BIN_PATH        = $(CCVIEWS_HOME)/$(OUTPATH)/ipl
BIN_FILE        = switchdrvr
DEVSHSYM_FILE   = devshell_symbols.gz

export COMPILER         = /opt/eldk/usr/bin/ppc_85xxDP-
#export KERNEL_PATH     = $(OLT_DIR)/../lib/kernel/linux-2.6.27.56
export KERNEL_PATH      = /home/smarques/linux-denx-3.18.25

CARD_FOLDER     = FastPath-Ent-esw-xgs4-pq3-LR-CSxw-IQH_CXO640G
CARD            = $(word 2,$(subst _, ,$(CARD_FOLDER)))
CPU             = $(word 5,$(subst -, ,$(CARD_FOLDER)))

export OUTPATH          := output/$(CARD_FOLDER)
export FP_VIEWNAME      := .
export CROSS_COMPILE:= $(COMPILER)
export KERNEL_SRC       := $(KERNEL_PATH)
export CCVIEWS_HOME     := $(OLT_DIR)/$(FP_FOLDER)

export SDK_LINK := vendor/broadcom
export SDK_PATH := $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom
#export SDK_PATH := /home/olt/svnrepo/olt-switchsw/trunk/lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.3.7/broadcom

export FP_CLI_PATH   := ../fastpath.cli
export FP_SHELL_PATH := ../fastpath.shell

export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

.PHONY: welcome all clean cleanall help h kernel transfer

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
        cd $(KERNEL_PATH)
#       cd $(KERNEL_PATH) && ./build_ppc_cxo360g.sh

install:
        sh cxo640g.install

help h:
        @echo ""
        @echo "Makefile Help"
        @echo " make                    "
        @echo " make clean              "
        @echo " make cleanall           "
        @echo " make install            "
        @echo " make kernel             "
        @echo " make cli                "
	@echo " make shell              "
	@echo " make cli_clean          "
	@echo " make shell_clean        "
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

cli:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-cxo640g.make

shell:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-cxo640g.make

cli_clean:
	@$(MAKE) -C $(FP_CLI_PATH) -f fp.cli-cxo640g.make clean

shell_clean:
	@$(MAKE) -C $(FP_SHELL_PATH) -f fp.shell-cxo640g.make clean

clean cleanall: welcome setsdk cli_clean shell_clean
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)

clean-platform: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) clean-binds clean-plat_bsp clean-cpu_bsp clean-base
	$(RM) -f $(TMP_FILE)

clean-ptin clean-switching clean-routing clean-base clean-andl: setsdk
	$(MAKE) -j$(NUM_CPUS) -C $(CCVIEWS_HOME)/$(OUTPATH) $@
	$(RM) -f $(TMP_FILE)




