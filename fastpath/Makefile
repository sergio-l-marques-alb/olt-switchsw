################################################
#                                              #
#	          FastPath Makefile                #
#                                              #
#	Daniel Figueira, 2013                      #
#	daniel-f-figueira@ext.ptinovacao.pt        #
#                                              #
################################################


.PHONY: help h tolt8g olt7-8chb cxp360g clean_tolt8g clean_olt7-8chb clean_cxp360g cleanall_tolt8g cleanall_olt7-8chb cleanall_cxp360g cleanall kernel_tolt8g kernel_olt7-8chb kernel_cxp360g

help h:
	@echo ""
	@echo "Makefile Help            "
	@echo " make kernel_tolt8g      "
	@echo " make kernel_olt7-8chb   "
	@echo " make kernel_cxp360g     "
	@echo ""
	@echo " make tolt8g             "
	@echo " make olt7-8chb          "
	@echo " make cxp360g            "
	@echo ""
	@echo " make clean_tolt8g       "
	@echo " make clean_olt7-8chb    "
	@echo " make clean_cxp360g      "
	@echo ""
	@echo " make cleanall_tolt8g    "
	@echo " make cleanall_olt7-8chb "
	@echo " make cleanall_cxp360g   "
	@echo " make cleanall           "
	@echo ""

kernel_tolt8g:
	@$(MAKE) -f tolt8g.make kernel

kernel_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make kernel

kernel_cxp360g:
	@$(MAKE) -f cxp360g.make kernel

tolt8g:
	@$(MAKE) -f tolt8g.make

olt7-8chb:
	@$(MAKE) -f olt7-8chb.make

cxp360g:
	@$(MAKE) -f cxp360g.make

clean_tolt8g:
	@$(MAKE) -f tolt8g.make clean

clean_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make clean

clean_cxp360g:
	@$(MAKE) -f cxp360g.make clean

cleanall_tolt8g:
	@$(MAKE) -f tolt8g.make cleanall
	
cleanall_olt7-8chb:
	@$(MAKE) -f olt7-8chb.make cleanall

cleanall_cxp360g:
	@$(MAKE) -f cxp360g.make cleanall
	
cleanall: cleanall_tolt8g cleanall_olt7-8chb cleanall_cxp360g
