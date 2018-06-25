# $Id: Makefile 1.15 Broadcom SDK $
# $Copyright: Copyright 2012 Broadcom Corporation.
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

PLATFORM=katanaarm
KERNEL_VER=2_6

SDK = $(CURDIR)
export SDK

export TOOLCHAIN_BASE_DIR = /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/host/usr
#/home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/host/usr
#/home/devtools/dev-bcm-ldk/3.4.9-RC5/buildroot-2013.11-gcc48-opt-broadcom/host/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib

export COMPILER           = $(TOOLCHAIN_BIN_DIR)/arm-linux-
export KERNEL_PATH        = /home/devtools/dev-ARM/ldk_3.4.7-RC4_target_olt1t0f_tg16f/iproc/kernel/linux-3.6.5
#/home/olt/svnrepo/olt-switchsw/trunk/lib/kernel/linux-3.6.5-arm
export KERNDIR = $(KERNEL_PATH)

export TARGET_ARCHITECTURE = arm-broadcom-linux-uclibcgnueabi
export KFLAG_INCLD = $(TOOLCHAIN_BASE_DIR)/lib/gcc/$(TARGET_ARCHITECTURE)/4.7.2/include

export CROSS_COMPILE := $(COMPILER)
export KERNEL_SRC    := $(KERNEL_PATH)
export KERNDIR       := $(KERNEL_PATH)

export SDKBUILD=build


SRC_PATH=$(SDK)/systems/linux/user
DST_PATH=$(SDK)/bin
PLATFORM_NAME=$(PLATFORM)-$(KERNEL_VER)

.PHONY: all clean

all:
	echo $(KFLAG_INCLD)
	gmake -C $(SRC_PATH)/$(PLATFORM_NAME)
	mkdir -p $(SDK)/$(SDKBUILD)
	mkdir -p $(DST_PATH)/$(PLATFORM_NAME)
	cp -v $(SRC_PATH)/$(PLATFORM_NAME)/bcm.user* $(SRC_PATH)/$(PLATFORM_NAME)/*.ko $(SRC_PATH)/$(PLATFORM_NAME)/netserve $(DST_PATH)/$(PLATFORM_NAME)

clean:
	gmake -C $(SRC_PATH)/$(PLATFORM_NAME) clean
	rm -f $(SRC_PATH)/$(PLATFORM_NAME)/bcm.user* $(SRC_PATH)/$(PLATFORM_NAME)/*.ko $(SRC_PATH)/$(PLATFORM_NAME)/netserve

cleanall: clean
	rm -rf $(SDK)/$(SDKBUILD)/*
	rm -rf $(DST_PATH)/$(PLATFORM_NAME)

