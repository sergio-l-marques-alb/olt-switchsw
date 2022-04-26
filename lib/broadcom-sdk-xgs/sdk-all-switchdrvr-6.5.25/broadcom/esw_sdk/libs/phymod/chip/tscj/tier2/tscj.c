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
#include <phymod/chip/tscj.h>
#include <phymod/chip/bcmi_tscj_xgxs_defs.h>
#include <phymod/chip/jupiter.h>
#include "jupiter/tier1/jupiter_core.h"
#include "jupiter/tier1/jupiter_dependencies.h"
#include "jupiter/tier1/jupiter_cfg_seq.h"
#include "jupiter/tier1/jupiter_ucode.h"
#include "tsco/tier1/tscomod.h"
#include "tsco/tier1/tscomod_1588_lkup_table.h"
#include "tsco/tier1/tscomod_spd_ctrl.h"
#include "tscj/tier1/tscjmod.h"
#include "tscj/tier1/tscjmod_sc_lkup_table.h"
#include "tscj/tier1/tscjmod_spd_ctrl.h"

#define JUPITER_BRINGUP_DONE 0
int tscj_core_identify(const phymod_core_access_t* core, uint32_t core_id,
                       uint32_t* is_identified)
{
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN0_SERDESIDr_t serdesid;
    int ioerr = 0;

    *is_identified = 0;
    ioerr += READ_PHYID2r(&core->access, &id2);
    ioerr += READ_PHYID3r(&core->access, &id3);

    if (PHYID2r_REGID1f_GET(id2) == TSCOMOD_ID0 &&
       (PHYID3r_REGID2f_GET(id3) == TSCOMOD_ID1)) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN0_SERDESIDr(&core->access, &serdesid);
        if ( (MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid)) == TSCJ_MODEL_NUMBER)  {
            *is_identified = 1;
        }
    }

    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
}

int tscj_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    int rv = 0;
    MAIN0_SERDESIDr_t serdes_id;
    char core_name[15] = "Tscj";
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &core->access;

    rv = READ_MAIN0_SERDESIDr(&core->access, &serdes_id);

    info->serdes_id = MAIN0_SERDESIDr_GET(serdes_id);
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(core, info->serdes_id, core_name, info));
    info->core_version = phymodCoreVersionTscj;

    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];

    return rv;
}

int tscj_phy_firmware_lane_config_get(const phymod_phy_access_t* phy,
                                      phymod_firmware_lane_config_t* fw_config)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_tx_lane_control_set(const phymod_phy_access_t* phy,
                                 phymod_phy_tx_lane_control_t tx_control)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Set the override to allow PCS/SW to override TX disable. */
    PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_disable_pin_override_set(&phy_copy.access, 1));

    switch (tx_control) {
        case phymodTxElectricalIdleEnable:
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_disable_set(&phy_copy.access, 1));
            break;
        case phymodTxElectricalIdleDisable:
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_disable_set(&phy_copy.access, 0));
            break;
        case phymodTxSquelchOn:
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_disable_set(&phy_copy.access, 1));
            break;
        case phymodTxSquelchOff:
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_disable_set(&phy_copy.access, 0));
            break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int tscj_phy_tx_lane_control_get(const phymod_phy_access_t* phy,
                                 phymod_phy_tx_lane_control_t *tx_control)
{
    uint32_t tx_disable, tx_disable_override;
    uint32_t lb_enable;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN(jupiter_pmd_tx_disable_get(&phy_copy.access, &tx_disable_override, &tx_disable));

    /* next check if PMD loopback is on */
    if (tx_disable_override && tx_disable) {
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_analog_loopback_get(&phy_copy.access, &lb_enable));
        if (lb_enable) tx_disable = 0;
    }

    if (tx_disable_override) {
        if (tx_disable) {
            *tx_control = phymodTxSquelchOn;
        } else {
            *tx_control = phymodTxSquelchOff;
        }
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_rx_lane_control_set(const phymod_phy_access_t* phy,
                                 phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (rx_control) {
        case phymodRxSquelchOn:
            /* Force signal detect value to 0. */
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_force_signal_detect_config_set(&phy_copy.access, JUPITER_SIGDET_FORCE0));
            break;
        case phymodRxSquelchOff:
           /* Signal detect value is not forced. */
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_force_signal_detect_config_set(&phy_copy.access, JUPITER_SIGDET_NORM));
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_rx_lane_control_get(const phymod_phy_access_t* phy,
                                 phymod_phy_rx_lane_control_t* rx_control)
{
    int rx_squelch_enable = 0;
    uint32_t lb_enable = 0;
    phymod_phy_access_t phy_copy;
    jupiter_force_sigdet_mode_t sigdet_mode;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_force_signal_detect_config_get(&phy_copy.access, &sigdet_mode));

    if (sigdet_mode == JUPITER_SIGDET_FORCE0) {
        rx_squelch_enable = 1;
    } else if (sigdet_mode == JUPITER_SIGDET_NORM){
        rx_squelch_enable = 0;
    }

    /* Check if PMD loopback is on */
    if (rx_squelch_enable) {
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_analog_loopback_get(&phy_copy.access, &lb_enable));
        if (lb_enable) rx_squelch_enable = 0;
    }
    if(rx_squelch_enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        *rx_control = phymodRxSquelchOff;
    }

    return PHYMOD_E_NONE;

}


STATIC
int _tscj_core_firmware_load(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config)
{
    phymod_core_access_t  core_copy;
    phymod_phy_access_t phy_access;
    unsigned int jupiter_ucode_len;
    unsigned char *jupiter_ucode;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    TSCOMOD_CORE_TO_PHY_ACCESS(&phy_access, core);

    phy_access.access.lane_mask = 0x0;
    jupiter_ucode = jupiter_ucode_get();
    jupiter_ucode_len = JUPITER_UCODE_IMAGE_SIZE;

    /*  */
    PHYMOD_IF_ERR_RETURN(jupiter_pmd_uc_ucode_crc_clear(&phy_access.access));
    switch(init_config->firmware_load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(jupiter_ucode_pmi_load(&core_copy.access, 0));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_IF_ERR_RETURN(jupiter_ucode_pmi_load(&core_copy.access, 1));
        PHYMOD_NULL_CHECK(init_config->firmware_loader);
        PHYMOD_IF_ERR_RETURN(init_config->firmware_loader(core, jupiter_ucode_len, jupiter_ucode));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), init_config->firmware_load_method));
    }

    return PHYMOD_E_NONE;
}


int tscj_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, port_start_lane, port_num_lane;
    uint32_t  pcs_lane_enable ;
    jupiter_txfir_config_t txfir_cfg;

    PHYMOD_MEMSET(&txfir_cfg, 0, sizeof(jupiter_txfir_config_t));

    /*
     * Extra bit could be set in phy->access.lane_mask as the special flag.
     * It is used in tscomod_port_start_lane_get() in order to get the correct
     * port_start_lane for 4-lane port or 8-lane port.
     * The extra bit is used in tscomod_port_start_lane_get() only,
     * and it should be cleared before calling other functions.
     */
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* clear the special bit, only bit 0-7 are cared */
    phy_copy.access.lane_mask &= 0xff;
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy_copy.access, &start_lane, &num_lane));

    /*get the start lane of the port lane mask */
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (tscomod_port_start_lane_get(&phy_copy.access, &port_start_lane, &port_num_lane));

    phy_copy.access.lane_mask = 1 << port_start_lane;

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tscomod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_disable_set(&phy_copy.access));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        txfir_cfg.CM3 = tx->pre3;
        txfir_cfg.CM2 = tx->pre2;
        txfir_cfg.CM1 = tx->pre;
        txfir_cfg.C1 = tx->post;
        txfir_cfg.C0 = tx->main;

        phy_copy.access.lane_mask = 1 << (start_lane + i);

        /* Set value for each TX FIR tap setting. */
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_txfir_config_set(&phy_copy.access, txfir_cfg));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_enable_set(&phy_copy.access));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _tscj_core_tx_rx_mux_select_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    uint32_t lane, pcs_tx_swap = 0, pcs_rx_swap = 0;
    phymod_core_access_t  core_copy;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    if (lane_map->num_of_lanes != TSCOMOD_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }

    for (lane = 0; lane < TSCOMOD_NOF_LANES_IN_CORE; lane++){
        if ((lane_map->lane_map_tx[lane] >= TSCOMOD_NOF_LANES_IN_CORE)||
             (lane_map->lane_map_rx[lane] >= TSCOMOD_NOF_LANES_IN_CORE)){
            return PHYMOD_E_CONFIG;
        }
        /*encode each lane as four bits*/
        pcs_tx_swap += lane_map->lane_map_tx[lane]<<(lane*4);
        pcs_rx_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }
    PHYMOD_IF_ERR_RETURN
        (tscomod_pcs_tx_mux_select(&core_copy.access, pcs_tx_swap));
    PHYMOD_IF_ERR_RETURN
        (tscomod_pcs_rx_mux_select(&core_copy.access, pcs_rx_swap));

    return PHYMOD_E_NONE;
}

