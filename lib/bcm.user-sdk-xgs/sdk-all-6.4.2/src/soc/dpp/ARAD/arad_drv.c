/*
 * $Id: arad_drv.c, Modified Broadcom SDK $
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
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*includes*/
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
#include <soc/ipoll.h>
#include <soc/linkctrl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/fabric.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/error.h>
#include <soc/dpp/fabric.h>
#include <soc/dpp/port_map.h>
#include <soc/dpp/ARAD/arad_drv.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_stat.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_scheduler_device.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/ARAD/arad_link.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_stat_if.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_wb_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>
#include <soc/dpp/PPC/ppc_api_lif.h>
#include <soc/dpp/PPC/ppc_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_frwrd_extend_p2p.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/PORT/arad_ps_db.h>
#include <soc/shmoo_ddr40.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>

#include <soc/dpp/PPC/ppc_api_profile_mgmt.h> 
#include <soc/dpp/mbcm_pp.h>
#include <soc/dcmn/dcmn_cmic.h>
#include <soc/dcmn/dcmn_mem.h>

/*ARAD ACP*/
soc_driver_t soc_driver_acp = {
    /* type                   */    SOC_CHIP_ACP,
    /* chip_string            */    "acp",
    /* origin                 */    "",
    /* pci_vendor             */    ACP_PCI_VENDOR_ID,
    /* pci_device             */    ACP_PCI_DEVICE_ID,
    /* pci_revision           */    ACP_PCI_REV_ID,
    /* num_cos                */    0,
    /* reg_info               */    NULL,
    /* reg_unique_acc         */    NULL,
    /* reg_above_64_info      */    NULL,
    /* reg_array_info         */    NULL,
    /* mem_info               */    NULL,
    /* mem_unique_acc         */    NULL,
    /* mem_aggr               */    NULL,
    /* mem_array_info         */    NULL,
    /* block_info             */    NULL,
    /* port_info              */    NULL,
    /* counter_maps           */    NULL,
    /* features               */    NULL,
    /* init                   */    NULL,
    /* services               */    NULL,
    /* port_num_blktype       */    1,
    /* cmicd_base             */    0x00000000
};  /* soc_driver             */

/* ARAD DRV defines */
#define SOC_DPP_ARAD_DEFAULT_TDM_SOURCE_FAP_ID_OFFSET       (256)
#define SOC_DPP_FIRST_SFI_PHY_PORT(unit)                    (SOC_IS_JERICHO(unit) ? 192 : 40)
#define SOC_DPP_ARAD_NUM_CPU_COSQ                           (64) 
#define SOC_DPP_ARAD_OAM_DEFAULT_MIRROR_PROFILE             (5) /* default profile should be above outbound mirror profiles and below OAM allocated profiles */

/* SBUS defines */
#ifdef BCM_SBUSDMA_SUPPORT
#define SOC_DPP_ARAD_MAX_SBUSDMA_CHANEELS    (3)
#define SOC_DPP_ARAD_TDMA_CHANNEL            (0)
#define SOC_DPP_ARAD_TSLAM_CHANNEL           (1)
#define SOC_DPP_ARAD_DESC_CHANNEL            (2)
#define SOC_DPP_MEM_CLEAR_CHUNK_SIZE          4 /* Use one entry buffers for SLAM DMA */
#endif /*BCM_SBUSDMA_SUPPORT*/

/* Interrupts defines */
#define SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_2_MASK 0x1e
#define SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_3_MASK 0xffffffff
#define SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_4_MASK 0xffffffff

#define JER_DEFAULT_NOF_MC_GROUPS (80 * 1024) /* default number of Jericho MC groups in dual core mode */
#define JER_MAX_NOF_MC_GROUPS (128 * 1024) /* maximum number of Jericho MC groups in dual core mode */

/* ARAD DRV Enums */
enum
{
  SOC_DPP_ARAD_PCI_CMC  = 0,
  SOC_DPP_ARAD_ARM1_CMC = 1,
  SOC_DPP_ARAD_ARM2_CMC = 2,
  SOC_DPP_ARAD_NUM_CMCS = 3
};

/*Arad Plus Specific*/
#ifdef BCM_88660_A0
#define EXT_MODE_4LANES_PHY_PORTS ((1<<11) | (1<<12) | (1<<27) | (1<<28))
#define EXT_MODE_8LANES_PHY_PORTS (EXT_MODE_4LANES_PHY_PORTS | (1<<13) | (1<<14) | (1<<15) | (1<<16))
#endif /*BCM_88660_A0*/

/* ARAD DRV Macros */
#define SOC_DPP_ARAD_FABRIC_PORT_TO_PHY_PORT(unit, fabric_port) \
    (fabric_port-FABRIC_LOGICAL_PORT_BASE(unit)+SOC_DPP_FIRST_SFI_PHY_PORT(unit))

#define SOC_DPP_ARAD_DRAM_MODE_REG_SET(field, prop) \
      val = soc_property_port_get(unit, 0, prop, prop_invalid); \
      if (val != prop_invalid) { \
        field = val; \
        dpp_arad->init.dram.dram_conf.params_mode.params.auto_mode = FALSE; \
      }

/* Functions */
int
soc_dpp_arad_str_prop_mc_tbl_mode(int unit)
{
    uint32 max_nof_egr_groups, max_nof_ingr_groups, default_nof_egr_groups, default_nof_ingr_groups;
#if defined(BCM_88660_A0) 
    char *propkey, *propval;    
#endif
    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CONFIG(unit)->tm.mc_mode = 0;
    max_nof_egr_groups = max_nof_ingr_groups = default_nof_egr_groups = default_nof_ingr_groups =
      SOC_IS_ARADPLUS_AND_BELOW(unit) ? ARAD_MULTICAST_TABLE_MODE :
      (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1 ? JER_DEFAULT_NOF_MC_GROUPS : JER_MAX_NOF_MC_GROUPS);
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SOC_DPP_CONFIG(unit)->tm.nof_mc_ids = default_nof_egr_groups;
        SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids = default_nof_ingr_groups;
    } else {
        SOC_DPP_CONFIG(unit)->tm.nof_mc_ids = soc_property_get(unit,spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MAX, default_nof_egr_groups - 1) + 1;
        if (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids > max_nof_egr_groups) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max MC group specified in %s is too big"), spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MAX));
        }
        SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids = soc_property_get(unit,spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MAX, default_nof_ingr_groups - 1) + 1;
        if (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids > max_nof_ingr_groups) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max MC group specified in %s is too big"), spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MAX));
        }
    }

    SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_0;
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_0_MAX_QUEUE;
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_0_NOF_SYSPORT;
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD;
    SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud = ARAD_MC_16B_MAX_EGR_CUD;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && (!SOC_DPP_PP_ENABLE(unit))) {
        propkey = spn_MULTICAST_DESTINATION_ENCODING;
        if ((propval = soc_property_get_str(unit, propkey))) {
            if (sal_strcmp(propval, "16B_CUD_96K_FLOWS_32K_SYSTEM_PORTS__EGRESS_16B_CUD_255_PORTS") == 0) {
            } else if (sal_strcmp(propval, "17B_CUD_64K_FLOWS_32K_SYSTEM_PORTS__EGRESS_16B_CUD_255_PORTS") == 0) {
                SOC_DPP_CONFIG(unit)->tm.mc_mode = DPP_MC_CUD_EXTENSION_MODE;
                SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_1;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_1_MAX_QUEUE;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_1_NOF_SYSPORT;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_1_MAX_ING_CUD;
            } else if (sal_strcmp(propval, "18B_CUD_32K_FLOWS_16K_SYSTEM_PORTS__EGRESS_16B_CUD_255_PORTS") == 0) {
                SOC_DPP_CONFIG(unit)->tm.mc_mode = DPP_MC_CUD_EXTENSION_MODE;
                SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_2;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_2_MAX_QUEUE;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_2_NOF_SYSPORT;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD;
            } else if (sal_strcmp(propval, "17B_CUD_96K_FLOWS_0_SYSTEM_PORTS__EGRESS_16B_CUD_255_PORTS") == 0) {
                SOC_DPP_CONFIG(unit)->tm.mc_mode = DPP_MC_CUD_EXTENSION_MODE;
                SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_3;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_3_MAX_QUEUE;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_3_NOF_SYSPORT;
                SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_3_MAX_ING_CUD;

            } else {
                SOC_DPP_CONFIG(unit)->tm.mc_mode = DPP_MC_EGR_17B_CUDS_127_PORTS_MODE | DPP_MC_CUD_EXTENSION_MODE;
                SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud = ARAD_MC_17B_MAX_EGR_CUD;
                if (sal_strcmp(propval, "16B_CUD_96K_FLOWS_32K_SYSTEM_PORTS__EGRESS_17B_CUD_127_PORTS") == 0) {
                } else if (sal_strcmp(propval, "17B_CUD_64K_FLOWS_32K_SYSTEM_PORTS__EGRESS_17B_CUD_127_PORTS") == 0) {
                    SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_1;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_1_MAX_QUEUE;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_1_NOF_SYSPORT;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_1_MAX_ING_CUD;
                } else if (sal_strcmp(propval, "18B_CUD_32K_FLOWS_16K_SYSTEM_PORTS__EGRESS_17B_CUD_127_PORTS") == 0) {
                    SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_2;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_2_MAX_QUEUE;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_2_NOF_SYSPORT;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD;
                } else if (sal_strcmp(propval, "17B_CUD_96K_FLOWS_0_SYSTEM_PORTS__EGRESS_17B_CUD_127_PORTS") == 0) {
                    SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode = ARAD_MC_DEST_ENCODING_3;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = ARAD_MC_DEST_ENCODING_3_MAX_QUEUE;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports = ARAD_MC_DEST_ENCODING_3_NOF_SYSPORT;
                    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = ARAD_MC_DEST_ENCODING_3_MAX_ING_CUD;
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s\n\r"), propval, propkey));
                }
            }
        }
    } else
#endif /* BCM_88660_A0 */
    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_links_to_core_mapping_mode == SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_SHARED && SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
        SOC_DPP_CONFIG(unit)->tm.mc_mode |= DPP_MC_EGR_HW_ENABLE_MODE; /* hardware controls whether each egress group is active */
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_default_config_tm_get(int unit)
{
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports = 2;
    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; i++) {
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[i] = SOC_DPP_FC_CAL_MODE_DISABLE;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_TMC_FC_OOB_CAL_LEN_MAX/2;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[i][SOC_TMC_CONNECTION_DIRECTION_TX] = SOC_TMC_FC_OOB_CAL_LEN_MAX/2;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_TMC_FC_OOB_CAL_REP_MIN;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[i][SOC_TMC_CONNECTION_DIRECTION_TX] = SOC_TMC_FC_OOB_CAL_REP_MIN;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_llfc_mode[i] = SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE_DISABLE;
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_llfc_mub_enable_mask[i] = SOC_DPP_FC_INBAND_INTLKN_LLFC_MUB_DISABLE;
    }

    SOC_DPP_CONFIG(unit)->tm.max_oob_ports = 2;
    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.max_oob_ports; i++) {
        SOC_DPP_CONFIG(unit)->tm.fc_oob_type[i] = SOC_TMC_FC_CAL_TYPE_NONE;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[i] = SOC_DPP_FC_CAL_MODE_DISABLE;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_TMC_FC_OOB_CAL_LEN_MAX;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[i][SOC_TMC_CONNECTION_DIRECTION_TX] = SOC_TMC_FC_OOB_CAL_LEN_MAX;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[i][SOC_TMC_CONNECTION_DIRECTION_RX] = SOC_TMC_FC_OOB_CAL_REP_MIN;
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[i][SOC_TMC_CONNECTION_DIRECTION_TX] = SOC_TMC_FC_OOB_CAL_REP_MIN;
    }

    SOC_DPP_CONFIG(unit)->tm.max_ses = SOC_TMC_SCH_MAX_SE_ID_ARAD + 1;  
    SOC_DPP_CONFIG(unit)->tm.cl_se_min = SOC_TMC_CL_SE_ID_MIN_ARAD;
    SOC_DPP_CONFIG(unit)->tm.cl_se_max = SOC_TMC_CL_SE_ID_MAX_ARAD;
    SOC_DPP_CONFIG(unit)->tm.fq_se_min = SOC_TMC_FQ_SE_ID_MIN_ARAD;
    SOC_DPP_CONFIG(unit)->tm.fq_se_max = SOC_TMC_FQ_SE_ID_MAX_ARAD;
    SOC_DPP_CONFIG(unit)->tm.hr_se_min = SOC_TMC_HR_SE_ID_MIN_ARAD;
    SOC_DPP_CONFIG(unit)->tm.hr_se_max = SOC_TMC_HR_SE_ID_MAX_ARAD;
    SOC_DPP_CONFIG(unit)->tm.port_hr_se_min = SOC_TMC_HR_SE_ID_MIN_ARAD;
    SOC_DPP_CONFIG(unit)->tm.port_hr_se_max = SOC_TMC_HR_SE_ID_MIN_ARAD + SOC_TMC_SCH_MAX_PORT_ID_ARAD;

    SOC_DPP_CONFIG(unit)->tm.max_connectors = SOC_TMC_SCH_MAX_FLOW_ID_ARAD + 1;  
    SOC_DPP_CONFIG(unit)->tm.max_egr_q_prio = SOC_TMC_EGR_NOF_Q_PRIO_ARAD;
    SOC_DPP_CONFIG(unit)->tm.invalid_port_id_num = SOC_TMC_SCH_PORT_ID_INVALID_ARAD;
    SOC_DPP_CONFIG(unit)->tm.invalid_se_id_num = SOC_TMC_SCH_SE_ID_INVALID_ARAD;
    SOC_DPP_CONFIG(unit)->tm.invalid_voq_connector_id_num = SOC_TMC_SCH_FLOW_ID_INVALID_ARAD;
    SOC_DPP_CONFIG(unit)->tm.nof_vsq_category = SOC_TMC_ITM_VSQ_GROUP_LAST_ARAD;
    SOC_DPP_CONFIG(unit)->tm.is_port_tc_enable = TRUE;

    SOC_DPP_CONFIG(unit)->tm.hr_isq = SOC_TMC_SCH_PORT_ID_INVALID_ARAD; /* PORT ISQ */
    for (i = 0; i < 4; i++) {
        SOC_DPP_CONFIG(unit)->tm.hr_fmqs[i] = SOC_TMC_SCH_PORT_ID_INVALID_ARAD; /* PORT FMQ */
    }

#if 1
    SOC_EXIT;
#endif

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_tdm_get(int unit)
{
    int bypass,packet;
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_FAP_TDM_BYPASS;
    propval = soc_property_get_str(unit, propkey);    

    bypass = (propval) ? 1:0;

    propkey = spn_FAP_TDM_PACKET;
    propval = soc_property_get_str(unit, propkey);    

    packet = (propval) ? 1:0;

    SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits = 48;  
    SOC_DPP_CONFIG(unit)->tdm.min_cell_size = 65;
    SOC_DPP_CONFIG(unit)->tdm.max_cell_size = 256;
    SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size = 0;
    SOC_DPP_CONFIG(unit)->tdm.is_bypass = bypass;    
    SOC_DPP_CONFIG(unit)->tdm.is_packet = packet;    

    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_qos_get(int unit)
{
    soc_dpp_config_qos_t *dpp_qos;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_qos = &(SOC_DPP_CONFIG(unit))->qos;

    dpp_qos->nof_ing_elsp = 1;
    dpp_qos->nof_ing_lif_cos = 64;
    dpp_qos->nof_ing_pcp_vlan = 16;
    dpp_qos->nof_egr_remark_id = 16;
    dpp_qos->nof_egr_pcp_vlan = 16;
    dpp_qos->nof_egr_l2_i_tag = 1;
    dpp_qos->nof_ing_cos_opcode = 7;
    dpp_qos->nof_egr_mpls_php = 4;
    dpp_qos->mpls_elsp_label_range_min = soc_property_get(unit, spn_MPLS_ELSP_LABEL_RANGE_MIN, -1);
    dpp_qos->mpls_elsp_label_range_max = soc_property_get(unit, spn_MPLS_ELSP_LABEL_RANGE_MAX, -1);
    dpp_qos->egr_pcp_vlan_dscp_exp_profile_id = -1;
    dpp_qos->egr_pcp_vlan_dscp_exp_enable = 1;
    dpp_qos->egr_remark_encap_enable = 1;
    dpp_qos->dp_max = 4;
    dpp_qos->ecn_mpls_one_bit_mode = (soc_property_get(unit, spn_MPLS_ECN_MODE, 2) == 1);
    dpp_qos->ecn_mpls_enabled = (soc_property_get(unit, spn_MPLS_ECN_MODE, 2) != 0);
    dpp_qos->ecn_ip_enabled = soc_property_get(unit, spn_IP_ECN_MODE, 1);

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        dpp_qos->nof_egr_dscp_exp_marking = 4;
    } else {
        dpp_qos->nof_egr_dscp_exp_marking = 0;
    }
#endif /* BCM_88660 */

    if ((dpp_qos->mpls_elsp_label_range_min != -1) && (dpp_qos->mpls_elsp_label_range_max != -1)) {
        if (dpp_qos->mpls_elsp_label_range_min > 0xfffff) {
            dpp_qos->mpls_elsp_label_range_min = -1;
            dpp_qos->mpls_elsp_label_range_max = -1;
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: mpls_elsp_label_range_min range 0 - 0xfffff")));
        }

        if (dpp_qos->mpls_elsp_label_range_max > 0xfffff) {
            dpp_qos->mpls_elsp_label_range_min = -1;
            dpp_qos->mpls_elsp_label_range_max = -1;
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: mpls_elsp_label_range_min range 0 - 0xfffff")));
        }

        if (dpp_qos->mpls_elsp_label_range_min > dpp_qos->mpls_elsp_label_range_max) {
            dpp_qos->mpls_elsp_label_range_min = -1;
            dpp_qos->mpls_elsp_label_range_max = -1;
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: mpls_elsp_label_range_min greater than mpls_elsp_label_range_max")));
        }
    } 

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_meter_get(int unit)
{
    soc_dpp_config_meter_t *dpp_meter;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_meter = &(SOC_DPP_CONFIG(unit))->meter;

    dpp_meter->nof_meter_a = 32768;
    dpp_meter->nof_meter_b = 32768;
    dpp_meter->nof_meter_profiles = 1024;
    dpp_meter->nof_high_rate_profiles = 0;
    dpp_meter->policer_min_rate = 146; /* kbits */
    dpp_meter->meter_min_rate = 18; /* kbits */
    dpp_meter->max_rate = ARAD_IF_MAX_RATE_KBPS; /* kbits */
    dpp_meter->min_burst = 64; /* bytes */
    dpp_meter->max_burst = 4161536; /* bytes */
    /* arad no lr/hr */
    dpp_meter->lr_max_burst = 4161536; /* bytes */
    dpp_meter->lr_max_rate = ARAD_IF_MAX_RATE_KBPS; /* bytes */

    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_l3_get(int unit)
{
    soc_dpp_config_l3_t *dpp_l3;
    uint32 ipmc_mode;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_l3 = &(SOC_DPP_CONFIG(unit))->l3;

    dpp_l3->ip_enable = soc_property_get(unit,spn_L3_ENABLE,1);
    /* In Jericho, do not limit the number of VRFs since it is user's responsibility to add a KAPS entry for each one */
    dpp_l3->max_nof_vrfs = soc_property_get(unit,spn_IPV4_NUM_VRFS, (SOC_IS_JERICHO(unit)? SOC_DPP_DEFS_GET(unit, nof_vrfs): 8));
    dpp_l3->nof_rifs = 4096;
    dpp_l3->ecmp_max_paths = 512;
    dpp_l3->eep_db_size = 65536;
    dpp_l3->fec_db_size = SOC_DPP_DEFS_GET(unit, nof_fecs);
    dpp_l3->fec_ecmp_reserved = SOC_DPP_DEFS_GET(unit, nof_ecmps); /* reserved for ECMP*/
    dpp_l3->vrrp_max_vid = soc_property_get(unit, spn_L3_VRRP_MAX_VID, 4096);
    dpp_l3->vrrp_ipv6_distinct = soc_property_get(unit, spn_L3_VRRP_IPV6_DISTINCT, 0);
    ipmc_mode = soc_property_get(unit, spn_IPMC_PIM_MODE, 0x1);
    /* bit 1 indicate BIDIR suppor */
    if (ipmc_mode & 2) {
        dpp_l3->nof_rps = 256;
        dpp_l3->nof_bidir_vrfs = 512;
    }
    else{
        dpp_l3->nof_rps = 0;
        dpp_l3->nof_bidir_vrfs = 0;
    }
    dpp_l3->ipmc_vpn_lookup_enable = soc_property_get(unit, spn_IPMC_VPN_LOOKUP_ENABLE, 0x1);

#ifdef BCM_88660_A0
    /* This mode is only available in ARADPLUS */
    if (SOC_IS_ARADPLUS(unit)) {
        dpp_l3->multiple_mymac_enabled = soc_property_get(unit, spn_L3_MULTIPLE_MYMAC_TERMINATION_ENABLE, 0); 
        dpp_l3->multiple_mymac_mode    = soc_property_get(unit, spn_L3_MULTIPLE_MYMAC_TERMINATION_MODE, 0); ;

        if (dpp_l3->multiple_mymac_mode > 1 && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Maximum multiple mymac mode for aradplus is 1.")));
        }
        
        /*  In ARADPLUS, all vrrp modes except for 256 are treated as 4096, because they are redundant.
            In Jericho, mode 256 is redundant as well.*/
        if (SOC_IS_JERICHO(unit) || dpp_l3->vrrp_max_vid != 256) {

            if (dpp_l3->vrrp_max_vid == 0 && dpp_l3->multiple_mymac_enabled) {
                /* User may choose to disable VRRP and enable multiple mymac mode. In that case, set multiple_mymac_enabled to 2
                   to indicate that VRRP APIs should not be set. */
                dpp_l3->multiple_mymac_enabled = 2;
            }
            dpp_l3->vrrp_max_vid = 4096;

            /* In aradplus vrrp mode 0 (max_vid 4096), mutliple mymac mode 1 (ipv4 distinct) and vrrp ipv6 distinct are set by the same register. */
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                dpp_l3->multiple_mymac_mode = (dpp_l3->multiple_mymac_mode || dpp_l3->vrrp_ipv6_distinct) ? TRUE : FALSE;
                dpp_l3->vrrp_ipv6_distinct = dpp_l3->multiple_mymac_mode;
            }
        }

        /* In ARADPLUS, mode 256 and multiple mymac can't co exist. */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) && dpp_l3->vrrp_max_vid == 256 && dpp_l3->multiple_mymac_enabled) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Can't use VRRP with max vid 256 and mutliple mymac.")));
        }
    }

