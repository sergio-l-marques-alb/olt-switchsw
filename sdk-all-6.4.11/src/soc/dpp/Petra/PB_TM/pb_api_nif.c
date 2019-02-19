/* $Id: pb_api_nif.c,v 1.11 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/petra_api_nif.h>
#include <soc/dpp/Petra/petra_api_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_nif.h>

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

/* $Id: pb_api_nif.c,v 1.11 Broadcom SDK $
 *	This utility converts from Soc_petra-A-style indexing
 *  to Soc_petra-B-style indexing.
 *  Only Soc_petra-B indexing can be used for NIF configuration
 */
uint32 soc_pb_nif_pa2pb_id(
            SOC_PB_NIF_TYPE         nif_type,
            SOC_PETRA_INTERFACE_ID  soc_pa_nif_id
          )
{
  SOC_PETRA_INTERFACE_ID
    soc_pb_id = SOC_PETRA_NIF_ID_NONE;

  if (SOC_PB_NIF_IS_PB_ID(soc_pa_nif_id))
  {
    /*
     *	Already PB-style
     */
    soc_pb_id = soc_pa_nif_id;
  }
  else
  {
    if (SOC_PB_NIF_IS_PA_ID(soc_pa_nif_id))
    {
      switch(nif_type) {
      case SOC_PB_NIF_TYPE_XAUI:
        soc_pb_id = SOC_PB_NIF_ID_XAUI_FIRST + SOC_PA_NIF2MAL_NDX(soc_pa_nif_id);
    	  break;
      case SOC_PB_NIF_TYPE_SGMII:
        soc_pb_id = SOC_PB_NIF_ID_SGMII_FIRST + soc_pa_nif_id;
    	  break;
      case SOC_PB_NIF_TYPE_RXAUI:
        soc_pb_id = SOC_PB_NIF_ID_RXAUI_FIRST + SOC_PA_NIF2MAL_NDX(soc_pa_nif_id);
    	  break;
      case SOC_PB_NIF_TYPE_QSGMII:
      case SOC_PB_NIF_TYPE_ILKN:
      default:
        /*
         *	This type is not supported for Soc_petra-A, and
         *  therefore Soc_petra-A index should not be used
         */
        soc_pb_id = SOC_PETRA_NIF_ID_NONE;
        break;
      }
    }
  }

  return soc_pb_id;
}

/*
 *	Derive Interface type
 *  from  Soc_petra-B style index
 */
SOC_PB_NIF_TYPE
  soc_pb_nif_id2type(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID  soc_pb_nif_id
  )
{
  SOC_PB_NIF_TYPE
    nif_type;

  if (SOC_PB_NIF_IS_TYPE_ID(XAUI, soc_pb_nif_id))
  {
    nif_type = SOC_PB_NIF_TYPE_XAUI;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(RXAUI, soc_pb_nif_id))
  {
    nif_type = SOC_PB_NIF_TYPE_RXAUI;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(SGMII, soc_pb_nif_id))
  {
    nif_type = SOC_PB_NIF_TYPE_SGMII;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(QSGMII, soc_pb_nif_id))
  {
    nif_type = SOC_PB_NIF_TYPE_QSGMII;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(ILKN, soc_pb_nif_id))
  {
    nif_type = SOC_PB_NIF_TYPE_ILKN;
  }
  else
  {
    nif_type = SOC_PB_NIF_TYPE_NONE;
  }

  return nif_type;
}

/*
 *	Derive Soc_petra Interface ID
 *  from Soc_petra-B style type, index
 */
SOC_PETRA_INTERFACE_ID
  soc_pb_nif_type2id(
    SOC_SAND_IN SOC_PB_NIF_TYPE soc_pb_nif_type,
    SOC_SAND_IN uint32 internal_id
  )
{
  SOC_PETRA_INTERFACE_ID
    nif_id;
  
  switch(soc_pb_nif_type) {
    case SOC_PB_NIF_TYPE_XAUI:
      nif_id = SOC_PB_NIF_ID(XAUI, internal_id);
  	  break;
    case SOC_PB_NIF_TYPE_RXAUI:
      nif_id = SOC_PB_NIF_ID(RXAUI, internal_id);
  	  break;
    case SOC_PB_NIF_TYPE_SGMII:
      nif_id = SOC_PB_NIF_ID(SGMII, internal_id);
  	  break;
    case SOC_PB_NIF_TYPE_QSGMII:
      nif_id = SOC_PB_NIF_ID(QSGMII, internal_id);
  	  break;
    case SOC_PB_NIF_TYPE_ILKN:
      nif_id = SOC_PB_NIF_ID(ILKN, internal_id);
  	  break;
    default:
      nif_id = SOC_PETRA_NIF_ID_NONE;
    }
  

  return nif_id;

}

/*
 *	This utility converts from Soc_petra-B-style indexing
 *  to Soc_petra-A-style indexing.
 *  Only Soc_petra-B indexing can be used for NIF configuration
 *  Soc_petra-A style indexing can be used in Soc_petra-A compatible APIs
 *  that do not configure the NIF (e.g. soc_petra_sch_if_shaper_rate_set)
 *  This APIs support both old and new style indexing.
 */
uint32
  soc_pb_nif_pb2pa_id(
     SOC_PETRA_INTERFACE_ID  soc_pb_nif_id
  )
{
  SOC_PETRA_INTERFACE_ID
    soc_pa_id = SOC_PETRA_NIF_ID_NONE;
  SOC_PB_NIF_TYPE
    nif_type;

  if (SOC_PB_NIF_IS_PA_ID(soc_pb_nif_id))
  {
    /*
     *	Already PA-style
     */
    soc_pa_id = soc_pb_nif_id;
  }
  else
  {
    nif_type = soc_pb_nif_id2type(soc_pb_nif_id);
    if (SOC_PB_NIF_IS_PB_ID(soc_pb_nif_id))
    {
      switch(nif_type) {
      case SOC_PB_NIF_TYPE_XAUI:
        soc_pa_id = SOC_PA_MAL2NIF_NDX(soc_pb_nif_id - SOC_PB_NIF_ID_XAUI_FIRST);
    	  break;
      case SOC_PB_NIF_TYPE_SGMII:
        soc_pa_id = soc_pb_nif_id - SOC_PB_NIF_ID_SGMII_FIRST;
    	  break;
      case SOC_PB_NIF_TYPE_RXAUI:
        soc_pa_id = SOC_PA_MAL2NIF_NDX(soc_pb_nif_id - SOC_PB_NIF_ID_RXAUI_FIRST);
    	  break;
      case SOC_PB_NIF_TYPE_QSGMII:
      case SOC_PB_NIF_TYPE_ILKN:
      default:
        /*
         *	This type is not supported for Soc_petra-A, and
         *  therefore Soc_petra-A index should not be used
         */
        soc_pa_id = SOC_PETRA_IF_ID_NONE;
        break;
      }
    }
  }

  if (soc_pa_id >= SOC_PETRA_IF_NOF_NIFS)
  {
    soc_pa_id = SOC_PETRA_IF_ID_NONE;
  }

  return soc_pa_id;
}

/*
 *	Converts from a Network interface offset (0 - 63),
 *  to PB-stype Soc_petra-NIF-id, given the NIF type.
 *  For example SOC_PB_NIF_TYPE_XAUI with offset 3 is converted to
 *  SOC_PETRA_NIF_ID_XAUI_3
 */
SOC_PETRA_INTERFACE_ID
  soc_pb_nif_offset2nif_id(
    SOC_SAND_IN  SOC_PB_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint32         nif_offset
  )
{
  SOC_PETRA_INTERFACE_ID
    nif_id;

  if (nif_offset >= SOC_PB_NIF_NOF_NIFS)
  {
    nif_id = SOC_PETRA_NIF_ID_NONE;
  }
  else
  {
    switch(nif_type) {
    case SOC_PB_NIF_TYPE_XAUI:
      nif_id = SOC_PB_NIF_ID(XAUI, nif_offset);
  	  break;
    case SOC_PB_NIF_TYPE_RXAUI:
      nif_id = SOC_PB_NIF_ID(RXAUI, nif_offset);
  	  break;
    case SOC_PB_NIF_TYPE_SGMII:
      nif_id = SOC_PB_NIF_ID(SGMII, nif_offset);
  	  break;
    case SOC_PB_NIF_TYPE_QSGMII:
      nif_id = SOC_PB_NIF_ID(QSGMII, nif_offset);
  	  break;
    case SOC_PB_NIF_TYPE_ILKN:
      nif_id = SOC_PB_NIF_ID(ILKN, nif_offset);
  	  break;
    default:
      nif_id = SOC_PETRA_NIF_ID_NONE;
    }
  }

  return nif_id;
}

/*
 *	Converts from an internal indexing (0 - 63)
 *  to PB-stype Soc_petra-NIF-id, given the NIF type
 */
