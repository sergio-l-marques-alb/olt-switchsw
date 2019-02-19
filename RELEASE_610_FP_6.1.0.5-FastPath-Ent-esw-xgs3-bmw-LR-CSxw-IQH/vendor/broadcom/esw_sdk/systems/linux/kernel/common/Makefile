# -*- Makefile -*-
# $Id: Makefile,v 1.1 2011/04/18 17:11:10 mruas Exp $
# $Copyright: Copyright 2009 Broadcom Corporation.
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

user-override=unix-user

all: check kernel_modules user_apps kernel_targets link_check

check:
ifeq (,$(SDK))
	@echo 'Makefile: error: The $$SDK environment variable is not set'; exit 1
else
ifeq (,$(platform))
	@echo 'Makefile: error: Internal error: platform variable is not set'; exit 1
else

override-target=linux-$(platform)
include $(SDK)/make/Make.config
endif
endif

ifeq (,$(kernel_version))
kernel_version=2_4
endif

ifeq ($(kernel_version),2_6)
KOBJ=ko
else
KOBJ=o
endif

ifeq (,$(DESTDIR))
DESTDIR=$(SDK)/systems/linux/kernel/$(platform)
endif

BCM_PROXY=$(DESTDIR)/bcm.user.proxy

KERNEL_BDE=$(DESTDIR)/linux-kernel-bde.$(KOBJ)
KERNEL_DIAG_FULL=$(DESTDIR)/linux-bcm-diag-full.$(KOBJ)
KERNEL_PROXY=$(DESTDIR)/linux-uk-proxy.$(KOBJ)
KERNEL_CORE=$(DESTDIR)/linux-bcm-core.$(KOBJ)
KERNEL_NET=$(DESTDIR)/linux-bcm-net.$(KOBJ)
KERNEL_DIAG=$(DESTDIR)/linux-bcm-diag.$(KOBJ)

KERNEL_TARGETS=$(KERNEL_BDE) $(KERNEL_DIAG_FULL) $(KERNEL_PROXY) \
               $(KERNEL_CORE) $(KERNEL_NET) $(KERNEL_DIAG)

lmsc_platform=$(platform)

ifeq (2.6.24.4, $(KERN_VER))
lmsc_platform=$(platform).2.6.24
endif

ifeq (2.6.25, $(KERN_VER))
lmsc_platform=$(platform).2.6.25
endif

kernel_modules:
	$(MAKE) -C $(SDK)/src
	$(MAKE) -C $(SDK)/systems/drv
ifdef BUILD_MACSEC
	@$(ECHO) "Building MACSEC libraries ...."
	$(MAKE) -C ${MACSEC_HOME} MACSEC_CC="${CC}" MACSEC_LD="${LD}" MACSEC_AR="$(AR)" MACSEC_CFLAGS="${MACSEC_BUILD_FLAG}" MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" all
endif
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules kernel_version=$(kernel_version)
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel

user_apps:
	$(MAKE) -C $(SDK)/systems/linux/kernel/user/bcm-diag-proxy \
		 BCM_PROXY=$(BCM_PROXY) \
		 override-target=$(user-override) \
		 bldroot_suffix=/$(override-target)

kernel_targets: $(KERNEL_TARGETS)


#
# Define the set of allowable link dependencies. 
# All configurations in the LINK_CHECKS variable
# must be clean. 
#
LINK_CHECKS = $(KERNEL_BDE)
LINK_CHECKS += $(KERNEL_PROXY)
LINK_CHECKS += $(KERNEL_BDE),$(KERNEL_CORE)
LINK_CHECKS += $(KERNEL_BDE),$(KERNEL_CORE),$(KERNEL_NET)
LINK_CHECKS += $(KERNEL_BDE),$(KERNEL_CORE),$(KERNEL_PROXY),$(KERNEL_DIAG)
LINK_CHECKS += $(KERNEL_BDE),$(KERNEL_PROXY),$(KERNEL_DIAG_FULL)

link_check: kernel_targets 
	$(PERL) $(SDK)/tools/lmsc.pl ferror=0 kernel=$(lmsc_platform) $(LINK_CHECKS)

$(KERNEL_BDE): $(BLDROOT)/linux-kernel-bde.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_DIAG_FULL): $(BLDDIR)/linux-bcm-diag-full.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_PROXY): $(BLDDIR)/linux-uk-proxy.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_CORE): $(BLDDIR)/linux-bcm-core.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_NET): $(BLDDIR)/linux-bcm-net.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_DIAG): $(BLDDIR)/linux-bcm-diag.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

clean:
	$(MAKE) -C $(SDK)/src $@
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules $@
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel $@
	$(MAKE) -C $(SDK)/systems/linux/kernel/user/bcm-diag-proxy \
		 BCM_PROXY=$(BCM_PROXY) \
		 override-target=$(user-override) \
		 bldroot_suffix=/$(override-target) $@
	$(RM) $(KERNEL_TARGETS)
ifdef BUILD_MACSEC
	$(MAKE) -C ${MACSEC_HOME} MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" clean
endif

distclean: clean

.PHONY: variable

#
# Echo variable values used for configuration
# usage: make VAR=CC variable
#
variable::
	@echo $($(VAR))

# Make.config defines remaining phony targets
.PHONY: check kernel_modules user_apps kernel_targets

