/* $Id: ppd_api_counting.h,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_counting.h
*
* MODULE PREFIX:  soc_ppd_counting
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

#ifndef __SOC_PPD_API_COUNTING_INCLUDED__
/* { */
#define __SOC_PPD_API_COUNTING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_counting.h>

#include <soc/dpp/PPD/ppd_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     indicates not to assign counter to the traffic          */

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
   * Auto generated. Do not edit following section {
   */
  SOC_PPD_CNT_COUNTER_TYPE_SET = SOC_PPD_PROC_DESC_BASE_COUNTING_FIRST,
  SOC_PPD_CNT_COUNTER_TYPE_SET_PRINT,
  SOC_PPD_CNT_COUNTER_TYPE_GET,
  SOC_PPD_CNT_COUNTER_TYPE_GET_PRINT,
  SOC_PPD_CNT_COUNTER_MAP_SET,
  SOC_PPD_CNT_COUNTER_MAP_SET_PRINT,
  SOC_PPD_CNT_COUNTER_MAP_GET,
  SOC_PPD_CNT_COUNTER_MAP_GET_PRINT,
  SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_SET,
  SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_PRINT,
  SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_GET,
  SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_GET_PRINT,
  SOC_PPD_CNT_COUNTER_VALUE_GET,
  SOC_PPD_CNT_COUNTER_VALUE_GET_PRINT,
  SOC_PPD_COUNTING_GET_PROCS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PPD_COUNTING_PROCEDURE_DESC_LAST
} SOC_PPD_COUNTING_PROCEDURE_DESC;

typedef SOC_PPC_CNT_COUNTER_TYPE                               SOC_PPD_CNT_COUNTER_TYPE;

typedef SOC_PPC_CNT_TYPE                                       SOC_PPD_CNT_TYPE;

typedef SOC_PPC_CNT_COUNTER_STAGE                              SOC_PPD_CNT_COUNTER_STAGE;

typedef SOC_PPC_CNT_COUNTER_ID                                 SOC_PPD_CNT_COUNTER_ID;
typedef SOC_PPC_CNT_KEY                                        SOC_PPD_CNT_KEY;
typedef SOC_PPC_CNT_RESULT                                     SOC_PPD_CNT_RESULT;

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
 *   soc_ppd_cnt_counter_type_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set counter type (count Packet/Bytes)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key -
 *     Counter instance ID
 *   SOC_SAND_IN  SOC_PPD_CNT_COUNTER_TYPE                    counter_type -
 *     Counter Type
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_cnt_counter_type_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_TYPE                    counter_type
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_cnt_counter_type_set" API.
 *     Refer to "soc_ppd_cnt_counter_type_set" API for details.
*********************************************************************/
uint32
  soc_ppd_cnt_counter_type_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_OUT SOC_PPD_CNT_COUNTER_TYPE                    *counter_type
  );

/*********************************************************************
* NAME:
 *   soc_ppd_cnt_counter_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map counting key to a counter instance.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key -
 *     SOC_SAND_IN SOC_PPD_CNT_KEY *count_key
 *   SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_id -
 *     Counter instance ID
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_cnt_counter_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_id
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_cnt_counter_map_set" API.
 *     Refer to "soc_ppd_cnt_counter_map_set" API for details.
*********************************************************************/
uint32
  soc_ppd_cnt_counter_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key,
    SOC_SAND_OUT SOC_PPD_CNT_COUNTER_ID                      *counter_id
  );

/*********************************************************************
* NAME:
 *   soc_ppd_cnt_counter_ins_range_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map range of counting keys to a range of counters.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx -
 *     Type of entity to be countered LIF/VSI/...
 *   SOC_SAND_IN  uint32                                first_countered_ndx -
 *     ID of first entity to be countered according to counter
 *     type.
 *   SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *first_counter_ins_id -
 *     The ID of first counter instance, first_countered_id
 *     will be mapped to this counter instance.
 *   SOC_SAND_IN  uint32                                nof_counter_ins -
 *     Number of counter instances to be used.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     Whether the operation succeeds (upon add). Operation may
 *     fail upon unavailable resources.
 * REMARKS:
 *   - in T20E the group in SOC_PPD_CNT_COUNTER_ID has to be
 *   zero, yet packet maybe assigned two counters, according
 *   to tunnel and PWE.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_cnt_counter_ins_range_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx,
    SOC_SAND_IN  uint32                                first_countered_ndx,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *first_counter_ins_id,
    SOC_SAND_IN  uint32                                nof_counter_ins,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_cnt_counter_ins_range_map_set" API.
 *     Refer to "soc_ppd_cnt_counter_ins_range_map_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_cnt_counter_ins_range_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx,
    SOC_SAND_IN  uint32                                first_countered_ndx,
    SOC_SAND_OUT SOC_PPD_CNT_COUNTER_ID                      *first_counter_ins_id,
    SOC_SAND_OUT uint32                                *nof_counter_ins
  );

/*********************************************************************
* NAME:
 *   soc_ppd_cnt_counter_value_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the number of packets and/or bytes, counted by
 *   the counter instance
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key -
 *     Counter ID.
 *   SOC_SAND_IN  uint8                               reset -
 *     if set to TRUE then the counter value is cleared after
 *     read. If set to FALSE only read the current counter
 *     value.
 *   SOC_SAND_OUT SOC_PPD_CNT_RESULT                          *val -
 *     The number of packets and/or bytes, counted by the
 *     counter instance
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_cnt_counter_value_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_IN  uint8                               reset,
    SOC_SAND_OUT SOC_PPD_CNT_RESULT                          *val
  );

/*********************************************************************
* NAME:
 *   soc_ppd_counting_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_counting module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_counting_get_procs_ptr(void);

void
  SOC_PPD_CNT_COUNTER_ID_clear(
    SOC_SAND_OUT SOC_PPD_CNT_COUNTER_ID *info
  );

void
  SOC_PPD_CNT_KEY_clear(
    SOC_SAND_OUT SOC_PPD_CNT_KEY *info
  );

void
  SOC_PPD_CNT_RESULT_clear(
    SOC_SAND_OUT SOC_PPD_CNT_RESULT *info
  );

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_CNT_COUNTER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_TYPE enum_val
  );

const char*
  SOC_PPD_CNT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_CNT_TYPE enum_val
  );

const char*
  SOC_PPD_CNT_COUNTER_STAGE_to_string(
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_STAGE enum_val
  );

void
  SOC_PPD_CNT_COUNTER_ID_print(
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID *info
  );

void
  SOC_PPD_CNT_KEY_print(
    SOC_SAND_IN  SOC_PPD_CNT_KEY *info
  );

void
  SOC_PPD_CNT_RESULT_print(
    SOC_SAND_IN  SOC_PPD_CNT_RESULT *info
  );

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_cnt_counter_type_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_TYPE                    counter_type
  );

void
  soc_ppd_cnt_counter_type_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key
  );

void
  soc_ppd_cnt_counter_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_id
  );

void
  soc_ppd_cnt_counter_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key
  );

void
  soc_ppd_cnt_counter_ins_range_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx,
    SOC_SAND_IN  uint32                                first_countered_ndx,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *first_counter_ins_id,
    SOC_SAND_IN  uint32                                nof_counter_ins
  );

void
  soc_ppd_cnt_counter_ins_range_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx,
    SOC_SAND_IN  uint32                                first_countered_ndx
  );

void
  soc_ppd_cnt_counter_value_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_IN  uint8                               reset
  );

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_COUNTING_INCLUDED__*/
#endif
