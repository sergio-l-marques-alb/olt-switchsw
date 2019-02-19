/*
 * $Id: $
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
 * File: jer_egr_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_port.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/phy/phymod_sim.h>
#include <soc/dpp/port_sw_db.h>

#define NUM_OF_LANES_IN_PM                4
#define MAX_NUM_OF_PMS_IN_ILKN            6

#define SOC_JERICHO_NOF_LANES_PER_CORE    4
#define SOC_JERICHO_NOF_LANES_PER_NBI     24
#define SOC_JERICHO_NOF_PMS_PER_NBI       6
#define SOC_JERICHO_NOF_PORTS_NBIH        24
#define SOC_JERICHO_NOF_PORTS_EACH_NBIL   60

#define SOC_JERICHO_SIM_MASK_NBIH   (0 << 5)
#define SOC_JERICHO_SIM_MASK_NBIL0  (1 << 5)
#define SOC_JERICHO_SIM_MASK_NBIL1  (2 << 5)

#define JER_NIF_PHY_SIF_PORT_0_NBIL0 (68)
#define JER_NIF_PHY_SIF_PORT_1_NIBL0_PM4 (52)
#define JER_NIF_PHY_SIF_PORT_1_NBIL0_PM5 (76)

#define JER_NIF_PHY_SIF_PORT_0_NBIL1 (128)
#define JER_NIF_PHY_SIF_PORT_1_NBIL1_PM4 (112)
#define JER_NIF_PHY_SIF_PORT_1_NBIL1_PM5 (136)

#define JER_NIF_PHY_SIF_PORT_0_NBIH (20)
#define JER_NIF_PHY_SIF_PORT_1_NBIH (22)

#define SOC_JERICHO_PORT_PHY_ADDR_INVALID 0xffffffff

STATIC void* pm4x25_user_acc[SOC_MAX_NUM_DEVICES]= {NULL};
STATIC void* pm4x10_user_acc[SOC_MAX_NUM_DEVICES]= {NULL};
STATIC void *fabric_user_acc[SOC_MAX_NUM_DEVICES] = {NULL};

extern unsigned char  tscf_ucode_dnx[];
extern unsigned short tscf_ucode_dnx_len;

#define SUB_PHYS_IN_QSGMII                  4

#define SOC_JER_NIF_NOF_LANE_QUADS          18

phymod_bus_t portmod_ext_default_bus = {
    "MDIO Bus",
    portmod_common_phy_mdio_c45_reg_read,
    portmod_common_phy_mdio_c45_reg_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

int
soc_jer_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy) 
{
    int qsgmii_count, skip;
    SOCDNX_INIT_FUNC_DEFS;

    if(phy < 37) {
        skip = 0;
    } else {
        qsgmii_count = phy - 37;
        if (qsgmii_count > 12) {
            qsgmii_count = 12;
        }
        if (phy > 15*4) {
           qsgmii_count += phy - 61;
        }

        skip = qsgmii_count*(SUB_PHYS_IN_QSGMII - 1);
    }

    *new_phy = phy + skip;

    SOCDNX_FUNC_RETURN;
}


int
soc_qax_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy) 
{
    int qsgmii_count, skip;
    SOCDNX_INIT_FUNC_DEFS;

    if(phy < 17) {
        skip = 0;
    } else {
        qsgmii_count = phy - 17;
        if (qsgmii_count > 12) {
            qsgmii_count = 12;
        }
        if (phy > 8*4) {
           qsgmii_count += phy - 33; 
           if (qsgmii_count > 24) {
               qsgmii_count = 24;
           }
        }

        skip = qsgmii_count*(SUB_PHYS_IN_QSGMII - 1);
    }

    *new_phy = phy + skip;

    SOCDNX_FUNC_RETURN;
}

int
soc_jer_qsgmii_offsets_add_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp) 
{
    uint32 phy, new_phy;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*new_pbmp);

    SOC_PBMP_ITER(*pbmp, phy) 
    {
        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_qax_qsgmii_offsets_add(unit, phy, &new_phy)); 
        } else {
            SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add(unit, phy, &new_phy)); 
        }
        SOC_PBMP_PORT_ADD(*new_pbmp, new_phy);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy) 
{
    int qsgmii_count;
    int reduce = 0;
    SOCDNX_INIT_FUNC_DEFS;

    reduce = 0;

    if(phy >= 37) {
        qsgmii_count = phy - 37;
        if (qsgmii_count > 48) {
            qsgmii_count = 48;
        } 

        if (phy >= 97) {
           qsgmii_count += phy - 97;
        }

        reduce = qsgmii_count - (qsgmii_count/SUB_PHYS_IN_QSGMII);
        
    }

    *new_phy = phy - reduce;

    SOCDNX_FUNC_RETURN;
}

int
soc_jer_qsgmii_offsets_remove_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp) 
{
    uint32 phy, new_phy;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*new_pbmp);

    SOC_PBMP_ITER(*pbmp, phy) 
    {

        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, phy, &new_phy));
        SOC_PBMP_PORT_ADD(*new_pbmp, new_phy);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_lane_map_get(int unit, int quad, phymod_lane_map_t* lane_map)
{
    uint32 txlane_map, rxlane_map, mask, shift;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_lane_map_t_init(lane_map));

    txlane_map = soc_property_suffix_num_get(unit, quad, spn_PHY_TX_LANE_MAP, "quad", 0x3210);
    LOG_DEBUG(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "quad %d - txlane_map 0x%x \n"),quad , txlane_map));
    rxlane_map = soc_property_suffix_num_get(unit, quad, spn_PHY_RX_LANE_MAP, "quad", 0x3210);
    LOG_DEBUG(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "quad %d - rxlane_map 0x%x \n"),quad , rxlane_map));

    lane_map->num_of_lanes = NUM_OF_LANES_IN_PM;

    mask = 0xf;
    shift = 0;
    for(i=0 ; i<NUM_OF_LANES_IN_PM ; i++) {
        lane_map->lane_map_tx[i] =  ((txlane_map & mask) >> shift);
        lane_map->lane_map_rx[i] =  ((rxlane_map & mask) >> shift);
        mask = mask << 4;
        shift = shift + 4;
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_portmod_calc_os(int unit, phymod_phy_access_t* phy_access, uint32* os_int, uint32* os_remainder)
{
    int rv;
    phymod_phy_diagnostics_t phy_diag;
    SOCDNX_INIT_FUNC_DEFS;

    rv = phymod_phy_diagnostics_t_init(&phy_diag);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = phymod_phy_diagnostics_get(phy_access, &phy_diag);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = phymod_osr_mode_to_actual_os(phy_diag.osr_mode, os_int, os_remainder);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_soft_reset(int unit, soc_port_t port, portmod_call_back_action_type_t action)
{
    int reg, reg_port, index, val;
    uint32  phy_port, lane, lane_i, rst_port;
    soc_pbmp_t phys, lanes;
    uint64 reg_val;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, phy_port, &lane));
    
    switch (action) {
    case portmodCallBackActionTypeDuring:
        reg = (lane < SOC_JERICHO_NOF_PORTS_NBIH) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;
        reg_port = (lane < SOC_JERICHO_NOF_PORTS_NBIH + 1) ? REG_PORT_ANY : lane > (2 * SOC_JERICHO_NOF_LANES_PER_NBI); 
        index = ((lane - 1) % SOC_JERICHO_NOF_LANES_PER_NBI) / NUM_OF_LANES_IN_PM;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &lanes));

        val = (interface_type == SOC_PORT_IF_QSGMII) ? 0xf : 0x1;
        rst_port = 0;
        SOC_PBMP_ITER(lanes, lane_i) {
            rst_port |= val << (((lane_i - 1) % 4) * 4);
        }

        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, index, &reg_val));
        soc_reg64_field32_set(unit, reg, &reg_val, TX_RESET_PORT_CREDITS_VALUE_QMLF_Nf, 0);
        soc_reg64_field32_set(unit, reg, &reg_val, TX_RESET_PORT_CREDITS_QMLF_Nf, 0);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, index, reg_val));

        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, index, &reg_val));
        soc_reg64_field32_set(unit, reg, &reg_val, TX_RESET_PORT_CREDITS_QMLF_Nf, rst_port);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, index, reg_val));

        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, index, &reg_val));
        soc_reg64_field32_set(unit, reg, &reg_val, TX_RESET_PORT_CREDITS_QMLF_Nf, 0);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, index, reg_val));
        break;
    default:
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_portmod_pmh_init(int unit)
{
    int pm, phy, l;
    soc_dpp_config_t *dpp = NULL;
    portmod_pm_create_info_t pm_info;
    portmod_pm_identifier_t ilkn_pms[MAX_NUM_OF_PMS_IN_ILKN];
    portmod_default_user_access_t* user_acc;
    soc_pbmp_t ilkn_phys;
    int is_sim, blk, first_phy;
    uint32 rx_polarity, tx_polarity, reg_val, otp_bits;
    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);
    SOC_PBMP_CLEAR(ilkn_phys);

    for(pm = 0 ; pm < SOC_JERICHO_PM_4x25 ; pm++)
    {
        portmod_pm_identifier_t_init(unit, &ilkn_pms[pm]);
    }

    /* allocate user access struct */
    user_acc = sal_alloc(sizeof(portmod_default_user_access_t)*SOC_JERICHO_PM_4x25, "PM4x25_USER_ACCESS");
    if(user_acc == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate user access memory for PM4x25")));
    }
    sal_memset(user_acc, 0, sizeof(portmod_default_user_access_t)*SOC_JERICHO_PM_4x25);
    pm4x25_user_acc[unit] = user_acc;

    /* Initialize PM4x25, phys 0-23 */
    for(pm=0 ; pm<SOC_JERICHO_PM_4x25 ; pm++) {
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));

        pm_info.type = portmodDispatchTypePm4x25;
        ilkn_pms[pm].type = portmodDispatchTypePm4x25;
        first_phy = NUM_OF_LANES_IN_PM*pm + 1;
        ilkn_pms[pm].phy = first_phy;

        SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(&(pm_info.pm_specific_info.pm4x25.polarity)));
        for(l=0 ; l<NUM_OF_LANES_IN_PM ; l++) {
            phy = first_phy + l;
            SOC_PBMP_PORT_ADD(pm_info.phys, phy);
            SOC_PBMP_PORT_ADD(ilkn_phys, phy);

            rx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_RX_POLARITY_FLIP, "phy", 0);
            tx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_TX_POLARITY_FLIP, "phy", 0);

            pm_info.pm_specific_info.pm4x25.polarity.rx_polarity |= ((rx_polarity & 0x1) << l);
            pm_info.pm_specific_info.pm4x25.polarity.tx_polarity |= ((tx_polarity & 0x1) << l);
        }

        SOCDNX_IF_ERR_EXIT(soc_to_phymod_ref_clk(unit, dpp->jer->pll.ref_clk_pmh_out, &(pm_info.pm_specific_info.pm4x25.ref_clk)));

        /* Init phy access structure */
        SOCDNX_IF_ERR_EXIT(phymod_access_t_init(&pm_info.pm_specific_info.pm4x25.access));

        SOCDNX_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, &(user_acc[pm])));
        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(user_acc[pm])));
        user_acc[pm].unit = unit;
        blk = CLP_BLOCK(unit, pm);
        user_acc[pm].blk_id = blk;
        user_acc[pm].mutex = sal_mutex_create("core mutex");
        if(user_acc[pm].mutex == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate mutex for PM4x25")));
        }

        pm_info.pm_specific_info.pm4x25.access.user_acc = &(user_acc[pm]);
        pm_info.pm_specific_info.pm4x25.access.addr = pm*NUM_OF_LANES_IN_PM; /*PMH PHY addresses are 0,4,8,...*/
        pm_info.pm_specific_info.pm4x25.access.bus = NULL; /* Use default bus */
        SOCDNX_IF_ERR_EXIT(soc_physim_check_sim(unit, phymodDispatchTypeTscf, &(pm_info.pm_specific_info.pm4x25.access), SOC_JERICHO_SIM_MASK_NBIH, &is_sim));

        /* Firmward loader */
        if(is_sim) {
            pm_info.pm_specific_info.pm4x25.fw_load_method = phymodFirmwareLoadMethodNone;
            SOC_DPP_JER_CONFIG(unit)->nif.fw_verify[pm] = 0;
        } else {
            pm_info.pm_specific_info.pm4x25.fw_load_method = soc_property_suffix_num_get(unit, pm, spn_LOAD_FIRMWARE, "quad", phymodFirmwareLoadMethodExternal);
            SOC_DPP_JER_CONFIG(unit)->nif.fw_verify[pm] = (pm_info.pm_specific_info.pm4x25.fw_load_method & 0xff00 ? 1 : 0);
            pm_info.pm_specific_info.pm4x25.fw_load_method &= 0xff;
        }
        pm_info.pm_specific_info.pm4x25.external_fw_loader = NULL; /* Use default external loader */

        /* Lane mapping */
        SOCDNX_IF_ERR_EXIT(soc_jer_lane_map_get(unit, pm, &(pm_info.pm_specific_info.pm4x25.lane_map)));

        /* General PM4x25 configuration */
        pm_info.pm_specific_info.pm4x25.in_pm_12x10 = 0;

        pm_info.pm_specific_info.pm4x25.portmod_mac_soft_reset = soc_jer_portmod_soft_reset;

        /* Add PM to PortMod*/
        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

        if (!SOC_WARM_BOOT(unit)) {
            /* Power Down on all ports - only active ports will be enabled */
            SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_CFGr(unit, pm, &reg_val));
            otp_bits =  soc_reg_field_get(unit, NBIH_NIF_PM_CFGr, reg_val, PMH_N_OTP_PORT_BOND_OPTIONf);
            otp_bits &= ~0x100; /* disable all quads - bit[8]  */
            soc_reg_field_set(unit, NBIH_NIF_PM_CFGr, &reg_val, PMH_N_OTP_PORT_BOND_OPTIONf, otp_bits);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_CFGr(unit, pm, reg_val));
        }

    }

    /* Add PMH ILKN */
    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = MAX_NUM_OF_PMS_IN_ILKN;
    pm_info.pm_specific_info.os_ilkn.wm_high = 14;
    pm_info.pm_specific_info.os_ilkn.wm_low = 15;
    SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys);

    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));


