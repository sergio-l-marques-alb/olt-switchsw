################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install image clean cleanall olt360 olt1t3 clean_olt360 clean_olt1t3 cleanall_olt360 cleanall_olt1t3 install_olt360 install_olt1t3 image_olt360 image_olt1t3

SVN_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = "test"

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo ""
	@echo " make all                "
	@echo " make olt360             "
	@echo " make olt1t3             "
	@echo ""
	@echo " make clean              "
	@echo " make clean_olt360       "
	@echo " make clean_olt1t3       "
	@echo ""
	@echo " make cleanall           "
	@echo " make cleanall_olt360    "
	@echo " make cleanall_olt1t3    "
	@echo ""
	@echo " make install            "
	@echo " make install_olt360     "
	@echo " make install_olt1t3     "
	@echo ""
	@echo " make image              "
	@echo " make image_olt360       "
	@echo " make install_olt1t3     "
	@echo ""

all:
	@$(MAKE) -f cxp360g.make
	@$(MAKE) -f tolt8g.make
	@$(MAKE) -f tg16g.make
	@$(MAKE) -f ta48ge.make
	@$(MAKE) -f cxo640g.make
	@$(MAKE) -f cxo160g.make
	@$(MAKE) -f olt1t0.make

install:
	@$(MAKE) -f cxp360g.make install
	@$(MAKE) -f tolt8g.make install
	@$(MAKE) -f tg16g.make install
	@$(MAKE) -f ta48ge.make install
	@$(MAKE) -f cxo640g.make install
	@$(MAKE) -f cxo160g.make install
	@$(MAKE) -f olt1t0.make install

clean:		clean_olt360 clean_olt1t3

cleanall:	cleanall_olt360 cleanall_olt1t3

image:		install
		cd $(IMAGE_DIR) && ./fastpath-olt.build $(version)

olt360:
	@$(MAKE) -f olt360.make all

olt1t3:
	@$(MAKE) -f olt1t3.make all

clean_olt360:
	@$(MAKE) -f olt360.make clean

clean_olt1t3:
	@$(MAKE) -f olt1t3.make clean

cleanall_olt360:
	@$(MAKE) -f olt360.make cleanall

cleanall_olt1t3:
	@$(MAKE) -f olt1t3.make cleanall

install_olt360:
	@$(MAKE) -f olt360.make install

install_olt1t3:
	@$(MAKE) -f olt1t3.make install

image_olt360:
	@$(MAKE) -f olt360.make image

image_olt1t3:
	@$(MAKE) -f olt1t3.make image

