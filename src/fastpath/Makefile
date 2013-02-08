################################################
#                                              #
#	          FastPath Makefile                #
#                                              #
#	Daniel Figueira, 2013                      #
#	daniel-f-figueira@ext.ptinovacao.pt        #
#                                              #
################################################


.PHONY: help h all tolt8g olt7-8chb cxp360g cli shell install_cxp360g install_tolt8g install_cli install_shell clean_tolt8g clean_olt7-8chb clean_cxp360g clean_cli clean_shell cleanall_tolt8g cleanall_olt7-8chb cleanall_cxp360g cleanall_cli cleanall_shell install clean cleanall kernel_tolt8g kernel_olt7-8chb kernel_cxp360g 

FP_CLI_DIR	= ../fastpath.cli
FP_CLI_BIN	= fastpath.cli

FP_SHELL_DIR	= ../fastpath.shell
FP_SHELL_BIN	= fastpath.shell

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo " make kernel_cxp360g     "
	@echo " make kernel_tolt8g      "
	@echo " make kernel_olt7-8chb   "
	@echo ""
	@echo " make all                "
	@echo " make cxp360g            "	
	@echo " make tolt8g             "
	@echo " make olt7-8chb          "
	@echo " make cli                "
	@echo " make shell              "
	@echo ""
	@echo " make clean              "
	@echo " make clean_cxp360g      "	
	@echo " make clean_tolt8g       "
	@echo " make clean_olt7-8chb    "
	@echo " make clean_cli          "
	@echo " make clean_shell        "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_cxp360g   "
	@echo " make cleanall_tolt8g    "
	@echo " make cleanall_olt7-8chb "
	@echo " make cleanall_cli       "
	@echo " make cleanall_shell     "
	@echo ""
	@echo " make install            "
	@echo " make install_tolt8g     "
	@echo " make install_cxp360g    "
	@echo " make install_olt7-8chb  "
	@echo " make install_cli        "
	@echo " make install_shell      "
	@echo ""

kernel_tolt8g:
	@$(MAKE) -f tolt8g.make kernel

kernel_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make kernel

kernel_cxp360g:
	@$(MAKE) -f cxp360g.make kernel

all:	cxp360g tolt8g cli shell

cxp360g:
	@$(MAKE) -f cxp360g.make

tolt8g:
	@$(MAKE) -f tolt8g.make

olt7-8chb:
	@$(MAKE) -f olt7-8chb.make

cli:
	@$(MAKE) -C $(FP_CLI_DIR)

shell:
	@$(MAKE) -C $(FP_SHELL_DIR)

clean_cxp360g:
	@$(MAKE) -f cxp360g.make clean

clean_tolt8g:
	@$(MAKE) -f tolt8g.make clean

clean_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make clean

clean_cli:
	@$(MAKE) clean -C $(FP_CLI_DIR)

clean_shell:
	@$(MAKE) clean -C $(FP_SHELL_DIR)

cleanall_cxp360g:
	@$(MAKE) -f cxp360g.make cleanall

cleanall_tolt8g:
	@$(MAKE) -f tolt8g.make cleanall
	
cleanall_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make cleanall

cleanall_cli:
	@$(MAKE) cleanall -C $(FP_CLI_DIR)

cleanall_shell:
	@$(MAKE) cleanall -C $(FP_SHELL_DIR)

install_cxp360g:
	@$(MAKE) -f cxp360g.make install

install_tolt8g:
	@$(MAKE) -f tolt8g.make install

install_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make install

install_cli:
	@$(MAKE) install -C $(FP_CLI_DIR)

install_shell:
	@$(MAKE) install -C $(FP_SHELL_DIR)

install:	install_cxp360g install_tolt8g cli shell

clean:		clean_cxp360g clean_tolt8g clean_olt7-8chb clean_cli clean_shell

cleanall:	cleanall_cxp360g cleanall_tolt8g cleanall_olt7-8chb cleanall_cli cleanall_shell