SOC_PETRA_INTERFACE_ID
  soc_pb_nif_intern2nif_id(
    SOC_SAND_IN  SOC_PB_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint32         internal_id
  )
{
  SOC_PETRA_INTERFACE_ID
    nif_id,
    nif_offset;

  if (internal_id >= SOC_PB_NIF_NOF_NIFS)
  {
    nif_id = SOC_PETRA_NIF_ID_NONE;
  }
  else
  {
    switch(nif_type) {
    case SOC_PB_NIF_TYPE_XAUI:
      nif_offset = internal_id / 8;
      nif_id = SOC_PB_NIF_ID(XAUI, nif_offset);
  	  break;
    case SOC_PB_NIF_TYPE_RXAUI:
      nif_offset = internal_id / 4;
      nif_id = SOC_PB_NIF_ID(RXAUI, nif_offset);
  	  break;
    case SOC_PB_NIF_TYPE_SGMII:
      nif_offset = internal_id / 2;
      nif_id = SOC_PB_NIF_ID(SGMII, nif_offset);
  	  break;
    case SOC_PB_NIF_TYPE_QSGMII:
      nif_offset = internal_id;
      nif_id = SOC_PB_NIF_ID(QSGMII, nif_offset);
  	  break;
    case SOC_PB_NIF_TYPE_ILKN:
      nif_offset = internal_id / 32;
      nif_id = SOC_PB_NIF_ID(ILKN, nif_offset);
  	  break;
    default:
      nif_id = SOC_PETRA_NIF_ID_NONE;
    }
  }

  return nif_id;
}

/*
 *	Convert from Soc_petra NIF-id to internal representation (0 - 63).
 *  Expects PB-style indexing
 */
uint32
  soc_pb_nif2intern_id(
    SOC_PETRA_INTERFACE_ID  soc_pb_nif_id
  )
{
  SOC_PETRA_INTERFACE_ID
    nif_intern_id;

  if (SOC_PB_NIF_IS_TYPE_ID(XAUI, soc_pb_nif_id))
  {
    nif_intern_id = SOC_PB_NIF_ID_OFFSET(XAUI, soc_pb_nif_id) * 8; 
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(RXAUI, soc_pb_nif_id))
  {
    nif_intern_id = SOC_PB_NIF_ID_OFFSET(RXAUI, soc_pb_nif_id) * 4; 
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(SGMII, soc_pb_nif_id))
  {
    nif_intern_id = SOC_PB_NIF_ID_OFFSET(SGMII, soc_pb_nif_id) * 2; 
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(QSGMII, soc_pb_nif_id))
  {
    nif_intern_id = SOC_PB_NIF_ID_OFFSET(QSGMII, soc_pb_nif_id);  
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(ILKN, soc_pb_nif_id))
  {
    nif_intern_id = SOC_PB_NIF_ID_OFFSET(ILKN, soc_pb_nif_id) * 32;
  }
  else
  {
    nif_intern_id = SOC_PETRA_NIF_ID_NONE;
  }
     
  return nif_intern_id;
}

/*********************************************************************
*     Turns the interface on/off. Optionally, powers up/down
 *     the attached SerDes also.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_on_off_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ON_OFF_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_on_off_set_verify(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_on_off_set_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_on_off_set()", 0, 0);
}

/*********************************************************************
*     Turns the interface on/off. Optionally, powers up/down
 *     the attached SerDes also.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_on_off_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_STATE_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ON_OFF_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_on_off_get_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_on_off_get_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_on_off_get()", 0, 0);
}

/*********************************************************************
*     Sets the specified Network Interface to loopback, by
 *     setting all its SerDeses in loopback (NSILB)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_loopback_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LOOPBACK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_nif_loopback_set_verify(
          unit,
          nif_ndx,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_loopback_set_unsafe(
          unit,
          nif_ndx,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_loopback_set()", 0, 0);
}

/*********************************************************************
*     Sets the specified Network Interface to loopback, by
 *     setting all its SerDeses in loopback (NSILB)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_loopback_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT uint8                      *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LOOPBACK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = soc_pb_nif_loopback_get_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_loopback_get_unsafe(
          unit,
          nif_ndx,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_loopback_get()", 0, 0);
}

/*********************************************************************
*     Sets basic MAL configuration. This configuration defines
 *     MAL to SerDes mapping topology. Following setting basic
 *     configuration, per-NIF-type (e.g. SPAUI/SGMII etc.)
 *     configuration must be set before enabling the MAL.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_mal_basic_conf_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_BASIC_CONF_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_mal_basic_conf_verify(
          unit,
          mal_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_mal_basic_conf_set_unsafe(
          unit,
          mal_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_basic_conf_set()", mal_ndx, 0);
}

/*********************************************************************
*     Sets basic MAL configuration. This configuration defines
 *     MAL to SerDes mapping topology. Following setting basic
 *     configuration, per-NIF-type (e.g. SPAUI/SGMII etc.)
 *     configuration must be set before enabling the MAL.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_mal_basic_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_PB_NIF_MAL_BASIC_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_BASIC_CONF_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_mal_id_verify(
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_mal_basic_conf_get_unsafe(
          unit,
          mal_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_basic_conf_get()", mal_ndx, 0);
}

/*********************************************************************
*     Sets minimal packet size as limited by the NIF. Note:
 *     Normally, the packet size is limited using
 *     soc_petra_mgmt_pckt_size_range_set(), and not this API.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_min_packet_size_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  uint32                       pckt_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MIN_PACKET_SIZE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_nif_min_packet_size_verify(
          unit,
          mal_ndx,
          direction_ndx,
          pckt_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_min_packet_size_set_unsafe(
          unit,
          mal_ndx,
          direction_ndx,
          pckt_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_min_packet_size_set()", mal_ndx, 0);
}

/*********************************************************************
*     Sets minimal packet size as limited by the NIF. Note:
 *     Normally, the packet size is limited using
 *     soc_petra_mgmt_pckt_size_range_set(), and not this API.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_min_packet_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT uint32                       *pckt_size_rx,
    SOC_SAND_OUT uint32                       *pckt_size_tx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MIN_PACKET_SIZE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pckt_size_rx);
  SOC_SAND_CHECK_NULL_INPUT(pckt_size_tx);

  res = soc_pb_nif_min_packet_size_ndx_verify(
          unit,
          mal_ndx,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_min_packet_size_get_unsafe(
          unit,
          mal_ndx,
          pckt_size_rx,
          pckt_size_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_min_packet_size_get()", mal_ndx, 0);
}

/*********************************************************************
*     Sets SPAUI configuration - configure SPAUI extensions
 *     for XAUI interface.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_spaui_conf_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SPAUI_CONF_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_spaui_conf_verify(
          unit,
          nif_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_spaui_conf_set_unsafe(
          unit,
          nif_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_spaui_conf_set()", 0, 0);
}

/*********************************************************************
*     Sets SPAUI configuration - configure SPAUI extensions
 *     for XAUI interface.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_spaui_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO              *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO              *info_tx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SPAUI_CONF_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  res = soc_pb_nif_spaui_conf_ndx_verify(
          unit,
          nif_ndx,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_spaui_conf_get_unsafe(
          unit,
          nif_ndx,
          info_rx,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_spaui_conf_get()", 0, 0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_gmii_conf_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_CONF_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_gmii_conf_verify(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_gmii_conf_set_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_conf_set()", 0, 0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_gmii_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_INFO               *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_CONF_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_gmii_conf_get_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_conf_get()", 0, 0);
}

/*********************************************************************
*     Gets SGMII/1000BASE-X interface diagnostics status
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_gmii_status_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_STAT              *status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  res = soc_pb_nif_gmii_status_ndx_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_gmii_status_get_unsafe(
          unit,
          nif_ndx,
          status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_status_get()", 0, 0);
}

/*********************************************************************
*     Sets SGMII interface link-rate.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_gmii_rate_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE              rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_RATE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_nif_gmii_rate_verify(
          unit,
          nif_ndx,
          rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_gmii_rate_set_unsafe(
          unit,
          nif_ndx,
          rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_rate_set()", 0, 0);
}

/*********************************************************************
*     Sets SGMII interface link-rate.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_gmii_rate_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_RATE              *rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_RATE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rate);

  res = soc_pb_nif_gmii_rate_ndx_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_gmii_rate_get_unsafe(
          unit,
          nif_ndx,
          rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_rate_get()", 0, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ilkn_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_ilkn_verify(
          unit,
          ilkn_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_ilkn_set_unsafe(
          unit,
          ilkn_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_set()", 0, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ilkn_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO               *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO               *info_tx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  res = soc_pb_nif_ilkn_ndx_verify(
          unit,
          ilkn_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_ilkn_get_unsafe(
          unit,
          ilkn_ndx,
          info_rx,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_get()", 0, 0);
}

/*********************************************************************
*     Reads the Interlaken status from the ILKN interrupt
 *     register and from other status registers. Clears the
 *     interrupts only if requested.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ilkn_diag(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  uint8                      clear_interrupts,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_DIAG_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_DIAG);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_ilkn_diag_verify(
          unit,
          ilkn_ndx,
          clear_interrupts
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_ilkn_diag_unsafe(
          unit,
          ilkn_ndx,
          clear_interrupts,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_diag()", 0, 0);
}

/*********************************************************************
*     Indicates whether the interface is configured and
 *     powered up
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_stat_activity_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS         *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_STAT_ACTIVITY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_stat_activity_ndx_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_stat_activity_get_unsafe(
          unit,
          nif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_stat_activity_get()", 0, 0);
}

/*********************************************************************
*     Indicates whether the interface is configured and
 *     powered up. All interfaces are read at once.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_stat_activity_all_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS_ALL     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_STAT_ACTIVITY_ALL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_stat_activity_all_ndx_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_stat_activity_all_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_stat_activity_all_get()", 0, 0);
}

/*********************************************************************
*     Gets link status, and whether there was change in the
 *     status.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_link_status_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_LINK_STATUS             *link_status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LINK_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(link_status);

  res = soc_pb_nif_link_status_ndx_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_link_status_get_unsafe(
          unit,
          nif_ndx,
          link_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_link_status_get()", 0, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_synce_clk_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK               *clk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_CLK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(clk);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_synce_clk_verify(
          unit,
          clk_ndx,
          clk
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_nif_synce_clk_set_unsafe(
          unit,
          clk_ndx,
          clk
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_clk_set()", 0, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_synce_clk_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_CLK               *clk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_CLK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(clk);

  res = soc_pb_nif_synce_clk_ndx_verify(
          unit,
          clk_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_synce_clk_get_unsafe(
          unit,
          clk_ndx,
          clk
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_clk_get()", 0, 0);
}

/*********************************************************************
*     Define the Recovered Clock signal to be transmitted on
 *     the Soc_petra external pins
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_synce_mode_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE              mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_MODE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_nif_synce_mode_verify(
          unit,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_synce_mode_set_unsafe(
          unit,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_mode_set()", 0, 0);
}

/*********************************************************************
*     Define the Recovered Clock signal to be transmitted on
 *     the Soc_petra external pins
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_synce_mode_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_MODE              *mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_MODE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mode);

  res = soc_pb_nif_synce_mode_ndx_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_synce_mode_get_unsafe(
          unit,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_mode_get()", 0, 0);
}

/*********************************************************************
*     Reads the PTP clock current reset time value
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_reset_value_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_PTP_CLK_RST_VAL         *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_RESET_VALUE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_ptp_clk_reset_value_ndx_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_ptp_clk_reset_value_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_reset_value_get()", 0, 0);
}

/*********************************************************************
*     Set the PTP Clock configuration for the Precision Time
 *     Protocol
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_ptp_clk_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_ptp_clk_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_set()", 0, 0);
}

/*********************************************************************
*     Set the Clock Frequency for the Precision Time Protocol
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_ptp_clk_ndx_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_ptp_clk_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_get()", 0, 0);
}

/*********************************************************************
*     Enable/disable and configure parameters for the
 *     Precision Time Protocol
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ptp_conf_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CONF_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_ptp_conf_verify(
          unit,
          nif_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_ptp_conf_set_unsafe(
          unit,
          nif_ndx,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_conf_set()", 0, 0);
}

/*********************************************************************
*     Enable/disable and configure parameters for the
 *     Precision Time Protocol
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ptp_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO                *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO                *info_tx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CONF_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_ptp_conf_get_unsafe(
          unit,
          nif_ndx,
          info_rx,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_conf_get()", 0, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_elk_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ELK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_elk_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_elk_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_elk_set()", 0, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_elk_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_ELK_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ELK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_elk_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_elk_get()", 0, 0);
}

/*********************************************************************
*     Sets Fat-pipe configuration mode
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_fatp_mode_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_MODE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_fatp_mode_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_fatp_mode_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_mode_set()", 0, 0);
}

/*********************************************************************
*     Sets Fat-pipe configuration mode
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_fatp_mode_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PB_NIF_FATP_MODE_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_MODE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_fatp_mode_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_mode_get()", 0, 0);
}

/*********************************************************************
*     Sets Fat-pipe configuration mode
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_fatp_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx,
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO               *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_fatp_verify(
          unit,
          fatp_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_nif_fatp_set_unsafe(
          unit,
          fatp_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_set()", 0, 0);
}

/*********************************************************************
*     Sets Fat-pipe configuration mode
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_fatp_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx,
    SOC_SAND_OUT SOC_PB_NIF_FATP_INFO               *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_nif_fatp_ndx_verify(
          unit,
          fatp_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_fatp_get_unsafe(
          unit,
          fatp_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_get()", 0, 0);
}

/*********************************************************************
*     Gets Value of statistics counter of the NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_counter_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE            counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                     *counter_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_COUNTER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counter_val);

  res = soc_pb_nif_counter_ndx_verify(
          unit,
          nif_ndx,
          counter_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_counter_get_unsafe(
          unit,
          nif_ndx,
          counter_type,
          counter_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_counter_get()", 0, 0);
}

/*********************************************************************
*     Gets Value of statistics counter of the NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_all_counters_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counter_val[SOC_PB_NIF_NOF_COUNTERS]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ALL_COUNTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_nif_all_counters_ndx_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_all_counters_get_unsafe(
          unit,
          nif_ndx,
          non_data_also,
          counter_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_all_counters_get()", 0, 0);
}

/*********************************************************************
*     Gets Statistics Counters for all the NIF-s in the
 *     device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_all_nifs_all_counters_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counters_val[SOC_PB_NIF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ALL_NIFS_ALL_COUNTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_nif_all_nifs_all_counters_get_unsafe(
          unit,
          non_data_also,
          counters_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_all_nifs_all_counters_get()", 0, 0);
}


  /************************************************************************/
  /* Clear Structures                                                     */
  /************************************************************************/

