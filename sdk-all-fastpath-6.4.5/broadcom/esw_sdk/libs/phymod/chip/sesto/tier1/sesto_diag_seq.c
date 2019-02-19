/*
* $Copyright: Copyright 2012 Broadcom Corporation.
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

/*
 *         
 * 
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

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include "sesto_address_defines.h"
#include "sesto_reg_structs.h"
#include "sesto_cfg_seq.h"
#include "sesto_diag_seq.h"
#include "sesto_serdes/falcon_furia_sesto_src/falcon_furia_sesto_functions.h"
#include "sesto_serdes/merlin_sesto_src/merlin_sesto_functions.h"
#include "sesto_serdes/common/srds_api_enum.h"

int _sesto_phymod_prbs_poly_to_serdes_prbs_poly(uint16_t phymod_poly, uint16_t *serdes_poly)
{
    switch (phymod_poly) {
        case phymodPrbsPoly7:
            *serdes_poly = PRBS_7;
        break;
        case phymodPrbsPoly9:
            *serdes_poly = PRBS_9;
        break;
        case phymodPrbsPoly11:
            *serdes_poly = PRBS_11;
        break;
        case phymodPrbsPoly15:
            *serdes_poly = PRBS_15;
        break;
        case phymodPrbsPoly23:
            *serdes_poly = PRBS_23;
        break;
        case phymodPrbsPoly31:
            *serdes_poly = PRBS_31;
        break;
        case phymodPrbsPoly58:
            *serdes_poly = PRBS_58;
        break;
        default: 
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}
int _sesto_serdes_prbs_poly_to_phymod_prbs_poly(uint16_t serdes_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch (serdes_poly) {
        case PRBS_7:
            *phymod_poly = phymodPrbsPoly7;
        break; 
        case PRBS_9:
            *phymod_poly = phymodPrbsPoly9;
        break; 
        case PRBS_11:
            *phymod_poly = phymodPrbsPoly11;
        break; 
        case PRBS_15:
            *phymod_poly = phymodPrbsPoly15;
        break; 
        case PRBS_23:
            *phymod_poly = phymodPrbsPoly23;
        break; 
        case PRBS_31:
            *phymod_poly = phymodPrbsPoly31;
        break; 
        case PRBS_58:
            *phymod_poly = phymodPrbsPoly58;
        break; 
        default: 
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int _sesto_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0, cast_type = 0, mcast_val = 0;
    uint16_t prbs_poly = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(pa, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(pa, config, ip);
    PHYMOD_IF_ERR_RETURN(
          _sesto_phymod_prbs_poly_to_serdes_prbs_poly (prbs->poly, &prbs_poly));

    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    
    SESTO_GET_CORE_SLICE_INFO(pa, ip, config, &cast_type, &mcast_val);

    PHYMOD_DEBUG_VERBOSE(("IP:%s Max_lane:%d lanemask:0x%x cast Type:%x\n", 
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask, cast_type));

    if (cast_type != SESTO_CAST_INVALID) {
        for (lane = 0; lane < max_lane; lane ++) {
            if (lane_mask & (1 << lane)) {
                PHYMOD_IF_ERR_RETURN (
                  _sesto_set_slice_reg (pa, cast_type, ip,
                              SESTO_DEV_PMA_PMD, mcast_val, lane));
                if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
                    if (ip == SESTO_FALCON_CORE) {
                        PHYMOD_IF_ERR_RETURN(
                            falcon_furia_sesto_config_tx_prbs(pa, prbs_poly, (uint8_t)prbs->invert)); 
                    } else {
                        PHYMOD_IF_ERR_RETURN(
                            merlin_sesto_config_tx_prbs(pa, prbs_poly, (uint8_t)prbs->invert)); 
                    }
                }
                if (PHYMOD_PRBS_DIRECTION_RX_GET(flags) || flags == 0) {
                    /* Enabling PRBS CHECKER SELF SYNC HYS*/
                    if (ip == SESTO_FALCON_CORE) {
                        PHYMOD_IF_ERR_RETURN(
                                falcon_furia_sesto_config_rx_prbs(pa, prbs_poly, 0, 
                                (uint8_t)prbs->invert));
                    } else {
                        PHYMOD_IF_ERR_RETURN(
                            merlin_sesto_config_rx_prbs(pa, prbs_poly, 0, 
                                (uint8_t)prbs->invert)); 
                    }
                }
                if (SESTO_IS_MULTI_BROAD_CAST_SET(cast_type)) {
                    break;
                }
            }
        }
    } else {
        PHYMOD_DEBUG_VERBOSE(("Lane mask not Fine \n"));
        return PHYMOD_E_PARAM;
    }

    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);

    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _sesto_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, dig_lpbk_en = 0;
    uint16_t lane_mask = 0, cast_type = 0, mcast_val = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(pa, 0, &config));

    SESTO_GET_IP(pa, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_CORE_SLICE_INFO(pa, ip, config, &cast_type, &mcast_val);

    PHYMOD_DEBUG_VERBOSE(("IP:%s Max_lane:%d lanemask:0x%x cast type:%d\n", 
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask, cast_type));

    if (cast_type != SESTO_CAST_INVALID) {
        for (lane = 0; lane < max_lane; lane ++) {
            if (lane_mask & (1 << lane)) {
                PHYMOD_IF_ERR_RETURN (
                  _sesto_set_slice_reg (pa, cast_type, ip,
                              SESTO_DEV_PMA_PMD, mcast_val, lane));
               if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
                   if (ip == SESTO_FALCON_CORE) {
                       PHYMOD_IF_ERR_RETURN(
                           falcon_furia_sesto_tx_prbs_en(pa, enable));
                   } else {
                       PHYMOD_IF_ERR_RETURN(
                            merlin_sesto_tx_prbs_en(pa, enable));
                   }
               }
               if (PHYMOD_PRBS_DIRECTION_RX_GET(flags) || flags == 0) {
                    /* Enabling PRBS CHECKER SELF SYNC HYS*/
                   if (ip == SESTO_FALCON_CORE) {
                       if (enable) {
                           PHYMOD_IF_ERR_RETURN (
	                          wr_falcon_furia_sesto_prbs_chk_clk_en_frc_on(1));
                           /* Check for digital loopback enable or not */
                           SESTO_CHIP_FIELD_READ(pa, F25G_TLB_RX_DIG_LPBK_CONFIG,
                                      dig_lpbk_en, dig_lpbk_en);
                           /* Do not enable prbs_chk_en_auto_mode when digital lpbk is set */
                           if (!dig_lpbk_en) {
                               PHYMOD_IF_ERR_RETURN (
                                  wr_falcon_furia_sesto_prbs_chk_en_auto_mode(1));
                           }
                       }
                       PHYMOD_IF_ERR_RETURN(
                            falcon_furia_sesto_rx_prbs_en(pa, enable));
                       if (!enable) {
                           PHYMOD_IF_ERR_RETURN (
	                          wr_falcon_furia_sesto_prbs_chk_clk_en_frc_on(0));
                           PHYMOD_IF_ERR_RETURN (
                              wr_falcon_furia_sesto_prbs_chk_en_auto_mode(0));
                       }
                   } else {
                       if (enable) {
                           PHYMOD_IF_ERR_RETURN (
                                wr_prbs_chk_clk_en_frc_on(1));
                           /* Check for digital loopback enable or not */
                           SESTO_CHIP_FIELD_READ(pa, M10G_TLB_RX_DIG_LPBK_CONFIG,
                                      dig_lpbk_en, dig_lpbk_en);
                           /* Do not enable prbs_chk_en_auto_mode when digital lpbk is set */
                           if (!dig_lpbk_en) {
                               PHYMOD_IF_ERR_RETURN (
                                    wr_prbs_chk_en_auto_mode(1));
                           } 
                       }
                       PHYMOD_IF_ERR_RETURN (
                            merlin_sesto_rx_prbs_en(pa, enable));
                       if (!enable) {
                           PHYMOD_IF_ERR_RETURN (
	                          wr_prbs_chk_clk_en_frc_on(0));
                           PHYMOD_IF_ERR_RETURN (
                                wr_prbs_chk_en_auto_mode(0));
                       }
                   }
               }
               if (SESTO_IS_MULTI_BROAD_CAST_SET(cast_type)) {
                   break;
               }
            }
        }
    } else {
        PHYMOD_DEBUG_VERBOSE(("Lane mask not Fine \n"));
        return PHYMOD_E_PARAM;
    }

    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);

    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _sesto_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t *enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, lane_mask = 0;
    uint8_t gen_en = 0, ckr_en = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(pa, 0, &config));

    SESTO_GET_IP(pa, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s :: IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                     SESTO_DEV_PMA_PMD, 0, lane));
            if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
                if (ip == SESTO_FALCON_CORE) {
                    PHYMOD_IF_ERR_RETURN(
                          falcon_furia_sesto_get_tx_prbs_en(pa, &gen_en));
                    *enable = gen_en;
                } else {
                    PHYMOD_IF_ERR_RETURN(
                          merlin_sesto_get_tx_prbs_en(pa, &gen_en));
                    *enable = gen_en;
                }
            }
            if (flags == 0 || PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
                if (ip == SESTO_FALCON_CORE) {
                    PHYMOD_IF_ERR_RETURN(
                            falcon_furia_sesto_get_rx_prbs_en(pa, &ckr_en));
                    *enable = ckr_en;
                } else {
                    PHYMOD_IF_ERR_RETURN(
                          merlin_sesto_get_rx_prbs_en(pa, &ckr_en));
                    *enable = ckr_en;
                }
            }
            break;
        }
    }
    if (flags == 0) {
        *enable = ckr_en & gen_en;
    } 
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _sesto_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, lane_mask;
    uint8_t data = 0;
    enum srds_prbs_checker_mode_enum temp = 0;
    enum srds_prbs_polynomial_enum serdes_poly = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(pa, 0, &config));

    SESTO_GET_IP(pa, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("IP:%s Max_lane:%d lanemask:0x%x\n", 
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
           PHYMOD_IF_ERR_RETURN (
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                   SESTO_DEV_PMA_PMD, 0, lane));

            if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
                if (ip == SESTO_FALCON_CORE) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_sesto_get_tx_prbs_config(pa, &serdes_poly, &data));
                    prbs->invert = data;                                          
                } else { 
                    PHYMOD_IF_ERR_RETURN(merlin_sesto_get_tx_prbs_config(pa, &serdes_poly, &data));
                    prbs->invert = data;                                          
                }                                                                 
            }                                                                     
            if (flags == 0 || PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {              
                if (ip == SESTO_FALCON_CORE) {                                    
                    PHYMOD_IF_ERR_RETURN(falcon_furia_sesto_get_rx_prbs_config(pa, &serdes_poly, &temp, &data));
                    prbs->invert = data;                                          
                } else {
                    PHYMOD_IF_ERR_RETURN(merlin_sesto_get_rx_prbs_config(pa, &serdes_poly, &temp, &data));
                    prbs->invert = data;
                }
            }
            break;
        }
    }

    PHYMOD_DEBUG_VERBOSE(("Checker POLY:%d\n", serdes_poly));
    PHYMOD_IF_ERR_RETURN (
        _sesto_serdes_prbs_poly_to_phymod_prbs_poly(serdes_poly, &prbs->poly));

    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);

    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _sesto_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, lane_mask = 0;
    uint8_t data = 0;
    uint32_t err_cnt = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(pa, 0, &config));

    SESTO_GET_IP(pa, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n",  __func__,
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));
    prbs_status->prbs_lock = 1;
    prbs_status->prbs_lock_loss = 1;
    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                     SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                PHYMOD_IF_ERR_RETURN(falcon_furia_sesto_prbs_chk_lock_state(pa, &data));
                prbs_status->prbs_lock &= data;
                PHYMOD_IF_ERR_RETURN(falcon_furia_sesto_prbs_err_count_state(pa, &err_cnt, &data));
                prbs_status->prbs_lock_loss &= data;
                prbs_status->error_count |= err_cnt;
            } else {
                PHYMOD_IF_ERR_RETURN(merlin_sesto_prbs_chk_lock_state(pa, &data));
                prbs_status->prbs_lock &= data;
                PHYMOD_IF_ERR_RETURN(merlin_sesto_prbs_err_count_state(pa, &err_cnt, &data));
                prbs_status->prbs_lock_loss &= data;
                prbs_status->error_count |= err_cnt;
            }
            READ_SESTO_PMA_PMD_REG(pa, 0x8000, data);
        }
    }

    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _sesto_phy_display_eyescan(const phymod_access_t *pa)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(pa, 0, &config));

    SESTO_GET_IP(pa, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                     SESTO_DEV_PMA_PMD, 0, lane));
            
            /* set the core to get the master /slave */
            /* System side always master Line side considered as slave*/
            if (ip == SESTO_FALCON_CORE) {
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_sesto_display_lane_state_hdr(pa));
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_sesto_display_lane_state(pa)); 
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_sesto_display_eye_scan(pa));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (merlin_sesto_display_lane_state_hdr(pa));
                PHYMOD_IF_ERR_RETURN
                    (merlin_sesto_display_lane_state(pa)); 
                PHYMOD_IF_ERR_RETURN
                    (merlin_sesto_display_eye_scan(pa));
            }
        }
    }
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

