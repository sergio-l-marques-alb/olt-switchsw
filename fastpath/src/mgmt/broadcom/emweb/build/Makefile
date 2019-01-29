# This is a package-level makefile used to build a partially-linked package library.
#
#   - PKGID designates package identifier used for customizing common make var names
#   - PKGNAME designates the output name of this package.
#
# Note: Unlike other package-level makefiles, this one produces an archive file
#       representing the EmWeb server subsystem that is linked into the actual
#       package library file by its parent makefile.
#
PKGID   := WEB
PKGNAME := $(ROOT_$(PKGID))


# Do not change these lines.
#

# Include target package output settings.
include $(FP_ROOT)/src/l7tools/build/make.hdr.pkg

# Override default Makefile location.
MAKEFILE_NAME := Makefile

FP_BASE := $(RELOUT)

# Not using $(FP_BASE)/src/l7tools/build/make.cfg (it's included later).

#----- Start Makefile transformation section.

include $(FP_BASE)/src/mgmt/$(L7_EMWEBPACKAGE)/emweb/build/Makefile.emweb

#----- End Makefile transformation section.

# Not using include $(FP_BASE)/src/l7tools/build/make.footer here.


