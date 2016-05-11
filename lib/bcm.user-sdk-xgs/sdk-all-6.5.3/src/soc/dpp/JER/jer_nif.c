/*
 * $Id: $
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
 *
 * File: jer_egr_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_port.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_stat.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/dpp/JER/jer_flow_control.h>
#include <soc/phy/phymod_sim.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dcmn/error.h>
#include <sal/appl/sal.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>

#include <phymod/chip/bcmi_tsce_xgxs_defs.h>
#ifndef PHYMOD_EXCLUDE_CHIPLESS_TYPES
#define PHYMOD_EXCLUDE_CHIPLESS_TYPES
#include <phymod/chip/bcmi_tscf_xgxs_defs.h>
#undef PHYMOD_EXCLUDE_CHIPLESS_TYPES
#endif /* PHYMOD_EXCLUDE_CHIPLESS_TYPES */

#define SOC_JER_INVALID_PORT             -1

#define SOC_JERICHO_NOF_LANES_PER_CORE    4
#define SOC_JERICHO_NOF_PMS_PER_NBI       6
#define SOC_JERICHO_NOF_ILKN_WRAP		  3
#define SOC_JERICHO_MAX_PMS_PER_ILKN_PORT 6
#define SOC_JERICHO_NUM_PMS_ILKN45		  3
#define SOC_JERICHO_NUM_FIRST_PM_ILKN45	  3
#define JER_NIF_ILKN_MAX_NOF_LANES        24
#define JER_NIF_ILKN_BURST_MAX_VAL        256

#define SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_JERICHO        24
#define SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_QMX            16

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

#define SOC_QMX_NUM_OF_PMLS   2
#define JER_NIF_PRIO_TDM_LEVEL 2
#define JER_NIF_PRIO_HIGH_LEVEL 1
#define JER_NIF_PRIO_LOW_LEVEL 0

#define JER_SOC_TSCE_VCO_6_25_PLL_DIV  (40)

#define JER_SOC_PRD_MODE_VLAN   (0)
#define JER_SOC_PRD_MODE_ITMH   (1)
#define JER_SOC_PRD_MODE_HIGIG  (2)

#define JER_SOC_PRD_MAP_BITS_PER_COLOR  (2)


STATIC void* pm4x25_user_acc[SOC_MAX_NUM_DEVICES]= {NULL};
STATIC void* pm4x10_user_acc[SOC_MAX_NUM_DEVICES]= {NULL};
STATIC void* pm4x10q_user_acc[SOC_MAX_NUM_DEVICES]= {NULL};
STATIC void *fabric_user_acc[SOC_MAX_NUM_DEVICES] = {NULL};

/*STATIC function declaration*/
STATIC int soc_jer_nif_is_ilkn_port_exist(int unit, int ilkn_id, soc_port_t* port);
STATIC int soc_jer_portmod_check_for_qmlf_conflict(int unit, int new_port);
STATIC int soc_jer_port_nbi_ports_rstn(int unit, soc_port_t port, int enable);
STATIC int soc_jer_nif_priority_quad_tdm_high_low_clear(int unit, uint32 quad, int clear_tdm, int clear_high_low);
STATIC soc_port_if_t soc_jer_get_interface_type(int unit, soc_port_t port, uint32 defl);

extern unsigned char  tscf_ucode[];
extern unsigned short tscf_ucode_len;

#define SUB_PHYS_IN_QSGMII                  4

#define SOC_JER_NIF_NOF_LANE_QUADS          18

#define QMLF_0_BIT   1
#define QMLF_1_BIT   2
#define QMLF_2_BIT   4

phymod_bus_t portmod_ext_default_bus = {
    "MDIO Bus",
    portmod_common_phy_mdio_c45_reg_read,
    portmod_common_phy_mdio_c45_reg_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

static soc_dpp_pm_entry_t soc_jer_pml_table[] = {
     /* is_qsgmii, is_vaild, pml_instance , PHY ID (used as core address in PHYMOD*/
        {0,         1,          0,            0x00},
        {0,         1,          0,            0x04},
        {0,         1,          0,            0x08},
        {0,         1,          0,            0x0c},
        {1,         1,          0,            0x10},
        {1,         1,          0,            0x14},

        {0,         0,          1,            0x00},
        {0,         0,          1,            0x04},
        {0,         0,          1,            0x08},
        {0,         1,          1,            0x0c},
        {1,         1,          1,            0x10},
        {1,         1,          1,            0x14},
};

static soc_dpp_pm_entry_t soc_qmx_pml_table[] = {
     /* is_qsgmii, is_vaild, pml_instance , PHY ID (used as core address in PHYMOD*/
        {0,         1,          0,            0x00},
        {0,         1,          0,            0x04},
        {0,         1,          0,            0x08},
        {0,         1,          0,            0x0c},
        {1,         1,          0,            0x10},
        {1,         1,          0,            0x14},

        {0,         1,          1,            0x00},
        {0,         1,          1,            0x04},
        {0,         1,          1,            0x08},
        {0,         1,          1,            0x0c},
        {1,         1,          1,            0x10},
        {1,         1,          1,            0x14},
};

static portmod_pm_instances_t jer_pm_instances[] = {
    {portmodDispatchTypeDnx_fabric, SOC_JERICHO_PM_FABRIC},
    {portmodDispatchTypePm4x25, SOC_JERICHO_PM_4x25},
    {portmodDispatchTypePm4x10, 5},
    {portmodDispatchTypePm4x10Q, 4},
    {portmodDispatchTypePmOsILKN, 3},
    /*QMX extras*/
    {portmodDispatchTypePm4x10, 3},
};

static portmod_pm_identifier_t jer_ilkn_pm_table[SOC_JERICHO_NOF_ILKN_WRAP][SOC_JERICHO_MAX_PMS_PER_ILKN_PORT] =
{
		{
		  {portmodDispatchTypePm4x25, 1},
		  {portmodDispatchTypePm4x25, 5},
		  {portmodDispatchTypePm4x25, 9},
		  {portmodDispatchTypePm4x25, 13},
		  {portmodDispatchTypePm4x25, 17},
		  {portmodDispatchTypePm4x25, 21}
		},
		{
		  {portmodDispatchTypePm4x10, 25},
		  {portmodDispatchTypePm4x10, 29},
		  {portmodDispatchTypePm4x10, 33},
		  {portmodDispatchTypePm4x10, 37},
		  {portmodDispatchTypePm4x10, 41},
		  {portmodDispatchTypePm4x10, 45}
		},
		{
		  {portmodDispatchTypePm4x10, 49},
		  {portmodDispatchTypePm4x10, 53},
		  {portmodDispatchTypePm4x10, 57},
		  {portmodDispatchTypePm4x10, 61},
		  {portmodDispatchTypePm4x10, 65},
		  {portmodDispatchTypePm4x10, 69}
		},
};

int
soc_jer_pml_table_get(int unit, soc_dpp_pm_entry_t **soc_pml_table)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QMX(unit))
    {
        *soc_pml_table = &soc_qmx_pml_table[0];
    }
    else
    {
        *soc_pml_table = &soc_jer_pml_table[0];
    }

    SOCDNX_FUNC_RETURN;
}


int
soc_jer_pm_instances_get(int unit, portmod_pm_instances_t **pm_instances, int *pms_instances_arr_len)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QMX(unit))
    {
          *pm_instances = &jer_pm_instances[0];
          *pms_instances_arr_len = 6;
    }
    else
    {
          *pm_instances = &jer_pm_instances[0];
          *pms_instances_arr_len = 5;
    }

    SOCDNX_FUNC_RETURN;
}

/* 
 * phy should be 1-based (1-72), 
 * new_phy returned is also 1-based (1-144) 
 */ 
int
soc_jer_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy)
{
    int qsgmii_count, skip;
    SOCDNX_INIT_FUNC_DEFS;

    if(phy < 37 || phy >= 192) {
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
soc_jer_qsgmii_offsets_add_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp)
{
    uint32 phy, new_phy;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*new_pbmp);

    SOC_PBMP_ITER(*pbmp, phy)
    {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_add, (unit, phy, &new_phy))); 
        SOC_PBMP_PORT_ADD(*new_pbmp, new_phy);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * phy should be 1-based (1-144), 
 * new_phy returned is also 1-based (1-72) 
 */ 
int
soc_jer_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy)
{
    int qsgmii_count;
    int reduce = 0;
    SOCDNX_INIT_FUNC_DEFS;

    reduce = 0;

    if(phy >= 37 && phy < 192) {
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

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy, &new_phy)));
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

STATIC int
soc_jer_nif_ilkn_lane_map_get(int unit, soc_port_t port, int *ilkn_lane_map_override, int ilkn_lane_map[JER_NIF_ILKN_MAX_NOF_LANES]) 
{
    uint32 nof_lanes, offset, phy_i;
    char *map_str;
    int mapping, lane, first_phy = 0, align;
    soc_pbmp_t phys, phy_ports;
    SOCDNX_INIT_FUNC_DEFS;

    *ilkn_lane_map_override = 0; /*no ovverride by default*/

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &nof_lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));

    for (lane = 0; lane < nof_lanes; lane++)
    {
        /*Format: ilkn_lane_map_<logical_port>_lane<num>=<mapping>*/
        /*To easily support reverse-order mapping the following convention can be used: ilkn_lane_map_1=reversed*/
        /*Default value is 1-1 mapping*/
        map_str = soc_property_port_suffix_num_get_str(unit, port, lane, spn_ILKN_LANE_MAP, "lane");
        if (map_str == NULL) {
            mapping = lane; 
        } else if (sal_strcmp(map_str, "reversed")== 0) {
            mapping = nof_lanes - lane;

            /*if the soc property explictly set - mark override*/
            *ilkn_lane_map_override = 1;
        } else {
            mapping = soc_property_port_suffix_num_get(unit, port, lane, spn_ILKN_LANE_MAP, "lane", lane);

            /*if the soc property explictly set - mark override*/
            *ilkn_lane_map_override = 1;
        }
        
        ilkn_lane_map[lane] = mapping;
    }

    if (*ilkn_lane_map_override == 1) {
        /* adjust the mapping to the actual lanes in the core */
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phy_ports, &phys));

        align = SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, offset) ? 192 : 1;
        SOC_PBMP_ITER(phys, phy_i){
            first_phy = (phy_i - align) % JER_NIF_ILKN_MAX_NOF_LANES;
            if (SOC_IS_QMX(unit) && SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, offset)) {
                first_phy += 8;
            }
            break; 
        }

        /*coverity - protect the array from memory override*/
        if (first_phy + nof_lanes > JER_NIF_ILKN_MAX_NOF_LANES) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("port %d lanes are out of bounds"), port));
        }

        for (lane = 0; lane < nof_lanes; ++lane) {
            if (first_phy + lane >= JER_NIF_ILKN_MAX_NOF_LANES) {
                continue; /*Coverity protection*/
            }
            /* There is a check that first_phy + lane doesn't exceed the size of the array ilkn_lane_map */
            /* coverity[ilkn_lane_map:FALSE] */
            ilkn_lane_map[lane + first_phy] = ilkn_lane_map[lane] + first_phy; 
        }
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
    uint32 nof_pms_per_nbi  = SOC_DPP_DEFS_GET(unit, nof_pms_per_nbi);
    uint32 nof_lanes_per_nbi = nof_pms_per_nbi * NUM_OF_LANES_IN_PM;
    uint32 nof_port_per_nbih = nof_lanes_per_nbi;
    SOCDNX_INIT_FUNC_DEFS;

    if (!(SOC_IS_KALIA(unit) && IS_SFI_PORT(unit,port))) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &lane)));
    } else {
        interface_type = SOC_PORT_IF_SFI;
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        lane = phy_port;
    }
    
    switch (action) {
    case portmodCallBackActionTypeDuring:
        if (!(SOC_IS_KALIA(unit) && IS_SFI_PORT(unit,port))) {
            reg = (lane < nof_port_per_nbih + 1) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;
            reg_port = (lane < nof_port_per_nbih + 1) ? REG_PORT_ANY : lane > (2 * nof_lanes_per_nbi);
            index = ((lane - 1) % nof_lanes_per_nbi) / NUM_OF_LANES_IN_PM;

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
            SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &lanes));
            val = (interface_type == SOC_PORT_IF_QSGMII) ? 0xf : 0x1;
            rst_port = 0;
            SOC_PBMP_ITER(lanes, lane_i) {
                rst_port |= val << (((lane_i - 1) % 4) * 4);
            }
        } else {
            reg = NBIH_TX_QMLF_CONFIGr;
            reg_port = REG_PORT_ANY;
            index = (lane - 1) % nof_lanes_per_nbi / NUM_OF_LANES_IN_PM; 
            rst_port = 0Xffff << (index * 4);
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
    int pm, phy, l, ams_pll_val;
    soc_reg_t reg;
    soc_field_t field;
    soc_dpp_config_t *dpp = NULL;
    portmod_pm_create_info_t pm_info;
    portmod_default_user_access_t* user_acc;
    int is_sim, blk, first_phy;
    uint32 rx_polarity, tx_polarity, reg_val, otp_bits;
    uint32 nof_pm4x25    = SOC_DPP_DEFS_GET(unit, nof_pm4x25);
    uint32 pmh_base_lane = SOC_DPP_DEFS_GET(unit, pmh_base_lane);
    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);

    /* allocate user access struct */
    user_acc = sal_alloc(sizeof(portmod_default_user_access_t)*nof_pm4x25, "PM4x25_USER_ACCESS");
    if(user_acc == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate user access memory for PM4x25")));
    }
    sal_memset(user_acc, 0, sizeof(portmod_default_user_access_t)*nof_pm4x25);
    pm4x25_user_acc[unit] = user_acc;

    for(pm=0 ; pm<nof_pm4x25 ; pm++) {
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));

        pm_info.type = portmodDispatchTypePm4x25;
        first_phy = pmh_base_lane + NUM_OF_LANES_IN_PM*pm + 1;

        SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(&(pm_info.pm_specific_info.pm4x25.polarity)));
        for(l=0 ; l<NUM_OF_LANES_IN_PM ; l++) {
            phy = first_phy + l;
            SOC_PBMP_PORT_ADD(pm_info.phys, phy);

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

        if (SOC_IS_JERICHO_A0(unit) || SOC_IS_QMX_A0(unit)) {
            ams_pll_val = soc_property_suffix_num_get(unit, 0, "custom_feature", "ams_pll_override", 0x1f); 
            
            if ((ams_pll_val != 0x19) && (ams_pll_val != 0x1E) && (ams_pll_val != 0x1f)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ams pll value 0x%x not valid"), ams_pll_val));
            }
            pm_info.pm_specific_info.pm4x25.afe_pll.afe_pll_change_default = 1; 
            pm_info.pm_specific_info.pm4x25.afe_pll.ams_pll_en_hrz = ams_pll_val & 1; /*only bit 0 of the val*/
            pm_info.pm_specific_info.pm4x25.afe_pll.ams_pll_iqp = (ams_pll_val &~ 1) >> 1; /*bits 1-4 of the val*/
        }

        /* Add PM to PortMod*/
        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

        if (!SOC_WARM_BOOT(unit)) {



 

            if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
            /* Power Down on all ports - only active ports will be enabled */
             reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIH_REG_0C06r : NBIH_NIF_PM_CFGr;
             SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, pm, &reg_val));
             field = (SOC_IS_QAX(unit)) ? PMH_N_OTP_PORT_BOND_OPTIONf : FIELD_0_8f;
             otp_bits =  soc_reg_field_get(unit, reg, reg_val, field);
             otp_bits |= 0xf0;
             otp_bits &= ~0x100; /* disable all quads - bit[8]  */
             soc_reg_field_set(unit, reg, &reg_val, field, otp_bits);
             SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, pm, reg_val));
            }
         }
 


    }


exit:
    if(SOCDNX_FUNC_ERROR) {
        if(pm4x25_user_acc[unit] != NULL) {
            for(pm=0 ; pm<nof_pm4x25 ; pm++) {
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

STATIC int
soc_jer_portmod_pml_init(int unit)
{
    int rv, pm, phy, l, quad, pml, xlp_instance, block_index, is_sim;
    soc_dpp_config_t *dpp = NULL;
    portmod_pm_create_info_t pm_info;
    portmod_pm4x10_create_info_t *pm4x10_info;
    portmod_pm4x10q_create_info_t *pm4x10q_info;
    portmod_default_user_access_t* user_acc;
    portmod_pm4x10q_user_data_t* qsgmii_user_acc;
    int first_phy, pm_idx, idx, otp_bits, qsgmii_index;
    uint32 ident_bits, rx_polarity, tx_polarity, reg_val;
    soc_reg_t reg;
    soc_dpp_pm_entry_t      *soc_pml_info = NULL;
    uint32 nof_pms_per_nbi  = SOC_DPP_DEFS_GET(unit, nof_pms_per_nbi);
    uint32 nof_pm4x25       = SOC_DPP_DEFS_GET(unit, nof_pm4x25);
    uint32 nof_pm4x10       = SOC_DPP_DEFS_GET(unit, nof_pm4x10);
    uint32 nof_pm4x10q      = SOC_DPP_DEFS_GET(unit, nof_pm4x10q);
    uint32 pml_base_lane    = SOC_DPP_DEFS_GET(unit, pml_base_lane);
    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);
    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_soc_pml_table_get,(unit, &soc_pml_info));
    SOCDNX_IF_ERR_EXIT(rv);
    /* allocate user access struct */    
    user_acc = sal_alloc(sizeof(portmod_default_user_access_t) * nof_pm4x10, "PM4x10_USER_ACCESS");
    if(user_acc == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate user access memory for PM4x10")));
    }
    pm4x10_user_acc[unit] = user_acc;

    qsgmii_user_acc = sal_alloc(sizeof(portmod_pm4x10q_user_data_t) * nof_pm4x10q, "PM4x10Q_USER_ACCESS");
    if(user_acc == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate user access memory for PM4x10Q")));
    }
    pm4x10q_user_acc[unit] = qsgmii_user_acc;

    qsgmii_index = 0;
    xlp_instance = 0;
    /* Initialize PM4x10\PM4x10Q */
    for(pm=0 ; pm<nof_pm4x10 ; pm++) {
        if(!soc_pml_info[pm].is_valid) {
            ++xlp_instance;
            continue;
        }
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        block_index = XLP_BLOCK(unit, xlp_instance); 

        if (soc_pml_info[pm].is_qsgmii) {
            pm_info.type = portmodDispatchTypePm4x10Q;
            pm4x10_info = &pm_info.pm_specific_info.pm4x10q.pm4x10_info;
            pm4x10q_info = &pm_info.pm_specific_info.pm4x10q;
        } else {
            pm_info.type = portmodDispatchTypePm4x10;
            pm4x10_info = &pm_info.pm_specific_info.pm4x10;
            pm4x10q_info = NULL;
        }
        pml = soc_pml_info[pm].pml_instance;
        first_phy = pml_base_lane + NUM_OF_LANES_IN_PM*pm + 1;

        SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(&(pm4x10_info->polarity)));

        for(l=0 ; l<NUM_OF_LANES_IN_PM ; l++) {
            phy = first_phy + l;
            SOC_PBMP_PORT_ADD(pm_info.phys, phy);

            rx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_RX_POLARITY_FLIP, "phy", 0);
            tx_polarity = soc_property_suffix_num_get(unit, phy, spn_PHY_TX_POLARITY_FLIP, "phy", 0);

            pm4x10_info->polarity.rx_polarity |= ((rx_polarity & 0x1) << l);
            pm4x10_info->polarity.tx_polarity |= ((tx_polarity & 0x1) << l);            
        }

        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add_pbmp(unit, &pm_info.phys, &pm_info.pm_specific_info.pm4x10.phy_ports));

        SOCDNX_IF_ERR_EXIT(soc_to_phymod_ref_clk(unit, dpp->jer->pll.ref_clk_pmh_out, &(pm_info.pm_specific_info.pm4x10.ref_clk)));

        SOCDNX_IF_ERR_EXIT(phymod_access_t_init(&pm4x10_info->access));

        SOCDNX_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, &(user_acc[pm])));
        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(user_acc[pm])));
        user_acc[pm].unit = unit;
        user_acc[pm].blk_id = block_index;
        user_acc[pm].mutex = sal_mutex_create("core mutex");
        if(user_acc[pm].mutex == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate mutex for PM4x10")));
        }

        if (soc_pml_info[pm].is_qsgmii) {
            SOCDNX_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, &(qsgmii_user_acc[qsgmii_index].qsgmiie_user_data)));
            PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(qsgmii_user_acc[qsgmii_index].qsgmiie_user_data)));
            qsgmii_user_acc[qsgmii_index].qsgmiie_user_data.unit = unit;
            qsgmii_user_acc[qsgmii_index].qsgmiie_user_data.blk_id = PMQ_BLOCK(unit, qsgmii_index);
            qsgmii_user_acc[qsgmii_index].qsgmiie_user_data.mutex = sal_mutex_create("core mutex");
            if(qsgmii_user_acc[qsgmii_index].qsgmiie_user_data.mutex == NULL) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate mutex for PM4x10")));
            }
            pm4x10q_info->blk_id = PMQ_BLOCK(unit, qsgmii_index);
        }


        /* Firmward loader */
        quad = nof_pm4x25 + pm; /* PML quads are after PMH */
        pm4x10_info->fw_load_method = soc_property_suffix_num_get(unit, quad, spn_LOAD_FIRMWARE, "quad", phymodFirmwareLoadMethodExternal);
        SOC_DPP_JER_CONFIG(unit)->nif.fw_verify[quad] = (pm4x10_info->fw_load_method & 0xff00 ? 1 : 0);
        pm4x10_info->fw_load_method &= 0xff;
        pm4x10_info->external_fw_loader = NULL; /* Use default external loader */

        pm4x10_info->access.user_acc = &(user_acc[pm]);
        pm4x10_info->access.addr = soc_pml_info[pm].phy_id;
        pm4x10_info->access.bus = NULL; /* Use default bus */

        if (soc_pml_info[pm].is_qsgmii) {
            pm4x10q_info->qsgmii_user_acc = (void*)(&(qsgmii_user_acc[qsgmii_index]));
            qsgmii_user_acc[qsgmii_index++].pm4x10_access = pm4x10_info->access;
        }

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

        pm_idx = pm % nof_pms_per_nbi;
        switch (pm_idx) {
        case 3:
            reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_3r : NBIL_NIF_PM_CFG_3r;
            idx = 0;
            break;
        case 4:
            reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_4r : NBIL_NIF_PM_CFG_4r;
            idx = 0;
            break;
        case 5:
            reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_5r : NBIL_NIF_PM_CFG_5r;
            idx = 0;
            break;
        default:
            reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A06r : NBIL_NIF_PM_CFGr;
            idx = pm_idx;
            break;
         }
 
        if (!SOC_WARM_BOOT(unit)) {



 

            if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
                /* Power Down on all ports - only active ports will be enabled */
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, pml, idx, &reg_val));
                otp_bits =  soc_reg_field_get(unit, reg, reg_val, SOC_IS_QAX(unit) ? PML_N_OTP_PORT_BOND_OPTIONf : FIELD_0_13f);
                otp_bits |= 0xf0;
                otp_bits &= ~0x100; /* disable all quads- bit [8] */
                soc_reg_field_set(unit, reg, &reg_val, SOC_IS_QAX(unit) ? PML_N_OTP_PORT_BOND_OPTIONf : FIELD_0_13f, otp_bits);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, pml, idx, reg_val));
            }
        }
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
soc_jer_portmod_ilkn_pmh_init(int unit)
{
	int pm_index;
	soc_pbmp_t ilkn_phys;
    portmod_pm_create_info_t pm_info;
    portmod_pm_identifier_t ilkn_pms[MAX_NUM_OF_PMS_IN_ILKN];
    portmod_pm_identifier_t *ilkn_table;

    SOCDNX_INIT_FUNC_DEFS;

    /* ILKN 0,1 */

    SOC_PBMP_CLEAR(ilkn_phys);
    for(pm_index = 0 ; pm_index < MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
        portmod_pm_identifier_t_init(unit, &ilkn_pms[pm_index]);
    }

    ilkn_table = jer_ilkn_pm_table[0];

    for(pm_index=0 ; pm_index<MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
    	ilkn_pms[pm_index].type = ilkn_table[pm_index].type;
    	ilkn_pms[pm_index].phy = ilkn_table[pm_index].phy;

    	SOC_PBMP_PORTS_RANGE_ADD(ilkn_phys, ilkn_pms[pm_index].phy, NUM_OF_LANES_IN_PM);
    }

    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = MAX_NUM_OF_PMS_IN_ILKN;
    pm_info.pm_specific_info.os_ilkn.wm_high = 14;
    pm_info.pm_specific_info.os_ilkn.wm_low = 15;
    SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys);
    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_portmod_ilkn_pml_init(int unit) {

    portmod_pm_create_info_t pm_info;
    soc_port_t fabric_port;
    portmod_pm_identifier_t *ilkn_table;
    soc_pbmp_t ilkn_phys[SOC_QMX_NUM_OF_PMLS], ilkn_over_fabric_ports;
    portmod_pm_identifier_t ilkn_pms[SOC_QMX_NUM_OF_PMLS][MAX_NUM_OF_PMS_IN_ILKN];
    int nof_fabric_ilkn_pms, first_pm, lane, i;
    int fab_pm, pm, pml, phy, pm_over_fabric = 0, nof_ilkn_pms[SOC_QMX_NUM_OF_PMLS]= {0 , 0};
    uint32 over_fabric_type;
    SOCDNX_INIT_FUNC_DEFS;

    /* clear structs and PBMPs */
    SOC_PBMP_CLEAR(ilkn_over_fabric_ports);
    for(pml = 0; pml < SOC_QMX_NUM_OF_PMLS; ++pml) {
        SOC_PBMP_CLEAR(ilkn_phys[pml]);
        for(pm = 0; pm < MAX_NUM_OF_PMS_IN_ILKN; ++pm) {
            portmod_pm_identifier_t_init(unit, &ilkn_pms[pml][pm]);
        }
    }

    if (SOC_JER_NIF_IS_ILKN_OVER_FABRIC_ENABLED(unit)) {
        over_fabric_type = SOC_IS_QMX(unit) ? 2 : 1;
        SOCDNX_IF_ERR_EXIT(soc_jer_nif_ilkn_over_fabric_pbmp_get(unit, &ilkn_over_fabric_ports));
    } else {
        over_fabric_type = 0;
    }

    nof_fabric_ilkn_pms = SOC_IS_QMX(unit) ? SOC_QMX_PM_FABRIC : MAX_NUM_OF_PMS_IN_ILKN;

    for(pml = 0; pml < SOC_QMX_NUM_OF_PMLS; ++pml) {

        ilkn_table = jer_ilkn_pm_table[pml + 1];

        /* build ILKN PM info */
        first_pm = pml * MAX_NUM_OF_PMS_IN_ILKN;
        fab_pm = SOC_IS_QMX(unit) ? -2 : 0; /* QMX 1st lane = jericho 8th lane */
        for(pm = first_pm; pm < first_pm + MAX_NUM_OF_PMS_IN_ILKN; ++pm, ++fab_pm) {
            /* if one lane from the PM is over fabric, the entire PM will be over fabric */
            pm_over_fabric = 0;
            if(!SOC_IS_QMX(unit) && soc_jer_pml_table[pm].is_valid == 0) {
                pm_over_fabric = 1; /*PM will be over fabric to complete ilkn_pms to 6 - even if ILKN port does not actually use these phys*/
            }
            if (SOC_JER_NIF_IS_ILKN_OVER_FABRIC_ENABLED(unit) && pml == 1 && fab_pm >= 0 && fab_pm < nof_fabric_ilkn_pms) {
                for (lane = 0; lane < NUM_OF_LANES_IN_PM; ++lane) {
                    fabric_port = FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id) + fab_pm * NUM_OF_LANES_IN_PM;
                    if (SOC_PBMP_MEMBER(ilkn_over_fabric_ports, fabric_port + lane)) {
                        pm_over_fabric = 1;
                        break;
                    }
                }
            }
            if (pm_over_fabric) {
                fabric_port = FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id) + fab_pm * NUM_OF_LANES_IN_PM; 
                ilkn_pms[pml][nof_ilkn_pms[pml]].phy = SOC_INFO(unit).port_l2p_mapping[fabric_port];
                ilkn_pms[pml][nof_ilkn_pms[pml]].type = portmodDispatchTypeDnx_fabric;
                
            } else {
                if(!SOC_IS_QMX(unit) && soc_jer_pml_table[pm].is_valid == 0) {
                    continue;
                }
                ilkn_pms[pml][nof_ilkn_pms[pml]].phy = ilkn_table[pm - first_pm].phy;
                ilkn_pms[pml][nof_ilkn_pms[pml]].type = portmodDispatchTypePm4x10;
            }

            for(lane = 0; lane < NUM_OF_LANES_IN_PM; ++lane) {
                phy = ilkn_pms[pml][nof_ilkn_pms[pml]].phy + lane;
                SOC_PBMP_PORT_ADD(ilkn_phys[pml], phy);
            }
            ++nof_ilkn_pms[pml];
        }

        /* Add ILKN PM */
        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        pm_info.type = portmodDispatchTypePmOsILKN;
        pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms[pml];
        pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = nof_ilkn_pms[pml];
        SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys[pml]);
        pm_info.pm_specific_info.os_ilkn.wm_high = 9;
        pm_info.pm_specific_info.os_ilkn.wm_low = 10;
        pm_info.pm_specific_info.os_ilkn.is_over_fabric = over_fabric_type;
        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    }

    /* disable port in SOC_PBMP_SFI */
    for (i = 0; i < nof_fabric_ilkn_pms * NUM_OF_LANES_IN_PM; ++i) {
        fabric_port = FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id) + i;
        if (SOC_PBMP_MEMBER(ilkn_over_fabric_ports, fabric_port)) {
            SOC_PBMP_PORT_REMOVE(SOC_PORT_BITMAP(unit, sfi), fabric_port); 
            SOC_PBMP_PORT_REMOVE(SOC_PORT_BITMAP(unit, port), fabric_port);
            SOC_PBMP_PORT_REMOVE(SOC_PORT_BITMAP(unit, all), fabric_port);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_ilkn_init(unit) {

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_ilkn_pmh_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_ilkn_pml_init(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_portmod_fabric_init(int unit)
{
    int rv, quads_in_fsrd, nof_pms;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit) && !SOC_IS_KALIA(unit))
    {
        SOC_EXIT;
    }

    quads_in_fsrd = SOC_DPP_DEFS_GET(unit, nof_quads_in_fsrd);

    if (SOC_IS_KALIA(unit)) {
        nof_pms = SOC_KALIA_PM_FABRIC;
    } else if (SOC_IS_QMX(unit)) {
        nof_pms = SOC_QMX_PM_FABRIC;
    } else {
        nof_pms = SOC_JERICHO_PM_FABRIC;
    }
    rv = soc_dcmn_fabric_pms_add(unit, nof_pms, FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id), 1, quads_in_fsrd, soc_jer_fabric_mdio_address_get, &fabric_user_acc[unit]);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


static int
soc_jer_portmod_xphy_core_config_update(int unit, int lport, int pport, int phy_idx, uint32_t xphy_id, portmod_xphy_core_info_t *xphy_core_info) {

    int fw_ld_method = 0x1, i;
    uint32_t rx_polarity = 0, tx_polarity = 0;
    uint32_t rx_lane_map = 0, tx_lane_map = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (NULL == xphy_core_info) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("xphy_core_info is NULL, invalid parameter\n")));
    }    

    /* soc read for phy device operation mode */
    xphy_core_info->gearbox_enable = (soc_property_port_get(unit, lport, spn_PHY_GEARBOX_ENABLE, FALSE));
    xphy_core_info->pin_compatibility_enable = soc_property_port_get(unit, lport, spn_PHY_PIN_COMPATIBILITY_ENABLE, FALSE);
    xphy_core_info->phy_mode_reverse = soc_property_port_get(unit, lport, spn_PORT_PHY_MODE_REVERSE, 0);
    xphy_core_info->core_access.device_op_mode = 0;    
    if(xphy_core_info->gearbox_enable) {
        PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_SET(&xphy_core_info->core_access);
    }
    if (xphy_core_info->pin_compatibility_enable) {
        PHYMOD_INTF_CONFIG_PHY_PIN_COMPATIBILITY_ENABLE_SET(&xphy_core_info->core_access);
    }
    if (xphy_core_info->phy_mode_reverse) {
        PHYMOD_INTF_CONFIG_PORT_PHY_MODE_REVERSE_SET(&xphy_core_info->core_access);
    }
    /*lower nibble to represent Force FW load and upper nibble to represent
      load method */
    fw_ld_method = 0x11;
    fw_ld_method = soc_property_port_get(unit, lport, spn_PHY_FORCE_FIRMWARE_LOAD, fw_ld_method);
    switch ((fw_ld_method >> 4) & 0xf) {
        case 0:
            xphy_core_info->fw_load_method = phymodFirmwareLoadMethodNone;
        break;
        case 1:
            xphy_core_info->fw_load_method = phymodFirmwareLoadMethodInternal;
        break;
        case 2:
            xphy_core_info->fw_load_method = phymodFirmwareLoadMethodProgEEPROM;
        break;
        default:
            xphy_core_info->fw_load_method = phymodFirmwareLoadMethodInternal;
        break;
    }

    switch (fw_ld_method & 0xf) {
        case 0:
            /* skip download */
            xphy_core_info->force_fw_load = phymodFirmwareLoadSkip;
        break;
        case 1:
            /* force download */
            xphy_core_info->force_fw_load = phymodFirmwareLoadForce;
        break;
        case 2:
            /* auto download. download firware if two versions are diffirent */
            xphy_core_info->force_fw_load = phymodFirmwareLoadAuto;
        break;
        default:
            xphy_core_info->force_fw_load = phymodFirmwareLoadSkip;
        break;
    }

    /* get the plarity settings for the core. look for the legacy config (logical port based) if 
     * not available then look for a new config mode */
    rx_polarity = soc_property_phy_get (unit, pport, phy_idx, 0, 0, spn_PHY_RX_POLARITY_FLIP, 0xFFFFFFFF); 
    if (rx_polarity == 0xFFFFFFFF) { 
        rx_polarity = soc_property_port_get(unit, lport, spn_PHY_RX_POLARITY_FLIP, 0);
    }
    tx_polarity = soc_property_phy_get (unit, pport, phy_idx, 0, 0, spn_PHY_TX_POLARITY_FLIP, 0xFFFFFFFF); 
    if (tx_polarity == 0xFFFFFFFF) {
        tx_polarity =  soc_property_port_get(unit, lport, spn_PHY_TX_POLARITY_FLIP, 0);
    } 
    xphy_core_info->polarity.rx_polarity = rx_polarity;
    xphy_core_info->polarity.tx_polarity = tx_polarity;
    /* get the lane map information */
    rx_lane_map = soc_property_phy_get (unit, pport, phy_idx, 0, 0, spn_PHY_RX_LANE_MAP, 0xFFFFFFFF);
    if (rx_lane_map == 0xFFFFFFFF) {
        rx_lane_map = soc_property_port_get(unit, lport, spn_PHY_RX_LANE_MAP, 0x3210);
    }
    /* get the lane map information */
    tx_lane_map = soc_property_phy_get (unit, pport, phy_idx, 0, 0, spn_PHY_TX_LANE_MAP, 0xFFFFFFFF);
    if (tx_lane_map == 0xFFFFFFFF) {
        tx_lane_map = soc_property_port_get(unit, lport, spn_PHY_TX_LANE_MAP, 0x3210);
    }   
    /*FIX harcoding the number of lanes to 4 for now 
     * need to figure out the better way to fix this */
    xphy_core_info->lane_map.num_of_lanes = 4;
    for (i = 0; i < 4; i++) { 
        xphy_core_info->lane_map.lane_map_rx[i] = ((rx_polarity >> (i*4)) & 0xf);
        xphy_core_info->lane_map.lane_map_tx[i] = ((tx_polarity >> (i*4)) & 0xf);
    }    

