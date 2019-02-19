
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_mgmt.c,v 1.9 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_framework.h>

#include <soc/dpp/SAND/Management/sand_device_management.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_MGMT_ELK_MODE_MAX                                (ARAD_PP_NOF_MGMT_ELK_MODES-1)
#define ARAD_PP_MGMT_LKP_TYPE_MAX                                (ARAD_PP_NOF_MGMT_LKP_TYPES-1)
#define ARAD_PP_MGMT_USE_ELK_MAX                                 (ARAD_PP_NOF_MGMT_LKP_TYPES-1)

/* } */

/*************
 *  MACROS   *
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

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_DEVICE_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_DEVICE_CLOSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_PROC_ERR_MECH_INIT),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'elk_mode' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MGMT_ELK_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MGMT_INGRESS_PKT_RATE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MGMT_INGRESS_PKT_RATE_OUT_OF_RANGE_ERR",
    "The parameter 'ingress_pkt_rate' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MGMT_ELK_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'lkp_type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MGMT_LKP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR,
    "ARAD_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR",
    "The parameter 'use_elk' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MGMT_LKP_TYPES-1.\n\r ",
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
  arad_pp_mgmt_proc_err_mech_init(void)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_PP_MGMT_PROC_ERR_MECH_INIT);

  /* Add list of ARAD_PP errors to all-system errors pool                                                  */
  res = arad_pp_errors_desc_add();
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  /* Now add list of ARAD_PP procedure descriptors to all-system pool.                                     */
  res = arad_pp_procedure_desc_add() ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_proc_err_mech_init()",0,0);
}


STATIC uint32
  arad_pp_mgmt_device_init(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_DEVICE_INIT);

  res = arad_pp_sw_db_device_init(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_device_init()",0,0);
}

uint32
  arad_pp_mgmt_device_close_unsafe(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_DEVICE_CLOSE);

  res = arad_pp_sw_db_device_close(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_device_close_unsafe()",0,0);
}

/*********************************************************************
*     Set arad_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_OPERATION_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  /*
   *	Init the error mechanism
   */
  res = arad_pp_mgmt_proc_err_mech_init();
  SOC_SAND_CHECK_FUNC_RESULT(res,10, exit);

  /*
   *	Init the SW DB
   */
  res = arad_pp_sw_db_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * Init the SW DB and Save the operation mode in
  */
  res = arad_pp_mgmt_device_init(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_operation_mode_set_unsafe()",0,0);
}

/*********************************************************************
*     Set arad_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_operation_mode_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE      *op_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_OPERATION_MODE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_operation_mode_verify()",0,0);
}

/*********************************************************************
*     Set arad_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT ARAD_PP_MGMT_OPERATION_MODE     *op_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_OPERATION_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.get(unit,op_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_operation_mode_get_unsafe()",0,0);
}

/*********************************************************************
*     Set the ELK interface mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_elk_mode_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_ELK_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ingress_pkt_rate);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_elk_mode_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mgmt_elk_mode_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_ELK_MODE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(elk_mode, ARAD_PP_MGMT_ELK_MODE_MAX, ARAD_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_elk_mode_set_verify()", 0, 0);
}

uint32
  arad_pp_mgmt_elk_mode_get_verify(
    SOC_SAND_IN  int           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_ELK_MODE_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_elk_mode_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the ELK interface mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_elk_mode_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_ELK_MODE *elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_ELK_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(elk_mode);
  SOC_SAND_CHECK_NULL_INPUT(ingress_pkt_rate);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_elk_mode_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Determine whether the specified lookup is externalized
 *     or not.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_use_elk_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_USE_ELK_SET_UNSAFE);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_use_elk_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mgmt_use_elk_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_USE_ELK_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lkp_type, ARAD_PP_MGMT_LKP_TYPE_MAX, ARAD_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(use_elk, ARAD_PP_MGMT_USE_ELK_MAX, ARAD_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_use_elk_set_verify()", 0, 0);
}

uint32
  arad_pp_mgmt_use_elk_get_verify(
    SOC_SAND_IN  int           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_USE_ELK_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_use_elk_get_verify()", 0, 0);
}

/*********************************************************************
*     Determine whether the specified lookup is externalized
 *     or not.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mgmt_use_elk_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_LKP_TYPE *lkp_type,
    SOC_SAND_OUT uint8           *use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_USE_ELK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_type);
  SOC_SAND_CHECK_NULL_INPUT(use_elk);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_use_elk_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_mgmt_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_mgmt;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_mgmt_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_mgmt;
}

#if ARAD_PP_DEBUG_IS_LVL1
#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

