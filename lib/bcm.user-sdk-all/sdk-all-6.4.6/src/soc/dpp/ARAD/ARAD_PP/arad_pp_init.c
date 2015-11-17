
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_init.c,v 1.55 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_ac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_qos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_extend_p2p.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fec.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_table.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_sa_auth.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_trap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_vid_assign.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/JER/JER_PP/jer_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mymac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_port.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vsi.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_slb.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vtt.h>
#include <soc/dpp/JER/jer_init.h>

#include <soc/dpp/drv.h>

#include <shared/swstate/access/sw_state_access.h>

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


/*
 * VSI for p2p, used for
 * - AC P2P to AC - no lookup
 * - AC P2P to PWE - no lookup
 * - AC P2P to PBB - no lookup
 *  - needed as all P2P services has the same VSI value
 *  - has no usage, and
 *  - VSI (1) can still be used for MP
 */

/* 3 Msb of destination encoding for system port in ARAD */
#define ARAD_PP_DESTINATION_SYSTEM_PORT_PREFIX  0x4
/* } */
/*************
 *  MACROS   *
 *************/
/* { */



#define ARAD_PP_INIT_PRINT_ADVANCE(str, phase_id)			\
          do                                                                              \
          {                                                                               \
            if (!silent)                                                                  \
            {                                                                             \
              LOG_VERBOSE(BSL_LS_SOC_INIT, \
                          (BSL_META_U(unit, \
                                      "    + Phase %u, %.2u: %s\n\r"), phase_id, ++stage_id, str)); \
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

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_init[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_INIT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_INIT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_FUNCTIONAL_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_HW_SET_DEFAULTS),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE1),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE2),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE1_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE2_VERIFY),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_init[] =
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

/* Fill FTMH header data in sw db for parsing in interrupt context */
STATIC uint32
  arad_pp_mgmt_sw_dbs_ftmh_header_set(
    SOC_SAND_IN  int                 unit
  )
{
  soc_error_t
    rv = SOC_E_NONE;
  uint32 reg_val;
  uint8 ftmh_lb_key_enable, ftmh_stacking_ext_mode, system_headers_mode;
  soc_reg_t
    global_f = SOC_IS_JERICHO(unit)? ECI_GLOBAL_SYS_HEADER_CFGr: ECI_GLOBALFr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* LB mode */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(rv,  10,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_get(unit, global_f, REG_PORT_ANY, 0, &reg_val));
  ARAD_FLD_FROM_REG(global_f, FTMH_LB_KEY_EXT_ENf, ftmh_lb_key_enable, reg_val, 20, exit);
  rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.ftmh_lb_key_ext_en.set(unit, ftmh_lb_key_enable);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);

  /* Stacking mode */
  ARAD_FLD_FROM_REG(global_f, FTMH_STACKING_EXT_ENABLEf, ftmh_stacking_ext_mode, reg_val, 50, exit);
  rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.ftmh_stacking_ext_enable.set(unit, ftmh_stacking_ext_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 60, exit);

  /* System headers mode */
  ARAD_FLD_FROM_REG(global_f, SYSTEM_HEADERS_MODEf, system_headers_mode, reg_val, 80, exit);
  rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.system_headers_mode.set(unit, system_headers_mode);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_sw_dbs_ftmh_header_set()",0,0);
}


