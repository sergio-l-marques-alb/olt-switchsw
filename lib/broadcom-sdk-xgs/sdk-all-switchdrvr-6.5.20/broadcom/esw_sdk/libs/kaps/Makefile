#
# $Id: Makefile,v 1.4 Broadcom SDK $
# $Copyright: (c) 2020 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#

LOCALDIR = libs/kaps

include ${SDK}/make/Make.config

lib = libkaps

INCDIR += -I${SDK}/libs/kaps/include
INCDIR += -I${SDK}/libs/kaps

#In case of a build for a shared lib add a PIC flag.
ifneq ($(filter $(SHARED_LIBRARY_SUPPORT) $(COMPILE_OPENNSA_SUPPORT),1),)
  CFLAGS += -fPIC
endif

include ${SDK}/make/Make.subdirs

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
