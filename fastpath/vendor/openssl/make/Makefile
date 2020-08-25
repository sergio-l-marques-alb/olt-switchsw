# This is a package-level makefile used to build a collection of archive files.
#
#   - PKGID designates package identifier used for customizing common make var names.
#   - PKGNAME designates the output name of this package.
#
PKGID   := OPENSSL
PKGNAME := $(ROOT_$(PKGID))


# Identify makefile containing specific build rules to be used.
#
# Note: This makefile must be included within make.magic.pkg to define
#       the rules within the right scope of makefile processing.
#
BLDRULESFILE = $(FP_ROOT)/vendor/openssl/make/makerules.pkg


# Do not change or move anything below this line.

# Include target package output settings.
include $(FP_ROOT)/src/l7tools/build/make.hdr.pkg

# Define all source locations used to build this package library.
include $(FP_ROOT)/vendor/openssl/make/make.pkg

# Special magic to allow building from output location.
include $(FP_ROOT)/src/l7tools/build/make.magic.pkg