int tscj_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    uint32_t lane, pcs_tx_swap = 0, pcs_rx_swap = 0;
    uint32_t pmd_tx_addr[8], pmd_rx_addr[8];
    phymod_core_access_t  core_copy;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    if (lane_map->num_of_lanes != TSCOMOD_NOF_LANES_IN_CORE) {
        return PHYMOD_E_CONFIG;
    }

    for (lane = 0; lane < TSCOMOD_NOF_LANES_IN_CORE; lane++){
        if ((lane_map->lane_map_tx[lane] >= TSCOMOD_NOF_LANES_IN_CORE)||
             (lane_map->lane_map_rx[lane] >= TSCOMOD_NOF_LANES_IN_CORE)) {
            return PHYMOD_E_CONFIG;
        }
        /* Encode each lane as four bits*/
        pcs_tx_swap += lane_map->lane_map_tx[lane]<<(lane*4);
        pcs_rx_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }

    /*
     * PMD API expects logical to physical mapping, but the range of
     * physical lanes is [1-8] instead of [0-7]
     */
    for (lane = 0; lane < TSCOMOD_NOF_LANES_IN_CORE; lane++){
        pmd_tx_addr[lane] = lane_map->lane_map_tx[lane] + 1;
        pmd_rx_addr[lane] = lane_map->lane_map_rx[lane] + 1;
    }

    PHYMOD_IF_ERR_RETURN
        (tscomod_pcs_tx_m1_lane_swap(&core_copy.access, pcs_tx_swap));
    PHYMOD_IF_ERR_RETURN
        (tscomod_pcs_tx_m0_lane_swap(&core_copy.access, pcs_tx_swap));
    PHYMOD_IF_ERR_RETURN
        (tscomod_pcs_rx_lane_swap(&core_copy.access, pcs_rx_swap));

    COMPILER_REFERENCE(pmd_tx_addr);
    COMPILER_REFERENCE(pmd_rx_addr);


    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_lane_map_set(&core_copy.access, pmd_tx_addr, pmd_rx_addr));

    return PHYMOD_E_NONE;
}

int tscj_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    int i = 0;
    uint32_t tx_lane_map[8] = {0};
    uint32_t rx_lane_map[8] = {0};
    phymod_core_access_t  core_copy;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_lane_map_get(&core_copy.access, tx_lane_map, rx_lane_map));

    /* Get the lane map into serdes api format */
    lane_map->num_of_lanes = TSCOMOD_NOF_LANES_IN_CORE;
    for (i = 0; i < TSCOMOD_NOF_LANES_IN_CORE; i++) {
        if ((tx_lane_map[i] >= 1) && (rx_lane_map[i] >= 1)) {
            lane_map->lane_map_tx[tx_lane_map[i] - 1] = i;
            lane_map->lane_map_rx[rx_lane_map[i] - 1] = i;
        }
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_firmware_lane_config_set(const phymod_phy_access_t* phy,
                                      phymod_firmware_lane_config_t fw_config)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_tx_polarity_set(&phy_copy.access, polarity->tx_polarity));

    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_rx_polarity_set(&phy_copy.access, polarity->rx_polarity));

    return PHYMOD_E_NONE;
}

int tscj_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    int start_lane, num_lane, i;
    phymod_polarity_t temp_pol;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    polarity->tx_polarity = 0;
    polarity->rx_polarity = 0;
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        temp_pol.tx_polarity = 0;
        temp_pol.rx_polarity = 0;
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_tx_polarity_get(&phy_copy.access, &temp_pol.tx_polarity));

        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_rx_polarity_get(&phy_copy.access, &temp_pol.rx_polarity));

        polarity->tx_polarity |= ((temp_pol.tx_polarity & 0x1) << i);
        polarity->rx_polarity |= ((temp_pol.rx_polarity & 0x1) << i);
    }

    return PHYMOD_E_NONE;
}

int tscj_port_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t pcs_enable;
    int start_lane, num_lane, port_an_enable, port_enable;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    /* Read port Autoneg enable bit */
    PHYMOD_IF_ERR_RETURN
        (tscomod_port_an_mode_enable_get(&phy_copy.access, &port_an_enable));

    /* Get current port enable bit */
    PHYMOD_IF_ERR_RETURN
        (tscomod_port_enable_get(&phy_copy.access, &port_enable));

    /* Check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tscomod_enable_get(&phy_copy.access, &pcs_enable));

#if JUPITER_BRINGUP_DONE
    /* Set the override to allow the PCS to override TX disable. */
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_tx_disable_pin_override_set(&phy_copy.access, 1));
#endif

    if ((port_an_enable) || (port_enable && !pcs_enable)) {
        /* Current port is in An mode mode */
        if (enable == 1) {
            /* Release both tx/rx */
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_tx_disable_set(&phy_copy.access, 0));
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_force_signal_detect_config_set(&phy_copy.access,JUPITER_SIGDET_NORM));
            phy_copy.access.lane_mask = 1 << start_lane;
        }
        PHYMOD_IF_ERR_RETURN
            (tscomod_port_cl73_enable_set(&phy_copy.access, enable));
        if (!enable) {
            /* Set tx/rx disabled */
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_tx_disable_set(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_force_signal_detect_config_set(&phy_copy.access,JUPITER_SIGDET_FORCE0));
        }
    } else {
        /* Current port is in forced speed mode */
        if (enable == 1) {
            /* Release tx/rx */
#if JUPITER_BRINGUP_DONE
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_tx_disable_set(&phy_copy.access, 0));
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_force_signal_detect_config_set(&phy_copy.access,JUPITER_SIGDET_NORM));
#endif
            phy_copy.access.lane_mask = 1 << start_lane;
            /* Enable speed control bit */
            PHYMOD_IF_ERR_RETURN
                (tscomod_enable_set(&phy_copy.access));
        } else if (enable == 0) {
            /* disable speed control bit */
            PHYMOD_IF_ERR_RETURN
                (tscomod_disable_set(&phy_copy.access));
            /*  Set tx/rx disabled */
#if JUPITER_BRINGUP_DONE
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_tx_disable_set(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_force_signal_detect_config_set(&phy_copy.access,JUPITER_SIGDET_FORCE0));
#endif
        }
    }

    /* Set port enable bit */
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (tscomod_port_enable_set(&phy_copy.access, enable));

    return PHYMOD_E_NONE;
}

int tscj_port_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;
    int temp_enable;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Get port enable bit */
    PHYMOD_IF_ERR_RETURN
        (tscomod_port_enable_get(&phy_copy.access, &temp_enable));

    *enable = (uint32_t) temp_enable;

    return PHYMOD_E_NONE;
}

int tscj_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    jupiter_txfir_config_t txfir_cfg;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMSET(&txfir_cfg, 0, sizeof(jupiter_txfir_config_t));

    /* Get current tx tap mode */
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_txfir_config_get(&phy_copy.access, &txfir_cfg));

    tx->pre = txfir_cfg.CM1;
    tx->pre2 = txfir_cfg.CM2;
    tx->pre3 = txfir_cfg.CM3;
    tx->main = txfir_cfg.C0;
    tx->post = txfir_cfg.C1;
    tx->post2 = 0;
    tx->post3 = 0;

    return PHYMOD_E_NONE;
}

/* This function is to assign forced speed SW speed_id
 * based on num_lane, data_rate and fec_type
 */
