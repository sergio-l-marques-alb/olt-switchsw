/* $Id: pcp_api_mgmt.h,v 1.3 Broadcom SDK $
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

#ifndef __SOC_PCP_API_MGMT_INCLUDED__
/* { */
#define __SOC_PCP_API_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/PCP/pcp_oam_api_general.h>
#include <soc/dpp/PCP/pcp_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


/*
 * whether the ipv4 module support caching operation
 * i.e. mode where user can add/remove many routes
 * to software shdaow and commit all changes in one call
 * see soc_ppd_frwrd_ip_routes_cache_mode_enable_set/
 *     soc_ppd_frwrd_ip_routes_cache_commit
 * used in PCP_MGMT_IPV4_INFO.flags
 */
#define  PCP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE (0x1)
/*
 * whether the ipv4 module support defragement operation
 * in Perta-B, IPv4 lpm includes dynamic memory management
 * This flag declare whether the module supports defragement
 * for these memories
 * see soc_ppd_frwrd_ipv4_mem_defrage
 * used in PCP_MGMT_IPV4_INFO.flags
 */
#define  PCP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG (0x2)

#define PCP_MGMT_ELK_OP_MOD_ILM_KEY_MASK_BITMAP_MASK_INRIF 				  (0)
#define PCP_MGMT_ELK_OP_MOD_ILM_KEY_MASK_BITMAP_MASK_PORT 				(1)

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



/*********************************************************************
* NAME:
*     PCP_MGMT_LPM_BANK_ENTRY_WRITE
* FUNCTION:
*  call back to write to 4th access of LPM bank
* INPUT:
*  SOC_SAND_IN  uint32             address -
*   address 
*  SOC_SAND_IN  uint32       *info -
*   info to write to entry, [as array of uint32]
*  SOC_SAND_IN  uint32       flags -
*   flags to be used by write operation
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
typedef
  uint32
  (*PCP_MGMT_LPM_BANK_ENTRY_WRITE)(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             address,
    SOC_SAND_IN  uint32                             *info,
    SOC_SAND_IN  uint32                             flags
  );

/*********************************************************************
* NAME:
*     PCP_MGMT_LPM_BANK_ENTRY_READ
* FUNCTION:
*  call back to read to 4th access of LPM bank
* INPUT:
*  SOC_SAND_IN  uint32             address -
*   address 
*  SOC_SAND_IN  uint32       *info -
*   info to read to entry, as array of uint32
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
typedef
  uint32
  (*PCP_MGMT_LPM_BANK_ENTRY_READ)(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             address,
    SOC_SAND_IN  uint32                             *info
  );


typedef enum
{
  /*
   *  RLDRAM memory size, 288 Mbits
   */
  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288 = 0,
  /*
   *  RLDRAM memory size, 576 Mbits
   */
  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_576 = 1,
  /*
   *  Number of types in PCP_MGMT_HW_IF_DRAM_SIZE_MBIT
   */
  PCP_MGMT_NOF_HW_IF_DRAM_SIZE_MBITS = 2
}PCP_MGMT_HW_IF_DRAM_SIZE_MBIT;

typedef enum
{
  /*
   *  DRAM memory size, 18 Mbits
   */
  PCP_MGMT_HW_IF_QDR_SIZE_MBIT_18 = 0,
  /*
   *  DRAM memory size, 36 Mbits
   */
  PCP_MGMT_HW_IF_QDR_SIZE_MBIT_36 = 1,
  /*
   *  Number of types in PCP_MGMT_HW_IF_QDR_SIZE_MBIT
   */
  PCP_MGMT_NOF_HW_IF_QDR_SIZE_MBIT = 2
}PCP_MGMT_HW_IF_QDR_SIZE_MBIT ;

