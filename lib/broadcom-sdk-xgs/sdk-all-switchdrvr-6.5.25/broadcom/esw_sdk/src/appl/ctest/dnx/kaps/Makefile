#
# $Id: Makefile Exp $
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#


LOCALDIR = src/appl/ctest/dnx/kaps

include ${SDK}/make/Make.config
INCDIR += -I${SDK}/libs/kaps/include
INCDIR += -I${SDK}/libs/kaps
INCDIR += -I${SDK}/${LOCALDIR}/verifier

subdirs += verifier

lib = libctest_dnxkaps

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

