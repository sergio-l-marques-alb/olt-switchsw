# File: bcmdrivers/Makefile
#
# Makefile for the Linux kernel modules.
#

-include $(KERNEL_DIR)/.config

export BCMDRIVERS_DIR:=$(src)

# The following must have config defined for each driver

#obj-y += regmod/
obj-y += timer/
obj-y += otp/
obj-y += rng/
obj-y += rng200/
obj-y += mdio/
obj-y += gsio_spi/
obj-y += sd/
obj-y += dma/
obj-y += qspi/
obj-y += nand/
obj-y += pwm/
obj-y += gpio/
obj-y += wdt/
obj-y += usb2h/
obj-y += usb3h/
obj-m += usbd/
obj-y += pcie/
obj-y += gmac/et/
obj-y += gmac/hnd/
obj-m += sound/
obj-y += smbus/
obj-y += sra/
obj-y += broadcom/char/fa/
#obj-m += tdm/
obj-y += ../bcmgpldrivers/
obj-y += fa2/
obj-y += crypto/
obj-y += xor/
ifdef CONFIG_MACH_CYGNUS
obj-y += synopsys/
endif


#obj-y += ../bcmpartnerdrivers/
obj-y += pmu/

### pae
obj-y += pae/paemsg/
obj-y += pae/ipsec/

## kdrivers integrated at ../kernel/linux-3.6.5/drivers/mtd/Makefile
## requires early compile

clean: 
