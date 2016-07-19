/* $Id: pcp_oam_api_general.h,v 1.3 Broadcom SDK $
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

#ifndef __SOC_PCP_OAM_API_GENERAL_INCLUDED__
/* { */
#define __SOC_PCP_OAM_API_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_DEBUG                               (SOC_SAND_DEBUG)

#define PCP_ECI_ID                             (0)
#define PCP_ELK_ID                             (1)
#define PCP_OAM_ID                             (2)
#define PCP_STS_ID                             (3)
#define PCP_NOF_MODULES                        (4)

#define PCP_DEBUG_IS_LVL1                       (PCP_DEBUG >= SOC_SAND_DBG_LVL1)

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
   *  CCM session (identified by local MEP handle and remote
   *  MEP ID) has timed out. info1 returns the ID of the remote MEP.
   *  info2 returns the ID of the remote MEP.
   */
  PCP_OAM_MSG_CCM_TIMEOUT = 0,
  /*
   *  CCM session (identified by local MEP handle and remote
   *  MEP ID) exit timed out. info1 returns the ID of the remote MEP.
   *  info2 returns the ID of the remote MEP.
   */
  PCP_OAM_MSG_CCM_EXIT_TIMEOUT = 1,
  /*
   *  BFD session has timed out. info1 returns
   *  My-discriminator of the timed-out session. info2 is not
   *  used.
   */
  PCP_OAM_MSG_BFD_TIMEOUT = 2,
  /*
   *  Monitored LSP has timed out. info1 returns the label of
   *  the timed-out LSPInfo2 returns: '0' if monitored LSP is
   *  PWE'1' if monitored LSP is Tunnel
   */
  PCP_OAM_MSG_MPLS_DLOCKV_SET = 3,
  /*
   *  Monitored LSP has recovered. info1 returns the label of
   *  the recovered LSPInfo2 returns: '0' if monitored LSP is
   *  PWE'1' if monitored LSP is Tunnel
   */
  PCP_OAM_MSG_MPLS_DLOCKV_CLEAR = 4,
  /*
   *  FDI state of the monitored LSP has been cleared (FDI
   *  messages ceased arriving). info1 returns the label of
   *  the monitored LSPInfo2 returns: '0' if monitored LSP is
   *  PWE'1' if monitored LSP is Tunnel
   */
  PCP_OAM_MSG_MPLS_FDI_CLEAR = 5,
  /*
   *  BDI state of the monitored LSP has been cleared (BDI
   *  messages ceased arriving). info1 returns the label of
   *  the monitored LSPInfo2 returns: '0' if monitored LSP is
   *  PWE'1' if monitored LSP is Tunnel
   */
  PCP_OAM_MSG_MPLS_BDI_CLEAR = 6,
  /*
   *  Monitored LSP has received dExcess message. info1
   *  returns the label of the timed-out LSPInfo2 returns: '0'
   *  if monitored LSP is PWE'1' if monitored LSP is Tunnel
   */
  PCP_OAM_MSG_MPLS_DEXCESS_SET = 7,
  /*
   *  Monitored LSP has recovered. info1 returns the label of
   *  the recovered LSPInfo2 returns: '0' if monitored LSP is
   *  PWE'1' if monitored LSP is Tunnel
   */
  PCP_OAM_MSG_MPLS_DEXCESS_CLEAR = 8,
  /*
   *  The message type arrived is unidentified
   */
  PCP_OAM_MSG_UNKNOWN = 9,

  PCP_OAM_MSG_NONE = 255,
  /*
   *  Number of types in PCP_OAM_MSG_EXCEPTION_TYPE
   */
  PCP_OAM_NOF_MSG_EXCEPTION_TYPES = 11
}PCP_OAM_MSG_EXCEPTION_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Monitored entity type
   */
  PCP_OAM_MSG_EXCEPTION_TYPE exception;
  /*
   *  The index where the exception occurred. Interpretation
   *  is defined in Error! Reference source not found.
   */
  uint32 info1;
  /*
   *  The index where the exception occurred. Interpretation
   *  is defined in Error! Reference source not found.
   */
  uint32 info2;

} PCP_OAM_MSG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  CPU port. Used to construct ITMH when trapping packets
   *  to CPU from the OAMP.
   */
  uint32 cpu_dst_sys_port;
  /*
   *  Traffic class. Used to construct ITMH when trapping
   *  packets to CPU from the OAMP.
   */
  uint32 cpu_tc;
  /*
   *  Drop precedence. Used to construct ITMH when trapping
   *  packets to CPU from the OAMP.
   */
  uint32 cpu_dp;

} PCP_OAM_GENERAL_INFO;


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
 *   pcp_oam_general_info_set
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
  pcp_oam_general_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO            *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_general_info_set" API.
 *     Refer to "pcp_oam_general_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_general_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_GENERAL_INFO            *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_callback_function_register
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
  pcp_oam_callback_function_register(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK              *user_callback_struct,
    SOC_SAND_OUT uint32                        *callback_id
  );

/*********************************************************************
* NAME:
 *   pcp_oam_interrupt_handler
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
  pcp_oam_interrupt_handler(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
* NAME:
 *   pcp_oam_msg_info_get
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
  pcp_oam_msg_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_MSG_INFO                *info
  );

void
  PCP_OAM_MSG_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MSG_INFO *info
  );

void
  PCP_OAM_GENERAL_INFO_clear(
    SOC_SAND_OUT PCP_OAM_GENERAL_INFO *info
  );

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_OAM_MSG_EXCEPTION_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_MSG_EXCEPTION_TYPE enum_val
  );

void
  PCP_OAM_MSG_INFO_print(
    SOC_SAND_IN  PCP_OAM_MSG_INFO *info
  );

void
  PCP_OAM_GENERAL_INFO_print(
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO *info
  );

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_OAM_API_GENERAL_INCLUDED__*/
#endif

