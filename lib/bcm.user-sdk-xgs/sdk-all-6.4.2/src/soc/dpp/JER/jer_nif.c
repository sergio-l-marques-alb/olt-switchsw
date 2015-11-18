/*
 * $Id: $
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
 * File: jer_egr_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <soc/portmod/portmod.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_port.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/phy/phymod_sim.h>

#define NUM_OF_LANES_IN_PM 4

/*72 NIF + 36 Fabric*/
#define SOC_MAX_JERICHO_PHYS 108

#define SOC_JERICHO_PM_4x25     6
#define SOC_JERICHO_PM_4x10     12
#define SOC_JERICHO_PM_ILKN     3
#define SOC_JERICHO_PM_FABRIC   9

STATIC void* pm4x25_user_acc[SOC_MAX_NUM_DEVICES]= {NULL};
STATIC void* pm4x10_user_acc[SOC_MAX_NUM_DEVICES]= {NULL};
STATIC void *fabric_user_acc[SOC_MAX_NUM_DEVICES] = {NULL};

#define SUB_PHYS_IN_QSGMII 4

int
soc_jer_qsgmii_offsets_add(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp) 
{
    uint32 phy;
    int qsgmii_count, skip;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*new_pbmp);

    SOC_PBMP_ITER(*pbmp, phy) 
    {
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

        SOC_PBMP_PORT_ADD(*new_pbmp, phy + skip);
    }

    SOCDNX_FUNC_RETURN;
}


int
soc_jer_qsgmii_offsets_remove(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp) 
{
    uint32 phy;
    int qsgmii_count;
    int reduce = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*new_pbmp);

    SOC_PBMP_ITER(*pbmp, phy) 
    {
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

        SOC_PBMP_PORT_ADD(*new_pbmp, phy - reduce);
    }

    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_lane_map_get(int unit, int quad, phymod_lane_map_t* lane_map)
{
    uint32 txlane_map, rxlane_map, mask, shift;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    phymod_lane_map_t_init(lane_map);

    txlane_map = soc_property_suffix_num_get(unit, quad, spn_PHY_TX_LANE_MAP, "quad", 0x3210);
    rxlane_map = soc_property_suffix_num_get(unit, quad, spn_PHY_RX_LANE_MAP, "quad", 0x3210);

    lane_map->num_of_lanes = NUM_OF_LANES_IN_PM;

    mask = 0xf;
    shift = 0;
    for(i=0 ; i<NUM_OF_LANES_IN_PM ; i++) {
        lane_map->lane_map_tx[i] =  ((txlane_map & mask) >> shift);
        lane_map->lane_map_rx[i] =  ((rxlane_map & mask) >> shift);
        mask = mask << 4;
        shift = shift + 4;
    }
    
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_portmod_pmh_init(int unit)
{
    int pm, phy, l, block_index;
    soc_dpp_config_t *dpp = NULL;
    portmod_pm_create_info_t pm_info;
    portmod_pm_identifier_t ilkn_pms[6];
    portmod_default_user_access_t* user_acc;
    soc_pbmp_t ilkn_phys;
    int is_sim, blk, first_phy;
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
    pm4x25_user_acc[unit] = user_acc;

    /* Initialize PM4x25, phys 0-23 */
    for(pm=0 ; pm<SOC_JERICHO_PM_4x25 ; pm++) {
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));

        pm_info.type = portmodDispatchTypePm4x25;
        block_index = CLP_BLOCK(unit, pm); 
        pm_info.first_blk_id = SOC_BLOCK_INFO(unit, block_index).cmic;
        ilkn_pms[pm].type = portmodDispatchTypePm4x25;
        first_phy = NUM_OF_LANES_IN_PM*pm + 1;
        ilkn_pms[pm].phy = first_phy;
        for(l=0 ; l<NUM_OF_LANES_IN_PM ; l++) {
            phy = first_phy + l;
            SOC_PBMP_PORT_ADD(pm_info.phys, phy);
            SOC_PBMP_PORT_ADD(ilkn_phys, phy);

            SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(&(pm_info.pm_specific_info.pm4x25.polarity[l])));
            pm_info.pm_specific_info.pm4x25.polarity[l].rx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_RX_POLARITY_FLIP, "phy", 0);
            pm_info.pm_specific_info.pm4x25.polarity[l].tx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_TX_POLARITY_FLIP, "phy", 0);
        }

        pm_info.pm_specific_info.pm4x25.ref_clk = dpp->jer->pll.ref_clk_pmh;

        /* Init phy access structure */
        SOCDNX_IF_ERR_EXIT(phymod_access_t_init(&pm_info.pm_specific_info.pm4x25.access));

        SOCDNX_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, &(user_acc[pm])));
        user_acc[pm].unit = unit;
        blk = CLP_BLOCK(unit, pm);
        user_acc[pm].blk_id = SOC_BLOCK2SCH(unit, blk);
        user_acc[pm].mutex = sal_mutex_create("core mutex");
        if(user_acc[pm].mutex == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate mutex for PM4x25")));
        }

        pm_info.pm_specific_info.pm4x25.access.user_acc = &(user_acc[pm]);
        pm_info.pm_specific_info.pm4x25.access.addr = pm*NUM_OF_LANES_IN_PM; /*PMH PHY addresses are 0,4,8,...*/
        pm_info.pm_specific_info.pm4x25.access.bus = NULL; /* Use default bus */
        SOCDNX_IF_ERR_EXIT(soc_physim_check_sim(unit, phymodDispatchTypeTscf, &(pm_info.pm_specific_info.pm4x25.access), &is_sim));
        if(is_sim) {
            pm_info.pm_specific_info.pm4x25.access.bus->bus_capabilities |= PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL;
        }

        /* Firmward loader */
        if(is_sim) {
            pm_info.pm_specific_info.pm4x25.fw_load_method = phymodFirmwareLoadMethodNone;
        } else {
            pm_info.pm_specific_info.pm4x25.fw_load_method = soc_property_suffix_num_get(unit, pm, spn_LOAD_FIRMWARE, "quad", phymodFirmwareLoadMethodExternal);
            pm_info.pm_specific_info.pm4x25.fw_load_method &= 0xff;
        }
        pm_info.pm_specific_info.pm4x25.external_fw_loader = NULL; /* Use default external loader */

        /* Lane mapping */
        SOCDNX_IF_ERR_EXIT(soc_jer_lane_map_get(unit, pm, &(pm_info.pm_specific_info.pm4x25.lane_map)));

        /* General PM4x25 configuration */
        pm_info.pm_specific_info.pm4x25.in_pm_12x10 = 0;

        /* Add PM to PortMod*/
        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

    }

    /* Add PMH ILKN */
    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    block_index = ILKN_PMH_BLOCK(unit);
    pm_info.first_blk_id = SOC_BLOCK_INFO(unit, block_index).cmic;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = 6;
    SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys);

    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));


