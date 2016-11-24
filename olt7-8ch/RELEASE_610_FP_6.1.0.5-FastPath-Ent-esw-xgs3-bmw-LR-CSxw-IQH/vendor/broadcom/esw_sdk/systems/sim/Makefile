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
#

LOCALDIR=systems/sim

uname := $(shell uname -s)
ifeq ($(uname),Linux)
 ifeq ($(shell uname -m),x86_64)
  override-target=unix-linux-64
 else
  override-target=unix-linux
 endif
endif
ifeq ($(uname),SunOS)
 override-target=unix-solaris
endif
ifeq ($(uname),NetBSD)
 override-target=unix-netbsd
endif

ifdef NO_SAL_APPL
export NO_SAL_APPL
ifndef BLDCONFIG
BLDCONFIG=nsa
endif
export BLDCONFIG
endif

MFLAGS = 

ifdef SDK
include ${SDK}/make/Make.config
else
$(error 'The $$SDK environment variable is not set')
endif

INSTALL_DIR=${SDK}/systems/sim

#
# To use Purify, add a line here similar to the following:
#
#CC=/net/igor/export/home/pure/purify-4.2-solaris2/purify gcc

#
# Define all targets to build
# pcid is built first so you can start it before bcm links :-)
#

PCID = ${INSTALL_DIR}/pcid.${targetplat}${all_suffix}
L2SWITCH = ${INSTALL_DIR}/l2switch.${targetplat}${all_suffix}

# This defines where main is found
ifdef NO_SAL_APPL
MAIN_LIB=${BLDDIR}/socdiag-nsa.o
NSA=.nsa
BCM_SYMLINK=bcm.sim.nsa
all_targets = bcm.sim
else
MAIN_LIB=${BLDDIR}/socdiag.o
BCM_SYMLINK=bcm.sim
all_targets = bcm.sim pcid.sim
endif

BCM = ${INSTALL_DIR}/bcm.${targetplat}${all_suffix}${NSA}
# MAIN_LIB=${LIBDIR}/libdisctest.a


#
# Define targets to be built by default.
#
build_targets = ${all_targets}

#
# Libraries
#

BCM_LIBS = ${APPL_LIBS} ${DIAG_LIBS} ${DRV_LIBS}
BCM_LIBS_BLD = $(addprefix ${LIBDIR}/,${BCM_LIBS})

ifdef BUILD_MACSEC
BCM_LIBS_BLD += ${BCM_LIB_MACSEC}
endif

PCID_LIBS = ${DRV_LIBS}
PCID_LIBS_BLD = $(addprefix ${LIBDIR}/,${PCID_LIBS})

L2SWITCH_LIBS = ${DRV_LIBS}
L2SWITCH_LIBS_BLD = $(addprefix ${LIBDIR}/,${L2SWITCH_LIBS})

all:: ${BLDDIR}/.tree ${build_targets}

#
# Simplified targets to build a target for current platform
#
.PHONY:	bcm pcid l2switch

bcm bcm.${targetplat}${all_suffix}: ${BCM}

pcid pcid.${targetplat}${all_suffix}: ${PCID}

l2switch l2switch.${targetplat}${all_suffix}: ${L2SWITCH}




# Libraries

.PHONY: _bcm_libraries

_bcm_libraries:
ifndef NO_BCM
	$(MAKE) -C ${SDK}
endif

ifdef BUILD_MACSEC
	$Q$(ECHO) "Building MACSEC libraries ...."
	$(MAKE) -C ${MACSEC_HOME} MACSEC_CC="${CC}" MACSEC_LD="${LD}" MACSEC_CFLAGS="${MACSEC_BUILD_FLAG}" MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}"
endif

# Broadcom Command Monitor

# Build platform_defines.c to compile in config.bcm
#
ifndef NO_SAL_APPL
PLATFORM_DEFINES_OBJ=${BLDDIR}/platform_defines.o

${BLDDIR}/platform_defines.c: $(BLDDIR)/.tree ${SDK}/rc/config.bcm
	+$(PERL) ${SDK}/tools/bcm2c_linux.pl ${SDK}/rc/config.bcm >$@

${BLDDIR}/platform_defines.o: ${BLDDIR}/platform_defines.c
	+${CC} ${CFLAGS} -c $< -o $@ ; \
	touch ${SDK}/rc/config.bcm
else
PLATFORM_DEFINES_OBJ=
endif

bcm.sim: ${BCM}
	${RM} -f $(BCM_SYMLINK)
	${LN} -s ${BCM} $(BCM_SYMLINK)


${BCM}: ${MAIN_LIB} _bde _bcm_libraries ${BLDDIR}/version.o \
        ${PLATFORM_DEFINES_OBJ} ${APPL_LIBS_BLD}
	@rm -f $@	# Prevent core dump if currently running
	${CC} -o $@ ${MAIN_LIB} ${BLDDIR}/version.o \
        ${PLATFORM_DEFINES_OBJ} \
        ${BCM_LIBS_BLD} ${BCM_LIBS_BLD} ${LDFLAGS}

# PCI Daemon (chip simulator)

pcid.sim: ${PCID}
	${RM} -f pcid.sim
	${LN} -s ${PCID} pcid.sim

${PCID}: _bcm_libraries _bde
	$(MAKE) -C ${SDK}/systems/sim/pcid \
		INSTALL_DIR=$(INSTALL_DIR) ${PCID}

# Example L2 Switch Application

${L2SWITCH}: _bcm_libraries _bde
	$(MAKE) -C ${SDK}/systems/sim/l2switch \
		INSTALL_DIR=$(INSTALL_DIR) ${L2SWITCH}





#
# Build version generator.  If NO_VERSION=1 is specified in Make.local
# or on command line, then use the existing version.c.  If that doesn't
# exist, copy the default one from config.
#
# Otherwise, force the generation of a new version.c using the perl script.
#
ifdef NO_VERSION
${BLDDIR}/version.c: ${BLDDIR}/.tree
	$(CP) ${SDK}/make/version.c $@
else
.PHONY: _force
${BLDDIR}/version.c: ${BLDDIR}/.tree _force
	$(MKVERS) > $@
endif
${BLDDIR}/version.o: ${BLDDIR}/version.c
	${CC} ${CFLAGS} -c $< -o $@

# Make.depend is before clean:: so that Make.depend's clean:: runs first.

include ${SDK}/make/Make.depend

# Clean

clean:: ${BLDDIR}/.tree
	rm -f ${all_targets} 
	rm -f ${BOBJS}
	# Cleaning in PCID cleans BCM libs, BDE, etc.
	# This should be last because it removes ${BLDROOT}.
	$(MAKE) -C ${SDK}/systems/sim/pcid \
		INSTALL_DIR=$(INSTALL_DIR) clean
	$(MAKE) -C ${SDK}/systems/sim/l2switch \
		INSTALL_DIR=$(INSTALL_DIR) clean

distclean:: clean

.PHONY: _bde
_bde:
	$(MAKE) -C ${SDK}/systems/bde/pli

# two libraries this binary needs
# point to them to allow parallel make processes
$(SDK)/build/%/libdiscover.a:
	$(MAKE) -C $(SDK)/src/appl/discover

$(SDK)/build/%/libstktask.a:
	$(MAKE) -C $(SDK)/src/appl/stktask



