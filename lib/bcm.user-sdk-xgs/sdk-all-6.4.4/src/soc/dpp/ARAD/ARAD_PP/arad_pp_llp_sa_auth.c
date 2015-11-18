#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_llp_sa_auth.c,v 1.29 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/swstate/sw_state_defs.h>

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_vid_assign.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_sa_auth.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_ports.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>




/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_LLP_SA_AUTH_RULE_TYPE_MAX                        (ARAD_PP_NOF_LLP_SA_AUTH_MATCH_RULE_TYPES-1)
#define ARAD_PP_LLP_SA_AUTH_PORT_MAX                             (SOC_SAND_U32_MAX)

/* } */
/*************
 * MACROS    *
 *************/
/* { */
/*
 * MACRO to check if SA authentication feature is enabled
 * will be called in beginning of each API
 */
#define ARAD_PP_LLP_SA_AUTH_FEATURE_CHECK_IF_ENABLED(unit)                            \
{                                                                                     \
    uint8 _authentication_enable;                                                     \
    res = arad_pp_sw_db_authentication_enable_get(unit, &_authentication_enable);     \
    SOC_SAND_CHECK_FUNC_RESULT(res, 777, exit);                                       \
    if(!_authentication_enable)  {                                                    \
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_SA_AUTH_FEATURE_DISABLED_ERR, 150, exit); \
    }                                                                                 \
}

/* given SA auth info, check if it actually accept all packets and check nothing */
#define   ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL(mac_auth_info) \
    ((mac_auth_info->tagged_only == FALSE) &&      \
    (mac_auth_info->expect_system_port.sys_id == ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS) &&    \
    (mac_auth_info->expect_tag_vid == ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_VIDS))    \

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
  Arad_pp_procedure_desc_element_llp_sa_auth[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_MAC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_MAC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_MAC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_MAC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_MAC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_MAC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_MAC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_MAC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_SA_AUTH_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
 SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_SA_BASED_KEY_TO_LEM_KEY_MAP),
 SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_SA_BASED_KEY_FROM_LEM_KEY_MAP),
 SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_SA_BASED_PAYLOAD_FROM_LEM_PAYLOAD_MAP),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_llp_sa_auth[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_LLP_SA_AUTH_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_SA_AUTH_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_SA_AUTH_EXPECT_SYSTEM_PORT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_SA_AUTH_EXPECT_SYSTEM_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'expect_system_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_SA_AUTH_RULE_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_SA_AUTH_RULE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'rule_type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_LLP_SA_AUTH_MATCH_RULE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_SA_AUTH_PORT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_SA_AUTH_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    ARAD_PP_LLP_SA_AUTH_FEATURE_DISABLED_ERR,
    "ARAD_PP_LLP_SA_AUTH_FEATURE_DISABLED_ERR",
    "SA authentication feature is disabled by operation mode. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_ILLEGAL_ERR,
    "ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_ILLEGAL_ERR",
    "SA authentication accept all packets, illegal usage. \n\r "
    "In this case use enable = FALSE, to remove entry. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_SA_BASED_LEM_KEY_MISMATCH_ERR,
    "ARAD_PP_LLP_SA_BASED_LEM_KEY_MISMATCH_ERR",
    "LEM key to convert to sa-based key is not legal. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_AUTH_EN_VID_ASSIGN_DIS_ERR,
    "ARAD_PP_LLP_AUTH_EN_VID_ASSIGN_DIS_ERR",
    "Cannot enable SA Authentication on port when \n\r "
    "VID assignment is disabled, first call: soc_ppd_llp_vid_assign_port_info_set\n\r "
    "to enable VID assignment according to port.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
  


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

uint32
  arad_pp_llp_sa_auth_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets port information for SA authentication, including
 *     whether to enable SA authentication on this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_sa_auth_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  )
{
  ARAD_PP_IHP_PINFO_LLR_TBL_DATA
    llr_pinfo_tbl;
  uint32
    vid_sa_base_enabled = 0;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_auth_info);
  ARAD_PP_LLP_SA_AUTH_FEATURE_CHECK_IF_ENABLED(unit);
  res = arad_pp_ihp_pinfo_llr_tbl_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &llr_pinfo_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  vid_sa_base_enabled = SHR_BITGET(
          sw_state[unit]->dpp.soc.arad.pp->llp_vid_assign->vid_sa_based_enable,
          local_port_ndx
        );

  if (!vid_sa_base_enabled && port_auth_info->sa_auth_enable)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_AUTH_EN_VID_ASSIGN_DIS_ERR,15,exit);
  }

  llr_pinfo_tbl.enable_sa_authentication = SOC_SAND_BOOL2NUM(port_auth_info->sa_auth_enable);
  /* sa lookup is enabled if there is authentication or vid assignment */
  llr_pinfo_tbl.sa_lookup_enable = SOC_SAND_BOOL2NUM(port_auth_info->sa_auth_enable || vid_sa_base_enabled);

  res = arad_pp_ihp_pinfo_llr_tbl_set_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &llr_pinfo_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_sa_auth_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_LLP_SA_AUTH_PORT_INFO, port_auth_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_sa_auth_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_port_info_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for SA authentication, including
 *     whether to enable SA authentication on this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_sa_auth_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  )
{
  ARAD_PP_IHP_PINFO_LLR_TBL_DATA
    llr_pinfo_tbl;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_auth_info);

  ARAD_PP_LLP_SA_AUTH_PORT_INFO_clear(port_auth_info);
  
  ARAD_PP_LLP_SA_AUTH_FEATURE_CHECK_IF_ENABLED(unit);

  res = arad_pp_ihp_pinfo_llr_tbl_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &llr_pinfo_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_auth_info->sa_auth_enable = SOC_SAND_NUM2BOOL(llr_pinfo_tbl.enable_sa_authentication);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_port_info_get_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_sa_based_key_to_lem_key_map(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS        *mac_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *key
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SA_BASED_KEY_TO_LEM_KEY_MAP);

  SOC_SAND_CHECK_NULL_INPUT(key);

  key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH;
  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH;

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          mac_key,
          key->param[0].value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SA_AUTH;
  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH(unit);
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sa_based_key_to_lem_key_map()", 0, 0);
}

