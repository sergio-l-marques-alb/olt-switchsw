/*
 * $Id: dump.c,v 1.1 2011/04/18 17:11:00 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * socdiag dump command
 */

#include <sal/appl/pci.h>
#include <sal/core/sync.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/phyctrl.h>
#include <soc/l2x.h>
#include <soc/ll.h>
#ifdef BCM_EASYRIDER_SUPPORT
#include <soc/easyrider.h>
#endif

#include <bcm/l2.h>
#include <bcm/pkt.h>

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
#include <bcm/mirror.h>
#include <bcm_int/esw/port.h> 
#include <bcm_int/esw/trunk.h> 
#include <bcm_int/esw/vlan.h> 
#include <bcm_int/esw/l2.h> 
#include <bcm_int/esw/mcast.h> 
#include <bcm_int/esw/mirror.h>
#include <bcm/l3.h> 
#include <bcm_int/esw/l3.h> 
#include <bcm_int/esw/ipmc.h> 
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */ 

#include <appl/diag/system.h>
#include <appl/diag/decode.h>
#include <appl/diag/sysconf.h>
#include <appl/diag/dport.h>

#include <ibde.h>

#define DUMP_PHY_COLS	4
#define DUMP_MW_COLS	4
#define DUMP_MH_COLS	8
#define DUMP_MB_COLS	16

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_cosq_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/*
 * Dump all of the CMIC registers.
 */

static void
do_dump_pcim(int unit, uint32 off_start, uint32 count)
{
    uint32 off, val;

    if ((off_start & 3) != 0) {
	printk("dump_pcim ERROR: offset must be a multiple of 4\n");
	return;
    }

    for (off = off_start; count--; off += 4) {
	val = soc_pci_read(unit, off);
	printk("0x%04x %s: 0x%x\n", off, soc_pci_off2name(unit, off), val);
    }
}

/*
 * Dump all of the SOC register addresses, and if do_values is true,
 * read and dump their values along with the addresses.
 */

#define DREG_ADR_SEL_MASK     0xf       /* Low order 4 bits */
#define DREG_ADDR               0       /* Address only */
#define DREG_RVAL               1       /* Address and reset default value */
#define DREG_REGS               2       /* Address and real value */
#define DREG_DIFF               3       /* Addr & real value if != default */
#define DREG_CNTR               4       /* Address and real value if counter */

#define DREG_PORT_ALL -1
#define DREG_BLOCK_ALL -1

struct dreg_data {
    int unit;
    int dreg_select;
    int only_port;    /* Select which port/block.  -1 ==> all */
    int only_block;
};

static int
dreg(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    struct dreg_data *dd = data;
    uint32 value;
    uint64 val64, resetVal;
    char name[80];
    int is64, is_default, rv;
    int no_match = FALSE;  /* If specific port/block requested, turns true */
    char rval_str[20];

    /* Filter (set no_match) on ports and blocks if selected. */
    if (dd->only_port != DREG_PORT_ALL) {
        /* Only print ports that match */
        if (ainfo->port != dd->only_port) {
            no_match = TRUE;
        }
    }

    if (dd->only_block != DREG_BLOCK_ALL) {
        /* Only print blocks that match */
        if (ainfo->block != dd->only_block) {
            no_match = TRUE;
        } else { /* Match found; undo no_match */
            no_match = FALSE;
        }
    }

    if (no_match) {
        return 0;
    }

    soc_reg_sprint_addr(unit, name, ainfo);

    if (dd->dreg_select == DREG_ADDR) {
	printk("0x%08x %s\n", ainfo->addr, name);
	return 0;
    }

    SOC_REG_RST_VAL_GET(unit, ainfo->reg, resetVal);
    format_uint64(rval_str, resetVal);

    if (dd->dreg_select == DREG_RVAL) {
	printk("0x%08x %s = 0x%s\n", ainfo->addr, name, rval_str);
	return 0;
    }

    if (SOC_REG_INFO(unit, ainfo->reg).flags & SOC_REG_FLAG_WO) {
	printk("0x%08x %s = Write Only\n", ainfo->addr, name);
	return 0;
    }

    if (SOC_REG_IS_SPECIAL(unit, ainfo->reg)) {
	printk("0x%08x %s = Requires special processing\n",
               ainfo->addr, name);
	return 0;
    }

    if (reg_mask_subset(unit, ainfo, NULL)) {
        /* Register does not exist on this port/cos */
        return 0;
    }

    is64 = SOC_REG_IS_64(unit, ainfo->reg);

    if (is64) {
	rv = soc_reg64_read(dd->unit, ainfo->addr, &val64);
	is_default = COMPILER_64_EQ(val64, resetVal);
    } else {
	rv = soc_reg32_read(dd->unit, ainfo->addr, &value);
	is_default = (value == COMPILER_64_LO(resetVal));
    }

    if (rv < 0) {
	printk("0x%08x %s = ERROR\n", ainfo->addr, name);
	return 0;
    }

    if (dd->dreg_select == DREG_DIFF && is_default) {
	return 0;
    }

    if ((dd->dreg_select == DREG_CNTR)  &&
        (!(SOC_REG_INFO(unit, ainfo->reg).flags & SOC_REG_FLAG_COUNTER))) {
	return 0;
    }

    if (is64) {
	printk("0x%08x %s = 0x%08x%08x\n",
	       ainfo->addr, name,
	       COMPILER_64_HI(val64), COMPILER_64_LO(val64));
    } else {
	printk("0x%08x %s = 0x%08x\n", ainfo->addr, name, value);
    }

    return 0;
}

