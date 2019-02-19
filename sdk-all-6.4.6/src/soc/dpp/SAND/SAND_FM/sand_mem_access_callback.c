/* $Id: sand_mem_access_callback.c,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
* FILENAME:       soc_sand_mem_access_callback.c
*
* AUTHOR:         Dune (S.Z.)
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



#include <soc/dpp/SAND/SAND_FM/sand_mem_access_callback.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>


/*****************************************************
 * See details in soc_sand_mem_access_callback.h
 *****************************************************/
uint32
  soc_sand_mem_read_callback(
    uint32 *buffer,
    uint32 size
  )
{
  SOC_SAND_MEM_READ_CALLBACK_STRUCT  *callback_struct;
  uint32 *new_buf;
  uint32 ex, no_err;
  int res;
  SOC_SAND_RET soc_sand_ret;

  ex = 0;
  no_err = ex;
  new_buf = NULL;
  callback_struct = NULL;
  soc_sand_ret = SOC_SAND_OK;
  /*
   */
  if (sizeof(SOC_SAND_MEM_READ_CALLBACK_STRUCT) != size)
  {
    soc_sand_set_error_code_into_error_word(SOC_SAND_ERR_8001,&ex);
    soc_sand_set_severity_into_error_word  (1, &ex);
    goto exit;
  }
  /*
   */
  callback_struct = (SOC_SAND_MEM_READ_CALLBACK_STRUCT  *)buffer;

  res = soc_sand_take_chip_descriptor_mutex(callback_struct->unit);
  if ( SOC_SAND_OK != res )
  {
    if (SOC_SAND_ERR == res)
    {
      soc_sand_set_error_code_into_error_word(SOC_SAND_SEM_TAKE_FAIL,  &ex);
      goto exit;
    }
    if ( 0 > res )
    {
      soc_sand_set_error_code_into_error_word(SOC_SAND_ILLEGAL_DEVICE_ID,  &ex);
      goto exit;
    }
  }
  /*
   * OK, all check are done, we can proceed with the physical read
   */
  soc_sand_ret = soc_sand_mem_read(
               callback_struct->unit,
               callback_struct->result_ptr,
               callback_struct->offset,
               callback_struct->size,
               callback_struct->indirect
             );
  soc_sand_set_error_code_into_error_word(soc_sand_ret, &ex);

  /*
   */
  if ( SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(callback_struct->unit) )
  {
    soc_sand_set_error_code_into_error_word(SOC_SAND_SEM_TAKE_FAIL,&ex);
    goto exit;
  }
  /*
   * There are 2 conditions to call the user supplied callback:
   * 1 - There was an error
   * 2 - The value read from the chip differs from the value
   *     read at the last time (if it is valid)
   */
  if ( (ex != no_err) ||
      !(callback_struct->copy_of_result_is_valid) ||
       (callback_struct->copy_of_result_is_valid &&
        soc_sand_os_memcmp(
          callback_struct->copy_of_last_result_ptr,
          callback_struct->result_ptr,
          SOC_SAND_MIN(callback_struct->size, SOC_SAND_CALLBACK_BUF_SIZE)
       ))
     )
  {
    if (callback_struct->user_callback_proc)
    {
      res = callback_struct->user_callback_proc(
                                callback_struct->user_callback_id,
                                callback_struct->result_ptr,
                                &new_buf,
                                callback_struct->size,
                                ex,
                                callback_struct->unit,
                                callback_struct->offset,
                                callback_struct->soc_sand_tcm_callback_id,
                                0
                             );
      soc_sand_os_memcpy(
        callback_struct->copy_of_last_result_ptr,
        callback_struct->result_ptr,
        SOC_SAND_MIN(callback_struct->size, SOC_SAND_CALLBACK_BUF_SIZE)
      );
      callback_struct->copy_of_result_is_valid = TRUE;
      if (new_buf)
      {
        callback_struct->result_ptr =  new_buf;
      }
      /*
       */
      if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(res))
      {
        soc_sand_set_error_code_into_error_word(SOC_SAND_ERR_8006,  &ex);
        callback_struct->copy_of_result_is_valid = FALSE;
        goto exit;
      }
    }
  }
  /*
   */
exit:
  if (ex != no_err)
  {
    soc_sand_error_handler(ex, "error in soc_sand_mem_read_callback()", 0,0,0,0,0,0);
  }
  else
  {
  }
  return ex;
}

