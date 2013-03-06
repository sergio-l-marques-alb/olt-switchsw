################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all kernel_cxp360g kernel_cxo640g kernel_tolt8g kernel_tg16g kernel_olt7-8chb olt360 olt1t3 cxp360g cxo640g tolt8g tg16g olt7-8chb cli shell clean clean_olt360 clean_olt1t3 clean_cxp360g clean_cxo640g clean_tolt8g clean_tg16g clean_olt7-8chb clean_cli clean_shell cleanall cleanall_olt360 cleanall_olt1t3 cleanall_cxp360g cleanall_cxo640g cleanall_tolt8g cleanall_tg16g cleanall_olt7-8chb cleanall_cli cleanall_shell install install_olt360 install_olt1t3 install_cxp360g install_cxo640g install_tolt8g install_tg16g install_olt7-8chb install_cli install_shell image image_olt360 image_olt1t3

FASTPATH_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = 0.0.0
version_used = $(version)-r$(FASTPATH_REV)

FP_CLI_DIR	= ../fastpath.cli
FP_CLI_BIN	= fastpath.cli

FP_SHELL_DIR	= ../fastpath.shell
FP_SHELL_BIN	= fastpath.shell

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo " make kernel_cxp360g     "
	@echo " make kernel_cxo640g     "
	@echo " make kernel_tolt8g      "
	@echo " make kernel_tg16g       "
	@echo " make kernel_olt7-8chb   "
	@echo ""
	@echo " make all                "
	@echo " make olt360             "
	@echo " make olt1t3             "
	@echo " make cxp360g            "
	@echo " make cxo640g            "
	@echo " make tolt8g             "
	@echo " make tg16g              "
	@echo " make olt7-8chb          "
	@echo " make cli                "
	@echo " make shell              "
	@echo ""
	@echo " make clean              "
	@echo " make clean_olt360       "
	@echo " make clean_olt1t3       "
	@echo " make clean_cxp360g      "
	@echo " make clean_cxo640g      "
	@echo " make clean_tolt8g       "
	@echo " make clean_tg16g        "
	@echo " make clean_olt7-8chb    "
	@echo " make clean_cli          "
	@echo " make clean_shell        "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_olt360    "
	@echo " make cleanall_olt1t3    "
	@echo " make cleanall_cxp360g   "
	@echo " make cleanall_cxo640g   "
	@echo " make cleanall_tolt8g    "
	@echo " make cleanall_tg16g     "
	@echo " make cleanall_olt7-8chb "
	@echo " make cleanall_cli       "
	@echo " make cleanall_shell     "
	@echo ""
	@echo " make install            "
	@echo " make install_olt360     "
	@echo " make install_olt1t3     "
	@echo " make install_cxp360g    "
	@echo " make install_cxo640g    "
	@echo " make install_tolt8g     "
	@echo " make install_tg16g      "
	@echo " make install_olt7-8chb  "
	@echo " make install_cli        "
	@echo " make install_shell      "
	@echo ""
	@echo " make image              "
	@echo " make image_olt360       "
	@echo " make image_olt1t3       "
	@echo ""

kernel_cxp360g:
	@$(MAKE) -f cxp360g.make kernel

kernel_cxo640g:
	@$(MAKE) -f cxo640g.make kernel

kernel_tolt8g:
	@$(MAKE) -f tolt8g.make kernel

kernel_tg16g:
	@$(MAKE) -f tg16g.make kernel

kernel_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make kernel


all:	cxo640g cxp360g tg16g tolt8g cli shell

olt360:	cxp360g tolt8g cli shell

olt1t3:	cxo640g tg16g cli shell

cxp360g:
	@$(MAKE) -f cxp360g.make

cxo640g:
	@$(MAKE) -f cxo640g.make

tolt8g:
	@$(MAKE) -f tolt8g.make

tg16g:
	@$(MAKE) -f tg16g.make

olt7-8chb:
	@$(MAKE) -f olt7-8chb.make

cli:
	@$(MAKE) -C $(FP_CLI_DIR)

shell:
	@$(MAKE) -C $(FP_SHELL_DIR)

clean_olt360:		clean_cxp360g clean_tolt8g clean_cli clean_shell

clean_olt1t3:		clean_cxo640g clean_tg16g clean_cli clean_shell

clean_cxp360g:
	@$(MAKE) -f cxp360g.make clean

clean_cxo640g:
	@$(MAKE) -f cxo640g.make clean

clean_tolt8g:
	@$(MAKE) -f tolt8g.make clean

clean_tg16g:
	@$(MAKE) -f tg16g.make clean

clean_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make clean

clean_cli:
	@$(MAKE) clean -C $(FP_CLI_DIR)

clean_shell:
	@$(MAKE) clean -C $(FP_SHELL_DIR)

cleanall_olt360:	cleanall_cxp360g cleanall_tolt8g cleanall_cli cleanall_shell

cleanall_olt1t3:	cleanall_cxo640g cleanall_tg16g cleanall_cli cleanall_shell

cleanall_cxp360g:
	@$(MAKE) -f cxp360g.make cleanall

cleanall_cxo640g:
	@$(MAKE) -f cxo640g.make cleanall

cleanall_tolt8g:
	@$(MAKE) -f tolt8g.make cleanall

cleanall_tg16g:
	@$(MAKE) -f tg16g.make cleanall

cleanall_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make cleanall

cleanall_cli:
	@$(MAKE) cleanall -C $(FP_CLI_DIR)

cleanall_shell:
	@$(MAKE) cleanall -C $(FP_SHELL_DIR)

install_olt360:		install_cxp360g install_tolt8g install_cli install_shell

install_olt1t3: 	install_cxo640g install_tg16g install_cli install_shell

install_cxp360g:
	@$(MAKE) -f cxp360g.make install

install_cxo640g:
	@$(MAKE) -f cxo640g.make install

install_tolt8g:
	@$(MAKE) -f tolt8g.make install

install_tg16g:
	@$(MAKE) -f tg16g.make install

install_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make install

install_cli:
	@$(MAKE) install -C $(FP_CLI_DIR)

install_shell:
	@$(MAKE) install -C $(FP_SHELL_DIR)

install:	install_cxp360g install_cxo640g install_tolt8g install_tg16g install_cli install_shell

clean:		clean_cxp360g clean_cxo640g clean_tolt8g clean_tg16g clean_olt7-8chb clean_cli clean_shell

cleanall:	cleanall_cxp360g cleanall_cxo640g cleanall_tolt8g cleanall_tg16g cleanall_olt7-8chb cleanall_cli cleanall_shell


image:
	cd $(IMAGE_DIR) && ./fastpath-olt.build $(version_used)

image_olt360:
	cd $(IMAGE_DIR) && ./fastpath-olt360.build $(version_used)

image_olt1t3:
	cd $(IMAGE_DIR) && ./fastpath-olt1t3.build $(version_used)

