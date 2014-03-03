#MAJOR.MINOR.REVISION.BUILD
MAJOR = \"1\"
MINOR = \"1\" 
REVISION=\"0\"
X1= $(shell date +%H)*60+$(shell date +%M)
X2 = $(shell echo $(X1) | bc)
BUILD=\"$(shell date +%y%m%d)-$(X2)\"

# Makefile for a multifile kernel module
CROSS ?= /opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr/bin/ppce500mc-fsl-linux/powerpc-fsl-linux-
LIB ?= /opt/fsl/1.2/sysroots/i686-fslsdk-linux/lib
CC=$(CROSS)gcc
LD=$(CROSS)ld

CFLAGS=-DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g 
#  -DMQ_DEBUG \
OPTIMIZACAO = -O6

#LDFLAGS=-static
LDFLAGS		= -Wl,-Bdynamic,-L$(LIB)
#LDFLAGS=-L/usr/local/pq2-linux-4.0.2/powerpc-e300c3-linux/lib

RM=rm -f
TARGET=bin/fastpath.shell

HDRS=-I. -Iutils -Iipc -Imain \
#-I$(CROSS_DIR)/powerpc-e300c3-linux/include
				
OBJ_MAIN = obj/main.o obj/utils.o obj/memchk.o 
OBJ_CTRL = obj/ipc_lib.o


OBJ =  $(OBJ_CTRL) $(OBJ_MAIN)
		     
all:	Makefile $(OBJ)
	       	$(CC) $(LDFLAGS) $(OPTIMIZACAO) -o $(TARGET) $(OBJ) -lc -lm -lrt
   
install:	$(TARGET)
		sh shell.install
	
clean cleanall:
		rm -f $(OBJ) $(TARGET)
								 
vpath %.c       # Search path for *.c files

#
# Rule for C compilation
#
obj/%.o : %.c
	$(RM) $@
	@echo Building $@
	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

obj/%.o : utils/%.c
	$(RM) $@
	@echo Building $@
	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

obj/%.o : ipc/%.c
	$(RM) $@
	@echo Building $@
	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

obj/%.o : main/%.c
	$(RM) $@
	@echo Building $@
	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<


