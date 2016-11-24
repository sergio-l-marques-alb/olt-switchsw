/*
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * eCos BDE
 */

#include "ecosbde.h"

#include <sal/types.h>
#include <sal/appl/pci.h>
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/core/boot.h>
#if 0
#include <sal/core/libc.h>
#endif
#include <cyg/hal/drv_api.h>
#ifdef BCM_MODENA_SUPPORT
#include <soc/modena/devids.h>
#else
#include <soc/devids.h>
#endif
#include <shared/util.h>

/* Structure of private spi device */
typedef struct spi_dev_s {
    uint8          cid;         /* Chip ID */
    uint8          part;        /* Part number of the chip */
    uint8          rev;         /* Revision of the chip */
    void           *robo;       /* ptr to robo info required to access SPI */
    unsigned short phyid_high;  /* PHYID HIGH in MII regs of detected chip */
    unsigned short phyid_low;   /* PHYID LOW in MII regs of detected chip */
} spi_dev_t;

struct spi_device_id {
    unsigned short phyid_high;  /* PHYID HIGH in MII regs of detected chip */
    unsigned short phyid_low;   /* PHYID LOW in MII regs of detected chip */
};

typedef struct ecosbde_dev_s {
    /* Specify the type of device, pci, spi, switch, ether ... */
    uint32 dev_type;

    ibde_dev_t bde_dev;
    union {
        pci_dev_t  _pci_dev;    /* PCI device type */
        spi_dev_t  _spi_dev;    /* SPI device type */
    } dev;
#define pci_dev		dev._pci_dev
#define spi_dev		dev._spi_dev
    int iLine;
    int iPin;
} ecosbde_dev_t;

/* HiNT HB4 PCI-PCI Bridge (21150 clone) */
#define HINT_HB4_VENDOR_ID 0x3388
#define HINT_HB4_DEVICE_ID 0x0022

#define HINT_BRIDGE_BUS_NO    1
#define HINT_BRIDGE_DEV_NO  14

#define PCI_MEM_START 0x08000000
#define PCI_MEM_END 0x0fffffff

#define MAX_SWITCH_DEVICES 16
#define MAX_ETHER_DEVICES 2
#define MAX_DEVICES (MAX_SWITCH_DEVICES + MAX_ETHER_DEVICES)
#define	BCM47XX_ENET_ID		0x4713		/* 4710 enet */

static ecosbde_dev_t _devices[MAX_DEVICES];
static int _n_devices = 0;
static int _switch_n_devices = 0;
static int _ether_n_devices = 0;

#define VALID_DEVICE(_n) ((_n >= 0) && (_n < _n_devices))
#define DEVICE_INDEX(_n) ((_n < _switch_n_devices) ? _n : \
		          (MAX_SWITCH_DEVICES+_n-_switch_n_devices))

static ecosbde_bus_t _bus;

#define BDE_DEV_MEM_MAPPED(d)                               \
            ((d) & (BDE_PCI_DEV_TYPE | BDE_ICS_DEV_TYPE |   \
                    BDE_EB_DEV_TYPE))

#ifdef BCM_RCPU_SUPPORT
/*****************EB devices***************/

static uint32  _read(int d, uint32 offset);

/*
 * Create EB device at the specified base address.
 */
