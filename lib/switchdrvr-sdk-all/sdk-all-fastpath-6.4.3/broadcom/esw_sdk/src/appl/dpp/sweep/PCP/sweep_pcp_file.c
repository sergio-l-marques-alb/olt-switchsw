/* $Id: sweep_pcp_file.c,v 1.12 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#ifndef __KERNEL__
#include <stdio.h>
#include <string.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <appl/dpp/sweep/PCP/sweep_pcp_app.h>
#include <appl/dpp/sweep/PCP/sweep_pcp_bsp_interface.h>

#include <soc/dpp/PCP/pcp_general.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SWEEP_PCP_MAX_LINE_SIZE  (200)
#define SWEEP_PCP_MSG_SIZE 80

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

static
  uint32 sweep_pcp_register_counter = 0;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  sweep_pcp_param_name_read(
    SOC_SAND_IN    char line[SWEEP_PCP_MAX_LINE_SIZE],
    SOC_SAND_INOUT char param_name[SWEEP_PCP_MAX_LINE_SIZE]
  )
{
  uint32
    char_i;

  SWP_INIT_ERR_DEFS("sweep_pcp_param_name_read");
  COMPILER_REFERENCE(m_silent_flag);

  for(char_i = 0; char_i < SWEEP_PCP_MAX_LINE_SIZE - 1; ++char_i)
  {
    if (line[char_i] == ' ')
    {
      break;
    }
    param_name[char_i] = line[char_i];
  }
  param_name[char_i] = 0;

  SWP_EXIT_AND_PRINT_ERR;
}

STATIC uint32
  sweep_pcp_type_SAND_UINT_get(
    SOC_SAND_IN     char        line[SWEEP_PCP_MAX_LINE_SIZE],
    SOC_SAND_INOUT  uint32   *value
  )
{
  char
    param[SWEEP_PCP_MAX_LINE_SIZE];
  char
    msg[SWEEP_PCP_MSG_SIZE];
  char
    *separetor;

  SWP_INIT_ERR_DEFS("sweep_pcp_type_SAND_UINT_get");
  COMPILER_REFERENCE(m_silent_flag);

  separetor = sal_strstr(line, " = ");

  if(separetor == NULL) {
    sal_sprintf(msg, "Failed to read the parameters from the line");
    SWP_SET_ERR_AND_EXIT_WITH_MSG(10, msg);
  }

  sal_strncpy(param, line, separetor - line);
  param[separetor - line] = '\0';
  while(*separetor && (*separetor < '0' || *separetor > '9')) {
    ++separetor;
  }

  if(*separetor == '\0') {
    sal_sprintf(msg, "Failed to read a parameter - %s", param);
    SWP_SET_ERR_AND_EXIT_WITH_MSG(10, msg);
  }

  *value = _shr_ctoi(separetor);

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

STATIC SOC_SAND_RET
  swp_pcp_type_SAND_U32_get(
    SOC_SAND_IN     char        line[SWEEP_PCP_MAX_LINE_SIZE],
    SOC_SAND_INOUT  uint32    *value
  )
{
  char
    param[SWEEP_PCP_MAX_LINE_SIZE];
  char
    msg[SWEEP_PCP_MSG_SIZE];
  char
    *separetor;

  SWP_INIT_ERR_DEFS("swp_pcp_type_SAND_U32_get");
  COMPILER_REFERENCE(m_silent_flag);

  separetor = sal_strstr(line, " = ");

  if(separetor == NULL) {
    sal_sprintf(msg, "Failed to read the parameters from the line");
    SWP_SET_ERR_AND_EXIT_WITH_MSG(10, msg);
  }

  sal_strncpy(param, line, separetor - line);
  param[separetor - line] = '\0';
  while(*separetor && (*separetor < '0' || *separetor > '9')) {
    ++separetor;
  }

  if(*separetor == '\0') {
    sal_sprintf(msg, "Failed to read a parameter - %s", param);
    SWP_SET_ERR_AND_EXIT_WITH_MSG(10, msg);
  }

  *value = _shr_ctoi(separetor);

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

STATIC uint32
  sweep_pcp_type_SAND_BOOL_get(
    SOC_SAND_IN     char        line[SWEEP_PCP_MAX_LINE_SIZE],
    SOC_SAND_INOUT  uint8   *value
  )
{
  char
    param[SWEEP_PCP_MAX_LINE_SIZE];
  char
    msg[SWEEP_PCP_MSG_SIZE];
  uint32
    ul_value;
  char
    *separetor;

  SWP_INIT_ERR_DEFS("sweep_pcp_type_SAND_BOOL_get");
  COMPILER_REFERENCE(m_silent_flag);

  separetor = sal_strstr(line, " = ");

  if(separetor == NULL) {
    sal_sprintf(msg, "Failed to read the parameters from the line");
    SWP_SET_ERR_AND_EXIT_WITH_MSG(10, msg);
  }

  sal_strncpy(param, line, separetor - line);
  param[separetor - line] = '\0';
  while(*separetor && (*separetor < '0' || *separetor > '9')) {
    ++separetor;
  }

  if(*separetor == '\0') {
    sal_sprintf(msg, "Failed to read a parameter - %s", param);
    SWP_SET_ERR_AND_EXIT_WITH_MSG(10, msg);
  }

  ul_value = _shr_ctoi(separetor);
  *value = SOC_SAND_NUM2BOOL(ul_value);

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_process_param_line(
    SOC_SAND_IN  char                 line[SWEEP_PCP_MAX_LINE_SIZE],
    SOC_SAND_OUT SWEEP_PCP_DEVICE_INIT_PARAMS  *sweep_info
  )
{
  uint32
    res = SOC_SAND_OK;
  char
    param_name[SWEEP_PCP_MAX_LINE_SIZE];
  char
    msg[SWEEP_PCP_MSG_SIZE];
  uint8
    tmp_bool=FALSE;
  uint32
    tmp=FALSE;

  SWP_INIT_ERR_DEFS("sweep_pcp_process_param_line");

  res = sweep_pcp_param_name_read(
          line,
          param_name
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 10, "");

  if( !strcmp( param_name, "srd_phys_param_preset_id"))
  {
    res = sweep_pcp_type_SAND_UINT_get(line, &(sweep_info->srd_phys_param_preset_id));
    SWP_EXIT_IF_ERR(res, 20);
    PCP_DO_NOTHING_AND_EXIT;
  }

  if( !strcmp( param_name, "enable_fec"))
  {
    res = sweep_pcp_type_SAND_BOOL_get(line, &(sweep_info->features_list.fec_en));
    SWP_EXIT_IF_ERR(res, 30);
    PCP_DO_NOTHING_AND_EXIT;
  }

  /*
   *  Operation mode param
   */

   /* Elk - Op mode */
   if( !strcmp( param_name, "op_mode_elk_enable"))
  {
    res = sweep_pcp_type_SAND_BOOL_get(line, &(sweep_info->operation_mode.elk.enable));
    SWP_EXIT_IF_ERR(res, 40);
    PCP_DO_NOTHING_AND_EXIT;
  }
   if( !strcmp( param_name, "op_mode_elk_ilm_key_mask_bitmap"))
  {
    res = swp_pcp_type_SAND_U32_get(line, &(sweep_info->operation_mode.elk.ilm_key_mask_bitmap));
    SWP_EXIT_IF_ERR(res, 50);
    PCP_DO_NOTHING_AND_EXIT;
  }

  /* sts - Op mode */
  if( !strcmp( param_name, "op_mode_sts_enable"))
  {
    res = sweep_pcp_type_SAND_BOOL_get(line, &(sweep_info->operation_mode.sts.enable));
    SWP_EXIT_IF_ERR(res, 60);
    PCP_DO_NOTHING_AND_EXIT;
  }
  if( !strcmp( param_name, "op_mode_sts_conf_word"))
  {
    res = swp_pcp_type_SAND_U32_get(line, &(sweep_info->operation_mode.sts.conf_word));
    SWP_EXIT_IF_ERR(res, 70);
    PCP_DO_NOTHING_AND_EXIT;
  }

  /* Oam - Op mode */
   if( !strcmp( param_name, "op_mode_oam_enable"))
  {
    res = sweep_pcp_type_SAND_BOOL_get(line, &(sweep_info->operation_mode.oam.enable));
    SWP_EXIT_IF_ERR(res, 80);
    PCP_DO_NOTHING_AND_EXIT;
  }
   
  if ( !strcmp( param_name, "pcp_core_freq"))
  {
    res = swp_pcp_type_SAND_U32_get(line, &(sweep_info->bsp_internal.pcp_core_freq));
    SWP_EXIT_IF_ERR(res, 90);
    PCP_DO_NOTHING_AND_EXIT;
  }
  if ( !strcmp( param_name, "pcp_elk_freq"))
  {
    res = swp_pcp_type_SAND_U32_get(line, &(sweep_info->bsp_internal.pcp_elk_freq));
    SWP_EXIT_IF_ERR(res, 100);
    PCP_DO_NOTHING_AND_EXIT;
  }

	if ( !strcmp( param_name, "do_memory_test"))
  {
    res = sweep_pcp_type_SAND_BOOL_get(line, &(sweep_info->do_memory_test));
    SWP_EXIT_IF_ERR(res, 110);
    PCP_DO_NOTHING_AND_EXIT;
  }

  if ( !strcmp( param_name, "rldram_small"))
  {
    res = sweep_pcp_type_SAND_BOOL_get(line, &(tmp_bool));
    SWP_EXIT_IF_ERR(res, 130);
    if (tmp_bool == TRUE)
    {
      sweep_info->operation_mode.elk.size_mbit = PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288;
      sweep_info->hw_adjustment.dram.size_mbit = PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288;
    }
    else
    {
      sweep_info->operation_mode.elk.size_mbit = PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_576;
      sweep_info->hw_adjustment.dram.size_mbit = PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_576;
    }
    PCP_DO_NOTHING_AND_EXIT;
  }

  if ( !strcmp( param_name, "elk_usage")) /* 1: EM, 2:lpm, 3:shared */
  {
    res = swp_pcp_type_SAND_U32_get(line, &(tmp));
    switch (tmp)
    {
    case 1:
      sweep_info->operation_mode.elk.mode = PCP_MGMT_ELK_LKP_MODE_EM;
    break;
    case 2:
    sweep_info->operation_mode.elk.mode = PCP_MGMT_ELK_LKP_MODE_LPM;
    break;
    default:
      sweep_info->operation_mode.elk.mode = PCP_MGMT_ELK_LKP_MODE_BOTH;
    break;
    }
    SWP_EXIT_IF_ERR(res, 140);
    PCP_DO_NOTHING_AND_EXIT;
  }

  if ( !strcmp( param_name, "nof_vrfs"))
  {
    res = swp_pcp_type_SAND_U32_get(line, &(sweep_info->operation_mode.ipv4_info.nof_vrfs));
    SWP_EXIT_IF_ERR(res, 150);
    PCP_DO_NOTHING_AND_EXIT;
  }

  if ( !strcmp( param_name, "nof_routes"))
  {
    res = swp_pcp_type_SAND_U32_get(line, &(sweep_info->operation_mode.ipv4_info.max_routes_in_vrf[0]));
    SWP_EXIT_IF_ERR(res, 160);
    PCP_DO_NOTHING_AND_EXIT;
  }

  if ( !strcmp( param_name, "bank_4_size"))
  {
    res = swp_pcp_type_SAND_U32_get(line, &(sweep_info->operation_mode.ipv4_info.lpm_4th_bank_size));
    SWP_EXIT_IF_ERR(res, 160);
    PCP_DO_NOTHING_AND_EXIT;
  }

  sal_sprintf(msg, "Specified parameter not recognized - %s", param_name);
  SWP_SET_ERR_AND_EXIT_WITH_MSG(120, msg);

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_read_line(
    SOC_SAND_INOUT FILE  *file_p,
    SOC_SAND_INOUT char  line[SWEEP_PCP_MAX_LINE_SIZE]
  )
{
  uint32
    ret = SOC_SAND_OK;
  uint32
    char_i = 0,
    nof_read;
  char
    character;
  uint8
    exit_condition = FALSE;
  char msg[SWEEP_PCP_MSG_SIZE];

  SWP_INIT_ERR_DEFS("sweep_pcp_read_line");

  ret = soc_sand_os_memset(
          line,
          0x0,
          SWEEP_PCP_MAX_LINE_SIZE
        );
  SWP_EXIT_AND_PRINT_IF_ERR(ret, FALSE, 10, "");

  while(!exit_condition)
  {
    nof_read = fread(&character, sizeof(char), 1, file_p);
    if(nof_read == 0)
    {
      exit_condition = TRUE;
    }
    else if( character == '\n' )
    {
      /*
       * Encounter new line.
       */
      exit_condition = TRUE;
    }
    else if (character == '\r')
    {
      /*
       * Do nothing.
       */
    }
    else
    {
      if(char_i >= SWEEP_PCP_MAX_LINE_SIZE)
      {
        /*
         * Exceeding maximum line size.
         */
        sal_sprintf(msg, "line is bigger than %u characters",SWEEP_PCP_MAX_LINE_SIZE);
        SWP_SET_ERR_AND_EXIT(SOC_SAND_ERR);
      }

      line[char_i++] = character;
    }
  }

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_remove_comment(
    SOC_SAND_INOUT char   line[SWEEP_PCP_MAX_LINE_SIZE]
  )
{
  uint32
    char_i = 0,
    str_size;

  SWP_INIT_ERR_DEFS("sweep_pcp_remove_comment");
  COMPILER_REFERENCE(m_silent_flag);

  str_size = soc_sand_os_strlen(line);

  if(str_size>SWEEP_PCP_MAX_LINE_SIZE)
  {
    str_size = SWEEP_PCP_MAX_LINE_SIZE;
  }

  for(char_i=1; char_i < str_size; ++char_i)
  {
    if(
        (line[char_i-1] == '/') &&
        (line[char_i] == '/')
      )
    {
      line[char_i-1] = '\0';
      goto exit;
    }
  }

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_remove_spaces_from_start(
    SOC_SAND_INOUT char                 line[SWEEP_PCP_MAX_LINE_SIZE]
  )
{
  uint32
    ret = SOC_SAND_OK;
  uint32
    char_i = 0,
    str_size,
    start_of_data;
  char tmp_line[SWEEP_PCP_MAX_LINE_SIZE];

  SWP_INIT_ERR_DEFS("sweep_pcp_remove_spaces_from_start");

  str_size = soc_sand_os_strlen(line);

  if(str_size >= SWEEP_PCP_MAX_LINE_SIZE)
  {
    str_size = SWEEP_PCP_MAX_LINE_SIZE - 1;
  }

  for(char_i = 0; char_i < str_size; ++char_i)
  {
    if( (line[char_i] == ' ') ||
        (line[char_i] == '\t')
      )
    {
      /*
       * space/tab at the start of line. Continue
       */
    }
    else
    {
      /*
       * no space/tap here.
       */
      break;
    }
  }
  start_of_data = char_i;

  if(start_of_data != 0)
  {
    ret = soc_sand_os_memcpy(
            tmp_line,
            &line[start_of_data],
            str_size - start_of_data + 1
          );
    SWP_EXIT_AND_PRINT_IF_ERR(ret, FALSE, 10, "");

    ret = soc_sand_os_memcpy(
            line,
            tmp_line,
            str_size - start_of_data + 1
          );
    SWP_EXIT_AND_PRINT_IF_ERR(ret, FALSE, 10, "");
  }

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_remove_spaces_from_end(
    SOC_SAND_INOUT char                 line[SWEEP_PCP_MAX_LINE_SIZE]
  )
{
  uint32
    char_i = 0,
    str_size;

  SWP_INIT_ERR_DEFS("sweep_pcp_remove_spaces_from_end");
  COMPILER_REFERENCE(m_silent_flag);

  str_size = soc_sand_os_strlen(line);

  if(str_size > SWEEP_PCP_MAX_LINE_SIZE - 1)
  {
    str_size = SWEEP_PCP_MAX_LINE_SIZE - 1;
  }
  if(str_size == 0) /* Nothing to do here. */
  {
    goto exit;
  }

  for(char_i = str_size - 1; char_i > 1 ; --char_i)
  {
    if ((line[char_i] != ' ') && (line[char_i] != '\t'))
    {
      /*
       * no space/tap here.
       */
      break;
    }
  }

  line[char_i + 1] = '\0';

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_convert_tab_to_space(
    SOC_SAND_INOUT char                 line[SWEEP_PCP_MAX_LINE_SIZE]
  )
{
  uint32
    char_i,
    str_size;

  SWP_INIT_ERR_DEFS("sweep_pcp_convert_tab_to_space");
  COMPILER_REFERENCE(m_silent_flag);

  str_size = soc_sand_os_strlen(line);

  if(str_size>SWEEP_PCP_MAX_LINE_SIZE)
  {
    str_size = SWEEP_PCP_MAX_LINE_SIZE;
  }

  for(char_i = 0; char_i < str_size; ++char_i)
  {
    if(line[char_i] == '\t')
    {
      line[char_i] = ' ';
    }
  }

  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_remove_spaces(
    SOC_SAND_INOUT char                 line[SWEEP_PCP_MAX_LINE_SIZE]
  )
{
  uint32
    res;

  SWP_INIT_ERR_DEFS("sweep_pcp_remove_spaces");

  res = sweep_pcp_remove_spaces_from_end(
          line
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 10, "");

  res = sweep_pcp_remove_spaces_from_start(
          line
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 10, "");

  res = sweep_pcp_convert_tab_to_space(
          line
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 10, "");

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_read_and_clear_line(
    SOC_SAND_INOUT FILE  *file_p,
    SOC_SAND_INOUT char  line[SWEEP_PCP_MAX_LINE_SIZE]
  )
{
  uint32
    res;

  SWP_INIT_ERR_DEFS("sweep_pcp_read_and_clear_line");

  res = sweep_pcp_read_line(
          file_p,
          line
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 10, "");

  res = sweep_pcp_remove_comment(
          line
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 10, "");

  res = sweep_pcp_remove_spaces(
          line
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 10, "");

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_SWEEP_PCP_DEVICE_INIT_PARAMS_read(
    SOC_SAND_INOUT FILE           *file_p,
    SOC_SAND_INOUT SWEEP_PCP_DEVICE_INIT_PARAMS  *sweep_info
  )
{
  uint32
    res = SOC_SAND_OK;
  char
    line[SWEEP_PCP_MAX_LINE_SIZE];
  uint32
    line_i = 1;

  SWP_INIT_ERR_DEFS("sweep_pcp_SWEEP_PCP_DEVICE_INIT_PARAMS_read");

  while (!feof(file_p))
  {
    res = sweep_pcp_read_and_clear_line(
            file_p,
            line
          );
    SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 10, "");

    if(soc_sand_os_strlen(line) != 0)
    {
      res = sweep_pcp_process_param_line(
              line,
              sweep_info
            );
      SWP_EXIT_AND_PRINT_IF_ERR(res, FALSE, 20, "");
    }

    ++line_i;
  }

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

uint32
  sweep_pcp_SWEEP_PCP_DEVICE_INIT_PARAMS_load(
    SOC_SAND_OUT SWEEP_PCP_DEVICE_INIT_PARAMS    *sweep_info,
    SOC_SAND_IN  uint8                          silent
  )
{
  uint32
    res = SOC_SAND_OK;
  FILE
    *file_p = NULL;
  char msg[SWEEP_PCP_MSG_SIZE];

  SWP_INIT_ERR_DEFS("sweep_pcp_SWEEP_PCP_DEVICE_INIT_PARAMS_load");

  res = sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_open(
          &file_p,
          TRUE
        );
  if (SWP_IS_ERR_RES(res))
  {
    sal_sprintf(
      msg,
      "Configuration file %s - NOT found,\r\n"
      "Using default configuration!",
      SWEEP_PCP_DEFAULT_FILE_NAME
    );
    SWP_PRINT_MSG(silent, msg);
    res = SOC_SAND_OK;
    PCP_DO_NOTHING_AND_EXIT;
  }
  else
  {
    SWP_PRINT_MSG(silent, "Configuration file found - updating default configuration");
  }


  sweep_pcp_register_counter = 0;
  res = sweep_pcp_SWEEP_PCP_DEVICE_INIT_PARAMS_read(
          file_p,
          sweep_info
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 20, "");
  sweep_pcp_register_counter = 0;

  SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 30, "");

exit:
  if (file_p) {
	  res = sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_close(
			  &file_p,
			  TRUE
	  );
  }
  SWP_EXIT_AND_PRINT_ERR;
}

#endif

/* } */
