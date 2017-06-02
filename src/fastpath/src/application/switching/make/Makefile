# This is a package-level makefile used to build a partially-linked package library.
#
#   - PKGID designates package identifier used for customizing common make var names.
#   - PKGNAME designates the output name of this package.
#
PKGID   := SWITCHING
PKGNAME := $(ROOT_$(PKGID))


# Do not change or move anything below this line.

# Include target package output settings.
include $(FP_ROOT)/src/l7tools/build/make.hdr.pkg

#PTIN Added: MGMD compile (DO NOT MOVE THIS LINE FROM HERE! It needs to be below make.hdr.pkg!!!!)
export LIBOBJSEXTRA += $(FP_ROOT)/src/application/switching/mgmd/rfs/usr/local/ptin/lib/libmgmd.a

# Define all source locations used to build this package library.
include $(FP_ROOT)/src/application/switching/make/make.pkg

# Special magic to allow building from output location.
include $(FP_ROOT)/src/l7tools/build/make.magic.pkg