static cmd_result_t
do_dump_soc(int unit, int dreg_select, int only_port, int only_block)
{
    struct dreg_data dd;

    dd.unit = unit;
    dd.dreg_select = dreg_select;
    dd.only_port = only_port;
    dd.only_block = only_block;

    (void) soc_reg_iterate(unit, dreg, &dd);

    return CMD_OK;
}

#ifdef   BCM_ICS
/* Do nothing on ICS */
static void
_pci_print_config(int dev)
{
}
#else

static void
_pci_print_config(int dev)
{
    uint32		data;
    int                 cap_len, cap_base, next_cap_base, i;

    data = bde->pci_conf_read(dev, PCI_CONF_VENDOR_ID);
    soc_cm_print("%04x: %08x  DeviceID=%04x  VendorID=%04x\n",
		 PCI_CONF_VENDOR_ID, data,
		 (data & 0xffff0000) >> 16,
		 (data & 0x0000ffff) >>  0);

    data = bde->pci_conf_read(dev, PCI_CONF_COMMAND);
    soc_cm_print("%04x: %08x  Status=%04x  Command=%04x\n",
		 PCI_CONF_COMMAND, data,
		 (data & 0xffff0000) >> 16,
		 (data & 0x0000ffff) >>  0);
    cap_len = (data >> 16) & 0x10 ? 4 : 0;

    data = bde->pci_conf_read(dev, PCI_CONF_REVISION_ID);
    soc_cm_print("%04x: %08x  ClassCode=%06x  RevisionID=%02x\n",
		 PCI_CONF_REVISION_ID, data,
		 (data & 0xffffff00) >> 8,
		 (data & 0x000000ff) >> 0);

    data = bde->pci_conf_read(dev, PCI_CONF_CACHE_LINE_SIZE);
    soc_cm_print("%04x: %08x  BIST=%02x  HeaderType=%02x  "
		 "LatencyTimer=%02x  CacheLineSize=%02x\n",
		 PCI_CONF_CACHE_LINE_SIZE, data,
		 (data & 0xff000000) >> 24,
		 (data & 0x00ff0000) >> 16,
		 (data & 0x0000ff00) >>  8,
		 (data & 0x000000ff) >>  0);

    data = bde->pci_conf_read(dev, PCI_CONF_BAR0);
    soc_cm_print("%04x: %08x  BaseAddress0=%08x\n",
		 PCI_CONF_BAR0, data, data);

    data = bde->pci_conf_read(dev, PCI_CONF_BAR1);
    soc_cm_print("%04x: %08x  BaseAddress1=%08x\n",
		 PCI_CONF_BAR1, data, data);

    data = bde->pci_conf_read(dev, PCI_CONF_BAR2);
    soc_cm_print("%04x: %08x  BaseAddress2=%08x\n",
		 PCI_CONF_BAR2, data, data);

    data = bde->pci_conf_read(dev, PCI_CONF_BAR3);
    soc_cm_print("%04x: %08x  BaseAddress3=%08x\n",
		 PCI_CONF_BAR3, data, data);

    data = bde->pci_conf_read(dev, PCI_CONF_BAR4);
    soc_cm_print("%04x: %08x  BaseAddress4=%08x\n",
		 PCI_CONF_BAR4, data, data);

    data = bde->pci_conf_read(dev, PCI_CONF_BAR5);
    soc_cm_print("%04x: %08x  BaseAddress5=%08x\n",
		 PCI_CONF_BAR5, data, data);

    data = bde->pci_conf_read(dev, PCI_CONF_CB_CIS_PTR);
    soc_cm_print("%04x: %08x  CardbusCISPointer=%08x\n",
		 PCI_CONF_CB_CIS_PTR, data, data);

    data = bde->pci_conf_read(dev, PCI_CONF_SUBSYS_VENDOR_ID);
    soc_cm_print("%04x: %08x  SubsystemID=%02x  SubsystemVendorID=%02x\n",
		 PCI_CONF_SUBSYS_VENDOR_ID, data,
		 (data & 0xffff0000) >> 16,
		 (data & 0x0000ffff) >>  0);

    data = bde->pci_conf_read(dev, PCI_CONF_EXP_ROM);
    soc_cm_print("%04x: %08x  ExpansionROMBaseAddress=%08x\n",
		 PCI_CONF_EXP_ROM, data, data);

    data = bde->pci_conf_read(dev, 0x34);
    soc_cm_print("%04x: %08x  Reserved=%06x  CapabilitiesPointer=%02x\n",
		 0x34, data,
		 (data & 0xffffff00) >> 8,
		 (data & 0x000000ff) >> 0);
    cap_base = cap_len ? data & 0xff : 0;

    data = bde->pci_conf_read(dev, 0x38);
    soc_cm_print("%04x: %08x  Reserved=%08x\n",
		 0x38, data, data);

    data = bde->pci_conf_read(dev, PCI_CONF_INTERRUPT_LINE);
    soc_cm_print("%04x: %08x  Max_Lat=%02x  Min_Gnt=%02x  "
		 "InterruptLine=%02x  InterruptPin=%02x\n",
		 PCI_CONF_INTERRUPT_LINE, data,
		 (data & 0xff000000) >> 24,
		 (data & 0x00ff0000) >> 16,
		 (data & 0x0000ff00) >>  8,
		 (data & 0x000000ff) >>  0);

    data = bde->pci_conf_read(dev, 0x40);
    soc_cm_print("%04x: %08x  Reserved=%02x  "
		 "RetryTimeoutValue=%02x  TRDYTimeoutValue=%02x\n",
		 0x40, data,
		 (data & 0xffff0000) >> 16,
		 (data & 0x0000ff00) >>  8,
		 (data & 0x000000ff) >>  0);

    data = bde->pci_conf_read(dev, 0x44);
    soc_cm_print("%04x: %08x  PLLConf=%01x\n",
		 0x44, data,
		 (data & 0x000000ff) >>  0);

    data = bde->pci_conf_read(dev, 0x48);
    soc_cm_print("%04x: %08x  -\n",
		 0x48, data);

    while (cap_base) {
        data = bde->pci_conf_read(dev, cap_base);
        soc_cm_print("%04x: %08x  CompabilityID=%02x "
                     "CapabilitiesPointer=%02x ",
                     cap_base, data, data & 0xff, (data >> 8) & 0xff);
        next_cap_base = (data >> 8) & 0xff;
        switch (data & 0xff) {
        case 0x01:
            cap_len = 2 * 4;
            soc_cm_print("PWR-MGMT\n");
            break;
        case 0x03:
            cap_len = 2 * 4;
            soc_cm_print("VPD\n");
            break;
        case 0x05:
            cap_len = 6 * 4; /* 3 to 6 DWORDS */
            soc_cm_print("MSI\n");
            break;
        case 0x10:
            cap_len = 3 * 4;
            soc_cm_print("PCIE\n");
            break;
        case 0x11:
            cap_len = 3 * 4;
            soc_cm_print("MSI-X\n");
            break;
        default:
            break;
        }
        for (i = 4; i < cap_len; i += 4) {
            data = bde->pci_conf_read(dev, cap_base + i);
            soc_cm_print("%04x: %08x  -\n", cap_base + i, data);
        }
        cap_base = next_cap_base;
    }

#ifdef VXWORKS
    
    {
#ifdef IDTRP334
	extern void sysBusErrDisable(void);
	extern void sysBusErrEnable(void);
#endif

	/* HINT (R) HB4 PCI-PCI Bridge (21150 clone) */
#define HINT_HB4_VENDOR_ID    0x3388
#define HINT_HB4_DEVICE_ID    0x0022

	int BusNo, DevNo, FuncNo;
	unsigned short tmp;

#ifdef IDTRP334
	sysBusErrDisable();
#endif

	/*
	 * HINTCORP HB4 PCI-PCI Bridge
	 */
	if (pciFindDevice(HINT_HB4_VENDOR_ID,
			  HINT_HB4_DEVICE_ID,
			  0,
			  &BusNo, &DevNo, &FuncNo) != ERROR) {

	    printk("-------------------------------------\n");
	    printk("HB4 PCI-PCI Bridge Status Registers  \n");
	    printk("-------------------------------------\n");

	    /* Dump the status registers */
	    pciConfigInWord(BusNo,DevNo,FuncNo, 0x06, &tmp);
	    printk("Primary Status (%xh):   0x%x\n", 0x06, tmp);
	    pciConfigInWord(BusNo,DevNo,FuncNo, 0x1e, &tmp);
	    printk("Secondary Status (%xh): 0x%x\n", 0x1e, tmp);
	    pciConfigInWord(BusNo,DevNo,FuncNo, 0x3e, &tmp);
	    printk("Bridge Control (%xh):   0x%x\n", 0x3e, tmp);
	    pciConfigInWord(BusNo,DevNo,FuncNo, 0x6a, &tmp);
	    printk("P_SERR Status (%xh):    0x%x\n", 0x6a, tmp);
	}

#ifdef IDTRP334
	sysBusErrEnable();
#endif
    }
#endif
}
#endif

