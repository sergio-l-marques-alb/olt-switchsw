/* $Id: sand_framework.c,v 1.12 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $
*/

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

#include <shared/bsl.h>
#include <soc/dnx/legacy/drv.h>

#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnx/legacy/SAND/Management/sand_general_params.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/Management/sand_chip_descriptors.h>

#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>

/* $Id: sand_framework.c,v 1.12 Broadcom SDK $
 */

int
  dnx_sand_is_long_aligned (
    uint32 word_to_check
  )
{
  if (word_to_check & DNX_SAND_UINT32_ALIGN_MASK)
  {
    return FALSE ;
  }
  return TRUE ;
}
/*
 */

void
  dnx_sand_check_driver_and_device(
    int  unit,
    uint32 *error_word
  )
{
  if ( !dnx_sand_general_get_driver_is_started() )
  {
    *error_word = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   */
  if ( !dnx_sand_is_chip_descriptor_valid(unit) )
  {
    *error_word = _SHR_E_INTERNAL ;
    goto exit ;
  }
  /*
   */
exit:
  return ;
}

uint32
  dnx_sand_get_index_of_max_member_in_array(
    DNX_SAND_IN     uint32                     array[],
    DNX_SAND_IN     uint32                    len
  )
{
  uint32
      index,
      index_of_max,
      max_val;

  index_of_max = 0;
  max_val = 0;

  for (index = 0; index < len; index++)
  {
    if (array[index] > max_val)
    {
      max_val = array[index];
      index_of_max = index;
    }
  }
  return index_of_max;
}

/*
 * Printing utility.
 * Convert from enumerator to string.
 */
const char*
  dnx_sand_SAND_OP_to_str(
    DNX_SAND_IN DNX_SAND_OP      dnx_sand_op,
    DNX_SAND_IN uint32 short_format
  )
{
  const char
    *str;

  switch(dnx_sand_op)
  {
  case DNX_SAND_NOP:
    if(short_format)
    {
      str = "NOP";
    }
    else
    {
      str = "DNX_SAND_NOP";
    }
    break;

  case DNX_SAND_OP_AND:
    if(short_format)
    {
      str = "AND";
    }
    else
    {
      str = "DNX_SAND_OP_AND";
    }
    break;

  case DNX_SAND_OP_OR:
    if(short_format)
    {
      str = "OR";
    }
    else
    {
      str = "DNX_SAND_OP_OR";
    }
    break;

  case DNX_SAND_NOF_SAND_OP:
    if(short_format)
    {
      str = "NOF_OP";
    }
    else
    {
      str = "DNX_SAND_NOF_SAND_OP";
    }
    break;

  default:
    str = "dnx_sand_SAND_OP_to_str input parameters error (dnx_sand_op)";
  }

  return str;
}

/*
 * Print HEX buffer.
 */
/*****************************************************
*NAME
* dnx_sand_print_hex_buff
*TYPE:
*  PROC
*DATE:
*  14-Sep-03
*FUNCTION:
*  Printing aid utility.
*  Prints byte buffer in HEX format.
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_IN char*        buff -
*      Byte buffer.
*    DNX_SAND_IN uint32 buff_byte_size -
*      Number of bytes in the buffer
*    DNX_SAND_IN uint32 nof_bytes_per_line -
*      Number of byte to print in one line.
*  DNX_SAND_INDIRECT:
*    None.
*OUTPUT:
*  DNX_SAND_DIRECT:
*    None.
*  DNX_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
void
  dnx_sand_print_hex_buff(
    DNX_SAND_IN char*        buff,
    DNX_SAND_IN uint32 buff_byte_size,
    DNX_SAND_IN uint32 nof_bytes_per_line
  )
{
  uint32
    byte_i;

  if(NULL == buff)
  {
    goto exit;
  }

  for (byte_i=0; byte_i<buff_byte_size; byte_i++)
  {
    if( (byte_i != 0)  &&
        (byte_i%4 == 0) &&
        ((byte_i%nof_bytes_per_line) != 0)
      )
    {
      LOG_CLI((BSL_META(" ")));
    }
    if( (byte_i != 0)  &&
        ((byte_i%nof_bytes_per_line) == 0)
      )
    {
      LOG_CLI((BSL_META("\n\r")));
    }
    if((byte_i%nof_bytes_per_line) == 0)
    {
      LOG_CLI((BSL_META("%3u-%3u:"),
               byte_i,
               DNX_SAND_MIN(byte_i + (nof_bytes_per_line-1), buff_byte_size-1)
               ));
    }

    LOG_CLI((BSL_META("%02X"), (buff[byte_i]&0xFF)));
  }
  LOG_CLI((BSL_META("\n\r")));

exit:
  return;
}

/*****************************************************
*NAME
* dnx_sand_print_bandwidth
*TYPE:
*  PROC
*DATE:
*  13-Sep-04
*FUNCTION:
*  Printing aid utility.
*  Prints Band-Width.
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_IN uint32 bw_kbps -
*      Band-Width in KiloBits-Second
*  DNX_SAND_INDIRECT:
*    None.
*OUTPUT:
*  DNX_SAND_DIRECT:
*    None.
*  DNX_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
void
  dnx_sand_print_bandwidth(
    DNX_SAND_IN uint32 bw_kbps,
    DNX_SAND_IN uint32  short_format
  )
{
  uint32
    tmp;

  tmp = bw_kbps;
  tmp /= 1000;

  LOG_CLI((BSL_META("%u Kbps,  %u.%03u Gbps"),
           bw_kbps,
           tmp/1000,
           tmp%1000
           ));

  if(!short_format)
  {
    LOG_CLI((BSL_META("\n\r")));
  }

  return;
}

/*****************************************************
*NAME:
* dnx_sand_set_field
*DATE:
* 10/SEP/2007
*FUNCTION:
*  set field onto the register
*INPUT:
*  DNX_SAND_DIRECT:
*    DNX_SAND_INOUT    uint32* reg_val  -
*       the value of the register to be changed
*    DNX_SAND_IN       uint32    ms_bit         -
*       most significant bit where to set the field_val,
*    DNX_SAND_IN       uint32    ls_bit         -
*       less significant bit where to set the field_val
*       Range 0:32.
*    DNX_SAND_IN       uint32    field_val -
*       field to set into reg_val
*  DNX_SAND_INDIRECT:
*OUTPUT:
*  DNX_SAND_DIRECT:
*    Non-zero in case of an error
*  DNX_SAND_INDIRECT:
*    the value of the register after setting the value into it.
*REMARKS:
*    None
*SEE ALSO:
*****************************************************/
shr_error_e
  dnx_sand_set_field(
    DNX_SAND_INOUT  uint32    *reg_val,
    DNX_SAND_IN  uint32       ms_bit,
    DNX_SAND_IN  uint32       ls_bit,
    DNX_SAND_IN  uint32       field_val
  )
{
  uint32
    tmp_reg;
  shr_error_e
    shr_error_e;

  shr_error_e = _SHR_E_NONE;
  tmp_reg = *reg_val;

  /*
   * 32 bits at most
   */
  if (ms_bit-ls_bit+1 > 32)
  {
    shr_error_e = _SHR_E_INTERNAL;
    goto exit;
  }

  tmp_reg &= DNX_SAND_ZERO_BITS_MASK(ms_bit,ls_bit);

  tmp_reg |= DNX_SAND_SET_BITS_RANGE(
              field_val,
              ms_bit,
              ls_bit
            );

  *reg_val = tmp_reg;

exit:
  return shr_error_e;

}

shr_error_e
  dnx_sand_U8_to_U32(
    DNX_SAND_IN uint8     *u8_val,
    DNX_SAND_IN uint32    nof_bytes,
    DNX_SAND_OUT uint32   *u32_val
  )
{
  uint32
    u8_indx,
    cur_u8_indx,
    u32_indx;
  DNX_SAND_IN uint8
    *cur_u8;
  shr_error_e
    shr_error_e = _SHR_E_NONE;

  if (!u8_val || !u32_val)
  {
    shr_error_e = _SHR_E_INTERNAL;
    goto exit ;
  }

  cur_u8_indx = 0;
  u32_indx = 0;

  for ( cur_u8 = u8_val, u8_indx = 0; u8_indx < nof_bytes; ++u8_indx, ++cur_u8)
  {
     dnx_sand_set_field(
       &(u32_val[u32_indx]),
       (cur_u8_indx + 1) * DNX_SAND_NOF_BITS_IN_BYTE - 1,
       cur_u8_indx * DNX_SAND_NOF_BITS_IN_BYTE,
       *cur_u8
     );

    cur_u8_indx++;
    if (cur_u8_indx >= sizeof(uint32))
    {
      cur_u8_indx = 0;
      ++u32_indx;
    }
  }
exit:
  return shr_error_e;
}

shr_error_e
  dnx_sand_U32_to_U8(
    DNX_SAND_IN uint32  *u32_val,
    DNX_SAND_IN uint32  nof_bytes,
    DNX_SAND_OUT uint8  *u8_val
  )
{
  uint32
    u8_indx,
    cur_u8_indx;
  DNX_SAND_IN uint32
    *cur_u32;

  shr_error_e
    shr_error_e = _SHR_E_NONE;

  if (!u8_val || !u32_val)
  {
    shr_error_e = _SHR_E_INTERNAL;
    goto exit ;
  }

  cur_u8_indx = 0;
  for ( cur_u32 = u32_val, u8_indx = 0; u8_indx < nof_bytes; ++u8_indx)
  {
    u8_val[u8_indx] = (uint8)
      DNX_SAND_GET_BITS_RANGE(
        *cur_u32,
        (cur_u8_indx + 1) * DNX_SAND_NOF_BITS_IN_BYTE - 1,
        cur_u8_indx * DNX_SAND_NOF_BITS_IN_BYTE
       );

    ++cur_u8_indx;
    if (cur_u8_indx >= sizeof(uint32))
    {
      cur_u8_indx = 0;
      ++cur_u32;
    }
  }
exit:
  return shr_error_e;
}

void
  dnx_sand_SAND_U32_RANGE_clear(
    DNX_SAND_OUT DNX_SAND_U32_RANGE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  dnx_sand_os_memset(info, 0x0, sizeof(DNX_SAND_U32_RANGE));
  info->start = 0;
  info->end = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  dnx_sand_SAND_TABLE_BLOCK_RANGE_clear(
    DNX_SAND_OUT DNX_SAND_TABLE_BLOCK_RANGE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  dnx_sand_os_memset(info, 0x0, sizeof(DNX_SAND_TABLE_BLOCK_RANGE));
  info->iter = 0;
  info->entries_to_scan = 0;
  info->entries_to_act = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  dnx_sand_SAND_U32_RANGE_print(
    DNX_SAND_IN  DNX_SAND_U32_RANGE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META("%u - %u "),info->start, info->end));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  dnx_sand_SAND_TABLE_BLOCK_RANGE_print(
    DNX_SAND_IN  DNX_SAND_TABLE_BLOCK_RANGE *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META("iter: %u\n\r"),info->iter));
  LOG_CLI((BSL_META("entries_to_scan: %u\n\r"),info->entries_to_scan));
  LOG_CLI((BSL_META("entries_to_act: %u\n\r"),info->entries_to_act));
exit:
  SHR_VOID_FUNC_EXIT;
}