exit:
    if(SOCDNX_FUNC_ERROR) {
        if(pm4x25_user_acc[unit] != NULL) {
            for(pm=0 ; pm<SOC_JERICHO_PM_4x25 ; pm++) {
                user_acc = (portmod_default_user_access_t*)(pm4x25_user_acc[unit]);
                if(user_acc[pm].mutex != NULL) {
                    sal_mutex_destroy(user_acc[pm].mutex);
                }
            }
            sal_free(pm4x25_user_acc);
            pm4x25_user_acc[unit] = NULL;
        }
    }
    SOCDNX_FUNC_RETURN;
}

typedef struct soc_jer_pml_quads_s {
    int is_qsgmii;
    int is_qmx_only;
    int pml_instance;
    int phy_id;
} soc_jer_pml_quads_t;

static soc_jer_pml_quads_t soc_jer_pml_info[] = {
     /* is_qsgmii, is_qmx_only, pml_instance , PHY ID (used as core address in PHYMOD*/
        {0,         0,          0,            0x00},
        {0,         0,          0,            0x04},
        {0,         0,          0,            0x08},
        {0,         0,          0,            0x0c},
        {1,         0,          0,            0x10},
        {1,         0,          0,            0x14},

        {0,         1,          1,            0x00},
        {0,         1,          1,            0x04},
        {0,         1,          1,            0x08},
        {0,         0,          1,            0x0c},
        {1,         0,          1,            0x10},
        {1,         0,          1,            0x14},


};

#define SOC_JER_PML_BASE_LANE 24 
#define SOC_QMX_NUM_OF_PMLS   2

STATIC int
soc_jer_portmod_pml_init(int unit)
{
    int pm, phy, l, quad, pml, xlp_instance, block_index, is_sim;
    soc_dpp_config_t *dpp = NULL;
    portmod_pm_create_info_t pm_info;
    portmod_pm4x10_create_info_t *pm4x10_info;
    portmod_default_user_access_t* user_acc;
    soc_pbmp_t ilkn_phys[SOC_QMX_NUM_OF_PMLS];
    portmod_pm_identifier_t ilkn_pms[SOC_QMX_NUM_OF_PMLS][MAX_NUM_OF_PMS_IN_ILKN];
    int nof_ilkn_pms[SOC_QMX_NUM_OF_PMLS]= {0 , 0};
    int first_phy, pm_idx, idx, otp_bits;
    uint32 ident_bits, rx_polarity, tx_polarity, reg_val;
    soc_reg_t reg;
    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);

    for(pml=0 ; pml<SOC_QMX_NUM_OF_PMLS ; pml++) {
        for(pm = 0 ; pm < MAX_NUM_OF_PMS_IN_ILKN ; pm++)
        {
            portmod_pm_identifier_t_init(unit, &ilkn_pms[pml][pm]);
        }
    }

    for(pml=0 ; pml<SOC_QMX_NUM_OF_PMLS ; pml++) {
        SOC_PBMP_CLEAR(ilkn_phys[pml]);
    }

    /* allocate user access struct */
    user_acc = sal_alloc(sizeof(portmod_default_user_access_t)*SOC_JERICHO_PM_4x10, "PM4x10_USER_ACCESS");
    if(user_acc == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate user access memory for PM4x10")));
    }
    pm4x10_user_acc[unit] = user_acc;

    xlp_instance = 0;
    /* Initialize PM4x10\PM4x10Q */
    for(pm=0 ; pm<SOC_JERICHO_PM_4x10 ; pm++) {
        if(!SOC_IS_QMX(unit) && soc_jer_pml_info[pm].is_qmx_only) {
            ++xlp_instance;
            continue;
        }
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        block_index = XLP_BLOCK(unit, xlp_instance); 

        if(soc_jer_pml_info[pm].is_qsgmii) {
            pm_info.type = portmodDispatchTypePm4x10Q;
            pm4x10_info = &pm_info.pm_specific_info.pm4x10q.pm4x10_info;
        } else {
            pm_info.type = portmodDispatchTypePm4x10;
            pm4x10_info = &pm_info.pm_specific_info.pm4x10;
        }
        pml = soc_jer_pml_info[pm].pml_instance;
        ilkn_pms[pml][nof_ilkn_pms[pml]].type = pm_info.type;
        first_phy = SOC_JER_PML_BASE_LANE + NUM_OF_LANES_IN_PM*pm + 1;
        ilkn_pms[pml][nof_ilkn_pms[pml]].phy = first_phy;
        nof_ilkn_pms[pml]++;

        SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(&(pm4x10_info->polarity)));

        for(l=0 ; l<NUM_OF_LANES_IN_PM ; l++) {
            phy = first_phy + l;
            SOC_PBMP_PORT_ADD(pm_info.phys, phy);
            SOC_PBMP_PORT_ADD(ilkn_phys[soc_jer_pml_info[pm].pml_instance], phy);

            rx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_RX_POLARITY_FLIP, "phy", 0);
            tx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_TX_POLARITY_FLIP, "phy", 0);

            pm4x10_info->polarity.rx_polarity |= ((rx_polarity & 0x1) << l);
            pm4x10_info->polarity.tx_polarity |= ((tx_polarity & 0x1) << l);            
        }

        SOCDNX_IF_ERR_EXIT(soc_to_phymod_ref_clk(unit, dpp->jer->pll.ref_clk_pmh_out, &(pm_info.pm_specific_info.pm4x25.ref_clk)));

        SOCDNX_IF_ERR_EXIT(phymod_access_t_init(&pm4x10_info->access));

        SOCDNX_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, &(user_acc[pm])));
        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(user_acc[pm])));
        user_acc[pm].unit = unit;
        user_acc[pm].blk_id = block_index;
        user_acc[pm].mutex = sal_mutex_create("core mutex");
        if(user_acc[pm].mutex == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate mutex for PM4x10")));
        }

        /* Firmward loader */
        quad = SOC_JERICHO_PM_4x25 + pm; /* PML quads are after PMH */
        pm4x10_info->fw_load_method = soc_property_suffix_num_get(unit, quad, spn_LOAD_FIRMWARE, "quad", phymodFirmwareLoadMethodExternal);
        SOC_DPP_JER_CONFIG(unit)->nif.fw_verify[quad] = (pm4x10_info->fw_load_method & 0xff00 ? 1 : 0);
        pm4x10_info->fw_load_method &= 0xff;
        pm4x10_info->external_fw_loader = NULL; /* Use default external loader */

        pm4x10_info->access.user_acc = &(user_acc[pm]);
        pm4x10_info->access.addr = soc_jer_pml_info[pm].phy_id;
        pm4x10_info->access.bus = NULL; /* Use default bus */

        /* check sim */
        ident_bits = pml ? SOC_JERICHO_SIM_MASK_NBIL1 : SOC_JERICHO_SIM_MASK_NBIL0;
        SOCDNX_IF_ERR_EXIT(soc_physim_check_sim(unit, phymodDispatchTypeTsce, &(pm_info.pm_specific_info.pm4x10.access), ident_bits, &is_sim));
        if(is_sim) {
            pm_info.pm_specific_info.pm4x10.fw_load_method = phymodFirmwareLoadMethodNone;
            SOC_DPP_JER_CONFIG(unit)->nif.fw_verify[quad] = 0;
        } 

        SOCDNX_IF_ERR_EXIT(soc_jer_lane_map_get(unit, quad, &(pm4x10_info->lane_map)));

        pm_info.pm_specific_info.pm4x10.portmod_mac_soft_reset = soc_jer_portmod_soft_reset;

        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
        xlp_instance++;

        pm_idx = pm % 6;
        switch (pm_idx) {
        case 3:
            reg = NBIL_NIF_PM_CFG_3r;
            idx = 0;
            break;
        case 4:
            reg = NBIL_NIF_PM_CFG_4r;
            idx = 0;
            break;
        case 5:
            reg = NBIL_NIF_PM_CFG_5r;
            idx = 0;
            break;
        default:
            reg = NBIL_NIF_PM_CFGr;
            idx = pm_idx;
            break;
        }

        if (!SOC_WARM_BOOT(unit)) {
            /* Power Down on all ports - only active ports will be enabled */
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, pml, idx, &reg_val));
            otp_bits =  soc_reg_field_get(unit, reg, reg_val, PML_N_OTP_PORT_BOND_OPTIONf);
            otp_bits &= ~0x100; /* disable all quads- bit [8] */
            soc_reg_field_set(unit, reg, &reg_val, PML_N_OTP_PORT_BOND_OPTIONf, otp_bits);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, pml, idx, reg_val));
        }

    }

    /* Add PML0 ILKN */
    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms[0];
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = nof_ilkn_pms[0];
    SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys[0]);
    pm_info.pm_specific_info.os_ilkn.wm_high = 9;
    pm_info.pm_specific_info.os_ilkn.wm_low = 10;
    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

    if(!SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric) {
        /* Add PML1 ILKN */
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        pm_info.type = portmodDispatchTypePmOsILKN;
        pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms[1];
        pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = nof_ilkn_pms[1];
        SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys[1]);
        pm_info.pm_specific_info.os_ilkn.wm_high = 9;
        pm_info.pm_specific_info.os_ilkn.wm_low = 10;
        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
