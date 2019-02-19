/*
 * $Id: phymod_ctrl.c,v 1.1.2.7 Broadcom SDK $
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
 *
 * File:        phymod_ctrl.c
 * Purpose:     Interface functions for PHYMOD
 */

#ifdef PHYMOD_SUPPORT

#include <soc/phy/phymod_port_control.h>
#include <soc/error.h>

#define LANE_MAP_NOF_LANES (4)

typedef int (*control_set_handler_f)(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2);
typedef int (*control_get_handler_f)(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value);


int
soc_prbs_poly_to_phymod(uint32 sdk_poly, phymod_prbs_poly_t *phymod_poly){
    switch(sdk_poly){
    case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1:
        *phymod_poly = phymodPrbsPoly11;
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
    case SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1:
        *phymod_poly = phymodPrbsPoly58;
        break;
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}


int
phymod_prbs_poly_to_soc(phymod_prbs_poly_t phymod_poly, uint32 *sdk_poly){
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
        break;
    case phymodPrbsPoly9:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
        break;
    case phymodPrbsPoly15:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
        break;
    case phymodPrbsPoly23:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
        break;
    case phymodPrbsPoly31:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
        break;
    case phymodPrbsPoly58:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/****************************************************************************** 
  Set controls handlers
*******************************************************************************/
STATIC int
control_handler_preemphasis_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.pre = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.pre = phymod_tx_def.pre;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));

    

    return SOC_E_NONE;
}
STATIC int
control_handler_tx_fir_pre_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.pre = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.pre = phymod_tx_def.pre;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_main_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.main = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.main = phymod_tx_def.main;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.post = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.post = phymod_tx_def.post;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post2_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.post2 = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.post2 = phymod_tx_def.post2;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post3_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.post3 = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.post3 = phymod_tx_def.post3;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_driver_current_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.amp = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.amp = phymod_tx_def.amp;
    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_rx_tap_release(int unit, phymod_phy_access_t *phy, uint32 tap, uint32 value2)
{
    phymod_rx_t phymod_rx;

    /* bounds check "tap" */
    if (tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.dfe[tap].enable = FALSE;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_tap_set(int unit, phymod_phy_access_t *phy, uint32 value, uint32 tap)
{
    phymod_rx_t phymod_rx;

    /* bounds check "tap" */
    if (tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.dfe[tap].enable = FALSE;
    phymod_rx.dfe[tap].value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}


STATIC int 
control_handler_pi_control_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_override_t tx_override;

    phymod_tx_override_t_init(&tx_override);
    tx_override.phase_interpolator.enable = (value1 == 0) ? 0 : 1;
    tx_override.phase_interpolator.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_override_set(phy, &tx_override));

    return SOC_E_NONE;
}


STATIC int
control_handler_tx_squelch(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_power_t  phy_power;

    phymod_phy_power_t_init(&phy_power);
    if (value1 == 1) {
        phy_power.tx = phymodPowerOff;
        phy_power.rx = phymodPowerNoChange;
    } else {
        phy_power.tx = phymodPowerOn;
        phy_power.rx = phymodPowerNoChange;
    }
    SOC_IF_ERROR_RETURN
        (phymod_phy_power_set(phy, &phy_power));
    return SOC_E_NONE;
}


STATIC int
control_handler_rx_peak_filter_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_rx_t phymod_rx;


    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.peaking_filter.enable = TRUE;
    phymod_rx.peaking_filter.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_vga_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.vga.enable = TRUE;
    phymod_rx.vga.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}


STATIC int 
control_handler_prbs_poly_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(soc_prbs_poly_to_phymod(value1, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_poly_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(soc_prbs_poly_to_phymod(value1, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int 
control_handler_prbs_rx_poly_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(soc_prbs_poly_to_phymod(value1, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_invert_data_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    prbs.invert = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_rx_invert_data_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    prbs.invert = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_enable_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(phy, flags, value1));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_rx_enable_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(phy, flags, value1));
    return SOC_E_NONE;
}


STATIC int
control_handler_rx_reset_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_reset_t reset;

    SOC_IF_ERROR_RETURN(phymod_phy_reset_get(phy, &reset));
    reset.rx = (value1 == 0)?  phymodResetDirectionOut : phymodResetDirectionIn;
    SOC_IF_ERROR_RETURN(phymod_phy_reset_set(phy, &reset));

    return SOC_E_NONE;
}


STATIC int
control_handler_tx_reset_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_reset_t reset;

    SOC_IF_ERROR_RETURN(phymod_phy_reset_get(phy, &reset));
    reset.tx = (value1 == 0)?  phymodResetDirectionOut : phymodResetDirectionIn;
    SOC_IF_ERROR_RETURN(phymod_phy_reset_set(phy, &reset));

    return SOC_E_NONE;
}


STATIC int
control_handler_cl72_enable_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_set(phy, value1));

    return SOC_E_NONE;
}