/*
 * Dump registers, tables, or an address space.
 */

static cmd_result_t
do_dump_registers(int unit, regtype_entry_t *rt, args_t *a)
{
    int		i;
    uint32	t1, t2;
    pbmp_t	pbmp;
    soc_port_t	port, dport;
    int		rv = CMD_OK;
    pci_dev_t	*pci_dev = NULL;
    uint32	flags = DREG_REGS;
    int		dump_port = DREG_PORT_ALL;
    int		dump_block = DREG_BLOCK_ALL;
    char	*an_arg;
    char	*count;

    an_arg = ARG_GET(a);
    count = ARG_GET(a);

    /* PCI config space does not take an offset */
    switch (rt->type) {
    case soc_pci_cfg_reg:
	_pci_print_config(unit);
	break;
    case soc_cpureg:
	if (an_arg) {
	    if (parse_cmic_regname(unit, an_arg, &t1) < 0) {
		printk("ERROR: unknown CMIC register name: %s\n", an_arg);
		rv = CMD_FAIL;
		goto done;
	    }
	    t2 = count ? parse_integer(count) : 1;
	} else {
	    t1 = CMIC_OFFSET_MIN;
	    t2 = (CMIC_OFFSET_MAX - CMIC_OFFSET_MIN) / 4 + 1;
	}
	do_dump_pcim(unit, t1, t2);
	break;

    case soc_schan_reg:
    case soc_genreg:
    case soc_portreg:
    case soc_cosreg:

        while (an_arg) {
            if (sal_strcasecmp(an_arg, "addr") == 0) {
                flags = DREG_ADDR;
            } else if (sal_strcasecmp(an_arg, "rval") == 0) {
                flags = DREG_RVAL;
            } else if (sal_strcasecmp(an_arg, "diff") == 0) {
                flags = DREG_DIFF;
            } else if (sal_strcasecmp(an_arg, "counter") == 0) {
                flags = DREG_CNTR;
            } else if (sal_strcasecmp(an_arg, "port") == 0) {
                an_arg = ARG_GET(a);
                dump_port = an_arg ? parse_integer(an_arg) : 0;
            } else if (sal_strcasecmp(an_arg, "block") == 0) {
                an_arg = ARG_GET(a);
                dump_block = an_arg ? parse_integer(an_arg) : 0;
            } else {
                printk("ERROR: unrecognized argument to DUMP SOC: %s\n",
                       an_arg);
                return CMD_FAIL;
            }
            if (count != NULL) {
                an_arg = count;
                count = NULL;
            } else {
                an_arg = ARG_GET(a);
            }
	}
        rv = do_dump_soc(unit, flags, dump_port, dump_block);
	break;

    case soc_phy_reg:
	if (an_arg) {
	    if (parse_pbmp(unit, an_arg, &pbmp)) {
		printk("Error: Invalid port identifier: %s\n", an_arg);
		rv = CMD_FAIL;
		break;
	    }
	} else {
	    pbmp = PBMP_PORT_ALL(unit);
	}
	SOC_PBMP_AND(pbmp, PBMP_PORT_ALL(unit));
        DPORT_SOC_PBMP_ITER(unit, pbmp, dport, port) {
	    uint8	phy_id = PORT_TO_PHY_ADDR(unit, port);
	    uint16	phy_data, phy_reg;
	    printk("\nPort %d (Phy ID %d)", port + 1, phy_id);
	    for (phy_reg = PHY_MIN_REG; phy_reg <= PHY_MAX_REG; phy_reg++) {
		rv = soc_miim_read(unit, phy_id, phy_reg, &phy_data);
		if (rv < 0) {
		    printk("Error: Port %d: cmic_read_miim failed: %s\n",
			   port + 1, soc_errmsg(rv));
		    rv = CMD_FAIL;
		    goto done;
		}
		printk("%s\t0x%02x: 0x%04x",
		       ((phy_reg % DUMP_PHY_COLS) == 0) ? "\n" : "",
		       phy_reg, phy_data);
	    }
	    printk("\n");
	}
	break;

    case soc_hostmem_w:
	if (!an_arg) {
	    printk("Dumping memory requires address and optional count\n");
	    rv = CMD_FAIL;
	    goto done;
	}
	t1 = parse_integer(an_arg) & ~3;
	t2 = count ? parse_integer(count) : 1;
	for (i = 0; i < (int)t2; i++, t1 += 4) {
	    if ((i % DUMP_MW_COLS) == 0)
		printk("%08x: ", t1);
	    printk("%08x%c",
		   pci_dma_getw(pci_dev, t1),
		   ((i + 1) % DUMP_MW_COLS) == 0 ? '\n' : ' ');
	}
	if (i % DUMP_MW_COLS)
	    printk("\n");
	break;
    case soc_hostmem_h:
	if (!an_arg) {
	    printk("Dumping memory requires address and optional count\n");
	    rv = CMD_FAIL;
	    goto done;
	}
	t1 = parse_integer(an_arg) & ~1;
	t2 = count ? parse_integer(count) : 1;
	for (i = 0; i < (int)t2; i++, t1 += 2) {
	    if ((i % DUMP_MH_COLS) == 0)
		printk("%08x: ", t1);
	    printk("%04x%c",
		   pci_dma_geth(pci_dev, t1),
		   ((i + 1) % DUMP_MH_COLS) == 0 ? '\n' : ' ');
	}
	if (i % DUMP_MH_COLS)
	    printk("\n");
	break;
    case soc_hostmem_b:
	if (!an_arg) {
	    printk("Dumping memory requires address and optional count\n");
	    rv = CMD_FAIL;
	    goto done;
	}
	t1 = parse_integer(an_arg);
	t2 = count ? parse_integer(count) : 1;
	for (i = 0; i < (int)t2; i++, t1 += 1) {
	    if ((i % DUMP_MB_COLS) == 0)
		printk("%08x: ", t1);
	    printk("%02x%c",
		   pci_dma_getb(pci_dev, t1),
		   ((i + 1) % DUMP_MB_COLS) == 0 ? '\n' : ' ');
	}
	if (i % DUMP_MB_COLS)
	    printk("\n");
	break;
    default:
	printk("Dumping register type %s is not yet implemented.\n",
	       rt->name);
	rv = CMD_FAIL;
	break;
    }

 done:
    return rv;
}