void
  SOC_PB_NIF_STATE_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_STATE_INFO));
  info->is_nif_on = 0;
  info->serdes_also = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_LANES_SWAP_clear(
    SOC_SAND_OUT SOC_PB_NIF_LANES_SWAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_LANES_SWAP));
  info->swap_rx = 0;
  info->swap_tx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_MAL_TOPOLOGY_clear(
    SOC_SAND_OUT SOC_PB_NIF_MAL_TOPOLOGY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_MAL_TOPOLOGY));
  SOC_PB_NIF_LANES_SWAP_clear(&(info->lanes_swap));
  info->is_qsgmii_alt = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_MAL_BASIC_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_MAL_BASIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_MAL_BASIC_INFO));
  info->type = SOC_PB_NIF_NOF_TYPES;
  SOC_PB_NIF_MAL_TOPOLOGY_clear(&(info->topology));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_PREAMBLE_COMPRESS_clear(
    SOC_SAND_OUT SOC_PB_NIF_PREAMBLE_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_PREAMBLE_COMPRESS));
  info->enable = 0;
  info->size = SOC_PB_NIF_PRMBL_SIZE_COLUMNS_2;
  info->skip_SOP = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_IPG_COMPRESS_clear(
    SOC_SAND_OUT SOC_PB_NIF_IPG_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_IPG_COMPRESS));
  info->enable = 0;
  info->size = 0xc;
  info->dic_mode = SOC_PB_NIF_IPG_DIC_MODE_AVERAGE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_CHANNELIZED_clear(
    SOC_SAND_OUT SOC_PB_NIF_CHANNELIZED *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_CHANNELIZED));
  info->enable = 0;
  info->bct_size = 0;
  info->bct_channel_byte_ndx = 0x1;
  info->is_burst_interleaving = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_FAULT_RESPONSE_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_FAULT_RESPONSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_FAULT_RESPONSE_INFO));
  info->local = SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_RF;
  info->remote = SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_IDLE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_SPAUI_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_SPAUI_INFO));
  info->enable = 0;
  info->link_partner_double_size_bus = FALSE;
  info->is_double_size_sop_odd_only = FALSE;
  info->is_double_size_sop_even_only = FALSE;
  SOC_PB_NIF_PREAMBLE_COMPRESS_clear(&(info->preamble));
  SOC_PB_NIF_IPG_COMPRESS_clear(&(info->ipg));
  info->crc_mode = SOC_PB_NIF_CRC_MODE_32;
  SOC_PB_NIF_CHANNELIZED_clear(&(info->ch_conf));
  SOC_PB_NIF_FAULT_RESPONSE_INFO_clear(&(info->fault_response));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_GMII_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_GMII_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_GMII_INFO));
  info->enable_rx = 0;
  info->enable_tx = 0;
  info->mode = SOC_PB_NIF_NOF_1GE_MODES;
  info->rate = SOC_PB_NIF_NOF_GMII_RATES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_GMII_STAT_clear(
    SOC_SAND_OUT SOC_PB_NIF_GMII_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_GMII_STAT));
  info->autoneg_not_complete = 0;
  info->autoneg_error = 0;
  info->remote_fault = 0;
  info->fault_type = SOC_PB_NIF_NOF_GMII_RFS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ILKN_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_ILKN_INFO));
  info->nof_lanes = 0;
  info->is_invalid_lane = 0;
  info->invalid_lane_id = 0;
  info->is_burst_interleaving = FALSE;
  info->metaframe_sync_period = 2048;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ILKN_INT_clear(
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_ILKN_INT));
  info->ilkn_rx_port_status_change_int = 0;
  info->ilkn_link_partner_status_change_lanes_int = 0;
  info->ilkn_rx_port_align_err_int = 0;
  info->ilkn_crc32_lane_err_indication_lanes_int = 0;
  info->ilkn_lane_synch_err_lanes_int = 0;
  info->ilkn_lane_framing_err_lanes_int = 0;
  info->ilkn_lane_bad_framing_type_err_lanes_int = 0;
  info->ilkn_lane_meta_frame_sync_word_err_lanes_int = 0;
  info->ilkn_lane_scrambler_state_err_lanes_int = 0;
  info->ilkn_lane_meta_frame_length_err_lanes_int = 0;
  info->ilkn_lane_meta_frame_repeat_err_lanes_int = 0;
  info->ilkn_rx_port_status_err_int = 0;
  info->ilkn_tx_port_status_err_int = 0;
  info->ilkn_rx_parity_err_int = 0;
  info->ilkn_tx_parity_err_int = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ILKN_STAT_clear(
    SOC_SAND_OUT SOC_PB_NIF_ILKN_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_ILKN_STAT));
  info->rx_stat_aligned_raw = 0;
  info->rx_port_active = 0;
  info->rx_stat_misaligned = 0;
  info->rx_stat_aligned_err = 0;
  info->rx_stat_crc24_err = 0;
  info->rx_stat_miss_sop_err = 0;
  info->rx_stat_miss_eop_err = 0;
  info->rx_stat_overflow_err = 0;
  info->rx_stat_burstmax_err = 0;
  info->rx_stat_burst_err = 0;
  info->rx_lp_ifc_status = 0;
  info->tx_ovfout = 0;
  info->tx_port_active = 0;
  info->tx_stat_underflow_err = 0;
  info->tx_stat_burst_err = 0;
  info->tx_stat_overflow_err = 0;
  info->link_partner_lanes_status_bitmap = 0;
  info->link_partner_interface_status_bitmap = 0;
  info->crc32_lane_valid_indication_bitmap = 0;
  info->crc32_lane_err_indication_bitmap = 0;
  info->lane_synchronization_achieved_bitmap = 0;
  info->lane_synchronization_err_bitmap = 0;
  info->lane_framing_err_bitmap = 0;
  info->lane_bad_framing_layer_err_bitmap = 0;
  info->lane_meta_frame_sync_word_err_bitmap = 0;
  info->lane_scrambler_state_err_bitmap = 0;
  info->lane_meta_frame_length_err_bitmap = 0;
  info->lane_meta_frame_repeat_err_bitmap = 0;
  info->ilkn_rx_stat_lanes_parity_err_bitmap = 0;
  info->ilkn_rx_stat_buff_parity_err = 0;
  info->ilkn_tx_stat_lanes_parity_err = 0;
  info->ilkn_tx_stat_buff_parity_err = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ILKN_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_ILKN_DIAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_ILKN_DIAG_INFO));
  SOC_PB_NIF_ILKN_INT_clear(&(info->interrupt));
  SOC_PB_NIF_ILKN_STAT_clear(&(info->stat));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_STAT_LINK_ACTIVITY_clear(
    SOC_SAND_OUT SOC_PB_NIF_STAT_LINK_ACTIVITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_STAT_LINK_ACTIVITY));
  info->is_up = 0;
  info->is_active_rx = 0;
  info->is_active_tx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ACTIVITY_STATUS_clear(
    SOC_SAND_OUT SOC_PB_NIF_ACTIVITY_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_ACTIVITY_STATUS));
  info->enabled = 0;
  SOC_PB_NIF_STAT_LINK_ACTIVITY_clear(&(info->status));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ACTIVITY_STATUS_ALL_clear(
    SOC_SAND_OUT SOC_PB_NIF_ACTIVITY_STATUS_ALL *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_ACTIVITY_STATUS_ALL));
  for (ind = 0; ind < SOC_PB_NIF_NOF_NIFS; ++ind)
  {
    SOC_PB_NIF_STAT_LINK_ACTIVITY_clear(&(info->activity[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_LINK_STATUS_clear(
    SOC_SAND_OUT SOC_PB_NIF_LINK_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_LINK_STATUS));
  info->link_status = 0;
  info->link_status_change = 0;
  info->pcs_sync = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_SYNCE_CLK_clear(
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_CLK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_SYNCE_CLK));
  info->enable = 0;
  info->nif_id = SOC_PETRA_IF_ID_NONE;
  info->clk_divider = SOC_PB_NIF_NOF_SYNCE_CLK_DIVS;
  info->squelch_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_PTP_CLK_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_PTP_CLK_INFO));
  info->ptp_clk_delta = 0;
  soc_sand_u64_clear(&(info->sync_reset_val));
  soc_sand_u64_clear(&(info->sync_autoinc_interval));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_PTP_CLK_RST_VAL_clear(
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_RST_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_PTP_CLK_RST_VAL));
  soc_sand_u64_clear(&info->curr_time);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_PTP_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_PTP_INFO));
  info->enable = 0;
  info->wire_delay_ns = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ELK_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_ELK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_ELK_INFO));
  info->enable = 0;
  info->mal_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_FATP_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_FATP_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_FATP_MODE_INFO));
  info->mode = SOC_PB_NIF_FATP_MODE_DISABLED;
  info->is_bypass_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_FATP_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_FATP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_NIF_FATP_INFO));
  info->enable = 0;
  info->base_port_id = 0;
  info->nof_ports = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

  /*
   *	Returns TRUE if the given internal ID
   *  represents an actual NIF, according to the configured NIF type
   */
