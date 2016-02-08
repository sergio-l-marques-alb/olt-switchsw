
/*
 * $Id: phy8706.c,v 1.1 2011/04/18 17:11:07 mruas Exp $
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
 * File:        phy8706.c
 * Purpose:    Phys Driver support for Broadcom 8706/8726 Serial 10Gig
 *             transceiver with XAUI interface.
 * Note:       To use this PHY driver, add phy_8706=1 config setting.
 *             PHY probing doesn't detect 8706 because the device ID is 
 *             the same as 8705.
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

#if defined(INCLUDE_PHY_8706)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyxehg.h"
#include "phy8706.h"

/* 
 * phy's revision info, device 1 
 * Revision     reg 0x0003      reg 0xC806
 *  A             0x6034            0x0010
 *  B0            0x6035            0x0010
 *  B1            0x6035            0x0011 
 */

#define PHY87X6_REV_A   0x6034
#define PHY87X6_REV_B   0x6035
#define PHY_ID1_8727    0x6036
#define PHY_8706_PCS_LOOPBACK           1
#if 0
#define PHY_8706_PMA_PMD_LOOPBACK       1
#define PHY_8706_SYSTEM_LOOPBACK        1
#define PHY_8706_XAUI_LOOPBACK          1
#endif

/*
 * SPI-ROM Program related defs
 */
#define SPI_CTRL_1_L        0xC000
#define SPI_CTRL_1_H        0xC002
#define SPI_CTRL_2_L        0xC400
#define SPI_CTRL_2_H        0xC402
#define SPI_TXFIFO          0xD000
#define SPI_RXFIFO          0xD400                                                                                 
#define WR_CPU_CTRL_REGS    0x11
#define RD_CPU_CTRL_REGS    0xEE
#define WR_CPU_CTRL_FIFO    0x66
/*
 * SPI Controller Commands(Messages).
 */
#define MSGTYPE_HWR          0x40
#define MSGTYPE_HRD          0x80
#define WRSR_OPCODE          0x01
#define WR_OPCODE            0x02
#define WRDI_OPCODE          0x04
#define RDSR_OPCODE          0x05
#define WREN_OPCODE          0x06
#define WR_BLOCK_SIZE        0x40
#define TOTAL_WR_BYTE        0x4000
                                                                                
#define WR_TIMEOUT   1000000
static int write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, uint16 * rddata);
                                                                                

STATIC int phy_8706_speed_get(int, soc_port_t, int *);

STATIC int
_8727_rom_load(int unit, int port, phy_ctrl_t *pc)
{
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0001));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x008C));
    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca85, 0x0001));

    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x018A));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0188));

    /* Wait for atleast 100ms for code to download via SPI port*/
    sal_usleep(200000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca85, 0x0000));

    sal_usleep(100000);
    return SOC_E_NONE;
}

STATIC int
_8726_rom_load(int unit, int port, phy_ctrl_t *pc)
{
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca85, 0x0001));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0188));
    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x018A));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca1e, 0x73A0));
    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0188));

    /* Wait for atleast 100ms for code to download via SPI port*/
    sal_usleep(200000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca85, 0x0000));

    sal_usleep(1000);
    return SOC_E_NONE;
}

