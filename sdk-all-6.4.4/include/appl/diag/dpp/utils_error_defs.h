/* $Id: utils_error_defs.h,v 1.5 Broadcom SDK $
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


#ifndef __UTILS_ERR_DEFS_H_INCLUDED__
/* { */
#define __UTILS_ERR_DEFS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include "soc/dpp/SAND/Utils/sand_os_interface.h"
#include "soc/dpp/SAND/Management/sand_error_code.h"

/* } */

#define UTILS_GEN_SVR_ERR(e_err_str, e_proc_name, e_err_code, e_svr_wrn) \
    gen_err(FALSE,FALSE,0,0,e_err_str, e_proc_name, e_svr_wrn,\
              e_err_code, TRUE, 0, 601, FALSE)


#define UTILS_INIT_ERR_DEFS(e_proc_name)  \
  unsigned int m_ret = 0;                 \
  unsigned int m_silent_flag;             \
  char* m_proc_name = e_proc_name;        \
  m_silent_flag = 1;

#define UTILS_SET_ERR_AND_EXIT(e_err_code)  \
{                                           \
  m_ret = e_err_code;                       \
  goto exit ;                               \
}

#define UTILS_PRINT_ERR_MSG(e_silent_flag, e_err_msg)   \
{                                                       \
  m_silent_flag = e_silent_flag;                        \
  if (!m_silent_flag)                                   \
  {                                                     \
    soc_sand_os_printf(                                     \
            "\n\r--> ERROR: %s\n\r", e_err_msg);        \
  }                                                     \
}

#define UTILS_PRINT_MSG(e_silent_flag, e_msg)           \
{                                                       \
  m_silent_flag = e_silent_flag;                        \
  if (!m_silent_flag)                                   \
  {                                                     \
    soc_sand_os_printf(                                     \
            "--> %s\n\r", e_msg);                   \
  }                                                     \
}



#define UTILS_EXIT_IF_ERR(e_sand_err,e_err_code) \
{                                         \
  m_ret = soc_sand_get_error_code_from_error_word(e_sand_err); \
  if(m_ret != 0)                          \
  {                                       \
    m_ret = e_err_code ;                  \
    goto exit ;                           \
  }                                       \
}

#define UTILS_FUNC_VALIDATE(e_func_name, e_err_code) \
{                                         \
  if (e_func_name == NULL)                \
  {                                       \
    UTILS_SET_ERR_AND_EXIT(e_err_code)  \
  }                                       \
}

#define UTILS_ERR_IF_NULL(e_ptr, e_err_code) \
{                                           \
  if (e_ptr == NULL)                        \
  {                                         \
    UTILS_SET_ERR_AND_EXIT(e_err_code)      \
  }                                         \
}



/*
 * Macro to handle procedure call which returns standard soc_sand
 * error code and, in case of error, sets error code, performs an exit function and quits.
 * Assumes local variables: ret, error_id, soc_sand_err
 * Assumes label: exit
 */
#define UTILS_EXIT_AND_PRINT_IF_ERR(e_result, e_silent_flag, e_err_code, e_err_msg)     \
  m_silent_flag = e_silent_flag;                        \
  m_ret = soc_sand_get_error_code_from_error_word(e_result);\
  if (SOC_SAND_OK != m_ret)                                 \
  {                                                     \
   /*                                                   \
    * Error detected. Quit with error.                  \
    */                                                  \
    UTILS_PRINT_ERR_MSG(m_silent_flag, e_err_msg)       \
                                                        \
    goto exit ;                                         \
  }


#define UTILS_EXIT_AND_PRINT_ERR           \
{                                          \
  if ((m_ret) && (m_silent_flag == FALSE)) \
    {                                      \
      soc_sand_os_printf(\
        "\n\r    + %-60s -- ERROR %u.\n\r",m_proc_name, m_ret);\
    }                                      \
    return m_ret;                          \
}

#ifdef  __cplusplus
}
#endif


/* } __UTILS_ERR_DEFS_H_INCLUDED__*/
#endif


