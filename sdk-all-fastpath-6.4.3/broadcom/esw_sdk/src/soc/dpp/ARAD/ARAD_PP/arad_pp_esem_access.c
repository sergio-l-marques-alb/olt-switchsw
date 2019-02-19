#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_esem_access.c,v 1.14 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/mem.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_framework.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_esem_access.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_sim_em.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define ARAD_PP_ESEM_FLD_INVALID                               (0xFFFFFFFF)

#define ARAD_PP_ESEM_ENTRY_REQ_TYPE_LSB                        (0)
#define ARAD_PP_ESEM_ENTRY_REQ_TYPE_MSB                        (2)
#define ARAD_PP_ESEM_ENTRY_REQ_TYPE_NOF_BITS                   SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_REQ_TYPE_LSB, ARAD_PP_ESEM_ENTRY_REQ_TYPE_MSB)

#define ARAD_PP_ESEM_ENTRY_REQ_STAMP_LSB                       (3)
#define ARAD_PP_ESEM_ENTRY_REQ_STAMP_MSB                       (10)
#define ARAD_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS                  SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_REQ_STAMP_LSB, ARAD_PP_ESEM_ENTRY_REQ_STAMP_MSB)

#define ARAD_PP_ESEM_ENTRY_REQ_STAMP_VAL                       (0xAE)

#define ARAD_PP_ESEM_ENTRY_REQ_KEY_LSB                         (11)
#define ARAD_PP_ESEM_ENTRY_REQ_KEY_MSB(unit)                   (SOC_IS_JERICHO(unit) ? 50:47)
#define ARAD_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS(unit)              SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_REQ_KEY_LSB, ARAD_PP_ESEM_ENTRY_REQ_KEY_MSB(unit))

#define ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB(unit)               (SOC_IS_JERICHO(unit) ? 51:48)
#define ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_MSB(unit)               (SOC_IS_JERICHO(unit) ? 67:63)
#define ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_NOF_BITS(unit)          SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB(unit), ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_MSB(unit))

#define ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS                       (3)

#define ARAD_PP_ESEM_KEY_CVID_LSB                              (0)
#define ARAD_PP_ESEM_KEY_CVID_MSB                              (11)
#define ARAD_PP_ESEM_KEY_CVID_NOF_BITS                         SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_CVID_LSB, ARAD_PP_ESEM_KEY_CVID_MSB)

#define ARAD_PP_ESEM_KEY_VSI_LSB                               (12)
#define ARAD_PP_ESEM_KEY_VSI_MSB                               (27)
#define ARAD_PP_ESEM_KEY_VSI_NOF_BITS                          SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_VSI_LSB, ARAD_PP_ESEM_KEY_VSI_MSB)

#define ARAD_PP_ESEM_KEY_VD_LSB                                (28)
#define ARAD_PP_ESEM_KEY_VD_MSB(unit)                          (SOC_IS_JERICHO(unit) ? 36:35)
#define ARAD_PP_ESEM_KEY_VD_NOF_BITS(unit)                     SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_VD_LSB, ARAD_PP_ESEM_KEY_VD_MSB(unit))

#define ARAD_PP_ESEM_KEY_VSI_DB_ID_LSB(unit)                   (SOC_IS_JERICHO(unit) ? 37:36)
#define ARAD_PP_ESEM_KEY_VSI_DB_ID_MSB(unit)                   (SOC_IS_JERICHO(unit) ? 37:36)
#define ARAD_PP_ESEM_KEY_VSI_DB_ID_NOF_BITS(unit)              SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_VSI_DB_ID_LSB(unit), ARAD_PP_ESEM_KEY_VSI_DB_ID_MSB(unit))
#define ARAD_PP_ESEM_KEY_VSI_DB_ID_VAL                         (1)

#define ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB                (0)
#define ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_MSB(unit)          (SOC_IS_JERICHO(unit) ? 17:15)
#define ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS(unit)     SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_MSB(unit))


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
  ARAD_PP_ESEM_ACTION_TYPE_DELETE = 0,
  ARAD_PP_ESEM_ACTION_TYPE_INSERT,
  ARAD_PP_ESEM_ACTION_TYPE_DEFRAG,
  ARAD_PP_ESEM_NOF_ACTION_TYPES
}ARAD_PP_ESEM_ACTION_TYPE;

