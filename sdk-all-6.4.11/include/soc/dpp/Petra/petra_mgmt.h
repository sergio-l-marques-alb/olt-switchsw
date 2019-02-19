/* $Id: soc_petra_mgmt.h,v 1.5 Broadcom SDK $
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


#ifndef __SOC_PETRA_MGMT_INCLUDED__
/* { */
#define __SOC_PETRA_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_mgmt.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_mgmt.h,v 1.5 Broadcom SDK $
 *  Packet size limitations
 */
/* Minimal packet size, variable size cells */
#define SOC_PETRA_MGMT_PCKT_SIZE_BYTES_VSC_MIN      64
/* Maximal packet size, variable size cells */
#define SOC_PETRA_MGMT_PCKT_SIZE_BYTES_VSC_MAX      (16*1024 - 128)
/* Minimal packet size, fixed size cells */
#define SOC_PETRA_MGMT_PCKT_SIZE_BYTES_FSC_MIN      33
/* Maximal packet size, fixed size cells */
#define SOC_PETRA_MGMT_PCKT_SIZE_BYTES_FSC_MAX      (16*1024 - 128)

/*
 *  Packet Size adjustments
 */
/* The offset decremented by the HW before checking packet range */
#define SOC_PETRA_MGMT_PCKT_RNG_HW_OFFSET 1

/* The CRC size assumed by the packet range API (external size).    */
/* Note: for 3-byte CRC (SPAUI), add one to the requested external  */
/* packet size range                                                */
#define SOC_PETRA_MGMT_PCKT_RNG_NIF_CRC_BYTES 4

/* DRAM CRC */
#define SOC_PETRA_MGMT_PCKT_RNG_DRAM_CRC_BYTES 2

/* The value to decrement from the requested when */
/* limiting the internal packet size              */
#define SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL \
  (SOC_PETRA_MGMT_PCKT_RNG_HW_OFFSET)


/* The value to decrement from the requested when */
/* limiting the external packet size              */
#define SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL \
  (SOC_PETRA_MGMT_PCKT_RNG_HW_OFFSET + SOC_PETRA_MGMT_PCKT_RNG_NIF_CRC_BYTES - SOC_PETRA_MGMT_PCKT_RNG_DRAM_CRC_BYTES)

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
  soc_petra_mgmt_init_sequence_fixes_apply_unsafe(
    SOC_SAND_IN int unit
  );

uint32
  soc_petra_mgmt_fdr_enablers_val_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable,
    SOC_SAND_OUT uint32  *value
  );

uint32
  soc_petra_mgmt_ver_set(
    SOC_SAND_IN int unit
  );

uint32
  soc_petra_mgmt_ver_get(
    SOC_SAND_IN int unit,
    SOC_SAND_OUT uint32  *ver
  );

