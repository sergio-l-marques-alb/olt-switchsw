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
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <soc/linkctrl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_map.h>
#include <soc/dpp/soc_sw_db.h>
#include <soc/dpp/Petra/petra_link.h>
#include <soc/dpp/Petra/petra_serdes_utils.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_api_tdm.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_bmact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ip_tcam.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/PPD/ppd_api_frwrd_extend_p2p.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#ifdef PLISIM
#include <../systems/sim/dpp/ChipSim/chip_sim.h>
#endif /* PLISIM */

#define SOC_PETRA_DRAM_MODE_REG_SET(field, prop) \
      val = soc_property_port_get(unit, lane_i, prop, prop_invalid); \
      if (val != prop_invalid) { \
        field = val; \
        dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.auto_mode = FALSE; \
      }


int
soc_petra_default_config_tm_get(int unit)
{
  int i, core;

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

    SOC_DPP_CONFIG(unit)->tm.max_ses = SOC_TMC_SCH_MAX_SE_ID_PETRA + 1;  
    SOC_DPP_CONFIG(unit)->tm.cl_se_min = SOC_TMC_CL_SE_ID_MIN_PETRA; 
    SOC_DPP_CONFIG(unit)->tm.cl_se_max = SOC_TMC_CL_SE_ID_MAX_PETRA;
    SOC_DPP_CONFIG(unit)->tm.fq_se_min = SOC_TMC_FQ_SE_ID_MIN_PETRA;
    SOC_DPP_CONFIG(unit)->tm.fq_se_max = SOC_TMC_FQ_SE_ID_MAX_PETRA;
    SOC_DPP_CONFIG(unit)->tm.hr_se_min = SOC_TMC_HR_SE_ID_MIN_PETRA;
    SOC_DPP_CONFIG(unit)->tm.hr_se_max = SOC_TMC_HR_SE_ID_MAX_PETRA;
    SOC_DPP_CONFIG(unit)->tm.port_hr_se_min = SOC_TMC_HR_SE_ID_MIN_PETRA;
    SOC_DPP_CONFIG(unit)->tm.port_hr_se_max = SOC_TMC_HR_SE_ID_MIN_PETRA + SOC_TMC_SCH_MAX_PORT_ID_PETRA;

    SOC_DPP_CONFIG(unit)->tm.max_connectors = SOC_TMC_SCH_MAX_FLOW_ID_PETRA + 1;  
    SOC_DPP_CONFIG(unit)->tm.max_egr_q_prio = SOC_TMC_EGR_NOF_Q_PRIO_PB;
    SOC_DPP_CONFIG(unit)->tm.invalid_port_id_num = SOC_TMC_SCH_PORT_ID_INVALID_PETRA;
    SOC_DPP_CONFIG(unit)->tm.invalid_se_id_num = SOC_TMC_SCH_SE_ID_INVALID_PETRA;
    SOC_DPP_CONFIG(unit)->tm.invalid_voq_connector_id_num = SOC_TMC_SCH_FLOW_ID_INVALID_PETRA;
    SOC_DPP_CONFIG(unit)->tm.nof_vsq_category = SOC_TMC_ITM_VSQ_GROUP_LAST_PETRA;
    SOC_DPP_CONFIG(unit)->tm.is_port_tc_enable = FALSE;
    SOC_DPP_CONFIG(unit)->tm.nof_mc_ids = SOC_PETRAB_MULTICAST_TABLE_MODE;
    SOC_DPP_CONFIG(unit)->tm.mc_mode = 0;
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue = 0;
    SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud =
    SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud = SOC_PETRAB_MAX_CUD;
    for(core=0 ; core<SOC_DPP_DEFS_MAX(NOF_CORES) ; core++) {
        SOC_DPP_CONFIG(unit)->tm.hr_isq[core] = SOC_TMC_SCH_PORT_ID_INVALID_PETRA; /* PORT ISQ */
        for (i = 0; i < SOC_TMC_MULT_FABRIC_NOF_CLS; i++) {
            SOC_DPP_CONFIG(unit)->tm.hr_fmqs[core][i] = SOC_TMC_SCH_PORT_ID_INVALID_PETRA; /* PORT FMQ */
        }    
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_default_config_tdm_get(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits = 32;  
    SOC_DPP_CONFIG(unit)->tdm.min_cell_size = 65;
    SOC_DPP_CONFIG(unit)->tdm.max_cell_size = 128;
    SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size = 1;
    SOC_DPP_CONFIG(unit)->tdm.is_bypass = 0;        

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_default_config_qos_get(int unit)
{
    soc_dpp_config_qos_t *dpp_qos;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_qos = &(SOC_DPP_CONFIG(unit))->qos;

    dpp_qos->nof_ing_elsp = 1;
    dpp_qos->nof_ing_lif_cos = 16;
    dpp_qos->nof_ing_pcp_vlan = 8;
    dpp_qos->nof_egr_remark_id = 1;
    dpp_qos->nof_egr_pcp_vlan = 8;
    dpp_qos->nof_ing_cos_opcode = 3;
    dpp_qos->nof_egr_mpls_php = 4;
    dpp_qos->nof_egr_l2_i_tag = 1;
    dpp_qos->mpls_elsp_label_range_min = soc_property_get(unit, spn_MPLS_ELSP_LABEL_RANGE_MIN, -1);
    dpp_qos->mpls_elsp_label_range_max = soc_property_get(unit, spn_MPLS_ELSP_LABEL_RANGE_MAX, -1);
    dpp_qos->egr_pcp_vlan_dscp_exp_profile_id = -1;
    dpp_qos->egr_pcp_vlan_dscp_exp_enable = 0;
    dpp_qos->egr_remark_encap_enable = 0;
    dpp_qos->dp_max = 2;
    dpp_qos->nof_egr_dscp_exp_marking = 0;


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
soc_petra_default_config_meter_get(int unit)
{
    soc_dpp_config_meter_t *dpp_meter;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_meter = &(SOC_DPP_CONFIG(unit))->meter;

    dpp_meter->nof_meter_a = 8192;
    dpp_meter->nof_meter_b = 8192;
    dpp_meter->nof_meter_profiles = 512;
    dpp_meter->nof_high_rate_profiles = 16;
    dpp_meter->policer_min_rate = 64; /* kbits */
    dpp_meter->meter_min_rate = 64; /* kbits */
    dpp_meter->max_rate = 120000000; /* kbits */
    dpp_meter->min_burst = 64; /* bytes */
    dpp_meter->max_burst = 4161536; /* bytes */
    dpp_meter->lr_max_burst = 1040384; /* bytes */
    dpp_meter->lr_max_rate = 19000000; /* bytes */

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_default_config_l3_get(int unit)
{
    soc_dpp_config_l3_t *dpp_l3;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_l3 = &(SOC_DPP_CONFIG(unit))->l3;

    dpp_l3->max_nof_vrfs = soc_property_get(unit,spn_IPV4_NUM_VRFS,255);; /* from 1 - to 255*/
    dpp_l3->nof_rifs = 4096; /* 4K l3 interfaces */
    dpp_l3->ecmp_max_paths = 16;
    dpp_l3->eep_db_size = 4096;
    dpp_l3->fec_db_size = SOC_DPP_NOF_FECS_PETRAB;
    dpp_l3->fec_ecmp_reserved = 0x0;/* no reservation*/
    dpp_l3->vrrp_max_vid = 4096;
    dpp_l3->vrrp_ipv6_distinct = 0;
    dpp_l3->ip_enable = 1;
    dpp_l3->nof_rps = 0;
    dpp_l3->nof_bidir_vrfs = 0;

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_default_config_general_get(int unit)
{
    soc_dpp_config_pp_general_t *dpp_general;
    soc_dpp_config_pp_t *dpp_pp;

    SOCDNX_INIT_FUNC_DEFS

    dpp_general = &(SOC_DPP_CONFIG(unit))->dpp_general;
    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    dpp_pp->ipv4_tunnel_term_bitmap_enable = 0; /* none enabled */
    dpp_pp->ingress_ipv4_tunnel_term_mode = SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_DISABLE;

    /* Petra-B default tunnel termination is enabled */
    dpp_pp->ipv4_tunnel_term_bitmap_enable |= SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT;
    dpp_pp->sys_rsrc_mgmt  = 0;
    /* Petra-B default VLAN Translation is always Standard mode */
    dpp_pp->vlan_translate_mode = 0;

    dpp_pp->ipv6_tunnel_enable = 0;

    dpp_general->nof_user_define_traps = 60; 
    dpp_general->nof_ingress_trap_codes = 256;
    dpp_pp->ipmc_enable = 1;
    dpp_pp->initial_vid_enable = 0;
    dpp_pp->drop_dest = 0x1FFFF;
    dpp_pp->mpls_label_index_enable = 0;
    dpp_pp->vlan_match_db_mode = SOC_DPP_VLAN_DB_MODE_DEFAULT;
    dpp_pp->extend_p2p_mpls_enable = 0;
    dpp_pp->extend_p2p_vlan_enable = 0;
    dpp_pp->extend_p2p_mim_enable = 0;
    dpp_pp->global_lif_index_simple = soc_property_get(unit, spn_LOGICAL_PORT_L2_BRIDGE, 1);
    dpp_pp->global_lif_index_drop = soc_property_get(unit, spn_LOGICAL_PORT_DROP, SOC_PPC_AC_ID_INVALID);
    dpp_pp->pon_application_enable = 0;
    dpp_pp->pon_tcam_lkup_enable = 0;
    dpp_pp->evb_enable = 0;
    dpp_pp->next_hop_mac_extension_enable = 0;
    dpp_pp->mpls_eli_enable = 0;

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_default_config_l2_get(int unit)
{
    soc_dpp_config_l2_t *dpp_l2;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_l2 = &(SOC_DPP_CONFIG(unit))->l2;

    dpp_l2->nof_vsis = 16384; 
    dpp_l2->nof_lifs = 16384; 
    dpp_l2->mac_size = 32768;
    dpp_l2->mac_nof_event_handlers = 2;

    dpp_l2->learn_limit_mode = 0;
    dpp_l2->learn_limit_lif_range_base[0] = 0;
    dpp_l2->learn_limit_lif_range_base[1] = 0;

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_default_config_trill_get(int unit)
{
    soc_dpp_config_trill_t *dpp_trill;
    uint8                   trill_mode;

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

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_default_config_get(int unit, soc_dpp_config_pb_t *cfg)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(cfg, 0, sizeof (soc_dpp_config_pb_t));

    SOC_PB_MGMT_OPERATION_MODE_clear(&cfg->op_mode);
    SOC_PB_HW_ADJUSTMENTS_clear(&cfg->hw_adjust);
    SOC_PB_INIT_BASIC_CONF_clear(&cfg->basic_conf);
    SOC_PB_INIT_PORTS_clear(&cfg->fap_ports);
    SOC_PB_INIT_OOR_clear(&cfg->oor);
    SOC_PB_PP_MGMT_OPERATION_MODE_clear(&cfg->pp_op_mode);  

    /* TM init */
    rv = soc_petra_default_config_tm_get(unit);
    SOCDNX_IF_ERR_EXIT(rv);    

    /* TDM init */
    rv = soc_petra_default_config_tdm_get(unit);
    SOCDNX_IF_ERR_EXIT(rv);    

    /* QoS init */
    rv = soc_petra_default_config_qos_get(unit);
    SOCDNX_IF_ERR_EXIT(rv);    

    /* Meter init */
    rv = soc_petra_default_config_meter_get(unit);
    SOCDNX_IF_ERR_EXIT(rv);    

    /* l3 init */
    rv = soc_petra_default_config_l3_get(unit);
    SOCDNX_IF_ERR_EXIT(rv);    

    /* general pp init */
    rv = soc_petra_default_config_general_get(unit);
    SOCDNX_IF_ERR_EXIT(rv);    

    /* l2 init */
    rv = soc_petra_default_config_l2_get(unit);
    SOCDNX_IF_ERR_RETURN(rv);      

    /* trill init */
    rv = soc_petra_default_config_trill_get(unit);
    SOCDNX_IF_ERR_EXIT(rv);  

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_petra_info_config_device_ports(int unit) 
{
    SOCDNX_INIT_FUNC_DEFS;

    /* This function is for MBCM */

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_system_cell_format_type_get(int unit, uint8 *variable_cell_size_enable)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SYSTEM_CELL_FORMAT;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
      if (sal_strcmp(propval, "FCS") == 0) {
          *variable_cell_size_enable = FALSE;
      } else if (sal_strcmp(propval, "VCS128") == 0) {
          *variable_cell_size_enable = TRUE;
      } else  {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
      }
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_system_ftmh_load_balancing_ext_mode_get(int unit, SOC_PB_MGMT_FTMH_LB_EXT_MODE *system_ftmh_load_balancing_ext_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SYSTEM_FTMH_LOAD_BALANCING_EXT_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if ((!propval) || (sal_strcmp(propval, "DISABLED") == 0)) {
        *system_ftmh_load_balancing_ext_mode = SOC_PB_MGMT_FTMH_LB_EXT_MODE_DISABLED;
    } else if (sal_strcmp(propval, "8B_LB_KEY_8B_STACKING_ROUTE_HISTORY") == 0) {
        *system_ftmh_load_balancing_ext_mode = SOC_PB_MGMT_FTMH_LB_EXT_MODE_8B_LB_KEY_8B_STACKING_ROUTE_HISTORY;
    } else if (sal_strcmp(propval, "16B_STACKING_ROUTE_HISTORY") == 0) {
        *system_ftmh_load_balancing_ext_mode = SOC_PB_MGMT_FTMH_LB_EXT_MODE_16B_STACKING_ROUTE_HISTORY;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_petra_prop_fap_device_mode_get(int unit, uint8 *pp_enable, SOC_TMC_MGMT_TDM_MODE *tdm_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_FAP_DEVICE_MODE;
    propval = soc_property_get_str(unit, propkey);    
   
    if (propval) {
        if (sal_strcmp(propval, "TM") == 0) {
            *pp_enable = FALSE;
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_PACKET;
        } else if (sal_strcmp(propval, "PP") == 0) {
            *pp_enable = TRUE;
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_PACKET;
        } else if (sal_strcmp(propval, "TDM_OPTIMIZED") == 0) {
            *pp_enable = FALSE;
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_TDM_OPT;
        } else if (sal_strcmp(propval, "TDM_STANDARD") == 0) {
            *pp_enable  = FALSE;
            *tdm_mode = SOC_TMC_MGMT_TDM_MODE_TDM_STA;
        } else  {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_ext_qdr_protection_type_get(int unit, SOC_PETRA_HW_QDR_PROTECT_TYPE *ext_qdr_protection_type)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_QDR_PROTECTION_TYPE;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "ECC") == 0) {
            *ext_qdr_protection_type = SOC_PETRA_HW_QDR_PROTECT_TYPE_ECC;
        } else if (sal_strcmp(propval, "PARITY") == 0) {
            *ext_qdr_protection_type = SOC_PETRA_HW_QDR_PROTECT_TYPE_PARITY;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *ext_qdr_protection_type = SOC_PETRA_HW_NOF_QDR_PROTECT_TYPES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_ext_qdr_type_get(int unit, SOC_PB_HW_QDR_TYPE *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_QDR_TYPE;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "QDR") == 0) {
            *p_val = SOC_PB_HW_QDR_TYPE_QDR;
        } else if (sal_strcmp(propval, "QDR2P") == 0) {
            *p_val = SOC_PB_HW_QDR_TYPE_QDR2P;
        } else if (sal_strcmp(propval, "QDR3") == 0) {
            *p_val = SOC_PB_HW_QDR_TYPE_QDR3;
        } else if (sal_strcmp(propval, "NONE") == 0) {
            *p_val = SOC_PB_HW_QDR_NOF_TYPES;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *p_val = SOC_PB_HW_QDR_NOF_TYPES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_ext_qdr_size_mbit_get(int unit, SOC_PETRA_HW_QDR_SIZE_MBIT *p_val)
{
    char *propkey;
    uint32 propval, propinvalid = 0xffffffff;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_QDR_SIZE_MBIT;
    propval = soc_property_get(unit, propkey, propinvalid);

    if (propval != propinvalid) {
        switch (propval) {
            case 18:
                *p_val = SOC_PETRA_HW_QDR_SIZE_MBIT_18;
                break;
            case 36:
                *p_val = SOC_PETRA_HW_QDR_SIZE_MBIT_36;
                break;
            case 72:
                *p_val = SOC_PETRA_HW_QDR_SIZE_MBIT_72;
                break;
            case 144:
                *p_val = SOC_PETRA_HW_QDR_SIZE_MBIT_144;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for %s"), propval, propkey));
        }
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined if QDR is used"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_report_mode_get(int unit, SOC_TMC_STAT_IF_REPORT_MODE *stat_if_report_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_MODE;
    propval = soc_property_get_str(unit, propkey);    
    *stat_if_report_mode = SOC_TMC_STAT_NOF_IF_REPORT_MODES;

    if (propval) {
        if (sal_strcmp(propval, "BILLING") == 0) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_MODE_BILLING;
        } else if (sal_strcmp(propval, "FAP20V") == 0) {
            *stat_if_report_mode = SOC_TMC_STAT_IF_REPORT_MODE_FAP20V;
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_phase_get(int unit, SOC_TMC_STAT_IF_PHASE *stat_if_phase)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;
    
    propkey = spn_STAT_IF_PHASE;
    propval = soc_property_get_str(unit, propkey);    
    
    *stat_if_phase = SOC_TMC_STAT_NOF_IF_PHASES;
    
    if (propval) {
        if ((sal_strcmp(propval, "PHASE_000") == 0) || (sal_strcmp(propval, "0") == 0)) {
            *stat_if_phase = SOC_TMC_STAT_IF_PHASE_000;
        } else if ((sal_strcmp(propval, "PHASE_090") == 0) || (sal_strcmp(propval, "90") == 0)) {
            *stat_if_phase = SOC_TMC_STAT_IF_PHASE_090;
        } else if ((sal_strcmp(propval, "PHASE_180") == 0) || (sal_strcmp(propval, "180") == 0)) {
            *stat_if_phase = SOC_TMC_STAT_IF_PHASE_180;
        } else if ((sal_strcmp(propval, "PHASE_270") == 0) || (sal_strcmp(propval, "270") == 0)) {
            *stat_if_phase = SOC_TMC_STAT_IF_PHASE_270;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 
    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_report_billing_mode_get(int unit, SOC_TMC_STAT_IF_BILLING_MODE *stat_if_billing_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_BILLING_MODE;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_billing_mode = SOC_TMC_STAT_NOF_IF_BILLING_MODES;

    if (propval) {
        if (sal_strcmp(propval, "EGR_Q_NB") == 0) {
            *stat_if_billing_mode = SOC_TMC_STAT_IF_BILLING_MODE_EGR_Q_NUM;
        } else if (sal_strcmp(propval, "CUD") == 0) {
            *stat_if_billing_mode = SOC_TMC_STAT_IF_BILLING_MODE_CUD;
        } else if (sal_strcmp(propval, "VSI_VLAN") == 0) {
            *stat_if_billing_mode = SOC_TMC_STAT_IF_BILLING_MODE_VSI_VLAN;
        } else if (sal_strcmp(propval, "BOTH_LIFS") == 0) {
            *stat_if_billing_mode = SOC_TMC_STAT_IF_BILLING_MODE_BOTH_LIFS;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 
    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_report_fap20v_mode_get(int unit, SOC_TMC_STAT_IF_FAP20V_MODE *stat_if_fap20v_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_FAP20V_MODE;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_fap20v_mode = SOC_TMC_STAT_NOF_IF_FAP20V_MODES;

    if (propval) {
        if (sal_strcmp(propval, "QUEUE") == 0) {
            *stat_if_fap20v_mode = SOC_TMC_STAT_IF_FAP20V_MODE_Q_SIZE;
        } else if (sal_strcmp(propval, "PACKET") == 0) {
            *stat_if_fap20v_mode = SOC_TMC_STAT_IF_FAP20V_MODE_PKT_SIZE;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_report_fap20v_fabric_mc_get(int unit, SOC_TMC_STAT_IF_MC_MODE *stat_if_fap20v_fabric_mc)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_FAP20V_FABRIC_MC;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_fap20v_fabric_mc = SOC_TMC_STAT_NOF_IF_MC_MODES;

    if (propval) {
        if (sal_strcmp(propval, "QUEUE_NUM") == 0) {
            *stat_if_fap20v_fabric_mc = SOC_TMC_STAT_IF_MC_MODE_Q_NUM;
        } else if (sal_strcmp(propval, "MC_ID") == 0) {
            *stat_if_fap20v_fabric_mc = SOC_TMC_STAT_IF_MC_MODE_MC_ID;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_report_fap20v_ing_mc_get(int unit, SOC_TMC_STAT_IF_MC_MODE *stat_if_fap20v_ing_mc)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_FAP20V_ING_MC;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_fap20v_ing_mc = SOC_TMC_STAT_NOF_IF_MC_MODES;

    if (propval) {
        if (sal_strcmp(propval, "QUEUE_NUM") == 0) {
            *stat_if_fap20v_ing_mc = SOC_TMC_STAT_IF_MC_MODE_Q_NUM;
        } else if (sal_strcmp(propval, "MC_ID") == 0) {
            *stat_if_fap20v_ing_mc = SOC_TMC_STAT_IF_MC_MODE_MC_ID;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_parity_enable_get(int unit, SOC_TMC_STAT_IF_PARITY_MODE *stat_if_parity_enable)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_PARITY_ENABLE;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_parity_enable = SOC_TMC_STAT_IF_PARITY_MODE_DIS;

    if (propval) {
        if (sal_strcmp(propval, "1") == 0) {
            *stat_if_parity_enable = SOC_TMC_STAT_IF_PARITY_MODE_EN;
        } else if (sal_strcmp(propval, "TRUE") == 0) {
            *stat_if_parity_enable = SOC_TMC_STAT_IF_PARITY_MODE_EN;
        }
    } 

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_count_snoop_get(int unit, uint8 *stat_if_count_snoop)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_FAP20V_COUNT_SNOOP;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_count_snoop = FALSE;

    if (propval) {
        if (sal_strcmp(propval, "1") == 0) {
            *stat_if_count_snoop = TRUE;
        } else if (sal_strcmp(propval, "TRUE") == 0) {
            *stat_if_count_snoop = TRUE;
        }
    } 

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_is_original_pkt_size_get(int unit, uint8 *stat_if_is_original_pkt_size)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_ORIGINAL_PKT_SIZE;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_is_original_pkt_size = FALSE;

    if (propval) {
        if (sal_strcmp(propval, "1") == 0) {
            *stat_if_is_original_pkt_size = TRUE;
        } else if (sal_strcmp(propval, "TRUE") == 0) {
            *stat_if_is_original_pkt_size = TRUE;
        }
    } 

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_single_copy_reported_get(int unit, uint8 *stat_if_single_copy_reported)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_FAP20V_SINGLE_COPY_REPORTED;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_single_copy_reported = FALSE;

    if (propval) {
        if (sal_strcmp(propval, "1") == 0) {
            *stat_if_single_copy_reported = TRUE;
        } else if (sal_strcmp(propval, "TRUE") == 0) {
            *stat_if_single_copy_reported = TRUE;
        }
    } 

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_stat_if_report_fap20v_cnm_report_get(int unit, SOC_TMC_STAT_IF_CNM_MODE *stat_if_fap20v_cnm_report)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_STAT_IF_REPORT_FAP20V_CNM_REPORT;
    propval = soc_property_get_str(unit, propkey);    

    *stat_if_fap20v_cnm_report = SOC_TMC_STAT_NOF_IF_CNM_MODES;

    if (propval) {
        if ((sal_strcmp(propval, "DISABLE") == 0) ||
            (sal_strcmp(propval, "DIS")     == 0) ||
            (sal_strcmp(propval, "FALSE")   == 0) ||
            (sal_strcmp(propval, "0")       == 0)) {
            *stat_if_fap20v_cnm_report = SOC_TMC_STAT_IF_CNM_MODE_DIS;
        } else if ((sal_strcmp(propval, "ENABLE") == 0) ||
                   (sal_strcmp(propval, "EN")     == 0) ||
                   (sal_strcmp(propval, "TRUE")   == 0) ||
                   (sal_strcmp(propval, "1")      == 0)) {
            *stat_if_fap20v_cnm_report = SOC_TMC_STAT_IF_CNM_MODE_EN;
        } else {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_petra_str_prop_spaui_ipg_dic_mode_get(int unit, soc_port_t port, SOC_PB_NIF_IPG_DIC_MODE *dic_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SPAUI_IPG_DIC_MODE;
    propval = soc_property_port_get_str(unit, port, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "AVERAGE") == 0) {
            *dic_mode = SOC_PB_NIF_IPG_DIC_MODE_AVERAGE;
        } else if (sal_strcmp(propval, "MIN") == 0) {
            *dic_mode = SOC_PB_NIF_IPG_DIC_MODE_MINIMUM;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *dic_mode = SOC_PB_NIF_NOF_IPG_DIC_MODES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_spaui_crc_mode_get(int unit, soc_port_t port, SOC_PB_NIF_CRC_MODE *crc_mode)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SPAUI_CRC_MODE;
    propval = soc_property_port_get_str(unit, port, propkey);

    if (propval) {
        if (sal_strcmp(propval, "32b") == 0) {
            *crc_mode = SOC_PB_NIF_CRC_MODE_32;
        } else if (sal_strcmp(propval, "24b") == 0) {
            *crc_mode = SOC_PB_NIF_CRC_MODE_24;
        } else if (sal_strcmp(propval, "NONE") == 0) {
            *crc_mode = SOC_PB_NIF_CRC_MODE_NONE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *crc_mode = SOC_PB_NIF_NOF_CRC_MODES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_spaui_chan_fault_response_get(int unit, soc_port_t port, char* propkey, SOC_PB_NIF_FAULT_RESPONSE *fault_response)
{
    char *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propval = soc_property_port_get_str(unit, port, propkey);

    if (propval) {
        if (sal_strcmp(propval, "DATA_AND_IDLE") == 0) {
            *fault_response = SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_IDLE;
        } else if (sal_strcmp(propval, "DATA_AND_RF") == 0) {
            *fault_response = SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_RF;
        } else if (sal_strcmp(propval, "DATA_AND_LF") == 0) {
            *fault_response = SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_LF;
        } else if (sal_strcmp(propval, "NO_DATA_IDLE") == 0) {
            *fault_response = SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_IDLE;
        } else if (sal_strcmp(propval, "NO_DATA_RF") == 0) {
            *fault_response = SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_RF;
        } else if (sal_strcmp(propval, "NO_DATA_LF") == 0) {
            *fault_response = SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_LF;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        *fault_response = SOC_PB_NIF_NOF_FAULT_RESPONSES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_parse_ucode_port(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID *p_val, uint32 *channel)
{
    int found;
    uint32 local_chan;
    char *propkey, *propval, *s, *ss;
    char *prefix;
    int prefix_len, id;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_UCODE_PORT;
    propval = soc_property_port_get_str(unit, port, propkey);    

    s = propval;
    found = 0;
    *channel=0;

    /* Parse Petra-B interfaces */
    if (propval) {
        prefix = "XAUI";
        prefix_len = sal_strlen(prefix);

        if (!sal_strncasecmp(s, prefix, prefix_len)) {
            s += sal_strlen(prefix);                
            id = sal_ctoi(s, &ss);
            if (s == ss) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
            }        
            *p_val = SOC_PB_NIF_ID(XAUI,id);
            found = 1;
            s = ss;
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
                *p_val = SOC_PB_NIF_ID(RXAUI,id);
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
                *p_val = SOC_PB_NIF_ID(SGMII,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "QSGMII";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                id = sal_ctoi(s, &ss);
                if (s == ss) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("No interface index in (\"%s\") for %s"), propval, propkey));
                }        
                *p_val = SOC_PB_NIF_ID(QSGMII,id);
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
                *p_val = SOC_PB_NIF_ID(ILKN,id);
                found = 1;
                s = ss;
            }
        }

        if (!found) {
            prefix = "CPU";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                *p_val = SOC_PETRA_IF_ID_CPU;
                found = 1;
            }
        }

        if (!found) {
            prefix = "RCY";
            prefix_len = sal_strlen(prefix);

            if (!sal_strncasecmp(s, prefix, prefix_len)) {
                s += sal_strlen(prefix);
                *p_val = SOC_PETRA_IF_ID_RCY;
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

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_parse_pb_serdes_lane_tx_phys_media_type(int unit, soc_port_t port, SOC_PETRA_SRD_MEDIA_TYPE *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_PB_SERDES_LANE_TX_PHYS_MEDIA_TYPE;
    propval = soc_property_port_get_str(unit, port, propkey);    

    if (propval) {
      if (sal_strcmp(propval, "CHIP2CHIP") == 0) {
        *p_val = SOC_PETRA_SRD_MEDIA_TYPE_CHIP2CHIP;
      } else if (sal_strcmp(propval, "SHORT_BACKPLANE") == 0) {
          *p_val = SOC_PETRA_SRD_MEDIA_TYPE_SHORT_BACKPLANE;
      } else if (sal_strcmp(propval, "LONG_BACKPLANE") == 0) {
          *p_val = SOC_PETRA_SRD_MEDIA_TYPE_LONG_BACKPLANE;
      } else  {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
      }
    } else {
      *p_val = SOC_PETRA_SRD_NOF_MEDIA_TYPES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_parse_pb_serdes_lane_power_state(int unit, soc_port_t port, SOC_PETRA_SRD_POWER_STATE *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_PB_SERDES_LANE_POWER_STATE;
    propval = soc_property_port_get_str(unit, port, propkey);    

    if (propval) {
      if (sal_strcmp(propval, "DOWN") == 0) {
        *p_val = SOC_PETRA_SRD_POWER_STATE_DOWN;
      } else if (sal_strcmp(propval, "UP") == 0) {
          *p_val = SOC_PETRA_SRD_POWER_STATE_UP;
      } else if (sal_strcmp(propval, "UP_AND_RELOCK") == 0) {
          *p_val = SOC_PETRA_SRD_POWER_STATE_UP_AND_RELOCK;
      } else  {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
      }
    } else {
      *p_val = SOC_PETRA_SRD_NOF_MEDIA_TYPES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_petra_validate_fabric_mode(int unit, SOC_TMC_FABRIC_CONNECT_MODE *fabric_connect_mode)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* For MBCM */

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_str_prop_parse_system_stag_enconding_enable_mode(int unit, SOC_PB_ITM_STAG_ENABLE_MODE *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_SYSTEM_STAG_ENCONDING_ENABLE_MODE;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
      if (sal_strcmp(propval, "DISABLED") == 0) {
          *p_val = SOC_PB_ITM_STAG_ENABLE_MODE_DIS;
      } else if (sal_strcmp(propval, "EN_NO_VSQ") == 0) {
          *p_val = SOC_PB_ITM_STAG_ENABLE_MODE_EN_NO_VSQ;
      } else if (sal_strcmp(propval, "EN_WITH_VSQ") == 0) {
          *p_val = SOC_PB_ITM_STAG_ENABLE_MODE_EN_WITH_VSQ;
      } else  {
          SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
      }
    } else {
        *p_val = SOC_PB_ITM_STAG_ENABLE_MODE_DIS;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_petra_is_olp(int unit, soc_port_t port, uint32* is_olp)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_olp = (port == SOC_DPP_PORT_INTERNAL_OLP(0) ? 1: 0);

    SOCDNX_FUNC_RETURN;
}

int 
soc_petra_is_oamp(int unit, soc_port_t port, uint32* is_oamp)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_oamp = (port == SOC_DPP_PORT_INTERNAL_OAMP(0) ? 1 : 0);

    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_ref_clk_to_enum(int unit, int ref_clk, uint32 *p_val)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (ref_clk)
    {
      case 125000:
        *p_val = SOC_PETRA_MGMT_SRD_REF_CLK_125_00;
        break;
    
      case 156250:
        *p_val = SOC_PETRA_MGMT_SRD_REF_CLK_156_25;
        break;
    
      case 200000:
        *p_val = SOC_PETRA_MGMT_SRD_REF_CLK_200_00;
        break;
    
      case 312500:
        *p_val = SOC_PETRA_MGMT_SRD_REF_CLK_312_50;
        break;
    
      case 218750:
        *p_val = SOC_PETRA_MGMT_SRD_REF_CLK_218_75;
        break;
    
      case 212500:
        *p_val = SOC_PETRA_MGMT_SRD_REF_CLK_212_50;
        break;
    
      default:  
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected reference clock (%d)"), ref_clk));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_petra_rate_to_enum(int unit, int rate, uint32 *p_val)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (rate)
    {
      case 1000000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_1000_00;
        break;
    
      case 1041670:
        *p_val = SOC_PETRA_SRD_DATA_RATE_1041_67;
        break;
    
      case 1171880:
        *p_val = SOC_PETRA_SRD_DATA_RATE_1171_88;
        break;
    
      case 1250000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_1250_00;
        break;
    
      case 1302030:
        *p_val = SOC_PETRA_SRD_DATA_RATE_1302_03;
        break;
    
      case 1333330:
        *p_val = SOC_PETRA_SRD_DATA_RATE_1333_33;
        break;
    
      case 1562500:
        *p_val = SOC_PETRA_SRD_DATA_RATE_1562_50;
        break;
    
      case 2343750:
        *p_val = SOC_PETRA_SRD_DATA_RATE_2343_75;
        break;
    
      case 2500000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_2500_00;
        break;
    
      case 2604160:
        *p_val = SOC_PETRA_SRD_DATA_RATE_2604_16;
        break;
    
      case 2666670:
        *p_val = SOC_PETRA_SRD_DATA_RATE_2666_67;
        break;
    
      case 2083330:
        *p_val = SOC_PETRA_SRD_DATA_RATE_2083_33;
        break;
    
      case 3000000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_3000_00;
        break;
    
      case 3125000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_3125_00;
        break;
    
      case 3750000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_3750_00;
        break;
    
      case 4000000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_4000_00;
        break;
    
      case 4166670:
        *p_val = SOC_PETRA_SRD_DATA_RATE_4166_67;
        break;
    
      case 4687500:
        *p_val = SOC_PETRA_SRD_DATA_RATE_4687_50;
        break;
    
      case 5000000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_5000_00;
        break;
    
      case 5208330:
        *p_val = SOC_PETRA_SRD_DATA_RATE_5208_33;
        break;
    
      case 5333330:
        *p_val = SOC_PETRA_SRD_DATA_RATE_5333_33;
        break;
    
      case 5833330:
        *p_val = SOC_PETRA_SRD_DATA_RATE_5833_33;
        break;
    
      case 6000000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_6000_00;
        break;
    
      case 6250000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_6250_00;
        break;
    
      case 4375000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_4375_00;
        break;
    
      case 5468750:
        *p_val = SOC_PETRA_SRD_DATA_RATE_5468_75;
        break;
    
      case 4250000:
        *p_val = SOC_PETRA_SRD_DATA_RATE_4250_00;
        break;
        
      case SOC_PETRA_SRD_DATA_RATE_3125_00_FDR:
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected reference clock (%d)"), rate));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_petra_deinit(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_attach(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}

int
soc_petra_fc_oob_mode_validate(int unit, int port)
{
    SOCDNX_INIT_FUNC_DEFS;

    if ( (port == 1) && (SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port] & SOC_DPP_FC_CAL_MODE_RX_ENABLE) ) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "Disabling OOB port(%d) mode Rx Enable"), port));
        SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port] &= ~(SOC_DPP_FC_CAL_MODE_RX_ENABLE);
    }
    if ( (SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port] & SOC_DPP_FC_CAL_MODE_RX_ENABLE) &&
                (SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port] & SOC_DPP_FC_CAL_MODE_TX_ENABLE) ) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "Disabling OOB port(%d) mode Tx Enable"), port));
        SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port] &= ~(SOC_DPP_FC_CAL_MODE_TX_ENABLE);
    }

    SOCDNX_FUNC_RETURN;
}

static int _pp_port_configured[SOC_MAX_NUM_DEVICES][SOC_DPP_PETRA_MAX_LOCAL_PORTS];
static int _int_pp_port_configured[SOC_MAX_NUM_DEVICES][SOC_DPP_PORT_RANGE_NUM_ENTRIES];

int
soc_petra_info_config(int unit) {
    int rv, prop_invalid = 0xffffffff, val;
    uint32 channel;
    SOC_TMC_INTERFACE_ID nif_id, nif_ndx;
    uint32 mal_i, mal_ndx;
    SOC_PB_NIF_TYPE nif_type;
    soc_port_t port_i;
    int ind;
    uint32 pp_port, tm_port;
    int star_i, qrtt_i, lane_i, glbl_qrtt, found, ref_clk, if_i;
    int force_clk_m_n_divisors_zero_combo, force_clk_m_n_divisors_zero_nif, force_clk_m_n_divisors_zero_fabric, force_clk_m_n_divisors_zero_tmp;
    int rx_amp, rx_main, rx_pre, rx_post;
    uint32 rate;
    soc_info_t          *si;
    soc_dpp_config_pb_t *dpp_pb;
    int combo;
    int fabric_link;
    char *bname;
    SOC_PB_NIF_IPG_DIC_MODE spaui_dic_mode = SOC_PB_NIF_NOF_IPG_DIC_MODES;
    SOC_PB_NIF_CRC_MODE spaui_crc_mode = SOC_PB_NIF_NOF_CRC_MODES;
    SOC_PB_NIF_FAULT_RESPONSE spaui_fault_response;
    int auxiliary_table_mode;
    int sa_auth, core;

    dpp_pb = SOC_DPP_CONFIG(unit)->pb;
    si  = &SOC_INFO(unit);

    sal_memset(_pp_port_configured, 0x0, sizeof(int) * SOC_DPP_PETRA_MAX_LOCAL_PORTS * SOC_MAX_NUM_DEVICES);
    sal_memset(_int_pp_port_configured, 0x0, sizeof(int) * SOC_DPP_PORT_RANGE_NUM_ENTRIES * SOC_MAX_NUM_DEVICES);

    SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system = 0x1;    

    /* Need to set this up prior to setting up port map */
    _soc_dpp_port_map_init(unit);

    /*
     * Operation mode
     */

    dpp_pb->op_mode.is_fap20_in_system = soc_property_get(unit, spn_SYSTEM_HAS_FAP20, FALSE);
    dpp_pb->op_mode.is_fap21_in_system = soc_property_get(unit, spn_SYSTEM_HAS_FAP21, FALSE);
    dpp_pb->op_mode.is_petra_rev_a_in_system = soc_property_get(unit, spn_SYSTEM_HAS_PETRA_REV_A, FALSE);
    dpp_pb->op_mode.egr_mc_16k_groups_enable = soc_property_get(unit, spn_EGR_MC_16K_GROUPS, FALSE);
    dpp_pb->op_mode.is_fe1600_in_system = soc_property_get(unit, spn_SYSTEM_IS_FE1600_IN_SYSTEM, FALSE);


    if (soc_petra_str_prop_system_cell_format_type_get(
        unit, &dpp_pb->op_mode.fabric_cell_format.variable_cell_size_enable)) {
        return SOC_E_FAIL;
    }
    
    dpp_pb->op_mode.fabric_cell_format.segmentation_enable = soc_property_get(unit, spn_FABRIC_SEGMENTATION_ENABLE, TRUE);

    dpp_pb->op_mode.is_fe200_fabric =
        !dpp_pb->op_mode.egr_mc_16k_groups_enable &&
        !dpp_pb->op_mode.fabric_cell_format.variable_cell_size_enable;

    force_clk_m_n_divisors_zero_nif = soc_property_suffix_num_get(unit, 0, spn_FORCE_CLK_M_N_DIVISORS_ZERO, "nif", 0);
    force_clk_m_n_divisors_zero_fabric = soc_property_suffix_num_get(unit, 0, spn_FORCE_CLK_M_N_DIVISORS_ZERO, "fabric", 0);
    force_clk_m_n_divisors_zero_combo = soc_property_suffix_num_get(unit, 0, spn_FORCE_CLK_M_N_DIVISORS_ZERO, "combo", 0);
        
    /* If we do not force the m/n divisors to zero, we need to convert the ref clock (and later on, the rate values)
       to enums */
    ref_clk = soc_property_get(unit, spn_NIF_REF_CLOCK, SOC_PETRA_MGMT_NOF_SRD_REF_CLKS);
    if (force_clk_m_n_divisors_zero_nif) {
      dpp_pb->op_mode.ref_clocks_conf.nif_ref_clk = ref_clk;
    } else {
      rv = soc_petra_ref_clk_to_enum(unit, ref_clk, &dpp_pb->op_mode.ref_clocks_conf.nif_ref_clk);
      SOCDNX_IF_ERR_RETURN(rv);
    }
    ref_clk = soc_property_get(unit, spn_FABRIC_REF_CLOCK, SOC_PETRA_MGMT_NOF_SRD_REF_CLKS);
    if (force_clk_m_n_divisors_zero_fabric) {
      dpp_pb->op_mode.ref_clocks_conf.fabric_ref_clk = ref_clk;
    } else {
      rv = soc_petra_ref_clk_to_enum(unit, ref_clk, &dpp_pb->op_mode.ref_clocks_conf.fabric_ref_clk);
      SOCDNX_IF_ERR_RETURN(rv);
    }
    ref_clk = soc_property_get(unit, spn_COMBO_REF_CLOCK, SOC_PETRA_MGMT_NOF_SRD_REF_CLKS);
    if (force_clk_m_n_divisors_zero_combo) {
      dpp_pb->op_mode.ref_clocks_conf.combo_ref_clk = ref_clk;
    } else {
      rv = soc_petra_ref_clk_to_enum(unit, ref_clk, &dpp_pb->op_mode.ref_clocks_conf.combo_ref_clk);
      SOCDNX_IF_ERR_RETURN(rv);
    }
     
    if (soc_petra_prop_fap_device_mode_get(
        unit, &dpp_pb->op_mode.pp_enable, &dpp_pb->op_mode.tdm_mode)) {
        return SOC_E_FAIL;
    }
    
    
    if (dpp_pb->op_mode.pp_enable) {
      int num_range;
      dpp_pb->pp_op_mode.authentication_enable = FALSE;
      dpp_pb->pp_op_mode.hairpin_enable = TRUE;
      dpp_pb->pp_op_mode.split_horizon_filter_enable = TRUE;
      dpp_pb->pp_op_mode.system_vsi_enable = FALSE;
      dpp_pb->pp_op_mode.mpls_ether_types[0] = 0x8847;
      dpp_pb->pp_op_mode.mpls_ether_types[1] = 0x8848;
      dpp_pb->pp_op_mode.ipv4_info.nof_vrfs = 256;
      dpp_pb->pp_op_mode.ipv4_info.flags = SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE|SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG;
      dpp_pb->pp_op_mode.ipv4_info.ipv4_host_extend = soc_property_get(unit, spn_IP4_HOST_EXTENSION_TABLE_ENABLE, TRUE);
      dpp_pb->pp_op_mode.ipv4_info.pvlan_enable = FALSE;
      for (ind = 0; ind < SOC_DPP_NOF_VRFS_PETRAB; ++ind)
      {
        if (ind == 0 )
        {
          dpp_pb->pp_op_mode.ipv4_info.max_routes_in_vrf[ind] = soc_property_get(unit,spn_IPV4_NUM_ROUTES,50000);
        }
        else
        {
          dpp_pb->pp_op_mode.ipv4_info.max_routes_in_vrf[ind] = 100;
        }
      }

      sa_auth = soc_property_get(unit, spn_SA_AUTH_ENABLED, 0);
      dpp_pb->pp_op_mode.authentication_enable = sa_auth == 1 ? TRUE : FALSE;

      auxiliary_table_mode = soc_property_get(unit, spn_BCM886XX_AUXILIARY_TABLE_MODE, 0);  
      SOC_DPP_CONFIG(unit)->pp.pvlan_enable = FALSE;
      
      if (auxiliary_table_mode == 0) {
          dpp_pb->pp_op_mode.split_horizon_filter_enable = FALSE;
          dpp_pb->pp_op_mode.ipv4_info.pvlan_enable = TRUE;
          SOC_DPP_CONFIG(unit)->pp.pvlan_enable = TRUE;
      }
      else if (auxiliary_table_mode == 1) {
          dpp_pb->pp_op_mode.split_horizon_filter_enable = TRUE;
      }      
      else if (auxiliary_table_mode == 2) {
          /* Enable Mac-in-Mac */
          if (dpp_pb->pp_op_mode.authentication_enable) {
            /* Error: SA authentication and Mac-in-Mac cannot be both set */
            return SOC_E_FAIL;
          }
          dpp_pb->pp_op_mode.split_horizon_filter_enable = FALSE;
      }

      /* Default trap/snoop strength */
      SOC_DPP_CONFIG(unit)->pp.default_trap_strength = soc_property_get(unit, spn_DEFAULT_TRAP_STRENGTH, 5);
      SOC_DPP_CONFIG(unit)->pp.default_snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 1);


      /* Max vsi is reserved for Mac-in-Mac */
      dpp_pb->pp_op_mode.p2p_info.mim_vsi = SOC_PB_PP_VSI_ID_MAX;
    
      /* MPLS tunnel termination ranges */
      for (num_range = 0; num_range < SOC_PPC_MPLS_TERM_MAX_NOF_TERM_LABELS; num_range++) {
          SOC_DPP_CONFIG(unit)->pp.label_ranges[num_range].first_label = soc_property_suffix_num_get(unit, num_range, spn_MPLS_TUNNEL_TERM_LABEL_RANGE_MIN, 
                         "", 0);
          SOC_DPP_CONFIG(unit)->pp.label_ranges[num_range].last_label = soc_property_suffix_num_get(unit, num_range, spn_MPLS_TUNNEL_TERM_LABEL_RANGE_MAX, 
                         "", 0);
      }    

      /* IP tunnel initator range */
      SOC_DPP_CONFIG(unit)->pp.min_egress_encap_ip_tunnel_range = soc_property_get(unit, spn_EGRESS_ENCAP_IP_TUNNEL_RANGE_MIN, 0x1001);
      SOC_DPP_CONFIG(unit)->pp.max_egress_encap_ip_tunnel_range = soc_property_get(unit, spn_EGRESS_ENCAP_IP_TUNNEL_RANGE_MAX, 0x2000);

      /* Application extend P2P */
      SOC_DPP_CONFIG(unit)->pp.extend_p2p_global_enable = FALSE;
      for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
          SOC_DPP_CONFIG(unit)->pp.extend_p2p_port_enable[port_i] = soc_property_port_get(unit, port_i, spn_BCM886XX_PORT_EXTEND_P2P, 0);
          if (SOC_DPP_CONFIG(unit)->pp.extend_p2p_port_enable[port_i]) {
              SOC_DPP_CONFIG(unit)->pp.extend_p2p_global_enable = TRUE;
          }
      }    

      if (soc_dpp_str_prop_parse_logical_port_mim(unit, &SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx, &SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac)) {
          return SOC_E_FAIL;
      }

    }

    dpp_pb->op_mode.is_combo_nif[0] =
        soc_property_suffix_num_get(unit, 0, spn_COMBO_NIF, "", 1);
    dpp_pb->op_mode.is_combo_nif[1] =
        soc_property_suffix_num_get(unit, 1, spn_COMBO_NIF, "", 1);
        
    /* Add SFI ports */
    for (fabric_link = FABRIC_LOGICAL_PORT_BASE(unit);
         fabric_link < FABRIC_LOGICAL_PORT_BASE(unit) + SOC_PETRA_NOF_FABRIC_REGULAR_LINKS;
         ++fabric_link) {
        DPP_ADD_PORT(sfi, fabric_link);
        ++si->port_num;
    }
    for (combo = 0; combo < 2; ++combo) {
        if (!dpp_pb->op_mode.is_combo_nif[combo]) {
            for (fabric_link = SOC_PETRA_FIRST_FABRIC_COMBO_PORT(combo);
                 fabric_link < SOC_PETRA_FIRST_FABRIC_COMBO_PORT(combo) + SOC_PETRA_NOF_FABRIC_LINKS_IN_COMBO;
                 ++fabric_link) {
                DPP_ADD_PORT(sfi, fabric_link);
                ++si->port_num;
            }
        }
    }

    /* Load-Balancing */
    rv = soc_petra_str_prop_system_ftmh_load_balancing_ext_mode_get(unit, &dpp_pb->op_mode.ftmh_lb_ext_mode);
    SOCDNX_IF_ERR_RETURN(rv);

    /*
     * HW Adjustments
     */    

    dpp_pb->hw_adjust.core_freq.enable = TRUE;
    dpp_pb->hw_adjust.core_freq.frequency = soc_property_get(unit, spn_CORE_CLOCK_SPEED, 0);
    
    dpp_pb->hw_adjust.streaming_if.multi_port_mode = soc_property_get(unit, spn_STREAMING_IF_MULTI_PORT_MODE, 0);
    dpp_pb->hw_adjust.streaming_if.enable_timeoutcnt = soc_property_get(unit, spn_STREAMING_IF_ENABLE_TIMEOUTCNT, 0);
    dpp_pb->hw_adjust.streaming_if.timeout_prd = soc_property_get(unit, spn_STREAMING_IF_TIMEOUT_PRD, 0);
    dpp_pb->hw_adjust.streaming_if.quiet_mode = soc_property_get(unit, spn_STREAMING_IF_QUIET_MODE, 0);
    dpp_pb->hw_adjust.streaming_if.discard_bad_parity = soc_property_get(unit, spn_STREAMING_IF_DISCARD_BAD_PARITY, 0);
    dpp_pb->hw_adjust.streaming_if.discard_pkt_streaming = soc_property_get(unit, spn_STREAMING_IF_DISCARD_PKT_STREAMING, 1);
     
    /* 
     * Statistic-Interface
     */
    dpp_pb->hw_adjust.stat_if.if_conf.enable = soc_property_get(unit, spn_STAT_IF_ENABLE, FALSE);
    if (dpp_pb->hw_adjust.stat_if.if_conf.enable) {
        rv = soc_petra_str_prop_stat_if_phase_get(unit, &(dpp_pb->hw_adjust.stat_if.if_conf.if_phase));
        if (dpp_pb->hw_adjust.stat_if.if_conf.if_phase == SOC_TMC_STAT_NOF_IF_PHASES) {
            dpp_pb->hw_adjust.stat_if.if_conf.enable = FALSE;

            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "Statistics IF is enabled but the IF Phase is not configured.\n\r")));
            return SOC_E_FAIL;
        }
        
        rv = soc_petra_str_prop_stat_if_report_mode_get(unit, &(dpp_pb->hw_adjust.stat_if.rep_conf.mode));
        if (dpp_pb->hw_adjust.stat_if.rep_conf.mode == SOC_TMC_STAT_NOF_IF_REPORT_MODES) {
            dpp_pb->hw_adjust.stat_if.if_conf.enable = FALSE;
            
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "Statistics IF is enabled but the Report Mode is not configured.\n\r")));
            return SOC_E_FAIL;
        }

        if (dpp_pb->hw_adjust.stat_if.rep_conf.mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING) {
            rv = soc_petra_str_prop_stat_if_report_billing_mode_get(unit, (SOC_TMC_STAT_IF_BILLING_MODE *)&(dpp_pb->hw_adjust.stat_if.rep_conf.format.billing.mode));
            if (dpp_pb->hw_adjust.stat_if.rep_conf.format.billing.mode == SOC_PB_STAT_NOF_IF_BILLING_MODES) {
                dpp_pb->hw_adjust.stat_if.if_conf.enable = FALSE;
                
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "Statistics IF is enabled but the Billing Report Mode is not configured.\n\r")));
                return SOC_E_FAIL;
            }
        } else if (dpp_pb->hw_adjust.stat_if.rep_conf.mode == SOC_TMC_STAT_IF_REPORT_MODE_FAP20V) {
            rv = soc_petra_str_prop_stat_if_report_fap20v_mode_get(unit, (SOC_TMC_STAT_IF_FAP20V_MODE *)&(dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.mode));
            if (dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.mode == SOC_PB_STAT_NOF_IF_FAP20V_MODES) {
                dpp_pb->hw_adjust.stat_if.if_conf.enable = FALSE;
                SOCDNX_IF_ERR_RETURN(rv);
            }

            if (dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.mode == SOC_PB_STAT_IF_FAP20V_MODE_Q_SIZE) {
                dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.fabric_mc  = SOC_TMC_STAT_IF_MC_MODE_Q_NUM;
                dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.ing_rep_mc = SOC_TMC_STAT_IF_MC_MODE_Q_NUM;
                dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.cnm_report = SOC_TMC_STAT_IF_CNM_MODE_DIS;
            } else {
                rv = soc_petra_str_prop_stat_if_report_fap20v_fabric_mc_get(unit, (SOC_TMC_STAT_IF_MC_MODE *)&(dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.fabric_mc));
                if (dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.fabric_mc == SOC_PB_STAT_NOF_IF_MC_MODES) {
                    dpp_pb->hw_adjust.stat_if.if_conf.enable = FALSE;
                    SOCDNX_IF_ERR_RETURN(rv);
                }
                
                rv = soc_petra_str_prop_stat_if_report_fap20v_ing_mc_get(unit, (SOC_TMC_STAT_IF_MC_MODE *)&(dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.ing_rep_mc));
                if (dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.ing_rep_mc == SOC_PB_STAT_NOF_IF_MC_MODES) {
                    dpp_pb->hw_adjust.stat_if.if_conf.enable = FALSE;
                    SOCDNX_IF_ERR_RETURN(rv);
                }

                rv = soc_petra_str_prop_stat_if_report_fap20v_cnm_report_get(unit, (SOC_TMC_STAT_IF_CNM_MODE *)&(dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.cnm_report));
                if (dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.cnm_report == SOC_PB_STAT_NOF_IF_CNM_MODES) {
                  dpp_pb->hw_adjust.stat_if.if_conf.enable = FALSE;
                  SOCDNX_IF_ERR_RETURN(rv);
                }
            }
            
            SOCDNX_IF_ERR_RETURN(soc_petra_str_prop_stat_if_count_snoop_get(unit, &(dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.count_snoop)));
            SOCDNX_IF_ERR_RETURN(soc_petra_str_prop_stat_if_is_original_pkt_size_get(unit, &(dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.is_original_pkt_size)));
            SOCDNX_IF_ERR_RETURN(soc_petra_str_prop_stat_if_single_copy_reported_get(unit, &(dpp_pb->hw_adjust.stat_if.rep_conf.format.fap20v.single_copy_reported)));
        }

        dpp_pb->hw_adjust.stat_if.rep_conf.sync_rate = soc_property_get(unit, spn_STAT_IF_SYNC_RATE, 0);
        rv = soc_petra_str_prop_stat_if_parity_enable_get(unit, (SOC_TMC_STAT_IF_PARITY_MODE *)&(dpp_pb->hw_adjust.stat_if.rep_conf.parity_mode));
        SOCDNX_IF_ERR_RETURN(rv);
    }
    
    /*
     * Init basic conf
     */

    dpp_pb->basic_conf.credit_worth_enable = TRUE;
    dpp_pb->basic_conf.credit_worth = soc_property_get(unit, spn_CREDIT_SIZE, 1024);
    dpp_pb->basic_conf.stag_enable = soc_property_get(unit, spn_STAG_ENABLE, 0);
    dpp_pb->basic_conf.stag.encoding.offset_4bits = soc_property_get(unit, spn_SYSTEM_STAG_ENCONDING_OFFSET_4BIT, 0);
    rv = soc_petra_str_prop_parse_system_stag_enconding_enable_mode(unit, &dpp_pb->basic_conf.stag.encoding.enable_mode);
    SOCDNX_IF_ERR_RETURN(rv);

    /*
     * Init pbmp ports
     */
    /* Map local port to interface/tm_port/pp_port */
    for (port_i = 0; port_i < SOC_DPP_PETRA_MAX_LOCAL_PORTS; ++port_i) {
      rv = soc_petra_str_prop_parse_ucode_port(unit, port_i, &nif_id, &channel);
      SOCDNX_IF_ERR_RETURN(rv);

      if (nif_id != SOC_TMC_NIF_ID_NONE) {
        rv = petra_soc_dpp_local_to_nif_id_set(unit, port_i, nif_id, channel);
        SOCDNX_IF_ERR_RETURN(rv);
      
        val = soc_property_port_get(unit, port_i, spn_LOCAL_TO_TM_PORT, prop_invalid);
        if (val != prop_invalid) {
            if (petra_soc_dpp_local_to_tm_port_set(unit, port_i, 0, val)) {
                return SOC_E_FAIL;
            }
        }
        val = soc_property_port_get(unit, port_i, spn_LOCAL_TO_PP_PORT, prop_invalid);
        if (val != prop_invalid) {
            if (petra_soc_dpp_local_to_pp_port_set(unit, port_i, val)) {
              return SOC_E_FAIL;
            }
        }
      }

      if (petra_soc_dpp_local_port_partial(unit, port_i)) {
          LOG_ERROR(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "local port %d is partially mapped (nif/tm_port/pp_port should be mapped)\n\r"), port_i));
          return SOC_E_FAIL;
      }
    }

    /*
     * Add OLP/ERP ports
     */

    val = soc_property_get(unit, spn_NUM_OLP_TM_PORTS, 0);
    if (val > 0 && (!SOC_DPP_PP_ENABLE(unit))) {
        /* Miss configuration, OLP enabled but device mode is TM only */
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "Unexpected olp number of ports (%d) when device mode is not PP\n"), val));
    }

    if (val > 0) {        
        si->olp_port[0] = SOC_DPP_PORT_INTERNAL_OLP(0);
        SOCDNX_IF_ERR_RETURN(petra_soc_dpp_local_to_nif_id_set(unit, si->olp_port[0], SOC_TMC_IF_ID_OLP, 0));
        SOCDNX_IF_ERR_RETURN(petra_soc_dpp_local_to_tm_port_set_internal(unit, si->olp_port[0], SOC_PETRA_OLP_PORT_ID));
        SOCDNX_IF_ERR_RETURN(petra_soc_dpp_local_to_pp_port_set_internal(unit, si->olp_port[0], 63));
    }

    val = soc_property_get(unit, spn_NUM_ERP_TM_PORTS, 0);
    if (val > 0) {
        si->erp_port[0] = SOC_DPP_PORT_INTERNAL_ERP(0);
        SOCDNX_IF_ERR_RETURN(petra_soc_dpp_local_to_nif_id_set(unit, si->erp_port[0], SOC_TMC_IF_ID_ERP, 0));
        SOCDNX_IF_ERR_RETURN(petra_soc_dpp_local_to_tm_port_set_internal(unit, si->erp_port[0], SOC_PETRA_ERP_PORT_ID));
    }

    /* Set up PBMP ports */
    for (port_i = 0; port_i < SOC_DPP_PETRA_MAX_LOCAL_PORTS; ++port_i) {
      if (!petra_soc_dpp_local_port_valid(unit, port_i)) {
        continue;
      }

      rv = petra_soc_dpp_local_to_pp_port_get(unit, port_i, &pp_port, &core);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = petra_soc_dpp_local_to_tm_port_get(unit, port_i, &tm_port, &core);
      SOCDNX_IF_ERR_RETURN(rv);

      rv = petra_soc_dpp_local_to_nif_id_get(unit, port_i,
                    &dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id,
                    &dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.channel_id);
      SOCDNX_IF_ERR_RETURN(rv);

#ifdef DBG_LOCAL_PORT    
      LOG_CLI((BSL_META_U(unit,
                          "configure pbmp port(%d) tm_port(%d) pp_port(%d) nif(%d)\n"), port_i, tm_port, pp_port,
               dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id));
#endif      
      bname = "?";

      switch(dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id) {
        case SOC_PETRA_IF_ID_CPU:
          si->cmic_port = 0;
        
                
          si->port_type[port_i] = SOC_BLK_CMIC;
          SOC_PBMP_PORT_ADD((si->cmic_bitmap), port_i);
          DPP_ADD_PORT(all, port_i);
          si->port_num++;
          bname = "cpu";
                    
          dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_out = SOC_TMC_PORT_HEADER_TYPE_CPU;
          break;
        case SOC_TMC_IF_ID_RCY:          
          DPP_ADD_PORT(rcy,port_i);  
          DPP_ADD_PORT(all,port_i);
          si->port_num++;
          si->port_speed_max[port_i] = 1000;
          si->port_type[port_i] = SOC_BLK_ECI; /* General Type */
          bname = "rcy";
          break;

        default:
          nif_type =
            soc_pb_nif_id2type(dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id);
            
        switch (nif_type) {
        case SOC_PB_NIF_TYPE_XAUI:
        case SOC_PB_NIF_TYPE_RXAUI:
            DPP_ADD_PORT(port,port_i);
            DPP_ADD_PORT(all,port_i);
            DPP_ADD_PORT(xe,port_i);
            si->port_num++;
            si->port_type[port_i] = SOC_BLK_XGPORT;
            si->port_speed_max[port_i] = 10000;
            bname = "xe";
            break;
        case SOC_PB_NIF_TYPE_SGMII:
        case SOC_PB_NIF_TYPE_QSGMII:
            DPP_ADD_PORT(port,port_i);
            DPP_ADD_PORT(all,port_i);
            DPP_ADD_PORT(ge,port_i);
            si->port_num++;
            si->port_type[port_i] = SOC_BLK_XGPORT;
            si->port_speed_max[port_i] = 1000;
            bname = "ge";
            break;
        case SOC_PB_NIF_TYPE_ILKN:
            DPP_ADD_PORT(port,port_i);
            DPP_ADD_PORT(all,port_i);
            DPP_ADD_PORT(il,port_i);
            si->port_num++;
            si->port_type[port_i] = SOC_BLK_IL;
            si->port_speed_max[port_i] = 10000;
            bname = "il";
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "Unexpected interface type (%s) when mapping interface to port type port(%d)\n"),
                       SOC_PB_NIF_TYPE_to_string(nif_type), port_i));
            break;
        }
      }

      if (dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id != SOC_PETRA_IF_ID_OLP) {
      sal_snprintf(si->port_name[port_i], sizeof(si->port_name[port_i]),
                       "%s%d", bname, port_i);
      }

      dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].port_ndx = tm_port;
      ++dpp_pb->fap_ports.if_map_nof_entries;

      if (soc_dpp_str_prop_parse_tm_port_header_type(unit, port_i,
                              &dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_in,
                                                      &dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_out, NULL)) {
      return SOC_E_FAIL;
      }

      if (dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_out == SOC_TMC_PORT_HEADER_TYPE_TM) {
          if (soc_dpp_str_prop_parse_tm_port_otmh_extensions_en(unit,port_i,
                                                                 &dpp_pb->fap_ports.otmh_ext_en[dpp_pb->fap_ports.hdr_type_nof_entries])) {
              return SOC_E_FAIL;
          }
      }

/* TDM port*/
      if ((dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_out == SOC_TMC_PORT_HEADER_TYPE_TDM)
          || (dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_out == SOC_TMC_PORT_HEADER_TYPE_TDM_RAW)) {
        DPP_ADD_PORT(tdm, port_i);
      }

      if (dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_in == SOC_TMC_PORT_HEADER_TYPE_ETH) {
      DPP_ADD_PORT(ether, port_i);
      }
                 
      dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].port_ndx = tm_port;
     
      if (!_pp_port_configured[unit][pp_port]) {
      dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].profile_ndx = pp_port;
      
      dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.header_type =
          dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_in;
      
      dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.is_stag_enabled =
          soc_property_port_get(unit, port_i, spn_STAG_ENABLE, 0);
      dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.first_header_size =
          soc_property_port_get(unit, port_i, spn_FIRST_HEADER_SIZE, 0);
      
      /* If header type is TM, enable pph_present. We want to support this for CPU port, and it is a driver
         limitation that all tm ports will behave the same */
      if (dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_in == SOC_TMC_PORT_HEADER_TYPE_TM) {
          dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.is_tm_pph_present_enabled = 1;
      }
      
      rv = soc_dpp_str_prop_parse_flow_control_type(
          unit, port_i, 
          &dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.fc_type);
      SOCDNX_IF_ERR_RETURN(rv);
          
      dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.is_snoop_enabled =
          soc_property_port_get(unit, port_i, spn_SNOOP_ENABLE, 0);
      dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.mirror_profile =
          soc_property_port_get(unit, port_i, spn_MIRROR_PROFILE, 0);
          
      ++dpp_pb->fap_ports.pp_port_nof_entries;
      _pp_port_configured[unit][pp_port] = TRUE;
      }

      ++dpp_pb->fap_ports.hdr_type_nof_entries;
      
      dpp_pb->fap_ports.tm2pp_port_map[dpp_pb->fap_ports.tm2pp_port_map_nof_entries].pp_port = 
      pp_port;
      dpp_pb->fap_ports.tm2pp_port_map[dpp_pb->fap_ports.tm2pp_port_map_nof_entries].port_ndx =
      tm_port;
      ++dpp_pb->fap_ports.tm2pp_port_map_nof_entries;
      
      dpp_pb->fap_ports.egr_q_profile_map[dpp_pb->fap_ports.egr_q_profile_map_nof_entries].port_ndx =
      tm_port;
      dpp_pb->fap_ports.egr_q_profile_map[dpp_pb->fap_ports.egr_q_profile_map_nof_entries].conf = 0;
      ++dpp_pb->fap_ports.egr_q_profile_map_nof_entries;
    }

    /* Handle internal ports */
    for (port_i = SOC_DPP_PORT_INTERNAL_START; port_i <= SOC_DPP_PORT_INTERNAL_END; ++port_i) {
    if (!petra_soc_dpp_local_port_valid(unit, port_i)) {
        continue;
    }

    rv = petra_soc_dpp_local_to_pp_port_get(unit, port_i, &pp_port, &core);
    SOCDNX_IF_ERR_RETURN(rv);
    rv = petra_soc_dpp_local_to_tm_port_get(unit, port_i, &tm_port, &core);
    SOCDNX_IF_ERR_RETURN(rv);

    rv = petra_soc_dpp_local_to_nif_id_get(unit, port_i,
                      &dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id,
                      &dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.channel_id);
    SOCDNX_IF_ERR_RETURN(rv);
#ifdef DBG_LOCAL_PORT      
      LOG_CLI((BSL_META_U(unit,
                          "configure internal port(%d) tm_port(%d) pp_port(%d) nif(%d)\n"), port_i, tm_port, pp_port,
               dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id));
#endif
    bname = "?";
    switch(dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id) {

        case SOC_PETRA_IF_ID_ERP:
        bname = "erp";
#ifdef DBG_LOCAL_PORT
        LOG_CLI((BSL_META_U(unit,
                            "erp port(%d) if_id(%d) pp_port(%d) tm_port(%d)\n"), port_i, SOC_PETRA_IF_ID_ERP, pp_port, tm_port));
#endif
        break;
          
        case SOC_PETRA_IF_ID_OLP:
        bname = "olp";
#ifdef DBG_LOCAL_PORT
        LOG_CLI((BSL_META_U(unit,
                            "olp port(%d) if_id(%d) pp_port(%d) tm_port(%d)\n"), port_i, SOC_PETRA_IF_ID_OLP, pp_port, tm_port));
#endif
        break;

        default:
          LOG_ERROR(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unexpected interface id (%d) for port(%d)\n"),dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id,
                                port_i));
            break;
    }

    if (dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].conf.if_id != SOC_PETRA_IF_ID_ERP) {

        dpp_pb->fap_ports.if_map[dpp_pb->fap_ports.if_map_nof_entries].port_ndx = tm_port;
        ++dpp_pb->fap_ports.if_map_nof_entries;

        /* port starts at SOC_DPP_PORT_INTERNAL_START (512) - soc properties should be set for these by use if used */
        if (soc_dpp_str_prop_parse_tm_port_header_type(unit, port_i,
                                &dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_in,
                              &dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_out, NULL)) {
        return SOC_E_FAIL;    
        }

        if (dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_out == SOC_TMC_PORT_HEADER_TYPE_TM) {
          if (soc_dpp_str_prop_parse_tm_port_otmh_extensions_en(unit,port_i,
                                                                 &dpp_pb->fap_ports.otmh_ext_en[dpp_pb->fap_ports.hdr_type_nof_entries])) {
              return SOC_E_FAIL;
          }
      }
#ifdef DBG_LOCAL_PORT
          LOG_CLI((BSL_META_U(unit,
                              "configure internal  tm_port(%d) header type(%d)\n"), 
                   tm_port, 
                   dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].conf));
#endif    
        
        dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].port_ndx = tm_port;
        
        if ((pp_port != -1) && !_int_pp_port_configured[unit][pp_port]) {
#ifdef DBG_LOCAL_PORT
          LOG_CLI((BSL_META_U(unit,
                              "configure internal  pp_port(%d) pp_port_no_entries(%d) hdr_no_entries(%d)\n"), 
                   pp_port, 
                   dpp_pb->fap_ports.pp_port_nof_entries,
                   dpp_pb->fap_ports.hdr_type_nof_entries));
#endif
        dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].profile_ndx = pp_port;
        
        dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.header_type =
            dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_in;
        
        dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.is_stag_enabled =
            soc_property_port_get(unit, port_i, spn_STAG_ENABLE, 0);
        dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.first_header_size =
            soc_property_port_get(unit, port_i, spn_FIRST_HEADER_SIZE, 0);
        
        /* If header type is TM, enable pph_present. We want to support this for CPU port, and it is a driver
           limitation that all tm ports will behave the same */
        if (dpp_pb->fap_ports.hdr_type[dpp_pb->fap_ports.hdr_type_nof_entries].header_type_in == SOC_TMC_PORT_HEADER_TYPE_TM) {
            dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.is_tm_pph_present_enabled = 1;
        }
        
        rv = soc_dpp_str_prop_parse_flow_control_type(unit, port_i, 
                                   &dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.fc_type);
        SOCDNX_IF_ERR_RETURN(rv);
        
        dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.is_snoop_enabled =
            soc_property_port_get(unit, port_i, spn_SNOOP_ENABLE, 0);
        dpp_pb->fap_ports.pp_port[dpp_pb->fap_ports.pp_port_nof_entries].conf.mirror_profile =
            soc_property_port_get(unit, port_i, spn_MIRROR_PROFILE, 0);
        
        ++dpp_pb->fap_ports.pp_port_nof_entries;
        _int_pp_port_configured[unit][pp_port] = TRUE;
        }
        
        ++dpp_pb->fap_ports.hdr_type_nof_entries;
        
        dpp_pb->fap_ports.tm2pp_port_map[dpp_pb->fap_ports.tm2pp_port_map_nof_entries].pp_port = pp_port;
        dpp_pb->fap_ports.tm2pp_port_map[dpp_pb->fap_ports.tm2pp_port_map_nof_entries].port_ndx = tm_port;
        ++dpp_pb->fap_ports.tm2pp_port_map_nof_entries;
        
        dpp_pb->fap_ports.egr_q_profile_map[dpp_pb->fap_ports.egr_q_profile_map_nof_entries].port_ndx = tm_port;
        dpp_pb->fap_ports.egr_q_profile_map[dpp_pb->fap_ports.egr_q_profile_map_nof_entries].conf = 0;
        ++dpp_pb->fap_ports.egr_q_profile_map_nof_entries;
    }
    }

    /*
     * SerDes
     */    
    dpp_pb->hw_adjust.serdes.enable = TRUE;
    
    for (star_i = 0; star_i < SOC_PETRA_SRD_NOF_STAR_IDS; ++star_i) {
      dpp_pb->hw_adjust.serdes.conf.star_conf[star_i].enable = TRUE;
      
      for (qrtt_i = 0;
           qrtt_i < soc_petra_srd_star_nof_qrtts_get(star_i);
           ++qrtt_i) {
        glbl_qrtt = star_i * SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX + qrtt_i;
        dpp_pb->hw_adjust.serdes.conf.star_conf[star_i].conf.qrtt[qrtt_i].is_active =
          soc_property_suffix_num_get(unit, glbl_qrtt, spn_PB_SERDES_QRTT_ACTIVE, "", 1);
          
        rate = soc_property_suffix_num_get(unit, glbl_qrtt, spn_PB_SERDES_QRTT_MAX_EXPECTED_RATE, "", SOC_PETRA_SRD_NOF_DATA_RATES);
        if (SOC_PETRA_SRD_IS_NIF_LANE(glbl_qrtt*4)) {
          /* Nif quartet */ 
          force_clk_m_n_divisors_zero_tmp = force_clk_m_n_divisors_zero_nif;
        }
        else if (soc_petra_srd_qrtt2combo_id(glbl_qrtt) != SOC_PETRA_COMBO_NOF_QRTTS) {
          /* Combo quartet */
          force_clk_m_n_divisors_zero_tmp = force_clk_m_n_divisors_zero_combo;
        } else {
          /* Fabric quartet */ 
          force_clk_m_n_divisors_zero_tmp = force_clk_m_n_divisors_zero_fabric;
        }
        
        if (force_clk_m_n_divisors_zero_tmp) {
          dpp_pb->hw_adjust.serdes.conf.star_conf[star_i].conf.qrtt[qrtt_i].max_expected_lane_rate = rate;
        } else {
          rv = soc_petra_rate_to_enum(unit, rate, &dpp_pb->hw_adjust.serdes.conf.star_conf[star_i].conf.qrtt[qrtt_i].max_expected_lane_rate);
          SOCDNX_IF_ERR_RETURN(rv);
        }
      }
    }

    for (lane_i = 0; lane_i < SOC_PETRA_SRD_NOF_LANES; ++lane_i) {
      rate =
        soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_RATE, SOC_PETRA_SRD_NOF_DATA_RATES);
                    
      if (rate != SOC_PETRA_SRD_NOF_DATA_RATES) {
            
        if (SOC_PETRA_SRD_IS_NIF_LANE(lane_i)) {
          /* Nif quartet */ 
          force_clk_m_n_divisors_zero_tmp = force_clk_m_n_divisors_zero_nif;
        }
        else if (SOC_PETRA_SRD_IS_COMBO_LANE(lane_i)) {
          /* Combo quartet */
          force_clk_m_n_divisors_zero_tmp = force_clk_m_n_divisors_zero_combo;
        } else {
          /* Fabric quartet */ 
          force_clk_m_n_divisors_zero_tmp = force_clk_m_n_divisors_zero_fabric;
        }
        
        if (force_clk_m_n_divisors_zero_tmp) {
          dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].rate_conf = rate;
        } else {
          rv = soc_petra_rate_to_enum(unit, rate, &dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].rate_conf);
          SOCDNX_IF_ERR_RETURN(rv);
        }                 
        
        dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].enable = TRUE;
        
        dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].is_swap_polarity_rx =
          soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_SWAP_POLARITY_RX, FALSE);
        dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].is_swap_polarity_tx =
          soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_SWAP_POLARITY_TX, FALSE);
        
        rv = soc_petra_str_prop_parse_pb_serdes_lane_power_state(
               unit, lane_i,
               &dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].power_state_conf);
        SOCDNX_IF_ERR_RETURN(rv);
        
        dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].rx_phys_conf.intern.zcnt =
          soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_RX_PHYS_ZCNT, 0);
        dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].rx_phys_conf.intern.z1cnt =
          soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_RX_PHYS_Z1CNT, 0);
        dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].rx_phys_conf.intern.dfelth =
          soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_RX_PHYS_DFELTH, 0);
        dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].rx_phys_conf.intern.tlth =
          soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_RX_PHYS_TLTH, 0);
        dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].rx_phys_conf.intern.g1cnt =
          soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_RX_PHYS_G1CNT, 0);
        
        rv = soc_petra_str_prop_parse_pb_serdes_lane_tx_phys_media_type(unit, lane_i,
                &dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].tx_phys_conf.conf.media_type);
        SOCDNX_IF_ERR_RETURN(rv);
        
        if (dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].tx_phys_conf.conf.media_type !=
            SOC_PETRA_SRD_NOF_MEDIA_TYPES) {
          dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].tx_phys_conf_mode =
            SOC_PETRA_SRD_TX_PHYS_CONF_MODE_MEDIA_TYPE;
        }
        
        /* Try to get internal params */
        rx_amp = soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_TX_PHYS_AMP, 0);
        rx_main = soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_TX_PHYS_MAIN, 0);
        rx_pre = soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_TX_PHYS_PRE, 0);
        rx_post = soc_property_port_get(unit, lane_i, spn_PB_SERDES_LANE_TX_PHYS_POST, 0);
            
        if ((rx_amp  != 0) || (rx_main != 0) || (rx_pre  != 0) || (rx_post != 0)) {      
          dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].tx_phys_conf.conf.intern.amp = rx_amp;
          dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].tx_phys_conf.conf.intern.main = rx_main;
          dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].tx_phys_conf.conf.intern.pre = rx_pre;
          dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].tx_phys_conf.conf.intern.post = rx_post;
    
          dpp_pb->hw_adjust.serdes.conf.lane_conf[lane_i].tx_phys_conf_mode =
            SOC_PETRA_SRD_TX_PHYS_CONF_MODE_INTERNAL;
        }
      }
    }

    /*
     * DRAM
     */
    dpp_pb->hw_adjust.dram_pll.r = soc_property_get(unit, spn_EXT_RAM_PLL_R, prop_invalid);
    dpp_pb->hw_adjust.dram_pll.f = soc_property_get(unit, spn_EXT_RAM_PLL_F, prop_invalid);
    dpp_pb->hw_adjust.dram_pll.q = soc_property_get(unit, spn_EXT_RAM_PLL_Q, prop_invalid);
    
    val = soc_property_get(unit, spn_EXT_RAM_PRESENT, 0);
    if (val == 0) {
      /* No dram */
      dpp_pb->hw_adjust.dram.enable = FALSE;
    } else {
      dpp_pb->hw_adjust.dram.enable = TRUE;
      switch (val) {
        case 2:
          dpp_pb->hw_adjust.dram.is_valid[0] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[1] = FALSE;
          dpp_pb->hw_adjust.dram.is_valid[2] = FALSE;
          dpp_pb->hw_adjust.dram.is_valid[3] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[4] = FALSE;
          dpp_pb->hw_adjust.dram.is_valid[5] = FALSE;
          break;
        case 3:
          dpp_pb->hw_adjust.dram.is_valid[0] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[1] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[2] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[3] = FALSE;
          dpp_pb->hw_adjust.dram.is_valid[4] = FALSE;
          dpp_pb->hw_adjust.dram.is_valid[5] = FALSE;
          break;
        case 4:
          dpp_pb->hw_adjust.dram.is_valid[0] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[1] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[2] = FALSE;
          dpp_pb->hw_adjust.dram.is_valid[3] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[4] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[5] = FALSE;
          break;
        case 6:
          dpp_pb->hw_adjust.dram.is_valid[0] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[1] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[2] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[3] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[4] = TRUE;
          dpp_pb->hw_adjust.dram.is_valid[5] = TRUE;
          break;
        default:
          LOG_ERROR(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unsupported number of external dram interfaces (%d)\n\r"), val));
          return SOC_E_FAIL;
      }
      
      rv = soc_dpp_str_prop_ext_ram_type(unit, &dpp_pb->hw_adjust.dram.dram_type);
      SOCDNX_IF_ERR_RETURN(rv);
      
      dpp_pb->hw_adjust.dram.dram_size_total_mbyte =
        soc_property_get(unit, spn_EXT_RAM_TOTAL_SIZE, 0);
      dpp_pb->hw_adjust.dram.conf_mode =
        SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS;
      dpp_pb->hw_adjust.dram.dram_conf.params_mode.dram_freq =
        soc_property_get(unit, spn_EXT_RAM_FREQ, 0);
      dpp_pb->hw_adjust.dram.dbuff_size =
        soc_property_get(unit, spn_EXT_RAM_DBUFF_SIZE, SOC_PETRA_ITM_NOF_DBUFF_SIZES);
      
      rv = soc_dpp_prop_parse_dram_number_of_columns(unit,
        &dpp_pb->hw_adjust.dram.nof_columns);
      SOCDNX_IF_ERR_RETURN(rv);
      dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.nof_cols =
        dpp_pb->hw_adjust.dram.nof_columns;
              
      rv = soc_dpp_prop_parse_dram_number_of_banks(unit,
        &dpp_pb->hw_adjust.dram.nof_banks);
      SOCDNX_IF_ERR_RETURN(rv);
      dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.nof_banks =
        dpp_pb->hw_adjust.dram.nof_banks;      
        
      rv = soc_dpp_prop_parse_dram_ap_bit_pos(unit,
        &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.ap_bit_pos);
      SOCDNX_IF_ERR_RETURN(rv);
      
      rv = soc_dpp_prop_parse_dram_burst_size(unit,
        &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.burst_size);
      SOCDNX_IF_ERR_RETURN(rv);

      dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.auto_mode = TRUE;
      
      dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.c_cas_latency = soc_property_get(unit, spn_EXT_RAM_C_CAS_LATENCY, prop_invalid);
      dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.c_wr_latency = soc_property_get(unit, spn_EXT_RAM_C_WR_LATENCY, prop_invalid);
      dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.c_wr_latency = soc_property_get(unit, spn_EXT_RAM_C_WR_LATENCY, prop_invalid);
      dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_rc =  soc_property_get(unit, spn_EXT_RAM_T_RC, prop_invalid);

      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RFC, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_rfc);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RAS, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_ras);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_FAW, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_faw);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RCD_RD, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_rcd_rd);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RCD_WR, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_rcd_wr);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RRD, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_rrd);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_REF, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_ref);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RP, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_rp);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_WR, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_wr);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_WTR, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_wtr);
      SOCDNX_IF_ERR_RETURN(rv);
      rv = soc_dpp_dram_prop_get(unit, spn_EXT_RAM_T_RTP, &dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.t_rtp);
      SOCDNX_IF_ERR_RETURN(rv);
      
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr2.mrs0_wr1, spn_EXT_RAM_DDR2_MRS0_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr2.mrs0_wr2, spn_EXT_RAM_DDR2_MRS0_WR2);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr2.emr0_wr1, spn_EXT_RAM_DDR2_EMR0_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr2.emr0_wr2, spn_EXT_RAM_DDR2_EMR0_WR2);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr2.emr0_wr3, spn_EXT_RAM_DDR2_EMR0_WR3);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr2.emr1_wr1, spn_EXT_RAM_DDR2_EMR1_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr2.emr2_wr1, spn_EXT_RAM_DDR2_EMR2_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs0_wr1, spn_EXT_RAM_DDR3_MRS0_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs0_wr2, spn_EXT_RAM_DDR3_MRS0_WR2);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs1_wr1, spn_EXT_RAM_DDR3_MRS1_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs2_wr1, spn_EXT_RAM_DDR3_MRS2_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.ddr3.mrs3_wr1, spn_EXT_RAM_DDR3_MRS3_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.gdd3.mrs0_wr1, spn_EXT_RAM_GDDR3_MRS0_WR1);
      SOC_PETRA_DRAM_MODE_REG_SET(dpp_pb->hw_adjust.dram.dram_conf.params_mode.params.mode_regs.gdd3.emr0_wr1, spn_EXT_RAM_GDDR3_EMR0_WR1);
    }

    /*
     * QDR
     */
  
    rv = soc_petra_str_prop_ext_qdr_type_get(unit, &dpp_pb->hw_adjust.qdr.qdr_type);
    SOCDNX_IF_ERR_RETURN(rv);
    
    dpp_pb->hw_adjust.qdr.enable =
      SOC_SAND_NUM2BOOL_INVERSE(dpp_pb->hw_adjust.qdr.qdr_type == SOC_PB_HW_QDR_NOF_TYPES);

    if (dpp_pb->hw_adjust.qdr.enable) {
        dpp_pb->hw_adjust.qdr.is_core_clock_freq = soc_property_get(unit, spn_EXT_QDR_USE_CORE_CLOCK_FREQ, 1);
        if (!dpp_pb->hw_adjust.qdr.is_core_clock_freq)
        {
          dpp_pb->hw_adjust.qdr.pll_conf.m = soc_property_get(unit, spn_EXT_QDR_PLL_M, 0);
          dpp_pb->hw_adjust.qdr.pll_conf.n = soc_property_get(unit, spn_EXT_QDR_PLL_N, 0);
          dpp_pb->hw_adjust.qdr.pll_conf.p = soc_property_get(unit, spn_EXT_QDR_PLL_P, 0);
        }
        
        rv = soc_petra_str_prop_ext_qdr_protection_type_get(unit, &dpp_pb->hw_adjust.qdr.protection_type);
        SOCDNX_IF_ERR_RETURN(rv);
        
        rv = soc_petra_str_prop_ext_qdr_size_mbit_get(unit, &dpp_pb->hw_adjust.qdr.qdr_size_mbit);
        SOCDNX_IF_ERR_RETURN(rv);
    }
       
    /*
     * Fabric
     */
    dpp_pb->hw_adjust.fabric.enable = TRUE;
     
    rv = soc_dpp_str_prop_fabric_connect_mode_get(unit, &dpp_pb->hw_adjust.fabric.connect_mode);
    SOCDNX_IF_ERR_RETURN(rv);
     
    rv = soc_dpp_str_prop_fabric_ftmh_outlif_extension_get(unit, &dpp_pb->hw_adjust.fabric.ftmh_extension);
    SOCDNX_IF_ERR_RETURN(rv);
    
    /*
     * Nif
     */
    for (ind = 0; ind < SOC_PB_NIF_NOF_ILKN_IDS; ++ind) {
       val = soc_property_port_get(unit, ind, spn_ILKN_NUM_LANES, 0);
       
       if (val != 0) {
          dpp_pb->hw_adjust.nif.ilkn[dpp_pb->hw_adjust.nif.ilkn_nof_entries].conf.nof_lanes =
             val;
   
          dpp_pb->hw_adjust.nif.ilkn[dpp_pb->hw_adjust.nif.ilkn_nof_entries].conf.nof_lanes =
            soc_property_port_get(unit, ind, spn_ILKN_NUM_LANES, 0);
    
          dpp_pb->hw_adjust.nif.ilkn[dpp_pb->hw_adjust.nif.ilkn_nof_entries].ilkn_ndx = 
             SOC_PB_NIF_ID(ILKN,ind);
          val =
            soc_property_port_get(unit, ind, spn_ILKN_INVALID_LANE_ID, prop_invalid);
          
          if (val != prop_invalid) {
            dpp_pb->hw_adjust.nif.ilkn[dpp_pb->hw_adjust.nif.ilkn_nof_entries].conf.is_invalid_lane =
              TRUE;
            dpp_pb->hw_adjust.nif.ilkn[dpp_pb->hw_adjust.nif.ilkn_nof_entries].conf.invalid_lane_id =
              val;
          }

          dpp_pb->hw_adjust.nif.ilkn[dpp_pb->hw_adjust.nif.ilkn_nof_entries].conf.is_burst_interleaving =
            soc_property_port_get(unit, ind, spn_ILKN_IS_BURST_INTERLEAVING, 0);
          dpp_pb->hw_adjust.nif.ilkn[dpp_pb->hw_adjust.nif.ilkn_nof_entries].conf.metaframe_sync_period =
            soc_property_port_get(unit, ind, spn_ILKN_METAFRAME_SYNC_PERIOD, 2048);
   
          ++dpp_pb->hw_adjust.nif.ilkn_nof_entries;
       }
    }
    
    for (port_i = 0; port_i < SOC_DPP_PETRA_MAX_LOCAL_PORTS; ++port_i) {
      if (!petra_soc_dpp_local_port_valid(unit, port_i)) {
        continue;
      }

      rv = petra_soc_dpp_local_to_nif_id_get(unit, port_i, &nif_id, &channel);
      SOCDNX_IF_ERR_RETURN(rv);
      
      nif_type = soc_pb_nif_id2type(nif_id);
      
      if (nif_type == SOC_PB_NIF_TYPE_NONE) {
        continue;
      }
      
      nif_ndx = soc_pb_nif2intern_id(nif_id);
      mal_ndx = SOC_PB_NIF2MAL_GLBL_ID(nif_ndx);
            
      
        /* Search if this mal is already cofigured. If it is - validate type. If not,
           configure. */
        found = 0;
        for (mal_i = 0; mal_i < dpp_pb->hw_adjust.nif.mal_nof_entries; ++mal_i) {
          if (dpp_pb->hw_adjust.nif.mal[mal_i].mal_ndx == mal_ndx) {
            if (dpp_pb->hw_adjust.nif.mal[mal_i].conf.type != nif_type) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Mal %d configured to two different nif type (%s, %s)\n\r"),
                                    dpp_pb->hw_adjust.nif.mal[mal_i].mal_ndx,
                         SOC_PB_NIF_TYPE_to_string(nif_type),
                         SOC_PB_NIF_TYPE_to_string(dpp_pb->hw_adjust.nif.mal[mal_i].conf.type)));
              return SOC_E_FAIL;
            } else {
              found = 1; 
            }
          }
        }
      if (nif_type != SOC_PB_NIF_TYPE_ILKN) {  
        if (!found) {
          /* First time this mal is being configured */
          dpp_pb->hw_adjust.nif.mal[dpp_pb->hw_adjust.nif.mal_nof_entries].mal_ndx = mal_ndx;
          dpp_pb->hw_adjust.nif.mal[dpp_pb->hw_adjust.nif.mal_nof_entries].conf.type = nif_type;
            
          dpp_pb->hw_adjust.nif.mal[dpp_pb->hw_adjust.nif.mal_nof_entries].conf.topology.lanes_swap.swap_rx =
            dpp_pb->hw_adjust.nif.mal[dpp_pb->hw_adjust.nif.mal_nof_entries].conf.topology.lanes_swap.swap_tx =
              soc_property_port_get(unit, port_i, spn_LANES_SWAP,0);
           
          ++dpp_pb->hw_adjust.nif.mal_nof_entries;
        }
        
        if ((nif_type == SOC_PB_NIF_TYPE_XAUI)|| (nif_type == SOC_PB_NIF_TYPE_RXAUI)) {
          /* May need to configure SPAUI extensions
             Spaui extensions is per interface, but port may be interface.channel
             Let's search if the interface is already configured */
          for (if_i = 0; if_i < dpp_pb->hw_adjust.nif.spaui_nof_entries; ++if_i) {
              if (dpp_pb->hw_adjust.nif.spaui[if_i].if_ndx == nif_id) {
                  break;
              }
          }

          if (if_i == dpp_pb->hw_adjust.nif.spaui_nof_entries) {
              /* Interfce was not already configured. */

              val = soc_property_port_get(unit, port_i, spn_SPAUI_CHAN_BCT_SIZE, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ch_conf.bct_size =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ch_conf.enable = TRUE;
              }
 
              val = soc_property_port_get(unit, port_i, spn_SPAUI_LINK_PARTNER_DOUBLE_SIZE_BUS, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.link_partner_double_size_bus =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
              }

              val = soc_property_port_get(unit, port_i, spn_SPAUI_IS_DOUBLE_SIZE_SOP_ODD_ONLY, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.is_double_size_sop_odd_only =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
              }
              
              val = soc_property_port_get(unit, port_i, spn_SPAUI_IS_DOUBLE_SIZE_SOP_EVEN_ONLY, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.is_double_size_sop_even_only =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
              }

              val = soc_property_port_get(unit, port_i, spn_SPAUI_PREAMBLE_SIZE, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.preamble.size =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.preamble.enable =
                    TRUE;
              }

              val = soc_property_port_get(unit, port_i, spn_SPAUI_PREAMBLE_SKIP_SOP, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.preamble.skip_SOP =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.preamble.enable =
                    TRUE;
              }

              val = soc_property_port_get(unit, port_i, spn_SPAUI_IPG_SIZE, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ipg.size =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ipg.enable =
                    TRUE;
              }

              rv = soc_petra_str_prop_spaui_ipg_dic_mode_get(unit, port_i, &spaui_dic_mode);
              SOCDNX_IF_ERR_RETURN(rv);
              if (spaui_dic_mode != SOC_PB_NIF_NOF_IPG_DIC_MODES) {
                  
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ipg.dic_mode =
                    spaui_dic_mode;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ipg.enable =
                    TRUE;
              }

              rv = soc_petra_str_prop_spaui_crc_mode_get(unit, port_i, &spaui_crc_mode);
              SOCDNX_IF_ERR_RETURN(rv);
              if (spaui_crc_mode != SOC_PB_NIF_NOF_CRC_MODES) {
                  
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.crc_mode =
                    spaui_crc_mode;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
              }

              val = soc_property_port_get(unit, port_i, spn_SPAUI_CHAN_BCT_CHANNEL_BYTE_NDX, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ch_conf.bct_channel_byte_ndx =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ch_conf.enable =
                    TRUE;
              }

              val = soc_property_port_get(unit, port_i, spn_SPAUI_CHAN_BCT_SIZE, prop_invalid);
              if (val != prop_invalid) {
                 dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ch_conf.bct_size =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ch_conf.enable =
                    TRUE;
              }
              val = soc_property_port_get(unit, port_i, spn_SPAUI_CHAN_IS_BURST_INTERLEAVING, prop_invalid);
              if (val != prop_invalid) {
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ch_conf.is_burst_interleaving =
                    val;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.ch_conf.enable =
                    TRUE;
              }

              rv = soc_petra_str_prop_spaui_chan_fault_response_get(unit, port_i, spn_SPAUI_CHAN_FAULT_RESPONSE_LOCAL,
                                                               &spaui_fault_response);
              SOCDNX_IF_ERR_RETURN(rv);
              if (spaui_fault_response != SOC_PB_NIF_NOF_FAULT_RESPONSES) {              
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.fault_response.local =
                    spaui_fault_response;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
              }

              rv = soc_petra_str_prop_spaui_chan_fault_response_get(unit, port_i, spn_SPAUI_CHAN_FAULT_RESPONSE_REMOTE,
                                                               &spaui_fault_response);
              SOCDNX_IF_ERR_RETURN(rv);
              if (spaui_fault_response != SOC_PB_NIF_NOF_FAULT_RESPONSES) {              
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.fault_response.remote =
                    spaui_fault_response;
                  dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
              }

              if (dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable == TRUE) {
                dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].if_ndx = nif_id;
                dpp_pb->hw_adjust.nif.spaui[dpp_pb->hw_adjust.nif.spaui_nof_entries].conf.enable = TRUE;
                ++dpp_pb->hw_adjust.nif.spaui_nof_entries;
              }
          }
        }
      }
      
      if ((nif_type == SOC_PB_NIF_TYPE_QSGMII) || (nif_type == SOC_PB_NIF_TYPE_SGMII)) {
          
        dpp_pb->hw_adjust.nif.gmii[dpp_pb->hw_adjust.nif.gmii_nof_entries].nif_ndx = nif_id;
        dpp_pb->hw_adjust.nif.gmii[dpp_pb->hw_adjust.nif.gmii_nof_entries].conf.enable_tx =
          soc_property_port_get(unit, port_i, spn_GMII_ENABLE_TX, 1);
        dpp_pb->hw_adjust.nif.gmii[dpp_pb->hw_adjust.nif.gmii_nof_entries].conf.enable_rx = 
          soc_property_port_get(unit, port_i, spn_GMII_ENABLE_RX, 1);
        dpp_pb->hw_adjust.nif.gmii[dpp_pb->hw_adjust.nif.gmii_nof_entries].conf.mode = 
          soc_property_port_get(unit, port_i, spn_GMII_MODE, 1);
        dpp_pb->hw_adjust.nif.gmii[dpp_pb->hw_adjust.nif.gmii_nof_entries].conf.rate = 
          soc_property_port_get(unit, port_i, spn_GMII_RATE, 0);
        dpp_pb->hw_adjust.nif.gmii_nof_entries++;
      }
    }
    
    for(mal_i = 0; mal_i < dpp_pb->hw_adjust.nif.mal_nof_entries; mal_i++)
    {
      if(SOC_PB_NIF_MAL2MALG_ID(dpp_pb->hw_adjust.nif.mal[mal_i].mal_ndx) == 0)
      {
        dpp_pb->hw_adjust.nif.mal[mal_i].conf.topology.is_qsgmii_alt = soc_property_suffix_num_get(unit, dpp_pb->hw_adjust.nif.mal[mal_i].mal_ndx % SOC_PB_NIF_MALS_IN_MALG, spn_PB_QSGMII_ALT_MAPPING, "a_", 0);
      }
      if(SOC_PB_NIF_MAL2MALG_ID(dpp_pb->hw_adjust.nif.mal[mal_i].mal_ndx) == 1)
      {
        dpp_pb->hw_adjust.nif.mal[mal_i].conf.topology.is_qsgmii_alt = soc_property_suffix_num_get(unit, dpp_pb->hw_adjust.nif.mal[mal_i].mal_ndx % SOC_PB_NIF_MALS_IN_MALG, spn_PB_QSGMII_ALT_MAPPING, "b_", 0);
      }
    }

    val = soc_property_get(unit, spn_EXTERNAL_LOOKUP_MAL, prop_invalid);
    if (val != prop_invalid) {
      dpp_pb->hw_adjust.nif.elk_nof_entries = 1;
      dpp_pb->hw_adjust.nif.elk.conf.enable = TRUE;
      dpp_pb->hw_adjust.nif.elk.conf.mal_id = val;
    }

    val = soc_property_get(unit, spn_MDIO_CLOCK_FREQ_KHZ, prop_invalid);
    if (val != prop_invalid) {
      dpp_pb->hw_adjust.nif.mdio_nof_entries = 1;
      dpp_pb->hw_adjust.nif.mdio.clk_freq_khz = val;
    }

    /*
     * Out of reset
     */
     
    dpp_pb->oor.nof_entries = 0;
    for (mal_i = 0; mal_i < dpp_pb->hw_adjust.nif.mal_nof_entries; mal_i++) {
        mal_ndx = dpp_pb->hw_adjust.nif.mal[mal_i].mal_ndx;
        nif_type = dpp_pb->hw_adjust.nif.mal[mal_i].conf.type;
        if ((nif_type == SOC_PB_NIF_TYPE_XAUI) && (!SOC_PB_NIF_IS_BASE_MAL(mal_ndx))) {
            continue;
        }
        dpp_pb->oor.nif_id_active[dpp_pb->oor.nof_entries++] = soc_pb_nif_intern2nif_id(nif_type, SOC_PB_NIF_MAL2NIF_BASE_ID(mal_ndx));
    }

    /* 
     *  Common TM configuration
     */
    rv = soc_dpp_info_config_common_tm(unit);
    SOCDNX_IF_ERR_RETURN(rv);    

    /* Port map is now set for soc init portion ISQ/FMQ added later */
    /* don't clear all entries later whether default setup or not   */
    _dflt_tm_pp_port_map[unit] = FALSE;
        
    return SOC_E_NONE;
}

