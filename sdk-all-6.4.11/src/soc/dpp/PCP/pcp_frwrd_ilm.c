/* $Id: pcp_frwrd_ilm.c,v 1.7 Broadcom SDK $
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

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_lem_access.h>
#include <soc/dpp/PCP/pcp_frwrd_ilm.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


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
  Pcp_procedure_desc_element_frwrd_ilm[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_ILM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Pcp_error_desc_element_frwrd_ilm[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_FRWRD_ILM_SUCCESS_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_ILM_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    PCP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR,
    "PCP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR",
    "If by the global setting soc_ppd_frwrd_ilm_glbl_info_set, the port is masked, \n\r"
    "then the inport field of ILM key has to be zero \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR,
    "PCP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR",
    "If by the global setting soc_ppd_frwrd_ilm_glbl_info_set, the port is masked, \n\r"
    "then the inrif field of ILM key has to be zero \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_ILM_EEI_NOT_MPLS_ERR,
    "PCP_FRWRD_ILM_EEI_NOT_MPLS_ERR",
    "EEI in ILM add command must be of type MPLS. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR,
    "PCP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR",
    "Mapped_exp ILM key field should be zero for labels not in the ELSP range . \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR,
    "PCP_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR",
    "In ILM key, Inrif and Port are always masked. This setting can not be changed in this SW release. \n\r ",
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

STATIC
  uint32
    pcp_frwrd_ilm_key_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_STRUCT_VERIFY(PCP_FRWRD_ILM_KEY, ilm_key, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_key_verify()", 0, 0);
}

/* build lem access key for ILM host address */
void
    pcp_frwrd_ilm_lem_key_build(
      SOC_SAND_IN  PCP_FRWRD_ILM_KEY  *ilm_key,
      SOC_SAND_OUT PCP_LEM_ACCESS_KEY *key
    )
{
  uint32
    num_bits;

  PCP_LEM_ACCESS_KEY_clear(key);
  
  key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_ILM;
  key->type = PCP_LEM_ACCESS_KEY_TYPE_ILM;

  num_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = ilm_key->in_label;
  
  num_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM;
  key->param[1].nof_bits = (uint8)num_bits;
  key->param[1].value[0] = ilm_key->mapped_exp;

  num_bits = PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
  key->param[2].nof_bits = (uint8)num_bits;
  key->param[2].value[0] = ilm_key->in_local_port;

  num_bits = PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
  key->param[3].nof_bits = (uint8)num_bits;
  key->param[3].value[0] = ilm_key->inrif;

  key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM;
  key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_ILM;
}

/* parse lem access key for IpV4 host address
*/
  void
    pcp_frwrd_ilm_lem_key_parse(
      SOC_SAND_IN PCP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT PCP_FRWRD_ILM_KEY                       *ilm_key
    )
{
  ilm_key->in_label = key->param[0].value[0];
  ilm_key->mapped_exp = (SOC_SAND_PP_MPLS_EXP)key->param[1].value[0];
  ilm_key->in_local_port = key->param[2].value[0];
  ilm_key->inrif = key->param[3].value[0];
}