/*********************************************************************
* NAME:
*     soc_petra_register_device_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This procedure registers a new device to be taken care
*     of by this device driver. Physical device must be
*     accessible by CPU when this call is made..
* INPUT:
*  SOC_SAND_IN  uint32                  *base_address -
*     Base address of direct access memory assigned for
*     device's registers. This parameter needs to be specified
*     even if physical access to device is not by direct
*     access memory since all logic, within driver, up to
*     actual physical access, assumes 'virtual' direct access
*     memory. Memory block assigned by this pointer must not
*     overlap other memory blocks in user's system and
*     certainly not memory blocks assigned to other SOC_PETRA
*     devices using this procedure.
*  SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr -
*     BSP-function for device reset. Refer to
*     'SOC_SAND_RESET_DEVICE_FUNC_PTR' definition.
*  SOC_SAND_OUT uint32                 *unit_ptr -
*     This procedure loads pointed memory with identifier of
*     newly added device. This identifier is to be used by the
*     caller for further accesses to this device..
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_register_device_unsafe(
             uint32                  *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_OUT int                 *unit_ptr
  );

/*********************************************************************
* NAME:
*     soc_petra_unregister_device_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Undo soc_petra_register_device()
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     The device ID to be unregistered.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_unregister_device_unsafe(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_operation_mode_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE *op_mode -
*     Soc_petra device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE *op_mode
  );

uint32
  soc_petra_mgmt_ref_clk_verify(
    SOC_SAND_IN  uint32 ref_clk
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_operation_mode_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE *op_mode -
*     Soc_petra device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_operation_mode_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_MGMT_OPERATION_MODE *op_mode -
*     Soc_petra device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_credit_worth_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  credit_worth -
*     Credit worth value. Range: 1 - 8K-1. Unit: bytes.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_credit_worth_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  credit_worth
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_credit_worth_verify
* TYPE:
*   PROC
* FUNCTION:
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  credit_worth -
*     Credit worth value. Range: 1 - 8K-1. Unit: bytes.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_credit_worth_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  credit_worth
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_credit_worth_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint32                  *credit_worth -
*     Credit worth value. Range: 1 - 8K-1. Unit: bytes.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_credit_worth_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *credit_worth
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_system_fap_id_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Set the fabric system ID of the device. Must be unique
*     in the system.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 sys_fap_id -
*     The system ID of the device (Unique in the system).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_system_fap_id_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set the fabric system ID of the device. Must be unique
*     in the system.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 sys_fap_id -
*     The system ID of the device (Unique in the system).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_system_fap_id_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set the fabric system ID of the device. Must be unique
*     in the system.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint32                 *sys_fap_id -
*     The system ID of the device (Unique in the system).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                 *sys_fap_id
  );

uint32
  soc_petra_mgmt_all_ctrl_cells_enable_get_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_OUT  uint8  *enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting control cells.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN uint8  enable -
*     Enable/disable indication
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_all_ctrl_cells_enable_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting control cells.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN uint8  enable -
*     Enable/disable indication
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_all_ctrl_cells_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
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
  soc_petra_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_enable_traffic_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     SOC_SAND_IN uint8 enable
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_enable_traffic_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint8  *enable -
*     SOC_SAND_OUT uint8  enable
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_pckt_size_range_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx -
*     External mode filters the packets according to there
*     original size. Internal mode filters the packets
*     according to their size inside the device, after ingress
*     editing.
*  SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range -
*     the minimal and maximal packet size boundaries.
* REMARKS:
 *   1. The allowed range depends on the Fabric Cell mode.
 *   For Fixed-Size cells, the range is 33 - 16K-128. For
 *   Variable Size cells, the range is 64 - 16K-128. 2. For
 *   external size limitation (before ingress editing),
 *   setting minimal or maximal size to '0' disables
 *   filtering according to the external (original) packet
 *   size. 3. For external size limitation, the API assumes
 *   Network CRC of 4 bytes. If 3-Bytes CRC is used (optional
 *   for a SPAUI interface), add '1' to the min/max
 *   boundaries of the external packet size. 4. In Soc_petra-B, a
 *   configuration of the maximum packet size can be done per
 *   port via the soc_pb_mgmt_max_pckt_size_set() API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_mgmt_pckt_size_range_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_pckt_size_range_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx -
*     External mode filters the packets according to there
*     original size. Internal mode filters the packets
*     according to their size inside the device, after ingress
*     editing.
*  SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range -
*     the minimal and maximal packet size boundaries.
* REMARKS:
 *   1. The allowed range depends on the Fabric Cell mode.
 *   For Fixed-Size cells, the range is 33 - 16K-128. For
 *   Variable Size cells, the range is 64 - 16K-128. 2. For
 *   external size limitation (before ingress editing),
 *   setting minimal or maximal size to '0' disables
 *   filtering according to the external (original) packet
 *   size. 3. For external size limitation, the API assumes
 *   Network CRC of 4 bytes. If 3-Bytes CRC is used (optional
 *   for a SPAUI interface), add '1' to the min/max
 *   boundaries of the external packet size. 4. In Soc_petra-B, a
 *   configuration of the maximum packet size can be done per
 *   port via the soc_pb_mgmt_max_pckt_size_set() API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_mgmt_pckt_size_range_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_pckt_size_range_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx -
*     External mode filters the packets according to there
*     original size. Internal mode filters the packets
*     according to their size inside the device, after ingress
*     editing.
*  SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE      *size_range -
*     the minimal and maximal packet size boundaries.
* REMARKS:
*     1. The allowed range depends on the Fabric Cell mode.
*     For Fixed-Size cells, the range is 33 - 16K-128, for
*     Variable Size cells, the range is 64 - 16K-128.
*     2. For external size limitation (before ingress editing),
*     setting minimal or maximal size to '0' disables
*     filtering according to the external (original) packet
*     size.
*     3. For external size limitation, the API assumes
*     Network CRC of 4 Bytes.
*     If 3-Bytes CRC is used (optional for a SPAUI interface),
*     add '1' to the min/max boundaries of the external packet size.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_mgmt_pckt_size_range_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  );

uint8
  soc_petra_mgmt_is_pp_enabled(
    SOC_SAND_IN int unit
  );

/*********************************************************************
* NAME:
 *   soc_petra_mgmt_core_frequency_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the core clock frequency of the device.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT uint32                                  *clk_freq -
 *     Device core clock frequency. Units: MHz. For Soc_petra-A,
 *     Range: 150 - 250. For Soc_petra-B, Range: 150 - 300.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_core_frequency_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint32                                 *clk_freq
  );

#define soc_pa_mgmt_core_frequency_get_unsafe soc_petra_mgmt_core_frequency_get_unsafe

uint32
  soc_petra_mgmt_core_frequency_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_MGMT_INCLUDED__*/
#endif
