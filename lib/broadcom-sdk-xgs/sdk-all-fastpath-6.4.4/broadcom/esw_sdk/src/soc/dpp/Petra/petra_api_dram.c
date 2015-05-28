/* $Id: petra_api_dram.c,v 1.9 Broadcom SDK $
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
*/
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_dram.c
*
* MODULE PREFIX:  soc_petra_dram
*
* FILE DESCRIPTION:  in the H file.
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_dram.h>
#include <soc/dpp/Petra/petra_dram.h>

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
*     This function configures the dram according to the
*     provided sets of parameters
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dram_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_dram_info_verify(
          unit,
          dram_freq,
          dram_type,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dram_info_set_unsafe(
          unit,
          dram_ndx,
          dram_freq,
          dram_type,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_set()",0,0);
}

/*********************************************************************
*     This function configures the dram according to the
*     provided sets of parameters
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dram_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_OUT SOC_PETRA_DRAM_TYPE           *dram_type,
    SOC_SAND_OUT SOC_PETRA_DRAM_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dram_info_get_unsafe(
          unit,
          dram_ndx,
          dram_freq,
          dram_type,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_get()",0,0);
}

/*********************************************************************
*     Diagnostic tool to get the DRAM valid window size
 *     through BIST commands. .
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dram_window_validity_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN                        *pattern,
    SOC_SAND_OUT SOC_PETRA_DRAM_WINDOW_INFO                    *window_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_WINDOW_VALIDITY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pattern);
  SOC_SAND_CHECK_NULL_INPUT(window_info);

  res = soc_petra_dram_window_validity_get_verify(
          unit,
          dram_ndx,
          pattern
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dram_window_validity_get_unsafe(
          unit,
          dram_ndx,
          pattern,
          window_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dram_window_validity_get()", dram_ndx, 0);
}

void
  soc_petra_PETRA_DRAM_MR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DRAM_MODE_REGS_INFO  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(info, 0x0, sizeof(SOC_PETRA_DRAM_MODE_REGS_INFO));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DRAM_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DRAM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(info, 0x0, sizeof(SOC_PETRA_DRAM_INFO));
  soc_petra_PETRA_DRAM_MR_INFO_clear(&info->mode_regs);
  info->auto_mode = TRUE;
  info->t_rc = 0;
  info->t_rfc = 0;
  info->t_ras = 0;
  info->t_faw = 0;
  info->t_rcd_rd = 0;
  info->t_rcd_wr = 0;
  info->t_rrd = 0;
  info->t_ref = 0;
  info->t_rp = 0;
  info->t_wr = 0;
  info->t_wtr = 0;
  info->c_cas_latency = 0;
  info->c_wr_latency = 0;
  info->t_rtp = 0;
  info->burst_size = SOC_PETRA_DRAM_BURST_SIZE_32;
  info->nof_banks = SOC_PETRA_DRAM_NUM_BANKS_4;
  info->nof_cols = SOC_PETRA_NOF_DRAM_NUMS_COLUMNS;
  info->nof_cols = SOC_PETRA_NOF_DRAM_AP_POSITIONS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DRAM_PATTERN_clear(
    SOC_SAND_OUT SOC_PETRA_DRAM_PATTERN *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DRAM_PATTERN));
  info->mode = SOC_PETRA_DRAM_NOF_PATTERN_MODES;
  for (ind = 0; ind < SOC_PETRA_DRAM_PATTERN_SIZE_IN_UINT32S; ++ind)
  {
    info->data[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DRAM_WINDOW_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DRAM_WINDOW_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DRAM_WINDOW_INFO));
  info->start = 0;
  info->end = 0;
  info->sampling_point = 0;
  info->total_size = 0;
  info->ratio_percent = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_DRAM_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DRAM_TYPE_GDDR3:
    str = "SOC_PETRA_DRAM_TYPE_GDDR3";
    break;
  case SOC_PETRA_DRAM_TYPE_DDR2:
    str = "SOC_PETRA_DRAM_TYPE_DDR2";
    break;
  case SOC_PETRA_DRAM_TYPE_DDR3:
    str = "SOC_PETRA_DRAM_TYPE_DDR3";
    break;

  case SOC_PETRA_DRAM_NOF_TYPES:
    str = "SOC_PETRA_DRAM_NOF_TYPES";
  break;

  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_petra_PETRA_DRAM_BURST_SIZE_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_BURST_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DRAM_BURST_SIZE_16:
    str = "SOC_PETRA_DRAM_BURST_SIZE_16";
  break;
  case SOC_PETRA_DRAM_BURST_SIZE_32:
    str = "SOC_PETRA_DRAM_BURST_SIZE_32";
  break;

  case SOC_PETRA_DRAM_NOF_BURST_SIZES:
    str = "SOC_PETRA_DRAM_NOF_BURST_SIZES";
  break;

  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_petra_PETRA_DRAM_NUM_BANKS_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_NUM_BANKS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DRAM_NUM_BANKS_4:
    str = "DRAM_BANKS_4";
  break;

  case SOC_PETRA_DRAM_NUM_BANKS_8:
    str = "DRAM_BANKS_8";
  break;

  case SOC_PETRA_NOF_DRAM_NUM_BANKS:
    str = "SOC_PETRA_NOF_DRAM_NUM_BANKS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_DRAM_AP_POSITION_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_AP_POSITION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DRAM_AP_POSITION_08:
    str = "08";
  break;

  case SOC_PETRA_DRAM_AP_POSITION_09:
    str = "09";
  break;

  case SOC_PETRA_DRAM_AP_POSITION_10:
    str = "10";
  break;

  case SOC_PETRA_DRAM_AP_POSITION_11:
    str = "11";
  break;

  case SOC_PETRA_DRAM_AP_POSITION_12:
    str = "12";
  break;

  case SOC_PETRA_NOF_DRAM_AP_POSITIONS:
    str = "SOC_PETRA_NOF_DRAM_AP_POSITIONS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_DRAM_PATTERN_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DRAM_PATTERN_MODE_AUTO:
    str = "auto";
  break;
  case SOC_PETRA_DRAM_PATTERN_MODE_USER_DEF:
    str = "user_def";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_petra_PETRA_DRAM_NUM_COLUMNS_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_NUM_COLUMNS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DRAM_NUM_COLUMNS_256:
    str = "256";
  break;

  case SOC_PETRA_DRAM_NUM_COLUMNS_512:
    str = "512";
  break;

  case SOC_PETRA_DRAM_NUM_COLUMNS_1024:
    str = "1024";
  break;

  case SOC_PETRA_DRAM_NUM_COLUMNS_2048:
    str = "2048";
  break;

  case SOC_PETRA_DRAM_NUM_COLUMNS_4096:
    str = "4096";
  break;

  case SOC_PETRA_DRAM_NUM_COLUMNS_8192:
    str = "8192";
  break;

  case SOC_PETRA_NOF_DRAM_NUMS_COLUMNS:
    str = "SOC_PETRA_NOF_DRAM_NUMS_COLUMNS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  soc_petra_PETRA_DRAM_MR_INFO_print(
    SOC_SAND_IN SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN SOC_PETRA_DRAM_MODE_REGS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (dram_type == SOC_PETRA_DRAM_TYPE_DDR2)
  {
    soc_sand_os_printf("mrs0_wr1: %x\n\r",info->ddr2.mrs0_wr1);
    soc_sand_os_printf("mrs0_wr2: %x\n\r",info->ddr2.mrs0_wr2);
    soc_sand_os_printf("emr0_wr1: %x\n\r",info->ddr2.emr0_wr1);
    soc_sand_os_printf("emr0_wr2: %x\n\r",info->ddr2.emr0_wr2);
    soc_sand_os_printf("emr0_wr3: %x\n\r",info->ddr2.emr0_wr3);
    soc_sand_os_printf("emr1_wr1: %x\n\r",info->ddr2.emr1_wr1);
    soc_sand_os_printf("emr2_wr1: %x\n\r",info->ddr2.emr2_wr1);
  }
  if (dram_type == SOC_PETRA_DRAM_TYPE_DDR3)
  {
    soc_sand_os_printf("mrs0_wr1: %x\n\r",info->ddr3.mrs0_wr1);
    soc_sand_os_printf("mrs0_wr2: %x\n\r",info->ddr3.mrs0_wr2);
    soc_sand_os_printf("mrs1_wr1: %x\n\r",info->ddr3.mrs1_wr1);
    soc_sand_os_printf("mrs2_wr1: %x\n\r",info->ddr3.mrs2_wr1);
    soc_sand_os_printf("mrs3_wr1: %x\n\r",info->ddr3.mrs3_wr1);
  }
  if (dram_type == SOC_PETRA_DRAM_TYPE_GDDR3)
  {
    soc_sand_os_printf("mrs0_wr1: %x\n\r",info->gdd3.mrs0_wr1);
    soc_sand_os_printf("emr0_wr1: %x\n\r",info->gdd3.emr0_wr1);
  }
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DRAM_INFO_print(
    SOC_SAND_IN SOC_PETRA_DRAM_TYPE dram_type,
    SOC_SAND_IN SOC_PETRA_DRAM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("c_cas_latency: %u\n\r",info->c_cas_latency);
  soc_sand_os_printf("c_wr_latency: %u\n\r",info->c_wr_latency);
  soc_sand_os_printf("burst_size: %s\n\r",soc_petra_PETRA_DRAM_BURST_SIZE_to_string(info->burst_size));
  soc_sand_os_printf("nof_banks: %s\n\r",soc_petra_PETRA_DRAM_NUM_BANKS_to_string(info->nof_banks));
  soc_sand_os_printf("nof_cols: %s\n\r",soc_petra_PETRA_DRAM_NUM_COLUMNS_to_string(info->nof_cols));
  soc_sand_os_printf("nof_cols: %s\n\r",soc_petra_PETRA_DRAM_AP_POSITION_to_string(info->ap_bit_pos));
  soc_sand_os_printf("t_rc: %u\n\r",info->t_rc);
  soc_sand_os_printf("t_rfc: %u\n\r",info->t_rfc);
  soc_sand_os_printf("t_ras: %u\n\r",info->t_ras);
  soc_sand_os_printf("t_faw: %u\n\r",info->t_faw);
  soc_sand_os_printf("t_rcd_rd: %u\n\r",info->t_rcd_rd);
  soc_sand_os_printf("t_rcd_wr: %u\n\r",info->t_rcd_wr);
  soc_sand_os_printf("t_rrd: %u\n\r",info->t_rrd);
  soc_sand_os_printf("t_ref: %u\n\r",info->t_ref);
  soc_sand_os_printf("t_rp: %u\n\r",info->t_rp);
  soc_sand_os_printf("t_wr: %u\n\r",info->t_wr);
  soc_sand_os_printf("t_wtr: %u\n\r",info->t_wtr);
  soc_sand_os_printf("t_rtp: %u\n\r",info->t_rtp);
  soc_petra_PETRA_DRAM_MR_INFO_print(dram_type, &info->mode_regs);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DRAM_PATTERN_print(
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mode %s ", soc_petra_PETRA_DRAM_PATTERN_MODE_to_string(info->mode));
  for (ind = 0; ind < SOC_PETRA_DRAM_PATTERN_SIZE_IN_UINT32S; ++ind)
  {
    soc_sand_os_printf("data[%u]: %u\n\r",ind,info->data[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DRAM_WINDOW_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DRAM_WINDOW_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("start: %u[tap]\n\r",info->start);
  soc_sand_os_printf("end: %u[tap]\n\r",info->end);
  soc_sand_os_printf("sampling_point: %u[tap]\n\r",info->sampling_point);
  soc_sand_os_printf("bit_time_in_taps: %u[taps]\n\r",info->total_size);
  soc_sand_os_printf("ratio_percent: %u[%%]\n\r",info->ratio_percent);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*********************************************************************
*     This diagnostics function reads and prints logical address, 
*     physical address and contents of a packet
*     in the given DRAM buffer. The size of the packet inside the buffer
*     must also be provided
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dram_pckt_from_buff_read(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  buff_id,
    SOC_SAND_IN uint32  pckt_size_in_buff
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_PCKT_FROM_BUFF_READ);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_dram_pckt_from_buff_verify(
          unit,
          buff_id,
          pckt_size_in_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dram_pckt_from_buff_read_unsafe(
          unit,
          buff_id,
          pckt_size_in_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_set()",0,0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

