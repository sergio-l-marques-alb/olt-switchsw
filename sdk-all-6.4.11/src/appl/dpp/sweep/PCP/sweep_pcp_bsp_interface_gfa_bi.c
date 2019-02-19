/* $Id: sweep_pcp_bsp_interface_gfa_bi.c,v 1.11 Broadcom SDK $
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
#if SWEEP_PCP_BSP_INTERFACE == SWEEP_PCP_BSP_INTERFACE_GFA_BI

/*************
 * INCLUDES  *
 *************/
/* { */
#ifndef __KERNEL__
#include <stdio.h>
#include <stdlib.h>
#endif

#if !DUNE_BCM
#include <soc/dpp/../../h/usrApp.h>
#include <TempWinLib/chassis.h>
#endif

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#ifndef __DUNE_LINUX_BCM_CPU_PCIE__
#include <soc/i2c.h>
#endif

#include <appl/diag/dpp/utils_line_gfa_petra.h>
#include <appl/diag/dpp/utils_line_gfa_bi.h>

/* #include <TempWinLib/chassis.h> */

#include <appl/dpp/sweep/PCP/sweep_pcp_app.h>

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


/*
 *  TX parameters preset: tuned for 5_000Gbps SerDes
 */


/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC SOC_SAND_RET
  swp_p_tm_type_PETRA_MGMT_SRD_REF_CLK_convert(
    SOC_SAND_IN     SOC_PETRA_MGMT_SRD_REF_CLK        ref_clk_enum,
    SOC_SAND_OUT    uint32                     *value_hz
  )
{
  SWP_INIT_ERR_DEFS("swp_p_tm_type_PETRA_MGMT_SRD_REF_CLK_convert");

  switch(ref_clk_enum)
  {
  case SOC_PETRA_MGMT_SRD_REF_CLK_125_00:
    *value_hz = 125000000;
    break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_156_25:
    *value_hz = 156250000;
    break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_200_00:
    *value_hz = 200000000;
    break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_312_50:
    *value_hz = 312500000;
    break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_218_75:
    *value_hz = 218750000;
    break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_212_50:
    *value_hz = 212500000;
    break;

  default:
    SWP_SET_ERR_AND_EXIT_WITH_MSG(10, "Invalid frequency");
 }

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  soc_swp_p_tm_bsp_internal_set_single(
    SOC_SAND_IN GFA_TG_SYNT_TYPE   targetsynt,
    SOC_SAND_IN uint32           targetfreq,
    SOC_SAND_IN uint8          allow_enum,
    SOC_SAND_IN  uint8         silent
    )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    targetfreq_actual,
    nominal_freq;

  SWP_INIT_ERR_DEFS("soc_swp_p_tm_bsp_internal_set_single");

  targetfreq_actual = 0;

  if (allow_enum && (targetfreq != SOC_PETRA_MGMT_NOF_SRD_REF_CLKS))
  {
    if (targetfreq < SOC_PETRA_MGMT_NOF_SRD_REF_CLKS)
    {
      ret = swp_p_tm_type_PETRA_MGMT_SRD_REF_CLK_convert(
              targetfreq,
              &targetfreq_actual
            );
      SWP_EXIT_IF_ERR(ret, 10);
    }
    else
    {
      targetfreq_actual = targetfreq;
    }
  }
  else
  {
    targetfreq_actual = targetfreq;
  }
  
  if (targetfreq_actual != 0)
  {
    /* In the following lines we assume that the current rate of the synt is
       the nominal rate. This means that this function should be called only
       once after board init. OW, this assumption is not correct */

    nominal_freq = gfa_petra_board_synt_nominal_freq_get(targetsynt);

    if (nominal_freq != targetfreq_actual)
    {
      ret = gfa_petra_board_synt_set(
              targetsynt,
              targetfreq_actual,
              silent
            );
      SWP_EXIT_IF_ERR(ret, 20);
    }
  }

exit:
  SWP_EXIT_AND_PRINT_ERR;
}


