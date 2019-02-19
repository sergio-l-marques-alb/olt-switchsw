####
#### NOTICE: THIS FILE IS NO LONGER USED BY THE MAKE SYSTEM AND IS NOW OBSOLETE.
####

FP_BASE=../../../..

#
# Product:      EmWeb
# Release: R6_2_0
#
# CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
# THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
# AND OTHER US AND INTERNATIONAL PATENTS PENDING.
# 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
# 
# Notice to Users of this Software Product:
# 
# This software product of Virata Corporation ("Virata"), 5 Clock Tower
# Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
# source and object code format embodies valuable intellectual property 
# including trade secrets, copyrights and patents which are the exclusive
# proprietary property of Virata. Access, use, reproduction, modification
# disclsoure and distribution are expressly prohibited unless authorized
# in writing by Virata.  Under no circumstances may you copy this
# software or distribute it to anyone else by any means whatsoever except in
# strict accordance with a license agreement between Virata and your
# company.  This software is also protected under patent, trademark and 
# copyright laws of the United States and foreign countries, as well as 
# under the license agreement, against any unauthorized copying, modification,
# or distribution.
# Please see your supervisor or manager regarding any questions that you may
# have about your right to use this software product.  Thank you for your
# cooperation.
# 
# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
# All Rights Reserved
# 
# Virata Corporation
# 5 Clock Tower Place
# Suite 400
# Maynard, MA  01754
# 
# Voice: +1 (978) 461-0888
# Fax:   +1 (978) 461-2080
# 
# http://www.emweb.com/
#   support@agranat.com
#     sales@agranat.com
#
# EmWeb/Server Makefile
#


srcdir  = .
ROOT = $(srcdir)/..
FP_LSRC  = $(ROOT)/websrc
LIB  = $(ROOT)/lib


CONFIG = linux
HOST = pclinux

include	$(ROOT)/config.$(CONFIG)/config.mak

include $(FP_BASE)/src/l7tools/build/make.cfg

CONTENT = $(FP_BASE)/src/mgmt/emweb/content

PERL=perl

IFLAGS  += -D_L7_OS_LINUX_
IFLAGS  += -I$(FP_BASE)/src/mgmt/emweb/websrc/include
IFLAGS  += -I$(ROOT)/websrc/server
IFLAGS  += -I./../../cli/base/include
IFLAGS  += -I./../../cli/switching/include
IFLAGS  += -I./../../cli/routing/include
IFLAGS  += -I./../../../l7public/api
IFLAGS  += -I./../../../l7public/common
IFLAGS  += -I./../../../l7public/porting
IFLAGS  += -I./../web/base
IFLAGS  += -I./../web/routing
IFLAGS  += -I./../web/routing/dot1q
IFLAGS  += -I./../web/routingv6
IFLAGS  += -I./../web/routingv6/include
IFLAGS  += -I./../web/bgp
IFLAGS  += -I./../web/qos/diffserv
IFLAGS  += -I./../web/ip_mcast
IFLAGS  += -I./../web/wireless
IFLAGS  += -I...../headers
IFLAGS  += -I...../headers
IFLAGS  += -I$(FP_BASE)/bsp/$(L7_CUSTOMER)/$(L7_PLATFORM)/$(L7_TARGETOS)/include
IFLAGS  += -I$(FP_BASE)/bsp/$(L7_CUSTOMER)/$(L7_PLATFORM)/common/include
IFLAGS  += -I$(FP_BASE)/src/mgmt/cli_web_mgr/include

# Define CONFIG to be something bogus so that if CONFIG is not set on the
# 'make' command line, the error message that results will give the user
# an idea what the problem is.

#Disable Optimization
ifdef CFLAGS
CFLAGS += -O0
else
CFLAGS_$(CONFIG) += -O0
endif



# need to add _content when ewc is working
bldlib: _libs hostews default clean_contents

installexe:

HOSTEWS  = $(OD)/ews$(EXEC_SUFFIX)
POSIX_OBJECTS   = $(OD)/ewmain.o $(OD)/ewsys.o

# Default target is just the posix port library
default: _objdir $(POSIX_OBJECTS)


# To build a host resident server, do 'make CONFIG=xxx hostews'
# This target can also be used for target operating systems that use
#   separate standalone executables, such as Chorus, Lynx, and QNX
#
hostews: _objdir $(HOSTEWS)

_vxworks: _objdir $(OD)/tEmWeb.o

#CONTENT	= $(ROOT)/content

# Uncomment if used with example CLI provided with EmWeb/Telnet package
#CONTENT += $(FP_LSRC)/telnet/content

include $(CD)/options.mak

################################################################
## CDMAKE
##   This makefile invokes make in a number of other parts of
##   the EmWeb distribution to build components
##   By default, this is done like:
##
##     $(MAKE) -C $(FP_LSRC)/server <other-make-args>
##
##   which changes to the $(FP_LSRC)/server directory before
##   running 'make'.  If this does not work with your 'make',
##   you may need to change each of these to:
##
##     cd $(FP_LSRC)/server; $(MAKE) <other-make-args>
##
################################################################

CONTENT_DATA = $(OD)/ew_data.o
# We use '+=' here so that options.mak can set options in EWFLAGS_$(CONFIG).
EWFLAGS_$(CONFIG) += --c --raw

# To build the server so that it uses a binary file data archive
# as opposed to a C-compiled .o file, invoke $(MAKE) with
# [Note: These directions are buggy!]
#    EWFLAGS=--raw CONTENT_DATA= XCFLAGS=-DEMWEB_USE_FILE_ARCHIVE
# on the command line.
# Or, comment out the assignments to CONTENT_DATA and EWFLAGS_* above and
# uncomment the following:
#EWFLAGS_$(CONFIG)+=--raw
#CONTENT_DATA=
#XCFLAGS+=-DEMWEB_USE_FILE_ARCHIVE