#define DUMP_TABLE_RAW		0x01
#define DUMP_TABLE_HEX		0x02
#define DUMP_TABLE_ALL		0x04
#define DUMP_TABLE_CHANGED	0x08

#ifdef BCM_EASYRIDER_SUPPORT

STATIC cmd_result_t
_do_dump_external_cells(int unit, soc_mem_t mem,
                        int copyno, int index, int count, int flags)
{
    soc_er_cd_chunk_t    chunk_data = NULL;
    soc_er_cd_slice_t    slice_data = NULL;
    soc_er_cd_columns_t  columns_data = NULL;
    int                  idx, chunk, word;
    int                  slice, column;
    int		         r, rv = CMD_OK;
    

    if (soc_er_dual_dimensional_array_alloc(&chunk_data, SOC_ER_CELLDATA_CHUNKS,
                                            SOC_MAX_MEM_WORDS) < 0) {
        printk ("Unable to allocate memory for chunk_data \n");
        rv = CMD_FAIL;
        goto done;
    }
    if (soc_er_dual_dimensional_array_alloc(&slice_data, SOC_ER_CELLDATA_SLICES,
                                            SOC_ER_CELLDATA_SLICE_WORDS) < 0) {
        printk ("Unable to allocate memory for slice_data \n");
        rv = CMD_FAIL;
        goto done;
    }
    if (soc_er_dual_dimensional_array_alloc(&columns_data, SOC_ER_CELLDATA_SLICES,
                                            SOC_ER_CELLDATA_SLICE_COLUMNS) < 0) {
        printk ("Unable to allocate memory for columns_data \n");
        rv = CMD_FAIL;
        goto done;
    }

    for (idx = index; idx < index + count; idx++) {
        if ((r = soc_er_celldata_chunk_read(unit, mem, idx, chunk_data)) < 0) {
            printk("Failed to read memory %s[%d]: %s\n",
                   SOC_MEM_NAME(unit, mem), idx, soc_errmsg(r));
            rv = CMD_FAIL;
            goto done;
        }

        printk("\n%s[%d]:\n", SOC_MEM_NAME(unit, mem), idx);
         
        if ((flags & DUMP_TABLE_RAW) || (flags & DUMP_TABLE_HEX)) {
            soc_er_celldata_chunk_to_slice(chunk_data, slice_data);
            if (flags & DUMP_TABLE_HEX) {
                soc_er_celldata_slice_to_columns(slice_data, columns_data);
            }

            for (slice = 0; slice < SOC_ER_CELLDATA_SLICES; slice++) {
                if (flags & DUMP_TABLE_HEX) {
                    printk("Slice[%2d]:", slice);
                    for (column = 3; column >= 0; column--) {
                        printk("\tColumn%d: 0x%05x", column,
                               columns_data[slice][column]);
                    }
                    printk("\n");
                } else {
                    printk("Slice[%2d]: 0x", slice);
                    printk("%02x", slice_data[slice][2] & 0xff);
                    printk("%08x", slice_data[slice][1]);
                    printk("%08x", slice_data[slice][0]);
                    printk("\n");
                }
            }
        } else {
            for (chunk = 0; chunk < SOC_ER_CELLDATA_CHUNKS; chunk++) {
                printk("Chunk[%2d]: 0x", chunk);
                for (word = SOC_ER_CELLDATA_CHUNK_WORDS - 1; word >= 0; word--) {
                    printk("%08x", chunk_data[chunk][word]);
                }
                printk("\n");
            }
        }
    }

 done:

     if (NULL != chunk_data) {
         soc_er_dual_dimensional_array_free(chunk_data, SOC_ER_CELLDATA_CHUNKS);
     }
     if (NULL != slice_data) {
         soc_er_dual_dimensional_array_free(slice_data, SOC_ER_CELLDATA_SLICES);
     }
     if (NULL != columns_data) {
         soc_er_dual_dimensional_array_free(columns_data, 
                                            SOC_ER_CELLDATA_SLICES);
     }

    return rv;
}
#endif /* BCM_EASYRIDER_SUPPORT */

