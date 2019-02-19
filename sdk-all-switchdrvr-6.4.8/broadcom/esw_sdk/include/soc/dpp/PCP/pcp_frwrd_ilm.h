/* $Id: pcp_frwrd_ilm.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PCP_FRWRD_ILM_INCLUDED__
/* { */
#define __SOC_PCP_FRWRD_ILM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PCP/pcp_api_frwrd_ilm.h>
#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_lem_access.h>

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

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_FRWRD_ILM_GLBL_INFO_SET = PCP_PROC_DESC_BASE_FRWRD_ILM_FIRST,
  PCP_FRWRD_ILM_GLBL_INFO_SET_PRINT,
  PCP_FRWRD_ILM_GLBL_INFO_SET_UNSAFE,
  PCP_FRWRD_ILM_GLBL_INFO_SET_VERIFY,
  PCP_FRWRD_ILM_GLBL_INFO_GET,
  PCP_FRWRD_ILM_GLBL_INFO_GET_PRINT,
  PCP_FRWRD_ILM_GLBL_INFO_GET_VERIFY,
  PCP_FRWRD_ILM_GLBL_INFO_GET_UNSAFE,
  PCP_FRWRD_ILM_ADD,
  PCP_FRWRD_ILM_ADD_PRINT,
  PCP_FRWRD_ILM_ADD_UNSAFE,
  PCP_FRWRD_ILM_ADD_VERIFY,
  PCP_FRWRD_ILM_GET,
  PCP_FRWRD_ILM_GET_PRINT,
  PCP_FRWRD_ILM_GET_UNSAFE,
  PCP_FRWRD_ILM_GET_VERIFY,
  PCP_FRWRD_ILM_GET_BLOCK,
  PCP_FRWRD_ILM_GET_BLOCK_PRINT,
  PCP_FRWRD_ILM_GET_BLOCK_UNSAFE,
  PCP_FRWRD_ILM_GET_BLOCK_VERIFY,
  PCP_FRWRD_ILM_REMOVE,
  PCP_FRWRD_ILM_REMOVE_PRINT,
  PCP_FRWRD_ILM_REMOVE_UNSAFE,
  PCP_FRWRD_ILM_REMOVE_VERIFY,
  PCP_FRWRD_ILM_TABLE_CLEAR,
  PCP_FRWRD_ILM_TABLE_CLEAR_PRINT,
  PCP_FRWRD_ILM_TABLE_CLEAR_UNSAFE,
  PCP_FRWRD_ILM_TABLE_CLEAR_VERIFY,
  PCP_FRWRD_ILM_GET_PROCS_PTR,
  PCP_FRWRD_ILM_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  PCP_FRWRD_ILM_PROCEDURE_DESC_LAST
} PCP_FRWRD_ILM_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_FRWRD_ILM_SUCCESS_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_FRWRD_ILM_FIRST,
  /*
   * } Auto generated. Do not edit previous section.
   */

   PCP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR,
   PCP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR,
   PCP_FRWRD_ILM_EEI_NOT_MPLS_ERR,
   PCP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR,
   PCP_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR,

  /*
   * Last element. Do no touch.
   */
  PCP_FRWRD_ILM_ERR_LAST
} PCP_FRWRD_ILM_ERR;

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

/*********************************************************************
* NAME:
 *   pcp_frwrd_ilm_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add mapping from incoming label to destination and MPLS
 *   command.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key -
 *     Key to perform the forwarding lookup. May include
 *     in-label, EXP and in-port.
 *   SOC_SAND_IN  PCP_FRWRD_DECISION_INFO                 *ilm_val -
 *     ILM Forwading decision. The destination to forward the
 *     packet to and the MPLS label command to perform. -
 *     Destination must be FEC pointer, or MC destination;
 *     error is returned otherwise.- EEI is MPLS command. MPLS
 *     command must be Pop (PHP application), or swap (LSR
 *     application), error will be returned if command is of
 *     type Push.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the ILM DB (LEM).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_ilm_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  pcp_frwrd_ilm_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO                 *ilm_val
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_ilm_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the value (destination and MPLS command) the
 *   incoming label key is associated with.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key -
 *     Key to perform the forwarding lookup. May include
 *     in-label, EXP and in-port.
 *   SOC_SAND_OUT PCP_FRWRD_DECISION_INFO                 *ilm_val -
 *     The destination to forward the packet to and the MPLS
 *     label command to perform. Valid only if found is TRUE.
 *   SOC_SAND_OUT uint8                                 *found -
 *     Does the ilm_key exist in the ILM DB. If TRUE, then
 *     ilm_val is valid.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_ilm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_OUT PCP_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  pcp_frwrd_ilm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_ilm_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the block of entries from the ILM DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Defines block in the ILM database.
 *   SOC_SAND_OUT PCP_FRWRD_ILM_KEY                       *ilm_keys -
 *     Array to include ILM keys.
 *   SOC_SAND_OUT PCP_FRWRD_DECISION_INFO                 *ilm_vals -
 *     Array to include ILM values.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries in ilm_key and ilm_val.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_ilm_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT PCP_FRWRD_ILM_KEY                       *ilm_keys,
    SOC_SAND_OUT PCP_FRWRD_DECISION_INFO                 *ilm_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  pcp_frwrd_ilm_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_ilm_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove incoming label key from the ILM DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key -
 *     ILM key. May include in-label, EXP and in-port.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_ilm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key
  );

uint32
  pcp_frwrd_ilm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY                       *ilm_key
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_ilm_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove all keys from the ILM DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_ilm_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  pcp_frwrd_ilm_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );

/* parse lem access key for IpV4 host address
*/
  void
    pcp_frwrd_ilm_lem_key_parse(
      SOC_SAND_IN PCP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT PCP_FRWRD_ILM_KEY                       *ilm_key
    );

  void
    pcp_frwrd_ilm_lem_payload_parse(
      SOC_SAND_IN PCP_LEM_ACCESS_PAYLOAD *payload,
      SOC_SAND_OUT  PCP_FRWRD_DECISION_INFO                 *ilm_val
    );

void
    pcp_frwrd_ilm_lem_key_build(
      SOC_SAND_IN  PCP_FRWRD_ILM_KEY  *ilm_key,
      SOC_SAND_OUT PCP_LEM_ACCESS_KEY *key
    );

/*********************************************************************
* NAME:
 *   pcp_frwrd_ilm_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_api_frwrd_ilm module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_frwrd_ilm_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_frwrd_ilm_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_api_frwrd_ilm module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_frwrd_ilm_get_errs_ptr(void);

uint32
  PCP_FRWRD_ILM_KEY_verify(
    SOC_SAND_IN  PCP_FRWRD_ILM_KEY *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_FRWRD_ILM_INCLUDED__*/
#endif

