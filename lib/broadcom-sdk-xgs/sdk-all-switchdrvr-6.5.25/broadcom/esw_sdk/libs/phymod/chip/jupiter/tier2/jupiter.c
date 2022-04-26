/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/chip/jupiter.h>

#include "jupiter/tier1/jupiter_dependencies.h"
#include "jupiter/tier1/jupiter_core.h"
#include "jupiter/tier1/jupiter_cfg_seq.h"

int jupiter_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    return PHYMOD_E_NONE;
}


/* This is not used by any upper layer */
int jupiter_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    info->core_version = phymodCoreVersionJupiter;
    info->serdes_id = 0;
    info->phy_id0 = 0;
    info->phy_id1 = 0;

    return PHYMOD_E_NONE;
}


int jupiter_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    int i;
    uint32_t lane_map_tx[JUPITER_NUM_LANES];
    uint32_t lane_map_rx[JUPITER_NUM_LANES];
    phymod_core_access_t core_copy;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    for (i = 0; i < JUPITER_NUM_LANES; i++) {
        /*
         * Logical lanes in Jupiter PMD API domain are in range [1,8],
         * where as it's in [0,7] in phymod API.
         */
        lane_map_tx[i] = lane_map->lane_map_tx[i] + 1;
        lane_map_rx[i] = lane_map->lane_map_rx[i] + 1;
    }

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_lane_map_set(&core_copy.access,
                                  lane_map_tx,
                                  lane_map_rx));
    return PHYMOD_E_NONE;
}

int jupiter_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    int i;
    uint32_t lane_map_tx[JUPITER_NUM_LANES];
    uint32_t lane_map_rx[JUPITER_NUM_LANES];
    phymod_phy_access_t core_copy;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_lane_map_get(&core_copy.access,
                                  lane_map_tx,
                                  lane_map_rx));
    for (i = 0 ; i < JUPITER_NUM_LANES; i++) {
        /*
         * Logical lanes in Jupiter PMD API domain are in range [1,8],
         * where as it's in [0,7] in phymod API.
         */
        lane_map->lane_map_tx[i] = lane_map_tx[i] - 1;
        lane_map->lane_map_rx[i] = lane_map_rx[i] - 1;
    }


    return PHYMOD_E_NONE;
}

int jupiter_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return PHYMOD_E_NONE;
}

int jupiter_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{
    return PHYMOD_E_NONE;
}

int jupiter_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_rx_restart(const phymod_phy_access_t* phy)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_tx_polarity_set(&phy_copy.access, polarity->tx_polarity));
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_rx_polarity_set(&phy_copy.access, polarity->rx_polarity));

    return PHYMOD_E_NONE;
}

int jupiter_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_tx_polarity_get(&phy_copy.access, &polarity->tx_polarity));
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_rx_polarity_get(&phy_copy.access, &polarity->rx_polarity));

    return PHYMOD_E_NONE;
}


int jupiter_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    jupiter_txfir_config_t txfir_cfg;

    PHYMOD_MEMSET(&txfir_cfg, 0, sizeof(txfir_cfg));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    txfir_cfg.CM1 = tx->pre;
    txfir_cfg.CM2 = tx->pre2;
    txfir_cfg.CM3 = tx->pre3;
    txfir_cfg.C0 = tx->main;
    txfir_cfg.C1 = tx->post;

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_txfir_config_set(&phy_copy.access, txfir_cfg));
    }

    return PHYMOD_E_NONE;
}

int jupiter_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    jupiter_txfir_config_t txfir_cfg;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMSET(&txfir_cfg, 0, sizeof(txfir_cfg));

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_txfir_config_get(&phy_copy.access, &txfir_cfg));

    tx->pre = txfir_cfg.CM1;
    tx->pre2 = txfir_cfg.CM2;
    tx->pre3 = txfir_cfg.CM3;
    tx->main = txfir_cfg.C0;
    tx->post = txfir_cfg.C1;

    return PHYMOD_E_NONE;
}


int jupiter_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{

    /* This API seems to be to override phase interpolator.
     * Not supported
     */

    return PHYMOD_E_NONE;
}

int jupiter_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
    /* No corresponding Jupiter PMD API */

    return PHYMOD_E_NONE;
}


int jupiter_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    /* No corresponding Jupiter PMD API */

    return PHYMOD_E_NONE;
}

int jupiter_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    /* No corresponding Jupiter PMD API */

    return PHYMOD_E_NONE;

}

