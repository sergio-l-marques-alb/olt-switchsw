# It will make ecos kernel, bde and sdk src.
SHELL = /bin/sh
LOCALDIR = systems/ecos/common

all:check build

check:
ifeq (,$(SDK))
	@echo 'Makefile: error: The $$SDK environment variable is not set'; exit 1
else
ifeq (,$(platform))
	@echo 'Makefile: error: Internal error: platform variable is not set'; exit 1
else

override-target=ecos-$(platform)
include $(SDK)/make/Make.config
include $(SDK)/make/Make.depend
endif
endif

BCM = $(TOP)/bcm.$(targetplat)$(all_suffix)

all_targets = $(BCM)

# Define targets to be built by default.
build_targets = $(all_targets)

TOP := ../$(platform)

ifndef $(SDK_DIR)
SDK_DIR := $(TOP)/sdk
endif

include $(ECOSKERNEL_INSTALL_DIR)/include/pkgconf/ecos.mak

XCC           = $(CROSS_COMPILE)gcc
XCXX          = $(XCC)
XLD           = $(XCC)

CFLAGS        = -I$(MOZART)/include -I$(SDK)/include -I$(ECOSKERNEL_INSTALL_DIR)/include $(CFGFLAGS)
CXXFLAGS      = $(CFLAGS)
LDFLAGS       = -nostartfiles -L$(ECOSKERNEL_INSTALL_DIR)/lib -Ttarget.ld

# it was added in make.config too
CFLAGS    += -DDEBUG

DIAG_LIBS = \
	libdiag.a \
	${COND_LIB_TEST} \
	$(APPL_LIBS) \
	$(COND_LIB_EDITLINE) \
	$(COND_LIB_WBDM) \
	$(COND_LIB_SMB) \
	$(COND_LIB_PHY_DIAGNOSTICS) \
	$(COND_LIB_TCL) $(COND_LIB_BSAFE) \
	libsal_appl.a libsal_appl_plat.a
#	libsal_appl.a libsal_appl_plat.a liblubde.a

LIB_BDE = $(BLDDIR)/../../../libecosbde.a

BCM_LIBS = $(DIAG_LIBS) $(DRV_LIBS)
BCM_LIBS_BLD = $(addprefix $(BLDDIR)/../../../,$(BCM_LIBS))

# RULES

.PHONY: all clean kernel_modules sdk_src_modules

VPATH=$(SDK)/src/appl/test:.

clean::
	$(MAKE) -C $(TOP)/mips_build $@
	$(MAKE) -C $(SDK)/systems/bde/ecos $@
	$(MAKE) -C $(SDK)/src CFLAGS="$(CFLAGS)" $@
	-rm -f $(BLDDIR)/version.c $(BLDDIR)/version.o
	-rm -f $(BLDDIR)/socdiag.o
	-rm -f $(BCM) $(BCM).map $(BLDDIR)/ecosApp.o $(BLDDIR)/ecosApp.d \
	                         $(BLDDIR)/ecosEnd.o $(BLDDIR)/ecosEnd.d \
	                         $(BLDDIR)/shell.o $(BLDDIR)/shell.d \
	                         $(BLDDIR)/flashfs.o $(BLDDIR)/flashfs.d

build:: $(BLDDIR)/.tree $(build_targets)

%.o: %.c
	$(XCC) -c -o $*.o $(CFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.cxx
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.C
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.cc
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

kernel_modules:
	$(MAKE) -C $(SDK)/systems/bde/ecos

sdk_src_modules:
	$(MAKE) -C $(SDK)/src
	
$(BCM): ${BLDDIR}/version.o $(BLDDIR)/ecosApp.o $(BLDDIR)/ecosEnd.o $(BLDDIR)/shell.o $(BLDDIR)/flashfs.o $(BLDDIR)/socdiag.o $(BLDDIR)/version.o sdk_src_modules kernel_modules
	$(XLD) $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -Wl,-Map,$(BCM).map -o $@ $(BLDDIR)/ecosApp.o $(BLDDIR)/ecosEnd.o $(BLDDIR)/shell.o $(BLDDIR)/flashfs.o $(BLDDIR)/socdiag.o $(BLDDIR)/version.o $(LIB_BDE) $(BCM_LIBS_BLD)
	$(STRIP) $@

$(BCM).bin: $(BCM)
	mipsisa32-elf-objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $(BCM)

$(BCM).gz : $(BCM).bin
	gzip -c9 $(BCM) > $(BCM).gz

# Build Version

.PHONY: _force
$(BLDDIR)/version.c: _force
	$(MKVERS) > $@
$(BLDDIR)/version.o: $(BLDDIR)/version.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BLDDIR)/socdiag.o: $(TOP)//socdiag.c
	$(CC) $(CFLAGS) -c $< -o $@

# Output the CFLAGS used by the SDK
#
ifdef LVL7_FIXUP
.PHONY: get_sdk_flags

get_sdk_flags::
	@echo "CFLAGS_SDK := $(filter -D%,$(CFLAGS))" >$(L7_OUT_SDK_FLAGS)
endif