#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
extern int _cpu_pci_register(int d);
#endif

SOC_SAND_RET
  swp_pcp_bsp_write(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
#ifndef __DUNE_GTO_BCM_CPU__
  if (base_address == (uint32*)GFA_BI_PETRA_BASE_ADDR)
  {
    soc_sand_os_printf("soc_petra write eci. base_address=0x%x, offset=0x%x, size=%d\n",base_address, offset, size);
    return soc_sand_eci_write(array, base_address, offset, size);
  }
  else if (base_address == (uint32*)GFA_BI_PCP_BASE_ADDR)
  {
    soc_sand_os_printf("pcp write i2c. base_address=0x%x, offset=0x%x, size=%d\n",base_address, offset, size);
    return gfa_bi_bsp_i2c_write_gen(GFA_BI_I2C_PCP_DEVICE_ADDR, (uint8 *)&offset, sizeof(uint32), (uint8 *)array, (uint8)size);
  }
 
  soc_sand_os_printf("pcp write ERROR!!!. Bad base_address=0x%x, offset=0x%x, size=%d\n",base_address, offset, size);
  return SOC_SAND_ERR;
#else
   SOC_SAND_RET
		ret = SOC_SAND_OK;
	 int 
		i,
		local_size;

	 local_size = size >> 2 ;
	
	 for (i=0;i<local_size;i++)
	 {
		 CMVEC(0).write(&CMDEV(0).dev, (offset + (i * 4)), (uint32)*(array + i));
	 }
	 return ret;
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
#ifndef __DUNE_GTO_BCM_CPU__
  uint16
    read_data_len;

  if (base_address == (uint32*)GFA_BI_PETRA_BASE_ADDR)
  {
    soc_sand_os_printf("soc_petra read eci. base_address=0x%x, offset=0x%x, size=%d\n",base_address, offset, size);
    return soc_sand_eci_read(array, base_address, offset, size);
  }
  else if (base_address == (uint32*)GFA_BI_PCP_BASE_ADDR)
  {
    soc_sand_os_printf("pcp read i2c. base_address=0x%x, offset=0x%x, size=%d\n",base_address, offset, size);
    return gfa_bi_bsp_i2c_read_gen(GFA_BI_I2C_PCP_DEVICE_ADDR, (uint8 *)&offset, sizeof(uint32), sizeof(uint32), array, &read_data_len);
  }
  
  soc_sand_os_printf("pcp read ERROR!!!. Bad base_address=0x%x, offset=0x%x, size=%d\n",base_address, offset, size);
  return SOC_SAND_ERR;
#else
    	SOC_SAND_RET
		ret = SOC_SAND_OK;
	int 
		i,
		local_size;

	local_size = size >> 2 ;
	
	for (i=0;i<local_size;i++)
	{
		*(array + i) = CMVEC(0).read(&CMDEV(0).dev, (offset + (i * 4)));
	}
	return ret;
#endif
}

uint32
  sweep_pcp_bsp_base_address(
    SOC_SAND_IN  uint8 is_csr,
    SOC_SAND_OUT uint32  **base
  )
{
  *base = (uint32 *)0; /* should be: GFA_BI_PCP_BASE_ADDR but currently in bcm unit=base_address */;
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
#ifndef __DUNE_GTO_BCM_CPU__
  d_usr_app_flavor_get(
    usr_app_flavor
  );
#endif
}

