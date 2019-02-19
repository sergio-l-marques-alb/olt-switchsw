/* $Id: pb_pp_esem_access.c,v 1.9 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_esem_access.c
*
* MODULE PREFIX:  soc_pb_pp
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
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_esem_access.h>


#ifdef USING_CHIP_SIM
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_ESEM_BUSY_WAIT_ITERATIONS   100
#define SOC_PB_PP_ESEM_TIMER_ITERATIONS       20
#define SOC_PB_PP_ESEM_TIMER_DELAY_MSEC       16

#define SOC_PB_PP_ESEM_FLD_INVALID                               (0xFFFFFFFF)

#define SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_LSB                        (0)
#define SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_MSB                        (2)
#define SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_NOF_BITS                   SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_LSB, SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_MSB)

#define SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_LSB                        (3)
#define SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_MSB                        (10)
#define SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS                   SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_LSB, SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_MSB)

#define SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_VAL                       (0xAE)

#define SOC_PB_PP_ESEM_ENTRY_REQ_KEY_LSB                         (11)
#define SOC_PB_PP_ESEM_ENTRY_REQ_KEY_MSB                         (31)
#define SOC_PB_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS                    SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_ENTRY_REQ_KEY_LSB, SOC_PB_PP_ESEM_ENTRY_REQ_KEY_MSB)

#define SOC_PB_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB                     (32)
#define SOC_PB_PP_ESEM_ENTRY_REQ_PAYLOAD_MSB                     (46)
#define SOC_PB_PP_ESEM_ENTRY_REQ_PAYLOAD_NOF_BITS                SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB, SOC_PB_PP_ESEM_ENTRY_REQ_PAYLOAD_MSB)

#define SOC_PB_PP_ESEM_ENTRY_REQ_SIZE_REGS                       (2)

#define SOC_PB_PP_ESEM_KEY_VD_LSB                                (14)
#define SOC_PB_PP_ESEM_KEY_VD_MSB                                (19)
#define SOC_PB_PP_ESEM_KEY_VD_NOF_BITS                           SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_KEY_VD_LSB, SOC_PB_PP_ESEM_KEY_VD_MSB)

#define SOC_PB_PP_ESEM_KEY_VSI_LSB                               (0)
#define SOC_PB_PP_ESEM_KEY_VSI_MSB                               (13)
#define SOC_PB_PP_ESEM_KEY_VSI_NOF_BITS                          SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_KEY_VSI_LSB, SOC_PB_PP_ESEM_KEY_VSI_MSB)

#define SOC_PB_PP_ESEM_KEY_VSI_DB_ID_LSB                               (20)
#define SOC_PB_PP_ESEM_KEY_VSI_DB_ID_MSB                               (20)
#define SOC_PB_PP_ESEM_KEY_VSI_DB_ID_NOF_BITS                          SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_KEY_VSI_DB_ID_LSB, SOC_PB_PP_ESEM_KEY_VSI_DB_ID_MSB)
#define SOC_PB_PP_ESEM_KEY_VSI_DB_ID_VAL                               (1)

#define SOC_PB_PP_ESEM_KEY_CVID_LSB                              (0)
#define SOC_PB_PP_ESEM_KEY_CVID_MSB                              (11)
#define SOC_PB_PP_ESEM_KEY_CVID_NOF_BITS                         SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_KEY_CVID_LSB, SOC_PB_PP_ESEM_KEY_CVID_MSB)

#define SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB                (1)
#define SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_MSB                (14)
#define SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS           SOC_SAND_RNG_COUNT(SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_MSB)

#define SOC_PB_PP_ESEM_KEY_SIZE                                  (1)
#define SOC_PB_PP_ESEM_ENTRY_SIZE                                (1)
        
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

typedef enum
{
  SOC_PB_PP_ESEM_ACTION_TYPE_DELETE = 0,
  SOC_PB_PP_ESEM_ACTION_TYPE_INSERT,
  SOC_PB_PP_ESEM_ACTION_TYPE_DEFRAG,
  SOC_PB_PP_ESEM_NOF_ACTION_TYPES
}SOC_PB_PP_ESEM_ACTION_TYPE;

typedef enum
{
  /*
   *	Not actual failure,
   *  e.g.
   *  Inserted existing,
   *  Learn over existing
   */
  SOC_PB_PP_ESEM_FAIL_REASON_NO_FAIL = 0,
  /*
   *	Cam table full, Table coherency
   */
  SOC_PB_PP_ESEM_FAIL_REASON_INTERNAL_ERR,
  /*
   *	Requested Delete operation on non-existing entry,
   *  Delete unknown key
   */
  SOC_PB_PP_ESEM_FAIL_REASON_DELETE_NON_EXIST,
  /*
   *	These errors should not happen on EM-DB,
   *  e.g. Change-fail non exist,
   *  Change non-exist from other,
   *  Change non-exist from self
   *  Learn request over static,
   *  Change request over static
   */
  SOC_PB_PP_ESEM_FAIL_REASON_INTERN_NON_EM_ERR,
  /*
   *	Reached max entry limit
   */
  SOC_PB_PP_ESEM_FAIL_REASON_OUT_OF_SPACE,
  SOC_PB_PP_NOF_ESEM_FAIL_REASONS
}SOC_PB_PP_ESEM_FAIL_REASON;


