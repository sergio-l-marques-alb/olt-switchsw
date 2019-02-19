# This is a package-level makefile used to build a partially-linked package library
#
#   - PKGID designates package identifier used for customizing common make var names
#   - PKGNAME designates the output name of this package
#
PKGID   := XWEB
PKGNAME := $(ROOT_$(PKGID))

SRC_DIR=$(FP_ROOT)/src/xui/framework
DST_DIR=../xui

C_CONTENT := $(DST_DIR)/content/obj.tmp
H_CONTENT := $(DST_DIR)/content/html

EWC=$(SRC_DIR)/tools/emweb/ewc
EWFLAGS=--raw  -m $(SRC_DIR)/config/emweb/mime.types
EWC_CMD=$(DBG) $(EWC) $(EWFLAGS) --no-strict-const -o $(C_CONTENT)/ew_ -c $(H_CONTENT) 
EW_CMD=$(DBG) perl $(SRC_DIR)/tools/emweb/ew_code.pl $(C_CONTENT)/ew_code.c

$(C_CONTENT)/ew_code.c :
	$(EWC_CMD) 
	$(EW_CMD)

bldlib : $(C_CONTENT)/ew_code.c

clean :
	$(DBG) rm -rf $(C_CONTENT)/ew_*.*