STATIC uint32
  soc_pb_nif_is_active_intern_id(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 nif_id,
    SOC_SAND_OUT uint8 *is_active_id,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_ID   *actual_nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_TYPE          *actual_nif_type
  )
{
  uint32
    res;
  SOC_PB_NIF_TYPE
    nif_type = SOC_PB_NIF_TYPE_NONE;
  SOC_PETRA_INTERFACE_ID
    nif_ndx = SOC_PB_NIF_TYPE_NONE;
  uint32
    mal_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  res = soc_pb_nif_mal_type_get(
               unit,
               mal_id,
               &nif_type
             );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  nif_ndx = soc_pb_nif_intern2nif_id(nif_type, nif_id);

  if ((soc_pb_nif2intern_id(nif_ndx)== nif_id) && (nif_type != SOC_PB_NIF_TYPE_NONE))
  {
    *is_active_id = TRUE;
  }
  *actual_nif_ndx = nif_ndx;
  *actual_nif_type = nif_type;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_is_active_intern_id()", 0, 0);
}

STATIC void
  soc_pb_nif_port_mapping_get(
    SOC_SAND_IN    int unit,
    SOC_SAND_INOUT uint32  nif_to_ports[SOC_PB_NIF_NOF_NIFS],
    SOC_SAND_INOUT uint8 is_multiple_ports[SOC_PB_NIF_NOF_NIFS]
  )
{
  uint32
    mapped_nif_id,
    port_indx;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    in_map_info,
    out_map_info;

  sal_memset(
    nif_to_ports,
    0xff,
    SOC_PB_NIF_NOF_NIFS * sizeof(uint32)
    );

  sal_memset(
    is_multiple_ports,
    0x0,
    SOC_PB_NIF_NOF_NIFS * sizeof(uint8)
    );

  for (port_indx = 0 ; port_indx < SOC_PETRA_NOF_LOCAL_PORTS; ++port_indx )
  {
    soc_petra_port_to_interface_map_get(
      unit,
      SOC_CORE_DEFAULT,
      port_indx,
      &in_map_info,
      &out_map_info
      );

    if (SOC_PB_NIF_IS_PB_ID(in_map_info.if_id))
    {
      mapped_nif_id = soc_pb_nif2intern_id(in_map_info.if_id);
      if(nif_to_ports[mapped_nif_id] == SOC_SAND_INTERN_VAL_INVALID_32)
      {
        /* First mapped port */
        nif_to_ports[mapped_nif_id] = port_indx;
      }
      else
      {
        /* Print only the first mapped port */
        is_multiple_ports[mapped_nif_id] = TRUE;
      }
    }
    else
    {
      if (SOC_PB_NIF_IS_PB_ID(out_map_info.if_id))
      {
        mapped_nif_id = soc_pb_nif2intern_id(out_map_info.if_id);
        if(nif_to_ports[mapped_nif_id] == SOC_SAND_INTERN_VAL_INVALID_32)
        {
          /* First mapped port */
          nif_to_ports[mapped_nif_id] = port_indx;
        }
        else
        {
          /* Print only the first mapped port */
          is_multiple_ports[mapped_nif_id] = TRUE;
        }
      }
    }
  }
}


/************************************************************************/
/* Print and Utilities                                                  */
/************************************************************************/