static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_esem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_ESEM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_ESEM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_ESEM_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_ESEM_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_ESEM_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_ESEM_TBL_WRITE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_esem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_ESEM_NOT_READY_ERR,
    "SOC_PB_PP_ESEM_NOT_READY_ERR",
    "Sem is not ready. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_ESEM_KEY_TYPE_INVALID_ERR,
    "SOC_PB_PP_ESEM_KEY_TYPE_INVALID_ERR",
    "ESEM Key type out-of-range \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR,
    "SOC_PB_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR",
    "ESEM operation reported an error,\n\r "
    "but the reported key is different from the one\n\r"
    "for which the ESEM was accessed\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_ESEM_NOT_READ_TIMEOUT_ERR,
    "SOC_PB_PP_ESEM_NOT_READ_TIMEOUT_ERR",
    "When reading ESEM-DB,  \n\r "
    "the look-up operation did not end",
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
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC
  uint32
    soc_pb_pp_esem_key2intern(
      SOC_SAND_IN SOC_PB_PP_ESEM_ACTION_TYPE action_type
    )
{
  uint32
    action_type_intern;

  switch(action_type) {
  case SOC_PB_PP_ESEM_ACTION_TYPE_DELETE:
    action_type_intern = 0x0;
  	break;
  case SOC_PB_PP_ESEM_ACTION_TYPE_INSERT:
    action_type_intern = 0x1;
  	break;
  case SOC_PB_PP_ESEM_ACTION_TYPE_DEFRAG:
    action_type_intern = 0x4;
  	break;
  default:
    action_type_intern = SOC_PB_PP_ESEM_FLD_INVALID;
  }

  return action_type_intern;
}

