# $Id$
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# PCIEPHY make tools
#

# Compiler command for generating dependencies
PCIEPHY_DEPEND ?= $(CC) -M -P $(CPPFLAGS) $<

# Perl is required for portable build tools
ifndef PCIEPHY_PERL
PCIEPHY_PERL = perl
endif

ifeq (n/a,$(PCIEPHY_PERL))
#
# If perl is not available, try building using standard UNIX utilities
#
RM      = rm -f
MKDIR   = mkdir -p
CP      = cp -d
ECHO    = echo
PRINTF  = printf
else
#
# If perl is available, use portable build tools
#
MKTOOL  = $(PCIEPHY_PERL) $(PCIEPHY)/make/mktool.pl
RM      = $(MKTOOL) -rm
MKDIR   = $(MKTOOL) -md
FOREACH = $(MKTOOL) -foreach
CP      = $(MKTOOL) -cp
MAKEDEP = $(MKTOOL) -dep
ECHO    = $(MKTOOL) -echo
PRINTF  = $(MKTOOL) -echo -n
MKBEEP  = $(MKTOOL) -beep
endif
