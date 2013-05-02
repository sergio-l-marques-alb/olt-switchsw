# This is a package-level makefile used to invoke the Broadcom SDK make system.
#
#   - PKGID designates package identifier used for customizing common make var names.
#   - PKGNAME designates the output name of this package.
#
PKGID   := ANDL
PKGNAME := $(ROOT_$(PKGID))


# Local source path abbreviations (for convenience).
#
src_esw  := andl/hapi/esw


# Include target package output settings.
#
include $(FP_ROOT)/src/l7tools/build/make.cfg


# Catch-all rule containing just a no-op command.
#
# Note: This pattern rule is canceled by the one defined in 
#       the conditionally included file below, if that file
#       exists.
#
%::
	$(NOOP)


# Add targets that are categorized as object-only for linkable object builds.
#
# Note: This is included conditionally, since the file is deleted when a
#       linkable object build is generated.
#
-include $(FP_ROOT)/$(src_esw)/make/Makefile.objdist