int _sesto_phy_diagnostics_get(const phymod_access_t *pa, phymod_phy_diagnostics_t* diag)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(pa, 0, &config));

    SESTO_GET_IP(pa, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                     SESTO_DEV_PMA_PMD, 0, lane));
            PHYMOD_DEBUG_VERBOSE(("Phy Diagnostics for Lane:%d \n",lane));
            /* set the core to get the master /slave */
            /* System side always master Line side considered as slave*/
            if (ip == SESTO_FALCON_CORE) {
                falcon_furia_sesto_lane_state_define_st state;
                PHYMOD_MEMSET(&state, 0, sizeof(falcon_furia_sesto_lane_state_define_st));
                PHYMOD_IF_ERR_RETURN
                        (_falcon_furia_sesto_read_lane_state_define(pa, &state));
                diag->signal_detect = state.sig_det;
                diag->osr_mode = state.osr_mode.tx_rx;
                diag->rx_lock = state.rx_lock;
                diag->tx_ppm = state.tx_ppm;
                diag->clk90_offset = state.clk90;
                diag->clkp1_offset = state.clkp1;
                diag->p1_lvl = state.p1_lvl;
                diag->dfe1_dcd = state.dfe1_dcd;
                diag->dfe2_dcd = state.dfe2_dcd;
                diag->slicer_offset.offset_pe = state.pe;
                diag->slicer_offset.offset_ze = state.ze;
                diag->slicer_offset.offset_me = state.me;
                diag->slicer_offset.offset_po = state.po;
                diag->slicer_offset.offset_zo = state.zo;
                diag->slicer_offset.offset_mo = state.mo;
                diag->eyescan.heye_left = state.heye_left;
                diag->eyescan.heye_right = state.heye_right;
                diag->eyescan.veye_upper = state.veye_upper;
                diag->eyescan.veye_lower = state.veye_lower;
                diag->link_time = state.link_time;
                diag->pf_main = state.pf_main;
                diag->pf_hiz = state.pf_hiz;
                diag->pf_bst = state.pf_bst;
                diag->pf_low = 0; /* Not Availble in serdes API*/
                diag->pf2_ctrl = state.pf2_ctrl;
                diag->vga = state.vga;
                diag->dc_offset = state.dc_offset;
                diag->p1_lvl_ctrl = state.p1_lvl_ctrl;
                diag->dfe1 = state.dfe1;
                diag->dfe2 = state.dfe2;
                diag->dfe3 = state.dfe3;
                diag->dfe4 = state.dfe4;
                diag->dfe5 = state.dfe5;
                diag->dfe6 = state.dfe6;
                diag->txfir_pre = state.txfir_pre;
                diag->txfir_main = state.txfir_main;
                diag->txfir_post1 = state.txfir_post1;
                diag->txfir_post2 = state.txfir_post2;
                diag->txfir_post3 = state.txfir_post3;
                diag->tx_amp_ctrl = 0; /* Not Availble in serdes API*/
                diag->br_pd_en = state.br_pd_en;
                break;
            } else {
                merlin_sesto_lane_state_define_st state;
                PHYMOD_IF_ERR_RETURN
                    (_merlin_sesto_read_lane_state_define(pa, &state));
                diag->signal_detect = state.sig_det;
                diag->osr_mode = state.osr_mode.tx_rx;
                diag->rx_lock = state.rx_lock;
                diag->tx_ppm = state.tx_ppm;
                diag->clk90_offset = state.clk90;
                diag->clkp1_offset = state.clkp1;
                diag->p1_lvl = state.p1_lvl;
                diag->dfe1_dcd = state.dfe1_dcd;
                diag->dfe2_dcd = state.dfe2_dcd;
                diag->slicer_offset.offset_pe = state.pe;
                diag->slicer_offset.offset_ze = state.ze;
                diag->slicer_offset.offset_me = state.me;
                diag->slicer_offset.offset_po = state.po;
                diag->slicer_offset.offset_zo = state.zo;
                diag->slicer_offset.offset_mo = state.mo;
                diag->eyescan.heye_left = state.heye_left;
                diag->eyescan.heye_right = state.heye_right;
                diag->eyescan.veye_upper = state.veye_upper;
                diag->eyescan.veye_lower = state.veye_lower;
                diag->link_time = state.link_time;
                diag->pf_main = state.pf_main;
                diag->pf_hiz = state.pf_hiz;
                diag->pf_bst = state.pf_bst;
                diag->pf_low = 0; /* Not Availble in serdes API*/
                diag->pf2_ctrl = state.pf2_ctrl;
                diag->vga = state.vga;
                diag->dc_offset = state.dc_offset;
                diag->p1_lvl_ctrl = state.p1_lvl_ctrl;
                diag->dfe1 = state.dfe1;
                diag->dfe2 = state.dfe2;
                diag->dfe3 = state.dfe3;
                diag->dfe4 = state.dfe4;
                diag->dfe5 = state.dfe5;
                diag->dfe6 = state.dfe6;
                diag->txfir_pre = state.txfir_pre;
                diag->txfir_main = state.txfir_main;
                diag->txfir_post1 = state.txfir_post1;
                diag->txfir_post2 = state.txfir_post2;
                diag->txfir_post3 = state.txfir_post3;
                diag->tx_amp_ctrl = 0; /* Not Availble in serdes API*/
                diag->br_pd_en = state.br_pd_en;
                break;
            }
        }
    }
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}
