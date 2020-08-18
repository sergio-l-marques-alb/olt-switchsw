/* $Id: sand_conv.c,v 1.3 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

#include <soc/dnx/legacy/SAND/Utils/sand_conv.h>
#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/SAND/Utils/sand_u64.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dnx/legacy/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

/*****************************************************
 * See details in dnx_sand_conv.h
 *****************************************************/
shr_error_e
  dnx_sand_kbits_per_sec_to_clocks(
    DNX_SAND_IN       uint32    rate,     /* in Kbits/sec */
    DNX_SAND_IN       uint32     credit,   /* in Bytes */
    DNX_SAND_IN       uint32    ticks_per_sec,
    DNX_SAND_OUT      uint32*   interval  /* in device clocks */
  )
{
  shr_error_e
    ex;
  DNX_SAND_U64
    calc,
    calc2;
  uint32
    tmp;

  if (NULL == interval)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  if (0 == rate)
  {
    /* Divide by zero */
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  dnx_sand_u64_multiply_longs((credit * DNX_SAND_NOF_BITS_IN_CHAR),
                          DNX_SAND_DIV_ROUND(ticks_per_sec,1000),
                          &calc);
  dnx_sand_u64_devide_u64_long(&calc, rate, &calc2);
  if (dnx_sand_u64_to_long(&calc2, &tmp))
  {
    /* Overflow */
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  *interval = tmp;
  ex = _SHR_E_NONE;
exit:
  return ex ;
}

/*****************************************************
 * See details in dnx_sand_conv.h
 *****************************************************/
shr_error_e
  dnx_sand_clocks_to_kbits_per_sec(
    DNX_SAND_IN       uint32    interval, /* in device clocks */
    DNX_SAND_IN       uint32     credit,   /* in Bytes */
    DNX_SAND_IN       uint32    ticks_per_sec,
    DNX_SAND_OUT      uint32*   rate      /* in Kbits/sec */
  )
{
  shr_error_e
    ex ;
  DNX_SAND_U64
    calc,
    calc2;
  uint32
    tmp;

  if (NULL == rate)
  {
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  if (0 == interval)
  {
    /* Divide by zero */
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  dnx_sand_u64_multiply_longs((credit * DNX_SAND_NOF_BITS_IN_CHAR),
                          DNX_SAND_DIV_ROUND(ticks_per_sec,1000),
                          &calc);
  dnx_sand_u64_devide_u64_long(&calc, interval, &calc2);
  if (dnx_sand_u64_to_long(&calc2, &tmp))
  {
    /* Overflow */
    ex = _SHR_E_INTERNAL ;
    goto exit ;
  }
  *rate = tmp;
  ex = _SHR_E_NONE ;
exit:
  return ex ;
}

/*****************************************************
 * See details in dnx_sand_conv.h
 *****************************************************/

shr_error_e
  dnx_sand_ip_addr_numeric_to_string(
    DNX_SAND_IN uint32 ip_addr,
    DNX_SAND_IN uint8  short_format,
    DNX_SAND_OUT char   decimal_ip[DNX_SAND_IP_STR_SIZE]
  )
{
  const char
    *format ;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    
  SHR_NULL_CHECK(decimal_ip, _SHR_E_PARAM, "decimal_ip");

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
  SHR_FUNC_EXIT;
}


/*****************************************************
 * See details in dnx_sand_conv.h
 *****************************************************/
shr_error_e
  dnx_sand_ip_addr_string_to_numeric(
    DNX_SAND_IN char   decimal_ip[DNX_SAND_IP_STR_SIZE],
    DNX_SAND_OUT uint32 *ip_addr
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

  SHR_FUNC_INIT_VARS(NO_UNIT);
    
  SHR_NULL_CHECK(decimal_ip, _SHR_E_PARAM, "decimal_ip");

  tmp_addr = 0;
  dots_count = 0;
  val = 0;
  illegal = FALSE;

  for (index = 0; index <DNX_SAND_IP_STR_SIZE; ++index)
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
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "SOC_SAND_ILLEGAL_IP_FORMAT");
  }

  *ip_addr = tmp_addr;

exit:
  SHR_FUNC_EXIT;

}