/*********************************************************************
*     Initialize roo application
*********************************************************************/
uint32 arad_mgmt_roo_init(
    SOC_SAND_IN  int                                unit)
{
    uint32
        reg_val, 
        fld_val; 
    uint64 
        reg_val_64;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_ARDON(unit) && SOC_IS_ARADPLUS(unit)) 
    {
        /* roo: skip ethernet feature: enable skip ethernet feature.
         * In roo, arad+, only way to terminate the inner ethernet
         * Only for Arad+.
         * For Jericho, use second my-mac termination */
        if (SOC_IS_ARADPLUS_A0(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IHP_VTT_GENERAL_CONFIGS_0r, SOC_CORE_ALL, 0, FIELD_31_31f, 1)); /* EnableIpSkipEthernet is an internal field. */
        }

        /* roo: used to check that the LL is of type data-entry.
           In this field, save the expected data-entry type of the LL */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_CFG_DC_OVERLAYr, SOC_CORE_ALL, 0, CFG_DATA_ENTRY_TYPE_IS_LL_ETHERNETf,  ARAD_PRGE_DATA_ENTRY_LSBS_ROO_VXLAN));

        SOCDNX_IF_ERR_EXIT(READ_IHP_VTT_GENERAL_CONFIGS_1r(unit, REG_PORT_ANY, &reg_val));
        soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, &reg_val, SKIP_ETHERNET_UPDATE_IN_RIFf, 1);
        soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, &reg_val, ENABLE_TUNNEL_TERMINATION_CODE_UPGRADESf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_IHP_VTT_GENERAL_CONFIGS_1r(unit, REG_PORT_ANY, reg_val));

        /* roo: enable chicken bits. LBP
         * Only for Arad+.
         * For Jericho, stamping is determined according to PPH-VSI-Source table. 
         */
        if (SOC_IS_ARADPLUS_A0(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_0r, SOC_CORE_ALL, 0, ENABLE_NATIVE_VSI_STAMPINGf, 1));
        }

        /* roo: enable chiken bits. Forwarding lookup */
        SOCDNX_IF_ERR_EXIT(READ_IHP_FLP_GENERAL_CFGr_REG64(unit, SOC_CORE_ALL, &reg_val_64));
        fld_val = 0x1; 
        soc_reg64_field32_set(unit, IHP_FLP_GENERAL_CFGr, &reg_val_64, ENABLE_MACT_FORMAT_3f, fld_val);
        soc_reg64_field32_set(unit, IHP_FLP_GENERAL_CFGr, &reg_val_64, ENABLE_INNER_COMPATIBLE_MCf, fld_val);

        /* for jericho */
        if (SOC_IS_JERICHO(unit)) 
        {
            /* forwarding registers */

            /* eei filling:
             * use register to fill eei. (eei to build the native ethernet header).
             * native-arp: Truncated entry from the host table (format 3b).  15b instead of 18b.
             *             Full entry: MactFormat3EeiBits[2:1] Native-ARP(15) MactFormat3EeiBits[0]
             * native arp pointer lsb is always 0 (to get full entry in eedb): MactFormat3EeiBits[0]             */ 


            /* get 2 msbs for the arp pointer from soc property.*/
            fld_val = SOC_ROO_HOST_ARP_MSBS(unit); 
            /* arp pointer 2 msbs are saved in MactFormat3EeiBits[2:1].
             * MactFormat3EeiBits[0] = 0 */
            fld_val = (fld_val << 1); 
            soc_reg64_field32_set(unit, IHP_FLP_GENERAL_CFGr, &reg_val_64, MACT_FORMAT_3_EEI_BITSf, fld_val);

            /* fec filling:
             * use register to fill fec.
             * Fec: Truncated entry from the host table (format 3b).
             *      Full entry: MactFormat3FecMsbBits(5) Fec(12)   .
             * Requirement: MactFormat3FecMsbBits = 0 */
            fld_val =  0; 
            soc_reg64_field32_set(unit, IHP_FLP_GENERAL_CFGr, &reg_val_64, MACT_FORMAT_3_FEC_MSB_BITSf, fld_val);
        }

        SOCDNX_IF_ERR_EXIT(WRITE_IHP_FLP_GENERAL_CFGr_REG64(unit, SOC_CORE_ALL, reg_val_64));


    }

exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC uint32
  arad_pp_mgmt_functional_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE1_CONF    *conf,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
    int core;
  soc_error_t new_res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_FUNCTIONAL_INIT);


  res = arad_pp_dbal_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_lem_access_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  res = arad_pp_eg_ac_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_eg_encap_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_pp_eg_filter_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = arad_pp_eg_qos_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = arad_pp_eg_vlan_edit_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = arad_pp_frwrd_ilm_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);