uint32
  soc_pb_pp_esem_key_to_buffer(
    SOC_SAND_IN  SOC_PB_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_OUT uint32                                  *buffer
  )
{
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PB_PP_CLEAR(buffer, uint32, SOC_PB_PP_ESEM_KEY_SIZE);

  switch(esem_key->key_type)
  {
    case SOC_PB_PP_ESEM_KEY_TYPE_VD_VSI:
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_vsi.vd, SOC_PB_PP_ESEM_KEY_VD_LSB, SOC_PB_PP_ESEM_KEY_VD_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_vsi.vsi, SOC_PB_PP_ESEM_KEY_VSI_LSB, SOC_PB_PP_ESEM_KEY_VSI_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
      tmp = SOC_PB_PP_ESEM_KEY_VSI_DB_ID_VAL;
      res = soc_sand_bitstream_set_any_field(&tmp, SOC_PB_PP_ESEM_KEY_VSI_DB_ID_LSB, SOC_PB_PP_ESEM_KEY_VSI_DB_ID_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
      break;
    case SOC_PB_PP_ESEM_KEY_TYPE_VD_CVID:
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_cvid.vd, SOC_PB_PP_ESEM_KEY_VD_LSB, SOC_PB_PP_ESEM_KEY_VD_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_cvid.cvid, SOC_PB_PP_ESEM_KEY_CVID_LSB, SOC_PB_PP_ESEM_KEY_CVID_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ESEM_KEY_TYPE_INVALID_ERR, 20, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_esem_key_to_buffer()", 0, 0);
}

uint32
  soc_pb_pp_esem_entry_to_buffer(
    SOC_SAND_IN  SOC_PB_PP_ESEM_ENTRY                          *esem_entry,
    SOC_SAND_OUT uint32                                  *buffer
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PB_PP_CLEAR(buffer, uint32, SOC_PB_PP_ESEM_ENTRY_SIZE);

  /* bit zero used for opcode*/
  res = soc_sand_bitstream_set_any_field(&esem_entry->out_ac, SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS, buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_esem_entry_to_buffer()", 0, 0);
}

uint32
  soc_pb_pp_esem_entry_from_buffer(
    SOC_SAND_IN  uint32                                  *buffer,
    SOC_SAND_OUT SOC_PB_PP_ESEM_ENTRY                           *esem_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PB_PP_CLEAR(esem_entry, SOC_PB_PP_ESEM_ENTRY, 1);

  res = soc_sand_bitstream_get_any_field(buffer, SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, SOC_PB_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS, &esem_entry->out_ac);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_esem_entry_from_buffer()", 0, 0);
}

uint32
  soc_pb_pp_esem_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_OUT SOC_PB_PP_ESEM_ENTRY                          *esem_entry,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    reg_val,
    found_fld_val,
    payload_fld_val,
    key_fld_val,
    lookup_trigger_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;
  uint32
    key_buffer[SOC_PB_PP_ESEM_KEY_SIZE];
  uint32
    entry_buffer[SOC_PB_PP_ESEM_ENTRY_SIZE];
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success,
    lkup_success;
  uint32
#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_ESEM_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(esem_entry);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_CLEAR(entry_buffer, uint32, SOC_PB_PP_ESEM_ENTRY_SIZE);

  regs = soc_pb_pp_regs();
  res = soc_pb_pp_esem_key_to_buffer(esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  key_fld_val = key_buffer[0];

  *found = FALSE;
  if (!is_low_sim_active)
  {
    SOC_PB_PP_FLD_SET(regs->epni.esem_diagnostics_key_reg.esem_diagnostics_key, key_fld_val, 10, exit);
    lookup_trigger_val = 0x1;
    SOC_PB_PP_FLD_SET(regs->epni.esem_diagnostics_reg.esem_diagnostics_lookup, lookup_trigger_val, 12, exit);

    SOC_PETRA_POLL_DEF(poll_info, 0x0, SOC_PB_PP_ESEM_BUSY_WAIT_ITERATIONS, SOC_PB_PP_ESEM_TIMER_ITERATIONS, SOC_PB_PP_ESEM_TIMER_DELAY_MSEC);
    
    res = soc_petra_status_fld_poll_unsafe(
            unit,
            &(regs->epni.esem_diagnostics_reg.esem_diagnostics_lookup),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (!poll_success)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ESEM_NOT_READ_TIMEOUT_ERR, 35, exit);
    }

    SOC_PB_PP_REG_GET(regs->epni.esem_diagnostics_lookup_result_reg, reg_val, 38, exit);
    SOC_PB_PP_FLD_FROM_REG(regs->epni.esem_diagnostics_lookup_result_reg.esem_entry_found, found_fld_val, reg_val, 40, exit);
    lkup_success = SOC_SAND_NUM2BOOL(found_fld_val);

    if (lkup_success)
    {
      SOC_PB_PP_FLD_FROM_REG(regs->epni.esem_diagnostics_lookup_result_reg.esem_entry_payload, payload_fld_val, reg_val, 42, exit);
      entry_buffer[0] = payload_fld_val;
    }
  }
  else
  {
#ifdef USING_CHIP_SIM
    res = chip_sim_exact_match_entry_get_unsafe(
            unit,
            SOC_PB_PP_ESEM_ADDR,
            key_buffer,
            SOC_PB_PP_ESEM_KEY_SIZE * sizeof(uint32),
            entry_buffer,
            SOC_PB_PP_ESEM_ENTRY_SIZE,
            &lkup_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
#else
    lkup_success = TRUE;
#endif
  }

  res = soc_pb_pp_esem_entry_from_buffer(entry_buffer, esem_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  *found = lkup_success;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_esem_entry_get_unsafe()", 0, 0);
}

STATIC
  void
    soc_pb_pp_esem_intern2fail(
      SOC_SAND_IN   uint32 intern_fail,
      SOC_SAND_OUT SOC_PB_PP_ESEM_FAIL_REASON *esem_fail_status,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *soc_sand_success_status
    )
{
  SOC_PB_PP_ESEM_FAIL_REASON
    fail = SOC_PB_PP_ESEM_FAIL_REASON_NO_FAIL;
  SOC_SAND_SUCCESS_FAILURE
    success_status;

  /*
   * Internal representation (EsemMngmntUnitFailureReason)
   * Specifies reason for EMC management
   * failure:
   * 10 - Cam table full
   * 09 - Table coherency
   * 08 - Delete unknown key
   * 07 - Reached max entry limit
   * 06 - Inserted existing
   * 05 - Learn request over static
   * 04 - Learn over existing
   * 03 - Change-fail non exist
   * 02 - Change request over static
   * 01 - Change non-exist from other
   * 00 - Change non-exist from self
   */

  /*
   *	ESEM Fail (relatively detailed)
   */
  switch(intern_fail) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      fail = SOC_PB_PP_ESEM_FAIL_REASON_INTERN_NON_EM_ERR;
      break;
    case 6:
      fail = SOC_PB_PP_ESEM_FAIL_REASON_NO_FAIL;
    	break;
    case 7:
      fail = SOC_PB_PP_ESEM_FAIL_REASON_OUT_OF_SPACE;
    	break;
    case 8:
      fail = SOC_PB_PP_ESEM_FAIL_REASON_DELETE_NON_EXIST;
    	break;
    case 9:
    case 10:
      fail = SOC_PB_PP_ESEM_FAIL_REASON_INTERNAL_ERR;
  	  break;
    default:
      fail = SOC_PB_PP_ESEM_FAIL_REASON_NO_FAIL;
  }

  /*
   *	SOC_SAND Success/Fail (high-level indication)
   */
  switch(fail) {
      case SOC_PB_PP_ESEM_FAIL_REASON_INTERNAL_ERR:
      case SOC_PB_PP_ESEM_FAIL_REASON_INTERN_NON_EM_ERR:
        success_status = SOC_SAND_FAILURE_INTERNAL_ERR;
      	break;
      case SOC_PB_PP_ESEM_FAIL_REASON_OUT_OF_SPACE:
        success_status = SOC_PB_PP_ESEM_FAIL_REASON_OUT_OF_SPACE;
      	break;
      case SOC_PB_PP_ESEM_FAIL_REASON_NO_FAIL:
      case SOC_PB_PP_ESEM_FAIL_REASON_DELETE_NON_EXIST:
      default:
        success_status = SOC_PB_PP_ESEM_FAIL_REASON_NO_FAIL;
      }

  *esem_fail_status =  fail;
  *soc_sand_success_status = success_status;
}

uint32
  soc_pb_pp_esem_tbl_write(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              key_buffer[SOC_PB_PP_ESEM_KEY_SIZE],
    SOC_SAND_IN  uint32              esem_buffer[SOC_PB_PP_ESEM_ENTRY_REQ_SIZE_REGS],
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
  uint32
    reg_val[2],
    fail_valid_val,
    fail_reason_val,
    fail_key_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PB_PP_ESEM_FAIL_REASON
    fail_reason;
  SOC_SAND_SUCCESS_FAILURE
    success_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_ESEM_TBL_WRITE);

  SOC_SAND_CHECK_NULL_INPUT(success);
  SOC_SAND_CHECK_NULL_INPUT(esem_buffer);
  
  regs = soc_pb_pp_regs();

  /*
   *	clear fail register before performing new command
   */
  SOC_PB_PP_REG_GET(regs->epni.esem_management_unit_failure_reg_0, reg_val[0], 40, exit);

  /*
   *	Write to EsemManagementRequest
   */
    res = soc_sand_tbl_write_unsafe(
            unit,
            esem_buffer,
            SOC_PB_PP_ESEM_ADDR,
            sizeof(uint32) * SOC_PB_PP_ESEM_ENTRY_REQ_SIZE_REGS,
            SOC_PB_EPNI_ID,
            SOC_PB_PP_ESEM_ENTRY_REQ_SIZE_REGS * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /*
     *	Read success/fail indications
     */
    SOC_PETRA_POLL_DEF(poll_info, 0x1, SOC_PB_PP_ESEM_BUSY_WAIT_ITERATIONS, SOC_PB_PP_ESEM_TIMER_ITERATIONS, SOC_PB_PP_ESEM_TIMER_DELAY_MSEC);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            &(regs->epni.esem_int_reg.esem_management_completed),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (!poll_success)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ESEM_NOT_READY_ERR, 37, exit);
    }
  
    SOC_PB_PP_REG_GET(regs->epni.esem_management_unit_failure_reg_0, reg_val[0], 40, exit);
    SOC_PB_PP_FLD_FROM_REG(regs->epni.esem_management_unit_failure_reg_0.esem_mngmnt_unit_failure_valid, fail_valid_val, reg_val[0], 42, exit);
    if (fail_valid_val)
    {
      /*
       *	Failure indication is on.
       *  Check if it is on the last operation
       */
      SOC_PB_PP_REG_GET(regs->epni.esem_management_unit_failure_reg_1, reg_val[1], 44, exit);

      res = soc_sand_bitstream_get_any_field(
              reg_val,
              regs->epni.esem_management_unit_failure_reg_0.esem_mngmnt_unit_failure_key.lsb,
              SOC_PB_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS,
              &fail_key_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

      /*if (fail_key_val != key_buffer[0])
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR, 50, exit);
      }*/

      SOC_PB_PP_FLD_FROM_REG(regs->epni.esem_management_unit_failure_reg_0.esem_mngmnt_unit_failure_reason, fail_reason_val, reg_val[0], 60, exit);
      soc_pb_pp_esem_intern2fail(fail_reason_val, &fail_reason, &success_status);
      *success = success_status;

    }
    else
    {
      *success = SOC_SAND_SUCCESS;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_esem_tbl_write()", 0, 0);
}

uint32
  soc_pb_pp_esem_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_IN  SOC_PB_PP_ESEM_ENTRY                          *esem_entry,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    key_type,
    tmp,
    buffer[SOC_PB_PP_ESEM_ENTRY_REQ_SIZE_REGS];
  uint32
    key_buffer[SOC_PB_PP_ESEM_KEY_SIZE];
  uint32
    entry_buffer[SOC_PB_PP_ESEM_ENTRY_SIZE];
  SOC_SAND_SUCCESS_FAILURE
    success_status;
  uint32
#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_ESEM_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(esem_entry);
  SOC_SAND_CHECK_NULL_INPUT(success);

  success_status = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  SOC_PB_PP_CLEAR(buffer, uint32, SOC_PB_PP_ESEM_ENTRY_REQ_SIZE_REGS);

  res = soc_pb_pp_esem_key_to_buffer(esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_pb_pp_esem_entry_to_buffer(esem_entry, entry_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  key_type = soc_pb_pp_esem_key2intern(SOC_PB_PP_ESEM_ACTION_TYPE_INSERT);
  res = soc_sand_bitstream_set_any_field(
         &key_type,
         SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_LSB,
         SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  tmp = SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_VAL;
  res = soc_sand_bitstream_set_any_field(
         &tmp,
         SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_LSB,
         SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_sand_bitstream_set_any_field(
          key_buffer,
          SOC_PB_PP_ESEM_ENTRY_REQ_KEY_LSB,
          SOC_PB_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = soc_sand_bitstream_set_any_field(
          entry_buffer,
          SOC_PB_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB,
          SOC_PB_PP_ESEM_ENTRY_REQ_PAYLOAD_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  if (!is_low_sim_active)
  {
    res = soc_pb_pp_esem_tbl_write(
            unit,
            key_buffer,
            buffer,
            &success_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    uint8
      is_success = TRUE;
#ifdef USING_CHIP_SIM
    res = chip_sim_exact_match_entry_add_unsafe(
            unit,
            SOC_PB_PP_ESEM_ADDR,
            key_buffer,
            SOC_PB_PP_ESEM_KEY_SIZE * sizeof(uint32),
            entry_buffer,
            SOC_PB_PP_ESEM_ENTRY_SIZE * sizeof(uint32),
            &is_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
#endif
    success_status = SOC_SAND_BOOL2SUCCESS(is_success);
  }

  *success = success_status;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_esem_entry_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_esem_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    key_type,
    tmp,
    buffer[SOC_PB_PP_ESEM_ENTRY_REQ_SIZE_REGS];
  uint32
    key_buffer[SOC_PB_PP_ESEM_KEY_SIZE];
  SOC_SAND_SUCCESS_FAILURE
    success_status;
  uint32
#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_ESEM_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_CLEAR(buffer, uint32, SOC_PB_PP_ESEM_ENTRY_REQ_SIZE_REGS);

  res = soc_pb_pp_esem_key_to_buffer(esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  key_type = soc_pb_pp_esem_key2intern(SOC_PB_PP_ESEM_ACTION_TYPE_DELETE);

  res = soc_sand_bitstream_set_any_field(
          &key_type,
          SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_LSB,
          SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_MSB - SOC_PB_PP_ESEM_ENTRY_REQ_TYPE_LSB + 1,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  tmp = SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_VAL;
  res = soc_sand_bitstream_set_any_field(
         &tmp,
         SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_LSB,
         SOC_PB_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  res = soc_sand_bitstream_set_any_field(
          key_buffer,
          SOC_PB_PP_ESEM_ENTRY_REQ_KEY_LSB,
          SOC_PB_PP_ESEM_ENTRY_REQ_KEY_MSB - SOC_PB_PP_ESEM_ENTRY_REQ_KEY_LSB + 1,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!is_low_sim_active)
  {
    res = soc_pb_pp_esem_tbl_write(
            unit,
            key_buffer,
            buffer,
            &success_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
#ifdef USING_CHIP_SIM
    res = chip_sim_exact_match_entry_remove_unsafe(
            unit,
            SOC_PB_PP_ESEM_ADDR,
            key_buffer,
            SOC_PB_PP_ESEM_KEY_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
#endif
    success_status = SOC_SAND_SUCCESS;
  }

  *success = SOC_SAND_SUCCESS2BOOL(success_status);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_esem_entry_remove_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_esem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_esem_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_esem;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_esem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_esem_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_esem;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

