/* $Id: sand_mem_access_callback.c,v 1.5 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       dnx_sand_mem_access_callback.c
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

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

#include <soc/dnx/legacy/SAND/SAND_FM/sand_mem_access_callback.h>
#include <soc/dnx/legacy/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dnx/legacy/SAND/Management/sand_chip_descriptors.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>


/*****************************************************
 * See details in dnx_sand_mem_access_callback.h
 *****************************************************/
shr_error_e
  dnx_sand_mem_read_callback(
    uint32 *buffer,
    uint32 size
  )
{
  DNX_SAND_MEM_READ_CALLBACK_STRUCT  *callback_struct;
  uint32 *new_buf;
  shr_error_e dnx_sand_ret;

 
  new_buf = NULL;
  callback_struct = NULL;
  dnx_sand_ret = _SHR_E_NONE;
  /*
   */
  if (sizeof(DNX_SAND_MEM_READ_CALLBACK_STRUCT) != size)
  {
    dnx_sand_ret = _SHR_E_INTERNAL;
    goto exit;
  }
  /*
   */
  callback_struct = (DNX_SAND_MEM_READ_CALLBACK_STRUCT  *)buffer;

  dnx_sand_ret = dnx_sand_take_chip_descriptor_mutex(callback_struct->unit);
  if ( _SHR_E_NONE != dnx_sand_ret )
  {
    goto exit;
  }
  /*
   * OK, all check are done, we can proceed with the physical read
   */
  dnx_sand_ret = dnx_sand_mem_read(
               callback_struct->unit,
               callback_struct->result_ptr,
               callback_struct->offset,
               callback_struct->size,
               callback_struct->indirect
             );

  /*
   */
  if ( _SHR_E_NONE != dnx_sand_give_chip_descriptor_mutex(callback_struct->unit) )
  {
    dnx_sand_ret = _SHR_E_INTERNAL;
    goto exit;
  }
  /*
   * There are 2 conditions to call the user supplied callback:
   * 1 - There was an error
   * 2 - The value read from the chip differs from the value
   *     read at the last time (if it is valid)
   */
  if ( (dnx_sand_ret != _SHR_E_NONE) ||
      !(callback_struct->copy_of_result_is_valid) ||
       (callback_struct->copy_of_result_is_valid &&
        dnx_sand_os_memcmp(
          callback_struct->copy_of_last_result_ptr,
          callback_struct->result_ptr,
          DNX_SAND_MIN(callback_struct->size, DNX_SAND_CALLBACK_BUF_SIZE)
       ))
     )
  {
    if (callback_struct->user_callback_proc)
    {
      dnx_sand_ret = callback_struct->user_callback_proc(
                                callback_struct->user_callback_id,
                                callback_struct->result_ptr,
                                &new_buf,
                                callback_struct->size,
                                dnx_sand_ret,
                                callback_struct->unit,
                                callback_struct->offset,
                                callback_struct->dnx_sand_tcm_callback_id,
                                0
                             );
      dnx_sand_os_memcpy(
        callback_struct->copy_of_last_result_ptr,
        callback_struct->result_ptr,
        DNX_SAND_MIN(callback_struct->size, DNX_SAND_CALLBACK_BUF_SIZE)
      );
      callback_struct->copy_of_result_is_valid = TRUE;
      if (new_buf)
      {
        callback_struct->result_ptr =  new_buf;
      }
      /*
       */
      if (_SHR_E_NONE != dnx_sand_ret)
      {
        callback_struct->copy_of_result_is_valid = FALSE;
        goto exit;
      }
    }
  }
  /*
   */
exit:
  SAND_IF_ERR_WARN(NO_UNIT, dnx_sand_ret);
  return dnx_sand_ret;
}