/* Function:
 *    phy_8706_init
 * Purpose:    
 *    Initialize 8706 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8706_init(int unit, soc_port_t port)
{
    uint16         phyid1;
    soc_timeout_t  to;
    int            rv;
    uint16         data16,ucode_ver;
    phy_ctrl_t     *pc;
    uint32         phy_ext_rom_boot; 
    char           *str;
    int            reg;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45);

    /* Reset the device */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_CTRLr(unit, pc,
                                      MII_CTRL_RESET, MII_CTRL_RESET));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PCS_CTRLr(unit, pc,
                                  MII_CTRL_RESET, MII_CTRL_RESET));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PHYXS_CTRLr(unit, pc,
                                    MII_CTRL_RESET, MII_CTRL_RESET));

    /* Wait for device to come out of reset */
    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_PHY8706_PHYXS_CTRLr(unit, pc, &data16);
        if (((data16 & MII_CTRL_RESET) == 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((data16 & MII_CTRL_RESET) != 0) {
        SOC_DEBUG_PRINT((DK_WARN,
                         "PHY8706/8726 reset failed: u=%d p=%d\n",
                         unit, port));
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, MII_PHY_ID1_REG, &phyid1));

    phy_ext_rom_boot = soc_property_port_get(unit, port, 
                                            spn_PHY_EXT_ROM_BOOT, 1);
 
    if (phy_ext_rom_boot) {
        if (phyid1 == PHY_ID1_8727) {
            _8727_rom_load(unit, port,pc);
        } else {
            _8726_rom_load(unit, port,pc);
        }
    }

    /* read the microcode revision */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA1A,&ucode_ver));
 
    /* Make sure SPI-ROM load is complete */
    soc_timeout_init(&to, 500000, 0);
    do  {
        if (phyid1 == PHY_ID1_8727) {
            
            /* if microcode revision >  0xd101, then use the new method
             * to check the completion of the SPI-ROM loading
             */
            if (ucode_ver < 0xd101) {
                rv = (READ_PHY8706_GENREG1r(unit, pc, &data16));
                if (data16 == 0x0AA0 || SOC_FAILURE(rv)) {
                    break;
                }
            } else {
                rv = READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA1C,&data16);
                if (data16 == 0x600D || SOC_FAILURE(rv)) {
                    break;
                }
            }
        } else {
            rv = (READ_PHY8706_GENREG1r(unit, pc, &data16));
            if ((data16 == 0x1234) || (data16 == 0x4321) || SOC_FAILURE(rv)) {
                break;
            }
        }
    } while (!soc_timeout_check(&to));

    if (soc_timeout_check(&to) || SOC_FAILURE(rv)) {
      soc_cm_debug(DK_WARN,"8706: p=%d SPI-ROM load didn't complete (0x%x)\n",
                    port,data16);
    } else {
        soc_cm_debug(DK_PHY, "8706: SPI-ROM load took 0x%x usec\n", 
                     soc_timeout_elapsed(&to));
        /* Required additional wait after SPI-ROM load completed */
        /* sal_usleep(800000); */
    }

    soc_cm_debug(DK_PHY, "8706: u=%d port%d rom code 0x%x: init.\n",
         unit, port,data16);

    /* For rev B: do not initialize any registers and use the default values
     * if the configuration variable is not set 
     */

    /* Change XAUI TX Polarity */

    str = soc_property_port_get_str(unit, port, spn_PHY_XAUI_TX_POLARITY_FLIP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) { 

        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port, 
                                    spn_PHY_XAUI_TX_POLARITY_FLIP, 0);
        } else {  /* rev A */
            rv = 1;
        }           
                    
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_XS_REG(unit, pc, 0x80a1, &data16));
    
        if (rv) {
            data16 |= (1 << 5);
        } else {
            data16 &= ~(1 << 5);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_XS_REG(unit, pc, 0x80a1, data16));
    }

    /* Change XAUI RX Polarity */

    str = soc_property_port_get_str(unit, port, spn_PHY_XAUI_RX_POLARITY_FLIP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_XAUI_RX_POLARITY_FLIP, 0);
        } else {  /* rev A */
            rv = 0;
        }
                                                                                      
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_XS_REG(unit, pc, 0x80f6, &data16));
        if (rv) {
            data16 |= (1 << 5);
        } else {
            data16 &= ~(1 << 5);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_XS_REG(unit, pc, 0x80f6, data16));
    }

    /* XAUI RX Lane Swap (Lane A, B, C, D changes to Lane D, C, B, A) */

    str = soc_property_port_get_str(unit, port, spn_PHY_XAUI_RX_LANE_SWAP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_XAUI_RX_LANE_SWAP, 0);
        } else {  /* rev A */
            rv = 1;
        }
                                                                                      
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_XS_REG(unit, pc, 0x8100, &data16));

        if (rv) {
            data16 |= (1 << 15);
        } else {
            data16 &= ~(1 << 15);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_XS_REG(unit, pc, 0x8100, data16));
    }
    
    /* XAUI TX Lane Swap (Lane A, B, C, D changes to Lane D, C, B, A) */

    str = soc_property_port_get_str(unit, port, spn_PHY_XAUI_TX_LANE_SWAP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_XAUI_TX_LANE_SWAP, 0);
        } else {  /* rev A */
            rv = 0;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_XS_REG(unit, pc, 0x8101, &data16));
        if (rv) {
            data16 |= (1 << 15);
        } else {
            data16 &= ~(1 << 15);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_XS_REG(unit, pc, 0x8101, data16));
    }

    /* Change PCS TX Polarity */

    str = soc_property_port_get_str(unit, port, spn_PHY_PCS_TX_POLARITY_FLIP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_PCS_TX_POLARITY_FLIP, 0);
        } else {  /* rev A */
            rv = 0;
        }

        if (phyid1 == PHY_ID1_8727) {
            reg = 0xcd08;
        } else {
            reg = 0xc808;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, reg, &data16));
        if (rv) {
            data16 |= (1 << 10);
        } else {
            data16 &= ~(1 << 10);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, reg, data16));
    }

    /* Change PCS RX Polarity */

    str = soc_property_port_get_str(unit, port, spn_PHY_PCS_RX_POLARITY_FLIP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_PCS_RX_POLARITY_FLIP, 0);
        } else {  /* rev A */
            rv = 0;
        }

        if (phyid1 == PHY_ID1_8727) {
            reg = 0xcd08;
        } else {
            reg = 0xc808;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, reg, &data16));

        if (rv) {
            data16 |= (1 << 9);
        } else {
            data16 &= ~(1 << 9);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, reg, data16));
    }


    /* Make sure 8706 XAUI lanes are synchronized with the SOC XAUI */
    soc_timeout_init(&to, 100000, 0);
    while (!soc_timeout_check(&to)) {
        rv = (READ_PHY8706_PHYXS_XGXS_LANE_STATr(unit, pc, &data16));
        if (((data16 & 0x000f) == 0x000f) || SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((data16 & 0x000f) != 0x000f) {
        soc_cm_debug(DK_PHY, "8706: u=%d p=%d XAUI lane out of sync (0x%x)\n",
                     unit, port, (data16 & 0x000f));
    }

    if (phyid1 == PHY_ID1_8727) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x8329, 0));
    }
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_an_get
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
phy_8706_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16 an_status;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, AN_CTRL_REG, &an_status));
    *an = (an_status & AN_ENABLE)? TRUE: FALSE;

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, AN_STATUS_REG, &an_status));
    *an_done = (an_status & AN_DONE)? TRUE: FALSE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_an_set
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
phy_8706_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_DEBUG_PRINT((DK_PHY, "phy_8706_an_set: u=%d p=%d an=%d\n",
                     unit, port, an));

    if (an) {
        /* Allow Clause 37 through Clause 73 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x8370,0x040c));

        /* Enable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0xFFE0, 0x1300));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, AN_CTRL_REG,
                          AN_EXT_NXT_PAGE |
                          AN_ENABLE |
                          AN_RESTART));
    } else {
        /* disable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0xFFE0, 0));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, AN_CTRL_REG, 0x0));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_ability_advert_get
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
phy_8706_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    uint16           an_adv;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    /* Clause73 advert register 1 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, 0x11, &an_adv));

    mode = 0;
    mode |= (an_adv & 0x20) ? SOC_PA_SPEED_1000MB : 0;

    /* check Full Duplex advertisement on Clause 37 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, 0xFFE4,&an_adv));
    mode |= (an_adv & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

    ability->speed_full_duplex = mode;
    ability->pause = 0;

    switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
        case MII_ANA_C37_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    SOC_DEBUG_PRINT((DK_PHY,
        "phy_8706_ability_advert_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n",
        unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
* Function:
*      phy_8706_ability_remote_get
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
phy_8706_ability_remote_get(int unit, soc_port_t port, 
                            soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint16      lp_abil;

    pc = EXT_PHY_SW_STATE(unit, port);
                                           
    if (NULL == ability) {
        return SOC_E_PARAM;
    }
                                          
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, 0xFFE5, &lp_abil));
    ability->speed_full_duplex  = (lp_abil & MII_ANA_C37_FD) ? 
                                   SOC_PA_SPEED_1000MB : 0;

    ability->pause = 0;
    switch (lp_abil & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
        case MII_ANP_C37_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANP_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, AN_STATUS_REG, &lp_abil));
    ability->flags     = (lp_abil & 0x1) ? SOC_PA_AUTONEG : 0;

    SOC_DEBUG_PRINT((DK_PHY,
        "phy_8706_ability_remote_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n",
        unit, port, ability->speed_full_duplex, ability->pause));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8706_ability_advert_set
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
phy_8706_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    uint16           an_adv;
    phy_ctrl_t      *pc;                                                                                  
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    /*
     * Set advertised duplex (only FD supported).
     */
    an_adv =  (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 0x20 : 0;

    /* CL73 advert register 1, advertising 1G only */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_AN_REG(unit, pc, 0x11, an_adv));

    an_adv =  (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 
               MII_ANA_C37_FD : 0;

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
        case SOC_PA_PAUSE_TX:
            an_adv |= MII_ANA_C37_ASYM_PAUSE;
            break;
        case SOC_PA_PAUSE_RX:
            an_adv |= MII_ANA_C37_ASYM_PAUSE | MII_ANA_C37_PAUSE;
            break;
        case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
            an_adv |= MII_ANA_C37_PAUSE;
            break;
    }

    /* write Clause37 advertisement */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_AN_REG(unit, pc, 0xFFE4, an_adv, 
                              MII_ANA_C37_ASYM_PAUSE | 
                              MII_ANA_C37_PAUSE |
                              MII_ANA_C37_FD |
                              MII_ANA_C37_HD ));

    SOC_DEBUG_PRINT((DK_PHY,
        "phy_8706_ability_advert_set: u=%d p=%d pause=0x%08x adv_reg1=0x%04x\n",
        unit, port, ability->pause, an_adv));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_ability_local_get
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
phy_8706_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_DEBUG_PRINT((DK_PHY,
                     "phy_8706_ability_local_get: u=%d p=%d\n",
                     unit, port));
                                                                               
    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
                                                                               
    switch(pc->speed_max) {
        case 10000:
        default:
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }
                                                                               
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM; 
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;
                                                                               
    SOC_DEBUG_PRINT((DK_PHY,
        "phy_8706_ability_local_get: u=%d p=%d speed=0x%x\n",
        unit, port, ability->speed_full_duplex));
                                                                               
    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_8706_link_get
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
phy_8706_link_get(int unit, soc_port_t port, int *link)
{
    uint16      pma_mii_stat, pcs_mii_stat, pxs_mii_stat, link_stat;
    phy_ctrl_t *pc;
    int cur_speed = 0;
    int an,an_done;
    uint16 speed_val; 

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    phy_8706_an_get(unit,port,&an,&an_done);

    /* return link false if in the middle of autoneg */
    if (an == TRUE && an_done == FALSE) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    /* AN only supports 1G */
    if (!an) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG, &speed_val));
        if ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK) !=
                                           PMAD_CTRL2r_PMA_TYPE_1G_KX) {
            cur_speed = 10000;
        }
    } else {
        cur_speed = 1000;
    }

    if (cur_speed == 10000) { /* check all 3 device's link status if 10G */

        /* Receive Link status */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PCS_STATr(unit, pc, &pcs_mii_stat));

        /* Transmit Link status */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PHYXS_STATr(unit, pc, &pxs_mii_stat));

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_STATr(unit, pc, &pma_mii_stat));
        link_stat = pma_mii_stat & pcs_mii_stat & pxs_mii_stat;
        *link = (link_stat & MII_STAT_LA) ? TRUE : FALSE;
    } else {
        /* in 1G mode, the PMA/PMD and PCS status register's link bit always
         * show link down.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_AN_REG(unit, pc, AN_1G_STATUS_REG, &link_stat));
        if ((link_stat & AN_1G_LINKUP) &&
            (!(link_stat & AN_1G_LINK_CHANGE)) ) {
            *link = TRUE;
        } else {
            *link = FALSE;
        }
    } 

    soc_cm_debug(DK_PHY | DK_VERBOSE,
         "phy_8706_link_get: u=%d port%d: link:%s\n",
         unit, port, *link ? "Up": "Down");

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8706_enable_set
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
phy_8706_enable_set(int unit, soc_port_t port, int enable)
{
    uint16      data;       /* Holder for new value to write to PHY reg */
    uint16      mask;       /* Holder for bit mask to update in PHY reg */
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    data = 0;
    mask = 1 << 0; /* Global PMD transmit disable */
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        data = 1 << 0;  /* Global PMD transmit disable */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_TX_DISABLEr(unit, pc, data, mask));
    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_8706_lb_set
 * Purpose:
 *    Put 8706 in PHY PCS/PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8706_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 
    uint16     tmp;
    int  speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* loopback may not supported in the passthru mode on this phy.
     * So loopback from internal phy
     */

    SOC_IF_ERROR_RETURN
        (phy_8706_speed_get(unit,port,&speed));

    if (speed < 10000) {
        phy_ctrl_t    *int_pc;   /* PHY software state */
        int rv;
                                                               
        int_pc = INT_PHY_SW_STATE(unit, port);
                                                                                
        if (NULL != int_pc) {
                rv = (PHY_LOOPBACK_SET(int_pc->pd, unit, port, enable));
            } else {
                rv = SOC_E_INTERNAL;
            }
    return rv;
    }

