# Linux makefile definitions included by bsp package.
# 
# Note: Various $(src_xxx) abbreviations are defined by caller.
#
# Note: Only use += here (to append to existing definitions).
#

# Master list of source paths to be built into this package library.
#  - All entries must be specified relative to top-of-view $(FP_ROOT).
#  - Defines all source locations to be referenced by the package-level makefile.
#
MASTERSRC += \
	$(src_cpu)/common/linux/ipl \
	$(src_cpu)/$(L7_CPU)/linux/src \
	$(src_cpu)/$(L7_CPU)/linux/customer/$(L7_CUSTOMER)/$(L7_PROJECT)/src


# List here individual source files to be selectively EXCLUDED from the build.
#
# Note:  These files are built during the link process.
#
MASTERSRC_FILE_EXCLUDE += \
	$(src_cpu)/common/linux/ipl/bootos.c \
	$(src_cpu)/common/linux/ipl/dummy_symTbl.c


# List here individual source include directories owned by this package (without the -I).
#
MASTERINC +=


# Additional source header file search paths (without the -I).
#
SRCINCPATHS +=

# Additional CFLAGS.
#
CFLAGSEXTRA +=

