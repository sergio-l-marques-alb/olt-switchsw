################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install clean cleanall kernel_olt7-8chb olt7-8chb cli shell clean_olt7-8chb clean_cli clean_shell cleanall_olt7-8chb cleanall_cli cleanall_shell install_olt7-8chb install_cli install_shell

SVN_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = "test"

ifeq ($(version),"test")
 version_final = $(version)-r$(SVN_REV)
else
 version_final = $(version)-r$(SVN_REV)
endif

FP_CLI_DIR	= ../fastpath.cli
FP_CLI_BIN	= fastpath.cli

FP_SHELL_DIR	= ../fastpath.shell
FP_SHELL_BIN	= fastpath.shell

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo " make kernel_olt7-8chb   "
	@echo ""
	@echo " make all                "
	@echo " make olt7-8chb          "
	@echo " make cli                "
	@echo " make shell              "
	@echo ""
	@echo " make clean              "
	@echo " make clean_olt7-8chb    "
	@echo " make clean_cli          "
	@echo " make clean_shell        "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_olt7-8chb "
	@echo " make cleanall_cli       "
	@echo " make cleanall_shell     "
	@echo ""
	@echo " make install            "
	@echo " make install_olt7-8chb  "
	@echo " make install_cli        "
	@echo " make install_shell      "
	@echo ""

kernel_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make kernel


all:	olt7-8chb cli shell

olt7-8chb:
	@$(MAKE) -f olt7-8chb.make

cli:
	@$(MAKE) -C $(FP_CLI_DIR)

shell:
	@$(MAKE) -C $(FP_SHELL_DIR)

clean_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make clean

clean_cli:
	@$(MAKE) clean -C $(FP_CLI_DIR)

clean_shell:
	@$(MAKE) clean -C $(FP_SHELL_DIR)

cleanall_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make cleanall

cleanall_cli:
	@$(MAKE) cleanall -C $(FP_CLI_DIR)

cleanall_shell:
	@$(MAKE) cleanall -C $(FP_SHELL_DIR)

install_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make install

install_cli:
	@$(MAKE) install -C $(FP_CLI_DIR)

install_shell:
	@$(MAKE) install -C $(FP_SHELL_DIR)

install:	install_olt7-8chb install_cli install_shell

clean:		clean_olt7-8chb clean_cli clean_shell

cleanall:	cleanall_olt7-8chb cleanall_cli cleanall_shell