typedef enum
{
  /*
   *  All ELK memory is used for exact match lookups (e.g.,
   *  MAC, ILM...)
   */
  PCP_MGMT_ELK_LKP_MODE_EM = 0,
  /*
   *  All ELK memory is used for LPM lookups (vrf + DIP)
   */
  PCP_MGMT_ELK_LKP_MODE_LPM = 1,
  /*
   *  Half of ELK memory is used for EM lookups, and the other
   *  half is used for LPM lookups
   */
  PCP_MGMT_ELK_LKP_MODE_BOTH = 2,
  /*
   *  Number of types in PCP_MGMT_ELK_LKP_MODE
   */
  PCP_MGMT_NOF_ELK_LKP_MODES = 3
}PCP_MGMT_ELK_LKP_MODE;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

 /*
  * number of supported VRFs
  * Range 1-256.
  * supported VRF IDs: 0-(nof_vrfs-1)
  */
  uint32 nof_vrfs;
 /*
  * maximum number of routes to support in LPM for IPv4 UC
  * for each VRF.
  * note: this is size to allocate for SW management.
  * maximum number of routes limited also by HW tables.
  */
  uint32 max_routes_in_vrf[PCP_NOF_VRFS];
 /*
  * flags for ipv4 management
  * see PCP_MGMT_IPV4_OP_MODE
  */
  uint32 flags;

 /*
  * 4th access bank size
  */
  uint32 lpm_4th_bank_size;

  PCP_MGMT_LPM_BANK_ENTRY_WRITE write_cb;
  
} PCP_MGMT_IPV4_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable ELK configuration
   */
  uint8 enable;
  /*
   *  RLDRAM memory size. Units: Mbits
   */
  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT size_mbit;
  /*
   *  ELK mode
   */
  PCP_MGMT_ELK_LKP_MODE mode;
    /*
   *  ILM key mask configuration
   */
  uint32 ilm_key_mask_bitmap;

} PCP_MGMT_ELK_OP_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Flow control threshold. When FIFO exceeds this threshold
   *  flow control indication is sent
   */
  uint32 fc;
  /*
   *  Drop threshold. When FIFO exceeds this threshold drop
   *  indication is sent
   */
  uint32 drop;

} PCP_DMA_THRESHOLD;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable DMA configuration
   */
  uint8 enable;
  /*
   *  Pointer to the head of the descriptors TX chain. Must be
   *  a physical address (not virtual). The user must make
   *  sure that the pointed memory is synchronized with CPU
   *  cache.
   */
  uint32 head_ptr;
  /*
   *  Configurations for flow control and drop thresholds
   */
  PCP_DMA_THRESHOLD thresholds;

} PCP_DMA_TX_OP_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable DMA configuration
   */
  uint8 enable;
  /*
   *  Pointer to the head of the descriptors RX chain. Must be
   *  a physical address (not virtual). The user must make
   *  sure that the pointed memory is synchronized with CPU
   *  cache. One chain for each traffic class
   */
  uint32 head_ptr[SOC_SAND_PP_NOF_TC];
  /*
   *  Buffer size configuration. The buffer size assigned in
   *  the CPU memory for packet payload.
   */
  uint32 buff_size;
  /*
   *  Configurations for flow control and drop thresholds.
   *  Sent to the Soc_petra on the out of band interface
   */
  PCP_DMA_THRESHOLD thresholds;
  /*
   *  Determines the calendar length of the out of band
   *  interface. Multiplying this value by cal_m should be
   *  equal to multiplication of the cal_m and cal_len
   *  configured in the Soc_petra
   */
  uint32 cal_len;
  /*
   *  Determines the calendar multiplier of the out of band
   *  interface. Multiplying this value by cal_len should be
   *  equal to multiplication of the cal_m and cal_len
   *  configured in the Soc_petra
   */
  uint32 cal_m;

} PCP_DMA_RX_OP_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable DMA configurationtx_mode
   */
  uint8 enable;
  /*
   *  operation mode configuration
   */
  PCP_DMA_TX_OP_MODE tx;
  /*
   *  RX operation mode configuration
   */
  PCP_DMA_RX_OP_MODE rx_mode;

} PCP_DMA_OP_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable OAM configuration
   */
  uint8 enable;

} PCP_MGMT_OAM_OP_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable statistics configuration
   */
  uint8 enable;
  /*
   *  Power up configuration word passed to the Soc_petra on the
   *  statistics interface
   */
  uint32 conf_word;

} PCP_MGMT_STS_OP_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Operation mode for ELK
   */
  PCP_MGMT_ELK_OP_MODE elk;
  /*
   *  Opeation mode for DMA
   */
  PCP_DMA_OP_MODE dma;
  /*
   *  Opeation mode for OAM
   */
  PCP_MGMT_OAM_OP_MODE oam;
  /*
   *  Opeation mode for statistics interface
   */
  PCP_MGMT_STS_OP_MODE sts;
  /*
   *  IPV4 info
   */
  PCP_MGMT_IPV4_INFO ipv4_info;


} PCP_MGMT_OP_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  RLDRAM memory size. Units: Mbits
   */
  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT size_mbit;

} PCP_MGMT_HW_IF_DRAM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  QDR memory size. Units: Mbits
   */
  PCP_MGMT_HW_IF_QDR_SIZE_MBIT qdr_size_mbit;

} PCP_MGMT_HW_IF_QDR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Equalization Control. Range 0 - 15
   */
  uint32 rx_eq_ctrl;
  /*
   *  Equalizer DC Gain Control. Range 0 - 3
   */
  uint32 rx_eq_dc_gain;
  /*
   *  Pre-tap Pre-emphasis Control. Range 0 - 15
   */
  uint32 tx_preemp_0t;
  /*
   *  1st Post-tap Pre-emphasis Control. Range 0 - 15
   */
  uint32 tx_preemp_1t;
  /*
   *  2nd Post-tap Pre-emphasis Control. Range 0 - 15
   */
  uint32 tx_preemp_2t;
  /*
   *  Voltage Output Differential Control. Range 0 - 7
   */
  uint32 tx_vodctrl;

} PCP_MGMT_HW_IF_SERDES_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable the interface
   */
  uint8 enable;
  /*
   *  Enable/disable Flow Control
   */
  uint8 enable_flow_ctrl;
  /*
   *  SerDes (PHY) information for the ELK Interface
   */
  PCP_MGMT_HW_IF_SERDES_INFO serdes_info;

} PCP_MGMT_HW_IF_ELK_IF_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable the interface
   */
  uint8 enable;
  /*
   *  PCI Express burst size configuration. Maximal burst size
   *  that may be read from the CPU in a single read command.
   */
  uint32 burst_size;

} PCP_MGMT_HW_IF_PCI_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  PCI express configuration
   */
  PCP_MGMT_HW_IF_PCI_INFO pci;
  /*
   *  Dram configuration
   */
  PCP_MGMT_HW_IF_DRAM_INFO dram;
  /*
   *  QDR configuration
   */
  PCP_MGMT_HW_IF_QDR_INFO qdr;
  /*
   *  ELK interface configuration
   */
  PCP_MGMT_HW_IF_ELK_IF_INFO elk_if;

} PCP_HW_ADJUSTMENTS;


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
 *   pcp_register_device
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
  pcp_register_device(
    SOC_SAND_IN  uint32                        *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR      reset_device_ptr,
    SOC_SAND_OUT int                       *unit_ptr
  );

/*********************************************************************
* NAME:
 *   pcp_unregister_device
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
  pcp_unregister_device(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
* NAME:
 *   pcp_mgmt_op_mode_set
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
  pcp_mgmt_op_mode_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE                *info
  );

/*********************************************************************
*     Gets the configuration set by the "pcp_mgmt_op_mode_set"
 *     API.
 *     Refer to "pcp_mgmt_op_mode_set" API for details.
*********************************************************************/
uint32
  pcp_mgmt_op_mode_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_MGMT_OP_MODE                *info
  );

/*********************************************************************
* NAME:
 *   pcp_mgmt_init_sequence_phase1
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
  pcp_mgmt_init_sequence_phase1(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  uint8                       silent
  );

/*********************************************************************
* NAME:
 *   pcp_mgmt_init_sequence_phase2
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
  pcp_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  );

uint32
  pcp_mgmt_memory_test(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  );

void
  PCP_MGMT_ELK_OP_MODE_clear(
    SOC_SAND_OUT PCP_MGMT_ELK_OP_MODE *info
  );

void
  PCP_DMA_THRESHOLD_clear(
    SOC_SAND_OUT PCP_DMA_THRESHOLD *info
  );

void
  PCP_DMA_TX_OP_MODE_clear(
    SOC_SAND_OUT PCP_DMA_TX_OP_MODE *info
  );

void
  PCP_DMA_RX_OP_MODE_clear(
    SOC_SAND_OUT PCP_DMA_RX_OP_MODE *info
  );

void
  PCP_DMA_OP_MODE_clear(
    SOC_SAND_OUT PCP_DMA_OP_MODE *info
  );

void
  PCP_MGMT_OAM_OP_MODE_clear(
    SOC_SAND_OUT PCP_MGMT_OAM_OP_MODE *info
  );

void
  PCP_MGMT_STS_OP_MODE_clear(
    SOC_SAND_OUT PCP_MGMT_STS_OP_MODE *info
  );

void
  PCP_MGMT_OP_MODE_clear(
    SOC_SAND_OUT PCP_MGMT_OP_MODE *info
  );

void
  PCP_MGMT_HW_IF_DRAM_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_DRAM_INFO *info
  );

void
  PCP_MGMT_HW_IF_QDR_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_QDR_INFO *info
  );

void
  PCP_MGMT_HW_IF_SERDES_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_SERDES_INFO *info
  );

void
  PCP_MGMT_HW_IF_ELK_IF_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_ELK_IF_INFO *info
  );

void
  PCP_MGMT_HW_IF_PCI_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_HW_IF_PCI_INFO *info
  );

void
  PCP_HW_ADJUSTMENTS_clear(
    SOC_SAND_OUT PCP_HW_ADJUSTMENTS *info
  );

void
  PCP_MGMT_IPV4_INFO_clear(
    SOC_SAND_OUT PCP_MGMT_IPV4_INFO *info
  );

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_to_string(
    SOC_SAND_IN  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT enum_val
  );

const char*
  PCP_MGMT_HW_IF_QDR_SIZE_MBIT_to_string(
    SOC_SAND_IN  PCP_MGMT_HW_IF_QDR_SIZE_MBIT  enum_val
  );

const char*
  PCP_MGMT_ELK_LKP_MODE_to_string(
    SOC_SAND_IN  PCP_MGMT_ELK_LKP_MODE enum_val
  );

void
  PCP_MGMT_ELK_OP_MODE_print(
    SOC_SAND_IN  PCP_MGMT_ELK_OP_MODE *info
  );

void
  PCP_DMA_THRESHOLD_print(
    SOC_SAND_IN  PCP_DMA_THRESHOLD *info
  );

void
  PCP_DMA_TX_OP_MODE_print(
    SOC_SAND_IN  PCP_DMA_TX_OP_MODE *info
  );

void
  PCP_DMA_RX_OP_MODE_print(
    SOC_SAND_IN  PCP_DMA_RX_OP_MODE *info
  );

void
  PCP_DMA_OP_MODE_print(
    SOC_SAND_IN  PCP_DMA_OP_MODE *info
  );

void
  PCP_MGMT_OAM_OP_MODE_print(
    SOC_SAND_IN  PCP_MGMT_OAM_OP_MODE *info
  );

void
  PCP_MGMT_STS_OP_MODE_print(
    SOC_SAND_IN  PCP_MGMT_STS_OP_MODE *info
  );

void
  PCP_MGMT_OP_MODE_print(
    SOC_SAND_IN  PCP_MGMT_OP_MODE *info
  );

void
  PCP_MGMT_HW_IF_DRAM_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_DRAM_INFO *info
  );

void
  PCP_MGMT_HW_IF_QDR_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_QDR_INFO *info
  );

void
  PCP_MGMT_HW_IF_SERDES_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_SERDES_INFO *info
  );

void
  PCP_MGMT_HW_IF_ELK_IF_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_ELK_IF_INFO *info
  );

void
  PCP_MGMT_HW_IF_PCI_INFO_print(
    SOC_SAND_IN  PCP_MGMT_HW_IF_PCI_INFO *info
  );

void
  PCP_HW_ADJUSTMENTS_print(
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS *info
  );


void
  PCP_MGMT_IPV4_INFO_print(
    SOC_SAND_IN  PCP_MGMT_IPV4_INFO *info
  );

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_API_MGMT_INCLUDED__*/
#endif