int _ecosbde_add_eb_devices(void)
{
    char                prop[64], *s;
    int                 unit = 0, max;
    ecosbde_dev_t       *vxd;
    uint32              dev_rev_id = 0x0, baddr = 0;

    vxd = _devices + _n_devices;
    max = MAX_SWITCH_DEVICES - _n_devices;

    for (unit = 0; unit < max; unit++) {
        sal_sprintf(prop, "eb_dev_addr.%d", unit);

        if ((s = sal_config_get(prop)) && 
            (baddr = sal_ctoi(s, 0))) {

            vxd->bde_dev.base_address = (sal_vaddr_t) baddr;
            vxd->dev_type = BDE_EB_DEV_TYPE | BDE_SWITCH_DEV_TYPE;
            /* 
             * FIXME : programming CMIC_ENDIANESS_SEL register might need
             * to be programmed based on Processor.
             */
            dev_rev_id = _read(_n_devices, 0x178);  /* CMIC_DEV_REV_ID */
            vxd->bde_dev.device = dev_rev_id & 0xFFFF;
            vxd->bde_dev.rev = (dev_rev_id >> 16) & 0xFF;

            vxd->iLine = 0;
            vxd->iPin  = 0;

            _n_devices++;
            _switch_n_devices++;
            vxd++;
        }
    }

    return 0;
}
#endif /* BCM_RCPU_SUPPORT */


#ifndef BCM_ICS
static int _first_mac = 0;
static int _pri_bus_no = -1;
static int _n_pri_devices = 0;
static int _n_sec_devices = 0;
#endif

uint32 bcm_bde_soc_cm_memory_base = 0;

static const char *
_name(void)
{
    return "ecos-pci-bde";
}

static int
_num_devices(int type)
{
    switch (type) {
        case BDE_ALL_DEVICES:
    return _n_devices;
        case BDE_SWITCH_DEVICES:
            return _switch_n_devices; 
        case BDE_ETHER_DEVICES:
            return _ether_n_devices; 
    }

    return 0;
}

static const ibde_dev_t *
_get_dev(int d)
{
    int _d;

    if (!VALID_DEVICE(d)) {
        return NULL;
    }
    _d = DEVICE_INDEX(d);

    return &_devices[_d].bde_dev;
}

static uint32
_get_dev_type(int d)
{
    int _d;

    if (!VALID_DEVICE(d)) {
        return  0;
    }
    _d = DEVICE_INDEX(d);

    return _devices[_d].dev_type;
}

static uint32 
_pci_read(int d, uint32 addr)
{
    int _d;

    if (!VALID_DEVICE(d)) {
	    return (uint32)0xFFFFFFFF;
    }
    _d = DEVICE_INDEX(d);

    if (!(_devices[_d].dev_type & BDE_PCI_DEV_TYPE)) {
        return (uint32)0xFFFFFFFF;
    }

    return pci_config_getw(&_devices[_d].pci_dev, addr);
}

static int
_pci_write(int d, uint32 addr, uint32 data)
{
    int _d;

    if (!VALID_DEVICE(d)) {
	    return -1;
    }
    _d = DEVICE_INDEX(d);

    if (!(_devices[_d].dev_type & BDE_PCI_DEV_TYPE)) {
        return -1;
    }
    return pci_config_putw(&_devices[_d].pci_dev, addr, data);
}

static void
_pci_bus_features(int unit,int *be_pio, int *be_packet, int *be_other)
{
    *be_pio = _bus.be_pio;
    *be_packet = _bus.be_packet;
    *be_other = _bus.be_other;
}

#ifdef PCI_DECOUPLED
static uint32  
_read(int d, uint32 offset)
{
    int _d;

    if (!VALID_DEVICE(d)) {
	    return -1;
    }
    _d = DEVICE_INDEX(d);

    if (_devices[_d].dev_type & BDE_PCI_DEV_TYPE) {
        return sysPciRead(
            &(((uint32 *)_devices[_d].bde_dev.base_address)[offset / 4]));
    }

    if (_devices[_d].dev_type & (BDE_ICS_DEV_TYPE | BDE_EB_DEV_TYPE )) {
        return ((uint32 *)_devices[_d].bde_dev.base_address)[offset / 4];
    }
    return -1;
}

