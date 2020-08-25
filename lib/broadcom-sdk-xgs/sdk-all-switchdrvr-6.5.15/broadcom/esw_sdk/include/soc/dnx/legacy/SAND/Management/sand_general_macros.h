/* $Id: sand_general_macros.h,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
#ifndef  DNX_SAND_DRIVER_SHORTCUTS_H
#define DNX_SAND_DRIVER_SHORTCUTS_H

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

#ifdef  __cplusplus
extern "C"
{
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <bcm/debug.h>

/* $Id: sand_general_macros.h,v 1.20 Broadcom SDK $
 * MACROs for validation code.
 */

#define DNX_SAND_CHECK_DRIVER_AND_DEVICE                        \
  dnx_sand_check_driver_and_device(unit, &ex);             \
  if (ex != no_err)                                         \
  {                                                         \
    DNX_SAND_DRIVER_AND_DEVICE_PRINT \
    goto exit;                                              \
  }

/*PCID LITE VERSION - skip code not relevant for arrakis in order to improve init time*/
#ifdef BCM_PCID_LITE
#define DNX_SAND_PCID_LITE_SKIP(unit)                         \
    goto exit
#else
#define DNX_SAND_PCID_LITE_SKIP(unit)                         
#endif

#define DNX_SAND_TAKE_DEVICE_SEMAPHORE                             \
  if (_SHR_E_NONE != dnx_sand_take_chip_descriptor_mutex(unit) )  \
  {                                                            \
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DNX_SAND_SEM_TAKE_FAIL"); \
    goto exit;                                                 \
  }


#define DNX_SAND_GIVE_DEVICE_SEMAPHORE                              \
  if (_SHR_E_NONE != dnx_sand_give_chip_descriptor_mutex(unit) )   \
  {                                                             \
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DNX_SAND_SEM_GIVE_FAIL"); \
    goto exit;                                                  \
  }


#define DNX_SAND_MALLOC(snd_ptr, snd_size, str)        \
{                                             \
    snd_ptr = dnx_sand_os_malloc(snd_size, str);       \
    if(snd_ptr == NULL)                       \
    {                                         \
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DNX_SAND_MALLOC_FAIL"); \
    goto exit;                                \
    }                                         \
}

/*
 * for save functions
 *  assume the following params: cur_size, total_size, buffer_size_bytes, res 
 *  defined locally
 */
#define DNX_SAND_COPY_TO_BUFF_AND_INC(var_dest_ptr, var_src_ptr, type, count)        \
  {                                                                       \
    if ((var_src_ptr == NULL) || (var_dest_ptr == NULL))                  \
    {                                                                     \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_SAND_GEN_ERR"); \
    }                                                                     \
    cur_size = (count) * sizeof(type);                       \
    total_size += cur_size;                                               \
    if (total_size > buffer_size_bytes)                                  \
    {                                                                     \
      SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_SAND_GEN_ERR");  \
    }                                                                     \
    res = dnx_sand_os_memcpy(                                                 \
            var_dest_ptr,                                                 \
            var_src_ptr,                                                  \
            cur_size                                                      \
          );                                                              \
    SHR_IF_ERR_EXIT(res);           \
    var_dest_ptr += cur_size;                                             \
  }

/*
 * for use in the exit part only
 */
#define DNX_SAND_FREE(snd_ptr) \
    {                             \
      if (snd_ptr)                \
      {                           \
        dnx_sand_os_free(snd_ptr);    \
        snd_ptr = NULL;           \
      }                           \
    }

/*
 * for use in the exit part only
 */

#define DNX_SAND_INTERRUPT_INIT_DEFS                      \
  uint32          __sand_macro_int_flags = 0;           \
  uint32  __sand_macro_is_int_stopped = FALSE

/*
 *	This MACRO is used to stop all interrupts.
 *  It is often used to prevent context switch in
 *  the areas where per-device semaphore cannot be taken 
 *  (e.g. before device registration).
 *  Please note that in multi-core systems it is not enough 
 *  to stop interrupts to prevent context-switch.
 *  In this case, the macro implementation should also take
 *  a global OS semaphore preventing any task-switching.
 */
#define DNX_SAND_INTERRUPTS_STOP                        \
{                                                   \
  dnx_sand_os_stop_interrupts(&__sand_macro_int_flags); \
  __sand_macro_is_int_stopped = TRUE;               \
}

/*
 *	Release interrupt lock as defined in "DNX_SAND_INTERRUPTS_STOP"
 *  Please note that if a global OS semaphore was also taken here
 *  to prevent task-switching, it should be release here.
 */
#define DNX_SAND_INTERRUPTS_START_IF_STOPPED              \
{                                                     \
  if (__sand_macro_is_int_stopped == TRUE)            \
  {                                                   \
    dnx_sand_os_start_interrupts(__sand_macro_int_flags); \
    __sand_macro_is_int_stopped = FALSE ;             \
  }                                                   \
}
/*
* Error if val_to_check is below min_val
*/
#define DNX_SAND_ERR_IF_BELOW_MIN(val_to_check, min_val,err_e,err_num,err_exit_label) \
{                                                                             \
  if ((val_to_check) < (min_val))                                                 \
  {                                                                           \
   SHR_ERR_EXIT(_SHR_E_PARAM, "Value below the allowed minimum.");   \
  }                                                                           \
}

/*see documentation for DNX_SAND_ERR_IF_BELOW_MIN() above.*/
#define DNX_SAND_ERR_IF_ABOVE_MAX(val_to_check, max_val,err_e,err_num,err_exit_label) \
{                                                                             \
  if ((val_to_check) > (max_val))                                                 \
  {                                                                           \
   SHR_ERR_EXIT(_SHR_E_PARAM, "Value above the allowed maximum.");                \
  }                                                                           \
}

/*see documentation for DNX_SAND_ERR_IF_BELOW_MIN() above.*/
#define DNX_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(val_to_check, max_val, null_val, err_e,err_num,err_exit_label) \
{                                                                             \
  if (((val_to_check) != (null_val)) && ((val_to_check) > (max_val)))         \
  {                                                                           \
   SHR_ERR_EXIT(_SHR_E_PARAM, "Value is not NULL and is above the allowed maximum.");                      \
  }                                                                           \
}

/*see documentation for DNX_SAND_ERR_IF_BELOW_MIN() above.*/
#define DNX_SAND_ERR_IF_ABOVE_NOF(val_to_check, nof,err_e,err_num,err_exit_label) \
{                                                                             \
  if ((val_to_check) >= (nof))                                                 \
  {                                                                           \
   SHR_ERR_EXIT(_SHR_E_PARAM, "Value above the allowed number.");                     \
  }                                                                           \
}

/*see documentation for DNX_SAND_ERR_IF_BELOW_MIN() above.*/
#define DNX_SAND_ERR_IF_OUT_OF_RANGE(val_to_check, min_val, max_val,err_e,err_num,err_exit_label) \
{                                                                         \
  if (((val_to_check) < (min_val)) || ((val_to_check) > (max_val)))       \
  {                                                                       \
   SHR_ERR_EXIT(_SHR_E_PARAM, "Value is out of range.");                  \
  }                                                                       \
}

/*see documentation for DNX_SAND_ERR_IF_BELOW_MIN() above.*/
#define DNX_SAND_ERR_IF_EQUALS_VALUE(val_to_check, val, err_e, err_num, err_exit_label) \
{                                                                         \
  if ((val_to_check) == (val))                                                \
  {                                                                       \
   SHR_ERR_EXIT(_SHR_E_PARAM, "Value1 is equal to value2.");     \
  }                                                                       \
}

/*see documentation for DNX_SAND_ERR_IF_BELOW_MIN() above.*/
#define DNX_SAND_ERR_IF_NOT_EQUALS_VALUE(val_to_check, val, err_e, err_num, err_exit_label) \
{                                                                         \
  if ((val_to_check) != (val))                                                \
  {                                                                       \
   SHR_ERR_EXIT(_SHR_E_PARAM, "Value1 is not equal to value2.");      \
  }                                                                       \
}

#define DNX_SAND_IS_VAL_IN_RANGE(val_to_check, min_val, max_val) \
  (((val_to_check) >= (min_val)) && ((val_to_check) <= (max_val)))

#define DNX_SAND_IS_VAL_OUT_OF_RANGE(val_to_check, min_val, max_val) \
  !(DNX_SAND_IS_VAL_IN_RANGE((val_to_check), (min_val), (max_val)))

#define DNX_SAND_LIMIT_FROM_ABOVE(val, limit) \
{                                         \
  if (val>limit)                          \
  {                                       \
    val=limit;                            \
  }                                       \
}

#define DNX_SAND_LIMIT_FROM_BELOW(val, limit) \
{                                         \
  if (val<limit)                          \
  {                                       \
    val=limit;                            \
  }                                       \
}

#define DNX_SAND_LIMIT_VAL(val, min_limit, max_limit) \
{                                                 \
  DNX_SAND_LIMIT_FROM_ABOVE(val, max_limit);          \
  DNX_SAND_LIMIT_FROM_BELOW(val, min_limit);          \
};

/* Max value for bit length up to 32 bits */
#define DNX_SAND_MAX_VAL_FOR_BIT_LEN(val, len)                          \
{                                                                       \
  if (DNX_SAND_IS_VAL_IN_RANGE((int)len, 0, 32))                        \
  {                                                                     \
    if (len == 32)                                                      \
    {                                                                   \
      val = DNX_SAND_U32_MAX;                                           \
    }                                                                   \
    else                                                                \
    {                                                                   \
      val = ((1 << len) - 1);                                           \
    }                                                                   \
  } else {                                                              \
   SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DNX_SAND_VALUE_OUT_OF_RANGE_ERR");     \
  }                                                                     \
}

#define SAND_IF_ERR_WARN(unit, _expr)      \
{                       \
  int _func_unit=unit;      \
  int _rv = _expr;      \
  if (SHR_FAILURE(_rv)) \
  {                     \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%s' indicated ; %s%s%s\r\n" ,           \
      shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;  \
  }                     \
}

#ifdef  __cplusplus
}
#endif

#endif /*DNX_SAND_DRIVER_SHORTCUTS_H*/
