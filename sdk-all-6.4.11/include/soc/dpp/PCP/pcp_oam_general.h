/* $Id: pcp_oam_general.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PCP_OAM_GENERAL_INCLUDED__
/* { */
#define __SOC_PCP_OAM_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PCP/pcp_oam_api_general.h>
#include <soc/dpp/PCP/pcp_framework.h>

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
  PCP_OAM_GENERAL_INFO_SET = PCP_PROC_DESC_BASE_OAM_GENERAL_FIRST,
  PCP_OAM_GENERAL_INFO_SET_PRINT,
  PCP_OAM_GENERAL_INFO_SET_UNSAFE,
  PCP_OAM_GENERAL_INFO_SET_VERIFY,
  PCP_OAM_GENERAL_INFO_GET,
  PCP_OAM_GENERAL_INFO_GET_PRINT,
  PCP_OAM_GENERAL_INFO_GET_VERIFY,
  PCP_OAM_GENERAL_INFO_GET_UNSAFE,
  PCP_OAM_CALLBACK_FUNCTION_REGISTER,
  PCP_OAM_CALLBACK_FUNCTION_REGISTER_PRINT,
  PCP_OAM_CALLBACK_FUNCTION_REGISTER_UNSAFE,
  PCP_OAM_CALLBACK_FUNCTION_REGISTER_VERIFY,
  PCP_OAM_INTERRUPT_HANDLER,
  PCP_OAM_INTERRUPT_HANDLER_PRINT,
  PCP_OAM_INTERRUPT_HANDLER_UNSAFE,
  PCP_OAM_INTERRUPT_HANDLER_VERIFY,
  PCP_OAM_MSG_INFO_GET,
  PCP_OAM_MSG_INFO_GET_PRINT,
  PCP_OAM_MSG_INFO_GET_UNSAFE,
  PCP_OAM_MSG_INFO_GET_VERIFY,
  PCP_OAM_GENERAL_GET_PROCS_PTR,
  PCP_OAM_GENERAL_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_OAM_GENERAL_PROCEDURE_DESC_LAST
} PCP_OAM_GENERAL_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_OAM_GENERAL_EXCEPTION_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_OAM_GENERAL_FIRST,
  PCP_OAM_GENERAL_INFO1_OUT_OF_RANGE_ERR,
  PCP_OAM_GENERAL_INFO2_OUT_OF_RANGE_ERR,
  PCP_OAM_GENERAL_CPU_DST_SYS_PORT_OUT_OF_RANGE_ERR,
  PCP_OAM_GENERAL_CPU_TC_OUT_OF_RANGE_ERR,
  PCP_OAM_GENERAL_CPU_DP_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  PCP_OAM_GENERAL_MSG_FIFO_EMPTY_ERR,
  PCP_OAM_GENERAL_MSG_INFO_UNKNOWN_EXCEPTION_CODE_ERR,

  /*
   * Last element. Do no touch.
   */
  PCP_OAM_GENERAL_ERR_LAST
} PCP_OAM_GENERAL_ERR;

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
 *   pcp_oam_general_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This procedure applies general configuration of the
 *   OAMP.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_GENERAL_INFO            *info -
 *     General OAMP configuration
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_general_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO            *info
  );

uint32
  pcp_oam_general_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO            *info
  );

uint32
  pcp_oam_general_info_get_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_general_info_set_unsafe" API.
 *     Refer to "pcp_oam_general_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_general_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_GENERAL_INFO            *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_callback_function_register_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This procedure registers user callback that will be
 *   called as a response to a certain interrupt.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_USER_CALLBACK              *user_callback_struct -
 *     Structure holding information about the required
 *     interrupt callback.
 *   SOC_SAND_OUT uint32                        *callback_id -
 *     Callback ID returned from the driver. May later be used
 *     to unregister the callback.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_callback_function_register_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK              *user_callback_struct,
    SOC_SAND_OUT uint32                        *callback_id
  );

uint32
  pcp_oam_callback_function_register_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK              *user_callback_struct
  );

/*********************************************************************
* NAME:
 *   pcp_oam_interrupt_handler_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This procedure is called from the CPU ISR when interrupt
 *   handling for OAMP is required.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_interrupt_handler_unsafe(
    SOC_SAND_IN  int                       unit
  );

uint32
  pcp_oam_interrupt_handler_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
* NAME:
 *   pcp_oam_msg_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function reads and parses the head of the OAM
 *   message FIFO. If no message exists an indication is
 *   returned.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT PCP_OAM_MSG_INFO                *info -
 *     Exception message info.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_msg_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_MSG_INFO                *info
  );

uint32
  pcp_oam_msg_info_get_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
* NAME:
 *   pcp_oam_general_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_oam_api_general module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_oam_general_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_oam_general_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_oam_api_general module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_oam_general_get_errs_ptr(void);

uint32
  PCP_OAM_MSG_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MSG_INFO *info
  );

uint32
  PCP_OAM_GENERAL_INFO_verify(
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_OAM_GENERAL_INCLUDED__*/
#endif