int soc_jer_fabric_mdio_address_get(int unit, int core_index, uint16 *phy_addr){
    uint16 bus_id = 0;
    uint16 port = 0;

    /* 
      Bus 1 cores: 0-2
      Bus 2 cores  3-5
      Bus 3 cores  6-8
     */
    bus_id = (core_index / 3) + 1 ; 

    /* phy ids: 0,4,8 */
    port = (core_index % 3) * 4;

    /* 
      encode for MIIM format:
      bits 0-4 for the "port" address 
      bits 5-6 and 8-9 for bus
      bit 7 for internal/external 
      0x80 for internal port
     */
    *phy_addr = 0x80 | ((bus_id & 0x3)<<PHY_ID_BUS_LOWER_SHIFT) | ((bus_id & 0xc)<<PHY_ID_BUS_UPPER_SHIFT) | port;

    return SOC_E_NONE;
}

STATIC int
soc_jer_portmod_fabric_init(int unit)
{
    int rv, quads_in_fsrd, nof_pms, nof_ilkn_pms, pm, link;
    portmod_pm_identifier_t ilkn_pms[MAX_NUM_OF_PMS_IN_ILKN];
    portmod_pm_create_info_t pm_info;
    soc_pbmp_t ilkn_phys;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    quads_in_fsrd = SOC_DPP_DEFS_GET(unit, nof_quads_in_fsrd);

    if(SOC_IS_QMX(unit)) {
        nof_pms = SOC_QMX_PM_FABRIC;
        nof_ilkn_pms = SOC_QMX_PM_FABRIC;
    } else {
        nof_pms = SOC_JERICHO_PM_FABRIC;
        nof_ilkn_pms = MAX_NUM_OF_PMS_IN_ILKN;
    }
    rv = soc_dcmn_fabric_pms_add(unit, nof_pms, FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id), 1, quads_in_fsrd, soc_jer_fabric_mdio_address_get, &fabric_user_acc[unit]);
    SOCDNX_IF_ERR_EXIT(rv);

    if(SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric) {

        SOC_PBMP_CLEAR(ilkn_phys);

        /* build ilkn_pms info */
        for(pm=0 ; pm<nof_ilkn_pms ; pm++) {
            /* first 24 fabric lanes can be used as ILKN (Jer)*/
            port = FABRIC_LOGICAL_PORT_BASE(unit) + pm*NUM_OF_LANES_IN_PM;
            ilkn_pms[pm].phy = SOC_INFO(unit).port_l2p_mapping[port];
            ilkn_pms[pm].type = portmodDispatchTypeDnx_fabric;

            for(link=0; link<NUM_OF_LANES_IN_PM ; link++) {
                SOC_PBMP_PORT_ADD(ilkn_phys, ilkn_pms[pm].phy + link);
            }
        }
        

        /* Add PML1 ILKN */
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        pm_info.type = portmodDispatchTypePmOsILKN;
        pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
        pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = nof_ilkn_pms;
        SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys);
        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_portmod_register_external_phys(int unit)
{
    int port_i;
    soc_port_t xphy_logical_port;
    uint32 xphy_mdio_addr, xphy_physical_port, tmp_xphy_physical_port;
    portmod_default_user_access_t* local_user_access;
    phymod_core_access_t tmp_ext_phy_access; 

    int iphy = 0, xphy = 0, lport = 0, core_index = PORTMOD_CORE_INDEX_INVALID;
    int port_primary = 0, port_offset = 0, port_phy_addr = 0;
    portmod_phy_core_info_t core_info;
    portmod_lane_connection_t lane_connection;

    SOCDNX_INIT_FUNC_DEFS;

    /*
     *  NIF Exteranl PHY
     */
    /* TBD */
    
    SOCDNX_IF_ERR_EXIT(soc_phy_common_init(unit));
    SOCDNX_IF_ERR_EXIT(soc_phyctrl_software_init(unit));

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {

        xphy_mdio_addr = soc_property_port_get(unit, port_i, spn_PORT_PHY_ADDR, SOC_JERICHO_PORT_PHY_ADDR_INVALID);
        if (xphy_mdio_addr == SOC_JERICHO_PORT_PHY_ADDR_INVALID) {
            continue;
        }

        xphy_logical_port = port_i;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, xphy_logical_port, &tmp_xphy_physical_port));
        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, tmp_xphy_physical_port, &xphy_physical_port));
        
        LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "%s(): EXT PHY Add for port %d phy %d ext_phy_addr 0x%x..\n"),FUNCTION_NAME(), xphy_logical_port, xphy_physical_port, xphy_mdio_addr));

        phymod_core_access_t_init(&tmp_ext_phy_access);
        tmp_ext_phy_access.access.bus = &portmod_ext_default_bus;
        tmp_ext_phy_access.access.addr = xphy_mdio_addr; 
        tmp_ext_phy_access.type = phymodDispatchTypeCount; /* Make sure it is invalid. */
        local_user_access = sal_alloc(sizeof(portmod_default_user_access_t), "pm4x10_specific_db");
        sal_memset(local_user_access, 0, sizeof(portmod_default_user_access_t));
        local_user_access->unit = unit;
        /*
        local_user_access->port = xphy_logical_port;
        local_user_access->is_legacy_phy_present = 0;
        local_user_access->cmd_for_phy = 0; 
        */ 
        tmp_ext_phy_access.access.user_acc = local_user_access;
        SOCDNX_IF_ERR_EXIT(portmod_ext_phy_attach(unit, xphy_physical_port, &tmp_ext_phy_access, 0x0));
    }

    /*
     *  Fabric Exteranl PHY
     */
    SOCDNX_IF_ERR_EXIT(portmod_phy_core_info_t_init(unit, &core_info));
    SOCDNX_IF_ERR_EXIT(portmod_lane_connection_t_init(unit, &lane_connection));       
    for (iphy = 0; iphy < SOC_MAX_NUM_PORTS; iphy++)
    {
        lport = SOC_INFO(unit).port_p2l_mapping[iphy];
        if (lport <= 0)
        {
            continue;
        }
        if(IS_SFI_PORT(unit, lport)) {
            port_primary = SOC_INFO(unit).port_l2pp_mapping[lport];
            port_offset = SOC_INFO(unit).port_l2po_mapping[lport];
            port_phy_addr = SOC_INFO(unit).port_l2pa_mapping[port_primary];
            if ((PORTMOD_PRIMARY_PORT_INVALID != port_primary) && \
                (PORTMOD_PORT_OFFSET_INVALID != port_offset)){
                xphy = SOC_INFO(unit).port_l2p_mapping[port_primary];
                core_index = xphy/SOC_JERICHO_NOF_LANES_PER_CORE;
                if(SOC_E_UNAVAIL == portmod_phychain_ext_phy_info_get(unit, 1, core_index, &core_info)){
                    SOCDNX_IF_ERR_EXIT(soc_dcmn_external_phy_core_access_get(unit, port_phy_addr, &core_info.core_access.access));
                    SOCDNX_IF_ERR_EXIT(portmod_phychain_ext_phy_info_set(unit, 1, core_index, &core_info));
                }
                lane_connection.core_index = core_index;
                lane_connection.lane_index = port_offset;
                SOCDNX_IF_ERR_EXIT(portmod_ext_phy_lane_attach(unit, iphy, 1, &lane_connection));
            }
        }
                
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_init(int unit)
{
    portmod_pm_instances_t pm_types_and_instances[] = {
        {portmodDispatchTypeDnx_fabric, SOC_JERICHO_PM_FABRIC},
        {portmodDispatchTypePm4x25, SOC_JERICHO_PM_4x25},
        {portmodDispatchTypePm4x10, 5},
        {portmodDispatchTypePm4x10Q, 4},
        {portmodDispatchTypePmOsILKN, 3},
        /*QMX extras*/
        {portmodDispatchTypePm4x10, 3},
    };

    int pms_instances_arr_len = 5;
    int i, inst, is_nbil_en;
    uint64 reg64_val;
    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QMX(unit)){
        pms_instances_arr_len = 6;
    }

    if (!SOC_WARM_BOOT(unit)) {
        for (i = 0; i < SOC_JERICHO_NOF_PMS_PER_NBI; ++i) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_TX_QMLF_CONFIGr(unit, i, &reg64_val)); 
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_0_QMLF_Nf, 1);
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_1_QMLF_Nf, 1);
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_2_QMLF_Nf, 1);
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_3_QMLF_Nf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_TX_QMLF_CONFIGr(unit, i, reg64_val));

            is_nbil_en = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "is_nbil_valid", 0);
            if (is_nbil_en) {
                for (inst = 0; inst < SOC_JER_NOF_INSTANCES_NBIL; ++inst) {
                    SOCDNX_IF_ERR_EXIT(READ_NBIL_TX_QMLF_CONFIGr(unit, inst, i, &reg64_val)); 
                    soc_reg64_field32_set(unit, NBIL_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_0_QMLF_Nf, 1);
                    soc_reg64_field32_set(unit, NBIL_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_1_QMLF_Nf, 1);
                    soc_reg64_field32_set(unit, NBIL_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_2_QMLF_Nf, 1);
                    soc_reg64_field32_set(unit, NBIL_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_3_QMLF_Nf, 1);
                    SOCDNX_IF_ERR_EXIT(WRITE_NBIL_TX_QMLF_CONFIGr(unit, inst, i, reg64_val));
                }
            }
        }
    }

    SOCDNX_IF_ERR_EXIT(portmod_create(unit, 0, SOC_MAX_NUM_PORTS, SOC_MAX_NUM_PORTS, pms_instances_arr_len, pm_types_and_instances));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_pmh_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_pml_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_fabric_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_register_external_phys(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_portmod_post_init(int unit, soc_pbmp_t* ports)
{
    int val, rv, nof_phys, credits_val;
    uint32 is_master, is_qsgmii, first_phy, flags, reg_val, phy_lane;
    int qmlf_index = 0, reg_port, sif0_val;
    uint32 rst_port_crdt = 0, orig_port_crdt;
    uint64 reg64_val;
    soc_pbmp_t phys, lanes;
    soc_reg_t reg;
    soc_port_t port, lane;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        SOC_PBMP_ITER(*ports, port) {
            if (!IS_SFI_PORT(unit, port)) {

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

                if(is_master) {

                    /* speed config */
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &val));
                    if(-1 != val && 42000 != val) {
                        if(val == 0) {
                            val = SOC_INFO(unit).port_speed_max[port];
                        }
                        rv = soc_jer_portmod_port_speed_set(unit, port, val);
                        SOCDNX_IF_ERR_EXIT(rv);
                    }
                    
                    if (interface_type != SOC_PORT_IF_ILKN) {

                        /* AN config */
                        val = soc_property_port_get(unit, port, spn_PORT_INIT_AUTONEG, -1);
                        if(val != -1) {
                            rv = soc_jer_portmod_autoneg_set(unit, port, val);
                            SOCDNX_IF_ERR_EXIT(rv);
                        }  

                        /* MAX RX Size */
                        val = soc_property_get(unit, spn_BCM_STAT_JUMBO, -1);
                        if(val != -1) {
                            rv = portmod_port_cntmaxsize_set(unit, port, val);
                            SOCDNX_IF_ERR_EXIT(rv);    
                        }

                        /* override_pm_credits_to_nbi */
                        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
                        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &lanes));
                        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
                        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, first_phy, &phy_lane));
                        --phy_lane; /* phy_lane returned is one-based */
                        --first_phy; /* first_phy returned is one-based */

                        reg_port = (first_phy < SOC_JERICHO_NOF_PORTS_NBIH) ? REG_PORT_ANY : (first_phy / (SOC_JERICHO_NOF_PORTS_NBIH + SOC_JERICHO_NOF_PORTS_EACH_NBIL));

                        qmlf_index = (phy_lane % SOC_JERICHO_NOF_PORTS_NBIH) / NUM_OF_LANES_IN_PM; 
                        is_qsgmii = (interface_type == SOC_PORT_IF_QSGMII);
                        SOC_PBMP_COUNT(phys, nof_phys);

                        credits_val = (is_qsgmii) ? 4 : (nof_phys * 8);
                        val = (is_qsgmii) ? 0xf : 0x1;
                        rst_port_crdt = 0;
                        
                        SOC_PBMP_ITER(lanes, lane) {
                            rst_port_crdt |= val << (((lane - 1) % 4) * 4);
                        }

                        reg = (first_phy < SOC_JERICHO_NOF_PORTS_NBIH) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;
                        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, qmlf_index, &reg64_val));
                        orig_port_crdt = soc_reg64_field32_get(unit, reg, reg64_val, TX_RESET_PORT_CREDITS_QMLF_Nf);
                        rst_port_crdt |= orig_port_crdt;
                        soc_reg64_field32_set(unit, reg, &reg64_val, TX_RESET_PORT_CREDITS_QMLF_Nf, rst_port_crdt);
                        soc_reg64_field32_set(unit, reg, &reg64_val, TX_RESET_PORT_CREDITS_VALUE_QMLF_Nf, credits_val);
                        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, qmlf_index, reg64_val));

                        /*bug-fix : reset value of SIF1 in PLM4 (PM10 or PM16) are taken from SIF0 */
                        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

                        if (SOC_PORT_IS_STAT_INTERFACE(flags) && (qmlf_index == 4)) {
                            SOCDNX_IF_ERR_EXIT(READ_NBIL_SIF_CFGr(unit, reg_port, 1, &reg_val));
                            sif0_val = soc_reg_field_get(unit, NBIL_SIF_CFGr, reg_val, SIF_PORT_N_RESET_PORT_CREDITS_VALUEf);
                            soc_reg_field_set(unit, NBIL_SIF_CFGr, &reg_val, SIF_PORT_N_RESET_PORT_CREDITSf, 1);
                            soc_reg_field_set(unit, NBIL_SIF_CFGr, &reg_val, SIF_PORT_N_RESET_PORT_CREDITS_VALUEf, rst_port_crdt);
                            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_SIF_CFGr(unit, reg_port, 1, reg_val));

                            SOCDNX_IF_ERR_EXIT(READ_NBIL_SIF_CFGr(unit, reg_port, 0, &reg_val));
                            soc_reg_field_set(unit, NBIL_SIF_CFGr, &reg_val, SIF_PORT_N_RESET_PORT_CREDITS_VALUEf, sif0_val);
                            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_SIF_CFGr(unit, reg_port, 0, reg_val));
                        }
                    }
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_deinit(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_destroy(unit));

    if(pm4x25_user_acc[unit] != NULL) {
        sal_free(pm4x25_user_acc[unit]);
        pm4x25_user_acc[unit] = NULL;
    }

    if(pm4x10_user_acc[unit] != NULL) {
        sal_free(pm4x10_user_acc[unit]);
        pm4x10_user_acc[unit] = NULL;
    }
    
    if(fabric_user_acc[unit] != NULL) {
        sal_free(fabric_user_acc[unit]);
        fabric_user_acc[unit] = NULL;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
_soc_jer_portmod_speed_to_if_config(int unit, soc_port_t port, int speed, portmod_port_interface_config_t* config)
{
    uint32 is_hg, flags, num_lanes;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, config));

    config->speed = speed;

    /*NIF  config*/
    if (!SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port))
    {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        config->interface = interface_type;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
        config->port_num_lanes = num_lanes;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_get(unit, port, &is_hg));
        if (is_hg) {
            PHYMOD_INTF_MODES_HIGIG_SET(config);
            config->encap_mode = SOC_ENCAP_HIGIG2;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

        if (SOC_PORT_IS_SCRAMBLER(flags)) {
            PHYMOD_INTF_MODES_SCR_SET(config);
        }

        if (SOC_PORT_IS_FIBER(flags)) {
            PHYMOD_INTF_MODES_FIBER_SET(config);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
_soc_jer_port_fabric_config_get(int unit, soc_port_t port, int is_init_sequence, dcmn_port_fabric_init_config_t* port_config)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Set defaults */
    port_config->pcs = PORTMOD_PCS_64B66B_RS_FEC;
    port_config->speed = 25000;
    port_config->cl72 = 1;

    /* Update according to soc properties */
    if(is_init_sequence) {
        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_config_get(unit, port, port_config));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_port_fabric_clk_freq_init(int unit, soc_pbmp_t pbmp)
{
    soc_port_t port;
    int index;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(pbmp, port)
    {
        index = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port) / (SOC_DPP_DEFS_GET(unit, nof_fabric_links) / 2);
        if((soc_dcmn_init_serdes_ref_clock_125 == SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_out[index])) {
           SOC_INFO(unit).port_refclk_int[port] = 125;
        } else {
           SOC_INFO(unit).port_refclk_int[port] = 156;
        }
    }
    

    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence)
{
    int rv, speed, counter_interval, cl72;
    soc_port_t port;
    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    portmod_port_add_info_t add_info;
    uint32 is_master, flags, counter_flags, phy_lane;
    dcmn_port_fabric_init_config_t port_config;
    soc_pbmp_t phys, counter_pbmp;
    int broadcast_load_fabric = 0;
    dcmn_port_init_stage_t stage;
    phymod_firmware_load_method_t fw_load_method_fabric = phymodFirmwareLoadMethodNone;
    int fw_verify = 0, fw_verify_fabric = 0, first_phy = -1;
    uint32 first_phy_quad, quad, offset, reg_val, otp_bits;
    soc_reg_t reg;
    soc_field_t field;
    int reg_port, pm_idx, idx, fiber_pref;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_WARM_BOOT(unit)) {
        SOC_PBMP_ASSIGN(*okay_pbmp, pbmp);
    } else {

        SOC_PBMP_CLEAR(*okay_pbmp);

        if (is_init_sequence) {
            fw_load_method_fabric = soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, "fabric", phymodFirmwareLoadMethodExternal);
            fw_verify_fabric = (fw_load_method_fabric & 0xff00 ? 1 : 0);
            fw_load_method_fabric &= 0xff;

            if(fw_load_method_fabric == phymodFirmwareLoadMethodExternal) {
                 broadcast_load_fabric = 1;
            }
        }


        SOC_PBMP_ITER(pbmp, port) {
            if(IS_SFI_PORT(unit, port)) { 
                /* SFI is loaded in 2 stages - this is stage 1*/
                rv = _soc_jer_port_fabric_config_get(unit, port, is_init_sequence, &port_config);
                SOCDNX_IF_ERR_EXIT(rv);

                stage = broadcast_load_fabric ? dcmn_port_init_until_fw_load : dcmn_port_init_full;
                rv = soc_dcmn_fabric_port_probe(unit, port, stage, fw_verify_fabric, &port_config);
                SOCDNX_IF_ERR_EXIT(rv);
            } else {
                /* Init NIF ports*/
                rv = soc_jer_port_open_path(unit, port);
                SOCDNX_IF_ERR_EXIT(rv);

                rv = portmod_port_add_info_t_init(unit, &add_info);
                SOCDNX_IF_ERR_EXIT(rv);

                /* default mode - strup & append CRC at MAC level*/
                PORTMOD_PORT_ADD_F_RX_SRIP_CRC_SET(&add_info);
                PORTMOD_PORT_ADD_F_TX_APPEND_CRC_SET(&add_info);

                rv = soc_port_sw_db_speed_get(unit, port, &speed);
                SOCDNX_IF_ERR_EXIT(rv);

                rv = _soc_jer_portmod_speed_to_if_config(unit, port, speed, &(add_info.interface_config));
                SOCDNX_IF_ERR_EXIT(rv);

                rv = soc_port_sw_db_phy_ports_get(unit, port, &phys);
                SOCDNX_IF_ERR_EXIT(rv);

                rv = soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &(add_info.phys));
                SOCDNX_IF_ERR_EXIT(rv);

                SOC_PBMP_ITER(add_info.phys, phy_lane) {
                    quad = (phy_lane - 1) / 4;
                    reg_port = (quad / 6 == 0) ? REG_PORT_ANY : (quad / 6) - 1;
                    pm_idx = quad % 6;
                    if (reg_port == REG_PORT_ANY) {
                        reg = NBIH_NIF_PM_CFGr;
                        field = PMH_N_OTP_PORT_BOND_OPTIONf;
                        idx = pm_idx;
                    } else { 
                        field = PML_N_OTP_PORT_BOND_OPTIONf;
                        switch (pm_idx) {
                        case 3:
                            reg = NBIL_NIF_PM_CFG_3r;
                            idx = 0;
                            break;
                        case 4:
                            reg = NBIL_NIF_PM_CFG_4r;
                            idx = 0;
                            break;
                        case 5:
                            reg = NBIL_NIF_PM_CFG_5r;
                            idx = 0;
                            break;
                        default:
                            reg = NBIL_NIF_PM_CFGr;
                            idx = pm_idx;
                            break;
                        }
                    }
                    /* Enable power on active lanes */
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, idx, &reg_val));
                    otp_bits = soc_reg_field_get(unit, reg, reg_val, field);
                    otp_bits |= 0x100; /* enable this quad */
                    soc_reg_field_set(unit, reg, &reg_val, field, otp_bits);
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, idx, reg_val));
                    break; /* only need to do this once per quad */
                }
                first_phy_quad = (first_phy - 1) / 4;
                /*coverity[overrun-local]*/
                fw_verify = SOC_DPP_JER_CONFIG(unit)->nif.fw_verify[first_phy_quad];

                if(fw_verify) {
                    PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(&add_info);
                } else {
                    PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&add_info);
                }

                if (add_info.interface_config.interface == SOC_PORT_IF_ILKN) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));

                    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
                    add_info.ilkn_core_id = (offset & 1);
                    add_info.rx_retransmit = ilkn_config->retransmit.enable_rx;
                    add_info.tx_retransmit = ilkn_config->retransmit.enable_tx;

                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
                    add_info.flags = SOC_PORT_IS_ELK_INTERFACE(flags) ? PORTMOD_PORT_FLAGS_ELK : 0;
                }

                /* configure link training */
                if (is_init_sequence) {
                    cl72 = soc_property_port_get(unit, port, spn_PORT_INIT_CL72, 0) ? 1 : 0;
                    add_info.link_training_en = (uint8)cl72; 

                    fiber_pref = soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 0) ? 1 : 0;
                    if (fiber_pref) {
                        PHYMOD_INTF_MODES_FIBER_SET(&add_info.interface_config);
                    }
                }
                
                rv = portmod_port_add(unit, port, &add_info); 
                SOCDNX_IF_ERR_EXIT(rv);

                rv = soc_port_sw_db_initialized_set(unit, port, 1);
                SOCDNX_IF_ERR_EXIT(rv);

                SOC_PBMP_PORT_ADD(*okay_pbmp, port);
            }
        }

        if(broadcast_load_fabric) {
            /* Load fabric firmware*/
            rv = soc_dcmn_fabric_broadcast_firmware_loader(unit, tscf_ucode_dnx_len, tscf_ucode_dnx);
            SOCDNX_IF_ERR_EXIT(rv);
        }
            
        SOC_PBMP_ITER(pbmp, port) {
            if(IS_SFI_PORT(unit, port)) { 
                if(broadcast_load_fabric) {
                    /* fabric side initialization - stage 2*/
                    rv = _soc_jer_port_fabric_config_get(unit, port, is_init_sequence, &port_config);
                    SOCDNX_IF_ERR_EXIT(rv);
                    rv = soc_dcmn_fabric_port_probe(unit, port, dcmn_port_init_resume_after_fw_load, fw_verify_fabric, &port_config);
                    SOCDNX_IF_ERR_EXIT(rv);
                }

                SOC_PBMP_PORT_ADD(*okay_pbmp, port);
            }
        }
       
        /* Update Counter thread*/
        SOCDNX_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
        if(counter_interval > 0) {
            soc_counter_stop(unit);

            SOC_PBMP_ITER(*okay_pbmp, port) {
                if (!IS_SFI_PORT(unit, port)) {

                    SOCDNX_IF_ERR_RETURN(soc_port_sw_db_is_master_get(unit, port, &is_master));
                    if(!is_master) {
                        /* don't count per channel*/
                        if(SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode != soc_arad_stat_ilkn_counters_mode_packets_per_channel) {
                            continue;
                        }
                        /* Count per channel is supported only for ILKN */
                        if(add_info.interface_config.interface != SOC_PORT_IF_ILKN) {
                            continue;
                        }
                    }

                    /* counter thread is supported only for NIF or fabric ports */
                    SOCDNX_IF_ERR_RETURN(soc_port_sw_db_flags_get(unit, port, &flags));
                    if(!SOC_PORT_IS_NETWORK_INTERFACE(flags)) {
                        continue;
                    }

                    SOC_PBMP_PORT_ADD(counter_pbmp, port);
                } else {
                    SOC_PBMP_PORT_ADD(counter_pbmp, port);
                }
            }

            SOCDNX_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_enable_set(int unit, soc_port_t port, uint32 mac_only, int enable)
{
    uint32 flags = 0, reg_val, offset;
    soc_port_if_t interface;
    soc_port_t reg_port;
    soc_reg_t reg;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    if(mac_only) {
        PORTMOD_PORT_ENABLE_MAC_SET(flags);
    }
    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, flags, enable));

    if (!IS_SFI_PORT(unit, port)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

        if (interface == SOC_PORT_IF_ILKN) {
            /** set_ilkn_port_reset in wrapper **/
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
            reg = (offset < 2) ? ILKN_PMH_ILKN_RESETr : ILKN_PML_ILKN_RESETr;
            reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4);

            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val));
            field = (offset & 1) ? ILKN_RX_1_PORT_RSTNf : ILKN_RX_0_PORT_RSTNf;
            soc_reg_field_set(unit, reg, &reg_val, field, 1);
            field = (offset & 1) ? ILKN_TX_1_PORT_RSTNf : ILKN_TX_0_PORT_RSTNf;
            soc_reg_field_set(unit, reg, &reg_val, field, 1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_enable_get(int unit, soc_port_t port, int* enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_speed_set(int unit, soc_port_t port, int speed)
{
    portmod_port_interface_config_t config;
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_IF_ERR_EXIT(_soc_jer_portmod_speed_to_if_config(unit, port, speed, &config));
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_set(unit, port, &config, FALSE));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_speed_get(int unit, soc_port_t port, int* speed)
{
    portmod_port_interface_config_t config;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config));

    (*speed) = config.speed; 
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Only store in DB, actul interface will be updated when calling speed_set */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_set(unit, port, intf));
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf)
{
    portmod_port_interface_config_t config;
    SOCDNX_INIT_FUNC_DEFS;

    /* Get actual configured value */
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config));

    (*intf) = config.interface; 
    
exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_portmod_port_link_state_get(int unit, soc_port_t port, int clear_status, int *is_link_up, int *is_latch_down) 
{
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);

exit:
    SOCDNX_FUNC_RETURN

}

int
soc_jer_portmod_is_supported_encap_get(int unit, int mode, int* is_supported) 
{
    SOCDNX_INIT_FUNC_DEFS;

    switch(mode) {
        case SOC_ENCAP_HIGIG2:
        case SOC_ENCAP_IEEE:
        case SOC_ENCAP_SOP_ONLY:
            (*is_supported) = 1;
            break;

        default:
            (*is_supported) = 0;
            break;

    }

    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_autoneg_set(int unit, soc_port_t port, int enable) 
{
    phymod_autoneg_control_t an;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_autoneg_get(unit, port, &an));
    an.enable = enable;
    SOCDNX_IF_ERR_EXIT(portmod_port_autoneg_set(unit, port, &an));

exit:
    SOCDNX_FUNC_RETURN;

}

int
soc_jer_portmod_autoneg_get(int unit, soc_port_t port, int* enable) 
{
    phymod_autoneg_control_t an;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_autoneg_get(unit, port, &an));
    (*enable) = an.enable;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_pfc_refresh_set(int unit, soc_port_t port, int value)
{
    portmod_pfc_control_t control;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &control));
    if(value == 0) { /* disable PFC refresh */
        control.refresh_timer = -1;
    } else {
        control.refresh_timer = value;
    }
    SOCDNX_IF_ERR_EXIT(portmod_port_pfc_control_set(unit, port, &control));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_pfc_refresh_get(int unit, soc_port_t port, int* value)
{
    portmod_pfc_control_t control;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &control));
    if(control.refresh_timer == -1) { /* disable PFC refresh */
        *value = 0;
    } else {
        *value = control.refresh_timer;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_phy_reset(int unit, soc_port_t port) 
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = portmod_port_reset_set(unit, port, 0, 0, 0); 
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);

