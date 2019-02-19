/* $Id$
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
* FILENAME:       soc_sand_ssr.h
*
* AUTHOR:         Dune (U.C.)
*
* FILE DESCRIPTION:
*
* REMARKS:
*
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
*******************************************************************/

#ifndef __SOC_SAND_SSR_H_INCLUDED__
/* { */
#define __SOC_SAND_SSR_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Management/sand_api_ssr.h>

typedef enum{
  SOC_SAND_SSR_CFG_VERSION_INVALID = 0,
  SOC_SAND_SSR_CFG_VERSION_NUM_0 = 1,
  SOC_SAND_SSR_CFG_VERSION_NUM_1 = 2
}SOC_SAND_SSR_CFG_VERSION_NUM;


typedef struct
{
  uint32 soc_sand_big_endian_was_checked;
  uint32 soc_sand_big_endian;
  uint32 soc_sand_physical_print_when_writing;
  uint32 soc_sand_physical_print_asic_style;
}SOC_SAND_SSR_DATA;

typedef struct
{
  SOC_SAND_SSR_HEADER header;
  SOC_SAND_SSR_DATA   data;
}SOC_SAND_SSR_BUFF;

uint32
  soc_sand_ssr_reload_globals(
      SOC_SAND_IN SOC_SAND_SSR_BUFF *curr_ssr_buff
    );
uint32
  soc_sand_ssr_save_globals(
      SOC_SAND_SSR_BUFF *curr_ssr_buff
    );

/*****************************************************
*NAME
*  soc_sand_ssr_get_ver_from_header
*TYPE:
*  PROC
*DATE:
*  22/02/2006
*FUNCTION:
*  gets soc_sand version number given a pointer to an SSR header
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_SSR_HEADER* header - pointer to an SSR header
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 - soc_sand error word
*  SOC_SAND_INDIRECT:
*    uint32* soc_sand_ver  - the soc_sand version extracted from SSR buffer header
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_ssr_get_ver_from_header(
      SOC_SAND_IN SOC_SAND_SSR_HEADER* header,
      SOC_SAND_OUT uint32* soc_sand_ver
  );

/*****************************************************
*NAME
*  soc_sand_ssr_get_ver_from_header
*TYPE:
*  PROC
*DATE:
*  22/02/2006
*FUNCTION:
*  gets SSR buffer size given a pointer to an SSR header
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_SSR_HEADER* header - pointer to an SSR header
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 - soc_sand error word
*  SOC_SAND_INDIRECT:
*    uint32* buff_size  - the size of the SSR buffer, extracted from SSR buffer header
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_ssr_get_size_from_header(
      SOC_SAND_IN SOC_SAND_SSR_HEADER* header,
      SOC_SAND_OUT uint32* buff_size
  );

#ifdef  __cplusplus
}
#endif

/* } __SOC_SAND_SSR_H_INCLUDED__*/
#endif