#ifdef PLISIM

SOC_SAND_RET
  soc_dpp_sand_write_cb_chipsim_pb(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  int
    unit;
    
  unit = PTR_TO_INT(base_address);
   
  if (unit >= CHIP_SIM_NOF_CHIPS) {
    LOG_ERROR(BSL_LS_SOC_INIT,
              (BSL_META_U(unit,
                          "unit (%d) >= CHIP_SIM_NOF_CHIPS (%d)"), unit, CHIP_SIM_NOF_CHIPS));
    return SOC_SAND_ERR;
  }

  return soc_sand_eci_write(array, Soc_pb_reg_buffer[unit], offset, size);
}

SOC_SAND_RET
  soc_dpp_sand_read_cb_chipsim_pb(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  int
    unit;
    
  unit = PTR_TO_INT(base_address);
   
  if (unit >= CHIP_SIM_NOF_CHIPS) {
    LOG_ERROR(BSL_LS_SOC_INIT,
              (BSL_META_U(unit,
                          "unit (%d) >= CHIP_SIM_NOF_CHIPS (%d)"), unit, CHIP_SIM_NOF_CHIPS));
    return SOC_SAND_ERR;
  }

  return soc_sand_eci_read(array, Soc_pb_reg_buffer[unit], offset, size);
}

