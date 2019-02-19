/*
 *         
 * $Id: portmod_ctrl.c,v 1.2.2.13 Broadcom SDK $
 * 
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 *     
 */
#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/pm4x10.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif 
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X10_SUPPORT


#define PM_ITER_ALL_PHYS(rv, idx) \
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (idx >= 0))

#define PM_ITER_EXT_PHY(rv, idx) \
    while((PHYMOD_E_UNAVAIL== rv) && (idx >= 0))


STATIC int
_pm_tx_fir_pre_set (const phymod_phy_access_t* phy_acc, 
                    int chain_length, uint32 value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_NONE;
    int                  phy_idx = chain_length-1;

    PM_ITER_ALL_PHYS(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_tx_get(pm_phy, &phymod_tx);
        if (rv) continue;

        phymod_tx.main = (value >> 8) & 0xff;
        phymod_tx.post = (value >> 16) & 0xff;
        phymod_tx.pre = value;
        rv = phymod_phy_tx_set(pm_phy, &phymod_tx);
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_tx_fir_main_set (const phymod_phy_access_t* phy_acc, 
                    int chain_length, uint32 value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_NONE;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    PM_ITER_ALL_PHYS(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_tx_get(pm_phy, &phymod_tx);
        if (rv) continue;

        phymod_tx.pre = value & 0xff;
        phymod_tx.post = (value >> 16) & 0xff;
        phymod_tx.main = value;

        rv = phymod_phy_tx_set(pm_phy, &phymod_tx);
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_tx_fir_post_set (const phymod_phy_access_t* phy_acc, 
                    int chain_length, uint32 value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_NONE;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_tx_get(pm_phy, &phymod_tx);
        if (rv) continue;

        phymod_tx.pre = value & 0xff;
        phymod_tx.main = (value >> 8) & 0xff;
        phymod_tx.post = value;

        rv = phymod_phy_tx_set(pm_phy, &phymod_tx);
    }

    return(PHYMOD_E_NONE);
}


STATIC int
_pm_tx_fir_post2_set (const phymod_phy_access_t* phy_acc,
                    int chain_length, uint32 value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_NONE;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.post2 = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_tx_fir_post3_set (const phymod_phy_access_t* phy_acc,
                    int chain_length, uint32 value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_NONE;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.post3 = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_per_lane_preemphasis_set (const phymod_phy_access_t* phy_acc, 
                              int chain_length, phymod_tx_t *ln_txparam, 
                              int lane_map, int lane, uint32 value) 
{
    phymod_phy_access_t  pm_phy_copy, *pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.pre  = value & 0xff;
        phymod_tx.main = (value >> 8) & 0xff;
        phymod_tx.post = (value >> 16) & 0xff;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
   
        ln_txparam[lane].pre = phymod_tx.pre ;
        ln_txparam[lane].main= phymod_tx.main ;
        ln_txparam[lane].post= phymod_tx.post ;
    }

    return(PHYMOD_E_NONE);
}


STATIC int
_pm_preemphasis_set (const phymod_phy_access_t* phy_acc, 
                     int chain_length, phymod_tx_t *ln_txparam, 
                     int lane_map, uint32 value) 
{
    phymod_phy_access_t  *pm_phy;
    phymod_tx_t          phymod_tx; 
    int                  ln, rv = PHYMOD_E_NONE;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);


        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.pre = value & 0xff;
        phymod_tx.main = (value >> 8) & 0xff;
        phymod_tx.post = (value >> 16) & 0xff;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));

        /* CHECK: If we need a loop for all internal phy_accs */ 
        ln = 0; 
        while(lane_map) { 
           if (lane_map & 1) {
               ln_txparam[ln].pre = phymod_tx.pre ;
               ln_txparam[ln].main= phymod_tx.main ;
               ln_txparam[ln].post= phymod_tx.post ;
           }
           ln++;
           lane_map >>= 1;
        }
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_driver_current_set (const phymod_phy_access_t* phy_acc,
                     int chain_length, phymod_tx_t *ln_txparam, 
                     uint32 lane_map, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_tx_t          phymod_tx; 
    int                  rv = PHYMOD_E_NONE;
    int                  ln,  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.amp = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));

        /* CHECK: If we need a loop for all internal phy_accs */ 
        ln = 0; 
        while(lane_map) { 
           if (lane_map & 0x01) 
               ln_txparam[ln].amp = phymod_tx.amp ;
           lane_map >>= 1;
           ln++;
        }  
    }

    return(PHYMOD_E_NONE);
}


