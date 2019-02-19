/* $Id: sweep_pcp_app.h,v 1.3 Broadcom SDK $
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
#ifndef PCP_SWP_APP_H_INCLUDED
/* { */
#define PCP_SWP_APP_H_INCLUDED
#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <soc/dpp/PCP/pcp_api_mgmt.h>

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

typedef struct
{
  uint8 link_level_fc_en;
  uint8 fec_en;
} SWEEP_PCP_FEATURES_LIST;

typedef struct
{
  uint32 pcp_core_freq;
  uint32 pcp_elk_freq;
} SWEEP_PCP_BSP_INTER;


typedef struct
{
 /*
  * Device's chip_id - Should be unique in the system.
  */
  uint32                  system_pcp_id ;

  PCP_MGMT_OP_MODE          operation_mode;

  /*
  * BSP related parameters.
  */
  PCP_HW_ADJUSTMENTS        hw_adjustment;
  /*
   * base address of the pcp device
   */
  uint32                  *base_addr;

  /*
   * Enable / Disable features, according to other devices in the system.
   * For example, link-level flow control / FEC / interleaving...
   */
  SWEEP_PCP_FEATURES_LIST   features_list;

  /*
   *  Physical parameters preset index.
   *  '0' means - no override, '1' - preset 1, etc.
   */
  uint32                  srd_phys_param_preset_id;

  /*
   *    BSP Internal (useful for Bring-up) - pcp synt ref clk
   */
  SWEEP_PCP_BSP_INTER bsp_internal;

  /*
   *    Perform Memory test - check
   */
  uint8 do_memory_test;

} SWEEP_PCP_DEVICE_INIT_PARAMS;


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

/*
 * General functions
 */
uint32
  sweep_pcp_app_unit_get(
    SOC_SAND_OUT int *unit
  );

/*
 * Device Init functions
 */

/*****************************************************
*NAME
*
* sweep_pcp_init_device
*TYPE:
*  PROC
*DATE:
*  12/03/6006
*FUNCTION:
*  Initializes a single PCP device.
*   This includes phase 1 and phase 2 initializations, and initial device configuration
*INPUT:
*  SOC_SAND_DIRECT:
*    SWP_PCP_INIT_PARAMS* pcp_init_params - pointer to the initial parameters structure
*    SOC_SAND_OUT uint32* device_handle - pointer to the device handle
*    uint32 is_hot_start - true if only the software is reset
*    uint32 silent - if true, the prints are disabled,
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 - error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  sweep_pcp_init_device(
    SOC_SAND_IN  SWEEP_PCP_DEVICE_INIT_PARAMS* pcp_init_params,
    SOC_SAND_OUT int* device_handle,
    SOC_SAND_IN  uint32 is_hot_start,
    SOC_SAND_IN  uint32 silent
  );
/*****************************************************
*NAME
*
* sweep_pcp_app_init
*TYPE:
*  PROC
*DATE:
*  12/03/6006
*FUNCTION:
*  Initializes pcp sweep application - sets the application parameter's values
*
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32 is_hot_start - true if only the software is reset
*    uint32 silent - if true, the prints are disabled,
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 - error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  sweep_pcp_app_init(
    uint32 is_hot_start,
    uint32 silent,
    int *p_unit
  );

/*****************************************************
*NAME
* sweep_pcp_app_close
*TYPE:
*  PROC
*DATE:
*  26/02/6006
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent - allow or supress prints
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 - error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  sweep_pcp_app_close(
      SOC_SAND_IN  int    unit,
      SOC_SAND_IN  uint8    silent
  );

/* } */

#ifdef _MSC_VER
  #pragma pack(pop)
#endif


#ifdef  __cplusplus
}
#endif


/* } */
#endif