typedef enum
{
  /*
   *    Not actual failure,
   *  e.g.
   *  Inserted existing,
   *  Learn over existing
   */
  ARAD_PP_ESEM_FAIL_REASON_NO_FAIL = 0,
  /*
   *    Cam table full, Table coherency
   */
  ARAD_PP_ESEM_FAIL_REASON_INTERNAL_ERR,
  /*
   *    Requested Delete operation on non-existing entry,
   *  Delete unknown key
   */
  ARAD_PP_ESEM_FAIL_REASON_DELETE_NON_EXIST,
  /*
   *    These errors should not happen on EM-DB,
   *  e.g. Change-fail non exist,
   *  Change non-exist from other,
   *  Change non-exist from self
   *  Learn request over static,
   *  Change request over static
   */
  ARAD_PP_ESEM_FAIL_REASON_INTERN_NON_EM_ERR,
  /*
   *    Reached max entry limit
   */
  ARAD_PP_ESEM_FAIL_REASON_OUT_OF_SPACE,
  ARAD_PP_NOF_ESEM_FAIL_REASONS
}ARAD_PP_ESEM_FAIL_REASON;


CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_esem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_TBL_WRITE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_esem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    ARAD_PP_ESEM_NOT_READY_ERR,
    "ARAD_PP_ESEM_NOT_READY_ERR",
    "Sem is not ready. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_ESEM_KEY_TYPE_INVALID_ERR,
    "ARAD_PP_ESEM_KEY_TYPE_INVALID_ERR",
    "ESEM Key type out-of-range \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR,
    "ARAD_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR",
    "ESEM operation reported an error,\n\r "
    "but the reported key is different from the one\n\r"
    "for which the ESEM was accessed\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_ESEM_NOT_READ_TIMEOUT_ERR,
    "ARAD_PP_ESEM_NOT_READ_TIMEOUT_ERR",
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
    arad_pp_esem_key2intern(
      SOC_SAND_IN ARAD_PP_ESEM_ACTION_TYPE action_type
    )
{
  uint32
    action_type_intern;

  switch(action_type) {
  case ARAD_PP_ESEM_ACTION_TYPE_DELETE:
    action_type_intern = 0x0;
    break;
  case ARAD_PP_ESEM_ACTION_TYPE_INSERT:
    action_type_intern = 0x1;
    break;
  case ARAD_PP_ESEM_ACTION_TYPE_DEFRAG:
    action_type_intern = 0x4;
    break;
  default:
    action_type_intern = ARAD_PP_ESEM_FLD_INVALID;
  }

  return action_type_intern;
}

uint32
  arad_pp_esem_key_to_buffer(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_ESEM_KEY      *esem_key,
    SOC_SAND_OUT uint32                *buffer
  )
{
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_KEY_TO_BUFFER);
  
  ARAD_PP_CLEAR(buffer, uint32, ARAD_PP_ESEM_KEY_SIZE);

  switch(esem_key->key_type)
  {
    case ARAD_PP_ESEM_KEY_TYPE_VD_VSI:
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_vsi.vd, ARAD_PP_ESEM_KEY_VD_LSB, ARAD_PP_ESEM_KEY_VD_NOF_BITS(unit), buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_vsi.vsi, ARAD_PP_ESEM_KEY_VSI_LSB, ARAD_PP_ESEM_KEY_VSI_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
      tmp = ARAD_PP_ESEM_KEY_VSI_DB_ID_VAL;
      res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_ESEM_KEY_VSI_DB_ID_LSB(unit), ARAD_PP_ESEM_KEY_VSI_DB_ID_NOF_BITS(unit), buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
      break;
    case ARAD_PP_ESEM_KEY_TYPE_VD_VSI_CVID:
      /* We are not going to change the macros becuase of such cases */
      /* coverity[result_independent_of_operands] */
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_cvid.vd, ARAD_PP_ESEM_KEY_VD_LSB, ARAD_PP_ESEM_KEY_VD_NOF_BITS(unit), buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_vsi.vsi, ARAD_PP_ESEM_KEY_VSI_LSB, ARAD_PP_ESEM_KEY_VSI_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_cvid.cvid, ARAD_PP_ESEM_KEY_CVID_LSB, ARAD_PP_ESEM_KEY_CVID_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_ESEM_KEY_TYPE_INVALID_ERR, 20, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_key_to_buffer()", 0, 0);
}

