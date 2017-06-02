################################################
#                                              #
#          FastPath Makefile                   #
#                                              #
# Milton Ruas, 2017                            #
# milton-r-silva@alticelabs.com                #
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
	@echo " make cxo640g            "
	@echo " make cxo160g            "
	@echo " make ta48ge             "
	@echo " make tg4g               "
	@echo " make tg16g              "
	@echo " make tg16gf             "
	@echo " make tt04sxg            "
	@echo " make olt1t0             "
	@echo " make olt1t0f            "
	@echo ""
	@echo " make cxo640g-clean      "
	@echo " make cxo160g-clean      "
	@echo " make ta48ge-clean       "
	@echo " make tg4g-clean         "
	@echo " make tg16g-clean        "
	@echo " make tg16gf-clean       "
	@echo " make tt04sxg-clean      "
	@echo " make olt1t0-clean       "
	@echo " make olt1t0f-clean      "
	@echo ""
	@echo " make cxo640g-cleanall   "
	@echo " make cxo160g-cleanall   "
	@echo " make ta48ge-cleanall    "
	@echo " make tg4g-cleanall      "
	@echo " make tg16g-cleanall     "
	@echo " make tg16gf-cleanall    "
	@echo " make tt04sxg-cleanall   "
	@echo " make olt1t0-cleanall    "
	@echo " make olt1t0f-cleanall   "
	@echo ""
	@echo " make cxo640g-install    "
	@echo " make cxo160g-install    "
	@echo " make ta48ge-install     "
	@echo " make tg4g-install       "
	@echo " make tg16g-install      "
	@echo " make tg16gf-install     "
	@echo " make tt04sxg-install    "
	@echo " make olt1t0-install     "
	@echo " make olt1t0f-install    "
	@echo ""
	
all: cxo640g cxo160g ta48ge tg16g tg16gf tt04sxg olt1t0 olt1t0f

clean: cxo640g-clean cxo160g-clean ta48ge-clean tg4g-clean tg16g-clean tg16gf-clean tt04sxg-clean olt1t0-clean olt1t0f-clean

cleanall: cxo640g-cleanall cxo160g-cleanall ta48ge-cleanall tg4g-cleanall tg16g-cleanall tg16gf-cleanall tt04sxg-cleanall olt1t0-cleanall olt1t0f-cleanall

install: cxo640g-install cxo160g-install ta48ge-install tg16g-install tg16gf-install tt04sxg-install olt1t0-install olt1t0f-install


cxo640g:
	@$(MAKE) -f swdrvr-cxo640g.make

cxo160g:
	@$(MAKE) -f swdrvr-cxo160g.make

ta48ge:
	@$(MAKE) -f swdrvr-ta48ge.make
	
tg4g:
	@$(MAKE) -f swdrvr-tg4g.make

tg16g:
	@$(MAKE) -f swdrvr-tg16g.make
	
tg16gf:
	@$(MAKE) -f swdrvr-tg16gf.make
	
tt04sxg:
	@$(MAKE) -f swdrvr-tt04sxg.make
	
olt1t0:
	@$(MAKE) -f swdrvr-olt1t0.make
	
olt1t0f:
	@$(MAKE) -f swdrvr-olt1t0f.make


cxo640g-clean:
	@$(MAKE) -f swdrvr-cxo640g.make clean

cxo160g-clean:
	@$(MAKE) -f swdrvr-cxo160g.make clean

ta48ge-clean:
	@$(MAKE) -f swdrvr-ta48ge.make clean
	
tg4g-clean:
	@$(MAKE) -f swdrvr-tg4g.make clean

tg16g-clean:
	@$(MAKE) -f swdrvr-tg16g.make clean
	
tg16gf-clean:
	@$(MAKE) -f swdrvr-tg16gf.make clean
	
tt04sxg-clean:
	@$(MAKE) -f swdrvr-tt04sxg.make clean
	
olt1t0-clean:
	@$(MAKE) -f swdrvr-olt1t0.make clean
	
olt1t0f-clean:
	@$(MAKE) -f swdrvr-olt1t0f.make clean

	
cxo640g-cleanall:
	@$(MAKE) -f swdrvr-cxo640g.make cleanall

cxo160g-cleanall:
	@$(MAKE) -f swdrvr-cxo160g.make cleanall

ta48ge-cleanall:
	@$(MAKE) -f swdrvr-ta48ge.make cleanall
	
tg4g-cleanall:
	@$(MAKE) -f swdrvr-tg4g.make cleanall

tg16g-cleanall:
	@$(MAKE) -f swdrvr-tg16g.make cleanall
	
tg16gf-cleanall:
	@$(MAKE) -f swdrvr-tg16gf.make cleanall
	
tt04sxg-cleanall:
	@$(MAKE) -f swdrvr-tt04sxg.make cleanall
	
olt1t0-cleanall:
	@$(MAKE) -f swdrvr-olt1t0.make cleanall
	
olt1t0f-cleanall:
	@$(MAKE) -f swdrvr-olt1t0f.make cleanall

	
cxo640g-install:
	@sh cxo640g.install

cxo160g-install:
	@sh cxo160g.install

ta48ge-install:
	@sh ta48ge.install
	
tg4g-install:
	@sh tg4g.install

tg16g-install:
	@sh tg16g.install
	
tg16gf-install:
	@sh tg16gf.install
	
tt04sxg-install:
	@sh tt04sxg.install
	
olt1t0-install:
	@sh olt1t0.install
	
olt1t0f-install:
	@sh olt1t0f.install
