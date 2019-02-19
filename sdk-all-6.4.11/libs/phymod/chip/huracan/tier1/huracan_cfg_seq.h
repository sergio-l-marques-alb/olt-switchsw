/* This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : huracan_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 *  $Id$
*/
#ifndef HURACAN_CFG_SEQ_H
#define HURACAN_CFG_SEQ_H
#include <phymod/phymod_acc.h>
#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>

/*
 *  Macros
 */
#define HURACAN_GET_IF_SIDE(_FLAGS, _SIDE) \
{                                                      \
    if(((_FLAGS >> PHYMOD_INTERFACE_SIDE_SHIFT) & 0x1) == 0x1) {   \
        _SIDE = SIDE_B;                                 \
    } else {                                            \
        _SIDE = SIDE_A;                                 \
    }                                                   \
}


/** Huracan Package Sides Type.
 * Package side is defined as:
 * For Duplex Chips:
 *   Side A = Line Side
 *   Side B = System Side
 * For Simplex Chips:
 *   Side A = TX Side
 *   Side B = RX Side
*/
typedef enum {
    SIDE_A   = 0,
    SIDE_B   = 1,
    ALL_SIDES = 10
} HURACAN_PKG_SIDE_E;



/*
 *  Functions
 */


/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param rev_id             Pointer to Revision ID retrieved from the chip
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int huracan_rev_id(const phymod_access_t *pa, uint32_t *rev_id);

/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return chip_id           Chip id retrieved from the chip
 */
uint32_t _huracan_get_chip_id(const phymod_access_t *pa);

/**   Get  link status of PHY 
 *    This function is used to retrieve the link status of PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param link_status        link status of the PHY 
 *                              1 - Up 
 *                              0 - Down 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int huracan_link_status(const phymod_access_t *pa, uint32_t *link_status);

int huracan_reg_read(const phymod_access_t *pa, uint32_t addr, uint32_t  *val);

int huracan_reg_write(const phymod_access_t *pa, uint32_t addr, uint32_t  val);



int huracan_tx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_tx_lane_control_t tx_control);

int huracan_tx_squelch_get(const phymod_access_t *pa,
                               int *tx_squelch);

int huracan_rx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_rx_lane_control_t rx_control);


int huracan_rx_squelch_get(const phymod_access_t *pa, int *rx_squelch);

int _huracan_phy_status_dump(const phymod_access_t *pa);


int huracan_hard_reset(const phymod_access_t* pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);

int _huracan_phy_power_set(const phymod_access_t* pa, const phymod_phy_power_t* power);

int _huracan_phy_power_get(const phymod_access_t* pa, phymod_phy_power_t* power);

int huracan_gpio_config_set(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t gpio_mode);

int huracan_gpio_config_get(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t* gpio_mode);

int huracan_gpio_pin_value_set(const phymod_access_t *pa, int pin_no, int value);

int huracan_gpio_pin_value_get(const phymod_access_t *pa, int pin_no, int* value);


#endif /* HURACAN_CFG_SEQ_H */