#if defined(INCLUDE_L3)
  if(ARAD_PP_L3_ROUTING_ENABLED(unit))
  {
      res = arad_pp_frwrd_fec_init_unsafe(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      res = arad_pp_frwrd_ipv4_init_unsafe(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }
#endif  /* INCLUDE_L3 */

  res = arad_pp_frwrd_mact_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  res = arad_pp_frwrd_mact_mgmt_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  res = arad_pp_lag_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  res = arad_pp_lif_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  new_res = arad_pp_lif_cos_init_unsafe(unit);
  if (new_res != SOC_E_NONE) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 210, exit);
  }

  if (SOC_DPP_CONFIG(unit)->trill.mode) {
    res = arad_pp_frwrd_trill_init_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  }

  res = arad_pp_lif_ing_vlan_edit_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

  res = arad_pp_lif_table_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

    res = arad_pp_llp_cos_init_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

  res = arad_pp_llp_filter_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

  res = arad_pp_llp_parse_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);

  res = arad_pp_llp_sa_auth_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

  res = arad_pp_llp_trap_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);

  res = arad_pp_llp_vid_assign_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);

  res = arad_pp_mpls_term_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);

  res = arad_pp_port_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);

  res = arad_pp_rif_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

  if (SOC_IS_JERICHO(unit)){
	  res = jer_pp_metering_init(unit);
  }
  else{
	  res = arad_pp_metering_init_unsafe(unit); 
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);

  res = arad_pp_mymac_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);

  if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable == TRUE) {
      /* Already called if TM */
      res = arad_pp_vtt_init(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);  
  }

  if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable == TRUE) {
    /* Already called if TM */
    res = arad_pp_trap_mgmt_init_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);
  }

  BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
    res = arad_pp_diag_init_unsafe(unit, core);
    SOC_SAND_CHECK_FUNC_RESULT(res, 405, exit);
  }
  res = arad_pp_vsi_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);

  res = arad_pp_fp_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 420, exit);



  
  res = arad_pp_diag_sample_enable_set_unsafe(unit, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);

  res = arad_pp_mgmt_sw_dbs_ftmh_header_set(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 440, exit);

  if (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE)
  {
    res = arad_pp_src_bind_init_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    uint8 enable_slb = soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0) == 1 ? TRUE : FALSE;
  
    res = arad_pp_slb_init_unsafe(unit, enable_slb);
    SOC_SAND_CHECK_FUNC_RESULT(res, 460, exit);
  }
#endif /* BCM_88660_A0 */

#ifdef BCM_88675
  /* Perform module initialization that is specific for Jericho and above */
  if (SOC_IS_JERICHO(unit)) {
      res = soc_jer_pp_mgmt_functional_init(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 470, exit);
  }
#endif /* BCM_88675 */  
  if (SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable)
  {
    res = arad_pp_ip6_compression_tcam_init_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 480, exit);
  }

  /* ROO init configuration */
  res = arad_mgmt_roo_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_functional_init_unsafe()",0,0);
}



