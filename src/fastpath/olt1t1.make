################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install clean cleanall kernel_cxo160g kernel_ta48ge kernel_tg4g kernel_tg16g cxo160g ta48ge tg4g tg16g clean_cxo160g clean_ta48ge clean_tg4g clean_tg16g cleanall_cxo160g cleanall_ta48ge cleanall_tg4g cleanall_tg16g install_cxo160g install_ta48ge install_tg4g install_tg16g image

SVN_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = "test"

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo " make kernel_cxo160g     "
	@echo " make kernel_ta48ge      "
	@echo " make kernel_tg16g       "
	@echo " make kernel_tg4g        "
	@echo ""
	@echo " make all                "
	@echo " make cxo160g            "
	@echo " make ta48ge             "
	@echo " make tg16g              "
	@echo " make tg4g               "
	@echo ""
	@echo " make clean              "
	@echo " make clean_cxo160g      "
	@echo " make clean_ta48ge       "
	@echo " make clean_tg16g        "
	@echo " make clean_tg4g         "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_cxo160g   "
	@echo " make cleanall_ta48ge    "
	@echo " make cleanall_tg16g     "
	@echo " make cleanall_tg4g      "
	@echo ""
	@echo " make install            "
	@echo " make install_cxo160g    "
	@echo " make install_ta48ge     "
	@echo " make install_tg16g      "
	@echo " make install_tg4g      "
	@echo ""
	@echo " make image              "
	@echo ""

kernel_cxo160g:
	@$(MAKE) -f cxo160g.make kernel

kernel_ta48ge:
	@$(MAKE) -f ta48ge.make kernel

kernel_tg4g:
	@$(MAKE) -f tg4g.make kernel

kernel_tg16g:
	@$(MAKE) -f tg16g.make kernel

all:	cxo160g tg16g ta48ge

cxo160g:
	@$(MAKE) -f cxo160g.make

ta48ge:
	@$(MAKE) -f ta48ge.make

tg4g:
	@$(MAKE) -f tg4g.make

tg16g:
	@$(MAKE) -f tg16g.make

clean_cxo160g:
	@$(MAKE) -f cxo160g.make clean

clean_ta48ge:
	@$(MAKE) -f ta48ge.make clean

clean_tg4g:
	@$(MAKE) -f tg4g.make clean

clean_tg16g:
	@$(MAKE) -f tg16g.make clean

cleanall_cxo160g:
	@$(MAKE) -f cxo160g.make cleanall

cleanall_ta48ge:
	@$(MAKE) -f ta48ge.make cleanall

cleanall_tg4g:
	@$(MAKE) -f tg4g.make cleanall

cleanall_tg16g:
	@$(MAKE) -f tg16g.make cleanall

install_cxo160g:
	@$(MAKE) -f cxo160g.make install

install_ta48ge:
	@$(MAKE) -f ta48ge.make install

install_tg4g:
	@$(MAKE) -f tg4g.make install

install_tg16g:
	@$(MAKE) -f tg16g.make install

install:	install_cxo160g install_ta48ge install_tg4g install_tg16g

clean:		clean_cxo160g clean_ta48ge clean_tg4g clean_tg16g

cleanall:	cleanall_cxo160g cleanall_ta48ge cleanall_tg4g cleanall_tg16g

image:		install
		cd $(IMAGE_DIR) && ./fastpath-olt1t1.build $(version)

