# $Id: Makefile,v 1.2 Broadcom SDK $
# $Copyright: Copyright 2015 Broadcom Corporation.
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

export TARGET
export BLDDIR
export LIBDIR

LOCALDIR = src/appl/dpp/UserInterface

include ${SDK}/make/Make.config

subdirs += ppd pcp

ifdef BCM_88640_A0
subdirs += pb
endif

include ${SDK}/make/Make.subdirs

lib = libappl_dpp_ui

OBJS  = \
      ui_fe600_api.o   \
      ui_general_hrp.o \
      ui_history.o    \
      ui_mem_init.o   \
      ui_memory_hrp.o \
      ui_module.o     \
      ui_nvram_hrp.o  \
      ui_pb_api.o     \
      ui_pb_pp_ind_acc.o \
      ui_petra_acc.o  \
      ui_petra_api.o  \
      ui_petra_pp_acc.o \
      ui_petra_pp_api.o \
      ui_ppd_api.o \
      ui_process.o \
      ui_sand.o    \
      ui_sweep_app_b.o  \
      ui_swp_p_pp_api.o \
      ui_utils.o \
      ui_pb_pp_ind_acc2.o \
      ui_pcp_api.o
#      ui_ppa_api.o 

ifeq (vxworks-negev,$(TARGETTYPE))
OBJS  += \
      ui_diag.o	         \
      ui_fe600_bsp.o     \
      ui_fe_gen_regs.o   \
      ui_fe_indirect.o   \
      ui_fmf.o           \
      ui_gsa_api.o       \
      ui_gsa_petra_api.o \
      ui_line_card.o  	 \
      ui_line_dhrp.o  	 \
      ui_line_gfa.o   	 \
      ui_line_tevb.o  	 \
      ui_line_tgs.o   	 \
      ui_negev_demo.o 	 \
      ui_petra_gfa.o     \
      ui_ptg.o           \
      ui_sweept20_api.o  \
      ui_tmd_api.o       \
      ui_cli_files.o 
endif

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
