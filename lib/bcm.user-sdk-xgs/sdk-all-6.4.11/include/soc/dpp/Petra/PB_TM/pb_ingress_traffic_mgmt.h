/* $Id: pb_ingress_traffic_mgmt.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_INGRESS_TRAFFIC_MGMT_INCLUDED__
/* { */
#define __SOC_PB_INGRESS_TRAFFIC_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_api_ingress_traffic_mgmt.h>

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
*     soc_pb_itm_init
* FUNCTION:
*     Initialization of the Soc_petra-B blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. Called as part of the initialization sequence.
*   2. This functions contains only the additional features, on top
*      of the Soc_petra-A initialization as implemented in soc_petra_itm_init()
*********************************************************************/
uint32
  soc_pb_itm_init(
    SOC_SAND_IN  int  unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_itm_stag_set_unsafe
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
  soc_pb_itm_stag_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_ITM_STAG_INFO            *info
  );

uint32
  soc_pb_itm_stag_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_ITM_STAG_INFO            *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_itm_stag_set_unsafe" API.
 *     Refer to "soc_pb_itm_stag_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_itm_stag_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_ITM_STAG_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_itm_committed_q_size_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the size of committed queue size (i.e., the
 *   guaranteed memory) for each VOQ, even in the case that a
 *   set of queues consume most of the memory resources.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    grnt_bytes -
 *     Value of the guaranteed memory size. Units: Bytes. Range:
 *     0 - 256M.
 *   SOC_SAND_OUT uint32                    *exact_grnt_bytes -
 *     Exact value of the guaranteed memory size. Units: Bytes.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_itm_committed_q_size_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32        rt_cls4_ndx,
    SOC_SAND_IN  uint32                    grnt_bytes,
    SOC_SAND_OUT uint32                    *exact_grnt_bytes
  );

uint32
  soc_pb_itm_committed_q_size_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32        rt_cls4_ndx,
    SOC_SAND_IN  uint32                    grnt_bytes
  );

uint32
  soc_pb_itm_committed_q_size_get_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        rt_cls4_ndx
  );
/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_itm_committed_q_size_set_unsafe" API.
 *     Refer to "soc_pb_itm_committed_q_size_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_itm_committed_q_size_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32        rt_cls4_ndx,
    SOC_SAND_OUT uint32                    *grnt_bytes
  );

/* $Id: pb_ingress_traffic_mgmt.h,v 1.6 Broadcom SDK $
 *	Extension to the Soc_petra API
 */
uint32
  soc_pb_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_GLOB_RCS_DROP_TH *info
  );

uint32
    soc_pb_itm_glob_rcs_drop_set_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PETRA_THRESH_WITH_HYST_INFO mem_size[SOC_PETRA_NOF_DROP_PRECEDENCE],
      SOC_SAND_OUT SOC_PETRA_THRESH_WITH_HYST_INFO exact_mem_size[SOC_PETRA_NOF_DROP_PRECEDENCE]
    );

uint32
  soc_pb_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PETRA_THRESH_WITH_HYST_INFO *mem_size
  );

/*********************************************************************
* NAME:
 *   soc_pb_itm_dp_discard_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the drop precedence value above which 
 *     all packets will always be discarded.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    discard_dp -
 *     Value of DP above which packets
 *     will de discarded. Range: 0-4.
 *     4 means disables the discard.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_itm_dp_discard_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  discard_dp
  );

uint32
  soc_pb_itm_dp_discard_set_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        discard_dp
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_itm_dp_discard_set_unsafe" API.
 *     Refer to "arad_itm_dp_discard_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_itm_dp_discard_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  );

uint32
  SOC_PB_ITM_STAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_ITM_STAG_INFO *info
  );
#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_INGRESS_TRAFFIC_MGMT_INCLUDED__*/
#endif
