/*
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
 * StrataSwitch MACSEC PHY control
 * MACSEC PHY initialization 
 */


#include <soc/drv.h>
#include <soc/phy/phyctrl.h>
#include <soc/debug.h>

#ifdef INCLUDE_MACSEC 
#include <bmacsec.h>
#include <phy54580.h>
#ifdef BCM_SBX_SUPPORT
#include <soc/sbx/sbx_drv.h>
#endif
#include <soc/macsecphy.h>


#define MACSEC_READ_PHY_REG(_unit, _pc, _mdio, _reg, _value)     \
            ((_pc->read)((_unit), (_mdio), (_reg), (_value)))
#define MACSEC_WRITE_PHY_REG(_unit, _pc, _mdio, _reg, _value)     \
            ((_pc->write)((_unit), (_mdio), (_reg), (_value)))


/*
 * Prototype for MACSEC Device PHY initialization
 */
typedef int (*macsec_phy_init)(phy_ctrl_t *pc, int dev_port, int dev_addr,
                               int macsec_enable);

/*
 * MACSEC PHY structure 
 */
typedef struct macsec_phy_s {
    int              phy_model; /* Phy model */
    bmacsec_core_t   dev_core;  /* Device type */
    macsec_phy_init  phy_init;  /* Initialization for the PHY */
} macsec_phy_t;


/* 
 * MACSEC PHY control structure for MACSEC related
 */
typedef struct macsec_phy_ctrl_s {
    int init_done; /* Init done */
    int dev_addr;  /* MDIO address of MACSEC (MMI) block */

    struct macsec_phy_ctrl_s *next;
} macsec_phy_ctrl_t;

/* Linked list of MACSEC Devices found */
macsec_phy_ctrl_t *macsec_phy_ctrl_list;



/* Local Protoyptes */
STATIC int _macsec_54580_init(phy_ctrl_t *pc, int dev_port, int dev_addr, 
                              int macsec_enable);
STATIC int macsecphy_54580_toplvl_reg_read(phy_ctrl_t *pc, uint8 phy_base_addr, 
                                           uint8 reg_offset, uint16 *data);
STATIC int macsecphy_54580_toplvl_reg_write(phy_ctrl_t *pc, uint8 phy_base_addr, 
                                            uint8 reg_offset, uint16 data);
STATIC macsec_phy_ctrl_t* macsecphy_lookup_macsec_phy_ctrl(int dev_addr);


/* 
 * MACSEC capable PHY table
 */
macsec_phy_t macsec_phy_tbl[] = {
    { BMACSEC_PHY_BCM54580_MODEL, BMACSEC_CORE_OCTAL_GIG, _macsec_54580_init},
    { BMACSEC_PHY_BCM54540_MODEL, BMACSEC_CORE_OCTAL_GIG, _macsec_54580_init},
    { BMACSEC_PHY_BCM54584_MODEL, BMACSEC_CORE_OCTAL_GIG, _macsec_54580_init},
    { BMACSEC_PHY_BCM54585_MODEL, BMACSEC_CORE_OCTAL_GIG, _macsec_54580_init},
};
int macsec_phy_tbl_cnt = (sizeof(macsec_phy_tbl) / sizeof(macsec_phy_t));


/*
 * 54580 Top level register Read
 */
STATIC int
macsecphy_54580_toplvl_reg_read(phy_ctrl_t *pc, uint8 phy_base_addr, 
                                uint8 reg_offset, uint16 *data)

{
    int    unit;
    uint8  port3_phy_id, port5_phy_id;
    uint16 reg_data, status;

    if (!pc) {
        return SOC_E_FAIL;
    }
    unit = pc->unit;
    port3_phy_id = phy_base_addr + 2;
    port5_phy_id = phy_base_addr + 4;
    /* port6_phy_id = phy_base_addr + 6; Used for writes only */

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Status READ from Port 3's register 0x15 */


    /* Write Reg offset to Port 5's register 0x1C, shadow 0x0B */
    reg_data = (0xAC00 | reg_offset);
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port5_phy_id, 0x1c, reg_data));

    /* Read data from Top level MII Status register(0x15h) */
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port3_phy_id, 0x17, 0x8F0B));
    SOC_IF_ERROR_RETURN
        (MACSEC_READ_PHY_REG(unit, pc, port3_phy_id, 0x15, &status));

    *data = (status & 0xff);

    return SOC_E_NONE;
}


