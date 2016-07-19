/* $Id: pb_pp_fp_egr.c,v 1.9 Broadcom SDK $
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
*/

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
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_egr.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

#include <soc/dpp/Petra/PB_TM/pb_egr_acl.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FP_EGR_DB_ID_NDX_MAX                             (SOC_PB_PP_FP_NOF_DBS-1)
#define SOC_PB_PP_FP_EGR_ENTRY_ID_NDX_MAX                          (SOC_PB_PP_FP_NOF_ENTRY_IDS-1)
#define SOC_PB_PP_FP_EGR_MANAGE_TYPE_MAX                           (SOC_PB_PP_NOF_FP_EGR_MANAGE_TYPES-1)

#define SOC_PB_PP_FP_EGR_ACTION_DP_LSB                             (0)
#define SOC_PB_PP_FP_EGR_ACTION_DP_MSB                             (0)
#define SOC_PB_PP_FP_EGR_ACTION_TC_LSB                             (1)
#define SOC_PB_PP_FP_EGR_ACTION_TC_MSB                             (3)

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
    Soc_pb_pp_procedure_desc_element_fp_egr[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_CREATE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_CREATE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_CREATE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_DESTROY_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_DESTROY_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DATABASE_DESTROY_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_ENTRY_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_ID_MANAGE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_ID_MANAGE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_ID_MANAGE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_ID_MANAGE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_FWD_TYPE_CONVERT),



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_fp_egr[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'db_id_ndx' is out of range. \n\r "
    "The range is: 0 - 127.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_EGR_ENTRY_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_EGR_ENTRY_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'entry_id_ndx' is out of range. \n\r "
    "The range is: 0 - 127.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_EGR_IS_FOUND_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_EGR_IS_FOUND_OUT_OF_RANGE_ERR",
    "The parameter 'is_found' is out of range. \n\r "
    "The range is: 0 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_EGR_HW_EGR_DB_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_EGR_HW_EGR_DB_OUT_OF_RANGE_ERR",
    "The parameter 'hw_egr_db' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_FP_EGR_MANAGE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_EGR_MANAGE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'manage_type' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_EGR_QUALS_NOT_PREDEFINED_KEY_ERR,
    "SOC_PB_PP_FP_EGR_QUALS_NOT_PREDEFINED_KEY_ERR",
    "The Database egress qualifiers are not all part \n\r "
    "of the same predefined egress ACL Key.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ACTION_TYPES_NOT_EGRESS_ERR,
    "SOC_PB_PP_FP_ACTION_TYPES_NOT_EGRESS_ERR",
    "The Database egress action types are not all part \n\r "
    "of the egress action types set.\n\r ",
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
  soc_pb_pp_fp_egr_fwd_type_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_OUT SOC_PB_PKT_FRWRD_TYPE            *fwd_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_FWD_TYPE_CONVERT);

  switch (fwd_type_ndx)
  {
  case SOC_PB_PP_FP_FWD_TYPE_BRIDGED:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_BRIDGE;
  	break;
  case SOC_PB_PP_FP_FWD_TYPE_IPV4_UC:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_IPV4_UC;
    break;
  case SOC_PB_PP_FP_FWD_TYPE_IPV4_MC:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_IPV4_MC;
    break;
  case SOC_PB_PP_FP_FWD_TYPE_IPV6_UC:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_IPV6_UC;
    break;
  case SOC_PB_PP_FP_FWD_TYPE_IPV6_MC:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_IPV6_MC;
    break;
  case SOC_PB_PP_FP_FWD_TYPE_MPLS:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_MPLS;
    break;
  case SOC_PB_PP_FP_FWD_TYPE_TRILL:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_TRILL;
    break;
  case SOC_PB_PP_FP_FWD_TYPE_BRIDGED_AFTER_TERM:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM;
    break;
  case SOC_PB_PP_FP_FWD_TYPE_CPU_TRAP:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_CPU_TRAP;
    break;
  case SOC_PB_PP_FP_FWD_TYPE_TM:
    *fwd_type = SOC_PB_PKT_FRWRD_TYPE_TM;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_FWD_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_fwd_type_convert()", 0, 0);
}