exit:
    SOCDNX_FUNC_RETURN; 
}

int 
soc_jer_port_mac_sa_set(int unit, int port, sal_mac_addr_t mac_sa)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_tx_mac_sa_set(unit, port, mac_sa));
    SOCDNX_IF_ERR_EXIT(portmod_port_rx_mac_sa_set(unit, port, mac_sa));

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_port_mac_sa_get(int unit, int port, sal_mac_addr_t mac_sa)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_rx_mac_sa_get(unit, port, mac_sa));

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int enable_ilkn_fields[] = {
    ENABLE_PORT_0f, 
    ENABLE_PORT_1f, 
    ENABLE_PORT_2f, 
    ENABLE_PORT_3f, 
    ENABLE_PORT_4f, 
    ENABLE_PORT_5f
};

STATIC int tdm_data_hrf_fields[] = {
    ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_0f,
    ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_1f, 
    ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_2f, 
    ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_3f, 
    ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_4f, 
    ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_5f
};

STATIC int nif_pmh_ilkn_rx_fields[] = {
    PMH_0_ILKN_RX_RSTNf,
    PMH_1_ILKN_RX_RSTNf,
    PMH_2_ILKN_RX_RSTNf,
    PMH_3_ILKN_RX_RSTNf,
    PMH_4_ILKN_RX_RSTNf,
    PMH_5_ILKN_RX_RSTNf
};

STATIC int nif_pmh_ilkn_tx_fields[] = {
    PMH_0_ILKN_TX_RSTNf,
    PMH_1_ILKN_TX_RSTNf,
    PMH_2_ILKN_TX_RSTNf,
    PMH_3_ILKN_TX_RSTNf,
    PMH_4_ILKN_TX_RSTNf,
    PMH_5_ILKN_TX_RSTNf
};

STATIC int nif_pml_ilkn_rx_fields[] = {
    PML_0_ILKN_RX_RSTNf,
    PML_1_ILKN_RX_RSTNf,
    PML_2_ILKN_RX_RSTNf,
    PML_3_ILKN_RX_RSTNf,
    PML_4_ILKN_RX_RSTNf,
    PML_5_ILKN_RX_RSTNf
};

STATIC int nif_pml_ilkn_tx_fields[] = {
    PML_0_ILKN_TX_RSTNf,
    PML_1_ILKN_TX_RSTNf,
    PML_2_ILKN_TX_RSTNf,
    PML_3_ILKN_TX_RSTNf,
    PML_4_ILKN_TX_RSTNf,
    PML_5_ILKN_TX_RSTNf
};

