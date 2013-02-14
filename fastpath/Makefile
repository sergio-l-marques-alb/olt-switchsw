################################################
#                                              #
#	          FastPath Makefile                #
#                                              #
#	Daniel Figueira, 2013                      #
#	daniel-f-figueira@ext.ptinovacao.pt        #
#                                              #
################################################


.PHONY: help h all all_olt360 all_olt640 tolt8g tg16g olt7-8chb cxp360g cxo640g cli shell install_cxp360g install_cxo640g install_tolt8g install_tg16g install_cli install_shell clean_tolt8g clean_tg16g clean_olt7-8chb clean_cxp360g clean_cxo640g clean_cli clean_shell cleanall_tolt8g cleanall_tg16g cleanall_olt7-8chb cleanall_cxp360g cleanall_cxo640g cleanall_cli cleanall_shell install install_olt360 install_olt640 clean clean_olt360 clean_olt640 cleanall cleanall_olt360 cleanall_olt640 kernel_tolt8g kernel_tg16g kernel_olt7-8chb kernel_cxp360g kernel_cxo640g

FP_CLI_DIR	= ../fastpath.cli
FP_CLI_BIN	= fastpath.cli

FP_SHELL_DIR	= ../fastpath.shell
FP_SHELL_BIN	= fastpath.shell

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
	@echo " make cxp360g            "
	@echo " make cxo640g            "
	@echo " make tolt8g             "
	@echo " make tg16g             "
	@echo " make olt7-8chb          "
	@echo " make cli                "
	@echo " make shell              "
	@echo ""
	@echo " make clean              "
	@echo " make clean_cxp360g      "
	@echo " make clean_cxo640g      "
	@echo " make clean_tolt8g       "
	@echo " make clean_tg16g        "
	@echo " make clean_olt7-8chb    "
	@echo " make clean_cli          "
	@echo " make clean_shell        "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_cxp360g   "
	@echo " make cleanall_cxo640g   "
	@echo " make cleanall_tolt8g    "
	@echo " make cleanall_tg16g     "
	@echo " make cleanall_olt7-8chb "
	@echo " make cleanall_cli       "
	@echo " make cleanall_shell     "
	@echo ""
	@echo " make install            "
	@echo " make install_cxp360g    "
	@echo " make install_cxo640g    "
	@echo " make install_tolt8g     "
	@echo " make install_tg16g      "
	@echo " make install_olt7-8chb  "
	@echo " make install_cli        "
	@echo " make install_shell      "
	@echo ""
	@echo " make all_olt360         "
	@echo " make all_olt640         "
	@echo " make clean_olt360       "
	@echo " make clean_olt640       "
	@echo " make cleanall_olt360    "
	@echo " make cleanall_olt640    "
	@echo " make install_olt360     "
	@echo " make install_olt640     "
	@echo ""

kernel_tolt8g:
	@$(MAKE) -f tolt8g.make kernel

kernel_tg16g:
	@$(MAKE) -f tg16g.make kernel

kernel_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make kernel

kernel_cxp360g:
	@$(MAKE) -f cxp360g.make kernel

kernel_cxo640g:
	@$(MAKE) -f cxo640g.make kernel

all:	cxp360g cxo640g tolt8g tg16g cli shell

all_olt360:	cxp360g tolt8g cli shell

all_olt640:	cxo640g tg16g cli shell

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

install_olt360: install_cxp360g install_tolt8g cli shell

install_olt640: install_cxo640g install_tg16g cli shell

clean_olt360:	clean_cxp360g clean_tolt8g clean_cli clean_shell

clean_olt640:	clean_cxo640g clean_tg16g clean_cli clean_shell

cleanall_olt360:	cleanall_cxp360g cleanall_tolt8g cleanall_cli cleanall_shell

cleanall_olt640:	cleanall_cxo640g cleanall_tg16g cleanall_cli cleanall_shell

install:	install_cxp360g install_cxo640g install_tolt8g install_tg16g cli shell

clean:		clean_cxp360g clean_cxo640g clean_tolt8g clean_tg16g clean_olt7-8chb clean_cli clean_shell

cleanall:	cleanall_cxp360g cleanall_cxo640g cleanall_tolt8g cleanall_tg16g cleanall_olt7-8chb cleanall_cli cleanall_shell

