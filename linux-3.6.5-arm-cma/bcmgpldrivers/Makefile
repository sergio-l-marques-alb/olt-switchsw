# File: bcmgpldrivers/Makefile
#
# Makefile for the Linux kernel modules.
#

-include $(KERNEL_DIR)/.config

export BCMGPLDRIVERS_DIR:=$(src)

# The following must have config defined for each driver
obj-m += sound/codecs/wm8955/
obj-m += sound/codecs/wm8750/
obj-y += watchdog/

clean: 
