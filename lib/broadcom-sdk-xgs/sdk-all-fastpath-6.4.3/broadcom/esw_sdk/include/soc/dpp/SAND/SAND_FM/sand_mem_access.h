/* $Id: sand_mem_access.h,v 1.5 Broadcom SDK $
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
 * $
*/


#ifndef __SOC_SAND_MEM_ACCESS_H_INCLUDED__
/* { */
#define __SOC_SAND_MEM_ACCESS_H_INCLUDED__
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>


extern uint32
  Soc_interrupt_mask_address_arr[SOC_SAND_MAX_DEVICE] ;
extern uint32
  Soc_sand_mem_check_read_write_protect;


SOC_SAND_RET
  soc_sand_mem_read(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint32    *result_ptr,
    SOC_SAND_IN  uint32    offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     indirect
  ) ;

SOC_SAND_RET
  soc_sand_mem_read_unsafe(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint32    *result_ptr,
    SOC_SAND_IN  uint32    offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     indirect
  );


SOC_SAND_RET
  soc_sand_mem_write(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 *result_ptr,
    SOC_SAND_IN uint32 offset,
    SOC_SAND_IN uint32  size,
    SOC_SAND_IN uint32  indirect
  ) ;



SOC_SAND_RET
  soc_sand_mem_write_unsafe(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 *result_ptr,
    SOC_SAND_IN uint32 offset,
    SOC_SAND_IN uint32  size,
    SOC_SAND_IN uint32  indirect
  );


/* $Id: sand_mem_access.h,v 1.5 Broadcom SDK $
 * {
 * Field access function. Uses soc_sand_mem_read/soc_sand_mem_write to read/write/update
 * specific field in a register.
 */

/*****************************************************
 * See details in soc_sand_mem_access.c
 *****************************************************/
SOC_SAND_RET
  soc_sand_read_field(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_OUT uint32* data
  );

SOC_SAND_RET
  soc_sand_read_field_int(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_OUT uint32* data
  );

SOC_SAND_RET
  soc_sand_read_field_ex(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_IN  uint32  indirect,
    SOC_SAND_OUT uint32* data
  );

/*****************************************************
 * See details in soc_sand_mem_access.c
 *****************************************************/
SOC_SAND_RET
  soc_sand_write_field(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_IN  uint32 data_to_write
  );
/*****************************************************
 * See details in soc_sand_mem_access.c
 *****************************************************/
SOC_SAND_RET
  soc_sand_write_field_ex(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_IN  uint32  indirect,
    SOC_SAND_IN  uint32 data_to_write
  );

/*
 * }
 */


/*
 * Set the interrupt mask address of the device.
 */
SOC_SAND_RET
  soc_sand_mem_interrupt_mask_address_set(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 interrupt_mask_address
  ) ;

/*
 * Get the interrupt mask address of the device.
 */
uint32
  soc_sand_mem_interrupt_mask_address_get(
    SOC_SAND_IN int  unit
  ) ;

/*
 * Clears unit interrupt mask address information.
 */
SOC_SAND_RET
  soc_sand_mem_interrupt_mask_address_clear(
    SOC_SAND_IN int  unit
  ) ;

/*
 * Clears all devices interrupt mask address information.
 */
SOC_SAND_RET
  soc_sand_mem_interrupt_mask_address_clear_all(
    void
  );

/*
 * Check read/write protect mechanism
 * {
 */


/*****************************************************
 * See details in soc_sand_mem_access.c
 *****************************************************/
uint32
  soc_sand_mem_check_read_write_protect_get(
    void
  );

/*****************************************************
 * See details in soc_sand_mem_access.c
 *****************************************************/
void
  soc_sand_mem_check_read_write_protect_set(
    SOC_SAND_IN uint32 read_write_protect
  );


#if SOC_SAND_DEBUG

#define SOC_SAND_ADDRESS_ARRAY_MAX_SIZE       0xFFFF
#define SOC_SAND_MAX_NOF_INDIRECT_TABLES      0xFF

#define SOC_SAND_DONT_PRINT_ZEROS SOC_SAND_BIT(0)
#define SOC_SAND_PRINT_BITS       SOC_SAND_BIT(1)
#define SOC_SAND_PRINT_RANGES     SOC_SAND_BIT(2)

#define SOC_SAND_PRINT_START_ADDR_LSB   (3)
#define SOC_SAND_PRINT_START_ADDR_MSB   (3+16)

/*****************************************************
 * See details in soc_sand_mem_access.c
 *****************************************************/
SOC_SAND_RET
  soc_sand_print_registers_array(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 address_array[SOC_SAND_ADDRESS_ARRAY_MAX_SIZE],
    SOC_SAND_IN uint32 nof_address,
    SOC_SAND_IN uint32 print_options_bm
  );

/*****************************************************
 * See details in soc_sand_mem_access.c
 *****************************************************/
SOC_SAND_RET
  soc_sand_print_block(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 address_array[SOC_SAND_ADDRESS_ARRAY_MAX_SIZE],
    SOC_SAND_IN uint32 nof_address,
    SOC_SAND_IN uint32 print_options_bm
  );

/*****************************************************
 * See details in soc_sand_mem_access.c
 *****************************************************/
typedef struct{
  uint32 addr;
  uint32 size;
  const char    *name;
}SOC_SAND_INDIRECT_PRINT_INFO;


typedef SOC_SAND_INDIRECT_PRINT_INFO indirect_print_info; /* Backward compatibility */

SOC_SAND_RET
  soc_sand_print_indirect_tables(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_SAND_INDIRECT_PRINT_INFO print_info[SOC_SAND_MAX_NOF_INDIRECT_TABLES],
    SOC_SAND_IN uint32            print_options_bm
  );

#endif
/*
 * }
 */



#ifdef  __cplusplus
}
#endif

/* } __SOC_SAND_MEM_ACCESS_H_INCLUDED__*/
#endif
