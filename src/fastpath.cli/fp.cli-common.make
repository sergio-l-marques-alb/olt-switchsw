#MAJOR.MINOR.REVISION.BUILD
MAJOR = \"1\"
MINOR = \"1\" 
REVISION=\"0\"
X1= $(shell date +%H)*60+$(shell date +%M)
X2 = $(shell echo $(X1) | bc)
BUILD=\"$(shell date +%y%m%d)-$(X2)\"

FP_FOLDER ?= fastpath

CARD_FOLDER ?= FastPath-Ent-esw-xgs4-$(CPU)-LR-CSxw-IQH_$(BOARD)

OUTPATH ?= output/$(CARD_FOLDER)

#CROSS_COMPILE := $(TOOLCHAIN_BIN_DIR)/$(CROSS_COMPILE)

TARGET_PATH=../$(FP_FOLDER)/$(OUTPATH)/ipl
OBJECT_PATH=../$(FP_FOLDER)/$(OUTPATH)/objects/fp.cli

RM=rm -f
MKDIR=mkdir -p
TARGET=fp.cli

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

HDRS=-I. -Iutils -Iipc -Imain
#-I$(CROSS_DIR)/powerpc-e300c3-linux/include \
#-I$(LINUX_DIR)/include

#-I/usr/local/pq2-linux-ptin/include
#-I/usr/local/pq2-linux-ptin/include \
#-I/usr/include/PCSC
				
OBJ_MAIN = $(OBJECT_PATH)/main.o $(OBJECT_PATH)/utils.o $(OBJECT_PATH)/memchk.o 
		
OBJ_CTRL = $(OBJECT_PATH)/ipc_lib.o

OBJ =  $(OBJ_CTRL) $(OBJ_MAIN)

ifeq ($(BOARD),CXO640G)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),CXO160G)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wno-strict-aliasing -g
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),TA48GE)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),TG4G)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),TG16G)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),TG16GF)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wno-strict-aliasing -g \
  -marm -march=armv7-a -mfpu=vfp -mfloat-abi=softfp -msoft-float -fomit-frame-pointer \
  -Wno-pointer-sign -Wno-unused-but-set-variable -Wno-enum-compare -Wno-switch -Wno-address -Wno-sequence-point
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),OLT1T0)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wno-strict-aliasing -g \
  -marm -march=armv7-a -mfpu=vfp -mfloat-abi=softfp -msoft-float -fomit-frame-pointer \
  -Wno-pointer-sign -Wno-unused-but-set-variable -Wno-enum-compare -Wno-switch -Wno-address -Wno-sequence-point
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),TT04SXG)
CFLAGS=-DPTIN_BOARD=$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wno-strict-aliasing -g \
  -marm -march=armv7-a -mfpu=vfp -mfloat-abi=softfp -msoft-float -fomit-frame-pointer \
  -Wno-pointer-sign -Wno-unused-but-set-variable -Wno-enum-compare -Wno-switch -Wno-address -Wno-sequence-point
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),OLT1T0F)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wno-strict-aliasing -g \
  -marm -march=armv7-a -mfpu=vfp -mfloat-abi=softfp -msoft-float -fomit-frame-pointer \
  -Wno-pointer-sign -Wno-unused-but-set-variable -Wno-enum-compare -Wno-switch -Wno-address -Wno-sequence-point
OPTIMIZACAO = -O6
LDFLAGS = -Wl,-Bdynamic
endif

ifeq ($(BOARD),CXP360G)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g
LDFLAGS = -Wl,-Bdynamic
OPTIMIZACAO = -O6
endif

ifeq ($(BOARD),TOLT8G)
CFLAGS=-DPTIN_BOARD=PTIN_BOARD_$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g
LDFLAGS = -Wl,-Bdynamic
OPTIMIZACAO = -O6
endif

all:	Makefile $(OBJ)
		@echo "Generating $(BOARD) fp.cli -> $(TARGET_PATH)/$(TARGET)"
		@$(MKDIR) $(TARGET_PATH)
		@$(CC) $(LDFLAGS) $(OPTIMIZACAO) -o $(TARGET_PATH)/$(TARGET) $(OBJ) -lc -lm -lpthread -lrt
   
install:	$(TARGET)
		@echo "Installing $(BOARD)"
		sh cli.install
	
clean cleanall:
		@echo "Cleaning $(BOARD)"
		@$(RM) -f $(TARGET_PATH)/$(TARGET)
		@$(RM) -rf $(OBJECT_PATH)

vpath %.c       # Search path for *.c files

#
# Rule for C compilation
#
$(OBJECT_PATH)/%.o : %.c
	@$(MKDIR) $(OBJECT_PATH)
	@$(RM) $@
	@echo Building $@
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

$(OBJECT_PATH)/%.o : utils/%.c
	@$(MKDIR) $(OBJECT_PATH)
	@$(RM) $@
	@echo Building $@
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

$(OBJECT_PATH)/%.o : ipc/%.c
	@$(MKDIR) $(OBJECT_PATH)
	@$(RM) $@
	@echo Building $@
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

$(OBJECT_PATH)/%.o : main/%.c
	@$(MKDIR) $(OBJECT_PATH)
	@$(RM) $@
	@echo Building $@
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<