exit:
    SOCDNX_FUNC_RETURN;

}


STATIC int
soc_jer_portmod_register_external_phys(int unit)
{
    int port_i;
    soc_port_t xphy_logical_port;
    uint32 xphy_mdio_addr = 0, xphy_physical_port = 0, tmp_xphy_physical_port = 0, xphy_gearbox_mode = 0;
    portmod_default_user_access_t* local_user_access;
    phymod_core_access_t tmp_ext_phy_access; 

    int iphy = 0, lport = 0, core_index = PORTMOD_CORE_INDEX_INVALID;
    int port_primary = 0, port_offset = 0, port_phy_addr = 0;
    portmod_ext_phy_core_info_t core_info;
    portmod_lane_connection_t lane_connection;

    portmod_xphy_core_info_t          xphy_core_info;
    portmod_xphy_lane_connection_t    xphy_lane_connection;
    soc_pbmp_t phy_ports;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit))
    {
        SOC_EXIT;
    }

    /*
     *  NIF Exteranl PHY
     */
    /* TBD */
    
    SOCDNX_IF_ERR_EXIT(soc_phy_common_init(unit));
    SOCDNX_IF_ERR_EXIT(soc_phyctrl_software_init(unit));

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {

        xphy_mdio_addr = soc_property_port_get(unit, port_i, spn_PORT_PHY_ADDR, SOC_JERICHO_PORT_PHY_ADDR_INVALID);
        xphy_gearbox_mode = soc_property_port_get(unit, port_i, spn_PHY_GEARBOX_ENABLE, 0);
        if (xphy_mdio_addr == SOC_JERICHO_PORT_PHY_ADDR_INVALID) {
            continue;
        }

        if (IS_IL_PORT(unit,port_i)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("XPHY: no support for ILKN interface")));
        }

        if (IS_SFI_PORT(unit,port_i)) {
            continue;
        }

        xphy_logical_port = port_i;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, xphy_logical_port, &phy_ports));

        SOC_PBMP_ITER(phy_ports, tmp_xphy_physical_port) {
            SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, tmp_xphy_physical_port, &xphy_physical_port));
            portmod_xphy_core_info_t_init(unit, &xphy_core_info);
            portmod_xphy_lane_connection_t_init(unit, &xphy_lane_connection);
            xphy_lane_connection.xphy_id = xphy_mdio_addr;
            xphy_lane_connection.ss_lane_mask = 0x1 << ((xphy_physical_port-1) % 4);
            /*This only support 2x25G to 4x10G gearbox*/
            if (xphy_gearbox_mode) {
                xphy_lane_connection.ls_lane_mask = 0x3 << (((xphy_physical_port-1) % 4) * 2);
            } else {
                xphy_lane_connection.ls_lane_mask = xphy_lane_connection.ss_lane_mask;
            }
            SOCDNX_IF_ERR_EXIT(portmod_xphy_lane_attach(unit, xphy_physical_port, 1, &xphy_lane_connection));
        }

        phymod_core_access_t_init(&tmp_ext_phy_access);
        tmp_ext_phy_access.access.bus = &portmod_ext_default_bus;
        tmp_ext_phy_access.access.addr = xphy_mdio_addr; 
        tmp_ext_phy_access.type = phymodDispatchTypeCount; /* Make sure it is invalid. */
        local_user_access = sal_alloc(sizeof(portmod_default_user_access_t), "nif ext phy user access");
        sal_memset(local_user_access, 0, sizeof(portmod_default_user_access_t));
        local_user_access->unit = unit;
        tmp_ext_phy_access.access.user_acc = local_user_access;
        xphy_core_info.core_initialized = 0;
        /* update the value from config */
        xphy_core_info.ref_clk = phymodRefClk156Mhz;
        xphy_core_info.fw_load_method = phymodFirmwareLoadMethodInternal;
        xphy_core_info.primary_core_num = xphy_mdio_addr;
        /*  xphy_core_info. polarity*/
        sal_memcpy(&xphy_core_info.core_access, &tmp_ext_phy_access, sizeof(tmp_ext_phy_access));
        /* get xphy core information from SOC proberty */
        SOCDNX_IF_ERR_EXIT(soc_jer_portmod_xphy_core_config_update(unit, xphy_logical_port, xphy_physical_port, \
            1, xphy_mdio_addr, &xphy_core_info));
        SOCDNX_IF_ERR_EXIT(portmod_xphy_add(unit, xphy_mdio_addr, &xphy_core_info));

    }

    /*
     *  Fabric Exteranl PHY
     */
    SOCDNX_IF_ERR_EXIT(portmod_ext_phy_core_info_t_init(unit, &core_info));
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
                core_index = port_primary;
                SOCDNX_IF_ERR_EXIT(soc_dcmn_external_phy_core_access_get(unit, port_phy_addr, &core_info.core_access.access));
                SOCDNX_IF_ERR_EXIT(portmod_phychain_ext_phy_info_set(unit, 1, core_index, &core_info));
                lane_connection.core_index = core_index;
                lane_connection.lane_index = port_offset;
                SOCDNX_IF_ERR_EXIT(portmod_ext_phy_lane_attach(unit, iphy, 1, &lane_connection));
            }
        }
                
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_tx_stop_data_to_pm(int unit)
{
    int i, inst, is_nbil_en;
    uint64 reg64_val;
    uint32 nof_pms_per_nbi    = SOC_DPP_DEFS_GET(unit, nof_pms_per_nbi);
    uint32 nof_instances_nbil = SOC_DPP_DEFS_GET(unit, nof_instances_nbil);

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        for (i = 0; i < nof_pms_per_nbi; ++i) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_TX_QMLF_CONFIGr(unit, i, &reg64_val)); 
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_0_QMLF_Nf, 1);
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_1_QMLF_Nf, 1);
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_2_QMLF_Nf, 1);
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_STOP_DATA_TO_PORT_MACRO_MLF_3_QMLF_Nf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_TX_QMLF_CONFIGr(unit, i, reg64_val));

            is_nbil_en = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "is_nbil_valid", 1);
            if (is_nbil_en) {
                for (inst = 0; inst < nof_instances_nbil; ++inst) {
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

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_config_start_tx_thrs(int unit)
{
    int i, inst, is_nbil_en, mac_fifo_start_tx_thrs;
    uint64 reg64_val;
    uint32 nof_pms_per_nbi    = SOC_DPP_DEFS_GET(unit, nof_pms_per_nbi);
    uint32 nof_instances_nbil = SOC_DPP_DEFS_GET(unit, nof_instances_nbil);

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        mac_fifo_start_tx_thrs = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mac_fifo_start_tx_thrs", 0x4);
        for (i = 0; i < nof_pms_per_nbi; ++i) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_TX_QMLF_CONFIGr(unit, i, &reg64_val));
            soc_reg64_field32_set(unit, NBIH_TX_QMLF_CONFIGr, &reg64_val, TX_START_TX_THRESHOLD_QMLF_Nf, mac_fifo_start_tx_thrs);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_TX_QMLF_CONFIGr(unit, i, reg64_val));

            is_nbil_en = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "is_nbil_valid", 1);
            if (is_nbil_en) {
                for (inst = 0; inst < nof_instances_nbil; ++inst) {
                    SOCDNX_IF_ERR_EXIT(READ_NBIL_TX_QMLF_CONFIGr(unit, inst, i, &reg64_val));
                    soc_reg64_field32_set(unit, NBIL_TX_QMLF_CONFIGr, &reg64_val, TX_START_TX_THRESHOLD_QMLF_Nf, mac_fifo_start_tx_thrs);
                    SOCDNX_IF_ERR_EXIT(WRITE_NBIL_TX_QMLF_CONFIGr(unit, inst, i, reg64_val));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_portmod_init(int unit)
{
    portmod_pm_instances_t *pm_types_and_instances = NULL;
    int rv, pms_instances_arr_len = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_soc_pm_instances_get,(unit, &pm_types_and_instances, &pms_instances_arr_len));
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(soc_jer_tx_stop_data_to_pm(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_config_start_tx_thrs(unit));

    SOCDNX_IF_ERR_EXIT(portmod_create(unit, 0, SOC_MAX_NUM_PORTS, SOC_MAX_NUM_PORTS, pms_instances_arr_len, pm_types_and_instances));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_pmh_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_pml_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_fabric_init(unit));

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_init,(unit)));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_register_external_phys(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_portmod_port_quad_get(int unit, soc_port_t port, soc_pbmp_t* quad_bmp)
{
    soc_pbmp_t phy_ports, phys;
    int phy;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*quad_bmp);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phy_ports, &phys));

    SOC_PBMP_ITER(phys, phy){
        SOC_PBMP_PORT_ADD(*quad_bmp, (phy - 1) / NUM_OF_LANES_IN_PM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_ilkn_bypass_interface_enable(int unit, int port, int enable) {

    soc_pbmp_t phys, phy_lanes;
    soc_port_t phy, reg_port;
    soc_reg_t reg;
    int shift;
    uint32 offset, rst_lanes = 0, reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &phy_lanes));

    reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4);

    if (!SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, offset)) {
        SOC_PBMP_ITER(phy_lanes, phy){
            if (phy == 0 /*Should be always false since NIF is 1 based*/)
            {
                continue;
            }
            shift = (phy - 1) % JER_NIF_ILKN_MAX_NOF_LANES;
            rst_lanes |= (1 << shift);
        }

        reg = (offset < 2) ? NBIH_NIF_PM_ILKN_RX_RSTNr : NBIL_NIF_PM_ILKN_RX_RSTNr;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val));
        reg_val = (enable) ? (reg_val | rst_lanes) : (reg_val & (~rst_lanes));
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));

        reg = (offset < 2) ? NBIH_NIF_PM_ILKN_TX_RSTNr : NBIL_NIF_PM_ILKN_TX_RSTNr;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val));
        reg_val = (enable) ? (reg_val | rst_lanes) : (reg_val & (~rst_lanes));
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));

    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_port_if_t soc_jer_get_interface_type(int unit, soc_port_t port, uint32 defl)
{
    soc_port_if_t interface_type;
    char *interface_type_as_str = NULL;
    char *interface_types_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
    int i, num_of_interfaces;

    interface_type_as_str = soc_property_port_get_str(unit, port, spn_SERDES_IF_TYPE);
    if (interface_type_as_str != NULL){
        num_of_interfaces = sizeof(interface_types_names)/sizeof(*interface_types_names);
        for(i = 0, interface_type = _SHR_PORT_IF_NOCXN; i < num_of_interfaces; ++i, ++interface_type){
            if(sal_strcasecmp(interface_type_as_str, interface_types_names[i]) == 0){
                break;
            }
        }
        if(i == num_of_interfaces){
            interface_type = soc_property_port_get(unit, port, spn_SERDES_IF_TYPE, defl);
        }
    }
    else{
            interface_type = soc_property_port_get(unit, port, spn_SERDES_IF_TYPE, defl);
    }
    return interface_type;
}

int
soc_jer_portmod_post_init(int unit, soc_pbmp_t* ports)
{
    int val, rv;
    uint32 is_master;
    uint32 runt_pad;
    soc_port_t port;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        SOC_PBMP_ITER(*ports, port) {

            interface_type = soc_jer_get_interface_type(unit, port, SOC_PORT_IF_NULL);

            if (interface_type > SOC_PORT_IF_NULL) {
                SOCDNX_IF_ERR_EXIT(soc_jer_portmod_port_interface_set(unit,  port, interface_type)); 
            }
            
            if (!IS_SFI_PORT(unit, port)) {

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

                if (interface_type != SOC_PORT_IF_ILKN) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_get(unit, port, &runt_pad));
                    SOCDNX_IF_ERR_EXIT(portmod_port_pad_size_set(unit, port, runt_pad));
                }

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

                    } else {
                        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_ilkn_bypass_interface_enable, (unit, port, 1)));
                    }
                }
            } else {
                if (SOC_IS_KALIA(unit)) {
                    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_fabric_o_nif_bypass_interface_enable, (unit, port, 1)));
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

    if(pm4x10q_user_acc[unit] != NULL) {
        sal_free(pm4x10q_user_acc[unit]);
        pm4x10q_user_acc[unit] = NULL;
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
    config->max_speed = SOC_INFO(unit).port_speed_max[port];

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

        if (SOC_PORT_IS_COPPER(flags)) {
            PHYMOD_INTF_MODES_COPPER_SET(config);
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
soc_jer_nif_ilkn_pbmp_get(int unit, soc_port_t port, uint32 ilkn_id, soc_pbmp_t* phys)
{
    int i, lanes, phy_offset;
    char* propval;
    char* propkey;
    int first_phy;
    soc_pbmp_t bm, phy_pbmp;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phy_pbmp);

    propkey = spn_ILKN_LANES;
    propval = soc_property_suffix_num_str_get(unit, ilkn_id, propkey, "");

    if(propval != NULL) {
        first_phy = SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, ilkn_id) ? SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit) - 1: (ilkn_id / 2) * JER_NIF_ILKN_MAX_NOF_LANES;

        if (_shr_pbmp_decode(propval, &bm) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("failed to decode (\"%s\") for %s"), propval, propkey)); 
        }
        SOC_PBMP_COUNT(bm, lanes);
        if ((ilkn_id & 1) && lanes > 12) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d can't have more than 12 lanes"), ilkn_id)); 
        }
        if ((SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, ilkn_id)) && 
            ((first_phy + lanes) > (SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit) + (SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit))))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d lanes are out of range"), ilkn_id)); 
        }
        SOC_PBMP_ITER(bm, i) {
            if(i >= JER_NIF_ILKN_MAX_NOF_LANES) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("lanes %d is out of range"), i));       
            }
            SOC_PBMP_PORT_ADD(phy_pbmp, first_phy + i + 1);
        }

    } else {
        if (SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, ilkn_id)) {
            /*Fabric over ILKN*/
            phy_offset = (ilkn_id == 5) ? 12 : 0;
            first_phy = SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit) + phy_offset;
        } else {
            first_phy = ilkn_id * 12 + 1;
        }
        lanes = soc_property_port_get(unit, ilkn_id, spn_ILKN_NUM_LANES, 12);

        if ((ilkn_id & 1) && lanes > 12) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d can't have more than 12 lanes"), ilkn_id)); 
        }
        if ((SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, ilkn_id)) && 
            ((first_phy + lanes) > (SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit) + (SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit))))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d lanes are out of range"), ilkn_id)); 
        }
        for(i = 0 ; i < lanes; i++) {
            SOC_PBMP_PORT_ADD(phy_pbmp, first_phy + i);
        }
    }
    SOC_PBMP_ASSIGN(*phys, phy_pbmp);

exit:
    SOCDNX_FUNC_RETURN;
}