STATIC int
soc_jer_port_open_ilkn_path(int unit, int port) {

    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    soc_reg_above_64_val_t channel_tdm_bmp;
    SHR_BITDCLNAME(hrf_sch_map, 32);
    soc_port_t reg_port;
    soc_reg_t reg;
    soc_field_t field;
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val;
    uint32 reg_val, offset, retrans_multiply_tx, channel, prio;
    uint32 is_master, base_index, num_lanes, egr_if, fld_val[1];
    int mubits_tx_polarity, mubits_rx_polarity, fc_tx_polarity, fc_rx_polarity, index, core;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    int *nif_pm_field_ptr, i, start;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
    /* offset = 0/1 -> ILKN_PMH, offset = 2/3 -> ILKN_PML0, offset = 4/5 -> ILKN_PML1 */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
        
    /* ILKN0/1 -> port=REG_PORT_ANY, ILKN2/3 -> port=0, ILKN4/5 -> port=1 */
    reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4); 

    if (is_master) {
        SOC_PBMP_PORT_ADD(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, port);

        /* Wake up relevant wrapper */
        field = (offset & 1) ? ILKN_1_PORT_RSTNf : ILKN_0_PORT_RSTNf;
        reg = (offset < 2 ) ? ILKN_PMH_ILKN_RESETr : ILKN_PML_ILKN_RESETr;

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val));
        soc_reg_field_set(unit, reg, &reg_val, field, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));
        /* Fabric mux - if PML1 */
        
        if (SOC_IS_QMX(unit) && offset >= 4 ) {
            SOCDNX_IF_ERR_EXIT(READ_ILKN_PML_ILKN_OVER_FABRICr(unit, 1, &reg_val));
            soc_reg_field_set(unit, ILKN_PML_ILKN_OVER_FABRICr, &reg_val, ILKN_OVER_FABRICf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_ILKN_PML_ILKN_OVER_FABRICr(unit, 1, reg_val));
        }
        /* Enable ILKN in NBIH */
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKENr(unit, &reg_val)); 
        soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, enable_ilkn_fields[offset], 1);
        if (((offset & 1) == 0) && num_lanes <= 12 ) {
              soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, enable_ilkn_fields[offset + 1], 1);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKENr(unit, reg_val));
    }

    /* Config ILKN ports in NBI */
    reg = (offset < 2) ? NBIH_HRF_TX_CONFIG_HRFr : NBIL_HRF_TX_CONFIG_HRFr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, (offset & 1), &reg_val)); 
    soc_reg_field_set(unit, reg, &reg_val, HRF_TX_NUM_CREDITS_TO_EGQ_HRF_Nf, ilkn_config->retransmit.enable_tx ? 0x400 : 0x200);
    if (ilkn_config->retransmit.enable_tx) {                                                 
        soc_reg_field_set(unit, reg, &reg_val, HRF_TX_USE_EXTENDED_MEM_HRF_Nf, 1); 
    }
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, (offset & 1), reg_val));

    switch(ilkn_config->retransmit.nof_seq_number_repetitions_tx) {
    case 1:
        retrans_multiply_tx = 0;
        break;
    case 2:
        retrans_multiply_tx = 1;
        break;
    case 4:
        retrans_multiply_tx = 2;
        break;
    case 8:
        retrans_multiply_tx = 3;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid nof_seq_number_repetitions value %d"),ilkn_config->retransmit.nof_seq_number_repetitions_rx));
        break;
    }
    reg = (offset < 2) ? NBIH_ILKN_TX_RETRANSMIT_CONFIG_HRFr : NBIL_ILKN_TX_RETRANSMIT_CONFIG_HRFr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, (offset & 1), &reg_val)); 
    soc_reg_field_set(unit, reg, &reg_val, ILKN_TX_RETRANS_ENABLE_HRF_Nf, ilkn_config->retransmit.enable_tx);
    soc_reg_field_set(unit, reg, &reg_val, ILKN_TX_RETRANS_NUM_ENTRIES_TO_SAVE_HRF_Nf, ilkn_config->retransmit.buffer_size_entries);
    soc_reg_field_set(unit, reg, &reg_val, ILKN_TX_RETRANS_MULTIPLY_HRF_Nf, retrans_multiply_tx);
    soc_reg_field_set(unit, reg, &reg_val, ILKN_TX_RETRANS_WAIT_FOR_SEQ_NUM_CHANGE_HRF_Nf, ilkn_config->retransmit.tx_wait_for_seq_num_change);
    soc_reg_field_set(unit, reg, &reg_val, ILKN_TX_RETRANS_IGNORE_REQ_WHEN_ALMOST_EMPTY_HRF_Nf, ilkn_config->retransmit.tx_ignore_requests_when_fifo_almost_empty);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, (offset & 1), reg_val));

    /* Configure HRFs and TDM in NBI (for every active ILKN core) */
    /* if ilkn 0/1 configure only nbih, else configure both nbil(with 0/1) and nbih(with 2-5) */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
    if (num_lanes <= 4) {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, &reg_val)); 
        soc_reg_field_set(unit, NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, &reg_val, tdm_data_hrf_fields[offset], 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_val));

        if (offset > 1) {
            field = (offset & 1) ? ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_1f : ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_0f;
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_port, &reg_val)); 
            soc_reg_field_set(unit, NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, &reg_val, field, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_port, reg_val));
        }
    }

    /* 4 HRF's per core : two Data and 2 TDM. if TDM, can configure only pipe field. */
    prio = 1; /* how to configure ?? */
    index = 0;
    SHR_BITCLR_RANGE(hrf_sch_map, 0, 32);
    switch (offset) {
    case 0:
        reg = NBIH_RX_SCH_CONFIG_HRFr;
        index = offset;
        SHR_BITSET_RANGE(hrf_sch_map, 0, 3);
        SHR_BITSET_RANGE(hrf_sch_map, 18, 3);
        break;
    case 1:
        reg = NBIH_RX_SCH_CONFIG_HRFr;
        index = offset;
        SHR_BITSET_RANGE(hrf_sch_map, 3, 3);
        SHR_BITSET_RANGE(hrf_sch_map, 20, 2);
        break;
    case 2:
        reg = NBIH_RX_SCH_CONFIG_HRF_4r;
        SHR_BITSET_RANGE(hrf_sch_map, 6, 3);
        SHR_BITSET_RANGE(hrf_sch_map, 22, 2);
        break;
    case 3:
        reg = NBIH_RX_SCH_CONFIG_HRF_5r;
        SHR_BITSET_RANGE(hrf_sch_map, 9, 3);
        SHR_BITSET_RANGE(hrf_sch_map, 24, 2);
        break;
    case 4:
        reg = NBIH_RX_SCH_CONFIG_HRF_8r;
        SHR_BITSET_RANGE(hrf_sch_map, 12, 3);
        SHR_BITSET_RANGE(hrf_sch_map, 26, 2);
        break;
    case 5:
        reg = NBIH_RX_SCH_CONFIG_HRF_9r;
        SHR_BITSET_RANGE(hrf_sch_map, 15, 3);
        SHR_BITSET_RANGE(hrf_sch_map, 28, 2);
        break;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, REG_PORT_ANY, index, &reg64_val)); 
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_PIPE_Nf, (offset & 1));
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_PRIORITY_HRF_Nf, prio);
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_SCH_MAP_HRF_Nf, *hrf_sch_map); 
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg, REG_PORT_ANY, index, reg64_val));

    /* configure tdm if needed*/
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_get(unit, port, &hdr_type));
    if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel)); 
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ILKN_CHANNEL_IS_TDM_PORTr(unit, (offset & 1), reg_above_64_val));
        soc_reg_above_64_field_get(unit, NBIH_ILKN_CHANNEL_IS_TDM_PORTr, reg_above_64_val, ILKN_CHANNEL_IS_TDM_PORTf, channel_tdm_bmp);
        SHR_BITSET(channel_tdm_bmp, channel);
        soc_reg_above_64_field_set(unit, NBIH_ILKN_CHANNEL_IS_TDM_PORTr, reg_above_64_val, ILKN_CHANNEL_IS_TDM_PORTf, channel_tdm_bmp);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ILKN_CHANNEL_IS_TDM_PORTr(unit, (offset & 1), reg_above_64_val));
    }

    reg = (offset < 2) ? NBIH_HRF_RX_CONFIG_HRFr : NBIL_HRF_RX_CONFIG_HRFr;
    /* HRF 0-1 -> data traffic, HRF 2-3 -> tdm traffic */
    base_index = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? 2 : 0;
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, reg_port, base_index + (offset & 1), &reg64_val)); 
    soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_EN_HRF_Nf, 1);
    soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_TH_HRF_Nf, 0x8); /*default val for now*/
    soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, ilkn_config->interleaved ? 0 : 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg, reg_port, base_index + (offset & 1), reg64_val));

    /* set_ilkn_tx_hrf_rstn - Make sure HRF is not enabled before it is configured */
    reg = (offset < 2) ? NBIH_HRF_RESETr : NBIL_HRF_RESETr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val)); 
    field = (offset & 1) ? HRF_TX_1_CONTROLLER_RSTNf : HRF_TX_0_CONTROLLER_RSTNf;
    soc_reg_field_set(unit, reg, &reg_val, field, 1);
    field = (offset & 1) ? HRF_RX_1_CONTROLLER_RSTNf : HRF_RX_0_CONTROLLER_RSTNf;
    soc_reg_field_set(unit, reg, &reg_val, field, 1);
    if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
        field = (offset & 1) ? HRF_RX_3_CONTROLLER_RSTNf : HRF_RX_2_CONTROLLER_RSTNf;
        soc_reg_field_set(unit, reg, &reg_val, field, 0); 
    }
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));

    /* All HRFs indications in NBIF for SCH */
    /* set_ilkn_rx_hrf_en */
    switch (offset) {
    case 0:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_2f : RX_HRF_ENABLE_HRF_0f;
        break;
    case 1:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_3f : RX_HRF_ENABLE_HRF_1f;
        break;
    case 2:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_6f : RX_HRF_ENABLE_HRF_4f;
        break;
    case 3:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_7f : RX_HRF_ENABLE_HRF_5f;
        break;
    case 4:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_10f : RX_HRF_ENABLE_HRF_8f;
        break;
    case 5:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_11f : RX_HRF_ENABLE_HRF_9f;
        break;
    }
    SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKEN_HRFr(unit, &reg_val)); 
    soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKEN_HRFr, &reg_val, field, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKEN_HRFr(unit, reg_val));

    /* set_enable_ilkn_port */
    reg = (offset < 2) ? ILKN_PMH_ENABLE_INTERLAKENr : ILKN_PML_ENABLE_INTERLAKENr;
    field = (offset & 1) ? ENABLE_PORT_1f : ENABLE_PORT_0f;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val));
    soc_reg_field_set(unit, reg, &reg_val, field, 1);
    soc_reg_field_set(unit, reg, &reg_val, ENABLE_CORE_CLOCKf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));

    /* set_fc_ilkn_cfg */
    mubits_tx_polarity = ilkn_config->mubits_tx_polarity;
    mubits_rx_polarity = ilkn_config->mubits_rx_polarity;
    fc_tx_polarity = ilkn_config->fc_tx_polarity;
    fc_rx_polarity = ilkn_config->fc_tx_polarity;
    if (mubits_tx_polarity || mubits_rx_polarity || fc_tx_polarity || fc_rx_polarity) {
        reg = (offset < 2) ? ILKN_PMH_ILKN_INVERT_POLARITY_SIGNALSr : ILKN_PML_ILKN_INVERT_POLARITY_SIGNALSr; 
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, (offset & 1), &reg_val));
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_RX_MUBITS_POLARITYf, mubits_rx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_TX_MUBITS_POLARITYf, mubits_tx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_RECEIVED_FC_POLARITYf, fc_rx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_TX_FC_POLARITYf, fc_tx_polarity);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, (offset & 1), reg_val));
    }

    start = (offset & 1) ? 3 : 0;

    reg = (offset < 2) ? NBIH_NIF_PM_ILKN_RX_RSTNr : NBIL_NIF_PM_ILKN_RX_RSTNr;
    nif_pm_field_ptr = (offset < 2) ? &nif_pmh_ilkn_rx_fields[0] : &nif_pml_ilkn_rx_fields[0];
    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val)); 
    for (i = 0; (i < MAX_NUM_OF_PMS_IN_ILKN) && (num_lanes > NUM_OF_LANES_IN_PM * i) ; ++i) {
        soc_reg_field_set(unit, reg, &reg_val, nif_pm_field_ptr[start + i], 0xf); 
    }
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));

    reg = (offset < 2) ? NBIH_NIF_PM_ILKN_TX_RSTNr : NBIL_NIF_PM_ILKN_TX_RSTNr;
    nif_pm_field_ptr = (offset < 2) ? &nif_pmh_ilkn_tx_fields[0] : &nif_pml_ilkn_tx_fields[0];
    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val)); 
    for (i = 0; (i < MAX_NUM_OF_PMS_IN_ILKN) && (num_lanes > NUM_OF_LANES_IN_PM * i) ; ++i) {
        soc_reg_field_set(unit, reg, &reg_val, nif_pm_field_ptr[start + i], 0xf);
    }
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));

    /* set_tx_retransmit_enable */
    reg = (offset < 2) ? ILKN_PMH_ILKN_TX_CONFr : ILKN_PML_ILKN_TX_CONFr;
    field = (offset & 1) ? TX_1_RETRANS_ENf : TX_0_RETRANS_ENf;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, reg_above_64_val)); 
    soc_reg_above_64_field32_set(unit, reg, &reg_val, field, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, reg_above_64_val));

    /* set fragmentation */
    if (is_master) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_egr_if_get(unit, port, &egr_if));

        SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

        *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);        
        SHR_BITCLR(fld_val, egr_if);
        soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);

        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

        /* handle ILKN dedicated mode */
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
            if (IS_TDM_PORT(unit, port)) {
                egr_if++;
            } else {
                egr_if--;
            }

            SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

            *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);        
            SHR_BITCLR(fld_val, egr_if);
            soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);

            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_open_nbi_path(int unit, int port) {

    const soc_field_t fields[4] = {TX_STOP_DATA_TO_PORT_MACRO_MLF_0_QMLF_Nf, TX_STOP_DATA_TO_PORT_MACRO_MLF_1_QMLF_Nf, TX_STOP_DATA_TO_PORT_MACRO_MLF_2_QMLF_Nf, TX_STOP_DATA_TO_PORT_MACRO_MLF_3_QMLF_Nf};
    int index = 0, reg_port, qmlf_index, qmlf_index_factor, core, qmlf_index_internal;
    uint32 first_lane; /*1-72*/
    uint64 reg64_val;
    uint32 flags, first_phy, reg_val, mode, phys_count;
    soc_reg_t reg;
    soc_pbmp_t phys, lanes;
    soc_port_t lane_i;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, first_phy, &first_lane));
    --first_phy; /* first_phy returned is one-based */
    --first_lane; /* first_lane returned is one-based */

    reg_port = (first_phy < SOC_JERICHO_NOF_PORTS_NBIH) ? REG_PORT_ANY : (first_phy / (SOC_JERICHO_NOF_PORTS_NBIH + SOC_JERICHO_NOF_PORTS_EACH_NBIL));
    index = (first_lane % SOC_JERICHO_NOF_PORTS_NBIH) / NUM_OF_LANES_IN_PM;

    /* take MLF out of reset - if port is ELK, SIF or ILKN- no need to take MLF ooo */
    if (!SOC_PORT_IS_ELK_INTERFACE(flags) && !SOC_PORT_IS_STAT_INTERFACE(flags)) {
        reg = (first_phy < SOC_JERICHO_NOF_PORTS_NBIH) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;

        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, reg_port, index, &reg64_val));
        SOC_PBMP_ITER(lanes, lane_i) {
            soc_reg64_field32_set(unit, reg, &reg64_val, fields[(lane_i - 1) % 4], 0);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg, reg_port, index, reg64_val));
    }

    /* Port Mode */
    qmlf_index_factor = (first_phy < 24) ? 0 : ((first_phy < (SOC_JERICHO_NOF_PORTS_NBIH + SOC_JERICHO_NOF_PORTS_EACH_NBIL)) ? 1 : 2); 
    qmlf_index = (qmlf_index_factor * 6) + index;
    qmlf_index_internal = qmlf_index % 6;

    SOC_PBMP_COUNT(phys, phys_count);
    if(phys_count == 1) {
        mode = 2; /* Four Ports */
    } else if (phys_count == 2){
        mode = 1; /* Two ports */
    } else {
        mode = 0; /* One Port */
    }

    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_QMLF_CONFIGr(unit, qmlf_index, &reg_val));
    soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, &reg_val, RX_PORT_MODE_QMLF_Nf, mode);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_QMLF_CONFIGr(unit, qmlf_index, reg_val));

    if(first_phy >= SOC_JERICHO_NOF_PORTS_NBIH) {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_QMLF_CONFIGr(unit, reg_port, qmlf_index_internal, &reg_val));
        soc_reg_field_set(unit, NBIL_RX_QMLF_CONFIGr, &reg_val, RX_PORT_MODE_QMLF_Nf, (interface_type == SOC_PORT_IF_QSGMII ? 3 : mode));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_QMLF_CONFIGr(unit, reg_port, qmlf_index_internal, reg_val));
    }

    reg = (first_phy < SOC_JERICHO_NOF_PORTS_NBIH) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, qmlf_index_internal, &reg64_val));
    soc_reg64_field32_set(unit, reg, &reg64_val, TX_PORT_MODE_QMLF_Nf, (interface_type == SOC_PORT_IF_QSGMII ? 3 : mode));
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, qmlf_index_internal, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}



