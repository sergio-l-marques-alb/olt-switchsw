/* $Id: pb_api_cnt.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_api_cnt.h
*
* MODULE PREFIX:  soc_pb_cnt
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

#ifndef __SOC_PB_API_CNT_INCLUDED__
/* { */
#define __SOC_PB_API_CNT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_cnt.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Cache length containing counters chosen by the polling
 *     algorithm.                                              */
#define  SOC_PB_CNT_CACHE_LENGTH (SOC_TMC_CNT_CACHE_LENGTH_PB)

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

#define SOC_PB_CNT_PROCESSOR_ID_A                              SOC_TMC_CNT_PROCESSOR_ID_A
#define SOC_PB_CNT_PROCESSOR_ID_B                              SOC_TMC_CNT_PROCESSOR_ID_B
#define SOC_PB_CNT_NOF_PROCESSOR_IDS                           SOC_TMC_CNT_NOF_PROCESSOR_IDS_PETRA_B
typedef SOC_TMC_CNT_PROCESSOR_ID                               SOC_PB_CNT_PROCESSOR_ID;

#define SOC_PB_CNT_MODE_STATISTICS_NO_COLOR                           SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR
#define SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_LOW                      SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW
#define SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_HI                       SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI
#define SOC_PB_CNT_NOF_MODE_INGS                               SOC_TMC_CNT_NOF_MODE_INGS
typedef SOC_TMC_CNT_MODE_STATISTICS                            SOC_PB_CNT_MODE_STATISTICS;

#define SOC_PB_CNT_MODE_EG_TYPE_TM                            SOC_TMC_CNT_MODE_EG_TYPE_TM
#define SOC_PB_CNT_MODE_EG_TYPE_VSI                           SOC_TMC_CNT_MODE_EG_TYPE_VSI
#define SOC_PB_CNT_MODE_EG_TYPE_OUTLIF                         SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF
#define SOC_PB_CNT_NOF_MODE_EGS                                SOC_TMC_CNT_NOF_MODE_EG_TYPES
typedef SOC_TMC_CNT_MODE_EG_TYPE                                    SOC_PB_CNT_MODE_EG_TYPES;
 
#define SOC_PB_CNT_MODE_EG_RES_NO_COLOR                        SOC_TMC_CNT_MODE_EG_RES_NO_COLOR
#define SOC_PB_CNT_MODE_EG_RES_COLOR                           SOC_TMC_CNT_MODE_EG_RES_COLOR
typedef SOC_TMC_CNT_MODE_EG_RES                                     SOC_PB_CNT_MODE_EG_RES;

#define SOC_PB_CNT_SRC_TYPE_ING_PP                             SOC_TMC_CNT_SRC_TYPE_ING_PP
#define SOC_PB_CNT_SRC_TYPE_VOQ                                SOC_TMC_CNT_SRC_TYPE_VOQ
#define SOC_PB_CNT_SRC_TYPE_STAG                               SOC_TMC_CNT_SRC_TYPE_STAG
#define SOC_PB_CNT_SRC_TYPE_VSQ                                SOC_TMC_CNT_SRC_TYPE_VSQ
#define SOC_PB_CNT_SRC_TYPE_CNM_ID                             SOC_TMC_CNT_SRC_TYPE_CNM_ID
#define SOC_PB_CNT_SRC_TYPE_EGR_PP                             SOC_TMC_CNT_SRC_TYPE_EGR_PP
#define SOC_PB_CNT_NOF_SRC_TYPES                               SOC_TMC_CNT_NOF_SRC_TYPES
typedef SOC_TMC_CNT_SRC_TYPE                                   SOC_PB_CNT_SRC_TYPE;

