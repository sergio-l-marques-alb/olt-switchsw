/* $Id: sand_api_ssr.c,v 1.5 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       dnx_sand_ssr.c
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


#include <soc/dnx/legacy/SAND/Management/sand_api_ssr.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_params.h>
#include <soc/dnx/legacy/SAND/Management/sand_ssr.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnxc/legacy/error.h>

int
  dnx_sand_ssr_get_buff_size(
    DNX_SAND_OUT uint32* byte_size_ptr
  )
{
  int
    ret = _SHR_E_NONE;

  

  if(!byte_size_ptr)
  {
    ret = _SHR_E_INTERNAL;
    goto exit;
  }
  *byte_size_ptr = sizeof(DNX_SAND_SSR_BUFF);
exit:
  return ret;
}

int
  dnx_sand_ssr_to_buff(
    DNX_SAND_OUT unsigned char* store_buff,
    DNX_SAND_IN  uint32  buff_byte_size
  )
{
  int
    ret = _SHR_E_NONE;

  DNX_SAND_SSR_BUFF
    *curr_ssr_buff = NULL;

  

  if(!store_buff)
  {
    ret = _SHR_E_INTERNAL;
    goto exit;
  }

  if(buff_byte_size != sizeof(DNX_SAND_SSR_BUFF))
  {
    ret = _SHR_E_RESOURCE;
    goto exit;
  }

  curr_ssr_buff =
    (DNX_SAND_SSR_BUFF *)store_buff;

  curr_ssr_buff->header.buffer_size  = buff_byte_size;
  curr_ssr_buff->header.dnx_sand_version = dnx_soc_get_sand_ver();

  dnx_sand_ssr_save_globals(
    curr_ssr_buff
    );


exit:
  return ret;
}

int
  dnx_sand_ssr_from_buff(
    DNX_SAND_IN  unsigned char* store_buff,
    DNX_SAND_IN  uint32  buff_byte_size
  )
{
  int
    ret = _SHR_E_NONE;
  const DNX_SAND_SSR_BUFF
    *curr_ssr_buff = NULL;

  

  if(!store_buff)
  {
    ret = _SHR_E_INTERNAL;
    goto exit;
  }

  /*
   * The buffer hold more information that the driver can handle
   * in the current version.
   */
  if(buff_byte_size > sizeof(DNX_SAND_SSR_BUFF))
  {
    ret = _SHR_E_RESOURCE;
    goto exit;
  }

  curr_ssr_buff =
    (const DNX_SAND_SSR_BUFF *)store_buff;

  if(buff_byte_size != curr_ssr_buff->header.buffer_size)
  {
    ret = _SHR_E_PARAM;
    goto exit;
  }

  dnx_sand_ssr_reload_globals(
      curr_ssr_buff
    );
exit:
  return ret;
}
