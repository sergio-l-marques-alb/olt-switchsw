# -*- Makefile -*-
# $Id: Makefile,v 1.56 Broadcom SDK $
# $Copyright: Copyright 2008 Broadcom Corporation.
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
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#

override-target=unix-user

all: check build

ifeq ($(LINUX_MAKE_SHARED_LIB),1)
libext = so.$(SHAREDLIBVER)
else
libext = a
endif

check:
ifeq (,$(SDK))
	@echo 'The $$SDK environment variable is not set'; exit 1
else
ifeq (,$(platform))
	@echo 'Internal error: platform variable is not set'; exit 1
else
include $(SDK)/make/Make.config
LOCALDIR=systems/linux/user/fastpath_common
include $(SDK)/make/Make.depend
kernel-override=linux-$(platform)
ifdef LVL7_FIXUP
KERN_BLDROOT=$(L7_OUTPUT)/$(kernel-override)$(bldroot_suffix)
DEST_DIR=$(L7_OUTPUT_KMOD)
else
ifdef BLDCONFIG
KERN_BLDROOT=$(SDK)/build/$(BLDCONFIG)/$(kernel-override)$(bldroot_suffix)
else
KERN_BLDROOT=$(SDK)/build/$(kernel-override)$(bldroot_suffix)
endif
endif # end LVL7_FIXUP
endif
endif

ifeq (,$(kernel_version))
kernel_version=2_4
endif

ifeq ($(kernel_version),2_6)
ADD_TO_CFLAGS += -DLKM_2_6
KOBJ=ko
else
ADD_TO_CFLAGS += -DLKM_2_4
KOBJ=o
endif

ifneq (,$(TOOLS_DIR))
override PATH := $(TOOLS_DIR):$(PATH)
endif

ifeq ($(DEST_DIR),)
DEST_DIR=../$(platform)
endif

BCM = $(DEST_DIR)/bcm.$(targetplat)$(all_suffix)

KERNEL_BDE=$(DEST_DIR)/linux-kernel-bde.$(KOBJ)
USER_BDE=$(DEST_DIR)/linux-user-bde.$(KOBJ)

NETSERVE = $(DEST_DIR)/netserve

ifdef LVL7_FIXUP
all_targets = kernel_modules user_libs $(KERNEL_BDE) $(USER_BDE)
else
all_targets = kernel_modules user_libs $(KERNEL_BDE) $(USER_BDE) $(BCM) $(NETSERVE)
endif


# Define targets to be built by default.
build_targets = $(all_targets)

ADD_TO_CFLAGS += -I$(SDK)/systems/linux/kernel/modules/include

# Target specific
ifndef LVL7_FIXUP
ADD_TO_CFLAGS += -DLINUX_SAL_DMA_ALLOC_OVERRIDE
#ADD_TO_CFLAGS += -DLINUX_SAL_USLEEP_OVERRIDE -DLINUX_SAL_SEM_OVERRIDE
#ADD_TO_CFLAGS += -DLINUX_SAL_UDELAY_OVERRIDE
else # Only override SEM
ADD_TO_CFLAGS += -DLINUX_SAL_SEM_OVERRIDE
endif

# User BDE libraries
ADD_TO_CFLAGS += -I$(SDK)/systems/bde/linux/include

CFLAGS += $(ADD_TO_CFLAGS)

ifdef ESW_CHIPS
ESW_DIAG_LIBS = libdiag_esw.a 
endif

ifdef ROBO_CHIPS
ROBO_DIAG_LIBS = libdiagrobo.a
endif

ifdef SBX_CHIPS
SBX_DIAG_LIBS = \
	libdiag_sbx.$(libext)
endif

DIAG_LIBS = \
        libdiag.a       \
        ${ESW_DIAG_LIBS} \
        ${ROBO_DIAG_LIBS} \
        ${SBX_DIAG_LIBS} \
	$(COND_LIB_EAV_APPL) \
	${COND_LIB_BCMX_DIAG} \
        ${COND_LIB_TEST} \
	$(COND_LIB_TCL) \
	$(APPL_LIBS) \
	$(COND_LIB_EDITLINE) \
	$(COND_LIB_WBDM) \
	$(COND_LIB_SMB) \
	$(COND_LIB_PHY_DIAGNOSTICS) \
	$(COND_LIB_BSAFE) \
	libsal_appl.a libsal_appl_plat.a liblubde.a

ACL_LIB = $(COND_LIB_ACL)
BCM_LIBS = $(DIAG_LIBS) $(DRV_LIBS) $(ACL_LIB) $(COND_LIB_ETH_DRV)
BCM_LIBS_BLD = $(addprefix $(BLDDIR)/,$(BCM_LIBS))

BCM_LIBS_BLD += ${EXTERNAL_LIBS}
ifdef LVL7_FIXUP
  ifdef QUIET
    entry_msg    = @${ECHO} Generating $(notdir $@)
    OBJCOPY      := @$(OBJCOPY)
  endif
