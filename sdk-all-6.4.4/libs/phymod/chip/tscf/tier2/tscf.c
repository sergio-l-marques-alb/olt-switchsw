/*
 *         
 * $Id: tscf.c,v 1.2.2.26 Broadcom SDK $
 * 
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
 *     
 */

#ifndef _DV_TB_
 #define _SDK_TEFMOD_ 1 
#endif

#include <phymod/phymod.h>
#include <phymod/phymod_util.h>
#include <phymod/chip/bcmi_tscf_xgxs_defs.h>
#include <phymod/chip/falcon.h>

#include "../../tscf/tier1/tefmod_enum_defines.h"
#include "../../tscf/tier1/tefmod.h"
#include "../../tscf/tier1/tefmod_device.h"
#include "../../tscf/tier1/tefmod_sc_lkup_table.h"
#include "../../tscf/tier1/tfPCSRegEnums.h"
#include "../../falcon/tier1/falcon_cfg_seq.h"  
#include "../../falcon/tier1/falcon_tsc_common.h" 
#include "../../falcon/tier1/falcon_tsc_interface.h" 
#include "../../falcon/tier1/falcon_tsc_dependencies.h" 

#ifdef PHYMOD_TSCF_SUPPORT
#define TSCF_ID0                 0x600d
#define TSCF_ID1                 0x8770
#define TSCF_MODEL               0x14
#define FALCON_MODEL             0x1b
#define TSCF_NOF_LANES_IN_CORE   4
#define TSCF_LANE_SWAP_LANE_MASK 3
#define TSCF_NOF_DFES            5
#define TSCF_PHY_ALL_LANES       0xf

#define TSCF_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type = (_core_access)->type; \
        (_phy_access)->access.lane_mask = TSCF_PHY_ALL_LANES; \
    }while(0)

#define TSCF_MAX_FIRMWARES (5)

#define TSCF_PMD_CRC_UCODE 1

/* uController's firmware */
extern unsigned char tscf_ucode[];
extern unsigned short tscf_ucode_ver;
extern unsigned short tscf_ucode_crc;
extern unsigned short tscf_ucode_len;



typedef int (*sequncer_control_f)(const phymod_access_t* core, uint32_t enable);
typedef int (*rx_DFE_tap_control_set_f)(const phymod_access_t* phy, uint32_t val);

int _tscf_pll_multiplier_get(uint32_t pll_div, uint32_t *pll_multiplier)
{
    switch (pll_div) {
    case 0x0:
        *pll_multiplier = 64;
        break;
    case 0x1:
        *pll_multiplier = 66;
        break;
    case 0x2:
        *pll_multiplier = 80;
        break;
    case 0x3:
        *pll_multiplier = 128;
        break;
    case 0x4:
        *pll_multiplier = 132;
        break;
    case 0x5:
        *pll_multiplier = 140;
        break;
    case 0x6:
        *pll_multiplier = 160;
        break;
    case 0x7:
        *pll_multiplier = 165;
        break;
    case 0x8:
        *pll_multiplier = 168;
        break;
    case 0x9:
        *pll_multiplier = 170;
        break;
    case 0xa:
        *pll_multiplier = 175;
        break;
    case 0xb:
        *pll_multiplier = 180;
        break;
    case 0xc:
        *pll_multiplier = 184;
        break;
    case 0xd:
        *pll_multiplier = 200;
        break;
    case 0xe:
        *pll_multiplier = 224;
        break;
    case 0xf:
        *pll_multiplier = 264;
        break;
    default:
        *pll_multiplier = 165;
        break;
    }
    return PHYMOD_E_NONE;
}

int _tscf_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct falcon_tsc_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint32_t rst_status;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        serdes_firmware_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
        serdes_firmware_config.field.an_enabled        = fw_config.AnEnabled;
        serdes_firmware_config.field.dfe_on            = fw_config.DfeOn; 
        serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
        /* serdes_firmware_config.field.cl72_emulation_en = fw_config.Cl72Enable; */
        serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
        serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
        serdes_firmware_config.field.media_type        = fw_config.MediaType; 
        serdes_firmware_config.field.dfe_lp_mode       = fw_config.LpDfeOn;

        PHYMOD_IF_ERR_RETURN(falcon_lane_soft_reset_read(&phy_copy.access, &rst_status));
        if(rst_status) PHYMOD_IF_ERR_RETURN (falcon_lane_soft_reset_release(&phy_copy.access, 0));
        PHYMOD_IF_ERR_RETURN(falcon_tsc_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
        if(rst_status) PHYMOD_IF_ERR_RETURN (falcon_lane_soft_reset_release(&phy_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}


int tscf_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN0_SERDESIDr_t serdesid;
    /* DIG_REVID0r_t revid; */
    uint32_t model;

    *is_identified = 0;

    if(core_id == 0){
        ioerr += READ_PHYID2r(pm_acc, &id2);
        ioerr += READ_PHYID3r(pm_acc, &id3);
    }
    else{
        PHYID2r_SET(id2, ((core_id >> 16) & 0xffff));
        PHYID3r_SET(id3, core_id & 0xffff);
    }

    if (PHYID2r_REGID1f_GET(id2) == TSCF_ID0 &&
       (PHYID3r_REGID2f_GET(id3) == TSCF_ID1)) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN0_SERDESIDr(pm_acc, &serdesid);
        model = MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid);
        if (model == TSCF_MODEL)  {
            /* PCS model matches - now check PMD model */
            /* for now bypass the pmd register rev check */
            /*ioerr += READ_DIG_REVID0r(pm_acc, &revid);
            if (DIG_REVID0r_REVID_MODELf_GET(revid) == FALCON_MODEL) */ {
                *is_identified = 1;
            }
        }
    }
        
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;    

}


int tscf_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{
    uint32_t serdes_id;
    uint32_t rev_number;
    PHYMOD_IF_ERR_RETURN
        (tefmod_revid_read(&phy->access, &serdes_id));
    info->serdes_id = serdes_id;
    rev_number = (serdes_id & 0xc000) >> 14; 
    if ((serdes_id & 0x3f) == TSCF_MODEL) {
        /* for rev a*/
        if (rev_number == 0)
            info->core_version = phymodCoreVersionTscfA0;
        if (rev_number == 1)
            info->core_version = phymodCoreVersionTscfB0;
    }
    return PHYMOD_E_NONE;
}


/* 
 * set lane swapping for core 
 * The tx swap is composed of PCS swap and after that the PMD swap. 
 * The rx swap is composed just by PCS swap
 *
 * lane_map_tx and lane_map_rx[lane=logic_lane] are logic-lane base.
 * pmd_swap and register is logic_lane base.
 * but pmd_tx_map and addr_index_swap (and registers) are physical lane base
 */
 