exit:
    if(SOCDNX_FUNC_ERROR) {
        if(pm4x25_user_acc[unit] != NULL) {
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
    portmod_pm_identifier_t ilkn_pms[SOC_QMX_NUM_OF_PMLS][6];
    int nof_ilkn_pms[SOC_QMX_NUM_OF_PMLS]= {0 , 0};
    int first_phy;
    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);

    for(pml=0 ; pml<SOC_QMX_NUM_OF_PMLS ; pml++) {
        for(pm = 0 ; pm < 6 ; pm++)
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
            continue;
        }
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        block_index = XLP_BLOCK(unit, xlp_instance); 
        pm_info.first_blk_id = SOC_BLOCK_INFO(unit, block_index).cmic; /*qsgmii first block is also xlp*/

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
        for(l=0 ; l<NUM_OF_LANES_IN_PM ; l++) {
            phy = first_phy + l;
            SOC_PBMP_PORT_ADD(pm_info.phys, phy);
            SOC_PBMP_PORT_ADD(ilkn_phys[soc_jer_pml_info[pm].pml_instance], phy);

            SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(&(pm4x10_info->polarity[l])));
            pm4x10_info->polarity[l].rx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_RX_POLARITY_FLIP, "phy", 0);
            pm4x10_info->polarity[l].tx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_TX_POLARITY_FLIP, "phy", 0);
        }

        pm4x10_info->ref_clk = dpp->jer->pll.ref_clk_pml[soc_jer_pml_info[pm].pml_instance];

        SOCDNX_IF_ERR_EXIT(phymod_access_t_init(&pm4x10_info->access));

        SOCDNX_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, &(user_acc[pm])));
        user_acc[pm].unit = unit;
        user_acc[pm].mutex = sal_mutex_create("core mutex");
        if(user_acc[pm].mutex == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate mutex for PM4x10")));
        }

        quad = SOC_JERICHO_PM_4x25 + pm; /* PML quads are after PMH */
        pm4x10_info->fw_load_method = soc_property_suffix_num_get(unit, quad, spn_LOAD_FIRMWARE, "quad", phymodFirmwareLoadMethodExternal);
        pm4x10_info->fw_load_method &= 0xff;
        pm4x10_info->external_fw_loader = NULL; /* Use default external loader */

        pm4x10_info->access.user_acc = &(user_acc[pm]);
        pm4x10_info->access.addr = soc_jer_pml_info[pm].phy_id;
        pm4x10_info->access.bus = NULL; /* Use default bus */
        SOCDNX_IF_ERR_EXIT(soc_physim_check_sim(unit, phymodDispatchTypeTscf, &(pm_info.pm_specific_info.pm4x10.access), &is_sim));
        if(is_sim) {
            pm_info.pm_specific_info.pm4x10.access.bus->bus_capabilities |= PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL;
        }

        /* Firmward loader */
        if(is_sim) {
            pm_info.pm_specific_info.pm4x10.fw_load_method = phymodFirmwareLoadMethodNone;
        } else {
            pm_info.pm_specific_info.pm4x10.fw_load_method = soc_property_suffix_num_get(unit, pm, spn_LOAD_FIRMWARE, "quad", phymodFirmwareLoadMethodExternal);
            pm_info.pm_specific_info.pm4x10.fw_load_method &= 0xff;
        }

        SOCDNX_IF_ERR_EXIT(soc_jer_lane_map_get(unit, quad, &(pm4x10_info->lane_map)));

        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
        xlp_instance++;

    }

    /* Add PML0 ILKN */
    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    block_index = ILKN_PML_BLOCK(unit);
    pm_info.first_blk_id = SOC_BLOCK_INFO(unit, block_index).cmic;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms[0];
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = nof_ilkn_pms[0];
    SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys[0]);
    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));


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
    bus_id = (core_index / 3) * 3 + 1 ; 

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
    int rv, quads_in_fsrd;
    SOCDNX_INIT_FUNC_DEFS;

    quads_in_fsrd = SOC_DPP_DEFS_GET(unit, nof_quads_in_fsrd);

    rv = soc_dcmn_fabric_pms_add(unit, SOC_JERICHO_PM_FABRIC, FABRIC_LOGICAL_PORT_BASE(unit), 1, quads_in_fsrd, soc_jer_fabric_mdio_address_get, &fabric_user_acc[unit]);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_portmod_register_external_phys(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* TBD */

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
        {portmodDispatchTypePmOsILKN, 2},
        /*QMX extras*/
        {portmodDispatchTypePmOsILKN, 1},
        {portmodDispatchTypePm4x10, 3},
    };

    int pms_instances_arr_len = 5;
    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QMX(unit)){
        pms_instances_arr_len = 8;
    }
    SOCDNX_IF_ERR_EXIT(portmod_create(unit, SOC_MAX_NUM_PORTS, SOC_MAX_NUM_PORTS, pms_instances_arr_len, pm_types_and_instances));

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
    soc_port_t port;
    int val, rv;
    uint32 is_master;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(*ports, port) {
        if (!IS_SFI_PORT(unit, port)) {

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));

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

                /* AN config */
                val = soc_property_port_get(unit, port, spn_PORT_INIT_AUTONEG, -1);
                if(val != -1) {
                    MIIM_LOCK(unit);
                    rv = soc_jer_portmod_autoneg_set(unit, port, val);
                    MIIM_UNLOCK(unit);
                    SOCDNX_IF_ERR_EXIT(rv);
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
    uint32 is_hg, flags;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, config));

    config->speed = speed;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    config->interface = interface_type;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_get(unit, port, &is_hg));
    if (is_hg) {
        PHYMOD_INTF_MODES_HIGIG_SET(config);
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

    if (SOC_PORT_IS_SCRAMBLER(flags)) {
        PHYMOD_INTF_MODES_SCR_SET(config);
    }

    if (SOC_PORT_IS_FIBER(flags)) {
        PHYMOD_INTF_MODES_FIBER_SET(config);
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

int
soc_jer_portmod_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence)
{
    int rv, speed, counter_interval;
    soc_port_t port;
    portmod_port_add_info_t add_info;
    uint32 is_master, flags, counter_flags;
    dcmn_port_fabric_init_config_t port_config;
    soc_pbmp_t phys, counter_pbmp;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_WARM_BOOT(unit)) {
        SOC_PBMP_ASSIGN(*okay_pbmp, pbmp);
    } else {

        SOC_PBMP_CLEAR(*okay_pbmp);

        SOC_PBMP_ITER(pbmp, port) {

            if(IS_SFI_PORT(unit, port)) {
                rv = _soc_jer_port_fabric_config_get(unit, port, is_init_sequence, &port_config);
                SOCDNX_IF_ERR_EXIT(rv);
                rv = soc_dcmn_fabric_port_probe(unit, port, &port_config);
                SOCDNX_IF_ERR_EXIT(rv);
            } else {
                rv = portmod_port_add_info_t_init(unit, &add_info);
                SOCDNX_IF_ERR_EXIT(rv);

                rv = soc_port_sw_db_speed_get(unit, port, &speed);
                SOCDNX_IF_ERR_EXIT(rv);

                rv = _soc_jer_portmod_speed_to_if_config(unit, port, speed, &(add_info.interface_config));
                SOCDNX_IF_ERR_EXIT(rv);

                rv = soc_port_sw_db_phy_ports_get(unit, port, &phys);
                SOCDNX_IF_ERR_EXIT(rv);

                rv = soc_jer_qsgmii_offsets_remove(unit, &phys, &(add_info.phys));
                SOCDNX_IF_ERR_EXIT(rv);

                rv = portmod_port_add(unit, port, &add_info);
                SOCDNX_IF_ERR_EXIT(rv);

                rv = soc_port_sw_db_initialized_set(unit, port, 1);
                SOCDNX_IF_ERR_EXIT(rv);
                
            }

            SOC_PBMP_PORT_ADD(*okay_pbmp, port);
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
soc_jer_portmod_port_enable_set(int unit, soc_port_t port, int enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, enable));

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
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_set(unit, port, &config));

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
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_set(unit, port, &config));

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

#undef _ERR_MSG_MODULE_NAME

