/* $Id: pb_api_egr_queuing.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_api_egr_queuing.h
*
* MODULE PREFIX:  soc_pb_egr
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

#ifndef __SOC_PB_API_EGR_QUEUING_INCLUDED__
/* { */
#define __SOC_PB_API_EGR_QUEUING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/Petra/petra_api_egr_queuing.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
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

typedef SOC_TMC_EGR_Q_PRIORITY                               SOC_PB_EGR_Q_PRIORITY;

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
 *   soc_pb_egr_q_nif_cal_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets egress queue calendar which control the service
 *   times of the nif ports.
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_q_nif_cal_set(
    SOC_SAND_IN  int                     unit
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_egr_q_nif_cal_set"
 *     API.
 *     Refer to "soc_pb_egr_q_nif_cal_set" API for details.
*********************************************************************/
uint32
  soc_pb_egr_q_nif_cal_get(
    SOC_SAND_IN  int                     unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_q_prio_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets egress queue priority per traffic class and drop
 *   precedence.
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx -
 *     mapping type, defining what traffic type
 *     (unicast/multicast) is mapped to what egress queue type
 *     (scheduled/unscheduled).
 *   SOC_SAND_IN  uint32                     tc_ndx -
 *     Traffic Class, as embedded in the packet header. Range:
 *     0 - 7.
 *   SOC_SAND_IN  uint32                     dp_ndx -
 *     Drop Precedence, as embedded in the packet header.
 *     Range: 0 - 3.
 *   SOC_SAND_IN  uint32                     map_profile_ndx -
 *     Mapping profile index. Range: 0 - 3.
 *   SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY             *priority -
 *     Egress Queuing priority (Drop precedence and Traffic
 *     class).
 * REMARKS:
 *   Each OFP is mapped to an egress queue profile by the
 *   soc_pb_egr_q_profile_map_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_q_prio_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY             *priority
  );

int
  soc_pb_egr_q_prio_set_dispatch(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE  map_type_ndx,
    SOC_SAND_IN  uint32                             tc_ndx,
    SOC_SAND_IN  uint32                             dp_ndx,
    SOC_SAND_IN  uint32                             map_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY              *priority
  );
/*********************************************************************
*     Gets the configuration set by the "soc_pb_egr_q_prio_set"
 *     API.
 *     Refer to "soc_pb_egr_q_prio_set" API for details.
*********************************************************************/
uint32
  soc_pb_egr_q_prio_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_OUT SOC_PB_EGR_Q_PRIORITY             *priority
  );

int
  soc_pb_egr_q_prio_get_dispatch(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE  map_type_ndx,
    SOC_SAND_IN  uint32                             tc_ndx,
    SOC_SAND_IN  uint32                             dp_ndx,
    SOC_SAND_IN  uint32                             map_profile_ndx,
    SOC_SAND_OUT SOC_PB_EGR_Q_PRIORITY              *priority
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_q_profile_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Function description
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx -
 *     Outgoing Fap Port index. Range: 0 - 79.
 *   SOC_SAND_IN  uint32                      map_profile_id -
 *     Egress queue priority profile index. Range: 0 - 3.
 * REMARKS:
 *   The profile configuration is set by the
 *   soc_pb_egr_q_prio_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_q_profile_map_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_IN  uint32                      map_profile_id
  );

int
  soc_pb_egr_q_profile_map_set_dispatch(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core_id,
    SOC_SAND_IN     uint32                tm_port,
    SOC_SAND_IN     uint32                map_profile_id
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_egr_q_profile_map_set" API.
 *     Refer to "soc_pb_egr_q_profile_map_set" API for details.
*********************************************************************/
uint32
  soc_pb_egr_q_profile_map_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_OUT uint32                      *map_profile_id
  );

int
  soc_pb_egr_q_profile_map_get_dispatch(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     uint32                tm_port,
    SOC_SAND_OUT    uint32                *map_profile_id
  );

void
  SOC_PB_EGR_Q_PRIORITY_clear(
    SOC_SAND_OUT SOC_PB_EGR_Q_PRIORITY *info
  );

#if SOC_PB_DEBUG_IS_LVL1

void
  SOC_PB_EGR_Q_PRIORITY_print(
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY *info
  );
  
void
  soc_pb_egr_q_nif_cal_get_and_print(
    SOC_SAND_IN int unit
 );
 
#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_EGR_QUEUING_INCLUDED__*/
#endif

