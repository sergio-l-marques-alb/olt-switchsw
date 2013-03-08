FP_BASE=../../..

####################################################################
# This make file creates the fastpath object library and
# deletes directories that are not distributed in source format.
# of the project
#
# History:
#
# Andrey Tsigler  4/1/2003
####################################################################

# always use the sh for executing shell commands
#SHELL=/bin/bash

# include the make configuration file
include make.cfg

RMF=rm -rf
#RMF=ls -l

all: apps
#all: apps mgmt

# This file performs the following actions:
#
#  Find all *.o files in application, dtl, usmdb directories.
#  Find *.o files in system_support, except in directories platform and zlib_win.
#  Find *.o files in the os/linux/routing directory.
#
#  Strip debug symbols from the object files.
#  Archive the objects in fastpath.lib
#
#  Delete directories which contains source that the customer is 
#  not supposed to see.
#
#  Replace ...src/system_support/base/Makefile with Makefile.obj.
#  The Makefile.obj builds only cnfgr/platform directory and
#  l7util/zlib_win directory.
#
#  Edit make.inc to remove calls to make files in deleted directories.
#

apps:
	rm -f objects
	-find $(FP_BASE)/src/application -name "*.o" -print > objects
	-find $(FP_BASE)/src/mgmt/mgmt_support -name "*.o" -print >> objects
	-find $(FP_BASE)/src/dtl -name "*.o" -print >> objects
	-find $(FP_BASE)/src/usmdb -name "*.o" -print >> objects
	-find $(FP_BASE)/src/binds -name "*.o" -print >> objects
	-find $(FP_BASE)/os/linux/routing -name "*.o" -print >> objects
	-find $(FP_BASE)/src/system_support -path '*/cnfgr/platform' -prune -or \
					   -path '*/zlib_win' -prune -or \
					   -name '*.o' -print >> objects

	cat objects | xargs $(STRIP) -g 
	cat objects | xargs $(AR) -rP $(FASTPATH_LIB)

	$(RMF) $(FP_BASE)/src/application
	$(RMF) $(FP_BASE)/src/mgmt/mgmt_support
	$(RMF) $(FP_BASE)/src/dtl
	$(RMF) $(FP_BASE)/src/usmdb
	$(RMF) $(FP_BASE)/src/binds
	$(RMF) $(FP_BASE)/src/system_support/security
	$(RMF) $(FP_BASE)/src/system_support/base/network
	$(RMF) $(FP_BASE)/src/system_support/base/services
	$(RMF) $(FP_BASE)/src/system_support/base/system
	$(RMF) $(FP_BASE)/src/system_support/base/infrastructure/cable_status
	$(RMF) $(FP_BASE)/src/system_support/base/infrastructure/nim
	$(RMF) $(FP_BASE)/src/system_support/base/infrastructure/sim
	$(RMF) $(FP_BASE)/os/linux/routing
	find $(FP_BASE)/src/system_support/base/infrastructure/cnfgr/* -path '*/cnfgr/platform' -prune -or \
					   -type 'd' -maxdepth 0 -print | xargs $(RMF)
	find $(FP_BASE)/src/system_support/base/l7util/*  -path '*/zlib_win' -prune -or \
					   	-type 'd' -maxdepth 0 -print | xargs $(RMF)
	find $(FP_BASE)/src/system_support/routing/*  -path '*/l7inc_paths' -prune -or \
                                                -type 'f' -maxdepth 0 -print -or \
					   	-type 'd' -maxdepth 0 -print | xargs $(RMF)

	rm -f $(FP_BASE)/src/system_support/base/Makefile
	cp $(FP_BASE)/src/system_support/base/Makefile.obj $(FP_BASE)/src/system_support/base/Makefile


	grep -iv 'src/application/' $(FP_BASE)/src/l7tools/build/make.inc | \
		grep -iv 'src/mgmt/mgmt_support/' | \
		grep -iv 'src/dtl/' | \
		grep -iv 'src/binds/' | \
		grep -iv 'src/usmdb/' | \
		grep -iv 'src/system_support/security' | \
		grep -iv 'src/system_support/routing' | \
		grep -iv 'os/linux/routing' > make.incobj

	cp make.inc make.inc.old
	rm -f make.inc
	cp make.incobj make.inc

# Add management code to the archive. This may help to
# reduce disk space and reduce link times for internal development.
#
mgmt:
	-find $(FP_BASE)/src/mgmt -name "*.o" -print > objects2
	cat objects2 | xargs $(STRIP) -g
	cat objects2 | xargs $(AR) -rP $(FASTPATH_LIB)
	$(RMF) $(FP_BASE)/src/mgmt

	grep -iv 'src/mgmt/' $(FP_BASE)/src/l7tools/build/make.inc > make.incobj2
	
	cp make.inc make.inc.old2
	rm -f make.inc
	cp make.incobj2 make.inc

