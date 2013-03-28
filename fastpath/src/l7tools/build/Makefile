##############################################################################
#
# Makefile
#
# Purpose:  
#      Defines primary build targets.
#
##############################################################################

# Get compiler version.
#
# Note: The following command may be needed for older versions of
#       the compiler that do not support the -dumpversion option:
#
#           export gccver=$(word 3,$(shell $(CC) --version))
#
ifeq ($(gccver),)
 ifneq ($(findstring CYGWIN, $(uname)),)
   export gccver=<unknown>
 else
   export gccver=$(shell $(CC) -dumpversion)
 endif
endif

# Source code file types for compiling/assembling.
#
# Note: Defined here so make.cmdparms local build option will work.
#
export SRC_FILE_TYPES := c cpp s

# Name of file containing STK_FILE and IMG_PARMS variable definitions.
#
export STK_NAME_VARS_FILE := stk_file_vars

# Uniquely identify linkable object builds (i.e. preparing a view for object-only).
#
LINKABLE_OBJECT_BUILD       := 0
LINKABLE_OBJECT_ANDLSRC     := 0
FP_PREFIX                   := fp
ifneq (,$(strip $(filter linkable-object linkable-object-andlsrc,$(MAKECMDGOALS))))
  LINKABLE_OBJECT_BUILD     := 1
  FP_PREFIX                 := fpobj
  ifneq (,$(strip $(filter linkable-object-andlsrc,$(MAKECMDGOALS))))
    LINKABLE_OBJECT_ANDLSRC := 1
  endif
endif
export LINKABLE_OBJECT_BUILD LINKABLE_OBJECT_ANDLSRC FP_PREFIX

# Load build type and set flags for checking within makefiles.
#
bldtype_file := $(FP_ROOT)/$(OUTPATH)/make_bldtype
include $(bldtype_file)

DEVELOPMENT_BUILD           := 0
PRODUCTION_BUILD            := 0
ifeq (development,$(BLDTYPE))
  DEVELOPMENT_BUILD         := 1
endif
ifeq (production,$(BLDTYPE))
  PRODUTION_BUILD           := 1
endif
export DEVELOPMENT_BUILD PRODUCTION_BUILD

# Load package options.
#
include $(FP_ROOT)/$(OUTPATH)/package.cfg

# Check for special make invocation command parameters.
#
include $(FP_ROOT)/src/l7tools/build/make.cmdparms

# Include the build output definition info for all packages.
#
include $(FP_ROOT)/src/l7tools/build/make.pkginfo

# Include the make configuration file.
#
include $(FP_ROOT)/src/l7tools/build/make.cfg

# For VxWorks builds, check for required environment variables.
#
ifeq (1,$(BLD_L7_TARGETOS_VXWORKS))
 ifeq (,$(strip $(WIND_BASE)))
   $(error You must set 'WIND_BASE' in your environment)
 endif
 ifeq (,$(strip $(WIND_HOST_TYPE)))
   $(error You must set 'WIND_HOST_TYPE' in your environment)
 endif
 ipeakfilecheck := $(wildcard $(FP_ROOT)/vendor/wrs/ipstack/Makefile)
 ifeq (,$(ipeakfilecheck))
  ipeakscriptcheck := $(wildcard $(FP_ROOT)/vendor/wrs/ipstack/apply_changes.pl)
  ifneq (,$(ipeakscriptcheck))
   $(error IP stack source not found. cd to vendor/wrs/ipstack and run './apply_changes.pl')
  endif
 endif
endif

# Include the target definitions.
#
include $(FP_ROOT)/src/l7tools/build/make.targets

	    
# Note: Be sure to reference this using +$(SUB_MAKE) so that certain make
#       command line parameters are passed through to the sub-makefile.
#
SUB_MAKE := $(DBG)$(MAKE) $(MPARMS) -C $(FP_BASE)/src/l7tools/build -f makerules.targets

making_target_msg = @printf "Making Target:  '%s'\n"
done_msg          = @printf "DONE!!\n"


