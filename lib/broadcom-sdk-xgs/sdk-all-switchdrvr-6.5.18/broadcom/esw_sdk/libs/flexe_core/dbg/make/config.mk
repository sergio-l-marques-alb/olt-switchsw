# $Id$
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# FLEXE make rules and definitions
#

#
# Provide reasonable defaults for configuration variables
#

# Default build directory
ifndef FLEXE_BLDDIR
FLEXE_BLDDIR = $(FLEXE)/build
endif

# Location to build objects in
ifndef FLEXE_OBJDIR
FLEXE_OBJDIR = $(FLEXE_BLDDIR)/obj
endif
override BLDDIR := $(FLEXE_OBJDIR)

# Location to place libraries
ifndef FLEXE_LIBDIR
FLEXE_LIBDIR = $(FLEXE_BLDDIR)
endif
LIBDIR := $(FLEXE_LIBDIR)

# Option to retrieve compiler version
ifndef FLEXE_CC_VERFLAGS
FLEXE_CC_VERFLAGS := -v
endif
CC_VERFLAGS = $(FLEXE_CC_VERFLAGS);

# Default suffix for object files
ifndef FLEXE_OBJSUFFIX
FLEXE_OBJSUFFIX = o
endif
OBJSUFFIX = $(FLEXE_OBJSUFFIX)

# Default suffix for library files
ifndef FLEXE_LIBSUFFIX
FLEXE_LIBSUFFIX = a
endif
LIBSUFFIX = $(FLEXE_LIBSUFFIX)

#
# Set up compiler options, etc.
#

# Default include path
FLEXE_INCLUDE_PATH = -I$(FLEXE)/bsp/inc
FLEXE_INCLUDE_PATH += -I$(FLEXE)/lib/inc
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/66bswitch
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/cpb
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/mux
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/demux
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/sar
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/flexe_oh
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/module1588
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/oam
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/init
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/mcmac
FLEXE_INCLUDE_PATH += -I$(FLEXE)/chip/module/rateadpt
FLEXE_INCLUDE_PATH += -I$(FLEXE)/top
INCDIR += $(FLEXE_INCLUDE_PATH)
# Import preprocessor flags avoiding include duplicates
TMP_FLEXE_CPPFLAGS := $(filter-out $(FLEXE_INCLUDE_PATH),$(FLEXE_CPPFLAGS))

TMP_FLEXE_CPPFLAGS += $(FLEXE_DSYM_CPPFLAGS)
export FLEXE_DSYM_CPPFLAGS

override CPPFLAGS = $(TMP_FLEXE_CPPFLAGS) $(FLEXE_INCLUDE_PATH)


# Import compiler flags
override CFLAGS = $(FLEXE_CFLAGS)




#
# Define standard targets, etc.
#

ifdef LOCALDIR
override BLDDIR := $(BLDDIR)/$(LOCALDIR)
endif

ifndef LSRCS
LSRCS = $(wildcard *.c)
endif
ifndef LOBJS
LOBJS = $(addsuffix .$(OBJSUFFIX), $(basename $(LSRCS)))
endif
ifndef BOBJS
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))
endif

# Use FLEXE_QUIET=1 to control printing of compilation lines.
ifdef FLEXE_QUIET
Q = @
endif

#
# Define rules for creating object directories
#
#
#	mkdir -p $(dir $@)
.PRECIOUS: $(BLDDIR)/.tree

%/.tree:
	@$(ECHO) 'Creating build directory $(dir $@)'
	$(Q)$(MKDIR) $(dir $@)
	@$(ECHO) "Build Directory for $(LOCALDIR) created" > $@
#
# Configure build tools
#
include $(FLEXE)/make/maketools.mk
