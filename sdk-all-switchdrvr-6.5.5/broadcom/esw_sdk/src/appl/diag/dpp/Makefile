#
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
# $Id: Makefile,v 1.11 Broadcom SDK $
LOCALDIR = src/appl/diag/dpp

include ${SDK}/make/Make.config

INCDIR += -I${SDK}/include/soc/dpp


CFLAGS += -DCINT_CONFIG_INCLUDE_SDK_SAL=1
CFLAGS += -DCINT_CONFIG_INCLUDE_STUBS=0

lib = libdiag_dpp


OBJS  = \
	l2.o \
	l3.o \
	vlan.o \
	utils_gen_err_ssf.o \
	txrx.o \
	dpp.o \
	sw_state_dump.o \
	fabric.o \
	stg.o \
	test_oam.o \
	test_bfd.o

ifeq (KBP,$(findstring KBP,$(FEATURE_LIST)))
ifneq (,$(findstring KBP_12K, $(KBP_DEVICE)))
OBJS += kbp.o
endif
endif

ifeq (KBP,$(findstring KBP,$(FEATURE_LIST)))
ifeq (KBP_ALG,$(findstring KBP_ALG, $(KBP_DEVICE)))
OBJS += kbp.o
endif
endif

ifeq (KNET,$(findstring KNET,$(FEATURE_LIST)))
OBJS += knetctrl.o
endif

ifeq (DUNE_UI,$(findstring DUNE_UI,$(FEATURE_LIST)))
OBJS += utils_char_queue_ssf.o \
	utils_tid_task.o \
	utils_string.o 	\
	utils_d_os_ssf.o
endif

ifeq (gto-2_6, $(platform))
OBJS  += \
        utils_arad_card.o
endif

ifeq (wrx-3_7, $(platform))
OBJS  += \
        utils_arad_card.o
endif


ifeq (vxworks-negev,$(TARGETTYPE))
OBJS  += \
	bsp_drv_flash28f.o \
	bsp_drv_flash28f_utils.o \
	bsp_info_access.o \
	dffs_driver.o \
	dffs_low.o \
	utils_dffs_cpu_mez.o \
	ref_design_petra_starter.o \
	ref_design_starter.o \
	utils_dune_fpga_download.o \
	utils_front_end_host_card.o \
	utils_host_board.o \
	utils_i2c_mem.o	\
	utils_line_PTG.o		
endif

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
