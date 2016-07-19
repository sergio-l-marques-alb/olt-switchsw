/* $Id: pb_api_ingress_traffic_mgmt.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_api_ingress_traffic_mgmt.h
*
* MODULE PREFIX:  soc_pb_ingress
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

#ifndef __SOC_PB_API_INGRESS_TRAFFIC_MGMT_INCLUDED__
/* { */
#define __SOC_PB_API_INGRESS_TRAFFIC_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximal number of DRAM buffers used for Full Multicast. */
#define  SOC_PB_ITM_DBUFF_FMC_MAX (32 * 1024)

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
   *  Statistics Tag is not used (disabled).
   */
  SOC_PB_ITM_STAG_ENABLE_MODE_DIS = 0,
  /*
   *  The Statistics Tag is enabled with no use of the VSQ pointer.
   *  In the Statistics Interface, the dequeue information is not
   *  available. It still can be used in Billing mode.
   */
  SOC_PB_ITM_STAG_ENABLE_MODE_EN_NO_VSQ = 1,
  /*
   *  The Statistics Tag is enabled and the use of the VSQ pointer
   *  is enabled.
   *  In the Statistics Interface, the dequeue information is not
   *  available. It still can be used in Billing mode.
   */
  SOC_PB_ITM_STAG_ENABLE_MODE_EN_WITH_VSQ = 2,
  /*
   *  Number of types in SOC_PB_ITM_STAG_ENABLE_MODE
   */
  SOC_PB_ITM_NOF_STAG_ENABLE_MODES = 3
}SOC_PB_ITM_STAG_ENABLE_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable (in the specified mode)/Disable STAG
   */
  SOC_PB_ITM_STAG_ENABLE_MODE enable_mode;
  /*
   *  Offset of the statistics tag header data from the base
   *  header, in 4 bit (nibble) units. Range: 5 - 63.
   *  The format of the Statistic-Tag is {VSQ-Pointer (8b, optional), Statistic-Tag(18b)}
   *  Note: the offset points to the LSB of the Statistic-Tag
   */
  uint32 offset_4bits;

} SOC_PB_ITM_STAG_INFO;


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
 *   soc_pb_itm_stag_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Defines the way the Statistics Tag is used. The
 *   statistics tag can be used in the Statistics Interface,
 *   for mapping to VSQs and further used for VSQ-based
 *   drop/FC decisions.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_ITM_STAG_INFO            *info -
 *     Pointer to configuration structure.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_itm_stag_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_ITM_STAG_INFO            *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_itm_stag_set" API.
 *     Refer to "soc_pb_itm_stag_set" API for details.
*********************************************************************/
uint32
  soc_pb_itm_stag_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_ITM_STAG_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_itm_committed_q_size_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the size of committed queue size (i.e., the
 *   guaranteed memory) for each VOQ, even in the case that a
 *   set of queues consume most of the memory resources.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32        rt_cls4_ndx -
 *     Queue rate class modulo 4. Range: 0 - 3.
 *   SOC_SAND_IN  uint32         grnt_bytes -
 *     Value of the guaranteed memory size. Units: Bytes.
 *     Range: 0 - 256M.
 *   SOC_SAND_OUT uint32                    *exact_grnt_bytes -
 *     Exact value of the guaranteed memory size. Units: Bytes.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_itm_committed_q_size_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32        rt_cls4_ndx,
    SOC_SAND_IN  uint32                    grnt_bytes,
    SOC_SAND_OUT uint32                    *exact_grnt_bytes
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_itm_committed_q_size_set" API.
 *     Refer to "soc_pb_itm_committed_q_size_set" API for details.
*********************************************************************/
uint32
  soc_pb_itm_committed_q_size_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32        rt_cls4_ndx,
    SOC_SAND_OUT uint32                    *grnt_bytes
  );

void
  SOC_PB_ITM_STAG_INFO_clear(
    SOC_SAND_OUT SOC_PB_ITM_STAG_INFO *info
  );

uint32
  soc_pb_itm_dp_discard_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN  uint32                  discard_dp
  );

uint32
  soc_pb_itm_dp_discard_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_ITM_STAG_ENABLE_MODE_to_string(
    SOC_SAND_IN  SOC_PB_ITM_STAG_ENABLE_MODE enum_val
  );

void
  SOC_PB_ITM_STAG_INFO_print(
    SOC_SAND_IN  SOC_PB_ITM_STAG_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_INGRESS_TRAFFIC_MGMT_INCLUDED__*/
#endif