#endif /* PLISIM */

int
soc_petra_init(int unit, int reset)
{    
    int soc_sand_rv = 0, silent = 0, rv;
    soc_dpp_config_t *dpp = NULL;
    int dram_size_per_interface_mbyte, nof_drams;
    int auxiliary_table_mode, port_i;
    int     core_id = SOC_CORE_INVALID;
    dpp = SOC_DPP_CONFIG(unit);
    
    if (!dpp->pb) {
      dpp->pb = sal_alloc(sizeof (soc_dpp_config_pb_t), "soc_pb_config");
      if (dpp->pb == NULL) {
          return SOC_E_MEMORY;
      }
    }
    
    soc_petra_default_config_get(unit, dpp->pb);

    rv = soc_petra_info_config(unit);
    SOCDNX_IF_ERR_RETURN(rv);

    /* 
     * Common device init 
     */
    rv = soc_dpp_common_init(unit);
    SOCDNX_IF_ERR_RETURN(rv);

    /*
     * Operation mode
     */
    soc_sand_rv = soc_pb_mgmt_operation_mode_set(unit, &(dpp->pb->op_mode));
    SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

    /*
     * Initialize SOC link control module
     */
    SOCDNX_IF_ERR_RETURN(soc_linkctrl_init(unit, &soc_linkctrl_driver_petra));

#ifdef BCM_WARM_BOOT_SUPPORT

    rv = soc_sw_db_init(unit);
    if (rv != SOC_E_NONE) {        
      return rv;
    }

    if (SOC_WARM_BOOT(unit))
    {      
      return SOC_E_NONE;
    }
#endif

         
    nof_drams = soc_property_get(unit, spn_EXT_RAM_PRESENT, 0);
    if (dpp->pb->hw_adjust.dram.enable && (nof_drams > 0)) {
      dram_size_per_interface_mbyte = (dpp->pb->hw_adjust.dram.dram_size_total_mbyte) / nof_drams;
      soc_sand_rv = soc_petra_sw_db_dram_dram_size_set(unit, dram_size_per_interface_mbyte);
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
    }

    /*
     * Phase 1
     */    
    soc_sand_rv = soc_pb_mgmt_init_sequence_phase1(unit, &(dpp->pb->hw_adjust),
              &(dpp->pb->basic_conf), &(dpp->pb->fap_ports), silent);
    SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
             
    if (dpp->pb->op_mode.pp_enable) {
      SOC_PB_PP_INIT_PHASE1_CONF pp_phase1_conf;
      SOC_PB_PP_INIT_PHASE2_CONF pp_phase2_conf;
      SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO label_range_info;
      SOC_PPD_EG_ENCAP_RANGE_INFO range_info;
      int num_range;
      SOC_PPD_FRWRD_MACT_AGING_INFO
          aging_info;
           
      SOC_PB_PP_INIT_PHASE1_CONF_clear(&pp_phase1_conf);
      SOC_PB_PP_INIT_PHASE2_CONF_clear(&pp_phase2_conf);
      SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO_clear(&label_range_info);
      SOC_PPD_EG_ENCAP_RANGE_INFO_clear(&range_info);
      
      soc_sand_rv = soc_ppd_procedure_desc_add();
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

      auxiliary_table_mode = soc_property_get(unit, spn_BCM886XX_AUXILIARY_TABLE_MODE, 0);
      if (auxiliary_table_mode == 2) {
          /* Enable Mac_in_Mac on device */
          soc_sand_rv = soc_pb_pp_frwrd_bmact_mac_in_mac_enable(unit);
          SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
      }
      
      soc_sand_rv = soc_pb_pp_mgmt_operation_mode_set(unit, &dpp->pb->pp_op_mode);
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
      
      soc_sand_rv = soc_pb_pp_mgmt_init_sequence_phase1(unit, &pp_phase1_conf, silent);
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

      soc_sand_rv = soc_pb_pp_mgmt_init_sequence_phase2(unit, &pp_phase2_conf, silent);
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
      
      soc_sand_rv = soc_ppd_diag_sample_enable_set(unit, TRUE);
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

      /* MPLS tunnel termination setup */
      for (num_range = 0; num_range < SOC_PPD_MPLS_TERM_MAX_NOF_TERM_LABELS; num_range++) {
          soc_sand_rv = soc_ppd_mpls_term_label_range_get(unit,num_range,&label_range_info);
          SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

          label_range_info.range.first_label = dpp->pp.label_ranges[num_range].first_label;
          label_range_info.range.last_label = dpp->pp.label_ranges[num_range].last_label;

          soc_sand_rv = soc_ppd_mpls_term_label_range_set(unit,num_range,&label_range_info);
          SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
      }

      /* Egress encapsulation: IP tunnel initatitor range set */
      soc_sand_rv = soc_ppd_eg_encap_range_info_get(unit,&range_info);
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
      
      range_info.ll_limit = SOC_SAND_MAX((dpp->pp.min_egress_encap_ip_tunnel_range - 1),0);
      range_info.ip_tnl_limit = dpp->pp.max_egress_encap_ip_tunnel_range;
      
      soc_sand_rv = soc_ppd_eg_encap_range_info_set(unit,&range_info);
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);


      SOC_PPD_FRWRD_MACT_AGING_INFO_clear(&aging_info);

      soc_sand_rv = soc_ppd_frwrd_mact_aging_info_set(unit,&aging_info);
      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

      /* Init. TCAM for IPV4/IPV6 MC/IPV6 UC.
          enable IPMC and,or IPV6, will result on limited resources for field
          processor - is called only on cold boot*/        
      {
          uint32 tcam_flags = 0;
          uint32 ipmc_enable, ipv6_enable;

          /* Set IPMC, IPV6 modes and initialize TCAM configuration accordingly */
          ipmc_enable = soc_property_get(unit, spn_IPMC_ENABLE, 1);
          ipv6_enable = soc_property_get(unit, spn_IPV6_ENABLE, 1);

          dpp->pp.ipmc_enable = (ipmc_enable) ? 1:0;

          if (ipmc_enable) {
              tcam_flags |= SOC_PB_PP_FRWRD_IP_TCAM_IPV4_MC;
          }
          if (ipv6_enable) {
              tcam_flags |= SOC_PB_PP_FRWRD_IP_TCAM_IPV6_UC;
          }
          if (ipmc_enable && ipv6_enable) {
              tcam_flags |= SOC_PB_PP_FRWRD_IP_TCAM_IPV6_MC;
          }
          if (tcam_flags != 0) {
              /* Call TCAM init */
              if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(unit)) {                                    
                  LOG_ERROR(BSL_LS_SOC_INIT,
                            (BSL_META_U(unit,
                                        "Error while taking soc_sand chip descriptor mutex\n")));
                  rv = SOC_E_INTERNAL;
                  SOCDNX_IF_ERR_RETURN(rv);                    
              }
              soc_sand_rv = soc_pb_pp_frwrd_ip_tcam_init_unsafe(unit,tcam_flags);

              if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit)) {
                  LOG_ERROR(BSL_LS_SOC_INIT,
                            (BSL_META_U(unit,
                                        "Error while giving soc_sand chip descriptor mutex\n")));
                  rv = SOC_E_INTERNAL;
                  SOCDNX_IF_ERR_RETURN(rv);                                             
              }    

              rv = handle_sand_result(soc_sand_rv);
              SOCDNX_IF_ERR_RETURN(rv);
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

         /* Global info MPLS termination */
         SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_mpls_term_lkup_info_get(unit,&lkup_info));

         lkup_info.key_type = (include_inrif) ? SOC_PPD_MPLS_TERM_KEY_TYPE_LABEL_RIF:SOC_PPD_MPLS_TERM_KEY_TYPE_LABEL;

         SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_mpls_term_lkup_info_set(unit,&lkup_info));
       }
       /* Extend P2P settings */
       {
         int global_enable = FALSE;
         SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO glbl_info;
         SOC_PPD_PORT_INFO port_info;

         SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_clear(&glbl_info);

         for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
           if (SOC_DPP_CONFIG(unit)->pp.extend_p2p_port_enable[port_i]) {
              global_enable = TRUE;
              glbl_info.enable = TRUE;

              SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_get(unit, core_id, port_i,&port_info));
              port_info.extend_p2p_info.enable_incoming_extend_p2p = TRUE;
              port_info.extend_p2p_info.enable_outgoing_extend_p2p = TRUE;
              SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_set(unit, core_id, port_i,&port_info));
           }           
         }
         
         if (global_enable) {
           glbl_info.pwe_info.system_vsi = SOC_PB_PP_VSI_ID_MAX; 
           glbl_info.ac_info.system_vsi = SOC_PB_PP_VSI_ID_MAX;            
           SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_frwrd_extend_p2p_glbl_info_set(unit,&glbl_info));
         }
       }
    }

    /* Common TM init */
    rv = soc_dpp_common_tm_init(unit, &(dpp->tm.multicast_egress_bitmap_group_range));
    SOCDNX_IF_ERR_RETURN(rv);
    
    /* 
     * Initialize TDM applications.
     */
    if (dpp->pb->op_mode.tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_OPT 
        || dpp->pb->op_mode.tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_STA) {         
        /* Set default ftmh for each relevant tm port */
        int index = 0;
        SOC_PB_TDM_FTMH_INFO ftmh_info;

        SOC_PB_TDM_FTMH_INFO_clear(&ftmh_info);
        ftmh_info.action_ing = SOC_PB_TDM_ING_ACTION_NO_CHANGE;
        ftmh_info.action_eg = SOC_PB_TDM_EG_ACTION_NO_CHANGE;

        for (index = 0; index < dpp->pb->fap_ports.hdr_type_nof_entries; index++) {
            if (dpp->pb->fap_ports.hdr_type[index].header_type_in == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                soc_sand_rv = soc_pb_tdm_ftmh_set(unit,
                                          SOC_CORE_DEFAULT,
                                          dpp->pb->fap_ports.hdr_type[index].port_ndx,
                                          &ftmh_info);
                SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
            }
        }
    }

     /* TDM Optimize init */
    if (dpp->pb->op_mode.tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_OPT) { 
        /* cell size in Optimize mode. */
        soc_sand_rv = soc_pb_tdm_opt_size_set(
            unit,
            SOC_DPP_DRV_TDM_OPT_SIZE
        );
        SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
    }

    return SOC_E_NONE;
}   

