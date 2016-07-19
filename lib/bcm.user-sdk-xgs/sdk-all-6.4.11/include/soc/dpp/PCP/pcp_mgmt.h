/* $Id: pcp_mgmt.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PCP_MGMT_INCLUDED__
/* { */
#define __SOC_PCP_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <soc/dpp/PCP/pcp_api_mgmt.h>
#include <soc/dpp/PCP/pcp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define PCP_MGMT_VER_REG_BASE        ((0x20000 + 0x50) * sizeof(uint32))
#define PCP_MGMT_CHIP_TYPE_FLD_LSB   0
#define PCP_MGMT_CHIP_TYPE_FLD_MSB   19
#define PCP_MGMT_DBG_VER_FLD_LSB     20
#define PCP_MGMT_DBG_VER_FLD_MSB     27
#define PCP_MGMT_CHIP_VER_FLD_LSB    28
#define PCP_MGMT_CHIP_VER_FLD_MSB    31

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
  PCP_REGISTER_DEVICE = PCP_PROC_DESC_BASE_MGMT_FIRST,
  PCP_REGISTER_DEVICE_PRINT,
  PCP_REGISTER_DEVICE_UNSAFE,
  PCP_REGISTER_DEVICE_VERIFY,
  PCP_UNREGISTER_DEVICE,
  PCP_UNREGISTER_DEVICE_PRINT,
  PCP_UNREGISTER_DEVICE_UNSAFE,
  PCP_UNREGISTER_DEVICE_VERIFY,
  PCP_MGMT_OP_MODE_SET,
  PCP_MGMT_OP_MODE_SET_PRINT,
  PCP_MGMT_OP_MODE_SET_UNSAFE,
  PCP_MGMT_OP_MODE_SET_VERIFY,
  PCP_MGMT_OP_MODE_GET,
  PCP_MGMT_OP_MODE_GET_PRINT,
  PCP_MGMT_OP_MODE_GET_VERIFY,
  PCP_MGMT_OP_MODE_GET_UNSAFE,
  PCP_MGMT_INIT_SEQUENCE_PHASE1,
  PCP_MGMT_INIT_SEQUENCE_PHASE1_PRINT,
  PCP_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE,
  PCP_MGMT_INIT_SEQUENCE_PHASE1_VERIFY,
  PCP_MGMT_INIT_SEQUENCE_PHASE2,
  PCP_MGMT_INIT_SEQUENCE_PHASE2_PRINT,
  PCP_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE,
  PCP_MGMT_INIT_SEQUENCE_PHASE2_VERIFY,
  PCP_MGMT_GET_PROCS_PTR,
  PCP_MGMT_GET_ERRS_PTR,
  PCP_MGMT_INDIRECT_MEMORY_MAP_GET,
  PCP_MGMT_INDIRECT_TABLE_MAP_GET,
  PCP_MGMT_INDIRECT_MODULE_INFO_GET,
  PCP_MGMT_INDIRECT_MODULE_INFO_INIT,
  PCP_MGMT_INDIRECT_MODULE_INIT,
  PCP_MGMT_ACCESS_DB_INIT,
  PCP_MGMT_MODULE_INIT,
  PCP_MGMT_DEVICE_INIT,
  PCP_MGMT_DEVICE_CLOSE,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_MGMT_PROCEDURE_DESC_LAST
} PCP_MGMT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_MGMT_MODE_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_MGMT_FIRST,
  PCP_MGMT_FC_OUT_OF_RANGE_ERR,
  PCP_MGMT_DROP_OUT_OF_RANGE_ERR,
  PCP_MGMT_HEAD_PTR_OUT_OF_RANGE_ERR,
  PCP_MGMT_BUFF_SIZE_OUT_OF_RANGE_ERR,
  PCP_MGMT_CAL_LEN_OUT_OF_RANGE_ERR,
  PCP_MGMT_CAL_M_OUT_OF_RANGE_ERR,
  PCP_MGMT_CONF_WORD_OUT_OF_RANGE_ERR,
  PCP_MGMT_SIZE_MBIT_OUT_OF_RANGE_ERR,
  PCP_MGMT_RX_EQ_CTRL_OUT_OF_RANGE_ERR,
  PCP_MGMT_RX_EQ_DC_GAIN_OUT_OF_RANGE_ERR,
  PCP_MGMT_TX_PREEMP_0T_OUT_OF_RANGE_ERR,
  PCP_MGMT_TX_PREEMP_1T_OUT_OF_RANGE_ERR,
  PCP_MGMT_TX_PREEMP_2T_OUT_OF_RANGE_ERR,
  PCP_MGMT_TX_VODCTRL_OUT_OF_RANGE_ERR,
  PCP_MGMT_BURST_SIZE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  PCP_GENERAL_EXIT_PLACE_TAKE_SEMAPHORE,
  PCP_GENERAL_EXIT_PLACE_GIVE_SEMAPHORE,

	PCP_MGMT_PCP_MEMORY_NOT_READY_ERR,
	PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR,

  /*
   * Last element. Do no touch.
   */
  PCP_MGMT_ERR_LAST
} PCP_MGMT_ERR;

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

/* general api */

uint32
  pcp_general_puc_enable_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint8            enable
  );

uint32
  pcp_oam_mgmt_device_type_definition_init(void);

