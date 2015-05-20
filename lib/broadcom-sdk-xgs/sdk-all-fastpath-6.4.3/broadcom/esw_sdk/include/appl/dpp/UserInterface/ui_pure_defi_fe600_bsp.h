/* $Id: ui_pure_defi_fe600_bsp.h,v 1.2 Broadcom SDK $
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


#ifndef __UI_PURE_DEFI_FE600_BSP_INCLUDED__
/* { */
#define __UI_PURE_DEFI_FE600_BSP_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
     #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
     #error  "Add your system support for packed attribute."
#endif

/*
 * Note:
 * the following definitions must range between PARAM_FE600_BSP_START_RANGE_ID
 * and PARAM_FE600_BSP_END_RANGE_ID.
 * See ui_pure_defi.h
 */

#define PARAM_FE600_BSP_RESET_DEVICE_REMAIN_OFF    (PARAM_FE600_BSP_START_RANGE_ID + 0 )
#define PARAM_FE600_BSP_RESET_DEVICE               (PARAM_FE600_BSP_START_RANGE_ID + 1 )
#define PARAM_FE600_BSP_INIT_HOST_BOARD            (PARAM_FE600_BSP_START_RANGE_ID + 2 )

#define PARAM_FE600_BSP_I2C_GEN_READ               (PARAM_FE600_BSP_START_RANGE_ID + 4 )
#define PARAM_FE600_BSP_I2C_GEN_WRITE              (PARAM_FE600_BSP_START_RANGE_ID + 5 )
#define PARAM_FE600_BSP_I2C_BYTE_0                 (PARAM_FE600_BSP_START_RANGE_ID + 6 )
#define PARAM_FE600_BSP_I2C_BYTE_1                 (PARAM_FE600_BSP_START_RANGE_ID + 7 )
#define PARAM_FE600_BSP_I2C_BYTE_2                 (PARAM_FE600_BSP_START_RANGE_ID + 8 )
#define PARAM_FE600_BSP_I2C_BYTE_3                 (PARAM_FE600_BSP_START_RANGE_ID + 9 )
#define PARAM_FE600_BSP_I2C_BYTE_4                 (PARAM_FE600_BSP_START_RANGE_ID + 10)
#define PARAM_FE600_BSP_I2C_BYTE_5                 (PARAM_FE600_BSP_START_RANGE_ID + 11)
#define PARAM_FE600_BSP_I2C_BYTE_6                  (PARAM_FE600_BSP_START_RANGE_ID + 12)
#define PARAM_FE600_BSP_I2C_BYTE_7                 (PARAM_FE600_BSP_START_RANGE_ID + 13)
#define PARAM_FE600_BSP_I2C_BYTE_8                 (PARAM_FE600_BSP_START_RANGE_ID + 14)
#define PARAM_FE600_BSP_I2C_BYTE_9                 (PARAM_FE600_BSP_START_RANGE_ID + 15)
#define PARAM_FE600_BSP_I2C_NOF_BYTES              (PARAM_FE600_BSP_START_RANGE_ID + 16)

#define PARAM_FE600_BSP_I2C_VAL                    (PARAM_FE600_BSP_START_RANGE_ID + 19)
#define PARAM_FE600_BSP_I2C_FE600_WRITE            (PARAM_FE600_BSP_START_RANGE_ID + 20)
#define PARAM_FE600_BSP_I2C_FE600_READ             (PARAM_FE600_BSP_START_RANGE_ID + 21)

#define PARAM_FE600_BSP_SYNT_SET_ID                (PARAM_FE600_BSP_START_RANGE_ID + 22)
#define PARAM_FE600_BSP_SYNT_SET_SYN_TYPE_ID       (PARAM_FE600_BSP_START_RANGE_ID + 23)
#define PARAM_FE600_BSP_SYNT_SET_FREQ_VAL_ID       (PARAM_FE600_BSP_START_RANGE_ID + 24)
/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_PURE_DEFI_FE600_BSP_INCLUDED__*/
#endif