/* build lem access payload for IpV4 host address */
STATIC
  uint32
    pcp_frwrd_ilm_lem_payload_build(
      SOC_SAND_IN PCP_FRWRD_DECISION_INFO                 *ilm_val,
      SOC_SAND_OUT PCP_LEM_ACCESS_PAYLOAD *payload
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_LEM_ACCESS_PAYLOAD_clear(payload);

  /*
  *	Get the encoded destination and ASD
  */
  res = pcp_fwd_decision_in_buffer_build(
          PCP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
          ilm_val,
          FALSE,
          &payload->dest,
          &payload->asd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  payload->age = 0;
  payload->is_dynamic = FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_lem_request_payload_build()", 0, 0);
}

/* parse lem access payload for IpV4 host address
*/
  void
    pcp_frwrd_ilm_lem_payload_parse(
      SOC_SAND_IN PCP_LEM_ACCESS_PAYLOAD *payload,
      SOC_SAND_OUT  PCP_FRWRD_DECISION_INFO                 *ilm_val
    )
{
  uint8 tmp;

  pcp_fwd_decision_in_buffer_parse(
    PCP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
    payload->dest,
    payload->asd,
    ilm_val,
    &tmp
  );
}

/*********************************************************************
*     Add mapping from incoming label to destination and MPLS
 *     command.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ilm_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_REQUEST
    request;
  PCP_LEM_ACCESS_PAYLOAD
    payload;
  PCP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);
  SOC_SAND_CHECK_NULL_INPUT(ilm_val);
  SOC_SAND_CHECK_NULL_INPUT(success);

  PCP_LEM_ACCESS_REQUEST_clear(&request);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = PCP_LEM_ACCESS_CMD_INSERT;

  pcp_frwrd_ilm_lem_key_build(
      ilm_key,
      &request.key
    );

  pcp_frwrd_ilm_lem_payload_build(
    ilm_val,
    &payload
  );

  res = pcp_lem_access_entry_add_unsafe(
         unit,
         &request,
         &payload,
         &ack
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if(ack.is_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_add_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ilm_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO                 *ilm_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_ADD_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_DECISION_INFO, ilm_val, 20, exit);

  res = pcp_frwrd_ilm_key_verify(
          unit,
          ilm_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if(ilm_val->additional_info.eei.type != PCP_EEI_TYPE_MPLS)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_ILM_EEI_NOT_MPLS_ERR, 60, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the value (destination and MPLS command) the
 *     incoming label key is associated with.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ilm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_OUT PCP_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_KEY
    key;
  PCP_LEM_ACCESS_PAYLOAD
    payload;
  PCP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);
  SOC_SAND_CHECK_NULL_INPUT(ilm_val);
  SOC_SAND_CHECK_NULL_INPUT(found);

  PCP_FRWRD_DECISION_INFO_clear(ilm_val);

  PCP_LEM_ACCESS_KEY_clear(&key);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  pcp_frwrd_ilm_lem_key_build(
    ilm_key,
    &key
  );

  res = pcp_lem_access_entry_by_key_get_unsafe(
          unit,
          &key,
          &payload,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if(*found)
  {
    pcp_frwrd_ilm_lem_payload_parse(
      &payload,
      ilm_val
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_get_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ilm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_GET_VERIFY);

  res = pcp_frwrd_ilm_key_verify(
          unit,
          ilm_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_get_verify()", 0, 0);
}

/*********************************************************************
*     Gets the block of entries from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ilm_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT PCP_FRWRD_ILM_KEY                       *ilm_keys,
    SOC_SAND_OUT PCP_FRWRD_DECISION_INFO                 *ilm_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    num_scanned = 0;
  uint8
    is_found,
    is_valid;
  PCP_LEM_ACCESS_KEY
    key;
  PCP_LEM_ACCESS_PAYLOAD
    payload;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);
  SOC_SAND_CHECK_NULL_INPUT(ilm_keys);
  SOC_SAND_CHECK_NULL_INPUT(ilm_vals);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  *nof_entries = 0;

  PCP_LEM_ACCESS_KEY_clear(&key);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);

  /*
   *	Loop on the entry index
   */
  for ( num_scanned = 0;
        (num_scanned < block_range->entries_to_scan) &&
         (*nof_entries < block_range->entries_to_act) &&
         (block_range->iter < PCP_LEM_NOF_ENTRIES);
        ++block_range->iter, ++num_scanned)
  {
    PCP_LEM_ACCESS_KEY_clear(&key);

    res = pcp_lem_access_entry_by_index_get_unsafe(
            unit,
            block_range->iter,
            &key,
            &is_valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if ((is_valid == TRUE) && (key.type == PCP_LEM_ACCESS_KEY_TYPE_ILM))
    {
      pcp_frwrd_ilm_lem_key_parse(
        &key,
        &ilm_keys[*nof_entries]
      );

      res = pcp_lem_access_entry_by_key_get_unsafe(
              unit,
              &key,
              &payload,
              &is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      pcp_frwrd_ilm_lem_payload_parse(
        &payload,
        &ilm_vals[*nof_entries]
      );

      (*nof_entries)++;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_get_block_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ilm_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_GET_BLOCK_VERIFY);

  

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Remove incoming label key from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ilm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_REQUEST
    request;
  PCP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);

  PCP_LEM_ACCESS_REQUEST_clear(&request);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = PCP_LEM_ACCESS_CMD_DELETE;

  pcp_frwrd_ilm_lem_key_build(
    ilm_key,
    &request.key
    );

  res = pcp_lem_access_entry_remove_unsafe(
         unit,
         &request,
         &ack
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_remove_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ilm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_REMOVE_VERIFY);

  /* remove it if exists*/

  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_remove_verify()", 0, 0);
}

/*********************************************************************
*     Remove all keys from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ilm_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
    i;
  PCP_LEM_ACCESS_REQUEST
    request;
  PCP_LEM_ACCESS_ACK_STATUS
    ack;
  uint8
    is_valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_TABLE_CLEAR_UNSAFE);

  PCP_LEM_ACCESS_REQUEST_clear(&request);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = PCP_LEM_ACCESS_CMD_DELETE;

  /*
   *	Loop on the entry index
   */
  for ( i = 0; i < PCP_LEM_NOF_ENTRIES; ++i)
  {
    res = pcp_lem_access_entry_by_index_get_unsafe(
            unit,
            i,
            &request.key,
            &is_valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if ((is_valid == TRUE) && (request.key.type == PCP_LEM_ACCESS_KEY_TYPE_ILM))
    {
      res = pcp_lem_access_entry_remove_unsafe(
              unit,
              &request,
              &ack
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_table_clear_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ilm_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_ILM_TABLE_CLEAR_VERIFY);

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ilm_table_clear_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_api_frwrd_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_frwrd_ilm_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_frwrd_ilm;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_frwrd_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_frwrd_ilm_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_frwrd_ilm;
}

uint32
  PCP_FRWRD_ILM_KEY_verify(
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mapped_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_local_port, PCP_PORT_MAX, PCP_PORT_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->inrif, PCP_RIF_ID_MAX, PCP_RIF_ID_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_ILM_KEY_verify()",0,0);
}

uint32
  PCP_FRWRD_ILM_GLBL_KEY_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_ILM_GLBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_ILM_GLBL_KEY_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