#if defined(PHY_8706_PCS_LOOPBACK)
    tmp = enable ? MII_CTRL_LE : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PCS_CTRLr(unit, pc, tmp, MII_CTRL_LE));
#endif /* PHY_8706_PCS_LOOPBACK */

#if defined(PHY_8706_PMA_PMD_LOOPBACK)
    tmp = enable ? MII_CTRL_PMA_LOOPBACK : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_CTRLr(unit, pc,
                                      tmp, MII_CTRL_PMA_LOOPBACK));
#endif /* PHY_8706_PMA_PMD_LOOPBACK */

#if defined(PHY_8706_SYSTEM_LOOPBACK)

   tmp = enable? XGXS_MODE_SYSTEM_LOOPBACK | XGXS_CTRL_RLOOP
         :XGXS_MODE_NORMAL;
   SOC_IF_ERROR_RETURN
       (MODIFY_PHY8706_XS_REG(unit,pc,XGXS_MODE_CTRLr,
                     tmp,
                     XGXS_MODE_MASK | XGXS_CTRL_RLOOP));

#endif /* PHY_8706_SYSTEM_LOOPBACK */

#if defined(PHY_8706_XAUI_LOOPBACK)
   SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, 0x8000, &tmp));
    tmp &= ~(0xf << 8);
    tmp |= ((enable) ? 6 : 1) << 8;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, 0x8000, tmp));

    tmp = (enable) ? (0x00f0) : (0x0000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, 0x8017, tmp));
