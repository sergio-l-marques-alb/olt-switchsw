/* $Id: soc_pb_api_ssr.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_API_SSR_H_INCLUDED__
/* { */
#define __SOC_PB_API_SSR_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_api_ssr.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum{
  SOC_PB_SW_DB_CFG_VERSION_INVALID = 0,
  SOC_PB_SW_DB_CFG_VERSION_NUM_0 = 1,
  SOC_PB_SW_DB_CFG_VERSION_NUM_1 = 2,
  SOC_PB_SW_DB_CFG_NOF_VERSIONS
}SOC_PB_SW_DB_CFG_VERSION_NUM;

#define SOC_PB_SW_DB_CFG_NOF_VALID_VERSIONS (SOC_PB_SW_DB_CFG_NOF_VERSIONS - 1)

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
  soc_pb_ssr_buff_size_get(
    SOC_SAND_IN  int   		    unit,
    SOC_SAND_OUT uint32                       *byte_size_ptr
  );

uint32
  soc_pb_ssr_to_buff(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint8                        *store_buff,
    SOC_SAND_IN  uint32                       buff_byte_size
  );

uint32
  soc_pb_ssr_from_buff(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  );

/*********************************************************************
* NAME:
*   soc_pb_ssr_is_device_init_done
* TYPE:
*   PROC
* FUNCTION:
*   Serves as a hot-start indication. The application can
*   call this API upon initialization, after the device
*   registration and before the driver initialization
*   sequence (any stage before init_phase1) to check if the
*   device initialization was already performed, meaning
*   this is a hot-start.
* INPUT:
*   SOC_SAND_IN  int unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT uint8 *is_init_done -
*     If TRUE, the device initialization is completed.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ssr_is_device_init_done(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *is_init_done
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_SSR_H_INCLUDED__*/
#endif
