/* $Id: ppd_api_counting.c,v 1.12 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_counting.c
*
* MODULE PREFIX:  ppd
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_counting.h>
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_counting.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 *  MACROS   *
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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_counting[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_TYPE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_TYPE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_TYPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_TYPE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_VALUE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CNT_COUNTER_VALUE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_COUNTING_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Set counter type (count Packet/Bytes)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_cnt_counter_type_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_TYPE                    counter_type
  )
{
#ifdef LINK_T20E_LIBRARIES
  uint32
    res = SOC_SAND_OK;
#endif /* LINK_T20E_LIBRARIES */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CNT_COUNTER_TYPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counter_key);

  SOC_T20E_DEVICE_CALL(cnt_counter_type_set,(unit, counter_key, counter_type));

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_cnt_counter_type_set_print,(unit,counter_key,counter_type));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_cnt_counter_type_set()", 0, 0);
}

/*********************************************************************
*     Set counter type (count Packet/Bytes)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_cnt_counter_type_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_OUT SOC_PPD_CNT_COUNTER_TYPE                    *counter_type
  )
{
#ifdef LINK_T20E_LIBRARIES
  uint32
    res = SOC_SAND_OK;
#endif /* LINK_T20E_LIBRARIES */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CNT_COUNTER_TYPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counter_key);
  SOC_SAND_CHECK_NULL_INPUT(counter_type);

  SOC_T20E_DEVICE_CALL(cnt_counter_type_get,(unit, counter_key, counter_type));

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_cnt_counter_type_get_print,(unit,counter_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_cnt_counter_type_get()", 0, 0);
}

/*********************************************************************
*     Map counting key to a counter instance.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_cnt_counter_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_id
  )
{
#ifdef LINK_T20E_LIBRARIES
  uint32
    res = SOC_SAND_OK;
#endif /* LINK_T20E_LIBRARIES */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CNT_COUNTER_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(count_key);
  SOC_SAND_CHECK_NULL_INPUT(counter_id);

  SOC_T20E_DEVICE_CALL(cnt_counter_map_set,(unit, count_key, counter_id));

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_cnt_counter_map_set_print,(unit,count_key,counter_id));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_cnt_counter_map_set()", 0, 0);
}

/*********************************************************************
*     Map counting key to a counter instance.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_cnt_counter_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key,
    SOC_SAND_OUT SOC_PPD_CNT_COUNTER_ID                      *counter_id
  )
{
#ifdef LINK_T20E_LIBRARIES
  uint32
    res = SOC_SAND_OK;
#endif /* LINK_T20E_LIBRARIES */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CNT_COUNTER_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(count_key);
  SOC_SAND_CHECK_NULL_INPUT(counter_id);

  SOC_T20E_DEVICE_CALL(cnt_counter_map_get,(unit, count_key, counter_id));

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_cnt_counter_map_get_print,(unit,count_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_cnt_counter_map_get()", 0, 0);
}

/*********************************************************************
*     Map range of counting keys to a range of counters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_cnt_counter_ins_range_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx,
    SOC_SAND_IN  uint32                                first_countered_ndx,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *first_counter_ins_id,
    SOC_SAND_IN  uint32                                nof_counter_ins,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
#ifdef LINK_T20E_LIBRARIES
  uint32
    res = SOC_SAND_OK;
#endif /* LINK_T20E_LIBRARIES */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(first_counter_ins_id);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_T20E_DEVICE_CALL(cnt_counter_ins_range_map_set,(unit, countered_type_ndx, first_countered_ndx, first_counter_ins_id, nof_counter_ins, success));

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_cnt_counter_ins_range_map_set_print,(unit,countered_type_ndx,first_countered_ndx,first_counter_ins_id,nof_counter_ins));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_cnt_counter_ins_range_map_set()", 0, first_countered_ndx);
}

/*********************************************************************
*     Map range of counting keys to a range of counters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_cnt_counter_ins_range_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx,
    SOC_SAND_IN  uint32                                first_countered_ndx,
    SOC_SAND_OUT SOC_PPD_CNT_COUNTER_ID                      *first_counter_ins_id,
    SOC_SAND_OUT uint32                                *nof_counter_ins
  )
{
#ifdef LINK_T20E_LIBRARIES
  uint32
    res = SOC_SAND_OK;
#endif /* LINK_T20E_LIBRARIES */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CNT_COUNTER_INS_RANGE_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(first_counter_ins_id);
  SOC_SAND_CHECK_NULL_INPUT(nof_counter_ins);

  SOC_T20E_DEVICE_CALL(cnt_counter_ins_range_map_get,(unit, countered_type_ndx, first_countered_ndx, first_counter_ins_id, nof_counter_ins));

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_cnt_counter_ins_range_map_get_print,(unit,countered_type_ndx,first_countered_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_cnt_counter_ins_range_map_get()", 0, first_countered_ndx);
}