int 
soc_jer_nif_ilkn_over_fabric_pbmp_get(int unit, soc_pbmp_t* phys)
{
    int i, lanes, test_lanes;
    char* propval;
    char* propkey;
    uint32 value;
    int first_port;
    soc_pbmp_t bm, phy_pbmp, test_bmp;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phy_pbmp);
    SOC_PBMP_CLEAR(test_bmp);

    SOCDNX_IF_ERR_EXIT(dcmn_property_suffix_num_get(unit, 5, spn_USE_FABRIC_LINKS_FOR_ILKN_NIF, "bmp", 0,&value));
    propkey = spn_USE_FABRIC_LINKS_FOR_ILKN_NIF;
    propval = soc_property_suffix_num_str_get(unit, 0, propkey, "bmp");

    if(propval != NULL) {
        first_port =  FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id);

        if (_shr_pbmp_decode(propval, &bm) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("failed to decode (\"%s\") for %s"), propval, propkey)); 
        }
        SOC_PBMP_COUNT(bm, lanes);
        if (lanes > (SOC_IS_QMX(unit) ? SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_QMX : SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_JERICHO)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN over fabric can't have more than %d lanes"),SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit))); 
        }

        SOC_PBMP_PORTS_RANGE_ADD(test_bmp, 0, (SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit)));
        SOC_PBMP_NEGATE(test_bmp, test_bmp);
        SOC_PBMP_AND(test_bmp, bm);
        SOC_PBMP_COUNT(test_bmp, test_lanes);
        if (test_lanes) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN over fabric lanes are outside valid range"))); 
        }

        SOC_PBMP_ITER(bm, i) {
            SOC_PBMP_PORT_ADD(phy_pbmp, first_port + i);
        }

    } else {
        
        /*Fabric over ILKN*/
        first_port = FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id);
        lanes = SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit);
        
        for(i = 0 ; i < lanes; i++) {
            SOC_PBMP_PORT_ADD(phy_pbmp, first_port + i);
        }
    }
    SOC_PBMP_ASSIGN(*phys, phy_pbmp);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_port_ilkn_config_get(int unit, soc_port_t port, portmod_port_add_info_t* add_info)
{
    soc_pbmp_t phys;
    int phy_i;
    uint32 offset, flags;
    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    uint32 oob_if = 0;
    SOCDNX_INIT_FUNC_DEFS;
	
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
    add_info->ilkn_core_id = (offset & 1);
	
    add_info->rx_retransmit = ilkn_config->retransmit.enable_rx;
    add_info->tx_retransmit = ilkn_config->retransmit.enable_tx;
	
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

    if (SOC_PORT_IS_ELK_INTERFACE(flags))
    {
        PORTMOD_PORT_ADD_F_ELK_SET(add_info);
    }
	
    add_info->ilkn_burst_max = soc_property_port_get(unit, offset, spn_ILKN_BURST_MAX, 256);
    if (add_info->ilkn_burst_max != 128 && add_info->ilkn_burst_max != 256) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, 
                             (_BSL_SOCDNX_MSG("Burst max %d not supported (only values 128, 256 are supported)"), add_info->ilkn_burst_max)); 
    }
	
    add_info->ilkn_burst_short = soc_property_port_get(unit, offset, spn_ILKN_BURST_SHORT, 32);
    if ((add_info->ilkn_burst_short > JER_NIF_ILKN_BURST_MAX_VAL / 2) || (add_info->ilkn_burst_short % 32 != 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, 
                             (_BSL_SOCDNX_MSG("Burst short %d should be lesser or equal than half of burst max %d, and must be a multiplier of 32"), 
                              add_info->ilkn_burst_short, JER_NIF_ILKN_BURST_MAX_VAL)); 
    }
	
    add_info->ilkn_metaframe_period = soc_property_port_get(unit, offset, spn_ILKN_METAFRAME_SYNC_PERIOD, 2048);
    if (add_info->ilkn_metaframe_period < 64) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Metaframe period should be at least 64"), add_info->ilkn_metaframe_period)); 
    }
    if (SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, offset)) {
    	SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_nif_ilkn_pbmp_get, (unit, port, offset, &phys)));
        SOC_PBMP_ASSIGN(add_info->phys, phys);
        add_info->ilkn_port_is_over_fabric = 1;
        SOC_PBMP_ITER(phys, phy_i){
            SOC_INFO(unit).port_p2l_mapping[phy_i] = port;
        }
    }

    add_info->ilkn_inb_cal_len_rx = SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[offset][SOC_TMC_CONNECTION_DIRECTION_RX] * 
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[offset][SOC_TMC_CONNECTION_DIRECTION_RX];
    add_info->ilkn_inb_cal_len_tx = SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[offset][SOC_TMC_CONNECTION_DIRECTION_TX] *
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[offset][SOC_TMC_CONNECTION_DIRECTION_TX];

    SOCDNX_IF_ERR_EXIT(jer_fc_find_oob_inf_for_ilkn_inf(unit, port, &oob_if));
    if (oob_if != -1) {
        add_info->ilkn_oob_cal_len_rx = SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[oob_if][SOC_TMC_CONNECTION_DIRECTION_RX] *
            SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[oob_if][SOC_TMC_CONNECTION_DIRECTION_RX];
        add_info->ilkn_oob_cal_len_tx = SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[oob_if][SOC_TMC_CONNECTION_DIRECTION_TX] *
            SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[oob_if][SOC_TMC_CONNECTION_DIRECTION_TX];
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_nif_pm_credits_override(int unit, soc_port_t port)
{
    soc_pbmp_t phys, lanes;
    uint32 first_phy, phy_lane;
    soc_reg_t reg;
    int qmlf_index = 0, reg_port, max_ports_nbih, max_ports_nbil;
    uint64 reg64_val;
    int nof_phys, is_qsgmii, val;
    soc_port_t lane;
    uint32 orig_port_crdt, credits_val, rst_port_crdt;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    max_ports_nbih = SOC_DPP_DEFS_GET(unit, nof_ports_nbih);
    max_ports_nbil = SOC_DPP_DEFS_GET(unit, nof_ports_nbil);

    /* override_pm_credits_to_nbi */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy, &phy_lane)));
	SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    --phy_lane; /* phy_lane returned is one-based */
    --first_phy; /* first_phy returned is one-based */

    reg_port = (first_phy < max_ports_nbih) ? REG_PORT_ANY : (first_phy / (max_ports_nbih + max_ports_nbil));

    qmlf_index = (phy_lane % max_ports_nbih) / NUM_OF_LANES_IN_PM; 
    is_qsgmii = (interface_type == SOC_PORT_IF_QSGMII);
    SOC_PBMP_COUNT(phys, nof_phys);

    credits_val = (is_qsgmii) ? 4 : (nof_phys * 8);
    val = (is_qsgmii) ? 0xf : 0x1;
    rst_port_crdt = 0;
    
    SOC_PBMP_ITER(lanes, lane) {
        if (lane == 0) {
            continue; /*Coverity protection*/
        }
        rst_port_crdt |= val << (((lane - 1) % 4) * 4); 
    }

    /* reset the credits for selected ports */
    reg = (first_phy < max_ports_nbih) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, qmlf_index, &reg64_val));
    orig_port_crdt = soc_reg64_field32_get(unit, reg, reg64_val, TX_RESET_PORT_CREDITS_QMLF_Nf);
    rst_port_crdt |= orig_port_crdt;
    soc_reg64_field32_set(unit, reg, &reg64_val, TX_RESET_PORT_CREDITS_QMLF_Nf, rst_port_crdt);
    soc_reg64_field32_set(unit, reg, &reg64_val, TX_RESET_PORT_CREDITS_VALUE_QMLF_Nf, credits_val);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, qmlf_index, reg64_val));

    /* restore default values*/
    soc_reg64_field32_set(unit, reg, &reg64_val, TX_RESET_PORT_CREDITS_QMLF_Nf, 0);
    soc_reg64_field32_set(unit, reg, &reg64_val, TX_RESET_PORT_CREDITS_VALUE_QMLF_Nf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, qmlf_index, reg64_val));
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_portmod_power_enable_set(int unit, soc_port_t port, int enable) {

    soc_pbmp_t quad_ports, phys, lanes;
    int idx, rv, pm_idx, reg_port, quad_nof_ports ;
    soc_field_t field;
    soc_reg_t reg;
     uint32 otp_bits, reg_val, quad, phy_lane, first_phy;
	 int nof_pms_per_nbi;
    SOCDNX_INIT_FUNC_DEFS;
	
	nof_pms_per_nbi = SOC_DPP_DEFS_GET(unit, nof_pms_per_nbi);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));

    /* ILKN over fabric ports dont need this configuration */
    if (first_phy >= SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit)) {
        SOC_EXIT;
    }

    rv = soc_port_sw_db_phy_ports_get(unit, port, &phys); 
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &lanes);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_jer_port_ports_to_same_quad_get(unit, port, &quad_ports);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_PBMP_COUNT(quad_ports, quad_nof_ports);
    /* if there is more than one port on this quad, do nothing*/
    if (!enable && quad_nof_ports > 1) {
        SOC_EXIT;
    }

    SOC_PBMP_ITER(lanes, phy_lane) {
        quad = (phy_lane - 1) / NUM_OF_LANES_IN_PM; 
        reg_port = (quad / nof_pms_per_nbi == 0) ? REG_PORT_ANY : (quad / nof_pms_per_nbi) - 1;
        pm_idx = quad % nof_pms_per_nbi;
        if (reg_port == REG_PORT_ANY) {
            reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIH_REG_0C06r : NBIH_NIF_PM_CFGr;
            field = SOC_IS_QAX(unit) ? PMH_N_OTP_PORT_BOND_OPTIONf : FIELD_0_8f;
            idx = pm_idx;
        } else { 
            field = SOC_IS_QAX(unit) ? PML_N_OTP_PORT_BOND_OPTIONf : FIELD_0_13f;
            switch (pm_idx) {
            case 3:
                reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_3r : NBIL_NIF_PM_CFG_3r;
                idx = 0;
                break;
            case 4:
                reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_4r : NBIL_NIF_PM_CFG_4r;
                idx = 0;
                break;
            case 5:
                reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_5r : NBIL_NIF_PM_CFG_5r;
                idx = 0;
                break;
            default:
                reg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A06r : NBIL_NIF_PM_CFGr;
                idx = pm_idx;
                break;
            }
        }
        /* Enable power on active lanes */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, idx, &reg_val));
        otp_bits = soc_reg_field_get(unit, reg, reg_val, field);
        otp_bits = (enable) ? (otp_bits | 0x100) : (otp_bits &~ 0x100); /* enable/disable this quad */
        otp_bits = (enable) ? (otp_bits &~ 0xf0) : (otp_bits | 0xf0); /* enable/disable all its lanes (once the quad is active - no power cost for enabling all its lanes) */
        soc_reg_field_set(unit, reg, &reg_val, field, otp_bits);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, idx, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_detach(int unit, int port){

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_remove(unit, port));

    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_power_enable_set(unit, port, 0));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_jer_portmod_config_info_init(int unit, soc_port_t port, portmod_port_add_info_t *add_info, int is_init_sequence)
{
    int rv, speed, cl72;
    int an_cl37 = 0, an_cl73=0;
    soc_pbmp_t phys;
    int fw_verify = 0;
    uint32 first_phy_quad, first_phy = 0, first_lane, phy_lane;
    phymod_operation_mode_t phy_op_mode=0;
    int serdes_1000x_at_6250_vco;
    SOCDNX_INIT_FUNC_DEFS;

    rv = portmod_port_add_info_t_init(unit, add_info);
                SOCDNX_IF_ERR_EXIT(rv);

    /* Skip autoneg configuration in port attach */
    PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_SET(add_info);
    /* Fill in init config */
    an_cl37 = soc_property_port_get(unit, port, spn_PHY_AN_C37, 0);
    an_cl73 = soc_property_port_get(unit, port, spn_PHY_AN_C73, 0);
    add_info->init_config.an_mode = phymod_AN_MODE_CL73 ;
    add_info->init_config.an_cl72 = 1 ;
    if (PORTMOD_CL73_WO_BAM == an_cl73) {
        add_info->init_config.an_mode = phymod_AN_MODE_CL73;
        add_info->init_config.an_cl72 = 1 ;
    }
    else if (PORTMOD_CL73_W_BAM == an_cl73) {
        add_info->init_config.an_mode = phymod_AN_MODE_CL73BAM;
        add_info->init_config.an_cl72 = 1 ;
    }
    else if (PORTMOD_CL37_W_BAM == an_cl37) {
        add_info->init_config.an_mode = phymod_AN_MODE_CL37BAM;
        add_info->init_config.an_cl72 = 0 ;
    }
    else if (PORTMOD_CL37_WO_BAM == an_cl37) {
        add_info->init_config.an_mode = phymod_AN_MODE_CL37;
        add_info->init_config.an_cl72 = 0 ;
    }
    add_info->init_config.fs_cl72 = soc_property_port_get(unit, port, \
        spn_PORT_INIT_CL72, 0);
    add_info->init_config.an_cl72 = soc_property_port_get(unit, port, \
        spn_PHY_AN_C72, add_info->init_config.an_cl72);
    add_info->init_config.an_fec = soc_property_port_get(unit, port, \
        spn_PHY_AN_FEC, add_info->init_config.an_fec);
    add_info->init_config.cx4_10g = soc_property_port_get(unit, port, \
                                     spn_10G_IS_CX4, TRUE);

    /* default mode - strup & append CRC at MAC level*/
    PORTMOD_PORT_ADD_F_RX_SRIP_CRC_SET(add_info);
    PORTMOD_PORT_ADD_F_TX_APPEND_CRC_SET(add_info);

    rv = soc_port_sw_db_speed_get(unit, port, &speed);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = _soc_jer_portmod_speed_to_if_config(unit, port, speed, &(add_info->interface_config));
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_phy_ports_get(unit, port, &phys);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &(add_info->phys));
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_first_phy_port_get(unit, port, &first_phy);
    SOCDNX_IF_ERR_EXIT(rv);

    if (add_info->interface_config.interface == SOC_PORT_IF_QSGMII) {
        add_info->sub_phy = (first_phy - 1) % 4;
    }

    SOC_PBMP_CLEAR(add_info->phy_ports);
    if (add_info->sub_phy) {
        SOC_PBMP_PORT_ADD(add_info->phy_ports, first_phy - add_info->sub_phy);
    } else {
        SOC_PBMP_ASSIGN(add_info->phy_ports, phys);
    }

    SOC_PBMP_ITER(add_info->phys, phy_lane) {




        if (SOC_IS_JERICHO_PLUS_A0(unit)) {
            break;
        }
        if (phy_lane >= SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit)) {
            add_info->ilkn_port_is_over_fabric = 1;
            break;
        }
    }
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy, &first_lane)));
    first_phy_quad = (first_lane - 1) / 4;
    /*coverity[overrun-local]*/
    fw_verify = SOC_DPP_JER_CONFIG(unit)->nif.fw_verify[first_phy_quad];

    if(fw_verify) {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(add_info);
    } else {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(add_info);
    }

    if (add_info->interface_config.interface == SOC_PORT_IF_ILKN) {
        SOCDNX_IF_ERR_EXIT(jer_fc_connect_ilkn_inf_to_oob_inf(unit, port));

        SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_config_get(unit, port, add_info));
    }

     /* configure link training */
    if (is_init_sequence) {
        cl72 = soc_property_port_get(unit, port, spn_PORT_INIT_CL72, 0) ? 1 : 0;
        add_info->link_training_en = (uint8)cl72;

        phy_op_mode = soc_property_port_get(unit, port, spn_PHY_PCS_REPEATER,
                                             phymodOperationModeRetimer);

        add_info->phy_op_mode = (phymod_operation_mode_t) phy_op_mode;
        add_info->interface_config.port_op_mode = (int) phy_op_mode;

        add_info->interface_config.line_interface = soc_jer_get_interface_type(unit, port, SOC_PORT_IF_NULL);

        serdes_1000x_at_6250_vco = soc_property_port_get(unit, port, spn_SERDES_1000X_AT_6250_VCO, 0);

        if (serdes_1000x_at_6250_vco) {
            add_info->interface_config.pll_divider_req = JER_SOC_TSCE_VCO_6_25_PLL_DIV;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}
int
soc_jer_portmod_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence)
{
    int rv, counter_interval;
    soc_port_t port;
    portmod_port_add_info_t add_info;
    uint32 is_master, flags, counter_flags;
    dcmn_port_fabric_init_config_t port_config;
    soc_pbmp_t counter_pbmp;
    int broadcast_load_fabric = 0;
    dcmn_port_init_stage_t stage;
    phymod_firmware_load_method_t fw_load_method_fabric = phymodFirmwareLoadMethodNone;
    int fw_verify_fabric = 0;
    int fsrd_block_, fmac_block_, blk, is_first_link, i;
    soc_info_t          *si;
    soc_port_if_t interface_type;
    pbmp_t nif_pbmp;
    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    if (SOC_WARM_BOOT(unit)) {
        SOC_PBMP_ASSIGN(*okay_pbmp, pbmp);
    } else {
        SOC_PBMP_CLEAR(*okay_pbmp);
        SOC_PBMP_CLEAR(nif_pbmp);

        if (is_init_sequence) {
            fw_load_method_fabric = soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, "fabric", phymodFirmwareLoadMethodExternal);
            fw_verify_fabric = (fw_load_method_fabric & 0xff00 ? 1 : 0);
            fw_load_method_fabric &= 0xff;

            if(fw_load_method_fabric == phymodFirmwareLoadMethodExternal) {
                 broadcast_load_fabric = 1;
            }
        }


        SOC_PBMP_ITER(pbmp, port) {
            if (IS_SFI_PORT(unit, port) || SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {

                if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
                    rv = soc_jer_port_first_link_in_fsrd_get(unit, port, &is_first_link, 1); 

                    if (is_first_link == 1) 
                    {
                        fsrd_block_ = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port)/SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd);

                        fmac_block_ = fsrd_block_ * SOC_DPP_DEFS_GET(unit, nof_quads_in_fsrd);

                        for (i = fmac_block_; i < fmac_block_ +  SOC_DPP_DEFS_GET(unit, nof_quads_in_fsrd) ; i++)
                        {
                            blk = si->fmac_block[fmac_block_];
                            si->block_valid[blk] = 1;
                        }

                        if (!SOC_IS_QAX(unit)) {
                            blk = si->fsrd_block[fsrd_block_];
                            si->block_valid[blk] = 1;

                            rv = soc_jer_port_update_fsrd_block(unit, port, 1);
                            SOCDNX_IF_ERR_EXIT(rv);
                        }
                    }
                }
                if (SOC_IS_KALIA(unit)) {
                     SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_open_fab_o_nif_path, (unit, port)));
                }

                /*make sure fabric port is not used for ILKN*/
                /* SFI is loaded in 2 stages - this is stage 1*/
                rv = _soc_jer_port_fabric_config_get(unit, port, is_init_sequence, &port_config);
                SOCDNX_IF_ERR_EXIT(rv);

                stage = broadcast_load_fabric ? dcmn_port_init_until_fw_load : dcmn_port_init_full;

                rv = soc_dcmn_fabric_port_probe(unit, port, stage, fw_verify_fabric, &port_config);
                SOCDNX_IF_ERR_EXIT(rv);
                if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
                    SOC_PBMP_PORT_ADD(si->sfi.bitmap, port);
                    SOC_PBMP_PORT_ADD(si->port.bitmap, port);
                    SOC_PBMP_PORT_ADD(si->all.bitmap, port);

                    SOC_PBMP_PORT_REMOVE(si->sfi.disabled_bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->port.disabled_bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);
                }
            }
        }
        /*NIF: Probe ,initialize and FW download*/
        /*step1: probe Serdes and external PHY core*/
        SOC_PBMP_ITER(pbmp, port) {
            /* Init NIF ports*/
            if (IS_SFI_PORT(unit, port) || SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
                continue;
            }

            /* Init NIF ports*/
            rv = soc_jer_port_open_path(unit, port);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = portmod_port_add_info_t_init(unit, &add_info);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = _soc_jer_portmod_config_info_init(unit, port, &add_info, is_init_sequence);
            SOCDNX_IF_ERR_EXIT(rv);
            SOCDNX_IF_ERR_EXIT(soc_jer_portmod_power_enable_set(unit, port, 1));

            if (add_info.interface_config.interface != SOC_PORT_IF_ILKN && (!is_init_sequence)) {
                /* Before adding the port - check that port doesn't overwrite an existing ILKN QMLF */
                SOCDNX_IF_ERR_EXIT (soc_jer_portmod_check_for_qmlf_conflict (unit, port));
            }
            /*ILKN and QSGMII ports only need one time init.*/
            if ((add_info.interface_config.interface != SOC_PORT_IF_ILKN) &&
                 (add_info.interface_config.interface != SOC_PORT_IF_QSGMII)) {
                 PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_SET(&add_info);
                 rv = portmod_port_add(unit, port, &add_info);
                 PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_CLR(&add_info);
            } else {
                 rv = portmod_port_add(unit, port, &add_info);
            }
            SOCDNX_IF_ERR_EXIT(rv);
        }

        /*step2 : initialize PASS1 for SerDes and external PHY*/
        SOC_PBMP_ITER(pbmp, port) {
            if (IS_SFI_PORT(unit, port) || SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
                continue;
            }
            rv = portmod_port_add_info_t_init(unit, &add_info);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = _soc_jer_portmod_config_info_init(unit, port, &add_info, is_init_sequence);
            SOCDNX_IF_ERR_EXIT(rv);
            if ((add_info.interface_config.interface != SOC_PORT_IF_ILKN) &&
                 (add_info.interface_config.interface != SOC_PORT_IF_QSGMII)) {
                 PORTMOD_PORT_ADD_F_INIT_PASS1_SET(&add_info);
                 rv = portmod_port_add(unit, port, &add_info);
                 PORTMOD_PORT_ADD_F_INIT_PASS1_CLR(&add_info);
            }

            SOCDNX_IF_ERR_EXIT(rv);


            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

            if (is_master && (interface_type != SOC_PORT_IF_ILKN) && (interface_type != SOC_PORT_IF_QSGMII)) {
                SOC_PBMP_PORT_ADD(nif_pbmp, port);
            }
        }

        /* step3:broadcast firmware download for all external phys inculde legacy and Phymod PHYs*/
        rv = portmod_legacy_ext_phy_init(unit, nif_pbmp);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = portmod_common_ext_phy_fw_bcst(unit, nif_pbmp);
        SOCDNX_IF_ERR_EXIT(rv);

        /*step4:initialize PASS2 for Serdes and external PHY*/
        SOC_PBMP_ITER(pbmp, port) {
            if (IS_SFI_PORT(unit, port) || SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
                continue;
            }
            rv = portmod_port_add_info_t_init(unit, &add_info);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = _soc_jer_portmod_config_info_init(unit, port, &add_info, is_init_sequence);
            SOCDNX_IF_ERR_EXIT(rv);
            if ((add_info.interface_config.interface != SOC_PORT_IF_ILKN) &&
                (add_info.interface_config.interface != SOC_PORT_IF_QSGMII)) {
                PORTMOD_PORT_ADD_F_INIT_PASS2_SET(&add_info);
                rv = portmod_port_add(unit, port, &add_info);
                PORTMOD_PORT_ADD_F_INIT_PASS2_CLR(&add_info);
            }

            SOCDNX_IF_ERR_EXIT(rv);
            if (add_info.interface_config.interface == SOC_PORT_IF_ILKN) {
                int lane_map_override;
                int ilkn_lane_map[JER_NIF_ILKN_MAX_NOF_LANES];
                uint32 nof_lanes;

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &nof_lanes));
                SOCDNX_IF_ERR_EXIT(soc_jer_nif_ilkn_lane_map_get(unit, port, &lane_map_override, ilkn_lane_map));

                if (lane_map_override) {
                    SOCDNX_IF_ERR_EXIT(portmod_port_logical_lane_order_set(unit, port, ilkn_lane_map, nof_lanes));
                }
            }
            else if (add_info.interface_config.interface == SOC_PORT_IF_QSGMII) {
                SOCDNX_IF_ERR_EXIT(soc_jer_nif_pm_credits_override(unit, port));
            }
            rv = soc_port_sw_db_initialized_set(unit, port, 1);
            SOCDNX_IF_ERR_EXIT(rv);

            SOC_PBMP_PORT_ADD(*okay_pbmp, port);
        }
        if(broadcast_load_fabric && !(SOC_IS_QAX(unit) && !SOC_IS_KALIA(unit))) {
            /* Load fabric firmware*/
            rv = soc_dcmn_fabric_broadcast_firmware_loader(unit, tscf_ucode_len, tscf_ucode);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        SOC_PBMP_ITER(pbmp, port) {
            if(IS_SFI_PORT(unit, port) && !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
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
                if ( !(IS_SFI_PORT(unit, port) || SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) ) {

                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));

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
                    if((!SOC_PORT_IS_NETWORK_INTERFACE(flags)) || SOC_PORT_IS_LB_MODEM(flags)) {
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
soc_jer_nif_ilkn_wrapper_port_enable_set(int unit, soc_port_t port, int enable)
{
    uint32  reg_val, offset;
    soc_port_t reg_port;
    soc_reg_t reg;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    /** set_ilkn_port_reset in wrapper **/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    reg = (offset < 2) ? ILKN_PMH_ILKN_RESETr : ILKN_PML_ILKN_RESETr;
    reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4);

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val));
    field = (offset & 1) ? ILKN_RX_1_PORT_RSTNf : ILKN_RX_0_PORT_RSTNf;
    soc_reg_field_set(unit, reg, &reg_val, field, enable ? 1 : 0);
    field = (offset & 1) ? ILKN_TX_1_PORT_RSTNf : ILKN_TX_0_PORT_RSTNf;
    soc_reg_field_set(unit, reg, &reg_val, field, enable ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_enable_set(int unit, soc_port_t port, uint32 mac_only, int enable)
{
    uint32 flags = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        SOC_EXIT;
    }
    if(SOC_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
        SOC_EXIT;
    }
    if(mac_only) {
        PORTMOD_PORT_ENABLE_MAC_SET(flags);
    }

    if (IS_IL_PORT(unit, port)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_ilkn_bypass_interface_enable, (unit, port, 0)));
    } else if (IS_SFI_PORT(unit,port) && SOC_IS_KALIA(unit)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_fabric_o_nif_bypass_interface_enable, (unit, port, 0)));
    }
    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, flags, enable));

    if (IS_IL_PORT(unit, port) && enable) {
        SOCDNX_IF_ERR_EXIT(soc_jer_nif_ilkn_wrapper_port_enable_set(unit, port, 1));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_ilkn_bypass_interface_enable, (unit, port, 1)));
    } else if (IS_SFI_PORT(unit,port) && SOC_IS_KALIA(unit) && enable) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_fabric_o_nif_bypass_interface_enable, (unit, port, 1)));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
