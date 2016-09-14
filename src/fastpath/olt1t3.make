################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install clean cleanall kernel_cxo640g kernel_ta48ge kernel_tg4g kernel_tg16g cxo640g ta48ge tg4g tg16g clean_cxo640g clean_ta48ge clean_tg4g clean_tg16g cleanall_cxo640g cleanall_ta48ge cleanall_tg4g cleanall_tg16g install_cxo640g install_ta48ge install_tg4g install_tg16g image

SVN_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = "test"

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo " make kernel_cxo640g     "
	@echo " make kernel_ta48ge      "
	@echo " make kernel_tg4g        "
	@echo " make kernel_tg16g       "
	@echo " make kernel_tg16gf      "
	@echo ""
	@echo " make all                "
	@echo " make cxo640g            "
	@echo " make ta48ge             "
	@echo " make tg4g               "
	@echo " make tg16g              "
	@echo " make tg16gf             "
	@echo ""
	@echo " make clean              "
	@echo " make clean_cxo640g      "
	@echo " make clean_ta48ge       "
	@echo " make clean_tg4g         "
	@echo " make clean_tg16g        "
	@echo " make clean_tg16gf       "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_cxo640g   "
	@echo " make cleanall_ta48ge    "
	@echo " make cleanall_tg4g      "
	@echo " make cleanall_tg16g     "
	@echo " make cleanall_tg16gf    "
	@echo ""
	@echo " make install            "
	@echo " make install_cxo640g    "
	@echo " make install_ta48ge     "
	@echo " make install_tg4g       "
	@echo " make install_tg16g      "
	@echo " make install_tg16gf     "
	@echo ""
	@echo " make image              "
	@echo ""

kernel_cxo640g:
	@$(MAKE) -f cxo640g.make kernel

kernel_ta48ge:
	@$(MAKE) -f ta48ge.make kernel

kernel_tg16gf:
	@$(MAKE) -f tg16gf.make kernel

kernel_tg16g:
	@$(MAKE) -f tg16g.make kernel

kernel_tg4g:
	@$(MAKE) -f tg4g.make kernel

cxo640g:
	@$(MAKE) -f cxo640g.make

ta48ge:
	@$(MAKE) -f ta48ge.make

tg4g:
	@$(MAKE) -f tg4g.make

tg16g:
	@$(MAKE) -f tg16g.make

tg16gf:
	@$(MAKE) -f tg16gf.make

clean_cxo640g:
	@$(MAKE) -f cxo640g.make clean

clean_ta48ge:
	@$(MAKE) -f ta48ge.make clean

clean_tg4g:
	@$(MAKE) -f tg4g.make clean

clean_tg16g:
	@$(MAKE) -f tg16g.make clean

clean_tg16gf:
	@$(MAKE) -f tg16gf.make clean

cleanall_cxo640g:
	@$(MAKE) -f cxo640g.make cleanall

cleanall_ta48ge:
	@$(MAKE) -f ta48ge.make cleanall

cleanall_tg4g:
	@$(MAKE) -f tg4g.make cleanall

cleanall_tg16g:
	@$(MAKE) -f tg16g.make cleanall

cleanall_tg16gf:
	@$(MAKE) -f tg16gf.make cleanall

install_cxo640g:
	@$(MAKE) -f cxo640g.make install

install_ta48ge:
	@$(MAKE) -f ta48ge.make install

install_tg4g:
	@$(MAKE) -f tg4g.make install

install_tg16g:
	@$(MAKE) -f tg16g.make install

install_tg16gf:
	@$(MAKE) -f tg16gf.make install

all:		cxo640g ta48ge tg16g tg16gf

install:	install_cxo640g install_ta48ge install_tg16g install_tg16gf

clean:		clean_cxo640g clean_ta48ge clean_tg16g clean_tg16gf

cleanall:	cleanall_cxo640g cleanall_ta48ge cleanall_tg16g cleanall_tg16gf

image:		install
		cd $(IMAGE_DIR) && ./fastpath-olt1t3.build $(version)