uint32
  arad_pp_esem_entry_to_buffer(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_PP_ESEM_ENTRY     *esem_entry,
    SOC_SAND_OUT uint32                 *buffer
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_TO_BUFFER);
  
  ARAD_PP_CLEAR(buffer, uint32, ARAD_PP_ESEM_ENTRY_SIZE);

  /* We are not going to change the macros becuase of such cases */
  /* coverity[result_independent_of_operands] */
  res = soc_sand_bitstream_set_any_field(&esem_entry->out_ac, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS(unit), buffer);
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_to_buffer()", 0, 0);
}

uint32
  arad_pp_esem_entry_from_buffer(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32               *buffer,
    SOC_SAND_OUT ARAD_PP_ESEM_ENTRY   *esem_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_FROM_BUFFER);
  
  ARAD_PP_CLEAR(esem_entry, ARAD_PP_ESEM_ENTRY, 1);

  /* We are not going to change the macros becuase of such cases */
  /* coverity[result_independent_of_operands] */
  res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS(unit), &esem_entry->out_ac);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_from_buffer()", 0, 0);
}

uint32
  arad_pp_esem_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_OUT ARAD_PP_ESEM_ENTRY                          *esem_entry,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    reg_val,
    found_fld_val,
    payload_fld_val,
    lookup_trigger_val,
    res = SOC_SAND_OK;       
  uint32
    key_buffer[ARAD_PP_ESEM_KEY_SIZE];
  uint32
    entry_buffer[ARAD_PP_ESEM_ENTRY_SIZE];
  uint64
    key_buffer_64,
    diag_key_reg;
  uint8
    lkup_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(esem_entry);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ARAD_PP_CLEAR(entry_buffer, uint32, ARAD_PP_ESEM_ENTRY_SIZE);
  
  res = arad_pp_esem_key_to_buffer(unit, esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Convert key buffer to 64b */
  COMPILER_64_SET(key_buffer_64,key_buffer[1],key_buffer[0]);
  COMPILER_64_ZERO(diag_key_reg);

  *found = FALSE;
  soc_reg64_field_set(unit, EDB_ESEM_DIAGNOSTICS_KEYr, &diag_key_reg, ESEM_DIAGNOSTICS_KEYf,key_buffer_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_EDB_ESEM_DIAGNOSTICS_KEYr(unit,diag_key_reg));

  lookup_trigger_val = 0x1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EDB_ESEM_DIAGNOSTICSr, REG_PORT_ANY, 0, ESEM_DIAGNOSTICS_LOOKUPf,  lookup_trigger_val));

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          EDB_ESEM_DIAGNOSTICSr,
          REG_PORT_ANY,
          0,
          ESEM_DIAGNOSTICS_LOOKUPf,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  38,  exit, ARAD_REG_ACCESS_ERR,READ_EDB_ESEM_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_val)); 
    
  ARAD_FLD_FROM_REG(EDB_ESEM_DIAGNOSTICS_LOOKUP_RESULTr, ESEM_ENTRY_FOUNDf, found_fld_val, reg_val, 40, exit);
  lkup_success = SOC_SAND_NUM2BOOL(found_fld_val);

  if (lkup_success)
  {
    ARAD_FLD_FROM_REG(EDB_ESEM_DIAGNOSTICS_LOOKUP_RESULTr, ESEM_ENTRY_PAYLOADf, payload_fld_val, reg_val, 42, exit);
    entry_buffer[0] = payload_fld_val;
  }
  else
  {
      if (SOC_DPP_IS_EM_SIM_ENABLE(unit))
      {
          res = chip_sim_exact_match_entry_get_unsafe(
                      unit,
                      ARAD_CHIP_SIM_ESEM_BASE,
                      key_buffer,
                      ARAD_PP_ESEM_KEY_SIZE * sizeof(uint32),
                      entry_buffer,
                      ARAD_PP_ESEM_ENTRY_SIZE * sizeof(uint32),
                      &lkup_success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      }
  }

  res = arad_pp_esem_entry_from_buffer(unit, entry_buffer, esem_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  
  *found = lkup_success;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_get_unsafe()", 0, 0);
}

STATIC
  void
    arad_pp_esem_intern2fail(
      SOC_SAND_IN   uint32 intern_fail,
      SOC_SAND_OUT ARAD_PP_ESEM_FAIL_REASON *esem_fail_status,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *soc_sand_success_status
    )
{
  ARAD_PP_ESEM_FAIL_REASON
    fail = ARAD_PP_ESEM_FAIL_REASON_NO_FAIL;
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
   *    ESEM Fail (relatively detailed)
   */
  switch(intern_fail) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      fail = ARAD_PP_ESEM_FAIL_REASON_INTERN_NON_EM_ERR;
      break;
    case 6:
      fail = ARAD_PP_ESEM_FAIL_REASON_NO_FAIL;
        break;
    case 7:
      fail = ARAD_PP_ESEM_FAIL_REASON_OUT_OF_SPACE;
        break;
    case 8:
      fail = ARAD_PP_ESEM_FAIL_REASON_DELETE_NON_EXIST;
        break;
    case 9:
    case 10:
      fail = ARAD_PP_ESEM_FAIL_REASON_INTERNAL_ERR;
      break;
    default:
      fail = ARAD_PP_ESEM_FAIL_REASON_NO_FAIL;
  }

  /*
   *    SOC_SAND Success/Fail (high-level indication)
   */
  switch(fail) {
      case ARAD_PP_ESEM_FAIL_REASON_INTERNAL_ERR:
      case ARAD_PP_ESEM_FAIL_REASON_INTERN_NON_EM_ERR:
        success_status = SOC_SAND_FAILURE_INTERNAL_ERR;
        break;
      case ARAD_PP_ESEM_FAIL_REASON_OUT_OF_SPACE:
        success_status = ARAD_PP_ESEM_FAIL_REASON_OUT_OF_SPACE;
        break;
      case ARAD_PP_ESEM_FAIL_REASON_NO_FAIL:
      case ARAD_PP_ESEM_FAIL_REASON_DELETE_NON_EXIST:
      default:
        success_status = ARAD_PP_ESEM_FAIL_REASON_NO_FAIL;
      }

  *esem_fail_status =  fail;
  *soc_sand_success_status = success_status;
}

