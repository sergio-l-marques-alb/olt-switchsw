ifdef CONFIG_MACH_CYGNUS
obj-y := io_map.o northstar.o common.o northstar_dmu.o board_bu.o 
else
obj-y := io_map.o northstar.o common.o northstar_dmu.o board_bu.o idm.o
endif
obj-$(CONFIG_PM) += pm.o

#obj-$(CONFIG_MACH_NS) += board_bu.o

obj-$(CONFIG_MTD) += flash.o
