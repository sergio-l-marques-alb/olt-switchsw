/* $Id: pb_framework.h,v 1.6 Broadcom SDK $
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
* FILENAME:       soc_pb_framework.h
*
* MODULE PREFIX:  soc_petra
*
* FILE DESCRIPTION:
*   This file contains all error-related defines/constants related
*   for soc_petra system application.
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PB_FRAMEWORK_H_INCLUDED__
/* { */
#define __SOC_PB_FRAMEWORK_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_DEBUG           (SOC_SAND_DEBUG)
#define SOC_PB_DEBUG_IS_LVL1   (SOC_PB_DEBUG >= SOC_SAND_DBG_LVL1)
#define SOC_PB_DEBUG_IS_LVL2   (SOC_PB_DEBUG >= SOC_SAND_DBG_LVL2)

/* Device Version managment { */
#define SOC_PB_REVISION_FLD_VAL_A0  0x1
#define SOC_PB_REVISION_FLD_VAL_A1  0x2
#define SOC_PB_REVISION_FLD_VAL_B0  0x3
/* Device Version managment } */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

                                                           
                                                           
                                                           
                                                           
                                                           
                                                           
#define SOC_PB_STRUCT_VERIFY(type, name, exit_num, exit_place) \
res = type##_verify(                                       \
        name                                               \
      );                                                   \
SOC_SAND_CHECK_FUNC_RESULT(res, exit_num, exit_place);

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
typedef enum
{
  SOC_PB_OLP_ID       = 0 ,
  SOC_PB_IRE_ID       = 1 ,
  SOC_PB_IDR_ID       = 2 ,
  SOC_PB_IRR_ID       = 3 ,
  SOC_PB_IHP_ID       = 4 ,
  SOC_PB_QDR_ID       = 5 ,
  SOC_PB_IPS_ID       = 6 ,
  SOC_PB_IPT_ID       = 7 ,
  SOC_PB_DPI_A_ID     = 8 ,
  SOC_PB_DPI_B_ID     = 9 ,
  SOC_PB_DPI_C_ID     = 10,
  SOC_PB_DPI_D_ID     = 11,
  SOC_PB_DPI_E_ID     = 12,
  SOC_PB_DPI_F_ID     = 13,
  SOC_PB_RTP_ID       = 14,
  SOC_PB_EGQ_ID       = 15,
  SOC_PB_SCH_ID       = 16,
  SOC_PB_CFC_ID       = 17,
  SOC_PB_EPNI_ID      = 18,
  SOC_PB_IQM_ID       = 19,
  SOC_PB_MMU_ID       = 20,
  SOC_PB_IHB_ID       = 21,
  SOC_PB_FDT_ID       = 22,
  SOC_PB_NBI_ID       = 23,
  SOC_PB_NOF_MODULES  = 24
}SOC_PB_MODULE_ID;

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
  soc_pb_disp_result(
    SOC_SAND_IN uint32          soc_pb_api_result,
    SOC_SAND_IN char              *proc_name
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PB_FRAMEWORK_H_INCLUDED__*/
#endif

