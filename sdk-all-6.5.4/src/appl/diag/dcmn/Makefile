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
# $Id: Makefile,v 1.10 Broadcom SDK $
LOCALDIR = src/appl/diag/dcmn

include ${SDK}/make/Make.config

INCDIR += -I${SDK}/include/shared

lib = libdiag_dcmn

CFGFLAGS += -std=gnu99 -D_GNU_SOURCE

OBJS  = \
	utils_board_general.o \
	cmdlist.o \
	counter.o \
	switchctl.o \
	port.o \
	mem.o \
	reg.o \
	soc.o \
	srcell.o \
	init.o \
	counters_data.o \
	dma.o \
	rate_calc.o \
	diag_egq.o \
	diag.o \
	diag_pp.o \
	diag_oam.o \
	diag_field.o \
	ddr.o \
	gport.o \
	diag_alloc.o \
	diag_sw_state_dump.o \
	dcmn_patch_database.o \
	fabric.o \
	cosq.o \
	init_deinit.o \
	diag_buffers.o \
	diag_mmu.o \
	rx.o \
	diag_signals_dump.o \
	diag_prt_print.o \
	diag_fc.o

ifdef DPP_CHIPS
OBJS += \
	export.o	\
	diag_pp_data.o \
	dpp_debug_signals.o \
	credit_request_thresholds.o
endif

ifdef BCM_88640_A0
ifeq (DUNE_UI,$(findstring DUNE_UI,$(FEATURE_LIST)))
endif
endif

ifeq (vxworks-gto, $(TARGET))
OBJS  += bsp_cards_consts.o
endif

ifeq (gto-2_6, $(platform))
OBJS  += bsp_cards_consts.o
endif

ifeq (vxworks-negev,$(TARGETTYPE))
OBJS  += bsp_cards_consts.o
endif

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
