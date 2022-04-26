#
# $Id: Makefile,v 1.4 2016/10/10
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#


LOCALDIR = src/bcm/dnx/flow/app_defs
include ${SDK}/make/Make.config

subdirs += mpls 
subdirs += ip_tunnel

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxflowappdefs

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