# Uncomment if used with example CLI provided with EmWeb/Telnet package
#EWFLAGS_$(CONFIG) += -as --map $(FP_LSRC)/telnet/content/snmp.map

CONTENT_CODE = $(OD)/ew_code.o
CONTENT_OBJECTS = $(CONTENT_CODE) $(CONTENT_DATA)

HOSTEWS_OBJECTS = $(OD)/ewtask.o

LIBS += $(OD)/ews.a
LIBS += $(OD)/compress.a
LIBS += $(OD)/ew_md5.a
LIBS += $(OD)/ew_buffer.a
# ews.a is deliberately repeated for one-pass linkers...
LIBS += $(OD)/ews.a

SOURCES = ewtask.c ewmain.c ewsys.c
INCLUDES = ewnet.h posix_config.h ewa_routines.h

include $(SOURCES:.c=.d)

_libs: $(LIBS)

$(HOSTEWS): _objdir $(OD)/ew_proto.h $(LIBS) $(XLIBS) \
			_start_posix   $(POSIX_OBJECTS) \
			_content \
			_start_hostews $(HOSTEWS_OBJECTS) \
			$(XOBJECTS) \
			Makefile .FORCE
#	@echo "----- Linking host server: $(HOSTEWS)"
#	$(CC) -o $@ $(XCFLAGS) \
#			$(XOBJECTS) \
#			$(HOSTEWS_OBJECTS) $(POSIX_OBJECTS) $(CONTENT_OBJECTS) \
#			$(LDFLAGS) $(LIBS) $(XLIBS) $(CLIBS) $(XCLIBS)

_start_posix:
	@echo "----- Building Posix port"

_start_content: $(OD)/ew_proto.h
	@echo "----- Building Content"

_start_hostews:
	@echo "----- Building host server"

#
# This rule actually creates all of:
#   $(OD)/ew_proto.h
#   $(OD)/ew_code.c
#   $(OD)/ew_data.c and/or $(OD)/ew_data.dat (depending on EWFLAGS)
# but if you list more than one target, some versions of make will
# run the ewc compiler more than once - which can cause date mismatch
# errors when the archive is installed.
_content: _start_content $(OD)/ew_code.o $(CONTENT_DATA)


$(OD)/ew_proto.h:
	@echo "----- Compiling Web Content ($(CONTENT))"
	$(EWC) --no-strict-const -o $(OD)/ew_ $(EWFLAGS) $(CONTENT)

$(OD)/ew_code.o: $(OD)/ew_code.c $(OD)/ew_proto.h
	$(CC) -o $@ $(CFLAGS) $(XCFLAGS) $(IFLAGS) $(XIFLAGS) -c $(OD)/ew_code.c

$(OD)/ew_data.o: $(OD)/ew_data.c $(OD)/ew_proto.h
	$(CC) -o $@ $(CFLAGS) $(XCFLAGS) $(IFLAGS) $(XIFLAGS) -c $(OD)/ew_data.c

# if -C causes a problem, see note above on CDMAKE
$(OD)/ews.a: .FORCE
	@echo "----- Build EmWeb/Server library"
	$(MAKE) -C $(FP_LSRC)/server CONFIG=$(CONFIG)

# if -C causes a problem, see note above on CDMAKE
$(OD)/compress.a: .FORCE
	@echo "----- Build EmWeb uncompression library"
	$(MAKE) -C $(FP_LSRC)/compress CONFIG=$(CONFIG)

#
# This is the .o file to be added to the VxWorks build (via MACH_EXTRA)
# it may also be loaded dynamically - it contains all of EmWeb/Server,
# the port, and the content.
#
$(OD)/tEmWeb.o: $(POSIX_OBJECTS) $(OD)/ew_proto.h $(CONTENT_OBJECTS) $(LIBS)
	$(LD) -o $@ -r $(POSIX_OBJECTS) $(CONTENT_OBJECTS) $(LIBS)

#
# If you already have a compatible MD5 library in your system
# you may want to comment this out.
#
# if -C causes a problem, see note above on CDMAKE
$(OD)/ew_md5.a: .FORCE
	@echo "----- Build MD5 library"
	$(MAKE) -C $(LIB)/md5 CONFIG=$(CONFIG)

# Use this only to use our buffer routines
# if -C causes a problem, see note above on CDMAKE
$(OD)/ew_buffer.a: .FORCE
	@echo "----- Build EmWeb buffer library"
	$(MAKE) -C $(LIB)/buffer CONFIG=$(CONFIG)

# Package libraries

include $(LIB)/rules.mak

#include depend.mak

#include $(CONTENT)/contents

#$(CONTENT)/contents :
#	$(PERL) $(FP_BASE)/src/l7tools/build/helpdependemweb.pl $(FP_BASE)/src/mgmt/emweb/content 

clean_contents :
#	$(RM) $(CONTENT)/contents

clean: clean-posix 

clean-posix:
	-$(RM) -f $(HOSTEWS_OBJECTS) $(POSIX_OBJECTS) $(CONTENT_OBJECTS)
	- $(RM) -f ./Makefile.mak.log
	-$(RM) -f $(OD)/ew*.*
	-$(RM) -f $(OD)/*.a
	-$(RM) -f $(OD)/*.o
	-$(RM) -f ../websrc/telnet/*.d
	-$(RM) -f ../websrc/server/*.d
	-$(RM) -f *.d