int tscf_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    uint32_t pcs_swap = 0 , pmd_swap = 0, lane;
    uint32_t addr_index_swap = 0, pmd_tx_map =0;
 
    if(lane_map->num_of_lanes != TSCF_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }
    for( lane = 0 ; lane < TSCF_NOF_LANES_IN_CORE ; lane++){
        if(lane_map->lane_map_rx[lane] >= TSCF_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        /*encode each lane as four bits*/
        /*pcs_map[lane] = rx_map[lane]*/
        pcs_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }

    for( lane = 0 ; lane < TSCF_NOF_LANES_IN_CORE ; lane++){
        if(lane_map->lane_map_tx[lane] >= TSCF_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        /*encode each lane as four bits*/
        /*considering the pcs lane swap: pmd_map[pcs_map[lane]] = tx_map[lane]*/
        pmd_swap += lane_map->lane_map_tx[lane]<<(lane_map->lane_map_rx[lane]*4);
    }
    
    for( lane = 0 ; lane < TSCF_NOF_LANES_IN_CORE ; lane++){
        addr_index_swap |= (lane << 4*((pcs_swap >> lane*4) & 0xf)) ;
        pmd_tx_map      |= (lane << 4*((pmd_swap >> lane*4) & 0xf)) ;
    }

    PHYMOD_IF_ERR_RETURN(tefmod_pcs_lane_swap(&core->access, pcs_swap));
    
    PHYMOD_IF_ERR_RETURN
        (tefmod_pmd_addr_lane_swap(&core->access, addr_index_swap));
    PHYMOD_IF_ERR_RETURN
        (tefmod_pmd_lane_swap_tx(&core->access,
                                pmd_tx_map));
    return PHYMOD_E_NONE;
}


int tscf_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    uint32_t pmd_swap = 0 , pcs_swap = 0, lane; 
    PHYMOD_IF_ERR_RETURN(tefmod_pcs_lane_swap_get(&core->access, &pcs_swap));
    PHYMOD_IF_ERR_RETURN(tefmod_pmd_lane_swap_tx_get(&core->access, &pmd_swap));
    for( lane = 0 ; lane < TSCF_NOF_LANES_IN_CORE ; lane++){
        /*deccode each lane from four bits*/
        lane_map->lane_map_rx[lane] = (pcs_swap>>(lane*4)) & TSCF_LANE_SWAP_LANE_MASK;
        /*considering the pcs lane swap: tx_map[lane] = pmd_map[pcs_map[lane]]*/
        lane_map->lane_map_tx[lane] = (pmd_swap>>(lane_map->lane_map_rx[lane]*4)) & TSCF_LANE_SWAP_LANE_MASK;
    }
    lane_map->num_of_lanes = TSCF_NOF_LANES_IN_CORE;   
    return PHYMOD_E_NONE;
}


int tscf_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}


int tscf_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{       
    return PHYMOD_E_UNAVAIL;
}


int tscf_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    
    /* fw_info->fw_crc = tscf_ucode_crc;
    fw_info->fw_version = tscf_ucode_ver; */
    return PHYMOD_E_NONE;
}

 
/* load tsce fw. the fw_loader parameter is valid just for external fw load*/
STATIC
int _tscf_core_firmware_load(const phymod_core_access_t* core, phymod_firmware_load_method_t load_method, phymod_firmware_loader_f fw_loader)
{
    switch(load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(falcon_tsc_ucode_mdio_load(&core->access, tscf_ucode, tscf_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader);
        PHYMOD_IF_ERR_RETURN(falcon_tsc_ucode_init(&core->access));
        PHYMOD_IF_ERR_RETURN
            (falcon_pram_firmware_enable(&core->access, 1, 0));
        PHYMOD_IF_ERR_RETURN(fw_loader(core, tscf_ucode_len, tscf_ucode)); 
        PHYMOD_IF_ERR_RETURN
            (falcon_pram_firmware_enable(&core->access, 0, 0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }
    if(load_method != phymodFirmwareLoadMethodNone){
        /* PHYMOD_IF_ERR_RETURN(tscf_core_firmware_info_get(core, &actual_fw));
        if((tscf_ucode_crc != actual_fw.fw_crc) || (tscf_ucode_ver != actual_fw.fw_version)){
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("fw load validation was failed")));
        } */
    }
    return PHYMOD_E_NONE;
}

int tscf_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    struct falcon_tsc_uc_core_config_st serdes_firmware_core_config;
    uint32_t rst_status;
    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_config.VcoRate; 
    PHYMOD_IF_ERR_RETURN(falcon_core_soft_reset_read(&phy->access, &rst_status));
    if(rst_status) PHYMOD_IF_ERR_RETURN (falcon_core_soft_reset_release(&phy->access, 0));
    PHYMOD_IF_ERR_RETURN(falcon_tsc_set_uc_core_config(&phy->access, serdes_firmware_core_config));
    if(rst_status) PHYMOD_IF_ERR_RETURN (falcon_core_soft_reset_release(&phy->access, 1));
    return PHYMOD_E_NONE;
}


int tscf_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    struct falcon_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_IF_ERR_RETURN(falcon_tsc_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    return PHYMOD_E_NONE; 
}


int tscf_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (falcon_lane_soft_reset_release(&phy_copy.access, 0));
    }

    PHYMOD_IF_ERR_RETURN
         (_tscf_phy_firmware_lane_config_set(phy, fw_config));
    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (falcon_lane_soft_reset_release(&phy_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}


int tscf_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct falcon_tsc_uc_lane_config_st serdes_firmware_config;
    PHYMOD_IF_ERR_RETURN(falcon_tsc_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->LaneConfigFromPCS = serdes_firmware_config.field.lane_cfg_from_pcs;
    fw_config->AnEnabled         = serdes_firmware_config.field.an_enabled;
    fw_config->DfeOn             = serdes_firmware_config.field.dfe_on;
    fw_config->LpDfeOn           = serdes_firmware_config.field.dfe_lp_mode;
    fw_config->ForceBrDfe        = serdes_firmware_config.field.force_brdfe_on;
    /* fw_config->Cl72Enable        = serdes_firmware_config.field.cl72_emulation_en; */
    fw_config->ScramblingDisable = serdes_firmware_config.field.scrambling_dis;
    fw_config->UnreliableLos     = serdes_firmware_config.field.unreliable_los;
    fw_config->MediaType         = serdes_firmware_config.field.media_type;
     
    return PHYMOD_E_NONE; 
}

/* reset pll sequencer
 * flags - unused parameter
 */
int tscf_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return PHYMOD_E_UNAVAIL;
}

int tscf_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
    switch(event){
    case phymodCoreEventPllLock: 
        /* PHYMOD_IF_ERR_RETURN(tefmod_pll_lock_wait(&core->access, timeout)); */
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal wait event %u"), event));
    }
    return PHYMOD_E_NONE;
}

int tscf_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* pll_multiplier)
{
    phymod_phy_access_t pm_phy_copy;
    uint32 pll_mode = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (falcon_pll_mode_get(&pm_phy_copy.access, &pll_mode));
    _tscf_pll_multiplier_get(pll_mode, pll_multiplier);
     
    return PHYMOD_E_NONE;
}

/* reset rx sequencer 
 * flags - unused parameter
 */
int tscf_phy_rx_restart(const phymod_phy_access_t* phy)
{
    PHYMOD_IF_ERR_RETURN(falcon_tsc_rx_restart(&phy->access, 1)); 
    return PHYMOD_E_NONE;
}


int tscf_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    PHYMOD_IF_ERR_RETURN
        (tefmod_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity));
    return PHYMOD_E_NONE;
}


int tscf_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
   PHYMOD_IF_ERR_RETURN
        (tefmod_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity));
    return PHYMOD_E_NONE;
}


