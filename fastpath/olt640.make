################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install clean cleanall kernel_cxo640g kernel_cxo640g_v1 kernel_tg16g cxo640g cxo640g_v1 tg16g cli shell clean_cxo640g clean_cxo640g_v1 clean_tg16g clean_cli clean_shell cleanall_cxo640g cleanall_cxo640g_v1 cleanall_tg16g cleanall_cli cleanall_shell install_cxo640g install_cxo640g_v1 install_tg16g install_cli install_shell image

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
	@echo " make kernel_cxo640g     "
	@echo " make kernel_cxo640g_v1  "
	@echo " make kernel_tg16g       "
	@echo ""
	@echo " make all                "
	@echo " make cxo640g            "
	@echo " make cxo640g_v1         "
	@echo " make tg16g              "
	@echo " make cli                "
	@echo " make shell              "
	@echo ""
	@echo " make clean              "
	@echo " make clean_cxo640g      "
	@echo " make clean_cxo640g_v1   "
	@echo " make clean_tg16g        "
	@echo " make clean_cli          "
	@echo " make clean_shell        "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_cxo640g   "
	@echo " make cleanall_cxo640g_v1"
	@echo " make cleanall_tg16g     "
	@echo " make cleanall_cli       "
	@echo " make cleanall_shell     "
	@echo ""
	@echo " make install            "
	@echo " make install_cxo640g    "
	@echo " make install_cxo640g_v1 "
	@echo " make install_tg16g      "
	@echo " make install_cli        "
	@echo " make install_shell      "
	@echo ""
	@echo " make image              "
	@echo ""

kernel_cxo640g:
	@$(MAKE) -f cxo640g.make kernel

kernel_cxo640g_v1:
	@$(MAKE) -f cxo640g_v1.make kernel

kernel_tg16g:
	@$(MAKE) -f tg16g.make kernel

all:	cxo640g tg16g cli shell

cxo640g:
	@$(MAKE) -f cxo640g.make

cxo640g_v1:
	@$(MAKE) -f cxo640g_v1.make

tg16g:
	@$(MAKE) -f tg16g.make

cli:
	@$(MAKE) -C $(FP_CLI_DIR)

shell:
	@$(MAKE) -C $(FP_SHELL_DIR)

clean_cxo640g:
	@$(MAKE) -f cxo640g.make clean

clean_cxo640g_v1:
	@$(MAKE) -f cxo640g_v1.make clean

clean_tg16g:
	@$(MAKE) -f tg16g.make clean

clean_cli:
	@$(MAKE) clean -C $(FP_CLI_DIR)

clean_shell:
	@$(MAKE) clean -C $(FP_SHELL_DIR)

cleanall_cxo640g:
	@$(MAKE) -f cxo640g.make cleanall

cleanall_cxo640g_v1:
	@$(MAKE) -f cxo640g_v1.make cleanall

cleanall_tg16g:
	@$(MAKE) -f tg16g.make cleanall

cleanall_cli:
	@$(MAKE) cleanall -C $(FP_CLI_DIR)

cleanall_shell:
	@$(MAKE) cleanall -C $(FP_SHELL_DIR)

install_cxo640g:
	@$(MAKE) -f cxo640g.make install

install_cxo640g_v1:
	@$(MAKE) -f cxo640g_v1.make install

install_tg16g:
	@$(MAKE) -f tg16g.make install

install_cli:
	@$(MAKE) install -C $(FP_CLI_DIR)

install_shell:
	@$(MAKE) install -C $(FP_SHELL_DIR)

install:	install_cxo640g install_cxo640g_v1 install_tg16g install_cli install_shell

clean:		clean_cxo640g clean_cxo640g_v1 clean_tg16g clean_cli clean_shell

cleanall:	cleanall_cxo640g cleanall_cxo640g_v1 cleanall_tg16g cleanall_cli cleanall_shell

image:		install
		cd $(IMAGE_DIR) && ./fastpath-olt1t3.build $(version_final)

