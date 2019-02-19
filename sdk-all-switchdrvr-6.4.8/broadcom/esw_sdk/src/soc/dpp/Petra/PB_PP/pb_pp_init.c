/* $Id: pb_pp_init.c,v 1.10 Broadcom SDK $
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
*/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_init.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_init_tbl.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_ac.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_encap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_qos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_vlan_edit.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_bmact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_extend_p2p.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_fec.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ilm.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_trill.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_init.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lag.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_table.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_sa_auth.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_trap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_vid_assign.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_metering.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mpls_term.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mymac.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_port.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_rif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_svem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_vsi.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* default setting for forwarding enable:
 * - AC P2P to AC - no lookup
 * - AC P2P to PWE - no lookup
 * - AC P2P to PBB - no lookup
 * - AC MP - lookup
 * - ISID P2P - no lookup
 * - ISID MP - lookup
 * - TRILL, IP-Tunnel, Label-PWE-MP, Label-VRF, Label-LSP -lookup
 * - lABEL-PWE-P2P - no lookup
 */

#define SOC_PB_PP_IHP_FLP_GENERAL_CFG_REG_DEFAULT_VAL 0x68 /* 8'b0110_1000  */

/*
 * VSI for p2p, used for
 * - AC P2P to AC - no lookup
 * - AC P2P to PWE - no lookup
 * - AC P2P to PBB - no lookup
 *  - needed as all P2P services has the same VSI value
 *  - has no usage, and
 *  - VSI (1) can still be used for MP
 */
#define SOC_PB_PP_IHP_P2P_VSI (1)

/* } */
/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PB_PP_INIT_PRINT_ADVANCE(str, phase_id)                                           \
          do                                                                              \
          {                                                                               \
            if (!silent)                                                                  \
            {                                                                             \
              soc_sand_os_printf("    + Phase %u, %.2u: %s\n\r", phase_id, ++stage_id, str);  \
            }                                                                             \
          }while(0)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_init[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_INIT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_INIT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_FUNCTIONAL_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_HW_SET_DEFAULTS),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE1),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE2),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE1_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE2_VERIFY),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_init[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC uint32
  soc_pb_pp_mgmt_functional_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_INIT_PHASE1_CONF    *conf,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_FUNCTIONAL_INIT);

  res = soc_pb_pp_lem_access_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_eg_ac_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_eg_encap_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_eg_filter_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_pb_pp_eg_mirror_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_pb_pp_eg_qos_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_pb_pp_eg_vlan_edit_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  
  res = soc_pb_pp_frwrd_extend_p2p_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_pb_pp_frwrd_fec_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_pb_pp_frwrd_ilm_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  res = soc_pb_pp_frwrd_ipv4_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  res = soc_pb_pp_frwrd_ipv6_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  res = soc_pb_pp_frwrd_mact_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  res = soc_pb_pp_frwrd_mact_mgmt_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  res = soc_pb_pp_frwrd_trill_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  res = soc_pb_pp_lag_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  res = soc_pb_pp_lif_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  res = soc_pb_pp_lif_cos_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

  res = soc_pb_pp_lif_ing_vlan_edit_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

  res = soc_pb_pp_lif_table_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

  res = soc_pb_pp_llp_cos_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

  res = soc_pb_pp_llp_filter_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

  res = soc_pb_pp_llp_mirror_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

  res = soc_pb_pp_llp_parse_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);

  res = soc_pb_pp_llp_sa_auth_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

  res = soc_pb_pp_llp_trap_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);

  res = soc_pb_pp_llp_vid_assign_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);

  res = soc_pb_pp_metering_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);

  res = soc_pb_pp_mpls_term_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);

  res = soc_pb_pp_mymac_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);

  res = soc_pb_pp_port_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);

  res = soc_pb_pp_rif_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

  res = soc_pb_pp_isem_access_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

  res = soc_pb_pp_svem_access_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 380, exit);

  res = soc_pb_pp_trap_mgmt_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);

  res = soc_pb_pp_vsi_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);

  res = soc_pb_pp_fp_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 420, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_functional_init_unsafe()",0,0);
}