#endif /* BCM_88660_A0 */
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_general_get(int unit)
{
    soc_dpp_config_pp_general_t *dpp_general;
    soc_dpp_config_pp_t *dpp_pp;
    uint8 vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_DEFAULT;
    uint8 pon_tcam_enable = 0;  
    char *propkey, *propval;
    uint8 prop_value;
    uint32 soc_sand_rv;
    char   mac_string[SOC_SAND_PP_MAC_ADDRESS_STRING_LEN];
    uint8 ipv4_term_enable = 0;

    SOCDNX_INIT_FUNC_DEFS

    dpp_general = &(SOC_DPP_CONFIG(unit))->dpp_general;
    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    propkey = spn_VLAN_MATCH_DB_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if ((propval)) {
        if (sal_strcmp(propval, "DEFAULT") == 0) {            
            /* Nothing to do */
        } else if (sal_strcmp(propval, "FULL_DB") == 0) {
            vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_FULL_DB;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_DEFAULT;
    }

    propkey = spn_VLAN_MATCH_CRITERIA_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if ((propval)) {
        if (sal_strcmp(propval, "DEFAULT") == 0) {            
            /* Nothing to do */
        } else if (sal_strcmp(propval, "PCP_LOOKUP") == 0) {
            if (vlan_match_db_mode != SOC_DPP_VLAN_DB_MODE_DEFAULT) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "Unexpected property value (\"%s\") for %s while VLAN match also set\n\r"), propval, propkey));
            }
            vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_PCP;
        } else if (sal_strcmp(propval, "PON_PCP_ETHERTYPE") == 0) {
            pon_tcam_enable = 1;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 

    /* l3_source_bind_mode */
    propkey = spn_L3_SOURCE_BIND_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if ((propval)) {
        if (sal_strcmp(propval, "DISABLE") == 0) {            
            dpp_pp->l3_source_bind_mode = SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE;
        } else if (sal_strcmp(propval, "IPV4") == 0) {
            dpp_pp->l3_source_bind_mode = SOC_DPP_L3_SOURCE_BIND_MODE_IPV4;
        } else if (sal_strcmp(propval, "IPV6") == 0) {
            dpp_pp->l3_source_bind_mode = SOC_DPP_L3_SOURCE_BIND_MODE_IPV6;
        } else if (sal_strcmp(propval, "IP") == 0) {
            dpp_pp->l3_source_bind_mode = SOC_DPP_L3_SOURCE_BIND_MODE_IP;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 

    /* l3_source_bind_subnet_mode */
    propkey = spn_L3_SOURCE_BIND_SUBNET_MODE;
    propval = soc_property_get_str(unit, propkey);
    if ((propval)) {
        if (sal_strcmp(propval, "DISABLE") == 0) {            
            dpp_pp->l3_source_bind_subnet_mode = SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_DISABLE;
        } else if (sal_strcmp(propval, "IPV4") == 0) {
            dpp_pp->l3_source_bind_subnet_mode = SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV4;
        } else if (sal_strcmp(propval, "IPV6") == 0) {
            dpp_pp->l3_source_bind_subnet_mode = SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV6;
        } else if (sal_strcmp(propval, "IP") == 0) {
            dpp_pp->l3_source_bind_subnet_mode = SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IP;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_ERR_MSG_MODULE_NAME, unit, _SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 

    /* Whether enable IP anti-spoofing function for ARP request */
    prop_value = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_source_bind_arp_relay", 0);
    if (prop_value > SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_BOTH) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_ERR_MSG_MODULE_NAME, unit, _SOCDNX_MSG("Unexpected property value (\"%hhu\") for %s"), prop_value, propkey));
    } else {
        dpp_pp->l3_src_bind_arp_relay = prop_value;
    }

    if (dpp_pp->l3_source_bind_mode ||
        dpp_pp->l3_source_bind_subnet_mode ||
        (dpp_pp->l3_src_bind_arp_relay & SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_UP)) {
        (SOC_DPP_CONFIG(unit))->qos.nof_ing_lif_cos = 32;
    }

    if (!SOC_IS_ARADPLUS(unit) && SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_OR_ARP_ENABLE(unit)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_ERR_MSG_MODULE_NAME, unit, _SOCDNX_MSG("The property value (\"%s\") for %s only working in arad_plus device"), propval, propkey));
    }

    /* VMAC enable */
    propkey = spn_VMAC_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    if (prop_value == 0) {
        dpp_pp->vmac_enable = 0;
    } else {
        if(dpp_pp->l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value vmac_enable for %s"), propkey));
        }
        dpp_pp->vmac_enable = 1;
        (SOC_DPP_CONFIG(unit))->qos.nof_ing_lif_cos = 32;
    }
    /* VMAC encoding */
    if (dpp_pp->vmac_enable) {
        /* VMAC encoding value */
        soc_sand_SAND_PP_MAC_ADDRESS_clear(&(dpp_pp->vmac_encoding_val));
        soc_sand_os_memset(mac_string, 0, sizeof(mac_string));
        propkey = spn_VMAC_ENCODING_VALUE;
        propval = soc_property_get_str(unit, propkey);

        if ((propval)) {
            /* 1st char must be '0' and next char must be 'x' */
            if (sal_strlen(propval) < 2 || *propval != '0' || (*(propval + 1) != 'x' && *(propval + 1) != 'X')) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }

            sal_memcpy(mac_string, (propval + 2), sal_strlen(propval));
            soc_sand_rv = soc_sand_pp_mac_address_string_parse(mac_string, &(dpp_pp->vmac_encoding_val));
            if (handle_sand_result(soc_sand_rv)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }
        }
        /* VMAC encoding mask  */
        soc_sand_SAND_PP_MAC_ADDRESS_clear(&(dpp_pp->vmac_encoding_mask));
        soc_sand_os_memset(mac_string, 0, sizeof(mac_string));
        propkey = spn_VMAC_ENCODING_MASK;
        propval = soc_property_get_str(unit, propkey);
        if ((propval)) {
            /* 1st char must be '0' and next char must be 'x' */
            if (sal_strlen(propval) < 2 || *propval != '0' || (*(propval + 1) != 'x' && *(propval + 1) != 'X')) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }

            sal_memcpy(mac_string, (propval + 2), sal_strlen(propval));
            soc_sand_rv = soc_sand_pp_mac_address_string_parse(mac_string, &(dpp_pp->vmac_encoding_mask));
            if (handle_sand_result(soc_sand_rv)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }
        }
    }
  


    propkey = spn_LOCAL_SWITCHING_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    if (prop_value == 0) {
        dpp_pp->local_switching_enable = 0;
    } else {
        dpp_pp->local_switching_enable = 1;
        (SOC_DPP_CONFIG(unit))->qos.nof_ing_lif_cos = 32;
    }
  
    dpp_pp->ipv4_tunnel_term_bitmap_enable = 0; /* none enabled */
    dpp_pp->ingress_ipv4_tunnel_term_mode = SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_DISABLE;

    /* IPV4 termination modes */
    propkey = spn_BCM886XX_IP4_TUNNEL_TERMINATION_MODE;
    prop_value = soc_property_get(unit, propkey, 3);

    /* IP tunnel */
    if (prop_value == 2) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT;
    } else if (prop_value == 1) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP;
    } else if (prop_value == 3) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= (SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT | SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP);
    } else if (prop_value == 4) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL);
    } else if (prop_value == 5) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= (SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT | SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL);
    }

    /* NVGRE */
    propkey = spn_BCM886XX_L2GRE_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);

    if (prop_value) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE;
    }

    /* VxLAN */
    propkey = spn_BCM886XX_VXLAN_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    if (prop_value) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN;
    }

    /* IPv4 tunnel deframent bit enable setting */
    propkey = spn_8865X_IPV4_TUNNEL_DF_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->ip_tunnel_defrag_set = (prop_value != 0);
    if (SOC_IS_ARAD_A0(unit) && prop_value != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("feature not supported on this device version(\"%s\") for %s"), propval, propkey));
    }

    /* FCoE properties*/
    propkey = spn_BCM886XX_FCOE_SWITCH_MODE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->fcoe_enable = (prop_value != 0);
    if (!SOC_IS_ARAD(unit) && prop_value != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("feature not supported on this device version(\"%s\") for %s"), propval, propkey));
    }
    /* vrf number to take from IP-VRF for FCoE usage on for all VFTs*/
    propkey = spn_BCM886XX_FCOE_NUM_VRF;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->fcoe_reserved_vrf = prop_value;
    if (dpp_pp->fcoe_enable && (dpp_pp->fcoe_reserved_vrf > SOC_DPP_CONFIG(unit)->l3.max_nof_vrfs)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("VRF used for FCF not in range of valid VRFs(\"%s\") for %s"), propval, propkey));
    }

    /* EoIP */
    propkey = spn_BCM886XX_ETHER_IP_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);

    if (prop_value) {
        dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_ETHER;
    }

    /* Tunnel termination for L2GRE modes */
    if (dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE) {
        propkey = spn_BCM886XX_L2GRE_TUNNEL_LOOKUP_MODE;
        prop_value = soc_property_get(unit, propkey, 2);       
    }

    /* Tunnel termination for VXLAN modes */
    if (dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) {
        propkey = spn_BCM886XX_VXLAN_TUNNEL_LOOKUP_MODE;
        prop_value = soc_property_get(unit, propkey, 2);       
    }

    if ((dpp_pp->ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN | SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE))) { 
        /* prop value 1 for seperated, 2 for joined */
        if (prop_value == 1) {
            dpp_pp->ingress_ipv4_tunnel_term_mode = SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_SEPERATED;
        } else{
            dpp_pp->ingress_ipv4_tunnel_term_mode = SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_JOIN;
        }
    }
    /* LIF-ID of IP-LIF-Dummy */
    ipv4_term_enable = ((dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_TUNNEL_MASK) || 
                        (dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE) || 
                        (dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) ||
                        (dpp_pp->ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_ETHER));
    if (ipv4_term_enable &&
       (dpp_pp->ingress_ipv4_tunnel_term_mode == SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_SEPERATED)) {
        dpp_pp->ip_lif_dummy_id = soc_property_get(unit, spn_DEFAULT_LOGICAL_INTERFACE_IP_TUNNEL_OVERLAY_MC, 0x0);
    }

    /* EVB */
    propkey = spn_EVB_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->evb_enable = prop_value;

    /* VLAN Translation*/
    propkey = spn_BCM886XX_VLAN_TRANSLATE_MODE;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->vlan_translate_mode = prop_value;

    /* system resource management */
    propkey = spn_BCM88XXX_SYSTEM_RESOURCE_MANAGEMENT;
    prop_value = soc_property_get(unit, propkey, 0);
    dpp_pp->sys_rsrc_mgmt = prop_value;

    dpp_pp->ipv6_tunnel_enable = 0;
    propkey = spn_BCM886XX_IPV6_TUNNEL_ENABLE;
    prop_value = soc_property_get(unit, propkey, 0);
    if (prop_value) {
       dpp_pp->ipv6_tunnel_enable = 1;
    }

    dpp_general->nof_user_define_traps = SOC_PPC_TRAP_CODE_USER_DEFINED_LAST - SOC_PPC_TRAP_CODE_USER_DEFINED_0 + 1;
    dpp_general->nof_ingress_trap_codes = 256;
    dpp_pp->ipmc_enable = 1;
    dpp_pp->initial_vid_enable = 1;
    dpp_pp->drop_dest = 0x7FFFF;
    dpp_pp->mpls_label_index_enable = soc_property_get(unit, spn_MPLS_TERMINATION_LABEL_INDEX_ENABLE, 0);
    dpp_pp->vlan_match_db_mode = vlan_match_db_mode;
    dpp_pp->extend_p2p_mpls_enable = 0;
    dpp_pp->extend_p2p_vlan_enable = 0;
    dpp_pp->extend_p2p_mim_enable = 0;
    dpp_pp->global_lif_index_simple = soc_property_get(unit, spn_LOGICAL_PORT_L2_BRIDGE, 1);
    dpp_pp->global_lif_index_drop = soc_property_get(unit, spn_LOGICAL_PORT_DROP, SOC_PPC_AC_ID_INVALID);
    dpp_pp->pon_application_enable = 0;
    dpp_pp->pon_tcam_lkup_enable = pon_tcam_enable;
    dpp_pp->pon_tls_in_tcam = soc_property_get(unit, spn_PON_TLS_DATABASE, 0);
    dpp_pp->pon_custom_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_special_double_tag_lookup", 0);
    dpp_pp->mim_vsi_mode = (soc_property_get(unit, spn_MIM_NUM_VSIS, 0) == 32768) ? 1 : 0;
    dpp_pp->next_hop_mac_extension_enable = soc_property_get(unit, spn_BCM886XX_NEXT_HOP_MAC_EXTENSION_ENABLE, 1); /* by default enabled */
    dpp_pp->mpls_eli_enable = soc_property_get(unit, spn_MPLS_ENTROPY_LABEL_INDICATOR_ENABLE, 1); /* by default enabled */
    dpp_pp->oam_ccm_2_fhei_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_ccm_two_fhei", 0);
    dpp_pp->oam_ccm_2_fhei_eg_default_mirror_profile = SOC_DPP_ARAD_OAM_DEFAULT_MIRROR_PROFILE;
    dpp_pp->oam_ccm_2_fhei_eg_fake_injection_port = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_eg_fake_injection_port", 0);
    if (SOC_IS_ARADPLUS_A0(unit)) {
        dpp_pp->oam_classifier_advanced_mode = soc_property_get(unit, spn_OAM_CLASSIFIER_ADVANCED_MODE, 1);
    } else {
        if (soc_property_get(unit, spn_OAM_CLASSIFIER_ADVANCED_MODE, 0)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("soc_dpp_info_config: oam_classifier_advanced_mode can be set only on ARAD+ device.")));
        } else {
            dpp_pp->oam_classifier_advanced_mode = 0;
        }
    }
    if (SOC_IS_ARADPLUS(unit)) {
        dpp_pp->oam_use_fifo_dma = soc_property_get(unit, spn_OAMP_FIFO_DMA_ENABLE, 0);
    } else {
        dpp_pp->oam_use_fifo_dma = 0;
    }
    dpp_pp->flexible_qinq_enable = soc_property_get(unit, spn_VLAN_TRANSLATION_MATCH_IPV4, 0); /*vlan translation match IPv4*/
    dpp_pp->ipv6_ext_header = soc_property_get(unit, spn_BCM886XX_IPV6_EXT_HDR_ENABLE, 0); /* IPv6 extension header processing enabled */
    dpp_pp->custom_feature_vt_tst1 = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vt_tst1", 0);
    dpp_pp->gal_support = soc_property_get(unit, spn_MPLS_TERMINATION_PWE_VCCV_TYPE4_MODE, 0); 
    dpp_pp->explicit_null_support = soc_property_get(unit, spn_MPLS_TERMINATION_EXPLICIT_NULL_LABEL_LOOKUP_MODE, 0);
    dpp_pp->explicit_null_support_lif_id = soc_property_get(unit, spn_DEFAULT_LOGICAL_INTERFACE_MPLS_TERMINATION_EXPLICIT_NULL, -1);
    if ((dpp_pp->explicit_null_support || dpp_pp->gal_support) && (!dpp_pp->custom_feature_vt_tst1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("gal and explicit null support can be enabled only when vt_tst1 is set.")));
    }
    if (dpp_pp->explicit_null_support && (dpp_pp->explicit_null_support_lif_id == -1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("when explicit null is set, default_logical_interface_mpls_termination_explicit_null must be set to valid LIF-ID.")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_l2_get(int unit)
{
    soc_dpp_config_l2_t *dpp_l2;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_l2 = &(SOC_DPP_CONFIG(unit))->l2;

    dpp_l2->nof_vsis = SOC_PPC_VSI_MAX_NOF_ARAD; 
    dpp_l2->nof_lifs = 65536; 
    dpp_l2->mac_size = 262144;
    dpp_l2->mac_nof_event_handlers = 4;


    dpp_l2->learn_limit_mode = 0;
    dpp_l2->learn_limit_lif_range_base[0] = 0;
    dpp_l2->learn_limit_lif_range_base[1] = 0;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        dpp_l2->learn_limit_mode = SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI;
        dpp_l2->learn_limit_lif_range_base[0] = 0;
        dpp_l2->learn_limit_lif_range_base[1] = 0x4000;
    }
#endif

    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_trill_get(int unit)
{
    soc_dpp_config_trill_t *dpp_trill;
    uint8                   trill_mode;
    uint8                   trill_transparent_service;
    soc_port_t              port_i;
    int                     lif_id;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_trill = &(SOC_DPP_CONFIG(unit))->trill;

    trill_mode = soc_property_get(unit,spn_TRILL_MODE, 0);
    switch (trill_mode) {
        case 0:
            dpp_trill->mode =  SOC_PPD_TRILL_MODE_DISABLED;
            break;
        case 1:
            dpp_trill->mode =  SOC_PPD_TRILL_MODE_VL;
            break;
        case 2:
            dpp_trill->mode = SOC_PPD_TRILL_MODE_FGL;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "soc_dpp_get_default_config_trill: Invalid trill mode \n")));
            break;         
    }

    dpp_trill->mc_prune_mode = soc_property_get(unit,spn_TRILL_MC_PRUNE_MODE, 0);
   /* mc_id = '1'  mean old implementation that uses sw db for mapping mc-id to nickname*/
    dpp_trill->mc_id=0;
    /* create_ecmp_port = '1' mean old implementation that define ecmp by creating ecmp trill port*/
    dpp_trill->create_ecmp_port=0;

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i)  {
        lif_id = soc_property_port_suffix_num_get(unit, port_i, -1, spn_CUSTOM_FEATURE, "trill_designated_vlan_inlif", -1);
        if (lif_id != -1) {
            dpp_trill->designated_vlan_inlif_enable = 1;
            break;
        }
    }

    trill_transparent_service = soc_property_get(unit,spn_TRILL_TRANSPARENT_SERVICE, 0);
	if ((dpp_trill->mode != SOC_PPD_TRILL_MODE_FGL) && (trill_transparent_service != 0)){
        LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "soc_dpp_get_default_config_trill: Invalid trill transparent service \n")));
	} else {	    
        dpp_trill->transparent_service= trill_transparent_service;
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_oam_get(int unit)
{
    soc_dpp_config_pp_t     *pp_config;
    uint8                   oam_mode;

    SOCDNX_INIT_FUNC_DEFS

    pp_config = &(SOC_DPP_CONFIG(unit))->pp;

    oam_mode = soc_property_get(unit,spn_NUM_OAMP_PORTS, 0);
    pp_config->oamp_enable = (oam_mode != 0);

    SOCDNX_FUNC_RETURN;
}

int
soc_arad_default_config_get(int unit, soc_dpp_config_arad_t *cfg)
{
  int rv = SOC_E_NONE;

  SOCDNX_INIT_FUNC_DEFS

  sal_memset(cfg, 0, sizeof (soc_dpp_config_arad_t));

  /* ARAD-specific init */
  arad_ARAD_MGMT_INIT_clear(&(cfg->init));
  cfg->voq_mapping_mode = VOQ_MAPPING_INDIRECT;
  cfg->action_type_source_mode = ACTION_TYPE_FROM_QUEUE_SIGNATURE;

  ARAD_PP_MGMT_OPERATION_MODE_clear(&cfg->pp_op_mode);

  /* TM init */
  rv = soc_arad_default_config_tm_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);    

  /* TDM init */
  rv = soc_arad_default_config_tdm_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);    

  /* QoS init */
  rv = soc_arad_default_config_qos_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);    

  /* meter init */
  rv = soc_arad_default_config_meter_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);    

  /* l3 init */
  rv = soc_arad_default_config_l3_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);    

  /* general pp init */
  rv = soc_arad_default_config_general_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);  

  /* l2 init */
  rv = soc_arad_default_config_l2_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  /* trill init */
  rv = soc_arad_default_config_trill_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

  /* oam init */
  rv = soc_arad_default_config_oam_get(unit);
  SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_reset_cmicm_regs(int unit) 
{
    uint32 rval, divisor, dividend, mdio_delay;
    SOCDNX_INIT_FUNC_DEFS;

    /*In each Byte swap rings*/
    if (SOC_IS_ARDON(unit)) {

        /* SBUS ring map:
         * Ring 0: FMAC0 (10), FMAC1 (11), FMAC2 (12), FMAC3 (13), FMAC4 (14),
         *         FMAC5 (15), FMAC6 (16), FMAC7 (17), FMAC8 (18)FSRD0 (19),
         *         FSRD1 (20), FSRD2 (21), CLP0 (24), CLP1 (25),
         *         XLP0 (27), XLP1 (28), DRCE (44), DRCF (45), DRCG (46),
         *         DRCH (47), BRDC_FSRD(60), BRDC_FMAC(61), Broadcast (63)
         *         CFC (1),EGQ (2),EPNI (3),FCR (4),FDR (56), FDT (6), MESH_TOPOLOGY (8),
         *         RTP (9), NBI (26),CRPS (35), IPS (36), IPT (37), IQM (38), 
         *         FCT (39), AVS (48), CGM (54), OAMP (7) , IRE (49), IDR (50), 
         *         IRR (51), IHP (52), IHB (53), SCH (55)
         * Ring 1: MMU (22), OCB (23) DRCA (40), DRCB (41), DRCC (42), DRCD (43)
         * Ring 2: OTPC (57)
         * Ring 5: ECI (0)
         */
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit,   0x00000005));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit,  0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x11000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x00001111));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x00000000));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x00000020));
    } else {

        /* SBUS ring map:
         * Ring 0: OTPC (57)
         * Ring 1: FMAC0 (10), FMAC1 (11), FMAC2 (12), FMAC3 (13), FMAC4 (14), 
         *         FMAC5 (15), FMAC6 (16), FMAC7 (17), FMAC8 (18)FSRD0 (19), 
         *         FSRD1 (20), FSRD2 (21), CLP0 (24), CLP1 (25),
         *         XLP0 (27), XLP1 (28), DRCA (40), DRCB (41), DRCC (42), 
         *         DRCD (43), DRCE (44), DRCF (45), DRCG (46), DRCH (47),
         *         BRDC_FSRD(60), BRDC_FMAC(61), Broadcast (63)
         * Ring 2: CFC (1),EGQ (2),EPNI (3),FCR (4),FDR (5), FDT (6), MESH_TOPOLOGY (8), 
         *         RTP (9), NBI (26),CRPS (35), IPS (36), IPT (37), IQM (38), 
         *        FCT (39), OLP (48), CGM (54), OAMP (56)
         * Ring 3: MMU (22), OCB (23), IRE (49), IDR (50), 
         *         IRR (51), IHP (52), IHB (53), SCH (55)    
         * Ring 7:  ECI (0)
         *     
         */

        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x02222227));    
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x11111122));   
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x33111111));  
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00011211));  
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x22222000));  
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x11111111));  
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x32333332));  
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x10110012));  
    }

    /* Mdio - internal*/
    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, 24);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf, dividend);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    /* Mdio - external*/
    dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);
    divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, 24);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_EXT_MDIOr, &rval, DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_EXT_MDIOr, &rval, DIVIDENDf, dividend);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RATE_ADJUST_EXT_MDIOr(unit, rval));

    /*Mdio -Delay*/
    rval = 0;
    mdio_delay = 0xf;
    soc_reg_field_set(unit, CMIC_MIIM_CONFIGr, &rval, MDIO_OUT_DELAYf, mdio_delay);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_CONFIGr(unit, rval));
 
    /* Led processor */  

    /* mapping xe port 1-32 to led processro memory indexes 1 -32 */  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit, 0x75E7E0));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit, 0x65A6DC));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit, 0x5565D8));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit, 0x4524D4));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit, 0x24A2CC));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit, 0x1461C8));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit, 0x0420C4));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit, 0x34E3D0));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit, 0x000000));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit, 0x000000));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit, 0x000000));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit, 0x000000));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, 0x000000));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, 0x000000));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit, 0x000000));  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit, 0x000000));  

    /* setting ARAD ports statuses scan values */  
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_LEDUP0_CTRLr(unit, 0xba));  

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_info_config_device_ports(int unit) 
{
    soc_info_t          *si;

    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    si->num_time_interface = 1;

    SOCDNX_FUNC_RETURN;

}

int
soc_arad_core_frequency_config_get(int unit, int dflt_freq_khz, uint32 *freq_khz) 
{
    SOCDNX_INIT_FUNC_DEFS;

    if (soc_property_get_str(unit, spn_CORE_CLOCK_SPEED) != NULL) {
        /*configure ref clock in MHz*/
        *freq_khz = soc_property_get(unit, spn_CORE_CLOCK_SPEED, dflt_freq_khz / 1000) * 1000;

    } else {
        /*configure ref clock in KHz*/
        *freq_khz = soc_property_suffix_num_get(unit, 0,  spn_CORE_CLOCK_SPEED, "khz", dflt_freq_khz);
    }

    SOCDNX_FUNC_RETURN;
}

int 
soc_arad_schan_timeout_config_get(int unit, int *schan_timeout)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SAL_BOOT_QUICKTURN) {
        *schan_timeout = SCHAN_TIMEOUT_QT;
    } else if (SAL_BOOT_PLISIM) {
        *schan_timeout = SCHAN_TIMEOUT_PLI;
    } else {
        *schan_timeout = SCHAN_TIMEOUT;
    }

    *schan_timeout = soc_property_get(unit, spn_SCHAN_TIMEOUT_USEC, *schan_timeout);

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_cmic_sbus_timeout_set(int unit)
{
    soc_control_t        *soc;
    int rv = SOC_E_NONE;
    uint32 frequency, ticks,
           max_uint = 0xFFFFFFFF,
           max_ticks= 0x3FFFFF;
    
    SOCDNX_INIT_FUNC_DEFS;

    soc = SOC_CONTROL(unit);

     /* configure ticks to be a HW timeout that is 75% of SW timeout: */
    /* units:
     * schanTimeout is in microsecond
     * frequency is recieved in KHz, and modified to be in MHz.
     * after the modification: ticks = frequency * Timeout 
     */
    rv = soc_arad_core_frequency_config_get(unit, 600000, &(SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency));
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_arad_schan_timeout_config_get(unit, &(SOC_CONTROL(unit)->schanTimeout));
    SOCDNX_IF_ERR_EXIT(rv);


    frequency = (SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency) / 1000;

    if ((max_uint / frequency) > soc->schanTimeout) { /* make sure ticks can be represented in 32 bits*/
        ticks = frequency * soc->schanTimeout;
        ticks = ((ticks / 100) * 75); /* make sure hardware timeout is smaller than software*/
    } else {
        ticks = max_ticks;
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_TIMEOUTr(unit, ticks));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_hard_reset(int unit, int reset_action)
{
    uint32 
        rv,
        reg32_val = 0;
    soc_timeout_t 
        to;

    SOCDNX_INIT_FUNC_DEFS;

    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CPS_RESETr(unit, 0x1)); 

        soc_timeout_init(&to, 100000, 100);
        for(;;) {
            SOCDNX_IF_ERR_EXIT(READ_CMIC_CPS_RESETr(unit, &reg32_val));
#ifdef PLISIM
            if (SAL_BOOT_PLISIM) {
                reg32_val = 0x0;
            }
#endif /* PLISIM */
            if (reg32_val == 0x0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                SOCDNX_EXIT_WITH_ERR(_SHR_E_INIT, (_BSL_SOCDNX_MSG("Error: CPS reset field not asserted correctly.")));
                break;
            }
        }   
    }

    /* Fix CMIC Endianess */
    soc_endian_config(unit);

    if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        rv = soc_arad_reset_cmicm_regs(unit);
        SOCDNX_IF_ERR_EXIT(rv);
        
        rv = soc_arad_cmic_sbus_timeout_set(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_system_ftmh_load_balancing_ext_mode_get(int unit, ARAD_MGMT_FTMH_LB_EXT_MODE *system_ftmh_load_balancing_ext_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SYSTEM_FTMH_LOAD_BALANCING_EXT_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if ((!propval) || (sal_strcmp(propval, "DISABLED") == 0)) {
        *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED;
    } else if (sal_strcmp(propval, "8B_LB_KEY_8B_STACKING_ROUTE_HISTORY") == 0) {
        *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_8B_LB_KEY_8B_STACKING_ROUTE_HISTORY;
    } else if (sal_strcmp(propval, "16B_STACKING_ROUTE_HISTORY") == 0) {
        *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_16B_STACKING_ROUTE_HISTORY;
    } else if (sal_strcmp(propval, "ENABLED") == 0) {
        *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_ENABLED;
    }
#ifdef BCM_88660_A0
    else if (sal_strcmp(propval, "STANDBY_MC_LB") == 0) {
        if(SOC_IS_ARADPLUS(unit)) {
            *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device doesn't support property  (\"%s\") for %s"), propval, propkey));
        }
    } else if (sal_strcmp(propval, "FULL_HASH") == 0) {
        if(SOC_IS_ARADPLUS(unit)) {
            *system_ftmh_load_balancing_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_FULL_HASH;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device doesn't support property  (\"%s\") for %s"), propval, propkey));
        }
    } 
#endif
     else {
         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_system_trunk_hash_format_get(int unit, ARAD_MGMT_TRUNK_HASH_FORMAT *system_trunk_hash_format)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_TRUNK_HASH_FORMAT;
    propval = soc_property_get_str(unit, propkey);    

    if ((!propval) || (sal_strcmp(propval, "NORMAL") == 0)) {
        *system_trunk_hash_format = ARAD_MGMT_TRUNK_HASH_FORMAT_NORMAL;
    } 
#ifdef BCM_88660_A0
    else if (SOC_IS_ARADPLUS(unit )){
        if(sal_strcmp(propval, "INVERTED")== 0) {
            *system_trunk_hash_format = ARAD_MGMT_TRUNK_HASH_FORMAT_INVERTED;
        } else if(sal_strcmp(propval, "DUPLICATED")== 0) {
            *system_trunk_hash_format = ARAD_MGMT_TRUNK_HASH_FORMAT_DUPLICATED;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 
#endif
     else {
         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device doesn't support property  (\"%s\") for %s"), propval, propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_validate_fap_device_mode(int unit, uint8 pp_enable, ARAD_MGMT_TDM_MODE tdm_mode)
{
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {
        case BCM88654_DEVICE_ID:
        case BCM88664_DEVICE_ID:
            if(pp_enable != 0 || 
               (tdm_mode != ARAD_MGMT_TDM_MODE_TDM_OPT && tdm_mode != ARAD_MGMT_TDM_MODE_TDM_STA)) {
                SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);            
            }
             break;
        default:
             break;                
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_prop_fap_device_mode_get(int unit, uint8 *pp_enable, SOC_TMC_MGMT_TDM_MODE *tdm_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_FAP_DEVICE_MODE;
    propval = soc_property_get_str(unit, propkey);    
   
    if (propval) {
        if (sal_strcmp(propval, "TM") == 0) {
            *pp_enable = FALSE;            
        } else if (sal_strcmp(propval, "PP") == 0) {
            *pp_enable = TRUE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));    
        }
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

    propkey = spn_FAP_TDM_BYPASS;
    propval = soc_property_get_str(unit, propkey);    

    if ((propval)) {
        if (sal_strcmp(propval, "TDM_OPTIMIZED") == 0) {            
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_TDM_OPT;
        } else if (sal_strcmp(propval, "TDM_STANDARD") == 0) {
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_TDM_STA;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));  
        }
    } else {
        *tdm_mode = SOC_TMC_MGMT_TDM_MODE_PACKET;
    }

    if(soc_arad_validate_fap_device_mode(unit, *pp_enable, *tdm_mode)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("fap_device_mode configuration doesn't supported")));  
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_mc_nbr_full_dbuff_get(int unit, ARAD_INIT_NBR_FULL_MULTICAST_DBUFF_MODES *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_MULTICAST_NBR_FULL_DBUFF;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE") == 0) {
            *p_val = ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE;
        } else if (sal_strcmp(propval, "ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE") == 0) {
            *p_val = ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));  
        }
    } else {
        *p_val = ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_stat_if_report_mode_get(int unit, SOC_TMC_STAT_IF_REPORT_MODE *stat_if_report_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_MODE;
    propval = soc_property_get_str(unit, propkey);    
    *stat_if_report_mode = SOC_TMC_STAT_NOF_IF_REPORT_MODES;

    if (propval) {
        if (sal_strcmp(propval, "BILLING") == 0) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_MODE_BILLING;
        } 
        else if (SOC_IS_ARAD(unit) && (sal_strcmp(propval, "QSIZE") == 0)) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_QSIZE;
        }
#ifdef BCM_88650_B0
        else if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && (sal_strcmp(propval, "BILLING_QUEUE_NUMBER") == 0)) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER;
        }