int tscf_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_write_tx_afe(&phy->access, TX_AFE_PRE, tx->pre));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_write_tx_afe(&phy->access, TX_AFE_MAIN, tx->main));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_write_tx_afe(&phy->access, TX_AFE_POST1, tx->post));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_write_tx_afe(&phy->access, TX_AFE_POST2, tx->post2));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_write_tx_afe(&phy->access, TX_AFE_POST3, tx->post3));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_write_tx_afe(&phy->access, TX_AFE_AMP,  tx->amp));
    
    return PHYMOD_E_NONE;
}

int tscf_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_tx_afe(&phy->access, TX_AFE_PRE, &tx->pre));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_tx_afe(&phy->access, TX_AFE_MAIN, &tx->main));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_tx_afe(&phy->access, TX_AFE_POST1, &tx->post));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_tx_afe(&phy->access, TX_AFE_POST2, &tx->post2));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_tx_afe(&phy->access, TX_AFE_POST3, &tx->post3));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_tx_afe(&phy->access, TX_AFE_AMP, &tx->amp));
    return PHYMOD_E_NONE;
}

int tscf_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_tx_pi_freq_override(&phy->access,
                                    tx_override->phase_interpolator.enable,
                                    tx_override->phase_interpolator.value));    
    return PHYMOD_E_NONE;
}

int tscf_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
/*
    PHYMOD_IF_ERR_RETURN
        (tefmod_tx_pi_control_get(&phy->access, &tx_override->phase_interpolator.value));
*/
    return PHYMOD_E_NONE;
}


int tscf_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    uint32_t i;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, k;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*params check*/
    if((rx->num_of_dfe_taps == 0) || (rx->num_of_dfe_taps < TSCF_NOF_DFES)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set %u"), rx->num_of_dfe_taps));
    }

    for (k = 0; k < num_lane; k++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + k);

        /*vga set*/
        if (rx->vga.enable) {
            /* first stop the rx adaption */
            PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&pm_phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(falcon_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_VGA, rx->vga.value));
        } else {
            PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&pm_phy_copy.access, 0));
        }
    
        /*dfe set*/
        for (i = 0 ; i < rx->num_of_dfe_taps ; i++){
            if(rx->dfe[i].enable){
                    PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&pm_phy_copy.access, 1));
                switch (i) {
                    case 0:
                            PHYMOD_IF_ERR_RETURN(falcon_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE1, rx->dfe[i].value));
                        break;
                    case 1:
                            PHYMOD_IF_ERR_RETURN(falcon_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE2, rx->dfe[i].value));
                        break;
                    case 2:
                            PHYMOD_IF_ERR_RETURN(falcon_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE3, rx->dfe[i].value));
                        break;
                    case 3:
                            PHYMOD_IF_ERR_RETURN(falcon_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE4, rx->dfe[i].value));
                        break;
                    case 4:
                            PHYMOD_IF_ERR_RETURN(falcon_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE5, rx->dfe[i].value));
                        break;
                    default:
                        return PHYMOD_E_PARAM;
                }
            } else {
                PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&pm_phy_copy.access, 0));
            }
        }
         
        /*peaking filter set*/
        if(rx->peaking_filter.enable){
            /* first stop the rx adaption */
            PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&pm_phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(falcon_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_PF, rx->peaking_filter.value));
        } else {
            PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&pm_phy_copy.access, 0));
        }
    
        if(rx->low_freq_peaking_filter.enable){
            /* first stop the rx adaption */
            PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&pm_phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(falcon_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_PF2, (int8_t)rx->low_freq_peaking_filter.value));
        } else {
            PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&pm_phy_copy.access, 0));
        }
    }
    return PHYMOD_E_NONE;
}


int tscf_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    int8_t tmpData;
    uint32_t rx_adaptation;

    rx_adaptation = PHYMOD_RX_ADAPTATION_ON_GET(rx);
    PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&phy->access, 1)); 

    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_rx_afe(&phy->access, RX_AFE_VGA,  &tmpData));
    rx->vga.value = tmpData; 
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_rx_afe(&phy->access, RX_AFE_DFE1,  &tmpData));
    rx->dfe[0].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_rx_afe(&phy->access, RX_AFE_DFE2,  &tmpData));
    rx->dfe[1].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_rx_afe(&phy->access, RX_AFE_DFE3,  &tmpData));
    rx->dfe[2].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_rx_afe(&phy->access, RX_AFE_DFE4,  &tmpData));
    rx->dfe[3].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_rx_afe(&phy->access, RX_AFE_DFE5,  &tmpData));
    rx->dfe[4].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_rx_afe(&phy->access, RX_AFE_PF,  &tmpData));
    rx->peaking_filter.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_read_rx_afe(&phy->access, RX_AFE_PF2,  &tmpData));
    rx->low_freq_peaking_filter.value = tmpData;


    rx->num_of_dfe_taps = 5;
    rx->dfe[0].enable = 1;
    rx->dfe[1].enable = 1;
    rx->dfe[2].enable = 1;
    rx->dfe[3].enable = 1;
    rx->dfe[4].enable = 1;
    rx->vga.enable = 1;
    rx->low_freq_peaking_filter.enable = 1;
    rx->peaking_filter.enable = 1;
    if (rx_adaptation) {
        PHYMOD_IF_ERR_RETURN(falcon_tsc_stop_rx_adaptation(&phy->access, 0));
    }     
    return PHYMOD_E_NONE;
}


int tscf_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}


int tscf_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}


int tscf_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tefmod_port_enable_set(&pm_phy_copy.access, 0));
        }
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tefmod_port_enable_set(&pm_phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(tefmod_power_control(&phy->access, 0, 0));
        }
    }
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
            /*disable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(falcon_tsc_tx_disable(&phy->access, 1));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
            /*enable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(falcon_tsc_tx_disable(&phy->access, 0));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
            /* disable rx on the PMD side */
            PHYMOD_IF_ERR_RETURN(tefmod_rx_squelch_set(&phy->access, 1));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
            /*enable rx on the PMD side */
            /* disable rx on the PMD side */
            PHYMOD_IF_ERR_RETURN(tefmod_rx_squelch_set(&phy->access, 0));
    }
    return PHYMOD_E_NONE; 
}

