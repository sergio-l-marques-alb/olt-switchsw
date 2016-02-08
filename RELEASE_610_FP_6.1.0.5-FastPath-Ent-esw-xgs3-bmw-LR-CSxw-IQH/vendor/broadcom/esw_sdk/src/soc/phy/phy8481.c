
/*
 * $Id: phy8481.c,v 1.1 2011/04/18 17:11:07 mruas Exp $
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
 * File:        phy8481.c
 * Purpose:    Phys Driver support for Broadcom 8481 Serial 10Gig
 *             transceiver with XAUI interface.
 */

#include <sal/types.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phydefs.h"      /* Must include before other phy related includes */

#if defined(INCLUDE_PHY_8481)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyxehg.h"
#include "phy8481.h"

/*
#define PHY8481_ARM_MEM_DEBUG
#define PHY8481_DEBUG_AUTOMEDIUM
#define PHY8481_FIX_XAUI2_PRTAD
#define PHY8481_FW_DEBUG
 */

#define SOC_PORT_MEDIUM_XAUI      SOC_PORT_MEDIUM_FIBER
#define PHY_FLAGS_XAUI            PHY_FLAGS_FIBER
#define PHY_XAUI_MODE(unit, port) PHY_FIBER_MODE((unit), (port))
#define SOC_PA_MEDIUM_XAUI        SOC_PA_MEDIUM_FIBER

#define PHY_ADDR_NONE  0xFF
#define NXT_PC(pc) ((phy_ctrl_t *)((pc)->driver_data))
#define PHYDRV_CALL_NOARG(pc,name) \
     do { \
        if (pc->driver_data) { \
	    int rv; \
	    phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
	    EXT_PHY_SW_STATE(pc->unit,pc->port) = (phy_ctrl_t *)(pc->driver_data); \
            rv = name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port); \
	    EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
	    if (SOC_FAILURE(rv)) { \
	        return rv; \
	    } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG1(pc,name,arg0) \
    do { \
       if (pc->driver_data) { \
	   int rv; \
	   phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
	   EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv = name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit, \
		(pc)->port,arg0); \
	   EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
	   if (SOC_FAILURE(rv)) { \
	       return rv; \
	   } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG2(pc,name,arg0,arg1) \
    do { \
       if (pc->driver_data) { \
	   int rv; \
	   phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
	   EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv=name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port,arg0, \
			arg1); \
	   EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
	   if (SOC_FAILURE(rv)) { \
	       return rv; \
	   } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG3(pc,name,arg0,arg1,arg2) \
    do { \
       if (pc->driver_data) { \
           int rv; \
           phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv=name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port,arg0, \
                        arg1,arg2); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
           if (SOC_FAILURE(rv)) { \
               return rv; \
           } \
       } \
    } while(0)

/* default MDIO addresses of phy device on the xaui port, per port based*/
static uint8 ext_phy_dft_addr1[] = {
    PHY_ADDR_NONE,  /* 0  */
    PHY_ADDR_NONE,  /* 1  */
    PHY_ADDR_NONE,  /* 2  */
    PHY_ADDR_NONE,  /* 3  */
    PHY_ADDR_NONE,  /* 4  */
    PHY_ADDR_NONE,  /* 5  */
    PHY_ADDR_NONE,  /* 6  */
    PHY_ADDR_NONE,  /* 7  */
    PHY_ADDR_NONE,  /* 8  */
    PHY_ADDR_NONE,  /* 9  */
    PHY_ADDR_NONE,  /* 10  */
    PHY_ADDR_NONE,  /* 11  */
    PHY_ADDR_NONE,  /* 12  */
    PHY_ADDR_NONE,  /* 13  */
    PHY_ADDR_NONE,  /* 14  */
    PHY_ADDR_NONE,  /* 15  */
    PHY_ADDR_NONE,  /* 16  */
    PHY_ADDR_NONE,  /* 17  */
    PHY_ADDR_NONE,  /* 18  */
    PHY_ADDR_NONE,  /* 19  */
    PHY_ADDR_NONE,  /* 20  */
    0x56,           /* 21  */
    0x57,           /* 22  */
    0x58,           /* 23  */
    0x59,           /* 24  */
    PHY_ADDR_NONE,  /* 25  */
    PHY_ADDR_NONE,  /* 26  */
    PHY_ADDR_NONE,  /* 27  */
    PHY_ADDR_NONE,  /* 28  */
    PHY_ADDR_NONE,  /* 29  */
    PHY_ADDR_NONE,  /* 30  */
    PHY_ADDR_NONE,  /* 31  */
    PHY_ADDR_NONE,  /* 32  */
    PHY_ADDR_NONE,  /* 33  */
    PHY_ADDR_NONE,  /* 34  */
    PHY_ADDR_NONE,  /* 35  */
    PHY_ADDR_NONE,  /* 36  */
    PHY_ADDR_NONE   /* 37  */
};

STATIC int phy_8481_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_8481_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_8481_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_8481_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int phy_8481_an_set(int unit, soc_port_t port, int an);
STATIC int phy_8481_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_8481_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_8481_link_up(int unit, soc_port_t port);

STATIC int _phy_8481_xaui_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int _phy_8481_xaui_an_set(int unit, soc_port_t port, int an);
STATIC int _phy_8481_phyxs_l_pll_lock_wait(int unit, soc_port_t port);
STATIC int _phy_8481_medium_change(int unit, soc_port_t port, int force_update);
STATIC int _phy_8481_medium_config_update(int unit, soc_port_t port,soc_phy_config_t *cfg);
STATIC int _phy_8481_xaui_speed_get(int unit, soc_port_t port, int *speed);
STATIC int _phy_8481_xaui_speed_set(int unit, soc_port_t port, int speed);
STATIC int _phy_8481_medium_check(int unit, soc_port_t port, int *medium);
STATIC int _phy_8481_xaui_enable_set(int unit, soc_port_t port, int enable);
STATIC int _phy_8481_copper_enable_set(int unit, soc_port_t port, int enable);

STATIC int _phy_8481_xaui_nxt_dev_probe(int unit, soc_port_t port);
int _phy_8481_get_fw_stat_reg(int unit, soc_port_t port, uint32 *data32);

/* Function:
 *    phy_8481_init
 * Purpose:    
 *    Initialize 8481 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8481_init(int unit, soc_port_t port)
{
    soc_timeout_t  to;
    phy_ctrl_t     *pc;
    uint16         data16;
    soc_port_ability_t  ability;
    soc_phy_info_t *pi;
    int            rv;
    int medium;

    pc = EXT_PHY_SW_STATE(unit, port);
    pi = &SOC_PHY_INFO(unit, port);
    
    PHY_FLAGS_SET(unit, port,  PHY_FLAGS_COPPER | PHY_FLAGS_XAUI | PHY_FLAGS_C45);

    /* Reset the device */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_CTRLr(unit, pc,
                                      MII_CTRL_RESET, MII_CTRL_RESET));
    /* Wait for device to come out of reset */
    soc_timeout_init(&to, 2000000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_PHY8481_PMAD_CTRLr(unit, pc, &data16);
        if (((data16 & MII_CTRL_RESET) == 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((data16 & MII_CTRL_RESET) != 0) {
        SOC_DEBUG_PRINT((DK_WARN,
                         "PHY8481 reset failed: u=%d p=%d\n",
                         unit, port));
    }

#ifdef PHY8481_FIX_XAUI2_PRTAD

    /* Change XAUI1 md_devad=3,multimmds_strap=1,other fields unchanged. */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_XAUI1_MDIO_CTL_REG_MD_DEVAD(unit, pc, 3));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_XAUI1_MDIO_CTL_REG_MULTIMMDS_STRAP(unit, pc, 1));

    /* Change XAUI2 prtad=phy_addr. */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_XAUI1_MDIO_CTL_REG(unit, pc, &data16));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_XAUI2_MDIO_CTL_REG_PRTAD(unit, pc,
            (data16&XAUI2_MDIO_CTL_PRTAD_MASK)>>XAUI2_MDIO_CTL_PRTAD_SHIFT));

    /* Block XAUI1 access. */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_XGP_PD_DEF_REG_02(unit, pc, 0));

    /* Set XAUI2 access, change prtad in XGP_PD_DEF_REG_06 to phy_addr(strap) */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_XAUI1_MDIO_CTL_REG(unit, pc, &data16));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_XGP_PD_DEF_REG_06_PRTAD(unit, pc,
            (data16&XAUI1_MDIO_CTL_PRTAD_MASK)>>XAUI1_MDIO_CTL_PRTAD_SHIFT));

