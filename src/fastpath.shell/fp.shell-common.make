#MAJOR.MINOR.REVISION.BUILD
MAJOR = \"1\"
MINOR = \"1\" 
REVISION=\"0\"
X1= $(shell date +%H)*60+$(shell date +%M)
X2 = $(shell echo $(X1) | bc)
BUILD=\"$(shell date +%y%m%d)-$(X2)\"

RM=rm -f
TARGET=fp.shell

HDRS=-I. -Iutils -Iipc -Imain \
	
OBJ_MAIN = obj/main.o obj/utils.o obj/memchk.o 
OBJ_CTRL = obj/ipc_lib.o


OBJ =  $(OBJ_CTRL) $(OBJ_MAIN)
		     
all:	Makefile $(OBJ)
		@echo Generating $(TARGET_PATH)/$(TARGET)
		@mkdir -p $(TARGET_PATH)
		@$(CC) $(LDFLAGS) $(OPTIMIZACAO) -o $(TARGET_PATH)/$(TARGET) $(OBJ) -lc -lm -lrt
   
install:	$(TARGET)
		sh shell.install
	
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
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

obj/%.o : utils/%.c
	@$(RM) $@
	@echo Building $@
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

obj/%.o : ipc/%.c
	@$(RM) $@
	@echo Building $@
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

obj/%.o : main/%.c
	@$(RM) $@
	@echo Building $@
	@$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall $(HDRS) -c -o $@ $<
# claudia	$(CC) $(CFLAGS) $(OPTIMIZACAO) -Wall -Werror $(HDRS) -c -o $@ $<