STATIC int
_pm_per_lane_driver_current_set (const phymod_phy_access_t* phy_acc,
                     int chain_length, phymod_tx_t *ln_txparam, 
                     uint32 lane_map, int lane, uint32 value)
{
    phymod_phy_access_t  pm_phy_copy, *pm_phy;
    phymod_tx_t          phymod_tx; 
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.amp = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
   
        ln_txparam[lane].amp = value ;
    }

    return(PHYMOD_E_NONE);
}

STATIC
int _pm_sdk_poly_to_phymod_poly(uint32 sdk_poly, uint32 *phymod_poly)
{
    switch(sdk_poly){
        case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
             *phymod_poly = phymodPrbsPoly7;
             break;
        case SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1:
             *phymod_poly = phymodPrbsPoly15;
             break;
        case SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1:
             *phymod_poly = phymodPrbsPoly23;
             break;
        case SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1:
             *phymod_poly = phymodPrbsPoly31;
             break;
        case SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1:
             *phymod_poly = phymodPrbsPoly9;
             break;
        case SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1:
             *phymod_poly = phymodPrbsPoly11;
             break;
        case SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1:
             *phymod_poly = phymodPrbsPoly58;
             break;
        default:
             return SOC_E_INTERNAL;
    }
    return PHYMOD_E_NONE;
}


/* CHECK IF LOOP ON CORE IS NEEDED */
STATIC int
_pm_prbs_tx_poly_set(const phymod_phy_access_t* phy_acc, int chain_length, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t         prbs;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_NONE;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy,  flags, &prbs));
        SOC_IF_ERROR_RETURN(_pm_sdk_poly_to_phymod_poly(value, &prbs.poly));
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    }
    return(PHYMOD_E_NONE);
}

/* CHECK IF LOOP ON CORE IS NEEDED */
STATIC int
_pm_prbs_rx_poly_set(const phymod_phy_access_t* phy_acc, int chain_length, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t         prbs;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_NONE;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));
        SOC_IF_ERROR_RETURN(_pm_sdk_poly_to_phymod_poly(value, &prbs.poly));
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    }
    return(PHYMOD_E_NONE);
}


/* CHECK IF LOOP ON CORE IS NEEDED */
STATIC int
_pm_prbs_tx_invert_data_set(const phymod_phy_access_t* phy_acc, int chain_length, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t         prbs;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_NONE;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));
        prbs.invert = value;
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags,  &prbs));
    }
    return(PHYMOD_E_NONE);
}

/* CHECK IF LOOP ON CORE IS NEEDED */
STATIC int
_pm_prbs_rx_invert_data_set(const phymod_phy_access_t* phy_acc, int chain_length, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t         prbs;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_NONE;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags,  &prbs));
        prbs.invert = value;
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    }
    return(PHYMOD_E_NONE);
}

/* CHECK IF LOOP ON CORE IS NEEDED */
STATIC int
_pm_prbs_tx_enable_set(const phymod_phy_access_t* phy_acc, int chain_length, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_NONE;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(pm_phy, flags, value));
    }
    return(PHYMOD_E_NONE);
}

/* CHECK IF LOOP ON CORE IS NEEDED */
STATIC int
_pm_prbs_rx_enable_set(const phymod_phy_access_t* phy_acc, int chain_length, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_NONE;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(pm_phy, flags,  value));
    }
    return(PHYMOD_E_NONE);
}

STATIC int
_pm_cl72_enable_set(const phymod_phy_access_t* phy_acc, int chain_length, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                   rv = PHYMOD_E_NONE;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_ALL_PHYS(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        SOC_IF_ERROR_RETURN(phymod_phy_cl72_set(pm_phy, value));
    }
    return(PHYMOD_E_NONE);
}

/*
 * Function:
 *      pm4x10_phy_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      PHYMOD_E_NONE
 */
int
pm4x10_phy_control_set (phymod_phy_access_t *phy_access, int chain_len, 
                        soc_phy_control_t type, phymod_tx_t *ln_txparam, 
                        uint32 lane_map, uint32 value)
{
int rv = PHYMOD_E_NONE;

    switch(type) {

        case SOC_PHY_CONTROL_TX_FIR_PRE:
             rv = _pm_tx_fir_pre_set(phy_access, chain_len, value);
             break;

        case SOC_PHY_CONTROL_TX_FIR_MAIN:
             rv = _pm_tx_fir_main_set(phy_access, chain_len, value);
             break;

        case SOC_PHY_CONTROL_TX_FIR_POST:
             rv = _pm_tx_fir_post_set(phy_access, chain_len, value);
             break;

        case SOC_PHY_CONTROL_TX_FIR_POST2:
             rv = _pm_tx_fir_post2_set(phy_access, chain_len, value);
             break;

        case SOC_PHY_CONTROL_TX_FIR_POST3:
             rv = _pm_tx_fir_post3_set(phy_access, chain_len, value);
             break;

        /* PREEMPHASIS */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
             rv = _pm_per_lane_preemphasis_set(phy_access, chain_len, ln_txparam, lane_map, 0, value);
             break;
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
             rv = _pm_per_lane_preemphasis_set(phy_access, chain_len, ln_txparam, lane_map, 1, value);
             break;
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
             rv = _pm_per_lane_preemphasis_set(phy_access, chain_len, ln_txparam, lane_map, 2, value);
             break;
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
             rv = _pm_per_lane_preemphasis_set(phy_access, chain_len, ln_txparam, lane_map, 3, value);
             break;

        case SOC_PHY_CONTROL_PREEMPHASIS:
             rv = _pm_preemphasis_set(phy_access, chain_len, ln_txparam, lane_map, value);
             break;

        /* DRIVER CURRENT */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
             rv = _pm_per_lane_driver_current_set(phy_access, chain_len, ln_txparam, lane_map, 0, value);
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
             rv = _pm_per_lane_driver_current_set(phy_access, chain_len, ln_txparam, lane_map, 1, value);
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
             rv = _pm_per_lane_driver_current_set(phy_access, chain_len, ln_txparam, lane_map, 2, value);
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
             rv = _pm_per_lane_driver_current_set(phy_access, chain_len, ln_txparam, lane_map, 3, value);
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             rv = _pm_driver_current_set(phy_access, chain_len, ln_txparam, lane_map, value);
             break;

        /* decoupled PRBS */
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
             rv = _pm_prbs_tx_poly_set(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
             rv = _pm_prbs_tx_invert_data_set(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
             rv = _pm_prbs_tx_enable_set(phy_access, chain_len, value);
             break; 
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
             rv = _pm_prbs_rx_poly_set(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
             rv = _pm_prbs_rx_invert_data_set(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
             rv = _pm_prbs_rx_enable_set(phy_access, chain_len, value);
             break;
        /* for legacy prbs usage mainly set both tx/rx the same */
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
             rv = _pm_prbs_tx_poly_set(phy_access, chain_len, value);
             rv = _pm_prbs_rx_poly_set(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
             rv = _pm_prbs_tx_invert_data_set(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
             rv = _pm_prbs_tx_enable_set(phy_access, chain_len, value);
             rv = _pm_prbs_rx_enable_set(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
             rv = _pm_prbs_tx_enable_set(phy_access, chain_len, value);
             rv = _pm_prbs_rx_enable_set(phy_access, chain_len, value);
             break;

        case SOC_PHY_CONTROL_CL72:
             rv = _pm_cl72_enable_set(phy_access, chain_len, value);
             break;

#ifdef _TO_BE_IMPLEMENTED_
        case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_VGA:
        case SOC_PHY_CONTROL_RX_TAP1:
        case SOC_PHY_CONTROL_RX_TAP2:
        case SOC_PHY_CONTROL_RX_TAP3:
        case SOC_PHY_CONTROL_RX_TAP4:
        case SOC_PHY_CONTROL_RX_TAP5:
        case SOC_PHY_CONTROL_RX_TAP1_RELEASE:           /* $$$ tbd $$$ */
        case SOC_PHY_CONTROL_RX_TAP2_RELEASE:           /* $$$ tbd $$$ */
        case SOC_PHY_CONTROL_RX_TAP3_RELEASE:           /* $$$ tbd $$$ */
        case SOC_PHY_CONTROL_RX_TAP4_RELEASE:           /* $$$ tbd $$$ */
        case SOC_PHY_CONTROL_RX_TAP5_RELEASE:           /* $$$ tbd $$$ */
        case SOC_PHY_CONTROL_PHASE_INTERP:
        case SOC_PHY_CONTROL_RX_POLARITY:
        case SOC_PHY_CONTROL_TX_POLARITY:
        case SOC_PHY_CONTROL_RX_RESET:
        case SOC_PHY_CONTROL_TX_RESET:
        case SOC_PHY_CONTROL_LANE_SWAP:
        case SOC_PHY_CONTROL_FIRMWARE_MODE:
        case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA7:

        case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        case SOC_PHY_CONTROL_LOOPBACK_PMD:
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        case SOC_PHY_CONTROL_SCRAMBLER:
        case SOC_PHY_CONTROL_8B10B:
        case SOC_PHY_CONTROL_64B66B:

        /* POWER */
        case SOC_PHY_CONTROL_POWER:
        case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        case SOC_PHY_CONTROL_TX_PPM_ADJUST:
        case SOC_PHY_CONTROL_VCO_FREQ:
        case SOC_PHY_CONTROL_PLL_DIVIDER:
        case SOC_PHY_CONTROL_OVERSAMPLE_MODE:
        case SOC_PHY_CONTROL_REF_CLK:
        case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
        case SOC_PHY_CONTROL_DRIVER_SUPPLY:
        case SOC_PHY_CONTROL_EEE:
#ifdef TSC_EEE_SUPPORT
        case SOC_PHY_CONTROL_EEE:
        case SOC_PHY_CONTROL_EEE_AUTO:
#endif
        case SOC_PHY_CONTROL_PCS_SPEED_ST_ENTRY:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_HCD:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CLEAR:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_NUM_OF_LANES:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PLL_DIVIDER:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PLL_DIVIDER:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PMA_OS:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PMA_OS:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_SCR_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_SCR_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_ENCODE_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_ENCODE_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CL48_CHECK_END:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CL48_CHECK_END:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_BLK_SYNC_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_BLK_SYNC_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_REORDER_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_REORDER_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CL36_EN:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CL36_EN:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DESCR1_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DESCR1_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DEC1_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DEC1_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DESKEW_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DESKEW_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DESC2_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DESC2_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DESC2_BYTE_DEL:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DESC2_BYTE_DEL:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_BRCM64B66_DESCR:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_BRCM64B66_DESCR:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_SGMII_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_SGMII_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CLKCNT0:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CLKCNT0:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CLKCNT1:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CLKCNT1:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_LPCNT0:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_LPCNT0:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_LPCNT1:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_LPCNT1:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_MAC_CGC:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_MAC_CGC:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PCS_REPCNT:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PCS_REPCNT:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PCS_CREDTEN:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PCS_CREDTEN:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PCS_CLKCNT:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PCS_CLKCNT:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PCS_CGC:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PCS_CGC:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CL72_EN:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CL72_EN:
#endif    /*_TO_BE_IMPLEMENTED_*/

        /* PRE_DRIVER CURRENT  not supported anymore */
        case SOC_PHY_CONTROL_RX_VGA_RELEASE:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:          /* POST2_DRIVER CURRENT not supported anymore */
        case SOC_PHY_CONTROL_DUMP:
        case SOC_PHY_CONTROL_RX_PLUS1_SLICER:               /* RX SLICER */
        case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
        case SOC_PHY_CONTROL_RX_D_SLICER:
        case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
        case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
        case SOC_PHY_CONTROL_CUSTOM1:
        default:
             rv = PHYMOD_E_UNAVAIL;
             break; 
    }
    return rv;
}


/*
 * Function:
 *      pm4x10_phy_link_mon_enable_set
 * Purpose:
 *      Configure PHY link monitor enable fucntion.
 * Parameters:
 *      phy_access  - phy_access .
 *      chain_len   - phy chain depth.
 *      value       - New setting for the control
 * Returns:
 *      PHYMOD_E_NONE
 */
int
pm4x10_phy_link_mon_enable_set (phymod_phy_access_t *phy_access, int chain_len, uint32 value)
{
int rv = PHYMOD_E_NONE;
uint32_t link_mon_mode = 0;

    if (value == 0) {
        rv = phymod_phy_link_mon_enable_set(phy_access, 0 /*When disable Dont care*/, 0);
    } else {
        link_mon_mode = value - 1;
        rv = phymod_phy_link_mon_enable_set(phy_access, link_mon_mode, 1);
    }
    return rv;
}



STATIC int
_pm_tx_fir_pre_get (const phymod_phy_access_t* phy_acc, 
                    int chain_length, uint32 *value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_tx_get(pm_phy, &phymod_tx);
        if (rv) continue;

        *value = phymod_tx.pre;
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_tx_fir_main_get (const phymod_phy_access_t* phy_acc, 
                    int chain_length, uint32 *value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_tx_get(pm_phy, &phymod_tx);
        if (rv) continue;

        *value = phymod_tx.main;
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_tx_fir_post_get (const phymod_phy_access_t* phy_acc, 
                    int chain_length, uint32 *value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_tx_get(pm_phy, &phymod_tx);
        if (rv) continue;

        *value = phymod_tx.post;
    }

    return(PHYMOD_E_NONE);
}


STATIC int
_pm_tx_fir_post2_get (const phymod_phy_access_t* phy_acc,
                    int chain_length, uint32 *value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_tx_get(pm_phy, &phymod_tx);
        if (rv == PHYMOD_E_NONE)
            *value = phymod_tx.post2;
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_tx_fir_post3_get (const phymod_phy_access_t* phy_acc,
                    int chain_length, uint32 *value)
{
    phymod_phy_access_t* pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_tx_get(pm_phy, &phymod_tx);
        if (rv == PHYMOD_E_NONE)
            *value = phymod_tx.post3;
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_per_lane_preemphasis_get (const phymod_phy_access_t* phy_acc, 
                              int chain_length, phymod_tx_t *ln_txparam, 
                              int lane_map, int lane, uint32 *value) 
{
    phymod_phy_access_t  pm_phy_copy, *pm_phy;
    phymod_tx_t          phymod_tx;
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;

        rv = phymod_phy_tx_get(&pm_phy_copy, &phymod_tx);
        if (rv == PHYMOD_E_NONE)
            *value = phymod_tx.pre  | (phymod_tx.main << 8) | (phymod_tx.post << 16);
    }

    return(PHYMOD_E_NONE);
}



STATIC int
_pm_per_lane_driver_current_get (const phymod_phy_access_t* phy_acc,
                     int chain_length, phymod_tx_t *ln_txparam, 
                     uint32 lane_map, int lane, uint32 *value)
{
    phymod_phy_access_t  pm_phy_copy, *pm_phy;
    phymod_tx_t          phymod_tx; 
    int                  rv = PHYMOD_E_UNAVAIL;
    int                  phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {

        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;

        rv = phymod_phy_tx_get(&pm_phy_copy, &phymod_tx);
        if (rv == PHYMOD_E_NONE)
            *value = phymod_tx.amp;
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_prbs_tx_poly_get(const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t         prbs;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_UNAVAIL;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    
    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        rv = phymod_phy_prbs_config_get(pm_phy, flags, &prbs);
        if (rv == PHYMOD_E_NONE) 
            *value = (int) prbs.poly;
        else
            continue;
    
        /* convert from PHYMOD enum to SDK enum */
        switch(prbs.poly){
            case phymodPrbsPoly7:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
                 break;
            case phymodPrbsPoly9:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
                 break;
            case phymodPrbsPoly15:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
                 break;
            case phymodPrbsPoly23:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
                 break;
            case phymodPrbsPoly31:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
                 break;
            case phymodPrbsPoly11:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
                 break;
            case phymodPrbsPoly58:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1; 
                 break;
            default:
                 return SOC_E_INTERNAL;
        }
    }
    return PHYMOD_E_NONE;
}

STATIC int
_pm_prbs_rx_poly_get(const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t         prbs;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_UNAVAIL;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    
    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        rv = phymod_phy_prbs_config_get(pm_phy, flags, &prbs);
        if (rv == PHYMOD_E_NONE) 
            *value = (int) prbs.poly;
        else
            continue;
    
        /* convert from PHYMOD enum to SDK enum */
        switch(prbs.poly){
            case phymodPrbsPoly7:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
                 break;
            case phymodPrbsPoly9:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
                 break;
            case phymodPrbsPoly15:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
                 break;
            case phymodPrbsPoly23:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
                 break;
            case phymodPrbsPoly31:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
                 break;
            case phymodPrbsPoly11:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
                 break;
            case phymodPrbsPoly58:
                 *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1;
                 break;
            default:
                 return SOC_E_INTERNAL;
        }
    }
    return PHYMOD_E_NONE;
}

STATIC int
_pm_prbs_tx_invert_data_get(const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t         prbs;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_UNAVAIL;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    
    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        rv = phymod_phy_prbs_config_get(pm_phy, flags, &prbs);
        if (rv == PHYMOD_E_NONE) *value = prbs.invert;
    }
    return(PHYMOD_E_NONE);
}

STATIC int
_pm_prbs_rx_invert_data_get(const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t         prbs;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_UNAVAIL;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    
    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        rv = phymod_phy_prbs_config_get(pm_phy, flags, &prbs);
        if (rv == PHYMOD_E_NONE)
            *value = prbs.invert;
    }
    return(PHYMOD_E_NONE);
}


STATIC int
_pm_prbs_tx_enable_get(const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_UNAVAIL;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    
    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        rv = phymod_phy_prbs_enable_get(pm_phy, flags, value);
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_prbs_rx_enable_get(const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t              flags = 0;
    int                   rv = PHYMOD_E_UNAVAIL;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    
    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(pm_phy, flags, value));
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_prbs_rx_status_get(const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_prbs_status_t    prbs_tmp;
    int                     prbs_lock = 0, lock_loss = 0, error_count = 0;
    int                     rv = PHYMOD_E_UNAVAIL;
    int                     phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;

    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        prbs_lock   = 1 ; 
        lock_loss   = 0 ;
        error_count = 0 ;

        /* $$$ Need to add diagnostic API */
        SOC_IF_ERROR_RETURN
                (phymod_phy_prbs_status_get(pm_phy, 0, &prbs_tmp));
    
        if(prbs_tmp.prbs_lock==0) {
                prbs_lock = 0 ;
        } else {
            if(prbs_tmp.prbs_lock_loss) {
                lock_loss = 1 ;
            } else {
                error_count += prbs_tmp.error_count;
            }
        }
        
    }
    if (prbs_lock == 0) {
        *value = -1;
    } else if ((lock_loss == 1) && (prbs_lock == 1)) {
        *value = -2;
    } else {
        *value = error_count;
    }
    return(PHYMOD_E_NONE);
}

STATIC int
_pm_cl72_enable_get (const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_cl72_status_t  status;
    int                   rv = PHYMOD_E_UNAVAIL;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    
    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_cl72_status_get(pm_phy, &status);
        if (rv == PHYMOD_E_NONE)
            *value = status.enabled;       
    }

    return(PHYMOD_E_NONE);
}

STATIC int
_pm_cl72_status_get (const phymod_phy_access_t* phy_acc, int chain_length, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_cl72_status_t  status;
    int                   rv = PHYMOD_E_UNAVAIL;
    int                   phy_idx = chain_length-1;

    if (phy_acc == NULL) return SOC_E_INTERNAL;
    
    PM_ITER_EXT_PHY(rv, phy_idx) {
        pm_phy = (phymod_phy_access_t *)(&phy_acc[phy_idx--]);

        rv = phymod_phy_cl72_status_get(pm_phy, &status);
        if (rv == PHYMOD_E_NONE)
            *value = status.locked;       
    }

    return(PHYMOD_E_NONE);
}

/*
 * Function:
 *      pm4x10_phy_control_get
 * Purpose:
 *      Get current control settings of the PHY. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - (OUT) Current setting for the control 
 * Returns:     
 *      PHYMOD_E_NONE
 */
int
pm4x10_phy_control_get(phymod_phy_access_t *phy_access, int chain_len,
                        soc_phy_control_t type, phymod_tx_t *ln_txparam, 
                        uint32 lane_map, uint32 *value)
{
int rv = PHYMOD_E_NONE;

    switch(type) {

             /* assume they are all the same as lane 0 */
        case SOC_PHY_CONTROL_TX_FIR_PRE:
             rv = _pm_tx_fir_pre_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_TX_FIR_MAIN:
             rv = _pm_tx_fir_main_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_TX_FIR_POST:
             rv = _pm_tx_fir_post_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_TX_FIR_POST2:
             rv = _pm_tx_fir_post2_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_TX_FIR_POST3:
             rv = _pm_tx_fir_post3_get(phy_access, chain_len, value);
             break;

        /* PREEMPHASIS */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
             rv = _pm_per_lane_preemphasis_get(phy_access, chain_len, ln_txparam, lane_map, 0, value);
             break;
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
             rv = _pm_per_lane_preemphasis_get(phy_access, chain_len, ln_txparam, lane_map, 1, value);
             break;
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
             rv = _pm_per_lane_preemphasis_get(phy_access, chain_len, ln_txparam, lane_map, 2, value);
             break;
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
             rv = _pm_per_lane_preemphasis_get(phy_access, chain_len, ln_txparam, lane_map, 3, value);
             break;

        /* DRIVER CURRENT */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
             rv = _pm_per_lane_driver_current_get(phy_access, chain_len, ln_txparam, lane_map,  0, value);
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
             rv = _pm_per_lane_driver_current_get(phy_access, chain_len,  ln_txparam, lane_map, 1, value);
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
             rv = _pm_per_lane_driver_current_get(phy_access, chain_len,  ln_txparam, lane_map, 2, value);
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
             rv = _pm_per_lane_driver_current_get(phy_access, chain_len,  ln_txparam, lane_map, 3, value);
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             rv = _pm_per_lane_driver_current_get(phy_access, chain_len,  ln_txparam, lane_map, 0, value);
             break;

        /* PRBS */
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
             rv = _pm_prbs_tx_poly_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
             rv = _pm_prbs_tx_invert_data_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
             rv = _pm_prbs_tx_enable_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
             rv = _pm_prbs_rx_poly_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
             rv = _pm_prbs_rx_invert_data_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
             rv = _pm_prbs_rx_enable_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
             rv = _pm_prbs_tx_poly_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
             rv = _pm_prbs_tx_invert_data_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
             rv = _pm_prbs_tx_enable_get(phy_access, chain_len, value);
           break;
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
             rv = _pm_prbs_rx_enable_get(phy_access, chain_len, value);
             break;
        case SOC_PHY_CONTROL_PRBS_RX_STATUS:
            rv = _pm_prbs_rx_status_get(phy_access, chain_len, value);
            break;
        case SOC_PHY_CONTROL_CL72:
            rv = _pm_cl72_enable_get(phy_access, chain_len, value);
            break;
        case SOC_PHY_CONTROL_CL72_STATUS:
            rv = _pm_cl72_status_get(phy_access, chain_len, value);
            break;

#ifdef _TO_BE_IMPLEMENTED_
        case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_VGA:
        case SOC_PHY_CONTROL_RX_TAP1:
        case SOC_PHY_CONTROL_RX_TAP2:
        case SOC_PHY_CONTROL_RX_TAP3:
        case SOC_PHY_CONTROL_RX_TAP4:
        case SOC_PHY_CONTROL_RX_TAP5:
        case SOC_PHY_CONTROL_PHASE_INTERP:
        case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
        case SOC_PHY_CONTROL_RX_SEQ_DONE:
        case SOC_PHY_CONTROL_RX_PPM:
        case SOC_PHY_CONTROL_FIRMWARE_MODE:
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        case SOC_PHY_CONTROL_LOOPBACK_PMD:
        case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
        case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
        case SOC_PHY_CONTROL_SCRAMBLER:
        case SOC_PHY_CONTROL_LANE_SWAP:
        case SOC_PHY_CONTROL_EEE:

        case SOC_PHY_CONTROL_PCS_SPEED_ST_ENTRY:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_HCD:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PMA_OS:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PMA_OS:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_SCR_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_SCR_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_ENCODE_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_ENCODE_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CL48_CHECK_END:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CL48_CHECK_END:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_BLK_SYNC_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_BLK_SYNC_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_REORDER_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_REORDER_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CL36_EN:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CL36_EN:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DESCR1_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DESCR1_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DEC1_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DEC1_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DESKEW_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DESKEW_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DESC2_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DESC2_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_DESC2_BYTE_DEL:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_DESC2_BYTE_DEL:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_BRCM64B66_DESCR:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_BRCM64B66_DESCR:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_SGMII_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_SGMII_MODE:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CLKCNT0:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CLKCNT0:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CLKCNT1:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CLKCNT1:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_LPCNT0:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_LPCNT0:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_LPCNT1:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_LPCNT1:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_MAC_CGC:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_MAC_CGC:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PCS_REPCNT:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PCS_REPCNT:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PCS_CREDTEN:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PCS_CREDTEN:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PCS_CLKCNT:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PCS_CLKCNT:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_PCS_CGC:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_PCS_CGC:
        case SOC_PHY_CONTROL_PCS_SPEED_HTO_CL72_EN:
        case SOC_PHY_CONTROL_PCS_SPEED_ST_CL72_EN:
#endif  /* TO_BE_IMPLEMENTED */


        case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
        case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
        case SOC_PHY_CONTROL_RX_D_SLICER:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT: /* POST2_DRIVER CURRENT */
        /* BERT */
        case SOC_PHY_CONTROL_BERT_TX_PACKETS:           /* fall through */
        case SOC_PHY_CONTROL_BERT_RX_PACKETS:           /* fall through */
        case SOC_PHY_CONTROL_BERT_RX_ERROR_BITS:        /* fall through */
        case SOC_PHY_CONTROL_BERT_RX_ERROR_BYTES:       /* fall through */
        case SOC_PHY_CONTROL_BERT_RX_ERROR_PACKETS:     /* fall through */
        case SOC_PHY_CONTROL_BERT_PATTERN:              /* fall through */
        case SOC_PHY_CONTROL_BERT_PACKET_SIZE:          /* fall through */
        case SOC_PHY_CONTROL_BERT_IPG:                  /* fall through */
        case SOC_PHY_CONTROL_CUSTOM1:
        case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
        default:
             rv = PHYMOD_E_UNAVAIL;
             break; 
    }
    return rv;
}     

/*
 * Function:
 *      pm4x10_phy_link_mon_enable_set
 * Purpose:
 *      Configure PHY link monitor enable fucntion.
 * Parameters:
 *      phy_access  - phy_access .
 *      chain_len   - phy chain depth.
 * Returns:
 *      PHYMOD_E_NONE
 */
int
pm4x10_phy_link_mon_status_get (phymod_phy_access_t *phy_access, int chain_len)
{
int rv = PHYMOD_E_NONE;
uint32_t lock_status = 0;
uint32_t  lock_lost_lh = 0;
uint32_t  error_count = 0;


    rv = phymod_phy_link_mon_status_get(phy_access, &lock_status, &lock_lost_lh, &error_count);

    if (rv == PHYMOD_E_NONE) {
        LOG_CLI((BSL_META_U(0,"Lane Map:0x%x Lock Status:%d lock lost:%d Error count:%d\n "),
            phy_access->access.lane_mask, lock_status, lock_lost_lh, error_count));
    }
    return rv;
}

#endif /* PORTMOD_PM4X10_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
