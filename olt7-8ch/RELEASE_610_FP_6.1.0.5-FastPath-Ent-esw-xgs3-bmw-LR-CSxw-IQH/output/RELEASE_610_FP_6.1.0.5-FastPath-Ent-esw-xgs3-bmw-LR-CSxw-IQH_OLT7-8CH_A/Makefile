# makefile generated for /home/lvl7build/build/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/output/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH

uname=$(shell uname)

# global definitions

ifeq ($(OUTPATH),)
   export OUTPATH:=output/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH
endif

ifeq ($(FP_VIEWNAME),)
   export FP_VIEWNAME:=RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH
endif

ifeq ($(FP_BASE),)
   export FP_BASE=$(patsubst %/,%,$(CCVIEWS_HOME))/$(FP_VIEWNAME)
endif

ifeq ($(FP_ROOT),)
   export FP_ROOT=$(FP_BASE)
endif

ifeq ($(EMWEB_HOME),)
   export EMWEB_HOME=$(FP_BASE)/src/mgmt/broadcom/emweb
endif

ifeq ($(EMWEB_HOST),)
   export EMWEB_HOST=$(emweb_host)
endif

ifeq ($(LVL7_INTERNAL_TOOLS),)
   export LVL7_INTERNAL_TOOLS=N
endif

ifeq ($(LVL7_MAKEFILE_LOGGING),)
   export LVL7_MAKEFILE_LOGGING=N
endif

#set to one of S (summary), T (terse), or V (verbose)
ifeq ($(LVL7_MAKEFILE_DISPLAY_MODE),)
   export LVL7_MAKEFILE_DISPLAY_MODE=S
endif

#When debugging with a probe, set DEBUG_OPTIMIZE to 'FALSE'
ifeq ($(DEBUG_OPTIMIZE),)
   export DEBUG_OPTIMIZE=TRUE
endif


# setup runtime OS specific stuff
ifneq ($(findstring Linux, $(uname)),)
   echo=echo
   emitpath=echo
   emweb_host=pclinux

   ifeq ($(CROSS_COMPILE),)
      export CROSS_COMPILE=/opt/ppc-ptin-4.0.2/bin/powerpc-e300c3-linux-
   endif

   ifeq ($(KERNEL_SRC),)
      export KERNEL_SRC=/home/milton/OLT_8CH/KERNEL/linux-2.6.12.3-PQ2-256M
   endif

   export L7_HOSTOS=linux

else
   ifneq ($(findstring CYGWIN, $(uname)),)
      SHELL=sh.exe
      echo=echo
      emitpath=echo
      emweb_host=win32

      ifeq ($(CROSS_COMPILE),)
         export CROSS_COMPILE=z:/projects/nwsoft-toolchains/wrl_2.0/bmw/host-cross/powerpc-wrs-linux-gnu/bin/powerpc-wrs-linux-gnu-ppc_603e-glibc_std-
      endif

      ifeq ($(KERNEL_SRC),)
         export KERNEL_SRC=$(FP_BASE)/tools/build_kernels/linux-2.6.21--bmw
      endif

      export L7_HOSTOS=mswin32

   else
      ifneq ($(findstring SunOS, $(uname)),)
         echo=echo
         emitpath=cygpath -mp
         emweb_host=solaris

         ifeq ($(CROSS_COMPILE),)
            export CROSS_COMPILE=/projects/nwsoft-toolchains/wrl_2.0/bmw/host-cross/powerpc-wrs-linux-gnu/bin/powerpc-wrs-linux-gnu-ppc_603e-glibc_std-
         endif

         ifeq ($(KERNEL_SRC),)
            export KERNEL_SRC=$(FP_BASE)/tools/build_kernels/linux-2.6.21--bmw
         endif

         export L7_HOSTOS=solaris

      else
         $(error unknown host type uname=$(uname))
      endif
   endif
endif

export echo
export MAKESHELL=$(SHELL)

# mandatory environment variables

ifeq (,$(strip $(CCVIEWS_HOME)))
   $(error The 'CCVIEWS_HOME' environment variable must be set to the fully-qualified path of your ClearCase views base location)
endif
export CCVIEWS_HOME:=$(subst \,/,$(CCVIEWS_HOME))

# bring in all of the build rules and targets
include $(FP_ROOT)/src/l7tools/build/Makefile
