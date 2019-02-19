/* $Id: sweep_pcp_bsp_interface_win.c,v 1.9 Broadcom SDK $
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

#include <appl/dpp/sweep/PCP/sweep_pcp_bsp_interface.h>
#if SWEEP_PCP_BSP_INTERFACE == SWEEP_PCP_BSP_INTERFACE_WIN

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#ifdef SAND_LOW_LEVEL_SIMULATION
  #include <sim/dpp/ChipSim/chip_sim_task.h>
  #include <sim/dpp/ChipSim/chip_sim_low.h>
#endif

#include <appl/diag/dpp/utils_line_gfa_bi.h>

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

SOC_SAND_RET
  swp_pcp_bsp_write(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
#ifndef SAND_LOW_LEVEL_SIMULATION
  return soc_sand_eci_write(array, base_address, offset, size);
#else
  VOLATILE uint32
    *ul_ptr;
  const VOLATILE uint32
    *local_buff_ptr ;
  uint32
    ui,
    local_offset,
    local_size;
  unsigned int
    big_endian;
  SOC_SAND_RET
    ex;

  ex = SOC_SAND_OK ;
  big_endian = soc_sand_system_is_big_endian();

#if PRINT_LOW_LEVEL_ACESS
  soc_sand_os_printf("\r\n eci_write() base: 0x%X ; offset: 0x%X ; size %d ; data: 0x%X", base_address, offset, size, *array) ;
#endif
  /*
   * Notice that due to the endian issue this method
   * handles only memory that is comprized of longs
   */
  local_offset  = offset >> 2 ;
  local_size    = size   >> 2 ;
  ul_ptr        = base_address + local_offset ;

  /*
   * Use local pointer
   */
  local_buff_ptr = array;

  for (ui=0 ; ui<local_size ; ++ui)
  {
/* { */
    if ( chip_sim_task_is_alive() )
    {
      if (chip_sim_write(PTR_TO_INT(ul_ptr), *local_buff_ptr) )
      {
#if SOC_SAND_LOW_LEVEL_ERR_MSG
/* { */
        logMsg(
          "soc_sand_physical_write_to_chip()"
          " chip_sim_write(0x%X) returned with an error\r\n",
          ul_ptr,0,0,0,0,0
        ) ;
/* } */
#endif/*SOC_SAND_LOW_LEVEL_ERR_MSG*/
        ex = SOC_SAND_ERR ;
        goto exit ;
      }
      local_buff_ptr++ ;
      ul_ptr++ ;
    }
    else
    {
      ex = SOC_SAND_ERR ;
      goto exit ;
    }
/* } */
  }
  goto exit ;
  /*
   */
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_PHYSICAL_WRITE_TO_CHIP,
        "error in soc_sand_physical_write_to_chip(): Cannot access chip",
         PTR_TO_INT(array), PTR_TO_INT(base_address),
         offset, size, 0,0
  ) ;
  return ex ;
#endif
}

SOC_SAND_RET
  swp_pcp_bsp_read(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
#ifndef SAND_LOW_LEVEL_SIMULATION
  return soc_sand_eci_read(array, base_address, offset, size);
#else
  const VOLATILE uint32
      *ul_ptr ;
  uint32
    ui,
    local_offset,
    local_size,
    *tmp_array ;
  unsigned int
    big_endian;
  SOC_SAND_RET
    ex;

#if TRACE_LOW_LEVEL
  uint32
    before, after ;
  uint32
    elapsed ;
#endif

  ex = SOC_SAND_OK ;
  big_endian = soc_sand_system_is_big_endian();
  tmp_array = array ;

  /*
   * Notice that due to the endian issue this method
   * handles only memory that is comprized of longs
   */
  local_offset  = offset  >> 2 ;
  local_size    = size    >> 2 ;
  ul_ptr        = base_address + local_offset ;
  for (ui = 0 ; ui < local_size ; ui++)
  {
    if ( chip_sim_task_is_alive())
    {
#if TRACE_LOW_LEVEL
/* { */
      before = soc_sand_os_get_time_micro() ;
/* } */
#endif
      if (chip_sim_read(PTR_TO_INT(ul_ptr), (uint32*)array) )
      {
#if SOC_SAND_LOW_LEVEL_ERR_MSG
/* { */
        logMsg(
          "soc_sand_physical_read_from_chip() chip_sim_read(0x%X) returned with an error\r\n",
          ul_ptr,0,0,0,0,0) ;
/* } */
#endif
        ex = SOC_SAND_ERR ;
        goto exit ;
      }
#if TRACE_LOW_LEVEL
/* { */
      after = soc_sand_os_get_time_micro() ;
      elapsed = after - before;
/* } */
#endif
      array++ ;
      ul_ptr++ ;
    }
    else
    {
      ex = SOC_SAND_ERR ;
      goto exit ;
    }
/* } */
  }

#if PRINT_LOW_LEVEL_ACESS
  soc_sand_os_printf("\r\n eci_read() base: 0x%X ; offset: 0x%X ; size %d ; data: 0x%X", base_address, offset, size, *tmp_array) ;
#endif
  goto exit ;
  /*
   */
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_PHYSICAL_READ_FROM_CHIP,
        "error in soc_sand_physical_read_from_chip(): Cannot access chip",
         PTR_TO_INT(array),
         PTR_TO_INT(base_address),
         offset,
         size,0,0) ;
  return ex ;
#endif
}


uint32
  sweep_pcp_bsp_base_address(
    SOC_SAND_IN  uint8 is_csr,
    SOC_SAND_OUT uint32  **base
  )
{
  *base = (uint32*)GFA_BI_PCP_BASE_ADDR;
  return SOC_SAND_OK;
}

uint32
  sweep_pcp_bsp_system_pcp_id_get(
    SOC_SAND_OUT uint32 *system_pcp_id
  )
{
  *system_pcp_id = 0;

  return SOC_SAND_OK;
}

void
  sweep_pcp_bsp_usr_app_flavor_get(
     SOC_D_USR_APP_FLAVOR* usr_app_flavor
  )
{
  *usr_app_flavor = SOC_D_USR_APP_FLAVOR_NORMAL;
}

SOC_SAND_RET
  swp_pcp_bsp_reset(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  remain_active
  )
{
  return SOC_SAND_OK;
}

/*
 * File functions
*/

#ifndef __KERNEL__
uint32
  sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_open(
    SOC_SAND_OUT  FILE              **file_p,
    SOC_SAND_IN   uint8         silent
  )
{
  const char
    *file_name = SWEEP_PCP_DEFAULT_FILE_NAME;
  char msg[SWEEP_PCP_MSG_SIZE];

  SWP_INIT_ERR_DEFS("sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_open");
  COMPILER_REFERENCE(m_silent_flag);

  *file_p = fopen(file_name, "r");
  if(*file_p == NULL)
  {
    sal_sprintf (msg, "Could not open %s", file_name);
    SWP_SET_ERR_AND_EXIT_WITH_MSG(10, msg);
  }

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_close(
    SOC_SAND_OUT  FILE        **file_p,
    SOC_SAND_IN   uint8   silent
  )
{
  uint32
    ret = SOC_SAND_OK;

  if(*file_p)
  {
    fclose(*file_p);
  }

  return ret;
}
#endif /* __KERNEL__ */
/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } */
#endif /* SWEEP_PCP_BSP_INTERFACE == SWEEP_PCP_BSP_INTERFACE_WIN */
