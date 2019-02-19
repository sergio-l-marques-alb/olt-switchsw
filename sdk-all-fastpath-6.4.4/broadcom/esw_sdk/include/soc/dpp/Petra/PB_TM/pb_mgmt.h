/* $Id: pb_mgmt.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_mgmt.h
*
* MODULE PREFIX:  soc_pb_mgmt
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

#ifndef __SOC_PB_MGMT_INCLUDED__
/* { */
#define __SOC_PB_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_MGMT_MDIO_CLK_FREQ_MAX                              (2500)
#define SOC_PB_MGMT_MDIO_CLK_FREQ_MIN                              (1)

#define SOC_PB_HW_ADJ_DRAM_PLL_F_MIN                               (7)
#define SOC_PB_HW_ADJ_DRAM_PLL_F_MAX                               (63)
#define SOC_PB_HW_ADJ_DRAM_PLL_R_MIN                               (0)
#define SOC_PB_HW_ADJ_DRAM_PLL_R_MAX                               (7)
#define SOC_PB_HW_ADJ_DRAM_PLL_Q_MIN                               (1)
#define SOC_PB_HW_ADJ_DRAM_PLL_Q_MAX                               (4)

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

uint32
  soc_pb_mgmt_init_sequence_fixes_apply_unsafe(
    SOC_SAND_IN int unit
  );

uint32
  soc_pb_mgmt_ver_set(
    SOC_SAND_IN int unit
  );

uint32
  soc_pb_mgmt_ver_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32  *ver
  );