/*********************************************************************
*     Create an Egress ACL database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_database_create_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    tcam_db_id,
    hw_egr_db,
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY
    predefined_acl_key;
  SOC_PB_EGR_ACL_DB_TYPE
    db_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DATABASE_CREATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   * Verify for Egress
   */
  res = soc_pb_pp_fp_egr_database_create_verify(
          unit,
          db_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Add the Database for the Egress ACL
   */
  res = soc_pb_pp_fp_egr_db_id_manage_unsafe(
          unit,
          db_id_ndx,
          SOC_PB_PP_FP_EGR_MANAGE_TYPE_ADD,
          &hw_egr_db,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  /*
   * Set it if success
   */
  if (*success == SOC_SAND_SUCCESS)
  {
    /*
     * Get the Predefined key
     */
    res = soc_pb_pp_fp_key_get(
            unit,
            TRUE, /* is_for_egress */
            info->qual_types,
            &predefined_acl_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_pb_pp_sw_db_fp_db_predefined_acl_key_set(
      unit,
      db_id_ndx,
      predefined_acl_key
    );

    switch (predefined_acl_key)
    {
    case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH:
      db_type = SOC_PB_EGR_ACL_DB_TYPE_ETH;
      break;
    case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4:
      db_type = SOC_PB_EGR_ACL_DB_TYPE_IPV4;
      break;
    case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS:
      db_type = SOC_PB_EGR_ACL_DB_TYPE_MPLS;
      break;
    case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_TM:
      db_type = SOC_PB_EGR_ACL_DB_TYPE_TM;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_EGR_QUALS_NOT_PREDEFINED_KEY_ERR, 30, exit);
    }

    tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
    res = soc_pb_egr_acl_db_create_unsafe(
            unit,
            hw_egr_db, /* Up to 8 */
            tcam_db_id,
            db_type,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_database_create_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_egr_database_create_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *info
  )
{
  uint32
    res;
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY
    predefined_acl_key;
  uint32
    action_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DATABASE_CREATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_EGR_DB_ID_NDX_MAX, SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /*
   * 1. Verify an Egress DB type
   */
  if (info->db_type != SOC_PB_PP_FP_DB_TYPE_EGRESS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DATABASE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  /*
   * 2. Verify the Qualifier types correspond to an Egress predefined key
   */
  res = soc_pb_pp_fp_key_get(
          unit,
          TRUE, /* is_for_egress */
          info->qual_types,
          &predefined_acl_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (predefined_acl_key == SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_EGR_QUALS_NOT_PREDEFINED_KEY_ERR, 40, exit);
  }

  /*
   * 3. Verify the Action types are all egress compliant
   */
  for (action_type_ndx = 0; action_type_ndx <= SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    if (
        (info->action_types[0] != SOC_PB_PP_NOF_FP_ACTION_TYPES)
        && (info->action_types[0] != SOC_PB_PP_FP_ACTION_TYPE_NOP)
        && (info->action_types[0] != SOC_PB_PP_FP_ACTION_TYPE_EGR_TRAP)
        && (info->action_types[0] != SOC_PB_PP_FP_ACTION_TYPE_EGR_OFP)
        && (info->action_types[0] != SOC_PB_PP_FP_ACTION_TYPE_EGR_TC_DP)
        && (info->action_types[0] != SOC_PB_PP_FP_ACTION_TYPE_EGR_OUTLIF)
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ACTION_TYPES_NOT_EGRESS_ERR, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_database_create_verify()", db_id_ndx, 0);
}

/*********************************************************************
*     Get the database parameters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_database_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DATABASE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Verify for Egress
   */
  res = soc_pb_pp_fp_egr_database_get_verify(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_database_get_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_egr_database_get_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DATABASE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_EGR_DB_ID_NDX_MAX, SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_database_get_verify()", db_id_ndx, 0);
}

/*********************************************************************
*     Destroy the database: all its entries are suppressed and
 *     the Database-ID is freed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_database_destroy_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx
  )
{
  uint32
    tcam_db_id,
    hw_egr_db,
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE
    success = SOC_SAND_SUCCESS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DATABASE_DESTROY_UNSAFE);

  /*
   * Verify for Egress
   */
  res = soc_pb_pp_fp_egr_database_destroy_verify(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Destroy the SW DB mapping and get the HW profile
   */
  res = soc_pb_pp_fp_egr_db_id_manage_unsafe(
          unit,
          db_id_ndx,
          SOC_PB_PP_FP_EGR_MANAGE_TYPE_RMV,
          &hw_egr_db,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (success == SOC_SAND_SUCCESS)
  {
    /*
     * The Database exists
     */
    tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
    res = soc_pb_egr_acl_db_destroy_unsafe(
            unit,
            hw_egr_db,
            tcam_db_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_pb_tcam_access_profile_destroy_unsafe(
            unit,
            tcam_db_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_database_destroy_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_egr_database_destroy_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DATABASE_DESTROY_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_EGR_DB_ID_NDX_MAX, SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_database_destroy_verify()", db_id_ndx, 0);
}

/*
 * Get predefined key sizes
 */
STATIC
  uint32
    soc_pb_pp_fp_predefined_key_size_and_type_get(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY   predefined_acl_key,
      SOC_SAND_OUT SOC_PB_TCAM_BANK_ENTRY_SIZE      *entry_size,
      SOC_SAND_OUT SOC_PB_TCAM_KEY_FORMAT           *tcam_key_format
    )
{
  uint32
    res;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    size;
  SOC_PB_PMF_TCAM_KEY_SRC
    pmf_key_src = SOC_PB_NOF_PMF_TCAM_KEY_SRCS;
  SOC_PB_EGR_ACL_DB_TYPE
    egr_acl_db_type = SOC_PB_EGR_NOF_ACL_DB_TYPES;
  SOC_PB_TCAM_KEY_FORMAT_TYPE
    type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PREDEFINED_KEY_SIZE_AND_TYPE_GET);

  SOC_PB_TCAM_KEY_FORMAT_clear(tcam_key_format);

  switch (predefined_acl_key)
  {
  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2:
    size = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
    pmf_key_src = SOC_PB_PMF_TCAM_KEY_SRC_L2;
    type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4:
    size = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
    pmf_key_src = SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4;
    type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6:
    size = SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS;
    pmf_key_src = SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6;
    type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH:
    size = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
    egr_acl_db_type = SOC_PB_EGR_ACL_DB_TYPE_ETH;
    type = SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4:
    size = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
    egr_acl_db_type = SOC_PB_EGR_ACL_DB_TYPE_IPV4;
    type = SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS:
    size = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
    egr_acl_db_type = SOC_PB_EGR_ACL_DB_TYPE_MPLS;
    type = SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL;
    break;

  case SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_TM:
    size = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
    egr_acl_db_type = SOC_PB_EGR_ACL_DB_TYPE_TM;
    type = SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL;
    break;

  default:
    size = SOC_PB_TCAM_NOF_BANK_ENTRY_SIZES;
    pmf_key_src = SOC_PB_NOF_PMF_TCAM_KEY_SRCS;
    type = SOC_PB_TCAM_NOF_KEY_FORMAT_TYPES;
    break;
  }

  *entry_size = size;
  tcam_key_format->type = type;
  tcam_key_format->pmf = pmf_key_src;
  tcam_key_format->egr_acl = egr_acl_db_type;

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_predefined_key_size_and_type_get()", 0, 0);
}

/*
 * Convert the predefined qualifier type to a TCAM field type
 */
STATIC
  uint32
    soc_pb_pp_fp_qual_type_to_tcam_type_convert(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT           *key_format,
      SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE            qual_type,
      SOC_SAND_OUT uint32                    *tcam_type /*SOC_PB_TCAM_KEY_FLD_TYPE*/
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tcam_key_fld_type = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_QUAL_TYPE_TO_TCAM_TYPE_CONVERT);

  if (key_format->type == SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF)
  {
    switch (key_format->pmf)
    {
    case SOC_PB_PMF_TCAM_KEY_SRC_L2:
      switch (qual_type)
      {
        /*
         * L2
         */
      case SOC_PB_PP_FP_QUAL_HDR_VLAN_FORMAT:
        tcam_key_fld_type = SOC_PB_PMF_TCAM_FLD_L2_LLVP;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG:
        tcam_key_fld_type = SOC_PB_PMF_TCAM_FLD_L2_STAG;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_SA:
        tcam_key_fld_type = SOC_PB_PMF_TCAM_FLD_L2_SA;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_DA:
        tcam_key_fld_type = SOC_PB_PMF_TCAM_FLD_L2_DA;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_ETHERTYPE:
        tcam_key_fld_type = SOC_PB_PMF_TCAM_FLD_L2_ETHERTYPE;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_2ND_VLAN_TAG:
        tcam_key_fld_type = SOC_PB_PMF_TCAM_FLD_L2_CTAG_IN_AC;
        break;

      case SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT:
        tcam_key_fld_type = SOC_PB_PMF_TCAM_FLD_L2_IN_PP_PORT;
	      break;

      default:
        break;
      }
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
      switch (qual_type)
      {
        /*
         * L3 IPv4
         */
      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DF:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_DF;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_MF:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_MF;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SIP:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_SIP;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DIP:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_DIP;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SRC_PORT:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_SRC_PORT;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DEST_PORT:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_DEST_PORT;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_NEXT_PRTCL:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_NEXT_PRTCL;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TOS:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_TOS;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TCP_CTL:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_TCP_CTL;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_HI:
      case SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_LOW:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_L4OPS;
        break;
   
      case SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_IN_PP_PORT;
        break;
   
      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_IN_VID:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV4_IN_VID;
        break;
      default:
        break;
      }
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
      switch (qual_type)
      {
        /*
         * L3 IPv6
         */
      case SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV6_SIP_HIGH;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV6_SIP_LOW;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV6_DIP_HIGH;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV6_DIP_LOW;
        break;

      case SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV6_NEXT_PRTCL;
        break;
    
      case SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV6_PP_PORT_TCP_CTL;
        break;
    
      case SOC_PB_PP_FP_QUAL_HDR_IPV6_L4OPS:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV6_L4OPS;
        break;

      case SOC_PB_PP_FP_QUAL_IRPP_IN_LIF:
        tcam_key_fld_type = SOC_PB_PMF_FLD_IPV6_IN_AC_VRF;
        break;
      default:
          break;
       }
      break;

    default:
      break;
    }
  }
  else
  {
   switch (key_format->egr_acl)
    {
    case SOC_PB_EGR_ACL_DB_TYPE_ETH:
      switch (qual_type)
      {
        /*
         * Egress Ethernet
         */
      case SOC_PB_PP_FP_QUAL_HDR_FWD_ETHERTYPE:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE;
        break;
      case SOC_PB_PP_FP_QUAL_IRPP_ETH_TAG_FORMAT:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_L2_OUTER_TAG;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_FWD_2ND_VLAN_TAG:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_L2_INNER_TAG;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_FWD_SA:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_L2_SA;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_FWD_DA:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_L2_DA;
        break;
      case SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA;
        break;
      default:
        break;
      }
    	break;
        
    case SOC_PB_EGR_ACL_DB_TYPE_IPV4:
      switch (qual_type)
      {
        /*
         * Egress IPv4
         */
      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TOS:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_TOS;
        break;
      case SOC_PB_PP_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SIP:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_SIP;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DIP:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_DIP;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID;
        break;
      case SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA;
        break;
      default:
        break;
     }
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_MPLS:
      switch (qual_type)
      {
        /*
         * Egress MPLS
         */
      case SOC_PB_PP_FP_QUAL_ERPP_FTMH:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_FTMH;
        break;
      case SOC_PB_PP_FP_QUAL_ERPP_PAYLOAD:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL_ID_FWD:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_LABEL;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP_FWD:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_EXP;
        break;
      case SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL_FWD:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_TTL;
        break;
      case SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA;
        break;
     default:
        break;
      }
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_TM:
      switch (qual_type)
      {
        /*
         * Egress TM
         */
      case SOC_PB_PP_FP_QUAL_ERPP_FTMH:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_TM_FTMH;
        break;
      case SOC_PB_PP_FP_QUAL_ERPP_PAYLOAD:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_TM_HDR_DATA;
        break;
      case SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA:
        tcam_key_fld_type = SOC_PB_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA;
        break;
      default:
        break;
      }

      break;
  
  default:
    	break;
    }
  }

  *tcam_type = tcam_key_fld_type;

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_type_to_tcam_type_convert()", 0, 0);
}


/*
 * Get the Key field according to the qualifier type
 */
STATIC
  uint32
    soc_pb_pp_fp_qual_type_to_key_fld_convert(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT           *key_format,
      SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE            qual_type,
      SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_TYPE     *tcam_key_fld_type
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_QUAL_TYPE_TO_KEY_FLD_CONVERT);

  SOC_PB_TCAM_KEY_FLD_TYPE_clear(key_format, tcam_key_fld_type);

  res = soc_pb_pp_fp_qual_type_to_tcam_type_convert(
          unit,
          key_format,
          qual_type,
          &fld_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (key_format->type == SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF)
  {
    switch (key_format->pmf)
    {
    case SOC_PB_PMF_TCAM_KEY_SRC_L2:
      tcam_key_fld_type->l2 = fld_type;
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
      tcam_key_fld_type->l3.ipv4_fld = fld_type;
      tcam_key_fld_type->l3.mode = SOC_PB_PMF_TCAM_FLD_L3_MODE_IPV4;
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
      tcam_key_fld_type->l3.ipv6_fld = fld_type;
      tcam_key_fld_type->l3.mode = SOC_PB_PMF_TCAM_FLD_L3_MODE_IPV6;
      break;

    default:
      break;
    }
  }
  else
  {
    switch (key_format->egr_acl)
    {
    case SOC_PB_EGR_ACL_DB_TYPE_ETH:
      tcam_key_fld_type->egr_l2 = fld_type;
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_IPV4:
      tcam_key_fld_type->egr_ipv4 = fld_type;
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_MPLS:
      tcam_key_fld_type->egr_mpls = fld_type;
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_TM:
      tcam_key_fld_type->egr_tm = fld_type;
      break;

    default:
      break;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_type_to_key_fld_convert()", 0, 0);
}



/*********************************************************************
*     Add an entry to the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_tcam_entry_add_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO      *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    qual_lsb,
    qual_length_no_padding,
    action_lsb,
    action_size,
    action_size_in_bits_in_fem,
    hw_egr_db,
    res = SOC_SAND_OK;
  SOC_PB_EGR_ACL_ENTRY_INFO
    egr_info;
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY
    predefined_acl_key;
  SOC_PB_TCAM_KEY_FORMAT
    key_format;
  SOC_PB_TCAM_KEY_FLD_TYPE
    tcam_key_fld_type;
  SOC_PB_TCAM_KEY_FLD_VAL
    fld_val,
    mask_val;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;
  SOC_PB_PMF_TCAM_ENTRY_ID
    entry;
  SOC_PB_PMF_TCAM_DATA
    tcam_data;
  SOC_PB_TCAM_KEY
    key;
  uint32
    arr_ndx,
    action_type_ndx,
    qual_type_ndx;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PMF_TCAM_DATA_clear(&tcam_data);

  /*
   * Get the Database info
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  /*
   * 2. Insert the TCAM Key
   */
  if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
  {
    /*
     *  Build the TCAM with the KEY manager functions
     */
    for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
    {
      if ((info->qual_vals[qual_type_ndx].type == SOC_PB_PP_NOF_FP_QUAL_TYPES) || (info->qual_vals[qual_type_ndx].type == BCM_FIELD_ENTRY_INVALID))
      {
        continue;
      }
      /* Find the Qualifier in the DB info list */
      res = soc_pb_pp_fp_qual_lsb_and_length_get(
              unit,
              db_id_ndx,
              info->qual_vals[qual_type_ndx].type,
              &fp_database_info,
              &qual_lsb,
              &qual_length_no_padding
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
      /* Set the data and mask */
      res = soc_sand_bitstream_set_any_field(
              info->qual_vals[qual_type_ndx].val.arr,
              qual_lsb,
              qual_length_no_padding,
              tcam_data.key.data.val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

      res = soc_sand_bitstream_set_any_field(
              info->qual_vals[qual_type_ndx].is_valid.arr,
              qual_lsb,
              qual_length_no_padding,
              tcam_data.key.data.mask
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);
    }

    SOC_PB_PMF_TCAM_ENTRY_ID_clear(&entry);
    entry.db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
    entry.entry_id = entry_id_ndx;
    tcam_data.priority = info->priority;
    tcam_data.output.val = 0;
    tcam_data.key.size = soc_pb_pp_sw_db_fp_tcam_db_key_size_get(
                           unit,
                           db_id_ndx
                         );
    tcam_data.key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
    {
      res = soc_pb_pp_fp_action_lsb_get(
              unit,
              info->actions[action_type_ndx].type,
              &fp_database_info,
              &action_lsb
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

      res = soc_pb_pp_fp_action_type_max_size_get(
              unit,
              info->actions[action_type_ndx].type,
              &action_size,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
      tcam_data.output.val |= SOC_SAND_SET_BITS_RANGE(info->actions[action_type_ndx].val, action_size + action_lsb - 1, action_lsb);
    }

    res = soc_pb_pmf_tcam_entry_add_unsafe(
            unit,
            &entry,
            &tcam_data,
            TRUE,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  else /* Egress */
  {
    /*
     * Get the Key type
     */
    soc_pb_pp_sw_db_fp_db_predefined_acl_key_get(
        unit,
        db_id_ndx,
        &predefined_acl_key
      );

    /*
     * 1. Build the TCAM Key
     */
    res = soc_pb_pp_fp_predefined_key_size_and_type_get(
            unit,
            predefined_acl_key,
            &entry_size,
            &key_format
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_pb_tcam_key_clear_unsafe(
            unit,
            &key_format,
            &key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
    {
      if ((info->qual_vals[qual_type_ndx].type == SOC_PB_PP_NOF_FP_QUAL_TYPES) || (info->qual_vals[qual_type_ndx].type == BCM_FIELD_ENTRY_INVALID))
     {
        continue;
      }

      SOC_PB_TCAM_KEY_FLD_TYPE_clear(&key_format, &tcam_key_fld_type);
      res = soc_pb_pp_fp_qual_type_to_key_fld_convert(
              unit,
              &key_format,
              info->qual_vals[qual_type_ndx].type,
              &tcam_key_fld_type
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      SOC_PB_TCAM_KEY_FLD_VAL_clear(&fld_val);
      SOC_PB_TCAM_KEY_FLD_VAL_clear(&mask_val);

      for (arr_ndx = 0; arr_ndx < SOC_SAND_U64_NOF_UINT32S; ++arr_ndx)
      {
        fld_val.val[arr_ndx] = info->qual_vals[qual_type_ndx].val.arr[arr_ndx];
        mask_val.val[arr_ndx] = info->qual_vals[qual_type_ndx].is_valid.arr[arr_ndx];
      }

      res = soc_pb_tcam_key_masked_val_set_unsafe(
              unit,
              &key,
              &tcam_key_fld_type,
              &fld_val,
              &mask_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    res = soc_pb_pp_fp_egr_db_id_manage_unsafe(
            unit,
            db_id_ndx,
            SOC_PB_PP_FP_EGR_MANAGE_TYPE_GET,
            &hw_egr_db,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /*
     * Set it if the Database exists
     */
    if (*success == SOC_SAND_SUCCESS)
    {
      SOC_PB_EGR_ACL_ENTRY_INFO_clear(&egr_info);
      egr_info.priority = (uint16) info->priority;
      egr_info.type = key_format.egr_acl;
      SOC_PETRA_COPY(&(egr_info.key), &key, SOC_PB_TCAM_KEY, 1);
      /*
       * Build the action
       */
      for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
      {
        switch (info->actions[action_type_ndx].type)
        {
        case SOC_PB_PP_FP_ACTION_TYPE_EGR_TRAP:
          egr_info.action_val.trap_en = TRUE;
          egr_info.action_val.trap_code = info->actions[action_type_ndx].val;
          break;

        case SOC_PB_PP_FP_ACTION_TYPE_EGR_OFP:
          egr_info.action_val.ofp_ov = TRUE;
          egr_info.action_val.ofp = info->actions[action_type_ndx].val;
          break;

        case SOC_PB_PP_FP_ACTION_TYPE_EGR_TC_DP:
          egr_info.action_val.dp_tc_ov = TRUE;
          egr_info.action_val.dp = SOC_SAND_GET_BITS_RANGE(info->actions[action_type_ndx].val, SOC_PB_PP_FP_EGR_ACTION_DP_MSB, SOC_PB_PP_FP_EGR_ACTION_DP_LSB);
          egr_info.action_val.tc = SOC_SAND_GET_BITS_RANGE(info->actions[action_type_ndx].val, SOC_PB_PP_FP_EGR_ACTION_TC_MSB, SOC_PB_PP_FP_EGR_ACTION_TC_LSB);
          break;

        case SOC_PB_PP_FP_ACTION_TYPE_EGR_OUTLIF:
          egr_info.action_val.cud = info->actions[action_type_ndx].val;
          break;

        default:
          break;
        }
      }

      res = soc_pb_egr_acl_entry_add_unsafe(
              unit,
              hw_egr_db,
              entry_id_ndx,
              &egr_info,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_tcam_entry_add_unsafe()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_tcam_entry_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx,
    SOC_SAND_OUT uint8                *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO        *info
  )
{
  uint32
    hw_egr_db,
    qual_lsb,
    qual_length_no_padding,
    action_lsb,
    action_size,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PMF_TCAM_ENTRY_ID
    entry;
  SOC_PB_PMF_TCAM_DATA
    tcam_data;
  uint8
    is_egress_acl = TRUE,
    found;
  uint32
    action_type_ndx,
    arr_ndx,
    qual_type_ndx;
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY
    predefined_acl_key;
  SOC_PB_TCAM_KEY_FLD_TYPE
    tcam_key_fld_type;
  SOC_PB_TCAM_KEY_FLD_VAL
    fld_val,
    mask_val;
  SOC_PB_EGR_ACL_ENTRY_INFO
    egr_info;
  SOC_SAND_SUCCESS_FAILURE
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Verify
   */
  res = soc_pb_pp_fp_tcam_entry_get_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_EGR_ACL_ENTRY_INFO_clear(&egr_info);
  SOC_PB_PMF_TCAM_DATA_clear(&tcam_data);
  if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
  {
    is_egress_acl = FALSE;
    /*
     * Look directly at the TCAM
     */
    SOC_PB_PMF_TCAM_ENTRY_ID_clear(&entry);
    entry.db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
    entry.entry_id = entry_id_ndx;
    res = soc_pb_pmf_tcam_entry_get_unsafe(
            unit,
            &entry,
            &tcam_data,
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    /*
     * Interpret the data
     */
    *is_found = found;
    if (found == TRUE)
    {
      /*
       * Get the priority and the data from the TCAM data
       */
      info->priority = tcam_data.priority;

    }
  }
  else
  {
    is_egress_acl = TRUE;
    res = soc_pb_pp_fp_egr_db_id_manage_unsafe(
            unit,
            db_id_ndx,
            SOC_PB_PP_FP_EGR_MANAGE_TYPE_GET,
            &hw_egr_db,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


    /*
     * Get the entry
     */
    res = soc_pb_egr_acl_entry_get_unsafe(
            unit,
            hw_egr_db,
            entry_id_ndx,
            &egr_info,
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    *is_found = found;
    if (found == TRUE)
    {
      info->priority = egr_info.priority;
      SOC_PETRA_COPY(&(tcam_data.key), &(egr_info.key), SOC_PB_TCAM_KEY, 1);
    }
  }

  /*
   * Stop the parsing if not found
   */
  if (found == FALSE)
  {
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    info->actions[action_type_ndx].type = fp_database_info.action_types[action_type_ndx];

    if (is_egress_acl == TRUE)
    {
      tcam_data.key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL;
      /*
       * Parse the action
       */
      switch (info->actions[action_type_ndx].type)
      {
      case SOC_PB_PP_FP_ACTION_TYPE_EGR_TRAP:
        info->actions[action_type_ndx].val = egr_info.action_val.trap_code;
        break;

      case SOC_PB_PP_FP_ACTION_TYPE_EGR_OFP:
        info->actions[action_type_ndx].val = egr_info.action_val.ofp;
        break;

      case SOC_PB_PP_FP_ACTION_TYPE_EGR_TC_DP:
        info->actions[action_type_ndx].val = egr_info.action_val.dp + (egr_info.action_val.tc << SOC_PB_PP_FP_EGR_ACTION_TC_LSB);
        break;

      case SOC_PB_PP_FP_ACTION_TYPE_EGR_OUTLIF:
        info->actions[action_type_ndx].val = egr_info.action_val.cud;
        break;

      default:
        break;
      }

      /*
       * Get the Qualifiers  - get the key before
       *  For egress, handle via predefined keys
       */
      res = soc_pb_pp_fp_key_get(
              unit,
              is_egress_acl,
              fp_database_info.qual_types,
              &predefined_acl_key
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
      res = soc_pb_pp_fp_predefined_key_size_and_type_get(
              unit,
              predefined_acl_key,
              &(tcam_data.key.size),
              &(tcam_data.key.format)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      SOC_PB_TCAM_KEY_FLD_TYPE_clear(&(tcam_data.key.format), &tcam_key_fld_type);
      for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
      {
        if ((fp_database_info.qual_types[qual_type_ndx] == SOC_PB_PP_NOF_FP_QUAL_TYPES)
		 || (fp_database_info.qual_types[qual_type_ndx] == BCM_FIELD_ENTRY_INVALID))
        {
          continue;
        }

        /*
         * Extract the data from its place
         */
        info->qual_vals[qual_type_ndx].type = fp_database_info.qual_types[qual_type_ndx];

        res = soc_pb_pp_fp_qual_type_to_key_fld_convert(
                unit,
                &(tcam_data.key.format),
                info->qual_vals[qual_type_ndx].type,
                &tcam_key_fld_type
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        SOC_PB_TCAM_KEY_FLD_VAL_clear(&fld_val);
        SOC_PB_TCAM_KEY_FLD_VAL_clear(&mask_val);
        res = soc_pb_tcam_key_masked_val_get_unsafe(
                unit,
                &(tcam_data.key),
                &tcam_key_fld_type,
                &fld_val,
                &mask_val
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        /*
         * Convert TCAM values
         */
        for (arr_ndx = 0; arr_ndx < SOC_SAND_U64_NOF_UINT32S; ++arr_ndx)
        {
          info->qual_vals[qual_type_ndx].val.arr[arr_ndx] = fld_val.val[arr_ndx];
          info->qual_vals[qual_type_ndx].is_valid.arr[arr_ndx] = mask_val.val[arr_ndx];
        }
      }
    }
    else
    {
      tcam_data.key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
      res = soc_pb_pp_fp_action_lsb_get(
              unit,
              info->actions[action_type_ndx].type,
              &fp_database_info,
              &action_lsb
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

      res = soc_pb_pp_fp_action_type_max_size_get(
              unit,
              info->actions[action_type_ndx].type,
              &action_size,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
      info->actions[action_type_ndx].val = SOC_SAND_GET_BITS_RANGE(tcam_data.output.val, action_size + action_lsb - 1, action_lsb);

      /*
       *  Get the Qualifier types, values and masks
       */
      for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
      {
        if ((fp_database_info.qual_types[qual_type_ndx] == SOC_PB_PP_NOF_FP_QUAL_TYPES)
		 || (fp_database_info.qual_types[qual_type_ndx] == BCM_FIELD_ENTRY_INVALID))
        {
          continue;
        }
        info->qual_vals[qual_type_ndx].type = fp_database_info.qual_types[qual_type_ndx];

        /* Find the Qualifier in the DB info list */
        res = soc_pb_pp_fp_qual_lsb_and_length_get(
                unit,
                db_id_ndx,
                info->qual_vals[qual_type_ndx].type,
                &fp_database_info,
                &qual_lsb,
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
        /* Set the data and mask */
        res = soc_sand_bitstream_get_any_field(
                tcam_data.key.data.val,
                qual_lsb,
                qual_length_no_padding,
                info->qual_vals[qual_type_ndx].val.arr
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

        res = soc_sand_bitstream_get_any_field(
                tcam_data.key.data.mask,
                qual_lsb,
                qual_length_no_padding,
                info->qual_vals[qual_type_ndx].is_valid.arr
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_tcam_entry_get_unsafe()", db_id_ndx, entry_id_ndx);
}

uint32
  soc_pb_pp_fp_tcam_entry_get_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_EGR_DB_ID_NDX_MAX, SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id_ndx, SOC_PB_PP_FP_EGR_ENTRY_ID_NDX_MAX, SOC_PB_PP_FP_EGR_ENTRY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_tcam_entry_get_verify()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Remove an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_tcam_entry_remove_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx,
    SOC_SAND_OUT uint8                *is_found
  )
{
  uint32
    hw_egr_db,
    tcam_db_id,
    res = SOC_SAND_OK;
  SOC_PB_PMF_TCAM_ENTRY_ID
    entry;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_TCAM_LOCATION
    location;
  SOC_SAND_SUCCESS_FAILURE
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_ENTRY_REMOVE_UNSAFE);

  /*
   * Verify
   */
  res = soc_pb_pp_fp_tcam_entry_remove_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
  SOC_PB_TCAM_LOCATION_clear(&location);
  res = soc_pb_tcam_db_entry_id_to_location_entry_get(
          unit,
          tcam_db_id,
          entry_id_ndx,
          &location,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
  {
    SOC_PB_PMF_TCAM_ENTRY_ID_clear(&entry);
    entry.db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
    entry.entry_id = entry_id_ndx;
    res = soc_pb_pmf_tcam_entry_remove_unsafe(
            unit,
            &entry,
            is_found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else
  {
    res = soc_pb_pp_fp_egr_db_id_manage_unsafe(
            unit,
            db_id_ndx,
            SOC_PB_PP_FP_EGR_MANAGE_TYPE_GET,
            &hw_egr_db,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

    res = soc_pb_egr_acl_entry_remove_unsafe(
            unit,
            hw_egr_db,
            entry_id_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_tcam_entry_remove_unsafe()", db_id_ndx, entry_id_ndx);
}

uint32
  soc_pb_pp_fp_tcam_entry_remove_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_EGR_DB_ID_NDX_MAX, SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id_ndx, SOC_PB_PP_FP_EGR_ENTRY_ID_NDX_MAX, SOC_PB_PP_FP_EGR_ENTRY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_tcam_entry_remove_verify()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Manage the mapping between the Database-ID and the
 *     Hardware egress ACL id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_id_manage_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_EGR_MANAGE_TYPE   manage_type,
    SOC_SAND_OUT uint32                 *hw_egr_db,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    sw_db_curr,
    res = SOC_SAND_OK;
  uint32
    hw_db_ndx;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DB_ID_MANAGE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(hw_egr_db);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   * Verify for Egress
   */
  res = soc_pb_pp_fp_egr_db_id_manage_verify(
          unit,
          db_id_ndx,
          manage_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *success = SOC_SAND_SUCCESS;
  *hw_egr_db = SOC_PB_PP_FP_NOF_EGR_ACL_DBS;

  switch (manage_type)
  {
  case SOC_PB_PP_FP_EGR_MANAGE_TYPE_ADD:
    /*
     * Find a new HW profile index and write in it
     */
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    is_found = FALSE;
    for (hw_db_ndx = 1; (hw_db_ndx < SOC_PB_PP_FP_NOF_EGR_ACL_DBS) && (is_found == FALSE); hw_db_ndx++)
    {
      sw_db_curr = soc_pb_sw_db_egr_acl_sw_db_id_get(unit, hw_db_ndx);
      if (sw_db_curr == SOC_PB_PP_FP_NOF_DBS)
      {
        /*
         * Take this profile
         */
        is_found = TRUE;
        *success = SOC_SAND_SUCCESS;
        *hw_egr_db = hw_db_ndx;
        
        soc_pb_sw_db_egr_acl_sw_db_id_set(unit, hw_db_ndx, db_id_ndx);
      }
    }
    break;

  case SOC_PB_PP_FP_EGR_MANAGE_TYPE_RMV:
  case SOC_PB_PP_FP_EGR_MANAGE_TYPE_GET:
    is_found = FALSE;
    for (hw_db_ndx = 1; (hw_db_ndx < SOC_PB_PP_FP_NOF_EGR_ACL_DBS) && (is_found == FALSE); hw_db_ndx++)
    {
      sw_db_curr = soc_pb_sw_db_egr_acl_sw_db_id_get(unit, hw_db_ndx);
      if (sw_db_curr == db_id_ndx)
      {
        /*
         * Take this profile
         */
        is_found = TRUE;
        *success = SOC_SAND_SUCCESS;
        *hw_egr_db = hw_db_ndx;
        
        /*
         * Remove this Profile in case of remove
         */
        if (manage_type == SOC_PB_PP_FP_EGR_MANAGE_TYPE_RMV)
        {
          soc_pb_sw_db_egr_acl_sw_db_id_set(unit, hw_db_ndx, SOC_PB_PP_FP_NOF_DBS);
        }
      }
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_EGR_MANAGE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_db_id_manage_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_egr_db_id_manage_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_EGR_MANAGE_TYPE   manage_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DB_ID_MANAGE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_EGR_DB_ID_NDX_MAX, SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(manage_type, SOC_PB_PP_FP_EGR_MANAGE_TYPE_MAX, SOC_PB_PP_FP_EGR_MANAGE_TYPE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_db_id_manage_verify()", db_id_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_fp_egr module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_fp_egr_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_fp_egr;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_fp_egr module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_fp_egr_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_fp_egr;
}
#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_FP_EGR_MANAGE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_EGR_MANAGE_TYPE  enum_val
  )
{
  return SOC_PPC_FP_EGR_MANAGE_TYPE_to_string(enum_val);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

