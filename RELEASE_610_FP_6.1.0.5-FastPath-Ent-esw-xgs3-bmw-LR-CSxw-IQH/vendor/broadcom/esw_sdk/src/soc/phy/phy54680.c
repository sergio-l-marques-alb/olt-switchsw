/*
 * $Id: phy54680.c,v 1.1 2011/04/18 17:11:07 mruas Exp $
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
 * File:        phy54680.c
 * Purpose:     PHY driver for BCM54680
 */
#include <sal/types.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phydefs.h"      /* Must include before other phy related includes */

#if defined(INCLUDE_PHY_54680)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */

#include "phyident.h"
#include "phyreg.h"
#include "phyfege.h"
#include "phynull.h"
#include "phy54680.h"

#define PHY_IS_BCM54680_A0(_pc)   (PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54680_OUI, \
                                                PHY_BCM54680_MODEL) \
                                   && ((_pc)->phy_rev == 0x0 ))

#define PHY_BCM54680_A0_ID0    PHY_ID0(PHY_BCM54680_OUI,PHY_BCM54680_MODEL,0)
#define PHY_BCM54680_A0_ID1    PHY_ID1(PHY_BCM54680_OUI,PHY_BCM54680_MODEL,0)

#ifndef DISABLE_CLK125
#define DISABLE_CLK125 0
#endif

#ifndef AUTO_MDIX_WHEN_AN_DIS
#define AUTO_MDIX_WHEN_AN_DIS 0
#endif

#define DISABLE_TEST_PORT 1

STATIC int
phy_54680_speed_set(int unit, soc_port_t port, int speed);
STATIC int
phy_54680_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int
phy_54680_master_set(int unit, soc_port_t port, int master);
STATIC int
phy_54680_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int
phy_54680_autoneg_set(int unit, soc_port_t port, int autoneg);
STATIC int
phy_54680_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
STATIC int
phy_54680_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);

/*
 * Function:
 *      phy_54680_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE.
 */
STATIC int
phy_54680_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{

    *medium = SOC_PORT_MEDIUM_COPPER;

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54680_medium_config_set
 * Purpose:
 *      Set the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_54680_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    COMPILER_REFERENCE(medium);

    SOC_IF_ERROR_RETURN
        (phy_54680_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_54680_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_54680_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (phy_54680_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_54680_autoneg_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (phy_54680_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_medium_config_get
 * Purpose:
 *      Get the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - (OUT) Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54680_medium_config_get(int unit, soc_port_t port, 
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    phy_ctrl_t    *pc;

    COMPILER_REFERENCE(medium);

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memcpy(cfg, &pc->copper, sizeof (*cfg));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _phy_54680_reset_setup
 * Purpose:
 *      Function to reset the PHY and set up initial operating registers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54680_reset_setup(int unit, soc_port_t port)
{
    phy_ctrl_t        *int_pc;
    phy_ctrl_t        *pc;
    uint16             tmp;

    SOC_IF_ERROR_RETURN(phy_ge_init(unit, port));


    pc     = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

#ifdef DISABLE_TEST_PORT
    if (PHY_IS_BCM54680_A0(pc))  {
        /* Disable test port */

        uint8  phy_addr;
        uint16 id0, id1;

        phy_addr = PORT_TO_PHY_ADDR(unit, port) + 1;

        SOC_IF_ERROR_RETURN
            (pc->read(unit, phy_addr, MII_PHY_ID0_REG, &id0));

        SOC_IF_ERROR_RETURN
            (pc->read(unit, phy_addr, MII_PHY_ID1_REG, &id1));

        if ( ( id0 != PHY_BCM54680_A0_ID0 ) || ( id1 != PHY_BCM54680_A0_ID1 ) ) {

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x1f, 0xffd0));

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x1e, 0x001f));

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x1f, 0x8000));

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x1d, 0x4002));

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x00, MII_CTRL_RESET));
        }
    }
#endif

    /* remove power down */
    if (pc->copper.enable) {
        tmp = PHY_DISABLED_MODE(unit, port) ? MII_CTRL_PD : 0;
    } else {
        tmp = MII_CTRL_PD;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54680_MII_CTRLr(unit, pc, tmp, MII_CTRL_PD));

    if (NULL != int_pc) {
        SOC_IF_ERROR_RETURN
            (PHY_INIT(int_pc->pd, unit, port));
    } 

#if DISABLE_CLK125
    /* Reduce EMI emissions by disabling the CLK125 pin if not used */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54680_SPARE_CTRL_3r(unit, pc, 0, 1));
#endif


    /* Configure Extended Control Register */
    /* Enable LEDs to indicate traffic status */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54680_MII_ECRr(unit, pc, 0x0020, 0x0020));

#if AUTO_MDIX_WHEN_AN_DIS
    /* Enable Auto-MDIX When autoneg disabled */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54680_MII_MISC_CTRLr(unit, pc, 0x0200, 0x0200));
#endif

    /* Enable extended packet length (4.5k through 25k) */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54680_MII_AUX_CTRLr(unit, pc, 0x4000, 0x4000));

    /* Configure LED selectors */
    tmp = ((pc->ledmode[1] & 0xf) << 4) | (pc->ledmode[0] & 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54680_LED_SELECTOR_1r(unit, pc, tmp));

    tmp = ((pc->ledmode[3] & 0xf) << 4) | (pc->ledmode[2] & 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54680_LED_SELECTOR_2r(unit, pc, tmp));

    tmp = (pc->ledctrl & 0x3ff);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54680_LED_CTRLr(unit, pc, tmp));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54680_EXP_LED_SELECTORr(unit, pc, pc->ledselect));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54680_init
 * Purpose:
 *      Init function for 54680 PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_54680_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;

    pc     = EXT_PHY_SW_STATE(unit, port);

    pc->automedium = FALSE;
    pc->fiber_detect = FALSE;
    pc->fiber.enable = FALSE;

    pc->copper.enable = TRUE;
    pc->copper.preferred = TRUE;
    pc->copper.autoneg_enable = TRUE;
    pc->copper.force_speed = 1000;
    pc->copper.force_duplex = TRUE;
    pc->copper.master = SOC_PORT_MS_AUTO;
    pc->copper.mdix = SOC_PORT_MDIX_AUTO;


    /* Initially configure for the preferred medium. */

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);

    /* Get Requested LED selectors (defaults are hardware defaults) */
    pc->ledmode[0] = soc_property_port_get(unit, port, spn_PHY_LED1_MODE, 0);
    pc->ledmode[1] = soc_property_port_get(unit, port, spn_PHY_LED2_MODE, 1);
    pc->ledmode[2] = soc_property_port_get(unit, port, spn_PHY_LED3_MODE, 3);
    pc->ledmode[3] = soc_property_port_get(unit, port, spn_PHY_LED4_MODE, 6);
    pc->ledctrl    = soc_property_port_get(unit, port, spn_PHY_LED_CTRL, 0x8);
    pc->ledselect  = soc_property_port_get(unit, port, spn_PHY_LED_SELECT, 0);

    SOC_IF_ERROR_RETURN
        (_phy_54680_reset_setup(unit, port));

    /* Advertise all possible by default */
    SOC_IF_ERROR_RETURN
        (phy_54680_ability_local_get(unit, port, &pc->copper.advert_ability));

    SOC_IF_ERROR_RETURN
        (phy_54680_medium_config_set(unit, port, 0, &pc->copper)); 

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54680_enable_set
 * Purpose:
 *      Enable or disable the physical interface.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54680_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;
    uint16 power_down;

    pc = EXT_PHY_SW_STATE(unit, port);

    power_down = (enable) ? 0 : MII_CTRL_PD;

    if (pc->copper.enable) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_CTRLr(unit, pc, power_down, MII_CTRL_PD));
    }

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_enable_set(unit, port, enable));

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54680_enable_get
 * Purpose:
 *      Enable or disable the physical interface for a 54680 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54680_enable_get(int unit, soc_port_t port, int *enable)
{
    SOC_IF_ERROR_RETURN
        (phy_fe_ge_enable_get(unit, port, enable));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54680_link_get
 * Purpose:
 *      Determine the current link up/down status for a 54680 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      link - (OUT) Boolean, true indicates link established.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If using automedium, also switches the mode.
 */

STATIC int
phy_54680_link_get(int unit, soc_port_t port, int *link)
{

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_link_get(unit, port, link));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_XXX
 *      SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes:
 *      The duplex is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 *      Autonegotiation is not manipulated.
 */

STATIC int
phy_54680_duplex_set(int unit, soc_port_t port, int duplex)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_duplex_set(unit, port, duplex));

    pc->copper.force_duplex = duplex;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_duplex_get
 * Purpose:
 *      Get the current operating duplex mode. If autoneg is enabled,
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The duplex is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_54680_duplex_get(int unit, soc_port_t port, int *duplex)
{
    SOC_IF_ERROR_RETURN
        (phy_fe_ge_duplex_get(unit, port, duplex));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_speed_set
 * Purpose:
 *      Set the current operating speed (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Requested speed.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The speed is set only for the ACTIVE medium.
 */

STATIC int
phy_54680_speed_set(int unit, soc_port_t port, int speed)
{
    SOC_IF_ERROR_RETURN
        (phy_fe_ge_speed_set(unit, port, speed));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_speed_get
 * Purpose:
 *      Get the current operating speed for a 54680 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The speed is retrieved for the ACTIVE medium.
 */

STATIC int
phy_54680_speed_get(int unit, soc_port_t port, int *speed)
{
    SOC_IF_ERROR_RETURN
        (phy_fe_ge_speed_get(unit, port, speed));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_master_set
 * Purpose:
 *      Set the current master mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The master mode is set only for the ACTIVE medium.
 */

STATIC int
phy_54680_master_set(int unit, soc_port_t port, int master)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_master_set(unit, port, master));

    pc->copper.master = master;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_master_get
 * Purpose:
 *      Get the current master mode for a 54680 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_54680_master_get(int unit, soc_port_t port, int *master)
{
    SOC_IF_ERROR_RETURN
        (phy_fe_ge_master_get(unit, port, master));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_autoneg_set
 * Purpose:
 *      Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is set only for the ACTIVE medium.
 */

STATIC int
phy_54680_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    int           rv;
    phy_ctrl_t    *pc;

    pc             = EXT_PHY_SW_STATE(unit, port);
    rv             = SOC_E_NONE;

    /* Set auto-neg on PHY */
    rv = phy_fe_ge_an_set(unit, port, autoneg);
    if (SOC_SUCCESS(rv)) {
        pc->copper.autoneg_enable = autoneg ? 1 : 0;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_autoneg_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - (OUT) if true, auto-negotiation is enabled.
 *      autoneg_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if autoneg == FALSE.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_54680_autoneg_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_an_get(unit, port, autoneg, autoneg_done));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54680_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    uint16       mii_adv, mii_gb_ctrl;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    mii_adv     = MII_ANA_ASF_802_3;

    SOC_IF_ERROR_RETURN
        (READ_PHY54680_MII_GB_CTRLr(unit, pc,  &mii_gb_ctrl));

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
        (MODIFY_PHY54680_MII_ANAr(unit, pc, mii_adv,
        MII_ANA_PAUSE|MII_ANA_ASYM_PAUSE|MII_ANA_FD_100|MII_ANA_HD_100|
        MII_ANA_FD_10|MII_ANA_HD_10|MII_ANA_ASF_802_3));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54680_MII_GB_CTRLr(unit, pc, mii_gb_ctrl,
        MII_GB_CTRL_ADV_1000FD|MII_GB_CTRL_ADV_1000HD));

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54680_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_54680_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint16      mii_ana;
    uint16      mii_gb_ctrl;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY54680_MII_ANAr(unit, pc, &mii_ana));
    SOC_IF_ERROR_RETURN
        (READ_PHY54680_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

    sal_memset(ability, 0, sizeof(*ability));

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

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_ability_remote_get
 * Purpose:
 *      Get partners current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remote advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. If Autonegotiation is
 *      disabled or in progress, this routine will return an error.
 */

STATIC int
phy_54680_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t       *pc;
    uint16            mii_stat;
    uint16            mii_anp;
    uint16            mii_gb_stat;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY54680_MII_STATr(unit, pc, &mii_stat));

    sal_memset(ability, 0, sizeof(*ability));

    if ((mii_stat & MII_STAT_AN_DONE & MII_STAT_LA)
                   == (MII_STAT_AN_DONE & MII_STAT_LA)) {
        /* Decode remote advertisement only when link is up and autoneg is
         * completed.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY54680_MII_ANPr(unit, pc, &mii_anp));
        SOC_IF_ERROR_RETURN
            (READ_PHY54680_MII_GB_STATr(unit, pc, &mii_gb_stat));

        ability->speed_half_duplex  = SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
        ability->speed_full_duplex  = SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB;

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

    } else {
        /* Simply return local abilities */
        phy_54680_ability_advert_get(unit, port, ability);
    }
    SOC_DEBUG_PRINT((DK_PHY,
         "phy_54680_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n",
         unit, port, ability->pause, ability->speed_full_duplex));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_ability_local_get
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
phy_54680_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);


    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    ability->speed_half_duplex  = SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
    ability->speed_full_duplex  = SOC_PA_SPEED_1000MB |SOC_PA_SPEED_100MB | 
                                  SOC_PA_SPEED_10MB;
                                  

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_SGMII;
    ability->medium    = SOC_PA_MEDIUM_COPPER; 
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54680_lb_set
 * Purpose:
 *      Set the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_54680_lb_set(int unit, soc_port_t port, int enable)
{
#if AUTO_MDIX_WHEN_AN_DIS
        {
        uint16          tmp;

        /* Disable Auto-MDIX When autoneg disabled */
        /* Enable Auto-MDIX When autoneg disabled */
        tmp = (enable) ? 0x0000: 0x0200;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_MISC_CTRLr(unit, pc, tmp, 0x0200));
        }
#endif
    SOC_IF_ERROR_RETURN
        (phy_fe_ge_lb_set(unit, port, enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_lb_get
 * Purpose:
 *      Get the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_54680_lb_get(int unit, soc_port_t port, int *enable)
{
    SOC_IF_ERROR_RETURN
        (phy_fe_ge_lb_get(unit, port, enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54680_interface_set
 * Purpose:
 *      Set the current operating mode of the internal PHY.
 *      (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54680_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(pif);

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54680_interface_get
 * Purpose:
 *      Get the current operating mode of the internal PHY.
 *      (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54680_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_SGMII;

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_54680_mdix_set
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
phy_54680_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (mode) {
    case SOC_PORT_MDIX_AUTO:
        /* Clear bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_ECRr(unit, pc, 0, 0x4000));

        /*
         * Write the result in the register 0x18, shadow copy 7
         */
        /* Clear bit 9 to disable forced auto MDI xover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_MISC_CTRLr(unit, pc, 0, 0x0200));
        break;

    case SOC_PORT_MDIX_FORCE_AUTO:
        /* Clear bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_ECRr(unit, pc, 0, 0x4000));

        /*
         * Write the result in the register 0x18, shadow copy 7
         */
        /* Set bit 9 to force automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_MISC_CTRLr(unit, pc, 0x0200, 0x0200));
        break;

    case SOC_PORT_MDIX_NORMAL:
        SOC_IF_ERROR_RETURN(phy_54680_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
            /* Set bit 14 for manual MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54680_MII_ECRr(unit, pc, 0x4000, 0x4000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY54680_TEST1r(unit, pc, 0));
        } else {
            return SOC_E_UNAVAIL;
        }
        break;

    case SOC_PORT_MDIX_XOVER:
        SOC_IF_ERROR_RETURN(phy_54680_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
             /* Set bit 14 for manual MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54680_MII_ECRr(unit, pc, 0x4000, 0x4000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY54680_TEST1r(unit, pc, 0x0080));
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
 *      phy_54680_mdix_get
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
phy_54680_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    pc = EXT_PHY_SW_STATE(unit, port);


    SOC_IF_ERROR_RETURN(phy_54680_speed_get(unit, port, &speed));
    if (speed == 1000) {
       *mode = SOC_PORT_MDIX_AUTO;
    } else {
        *mode = pc->copper.mdix;
    }

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_54680_mdix_status_get
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
phy_54680_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    phy_ctrl_t    *pc;
    uint16               tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY54680_MII_ESRr(unit, pc, &tmp));
    if (tmp & 0x2000) {
        *status = SOC_PORT_MDIX_STATUS_XOVER;
    } else {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
    }

    return SOC_E_NONE;
}    

STATIC int
_phy_54680_power_mode_set (int unit, soc_port_t port, int mode)
{
    phy_ctrl_t    *pc;

    pc       = EXT_PHY_SW_STATE(unit, port);

    if (pc->power_mode == mode) {
        return SOC_E_NONE;
    }

    if (mode == SOC_PHY_CONTROL_POWER_LOW) {
        /* enable dsp clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_AUX_CTRLr(unit,pc,0x0c00,0x0c00));

        /* enable low power 136 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_AUTO_POWER_DOWNr(unit,pc,0x80,0x80));

        /* reduce tx bias current to -20% */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x17, 0x0f75));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x15, 0x1555));

        /* disable dsp clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_AUX_CTRLr(unit,pc,0x0400,0x0c00));
        pc->power_mode = mode;

    } else if (mode == SOC_PHY_CONTROL_POWER_FULL) {

        /* back to normal mode */
        /* enable dsp clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_AUX_CTRLr(unit,pc,0x0c00,0x0c00));

        /* disable low power 136 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_AUTO_POWER_DOWNr(unit,pc,0x00,0x80));

        /* set tx bias current to nominal */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x17, 0x0f75));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x15, 0x0));

        /* disable dsp clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54680_MII_AUX_CTRLr(unit,pc,0x0400,0x0c00));
        pc->power_mode = mode;
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54680_control_set
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
phy_54680_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
        case SOC_PHY_CONTROL_POWER:
            rv = _phy_54680_power_mode_set(unit,port,value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}

/*
 * Function:
 *      phy_54680_control_get
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
phy_54680_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t *pc;

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    switch(type) {
        case SOC_PHY_CONTROL_POWER:
            *value = pc->power_mode;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}


/*
 * Function:
 *      phy_54680_cable_diag
 * Purpose:
 *      Run 546x cable diagnostics
 * Parameters:
 *      unit - device number
 *      port - port number
 *      status - (OUT) cable diagnotic status structure
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_54680_cable_diag(int unit, soc_port_t port,
                    soc_port_cable_diag_t *status)
{
    int                 rv, rv2, i;

    extern int phy_5464_cable_diag_sw(int, soc_port_t ,
                                      soc_port_cable_diag_t *);

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    status->state = SOC_PORT_CABLE_STATE_OK;
    status->npairs = 4;
    status->fuzz_len = 0;
    for (i = 0; i < 4; i++) {
        status->pair_state[i] = SOC_PORT_CABLE_STATE_OK;
    }

    MIIM_LOCK(unit);    /* this locks out linkscan, essentially */
    rv = phy_5464_cable_diag_sw(unit,port, status);
    MIIM_UNLOCK(unit);
    rv2 = 0;
    if (rv <= 0) {      /* don't reset if > 0 -- link was up */
        rv2 = _phy_54680_reset_setup(unit, port);
    }
    if (rv >= 0 && rv2 < 0) {
        return rv2;
    }
    return rv;
}

/*
 * Variable:    phy_54680drv_ge
 * Purpose:     PHY driver for 54680
 */

phy_driver_t phy_54680drv_ge = {
    "54680 Gigabit PHY Driver",
    phy_54680_init,
    phy_fe_ge_reset,
    phy_54680_link_get,
    phy_54680_enable_set,
    phy_54680_enable_get,
    phy_54680_duplex_set,
    phy_54680_duplex_get,
    phy_54680_speed_set,
    phy_54680_speed_get,
    phy_54680_master_set,
    phy_54680_master_get,
    phy_54680_autoneg_set,
    phy_54680_autoneg_get,
    NULL,
    NULL,
    NULL,
    phy_54680_lb_set,
    phy_54680_lb_get,
    phy_54680_interface_set,
    phy_54680_interface_get,
    NULL,                       /* Deprecated */
    NULL,                       /* Link up event */
    NULL,
    phy_54680_mdix_set,
    phy_54680_mdix_get,
    phy_54680_mdix_status_get,
    phy_54680_medium_config_set,
    phy_54680_medium_config_get,
    phy_54680_medium_status,
    phy_54680_cable_diag,
    NULL,                        /* phy_link_change */
    phy_54680_control_set,       /* phy_control_set */ 
    phy_54680_control_get,       /* phy_control_get */
    phy_ge_reg_read,
    phy_ge_reg_write,
    phy_ge_reg_modify,
    NULL,                        /* Phy notify event */    
    NULL,                        /* pd_probe  */
    phy_54680_ability_advert_set,/* pd_ability_advert_set */
    phy_54680_ability_advert_get,/* pd_ability_advert_get */
    phy_54680_ability_remote_get,/* pd_ability_remote_get */
    phy_54680_ability_local_get  /* pd_ability_local_get  */
};

#else /* INCLUDE_PHY_54680_ESW */
int _phy_54680_not_empty;
#endif /* INCLUDE_PHY_54680_ESW */

