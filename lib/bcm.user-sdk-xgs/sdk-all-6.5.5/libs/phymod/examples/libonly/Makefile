#
# $Id$
# $Copyright: Copyright 2016 Broadcom Corporation.
# This program is the proprietary software of Broadcom Corporation
# and/or its licensors, and may only be used, duplicated, modified
# or distributed pursuant to the terms and conditions of a separate,
# written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized
# License, Broadcom grants no license (express or implied), right
# to use, or waiver of any kind with respect to the Software, and
# Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE
# NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
# IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
# ALL USE OF THE SOFTWARE.  
#  
# Except as expressly set forth in the Authorized License,
#  
# 1.     This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use
# all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of
# Broadcom integrated circuit products.
#  
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
# PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
# REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
# OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
# DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
# NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
# ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
# CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
# OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
# 
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
# BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
# INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
# ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
# TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#
#
# This makefile builds the PHYMOD library with different options to
# validate the code portability.
#
# Run 'make help' for additional info.
#

PHYMOD= $(CURDIR)/../..

PHYMOD_CPPFLAGS += -I$(CURDIR)
PHYMOD_CPPFLAGS += -I$(PHYMOD)/include -I${SDK}/include
PHYMOD_CPPFLAGS += -DPHYMOD_INCLUDE_CUSTOM_CONFIG 

PHYMOD_CFLAGS += -Wall -Werror

PHYMOD_BLDDIR = $(CURDIR)/build

export PHYMOD_CPPFLAGS
export PHYMOD_CFLAGS
export PHYMOD_BLDDIR

export CC = gcc
export ARFLAGS = rc

ifeq (,$(MAKECMDGOALS))
override NOCLEAN = 0
endif

# NOSTOP=1 allows multi-target regression to continue after error
ifeq (1,$(NOSTOP))
TRUE = true
endif

# Default target compiles with no special options
AVAILABLE_TARGETS = default

# Enforce check for function declarations
AVAILABLE_TARGETS += chkproto
ifneq (,$(findstring chkproto,$(MAKECMDGOALS)))
PHYMOD_CFLAGS += -Wmissing-prototypes
endif

# Enforce check for ANSI C compliance
AVAILABLE_TARGETS += pedantic
ifneq (,$(findstring pedantic,$(MAKECMDGOALS)))
PHYMOD_CFLAGS += -ansi -pedantic
endif

# Miscellaneous MDK checks
AVAILABLE_TARGETS += miscmdk
ifneq (,$(findstring miscmdk,$(MAKECMDGOALS)))
PHYMOD_CFLAGS += -Wsign-compare -Wundef -Wunused -Wshadow
endif

# PHY_DIAG is enabled by default - allow compilation without this
AVAILABLE_TARGETS += nodiag
ifeq (,$(findstring nodiag,$(MAKECMDGOALS)))
PHYMOD_CPPFLAGS += -DPHYMOD_DIAG=1
endif

# Redefine uint32_t to check proper use of PRIx32 in printf format strings
AVAILABLE_TARGETS += longuint32
ifneq (,$(findstring longuint32,$(MAKECMDGOALS)))
PHYMOD_CPPFLAGS += -DTOOLS_UINT32_IS_LONG
PHYMOD_CFLAGS += -m32
endif

# Check for invalid use of int8_t, int16_t and int32_t in PHYMOD API
AVAILABLE_TARGETS += nosignedbase
ifneq (,$(findstring nosignedbase,$(MAKECMDGOALS)))
PHYMOD_CPPFLAGS += -DTOOLS_NO_SIGNED_STDINT
override PHYMOD_CHIPS = NONE
endif

# Check for int8_t, int16_t and int32_t also in chip drivers
AVAILABLE_TARGETS += nosignedchip
ifneq (,$(findstring nosignedchip,$(MAKECMDGOALS)))
PHYMOD_CPPFLAGS += -DTOOLS_NO_SIGNED_STDINT
endif

# Skip nosignedchip target from standard regression
ALL_TARGETS = $(filter-out nosignedchip,$(AVAILABLE_TARGETS))

all: $(ALL_TARGETS)

$(AVAILABLE_TARGETS):
	@echo
ifneq ($(NOCLEAN),1)
	make -C ${PHYMOD} clean
	@echo 
endif
	@echo "*****************************"
	@echo Building $@ 1>&2
	@echo "*****************************"
	@echo 
	(make -C $(PHYMOD) PHYMOD_CHIPS=$(PHYMOD_CHIPS);$(TRUE))

clean:
	make -C ${PHYMOD} clean

help:
	@echo
	@echo "make [<target>]"
	@echo
	@echo "Please choose one of the following targets:"
	@echo "all $(AVAILABLE_TARGETS)"
	@echo
	@echo "Set PHYMOD_CHIPS=<chip> to build only for a single chip."
	@echo
	@echo "Set NOCLEAN=1 to suppress 'make clean'."
	@echo
	@echo "Set NOSTOP=1 to continue multi-target regression despite errors."
	@echo
	@echo "Examples:"
	@echo "make > /dev/null"
	@echo "make -s PHYMOD_CHIPS=EAGLE pedantic"
	@echo "make -s PHYMOD_CHIPS=NONE NOCLEAN=1 pedantic"
	@echo

.PHONY: all clean help $(AVAILABLE_TARGETS)
