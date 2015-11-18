#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)

/* $Id: arad_pp_ce_instruction.c,v 1.3 Broadcom SDK $
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ce_instruction.h>
#include <soc/dpp/ARAD/arad_general.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */



#define ARAD_PP_CE_16_BIT_FIELD_INST_SIZE_BITS_LSB        (12)
#define ARAD_PP_CE_32_BIT_FIELD_INST_SIZE_BITS_LSB        (11)
#define ARAD_PP_CE_16_BIT_FIELD_INST_SHIFT_BITS_LSB       (4)
#define ARAD_PP_CE_32_BIT_FIELD_INST_SHIFT_BITS_LSB       (4)
#define ARAD_PP_CE_16_BIT_FIELD_INST_INPUT_SEL_BIT_LSB    (3)
#define ARAD_PP_CE_32_BIT_FIELD_INST_INPUT_SEL_BIT_LSB    (3)
#define ARAD_PP_CE_16_BIT_FIELD_INST_OFFSET_SEL_SIZE_LSB  (0)
#define ARAD_PP_CE_32_BIT_FIELD_INST_OFFSET_SEL_BITS_LSB  (0)

#define ARAD_PP_CE_16_BIT_FIELD_INST_SIZE_BITS_MSB        (15)
#define ARAD_PP_CE_32_BIT_FIELD_INST_SIZE_BITS_MSB        (15)
#define ARAD_PP_CE_16_BIT_FIELD_INST_SHIFT_BITS_MSB       (11)
#define ARAD_PP_CE_32_BIT_FIELD_INST_SHIFT_BITS_MSB       (10)
#define ARAD_PP_CE_16_BIT_FIELD_INST_INPUT_SEL_BIT_MSB    (3)
#define ARAD_PP_CE_32_BIT_FIELD_INST_INPUT_SEL_BIT_MSB    (3)
#define ARAD_PP_CE_16_BIT_FIELD_INST_OFFSET_SEL_SIZE_MSB  (2)
#define ARAD_PP_CE_32_BIT_FIELD_INST_OFFSET_SEL_BITS_MSB  (2)

/* get field defines } */

void 
  arad_pp_ce_instruction_composer_checks (
    uint32 field_size_in_bits,
    uint32 header_index,
    uint32 offest_from_header_start_in_bits,
    uint32 ce_16_or_32)
{
  /* checks*/
  if (header_index > 7)
  {
  	/*ARAD_PP_INIT_PRINT_ADVANCE("header_index 0..7, header_index: %d", header_index);*/
  }

  if (ce_16_or_32 == ARAD_PP_CE_IS_CE16)
  {
  	if (field_size_in_bits > 16)
    {
  	  /*ARAD_PP_INIT_PRINT_ADVANCE("ARAD_PP_CE16 support field size 0..16, requested field size: %d", field_size_in_bits);*/
    }
  	if ((offest_from_header_start_in_bits % 4) != 0)
    {
      /*ARAD_PP_INIT_PRINT_ADVANCE("ARAD_PP_CE16 support offest_from_header_start_in_bits in nibble resolution, requested offest_from_header_start_in_bits: %d", offest_from_header_start_in_bits);*/
    }
  }
  if (ce_16_or_32 == ARAD_PP_CE_IS_CE32)
  {
  	if (field_size_in_bits > 32)
    {
  	  /*ARAD_PP_INIT_PRINT_ADVANCE("ARAD_PP_CE32 support field size 0..32, requested field size: %d", field_size_in_bits);*/
    }
  	if ((offest_from_header_start_in_bits % 8) != 0)
    {
      /*ARAD_PP_INIT_PRINT_ADVANCE("ARAD_PP_CE16 support offest_from_header_start_in_bits in byte resolution, requested offest_from_header_start_in_bits: %d", offest_from_header_start_in_bits);  	  */
    }
  }
}

/*
 * Function description 
 * the copy engine work as following: 
 * the actual offset = the copy engine size + offest_from_header_start_in_bits 
 * the field size = number of bits from the offset 
 *  
 * for example 
 *  
 * if we desire to copy 20 LSB bits from the DA we will use the following: 
 *  copy engine of 32 bit. 
 *  field_size_in_bits = 20
 *  offest_from_header_start_in_bits = 15
 *  
 *  so the actual offset will be: 15 + 32 = 47.
 *  the field size will be: 20
 *  
 *  
 *  0               DA                 47               SA
 *  |----------------------------------|----------------------------------|
 *                  |   copied part    | 
 *                  15                 0
*/
uint32 
   arad_pp_ce_instruction_composer (
     uint32 field_size_in_bits,
	   uint32 header_index,
	   uint32 offest_from_header_start_in_bits,
	   uint32 ce_16_or_32
	 )
{
  uint32 inst16=0;
  uint32 inst32=0;
  uint32 tmp=0;
  arad_pp_ce_instruction_composer_checks(field_size_in_bits,header_index,offest_from_header_start_in_bits,ce_16_or_32);
  
  if (ce_16_or_32 == ARAD_PP_CE_IS_CE16)
  {
    tmp = field_size_in_bits-1;
    SHR_BITCOPY_RANGE(&inst16, ARAD_PP_CE_16_BIT_FIELD_INST_SIZE_BITS_LSB, &tmp, 0, (ARAD_PP_CE_16_BIT_FIELD_INST_SIZE_BITS_MSB-ARAD_PP_CE_16_BIT_FIELD_INST_SIZE_BITS_LSB+1));
    tmp = offest_from_header_start_in_bits /4;
    SHR_BITCOPY_RANGE(&inst16, ARAD_PP_CE_16_BIT_FIELD_INST_SHIFT_BITS_LSB, &tmp, 0, (ARAD_PP_CE_16_BIT_FIELD_INST_SHIFT_BITS_MSB-ARAD_PP_CE_16_BIT_FIELD_INST_SHIFT_BITS_LSB+1));
    tmp = 0;
    SHR_BITCOPY_RANGE(&inst16, ARAD_PP_CE_16_BIT_FIELD_INST_INPUT_SEL_BIT_LSB, &tmp, 0, (ARAD_PP_CE_16_BIT_FIELD_INST_INPUT_SEL_BIT_MSB-ARAD_PP_CE_16_BIT_FIELD_INST_INPUT_SEL_BIT_LSB+1));
    tmp = header_index;
    SHR_BITCOPY_RANGE(&inst16, ARAD_PP_CE_16_BIT_FIELD_INST_OFFSET_SEL_SIZE_LSB, &tmp, 0, (ARAD_PP_CE_16_BIT_FIELD_INST_OFFSET_SEL_SIZE_MSB-ARAD_PP_CE_16_BIT_FIELD_INST_OFFSET_SEL_SIZE_LSB+1));
    tmp = inst16;
  }
  else
  {
    tmp = field_size_in_bits-1;
    SHR_BITCOPY_RANGE(&inst32, ARAD_PP_CE_32_BIT_FIELD_INST_SIZE_BITS_LSB, &tmp, 0, (ARAD_PP_CE_32_BIT_FIELD_INST_SIZE_BITS_MSB-ARAD_PP_CE_32_BIT_FIELD_INST_SIZE_BITS_LSB+1));
    tmp = offest_from_header_start_in_bits /8;
    SHR_BITCOPY_RANGE(&inst32, ARAD_PP_CE_32_BIT_FIELD_INST_SHIFT_BITS_LSB, &tmp, 0, (ARAD_PP_CE_32_BIT_FIELD_INST_SHIFT_BITS_MSB-ARAD_PP_CE_32_BIT_FIELD_INST_SHIFT_BITS_LSB+1));
    tmp = 0;
    SHR_BITCOPY_RANGE(&inst32, ARAD_PP_CE_32_BIT_FIELD_INST_INPUT_SEL_BIT_LSB, &tmp, 0, (ARAD_PP_CE_32_BIT_FIELD_INST_INPUT_SEL_BIT_MSB-ARAD_PP_CE_32_BIT_FIELD_INST_INPUT_SEL_BIT_LSB+1));
    tmp = header_index;
    SHR_BITCOPY_RANGE(&inst32, ARAD_PP_CE_32_BIT_FIELD_INST_OFFSET_SEL_BITS_LSB, &tmp, 0, (ARAD_PP_CE_32_BIT_FIELD_INST_OFFSET_SEL_BITS_MSB-ARAD_PP_CE_32_BIT_FIELD_INST_OFFSET_SEL_BITS_LSB+1));
    tmp = inst32;
  }
  
  return tmp;
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

