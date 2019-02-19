/* $Id: ui_pure_defi_fap21v_app.h,v 1.2 Broadcom SDK $
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


#ifndef __UI_PURE_DEFI_FAP21V_APP_INCLUDED__
/* { */
#define __UI_PURE_DEFI_FAP21V_APP_INCLUDED__

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
 * the following definitions must range between PARAM_FAP21V_API_START_RANGE_ID
 * and PARAM_FAP21V_API_END_RANGE_ID.
 * See ui_pure_defi.h
 */

#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_PORT_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 1)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_NOMINAL_RATE_MBPS_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 2)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_SCHEME_ID_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 3)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MINIMUM_RATE_BY_PERCENTAGE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 4)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MAXIMUM_RATE_BY_PERCENTAGE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 5)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 7)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_WEIGHT_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 8)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_SWEEP21V_SCH_HR_CLASS_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 9)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MAX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 10)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MIN_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 11)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 12)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 13)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 14)
#define PARAM_FAP21V_APP_SSR_SAVE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 15)
#define PARAM_FAP21V_APP_SSR_RESTORE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 16)
#define PARAM_FAP21V_APP_SSR_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 17)
#define PARAM_FAP21V_APP_FE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 18)
#define PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 19)
#define PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_FE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 20)
#define PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 21)
#define PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_FE_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 22)

#define PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 23)
#define PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_FAP_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 24)
#define PARAM_FAP21V_APP_SERDES_POWER_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 26)
#define PARAM_FAP21V_APP_SERDES_POWER_DOWN_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 27)
#define PARAM_FAP21V_APP_SERDES_POWER_DOWN_FE_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 28)
#define PARAM_FAP21V_APP_SERDES_POWER_DOWN_SERDES_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 29)
#define PARAM_FAP21V_APP_SERDES_POWER_UP_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 30)
#define PARAM_FAP21V_APP_SERDES_POWER_UP_FE_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 31)
#define PARAM_FAP21V_APP_SERDES_POWER_UP_SERDES_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 32)
#define PARAM_FAP21V_APP_LINKS_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 33)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_FAP_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 34)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_FAP_NDX_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 35)
#define PARAM_FAP21V_APP_SCHEDULING_SCHEMES_ID \
                       (PARAM_FAP21V_APP_START_RANGE_ID + 36)
/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_PURE_DEFI_FAP21V_APP_INCLUDED__*/
#endif