STATIC uint32
  arad_pp_mgmt_hw_set_defaults(
    SOC_SAND_IN  int                 unit
  )
{
 uint32
   fld_val,
   nof_vrfs,
   vrf_bits,
   bits_per_depth,
   dpth_indx,
   res,
   sa_prefix_for_key_construction,
   reg_val;
 uint8
   mac_in_mac_enabled;
 ARAD_PP_IPV4_LPM_MNGR_INFO
   lpm_mngr;
 ARAD_PP_MGMT_OPERATION_MODE
   *oper_mode = NULL;
 soc_reg_above_64_val_t
     fld_value,
     reg_above64_val;
 soc_reg_t
     disable_pp_reg;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_HW_SET_DEFAULTS);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_REG_ABOVE_64_CLEAR(fld_value);

  /* set pp as enabled */
  disable_pp_reg = SOC_IS_JERICHO(unit)? ECI_GLOBAL_PP_7r: ECI_GLOBAL_1r;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, soc_reg32_get(unit, disable_pp_reg, REG_PORT_ANY,  0, &reg_val));
  fld_val = 0;
  soc_reg_field_set(unit, disable_pp_reg, &reg_val, DISABLE_PPf, fld_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_set(unit, disable_pp_reg, REG_PORT_ANY,  0, reg_val));

  if(ARAD_PP_L3_ROUTING_ENABLED(unit) && SOC_IS_ARADPLUS_AND_BELOW(unit)){
      /* In Jericho, do not use the VRF logic */
      res = arad_pp_sw_db_ipv4_nof_vrfs_get(unit,&nof_vrfs);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else{
      nof_vrfs = 0;
  }

  ARAD_ALLOC(oper_mode, ARAD_PP_MGMT_OPERATION_MODE, 1, "arad_pp_mgmt_hw_set_defaults.oper_mode");
  res = arad_pp_mgmt_operation_mode_get_unsafe(
          unit,
          oper_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  reg_val = 0;
  if (SOC_IS_JERICHO(unit)) {
    /* fix destination encoding for learned system ports to be as in ARAD */
    soc_reg_field_set(unit, ECI_GLOBAL_PP_8r, &reg_val, SRC_SYSTEM_PORT_ID_PREFIXf, ARAD_PP_DESTINATION_SYSTEM_PORT_PREFIX);
  }
  /* set MIM VSI*/
  soc_reg_field_set(unit, ECI_GLOBAL_PP_8r, &reg_val, MAC_IN_MAC_VSIf, oper_mode->p2p_info.mim_vsi);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_ECI_GLOBAL_PP_8r(unit,reg_val));
  /* WRITE_EGQ_REG_00DAr(unit, REG_PORT_ANY,fld_val);
  WRITE_EPNI_REG_00DAr(unit, REG_PORT_ANY,fld_val); */

  /* set ether type  for MPLS*/
  /* for ingress parsing*/
  reg_val = 0;
  soc_reg_field_set(unit, ECI_GLOBAL_PP_6r, &reg_val, ETHERTYPE_MPLS_0f, oper_mode->mpls_info.mpls_ether_types[0]);
  soc_reg_field_set(unit, ECI_GLOBAL_PP_6r, &reg_val, ETHERTYPE_MPLS_1f, oper_mode->mpls_info.mpls_ether_types[1]);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_ECI_GLOBAL_PP_6r(unit,reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_EGQ_ETHERNET_TYPESr(unit, REG_PORT_ANY, reg_above64_val));
  /* Set the EGQ MPLS EtherType */
  fld_value[0] = oper_mode->mpls_info.mpls_ether_types[0];
  ARAD_FLD_TO_REG_ABOVE_64(EGQ_ETHERNET_TYPESr, ETH_TYPE_0f, fld_value, reg_above64_val, 70, exit);
  fld_value[0] = oper_mode->mpls_info.mpls_ether_types[1];
  ARAD_FLD_TO_REG_ABOVE_64(EGQ_ETHERNET_TYPESr, ETH_TYPE_1f, fld_value, reg_above64_val, 80, exit);
  /* Set CNM default EtherType */
  fld_value[0] = 0x22E7;
  ARAD_FLD_TO_REG_ABOVE_64(EGQ_ETHERNET_TYPESr, ETH_TYPE_2f, fld_value, reg_above64_val, 85, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, WRITE_EGQ_ETHERNET_TYPESr(unit, REG_PORT_ANY, reg_above64_val));



  /* Set Service-type For PWE P2P and MP */
  reg_val = 0;
  soc_reg_field_set(unit, IHP_SERVICE_TYPE_VALUESr, &reg_val, SERVICE_TYPE_LABEL_PWE_P2Pf, 0x0);
  soc_reg_field_set(unit, IHP_SERVICE_TYPE_VALUESr, &reg_val, SERVICE_TYPE_LABEL_PWE_MPf, 0x3);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_IHP_SERVICE_TYPE_VALUESr(unit, REG_PORT_ANY,reg_val));

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) { /* no such fields in Jericho */
      /* Set Parser leaf context action with no update in case of upper protocol over MPLS */
      reg_val = 0;
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit,READ_IHP_VTT_GENERAL_CONFIGS_0r(unit, REG_PORT_ANY, &reg_val));
      soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_0r, &reg_val, IPV4_OVER_MPLS_PARSER_LEAF_CONTEXT_MASKf, 0xF);
      soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_0r, &reg_val, IPV6_OVER_MPLS_PARSER_LEAF_CONTEXT_MASKf, 0xF);
      soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_0r, &reg_val, ETH_OVER_PWE_PARSER_LEAF_CONTEXT_MASKf , 0xF);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit,WRITE_IHP_VTT_GENERAL_CONFIGS_0r(unit, REG_PORT_ANY,reg_val));
  }


  /* set decrement TTL 
    0x0 - ACTION_TYPE_DATA
    0x1 - ACTION_TYPE_MPLS
    0x2 - ACTION_TYPE_IP_TUNNEL
    0x3 - ACTION_TYPE_ISID
    0x4 - ACTION_TYPE_OUT_RIF
    0x5 - ACTION_TYPE_TRILL
    0x6 - ACTION_TYPE_LL_ARP
    0x7 - ACTION_TYPE_AC
  */
  fld_val = 0x56; /* out-rif, LL, IP, MPLS*/
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 130, exit, WRITE_EPNI_TTL_DECREMENT_ENABLEr(unit, REG_PORT_ANY,fld_val));

  /* ipv4 setting */
  /* init number of VRFs for LPM lookup*/
  if(nof_vrfs > 0)
  {
    vrf_bits = soc_sand_log2_round_up(nof_vrfs);
    reg_val = 0;

    res = arad_pp_sw_db_ipv4_lpm_mngr_get(
            unit,
            &lpm_mngr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);


    for (dpth_indx = 0 ; dpth_indx < 4; ++dpth_indx)
    {
      bits_per_depth = lpm_mngr.init_info.nof_bits_per_bank[dpth_indx+1];
      reg_val = reg_val | (bits_per_depth<<(dpth_indx*3));
    }
    soc_reg_field_set(unit,IHB_LPM_QUERY_CONFIGURATIONr,&reg_val, VRF_DEPTHf, vrf_bits);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, WRITE_IHB_LPM_QUERY_CONFIGURATIONr(unit,reg_val));
  }

  sa_prefix_for_key_construction = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_SA_AUTH;
  if (SOC_IS_JERICHO(unit)) {
      /* SA authentication has a 10 bit prefix. Push the 4 bits value to the MSBs of the 10 bits value */
      sa_prefix_for_key_construction = sa_prefix_for_key_construction << 6;
  }

  /* Set sa-based prefix */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_LINK_LAYER_LOOKUP_CFGr, SOC_CORE_ALL, 0, SA_BASED_VID_PREFIXf,  sa_prefix_for_key_construction));
  
  /* if mac in mac is enabled, set bit 7 */  
  res = arad_pp_is_mac_in_mac_enabled(
      unit,
      &mac_in_mac_enabled
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  /*
  * set AUX mode at egress
  */
  if (oper_mode->split_horizon_filter_enable)
  {
    fld_val = 1; /* Split Horizon */
  }
  else if (mac_in_mac_enabled)
  {
    fld_val = 2; /* Mac in Mac (Provider Backbone Bridging) */
  }
  else
  {
    fld_val = 0; /* Private VLAN */
  } 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_AUXILIARY_DATA_TABLEr, SOC_CORE_ALL, 0, AUXILIARY_DATA_TABLE_MODEf,  fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_AUXILIARY_DATA_TABLEr, SOC_CORE_ALL, 0, AUXILIARY_DATA_TABLE_MODEf,  fld_val));