int tscf_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    int enable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(tefmod_rx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(falcon_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    power->rx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    /* Commented the following line. Because if in PMD loopback mode, we squelch the
           xmit, and we should still see the correct port status */
    /* PHYMOD_IF_ERR_RETURN(temod_tx_squelch_get(&pm_phy_copy.access, &enable)); */
    power->tx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    return PHYMOD_E_NONE;
}

int tscf_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{   
    switch (tx_control) {
    case phymodTxTrafficDisable:
        PHYMOD_IF_ERR_RETURN(tefmod_tx_lane_control_set(&phy->access, TEFMOD_TX_LANE_TRAFFIC_DISABLE));
        break;
    case phymodTxTrafficEnable:
        PHYMOD_IF_ERR_RETURN(tefmod_tx_lane_control_set(&phy->access, TEFMOD_TX_LANE_TRAFFIC_ENABLE));
        break;
    case phymodTxReset:
        PHYMOD_IF_ERR_RETURN(tefmod_tx_lane_control_set(&phy->access, TEFMOD_TX_LANE_RESET));
        break;
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(tefmod_tx_squelch_set(&phy->access, 1));
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(tefmod_tx_squelch_set(&phy->access, 0));
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int tscf_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{   
    int enable, reset, tx_lane;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(tefmod_tx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(falcon_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    if(enable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(tefmod_tx_lane_control_get(&pm_phy_copy.access, &reset, &tx_lane));
        if (!reset) {
            *tx_control = phymodTxReset;
        } else if (!tx_lane) {
            *tx_control = phymodTxTrafficDisable;
        } else {
            *tx_control = phymodTxTrafficEnable;
        }
    }
    return PHYMOD_E_NONE;
}


int tscf_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{       
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    switch (rx_control) {
    case phymodRxReset:
        PHYMOD_IF_ERR_RETURN(tefmod_rx_lane_control_set(&phy->access, 1));
        break;
    case phymodRxSquelchOn:
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tefmod_rx_squelch_set(&pm_phy_copy.access, 1));
        }
        break;
    case phymodRxSquelchOff:
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tefmod_rx_squelch_set(&pm_phy_copy.access, 0));
        }
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int tscf_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{   
    int enable, reset;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(tefmod_rx_squelch_get(&pm_phy_copy.access, &enable));
    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(falcon_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }
    if(enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(tefmod_rx_lane_control_get(&pm_phy_copy.access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }
    return PHYMOD_E_NONE;
}

int tscf_phy_tx_disable_set(const phymod_phy_access_t* phy, uint32_t tx_disable)
{       
    return PHYMOD_E_NONE;
}


int tscf_phy_tx_disable_get(const phymod_phy_access_t* phy, uint32_t* tx_disable)
{
    return PHYMOD_E_NONE;
}

/*
 * enables/disables FEC function.
 * enable[6:0] = Enable for fec
 *               0x1    Enable FEC (speed 100G CL91 or CL74)
 * 
 */
int tscf_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    int fec_en;
    int fec_dis;
    int cl74or91;
    int speed_id;

    fec_en = 0;
    fec_dis = 0;
    cl74or91 = 0;

    cl74or91 = (enable >> 16) & 0x2;

    PHYMOD_IF_ERR_RETURN(
      tefmod_speed_id_get(&phy->access, &speed_id));

    if ((speed_id > 70) || (speed_id < 64)) {
      if(enable & 1) {
        fec_en = 0x30;
        fec_dis = 0x0f;
      } else {
        fec_dis = 0x30;
      }
    } else {
      if(enable & 1) {
        fec_en = 3;
        fec_dis = 0x3c;
      } else {
        fec_dis = 3;
      }
    }

    PHYMOD_IF_ERR_RETURN(
      tefmod_FEC_control(&phy->access, fec_en, fec_dis, cl74or91));

    return PHYMOD_E_NONE;

}

int tscf_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    int fec_en;
    int cl74or91;

    PHYMOD_IF_ERR_RETURN(
      tefmod_FEC_get(&phy->access, &fec_en, &cl74or91));
    PHYMOD_DEBUG_VERBOSE(("FEC enable state :: %x :: cl74or91 :: %x\n", fec_en, cl74or91));
    /* *enable = fec_en | (cl74or91<<16); */
    *enable = fec_en;
    return PHYMOD_E_NONE;

}

STATIC
int _tscf_st_hto_interface_config_set(const tefmod_device_aux_modes_t *device, int lane_num, int speed_vec, uint32_t *new_pll_div, int16_t *os_mode)
{
    int i, max_lane=4 ;
    *os_mode = -1 ;
    /* ST */
    for(i=0;i<max_lane; i++) {
        if(device->st_hcd[i]==speed_vec){
            *new_pll_div = device->st_pll_div[i];
            *os_mode     = device->st_os[i] ;
            break ;
        }
    }
    /* HTO */
    if(device->hto_enable[lane_num]){
        *new_pll_div = device->hto_pll_div[lane_num];
        *os_mode     = device->hto_os[lane_num] ;
    }
    return PHYMOD_E_NONE;
} 


int tscf_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    phymod_tx_t tx_params;  
    uint32_t current_pll_div=0;
    uint32_t new_pll_div=0;
    uint16_t new_speed_vec=0;
    int16_t  new_os_mode =-1;
    tefmod_spd_intfc_type_t spd_intf = TEFMOD_SPD_ILLEGAL;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i, pll_switch=0;
    int lane_bkup; 

    /* phymod_access_t tmp_phy_access; */
    uint32_t pll_multiplier, vco_rate;
    
    /* sc_table_entry exp_entry; RAVI */
    phymod_firmware_lane_config_t firmware_lane_config;  
    phymod_firmware_core_config_t firmware_core_config;

    firmware_lane_config.MediaType = 0;

    /*next program the tx fir taps and driver current based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (falcon_lane_soft_reset_release(&pm_phy_copy.access, 0));
    }

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
     PHYMOD_IF_ERR_RETURN
        (tscf_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config)); 

    /*make sure that an and config from pcs is off*/
    firmware_core_config.CoreConfigFromPCS = 0;
    firmware_lane_config.AnEnabled = 0;
    firmware_lane_config.LaneConfigFromPCS = 0;
    firmware_lane_config.DfeOn = 1; 

    if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    } else if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
    } else {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
    }     
    if(config->data_rate == 1000)
       firmware_lane_config.DfeOn = 0;

    PHYMOD_IF_ERR_RETURN
        (tefmod_update_port_mode(&phy->access, (int *) &pll_switch));
    

    if (config->interface_type == phymodInterface1000X) {
       spd_intf = TEFMOD_SPD_1G_25G;
    } else if (config->interface_type == phymodInterfaceSGMII) {
       spd_intf = TEFMOD_SPD_1000_SGMII;
    } else if ((config->interface_type == phymodInterfaceXFI) ||
               (config->interface_type == phymodInterfaceKR) ||
               (config->interface_type == phymodInterfaceCR) ||
               (config->interface_type == phymodInterfaceSFI)) {
        switch (config->data_rate) {
         case 10000:
           spd_intf = TEFMOD_SPD_10000_XFI;
           if (!PHYMOD_INTF_MODES_COPPER_GET(config)) {
               firmware_lane_config.DfeOn = 0;
           }
           break;
         case 10600:
         case 11000:
           spd_intf = TEFMOD_SPD_10600_XFI_HG;
           if (!PHYMOD_INTF_MODES_COPPER_GET(config)) {
               firmware_lane_config.DfeOn = 0;
           }
           break;
         case 20000:
           spd_intf = TEFMOD_SPD_20000_XFI;
           break;
         case 21200:
           spd_intf = TEFMOD_SPD_21200_XFI_HG;
           break;
         case 25000:
           spd_intf = TEFMOD_SPD_25000_XFI;
           break;
         case 26500:
         case 27000:
           spd_intf = TEFMOD_SPD_26500_XFI_HG;
           break;
         default:
           spd_intf = TEFMOD_SPD_10000_XFI;
           firmware_lane_config.DfeOn = 0;
           break;
        }
    } else if ((config->interface_type == phymodInterfaceKR2) ||
               (config->interface_type == phymodInterfaceCR2) ||
               (config->interface_type == phymodInterfaceXLAUI2) ||
               (config->interface_type == phymodInterfaceRXAUI) ||
               (config->interface_type == phymodInterfaceX2))  {
       switch (config->data_rate) {
         case 20000:
           spd_intf = TEFMOD_SPD_20G_MLD_X2;
           if (!PHYMOD_INTF_MODES_COPPER_GET(config)) {
               firmware_lane_config.DfeOn = 0;
           }
           break;
         case 21000:
         case 21200:
           if (!PHYMOD_INTF_MODES_COPPER_GET(config)) {
               firmware_lane_config.DfeOn = 0;
           }
           spd_intf = TEFMOD_SPD_21G_MLD_HG_X2;
           break;
         case 40000:
           spd_intf = TEFMOD_SPD_40G_MLD_X2;
           break;
         case 42000:
           spd_intf = TEFMOD_SPD_42G_MLD_HG_X2;
           break;
         case 50000:
           spd_intf = TEFMOD_SPD_50G_MLD_X2;
           break;
         case 53000:
           spd_intf = TEFMOD_SPD_53G_MLD_HG_X2;
           break;
         default:
           spd_intf = TEFMOD_SPD_20G_MLD_X2;
           break;
       }
    } else { 
       switch (config->data_rate) {
         case 40000:
           spd_intf = TEFMOD_SPD_40G_MLD_X4;
           if (!PHYMOD_INTF_MODES_COPPER_GET(config)) {
               firmware_lane_config.DfeOn = 0;
           }
           break;
         case 42000:
           spd_intf = TEFMOD_SPD_42G_MLD_HG_X4;
           if (!PHYMOD_INTF_MODES_COPPER_GET(config)) {
               firmware_lane_config.DfeOn = 0;
           }
           break;
         case 50000:
           spd_intf = TEFMOD_SPD_50G_MLD_X4;
           break;
         case 53000:
           spd_intf = TEFMOD_SPD_53G_MLD_HG_X4;
           break;
         case 100000:
           spd_intf = TEFMOD_SPD_100G_MLD_X4;
           break;
         case 106000:
           spd_intf = TEFMOD_SPD_106G_MLD_HG_X4;
           break;
         default:
           spd_intf = TEFMOD_SPD_40G_MLD_X4;
           firmware_lane_config.DfeOn = 0;
           break;
       }
    }
    
    PHYMOD_IF_ERR_RETURN
        (tefmod_get_plldiv(&phy->access, &current_pll_div));
   
    if(config->interface_type == phymodInterfaceBypass) {

      PHYMOD_IF_ERR_RETURN
        (falcon_tsc_get_vco(config, &vco_rate, &new_pll_div, &new_os_mode));
    } else {

        PHYMOD_IF_ERR_RETURN
            (tefmod_plldiv_lkup_get(&phy->access, spd_intf, &new_pll_div));
    
        if(config->device_aux_modes !=NULL){    
            PHYMOD_IF_ERR_RETURN
                (_tscf_st_hto_interface_config_set(config->device_aux_modes, start_lane, new_speed_vec, &new_pll_div, &new_os_mode)) ;
        }
    }

    tefmod_disable_set(&phy->access);

    if(new_os_mode>=0) {
        new_os_mode |= 0x80000000 ;   
    } else {
        new_os_mode = 0 ;
    }
    
    PHYMOD_IF_ERR_RETURN
        (tefmod_pmd_osmode_set(&phy->access, spd_intf, new_os_mode));

    /*if pll change is enabled*/
    if((current_pll_div != new_pll_div) && (PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, 
                               (_PHYMOD_MSG("pll has to change for speed_set from %u to %u but DONT_TURN_OFF_PLL flag is enabled"),
                                 current_pll_div, new_pll_div));
    }

    /*pll switch is required and expected */
    if((current_pll_div != new_pll_div) && !(PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)) {

        /* Change in PLL, so reset all the ports first. */
        lane_bkup = pm_phy_copy.access.lane_mask;
        pm_phy_copy.access.lane_mask = 0xf;
        tefmod_disable_set(&pm_phy_copy.access); 
        pm_phy_copy.access.lane_mask = lane_bkup;
        
        PHYMOD_IF_ERR_RETURN
            (falcon_core_soft_reset_release(&pm_phy_copy.access, 0));

        /*release the uc reset */
        PHYMOD_IF_ERR_RETURN
            (falcon_tsc_uc_reset(&pm_phy_copy.access ,0)); 

        /*set the PLL divider */
        PHYMOD_IF_ERR_RETURN
            (falcon_pll_mode_set(&phy->access, new_pll_div));
        PHYMOD_IF_ERR_RETURN
            (_tscf_pll_multiplier_get(new_pll_div, &pll_multiplier));
        /* update the VCO rate properly */
        switch (config->ref_clock) {
            case phymodRefClk156Mhz:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
            case phymodRefClk125Mhz:
                vco_rate = pll_multiplier * 125;
                break;
            default:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
        }
        firmware_core_config.VcoRate = (vco_rate * 16 + 500) / 1000 - 224;                 

        /*change the  master port num to the current caller port */
        
        PHYMOD_IF_ERR_RETURN
            (tefmod_master_port_num_set(&phy->access, start_lane));
        PHYMOD_IF_ERR_RETURN
            (tefmod_pll_reset_enable_set(&phy->access, 1));
        /*update the firmware config properly*/
        PHYMOD_IF_ERR_RETURN
            (tscf_phy_firmware_core_config_set(&pm_phy_copy, firmware_core_config));
        PHYMOD_IF_ERR_RETURN
            (falcon_core_soft_reset_release(&pm_phy_copy.access, 1));
    }

    if(config->interface_type == phymodInterfaceBypass) {
      PHYMOD_IF_ERR_RETURN
          (tefmod_init_pcs_ilkn(&phy->access));    
    } else {
      PHYMOD_IF_ERR_RETURN
          (tefmod_set_spd_intf(&phy->access, spd_intf));    
    }

    /*change TX parameters if enabled*/
    /*if((PHYMOD_IF_F_DONT_OVERIDE_TX_PARAMS & flags) == 0) */ {
        PHYMOD_IF_ERR_RETURN
            (falcon_phy_media_type_tx_get(phy, phymodMediaTypeMid, &tx_params));
#ifdef FIXME 
/*
 * We are calling this function with all params=0
*/
        PHYMOD_IF_ERR_RETURN
            (tscf_phy_tx_set(phy, &tx_params));
#endif
    }
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (_tscf_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }
    
    /*release the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (falcon_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }
              
    return PHYMOD_E_NONE;
}

/* Yet to fix the interface_type */
int _tscf_speed_id_interface_config_get(const phymod_phy_access_t* phy, int speed_id, phymod_phy_inf_config_t* config)
{
    int ilkn_set;
    int osr_mode;
    int div_osr_value;
    uint32_t vco_rate;
    uint32_t pll_multiplier;
    uint32_t plldiv_r_val;
    PHYMOD_IF_ERR_RETURN(tefmod_pcs_ilkn_chk(&phy->access, &ilkn_set));
    if(ilkn_set) {
        config->interface_type = phymodInterfaceBypass;
        PHYMOD_IF_ERR_RETURN(tefmod_get_plldiv(&phy->access, &plldiv_r_val));
        PHYMOD_IF_ERR_RETURN
            (_tscf_pll_multiplier_get(plldiv_r_val, &pll_multiplier));
        PHYMOD_IF_ERR_RETURN
            (falcon_osr_mode_get(&phy->access, &osr_mode));

        switch (config->ref_clock) {
            case phymodRefClk156Mhz:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
            case phymodRefClk125Mhz:
                vco_rate = pll_multiplier * 125;
                break;
            default:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
        }
        div_osr_value = 1 << osr_mode;
        config->data_rate = vco_rate/div_osr_value;
    } else {
        switch (speed_id) {
        case 0x0:
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x1:
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x2:
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x4:
            config->data_rate = 11000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x5:
            config->data_rate = 11000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x6:
            config->data_rate = 11000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x8:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x9:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0xa:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0xc:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0xd:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0xe:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x10:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x11:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x12:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x14:
            config->data_rate = 27000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x15:
            config->data_rate = 27000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x16:
            config->data_rate = 27000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x18:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x19:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x1a:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x1c:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x1d:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x1e:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x20:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x21:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x22:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x24:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x25:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x26:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x28:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x29:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x2a:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x2c:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x2d:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x2e:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x30:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x31:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x32:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x34:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x35:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x36:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x38:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x39:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x3a:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x3c:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x3d:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x3e:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x40:
            config->data_rate = 100000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x41:
            config->data_rate = 100000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x42:
            config->data_rate = 100000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x44:
            config->data_rate = 106000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x45:
            config->data_rate = 106000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x46:
            config->data_rate = 106000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x48:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x50:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x58:
            config->data_rate = 1000;
            config->interface_type = phymodInterfaceCX2;
            break;
        case 0x60:
            config->data_rate = 1000;
            config->interface_type = phymodInterfaceCX2;
            break;
        default:
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceKR;
            break;
        }
    }
    return PHYMOD_E_NONE;
}


/*flags- unused parameter*/
int tscf_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    int speed_id;
    phymod_firmware_lane_config_t firmware_lane_config;  
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    config->ref_clock = ref_clock;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (tefmod_speed_id_get(&phy->access, &speed_id));
    PHYMOD_IF_ERR_RETURN
        (_tscf_speed_id_interface_config_get(phy, speed_id, config));
    /* read the current media type */     
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tscf_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));
    if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeOptics) {
        PHYMOD_INTF_MODES_FIBER_SET(config);
    } else if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
        PHYMOD_INTF_MODES_COPPER_SET(config);
    } else {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
        PHYMOD_INTF_MODES_BACKPLANE_SET(config);
    }             
    return PHYMOD_E_NONE;
}


