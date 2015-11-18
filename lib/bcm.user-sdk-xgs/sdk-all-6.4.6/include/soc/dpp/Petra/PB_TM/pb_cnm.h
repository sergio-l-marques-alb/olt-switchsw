/* $Id: pb_cnm.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_cnm.h
*
* MODULE PREFIX:  soc_pb_cnm
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

#ifndef __SOC_PB_CNM_INCLUDED__
/* { */
#define __SOC_PB_CNM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_cnm.h>

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
 *   soc_pb_cnm_cp_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Congestion Point functionality, in particular
 *   the packet generation mode and the fields of the CNM
 *   packet.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNM_CP_INFO              *info -
 *     Attributes of the congestion point functionality
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_cp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO              *info
  );

uint32
  soc_pb_cnm_cp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO              *info
  );

uint32
  soc_pb_cnm_cp_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_cnm_cp_set_unsafe"
 *     API.
 *     Refer to "soc_pb_cnm_cp_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_cp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_CP_INFO              *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_q_mapping_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mapping from the pair (destination, traffic
 *   class) to the CP Queue.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO       *info -
 *     Attributes of the CP Queue mapping
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_q_mapping_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO       *info
  );

uint32
  soc_pb_cnm_q_mapping_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO       *info
  );

uint32
  soc_pb_cnm_q_mapping_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_q_mapping_set_unsafe" API.
 *     Refer to "soc_pb_cnm_q_mapping_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_q_mapping_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_Q_MAPPING_INFO       *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_congestion_test_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set parameters of the congestion test.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info -
 *     Attributes of the congestion test functionality
 * REMARKS:
 *   If not set, the default values of the different
 *   configurations are False.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_congestion_test_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  );

uint32
  soc_pb_cnm_congestion_test_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  );

uint32
  soc_pb_cnm_congestion_test_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_congestion_test_set_unsafe" API.
 *     Refer to "soc_pb_cnm_congestion_test_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_cnm_congestion_test_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_CONGESTION_TEST_INFO *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_cp_profile_set_unsafe
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
 *   SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO      *info -
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
  soc_pb_cnm_cp_profile_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO      *info
  );

uint32
  soc_pb_cnm_cp_profile_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO      *info
  );

uint32
  soc_pb_cnm_cp_profile_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_cp_profile_set_unsafe" API.
 *     Refer to "soc_pb_cnm_cp_profile_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_cp_profile_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CP_PROFILE_INFO      *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_sampling_profile_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure an entry into the Congestion Point Profile
 *   Table for the sampling mode.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   profile_ndx -
 *     Index of the profile in the Congestion Point profile
 *     table. Range: 0 - 7.
 *   SOC_SAND_IN  uint32                    sampling_rate -
 *     The minimum number of enqueued bytes between two
 *     generations of a CNM packet and used as the sampling
 *     rate. Must be a multiple of 64 bytes. Default value:
 *     150,000 Bytes. Units: Bytes. Range: 0 - (8K-1)*64.
 * REMARKS:
 *   Relevant only in a sampling mode(set in cnm_cp_set API).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_sampling_profile_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  uint32                    sampling_rate
  );

uint32
  soc_pb_cnm_sampling_profile_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  uint32                    sampling_rate
  );

uint32
  soc_pb_cnm_sampling_profile_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_sampling_profile_set_unsafe" API.
 *     Refer to "soc_pb_cnm_sampling_profile_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_cnm_sampling_profile_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_OUT uint32                    *sampling_rate
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_cpq_pp_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the CP Queue attributes into an entry of the
 *   Congestion Point Table.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   cpq_ndx -
 *     Index of the CP Queue. Range: 0 - 4K-1.
 *   SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info -
 *     Attributes of the congestion point profile entry
 * REMARKS:
 *   Relevant only if not in a sampling mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_cpq_pp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  );

uint32
  soc_pb_cnm_cpq_pp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  );

uint32
  soc_pb_cnm_cpq_pp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_cpq_pp_set_unsafe" API.
 *     Refer to "soc_pb_cnm_cpq_pp_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_cpq_pp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO             *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_cpq_sampling_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the CP Queue attributes into an entry of the
 *   Congestion Point Table.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   cpq_ndx -
 *     Index of the CP Queue. Range: 0 - 8K-1.
 *   SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info -
 *     Attributes of the congestion point profile entry
 * REMARKS:
 *   Relevant only if in a sampling mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_cpq_sampling_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  );

uint32
  soc_pb_cnm_cpq_sampling_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  );

uint32
  soc_pb_cnm_cpq_sampling_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_cpq_sampling_set_unsafe" API.
 *     Refer to "soc_pb_cnm_cpq_sampling_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_cnm_cpq_sampling_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO             *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_intercept_timer_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the timer delay of the CNM Intercept functionality
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                     delay -
 *     Timer delay during which the flow control is set unless
 *     a new CNM is received for the same flow control signal.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_intercept_timer_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                     delay
  );

uint32
  soc_pb_cnm_intercept_timer_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                     delay
  );

uint32
  soc_pb_cnm_intercept_timer_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_intercept_timer_set_unsafe" API.
 *     Refer to "soc_pb_cnm_intercept_timer_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_cnm_intercept_timer_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT uint32                               *delay
  );



uint32
  SOC_PB_CNM_Q_MAPPING_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO *info
  );

uint32
  SOC_PB_CNM_CONGESTION_TEST_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  );

uint32
  SOC_PB_CNM_CP_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO *info
  );

uint32
  SOC_PB_CNM_CPQ_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO *info
  );

uint32
  SOC_PB_CNM_PPH_verify(
    SOC_SAND_IN  SOC_PB_CNM_PPH *info
  );

uint32
  SOC_PB_CNM_PDU_verify(
    SOC_SAND_IN  SOC_PB_CNM_PDU *info
  );

uint32
  SOC_PB_CNM_PETRA_B_PP_verify(
    SOC_SAND_IN  SOC_PB_CNM_PETRA_B_PP *info
  );

uint32
  SOC_PB_CNM_PACKET_verify(
    SOC_SAND_IN  SOC_PB_CNM_PACKET *info
  );

uint32
  SOC_PB_CNM_CP_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO *info
  );
#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_CNM_INCLUDED__*/
#endif