cmd_result_t
do_dump_table(int unit, soc_mem_t mem,
	      int copyno, int index, int count, int flags)
{
    int			k, i;
    uint32		entry[SOC_MAX_MEM_WORDS];
    char		lineprefix[256];
    int			entry_dw;
    int			rv = CMD_FAIL;

    assert(copyno >= 0);

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit) && ((mem == C0_CELLm) || (mem == C1_CELLm))) {
        return _do_dump_external_cells(unit, mem, copyno, index, count, flags);
    }
#endif
    entry_dw = soc_mem_entry_words(unit, mem);

    for (k = index; k < index + count; k++) {

#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit) && (mem == FP_EXTERNALm)) {
            /* Access in normalized format */
            i = soc_er_fp_ext_read(unit, k, (void *)entry);
        } else if (SOC_IS_EASYRIDER(unit) && (mem == FP_TCAM_EXTERNALm)) {
            /* Access in normalized format */
            i = soc_er_fp_tcam_ext_read(unit, k, (void *)entry);
        } else
#endif
        {
            i = soc_mem_read(unit, mem, copyno, k, entry);
        }
	if (i < 0) {
	    printk("Read ERROR: table %s.%s[%d]: %s\n",
		   SOC_MEM_UFNAME(unit, mem),
		   SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i));
	    goto done;
	}

	if (!(flags & DUMP_TABLE_ALL)) {
	    int		validf;

	    validf = -1;

#if defined(BCM_TRX_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
	    if (soc_mem_field_length(unit, mem, ACTIVEf) > 0) {
		validf = ACTIVEf;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }
#endif /* BCM_TRX_SUPPORT || BCM_EASYRIDER_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
	    if (mem == EXT_L2_ENTRY_TCAMm) {
		validf = FREEf;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 1) {
                    continue;
                }
	    }