STATIC
int _tscj_phy_speed_id_set(int num_lane,
                            uint32_t data_rate,
                            phymod_fec_type_t fec_type,
                            tscomod_spd_intfc_type_t* spd_intf)
{
    if (num_lane == 1) {
        switch (data_rate) {
            case 10000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TSCOMOD_SPD_10G_IEEE_KR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 25000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TSCOMOD_SPD_25G_BRCM_NO_FEC_KR1_CR1;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TSCOMOD_SPD_25G_BRCM_FEC_528_KR1_CR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 50000:
                if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TSCOMOD_SPD_50G_IEEE_KR1_CR1;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TSCOMOD_SPD_50G_BRCM_FEC_272_CR1_KR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_RS544_2XN) {
                    *spd_intf = TSCOMOD_SPD_100G_IEEE_KR1_CR1_OPT;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TSCOMOD_SPD_100G_BRCM_KR1_CR1;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TSCOMOD_SPD_100G_BRCM_FEC_272_KR1_CR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else if (num_lane == 2) {
        switch (data_rate) {
            case 50000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TSCOMOD_SPD_50G_BRCM_CR2_KR2_NO_FEC;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TSCOMOD_SPD_50G_BRCM_CR2_KR2_RS_FEC;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TSCOMOD_SPD_50G_BRCM_FEC_544_CR2_KR2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TSCOMOD_SPD_100G_IEEE_KR2_CR2;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TSCOMOD_SPD_100G_BRCM_FEC_272_CR2_KR2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 200000:
                if (fec_type == phymod_fec_RS544_2XN) {
                    *spd_intf = TSCOMOD_SPD_200G_IEEE_KR2_CR2;
                } else if (fec_type == phymod_fec_RS272_2XN) {
                    *spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_272_KR2_CR2;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_544_KR2_CR2;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_272_N2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else if (num_lane == 4) {
        switch (data_rate) {
            case 40000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TSCOMOD_SPD_40G_IEEE_KR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TSCOMOD_SPD_100G_BRCM_NO_FEC_X4;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TSCOMOD_SPD_100G_IEEE_KR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 200000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TSCOMOD_SPD_200G_BRCM_NO_FEC_KR4_CR4;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TSCOMOD_SPD_200G_BRCM_KR4_CR4;
                } else if (fec_type == phymod_fec_RS544_2XN) {
                    *spd_intf = TSCOMOD_SPD_200G_IEEE_KR4_CR4;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_272_N4;
                } else if (fec_type == phymod_fec_RS272_2XN) {
                    *spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_272_CR4_KR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 400000:
                if (fec_type == phymod_fec_RS544_2XN) {
                    *spd_intf = TSCOMOD_SPD_400G_IEEE_KR4_CR4;
                } else if (fec_type == phymod_fec_RS272_2XN) {
                    *spd_intf = TSCOMOD_SPD_400G_BRCM_FEC_272_KR4_CR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else if (num_lane == 8) {
        if (data_rate == 400000 && fec_type == phymod_fec_RS544_2XN) {
            *spd_intf = TSCOMOD_SPD_400G_BRCM_FEC_KR8_CR8;
        } else if (data_rate == 400000 && fec_type == phymod_fec_RS272_2XN) {
            *spd_intf = TSCOMOD_SPD_400G_BRCM_FEC_272_N8;
        } else if (data_rate == 200000 && fec_type == phymod_fec_RS544_2XN) {
            *spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_544_CR8_KR8;
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        return PHYMOD_E_UNAVAIL;
    }

    return PHYMOD_E_NONE;
}

STATIC
int _tscj_speed_table_entry_to_speed_config_get(tscjmod_spd_id_tbl_entry_t *speed_config_entry,
                                                phymod_phy_speed_config_t *speed_config)
{
    int num_lane;
    uint32_t  data_rate_lane = 0;

    switch (speed_config_entry->num_lanes) {
        case 0:
            num_lane = 1;
            break;
        case 1:
            num_lane = 2;
            break;
        case 2:
            num_lane = 4;
            break;
        case 3:
            num_lane = 8;
            break;
        case 4:
            num_lane = 3;
            break;
        case 5:
            num_lane = 6;
            break;
        case 6:
            num_lane = 7;
            break;
        default:
            PHYMOD_DEBUG_ERROR(("Unsupported number of lane \n"));
            return PHYMOD_E_UNAVAIL;
    }

    switch (speed_config_entry->pmd_preset) {
        case JUPITER_NRZ_1p25_2p5:
            data_rate_lane = 1000;
            break;
        case JUPITER_NRZ_10p3125:
            data_rate_lane = 10000;
            break;
        case JUPITER_NRZ_25p78125:
            data_rate_lane = 25000;
            break;
        case JUPITER_NRZ_26p5625:
            data_rate_lane = 25000;
            break;
        case JUPITER_PAM4_53p125:
            data_rate_lane = 50000;
            break;
        case JUPITER_PAM4_106p25:
            data_rate_lane = 100000;
            break;
        default:
            PHYMOD_DEBUG_ERROR(("Unsupported PMD PRESET. \n"));
            return PHYMOD_E_UNAVAIL;
    }

    speed_config->data_rate = data_rate_lane * num_lane;

    switch (speed_config_entry->pmd_training_mode) {
        case 0:
            speed_config->linkTraining = 0;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            speed_config->linkTraining = 1;
            break;
        default:
            PHYMOD_DEBUG_ERROR(("Unsupported PMD TRAINING MODE. \n"));
            return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_IF_ERR_RETURN
        (tscomod_fec_arch_decode_get(speed_config_entry->fec_arch, &(speed_config->fec_type)));

    return PHYMOD_E_NONE;
}

int tscj_phy_speed_config_set(const phymod_phy_access_t* phy,
                              const phymod_phy_speed_config_t* speed_config,
                              const phymod_phy_pll_state_t* old_pll_state,
                              phymod_phy_pll_state_t* new_pll_state)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, mapped_speed_id;
    tscomod_spd_intfc_type_t spd_intf = 0;
    int port_enable;
    uint32_t *tscj_spd_id_entry_53;
    uint32_t lane_mask_backup = 0;

    tscj_spd_id_entry_53 = tscj_spd_id_entry_53_get();

    /* Make sure that tvco pll index is valid */
    if (phy->access.tvco_pll_index > 0) {
        PHYMOD_DEBUG_ERROR(("Unsupported tvco index\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /* Copy the PLL state */
    *new_pll_state = *old_pll_state;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    lane_mask_backup = phy->access.lane_mask;

    /* Get current port enable bit */
    PHYMOD_IF_ERR_RETURN
        (tscomod_port_enable_get(&pm_phy_copy.access, &port_enable));

    /* Clear the port Autoneg mode enable bit */
    PHYMOD_IF_ERR_RETURN
        (tscomod_port_an_mode_enable_set(&pm_phy_copy.access, 0));

    /* Hold the pcs lane reset */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tscomod_disable_set(&pm_phy_copy.access));

    /* Write this port forced speed id entry */
    PHYMOD_IF_ERR_RETURN
      (tscomod_set_sc_speed(&pm_phy_copy.access, TSCOMOD_FORCED_SPEED_ID_OFFSET + start_lane, 0));

    pm_phy_copy.access.lane_mask = lane_mask_backup;

    /* Update port mode */
    PHYMOD_IF_ERR_RETURN
        (tscomod_update_port_mode(&pm_phy_copy.access, speed_config->data_rate));

    /* for speed mode config set */
    PHYMOD_IF_ERR_RETURN(_tscj_phy_speed_id_set(num_lane, speed_config->data_rate,
                                                 speed_config->fec_type, &spd_intf));

    PHYMOD_IF_ERR_RETURN
        (tscomod_get_mapped_speed(spd_intf, &mapped_speed_id));

    /* Set the rs fec CW properly */
    if ((speed_config->fec_type == phymod_fec_RS544) ||
        (speed_config->fec_type == phymod_fec_RS544_2XN)) {
        if (start_lane < 2) {
            pm_phy_copy.access.lane_mask = 1 << 0;
        } else if (start_lane < 4)  {
            pm_phy_copy.access.lane_mask = 1 << 2;
        } else if (start_lane < 6)  {
            pm_phy_copy.access.lane_mask = 1 << 4;
        } else {
            pm_phy_copy.access.lane_mask = 1 << 6;
        }
        PHYMOD_IF_ERR_RETURN
            (tscomod_rsfec_cw_type_set(&pm_phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_544, 0));

        /* Check if 8 or 400Gx4 lane port, need to set MPP1 as well */
        if ((num_lane == 8) ||
            ((num_lane == 4) && (speed_config->data_rate == 400000))) {
            pm_phy_copy.access.lane_mask = 1 << 2;
            PHYMOD_IF_ERR_RETURN
                (tscomod_rsfec_cw_type_set(&pm_phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_544, 0));
        }

    } else if ((speed_config->fec_type == phymod_fec_RS272) ||
               (speed_config->fec_type == phymod_fec_RS272_2XN)) {
        if (start_lane < 2) {
            pm_phy_copy.access.lane_mask = 1 << 0;
        } else if (start_lane < 4)  {
            pm_phy_copy.access.lane_mask = 1 << 2;
        } else if (start_lane < 6)  {
            pm_phy_copy.access.lane_mask = 1 << 4;
        } else {
            pm_phy_copy.access.lane_mask = 1 << 6;
        }
        PHYMOD_IF_ERR_RETURN
            (tscomod_rsfec_cw_type_set(&pm_phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_272, 0));
        /* next check if 8 or 400Gx4 lane port, need to set MPP1 as well */
        if ((num_lane == 8) ||
            ((num_lane == 4) && (speed_config->data_rate == 400000))) {
            pm_phy_copy.access.lane_mask = 1 << 2;
            PHYMOD_IF_ERR_RETURN
                (tscomod_rsfec_cw_type_set(&pm_phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_272, 0));
        }
    }

    /* Load speed id entry */
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, TSCOMOD_FORCED_SPEED_ID_OFFSET + start_lane,
                              (tscj_spd_id_entry_53 + mapped_speed_id * TSCJMOD_SPEED_ID_ENTRY_SIZE)));

    /* Release the pcs lane reset */
    if (port_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_enable_set(&pm_phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_speed_config_get(const phymod_phy_access_t* phy,
                              phymod_phy_speed_config_t* speed_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, speed_id;
    uint32_t packed_entry[20];
    tscjmod_spd_id_tbl_entry_t speed_config_entry;
    int an_en, an_done;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMSET(&speed_config_entry, 0, sizeof(tscjmod_spd_id_tbl_entry_t));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    /* For ethernet port */
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        /* Read speed id from resolved status */
        PHYMOD_IF_ERR_RETURN
            (tscomod_speed_id_get(&phy_copy.access, &speed_id));

        /* Check check if AN enabled */
        PHYMOD_IF_ERR_RETURN
           (tscomod_autoneg_status_get(&phy_copy.access, &an_en, &an_done));

        /* Read the speed entry and then decode the speed and FEC type */
        phy_copy.access.lane_mask = 1 << 0;
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

        /* Decode speed entry */
        tscjmod_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

        PHYMOD_IF_ERR_RETURN
            (_tscj_speed_table_entry_to_speed_config_get(&speed_config_entry, speed_config));

        /* If autoneg enabled, needs to update the FEC_ARCH based on the An resolved status */
        if (an_en && an_done) {
            uint8_t fec_arch;
            uint32_t fec_align_live = 1;
            phy_copy.access.lane_mask = 0x1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tscomod_autoneg_fec_status_get(&phy_copy.access, &fec_arch));
            PHYMOD_IF_ERR_RETURN
                (tscomod_fec_arch_decode_get(fec_arch, &(speed_config->fec_type)));

            /*
             * For 100G 4 lane NO FEC the AN resolved status is not correct.
             * Hence check if FEC align indeed happened to distinguish NO FEC case
             */
            if ((speed_id == TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_KR4) ||
                (speed_id == TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_CR4)) {
                PHYMOD_IF_ERR_RETURN
                    (tscomod_fec_align_status_get(&phy_copy.access, &fec_align_live));
                if (!fec_align_live) {
                    /* Case of 100G 4 LANE with NO FEC */
                    speed_config->fec_type = phymod_fec_None;
                }
            }
        }
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    phymod_phy_access_t phy_copy;
    int speed_id;
    uint32_t packed_entry[20];
    tscjmod_spd_id_tbl_entry_t speed_config_entry;
    phymod_phy_speed_config_t speed_config;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMSET(&speed_config_entry, 0, sizeof(tscjmod_spd_id_tbl_entry_t));
    PHYMOD_MEMSET(&speed_config, 0x0, sizeof(phymod_phy_speed_config_t));

    /* Read speed id from resolved status */
    PHYMOD_IF_ERR_RETURN
        (tscomod_speed_id_get(&phy_copy.access, &speed_id));

    /* Read the speed entry and then decode the speed and FEC type */
    phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

    /* Decode speed entry */
    tscjmod_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    PHYMOD_IF_ERR_RETURN
        (_tscj_speed_table_entry_to_speed_config_get(&speed_config_entry, &speed_config));

    *cl72_en = speed_config.linkTraining;

    return PHYMOD_E_NONE;
}

int tscj_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    int i;
    uint32_t cl72_en;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int an_en, an_done, speed_id;
    tscjmod_spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    jupiter_lt_status_t lt_status;
    phymod_phy_speed_config_t speed_config;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMSET(&lt_status, 0x0, sizeof(jupiter_lt_status_t));
    PHYMOD_MEMSET(&speed_config, 0x0, sizeof(phymod_phy_speed_config_t));
    status->locked = 0;

    /* Get the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    /* check check if AN enabled */
    PHYMOD_IF_ERR_RETURN
       (tscomod_autoneg_status_get(&phy_copy.access, &an_en, &an_done));


    PHYMOD_IF_ERR_RETURN
        (tscomod_speed_id_get(&phy_copy.access, &speed_id));
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));
    tscjmod_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    if (an_en && an_done) {
        /* Update num_lane for AN port */
        num_lane = 1 << speed_config_entry.num_lanes;
    }
    PHYMOD_IF_ERR_RETURN
        (_tscj_speed_table_entry_to_speed_config_get(&speed_config_entry, &speed_config));
    cl72_en = speed_config.linkTraining;

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);

        PHYMOD_IF_ERR_RETURN(jupiter_pmd_link_training_status_get(&phy_copy.access, &lt_status));
        if (cl72_en) {
            
            status->locked = lt_status.lt_rx_ready;
            return PHYMOD_E_NONE;
        }
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_loopback_set(const phymod_phy_access_t* phy,
                          phymod_loopback_mode_t loopback, uint32_t enable)
{
    int i;
    int start_lane, num_lane, port_enable;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Get the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
        case phymodLoopbackGlobal :
        case phymodLoopbackGlobalPMD :
            /* Disable port */
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tscomod_disable_set(&phy_copy.access));
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 0x1 << (i + start_lane);
                PHYMOD_IF_ERR_RETURN(jupiter_pmd_analog_loopback_set(&phy_copy.access, enable));
            }
            /* Enable port */
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tscomod_enable_set(&phy_copy.access));
            break;
        case phymodLoopbackRemotePMD :
            /* get current port enable bit */
            PHYMOD_IF_ERR_RETURN
                (tscomod_port_enable_get(&phy_copy.access, &port_enable));
            if (port_enable) {
                phy_copy.access.lane_mask = 1 << start_lane;
                PHYMOD_IF_ERR_RETURN
                    (tscomod_disable_set(&phy_copy.access));
            }

            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 0x1 << (i + start_lane);
                PHYMOD_IF_ERR_RETURN(jupiter_pmd_remote_loopback_set(&phy_copy.access, enable));
            }

            /* Release the pcs lane reset if port is enabled */
            if (port_enable) {
                phy_copy.access.lane_mask = 1 << start_lane;
                PHYMOD_IF_ERR_RETURN
                    (tscomod_enable_set(&phy_copy.access));
            }
            break;
        case phymodLoopbackRemotePCS :
        default :
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_UNAVAIL,
                               (_PHYMOD_MSG("This mode is not supported\n")));
            break;
    }
    return PHYMOD_E_NONE;
}

int tscj_phy_loopback_get(const phymod_phy_access_t* phy,
                          phymod_loopback_mode_t loopback, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (loopback) {
    case phymodLoopbackGlobal :
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_analog_loopback_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(jupiter_pmd_remote_loopback_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePCS :
    default :
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

/* Core initialization
 * (PASS1)
 * 1.  De-assert PMD lane reset
 * 2.  Switch PMD register access clock 
 * 3.  Micro code load and verify
 * (PASS2)
 * 4.  Configure PMD lane mapping and PCS lane swap
 * 5.  Set PML0 tx/rx mux selection
 * 6.  Setup PMD link-training for each lane
 * 7.  Load speed_id_table, am_table and um_table into TSC memory
 * 8.  Config PMD polarity
 * 9.  Set FCLK period
 * 10. Program AN default timer
 */
STATIC
int _tscj_core_init_pass1(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    int  rv, lane;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;

    TSCOMOD_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    /* 1. De-assert PMD lane reset */
    for (lane = 0; lane < TSCOMOD_NOF_LANES_IN_CORE; lane++) {
        phy_access.access.lane_mask = 1 << lane;
        PHYMOD_IF_ERR_RETURN
          (tscomod_pmd_x4_reset(&phy_access.access));
    }

    /* 2. Switch PMD register access clock to LSREF. */
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_access_clk_switch(&phy_access.access, 1));

    /* 3. Micro code load and verify */
    rv = _tscj_core_firmware_load(&core_copy, init_config);
    if (rv != PHYMOD_E_NONE) {
        PHYMOD_DEBUG_ERROR(("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));
        PHYMOD_IF_ERR_RETURN(rv);
    }

    /* Switch PMD register access clock back to heartbeat. */
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_access_clk_switch(&phy_access.access, 0));

     /* need to check if the ucode load is correct or not */
    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
            rv = jupiter_ucode_load_verify(&core_copy.access);
            if (rv != PHYMOD_E_NONE) {
                PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));
                PHYMOD_IF_ERR_RETURN(rv);
            }
        }
    }

    return PHYMOD_E_NONE;
}


STATIC
int _tscj_core_init_pass2(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_polarity_t tmp_pol;
    int lane, pmd_config_bypass = 0;
    uint32_t am_table_load_size, um_table_load_size;
    uint32_t i, fclk_div_mode = 1;
    uint32_t *tscj_am_table_entry;
    uint32_t *tscj_um_table_entry;
    uint32_t *tscj_speed_priority_mapping_table;
    uint32_t expected_crc = JUPITER_UCODE_IMAGE_CRC;
    
    uint32_t nrz_an_timers[3] = {0xa7, 0x27b6, 0x27b6};
    uint32_t pam4_an_timers[3] = {0x2a, 0xca79, 0xca79};

    TSCOMOD_CORE_TO_PHY_ACCESS(&phy_access, core);
    phy_access_copy = phy_access;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    tscj_am_table_entry = tscj_am_table_entry_get();
    tscj_um_table_entry = tscj_um_table_entry_get();
    tscj_speed_priority_mapping_table = tscj_speed_priority_mapping_table_get();

    /* first check if PMD config needs to be skipped */
    if (PHYMOD_CORE_INIT_F_BYPASS_PMD_CONFIGURATION_GET(init_config)) {
        pmd_config_bypass = 1;
        core_copy.access.lane_mask = 0x10;
        phy_access_copy = phy_access;
        phy_access_copy.access = core->access;
        phy_access_copy.access.lane_mask = 0x10;
        phy_access_copy.type = core->type;
    } else {
        core_copy.access.lane_mask = 0x1;
        phy_access_copy = phy_access;
        phy_access_copy.access = core->access;
        phy_access_copy.access.lane_mask = 0x1;
        phy_access_copy.type = core->type;
    }

    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));

    /* 4. Configure PMD lane mapping and PCS lane swap */
    PHYMOD_IF_ERR_RETURN
        (tscj_core_lane_map_set(&core_copy, &init_config->lane_map));

    /* 5. Set PML0 tx/rx mux selection */
    if (!pmd_config_bypass) {
        PHYMOD_IF_ERR_RETURN
           ( _tscj_core_tx_rx_mux_select_set(&core_copy, &init_config->lane_map));
    }

    if (init_config->interface.ref_clock == phymodRefClk312Mhz) {
        PHYMOD_IF_ERR_RETURN
            (tscomod_refclk_set(&phy_access.access, TSCOMOD_REF_CLK_312P5MHZ));
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /* 6. Setup PMD link-training for each lane */
    if (!pmd_config_bypass) {
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_link_training_setup(&phy_access.access));
    }

    /* 7. Load UM table and AM table */
    am_table_load_size = TSCJMOD_AM_TABLE_SIZE > TSCJMOD_HW_AM_TABLE_SIZE ? TSCJMOD_HW_AM_TABLE_SIZE : TSCJMOD_AM_TABLE_SIZE;
    um_table_load_size = TSCJMOD_UM_TABLE_SIZE > TSCJMOD_HW_UM_TABLE_SIZE ? TSCJMOD_HW_UM_TABLE_SIZE : TSCJMOD_UM_TABLE_SIZE;

    for (i = 0; i < am_table_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemAMTable, i,  (tscj_am_table_entry + i * TSCJMOD_AM_ENTRY_SIZE)));
    }

    for (i = 0; i < um_table_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemUMTable, i,  (tscj_um_table_entry + i * TSCJMOD_UM_ENTRY_SIZE)));
    }

    /* Load speed_priority_mapping_table */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&core_copy.access, phymodMemSpeedPriorityMapTable, 0,  tscj_speed_priority_mapping_table));

    if ((init_config->firmware_load_method != phymodFirmwareLoadMethodNone) && (!pmd_config_bypass)) {
        /* Check the FW crc checksum error */
        if (!PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_GET(init_config)) {
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_uc_ucode_crc_verify(&phy_access.access, expected_crc));
        }
    }

    /* 8. RX/TX polarity configuration */
    if (!pmd_config_bypass) {
        for (lane = 0; lane < TSCOMOD_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            tmp_pol.tx_polarity = (init_config->polarity_map.tx_polarity) >> lane & 0x1;
            tmp_pol.rx_polarity = (init_config->polarity_map.rx_polarity) >> lane & 0x1;
            PHYMOD_IF_ERR_RETURN
                (tscj_phy_polarity_set(&phy_access_copy, &tmp_pol));
            /* clear the tmp vairiable */
            PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
        }
    }

    COMPILER_REFERENCE(fclk_div_mode);
    /* 9. Set FCLK period. */
    PHYMOD_IF_ERR_RETURN
        (tscomod_set_fclk_period(&core_copy.access, TSCOMOD_VCO_53G, fclk_div_mode));

    /* 10. Program AN default timer  for both MMP0 and MMP1*/
    /* PML0 config */
    if (!pmd_config_bypass) {
        core_copy.access.lane_mask = 0x1;
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (tscomod_set_an_timers(&core_copy.access, init_config->interface.ref_clock,
                                   nrz_an_timers, pam4_an_timers));
        /* enable FEC COBRA as default */
        PHYMOD_IF_ERR_RETURN
            (tscomod_fec_cobra_enable(&core_copy.access, 1));

        core_copy.access.lane_mask = 0x4;
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (tscomod_set_an_timers(&core_copy.access, init_config->interface.ref_clock,
                                   nrz_an_timers, pam4_an_timers));
        /* enable FEC COBRA as default */
        PHYMOD_IF_ERR_RETURN
            (tscomod_fec_cobra_enable(&core_copy.access, 1));
    } else {
        /* PML1 config */
        core_copy.access.lane_mask = 0x10;
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (tscomod_set_an_timers(&core_copy.access, init_config->interface.ref_clock,
                                   nrz_an_timers, pam4_an_timers));
        /* enable FEC COBRA as default */
        PHYMOD_IF_ERR_RETURN
            (tscomod_fec_cobra_enable(&core_copy.access, 1));

        core_copy.access.lane_mask = 0x40;
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (tscomod_set_an_timers(&core_copy.access, init_config->interface.ref_clock,
                                   nrz_an_timers, pam4_an_timers));
        /* enable FEC COBRA as default */
        PHYMOD_IF_ERR_RETURN
            (tscomod_fec_cobra_enable(&core_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}

int tscj_core_init(const phymod_core_access_t* core,
                   const phymod_core_init_config_t* init_config,
                   const phymod_core_status_t* core_status)
{
    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscj_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscj_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    //jupiter_txfir_config_t txfir_cfg;
    jupiter_pll_pstatus_t pll_pwr_stat;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&pll_pwr_stat, 0x0, sizeof(jupiter_pll_pstatus_t));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&pm_phy_copy.access, &start_lane, &num_lane));
    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (tscomod_pmd_x4_reset(&pm_phy_copy.access));

    /* next check if 8 lane port, if so need to modify PMD tx/rx mux selection */
    if (num_lane == 8) {
        PHYMOD_IF_ERR_RETURN
            (tscomod_pcs_rx_mux_select(&pm_phy_copy.access, 0x0));
        PHYMOD_IF_ERR_RETURN
            (tscomod_pcs_tx_mux_select(&pm_phy_copy.access, 0x0));
    }


    for (i = 0; i < num_lane; i++) {
#if JUPITER_BRINGUP_DONE
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        txfir_cfg.CM3 = init_config->tx[i].pre3;
        txfir_cfg.CM2 = init_config->tx[i].pre2;
        txfir_cfg.CM1 = init_config->tx[i].pre;
        txfir_cfg.C0 = init_config->tx[i].main;
        txfir_cfg.C1 = init_config->tx[i].post;

        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_txfir_config_set(&pm_phy_copy.access, txfir_cfg));
