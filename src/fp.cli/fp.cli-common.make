#MAJOR.MINOR.REVISION.BUILD
MAJOR = \"1\"
MINOR = \"1\" 
REVISION=\"0\"
X1= $(shell date +%H)*60+$(shell date +%M)
X2 = $(shell echo $(X1) | bc)
BUILD=\"$(shell date +%y%m%d)-$(X2)\"

RM=rm -f
TARGET=fp.cli

HDRS=-I. -Iutils -Iipc -Imain
#-I$(CROSS_DIR)/powerpc-e300c3-linux/include \
#-I$(LINUX_DIR)/include

#-I/usr/local/pq2-linux-ptin/include
#-I/usr/local/pq2-linux-ptin/include \
#-I/usr/include/PCSC

OBJ = obj/cli_olt.o obj/cli_olt_fw_layer.o obj/memchk.o obj/utils.o obj/ipc_lib.o
		
all:	Makefile $(OBJ)
		@echo Generating $(TARGET_PATH)/$(TARGET)
		@mkdir -p $(TARGET_PATH)
	       	@$(CC) $(LDFLAGS) $(OPTIMIZACAO) -o $(TARGET_PATH)/$(TARGET) $(OBJ) -lc -lm -lpthread -lrt
   
install:	$(TARGET)
		sh cli.install
	
clean cleanall:
		@echo Cleaning $(TARGET)
		@rm -f $(OBJ) $(TARGET_PATH)/$(TARGET)

vpath %.c       # Search path for *.c files

#
# Rule for C compilation
#
obj/%.o : %.c
	@$(RM) $@
	@echo Building $@
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

