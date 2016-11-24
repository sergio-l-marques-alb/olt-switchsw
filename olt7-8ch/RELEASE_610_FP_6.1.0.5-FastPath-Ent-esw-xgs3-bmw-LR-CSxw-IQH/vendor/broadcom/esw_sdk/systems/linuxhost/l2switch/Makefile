# $Id: Makefile,v 1.1 2011/04/18 17:11:10 mruas Exp $
# $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
# OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
# BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
#

LOCALDIR = systems/sim/l2switch

ifdef SDK
include ${SDK}/make/Make.config
else
toast:; @$(ECHO) 'The $$SDK environment variable is not set'; exit 1
endif

ifeq ($(INSTALL_DIR),)
INSTALL_DIR=.
endif

#
# To use Purify, add a line here similar to the following:
#
#CC=/net/igor/export/home/pure/purify-4.2-solaris2/purify gcc

#
# Define all targets to build
#

L2SWITCH = ${INSTALL_DIR}/l2switch.${targetplat}${all_suffix}
L2SWITCH_LIB = ${LIBDIR}/libl2switch.a

all_targets = ${L2SWITCH} ${L2SWITCH_LIB}

#
# Define targets to be built by default.
#
build_targets = ${all_targets}

#
# Libraries
#

#DIAG_LIBS = \
#	${COND_LIB_EDITLINE} \
#	libdiag.a libtest.a \
#	${COND_LIB_TCL} \
#	libsal_appl.a libsal_appl_plat.a libplibde.a

L2SWITCH_LIBS =  ${APPL_LIBS} ${DRV_LIBS} ${DIAG_LIBS}
L2SWITCH_LIBS_BLD = $(addprefix ${BLDROOT}/,${L2SWITCH_LIBS})


all:: ${BLDDIR}/.tree ${build_targets}

# Libraries

.PHONY: _bcm_libraries

_bcm_libraries:
ifndef NO_BCM
	${MAKE} -C ${SDK}
endif

# Example L2 Switch Application

${L2SWITCH}: _bde _bcm_libraries ${BOBJS} ${BLDDIR}/version.o
	@rm -f ${L2SWITCH}	# Prevent core dump if currently running
	${CC} -o ${L2SWITCH} ${BOBJS} ${BLDDIR}/version.o \
		${L2SWITCH_LIBS_BLD} \
		${L2SWITCH_LIBS_BLD} \
		${L2SWITCH_LIBS_BLD} \
		${LDFLAGS}

L2SWITCH_SRCS = \
	l2switch.c
L2SWITCH_OBJS = $(addsuffix .o, $(basename ${L2SWITCH_SRCS}))
L2SWITCH_BOBJS = $(addprefix ${BLDDIR}/,${L2SWITCH_OBJS})

${L2SWITCH_LIB}: ${L2SWITCH_BOBJS}
	${RM} $@
	${AR} ${ARFLAGS} $@ ${L2SWITCH_BOBJS}


# Build Version

${BLDDIR}/version.c: ${BLDDIR}/.tree
	$(MKVERS) > $@
${BLDDIR}/version.o: ${BLDDIR}/version.c
	${CC} ${CFLAGS} -c $< -o $@

# Make.depend is before clean:: so that Make.depend's clean:: runs first.

include ${SDK}/make/Make.depend

# Clean

clean:: ${BLDDIR}/.tree 
	rm -f ${all_targets}
	rm -f ${BOBJS}
	rm -f ${LIBDIR}/libl2switch.a
	rm -f ${BLDDIR}/version.c ${BLDDIR}/version.o
ifndef NO_BCM
	${MAKE} -C ${SDK} clean
endif
	$(MAKE) -C ${SDK}/systems/bde/pli clean
	@# Rather than just removing the build directory, the following
	@# will detect any file missed by 'clean' so it can be fixed.
	@-find ${BLDROOT} -depth \
		\( -name .tree -exec rm {} \; \) -o \
		\( -type d -exec rmdir {} \; \) -o \
		-exec echo WARNING: make clean did not remove {} \; \
		-exec rm -f {} \;

distclean:: clean

.PHONY: _bde
_bde:
ifndef NO_BDE
	$(MAKE) -C ${SDK}/systems/bde/pli
endif