uint32
  arad_pp_sa_based_key_from_lem_key_map(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *key,
      SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS        *mac_key
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SA_BASED_KEY_FROM_LEM_KEY_MAP);

  SOC_SAND_CHECK_NULL_INPUT(key);

  if( key->type != ARAD_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH ||
      key->nof_params != ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH
    )
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_SA_BASED_LEM_KEY_MISMATCH_ERR, 10, exit);
  }

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          key->param[0].value,
          mac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sa_based_key_from_lem_key_map()", 0, 0);
}
uint32
  arad_pp_sa_based_trill_key_to_lem_key_map(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS          *mac_key,
      SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID          *expect_system_port,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY           *key
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SA_BASED_KEY_TO_LEM_KEY_MAP);

  SOC_SAND_CHECK_NULL_INPUT(key);

  key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_ADJ;
 
  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          mac_key,
          key->param[0].value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_ADJ;
  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_ADJ;
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_ADJ;
  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_ADJ;
  key->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_ADJ;
  key->param[1].value[0] = expect_system_port->sys_id;

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sa_based_trill_key_to_lem_key_map()", 0, 0);
}

uint32
  arad_pp_sa_based_trill_key_from_lem_key_map(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *key,
      SOC_SAND_OUT SOC_SAND_PP_SYS_PORT_ID      *expect_system_port,
      SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS      *mac_key
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SA_BASED_KEY_FROM_LEM_KEY_MAP);

  SOC_SAND_CHECK_NULL_INPUT(key);

  if( key->type != ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_ADJ ||
      key->nof_params != ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_ADJ 
    )
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_SA_BASED_LEM_KEY_MISMATCH_ERR, 10, exit);
  }

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          key->param[0].value,
          mac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  expect_system_port->sys_id = key->param[1].value[0];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sa_based_key_from_lem_key_map()", 0, 0);
}


uint32
  arad_pp_sa_based_payload_from_lem_payload_map(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD        *lem_payload,
    SOC_SAND_OUT ARAD_PP_LLP_SA_AUTH_MAC_INFO      *auth_info
  )
{
  uint8
    is_lag_not_phys;
  uint32
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SA_BASED_KEY_FROM_LEM_KEY_MAP);

  SOC_SAND_CHECK_NULL_INPUT(lem_payload);
  SOC_SAND_CHECK_NULL_INPUT(auth_info);

  ARAD_PP_LLP_SA_AUTH_MAC_INFO_clear(auth_info);

  /*found, check if valid*/
  res = arad_pp_lem_access_sa_based_asd_parse(
          unit,
          lem_payload->asd,
          auth_info,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

 /*
  * get the specific port if needed
  */
  if (auth_info->expect_system_port.sys_id != ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
  {
      /* Always System port */
      res = arad_ports_logical_sys_id_parse(
                unit,
                lem_payload->dest,
                &is_lag_not_phys,
                &lag_id,
                &lag_member_id,
                &sys_phys_port_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

      if(is_lag_not_phys) 
      {
        auth_info->expect_system_port.sys_port_type = SOC_TMC_DEST_SYS_PORT_TYPE_LAG;
        auth_info->expect_system_port.sys_id = lag_id;
      }
      else
      {
        auth_info->expect_system_port.sys_id = sys_phys_port_id;
        auth_info->expect_system_port.sys_port_type = SOC_TMC_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
      }            
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sa_based_payload_from_lem_payload_map()", 0, 0);
}

/*********************************************************************
*     Set authentication information for a MAC address,
 *     including the expected VLAN/ports the MAC address has to
 *     come with.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_sa_auth_mac_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_IN  uint8                                 enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  ARAD_PP_FRWRD_DECISION_INFO
    fwd_decision;
  ARAD_PP_LLP_SA_AUTH_MAC_INFO
    no_mac_auth_info;
  uint8
    is_found = FALSE; 
  uint32
    old_asd,
    new_asd = 0, 
    updated_asd,
    asd_tag_part;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_MAC_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  request.stamp = 0;
  /* convert to LEM key */
  res = arad_pp_sa_based_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* set payload */

  /* get previous payload. ASD , as part of the ASD used for VID-assignment */
  /* if key is found and destination is not relevant then keep exist destination payload.dest*/
  res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* if remove and not found then done */
  if (!is_found && !enable)
  {
    *success = SOC_SAND_SUCCESS;
    goto exit;
  }

  old_asd = payload.asd;

  /* set defaults */
  payload.age = ARAD_PP_SA_AUTH_ENTRY_AGE;
  payload.is_dynamic = ARAD_PP_SA_AUTH_ENTRY_IS_DYNAMIC;

  /* set payload for add operation */
  if (enable)
  {
   /*
    * update destination if needed
    */
    /* Get the encoded for destination ASD will be overwritten  */
    /* create forwarding decision for this purpose */
    if (mac_auth_info->expect_system_port.sys_id != ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
    {
      ARAD_PP_FRWRD_DECISION_INFO_clear(&fwd_decision);
      if (mac_auth_info->expect_system_port.sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG)
      {
        fwd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_UC_LAG;
      }
      else
      {
        fwd_decision.type = ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT;
      }

      fwd_decision.dest_id = mac_auth_info->expect_system_port.sys_id;
      fwd_decision.additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_NONE;

      res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH,
          &(fwd_decision),
          &(payload.dest),
          &(payload.asd)
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

   /*
    * new ASD
    */
    res = arad_pp_lem_access_sa_based_asd_build(
            unit,
            mac_auth_info,
            NULL,
            &new_asd
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

   /*
    * if found then leave VID, MAC based assignment bit untouched.
    * if not found then set to zeros, i.e. don't used VID.
    */
    if (is_found)
    {
      updated_asd = old_asd;
    }
    else
    {
      updated_asd = 0;
    }
   /*
    * set rest of bits
    */
    res = soc_sand_set_field(&updated_asd, ARAD_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_MSB, ARAD_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB, new_asd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    payload.asd = updated_asd;
    payload.flags = ARAD_PP_LEM_ACCESS_ASD_FORMAT_3;
    /* add entry */
    request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;

    res = arad_pp_lem_access_entry_add_unsafe(
            unit,
            &request,
            &payload,
            &ack_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else /* found and remove operation*/
  {
    /* check if tagging field are not in use */
    asd_tag_part = SOC_SAND_GET_BITS_RANGE(old_asd, ARAD_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_MSB, ARAD_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LSB);
    
    if (asd_tag_part == 0) /* not in use --> remove entry */
    {
      request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;

      res = arad_pp_lem_access_entry_remove_unsafe(
              unit,
              &request,
              &ack_status
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    else /* in use --> update ASD entry to disable authentication */
    {
      ARAD_PP_LLP_SA_AUTH_MAC_INFO_clear(&no_mac_auth_info);
      no_mac_auth_info.tagged_only = FALSE;
      no_mac_auth_info.expect_system_port.sys_id = ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS;
      no_mac_auth_info.expect_tag_vid = ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_VIDS;

      res = arad_pp_lem_access_sa_based_asd_build(
              unit,
              &no_mac_auth_info,
              NULL,
              &new_asd
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = soc_sand_set_field(&new_asd, ARAD_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_MSB, ARAD_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LSB, asd_tag_part);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      /* add entry */
      request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
      payload.asd = new_asd;
      payload.flags = ARAD_PP_LEM_ACCESS_ASD_FORMAT_3;

      res = arad_pp_lem_access_entry_add_unsafe(
              unit,
              &request,
              &payload,
              &ack_status
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

  if (ack_status.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    if (ack_status.reason == ARAD_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN)
    {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
    }
    else
    {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_mac_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_llp_sa_auth_mac_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_MAC_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_LLP_SA_AUTH_MAC_INFO, mac_auth_info, 10, exit);
  /* check that it's not accept all where enable set tot TRUE
  if (enable && ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL(mac_auth_info))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_ILLEGAL_ERR, 20, exit);
  }
*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_mac_info_set_verify()", 0, 0);
}

uint32
  arad_pp_llp_sa_auth_mac_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_MAC_INFO_GET_VERIFY);


  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_mac_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set authentication information for a MAC address,
 *     including the expected VLAN/ports the MAC address has to
 *     come with.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_sa_auth_mac_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_OUT ARAD_PP_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  ARAD_PP_FRWRD_DECISION_INFO
    fwd_decision;
  uint8
    is_found = FALSE; 
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_MAC_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(enable);

  ARAD_PP_LLP_SA_AUTH_MAC_INFO_clear(mac_auth_info);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  request.stamp = 0;
  /* convert to LEM key */
  res = arad_pp_sa_based_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* get previous payload. ASD , as part of the ASD used for VID-assignment */
  /* if key is found and destination is not relevant then keep exist destination payload.dest*/
  res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* if not found then done */
  if (!is_found)
  {
    *enable = FALSE;
    goto exit;
  }

  /*found, check if valid*/
  res = arad_pp_lem_access_sa_based_asd_parse(
          unit,
          payload.asd,
          mac_auth_info,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

 /*
  * get the specific port if needed
  */
  if (mac_auth_info->expect_system_port.sys_id != ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS || SOC_IS_ARAD_A0(unit))
  {
      res = arad_pp_fwd_decision_in_buffer_parse(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH,
          payload.dest,
          payload.asd,
          ARAD_PP_FWD_DECISION_PARSE_DEST, /* Don't matter */
          &(fwd_decision)
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if (fwd_decision.type == ARAD_PP_FRWRD_DECISION_TYPE_UC_LAG)
      {
        mac_auth_info->expect_system_port.sys_port_type = SOC_TMC_DEST_SYS_PORT_TYPE_LAG;
        mac_auth_info->expect_system_port.sys_id = fwd_decision.dest_id;
      }
      else
      {
        mac_auth_info->expect_system_port.sys_port_type = SOC_TMC_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
        mac_auth_info->expect_system_port.sys_id = fwd_decision.dest_id;
      }
  }

  *enable = TRUE;/*(uint8)!ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL(mac_auth_info);*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_mac_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get SA authentation information according to source MAC
 *     address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_sa_auth_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_MATCH_RULE              *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key_arr,
    SOC_SAND_OUT ARAD_PP_LLP_SA_AUTH_MAC_INFO                *auth_info_arr,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    access_only = FALSE,
    read_index = 0,
    tmp_nof_entries = 0;     
  ARAD_PP_LEM_ACCESS_KEY
    key;
  ARAD_PP_LEM_ACCESS_KEY            
    key_mask;
  ARAD_PP_LEM_ACCESS_KEY
    *read_keys = NULL;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    *read_vals = NULL;
  SOC_SAND_PP_MAC_ADDRESS
    mac_address_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(mac_address_key_arr);
  SOC_SAND_CHECK_NULL_INPUT(auth_info_arr);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);
  

  *nof_entries = 0;
  
  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&mac_address_key);
  
  /* convert to LEM key */
  res = arad_pp_sa_based_key_to_lem_key_map(
          unit,
          &mac_address_key,
          &key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* consider all lem entries for SA Auth */
  key.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH;
  key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
  key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH(unit);
  
   /* malloc place for keys and vals */
  read_keys = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act,"read_keys");
  read_vals = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range->entries_to_act,"read_vals");

  if (!read_keys)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 40, exit);
  }

  if (!read_vals)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 45, exit);
  }

  /* initialize block range */
  res = soc_sand_os_memset(
          read_keys,
          0x0,
          sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_sand_os_memset(
          read_vals,
          0x0,
          sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range->entries_to_act
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = arad_pp_frwrd_lem_get_block_unsafe(unit,&key,&key_mask,NULL,access_only ,block_range,read_keys,read_vals,&tmp_nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *  Loop on the entry index
   */
  for ( read_index = 0; read_index < tmp_nof_entries; ++read_index)
  {
    if ((read_keys[read_index].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH))
    {     
      res = arad_pp_sa_based_key_from_lem_key_map(
              unit,
              &read_keys[read_index],
              &(mac_address_key_arr[*nof_entries])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


      res = arad_pp_sa_based_payload_from_lem_payload_map(
              unit,
              &read_vals[read_index],
              &(auth_info_arr[*nof_entries])
            );      
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      ++*nof_entries;
    }
  }

exit:
  if (read_keys)
  {
    soc_sand_os_free_any_size(read_keys);
  }
  if (read_vals)
  {
    soc_sand_os_free_any_size(read_vals);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_get_block_unsafe()", 0, 0);
}

uint32
  arad_pp_llp_sa_auth_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_MATCH_RULE              *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_SA_AUTH_GET_BLOCK_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_LLP_SA_AUTH_MATCH_RULE, rule, 10, exit);
  /* ARAD_PP_STRUCT_VERIFY(SOC_SAND_TABLE_BLOCK_RANGE, block_range, 20, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_sa_auth_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_llp_sa_auth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_sa_auth_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_llp_sa_auth;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_llp_sa_auth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_sa_auth_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_llp_sa_auth;
}
uint32
  ARAD_PP_LLP_SA_AUTH_PORT_INFO_verify(
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LLP_SA_AUTH_PORT_INFO_verify()",0,0);
}

uint32
  ARAD_PP_LLP_SA_AUTH_MAC_INFO_verify(
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_MAC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->expect_tag_vid != ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_VIDS)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->expect_tag_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);
  }
  if (info->expect_system_port.sys_id != ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
  {
    /*ARAD_PP_STRUCT_VERIFY(SOC_TMC_DEST_SYS_PORT_INFO, info->expect_system_port, 20, exit);*/ 
  }
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LLP_SA_AUTH_MAC_INFO_verify()",0,0);
}

uint32
  ARAD_PP_LLP_SA_AUTH_MATCH_RULE_verify(
    SOC_SAND_IN  ARAD_PP_LLP_SA_AUTH_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->rule_type != ARAD_PP_LLP_SA_MATCH_RULE_TYPE_ALL) {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->rule_type, ARAD_PP_LLP_SA_AUTH_RULE_TYPE_MAX, ARAD_PP_LLP_SA_AUTH_RULE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  if (info->port.sys_id != ARAD_PP_IGNORE_VAL) {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->port.sys_id, ARAD_PP_LLP_SA_AUTH_PORT_MAX, ARAD_PP_LLP_SA_AUTH_PORT_OUT_OF_RANGE_ERR, 11, exit);
  }

  if (info->vid != ARAD_PP_IGNORE_VAL) {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 12, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LLP_SA_AUTH_MATCH_RULE_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