#endif
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));  
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_stat_if_billing_pkt_size_get(int unit, SOC_TMC_STAT_IF_PKT_SIZE *stat_if_pkt_size)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_PKT_SIZE;
    propval = soc_property_get_str(unit, propkey);    
    *stat_if_pkt_size = SOC_TMC_NOF_STAT_IF_PKT_SIZES;

    if (propval) {
        if (sal_strcmp(propval, "64B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_65B;
        } else if (sal_strcmp(propval, "128B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_126B;
        } else if (sal_strcmp(propval, "256B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_248B;
        } else if (sal_strcmp(propval, "512B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_492B;
        } else if (sal_strcmp(propval, "1024B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_1024B;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));  
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_stat_if_qsize_pkt_size_get(int unit, SOC_TMC_STAT_IF_PKT_SIZE *stat_if_pkt_size)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_PKT_SIZE;
    propval = soc_property_get_str(unit, propkey);    
    *stat_if_pkt_size = SOC_TMC_NOF_STAT_IF_PKT_SIZES;

    if (propval) {
        if (sal_strcmp(propval, "65B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_65B;
        } else if (sal_strcmp(propval, "126B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_126B;
        } else if (sal_strcmp(propval, "248B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_248B;
        } else if (sal_strcmp(propval, "492B") == 0) {
            *stat_if_pkt_size = SOC_TMC_STAT_IF_PKT_SIZE_492B;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey)); 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_parse_ucode_port(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID *p_val, uint32 *channel, uint32* is_hg)
{
    int found;
    uint32 local_chan;
    char *propkey, *propval, *s, *ss;
    char *prefix;
    int prefix_len, id;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_UCODE_PORT;
    propval = soc_property_port_get_str(unit, port, propkey);    

    if(is_hg) {
        (*is_hg) = 0;
    }

    s = propval;
    found = 0;
    *channel=0;

    /* Parse ARAD interfaces */
    if (propval) {

        /* Disable/Ignore the Interface */
        if (!found) {
            prefix = "IGNORE";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                *p_val = SOC_TMC_NIF_ID_NONE;
                found = 1;
            }
        }

        if (!found) {
            prefix = "XAUI";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);                
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                }        
                *p_val = ARAD_NIF_ID(XAUI,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "RXAUI";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);                
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                }        
                *p_val = ARAD_NIF_ID(RXAUI,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "SGMII";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                }        
                *p_val = ARAD_NIF_ID(SGMII,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "ILKN";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);                
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                }        
                *p_val = ARAD_NIF_ID(ILKN,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "10GBase-R";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);                
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                }        
                *p_val = ARAD_NIF_ID(10GBASE_R,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "XLGE";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);                
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                }        
                *p_val = ARAD_NIF_ID(XLGE,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "CGE";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);                
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
                }        
                *p_val = ARAD_NIF_ID(CGE,id);
                found = 1;
                s = ss;
            }
        }


        if (!found) {
            prefix = "CPU";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                *p_val = ARAD_IF_ID_CPU;
                found = 1;
            }
        }

        if (!found) {
            prefix = "RCY";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                *p_val = ARAD_IF_ID_RCY;
                found = 1;
            }
        }

        if (!found) {
            prefix = "TM_INTERNAL_PKT";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {

                if (!SOC_IS_ARDON(unit)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("TM_INTERNAL_PKT Interface is Enabled only for Ardon device. property value (\"%s\") for %s"), propval, propkey)); 
                }

                s += sal_strlen(prefix);
                *p_val = ARAD_IF_ID_TM_INTERNAL_PKT;
                found = 1;
            }
        }

        if (!found) {
            prefix = "RESERVED";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {

                s += sal_strlen(prefix);
                *p_val = ARAD_IF_ID_RESERVED;
                found = 1;
            }
        }
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey)); 
        }
    } else {
        *p_val = SOC_TMC_NIF_ID_NONE;      
    }    


    if (s && (*s == '.')) {
        /* Parse channel number */
        ++s;
        local_chan = sal_ctoi(s, &ss);
        if (s != ss) {
            *channel = local_chan;
        }
        s = ss;
    }

    while (s && (*s == ':')) {
        ++s;

        /* Check if higig port */
        prefix = "hg";
        prefix_len = sal_strlen(prefix);

        if (!sal_strncasecmp(s, prefix, prefix_len)) {
            s += sal_strlen(prefix);
            if(is_hg) {
                (*is_hg) = 1;
            }
        } else {
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_parse_ucode_port_erp_lane(int unit, int *lane)
{
    char *propkey, *propval, *s, *ss;
    char *prefix;
    int prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_UCODE_PORT;   
    propval = soc_property_suffix_num_only_suffix_str_get(unit, 0, propkey, "erp");

    s = propval;

    /* Parse ERP lane */
    if (propval) {
        prefix = "ERP";
        prefix_len = sal_strlen(prefix);

        if (!sal_strncasecmp(s, prefix, prefix_len)) {
            s += sal_strlen(prefix);                
            *lane = sal_ctoi(s, &ss);
            if (s == ss) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey)); 
            }        
            s = ss;
        }
    } else {
        /* indicates no lane is specified */
        *lane = -1;      
    }    

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_parse_pon_port(int unit, soc_port_t port, uint32* is_pon)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_PON_APPLICATION_SUPPORT_ENABLED;    
    propval = soc_property_port_get_str(unit, port, propkey);    

    SOCDNX_NULL_CHECK(is_pon);
    (*is_pon) = 0;

    if (propval) {      
        if (sal_strcmp(propval, "TRUE") == 0) {
            (*is_pon) = 1;
        }
        else {
            (*is_pon) = 0;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_str_prop_parse_init_vid_enabled_port_get(int unit, soc_port_t port, uint32 *init_vid_enabled)
{
    char  *propkey;
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(init_vid_enabled);

    propkey = spn_VLAN_TRANSLATION_INITIAL_VLAN_ENABLE;    
    propval = soc_property_port_get(unit, port, propkey, 1);    
   
    if (propval) {
        (*init_vid_enabled) = 1;
    }
    else {
        (*init_vid_enabled) = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_parse_mact_learn_limit_mode(int unit, uint8 *l2_learn_limit_mode)
{
    char *prop_key, *prop_str;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* Set the default value for L2_LEARN_LIMIT_MODE */
    *l2_learn_limit_mode = SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI;

    /* Get the SOC Property value for the L2_LEARN_LIMIT_MODE */
    prop_key = spn_L2_LEARN_LIMIT_MODE;
    prop_str = soc_property_get_str(unit, prop_key);
    if (prop_str != NULL) {
        if ((sal_strcmp(prop_str, "VLAN") == 0) || (sal_strcmp(prop_str, "0") == 0)) {
            *l2_learn_limit_mode = SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI;
        } else if ((sal_strcmp(prop_str, "VLAN_PORT") == 0) || (sal_strcmp(prop_str, "1") == 0)) {
            *l2_learn_limit_mode = SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF;
        } else if ((sal_strcmp(prop_str, "TUNNEL") == 0) || (sal_strcmp(prop_str, "2") == 0)) {
            *l2_learn_limit_mode = SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE_TUNNEL;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), prop_str, prop_key));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#ifdef BCM_88660_A0
STATIC int
soc_arad_str_prop_parse_mact_learn_limit_range(int unit, int32 *used_lif_range)
{
    char *prop_key, *prop_str;
    uint32  lif_range_ix;

    SOCDNX_INIT_FUNC_DEFS;

    /* Set the default values for L2_LEARN_LIF_RANGE_BASE */
    used_lif_range[0] = 0;
    used_lif_range[1] = 0x4000;

    /* Get the SOC Property value for the L2_LEARN_LIF_RANGE_BASE and store the range num 0-3 */
    prop_key = spn_L2_LEARN_LIF_RANGE_BASE;
    for (lif_range_ix = 0; lif_range_ix < SOC_DPP_MAX_NOF_MACT_LIMIT_MAPPED_LIF_RANGES; lif_range_ix++) {
        prop_str = soc_property_port_get_str(unit, lif_range_ix, prop_key);
        if (prop_str != NULL) {
            if (sal_strcmp(prop_str, "0") == 0) {
                used_lif_range[lif_range_ix] = 0;
            } else if (sal_strcmp(prop_str, "16K") == 0) {
                used_lif_range[lif_range_ix] = 0x4000;
            } else if (sal_strcmp(prop_str, "32K") == 0) {
                used_lif_range[lif_range_ix] = 0x8000;
            } else if (sal_strcmp(prop_str, "48K") == 0) {
                used_lif_range[lif_range_ix] = 0xC000;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), prop_str, prop_key));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

int
soc_arad_validate_fabric_mode(int unit, SOC_TMC_FABRIC_CONNECT_MODE *fabric_connect_mode)
{
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {
        case BCM88350_DEVICE_ID:
        case BCM88351_DEVICE_ID:
        case BCM88360_DEVICE_ID:
        case BCM88361_DEVICE_ID:
        case BCM88363_DEVICE_ID:
            if(*fabric_connect_mode != SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP) { /* disable fabric */
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "device support only fabric_connect_mode - SINGLE_FAP mode. fabric connect mode SINGLE_FAP enforced.\n")));
                *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP;
            }
            break;

        case BCM88450_DEVICE_ID:
        case BCM88451_DEVICE_ID:
        case BCM88460_DEVICE_ID:
        case BCM88461_DEVICE_ID:
            if(*fabric_connect_mode != SOC_TMC_FABRIC_CONNECT_MODE_MESH && 
               *fabric_connect_mode !=SOC_TMC_FABRIC_CONNECT_MODE_BACK2BACK) {
                
                *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_MESH; 
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "device support only fabric_connect_mode - MESH mode (or BACK2BACK). fabric connect mode MESH enforced.\n")));
                    
             }    
             break;

        default:
            break;       
    }
    

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_ilkn_counters_mode_get(int unit, uint32 *ilkn_counters_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_ILKN_COUNTERS_MODE;
    propval = soc_property_get_str(unit, propkey);   
    if (propval) {
        if (sal_strcmp(propval, "STAT_PER_PHYSICAL_PORT") == 0) {
            *ilkn_counters_mode = soc_arad_stat_ilkn_counters_mode_physical;
        } else if (sal_strcmp(propval, "PACKET_PER_CHANNEL") == 0) {
            *ilkn_counters_mode = soc_arad_stat_ilkn_counters_mode_packets_per_channel;
        }
    } else {
        /*Default Mode*/
        *ilkn_counters_mode = soc_arad_stat_ilkn_counters_mode_physical;
    }

#if defined(INCLUDE_KBP) && !defined(BCM_88030) && defined(BCM_88660)
    if(SOC_IS_ARADPLUS(unit) && (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode) && 
       (*ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_packets_per_channel)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn counter mode packet per channel is not supported while ext_interface_mode_enabled")));
    }
#endif
     
#if defined(INCLUDE_KBP) && !defined(BCM_88030) && defined(BCM_88660)   
exit:
#endif
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_lag_mode_get(int unit, SOC_TMC_PORT_LAG_MODE *lag_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_NUMBER_OF_TRUNKS;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "1024") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_1K_16;
        } else if (sal_strcmp(propval, "512") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_512_32;
        } else if (sal_strcmp(propval, "256") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_256_64;
        } else if (sal_strcmp(propval, "128") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_128_128;
        } else if (sal_strcmp(propval, "64") == 0) {
            *lag_mode = SOC_TMC_PORT_LAG_MODE_64_256;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *lag_mode = SOC_TMC_PORT_LAG_MODE_1K_16;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_swap_info_get(int unit, ARAD_SWAP_INFO *swap_info)
{
    char *propkey;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(swap_info, 0, sizeof (*swap_info));

    /* Set the swap offset according to the SOC Properties */
    propkey = spn_PREPEND_TAG_OFFSET;
    swap_info->offset = soc_property_get(unit, propkey, 0x0);

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        char *propval;

        /* Get the swap mode from the SOC Properties */
        propkey = spn_PREPEND_TAG_BYTES;
        propval = soc_property_get_str(unit, propkey);    

        /* Set the init swap mode */
        if (propval) {
            if (sal_strcmp(propval, "4B") == 0) {
                swap_info->mode = SOC_TMC_SWAP_MODE_4_BYTES;
            } else if (sal_strcmp(propval, "8B") == 0) {
                swap_info->mode = SOC_TMC_SWAP_MODE_8_BYTES;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
            }
        } else {
            swap_info->mode = SOC_TMC_SWAP_MODE_4_BYTES;
        }
    }
#endif

#ifdef BCM_88660_A0
exit:
#endif
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_synce_mode_get(int unit, ARAD_NIF_SYNCE_MODE *mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SYNC_ETH_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "TWO_DIFF_CLK") == 0) {
            *mode = ARAD_NIF_SYNCE_MODE_TWO_DIFF_CLK;
        } else if (sal_strcmp(propval, "TWO_CLK_AND_VALID") == 0) {
            *mode = ARAD_NIF_SYNCE_MODE_TWO_CLK_AND_VALID;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *mode = ARAD_NIF_SYNCE_MODE_TWO_CLK_AND_VALID;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_synce_clk_div_get(int unit, int ind, ARAD_NIF_SYNCE_CLK_DIV *clk_div)
{
    int propval;

    SOCDNX_INIT_FUNC_DEFS;

    propval = soc_property_suffix_num_get(unit, ind, spn_SYNC_ETH_CLK_DIVIDER, "clk_", 0x0);    

    if (propval) {
        if (propval == 1) {
            *clk_div = ARAD_NIF_SYNCE_CLK_DIV_1;
        } else if (propval == 2) {
            *clk_div = ARAD_NIF_SYNCE_CLK_DIV_2;
        } else if (propval == 4) {
            *clk_div = ARAD_NIF_SYNCE_CLK_DIV_4;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for %s"), propval, spn_SYNC_ETH_CLK_DIVIDER));
        }
    } else {
        *clk_div = ARAD_NIF_SYNCE_CLK_DIV_1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC int
soc_arad_str_prop_nif_elk_tcam_dev_type_get(int unit, ARAD_NIF_ELK_TCAM_DEV_TYPE *tcam_dev_type)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_TCAM_DEV_TYPE;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "NONE") == 0) {
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE;
        } else if ((sal_strcmp(propval, "NL88650") == 0) || (sal_strcmp(propval, "NL88650A") == 0) ){
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NL88650A;
        } else if ((sal_strcmp(propval, "NL88650B") == 0)){
            *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NL88650B;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *tcam_dev_type = ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_nif_elk_fwd_table_size_get(int unit, ARAD_INIT_ELK *elk)
{
    int shared_ip_mpls_tbl_size = 0, extended_ipv6_tbl_size = 0;
    SOCDNX_INIT_FUNC_DEFS;

    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0] = 
        SOC_SAND_MAX(soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0), soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0x0));
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1] = 
        SOC_SAND_MAX(soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0), soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0x0));
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC] = soc_property_get(unit, spn_EXT_IP4_MC_FWD_TABLE_SIZE, 0x0);   
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0] = soc_property_get(unit, spn_EXT_IP6_UC_RPF_FWD_TABLE_SIZE, 0x0);
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1] = soc_property_get(unit, spn_EXT_IP6_UC_RPF_FWD_TABLE_SIZE, 0x0);
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_MC] = soc_property_get(unit, spn_EXT_IP6_MC_FWD_TABLE_SIZE, 0x0);
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_UC] = soc_property_get(unit, spn_EXT_TRILL_UC_FWD_TABLE_SIZE, 0x0);
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_MC] = soc_property_get(unit, spn_EXT_TRILL_MC_FWD_TABLE_SIZE, 0x0);
    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR] = soc_property_get(unit, spn_EXT_MPLS_FWD_TABLE_SIZE, 0x0);

    shared_ip_mpls_tbl_size = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv4_mpls_extended_fwd_table_size", 0);
    if(shared_ip_mpls_tbl_size != 0){        
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED] = shared_ip_mpls_tbl_size;
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP] = shared_ip_mpls_tbl_size;
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR] = shared_ip_mpls_tbl_size;
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1] = shared_ip_mpls_tbl_size;
    }
    
    extended_ipv6_tbl_size = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv6_extended_fwd_table_size", 0);
    if(extended_ipv6_tbl_size != 0){        
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6] = extended_ipv6_tbl_size;
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1] = extended_ipv6_tbl_size;        
        elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0] = 8;/* Dummy size for the DB (min size is 8) */
    }

    elk->fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P] = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_p2p_extended_fwd_table_size", 0);
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_is_device_elk_disabled(int unit)
{
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    if(driver_dev_id == BCM88654_DEVICE_ID ||
       driver_dev_id == BCM88664_DEVICE_ID) {

        return TRUE;
    }

    return FALSE;
}

#endif /* INCLUDE_KBP */

int
soc_arad_ilkn_phy_ports_btmp_set(int unit, uint32 ilkn_id, soc_pbmp_t *phy_ports)
{
    int i;
    uint32 lanes;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*phy_ports);
    lanes = soc_property_port_get(unit, ilkn_id, spn_ILKN_NUM_LANES, 0);

#if defined(INCLUDE_KBP) && !defined(BCM_88030) && defined(BCM_88660_A0)
    if(SOC_IS_ARADPLUS(unit) && (ilkn_id == 1) && (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode)){
        uint32 phy_interfaces = 0;
        
        phy_interfaces = _SHR_PBMP_WORD_GET(*phy_ports , 0);
        switch(lanes){
            case 4:
                phy_interfaces |= EXT_MODE_4LANES_PHY_PORTS;
                break;
            case 8:
                phy_interfaces |= EXT_MODE_8LANES_PHY_PORTS;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Support just 4 or 8 lanes in ext_interface_mode")));
        }
        _SHR_PBMP_WORD_SET(*phy_ports, 0, phy_interfaces);
    }
    else
#endif /* INCLUDE_KBP */
    {
        for(i=0;i<lanes;i++) {
            if(i<12) {
                SOC_PBMP_PORT_ADD(*phy_ports, ilkn_id*16+i+1);
            } else{
                SOC_PBMP_PORT_ADD(*phy_ports, 40-i);
            }
        }
    }

#if defined(INCLUDE_KBP) && !defined(BCM_88030) && defined(BCM_88660_A0)
exit:
#endif
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_validate_device_core_frequency(int unit, int frequency)
{
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    if( driver_dev_id == BCM88351_DEVICE_ID ||
        driver_dev_id == BCM88451_DEVICE_ID ||
        driver_dev_id == BCM88551_DEVICE_ID ||
        driver_dev_id == BCM88552_DEVICE_ID ||
        driver_dev_id == BCM88651_DEVICE_ID ||
        driver_dev_id == BCM88361_DEVICE_ID ||
        driver_dev_id == BCM88461_DEVICE_ID ||
        driver_dev_id == BCM88561_DEVICE_ID ||
        driver_dev_id == BCM88562_DEVICE_ID ||
        driver_dev_id == BCM88661_DEVICE_ID) {

        if(frequency > 500000) {
            return -1;
        }
    }

    return 0;
}

STATIC int
soc_arad_str_prop_voq_mapping_mode_get(int unit, int8 *voq_mapping_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_VOQ_MAPPING_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "DIRECT") == 0) {
            *voq_mapping_mode = VOQ_MAPPING_DIRECT;
        } else if (sal_strcmp(propval, "INDIRECT") == 0) {
            *voq_mapping_mode = VOQ_MAPPING_INDIRECT;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *voq_mapping_mode = VOQ_MAPPING_INDIRECT;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_action_type_source_mode_get(int unit, int8 *action_type_source_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_ACTION_TYPE_SIGNATURE_STAMPING;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "FORWARDING_ACTION") == 0) {
            *action_type_source_mode = ACTION_TYPE_FROM_FORWARDING_ACTION;
        } else if (sal_strcmp(propval, "QUEUE_SIGNATURE") == 0) {
            *action_type_source_mode = ACTION_TYPE_FROM_QUEUE_SIGNATURE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *action_type_source_mode = ACTION_TYPE_FROM_FORWARDING_ACTION;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_egress_shared_resources_mode_get(int unit, SOC_TMC_EGR_QUEUING_PARTITION_SCHEME *egress_shared_resources_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EGRESS_SHARED_RESOURCES_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "Strict") == 0) {
            *egress_shared_resources_mode = SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_STRICT;
        } else if (sal_strcmp(propval, "Discrete") == 0) {
            *egress_shared_resources_mode = SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_DISCRETE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *egress_shared_resources_mode = SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_STRICT;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_credit_worth_resolution_get(int unit, ARAD_MGMT_CREDIT_WORTH_RESOLUTION *credit_worth_resolution)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_CREDIT_WORTH_RESOLUTION;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "high") == 0) {
            *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_HIGH;
        } else if (sal_strcmp(propval, "medium") == 0) {
            *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_MEDIUM;
        } else if (sal_strcmp(propval, "low") == 0) {
            *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_LOW;
        } else if (sal_strcmp(propval, "auto") == 0) {
            *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_AUTO;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *credit_worth_resolution = ARAD_MGMT_CREDIT_WORTH_RESOLUTION_AUTO;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_external_voltage_mode_get(int unit, ARAD_MGMT_EXT_VOLT_MOD *external_voltage_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_VOLTAGE_MODE;
    propval = soc_property_suffix_num_str_get(unit, -1, spn_EXT_VOLTAGE_MODE, "oob");    

    if (propval) {
        if (sal_strcmp(propval, "HSTL_1.5V") == 0) {
            *external_voltage_mode=ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V;
        } else if (sal_strcmp(propval, "3.3V") == 0) {
            *external_voltage_mode=ARAD_MGMT_EXT_VOL_MOD_3p3V;
        } else if (sal_strcmp(propval, "HSTL_1.5V_VDDO_DIV_2") == 0) {
            *external_voltage_mode=ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V_VDDO;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *external_voltage_mode = ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_dpp_str_prop_xgs_compatible_system_port_mode_get(int unit, int8 *xgs_compatible_system_port_mode)
{
    uint32 propval;
    char *propkey;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_HIGIG_FRC_TM_SYSTEM_PORT_ENCODING;
    propval = soc_property_get(unit, propkey, SOC_DPP_XGS_TM_7_MODID_8_PORT);    

    switch (propval) {   
        case SOC_DPP_XGS_TM_7_MODID_8_PORT:
            *xgs_compatible_system_port_mode = SOC_DPP_XGS_TM_7_MODID_8_PORT;
            break;
        case SOC_DPP_XGS_TM_8_MODID_7_PORT:
            *xgs_compatible_system_port_mode = SOC_DPP_XGS_TM_8_MODID_7_PORT;
            break;
        default:
            *xgs_compatible_system_port_mode = SOC_DPP_XGS_TM_7_MODID_8_PORT;
    }

    SOCDNX_FUNC_RETURN;
}

int 
soc_arad_is_olp(int unit, soc_port_t port, uint32* is_olp)
{
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    *is_olp = (SOC_PORT_IF_OLP == interface ? 1: 0);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_arad_is_oamp(int unit, soc_port_t port, uint32* is_oamp)
{
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    *is_oamp = (SOC_PORT_IF_OAMP == interface ? 1: 0);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_parse_mpls_termination_database_mode(int unit, uint8 mpls_termination_label_index_enable, uint8 mpls_termination_rif_enable, uint8 *mpls_database, SOC_PPC_MPLS_TERM_NAMESPACE_TYPE  *mpls_namespace)
{
    int default_value, mode;

    SOCDNX_INIT_FUNC_DEFS;

    if (mpls_termination_label_index_enable) {
        default_value = 2; /* deafult mode for MPLS indexed */
    } else {
        default_value = 0; /* default mode for MPLS unindex */
    }

    mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, default_value);
    
    /* Initialize to invalid */
    mpls_database[0] = mpls_database[1] = mpls_database[2] = 0xff;

    /* Verify mode match other soc properties */
    if ((mode == 0 || mode == 1) &&
        mpls_termination_label_index_enable) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE when mpls_termination_label_index_enable is set"), mode));
    }

    if (!mpls_termination_label_index_enable && mode > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE when mpls_termination_label_index_enable is unset"), mode));
    }

    if ((mode == 6 || mode == 7) &&
        mpls_termination_rif_enable) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE when mpls_termination_rif_enable is set"), mode));
    }

    /* ARAD+ and above only check */
    if (SOC_IS_ARAD_B1_AND_BELOW(unit) && mode == 10) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE for ARAD-B and below"), mode));
    }

    /* Tunel termination only in tt stage according to MPLS termination modes */
    SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only = (mode == 10)  ? 1:0;
    
    switch (mode) {
        case 0:
            /* 0: MPLS_1 refers to label namespaces L1,L2. MPLS_1 is located in SEM-B. */
            mpls_database[0] =  1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            break;
        case 1:
            /* 1: MPLS_1 refers to label namespaces L1,L2. MPLS_1 is located in SEM-A. */
            mpls_database[0] =  0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            break;
        case 2:
            /* 2: MPLS_x refers to label namespace Lx. MPLS_1,_3 is located in SEM-B, MPLS_2 is located in SEM-A. */
            mpls_database[0] = 1;
            mpls_database[1] = 0;
            mpls_database[2] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            mpls_namespace[2] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 3:
            /* 3: MPLS_x refers to label namespace Lx. MPLS_1,_3 is located in SEM-A, MPLS_2 is located in SEM-B. */
            mpls_database[0] = 0;
            mpls_database[1] = 1;
            mpls_database[2] = 0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            mpls_namespace[2] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 4:
            /* 4: MPLS_x refers to label namespace Lx. MPLS_1,_2 is located in SEM-B, MPLS_3 is located in SEM-A. */
            mpls_database[0] = 1;
            mpls_database[1] = 1;
            mpls_database[2] = 0;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            mpls_namespace[2] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 5:
            /* 5: MPLS_x refers to label namespace Lx. MPLS_1,_2 is located in SEM-A, MPLS_3 is located in SEM-B. */
            mpls_database[0] = 0;
            mpls_database[1] = 0;
            mpls_database[2] = 1;
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;
            mpls_namespace[2] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;
            break;
        case 6:
            /* 6: MPLS_1 refers to label namespace L1,L2 and located in SEM-A. MPLS_2 refers to label namespace L3 and located in SEM-B. */
            mpls_database[0] = 0;
            mpls_database[1] = 1;        
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;        
            break;
        case 7:
            /* 7: MPLS_1 refers to label namespace L1,L2 and located in SEM-B. MPLS_2 refers to label namespace L3 and located in SEM-A. */
            mpls_database[0] = 1;
            mpls_database[1] = 0;        
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L3;    
            break;
        case 8:
            /* 8: MPLS_1 refers to label namespace L1,L3 and located in SEM-A. MPLS_2 refers to label namespace L2 and located in SEM-B. */
            mpls_database[0] = 0;
            mpls_database[1] = 1;        
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;    
            break;
        case 9:
            /* 9: MPLS_1 refers to label namespace L1,L3 and located in SEM-B. MPLS_2 refers to label namespace L2 and located in SEM-A. */
            mpls_database[0] = 1;
            mpls_database[1] = 0;        
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;    
            break;
        case 10:
            /* 10: MPLS_1 refers to label namespace L1 and located in SEM-A. MPLS_2 refers to label namespace L2 and located in SEM-B. */
            mpls_database[0] = 0;
            mpls_database[1] = 1;        
            mpls_namespace[0] = SOC_PPC_MPLS_TERM_NAMESPACE_L1;
            mpls_namespace[1] = SOC_PPC_MPLS_TERM_NAMESPACE_L2;    
            break;
      default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for BCM886XX_MPLS_TERMINATION_DATABASE_MODE"), mode));
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_prop_parse_dram_number_of_rows(int unit, SOC_TMC_DRAM_NUM_ROWS *p_val) 
{
    char *propkey;
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_RAM_ROWS;
    propval = soc_property_get(unit, propkey, SOC_TMC_NOF_DRAM_NUM_ROWS);    
    switch (propval) {   
        case 8192:
            *p_val = SOC_TMC_DRAM_NUM_ROWS_8192;
            break;
        case 16384:
            *p_val = SOC_TMC_DRAM_NUM_ROWS_16384;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_arad_dram_nof_buffs_calc(int unit, 
                             int dram_size_total_mbyte, 
                             ARAD_ITM_DBUFF_SIZE_BYTES  dbuff_size, 
                             ARAD_INIT_PDM_MODE pdm_mode, 
                             int *nof_dram_buffs)
{
    int rv = SOC_E_NONE,
        max_buffs_by_dram;
    uint32 pdm_nof_entries;

    SOCDNX_INIT_FUNC_DEFS;

    rv = handle_sand_result(arad_init_pdm_nof_entries_calc(unit, pdm_mode, &pdm_nof_entries));
    SOCDNX_IF_ERR_EXIT(rv);

    max_buffs_by_dram = (dram_size_total_mbyte * 1024 * 1024)/dbuff_size;
    SOC_SAND_LIMIT_FROM_ABOVE(max_buffs_by_dram, ARAD_ITM_NOF_DRAM_BUFFS);

    *nof_dram_buffs = SOC_SAND_MIN(pdm_nof_entries, max_buffs_by_dram);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_dram_param_set(int unit, soc_dpp_config_arad_t *dpp_arad)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    /* DBuff configuration */
    dpp_arad->init.dram.dbuff_size                     = soc_property_get(unit, spn_EXT_RAM_DBUFF_SIZE, ARAD_ITM_DBUFF_SIZE_BYTES_1024);

    dpp_arad->init.dram.dram_size_total_mbyte          = soc_property_get(unit, spn_EXT_RAM_TOTAL_SIZE, 200);
    dpp_arad->init.dram.dram_user_buffer_size_mbytes   = soc_property_suffix_num_get(unit, -1, spn_USER_BUFFER_SIZE, "dram", 0x0);
    dpp_arad->init.dram.dram_device_buffer_size_mbytes = dpp_arad->init.dram.dram_size_total_mbyte - dpp_arad->init.dram.dram_user_buffer_size_mbytes;

    /* PDM mode */
    switch(soc_property_get(unit, spn_BCM886XX_PDM_MODE , 0))
    {
        case 0:
            dpp_arad->init.dram.pdm_mode = ARAD_INIT_PDM_MODE_SIMPLE;
            break;
        case 1:
            dpp_arad->init.dram.pdm_mode = ARAD_INIT_PDM_MODE_REDUCED;
            break;
        default:
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    }

    /* Calc number of dram buffers */
    rv = soc_arad_dram_nof_buffs_calc(unit, 
                                           dpp_arad->init.dram.dram_device_buffer_size_mbytes, 
                                           dpp_arad->init.dram.dbuff_size, 
                                           dpp_arad->init.dram.pdm_mode,
                                           &dpp_arad->init.dram.nof_dram_buffers);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Set User dram buffer start ptr (first buffer number) */
    dpp_arad->init.dram.dram_user_buffer_start_ptr = dpp_arad->init.dram.nof_dram_buffers + 1;

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_str_prop_parse_otm_port_rate_type(int unit, soc_port_t port, SOC_TMC_EGR_PORT_SHAPER_MODE *p_val)
{
    int rv = SOC_E_NONE;
    char *propkey, *propval;
    uint32 is_olp = 0, is_oamp = 0;
    soc_port_if_t interface;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_OTM_PORT_PACKET_RATE;
    propval = soc_property_port_get_str(unit, port, propkey);

    if (propval) {
        if (sal_strcmp(propval, "DATA") == 0) {
            *p_val = SOC_TMC_EGR_PORT_SHAPER_DATA_MODE;
        } else if (sal_strcmp(propval, "PACKET") == 0) {
            *p_val = SOC_TMC_EGR_PORT_SHAPER_PACKET_MODE;     
        } else  {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {          
        rv = soc_port_sw_db_interface_type_get(unit, port, &interface);
        SOCDNX_IF_ERR_EXIT(rv);

        is_olp = (SOC_PORT_IF_OLP == interface ? 1: 0);
        is_oamp = (SOC_PORT_IF_OAMP == interface ? 1 : 0);

        /* By default each port should be in data mode except CMIC or OLP ports */
        if (is_olp || is_oamp || (SOC_PBMP_MEMBER(PBMP_CMIC(unit), port))) {
            *p_val = SOC_TMC_EGR_PORT_SHAPER_PACKET_MODE;
        }
        else {
            *p_val = SOC_TMC_EGR_PORT_SHAPER_DATA_MODE;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

void
soc_arad_dma_mutex_destroy(int unit)
{
    soc_control_t *soc;
    
    soc = SOC_CONTROL(unit);

    if (soc->tslamDmaIntr != NULL) {
        sal_sem_destroy(soc->tslamDmaIntr);
        soc->tslamDmaIntr = NULL;
    }

    if (soc->tslamDmaMutex != NULL) {
        sal_mutex_destroy(soc->tslamDmaMutex );
        soc->tslamDmaMutex  = NULL;
    }

    if (soc->tableDmaIntr!= NULL) {
        sal_sem_destroy(soc->tableDmaIntr);
        soc->tableDmaIntr = NULL;
    }

    if (soc->tableDmaMutex != NULL) {
        sal_mutex_destroy(soc->tableDmaMutex);
        soc->tableDmaMutex  = NULL;
    }
}

int
soc_arad_deinit(int unit)
{
    int rv;     

    SOCDNX_INIT_FUNC_DEFS;

    /* 
     *  Deinit interrupts 
     */
     
    /* Make sure all interrupts are masked, 
     * Application should have already Mask all interrupts in Cmic.
     * This Masking update WB DB, althoght we dont use WB DB. 
     */ 
    soc_cmicm_intr2_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_2_MASK);
    soc_cmicm_intr3_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_3_MASK);
    soc_cmicm_intr4_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_2_MASK);

    arad_interrupts_array_deinit(unit);
    
    if ((!SAL_BOOT_NO_INTERRUPTS)) {
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_disconnect(unit) < 0) {
               LOG_ERROR(BSL_LS_SOC_INIT,
                         (BSL_META_U(unit,
                                     "error disconnecting polled interrupt mode\n")));
            }
        } else {
            /* unit # is ISR arg */
            if (soc_cm_interrupt_disconnect(unit) < 0) {
               LOG_ERROR(BSL_LS_SOC_INIT,
                         (BSL_META_U(unit,
                                     "could not disconnect interrupt line\n")));
            }
        }
    }

    rv = soc_linkctrl_deinit(unit);
    if (SOC_FAILURE(rv)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Error Running soc_linkctrl_deinit.")));
    }


#ifdef BCM_SBUSDMA_SUPPORT        
    if ((rv=soc_sbusdma_desc_detach(unit)) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_sbusdma_desc_detach returns error.")));
    }
#endif
    SOCDNX_IF_ERR_EXIT(dcmn_deinit_fill_table(unit));

    /* Detach DMA */
    soc_arad_dma_mutex_destroy(unit);
 
    rv = soc_dma_detach(unit);
    if (SOC_FAILURE(rv)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dma_detach returns error.")));
    }

    _soc_dpp_port_map_deinit(unit); /* pp_ports     tm_ports*/

#if defined(BCM_WARM_BOOT_SUPPORT)
    soc_port_sw_db_destroy(unit);
    arad_wb_db_deinit(unit); /* wb egr_port*/
    soc_scache_detach(unit); 
#endif
    arad_tbl_access_deinit(unit);
    arad_pp_isem_access_deinit(unit);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_attach(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Allocate Arad configuration Structure */
    if (SOC_IS_ARAD(unit)) {
        SOC_DPP_CONFIG(unit)->arad = sal_alloc(sizeof (soc_dpp_config_arad_t), "arad_config");
        if (SOC_DPP_CONFIG(unit)->arad == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to alloc arad_config")));
        }
        sal_memset(SOC_DPP_CONFIG(unit)->arad, 0x0, sizeof(soc_dpp_config_arad_t)); 
    }

    if (SOC_IS_ARADPLUS(unit)) {
        SOC_DPP_CONFIG(unit)->arad_plus = sal_alloc(sizeof (soc_dpp_config_arad_plus_t), "arad_plus_config");
        if (SOC_DPP_CONFIG(unit)->arad_plus == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to alloc arad_plus_config")));
        }
        sal_memset(SOC_DPP_CONFIG(unit)->arad_plus, 0x0, sizeof(soc_dpp_config_arad_plus_t)); /* All FAPs initialized to local credit value */
    }

#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit)) {
        SOC_DPP_CONFIG(unit)->jer = sal_alloc(sizeof(soc_dpp_config_jer_t), "soc_dpp_config_jer_t");
        if (SOC_DPP_CONFIG(unit)->jer == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to alloc soc_dpp_config_jer_t")));
        }
        sal_memset(SOC_DPP_CONFIG(unit)->jer, 0x0, sizeof(soc_dpp_config_jer_t)); /* All FAPs initialized to local credit value */
    }
#endif /* BCM_JERICHO_SUPPORT */

exit:
    SOCDNX_FUNC_RETURN;
}


/* find a free local tm port up to the given maximum port and a free recycle channel and allocate them */
int
soc_arad_allocate_tm_port_in_range_and_recycle_channel(int unit, int max_port, int *tm_port, uint32 *channel)
{
    soc_port_t port;
    soc_port_t port_first = 0;
    uint32 tchannel, is_valid;
    uint8 channels[SOC_DPP_MAX_NOF_CHANNELS] = {0};
    soc_pbmp_t phy_ports, ports_bm;
    soc_port_if_t interface_type;
    int found = 0;

    SOCDNX_INIT_FUNC_DEFS;

    /* loop over all local ports, marking used recycle channels, and finding the first free port */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));
    SOC_PBMP_ITER(ports_bm, port){
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        if(SOC_PORT_IF_RCY == interface_type) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &tchannel));
             channels[tchannel] = 1;
        }
    }

    for (tchannel = 0; tchannel < SOC_DPP_MAX_NOF_CHANNELS && channels[tchannel]; ++tchannel); /* find the first free channel */
    if (tchannel >= SOC_DPP_MAX_NOF_CHANNELS) { /* if all channels are taken */
        return SOC_E_RESOURCE;
    }
    *channel = tchannel;

    if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
        port_first = 128;
    }
    
    /* first loop over reserved ports */  
    SOC_PBMP_ITER(SOC_DPP_CONFIG(unit)->arad->reserved_ports, port)
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
        if (!is_valid) {
            if (SOC_PBMP_MEMBER(SOC_DPP_CONFIG(unit)->arad->reserved_ports, port))
            {
                *tm_port = port;
                found = 1;
                break;
            }
        }
    }
    /* loop over local ports, finding the first free one */
    if (!found) {
        for (port = port_first; port < SOC_MAX_NUM_PORTS; ++port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
            if (!is_valid) {
                *tm_port = port;
                found = 1;
                break;
            }
        }
    }

    if (found) {
        SOC_PBMP_CLEAR(phy_ports);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_add(unit, port, 0, *channel, SOC_PORT_FLAGS_VIRTUAL_RCY_INTERFACE, SOC_PORT_IF_RCY, phy_ports));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, port, port));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, port, port));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 1));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ERROR: No availabe ports")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_free_tm_port_and_recycle_channel(int unit, int port)
{
    uint32 is_valid;
    soc_port_if_t interface_type;
    uint32 pp_port, tm_port;
    int core;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("invalid port %d"), port));
    }
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    if (interface_type != SOC_PORT_IF_RCY || tm_port != port || pp_port != port) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("port(%d) not allocated by soc_arad_allocate_tm_port_in_range_and_recycle_channel()"), port));
    }
   
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, port, _SOC_DPP_PORT_INVALID));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, port, _SOC_DPP_PORT_INVALID));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 0));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_remove(unit, port));
  
exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_fc_oob_mode_validate(int unit, int port)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}


STATIC int 
soc_arad_ps_static_mapping(int unit, soc_port_t port, int out_port_priority, int *is_static_mapping, int* base_q_pair)
{
    soc_dpp_config_arad_t *dpp_arad;
    int prop_invalid = 0xffffffff, val;
    soc_port_if_t interface_type;
    uint32 channel;
    SOCDNX_INIT_FUNC_DEFS;

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    *is_static_mapping = 0;

    /* Static mapping by SOC property. Explicit OTM-queue base pair assignment */
    val = soc_property_port_get(unit, port, spn_OTM_BASE_Q_PAIR, prop_invalid);
    if (val != prop_invalid) 
    {
        /* take qpair according to soc property value */
        *base_q_pair = val;
        *is_static_mapping = 1;
    } 
    else 
    {
       /* if not set by SOC property - check CPU ports according to channel */
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        if (SOC_PORT_IF_CPU == interface_type && !SOC_PBMP_MEMBER(dpp_arad->init.rcpu.slave_port_pbmp, port)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
            *base_q_pair = ARAD_PS_CPU_FIRST_VALID_QPAIR + channel;
            *is_static_mapping = 1;
        }
    }

    if(*is_static_mapping) {
        if((*base_q_pair)+out_port_priority >= ARAD_EGR_NOF_Q_PAIRS) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Port %d, qpair is out of range"), port));
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

/* ser reserved base q pairs fro isq and fmq, needed in case of dynamic nif */
STATIC 
int soc_arad_ps_reserved_mapping_init(int unit)
{
    int  val, i;
    char *propval;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(SOC_DPP_CONFIG(unit)->arad->reserved_isq_base_q_pair);
    SOC_PBMP_CLEAR(SOC_DPP_CONFIG(unit)->arad->reserved_fmq_base_q_pair);
    
    /* Static mapping by SOC property. Explicit OTM-queue base pair assignment for ISQ-ROOT */
    propval = soc_property_suffix_num_only_suffix_str_get(unit, 0, spn_OTM_BASE_Q_PAIR, "isq");
    if (propval != NULL) {
        val = _shr_ctoi(propval);
        if (val < 0 || val >= SOC_MAX_NUM_PORTS) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_arad_ps_reserved_mapping_init error in soc_arad_info_config")));
        }
        SOC_PBMP_PORT_ADD(SOC_DPP_CONFIG(unit)->arad->reserved_isq_base_q_pair, val);
    }

    /* Static mapping by SOC property. Explicit OTM-queue base pair assignment for FMQ-ROOT */
    for (i=0; i<4; i++) 
    {
        propval = soc_property_suffix_num_only_suffix_str_get(unit, i, spn_OTM_BASE_Q_PAIR, "fmq");
        if (propval != NULL) {
            val = _shr_ctoi(propval);
            if (val < 0 || val >= SOC_MAX_NUM_PORTS) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_arad_ps_reserved_mapping_init error in soc_arad_info_config")));
            }
            SOC_PBMP_PORT_ADD(SOC_DPP_CONFIG(unit)->arad->reserved_fmq_base_q_pair, val);
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_fmq_isq_hr_init(int unit, uint8 is_fmq)
{
    int index = 0, base_q_pair = 0, found = 0;

    SOCDNX_INIT_FUNC_DEFS;

    /* Allocate free Q-Pair for ISQ */
    if (!is_fmq && (SOC_DPP_CONFIG(unit)->tm.hr_isq == SOC_TMC_SCH_PORT_ID_INVALID_ARAD) /* PORT ISQ */) {
        for (base_q_pair=0; base_q_pair<SOC_MAX_NUM_PORTS; base_q_pair++) 
        {
            if (SOC_PBMP_MEMBER(SOC_DPP_CONFIG(unit)->arad->reserved_isq_base_q_pair, base_q_pair)) {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            SOCDNX_IF_ERR_EXIT(arad_ps_db_find_free_non_binding_ps(unit, 0, &base_q_pair));
        }

        SOC_DPP_CONFIG(unit)->tm.hr_isq = base_q_pair; 
    }
    
    /* Allocate free Q-Pairs (4) for FMQ */
    if (is_fmq && (SOC_DPP_CONFIG(unit)->tm.hr_fmqs[0] == SOC_TMC_SCH_PORT_ID_INVALID_ARAD) /* PORT FMQ */) {
        for (base_q_pair=0; base_q_pair<SOC_MAX_NUM_PORTS; base_q_pair++) 
        {
            if (SOC_PBMP_MEMBER(SOC_DPP_CONFIG(unit)->arad->reserved_isq_base_q_pair, base_q_pair) && index<4) {
                SOC_DPP_CONFIG(unit)->tm.hr_fmqs[index] = base_q_pair;
                index++;
            }
        }
        while (index < 4) {
            SOCDNX_IF_ERR_EXIT(arad_ps_db_find_free_non_binding_ps(unit, 0, &base_q_pair));
            SOC_DPP_CONFIG(unit)->tm.hr_fmqs[index] = base_q_pair;
            index++;
        }      
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_isq_hr_get(int unit, int *hr_isq)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_arad_fmq_isq_hr_init(unit,FALSE);
    SOCDNX_IF_ERR_EXIT(rv);

    *hr_isq = SOC_DPP_CONFIG(unit)->tm.hr_isq;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_fmq_base_hr_get(int unit, int** base_hr_fmq)
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_arad_fmq_isq_hr_init(unit,TRUE);
    SOCDNX_IF_ERR_EXIT(rv);

    *base_hr_fmq = &(SOC_DPP_CONFIG(unit)->tm.hr_fmqs[0]);

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_collect_nif_config(int unit) 
{
    uint32 nof_ilkn_ports, nof_caui_ports, profile_offset;

    SOCDNX_INIT_FUNC_DEFS;

    nof_ilkn_ports = SOC_DPP_DEFS_GET(unit, nof_interlaken_ports);
    nof_caui_ports = SOC_DPP_DEFS_GET(unit, nof_caui_ports);

    /* Interlaken attributes */
    for (profile_offset = 0; profile_offset < nof_ilkn_ports; profile_offset++) {
        SOCDNX_IF_ERR_EXIT(soc_arad_ilkn_config_get(unit,profile_offset));
    }

    /* CAUI attributes */
    for (profile_offset = 0; profile_offset < nof_caui_ports; profile_offset++) {
        SOCDNX_IF_ERR_EXIT(soc_arad_caui_config_get(unit,profile_offset));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* This function check if the device is ILKN disable or not. */
int
soc_arad_is_device_ilkn_disabled(int unit, int inlk)
{
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    if(driver_dev_id == BCM88350_DEVICE_ID ||
       driver_dev_id == BCM88351_DEVICE_ID ||
       driver_dev_id == BCM88450_DEVICE_ID ||
       driver_dev_id == BCM88451_DEVICE_ID ||
       driver_dev_id == BCM88550_DEVICE_ID ||
       driver_dev_id == BCM88551_DEVICE_ID || 
       driver_dev_id == BCM88552_DEVICE_ID ||
       driver_dev_id == BCM88360_DEVICE_ID ||
       driver_dev_id == BCM88361_DEVICE_ID ||
       driver_dev_id == BCM88460_DEVICE_ID ||
       driver_dev_id == BCM88461_DEVICE_ID ||
       driver_dev_id == BCM88560_DEVICE_ID ||
       driver_dev_id == BCM88561_DEVICE_ID 
       ) {

#if defined(INCLUDE_KBP) && !defined(BCM_88030)  
        /* if device is ILKN enable - then ILKN 1 must be configure */
         if(SOC_DPP_CONFIG(unit)->arad->init.elk.enable == FALSE || 
           (SOC_DPP_CONFIG(unit)->arad->init.elk.enable == TRUE && inlk == 0))        
#endif /* INCLUDE_KBP */
        return TRUE;
    }


    return FALSE;
}

/* This function validate the configured num of DRAM is supported in the device. */
STATIC int
soc_arad_validate_device_num_of_dram(int unit, uint32 val)
{
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {

        case BCM88351_DEVICE_ID:
        case BCM88451_DEVICE_ID:
        case BCM88551_DEVICE_ID:
        case BCM88651_DEVICE_ID:
        case BCM88361_DEVICE_ID:
        case BCM88461_DEVICE_ID:
        case BCM88561_DEVICE_ID:
        case BCM88661_DEVICE_ID:            
            if(val > 6) {
                SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
            }
            break;
        case BCM88552_DEVICE_ID:
        case BCM88562_DEVICE_ID:
         
            if(val > 4) {
                SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
            }
            break;   
            
        case BCM88664_DEVICE_ID:
        case BCM88560_DEVICE_ID:
            if(val != 0) {
                SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
            }
            break;
        default:
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 this function validate that ARAD support the nif in use
*/
STATIC int
soc_arad_validate_nif_in_use(int unit,  soc_pbmp_t phy_ports)
{
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;
    int port_i;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {

        case BCM88351_DEVICE_ID:
        case BCM88361_DEVICE_ID:
                
            for(port_i=17; port_i < 28; ++port_i) {
                /*  remove all the port that should not be config in the device */
                if(SOC_PBMP_MEMBER(phy_ports, port_i)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Port %d disable by the device, ( 0x%x pool A disabled)"), port_i, driver_dev_id));      
                }                
            }
            break;

            default:
                break;                
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

/* This function deduces the ocb enable mode from the config */
STATIC int 
soc_arad_ocb_enable_mode(int unit, ARAD_INIT_OCB *ocb)
{
    int val;
    char *val_string;

    SOCDNX_INIT_FUNC_DEFS;
    
    val_string = soc_property_get_str(unit, spn_BCM886XX_OCB_ENABLE);
    if (val_string == NULL) {
    /*default option*/
        ocb->ocb_enable = OCB_ENABLED;
    }
    else if ((sal_strcmp(val_string, "0") == 0) || (sal_strcmp(val_string, "FALSE") == 0)) {
        ocb->ocb_enable = OCB_DISABLED;
    }
    else if ((sal_strcmp(val_string, "1") == 0) || (sal_strcmp(val_string, "TRUE") == 0)) {
        ocb->ocb_enable = OCB_ENABLED;
    }
    else if (SOC_IS_ARADPLUS(unit)){
        if (sal_strcmp(val_string, "ONE_WAY_BYPASS") == 0) {
            val = soc_property_get(unit, spn_EXT_RAM_PRESENT,0);
            if (val==0) {
                ocb->ocb_enable = OCB_ONLY;
            }
            else if (val==1) {
                ocb->ocb_enable = OCB_ONLY_1_DRAM;
            }
            else if ((val>=2) && (val<=8)) {
                ocb->ocb_enable = OCB_DRAM_SEPARATE;
            }
            else{
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("invalid soc property: 'ext_ram_present'")));
            }
        }
    }
    else {
         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("invalid soc property: 'bcm886xx_ocb_enable'")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 this function validate ocb_enable per chip id */
STATIC int
soc_arad_validate_ocb_enable(int unit,  ARAD_OCB_ENABLE_MODE ocb_enable)
{
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    switch(driver_dev_id) {

        case BCM88654_DEVICE_ID:
        case BCM88664_DEVICE_ID:        
            if(ocb_enable != OCB_DISABLED) {
                SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);            
            }

            break;

        default:
            break;                
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_arad_cmic_info_config(int unit)
{

    int cmc_i;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PCI_CMC(unit)   = soc_property_uc_get(unit, 0, spn_CMC, SOC_DPP_ARAD_PCI_CMC);
    SOC_ARM_CMC(unit, 0) = soc_property_uc_get(unit, 1, spn_CMC, SOC_DPP_ARAD_ARM1_CMC);
    SOC_ARM_CMC(unit, 1) = soc_property_uc_get(unit, 2, spn_CMC, SOC_DPP_ARAD_ARM2_CMC);

    /* CMC COSQ configuration */
    SOC_CMCS_NUM(unit) = SOC_DPP_ARAD_NUM_CMCS;
    NUM_CPU_COSQ(unit) = SOC_DPP_ARAD_NUM_CPU_COSQ - 1;

    
    

    /* Get these values from SOC Properties */
    NUM_CPU_ARM_COSQ(unit, 0) = soc_property_uc_get(unit, 0, spn_NUM_QUEUES, NUM_CPU_COSQ(unit));
    NUM_CPU_ARM_COSQ(unit, 1) = soc_property_uc_get(unit, 1, spn_NUM_QUEUES, 0);
    NUM_CPU_ARM_COSQ(unit, 2) = soc_property_uc_get(unit, 2, spn_NUM_QUEUES, 0);

    /* clear ('0') the cosq bitmaps per cmc */
    for (cmc_i = 0; cmc_i < SOC_CMCS_NUM(unit); cmc_i++) {
        SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, cmc_i), 0, NUM_CPU_COSQ_MAX);
    }

    /* set ('1') the cosq bitmaps per cmc */
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 0), 0, NUM_CPU_ARM_COSQ(unit, 0));
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 1), NUM_CPU_ARM_COSQ(unit, 0), NUM_CPU_ARM_COSQ(unit, 1));
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 2), NUM_CPU_ARM_COSQ(unit, 0) + NUM_CPU_ARM_COSQ(unit, 1), NUM_CPU_ARM_COSQ(unit, 2));
    
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_ports_info_config(int unit) 
{
    int rv = SOC_E_NONE;
    int val;
    uint32 channel, is_hg, flags, is_pon, init_vid_enabled, erp_base_q_pair, nof_channels, offset;
    SOC_TMC_INTERFACE_ID nif_id;    
    soc_port_t port_i;
    soc_info_t          *si; 
    int i, intern_id;
    soc_pbmp_t phy_ports, pon_port_bm;
    soc_port_if_t interface;
    uint32 erp_tm_port_id;

    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    /* Marking dynamic tables according Arad table confluence in order to not prevent write to them by SW */
    arad_tbl_default_dynamic_set();

    /* Marking non-dynamic tables as cachable */
    arad_tbl_mark_cachable(unit);

    /* Init NIF ports */
    SOC_PBMP_CLEAR(phy_ports);
    for (port_i = 0; port_i < SOC_DPP_NOF_DIRECT_PORTS; ++port_i) {
        SOC_PBMP_PORT_ADD(phy_ports, port_i+1);
    }

    /*init SW DB*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_init(unit));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_wb_init(unit));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_snapshot_init(unit));


    /* init reserved bitmap*/
    SOC_PBMP_CLEAR(SOC_DPP_CONFIG(unit)->arad->reserved_ports);

    /* init reserved ISQ and FMQ base q pairs */
    soc_arad_ps_reserved_mapping_init(unit);


    if (SOC_WARM_BOOT(unit)) {

       /*  
        * Take a snapshot of port sw data base and then restore it back in order to update bitmaps
        */       
       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_snapshot_take(unit));

       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_init(unit));

       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_snapshot_restore(unit));

    }

    /*init port software data base*/
    SOCDNX_IF_ERR_EXIT(soc_pm_init(unit, phy_ports, SOC_DPP_FIRST_DIRECT_PORT(unit)));

    /* Map local port to interface/tm_port/pp_port */
    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) 
    {
        nif_id = SOC_TMC_NIF_ID_NONE;
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_parse_ucode_port(unit, port_i, &nif_id, &channel, &is_hg));
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_parse_pon_port(unit, port_i, &is_pon));
        SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_parse_init_vid_enabled_port_get(unit, port_i, &init_vid_enabled));


        /* pre defined ports, needed in case dynamic nif feature is active in order to preserve tm ports */
        if (nif_id == SOC_TMC_IF_ID_RESERVED) {
            SOC_PBMP_PORT_ADD(SOC_DPP_CONFIG(unit)->arad->reserved_ports, port_i);
            continue;
        }
        
        /* indicate pon port */
        if (is_pon) {
          SOC_DPP_CONFIG(unit)->pp.pon_application_enable = 1;
          PORT_SW_DB_PORT_ADD(pon, port_i);
        }

        /* In this port, support use Initial-VID with no differencebetween 
         * untagged packets and tagged packets. and need install initial-vid
         * program when init isem DB.
         * The decision to install programs on init isem DB is decided by 
         * port_use_initial_vlan_only_enabled parameter.
         */
        if (!init_vid_enabled) {
           SOC_DPP_CONFIG(unit)->pp.port_use_initial_vlan_only_enabled = 1;
        }

        if (!SOC_WARM_BOOT(unit)) {
            if (nif_id != SOC_TMC_NIF_ID_NONE && nif_id != SOC_TMC_IF_ID_RESERVED) {
                interface = SOC_PORT_IF_NULL;
                flags = 0;
         
                intern_id = arad_nif2intern_id(unit, nif_id);
                SOC_PBMP_CLEAR(phy_ports);

                if (ARAD_NIF_IS_TYPE_ID(XAUI, nif_id)) 
                {
                    interface = SOC_PORT_IF_DNX_XAUI;
                    for(i=0;i<4;i++) {
                        SOC_PBMP_PORT_ADD(phy_ports, intern_id+i+1);
                    }
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(RXAUI, nif_id))
                {
                    interface = SOC_PORT_IF_RXAUI;
                    for(i=0;i<2;i++) {
                        SOC_PBMP_PORT_ADD(phy_ports, intern_id+i+1);
                    }
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(SGMII, nif_id))
                {
                    interface = SOC_PORT_IF_SGMII;
                    SOC_PBMP_PORT_ADD(phy_ports, intern_id+1);
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_id))
                {
                    uint32 ilkn_id;
                    ilkn_id = ARAD_NIF_ID_OFFSET(ILKN, nif_id);
                    if(soc_arad_is_device_ilkn_disabled(unit, ilkn_id)){ 
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc property try to config ILKN, device is ILKN disabled.")));            
                    }
                    interface = SOC_PORT_IF_ILKN;
                    SOCDNX_IF_ERR_EXIT(soc_arad_ilkn_phy_ports_btmp_set(unit, ilkn_id, &phy_ports));
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(10GBASE_R, nif_id))
                {
                    interface = SOC_PORT_IF_XFI;
                    SOC_PBMP_PORT_ADD(phy_ports, intern_id+1);
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(CGE, nif_id))
                {
                    interface = SOC_PORT_IF_CAUI;
                    val = soc_property_port_get(unit, intern_id/16, spn_CAUI_NUM_LANES, 0);
                    for(i=0;i<val;i++) {
                        SOC_PBMP_PORT_ADD(phy_ports, intern_id+i+1);
                    }
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if (ARAD_NIF_IS_TYPE_ID(XLGE, nif_id))
                {
                    interface = SOC_PORT_IF_XLAUI;
                    for(i=0;i<4;i++) {
                        SOC_PBMP_PORT_ADD(phy_ports, intern_id+i+1);
                    }
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                }
                else if(ARAD_IF_ID_CPU == nif_id) {
                    interface = SOC_PORT_IF_CPU;
                    /*port 0 is reserved for CPU*/
                    SOC_PBMP_PORT_ADD(phy_ports, 0);
                }
                else if(ARAD_IF_ID_RCY == nif_id) {
                    interface = SOC_PORT_IF_RCY;
                    SOC_PBMP_CLEAR(phy_ports);
                } 
                else if(ARAD_IF_ID_TM_INTERNAL_PKT == nif_id) {
                    interface = SOC_PORT_IF_TM_INTERNAL_PKT;
                    flags |= SOC_PORT_FLAGS_NETWORK_INTERFACE;
                    /* interface 0 is reserved for TM_INTERNAL_PKT in Ardon */
                    SOC_PBMP_PORT_ADD(phy_ports, 1);
                }

                 /*set is_stat_if*/
                if(SOC_PBMP_MEMBER(phy_ports, 29) || 
                   SOC_PBMP_MEMBER(phy_ports, 30) ||
                   SOC_PBMP_MEMBER(phy_ports, 31) ||
                   SOC_PBMP_MEMBER(phy_ports, 32)) 
                {
                    flags |= SOC_PORT_FLAGS_STAT_INTERFACE;
                }

                if (is_pon) {
                  flags |= SOC_PORT_FLAGS_PON_INTERFACE;
                }

                /* In this port, support use Initial-VID with no differencebetween 
                 * untagged packets and tagged packets. and need install initial-vid
                 * program when init isem DB.
                 * The decision to install programs on init isem DB is decided by 
                 * port_use_initial_vlan_only_enabled parameter.
                 */
                if (!init_vid_enabled) {
                    flags |= SOC_PORT_FLAGS_INIT_VID_ONLY;
                } else {
                      /* indicates if we really need to allocate some default programs */
                      /* In case Initial-VID is enabled for all ports then we can skip some programs */
                      SOC_DPP_CONFIG(unit)->pp.port_use_outer_vlan_and_initial_vlan_enabled = 1;
                }
               
                SOCDNX_IF_ERR_EXIT(soc_arad_validate_nif_in_use(unit, phy_ports)); 
                             
                rv = soc_port_sw_db_port_validate_and_add(unit, port_i, 0, channel, flags, interface, phy_ports);
                SOCDNX_IF_ERR_EXIT(rv);

                /* update protocol offset */
                if (interface == SOC_PORT_IF_ILKN || interface == SOC_PORT_IF_CAUI) {
                    offset = 0;
                    if (interface == SOC_PORT_IF_ILKN) {
                        offset = ARAD_NIF_ID_OFFSET(ILKN, nif_id);
                    } else {
                        if (interface == SOC_PORT_IF_CAUI) {
                            offset = ARAD_NIF_ID_OFFSET(CGE, nif_id);
                        }
                    }

                   SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_set(unit, port_i, offset));
                }


                rv = soc_port_sw_db_is_hg_set(unit, port_i, is_hg);
                SOCDNX_IF_ERR_EXIT(rv);

                val = soc_property_port_get(unit, port_i, spn_LOCAL_TO_TM_PORT, port_i);
                if (soc_port_sw_db_local_to_tm_port_set(unit, port_i, val)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("_soc_dpp_local_to_tm_port_set error in soc_arad_info_config")));
                }
                val = soc_property_port_get(unit, port_i, spn_LOCAL_TO_PP_PORT, port_i);

                if (soc_port_sw_db_local_to_pp_port_set(unit, port_i, val)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("_soc_dpp_local_to_pp_port_set error in soc_arad_info_config")));
                }
            }
        }
    }

    SOC_PBMP_CLEAR(pon_port_bm);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_PON_INTERFACE, &pon_port_bm));

    SOC_PBMP_ITER(pon_port_bm, port_i) {
        /* CPU port is already channelized. Skip it. */
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));
        if (SOC_PORT_IF_CPU == interface) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port_i, &nof_channels));

        if (nof_channels > 1) {
          SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_enable = 1;
        }

        /* Get max channel num of PON port */
        if (nof_channels > SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num) {
            SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num = nof_channels;
        }
    }

    if (!SOC_WARM_BOOT(unit)) {

        /* * Add OLP/ERP ports */
        SOC_PBMP_CLEAR(phy_ports);
        si->olp_port[0] = -1;
        si->olp_port[1] = -1;
        val = soc_property_get(unit, spn_NUM_OLP_TM_PORTS, 0);
        if (val > 0) {
            si->olp_port[0] = ARAD_OLP_PORT_ID;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, si->olp_port[0], 0, 0, 0, SOC_PORT_IF_OLP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->olp_port[0], ARAD_OLP_PORT_ID));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->olp_port[0], ARAD_OLP_PORT_ID));  
        }

        si->oamp_port[0] = -1;
        si->oamp_port[1] = -1;
        val = soc_property_get(unit, spn_NUM_OAMP_PORTS, 0) ;
        if (val > 0) {
            si->oamp_port[0] = ARAD_OAMP_PORT_ID;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, si->oamp_port[0], 0, 0, 0, SOC_PORT_IF_OAMP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->oamp_port[0], ARAD_OAMP_PORT_ID));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->oamp_port[0], ARAD_OAMP_PORT_ID));
        }

        si->erp_port[0] = -1;
        si->erp_port[1] = -1;
        val = soc_property_get(unit, spn_NUM_ERP_TM_PORTS, 0);
        if (val > 0) {
            si->erp_port[0] = SOC_DPP_PORT_INTERNAL_ERP(0);
            erp_tm_port_id = soc_property_suffix_num_get(unit, si->erp_port[0], spn_LOCAL_TO_TM_PORT, "erp", ARAD_ERP_PORT_ID);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_validate_and_add(unit, si->erp_port[0], 0, 0, 0, SOC_PORT_IF_ERP, phy_ports));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_set(unit, si->erp_port[0], erp_tm_port_id));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_set(unit, si->erp_port[0], erp_tm_port_id)); /*meaningless, added for compliance*/ 
            erp_base_q_pair = (ARAD_EGR_NOF_PS-1)*ARAD_EGR_NOF_Q_PAIRS_IN_PS;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_set(unit, si->erp_port[0], erp_base_q_pair));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_set(unit, si->erp_port[0], 1));
            
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_info_elk_config(int unit) 
{
    
    SOCDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
{
    soc_dpp_config_arad_t *dpp_arad;

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    /* ELK  */ 
    SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_nif_elk_tcam_dev_type_get(unit, &dpp_arad->init.elk.tcam_dev_type));

    if (dpp_arad->init.elk.tcam_dev_type != ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE) {

        if(soc_arad_is_device_elk_disabled(unit)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc property try to config ELK, device is ELK disabled.")));
        }
#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit)) {
            dpp_arad->init.elk.ext_interface_mode = soc_property_get(unit, spn_EXT_INTERFACE_MODE, FALSE);
        }
#endif /* BCM_88660_A0 */
        dpp_arad->init.elk.enable = 0x1;
    }
    SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_nif_elk_fwd_table_size_get(unit, &dpp_arad->init.elk));
}
exit:
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

    SOCDNX_FUNC_RETURN;
}