/*********************************************************************
* NAME:
 *   soc_pb_register_device_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This procedure registers a new device to be taken care
 *   of by this device driver. Physical device must be
 *   accessible by CPU when this call is made..
 * INPUT:
 *              uint32                       *base_address -
 *     Base address of direct access memory assigned for
 *     device's registers. This parameter needs to be specified
 *     even if physical access to device is not by direct
 *     access memory since all logic, within driver, up to
 *     actual physical access, assumes 'virtual' direct access
 *     memory. Memory block assigned by this pointer must not
 *     overlap other memory blocks in user's system and
 *     certainly not memory blocks assigned to other SOC_PETRA
 *     devices using this procedure.
 *   SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR     reset_device_ptr -
 *     BSP-function for device reset. Refer to
 *     'SOC_SAND_RESET_DEVICE_FUNC_PTR' definition.
 *   SOC_SAND_OUT uint32                      *unit_ptr -
 *     This procedure loads pointed memory with identifier of
 *     newly added device. This identifier is to be used by the
 *     caller for further accesses to this device..
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_register_device_unsafe(
             uint32                       *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR     reset_device_ptr,
    SOC_SAND_OUT int                      *unit_ptr
  );

uint32
  soc_pb_register_device_verify(
    SOC_SAND_IN  uint32                       *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR     reset_device_ptr
  );

/*********************************************************************
* NAME:
 *   soc_pb_unregister_device_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Undo soc_petra_register_device()
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     The device ID to be unregistered.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_unregister_device_unsafe(
    SOC_SAND_IN  int                      unit
  );

uint32
  soc_pb_unregister_device_verify(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_operation_mode_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set soc_petra device operation mode. This defines
 *   configurations, such as support for certain header
 *   types, etc.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE         *op_mode -
 *     Device operation mode.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE         *op_mode
  );

uint32
  soc_pb_mgmt_operation_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE         *op_mode
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_mgmt_operation_mode_set_unsafe" API.
 *     Refer to "soc_pb_mgmt_operation_mode_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_MGMT_OPERATION_MODE         *op_mode
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_max_pckt_size_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the maximal allowed packet size. The limitation can
 *   be performed based on the packet size before or after
 *   the ingress editing (external and internal configuration
 *   mode, accordingly). Packets above the specified value
 *   are dropped.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      port_ndx -
 *     Incoming port index. Range: 0 - 79.
 *   SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx -
 *     External mode filters the packets according to there
 *     original size. Internal mode filters the packets
 *     according to their size inside the device, after ingress
 *     editing.
 *   SOC_SAND_IN  uint32                       *max_size -
 *     Maximal allowed packet size per incoming port. Packets
 *     above this value will be dropped. Units: bytes.
 * REMARKS:
 *   1. This API gives a better resolution (i.e., per
 *   incoming port) than soc_petra_mgmt_pckt_size_range_set. 2.
 *   If both APIs are used to configure the maximal packet
 *   size, the value configured is set by the API called at
 *   last.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_max_pckt_size_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  );

uint32
  soc_pb_mgmt_max_pckt_size_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  );

uint32
  soc_pb_mgmt_max_pckt_size_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_mgmt_max_pckt_size_set_unsafe" API.
 *     Refer to "soc_pb_mgmt_max_pckt_size_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_mgmt_max_pckt_size_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT uint32                       *max_size
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_core_frequency_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the core clock frequency of the device.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT uint32                       clk_freq -
 *     Device core clock frequency. Units: MHz. Range: 150 -
 *     300.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_core_frequency_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                       *clk_freq
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_enable_traffic_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     SOC_SAND_IN uint32 enable_indication
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  );

uint32
  soc_pb_mgmt_pckt_size_range_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE          *size_range
  );

uint32
  soc_pb_mgmt_pckt_size_range_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE          *size_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_rev_b0_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Soc_petra-B B0 revision specific features.
 * INPUT:
 *   SOC_SAND_IN  int       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info -
 *     Soc_petra-B rev. B0 revision-specific features.
 * REMARKS:
 *   Relevant for Soc_petra-B rev. B0 only.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_rev_b0_set_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  );

uint32
  soc_pb_mgmt_rev_b0_set_verify(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_mgmt_rev_b0_set_unsafe" API.
 *     Refer to "soc_pb_mgmt_rev_b0_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_mgmt_rev_b0_get_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_OUT SOC_PB_MGMT_B0_INFO *info
  );


uint32
  SOC_PB_MGMT_OPERATION_MODE_verify(
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE *info
  );

uint32
  SOC_PB_INIT_PORTS_verify(
    SOC_SAND_IN  SOC_PB_INIT_PORTS *info
  );

uint32
  SOC_PB_HW_ADJ_MAL_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_MAL *info
  );

uint32
  SOC_PB_HW_ADJ_SPAUI_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_SPAUI *info
  );

uint32
  SOC_PB_HW_ADJ_GMII_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_GMII *info
  );

uint32
  SOC_PB_HW_ADJ_FATP_verify(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE mode,
    SOC_SAND_IN  SOC_PB_HW_ADJ_FATP *info
  );

uint32
  SOC_PB_HW_ADJ_ILKN_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_ILKN *info
  );

uint32
  SOC_PB_HW_ADJ_ELK_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_ELK *info
  );

uint32
  SOC_PB_HW_ADJ_SYNCE_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_SYNCE *info
  );

uint32
  SOC_PB_HW_ADJ_NIF_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_NIF *info
  );

uint32
  SOC_PB_HW_ADJ_FC_SCHEME_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_FC_SCHEME *info
  );

uint32
  SOC_PB_HW_ADJ_DRAM_PLL_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_DRAM_PLL *info
  );

uint32
  SOC_PB_HW_ADJ_STAT_IF_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_STAT_IF *info
  );

uint32
  SOC_PB_HW_ADJ_STREAMING_IF_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_STREAMING_IF *info
  );

uint32
  SOC_PB_HW_ADJUSTMENTS_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS *info
  );

uint32
  SOC_PB_INIT_STAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_INIT_STAG_INFO *info
  );

uint32
  SOC_PB_INIT_BASIC_CONF_verify(
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF *info
  );

uint32
  SOC_PB_MGMT_B0_INFO_verify(
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  );
#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_MGMT_INCLUDED__*/
#endif

