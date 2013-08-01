################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install image clean cleanall olt360 olt640 cli shell clean_olt360 clean_olt640 clean_cli clean_shell cleanall_olt360 cleanall_olt640 cleanall_cli cleanall_shell install_olt360 install_olt640 install_cli install_shell image_olt360 image_olt640

SVN_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = "test"

FP_CLI_DIR	= ../fastpath.cli
FP_CLI_BIN	= fastpath.cli

FP_SHELL_DIR	= ../fastpath.shell
FP_SHELL_BIN	= fastpath.shell

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo ""
	@echo " make all                "
	@echo " make olt360             "
	@echo " make olt640             "
	@echo " make olt1t3             "
	@echo " make cli                "
	@echo " make shell              "
	@echo ""
	@echo " make clean              "
	@echo " make clean_olt360       "
	@echo " make clean_olt640       "
	@echo " make clean_olt1t3       "
	@echo " make clean_cli          "
	@echo " make clean_shell        "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_olt360    "
	@echo " make cleanall_olt640    "
	@echo " make cleanall_olt1t3    "
	@echo " make cleanall_cli       "
	@echo " make cleanall_shell     "
	@echo ""
	@echo " make install            "
	@echo " make install_olt360     "
	@echo " make install_olt640     "
	@echo " make install_olt1t3     "
	@echo " make install_cli        "
	@echo " make install_shell      "
	@echo ""
	@echo " make image              "
	@echo " make image_olt360       "
	@echo " make image_olt640       "
	@echo " make install_olt1t3     "
	@echo ""

all:		olt360 olt640

install:	install_olt360 install_olt640

clean:		clean_olt360 clean_olt640

cleanall:	cleanall_olt360 cleanall_olt640

image:		install
		cd $(IMAGE_DIR) && ./fastpath-olt.build $(version)

olt360:
	@$(MAKE) -f olt360.make all

olt640:
	@$(MAKE) -f olt640.make all

olt1t3:	olt640

clean_olt360:
	@$(MAKE) -f olt360.make clean

clean_olt640:
	@$(MAKE) -f olt640.make clean

clean_olt1t3:		clean_olt640

cleanall_olt360:
	@$(MAKE) -f olt360.make cleanall

cleanall_olt640:
	@$(MAKE) -f olt640.make cleanall

cleanall_olt1t3:	cleanall_olt1t3

install_olt360:
	@$(MAKE) -f olt360.make install

install_olt640:
	@$(MAKE) -f olt640.make install

install_olt1t3:		install_olt640

image_olt360:
	@$(MAKE) -f olt360.make image

image_olt640:
	@$(MAKE) -f olt640.make image

image_olt1t3:		image_olt640

cli:
	@$(MAKE) -C $(FP_CLI_DIR)

shell:
	@$(MAKE) -C $(FP_SHELL_DIR)


clean_cli:
	@$(MAKE) clean -C $(FP_CLI_DIR)

clean_shell:
	@$(MAKE) clean -C $(FP_SHELL_DIR)

cleanall_cli:
	@$(MAKE) cleanall -C $(FP_CLI_DIR)

cleanall_shell:
	@$(MAKE) cleanall -C $(FP_SHELL_DIR)

install_cli:
	@$(MAKE) install -C $(FP_CLI_DIR)

install_shell:
	@$(MAKE) install -C $(FP_SHELL_DIR)