/*
 * Configures soc data structures for Arad or Jericho
 */
int
soc_arad_info_config(int unit) 
{
    int rv = SOC_E_NONE;
    soc_dpp_config_arad_t *dpp_arad;
    int prop_invalid = 0xffffffff, val;
    uint32 is_hg_header, flags, is_sop_only;
    int fabric_link;
    uint8 include_port = 0;
    int ind;
    int ip_lpm_total = 0;
    soc_port_t port_i, phy_port;
    int blk;
    int disabled_port = FALSE;
    int blktype;
    int pp_port_configured[SOC_MAX_NUM_PORTS];
    uint32 pp_port, tm_port;
    uint32 out_port_priority;
    soc_info_t          *si; 
    int base_q_pair;   
    int is_static_mapping, core;
    int auxiliary_table_mode;
    int quad_active, quad_index;
    int i, quad, nof_quads;
    soc_pbmp_t pbmp_disabled;
    soc_port_if_t interface;
    uint32 is_valid, multicast_offset;
    uint32 padding_size;
    uint8 xgs_port_exists = 0;
    uint8 udh_port_exists =0;
    soc_dpp_config_pp_t *dpp_pp;
    uint8 bank_phase;
    ARAD_PORT_HEADER_TYPE header_type_in, header_type_out; 
    uint32 first_header_size;
    uint32 shaper_mode = 0;
    uint32 peer_tm_domain;
    int dflt_core_clck_frq_mhz, dflt_glob_clck_frq_mhz;
    char *propkey, *propval;
    int tdm_priority;
	uint32 dram_bitmap = 0x0;
    uint32 is_vendor_pp_port;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(pp_port_configured, 0x0, sizeof(pp_port_configured));

    dpp_arad = SOC_DPP_CONFIG(unit)->arad; 
    si  = &SOC_INFO(unit);
    
    SOCDNX_IF_ERR_EXIT(soc_arad_cmic_info_config(unit));

    SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system = soc_property_get(unit, spn_SYSTEM_IS_PETRA_B_IN_SYSTEM, 0x0);
    SOC_DPP_CONFIG(unit)->emulation_system = soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0);
    si->num_ucs = 2;

    rv = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_MAP);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "Error init SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_MAP buffer in soc_dpp_info_config_arad\n")));
    }
    SOCDNX_IF_ERR_RETURN(rv);

    _soc_dpp_port_map_init(unit);

    if (SOC_WARM_BOOT(unit)) {
        _soc_dpp_wb_pp_port_restore(unit);
        _dflt_tm_pp_port_map[unit] = FALSE;
    }

    /* 
     * Initialize system
     */
    dpp_arad->init.is_petrab_in_system = SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system;

    /* 
     * Initialize TDM
     */
    SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits = (SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) ? 32:48;

    SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size = (SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) ? 1:0;

    if (SOC_IS_JERICHO(unit)) {
        SOC_DPP_CONFIG(unit)->tdm.is_tdm_over_primary_pipe = 0x0;
    } else {
        SOC_DPP_CONFIG(unit)->tdm.is_tdm_over_primary_pipe = soc_property_get(unit, spn_FABRIC_TDM_OVER_PRIMARY_PIPE, 0x0); 
    }
    /* 
     * Core frequency
     */
    dflt_core_clck_frq_mhz = SOC_DPP_DEFS_GET(unit, core_clock_freq_mhz);
    rv = soc_arad_core_frequency_config_get(unit, dflt_core_clck_frq_mhz * 1000, &(dpp_arad->init.core_freq.frequency));
    SOCDNX_IF_ERR_EXIT(rv);
    SOC_INFO(unit).frequency = dpp_arad->init.core_freq.frequency/1000;

    SOCDNX_IF_ERR_EXIT(soc_arad_validate_device_core_frequency(unit, dpp_arad->init.core_freq.frequency)); 

    dflt_glob_clck_frq_mhz = SOC_DPP_DEFS_GET(unit, glob_clock_freq_mhz);
    if (soc_property_get_str(unit, spn_SYSTEM_REF_CORE_CLOCK) != NULL) {
        /*configure core clock in MHz*/
        dpp_arad->init.core_freq.system_ref_clock = soc_property_get(unit, spn_SYSTEM_REF_CORE_CLOCK, dflt_glob_clck_frq_mhz) * 1000; 
    } else {
        /*configure core clock in KHz*/
        dpp_arad->init.core_freq.system_ref_clock = soc_property_suffix_num_get(unit, 0, spn_SYSTEM_REF_CORE_CLOCK, "khz", dflt_glob_clck_frq_mhz * 1000);
    }
    dpp_arad->init.core_freq.enable = TRUE;
     
    SOCDNX_IF_ERR_EXIT(soc_arad_prop_fap_device_mode_get(unit, &dpp_arad->init.pp_enable, &dpp_arad->init.tdm_mode));

    /*
     * Cmic interrupts configuration.
     */
    
    rv = soc_arad_schan_timeout_config_get(unit, &(SOC_CONTROL(unit)->schanTimeout));
    SOCDNX_IF_ERR_EXIT(rv);
    
    SOC_CONTROL(unit)->schanIntrEnb = soc_property_get(unit, spn_SCHAN_INTR_ENABLE, 0);
    SOC_CONTROL(unit)->miimTimeout =  soc_property_get(unit, spn_MIIM_TIMEOUT_USEC, 2000); 
    SOC_CONTROL(unit)->miimIntrEnb = soc_property_get(unit, spn_MIIM_INTR_ENABLE, 0);

    /* 
     * Credit configuration
     */
    dpp_arad->init.credit.credit_worth_enable = TRUE;
    dpp_arad->init.credit.credit_worth = soc_property_get(unit, spn_CREDIT_SIZE, 1024);
    rv = soc_arad_str_prop_credit_worth_resolution_get(unit, &dpp_arad->init.credit.credit_worth_resolution);
    SOCDNX_IF_ERR_EXIT(rv);
    
    /* 
     *  Common TM configuration
     */
    SOCDNX_IF_ERR_EXIT(soc_dpp_info_config_common_tm(unit));

    /* Fabric Pipes configuration */
    if (SOC_IS_JERICHO(unit)) {
        dpp_arad->init.fabric.dual_pipe_tdm_packet = FALSE;
        dpp_arad->init.fabric.is_dual_mode_in_system = FALSE;
        dpp_arad->init.fabric.system_contains_multiple_pipe_device = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_SYSTEM_CONTAINS_MULTIPLE_PIPE_DEVICE, FALSE)); 
        dpp_arad->init.fabric.fabric_pipe_map_config.nof_pipes = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_FABRIC_NUM_PIPES, 1));
        SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_pipes_config(unit));
    } else {
        dpp_arad->init.fabric.dual_pipe_tdm_packet = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_IS_DUAL_MODE, FALSE)); 
        dpp_arad->init.fabric.fabric_pipe_map_config.nof_pipes = dpp_arad->init.fabric.dual_pipe_tdm_packet ? 2 : 1;
        dpp_arad->init.fabric.is_dual_mode_in_system = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_SYSTEM_IS_DUAL_MODE_IN_SYSTEM,  dpp_arad->init.fabric.dual_pipe_tdm_packet));
	    dpp_arad->init.fabric.system_contains_multiple_pipe_device = dpp_arad->init.fabric.is_dual_mode_in_system;
        if (dpp_arad->init.fabric.dual_pipe_tdm_packet && !(dpp_arad->init.fabric.is_dual_mode_in_system)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Unsupported properties: is_dual_mode=1 & system_is_dual_mode_in_system=0")));
        }
    }

    /* Init multicast configuration */
    SOCDNX_IF_ERR_EXIT(soc_dpp_arad_str_prop_mc_tbl_mode(unit));

    /*
     * init Arad multicast auto allocation ranges
     */
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start = soc_property_get(unit,
      spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MIN, 0);
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end = soc_property_get(unit,
      spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MAX, ARAD_MULTICAST_TABLE_MODE - 1);
    SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start = soc_property_get(unit,
      spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MIN, SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high + 1);
    SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end = soc_property_get(unit,
      spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MAX, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1);

    if (SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start > SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end ||
        SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start > SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end) {
       LOG_ERROR(BSL_LS_SOC_INIT,
                 (BSL_META_U(unit,
                             "multicast group id ranges can not have a min value bigger than a max value:\n\r"
                             "ingress range: min=%u to max=%d\n\r"
                             "egress range: min=%u to max=%d\n\r"),
                  SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start, SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end,
                  SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start, SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end));
        SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    } else if (SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end >= ARAD_MULTICAST_TABLE_MODE) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "ingress multicast group id range ends in %u which is not a valid group id\n\r"),
                              SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end));
        SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    } else if (SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "egress multicast group id range ends in %u which is not a valid group id\n\r"),
                              SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end));
        SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    }

    /* 
     * ILKN TDM SP mode
     */
    val = soc_property_get(unit, spn_ILKN_TDM_DEDICATED_QUEUING, 0);
    dpp_arad->init.ilkn_tdm_dedicated_queuing = (val == 0) ? ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_OFF : ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON;

    /*
     * Init NIF ports
     */
    dpp_arad->init.nif_recovery_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "nif_recovery_enable", 1);
    dpp_arad->init.nif_recovery_iter = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "nif_recovery_iter", 3);

    /* RCPU */
    dpp_arad->init.rcpu.slave_port_pbmp = soc_property_get_pbmp(unit, spn_RCPU_RX_PBMP, 0);

    SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge = 0;

    SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists = 0;

    SOCDNX_IF_ERR_EXIT(arad_ps_db_init(unit));

    /* First loop - operations required before main loop */
     for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
      
         SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
         if (!is_valid) {
             continue;
         }

         /*  Retrieve outgoing port priorities information. */
         if (SOC_WARM_BOOT(unit)) {
             rv = soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &out_port_priority);
             SOCDNX_IF_ERR_EXIT(rv);
         } else {
             out_port_priority = soc_property_port_get(unit, port_i, spn_PORT_PRIORITIES, prop_invalid);
             if (out_port_priority != prop_invalid) {
                 rv = soc_port_sw_db_local_to_out_port_priority_set(unit, port_i, out_port_priority);
                 SOCDNX_IF_ERR_EXIT(rv);
             }
         }
         if (SOC_WARM_BOOT(unit)) 
         {
             rv = soc_port_sw_db_base_q_pair_get(unit, port_i, (uint32 *) &base_q_pair);
             SOCDNX_IF_ERR_EXIT(rv);

             rv = arad_ps_db_alloc_binding_ps_with_id(unit, port_i, out_port_priority, base_q_pair);
             SOCDNX_IF_ERR_EXIT(rv);
         }
         else
         {
             /* Allocate static base q pairs */
             rv = soc_arad_ps_static_mapping(unit, port_i, out_port_priority, &is_static_mapping, &base_q_pair);
             SOCDNX_IF_ERR_EXIT(rv);

             if(is_static_mapping)
             {
                 rv = arad_ps_db_alloc_binding_ps_with_id(unit, port_i, out_port_priority, base_q_pair);
                 SOCDNX_IF_ERR_EXIT(rv);

                 rv = (soc_port_sw_db_base_q_pair_set(unit, port_i, base_q_pair));
                 SOCDNX_IF_ERR_EXIT(rv);
             }
         }

    }

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
        if (!is_valid) {
            continue;
        }

        if (!SOC_WARM_BOOT(unit)) {         

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port_i, &pp_port, &core));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));     

            if(SOC_PORT_IF_CPU == interface) {     
               SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit,port_i,SOC_TMC_PORT_HEADER_TYPE_CPU));
            }

            padding_size = soc_property_port_get(unit, port_i, spn_PACKET_PADDING_SIZE, 0);
            if(padding_size > SOC_DPP_MAX_PADDING_SIZE) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Padding size is out-of-range port %d, value %d, max size %d"), port_i, padding_size, SOC_DPP_MAX_PADDING_SIZE));
            }
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_set(unit, port_i, padding_size));

            /* multicast_id_offset */ 
            multicast_offset = soc_property_port_get(unit, port_i , spn_MULTICAST_ID_OFFSET, 0);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_mc_offset_set(unit,port_i,multicast_offset));

            if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                /*  Interface rate set per Logical port. For Jericho and above already done before this code. */
                val = soc_property_port_get(unit, port_i, spn_PORT_INIT_SPEED, -1);
                if(-1 == val) {
                    SOCDNX_IF_ERR_EXIT(soc_pm_default_speed_get(unit, port_i, &val));
                }

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port_i, val));
            }

            if(SOC_PORT_IF_ERP != interface) {
                  
                if (soc_dpp_str_prop_parse_tm_port_header_type(unit, port_i, &(header_type_in), &(header_type_out),&is_hg_header)) {    
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_dpp_str_prop_parse_tm_port_header_type error")));       
                }
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_in_set(unit,port_i,header_type_in));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit,port_i,header_type_out));
                if(is_hg_header) { /*if the header is HG force the port to HG mode*/
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_set(unit, port_i, is_hg_header));
                }
                is_sop_only = soc_property_port_get(unit, port_i, spn_PREAMBLE_SOP_ONLY, FALSE);
                if(IS_HG_PORT(unit,port_i)){
#ifdef BCM_88660_A0
                    if (SOC_IS_ARADPLUS(unit)) {
                        if(is_sop_only){
                            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("can't set port %d to be hg and preamble sop only"), port_i));
                        }
                    }
#endif /* BCM_88660_A0 */
                }
                else{
                    if(SOC_DPP_ARAD_IS_HG_SPEED_ONLY(val)){
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("port_init_speed hg speed %d for not hg port %d"), val, port_i));
                    }  
#ifdef BCM_88660_A0
                    if (SOC_IS_ARADPLUS(unit)) {
                        if(is_sop_only){
                            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_set(unit, port_i, SOC_ENCAP_SOP_ONLY));
                        }
                    }