endif


build:: $(BLDDIR)/.tree $(build_targets)

kernel_modules:
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_CFLAGS)"
	$(MAKE) -C $(SDK)/systems/bde/linux/user/kernel kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_CFLAGS)"

# LVL7_FIXUP :
# 1. Added $(MAKE) -C $(SDK)/systems/linux/user/common -f Makefile.fastpath CFLAGS="$(CFLAGS)"
# 2. Remove $(MAKE) -C $(SDK)/systems/drv CFLAGS="$(CFLAGS)"
user_libs:
	$(MAKE) -C $(SDK)/systems/linux/user/fastpath_common -f Makefile.fastpath CFLAGS="$(CFLAGS)"
	$(MAKE) -C $(SDK)/systems/bde/linux/user CFLAGS="$(CFLAGS)"
	$(MAKE) -C $(SDK)/src CFLAGS="$(ADD_TO_CFLAGS)"
ifdef BUILD_MACSEC
	@${ECHO} "Building MACSEC libraries ...."
	$(MAKE) -C ${MACSEC_HOME} MACSEC_CC="${CC}" MACSEC_LD="${LD}" MACSEC_AR="$(AR)" MACSEC_CFLAGS="${MACSEC_BUILD_FLAG}" MACSEC_TARGET="${targetbase}/${targetplat}/${platform}"
endif

# Broadcom Command Monitor

# Build platform_defines.c to compile in config.bcm
#
ifndef NO_SAL_APPL
PLATFORM_DEFINES_OBJ=${BLDDIR}/platform_defines.o
${BLDDIR}/platform_defines.c: ${SDK}/rc/config.bcm
	$(PERL) ${SDK}/tools/bcm2c_linux.pl ${SDK}/rc/config.bcm >$@
${BLDDIR}/platform_defines.o: ${BLDDIR}/platform_defines.c
	${CC} ${CFLAGS} -c $< -o $@
else
PLATFORM_DEFINES_OBJ=
endif


$(BCM): $(BCM).dbg
	$(OBJCOPY) --strip-debug $< $@


$(BCM).dbg: $(BLDDIR)/bdecreate.o $(BLDDIR)/version.o \
	${PLATFORM_DEFINES_OBJ} $(BCM_LIBS_BLD) 
	@rm -f $@	# Prevent core dump if currently running
	$(CC) $(CFLAGS) -o $@ $(BLDDIR)/bdecreate.o $(BLDDIR)/version.o \
	${PLATFORM_DEFINES_OBJ} $(BCM_LIBS_BLD) $(LDFLAGS)

$(KERNEL_BDE): $(KERN_BLDROOT)/linux-kernel-bde.$(KOBJ)
ifdef LVL7_FIXUP
	$(entry_msg)
endif
	$(OBJCOPY) --strip-debug $< $@

$(USER_BDE): $(KERN_BLDROOT)/linux-user-bde.$(KOBJ)
ifdef LVL7_FIXUP
	$(entry_msg)
endif
	$(OBJCOPY) --strip-debug $< $@

$(NETSERVE): $(BLDDIR)/netserve.o
	$(CC) $(CFLAGS) -o $@ $(BLDDIR)/netserve.o $(LDFLAGS) -lutil
	$(OBJCOPY) --strip-debug $@




# Build Version

$(BLDDIR)/version.c: $(BLDDIR)/bdecreate.o $(BCM_LIBS_BLD)
	$(PERL) $(SDK)/tools/version.pl > $@
$(BLDDIR)/version.o: $(BLDDIR)/version.c
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel $@
	$(MAKE) -C $(SDK)/systems/bde/linux/user/kernel $@
	$(MAKE) -C $(SDK)/systems/bde/linux/user CFLAGS="$(CFLAGS)" $@
	$(MAKE) -C $(SDK)/src CFLAGS="$(CFLAGS)" $@
	$(RM) $(BLDDIR)/version.c $(BLDDIR)/version.o
	$(RM) $(BLDDIR)/platform_defines.c $(BLDDIR)/platform_defines.o
	$(RM) $(BLDDIR)/bdecreate.o
	$(RM) $(KERNEL_BDE) $(USER_BDE) $(BCM) $(BCM).dbg
	$(RM) $(BLDDIR)/netserve.o
	$(RM) $(NETSERVE)

distclean:: clean

.PHONY: variable

#
# Echo variable values used for configuration
# usage: make VAR=CC variable
#
variable::
	@echo $($(VAR))

# Make.config defines remaining phony targets
.PHONY: build kernel_modules user_libs

# Output the CFLAGS used by the SDK
#
ifdef LVL7_FIXUP
.PHONY: get_sdk_flags

get_sdk_flags::
	@echo "CFLAGS_SDK := $(filter -D%,$(CFLAGS))" >$(L7_OUT_SDK_FLAGS)
endif
