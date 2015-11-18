/* $Id: sand_api_ssr.c,v 1.5 Broadcom SDK $
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
* FILENAME:       soc_sand_ssr.c
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


#include <soc/dpp/SAND/Management/sand_api_ssr.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_ssr.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

uint32
  soc_sand_ssr_get_buff_size(
    SOC_SAND_OUT uint32* byte_size_ptr
  )
{
  uint32
    ret = 0;

  if(!byte_size_ptr)
  {
    ret = 1;
    goto exit;
  }
  *byte_size_ptr = sizeof(SOC_SAND_SSR_BUFF);
exit:
  return ret;
}

uint32
  soc_sand_ssr_to_buff(
    SOC_SAND_OUT unsigned char* store_buff,
    SOC_SAND_IN  uint32  buff_byte_size
  )
{
  uint32
    ret = 0;

  SOC_SAND_SSR_BUFF
    *curr_ssr_buff = NULL;

  if(!store_buff)
  {
    ret = 1;
    goto exit;
  }

  if(buff_byte_size != sizeof(SOC_SAND_SSR_BUFF))
  {
    ret = 2;
    goto exit;
  }

  curr_ssr_buff =
    (SOC_SAND_SSR_BUFF *)store_buff;

  curr_ssr_buff->header.buffer_size  = buff_byte_size;
  curr_ssr_buff->header.soc_sand_version = soc_get_sand_ver();

  soc_sand_ssr_save_globals(
    curr_ssr_buff
    );


exit:
  return ret;
}

uint32
  soc_sand_ssr_from_buff(
    SOC_SAND_IN  unsigned char* store_buff,
    SOC_SAND_IN  uint32  buff_byte_size
  )
{
  uint32
    ret = 0;
  const SOC_SAND_SSR_BUFF
    *curr_ssr_buff = NULL;

  if(!store_buff)
  {
    ret = 1;
    goto exit;
  }

  /*
   * The buffer hold more information that the driver can handle
   * in the current version.
   */
  if(buff_byte_size > sizeof(SOC_SAND_SSR_BUFF))
  {
    ret = 2;
    goto exit;
  }

  curr_ssr_buff =
    (const SOC_SAND_SSR_BUFF *)store_buff;

  if(buff_byte_size != curr_ssr_buff->header.buffer_size)
  {
    ret = 3;
    goto exit;
  }

  soc_sand_ssr_reload_globals(
      curr_ssr_buff
    );
exit:
  return ret;
}