STATIC int
control_handler_lane_map_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_core_access_t core;
    phymod_lane_map_t lane_map;
    uint32 idx;

    core.type = phy->type;
    sal_memcpy(&core.access, &phy->access, sizeof(core.access));
    core.access.lane_mask = 0;

    lane_map.num_of_lanes = LANE_MAP_NOF_LANES;
    for (idx=0; idx < LANE_MAP_NOF_LANES; idx++) {
        lane_map.lane_map_rx[idx] = (value1 >> (idx * 4 /*4 bit per lane*/)) & 0xf;
    }
    for (idx=0; idx < LANE_MAP_NOF_LANES; idx++) {
        lane_map.lane_map_tx[idx] = (value1 >> (16 + idx * 4 /*4 bit per lane*/)) & 0xf;
    }
    SOC_IF_ERROR_RETURN(phymod_core_lane_map_set(&core, &lane_map));
    return SOC_E_NONE;
}


STATIC int 
control_handler_loopback_internal_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(phy, phymodLoopbackGlobal, value1));

    return SOC_E_NONE;
}


STATIC int 
control_handler_loopback_remote_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(phy, phymodLoopbackRemotePMD, value1));

    return SOC_E_NONE;
}


STATIC int 
control_handler_loopback_remote_pcs_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(phy, phymodLoopbackRemotePCS, value1));

    return SOC_E_NONE;
}


STATIC int
control_handler_power_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_power_t      power;

    phymod_phy_power_t_init(&power);
    if (value1) {
        power.tx = phymodPowerOn;
        power.rx = phymodPowerOn;
    } 
    else {
        power.tx = phymodPowerOff;
        power.rx = phymodPowerOff;
    }
     SOC_IF_ERROR_RETURN(phymod_phy_power_set(phy, &power));

    return SOC_E_NONE;
}


STATIC int
control_handler_rx_low_freq_filter_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.low_freq_peaking_filter.enable = TRUE;
    phymod_rx.low_freq_peaking_filter.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}




STATIC int
control_handler_firmware_mode_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_firmware_lane_config_t fw_config;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &fw_config));

    
    switch (value1) {
        case SOC_PHY_FIRMWARE_DEFAULT: 
            fw_config.LaneConfigFromPCS = 1;
            break;
        case SOC_PHY_FIRMWARE_FORCE_OSDFE:
            fw_config.DfeOn = 1;
            break;
        case SOC_PHY_FIRMWARE_FORCE_BRDFE:
            fw_config.ForceBrDfe = 1;
            break;
        case SOC_PHY_FIRMWARE_CL72_WITHOUT_AN:
            fw_config.Cl72Enable = 1;
            break;
        case SOC_PHY_FIRMWARE_SFP_DAC:
            fw_config.MediaType = phymodFirmwareMediaTypeCopperCable;
            break;
        case SOC_PHY_FIRMWARE_XLAUI:
            fw_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
            break;
        case SOC_PHY_FIRMWARE_SFP_OPT_SR4:
            fw_config.MediaType = phymodFirmwareMediaTypeOptics;
            break;
        default:
            fw_config.LaneConfigFromPCS = 1;
            break;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(phy, fw_config));

    return SOC_E_NONE;
}


STATIC int
control_handler_rx_seq_toggle_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN(phymod_phy_rx_restart(phy));
    return SOC_E_NONE;
}


STATIC int
control_handler_scrambler_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 ref_clk)
{
    phymod_phy_inf_config_t config;

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, ref_clk, &config));
    PHYMOD_INTF_MODES_SCR_SET(&config);
    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(phy, PHYMOD_INTF_F_DONT_OVERIDE_TX_PARAMS, &config));

    return SOC_E_NONE;
}


STATIC int 
control_handler_tx_polarity_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_polarity_t    polarity;

    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(phy, &polarity));
    polarity.tx_polarity = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(phy, &polarity));

    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_polarity_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_polarity_t    polarity;

    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(phy, &polarity));
    polarity.rx_polarity = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(phy, &polarity));

    return SOC_E_NONE;
}