soc_jer_portmod_port_enable_get(int unit, soc_port_t port, uint32 mac_only, int* enable)
{
    int flags = 0;
    SOCDNX_INIT_FUNC_DEFS;
    

    if (IS_SFI_PORT(unit, port) && SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)) {
        *enable = 0;
    } else {
        if(mac_only) {
            PORTMOD_PORT_ENABLE_MAC_SET(flags);
        }
        SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, flags, enable)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_speed_set(int unit, soc_port_t port, int speed)
{
    portmod_port_interface_config_t config;
    portmod_port_interface_config_t config_pm;
    SOCDNX_INIT_FUNC_DEFS;
		
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config_pm));

    if (IS_IL_PORT(unit, port)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_ilkn_bypass_interface_enable, (unit, port, 0)));
    } else if (IS_SFI_PORT(unit,port) && SOC_IS_KALIA(unit)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_fabric_o_nif_bypass_interface_enable, (unit, port, 0)));
    }

    SOCDNX_IF_ERR_EXIT(_soc_jer_portmod_speed_to_if_config(unit, port, speed, &config));
    config.pll_divider_req = config_pm.pll_divider_req;

    /* keep the correct phy interface, not the main interface stored in sw_db */
	config.interface = config_pm.interface;

	/* this sets the speed */
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_set(unit, port, &config, 
                                          PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
    if (IS_IL_PORT(unit, port)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_ilkn_bypass_interface_enable, (unit, port, 1)));
    } else if (IS_SFI_PORT(unit,port) && SOC_IS_KALIA(unit)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_fabric_o_nif_bypass_interface_enable, (unit, port, 1)));
    }
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
    portmod_port_interface_config_t config;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (!IS_SFI_PORT(unit, port)) {

        SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, &config));
        SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config));

        if (IS_IL_PORT(unit, port)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_ilkn_bypass_interface_enable, (unit, port, 0)));
        }
        /* fiber/copper are relevant to the phy interface, thus clear them and set according to new interface */
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_FIBER));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_COPPER));
		PHYMOD_INTF_MODES_FIBER_CLR(&config);
        PHYMOD_INTF_MODES_COPPER_CLR(&config);
        
        switch (intf) {
            case SOC_PORT_IF_SR:
            case SOC_PORT_IF_SR4:
            case SOC_PORT_IF_SR10:
            case SOC_PORT_IF_SR2:
            case SOC_PORT_IF_LR:
            case SOC_PORT_IF_LR4:
            case SOC_PORT_IF_LR10:
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_FIBER));
                PHYMOD_INTF_MODES_FIBER_SET(&config);
                break;
            case SOC_PORT_IF_CR:
            case SOC_PORT_IF_CR4:
            case SOC_PORT_IF_CR2:
            case SOC_PORT_IF_CR10:
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_COPPER));
                PHYMOD_INTF_MODES_COPPER_SET(&config);
                break;
            default:
                break;
        }

        config.interface = intf;
        SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_set(unit, port, &config,
                                            PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
        if (IS_IL_PORT(unit, port)) {
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_ilkn_bypass_interface_enable, (unit, port, 1)));
        }
    } else {
        SOCDNX_IF_ERR_EXIT(soc_jer_portmod_fabric_port_interface_set(unit, port, intf));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf)
{
    portmod_port_interface_config_t config;
    SOCDNX_INIT_FUNC_DEFS;

    /* Get actual configured value */
    if (IS_SFI_PORT(unit, port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_portmod_fabric_port_interface_get(unit, port, intf));
    } else {
        SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config));
        (*intf) = config.interface; 
    }

    

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_portmod_fabric_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf)
{
    phymod_ref_clk_t ref_clk;
    phymod_phy_inf_config_t phy_config;
    phymod_phy_access_t phys[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    int lane = -1, i;
    portmod_access_get_params_t params; 

    SOCDNX_INIT_FUNC_DEFS;


    portmod_access_get_params_t_init(unit, &params);
    params.lane = lane;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));


    SOCDNX_IF_ERR_EXIT(soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk));


    for (i=0 ; i<phys_returned ; i++) {
        SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(&phys[i], 0 /* flags */, ref_clk, &phy_config)); 
        PHYMOD_INTF_MODES_FIBER_CLR(&phy_config); 
        PHYMOD_INTF_MODES_COPPER_CLR(&phy_config); 
        switch (intf) {
        case SOC_PORT_IF_SR:
            PHYMOD_INTF_MODES_FIBER_SET(&phy_config); 
            break;
        case SOC_PORT_IF_CR:
            PHYMOD_INTF_MODES_COPPER_SET(&phy_config); 
            break;
        default:
            break;
        }
        if (SOC_IS_KALIA(unit)){
            phy_config.interface_type = phymodInterfaceBypass;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(&phys[i], 0 /* flags */,&phy_config));

    }
 
exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_portmod_fabric_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf)
{
    phymod_ref_clk_t ref_clk;
    phymod_phy_inf_config_t phy_config;
    phymod_phy_access_t phys[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    int lane = -1;
    portmod_access_get_params_t params; 

    SOCDNX_INIT_FUNC_DEFS;


    portmod_access_get_params_t_init(unit, &params);
    params.lane = lane;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));


    SOCDNX_IF_ERR_EXIT(soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk));


    
    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(&phys[0], 0 /* flags */, ref_clk, &phy_config)); 


    if (PHYMOD_INTF_MODES_FIBER_GET(&phy_config)) {
        *intf = SOC_PORT_IF_SR; 
    } else if (PHYMOD_INTF_MODES_COPPER_GET(&phy_config)) {
        *intf = SOC_PORT_IF_CR;
    } else {
        *intf = SOC_PORT_IF_KR;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_portmod_port_link_state_get(int unit, soc_port_t port, int clear_status, int *is_link_up, int *is_latch_down) 
{
    uint32 offset, reg32_val, reg_port = 0, latch_down;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_link_get(unit, port, is_link_up));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    if (SOC_PORT_IF_ILKN == interface_type) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_get(unit, port, is_latch_down));
        if (!(*is_latch_down) && (*is_link_up)) {
            /* ILKN0/1 -> port=REG_PORT_ANY, ILKN2/3 -> port=0, ILKN4/5 -> port=1 */
             reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4); 

            if (offset < 2 ) { /* PMH */
                SOCDNX_IF_ERR_EXIT(READ_ILKN_PMH_RX_ILKN_STATUSr(unit, reg_port, offset % 2, &reg32_val));
                latch_down = soc_reg_field_get(unit, ILKN_PMH_RX_ILKN_STATUSr, reg32_val, RX_N_STAT_ALIGNED_LATCH_LOWf); 
            } else { /* PML */
                
                SOCDNX_IF_ERR_EXIT(READ_ILKN_PML_RX_ILKN_STATUSr(unit, reg_port, offset % 2, &reg32_val));
                latch_down = soc_reg_field_get(unit, ILKN_PML_RX_ILKN_STATUSr, reg32_val, RX_N_STAT_ALIGNED_LATCH_LOWf);
            }
            (*is_latch_down) = (latch_down ? 0 : 1);            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_set(unit, port, *is_latch_down));
        } 

        if (clear_status) {
            /* ILKN0/1 -> port=REG_PORT_ANY, ILKN2/3 -> port=0, ILKN4/5 -> port=1 */
             reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4); 
            /* read 'clear on read' register */
            if (offset < 2 ) { /* PMH */
                SOCDNX_IF_ERR_EXIT(READ_ILKN_PMH_RX_ILKN_STATUSr(unit, reg_port, offset % 2, &reg32_val));
            } else { /* PML */
                SOCDNX_IF_ERR_EXIT(READ_ILKN_PML_RX_ILKN_STATUSr(unit, reg_port, offset % 2, &reg32_val));
            }
        }
    } else {
        SOCDNX_IF_ERR_EXIT(portmod_port_link_latch_down_get(unit, port, 
                                                            clear_status ? PORTMOD_PORT_LINK_LATCH_DOWN_F_CLEAR : 0, is_latch_down));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_set(unit, port, *is_latch_down));
    }

    if (clear_status) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_set(unit, port, 0));
    }

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
soc_jer_port_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask) 
{
    int rv;
    phymod_autoneg_status_t an_status;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_autoneg_status_get(unit, port, &an_status));
    if (!an_status.enabled) {
        rv = BCM_E_DISABLED;
    } else if (!an_status.locked) {
        rv = BCM_E_BUSY;
    } else {
        rv = portmod_port_ability_remote_get(unit, port, ability_mask);
    }
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_port_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability_mask) 
{
    int rv;
    int nof_lanes;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_port_nof_lanes_get(unit, port, &nof_lanes);
    if ((1 == nof_lanes) && (ability_mask->speed_full_duplex & SOC_PA_SPEED_25GB)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("25G auto-neg on 1-lane port is not supported by Jericho")));
    }
    rv = portmod_port_ability_advert_set(unit, port, ability_mask);
    SOCDNX_IF_ERR_EXIT(rv);

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

/* ILKN port takes over QMLF memory. (ILKN0 => QMLF 0-1, ILKN1 => QMLF 3-4)
 * ETH cannot work on these quads with ILKN. if using same quad for data and tdm, ILKN only use 1 quad's memory*/
STATIC int
soc_jer_nif_ilkn_vs_eth_collision_resolve(int unit, int port, int ilkn_id, int* free_quad)
{
    uint32 port_i, phy_port, phy_lane, num_phys;
    soc_pbmp_t all_valid_ports;
    int first_phy_quad, quad, ilkn_quad, is_collision = 0;
    int first_quad, found_free_quad = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *free_quad = -1;
    first_quad = ilkn_id * 3;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &phy_lane)));
    ilkn_quad = (phy_lane - 1) / NUM_OF_LANES_IN_PM;

    /* optimization - check if ILKN is already on one of the designated QMLFs*/
    for (quad = first_quad; quad < first_quad + 3; ++quad) {
        if (ilkn_quad == quad) {
            *free_quad = quad % 3;
            SOC_EXIT;
        }
    }
    /* search for collision between ethernet ports and ILKN port*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &all_valid_ports));

    for (quad = first_quad; quad < first_quad + 3; ++quad) {
        is_collision = 0;
        SOC_PBMP_ITER(all_valid_ports, port_i){
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port_i, &num_phys));
            if (num_phys && port_i != port) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_i, &phy_port)); 
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &phy_lane)));
                first_phy_quad = (phy_lane - 1) / NUM_OF_LANES_IN_PM;
                /*collision-> move to next quad*/
                if (first_phy_quad == quad) {
                    found_free_quad = 0;
                    is_collision = 1;
                    break;
                }
            }
        }
        if (is_collision) {
            continue;
        } else {
            found_free_quad = 1;
            break;
        }
    }

    if (found_free_quad == 1) {
        /*return a number between 0-2*/
        *free_quad = (quad % 3);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Interface ILKN%d collides with ports in quads %d, %d, %d. please make one of these quads available for ILKN"),
                                            ilkn_id, first_quad, first_quad + 1, first_quad + 2));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


/* The purpose of this function is to verify that the added port is not conflicting with allocated ILKN QMLF
 * QMLF allocation theory (each line is stronger than the lines following it):
 * If ILKN is not in use or ILKN in KBP format                                                => 0 QMLFs are in use
 * else, if tdm and data are sharing the same QMLF according to
 *   NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF (and by implementation num_of_lanes <= 12) => 1 QMLFs is in use, according to NBIL_HRF_RX_MEM_CONFIG
 * else, if num_of_lanes <= 12 (and from above - tdm is using a dedicated channel)            => 2 QMLFs are in use: 0 and 1
 * else, if num_of_lanes > 12                                                                 => 3 QMLFs are in use: 0, 1 and 2
 * else, not supported. We shouldn't get here
 *
 *
*/
STATIC int
soc_jer_portmod_check_for_qmlf_conflict(int unit, int new_port)
{

    uint32 num_lanes, is_tdm_and_data_sharing_hrf, new_port_offset_within_trio, reg_val;
    uint32 new_port_first_phy_qsgmii, new_port_first_phy, new_port_trio;
    uint32 QMLF_bit_map, conflict_port_bit;
    uint32 flags, new_port_PM, offset;
    soc_port_t reg_port;
    soc_reg_t reg;
    soc_port_t ilkn_logical_port;


    SOCDNX_INIT_FUNC_DEFS;

    /*Getting the PM of the new (non-ILKN) port*/
    SOCDNX_IF_ERR_EXIT (soc_port_sw_db_first_phy_port_get(unit, new_port, &new_port_first_phy_qsgmii));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, new_port_first_phy_qsgmii, &new_port_first_phy));
    new_port_PM = (new_port_first_phy - 1) / NUM_OF_LANES_IN_PM;
    new_port_trio = new_port_PM/3; /*new port trio is the non-ILKN equivalent for ILKN offset*/
    new_port_offset_within_trio = new_port_PM % 3;

    /*Checking - do we have ILKN on this offset (i.e. group of 3 PMs for which the new PM belongs)*/
    SOCDNX_IF_ERR_EXIT(soc_jer_nif_is_ilkn_port_exist(unit, new_port_trio, &ilkn_logical_port));
    if (ilkn_logical_port != SOC_JER_INVALID_PORT) {                /* There is ILKN on this 3 PM offsets! */
        /* Now, if we have ILKN on this 3 PM group - check that its QMLF doesn't conflict with new port PM */

        /*Get info on ILKN port*/
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, ilkn_logical_port, &num_lanes));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, ilkn_logical_port, &flags));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, ilkn_logical_port, 0, &offset));
        reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4); /*Any for 0-1 (NBIH), 0 for 2-3 (NBIL0), 1 for 4-5 (NBIL1)*/
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, &reg_val));
        is_tdm_and_data_sharing_hrf = soc_reg_field_get(unit, NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, reg_val, tdm_data_hrf_fields[offset]);

        /*Sanity test - is offset is correct*/
        if (offset != new_port_trio){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Incompatibility between ILKN offset %d and new_port_trio %d"),offset, new_port_trio));
        }

        /*Getting the internal offsets (0-2) of the ILKN QMLF - see description in function header*/
        if (SOC_PORT_IS_ELK_INTERFACE(flags)){
            QMLF_bit_map = 0;
        }else{
            if (is_tdm_and_data_sharing_hrf){
                reg = (offset < 2) ? NBIH_HRF_RX_MEM_CONFIGr : NBIL_HRF_RX_MEM_CONFIGr;
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, offset & 1, &reg_val));
                QMLF_bit_map = reg_val;
            }else{
                if (num_lanes <= 12) {
                    QMLF_bit_map = QMLF_0_BIT | QMLF_1_BIT;
                }else{
                    QMLF_bit_map = QMLF_0_BIT | QMLF_1_BIT | QMLF_2_BIT;
                }
            }
        }

        /*Checking that ILKN QMLF doesn't conflict with new port PM*/
        conflict_port_bit = QMLF_bit_map & (1 << new_port_offset_within_trio);
        if (conflict_port_bit) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("New port %d is using PM %d. However, this PM is occupied by ILKN QMLF since ILKN %d is using its %d PM!"),
                    new_port, new_port_PM, offset, soc_sand_log2_round_up(conflict_port_bit)));
        }

    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_jer_port_ilkn_tdm_and_data_share_hrf_get(int unit, int port, int* shr_hrf)
{
    uint32 offset;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));

    *shr_hrf = soc_property_port_suffix_num_get(unit, offset, 0, "custom_feature", "ilkn_tdm_and_data_shr_hrf", 0); 
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_jer_port_ilkn_hrf_config(int unit, int port, int offset, soc_pbmp_t* phy_lanes)
{
    int free_quad_for_ilkn = 0, shr_hrf;
    soc_field_t field;
    soc_port_t reg_port;
    soc_reg_t reg;
    uint32 reg_val, num_lanes;
    bcm_pbmp_t pbmp;
    soc_port_t port_i;
    uint32 tdm_channel_exist = 0;
    uint32 flags;

    SOCDNX_INIT_FUNC_DEFS;

    reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4);

    /* Configure HRFs and TDM in NBI (for every active ILKN core) */
    /* if ilkn 0/1 configure only nbih, else configure both nbil(with 0/1) and nbih(with 2-5) */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

    if ((num_lanes <= 12) && (!SOC_PORT_IS_ELK_INTERFACE(flags))) {

        if (SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, offset)) {
            free_quad_for_ilkn = 0;
        } else {
            SOCDNX_IF_ERR_EXIT(soc_jer_nif_ilkn_vs_eth_collision_resolve(unit, port, offset, &free_quad_for_ilkn));
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &pbmp));
        SOC_PBMP_ITER(pbmp, port_i) {
            if(IS_TDM_PORT(unit,port_i)) {
                tdm_channel_exist = 1;
                break;
            }
        }
        SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_tdm_and_data_share_hrf_get(unit, port, &shr_hrf));
        if (!tdm_channel_exist || shr_hrf) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, &reg_val)); 
            soc_reg_field_set(unit, NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, &reg_val, tdm_data_hrf_fields[offset], 1);
            if ((offset < 2) && num_lanes <= 12) {
                  soc_reg_field_set(unit, NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, &reg_val, tdm_data_hrf_fields[offset + 1], 1);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_val));

            if (offset > 1) {
                field = (offset & 1) ? ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_1f : ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_0f;
                SOCDNX_IF_ERR_EXIT(READ_NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_port, &reg_val)); 
                soc_reg_field_set(unit, NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, &reg_val, field, 1);
                if (((offset & 1) == 0) && num_lanes <= 12) {
                    soc_reg_field_set(unit, NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, &reg_val, ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_1f, 1);
                }
                SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_port, reg_val));
            }
        }
        if (free_quad_for_ilkn != 0 && (!tdm_channel_exist || shr_hrf)) {
            reg = (offset < 2) ? NBIH_HRF_RX_MEM_CONFIGr : NBIL_HRF_RX_MEM_CONFIGr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, offset & 1, &reg_val));
            soc_reg_field_set(unit, reg, &reg_val, HRF_RX_MEM_0_HRF_Nf, 0);
            field = (free_quad_for_ilkn == 1) ? HRF_RX_MEM_1_HRF_Nf : HRF_RX_MEM_2_HRF_Nf;
            soc_reg_field_set(unit, reg, &reg_val, field, 1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, offset & 1, reg_val));
			
            reg = (offset < 2) ? NBIH_HRF_TX_MEM_CONFIGr : NBIL_HRF_TX_MEM_CONFIGr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, offset & 1, &reg_val));
            soc_reg_field_set(unit, reg, &reg_val, HRF_TX_MEM_0_HRF_Nf, 0);
            field = (free_quad_for_ilkn == 1) ? HRF_TX_MEM_1_HRF_Nf : HRF_TX_MEM_2_HRF_Nf;
            soc_reg_field_set(unit, reg, &reg_val, field, 1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, offset & 1, reg_val));

        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_config_for_tdm_hrfs_set(int unit, uint32 ilkn_id, int core)
{
    static soc_field_t HRF_RX_PIPE_Nl[] = { HRF_RX_PIPE_2f, HRF_RX_PIPE_3f, HRF_RX_PIPE_6f, HRF_RX_PIPE_7f, HRF_RX_PIPE_10f, HRF_RX_PIPE_11f};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, NBIH_RX_SCH_CONFIG_FOR_TDM_HRFSr, REG_PORT_ANY, HRF_RX_PIPE_Nl[ilkn_id], core));

exit:
    SOCDNX_FUNC_RETURN;

}

int
soc_jer_port_ilkn_over_fabric_set(int unit, soc_port_t port, uint32 ilkn_id) {

    uint32 first_phy_port;
    soc_pbmp_t phys;
    soc_port_t fabric_port;
    int nof_fabric_ilkn_pms, i;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));
    
    if (first_phy_port >= SOC_DPP_FIRST_FABRIC_PHY_PORT(unit) && ilkn_id >= 4) {
        SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric[ilkn_id & 1] = 1;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));

        nof_fabric_ilkn_pms = SOC_IS_QMX(unit) ? SOC_QMX_PM_FABRIC : MAX_NUM_OF_PMS_IN_ILKN;
        /* disable port in SOC_PBMP_SFI */
        for (i = 0; i < nof_fabric_ilkn_pms * NUM_OF_LANES_IN_PM; ++i) {
            fabric_port = FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id) + i;
            if (SOC_PBMP_MEMBER(phys, fabric_port)) {
                SOC_PBMP_PORT_REMOVE(SOC_PORT_BITMAP(unit, sfi), fabric_port); 
                SOC_PBMP_PORT_REMOVE(SOC_PORT_BITMAP(unit, port), fabric_port);
                SOC_PBMP_PORT_REMOVE(SOC_PORT_BITMAP(unit, all), fabric_port);
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_protocol_offset_verify(int unit, soc_port_t port, uint32 protocol_offset) {
    uint32 phy_port;
    uint32 lane_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &lane_id)));
    
    if (!SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, protocol_offset) &&
        protocol_offset / ARAD_NIF_NUM_OF_OFFSETS_IN_PROTOCOL_GROUP != (lane_id - 1) / ARAD_NIF_NUM_OF_LANES_IN_PROTOCOL_GROUP) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Protocol offset %d is out of range for the given interface"), protocol_offset)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_port_open_ilkn_path(int unit, int port) {

    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    int is_tdm;
    soc_port_t reg_port, phy, link;
    soc_pbmp_t phys, phy_lanes;
    soc_pbmp_t ilkn_over_fabric_ports;
    soc_reg_t reg;
    soc_field_t field;
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val;
    uint64 reachability_allowed_bm;
    uint32 reg_val, offset, retrans_multiply_tx, il_over_fabric;
    uint32 is_master, base_index, num_lanes, egr_if, fld_val[1];
    int mubits_tx_polarity, mubits_rx_polarity, fc_tx_polarity, fc_rx_polarity, core, shr_hrf;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phys);
    SOC_PBMP_CLEAR(phy_lanes);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
    /* offset = 0/1 -> ILKN_PMH, offset = 2/3 -> ILKN_PML0, offset = 4/5 -> ILKN_PML1 */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
        
    /* ILKN0/1 -> port=REG_PORT_ANY, ILKN2/3 -> port=0, ILKN4/5 -> port=1 */
    reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4); 

    if (is_master) {
        SOC_PBMP_PORT_ADD(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, port);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &phy_lanes));

        /* Wake up relevant wrapper */
        field = (offset & 1) ? ILKN_1_PORT_RSTNf : ILKN_0_PORT_RSTNf;
        reg = (offset < 2 ) ? ILKN_PMH_ILKN_RESETr : ILKN_PML_ILKN_RESETr;

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val));
        soc_reg_field_set(unit, reg, &reg_val, field, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));
        /* Fabric mux - only in PML1 */
        if (SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, offset)) {
            SOCDNX_IF_ERR_EXIT(READ_ILKN_PML_ILKN_OVER_FABRICr(unit, 1, &reg_val));
            il_over_fabric = soc_reg_field_get(unit, ILKN_PML_ILKN_OVER_FABRICr, reg_val, ILKN_OVER_FABRICf);
            SOC_PBMP_ITER(phy_lanes, phy){
                il_over_fabric |= 1 << (((phy)  % JER_NIF_ILKN_MAX_NOF_LANES) + (SOC_IS_QMX(unit) ? 8 : 0));
            }
    
            soc_reg_field_set(unit, ILKN_PML_ILKN_OVER_FABRICr, &reg_val, ILKN_OVER_FABRICf, il_over_fabric);
            SOCDNX_IF_ERR_EXIT(WRITE_ILKN_PML_ILKN_OVER_FABRICr(unit, 1, reg_val));
             
            SOC_PBMP_CLEAR(ilkn_over_fabric_ports);    
            SOCDNX_IF_ERR_EXIT(soc_jer_nif_ilkn_over_fabric_pbmp_get(unit, &ilkn_over_fabric_ports));
        
            SOCDNX_IF_ERR_EXIT(READ_RTP_ALLOWED_LINKS_FOR_REACHABILITY_MESSAGESr(unit,&reachability_allowed_bm));
            COMPILER_64_NOT(reachability_allowed_bm);
            SOC_PBMP_ITER(ilkn_over_fabric_ports, link){
                if ((link - SOC_DPP_DEFS_GET(unit, first_fabric_link_id) - FABRIC_LOGICAL_PORT_BASE(unit) ) <= SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit)) {
                    COMPILER_64_BITSET(reachability_allowed_bm, (link - SOC_DPP_DEFS_GET(unit, first_fabric_link_id) - FABRIC_LOGICAL_PORT_BASE(unit) )); 
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Invalid ILKN over fabric lane"))); 
                }
            }
            COMPILER_64_NOT(reachability_allowed_bm);
            SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_010Fr(unit, reachability_allowed_bm));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALLOWED_LINKS_FOR_REACHABILITY_MESSAGESr(unit, reachability_allowed_bm));
        }
        /* Enable ILKN in NBIH */
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKENr(unit, &reg_val)); 
        soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, enable_ilkn_fields[offset], 1);
        if ((offset == 0) && num_lanes <= 12 ) {
              soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, enable_ilkn_fields[offset + 1], 1);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKENr(unit, reg_val));

        if(offset >= 2) {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ENABLE_INTERLAKENr(unit, reg_port, &reg_val)); 
            soc_reg_field_set(unit, NBIL_ENABLE_INTERLAKENr, &reg_val, enable_ilkn_fields[offset & 1], 1);
            if (((offset & 1) == 0) && num_lanes <= 12 ) {
                soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, ENABLE_PORT_1f, 1);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ENABLE_INTERLAKENr(unit, reg_port, reg_val));
        }
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

    SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_hrf_config(unit, port, offset, &phy_lanes));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    /* configure by default ilkn nif priority to high */
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_set( unit, core, offset, 1, 0, 1, JER_NIF_PRIO_HIGH_LEVEL));

    SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_tdm_and_data_share_hrf_get(unit, port, &shr_hrf));
    is_tdm = (IS_TDM_PORT(unit, port) && !shr_hrf);

    if (is_tdm) {
        /* if ilkn is tdm add tdm nif priority */
        SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_set( unit, core, offset, 1, 0, 1, JER_NIF_PRIO_TDM_LEVEL));

        /* configure the pipe id for the tdm hrf */
        SOCDNX_IF_ERR_EXIT( soc_jer_sch_config_for_tdm_hrfs_set(unit, offset, core));
    }

    reg = (offset < 2) ? NBIH_HRF_RX_CONFIG_HRFr : NBIL_HRF_RX_CONFIG_HRFr;
    /* HRF 0-1 -> data traffic, HRF 2-3 -> tdm traffic */
    base_index = (is_tdm) ? 2 : 0;
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, reg_port, base_index + (offset & 1), &reg64_val)); 
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_BURST_MERGE_EN_HRF_Nf, 1);
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_BURST_MERGE_TH_HRF_Nf, 0x8); /*default val for now*/
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, ilkn_config->interleaved ? 0 : 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg, reg_port, base_index + (offset & 1), reg64_val));

    /* set_ilkn_tx_hrf_rstn - Make sure HRF is not enabled before it is configured */
    reg = (offset < 2) ? NBIH_HRF_RESETr : NBIL_HRF_RESETr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg_val)); 
    field = (offset & 1) ? HRF_TX_1_CONTROLLER_RSTNf : HRF_TX_0_CONTROLLER_RSTNf;
    soc_reg_field_set(unit, reg, &reg_val, field, 1);
    field = (offset & 1) ? HRF_RX_1_CONTROLLER_RSTNf : HRF_RX_0_CONTROLLER_RSTNf;
    soc_reg_field_set(unit, reg, &reg_val, field, 1);
    if (is_tdm) {
        field = (offset & 1) ? HRF_RX_3_CONTROLLER_RSTNf : HRF_RX_2_CONTROLLER_RSTNf;
        soc_reg_field_set(unit, reg, &reg_val, field, 1); 
    }
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg_val));

    /* All HRFs indications in NBIF for SCH */
    /* set_ilkn_rx_hrf_en */
    switch (offset) {
    case 0:
        field = (is_tdm) ? RX_HRF_ENABLE_HRF_2f : RX_HRF_ENABLE_HRF_0f;
        break;
    case 1:
        field = (is_tdm) ? RX_HRF_ENABLE_HRF_3f : RX_HRF_ENABLE_HRF_1f;
        break;
    case 2:
        field = (is_tdm) ? RX_HRF_ENABLE_HRF_6f : RX_HRF_ENABLE_HRF_4f;
        break;
    case 3:
        field = (is_tdm) ? RX_HRF_ENABLE_HRF_7f : RX_HRF_ENABLE_HRF_5f;
        break;
    case 4:
        field = (is_tdm) ? RX_HRF_ENABLE_HRF_10f : RX_HRF_ENABLE_HRF_8f;
        break;
    case 5:
        field = (is_tdm) ? RX_HRF_ENABLE_HRF_11f : RX_HRF_ENABLE_HRF_9f;
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

    /* set_tx_retransmit_enable */
    reg = (offset < 2) ? ILKN_PMH_ILKN_TX_CONFr : ILKN_PML_ILKN_TX_CONFr;
    field = (offset & 1) ? TX_1_RETRANS_ENf : TX_0_RETRANS_ENf;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, reg_above_64_val)); 
    soc_reg_above_64_field32_set(unit, reg, reg_above_64_val, field, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, reg_above_64_val));

    /* set fragmentation */
    if (is_master) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);

        SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

        *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);        
        SHR_BITCLR(fld_val, egr_if);
        soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);

        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

        /* handle ILKN dedicated mode */
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
            if (is_tdm) {
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
    int pm_index = 0, reg_port, qmlf_index, core, nbi_inst, max_ports_nbih, nof_lanes_nbi;
    uint32 first_lane; /*1-72*/
    uint64 reg64_val;
    uint32 flags, first_phy_port, reg_val, mode, phys_count;
    soc_reg_t reg;
    soc_pbmp_t phys, lanes;
    soc_port_t lane_i;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    max_ports_nbih = SOC_DPP_DEFS_GET(unit, nof_ports_nbih);
    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy_port, &first_lane)));
    --first_phy_port; /* first_phy returned is one-based */
    --first_lane; /* first_lane returned is one-based */

    nbi_inst = first_lane / nof_lanes_nbi; /* NBIH = 0, NBIL0 = 1, NBIL1 = 2*/
    reg_port = (nbi_inst == 0) ? REG_PORT_ANY : nbi_inst - 1;
    pm_index = (first_lane % max_ports_nbih) / NUM_OF_LANES_IN_PM;
    qmlf_index = first_lane / NUM_OF_LANES_IN_PM; /*qmlf index in entire NIF (0-17)*/

    /* take MLF out of reset - if port is ELK, SIF or ILKN- no need to take MLF ooo */
    if (!SOC_PORT_IS_ELK_INTERFACE(flags) && !SOC_PORT_IS_STAT_INTERFACE(flags)) {
        reg = (first_phy_port < max_ports_nbih) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;

        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, reg_port, pm_index, &reg64_val));
        SOC_PBMP_ITER(lanes, lane_i) {
            soc_reg64_field32_set(unit, reg, &reg64_val, fields[(lane_i - 1) % 4], 0);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg, reg_port, pm_index, reg64_val));
    }

    /* Port Mode */
    SOC_PBMP_COUNT(phys, phys_count);
    if (interface_type == SOC_PORT_IF_QSGMII) {
        mode = 3;
    } else if (phys_count == 1) {
        mode = 2; /* Four Ports */
    } else if (phys_count == 2){
        mode = 1; /* Two ports */
    } else {
        mode = 0; /* One Port */
    }

    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_QMLF_CONFIGr(unit, qmlf_index, &reg_val));
    soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, &reg_val, RX_PORT_MODE_QMLF_Nf, mode);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_QMLF_CONFIGr(unit, qmlf_index, reg_val));

    if(first_phy_port >= max_ports_nbih) {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_QMLF_CONFIGr(unit, reg_port, pm_index, &reg_val));
        soc_reg_field_set(unit, NBIL_RX_QMLF_CONFIGr, &reg_val, RX_PORT_MODE_QMLF_Nf, mode);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_QMLF_CONFIGr(unit, reg_port, pm_index, reg_val));
    }

    reg = (first_phy_port < max_ports_nbih) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, pm_index, &reg64_val));
    soc_reg64_field32_set(unit, reg, &reg64_val, TX_PORT_MODE_QMLF_Nf, mode);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, pm_index, reg64_val));

    SOCDNX_IF_ERR_EXIT(soc_jer_port_nbi_ports_rstn(unit, port, 1));