/*
 * 54580 Top level register Write
 */
STATIC int
macsecphy_54580_toplvl_reg_write(phy_ctrl_t *pc, uint8 phy_base_addr, 
                                 uint8 reg_offset, uint16 data)

{
    int    unit;
    uint8  port3_phy_id, port6_phy_id, port5_phy_id;
    uint16 status, reg_data;

    if (!pc) {
        return SOC_E_FAIL;
    }
    unit = pc->unit;

    port3_phy_id = phy_base_addr + 2;
    port5_phy_id = phy_base_addr + 4;
    port6_phy_id = phy_base_addr + 5;

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Write data to Port 6's register 0x1C, shadow 0x0c */
    /* Status READ from Port 3's register 0x15 */

    /* Write Data to port6, register 0x1C, shadow 0x0c */
    reg_data = (0xB000 | (data & 0xff));
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port6_phy_id, 0x1c, reg_data));

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Enable Write ( Port 5's register 0x1C, shadow 0x0B) Bit 7 = 1 */
    reg_data = (0xAC80 | reg_offset);
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port5_phy_id, 0x1c, reg_data));

    /* Disable Write ( Port 5's register 0x1C, shadow 0x0B) Bit 7 = 0 */
    reg_data = (0xAC00 | reg_offset);
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port5_phy_id, 0x1c, reg_data));

#ifdef BROADCOM_DEBUG
    /* Read data from Top level MII Status register(0x15h) */
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port3_phy_id, 0x17, 0x8F0B));
    SOC_IF_ERROR_RETURN
        (MACSEC_READ_PHY_REG(unit, pc, port3_phy_id, 0x15, &status));

#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      _macsec_54580_init
 * Purpose:
 *      Initialize MACSEC Top Level configuration
 *
 * Parameters:
 *      pc       - Phy control structure
 *      dev_port - Port in the MACSEC device
 *      dev_addr - MMI address
 *
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_macsec_54580_init(phy_ctrl_t *pc, int dev_port, int dev_addr,
                   int macsec_enable)
{
    int   unit, port;
    uint8 phy_base_add, reg_offset;
    uint16 data;
 
#define MACSEC_MDIO_ENABLE                                (1U << 5)
#define MACSEC_MDIO_ADDR_MASK                             0x1F
#define MACSEC_INTERRUPT_ENABLE                           (1U << 3)
#define MACSEC_TOP_LVL_PORT_PASS_THRU_REG_OFFSET          0x2C
#define MACSEC_TOP_LVL_PORT_GLOBAL_PASS_THRU_REG_OFFSET   0x2D
#define MACSEC_TOP_LVL_MDIO_ENABLE_REG_OFFSET             0x2E
#define MACSEC_TOP_LVL_QUAD_MEDIA_DETECT_REG_OFFSET       0x0A

    if (!pc) {
        return SOC_E_FAIL;
    }
    unit = pc->unit;
    port = pc->port;

    phy_base_add = pc->phy_id - dev_port;

    if (macsec_enable) { 

        /* Enable MACSEC in Top level register 0x2D */
        reg_offset = MACSEC_TOP_LVL_PORT_GLOBAL_PASS_THRU_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        /* Write to register 0x2D to Enable MACSEC  GLOBALLY */
        if ((data & 0x1) == 0) {
            int         ii;
            uint16      aer_addr;

            data |= 1;  /* Enable MACSEC GLOBALLY */
            if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                                 reg_offset, data) < 0) {
                return SOC_E_FAIL;
            }
            if ((pc->phy_model == BMACSEC_PHY_BCM54584_MODEL) ||
                (pc->phy_model == BMACSEC_PHY_BCM54585_MODEL)) {
                /*
                 * Reset QSGMII
                 */
                for(ii = 0; ii < 8; ii++) {
                    aer_addr = phy_base_add + 8 + ii;
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + 8,
                                              0x1f, 0xffd0));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + 8,
                                              0x1e, aer_addr));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add+8,
                                              0x1f, 0x0000));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add+8,
                                              0x00, 0x9140));
                }
            }
            if ((pc->phy_model == BMACSEC_PHY_BCM54580_MODEL) ||
                (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL)) {
                /*
                 * Reset SGMII
                 */
                for(ii = 0; ii < 8; ii++) {
                    uint16 blk_sel;

                    /* Map 1000X page */ 
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                              0x1c, 0x7c00));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_READ_PHY_REG(unit, pc, phy_base_add + ii,
                                             0x1c, &blk_sel));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                              0x1c, blk_sel | 0x8001));

                    /* Write 1000X IEEE register */
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                              0x0, 0x8000));

                   /* Restore IEEE mapping */
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                              0x1c, 
                                              (blk_sel & 0xfffe) | 0x8000));
                }
            }
            if (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL) {
                reg_offset = MACSEC_TOP_LVL_QUAD_MEDIA_DETECT_REG_OFFSET;
                if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
                    return SOC_E_FAIL;
                }
                data &= ~(0x1f);
                data |= 0x10;  /* Enable MACSEC GLOBALLY */
                if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                                 reg_offset, data) < 0) {
                    return SOC_E_FAIL;
                }
            }
        }

