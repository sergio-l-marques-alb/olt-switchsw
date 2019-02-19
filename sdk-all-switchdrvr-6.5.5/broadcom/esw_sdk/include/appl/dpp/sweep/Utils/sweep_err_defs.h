/* $Id: sweep_err_defs.h,v 1.2 Broadcom SDK $
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


#ifndef __SWEEP_ERR_DEFS_H_INCLUDED__
/* { */
#define __SWEEP_ERR_DEFS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#define SWP_PRINT_MSG(e_silent_flag, e_msg)   \
{                                             \
  if (!e_silent_flag)                         \
  {                                           \
    soc_sand_os_printf(                           \
            "\r--> %s\n\r", e_msg);         \
  }                                           \
}



#define SWP_INIT_ERR_DEFS(e_proc_name)    \
  unsigned int m_ret = 0;                 \
  unsigned int m_silent_flag;             \
  char* m_proc_name = e_proc_name;        \
  m_silent_flag = 1;

#define SWP_SET_ERR_AND_EXIT(e_err_code)  \
{                                         \
  m_ret = e_err_code;                       \
  goto exit ;                             \
}

#define SWP_PRINT_ERR_MSG(e_silent_flag, e_err_msg)     \
{                                                       \
  m_silent_flag = e_silent_flag;                        \
  if (!m_silent_flag)                                   \
  {                                                     \
    soc_sand_os_printf(                                     \
            "\n\r--> ERROR: %s\n\r", e_err_msg);        \
  }                                                     \
}

#define SWP_SET_ERR_AND_EXIT_WITH_MSG(e_err_code, e_err_msg) \
  {                                                         \
  m_ret = e_err_code;                                     \
  SWP_PRINT_MSG(FALSE, e_err_msg);                             \
  goto exit ;                                             \
}


#define SWP_EXIT_IF_ERR(e_sand_err,e_err_code) \
{                                         \
  if(soc_sand_get_error_code_from_error_word(e_sand_err))                          \
  {                                       \
    m_ret = e_err_code ;                  \
    goto exit ;                           \
  }                                       \
}

#define SWP_IS_ERR_RES(e_sand_err) (soc_sand_get_error_code_from_error_word(e_sand_err) == 0?FALSE:TRUE)

/*
 * Macro to handle procedure call which returns standard soc_sand
 * error code and, in case of error, sets error code, performs an exit function and quits.
 * Assumes local variables: ret, error_id, soc_sand_err
 * Assumes label: exit
 */
#define SWP_EXIT_AND_PRINT_IF_ERR(e_result, e_silent_flag, e_err_code, e_err_msg)     \
  m_silent_flag = e_silent_flag;                        \
  m_ret = soc_sand_get_error_code_from_error_word(e_result);\
  if (SOC_SAND_OK != m_ret)                                 \
  {                                                    \
   /*                                                  \
    * Error detected. Quit with error.                 \
    */                                                 \
    SWP_PRINT_ERR_MSG(m_silent_flag, e_err_msg)        \
                                                       \
    goto exit ;                                        \
  }


#define SWP_EXIT_AND_PRINT_ERR        \
{                                     \
  if (m_ret)                            \
    {                                 \
      soc_sand_os_printf(\
        "\n\r    + %-60s -- ERROR %u.\n\r",m_proc_name, m_ret);\
    }                                 \
    return m_ret;                       \
}

#ifdef  __cplusplus
}
#endif


/* } __SWEEP_ERR_DEFS_H_INCLUDED__*/
#endif