const char*
  SOC_PB_NIF_MALG_ID_to_string(
    SOC_SAND_IN  SOC_PB_NIF_MALG_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_MALG_ID_A:
    str = "a";
  break;
  case SOC_PB_NIF_MALG_ID_B:
    str = "b";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_NIF_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_TYPE_NONE:
    str = "none";
  break;
  case SOC_PB_NIF_TYPE_XAUI:
    str = "xaui";
  break;
  case SOC_PB_NIF_TYPE_SGMII:
    str = "sgmii";
  break;
  case SOC_PB_NIF_TYPE_RXAUI:
    str = "rxaui";
  break;
  case SOC_PB_NIF_TYPE_QSGMII:
    str = "qsgmii";
  break;
  case SOC_PB_NIF_TYPE_ILKN:
    str = "ilkn";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_PREAMBLE_SIZE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_PREAMBLE_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_PRMBL_SIZE_COLUMNS_0:
    str = "prmbl_size_columns_0";
  break;
  case SOC_PB_NIF_PRMBL_SIZE_COLUMNS_1:
    str = "prmbl_size_columns_1";
  break;
  case SOC_PB_NIF_PRMBL_SIZE_COLUMNS_2:
    str = "prmbl_size_columns_2";
  break;
  case SOC_PB_NIF_NOF_PRMBL_SIZES:
    str = "nof_prmbl_sizes";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_IPG_DIC_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_IPG_DIC_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_IPG_DIC_MODE_AVERAGE:
    str = "average";
  break;
  case SOC_PB_NIF_IPG_DIC_MODE_MINIMUM:
    str = "minimum";
  break;
  case SOC_PB_NIF_NOF_IPG_DIC_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_CRC_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_CRC_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_CRC_MODE_32:
    str = "mode_32";
  break;
  case SOC_PB_NIF_CRC_MODE_24:
    str = "mode_24";
  break;
  case SOC_PB_NIF_CRC_MODE_NONE:
    str = "none";
  break;
  case SOC_PB_NIF_NOF_CRC_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_LINK_FAULT_LOCATION_to_string(
    SOC_SAND_IN  SOC_PB_NIF_LINK_FAULT_LOCATION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_LINK_FAULT_LOCATION_LOCAL:
    str = "local";
  break;
  case SOC_PB_NIF_LINK_FAULT_LOCATION_REMOTE:
    str = "remote";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_FAULT_RESPONSE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_FAULT_RESPONSE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_IDLE:
    str = "data_and_idle";
  break;
  case SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_RF:
    str = "data_and_rf";
  break;
  case SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_LF:
    str = "data_and_lf";
  break;
  case SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_IDLE:
    str = "no_data_idle";
  break;
  case SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_RF:
    str = "no_data_rf";
  break;
  case SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_LF:
    str = "no_data_lf";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_1GE_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_1GE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_1GE_MODE_1000BASE_X:
    str = "mode_1000base_x";
  break;
  case SOC_PB_NIF_1GE_MODE_SGMII:
    str = "sgmii";
  break;
  case SOC_PB_NIF_NOF_1GE_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_GMII_RATE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_GMII_RATE_AUTONEG:
    str = "autoneg";
  break;
  case SOC_PB_NIF_GMII_RATE_10Mbps:
    str = "rate_10mbps";
  break;
  case SOC_PB_NIF_GMII_RATE_100Mbps:
    str = "rate_100mbps";
  break;
  case SOC_PB_NIF_GMII_RATE_1000Mbps:
    str = "rate_1000mbps";
  break;
  case SOC_PB_NIF_NOF_GMII_RATES:
    str = "nof_sgmii_rates";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_GMII_RF_to_string(
    SOC_SAND_IN  SOC_PB_NIF_GMII_RF enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_GMII_RF_OK:
    str = "ok";
  break;
  case SOC_PB_NIF_GMII_RF_OFFLINE_REQ:
    str = "offline_req";
  break;
  case SOC_PB_NIF_GMII_RF_LNK_FAIL:
    str = "lnk_fail";
  break;
  case SOC_PB_NIF_GMII_RF_AUTONEG_ERR:
    str = "autoneg_err";
  break;
  case SOC_PB_NIF_NOF_GMII_RFS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_ILKN_ID_to_string(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_ILKN_ID_A:
    str = "a";
  break;
  case SOC_PB_NIF_ILKN_ID_B:
    str = "b";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_SYNCE_CLK_DIV_to_string(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_DIV enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_SYNCE_CLK_DIV_20:
    str = "div_20";
  break;
  case SOC_PB_NIF_SYNCE_CLK_DIV_40:
    str = "div_40";
  break;
  case SOC_PB_NIF_SYNCE_CLK_DIV_80:
    str = "div_80";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_SYNCE_CLK_ID_to_string(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_SYNCE_CLK_ID_0:
    str = "id_0";
  break;
  case SOC_PB_NIF_SYNCE_CLK_ID_1_OR_VALID:
    str = "id_1_or_valid";
  break;
  case SOC_PB_NIF_SYNCE_CLK_ID_2:
    str = "id_2";
  break;
  case SOC_PB_NIF_SYNCE_CLK_ID_3_OR_VALID:
    str = "id_3_or_valid";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_SYNCE_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_SYNCE_MODE_TWO_DIFF_CLK:
    str = "two_diff_clk";
  break;
  case SOC_PB_NIF_SYNCE_MODE_FOUR_CLK:
    str = "four_clk";
  break;
  case SOC_PB_NIF_SYNCE_MODE_TWO_CLK_AND_VALID:
    str = "two_clk_and_valid";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_FATP_ID_to_string(
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_FATP_ID_A:
    str = "a";
  break;
  case SOC_PB_NIF_FATP_ID_B:
    str = "b";
  break;
  case SOC_PB_NIF_FATP_ID_C:
    str = "c";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_FATP_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_FATP_MODE_DISABLED:
    str = "disabled";
  break;
  case SOC_PB_NIF_FATP_MODE_3_X_4:
    str = "mode_3_x_4";
  break;
  case SOC_PB_NIF_FATP_MODE_2_X_6:
    str = "mode_2_x_6";
  break;
  case SOC_PB_NIF_FATP_MODE_1_X_12:
    str = "mode_1_x_12";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_NIF_COUNTER_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_NIF_RX_OK_OCTETS:
    str = "rx_ok_octets";
  break;
  case SOC_PB_NIF_RX_BCAST_PACKETS:
    str = "rx_bcast_packets";
  break;
  case SOC_PB_NIF_RX_MCAST_BURSTS:
    str = "rx_mcast_bursts";
  break;
  case SOC_PB_NIF_RX_OK_PACKETS:
    str = "rx_ok_packets";
  break;
  case SOC_PB_NIF_RX_ERR_PACKETS:
    str = "rx_err_packets";
  break;
  case SOC_PB_NIF_RX_LEN_BELOW_MIN:
    str = "rx_len_below_min";
  break;
  case SOC_PB_NIF_RX_LEN_MIN_59:
    str = "rx_len_min_59";
  break;
  case SOC_PB_NIF_RX_LEN_60:
    str = "rx_len_60";
  break;
  case SOC_PB_NIF_RX_LEN_61_123:
    str = "rx_len_61_123";
  break;
  case SOC_PB_NIF_RX_LEN_124_251:
    str = "rx_len_124_251";
  break;
  case SOC_PB_NIF_RX_LEN_252_507:
    str = "rx_len_252_507";
  break;
  case SOC_PB_NIF_RX_LEN_508_1019:
    str = "rx_len_508_1019";
  break;
  case SOC_PB_NIF_RX_LEN_1020_1514CFG:
    str = "rx_len_1020_1514/1518";
  break;
  case SOC_PB_NIF_RX_LEN_1515CFG_MAX:
    str = "rx_len_1515/1519_max";
  break;
  case SOC_PB_NIF_RX_LEN_ABOVE_MAX:
    str = "rx_len_above_max";
  break;
  case SOC_PB_NIF_RX_OK_PAUSE_FRAMES:
    str = "rx_ok_pause_frames";
  break;
  case SOC_PB_NIF_RX_ERR_PAUSE_FRAMES:
    str = "rx_err_pause_frames";
  break;
  case SOC_PB_NIF_RX_PTP_FRAMES:
    str = "rx_ptp_frames";
  break;
  case SOC_PB_NIF_RX_FRAME_ERR_PACKETS:
    str = "frame_err_packets";
  break;
  case SOC_PB_NIF_RX_BCT_ERR_PACKETS:
    str = "bct_err_packets";
  break;
  case SOC_PB_NIF_TX_OK_OCTETS:
    str = "tx_ok_octets";
  break;
  case SOC_PB_NIF_TX_BCAST_PACKETS:
    str = "tx_bcast_packets";
  break;
  case SOC_PB_NIF_TX_MCAST_BURSTS:
    str = "tx_mcast_bursts";
  break;
  case SOC_PB_NIF_TX_OK_PACKETS:
    str = "tx_ok_packets";
  break;
  case SOC_PB_NIF_TX_ERR_PACKETS:
    str = "tx_err_packets";
  break;
  case SOC_PB_NIF_TX_PAUSE_FRAMES:
    str = "tx_pause_frames";
  break;
  case SOC_PB_NIF_TX_PTP_FRAMES:
    str = "tx_ptp_frames";
  break;
  case SOC_PB_NIF_TX_NO_LINK_PACKETS:
    str = "tx_no_link_packets";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PB_NIF_STATE_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("is_nif_on: %u\n\r",info->is_nif_on);
  soc_sand_os_printf("serdes_also: %u\n\r",info->serdes_also);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_LANES_SWAP_print(
    SOC_SAND_IN  SOC_PB_NIF_LANES_SWAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("swap_rx: %u, ",info->swap_rx);
  soc_sand_os_printf("swap_tx: %u, ",info->swap_tx);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_MAL_TOPOLOGY_print(
    SOC_SAND_IN  SOC_PB_NIF_MAL_TOPOLOGY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" lanes_swap:");
  SOC_PB_NIF_LANES_SWAP_print(&(info->lanes_swap));
  soc_sand_os_printf(" is_qsgmii_alt: %u\n\r",info->is_qsgmii_alt);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_MAL_BASIC_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("type: %s \n\r", SOC_PB_NIF_TYPE_to_string(info->type));
  soc_sand_os_printf("topology:");
  SOC_PB_NIF_MAL_TOPOLOGY_print(&(info->topology));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_PREAMBLE_COMPRESS_print(
    SOC_SAND_IN  SOC_PB_NIF_PREAMBLE_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("size %s ", SOC_PB_NIF_PREAMBLE_SIZE_to_string(info->size));
  soc_sand_os_printf("skip_sop: %u\n\r",info->skip_SOP);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_IPG_COMPRESS_print(
    SOC_SAND_IN  SOC_PB_NIF_IPG_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("size: %u\n\r",info->size);
  soc_sand_os_printf("dic_mode %s ", SOC_PB_NIF_IPG_DIC_MODE_to_string(info->dic_mode));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_CHANNELIZED_print(
    SOC_SAND_IN  SOC_PB_NIF_CHANNELIZED *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("bct_size: %u\n\r",info->bct_size);
  soc_sand_os_printf("bct_channel_byte_ndx: %u\n\r",info->bct_channel_byte_ndx);
  soc_sand_os_printf("is_burst_interleaving: %u\n\r",info->is_burst_interleaving);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_FAULT_RESPONSE_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_FAULT_RESPONSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("local %s ", SOC_PB_NIF_FAULT_RESPONSE_to_string(info->local));
  soc_sand_os_printf("remote %s ", SOC_PB_NIF_FAULT_RESPONSE_to_string(info->remote));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_SPAUI_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("link_partner_double_size_bus: %u\n\r",info->link_partner_double_size_bus);
  soc_sand_os_printf("is_double_size_sop_odd_only: %u\n\r",info->is_double_size_sop_odd_only);
  soc_sand_os_printf("is_double_size_sop_even_only: %u\n\r",info->is_double_size_sop_even_only);
  soc_sand_os_printf("preamble: ");
  SOC_PB_NIF_PREAMBLE_COMPRESS_print(&(info->preamble));
  soc_sand_os_printf("ipg:");
  SOC_PB_NIF_IPG_COMPRESS_print(&(info->ipg));
  soc_sand_os_printf("crc_mode: %s \n\r", SOC_PB_NIF_CRC_MODE_to_string(info->crc_mode));
  soc_sand_os_printf("ch_conf: ");
  SOC_PB_NIF_CHANNELIZED_print(&(info->ch_conf));
  soc_sand_os_printf("fault_response: ");
  SOC_PB_NIF_FAULT_RESPONSE_INFO_print(&(info->fault_response));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_GMII_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable_rx: %u\n\r",info->enable_rx);
  soc_sand_os_printf("enable_tx: %u\n\r",info->enable_tx);
  soc_sand_os_printf("mode %s ", SOC_PB_NIF_1GE_MODE_to_string(info->mode));
  soc_sand_os_printf("rate %s ", SOC_PB_NIF_GMII_RATE_to_string(info->rate));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_GMII_STAT_print(
    SOC_SAND_IN  SOC_PB_NIF_GMII_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("autoneg_not_complete: %u\n\r",info->autoneg_not_complete);
  soc_sand_os_printf("autoneg_error:        %u\n\r",info->autoneg_error);
  soc_sand_os_printf("remote_fault:         %u\n\r",info->remote_fault);
  soc_sand_os_printf("fault_type:           %s\n\r", SOC_PB_NIF_GMII_RF_to_string(info->fault_type));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ILKN_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("nof_lanes: %u\n\r",info->nof_lanes);
  soc_sand_os_printf("is_invalid_lane: %u\n\r",info->is_invalid_lane);
  soc_sand_os_printf("invalid_lane_id: %u\n\r",info->invalid_lane_id);
  soc_sand_os_printf("is_burst_interleaving: %u\n\r",info->is_burst_interleaving);
  soc_sand_os_printf("metaframe_sync_period: %u[words (66-bit blocks)]\n\r",info->metaframe_sync_period);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ILKN_INT_print(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INT *info
  )
{
  SOC_PB_NIF_ILKN_INT
    no_int;
  uint8
    is_equal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_NIF_ILKN_INT_clear(&no_int);
  
  SOC_PETRA_COMP(info, &no_int, SOC_PB_NIF_ILKN_INT, 1, is_equal);
  if (is_equal)
  {
    soc_sand_os_printf(
      "None\n\r"
    );
  }
  else
  {
    if (info->ilkn_rx_port_status_change_int != 0)                {soc_sand_os_printf("ilkn_rx_port_status_change_int:               %u\n\r",info->ilkn_rx_port_status_change_int);}
    if (info->ilkn_link_partner_status_change_lanes_int != 0)     {soc_sand_os_printf("ilkn_link_partner_status_change_lanes_int:    %u\n\r",info->ilkn_link_partner_status_change_lanes_int);}
    if (info->ilkn_rx_port_align_err_int != 0)                    {soc_sand_os_printf("ilkn_rx_port_align_err_int:                   %u\n\r",info->ilkn_rx_port_align_err_int);}
    if (info->ilkn_crc32_lane_err_indication_lanes_int != 0)      {soc_sand_os_printf("ilkn_crc32_lane_err_indication_lanes_int:     %u\n\r",info->ilkn_crc32_lane_err_indication_lanes_int);}
    if (info->ilkn_lane_synch_err_lanes_int != 0)                 {soc_sand_os_printf("ilkn_lane_synch_err_lanes_int:                %u\n\r",info->ilkn_lane_synch_err_lanes_int);}
    if (info->ilkn_lane_framing_err_lanes_int != 0)               {soc_sand_os_printf("ilkn_lane_framing_err_lanes_int:              %u\n\r",info->ilkn_lane_framing_err_lanes_int);}
    if (info->ilkn_lane_bad_framing_type_err_lanes_int != 0)      {soc_sand_os_printf("ilkn_lane_bad_framing_type_err_lanes_int:     %u\n\r",info->ilkn_lane_bad_framing_type_err_lanes_int);}
    if (info->ilkn_lane_meta_frame_sync_word_err_lanes_int != 0)  {soc_sand_os_printf("ilkn_lane_meta_frame_sync_word_err_lanes_int: %u\n\r",info->ilkn_lane_meta_frame_sync_word_err_lanes_int);}
    if (info->ilkn_lane_scrambler_state_err_lanes_int != 0)       {soc_sand_os_printf("ilkn_lane_scrambler_state_err_lanes_int:      %u\n\r",info->ilkn_lane_scrambler_state_err_lanes_int);}
    if (info->ilkn_lane_meta_frame_length_err_lanes_int != 0)     {soc_sand_os_printf("ilkn_lane_meta_frame_length_err_lanes_int:    %u\n\r",info->ilkn_lane_meta_frame_length_err_lanes_int);}
    if (info->ilkn_lane_meta_frame_repeat_err_lanes_int != 0)     {soc_sand_os_printf("ilkn_lane_meta_frame_repeat_err_lanes_int:    %u\n\r",info->ilkn_lane_meta_frame_repeat_err_lanes_int);}
    if (info->ilkn_rx_port_status_err_int != 0)                   {soc_sand_os_printf("ilkn_rx_port_status_err_int:                  %u\n\r",info->ilkn_rx_port_status_err_int);}
    if (info->ilkn_tx_port_status_err_int != 0)                   {soc_sand_os_printf("ilkn_tx_port_status_err_int:                  %u\n\r",info->ilkn_tx_port_status_err_int);}
    if (info->ilkn_rx_parity_err_int != 0)                        {soc_sand_os_printf("ilkn_rx_parity_err_int:                       %u\n\r",info->ilkn_rx_parity_err_int);}
    if (info->ilkn_tx_parity_err_int != 0)                        {soc_sand_os_printf("ilkn_tx_parity_err_int:                       %u\n\r",info->ilkn_tx_parity_err_int);}
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ILKN_STAT_print(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "\n\rRX:\n\r"
    "--\n\r"
  );
  soc_sand_os_printf("rx_stat_aligned_raw:     %u\n\r",info->rx_stat_aligned_raw);
  soc_sand_os_printf("rx_port_active:          %u\n\r",info->rx_port_active);
  soc_sand_os_printf("rx_stat_misaligned:      %u\n\r",info->rx_stat_misaligned);
  soc_sand_os_printf("rx_stat_aligned_err:     %u\n\r",info->rx_stat_aligned_err);
  soc_sand_os_printf("rx_stat_crc24_err:       %u\n\r",info->rx_stat_crc24_err);
  soc_sand_os_printf("rx_stat_miss_sop_err:    %u\n\r",info->rx_stat_miss_sop_err);
  soc_sand_os_printf("rx_stat_miss_eop_err:    %u\n\r",info->rx_stat_miss_eop_err);
  soc_sand_os_printf("rx_stat_overflow_err:    %u\n\r",info->rx_stat_overflow_err);
  soc_sand_os_printf("rx_stat_burstmax_err:    %u\n\r",info->rx_stat_burstmax_err);
  soc_sand_os_printf("rx_stat_burst_err:       %u\n\r",info->rx_stat_burst_err);
  soc_sand_os_printf("rx_lp_ifc_status:        %u\n\r",info->rx_lp_ifc_status);
  soc_sand_os_printf("rx_stat_buff_parity_err: %u\n\r",info->ilkn_rx_stat_buff_parity_err);
  
  soc_sand_os_printf(
    "\n\rTX:\n\r"
    "--\n\r"
  );
  soc_sand_os_printf("tx_ovfout:               %u\n\r",info->tx_ovfout);
  soc_sand_os_printf("tx_port_active:          %u\n\r",info->tx_port_active);
  soc_sand_os_printf("tx_stat_underflow_err:   %u\n\r",info->tx_stat_underflow_err);
  soc_sand_os_printf("tx_stat_burst_err:       %u\n\r",info->tx_stat_burst_err);
  soc_sand_os_printf("tx_stat_overflow_err:    %u\n\r",info->tx_stat_overflow_err);
  soc_sand_os_printf("tx_stat_buff_parity_err: %u\n\r",info->ilkn_tx_stat_buff_parity_err);
  

  soc_sand_os_printf(
    "\n\r"
    "Per-lane Bitmap, RX:\n\r"
    "--------------------\n\r"
  );
  soc_sand_os_printf("link_partner_lanes_status_bitmap:     0x%6x\n\r",info->link_partner_lanes_status_bitmap);
  soc_sand_os_printf("link_partner_interface_status_bitmap: 0x%6x\n\r",info->link_partner_interface_status_bitmap);
  soc_sand_os_printf("crc32_lane_valid_indication_bitmap:   0x%6x\n\r",info->crc32_lane_valid_indication_bitmap);
  soc_sand_os_printf("crc32_lane_err_indication_bitmap:     0x%6x\n\r",info->crc32_lane_err_indication_bitmap);
  soc_sand_os_printf("lane_synchronization_achieved_bitmap: 0x%6x\n\r",info->lane_synchronization_achieved_bitmap);
  soc_sand_os_printf("lane_synchronization_err_bitmap:      0x%6x\n\r",info->lane_synchronization_err_bitmap);
  soc_sand_os_printf("lane_framing_err_bitmap:              0x%6x\n\r",info->lane_framing_err_bitmap);
  soc_sand_os_printf("lane_bad_framing_layer_err_bitmap:    0x%6x\n\r",info->lane_bad_framing_layer_err_bitmap);
  soc_sand_os_printf("lane_meta_frame_sync_word_err_bitmap: 0x%6x\n\r",info->lane_meta_frame_sync_word_err_bitmap);
  soc_sand_os_printf("lane_scrambler_state_err_bitmap:      0x%6x\n\r",info->lane_scrambler_state_err_bitmap);
  soc_sand_os_printf("lane_meta_frame_length_err_bitmap:    0x%6x\n\r",info->lane_meta_frame_length_err_bitmap);
  soc_sand_os_printf("lane_meta_frame_repeat_err_bitmap:    0x%6x\n\r",info->lane_meta_frame_repeat_err_bitmap);
  soc_sand_os_printf("rx_stat_lanes_parity_err_bitmap:      0x%6x\n\r",info->ilkn_rx_stat_lanes_parity_err_bitmap);

  soc_sand_os_printf(
    "\n\r"
    "Per-lane Bitmap, TX:\n\r"
    "--------------------\n\r"
  );
  soc_sand_os_printf("tx_stat_lanes_parity_err:             0x%6x\n\r",info->ilkn_tx_stat_lanes_parity_err);
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ILKN_DIAG_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_DIAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "\n\r"
    "Interrupt indications(non-zero only):\n\r"
    "=====================================\n\r"
  );
  SOC_PB_NIF_ILKN_INT_print(&(info->interrupt));

  soc_sand_os_printf(
    "\n\r"
    "Status indications:\n\r"
    "===================\n\r"
  );
  SOC_PB_NIF_ILKN_STAT_print(&(info->stat));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_STAT_LINK_ACTIVITY_print(
    SOC_SAND_IN  SOC_PB_NIF_STAT_LINK_ACTIVITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("is_up: %u\n\r",info->is_up);
  soc_sand_os_printf("is_active_rx: %u\n\r",info->is_active_rx);
  soc_sand_os_printf("is_active_tx: %u\n\r",info->is_active_tx);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ACTIVITY_STATUS_print(
    SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enabled: %u\n\r",info->enabled);
  soc_sand_os_printf("status:");
  SOC_PB_NIF_STAT_LINK_ACTIVITY_print(&(info->status));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ACTIVITY_STATUS_ALL_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS_ALL *info
  )
{
  uint32
    res,
    active_cnt = 0,
    link_up_cnt = 0,
    active_rx_cnt = 0,
    active_tx_cnt = 0,
    nif_id;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PETRA_INTERFACE_ID
    nif_ndx;
  uint8
    is_active_id;
  uint32
    nif_to_ports[SOC_PB_NIF_NOF_NIFS];
  uint8
    is_multiple_ports[SOC_PB_NIF_NOF_NIFS];
  char
    count_value[50];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
/* 
* COVERITY * 
* Coverity note: "index_bounded_by_const: Pointer "nif_to_ports" indexed by scalar "port_indx" which is upper bounded by constant "80U" in conditional "port_indx < 80U" " */
/* checked and there is no overun */
/* coverity[overrun-buffer-val] */
  soc_pb_nif_port_mapping_get(unit, nif_to_ports, is_multiple_ports);

  for (nif_id = 0; nif_id < SOC_PB_NIF_NOF_NIFS; ++nif_id)
  {
    if((info->activity[nif_id].is_up) || (info->activity[nif_id].is_active_rx) || (info->activity[nif_id].is_active_tx))
    {
      res = soc_pb_nif_is_active_intern_id(
              unit,
              nif_id,
              &is_active_id,
              &nif_ndx,
              &nif_type);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      
      if(is_active_id)
      {
        if(active_cnt == 0)
        {
          soc_sand_os_printf("+---------------------------------+\n\r");
          soc_sand_os_printf("| NIF-Id (Port-Id) | Up | RX | TX |\n\r");
          soc_sand_os_printf("+---------------------------------+\n\r");
        }
        active_cnt++;
        if(info->activity[nif_id].is_up)
        {
          link_up_cnt++;
        }
        if(info->activity[nif_id].is_active_rx)
        {
          active_rx_cnt++;
        }
        if(info->activity[nif_id].is_active_tx)
        {
          active_tx_cnt++;
        }

        if (nif_type == SOC_PB_NIF_TYPE_NONE)
        {
          /* Should not get here */
          soc_sand_os_printf("| %s%-3u", "INTERN-", nif_id);
        }
        else
        {
          soc_sand_os_printf("| %-10s", soc_petra_PETRA_INTERFACE_ID_to_string(soc_pb_nif_intern2nif_id(nif_type, nif_id)));
        }

        if (nif_to_ports[nif_id] != SOC_SAND_INTERN_VAL_INVALID_32)
        {
          sal_sprintf(
            count_value,
            "(%u%s",
            nif_to_ports[nif_id],
            is_multiple_ports[nif_id]?",..)":")"
            );
          soc_sand_os_printf("%-7s", count_value);
        }
        else
        {
          soc_sand_os_printf(" %-4s", "(None)");
        }

        soc_sand_os_printf("| %s  ",info->activity[nif_id].is_up?"V":"-");
        soc_sand_os_printf("| %s  ",info->activity[nif_id].is_active_rx?"V":"-");
        soc_sand_os_printf("| %s  |\n\r",info->activity[nif_id].is_active_tx?"V":"-");
      }
    }
  }

  if(active_cnt != 0)
  {
    soc_sand_os_printf("+---------------------------------+\n\r");
    soc_sand_os_printf("Total: %u links-up, %u active-RX, %u active-TX\n\r", link_up_cnt, active_rx_cnt, active_tx_cnt);
  }
  else
  {
    soc_sand_os_printf("No activity on the Network Interfaces, all links down\n\r");
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_LINK_STATUS_print(
    SOC_SAND_IN  SOC_PB_NIF_LINK_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("link_status: %u\n\r",info->link_status);
  soc_sand_os_printf("link_status_change: %u\n\r",info->link_status_change);
  soc_sand_os_printf("pcs_sync: %u\n\r",info->pcs_sync);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_SYNCE_CLK_print(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("nif_id:");
  soc_petra_PETRA_INTERFACE_ID_print(info->nif_id);
  soc_sand_os_printf("clk_divider %s ", SOC_PB_NIF_SYNCE_CLK_DIV_to_string(info->clk_divider));
  soc_sand_os_printf("squelch_enable: %u\n\r",info->squelch_enable);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_PTP_CLK_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("ptp_clk_delta: %u\n\r",info->ptp_clk_delta);
  soc_sand_os_printf("sync_reset_val:");
  soc_sand_u64_print(&(info->sync_reset_val), 0, TRUE);
  soc_sand_os_printf("sync_autoinc_interval:");
  soc_sand_u64_print(&(info->sync_autoinc_interval), 0, TRUE);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_PTP_CLK_RST_VAL_print(
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_RST_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("curr_time [Nanoseconds]:\n\r");
  soc_sand_u64_print(&info->curr_time, 0, TRUE);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_PTP_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("wire_delay_ns: %u[Nanoseconds]\n\r",info->wire_delay_ns);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ELK_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("mal_id: %u\n\r",info->mal_id);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_FATP_MODE_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mode %s ", SOC_PB_NIF_FATP_MODE_to_string(info->mode));
  soc_sand_os_printf("is_bypass_enable: %u\n\r",info->is_bypass_enable);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_FATP_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("base_port_id: %u\n\r",info->base_port_id);
  soc_sand_os_printf("nof_ports: %u\n\r",info->nof_ports);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_NIF_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN SOC_SAND_64CNT                        all_counters[SOC_PB_NIF_NOF_COUNTERS]
  )
{
  uint32
    counter_i;
  uint8
    short_format = TRUE,
    dont_print_zero = TRUE ;

  if (NULL == all_counters)
  {
    soc_sand_os_printf("SOC_PB_NIF_ALL_STATISTIC_COUNTERS_print received NULL ptr\n\r");
    goto exit;
  }

  soc_sand_os_printf("%u counters:\n\r", SOC_PB_NIF_NOF_COUNTERS);
  for(counter_i = 0; counter_i < SOC_PB_NIF_NOF_COUNTERS; ++counter_i)
  {
    if(
       (soc_sand_u64_is_zero(&(all_counters[counter_i].u64))) &&
       (dont_print_zero)
      )
    {
      continue;
    }
    soc_sand_os_printf("  %-38s:", SOC_PB_NIF_COUNTER_TYPE_to_string(counter_i));
    soc_sand_64cnt_print(&(all_counters[counter_i]), short_format);
    soc_sand_os_printf("\n\r");
  }

exit:
  return;
}

void
  SOC_PB_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN     int                        unit,
    SOC_SAND_INOUT  SOC_SAND_64CNT                       all_counters[SOC_PB_NIF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS]
  )
{
  uint32
    res,
    counter_indx,
    nof_printed = 0,
    total_printed = 0,
    str_prog,
    nif_per_block = 3,
    start_nif,
    nif_id,
    nof_nifs_to_print = 0,
    idx;
  uint8
    dont_print_zero = TRUE,
    not_zero;
  uint8
    nif_to_print[SOC_PB_NIF_NOF_NIFS],
    counter_to_print[SOC_PB_NIF_NOF_COUNTERS];
  SOC_SAND_IN SOC_SAND_U64
    *u64;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PETRA_INTERFACE_ID
    nif_ndx;
  uint32
    nif_to_ports[SOC_PB_NIF_NOF_NIFS];
  uint8
    is_multiple_ports[SOC_PB_NIF_NOF_NIFS];
  char
    count_value[50];
  uint8
    is_active_id,
    is_line_active = FALSE,
    overflowed;


  if (NULL == all_counters)
  {
    soc_sand_os_printf("SOC_PB_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print received NULL ptr\n\r");
    goto exit;
  }
/* 
* COVERITY * 
* Coverity note: "index_bounded_by_const: Pointer "nif_to_ports" indexed by scalar "port_indx" which is upper bounded by constant "80U" in conditional "port_indx < 80U" " */
/* checked and there is no overun */
/* coverity[overrun-buffer-val] */
  soc_pb_nif_port_mapping_get(unit, nif_to_ports, is_multiple_ports);

  if (dont_print_zero)
  {
    sal_memset(
      nif_to_print,
      0x0,
      SOC_PB_NIF_NOF_NIFS * sizeof(uint8)
    );

    sal_memset(
      counter_to_print,
      0x0,
      SOC_PB_NIF_NOF_COUNTERS * sizeof(uint8)
    );
  }
  else
  {
    /* Petra b code. Almost not in use. Ignore coverity defects */
    /* coverity[dead_error_begin] */
    sal_memset(
      nif_to_print,
      0x1,
      SOC_PB_NIF_NOF_NIFS * sizeof(uint8)
    );

    sal_memset(
      counter_to_print,
      0x1,
      SOC_PB_NIF_NOF_COUNTERS * sizeof(uint8)
    );
    nof_nifs_to_print = SOC_PB_NIF_NOF_NIFS;
  }

  if (dont_print_zero)
  {
    for(nif_id = 0; nif_id < SOC_PB_NIF_NOF_NIFS; ++nif_id)
    {
      for(counter_indx = 0; counter_indx < SOC_PB_NIF_NOF_COUNTERS; ++counter_indx)
      {
        not_zero = (uint8)!(soc_sand_u64_is_zero(&(all_counters[nif_id][counter_indx].u64)));
        if (
          (nif_to_print[nif_id] == 0) &&
          (not_zero == TRUE)
          )
        {
          nif_to_print[nif_id] = TRUE;
          nof_nifs_to_print++;
        }
        counter_to_print[counter_indx] |= not_zero;
      }
    }
  }

  if (nof_nifs_to_print == 0 && dont_print_zero)
  {
    soc_sand_os_printf("All NIF Counters are clear (0)\n\r");
    goto exit;
  }

  for(nif_id = 0; nif_id < SOC_PB_NIF_NOF_NIFS; )
  {
    total_printed += nof_printed;
    if (total_printed == nof_nifs_to_print)
    {
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }
    
    soc_sand_os_printf("+------------------------------------------------------------------------------------+\n\r");
    soc_sand_os_printf("|                   NIF Counters: NIF-Id          (Local-Port-Id)                    |\n\r");
    soc_sand_os_printf("+------------------------------------------------------------------------------------+\n\r");
    soc_sand_os_printf("|Counter\\IF(Port)     |");

    start_nif = SOC_PB_NIF_NOF_NIFS;

    for(nof_printed = 0; (nof_printed < nif_per_block) && (nif_id < SOC_PB_NIF_NOF_NIFS); ++nif_id)
    {
      res = soc_pb_nif_is_active_intern_id(
              unit,
              nif_id,
              &is_active_id,
              &nif_ndx,
              &nif_type);
      if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK){
        soc_sand_os_printf("Failed in soc_pb_nif_is_active_intern_id\n");
        goto exit;
      }
    
      if((nif_to_print[nif_id]) && (is_active_id))
      {
        is_line_active = TRUE;
        
        if (nif_type == SOC_PB_NIF_TYPE_NONE)
        {
          /* Should not get here */
          soc_sand_os_printf("   %s%-3u", "INTERN-", nif_id);
        }
        else
        {
          soc_sand_os_printf("   %-10s", soc_petra_PETRA_INTERFACE_ID_to_string(soc_pb_nif_intern2nif_id(nif_type, nif_id)));
        }
        if (nif_to_ports[nif_id] != SOC_SAND_INTERN_VAL_INVALID_32)
        {
          sal_sprintf(
            count_value,
            "(%u%s",
            nif_to_ports[nif_id],
            is_multiple_ports[nif_id]?",..)":")"
          );
          soc_sand_os_printf("%-7s|", count_value);
        }
        else
        {
          soc_sand_os_printf(" %-4s|", "(None)");
        }

        if (start_nif == SOC_PB_NIF_NOF_NIFS)
        {
          start_nif = nif_id;
        }
        ++nof_printed;
      }
    }
    if ((is_line_active) && (nof_printed < nif_per_block))
    {
      for (idx = 0; idx < (nif_per_block - nof_printed); idx++)
      {
        soc_sand_os_printf("%-20s|", "");
      }
    }

    soc_sand_os_printf("\n\r");
    soc_sand_os_printf("+------------------------------------------------------------------------------------+\n\r");

    if (nof_printed == 0)
    {
      goto exit;
    }

    for(counter_indx = 0; counter_indx < SOC_PB_NIF_NOF_COUNTERS; ++counter_indx)
    {
      if (!counter_to_print[counter_indx])
      {
        continue;
      }

      is_line_active = FALSE;
      soc_sand_os_printf("|%-21s|", SOC_PB_NIF_COUNTER_TYPE_to_string(counter_indx));

      for(nif_id = start_nif, nof_printed = 0; nof_printed < nif_per_block  && nif_id < SOC_PB_NIF_NOF_NIFS; ++nif_id)
      {
        if (!nif_to_print[nif_id])
        {
          continue;
        }
        is_line_active = TRUE;
        ++nof_printed;
        u64 = &(all_counters[nif_id][counter_indx].u64);
        overflowed = (uint8)all_counters[nif_id][counter_indx].overflowed;

        str_prog = 0;

        if (overflowed)
        {
          sal_sprintf(count_value, "%s" , "      OverFlow");
        }
        else
        {
          if(0 != u64->arr[1])
          {
            sal_sprintf(
              count_value,
              "%3u*2^32+",
              u64->arr[1]
            );
            str_prog = 9;
          }
          sal_sprintf(
            count_value + str_prog,
            "%u",
            u64->arr[0]
          );
        }

      soc_sand_os_printf("%-20s|",count_value);
      }
      if ((is_line_active) && (nof_printed < nif_per_block))
      {
        for (idx = 0; idx < (nif_per_block - nof_printed); idx++)
        {
          soc_sand_os_printf("%-20s|", "");
        }
      }
      soc_sand_os_printf("\n\r");
    }
    soc_sand_os_printf("+------------------------------------------------------------------------------------+\n\r");
    soc_sand_os_printf("\n\r");
  }
exit:
  return;
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