int tscf_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct falcon_tsc_uc_lane_config_st serdes_firmware_config;
    PHYMOD_IF_ERR_RETURN(falcon_tsc_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));

    if(serdes_firmware_config.field.dfe_on == 0) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72/CL93 with no DFE\n"));  
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (tefmod_clause72_control(&phy->access, cl72_en));
    return PHYMOD_E_NONE;
}

int tscf_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
/*    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_display_cl93n72_status(&phy->access)); */
    PHYMOD_IF_ERR_RETURN
        (falcon_clause72_control_get(&phy->access, cl72_en));
    return PHYMOD_E_NONE;
}


int tscf_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    uint32_t local_status;
/*    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_display_cl93n72_status(&phy->access)); */
    PHYMOD_IF_ERR_RETURN
        (falcon_pmd_cl72_receiver_status(&phy->access, &local_status));
    status->locked = local_status;
    return PHYMOD_E_NONE;
}

int tscf_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    tefmod_an_adv_ability_t value;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    phymod_core_info_t core_info;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
    PHYMOD_MEMSET(&core_info, 0x0, sizeof(core_info));

    PHYMOD_IF_ERR_RETURN
        (tscf_core_info_get((phymod_core_access_t *)phy, &core_info));

    value.an_cl72 = an_ability->an_cl72;
    if (an_ability->an_fec) {
        value.an_fec = TEFMOD_FEC_SUPRTD_REQSTD;
    } else {
        value.an_fec = TEFMOD_FEC_SUPRTD_NOT_REQSTD;
    }        
    /* value.an_fec = an_ability->an_fec; */
    value.an_hg2 = an_ability->an_hg2;

    /* tscf a0 does not support cl91 */
    /* next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.an_pause = TEFMOD_SYMM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.an_pause = TEFMOD_ASYM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.an_pause = TEFMOD_ASYM_SYMM_PAUSE;
    }

    /* check cl73 and cl73 bam ability */
    if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap)) 
        value.an_base_speed |= 1 << TEFMOD_CL73_1GBASE_KX;
    if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap)) 
        value.an_base_speed |= 1 << TEFMOD_CL73_10GBASE_KR1;
    if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) 
        value.an_base_speed |= 1 << TEFMOD_CL73_40GBASE_KR4;
    if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) 
        value.an_base_speed |= 1 << TEFMOD_CL73_40GBASE_CR4;
    if (PHYMOD_AN_CAP_100G_KR4_GET(an_ability->an_cap)) {
        if (core_info.core_version == phymodCoreVersionTscfA0) {
            value.an_fec = TEFMOD_FEC_NOT_SUPRTD;
        }
        value.an_base_speed |= 1 << TEFMOD_CL73_100GBASE_KR4;
    }
    if (PHYMOD_AN_CAP_100G_CR4_GET(an_ability->an_cap)) {
        if (core_info.core_version == phymodCoreVersionTscfA0) {
            value.an_fec = TEFMOD_FEC_NOT_SUPRTD;
        }
        value.an_base_speed |= 1 << TEFMOD_CL73_100GBASE_CR4;
    }
    /* check cl73 bam ability */
    if (PHYMOD_BAM_CL73_CAP_20G_KR2_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_20GBASE_KR2;
    if (PHYMOD_BAM_CL73_CAP_20G_KR2_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_20GBASE_CR2;
    if (PHYMOD_BAM_CL73_CAP_40G_KR2_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_40GBASE_KR2;
    if (PHYMOD_BAM_CL73_CAP_40G_CR2_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_40GBASE_CR2;
    if (PHYMOD_BAM_CL73_CAP_50G_KR2_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_50GBASE_KR2;
    if (PHYMOD_BAM_CL73_CAP_50G_CR2_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_50GBASE_CR2;
    if (PHYMOD_BAM_CL73_CAP_50G_KR4_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_50GBASE_KR4;
    if (PHYMOD_BAM_CL73_CAP_50G_CR4_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_50GBASE_CR4;

    if (PHYMOD_BAM_CL73_CAP_20G_KR1_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed1 |= 1 << TEFMOD_CL73_BAM_20GBASE_KR1;
    if (PHYMOD_BAM_CL73_CAP_20G_CR1_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed1 |= 1 << TEFMOD_CL73_BAM_20GBASE_CR1;
    if (PHYMOD_BAM_CL73_CAP_25G_KR1_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed1 |= 1 << TEFMOD_CL73_BAM_25GBASE_KR1;
    if (PHYMOD_BAM_CL73_CAP_25G_CR1_GET(an_ability->cl73bam_cap)) 
        value.an_bam_speed1 |= 1 << TEFMOD_CL73_BAM_25GBASE_CR1;

    PHYMOD_IF_ERR_RETURN
        (tefmod_autoneg_set(&phy_copy.access, &value));
    return PHYMOD_E_NONE;
    
}

int tscf_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type){
    tefmod_an_adv_ability_t value;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
     
    PHYMOD_IF_ERR_RETURN
        (tefmod_autoneg_local_ability_get(&phy_copy.access, &value));
    an_ability_get_type->an_cl72 = value.an_cl72;
    an_ability_get_type->an_fec = value.an_fec;

    if (value.an_pause == TEFMOD_ASYM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
    } else if (value.an_pause == TEFMOD_SYMM_PAUSE) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    } else if (value.an_pause == TEFMOD_ASYM_SYMM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    }

    /* first check cl73 base  ability */
    if (value.an_base_speed &  1 << TEFMOD_CL73_100GBASE_CR4) 
        PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
    if (value.an_base_speed & 1 << TEFMOD_CL73_100GBASE_KR4) 
        PHYMOD_AN_CAP_100G_KR4_SET(an_ability_get_type->an_cap);
    if (value.an_base_speed & 1 << TEFMOD_CL73_40GBASE_CR4) 
        PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
    if (value.an_base_speed & 1 << TEFMOD_CL73_40GBASE_KR4) 
        PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
    if (value.an_base_speed & 1 << TEFMOD_CL73_10GBASE_KR1) 
        PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
    if (value.an_base_speed & 1 << TEFMOD_CL73_1GBASE_KX) 
        PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);

    /* next check cl73 bam ability */
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_20GBASE_KR2) 
        PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_20GBASE_CR2) 
        PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_40GBASE_KR2) 
        PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_40GBASE_CR2) 
        PHYMOD_BAM_CL73_CAP_40G_CR2_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_50GBASE_KR2) 
        PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_50GBASE_CR2) 
        PHYMOD_BAM_CL73_CAP_50G_CR2_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_50GBASE_KR4) 
        PHYMOD_BAM_CL73_CAP_50G_KR4_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_50GBASE_CR4) 
        PHYMOD_BAM_CL73_CAP_50G_CR4_SET(an_ability_get_type->cl73bam_cap);

    if (value.an_bam_speed1 & 1 << TEFMOD_CL73_BAM_20GBASE_KR1) 
        PHYMOD_BAM_CL73_CAP_20G_KR1_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed1 & 1 << TEFMOD_CL73_BAM_20GBASE_CR1) 
        PHYMOD_BAM_CL73_CAP_20G_CR1_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed1 & 1 << TEFMOD_CL73_BAM_25GBASE_KR1) 
        PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_ability_get_type->cl73bam_cap);
    if (value.an_bam_speed1 & 1 << TEFMOD_CL73_BAM_25GBASE_CR1) 
        PHYMOD_BAM_CL73_CAP_25G_CR1_SET(an_ability_get_type->cl73bam_cap);

    return PHYMOD_E_NONE;
}

int tscf_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    /* Place your code here */
    return PHYMOD_E_NONE;
}

int tscf_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded;
    int start_lane, num_lane;
    int i;
    int do_config_set;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_firmware_core_config_t firmware_core_config_tmp;
    tefmod_an_control_t an_control;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    
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
        case 10:
            num_lane_adv_encoded = 3;
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    an_control.an_property_type = 0x0;   /* for now disable */  
    switch (an->an_mode) {
    case phymod_AN_MODE_CL73:
        an_control.an_type = TEFMOD_AN_MODE_CL73;
        break;
    case phymod_AN_MODE_CL73BAM:
        an_control.an_type = TEFMOD_AN_MODE_CL73BAM;
        break;
    case phymod_AN_MODE_HPAM:
        an_control.an_type = TEFMOD_AN_MODE_HPAM;
        break;
    default:
        return PHYMOD_E_PARAM;
    }

    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (tscf_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));
    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (tscf_phy_firmware_core_config_get(&phy_copy, &firmware_core_config_tmp));
    do_config_set = 0;
    if (an->enable) {
        if(firmware_lane_config.AnEnabled != 1) {
          firmware_lane_config.AnEnabled = 1;
          do_config_set = 1;
        }
        if(firmware_lane_config.LaneConfigFromPCS != 1) {
          firmware_lane_config.LaneConfigFromPCS = 1;
          do_config_set = 1;
        }
        if(firmware_core_config_tmp.CoreConfigFromPCS != 1) {
          firmware_core_config_tmp.CoreConfigFromPCS = 1;
          do_config_set = 1;
        }
    } else {
        if(firmware_lane_config.AnEnabled != 0) {
          firmware_lane_config.AnEnabled = 0;
          do_config_set = 1;
        }
        if(firmware_lane_config.LaneConfigFromPCS != 0) {
          firmware_lane_config.LaneConfigFromPCS = 0;
          do_config_set = 1;
        }
        if(firmware_core_config_tmp.CoreConfigFromPCS != 0) {
          firmware_core_config_tmp.CoreConfigFromPCS = 0;
          do_config_set = 1;
        }
    }

    if(do_config_set) {
        PHYMOD_IF_ERR_RETURN
            (falcon_core_soft_reset_release(&phy_copy.access, 0));
        PHYMOD_IF_ERR_RETURN
            (tscf_phy_firmware_core_config_set(&phy_copy, firmware_core_config_tmp));
        PHYMOD_IF_ERR_RETURN
            (falcon_core_soft_reset_release(&phy_copy.access, 1));

        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (falcon_lane_soft_reset_release(&phy_copy.access, 0));
        }

        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (_tscf_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
        }

        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (falcon_lane_soft_reset_release(&phy_copy.access, 1));
        }
    }
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tefmod_master_port_num_set(&phy_copy.access, start_lane));

    if (an->enable) {
      PHYMOD_IF_ERR_RETURN
          (tefmod_set_an_port_mode(&phy->access, num_lane_adv_encoded, start_lane, 1));
    } else {
      PHYMOD_IF_ERR_RETURN
          (tefmod_set_an_port_mode(&phy->access, num_lane_adv_encoded, start_lane, 0));
    }
    PHYMOD_IF_ERR_RETURN
        (tefmod_autoneg_control(&phy_copy.access, &an_control));       
    return PHYMOD_E_NONE;
}

int tscf_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    tefmod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane; 
    int an_complete = 0;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(tefmod_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (tefmod_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));
    
    if (an_control.enable) {
        an->enable = 1;
        *an_done = an_complete; 
    } else {
        an->enable = 0;
    }                
    return PHYMOD_E_NONE;
}


int tscf_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}

int _tscf_core_init_pass1(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    int rv;
    uint32 uc_enable = 0;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;

    TSCF_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    rv =  falcon_uc_active_get(&phy_access.access, &uc_enable);
    if (uc_enable){
        return PHYMOD_E_NONE;
    }    

    PHYMOD_IF_ERR_RETURN
        (tefmod_pmd_reset_seq(&core_copy.access, core_status->pmd_active));
    /*need to set the heart beat default is for 156.25M*/
    if (init_config->interface.ref_clock == phymodRefClk125Mhz) {
        /*we need to set the ref clock config differently */
    }

    rv = _tscf_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader);
    if (rv != PHYMOD_E_NONE) {
        PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC firmware-load failed\n", core->access.addr, core->access.lane_mask));  
        PHYMOD_IF_ERR_RETURN(rv);
    }
    return PHYMOD_E_NONE;
}


int _tscf_core_init_pass2(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;

    TSCF_CORE_TO_PHY_ACCESS(&phy_access, core);
    phy_access_copy = phy_access;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
        phy_access_copy = phy_access;
        phy_access_copy.access = core->access;
        phy_access_copy.access.lane_mask = 0x1;
        phy_access_copy.type = core->type;

    /*next we need to check if the load is correct or not */
#ifndef TSCF_PMD_CRC_UCODE 
    if(init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        rv = falcon_tsc_ucode_load_verify(&core_copy.access, (uint8_t *) &tscf_ucode, tscf_ucode_len);
        if(rv != PHYMOD_E_NONE) {
            PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));  
            PHYMOD_IF_ERR_RETURN(rv);
        }
    }
