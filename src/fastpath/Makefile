################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Daniel Figueira, 2013                        #
# daniel-f-figueira@ext.ptinovacao.pt          #
#                                              #
################################################

.PHONY: help h all install clean cleanall

SVN_REV := $(shell svnversion -n | sed -e 's/.*://' -e 's/[A-Z]*$$//')

version = "test"

IMAGE_DIR       = ../../builds

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo ""
	@echo " make all                "
	@echo " make clean              "
	@echo " make cleanall           "
	@echo " make install            "
	@echo ""

all:
	@$(MAKE) -f tu100g.make
	@$(MAKE) -f tt08sxg.make
	@$(MAKE) -f ta12xg.make

install:
	@$(MAKE) -f tu100g.make install
	@$(MAKE) -f tt08sxg.make install
	@$(MAKE) -f ta12xg.make install

clean:		
	@$(MAKE) -f tu100g.make clean
	@$(MAKE) -f tt08sxg.make clean
	@$(MAKE) -f ta12xg.make clean

cleanall:
	@$(MAKE) -f tu100g.make cleanall
	@$(MAKE) -f tt08sxg.make cleanall
	@$(MAKE) -f ta12xg.make cleanall

