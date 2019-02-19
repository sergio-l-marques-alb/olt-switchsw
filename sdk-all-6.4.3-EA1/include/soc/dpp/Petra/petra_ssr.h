/* $Id: petra_ssr.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_petra_ssr.h
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PETRA_SSR_H_INCLUDED__
/* { */
#define __SOC_PETRA_SSR_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_api_ssr.h>

#include <soc/dpp/Petra/petra_api_ssr.h>
#include <soc/dpp/Petra/petra_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_SSR_BASE_SAND_VER 2830 /* first driver version supporting soc_petra SSR */

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

typedef struct
{
  SOC_PETRA_SW_DB_DEVICE_SSR_V01  ssr_v01;

}  SOC_PETRA_SSR_DATA_V01;

typedef SOC_PETRA_SSR_DATA_V01 SOC_PETRA_SSR_DATA;

typedef struct
{
  uint32
    prev_unit;

  uint32
    fap_id;

}  SOC_PETRA_SSR_HEADER;

typedef struct
{
  SOC_SAND_SSR_HEADER   header;

  SOC_PETRA_SSR_HEADER device_header;

  union
  {
    SOC_PETRA_SSR_DATA ssr_data;

    uint8         stream[sizeof(SOC_PETRA_SSR_DATA)];
  }info;

}  SOC_PETRA_SSR_BUFF;

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
  soc_petra_ssr_sw_db_size_get(
    SOC_SAND_IN  uint32                      ver_idx,
    SOC_SAND_OUT uint32                       *size
  );

uint32
  soc_petra_ssr_to_buff_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_OUT uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  );

uint32
  soc_petra_ssr_from_buff_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  );

/*********************************************************************
* NAME:
*   soc_petra_ssr_is_device_init_done_unsafe
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
  soc_petra_ssr_is_device_init_done_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *is_init_done
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_SSR_H_INCLUDED__*/
#endif