static int
_write(int d, uint32 offset, uint32 data)
{
    int _d;

    if (!VALID_DEVICE(d)) {
	    return -1;
    }
    _d = DEVICE_INDEX(d);

    if (_devices[_d].dev_type & BDE_PCI_DEV_TYPE) {
        sysPciWrite(
            &(((uint32 *)_devices[_d].bde_dev.base_address)[offset / 4]),
            data);
        return 0;
    }

    if (_devices[_d].dev_type & (BDE_ICS_DEV_TYPE | BDE_EB_DEV_TYPE )) {
        ((uint32 *)_devices[_d].bde_dev.base_address)[offset / 4] = data;
        return 0;
    }

    return -1;
}
#else
static uint32  
_read(int d, uint32 offset)
{
    int _d;

    if (!VALID_DEVICE(d)) {
	    return -1;
    }
    _d = DEVICE_INDEX(d);

    if (!(BDE_DEV_MEM_MAPPED(_devices[_d].dev_type))) {
        return -1;
    }

    return ((uint32 *)_devices[_d].bde_dev.base_address)[offset / 4];
}

static int
_write(int d, uint32 offset, uint32 data)
{
    int _d;

    if (!VALID_DEVICE(d)) {
	    return -1;
    }
    _d = DEVICE_INDEX(d);

    if (!(BDE_DEV_MEM_MAPPED(_devices[_d].dev_type))) {
        return -1;
    }

    ((uint32 *)_devices[_d].bde_dev.base_address)[offset / 4] = data;
    return 0;
}
#endif

static uint32 * 
_salloc(int d, int size, const char *name)
{
    COMPILER_REFERENCE(d);

    return sal_dma_alloc(size, (char *)name);
}

static void
_sfree(int d, void *ptr)
{
    COMPILER_REFERENCE(d);

    sal_dma_free(ptr);
}

static int 
_sflush(int d, void *addr, int length)
{
    COMPILER_REFERENCE(d);

    sal_dma_flush(addr, length);

    return 0;
}

static int
_sinval(int d, void *addr, int length)
{
    COMPILER_REFERENCE(d);

    sal_dma_inval(addr, length);

    return 0;
}

static int 
_interrupt_connect(int d, void (*isr)(void *), void *data)
{
    int _d;

    if (!VALID_DEVICE(d)) {
	    return -1;
    }
    _d = DEVICE_INDEX(d);

    if (!(_devices[_d].dev_type & 
          (BDE_PCI_DEV_TYPE | BDE_ICS_DEV_TYPE))) {
        return -1;
    }

    return pci_int_connect(_devices[_d].iLine, isr, data);
}
			 
static int
_interrupt_disconnect(int d)
{
    COMPILER_REFERENCE(d);

    return 0;
}

#ifdef   BCM_ICS

#define SB_K1_TO_PHYS(x) ((uint32)x & 0x1fffffff)
#define SB_PHYS_TO_K1(x) ((uint32)x | 0xa0000000)

static uint32 
_l2p(int d, void *laddr)
{
    COMPILER_REFERENCE(d);

    return (uint32) SB_K1_TO_PHYS(laddr);
}

static uint32 *
_p2l(int d, uint32 paddr)
{
    COMPILER_REFERENCE(d);

    return (uint32 *) SB_PHYS_TO_K1(paddr);
}

#else

#if defined(NSX)
#define SB_K0_TO_PHYS(x) ((uint32)x & 0x7fffffff)
#define SB_PHYS_TO_K0(x) ((uint32)x | 0x80000000)
#endif
#define SB_K1_TO_PHYS(x) ((uint32)x & 0x1fffffff)
#define SB_PHYS_TO_K1(x) ((uint32)x | 0xa0000000)

static uint32 
_l2p(int d, void *laddr)
{

#if defined(NSX)
    COMPILER_REFERENCE(d);
    return (uint32) SB_K0_TO_PHYS(laddr);
#else
    int _d;

    if (!VALID_DEVICE(d)) {
	    return -1;
    }
    _d = DEVICE_INDEX(d);

    if ((_devices[_d].dev_type & BDE_ETHER_DEV_TYPE)) {
        return (uint32) SB_K1_TO_PHYS(laddr);
    }else {
    return (uint32)laddr;
    }

#endif
}

