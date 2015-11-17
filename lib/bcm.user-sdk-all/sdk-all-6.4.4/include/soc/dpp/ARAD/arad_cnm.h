/*
 * $Id: arad_cnm.h,v 1.2 Broadcom SDK $
 *
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
 *
 */

#ifndef __ARAD_CNM_INCLUDED__
/* { */
#define __ARAD_CNM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_cnm.h>
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
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
 *   arad_cnm_cp_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Congestion Point functionality, in particular
 *   the packet generation mode and the fields of the CNM
 *   packet.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNM_CP_INFO                *info -
 *     Attributes of the congestion point functionality
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnm_cp_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_CNM_CP_INFO                *info
  );

uint32
  arad_cnm_cp_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT ARAD_CNM_CP_INFO                *info
  );

uint32
  arad_cnm_cp_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_CNM_CP_INFO                *info
  );


/*********************************************************************
* NAME:
 *   arad_cnm_queue_mapping_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mapping from the pair (destination, traffic
 *   class) to the CP Queue.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNM_Q_MAPPING_INFO                  *info -
 *     Attributes of the CP Queue mapping
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnm_queue_mapping_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_Q_MAPPING_INFO             *info
  );

uint32
  arad_cnm_queue_mapping_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT ARAD_CNM_Q_MAPPING_INFO             *info
  );

uint32
  arad_cnm_queue_mapping_set_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_Q_MAPPING_INFO             *info
  );


/*********************************************************************
* NAME:
 *   arad_cnm_cpq_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the CP Queue attributes into an entry of the
 *   Congestion Point Table.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              cpq_ndx -
 *     Index of the CP Queue. Range: 0 - 4K-1.
 *   SOC_SAND_IN  ARAD_CNM_CPQ_INFO                        *info -
 *     Attributes of the congestion point profile entry
 * REMARKS:
 *   Relevant only if not in a sampling mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnm_cpq_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_IN  ARAD_CNM_CPQ_INFO                   *info
  );

uint32
  arad_cnm_cpq_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_OUT ARAD_CNM_CPQ_INFO                   *info
  );

uint32
  arad_cnm_cpq_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_IN  ARAD_CNM_CPQ_INFO                   *info
  );

/*********************************************************************
* NAME:
 *   arad_cnm_cp_profile_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure an entry into the Congestion Point Profile
 *   Table.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   profile_ndx -
 *     Index of the profile in the Congestion Point profile
 *     table. Range: 0 - 7.
 *   SOC_SAND_IN  ARAD_CNM_CP_PROFILE_INFO      *info -
 *     Attributes of the congestion point profile entry.
 * REMARKS:
 *   Relevant only if not in a sampling mode (set in
 *   cnm_cp_set API). The terminology used in this API
 *   corresponds to the 802.1 Qau standard of Congestion
 *   Notification.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnm_cp_profile_set_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         profile_ndx,
    SOC_SAND_IN  ARAD_CNM_CP_PROFILE_INFO       *info
  );

uint32
  arad_cnm_cp_profile_get_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         profile_ndx,
    SOC_SAND_OUT ARAD_CNM_CP_PROFILE_INFO       *info
  );

uint32
  arad_cnm_cp_profile_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         profile_ndx,
    SOC_SAND_IN  ARAD_CNM_CP_PROFILE_INFO       *info
  );

/*********************************************************************
* NAME:
 *   arad_cnm_cp_packet_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the CNM packet properties (such as FTMH, MAC SA, etc)
 *   Table.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNM_CP_PACKET_INFO      *info -
 *     Attributes of the congestion point profile entry.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnm_cp_packet_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_CNM_CP_PACKET_INFO       *info
  );

uint32
  arad_cnm_cp_packet_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT ARAD_CNM_CP_PACKET_INFO       *info
  );

/*********************************************************************
* NAME:
 *   arad_cnm_cp_options_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Congestion Point generation options.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNM_CP_OPTIONS                *info -
 *     Options for the congestion point functionality
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnm_cp_options_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_CP_OPTIONS                 *info
  );

uint32
  arad_cnm_cp_options_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT ARAD_CNM_CP_OPTIONS                 *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_CNT_INCLUDED__*/
#endif