# Default target.
#
.PHONY: all
all::
	$(making_target_msg) bldlib
	+$(SUB_MAKE) bldlib
  ifeq (0,$(CMD_LOCAL_DIR_OR_FILE))
	$(making_target_msg) installexe
	+$(SUB_MAKE) installexe
    ifeq (Y,$(MKLOG))
	$(making_target_msg) snafu
	+$(SUB_MAKE) snafu
    endif
    ifeq (verbose,$(CMD_DISPLAY_MODE))
	$(making_target_msg) metrics
	+$(SUB_MAKE) metrics
    endif
  endif	
	$(done_msg)


# Target to redo link process.
# Note: For local builds, invoking sub-make on self to trigger error msg.
#
.PHONY: relink
relink::
  ifeq (0,$(CMD_LOCAL_DIR_OR_FILE))
	$(making_target_msg) cleanexe
	+$(SUB_MAKE) cleanexe
	$(making_target_msg) installexe
	+$(SUB_MAKE) installexe
	$(done_msg)
  else	
	+$(SUB_MAKE) relink
  endif


# Build linkable object archives of all source code that is not distributed.
#
# Note: Invoking this target will delete files from the view source tree and 
#       will overwrite certain Makefiles!  This is a destructive target that
#       can only be run once on a given source view.
#
# Note: A source view that is intended for conversion to a linkable object
#       distribution must contain one (and only one) defined output directory.
#
# Note: The linkable-object-andlsrc target is used to distribute the 
#       ANDL and vendor SDK code files in source format.  Normally, these
#       are intended to be distributed in object-only form.
#
.PHONY: linkable-object linkable-object-andlsrc
linkable-object linkable-object-andlsrc:
	$(making_target_msg) $@
	$(DBG) if [[ -f $(FASTPATH_LIB) ]] ; \
	then \
		$(echo) "ERROR: Cannot build a linkable object for a source tree that was already converted to source/object form." ;\
		exit 19 ; \
	fi
	+$(SUB_MAKE) bldlib
	$(DBG)$(MAKE) $(MPARMS) -C $(FP_BASE)/$(OUTPATH) -f $(FP_BASE)/src/l7tools/build/makerules.linkable
	$(DBG)$(MAKE) $(MPARMS) cleanall
	@$(echo) "Linkable object created.  Run 'make' to build an executable image."
	$(done_msg)


# Invoke buildable targets specified on the command line.
#
# Note: Using make recursion here so that only one command line target
#       is processed at a time.
#
.PHONY: $(ALL_TARGETS)
$(ALL_TARGETS)::
	$(making_target_msg) $@
	+$(SUB_MAKE) $@
	$(done_msg)


# Clean up all the log files generated by the make process.
#
.PHONY: cleanlog
cleanlog:
 ifeq (1,$(BLD_L7_HOSTOS_MSWIN32))
  ifeq ($(OLD_MAKE_VER),$(MAKE_VERSION))
	-$(DBG)find $(FP_ROOT)/$(OUTPATH) -name Makefile.log -exec rm -f {} ;
  else
	-$(DBG)find $(FP_ROOT)/$(OUTPATH) -name Makefile.log | xargs rm -f
  endif
 endif
 ifeq (1,$(BLD_L7_HOSTOS_LINUX))
	-$(DBG)find $(FP_ROOT)/$(OUTPATH) -name "Makefile.log" | xargs $(RM)
 endif
 ifeq (1,$(BLD_L7_HOSTOS_SOLARIS))
	-$(DBG)find $(FP_ROOT)/$(OUTPATH) -name "Makefile.log" -exec rm -f {} \;
 endif


# Bulk clean all of the build output files.
#

# Define top of view path relative to this Makefile in the source tree.
#
cleanall_top = $(FP_BASE)


# Define which output system directory to use for Linux builds.
#
ifeq (1,$(BLD_L7_TARGETOS_LINUX))
 ifeq (1,$(BLD_L7_TARGETOS_LINUX_24))
   linux_out_system := $(OUT_SYSTEM24)
 else
   linux_out_system := $(OUT_SYSTEM)
 endif
endif


# Indicate what is being removed (if debug messages are active).
#
ifneq (,$(CLEANING_MSG))
  removing_msg = echo Removing $$i ;
endif


