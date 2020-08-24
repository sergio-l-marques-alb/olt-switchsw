#
# $Id$
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
LOCALDIR = libs/flexe_core/dbg
ifndef FLEXE
FLEXE = $(CURDIR)
endif

include ${FLEXE}/make/config.mk

#FLEXE_CORE support
VPATH += bsp/src
VPATH += chip/module/66bswitch chip/module/cpb chip/module/demux chip/module/flexe_oh chip/module/module1588 chip/module/oam chip/module/mux chip/module/sar chip/module/init chip/module/mcmac chip/module/rateadpt
VPATH += lib/src
VPATH += protocal/demux/src protocal/mux/src
VPATH += access
 
VSRCS += $(wildcard bsp/src/*.c) $(wildcard lib/src/*.c) $(wildcard chip/module/66bswitch/*.c) $(wildcard chip/module/cpb/*.c) $(wildcard chip/module/demux/*.c)
VSRCS += $(wildcard chip/module/flexe_oh/*.c) $(wildcard chip/module/module1588/*.c) $(wildcard chip/module/oam/*.c) $(wildcard chip/module/mux/*.c) $(wildcard chip/module/sar/*.c) $(wildcard protocal/demux/src/*.c) $(wildcard protocal/mux/src/*.c) $(wildcard chip/module/init/*.c) $(wildcard chip/module/mcmac/*.c) $(wildcard chip/module/rateadpt/*.c)
VSRCS += $(wildcard access/*.c)

LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o, $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

LIBNAME = libflexe_core_dbg
include ${FLEXE}/make/lib.mk

include ${FLEXE}/make/depend.mk