#endif

    /* Switch the register bank to copper */
    PHY8481_SELECT_REGS_CU(unit, pc);

    /* Fix the PHY_ID as the device can return values in the PMA/PMD ID registers */

    if (READ_PHY8481_TOPLVL1_ID0r(unit, pc, &pi->phy_id0) < 0) {
        return SOC_E_NOT_FOUND;
    }

    if (READ_PHY8481_TOPLVL1_ID1r(unit, pc, &pi->phy_id1) < 0) {
        return SOC_E_NOT_FOUND;
    }

    pc->automedium = soc_property_port_get(unit, port, spn_PHY_AUTOMEDIUM, 1);

    /* software controlled medium selection is not available yet */
    pc->copper.enable         = TRUE;
    pc->copper.force_speed    = 10000;
    pc->fiber.enable          = TRUE;
    pc->fiber.force_speed     = 10000;
    pc->fiber.force_duplex    = TRUE;

    SOC_IF_ERROR_RETURN
        (_phy_8481_medium_check(unit, port, &medium));

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    } else {        /* Fiber */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
    }

    /* check if there is another PHY device connected to the XAUI port */
    rv = _phy_8481_xaui_nxt_dev_probe(unit,port);
    if (SOC_SUCCESS(rv)) {
        PHYDRV_CALL_NOARG(pc,PHY_INIT);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        /* disable the fiber if in copper mode*/
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_enable_set(unit, port, FALSE));
        PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,FALSE);
    } else {
        /* disable the copper if in fiber mode */
        SOC_IF_ERROR_RETURN(_phy_8481_copper_enable_set(unit, port, FALSE));
    }

    /* Set Local Advertising Configuration */
    SOC_IF_ERROR_RETURN
        (phy_8481_ability_local_get(unit, port, &ability));
    SOC_IF_ERROR_RETURN
        (phy_8481_ability_advert_set(unit, port, &ability));

#ifdef PHY8481_FW_DEBUG
    {
        uint32 fw_stat_reg;

        SOC_IF_ERROR_RETURN
            (_phy_8481_get_fw_stat_reg(unit, port, &fw_stat_reg));

        soc_cm_debug(DK_PHY, "8481: u=%d port%d: init. SPI FW STAT REG = %08x "
                             "Version = %02d.%02d Date = %02d/%02d/20%02d\n",
            unit, port, fw_stat_reg, (fw_stat_reg>>7)&0x1f, (fw_stat_reg>>0)&0x7f,
            (fw_stat_reg>>23)&0xf, (fw_stat_reg>>18)&0x1f, (fw_stat_reg>>14)&0xf);
    }
#endif

    return SOC_E_NONE;
}

STATIC int
_phy_8481_copper_link_get(int unit, soc_port_t port, int *link) {

    uint16      mii_stat, pma_mii_stat, pcs_mii_stat, speed_val, status, ctrl;
    phy_ctrl_t     *pc;
    int an,an_done,cur_speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (phy_8481_an_get(unit,port,&an,&an_done));

    /* return link false if in the middle of autoneg */
    if (an == TRUE && an_done == FALSE) {
        *link = FALSE;
        return SOC_E_NONE;
    } 

   /* check if the current speed is set to 10G */
    cur_speed = 0;
    if (an) { /* autoneg is enabled */
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TENG_AN_CTRLr(unit, pc, &ctrl));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TENG_AN_STATr(unit, pc, &status));
            if ((ctrl & TENG_IEEE_AN_CTRL_10GBT)&&(status & TENG_IEEE_AN_STAT_LP_10GBT)) {
                cur_speed = 10000;
            }
    } else { /* forced mode */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_CTRLr(unit, pc, &speed_val));
        if ((speed_val & IEEE_CTRLr_SPEED_10G_MASK)==IEEE_CTRLr_SPEED_10G) {
            cur_speed = 10000;
        }
    }

    if (cur_speed == 10000) { /* check all 3 device's link status if 10G */

        /* System side XAUI link status , DEVAD=4 */
        /* SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_M_STATr(unit, pc, &pxs_m_mii_stat)); */

        /* PCS link status , DEVAD=3 */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PCS_STATr(unit, pc, &pcs_mii_stat));

        /* Line side link status , DEVAD=1 */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_STATr(unit, pc, &pma_mii_stat));

        *link = (pma_mii_stat & pcs_mii_stat & MII_STAT_LA) ? TRUE : FALSE;
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_STATr(unit, pc, &mii_stat));

        *link = (mii_stat & MII_STAT_LA) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_link_get(int unit, soc_port_t port, int *link) {

    phy_ctrl_t *pc;
    uint16  xgxs_stat1;
    uint16  xgxs_stat3;
    int     an_done;
    int     an;
    int     speed;
    int     speed_local;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ((!PHY_COPPER_MODE(unit, port)) && NXT_PC(pc)) {
        /* there is a PHY device connected to this XAUI port */
        /* When it is in AN mode, its negotiated speed needs to be
         * propagated to this device. Sync up here since this function is periodically
         * called by a dedicated linkscan task.
         */
        PHYDRV_CALL_ARG1(pc,PHY_LINK_GET,link);

        if (*link == TRUE) {
            /* check if in an mode */
            PHYDRV_CALL_ARG2(pc,PHY_AUTO_NEGOTIATE_GET,&an,&an_done);

            if (an == TRUE && an_done == TRUE) {
                /* get the speed */
                PHYDRV_CALL_ARG1(pc,PHY_SPEED_GET,&speed);
                SOC_IF_ERROR_RETURN
                    (_phy_8481_xaui_speed_get(unit, port, &speed_local));

                if (speed && (speed != speed_local)) {
                    /* set this device's speed accordingly */
                    SOC_IF_ERROR_RETURN
                        (_phy_8481_xaui_speed_set(unit,port,speed));
                }
            }
        }
        return SOC_E_NONE;
    }

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    /* Link status on both SERDES_MII_STAT and XAUI_MII_STAT registers
     * are latched low.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_GP_STATUS_XGXSSTATUS3r(unit, pc, &xgxs_stat3));

    *link =  (xgxs_stat3 & GP_STATUS_XGXSSTATUS3_LINK_MASK) ? 1 : 0;

    if (pc->fiber.autoneg_enable) {
        /* If autoneg is enabled, check for autoneg done. */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_GP_STATUS_XGXSSTATUS1r(unit, pc, &xgxs_stat1));

        an_done = ((xgxs_stat1 & GP_STATUS_XGXSSTATUS1_AUTONEG_COMPLETE_MASK)
                  != 0);
        *link = (*link) && (an_done);
    }

    return (SOC_E_NONE);
}


static int
_phy8481_auto_negotiate_gcd(int unit, soc_port_t port, int *speed, int *duplex)
{
    int        t_speed, t_duplex;
    uint16     mii_ana, mii_anp, mii_stat;
    uint16     mii_gb_stat, mii_esr, mii_gb_ctrl;
    phy_ctrl_t *pc;

    mii_gb_stat = 0;            /* Start off 0 */
    mii_gb_ctrl = 0;            /* Start off 0 */

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_ANAr(unit, pc, &mii_ana));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_ANPr(unit, pc, &mii_anp));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_STATr(unit, pc, &mii_stat));

    if (mii_stat & MII_STAT_ES) {    /* Supports extended status */
        /*
         * If the PHY supports extended status, check if it is 1000MB
         * capable.  If it is, check the 1000Base status register to see
         * if 1000MB negotiated.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_ESRr(unit, pc, &mii_esr));

        if (mii_esr & (MII_ESR_1000_X_FD | MII_ESR_1000_X_HD | 
                       MII_ESR_1000_T_FD | MII_ESR_1000_T_HD)) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_MII_GB_STATr(unit, pc, &mii_gb_stat));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));
        }
    }

    /*
     * At this point, if we did not see Gig status, one of mii_gb_stat or 
     * mii_gb_ctrl will be 0. This will cause the first 2 cases below to 
     * fail and fall into the default 10/100 cases.
     */

    mii_ana &= mii_anp;

    if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD) &&
        (mii_gb_stat & MII_GB_STAT_LP_1000FD)) {
        t_speed  = 1000;
        t_duplex = 1;
    } else if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD) &&
               (mii_gb_stat & MII_GB_STAT_LP_1000HD)) {
        t_speed  = 1000;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_100) {         /* [a] */
        t_speed = 100;
        t_duplex = 1;
    } else if (mii_ana & MII_ANA_T4) {            /* [b] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_HD_100) {        /* [c] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_10) {        /* [d] */
        t_speed = 10;
        t_duplex = 1 ;
    } else if (mii_ana & MII_ANA_HD_10) {        /* [e] */
        t_speed = 10;
        t_duplex = 0;
    } else {
        return(SOC_E_FAIL);
    }

    if (speed)  {
        *speed  = t_speed;
    }
    if (duplex) {
       *duplex = t_duplex;
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *    phy_8481_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8481_link_get(int unit, soc_port_t port, int *link)
{

    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    if (pc->automedium) {
       SOC_IF_ERROR_RETURN
           (_phy_8481_medium_change(unit, port,FALSE));
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_link_get(unit, port, link));
    } else {
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_link_get(unit, port, link));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8481_enable_set
 * Purpose:
 *    Enable/Disable phy 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_8481_copper_enable_set(int unit, soc_port_t port, int enable)
{
    uint16      data;       /* Holder for new value to write to PHY reg */
    uint16      mask;       /* Holder for bit mask to update in PHY reg */
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    data = 0;
    mask = 1U << 0; /* Global PMD transmit disable */
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        data = 1U << 0;  /* Global PMD transmit disable */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_TX_DISABLEr(unit, pc, data, mask));

    return (SOC_E_NONE);
}

