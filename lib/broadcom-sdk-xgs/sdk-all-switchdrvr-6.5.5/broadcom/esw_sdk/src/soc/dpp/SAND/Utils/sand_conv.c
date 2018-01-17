/* $Id: sand_conv.c,v 1.3 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_conv.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

/*****************************************************
 * See details in soc_sand_conv.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_kbits_per_sec_to_clocks(
    SOC_SAND_IN       uint32    rate,     /* in Kbits/sec */
    SOC_SAND_IN       uint32     credit,   /* in Bytes */
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   interval  /* in device clocks */
  )
{
  SOC_SAND_RET
    ex ;
  SOC_SAND_U64
    calc,
    calc2;
  uint32
    tmp;

  if (NULL == interval)
  {
    ex = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  if (0 == rate)
  {
    /* Divide by zero */
    ex = SOC_SAND_DIV_BY_ZERO_ERR ;
    goto exit ;
  }
  soc_sand_u64_multiply_longs((credit * SOC_SAND_NOF_BITS_IN_CHAR),
                          SOC_SAND_DIV_ROUND(ticks_per_sec,1000),
                          &calc);
  soc_sand_u64_devide_u64_long(&calc, rate, &calc2);
  if (soc_sand_u64_to_long(&calc2, &tmp))
  {
    /* Overflow */
    ex = SOC_SAND_OVERFLOW_ERR ;
    goto exit ;
  }
  *interval = tmp;
  ex = SOC_SAND_OK ;
exit:
  return ex ;
}

/*****************************************************
 * See details in soc_sand_conv.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_clocks_to_kbits_per_sec(
    SOC_SAND_IN       uint32    interval, /* in device clocks */
    SOC_SAND_IN       uint32     credit,   /* in Bytes */
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   rate      /* in Kbits/sec */
  )
{
  SOC_SAND_RET
    ex ;
  SOC_SAND_U64
    calc,
    calc2;
  uint32
    tmp;

  if (NULL == rate)
  {
    ex = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  if (0 == interval)
  {
    /* Divide by zero */
    ex = SOC_SAND_DIV_BY_ZERO_ERR ;
    goto exit ;
  }
  soc_sand_u64_multiply_longs((credit * SOC_SAND_NOF_BITS_IN_CHAR),
                          SOC_SAND_DIV_ROUND(ticks_per_sec,1000),
                          &calc);
  soc_sand_u64_devide_u64_long(&calc, interval, &calc2);
  if (soc_sand_u64_to_long(&calc2, &tmp))
  {
    /* Overflow */
    ex = SOC_SAND_OVERFLOW_ERR ;
    goto exit ;
  }
  *rate = tmp;
  ex = SOC_SAND_OK ;
exit:
  return ex ;
}

/*****************************************************
 * See details in soc_sand_conv.h
 *****************************************************/

uint32
  soc_sand_ip_addr_numeric_to_string(
    SOC_SAND_IN uint32 ip_addr,
    SOC_SAND_IN uint8  short_format,
    SOC_SAND_OUT char   decimal_ip[SOC_SAND_IP_STR_SIZE]
  )
{
  const char
    *format ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_IP_ADDR_NUMERIC_TO_STRING);
  SOC_SAND_CHECK_NULL_INPUT(decimal_ip);

  if (short_format)
  {
    format = "%d.%d.%d.%d";
  }
  else
  {
    format = "%03d.%03d.%03d.%03d";
  }

/* $Id: sand_conv.c,v 1.3 Broadcom SDK $
 * EXTRACTING BYTES FROM uint32
 */
  sal_sprintf(decimal_ip, format,
    (unsigned char)(((ip_addr)>>24)&0xFF),
    (unsigned char)(((ip_addr)>>16)&0xFF),
    (unsigned char)(((ip_addr)>>8)&0xFF),
    (unsigned char)((ip_addr)&0xFF)) ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_ip_addr_numeric_to_string()",0,0);
}


/*****************************************************
 * See details in soc_sand_conv.h
 *****************************************************/
uint32
  soc_sand_ip_addr_string_to_numeric(
    SOC_SAND_IN char   decimal_ip[SOC_SAND_IP_STR_SIZE],
    SOC_SAND_OUT uint32 *ip_addr
  )
{
  uint8
    curr_char;
  uint32
    index,
    dots_count;
  uint32
    val,
    tmp_addr;
  uint8
    illegal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_IP_ADDR_STRING_TO_NUMERIC);
  SOC_SAND_CHECK_NULL_INPUT(decimal_ip);

  tmp_addr = 0;
  dots_count = 0;
  val = 0;
  illegal = FALSE;

  for (index = 0; index <SOC_SAND_IP_STR_SIZE; ++index)
  {
    curr_char = decimal_ip[index];

    if( curr_char == '\0')
    {
      tmp_addr |= val << 8 * ( 3 - dots_count);
      break;
    }
    if(curr_char == '.')
    {
      tmp_addr |= val << 8 * ( 3 - dots_count);
      val = 0;
      ++dots_count;
      if(dots_count > 3)
      {
        illegal = TRUE;
        break;
      }
    }
    else if(curr_char >= '0' && curr_char <= '9')
    {
      val = val * 10 + (curr_char - '0');
    }
    else
    {
      illegal = TRUE;
      break;
    }
    if (val > 0xFF)
    {
      illegal = TRUE;
      break;
    }
  }

  if (illegal || dots_count < 3)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_IP_FORMAT,10,exit);
  }

  *ip_addr = tmp_addr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_ip_addr_string_to_numeric()",0,0);

}
