/* $Id: utilex_framework.c,v 1.9 Broadcom SDK $
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
/*! \file utilex_framework.c 
 *  
 * All common utilities related to framework. 
 *  
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

/*************
* INCLUDES  *
*************/
/* { */

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <sal/types.h>

/* } */

/*!
 * \brief
 *  set field onto the register
 * 
 * \par DIRECT INPUT
 *   \param [in,out] *reg_val  - \n
 *       the value of the register to be changed
 *   \param [in] ms_bit         -\n
 *       most significant bit where to set the field_val,
 *   \param [in] ls_bit         -\n
 *       less significant bit where to set the field_val 
 *   \param [in] field_val         -\n
 *       field to set into reg_val
 *  \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 *  \par INDIRECT OUTPUT
 *   \param *reg_val the value of the register after setting the value into it.
 */
shr_error_e
utilex_set_field(
  uint32 * reg_val,
  uint32 ms_bit,
  uint32 ls_bit,
  uint32 field_val)
{
  uint32 tmp_reg;
  shr_error_e ret;

  ret = _SHR_E_NONE;
  tmp_reg = *reg_val;

  /*
   * 32 bits at most
   */
  if (ms_bit - ls_bit + 1 > 32)
  {
    ret = _SHR_E_PARAM;
    goto exit;
  }

  tmp_reg &= UTILEX_ZERO_BITS_MASK(ms_bit, ls_bit);

  tmp_reg |= UTILEX_SET_BITS_RANGE(field_val, ms_bit, ls_bit);

  *reg_val = tmp_reg;

exit:
  return ret;

}

/*!
 * \brief
 * Convert an uint8 array to an uint32 array 
 * 
 * \par DIRECT INPUT
 *   \param [in] *u8_val  - \n
 *       pointer to an array of uint8, 
 *   \param [in] nof_bytes -\n
 *       size of the array  
 *  \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 *  \par INDIRECT OUTPUT
 *   \param *u32_val -\n
 *   the array of uint32  
 */
shr_error_e
utilex_U8_to_U32(
  uint8 * u8_val,
  uint32 nof_bytes,
  uint32 * u32_val)
{
  uint32 u8_indx, cur_u8_indx, u32_indx;
  uint8 *cur_u8;
  shr_error_e ret = _SHR_E_NONE;

  if (!u8_val || !u32_val)
  {
    ret = _SHR_E_PARAM;
    goto exit;
  }

  cur_u8_indx = 0;
  u32_indx = 0;

  for (cur_u8 = u8_val, u8_indx = 0; u8_indx < nof_bytes; ++u8_indx, ++cur_u8)
  {
    utilex_set_field(&(u32_val[u32_indx]),
                     (cur_u8_indx + 1) * SAL_UINT8_NOF_BITS - 1, cur_u8_indx * SAL_UINT8_NOF_BITS, *cur_u8);

    cur_u8_indx++;
    if (cur_u8_indx >= sizeof(uint32))
    {
      cur_u8_indx = 0;
      ++u32_indx;
    }
  }
exit:
  return ret;
}

/*!
 * \brief
 * Convert an uint32 array to an uint8 array 
 * 
 * \par DIRECT INPUT
 *   \param [in] *u32_val  - \n
 *       pointer to an array of uint32
 *   \param [in] nof_bytes -\n
 *       nof bytes  
 *  \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 *  \par INDIRECT OUTPUT
 *   \param *u8_val the array of uint8  
 */
shr_error_e
utilex_U32_to_U8(
  uint32 * u32_val,
  uint32 nof_bytes,
  uint8 * u8_val)
{
  uint32 u8_indx, cur_u8_indx;
  uint32 *cur_u32;

  shr_error_e ret = _SHR_E_NONE;

  if (!u8_val || !u32_val)
  {
    ret = _SHR_E_PARAM;
    goto exit;
  }

  cur_u8_indx = 0;
  for (cur_u32 = u32_val, u8_indx = 0; u8_indx < nof_bytes; ++u8_indx)
  {
    u8_val[u8_indx] = (uint8)
      UTILEX_GET_BITS_RANGE(*cur_u32, (cur_u8_indx + 1) * SAL_UINT8_NOF_BITS - 1, cur_u8_indx * SAL_UINT8_NOF_BITS);

    ++cur_u8_indx;
    if (cur_u8_indx >= sizeof(uint32))
    {
      cur_u8_indx = 0;
      ++cur_u32;
    }
  }
exit:
  return ret;
}
