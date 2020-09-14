# $Id$
# $Copyright: (c) 2020 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# PCIEPHY make rules for libraries
#

include $(PCIEPHY)/make/config.mk

include $(PCIEPHY)/make/rules.mk

BLIBNAME = $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX)

.SECONDARY:: $(BOBJS)

all:: $(BLDDIR)/.tree $(BLIBNAME)

clean::
	@$(ECHO) Cleaning objects for $(notdir $(BLIBNAME))
	$(Q)$(RM) $(BLDDIR)/.tree $(BOBJS) $(BLIBNAME)

include $(PCIEPHY)/make/depend.mk