uint32
  arad_pp_esem_tbl_write(
    SOC_SAND_IN    int             unit,
    SOC_SAND_IN    uint32              key_buffer[ARAD_PP_ESEM_KEY_SIZE],
    SOC_SAND_INOUT uint32              esem_buffer[ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS],
    SOC_SAND_OUT   SOC_SAND_SUCCESS_FAILURE  *success
  )
{
  uint32    
    fail_valid_val,
    fail_reason_val;
  uint64
    key_buffer_64,
    fail_key_val,
    failure;  
  ARAD_PP_ESEM_FAIL_REASON
    fail_reason;
  SOC_SAND_SUCCESS_FAILURE
    success_status;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_TBL_WRITE);

  SOC_SAND_CHECK_NULL_INPUT(success);
  SOC_SAND_CHECK_NULL_INPUT(esem_buffer);
   
  /*
   *    clear fail register before performing new command
   */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, READ_EDB_ESEM_MANAGEMENT_UNIT_FAILUREr(unit,&failure));
  
  /*
   *    Write to EsemManagementRequest
   */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, WRITE_EDB_ESEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ALL, 0, esem_buffer));

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          EDB_ESEM_INTERRUPT_REGISTER_ONEr,
          REG_PORT_ANY,
          0,
          ESEM_MANAGEMENT_COMPLETEDf,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_EDB_ESEM_MANAGEMENT_UNIT_FAILUREr(unit,&failure));
  fail_valid_val = soc_reg64_field32_get(unit, EDB_ESEM_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_MNGMNT_UNIT_FAILURE_VALIDf);
    
    if (fail_valid_val)
    {
      /*
       *    Failure indication is on.
       *  Check if it is on the last operation
       */       
      fail_key_val = soc_reg64_field_get(unit, EDB_ESEM_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_MNGMNT_UNIT_FAILURE_KEYf);
      
      /* Convert key buffer to 64b */
      COMPILER_64_SET(key_buffer_64,key_buffer[1],key_buffer[0]);

      if (COMPILER_64_NE(fail_key_val,key_buffer_64))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR, 50, exit);
      }

      fail_reason_val = soc_reg64_field32_get(unit, EDB_ESEM_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_MNGMNT_UNIT_FAILURE_REASONf);      
      arad_pp_esem_intern2fail(fail_reason_val, &fail_reason, &success_status);
      *success = success_status;
    }
    else
    {
      *success = SOC_SAND_SUCCESS;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_tbl_write()", 0, 0);
}