int
soc_petra_default_ofp_rates_set(int unit) {
    int soc_sand_rv = 0;
    int rv = SOC_E_NONE;
    soc_dpp_config_t *dpp = NULL;
    const uint32
      ndx_max = SOC_PB_NOF_MAC_LANES + 4; /* MAC lanes + CPU,ERP,OLP,RCY */
    SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
      *shaper = NULL;
    SOC_PETRA_OFP_RATES_TBL_INFO
      *rates_tbl = NULL;
    uint32 mal_ids[SOC_PB_NOF_MAC_LANES + 4];
    SOC_PB_NIF_TYPE nif_type;             
    int i;
    uint32 rate;
    uint32 mal_id, ndx;

    
    shaper = sal_alloc(sizeof(SOC_PETRA_OFP_RATES_MAL_SHPR_INFO) * ndx_max, "shaper");
    if (!shaper) {
        rv = SOC_E_MEMORY;
        goto exit;
    }
    rates_tbl = sal_alloc(sizeof(SOC_PETRA_OFP_RATES_TBL_INFO) * ndx_max, "rates_tbl");
    if (!rates_tbl) {
        rv = SOC_E_MEMORY;
        goto exit;
    }

    

    dpp = SOC_DPP_CONFIG(unit);
    
    for (ndx = 0; ndx < ndx_max; ++ndx) {
        soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(&rates_tbl[ndx]);
        soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&shaper[ndx]);
    }
    
    soc_sand_rv = soc_petra_ofp_rates_update_device_set(unit, FALSE);
    rv = handle_sand_result(soc_sand_rv);
    if (rv) {
        goto exit;
    }

    for (i = 0; i < dpp->pb->fap_ports.if_map_nof_entries; ++i) {
      mal_id = SOC_PB_IF2MAL_NDX(dpp->pb->fap_ports.if_map[i].conf.if_id);
      if (mal_id == SOC_PB_MAL_ID_NONE) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "ERROR: Mal ID is invalid (if_id=%d)"), dpp->pb->fap_ports.if_map[i].conf.if_id));
        rv =  SOC_E_INTERNAL;
        goto exit;
      }
      
      switch (mal_id) {
        case SOC_PB_MAL_ID_CPU:
          ndx = SOC_PB_NOF_MAC_LANES+0;
          rate = 1000000;
          break;
        case SOC_PB_MAL_ID_ERP:
          ndx = SOC_PB_NOF_MAC_LANES+1;
          rate = 1000000;
          break;
        case SOC_PB_MAL_ID_OLP:
          ndx = SOC_PB_NOF_MAC_LANES+2;
          rate = 1000000;
          break;
        case SOC_PB_MAL_ID_RCY:
          ndx = SOC_PB_NOF_MAC_LANES+3;
          rate = 1000000;
          break;
        default:
          ndx = mal_id;
          nif_type =                                                                                  
            soc_pb_nif_id2type(dpp->pb->fap_ports.if_map[i].conf.if_id);
          switch (nif_type) {
            case SOC_PB_NIF_TYPE_XAUI:
            case SOC_PB_NIF_TYPE_RXAUI:
            case SOC_PB_NIF_TYPE_ILKN:
              rate = 10000000;
              break;
            case SOC_PB_NIF_TYPE_SGMII:
            case SOC_PB_NIF_TYPE_QSGMII:
              rate = 1000000;
              break;
            default:       
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Unexpected interface type (%s) when mapping interface to port type\n"),
                         SOC_PB_NIF_TYPE_to_string(nif_type)));
                rv = SOC_E_INTERNAL;
                goto exit;
              break;
          }
      }
      mal_ids[ndx] = mal_id;        
      
      rates_tbl[ndx].rates[rates_tbl[ndx].nof_valid_entries].sch_rate = rate + (rate / 100) * 5; /* 5% speedup */
      rates_tbl[ndx].rates[rates_tbl[ndx].nof_valid_entries].egq_rate = rate + (rate / 100) * 1; /* 1% speedup */       
      rates_tbl[ndx].rates[rates_tbl[ndx].nof_valid_entries].port_id = dpp->pb->fap_ports.if_map[i].port_ndx;
      ++(rates_tbl[ndx].nof_valid_entries);
      shaper[ndx].sch_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
      shaper[ndx].egq_shaper.rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
    }

    for (ndx = 0; ndx < ndx_max; ++ndx) {
      if (rates_tbl[ndx].nof_valid_entries > 0) {
          soc_sand_rv = soc_petra_ofp_rates_set(unit, mal_ids[ndx], &shaper[ndx], &rates_tbl[ndx]);
          rv = handle_sand_result(soc_sand_rv);
          if (rv) {
              goto exit;
          }
      }
    }
    
    soc_sand_rv = soc_petra_ofp_rates_update_device_set(unit, TRUE);
    rv = handle_sand_result(soc_sand_rv);
    if (rv) {
        goto exit;
    }
  