#endif /* BCM_88660_A0 */
                }
                if (header_type_out == SOC_TMC_PORT_HEADER_TYPE_TM) {
                    SOC_TMC_PORTS_OTMH_EXTENSIONS_EN otmh_ext_en;
                    if (soc_dpp_str_prop_parse_tm_port_otmh_extensions_en(unit,port_i, &otmh_ext_en)) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_dpp_str_prop_parse_tm_port_otmh_extensions_en error")));
                    }

                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_src_ext_en_set(unit, port_i, otmh_ext_en.src_ext_en));
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_dst_ext_en_set(unit, port_i, otmh_ext_en.dest_ext_en));
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_olif_ext_en_set(unit, port_i, otmh_ext_en.outlif_ext_en));
                    if(otmh_ext_en.outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ENLARGE) {
                      SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge = 1;
                    }
                }
                if(SOC_PORT_IF_OLP != interface && SOC_PORT_IF_OAMP != interface) {
                    /* 
                     * Introduce flag indication in Port SW DB to indicate port is XGS MAC extender
                     */
                    if (header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT) {
                       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port_i, SOC_PORT_FLAGS_XGS_MAC_EXT));
                    }
                }

                if (!xgs_port_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)) {
                    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
                    dpp_pp->xgs_port_exists = 1;
                    xgs_port_exists = 1;
                }

                /* Out port - with UDH packet*/
                if (!udh_port_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_UDH_ETH)) {
                    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
                    dpp_pp->udh_port_exists = 1;
                    udh_port_exists = 1;
                }

          /* Retrieve the configured routing recycle local port number */
          if (!SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists)
          {
              dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
              dpp_pp->ipv6_fwd_pass2_rcy_local_port = soc_property_suffix_num_get(unit,  -1, spn_CUSTOM_FEATURE, CF_IPV6_UC_WITH_RPF_2PASS_ENABLED, -1);

              if (dpp_pp->ipv6_fwd_pass2_rcy_local_port >= 0) 
              {
                  dpp_pp->ipv6_with_rpf_2pass_exists = 1;
              }
          }

          is_vendor_pp_port = soc_property_port_suffix_num_get(unit, port_i, -1, spn_CUSTOM_FEATURE,"vendor_custom_pp_port", FALSE);
          if (is_vendor_pp_port) {
              SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port_i, SOC_PORT_FLAGS_VENDOR_PP_PORT));
          }

                first_header_size = soc_property_port_get(unit, port_i, spn_FIRST_HEADER_SIZE, 0);
                
                if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED) || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP)) 
                {
                    first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH;
                }
                else if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2) || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP)) 
                {
                    first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH_2;
                }
                else if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)) {
                   first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_FRC_PPD;
                }
          else if (is_vendor_pp_port) {
              first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_VENDOR_PP;
          }

                /* only needed in case interface is hg but header processing is not xgs */ 
                if (is_hg_header && (!((header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_HQoS) 
                                       || header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_DiffServ
                                       || header_type_in == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT))) 
                {
                    first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_HIGIG_FB;
                }

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_frst_hdr_sz_set(unit, port_i, first_header_size));

                if ((pp_port != -1) && !pp_port_configured[pp_port]) {
                    uint8 is_stag_enabled,
                          is_snoop_enabled;
                    uint32 mirror_profile;
                    SOC_TMC_PORTS_FC_TYPE fc_type;

                    if (header_type_out == SOC_TMC_PORT_HEADER_TYPE_CPU) {
                        soc_port_sw_db_is_tm_pph_present_en_set(unit,port_i,1);
                    }

                    /* If header type is TM, enable pph_present. We want to support this for CPU port, and it is a driver
                      limitation that all tm ports will behave the same */
                    if (header_type_in == SOC_TMC_PORT_HEADER_TYPE_TM) {
                       soc_port_sw_db_is_tm_pph_present_en_set(unit,port_i,1);
                    }

                    is_stag_enabled = soc_property_port_get(unit, port_i, spn_STAG_ENABLE, 0);
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_stag_enabled_set(unit,port_i,is_stag_enabled));
              
                    rv = soc_dpp_str_prop_parse_flow_control_type(unit, port_i, &(fc_type));
                    SOCDNX_IF_ERR_EXIT(rv);
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_fc_type_set(unit,port_i,fc_type));
                          
                    is_snoop_enabled = soc_property_port_get(unit, port_i, spn_SNOOP_ENABLE, 0);
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_snoop_enabled_set(unit,port_i,is_snoop_enabled));
                    mirror_profile = soc_property_port_get(unit, port_i, spn_MIRROR_PROFILE, 0);
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_mirror_profile_set(unit,port_i,mirror_profile));
             
                    pp_port_configured[pp_port] = TRUE;
                }
            } /* end of if(SOC_PORT_IF_ERP != interface) */

            /* Egress port info */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &out_port_priority));

            switch (out_port_priority)
            {
                case 1:
                    rv = soc_port_sw_db_local_to_out_port_priority_set(unit,port_i,ARAD_EGR_PORT_ONE_PRIORITY);
                    if (SOC_FAILURE(rv)) {
                         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG_STR( "invalid port %d\n"), port_i));
                    }
                    break; 
                case 2:
                    rv = soc_port_sw_db_local_to_out_port_priority_set(unit,port_i,ARAD_EGR_PORT_TWO_PRIORITIES);
                    if (SOC_FAILURE(rv)) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL,(_BSL_SOCDNX_MSG_STR( "invalid port %d\n"), port_i));
                    }
                    break;
                case 8:
                    rv = soc_port_sw_db_local_to_out_port_priority_set(unit,port_i,ARAD_EGR_PORT_EIGHT_PRIORITIES);
                    if (SOC_FAILURE(rv)) {
                         SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG_STR( "invalid port %d\n"), port_i));
                    }
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("out_port_priority error priority is out of valid values")));
            }

            rv = soc_arad_ps_static_mapping(unit, port_i, out_port_priority, &is_static_mapping, &base_q_pair);
            SOCDNX_IF_ERR_EXIT(rv);

            if(!is_static_mapping) {
                /* Search for suitable PS and corresponded base_q_pair. */
                SOCDNX_IF_ERR_EXIT(arad_ps_db_find_free_binding_ps(unit, port_i, out_port_priority, &base_q_pair));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_set(unit,port_i,base_q_pair));
            }
              
            /* egress port shaper rate */
            rv = soc_arad_str_prop_parse_otm_port_rate_type(unit, port_i, &shaper_mode);
            SOCDNX_IF_ERR_EXIT(rv);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_shaper_mode_set(unit,port_i,shaper_mode));
        }
        else { /*if(SOC_WARM_BOOT(unit)) */
            if (interface != SOC_PORT_IF_ERP) {

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_get(unit,port_i,&header_type_out));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface));

                if (!xgs_port_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)) {
                    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
                    dpp_pp->xgs_port_exists = 1;
                    xgs_port_exists = 1;
                }

                /* Out port - with UDH packet*/
                if (!udh_port_exists && (header_type_out == SOC_TMC_PORT_HEADER_TYPE_UDH_ETH)) {
                    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
                    dpp_pp->udh_port_exists = 1;
                    udh_port_exists = 1;
                }
                if (header_type_out == SOC_TMC_PORT_HEADER_TYPE_TM) {
                    SOC_TMC_PORTS_FTMH_EXT_OUTLIF outlif_ext_en;
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_olif_ext_en_get(unit, port_i, &outlif_ext_en));
                    if(outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ENLARGE) {
                      SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge = 1;
                    }
                }
            }
        }
    }

    /* ILKN counters mode */
    rv = soc_arad_str_prop_ilkn_counters_mode_get(unit, &(dpp_arad->init.ports.ilkn_counters_mode));
    SOCDNX_IF_ERR_EXIT(rv);

    /* LAG mode */
    rv = soc_arad_str_prop_lag_mode_get(unit, &dpp_arad->init.ports.lag_mode);
    SOCDNX_IF_ERR_EXIT(rv);
    
    /* SyncE */
    rv = soc_arad_str_prop_synce_mode_get(unit, &dpp_arad->init.synce.mode);
    SOCDNX_IF_ERR_EXIT(rv);

    for (ind = 0; ind < ARAD_NIF_NOF_SYNCE_CLK_IDS ; ind++) {
        dpp_arad->init.synce.conf[ind].port_id = soc_property_suffix_num_get(unit, ind, spn_SYNC_ETH_CLK_TO_NIF_ID, "clk_", 0xffffffff);
        if (dpp_arad->init.synce.conf[ind].port_id != 0xffffffff) {
            dpp_arad->init.synce.conf[ind].enable = 0x1;
            dpp_arad->init.synce.enable = 0x1;
            
            rv = soc_arad_str_prop_synce_clk_div_get(unit, ind, &(dpp_arad->init.synce.conf[ind].clk_divider));
            if (rv != SOC_E_NONE) {
              LOG_INFO(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_arad_str_prop_synce_clk_div_get error\n")));
            }
            dpp_arad->init.synce.conf[ind].squelch_enable = soc_property_suffix_num_get(unit, ind, spn_SYNC_ETH_CLK_SQUELCH_ENABLE, "clk_", 0x0);
        }
    }

    /*
     * Fabric
     */
    dpp_arad->init.fabric.enable = TRUE;

    rv = soc_dpp_str_prop_fabric_connect_mode_get(unit, &dpp_arad->init.fabric.connect_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_dpp_str_prop_fabric_ftmh_outlif_extension_get(unit, &dpp_arad->init.fabric.ftmh_extension);
    SOCDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit)) {
        if (dpp_arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH) {
            dpp_arad->init.fabric.fabric_mesh_multicast_enable = soc_property_get(unit, spn_FABRIC_MESH_MULTICAST_ENABLE, 1); 
        } else {
            dpp_arad->init.fabric.fabric_mesh_multicast_enable = 0;
        }
    }

    dpp_arad->init.fabric.scheduler_adapt_to_links = SOC_SAND_NUM2BOOL(soc_property_get(unit,spn_SCHEDULER_FABRIC_LINKS_ADAPTATION_ENABLE, FALSE));
    if (SOC_IS_JERICHO(unit)) {
        dpp_arad->init.fabric.segmentation_enable = TRUE;
    } else {
        dpp_arad->init.fabric.segmentation_enable = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_FABRIC_SEGMENTATION_ENABLE, TRUE));
    }

    propval = soc_property_get_str(unit, spn_FABRIC_LINKS_TO_CORE_MAPPING_MODE);
    if ((propval == NULL) || (sal_strcmp(propval, "SHARED"))){
        dpp_arad->init.fabric.fabric_links_to_core_mapping_mode = SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_SHARED;
    } else if (sal_strcmp(propval, "DEDICATED") == 0){
        dpp_arad->init.fabric.fabric_links_to_core_mapping_mode = SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_DEDICATED;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported properties: fabric_links_to_core_mapping_mode should be SHARED or DEDICATED")));
    }
       
    /* Configure Cell Format */
    if (SOC_IS_JERICHO(unit)) {
        dpp_arad->init.fabric.is_fe600 = FALSE;
    } else {
        dpp_arad->init.fabric.is_fe600 = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_SYSTEM_IS_FE600_IN_SYSTEM,  FALSE)); 
    }
    dpp_arad->init.fabric.is_128_in_system = SOC_SAND_NUM2BOOL(soc_property_get(unit, spn_SYSTEM_IS_VCS_128_IN_SYSTEM,  FALSE));
    if (dpp_arad->init.fabric.is_fe600 && !(dpp_arad->init.fabric.is_128_in_system)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported properties: is_fe600=1 & dpp_arad->init.fabric.is_128_in_system=0")));
    }
    /* In case sending VCS256 to a Petra device, the TDM source FAP ID will be the FAP ID + tdm_source_fap_id_offset */
    flags = soc_property_get(unit, spn_TDM_SOURCE_FAP_ID_OFFSET, SOC_DPP_ARAD_DEFAULT_TDM_SOURCE_FAP_ID_OFFSET);
    dpp_arad->tdm_source_fap_id_offset = flags >= ARAD_NOF_FAPS_IN_SYSTEM ? SOC_DPP_ARAD_DEFAULT_TDM_SOURCE_FAP_ID_OFFSET : flags;

    /*Egress queueing- delete fifo threholds*/
    SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast_low_priority = soc_property_get(unit, spn_EGRESS_FABRIC_DROP_THRESHOLD_MULTICAST_LOW,  SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_almost_full_mc_low_prio));
    SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast = soc_property_get(unit, spn_EGRESS_FABRIC_DROP_THRESHOLD_MULTICAST,  SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_almost_full_mc));
    SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_all = soc_property_get(unit, spn_EGRESS_FABRIC_DROP_THRESHOLD_ALL,  SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_almost_full_all));
    /* validate */
    if(SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast_low_priority > SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_threshold_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("egress_fabric_drop_threshold_multicast_low is out of range")));
    }
    if(SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast > SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_threshold_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("egress_fabric_drop_threshold_multicast is out of range")));
    }
    if(SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_all > SOC_DPP_IMP_DEFS_GET(unit, egr_delete_fifo_threshold_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("egress_fabric_drop_threshold_all is out of range")));
    }
    if (SOC_IS_JERICHO(unit)) {
        propkey = spn_FABRIC_TDM_PRIORITY_MIN;
        propval = soc_property_get_str(unit, propkey);
        if (propval) {
            if (sal_strcmp(propval, "NONE") == 0) {
                dpp_arad->init.fabric.fabric_tdm_priority_min = SOC_DPP_FABRIC_TDM_PRIORITY_NONE;
            } else {
                tdm_priority = soc_property_get(0, spn_FABRIC_TDM_PRIORITY_MIN, 3);
                if ((tdm_priority < 0) || (tdm_priority >= ARAD_FBC_PRIORITY_NOF)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s\n\r"), propval, propkey)); 
                }
                dpp_arad->init.fabric.fabric_tdm_priority_min = tdm_priority;
            }
        } else {
            dpp_arad->init.fabric.fabric_tdm_priority_min = 3;
        }
    } else {
        dpp_arad->init.fabric.fabric_tdm_priority_min = 3;
    }

    /* Load-Balancing */
    rv = soc_arad_str_prop_system_ftmh_load_balancing_ext_mode_get(unit, &dpp_arad->init.fabric.ftmh_lb_ext_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Hashing format */  
    rv = soc_arad_str_prop_system_trunk_hash_format_get(unit, &dpp_arad->init.fabric.trunk_hash_format);
    SOCDNX_IF_ERR_EXIT(rv);

    /* PCP configuration */
    if (SOC_IS_JERICHO(unit) && !(SOC_DPP_IS_MESH(unit))) {
        dpp_arad->init.fabric.fabric_pcp_enable = soc_property_get(unit, spn_FABRIC_PCP_ENABLE, 1);
    } else {
        dpp_arad->init.fabric.fabric_pcp_enable = 0; 
    }

    /* Stacking */
    dpp_arad->init.ports.is_stacking_system = soc_property_get(unit, spn_STACKING_ENABLE, 0x0);
    if (dpp_arad->init.ports.is_stacking_system == 0x1) {
        dpp_arad->init.fabric.ftmh_stacking_ext_mode = 0x1;
    }
    dpp_arad->init.ports.tm_domain = soc_property_get(unit, spn_DEVICE_TM_DOMAIN, 0x0);
    
    for (port_i = 0; port_i < ARAD_NOF_FAP_PORTS; ++port_i) {
      peer_tm_domain = soc_property_port_get(unit, port_i, spn_PEER_TM_DOMAIN, prop_invalid);
      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_peer_tm_domain_set(unit,port_i,peer_tm_domain));
    }

    /* System RED */
    dpp_arad->init.ports.is_system_red = soc_property_get(unit, spn_SYSTEM_RED_ENABLE, 0x0);

    /* 
     * Set the device packet swap data according to the soc properties: 
     * swap mode & swap offset. 
     */        
    rv = soc_arad_str_prop_swap_info_get(unit, &dpp_arad->init.ports.swap_info);
    SOCDNX_IF_ERR_EXIT(rv);

    /*  VOQ mapping mode */
    rv = soc_arad_str_prop_voq_mapping_mode_get(unit, &dpp_arad->voq_mapping_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    /* action type from queue signature or from packet header */
    rv = soc_arad_str_prop_action_type_source_mode_get(unit, &dpp_arad->action_type_source_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    /* set default credit watchdog mode values marking that the configuration needs to be restored from hardware or initially set */
    dpp_arad->credit_watchdog_mode = CREDIT_WATCHDOG_UNINITIALIZED;
    dpp_arad->exact_credit_watchdog_scan_time_nano = 0;

    /* Shared multicast resource mode: Strict priority or Discrete partitioning */
    rv = soc_arad_str_prop_egress_shared_resources_mode_get(unit, &dpp_arad->init.eg_cgm_scheme);
    SOCDNX_IF_ERR_EXIT(rv);

    dpp_arad->port_egress_recycling_scheduler = soc_property_get(unit, spn_PORT_EGRESS_RECYCLING_SCHEDULER_CONFIGURATION,  0);

    /* 
     * Supported encoding system port modes for 886xx-XGS Compatible TM Diffserv, HQOS: 
     *    8_modid_7_port System port derived according 8 bits MODID, 7 lsbs Port. In this mode MSB of Port must be set to 0.
     *    7_modid_8_port System port derived according to 7 lsbs MODID, 8 bits Port. In this mode MSB of MODID must be set to 0.
     */        
    rv = soc_dpp_str_prop_xgs_compatible_system_port_mode_get(unit, &dpp_arad->xgs_compatability_tm_system_port_encoding);
    SOCDNX_IF_ERR_EXIT(rv);
    
    /*
    * support to lookup customized keys in LPM.
    */
    dpp_arad->enable_lpm_custom_lookup = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "lpm_custom_lookup", 0);

    if (dpp_arad->enable_lpm_custom_lookup) {
        /* if lpm_custom_lookup is enabled, then FCoE must be disabed as it reuses FCoE APIs to add/remove entries in LPM */
        if ((SOC_DPP_CONFIG(unit))->pp.fcoe_enable) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("FCoE and LPM custom lookup could not be enabled at the same time")));            
        }
    }
	
    /* 
     *read disabled quads in order to reduce power consumption. 
     *in order to disable quad number n use "serdes_qrtt_active_<n>=0"
     */
    SOC_PBMP_CLEAR(pbmp_disabled);
        nof_quads = SOC_DPP_DEFS_GET(unit, nof_fabric_macs);
    for (quad = 0; quad < nof_quads; quad++) {
        quad_active = soc_property_suffix_num_get(unit, quad, spn_SERDES_QRTT_ACTIVE, "", 1);
        if (!quad_active) {
            for (quad_index = 0; quad_index < 4; quad_index++) {
                SOC_PBMP_PORT_ADD(pbmp_disabled, FABRIC_LOGICAL_PORT_BASE(unit) + quad*4 + quad_index);
            }
         }
    }


    /* Init local SFI ports, currently only 36 links (SOC_DPP_NOF_FABRIC_LINKS) */
    for (fabric_link = FABRIC_LOGICAL_PORT_BASE(unit);
        fabric_link < FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, nof_fabric_links);
        ++fabric_link) {  
        disabled_port = FALSE;    
        if (PBMP_MEMBER(pbmp_disabled, fabric_link)) {
            disabled_port = TRUE;
            DPP_ADD_DISABLED_PORT(sfi, fabric_link); 
            DPP_ADD_DISABLED_PORT(port, fabric_link); 
            DPP_ADD_DISABLED_PORT(all, fabric_link); 
        }
        else {
            PORT_SW_DB_PORT_ADD(sfi, fabric_link);
            PORT_SW_DB_PORT_ADD(port,fabric_link);
            PORT_SW_DB_PORT_ADD(all,fabric_link);
        }
        /* 
         * Fabric ports mapping to physical ports
         */
        phy_port = SOC_DPP_ARAD_FABRIC_PORT_TO_PHY_PORT(unit, fabric_link);   

        blk = SOC_PORT_IDX_INFO(unit, phy_port, 0).blk;
        if (blk < 0) { /* empty slot */
            disabled_port = TRUE;
            blktype = 0;
        } 
        else
        {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
        }
        
        if (disabled_port) {
            sal_snprintf(si->port_name[fabric_link], sizeof(si->port_name[fabric_link]),
                         "?%d", fabric_link);
            si->port_offset[fabric_link] = fabric_link;
            continue;
        }
        sal_snprintf(si->port_name[fabric_link], sizeof(si->port_name[fabric_link]),
                         "sfi%d", fabric_link);
        sal_snprintf(si->port_name_alter[fabric_link], sizeof(si->port_name_alter[fabric_link]),
                         "fabric%d", fabric_link - SOC_INFO(unit).fabric_logical_port_base);
        si->port_name_alter_valid[fabric_link] = 1;
        SOC_PBMP_PORT_ADD(si->block_bitmap[blk], fabric_link); 
        si->port_p2l_mapping[phy_port] = fabric_link;   
        si->port_l2p_mapping[fabric_link] = phy_port;          
        si->port_type[fabric_link] = blktype;
        ++si->port_num;
    }

    SOCDNX_IF_ERR_EXIT(soc_arad_collect_nif_config(unit));

    /* OCB */
    
    SOCDNX_IF_ERR_EXIT(soc_arad_ocb_enable_mode(unit, &(dpp_arad->init.ocb))); 
    SOCDNX_IF_ERR_EXIT(soc_arad_validate_ocb_enable(unit, dpp_arad->init.ocb.ocb_enable)); 

    dpp_arad->init.ocb.databuffer_size = soc_property_get(unit, spn_BCM886XX_OCB_DATABUFFER_SIZE, 256);
    val = soc_property_get(unit, spn_BCM886XX_OCB_REPARTITION, 0);
    switch(val)
    {
      case 0:
        dpp_arad->init.ocb.repartition_mode = ARAD_OCB_REPARTITION_MODE_80_PRESENTS_UNICAST;
        break;
      case 1:
        dpp_arad->init.ocb.repartition_mode = ARAD_OCB_REPARTITION_MODE_ALL_UNICAST;
        break;
      default:
        SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
    }

      /* DRAM - PLL */
      val = soc_property_get(unit, spn_SERDES_NIF_CLK_FREQ, 0);
      switch(val)
      {
          case 0:
              dpp_arad->init.pll.nif_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_125;
              break;
          case 1:
              dpp_arad->init.pll.nif_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_156_25;
              break;
          default:
              SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
      }

      val = soc_property_get(unit, spn_SERDES_FABRIC_CLK_FREQ, 0);
      switch(val)
      {
          case 0:
              dpp_arad->init.pll.fabric_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_125;
              break;
          case 1:
              dpp_arad->init.pll.fabric_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_156_25;
              break;
          default:
              SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
      }

      val = soc_property_get(unit, spn_SYNTH_DRAM_FREQ, 25);
      switch(val)
      {
        case 25:
            dpp_arad->init.pll.synthesizer_clock_freq = ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_25_MHZ;
            break;
        case 125:
            dpp_arad->init.pll.synthesizer_clock_freq = ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_125_MHZ;
            break;
        default:
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
      }

      /* The following code uses val --> don't move this code */
      val = soc_property_get(unit, spn_EXT_RAM_PRESENT, 0);
      dpp_arad->init.dram.nof_drams = val;
      
      SOCDNX_IF_ERR_EXIT(soc_arad_validate_device_num_of_dram(unit, val));

      if (SOC_IS_DPP_DRC_COMBO28(unit)) {
            /* Clear Drc info */
            rv = soc_dpp_drc_combo28_info_config_default(unit, &(dpp_arad->init.drc_info));
            SOCDNX_IF_ERR_EXIT(rv);

            /* Adjust Dram Bitmap */
            if (SOC_IS_ARDON(unit)) {
                /* Allowed values for Ardon/88202: 0 / 1 (Dram C) / 2 (Dram's C, D) / 3 (Dram's A, C, D) / 4 (Dram's A, B, C, D) / */
                switch (val) {
                case 0:
                    break;
                case 1:
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                    break;
                case 2:
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 3);
                    break;
                case 3:
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 0);
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 3);
                    break;
                case 4:
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 0);
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 1);
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                    SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 3);
                    break;
                default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Ardon: Unsupported number of external dram interfaces (%d)"), val));
                }
            } else if (SOC_IS_JERICHO(unit)) {
                dram_bitmap = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dram_bitamp", 0x0);
                if (dram_bitmap != 0) {
                    SOC_PBMP_WORD_SET(dpp_arad->init.drc_info.dram_bitmap, 0, dram_bitmap);
                } else {

                    /* Allowed values for Jericho/88670:
                     *  0
                     *  2  (Dram's A, C)
                     *  3  (Dram's A, B, C)
                     *  41 (Dram's A, B, C, D)
                     *  42 (Dram's A, C, F, H)
                     *  6  (Dram's A, C, D, E, F, H)
                     *  8  (Dram's A, B, C, D, E, F, G, H)
                     */
                    switch (val) {
                    case 0:
                        break;
                    case 2:
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                        break;
                    case 3:
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 1);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                        break;
                    case 41:
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 1);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 3);
                        break;
                    case 42:
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 5);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 7);
                        break;
                    case 6:
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 3);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 4);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 5);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 7);
                        break;
                    case 8:
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 0);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 1);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 2);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 3);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 4);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 5);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 6);
                        SOC_PBMP_PORT_ADD(dpp_arad->init.drc_info.dram_bitmap, 7);
                        break;
                    default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Jericho: Unsupported number of external dram interfaces (%d)"), val));
                    }
                }
            } else {
                SOC_PBMP_WORD_SET(dpp_arad->init.drc_info.dram_bitmap, 0, val);
            }
            
           /* Set DRC Info */
           rv = soc_dpp_drc_combo28_info_config(unit, &(dpp_arad->init.drc_info));
           SOCDNX_IF_ERR_EXIT(rv);

           dpp_arad->init.drc_info.device_core_freq = dpp_arad->init.core_freq.system_ref_clock;
           if ((SOC_DPP_CONFIG(unit)->emulation_system) 
#if defined(PLISIM)
            || (SAL_BOOT_PLISIM)
#endif
               )
           {
               dpp_arad->init.drc_info.sim_system_mode = 1;
           }
           

           
      } else {
       
          if (val == 0) {
              /* No dram */
              dpp_arad->init.dram.enable = FALSE;
          } else {
              dpp_arad->init.dram.enable = TRUE;
        
            switch (val) {
              case 1:
                if ((dpp_arad->init.ocb.ocb_enable == OCB_DISABLED) || (dpp_arad->init.ocb.ocb_enable == OCB_ENABLED)) {
                    /* 1 Dram allowed only in one-way-bypass mode */
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported number of external dram interfaces (%d). Not one-way-bypass mode"), val));
                }
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = FALSE;
                dpp_arad->init.dram.is_valid[2] = FALSE;
                dpp_arad->init.dram.is_valid[3] = FALSE;
                dpp_arad->init.dram.is_valid[4] = FALSE;
                dpp_arad->init.dram.is_valid[5] = FALSE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 2:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = FALSE;
                dpp_arad->init.dram.is_valid[2] = FALSE;
                dpp_arad->init.dram.is_valid[3] = TRUE;
                dpp_arad->init.dram.is_valid[4] = FALSE;
                dpp_arad->init.dram.is_valid[5] = FALSE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 3:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = TRUE;
                dpp_arad->init.dram.is_valid[2] = TRUE;
                dpp_arad->init.dram.is_valid[3] = FALSE;
                dpp_arad->init.dram.is_valid[4] = FALSE;
                dpp_arad->init.dram.is_valid[5] = FALSE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 4:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = TRUE;
                dpp_arad->init.dram.is_valid[2] = FALSE;
                dpp_arad->init.dram.is_valid[3] = TRUE;
                dpp_arad->init.dram.is_valid[4] = TRUE;
                dpp_arad->init.dram.is_valid[5] = FALSE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 6:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = TRUE;
                dpp_arad->init.dram.is_valid[2] = TRUE;
                dpp_arad->init.dram.is_valid[3] = TRUE;
                dpp_arad->init.dram.is_valid[4] = TRUE;
                dpp_arad->init.dram.is_valid[5] = TRUE;
                dpp_arad->init.dram.is_valid[6] = FALSE;
                dpp_arad->init.dram.is_valid[7] = FALSE;
                break;
              case 8:
                dpp_arad->init.dram.is_valid[0] = TRUE;
                dpp_arad->init.dram.is_valid[1] = TRUE;
                dpp_arad->init.dram.is_valid[2] = TRUE;
                dpp_arad->init.dram.is_valid[3] = TRUE;
                dpp_arad->init.dram.is_valid[4] = TRUE;
                dpp_arad->init.dram.is_valid[5] = TRUE;
                dpp_arad->init.dram.is_valid[6] = TRUE;
                dpp_arad->init.dram.is_valid[7] = TRUE;
                break;
              default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported number of external dram interfaces (%d)"), val));
            }
    
        /* Set block_valid = FALSE to unconfigured DRAM's blocks. */
          {
              int drams[8] = {SOC_BLK_DRCA, SOC_BLK_DRCB, SOC_BLK_DRCC, SOC_BLK_DRCD, SOC_BLK_DRCE, SOC_BLK_DRCF, SOC_BLK_DRCG, SOC_BLK_DRCH};
           
              for(i=0; i<8; ++i) {
                  if(dpp_arad->init.dram.enable == FALSE || dpp_arad->init.dram.is_valid[i] == FALSE) {
                      SOC_BLOCK_ITER(unit, val, drams[i]){
                         SOC_INFO(unit).block_valid[val]= FALSE;
                      }
                  }
              }
          }    
    
          rv = soc_dpp_str_prop_ext_ram_type(unit, &dpp_arad->init.dram.dram_type);
          SOCDNX_IF_ERR_EXIT(rv);
    
          dpp_arad->init.dram.pll_conf.m = soc_property_get(unit, spn_EXT_QDR_PLL_M, 0);
          dpp_arad->init.dram.pll_conf.n = soc_property_get(unit, spn_EXT_QDR_PLL_N, 0);
          dpp_arad->init.dram.pll_conf.p = soc_property_get(unit, spn_EXT_QDR_PLL_P, 0);
          
          dpp_arad->init.dram.conf_mode = ARAD_HW_DRAM_CONF_MODE_PARAMS;
          dpp_arad->init.dram.dram_conf.params_mode.dram_freq = soc_property_get(unit, spn_EXT_RAM_FREQ, 0);
          if (dpp_arad->init.dram.dram_conf.params_mode.dram_freq < 500){
              SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ERROR PLL setup does not support dram freq below 500Mhz")));
          }
          
          rv = soc_dpp_prop_parse_dram_number_of_columns(unit, &dpp_arad->init.dram.nof_columns);
          SOCDNX_IF_ERR_EXIT(rv);
          dpp_arad->init.dram.dram_conf.params_mode.params.nof_cols = dpp_arad->init.dram.nof_columns;
    
          rv = soc_arad_prop_parse_dram_number_of_rows(unit,&dpp_arad->init.dram.nof_rows);
          SOCDNX_IF_ERR_EXIT(rv);
    
          rv = soc_dpp_prop_parse_dram_number_of_banks(unit, &dpp_arad->init.dram.nof_banks);
          SOCDNX_IF_ERR_EXIT(rv);
          dpp_arad->init.dram.dram_conf.params_mode.params.nof_banks = dpp_arad->init.dram.nof_banks;     
    
#ifdef BCM_DDR3_SUPPORT
          SOC_DDR3_NUM_MEMORIES(unit)          = dpp_arad->init.dram.nof_drams * 2;
          SOC_DDR3_CLOCK_MHZ(unit)             = dpp_arad->init.dram.dram_conf.params_mode.dram_freq;
          SOC_DDR3_NUM_COLUMNS(unit)           = soc_property_get(unit,spn_EXT_RAM_COLUMNS, 1024);
          SOC_DDR3_NUM_ROWS(unit)              = dpp_arad->init.dram.nof_rows;
          SOC_DDR3_NUM_BANKS(unit)             = dpp_arad->init.dram.dram_conf.params_mode.params.nof_banks;
          SOC_DDR3_MEM_GRADE(unit)             = soc_property_get(unit, spn_DDR3_MEM_GRADE, 0x111111); 
          SOC_DDR3_OFFSET_WR_DQ_CI02_WL0(unit) = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, 
                                                                             "ddr3_offset_we_dq_ci02_wl0", SHMOO_CI02_WL0_OFFSET_WR_DQ);
          SOC_DDR3_OFFSET_WR_DQ_CI00_WL1(unit)  = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, 
                                                                             "ddr3_offset_we_dq_ci00_wl1", SHMOO_CI0_WL1_OFFSET_WR_DQ);
#endif
        
          rv = soc_dpp_prop_parse_dram_ap_bit_pos(unit, &dpp_arad->init.dram.dram_conf.params_mode.params.ap_bit_pos);
          SOCDNX_IF_ERR_EXIT(rv);
    
          rv = soc_dpp_prop_parse_dram_burst_size(unit, &dpp_arad->init.dram.dram_conf.params_mode.params.burst_size);
          SOCDNX_IF_ERR_EXIT(rv);
    
          dpp_arad->init.dram.dram_conf.params_mode.params.auto_mode = TRUE;
          dpp_arad->init.dram.dram_conf.params_mode.params.c_cas_latency = soc_property_get(unit, spn_EXT_RAM_C_CAS_LATENCY, prop_invalid);
          dpp_arad->init.dram.dram_conf.params_mode.params.c_wr_latency = soc_property_get(unit, spn_EXT_RAM_C_WR_LATENCY, prop_invalid);
          dpp_arad->init.dram.dram_conf.params_mode.params.t_rc =  soc_property_get(unit, spn_EXT_RAM_T_RC, prop_invalid);
          dpp_arad->init.dram.dram_conf.params_mode.params.jedec =  soc_property_get(unit, spn_EXT_RAM_JEDEC, prop_invalid);
    
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RFC, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rfc);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RAS, &dpp_arad->init.dram.dram_conf.params_mode.params.t_ras);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_FAW, &dpp_arad->init.dram.dram_conf.params_mode.params.t_faw);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RCD_RD, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rcd_rd);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RCD_WR, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rcd_wr);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RRD, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rrd);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_REF, &dpp_arad->init.dram.dram_conf.params_mode.params.t_ref);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RP, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rp);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_WR, &dpp_arad->init.dram.dram_conf.params_mode.params.t_wr);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_WTR, &dpp_arad->init.dram.dram_conf.params_mode.params.t_wtr);
          SOCDNX_IF_ERR_EXIT(rv);
          rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RTP, &dpp_arad->init.dram.dram_conf.params_mode.params.t_rtp);
          SOCDNX_IF_ERR_EXIT(rv);
    
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs0_wr1, spn_EXT_RAM_DDR3_MRS0_WR1);
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs0_wr2, spn_EXT_RAM_DDR3_MRS0_WR2);
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs1_wr1, spn_EXT_RAM_DDR3_MRS1_WR1);
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs2_wr1, spn_EXT_RAM_DDR3_MRS2_WR1);
          SOC_DPP_ARAD_DRAM_MODE_REG_SET(dpp_arad->init.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs3_wr1, spn_EXT_RAM_DDR3_MRS3_WR1);
         
        }
    
        /* Enable/disable Dram BIST on initialization */
        dpp_arad->init.dram.bist_enable = soc_property_suffix_num_get(unit, -1, spn_BIST_ENABLE, "dram", 0x1);
        
        /* get interrupts application max num of crc err for dram buffer */
        val = soc_property_get(unit, spn_DRAM_CRC_DEL_BUFFER_MAX_RECLAIMS, 0x0);
        
    
        if(SOC_SAND_FAILURE(arad_dram_crc_del_buffer_max_reclaims_set_unsafe((uint32)unit, (uint32)val))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("arad_dram_crc_del_buffer_max_reclaims_set_unsafe error")));
        }
    
        /* dram clam shell mode */
        for(ind = 0; ind < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++ind) {
          if(soc_property_port_get(unit, ind, spn_DRAM0_CLAMSHELL_ENABLE, FALSE))
          {
            if(soc_property_port_get(unit, ind, spn_DRAM1_CLAMSHELL_ENABLE, FALSE)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("only one of the dram should be enabled")));
            }
            dpp_arad->init.dram.dram_clam_shell_mode[ind] = ARAD_DDR_CLAM_SHELL_MODE_DRAM_0;
          }
          else if(soc_property_port_get(unit, ind, spn_DRAM1_CLAMSHELL_ENABLE, FALSE)) {
            dpp_arad->init.dram.dram_clam_shell_mode[ind] = ARAD_DDR_CLAM_SHELL_MODE_DRAM_1;
          }
          else
          {
            dpp_arad->init.dram.dram_clam_shell_mode[ind] = ARAD_DDR_CLAM_SHELL_MODE_DISABLED;
          }
        }
      }

    /* Dynamic nif enable */
      dpp_arad->init.dynamic_port_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dynamic_port", FALSE);


    /* OOB External mode */
     SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_external_voltage_mode_get(unit, &(dpp_arad->init.ex_vol_mod) ));


    /* Statistic-Interface */
    dpp_arad->init.stat_if.stat_if_enable = soc_property_get(unit, spn_STAT_IF_ENABLE, FALSE);

    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
        soc_port_t port;

        dpp_arad->init.stat_if.stat_if_enable = FALSE;
        for(port=0; port<SOC_MAX_NUM_PORTS ; port++){
            dpp_arad->init.stat_if.stat_if_info.stat_if_port_enable[port] = soc_property_port_get(unit, port, spn_STAT_IF_ENABLE, FALSE);
            dpp_arad->init.stat_if.stat_if_enable |= dpp_arad->init.stat_if.stat_if_info.stat_if_port_enable[port];
        }
    }

    if (dpp_arad->init.stat_if.stat_if_enable) {
        rv = soc_arad_str_prop_stat_if_report_mode_get(unit, &(dpp_arad->init.stat_if.stat_if_info.mode));
        SOCDNX_IF_ERR_EXIT(rv);
 
        dpp_arad->init.stat_if.stat_if_info.is_idle_reports_present = soc_property_get(unit, spn_STAT_IF_IDLE_REPORTS_PRESENT, TRUE);
        dpp_arad->init.stat_if.stat_if_info.if_report_original_pkt_size = soc_property_get(unit, spn_STAT_IF_REPORT_ORIGINAL_PKT_SIZE, FALSE);


        if(SOC_TMC_STAT_IF_REPORT_MODE_BILLING == dpp_arad->init.stat_if.stat_if_info.mode || (SOC_IS_ARAD_B0_AND_ABOVE(unit) && SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER == dpp_arad->init.stat_if.stat_if_info.mode)) {
        
            rv = soc_arad_str_prop_stat_if_billing_pkt_size_get(unit, &(dpp_arad->init.stat_if.stat_if_info.if_pkt_size));
            SOCDNX_IF_ERR_EXIT(rv);
            dpp_arad->init.stat_if.stat_if_info.if_report_mc_once = soc_property_get(unit, spn_STAT_IF_REPORT_MULTICAST_SINGLE_COPY, TRUE);
        }
        else {
            rv = soc_arad_str_prop_stat_if_qsize_pkt_size_get(unit, &(dpp_arad->init.stat_if.stat_if_info.if_pkt_size));
            SOCDNX_IF_ERR_EXIT(rv);

        }
       
        dpp_arad->init.stat_if.stat_if_info.if_scrubber_queue_min          = soc_property_get(unit, spn_STAT_IF_SCRUBBER_QUEUE_MIN, ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MIN);
        dpp_arad->init.stat_if.stat_if_info.if_scrubber_queue_max          = soc_property_get(unit, spn_STAT_IF_SCRUBBER_QUEUE_MAX, ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MAX);
        dpp_arad->init.stat_if.stat_if_info.if_scrubber_rate_min           = soc_property_get(unit, spn_STAT_IF_SCRUBBER_RATE_MIN,  ARAD_STAT_IF_REPORT_SCRUBBER_DISABLE);
        dpp_arad->init.stat_if.stat_if_info.if_scrubber_rate_max           = soc_property_get(unit, spn_STAT_IF_SCRUBBER_RATE_MAX,  ARAD_STAT_IF_REPORT_SCRUBBER_DISABLE);

#ifdef BCM_88650_B0
        if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
            dpp_arad->init.stat_if.stat_if_info.if_qsize_queue_min          = soc_property_get(unit, spn_STAT_IF_SELECTIVE_REPORT_QUEUE_MIN, ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MIN);
            dpp_arad->init.stat_if.stat_if_info.if_qsize_queue_max          = soc_property_get(unit, spn_STAT_IF_SELECTIVE_REPORT_QUEUE_MIN, ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MAX);
        }
