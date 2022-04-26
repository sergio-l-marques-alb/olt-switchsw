# $Id$
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# AAPL make rules and definitions
#

#
# Provide reasonable defaults for configuration variables
#

# Default build directory
ifndef AAPL_BLDDIR
AAPL_BLDDIR = $(AAPL)/build
endif

# Location to build objects in
ifndef AAPL_OBJDIR
AAPL_OBJDIR = $(AAPL_BLDDIR)/obj
endif
override BLDDIR := $(AAPL_OBJDIR)

# Location to place libraries
ifndef AAPL_LIBDIR
AAPL_LIBDIR = $(AAPL_BLDDIR)
endif
LIBDIR := $(AAPL_LIBDIR)

# Option to retrieve compiler version
ifndef AAPL_CC_VERFLAGS
AAPL_CC_VERFLAGS := -v
endif
CC_VERFLAGS = $(AAPL_CC_VERFLAGS);

# Default suffix for object files
ifndef AAPL_OBJSUFFIX
AAPL_OBJSUFFIX = o
endif
OBJSUFFIX = $(AAPL_OBJSUFFIX)

# Default suffix for library files
ifndef AAPL_LIBSUFFIX
AAPL_LIBSUFFIX = a
endif
LIBSUFFIX = $(AAPL_LIBSUFFIX)

#
# Set up compiler options, etc.
#

# Default include path
AAPL_INCLUDE_PATH = -I$(AAPL)/include
INCDIR += $(AAPL_INCLUDE_PATH)
# Import preprocessor flags avoiding include duplicates
TMP_AAPL_CPPFLAGS := $(filter-out $(AAPL_INCLUDE_PATH),$(AAPL_CPPFLAGS))

TMP_AAPL_CPPFLAGS += $(AAPL_DSYM_CPPFLAGS)
export AAPL_DSYM_CPPFLAGS

override CPPFLAGS = $(TMP_AAPL_CPPFLAGS) $(AAPL_INCLUDE_PATH)


# Import compiler flags
override CFLAGS = $(AAPL_CFLAGS)




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

# Use AAPL_QUIET=1 to control printing of compilation lines.
ifdef AAPL_QUIET
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
include $(AAPL)/make/maketools.mk
