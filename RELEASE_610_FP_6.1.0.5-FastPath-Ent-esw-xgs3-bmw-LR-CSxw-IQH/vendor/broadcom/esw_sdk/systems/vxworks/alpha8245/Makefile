# $Id: Makefile,v 1.1 2011/04/18 17:11:11 mruas Exp $
# $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
# OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
# BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
#
# Makefile for VxWorks PPC BSP with BCM driver library
# VxWorks (sp) configuration variables
#

LOCALDIR=systems/vxworks/bmw
LOCAL_CLEAN=local_clean

override-target=vxworks-bmw

#VX_VERSION=62

ifdef SDK
include ${SDK}/make/Make.config
include ${SDK}/make/Make.vxworks
else
toast:; @$(ECHO) 'The $$SDK environment variable is not set'; exit 1
endif

#
# This optional override builds a binary that loads the symbol table
# over the network instead of having it compiled in.
#
#override VX_TARGET = vxWorks

#
# Point to the VxWorks BSP source directory
#
SRC_DIR=${SDK}/systems/vxworks/bmw/mpc824x

#
# Define rule to build ${VX_BINARY}
#

${VX_BINARY} vxWorks.st vxWorks.st.ppc: \
		bde bcm_libraries driver_libraries \
		${BLDDIR}/.tree ${BLDDIR}/socdiag.o ${BLDDIR}/version.o\
		${BLDDIR}/platform_defines.o ${BLDDIR}/hal.o
	${MAKE} -C ${SRC_DIR} WIND_BASE="${WIND_BASE}" \
		MAKEFLAGS="${MAKEFLAGS}" ${KERNFLAGS} ${VX_TARGET}
	${CP} ${SRC_DIR}/${VX_TARGET} ${VX_BINARY}.sym
	${CP} ${SRC_DIR}/${VX_TARGET} ${VX_BINARY}
ifneq ($(OS),Windows_Cygnus)
# stripppc.exe is broken
	${STRIP} ${VX_BINARY}
	${STRIPFIX} ${SRC_DIR}/${VX_TARGET} ${VX_BINARY} 56 5
endif
	${MKBEEP}

bootrom_uncmp:
	${MAKE} -C ${SRC_DIR} WIND_BASE="${WIND_BASE}" \
	MAKEFLAGS="${MAKEFLAGS}" bootrom_uncmp
	${CP} ${SRC_DIR}/bootrom_uncmp bootrom_uncmp

vxWorks.res_rom:
	${MAKE} -C ${SRC_DIR} WIND_BASE="${WIND_BASE}" \
	MAKEFLAGS="${MAKEFLAGS}" vxWorks.res_rom
	${CP} ${SRC_DIR}/vxWorks.res_rom vxWorks.res_rom