#endif

        for(ind = 0; ind < ARAD_STAT_IF_REPORT_THRESHOLD_LINES; ++ind) {
            dpp_arad->init.stat_if.stat_if_info.if_scrubber_buffer_descr_th[ind]    = soc_property_port_get(unit, ind, spn_STAT_IF_SCRUBBER_BUFFER_DESCR_TH, ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED);
            dpp_arad->init.stat_if.stat_if_info.if_scrubber_bdb_th[ind]             = soc_property_port_get(unit, ind, spn_STAT_IF_SCRUBBER_BDB_TH, ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED);
            dpp_arad->init.stat_if.stat_if_info.if_scrubber_uc_dram_buffer_th[ind]  = soc_property_port_get(unit, ind, spn_STAT_IF_SCRUBBER_UC_DRAM_BUFFER_TH, ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED);
        }
        
       dpp_arad->init.stat_if.stat_if_info.if_report_enqueue_enable = soc_property_get(unit, spn_STAT_IF_REPORT_ENQUEUE_ENABLE, 1);
       dpp_arad->init.stat_if.stat_if_info.if_report_dequeue_enable = soc_property_get(unit, spn_STAT_IF_REPORT_DEQUEUE_ENABLE, 1);
    
    }

    for(ind = 0; ind<SOC_TMC_FC_NOF_OOB_IDS; ind++) {
        val = soc_property_port_get(unit, ind, spn_FC_OOB_TX_FREQ_RATIO, 4);
        switch (val)
        {
        case 2:
          dpp_arad->init.fc.oob_tx_speed[ind] = ARAD_FC_OOB_TX_SPEED_CORE_2;
          break;
        case 8:
          dpp_arad->init.fc.oob_tx_speed[ind] = ARAD_FC_OOB_TX_SPEED_CORE_8;
          break;
        case 4:
        default:
          dpp_arad->init.fc.oob_tx_speed[ind] = ARAD_FC_OOB_TX_SPEED_CORE_4;
        }     
      }

      /* flow control properties - Out-Of-Band */
      for (ind = 0; ind < SOC_TMC_FC_NOF_OOB_IDS; ind++) {
        dpp_arad->init.fc.fc_oob_type[ind] = soc_property_port_get(unit, ind, spn_FC_OOB_TYPE, 0);

        dpp_arad->init.fc.fc_directions[ind] = soc_property_port_get(unit, ind, spn_FC_OOB_MODE, 0);
        dpp_arad->init.fc.fc_directions[ind] &= (SOC_DPP_FC_CAL_MODE_RX_ENABLE | SOC_DPP_FC_CAL_MODE_TX_ENABLE);

        dpp_arad->init.fc.fc_oob_calender_length[ind][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, ind, spn_FC_OOB_CALENDER_LENGTH, "rx", 0);
        dpp_arad->init.fc.fc_oob_calender_length[ind][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, ind, spn_FC_OOB_CALENDER_LENGTH, "tx", 0);

        dpp_arad->init.fc.fc_oob_calender_rep_count[ind][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, ind, spn_FC_OOB_CALENDER_REP_COUNT, "rx", 0);
        dpp_arad->init.fc.fc_oob_calender_rep_count[ind][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, ind, spn_FC_OOB_CALENDER_REP_COUNT, "tx", 0);

      }

      dpp_arad->init.fc.cl_sch_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "cl_scheduler_fc", 0);


    /* pp init {*/

    
    if (dpp_arad->init.pp_enable) {
      
      dpp_arad->pp_op_mode.authentication_enable = FALSE;
      dpp_arad->pp_op_mode.hairpin_enable = TRUE;
      dpp_arad->pp_op_mode.system_vsi_enable = FALSE;
      dpp_arad->pp_op_mode.mpls_info.mpls_ether_types[0] = 0x8847;
      dpp_arad->pp_op_mode.mpls_info.mpls_ether_types[1] = 0x8848;
      dpp_arad->pp_op_mode.ipv4_info.nof_vrfs = SOC_DPP_CONFIG(unit)->l3.max_nof_vrfs;
      dpp_arad->pp_op_mode.ipv4_info.flags = ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE|ARAD_PP_MGMT_IPV4_SHARED_ROUTES_MEMORY|ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG;
      dpp_arad->pp_op_mode.ipv4_info.pvlan_enable = FALSE;
      dpp_arad->pp_op_mode.mim_enable = FALSE;

      for (ind = 0; ind < dpp_arad->pp_op_mode.ipv4_info.nof_vrfs; ++ind)
      {
        if (ind == 0 )
        {
          dpp_arad->pp_op_mode.ipv4_info.max_routes_in_vrf[ind] = soc_property_get(unit,spn_IPV4_NUM_ROUTES,50000);
        }
        else
        {
          dpp_arad->pp_op_mode.ipv4_info.max_routes_in_vrf[ind] = 0;
        }
      }

      /* configure for each LPM bank how bits to consider */

      /* first has to be 14 */
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[0] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE1_SIZE,14);
      if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[0] != 14) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("LPM first slices size has to be 14")));
      }

      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[1] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE2_SIZE,0);
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[2] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE3_SIZE,0);
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[3] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE4_SIZE,0);
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[4] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE5_SIZE,0);
      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[5] = soc_property_get(unit,spn_BCM886XX_IPV4_SLICE6_SIZE,0);

      dpp_arad->pp_op_mode.ipv4_info.bits_in_phase_valid = 0;

      /* check if user configured any of the phases */
      for (ind = 1; ind < ARAD_PP_MGMT_IPV4_LPM_BANKS; ++ind) {
          if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind] != 0){
              dpp_arad->pp_op_mode.ipv4_info.bits_in_phase_valid = 1;
              break;
          }
      }

      ip_lpm_total = dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[0];
      /* if user updated, then check parameters are ok */
      if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase_valid == 1){
          for (ind = 1; ind < ARAD_PP_MGMT_IPV4_LPM_BANKS; ++ind) {
              if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind] != 0){
                  if(dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind] < 3 ||
                     dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind] > 7
                    ) {
                      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("incorrcet slice size for LPM config")));
                  }
                  ip_lpm_total += dpp_arad->pp_op_mode.ipv4_info.bits_in_phase[ind];
              }
          }

          /* check total bits equal to IP-length + vrf bits */
          if(ip_lpm_total != 32 + soc_sand_log2_round_up(dpp_arad->pp_op_mode.ipv4_info.nof_vrfs)) {
              SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("incorrcet slice size for LPM config")));
          }
      }

      dpp_arad->pp_op_mode.authentication_enable = (soc_property_get(unit, spn_SA_AUTH_ENABLED, 0) == 1 ? TRUE : FALSE);

      auxiliary_table_mode = soc_property_get(unit, spn_BCM886XX_AUXILIARY_TABLE_MODE, 0);  
      SOC_DPP_CONFIG(unit)->pp.pvlan_enable = FALSE;
      
      if (auxiliary_table_mode == 0) {
          dpp_arad->pp_op_mode.split_horizon_filter_enable = FALSE;
          dpp_arad->pp_op_mode.ipv4_info.pvlan_enable = TRUE;
          SOC_DPP_CONFIG(unit)->pp.pvlan_enable = TRUE;
      }
      else if (auxiliary_table_mode == 1) {
          dpp_arad->pp_op_mode.split_horizon_filter_enable = TRUE;
      }      
      else if (auxiliary_table_mode == 2) {
          /* Enable Mac-in-Mac */
          if (dpp_arad->pp_op_mode.authentication_enable) {
            /* Error: SA authentication and Mac-in-Mac cannot be both set */
            SOCDNX_IF_ERR_EXIT(SOC_E_FAIL);
          }
          dpp_arad->pp_op_mode.split_horizon_filter_enable = FALSE;
          dpp_arad->pp_op_mode.mim_enable = TRUE;
      }

      /* VLAN translation / MPLS termination stage */
      dpp_arad->pp_op_mode.mpls_info.fast_reroute_labels_enable = (soc_property_get(unit, spn_FAST_REROUTE_LABELS_ENABLE, 0) == 1 ? TRUE : FALSE);       
      dpp_arad->pp_op_mode.mpls_info.mpls_termination_label_index_enable = (soc_property_get(unit, spn_MPLS_TERMINATION_LABEL_INDEX_ENABLE, 0) == 1 ? TRUE : FALSE);
      dpp_arad->pp_op_mode.mpls_info.mpls_coupling_enable = (soc_property_get(unit, spn_MPLS_CONTEXT_SPECIFIC_LABEL_ENABLE, 0) == 1 ? TRUE : FALSE);       

      rv = soc_dpp_str_prop_parse_mpls_context(unit,&(dpp_arad->pp_op_mode.mpls_info.lookup_include_inrif),&include_port);
      SOCDNX_IF_ERR_EXIT(rv);

      rv = soc_arad_str_prop_parse_mpls_termination_database_mode(unit,dpp_arad->pp_op_mode.mpls_info.mpls_termination_label_index_enable, 
                                                                  dpp_arad->pp_op_mode.mpls_info.lookup_include_inrif, 
                                                                  SOC_DPP_CONFIG(unit)->pp.mpls_databases,SOC_DPP_CONFIG(unit)->pp.mpls_namespaces);
      SOCDNX_IF_ERR_EXIT(rv);

      
      /* Verify several VTT misconfigurations */
      if (SOC_DPP_CONFIG(unit)->trill.mode && (SOC_DPP_CONFIG(unit)->pp.vlan_match_db_mode != SOC_DPP_VLAN_DB_MODE_DEFAULT)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Trill application and DB mode (%d) cant be both set"), SOC_DPP_CONFIG(unit)->pp.vlan_match_db_mode));
      }

      /* L2oIP only one can be enabled */
      if ((SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE) && 
          SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN | SOC_DPP_IP_TUNNEL_TERM_DB_ETHER)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("IP applications L2oIP only one mode of VXLAN, NVGRE and EoIP can be enabled. Bimtap enabled 0x%x"), SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable));
      }

      if ((SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) && 
          SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE | SOC_DPP_IP_TUNNEL_TERM_DB_ETHER)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("IP applications L2oIP only one mode of VXLAN, NVGRE and EoIP can be enabled. Bimtap enabled 0x%x"), SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable));
      }

      if ((SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_ETHER) && 
          SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE | SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("IP applications L2oIP only one mode of VXLAN, NVGRE and EoIP can be enabled.")));
      }

      if (SOC_DPP_CONFIG(unit)->arad->pp_op_mode.mpls_info.mpls_coupling_enable && SOC_DPP_CONFIG(unit)->pp.evb_enable) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("EVB and Coupling do not co exist on same device")));
      }

      if (SOC_DPP_CONFIG(unit)->arad->pp_op_mode.mpls_info.fast_reroute_labels_enable && SOC_DPP_CONFIG(unit)->pp.evb_enable) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("EVB and FRR do not co exist on same device")));
      }

      if (SOC_DPP_CONFIG(unit)->arad->pp_op_mode.mpls_info.fast_reroute_labels_enable && SOC_DPP_CONFIG(unit)->pp.explicit_null_support) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Explicit NULL and FRR do not co exist on same device becuase of VT port profile resources")));
      }

      if (SOC_DPP_CONFIG(unit)->pp.evb_enable && SOC_DPP_CONFIG(unit)->pp.explicit_null_support) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Explicit NULL and EVB do not co exist on same device becuase of VT port profile resources")));
      }

      if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable && SOC_DPP_CONFIG(unit)->pp.evb_enable) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("EVB and PON do not co exist on same device")));
      }

      if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable && SOC_DPP_CONFIG(unit)->trill.mode) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("PON and Trill do not co exist on same device")));
      }

      if ((SOC_DPP_CONFIG(unit)->pp.explicit_null_support || SOC_DPP_CONFIG(unit)->pp.gal_support) && (!SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only))  {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("gal and explicit NULL supported only in case tunnel termination is done only in TT stage")));
      }

      /* Verify several FLP misconfigurations */
      if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IPV4 || SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IP) 
          && SOC_DPP_CONFIG(unit)->trill.mode) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Trill and L3 PON Source bind do not co exist on same device")));
      }

      if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IPV4 || SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IP) 
          && dpp_arad->pp_op_mode.mpls_info.mpls_coupling_enable) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("MPLS Coupling and L3 PON Source bind do not co exist on same device")));
      }      

      /* max vsi is reserved for Mac-in-mac */
      dpp_arad->pp_op_mode.p2p_info.mim_vsi = ARAD_PP_VSI_ID_MAX;

     /* Default trap/snoop strength */
      SOC_DPP_CONFIG(unit)->pp.default_trap_strength = soc_property_get(unit, spn_DEFAULT_TRAP_STRENGTH, 4);
      SOC_DPP_CONFIG(unit)->pp.default_snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 1);
    
      /* IP tunnel initator range */
      SOC_DPP_CONFIG(unit)->pp.min_egress_encap_ip_tunnel_range = soc_property_get(unit, spn_EGRESS_ENCAP_IP_TUNNEL_RANGE_MIN, 0x1001);
      SOC_DPP_CONFIG(unit)->pp.max_egress_encap_ip_tunnel_range = soc_property_get(unit, spn_EGRESS_ENCAP_IP_TUNNEL_RANGE_MAX, 0x2000);
      
      /* Application Extend P2P */
      SOC_DPP_CONFIG(unit)->pp.extend_p2p_global_enable = FALSE;
      for (port_i = 0; port_i < SOC_MAX_NUM_PORTS ; ++port_i) {
          SOC_DPP_CONFIG(unit)->pp.extend_p2p_port_enable[port_i] = soc_property_port_get(unit, port_i, spn_BCM886XX_PORT_EXTEND_P2P, 0);
          if (SOC_DPP_CONFIG(unit)->pp.extend_p2p_port_enable[port_i]) {
              SOC_DPP_CONFIG(unit)->pp.extend_p2p_global_enable = TRUE;
          }
      } 
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_str_prop_parse_logical_port_mim(unit, &SOC_DPP_CONFIG(unit)->pp.mim_lif_ndx, &SOC_DPP_CONFIG(unit)->pp.mim_out_ac));

      /* Init VLAN edit values */
      SOC_DPP_CONFIG(unit)->pp.nof_ive_action_ids = SOC_PPC_NOF_INGRESS_VLAN_EDIT_ACTION_IDS_ARAD;
      SOC_DPP_CONFIG(unit)->pp.nof_ive_reserved_action_ids = SOC_PPC_NOF_INGRESS_VLAN_EDIT_RESERVED_ACTION_IDS_ARAD;
      SOC_DPP_CONFIG(unit)->pp.nof_eve_action_mappings = SOC_PPC_NOF_EGRESS_VLAN_EDIT_ACTION_MAPPINGS_ARAD;
      SOC_DPP_CONFIG(unit)->pp.nof_eve_action_ids = SOC_PPC_NOF_EGRESS_VLAN_EDIT_ACTION_IDS_ARAD;
      SOC_DPP_CONFIG(unit)->pp.nof_eve_reserved_action_ids = SOC_PPC_NOF_EGRESS_VLAN_EDIT_RESERVED_ACTION_IDS_ARAD;
    }
    /* pp init }*/


    /* IEEE1588 DPLL mode, 0 - phase lock, 1 - frequency lock */
    dpp_arad->init.pll.ts_clk_mode = soc_property_get(unit, spn_PHY_1588_DPLL_FREQUENCY_LOCK, 0x0);
    dpp_arad->init.pll.ts_pll_phase_initial_lo = soc_property_get(unit, spn_PHY_1588_DPLL_PHASE_INITIAL_LO, 0x40000000);
    dpp_arad->init.pll.ts_pll_phase_initial_hi = soc_property_get(unit, spn_PHY_1588_DPLL_PHASE_INITIAL_HI, 0x10000000);

    /* Phase access of egress encapsulation bank */
    for (i = 0; i < SOC_PPD_EG_ENCAP_NOF_BANKS; i++) {
        bank_phase = soc_property_port_get(unit, i, spn_EGRESS_ENCAP_BANK_PHASE, 0);
        if (bank_phase == egress_encap_bank_phase_static_5 || bank_phase == egress_encap_bank_phase_static_6) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Doesn't support to allocate a static egress encap bank for AC and PWE")));
        }
        SOC_DPP_CONFIG(unit)->pp.egress_encap_bank_phase[i] = bank_phase;
    }

    /* Port map is now set for soc init portion ISQ/FMQ added later */
    /* don't clear all entries later whether default setup or not   */
    _dflt_tm_pp_port_map[unit] = FALSE;

    /* 
     *  Init DMA information
     */ 

#ifdef BCM_SBUSDMA_SUPPORT
        SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DPP_ARAD_MAX_SBUSDMA_CHANEELS;
        SOC_CONTROL(unit)->tdma_ch              = SOC_DPP_ARAD_TDMA_CHANNEL;
        SOC_CONTROL(unit)->tslam_ch             = SOC_DPP_ARAD_TSLAM_CHANNEL;
        SOC_CONTROL(unit)->desc_ch              = SOC_DPP_ARAD_DESC_CHANNEL;
        /* maximum possible memory entry size used for clearing memory, should be a multiple of 32bit words, */
        SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, 
            soc_property_get(unit, spn_MEM_CLEAR_CHUNK_SIZE, SOC_DPP_MEM_CLEAR_CHUNK_SIZE));
#endif

#ifdef BCM_ARAD_SUPPORT
      SOC_DPP_CONFIG(unit)->pp.roo_enable = (soc_property_get(unit, spn_BCM886XX_ROO_ENABLE, 0)); 
#endif

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_arad_dma_mutex_init(int unit)
{
    soc_control_t *soc;
    
    SOCDNX_INIT_FUNC_DEFS

    soc = SOC_CONTROL(unit);

    /* Table DMA timeout, interrupt mode and mutex allocation accrding to SOC property configuration */
    soc->tableDmaMutex = NULL;        
    soc->tableDmaIntr = NULL;     
    if (soc_property_get(unit, spn_TABLE_DMA_ENABLE, 1)) {         
        if (SAL_BOOT_QUICKTURN) {             
            soc->tableDmaTimeout = TDMA_TIMEOUT_QT;        
        } else {            
            soc->tableDmaTimeout = TDMA_TIMEOUT;        
        } 

        soc->tableDmaTimeout = soc_property_get(unit, spn_TDMA_TIMEOUT_USEC, soc->tableDmaTimeout);        

        if (soc->tableDmaTimeout) {            
            soc->tableDmaMutex = sal_mutex_create("TblDMA");            
            if (soc->tableDmaMutex == NULL) {                  
                SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate tableDmaMutex")));             
            }             
            soc->tableDmaIntr = sal_sem_create("TDMA interrupt", sal_sem_BINARY, 0);            
            if (soc->tableDmaIntr == NULL) {                 
                SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate tableDmaIntr")));            
            }             
            soc->tableDmaIntrEnb = soc_property_get(unit, spn_TDMA_INTR_ENABLE, 0);         
        }      
    }

    /* SLAM Table DMA timeout, interrupt mode and mutex allocation accrding to SOC property configuration */
    soc->tslamDmaMutex = NULL;   
    soc->tslamDmaIntr = NULL;    
    if (soc_property_get(unit, spn_TSLAM_DMA_ENABLE, 1)) {        
        if (SAL_BOOT_QUICKTURN) {             
            soc->tslamDmaTimeout = TSLAM_TIMEOUT_QT;        
        } else {            
            soc->tslamDmaTimeout = TSLAM_TIMEOUT;
        }         
        soc->tslamDmaTimeout = soc_property_get(unit, spn_TSLAM_TIMEOUT_USEC, soc->tslamDmaTimeout);

        if (soc->tslamDmaTimeout) {            
            soc->tslamDmaMutex = sal_mutex_create("TSlamDMA");              
            if (soc->tslamDmaMutex == NULL) {                  
                SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate tslamDmaTimeout")));            
            }             
            soc->tslamDmaIntr = sal_sem_create("TSLAM interrupt", sal_sem_BINARY, 0);
            if (soc->tslamDmaIntr == NULL) {                 
                SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate tslamDmaIntr")));             
            }             
            soc->tslamDmaIntrEnb = soc_property_get(unit, spn_TSLAM_INTR_ENABLE, 0);          
        }     
    } 

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_reset(int unit, int reset_action)
{
    soc_reg_above_64_val_t reg_above_64_val = {0};
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        SHR_BITSET_RANGE(reg_above_64_val, 0, 64);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    }

     if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
         SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
         SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
          
         soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, REG_PORT_ANY, 0, IQC_INITf, 0x0);
         SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

         soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, 0, IPS_INIT_TRIGGERf, 0x0);
         SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

         soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr, REG_PORT_ANY, 0, EGQ_BLOCK_INITf, 0x0);
         SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
     }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_soft_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_FULL);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_soft_ingress_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_INGR);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_soft_egress_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_EGR);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_and_fabric_soft_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_and_fabric_soft_ingress_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_device_blocks_and_fabric_soft_egress_reset(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    soc_sand_rv = arad_dbg_dev_reset(unit, ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_arad_bring_up_interrupts
 * Purpose:
 *      Connect the Arad interrupts.
 * Parameters:
 *      unit -  unit number
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after soc_dpp_common_init
 *
 */
STATIC int
soc_arad_bring_up_interrupts(int unit)
{
    int cmc;

    SOCDNX_INIT_FUNC_DEFS;

    cmc = SOC_PCI_CMC(unit);
     
    /* mask all interrupts in cmicm. This Masking update WB DB, althoght we dont use WB DB. */ 
    soc_cmicm_intr2_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_2_MASK);
    soc_cmicm_intr3_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_3_MASK);
    soc_cmicm_intr4_disable(unit, SOC_DPP_ARAD_INTERRUPTS_CMIC_REGISTER_4_MASK);

    if (!SAL_BOOT_NO_INTERRUPTS) {

        /* Init interrupt DB */
        if (arad_interrupts_array_init(unit) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
        }

        /* connect interrupts / start interrupt thread */
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_connect(unit, soc_cmicm_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
            }
            SOC_CONTROL(unit)->soc_flags |= SOC_F_POLLED;
        } else {
            /* unit # is ISR arg */
            if (soc_cm_interrupt_connect(unit, soc_cmicm_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("could not connect interrupt line")));
            }
        }    

        if (!SOC_WARM_BOOT(unit)) {
            uint32 rval;

            rval = soc_pci_read(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc));
            soc_reg_field_set(unit, CMIC_CMC0_PCIE_MISCELr, &rval, ENABLE_MSIf, 0x1);
            soc_pci_write(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc), rval); 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_arad_init_interrupts_db
 * Purpose:
 *      Initiate the Arad interrupts  arad_sw_db.
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after soc_dpp_common_init
 *
 */
STATIC int
soc_arad_init_interrupts_db(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (!SAL_BOOT_NO_INTERRUPTS) {
        /* Dram interrupt init */
        if(SOC_SAND_FAILURE(arad_dram_init_buffer_error_cntr_unsafe(unit, -1))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("error: arad_dram_init_buffer_error_cntr_unsafe in soc_dpp_arad_init")));;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * Function: 
 *      soc_arad_specific_info_config_before
 * Description: 
 *      configures soc properties specific for Arad which are not dependent on soc properties which are common to both Jericho and Arad
 * Parameters: 
 *      int unit - unit number
 * Return Value: 
 *      SOC_E_XXX
 */
int soc_arad_specific_info_config_direct(int unit)
{
    
    SOCDNX_INIT_FUNC_DEFS;
  
    SOCDNX_IF_ERR_EXIT(soc_arad_info_elk_config(unit)); 
    SOCDNX_IF_ERR_EXIT(soc_arad_ports_info_config(unit));

    exit:
        SOCDNX_FUNC_RETURN;
}

/* 
 * Function: 
 *      soc_arad_specific_info_config_derived
 * Description: 
 *      configures soc properties specific for Arad which are dependent on soc properties which are
 *      either common to both Jericho and Arad or uniqe for Arad
 * Parameters: 
 *      int unit - unit number
 * Return Value: 
 *      SOC_E_XXX
 */
int soc_arad_specific_info_config_derived(int unit) 
{
    int rv;
    soc_dpp_config_arad_t *dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    SOCDNX_INIT_FUNC_DEFS;

    /* OCB */

    /* Validating that the OCB buffer-size is greater than or equal to the DRAM buffer size (only needed on certain cases) */
    if ((dpp_arad->init.ocb.ocb_enable == OCB_DRAM_SEPARATE) || 
        (dpp_arad->init.ocb.ocb_enable == OCB_ONLY_1_DRAM) ||
        ((dpp_arad->init.ocb.ocb_enable == OCB_ENABLED) && (soc_property_get(unit, spn_EXT_RAM_PRESENT, 0) == 0))) {
        if (soc_property_get(unit, spn_EXT_RAM_DBUFF_SIZE, ARAD_ITM_NOF_DBUFF_SIZES) != dpp_arad->init.ocb.databuffer_size) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Dram buffer size has to be equal to Ocb buffer size.")));
        }
    }
    rv = soc_arad_str_prop_mc_nbr_full_dbuff_get(unit, &(dpp_arad->init.dram.fmc_dbuff_mode));
    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      ssoc_dpp_arad_pp_init
 * Purpose:
 *      Initiate the PP DB.
 * Parameters:
 *      unit -  unit number
 * Returns:
 *      SOC_E_XXX
 *
 */
int
soc_arad_pp_init(int unit)
{
    int 
        rv = SOC_E_NONE,
        soc_sand_rv = 0, silent = 0,
        auxiliary_table_mode, port_i,
        vlan;
    uint32 
        ipmc_enable;
    soc_dpp_config_t *dpp = NULL;
    ARAD_PP_INIT_PHASE1_CONF pp_phase1_conf;
    ARAD_PP_INIT_PHASE2_CONF pp_phase2_conf;
    soc_pbmp_t ports_pbmp;

    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);

    /*SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO label_range_info;
     SOC_PPD_EG_ENCAP_RANGE_INFO range_info;
    int num_range;*/
      
    ARAD_PP_INIT_PHASE1_CONF_clear(&pp_phase1_conf);
    ARAD_PP_INIT_PHASE2_CONF_clear(&pp_phase2_conf);
    /*SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO_clear(&label_range_info);
      SOC_PPD_EG_ENCAP_RANGE_INFO_clear(&range_info);*/
    if (!SOC_WARM_BOOT(unit)) {
        auxiliary_table_mode = soc_property_get(unit, spn_BCM886XX_AUXILIARY_TABLE_MODE, 0);
        if (auxiliary_table_mode == 2) {
            /* Enable Mac-in-Mac on device */
            soc_sand_rv = arad_pp_frwrd_bmact_mac_in_mac_enable(unit);
            SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
        }
    }
    /* 
     * initialization of pp_sw_db is done here (warmboot is handled internally
     */ 
    soc_sand_rv = arad_pp_mgmt_operation_mode_set(unit, &dpp->arad->pp_op_mode);
    rv = handle_sand_result(soc_sand_rv);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Set IPMC mode */
    ipmc_enable = soc_property_get(unit, spn_IPMC_ENABLE, 1);
    dpp->pp.ipmc_enable = (ipmc_enable) ? 1:0;


    /* Retrieve MACT learn limit mode SOC property */
    rv = soc_arad_str_prop_parse_mact_learn_limit_mode(unit, &(SOC_PPD_FRWRD_MACT_LEARN_LIMIT_MODE));
    SOCDNX_IF_ERR_EXIT(rv);
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        /* Get the Arad+ specific SOC Properties for MACT Learn Limit - mapping ranges */
        rv = soc_arad_str_prop_parse_mact_learn_limit_range(unit, &(SOC_PPD_FRWRD_MACT_LEARN_LIF_RANGE_BASE(0)));
        SOCDNX_IF_ERR_EXIT(rv);
    }
#endif

    /* lif/rif profile management init */
    rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_profile_mgmt_if_profile_init,(unit)));
    SOCDNX_IF_ERR_EXIT(rv);

    /* 
     * if warmboot, execution stop here.
     */ 
    if (SOC_WARM_BOOT(unit)) {
      SOC_EXIT;
    }      
    soc_sand_rv = arad_pp_mgmt_init_sequence_phase1(unit, &pp_phase1_conf, silent);
    rv = handle_sand_result(soc_sand_rv);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = arad_pp_mgmt_init_sequence_phase2(unit, &pp_phase2_conf, silent);
    rv = handle_sand_result(soc_sand_rv);
    SOCDNX_IF_ERR_EXIT(rv);
      
    /* Init. TCAM for IPV4/IPV6 MC/IPV6 UC.
       enable IPMC and,or IPV6, will result on limited resources for field
        processor */
    {
        uint32 tcam_flags = 0;
        uint32 ipv6_enable;

        /* Set IPMC, IPV6 modes and initialize TCAM configuration accordingly */
        ipv6_enable = soc_property_get(unit, spn_IPV6_ENABLE, 1);

        if (ipmc_enable) {
            tcam_flags |= ARAD_PP_FRWRD_IP_TCAM_IPV4_MC;
        }
        if (ipv6_enable) {
            tcam_flags |= ARAD_PP_FRWRD_IP_TCAM_IPV6_UC;
        }
        if (ipmc_enable && ipv6_enable) {
            tcam_flags |= ARAD_PP_FRWRD_IP_TCAM_IPV6_MC;
        }
        if (tcam_flags != 0) {
            /* Call TCAM init */
            if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(unit)) {                  
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "Error while taking soc_sand chip descriptor mutex")));
                rv = SOC_E_INTERNAL;
                SOCDNX_IF_ERR_EXIT(rv);                        
            }
            soc_sand_rv = arad_pp_frwrd_ip_tcam_init_unsafe(unit,tcam_flags);

            if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit)) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "Error while giving soc_sand chip descriptor mutex")));
                rv = SOC_E_INTERNAL;
                SOCDNX_IF_ERR_EXIT(rv); 
            }    
  
                rv = handle_sand_result(soc_sand_rv);
                SOCDNX_IF_ERR_EXIT(rv);
        }                
    }
    
    /* MPLS init settings */
    {
        uint8 include_inrif;
        uint8 include_port;
        SOC_PPD_MPLS_TERM_LKUP_INFO lkup_info;
        SOC_PPD_FRWRD_ILM_GLBL_INFO glbl_info;

        SOC_PPD_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
        SOC_PPD_MPLS_TERM_LKUP_INFO_clear(&lkup_info);

        rv = soc_dpp_str_prop_parse_mpls_context(unit,&include_inrif,&include_port);
        SOCDNX_IF_ERR_RETURN(rv);

        /* Global key info ILM */
        SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_frwrd_ilm_glbl_info_get(unit, &glbl_info));

        if ((SOC_DPP_CONFIG(unit)->qos.mpls_elsp_label_range_min != -1) &&
             (SOC_DPP_CONFIG(unit)->qos.mpls_elsp_label_range_max != -1)) {

            glbl_info.elsp_info.labels_range.start = SOC_DPP_CONFIG(unit)->qos.mpls_elsp_label_range_min;
            glbl_info.elsp_info.labels_range.end = SOC_DPP_CONFIG(unit)->qos.mpls_elsp_label_range_max;
        }

        glbl_info.key_info.mask_inrif = (!include_inrif);
        glbl_info.key_info.mask_port = (!include_port);

        SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_frwrd_ilm_glbl_info_set(unit, &glbl_info));               
    }    

    /* Extend P2P settings */
    {
        int global_enable = FALSE;
        SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO glbl_info;
        SOC_PPD_PORT_INFO port_info;

        SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_clear(&glbl_info);

        for (port_i = 0; port_i < SOC_MAX_NUM_PORTS ; ++port_i) {
            if (SOC_DPP_CONFIG(unit)->pp.extend_p2p_port_enable[port_i]) {
                global_enable = TRUE;
                glbl_info.enable = TRUE;

                 SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_get(unit,port_i,&port_info));
                 port_info.extend_p2p_info.enable_incoming_extend_p2p = TRUE;
                 port_info.extend_p2p_info.enable_outgoing_extend_p2p = TRUE;
                 SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_set(unit,port_i,&port_info));
             }           
         }
             
         if (global_enable) {
             glbl_info.pwe_info.system_vsi = ARAD_PP_VSI_ID_MAX; 
             SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_frwrd_extend_p2p_glbl_info_set(unit,&glbl_info));
         }
    }

    {   
        SOC_PPD_PORT_INFO port_info;
        uint32 init_vid_enabled = 0;
      /* Pon port vlan translation profile settings */ 
        if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
           
            for (port_i = 0; port_i < SOC_MAX_NUM_PORTS ; ++port_i) {   

                if (IS_PON_PORT(unit, port_i)) {   
                    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_get(unit,port_i,&port_info));   
                    port_info.vlan_translation_profile = SOC_PPD_PORT_DEFINED_VT_PROFILE_PON_DEFAULT;   
                    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_set(unit,port_i,&port_info));   
                }

                if (IS_E_PORT(unit, port_i)) {
                    /* 
                     * In case of PON application, reglardless of Tunnel-ID / VLANs, 
                     * always keep outer tag and always be in membership
                     */
                    for (vlan = 0; vlan < 4096; vlan++) {   
                        SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(unit,port_i,vlan,TRUE));   
                    }
                }
            }   
        }

        /* Port vlan translation profile Initial-VID settings */
        if (SOC_DPP_CONFIG(unit)->pp.port_use_initial_vlan_only_enabled) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE | SOC_PORT_FLAGS_INIT_VID_ONLY, &ports_pbmp));
            
            SOC_PBMP_ITER(ports_pbmp, port_i) {
                rv = soc_arad_str_prop_parse_init_vid_enabled_port_get(unit, port_i, &init_vid_enabled);
                if (rv != SOC_E_NONE) {
                   LOG_ERROR(BSL_LS_SOC_INIT,
                             (BSL_META_U(unit,
                                         "soc_arad_str_prop_parse_init_vid_enabled_port_get error in soc_arad_info_config\n")));
                }
                /* In this port, support use Initial-VID with no differencebetween 
                 * untagged packets and tagged packets. so set port vlan tranlation file as
                 * SOC_PPD_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID.
                 */
                if (!init_vid_enabled) {   
                    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_get(unit,port_i,&port_info));   
                    port_info.vlan_translation_profile = SOC_PPD_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID;   
                    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_set(unit,port_i,&port_info));   
                }
            }   
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* dummy warmboot callbacks to be used if WArmboot is not initialized */
#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_dpp_read_dummy_func(int unit, uint8 *buf, int offset, int nbytes)
{
    return SOC_E_RESOURCE;
}

