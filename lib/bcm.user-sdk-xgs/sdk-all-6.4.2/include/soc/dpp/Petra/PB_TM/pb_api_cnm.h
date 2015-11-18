/* $Id: soc_pb_api_cnm.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_API_CNM_INCLUDED__
/* { */
#define __SOC_PB_API_CNM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/TMC/tmc_api_cnm.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of sampling bases in a CP profile entry          */
#define  SOC_PB_CNM_NOF_SAMPLING_BASES (SOC_TMC_CNM_NOF_SAMPLING_BASES)

/*     Number of longs in the CP Id field in the PDU header    */
#define  SOC_PB_CNM_NOF_UINT32S_IN_CP_ID (SOC_TMC_CNM_NOF_UINT32S_IN_CP_ID)

/*     Number of CP profiles                                   */

/* $Id: soc_pb_api_cnm.h,v 1.6 Broadcom SDK $
 * Can be used as dest_tm_port (SOC_PB_CNM_PACKET structure).
 * In this case, the destination port will be the same port as
 * the incoming port for CNM message
 */
#define  SOC_PB_CNM_DEST_TM_PORT_AS_INCOMING (SOC_TCM_CNM_DEST_TM_PORT_AS_INCOMING)

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

#define SOC_PB_CNM_GEN_MODE_EXT_PP                             SOC_TMC_CNM_GEN_MODE_EXT_PP
#define SOC_PB_CNM_GEN_MODE_PETRA_B_PP                         SOC_TMC_CNM_GEN_MODE_PETRA_B_PP
#define SOC_PB_CNM_GEN_MODE_SAMPLING                           SOC_TMC_CNM_GEN_MODE_SAMPLING
#define SOC_PB_CNM_NOF_GEN_MODES                               SOC_TMC_CNM_NOF_GEN_MODES
typedef SOC_TMC_CNM_GEN_MODE                                   SOC_PB_CNM_GEN_MODE;

#define SOC_PB_CNM_Q_SET_8_CPS                                 SOC_TMC_CNM_Q_SET_8_CPS
#define SOC_PB_CNM_Q_SET_4_CPS                                 SOC_TMC_CNM_Q_SET_4_CPS
#define SOC_PB_CNM_NOF_Q_SETS                                  SOC_TMC_CNM_NOF_Q_SETS
typedef SOC_TMC_CNM_Q_SET                                      SOC_PB_CNM_Q_SET;