#endif /* PHY_8706_XAUI_LOOPBACK */

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8706_lb_get
 * Purpose:
 *    Get 8706 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8706_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      tmp;
    phy_ctrl_t *pc;
    int  speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_8706_speed_get(unit,port,&speed));

    if (speed < 10000) {
        phy_ctrl_t    *int_pc;   /* PHY software state */
        int rv;
                                                                                
        int_pc = INT_PHY_SW_STATE(unit, port);
                                                                                
        if (NULL != int_pc) {
                rv = (PHY_LOOPBACK_GET(int_pc->pd, unit, port, enable));
            } else {
                rv = SOC_E_INTERNAL;
            }
    return rv;
    }

#if defined(PHY_8706_PCS_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PCS_CTRLr(unit, pc, &tmp));

    *enable = (tmp & MII_CTRL_LE) ? TRUE : FALSE;
#endif /* PHY_8706_PCS_LOOPBACK */

#if defined(PHY_8706_PMA_PMD_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_CTRLr(unit, pc, &tmp));

    *enable = (tmp & MII_CTRL_PMA_LOOPBACK) ? TRUE : FALSE;
#endif /* PHY_8706_PMA_PMD_LOOPBACK */

#if defined(PHY_8706_SYSTEM_LOOPBACK)
   SOC_IF_ERROR_RETURN
       (READ_PHY8706_XS_REG(unit,pc,XGXS_MODE_CTRLr,&tmp));
   *enable = (tmp & XGXS_MODE_MASK)? FALSE:TRUE;

