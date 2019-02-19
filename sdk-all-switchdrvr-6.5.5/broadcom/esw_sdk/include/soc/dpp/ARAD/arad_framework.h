/* $Id: arad_framework.h,v 1.2 Broadcom SDK $
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


#ifndef __ARAD_FRAMEWORK_H_INCLUDED__
/* { */
#define __ARAD_FRAMEWORK_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_DEBUG (SOC_SAND_DEBUG)
#define ARAD_DEBUG_IS_LVL1   (ARAD_DEBUG >= SOC_SAND_DBG_LVL1)
#define ARAD_DEBUG_IS_LVL2   (ARAD_DEBUG >= SOC_SAND_DBG_LVL2)
#define ARAD_DEBUG_IS_LVL3   (ARAD_DEBUG >= SOC_SAND_DBG_LVL3)

/* Device Version managment { */
#define ARAD_REVISION_FLD_VAL_A0  0x0
#define ARAD_REVISION_FLD_VAL_A1  0x10
#define ARAD_REVISION_FLD_VAL_A2  0x30
#define ARAD_REVISION_FLD_VAL_A3  0x32
#define ARAD_REVISION_FLD_VAL_A4  0x832

/* Device Version managment } */

/*
 *    Arad device version,
 *  e.g. Arad-A, Arad-B...
 */

typedef enum
{
  ARAD_DEV_VER_A = 0, /* Assumed zero by the Arad-A driver */
  ARAD_DEV_VER_B,
  ARAD_DEV_NOV_VERS
}ARAD_DEV_VER;


/* } */

/*************
 *  MACROS   *
 *************/
/* { */


#define ARAD_ERR_IF_NULL(e_ptr_, e_err_code) \
{                                         \
  if (e_ptr_ == NULL)                     \
  {                                       \
    ARAD_SET_ERR_AND_EXIT(e_err_code)    \
  }                                       \
}

#define ARAD_INIT_ERR_DEFS                 \
  SOC_SAND_RET m_ret = SOC_SAND_OK;

#define ARAD_SET_ERR_AND_EXIT(e_err_code)  \
{                                           \
  m_ret = e_err_code;                       \
  goto exit ;                               \
}

#define ARAD_EXIT_IF_ERR(e_sand_err,e_err_code) \
{                                         \
  m_ret = soc_sand_get_error_code_from_error_word(e_sand_err); \
  if(m_ret != 0)                          \
  {                                       \
    m_ret = e_err_code ;                  \
    goto exit ;                           \
  }                                       \
}

#define ARAD_RETURN                  \
{                                     \
  return m_ret;                       \
}

#define ARAD_STRUCT_VERIFY(type, name, exit_num, exit_place) \
  res = type##_verify(                                \
        name                                               \
      );                                                   \
SOC_SAND_CHECK_FUNC_RESULT(res, exit_num, exit_place);


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

uint32
  arad_disp_result(
    SOC_SAND_IN uint32          arad_api_result,
    SOC_SAND_IN char              *proc_name
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __ARAD_FRAMEWORK_H_INCLUDED__*/
#endif


