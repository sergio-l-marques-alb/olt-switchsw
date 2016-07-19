/* $Id: petra_api_ingress_header_parsing.c,v 1.9 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_ingress_header_parsing.c
*
* MODULE PREFIX:  soc_petra_ihp
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_ingress_header_parsing.h>
#include <soc/dpp/Petra/petra_ingress_header_parsing.h>

#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_general.h>

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

/*********************************************************************
*     Set ingress port configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_port_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_IHP_PORT_INFO       *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ihp_port_verify(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ihp_port_set_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_port_set()",0,0);
}

/*********************************************************************
*     Set ingress port configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_port_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_IHP_PORT_INFO       *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_PORT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ihp_port_get_unsafe(
    unit,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_port_get()",0,0);
}

/*********************************************************************
*     Define the position and length of the statistics tag
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_stag_hdr_data_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_STAG_HDR_DATA_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_stag_hdr_data_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ihp_stag_hdr_data_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_stag_hdr_data_set()",0,0);
}

/*********************************************************************
*     Define the position and length of the statistics tag
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_stag_hdr_data_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_STAG_HDR_DATA_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ihp_stag_hdr_data_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_stag_hdr_data_get()",0,0);
}

/*********************************************************************
*     Define the position and length of the TM-LAG Hushing
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_tmlag_hush_field_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_TMLAG_HUSH_FIELD_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_tmlag_hush_field_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ihp_tmlag_hush_field_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_tmlag_hush_field_set()",0,0);
}

/*********************************************************************
*     Define the position and length of the TM-LAG Hushing
*     field in incoming packet header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ihp_tmlag_hush_field_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IHP_TMLAG_HUSH_FIELD_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ihp_tmlag_hush_field_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ihp_tmlag_hush_field_get()",0,0);
}

void
  soc_petra_PETRA_IHP_PCKT_STAG_HDR_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_STAG_HDR_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_IHP_PCKT_STAG_HDR_DATA));
  info->base_header = SOC_PETRA_IHP_NOF_PCKT_HEADER_IDS;
  info->offset_4bits = 0;
  info->length_4bits = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_IHP_PCKT_TMLAG_HUSH_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO));
  info->base_header = SOC_PETRA_IHP_NOF_PCKT_HEADER_IDS;
  info->offset_4bits = 0;
  info->length_4bits = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_IHP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IHP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_IHP_PORT_INFO));
  info->sop2header_offset_bytes = 0;
  info->strip_from_sop_bytes = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_IHP_PCKT_HEADER_ID_to_string(
    SOC_SAND_IN SOC_PETRA_IHP_PCKT_HEADER_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_IHP_PCKT_HEADER_ID_SOP:
    str = "PCKT_HEADER_ID_SOP";
  break;

  case SOC_PETRA_IHP_PCKT_HEADER_ID_0:
    str = "PCKT_HEADER_ID_0";
  break;

  case SOC_PETRA_IHP_PCKT_HEADER_ID_1:
    str = "PCKT_HEADER_ID_1";
  break;

  case SOC_PETRA_IHP_PCKT_HEADER_ID_2:
    str = "PCKT_HEADER_ID_2";
  break;

  case SOC_PETRA_IHP_NOF_PCKT_HEADER_IDS:
    str = "NOF_PCKT_HEADER_IDS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


void
  soc_petra_PETRA_IHP_PCKT_STAG_HDR_DATA_print(
    SOC_SAND_IN SOC_PETRA_IHP_PCKT_STAG_HDR_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "  Base_header %s \n\r",
    soc_petra_PETRA_IHP_PCKT_HEADER_ID_to_string(info->base_header)
  );
  soc_sand_os_printf("  Offset_4bits: %u[Nibbles]\n\r",info->offset_4bits);
  soc_sand_os_printf("  Length_4bits: %u[Nibbles]\n\r",info->length_4bits);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_IHP_PCKT_TMLAG_HUSH_INFO_print(
    SOC_SAND_IN SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "Base_header %s \n\r",
    soc_petra_PETRA_IHP_PCKT_HEADER_ID_to_string(info->base_header)
  );
  soc_sand_os_printf("Offset_4bits: %u[Nibbles]\n\r",info->offset_4bits);
  soc_sand_os_printf("Length_4bits: %u[Nibbles]\n\r",info->length_4bits);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}




void
  soc_petra_PETRA_IHP_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_IHP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("  Sop2header_offset_bytes: %u[Bytes]\n\r",info->sop2header_offset_bytes);
  soc_sand_os_printf("  Strip_from_sop_bytes: %u[Bytes]\n\r",info->strip_from_sop_bytes);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>
