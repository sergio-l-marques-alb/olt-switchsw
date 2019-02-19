#
# Release: R6_0_1
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
# EmWeb Makefile Configuration
#

##############################################################################
#
# Makefile Configuration
#
# This file is modified by hand or the configure script to create config.mak
# in the appropriate config.* directory
#
# DO NOT specify any targets in this file - this is included before the
#                                           default target in many Makefiles
##############################################################################

# Get LVL7 Common Rules
include $(FP_BASE)/src/l7tools/build/make.cfg

## If configuring by hand, change the following to 'MAKE=make' _only_ if
## the 'make' you are using does not set that for you - if it does (most do),
## delete the following line:


#
# EmWeb/Compiler and EmWeb/Server configurations.

## If configuring a target by hand, copy the values for these variables
## from the version of this file that was created for your development
## host by the configure script.
#prefix		= /usr/local
#exec_prefix	= ${prefix}
#bindir		= ${exec_prefix}/bin
#datadir		= ${prefix}/share
#ewdatadir	= $(datadir)/emweb
#EXEC_SUFFIX	= 

##
## The development host -
##   this must match the ../bin.$(HOST) where
##   your EmWeb/Compiler (ewc) is found.
HOST = $(EMWEB_HOST)

## Directory names common to all the makefiles
## for directories where derived objects are created
## The ROOT should be set in each makefile to the 'emweb' directory
## (this is usually set as a relative path, but may be absolute)
#OD  := $(ROOT)/obj.$(CONFIG)
OD  := $(FP_BASE)/$(OUT_EMWEB)
CD  := $(ROOT)/config.$(CONFIG)
BD  := $(ROOT)/bin.$(HOST)

#
# Configuration-specific definitions
#
CC_linux       = $(CROSS_COMPILE)gcc
CFLAGS_linux   = -g -O2 -D__NO_STRING_INLINES -I$(ROOT)/linux
#
# LD - loader for building executables; in some toolsets this should be
#      the same as CC_linux, in others it is a separate program
#      Check the makefile for the target or lib/rules.mak to see which
#      is used
#
LD_linux       = $(CROSS_COMPILE)ld
#
# LDINITFLAGS - flags to be passed to LD_linux before the objects
#               For Chorus, these should be: -nostartfiles -nostdlib -e _start
#
LDINITFLAGS_linux  = -static
LDFLAGS_linux  = 
CLIBS_linux    = 
AR_linux       = $(CROSS_COMPILE)ar
ARFLAGS_linux  = rc
RANLIB_linux   = $(CROSS_COMPILE)ranlib
SH             = /bin/sh
TOUCH          = /bin/touch

# This may be overridden if the platform has makedepend (see rules.mak)
MAKEDEPEND      = :


CFLAGS_linux	+= $(DEBUG_CFLAGS)
CFLAGS_linux	+= -DHAVE_CONFIG_H -D_L7_OS_LINUX_ -O0

# EWFLAGS are flags to the EmWeb compiler
EWFLAGS_linux += --raw


## Local Variables: ***
## mode: makefile ***
## tab-width: 8 ***
## comment-start: "## "	 ***
## End: ***

