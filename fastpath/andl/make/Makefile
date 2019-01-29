# This is a package-level makefile used to build a partially-linked package library.
#
#   - PKGID designates package identifier used for customizing common make var names.
#   - PKGNAME designates the output name of this package.
#
PKGID   := ANDL
PKGNAME := $(ROOT_$(PKGID))

# Must export this PKGNAME so it can be used by the vendor make.sdk file
# and by the filter_sdk_output.pl script.
#
export PKGNAME


# Do not change or move anything below this line.

# Include target package output settings.
include $(FP_ROOT)/src/l7tools/build/make.hdr.pkg

# Define all source locations used to build this package library.
include $(FP_ROOT)/andl/make/make.pkg

# Special magic to allow building from output location.
include $(FP_ROOT)/src/l7tools/build/make.magic.pkg

