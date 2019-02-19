/* $Id: pb_pp_svem_access.c,v 1.10 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP_/src/soc_pb_pp_svem_access.c
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
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_svem_access.h>
#ifdef USING_CHIP_SIM
#include <sim/dpp/ChipSim/chip_sim_em.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_LSB                       (0)
#define SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_MSB                       (2)
#define SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_NOF_BITS                  (SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_MSB - SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_LSB + 1)

#define SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_LSB                        (11)
#define SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_MSB                        (26)
#define SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_NOF_BITS                   (SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_MSB - SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_LSB + 1)

#define SOC_PB_PP_SVEM_ACCESS_ENTRY_PAYLOAD_LSB                    (27)
#define SOC_PB_PP_SVEM_ACCESS_ENTRY_PAYLOAD_MSB                    (38)
#define SOC_PB_PP_SVEM_ACCESS_ENTRY_PAYLOAD_NOF_BITS               (SOC_PB_PP_SVEM_ACCESS_ENTRY_PAYLOAD_MSB - SOC_PB_PP_SVEM_ACCESS_ENTRY_PAYLOAD_LSB + 1)

#define SOC_PB_PP_SVEM_BUSY_WAIT_ITERATIONS   100
#define SOC_PB_PP_SVEM_TIMER_ITERATIONS       20
#define SOC_PB_PP_SVEM_TIMER_DELAY_MSEC       16

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
  SOC_PB_PP_SVEM_ACCESS_ACTION_TYPE_DELETE = 0,
  SOC_PB_PP_SVEM_ACCESS_ACTION_TYPE_INSERT = 1,
  SOC_PB_PP_SVEM_ACCESS_ACTION_TYPE_DEFRAG = 4
}SOC_PB_PP_SVEM_ACCESS_ACTION_TYPE;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_svem_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SVEM_ACCESS_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SVEM_ACCESS_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SVEM_ACCESS_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SVEM_ACCESS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_SVEM_ACCESS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_svem_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_SVEM_ACCESS_NOT_READY_ERR,
    "SOC_PB_PP_SVEM_ACCESS_NOT_READY_ERR",
    "Svem is not ready. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_SVEM_ACCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_SVEM_ACCESS_OUT_OF_RANGE_ERR",
    "'vsi' is out of range. \n\r "
    "The range is: 1 to 16K-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_SVEM_NOT_READ_TIMEOUT_ERR,
    "SOC_PB_PP_SVEM_NOT_READ_TIMEOUT_ERR",
    "When reading VSEM-DB,  \n\r "
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
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_svem_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_svem_access_init_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_svem_access_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_SVEM_ACCESS_KEY                     *svem_key,
    SOC_SAND_IN  uint32                                  *svem_payload,
    SOC_SAND_IN  uint32                                  svem_payload_size_bytes,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    temp,
    buffer[SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE];
  SOC_SAND_SUCCESS_FAILURE
    success_status;
  uint32
#ifndef SAND_LOW_LEVEL_SIMULATION
    is_low_sim_active = FALSE;
#else
    is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE);

  SOC_SAND_CHECK_NULL_INPUT(svem_payload);
  SOC_SAND_CHECK_NULL_INPUT(svem_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_CLEAR(buffer, uint32, SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE);

  success_status = SOC_SAND_FAILURE_OUT_OF_RESOURCES;

  temp = SOC_PB_PP_SVEM_ACCESS_ACTION_TYPE_INSERT;
  res = soc_sand_bitstream_set_any_field(
          &temp,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_LSB,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_set_any_field(
          svem_key->key,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_LSB,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_set_any_field(
          svem_payload,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_PAYLOAD_LSB,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_PAYLOAD_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!is_low_sim_active)
  {
    res = soc_sand_tbl_write_unsafe(
            unit,
            buffer,
            SOC_PB_PP_SVEM_ADDR,
            sizeof(buffer),
            SOC_PB_PP_EGQ_ID,
            SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
   uint8
      is_success = TRUE;
#ifdef USING_CHIP_SIM
    res = chip_sim_exact_match_entry_add_unsafe(
            unit,
            SOC_PB_PP_SVEM_ADDR,
            svem_key->key,
            sizeof(svem_key->key),
            svem_payload,
            svem_payload_size_bytes,
            &is_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
#endif
    success_status = SOC_SAND_BOOL2SUCCESS(is_success);
  }

    *success = success_status;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_svem_access_entry_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_svem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_SVEM_ACCESS_KEY                     *svem_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    temp,
    buffer[SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE];
  uint32
#ifndef SAND_LOW_LEVEL_SIMULATION
    is_low_sim_active = FALSE;
#else
    is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_SVEM_ACCESS_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(svem_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  temp = SOC_PB_PP_SVEM_ACCESS_ACTION_TYPE_DELETE;
  res = soc_sand_bitstream_set_any_field(
          &temp,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_LSB,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_TYPE_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_set_any_field(
          svem_key->key,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_LSB,
          SOC_PB_PP_SVEM_ACCESS_ENTRY_KEY_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!is_low_sim_active)
  {

    res = soc_sand_tbl_write_unsafe(
            unit,
            buffer,
            SOC_PB_PP_SVEM_ADDR,
            sizeof(buffer),
            SOC_PB_PP_EGQ_ID,
            SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
#ifdef USING_CHIP_SIM
    res = chip_sim_exact_match_entry_remove_unsafe(
            unit,
            SOC_PB_PP_SVEM_ADDR,
            svem_key->key,
            sizeof(svem_key->key)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
#endif
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_svem_access_entry_remove_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_svem_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_SVEM_ACCESS_KEY                     *svem_key,
    SOC_SAND_OUT uint32                                  *svem_entry,
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
  SOC_SAND_IN uint32
    *key_buffer;
  uint32
    entry_buffer[SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE];
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_SVEM_ACCESS_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(svem_key);
  SOC_SAND_CHECK_NULL_INPUT(svem_entry);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_CLEAR(entry_buffer, uint32, SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE);

  regs = soc_pb_pp_regs();
  key_buffer = svem_key->key;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  key_fld_val = key_buffer[0];

  *found = FALSE;
  if (!is_low_sim_active)
  {
    SOC_PB_PP_FLD_SET(regs->egq.svem_diagnostics_key_reg.svem_diagnostics_key, key_fld_val, 10, exit);
    lookup_trigger_val = 0x1;
    SOC_PB_PP_FLD_SET(regs->egq.svem_diagnostics_reg.svem_diagnostics_lookup, lookup_trigger_val, 12, exit);

    SOC_PETRA_POLL_DEF(poll_info, 0x0, SOC_PB_PP_SVEM_BUSY_WAIT_ITERATIONS, SOC_PB_PP_SVEM_TIMER_ITERATIONS, SOC_PB_PP_SVEM_TIMER_DELAY_MSEC);
    
    res = soc_petra_status_fld_poll_unsafe(
            unit,
            &(regs->egq.svem_diagnostics_reg.svem_diagnostics_lookup),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (!poll_success)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_SVEM_NOT_READ_TIMEOUT_ERR, 35, exit);
    }

    SOC_PB_PP_REG_GET(regs->egq.svem_diagnostics_lookup_result_reg, reg_val, 38, exit);
    SOC_PB_PP_FLD_FROM_REG(regs->egq.svem_diagnostics_lookup_result_reg.svem_entry_found, found_fld_val, reg_val, 40, exit);
    lkup_success = SOC_SAND_NUM2BOOL(found_fld_val);

    if (lkup_success)
    {
      SOC_PB_PP_FLD_FROM_REG(regs->egq.svem_diagnostics_lookup_result_reg.svem_entry_payload, payload_fld_val, reg_val, 42, exit);
      entry_buffer[0] = payload_fld_val;
    }
  }
  else
  {
#ifdef USING_CHIP_SIM
    res = chip_sim_exact_match_entry_get_unsafe(
            unit,
            SOC_PB_PP_SVEM_ADDR,
            key_buffer,
            SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE * sizeof(uint32),
            svem_entry,
            SOC_PB_PP_SVEM_ACCESS_ENTRY_SIZE,
            &lkup_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
#else
    lkup_success = TRUE;
#endif
  }

  *found = lkup_success;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_svem_entry_get_unsafe()", 0, 0);
}



/*********************************************************************
*     Get the pointer to the list of procedures of the
*     soc_pb_pp_api_svem_access module.
*     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_svem_access_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_svem_access;
}

/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_svem_access module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_svem_access_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_svem_access;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

