################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install clean cleanall image kernel_cxp360g kernel_tolt8g cxp360g tolt8g clean_cxp360g clean_tolt8g cleanall_cxp360g cleanall_tolt8g install_cxp360g install_tolt8g

SVN_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = "test"

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo " make kernel_cxp360g     "
	@echo " make kernel_tolt8g      "
	@echo ""
	@echo " make all                "
	@echo " make cxp360g            "
	@echo " make tolt8g             "
	@echo ""
	@echo " make clean              "
	@echo " make clean_cxp360g      "
	@echo " make clean_tolt8g       "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_cxp360g   "
	@echo " make cleanall_tolt8g    "
	@echo ""
	@echo " make install            "
	@echo " make install_cxp360g    "
	@echo " make install_tolt8g     "
	@echo ""
	@echo " make image              "
	@echo ""

kernel_cxp360g:
	@$(MAKE) -f cxp360g.make kernel

kernel_tolt8g:
	@$(MAKE) -f tolt8g.make kernel

all:	cxp360g tolt8g

cxp360g:
	@$(MAKE) -f cxp360g.make

tolt8g:
	@$(MAKE) -f tolt8g.make

clean_cxp360g:
	@$(MAKE) -f cxp360g.make clean

clean_tolt8g:
	@$(MAKE) -f tolt8g.make clean

cleanall_cxp360g:
	@$(MAKE) -f cxp360g.make cleanall

cleanall_tolt8g:
	@$(MAKE) -f tolt8g.make cleanall

install_cxp360g:
	@$(MAKE) -f cxp360g.make install

install_tolt8g:
	@$(MAKE) -f tolt8g.make install

install:	install_cxp360g install_tolt8g

clean:		clean_cxp360g clean_tolt8g

cleanall:	cleanall_cxp360g cleanall_tolt8g

image:		install
		cd $(IMAGE_DIR) && ./fastpath-olt360.build $(version)

