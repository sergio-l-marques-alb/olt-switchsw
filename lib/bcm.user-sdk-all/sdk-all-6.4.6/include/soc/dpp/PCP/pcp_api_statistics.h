/* $Id: pcp_api_statistics.h,v 1.3 Broadcom SDK $
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

#ifndef __SOC_PCP_API_STATISTICS_INCLUDED__
/* { */
#define __SOC_PCP_API_STATISTICS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/PCP/pcp_oam_general.h>

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

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Ingress bytes counter
   */
  SOC_SAND_U64 bytes;
  /*
   *  Ingress packets counter
   */
  SOC_SAND_U64 packets;

} PCP_STS_ING_COUNTER_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Egress packets counter
   */
  SOC_SAND_U64 packets;

} PCP_STS_EGR_COUNTER_INFO;


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
 *   pcp_sts_ing_counter_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This API retrieves statistics on the specified ingress
 *   counter index
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access
 *   SOC_SAND_IN  uint32                        counter_ndx -
 *     Selects the counter on which statistics should be
 *     retrieved
 *   SOC_SAND_OUT PCP_STS_ING_COUNTER_INFO        *value -
 *     The data of the counter
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_sts_ing_counter_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        counter_ndx,
    SOC_SAND_OUT PCP_STS_ING_COUNTER_INFO        *value
  );

/*********************************************************************
* NAME:
 *   pcp_sts_egr_counter_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This API retrieves statistics on the specified egress
 *   counter index
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access
 *   SOC_SAND_IN  uint32                        counter_ndx -
 *     Selects the counter on which statistics should be
 *     retrieved
 *   SOC_SAND_OUT PCP_STS_EGR_COUNTER_INFO        *value -
 *     The data of the counter
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_sts_egr_counter_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        counter_ndx,
    SOC_SAND_OUT PCP_STS_EGR_COUNTER_INFO        *value
  );

void
  PCP_STS_ING_COUNTER_INFO_clear(
    SOC_SAND_OUT PCP_STS_ING_COUNTER_INFO *info
  );

void
  PCP_STS_EGR_COUNTER_INFO_clear(
    SOC_SAND_OUT PCP_STS_EGR_COUNTER_INFO *info
  );

#if PCP_DEBUG_IS_LVL1

void
  PCP_STS_ING_COUNTER_INFO_print(
    SOC_SAND_IN  PCP_STS_ING_COUNTER_INFO *info
  );

void
  PCP_STS_EGR_COUNTER_INFO_print(
    SOC_SAND_IN  PCP_STS_EGR_COUNTER_INFO *info
  );

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_API_STATISTICS_INCLUDED__*/
#endif

