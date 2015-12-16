/* $Id: pb_pp_rif.c,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP_/src/soc_pb_pp_rif.c
*
* MODULE PREFIX:  soc_pb_pp_pp
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_rif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_mpls_term.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mpls_term.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>


#define SOC_PPD_LIF_NULL 0
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_RIF_DIP_KEY_MAX                                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_RIF_TTL_SCOPE_NDX_MAX                            (7)
#define SOC_PB_PP_RIF_FIRST_LABEL_MAX                              ((1<<20) - 1)
#define SOC_PB_PP_RIF_LAST_LABEL_MAX                               ((1<<20) - 1)
#define SOC_PB_PP_RIF_COS_PROFILE_MAX                              (15)
#define SOC_PB_PP_RIF_TTL_SCOPE_INDEX_MAX                          (7)
#define SOC_PB_PP_RIF_ROUTING_ENABLERS_BM_MAX                      (SOC_SAND_UINT_MAX)

#define SOC_PB_PP_RIF_ROUTING_ENABLER_UC 0x1
#define SOC_PB_PP_RIF_ROUTING_ENABLER_MC 0x2
#define SOC_PB_PP_RIF_ROUTING_ENABLER_MPLS 0x4

#define SOC_PB_PP_RIF_ISEM_ENTRY_OP_CODE 0

#define SOC_PB_PP_RIF_ISEM_RES_MPLS_TYPE_VRL 2
#define SOC_PB_PP_RIF_ISEM_RES_MPLS_TYPE_LSP 3

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

static
  SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_rif[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_VSID_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_VSID_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_VSID_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_VSID_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_VSID_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_VSID_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_VSID_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_VSID_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_TTL_SCOPE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_TTL_SCOPE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_TTL_SCOPE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_TTL_SCOPE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_TTL_SCOPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_TTL_SCOPE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_TTL_SCOPE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_TTL_SCOPE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_RIF_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_rif[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_RIF_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR",
    "The parameter 'dip_key' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_TTL_SCOPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_TTL_SCOPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'ttl_scope_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_FIRST_LABEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_FIRST_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'first_label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_LAST_LABEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_LAST_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'last_label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'ttl_scope_index' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_ROUTING_ENABLERS_BM_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_ROUTING_ENABLERS_BM_OUT_OF_RANGE_ERR",
    "The parameter 'routing_enablers_bm' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_MPLS_LABEL_INVALID_RANGE_ERR,
    "SOC_PB_PP_RIF_MPLS_LABEL_INVALID_RANGE_ERR",
    "The MPLS label range is invalid (first > last). \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR,
    "SOC_PB_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR",
    "invalid key for MPLS termination, \n\r "
    "key type set to Label only, and key inserted \n\r "
    "includes VSI != 0\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
  
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
  soc_pb_pp_rif_info_set_imp_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_id,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA
    tbl_data;
  SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA
    egq_ttl_scope_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_VSID_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res =  soc_pb_pp_ihp_in_rif_config_table_tbl_get_unsafe(
          unit,
          rif_id,
          &tbl_data
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl_data.in_rif_cos_profile = rif_info->cos_profile;
  tbl_data.vrf = rif_info->vrf_id;
  tbl_data.uc_rpf_enable = rif_info->uc_rpf_enable;

  tbl_data.enable_routing_mc = (rif_info->routing_enablers_bm & SOC_PB_PP_RIF_ROUTING_ENABLER_MC) ? 1 : 0;
  tbl_data.enable_routing_uc = (rif_info->routing_enablers_bm & SOC_PB_PP_RIF_ROUTING_ENABLER_UC) ? 1 : 0;
  tbl_data.enable_routing_mpls = (rif_info->routing_enablers_bm & SOC_PB_PP_RIF_ROUTING_ENABLER_MPLS) ? 1 : 0;

  res =  soc_pb_pp_ihp_in_rif_config_table_tbl_set_unsafe(
          unit,
          rif_id,
          &tbl_data
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

 /*
  * set TTL scope index:
  */
  egq_ttl_scope_tbl.ttl_scope = rif_info->ttl_scope_index;

  res =  soc_pb_pp_egq_ttl_scope_tbl_set_unsafe(
          unit,
          (uint32)rif_id,
          &egq_ttl_scope_tbl
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_info_set_imp_unsafe()", rif_id, 0);
}