#endif /* BCM_TRIUMPH_SUPPORT */

	    if (soc_mem_field_length(unit, mem, VALID_BITf) > 0) {
		validf = VALID_BITf;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }

	    if (soc_mem_field_length(unit, mem, VALIDf) > 0) {
		validf = VALIDf;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }

	    if (soc_mem_field_length(unit, mem, L3_VALIDf) > 0) {
		validf = L3_VALIDf;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }

	    if (soc_mem_field_length(unit, mem, BUCKET_BITMAPf) > 0) {
		validf = BUCKET_BITMAPf;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }

#ifdef BCM_FIREBOLT_SUPPORT
	    if (soc_mem_field_length(unit, mem, VALID0f) > 0) {
		validf = VALID0f;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    if (soc_mem_field_length(unit, mem, VALID1f) > 0) {
                        validf = VALID1f;
                        if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                            continue;
                        }
                    }
                }
	    }

	    if (soc_mem_field_length(unit, mem, VALID_0f) > 0) {
		validf = VALID_0f;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }

	    if (soc_mem_field_length(unit, mem, VALID_1f) > 0) {
		validf = VALID_1f;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }

	    if (soc_mem_field_length(unit, mem, VALID_2f) > 0) {
		validf = VALID_2f;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }

	    if (soc_mem_field_length(unit, mem, VALID_3f) > 0) {
		validf = VALID_3f;
		if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
	    }