#endif
    }

    /* make sure that power up PLL is locked */
    pm_phy_copy.access.pll_idx = 0;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_pll_pwrdn_get(&pm_phy_copy.access, &pll_pwr_stat));

    if (pll_pwr_stat == JUPITER_PWR_UP) {
        uint32_t cnt = 0, pll_lock = 0;
        cnt = 0;
        while (cnt < 1000) {
            PHYMOD_IF_ERR_RETURN(jupiter_pmd_pll_lock_get(&pm_phy_copy.access, &pll_lock));
            cnt = cnt + 1;
            if (pll_lock) {
                break;
            } else {
                if(cnt == 1000) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL0 is not locked within 10 milli second \n", pm_phy_copy.access.addr));
                    break;
                }
            }
            PHYMOD_USLEEP(10);
        }
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(tscomod_get_pcs_latched_link_status(&pm_phy_copy.access, link_status));
    return PHYMOD_E_NONE;
}

int tscj_phy_tx_taps_default_get(const phymod_phy_access_t* phy,
                                 phymod_phy_signalling_method_t mode,
                                 phymod_tx_t* tx)
{
    uint32_t cm3, cm2, cm1, c1, c0;

    PHYMOD_IF_ERR_RETURN
       (jupiter_pmd_tx_tap_mode_get(&cm3, &cm2, &cm1, &c1, &c0));

    tx->pre3 = cm3;
    tx->pre2 = cm2;
    tx->pre = cm1;
    tx->main = c0;
    tx->post = c1;
    tx->post2 = 0;
    tx->post3 = 0;

    return PHYMOD_E_NONE;
}

int tscj_phy_lane_config_default_get(const phymod_phy_access_t* phy,
                                     phymod_phy_signalling_method_t mode,
                                     phymod_firmware_lane_config_t* lane_config)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    int pcs_num_copy = 0;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* first check if PMD register */
    if (reg_addr & 0x10000) {

        PHYMOD_IF_ERR_RETURN(pmd_read_addr(&phy->access, reg_addr, val));
    } else {
        /* need to figure out 1 /2 or 4 copy register */
        PHYMOD_IF_ERR_RETURN
            (tscomod_pcs_reg_num_copy_get(reg_addr, &pcs_num_copy));
        if (pcs_num_copy == 1) {
            phy_copy.access.lane_mask &= 0xff;
            PHYMOD_IF_ERR_RETURN(phymod_tsco1_iblk_read(&phy_copy.access, reg_addr, val));
        } else if (pcs_num_copy == 2) {
            phy_copy.access.lane_mask &= 0xff;
            PHYMOD_IF_ERR_RETURN(phymod_tsco2_iblk_read(&phy_copy.access, reg_addr, val));
        } else if (pcs_num_copy == 4) {
            PHYMOD_IF_ERR_RETURN(phymod_tsco4_iblk_read(&phy->access, reg_addr, val));
        } else {
            PHYMOD_DEBUG_ERROR(("ERROR :: invalid pcs register address 0x%x \n", reg_addr));
            return PHYMOD_E_UNAVAIL;
        }
    }
    return PHYMOD_E_NONE;
}

int tscj_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    int pcs_num_copy = 0;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* first check if PMD register */
    if (reg_addr & 0x10000) {
        PHYMOD_IF_ERR_RETURN(pmd_write_addr(&phy->access, reg_addr, val));
    } else {
        /* need to figure out 1 /2 or 4 copy register */
        PHYMOD_IF_ERR_RETURN
            (tscomod_pcs_reg_num_copy_get(reg_addr, &pcs_num_copy));
        if (pcs_num_copy == 1) {
            phy_copy.access.lane_mask &= 0xff;
            PHYMOD_IF_ERR_RETURN(phymod_tsco1_iblk_write(&phy_copy.access, reg_addr, val));
        } else if (pcs_num_copy == 2) {
            phy_copy.access.lane_mask &= 0xff;
            PHYMOD_IF_ERR_RETURN(phymod_tsco2_iblk_write(&phy_copy.access, reg_addr, val));
        } else if (pcs_num_copy == 4) {
            PHYMOD_IF_ERR_RETURN(phymod_tsco4_iblk_write(&phy->access, reg_addr, val));
        } else {
            PHYMOD_DEBUG_ERROR(("ERROR :: invalid pcs register address 0x%x \n", reg_addr));
            return PHYMOD_E_UNAVAIL;
        }
    }
    return PHYMOD_E_NONE;
}

int tscj_phy_firmware_load_info_get(const phymod_phy_access_t* phy, phymod_firmware_load_info_t* info)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_autoneg_advert_ability_set(const phymod_phy_access_t* phy,
                                        const phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                        const phymod_phy_pll_state_t* old_pll_adv_state,
                                        phymod_phy_pll_state_t* new_pll_adv_state)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    /* Program local advert abilitiy registers */
    PHYMOD_IF_ERR_RETURN
        (tscomod_autoneg_ability_set(&phy_copy.access, an_advert_abilities));

    return PHYMOD_E_NONE;
}

int tscj_phy_autoneg_advert_ability_get(const phymod_phy_access_t* phy,
                                        phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tscomod_autoneg_ability_get(&phy_copy.access, an_advert_abilities));

    return PHYMOD_E_NONE;
}

int tscj_phy_autoneg_remote_advert_ability_get(const phymod_phy_access_t* phy,
                                               phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tscomod_autoneg_remote_ability_get(&phy_copy.access, an_advert_abilities));

    return PHYMOD_E_NONE;
}

int tscj_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded, mapped_speed_id;
    int i, start_lane, num_lane;
    tscomod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    tscomod_spd_intfc_type_t spd_intf = 0;

    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* Set the port an mode enable bit */
    PHYMOD_IF_ERR_RETURN
        (tscomod_port_an_mode_enable_set(&phy_copy.access, (int) an->enable));

    phy_copy.access.lane_mask = 0x1 << start_lane;

    if (an->enable) {
        PHYMOD_IF_ERR_RETURN(tscomod_disable_set(&phy_copy.access));
        /* Adjust FEC arch for RS544/RS272 */
        if (PHYMOD_AN_F_FEC_RS272_CLR_GET(an)) {
            if (start_lane < 2) {
                phy_copy.access.lane_mask = 0x1;
            } else if (start_lane < 4) {
                phy_copy.access.lane_mask = 0x4;
            } else if (start_lane < 6) {
                phy_copy.access.lane_mask = 0x10;
            } else if (start_lane < 8) {
                phy_copy.access.lane_mask = 0x40;
            }
            PHYMOD_IF_ERR_RETURN(tscomod_rsfec_cw_type_set(&phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_544, 0));
            if (num_lane == 8) {
                /* need to check if 400G 8 lane or 400G 4 lane ability are advertized */
                uint32_t an_400g_8lane_ability_enabled = 0;
                uint32_t an_400g_4lane_ability_enabled = 0;
                PHYMOD_IF_ERR_RETURN
                    (tscomod_autoneg_ability_400g_8lane_get(&phy_copy.access, &an_400g_8lane_ability_enabled));
                PHYMOD_IF_ERR_RETURN
                    (tscomod_autoneg_ability_400g_4lane_get(&phy_copy.access, &an_400g_4lane_ability_enabled));
                if (an_400g_8lane_ability_enabled | an_400g_4lane_ability_enabled) {
                    phy_copy.access.lane_mask = 0x4;
                    PHYMOD_IF_ERR_RETURN
                        (tscomod_rsfec_cw_type_set(&phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_544, 0));
                }
            } else if (num_lane == 4) {
                /* need to check if 400G 4 lane is advertized, if so, both MPP needs to be adjusted */
                uint32_t an_400g_4lane_ability_enabled = 0;
                PHYMOD_IF_ERR_RETURN
                    (tscomod_autoneg_ability_400g_4lane_get(&phy_copy.access, &an_400g_4lane_ability_enabled));
                if (an_400g_4lane_ability_enabled) {
                    phy_copy.access.lane_mask = 1 << (start_lane + 2);
                    PHYMOD_IF_ERR_RETURN
                        (tscomod_rsfec_cw_type_set(&phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_544, 0));
                }
            }
        } else if (PHYMOD_AN_F_FEC_RS272_REQ_GET(an)) {
            if (start_lane < 2) {
                phy_copy.access.lane_mask = 0x1;
            } else if (start_lane < 4) {
                phy_copy.access.lane_mask = 0x4;
            } else if (start_lane < 6) {
                phy_copy.access.lane_mask = 0x10;
            } else if (start_lane < 8) {
                phy_copy.access.lane_mask = 0x40;
            }
            PHYMOD_IF_ERR_RETURN
                (tscomod_rsfec_cw_type_set(&phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_272, 0));
            if (num_lane == 8) {
                /* need to check if 400G 8 lane or 400G 4 lane ability are advertized */
                uint32_t an_400g_8lane_ability_enabled = 0;
                uint32_t an_400g_4lane_ability_enabled = 0;
                PHYMOD_IF_ERR_RETURN
                    (tscomod_autoneg_ability_400g_8lane_get(&phy_copy.access, &an_400g_8lane_ability_enabled));
                PHYMOD_IF_ERR_RETURN
                    (tscomod_autoneg_ability_400g_4lane_get(&phy_copy.access, &an_400g_4lane_ability_enabled));
                if (an_400g_8lane_ability_enabled | an_400g_4lane_ability_enabled) {
                    phy_copy.access.lane_mask = 0x4;
                    PHYMOD_IF_ERR_RETURN
                        (tscomod_rsfec_cw_type_set(&phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_272, 0));
                }
            } else if (num_lane == 4) {
                /* need to check if 400G 4 lane is advertized, if so,  MPP1 needs to be adjusted */
                uint32_t an_400g_4lane_ability_enabled = 0;
                PHYMOD_IF_ERR_RETURN
                    (tscomod_autoneg_ability_400g_4lane_get(&phy_copy.access, &an_400g_4lane_ability_enabled));
                if (an_400g_4lane_ability_enabled) {
                    phy_copy.access.lane_mask = 1 << (start_lane + 2);
                    PHYMOD_IF_ERR_RETURN
                    (tscomod_rsfec_cw_type_set(&phy_copy.access, TSCOMOD_RS_FEC_CW_TYPE_272, 0));
                }
            }
        }
    }

    switch (an->num_lane_adv) {
        case 1:
            num_lane_adv_encoded = 0;
            break;
        case 2:
            num_lane_adv_encoded = 1;
            break;
        case 4:
            num_lane_adv_encoded = 2;
            break;
        case 8:
            num_lane_adv_encoded = 3;
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    switch (an->an_mode) {
        case phymod_AN_MODE_CL73:
            an_control.an_type = TSCOMOD_AN_MODE_CL73;
            break;
        case phymod_AN_MODE_CL73BAM:
            an_control.an_type = TSCOMOD_AN_MODE_CL73_BAM;
            break;
        case phymod_AN_MODE_CL73_MSA:
            an_control.an_type = TSCOMOD_AN_MODE_CL73_MSA;
            break;
        default:
            return PHYMOD_E_PARAM;
            break;
    }

    if (an->enable) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        /* Set AN port mode */
        PHYMOD_IF_ERR_RETURN
            (tscomod_set_an_port_mode(&phy_copy.access, start_lane));

        /* load 53G VCO spd_id */
        spd_intf = TSCOMOD_SPD_CL73_IEEE_53G;

        PHYMOD_IF_ERR_RETURN
            (tscomod_get_mapped_speed(spd_intf, &mapped_speed_id));

        phy_copy.access.lane_mask = 0x1 << start_lane;
        phy_copy.access.pll_idx = 0;

        /* Load 1G speed ID */
        PHYMOD_IF_ERR_RETURN
            (tscomod_set_sc_speed(&phy_copy.access, mapped_speed_id, 0));
    } else {
        
        /* Disable Tx PAM4 pre-coding. It might be enabled by AN link training. */
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_tx_pam4_precoder_override_set(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN
                (jupiter_pmd_tx_pam4_precoder_enable_set(&phy_copy.access, 0, 0));
        }
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tscomod_autoneg_control(&phy_copy.access, &an_control));

    return PHYMOD_E_NONE;
}

int tscj_phy_autoneg_get(const phymod_phy_access_t* phy,
                         phymod_autoneg_control_t* an, uint32_t* an_done)
{
    tscomod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    int an_complete = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(tscomod_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (tscomod_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

    if (an_control.enable) {
        an->enable = 1;
        *an_done = an_complete;
    } else {
        an->enable = 0;
    }

    switch (an_control.num_lane_adv) {
        case 0:
            an->num_lane_adv = 1;
            break;
        case 1:
            an->num_lane_adv = 2;
            break;
        case 2:
            an->num_lane_adv = 4;
            break;
        case 3:
            an->num_lane_adv = 8;
            break;
        default:
            an->num_lane_adv = 0;
            break;
    }

    switch (an_control.an_type) {
        case TSCOMOD_AN_MODE_CL73:
            an->an_mode = phymod_AN_MODE_CL73;
            break;
        case TSCOMOD_AN_MODE_CL73_BAM:
            an->an_mode = phymod_AN_MODE_CL73BAM;
            break;
        case TSCOMOD_AN_MODE_MSA:
            an->an_mode = phymod_AN_MODE_MSA;
            break;
        case TSCOMOD_AN_MODE_CL73_MSA:
            an->an_mode = phymod_AN_MODE_CL73_MSA;
            break;
        default:
            an->an_mode = phymod_AN_MODE_NONE;
            break;
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    int an_en, an_done;
    phymod_phy_speed_config_t speed_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, speed_id;
    uint32_t packed_entry[5];
    tscjmod_spd_id_tbl_entry_t speed_config_entry;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
       (tscomod_autoneg_status_get(&phy_copy.access, &an_en, &an_done));

    PHYMOD_IF_ERR_RETURN
       (tscj_phy_speed_config_get(phy, &speed_config));

    if (an_en && an_done) {
        uint32_t an_resolved_mode;
        PHYMOD_IF_ERR_RETURN
            (tscomod_speed_id_get(&phy_copy.access, &speed_id));
        /* Read the speed id entry and get the num_lane info */
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));
        tscjmod_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);
        num_lane = 1 << speed_config_entry.num_lanes;
        /* Read the AN final resolved port mode */
        PHYMOD_IF_ERR_RETURN
            (tscomod_resolved_port_mode_get(&phy_copy.access, &an_resolved_mode));
        status->resolved_port_mode = an_resolved_mode;
    }

    status->enabled   = an_en;
    status->locked    = an_done;
    status->data_rate = speed_config.data_rate;
    status->resolved_num_lane = num_lane;

    return PHYMOD_E_NONE;
}

int tscj_phy_pll_reconfig(const phymod_phy_access_t* phy,
                          uint8_t pll_index,
                          uint32_t pll_div,
                          phymod_ref_clk_t ref_clk1)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_tx_pam4_precoder_enable_set(const phymod_phy_access_t* phy, int enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint32_t pcs_lane_enable;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tscomod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* Disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_disable_set(&phy_copy.access));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_tx_pam4_precoder_override_set(&phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_tx_pam4_precoder_enable_set(&phy_copy.access, enable, 0));
    }

    /* Re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_tx_pam4_precoder_enable_get(const phymod_phy_access_t* phy, int *enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t ovr_en = 0, plusd_en = 0, gray_en = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_tx_pam4_precoder_override_get(&phy_copy.access, &ovr_en));
    PHYMOD_IF_ERR_RETURN
        (jupiter_pmd_tx_pam4_precoder_enable_get(&phy_copy.access, &gray_en, &plusd_en));

    *enable = ovr_en ? (gray_en || plusd_en) : 0;

    return PHYMOD_E_NONE;
}

int tscj_timesync_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{

    return PHYMOD_E_NONE;
}

int tscj_timesync_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t* enable)
{

    return PHYMOD_E_NONE;
}

int tscj_timesync_offset_set(const phymod_core_access_t* core, uint32_t ts_offset)
{
    return PHYMOD_E_NONE;
}

int tscj_timesync_offset_get(const phymod_core_access_t* core, uint32_t *ts_offset)
{
    return PHYMOD_E_NONE;
}

int tscj_timesync_adjust_set(const phymod_phy_access_t* phy,
                             uint32_t flags, uint32_t ts_am_norm_mode)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_pcs_lane_swap_adjust(const phymod_phy_access_t* phy,
                                  uint32_t active_lane_map,
                                  uint32_t original_tx_lane_map,
                                  uint32_t original_rx_lane_map)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, disable_lane[2]= {0,0}, disable_lane_num = 0;
    uint32_t new_tx_lane_map, new_rx_lane_map, swap_mask, tmp_tx_lane_map, tmp_rx_lane_map;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next based on active_lane_map, figure out the disable lane num  and index*/
    for (i = 0; i < num_lane; i++)
    {
        if(active_lane_map & (1 << (i + start_lane)))
        {
            continue;
        } else {
            disable_lane[disable_lane_num] = i + start_lane;
            disable_lane_num++;
        }
    }

    /* next check if disable_lane_num 0 */
    if (disable_lane_num == 0) {
        new_tx_lane_map = original_tx_lane_map;
        new_rx_lane_map = original_rx_lane_map;

        PHYMOD_IF_ERR_RETURN
            (tscomod_pcs_tx_m1_lane_swap(&phy_copy.access, new_tx_lane_map));
        PHYMOD_IF_ERR_RETURN
            (tscomod_pcs_rx_lane_swap(&phy_copy.access, new_rx_lane_map));
    } else  {
        if (num_lane == 4) {
            /* first check if disable lane is the last, if yes, then need to adjust */
            if ((disable_lane[0] == 3) || (disable_lane[0] == 7)) {
                return PHYMOD_E_NONE;
            } else if (disable_lane[0] < 4) {
                /* PML0 */
                new_tx_lane_map = (original_tx_lane_map & (0xf << (3 * 4)) & 0xffffffff) >> (4 * (3 - disable_lane[0]));
                new_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (3 - disable_lane[0]));
                new_rx_lane_map = (original_rx_lane_map & (0xf << (3 * 4)) & 0xffffffff) >> (4 * (3 - disable_lane[0]));
                new_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (3 - disable_lane[0]));
                swap_mask = ~(0xf << (4 * 3) | 0xf << (4 * disable_lane[0]));
                new_tx_lane_map |=  (original_tx_lane_map & swap_mask);
                new_rx_lane_map |=  (original_rx_lane_map & swap_mask);

                PHYMOD_IF_ERR_RETURN
                    (tscomod_pcs_tx_m1_lane_swap(&phy_copy.access, new_tx_lane_map));
                PHYMOD_IF_ERR_RETURN
                    (tscomod_pcs_rx_lane_swap(&phy_copy.access, new_rx_lane_map));
            } else {
                /* PML1 */
                new_tx_lane_map = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                new_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[0]));
                new_rx_lane_map = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                new_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[0]));
                swap_mask = ~(0xf << (4 * 7) | 0xf << (4 * disable_lane[0]));
                new_tx_lane_map |=  (original_tx_lane_map & swap_mask);
                new_rx_lane_map |=  (original_rx_lane_map & swap_mask);

                PHYMOD_IF_ERR_RETURN
                    (tscomod_pcs_tx_m1_lane_swap(&phy_copy.access, new_tx_lane_map));
                PHYMOD_IF_ERR_RETURN
                    (tscomod_pcs_rx_lane_swap(&phy_copy.access, new_rx_lane_map));
            }
        } else if (num_lane == 8) {
            /* first check if disable lane is the last, if yes, then no need to adjust */
            if (disable_lane_num == 1) {
                /* no need to do pcs adjust */
                if (disable_lane[0] == 7) {
                    return PHYMOD_E_NONE;
                } else {
                    new_tx_lane_map = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                    new_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[0]));
                    new_rx_lane_map = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                    new_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[0]));
                    swap_mask = ~(0xf << (4 * 7) | 0xf << (4 * disable_lane[0]));
                    new_tx_lane_map |=  (original_tx_lane_map & swap_mask);
                    new_rx_lane_map |=  (original_rx_lane_map & swap_mask);
                }
            } else { /*2 lane disabled */
                if ((disable_lane[0] == 6)  && (disable_lane[1] == 7)) {
                    /* no need to do pcs adjust */
                    return PHYMOD_E_NONE;
                } else if (disable_lane[1] != 6) {
                    new_tx_lane_map = (original_tx_lane_map & (0xf << (6 * 4)) & 0xffffffff) >> (4 * (6 - disable_lane[0]));
                    new_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (6 - disable_lane[0]));
                    tmp_tx_lane_map = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[1]));
                    tmp_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[1] * 4)) & 0xffffffff) <<  (4 * (7 - disable_lane[1]));
                    new_tx_lane_map |= tmp_tx_lane_map;
                    new_rx_lane_map = (original_rx_lane_map & (0xf << (6 * 4)) & 0xffffffff) >> (4 * (6 - disable_lane[0]));
                    new_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (6 - disable_lane[0]));
                    tmp_rx_lane_map = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[1]));
                    tmp_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[1] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[1]));
                    new_rx_lane_map |= tmp_rx_lane_map;
                    swap_mask = ~(0xff << (4 * 6) | 0xf << (4 * disable_lane[0]) | 0xf << (4 * disable_lane[1]));
                    new_tx_lane_map |=  (original_tx_lane_map & swap_mask);
                    new_rx_lane_map |=  (original_rx_lane_map & swap_mask);
                } else {
                    tmp_tx_lane_map = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                    tmp_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) <<  (4 * (7 - disable_lane[0]));
                    new_tx_lane_map = tmp_tx_lane_map;
                    tmp_rx_lane_map = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                    tmp_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[0]));
                    new_rx_lane_map = tmp_rx_lane_map;
                    swap_mask = ~(0xf << (4 * 7) | 0xf << (4 * disable_lane[0]));
                    new_tx_lane_map |=  (original_tx_lane_map & swap_mask);
                    new_rx_lane_map |=  (original_rx_lane_map & swap_mask);
                }
            }

            PHYMOD_IF_ERR_RETURN
                (tscomod_pcs_tx_m1_lane_swap(&phy_copy.access, new_tx_lane_map));
            PHYMOD_IF_ERR_RETURN
                (tscomod_pcs_rx_lane_swap(&phy_copy.access, new_rx_lane_map));
        }
    }
    return PHYMOD_E_NONE;
}

int tscj_phy_load_speed_id_entry(const phymod_phy_access_t* phy,
                                 uint32_t speed,
                                 uint32_t num_lane,
                                 phymod_fec_type_t fec_type)
{
    phymod_phy_access_t phy_copy;
    int start_lane, local_num_lane, mapped_speed_id;
    tscomod_spd_intfc_type_t spd_intf = TSCOMOD_SPD_ILLEGAL;
    uint32_t *tscj_spd_id_entry_53;

    tscj_spd_id_entry_53 = tscj_spd_id_entry_53_get();

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &local_num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Check the speed */
    switch (speed) {
    case 150000:
        spd_intf = TSCOMOD_SPD_150G_FEC_544_2XN_N3;
        break;
    case 350000:
        spd_intf = TSCOMOD_SPD_350G_FEC_544_2XN_N7;
        break;
    case 300000:
        spd_intf = TSCOMOD_SPD_300G_FEC_544_2XN_N6;
        break;
    case 400000:
        if (local_num_lane == 4) {
            if (fec_type == phymod_fec_RS544_2XN) {
                spd_intf = TSCOMOD_SPD_400G_IEEE_KR4_CR4;
            } else if (fec_type == phymod_fec_RS272_2XN) {
                spd_intf = TSCOMOD_SPD_400G_BRCM_FEC_272_KR4_CR4;
            } else {
                PHYMOD_DEBUG_ERROR(("Unsupported 400G fec type\n"));
                return PHYMOD_E_UNAVAIL;
            }
        } else if (local_num_lane == 8) {
            if (fec_type == phymod_fec_RS544_2XN) {
                spd_intf = TSCOMOD_SPD_400G_BRCM_FEC_KR8_CR8;
            } else if (fec_type == phymod_fec_RS272_2XN) {
                spd_intf = TSCOMOD_SPD_400G_BRCM_FEC_272_N8;
            } else {
                PHYMOD_DEBUG_ERROR(("Unsupported 400G fec type\n"));
                return PHYMOD_E_UNAVAIL;
            }
        } else {
            PHYMOD_DEBUG_ERROR(("Unsupported num lane for 400G\n"));
            return PHYMOD_E_UNAVAIL;
        }
        break;
    case 200000:
        if (local_num_lane == 4) {
            if (fec_type == phymod_fec_RS544_2XN) {
                spd_intf = TSCOMOD_SPD_200G_IEEE_KR4_CR4;
            } else if (fec_type == phymod_fec_RS544) {
                spd_intf = TSCOMOD_SPD_200G_BRCM_KR4_CR4;
            } else if (fec_type == phymod_fec_RS272) {
                spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_272_N4;
            } else if (fec_type == phymod_fec_RS272_2XN) {
                spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_272_CR4_KR4;
            } else {
                PHYMOD_DEBUG_ERROR(("Unsupported 200G fec type\n"));
                return PHYMOD_E_UNAVAIL;
            }
        } else if (local_num_lane == 8) {
            if (fec_type == phymod_fec_RS544_2XN) {
                spd_intf = TSCOMOD_SPD_200G_BRCM_FEC_544_CR8_KR8;
            }
        } else {
                PHYMOD_DEBUG_ERROR(("Unsupported num lane for 200G\n"));
                return PHYMOD_E_UNAVAIL;
        }
        break;
    default:
        PHYMOD_DEBUG_ERROR(("Unsupported speed for speed id load function \n"));
        return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_IF_ERR_RETURN
        (tscomod_get_mapped_speed(spd_intf, &mapped_speed_id));

    phy_copy.access.pll_idx = 0;
    phy_copy.access.lane_mask = 1 << 0;
    /* Load 53G TVCO speed id entry */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCOMOD_FORCED_SPEED_ID_OFFSET + start_lane,
                              (tscj_spd_id_entry_53 + mapped_speed_id * TSCJMOD_SPEED_ID_ENTRY_SIZE)));

    return PHYMOD_E_NONE;
}

int tscj_timesync_tx_info_get(const phymod_phy_access_t* phy, phymod_ts_fifo_status_t* ts_tx_info)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_autoneg_speed_id_table_reload(const phymod_phy_access_t* phy, uint32_t gsh_header_enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t i, an_fec_override_enable = 0;
    uint32_t *tscj_spd_id_entry_53;
    uint32_t *tscj_spd_id_entry_53_gsh;
    uint32_t *tscj_spd_id_entry_100g_4lane_no_fec_53;

    tscj_spd_id_entry_53 = tscj_spd_id_entry_53_get();
    tscj_spd_id_entry_53_gsh = tscj_spd_id_entry_53_gsh_get();
    tscj_spd_id_entry_100g_4lane_no_fec_53 = tscj_spd_id_entry_100g_4lane_no_fec_53_get();

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    phy_copy.access.pll_idx = 0;
    phy_copy.access.lane_mask = 1 << 0;

    /* Check if fec_override is set */
    PHYMOD_IF_ERR_RETURN(tscomod_fec_override_get(&phy_copy.access, &an_fec_override_enable));

    /* Need to reload autoneg speed id table based on gsh enable or not */
    for (i = 0; i < TSCOMOD_AUTONEG_SPEED_ID_COUNT; i++) {
        if (gsh_header_enable) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, i,
                                      (tscj_spd_id_entry_53_gsh + i * TSCJMOD_SPEED_ID_ENTRY_SIZE)));
        } else {
            if (an_fec_override_enable &&
                (i == TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_KR4 ||
                 i == TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_CR4)) {
                 /* Load AN entry for CL73 100Gx4 NO FEC */
                 PHYMOD_IF_ERR_RETURN
                        (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                                          i, tscj_spd_id_entry_100g_4lane_no_fec_53));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, i,
                                      (tscj_spd_id_entry_53 + i * TSCJMOD_SPEED_ID_ENTRY_SIZE)));
            }
        }
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_pcs_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (enable == 1) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_enable_set(&phy_copy.access));
    } else if (enable == 0) {
        PHYMOD_IF_ERR_RETURN
            (tscomod_disable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy,
                                phymod_synce_clk_ctrl_t cfg)
{
    return PHYMOD_E_NONE;
}

int tscj_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy,
                                phymod_synce_clk_ctrl_t *cfg)
{

    return PHYMOD_E_NONE;
}