int jupiter_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint32_t adapt_enable = 0, ctle_boost = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);

        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_rx_dfe_adapt_get(&phy_copy.access, &adapt_enable));
        if (!adapt_enable) {
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_rx_dfe_adapt_set(&phy_copy.access, 1));
        }

        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_rx_ctle_adapt_get(&phy_copy.access, &adapt_enable, &ctle_boost));
        if (!adapt_enable) {
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_rx_ctle_adapt_set(&phy_copy.access, 1, 0));
        }

        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_rx_background_adapt_enable_get(&phy_copy.access, &adapt_enable));
        if (!adapt_enable) {
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_rx_background_adapt_enable_set(&phy_copy.access, 1));
        }

        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_rx_autoeq_get(&phy_copy.access, &adapt_enable));
        if (!adapt_enable) {
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_rx_autoeq_set(&phy_copy.access, 1));
        }
    }

    return PHYMOD_E_NONE;
}


int jupiter_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    return PHYMOD_E_NONE;
}

int jupiter_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    return PHYMOD_E_NONE;
}

int jupiter_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    return PHYMOD_E_NONE;
}

int jupiter_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    return PHYMOD_E_NONE;
}


int jupiter_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    return PHYMOD_E_NONE;
}

int jupiter_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
    int start_lane, num_lane, i;
    phymod_phy_access_t pm_phy_copy;
    uint32_t tmp_lock;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    *rx_pmd_locked = 1;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_rx_lock_status_get(&pm_phy_copy.access, &tmp_lock));
        *rx_pmd_locked &= tmp_lock;
    }

    return PHYMOD_E_NONE;
}


int jupiter_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* rx_signal_detect)
{
    int start_lane, num_lane, i;
    phymod_phy_access_t pm_phy_copy;
    uint32_t tmp_detect;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    *rx_signal_detect = 1;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_rx_signal_detect_get(&pm_phy_copy.access, &tmp_detect));
        *rx_signal_detect &= tmp_detect;
    }

    return PHYMOD_E_NONE;
}


int jupiter_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_eye_margin_est_get(const phymod_phy_access_t* phy, phymod_eye_margin_mode_t eye_margin_mode, uint32_t* value)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_speed_config_set(const phymod_phy_access_t* phy, const phymod_phy_speed_config_t* speed_config, const phymod_phy_pll_state_t* old_pll_state, phymod_phy_pll_state_t* new_pll_state)
{
    return PHYMOD_E_NONE;
}

int jupiter_phy_speed_config_get(const phymod_phy_access_t* phy, phymod_phy_speed_config_t* speed_config)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_firmware_load_info_get(const phymod_phy_access_t* phy, phymod_firmware_load_info_t* info)
{
    return PHYMOD_E_NONE;
}

int jupiter_phy_tx_pam4_precoder_enable_set(const phymod_phy_access_t* phy, int enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_tx_pam4_precoder_override_set(&phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_tx_pam4_precoder_enable_set(&phy_copy.access, (uint32_t)enable, 0));
    }

    return PHYMOD_E_NONE;
}

int jupiter_phy_tx_pam4_precoder_enable_get(const phymod_phy_access_t* phy, int* enable)
{
    phymod_phy_access_t pm_phy_copy;
    uint32_t ovr_enable = 0, gray_code_en = 0, dplus_en = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_tx_pam4_precoder_override_get(&pm_phy_copy.access, &ovr_enable));
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_tx_pam4_precoder_enable_get(&pm_phy_copy.access, &gray_code_en, &dplus_en));

    *enable = ovr_enable?(gray_code_en || dplus_en):0;

    return PHYMOD_E_NONE;

}

int jupiter_phy_pll_pwrdn(const phymod_phy_access_t* phy, uint32_t pll_index, uint32_t pwrdn)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_channel_loss_hint_set(const phymod_phy_access_t* phy, uint32_t channel_loss)
{
    return PHYMOD_E_NONE;
}

int jupiter_phy_channel_loss_hint_get(const phymod_phy_access_t* phy, uint32_t* channel_loss)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_pmd_info_init(const phymod_phy_access_t* phy)
{
    return PHYMOD_E_NONE;
}


int jupiter_phy_pll_powerdown_get(const phymod_phy_access_t* phy, uint32_t pll_index, uint32_t* powerdown)
{
    jupiter_pll_pstatus_t pll_power_status;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_pll_pwrdn_get(&phy_copy.access, &pll_power_status));

    if (pll_power_status == JUPITER_PWR_DOWN) {
        *powerdown = 1;
    } else {
        *powerdown = 0;
    }

    return PHYMOD_E_NONE;
}


