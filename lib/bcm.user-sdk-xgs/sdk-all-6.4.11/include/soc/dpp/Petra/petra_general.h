/* $Id: petra_general.h,v 1.9 Broadcom SDK $
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


#ifndef __SOC_PETRA_GENERAL_INCLUDED__
/* { */
#define __SOC_PETRA_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_reg_access.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_GEN_ERR_NUM_BASE            2000
#define SOC_PETRA_GEN_ERR_NUM_ALLOC           (SOC_PETRA_GEN_ERR_NUM_BASE + 0)
#define SOC_PETRA_GEN_ERR_NUM_CLEAR           (SOC_PETRA_GEN_ERR_NUM_BASE + 6)

#define SOC_PETRA_TOTAL_SIZE_OF_REGS \
    SOC_PETRA_CST_VALUE_DISTINCT(TOTAL_SIZE_OF_REGS, uint32)

/* } */

/*************
 * MACROS    *
 *************/
/* { */

/* No need to check soc_sand_os_memset return value because it returns OK. 
* Checking the return value can cause coverity issue */
#define SOC_PETRA_ALLOC(var, type, count)                                     \
  {                                                                       \
    if(var != NULL)                                                       \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ALLOC_TO_NON_NULL_ERR, SOC_PETRA_GEN_ERR_NUM_ALLOC, exit); \
    }                                                                     \
    var = (type*)soc_sand_os_malloc((count) * sizeof(type), "SOC_PETRA_ALLOC");                  \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_PETRA_GEN_ERR_NUM_ALLOC, exit);  \
    }                                                                     \
    res = soc_sand_os_memset(                                                 \
            var,                                                          \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, SOC_PETRA_GEN_ERR_NUM_ALLOC, exit);\
  }
#define SOC_PETRA_ALLOC_ANY_SIZE(var, type, count)                             \
  {                                                                       \
    if(var != NULL)                                                       \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ALLOC_TO_NON_NULL_ERR, SOC_PETRA_GEN_ERR_NUM_ALLOC, exit); \
    }                                                                     \
    var = (type*)soc_sand_os_malloc_any_size((count) * sizeof(type), "SOC_PETRA_ALLOC_ANY_SIZE");         \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    res = soc_sand_os_memset(                                                 \
            var,                                                          \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, SOC_PETRA_GEN_ERR_NUM_ALLOC, exit);\
  }

#define SOC_PETRA_CLEAR_STRUCT(var, type)                                     \
  {                                                                       \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    soc_petra_##type##_clear(var);                                            \
  }

#define SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(var, type)                           \
  {                                                                       \
    var = (SOC_##type*)soc_sand_os_malloc(sizeof(SOC_##type), "SOC_PETRA_ALLOC_AND_CLEAR_STRUCT");                            \
    SOC_PETRA_CLEAR_STRUCT(var, type);                                        \
  }

#define SOC_PETRA_CLEAR_STRUCT_UNIT(unit, var, type)                                     \
  {                                                                       \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    soc_petra_##type##_clear(unit, var);                                            \
  }


#define SOC_PETRA_ALLOC_AND_CLEAR_STRUCT_UNIT(unit, var, type) \
  { \
     var = (SOC_##type*)soc_sand_os_malloc(sizeof(SOC_##type), "SOC_PETRA_ALLOC_AND_CLEAR_STRUCT");                            \
     SOC_PETRA_CLEAR_STRUCT_UNIT(unit, var, type); \
  }



#define SOC_PETRA_FREE(var)                                                    \
  if (var != NULL)                                                        \
  {                                                                        \
    soc_sand_os_free(var);                                                     \
    var = NULL;                                                           \
  }

#define SOC_PETRA_FREE_ANY_SIZE(var)                                           \
  if (var != NULL)                                                        \
  {                                                                        \
    soc_sand_os_free_any_size(var);                                            \
    var = NULL;                                                           \
  }

#define SOC_PETRA_CLEAR(var_ptr, type, count)                                 \
  {                                                                       \
    res = soc_sand_os_memset(                                                 \
            var_ptr,                                                      \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, SOC_PETRA_GEN_ERR_NUM_CLEAR, exit);           \
  }

#define SOC_PETRA_COPY(var_dest_ptr, var_src_ptr, type, count)                \
  {                                                                       \
    res = soc_sand_os_memcpy(                                                 \
            var_dest_ptr,                                                 \
            var_src_ptr,                                                  \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, SOC_PETRA_GEN_ERR_NUM_CLEAR, exit);           \
  }

#define SOC_PETRA_COMP(var_ptr1, var_ptr2, type, count, is_equal_res)         \
  {                                                                       \
    is_equal_res = SOC_SAND_NUM2BOOL_INVERSE(soc_sand_os_memcmp(                  \
            var_ptr1,                                                     \
            var_ptr2,                                                     \
            (count) * sizeof(type)                                        \
          ));                                                             \
  }

#define SOC_PETRA_API_INVALID_FOR_PETRA_B(err_num, exit_place) \
{                                                          \
  switch(soc_petra_sw_db_ver_get(unit)) {                 \
    case SOC_PETRA_DEV_VER_B:                                  \
      SOC_PETRA_INVALID_DEVICE_ERROR(0, 0, err_num, exit_place)\
      break;                                               \
    case SOC_PETRA_DEV_VER_A:                                  \
    default:                                               \
      break;                                               \
  }                                                        \
}

/* $Id: petra_general.h,v 1.9 Broadcom SDK $
 * Differentiation macros between Soc_petra-A and Soc_petra-B
 */
#define SOC_PETRA_CST_VALUE_DISTINCT(suffix, type) \
  ((SOC_PETRA_IS_DEV_PETRA_A)? SOC_PA_##suffix : (type) SOC_PB_##suffix)

#ifdef LINK_PA_LIBRARIES
#define SOC_PETRA_PA_DEVICE_CALL(func, params)   \
  do                                         \
    {                                        \
    res = soc_pa_##func params;                  \
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);   \
    } while(0)
#else
#define SOC_PETRA_PA_DEVICE_CALL(func, params)                  \
  do                                                        \
    {                                                       \
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FUNC_CALL_NO_PA_ERR, 30, exit); \
    } while(0)
#endif

#ifdef LINK_PB_LIBRARIES
#define SOC_PETRA_PB_DEVICE_CALL(func, params)   \
  do                                         \
    {                                        \
    res = soc_pb_##func params;                  \
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);   \
    } while(0)
#else
#define SOC_PETRA_PB_DEVICE_CALL(func, params)                     \
  do                                                           \
    {                                                          \
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FUNC_CALL_NO_PB_ERR, 30, exit); \
    } while(0)
#endif

#define SOC_PETRA_DIFF_DEVICE_CALL(func,params)  \
  switch (soc_petra_sw_db_ver_get(unit))    \
  {                                          \
  case SOC_PETRA_DEV_VER_A:                      \
    SOC_PETRA_PA_DEVICE_CALL(func, params);      \
    break;                                   \
  case SOC_PETRA_DEV_VER_B:                      \
    SOC_PETRA_PB_DEVICE_CALL(func, params);      \
    break;                                   \
  default:                                   \
    SOC_PETRA_PA_DEVICE_CALL(func, params);      \
    break;                                   \
  }

#define SOC_PETRA_IS_DEV_PETRA_A  (soc_petra_sw_db_ver_get(unit) == SOC_PETRA_DEV_VER_A)
#define SOC_PETRA_IS_DEV_PETRA_B  (soc_petra_sw_db_ver_get(unit) == SOC_PETRA_DEV_VER_B)
#define SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE  \
  (soc_petra_sw_db_ver_get(unit) > SOC_PETRA_DEV_VER_A)


#ifdef LINK_PB_LIBRARIES

#define SOC_PETRA_DBG_FUNC_PRINT_EN   0
  #if SOC_PETRA_DBG_FUNC_PRINT_EN
    #define SOC_PETRA_PB_API_NOT_IMPLEMENTED_YET                  \
      if ((SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE))\
      {                                                       \
         soc_sand_os_printf("This API is not implemented for Soc_petra-B yet: %s\n\r", FUNCTION_NAME());\
         goto exit;                                           \
      }
  #else
#define SOC_PETRA_PB_API_NOT_IMPLEMENTED_YET                  \
  if ((SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE))\
  {                                                       \
     soc_petra_pb_api_not_implemented_err(unit);         \
     goto exit;                                           \
  }
#endif

#else

#define SOC_PETRA_PB_API_NOT_IMPLEMENTED_YET
#endif

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */


#define SOC_PETRA_IF_MAX_RATE_MBPS \
  SOC_PETRA_CST_VALUE_DISTINCT(IF_MAX_RATE_MBPS, uint32)

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

uint32
  soc_petra_interrupt_handler_unsafe(
    SOC_SAND_IN  int                 unit
  );

/*****************************************************
*NAME
*  soc_petra_initialize_database
*TYPE:
*  PROC
*DATE:
*  08/14/2007
*FUNCTION:
*  This procedure initializes the database after registering the device.
*  Once called it should not be called again.
*INPUT:
*  uint32 unit
*    The device id as returned from soc_petra_register_device
*OUTPUT:
*    None
*****************************************************/

/*****************************************************
*NAME
*  soc_petra_sw_db_cfg_ticks_per_sec_get
*TYPE:
*  PROC
*DATE:
*  02-OCT-2007
*FUNCTION:
*  This procedure returns the Soc_petra_ticks_per_sec.
*INPUT:
*  void.
*OUTPUT:
*    None
*****************************************************/
uint32
  soc_petra_sw_db_cfg_ticks_per_sec_get(
    void
  );

uint8
  soc_petra_is_multicast_id_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multicast_id
  );

uint8
  soc_petra_is_queue_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                queue
  );

uint8
  soc_petra_is_flow_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                flow
  );

uint32
  soc_petra_fbr2srd_lane_id(
    SOC_SAND_IN  uint32 fabric_link_id
  );

uint32
  soc_petra_srd2fbr_link_id(
    SOC_SAND_IN  uint32 serdes_lane_id
  );

/*
 *  Internal Rate to clock conversion.
 *  Used for rate configuration, e.g. IPS (IssMaxCrRate),
 *  FMC (FmcMaxCrRate), Guaranteed/Best Effort FMC (GfmcMaxCrRate/BfmcMaxCrRate)
 */
uint32
  soc_petra_intern_rate2clock(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  rate_kbps,
    SOC_SAND_IN  uint8 is_for_ips,
    SOC_SAND_OUT uint32  *clk_interval
  );

/*
 *  Internal Rate to clock conversion.
 *  Used for rate configuration, e.g. IPS (IssMaxCrRate),
 *  FMC (FmcMaxCrRate), Guaranteed/Best Effort FMC (GfmcMaxCrRate/BfmcMaxCrRate)
 */
uint32
  soc_petra_intern_clock2rate(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  clk_interval,
    SOC_SAND_IN  uint8 is_for_ips,
    SOC_SAND_OUT uint32  *rate_kbps
  );

#ifdef LINK_PB_LIBRARIES

void
  soc_petra_pb_api_not_implemented_err(
    SOC_SAND_IN  int              unit
  );

#endif /* LINK_PB_LIBRARIES */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_GENERAL_INCLUDED__*/
#endif