/****************************************************************************** 
 Get controls handlers
*******************************************************************************/
STATIC int
control_handler_preemphasis_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.pre;
    

    return SOC_E_NONE;
}
STATIC int
control_handler_tx_fir_pre_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.pre;
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_main_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.main;
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.post;
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post2_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.post2;
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post3_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.post3;
    return SOC_E_NONE;
}



STATIC int
control_handler_driver_current_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.amp;
    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_tap_get(int unit, phymod_phy_access_t *phy, uint32 tap, uint32 *value)
{
    phymod_rx_t phymod_rx;

    /* bounds check "tap" */
    if (tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    *value = phymod_rx.dfe[tap].value;

    return SOC_E_NONE;
}


STATIC int 
control_handler_pi_control_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_override_t tx_override;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_override_get(phy, &tx_override));
    *value = tx_override.phase_interpolator.value;

    return SOC_E_NONE;
}


STATIC int
control_handler_tx_squelch_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_phy_power_t  phy_power;

    SOC_IF_ERROR_RETURN(phymod_phy_power_get(phy, &phy_power));
    *value = (phy_power.tx == phymodPowerOn)? 1 : 0;

    return SOC_E_NONE;
}


STATIC int
control_handler_rx_peak_filter_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    *value = phymod_rx.peaking_filter.value;

    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_vga_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    *value = phymod_rx.vga.value;

    return SOC_E_NONE;
}


STATIC int 
control_handler_prbs_poly_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phymod_prbs_poly_to_soc(prbs.poly, value));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_poly_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phymod_prbs_poly_to_soc(prbs.poly, value));
    return SOC_E_NONE;
}


STATIC int 
control_handler_prbs_rx_poly_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phymod_prbs_poly_to_soc(prbs.poly, value));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_invert_data_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    *value = prbs.invert;
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_rx_invert_data_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    *value = prbs.invert;
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_enable_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(phy, flags, value));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_rx_enable_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(phy, flags, value));
    return SOC_E_NONE;
}



STATIC int
control_handler_prbs_rx_status_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t flags = PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ;
    phymod_prbs_status_t status;

    SOC_IF_ERROR_RETURN(phymod_phy_prbs_status_get(phy, flags, &status));
    if(status.prbs_lock_loss){
        *value = -2;
    } else if(status.prbs_lock){
        *value = -1;
    } else{
        *value = status.error_count;
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_cl72_enable_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_get(phy, value));

    return SOC_E_NONE;
}


STATIC int
control_handler_cl72_status_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_cl72_status_t status;
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_status_get(phy, &status));
    *value = status.locked;

    return SOC_E_NONE;
}


STATIC int
control_handler_lane_map_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_core_access_t core;
    phymod_lane_map_t lane_map;
    uint32 idx;

    core.type = phy->type;
    sal_memcpy(&core.access, &phy->access, sizeof(core.access));
    core.access.lane_mask = 0;
    *value = 0;
    sal_memset(&lane_map, 0, sizeof(phymod_lane_map_t));

    SOC_IF_ERROR_RETURN(phymod_core_lane_map_get(&core, &lane_map));
    if(lane_map.num_of_lanes != LANE_MAP_NOF_LANES){
        return SOC_E_INTERNAL;
    }
    for (idx=0; idx < LANE_MAP_NOF_LANES; idx++) {
        *value += ((lane_map.lane_map_rx[idx] & 0xf) << (idx * 4));
    }
    for (idx=0; idx < LANE_MAP_NOF_LANES; idx++) {
        *value += ((lane_map.lane_map_tx[idx] & 0xf)<< (idx * 4 + 16));
    }
    
    return SOC_E_NONE;
}



STATIC int 
control_handler_loopback_internal_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(phy, phymodLoopbackGlobal, value));

    return SOC_E_NONE;
}


STATIC int 
control_handler_loopback_remote_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(phy, phymodLoopbackRemotePMD, value));

    return SOC_E_NONE;
}


STATIC int 
control_handler_loopback_remote_pcs_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(phy, phymodLoopbackRemotePCS, value));

    return SOC_E_NONE;
}


STATIC int
control_handler_rx_low_freq_filter_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    *value = phymod_rx.low_freq_peaking_filter.value;

    return SOC_E_NONE;
}


STATIC int
control_handler_scrambler_get(int unit, phymod_phy_access_t *phy, uint32 ref_clk, uint32 *value)
{
    phymod_phy_inf_config_t config;

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, ref_clk, &config));
    *value = PHYMOD_INTF_MODES_SCR_GET(&config); 

    return SOC_E_NONE;
}