#if 0 
        /* Enable MACSEC in Top level register 0x2c */
        reg_offset = MACSEC_TOP_LVL_PORT_PASS_THRU_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        /* Write to register 0x2C to Enable MACSEC */
        data |= (1 << dev_port);  /* Enable MACSEC on given port */
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                            reg_offset, data) < 0) {
            return SOC_E_FAIL;
        }
#endif

        /* Enable MACSEC MDIO and program MACSEC core MDIO address */
        reg_offset = MACSEC_TOP_LVL_MDIO_ENABLE_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        data &= ~(0x3f);
        data |= (MACSEC_MDIO_ENABLE | (dev_addr & MACSEC_MDIO_ADDR_MASK));
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                        reg_offset, data) < 0) {
            return SOC_E_FAIL;
        }

        /* Enable MACSEC interrupts */
        reg_offset = MACSEC_TOP_LVL_PORT_GLOBAL_PASS_THRU_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        data &= ~(0x08);
        data |= MACSEC_INTERRUPT_ENABLE;
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                        reg_offset, data) < 0) {
            return SOC_E_FAIL;
        }

    } else {
        /* Disable MACSEC in Top level register 0x2D */
        reg_offset = MACSEC_TOP_LVL_PORT_GLOBAL_PASS_THRU_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        /* Write to register 0x2D to Disable MACSEC GLOBALLY */
        data &= 0xfffe;  /* Disable MACSEC GLOBALLY (bit 0 = 0) */
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                                 reg_offset, data) < 0) {
           return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      macsecphy_lookup_macsec_phy_ctrl
 * Purpose:
 *      Lookup for macsec phy control structure which matched dev_addr
 *
 * Parameters:
 *      dev_addr - Device Address 
 * Returns:
 *      SOC_E_XXX
 */

STATIC macsec_phy_ctrl_t *
macsecphy_lookup_macsec_phy_ctrl(int dev_addr)
{
    macsec_phy_ctrl_t *macsec_phy_ctrl;

    macsec_phy_ctrl = macsec_phy_ctrl_list;

    while(macsec_phy_ctrl) {
        if (macsec_phy_ctrl->dev_addr == dev_addr) {
            return macsec_phy_ctrl;
        }
        macsec_phy_ctrl = macsec_phy_ctrl->next;
    }
    return NULL;
}


/*
 * Function:
 *      soc_macsecphy_init
 * Purpose:
 *      Initializes the MACSEC phy port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      pc  - Phy control structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_macsecphy_init(int unit, soc_port_t port, phy_ctrl_t *pc)
{
    macsec_phy_t            *macsec_phy = NULL;
    int                     ii, p, rv;
    macsec_phy_ctrl_t       *macsec_phy_ctrl;
    uint32                  macsec_dev_addr;
    uint32                  dev_port;
    soc_macsec_dev_addr_t   dev_addr;
    int                     macsec_enable = 0;
    bmacsec_port_config_t   cfg;

    
    /* lookup this table to check if this is macsec capable */
    for(ii = 0; ii < macsec_phy_tbl_cnt; ii++) {
        if (macsec_phy_tbl[ii].phy_model == pc->phy_model) {
            macsec_phy = &macsec_phy_tbl[ii];
            break;
        }
    }
    if (ii >= macsec_phy_tbl_cnt) {
        return SOC_E_NONE;
    }

    /* Find my dev_port, MMI address from spn_XXX peroperty */
    dev_port = soc_property_port_get(unit, port, 
                                     spn_MACSEC_DEVICE_PORT, -1);
    macsec_dev_addr = soc_property_port_get(unit, port, 
                                        spn_MACSEC_DEVICE_ADDRESS, -1);
    if (dev_port == -1) {
        soc_cm_debug(DK_WARN, "soc_macsecphy_init: "
                     "MACSEC_DEVICE_PORT property not configured for "
                     "u=%d p=%d\n", unit, port);
        return SOC_E_CONFIG;
    }
    if (macsec_dev_addr == -1) {
        soc_cm_debug(DK_WARN, "soc_macsecphy_init: "
                        "MACSEC_DEVICE_ADDRESS property "
                        "not configured for u=%d p=%d\n", unit, port);
        return SOC_E_CONFIG;
    }
    pc->macsec_dev_port = dev_port;
    pc->macsec_dev_addr = macsec_dev_addr;  /* Change macsec_dev_addr to
                                           macsec_mdio in phy_ctrl_t */
   
    /* find macsec_phy_ctrl_t corresponding to this MMI address */
    dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->macsec_dev_addr, 0, 0);
    macsec_phy_ctrl = macsecphy_lookup_macsec_phy_ctrl(dev_addr);
    if (macsec_phy_ctrl == NULL) {
        macsec_phy_ctrl = sal_alloc (sizeof (macsec_phy_ctrl_t), 
                                         "MACSEC PHY control");
        if (macsec_phy_ctrl == NULL) {
            return SOC_E_RESOURCE;
        }
        sal_memset(macsec_phy_ctrl, 0, sizeof(macsec_phy_ctrl_t));

        macsec_phy_ctrl->dev_addr = dev_addr;
        macsec_phy_ctrl->init_done = 0;
        /* Add to the list */
        if (macsec_phy_ctrl_list) {
            macsec_phy_ctrl->next = macsec_phy_ctrl_list;
        }
        macsec_phy_ctrl_list = macsec_phy_ctrl;
    }

    /* By default MACSEC is disable */
    /* Check if enable property is set */
    macsec_enable = soc_property_port_get(unit, port, spn_MACSEC_ENABLE, 0);

    /* Need to PASS MACSEC MDIO address, as MDIO gets programmed */
    rv = macsec_phy->phy_init(pc, pc->macsec_dev_port, pc->macsec_dev_addr,
                              macsec_enable);
                      
    if (!SOC_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }

    if (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL) {
        pc->macsec_dev_port = dev_port + 4;
    }

    p = SOC_MACSEC_PORTID(unit, port);
    /* Destroy the port if it were created before. */
    (void)bmacsec_port_destroy(p);

    pc->macsec_enable = macsec_enable;
    if (macsec_enable) {
        dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->macsec_dev_addr, 0, 0);

        /* Create MACSEC Port */
        rv = bmacsec_port_create(p, macsec_phy->dev_core, dev_addr, 
                             pc->macsec_dev_port, bmacsec_io_mmi);
        if (rv != BMACSEC_E_NONE) {
            soc_cm_debug(DK_WARN, "soc_macsecphy_init: "
                        "MACSEC port create failed for u=%d p=%d rv = %d\n",
                        unit, port, rv);
            return SOC_E_FAIL;
        }

        /* Set Port in Store and Forward Mode */
        rv = bmacsec_port_config_get(p, &cfg);
        if (rv != BMACSEC_E_NONE) {
            soc_cm_debug(DK_WARN, "soc_macsecphy_init: "
                        "MACSEC port config get failed u=%d p=%d rv = %d\n",
                         unit, port, rv);
            return SOC_E_FAIL;
        }

        cfg.flags |= BMACSEC_PORT_INGRESS_STORE_FORWARD;
        rv = bmacsec_port_config_set(p, &cfg);
        if (rv != BMACSEC_E_NONE) {
            soc_cm_debug(DK_WARN, "soc_macsecphy_init: "
                        "MACSEC port config set failed u=%d p=%d rv = %d\n",
                         unit, port, rv);
            return SOC_E_FAIL;
        }
        
        
    }
    return SOC_E_NONE;
}