#endif /* BCM_FIREBOLT_SUPPORT */

	    if (mem == GFILTER_IMASKm || mem == FILTER_IMASKm) {
		uint32 mask_val[SOC_MAX_MEM_WORDS];
		soc_mem_field_get(unit, mem, entry, FMASKf, mask_val);
		if (sal_memcmp(mask_val,
			       soc_mem_entry_zeroes(unit, mem),
			       soc_mem_field_length(unit, mem,
						    FMASKf) / 8) == 0) {
		    continue;
		}
	    }
	}

	if (flags & DUMP_TABLE_HEX) {
	    for (i = 0; i < entry_dw; i++) {
		printk("%08x\n", entry[i]);
            }
	} else if (flags & DUMP_TABLE_CHANGED) {
	    sal_sprintf(lineprefix, "%s.%s[%d]: ",
		    SOC_MEM_UFNAME(unit, mem),
		    SOC_BLOCK_NAME(unit, copyno),
		    k);
	    soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
	} else {
	    printk("%s.%s[%d]: ",
		   SOC_MEM_UFNAME(unit, mem),
		   SOC_BLOCK_NAME(unit, copyno),
		   k);

	    if (flags & DUMP_TABLE_RAW) {
		for (i = 0; i < entry_dw; i++) {
		    printk("0x%08x ", entry[i]);
                }
	    } else {
		soc_mem_entry_dump(unit, mem, entry);
	    }

	    printk("\n");
	}
    }

    rv = CMD_OK;

 done:
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
STATIC cmd_result_t 
do_dump_sw(int unit, args_t *a) 
{ 
    char *c; 
    int  dump_all = FALSE; 
        
    if ((c = ARG_GET(a)) == NULL) { 
        return CMD_USAGE; 
    } 
     
    if (!sal_strcasecmp(c, "all")) { 
        dump_all = TRUE; 
    } 
     
    if (dump_all || !sal_strcasecmp(c, "cosq")) { 
        _bcm_cosq_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "port")) { 
        _bcm_port_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "trunk")) { 
        _bcm_trunk_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "vlan")) { 
        _bcm_vlan_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "l2")) { 
        _bcm_l2_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "mcast")) { 
        _bcm_mcast_sw_dump(unit); 
    } 
    
#ifdef INCLUDE_L3 
#if defined(BCM_FIREBOLT_SUPPORT) || defined (BCM_EASYRIDER_SUPPORT)
    if (dump_all || !sal_strcasecmp(c, "l3")) { 
        _bcm_l3_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "ipmc")) { 
        _bcm_ipmc_sw_dump(unit); 
    } 
#endif /* BCM_FIREBOLT_SUPPORT || BCM_EASYRIDER_SUPPORT */
#endif  /* INCLUDE_L3 */ 
     
    return CMD_OK; 
} 
#endif  /*  BCM_WARM_BOOT_SUPPORT_SW_DUMP */ 

int
mem_dump_iter_callback(int unit, soc_mem_t mem, void *data)
{
    uint32     index_min, count, copyno;
    int        rv = SOC_E_NONE;
    int        flags = PTR_TO_INT(data);

    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG)) {
        return rv;
    }

    index_min = soc_mem_index_min(unit, mem);
    count = soc_mem_index_count(unit, mem);

    SOC_MEM_BLOCK_ITER(unit, mem, copyno) {
        /*
         * Bypass dumping MMU memories.
         */
        if (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_MMU) {
            continue;
        }

        if ((do_dump_table(unit, mem, copyno, index_min, 
                           count, flags)) != CMD_OK) {
            rv = SOC_E_INTERNAL;
            break;
        }
    }

    return rv;
}

static cmd_result_t
do_dump_memories(int unit, args_t *a)
{
    char	*an_arg;
    int          flags = DUMP_TABLE_ALL, rv = CMD_OK;

    an_arg = ARG_GET(a);

    while (an_arg) {
        if (sal_strcasecmp(an_arg, "diff") == 0) {
            flags = DUMP_TABLE_CHANGED;
        } else {
            printk("ERROR: unrecognized argument to DUMP SOC: %s\n",
                   an_arg);
            return CMD_FAIL;
        }
        an_arg = ARG_GET(a);
    }

    if ((soc_mem_iterate(unit, 
                         mem_dump_iter_callback, INT_TO_PTR(flags))) < 0) {
        rv = CMD_FAIL;
    }

    return rv;
}


cmd_result_t
cmd_esw_dump(int unit, args_t *a)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    regtype_entry_t *rt;
    soc_mem_t mem;
    char *arg1, *arg2, *arg3;
    volatile int flags = 0;
    int copyno;
    volatile int rv = CMD_FAIL;
    parse_table_t pt;
    volatile char *fname = "";
    int append = FALSE;
#ifndef NO_SAL_APPL
    volatile int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    jmp_buf	ctrl_c;
#endif

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	goto done;
    }

    if (parse_arg_eq(a, &pt) < 0) {
	rv = CMD_USAGE;
	goto done;
    }

#ifndef NO_SAL_APPL

    console_was_on = printk_cons_is_enabled();

    if (fname[0] != 0) {
	/*
	 * Catch control-C in case if using file output option.
	 */

#ifndef NO_CTRL_C
	if (setjmp(ctrl_c)) {
	    rv = CMD_INTR;
	    goto done;
	}
#endif

	sh_push_ctrl_c(&ctrl_c);

	pushed_ctrl_c = TRUE;

	if (printk_file_is_enabled()) {
	    printk("%s: Can't dump to file while logging is enabled\n",
		   ARG_CMD(a));
	    rv = CMD_FAIL;
	    goto done;
	}

	if (printk_file_open((char *)fname, append) < 0) {
	    printk("%s: Could not start log file\n", ARG_CMD(a));
	    rv = CMD_FAIL;
	    goto done;
	}

	printk_cons_enable(FALSE);

	console_disabled = 1;
    }

#endif /* NO_SAL_APPL */

    arg1 = ARG_GET(a);

    for (;;) {
	if (arg1 != NULL && !sal_strcasecmp(arg1, "raw")) {
	    flags |= DUMP_TABLE_RAW;
	    arg1 = ARG_GET(a);
	} else if (arg1 != NULL && !sal_strcasecmp(arg1, "hex")) {
	    flags |= DUMP_TABLE_HEX;
	    arg1 = ARG_GET(a);
	} else if (arg1 != NULL && !sal_strcasecmp(arg1, "all")) {
	    flags |= DUMP_TABLE_ALL;
	    arg1 = ARG_GET(a);
	} else if (arg1 != NULL && !sal_strcasecmp(arg1, "chg")) {
	    flags |= DUMP_TABLE_CHANGED;
	    arg1 = ARG_GET(a);
	} else {
	    break;
	}
    }

    if (arg1 == NULL) {
	rv = CMD_USAGE;
	goto done;
    }

    /* See if dumping internal ARL/L2 shadow copy */

    if (!sal_strcasecmp(arg1, "sarl") || !sal_strcasecmp(arg1, "sa")) {
	if (soc->arlShadow == NULL) {
	    printk("No software ARL shadow table\n");
	    rv = CMD_FAIL;
	    goto done;
	}

	sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);

#ifdef	BCM_XGS_SWITCH_SUPPORT
	if (soc_feature(unit, soc_feature_arl_hashed)) {
	    shr_avl_traverse(soc->arlShadow, soc_l2x_entry_dump, NULL);
	}
#endif	/* BCM_XGS_SWITCH_SUPPORT */

	sal_mutex_give(soc->arlShadowMutex);

	rv = CMD_OK;
	goto done;
    }

    /* See if dumping a DV */
    if (!sal_strcasecmp(arg1, "dv")) {
        arg2 = ARG_GET(a);
	if (!arg2 || !isint(arg2)) {
	    rv = CMD_USAGE;
	    goto done;
	}
	soc_dma_dump_dv(unit, " ", (void *)parse_address(arg2));
	rv = CMD_OK;
	goto done;
    }

    /* See if dumping a packet */
    if (!sal_strcasecmp(arg1, "pkt")) {
        arg2 = ARG_GET(a);
	if (!arg2 || !isint(arg2)) {
	    rv = CMD_USAGE;
	    goto done;
	}
#ifdef	BROADCOM_DEBUG
        arg3 = ARG_GET(a);
        arg3 = arg3 ? arg3 : "0000";
	bcm_pkt_dump(unit,
		     INT_TO_PTR(parse_integer(arg2)),
		     parse_integer(arg3));
	rv = CMD_OK;
#else
	printk("cannot dump pkt in non-BROADCOM_DEBUG compilation\n");
	rv = CMD_FAIL;
#endif	/* BROADCOM_DEBUG */
	goto done;
    }

    /* See if dumping a memory table */

    if (parse_memory_name(unit, &mem, arg1, &copyno) >= 0) {
	int index, count;

        arg2 = ARG_GET(a);
        arg3 = ARG_GET(a);
	if (!SOC_MEM_IS_VALID(unit, mem)) {
	    debugk(DK_ERR, "Error: Memory %s not valid for chip %s.\n",
		   SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
	    goto done;
	}
	if (copyno == COPYNO_ALL) {
	    copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
	if (arg2) {
	    index = parse_memory_index(unit, mem, arg2);
	    count = (arg3 ? parse_integer(arg3) : 1);
	} else {
	    index = soc_mem_index_min(unit, mem);
	    if (soc_mem_is_sorted(unit, mem) &&
		!(flags & DUMP_TABLE_ALL)) {
		count = soc_mem_entries(unit, mem, copyno);
	    } else {
		count = soc_mem_index_max(unit, mem) - index + 1;
	    }
	}
	rv = do_dump_table(unit, mem, copyno, index, count, flags);
	goto done;
    }

    if (!sal_strcasecmp(arg1, "socmem")) {
        rv = do_dump_memories(unit, a);
        goto done;
    }

    /*
     * See if dumping a register type
     */
    if ((rt = regtype_lookup_name(arg1)) != NULL) {
        rv = do_dump_registers(unit, rt, a);
        goto done;
    }

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP 
    /* 
     * SW data structures dump 
     */ 
    if (!sal_strcasecmp(arg1, "sw")) { 
        rv = do_dump_sw(unit, a); 
        goto done; 
    } 
#endif 

    printk("Unknown option or memory to dump "
	   "(use 'help dump' for more info)\n");

    rv = CMD_FAIL;

 done:

#ifndef NO_SAL_APPL
    if (fname[0] != 0) {
	printk_file_close();
    }

    if (console_disabled && console_was_on) {
	printk_cons_enable(TRUE);
    }

    if (pushed_ctrl_c) {
	sh_pop_ctrl_c();
    }
#endif

    parse_arg_eq_done(&pt);
    return rv;
}
