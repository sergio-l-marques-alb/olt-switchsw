# $Id: Makefile,v 1.2.10.1 Broadcom SDK $
# $Copyright: (c) 2018 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
export TARGET
export BLDDIR
export LIBDIR

LOCALDIR = src/soc/dnx/legacy/SAND/Utils

include ${SDK}/make/Make.config

lib = libsoc_dnxlegacysandutils

OBJS = \
  sand_delta_list.o \
  sand_conv.o \
  sand_64cnt.o \
  sand_workload_status.o \
  sand_u64.o \
  sand_trace.o \
  sand_rand.o \
  sand_bitstream.o \
  sand_tcm.o \
  sand_os_bcm_interface.o \
  sand_integer_arithmetic.o \
  sand_array_memory_allocator.o \
  sand_framework.o


BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