#define SOC_PB_CNT_Q_SET_SIZE_1_Q                              SOC_TMC_CNT_Q_SET_SIZE_1_Q
#define SOC_PB_CNT_Q_SET_SIZE_2_Q                              SOC_TMC_CNT_Q_SET_SIZE_2_Q
#define SOC_PB_CNT_Q_SET_SIZE_4_Q                              SOC_TMC_CNT_Q_SET_SIZE_4_Q
#define SOC_PB_CNT_Q_SET_SIZE_8_Q                              SOC_TMC_CNT_Q_SET_SIZE_8_Q
#define SOC_PB_CNT_NOF_Q_SET_SIZES                             SOC_TMC_CNT_NOF_Q_SET_SIZES
typedef SOC_TMC_CNT_Q_SET_SIZE                                 SOC_PB_CNT_Q_SET_SIZE;

typedef SOC_TMC_CNT_VOQ_PARAMS                                 SOC_PB_CNT_VOQ_PARAMS;
typedef SOC_TMC_CNT_MODE_EG                                    SOC_PB_CNT_MODE_EG;
typedef SOC_TMC_CNT_COUNTERS_INFO                              SOC_PB_CNT_COUNTERS_INFO;
typedef SOC_TMC_CNT_OVERFLOW                                   SOC_PB_CNT_OVERFLOW;
typedef SOC_TMC_CNT_STATUS                                     SOC_PB_CNT_STATUS;
typedef SOC_TMC_CNT_RESULT                                     SOC_PB_CNT_RESULT;
typedef SOC_TMC_CNT_RESULT_ARR                                 SOC_PB_CNT_RESULT_ARR;

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
 *   soc_pb_cnt_counters_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the Counter Processor: its counting mode and
 *   its counting source.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx -
 *     Counter processor index
 *   SOC_SAND_IN  SOC_PB_CNT_COUNTERS_INFO                   *info -
 *     Counter info of the counter processor
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnt_counters_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_IN  SOC_PB_CNT_COUNTERS_INFO                   *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_cnt_counters_set"
 *     API.
 *     Refer to "soc_pb_cnt_counters_set" API for details.