/*********************************************************************
* NAME:
 *   pcp_register_device_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This procedure registers a new device to be taken care
 *   of by this device driver. Physical device must be
 *   accessible by CPU when this call is made
 * INPUT:
 *   SOC_SAND_IN  uint32                        *base_address -
 *     Base address of direct access memory assigned for
 *     device's registers. This parameter needs to be specified
 *     even if physical access to device is not by direct
 *     access memory since all logic, within driver, up to
 *     actual physical access, assumes 'virtual' direct access
 *     memory. Memory block assigned by this pointer must not
 *     overlap other memory blocks in user's system and
 *     certainly not memory blocks assigned to other devices
 *     using this procedure.
 *   SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR      reset_device_ptr -
 *     BSP-function for device reset. Refer to
 *     'SOC_SAND_RESET_DEVICE_FUNC_PTR' definition
 *   SOC_SAND_IN  uint32                       *unit_ptr -
 *     This procedure loads pointed memory with identifier of
 *     newly added device. This identifier is to be used by the
 *     caller for further accesses to this device
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_register_device_unsafe(
    SOC_SAND_IN  uint32                        *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR      reset_device_ptr,
    SOC_SAND_OUT int                       *unit_ptr
  );

uint32
  pcp_register_device_verify(
    SOC_SAND_IN  uint32                        *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR      reset_device_ptr,
    SOC_SAND_IN  int                       *unit_ptr
  );

/*********************************************************************
* NAME:
 *   pcp_unregister_device_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Undo pcp_register_device
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_unregister_device_unsafe(
    SOC_SAND_IN  int                       unit
  );

uint32
  pcp_unregister_device_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
* NAME:
 *   pcp_mgmt_op_mode_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set PCP device operation mode.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access
 *   SOC_SAND_IN  PCP_MGMT_OP_MODE                *info -
 *     Device operation mode
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_mgmt_op_mode_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE                *info
  );

uint32
  pcp_mgmt_op_mode_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE                *info
  );

uint32
  pcp_mgmt_op_mode_get_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_mgmt_op_mode_set_unsafe" API.
 *     Refer to "pcp_mgmt_op_mode_set_unsafe" API for details.
*********************************************************************/
uint32
  pcp_mgmt_op_mode_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_MGMT_OP_MODE                *info
  );

/*********************************************************************
* NAME:
 *   pcp_mgmt_init_sequence_phase1_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Initialize the device, including:1. Configuration of
 *   Soc_petra power up configuration word2. Initialize the
 *   device tables and registers to default values 3.
 *   Initialize board-specific hardware interfaces according
 *   to configurable information, as passed in 'hw_adjust' 4.
 *   Perform basic device initialization 5. DMA
 *   initialization
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access
 *   SOC_SAND_IN  PCP_HW_ADJUSTMENTS              *hw_adjust -
 *     Contains user-defined initialization information for
 *     hardware interfaces
 *   SOC_SAND_IN  uint8                       silent -
 *     If TRUE, progress printing will be suppressed
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  uint8                       silent
  );

uint32
  pcp_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  uint8                       silent
  );

/*********************************************************************
* NAME:
 *   pcp_mgmt_init_sequence_phase2_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This API enables all interfaces, as set in the operation
 *   mode.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access
 *   SOC_SAND_IN  uint8                       silent -
 *     If TRUE, progress printing will be suppressed
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  );

uint32
  pcp_mgmt_init_sequence_phase2_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  );

uint32
  pcp_mgmt_memory_test_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  );

/*********************************************************************
* NAME:
 *   pcp_mgmt_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_api_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_mgmt_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_mgmt_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_api_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_mgmt_get_errs_ptr(void);

uint32
  PCP_MGMT_ELK_OP_MODE_verify(
    SOC_SAND_IN  PCP_MGMT_ELK_OP_MODE *info
  );

uint32
  PCP_DMA_THRESHOLD_verify(
    SOC_SAND_IN  PCP_DMA_THRESHOLD *info
  );

uint32
  PCP_DMA_TX_OP_MODE_verify(
    SOC_SAND_IN  PCP_DMA_TX_OP_MODE *info
  );

uint32
  PCP_DMA_RX_OP_MODE_verify(
    SOC_SAND_IN  PCP_DMA_RX_OP_MODE *info
  );

uint32
  PCP_DMA_OP_MODE_verify(
    SOC_SAND_IN  PCP_DMA_OP_MODE *info
  );

uint32
  PCP_MGMT_OAM_OP_MODE_verify(
    SOC_SAND_IN  PCP_MGMT_OAM_OP_MODE *info
  );

uint32
  PCP_MGMT_STS_OP_MODE_verify(
    SOC_SAND_IN  PCP_MGMT_STS_OP_MODE *info
  );

uint32
  PCP_MGMT_OP_MODE_verify(
    SOC_SAND_IN  PCP_MGMT_OP_MODE *info
  );

uint32
  PCP_MGMT_HW_IF_DRAM_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_DRAM_INFO *info
  );

uint32
  PCP_MGMT_HW_IF_QDR_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_QDR_INFO *info
  );

uint32
  PCP_MGMT_HW_IF_SERDES_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_SERDES_INFO *info
  );

uint32
  PCP_MGMT_HW_IF_ELK_IF_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_ELK_IF_INFO *info
  );

uint32
  PCP_MGMT_HW_IF_PCI_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_PCI_INFO *info
  );

uint32
  PCP_HW_ADJUSTMENTS_verify(
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_MGMT_INCLUDED__*/
#endif