int 
soc_macsecphy_miim_read(soc_macsec_dev_addr_t dev_addr, 
                        uint32 phy_reg_addr, uint16 *data)
{
    int unit, phy_id, clause45;
    int rv = SOC_E_NONE;

    /*
     * Decode dev_addr into phyid and unit.
     */
    unit = SOC_MACSECPHY_ADDR2UNIT(dev_addr);
    phy_id = SOC_MACSECPHY_ADDR2MDIO(dev_addr);
    clause45 = SOC_MACSECPHY_ADDR_IS_CLAUSE45(dev_addr);

#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_ESW(unit)) {
        if (clause45) {
            rv = soc_esw_miimc45_read(unit, phy_id, 
                                  phy_reg_addr, data);
        } else {
            rv = soc_miim_read(unit, phy_id, phy_reg_addr, data);
        }
    }
#endif /* BCM_ESW_SUPPORT */

#ifdef BCM_SBX_SUPPORT
    if (SOC_IS_SBX(unit)) {
        if (clause45) {
            rv = soc_sbx_miimc45_read(unit, phy_id, 
                                      phy_reg_addr, data);
        } else {
            rv = soc_sbx_miim_read(unit, phy_id, phy_reg_addr, data);
        }
    }
#endif /* BCM_SBX_SUPPORT */