/*********************************************************************
*     Returns the number of packets and/or bytes, counted by
 *     the counter instance
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_cnt_counter_value_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_IN  uint8                               reset,
    SOC_SAND_OUT SOC_PPD_CNT_RESULT                          *val
  )
{
#ifdef LINK_T20E_LIBRARIES
  uint32
    res = SOC_SAND_OK;
#endif /* LINK_T20E_LIBRARIES */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CNT_COUNTER_VALUE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counter_key);
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_T20E_DEVICE_CALL(cnt_counter_value_get,(unit, counter_key, reset, val));

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_cnt_counter_value_get_print,(unit,counter_key,reset));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_cnt_counter_value_get()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_counting module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_counting_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_counting;
}
void
  SOC_PPD_CNT_COUNTER_ID_clear(
    SOC_SAND_OUT SOC_PPD_CNT_COUNTER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_CNT_COUNTER_ID_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_CNT_KEY_clear(
    SOC_SAND_OUT SOC_PPD_CNT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_CNT_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_CNT_RESULT_clear(
    SOC_SAND_OUT SOC_PPD_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_CNT_RESULT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_CNT_COUNTER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_TYPE enum_val
  )
{
  return SOC_PPC_CNT_COUNTER_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_CNT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_CNT_TYPE enum_val
  )
{
  return SOC_PPC_CNT_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_CNT_COUNTER_STAGE_to_string(
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_STAGE enum_val
  )
{
  return SOC_PPC_CNT_COUNTER_STAGE_to_string(enum_val);
}

void
  SOC_PPD_CNT_COUNTER_ID_print(
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_CNT_COUNTER_ID_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_CNT_KEY_print(
    SOC_SAND_IN  SOC_PPD_CNT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_CNT_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_CNT_RESULT_print(
    SOC_SAND_IN  SOC_PPD_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_CNT_RESULT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_cnt_counter_type_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_TYPE                    counter_type
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "counter_key:")));
  SOC_PPD_CNT_COUNTER_ID_print((counter_key));

  LOG_CLI((BSL_META_U(unit,
                      "counter_type %s "), SOC_PPD_CNT_COUNTER_TYPE_to_string(counter_type)));

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_ppd_cnt_counter_type_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "counter_key:")));
  SOC_PPD_CNT_COUNTER_ID_print((counter_key));

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_ppd_cnt_counter_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_id
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "count_key:")));
  SOC_PPD_CNT_KEY_print((count_key));

  LOG_CLI((BSL_META_U(unit,
                      "counter_id:")));
  SOC_PPD_CNT_COUNTER_ID_print((counter_id));

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_ppd_cnt_counter_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_KEY                             *count_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "count_key:")));
  SOC_PPD_CNT_KEY_print((count_key));

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_ppd_cnt_counter_ins_range_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx,
    SOC_SAND_IN  uint32                                first_countered_ndx,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *first_counter_ins_id,
    SOC_SAND_IN  uint32                                nof_counter_ins
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "countered_type_ndx %s "), SOC_PPD_CNT_TYPE_to_string(countered_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "first_countered_ndx: %lu\n\r"),first_countered_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "first_counter_ins_id:")));
  SOC_PPD_CNT_COUNTER_ID_print((first_counter_ins_id));

  LOG_CLI((BSL_META_U(unit,
                      "nof_counter_ins: %lu\n\r"),nof_counter_ins));

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_ppd_cnt_counter_ins_range_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_TYPE                            countered_type_ndx,
    SOC_SAND_IN  uint32                                first_countered_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "countered_type_ndx %s "), SOC_PPD_CNT_TYPE_to_string(countered_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "first_countered_ndx: %lu\n\r"),first_countered_ndx));

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_ppd_cnt_counter_value_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_CNT_COUNTER_ID                      *counter_key,
    SOC_SAND_IN  uint8                               reset
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "counter_key:")));
  SOC_PPD_CNT_COUNTER_ID_print((counter_key));

  LOG_CLI((BSL_META_U(unit,
                      "reset: %u\n\r"),reset));

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

