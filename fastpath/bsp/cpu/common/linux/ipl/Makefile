# This is a link-level makefile used to combine all packages into a loadable image.
#
#   - PKGID designates package identifier used for customizing common make var names.
#   - PKGNAME designates the output name of this package.
#
PKGID   := IPL
PKGNAME := $(ROOT_$(PKGID))


# Identify makefile containing specific build rules to be used
# (or set to empty value if none required).
#
# Note: This makefile must be included within make.magic.link to define
#       the rules within the right scope of makefile processing.
#
BLDRULESFILE = $(FP_ROOT)/bsp/cpu/common/linux/ipl/makerules.link


# Do not change or move anything below this line.

# Include linker output settings.
include $(FP_ROOT)/src/l7tools/build/make.hdr.link

# Define all source locations used to build this package library.
include $(FP_ROOT)/bsp/cpu/common/linux/ipl/make.link

# Special magic to allow building from output location
include $(FP_ROOT)/src/l7tools/build/make.magic.link