exit:
    SOCDNX_FUNC_RETURN;
}

/* receive ILKN protocol offset and return the logical port if exist */
STATIC 
int soc_jer_nif_is_ilkn_port_exist(int unit, int ilkn_id, soc_port_t* port)
{
    uint32 offset;
    soc_port_t port_i;
    soc_pbmp_t all_valid_ports;
    SOCDNX_INIT_FUNC_DEFS;

    *port = SOC_JER_INVALID_PORT;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &all_valid_ports));

    SOC_PBMP_ITER(all_valid_ports, port_i){
        if (IS_IL_PORT(unit, port_i)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port_i, 0, &offset));
            if (offset == ilkn_id) {
                *port = port_i;
                break;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int 
soc_jer_port_close_ilkn_path(int unit, soc_port_t port) {

    int rv;
    int core, is_tdm;
    uint32 egr_if, fld_val[1], reg_val[1], offset, num_lanes;
    soc_reg_above_64_val_t reg_above_64_val;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    uint32 first_phy, first_lane;
    int reg_port;
    uint32 il_over_fabric;
    soc_reg_t reg, phy, link;
    soc_pbmp_t ilkn_over_fabric_ports;
    soc_field_t field;
    soc_pbmp_t phy_ports, phy_lanes;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    uint32 base_index;
    uint64 reg64_val;
    uint64 reachability_allowed_bm;
    int i, index, shr_hrf, nof_fabric_ilkn_pms;
    soc_port_t second_ilkn_port, fabric_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phy_ports, &phy_lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, first_phy, &first_lane));

    --first_phy; /* first_phy returned is one-based */
    --first_lane; /* first_lane returned is one-based */

    nof_fabric_ilkn_pms = SOC_IS_QMX(unit) ? SOC_QMX_PM_FABRIC : MAX_NUM_OF_PMS_IN_ILKN;

    /* enabling an ILKN port can sometimed require configurations for the other ILKN of the same core
       before removing an ILKN port, we need to find out if the other ILKN port is enabled */
    SOCDNX_IF_ERR_EXIT(soc_jer_nif_is_ilkn_port_exist(unit, ((offset & 1) ? offset - 1 : offset + 1), &second_ilkn_port));
    
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type); 
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_hrf_config(unit, port, offset, &phy_lanes));

    reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4);

    /*unset fragmentation */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

    *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);        
    SHR_BITSET(fld_val, egr_if);
    soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);

    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

    /* handle ILKN dedicated mode */
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

    SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_tdm_and_data_share_hrf_get(unit, port, &shr_hrf));
    is_tdm = (IS_TDM_PORT(unit, port) && !shr_hrf);

    if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
         if (is_tdm) {
             egr_if++;
         } else {
             egr_if--;
         }

         SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

         *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);        
         SHR_BITSET(fld_val, egr_if);
         soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);

         SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
    }


    /* unset_tx_retransmit_enable */
    reg = (offset < 2) ? ILKN_PMH_ILKN_TX_CONFr : ILKN_PML_ILKN_TX_CONFr;
    field = (offset & 1) ? TX_1_RETRANS_ENf : TX_0_RETRANS_ENf;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, reg_above_64_val)); 
    soc_reg_above_64_field32_set(unit, reg, reg_above_64_val, field, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, reg_above_64_val));

    /*disable ilkn bypass interface*/
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_port_ilkn_bypass_interface_enable, (unit, port, 0)));

    /* unset_fc_ilkn_cfg */
    reg = (offset < 2) ? ILKN_PMH_ILKN_INVERT_POLARITY_SIGNALSr : ILKN_PML_ILKN_INVERT_POLARITY_SIGNALSr; 
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, (offset & 1), reg_val));
    soc_reg_field_set(unit, reg, reg_val, ILKN_N_INVERT_RX_MUBITS_POLARITYf, 0x0);
    soc_reg_field_set(unit, reg, reg_val, ILKN_N_INVERT_TX_MUBITS_POLARITYf, 0x0);
    soc_reg_field_set(unit, reg, reg_val, ILKN_N_INVERT_RECEIVED_FC_POLARITYf, 0x0);
    soc_reg_field_set(unit, reg, reg_val, ILKN_N_INVERT_TX_FC_POLARITYf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, (offset & 1), *reg_val));
    

    /* unset_enable_ilkn_port */
    reg = (offset < 2) ? ILKN_PMH_ENABLE_INTERLAKENr : ILKN_PML_ENABLE_INTERLAKENr; 
    field = (offset & 1) ? ENABLE_PORT_1f : ENABLE_PORT_0f;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, reg_val));
    soc_reg_field_set(unit, reg, reg_val, field, 0);
    soc_reg_field_set(unit, reg, reg_val, ENABLE_CORE_CLOCKf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, *reg_val));

    /* All HRFs indications in NBIF for SCH */
    /* unset_ilkn_rx_hrf_en */
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
    SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKEN_HRFr(unit, reg_val)); 
    soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKEN_HRFr, reg_val, field, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKEN_HRFr(unit, *reg_val));

    /* unset_ilkn_tx_hrf_rstn - Make sure HRF is not enabled before it is configured */
    reg = (offset < 2) ? NBIH_HRF_RESETr : NBIL_HRF_RESETr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, reg_val)); 
    field = (offset & 1) ? HRF_TX_1_CONTROLLER_RSTNf : HRF_TX_0_CONTROLLER_RSTNf;
    soc_reg_field_set(unit, reg, reg_val, field, 0);
    field = (offset & 1) ? HRF_RX_1_CONTROLLER_RSTNf : HRF_RX_0_CONTROLLER_RSTNf;
    soc_reg_field_set(unit, reg, reg_val, field, 0);
    if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
        field = (offset & 1) ? HRF_RX_3_CONTROLLER_RSTNf : HRF_RX_2_CONTROLLER_RSTNf;
        soc_reg_field_set(unit, reg, reg_val, field, 0); 
    }
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, *reg_val));

    reg = (offset < 2) ? NBIH_HRF_RX_CONFIG_HRFr : NBIL_HRF_RX_CONFIG_HRFr;
    /* HRF 0-1 -> data traffic, HRF 2-3 -> tdm traffic */
    base_index = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? 2 : 0;
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, reg_port, base_index + (offset & 1), &reg64_val)); 
    soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_EN_HRF_Nf, 0);
    soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_TH_HRF_Nf, 0x8); /*default val for now*/
    soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg, reg_port, base_index + (offset & 1), reg64_val));


    SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_hrf_config(unit, port, offset, &phy_lanes));

    /* 4 HRF's per core : two Data and 2 TDM. if TDM, can configure only pipe field. */
    index = 0;
    switch (offset) {
    case 0:
        reg = NBIH_RX_SCH_CONFIG_HRFr;
        index = offset;
        break;
    case 1:
        reg = NBIH_RX_SCH_CONFIG_HRFr;
        index = offset;
        break;
    case 2:
        reg = NBIH_RX_SCH_CONFIG_HRF_4r;
        break;
    case 3:
        reg = NBIH_RX_SCH_CONFIG_HRF_5r;
        break;
    case 4:
        reg = NBIH_RX_SCH_CONFIG_HRF_8r;
        break;
    case 5:
        reg = NBIH_RX_SCH_CONFIG_HRF_9r;
        break;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, REG_PORT_ANY, index, &reg64_val)); 
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_PIPE_Nf, 0);
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_PRIORITY_HRF_Nf, 0);
    soc_reg64_field32_set(unit, reg, &reg64_val, HRF_RX_SCH_MAP_HRF_Nf, 0); 
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg, REG_PORT_ANY, index, reg64_val));

    /* Unconfig ILKN ports in NBI */
    reg = (offset < 2) ? NBIH_HRF_TX_CONFIG_HRFr : NBIL_HRF_TX_CONFIG_HRFr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, (offset & 1), reg_val)); 
    soc_reg_field_set(unit, reg, reg_val, HRF_TX_NUM_CREDITS_TO_EGQ_HRF_Nf, 0x200);                                          
    soc_reg_field_set(unit, reg, reg_val, HRF_TX_USE_EXTENDED_MEM_HRF_Nf, 0); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, (offset & 1), *reg_val));


    reg = (offset < 2) ? NBIH_ILKN_TX_RETRANSMIT_CONFIG_HRFr : NBIL_ILKN_TX_RETRANSMIT_CONFIG_HRFr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, (offset & 1), reg_val)); 
    soc_reg_field_set(unit, reg, reg_val, ILKN_TX_RETRANS_ENABLE_HRF_Nf, 0);
    soc_reg_field_set(unit, reg, reg_val, ILKN_TX_RETRANS_NUM_ENTRIES_TO_SAVE_HRF_Nf, 0xff);
    soc_reg_field_set(unit, reg, reg_val, ILKN_TX_RETRANS_MULTIPLY_HRF_Nf, 0);
    soc_reg_field_set(unit, reg, reg_val, ILKN_TX_RETRANS_WAIT_FOR_SEQ_NUM_CHANGE_HRF_Nf, 0);
    soc_reg_field_set(unit, reg, reg_val, ILKN_TX_RETRANS_IGNORE_REQ_WHEN_ALMOST_EMPTY_HRF_Nf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, (offset & 1), *reg_val));

    /* Disable ILKN in NBI - only for "big" ports */
    if ((offset & 1) == 0 || (second_ilkn_port == SOC_JER_INVALID_PORT)) {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKENr(unit, reg_val)); 
        soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, reg_val, enable_ilkn_fields[offset], 0);
        if ((offset == 0) && (num_lanes <= 12) && (second_ilkn_port == SOC_JER_INVALID_PORT)) {
              soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, reg_val, enable_ilkn_fields[offset + 1], 0);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKENr(unit, *reg_val));

        if(offset >= 2) {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ENABLE_INTERLAKENr(unit, reg_port, reg_val)); 
            soc_reg_field_set(unit, NBIL_ENABLE_INTERLAKENr, reg_val, enable_ilkn_fields[offset & 1], 0);
            if (((offset & 1) == 0) && (num_lanes <= 12) && (second_ilkn_port == SOC_JER_INVALID_PORT)) {
                soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, reg_val, ENABLE_PORT_1f, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ENABLE_INTERLAKENr(unit, reg_port, *reg_val));
        }

        SOCDNX_IF_ERR_EXIT(READ_NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_val)); 
        soc_reg_field_set(unit, NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, reg_val, tdm_data_hrf_fields[offset], 0);
        if ((offset == 0) && num_lanes <= 12 && (second_ilkn_port == SOC_JER_INVALID_PORT)) {
              soc_reg_field_set(unit, NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, reg_val, tdm_data_hrf_fields[offset + 1], 0);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, *reg_val));

        if (offset >= 2) {
            field = (offset & 1) ? ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_1f : ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_0f;
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_port, reg_val)); 
            soc_reg_field_set(unit, NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, reg_val, field, 0);
            if (((offset & 1) == 0) && (num_lanes <= 12) && (second_ilkn_port == SOC_JER_INVALID_PORT)) {
                soc_reg_field_set(unit, NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr, reg_val, ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRF_ILKN_1f, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ILKN_RX_TDM_AND_DATA_TRAFFIC_ON_SAME_HRFr(unit, reg_port, *reg_val));
        }
    }

    /* Fabric mux - only in PML1 */
    if (SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, offset)) {
        SOCDNX_IF_ERR_EXIT(READ_ILKN_PML_ILKN_OVER_FABRICr(unit, 1, reg_val));
        il_over_fabric = soc_reg_field_get(unit, ILKN_PML_ILKN_OVER_FABRICr, *reg_val, ILKN_OVER_FABRICf);
        SOC_PBMP_ITER(phy_lanes, phy){
            il_over_fabric |= 1 << (((phy) % JER_NIF_ILKN_MAX_NOF_LANES) + (SOC_IS_QMX(unit) ? 8 : 0));
        }
        soc_reg_field_set(unit, ILKN_PML_ILKN_OVER_FABRICr, reg_val, ILKN_OVER_FABRICf, il_over_fabric);
        SOCDNX_IF_ERR_EXIT(WRITE_ILKN_PML_ILKN_OVER_FABRICr(unit, 1, *reg_val));

        SOC_PBMP_CLEAR(ilkn_over_fabric_ports);    
        SOCDNX_IF_ERR_EXIT(soc_jer_nif_ilkn_over_fabric_pbmp_get(unit, &ilkn_over_fabric_ports));
	    
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALLOWED_LINKS_FOR_REACHABILITY_MESSAGESr(unit,&reachability_allowed_bm));
        COMPILER_64_NOT(reachability_allowed_bm);
        SOC_PBMP_ITER(ilkn_over_fabric_ports, link){
        if ((link - SOC_DPP_DEFS_GET(unit, first_fabric_link_id) - FABRIC_LOGICAL_PORT_BASE(unit) ) <= SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit)) {
                COMPILER_64_BITSET(reachability_allowed_bm, (link - SOC_DPP_DEFS_GET(unit, first_fabric_link_id) - FABRIC_LOGICAL_PORT_BASE(unit) )); 
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Invalid ILKN over fabric lane"))); 
            }
        }
        COMPILER_64_NOT(reachability_allowed_bm);
	SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_010Fr(unit, reachability_allowed_bm));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALLOWED_LINKS_FOR_REACHABILITY_MESSAGESr(unit, reachability_allowed_bm));

        for (i = 0; i < nof_fabric_ilkn_pms * NUM_OF_LANES_IN_PM; ++i) {
            fabric_port = FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id) + i;
            if (SOC_PBMP_MEMBER(phy_lanes, fabric_port)) {
                SOC_PBMP_PORT_ADD(SOC_PORT_BITMAP(unit, sfi), fabric_port); 
                SOC_PBMP_PORT_ADD(SOC_PORT_BITMAP(unit, port), fabric_port);
                SOC_PBMP_PORT_ADD(SOC_PORT_BITMAP(unit, all), fabric_port);
            }
        }

        SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric[offset & 1] = 0;
    }

    /* Shut down relevant wrapper */
    reg = (offset < 2 ) ? ILKN_PMH_ILKN_RESETr : ILKN_PML_ILKN_RESETr;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, reg_val));
    field = (offset & 1) ? ILKN_1_PORT_RSTNf : ILKN_0_PORT_RSTNf;
    soc_reg_field_set(unit, reg, reg_val, field, 0);
    field = (offset & 1) ? ILKN_RX_1_PORT_RSTNf : ILKN_RX_0_PORT_RSTNf;
	soc_reg_field_set(unit, reg, reg_val, field, 0);
    field = (offset & 1) ? ILKN_TX_1_PORT_RSTNf : ILKN_TX_0_PORT_RSTNf;
	soc_reg_field_set(unit, reg, reg_val, field, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, *reg_val));
        


exit:
    SOCDNX_FUNC_RETURN;
}


int 
soc_jer_port_close_nbi_path(int unit, soc_port_t port) {

    const soc_field_t fields[4] = {TX_STOP_DATA_TO_PORT_MACRO_MLF_0_QMLF_Nf, TX_STOP_DATA_TO_PORT_MACRO_MLF_1_QMLF_Nf, TX_STOP_DATA_TO_PORT_MACRO_MLF_2_QMLF_Nf, TX_STOP_DATA_TO_PORT_MACRO_MLF_3_QMLF_Nf};
    int pm_index = 0, reg_port, qmlf_index, core, max_ports_nbih, max_ports_nbil, nof_lanes_nbi;
    uint32 first_lane; /*1-72*/
    uint64 reg64_val;
    uint32 flags, first_phy, reg_val, mode;
    soc_reg_t reg;
    soc_pbmp_t phys, lanes;
    soc_port_t lane_i;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    max_ports_nbih = SOC_DPP_DEFS_GET(unit, nof_ports_nbih);
    max_ports_nbil = SOC_DPP_DEFS_GET(unit, nof_ports_nbil);
    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy, &first_lane)));
    --first_phy; /* first_phy returned is one-based */
    --first_lane; /* first_lane returned is one-based */

    SOCDNX_IF_ERR_EXIT(soc_jer_port_nbi_ports_rstn(unit, port, 0));

    reg_port = (first_phy < max_ports_nbih) ? REG_PORT_ANY : (first_phy / (max_ports_nbih + max_ports_nbil));
    pm_index = (first_lane % nof_lanes_nbi) / NUM_OF_LANES_IN_PM; /*internal PM index inside NBIH/L (0-5)*/
    qmlf_index = first_lane / NUM_OF_LANES_IN_PM; /*qmlf index in entire NIF (0-17)*/

    /* take MLF out of reset - if port is ELK, SIF or ILKN- no need to take MLF ooo */
    if (!SOC_PORT_IS_ELK_INTERFACE(flags) && !SOC_PORT_IS_STAT_INTERFACE(flags)) {
        reg = (first_phy < max_ports_nbih) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;

        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, reg_port, pm_index, &reg64_val));
        SOC_PBMP_ITER(lanes, lane_i) {
            soc_reg64_field32_set(unit, reg, &reg64_val, fields[(lane_i - 1) % 4], 1);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg, reg_port, pm_index, reg64_val));
    }

    /* Port Mode */
    mode = 2; /* Four Ports - default */

    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_QMLF_CONFIGr(unit, qmlf_index, &reg_val));
    soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, &reg_val, RX_PORT_MODE_QMLF_Nf, mode);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_QMLF_CONFIGr(unit, qmlf_index, reg_val));

    if(first_phy >= max_ports_nbih) {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_QMLF_CONFIGr(unit, reg_port, pm_index, &reg_val));
        soc_reg_field_set(unit, NBIL_RX_QMLF_CONFIGr, &reg_val, RX_PORT_MODE_QMLF_Nf, mode);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_QMLF_CONFIGr(unit, reg_port, pm_index, reg_val));
    }

    reg = (first_phy < max_ports_nbih) ? NBIH_TX_QMLF_CONFIGr : NBIL_TX_QMLF_CONFIGr;
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, pm_index, &reg64_val));
    soc_reg64_field32_set(unit, reg, &reg64_val, TX_PORT_MODE_QMLF_Nf, mode);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, pm_index, reg64_val));

exit:
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
soc_jer_port_sch_config(int unit, soc_port_t port)
{
    soc_pbmp_t quad_bmp;
    int qmlf_index, core, priority_level;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_port_quad_get(unit, port, &quad_bmp));

    /* ReqEn register */
    priority_level = ( IS_TDM_PORT(unit, port)) ? JER_NIF_PRIO_TDM_LEVEL : JER_NIF_PRIO_HIGH_LEVEL;
    SOC_PBMP_ITER(quad_bmp, qmlf_index) {
        SOCDNX_IF_ERR_EXIT(soc_jer_nif_priority_set(unit, core, qmlf_index, 0, 0, 1, priority_level)); 
    }

exit:
    SOCDNX_FUNC_RETURN;   
}

STATIC int 
soc_jer_port_nbi_ports_rstn(int unit, soc_port_t port, int enable){

    int index = 0, nof_lanes_nbi, max_ports_nbil, max_ports_nbih, nbi_inst;
    uint64 reg64_val, nbil_ports_rstn;
    uint32 first_phy, first_lane, reg_val;
    soc_pbmp_t phys;
    uint32 nbih_ports_rstn[1] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    max_ports_nbih = SOC_DPP_DEFS_GET(unit, nof_ports_nbih);
    max_ports_nbil = SOC_DPP_DEFS_GET(unit, nof_ports_nbil);
    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy, &first_lane)));
    --first_phy; /* first_phy returned is one-based */
    --first_lane; /* first_lane returned is one-based */
    nbi_inst = first_lane / nof_lanes_nbi; /* NBIH = 0, NBIL0 = 1, NBIL1 = 2*/

    if (first_phy < max_ports_nbih) {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_PORTS_SRSTNr(unit, &reg_val));
        *nbih_ports_rstn = soc_reg_field_get(unit, NBIH_RX_PORTS_SRSTNr, reg_val, RX_PORTS_SRSTNf);

        SHR_BITCLR(nbih_ports_rstn, first_phy);
        soc_reg_field_set(unit, NBIH_RX_PORTS_SRSTNr, &reg_val, RX_PORTS_SRSTNf, *nbih_ports_rstn);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_PORTS_SRSTNr(unit, reg_val));

        if (enable) {
            SHR_BITSET(nbih_ports_rstn, first_phy); 
            soc_reg_field_set(unit, NBIH_RX_PORTS_SRSTNr, &reg_val, RX_PORTS_SRSTNf, *nbih_ports_rstn);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_PORTS_SRSTNr(unit, reg_val));
        }

        SOCDNX_IF_ERR_EXIT(READ_NBIH_TX_PORTS_SRSTNr(unit, &reg_val));
        *nbih_ports_rstn = soc_reg_field_get(unit, NBIH_TX_PORTS_SRSTNr, reg_val, TX_PORTS_SRSTNf);

        SHR_BITCLR(nbih_ports_rstn, first_phy);
        soc_reg_field_set(unit, NBIH_TX_PORTS_SRSTNr, &reg_val, TX_PORTS_SRSTNf, *nbih_ports_rstn);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_TX_PORTS_SRSTNr(unit, reg_val));

        if (enable) {
            SHR_BITSET(nbih_ports_rstn, first_phy); 
            soc_reg_field_set(unit, NBIH_TX_PORTS_SRSTNr, &reg_val, TX_PORTS_SRSTNf, *nbih_ports_rstn);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_TX_PORTS_SRSTNr(unit, reg_val));
        }
    } else {
        index = nbi_inst - 1;

        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_PORTS_SRSTNr(unit, index, &reg64_val));
        nbil_ports_rstn = soc_reg64_field_get(unit, NBIL_RX_PORTS_SRSTNr, reg64_val, RX_PORTS_SRSTNf);

        COMPILER_64_BITCLR(nbil_ports_rstn, first_phy - max_ports_nbih - (index * max_ports_nbil));
        soc_reg64_field_set(unit, NBIL_RX_PORTS_SRSTNr, &reg64_val, RX_PORTS_SRSTNf, nbil_ports_rstn);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_PORTS_SRSTNr(unit, index, reg64_val));

        if (enable) {
            COMPILER_64_BITSET(nbil_ports_rstn, first_phy - max_ports_nbih - (index * max_ports_nbil)); 
            soc_reg64_field_set(unit, NBIL_RX_PORTS_SRSTNr, &reg64_val, RX_PORTS_SRSTNf, nbil_ports_rstn);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_PORTS_SRSTNr(unit, index, reg64_val));
        }

        SOCDNX_IF_ERR_EXIT(READ_NBIL_TX_PORTS_SRSTNr(unit, index, &reg64_val));
        nbil_ports_rstn = soc_reg64_field_get(unit, NBIL_TX_PORTS_SRSTNr, reg64_val, TX_PORTS_SRSTNf);

        COMPILER_64_BITCLR(nbil_ports_rstn, first_phy - max_ports_nbih - (index * max_ports_nbil));
        soc_reg64_field_set(unit, NBIL_TX_PORTS_SRSTNr, &reg64_val, TX_PORTS_SRSTNf, nbil_ports_rstn);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_TX_PORTS_SRSTNr(unit, index, reg64_val));

        if (enable) {
            COMPILER_64_BITSET(nbil_ports_rstn, first_phy - max_ports_nbih - (index * max_ports_nbil)); 
            soc_reg64_field_set(unit, NBIL_TX_PORTS_SRSTNr, &reg64_val, TX_PORTS_SRSTNf, nbil_ports_rstn);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_TX_PORTS_SRSTNr(unit, index, reg64_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_open_path(int unit, soc_port_t port) {

    soc_port_if_t interface_type;
    int core;
    uint32 first_phy, flags, is_master;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_RETURN(soc_port_sw_db_is_master_get(unit, port, &is_master));

    if (is_master) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core)); 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
        --first_phy;

        if (SOC_PORT_IF_ILKN == interface_type) {
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_open_ilkn_path, (unit, port)));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_jer_port_open_nbi_path(unit, port));

            /* statistics interface */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
            if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
                SOCDNX_IF_ERR_EXIT(_soc_jer_nif_sif_set(unit, first_phy));
            }
            /* configure NBI scheduler */
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_sch_config, (unit, port)));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;   
}


int
soc_jer_port_close_path(int unit, soc_port_t port) {

    uint32 nof_channels;
    int rv, core, qmlf_index;
    soc_pbmp_t quad_ports;
    uint32 first_phy, phy_lane,flags, egr_if, nof_ports_on_quad = 0;
    soc_reg_above_64_val_t reg_above64_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &nof_channels));

    if (nof_channels == 1) {

        if (IS_IL_PORT(unit, port)) {
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_close_ilkn_path, (unit, port)));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy, &phy_lane)));
            --phy_lane; /* phy_lane returned is one-based */
            --first_phy; /* first_phy returned is one-based */

            /* clear NBI scheduler */
            SOCDNX_IF_ERR_EXIT(soc_jer_port_ports_to_same_quad_get(unit, port, &quad_ports));
            SOC_PBMP_COUNT(quad_ports, nof_ports_on_quad);
            if(nof_ports_on_quad == 1)
            {
                qmlf_index = phy_lane / NUM_OF_LANES_IN_PM;
                SOCDNX_IF_ERR_EXIT(soc_jer_nif_priority_quad_tdm_high_low_clear(unit, qmlf_index, 1, 1));
            }
            /* statistics interface */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
            if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
               SOCDNX_IF_ERR_EXIT(_soc_jer_nif_sif_set(unit, first_phy));
            }

        	SOCDNX_IF_ERR_EXIT(soc_jer_port_close_nbi_path(unit, port));
        }

        /* clear FQP TXI ready bits in EGQ */
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core)); 
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);

        SOCDNX_IF_ERR_EXIT(READ_EGQ_INIT_FQP_TXI_NIFr(unit, core, reg_above64_val));
        SHR_BITSET(reg_above64_val, egr_if);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_NIFr(unit, core, reg_above64_val));

        sal_usleep(1);

        SHR_BITCLR(reg_above64_val, egr_if);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_NIFr(unit, core, reg_above64_val));

    }

exit:
    SOCDNX_FUNC_RETURN;
}



int 
soc_jer_port_pll_type_get(int unit, soc_port_t port, SOC_JER_NIF_PLL_TYPE *pll_type)
{
    uint32 first_phy_port;
    uint32 pll_type_pmh_last_phy_lane = SOC_DPP_DEFS_GET(unit, pll_type_pmh_last_phy_lane);
    uint32 pll_type_pml0_last_phy_lane = SOC_DPP_DEFS_GET(unit, pll_type_pml0_last_phy_lane);
    uint32 pll_type_pml1_last_phy_lane = SOC_DPP_DEFS_GET(unit, pll_type_pml1_last_phy_lane);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port /*one based*/));

    first_phy_port--; /* zero based */

    if (first_phy_port <= pll_type_pmh_last_phy_lane) {
        *pll_type = SOC_JER_NIF_PLL_TYPE_PMH;
    } else if (first_phy_port > pll_type_pmh_last_phy_lane && first_phy_port <= pll_type_pml0_last_phy_lane) {
        *pll_type = SOC_JER_NIF_PLL_TYPE_PML0;
    } else if (first_phy_port > pll_type_pml0_last_phy_lane && first_phy_port <= pll_type_pml1_last_phy_lane) {
        *pll_type = SOC_JER_NIF_PLL_TYPE_PML1;
    } else if ((first_phy_port > SOC_JER_NIF_PLL_TYPE_FABRIC0_FIRST_PHY_LANE) &&
               (first_phy_port <= SOC_JER_NIF_PLL_TYPE_FABRIC0_LAST_PHY_LANE)) {
        *pll_type = SOC_JER_NIF_PLL_TYPE_FABRIC0;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid phy port %d"), first_phy_port));
    }

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

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy, &new_phy)));
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
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy_port, &first_lane)));
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_add, (unit, first_lane + NUM_OF_LANES_IN_PM - 1, &last_phy_port)));

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
    uint32 first_phy_port, is_enabled, pm_idx, pml_idx, reg_port, reg_idx;
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
        soc_reg_t nbih_nif_pm_cfg = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIH_REG_0C06r : NBIH_NIF_PM_CFGr;
        soc_field_t pmh_neee_pd_en = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? FIELD_9_9f : PMH_NEEE_PD_ENf;

        if (value && (0 == nof_ports_enabled)) {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, nbih_nif_pm_cfg, REG_PORT_ANY, pm_idx, &rval));
            soc_reg_field_set(unit, nbih_nif_pm_cfg, &rval, pmh_neee_pd_en, 1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, nbih_nif_pm_cfg, REG_PORT_ANY, pm_idx, rval));
        } else if ((0 == value) && (1 == nof_ports_enabled) && is_enabled) {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, nbih_nif_pm_cfg, REG_PORT_ANY, pm_idx, &rval));
            soc_reg_field_set(unit, nbih_nif_pm_cfg, &rval, pmh_neee_pd_en, 0);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, nbih_nif_pm_cfg, REG_PORT_ANY, pm_idx, rval));
        } else {
            /* do nothing */
        }
    } else {
        soc_reg_t nbil_nif_pm_config;
        soc_field_t pml_neee_pd_en = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? FIELD_14_14f : PML_NEEE_PD_ENf;
        reg_port = (pm_idx / 6) - 1;
        pml_idx = pm_idx % 6;
        switch (pml_idx) {
            case 3:
                nbil_nif_pm_config = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_3r : NBIL_NIF_PM_CFG_3r;
                reg_idx = 0;
                break;
            case 4:
                nbil_nif_pm_config = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_4r : NBIL_NIF_PM_CFG_4r;
                reg_idx = 0;
                break;
            case 5:
                nbil_nif_pm_config = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A09_5r : NBIL_NIF_PM_CFG_5r;
                reg_idx = 0;
                break;
            default:
                nbil_nif_pm_config = (SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_B0(unit)) ? NBIL_REG_0A06r : NBIL_NIF_PM_CFGr;
                reg_idx = pml_idx;
                break;
        }
        if (value && (0 == nof_ports_enabled)) {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, nbil_nif_pm_config, reg_port, reg_idx, &rval));
            soc_reg_field_set(unit, nbil_nif_pm_config, &rval, pml_neee_pd_en, 1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, nbil_nif_pm_config, reg_port, reg_idx, rval));
        } else if ((0 == value) && (1 == nof_ports_enabled) && is_enabled) {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, nbil_nif_pm_config, reg_port, reg_idx, &rval));
            soc_reg_field_set(unit, nbil_nif_pm_config, &rval, pml_neee_pd_en, 0);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, nbil_nif_pm_config, reg_port, reg_idx, rval));
        } else {
            /* do nothing */
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
        case SOC_PORT_IF_SAT:
        case SOC_PORT_IF_IPSEC:
            *nof_lanes = 0;
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Interface %d isn't supported"), interface));
            break;
    }


    /* validity check */
    if ((interface != SOC_PORT_IF_ILKN)  &&
        (interface != SOC_PORT_IF_CPU)   &&
        (interface != SOC_PORT_IF_RCY)   &&
        (interface != SOC_PORT_IF_ERP)   &&
        (interface != SOC_PORT_IF_OLP)   &&
        (interface != SOC_PORT_IF_OAMP)  &&
        (interface != SOC_PORT_IF_IPSEC) &&
        (interface != SOC_PORT_IF_SAT)) {
        if ((first_phy_port - 1) % (*nof_lanes) != 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("interface not supported on port")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 jer_port_link_up_mac_update(int unit, soc_port_t port, int link)
{
    portmod_port_update_control_t portmod_port_update_control;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_update_control_t_init(unit, &portmod_port_update_control));
    portmod_port_update_control.link_status = link;
    SOCDNX_IF_ERR_EXIT(portmod_port_update(unit, port, &portmod_port_update_control));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_remote_fault_enable_set(int unit, bcm_port_t port, int enable)
{
    int rv; 
    portmod_remote_fault_control_t control;
    SOCDNX_INIT_FUNC_DEFS;
    
    rv = portmod_remote_fault_control_t_init(unit, &control);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = portmod_port_remote_fault_control_get(unit, port, &control);
    SOCDNX_IF_ERR_EXIT(rv);
    control.enable = (uint8)enable;
    rv = portmod_port_remote_fault_control_set(unit, port, &control);
    SOCDNX_IF_ERR_EXIT(rv);

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
        if (phy_port >= SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit) && phy_port < SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit) + 
            (SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit))) {
            /*no core collision - fabric SerDes can be use for a single SerDes only*/
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));

        /* calculate quad */
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &new_phy)));
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

/*
 * Function:
 *      soc_jer_port_ports_to_same_quad_get
 * Purpose:
 *      gets all ports on the same quad
 * Parameters:
 *      unit    - Device Number
 *      port    - local port
 *      ports   - bit map with relevant ports
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_port_ports_to_same_quad_get(int unit, soc_port_t port, soc_pbmp_t* ports)
{
    int port_i;
    uint32 is_valid = 0;
    uint32 quad_count;
    soc_pbmp_t ref_quad_pbmp;
    soc_pbmp_t current_quad_pbmp;

    SOCDNX_INIT_FUNC_DEFS;

    /* Check input */
    SOCDNX_NULL_CHECK(ports);
    SOCDNX_IF_ERR_EXIT( soc_port_sw_db_is_valid_port_get( unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d is not valid"), port)); 
    }

    /* get port's quad to ref_quad */
    SOC_PBMP_CLEAR(ref_quad_pbmp);
    SOCDNX_IF_ERR_EXIT( soc_jer_portmod_port_quad_get(unit, port, &ref_quad_pbmp));

    /* iterate over other ports and see which has the same quad */
    SOCDNX_IF_ERR_EXIT( soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, ports));
    SOC_PBMP_ITER((*ports), port_i) 
    {
        SOC_PBMP_CLEAR(current_quad_pbmp);
        SOCDNX_IF_ERR_EXIT( soc_jer_portmod_port_quad_get(unit, port_i, &current_quad_pbmp));
        SOC_PBMP_AND(current_quad_pbmp, ref_quad_pbmp);
        SOC_PBMP_COUNT(current_quad_pbmp, quad_count);
        if (quad_count == 0) {
            SOC_PBMP_PORT_REMOVE(*ports, port_i);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_port_quad_ports_get
 * Purpose:
 *      gets all ports on given quad
 * Parameters:
 *      unit    - Device Number
 *      quad    - quad number
 *      ports   - bit map with relevant ports
 * Returns:
 *      SOC_E_XXX
 */

int soc_jer_port_quad_ports_get(int unit, uint32 quad, soc_pbmp_t* ports_bm)
{
    int port_i;
    soc_pbmp_t valid_ports;
    soc_pbmp_t quad_bmp;

    SOCDNX_INIT_FUNC_DEFS;    
    
    /* Check input */
    SOCDNX_NULL_CHECK(ports_bm);
    if(quad >= SOC_JER_NIF_NOF_LANE_QUADS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Quad %d is invalid"), quad)); 
    }

    SOC_PBMP_CLEAR(*ports_bm);
    /* get valid ports */
    SOCDNX_IF_ERR_EXIT( soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &valid_ports));

    /* iterate over valid ports */
    SOC_PBMP_ITER(valid_ports, port_i) {
        SOCDNX_IF_ERR_EXIT( soc_jer_portmod_port_quad_get(unit, port_i, &quad_bmp));
        /* find first valid port on reference quad */
        if( SOC_PBMP_MEMBER(quad_bmp, quad)) {
            /* find other ports on same quad */
            SOCDNX_IF_ERR_EXIT( soc_jer_port_ports_to_same_quad_get(unit, port_i, ports_bm));
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/* Set nbih trigger and wait until it reset */
int
soc_jer_wait_gtimer_trigger(int unit)
{
    int rv, counter;
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    rv = READ_NBIH_GTIMER_TRIGGERr(unit, &reg_val);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_field_set(unit, NBIH_GTIMER_TRIGGERr, &reg_val, GTIMER_TRIGGERf, 0x0);
    rv = WRITE_NBIH_GTIMER_TRIGGERr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);
    sal_usleep(500000);

    soc_reg_field_set(unit, NBIH_GTIMER_TRIGGERr, &reg_val, GTIMER_TRIGGERf, 0x1);
    rv = WRITE_NBIH_GTIMER_TRIGGERr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_NBIH_GTIMER_TRIGGERr(unit, &reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

    counter = 0;
    while (reg_val == 0x1) {
        sal_usleep(500000);
        rv = READ_NBIH_GTIMER_TRIGGERr(unit, &reg_val);
        SOCDNX_IF_ERR_EXIT(rv);
        if(10 == counter){
            rv = SOC_E_TIMEOUT;
            SOC_EXIT;
        }

        counter++;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_quad_tdm_high_low_clear
 * Purpose:
 *      clears quad priority.
 * Parameters:
 *      unit            - Device Number
 *      quad            - quad to clear
 *      clear_tdm       - clear tdm priority configurations
 *      clear_high_low  - clear high and low priority configurations
 * Returns:
 *      SOC_E_XXX
 * Notes: 
 *      this function clears all quad priorities ( tdm, high and low ) 
 */
STATIC int soc_jer_nif_priority_quad_tdm_high_low_clear(int unit, uint32 quad, int clear_tdm, int clear_high_low)
{
    uint32 mask;
    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t above64_mask;

    SOCDNX_INIT_FUNC_DEFS;

    /* Clearing mask */
    mask = ~(1 << quad);
    SOC_REG_ABOVE_64_SET_WORD_PATTERN(above64_mask, mask);

    
    if (clear_tdm) {
        /* Clear TDM */
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg32_val)); 
        reg32_val &= mask;
        SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg32_val));

        if (!SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_1_TDM_ENr(unit, &reg32_val)); 
            reg32_val &= mask;
            SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_1_TDM_ENr(unit, reg32_val));
        }
    }

    if (clear_high_low) {
        /* Clear High */
        SOCDNX_IF_ERR_EXIT( READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg32_val));
        reg32_val &= mask;
        SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg32_val));

        if (!SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT( READ_NBIH_RX_REQ_PIPE_1_HIGH_ENr(unit, &reg32_val));
            reg32_val &= mask;
            SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_1_HIGH_ENr(unit, reg32_val));
        }

        /* Clear Low */
        SOCDNX_IF_ERR_EXIT( READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
        SOC_REG_ABOVE_64_AND(reg_above_64_val, above64_mask);
        SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));

        if (!SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT( READ_NBIH_RX_REQ_PIPE_1_LOW_ENr(unit, reg_above_64_val));
            SOC_REG_ABOVE_64_AND(reg_above_64_val, above64_mask);
            SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_1_LOW_ENr(unit, reg_above_64_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_nif_priority_quad_tdm_set
 * Purpose:
 *      set tdm priority according to core
 * Parameters:
 *      unit    - Device Number
 *      core    - core for setting priority
 *      quad    - quad to clear
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_quad_tdm_set(int unit, int core, uint32 quad)
{
    uint32 mask;
    uint32 reg32_val;
    soc_reg_t prio_reg;

    SOCDNX_INIT_FUNC_DEFS;

    prio_reg = (core == 0) ? NBIH_RX_REQ_PIPE_0_TDM_ENr : NBIH_RX_REQ_PIPE_1_TDM_ENr;

    /* Setting mask */
    mask = (1 << quad);

    /* get modify set reg */
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, prio_reg, REG_PORT_ANY, 0, &reg32_val));
    reg32_val |= mask;
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, prio_reg, REG_PORT_ANY, 0, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_quad_high_set
 * Purpose:
 *      set high priority according to core
 * Parameters:
 *      unit    - Device Number
 *      core    - core for setting priority
 *      quad    - quad to clear
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_quad_high_set(int unit, int core, uint32 quad)
{
    uint32 mask;
    uint32 reg32_val;
    soc_reg_t prio_reg;

    SOCDNX_INIT_FUNC_DEFS;

    prio_reg = (core == 0) ? NBIH_RX_REQ_PIPE_0_HIGH_ENr : NBIH_RX_REQ_PIPE_1_HIGH_ENr;

    /* Setting mask */
    mask = (1 << quad);

    /* get modify set reg */
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, prio_reg, REG_PORT_ANY, 0, &reg32_val));
    reg32_val |= mask;
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, prio_reg, REG_PORT_ANY, 0, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_quad_low_set
 * Purpose:
 *      set low priority according to core
 * Parameters:
 *      unit    - Device Number
 *      core    - core for setting priority
 *      quad    - quad to clear
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_quad_low_set(int unit, int core, uint32 quad)
{
    uint32 mask;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t above64_mask;
    soc_reg_t prio_reg;

    SOCDNX_INIT_FUNC_DEFS;

    prio_reg = (core == 0) ? NBIH_RX_REQ_PIPE_0_LOW_ENr : NBIH_RX_REQ_PIPE_1_LOW_ENr;

    /* Setting mask */
    mask = (1 << quad);
    SOC_REG_ABOVE_64_SET_WORD_PATTERN(above64_mask, mask);

    /* get modify set reg */
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, prio_reg, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_OR(reg_above_64_val, above64_mask);
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_set(unit, prio_reg, REG_PORT_ANY, 0, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_ilkn_tdm_high_low_create_mask
 * Purpose:
 *      creates the set mask for given ilkn according to the needed priority.
 * Parameters:
 *      unit    - Device Number
 *      ilkn_id - id of the ilkn port (0-5)
 *      is_tdm  - set if mask needed is for tdm prio
 *      is_high - set if mask needed is for high prio
 *      mask    - return value of mask
 * Returns:
 *      SOC_E_XXX
 * Notes: 
 *      this function works for all priorities ( tdm, high and low ) 
 */
STATIC int soc_jer_nif_priority_ilkn_tdm_high_low_create_mask(int unit, uint32 ilkn_id, int is_tdm, int is_high, uint32* mask)
{

    uint32 unique_mask;
    uint32 default_mask;
    SHR_BITDCL* serdes_qmlfs;
    SHR_BITDCL* memory_qmlfs;
    static int nif_ilkn_tdm_priority_bits[] = { 0xC0002, 0x300010, 0xC00080, 0x3000400, 0xC002000, 0x30010000};
    /* Each priority level and ilkn couple has thier own unique prioritt bits controling them, the following arrays describe those bits, array for high and low prio, and index mark the ilkn */
    static int nif_ilkn_uniqe_high_priority_bits[] = { 0xC0000, 0x300000, 0xC00000, 0x3000000, 0xC000000, 0x30000000};
    static int nif_ilkn_uniqe_low_priority_bits[] = { 0xF8600000, 0x0, 0x1C0000, 0x0, 0x7800000, 0x0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(mask);

    if (is_tdm) 
    {
        /* set mask according to ilkn */
        *mask = nif_ilkn_tdm_priority_bits[ilkn_id];
    } else {
        /* get taken serdes and memory qmlfs */
        serdes_qmlfs = SOC_DPP_CONFIG(unit)->jer->nif.ilkn_qmlf_resources[ilkn_id].serdes_qmlfs; 
        memory_qmlfs = SOC_DPP_CONFIG(unit)->jer->nif.ilkn_qmlf_resources[ilkn_id].memory_qmlfs;

        /* get uniqe mask according to wanted prio */
        unique_mask = is_high ? nif_ilkn_uniqe_high_priority_bits[ilkn_id] : nif_ilkn_uniqe_low_priority_bits[ilkn_id] ;

        /* get default mask, the default mask is for each ilkn the 3 coresponding qmlfs 0:0,1,2  1:3,4,5 etc. */
        default_mask = 0x7 << (ilkn_id * 3);

        /* set mask as uniqe mask and bits already taken by the qmlfs */
        *mask = unique_mask | (default_mask & serdes_qmlfs[0]) | (default_mask & memory_qmlfs[0]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_ilkn_tdm_clear
 * Purpose:
 *      clears the tdm prio for given ilkn
 * Parameters:
 *      unit    - Device Number
 *      ilkn_id - id of the ilkn port (0-5)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_ilkn_tdm_clear(int unit, uint32 ilkn_id)
{
    int i;
    int size;
    uint32 reg32_val;
    uint32 mask;
    soc_reg_t prio_regs[] = {NBIH_RX_REQ_PIPE_0_TDM_ENr, NBIH_RX_REQ_PIPE_1_TDM_ENr};

    SOCDNX_INIT_FUNC_DEFS;

    /* create mask */
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_tdm_high_low_create_mask(unit, ilkn_id, 1, 0, &mask));
    mask = ~mask;

    /* clear prio */
    size = sizeof(prio_regs)/sizeof(soc_reg_t);
    for (i = 0; i < size; ++i) {
        SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, prio_regs[i], REG_PORT_ANY, 0, &reg32_val));
        reg32_val &= mask;
        SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, prio_regs[i], REG_PORT_ANY, 0, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_ilkn_tdm_set
 * Purpose:
 *      sets the tdm prio for given ilkn
 * Parameters:
 *      unit    - Device Number
 *      core    - core for which to set priority
 *      ilkn_id - id of the ilkn port (0-5)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_ilkn_tdm_set(int unit, int core, uint32 ilkn_id)
{
    uint32 reg32_val;
    uint32 mask;
    soc_reg_t prio_reg;

    SOCDNX_INIT_FUNC_DEFS;

    prio_reg = (core == 0) ? NBIH_RX_REQ_PIPE_0_TDM_ENr : NBIH_RX_REQ_PIPE_1_TDM_ENr;

    /* create mask */
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_tdm_high_low_create_mask(unit, ilkn_id, 1, 0, &mask));

    /* get modify set reg */
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, prio_reg, REG_PORT_ANY, 0, &reg32_val));
    reg32_val |= mask;
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, prio_reg, REG_PORT_ANY, 0, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_map_ilkn_to_hrf_reg
 * Purpose:
 *      maps an ilkn to its matching data hrf register
 * Parameters:
 *      unit    - Device Number
 *      ilkn_id - id of the ilkn port (0-5)
 *      hrf_reg - returned matching hrf data register.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_map_ilkn_to_hrf_reg(int unit, uint32 ilkn_id, soc_reg_t* hrf_reg)
{
    static soc_reg_t NBIH_RX_SCH_CONFIG_HRF_Nl[] = { NBIH_RX_SCH_CONFIG_HRFr, NBIH_RX_SCH_CONFIG_HRFr, NBIH_RX_SCH_CONFIG_HRF_4r, NBIH_RX_SCH_CONFIG_HRF_5r, NBIH_RX_SCH_CONFIG_HRF_8r, NBIH_RX_SCH_CONFIG_HRF_9r};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(hrf_reg);

    *hrf_reg = NBIH_RX_SCH_CONFIG_HRF_Nl[ilkn_id];

    exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_ilkn_high_low_clear
 * Purpose:
 *      clears high or low priorities for given ilkn.
 * Parameters:
 *      unit    - Device Number
 *      ilkn_id - id of the ilkn port (0-5)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_ilkn_high_low_clear(int unit, uint32 ilkn_id)
{

    uint32 priority_bits;
    uint32 pipe_id;
    uint32 is_high_priority;
    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 mask;
    soc_reg_above_64_val_t above64_mask;
    uint64 reg64_val;
    soc_reg_t priority_enable_reg;
    soc_reg_t sch_config_hrf_reg;

    int index = 0;

    SOCDNX_INIT_FUNC_DEFS;

    /* get relevant hrf to read what is the priority level, what are the priority bits, and what is the core/pipe id */
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_map_ilkn_to_hrf_reg(unit, ilkn_id, &sch_config_hrf_reg));
    index = ilkn_id == 1 ? 1 : 0;

    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, sch_config_hrf_reg, REG_PORT_ANY, index, &reg64_val));
    priority_bits = soc_reg64_field32_get(unit, sch_config_hrf_reg, reg64_val, HRF_RX_SCH_MAP_HRF_Nf);
    pipe_id = soc_reg64_field32_get(unit, sch_config_hrf_reg, reg64_val, HRF_RX_PIPE_Nf);
    is_high_priority = soc_reg64_field32_get(unit, sch_config_hrf_reg, reg64_val, HRF_RX_PRIORITY_HRF_Nf);
    soc_reg64_field32_set(unit, sch_config_hrf_reg, &reg64_val, HRF_RX_SCH_MAP_HRF_Nf, 0);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, sch_config_hrf_reg, REG_PORT_ANY, index, reg64_val));

    /* create mask */
    mask = ~priority_bits;

    /* clear priority bits */
    if (is_high_priority) 
    {
        priority_enable_reg = (pipe_id == 1) ? NBIH_RX_REQ_PIPE_1_HIGH_ENr : NBIH_RX_REQ_PIPE_0_HIGH_ENr;
        SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, priority_enable_reg, REG_PORT_ANY, 0, &reg32_val));
        reg32_val &= mask;
        SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, priority_enable_reg, REG_PORT_ANY, 0, reg32_val));
    } else {
        priority_enable_reg = (pipe_id == 1) ? NBIH_RX_REQ_PIPE_1_LOW_ENr : NBIH_RX_REQ_PIPE_0_LOW_ENr;
        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, priority_enable_reg, REG_PORT_ANY, 0, reg_above_64_val));
        SOC_REG_ABOVE_64_SET_WORD_PATTERN(above64_mask, mask);
        SOC_REG_ABOVE_64_AND(reg_above_64_val, above64_mask);
        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_set(unit, priority_enable_reg, REG_PORT_ANY, 0, reg_above_64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_ilkn_high_low_set
 * Purpose:
 *      sets high or low priorities for given ilkn.
 * Parameters:
 *      unit    - Device Number
 *      core    - core of the ilkn
 *      is_high - set to 1 if priority is high
 *      ilkn_id - id of the ilkn port (0-5)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_ilkn_high_low_set(int unit, int core, int is_high, uint32 ilkn_id)
{
    int index;
    uint32 mask;
    uint32 reg32_val;
    uint64 reg64_val;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t above64_mask;
    soc_reg_t priority_enable_reg;
    soc_reg_t sch_config_hrf_reg;

    SOCDNX_INIT_FUNC_DEFS;

    /* create mask */
    SOCDNX_IF_ERR_EXIT(soc_jer_nif_priority_ilkn_tdm_high_low_create_mask(unit, ilkn_id, 0, 1, &mask));

    /* get relevant hrf data register */
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_map_ilkn_to_hrf_reg(unit, ilkn_id, &sch_config_hrf_reg));
    index = (ilkn_id == 1) ? 1 : 0;

    /* update info in hrf regiter, relevant prio bits, pipe and high/low prio */
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, sch_config_hrf_reg, REG_PORT_ANY, index, &reg64_val));
    soc_reg64_field32_set(unit, sch_config_hrf_reg, &reg64_val, HRF_RX_SCH_MAP_HRF_Nf, mask);
    soc_reg64_field32_set(unit, sch_config_hrf_reg, &reg64_val, HRF_RX_PIPE_Nf, core);
    soc_reg64_field32_set(unit, sch_config_hrf_reg, &reg64_val, HRF_RX_PRIORITY_HRF_Nf, is_high);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, sch_config_hrf_reg, REG_PORT_ANY, index, reg64_val));

    /* set priority bits */
    if (is_high) 
    {
        priority_enable_reg = (core == 1) ? NBIH_RX_REQ_PIPE_1_HIGH_ENr : NBIH_RX_REQ_PIPE_0_HIGH_ENr; 
        SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, priority_enable_reg, REG_PORT_ANY, 0, &reg32_val));
        reg32_val |= mask;
        SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, priority_enable_reg, REG_PORT_ANY, 0, reg32_val));
    } else {
        priority_enable_reg = (core == 1) ? NBIH_RX_REQ_PIPE_1_LOW_ENr : NBIH_RX_REQ_PIPE_0_LOW_ENr; 
        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, priority_enable_reg, REG_PORT_ANY, 0, reg_above_64_val));
        SOC_REG_ABOVE_64_SET_WORD_PATTERN(above64_mask, mask);
        SOC_REG_ABOVE_64_OR(reg_above_64_val, above64_mask);
        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_set(unit, priority_enable_reg, REG_PORT_ANY, 0, reg_above_64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_set
 * Purpose:
 *      set nif priority
 * Parameters:
 *      unit            - Device Number
 *      core            - core of the ilkn
 *      quad_ilkn       - quad or ilkn id
 *      is_ilkn         - set to 1 if quad_ilkn is ilkn and not quad
 *      flags           - relevant flags
 *      allow_tdm       - allow configuring tdm
 *      priority_level  - tdm/high/low prio (2/1/0) 
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_nif_priority_set(   SOC_SAND_IN     int     unit,
                                SOC_SAND_IN     int     core,
                                SOC_SAND_IN     uint32  quad_ilkn,
                                SOC_SAND_IN     uint32  is_ilkn,
                                SOC_SAND_IN     uint32  flags,
                                SOC_SAND_IN     uint32  allow_tdm,
                                SOC_SAND_IN     int     priority_level)
{
    SOCDNX_INIT_FUNC_DEFS;

    if((priority_level < JER_NIF_PRIO_LOW_LEVEL) || (priority_level > JER_NIF_PRIO_HIGH_LEVEL)) 
    {
        if (priority_level != JER_NIF_PRIO_TDM_LEVEL) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("priority level %d is invalid for device"), priority_level)); 
        } else if ((priority_level == JER_NIF_PRIO_TDM_LEVEL) && (allow_tdm == 0)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("priority level %d can't be configured manually with this API"), priority_level)); 
        }
    }

    if (is_ilkn) 
    {
        /* Handle ILKN prio */
        if (priority_level == JER_NIF_PRIO_TDM_LEVEL) 
        {
            /* Clear existing configuration from both cores */
            SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_tdm_clear(unit, quad_ilkn));
            /* Set wanted configuration for relevant ILKN as TDM */
            SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_tdm_set(unit, core, quad_ilkn));
        } else {
            /* Clear existing configuration from both cores */
            SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_high_low_clear(unit, quad_ilkn));
            if (priority_level == JER_NIF_PRIO_HIGH_LEVEL)
            {
                /* Set wanted configuration for relevant ILKN as HIGH */
                SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_high_low_set(unit, core, 1, quad_ilkn));
            } else /* priority_level == JER_NIF_PRIO_LOW_LEVEL */ {
                /* Set wanted configuration for relevant ILKN as LOW */
                SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_high_low_set(unit, core, 0, quad_ilkn));
            }
        }
    } else {
        /* Handle Quad prio */
        if (priority_level == JER_NIF_PRIO_TDM_LEVEL) 
        {
            /* Clear existing configuration from both cores (TDM) */
            SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_tdm_high_low_clear(unit, quad_ilkn, 1, 0));
            /* Set wanted configuration for relevant Quad as TDM */
            SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_tdm_set(unit, core, quad_ilkn));
        } else {
            /* Clear existing configuration from both cores (HIGH and LOW) */
            SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_tdm_high_low_clear(unit, quad_ilkn, 0, 1));
            if (priority_level == JER_NIF_PRIO_HIGH_LEVEL)
            {
                /* Set wanted configuration for relevant Quad as HIGH */
                SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_high_set(unit, core, quad_ilkn));
            } else /* priority_level == JER_NIF_PRIO_LOW_LEVEL */ {
                /* Set wanted configuration for relevant Quad as LOW */
                SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_low_set(unit, core, quad_ilkn));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_quad_ilkn_tdm_get
 * Purpose:
 *      gets tdm priority for given quad/ilkn.
 * Parameters:
 *      unit        - Device Number
 *      core        - core
 *      quad_ilkn   - quad or ilkn id
 *      is_ilkn     - set to 1 if is ilkn
 *      is_set      - returns if tdm prio is set for quad/ilkn
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_quad_ilkn_tdm_get(int unit, int core, uint32 quad_ilkn, uint32 is_ilkn, uint32* is_set)
{

    uint32 mask;
    uint32 reg32_val;
    soc_reg_t tdm_en_prio_reg;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(is_set);

    /* creste mask */
    if (is_ilkn) {
        SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_tdm_high_low_create_mask(unit, quad_ilkn, 1, 0, &mask));
    } else {
        mask = 1 << quad_ilkn; 
    }

    /* see if any of the prio bits is set */
    tdm_en_prio_reg = (core == 1) ? NBIH_RX_REQ_PIPE_1_TDM_ENr : NBIH_RX_REQ_PIPE_0_TDM_ENr;
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, tdm_en_prio_reg, REG_PORT_ANY, 0, &reg32_val));
    *is_set = (reg32_val & mask) ? 1 : 0;

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_quad_ilkn_high_get
 * Purpose:
 *      gets high priority for given quad/ilkn.
 * Parameters:
 *      unit        - Device Number
 *      core        - core
 *      quad_ilkn   - quad or ilkn id
 *      is_ilkn     - set to 1 if is ilkn
 *      is_set      - returns if tdm prio is set for quad/ilkn
 * Returns:
 *      SOC_E_XXX
 */
