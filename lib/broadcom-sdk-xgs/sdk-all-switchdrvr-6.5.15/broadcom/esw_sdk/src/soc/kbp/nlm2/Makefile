# $Id: Makefile,v 1.4 2007/10/08 22:27:33 iakramov Exp $
# $Copyright: (c) 2005 Broadcom Corp.
# All Rights Reserved.$
#
#
PLATFORM=m32

include ${SDK}/make/Make.config

LIBDIRS = nlmdevmgr nlmrangemgr nlmgenerictblmgr nlmfibtblmgr nlmxpt 
APPDIRS = nlmgenerictblmgr_refapp nlmgtmftm_refapp diagnostic 
APPDIRS += nlmdevmgr_refapp nlmrangemgr_refapp nlmrangemgr_refapp2 \
        nlmrangemgr_refapp3 nlmfibtblmgr_refapp

$(foreach lib, $(LIBDIRS),$(eval subdirs += $(lib)))
$(foreach lib, $(APPDIRS),$(eval subdirs += $(lib)))

include ${SDK}/make/Make.subdirs

include ${SDK}/make/Make.depend

