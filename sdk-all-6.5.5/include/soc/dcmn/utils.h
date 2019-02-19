/* $Id: SOCDNX_general.h,v 1.8 Broadcom SDK $
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


#ifndef __SOCDNX_GENERAL_INCLUDED__
/* { */
#define __SOCDNX_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 * MACROS    *
 *************/
/* { */

#define SOCDNX_ALLOC_ANY_SIZE(var, type, count,str)                       \
  do {                                                                    \
    if(var != NULL)                                                       \
    {                                                                     \
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_SOCDNX_MSG("Trying to allocate to a non null ptr is forbidden"))); \
    }                                                                     \
    var = (type*)soc_sand_os_malloc_any_size((count) * (uint32)sizeof(type),str); \
    if (var == NULL)                                                      \
    {                                                                     \
      SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_SOCDNX_MSG("Failed to allocate memory")));\
    }                                                                     \
    _rv = soc_sand_os_memset(                                                 \
            var,                                                          \
            0x0,                                                          \
            (count) * (uint32)sizeof(type)                                \
          );                                                              \
    SOCDNX_SAND_IF_ERR_EXIT(_rv);                                          \
  } while (0);


#define SOCDNX_CLEAR(var_ptr, type, count)                                \
  do {                                                                    \
    _rv = soc_sand_os_memset(                                             \
            var_ptr,                                                      \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOCDNX_IF_ERR_EXIT(_rv);                                              \
  } while (0);

#define SOCDNX_COPY(var_dest_ptr, var_src_ptr, type, count)                \
  do {                                                                    \
    _rc = soc_sand_os_memcpy(                                             \
            var_dest_ptr,                                                 \
            var_src_ptr,                                                  \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOCDNX_IF_ERR_EXIT(_rv);                                              \
  } while (0);

#define SOCDNX_COMP(var_ptr1, var_ptr2, type, count, is_equal_res)         \
  do {                                                                     \
    is_equal_res = SOC_SAND_NUM2BOOL_INVERSE(soc_sand_os_memcmp(                  \
            var_ptr1,                                                     \
            var_ptr2,                                                     \
            (count) * sizeof(type)                                        \
          ));                                                             \
  } while (0);

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

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOCDNX_GENERAL_INCLUDED__*/
#endif



