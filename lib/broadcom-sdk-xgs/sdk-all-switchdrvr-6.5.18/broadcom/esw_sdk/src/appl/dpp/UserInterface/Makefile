# $Id: Makefile,v 1.2 Broadcom SDK $
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# 

export TARGET
export BLDDIR
export LIBDIR

LOCALDIR = src/appl/dpp/UserInterface

include ${SDK}/make/Make.config

subdirs += ppd

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
      ui_petra_pp_acc.o \
      ui_petra_pp_api.o \
      ui_ppd_api.o \
      ui_process.o \
      ui_sand.o    \
      ui_sweep_app_b.o  \
      ui_swp_p_pp_api.o \
      ui_utils.o 

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