#ifdef BCM_ROBO_SUPPORT
    if (SOC_IS_ROBO(unit)) {
        rv = soc_robo_miim_read(unit, phy_id, phy_reg_addr, data);
    }
#endif /* BCM_ROBO_SUPPORT */

    return (rv == SOC_E_NONE) ? 0 : -1;
}

int 
soc_macsecphy_miim_write(soc_macsec_dev_addr_t dev_addr, 
                         uint32 phy_reg_addr, uint16 data)
{
    int unit, phy_id, clause45;
    int rv = SOC_E_NONE;

    /*
     * Decode dev_addr into phyid and unit.
     */
    unit = SOC_MACSECPHY_ADDR2UNIT(dev_addr);
    phy_id = SOC_MACSECPHY_ADDR2MDIO(dev_addr);

    clause45 = SOC_MACSECPHY_ADDR_IS_CLAUSE45(dev_addr);

#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_ESW(unit)) {
        if (clause45) {
            rv = soc_esw_miimc45_write(unit, phy_id, 
                                   phy_reg_addr, data);
        } else {
            rv = soc_miim_write(unit, phy_id, phy_reg_addr, data);
        }
    }
#endif /* BCM_ESW_SUPPORT */

#ifdef BCM_SBX_SUPPORT
    if (SOC_IS_SBX(unit)) {
        if (clause45) {
            rv = soc_sbx_miimc45_write(unit, phy_id, 
                                      phy_reg_addr, data);
        } else {
            rv = soc_sbx_miim_write(unit, phy_id, phy_reg_addr, data);
        }
    }
#endif /* BCM_SBX_SUPPORT */

#ifdef BCM_ROBO_SUPPORT
    if (SOC_IS_ROBO(unit)) {
        rv = soc_robo_miim_write(unit, phy_id, phy_reg_addr, data);
    }
#endif /* BCM_ROBO_SUPPORT */

    return (rv == SOC_E_NONE) ? 0 : -1;
}

#else

int 
soc_macsecphy_init(int unit, soc_port_t port, void *pc)
{
    return SOC_E_NONE;
}
#endif /* INCLUDE_MACSEC */

