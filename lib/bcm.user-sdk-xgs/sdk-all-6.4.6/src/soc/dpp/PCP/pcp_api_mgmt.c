/* $Id: pcp_api_mgmt.c,v 1.5 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PCP/pcp_general.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_api_mgmt.h>
#include <soc/dpp/PCP/pcp_mgmt.h>
#include <soc/dpp/PCP/pcp_api_framework.h>

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     This procedure registers a new device to be taken care
 *     of by this device driver. Physical device must be
 *     accessible by CPU when this call is made
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_register_device(
    SOC_SAND_IN  uint32                        *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR      reset_device_ptr,
    SOC_SAND_OUT int                       *unit_ptr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(PCP_REGISTER_DEVICE);

  SOC_SAND_CHECK_NULL_INPUT(unit_ptr);

  res = pcp_register_device_verify(
          base_address,
          reset_device_ptr,
          unit_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_register_device_unsafe(
          base_address,
          reset_device_ptr,
          unit_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_register_device()", 0, 0);
}

/*********************************************************************
*     Undo pcp_register_device
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_unregister_device(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_UNREGISTER_DEVICE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = pcp_unregister_device_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_unregister_device_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_unregister_device()", 0, 0);
}

/*********************************************************************
*     Set PCP device operation mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_mgmt_op_mode_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_OP_MODE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_mgmt_op_mode_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_mgmt_op_mode_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_op_mode_set()", 0, 0);
}

/*********************************************************************
*     Set PCP device operation mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_mgmt_op_mode_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_MGMT_OP_MODE                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_OP_MODE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_mgmt_op_mode_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_mgmt_op_mode_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_op_mode_get()", 0, 0);
}

/*********************************************************************
*     Initialize the device, including:1. Configuration of
 *     Soc_petra power up configuration word2. Initialize the
 *     device tables and registers to default values 3.
 *     Initialize board-specific hardware interfaces according
 *     to configurable information, as passed in 'hw_adjust' 4.
 *     Perform basic device initialization 5. DMA
 *     initialization
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_mgmt_init_sequence_phase1(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  uint8                       silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INIT_SEQUENCE_PHASE1);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  res = pcp_mgmt_init_sequence_phase1_verify(
          unit,
          hw_adjust,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_mgmt_init_sequence_phase1_unsafe(
          unit,
          hw_adjust,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_init_sequence_phase1()", 0, 0);
}

/*********************************************************************
*     This API enables all interfaces, as set in the operation
 *     mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INIT_SEQUENCE_PHASE2);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = pcp_mgmt_init_sequence_phase2_verify(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_mgmt_init_sequence_phase2_unsafe(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_init_sequence_phase2()", 0, 0);
}

uint32
  pcp_mgmt_memory_test(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_mgmt_memory_test_unsafe(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_memory_test()", 0, 0);
}

void
  PCP_MGMT_ELK_OP_MODE_clear(
    SOC_SAND_OUT PCP_MGMT_ELK_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_ELK_OP_MODE));
  info->enable = 0;
  info->mode = 0;
  info->ilm_key_mask_bitmap = 0;
  info->size_mbit = PCP_MGMT_NOF_HW_IF_DRAM_SIZE_MBITS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_DMA_THRESHOLD_clear(
    SOC_SAND_OUT PCP_DMA_THRESHOLD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_DMA_THRESHOLD));
  info->fc = 0;
  info->drop = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_DMA_TX_OP_MODE_clear(
    SOC_SAND_OUT PCP_DMA_TX_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_DMA_TX_OP_MODE));
  info->enable = 0;
  info->head_ptr = 0;
  PCP_DMA_THRESHOLD_clear(&(info->thresholds));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_DMA_RX_OP_MODE_clear(
    SOC_SAND_OUT PCP_DMA_RX_OP_MODE *info
  )
{

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_DMA_RX_OP_MODE));
  info->enable = 0;
  for (ind = 0; ind < SOC_SAND_PP_NOF_TC; ++ind)
  {
    info->head_ptr[ind] = 0;
  }
  info->buff_size = 0;
  PCP_DMA_THRESHOLD_clear(&(info->thresholds));
  info->cal_len = 0;
  info->cal_m = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_DMA_OP_MODE_clear(
    SOC_SAND_OUT PCP_DMA_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_DMA_OP_MODE));
  info->enable = 0;
  PCP_DMA_TX_OP_MODE_clear(&(info->tx));
  PCP_DMA_RX_OP_MODE_clear(&(info->rx_mode));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_OAM_OP_MODE_clear(
    SOC_SAND_OUT PCP_MGMT_OAM_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_OAM_OP_MODE));
  info->enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_STS_OP_MODE_clear(
    SOC_SAND_OUT PCP_MGMT_STS_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_STS_OP_MODE));
  info->enable = 0;
  info->conf_word = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_OP_MODE_clear(
    SOC_SAND_OUT PCP_MGMT_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_OP_MODE));
  PCP_MGMT_ELK_OP_MODE_clear(&(info->elk));
  PCP_DMA_OP_MODE_clear(&(info->dma));
  PCP_MGMT_OAM_OP_MODE_clear(&(info->oam));
  PCP_MGMT_STS_OP_MODE_clear(&(info->sts));
  PCP_MGMT_IPV4_INFO_clear(&(info->ipv4_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_DRAM_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_DRAM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_HW_IF_DRAM_INFO));
  info->size_mbit = PCP_MGMT_NOF_HW_IF_DRAM_SIZE_MBITS - 1;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_QDR_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_QDR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_HW_IF_QDR_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_SERDES_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_SERDES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_HW_IF_SERDES_INFO));
  info->rx_eq_ctrl = 0;
  info->rx_eq_dc_gain = 0;
  info->tx_preemp_0t = 0;
  info->tx_preemp_1t = 0;
  info->tx_preemp_2t = 0;
  info->tx_vodctrl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_ELK_IF_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_ELK_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_HW_IF_ELK_IF_INFO));
  info->enable = 0;
  info->enable_flow_ctrl = 0;
  PCP_MGMT_HW_IF_SERDES_INFO_clear(&(info->serdes_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_PCI_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_PCI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_MGMT_HW_IF_PCI_INFO));
  info->enable = 0;
  info->burst_size = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_HW_ADJUSTMENTS_clear(
    SOC_SAND_OUT PCP_HW_ADJUSTMENTS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_HW_ADJUSTMENTS));
  PCP_MGMT_HW_IF_PCI_INFO_clear(&(info->pci));
  PCP_MGMT_HW_IF_DRAM_INFO_clear(&(info->dram));
  PCP_MGMT_HW_IF_QDR_INFO_clear(&(info->qdr));
  PCP_MGMT_HW_IF_ELK_IF_INFO_clear(&(info->elk_if));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  PCP_MGMT_IPV4_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_IPV4_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(PCP_MGMT_IPV4_INFO));
  info->nof_vrfs = 0;
  for (ind = 0; ind < PCP_NOF_VRFS; ++ind)
  {
    info->max_routes_in_vrf[ind] = 0;
  }
  info->lpm_4th_bank_size = 0;
  info->write_cb = NULL;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if PCP_DEBUG_IS_LVL1

const char*
  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_to_string(
    SOC_SAND_IN  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288:
    str = "mbit_288";
  break;
  case PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_576:
    str = "mbit_576";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  PCP_MGMT_HW_IF_QDR_SIZE_MBIT_to_string(
    SOC_SAND_IN  PCP_MGMT_HW_IF_QDR_SIZE_MBIT  enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_MGMT_HW_IF_QDR_SIZE_MBIT_18:
    str = "pcp_mgmt_hw_if_qdr_size_mbit_18";
  break;
  case PCP_MGMT_HW_IF_QDR_SIZE_MBIT_36:
    str = "pcp_mgmt_hw_if_qdr_size_mbit_36";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  PCP_MGMT_ELK_LKP_MODE_to_string(
    SOC_SAND_IN  PCP_MGMT_ELK_LKP_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_MGMT_ELK_LKP_MODE_EM:
    str = "em";
  break;
  case PCP_MGMT_ELK_LKP_MODE_LPM:
    str = "lpm";
  break;
  case PCP_MGMT_ELK_LKP_MODE_BOTH:
    str = "both";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  PCP_MGMT_ELK_OP_MODE_print(
    SOC_SAND_IN  PCP_MGMT_ELK_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META("mode %s "), PCP_MGMT_ELK_LKP_MODE_to_string(info->mode)));
  LOG_CLI((BSL_META("ilm_key_mask_bitmap 0x%x "), info->ilm_key_mask_bitmap));
  LOG_CLI((BSL_META("size_mbit %s "), PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_to_string(info->size_mbit)));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_DMA_THRESHOLD_print(
    SOC_SAND_IN  PCP_DMA_THRESHOLD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("fc: %u\n\r"),info->fc));
  LOG_CLI((BSL_META("drop: %u\n\r"),info->drop));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_DMA_TX_OP_MODE_print(
    SOC_SAND_IN  PCP_DMA_TX_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META("head_ptr: %u\n\r"),info->head_ptr));
  LOG_CLI((BSL_META("thresholds:")));
  PCP_DMA_THRESHOLD_print(&(info->thresholds));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_DMA_RX_OP_MODE_print(
    SOC_SAND_IN  PCP_DMA_RX_OP_MODE *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("enable: %u\n\r"),info->enable));
  for (ind = 0; ind < SOC_SAND_PP_NOF_TC; ++ind)
  {
    LOG_CLI((BSL_META("head_ptr[%u]: %u\n\r"),ind,info->head_ptr[ind]));
  }
  LOG_CLI((BSL_META("buff_size: %u\n\r"),info->buff_size));
  LOG_CLI((BSL_META("thresholds:")));
  PCP_DMA_THRESHOLD_print(&(info->thresholds));
  LOG_CLI((BSL_META("cal_len: %u\n\r"),info->cal_len));
  LOG_CLI((BSL_META("cal_m: %u\n\r"),info->cal_m));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_DMA_OP_MODE_print(
    SOC_SAND_IN  PCP_DMA_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META("tx:")));
  PCP_DMA_TX_OP_MODE_print(&(info->tx));
  LOG_CLI((BSL_META("rx_mode:")));
  PCP_DMA_RX_OP_MODE_print(&(info->rx_mode));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_OAM_OP_MODE_print(
    SOC_SAND_IN  PCP_MGMT_OAM_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("enable: %u\n\r"),info->enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_STS_OP_MODE_print(
    SOC_SAND_IN  PCP_MGMT_STS_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META("conf_word: %u\n\r"),info->conf_word));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_OP_MODE_print(
    SOC_SAND_IN  PCP_MGMT_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("elk:")));
  PCP_MGMT_ELK_OP_MODE_print(&(info->elk));
  LOG_CLI((BSL_META("dma:")));
  PCP_DMA_OP_MODE_print(&(info->dma));
  LOG_CLI((BSL_META("oam:")));
  PCP_MGMT_OAM_OP_MODE_print(&(info->oam));
  LOG_CLI((BSL_META("sts:")));
  PCP_MGMT_STS_OP_MODE_print(&(info->sts));
  PCP_MGMT_IPV4_INFO_print(&(info->ipv4_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_DRAM_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_DRAM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("size_mbit %s "), PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_to_string(info->size_mbit)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_QDR_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_QDR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_SERDES_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_SERDES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("rx_eq_ctrl: %u\n\r"),info->rx_eq_ctrl));
  LOG_CLI((BSL_META("rx_eq_dc_gain: %u\n\r"),info->rx_eq_dc_gain));
  LOG_CLI((BSL_META("tx_preemp_0t: %u\n\r"),info->tx_preemp_0t));
  LOG_CLI((BSL_META("tx_preemp_1t: %u\n\r"),info->tx_preemp_1t));
  LOG_CLI((BSL_META("tx_preemp_2t: %u\n\r"),info->tx_preemp_2t));
  LOG_CLI((BSL_META("tx_vodctrl: %u\n\r"),info->tx_vodctrl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_ELK_IF_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_ELK_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META("enable_flow_ctrl: %u\n\r"),info->enable_flow_ctrl));
  LOG_CLI((BSL_META("serdes_info:")));
  PCP_MGMT_HW_IF_SERDES_INFO_print(&(info->serdes_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MGMT_HW_IF_PCI_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_PCI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META("burst_size: %u\n\r"),info->burst_size));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_HW_ADJUSTMENTS_print(
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("pci:")));
  PCP_MGMT_HW_IF_PCI_INFO_print(&(info->pci));
  LOG_CLI((BSL_META("dram:")));
  PCP_MGMT_HW_IF_DRAM_INFO_print(&(info->dram));
  LOG_CLI((BSL_META("qdr:")));
  PCP_MGMT_HW_IF_QDR_INFO_print(&(info->qdr));
  LOG_CLI((BSL_META("elk_if:")));
  PCP_MGMT_HW_IF_ELK_IF_INFO_print(&(info->elk_if));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
PCP_MGMT_IPV4_INFO_print(
                         SOC_SAND_IN  PCP_MGMT_IPV4_INFO *info
                         )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("nof_vrfs: %u\n\r"),info->nof_vrfs));
  LOG_CLI((BSL_META("lpm_4th_bank_size: %u\n\r"),info->lpm_4th_bank_size));
  for (ind = 0; ind < info->nof_vrfs; ++ind)
  {
    LOG_CLI((BSL_META("info->max_routes_in_vrf[ind]: %u\n\r"),info->max_routes_in_vrf[ind]));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

