/* $Id: petra_api_nif.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_nif.c
*
* MODULE PREFIX:  soc_petra_nif
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

#include <soc/dpp/Petra/petra_api_nif.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_api_ports.h>
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
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_on_off_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_ON_OFF_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_on_off_verify(
    unit,
    nif_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_on_off_set_unsafe(
    unit,
    nif_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_on_off_set()",0,0);
}

/*********************************************************************
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_on_off_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_STATE_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_ON_OFF_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_on_off_get_unsafe(
    unit,
    nif_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_on_off_get()",0,0);
}

/*********************************************************************
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(nif_info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_mal_basic_conf_verify(
    unit,
    mal_ndx,
    nif_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_mal_basic_conf_set_unsafe(
    unit,
    mal_ndx,
    nif_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_set()",0,0);
}

/*********************************************************************
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_INFO            *nif_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(nif_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_mal_basic_conf_get_unsafe(
    unit,
    mal_ndx,
    nif_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_get()",0,0);
}

uint32
  soc_petra_nif_mal_basic_conf_set_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_SET_ALL);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_mal_basic_conf_verify_all(
          unit,
          nif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_nif_mal_basic_conf_set_all_unsafe(
            unit,
            nif_info
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_set_all()",0,0);
}


uint32
  soc_petra_nif_mal_basic_conf_get_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_GET_ALL);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_mal_basic_conf_get_all_unsafe(
          unit,
          nif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_get_all()",0,0);
}

/*********************************************************************
*     Validates NIF to SerDes mapping topology configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_topology_validate(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_TOPOLOGY_VALIDATE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_topology_validate_unsafe(
    unit
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_topology_validate()",0,0);
}

/*********************************************************************
*     Enables/disable specified MAL.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_enabled
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_mal_enable_verify(
    unit,
    mal_ndx,
    is_enabled
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_mal_enable_set_unsafe(
    unit,
    mal_ndx,
    is_enabled
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_enable_set()",0,0);
}

/*********************************************************************
*     Enables/disable specified MAL.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint8                 *is_enabled
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_enabled);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_mal_enable_get_unsafe(
    unit,
    mal_ndx,
    is_enabled
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_enable_get()",0,0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_CONF_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_sgmii_conf_verify(
    unit,
    nif_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_sgmii_conf_set_unsafe(
    unit,
    nif_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_conf_set()",0,0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_rx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_tx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_CONF_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_sgmii_conf_get_unsafe(
          unit,
          nif_ndx,
          info_rx,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_conf_get()",0,0);
}

/*********************************************************************
*     Gets SGMII/1000BASE-X interface diagnostics status
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_STAT      *status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_STATUS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_sgmii_status_get_unsafe(
    unit,
    nif_ndx,
    status
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_status_get()",0,0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface rate - autonegotiation
*     mode, or explicit.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_RATE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);


  res = soc_petra_nif_sgmii_rate_verify(
    unit,
    nif_ndx,
    rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_sgmii_rate_set_unsafe(
    unit,
    nif_ndx,
    rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_rate_set()",0,0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface rate - autonegotiation
*     mode, or explicit.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_RATE      *rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_RATE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rate);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_sgmii_rate_get_unsafe(
    unit,
    nif_ndx,
    rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_rate_get()",0,0);
}

/*********************************************************************
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SPAUI_CONF_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_spaui_conf_verify(
    unit,
    mal_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_spaui_conf_set_unsafe(
    unit,
    mal_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_spaui_conf_set()",0,0);
}

/*********************************************************************
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_rx,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_tx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SPAUI_CONF_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_spaui_conf_get_unsafe(
          unit,
          mal_ndx,
          info_rx,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_spaui_conf_get()",0,0);
}

/*********************************************************************
*     Gets interface type and channelized indication
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_TYPE            *nif_type,
    SOC_SAND_OUT uint8                 *is_channelized
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(nif_type);
  SOC_SAND_CHECK_NULL_INPUT(is_channelized);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_type_get_unsafe(
    unit,
    mal_ndx,
    nif_type,
    is_channelized
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_type_get()",0,0);
}

/*********************************************************************
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_lf_ovrd
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOCAL_FAULT_OVRD_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_local_fault_ovrd_verify(
    unit,
    mal_ndx,
    is_lf_ovrd
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_local_fault_ovrd_set_unsafe(
    unit,
    mal_ndx,
    is_lf_ovrd
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_local_fault_ovrd_set()",0,0);
}

/*********************************************************************
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint8                 *is_lf_ovrd
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOCAL_FAULT_OVRD_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_lf_ovrd);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_local_fault_ovrd_get_unsafe(
    unit,
    mal_ndx,
    is_lf_ovrd
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_local_fault_ovrd_get()",0,0);
}

/*********************************************************************
*     Configures Fat Pipe interface, including FAP Ports to
*     NIF mapping.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FAT_PIPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_fat_pipe_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_fat_pipe_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fat_pipe_set()",0,0);
}

/*********************************************************************
*     Configures Fat Pipe interface, including FAP Ports to
*     NIF mapping.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FAT_PIPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_fat_pipe_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fat_pipe_get()",0,0);
}

/*********************************************************************
*     MDIO Clause 22 write function
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mdio22_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_IN  uint16                  *data
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MDIO22_WRITE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = soc_petra_nif_mdio22_write_unsafe(
    unit,
    phy_addr,
    reg_addr,
    size_in_words,
    data
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mdio22_write()",0,0);
}

/*********************************************************************
*     MDIO Clause 22 read function
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mdio22_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_OUT uint16                  *data
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MDIO22_READ);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_mdio22_read_unsafe(
    unit,
    phy_addr,
    reg_addr,
    size_in_words,
    data
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mdio22_read()",0,0);
}

/*********************************************************************
*     MDIO Clause 45 write function
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mdio45_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port_addr,
    SOC_SAND_IN  uint32                  dev_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_IN  uint16                  *data
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MDIO45_WRITE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_mdio45_write_unsafe(
          unit,
          port_addr,
          dev_addr,
          reg_addr,
          size_in_words,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mdio45_write()",0,0);
}

/*********************************************************************
*     MDIO Clause 45 read function
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mdio45_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port_addr,
    SOC_SAND_IN  uint32                  dev_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_OUT uint16                  *data
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MDIO45_READ);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_mdio45_read_unsafe(
    unit,
    port_addr,
    dev_addr,
    reg_addr,
    size_in_words,
    data
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mdio45_read()",0,0);
}

/*********************************************************************
*     Sets minimal packet size as limited by the NIF. Note:
*     Normally, the packet size is limited using
*     soc_petra_mgmt_pckt_size_range_set(), and not this API.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_min_packet_size_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  uint32                  pckt_size
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MIN_PACKET_SIZE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_min_packet_size_verify(
    unit,
    mal_ndx,
    direction_ndx,
    pckt_size
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_min_packet_size_set_unsafe(
    unit,
    mal_ndx,
    direction_ndx,
    pckt_size
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_min_packet_size_set()",0,0);
}

/*********************************************************************
*     Sets minimal packet size as limited by the NIF. Note:
*     Normally, the packet size is limited using
*     soc_petra_mgmt_pckt_size_range_set(), and not this API.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_min_packet_size_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint32                  *pckt_size_rx,
    SOC_SAND_OUT uint32                  *pckt_size_tx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MIN_PACKET_SIZE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pckt_size_rx);
  SOC_SAND_CHECK_NULL_INPUT(pckt_size_tx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

 res = soc_petra_nif_min_packet_size_get_unsafe(
          unit,
          mal_ndx,
          pckt_size_rx,
          pckt_size_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_min_packet_size_get()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_IN  uint32                  pause_quanta
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_QUANTA_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);


  res = soc_petra_nif_pause_quanta_verify(
    unit,
    mal_ndx,
    direction_ndx,
    pause_quanta
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_pause_quanta_set_unsafe(
    unit,
    mal_ndx,
    direction_ndx,
    pause_quanta
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_quanta_set()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_OUT uint32                  *pause_quanta
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_QUANTA_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pause_quanta);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

 res = soc_petra_nif_pause_quanta_get_unsafe(
    unit,
    mal_ndx,
    direction_ndx,
    pause_quanta
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_quanta_get()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS          *mac_addr
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_pause_frame_src_addr_verify(
    unit,
    mal_ndx,
    mac_addr
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_pause_frame_src_addr_set_unsafe(
    unit,
    mal_ndx,
    mac_addr
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_frame_src_addr_set()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS       *mac_addr
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_pause_frame_src_addr_get_unsafe(
    unit,
    mal_ndx,
    mac_addr
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_frame_src_addr_get()",0,0);
}

/*********************************************************************
*     Configures the way Networks Interface handles Link Level
*     Flow Control generation (TX) and reception (RX)
*     internally.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FC_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_fc_verify(
    unit,
    nif_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_fc_set_unsafe(
    unit,
    nif_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fc_set()",0,0);
}

/*********************************************************************
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_FC_INFO         *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_fc_get_unsafe(
    unit,
    nif_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fc_get()",0,0);
}

/*********************************************************************
*     Set all Network Interface SerDes in loopback.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_loopback_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOOPBACK_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);


  res = soc_petra_nif_loopback_verify(
    unit,
    nif_ndx,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_loopback_set_unsafe(
    unit,
    nif_ndx,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_loopback_set()",0,0);
}

/*********************************************************************
*     Set all Network Interface SerDes in loopback.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_loopback_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOOPBACK_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_loopback_get_unsafe(
    unit,
    nif_ndx,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_loopback_get()",0,0);
}


/*********************************************************************
*     Gets counter value of the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_counter_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_COUNTER_TYPE  counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                   *counter_val
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_COUNTER_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counter_val);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);


  res = soc_petra_nif_id_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_counter_get_unsafe(
          unit,
          nif_ndx,
          counter_type,
          counter_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_counter_get()",0,0);
}




/*********************************************************************
*     Gets counter value of the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_all_counters_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_NIF_NOF_COUNTERS]
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_ALL_COUNTERS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counters_val);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_id_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_all_counters_get_unsafe(
          unit,
          nif_ndx,
          counters_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_all_counters_get()",0,0);
}


/*********************************************************************
*     Gets counter value of the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_all_nifs_all_counters_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_IF_NOF_NIFS][SOC_PETRA_NIF_NOF_COUNTERS]
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_ALL_NIFS_ALL_COUNTERS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counters_val);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_all_nifs_all_counters_get_unsafe(
          unit,
          counters_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_all_counters_get()",0,0);
}
/*********************************************************************
*     Gets link status, and whether was change in the status.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_link_status_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_LINK_STATUS        *link_status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LINK_STATUS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(link_status);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_id_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_link_status_get_unsafe(
          unit,
          nif_ndx,
          link_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_link_status_get()",0,0);
}


uint32
  soc_petra_nif_is_channelized(
    SOC_SAND_IN   int         unit,
    SOC_SAND_IN   SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT  uint8         *is_channelized
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_IS_CHANNELIZED);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_channelized);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_nif_is_channelized_unsafe(
    unit,
    nif_ndx,
    is_channelized
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_is_channelized()",0,0);
}
void
  soc_petra_PETRA_NIF_LANES_SWAP_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_LANES_SWAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_LANES_SWAP));
  info->swap_rx = 0;
  info->swap_tx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_INFO));
  info->type = SOC_PETRA_NIF_NOF_TYPES;
  info->serdes_rate = SOC_PETRA_NIF_NOF_SERDES_RATES;
  soc_petra_PETRA_NIF_LANES_SWAP_clear(&(info->lanes_swap));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_PREAMBLE_COMPRESS_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_PREAMBLE_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_PREAMBLE_COMPRESS));
  info->enable = 0;
  info->size = SOC_PETRA_NIF_NOF_PRMBL_SIZES;
  info->skip_SOP = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_IPG_COMPRESS_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_IPG_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_IPG_COMPRESS));
  info->enable = 0;
  info->size = 0;
  info->dic_mode = SOC_PETRA_NIF_NOF_IPG_DIC_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_CHANNELIZED_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_CHANNELIZED *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_CHANNELIZED));
  info->enable = 0;
  info->bct_size = 0;
  info->bct_channel_byte_ndx = 0;
  info->is_burst_interleaving = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_SPAUI_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_SPAUI_INFO));
  info->enable = 0;
  info->link_partner_double_size_bus = 0;
  soc_petra_PETRA_NIF_PREAMBLE_COMPRESS_clear(&(info->preamble));
  soc_petra_PETRA_NIF_IPG_COMPRESS_clear(&(info->ipg));
  info->crc_mode = SOC_PETRA_NIF_CRC_MODE_32;
  soc_petra_PETRA_NIF_CHANNELIZED_clear(&(info->ch_conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_SGMII_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_SGMII_INFO));
  info->enable = 0;
  info->mode = SOC_PETRA_NIF_1GE_MODE_SGMII;
  info->is_autoneg = 1;
  info->rate = SOC_PETRA_NIF_SGMII_RATE_1000Mbps;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_SGMII_STAT_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_SGMII_STAT));
  info->autoneg_not_complete = FALSE;
  info->remote_fault = FALSE;
  info->fault_type = SOC_PETRA_NIF_SGMII_RFLT_OK;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_FAT_PIPE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_FAT_PIPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_FAT_PIPE_INFO));
  info->enable = 0;
  info->nof_ports = 0;
  info->bypass_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_STATE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_STATE_INFO));
  info->on_off = SOC_PETRA_NIF_NOF_STATES;
  info->serdes_also = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_FC_TX_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_FC_TX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_FC_TX));
  info->ll_mode = SOC_PETRA_NIF_NOF_LL_FC_MODES;
  info->cb_enable = 0;
  info->on_if_cleared = TRUE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_FC_RX_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_FC_RX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_FC_RX));
  info->ll_mode = SOC_PETRA_NIF_NOF_LL_FC_MODES;
  info->treat_ll_class_based = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_FC_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_FC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_FC_INFO));
  soc_petra_PETRA_NIF_FC_RX_clear(&(info->rx));
  soc_petra_PETRA_NIF_FC_TX_clear(&(info->tx));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_LINK_STATUS_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_LINK_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_NIF_LINK_STATUS));
  info->link_status = 0;
  info->link_status_change = 0;
  info->pl_status_change = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_NIF_SERDES_RATE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_SERDES_RATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_SERDES_RATE_SINGLE:
    str = "SINGLE";
  break;

  case SOC_PETRA_NIF_SERDES_RATE_DOUBLE:
    str = "DOUBLE";
  break;

  case SOC_PETRA_NIF_NOF_SERDES_RATES:
    str = "NOF_SERDES_RATES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_TYPE_NONE:
    str = "NONE";
  break;

  case SOC_PETRA_NIF_TYPE_XAUI:
    str = "XAUI";
  break;

  case SOC_PETRA_NIF_TYPE_SPAUI:
    str = "SPAUI";
  break;

  case SOC_PETRA_NIF_TYPE_SGMII:
    str = "TYPE_SGMII";
  break;

  case SOC_PETRA_NIF_NOF_TYPES:
    str = "NOF_TYPES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_PREAMBLE_SIZE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_PREAMBLE_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_PRMBL_SIZE_COLUMNS_0:
    str = "SIZE_COLUMNS_0";
  break;

  case SOC_PETRA_NIF_PRMBL_SIZE_COLUMNS_1:
    str = "SIZE_COLUMNS_1";
  break;

  case SOC_PETRA_NIF_PRMBL_SIZE_COLUMNS_2:
    str = "SIZE_COLUMNS_2";
  break;

  case SOC_PETRA_NIF_NOF_PRMBL_SIZES:
    str = "NOF_PRMBL_SIZES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_IPG_DIC_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_IPG_DIC_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_IPG_DIC_MODE_AVERAGE:
    str = "AVERAGE";
  break;

  case SOC_PETRA_NIF_IPG_DIC_MODE_MINIMUM:
    str = "MINIMUM";
  break;

  case SOC_PETRA_NIF_NOF_IPG_DIC_MODES:
    str = "NOF_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_CRC_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_CRC_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_CRC_MODE_32:
    str = "CRC_32";
  break;

  case SOC_PETRA_NIF_CRC_MODE_24:
    str = "CRC_24";
  break;

  case SOC_PETRA_NIF_CRC_MODE_NONE:
    str = "NO_CRC";
  break;

  case SOC_PETRA_NIF_NOF_CRC_MODES:
    str = "NOF_CRC_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_1GE_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_1GE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_1GE_MODE_1000BASE_X:
    str = "1000BASE_X";
  break;

  case SOC_PETRA_NIF_1GE_MODE_SGMII:
    str = "SGMII";
  break;

  case SOC_PETRA_NIF_NOF_1GE_MODES:
    str = "NOF_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_SGMII_RATE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_SGMII_RATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_SGMII_RATE_10Mbps:
    str = "10Mbps";
  break;

  case SOC_PETRA_NIF_SGMII_RATE_100Mbps:
    str = "100Mbps";
  break;

  case SOC_PETRA_NIF_SGMII_RATE_1000Mbps:
    str = "1000Mbps";
  break;

  case SOC_PETRA_NIF_NOF_SGMII_RATES:
    str = "NOF_RATES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_SGMII_RFLT_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_SGMII_RFLT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_SGMII_RFLT_OK:
    str = "REMOTE_FAULT_OK";
  break;

  case SOC_PETRA_NIF_SGMII_RFLT_OFFLINE_REQ:
    str = "REMOTE_FAULT_OFFLINE_REQ";
  break;

  case SOC_PETRA_NIF_SGMII_RFLT_LNK_FAIL:
    str = "REMOTE_FAULT_LNK_FAIL";
  break;

  case SOC_PETRA_NIF_SGMII_RFLT_AUTONEG_ERR:
    str = "REMOTE_FAULT_AUTONEG_ERR";
  break;

  case SOC_PETRA_NIF_NOF_SGMII_RFLTS:
    str = "NOF_FAULTS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_STATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_STATE_ON:
    str = "ON";
  break;

  case SOC_PETRA_NIF_STATE_OFF:
    str = "OFF";
  break;

  case SOC_PETRA_NIF_NOF_STATES:
    str = "NOF_STATES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_NIF_LL_FC_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_LL_FC_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_NIF_LL_FC_MODE_DISABLE:
    str = "DISABLE";
  break;

  case SOC_PETRA_NIF_LL_FC_MODE_ENABLE_PAUSE_FRAME:
    str = "ENABLE_PAUSE_FRAME";
  break;

  case SOC_PETRA_NIF_LL_FC_MODE_ENABLE_BCT:
    str = "ENABLE_BCT";
  break;

  case SOC_PETRA_NIF_NOF_LL_FC_MODES:
    str = "NOF_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}



void
  soc_petra_PETRA_NIF_LANES_SWAP_print(
    SOC_SAND_IN SOC_PETRA_NIF_LANES_SWAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Swap_rx: %d, ",info->swap_rx);
  soc_sand_os_printf("Swap_tx: %d.\n\r",info->swap_tx);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "   Type: %s \n\r",
    soc_petra_PETRA_NIF_TYPE_to_string(info->type)
  );
  soc_sand_os_printf(
    "   Serdes_rate: %s \n\r",
    soc_petra_PETRA_NIF_SERDES_RATE_to_string(info->serdes_rate)
  );
  soc_sand_os_printf("   Lanes_swap: ");
  soc_petra_PETRA_NIF_LANES_SWAP_print(&(info->lanes_swap));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_PREAMBLE_COMPRESS_print(
    SOC_SAND_IN SOC_PETRA_NIF_PREAMBLE_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("    Enable:                      %u\n\r",info->enable);
  soc_sand_os_printf(
    "    Size:                        %s \n\r",
    soc_petra_PETRA_NIF_PREAMBLE_SIZE_to_string(info->size)
  );
  soc_sand_os_printf("    Skip_SOP:                    %u\n\r",info->skip_SOP);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_IPG_COMPRESS_print(
    SOC_SAND_IN SOC_PETRA_NIF_IPG_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("    Enable:                      %u\n\r",info->enable);
  soc_sand_os_printf("    Size:                        %u\n\r",info->size);
  soc_sand_os_printf(
    "    Dic_mode:                    %s \n\r",
    soc_petra_PETRA_NIF_IPG_DIC_MODE_to_string(info->dic_mode)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_CHANNELIZED_print(
    SOC_SAND_IN SOC_PETRA_NIF_CHANNELIZED *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("    Enable:                      %u\n\r",info->enable);
  soc_sand_os_printf("    Bct_size:                    %u\n\r",info->bct_size);
  soc_sand_os_printf("    Bct_channel_byte_ndx:        %u\n\r",info->bct_channel_byte_ndx);
  soc_sand_os_printf("    Is_burst_interleaving:       %u\n\r",info->is_burst_interleaving);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_SPAUI_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_SPAUI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("   Enable:                       %u\n\r",info->enable);
  soc_sand_os_printf("   Link_partner_double_size_bus: %d\n\r",info->link_partner_double_size_bus);
  soc_sand_os_printf("   Preamble:\n\r");
  soc_petra_PETRA_NIF_PREAMBLE_COMPRESS_print(&(info->preamble));
  soc_sand_os_printf("   Ipg:\n\r");
  soc_petra_PETRA_NIF_IPG_COMPRESS_print(&(info->ipg));
  soc_sand_os_printf(
    "   Crc_mode:                     %s \n\r",
    soc_petra_PETRA_NIF_CRC_MODE_to_string(info->crc_mode)
  );
  soc_sand_os_printf("   Ch_conf:\n\r");
  soc_petra_PETRA_NIF_CHANNELIZED_print(&(info->ch_conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_SGMII_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_SGMII_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("   Enable: %u\n\r",info->enable);
  soc_sand_os_printf(
    "   Mode:    %s \n\r",
    soc_petra_PETRA_NIF_1GE_MODE_to_string(info->mode)
  );
  soc_sand_os_printf("   Autoneg: %s\n\r",(info->is_autoneg)?"Enabled":"Disabled");
  soc_sand_os_printf(
    "   Rate:    %s \n\r",
    soc_petra_PETRA_NIF_SGMII_RATE_to_string(info->rate)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_SGMII_STAT_print(
    SOC_SAND_IN SOC_PETRA_NIF_SGMII_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("   Autoneg_not_complete: %u\n\r",info->autoneg_not_complete);
  soc_sand_os_printf("   Remote_fault:         %u\n\r",info->remote_fault);
  soc_sand_os_printf(
    "   Fault_type:           %s \n\r",
    soc_petra_PETRA_NIF_SGMII_RFLT_to_string(info->fault_type)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_FAT_PIPE_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_FAT_PIPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("   Enable:        %u\n\r",info->enable);
  soc_sand_os_printf("   Nof_ports:     %u[Ports]\n\r",info->nof_ports);
  soc_sand_os_printf("   Bypass_enable: %u\n\r",info->bypass_enable);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_STATE_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "   On-Off state %s, ",
    soc_petra_PETRA_NIF_STATE_to_string(info->on_off)
  );
  soc_sand_os_printf("Serdes_also: %u\n\r",info->serdes_also);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_FC_TX_print(
    SOC_SAND_IN SOC_PETRA_NIF_FC_TX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    " Ll_mode:       %s \n\r",
    soc_petra_PETRA_NIF_LL_FC_MODE_to_string(info->ll_mode)
  );
  soc_sand_os_printf(" Cb_enable:     %u\n\r",info->cb_enable);
  soc_sand_os_printf(" On_if_cleared: %u\n\r",info->on_if_cleared);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_FC_RX_print(
    SOC_SAND_IN SOC_PETRA_NIF_FC_RX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    " Ll_mode:              %s\n\r",
    soc_petra_PETRA_NIF_LL_FC_MODE_to_string(info->ll_mode)
  );
  soc_sand_os_printf(" Treat_ll_class_based: %u\n\r",info->treat_ll_class_based);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_NIF_FC_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_FC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Rx:\n\r");
  soc_petra_PETRA_NIF_FC_RX_print(&(info->rx));
  soc_sand_os_printf("Tx:\n\r");
  soc_petra_PETRA_NIF_FC_TX_print(&(info->tx));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_NIF_LINK_STATUS_print(
    SOC_SAND_IN SOC_PETRA_NIF_LINK_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Link_status:        %u \n\r", info->link_status);
  soc_sand_os_printf("Link_status_change: %u \n\r", info->link_status_change);
  soc_sand_os_printf("Pl_status_change:   %u \n\r", info->pl_status_change);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


const char*
  soc_petra_PETRA_NIF_COUNTER_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_COUNTER_TYPE enum_val,
    SOC_SAND_IN uint8                  short_format
  )
{
  const char* str = NULL;
  if (short_format)
  {
    switch(enum_val)
    {
    case SOC_PETRA_NIF_RX_OK_PACKETS:
      str = "RX_OK_PKTS";
      break;

    case SOC_PETRA_NIF_RX_OK_OCTETS:
      str = "RX_OK_OCTETS";
      break;

    case SOC_PETRA_NIF_RX_PACKETS_WITH_FRAMES_ERR :
      str = "RX_PKTS_FRM_ERR";
      break;

    case SOC_PETRA_NIF_RX_PACKETS_WITH_CRC_ERR:
      str = "RX_PKTS_CRC_ERR";
      break;

    case SOC_PETRA_NIF_RX_PACKETS_WITH_BCT_ERR:
      str = "RX_PKTS_BCT_ERR";
      break;

    case SOC_PETRA_NIF_RX_PAUSE_FRAMES:
      str = "RX_PAUSE_FRAMES";
      break;

    case SOC_PETRA_NIF_RX_MAC_CTRL_FRAME_EX_PAUSE:
      str = "RX_MAC_CTRL_PAUS";
      break;

    case SOC_PETRA_NIF_RX_MCAST_PACKETS:
      str = "RX_MCAST_PKTS";
      break;

    case SOC_PETRA_NIF_RX_BCAST_PACKETS:
      str = "RX_BCAST_PKTS";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_0_63:
      str = "RX_PKT_0_63";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_64:
      str = "RX_PKT_64";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_65_127:
      str = "RX_PKT_65_127";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_128_255:
      str = "RX_PKT_128_255";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_256_511:
      str = "RX_PKT_256_511";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_512_1023:
      str = "RX_PKT_512_1023";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_1024_1517:
      str = "RX_PKT_1024_1517";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_1518_MAX:
      str = "RX_PKT_1518_MAX";
      break;

    case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_TOO_LONG:
      str = "RX_PKT_TOO_LONG";
      break;

    case SOC_PETRA_NIF_TX_OK_OCTETS:
      str = "TX_OK_OCTETS";
      break;

    case SOC_PETRA_NIF_TX_OK_NORMAL_PACKETS:
      str = "TX_OK_NRML_PKTS";
      break;

    case SOC_PETRA_NIF_TX_MCAST_PACKETS:
      str = "TX_MCAST_PKTS";
      break;

    case SOC_PETRA_NIF_TX_BCAST_PACKETS:
      str = "TX_BCAST_PKTS";
      break;

    case SOC_PETRA_NIF_TX_PAUSE_FRAMES:
      str = "TX_PAUSE_FRAMES";
      break;

    case SOC_PETRA_NIF_RX_DISC_FIFO_OVF:
      str = "RX_DISC_FIFO_OVF";
      break;
    default:
      str = "Unknown";
    }
  }
  else
  {

    switch(enum_val)
    {
      case SOC_PETRA_NIF_RX_OK_PACKETS:
        str = "Number of Received packets Includes all pkt types:";
        break;

      case SOC_PETRA_NIF_RX_OK_OCTETS:
        str = "Number of Received octets in valid frames including CRC.";
        break;

      case SOC_PETRA_NIF_RX_PACKETS_WITH_FRAMES_ERR :
        str = "Received Packets with frame error.";
        break;

      case SOC_PETRA_NIF_RX_PACKETS_WITH_CRC_ERR:
        str = "Received Packets with CRC error.";
        break;

      case SOC_PETRA_NIF_RX_PACKETS_WITH_BCT_ERR:
        str = "Received Packets with BCT error.";
        break;

      case SOC_PETRA_NIF_RX_PAUSE_FRAMES:
        str = "Received Pause Frames.";
        break;

      case SOC_PETRA_NIF_RX_MAC_CTRL_FRAME_EX_PAUSE:
        str = "Received MAC Control Frames (excluding Pause) ";
        break;

      case SOC_PETRA_NIF_RX_MCAST_PACKETS:
        str = "Received MCAST Packets";
        break;

      case SOC_PETRA_NIF_RX_BCAST_PACKETS:
        str = "Received BCAST Packets";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_0_63:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 0 - 63";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_64:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 64";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_65_127:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 65-127";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_128_255:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 128-255";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_256_511:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 256-511";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_512_1023:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 512-1023";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_1024_1517:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 1024-1517";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_1518_MAX:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 1518-MAX";
        break;

      case SOC_PETRA_NIF_RX_PACKET_LEN_BINS_TOO_LONG:
        str = "Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) too long";
        break;

      case SOC_PETRA_NIF_TX_OK_OCTETS:
        str = "Transmitted octets in valid frames including CRC.";
        break;

      case SOC_PETRA_NIF_TX_OK_NORMAL_PACKETS:
        str = "Transmitted Normal packets (exclude pause frames, M-CAST and B-CAST frames";
        break;

      case SOC_PETRA_NIF_TX_MCAST_PACKETS:
        str = "Transmitted MCAST Packets";
        break;

      case SOC_PETRA_NIF_TX_BCAST_PACKETS:
        str = "Transmitted BCAST Packets";
        break;

      case SOC_PETRA_NIF_TX_PAUSE_FRAMES:
        str = "Transmitted MAC Control Frames (excluding FC bursts) ";
        break;

      case SOC_PETRA_NIF_RX_DISC_FIFO_OVF:
        str = "good packets Received that were discarded due to FIFO overflow";
        break;
      default:
        str = " Unknown";
    }
  }
 return str;
}


void
  soc_petra_PETRA_NIF_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN SOC_SAND_64CNT                        all_counters[SOC_PETRA_NIF_NOF_COUNTERS],
    SOC_SAND_IN SOC_SAND_PRINT_FLAVORS                flavor_bitmap
  )
{
  uint32
    counter_i;
  uint8
    short_format    = (uint8)flavor_bitmap & SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_SHORT),
    dont_print_zero = (uint8)flavor_bitmap & SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS);

  if (NULL == all_counters)
  {
    soc_sand_os_printf("soc_petra_PETRA_NIF_ALL_STATISTIC_COUNTERS_print received NULL ptr\n\r");
    goto exit;
  }

  soc_sand_os_printf("%u counters:\n\r", SOC_PETRA_NIF_NOF_COUNTERS);
  for(counter_i = 0; counter_i < SOC_PETRA_NIF_NOF_COUNTERS; ++counter_i)
  {
    if(
       (soc_sand_u64_is_zero(&(all_counters[counter_i].u64))) &&
       (dont_print_zero)
      )
    {
      continue;
    }
    soc_sand_os_printf("  %-38s:", soc_petra_PETRA_NIF_COUNTER_TYPE_to_string(counter_i, short_format));
    soc_sand_64cnt_print(&(all_counters[counter_i]), short_format);
    soc_sand_os_printf("\n\r");
  }

exit:
  return;
}


void
  soc_petra_PETRA_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN  int                        unit,
             SOC_SAND_64CNT                        all_counters[SOC_PETRA_IF_NOF_NIFS][SOC_PETRA_NIF_NOF_COUNTERS],
    SOC_SAND_IN  SOC_SAND_PRINT_FLAVORS                flavor_bitmap
  )
{
  uint32
    counter_indx,
    nof_printed = 0,
    total_printed = 0,
    str_prog,
    port_indx,
    nif_per_block = 3,
    start_nif,
    nif_indx,
    nof_nifs_to_print = 0,
    idx;
  uint8
    short_format    = (uint8)flavor_bitmap & SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_SHORT),
    dont_print_zero = (uint8)flavor_bitmap & SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS),
    not_zero;
  uint8
    nif_to_print[SOC_PETRA_IF_NOF_NIFS],
    counter_to_print[SOC_PETRA_NIF_NOF_COUNTERS];
  SOC_SAND_IN SOC_SAND_U64
    *u64;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    in_map_info,
    out_map_info;
  uint32
    nif_to_ports[SOC_PETRA_IF_NOF_NIFS];
  char
    count_value[50];
  uint8
    is_line_active = FALSE,
    overflowed;


  if (NULL == all_counters)
  {
    soc_sand_os_printf("soc_petra_PETRA_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print received NULL ptr\n\r");
    goto exit;
  }

  sal_memset(
    nif_to_ports,
    0xff,
    SOC_PETRA_IF_NOF_NIFS * sizeof(uint32)
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

    if (in_map_info.if_id < SOC_PETRA_IF_NOF_NIFS)
    {
      nif_to_ports[in_map_info.if_id] = port_indx;
    }
    else
    {
      if (out_map_info.if_id < SOC_PETRA_IF_NOF_NIFS)
      {
        nif_to_ports[out_map_info.if_id] = port_indx;
      }
    }
  }

  if (dont_print_zero)
  {
    sal_memset(
      nif_to_print,
      0x0,
      SOC_PETRA_IF_NOF_NIFS * sizeof(uint8)
    );

    sal_memset(
      counter_to_print,
      0x0,
      SOC_PETRA_NIF_NOF_COUNTERS * sizeof(uint8)
    );
  }
  else
  {
    sal_memset(
      nif_to_print,
      0x1,
      SOC_PETRA_IF_NOF_NIFS * sizeof(uint8)
    );

    sal_memset(
      counter_to_print,
      0x1,
      SOC_PETRA_NIF_NOF_COUNTERS * sizeof(uint8)
    );
    nof_nifs_to_print = SOC_PETRA_IF_NOF_NIFS;
  }

  if (dont_print_zero)
  {
    for(nif_indx = 0; nif_indx < SOC_PETRA_IF_NOF_NIFS; ++nif_indx)
    {
      for(counter_indx = 0; counter_indx < SOC_PETRA_NIF_NOF_COUNTERS; ++counter_indx)
      {
        not_zero = (uint8)!(soc_sand_u64_is_zero(&(all_counters[nif_indx][counter_indx].u64)));
        if (
          (nif_to_print[nif_indx] == 0) &&
          (not_zero == TRUE)
          )
        {
          nif_to_print[nif_indx] = TRUE;
          nof_nifs_to_print++;
        }
        counter_to_print[counter_indx] |= not_zero;
      }
    }
  }

  if (nof_nifs_to_print == 0 && dont_print_zero)
  {
    soc_sand_os_printf("All NIF Counters are clear (0)\n\r");
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }


  for(nif_indx = 0; nif_indx < SOC_PETRA_IF_NOF_NIFS; )
  {
    total_printed += nof_printed;
    if (total_printed == nof_nifs_to_print)
    {
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }
    soc_sand_os_printf("+-------------------------------------------------------------------------------+\n\r");
    soc_sand_os_printf("|                 NIF Counters: NIF-Id (MAL-Id) (Local-Port-Id)                 |\n\r");
    soc_sand_os_printf("+-------------------------------------------------------------------------------+\n\r");
    soc_sand_os_printf("|Counter\\IF(Port)|");


    start_nif = SOC_PETRA_IF_NOF_NIFS;
    for(nof_printed = 0; (nof_printed < nif_per_block) && (nif_indx < SOC_PETRA_IF_NOF_NIFS); ++nif_indx)
    {
      if(nif_to_print[nif_indx])
      {
        is_line_active = TRUE;
        soc_sand_os_printf("   %.2u (MAL %.1u)", nif_indx, SOC_PETRA_NIF2MAL_NDX(nif_indx));
        if (nif_to_ports[nif_indx] != SOC_SAND_U32_MAX)
        {
          sal_sprintf(
            count_value,
            "(%u)",
            nif_to_ports[nif_indx]
          );
          soc_sand_os_printf(" %-6s|", count_value);
        }
        else
        {
          soc_sand_os_printf(" %-4s|", "(None)");
        }

        if (start_nif == SOC_PETRA_IF_NOF_NIFS)
        {
          start_nif = nif_indx;
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
    soc_sand_os_printf("+-------------------------------------------------------------------------------+\n\r");

    if (nof_printed == 0)
    {
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

    for(counter_indx = 0; counter_indx < SOC_PETRA_NIF_NOF_COUNTERS; ++counter_indx)
    {
      if (!counter_to_print[counter_indx])
      {
        continue;
      }

      is_line_active = FALSE;
      soc_sand_os_printf("|%-16s|", soc_petra_PETRA_NIF_COUNTER_TYPE_to_string(counter_indx, short_format));

      for(nif_indx = start_nif, nof_printed = 0; nof_printed < nif_per_block  && nif_indx < SOC_PETRA_IF_NOF_NIFS; ++nif_indx)
      {
        if (!nif_to_print[nif_indx])
        {
          continue;
        }
        is_line_active = TRUE;
        ++nof_printed;
        u64 = &(all_counters[nif_indx][counter_indx].u64);
        overflowed = (uint8)all_counters[nif_indx][counter_indx].overflowed;

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
    soc_sand_os_printf("+-------------------------------------------------------------------------------+\n\r");
    soc_sand_os_printf("\n\r");
  }
exit:
  return;
}


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