SOC_SAND_RET
  swp_pcp_bsp_internal_set(
    SOC_SAND_IN  SWEEP_PCP_BSP_INTER  info,
    SOC_SAND_IN  uint8            silent
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  SWP_INIT_ERR_DEFS("swp_pcp_bsp_internal_set");
#ifndef DUNE_STANDALONE_PACKAGE
  ret = soc_swp_p_tm_bsp_internal_set_single(GFA_PETRA_SYNT_TYPE_PCP_CORE, info.pcp_core_freq, FALSE, silent);
  SWP_EXIT_IF_ERR(ret, 20);

  ret = soc_swp_p_tm_bsp_internal_set_single(GFA_PETRA_SYNT_TYPE_PCP_ELK, info.pcp_elk_freq, FALSE, silent);
  SWP_EXIT_IF_ERR(ret, 30);
#endif
exit:
  SWP_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  swp_pcp_bsp_reset(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  remain_active
  )
{
  return SOC_SAND_OK;
}

SOC_SAND_RET
  swp_pcp_bsp_device_out_of_reset(
    SOC_SAND_IN SWEEP_PCP_DEVICE_INIT_PARAMS  device_init_params
  )
{
  SOC_SAND_RET
    ret;

  SWP_INIT_ERR_DEFS("swp_pcp_bsp_device_out_of_reset");

  /************************************************************************/
  /* Set in-reset                                                         */
  /************************************************************************/
  ret = gfa_bi_fpga_pcp_hw_reset(TRUE);
  SWP_EXIT_IF_ERR(ret, 10);

  /*
   * The minimal period for Core PLL to be in-reset
   * is 20 milliseconds.
   */
  soc_sand_os_task_delay_milisec(50);

  ret = swp_pcp_bsp_internal_set(
            device_init_params.bsp_internal,
            FALSE
           );
  SWP_EXIT_IF_ERR(ret, 20);

  /************************************************************************/
  /* Set out-of-reset                                                     */
  /************************************************************************/

  ret = gfa_bi_fpga_pcp_hw_reset(FALSE);
  SWP_EXIT_IF_ERR(ret, 30);

  /*
   * The minimal period between Core PLL and Soc_petra OOR
   * is 100 milliseconds.
   */
  soc_sand_os_task_delay_milisec(100);

#ifdef __DUNE_LINUX_BCM_CPU_PCIE__ 
  soc_sand_os_task_delay_milisec(50); 
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
  _cpu_pci_register(0);
#endif
  soc_sand_os_task_delay_milisec(100); /* delay for pcie to configure */
#endif
 
exit:
  SWP_EXIT_AND_PRINT_ERR;
}

/*
 * File functions
*/
#ifndef __KERNEL__
#ifndef __DUNE_GTO_BCM_CPU__
uint32
  sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_open(
    SOC_SAND_OUT  FILE        **file_p,
    SOC_SAND_IN   uint8   silent
  )
{
  uint32
    ret = SOC_SAND_OK;
  uint8
    delete_cli_file = FALSE;
  const char
    *file_name = SWEEP_PCP_DEFAULT_FILE_NAME;

  if (!silent)
  {
    soc_sand_os_printf(
      "Trying to download file '%s' from the TFTP server:\n\r",
      file_name
    );
  }
  delete_cli_file = TRUE;

  *file_p = fopen(file_name, "r");
  if(*file_p == NULL)
  {
    if (!silent)
    {
      soc_sand_os_printf (
        "Could not open %s\n\r",
        file_name
      );
    }
    ret = SOC_SAND_ERR;
    goto exit;
  }

exit:
  return ret;
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
#else
uint32
  sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_open(
    SOC_SAND_OUT  FILE        **file_p,
    SOC_SAND_IN   uint8   silent
  )
{
  uint32
    ret = SOC_SAND_OK;
 const char
    *file_name = SWEEP_PCP_DEFAULT_FILE_NAME;
 
  *file_p = fopen(file_name, "r");
  if(*file_p == NULL)
  {
    if (!silent)
    {
      soc_sand_os_printf (
        "Could not open %s\n\r",
        file_name
      );
    }
    ret = SOC_SAND_ERR;
    goto exit;
  }

exit:
  return ret;
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

#endif
#endif /* __KERNEL__ */

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } */
#endif /* SWEEP_PCP_BSP_INTERFACE == SWEEP_PCP_BSP_INTERFACE_GFA_BI */