uint32
  arad_pp_esem_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_IN  ARAD_PP_ESEM_ENTRY                          *esem_entry,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    key_type,
    tmp,
    buffer[ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS];
  uint32
    key_buffer[ARAD_PP_ESEM_KEY_SIZE];
  uint32
    entry_buffer[ARAD_PP_ESEM_ENTRY_SIZE];       
  SOC_SAND_SUCCESS_FAILURE
    success_status;
  uint8
    is_success = TRUE;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(esem_entry);
  SOC_SAND_CHECK_NULL_INPUT(success); 
  
  success_status = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  ARAD_PP_CLEAR(buffer, uint32, ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS);

  res = arad_pp_esem_key_to_buffer(unit, esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = arad_pp_esem_entry_to_buffer(unit, esem_entry, entry_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  key_type = arad_pp_esem_key2intern(ARAD_PP_ESEM_ACTION_TYPE_INSERT);
  res = soc_sand_bitstream_set_any_field(
         &key_type,
         ARAD_PP_ESEM_ENTRY_REQ_TYPE_LSB,
         ARAD_PP_ESEM_ENTRY_REQ_TYPE_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  tmp = ARAD_PP_ESEM_ENTRY_REQ_STAMP_VAL;
  res = soc_sand_bitstream_set_any_field(
         &tmp,
         ARAD_PP_ESEM_ENTRY_REQ_STAMP_LSB,
         ARAD_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  /* We are not going to change the macros becuase of such cases */
  /* coverity[result_independent_of_operands] */
  res = soc_sand_bitstream_set_any_field(
          key_buffer,
          ARAD_PP_ESEM_ENTRY_REQ_KEY_LSB,
          ARAD_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS(unit),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = soc_sand_bitstream_set_any_field(
          entry_buffer,
          ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB(unit),
          ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_NOF_BITS(unit),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  res = arad_pp_esem_tbl_write(
          unit,
          key_buffer,
          buffer,
          &success_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

   if (SOC_DPP_IS_EM_SIM_ENABLE(unit))
   {
      res = chip_sim_exact_match_entry_add_unsafe(
                  unit,
                  ARAD_CHIP_SIM_ESEM_BASE,
                  key_buffer,
                  ARAD_PP_ESEM_KEY_SIZE * sizeof(uint32),
                  entry_buffer,
                  ARAD_PP_ESEM_ENTRY_SIZE * sizeof(uint32),
                  &is_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
   }
    
    success_status = SOC_SAND_BOOL2SUCCESS(is_success);

  *success = success_status;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_add_unsafe()", 0, 0);
}

uint32
  arad_pp_esem_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    key_type,
    tmp,
    buffer[ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS];
  uint32
    key_buffer[ARAD_PP_ESEM_KEY_SIZE];   
  SOC_SAND_SUCCESS_FAILURE
    success_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(success);  

  ARAD_PP_CLEAR(buffer, uint32, ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS);

  res = arad_pp_esem_key_to_buffer(unit, esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  key_type = arad_pp_esem_key2intern(ARAD_PP_ESEM_ACTION_TYPE_DELETE);

  res = soc_sand_bitstream_set_any_field(
          &key_type,
          ARAD_PP_ESEM_ENTRY_REQ_TYPE_LSB,
          ARAD_PP_ESEM_ENTRY_REQ_TYPE_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  tmp = ARAD_PP_ESEM_ENTRY_REQ_STAMP_VAL;
  res = soc_sand_bitstream_set_any_field(
         &tmp,
         ARAD_PP_ESEM_ENTRY_REQ_STAMP_LSB,
         ARAD_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  /* We are not going to change the macros becuase of such cases */
  /* coverity[result_independent_of_operands] */
  res = soc_sand_bitstream_set_any_field(
          key_buffer,
          ARAD_PP_ESEM_ENTRY_REQ_KEY_LSB,
          ARAD_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS(unit),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_esem_tbl_write(
          unit,
          key_buffer,
          buffer,
          &success_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (SOC_DPP_IS_EM_SIM_ENABLE(unit))
  {
        res = chip_sim_exact_match_entry_remove_unsafe(
                unit,
                ARAD_CHIP_SIM_ESEM_BASE,
                key_buffer,
                ARAD_PP_ESEM_KEY_SIZE * sizeof(uint32)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  }
    success_status = SOC_SAND_SUCCESS;
  
  *success = SOC_SAND_SUCCESS2BOOL(success_status);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_remove_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_esem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_esem_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_esem;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_esem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_esem_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_esem;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */


