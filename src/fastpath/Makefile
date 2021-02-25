################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Milton Ruas, 2017                            #
# milton-r-silva@alticelabs.com                #
#                                              #
################################################

# List of all supported cards
cards = cxo640g cxo160g ta48ge tc16sxg tg16g tg16gf olt1t0 olt1t0f


######################## Do not change any line below ########################

.PHONY: help h all install clean cleanall release

help h:
	@echo ""
	@echo "Fastpath Makefile Help                         "
	@echo ""
	@echo " Supported cards:                              "
	@echo "  $(cards)"
	@echo ""
	@echo " Global targets:                               "
	@echo "  make all         build all cards             "
	@echo "       clean       clean all cards             "
	@echo "       cleanall    cleanall all cards          "
	@echo "       install     install all cards           "
	@echo "       release     all+install, in sequence    "
	@echo ""
	@echo " Individual card targets:                      "
	@echo "  make <card>                                  "
	@echo "  make clean-<card>                            "
	@echo "  make cleanall-<card>                         "
	@echo "  make install-<card>                          "
	@echo "  make release-<card>                          "
	@echo ""
	@echo " NOTE: by default -j<MAX_CPU> is used          "
	@echo ""

# Maximum number of CPUs to be used
MAX_CPUS = $(shell grep -c '^processor' /proc/cpuinfo)

# if SWDRVR_MULTICORE is not defined, parallel board compilation will be done with all available cores
ifeq ($(SWDRVR_MULTICORE), )
export NUM_CPUS = $(MAX_CPUS)
EXTRA_FLAGS+=-j$(MAX_CPUS)

# if SWDRVR_MULTICORE is null, serial board compilation is done, but each board will be compiled with all available CPUs
else ifeq ($(SWDRVR_MULTICORE), 0)
export NUM_CPUS = $(MAX_CPUS)
EXTRA_FLAGS+=

# if SWDRVR_MULTICORE has a positive number, compile all boards with the specified number of CPUs
else
export NUM_CPUS = $(SWDRVR_MULTICORE)
EXTRA_FLAGS+=-j$(SWDRVR_MULTICORE)
endif

# Set by default to use the maximum CPUs available
export MAKEFLAGS += --no-print-directory $(EXTRA_FLAGS)

# Disable jobserver if multiple targets are detected
ifneq ($(words $(MAKECMDGOALS)), 0)
ifneq ($(words $(MAKECMDGOALS)), 1)
.NOTPARALLEL:
$(info WARNING: multiple targets detected. disabling jobserver mode)
endif
endif

# Global targets
all: $(cards)

clean: $(addprefix clean-,$(cards))

cleanall: $(addprefix cleanall-,$(cards))

install: $(addprefix install-,$(cards))

release:
	@$(MAKE) all
	@$(MAKE) install

# Individual targets
$(cards):
	@echo "$@ compilation started (log file swdrvr-$@.make.log) ..."
	@{ time $(MAKE) -f swdrvr-$@.make > swdrvr-$@.make.log 2>&1; } 2>.swdrvr-$@.make.time; echo "make-retval $$?" >> .swdrvr-$@.make.time;

	@{ ret=`cat .swdrvr-$@.make.time | grep 'make-retval' | awk '{print $$2}'`;\
	time=`cat .swdrvr-$@.make.time | grep 'real' | awk '{print $$2}'`;\
	if [ "$$ret" != "0" ]; then \
		echo "ERROR!!! $@ compilation finished in $${time} with errors:"; \
		tail -n 25 swdrvr-$@.make.log; \
		echo "Please see log file swdrvr-$@.make.log for more details."; \
		echo; \
		exit -1; \
	else \
		echo "Success: $@ compilation finished in $${time}"; \
	fi; \
	rm .swdrvr-$@.make.time; }

$(addprefix clean-, $(cards)):
	@echo "$@ started (log: swdrvr-$(subst clean-,,$@).clean.log) ..."
	@$(MAKE) -f swdrvr-$(subst clean-,,$@).make clean > swdrvr-$(subst clean-,,$@).clean.log 2>&1
	@echo "$@ done!"
	
$(addprefix cleanall-, $(cards)):
	@echo "$@ started (log: swdrvr-$(subst cleanall-,,$@).cleanall.log) ..."
	@$(MAKE) -f swdrvr-$(subst cleanall-,,$@).make cleanall > swdrvr-$(subst clean-,,$@).cleanall.log 2>&1
	@echo "$@ done!"
	
$(addprefix install-, $(cards)):
	@echo "$@ started (log: swdrvr-$(subst install-,,$@).install.log) ..."
	@sh swdrvr-$(subst install-,,$@).install  > swdrvr-$(subst clean-,,$@).install.log 2>&1
	@echo "$@ done!"

$(addprefix release-, $(cards)):
	@echo "Doing $@"
	@$(MAKE) $(subst release-,,$@)
	@$(MAKE) $(subst release,install,$@)

