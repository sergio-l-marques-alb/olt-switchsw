/* $Id: arad_pp_api_ptp.h,v 1.2 Broadcom SDK $
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

#ifndef __ARAD_PP_API_PTP_INCLUDED__
/* { */
#define __ARAD_PP_API_PTP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#include <soc/dpp/PPC/ppc_api_ptp.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_PTP_ACTION_FIELD_ACTION_INDEX_TRAP     SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_TRAP
#define ARAD_PP_PTP_ACTION_FIELD_ACTION_INDEX_DROP     SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_DROP
#define ARAD_PP_PTP_ACTION_FIELD_ACTION_INDEX_FWD      SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_FWD

#define ARAD_PP_PTP_PORT_TIMESYNC_ONE_STEP_TIMESTAMP SOC_PPC_PTP_PORT_TIMESYNC_ONE_STEP_TIMESTAMP
#define ARAD_PP_PTP_PORT_TIMESYNC_TWO_STEP_TIMESTAMP SOC_PPC_PTP_PORT_TIMESYNC_TWO_STEP_TIMESTAMP

#define ARAD_PP_PTP_IN_PP_PORT_PROFILE_NONE       SOC_PPC_PTP_IN_PP_PORT_PROFILE_NONE
#define ARAD_PP_PTP_IN_PP_PORT_PROFILE_0          SOC_PPC_PTP_IN_PP_PORT_PROFILE_0
#define ARAD_PP_PTP_IN_PP_PORT_PROFILE_1          SOC_PPC_PTP_IN_PP_PORT_PROFILE_1
#define ARAD_PP_PTP_IN_PP_PORT_PROFILE_2          SOC_PPC_PTP_IN_PP_PORT_PROFILE_2
#define ARAD_PP_PTP_IN_PP_PORT_PROFILE_3          SOC_PPC_PTP_IN_PP_PORT_PROFILE_3
#define ARAD_PP_PTP_IN_PP_PORT_PROFILES_NUM       SOC_PPC_PTP_IN_PP_PORT_PROFILES_NUM
typedef SOC_PPC_PTP_IN_PP_PORT_PROFILE            ARAD_PP_PTP_IN_PP_PORT_PROFILE;

#define ARAD_PP_PTP_1588_PKT_SYNC                 SOC_PPC_PTP_1588_PKT_SYNC
#define ARAD_PP_PTP_1588_PKT_DELAY_REQ            SOC_PPC_PTP_1588_PKT_DELAY_REQ
#define ARAD_PP_PTP_1588_PKT_PDELAY_REQ           SOC_PPC_PTP_1588_PKT_PDELAY_REQ
#define ARAD_PP_PTP_1588_PKT_PDELAY_RESP          SOC_PPC_PTP_1588_PKT_PDELAY_RESP
#define ARAD_PP_PTP_1588_PKT_FOLLOWUP             SOC_PPC_PTP_1588_PKT_FOLLOWUP
#define ARAD_PP_PTP_1588_PKT_DELAY_RESP           SOC_PPC_PTP_1588_PKT_DELAY_RESP
#define ARAD_PP_PTP_1588_PKT_PDELAY_RESP_FOLLOWUP SOC_PPC_PTP_1588_PKT_PDELAY_RESP_FOLLOWUP
#define ARAD_PP_PTP_1588_PKT_ANNOUNCE             SOC_PPC_PTP_1588_PKT_ANNOUNCE
#define ARAD_PP_PTP_1588_PKT_SIGNALLING           SOC_PPC_PTP_1588_PKT_SIGNALLING
#define ARAD_PP_PTP_1588_PKT_MANAGMENT            SOC_PPC_PTP_1588_PKT_MANAGMENT

typedef SOC_PPC_PTP_PORT_INFO  ARAD_PP_PTP_PORT_INFO;

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
 *   arad_pp_ptp_init
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure ARAD device to support PTP (1588) protocol.
 *   initialize 1588 general(not per port) registers & tables .
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_ptp_init(
    SOC_SAND_IN  int                                     unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_ptp_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   disable/enable 1588 per port,
 *   initialize 1588 general per port registers & tables .
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                local_port_ndx -
 *     pp port 
 *   SOC_SAND_IN  ARAD_PP_PTP_PORT_INFO                 *info -
 *     PTP (1588) port info
 *   SOC_SAND_IN  ARAD_PP_PTP_IN_PP_PORT_PROFILE        profile -
 *     the PTP (1588) profile index to set to port to.
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_ptp_port_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_IN  ARAD_PP_PTP_PORT_INFO                  *info,
    SOC_SAND_IN  ARAD_PP_PTP_IN_PP_PORT_PROFILE         profile
  );

/*********************************************************************
* NAME:
 *   arad_pp_ptp_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   get 1588 status (enabled/disabled) per port
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                local_port_ndx -
 *     pp port 
 *   SOC_SAND_OUT ARAD_PP_PTP_PORT_INFO                 *profile -
 *     the PTP (1588) profile index of the port.
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_ptp_port_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_OUT ARAD_PP_PTP_IN_PP_PORT_PROFILE         *profile
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_LAG_INCLUDED__*/
#endif