#endif

    PHYMOD_IF_ERR_RETURN
        (falcon_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x1));

    /*next we need to set the uc active and release uc */
    PHYMOD_IF_ERR_RETURN
        (falcon_uc_active_set(&core_copy.access ,1)); 
    /*release the uc reset */
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_uc_reset(&core_copy.access ,0)); 
    /* we need to wait at least 10ms for the uc to settle */
    /* PHYMOD_USLEEP(10000); */ 

    if(PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifndef TSCF_PMD_CRC_UCODE
        /* poll the ready bit in 10 ms */
        /* Coverity fix: checking the return value with PHYMOD_IF_ERR_RETURN */
        PHYMOD_IF_ERR_RETURN
            (falcon_tsc_poll_uc_dsc_ready_for_cmd_equals_1(&phy_access_copy.access, 100));
#else
        PHYMOD_IF_ERR_RETURN(
            falcon_tsc_ucode_crc_verify( &core_copy.access, tscf_ucode_len, tscf_ucode_crc));
#endif
    }
    PHYMOD_IF_ERR_RETURN(
        falcon_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x0));

    /* plldiv CONFIG */
    PHYMOD_IF_ERR_RETURN
        (falcon_pll_mode_set(&core_copy.access, 0xa));

    /*now config the lane mapping and polarity */
    PHYMOD_IF_ERR_RETURN
        (tscf_core_lane_map_set(core, &init_config->lane_map));
    PHYMOD_IF_ERR_RETURN
        (tefmod_autoneg_timer_init(&core->access));
    PHYMOD_IF_ERR_RETURN
        (tefmod_master_port_num_set(&core->access, 0));
    /*don't overide the fw that set in config set if not specified*/
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;
    /*set the vco rate to be default at 27.34G */
    firmware_core_config_tmp.VcoRate = 214;

    PHYMOD_IF_ERR_RETURN
        (tscf_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp)); 

    /* release core soft reset */
    PHYMOD_IF_ERR_RETURN
        (falcon_core_soft_reset_release(&core_copy.access, 1));
        
    return PHYMOD_E_NONE;
}

int tscf_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscf_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscf_core_init_pass2(core, init_config, core_status));
    }
        
    return PHYMOD_E_NONE;
}


int tscf_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    int pll_restart = 0;
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    int lane_bkup;
    phymod_polarity_t tmp_pol;
    phymod_firmware_lane_config_t firmware_lane_config;  

    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /*next program the tx fir taps and driver current based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
    /*per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (tefmod_pmd_x4_reset(pm_acc));

    lane_bkup = pm_phy_copy.access.lane_mask;
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (falcon_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }
    pm_phy_copy.access.lane_mask = lane_bkup;

    /* clearing all the lane config */
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    /* program the rx/tx polarity */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) >> i & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) >> i & 0x1;
        PHYMOD_IF_ERR_RETURN
            (tscf_phy_polarity_set(&pm_phy_copy, &tmp_pol));
    }

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (tscf_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
             (_tscf_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    PHYMOD_IF_ERR_RETURN
        (tefmod_update_port_mode(pm_acc, &pll_restart));

    PHYMOD_IF_ERR_RETURN
        (tefmod_rx_lane_control_set(pm_acc, 1));
    PHYMOD_IF_ERR_RETURN
        (tefmod_tx_lane_control_set(pm_acc, TEFMOD_TX_LANE_RESET_TRAFFIC_ENABLE));         /* TX_LANE_CONTROL */

    return PHYMOD_E_NONE;
}


int tscf_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int i;
    int start_lane, num_lane;
    int rv = PHYMOD_E_NONE;
    uint32_t cl72_en;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN
            (tscf_phy_cl72_get(phy, &cl72_en));
        if(cl72_en == 1) {
             PHYMOD_DEBUG_ERROR(("adr=%0x,lane 0x%x: Error! pcs gloop not supported with cl72 enabled\n",  phy_copy.access.addr, start_lane));
             break;
        }
        PHYMOD_IF_ERR_RETURN(tefmod_tx_loopback_control(&phy->access, enable, start_lane, num_lane));
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN
            (tscf_phy_cl72_get(phy, &cl72_en));
        if(cl72_en == 1) {
             PHYMOD_DEBUG_ERROR(("adr=%0x,lane 0x%x: Error! pmd gloop not supported with cl72 enabled\n",  phy_copy.access.addr, start_lane));
             break;
        }
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(falcon_tsc_dig_lpbk(&phy_copy.access, (uint8_t) enable));
            PHYMOD_IF_ERR_RETURN(falcon_pmd_force_signal_detect(&phy_copy.access, (int) enable));
            PHYMOD_IF_ERR_RETURN(tefmod_rx_lane_control_set(&phy->access, 1));
        }
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(falcon_tsc_rmt_lpbk(&phy->access, (uint8_t)enable));
        break;
    case phymodLoopbackRemotePCS :
        PHYMOD_IF_ERR_RETURN(tefmod_rx_loopback_control(&phy->access, enable)); /* RAVI */
        break;
    default :
        break;
    }             
    return rv;
}

int tscf_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    uint32_t enable_core;
    int start_lane, num_lane;

    *enable = 0;

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN(tefmod_tx_loopback_get(&phy->access, &enable_core));
        *enable = (enable_core >> start_lane) & 0x1; 
        break;
    case phymodLoopbackGlobalPMD :
        /* PHYMOD_IF_ERR_RETURN(falcon_tsc_dig_lpbk(&phy->access, (uint8_t) enable)); */
        break;
    case phymodLoopbackRemotePMD :
        /* PHYMOD_IF_ERR_RETURN(falcon_tsc_rmt_lpbk(&phy->access, (uint8_t)enable)); */
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(tefmod_rx_loopback_control(&phy->access, enable, enable, enable)); */ /* RAVI */
        break;
    default :
        break;
    }             
    return PHYMOD_E_NONE;
}


int tscf_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done){
    PHYMOD_IF_ERR_RETURN(tefmod_pmd_lock_get(&phy->access, rx_seq_done));
    return PHYMOD_E_NONE;
}


int tscf_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    PHYMOD_IF_ERR_RETURN(tefmod_get_pcs_link_status(&phy->access, link_status));
    return PHYMOD_E_NONE;
}

int tscf_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
    return PHYMOD_E_UNAVAIL;
}

int tscf_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
    return PHYMOD_E_UNAVAIL;
}

int tscf_phy_status_dump(const phymod_phy_access_t* phy)
{
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_display_core_state(&phy->access));
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_display_lane_debug_status(&phy->access));

    return PHYMOD_E_NONE;
}

int tscf_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t *val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}


int tscf_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;  
}

int tscf_phy_eee_set(const phymod_phy_access_t* phy, uint32 enable)
{
    return PHYMOD_E_NONE;
}

int tscf_phy_eee_get(const phymod_phy_access_t* phy, uint32* enable)
{
    return PHYMOD_E_NONE;
}



#endif /* PHYMOD_TSCF_SUPPORT */