STATIC uint32
  soc_pb_pp_rif_info_get_imp_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_id,
    SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA
    tbl_data;
  SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA
    egq_ttl_scope_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PB_PP_RIF_INFO_clear(rif_info);

  res =  soc_pb_pp_ihp_in_rif_config_table_tbl_get_unsafe(
          unit,
          rif_id,
          &tbl_data
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  rif_info->vrf_id = tbl_data.vrf;
  rif_info->cos_profile = tbl_data.in_rif_cos_profile;
  rif_info->uc_rpf_enable = (tbl_data.uc_rpf_enable) ? TRUE : FALSE;
  rif_info->routing_enablers_bm = \
    ((tbl_data.enable_routing_mc == 0) ? 0 : SOC_PB_PP_RIF_ROUTING_ENABLER_MC) |
    ((tbl_data.enable_routing_uc == 0) ? 0 : SOC_PB_PP_RIF_ROUTING_ENABLER_UC) |
    ((tbl_data.enable_routing_mpls == 0) ? 0 : SOC_PB_PP_RIF_ROUTING_ENABLER_MPLS);


 /*
  * Get TTL scope index:
  */
  res =  soc_pb_pp_egq_ttl_scope_tbl_get_unsafe(
          unit,
          (uint32)rif_id,
          &egq_ttl_scope_tbl
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  rif_info->ttl_scope_index = egq_ttl_scope_tbl.ttl_scope;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_info_get_imp_unsafe()", rif_id, 0);
}


uint32
  soc_pb_pp_rif_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the MPLS labels that may be mapped to Router
 *     Interfaces
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_labels_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_labels_range_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_rif_mpls_labels_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_RIF_MPLS_LABELS_RANGE, rif_labels_range, 10, exit);

  /* ignore */
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_labels_range_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_rif_mpls_labels_range_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_labels_range_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the MPLS labels that may be mapped to Router
 *     Interfaces
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_labels_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_clear(rif_labels_range);
  
  /* ignore */
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);
  
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_labels_range_get_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_rif_mpls_key_to_isem_key_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT  SOC_PB_PP_ISEM_ACCESS_KEY                    *isem_key
  )
{
   uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_MPLS_TERM_LKUP_INFO
    mpls_lkup_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(isem_key);

  SOC_PB_PP_CLEAR(isem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
  
  /* lookup type */
  res = soc_pb_pp_mpls_term_lkup_info_get_unsafe(
          unit,
          &mpls_lkup_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (mpls_lkup_key.key_type == SOC_PB_PP_MPLS_TERM_KEY_TYPE_LABEL_RIF)
  {
    isem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF;
    isem_key->key_info.mpls.label = mpls_key->label_id;
    isem_key->key_info.mpls.in_rif = mpls_key->vsid;
  }
  else
  {
    isem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
    isem_key->key_info.mpls.label = mpls_key->label_id;
    isem_key->key_info.mpls.in_rif = 0;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_key_to_isem_key_unsafe()", 0, 0);
}
 
/*********************************************************************
*     Enable MPLS labels termination and setting the Router
 *     interface according to the terminated MPLS label.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_label_map_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res =  soc_pb_pp_rif_mpls_label_map_add_internal_unsafe(
          unit,
          mpls_key,
          FALSE,
          lif_index,
          term_info,
          rif_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 /*
  * success or overwriting exist entry
  */
  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_label_map_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_rif_mpls_label_map_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    isem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    isem_entry;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA
    sem_res_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(mpls_key);

    SOC_PB_PP_CLEAR(&isem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    SOC_PB_PP_CLEAR(&isem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
    
    /* SEM table */
    res = soc_pb_pp_rif_mpls_key_to_isem_key_unsafe(
            unit,
            mpls_key,
            &isem_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    isem_entry.sem_result_ndx = lif_index;
    isem_entry.sem_op_code = SOC_PB_PP_RIF_ISEM_ENTRY_OP_CODE;
    
    res = soc_pb_pp_isem_access_entry_add_unsafe(
            unit,
            &isem_key,
            &isem_entry,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(*success != SOC_SAND_SUCCESS)
    {
      goto exit;
    }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  res = soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_get_unsafe(
          unit,
          lif_index,
          &sem_res_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  sem_res_tbl_data.cos_profile = term_info->cos_profile;

  if(term_info->rif != SOC_PPD_RIF_NULL)
  {
    sem_res_tbl_data.in_rif = term_info->rif;
    sem_res_tbl_data.in_rif_valid = 1;
  }
  else
  {
    sem_res_tbl_data.in_rif_valid = 0;
  }
  
  sem_res_tbl_data.type = \
    (term_info->next_prtcl == SOC_PB_PP_L3_NEXT_PRTCL_TYPE_MPLS) ? SOC_PB_PP_RIF_ISEM_RES_MPLS_TYPE_LSP : SOC_PB_PP_RIF_ISEM_RES_MPLS_TYPE_VRL;
  
  sem_res_tbl_data.valid = 1;
  sem_res_tbl_data.model_is_pipe = (term_info->processing_type == SOC_PB_PP_MPLS_TERM_MODEL_PIPE) ? 1 : 0;
  sem_res_tbl_data.service_type = SOC_PB_PP_RIF_ISEM_RES_SERVICE_TYPE;

  res = soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_set_unsafe(
          unit,
          lif_index,
          &sem_res_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* RIF table */
  if(term_info->rif != SOC_PPD_RIF_NULL)
  {
    res =  soc_pb_pp_rif_info_set_imp_unsafe(
            unit,
            term_info->rif,
            rif_info
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_label_map_add_internal_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_rif_mpls_label_map_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  SOC_PB_PP_MPLS_TERM_LKUP_INFO
    lkup_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_LABEL_RIF_KEY, mpls_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(lif_index, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_INFO, term_info, 30, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_RIF_INFO, rif_info, 40, exit);
  res = soc_pb_pp_mpls_term_lkup_info_get_unsafe(unit,&lkup_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (lkup_info.key_type == SOC_PB_PP_MPLS_TERM_KEY_TYPE_LABEL && mpls_key->vsid != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_label_map_add_verify()", 0, 0);
}

/*********************************************************************
*     Remove MPLS label that was mapped to a RIF-Tunnel
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_label_map_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    isem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    isem_entry;
  uint8
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  /* SEM table */
  res = soc_pb_pp_rif_mpls_key_to_isem_key_unsafe(
          unit,
          mpls_key,
          &isem_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = soc_pb_pp_isem_access_entry_get_unsafe(
          unit,
          &isem_key,
          &isem_entry,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if(success == TRUE)
  {
    /* remove from SEM */
    *lif_index = isem_entry.sem_result_ndx;
    res = soc_pb_pp_isem_access_entry_remove_unsafe(
            unit,
            &isem_key,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

   /*
    * set entry type
    */
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            *lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else
  {
    *lif_index = SOC_PPD_LIF_NULL;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_label_map_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_rif_mpls_label_map_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_LABEL_RIF_KEY, mpls_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_label_map_remove_verify()", 0, 0);
}

/*********************************************************************
*     Enable MPLS labels termination and setting the Router
 *     interface according to the terminated MPLS label.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_label_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ID                             *lif_index,
    SOC_SAND_OUT  SOC_PB_PP_MPLS_TERM_INFO                     *term_info,
    SOC_SAND_OUT  SOC_PB_PP_RIF_INFO                           *rif_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  /* SEM table */
  res = soc_pb_pp_rif_mpls_label_map_get_internal_unsafe(
          unit,
          mpls_key,
          FALSE,
          lif_index,
          term_info,
          rif_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_label_map_get_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_rif_mpls_label_map_get_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ID                             *lif_index,
    SOC_SAND_OUT  SOC_PB_PP_MPLS_TERM_INFO                     *term_info,
    SOC_SAND_OUT  SOC_PB_PP_RIF_INFO                           *rif_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    isem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    isem_entry;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA
    sem_res_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_MPLS_TERM_INFO_clear(term_info);
  SOC_PB_PP_RIF_INFO_clear(rif_info);

  if (!ignore_key)
  {
    SOC_PB_PP_CLEAR(&isem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    SOC_PB_PP_CLEAR(&isem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
    
    /* SEM table */
    res = soc_pb_pp_rif_mpls_key_to_isem_key_unsafe(
            unit,
            mpls_key,
            &isem_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_pb_pp_isem_access_entry_get_unsafe(
            unit,
            &isem_key,
            &isem_entry,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    *found = TRUE;
    isem_entry.sem_result_ndx = *lif_index;
  }

  if(!*found)
  {
    goto exit;
  }

  *lif_index = isem_entry.sem_result_ndx;
  res = soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_get_unsafe(
          unit,
          *lif_index,
          &sem_res_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* found but LIF not valid */
  if (sem_res_tbl_data.valid == 0)
  {
    *found = FALSE;
    goto exit;
  }

  term_info->cos_profile = sem_res_tbl_data.cos_profile;

  if(sem_res_tbl_data.in_rif_valid)
  {
    term_info->rif = sem_res_tbl_data.in_rif;
  }
  else
  {
    term_info->rif = SOC_PPD_RIF_NULL;
  }

  term_info->next_prtcl = \
    (sem_res_tbl_data.type == SOC_PB_PP_RIF_ISEM_RES_MPLS_TYPE_LSP) ? SOC_PB_PP_L3_NEXT_PRTCL_TYPE_MPLS : SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IPV4;

  term_info->processing_type = (sem_res_tbl_data.model_is_pipe == 1) ? SOC_PB_PP_MPLS_TERM_MODEL_PIPE : SOC_PB_PP_MPLS_TERM_MODEL_UNIFORM;

  /* RIF table */
  if(term_info->rif != SOC_PPD_RIF_NULL)
  {
    res =  soc_pb_pp_rif_info_get_imp_unsafe(
            unit,
            term_info->rif,
            rif_info
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_label_map_get_internal_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_rif_mpls_label_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_LABEL_RIF_KEY, mpls_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_mpls_label_map_get_verify()", 0, 0);
}


/*********************************************************************
*     Enable IP Tunnels termination and setting the Router
 *     interface according to the terminated IP tunnel.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_ip_tunnel_map_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_rif_ip_tunnel_map_add_internal_unsafe(
          unit,
          dip_key,
          FALSE,
          lif_index,
          term_info,
          rif_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * success or overwriting exist entry
  */
  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_IP_TUNNEL_RIF
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ip_tunnel_map_add_unsafe()", dip_key, 0);
}

uint32
  soc_pb_pp_rif_ip_tunnel_map_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    isem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    isem_entry;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_DATA
    sem_res_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_CLEAR(&isem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
  SOC_PB_PP_CLEAR(&isem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
  
  if (!ignore_key)
  {
    /* SEM table */
    isem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL;
    isem_key.key_info.ip_tunnel.dip = dip_key;

    isem_entry.sem_result_ndx = lif_index;
    isem_entry.sem_op_code = SOC_PB_PP_RIF_ISEM_ENTRY_OP_CODE;
    
    res = soc_pb_pp_isem_access_entry_add_unsafe(
            unit,
            &isem_key,
            &isem_entry,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(*success != SOC_SAND_SUCCESS)
    {
      goto exit;
    }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  res = soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_get_unsafe(
          unit,
          lif_index,
          &sem_res_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  sem_res_tbl_data.cos_profile = term_info->cos_profile;
  if(term_info->rif != SOC_PPD_RIF_NULL)
  {
    sem_res_tbl_data.in_rif = term_info->rif;
    sem_res_tbl_data.in_rif_valid = 1;
  }
  else
  {
    sem_res_tbl_data.in_rif_valid = 0;
  }
  sem_res_tbl_data.service_type = SOC_PB_PP_RIF_ISEM_RES_SERVICE_TYPE;
  
  res = soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_set_unsafe(
          unit,
          lif_index,
          &sem_res_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  /* RIF table */
  if(term_info->rif != SOC_PPD_RIF_NULL)
  {
    res =  soc_pb_pp_rif_info_set_imp_unsafe(
            unit,
            term_info->rif,
            rif_info
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ip_tunnel_map_add_internal_unsafe()", dip_key, 0);
}

uint32
  soc_pb_pp_rif_ip_tunnel_map_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(dip_key, SOC_PB_PP_RIF_DIP_KEY_MAX, SOC_PB_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(lif_index, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_RIF_IP_TERM_INFO, term_info, 30, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_RIF_INFO, rif_info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ip_tunnel_map_add_verify()", dip_key, 0);
}

/*********************************************************************
*     Remove the IP Tunnel
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_ip_tunnel_map_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    isem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    isem_entry;
  uint8
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  SOC_PB_PP_CLEAR(&isem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
  
  /* SEM table */
  isem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL;
  isem_key.key_info.ip_tunnel.dip = dip_key;
  
  res = soc_pb_pp_isem_access_entry_get_unsafe(
          unit,
          &isem_key,
          &isem_entry,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if(success == TRUE)
  {
    /* remove from SEM table */
    *lif_index = isem_entry.sem_result_ndx;
    res = soc_pb_pp_isem_access_entry_remove_unsafe(
            unit,
            &isem_key,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

   /*
    * set entry type
    */
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            *lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  }
  else
  {
    *lif_index = SOC_PPD_LIF_NULL;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ip_tunnel_map_remove_unsafe()", dip_key, 0);
}

uint32
  soc_pb_pp_rif_ip_tunnel_map_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(dip_key, SOC_PB_PP_RIF_DIP_KEY_MAX, SOC_PB_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ip_tunnel_map_remove_verify()", dip_key, 0);
}

/*********************************************************************
*     Enable IP Tunnels termination and setting the Router
 *     interface according to the terminated IP tunnel.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_ip_tunnel_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ID                             *lif_index,
    SOC_SAND_OUT  SOC_PB_PP_RIF_IP_TERM_INFO                   *term_info,
    SOC_SAND_OUT  SOC_PB_PP_RIF_INFO                           *rif_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_rif_ip_tunnel_map_get_internal_unsafe(
          unit,
          dip_key,
          FALSE,
          lif_index,
          term_info,
          rif_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ip_tunnel_map_get_unsafe()", dip_key, 0);
}


uint32
  soc_pb_pp_rif_ip_tunnel_map_get_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ID                             *lif_index,
    SOC_SAND_OUT  SOC_PB_PP_RIF_IP_TERM_INFO                   *term_info,
    SOC_SAND_OUT  SOC_PB_PP_RIF_INFO                           *rif_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    isem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    isem_entry;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_DATA
    sem_res_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_RIF_IP_TERM_INFO_clear(term_info);
  SOC_PB_PP_RIF_INFO_clear(rif_info);

  if (!ignore_key)
  {
    SOC_PB_PP_CLEAR(&isem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    SOC_PB_PP_CLEAR(&isem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
    
    /* SEM table */
    isem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL;
    isem_key.key_info.ip_tunnel.dip = dip_key;

    res = soc_pb_pp_isem_access_entry_get_unsafe(
            unit,
            &isem_key,
            &isem_entry,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(!*found)
    {
      goto exit;
    }
    *lif_index = isem_entry.sem_result_ndx;
  }
  else
  {
    *found = TRUE;
  }

  res = soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_get_unsafe(
          unit,
          *lif_index,
          &sem_res_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  term_info->cos_profile = sem_res_tbl_data.cos_profile;
  if(sem_res_tbl_data.in_rif_valid)
  {
     term_info->rif = sem_res_tbl_data.in_rif;
  }
  else
  {
    term_info->rif = SOC_PPD_RIF_NULL;
  }

    /* RIF table */
  if(term_info->rif != SOC_PPD_RIF_NULL)
  {
    res =  soc_pb_pp_rif_info_get_imp_unsafe(
            unit,
            term_info->rif,
            rif_info
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ip_tunnel_map_get_internal_unsafe()", dip_key, 0);
}

uint32
  soc_pb_pp_rif_ip_tunnel_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(dip_key, SOC_PB_PP_RIF_DIP_KEY_MAX, SOC_PB_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ip_tunnel_map_get_verify()", dip_key, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_vsid_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_VSID_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res =  soc_pb_pp_rif_info_set_imp_unsafe(
          unit,
          vsid_ndx,
          rif_info
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_vsid_map_set_unsafe()", vsid_ndx, 0);
}

uint32
  soc_pb_pp_rif_vsid_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_VSID_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsid_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_RIF_INFO, rif_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_vsid_map_set_verify()", vsid_ndx, 0);
}

uint32
  soc_pb_pp_rif_vsid_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_VSID_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsid_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_vsid_map_get_verify()", vsid_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_vsid_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_VSID_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PB_PP_RIF_INFO_clear(rif_info);

  res =  soc_pb_pp_rif_info_get_imp_unsafe(
          unit,
          vsid_ndx,
          rif_info
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_vsid_map_get_unsafe()", vsid_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res =  soc_pb_pp_rif_info_set_imp_unsafe(
           unit,
           rif_ndx,
           rif_info
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_info_set_unsafe()", rif_ndx, 0);
}

uint32
  soc_pb_pp_rif_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rif_ndx, SOC_PB_PP_RIF_ID_MAX, SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_RIF_INFO, rif_info, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_info_set_verify()", rif_ndx, 0);
}

uint32
  soc_pb_pp_rif_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rif_ndx, SOC_PB_PP_RIF_ID_MAX, SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_info_get_verify()", rif_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx,
    SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PB_PP_RIF_INFO_clear(rif_info);

  res =  soc_pb_pp_rif_info_get_imp_unsafe(
          unit,
          rif_ndx,
          rif_info
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_info_get_unsafe()", rif_ndx, 0);
}

/*********************************************************************
*     Set TTL value for TTL-scope.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_ttl_scope_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  )
{
  uint32
    reg_vals[2];
  uint32
    tmp;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_TTL_SCOPE_SET_UNSAFE);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_CLEAR(reg_vals,uint32,2);
  
  regs = soc_pb_pp_regs();

  res = soc_petra_read_reg_buffer_unsafe(
          unit,
          (SOC_PETRA_REG_ADDR*)&(regs->egq.ttl_scope_reg_0),
          SOC_PETRA_DEFAULT_INSTANCE,
          2,
          reg_vals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = (uint32)ttl_val;

  res = soc_sand_bitstream_set_any_field(
          &tmp,
          ttl_scope_ndx * SOC_SAND_PP_IP_TTL_NOF_BITS,
          SOC_SAND_PP_IP_TTL_NOF_BITS,
          reg_vals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_write_reg_buffer_unsafe(
          unit,
          (SOC_PETRA_REG_ADDR*)&(regs->egq.ttl_scope_reg_0),
          SOC_PETRA_DEFAULT_INSTANCE,
          2,
          reg_vals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ttl_scope_set_unsafe()", ttl_scope_ndx, 0);
}

uint32
  soc_pb_pp_rif_ttl_scope_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_TTL_SCOPE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ttl_scope_ndx, SOC_PB_PP_RIF_TTL_SCOPE_NDX_MAX, SOC_PB_PP_RIF_TTL_SCOPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(ttl_val, SOC_SAND_PP_IP_TTL_MAX, SOC_SAND_PP_IP_TTL_OUT_OF_RANGE_ERR, 20, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ttl_scope_set_verify()", ttl_scope_ndx, 0);
}

uint32
  soc_pb_pp_rif_ttl_scope_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_TTL_SCOPE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ttl_scope_ndx, SOC_PB_PP_RIF_TTL_SCOPE_NDX_MAX, SOC_PB_PP_RIF_TTL_SCOPE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ttl_scope_get_verify()", ttl_scope_ndx, 0);
}

/*********************************************************************
*     Set TTL value for TTL-scope.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_rif_ttl_scope_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl_val
  )
{
  uint32
    reg_vals[2];
  uint32
    tmp;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_RIF_TTL_SCOPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ttl_val);

  SOC_PB_PP_CLEAR(reg_vals,uint32,2);
  
  regs = soc_pb_pp_regs();

  res = soc_petra_read_reg_buffer_unsafe(
          unit,
          (SOC_PETRA_REG_ADDR*)&(regs->egq.ttl_scope_reg_0),
          SOC_PETRA_DEFAULT_INSTANCE,
          2,
          reg_vals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = 0;

  res = soc_sand_bitstream_get_any_field(
          reg_vals,
          ttl_scope_ndx * SOC_SAND_PP_IP_TTL_NOF_BITS,
          SOC_SAND_PP_IP_TTL_NOF_BITS,
          &tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *ttl_val = (SOC_SAND_PP_IP_TTL)tmp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_rif_ttl_scope_get_unsafe()", ttl_scope_ndx, 0);
}
/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_rif module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_rif_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_rif;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_rif module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_rif_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_rif;
}
uint32
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_RIF_MPLS_LABELS_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_label, SOC_PB_PP_RIF_FIRST_LABEL_MAX, SOC_PB_PP_RIF_FIRST_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_label, SOC_PB_PP_RIF_LAST_LABEL_MAX, SOC_PB_PP_RIF_LAST_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  if(info->first_label > info->last_label)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_RIF_MPLS_LABEL_INVALID_RANGE_ERR, 20, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_RIF_MPLS_LABELS_RANGE_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_LABEL_RIF_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->label_id, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsid, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_LABEL_RIF_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_RIF_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->vrf_id != 0)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->vrf_id, SOC_PB_PP_VRF_ID_MIN, SOC_PB_PP_VRF_ID_MAX, SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR, 11, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, SOC_PB_PP_RIF_COS_PROFILE_MAX, SOC_PB_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl_scope_index, SOC_PB_PP_RIF_TTL_SCOPE_INDEX_MAX, SOC_PB_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->routing_enablers_bm, SOC_PB_PP_RIF_ROUTING_ENABLERS_BM_MAX, SOC_PB_PP_RIF_ROUTING_ENABLERS_BM_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_RIF_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_RIF_IP_TERM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(info->rif, SOC_PB_PP_RIF_ID_MAX, SOC_PB_PP_RIF_NULL, SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, SOC_PB_PP_RIF_COS_PROFILE_MAX, SOC_PB_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_RIF_IP_TERM_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