int 
soc_jer_port_close_ilkn_path(int unit, int port) {

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}


int 
soc_jer_port_close_nbi_path(int unit, int port) {

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}



STATIC int
_soc_jer_nif_sif_set(int unit, uint32 first_phy)
{
    int port_num, pm_select = 0, nbil_index;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (first_phy == JER_NIF_PHY_SIF_PORT_0_NBIH || first_phy == JER_NIF_PHY_SIF_PORT_1_NBIH)
    {
        port_num = (first_phy == JER_NIF_PHY_SIF_PORT_0_NBIH) ? 0:1;
        SOCDNX_IF_ERR_EXIT(READ_NBIH_SIF_CFGr(unit, port_num ? 0:1, &reg_val));
        soc_reg_field_set(unit, NBIH_SIF_CFGr, &reg_val, SIF_PORT_N_ENf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_SIF_CFGr(unit, port_num, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIH_ADDITIONAL_RESETSr(unit, &reg_val));
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ADDITIONAL_RESETSr(unit, reg_val));
    }
    else
    {
        if (first_phy == JER_NIF_PHY_SIF_PORT_0_NBIL0 || first_phy == JER_NIF_PHY_SIF_PORT_1_NIBL0_PM4 || first_phy == JER_NIF_PHY_SIF_PORT_1_NBIL0_PM5)
        {
            nbil_index = 0;
            port_num = (first_phy == JER_NIF_PHY_SIF_PORT_0_NBIL0) ? 0:1;
            if (port_num == 1 && first_phy == JER_NIF_PHY_SIF_PORT_1_NIBL0_PM4)
            {
                pm_select= 1;
            }
        }
        else if (first_phy == JER_NIF_PHY_SIF_PORT_0_NBIL1 || first_phy == JER_NIF_PHY_SIF_PORT_1_NBIL1_PM4 || first_phy == JER_NIF_PHY_SIF_PORT_1_NBIL1_PM5) 
        {
            nbil_index = 1;
            port_num = (first_phy == JER_NIF_PHY_SIF_PORT_0_NBIL1) ? 0:1;
            if (port_num == 1 && first_phy == JER_NIF_PHY_SIF_PORT_1_NBIL1_PM4)
            {
                pm_select = 1;
            }
        }
        else
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid phy port configured as statistics interface"), first_phy));
        }

        SOCDNX_IF_ERR_EXIT(READ_NBIL_SIF_CFGr(unit, nbil_index, port_num, &reg_val));
        soc_reg_field_set(unit, NBIL_SIF_CFGr, &reg_val, SIF_PORT_N_ENf, 1);
        soc_reg_field_set(unit, NBIL_SIF_CFGr, &reg_val, SIF_PORT_N_PM_SELf, pm_select);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_SIF_CFGr(unit, nbil_index, port_num, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIL_ADDITIONAL_RESETSr(unit, nbil_index, &reg_val));
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, nbil_index, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_open_path(int unit, soc_port_t port) {

    soc_port_if_t interface_type;
    int index = 0, qmlf_index, qmlf_index_factor, core;
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val, nbil_ports_rstn;
    uint32 first_phy, first_lane, reg_val, flags;
    soc_pbmp_t phys;
    soc_port_t port_i;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    uint32 rx_req_high_en = 0, rx_req_tdm_en = 0; 
    uint32 nbih_ports_rstn = 0;
    soc_reg_above_64_val_t rx_req_low_en;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    if (SOC_PORT_IF_ILKN == interface_type) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_open_ilkn_path(unit, port));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_open_nbi_path(unit, port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, first_phy, &first_lane));
    --first_phy; /* first_phy returned is one-based */
    --first_lane; /* first_lane returned is one-based */

    if (first_phy < SOC_JERICHO_NOF_PORTS_NBIH) {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_PORTS_SRSTNr(unit, &reg_val));
        nbih_ports_rstn = soc_reg_field_get(unit, NBIH_RX_PORTS_SRSTNr, reg_val, RX_PORTS_SRSTNf);
        SOC_PBMP_ITER(phys, port_i) {
            SHR_BITSET(&nbih_ports_rstn, port_i - 1);/* make port_i zero based */
        }
        soc_reg_field_set(unit, NBIH_RX_PORTS_SRSTNr, &reg_val, RX_PORTS_SRSTNf, nbih_ports_rstn);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_PORTS_SRSTNr(unit, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIH_TX_PORTS_SRSTNr(unit, &reg_val));
        nbih_ports_rstn = soc_reg_field_get(unit, NBIH_TX_PORTS_SRSTNr, reg_val, TX_PORTS_SRSTNf);
        SOC_PBMP_ITER(phys, port_i) {
            SHR_BITSET(&nbih_ports_rstn, port_i - 1);
        }
        soc_reg_field_set(unit, NBIH_TX_PORTS_SRSTNr, &reg_val, TX_PORTS_SRSTNf, nbih_ports_rstn);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_TX_PORTS_SRSTNr(unit, reg_val));
    } else {
        index = first_phy / (SOC_JERICHO_NOF_PORTS_NBIH + SOC_JERICHO_NOF_PORTS_EACH_NBIL);

        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_PORTS_SRSTNr(unit, index, &reg64_val));
        nbil_ports_rstn = soc_reg64_field_get(unit, NBIL_RX_PORTS_SRSTNr, reg64_val, RX_PORTS_SRSTNf);
        SOC_PBMP_ITER(phys, port_i) {
            COMPILER_64_BITSET(nbil_ports_rstn, port_i - SOC_JERICHO_NOF_PORTS_NBIH - (index*SOC_JERICHO_NOF_PORTS_EACH_NBIL) - 1);
        }
        soc_reg64_field_set(unit, NBIL_RX_PORTS_SRSTNr, &reg64_val, RX_PORTS_SRSTNf, nbil_ports_rstn);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_PORTS_SRSTNr(unit, index, reg64_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIL_TX_PORTS_SRSTNr(unit, index, &reg64_val));
        nbil_ports_rstn = soc_reg64_field_get(unit, NBIL_TX_PORTS_SRSTNr, reg64_val, TX_PORTS_SRSTNf);
        SOC_PBMP_ITER(phys, port_i) {
            COMPILER_64_BITSET(nbil_ports_rstn, port_i - SOC_JERICHO_NOF_PORTS_NBIH - (index*SOC_JERICHO_NOF_PORTS_EACH_NBIL) - 1);
        }
        soc_reg64_field_set(unit, NBIL_TX_PORTS_SRSTNr, &reg64_val, TX_PORTS_SRSTNf, nbil_ports_rstn);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_TX_PORTS_SRSTNr(unit, index, reg64_val));
    }

    /* Port Mode */
    index = (first_lane % SOC_JERICHO_NOF_PORTS_NBIH) / NUM_OF_LANES_IN_PM;
    qmlf_index_factor = (first_phy < 24) ? 0 : ((first_phy < (SOC_JERICHO_NOF_PORTS_NBIH + SOC_JERICHO_NOF_PORTS_EACH_NBIL)) ? 1 : 2); 
    qmlf_index = (qmlf_index_factor * 6) + index;

    /* statistics interface */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
        SOCDNX_IF_ERR_EXIT(_soc_jer_nif_sif_set(unit, first_phy));
     }
    
    /* ReqEn register */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_get(unit, port, &hdr_type));
    if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
        if(core == 0) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg_val));
            rx_req_tdm_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, reg_val, RX_REQ_PIPE_0_TDM_ENf);
            SHR_BITSET(&rx_req_tdm_en, qmlf_index);
            soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, &reg_val, RX_REQ_PIPE_0_TDM_ENf, rx_req_tdm_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg_val));
        } else {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_1_TDM_ENr(unit, &reg_val));
            rx_req_tdm_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_1_TDM_ENr, reg_val, RX_REQ_PIPE_1_TDM_ENf);
            SHR_BITSET(&rx_req_tdm_en, qmlf_index);
            soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_1_TDM_ENr, &reg_val, RX_REQ_PIPE_1_TDM_ENf, rx_req_tdm_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_1_TDM_ENr(unit, reg_val));
        }

    } else {
        if(core == 0) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
            soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
            SHR_BITSET(rx_req_low_en, qmlf_index);
            SHR_BITSET(rx_req_low_en, qmlf_index + 32);
            SHR_BITSET(rx_req_low_en, qmlf_index + 64);
            SHR_BITSET(rx_req_low_en, qmlf_index + 96);
            soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));

            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg_val));
            rx_req_high_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, reg_val, RX_REQ_PIPE_0_HIGH_ENf);
            SHR_BITOR_RANGE(&rx_req_high_en, rx_req_low_en, 0, 32, &rx_req_high_en);
            soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, &reg_val, RX_REQ_PIPE_0_HIGH_ENf, rx_req_high_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg_val));

            /* just to be on the safe side - clear this bit from opposite pipe */
            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_1_LOW_ENr(unit, reg_above_64_val));
            soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_1_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_1_LOW_ENf, rx_req_low_en);
            SHR_BITCLR(rx_req_low_en, qmlf_index);
            SHR_BITCLR(rx_req_low_en, qmlf_index + 32);
            SHR_BITCLR(rx_req_low_en, qmlf_index + 64);
            SHR_BITCLR(rx_req_low_en, qmlf_index + 96);
            soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_1_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_1_LOW_ENf, rx_req_low_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_1_LOW_ENr(unit, reg_above_64_val));

            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_1_HIGH_ENr(unit, &reg_val));
            rx_req_high_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_1_HIGH_ENr, reg_val, RX_REQ_PIPE_1_HIGH_ENf);
            SHR_BITCLR(&rx_req_high_en, qmlf_index);
            soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_1_HIGH_ENr, &reg_val, RX_REQ_PIPE_1_HIGH_ENf, rx_req_high_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_1_HIGH_ENr(unit, reg_val));

        } else {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_1_LOW_ENr(unit, reg_above_64_val));
            soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_1_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_1_LOW_ENf, rx_req_low_en);
            SHR_BITSET(rx_req_low_en, qmlf_index);
            SHR_BITSET(rx_req_low_en, qmlf_index + 32);
            SHR_BITSET(rx_req_low_en, qmlf_index + 64);
            SHR_BITSET(rx_req_low_en, qmlf_index + 96);
            soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_1_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_1_LOW_ENf, rx_req_low_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_1_LOW_ENr(unit, reg_above_64_val));

            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_1_HIGH_ENr(unit, &reg_val));
            rx_req_high_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, reg_val, RX_REQ_PIPE_0_HIGH_ENf);
            SHR_BITOR_RANGE(&rx_req_high_en, rx_req_low_en, 0, 32, &rx_req_high_en);
            soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_1_HIGH_ENr, &reg_val, RX_REQ_PIPE_1_HIGH_ENf, rx_req_high_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_1_HIGH_ENr(unit, reg_val));

            /* just to be on the safe side - clear this bit from opposite pipe */
            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
            soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
            SHR_BITCLR(rx_req_low_en, qmlf_index);
            SHR_BITCLR(rx_req_low_en, qmlf_index + 32);
            SHR_BITCLR(rx_req_low_en, qmlf_index + 64);
            SHR_BITCLR(rx_req_low_en, qmlf_index + 96);
            soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));

            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg_val));
            rx_req_high_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, reg_val, RX_REQ_PIPE_0_HIGH_ENf);
            SHR_BITCLR(&rx_req_high_en, qmlf_index); 
            soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, &reg_val, RX_REQ_PIPE_0_HIGH_ENf, rx_req_high_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;   
}