#ifdef BCM_JERICHO_SUPPORT 
  if (SOC_IS_JERICHO(unit))  {
    /* The port default VLAN translation Advanced mode is enabed by default. */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 200, exit,READ_IHP_VTT_GENERAL_CONFIGS_0r(unit, REG_PORT_ANY, &reg_val));
    soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_0r, &reg_val, FORCE_VEC_FROM_LIFf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 210, exit,WRITE_IHP_VTT_GENERAL_CONFIGS_0r(unit, REG_PORT_ANY,reg_val));

    /* Configure number of cycles between lookup to reply in KAPS */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  220,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHP_FLP_GENERAL_CFGr, REG_PORT_ANY, 0, FLP_KAPS_DELAY_IS_EVENf,  1));
  }
#endif
exit:
  ARAD_FREE(oper_mode);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_hw_set_defaults()",0,0);
}
/*********************************************************************
*     Initialize the device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE1_CONF    *conf,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;
  uint32
    stage_id = 0;
  soc_error_t
      new_rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(conf);

  if (!silent)
  {
    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "   Phase 1 : PP initialization: device %u\n\r"), unit));
  }

  /* To be removed once TM init is set in the emulation */
  if (SOC_IS_JERICHO(unit)) {
      new_rv = arad_tbl_access_init_unsafe(unit);
      if (new_rv != SOC_E_NONE) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 55555, exit);
      }
  }

  /************************************************************************/
  /* 1. Per functional module, perform initializations                    */
  /* covered by module's functionality.                                   */
  /************************************************************************/
  res = arad_pp_mgmt_functional_init_unsafe(
          unit,
          conf,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  ARAD_PP_INIT_PRINT_ADVANCE("arad_pp_mgmt_functional_init", 1);

  /************************************************************************/
  /* 2. Set registers not covered in any functional module,               */
  /* with default values different from hardware defaults                 */
  /************************************************************************/
  res = arad_pp_mgmt_hw_set_defaults(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  ARAD_PP_INIT_PRINT_ADVANCE("arad_pp_mgmt_hw_set_defaults", 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_init_sequence_phase1_unsafe()", 0, 0);
}

uint32
  arad_pp_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE1_CONF    *conf
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE1_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(conf);
  SOC_SAND_MAGIC_NUM_VERIFY(conf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_init_sequence_phase1_verify()", 0, 0);
}

/*********************************************************************
*     Out-of-reset sequence. Enable/Disable the device from
*     receiving and transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE2_CONF    *conf,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_init_sequence_phase2_unsafe()", 0, 0);
}

uint32
  arad_pp_mgmt_init_sequence_phase2_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE2_CONF    *conf
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_INIT_SEQUENCE_PHASE2_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_init_sequence_phase2_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
*     arad_pp_api_init module.
*     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_init_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_init;
}

/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_init module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_init_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_init;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */


