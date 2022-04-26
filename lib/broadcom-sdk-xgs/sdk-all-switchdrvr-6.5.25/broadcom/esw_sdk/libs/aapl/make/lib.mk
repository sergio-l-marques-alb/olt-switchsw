# $Id$
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# AAPL make rules for libraries
#

include $(AAPL)/make/config.mk

include $(AAPL)/make/rules.mk

BLIBNAME = $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX)

.SECONDARY:: $(BOBJS)

all:: $(BLDDIR)/.tree $(BLIBNAME)

clean::
	@$(ECHO) Cleaning objects for $(notdir $(BLIBNAME))
	$(Q)$(RM) $(BLDDIR)/.tree $(BOBJS) $(BLIBNAME)

include $(AAPL)/make/depend.mk