# Common 'cleanall' logic for shell-based build environments.
#
# Note: Checking for what appears to be a legitimate output directory, based on 
#       path variable contents and the existence of the required package.cfg file,
#       before proceeding with the destructive 'rm -f -r' command.
#
define cleanall-shell-cmds
	$(DBG) if [[	! -n "$(OUTPATH)" || \
			! -d $(cleanall_top)/$(OUTPATH) || \
			! -s $(cleanall_top)/$(OUTPATH)/package.cfg ]] ; \
	then \
		$(echo) "ERROR: Attempting 'cleanall' for a non-output directory. Check your build environment." ;\
		exit 21 ; \
	fi

	$(DBG) for i in $$(find $(cleanall_top)/$(OUTPATH)/* -maxdepth 0 -type d -name "*" -print) ; \
	do \
		if [[ -d $$i ]] ; \
		then \
			$(removing_msg) \
			$(RM) -r $$i ; \
		fi ; \
	done
endef

# Common 'cleanall' logic for old-style build environments.
#
define cleanall-oldstyle-cmds
	-$(DBG)find $(FP_ROOT)/$(OUT_IPL) -type f -name "*" -exec rm -f {} ;
	-$(DBG)find $(FP_ROOT)/$(OUT_LIBS) -type f -name "*" -exec rm -f {} ;
	-$(DBG)find $(FP_ROOT)/$(OUT_OBJECTS) -type f -name "*" -exec rm -f {} ;
	-$(DBG)find $(FP_ROOT)/$(OUT_START) -type f -name "*" -exec rm -f {} ;
  ifeq (1,$(BLD_L7_TARGETOS_LINUX))
	-$(DBG)find $(FP_ROOT)/$(linux_out_system) -type f -name "*" -exec rm -f {} ;
	-$(DBG)find $(FP_ROOT)/$(OUT_TARGET) -type f -name "*" -exec rm -f {} ;
  endif
endef


# Clean up and remove all output subdirectories.
#
.PHONY: cleanall
cleanall: cleanlog
ifeq (1,$(BLD_L7_HOSTOS_MSWIN32))
  ifeq ($(OLD_MAKE_VER),$(MAKE_VERSION))
	$(cleanall-oldstyle-cmds)
  else
	$(cleanall-shell-cmds)  
  endif
endif
ifeq (1,$(BLD_L7_HOSTOS_LINUX))
	$(cleanall-shell-cmds)  
endif
ifeq (1,$(BLD_L7_HOSTOS_SOLARIS))
	$(cleanall-oldstyle-cmds)
endif

# Modify the build type: production (default) or development.
#
bldtype_temp := $(bldtype_file)_temp
.PHONY: development production
development production:: cleanall
	@if [ ! -s $(bldtype_file) ] ;\
	then \
		$(echo) ">>> Please run 'cchelper' to configure build parameters for the current output directory '$(out)'" ;\
		exit 21 ; \
	fi
	@cat $(bldtype_file) > $(bldtype_temp)
	@sed -e 's#^\(export BLDTYPE := \).*#\1$@#' <$(bldtype_temp) >$(bldtype_file)
	-@$(RM) $(bldtype_temp)
	@echo "The project build type has been set to: '$@'"

# Dump the required environment variables.
#
.PHONY: dump-variables dump_variables
dump-variables dump_variables:
	@$(echo) "SHELL=$(SHELL)"
	@$(echo) "MAKESHELL=$(MAKESHELL)"
	@$(echo) "MAKECMDGOALS=$(MAKECMDGOALS)"
	@$(echo) "MAKE_VERSION=$(MAKE_VERSION)"
	@$(echo) "uname=$(uname)"
	@$(echo) "gccver=$(gccver)"
	@$(echo) "CCVIEWS_HOME=$(CCVIEWS_HOME)"
	@$(echo) "CURDIR=$(CURDIR)"
	@$(echo) "OUTPATH=$(OUTPATH)"
	@$(echo) "FP_VIEWNAME=$(FP_VIEWNAME)"
	@$(echo) "FP_BASE=$(FP_BASE)"
	@$(echo) "FP_ROOT=$(FP_ROOT)"
	@$(echo) "EMWEB_HOME=$(EMWEB_HOME)"
	@$(echo) "EMWEB_HOST=$(EMWEB_HOST)"
	@$(echo) "BLDTYPE=$(BLDTYPE)"
	@$(echo) "LVL7_INTERNAL_TOOLS=$(LVL7_INTERNAL_TOOLS)"
	@$(echo) "LVL7_MAKEFILE_LOGGING=$(LVL7_MAKEFILE_LOGGING)"
	@$(echo) "LVL7_MAKEFILE_DISPLAY_MODE=$(LVL7_MAKEFILE_DISPLAY_MODE)"
	@$(echo) "CROSS_COMPILE=$(CROSS_COMPILE)"
	@$(echo) "KERNEL_SRC=$(KERNEL_SRC)"
	@$(echo) "LVL7_TORNADO_VER=$(LVL7_TORNADO_VER)"
	@$(echo) "WIND_BASE=$(WIND_BASE)"
	@$(echo) "WIND_HOST_TYPE=$(WIND_HOST_TYPE)"
	@$(echo) "PATH=$(PATH)"



# Dump the command parameter make variables (see make.cmdparms file).
#
.PHONY: dump-cmdparms dump_cmdparms
dump-cmdparms dump_cmdparms:
	@ $(echo) "CMD_DISPLAY_MODE=$(CMD_DISPLAY_MODE)"
	@ $(echo) "DBG=$(DBG)"
	@ $(echo) "MPARMS=$(MPARMS)"
	@ $(echo) "MKLOG=$(MKLOG)"
	@ $(echo) "GCOV=$(GCOV)"
	@ $(echo) "CMD_LOCAL_DIR_OR_FILE=$(CMD_LOCAL_DIR_OR_FILE)"
	@ $(echo) "CMD_LOCAL_DIR=$(CMD_LOCAL_DIR)"
	@ $(echo) "CMD_LOCAL_FILE=$(CMD_LOCAL_FILE)"


# List all target names for user help and debugging.
#
.PHONY: list-targets list_targets
list-targets list_targets:
	@$(echo) ""
	@$(echo) "MULTI TARGETS:"
	@$(echo) "$(sort $(MULTI_TARGETS))"
	@$(echo) ""
	@$(echo) "PRIMARY TARGETS:"
	@$(echo) "$(sort $(PRIMARY_TARGETS))"
	@$(echo) ""
	@$(echo) "PACKAGE TARGETS (bldlib):"
	@$(echo) "$(strip $(LIB_TARGETS))"
	@$(echo) ""
	@$(echo) "LINK TARGETS (installexe):"
	@$(echo) "$(strip $(LINK_TARGETS))"
	@$(echo) ""
	@$(echo) "CLEAN TARGETS (clean):"
	@$(echo) "$(sort $(CLEAN_TARGETS))"
	@$(echo) ""
	@$(echo) "DEBUG TARGETS (debug-variables):"
	@$(echo) "$(sort $(DEBUG_TARGETS))"
	@$(echo) ""


# Displays all output directory names (ones containing a package.cfg file).
#
outcfg_file := $(FP_ROOT)/make_configure
temp_file   := __temp_listout__

.PHONY: list-outputs list_outputs
list-outputs list_outputs::
	@$(echo) ""
	@$(echo) "Existing output directories: (+ denotes default from top-of-view):"
	$(shell cat $(outcfg_file) | grep ":=" | sed 's/.*:= //' > $(temp_file))
	@find $(FP_ROOT) -maxdepth 3 -type f -name "package.cfg" -printf "%h\n" \
		| grep -v "$(FP_ROOT)/(andl|bsp|os|src|tools|vendor)/" \
		| sed -e 's#$(FP_ROOT)/#  #'$ \
		| sed -e 's#  \($(shell cat $(temp_file))\)#+ \1#'$
	$(shell rm -f $(temp_file))
	@$(echo) ""


# Display debugging help information for this make system.
#
.PHONY: help-debug help_debug
help-debug help_debug:
	@ $(echo) ""
	@ $(echo) "MAKE SYSTEM DEBUGGING HELP INFORMATION"
	@ $(echo) "--------------------------------------"
	@ $(echo) ""
	@ $(echo) "The following commands may be useful when debugging make-related issues:"
	@ $(echo) ""
	@ $(echo) "  '$(MAKE) debug-variables' - display variables internal to this make system (see Note)"
	@ $(echo) "  '$(MAKE) dump-cmdparms'   - display the variables supporting optional command parameters"
	@ $(echo) "  '$(MAKE) dump-variables'  - display the required environment variables"
	@ $(echo) ""
	@ $(echo) "Note:  The '$(MAKE) debug-variables' command produces a lot of output, so it is recommended"
	@ $(echo) "that this be redirected to a file.  An alternative is to specify a package-specific debug"
	@ $(echo) "target, such as '$(MAKE) debug-switching'.  Run 'make list-targets' to see the full list."
	@ $(echo) ""


# Display general help information for this make system.
#
.PHONY: help
help:
	@ $(echo) ""
	@ $(echo) "MAKE SYSTEM HELP INFORMATION"
	@ $(echo) "----------------------------"
	@ $(echo) ""
	@ $(echo) "The make command may be invoked from either top-of-view or within an output directory."
	@ $(echo) ""
	@ $(echo) "  '$(MAKE) [target(s)] [options]'"
	@ $(echo) ""
	@ $(echo) "Common target names are:"
	@ $(echo) ""
	@ $(echo) "  all                - build and link entire image (default target)"
	@ $(echo) "  bldlib             - compile and pre-link all packages"
	@ $(echo) "  clean              - delete all output files"
	@ $(echo) "  cleanall           - delete all output files and remove all output sub-directories"
	@ $(echo) "  cleanlog           - delete all Makefile.log files"
	@ $(echo) "  configure          - set the default output directory for the view (requires out= option)"
	@ $(echo) "  convert            - create CLICK-format symbols for VxWorks source code debugger"
	@ $(echo) "  development        - set the make environment to do a 'development' build"
	@ $(echo) "  installexe         - link final executable(s)"
	@ $(echo) "  linkable-object    - create linkable object archives and delete undistributed source files"
	@ $(echo) "  linkable-object-andlsrc - like linkable-object, but ANDL (and vendor SDK) distributed as source"
	@ $(echo) "  metrics            - display various build metrics from link .map file(s)"
	@ $(echo) "  production         - set the make environment to do a 'production' build"
	@ $(echo) "  relink             - clean ipl (and start) package outputs and re-link final executable(s)"
	@ $(echo) "  snafu              - check all Makefile.log files for errors"
	@ $(echo) ""
	@ $(echo) "For a complete list of valid targets, type '$(MAKE) list-targets'"
	@ $(echo) ""
	@ $(echo) "Command line options (may be listed in any order):"
	@ $(echo) "  dmode=<s|t|v>       - override make message display mode:  summary (s), terse (t) or verbose (v)"
	@ $(echo) "                          - summary: captions each command as a single line of output (default)"
	@ $(echo) "                          - terse  : only start/end message, but does show warnings/errors"
	@ $(echo) "                          - verbose: displays all directory changes and every command executed"
	@ $(echo) "  -j 2 (or -j2)      - invoke parallel make jobs to speed up the build (Linux hosts only)"
	@ $(echo) "  local=<name>       - specify single directory or file to build (path relative to top-of-view)"
	@ $(echo) "  log=<y|n>          - override makefile logging mode:  on (y) or off (n)"
	@ $(echo) "  out=<output_path>  - specify default output directory (use with 'make configure')"
	@ $(echo) "  opt=<y|n>          - Turns on/off optimization.  On results in platform default."
	@ $(echo) "  gcov=<y|n>         - Turns on/off GCOV support.  If turned on, optimization is disabled."
	@ $(echo) "                       If turned on during compilation, it must be turned on when linking or"
	@ $(echo) "                       undefined reference errors will occur."
	@ $(echo) ""
	@ $(echo) "For additional assistance, please check the following commands:"
	@ $(echo) ""
	@ $(echo) "  '$(MAKE) help-debug'    - display additional make system debugging help information"
	@ $(echo) "  '$(MAKE) list-outputs'  - display active default output directory (from top-of-view only)"
	@ $(echo) "  '$(MAKE) list-targets'  - display list of all valid targets that can be invoked for this build"
	@ $(echo) ""