#endif /* PHY_8706_SYSTEM_LOOPBACK */

#if defined(PHY_8706_XAUI_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, 0x8000, &tmp));
    *enable = ((tmp & (0xf << 8)) == (6 << 8));
#endif /* PHY_8706_XAUI_LOOPBACK */

    soc_cm_debug(DK_PHY,
         "phy_8706_lb_get: u=%d port%d: loopback:%s\n",
         unit, port, *enable ? "Enabled": "Disabled");
    
    return SOC_E_NONE;
}

STATIC int
_phy_8706_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    uint16       data;  /* Temporary holder of reg value to be written */
    uint16       mask;  /* Bit mask of reg value to be updated */
    phy_ctrl_t  *pc;    /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Dozen SerDes */
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         data = (uint16)(value & 0xf);
         data = data << 12;
         mask = 0xf000;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data = (uint16)(value & 0xf);
         data = data << 8;
         mask = 0x0f00;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data = (uint16)(value & 0xf);
         data = data << 4;
         mask = 0x00f0;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_TXA_ACTRL_3r(unit, pc, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_8706_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16         data16;   /* Temporary holder of a reg value */
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    data16 = 0;
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_TXA_ACTRL_3r(unit, pc, &data16));
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         *value = (data16 & 0xf000) >> 12;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         *value = (data16 & 0x0f00) >> 8;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         *value = (data16 & 0x00f0) >> 4;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_control_set
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
phy_8706_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8706_control_tx_driver_set(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}
/*
 * Function:
 *      phy_8706_control_get
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
phy_8706_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8706_control_tx_driver_get(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_8706_speed_set
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
phy_8706_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;
    int rv = SOC_E_NONE;
    int type;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port); 

    soc_cm_debug(DK_PHY,"phy_8706_speed_set: u=%d p=%d speed=%d\n", 
                 unit, port,speed);

    if (!(speed == 10000 || speed == 1000 || speed == 2500)) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    if (speed == 10000) {
        if (pc->phy_id1 == PHY_ID1_8727) {
            type = PMAD_CTRL2r_PMA_TYPE_10G_LRM;
        } else {
            type = PMAD_CTRL2r_PMA_TYPE_10G_KR;
        }

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc, MII_CTRL_REG,
                    MII_CTRL_SS_LSB,MII_CTRL_SS_LSB));

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG,
                    type,PMAD_CTRL2r_PMA_TYPE_MASK));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, AN_CTRL_REG,
                          AN_ENABLE |
                          AN_RESTART));

        sal_usleep(40000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit,pc, AN_CTRL_REG, 0x0));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG,
                    PMAD_CTRL2r_PMA_TYPE_1G_KX,PMAD_CTRL2r_PMA_TYPE_MASK));

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc,MII_CTRL_REG,
                    0,MII_CTRL_SS_LSB));
    }

    /* need to set the internal phy's speed accordingly */

    if (NULL != int_pc) {
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
    }

    return rv;
}

