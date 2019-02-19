/* $Id$
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


#ifndef __SOC_TMC_API_DEBUG_INCLUDED__
/* { */
#define __SOC_TMC_API_DEBUG_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>
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
   *  Traffic route not forced.
   */
  SOC_TMC_DBG_FORCE_MODE_NONE=0,
  /*
   *  Traffic route forced to the local route (never routed
   *  through the fabric).
   */
  SOC_TMC_DBG_FORCE_MODE_LOCAL=1,
  /*
   *  Traffic route forced to the fabric route (always routed
   *  through the fabric).
   */
  SOC_TMC_DBG_FORCE_MODE_FABRIC=2,
  /*
   *  Total number of traffic routing force modes.
   */
  SOC_TMC_DBG_NOF_FORCE_MODES=3
}SOC_TMC_DBG_FORCE_MODE;

typedef enum
{
  /*
   *  The packets in the queue are dequeued and go to their
   *  destination.
   */
  SOC_TMC_DBG_FLUSH_MODE_DEQUEUE=0,
  /*
   *  The packets in the queue are deleted.
   */
  SOC_TMC_DBG_FLUSH_MODE_DELETE=1,
  /*
   *  Total number of flushing modes.
   */
  SOC_TMC_DBG_NOF_FLUSH_MODES=2
}SOC_TMC_DBG_FLUSH_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The index of the first queue for the generation of auto
   *  credits. Range: 0 - 32767. Note: if last_queue <
   *  first_queue, then all the queues are selected.
   */
  uint32 first_queue;
  /*
   *  The index of the last queue for the generation of auto
   *  credits. Range: 0 - 32767. Note: if last_queue <
   *  first_queue, then all the queues are selected.
   */
  uint32 last_queue;
  /*
   *  The rate for the generation of auto credits. Units: Mbps.
   */
  uint32 rate;
}SOC_TMC_DBG_AUTOCREDIT_INFO;

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
  SOC_TMC_DBG_AUTOCREDIT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_DBG_AUTOCREDIT_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_DBG_FORCE_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DBG_FORCE_MODE enum_val
  );

const char*
  SOC_TMC_DBG_FLUSH_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_DBG_FLUSH_MODE enum_val
  );

void
  SOC_TMC_DBG_AUTOCREDIT_INFO_print(
    SOC_SAND_IN SOC_TMC_DBG_AUTOCREDIT_INFO *info
  );

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_TMC_API_DEBUG_INCLUDED__*/
#endif
