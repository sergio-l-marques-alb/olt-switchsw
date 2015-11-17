/* $Id: pb_egr_acl.c,v 1.8 Broadcom SDK $
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
*/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_acl.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>

#include <soc/dpp/Petra/petra_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_EGR_ACL_CODE_NDX_MIN                                (1)
#define SOC_PB_EGR_ACL_CODE_NDX_MAX                                (15)
#define SOC_PB_EGR_ACL_PROTOCOL_CODE_MAX                           (255)
#define SOC_PB_EGR_ACL_PP_PORT_NDX_MAX                             (SOC_PB_PORT_NOF_PP_PORTS - 1)
#define SOC_PB_EGR_ACL_FWD_TYPE_MAX                                (0xe)
#define SOC_PB_EGR_ACL_ACL_PROFILE_NDX_MAX                         (3)
#define SOC_PB_EGR_ACL_KEY_PROFILE_MAX                             (7)
#define SOC_PB_EGR_ACL_DB_NDX_MAX                                  (SOC_SAND_U32_MAX) /* No verification due to TCAM database id management */
#define SOC_PB_EGR_ACL_ENTRY_NDX_MAX                               (2047)
#define SOC_PB_EGR_ACL_DP_VAL_MAX                                  (SOC_PETRA_MAX_DROP_PRECEDENCE)
#define SOC_PB_EGR_ACL_ACL_DATA_MAX                                (63)
#define SOC_PB_EGR_ACL_TRAP_CODE_MAX                               (7)
#define SOC_PB_EGR_ACL_OFP_MAX                                     (SOC_PETRA_MAX_FAP_PORT_ID)
#define SOC_PB_EGR_ACL_TC_MAX                                      (SOC_PETRA_TR_CLS_MAX)
#define SOC_PB_EGR_ACL_DP_MAX                                      (SOC_PETRA_MAX_DROP_PRECEDENCE)
#define SOC_PB_EGR_ACL_CUD_MAX                                     (SOC_PETRA_MAX_CUD_ID)


#define SOC_PB_EGR_ACL_DB_TYPE_ETH_FLD_VAL                         (1)
#define SOC_PB_EGR_ACL_DB_TYPE_IPV4_FLD_VAL                        (2)
#define SOC_PB_EGR_ACL_DB_TYPE_MPLS_FLD_VAL                        (3)
#define SOC_PB_EGR_ACL_DB_TYPE_TM_FLD_VAL                          (4)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_egr_fp_tcam_db_id_get(
    SOC_SAND_IN uint32 db_id
  )
{
  return SOC_PB_FP_TCAM_DB_ALLOCATION_BASE + db_id;
}

uint32
  soc_pb_egr_fp_db_id_from_tcam_db_id_get(
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return tcam_db_id - SOC_PB_FP_TCAM_DB_ALLOCATION_BASE;
}



