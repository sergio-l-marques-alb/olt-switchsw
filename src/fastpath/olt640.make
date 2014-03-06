################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install clean cleanall kernel_cxo640g kernel_ta48ge kernel_tg16g cxo640g ta48ge tg16g clean_cxo640g clean_ta48ge clean_tg16g cleanall_cxo640g cleanall_ta48ge cleanall_tg16g install_cxo640g install_ta48ge install_tg16g image

SVN_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = "test"

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo " make kernel_cxo640g     "
	@echo " make kernel_ta48ge      "
	@echo " make kernel_tg16g       "
	@echo ""
	@echo " make all                "
	@echo " make cxo640g            "
	@echo " make ta48ge             "
	@echo " make tg16g              "
	@echo ""
	@echo " make clean              "
	@echo " make clean_cxo640g      "
	@echo " make clean_ta48ge       "
	@echo " make clean_tg16g        "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_cxo640g   "
	@echo " make cleanall_ta48ge    "
	@echo " make cleanall_tg16g     "
	@echo ""
	@echo " make install            "
	@echo " make install_cxo640g    "
	@echo " make install_ta48ge     "
	@echo " make install_tg16g      "
	@echo ""
	@echo " make image              "
	@echo ""

kernel_cxo640g:
	@$(MAKE) -f cxo640g.make kernel

kernel_ta48ge:
	@$(MAKE) -f ta48ge.make kernel

kernel_tg16g:
	@$(MAKE) -f tg16g.make kernel

all:	cxo640g tg16g ta48ge

cxo640g:
	@$(MAKE) -f cxo640g.make

ta48ge:
	@$(MAKE) -f ta48ge.make

tg16g:
	@$(MAKE) -f tg16g.make

clean_cxo640g:
	@$(MAKE) -f cxo640g.make clean

clean_ta48ge:
	@$(MAKE) -f ta48ge.make clean

clean_tg16g:
	@$(MAKE) -f tg16g.make clean

cleanall_cxo640g:
	@$(MAKE) -f cxo640g.make cleanall

cleanall_ta48ge:
	@$(MAKE) -f ta48ge.make cleanall

cleanall_tg16g:
	@$(MAKE) -f tg16g.make cleanall

install_cxo640g:
	@$(MAKE) -f cxo640g.make install

install_ta48ge:
	@$(MAKE) -f ta48ge.make install

install_tg16g:
	@$(MAKE) -f tg16g.make install

install:	install_cxo640g install_ta48ge install_tg16g

clean:		clean_cxo640g clean_ta48ge clean_tg16g

cleanall:	cleanall_cxo640g cleanall_ta48ge cleanall_tg16g

image:		install
		cd $(IMAGE_DIR) && ./fastpath-olt1t3.build $(version)