/*
 * Function:
 *      phy_8706_speed_get
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
phy_8706_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;
    uint16       speed_val;
    int an = 0;
    int an_done;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_8706_an_get(unit,port,&an,&an_done);

    *speed = 10000;
    if (an) { /* autoneg is enabled */
        if (an_done) { 
            *speed = 1000;  /* can only do 1G autoneg */
        }
    } else { /* autoneg is not enabled, forced speed */

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG, &speed_val));
        if ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK) ==
                                                PMAD_CTRL2r_PMA_TYPE_1G_KX) {
            *speed = 1000;
        } else {
            *speed = 10000;
        }
    }    
    return SOC_E_NONE;
}

/*
 *  Function:  *      phy_8706_rom_wait
 *
 *  Purpose:
 *      Wait for data to be written to the SPI-ROM.
 *  Input:
 *      unit, port
 */
STATIC int
phy_8706_rom_wait(int unit, int port)
{
    uint16     rd_data;     
    uint16     wr_data;
    int        count;     
    phy_ctrl_t *pc;
    soc_timeout_t to;     
    int        rv;
    int        SPI_READY; 

    rv = SOC_E_NONE;     
    pc = EXT_PHY_SW_STATE(unit, port);

    rd_data = 0;
    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        /* Write SPI Control Register Read Command. */
        count = 1;
        wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        if (rd_data & 0x0100) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if (!(rd_data & 0x0100)) {
        soc_cm_debug(DK_ERR,"phy_8706_rom_program: u = %d p = %d "
                            "timeout 1\n", unit, port);
        return SOC_E_TIMEOUT;
    }

    SPI_READY = 1;
    while (SPI_READY == 1) {
        /* Set-up SPI Controller To Receive SPI EEPROM Status. */
        count = 1;
        wr_data = ((WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = SPI_CTRL_2_H;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Fill-up SPI Transmit Fifo To check SPI Status. */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write Tx Fifo Register Address. */
        wr_data = SPI_TXFIFO;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-1. */
        wr_data = ((1 * 0x0100) | MSGTYPE_HRD);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Tx Fifo Control Word-2. */
        wr_data = RDSR_OPCODE;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control Register Write Command. */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-1. */
        wr_data = 0x0101;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control Register Write Command. */
        count = 1;
        wr_data = ((WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_CTRL_1_H;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0103;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Wait For 64 bytes To be written.   */
        rd_data = 0x0000;
        soc_timeout_init(&to, WR_TIMEOUT, 0);
        do {
            count = 1;
            wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
            SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
            wr_data = SPI_CTRL_1_L;
            SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
            if (rd_data & 0x0100) {
                break;
            }
        } while (!soc_timeout_check(&to));
        if (!(rd_data & 0x0100)) {
            soc_cm_debug(DK_ERR,"phy_8706_rom_program: u = %d p = %d "
                                "timeout 2\n", unit, port);
            return SOC_E_TIMEOUT;
        }
        /* Write SPI Control Register Read Command. */
        count = 1;
        wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_RXFIFO;

        SOC_IF_ERROR_RETURN         
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA12, wr_data));
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));

        /* Clear LASI Message Out Status. */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &wr_data));

        if ((rd_data & 0x1) == 0) {
            SPI_READY = 0;
        }
    } /* SPI_READY  */
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8706_rom_write_enable_set
 *
 * Purpose:
 *      Enable disable protection on SPI_EEPROM
 *
 * Input:
 *      unit
 *      port
 *      enable
 * Output:
 *      SOC_E_xxx
 *
 * Notes:
 *          25AA256 256Kbit Serial EEPROM
 *          STATUS Register
 *          +------------------------------------------+
 *          | WPEN | x | x | x | BP1 | BP0 | WEL | WIP |
 *          +------------------------------------------+
 *      BP1 BP0  :   Protected Blocks
 *       0   0   :  Protect None
 *       1   1   :  Protect All
 *
 *      WEL : Write Latch Enable
 *       0  : Do not allow writes
 *       1  : Allow writes
 */

