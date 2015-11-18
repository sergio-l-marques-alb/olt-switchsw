/* $Id: ppc_api_ptp.h,v 1.2 Broadcom SDK $
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


#ifndef __SOC_PPC_API_PTP_INCLUDED__
/* { */
#define __SOC_PPC_API_PTP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

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

enum {

	SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_TRAP  = 0,
	SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_DROP  = 1,
	SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_FWD   = 2
};

typedef enum
{
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_0    = 0,
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_1    = 1,
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_2    = 2,
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_3    = 3,

  SOC_PPC_PTP_IN_PP_PORT_PROFILES_NUM

}SOC_PPC_PTP_IN_PP_PORT_PROFILE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  wehter PTP (1588) is enabled in the port or not
   *    0 - disabled
   *    1 - enabled
   */
  uint8 ptp_enabled;

#define SOC_PPC_PTP_PORT_TIMESYNC_ONE_STEP_TIMESTAMP 0x00000004 /* Port one step timestamp configuration */
#define SOC_PPC_PTP_PORT_TIMESYNC_TWO_STEP_TIMESTAMP 0x00000008 /* Port two step timestamp configuration */
  /*
   *  flags indicating wether 1/2 step clock are enabled.
   */
    uint32 flags;

#define SOC_PPC_PTP_1588_PKT_SYNC                 0x0 /* Sync Event Packet type */
#define SOC_PPC_PTP_1588_PKT_DELAY_REQ            0x1 /* Delay Request Event Packet type */
#define SOC_PPC_PTP_1588_PKT_PDELAY_REQ           0x2 /* Pdelay Request Event Packet type */
#define SOC_PPC_PTP_1588_PKT_PDELAY_RESP          0x3 /* Pdelay Response Event Packet type */
#define SOC_PPC_PTP_1588_PKT_FOLLOWUP             0x8 /* Followup General Packet type */
                                                  /* 4-7 reserved */
#define SOC_PPC_PTP_1588_PKT_DELAY_RESP           0x9 /* Delay_resp General Packet type */
#define SOC_PPC_PTP_1588_PKT_PDELAY_RESP_FOLLOWUP 0xa /* Pdelay_resp_followup General Packet type */
#define SOC_PPC_PTP_1588_PKT_ANNOUNCE             0xb /* Announce General Packet type */
#define SOC_PPC_PTP_1588_PKT_SIGNALLING           0xc /* Signalling General Packet type */
#define SOC_PPC_PTP_1588_PKT_MANAGMENT            0xd /* Management General Packet type */
                                                  /* e-f reserved */
  /*
   * bitmap of 1588 event and general packet types above,
   * indicating wether to :
   *   1. forward (drop-0,tocpu-0) 
   *   2. trap/snoop(drop-0,tocpu-1)
   *   3. drop(drop-1,tocpu-0) 
   * the packet.
   */
  uint32 pkt_drop;
  uint32 pkt_tocpu;

} SOC_PPC_PTP_PORT_INFO;

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

void
  SOC_PPC_PTP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_PTP_PORT_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_to_string(
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE enum_val
  );

void
  SOC_PPC_PTP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO *info
  );

#endif /* SOC_PPC_DEBUG_IS_LVL1 */
/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_API_OAM_INCLUDED__*/
#endif
