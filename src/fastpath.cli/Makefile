#MAJOR.MINOR.REVISION.BUILD
MAJOR = \"1\"
MINOR = \"1\" 
REVISION=\"0\"
X1= $(shell date +%H)*60+$(shell date +%M)
X2 = $(shell echo $(X1) | bc)
BUILD=\"$(shell date +%y%m%d)-$(X2)\"

# Makefile for a multifile kernel module
CROSS ?= /opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux/bin/powerpc-e300c3-linux-
#LIB = 
CC=$(CROSS)gcc
LD=$(CROSS)ld

CFLAGS=-DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g 
#  -DMQ_DEBUG \
OPTIMIZACAO = -O6

#LDFLAGS=-static
LDFLAGS		= -Wl,-Bdynamic
#,-L$(LIB)
#LDFLAGS=-L/usr/local/pq2-linux-4.0.2/powerpc-e300c3-linux/lib

RM=rm -f
TARGET=bin/fastpath.cli

INSTALL_DIR1=../../../PR1003/builds_olt360/apps/CXP360G
INSTALL_DIR2=../../../PR1003/builds_olt360/apps/TOLT8G

#PONJACKET_DIR=../SW-BL3458PONJacket_1_6_1413
CROSS_DIR=/usr/local/pq2-linux-4.0.2

HDRS=-I. -Iutils -Iipc -Imain \
#-I$(CROSS_DIR)/powerpc-e300c3-linux/include \
#-I$(LINUX_DIR)/include

#-I/usr/local/pq2-linux-ptin/include
#-I/usr/local/pq2-linux-ptin/include \
#-I/usr/include/PCSC
				
OBJ_MAIN = obj/main.o obj/utils.o obj/memchk.o 
		
OBJ_CTRL = obj/ipc_lib.o


OBJ =  $(OBJ_CTRL) $(OBJ_MAIN)
		     
all:	Makefile $(OBJ)
	       	$(CC) $(LDFLAGS) $(OPTIMIZACAO) -o $(TARGET) $(OBJ) -lc -lm -lrt
   
install:	$(TARGET)
		cp $(TARGET) $(INSTALL_DIR1)
		cp $(TARGET) $(INSTALL_DIR2)
	
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