int
soc_jer_port_close_path(int unit, int port) {

    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    if (SOC_PORT_IF_ILKN == interface_type) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_close_ilkn_path(unit, port));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_close_nbi_path(unit, port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int 
soc_jer_port_pll_type_get(int unit, soc_port_t port, SOC_JER_NIF_PLL_TYPE *pll_type)
{
    uint32 first_phy_port;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port /*one based*/));

    first_phy_port--; /* zero based */

    if (first_phy_port <= SOC_JER_NIF_PLL_TYPE_PMH_LAST_PHY_LANE) {
        *pll_type = SOC_JER_NIF_PLL_TYPE_PMH;
    } else if (first_phy_port > SOC_JER_NIF_PLL_TYPE_PMH_LAST_PHY_LANE && first_phy_port <= SOC_JER_NIF_PLL_TYPE_PML0_LAST_PHY_LANE) {
        *pll_type = SOC_JER_NIF_PLL_TYPE_PML0;
    } else if (first_phy_port > SOC_JER_NIF_PLL_TYPE_PML0_LAST_PHY_LANE && first_phy_port <= SOC_JER_NIF_PLL_TYPE_PML1_LAST_PHY_LANE) {
        *pll_type = SOC_JER_NIF_PLL_TYPE_PML1;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid phy port")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_autoneg_get(int unit, soc_port_t port, int *autoneg)
{
    int rv;
    phymod_autoneg_control_t an;
    SOCDNX_INIT_FUNC_DEFS;

    rv = portmod_port_autoneg_get(unit, port, &an);
    SOCDNX_IF_ERR_EXIT(rv);

    *autoneg = an.enable;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_duplex_set(int unit, soc_port_t port, int duplex)
{
    phymod_autoneg_control_t an;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_autoneg_get(unit, port, &an));
    an.enable = FALSE;
    SOCDNX_IF_ERR_EXIT(portmod_port_autoneg_set(unit, port, &an));

    SOCDNX_IF_ERR_EXIT(portmod_port_duplex_set(unit, port, duplex));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_fault_get(int unit, soc_port_t port, uint32 *flags)
{
    int local_fault;
    int remote_fault;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_local_fault_status_get(unit, port, &local_fault));
    SOCDNX_IF_ERR_EXIT(portmod_port_remote_fault_status_get(unit, port, &remote_fault));

    *flags = 0;
    if (remote_fault) {
        *flags |= BCM_PORT_FAULT_REMOTE;
    }
    if (local_fault) {
        *flags |= BCM_PORT_FAULT_LOCAL;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
_soc_jer_port_pm_get(int unit, uint32 phy, uint32 *pm_idx)
{
    uint32 new_phy;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, phy, &new_phy));
    *pm_idx = new_phy / NUM_OF_LANES_IN_PM;

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_jer_port_eee_pm_nof_ports_enabled(int unit, soc_port_t port, int *nof_ports_enabled)
{
    portmod_eee_t eee;
    soc_pbmp_t ports_bm;
    soc_port_t port_idx;
    uint32 first_lane, first_phy_port, last_phy_port, phy_port;
    uint32 enabled = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /* Get first phy port in pm */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port /*one based*/));
    --first_phy_port; /*zero based*/

    /* Get last phy port in pm */
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, first_phy_port, &first_lane));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add(unit, first_lane + NUM_OF_LANES_IN_PM - 1, &last_phy_port));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &ports_bm));
    SOC_PBMP_ITER(ports_bm, port_idx) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_idx, &phy_port /*one based*/));
        --phy_port; /*zero based*/
        if (phy_port >= first_phy_port && phy_port <= last_phy_port) {
            SOCDNX_IF_ERR_EXIT(portmod_port_eee_get(unit, port_idx, &eee));
            if (eee.enable) {
                ++enabled;
            }
        }
    }

    *nof_ports_enabled = enabled;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_eee_enable_set(int unit, soc_port_t port, uint32 value)
{
    portmod_eee_t eee;
    SOC_JER_NIF_PLL_TYPE pll_type;
    uint32 first_phy_port, is_enabled, pm_idx, pml_idx;
    uint32 rval = 0;
    int nof_ports_enabled;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port /*one based*/));
    --first_phy_port; /*zero based*/

    SOCDNX_IF_ERR_EXIT(soc_jer_port_pll_type_get(unit, port, &pll_type));

    SOCDNX_IF_ERR_EXIT(_soc_jer_port_pm_get(unit, first_phy_port, &pm_idx));

    /* Get nof ports that are eee enabled in pm */
    SOCDNX_IF_ERR_EXIT(_soc_jer_port_eee_pm_nof_ports_enabled(unit, port, &nof_ports_enabled));
    SOCDNX_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    is_enabled = eee.enable;

    /* Enable NBI EEE */
    if (pll_type == SOC_JER_NIF_PLL_TYPE_PMH) {
        if (value && (0 == nof_ports_enabled)) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_CFGr(unit, pm_idx, &rval));
            soc_reg_field_set(unit, NBIH_NIF_PM_CFGr, &rval, PMH_NEEE_PD_ENf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_CFGr(unit, pm_idx, rval));
        } else if ((0 == value) && (1 == nof_ports_enabled) && is_enabled) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_CFGr(unit, pm_idx, &rval));
            soc_reg_field_set(unit, NBIH_NIF_PM_CFGr, &rval, PMH_NEEE_PD_ENf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_CFGr(unit, pm_idx, rval));
        } else {
            /* do nothing */
        }
    } else {
        (pll_type == SOC_JER_NIF_PLL_TYPE_PML0) ? (pml_idx = pm_idx - SOC_JERICHO_NOF_PMS_PER_NBI) :
      /* pll_type == SOC_JER_NIF_PLL_TYPE_PML1 */ (pml_idx = (pm_idx - SOC_JERICHO_NOF_PMS_PER_NBI) / 2);

        if (pml_idx < 3) {
            if (value && (0 == nof_ports_enabled)) {
                SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_CFGr(unit, port, pml_idx, &rval));
                soc_reg_field_set(unit, NBIL_NIF_PM_CFGr, &rval, PML_NEEE_PD_ENf, 1);
                SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_CFGr(unit, port, pml_idx, rval));
            } else if ((0 == value) && (1 == nof_ports_enabled) && is_enabled) {
                SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_CFGr(unit, port, pml_idx, &rval));
                soc_reg_field_set(unit, NBIL_NIF_PM_CFGr, &rval, PML_NEEE_PD_ENf, 0);
                SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_CFGr(unit, port, pml_idx, rval));
            } else {
                /* do nothing */
            }
        } else {
            soc_reg_t nbil_nif_pm_config;

            if (pml_idx == 3) {
                nbil_nif_pm_config = NBIL_NIF_PM_CFG_3r;
            } else if (pml_idx == 4) {
                nbil_nif_pm_config = NBIL_NIF_PM_CFG_4r;
            } else { /* pml_idx = 5 */
                nbil_nif_pm_config = NBIL_NIF_PM_CFG_5r;
            }

            if (value && (0 == nof_ports_enabled)) {
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, nbil_nif_pm_config, port, 0, &rval));
                soc_reg_field_set(unit, nbil_nif_pm_config, &rval, PML_NEEE_PD_ENf, 1);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, nbil_nif_pm_config, port, 0, rval));
            } else if ((0 == value) && (1 == nof_ports_enabled) && is_enabled) {
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, nbil_nif_pm_config, port, 0, &rval));
                soc_reg_field_set(unit, nbil_nif_pm_config, &rval, PML_NEEE_PD_ENf, 0);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, nbil_nif_pm_config, port, 0, rval));
            } else {
                /* do nothing */
            }
        }
    }

    /* Enable port EEE */
    SOCDNX_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    eee.enable = value;
    SOCDNX_IF_ERR_EXIT(portmod_port_eee_set(unit, port, &eee));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_eee_enable_get(int unit, soc_port_t port, uint32 *value)
{
    portmod_eee_t eee;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    *value = eee.enable;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_eee_tx_idle_time_set(int unit, soc_port_t port, uint32 value)
{
    portmod_eee_t eee;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    eee.tx_idle_time = value;
    SOCDNX_IF_ERR_EXIT(portmod_port_eee_set(unit, port, &eee));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_eee_tx_idle_time_get(int unit, soc_port_t port, uint32 *value)
{
    portmod_eee_t eee;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    *value = eee.tx_idle_time;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_eee_tx_wake_time_set(int unit, soc_port_t port, uint32 value)
{
    portmod_eee_t eee;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    eee.tx_wake_time = value;
    SOCDNX_IF_ERR_EXIT(portmod_port_eee_set(unit, port, &eee));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_eee_tx_wake_time_get(int unit, soc_port_t port, uint32 *value)
{
    portmod_eee_t eee;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    *value = eee.tx_wake_time;

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_port_nif_nof_lanes_get(int unit, soc_port_if_t interface, uint32 first_phy_port, uint32 nof_lanes_to_set, uint32 *nof_lanes)
{
    soc_port_t port_i;

    SOCDNX_INIT_FUNC_DEFS;

    *nof_lanes = 0;

    switch(interface) {
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_QSGMII:
            *nof_lanes = 1;
            break;

        case SOC_PORT_IF_CPU:
            *nof_lanes = 1;
            if (first_phy_port != 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("interface not supported on port")));
            }
            break;

        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_XLAUI2:
            *nof_lanes = 2;
            break;

        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_CAUI:
            *nof_lanes = 4;
            break;

        case SOC_PORT_IF_ILKN:
            /* if this is the first ILKN port, then num_lanes should be as provided, otherwise get the current num_lanes */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_from_interface_type_get(unit, interface, first_phy_port, &port_i));
            if (port_i != SOC_MAX_NUM_PORTS) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port_i, nof_lanes));                   
            } else {
                *nof_lanes = nof_lanes_to_set;
            }
            break;

        case SOC_PORT_IF_RCY:
        case SOC_PORT_IF_ERP:
        case SOC_PORT_IF_OLP:
        case SOC_PORT_IF_OAMP:
            *nof_lanes = 0;
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Interface %d isn't supported"), interface));
            break;
    }


    /* validity check */
    if ((interface != SOC_PORT_IF_ILKN) && (interface != SOC_PORT_IF_CPU) && (interface != SOC_PORT_IF_RCY) && 
        (interface != SOC_PORT_IF_ERP) && (interface != SOC_PORT_IF_OLP) && (interface != SOC_PORT_IF_OAMP)) {
        if ((first_phy_port - 1) % (*nof_lanes) != 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("interface not supported on port")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 jer_port_link_up_mac_update(int unit, soc_port_t port)
{
    portmod_port_update_control_t portmod_port_update_control;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_update_control_t_init(unit, &portmod_port_update_control));
    SOCDNX_IF_ERR_EXIT(portmod_port_update(unit, port, &portmod_port_update_control));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_nif_quad_to_core_validate(int unit)
{
    soc_pbmp_t ports_bm;
    soc_port_t port;
    int core;
    uint32 quad, phy_port, new_phy;
    int quads_core_array[SOC_JER_NIF_NOF_LANE_QUADS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &ports_bm));
    SOC_PBMP_ITER(ports_bm, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
        
        /* calculate quad */
        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, phy_port, &new_phy));
        new_phy = new_phy-1;
        quad = new_phy/4;

        if ((quads_core_array[quad] != -1) && (quads_core_array[quad] != core)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d is mapped to a quad on a different core"), port)); 
        } else {
            quads_core_array[quad] = core;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