int
soc_dpp_write_dummy_func(int unit, uint8 *buf, int offset, int nbytes)
{
    return SOC_E_RESOURCE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
void
soc_arad_init_empty_scache(int unit)
{
    int     stable_location = 0;
    uint32  stable_flags = 0;
    int     stable_size = 0;

    if (soc_scache_is_config(unit)!=SOC_E_NONE) {
       /* EMPTY SCACHE INITIALIZATION ->
          in case stable_* parameters are not defined in configuration file, 
          initiating scache with size 0(zero). in order that scache commits 
          wont fail and cause application exit upon startup */
       if (soc_switch_stable_register(unit,
                                      &soc_dpp_read_dummy_func,
                                      &soc_dpp_write_dummy_func,
                                      NULL, NULL) < 0) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "soc_switch_stable_register failure.\n")));
       }

       if (soc_stable_set(unit, stable_location, stable_flags) < 0) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "soc_stable_set failure\n")));
       } else if (soc_stable_size_set(unit, stable_size) < 0) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "soc_stable_size_set failure\n")));
       }
       /* <- EMPTY SCACHE INITIALIZATION */
    }
}
#endif /* BCM_WARM_BOOT_SUPPORT */

STATIC int soc_arad_rcpu_base_q_pair_init(int unit, int port_i)
{
    uint32 base_q_pair = 0, rval = 0;
    
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port_i, &base_q_pair));

    if (base_q_pair < 32) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_0r(unit, &rval));
        rval |= 0x1 << base_q_pair;
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_0r(unit, rval));
    } else if (base_q_pair < 64) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_1r(unit, &rval));
        rval |= 0x1 << (base_q_pair - 32);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_1r(unit, rval));
    } else if (base_q_pair < 96) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PKT_PORTS_2r(unit, &rval));
        rval |= 0x1 << (base_q_pair - 64);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_2r(unit, rval));
    } else if (base_q_pair == 96) 
    {
        rval = 0x1;
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_PORTS_3r(unit, rval));
    } else 
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error: RCPU base_q_pair range is 0 - 96\n")) );
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }              
    exit:
         SOCDNX_FUNC_RETURN;
}

STATIC int soc_arad_rcpu_init(int unit, soc_dpp_config_t *dpp)
{
    int port_i = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(dpp->arad->init.rcpu.slave_port_pbmp, port_i) 
    {
        SOCDNX_IF_ERR_EXIT(soc_arad_rcpu_base_q_pair_init(unit, port_i));
    }

    exit:
         SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_dpp_arad_init
 * Purpose:
 *      Optionally reset, and initialize a StrataSwitch.
 * Parameters:
 *      unit -  unit number
 *      reset - Boolean, if TRUE, device is reset.
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 *
 */
int
soc_arad_init(int unit, int reset_action)
{
    int rv = SOC_E_NONE;     
    soc_dpp_config_t *dpp = NULL;
    int soc_sand_rv = 0, silent = 0;
    int port_i, core;
    SOC_TMC_PORT_HEADER_TYPE header_type_in;
    soc_pbmp_t pbmp;
    uint32 disable_hard_reset, tm_port;

    uint32 perform_iqm_workaround;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    soc_arad_init_empty_scache(unit);
#endif

    dpp = SOC_DPP_CONFIG(unit);

    if (dpp == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("SOC_DPP_CONFIG() is not allocated.")));
    }
    
   if (dpp->arad == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("dpp->arad is not allocated.")));
    }

    if (SOC_IS_ARADPLUS(unit) && (dpp->arad_plus == NULL)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("dpp->arad_plus is not allocated.")));
    }

    /* Fabric ports- static configuration */
    SOC_INFO(unit).fabric_logical_port_base = SOC_DPP_FABRIC_LOGICAL_PORT_BASE_DEFAULT;


    /* Set device configuration */
    SOCDNX_IF_ERR_EXIT(soc_dpp_device_specific_info_config_direct(unit));

    SOCDNX_IF_ERR_EXIT(soc_arad_info_config(unit)); 

    SOCDNX_IF_ERR_EXIT(soc_dpp_device_specific_info_config_derived(unit));

    /*
     * Reset device.
     * No access allowed before this stage. 
     *  
     */
    disable_hard_reset = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_without_device_hard_reset", 0); 
    if ((!SOC_WARM_BOOT(unit)) && (disable_hard_reset ==0 ))
    {
        /* Arad CPS Reset */
        
        rv = soc_arad_device_hard_reset(unit, reset_action);
        SOCDNX_IF_ERR_EXIT(rv);
        
        if (reset_action == SOC_DPP_RESET_ACTION_IN_RESET) {
            SOC_EXIT;
        }
    }

    /*
     * Bring up interrupts.  
     */
    rv = soc_arad_bring_up_interrupts(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    if (!SOC_WARM_BOOT(unit))
    {  
       if (reset_action != SOC_DPP_RESET_ACTION_OUT_RESET) {
            /*Reset*/
            rv = soc_arad_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_IN_RESET);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    /* 
     *  Attach/Init DMA
     */ 

    soc_dcb_unit_init(unit);

    if ((rv = soc_dma_attach(unit, 1)) < 0) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_attach: soc_dma_attach returns error.\n")));
        return SOC_E_INTERNAL;
    }

    /* init cmic_pcie_userif_purge_ctrl */
    rv = soc_dcmn_cmic_pcie_userif_purge_ctrl_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    /* init mutexes for DMA*/
    rv = soc_arad_dma_mutex_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

#ifdef BCM_SBUSDMA_SUPPORT    
    rv = soc_sbusdma_desc_init(unit, 0, 0);
    SOCDNX_IF_ERR_EXIT(rv);
#endif
    SOCDNX_IF_ERR_EXIT(dcmn_init_fill_table(unit));

    /* 
     * Common device init 
     */
    rv = soc_dpp_common_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);
  
    rv = soc_arad_init_interrupts_db(unit);
    SOCDNX_IF_ERR_EXIT(rv);    
    
    /*
     * Initialize SOC link control module
     */
    SOCDNX_IF_ERR_RETURN(soc_linkctrl_init(unit, &soc_linkctrl_driver_arad));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
      SOCDNX_IF_ERR_EXIT(dpp_mcds_multicast_init2(unit));
    }
#endif
    if (!SOC_WARM_BOOT(unit)) {
        /* Init phase 1 */
        soc_sand_rv = arad_mgmt_init_sequence_phase1(unit, &(dpp->arad->init), silent);
        rv = handle_sand_result(soc_sand_rv);
        SOCDNX_IF_ERR_EXIT(rv);

        /* Common TM init */
        rv = soc_dpp_common_tm_init(unit, &(dpp->tm.multicast_egress_bitmap_group_range));
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else {
        /* only reinit operation mode */
        arad_sw_db_tdm_mode_set(
          unit,
          dpp->arad->init.tdm_mode
        );  

        arad_sw_db_ilkn_tdm_dedicated_queuing_set(
          unit,
          dpp->arad->init.ilkn_tdm_dedicated_queuing
        );

        arad_sw_db_is_petrab_in_system_set(
          unit,
          dpp->arad->init.is_petrab_in_system
        );
    }

    /* 
     * pp init 
     */
    if (dpp->arad->init.pp_enable) {
        rv = soc_arad_pp_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Arad-only SOC properties */

    /* no need to reinit TDM during warm reboot. */
    if (!SOC_WARM_BOOT(unit)) {
        /*
         * Initialize TDM applications.
         */
        if (dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT
            || dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA) {
            /* Set default ftmh for each relevant tm port */
            ARAD_TDM_FTMH_INFO ftmh_info;

            ARAD_TDM_FTMH_INFO_clear(&ftmh_info);
            ftmh_info.action_ing = ARAD_TDM_ING_ACTION_ADD;
            ftmh_info.action_eg = ARAD_TDM_EG_ACTION_REMOVE;

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

            SOC_PBMP_ITER(pbmp, port_i) {
              if (port_i != SOC_DPP_PORT_INTERNAL_ERP(0)) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_in_get(unit, port_i, &header_type_in));
                if (header_type_in == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
                  soc_sand_rv = arad_tdm_ftmh_set(unit, 
                                                  tm_port,
                                                  &ftmh_info);
                  rv = handle_sand_result(soc_sand_rv);
                  SOCDNX_IF_ERR_EXIT(rv);
                }
              }
            }
        }

        /* TDM Optimize init */
        if (dpp->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT) {
            /* cell size in Optimize mode. */
            soc_sand_rv = arad_tdm_opt_size_set(
                unit,
                SOC_DPP_DRV_TDM_OPT_SIZE
            );
            rv = handle_sand_result(soc_sand_rv);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        /* TDM Always High Priority Scheduling { */
        {
            uint32 is_valid;
            int core;
            SOC_TMC_EGR_OFP_SCH_INFO ofp_sch_info;
            SOC_TMC_EGR_OFP_SCH_INFO_clear(&ofp_sch_info);

            for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
                /* Invalid port */
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
                if (!is_valid) {
                    continue;
                }

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));

                if (IS_TDM_PORT(unit, port_i)) {
                    soc_sand_rv = arad_egr_ofp_scheduling_get(unit, tm_port, core, &ofp_sch_info);
                    rv = handle_sand_result(soc_sand_rv);
                    SOCDNX_IF_ERR_EXIT(rv);

                    ofp_sch_info.nif_priority = SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH;

                    soc_sand_rv = arad_egr_ofp_scheduling_set(unit, tm_port, core, &ofp_sch_info);
                    rv = handle_sand_result(soc_sand_rv);
                    SOCDNX_IF_ERR_EXIT(rv);
                }
            }
        }
        /* TDM High Low Priority ILKN } */

        /* Init RCPU */
        SOCDNX_IF_ERR_EXIT(soc_arad_rcpu_init(unit, dpp));

        perform_iqm_workaround = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "8865001", 0);
        if (perform_iqm_workaround) {
            rv = arad_iqm_workaround(unit);
            SOCDNX_SAND_IF_ERR_RETURN(rv);
        }

    }

    /* initialize information for fast register and field access */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    if (!SOC_IS_ARDON(unit)) {
        arad_fast_regs_and_fields_access_init(unit);
    }
#endif /* ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS */

    /* need to be at the end of soc_init in order to prevent ecc errors from un initialized memories */
    rv = arad_ser_init(unit);
    SOCDNX_SAND_IF_ERR_RETURN(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_arad_init_reg_access(int unit, int reset)
{
      
  int rv = SOC_E_NONE;
  uint32 reg32_val;

  SOCDNX_INIT_FUNC_DEFS;

  rv = soc_arad_device_hard_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET);
  SOCDNX_IF_ERR_EXIT(rv);

  /*Reset*/
  rv = soc_arad_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET);
  SOCDNX_IF_ERR_EXIT(rv);

  if (!SOC_IS_ARDON(unit)) {
      reg32_val = 0;
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, CMAC_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_BYPASS_OSTSf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_LINE_LPBK_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_BYPASS_OSTSf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_LINE_LPBK_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_BYPASS_OSTSf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_LINE_LPBK_RESETf, 0);
      soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_RESETf, 0);     
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) /*###CLP0*/, reg32_val));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)) /*###CLP0*/, reg32_val));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)) /*###XLP1*/, reg32_val));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)) /*###XLP1*/, reg32_val));

      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) /*###CLP0*/, 0xfff));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)) /*###CLP0*/, 0xfff));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)) /*###XLP1*/, 0xfff));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)) /*###XLP1*/, 0xfff));
       
      sal_usleep(1000);
       
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) /*###CLP0*/, 0));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)) /*###CLP0*/, 0));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)) /*###XLP1*/, 0));
      SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)) /*###XLP1*/, 0));
  }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_arad_init_enable_traffic(int unit, int reset_action)
{
    int soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    if (reset_action == SOC_DPP_RESET_ACTION_IN_RESET) {
        soc_sand_rv = arad_mgmt_enable_traffic_set(unit, FALSE);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        soc_sand_rv = arad_mgmt_enable_traffic_set(unit, TRUE);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_device_reset(int unit, int mode, int action)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Arad function. invalid Device")));
    }

    switch (mode) {
    case SOC_DPP_RESET_MODE_HARD_RESET:
        rv = soc_arad_device_hard_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_RESET:
        rv = soc_arad_device_blocks_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET:
        rv = soc_arad_device_blocks_soft_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET:
        rv = soc_arad_device_blocks_soft_ingress_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET:
        rv = soc_arad_device_blocks_soft_egress_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET:
        rv = soc_arad_device_blocks_and_fabric_soft_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET:
        rv = soc_arad_device_blocks_and_fabric_soft_ingress_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET:
        rv = soc_arad_device_blocks_and_fabric_soft_egress_reset(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_INIT_RESET:
        rv = soc_arad_init(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_REG_ACCESS:
        rv = soc_arad_init_reg_access(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;
    case SOC_DPP_RESET_MODE_ENABLE_TRAFFIC:
        rv = soc_arad_init_enable_traffic(unit, action);
        SOCDNX_IF_ERR_RETURN(rv);
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown/Unsupported Reset Mode")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_arad_reinit(int unit, int reset)
{
     int rv = SOC_E_NONE;
    uint32 
        reg32_val; 

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_arad_device_hard_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rv);

     /*Reset*/
     rv = soc_arad_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET);
     SOCDNX_IF_ERR_EXIT(rv);

     if (SOC_PORT_NUM(unit,port) > 0) {
         reg32_val = 0;
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, CMAC_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_BYPASS_OSTSf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_LINE_LPBK_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_0_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_BYPASS_OSTSf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_LINE_LPBK_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_1_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_BYPASS_OSTSf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_LINE_LPBK_RESETf, 0);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg32_val, XMAC_2_RESETf, 0);     
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) /*###CLP0*/, reg32_val));
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)) /*###CLP0*/, reg32_val));
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)) /*###XLP1*/, reg32_val));
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)) /*###XLP1*/, reg32_val));
     }

 exit: 
    SOCDNX_FUNC_RETURN;
}

typedef struct {
    uint32 if_first_port; 
    uint32 is_valid; 
    uint32 sch_total_if_rate; 
} arad_ports_rate_t; 

typedef struct {
    uint32 chan_arb_first_port; 
    uint32 is_valid;
    uint32 egq_total_if_rate;
} arad_chan_arbs_rate_t;

int
soc_arad_default_ofp_rates_set(int unit) {
    uint32 phy_port;    
    int soc_sand_rv = 0;
    uint32 tm_port;
    int rv = SOC_E_NONE;
    uint32 ndx_max = ARAD_NIF_NOF_NIFS + 5; /* + CPU,ERP,OLP,RCY,OAM */
    arad_ports_rate_t if_ids_first_ports[ARAD_NIF_NOF_NIFS + 5];
    arad_chan_arbs_rate_t chan_arbs_first_ports[ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB];
    int first_port = -1, core;
    int priority_i, tcg_i;
    uint32 nof_priorities;
    uint32 rate, if_rate_mbps;
    uint32 if_id_internal, ndx;
    uint32 nof_nifs = ARAD_NIF_NOF_NIFS;
    SOC_TMC_EGR_QUEUING_TCG_INFO egq_tcg_info;
    SOC_TMC_SCH_PORT_INFO sch_tcg_info;
    uint32 nof_channels;
    soc_pbmp_t ports_bm;
    soc_port_t port_i;
    soc_port_if_t interface;
    uint32 flags;
    ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id;
    uint32 tcg_egq_rate_arr[ARAD_NOF_TCGS];
    uint32 tcg_sch_rate_arr[ARAD_NOF_TCGS];
    uint32 sch_rate; 
    uint32 egq_rate;

    SOCDNX_INIT_FUNC_DEFS;

    if(unit < 0 || unit >= SOC_MAX_NUM_DEVICES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG_STR( "ERROR: invalid unit")));
    }

    SOC_TMC_EGR_QUEUING_TCG_INFO_clear(&egq_tcg_info);
    SOC_TMC_SCH_PORT_INFO_clear(&sch_tcg_info);
    
    /*Clear arrays*/
    sal_memset(if_ids_first_ports,0,sizeof(arad_ports_rate_t)*(ndx_max));
    sal_memset(chan_arbs_first_ports,0,sizeof(arad_chan_arbs_rate_t)*ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB);
    sal_memset(tcg_egq_rate_arr,0,sizeof(uint32)*ARAD_NOF_TCGS);
    sal_memset(tcg_sch_rate_arr,0,sizeof(uint32)*ARAD_NOF_TCGS);


    /* Optimization - update device FALSE */
    
    rv = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
    if (rv != SOC_E_NONE)
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm valid ports bitmap failed")));
    }

    SOC_PBMP_ITER(ports_bm, port_i) {
      rv =  soc_port_sw_db_flags_get(unit, port_i, &flags);
      if (rv != SOC_E_NONE)
      {
       SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive port flags failed")));
      }
      if(SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_VIRTUAL_RCY_INTERFACE(flags)) {
        continue;
      }
      
      /* Get interface rate, nof_priorities */
      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &nof_priorities));

      rv = soc_port_sw_db_interface_rate_get(unit, port_i, &if_rate_mbps);
      if (rv != SOC_E_NONE)
      {
         SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm local port interface rate %d failed"), port_i));
      }
   
      /* rates are in kbps */
      rate = if_rate_mbps * ARAD_RATE_1K;

      rv = soc_port_sw_db_num_of_channels_get(unit, port_i, &nof_channels);
      if (rv != SOC_E_NONE)
      {
         SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm local port number of channels %d failed"), port_i));
      }

      /* Get nof channels */
      rate = rate / nof_channels;

      sch_rate = SOC_SAND_MIN(SOC_TMC_RATE_1K * ((SOC_TMC_IF_MAX_RATE_MBPS_ARAD * 9)/10),(rate + (rate / 100) * 5)); /* 5% speedup */
      egq_rate = SOC_SAND_MIN(SOC_TMC_RATE_1K * ((SOC_TMC_IF_MAX_RATE_MBPS_ARAD * 9)/10),(rate + (rate / 100) * 1)); /* 1% speedup */

      rv = soc_port_sw_db_interface_type_get(unit, port_i, &interface);
      if (rv != SOC_E_NONE)
      {
         SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm local port interface type %d failed"), port_i));
      }

      /* Get internal nif index */
      switch (interface)
      {
      case SOC_PORT_IF_CPU:
        ndx = nof_nifs+0;
        break;
      case SOC_PORT_IF_ERP:
        ndx = nof_nifs+1;
        break;
      case SOC_PORT_IF_OLP:
        ndx = nof_nifs+2;
        break;
      case SOC_PORT_IF_RCY:
        ndx = nof_nifs+3;
        break;
      case SOC_PORT_IF_OAMP:
        ndx = nof_nifs+4;
        break;
      default:
        rv = soc_port_sw_db_first_phy_port_get(unit, port_i, &phy_port);
        if (rv != SOC_E_NONE)
        {
         SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: retreive bcm local port phy port %d failed"), port_i));
        }
        if_id_internal = phy_port-1;       
        ndx = if_id_internal;
      }
      
      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core)); 

      if (!if_ids_first_ports[ndx].is_valid) {
          if_ids_first_ports[ndx].is_valid = TRUE;
          if_ids_first_ports[ndx].if_first_port = tm_port;
          if_ids_first_ports[ndx].sch_total_if_rate = 0;
      }

      /*Get port's chan_arb*/
      rv = arad_ofp_rates_port2chan_arb(unit,tm_port,&chan_arb_id);
      if (SOC_FAILURE(rv)) {
          SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: can't get chan_arb_id in ofp_rates setting port %d"), port_i));
      }

      /*Save port for later rate setting to hw*/
      if (!chan_arbs_first_ports[chan_arb_id].is_valid) {
          chan_arbs_first_ports[chan_arb_id].is_valid = TRUE;
          chan_arbs_first_ports[chan_arb_id].egq_total_if_rate = 0;
          chan_arbs_first_ports[chan_arb_id].chan_arb_first_port = tm_port;
          first_port = tm_port;
      }

      if_ids_first_ports[ndx].sch_total_if_rate += sch_rate;
      chan_arbs_first_ports[chan_arb_id].egq_total_if_rate += egq_rate;

      /* 
       *Set port sch and egq rates to sw_db
       *bound sch and egq rates to 90% of max val, 
       *because calender accurate value might be larger than requested
       */
      /*sch*/
      /*doron TBD - define tool*/
      #define _DPP_COSQ_EGR_SCH_RATE_MAX_KBITS_SEC_PER_DEVICE(unit)  SOC_TMC_SCH_MAX_RATE_KBPS_ARAD

      if (sch_rate > _DPP_COSQ_EGR_SCH_RATE_MAX_KBITS_SEC_PER_DEVICE(unit)) {
          sch_rate = _DPP_COSQ_EGR_SCH_RATE_MAX_KBITS_SEC_PER_DEVICE(unit); /*reduce to maximum if above*/
      }    
      rv = arad_ofp_rates_sch_single_port_rate_sw_set(unit, tm_port, 0, sch_rate);
      if(SOC_FAILURE(rv)) {
         SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set port sch rate for port %d"), tm_port));
      }
                
      /*egq*/
      rv = arad_ofp_rates_egq_single_port_rate_sw_set(unit,tm_port,egq_rate);
      if(SOC_FAILURE(rv)) {
         SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set port egq rate for port %d"), tm_port));
      }
      /* QPAIR shapers rate is devided equally between queues. Sum of port rate */
      for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
      {
         /*egq*/
         rv = arad_ofp_rates_egq_port_priority_rate_sw_set(unit,tm_port,priority_i,egq_rate);
         if(SOC_FAILURE(rv)) {
             SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq port_priority rate for port %d priority %d"), tm_port, priority_i));
         }
         /*sch*/
         rv =  arad_ofp_rates_sch_port_priority_rate_set(unit,tm_port,priority_i,sch_rate);
         if(SOC_FAILURE(rv)) {
             SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set sch port_priority rate for port %d priority %d"), tm_port, priority_i));
         }
      }    

      /* TCG shapers - only in case of eight priorities. Requires sum of all queues that point to required tcg */
      if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
      {
        soc_sand_rv = arad_egr_queuing_ofp_tcg_get(unit, tm_port, &egq_tcg_info);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* E2E TCG info */
        soc_sand_rv = arad_sch_port_sched_get(unit, tm_port, &sch_tcg_info);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

        for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
        {
          tcg_egq_rate_arr[egq_tcg_info.tcg_ndx[priority_i]] = egq_rate;
          tcg_sch_rate_arr[sch_tcg_info.tcg_ndx[priority_i]] = sch_rate;       
        }

        /* Set TCG shaper */
        for (tcg_i = 0; tcg_i < SOC_TMC_NOF_TCGS; ++tcg_i)
        {          
            /*egq*/
            rv =  arad_ofp_rates_egq_tcg_rate_sw_set(unit,tm_port,tcg_i,tcg_egq_rate_arr[tcg_i]);
            if(SOC_FAILURE(rv)) {
             SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq tcg rate for port %d tcg %d"), tm_port, tcg_i));
            }
            /*sch*/
            rv =  arad_ofp_rates_sch_tcg_rate_set(unit,tm_port,tcg_i,tcg_sch_rate_arr[tcg_i]);
            if(SOC_FAILURE(rv)) {
             SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set sch tcg rate for port %d tcg %d"), tm_port, tcg_i));
            }        
        }
      }       
    }

    /*Set egq port_priority rates to hw*/
    rv =  arad_ofp_rates_egq_port_priority_rate_hw_set(unit);
    if(SOC_FAILURE(rv)) {
     SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq port_priority rate to hw")));
    }
    /*Set egq tcg rates to hw*/
    rv =  arad_ofp_rates_egq_tcg_rate_hw_set(unit);
    if(SOC_FAILURE(rv)) {
     SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq tcg rate to hw")));
    }

    /*Set device to update all chan_arbs*/
    soc_sand_rv  = arad_sw_db_ofp_rates_update_dev_changed_set(unit,TRUE);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    /*Set single ports rates to hw*/
    for (ndx = 0; ndx < ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB; ++ndx) {
        if(chan_arbs_first_ports[ndx].is_valid){
            /*sch*/
            rv = arad_ofp_rates_sch_single_port_rate_hw_set(unit, chan_arbs_first_ports[ndx].chan_arb_first_port, 0);
            if(SOC_FAILURE(rv)) {
             SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set port sch rate for port %d"), chan_arbs_first_ports[ndx].chan_arb_first_port));
            }
        }
    }
    /*egq*/
    rv = arad_ofp_rates_egq_single_port_rate_hw_set(unit,first_port);
    if(SOC_FAILURE(rv)) {
     SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set port egq rate for port %d"), first_port));
    }

    /*Set device to update only changed chan_arbs*/
    soc_sand_rv  = arad_sw_db_ofp_rates_update_dev_changed_set(unit,FALSE);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    /*Set interface shapers*/
    for (ndx = 0; ndx < ndx_max; ++ndx) {
      if (if_ids_first_ports[ndx].is_valid) {
          rv = arad_sch_if_shaper_rate_set(unit, if_ids_first_ports[ndx].if_first_port, 0, if_ids_first_ports[ndx].sch_total_if_rate);
          if(SOC_FAILURE(rv)) {
             SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set sch interface shaper for port %d"), if_ids_first_ports[ndx].if_first_port));
            }  
      }
    }
    for (chan_arb_id = 0; chan_arb_id < ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB; ++chan_arb_id) {
      if (chan_arbs_first_ports[chan_arb_id].is_valid) {
          rv = arad_ofp_rates_egq_interface_shaper_set(unit,chan_arbs_first_ports[chan_arb_id].chan_arb_first_port, SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE, chan_arbs_first_ports[chan_arb_id].egq_total_if_rate);
          if(SOC_FAILURE(rv)) {
             SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG_STR( "ERROR: failed to set egq interface shaper for port %d"), chan_arbs_first_ports[chan_arb_id].chan_arb_first_port));
          } 
      }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_arad_ilkn_config_get(int unit, uint32 protocol_offset)
{
  ARAD_PORTS_ILKN_CONFIG *ilkn_config;
  SOCDNX_INIT_FUNC_DEFS;

  ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

  sal_memset(ilkn_config, 0, sizeof(ARAD_PORTS_ILKN_CONFIG));

  

  ilkn_config->metaframe_sync_period = soc_property_port_get(unit, protocol_offset, spn_ILKN_METAFRAME_SYNC_PERIOD, 2048);
  ilkn_config->interfcae_status_ignore = soc_property_port_get(unit, protocol_offset, spn_ILKN_INTERFACE_STATUS_IGNORE, 0);
  ilkn_config->interfcae_status_oob_ignore = soc_property_port_get(unit, protocol_offset, spn_ILKN_INTERFACE_STATUS_OOB_IGNORE, 0);

  /* Interlaken ReTransmit attributes */
  ilkn_config->retransmit.enable_rx = soc_property_port_get(unit,protocol_offset, spn_ILKN_RETRANSMIT_ENABLE_RX, FALSE);
  ilkn_config->retransmit.enable_tx = soc_property_port_get(unit, protocol_offset, spn_ILKN_RETRANSMIT_ENABLE_TX, FALSE);
  ilkn_config->retransmit.buffer_size_entries = soc_property_get(unit, spn_ILKN_RETRANSMIT_BUFFER_SIZE, 255);
  ilkn_config->retransmit.nof_requests_resent = soc_property_get(unit, spn_ILKN_RETRANSMIT_NUM_REQUESTS_RESENT, 15);
  ilkn_config->retransmit.nof_seq_number_repetitions_tx = soc_property_get(unit, spn_ILKN_RETRANSMIT_NUM_SN_REPETITIONS_TX, 1);
  ilkn_config->retransmit.nof_seq_number_repetitions_rx = soc_property_get(unit, spn_ILKN_RETRANSMIT_NUM_SN_REPETITIONS_RX, 1);
  ilkn_config->retransmit.rx_timeout_words =soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_TIMEOUT_WORDS, 0);
  ilkn_config->retransmit.rx_timeout_sn =soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_TIMEOUT_SN, 0);
  ilkn_config->retransmit.rx_ignore = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_IGNORE, 0);
  ilkn_config->retransmit.rx_watchdog = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_WATCHDOG, 0);
  ilkn_config->retransmit.rx_reset_when_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_ERROR_ENABLE, 1);    
  ilkn_config->retransmit.rx_reset_when_retry_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_RETRY_ERROR_ENABLE, 1);
  ilkn_config->retransmit.rx_reset_when_alligned_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_ALLIGNED_ERROR_ENABLE, 1);
  ilkn_config->retransmit.rx_reset_when_timout_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_TIMOUT_ERROR_ENABLE, 0);
  ilkn_config->retransmit.rx_reset_when_wrap_after_disc_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_WRAP_AFTER_DISC_ERROR_ENABLE, 1);
  ilkn_config->retransmit.rx_reset_when_wrap_before_disc_error = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_WHEN_WRAP_BEFORE_DISC_ERROR_ENABLE, 0);
  ilkn_config->retransmit.rx_reset_when_watchdog_err = soc_property_get(unit, spn_ILKN_RETRANSMIT_RX_RESET_UPON_WATCHDOG_ERROR_ENABLE, 1);

  ilkn_config->retransmit.tx_ignore_requests_when_fifo_almost_empty = soc_property_get(unit, spn_ILKN_RETRANSMIT_TX_IGNORE_REQUESTS_WHEN_FIFO_ALMOST_EMPTY, 1);
  ilkn_config->retransmit.tx_wait_for_seq_num_change = soc_property_get(unit, spn_ILKN_RETRANSMIT_TX_WAIT_FOR_SEQ_NUM_CHANGE_ENABLE, 1);
  
  SOCDNX_FUNC_RETURN;
}


int
soc_arad_caui_config_get(int unit, uint32 protocol_offset)
{
  ARAD_PORTS_CAUI_CONFIG *caui_config;

  SOCDNX_INIT_FUNC_DEFS;

  caui_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.caui[protocol_offset];

  sal_memset(caui_config, 0, sizeof(ARAD_PORTS_CAUI_CONFIG));

  caui_config->rx_recovery_lane = 0;
#ifdef BCM_88660_A0
  if(SOC_IS_ARADPLUS(unit)){
    caui_config->rx_recovery_lane = soc_property_port_get(unit, (protocol_offset == 0) ? 0 : 1, spn_CAUI_RX_CLOCK_RECOVERY_LANE, 0);
    if(caui_config->rx_recovery_lane > 3){
      SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid option for rx recovery lane %d"), caui_config->rx_recovery_lane));
    }
  }
#endif /* BCM_88660_A0 */

exit:
  SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

