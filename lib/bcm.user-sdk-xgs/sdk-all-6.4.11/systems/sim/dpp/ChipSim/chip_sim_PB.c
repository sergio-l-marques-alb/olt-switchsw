/* $Id: chip_sim_PB.c,v 1.9 Broadcom SDK $
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
* FILENAME:       chip_sim_PB.c
*
* MODULE PREFIX:  chip_sim
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


#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__ 
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else 
    #error  "Add your system support for packed attribute." 
#endif

#if LINK_PB_LIBRARIES

/*************
 * INCLUDES  *
 *************/
/* { */
#include "chip_sim.h" 
#include "chip_sim_counter.h" 
#include "chip_sim_interrupts.h" 
#include "chip_sim_indirect.h" 
#include "chip_sim_PB.h" 
#include "chip_sim_PB_TM.h" 
#include "chip_sim_PB_PP.h" 
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h> 
#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h> 
#include <soc/dpp/Petra/petra_chip_defines.h> 
#include <soc/dpp/SAND/Management/sand_general_macros.h> 
#include <soc/dpp/SAND/Management/sand_error_code.h> 
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
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
CHIP_SIM_INDIRECT_BLOCK
  Soc_pb_indirect_blocks[SOC_PB_PP_TBL_ID_LAST + 1];
CHIP_SIM_COUNTER
  Soc_pb_counters[2];
CHIP_SIM_INTERRUPT
  Soc_pb_interrupts[1];
uint8
  Soc_pb_init_device_values = TRUE;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*****************************************************
*NAME
* soc_pb_indirect_blocks_init
*TYPE:
*  PROC
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  soc_pb_indirect_blocks_init(
  )
{
  soc_pb_tm_indirect_blocks_init();
  soc_pb_pp_indirect_blocks_init();
}
/*****************************************************
*NAME
* soc_pb_indirect_counter_init
*TYPE:
*  PROC
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  soc_pb_indirect_counter_init(
  )
{

  Soc_pb_counters[0].chip_offset = INVALID_ADDRESS; 
  Soc_pb_counters[0].mask = 0xFFFFFFFF;
  Soc_pb_counters[0].shift = 0;
  Soc_pb_counters[0].is_random = FALSE;
  Soc_pb_counters[0].count_per_sec = 1000;
  Soc_pb_counters[0].min = 0;
  Soc_pb_counters[0].max = 0xFFFFFFFF;

  Soc_pb_counters[1].chip_offset = INVALID_ADDRESS;
}

/*****************************************************
*NAME
* soc_pb_indirect_interrupt_init
*TYPE:
*  PROC
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  soc_pb_indirect_interrupt_init(
  )

{
    Soc_pb_interrupts[0].int_chip_offset = INVALID_ADDRESS;
}
/*****************************************************
*NAME
* soc_pb_indirect_init
*TYPE:
*  PROC
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
  soc_pb_indirect_init(
  )
{
   soc_pb_indirect_blocks_init();
   soc_pb_indirect_counter_init();
   soc_pb_indirect_interrupt_init();
}
/*****************************************************
*NAME
* soc_pb_em_init
*TYPE:
*  PROC
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    error indication
*  EM:
*    em tables
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
soc_pb_em_init(
             )
{
}

void
  soc_pb_initialize_device_values(
    SOC_SAND_OUT uint32   *base_ptr
  )
{
#if 0 /* Not in use in BCM */
    FILE*
    file_p;
  uint32
    addr, 
    value;

  if (!Soc_pb_init_device_values)
  {
    goto exit;
  }

  if((file_p = fopen("soc_pb_reg_file.txt", "r")) == NULL)
  {
    goto exit;
  }

  while (2 == fscanf(file_p, "0x%04X: 0x%08X\n", &addr, &value))
  {
    base_ptr[addr] = value;
  }

  fclose(file_p);

exit:
  return;
#endif
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* LINK_PB_LIBRARIES */
