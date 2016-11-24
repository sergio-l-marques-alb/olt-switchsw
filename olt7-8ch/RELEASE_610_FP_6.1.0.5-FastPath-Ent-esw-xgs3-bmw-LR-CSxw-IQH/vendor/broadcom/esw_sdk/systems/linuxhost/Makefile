# $Id: Makefile,v 1.1 2011/04/18 17:11:10 mruas Exp $
# $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
# OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
# BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
#

LOCALDIR=systems/linuxhost

override-target=unix-linuxhost

ifdef SDK
include ${SDK}/make/Make.config
else
toast:; @$(ECHO) 'The $$SDK environment variable is not set'; exit 1
endif

lib = libsystem_linuxhost

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

all bldlib:: _bde _bcm_libraries

# Libraries
.PHONY: _bcm_libraries _bde

_bcm_libraries:
ifndef NO_BCM
	${MAKE} -C ${SDK}
endif

_bde:
	$(MAKE) -C ${SDK}/systems/bde/pli

# Clean

clean:: ${BLDDIR}/.tree
	rm -f ${all_targets} 
	rm -f ${BOBJS}
	rm -f ${LIBDIR}/libpcid.a
	rm -f ${BLDDIR}/version.c ${BLDDIR}/version.o
	rm -f ${BLDDIR}/libsystem_linuxhost.a
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

# Output the CFLAGS used by the SDK
#
.PHONY: get_sdk_flags

get_sdk_flags::
	@echo "CFLAGS_SDK := $(filter -D%,$(CFLAGS))" >$(L7_OUT_SDK_FLAGS)