STATIC uint32
  soc_pb_pp_mgmt_hw_set_defaults(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    nof_vrfs,
    vrf_bits,
    nof_bits,
    bits_per_depth,
    dpth_indx,
    reg_val;
  uint8
	mac_in_mac_enabled,
    is_petra_rev_a_in_system,
    add_pph_eep_ext;
  SOC_PB_PP_MGMT_OPERATION_MODE
    oper_mode;
  SOC_PB_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_REGS
    *tm_regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_HW_SET_DEFAULTS);

  regs = soc_pb_pp_regs();
  tm_regs = soc_pb_regs();

  res = soc_pb_pp_mgmt_operation_mode_get_unsafe(
          unit,
          &oper_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
  /*
   * PPH mode, EEP extension, if Soc_petra-A in system then no EEP extension and set PA-compatible bit
   */
  is_petra_rev_a_in_system = soc_petra_sw_db_is_petra_rev_a_in_system_get(unit);
  add_pph_eep_ext = soc_petra_sw_db_add_pph_eep_ext_get(unit);
  SOC_PB_PP_REG_GET(regs->eci.system_headers_configurations0_reg, reg_val, 20, exit);
  fld_val = SOC_SAND_BOOL2NUM(is_petra_rev_a_in_system);
  SOC_PB_PP_FLD_TO_REG(regs->eci.system_headers_configurations0_reg.pph_petra_a_compatible, fld_val, reg_val, 30, exit);
  fld_val = SOC_SAND_BOOL2NUM((!is_petra_rev_a_in_system) && add_pph_eep_ext);
  SOC_PB_PP_FLD_TO_REG(regs->eci.system_headers_configurations0_reg.add_pph_eep_ext, fld_val, reg_val, 40, exit);
  SOC_PB_PP_REG_SET(regs->eci.system_headers_configurations0_reg, reg_val, 30, exit);

  /* set default setting for forwarding enable */
  fld_val = SOC_PB_PP_IHP_FLP_GENERAL_CFG_REG_DEFAULT_VAL;
  /* if mac in mac is enabled, set bit 7 */  
  res = soc_pb_pp_is_mac_in_mac_enabled(
	  unit,
	  &mac_in_mac_enabled
	  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (mac_in_mac_enabled)
	  fld_val |= 0x80;
  SOC_PB_PP_FLD_SET(regs->ihb.flp_general_cfg_reg.enable_forwarding_lookup, fld_val, 20, exit);

  /* set ether type  for MPLS*/
  reg_val = oper_mode.mpls_ether_types[1] << SOC_SAND_PP_ETHER_TYPE_NOF_BITS | oper_mode.mpls_ether_types[0];
  SOC_PB_IMPLICIT_REG_SET(tm_regs->eci.egress_pp_configuration_reg[0], reg_val, 60, exit);
  /* for ingress parsing*/
  reg_val = oper_mode.mpls_ether_types[0];
  SOC_PB_PP_REG_SET(regs->eci.ethernet_type_mpls_configuration_reg_reg,reg_val,5,exit);
  /* set TTL decrement enable at egress*/
  fld_val = 1;
  SOC_PB_PP_FLD_SET(regs->epni.ttl_decrement_enable_reg.ttl_decrement_enable,fld_val,5,exit);
  fld_val = 1;
  SOC_PB_PP_FLD_SET(regs->epni.ttl_decrement_enable_reg.ttl_tunnel_disable,fld_val,5,exit);

  /* init number of VRFs for LPM lookup*/
  res = soc_pb_pp_sw_db_ipv4_nof_vrfs_get(unit,&nof_vrfs);
  /* init only if routing is supported*/
  if (nof_vrfs > 0)
  {
    vrf_bits = soc_sand_log2_round_up(nof_vrfs);
    nof_bits = 32 - (12 - vrf_bits);
    reg_val = 0;

    res = soc_pb_pp_sw_db_ipv4_lpm_mngr_get(
            unit,
            &lpm_mngr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


    for (dpth_indx = 0 ; dpth_indx < 4; ++dpth_indx)
    {
      bits_per_depth = SOC_SAND_DIV_ROUND_UP(nof_bits, 5);
      bits_per_depth = lpm_mngr.init_info.nof_bits_per_bank[dpth_indx+1];
      reg_val = reg_val | (bits_per_depth<<(dpth_indx*3));
    }
    SOC_PB_PP_REG_SET(regs->ihb.lpm_query_configuration_reg,reg_val,5,exit);
    SOC_PB_PP_FLD_SET(regs->ihb.lpm_query_configuration_reg.vrf_depth,vrf_bits,5,exit);
  }
 /*
  * set SA lookup type:
  *
  */
  if (oper_mode.authentication_enable)
  {
	  fld_val = 1;
	  reg_val = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH <<
		  (SOC_DPP_LEM_WIDTH_PETRAB -
		  SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SA_AUTH -
		  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH);
	  SOC_PB_PP_REG_SET(regs->ihp.link_layer_lookup_cfg_reg,reg_val,5,exit);
  }
  else
  {
	  /*
       * check if mac in mac is enabled. 
       */
      if (mac_in_mac_enabled)
	  {
		fld_val = 0;
	  }
	  else
	  {
		fld_val = 2;
	  }
  }
  SOC_PB_PP_FLD_SET(regs->eci.sa_lookup_type_reg.sa_lookup_type, fld_val, 150, exit);

  
  /*
   *  Set MPLS termination PMF profiles
   */
  fld_val = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_PP;
  SOC_PB_PP_FLD_SET(regs->ihp.vtt_general_configs_reg.eth_over_pwe_parser_pmf_profile, fld_val, 159, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.lsr_action_profile_reg.parser_pmf_profile_eth_over_pwe, fld_val, 160, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.vtt_general_configs_reg.ipv4_over_mpls_parser_pmf_profile, fld_val, 161, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.vtt_general_configs_reg.ipv6_over_mpls_parser_pmf_profile, fld_val, 162, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.vtt_general_configs_reg.my_bmac_parser_pmf_profile, fld_val, 163, exit);

 /*
  * extend Host EEP - different call if Soc_petra-B B0
  */
  fld_val = SOC_SAND_BOOL2NUM_INVERSE(oper_mode.ipv4_info.ipv4_host_extend);
  if (SOC_PB_REV_ABOVE_A1)
  {
    SOC_PB_PP_FLD_SET(regs->ihb.dc_app_reg_b0.enable_outlif_1, fld_val, 169, exit);
  }
  else
  {
    SOC_PB_PP_FLD_SET(regs->ihb.dc_app_reg.enable_outlif_1, fld_val, 170, exit);
  }

/*
 * for IP packet enable hw extension of outlif from 8K to 16K in the FEC table
 */
  fld_val = 1;
  SOC_PB_PP_FLD_SET(regs->ihb.dc_app_reg.dc_app, fld_val, 170, exit);

/*
 * set P2P VSI
 */
  fld_val = SOC_PB_PP_IHP_P2P_VSI;
  SOC_PB_PP_FLD_SET(regs->ihp.vsi_values0_reg.ac_p2p_to_ac_vsi, fld_val, 180, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.vsi_values0_reg.ac_p2p_to_pwe_vsi, fld_val, 190, exit);

  SOC_PB_PP_FLD_SET(regs->ihp.vsi_values1_reg.label_pwe_p2p_vsi, fld_val, 200, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.vsi_values1_reg.trill_default_vsi, fld_val, 210, exit);

 /*
  * set P2P ISID VSI, to be different than zero (cause illegal value)
  * when packet is tunneled VSI always considerer as zero, cause not-important
  */
  fld_val = oper_mode.p2p_info.mim_vsi;
  SOC_PB_PP_FLD_SET(regs->epni.general_pp_config_reg.p2p_vsi,fld_val,220,exit);
  SOC_PB_PP_FLD_SET(regs->eci.mac_in_mac_vsi_reg.mac_in_mac_vsi,fld_val,230,exit);

 /*
  * set AUX mode at egress
  */
  if (oper_mode.split_horizon_filter_enable)
  {
    fld_val = 1; /* Split Horizon */
  }
  else if(mac_in_mac_enabled)
  {
    fld_val = 2; /* Mac in Mac (Provider Backbone Bridging) */
  }
  else
  {
    fld_val = 0; /* Private VLAN */
  } 
  SOC_PB_PP_FLD_SET(regs->egq.ehpgeneral_settings_reg.auxiliary_data_table_mode,fld_val,240,exit);
  SOC_PB_PP_FLD_SET(regs->epni.general_pp_config_reg.auxiliary_data_table_mode,fld_val,250,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_hw_set_defaults()",0,0);
}

/*********************************************************************
*     Initialize the device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_INIT_PHASE1_CONF    *conf,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;
  uint32
    stage_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(conf);

  if (!silent)
  {
    soc_sand_os_printf("   Phase 1 : PP initialization: device %u\n\r", unit);
  }

  /************************************************************************/
  /* 1. Per functional module, perform initializations                    */
  /* covered by module's functionality.                                   */
  /************************************************************************/
  res = soc_pb_pp_mgmt_functional_init_unsafe(
          unit,
          conf,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  SOC_PB_PP_INIT_PRINT_ADVANCE("soc_pb_pp_mgmt_functional_init", 1);

  /************************************************************************/
  /* 2. Set registers not covered in any functional module,               */
  /* with default values different from hardware defaults                 */
  /************************************************************************/
  res = soc_pb_pp_mgmt_hw_set_defaults(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  SOC_PB_PP_INIT_PRINT_ADVANCE("soc_pb_pp_mgmt_hw_set_defaults", 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_init_sequence_phase1_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_INIT_PHASE1_CONF    *conf
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE1_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(conf);
  SOC_SAND_MAGIC_NUM_VERIFY(conf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_init_sequence_phase1_verify()", 0, 0);
}

/*********************************************************************
*     Out-of-reset sequence. Enable/Disable the device from
*     receiving and transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_INIT_PHASE2_CONF    *conf,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_init_sequence_phase2_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mgmt_init_sequence_phase2_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_INIT_PHASE2_CONF    *conf
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_INIT_SEQUENCE_PHASE2_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_init_sequence_phase2_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
*     soc_pb_pp_api_init module.
*     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_init_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_init;
}

/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_init module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_init_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_init;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