STATIC int
control_handler_firmware_mode_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_firmware_lane_config_t fw_config;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &fw_config));

    if (fw_config.LaneConfigFromPCS) {
        *value = SOC_PHY_FIRMWARE_DEFAULT;
    } else if (fw_config.DfeOn) {
        *value = SOC_PHY_FIRMWARE_FORCE_OSDFE;
    } else if (fw_config.ForceBrDfe) {
        *value = SOC_PHY_FIRMWARE_FORCE_BRDFE;
    } else if (fw_config.Cl72Enable) {
        *value = SOC_PHY_FIRMWARE_CL72_WITHOUT_AN;
    } else {
        /*leave it blank for now */
    }                      

    return SOC_E_NONE;
}
STATIC int
control_handler_rx_seq_done_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{

    SOC_IF_ERROR_RETURN(phymod_phy_rx_pmd_locked_get(phy, value));

    return SOC_E_NONE;
}

STATIC int 
control_handler_tx_polarity_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_polarity_t    polarity;

    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(phy, &polarity));
    *value = polarity.tx_polarity;

    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_polarity_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_polarity_t    polarity;

    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(phy, &polarity));
    *value = polarity.rx_polarity;

    return SOC_E_NONE;
}


STATIC int
control_handler_rx_reset_get(int unit, phymod_phy_access_t *phy,  uint32 param, uint32 *value)
{
    phymod_phy_reset_t reset;

    SOC_IF_ERROR_RETURN(phymod_phy_reset_get(phy, &reset));
    *value = (reset.rx  == phymodResetDirectionIn) ?  1 : 0;

    return SOC_E_NONE;
}


STATIC int
control_handler_tx_reset_get(int unit, phymod_phy_access_t *phy,  uint32 param, uint32 *value)
{
    phymod_phy_reset_t reset;

    SOC_IF_ERROR_RETURN(phymod_phy_reset_get(phy, &reset));
    *value = (reset.tx  == phymodResetDirectionIn) ?  1 : 0;

    return SOC_E_NONE;
}


/****************************************************************************** 
  port and phy control set wrapers
*******************************************************************************/



int soc_port_control_get_wrapper(int unit, phymod_ref_clk_t ref_clock, int is_lane_control, phymod_phy_access_t *phys, int nof_phys, soc_phy_control_t control, uint32 *value)
{
    control_get_handler_f handler = NULL;
    uint32 param = 0;
    int i;
    int lane_control_support = FALSE;

    if ((control < 0) || (control >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    switch(control) {
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        handler = control_handler_tx_fir_pre_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        handler = control_handler_tx_fir_main_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        handler = control_handler_tx_fir_post_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        handler = control_handler_tx_fir_post2_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        handler = control_handler_tx_fir_post3_get;
        lane_control_support = TRUE;
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS:
        handler = control_handler_preemphasis_get;
        lane_control_support = TRUE;
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        handler = control_handler_driver_current_get;
        lane_control_support = TRUE;
        break;
     /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        handler = control_handler_tx_squelch_get;
        lane_control_support = TRUE;
        break;
    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        handler = control_handler_rx_peak_filter_get;
        lane_control_support = TRUE;
        break;
    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        handler = control_handler_rx_vga_get;
        lane_control_support = TRUE;
        break;
    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 1;
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 2;
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 3;
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 4;
        break;
    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
        handler = control_handler_pi_control_get;
        break;

    /* CL72 ENABLE */
    case SOC_PHY_CONTROL_CL72:
        handler = control_handler_cl72_enable_get;
        lane_control_support = TRUE;
        break;

    /* CL72 STATUS */
    case SOC_PHY_CONTROL_CL72_STATUS:
        handler = control_handler_cl72_status_get;
        lane_control_support = TRUE;
        break;

    /* LANE SWAP */
    case SOC_PHY_CONTROL_LANE_SWAP:
        handler = control_handler_lane_map_get;
        break;
    /* PRBS */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        handler = control_handler_prbs_poly_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        handler = control_handler_prbs_tx_invert_data_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        handler = control_handler_prbs_tx_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        handler = control_handler_prbs_rx_invert_data_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        handler = control_handler_prbs_rx_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        handler = control_handler_prbs_tx_poly_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        handler = control_handler_prbs_tx_invert_data_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        handler = control_handler_prbs_tx_enable_get;
        lane_control_support = TRUE;
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        handler = control_handler_prbs_rx_poly_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        handler = control_handler_prbs_rx_invert_data_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        handler = control_handler_prbs_rx_enable_get;
        lane_control_support = TRUE;
        break; 
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        handler = control_handler_prbs_rx_status_get;
        lane_control_support = TRUE;
        break;
    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        handler = control_handler_loopback_internal_get;
        lane_control_support = TRUE;
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        handler = control_handler_loopback_remote_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        handler = control_handler_loopback_remote_pcs_get;
        lane_control_support = TRUE;
        break;

    /* SCRAMBLER */
    case SOC_PHY_CONTROL_SCRAMBLER:
        handler = control_handler_scrambler_get;
        param = ref_clock;
        break;
    /* RX_LOW_FREQ_PEAK_FILTER */
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        handler = control_handler_rx_low_freq_filter_get;
        lane_control_support = TRUE;
        break;
    /* RX_SEQ_DONE */
    case SOC_PHY_CONTROL_RX_SEQ_DONE:
       handler = control_handler_rx_seq_done_get;
       lane_control_support = TRUE;
       break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
        handler = control_handler_firmware_mode_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        handler = control_handler_tx_polarity_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_POLARITY:
        handler = control_handler_rx_polarity_get;
        lane_control_support = TRUE;
        break;
     /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        handler = control_handler_rx_reset_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        handler = control_handler_tx_reset_get;
        lane_control_support = TRUE;
        break;
    default:
        return SOC_E_UNAVAIL;
        break; 
    }
    if(is_lane_control && !lane_control_support){
        return SOC_E_UNAVAIL;
    }
    if(handler == NULL){
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(handler(unit, &phys[0], param, value));

    if((control == SOC_PHY_CONTROL_PRBS_RX_STATUS) && (((int)(*value)) >= 0)){
        uint32 value2 = 0;

        for(i = 1 ; i < nof_phys ; i++){
            SOC_IF_ERROR_RETURN(handler(unit, &phys[i], param, &value2));
            if(((int)value2) < 0){
                *value = value2;
                break;
            }
            else{
                *value += value2;
            }
        }
    }

    return SOC_E_NONE;
}




int soc_port_control_set_wrapper(int unit, phymod_ref_clk_t ref_clock, int is_lane_control, phymod_phy_access_t *phys, int nof_phys, soc_phy_control_t control, uint32 value)
{
    control_set_handler_f handler = NULL;
    uint32 param2 = 0;
    int lane_control_support = FALSE;
    int i;

    if ((control < 0) || (control >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    switch(control) {
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        handler = control_handler_tx_fir_pre_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        handler = control_handler_tx_fir_main_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        handler = control_handler_tx_fir_post_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        handler = control_handler_tx_fir_post2_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        handler = control_handler_tx_fir_post3_set;
        lane_control_support = TRUE;
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS:
        handler = control_handler_preemphasis_set;
        lane_control_support = TRUE;
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        handler = control_handler_driver_current_set;
        lane_control_support = TRUE;
        break;
     /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        handler = control_handler_tx_squelch;
        lane_control_support = TRUE;
        break;

    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        handler = control_handler_rx_peak_filter_set;
        lane_control_support = TRUE;
        break;
    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        handler = control_handler_rx_vga_set;
        lane_control_support = TRUE;
        break;
    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 1;
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 2;
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 3;
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 4;
        break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 1;
        break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 2;
        break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 3;
        break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 4;
        break;
    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
        handler = control_handler_pi_control_set;
        lane_control_support = TRUE;
        break;
    /* POLARITY */
    case SOC_PHY_CONTROL_RX_POLARITY:
        handler = control_handler_rx_polarity_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        handler = control_handler_tx_polarity_set;
        lane_control_support = TRUE;
        break;
    /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        handler = control_handler_rx_reset_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        handler = control_handler_tx_reset_set;
        lane_control_support = TRUE;
        break;

    /* CL72 ENABLE */
    case SOC_PHY_CONTROL_CL72:
        handler = control_handler_cl72_enable_set;
        lane_control_support = TRUE;
        break;

    /* LANE SWAP */
    case SOC_PHY_CONTROL_LANE_SWAP:
        handler = control_handler_lane_map_set;
        break;

    /* FIRMWARE MODE */
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
        handler = control_handler_firmware_mode_set;
        lane_control_support = TRUE;
        break; 
#if defined(TODO_CONTROLS) /*not implemented yet*/
    /* TX PATTERN */
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
        rv = tsce_pattern_256bit_set(pmc, &pCfg->pattern_256bit, value);
        break ;
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
        rv = tsce_pattern_256bit_set(pmc, &pCfg->pattern_256bit, value);
        break ;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        rv = tsce_pattern_data_set(0, &pCfg->pattern_256bit, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        rv = tsce_pattern_data_set(1, &pCfg->pattern_256bit, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        rv = tsce_pattern_data_set(2, &pCfg->pattern_256bit, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        rv = tsce_pattern_data_set(3, &pCfg->pattern_256bit, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        rv = tsce_pattern_data_set(4, &pCfg->pattern_256bit, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        rv = tsce_pattern_data_set(5, &pCfg->pattern_256bit, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        rv = tsce_pattern_data_set(6, &pCfg->pattern_256bit, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        rv = tsce_pattern_data_set(7, &pCfg->pattern_256bit, value);
        break;
#endif
    /* PRBS */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        handler = control_handler_prbs_poly_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        handler = control_handler_prbs_tx_invert_data_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        handler = control_handler_prbs_tx_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        handler = control_handler_prbs_rx_invert_data_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        handler = control_handler_prbs_rx_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        handler = control_handler_prbs_tx_poly_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        handler = control_handler_prbs_tx_invert_data_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        handler = control_handler_prbs_tx_enable_set;
        lane_control_support = TRUE;
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        handler = control_handler_prbs_rx_poly_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        handler = control_handler_prbs_rx_invert_data_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        handler = control_handler_prbs_rx_enable_set;
        lane_control_support = TRUE;
        break; 

    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        handler = control_handler_loopback_internal_set;
        lane_control_support = TRUE;
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        handler = control_handler_loopback_remote_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        handler = control_handler_loopback_remote_pcs_set;
        lane_control_support = TRUE;
        break;

    /* SCRAMBLER */
    case SOC_PHY_CONTROL_SCRAMBLER:
        handler = control_handler_scrambler_set;
        lane_control_support = TRUE;
        param2 = ref_clock;
        break;

#if defined(TODO_CONTROLS) /*not implemented yet*/
    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        rv = tsce_fec_set(pmc, value);
        break;
    /* 8B10B */
    case SOC_PHY_CONTROL_8B10B:
        rv = tsce_8b10b_set(pmc, value);
        break;

    /* 64B65B */
    case SOC_PHY_CONTROL_64B66B:
        rv = tsce_64b65b_set(pmc, value);
        break;
#endif /*not implemented yet*/

    /* POWER */
    case SOC_PHY_CONTROL_POWER:
        handler = control_handler_power_set;
        lane_control_support = TRUE;
        break;

    /* RX_LOW_FREQ_PEAK_FILTER */
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        handler = control_handler_rx_low_freq_filter_set;
        lane_control_support = TRUE;
        break;

#if defined(TODO_CONTROLS) /*not implemented yet*/
    /* TX_PPM_ADJUST */
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
       handler = ;
       break;
    /* VCO_FREQ */
    case SOC_PHY_CONTROL_VCO_FREQ:
       rv = tsce_vco_freq_set(pmc, value);
       break;

    /* PLL_DIVIDER */
    case SOC_PHY_CONTROL_PLL_DIVIDER:
       rv = tsce_pll_divider_set(pmc, value);
       break;

    /* OVERSAMPLE_MODE */
    case SOC_PHY_CONTROL_OVERSAMPLE_MODE:
       rv = tsce_oversample_mode_set(pmc, value);
       break;

    /* REF_CLK */
    case SOC_PHY_CONTROL_REF_CLK:
       rv = tsce_ref_clk_set(pmc, value);
       break;
    /* DRIVER_SUPPLY */
    case SOC_PHY_CONTROL_DRIVER_SUPPLY:
       rv = tsce_driver_supply_set(pmc, value);
       break;
#endif /*not implemented yet*/
    /* RX_SEQ_TOGGLE */
    case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
       handler = control_handler_rx_seq_toggle_set;
       lane_control_support = TRUE;
       break;
    default:
        return SOC_E_UNAVAIL;
        break; 
    }
    if(is_lane_control && !lane_control_support){
        return SOC_E_UNAVAIL;
    }
    if(handler == NULL){
        return SOC_E_INTERNAL;
    }
    for(i = 0 ; i < nof_phys ; i++){
        SOC_IF_ERROR_RETURN(handler(unit, &phys[i], value, param2));
    }
    return SOC_E_NONE;
}

#else
int phymod_port_control_not_empty;
#endif 
