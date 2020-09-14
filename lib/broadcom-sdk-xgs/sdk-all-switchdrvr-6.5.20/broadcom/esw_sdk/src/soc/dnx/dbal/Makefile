#
# $Id: Makefile,v 1.4 Broadcom SDK $
# $Copyright: (c) 2020 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#


LOCALDIR = src/soc/dnx/dbal

include ${SDK}/make/Make.config

subdirs := auto_generated \
           dbal_init

INCDIR += -I${SDK}/include/soc/dnx

include ${SDK}/make/Make.subdirs

lib = libsoc_dnxdbal

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

