/* $Id: sand_device_management.h,v 1.5 Broadcom SDK $
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
 * $
*/


#ifndef __SOC_SAND_DEVICE_MANAGEMENT_H_INCLUDED__
/* { */
#define __SOC_SAND_DEVICE_MANAGEMENT_H_INCLUDED__
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>

/* $Id: sand_device_management.h,v 1.5 Broadcom SDK $
 * Version Information structure.
 * For inner driver usage - not to be used out side the driver.
 * This structure is for passing parameters to 'soc_sand_device_register()'.
 * {
 */

typedef struct
{
  /*
   * Version register offset
   */
  uint32  ver_reg_offset;

  /*
   * Chip-Type info.
   */

  SOC_SAND_DEVICE_TYPE logic_chip_type;
  uint32    chip_type;
  uint32    chip_type_shift;
  uint32    chip_type_mask;

  /*
   * Debug-Version info.
   */
  uint32  dbg_ver_shift;
  uint32  dbg_ver_mask;

  /*
   * Chip-Version info.
   */
  uint32  chip_ver_shift;
  uint32  chip_ver_mask;

  /* 
   * BCM only: skip verification when CMIC
   */
  uint8 cmic_skip_verif;

} SOC_SAND_DEV_VER_INFO;

extern uint8 Soc_register_with_id;

void
  soc_sand_clear_SAND_DEV_VER_INFO(
    SOC_SAND_OUT SOC_SAND_DEV_VER_INFO* ver_info
  );
/*
 * }
 */


SOC_SAND_RET
  soc_sand_device_register_with_id(SOC_SAND_IN uint8 enable);

SOC_SAND_RET
  soc_sand_device_register(
    uint32                 *base_address,
    uint32                 mem_size, /*in bytes*/
    SOC_SAND_UNMASK_FUNC_PTR               unmask_func_ptr,
    SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR    is_bit_ac_func_ptr,
    SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED   is_dev_int_mask_func_ptr,
    SOC_SAND_GET_DEVICE_INTERRUPTS_MASK    get_dev_mask_func_ptr,
    SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE mask_specific_interrupt_cause_ptr,
    SOC_SAND_RESET_DEVICE_FUNC_PTR    reset_device_ptr,
    SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR is_read_write_protect_ptr,
    SOC_SAND_DEV_VER_INFO             *ver_info,
    SOC_SAND_INDIRECT_MODULE          *indirect_module,
    uint32                 interrupt_mask_address,
    int                  *unit_ptr
  );

SOC_SAND_RET
  soc_sand_device_unregister(
    int unit
  ) ;

/*
* Return the chip definitions.
*/
SOC_SAND_RET
  soc_sand_get_device_type(
    int      unit,
    SOC_SAND_DEVICE_TYPE *chip_type,
    uint32    *chip_ver,
    uint32    *dbg_ver
  ) ;
#ifdef SOC_SAND_DEBUG


/*
 * Printing utility.
 * Convert from enumerator to string.
 */
const char*
  soc_sand_DEVICE_TYPE_to_str(
    SOC_SAND_IN SOC_SAND_DEVICE_TYPE dev_type
  );
#endif


#ifdef  __cplusplus
}
#endif

/* } __SOC_SAND_DEVICE_MANAGEMENT_H_INCLUDED__*/
#endif