STATIC int soc_jer_nif_priority_quad_ilkn_high_get(int unit, int core, uint32 quad_ilkn, uint32 is_ilkn, uint32* is_set)
{

    uint32 mask;
    uint32 reg32_val;
    soc_reg_t high_en_prio_reg;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(is_set);

    /* creste mask */
    if (is_ilkn) {
        SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_tdm_high_low_create_mask(unit, quad_ilkn, 0, 1, &mask));
    } else {
        mask = 1 << quad_ilkn; 
    }

    /* see if any of the prio bits is set */
    high_en_prio_reg = (core == 1) ? NBIH_RX_REQ_PIPE_1_HIGH_ENr : NBIH_RX_REQ_PIPE_0_HIGH_ENr;
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, high_en_prio_reg, REG_PORT_ANY, 0, &reg32_val));
    *is_set = (reg32_val & mask) ? 1 : 0;

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_quad_ilkn_low_get
 * Purpose:
 *      gets low priority for given quad/ilkn.
 * Parameters:
 *      unit        - Device Number
 *      core        - core
 *      quad_ilkn   - quad or ilkn id
 *      is_ilkn     - set to 1 if is ilkn
 *      is_set      - returns if tdm prio is set for quad/ilkn
 * Returns:
 *      SOC_E_XXX
 */

STATIC int soc_jer_nif_priority_quad_ilkn_low_get(int unit, int core, uint32 quad_ilkn, uint32 is_ilkn, uint32* is_set)
{
    uint32 mask;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t above64_mask;
    soc_reg_t low_en_prio_reg;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(is_set);

    /* creste mask */
    if (is_ilkn) {
        SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_ilkn_tdm_high_low_create_mask(unit, quad_ilkn, 0, 1, &mask));
    } else {
        mask = 1 << quad_ilkn; 
    }

    /* see if any of the prio bits is set */
    low_en_prio_reg = (core == 1) ? NBIH_RX_REQ_PIPE_1_LOW_ENr : NBIH_RX_REQ_PIPE_0_LOW_ENr;
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, low_en_prio_reg, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_SET_WORD_PATTERN(above64_mask, mask);
    SOC_REG_ABOVE_64_AND(reg_above_64_val, above64_mask);
    *is_set = (SOC_REG_ABOVE_64_IS_ZERO(reg_above_64_val)) ? 0 : 1;

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_nif_priority_get
 * Purpose:
 *      get nif priority
 * Parameters:
 *      unit            - Device Number
 *      core            - core of the ilkn
 *      quad_ilkn       - quad or ilkn id
 *      is_ilkn         - set to 1 if quad_ilkn is ilkn and not quad
 *      flags           - relevant flags
 *      allow_tdm       - allow configuring tdm
 *      priority_level  - returned prio level tdm/high/low (2/1/0)
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_nif_priority_get(   SOC_SAND_IN     int     unit,
                                SOC_SAND_IN     int     core,
                                SOC_SAND_IN     uint32  quad_ilkn,
                                SOC_SAND_IN     uint32  is_ilkn,
                                SOC_SAND_IN     uint32  flags,
                                SOC_SAND_IN     uint32  allow_tdm,
                                SOC_SAND_OUT    int*    priority_level)
{
    uint32 is_set;

    SOCDNX_INIT_FUNC_DEFS;

    /* set priority level with invalid value */
    *priority_level = -1;

    /* check if tdm prio when relevant flag is on */
    if ( allow_tdm ) {
        SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_ilkn_tdm_get(unit, core, quad_ilkn, is_ilkn, &is_set));
        if (is_set) {
            *priority_level = JER_NIF_PRIO_TDM_LEVEL;
            SOC_EXIT;
        }
    }

    /* check if high prio */
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_ilkn_high_get(unit, core, quad_ilkn, is_ilkn, &is_set));
    if (is_set) {
        *priority_level = JER_NIF_PRIO_HIGH_LEVEL;
        SOC_EXIT;
    }

    /* check if low prio */
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_ilkn_low_get(unit, core, quad_ilkn, is_ilkn, &is_set));
    if (is_set) {
        *priority_level = JER_NIF_PRIO_LOW_LEVEL;
        SOC_EXIT;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * How to get serdes frequency:
 * We can't measure the serdes freq directly, but we can measure the sync_eth counter, and reconstruct the serdes freq from it by multiplying it with the blocks dividers.
 *
 * In PML and PMH GSMII the dividers are:
 *  VCO                                                   Sync_eth counter
 *   _        --> PM synce_div --> NBIL/H synce div -->         _
 * _| |_                                                      _| |_
 * So 
 * VCO = Fsynce * PMH_40_PML_20 * PM_1_7_11
 * and we need to do:
 * SerDes_rate = VCO/Oversample = Fsynce * PMH_40_PML_20 * PM_1_7_11 / Oversample
 *
 *
 * In PMH which is not GSMII the dividers are:
 * Serdes freq                                            Sync_eth counter
 *   _         --> PM synce_div --> NBIL/H synce div -->        _
 * _| |_                                                      _| |_
 * So we need to do:
 * SerDes_rate = Fsynce * PMH_40_PML_20 * PM_1_7_11
 */


int
soc_jer_phy_nif_measure(int unit, soc_port_t port, uint32 *type_of_bit_clk, int *one_clk_time_measured_int, int *one_clk_time_measured_remainder, int *serdes_freq_int, int *serdes_freq_remainder, uint32 *lane)
{
    uint32 sync_sth_cnt;
    int serdes_freq_int_temp, serdes_freq_remainder_temp;
    int total_time_measured_int, total_time_measured_remainder;
    int number_of_gtimer_cycles = 10240;
    int clock_speed_int, clock_speed_remainder;
    int one_bit_clk_period_int, one_bit_clk_period_remainder;
    soc_dpp_config_arad_t *dpp_arad;
    uint32 reg_val, pmh_synce_rstn_prev = 0, synce_prev_config = 0;
    uint64 reg_val_64, default_configuration;
    int synce_read = 0, pmh_synce_read = 0;
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 os_int, os_remainder;
    int rv = SOC_E_NONE;
    uint16 lane_map;
    uint32 swapped_lane;
    uint32 synce_div = 2, vco_div;
    SOC_JER_NIF_PLL_TYPE pll_type;
    soc_port_if_t interface_type;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    int nof_phys, i, speed;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);

    rv = soc_jer_port_pll_type_get(unit, port, &pll_type);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_interface_type_get(unit, port, &interface_type);
    SOCDNX_IF_ERR_EXIT(rv);



    /* Save previous configuration to restore at exit */
    rv = READ_NBIH_GTIMER_CONFIGURATIONr(unit, &default_configuration);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Get physical lane
     *
     * in the range of 1-72 */
    if(SOC_IS_DIRECT_PORT(unit, port)) {
        *lane = port - SOC_INFO(unit).physical_port_offset;
    } else {
        *lane = SOC_INFO(unit).port_l2p_mapping[port]; /*lane should be 1-based*/
        
        if (*lane >= SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit) - 1)
        {
            *one_clk_time_measured_int = -1;
            SOC_EXIT;
        }
    }
    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove,
            (unit, *lane, lane));
    SOCDNX_IF_ERR_EXIT(rv);
    *lane -= 1; /*make lane 0-based*/

    /* get swapped lane map. ex: 2301 */
    lane_map = soc_property_suffix_num_get(unit, (*lane)/4, spn_PHY_RX_LANE_MAP, "quad", 0x3210) & 0xffff;
    /* find swapped number of lane (0-3). ex: if lane=14 -> swapped_lane=3 */
    swapped_lane = ((lane_map >> (((*lane)%4)*4)) & 0xf);
    /* add base of lane to get swapped_lane. ex: if lane=14 -> swapped_lane=14-2+3=15 */
    swapped_lane = (*lane)-((*lane)%4)+swapped_lane;

    rv = READ_NBIH_GTIMER_CONFIGURATIONr(unit, &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Config NBIH Gtimer */
    soc_reg64_field32_set(unit, NBIH_GTIMER_CONFIGURATIONr, &reg_val_64, GTIMER_ENABLEf, 0x1);
    soc_reg64_field32_set(unit, NBIH_GTIMER_CONFIGURATIONr, &reg_val_64, GTIMER_CYCLEf, number_of_gtimer_cycles);
    soc_reg64_field32_set(unit, NBIH_GTIMER_CONFIGURATIONr, &reg_val_64, GTIMER_RESET_ON_TRIGGERf, 0x0);
    rv = WRITE_NBIH_GTIMER_CONFIGURATIONr(unit, reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Enable synce */
    rv = READ_ECI_GP_CONTROL_9r(unit, reg_val_above_64);
    pmh_synce_read = 1;
    pmh_synce_rstn_prev = soc_reg_above_64_field32_get(unit, ECI_GP_CONTROL_9r, reg_val_above_64, PMH_SYNCE_RSTNf); /* Save configuration to restore at end of function */
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_val_above_64, PMH_SYNCE_RSTNf, 0x1);
    rv = WRITE_ECI_GP_CONTROL_9r(unit, reg_val_above_64);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Config synce */
    rv = READ_NBIH_SYNC_ETH_CFGr(unit, 0, &reg_val);
    SOCDNX_IF_ERR_EXIT(rv);
    synce_read = 1;
    synce_prev_config = reg_val; /* Save synce configuration to restore at end of function */
    soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_CLOCK_SEL_Nf, swapped_lane);
    soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_CLOCK_DIV_Nf, 0x1);
    soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_SQUELCH_EN_Nf, 0x1);
    soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_LINK_VALID_SEL_Nf, 0x1);
    soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_GTIMER_MODE_Nf, 0x1);
    rv = WRITE_NBIH_SYNC_ETH_CFGr(unit, 0, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_wait_gtimer_trigger, (unit));
    SOCDNX_IF_ERR_EXIT(rv);

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;
    if(0 == dpp_arad->init.core_freq.frequency){
        rv = SOC_E_INTERNAL;
        SOC_EXIT;
    }
    clock_speed_int = (1000000 / dpp_arad->init.core_freq.frequency);
    clock_speed_remainder = ((100000000 / (dpp_arad->init.core_freq.frequency / 100))) % 10000; /* devided by 100 instead of 10000 for better resolution */

    total_time_measured_int = (((clock_speed_int * 10000) + clock_speed_remainder) * number_of_gtimer_cycles) / 10000;
    total_time_measured_remainder = ((int)(((clock_speed_int * 10000) + clock_speed_remainder) * (number_of_gtimer_cycles)) % 10000);

    /* Read synce counter */
    rv = READ_NBIH_SYNC_ETH_COUNTERr(unit, 0, &sync_sth_cnt);
    SOCDNX_IF_ERR_EXIT(rv);


    rv = portmod_access_get_params_t_init(unit, &params);
    SOCDNX_IF_ERR_EXIT(rv);
    for (i = 0; i < SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT; ++i) {
        rv = phymod_phy_access_t_init(&phy_access[i]);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    params.phyn=0;
    rv = portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT,  phy_access, &nof_phys, NULL);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to get lane access: %s\n", bcm_errmsg(rv));
        goto exit;
    }

    if (IS_QSGMII_PORT(unit, port)) {
        if (phy_access[0].access.lane_mask < 0x10) {
            phy_access[0].access.lane_mask = 0x1;
        } else if (phy_access[0].access.lane_mask < 0x100) {
            phy_access[0].access.lane_mask = 0x2;
        } else if (phy_access[0].access.lane_mask < 0x1000) {
            phy_access[0].access.lane_mask = 0x4;
        } else if (phy_access[0].access.lane_mask < 0x10000) {
            phy_access[0].access.lane_mask = 0x8;
        }
    }

    if (pll_type != SOC_JER_NIF_PLL_TYPE_PMH) {
        BCMI_TSCE_XGXS_MAIN0_SYNCE_CTLr_t synce_ctrl;
        rv = BCMI_TSCE_XGXS_READ_MAIN0_SYNCE_CTLr(&phy_access[0].access, &synce_ctrl);
        SOCDNX_IF_ERR_EXIT(rv);
        if (phy_access[0].access.lane_mask & 0x1) {
            synce_div = BCMI_TSCE_XGXS_MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN0f_GET(synce_ctrl);
        } else if (phy_access[0].access.lane_mask & 0x2) {
            synce_div = BCMI_TSCE_XGXS_MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN1f_GET(synce_ctrl);
        } else if (phy_access[0].access.lane_mask & 0x4) {
            synce_div = BCMI_TSCE_XGXS_MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN2f_GET(synce_ctrl);
        } else if (phy_access[0].access.lane_mask & 0x8) {
            synce_div= BCMI_TSCE_XGXS_MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN3f_GET(synce_ctrl);
        } else {
            SOC_EXIT;
        }
    } else {
        BCMI_TSCF_XGXS_MAIN0_SYNCE_CTLr_t synce_ctrl;
        rv = BCMI_TSCF_XGXS_READ_MAIN0_SYNCE_CTLr(&phy_access[0].access, &synce_ctrl);
        SOCDNX_IF_ERR_EXIT(rv);
        if (phy_access[0].access.lane_mask & 0x1) {
            synce_div = BCMI_TSCF_XGXS_MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE0f_GET(synce_ctrl);
        } else if (phy_access[0].access.lane_mask & 0x2) {
            synce_div = BCMI_TSCF_XGXS_MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE1f_GET(synce_ctrl);
        } else if (phy_access[0].access.lane_mask & 0x4) {
            synce_div = BCMI_TSCF_XGXS_MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE2f_GET(synce_ctrl);
        } else if (phy_access[0].access.lane_mask & 0x8) {
            synce_div = BCMI_TSCF_XGXS_MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE3f_GET(synce_ctrl);
        } else {
            SOC_EXIT;
        }
    }

    if (2 == synce_div) {
        *type_of_bit_clk = 11;
    } else if (1 == synce_div) {
        *type_of_bit_clk = 7;
    } else {
        *type_of_bit_clk = 1;
    }

    rv = READ_NBIH_GTIMER_CONFIGURATIONr(unit, &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg64_field32_set(unit, NBIH_GTIMER_CONFIGURATIONr, &reg_val_64, GTIMER_ENABLEf, 0x0);
    rv = WRITE_NBIH_GTIMER_CONFIGURATIONr(unit, reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    /* VCO devider is 40 for PMH and 20 for PML */
    (pll_type == SOC_JER_NIF_PLL_TYPE_PMH) ? (vco_div = 40) : (vco_div = 20);

    /* Get serdes_freq from total_time_measured by removing all the dividers between them */
    if(0 == sync_sth_cnt){
        rv = SOC_E_INTERNAL;
        SOC_EXIT;
    }
    *one_clk_time_measured_int = (((total_time_measured_int*10000) + (total_time_measured_remainder)) / (sync_sth_cnt * vco_div)) / 10000;
    *one_clk_time_measured_remainder = ((((total_time_measured_int*10000) + (total_time_measured_remainder)) / (sync_sth_cnt * vco_div)) % 10000);
    if(0 == *type_of_bit_clk){
        rv = SOC_E_INTERNAL;
        SOC_EXIT;
    }
    one_bit_clk_period_int = ((*one_clk_time_measured_int*10000 + *one_clk_time_measured_remainder) / *type_of_bit_clk) / 10000;
    one_bit_clk_period_remainder = (((*one_clk_time_measured_int*10000 + *one_clk_time_measured_remainder) / *type_of_bit_clk) % 10000);
    if(0 == (one_bit_clk_period_int*10000 + one_bit_clk_period_remainder)){
        rv = SOC_E_INTERNAL;
        SOC_EXIT;
    }

    serdes_freq_int_temp = (10000/(one_bit_clk_period_int*10000 + one_bit_clk_period_remainder));
    serdes_freq_remainder_temp = (10000000/(one_bit_clk_period_int*10000 + one_bit_clk_period_remainder)) % 1000;

    /* Get oversample */
    rv = soc_jer_portmod_calc_os(unit, &phy_access[0], &os_int, &os_remainder);
    SOCDNX_IF_ERR_EXIT(rv);
    
    /*  The HW is connected in a way that the oversample affects the SyncE->SerDesRate calculation
        only on PML and PMH SGMII. Otherwise - we should ignore it ! */
    if ((pll_type == SOC_JER_NIF_PLL_TYPE_PMH) && (interface_type != BCM_PORT_IF_SGMII)){
        os_int = 1;
        os_remainder = 0;
    }

    *serdes_freq_int = ((((serdes_freq_int_temp * 1000) + serdes_freq_remainder_temp) * 1000) / ((os_int * 1000) + os_remainder)) / 1000;
    *serdes_freq_remainder = ((((serdes_freq_int_temp * 1000) + serdes_freq_remainder_temp) * 1000) / ((os_int * 1000) + os_remainder)) % 1000;

    rv = bcm_port_speed_get(unit, port, &speed);
    SOCDNX_IF_ERR_EXIT(rv);

    if (speed == 100) {
        *serdes_freq_int = 0;
        *serdes_freq_remainder = *serdes_freq_remainder/10 + 100;
    }

    if (speed == 10) {
        *serdes_freq_int = 0;
        *serdes_freq_remainder = *serdes_freq_remainder/100 + 10;
    }



exit:

    if (unit < SOC_MAX_NUM_DEVICES) {
        if(WRITE_NBIH_GTIMER_CONFIGURATIONr(unit, default_configuration) != SOC_E_NONE) {
            cli_out("WRITE_NBIH_GTIMER_CONFIGURATIONr failed\n");
        }

        if (pmh_synce_read) {
            if (READ_ECI_GP_CONTROL_9r(unit, reg_val_above_64) == SOC_E_NONE) {
                soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_val_above_64, PMH_SYNCE_RSTNf, pmh_synce_rstn_prev);
                if(WRITE_ECI_GP_CONTROL_9r(unit, reg_val_above_64) != SOC_E_NONE) {
                    cli_out("WRITE_ECI_GP_CONTROL_9r failed\n");
                }
            }
        }

        if (synce_read) {
            WRITE_NBIH_SYNC_ETH_CFGr(unit, 0, synce_prev_config);
        }
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_jer_phy_nif_pll_div_get(int unit, soc_port_t port, soc_dcmn_init_serdes_ref_clock_t *ref_clk, int *p_div, int *n_div, int *m0_div)
{
    int rv = SOC_E_NONE;
    SOC_JER_NIF_PLL_TYPE pll_type;
    soc_reg_above_64_val_t reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_jer_port_pll_type_get(unit, port, &pll_type);
    SOCDNX_IF_ERR_EXIT(rv);

    if (pll_type == SOC_JER_NIF_PLL_TYPE_PMH) {
        rv = READ_ECI_NIF_PMH_PLL_CONFIGr(unit, reg_val);
        SOCDNX_IF_ERR_EXIT(rv);

        *n_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PMH_PLL_FBDIV_NDIV_INTf : NIF_PMH_PLL_CFG_NDIVf));
        *p_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PMH_PLL_PDIVf : NIF_PMH_PLL_CFG_PDIVf));
        *m0_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PMH_PLL_CH_0_MDIVf : NIF_PMH_PLL_CFG_CH_0_MDIVf));
        *ref_clk = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmh_in;

    } else if (pll_type == SOC_JER_NIF_PLL_TYPE_PML0) {
        rv = READ_ECI_NIF_PML_0_PLL_CONFIGr(unit, reg_val);
        SOCDNX_IF_ERR_EXIT(rv);

        *n_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PML_0_PLL_FBDIV_NDIV_INTf : NIF_PML_0_PLL_CFG_NDIVf));
        *p_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PML_0_PLL_PDIVf : NIF_PML_0_PLL_CFG_PDIVf));
        *m0_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PML_0_PLL_CH_0_MDIVf : NIF_PML_0_PLL_CFG_CH_0_MDIVf));
        *ref_clk = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_in[0];

    } else { /* pll_type == SOC_JER_NIF_PLL_TYPE_PML1 */
        rv = READ_ECI_NIF_PML_1_PLL_CONFIGr(unit, reg_val);
        SOCDNX_IF_ERR_EXIT(rv);

        *n_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PML_1_PLL_FBDIV_NDIV_INTf : NIF_PML_1_PLL_CFG_NDIVf));
        *p_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PML_1_PLL_PDIVf : NIF_PML_1_PLL_CFG_PDIVf));
        *m0_div = soc_reg_above_64_field32_get(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_val, (SOC_IS_QAX(unit) ? PML_1_PLL_CH_0_MDIVf : NIF_PML_1_PLL_CFG_CH_0_MDIVf));
        *ref_clk = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_in[1];
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
jer_nif_ilkn_counter_clear(int unit, soc_port_t port) {

    uint32 channel, reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));

    SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_ACCr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_TYPEf, 0x0);
    soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_CMDf, 0x2);
    soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_ADDRf, channel);
    SOCDNX_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_STATS_ACCr(unit, port, reg_val));


    SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_ACCr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_TYPEf, 0x0);
    soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_CMDf, 0x2);
    soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_ADDRf, channel);
    SOCDNX_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_STATS_ACCr(unit, port, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  jer_nif_ilkn_counter_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  soc_jer_counters_t       counter_type,
    SOC_SAND_OUT uint64                   *counter_val
  )
{
    uint32 channel, ctr_low, ctr_high, reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(*counter_val);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));

    switch (counter_type) {
    case soc_jer_counters_ilkn_rx_pkt_counter:
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_CMDf, 0x1);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_ADDRf, channel);
        SOCDNX_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_STATS_ACCr(unit, port, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_RD_PKT_LOWr(unit, port, &ctr_low));
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_RD_PKT_HIGHr(unit, port, &ctr_high));
        COMPILER_64_SET(*counter_val, ctr_high, ctr_low);
        break;
    case soc_jer_counters_ilkn_tx_pkt_counter:
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_CMDf, 0x1);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_ADDRf, channel);
        SOCDNX_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_STATS_ACCr(unit, port, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_RD_PKT_LOWr(unit, port, &ctr_low));
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_RD_PKT_HIGHr(unit, port, &ctr_high));
        COMPILER_64_SET(*counter_val, ctr_high, ctr_low);
        break;
    case soc_jer_counters_ilkn_rx_byte_counter:
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_CMDf, 0x1);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_ADDRf, channel);
        SOCDNX_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_STATS_ACCr(unit, port, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_RD_BYTE_LOWr(unit, port, &ctr_low));
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_RD_BYTE_HIGHr(unit, port, &ctr_high));
        COMPILER_64_SET(*counter_val, ctr_high, ctr_low);
        break;
    case soc_jer_counters_ilkn_tx_byte_counter:
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_CMDf, 0x1);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_ADDRf, channel);
        SOCDNX_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_STATS_ACCr(unit, port, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_RD_BYTE_LOWr(unit, port, &ctr_low));
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_RD_BYTE_HIGHr(unit, port, &ctr_high));
        COMPILER_64_SET(*counter_val, ctr_high, ctr_low);
        break;
    case soc_jer_counters_ilkn_rx_err_pkt_counter:
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_CMDf, 0x1);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_ADDRf, channel);
        SOCDNX_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_STATS_ACCr(unit, port, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_RD_ERR_LOWr(unit, port, &ctr_low));
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_RD_ERR_HIGHr(unit, port, &ctr_high));
        COMPILER_64_SET(*counter_val, ctr_high, ctr_low);
        break;
    case soc_jer_counters_ilkn_tx_err_pkt_counter:
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_CMDf, 0x1);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_ADDRf, channel);
        SOCDNX_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_STATS_ACCr(unit, port, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_RD_ERR_LOWr(unit, port, &ctr_low));
        SOCDNX_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_RD_ERR_HIGHr(unit, port, &ctr_high));
        COMPILER_64_SET(*counter_val, ctr_high, ctr_low);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Counter type %d is invalid"), counter_type)); 
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int
  soc_jer_nif_port_rx_enable_get(
    int                         unit,
    soc_port_t                  port,
    int                         *enable
  )
{

    int flags;
    SOCDNX_INIT_FUNC_DEFS;
    flags = PORTMOD_PORT_ENABLE_RX;

   SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, flags, enable));

