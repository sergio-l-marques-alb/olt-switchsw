# $Id: Makefile,v 1.1 2011/04/18 17:11:10 mruas Exp $
# $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
# OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
# BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
#

LOCALDIR = systems/sim/pcid

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

PCID = ${INSTALL_DIR}/pcid.${targetplat}${all_suffix}
PCID_LIB = ${LIBDIR}/libpcid.a

all_targets = ${PCID} ${PCID_LIB}

#
# Define targets to be built by default.
#
build_targets = ${all_targets}

#
# Libraries
#
DIAG_LIBS = \
	${COND_LIB_EDITLINE} \
	libsal_appl.a libsal_appl_plat.a libplibde.a

PCID_LIBS = ${EXTRA_TEST_LIBS} ${DRV_LIBS} ${DIAG_LIBS} ${APPL_LIBS}
PCID_LIBS_BLD = $(addprefix ${BLDROOT}/,${PCID_LIBS})


all:: ${BLDDIR}/.tree ${build_targets}

# Libraries

.PHONY: _bcm_libraries

_bcm_libraries:
ifndef NO_BCM
	${MAKE} -C ${SDK}
endif

# Broadcom Command Monitor

# PCI Daemon (chip simulator)

${PCID}: _bde _bcm_libraries ${BOBJS} ${BLDDIR}/version.o
	@rm -f ${PCID}	# Prevent core dump if currently running
	${CC} -o ${PCID} ${BOBJS} ${BLDDIR}/version.o \
		${PCID_LIBS_BLD} ${PCID_LIBS_BLD} ${LDFLAGS}

PCID_SRCS = \
	cmicsim.c pli.c util.c dma.c  physim.c\
	l2x.c l3x.c socintf.c proc_req.c arl.c packet.c
PCID_OBJS = $(addsuffix .o, $(basename ${PCID_SRCS}))
PCID_BOBJS = $(addprefix ${BLDDIR}/,${PCID_OBJS})

${PCID_LIB}: ${PCID_BOBJS}
	${RM} $@
	${AR} ${ARFLAGS} $@ ${PCID_BOBJS}


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
	rm -f ${LIBDIR}/libpcid.a
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
