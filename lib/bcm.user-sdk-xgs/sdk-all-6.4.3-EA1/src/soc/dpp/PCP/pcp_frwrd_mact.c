/* $Id: pcp_frwrd_mact.c,v 1.10 Broadcom SDK $
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

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_frwrd_mact.h>
#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_api_tbl_access.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_reg_access.h>

#include <soc/dpp/PCP/pcp_lem_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_FRWRD_MACT_ADD_TYPE_MAX                          (PCP_NOF_FRWRD_MACT_ADD_TYPES-1)
#define PCP_FRWRD_MACT_KEY_TYPE_MAX                          (PCP_NOF_FRWRD_MACT_KEY_TYPES-1)
#define PCP_FRWRD_MACT_AGE_STATUS_MIN                        (1)
#define PCP_FRWRD_MACT_AGE_STATUS_MAX                        (6)
#define PCP_FRWRD_MACT_SEC_MAX                               (SOC_SAND_U32_MAX)
#define PCP_FRWRD_MACT_MILI_SEC_MAX                          (999999)
#define PCP_FRWRD_MACT_COMPARE_MASK_MAX                      (SOC_SAND_U32_MAX)
#define PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_MAX              (PCP_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES-1)
#define PCP_FRWRD_MACT_NOF_MATCHED_ENTRIES_MAX               (SOC_SAND_U32_MAX)

#define PCP_FRWRD_MACT_IPV4_MC_DIP_BASE_VAL                  (0xE0000000)

#define PCP_FRWRD_MACT_GET_BLOCK_MAX_ENTRIES_PER_ITER        (PCP_LEM_NOF_ENTRIES)

#define PCP_FRWRD_MACT_IPV4_DIP_LSB                           (28)
#define PCP_FRWRD_MACT_IPV4_DIP_MSB                           (31)
#define PCP_FRWRD_MACT_IPV4_DIP_VALUE                         (0xE)

#define PCP_FRWRD_MACT_NOF_BITS_IN_MAC_ADDR                   (8)
#define PCP_FRWRD_MACT_NOF_MAC_ADDR_IN_UINT32                 (4)


#define PCP_FRWRD_MACT_MASK_DEFAULT_NULL                  (0xFFFFFFFF)

#define PCP_FRWRD_MACT_KEY_15_MSBS_LSB                    (16)
#define PCP_FRWRD_MACT_KEY_15_MSBS_MSB                    (30)

#define PCP_FRWRD_MACT_TABLE_TRAVERSE_NOF_LOOPS           (10)



#define PCP_FRWRD_MACT_TRAVERSE_STATUS_GET_TIME_REMANING_MS_MULTI_CONST 4

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

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Pcp_procedure_desc_element_frwrd_mact[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_TRAVERSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_TRAVERSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_TRAVERSE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_TRAVERSE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_TRAVERSE_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_TRAVERSE_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_TRAVERSE_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_TRAVERSE_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_PRINT_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_PRINT_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_PRINT_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_PRINT_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_KEY_CONVERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_PAYLOAD_MASK_BUILD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_KEY_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_RULE_FROM_KEY_BUILD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_PAYLOAD_CONVERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_RULE_TO_KEY_CONVERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_PAYLOAD_BUILD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_MACT_PAYLOAD_MASK_RETRIEVE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Pcp_error_desc_element_frwrd_mact[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'add_type' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_MACT_ADD_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_SUCCESS_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_FLAVORS_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_FLAVORS_OUT_OF_RANGE_ERR",
    "The parameter 'flavors' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_DIP_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_DIP_OUT_OF_RANGE_ERR",
    "The parameter 'dip' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'key_type' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_MACT_KEY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_AGE_STATUS_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_AGE_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'age_status' is out of range. \n\r "
    "The range is: 0 - 6.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_SEC_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_SEC_OUT_OF_RANGE_ERR",
    "The parameter 'sec' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_MILI_SEC_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_MILI_SEC_OUT_OF_RANGE_ERR",
    "The parameter 'mili_sec' is out of range. \n\r "
    "The range is: 0 - 999.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_DIP_MASK_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_DIP_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'dip_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_COMPARE_MASK_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_COMPARE_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'compare_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_UPDATE_MASK_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_UPDATE_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'update_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_NOF_MATCHED_ENTRIES_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_MACT_NOF_MATCHED_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_matched_entries' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    PCP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
    "PCP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_act' is out of range. \n\r "
    "The range is: 0 - 100.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_BLOCK_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
    "PCP_BLOCK_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_scan' is out of range. \n\r "
    "The range is: 0 - 64K+32.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR,
    "PCP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR",
    "The parameter 'iter' is out of range. \n\r "
    "The range is: 0 - 64K+32-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_ACTION_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_ACTION_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MACT_RETRIEVE_TYPE_OUT_OF_RANGE_ERR,
    "PCP_MACT_RETRIEVE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' has a forbidden value. \n\r "
    "PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE is not supported in PCP.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR,
    "PCP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR",
    "The MACT traverse API has failed due to \n\r "
    "an excessive waiting time to finish as desired.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MACT_FWD_DECSION_DROP_ERR,
    "PCP_MACT_FWD_DECSION_DROP_ERR",
    "The MACT forwarding decision cannot be drop. \n\r "
    "use trap-code instead.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
   {
    PCP_FID_OUT_OF_RANGE_ERR,
    "PCP_FID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_FID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
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
  pcp_frwrd_mact_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_DO_NOTHING_AND_EXIT;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_init_unsafe()", 0, 0);
}


STATIC uint32
  pcp_frwrd_mact_payload_build(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_VALUE     *mac_entry_value,
      SOC_SAND_OUT PCP_LEM_ACCESS_PAYLOAD         *payload
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_PAYLOAD_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(payload);

  /*
   *	Convert the payload
   */
  payload->age = mac_entry_value->aging_info.age_status;
  payload->is_dynamic = mac_entry_value->aging_info.is_dynamic;

  /*
   *	Get the encoded destination and ASD
   */
  res = pcp_fwd_decision_in_buffer_build(
          PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
          &(mac_entry_value->frwrd_info.forward_decision),
          mac_entry_value->frwrd_info.drop_when_sa_is_known,
          &(payload->dest),
          &(payload->asd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_payload_build()", 0, 0);
}


STATIC uint32
  pcp_frwrd_mact_payload_mask_build(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_VALUE  *entry_val,
      SOC_SAND_IN  uint32                    compare_mask,
      SOC_SAND_OUT uint32                    payload_data[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_OUT uint32                    payload_mask[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_IN  uint32                    mask_val
    )
{
  PCP_LEM_ACCESS_PAYLOAD
    payload;
  uint8
    payload_valid=FALSE;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_PAYLOAD_MASK_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(payload_data);
  SOC_SAND_CHECK_NULL_INPUT(payload_mask);

  /*
   *	The Destination type mask is not supported for this API (different prefix lengths)
   */
  payload_mask[0] = 0xffffffff;
  payload_mask[1] = 0x1ff;

  if ((compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL) != 0)
  {
    payload_mask[0] &= SOC_SAND_ZERO_BITS_MASK(PCP_LEM_ACCESS_DEST_NOF_BITS - 1, 0);
  }

  if ((compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO) != 0)
  {
    payload_mask[0] &= SOC_SAND_ZERO_BITS_MASK(PCP_LEM_ACCESS_ASD_FIRST_REG_MSB, PCP_LEM_ACCESS_ASD_FIRST_REG_LSB);
    /* last bit is for SA-drop*/
    payload_mask[1] &= SOC_SAND_ZERO_BITS_MASK(PCP_LEM_ACCESS_ASD_SCND_REG_MSB-1, PCP_LEM_ACCESS_ASD_SCND_REG_LSB);
  }

  if ((compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC) != 0)
  {
    payload_mask[1] &= SOC_SAND_ZERO_BITS_MASK(PCP_LEM_ACCESS_IS_DYN_LSB, PCP_LEM_ACCESS_IS_DYN_LSB);
  }

  if ((compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA) != 0)
  {
    payload_mask[1] &= SOC_SAND_ZERO_BITS_MASK(PCP_LEM_ACCESS_ASD_SCND_REG_MSB, PCP_LEM_ACCESS_ASD_SCND_REG_MSB);
  }

  if (compare_mask & (PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA|PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC|PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO|PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL))
  {
    payload_valid = TRUE;
  }

  if (payload_valid)
  {
    res = pcp_frwrd_mact_payload_build(
              unit,
              entry_val,
              &payload
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    
    res = pcp_lem_access_payload_build(
            unit,
            &payload,
            payload_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (mask_val == 0)
  {
    payload_mask[0] = ~payload_mask[0];
    payload_mask[1] = ~payload_mask[1] & 0x1ff;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_payload_mask_build()", 0, 0);
}

STATIC uint32
  pcp_frwrd_mact_payload_mask_retrieve(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                    payload_data[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_IN  uint32                    payload_mask[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_OUT  PCP_LEM_ACCESS_PAYLOAD   *payload,
      SOC_SAND_OUT  uint32                   *compare_mask
    )
{
  uint32
    mask;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_PAYLOAD_MASK_RETRIEVE);

  SOC_SAND_CHECK_NULL_INPUT(compare_mask);
  SOC_SAND_CHECK_NULL_INPUT(payload);

  payload->dest = SOC_SAND_GET_BITS_RANGE(payload_data[0], PCP_LEM_ACCESS_DEST_NOF_BITS-1, 0);
  payload->asd = SOC_SAND_GET_BITS_RANGE(payload_data[0], PCP_LEM_ACCESS_ASD_FIRST_REG_MSB, PCP_LEM_ACCESS_ASD_FIRST_REG_LSB)
    + SOC_SAND_GET_BITS_RANGE(payload_data[1], PCP_LEM_ACCESS_ASD_SCND_REG_MSB, PCP_LEM_ACCESS_ASD_SCND_REG_LSB);
  payload->is_dynamic = SOC_SAND_GET_BIT(payload_data[1], PCP_LEM_ACCESS_IS_DYN_LSB);

  /*
   *	The Destination type mask is not supported for this API (different prefix lengths)
   */
  mask = 0;
  
  if (SOC_SAND_GET_BITS_RANGE(payload_mask[0], PCP_LEM_ACCESS_DEST_NOF_BITS-1, 0) != 0x0)
  {
    mask += PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL;
  }

  if (
      (SOC_SAND_GET_BITS_RANGE(payload_mask[0], PCP_LEM_ACCESS_ASD_FIRST_REG_MSB, PCP_LEM_ACCESS_ASD_FIRST_REG_LSB) != 0x0)
      && (SOC_SAND_GET_BITS_RANGE(payload_mask[1], PCP_LEM_ACCESS_ASD_SCND_REG_MSB, PCP_LEM_ACCESS_ASD_SCND_REG_LSB) != 0x0)
     )
  {
    mask += PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO;
  }

  if (SOC_SAND_GET_BIT(payload_mask[0], PCP_LEM_ACCESS_IS_DYN_LSB) != 0x0)
  {
    mask += PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC;
  }

  *compare_mask = mask;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_payload_mask_retrieve()", 0, 0);
}

STATIC uint32
  pcp_frwrd_mact_key_convert(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY *mac_entry_key,
      SOC_SAND_OUT PCP_LEM_ACCESS_KEY       *key
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_KEY_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(key);

  switch(mac_entry_key->key_type)
  {
  case PCP_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
    key->type = PCP_LEM_ACCESS_KEY_TYPE_MAC;
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_MAC;

    /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    res = soc_sand_pp_mac_address_struct_to_long(
            &(mac_entry_key->key_val.mac.mac),
            key->param[0].value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    key->param[1].value[0] = mac_entry_key->key_val.mac.fid;
    key->param[1].value[1] = 0;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC;
    key->param[1].nof_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_MAC;
    break;
  
  case PCP_FRWRD_MACT_KEY_TYPE_IPV4_MC:
    key->type = PCP_LEM_ACCESS_KEY_TYPE_IPV4_MC;
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPV4_MC;
    key->param[0].value[0] = SOC_SAND_GET_BITS_RANGE(mac_entry_key->key_val.ipv4_mc.dip, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC-1, 0);
    key->param[0].value[1] = 0;
    key->param[1].value[0] = mac_entry_key->key_val.ipv4_mc.fid;
    key->param[1].value[1] = 0;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC;
    key->param[1].nof_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_key_convert()", 0, 0);
}

    
STATIC uint32
  pcp_frwrd_mact_rule_to_key_convert(
      SOC_SAND_IN  int                             unit,
      SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE *rule,
      SOC_SAND_OUT PCP_LEM_ACCESS_KEY                  *rule_key,
      SOC_SAND_OUT PCP_LEM_ACCESS_KEY                  *rule_key_mask,
      SOC_SAND_IN  uint32                              mask_val
    )
{
  uint32
    res;
  PCP_FRWRD_MACT_ENTRY_KEY
    mac_entry_key_rule,
    mac_entry_key;
  uint32
    mac_add_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_RULE_TO_KEY_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(rule);

  PCP_FRWRD_MACT_ENTRY_KEY_clear(&mac_entry_key);
  PCP_FRWRD_MACT_ENTRY_KEY_clear(&mac_entry_key_rule);

  mac_entry_key.key_type = rule->key_type;
  mac_entry_key_rule.key_type = rule->key_type;
  switch(rule->key_type)
  {
  case PCP_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
    PCP_COPY(&(mac_entry_key.key_val.mac.mac), &(rule->key_rule.mac.mac), SOC_SAND_PP_MAC_ADDRESS, 1);
    PCP_COPY(&(mac_entry_key_rule.key_val.mac.mac), &(rule->key_rule.mac.mac_mask), SOC_SAND_PP_MAC_ADDRESS, 1);
    if (mask_val)
    {
      /* Take the inverse MAC mask for the flush machine */
      for (mac_add_ndx = 0; mac_add_ndx < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; mac_add_ndx++)
      {
        mac_entry_key_rule.key_val.mac.mac.address[mac_add_ndx] = ~(mac_entry_key_rule.key_val.mac.mac.address[mac_add_ndx]);
      }
    }
    mac_entry_key.key_val.mac.fid = rule->key_rule.mac.fid;
    mac_entry_key_rule.key_val.mac.fid = (mask_val)?~rule->key_rule.mac.fid_mask:rule->key_rule.mac.fid_mask;
    break;

  case PCP_FRWRD_MACT_KEY_TYPE_IPV4_MC:
    mac_entry_key.key_val.ipv4_mc.dip = rule->key_rule.ipv4_mc.dip;
    mac_entry_key_rule.key_val.ipv4_mc.dip = rule->key_rule.ipv4_mc.dip_mask;
    mac_entry_key.key_val.ipv4_mc.fid = rule->key_rule.ipv4_mc.fid;
    mac_entry_key_rule.key_val.ipv4_mc.fid = (mask_val)?~rule->key_rule.ipv4_mc.fid_mask:rule->key_rule.ipv4_mc.fid_mask;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  res = pcp_frwrd_mact_key_convert(
          unit,
          &mac_entry_key,
          rule_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = pcp_frwrd_mact_key_convert(
          unit,
          &mac_entry_key_rule,
          rule_key_mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /* reset prefix value so no mask for DB-id*/
  rule_key_mask->prefix.value = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_rule_to_key_convert()", 0, 0);
}

uint32
  pcp_frwrd_mact_key_parse(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  PCP_LEM_ACCESS_KEY       *key,
      SOC_SAND_OUT PCP_FRWRD_MACT_ENTRY_KEY *mac_entry_key
    )
{
  uint32
    reg_ndx,
    shift_ndx,
    mac_add_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_KEY_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(key);

  switch(key->type)
  {
  case PCP_LEM_ACCESS_KEY_TYPE_MAC:
    mac_entry_key->key_type = PCP_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    for (mac_add_ndx = 0; mac_add_ndx < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++mac_add_ndx)
    {
      reg_ndx = mac_add_ndx / PCP_FRWRD_MACT_NOF_MAC_ADDR_IN_UINT32;
      shift_ndx = mac_add_ndx % PCP_FRWRD_MACT_NOF_MAC_ADDR_IN_UINT32;
      mac_entry_key->key_val.mac.mac.address[mac_add_ndx] = (uint8)
        SOC_SAND_GET_BITS_RANGE(key->param[0].value[reg_ndx],
          PCP_FRWRD_MACT_NOF_BITS_IN_MAC_ADDR*shift_ndx + (PCP_FRWRD_MACT_NOF_BITS_IN_MAC_ADDR-1),
          PCP_FRWRD_MACT_NOF_BITS_IN_MAC_ADDR*shift_ndx
        );
    }
    mac_entry_key->key_val.mac.fid = key->param[1].value[0];
    break;
  
  case PCP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
    mac_entry_key->key_type = PCP_FRWRD_MACT_KEY_TYPE_IPV4_MC;
    mac_entry_key->key_val.ipv4_mc.dip = key->param[0].value[0] + PCP_FRWRD_MACT_IPV4_MC_DIP_BASE_VAL;
    mac_entry_key->key_val.ipv4_mc.fid = key->param[1].value[0];
    break;

  default:
    break; /* Avoid the error for empty events */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_key_parse()", 0, 0);
}

STATIC uint32
  pcp_frwrd_mact_rule_from_key_build(
      SOC_SAND_IN  int                             unit,
      SOC_SAND_IN PCP_LEM_ACCESS_KEY                  *rule_key,
      SOC_SAND_IN PCP_LEM_ACCESS_KEY                  *rule_key_mask,
      SOC_SAND_OUT PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE *rule
    )
{
  uint32
    res;
  PCP_FRWRD_MACT_ENTRY_KEY
    mac_entry_key_rule,
    mac_entry_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_RULE_FROM_KEY_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(rule);

  PCP_FRWRD_MACT_ENTRY_KEY_clear(&mac_entry_key);
  PCP_FRWRD_MACT_ENTRY_KEY_clear(&mac_entry_key_rule);

  res = pcp_frwrd_mact_key_parse(
          unit,
          rule_key,
          &mac_entry_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_frwrd_mact_key_parse(
          unit,
          rule_key_mask,
          &mac_entry_key_rule
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  rule->key_type = mac_entry_key.key_type;
  switch(mac_entry_key.key_type)
  {
  case PCP_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
    PCP_COPY(&(rule->key_rule.mac.mac), &(mac_entry_key.key_val.mac.mac), SOC_SAND_PP_MAC_ADDRESS, 1);
    PCP_COPY(&(rule->key_rule.mac.mac_mask), &(mac_entry_key_rule.key_val.mac.mac), SOC_SAND_PP_MAC_ADDRESS, 1);
    rule->key_rule.mac.fid = mac_entry_key.key_val.mac.fid;
    rule->key_rule.mac.fid_mask = mac_entry_key_rule.key_val.mac.fid;
    break;

  case PCP_FRWRD_MACT_KEY_TYPE_IPV4_MC:
    rule->key_rule.ipv4_mc.dip = mac_entry_key.key_val.ipv4_mc.dip;
    rule->key_rule.ipv4_mc.dip_mask = mac_entry_key_rule.key_val.ipv4_mc.dip;
    rule->key_rule.ipv4_mc.fid = mac_entry_key.key_val.ipv4_mc.fid;
    rule->key_rule.ipv4_mc.fid_mask = mac_entry_key_rule.key_val.ipv4_mc.fid;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_rule_from_key_build()", 0, 0);
}

    
uint32
  pcp_frwrd_mact_payload_convert(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  PCP_LEM_ACCESS_PAYLOAD     *payload,
      SOC_SAND_OUT PCP_FRWRD_MACT_ENTRY_VALUE *mac_entry_value
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_PAYLOAD_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);

  /*
   *	Convert the payload
   */
  mac_entry_value->aging_info.age_status = (uint8) payload->age;
  mac_entry_value->aging_info.is_dynamic = payload->is_dynamic;

  /*
   *	Get the encoded destination and ASD
   */
  res = pcp_fwd_decision_in_buffer_parse(
          PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
          payload->dest,
          payload->asd,
          &(mac_entry_value->frwrd_info.forward_decision),
          &(mac_entry_value->frwrd_info.drop_when_sa_is_known)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_payload_convert()", 0, 0);
}


/*********************************************************************
*     Add an entry to the MAC table. Given a key, e.g. (FID,
 *     MAC), the packets associated with this FID and having
 *     this MAC address as DA will be processed and forwarded
 *     according to the given value.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    dest_encoded,
    asd_encoded,
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_REQUEST
    request;
  PCP_LEM_ACCESS_CMD
    access_cmd;
  PCP_LEM_ACCESS_PAYLOAD
    payload;
  PCP_LEM_ACCESS_ACK_STATUS
    ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(success);

  PCP_LEM_ACCESS_REQUEST_clear(&request);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  /*
   *	Adapt the variables for the generic access to LEM (Large exact match)
   */
  switch(add_type)
  {
  case PCP_FRWRD_MACT_ADD_TYPE_INSERT:
    access_cmd = PCP_LEM_ACCESS_CMD_INSERT;
      break;

  case PCP_FRWRD_MACT_ADD_TYPE_TRANSPLANT:
    access_cmd = PCP_LEM_ACCESS_CMD_TRANSPLANT;
      break;

  case PCP_FRWRD_MACT_ADD_TYPE_LEARN:
    access_cmd = PCP_LEM_ACCESS_CMD_LEARN;
      break;

  case PCP_FRWRD_MACT_ADD_TYPE_REFRESH:
    access_cmd = PCP_LEM_ACCESS_CMD_REFRESH;
      break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }
  request.command = access_cmd;
  request.stamp = 0;

  res = pcp_frwrd_mact_key_convert(
          unit,
          mac_entry_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  payload.age = mac_entry_value->aging_info.age_status;
  payload.is_dynamic = mac_entry_value->aging_info.is_dynamic;

  /*
   *	Get the encoded destination and ASD
   */
  res = pcp_fwd_decision_in_buffer_build(
          PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
          &(mac_entry_value->frwrd_info.forward_decision),
          mac_entry_value->frwrd_info.drop_when_sa_is_known,
          &dest_encoded,
          &asd_encoded
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  payload.dest = dest_encoded;
  payload.asd = asd_encoded;

  /*
   *	Access to the LEM (i.e., MAC Table)
   */
  res = pcp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (ack_status.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    switch(ack_status.reason)
    {
    case PCP_LEM_ACCESS_FAIL_REASON_MACT_FULL:
    case PCP_LEM_ACCESS_FAIL_REASON_CAM_FULL:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      break;

    case PCP_LEM_ACCESS_FAIL_REASON_FID_LIMIT:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
      break;

    case PCP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT:
   case PCP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP:
    case PCP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN:
    case PCP_LEM_ACCESS_FAIL_REASON_UNKNOWN:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_3;
      break;

   case PCP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC:
    case PCP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC:
      *success = SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST;
      break;

    case PCP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN:
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_mact_entry_add_unsafe()",0,0);
}

uint32
  pcp_frwrd_mact_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value
  )
{
 uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(add_type, PCP_FRWRD_MACT_ADD_TYPE_MAX, PCP_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_KEY, mac_entry_key, 20, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_VALUE, mac_entry_value, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_entry_add_verify()", 0, 0);
}

/*********************************************************************
*     Remove an entry from the MAC table according to the
 *     given key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_REQUEST
    request;
  PCP_LEM_ACCESS_ACK_STATUS
    ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);

  PCP_LEM_ACCESS_REQUEST_clear(&request);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  /*
   *	Adapt the variables for the generic access to LEM (Large exact match)
   */
  request.command = PCP_LEM_ACCESS_CMD_DELETE;
  request.stamp = 0;

  res = pcp_frwrd_mact_key_convert(
          unit,
          mac_entry_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *	Access to the LEM (i.e., MAC Table)
   */
  res = pcp_lem_access_entry_remove_unsafe(
          unit,
          &request,
          &ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (!((ack_status.is_success == TRUE) || (ack_status.reason == PCP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN)))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_entry_remove_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_mact_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_ENTRY_REMOVE_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_KEY, mac_entry_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_entry_remove_verify()", 0, 0);
}

/*********************************************************************
*     Get an entry according to its key, e.g. (FID, MAC
 *     address).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT PCP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_KEY
    key;
  PCP_LEM_ACCESS_PAYLOAD
    payload;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(found);

  PCP_FRWRD_MACT_ENTRY_VALUE_clear(mac_entry_value);
  PCP_LEM_ACCESS_KEY_clear(&key);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);

  /*
   *	Adapt the variables for the generic access to LEM (Large exact match)
   */
  res = pcp_frwrd_mact_key_convert(
          unit,
          mac_entry_key,
          &(key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Access to the MAC Table
   */
  res = pcp_lem_access_entry_by_key_get_unsafe(
          unit,
          &key,
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	Convert the payload
   */
  res = pcp_frwrd_mact_payload_convert(
          unit,
          &payload,
          mac_entry_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *found = is_found;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_entry_get_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_mact_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_ENTRY_GET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_KEY, mac_entry_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_entry_get_verify()", 0, 0);
}

/*********************************************************************
*     Traverse the MACT entries. Compare each entry to a given
 *     rule, and for matching entries perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_traverse_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{
  uint32
    fld_val,
    waiting_time_in_ms,
    payload_ndx,
    payload_data[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_mask[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_new_data[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_new_mask[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    res = SOC_SAND_OK;
  PCP_REGS
    *regs;
  uint32
    wait_loop_ndx,
    entry_offset = 0;
  PCP_ELK_FLUSH_DB_TBL_DATA
    tbl_data;
  PCP_LEM_ACCESS_KEY
    rule_key,
    rule_key_mask;
  PCP_LEM_ACCESS_PAYLOAD
    payload_new,
    payload;
  PCP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer_mask,
    key_in_buffer;
  PCP_FRWRD_MACT_TRAVERSE_STATUS_INFO
    status;
  uint8
    is_wait_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_TRAVERSE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

  PCP_LEM_ACCESS_KEY_clear(&rule_key);
  PCP_LEM_ACCESS_KEY_clear(&rule_key_mask);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload_new);
  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer_mask);
  PCP_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(&status);

  for (payload_ndx = 0; payload_ndx < PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
  {
    payload_data[payload_ndx] = 0;
    payload_new_data[payload_ndx] = 0;
    payload_mask[payload_ndx] = PCP_FRWRD_MACT_MASK_DEFAULT_NULL;
    payload_new_mask[payload_ndx] = PCP_FRWRD_MACT_MASK_DEFAULT_NULL;
  }

  regs = pcp_regs();

  /*
   *	Get the rule parameters in the internal form
   */
  res = pcp_frwrd_mact_rule_to_key_convert(
          unit,
          rule,
          &rule_key,
          &rule_key_mask,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  res = pcp_frwrd_mact_payload_mask_build(
          unit,
          &(rule->value_rule.val),
          rule->value_rule.compare_mask,
          payload_data,
          payload_mask,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	Configure the Flush machine
   */
  fld_val = 0x0;
  PCP_FLD_SET(regs->elk.flu_machine_end_index_reg.flu_machine_end_index, fld_val, 10, exit);

  /*
   *	Set the rule of the Flush machine (only one rule)
   *  The following sentence is true for MAC and IPv4 compatible MC at least
   */
  tbl_data.compare_valid = TRUE;
  
  res = pcp_lem_key_encoded_build(
          unit,
          &rule_key,
          &key_in_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  res = pcp_lem_key_encoded_build(
          unit,
          &rule_key_mask,
          &key_in_buffer_mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  tbl_data.key_fid  = SOC_SAND_GET_BITS_RANGE(key_in_buffer.buffer[1]     , PCP_FRWRD_MACT_KEY_15_MSBS_MSB, PCP_FRWRD_MACT_KEY_15_MSBS_LSB);
  tbl_data.key_mask = SOC_SAND_GET_BITS_RANGE(key_in_buffer_mask.buffer[1], PCP_FRWRD_MACT_KEY_15_MSBS_MSB, PCP_FRWRD_MACT_KEY_15_MSBS_LSB);

  for (payload_ndx = 0; payload_ndx < PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
  {
    tbl_data.payload[payload_ndx]      = payload_data[payload_ndx];
    tbl_data.payload_mask[payload_ndx] = payload_mask[payload_ndx];
  }

  if (action->type == PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE)
  {
    tbl_data.drop = 0x1;
  }
  else
  {
    tbl_data.drop = 0x0;
  }

  if (action->type == PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE)
  {
    res = pcp_frwrd_mact_payload_mask_build(
            unit,
            &action->updated_val,
            action->update_mask,
            payload_new_data,
            payload_new_mask,
            0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    for (payload_ndx = 0; payload_ndx < PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
    {
      tbl_data.new_payload[payload_ndx]      = payload_new_data[payload_ndx];
      tbl_data.new_payload_mask[payload_ndx] = payload_new_mask[payload_ndx];
    }
  }
  else
  {
    /*
     *	For the count mode, set the mask to 0 to change nothing
     */
    for (payload_ndx = 0; payload_ndx < PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
    {
      tbl_data.new_payload[payload_ndx] = 0x0;
      tbl_data.new_payload_mask[payload_ndx] = 0x0;
    }
  }

  res = pcp_elk_flush_db_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   *	Set the trigger to run the Flush machine.
   */
  fld_val = PCP_FRWRD_MACT_TABLE_ENTRY_MAX;
  PCP_FLD_SET(regs->elk.flu_machine_current_index_reg.flu_machine_current_index , fld_val, 70, exit);

  /*
   *	Wait until the flush machine finishes according to the expected finish time
   */
  if (wait_till_finish)
  {
    /*
     *	Get the number of matched entries
     */
    is_wait_success = FALSE;
    for (wait_loop_ndx = 0; wait_loop_ndx < PCP_FRWRD_MACT_TABLE_TRAVERSE_NOF_LOOPS; ++wait_loop_ndx)
    {
      res = pcp_frwrd_mact_traverse_status_get_unsafe(
              unit,
              &status
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

#ifdef SAND_LOW_LEVEL_SIMULATION
      status.time_to_finish.mili_sec = 0;
      status.time_to_finish.sec = 0;
      status.nof_matched_entries = 0;
#endif

      if ((status.time_to_finish.sec == 0) && (status.time_to_finish.mili_sec == 0))
      {
        is_wait_success = TRUE;
        *nof_matched_entries = status.nof_matched_entries;
        break;
      }
      else
      {
        waiting_time_in_ms = status.time_to_finish.mili_sec
          + (status.time_to_finish.sec * PCP_FRWRD_MACT_NOF_MS_IN_SEC);
        res = SOC_SAND_OK; sal_msleep(waiting_time_in_ms);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      }
    }
    if (is_wait_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR, 90, exit);
    }
  }
  else
  {
    /*
     *	No wait to match
     */
    *nof_matched_entries = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_traverse_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_mact_traverse_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_TRAVERSE_VERIFY);

  /*
   *	Do not verify because of the influence of the mask
   */
/*
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE, rule, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TRAVERSE_ACTION, action, 20, exit);
*/
  PCP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_traverse_verify()", 0, 0);
}

/*********************************************************************
*     Returns the status of the traverse, including which
 *     action is performed and according to what rule, besides
 *     the expected time to finish the traverse, and the number
 *     of matched entries if the traverse was finished.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_traverse_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  )
{
  uint32
    ticks_per_ms = 0,
    time_remaning_ms,
	cpu_acc_per_clk,
    fld_val,
    access_in_clks,
    res = SOC_SAND_OK;
  PCP_REGS
    *regs;
  uint32
    entry_offset = 0;
  PCP_ELK_FLUSH_DB_TBL_DATA
    tbl_data;
  PCP_LEM_ACCESS_KEY
    rule_key,
    rule_key_mask;
  PCP_LEM_ACCESS_PAYLOAD
    payload_new,
    payload;
  PCP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer_mask,
    key_in_buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_TRAVERSE_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(status);

  PCP_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(status);

  regs = pcp_regs();

  PCP_LEM_ACCESS_KEY_clear(&rule_key);
  PCP_LEM_ACCESS_KEY_clear(&rule_key_mask);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload_new);
  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer_mask);

  /*
   *	Number of ticks per ms
   */
  ticks_per_ms = pcp_chip_kilo_ticks_per_sec_get(); 

  /*
   *	Get the current index the Flush machine goes through and evaluate
   *  the remaining time
   */
  PCP_FLD_GET(regs->elk.flu_machine_current_index_reg.flu_machine_current_index,    fld_val, 				10, exit);
  PCP_FLD_GET(regs->elk.flu_machine_configuration_reg.flu_machine_access_shaper, access_in_clks, 	20, exit);
  PCP_FLD_GET(regs->eci.cpu_acc_per_clk_reg.cpu_acc_per_clk, 								 cpu_acc_per_clk, 25, exit);

  if ((fld_val == 0))
  {
    /*
     *	The Flush machine has finished
     */
    time_remaning_ms = 0;
  }
  else
  {
    time_remaning_ms = (fld_val * SOC_SAND_MAX(access_in_clks,cpu_acc_per_clk) * PCP_FRWRD_MACT_TRAVERSE_STATUS_GET_TIME_REMANING_MS_MULTI_CONST) / (ticks_per_ms);
  }
  status->time_to_finish.sec = time_remaning_ms / PCP_FRWRD_MACT_NOF_MS_IN_SEC;
  status->time_to_finish.mili_sec = time_remaning_ms % PCP_FRWRD_MACT_NOF_MS_IN_SEC;

  /*
   *	Get the rule table entry
   */
  res = pcp_elk_flush_db_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   *  Get the action type
   */
  if (tbl_data.drop == 0x1)
  {
    status->action.type = PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;
  }
  else
  {
    if ((tbl_data.new_payload_mask[0] == 0x0) && (tbl_data.new_payload_mask[1] == 0x0))
    {
      status->action.type = PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT;
    }
    else
    {
      status->action.type = PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE;
      /*
       *	Retrieve the payload for the update
       */
      res = pcp_frwrd_mact_payload_mask_retrieve(
              unit,
              tbl_data.new_payload,
              tbl_data.new_payload_mask,
              &payload_new,
              &(status->action.update_mask)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = pcp_frwrd_mact_payload_convert(
              unit,
              &payload_new,
              &(status->action.updated_val)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }

  /*
   *	Retrieve the rule key
   */
  key_in_buffer.buffer[1] |= SOC_SAND_SET_BITS_RANGE(tbl_data.key_fid, PCP_FRWRD_MACT_KEY_15_MSBS_MSB, PCP_FRWRD_MACT_KEY_15_MSBS_LSB);
  key_in_buffer_mask.buffer[1] |= SOC_SAND_SET_BITS_RANGE(tbl_data.key_mask, PCP_FRWRD_MACT_KEY_15_MSBS_MSB-1, PCP_FRWRD_MACT_KEY_15_MSBS_LSB);
  /* to set MSB, as removed from mask */
  key_in_buffer_mask.buffer[1] |= SOC_SAND_BIT(30);

  res = pcp_lem_key_encoded_parse(
          unit,
          &key_in_buffer,
          &rule_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = pcp_lem_key_encoded_parse(
          unit,
          &key_in_buffer_mask,
          &rule_key_mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = pcp_frwrd_mact_rule_from_key_build(
          unit,
          &rule_key,
          &rule_key_mask,
          &(status->rule)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

  /*
   *  Retrieve the rule payload	only after
   */
  res = pcp_frwrd_mact_payload_mask_retrieve(
          unit,
          tbl_data.payload,
          tbl_data.payload_mask,
          &payload,
          &(status->rule.value_rule.compare_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = pcp_frwrd_mact_payload_convert(
          unit,
          &payload,
          &(status->rule.value_rule.val)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /* invalidate FLUSH entry if done */
  if (status->time_to_finish.mili_sec == 0 && status->time_to_finish.sec == 0)
  {
    /*
     *	Get the number of matched entries
     */
    PCP_FLD_GET(regs->elk.flu_machine_hit_counter_reg.flu_machine_hit_counter, fld_val, 30, exit);
    status->nof_matched_entries = fld_val;

    tbl_data.compare_valid = FALSE;

    res = pcp_elk_flush_db_tbl_set_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_traverse_status_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Traverse the MAC Table entries (in specified range), and
 *     retrieve entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT PCP_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT PCP_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    entries_to_scan,
    nof_valid_entries = 0,
    res = SOC_SAND_OK;
  uint32
    param_ndx,
    value_ndx,
    entry_ndx;
  uint8
    is_found,
    is_key_for_rule,
    is_param_for_rule,
    is_valid;
  PCP_LEM_ACCESS_KEY
    rule_key,
    rule_key_mask,
    key;
  PCP_LEM_ACCESS_KEY_TYPE
    key_type;
  PCP_FRWRD_MACT_ENTRY_KEY
    mac_key;
  PCP_LEM_ACCESS_PAYLOAD
    payload;
  PCP_FRWRD_MACT_ENTRY_VALUE
    mac_entry_value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(mact_keys);
  SOC_SAND_CHECK_NULL_INPUT(mact_vals);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  PCP_LEM_ACCESS_KEY_clear(&rule_key);
  PCP_LEM_ACCESS_KEY_clear(&rule_key_mask);
  PCP_FRWRD_MACT_ENTRY_KEY_clear(&mac_key);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);
  PCP_FRWRD_MACT_ENTRY_VALUE_clear(&mac_entry_value);

  /*
   *	First verification on the block iteration
   */
  if (block_range->iter > PCP_FRWRD_MACT_TABLE_ENTRY_MAX)
  {
    *nof_entries = 0;
    SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
    PCP_DO_NOTHING_AND_EXIT;
  }
  entries_to_scan = block_range->entries_to_scan;

  /*
   *	Special case of full MACT
   */
  if ((entries_to_scan == PCP_LEM_NOF_ENTRIES) || (entries_to_scan == SOC_SAND_TBL_ITER_SCAN_ALL))
  {
    entries_to_scan = PCP_FRWRD_MACT_TABLE_NOF_ENTRIES;
  }

  /*
   *	Get the rule key
   */
  res = pcp_frwrd_mact_rule_to_key_convert(
          unit,
          rule,
          &rule_key,
          &rule_key_mask,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Loop on the entry index
   */
  for (entry_ndx = block_range->iter; entry_ndx < block_range->iter + entries_to_scan; ++entry_ndx)
  {
    
    if (nof_valid_entries >= block_range->entries_to_act || entry_ndx  >= PCP_FRWRD_MACT_TABLE_NOF_ENTRIES)
    {
      /*
       *	No need to go further, maximal number of entries have been found
       */
      break;
    }

    PCP_LEM_ACCESS_KEY_clear(&key);
    res = pcp_lem_access_entry_by_index_get_unsafe(
            unit,
            entry_ndx,
            &key,
            &is_valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
     *	Continue only for valid keys (existing entries)
     */
    if (is_valid == TRUE)
    {
      /*
       *	Check if the key follows the rules first for MAC and IPv4 MC compatible addresses
       */
      is_key_for_rule = TRUE;
      switch(rule->key_type)
      {
      case PCP_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
        key_type = PCP_LEM_ACCESS_KEY_TYPE_MAC;
       break;
      
      case PCP_FRWRD_MACT_KEY_TYPE_IPV4_MC:
        key_type = PCP_LEM_ACCESS_KEY_TYPE_IPV4_MC;

        break;

      default:
        SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
      }

      if ((key.type != key_type) && (rule))
      {
        is_key_for_rule = FALSE;
      }
      for (param_ndx = 0; param_ndx < PCP_LEM_KEY_MAX_NOF_PARAMS; ++param_ndx)
      {
        for (value_ndx = 0; value_ndx < PCP_LEM_KEY_PARAM_MAX_IN_UINT32S; ++value_ndx)
        {
          if ((key.param[param_ndx].value[value_ndx] & rule_key_mask.param[param_ndx].value[value_ndx])
            != (rule_key.param[param_ndx].value[value_ndx] & rule_key_mask.param[param_ndx].value[value_ndx]))
          {
            is_key_for_rule = FALSE;
          }
        }
      }

      /*
       *	Continue only if the key mask is validated
       */
      if (is_key_for_rule == TRUE)
      {
        /*
         *	Get the payload for this corresponding key
         */

        res = pcp_lem_access_entry_by_key_get_unsafe(
                unit,
                &key,
                &payload,
                &is_found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        /*
         *	The payload might be not found if deleted meanwhile
         */
        if (is_found == TRUE)
        {
          /*
           *	Check if the payload is relevant
           */
          res = pcp_frwrd_mact_payload_convert(
                  unit,
                  &payload,
                  &mac_entry_value
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          is_param_for_rule = TRUE;

          /*
           *	Compare according to the bitmap mask
           */
          if ((rule->value_rule.compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE) != 0)
          {
            if (rule->value_rule.val.frwrd_info.forward_decision.type
              != mac_entry_value.frwrd_info.forward_decision.type)
            {
              is_param_for_rule = FALSE;
            }
          }

          if ((rule->value_rule.compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL) != 0)
          {
            if (rule->value_rule.val.frwrd_info.forward_decision.dest_id
              != mac_entry_value.frwrd_info.forward_decision.dest_id)
            {
              is_param_for_rule = FALSE;
            }
          }

          if ((rule->value_rule.compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO) != 0)
          {
            if (
                   (rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.type
                      != mac_entry_value.frwrd_info.forward_decision.additional_info.eei.type)
                  ||(rule->value_rule.val.frwrd_info.forward_decision.additional_info.outlif.type
                      != mac_entry_value.frwrd_info.forward_decision.additional_info.outlif.type)
                  ||(rule->value_rule.val.frwrd_info.forward_decision.additional_info.outlif.val
                      != mac_entry_value.frwrd_info.forward_decision.additional_info.outlif.val)
                  ||(rule->value_rule.val.frwrd_info.forward_decision.additional_info.trap_info.action_profile.frwrd_action_strength
                      != mac_entry_value.frwrd_info.forward_decision.additional_info.trap_info.action_profile.frwrd_action_strength)
                  ||(rule->value_rule.val.frwrd_info.forward_decision.additional_info.trap_info.action_profile.snoop_action_strength
                      != mac_entry_value.frwrd_info.forward_decision.additional_info.trap_info.action_profile.snoop_action_strength)
                  ||(rule->value_rule.val.frwrd_info.forward_decision.additional_info.trap_info.action_profile.trap_code
                      != mac_entry_value.frwrd_info.forward_decision.additional_info.trap_info.action_profile.trap_code)
                )
            {
              is_param_for_rule = FALSE;
            }

            if (rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.type == PCP_EEI_TYPE_TRILL)
            {
              if (
                    (rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.val.trill_dest.dest_nick
                    != mac_entry_value.frwrd_info.forward_decision.additional_info.eei.val.trill_dest.dest_nick)
                  ||(rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.val.trill_dest.is_multicast
                    != mac_entry_value.frwrd_info.forward_decision.additional_info.eei.val.trill_dest.is_multicast)
                )
              {
                is_param_for_rule = FALSE;
              }
            }
            else if (rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.type == PCP_EEI_TYPE_MPLS)
            {
              if (
                    (rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.command
                    != mac_entry_value.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.command)
                  ||(rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.label
                    != mac_entry_value.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.label)
                  ||(rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.push_profile
                    != mac_entry_value.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.push_profile)
                )
              {
                is_param_for_rule = FALSE;
              }
            }
            else if (rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.type == PCP_EEI_TYPE_MIM)
            {
              if (
                  (rule->value_rule.val.frwrd_info.forward_decision.additional_info.eei.val.isid
                  != mac_entry_value.frwrd_info.forward_decision.additional_info.eei.val.isid)
                )
              {
                is_param_for_rule = FALSE;
              }
            }

          }

          if ((rule->value_rule.compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC) != 0)
          {
            if (rule->value_rule.val.aging_info.is_dynamic
              != mac_entry_value.aging_info.is_dynamic)
            {
              is_param_for_rule = FALSE;
            }
          }

          if ((rule->value_rule.compare_mask & PCP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA) != 0)
          {
            if (rule->value_rule.val.frwrd_info.drop_when_sa_is_known
                  != mac_entry_value.frwrd_info.drop_when_sa_is_known)
            {
              is_param_for_rule = FALSE;
            }
          }

          /*
           *	Continue only if the payload is validated
           */
          if (is_param_for_rule == TRUE)
          {
            /*
             *	This entry is validated
             */
            res = pcp_frwrd_mact_key_parse(
                    unit,
                    &key,
                    &mac_key
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            mact_keys[nof_valid_entries] = mac_key;
            mact_vals[nof_valid_entries] = mac_entry_value;

            nof_valid_entries ++;
          }
        }
      }
    }
  }

  /*
   *	Return the right values
   */
  *nof_entries = nof_valid_entries;
  block_range->iter = entry_ndx;

  if (entry_ndx  >= PCP_FRWRD_MACT_TABLE_NOF_ENTRIES)
  {
    SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_get_block_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_mact_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_GET_BLOCK_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE, rule, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(block_range->entries_to_act, PCP_FRWRD_MACT_GET_BLOCK_MAX_ENTRIES_PER_ITER, PCP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(block_range->iter, PCP_FRWRD_MACT_TABLE_NOF_ENTRIES - 1, PCP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_api_frwrd_mact module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_frwrd_mact_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_frwrd_mact;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_frwrd_mact module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_frwrd_mact_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_frwrd_mact;
}

uint32
  PCP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, &(info->mac), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid, PCP_FID_MAX, PCP_FID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_ENTRY_KEY_IPV4_MC_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (SOC_SAND_GET_BITS_RANGE(info->dip, PCP_FRWRD_MACT_IPV4_DIP_MSB, PCP_FRWRD_MACT_IPV4_DIP_LSB) != PCP_FRWRD_MACT_IPV4_DIP_VALUE)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_MACT_DIP_OUT_OF_RANGE_ERR, 90, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid, PCP_FID_MAX, PCP_FID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_ENTRY_KEY_IPV4_MC_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_ENTRY_KEY_VAL_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY_VAL *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR, &(info->mac), 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_KEY_IPV4_MC, &(info->ipv4_mc), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_ENTRY_KEY_VAL_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_ENTRY_KEY_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_type, PCP_FRWRD_MACT_KEY_TYPE_MAX, PCP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  if (info->key_type == PCP_FRWRD_MACT_KEY_TYPE_MAC_ADDR)
  {
    PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR, &(info->key_val.mac), 20, exit);
  }
  else
  {
    PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_KEY_IPV4_MC, &(info->key_val.ipv4_mc), 21, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_ENTRY_KEY_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_ENTRY_AGING_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->age_status, PCP_FRWRD_MACT_AGE_STATUS_MIN, PCP_FRWRD_MACT_AGE_STATUS_MAX, PCP_FRWRD_MACT_AGE_STATUS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_ENTRY_AGING_INFO_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_ENTRY_FRWRD_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->forward_decision.type == PCP_FRWRD_DECISION_TYPE_DROP)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_MACT_FWD_DECSION_DROP_ERR, 5, exit);
  }
  PCP_STRUCT_VERIFY(PCP_FRWRD_DECISION_INFO, &(info->forward_decision), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_ENTRY_FRWRD_INFO_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_ENTRY_VALUE_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_ENTRY_VALUE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_FRWRD_INFO, &(info->frwrd_info), 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_AGING_INFO, &(info->aging_info), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_ENTRY_VALUE_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_TIME_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_TIME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sec, PCP_FRWRD_MACT_SEC_MAX, PCP_FRWRD_MACT_SEC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mili_sec, PCP_FRWRD_MACT_MILI_SEC_MAX, PCP_FRWRD_MACT_MILI_SEC_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_TIME_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid, PCP_FID_MAX, PCP_FID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_mask, PCP_FID_MAX, PCP_FID_OUT_OF_RANGE_ERR, 11, exit);
  /*PCP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, &(info->mac), 12, exit);*/
  /*PCP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, &(info->mac_mask), 13, exit);*/

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	No constraints on DIP and DIP mask
   */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid, PCP_FID_MAX, PCP_FID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_mask, PCP_FID_MAX, PCP_FID_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_VALUE, &(info->val), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->compare_mask, PCP_FRWRD_MACT_COMPARE_MASK_MAX, PCP_FRWRD_MACT_COMPARE_MASK_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_type, PCP_FRWRD_MACT_KEY_TYPE_MAX, PCP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  if (info->key_type == PCP_FRWRD_MACT_KEY_TYPE_MAC_ADDR)
  {
    PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC, &(info->key_rule.mac), 11, exit);
  }
  else /* PCP_FRWRD_MACT_KEY_TYPE_MAC_ADDR */
  {
    PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC, &(info->key_rule.ipv4_mc), 12, exit);
  }

  /*
   *	No verification because of the mask influence
   */
/*  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE, &(info->value_rule), 13, exit); */

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_TRAVERSE_ACTION_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_ACTION *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_MAX, PCP_FRWRD_MACT_ACTION_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_ENTRY_VALUE, &(info->updated_val), 11, exit);

  if (info->type == PCP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_MACT_RETRIEVE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_TRAVERSE_ACTION_verify()",0,0);
}

uint32
  PCP_FRWRD_MACT_TRAVERSE_STATUS_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TRAVERSE_MATCH_RULE, &(info->rule), 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TRAVERSE_ACTION, &(info->action), 11, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_MACT_TIME, &(info->time_to_finish), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_matched_entries, PCP_FRWRD_MACT_NOF_MATCHED_ENTRIES_MAX, PCP_FRWRD_MACT_NOF_MATCHED_ENTRIES_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_MACT_TRAVERSE_STATUS_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