STATIC int
_phy_8481_xaui_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    /* disable the Tx */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_XGXSBLK0_MISCCONTROL1r(unit, pc, 
               enable? 0:XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK,
               XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK));

    /* disable the Rx */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_RX_ALL_RX_CONTROLr(unit, pc,
               enable? 0:DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK,
               DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK));

    return (SOC_E_NONE);
}

STATIC int
phy_8481_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_enable_set(unit, port, enable));
    } else {
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_enable_set(unit, port, enable));
        PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,enable);
    }

    return (SOC_E_NONE);
}


/*
 * Function:
 *    phy_8481_lb_set
 * Purpose:
 *    Put 8481 in PHY PCS/PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_8481_copper_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 
    int speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (phy_8481_speed_get(unit, port, &speed));

    if (speed == 10000) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PMAD_CTRLr(unit, pc,
            enable ? PMAD_IEEE_CTL1_LPBK:0, PMAD_IEEE_CTL1_LPBK));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_CTRLr(unit, pc, (enable) ? MII_CTRL_LE:0, MII_CTRL_LE));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_CTRLr(unit, pc, (enable) ? MII_CTRL_LE : 0, MII_CTRL_LE));

    /* Configure Loopback in SerDes */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc,
                                             (enable) ? MII_CTRL_LE : 0,
                                             MII_CTRL_LE));
    return SOC_E_NONE;
}

STATIC int
phy_8481_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_lb_set(unit, port, enable));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_LOOPBACK_SET,enable);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_lb_set(unit, port, enable));
    }

    if (enable) {
        SOC_IF_ERROR_RETURN
            (phy_8481_link_up(unit, port));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *    phy_8481_lb_get
 * Purpose:
 *    Get 8481 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_8481_copper_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc;
    uint16      tmp;
    int speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (phy_8481_speed_get(unit, port, &speed));

    if (speed == 10000) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_CTRLr(unit, pc, &tmp));
        *enable = (tmp & PMAD_IEEE_CTL1_LPBK) ? TRUE : FALSE;
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_CTRLr(unit, pc, &tmp));
        *enable = (tmp & MII_CTRL_LE) ? TRUE : FALSE;
    }

    soc_cm_debug(DK_PHY,
         "phy_8481_lb_get: u=%d port%d: loopback:%s\n",
         unit, port, *enable ? "Enabled": "Disabled");


    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      mii_ctrl;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
    *enable = ((mii_ctrl & MII_CTRL_LE) == MII_CTRL_LE);

    return (SOC_E_NONE);
}


STATIC int
phy_8481_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_lb_get(unit, port, enable));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_LOOPBACK_GET,enable);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_lb_get(unit, port, enable));
    }

    return SOC_E_NONE;
}

#ifdef PHY8481_DEBUG_AUTOMEDIUM
int
show_phy_8481_clkrst_reg(int unit, soc_port_t port)
{
    phy_ctrl_t   *pc;
    uint16    status=0;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_CLKRST_SCRATCH_REG(unit, pc, &status));

    soc_cm_print("CLKRST_SCRATCH_REG = %x\n", status);

    return SOC_E_NONE;
}
#endif

STATIC int
_phy_8481_medium_check(int unit, soc_port_t port, int *medium)
{
    phy_ctrl_t   *pc;
    uint16    status;

    pc = EXT_PHY_SW_STATE(unit, port);

    *medium = SOC_PORT_MEDIUM_COPPER;

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_CLKRST_SCRATCH_REG(unit, pc, &status));

    if ( status & CLKRST_SCRATCH_REG_FIBER_PRIO ) {

        if ( status & CLKRST_SCRATCH_REG_FIBER_LINK ) {
            *medium = SOC_PORT_MEDIUM_FIBER;
        } else {
            if ( status & CLKRST_SCRATCH_REG_COPPER_LINK ) {
                *medium = SOC_PORT_MEDIUM_COPPER;
            }
        }

    } else {

        if ( status & CLKRST_SCRATCH_REG_COPPER_LINK ) {
            *medium = SOC_PORT_MEDIUM_COPPER;
        } else {
            if ( status & CLKRST_SCRATCH_REG_FIBER_LINK ) {
                *medium = SOC_PORT_MEDIUM_FIBER;
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_medium_change(int unit, soc_port_t port, int force_update)
{
    phy_ctrl_t    *pc;
    int            medium;

    pc    = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_8481_medium_check(unit, port, &medium));

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        if ((!PHY_COPPER_MODE(unit, port)) || force_update) { /* Was fiber */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);

            /* disable the fiber medium */
            SOC_IF_ERROR_RETURN(_phy_8481_xaui_enable_set(unit, port, FALSE));
            PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,FALSE);

            /* Do not power up the interface if medium is disabled. */
            if (pc->copper.enable) {
                /* enable the copper medium */
                SOC_IF_ERROR_RETURN(_phy_8481_copper_enable_set(unit, port, TRUE));

                SOC_IF_ERROR_RETURN
                    (_phy_8481_medium_config_update(unit, port, &pc->copper));
            }
            /* soc_cm_print("COPPER selected\n"); */
            SOC_DEBUG_PRINT((DK_PHY,
                         "_phy_8481_link_auto_detect: u=%d p=%d [F->X]\n",
                          unit, port));
        }
    } else {        /* Fiber */
        if (PHY_COPPER_MODE(unit, port) || force_update) { /* Was copper */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);

            /* disable the copper medium */
            SOC_IF_ERROR_RETURN(_phy_8481_copper_enable_set(unit, port, FALSE));

            if (pc->fiber.enable) {
                /* enable the fiber medium */
                SOC_IF_ERROR_RETURN(_phy_8481_xaui_enable_set(unit, port, TRUE));
                PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,TRUE);

                SOC_IF_ERROR_RETURN
                    (_phy_8481_medium_config_update(unit, port, &pc->fiber));
            }
            /* soc_cm_print("FIBER selected\n"); */
            SOC_DEBUG_PRINT((DK_PHY,
                          "_phy_8481_link_auto_detect: u=%d p=%d [C->X]\n",
                          unit, port));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_XAUI
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_8481_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    if (PHY_COPPER_MODE(unit, port)) {
        *medium = SOC_PORT_MEDIUM_COPPER;
    } else {
        *medium = SOC_PORT_MEDIUM_XAUI;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_8481_medium_config_update
 * Purpose:
 *      Update the PHY with config parameters
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      cfg - Config structure.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_8481_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{

    SOC_IF_ERROR_RETURN
        (phy_8481_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_8481_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_8481_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_8481_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_medium_config_set
 * Purpose:
 *      Set the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/XAUI
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8481_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Changes take effect immediately if the target medium is active.
     */
    switch (medium) {

    case SOC_PORT_MEDIUM_COPPER:
        /* Change the copper modes */
        sal_memcpy(&pc->copper, cfg, sizeof (pc->copper));

        /* pc->copper.autoneg_advert &= ADVERT_ALL_COPPER; */
        if (PHY_COPPER_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (_phy_8481_medium_config_update(unit, port, &pc->copper));
        }
        return SOC_E_NONE;

    case SOC_PORT_MEDIUM_FIBER:
        /* Change the fiber modes */
        sal_memcpy(&pc->fiber, cfg, sizeof (pc->fiber));

        /* pc->fiber.autoneg_advert &= ADVERT_ALL_FIBER; */
        if (!PHY_COPPER_MODE(unit, port)) {
            if (NXT_PC(pc)) {
                SOC_IF_ERROR_RETURN
                    (_phy_8481_xaui_speed_set(unit, port, pc->fiber.force_speed));
            } else {
                SOC_IF_ERROR_RETURN
                    (_phy_8481_medium_config_update(unit, port, &pc->fiber));
            }
        }
        return SOC_E_NONE;

    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_medium_config_get
 * Purpose:
 *      Get the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/XAUI
 *      cfg - (OUT) Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8481_medium_config_get(int unit, soc_port_t port, 
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    int            rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (pc->automedium || PHY_COPPER_MODE(unit, port)) {
            sal_memcpy(cfg, &pc->copper, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (pc->automedium || (!PHY_COPPER_MODE(unit, port))) {
            sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    default:
        rv = SOC_E_PARAM;
    }

    return rv;
}


int
_phy_8481_copper_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t  *pc;    /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         break;

    default:
         /* should never get here */
         return SOC_E_PARAM;
    }



    return SOC_E_NONE;
}

int
_phy_8481_copper_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         break;

    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int
_phy_8481_xaui_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t  *pc;    /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         break;

    default:
         /* should never get here */
         return SOC_E_PARAM;
    }



    return SOC_E_NONE;
}

int
_phy_8481_xaui_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         break;

    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_8481_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        return(_phy_8481_copper_control_tx_driver_set(unit, port, type, value));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG2(pc,PHY_CONTROL_SET,type,value);
            return SOC_E_NONE;
        }
        return(_phy_8481_xaui_control_tx_driver_set(unit, port, type, value));
    }

}
/*
 * Function:
 *      phy_8481_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_8481_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        return(_phy_8481_copper_control_tx_driver_get(unit, port, type, value));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG2(pc,PHY_CONTROL_GET,type,value);
            return SOC_E_NONE;
        }
        return(_phy_8481_xaui_control_tx_driver_get(unit, port, type, value));
    }

}

/*
 * Function:
 *      phy_8481_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.  
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */
                                                                                      

STATIC int
phy_8481_probe(int unit, phy_ctrl_t *pc)
{
    uint16 id0, id1;
    soc_phy_info_t *pi;

    /* Switch the register bank to copper */
    PHY8481_SELECT_REGS_CU(unit, pc);
    if (READ_PHY8481_TOPLVL1_ID0r(unit, pc, &id0) < 0) {
        return SOC_E_NOT_FOUND;
    }

    if (READ_PHY8481_TOPLVL1_ID1r(unit, pc, &id1) < 0) {
        return SOC_E_NOT_FOUND;
    }

    if ((id0 != 0x0362) || ((id1&0xfff0) != 0x5c00)) {
        return SOC_E_NOT_FOUND;
    }

    /* temporary hack until the phy idents of 8481 are fixed */
    pi = &SOC_PHY_INFO(unit, pc->port);

    pi->phy_name = "BCM8481";

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_8481_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
_phy_8481_copper_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;
    uint16 speed_c45, speed_c22;
    int rv = SOC_E_NONE;

    soc_cm_debug(DK_PHY,"phy_8481_speed_set: u=%d p=%d speed=%d\n",
                 unit, port,speed);

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    switch (speed) {
        case 10000:
            speed_c45 = IEEE_CTRLr_SPEED_10G;
            speed_c22 = MII_CTRL_SS_MSB | MII_CTRL_SS_LSB;
            break;

        case 1000:
            speed_c45 = 0;
            speed_c22 = MII_CTRL_SS_1000;
            break;

        case 100:
            speed_c45 = 0;
            speed_c22 = MII_CTRL_SS_100;
            break;

        case 10:
            speed_c45 = 0;
            speed_c22 = MII_CTRL_SS_10;
            break;

        default:
            return SOC_E_PARAM;

    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_CTRLr(unit, pc, speed_c45|speed_c22, 
        IEEE_CTRLr_SPEED_10G_MASK | MII_CTRL_SS_MSB | MII_CTRL_SS_LSB));

    /*
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_MII_CTRLr(unit, pc, speed_c22, MII_CTRL_SS_MSB | MII_CTRL_SS_LSB));
     */

    /* Sync the speed bits with the GPHY MII CTRL reg. */

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_AN_REG(unit, pc, 0x8000, (1U<<13), (1U << 13)));

    /* need to set the internal phy's speed accordingly */

    if (NULL != int_pc) {
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
    }
                                                                                
    return rv;
}

STATIC int
_phy_8481_xaui_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;
    uint16      force_speed;
    uint16      speed_mii;

    pc = EXT_PHY_SW_STATE(unit, port);
    speed_mii = 0;

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    /* Check for valid speed and convert valid speed to HW values */
    force_speed = SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK;
    switch (speed) {
    case 10:
        speed_mii = MII_CTRL_SS_10;
        break;
    case 100:
        speed_mii = MII_CTRL_SS_100;
        break;
    case 1000:
        speed_mii = MII_CTRL_SS_1000;
        force_speed = 0;
        break;
    case 2500:
        speed_mii = MII_CTRL_SS_MSB | MII_CTRL_SS_LSB;
        force_speed |= 0;
        break;
    case 5000:
        force_speed |= 1;
        break;
    case 6000:
        force_speed |= 2;
        break;
    case 0:      /* set to reasonable speed */
        /* fall through */
    case 10000:
        force_speed |= 4; /* 10G CX4 */
        break;
    }

    /* Disable autoneg */
    SOC_IF_ERROR_RETURN
        (_phy_8481_xaui_an_set(unit, port, 0));

    /* Puts PLL in reset state and forces all datapath into reset state */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_XGXSBLK0_XGXSCONTROLr(unit, pc, 0,
                                  XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    /* Set force speed */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_SERDESDIGITAL_MISC1r(unit, pc, force_speed,
                                SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK |
                                SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, speed_mii,
                                MII_CTRL_SS_LSB|MII_CTRL_SS_MSB));

    /* Bring PLL out of reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                                XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    (void) _phy_8481_phyxs_l_pll_lock_wait(unit, port);

    return (SOC_E_NONE);
}


STATIC int
phy_8481_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_speed_set(unit, port, speed));
        pc->copper.force_speed = speed;
    } else {
        PHYDRV_CALL_ARG1(pc,PHY_SPEED_SET,speed);
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_speed_set(unit, port, speed));
        pc->fiber.force_speed = speed;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - current link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
_phy_8481_copper_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    uint16 ctrl, status, speed_c45;
    int an = 0;
    int an_done = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (phy_8481_an_get(unit, port, &an, &an_done));

    if (an) { /* autoneg is enabled */

        if (an_done) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TENG_AN_CTRLr(unit, pc, &ctrl));
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_TENG_AN_STATr(unit, pc, &status));
            if ((ctrl & TENG_IEEE_AN_CTRL_10GBT)&&(status & 
                TENG_IEEE_AN_STAT_LP_10GBT)) {

                *speed = 10000;
            } else {
                /* look at the CL22 regs and determine the gcd */
                SOC_IF_ERROR_RETURN
                    (_phy8481_auto_negotiate_gcd(unit, port, speed, NULL));
            }
        } else {
            *speed = 0;
            return(SOC_E_NONE);
        }

    } else { /* autoneg is not enabled, forced speed */
             /* read the mii ctrl reg. */

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_CTRLr(unit, pc, &speed_c45));
        switch(MII_CTRL_SS(speed_c45)) {
        case MII_CTRL_SS_10:
            *speed = 10;
            break;
        case MII_CTRL_SS_100:
            *speed = 100;
            break;
        case MII_CTRL_SS_1000:
            *speed = 1000;
            break;
        default:
            if ((speed_c45 & (IEEE_CTRLr_SPEED_10G_MASK|MII_CTRL_SS_MSB|MII_CTRL_SS_LSB))
                ==IEEE_CTRLr_SPEED_10G) {
                *speed = 10000;
                break;
            } else {
                *speed = 0;
                return(SOC_E_NONE);
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    uint16      hw_speed;
    int         autoneg;

    static int  actual_speed[] = {10,      /*  0  */
                                  100,     /*  1  */
                                  1000,    /*  2  */
                                  2500,    /*  3  */
                                  5000,    /*  4  */
                                  6000,    /*  5  */
                                  10000,   /*  6  */
                                  10000,   /*  7  */
                                  12000,   /*  8  */
                                  12500,   /*  9  */
                                  13000,   /*  A  */
                                  15000,   /*  B  */
                                  16000,   /*  C  */
                                  1000,    /*  D  */
                                  10000};  /*  E  */
    static int  forced_speed[] = {2500,    /*  0  */
                                  5000,    /*  1  */
                                  6000,    /*  2  */
                                  10000,   /*  3  */
                                  10000,   /*  4  */
                                  12000,   /*  5  */
                                  12500,   /*  6  */
                                  13000,   /*  7  */
                                  15000,   /*  8  */
                                  16000};  /*  9  */ 

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_8481_xaui_an_get(unit, port, &autoneg, NULL));

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    /* Set reasonable default speed */
    *speed = 10000; 
    if (autoneg) {
        /* Get actual speed */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_GP_STATUS_TOPANSTATUS1r(unit, pc, &hw_speed));
        hw_speed = (hw_speed & GP_STATUS_TOPANSTATUS1_ACTUAL_SPEED_MASK) >>
                   GP_STATUS_TOPANSTATUS1_ACTUAL_SPEED_SHIFT;
        if (hw_speed < COUNTOF(actual_speed)) {
            *speed = actual_speed[hw_speed];
        } else {
            return (SOC_E_INTERNAL);
        }
    } else {
        /* Get forced speed */
        SOC_IF_ERROR_RETURN 
            (READ_PHY8481_PHYXS_L_SERDESDIGITAL_MISC1r(unit, pc, &hw_speed));
        if (hw_speed & SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK) {
            hw_speed = (hw_speed & SERDESDIGITAL_MISC1_FORCE_SPEED_MASK) >>
                       SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT;
            if (hw_speed < COUNTOF(forced_speed)){
                *speed = forced_speed[hw_speed];
            } else {
                return (SOC_E_INTERNAL);
            }
        } else {
            uint16       speed_mii;
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, &speed_mii));
            speed_mii &= (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
            switch (speed_mii) {
            case MII_CTRL_SS_10:
                *speed = 10;
                break;
            case MII_CTRL_SS_100:
                *speed = 100;
                break;
            case MII_CTRL_SS_1000:
                *speed = 1000;
                break;
            case (MII_CTRL_SS_MSB | MII_CTRL_SS_LSB):   
                *speed = 2500;
                break;
            default:
                *speed = pc->speed_max;
                break;
            }
        }
    }

    SOC_DEBUG_PRINT((DK_PHY,
    "_phy_8481_xaui_speed_get: u=%d p=%d GP_STATUS_TOPANSTATUS1 %04x speed= %d\n",
                        unit, port, hw_speed,*speed));

    return (SOC_E_NONE);
}



STATIC int
phy_8481_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_speed_get(unit, port, speed));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_SPEED_GET,speed);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_speed_get(unit, port, speed));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_copper_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16      mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_CTRLr(unit, pc, &mii_ctrl));

    if (duplex) {
        mii_ctrl |= MII_CTRL_FD;
    } else {
        mii_ctrl &= ~MII_CTRL_FD;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MII_CTRLr(unit, pc, mii_ctrl));

    pc->copper.force_duplex = duplex;

    return(SOC_E_NONE);
}

STATIC int
_phy_8481_xaui_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16      mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));

    if (duplex) {
        mii_ctrl |= MII_CTRL_FD;
    } else {
        mii_ctrl &= ~MII_CTRL_FD;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, mii_ctrl));

    pc->fiber.force_duplex = duplex;

    return SOC_E_NONE;
}

STATIC int
phy_8481_duplex_set(int unit, soc_port_t port, int duplex)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_duplex_set(unit, port, duplex));
    } else {
        PHYDRV_CALL_ARG1(pc,PHY_DUPLEX_SET,duplex);
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_duplex_set(unit, port, duplex));
    }

    return SOC_E_NONE;
}


STATIC int
_phy_8481_copper_duplex_get(int unit, soc_port_t port, int *duplex)
{
    int         rv;
    uint16      mii_ctrl, mii_stat;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_STATr(unit, pc, &mii_stat));

    rv = SOC_E_NONE;
    if (mii_ctrl & MII_CTRL_AE) {     /* Auto-negotiation enabled */
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *duplex = FALSE;
        } else {
            rv = _phy8481_auto_negotiate_gcd(unit, port, NULL, duplex);
        }
    } else {                /* Auto-negotiation disabled */
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    }

    return(rv);
}

STATIC int
_phy_8481_xaui_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    *duplex = TRUE;

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_GP_STATUS_STATUS1000X1r(unit, pc, &reg0_16));

    if (reg0_16 & GP_STATUS_STATUS1000X1_SGMII_MODE_MASK) {

    /* retrieve the duplex setting in SGMII mode */
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8481_PHYXS_L_COMBO_IEEE0_AUTONEGLPABILr(unit,pc,&reg0_16));

            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if (reg0_16 & MII_ANP_SGMII_MODE) {
                if (reg0_16 & MII_ANP_SGMII_FD) {
                    *duplex = TRUE;
                } else {
                    *duplex = FALSE;
                }

                return SOC_E_NONE;
            }
        }
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

STATIC int
phy_8481_duplex_get(int unit, soc_port_t port, int *duplex)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_duplex_get(unit, port, duplex));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_DUPLEX_GET,duplex);
            if ((*duplex) == FALSE) {
                return SOC_E_NONE;
            }
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_duplex_get(unit, port, duplex));
    }

    return SOC_E_NONE;
}


#define PHY8481_PHYXS_L_PLL_WAIT  250000

STATIC int
_phy_8481_phyxs_l_pll_lock_wait(int unit, soc_port_t port)
{
    uint16           data16;
    phy_ctrl_t      *pc;
    soc_timeout_t    to;
    int              rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    soc_timeout_init(&to, PHY8481_PHYXS_L_PLL_WAIT, 0);
    do {
        rv = READ_PHY8481_PHYXS_L_XGXSBLK0_XGXSSTATUSr(unit, pc, &data16);
        if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) == 0) {
        SOC_DEBUG_PRINT((DK_WARN,
                       "PHY 8481 XS L : TXPLL did not lock: u=%d p=%d\n",
                        unit, port));
        return (SOC_E_TIMEOUT);
    }
    return (SOC_E_NONE);
}


/*
 * Function:
 *      phy_8481_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_8481_copper_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16 mii_ctrl, mii_stat;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_STATr(unit, pc, &mii_stat));

    /* Looking at the MII regs would suffice */
    *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;

    if (NULL != an_done) {
        *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      mii_ctrl;
    uint16      an_status;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
    *an = ((mii_ctrl & MII_CTRL_AE) != 0);

    if (NULL != an_done) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_GP_STATUS_TOPANSTATUS1r(unit, pc, &an_status));
        *an_done = ((an_status &
                     GP_STATUS_TOPANSTATUS1_CL37_AUTONEG_COMPLETE_MASK) != 0);

        if (PHY_CLAUSE73_MODE(unit, port)) {
            *an_done |= ((an_status &
                         GP_STATUS_TOPANSTATUS1_CL73_AUTONEG_COMPLETE_MASK) !=
                        0);
        }
    }

    return (SOC_E_NONE);
}


STATIC int
phy_8481_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_an_get(unit, port, an, an_done));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG2(pc,PHY_AUTO_NEGOTIATE_GET,an,an_done);
            return SOC_E_NONE;
        }

        SOC_IF_ERROR_RETURN(_phy_8481_xaui_an_get(unit, port, an, an_done));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_an_set
 * Purpose:
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_8481_copper_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_MII_CTRLr(unit, pc,
        an ? MII_CTRL_AE|MII_CTRL_RAN : 0,
        MII_CTRL_AE|MII_CTRL_RAN));

    /*
    if (mii_ctrl & MII_CTRL_AE) {
        return SOC_E_NONE;
    }
    */

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_AN_CTRLr(unit, pc, 
        an ? AN_IEEE_CTRL_REG_AN_ENA|AN_IEEE_CTRL_REG_AN_RESTART : 0,
        AN_IEEE_CTRL_REG_AN_ENA|AN_IEEE_CTRL_REG_AN_RESTART));

    pc->copper.autoneg_enable = an;
    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_an_set(int unit, soc_port_t port, int an)
{
    uint16      data16;
    uint16      mask16;
    int         cur_autoneg;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_8481_xaui_an_get(unit, port, &cur_autoneg, NULL));

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    /* If request to disable autoneg and autoneg is already disabled,
     * do nothing.
     */
    if ((!cur_autoneg) && (!an)) {
        return (SOC_E_NONE);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PHYXS_L_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr(unit, pc, an ? 1 : 0));

    /* If request to enable autoneg and autoneg is already enabled,
     * restart autoneg.
     */
    if (cur_autoneg && an) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc,
                                             MII_CTRL_AE | MII_CTRL_RAN,
                                             MII_CTRL_AE | MII_CTRL_RAN));
        if (PHY_CLAUSE73_MODE(unit, port)) {
            /* Fixme: Restart CL73 autoneg */
        }
    }

    /* Do the following sequence if changing autoneg setting. */
    /* Stop PLL Sequencer */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_XGXSBLK0_XGXSCONTROLr(unit, pc, 0,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    /* Disable/Enable 10G parallel detect */
    data16 = 0;
    mask16 = AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
    if (soc_property_port_get(unit, port, spn_XGXS_PDETECT_10G, 1) && an) {
        data16 = AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_AN73_PDET_PARDET10GCONTROLr(unit, pc, data16, mask16));

    /* Disable/Enable 1000X parallel detect */
    data16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_SERDESDIGITAL_CONTROL1000X2r(unit, pc, data16, mask16));

    /* Clause 73 backplane support */
    if (PHY_CLAUSE73_MODE(unit, port)) {
        /* Fixme: Need to program
         *    0x38008380  CL73 AN Control register
         *    0x38008372  CL73 BAM Control 1 register
         */
    }

    /* Enable autoneg and restart autoneg */
    data16 = (an) ? MII_CTRL_AE | MII_CTRL_RAN : 0;
    mask16 = MII_CTRL_AE | MII_CTRL_RAN;
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_COMBO_IEEE0_MIICNTLr(unit, pc, data16, mask16));

    /* Start PLL Sequencer and wait for PLL to lock */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PHYXS_L_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    (void) _phy_8481_phyxs_l_pll_lock_wait(unit, port);

    /* Update software state */
    pc->fiber.autoneg_enable = an;

    return SOC_E_NONE;
}


STATIC int
phy_8481_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_an_set(unit, port, an));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_AUTO_NEGOTIATE_SET,an);
            SOC_IF_ERROR_RETURN(_phy_8481_xaui_an_set(unit, port, FALSE));
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_an_set(unit, port, an));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8481_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
_phy_8481_copper_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    uint16      mii_ana, mii_gb_ctrl, teng_an_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_ANAr(unit, pc, &mii_ana));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_TENG_AN_CTRLr(unit, pc, &teng_an_ctrl));

    if (mii_ana & MII_ANA_HD_10) {
       ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
    }
    if (mii_ana & MII_ANA_HD_100) { 
      ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
    }
    if (mii_ana & MII_ANA_FD_10) {
      ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if (mii_ana & MII_ANA_FD_100) {
      ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }

    ability->pause     = 0;

    switch (mii_ana & (MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE)) {
        case MII_ANA_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    /* GE Specific values */

    if (mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD) {
       ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
    }
    if (mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD) {
       ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }

    /* 10G Specific values */
    if (teng_an_ctrl & TENG_IEEE_AN_CTRL_10GBT) {
       ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16      an_adv;
    uint16      adv_over1g;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_AUTONEGADVr(unit, pc, &an_adv));

    switch (an_adv & (MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE)) {
    case MII_ANP_C37_ASYM_PAUSE:
        ability->pause |= SOC_PA_PAUSE_TX;
        break;
    case (MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE):
        ability->pause |= SOC_PA_PAUSE_RX;
        break;
    case MII_ANP_C37_PAUSE:
        ability->pause |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        break;
    }

    ability->speed_full_duplex |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_OVER1G_UP1r(unit, pc, &adv_over1g));
    ability->speed_full_duplex |= (adv_over1g & OVER1G_UP1_DATARATE_10GCX4_MASK) ?
                                  SOC_PA_SPEED_10GB : 0;

    return SOC_E_NONE;
}

STATIC int
phy_8481_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_ability_advert_get(unit, port, ability));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_GET,ability);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_ability_advert_get(unit, port, ability));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8481_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */


STATIC int
_phy_8481_copper_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    uint16       mii_adv, mii_gb_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    mii_adv     = MII_ANA_ASF_802_3;

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_GB_CTRLr(unit, pc,  &mii_gb_ctrl));

    mii_gb_ctrl &= ~(MII_GB_CTRL_ADV_1000HD | MII_GB_CTRL_ADV_1000FD);

    if (ability->speed_half_duplex & SOC_PA_SPEED_10MB)  {
        mii_adv |= MII_ANA_HD_10;
    }
    if (ability->speed_half_duplex & SOC_PA_SPEED_100MB) {
        mii_adv |= MII_ANA_HD_100;
    }
    if (ability->speed_half_duplex & SOC_PA_SPEED_1000MB) {
        mii_gb_ctrl |= MII_GB_CTRL_ADV_1000HD;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_10MB)  {
        mii_adv |= MII_ANA_FD_10;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
        mii_adv |= MII_ANA_FD_100;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
        mii_gb_ctrl |= MII_GB_CTRL_ADV_1000FD;
    }

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        mii_adv |= MII_ANA_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        mii_adv |= MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        mii_adv |= MII_ANA_PAUSE;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_MII_ANAr(unit, pc, mii_adv, 
        MII_ANA_PAUSE|MII_ANA_ASYM_PAUSE|MII_ANA_FD_100|MII_ANA_HD_100|
        MII_ANA_FD_10|MII_ANA_HD_10|MII_ANA_ASF_802_3));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_MII_GB_CTRLr(unit, pc, mii_gb_ctrl, 
        MII_GB_CTRL_ADV_1000FD|MII_GB_CTRL_ADV_1000HD));

    if (((ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ||
         (ability->speed_half_duplex & SOC_PA_SPEED_1000MB)) && 
        (ability->speed_full_duplex & SOC_PA_SPEED_10GB)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_TENG_AN_CTRLr(unit, pc, TENG_IEEE_AN_CTRL_10GBT, 
            TENG_IEEE_AN_CTRL_10GBT));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_TENG_AN_CTRLr(unit, pc, 0, TENG_IEEE_AN_CTRL_10GBT));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_ability_advert_set(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16      an_adv;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    /* Support only full duplex */
    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? MII_ANP_C37_FD : 0;
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        an_adv |= MII_ANP_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        an_adv |= MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        an_adv |= MII_ANP_C37_PAUSE;
        break;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PHYXS_L_COMBO_IEEE0_AUTONEGADVr(unit, pc, an_adv));

    if (ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYXS_L_OVER1G_UP1r(unit, pc, OVER1G_UP1_DATARATE_10GCX4_MASK));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_PHYXS_L_OVER1G_UP1r(unit, pc, 0));
    }

    return SOC_E_NONE;
}


STATIC int
phy_8481_ability_advert_set(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_ability_advert_set(unit, port, ability));
        pc->copper.advert_ability = *ability;
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_SET,ability);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_ability_advert_set(unit, port, ability));
        pc->fiber.advert_ability = *ability;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_nxt_dev_probe(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t phy_ctrl;
    soc_phy_info_t phy_info;
    extern int _ext_phy_probe(int unit, soc_port_t port,soc_phy_info_t *pi, 
                              phy_ctrl_t *ext_pc);

    pc = EXT_PHY_SW_STATE(unit, port);

    /* initialize the phy_ctrl for the next device connected to the xaui port */
    sal_memset(&phy_ctrl, 0, sizeof(phy_ctrl_t));
    phy_ctrl.unit = unit;
    phy_ctrl.port = port;
    phy_ctrl.speed_max = pc->speed_max;
    phy_ctrl.read = pc->read;    /* use same MDIO read routine as this device's */
    phy_ctrl.write = pc->write;  /* use same MDIO write routine as this device's */ 

    /* get the mdio address of the next device */
    phy_ctrl.phy_id = soc_property_port_get(unit, port,
                                     spn_PORT_PHY_ADDR1, 
                        port < sizeof(ext_phy_dft_addr1)/sizeof(ext_phy_dft_addr1[0])?
                        ext_phy_dft_addr1[port]:0);

    /* probe the next device at the given address */
    SOC_IF_ERROR_RETURN
        (_ext_phy_probe(unit, port, &phy_info, &phy_ctrl));

    if (phy_ctrl.pd == NULL) {
        pc->driver_data = NULL;
        /* device not found */
        return SOC_E_NONE;
    }

    SOC_DEBUG_PRINT((DK_PHY, "_phy_8481_xaui_nxt_dev_probe: found phy device"
        " u=%d p=%d id0=0x%x id1=0x%x\n", 
        unit, port,phy_ctrl.phy_id0,phy_ctrl.phy_id1));

    /* Found device, allocate control structure */
    pc->driver_data = (void *)sal_alloc (sizeof (phy_ctrl_t), phy_ctrl.pd->drv_name);
    if (pc->driver_data == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memcpy(pc->driver_data, &phy_ctrl, sizeof(phy_ctrl_t));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_ability_remote_get
 * Purpose:
 *      Get the current remoteisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remoteisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
_phy_8481_copper_ability_remote_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    uint16      mii_stat, mii_anp, mii_gb_stat, teng_an_stat;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_CU(unit, pc);

    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_STATr(unit, pc, &mii_stat));

    if ((mii_stat & MII_STAT_AN_DONE & MII_STAT_LA)
                   == (MII_STAT_AN_DONE & MII_STAT_LA)) {
        /* Decode remote advertisement only when link is up and autoneg is
         * completed.
         */

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_ANPr(unit, pc, &mii_anp));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MII_GB_STATr(unit, pc, &mii_gb_stat));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_TENG_AN_STATr(unit, pc, &teng_an_stat));

        if (mii_anp & MII_ANA_HD_10) {
           ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        }
        if (mii_anp & MII_ANA_HD_100) { 
          ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
        }
        if (mii_anp & MII_ANA_FD_10) {
          ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
        if (mii_anp & MII_ANA_FD_100) {
          ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        }

        ability->pause     = 0;

        switch (mii_anp & (MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE)) {
            case MII_ANA_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANA_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX;
                break;
            case MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_RX;
                break;
        }

        /* GE Specific values */

        if (mii_gb_stat & MII_GB_STAT_LP_1000HD) {
           ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
        }
        if (mii_gb_stat & MII_GB_STAT_LP_1000FD) {
           ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }

        /* 10G Specific values */
        if (teng_an_stat & TENG_IEEE_AN_STAT_LP_10GBT) {
           ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }
    } else {
        /* Simply return local abilities */
        SOC_IF_ERROR_RETURN
            (_phy_8481_copper_ability_advert_get(unit, port, ability));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8481_xaui_ability_remote_get(int unit, soc_port_t port,
                               soc_port_ability_t *ability)
{
    uint16            mii_stat_combo;
    uint16            link_stat_gp;
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t       *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY8481_SELECT_REGS_XAUI(unit, pc);

    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_COMBO_IEEE0_MIISTATr(unit, pc, &mii_stat_combo));
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PHYXS_L_GP_STATUS_XGXSSTATUS3r(unit, pc, &link_stat_gp));
     
    SOC_DEBUG_PRINT((DK_PHY,
                     "u=%d p=%d mii_stat_combo=%04x link_stat_gp=%04x\n",
                     unit, port, mii_stat_combo, link_stat_gp));


    mode = 0;
    if ((link_stat_gp & GP_STATUS_XGXSSTATUS3_LINK_MASK) &&
        (mii_stat_combo & MII_STAT_AN_DONE)) {
        /* Decode remote advertisement only when link is up and autoneg is
         * completed.
         */

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_GP_STATUS_LP_UP1r(unit, pc, &an_adv));

        mode |= (an_adv & OVER1G_LP_UP1_DATARATE_10GCX4_MASK) ?
                 SOC_PA_SPEED_10GB : 0;

        SOC_DEBUG_PRINT((DK_PHY,
                         "u=%d p=%d over1G an_adv=%04x\n",
                         unit, port, an_adv));
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PHYXS_L_COMBO_IEEE0_AUTONEGLPABILr(unit, pc, &an_adv));
        SOC_DEBUG_PRINT((DK_PHY,
                         "u=%d p=%d combo an_adv=%04x\n",
                         unit, port, an_adv));

        mode |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
        ability->speed_full_duplex = mode;

        mode = 0; 
        switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
            case MII_ANP_C37_PAUSE:
                mode |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANP_C37_ASYM_PAUSE:
                mode |= SOC_PA_PAUSE_TX;
                break;
            case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
                mode |= SOC_PA_PAUSE_RX;
                break;
        }
        ability->pause = mode;

        if (PHY_CLAUSE73_MODE(unit, port)) {
            /* Fixme: Add support for clause 73 */
        }
    } else {
        /* Simply return local abilities */
        SOC_IF_ERROR_RETURN
            (phy_8481_ability_advert_get(unit, port, ability));
    }
    SOC_DEBUG_PRINT((DK_PHY,
         "phy_8481_xaui_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n",
         unit, port, ability->pause, ability->speed_full_duplex));

    return (SOC_E_NONE);
}

STATIC int
phy_8481_ability_remote_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_8481_copper_ability_remote_get(unit, port, ability));
    } else {
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_GET,ability);
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_phy_8481_xaui_ability_remote_get(unit, port, ability));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_ability_local_get
 * Purpose:
 *      Get the device's complete abilities.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - return device's abilities.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8481_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);


    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    ability->speed_half_duplex  = SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
    ability->speed_full_duplex  = SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB |
                                  SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_COPPER | SOC_PA_MEDIUM_XAUI;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;


    return (SOC_E_NONE);
}

STATIC int
phy_8481_link_up(int unit, soc_port_t port)
{
    int speed, duplex;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_8481_speed_get(unit, port, &speed));

    switch (speed) {

    case 10000:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_XAUI));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventSpeed, 10000));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventResume, 1));
        break;

    case 1000:
        if (PHY_COPPER_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_XAUI));
        }
        break;

    case 100:
        SOC_IF_ERROR_RETURN
            (phy_8481_duplex_get(unit, port, &duplex));
        if (duplex) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PHYXS_M_REG(unit, pc, 0xffe0,0x2100));
        } else {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PHYXS_M_REG(unit, pc, 0xffe0,0x2000));
        }
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));
        break;

    case 10:
        SOC_IF_ERROR_RETURN
            (phy_8481_duplex_get(unit, port, &duplex));
        if (duplex) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PHYXS_M_REG(unit, pc, 0xffe0,0x0100));
        } else {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_PHYXS_M_REG(unit, pc, 0xffe0,0x0000));
        }
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));
        break;

    default:
        break;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8481_mdix_set
 * Description:
 *      Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_8481_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (mode) {
    case SOC_PORT_MDIX_AUTO:
        /* Clear bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_ECRr(unit, pc, 0, 0x4000));

        /*
         * Write the result in the register 0x18, shadow copy 7
         */
        /* Clear bit 9 to disable forced auto MDI xover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_MISC_CTRLr(unit, pc, 0, 0x0200));
        break;

    case SOC_PORT_MDIX_FORCE_AUTO:
        /* Clear bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_ECRr(unit, pc, 0, 0x4000));

        /*
         * Write the result in the register 0x18, shadow copy 7
         */
        /* Set bit 9 to force automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8481_MII_MISC_CTRLr(unit, pc, 0x0200, 0x0200));
        break;

    case SOC_PORT_MDIX_NORMAL:
        SOC_IF_ERROR_RETURN(phy_8481_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
            /* Set bit 14 for manual MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_MII_ECRr(unit, pc, 0x4000, 0x4000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TEST1r(unit, pc, 0));
        } else {
            return SOC_E_UNAVAIL;
        }
        break;

    case SOC_PORT_MDIX_XOVER:
        SOC_IF_ERROR_RETURN(phy_8481_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
             /* Set bit 14 for manual MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8481_MII_ECRr(unit, pc, 0x4000, 0x4000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY8481_TEST1r(unit, pc, 0x0080));
        } else {
            return SOC_E_UNAVAIL;
        }
        break;

    default:
        return SOC_E_PARAM;
        break;
    }

    pc->copper.mdix = mode;
    return SOC_E_NONE;

}        

/*
 * Function:
 *      phy_8481_mdix_get
 * Description:
 *      Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - (Out) One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_8481_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(phy_8481_speed_get(unit, port, &speed));
    if (speed == 1000) {
       *mode = SOC_PORT_MDIX_AUTO;
    } else {
        *mode = pc->copper.mdix;
    }

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_8481_mdix_status_get
 * Description:
 *      Get the current MDIX status on a port/PHY
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 *      status  - (OUT) One of:
 *              SOC_PORT_MDIX_STATUS_NORMAL
 *                      Straight connection
 *              SOC_PORT_MDIX_STATUS_XOVER
 *                      Crossover has been performed
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_8481_mdix_status_get(int unit, soc_port_t port,
                         soc_port_mdix_status_t *status)
{
    phy_ctrl_t    *pc;
    uint16               tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_ESRr(unit, pc, &tmp));
    if (tmp & 0x2000) {
        *status = SOC_PORT_MDIX_STATUS_XOVER;
    } else {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8481_master_set
 * Purpose:
 *      Set the master mode for the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_8481_master_set(int unit, soc_port_t port, int master)
{
    uint16      mii_gb_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

    switch (master) {
    case SOC_PORT_MS_SLAVE:
        mii_gb_ctrl |= MII_GB_CTRL_MS_MAN;
        mii_gb_ctrl &= ~MII_GB_CTRL_MS;
        break;
    case SOC_PORT_MS_MASTER:
        mii_gb_ctrl |= MII_GB_CTRL_MS_MAN;
        mii_gb_ctrl |= MII_GB_CTRL_MS;
        break;
    case SOC_PORT_MS_AUTO:
        mii_gb_ctrl &= ~MII_GB_CTRL_MS_MAN;
        break;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MII_GB_CTRLr(unit, pc, mii_gb_ctrl));

    pc->copper.master = master;

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_8481_master_get
 * Purpose:
 *      Get the master mode for the PHY.  If mode is forced, then
 *      forced mode is returned; otherwise operating mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_8481_master_get(int unit, soc_port_t port, int *master)
{
    uint16      mii_gb_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

    if (!(mii_gb_ctrl & MII_GB_CTRL_MS_MAN)) {
        *master = SOC_PORT_MS_AUTO;
    } else if (mii_gb_ctrl & MII_GB_CTRL_MS) {
        *master = SOC_PORT_MS_MASTER;
    } else {
        *master = SOC_PORT_MS_SLAVE;
    }

    return(SOC_E_NONE);
}



STATIC int
_phy_8481_halt(int unit, phy_ctrl_t *pc)
{

    /* LED control stuff */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa82c,0xffff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa82d,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa82e,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa82f,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa830,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa831,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa832,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa833,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa834,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa835,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa836,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa837,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa838,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa839,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa83a,0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa83b,0xb6db));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa83c,0xffff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_PMAD_REG(unit, pc, 0xa83d,0x0000));


    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4186,0x8000)); /* enable global reset */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x017c)); /* assert reset for the whole ARM system */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0040)); /* deassert reset for the whole ARM system 
                                                                 but the ARM processor */

    /* write_addr=0xc300_0000, data=0x0000_001e, remove bottom write protection 
       and set VINITHI signal to 1 in order to have the ARM processor start executing 
       bootrom space 0xffff 0000 */

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xc300));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0x001e));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

    /* write_addr=0xffff_0000, data=0xeaff_fffe, replace the first 32 bits of bootrom
       at 0xffff0000 with the instruction "B 0xffff0000" to loop the processor in the
       bootrom address space */

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xffff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0xfffe));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 0xeaff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0000)); /* deassert reset */ 

    return (SOC_E_NONE);
}

STATIC int
_phy_8481_prog_eeprom(int unit, phy_ctrl_t *pc)
{
    /* write_addr=0xc300_0000, data=0x0000_000c, enable bottom write protection 
       and set VINITHI signal to 0 */

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, 0xc300));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, 0x000c));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|(1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0040));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0000));

    return (SOC_E_NONE);
}

STATIC int
_phy_8481_restart(int unit, phy_ctrl_t *pc)
{

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x017c));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0040));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4181,0x0000));

    return (SOC_E_NONE);
}

#define WRITE_BLK_SIZE 4

/* This routine should be used only when the ARM CPU is halted/looping. */

STATIC int
_phy_8481_write_to_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data,int len)
{
    int n_writes, i;
    uint16 ctrl;

    n_writes = (len + WRITE_BLK_SIZE -1) / WRITE_BLK_SIZE;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));

    for ( i = 0; i < n_writes; i++ ) {

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, ((*(data + 1))<<8)|(*data)));
        data += WRITE_BLK_SIZE/2;
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, ((*(data + 1))<<8)|(*data)));
        data += WRITE_BLK_SIZE/2;

        if ( i==n_writes-1 ) { /* last write */
            ctrl = (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT) 
                   | (1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT);
        } else {
            ctrl = (1U << MDIO2ARM_CTL_MDIO2ARM_SELF_INC_ADDR_SHIFT) 
                   | (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT) 
                   | (1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, ctrl));
    }

    return (SOC_E_NONE);
}

#ifdef PHY8481_ARM_MEM_DEBUG

#define READ_BLK_SIZE 4

/* This routine should be used only when the ARM CPU is halted/looping. */

STATIC int
_phy_8481_read_from_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data,int len)
{
    int n_reads, i;
    uint16 ctrl, data16;

    n_reads = (len + READ_BLK_SIZE -1) / READ_BLK_SIZE;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));

    for ( i = 0; i < n_reads; i++ ) {

        if ( i==n_reads-1 ) { /* last read */
            ctrl = (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT) 
                   | (1U << MDIO2ARM_CTL_MDIO2ARM_RD_SHIFT);
        } else {
            ctrl = (1U << MDIO2ARM_CTL_MDIO2ARM_SELF_INC_ADDR_SHIFT) 
                   | (2U << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT) 
                   | (1U << MDIO2ARM_CTL_MDIO2ARM_RD_SHIFT);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, ctrl));

        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, &data16));
        *data     = data16 & 0xff;
        *(data+1) = (data16>>8) & 0xff;
        data += READ_BLK_SIZE/2;
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, &data16));
        *data     = data16 & 0xff;
        *(data+1) = (data16>>8) & 0xff;
        data += READ_BLK_SIZE/2;

    }

    return (SOC_E_NONE);
}
#endif

/* This routine could be used while the ARM CPU is running. */

STATIC int
_phy_8481_mdio2arm_read(int unit, soc_port_t port, uint32 addr, uint32 *val, int access32)
{
    soc_timeout_t  to;
    int            rv;
    uint16 data_low = 0, data_high = 0, status;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            ((access32 ? 2U : 1U) << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT) |
            (1U << MDIO2ARM_CTL_MDIO2ARM_RD_SHIFT)));

    soc_timeout_init(&to, 10000, 0);

    while (!soc_timeout_check(&to)) {
        rv = READ_PHY8481_MDIO2ARM_STS_REG(unit, pc, &status);
        if (((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) == 0) {
        SOC_DEBUG_PRINT((DK_WARN,
                         "PHY8481 MDIO2ARM read failed: u=%d p=%d addr=%08x\n", unit, port, addr));
        return (SOC_E_FAIL);
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, &data_low));

    if (access32) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, &data_high));
    }

    *val = (data_high<<16)|data_low; 

    return (SOC_E_NONE);

}

/* This routine could be used while the ARM CPU is running. */

STATIC int
_phy_8481_mdio2arm_write(int unit, soc_port_t port, uint32 addr, uint32 val, int access32)
{
    soc_timeout_t  to;
    int            rv;
    uint16 status;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_LOW_REG(unit, pc, (addr & 0xffff)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_ADDR_HIGH_REG(unit, pc, ((addr>>16) & 0xffff)));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_DATA_LOW_REG(unit, pc, (val & 0xffff)));

    if (access32) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8481_MDIO2ARM_DATA_HIGH_REG(unit, pc, ((val>>16) & 0xffff)));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8481_MDIO2ARM_CTL_REG(unit, pc, 
            ((access32 ? 2U : 1U) << MDIO2ARM_CTL_MDIO2ARM_SIZE_SHIFT)|
            (1U << MDIO2ARM_CTL_MDIO2ARM_WR_SHIFT)));

    soc_timeout_init(&to, 10000, 0);

    while (!soc_timeout_check(&to)) {
        rv = READ_PHY8481_MDIO2ARM_STS_REG(unit, pc, &status);
        if (((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((status & MDIO2ARM_STS_MDIO2ARM_DONE_MASK) == 0) {
        SOC_DEBUG_PRINT((DK_WARN,
                         "PHY8481 MDIO2ARM write failed: u=%d p=%d addr=%08x\n", unit, port, addr));
        return (SOC_E_FAIL);
    }

    return (SOC_E_NONE);

}

int
_phy_8481_mdio2arm_read32(int unit, soc_port_t port, uint32 addr, uint32 *val)
{
    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read(unit, port, addr, val, TRUE));
    return (SOC_E_NONE);
}

int
_phy_8481_mdio2arm_write32(int unit, soc_port_t port, uint32 addr, uint32 val)
{
    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_write(unit, port, addr, val, TRUE));
    return (SOC_E_NONE);
}

int
_phy_8481_get_fw_stat_reg(int unit, soc_port_t port, uint32 *data32)
{

    /* Write SPI_BRIDGE_CTRL_2=0x03000000 (SPI_BRIDGE_CTRL_2 @ 0xc200_0014) */
    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_write32(unit, port, 0xc2000014, 0x03000000));

    /* Read SPI_FW_STATUS bit0-bit6 (SPI_FW_STATUS @ 0xc200_0000) */
    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read32(unit, port, 0xc2000000, data32));

    return (SOC_E_NONE);

}

#ifdef PHY8481_ARM_MEM_DEBUG

int
_phy_8481_mdio2arm_read16(int unit, soc_port_t port, uint32 addr, uint16 *val)
{
    uint32 data;

    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read(unit, port, addr, &data, FALSE));
    *val = data;

    return (SOC_E_NONE);
}

int
_phy_8481_mdio2arm_write16(int unit, soc_port_t port, uint32 addr, uint16 val)
{
    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_write(unit, port, addr, (uint32)val, FALSE));
    return (SOC_E_NONE);
}


int
_phy_8481_show_arm16(int unit, soc_port_t port, uint32 addr)
{
    uint16 data;

    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read16(unit, port, addr, &data));

    soc_cm_print("Value = %04x\n", data); 

    return (SOC_E_NONE);
}

int
_phy_8481_show_arm32(int unit, soc_port_t port, uint32 addr)
{
    uint32 data;

    SOC_IF_ERROR_RETURN
        (_phy_8481_mdio2arm_read32(unit, port, addr, &data));

    soc_cm_print("Value = %08x\n", data); 

    return (SOC_E_NONE);
}

#endif

/*
 * Function:
 *      phy_8481_firmware_set
 * Purpose:
 *      program the given firmware into the SPI-ROM
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      offset - offset to the data stream
 *      array  - the given data
 *      datalen- the data length
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_8481_firmware_set(int unit, int port, int offset, uint8 *data,int len)
{
    soc_timeout_t  to;
    int            rv;
    uint16 status;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ((!PHY_COPPER_MODE(unit, port)) && NXT_PC(pc)) {
        /* there is a PHY device connected to this XAUI port. Always program the
         * outmost PHY device. To program the next external device in the chain,
         * detach/remove the outmost phy device first
         */
        PHYDRV_CALL_ARG3(pc,PHY_FIRMWARE_SET,offset,data,len);
        return SOC_E_NONE;
    }

    soc_cm_print("Step 1: Halting the system.\n");

    SOC_IF_ERROR_RETURN
        (_phy_8481_halt(unit, pc));

    soc_cm_print("Step 2: Uploading the SPI loader and data into main memory.\n");

    SOC_IF_ERROR_RETURN
        (_phy_8481_write_to_arm(unit, pc, (uint32) offset, data, len));

    SOC_IF_ERROR_RETURN
        (_phy_8481_prog_eeprom(unit, pc));

    soc_cm_print("Step 3: Waiting for the Programming phase to be completed (5 seconds).\n");

    sal_sleep(5);

    /* Poll the LED4 status for success */

    soc_timeout_init(&to, 10000, 0);

    while (!soc_timeout_check(&to)) {
        rv = READ_PHY8481_PMAD_REG(unit, pc, 0xa838, &status);
        if (status || SOC_FAILURE(rv)) {
            break;
        }
    }

    if (status != 0xffff) {
        SOC_DEBUG_PRINT((DK_WARN,
                         "PHY8481 firmware upgrade possibly failed: u=%d p=%d\n", unit, port));
        return (SOC_E_FAIL);
    }

    soc_cm_print("Step 4: Restarting system.\n");

    SOC_IF_ERROR_RETURN
        (_phy_8481_restart(unit, pc));

    return SOC_E_NONE;
}

/*
 * Variable:
 *    phy_8481_drv
 * Purpose:
 *    Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_8481drv_xe = {
    "8481 10-Gigabit PHY Driver",
    phy_8481_init,        /* Init */
    phy_null_reset,       /* Reset */
    phy_8481_link_get,    /* Link get   */
    phy_8481_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_8481_duplex_set,  /* Duplex set */
    phy_8481_duplex_get,  /* Duplex get */
    phy_8481_speed_set,   /* Speed set  */
    phy_8481_speed_get,   /* Speed get  */
    phy_8481_master_set,  /* Master set */
    phy_8481_master_get,  /* Master get */
    phy_8481_an_set,      /* ANA set */
    phy_8481_an_get,      /* ANA get */
    NULL,                 /* Local Advert set, deprecated */
    NULL,                 /* Local Advert get, deprecated */
    NULL,                 /* Remote Advert get, deprecated */
    phy_8481_lb_set,      /* PHY loopback set */
    phy_8481_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_xehg_interface_get, /* IO Interface get */
    NULL,                   /* pd_ability, deprecated */
    phy_8481_link_up,
    NULL,
    phy_8481_mdix_set,
    phy_8481_mdix_get,
    phy_8481_mdix_status_get,
    phy_8481_medium_config_set, /* medium config setting set */
    phy_8481_medium_config_get, /* medium config setting get */
    phy_8481_medium_status,        /* active medium */
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8481_control_set,    /* phy_control_set */
    phy_8481_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_8481_probe,          /* pd_probe  */
    phy_8481_ability_advert_set,  /* pd_ability_advert_set */
    phy_8481_ability_advert_get,  /* pd_ability_advert_get */
    phy_8481_ability_remote_get,  /* pd_ability_remote_get */
    phy_8481_ability_local_get,   /* pd_ability_local_get  */
    phy_8481_firmware_set
};

#else /* INCLUDE_PHY_8481 */
int _phy_8481_not_empty;
#endif /* INCLUDE_PHY_8481 */