exit:
    if (shaper) {
        sal_free(shaper);
    }
    if (rates_tbl) {
        sal_free(rates_tbl);
    }

    return rv;
}

int
soc_petra_allocate_tm_port_in_range_and_recycle_channel(int unit, int core, int max_port, int *tm_port, uint32 *channel)
{
    int port;
    int max = max_port >= SOC_DPP_DEFS_GET(unit, nof_logical_ports) ? SOC_DPP_DEFS_GET(unit, nof_logical_ports) - 1 : max_port;
    soc_dpp_port_map_t *port_map_p;
    uint8 channels[SOC_DPP_MAX_NOF_CHANNELS] = {0};
   
    /* loop over all local ports, marking used recycle channels, and finding the first free port */
    for (port = SOC_DPP_DEFS_GET(unit, nof_logical_ports) -1; port >= 0; --port) {
        port_map_p = &_port_map[unit][port];
        if (port_map_p->nif_id == SOC_TMC_IF_ID_RCY && port_map_p->channel < SOC_DPP_MAX_NOF_CHANNELS) {
            channels[port_map_p->channel] = 1;
        }
    }

    for (port = 0; port < SOC_DPP_MAX_NOF_CHANNELS && channels[port] ; ++port); /* find the first free channel */
    if (port >= SOC_DPP_MAX_NOF_CHANNELS) { /* if all channels are taken */
        return SOC_E_RESOURCE;
    }
    *channel = port;
      
    /* loop over local ports, finding the first free one */
    for (port = 0; port <= max; ++port) {
        port_map_p = &_port_map[unit][port];
        if (port_map_p->nif_id == SOC_TMC_NIF_ID_NONE && !port_map_p->channel &&
           !SOC_DPP_TM_PORT_IN_USE(unit, port) && !SOC_DPP_PP_PORT_IN_USE(unit, port)) {
#ifdef DBG_LOCAL_PORT
            LOG_CLI((BSL_META_U(unit,
                                "_soc_dpp_local_find_free_tm_port_and_allocate_in_range() allocated port %d with recycle channel %u\n"),
                     port, (unsigned) *channel ));
#endif
            *tm_port = port;
            port_map_p->tm_port = port;
            port_map_p->pp_port = port;
            port_map_p->nif_id = SOC_TMC_IF_ID_RCY;
            port_map_p->channel = *channel ;
            SOC_DPP_TM_PORT_RESERVE(unit, port);
            SOC_DPP_PP_PORT_RESERVE(unit, port);

            return SOC_E_NONE;
        }
    }
    return SOC_E_RESOURCE;
}