*********************************************************************/
uint32
  soc_pb_cnt_counters_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_OUT SOC_PB_CNT_COUNTERS_INFO                   *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnt_status_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the current status of the counter processor.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx -
 *     Counter processor index
 *   SOC_SAND_OUT SOC_PB_CNT_STATUS                          *proc_status -
 *     Counter processor status info
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnt_status_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_OUT SOC_PB_CNT_STATUS                          *proc_status
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnt_algorithmic_read
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Read the counter values saved in a cache according to
 *   the polling algorithm (algorithmic method).
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx -
 *     Counter processor index
 *   SOC_SAND_OUT SOC_PB_CNT_RESULT_ARR                      *result_arr -
 *     Counter values from the first 'nof_counters' counters in
 *     the cache which contains the counters chosen by the
 *     polling algorithm.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnt_algorithmic_read(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO     *counter_info_array,
    SOC_SAND_OUT SOC_PB_CNT_RESULT_ARR                      *result_arr
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnt_direct_read
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Read the counter value according to the counter.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx -
 *     Counter processor index
 *   SOC_SAND_IN  uint32                               counter_ndx -
 *     Counter index. Range: 0 - 8K-1.
 *   SOC_SAND_OUT SOC_PB_CNT_RESULT                          *read_rslt -
 *     Counter processor read result (in particular the counter
 *     values).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnt_direct_read(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_IN  SOC_PB_CNT_COUNTERS_INFO        *counter_info,
    SOC_SAND_IN  uint32                               counter_ndx,
    SOC_SAND_OUT SOC_PB_CNT_RESULT                          *read_rslt
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnt_q2cnt_id
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Convert a queue index to the counter index.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              queue_ndx -
 *     Queue index. Range: 0 - 32K-1.
 *   SOC_SAND_OUT uint32                              *counter_ndx -
 *     Counter index. Range: 0 - 8K-1.
 * REMARKS:
 *   The result depends directly on the parameters set with
 *   the function soc_pb_cnt_src_type_set. Relevant only if the
 *   counting source is the VOQs.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnt_q2cnt_id(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_IN  uint32                              queue_ndx,
    SOC_SAND_OUT uint32                              *counter_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnt_cnt2q_id
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Convert a counter index to the respective queue index.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   counter_ndx -
 *     Counter index. Range: 0 - 8K-1.
 *   SOC_SAND_OUT uint32                   *queue_ndx -
 *     Queue index. Range: 0 - 32K-1.
 * REMARKS:
 *   The result depends directly on the parameters set with
 *   the function soc_pb_cnt_src_type_set. Relevant only if the
 *   counting source is the VOQs.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnt_cnt2q_id(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   counter_ndx,
    SOC_SAND_OUT uint32                   *queue_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_cnt_meter_hdr_compensation_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the header compensation factor for the counter
 *   and meter modules to enable a meter and counter
 *   processing of this packet according to its original
 *   packet size.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   port_ndx -
 *     Incoming PP port id. Range: 0 - 63.
 *   SOC_SAND_IN  int32                    hdr_compensation -
 *     Header compensation value to remove from the packet
 *     size. Units: Bytes. Range: -15 - 15.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_cnt_meter_hdr_compensation_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  int32                    hdr_compensation
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_cnt_meter_hdr_compensation_set" API.
 *     Refer to "soc_pb_cnt_meter_hdr_compensation_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_cnt_meter_hdr_compensation_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT int32                    *hdr_compensation
  );

void
  SOC_PB_CNT_VOQ_PARAMS_clear(
    SOC_SAND_OUT SOC_PB_CNT_VOQ_PARAMS *info
  );
void
  SOC_PB_CNT_MODE_EG_clear(
    SOC_SAND_OUT SOC_PB_CNT_MODE_EG *info
  );

void
  SOC_PB_CNT_COUNTERS_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNT_COUNTERS_INFO *info
  );

void
  SOC_PB_CNT_OVERFLOW_clear(
    SOC_SAND_OUT SOC_PB_CNT_OVERFLOW *info
  );

void
  SOC_PB_CNT_STATUS_clear(
    SOC_SAND_OUT SOC_PB_CNT_STATUS *info
  );

void
  SOC_PB_CNT_RESULT_clear(
    SOC_SAND_OUT SOC_PB_CNT_RESULT *info
  );

void
  SOC_PB_CNT_RESULT_ARR_clear(
    SOC_SAND_OUT SOC_PB_CNT_RESULT_ARR *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_CNT_PROCESSOR_ID_to_string(
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID enum_val
  );

const char*
  SOC_PB_CNT_MODE_STATISTICS_to_string(
    SOC_SAND_IN  SOC_PB_CNT_MODE_STATISTICS enum_val
  );

const char*
  SOC_PB_CNT_MODE_EG_to_string(
    SOC_SAND_IN  SOC_PB_CNT_MODE_EG enum_val
  );

const char*
  SOC_PB_CNT_SRC_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_CNT_SRC_TYPE enum_val
  );

const char*
  SOC_PB_CNT_Q_SET_SIZE_to_string(
    SOC_SAND_IN  SOC_PB_CNT_Q_SET_SIZE enum_val
  );

void
  SOC_PB_CNT_VOQ_PARAMS_print(
    SOC_SAND_IN  SOC_PB_CNT_VOQ_PARAMS *info
  );

void
  SOC_PB_CNT_MODE_EG_print(
    SOC_SAND_IN  SOC_PB_CNT_MODE_EG *info
  );
void
  SOC_PB_CNT_COUNTERS_INFO_print(
    SOC_SAND_IN  SOC_PB_CNT_COUNTERS_INFO *info
  );

void
  SOC_PB_CNT_OVERFLOW_print(
    SOC_SAND_IN  SOC_PB_CNT_OVERFLOW *info
  );

void
  SOC_PB_CNT_STATUS_print(
    SOC_SAND_IN  SOC_PB_CNT_STATUS *info
  );

void
  SOC_PB_CNT_RESULT_print(
    SOC_SAND_IN  SOC_PB_CNT_RESULT *info
  );

void
  SOC_PB_CNT_RESULT_ARR_print(
    SOC_SAND_IN  SOC_PB_CNT_RESULT_ARR *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_CNT_INCLUDED__*/
#endif