exit:
    SOCDNX_FUNC_RETURN;;

}

int
  soc_jer_nif_port_rx_enable_set(
    int                         unit,
    soc_port_t                  port,
    int                       enable
  )
{

    int flags;
    SOCDNX_INIT_FUNC_DEFS;
    flags = PORTMOD_PORT_ENABLE_RX;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, flags, enable));

exit:
    SOCDNX_FUNC_RETURN;

}

int
  soc_jer_nif_port_tx_enable_get(
    int                         unit,
    soc_port_t                  port,
    int                         *enable
  )
{

    int flags;
    SOCDNX_INIT_FUNC_DEFS;
    flags = PORTMOD_PORT_ENABLE_TX;

   SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, flags, enable));

exit:
    SOCDNX_FUNC_RETURN;;

}

int
  soc_jer_nif_port_tx_enable_set(
    int                         unit,
    soc_port_t                  port,
    int                       enable
  )
{

    int flags;
    SOCDNX_INIT_FUNC_DEFS;
    flags = PORTMOD_PORT_ENABLE_TX;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, flags, enable));

exit:
    SOCDNX_FUNC_RETURN;

}

int soc_jer_port_is_pcs_loopback(int unit, soc_port_t port, int *result)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_loopback_get(unit, port, portmodLoopbackPhyGloopPCS, result));

    exit:
        SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_enable_set(int unit, soc_port_t port, uint32 flags, int enable)
{
    int rv, phy, first_phy=-1;
    soc_reg_above_64_val_t reg_prd_config;
    uint32 reg_val, qmlf_index, field, prd_mode=0, is_hg, en_bit, en_mask=0, nof_lanes_nbi;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    soc_pbmp_t nif_ports, phys;

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type);
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_get(unit, port, &is_hg));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        if (first_phy < 0)
        {
            first_phy = (phy-1);
        }

        en_bit = 1 << ( (phy-1) % nof_lanes_nbi);
        en_mask |= en_bit;
    }

    qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;

    if (enable) /* Enable PRD */
    {
        if (is_hg)
        {
            prd_mode = JER_SOC_PRD_MODE_HIGIG;
        }
        else
        {
            switch(hdr_type)
            {
            case SOC_TMC_PORT_HEADER_TYPE_ETH:
                prd_mode = JER_SOC_PRD_MODE_VLAN;
                break;

            case SOC_TMC_PORT_HEADER_TYPE_TM:
                prd_mode = JER_SOC_PRD_MODE_ITMH;
                break;

            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported header type\n")));
            }
        }

        if (first_phy < nof_lanes_nbi)  /* NBIH */
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_ENr(unit, &reg_val));
            field =  soc_reg_field_get(unit, NBIH_PRD_ENr, reg_val, PRD_ENf);
            soc_reg_field_set(unit, NBIH_PRD_ENr, &reg_val, PRD_ENf, field | en_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_PRD_ENr(unit, reg_val));

            SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_CONFIGr(unit, qmlf_index, reg_prd_config));
            soc_reg_above_64_field32_set(unit, NBIH_PRD_CONFIGr, reg_prd_config, PRD_PKT_TYPE_QMLF_Nf, prd_mode);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_PRD_CONFIGr(unit, qmlf_index, reg_prd_config));

        }
        else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, 0, &reg_val));
            field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
            soc_reg_field_set(unit, NBIL_PRD_ENr, &reg_val, PRD_ENf, field | en_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_ENr(unit, 0, reg_val));

            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 0, qmlf_index, reg_prd_config));
            soc_reg_above_64_field32_set(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_PKT_TYPE_QMLF_Nf, prd_mode);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIGr(unit, 0, qmlf_index, reg_prd_config));
        }
        else /* NBIL1 */
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, 1, &reg_val));
            field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
            soc_reg_field_set(unit, NBIL_PRD_ENr, &reg_val, PRD_ENf, field | en_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_ENr(unit, 1, reg_val));

            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 1, qmlf_index, reg_prd_config));
            soc_reg_above_64_field32_set(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_PKT_TYPE_QMLF_Nf, prd_mode);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIGr(unit, 1, qmlf_index, reg_prd_config));
        }
    }
    else /* Disable PRD */
    {
        if (first_phy < nof_lanes_nbi)  /* NBIH */
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_ENr(unit, &reg_val));
            field =  soc_reg_field_get(unit, NBIH_PRD_ENr, reg_val, PRD_ENf);
            soc_reg_field_set(unit, NBIH_PRD_ENr, &reg_val, PRD_ENf, field & ~en_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_PRD_ENr(unit, reg_val));
        }
        else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, 0, &reg_val));
            field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
            soc_reg_field_set(unit, NBIL_PRD_ENr, &reg_val, PRD_ENf, field & ~en_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_ENr(unit, 0, reg_val));
        }
        else /* NBIL1 */
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, 1, &reg_val));
            field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
            soc_reg_field_set(unit, NBIL_PRD_ENr, &reg_val, PRD_ENf, field & ~en_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_ENr(unit, 1, reg_val));
        }
    }

    exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_enable_get(int unit, soc_port_t port, uint32 flags, int *enable)
{
    int phy, first_phy=-1;
    uint32 reg_val, field=0, nof_lanes_nbi, en_bit, en_mask=0;
    soc_pbmp_t nif_ports, phys;

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        if (first_phy < 0)
        {
            first_phy = (phy-1);
        }

        en_bit = 1 << ( (phy-1) % nof_lanes_nbi);
        en_mask |= en_bit;
    }

    if (first_phy < nof_lanes_nbi)  /* NBIH */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_ENr(unit, &reg_val));
        field =  soc_reg_field_get(unit, NBIH_PRD_ENr, reg_val, PRD_ENf);

    }
    else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, 0, &reg_val));
        field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
    }
    else /* NBIL1 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, 1, &reg_val));
        field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
    }

    *enable = (field & en_mask) ? 1 : 0;

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_config_set(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config)
{
    int phy, first_phy=-1;
    soc_reg_above_64_val_t reg_prd_config;
    uint32 qmlf_index, nof_lanes_nbi;
    uint32 untagged_pcp;
    soc_pbmp_t nif_ports, phys;

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        first_phy = (phy-1);
        break;
    }

    qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;
    untagged_pcp = config->untagged_pcp;

    if (first_phy < nof_lanes_nbi)  /* NBIH */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_CONFIGr(unit, qmlf_index, reg_prd_config));
        soc_reg_above_64_field32_set(unit, NBIH_PRD_CONFIGr, reg_prd_config, PRD_UNTAG_PCP_QMLF_Nf, untagged_pcp);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_PRD_CONFIGr(unit, qmlf_index, reg_prd_config));

    }
    else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 0, qmlf_index, reg_prd_config));
        soc_reg_above_64_field32_set(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_UNTAG_PCP_QMLF_Nf, untagged_pcp);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIGr(unit, 0, qmlf_index, reg_prd_config));
    }
    else /* NBIL1 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 1, qmlf_index, reg_prd_config));
        soc_reg_above_64_field32_set(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_UNTAG_PCP_QMLF_Nf, untagged_pcp);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIGr(unit, 1, qmlf_index, reg_prd_config));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_config_get(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config)
{
    int phy, first_phy=-1;
    soc_reg_above_64_val_t reg_prd_config;
    uint32 qmlf_index, nof_lanes_nbi, untagged_pcp;
    soc_pbmp_t nif_ports, phys;

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        first_phy = (phy-1);
        break;
    }

    qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;

    if (first_phy < nof_lanes_nbi)  /* NBIH */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_CONFIGr(unit, qmlf_index, reg_prd_config));
        untagged_pcp = soc_reg_above_64_field32_get(unit, NBIH_PRD_CONFIGr, reg_prd_config, PRD_UNTAG_PCP_QMLF_Nf);
    }
    else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 0, qmlf_index, reg_prd_config));
        untagged_pcp = soc_reg_above_64_field32_get(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_UNTAG_PCP_QMLF_Nf);
    }
    else /* NBIL1 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 1, qmlf_index, reg_prd_config));
        untagged_pcp = soc_reg_above_64_field32_get(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_UNTAG_PCP_QMLF_Nf);
    }

    config->untagged_pcp = untagged_pcp;

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_port_prd_threshold_set(int unit, soc_port_t port, uint32 flags, soc_color_t color, uint32 value)
{
    int phy, first_phy=-1;
    uint64 reg_val64;
    uint32 qmlf_index, nof_lanes_nbi, field;
    soc_pbmp_t nif_ports, phys;

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        first_phy = (phy-1);
        break;
    }

    qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;

    switch (color)
    {
    case socColorGreen:
        field = RX_PRD_THRESHOLD_0_QMLF_Nf;
        break;
    case socColorYellow:
        field = RX_PRD_THRESHOLD_1_QMLF_Nf;
        break;
    case socColorRed:
        field = RX_PRD_THRESHOLD_2_QMLF_Nf;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported color type\n")));
    }

    if (first_phy < nof_lanes_nbi)  /* NBIH */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, qmlf_index, &reg_val64));
        soc_reg64_field32_set(unit, NBIH_RX_MLF_PRD_THRESHOLDS_CONFIGr, &reg_val64, field, value);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, qmlf_index, reg_val64));

    }
    else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, 0, qmlf_index, &reg_val64));
        soc_reg64_field32_set(unit, NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr, &reg_val64, field, value);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, 0, qmlf_index, reg_val64));
    }
    else /* NBIL1 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, 1, qmlf_index, &reg_val64));
        soc_reg64_field32_set(unit, NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr, &reg_val64, field, value);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, 1, qmlf_index, reg_val64));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_threshold_get(int unit, soc_port_t port, uint32 flags, soc_color_t color, uint32 *value)
{
    int phy, first_phy=-1;
    uint64 reg_val64;
    uint32 qmlf_index, nof_lanes_nbi, field, th_value;
    soc_pbmp_t nif_ports, phys;

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        first_phy = (phy-1);
        break;
    }

    qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;

    switch (color)
    {
    case socColorGreen:
        field = RX_PRD_THRESHOLD_0_QMLF_Nf;
        break;
    case socColorYellow:
        field = RX_PRD_THRESHOLD_1_QMLF_Nf;
        break;
    case socColorRed:
        field = RX_PRD_THRESHOLD_2_QMLF_Nf;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported color type\n")));
    }

    if (first_phy < nof_lanes_nbi)  /* NBIH */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, qmlf_index, &reg_val64));
        th_value = soc_reg64_field32_get(unit, NBIH_RX_MLF_PRD_THRESHOLDS_CONFIGr, reg_val64, field);

    }
    else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, 0, qmlf_index, &reg_val64));
        th_value = soc_reg64_field32_get(unit, NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr, reg_val64, field);
    }
    else /* NBIL1 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr(unit, 1, qmlf_index, &reg_val64));
        th_value = soc_reg64_field32_get(unit, NBIL_RX_MLF_PRD_THRESHOLDS_CONFIGr, reg_val64, field);
    }

    *value = th_value;

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_map_set(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, soc_color_t  color)
{
    int phy, first_phy=-1;
    uint32 qmlf_index, nof_lanes_nbi, color_val;
    soc_reg_above_64_val_t reg_prd_config;
    soc_pbmp_t nif_ports, phys;
    int key_tbl_index;
    soc_reg_above_64_val_t prio_map;
    SOCDNX_INIT_FUNC_DEFS;

    switch (map) {
    case socDppPrdEthPcpDeiToPriorityTable:
    case socDppPrdTmTcDpPriorityTable:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Priority drop map %d is invalid"), map)); 
        break;
    }
    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        first_phy = (phy-1);
        break;
    }

    qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;
        
    switch (color) {
        case socColorGreen:
        color_val = 0;
            break;
        case socColorYellow:
        color_val = 1;
            break;
        case socColorRed:
        color_val = 2;
            break;
        case socColorBlack:
        color_val = 3;
            break;
        default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Color %d is invalid"), color)); 
    }

    key_tbl_index = key * JER_SOC_PRD_MAP_BITS_PER_COLOR;

    if (first_phy < nof_lanes_nbi)  /* NBIH */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_CONFIGr(unit, qmlf_index, reg_prd_config));
        soc_reg_above_64_field_get(unit, NBIH_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
        SOC_REG_ABOVE_64_RANGE_COPY(prio_map, key_tbl_index, &color_val, 0, JER_SOC_PRD_MAP_BITS_PER_COLOR); 
        soc_reg_above_64_field_set(unit, NBIH_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_PRD_CONFIGr(unit, qmlf_index, reg_prd_config));
    }
    else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 0, qmlf_index, reg_prd_config));
        soc_reg_above_64_field_get(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
        SOC_REG_ABOVE_64_RANGE_COPY(prio_map, key_tbl_index, &color_val, 0, JER_SOC_PRD_MAP_BITS_PER_COLOR);
        soc_reg_above_64_field_set(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIGr(unit, 0, qmlf_index, reg_prd_config));
    }
    else /* NBIL1 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 1, qmlf_index, reg_prd_config));
        soc_reg_above_64_field_get(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
        SOC_REG_ABOVE_64_RANGE_COPY(prio_map, key_tbl_index, &color_val, 0, JER_SOC_PRD_MAP_BITS_PER_COLOR);
        soc_reg_above_64_field_set(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIGr(unit, 1, qmlf_index, reg_prd_config));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_map_get(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, soc_color_t  *color)
{
    int phy, first_phy=-1;
    uint32 qmlf_index, nof_lanes_nbi, color_val = 0;
    soc_reg_above_64_val_t reg_prd_config;
    soc_pbmp_t nif_ports, phys;
    int key_tbl_index; 
    soc_reg_above_64_val_t prio_map;
    SOCDNX_INIT_FUNC_DEFS;

    switch (map) {
    case socDppPrdEthPcpDeiToPriorityTable:
    case socDppPrdTmTcDpPriorityTable:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Priority drop map %d is invalid"), map)); 
        break;
    }

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        first_phy = (phy-1);
        break;
    }

    qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;

    if (first_phy < nof_lanes_nbi)  /* NBIH */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_CONFIGr(unit, qmlf_index, reg_prd_config));
        soc_reg_above_64_field_get(unit, NBIH_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
    }
    else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 0, qmlf_index, reg_prd_config));
        soc_reg_above_64_field_get(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
    }
    else /* NBIL1 */
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIGr(unit, 1, qmlf_index, reg_prd_config));
        soc_reg_above_64_field_get(unit, NBIL_PRD_CONFIGr, reg_prd_config, PRD_PRIO_MAP_QMLF_Nf, prio_map);
    }

    key_tbl_index = key * JER_SOC_PRD_MAP_BITS_PER_COLOR;

    SOC_REG_ABOVE_64_RANGE_COPY(&color_val, 0, prio_map, key_tbl_index, JER_SOC_PRD_MAP_BITS_PER_COLOR);
    
    switch (color_val) {
        case 0:
        *color = socColorGreen;
            break;
        case 1:
        *color = socColorYellow;
            break;
        case 2:
        *color = socColorRed;
            break;
        case 3:
        *color = socColorBlack;
            break;
        default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Color %d is invalid"), color_val)); 
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_drop_count_get(int unit, soc_port_t port, uint32 *count)
{
    int phy, first_phy=-1;
    uint32 reg_val;
    uint32 nof_lanes_nbi, lane;
    soc_pbmp_t nif_ports, phys;
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        first_phy = (phy-1);
        break;
    }

    /*qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;*/

    if (first_phy < nof_lanes_nbi)  /* NBIH */
    {
        lane = first_phy;
        SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_PKT_DROP_CNT_PORTr_REG32(unit, lane, count));
    }
    else if ( first_phy < 2*nof_lanes_nbi )  /* NBIL0 */
    {
        lane = first_phy - nof_lanes_nbi;
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_PRD_PKT_DROP_CNTr(unit, 0, lane, &reg_val));
        *count = soc_reg_field_get(unit, NBIL_RX_PRD_PKT_DROP_CNTr, reg_val, PRD_PKT_DROP_CNT_PORT_Nf);
    }
    else /* NBIL1 */
    {
        lane = first_phy - 2*nof_lanes_nbi;
        SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_PRD_PKT_DROP_CNTr(unit, 1, lane, &reg_val));
        *count = soc_reg_field_get(unit, NBIL_RX_PRD_PKT_DROP_CNTr, reg_val, PRD_PKT_DROP_CNT_PORT_Nf);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_tpid_set(int unit, uint32 flags, int index, uint16 tpid)
{
    uint32 reg_val;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    field = (index == 1 ? PRD_VLAN_ETHERTYPE_1f : PRD_VLAN_ETHERTYPE_2f);

    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_PRD_VLAN_ETHERTYPEr(unit, &reg_val));
    soc_reg_field_set(unit, NBIH_RX_PRD_VLAN_ETHERTYPEr, &reg_val, field, (uint32)tpid);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_PRD_VLAN_ETHERTYPEr(unit, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_PRD_VLAN_ETHERTYPEr(unit, 0, &reg_val));
    soc_reg_field_set(unit, NBIL_RX_PRD_VLAN_ETHERTYPEr, &reg_val, field, (uint32)tpid);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_PRD_VLAN_ETHERTYPEr(unit, 0, reg_val));


    SOCDNX_IF_ERR_EXIT(READ_NBIL_RX_PRD_VLAN_ETHERTYPEr(unit, 1, &reg_val));
    soc_reg_field_set(unit, NBIL_RX_PRD_VLAN_ETHERTYPEr, &reg_val, field, (uint32)tpid);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIL_RX_PRD_VLAN_ETHERTYPEr(unit, 1, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_prd_tpid_get(int unit, uint32 flags, int index, uint16 *tpid)
{
    uint32 reg_val, field_val;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    field = (index == 1 ? PRD_VLAN_ETHERTYPE_1f : PRD_VLAN_ETHERTYPE_2f);

    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_PRD_VLAN_ETHERTYPEr(unit, &reg_val));
    field_val = soc_reg_field_get(unit, NBIH_RX_PRD_VLAN_ETHERTYPEr, reg_val, field);

    *tpid = (uint16)field_val;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_nif_qsgmii_pbmp_get(int unit, soc_port_t port, uint32 id, soc_pbmp_t *phy_pbmp)
{
    int first_phy;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*phy_pbmp);
    if(id < 32) {
        first_phy = id  + 53;
    } else {
        first_phy = id - 32 + 113;
    }

    SOC_PBMP_PORT_ADD(*phy_pbmp, first_phy);

    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