#define TSCOMOD_TS_ENTRY_SIZE 3
#define TSCOMOD_TS_TX_MEM_SIZE 160
#define TSCOMOD_TS_RX_MPP_MEM_SIZE 80
/* This function will handle PCS ECC interrupts.
 * 1. Clear interrupt status.
 * 2. Re-load the table entry if error is in UM, AM table.
 * 3. Re-load the 1588 tables and SPEED_ID table entry for 1b error.
 * 4. Return is_handled = 0 if 2b-error happens and Phymod can not recover.
 */
int tscj_intr_handler(const phymod_phy_access_t* phy,
                      phymod_interrupt_type_t type,
                      uint32_t* is_handled)
{
    phymod_phy_access_t phy_copy;
    tscomod_intr_status_t intr_status;
    uint32_t speed_id_table_entry[TSCJMOD_SPEED_ID_ENTRY_SIZE];
    uint32_t ts_table_entry[TSCOMOD_TS_ENTRY_SIZE];
    int index;
    uint32_t *tscj_am_table_entry;
    uint32_t *tscj_um_table_entry;

    tscj_am_table_entry = tscj_am_table_entry_get();
    tscj_um_table_entry = tscj_um_table_entry_get();

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMSET(&intr_status, 0, sizeof(intr_status));
    intr_status.type = type;

    /* Get and clear interrupt status. */
    PHYMOD_IF_ERR_RETURN
        (tscomod_intr_status_get(&phy_copy.access, &intr_status));

    phy_copy.access.lane_mask = 0x1;

    if (intr_status.is_2b_err) {
        *is_handled = 0;
    } else {
        *is_handled = 1;
    }

    switch (type) {
        case phymodIntrEccAMTable:
            if (intr_status.is_2b_err || intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCJMOD_HW_AM_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemAMTable, index,
                                      (tscj_am_table_entry + index * TSCJMOD_AM_ENTRY_SIZE)));
                *is_handled = 1;
            }
            break;
        case phymodIntrEccSpeedTable:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCJMOD_HW_SPEED_ID_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable,
                                     index, speed_id_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                                      index, speed_id_table_entry));
            }
            break;
        case phymodIntrEccUMTable:
            if (intr_status.is_2b_err || intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCJMOD_HW_UM_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemUMTable, index,
                                      (tscj_um_table_entry + index * TSCJMOD_UM_ENTRY_SIZE)));
                *is_handled = 1;
            }
            break;
        case phymodIntrEccRx1588Mpp1:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCOMOD_TS_RX_MPP_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemRxLkup1588Mpp1,
                                     index, ts_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemRxLkup1588Mpp1,
                                      index, ts_table_entry));
            }
            break;
        case phymodIntrEccRx1588Mpp0:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCOMOD_TS_RX_MPP_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemRxLkup1588Mpp0,
                                     index, ts_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemRxLkup1588Mpp0,
                                      index, ts_table_entry));
            }
            break;
        case phymodIntrEccTx1588Mpp0:
            /*
             * TscO only has one TX 1588 memory.
             * Here we use phymodIntrEccTx1588Mpp0 to represent
             * TX 1588 ECC error type.
             */
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCOMOD_TS_TX_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemTxLkup1588Mpp0,
                                     index, ts_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemTxLkup1588Mpp0,
                                      index, ts_table_entry));
            }
            break;
        default:
            break;
    }

    if (!(*is_handled)) {
        /* Disable the interrupt to avoid continuous firing */
        switch (type) {
            case phymodIntrEccRsFECMpp1:
            case phymodIntrEccRsFECMpp0:
            case phymodIntrEccRsFECRbufMpp1:
            case phymodIntrEccRsFECRbufMpp0:
                PHYMOD_IF_ERR_RETURN(tscomod_interrupt_enable_set(&phy_copy.access,
                                                                  type, 0));
                break;
            default:
                break;

        }
    }


    return PHYMOD_E_NONE;
}

int tscj_phy_codec_mode_set(const phymod_phy_access_t* phy, phymod_phy_codec_mode_t codec_type)
{
    phymod_phy_access_t phy_copy;
    tscjmod_spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    int start_lane, num_lane;
    uint32_t pcs_lane_enable;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tscomod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* Disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_disable_set(&phy_copy.access));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* Read the current speed id entry for this port */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, TSCOMOD_FORCED_SPEED_ID_OFFSET + start_lane, packed_entry));
    tscjmod_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    /* Update the codec field */
    speed_config_entry.codec_mode = codec_type;

    /* Pack the speed config entry into 5 word format */
    tscjmod_spd_ctrl_pack_spd_id_tbl_entry(&speed_config_entry, &packed_entry[0]);

    /* write back to the speed id HW table */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCOMOD_FORCED_SPEED_ID_OFFSET + start_lane, &packed_entry[0]));

    /* Re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_codec_mode_get(const phymod_phy_access_t* phy, phymod_phy_codec_mode_t* codec_type)
{
    phymod_phy_access_t phy_copy;
    tscjmod_spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    int start_lane, num_lane, speed_id;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    phy_copy.access.lane_mask = 1 << start_lane;

    /* Get speed id from resolved status */
    PHYMOD_IF_ERR_RETURN
        (tscomod_speed_id_get(&phy_copy.access, &speed_id));

    /* Get the current speed id entry for the port */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

    tscjmod_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    /* Get codec type */
    *codec_type = speed_config_entry.codec_mode;

    return PHYMOD_E_NONE;
}

int tscj_phy_fec_bypass_indication_set(const phymod_phy_access_t* phy,
                                       uint32_t enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    uint32_t pcs_lane_enable;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    /* Check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tscomod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* Disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_disable_set(&phy_copy.access));
    }

    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
      (tscomod_fec_bypass_indication_set(&phy_copy.access, enable));

    /* Re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tscomod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_fec_bypass_indication_get(const phymod_phy_access_t* phy,
                                       uint32_t *enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    PHYMOD_IF_ERR_RETURN
      (tscomod_fec_bypass_indication_get(&phy_copy.access, enable));

    return PHYMOD_E_NONE;
}

int tscj_phy_rs_fec_rxp_get(const phymod_phy_access_t* phy, uint32_t* hi_ser_lh, uint32_t* hi_ser_live)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_generic_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tscomod_rs_fec_hi_ser_get(&phy_copy.access, hi_ser_lh, hi_ser_live));

    return PHYMOD_E_NONE;
}

int tscj_phy_pmd_override_enable_set(const phymod_phy_access_t* phy,
                                           phymod_override_type_t pmd_override_type,
                                           uint32_t override_enable,
                                           uint32_t override_val)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (tscomod_pmd_override_enable_set(&phy_copy.access, pmd_override_type, override_enable, override_val));

    return PHYMOD_E_NONE;
}

int tscj_phy_fec_override_set (const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t *tscj_spd_id_entry_53 = tscj_spd_id_entry_53_get();
    uint32_t *tscj_spd_id_entry_100g_4lane_no_fec_53 = tscj_spd_id_entry_100g_4lane_no_fec_53_get();

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1;

    if ((enable ==  0) || (enable == 1)) {
        PHYMOD_IF_ERR_RETURN(tscomod_fec_override_set(&phy_copy.access, enable));
    } else {
        PHYMOD_DEBUG_ERROR(("ERROR :: Supported input values: 1 to set FEC override, 0 to disable FEC override\n"));
    }

    phy_copy.access.pll_idx = 0;
    if (enable) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                              TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_KR4,
                              tscj_spd_id_entry_100g_4lane_no_fec_53));
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                              TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_CR4,
                              tscj_spd_id_entry_100g_4lane_no_fec_53));
    } else {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                              TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_KR4,
                              (tscj_spd_id_entry_53 + (TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_KR4 * TSCJMOD_SPEED_ID_ENTRY_SIZE))));
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                              TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_CR4,
                              (tscj_spd_id_entry_53 + (TSCJ_SPEED_ID_INDEX_100G_4_LANE_CL73_CR4 * TSCJMOD_SPEED_ID_ENTRY_SIZE))));
    }

    return PHYMOD_E_NONE;
}

int tscj_phy_fec_override_get (const phymod_phy_access_t* phy, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1;
    PHYMOD_IF_ERR_RETURN(tscomod_fec_override_get(&phy_copy.access, enable));

    return PHYMOD_E_NONE;

}

int tscj_phy_interrupt_enable_set(const phymod_phy_access_t* phy,
                                        phymod_interrupt_type_t intr_type,
                                        uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    tscomod_intr_status_t intr_status;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Get and clear interrupt status. */
    intr_status.type = intr_type;
    PHYMOD_IF_ERR_RETURN
        (tscomod_intr_status_get(&phy_copy.access, &intr_status));
    PHYMOD_IF_ERR_RETURN(tscomod_interrupt_enable_set(&phy_copy.access,
                                                      intr_type, enable));
    return PHYMOD_E_NONE;
}

int tscj_phy_interrupt_enable_get(const phymod_phy_access_t* phy,
                                        phymod_interrupt_type_t intr_type,
                                        uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(tscomod_interrupt_enable_get(&phy_copy.access,
                                                      intr_type, enable));
    return PHYMOD_E_NONE;
}