typedef SOC_TMC_CNM_Q_MAPPING_INFO                             SOC_PB_CNM_Q_MAPPING_INFO;
typedef SOC_TMC_CNM_CONGESTION_TEST_INFO                       SOC_PB_CNM_CONGESTION_TEST_INFO;
typedef SOC_TMC_CNM_CP_PROFILE_INFO                            SOC_PB_CNM_CP_PROFILE_INFO;
typedef SOC_TMC_CNM_CPQ_INFO                                   SOC_PB_CNM_CPQ_INFO;
typedef SOC_TMC_CNM_PPH                                        SOC_PB_CNM_PPH;
typedef SOC_TMC_CNM_PDU                                        SOC_PB_CNM_PDU;
typedef SOC_TMC_CNM_PETRA_B_PP                                 SOC_PB_CNM_PETRA_B_PP;
typedef SOC_TMC_CNM_PACKET                                     SOC_PB_CNM_PACKET;
typedef SOC_TMC_CNM_CP_INFO                                    SOC_PB_CNM_CP_INFO;

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
 *   soc_pb_cnm_cp_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Congestion Point functionality, in particular
 *   the packet generation mode and the fields of the CNM
 *   packet.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNM_CP_INFO                         *info -
 *     Attributes of the congestion point functionality
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_cp_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO                         *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_cnm_cp_set" API.
 *     Refer to "soc_pb_cnm_cp_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_cp_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PB_CNM_CP_INFO                         *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_q_mapping_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mapping from the pair (destination, traffic
 *   class) to the CP Queue.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO                  *info -
 *     Attributes of the CP Queue mapping
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_q_mapping_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO                  *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_cnm_q_mapping_set"
 *     API.
 *     Refer to "soc_pb_cnm_q_mapping_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_q_mapping_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PB_CNM_Q_MAPPING_INFO                  *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_congestion_test_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set parameters of the congestion test.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO            *info -
 *     Attributes of the congestion test functionality
 * REMARKS:
 *   If not set, the default values of the different
 *   configurations are False.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_congestion_test_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO            *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_congestion_test_set" API.
 *     Refer to "soc_pb_cnm_congestion_test_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_congestion_test_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PB_CNM_CONGESTION_TEST_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_cp_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure an entry into the Congestion Point Profile
 *   Table.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              profile_ndx -
 *     Index of the profile in the Congestion Point profile
 *     table. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO                 *info -
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
  soc_pb_cnm_cp_profile_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              profile_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO                 *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_cp_profile_set" API.
 *     Refer to "soc_pb_cnm_cp_profile_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_cp_profile_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              profile_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CP_PROFILE_INFO                 *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_sampling_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure an entry into the Congestion Point Profile
 *   Table for the sampling mode.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              profile_ndx -
 *     Index of the profile in the Congestion Point profile
 *     table. Range: 0 - 7.
 *   SOC_SAND_IN  uint32                               sampling_rate -
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
  soc_pb_cnm_sampling_profile_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              profile_ndx,
    SOC_SAND_IN  uint32                               sampling_rate
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_sampling_profile_set" API.
 *     Refer to "soc_pb_cnm_sampling_profile_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_sampling_profile_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              profile_ndx,
    SOC_SAND_OUT uint32                               *sampling_rate
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_cpq_pp_set
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
 *   SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO                        *info -
 *     Attributes of the congestion point profile entry
 * REMARKS:
 *   Relevant only if not in a sampling mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_cpq_pp_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO                        *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_cnm_cpq_pp_set"
 *     API.
 *     Refer to "soc_pb_cnm_cpq_pp_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_cpq_pp_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO                        *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_cpq_sampling_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the CP Queue attributes into an entry of the
 *   Congestion Point Table.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              cpq_ndx -
 *     Index of the CP Queue. Range: 0 - 8K-1.
 *   SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO                        *info -
 *     Attributes of the congestion point profile entry
 * REMARKS:
 *   Relevant only if in a sampling mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_cpq_sampling_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO                        *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_cpq_sampling_set" API.
 *     Refer to "soc_pb_cnm_cpq_sampling_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_cpq_sampling_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO                        *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnm_intercept_timer_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the timer delay of the CNM Intercept functionality
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               delay -
 *     Timer delay during which the flow control is set unless
 *     a new CNM is received for the same flow control signal.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnm_intercept_timer_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               delay
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnm_intercept_timer_set" API.
 *     Refer to "soc_pb_cnm_intercept_timer_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnm_intercept_timer_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT uint32                               *delay
  );

void
  SOC_PB_CNM_Q_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_Q_MAPPING_INFO *info
  );

void
  SOC_PB_CNM_CONGESTION_TEST_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_CONGESTION_TEST_INFO *info
  );

void
  SOC_PB_CNM_CP_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_CP_PROFILE_INFO *info
  );

void
  SOC_PB_CNM_CPQ_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO *info
  );

void
  SOC_PB_CNM_PPH_clear(
    SOC_SAND_OUT SOC_PB_CNM_PPH *info
  );

void
  SOC_PB_CNM_PDU_clear(
    SOC_SAND_OUT SOC_PB_CNM_PDU *info
  );

void
  SOC_PB_CNM_PETRA_B_PP_clear(
    SOC_SAND_OUT SOC_PB_CNM_PETRA_B_PP *info
  );

void
  SOC_PB_CNM_PACKET_clear(
    SOC_SAND_OUT SOC_PB_CNM_PACKET *info
  );

void
  SOC_PB_CNM_CP_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_CP_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_CNM_GEN_MODE_to_string(
    SOC_SAND_IN  SOC_PB_CNM_GEN_MODE enum_val
  );

const char*
  SOC_PB_CNM_Q_SET_to_string(
    SOC_SAND_IN  SOC_PB_CNM_Q_SET enum_val
  );

void
  SOC_PB_CNM_Q_MAPPING_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO *info
  );

void
  SOC_PB_CNM_CONGESTION_TEST_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  );

void
  SOC_PB_CNM_CP_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO *info
  );

void
  SOC_PB_CNM_CPQ_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO *info
  );

void
  SOC_PB_CNM_PPH_print(
    SOC_SAND_IN  SOC_PB_CNM_PPH *info
  );

void
  SOC_PB_CNM_PDU_print(
    SOC_SAND_IN  SOC_PB_CNM_PDU *info
  );

void
  SOC_PB_CNM_PETRA_B_PP_print(
    SOC_SAND_IN  SOC_PB_CNM_PETRA_B_PP *info
  );

void
  SOC_PB_CNM_PACKET_print(
    SOC_SAND_IN  SOC_PB_CNM_PACKET *info
  );

void
  SOC_PB_CNM_CP_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_CNM_INCLUDED__*/
#endif