static uint32 *
_p2l(int d, uint32 paddr)
{

#if defined(NSX)
    COMPILER_REFERENCE(d);
    return (uint32 *) SB_PHYS_TO_K0(paddr);
#else
    int _d;

    if (!VALID_DEVICE(d)) {
	    return NULL;
    }
    _d = DEVICE_INDEX(d);

    if ((_devices[_d].dev_type & BDE_ETHER_DEV_TYPE)) {
        return (uint32 *) SB_PHYS_TO_K1(paddr);
    }else {
    return (uint32 *) paddr;
    }
#endif
}

#endif

static ibde_t _ibde = {
    _name, 
    _num_devices, 
    _get_dev, 
    _get_dev_type, 
    _pci_read,
    _pci_write,
    _pci_bus_features,
    _read,
    _write,
    _salloc,
    _sfree,
    _sflush,
    _sinval,
    _interrupt_connect,
    _interrupt_disconnect,
    _l2p,
    _p2l
};

#ifdef BCM_MODENA_SUPPORT
static int
fixup_p2p_bridge(void)
{
    pci_dev_t   pcidev;
    uint32    data;
    uint32    mem_base;
    uint32    mem_limit;


    pcidev.busNo = HINT_BRIDGE_BUS_NO;
    pcidev.devNo = HINT_BRIDGE_DEV_NO;
    pcidev.funcNo = 0;
    mem_base = pci_config_getw(&pcidev, PCI_CONF_BASE4);
    if (mem_base == 0) {
        mem_base = PCI_MEM_START;
        mem_limit = PCI_MEM_END;
        data =  ((mem_base >> 16) | (mem_limit & 0xfff00000));
        pci_config_putw(&pcidev, PCI_CONF_BASE4, data);
    }
    /* Configure the PCI bus no */
    data = ((pcidev.busNo-1) | (pcidev.busNo) << 8 | (pcidev.busNo) << 16);
    pci_config_putw(&pcidev, PCI_CONF_BASE2, data);

    /* Configure command register */
    data = PCI_COMM_MAE | PCI_COMM_ME;
    data |= 0xffff0000; /* clear all status bits */
    pci_config_putw(&pcidev, PCI_CONF_COMM, data);
            
    return 0;
}
#endif

#ifdef   BCM_ICS
int
ecosbde_create(ecosbde_bus_t *bus, 
	     ibde_t **bde)
{
    cyg_drv_interrupt_unmask(1);
    
    if (_n_devices == 0) {
        ecosbde_dev_t *vxd;
	       _bus = *bus;
        uint32  dev_rev_id = 0x0;

        bcm_bde_soc_cm_memory_base = _bus.base_addr_start;
        vxd = _devices + _n_devices++;
        _switch_n_devices++;
      
        vxd->bde_dev.base_address = _bus.base_addr_start;
        vxd->iLine = _bus.int_line;
        vxd->iPin  = 0;
        _bus.base_addr_start += PCI_SOC_MEM_WINSZ;

        vxd->dev_type = BDE_ICS_DEV_TYPE | BDE_SWITCH_DEV_TYPE;

        dev_rev_id = _read(0, 0x178);  /* CMIC_DEV_REV_ID */
        vxd->bde_dev.device = dev_rev_id & 0xFFFF;
        vxd->bde_dev.rev = (dev_rev_id >> 16) & 0xFF;
    }
    *bde = &_ibde;

#ifdef BCM_RCPU_SUPPORT
    /*
     * Check if devices are connected to bus in EB mode.
     */
    if (sal_config_get("eb_probe") != NULL)
        _ecosbde_add_eb_devices();
    }
#endif /* BCM_RCPU_SUPPORT */

    return 0;  
}

#else /* ifdef BCM_ICS */

static int
_setup(pci_dev_t *dev, 
       uint16 pciVenID, 
       uint16 pciDevID,
       uint8 pciRevID)
{
    uint32 flags = 0;
    ecosbde_dev_t *vxd;
    char	pll_bypass[20];
    char	pci_soccfg0[20];
    char	pci_soccfg1[20];
    char	pci2eb_override[20];
    char	*val_str;
    int done = 0;

    if (pciVenID != BROADCOM_VENDOR_ID) {
        return 0;
    }

    /* don't want to expose non 56XX/53XXX devices */
    if(((pciDevID & 0xFF00) != 0x5600) &&
       ((pciDevID & 0xF000) != 0xc000) &&
       ((pciDevID & 0xF000) != 0xb000)) {
        if (pciDevID != BCM47XX_ENET_ID) {
            return 0;
        }
        /*
         * Use ENET MAC 1 
         */
        if (_first_mac == 0) {
            _first_mac++;
            return 0;
        }
        flags |= BDE_ETHER_DEV_TYPE;

        /* Pay dirt */
        vxd = _devices + MAX_SWITCH_DEVICES + _ether_n_devices++;
        done = 1;
    } else {
        flags |= BDE_SWITCH_DEV_TYPE;

        /* Pay dirt */
        vxd = _devices + _switch_n_devices++;
    }

    sal_sprintf(pll_bypass, "pll_bypass.%d", _n_devices);
    sal_sprintf(pci_soccfg0, "pci_conf_soccfg0.%d", _n_devices);
    sal_sprintf(pci_soccfg1, "pci_conf_soccfg1.%d", _n_devices);
    sal_sprintf(pci2eb_override, "pci2eb_override.%d", _n_devices);
    /* Pay dirt */
  
    flags |= BDE_PCI_DEV_TYPE;
    _n_devices++;
    vxd->bde_dev.device = pciDevID;
    vxd->bde_dev.rev = pciRevID;
    vxd->bde_dev.base_address = 0; /* read back */
    vxd->pci_dev = *dev;
    vxd->dev_type = flags;
	
	   /* Configure PCI */

    /* Write control word (turns on parity detect) */
    pci_config_putw(dev,PCI_CONF_COMMAND, ( PCI_CONF_COMMAND_BM |
			PCI_CONF_COMMAND_MS |
				PCI_CONF_COMMAND_PERR));
    if (_bus.base_addr_start) {    
	uint32 tmp;

      /*
       * The standard procedure to determine device window size is to
       * write 0xffffffff to BASE0, read it back, and see how many
       * LSBs are hardwired to zero.  In our case, we would get
       * 0xffff0000 indicating a window size of 64kB.
       *
       * While the window size could be assumed 64kB, we must still do
       * the standard write and read because some PCI bridges (most
       * notably the Quickturn Speed Bridge) observe these
       * transactions to record the window size internally.
       */
      
      pci_config_putw(dev, PCI_CONF_BAR0, 0xffffffff);
      (void)pci_config_getw(dev, PCI_CONF_BAR0);
      
      pci_config_putw(dev, PCI_CONF_BAR0, _bus.base_addr_start);
      
      tmp = pci_config_getw(dev,PCI_CONF_INTERRUPT_LINE);
      tmp = (tmp & ~0xff) | _bus.int_line;
      pci_config_putw(dev, PCI_CONF_INTERRUPT_LINE, tmp);
      
      /* should read window size from device */
      _bus.base_addr_start += PCI_SOC_MEM_WINSZ;
    }
#if defined(NSX)
    else {
      uint32 tmp;

      tmp = pci_config_getw(dev,PCI_CONF_INTERRUPT_LINE);
      tmp = (tmp & ~0xff) | _bus.int_line;
      pci_config_putw(dev,PCI_CONF_INTERRUPT_LINE, tmp);
    }
#endif /* NSX */

    vxd->bde_dev.base_address = (sal_vaddr_t) 
	   ((uint32*)(pci_config_getw(dev, PCI_CONF_BAR0) & PCI_CONF_BAR_MASK));
	
    if(flags & BDE_ETHER_DEV_TYPE){
        vxd->iLine = 0x2;
        vxd->iPin = 0x0;
    }else{            	
    vxd->iLine = pci_config_getw(dev, PCI_CONF_INTERRUPT_LINE) >> 0 & 0xff;
    vxd->iPin  = pci_config_getw(dev, PCI_CONF_INTERRUPT_LINE) >> 8 & 0xff;
    }

    /*
     * Set # retries to infinite.  Otherwise other devices using the bus
     * may monopolize it long enough for us to time out.
     */
    pci_config_putw(dev, PCI_CONF_TRDY_TO, 0x0080);

    /* 
     * Optionally enable PLL bypass in reserved register.
     */
    if ((val_str = sal_config_get(pll_bypass)) != NULL) {
        if (_shr_ctoi(val_str)) {
            pci_config_putw(dev, PCI_CONF_PLL_BYPASS, 0x2);
            if (SAL_BOOT_QUICKTURN) {
                sal_usleep(100000);
            }
        }
    }

    /*
     * Optionally configure clocks etc. in reserved registers.
     * These registers may contain multiple configuration bits.
     */
    if ((val_str = sal_config_get(pci_soccfg0)) != NULL) {
        pci_config_putw(dev, PCI_CONF_SOCCFG0, _shr_ctoi(val_str));
        if (SAL_BOOT_QUICKTURN) {
            sal_usleep(100000);
        }
    }
    if ((val_str = sal_config_get(pci_soccfg1)) != NULL) {
        pci_config_putw(dev, PCI_CONF_SOCCFG1, _shr_ctoi(val_str));
        if (SAL_BOOT_QUICKTURN) {
            sal_usleep(100000);
        }
    }

    /* Have at it */
    return 0;
}

static int
_soc_dev_probe(pci_dev_t *dev, 
       uint16 pciVenID, 
       uint16 pciDevID,
       uint8 pciRevID)
{
    if (pciVenID != BROADCOM_VENDOR_ID) {
        return 0;
    }

    /* don't want to expose non 56XX/53XXX devices */
    if(((pciDevID & 0xFF00) != 0x5600) &&
       ((pciDevID & 0xF000) != 0xc000) &&
       ((pciDevID & 0xF000) != 0xb000)) {
        return 0;
    }

    if (_pri_bus_no ==  -1) {
        _pri_bus_no = dev->busNo;
    }

    if (dev->busNo == _pri_bus_no) {
        _n_pri_devices++;
    } else {
        _n_sec_devices++;
    }
  
    return 0;
}

static void
_adjust_bus_base_addr_start(void)
{
    pci_device_iter(_soc_dev_probe);
    if (_n_sec_devices) {
        _bus.base_addr_start -= PCI_SOC_MEM_WINSZ * _n_pri_devices;
    }
    bcm_bde_soc_cm_memory_base = _bus.base_addr_start;
}

int
ecosbde_create(ecosbde_bus_t *bus, 
	     ibde_t **bde)
{
    /*
     * Unmask UART interrupt (1).
     *
     * XXX: It's strange that this interrupt mask got overwritten often
     *      if we unmask it in some other place.
     *      We have to do it right here to make it work.
     */
    cyg_drv_interrupt_unmask(1);

#ifdef BCM_MODENA_SUPPORT
    fixup_p2p_bridge();
#endif

#ifdef LVL7_FIXUP
    cyg_pci_init();
#endif

    if (_n_devices == 0) {
        _bus = *bus;
        _adjust_bus_base_addr_start();
        pci_device_iter(_setup);
    }
    *bde = &_ibde;
#ifdef PCI_DECOUPLED 
    sysPciRWInit();
#endif
    
#ifdef BCM_RCPU_SUPPORT
    /*
     * Check if devices are connected to bus in EB mode.
     */
    if (sal_config_get("eb_probe") != NULL) {
        _ecosbde_add_eb_devices();
    }
#endif /* BCM_RCPU_SUPPORT */        

    return 0;  
}
#endif