STATIC
  uint32
    soc_pb_egr_acl_tcam_callback(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  user_data
    )
{
  uint32
    fld_val,
    db_id,
    db_id_tcam,
    db_id_hw,
    res = SOC_SAND_OK;
  uint32
    bank_ndx,
    shift;
  SOC_PB_TCAM_PREFIX
    prefix;
  uint8
    is_used;
  SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA
    tbl_data;
  SOC_PB_EGR_ACL_DB_TYPE
    db_type;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_TCAM_CALLBACK);

  db_id = soc_pb_sw_db_egr_acl_sw_db_id_get(unit, user_data);
  db_id_tcam = soc_pb_egr_fp_tcam_db_id_get(db_id);
  db_id_hw = user_data;
 
  regs = soc_pb_pp_regs();

  SOC_PETRA_CLEAR(&tbl_data, SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA, 1);

  /*
   *  Update the Egress ACL TCAM key resolution profile = DB-Id
   */
  /*
   * Get the Prefix and the Banks of this Database Id
   */
  SOC_PB_TCAM_PREFIX_clear(&prefix);
  for (bank_ndx = 0; bank_ndx < SOC_PB_TCAM_NOF_BANKS; ++bank_ndx)
  {
    res = soc_pb_tcam_db_is_bank_used_unsafe(
            unit,
            db_id_tcam,
            bank_ndx,
            &is_used
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (is_used == TRUE)
    {
      tbl_data.tcam_profile += 1 << bank_ndx;
      /*
       * Presence of this bank in the Database
       * All the prefixes should be identical (no verification)
       */
      res = soc_pb_tcam_db_bank_prefix_get_unsafe(
              unit,
              db_id_tcam,
              bank_ndx,
              &prefix
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      shift = 4 - prefix.length;
      tbl_data.key_and_value = (prefix.length == 4) ? 0 : SOC_SAND_BITS_MASK(shift - 1, 0);
      tbl_data.key_or_value = prefix.bits << shift;

      /*
       * Enable this bank to be accessed
       */
      SOC_PB_PP_FLD_SET(regs->egq.acl_tcam_access_enabler_reg.enable_tcam_bank_access[bank_ndx], 0x1, 25, exit);
    }
  }

  /*
   * Get the Database-ID key type
   */
  res = soc_pb_egr_acl_db_get_unsafe(
          unit,
          db_id_hw,
          &db_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  switch (db_type)
  {
  case SOC_PB_EGR_ACL_DB_TYPE_ETH:
    fld_val = SOC_PB_EGR_ACL_DB_TYPE_ETH_FLD_VAL;
  	break;
  case SOC_PB_EGR_ACL_DB_TYPE_IPV4:
    fld_val = SOC_PB_EGR_ACL_DB_TYPE_IPV4_FLD_VAL;
    break;
  case SOC_PB_EGR_ACL_DB_TYPE_MPLS:
    fld_val = SOC_PB_EGR_ACL_DB_TYPE_MPLS_FLD_VAL;
    break;
  case SOC_PB_EGR_ACL_DB_TYPE_TM:
    fld_val = SOC_PB_EGR_ACL_DB_TYPE_TM_FLD_VAL;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_DB_TYPE_OUT_OF_RANGE_ERR, 40, exit);
  }
  tbl_data.key_select = fld_val;

  res = soc_pb_egq_tcam_key_resolution_profile_tbl_set_unsafe(
          unit,
          db_id_hw,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_tcam_callback()", 0, 0);
}


/*********************************************************************
* NAME:
*     soc_pb_egr_acl_init
* FUNCTION:
*     Initialization of the egr_acl configuration.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_pb_egr_acl_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    res;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_INIT);

  regs = soc_pb_pp_regs();

  /* General configuration */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(regs->egq.acl_general_configuration_reg.enable_action_profile, fld_val, 56, exit);
  SOC_PB_PP_FLD_SET(regs->egq.acl_general_configuration_reg.enable_out_tm_port, fld_val, 58, exit);
  SOC_PB_PP_FLD_SET(regs->egq.acl_general_configuration_reg.enable_cud_action, fld_val, 60, exit);
  SOC_PB_PP_FLD_SET(regs->egq.acl_general_configuration_reg.enable_tc_action, fld_val, 62, exit);
  SOC_PB_PP_FLD_SET(regs->egq.acl_general_configuration_reg.enable_dp_action, fld_val, 64, exit);

  SOC_PB_PP_FLD_SET(regs->egq.acl_general_configuration_reg.high_priority_acl, 0x0, 68, exit);
  SOC_PB_PP_FLD_SET(regs->egq.ehpgeneral_settings_reg.ad_acl_arp_en, 0x0, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_egr_acl_init()",0,0);
}

/*********************************************************************
*     Set the predefined Drop Precedence values if a DP
 *     modification must be done.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_dp_values_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES        *dp_val
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;
  uint32
    dp_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DP_VALUES_SET_UNSAFE);

  regs = soc_pb_pp_regs();

  SOC_SAND_CHECK_NULL_INPUT(dp_val);

  /*
   * Verify - no API
   */
  res = soc_pb_egr_acl_dp_values_set_verify(
          unit,
          dp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set the register
   */
  for (dp_ndx = 0; dp_ndx < SOC_PB_EGR_ACL_NOF_DP_VALUES; ++dp_ndx)
  {
    SOC_PB_PP_FLD_SET(regs->egq.acl_general_configuration_reg.acl_dp[dp_ndx], dp_val->val[dp_ndx], 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_dp_values_set_unsafe()", 0, 0);
}

uint32
  soc_pb_egr_acl_dp_values_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES        *dp_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DP_VALUES_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_EGR_ACL_DP_VALUES, dp_val, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_dp_values_set_verify()", 0, 0);
}

/*********************************************************************
*     Set the predefined Drop Precedence values if a DP
 *     modification must be done.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_dp_values_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_EGR_ACL_DP_VALUES        *dp_val
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;
  uint32
    dp_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DP_VALUES_GET_UNSAFE);
 
  regs = soc_pb_pp_regs();

  SOC_SAND_CHECK_NULL_INPUT(dp_val);

  SOC_PB_EGR_ACL_DP_VALUES_clear(dp_val);

  for (dp_ndx = 0; dp_ndx < SOC_PB_EGR_ACL_NOF_DP_VALUES; ++dp_ndx)
  {
    SOC_PB_PP_FLD_GET(regs->egq.acl_general_configuration_reg.acl_dp[dp_ndx], dp_val->val[dp_ndx], 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_dp_values_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Protocol code value according to the L4 Protocol
 *     code index. It determines the protocol field in L3 ACL
 *     Key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_l4_protocol_code_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   code_ndx,
    SOC_SAND_IN  uint32                    protocol_code
  )
{
  uint32
    reg_ndx,
    fld_ndx,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_L4_PROTOCOL_CODE_SET_UNSAFE);

  regs = soc_pb_pp_regs();

  res = soc_pb_egr_acl_l4_protocol_code_set_verify(
          unit,
          code_ndx,
          protocol_code
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  reg_ndx = (code_ndx - 1) / PP_PB_NOF_EGQ_L4_PROTOCOL_CODE_FLDS;
  fld_ndx = (code_ndx - 1) % PP_PB_NOF_EGQ_L4_PROTOCOL_CODE_FLDS;

  SOC_PB_PP_FLD_SET(regs->egq.l4_protocol_code_reg[reg_ndx].l4_protocol_code[fld_ndx], protocol_code, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_l4_protocol_code_set_unsafe()", code_ndx, 0);
}

uint32
  soc_pb_egr_acl_l4_protocol_code_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   code_ndx,
    SOC_SAND_IN  uint32                    protocol_code
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_L4_PROTOCOL_CODE_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(code_ndx, SOC_PB_EGR_ACL_CODE_NDX_MIN, SOC_PB_EGR_ACL_CODE_NDX_MAX, SOC_PB_EGR_ACL_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(protocol_code, SOC_PB_EGR_ACL_PROTOCOL_CODE_MAX, SOC_PB_EGR_ACL_PROTOCOL_CODE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_l4_protocol_code_set_verify()", code_ndx, 0);
}

uint32
  soc_pb_egr_acl_l4_protocol_code_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   code_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_L4_PROTOCOL_CODE_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(code_ndx, SOC_PB_EGR_ACL_CODE_NDX_MIN, SOC_PB_EGR_ACL_CODE_NDX_MAX, SOC_PB_EGR_ACL_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_l4_protocol_code_get_verify()", code_ndx, 0);
}

/*********************************************************************
*     Set the Protocol code value according to the L4 Protocol
 *     code index. It determines the protocol field in L3 ACL
 *     Key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_l4_protocol_code_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   code_ndx,
    SOC_SAND_OUT uint32                    *protocol_code
  )
{
  uint32
    reg_ndx,
    fld_ndx,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_L4_PROTOCOL_CODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(protocol_code);

  regs = soc_pb_pp_regs();

  res = soc_pb_egr_acl_l4_protocol_code_get_verify(
          unit,
          code_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  reg_ndx = (code_ndx - 1) / PP_PB_NOF_EGQ_L4_PROTOCOL_CODE_FLDS;
  fld_ndx = (code_ndx - 1) % PP_PB_NOF_EGQ_L4_PROTOCOL_CODE_FLDS;

  SOC_PB_PP_FLD_GET(regs->egq.l4_protocol_code_reg[reg_ndx].l4_protocol_code[fld_ndx], *protocol_code, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_l4_protocol_code_get_unsafe()", code_ndx, 0);
}

/*********************************************************************
*     Set the Port ACL properties: ACL profile and ACL Data.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_port_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   pp_port_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
    pp_ppct;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_PORT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(
          unit,
          pp_port_ndx,
          &pp_ppct
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  pp_ppct.acl_profile = info->profile;
  pp_ppct.acl_data = info->acl_data;

  res = soc_pb_pp_egq_pp_ppct_tbl_set_unsafe(
          unit,
          pp_port_ndx,
          &pp_ppct
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_port_set_unsafe()", pp_port_ndx, 0);
}

uint32
  soc_pb_egr_acl_port_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   pp_port_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_PORT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, SOC_PB_EGR_ACL_PP_PORT_NDX_MAX, SOC_PB_EGR_ACL_PP_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_EGR_ACL_PORT_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_port_set_verify()", pp_port_ndx, 0);
}

uint32
  soc_pb_egr_acl_port_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   pp_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_PORT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, SOC_PB_EGR_ACL_PP_PORT_NDX_MAX, SOC_PB_EGR_ACL_PP_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_port_get_verify()", pp_port_ndx, 0);
}

/*********************************************************************
*     Set the Port ACL properties: ACL profile and ACL Data.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_port_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   pp_port_ndx,
    SOC_SAND_OUT SOC_PB_EGR_ACL_PORT_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
    pp_ppct;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_PORT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_EGR_ACL_PORT_INFO_clear(info);

  res = soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(
          unit,
          pp_port_ndx,
          &pp_ppct
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->profile = pp_ppct.acl_profile;
  info->acl_data = pp_ppct.acl_data;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_port_get_unsafe()", pp_port_ndx, 0);
}

/*********************************************************************
*     Set the mapping between the forwarding type and
 *     Port-ACL-profile to the ACL-Key-profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_key_profile_map_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type,
    SOC_SAND_IN  uint32                   acl_profile_ndx,
    SOC_SAND_IN  uint32                    fp_hw_db_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA
    key_profile_map_index_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_KEY_PROFILE_MAP_SET_UNSAFE);

  res = soc_pb_egr_acl_key_profile_map_set_verify(
          unit,
          fwd_type,
          acl_profile_ndx,
          fp_hw_db_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  key_profile_map_index_tbl.key_profile_map_index = fp_hw_db_ndx;
  res = soc_pb_egq_key_profile_map_index_tbl_set_unsafe(
          unit,
          fwd_type,
          acl_profile_ndx,
          &key_profile_map_index_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_key_profile_map_set_unsafe()", acl_profile_ndx, 0);
}

uint32
  soc_pb_egr_acl_key_profile_map_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type,
    SOC_SAND_IN  uint32                   acl_profile_ndx,
    SOC_SAND_IN  uint32                    fp_hw_db_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_KEY_PROFILE_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fwd_type, SOC_PB_EGR_ACL_FWD_TYPE_MAX, SOC_PB_EGR_ACL_FWD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(acl_profile_ndx, SOC_PB_EGR_ACL_ACL_PROFILE_NDX_MAX, SOC_PB_EGR_ACL_ACL_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fp_hw_db_ndx, SOC_PB_EGR_ACL_KEY_PROFILE_MAX, SOC_PB_EGR_ACL_KEY_PROFILE_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_key_profile_map_set_verify()", acl_profile_ndx, 0);
}

/*********************************************************************
*     Set the mapping between the forwarding type and
 *     Port-ACL-profile to the ACL-Key-profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_key_profile_map_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type,
    SOC_SAND_IN  uint32                   acl_profile_ndx,
    SOC_SAND_OUT uint32                    *fp_hw_db_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA
    key_profile_map_index_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_KEY_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fp_hw_db_ndx);

  res = soc_pb_egr_acl_key_profile_map_get_verify(
          unit,
          fwd_type,
          acl_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_egq_key_profile_map_index_tbl_get_unsafe(
          unit,
          fwd_type,
          acl_profile_ndx,
          &key_profile_map_index_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  *fp_hw_db_ndx = key_profile_map_index_tbl.key_profile_map_index;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_key_profile_map_get_unsafe()", acl_profile_ndx, 0);
}
uint32
  soc_pb_egr_acl_key_profile_map_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type,
    SOC_SAND_IN  uint32                   acl_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_KEY_PROFILE_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fwd_type, SOC_PB_EGR_ACL_FWD_TYPE_MAX, SOC_PB_EGR_ACL_FWD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(acl_profile_ndx, SOC_PB_EGR_ACL_ACL_PROFILE_NDX_MAX, SOC_PB_EGR_ACL_ACL_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_key_profile_map_set_verify()", acl_profile_ndx, 0);
}

/*********************************************************************
*     Create the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_db_create_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx, /* For SW DB and HW */
    SOC_SAND_IN  uint32                   hw_db_ndx,     /* For TCAM management, = fp_hw_db_ndx + 3 (like PMF DB) */
    SOC_SAND_IN  SOC_PB_EGR_ACL_DB_TYPE          db_type,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    prefix_length = 0,
    res = SOC_SAND_OK;
  uint32
    cycle_ndx;
  SOC_SAND_SUCCESS_FAILURE
    success_l = SOC_SAND_SUCCESS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DB_CREATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  switch (db_type)
  {
  case SOC_PB_EGR_ACL_DB_TYPE_ETH:
    prefix_length = 1;
    break;
  case SOC_PB_EGR_ACL_DB_TYPE_IPV4:
  case SOC_PB_EGR_ACL_DB_TYPE_MPLS:
  case SOC_PB_EGR_ACL_DB_TYPE_TM:
    prefix_length = 4;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_DB_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_pb_tcam_db_create_unsafe(
          unit,
          hw_db_ndx,
          SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS,
          prefix_length,
          SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Save the type in the SW DB (must be done before any bank is allocated)
   */
  if (success_l == SOC_SAND_SUCCESS)
  {
    soc_pb_sw_db_eg_acl_type_set(
      unit,
      fp_hw_db_ndx,
      db_type
    );
  }

  /*
   * Try both cycles to insert the Database
   */
  for (cycle_ndx = 0; cycle_ndx < SOC_PB_TCAM_NOF_CYCLES; ++cycle_ndx)
  {
    res = soc_pb_tcam_access_profile_create_unsafe(
            unit,
            hw_db_ndx,
            hw_db_ndx,
            cycle_ndx,
            TRUE,
            1,
            soc_pb_egr_acl_tcam_callback,
            fp_hw_db_ndx,
            &success_l
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (success_l == SOC_SAND_SUCCESS)
    {
      break;
    }
  }

  if (success_l != SOC_SAND_SUCCESS)
  {
    /*
     *  Revert change in the software database
     */
    soc_pb_sw_db_eg_acl_type_set(
      unit,
      fp_hw_db_ndx,
      SOC_PB_EGR_NOF_ACL_DB_TYPES
    );
  }

  *success = success_l;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_db_create_unsafe()", hw_db_ndx, fp_hw_db_ndx);
}

uint32
  soc_pb_egr_acl_db_create_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   hw_db_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_DB_TYPE          db_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DB_CREATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fp_hw_db_ndx, SOC_PB_EGR_ACL_DB_NDX_MAX, SOC_PB_EGR_ACL_DB_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(db_type, SOC_PB_EGR_ACL_DB_TYPE_MAX, SOC_PB_EGR_ACL_DB_TYPE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_db_create_verify()", fp_hw_db_ndx, hw_db_ndx);
}

/*********************************************************************
*     Get the Database key type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_db_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_OUT SOC_PB_EGR_ACL_DB_TYPE          *db_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(db_type);

  res = soc_pb_egr_acl_db_get_verify(
          unit,
          fp_hw_db_ndx
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Retrieve the type from SW DB
   */
  *db_type = soc_pb_sw_db_eg_acl_type_get(unit, fp_hw_db_ndx);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_db_get_unsafe()", fp_hw_db_ndx, 0);
}

uint32
  soc_pb_egr_acl_db_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DB_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fp_hw_db_ndx, SOC_PB_EGR_ACL_DB_NDX_MAX, SOC_PB_EGR_ACL_DB_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_db_get_verify()", fp_hw_db_ndx, 0);
}

/*********************************************************************
*     Remove the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_db_destroy_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx, /* For SW DB and HW */
    SOC_SAND_IN  uint32                   hw_db_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DB_DESTROY_UNSAFE);

  soc_pb_sw_db_eg_acl_type_set(
    unit,
    fp_hw_db_ndx,
    SOC_PB_EGR_NOF_ACL_DB_TYPES
  );

  /*
   * Destroy in the TCAM
   */
  res = soc_pb_tcam_db_destroy_unsafe(
          unit,
          hw_db_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_db_destroy_unsafe()", hw_db_ndx, fp_hw_db_ndx);
}

uint32
  soc_pb_egr_acl_db_destroy_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   hw_db_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_DB_DESTROY_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fp_hw_db_ndx, SOC_PB_EGR_ACL_DB_NDX_MAX, SOC_PB_EGR_ACL_DB_NDX_OUT_OF_RANGE_ERR, 11, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_db_destroy_verify()", fp_hw_db_ndx, hw_db_ndx);
}

/*********************************************************************
*     Add an entry to the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_entry_add_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    db_id_tcam,
    fld_val,
    output_val,
    res = SOC_SAND_OK;
  uint32
    ndx;
  SOC_PB_TCAM_ENTRY
    entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_TCAM_ENTRY_clear(&entry);

  *success = SOC_SAND_SUCCESS;

  res = soc_pb_egr_acl_entry_add_verify(
          unit,
          fp_hw_db_ndx,
          entry_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Prepare the TCAM entry data
   */
  for (ndx = 0; ndx < SOC_PB_TCAM_ENTRY_MAX_LEN; ++ndx)
  {
    entry.value[ndx] = info->key.data.val[ndx];
    entry.mask[ndx]  = info->key.data.mask[ndx];
  }

  /*
   * Build the Output value
   */
  output_val = 0;
  output_val |= SOC_SAND_SET_BITS_RANGE(info->action_val.cud, 15, 0);
  output_val |= SOC_SAND_SET_BITS_RANGE(info->action_val.dp, 16, 16);
  output_val |= SOC_SAND_SET_BITS_RANGE(info->action_val.tc, 19, 17);
  output_val |= SOC_SAND_SET_BITS_RANGE(info->action_val.dp_tc_ov, 20, 20);
  fld_val = info->action_val.ofp;
  if (info->action_val.ofp_ov == FALSE)
  {
    fld_val = 0x7F; /* All '1's means no override */
  }
  output_val |= SOC_SAND_SET_BITS_RANGE(fld_val, 27, 21);
  output_val |= SOC_SAND_SET_BITS_RANGE(info->action_val.trap_code, 30, 28);
  output_val |= SOC_SAND_SET_BITS_RANGE(info->action_val.trap_en, 31, 31);

  /*
   *  Remove entry, if it already exists
   */
  db_id_tcam = soc_pb_egr_fp_tcam_db_id_get(soc_pb_sw_db_egr_acl_sw_db_id_get(unit, fp_hw_db_ndx));
  res = soc_pb_tcam_db_entry_remove_unsafe(
          unit,
          db_id_tcam,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_tcam_managed_db_entry_add_unsafe(
          unit,
          db_id_tcam,
          entry_ndx,
          (uint16) info->priority,
          &entry,
          output_val,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_entry_add_unsafe()", fp_hw_db_ndx, entry_ndx);
}

uint32
  soc_pb_egr_acl_entry_add_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGR_ACL_DB_TYPE
    db_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fp_hw_db_ndx, SOC_PB_EGR_ACL_DB_NDX_MAX, SOC_PB_EGR_ACL_DB_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_EGR_ACL_ENTRY_NDX_MAX, SOC_PB_EGR_ACL_ENTRY_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_EGR_ACL_ENTRY_INFO, info, 30, exit);

  /*
   * Verify the DB type is of the same type
   */
  res = soc_pb_egr_acl_db_get_unsafe(
          unit,
          fp_hw_db_ndx,
          &db_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (db_type != info->type)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_TYPE_DIFFERENT_FROM_DB_ERR, 50, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_entry_add_verify()", fp_hw_db_ndx, entry_ndx);
}

/*********************************************************************
*     Get an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_entry_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx,
    SOC_SAND_OUT SOC_PB_EGR_ACL_ENTRY_INFO       *info,
    SOC_SAND_OUT uint8                   *is_found
  )
{
  uint32
    db_id_tcam,
    fld_val,
    action,
    res = SOC_SAND_OK;
  SOC_PB_TCAM_ENTRY
    entry;
  uint32
    tcam_val_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  SOC_PB_EGR_ACL_ENTRY_INFO_clear(info);

  res = soc_pb_egr_acl_entry_get_verify(
          unit,
          fp_hw_db_ndx,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Get the entry type from the Database
   */
  res = soc_pb_egr_acl_db_get_unsafe(
          unit,
          fp_hw_db_ndx,
          &(info->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->key.format.type    = SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL;
  info->key.format.egr_acl = info->type;
  info->key.size           = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS; /* All entries are 144b */

  /*
   * Get the entry
   */
  SOC_PB_TCAM_ENTRY_clear(&entry);
  db_id_tcam = soc_pb_egr_fp_tcam_db_id_get(soc_pb_sw_db_egr_acl_sw_db_id_get(unit, fp_hw_db_ndx));
  res = soc_pb_tcam_db_entry_get_unsafe(
          unit,
          db_id_tcam,
          entry_ndx,
          &(info->priority),
          &entry,
          &action,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (*is_found)
  {
    /*
     * Parse the entry
     */
    for (tcam_val_ndx = 0; tcam_val_ndx < SOC_PB_TCAM_RULE_NOF_UINT32S_MAX; ++tcam_val_ndx)
    {
      info->key.data.val[tcam_val_ndx] = entry.value[tcam_val_ndx];
      info->key.data.mask[tcam_val_ndx] = entry.mask[tcam_val_ndx];
    }

    /*
     * Parse the action
     */
    info->action_val.cud = SOC_SAND_GET_BITS_RANGE(action, 15, 0);
    info->action_val.dp = SOC_SAND_GET_BITS_RANGE(action, 16, 16);
    info->action_val.tc = SOC_SAND_GET_BITS_RANGE(action, 19, 17);
    info->action_val.dp_tc_ov = SOC_SAND_GET_BIT(action, 20);
    fld_val = SOC_SAND_GET_BITS_RANGE(action, 27, 21);
    if (fld_val == 0x7F)
    {
      /* All '1's means no override */
      info->action_val.ofp_ov = FALSE;
    }
    else
    {
      info->action_val.ofp_ov = TRUE;
      info->action_val.ofp = fld_val;
    }

    info->action_val.trap_code = SOC_SAND_GET_BITS_RANGE(action, 30, 28);
    info->action_val.trap_en = SOC_SAND_GET_BIT(action, 31);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_entry_get_unsafe()", fp_hw_db_ndx, entry_ndx);
}

uint32
  soc_pb_egr_acl_entry_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fp_hw_db_ndx, SOC_PB_EGR_ACL_DB_NDX_MAX, SOC_PB_EGR_ACL_DB_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_EGR_ACL_ENTRY_NDX_MAX, SOC_PB_EGR_ACL_ENTRY_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_entry_get_verify()", fp_hw_db_ndx, entry_ndx);
}

/*********************************************************************
*     Remove an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_acl_entry_remove_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx
  )
{
  uint32
    db_id_tcam,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_ENTRY_REMOVE_UNSAFE);

  res = soc_pb_egr_acl_entry_remove_verify(
          unit,
          fp_hw_db_ndx,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  db_id_tcam = soc_pb_egr_fp_tcam_db_id_get(soc_pb_sw_db_egr_acl_sw_db_id_get(unit, fp_hw_db_ndx));
  res = soc_pb_tcam_db_entry_remove_unsafe(
          unit,
          db_id_tcam,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_entry_remove_unsafe()", fp_hw_db_ndx, entry_ndx);
}

uint32
  soc_pb_egr_acl_entry_remove_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_ACL_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fp_hw_db_ndx, SOC_PB_EGR_ACL_DB_NDX_MAX, SOC_PB_EGR_ACL_DB_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_EGR_ACL_ENTRY_NDX_MAX, SOC_PB_EGR_ACL_ENTRY_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_acl_entry_remove_verify()", fp_hw_db_ndx, entry_ndx);
}



uint32
  SOC_PB_EGR_ACL_DP_VALUES_verify(
    SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_EGR_ACL_NOF_DP_VALUES; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[ind], SOC_PB_EGR_ACL_DP_VAL_MAX, SOC_PB_EGR_ACL_VAL_OUT_OF_RANGE_ERR, 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_EGR_ACL_DP_VALUES_verify()",0,0);
}

uint32
  SOC_PB_EGR_ACL_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->profile, SOC_PB_EGR_ACL_ACL_PROFILE_NDX_MAX, SOC_PB_EGR_ACL_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->acl_data, SOC_PB_EGR_ACL_ACL_DATA_MAX, SOC_PB_EGR_ACL_ACL_DATA_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_EGR_ACL_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PB_EGR_ACL_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_EGR_ACL_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->trap_code, SOC_PB_EGR_ACL_TRAP_CODE_MAX, SOC_PB_EGR_ACL_TRAP_CODE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ofp, SOC_PB_EGR_ACL_OFP_MAX, SOC_PB_EGR_ACL_OFP_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_PB_EGR_ACL_TC_MAX, SOC_PB_EGR_ACL_TC_OUT_OF_RANGE_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_PB_EGR_ACL_DP_MAX, SOC_PB_EGR_ACL_DP_OUT_OF_RANGE_ERR, 16, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cud, SOC_PB_EGR_ACL_CUD_MAX, SOC_PB_EGR_ACL_CUD_OUT_OF_RANGE_ERR, 17, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_EGR_ACL_ACTION_VAL_verify()",0,0);
}

uint32
  SOC_PB_EGR_ACL_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_EGR_ACL_DB_TYPE_MAX, SOC_PB_EGR_ACL_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_EGR_ACL_ACTION_VAL, &(info->action_val), 13, exit);

  /*
   * Verify the Key
   */
  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_KEY, &(info->key), 14, exit);

  if (info->key.size != SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_SIZE_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->key.format.type != SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_KEY_FORMAT_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_EGR_ACL_ENTRY_INFO_verify()",0,0);
}

void
  SOC_PB_EGR_ACL_DP_VALUES_clear(
    SOC_SAND_OUT SOC_PB_EGR_ACL_DP_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_ACL_DP_VALUES_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_EGR_ACL_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_EGR_ACL_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_ACL_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_EGR_ACL_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_EGR_ACL_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_ACL_ACTION_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_EGR_ACL_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_EGR_ACL_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_ACL_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

void
  SOC_PB_EGR_ACL_DP_VALUES_print(
    SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_ACL_DP_VALUES_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_EGR_ACL_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_ACL_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_EGR_ACL_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_EGR_ACL_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_ACL_ACTION_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_EGR_ACL_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_ACL_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