STATIC int
phy_8706_rom_write_enable_set(int unit, int port, int enable)
{
    uint16     rd_data;
    uint16     wr_data;
    uint8      wrsr_data;
    int        count;
    phy_ctrl_t *pc;
    int        rv;

    rv = SOC_E_NONE;
    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Write SPI Control Register Write Command.
     */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Address.
     */
    wr_data = SPI_CTRL_2_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Word-1.
     */
    wr_data = 0x8200;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Word-2.
     */
    wr_data = 0x0100;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Fill-up SPI Transmit Fifo With SPI EEPROM Messages.
     * Write SPI Control Register Write Command.
     */
    count = 4;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write Tx Fifo Register Address.
     */
    wr_data = SPI_TXFIFO;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-1.
     */
    wr_data = ((1 * 0x0100) | MSGTYPE_HWR);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-2.
     */
    wr_data = ((MSGTYPE_HWR * 0x0100) | WREN_OPCODE);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-3.
     */
    wr_data = ((WRSR_OPCODE * 0x100) | (0x2));
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-4.
     */
    wrsr_data = enable ? 0x2 : 0xc;
    wr_data = ((wrsr_data * 0x0100) | wrsr_data);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control Register Write Command.
     */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Address.
     */
    wr_data = SPI_CTRL_1_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Word-1.
     */
    wr_data = 0x0101;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Word-2.
     */
    wr_data = 0x0003;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Wait For WRSR Command To be written.
     */
    SOC_IF_ERROR_RETURN(phy_8706_rom_wait(unit, port));

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_8706_firmware_set
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
phy_8706_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{

    uint16     rd_data;
    uint16     wr_data;
    int        j;
    int        i = 0;
    int        count;
    phy_ctrl_t *pc;
    uint8       spi_values[WR_BLOCK_SIZE];

    pc = EXT_PHY_SW_STATE(unit, port);
    /*
     * ser_boot pin HIGH
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xCA85, 0x1, 0x1)); 
    /*
     * Read LASI Status registers To clear initial Failure status.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9004, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9005, &rd_data));

    /*
     * Enable the LASI For Message out. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x9000, 0x0400));
    /*
     * 0x9002, bit 2 Is Rx Alarm enabled For LASI. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x9002, 0x0004));
    /*
     * Read Any Residual Message out register.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));
    /*
     * Clear LASI Message Out Status. 
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));

     /* set SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(phy_8706_rom_write_enable_set(unit, port, 1));

    SOC_DEBUG_PRINT((DK_PHY, "init0: u=%d p=%d\n",
                         unit, port));

    for (j = 0; j < datalen; j += WR_BLOCK_SIZE) {
        /*
         * Setup SPI Controller. 
         */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Address.*/
        wr_data = SPI_CTRL_2_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Word-1. */
        wr_data = 0x8200;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Word-2. */
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Fill-up SPI Transmit Fifo.
         * Write SPI Control Register Write Command.
         */
        count = 4 + (WR_BLOCK_SIZE / 2);
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write Tx Fifo Register Address. */
        wr_data = SPI_TXFIFO;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-1. */
        wr_data = ((1 * 0x0100) | MSGTYPE_HWR);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-2. */
        wr_data = ((MSGTYPE_HWR * 0x0100) | WREN_OPCODE);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-3. */
        wr_data = ((WR_OPCODE * 0x0100) | (0x3 + WR_BLOCK_SIZE));
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-4. */
        wr_data = (((j & 0x00FF) * 0x0100) | ((j & 0xFF00) / 0x0100));
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        SOC_DEBUG_PRINT((DK_PHY, "loop: u=%d p=%d,inxj: %d,inxi:%d\n",
                         unit, port,j,i));

        if (datalen < (j + WR_BLOCK_SIZE)) {   /* last block */ 
            sal_memset(spi_values,0,WR_BLOCK_SIZE);
            sal_memcpy(spi_values,&array[j],datalen - j);

            for (i = 0; i < WR_BLOCK_SIZE; i += 2) {
                /* Write SPI Tx Fifo Data Word-4. */
                wr_data = ((spi_values[i+1] * 0x0100) | spi_values[i]);
                SOC_IF_ERROR_RETURN(write_message(unit, pc,wr_data, &rd_data));
            }           
        } else {        
            for (i = 0; i < WR_BLOCK_SIZE; i += 2) {
                /* Write SPI Tx Fifo Data Word-4. */
                wr_data = ((array[j+i+1] * 0x0100) | array[j+i]);
                SOC_IF_ERROR_RETURN(write_message(unit, pc,wr_data, &rd_data));
            }
        }

        /* 
         * Set-up SPI Controller To Transmit.
         * Write SPI Control Register Write Command.
         */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -1 Register Word-1. */
        wr_data = 0x0501;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0003;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Wait For 64 bytes To be written.   */
        SOC_IF_ERROR_RETURN(phy_8706_rom_wait(unit,port));

    } /* SPI_WRITE */

    /* clear SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(phy_8706_rom_write_enable_set(unit, port, 0));

    /* Disable SPI EEPROM. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Address. */
    wr_data = SPI_CTRL_2_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-1. */
    wr_data = 0x8200;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-2. */
    wr_data = 0x0100;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));


    /* Fill-up SPI Transmit Fifo With SPI EEPROM Messages. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write Tx Fifo Register Address. */
    wr_data = SPI_TXFIFO;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-1. */
    wr_data = ((0x1*0x0100) | MSGTYPE_HWR);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-2. */
    wr_data = WRDI_OPCODE;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control Register Write Command. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Address. */
    wr_data = SPI_CTRL_1_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Word-1. */
    wr_data = 0x0101;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Word-2. */
    wr_data = 0x0003;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * ser_boot pin LOW
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xCA85, 0x0, 0x1)); 
    soc_cm_debug(DK_PHY, "phy_8706_rom_program: u=%d p=%d done\n", unit, port);

    return SOC_E_NONE;
}

static int
write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, uint16 *rddata)
{

    uint16     tmp_data = 0;
    soc_timeout_t to;

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA12, wrdata));

    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9005, &tmp_data));
        if (tmp_data & 0x4)
            break;
    } while (!soc_timeout_check(&to));
    if (!(tmp_data & 0x4)) {
        soc_cm_debug(DK_ERR, "write_message failed: wrdata %04x\n", wrdata);
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    *rddata = tmp_data;
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &tmp_data));

    return SOC_E_NONE;
}

/*
 * Variable:
 *    phy_8706_drv
 * Purpose:
 *    Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_8706drv_xe = {
    "8706 10-Gigabit PHY Driver",
    phy_8706_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_8706_link_get,    /* Link get   */
    phy_8706_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_null_set,         /* Duplex set */
    phy_null_one_get,     /* Duplex get */
    phy_8706_speed_set,   /* Speed set  */
    phy_8706_speed_get,   /* Speed get  */
    phy_null_set,         /* Master set */
    phy_null_zero_get,    /* Master get */
    phy_8706_an_set,      /* ANA set */
    phy_8706_an_get,      /* ANA get */
    NULL,                 /* Local Advert set */
    NULL,                 /* Local Advert get */
    phy_null_mode_get,    /* Remote Advert get */
    phy_8706_lb_set,      /* PHY loopback set */
    phy_8706_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_xehg_interface_get, /* IO Interface get */
    NULL,   /* PHY abilities mask */
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8706_control_set,    /* phy_control_set */
    phy_8706_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    NULL,                    /* pd_probe  */
    phy_8706_ability_advert_set,  /* pd_ability_advert_set */
    phy_8706_ability_advert_get,  /* pd_ability_advert_get */
    phy_8706_ability_remote_get,  /* pd_ability_remote_get */
    phy_8706_ability_local_get,   /* pd_ability_local_get  */
    phy_8706_firmware_set         /* pd_firmware_set */
};

#else /* INCLUDE_PHY_8706 */
int _phy_8706_not_empty;
#endif /* INCLUDE_PHY_8706 */