/* deallocate a (non internal) used tm port and its recycle channel */
int
soc_petra_free_tm_port_and_recycle_channel(int unit, int port)
{
    soc_dpp_port_map_t *port_map_p = &_port_map[unit][port];
    if (port < 0 || port >= SOC_DPP_DEFS_GET(unit, nof_logical_ports)) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "invalid port(%d)\n"), port));
        return SOC_E_PARAM;
    }
    if (port_map_p->nif_id != SOC_TMC_IF_ID_RCY || port_map_p->tm_port != port || port_map_p->pp_port != port ||
        !SOC_DPP_TM_PORT_IN_USE(unit, port) || !SOC_DPP_PP_PORT_IN_USE(unit, port) ) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "port(%d) not  allocated by _soc_dpp_local_find_free_tm_port_and_allocate_in_range()\n"), port));
        return SOC_E_FAIL;
    }
   
#ifdef DBG_LOCAL_PORT
    LOG_CLI((BSL_META_U(unit,
                        "_soc_dpp_free_local_tm_port() freeing port %d with recycle channel %u\n"), port, (unsigned)port_map_p->channel));
#endif
    _port_map[unit][port].tm_port = _SOC_DPP_PORT_INVALID;
    _port_map[unit][port].pp_port = _SOC_DPP_PORT_INVALID;
            port_map_p->nif_id = SOC_TMC_NIF_ID_NONE;
            port_map_p->channel = 0 ;
    SOC_DPP_TM_PORT_FREE(unit, port);
    return SOC_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME



