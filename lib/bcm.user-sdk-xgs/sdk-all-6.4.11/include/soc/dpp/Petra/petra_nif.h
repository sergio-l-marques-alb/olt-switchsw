/* $Id: petra_nif.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_nif.h
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


#ifndef __SOC_PETRA_NIF_INCLUDED__
/* { */
#define __SOC_PETRA_NIF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_nif.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* Inter Packet Gap limits */
#define  SOC_PETRA_NIF_IPG_SIZE_MIN 1
#define  SOC_PETRA_NIF_IPG_SIZE_MAX 255

/* Burst Control Tag limits */
#define  SOC_PETRA_NIF_BCT_SIZE_MAX   2
#define  SOC_PETRA_NIF_BCT_CH_IDX_MAX 3

/* } */

/*************
 * MACROS    *
 *************/
/* { */
#define SOC_PETRA_NIF_IS_LAST_MAL(mal_ndx) \
  ((mal_ndx % SOC_PETRA_MAX_NIFS_PER_MAL) == (SOC_PETRA_MAX_NIFS_PER_MAL - 1))

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
* NAME:
*     soc_petra_nif_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_nif_regs_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_nif_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_on_off_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0 - 31.
*  SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info -
*     Chooses whether to turn the NIF on or off, and whether
*     to power up/down the attached SerDes also.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_on_off_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_on_off_verify
* TYPE:
*   PROC
* FUNCTION:
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0 - 31.
*  SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info -
*     Chooses whether to turn the NIF on or off, and whether
*     to power up/down the attached SerDes also.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_on_off_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_on_off_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0 - 31.
*  SOC_SAND_OUT SOC_PETRA_NIF_STATE_INFO      *info -
*     Chooses whether to turn the NIF on or off, and whether
*     to power up/down the attached SerDes also.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_on_off_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_STATE_INFO      *info
  );


/*****************************************************
* NAME
*    soc_petra_nif_is_sgmii_get
* TYPE:
*   PROC
* DATE:
*   07/02/2008
* FUNCTION:
*   Identify sgmii interface
* INPUT:
*   SOC_SAND_IN  uint32       mal_ndx -
*      MAC Lane index. Range: 0 - 7.
*   SOC_SAND_OUT uint8       *is_sgmii -
*     TRUE if the interface is sgmii.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This function is more efficient then
*   nif_type_getm and nif_mal_basic_conf_get
*****************************************************/
uint32
  soc_petra_nif_is_sgmii_get(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       mal_ndx,
    SOC_SAND_OUT uint8       *is_sgmii
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info -
*     Basic Network interface configuration -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_verify
* TYPE:
*   PROC
* FUNCTION:
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info -
*     Basic Network interface configuration -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_OUT SOC_PETRA_NIF_INFO            *nif_info -
*     Basic Network interface configuration -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_INFO            *nif_info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_set_all_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES] -
*     Basic Network interface configuration, all (8) MAL-s -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_set_all_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_get_all_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Gets basic NIF configuration. After setting basic
*     configuration. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
SOC_SAND_OUT  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
*     Basic Network interface configuration, all (8) MAL-s -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_get_all_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  );

uint32
  soc_petra_nif_mal_basic_conf_verify_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_topology_validate_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Validates NIF to SerDes mapping topology configuration.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* REMARKS:
*     Effective after the relevant MAL-s were enabled.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_topology_validate_unsafe(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_enable_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disable specified MAL.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_IN  uint8                 is_enabled -
*     If TRUE, the specified MAL will be enabled. Otherwise -
*     disabled.
* REMARKS:
*     This function must be called after completing NIF
*     configuration.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_enable_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disable specified MAL.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_IN  uint8                 is_enabled -
*     If TRUE, the specified MAL will be enabled. Otherwise -
*     disabled.
* REMARKS:
*     This function must be called after completing NIF
*     configuration.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_enable_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disable specified MAL.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_OUT uint8                 *is_enabled -
*     If TRUE, the specified MAL will be enabled. Otherwise -
*     disabled.
* REMARKS:
*     This function must be called after completing NIF
*     configuration.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint8                 *is_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_conf_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets SGMII/1000BASE-X interface configuration
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO      *info -
*     SGMII-specific configuration.
* REMARKS:
*     1. This API does not configure the SerDes. Dedicated API
*     must be used for SerDes configuration. 2. The rate can
*     also be configured via a separate API.
*     3. Th get function is not entirely symmetric to the set function
*     (where only rx, tx or both directions can be defined). The get
*     function returns both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_conf_verify
* TYPE:
*   PROC
* FUNCTION:
*     Sets SGMII/1000BASE-X interface configuration
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO      *info -
*     SGMII-specific configuration.
* REMARKS:
*     1. This API does not configure the SerDes. Dedicated API
*     must be used for SerDes configuration. 2. The rate can
*     also be configured via a separate API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_conf_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets SGMII/1000BASE-X interface configuration
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_rx -
*     SGMII-specific configuration for rx.
*  SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_tx -
*     SGMII-specific configuration for tx.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     1. This API does not configure the SerDes. Dedicated API
*     must be used for SerDes configuration. 2. The rate can
*     also be configured via a separate API. 3. Not entirely
*     symmetric to the set function (where only rx,
*     tx or both directions can be defined). The get function returns
*     the both directions.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_rx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_status_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Gets SGMII/1000BASE-X interface diagnostics status
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_NIF_SGMII_STAT      *status -
*     SGMII interface status.
* REMARKS:
*     None .
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_status_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_STAT      *status
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_rate_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Sets SGMII interface link-rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate -
*     SGMII rate (10/100/1000 Mbps).
* REMARKS:
*   1. The SGMII rate is typically configured upon
*   initialization by the sgmii_conf_get_verify API, called
*   by init_sequence_phase1. 2. The SGMII rate configuration
*   changes the link rate. It does not alter the
*   Autonegotiation enable/disable configuration, which is
*   PHY-dependant.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_rate_verify
* TYPE:
*   PROC
* FUNCTION:
*   Sets SGMII interface link-rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate -
*     SGMII rate (10/100/1000 Mbps).
* REMARKS:
*   1. The SGMII rate is typically configured upon
*   initialization by the sgmii_conf_get_verify API, called
*   by init_sequence_phase1. 2. The SGMII rate configuration
*   changes the link rate. It does not alter the
*   Autonegotiation enable/disable configuration, which is
*   PHY-dependant.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_rate_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Sets SGMII interface link-rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate -
*     SGMII rate (10/100/1000 Mbps).
* REMARKS:
*   1. The SGMII rate is typically configured upon
*   initialization by the sgmii_conf_get_verify API, called
*   by init_sequence_phase1. 2. The SGMII rate configuration
*   changes the link rate. It does not alter the
*   Autonegotiation enable/disable configuration, which is
*   PHY-dependant.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_RATE      *rate
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_spaui_conf_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO      *info -
*     SPAUI extensions information - - preamble- ipg
*     (inter-packet gap)- channelized interface configuration-
*     crc mode
* REMARKS:
*     1. This is a SPAUI extension to standard XAUI
*     interface.2. IPG configuration is only applicable in TX
*     direction - will be ignored for RX configuration.
*     3. Th get function is not entirely symmetric to the set function
*     (where only rx, tx or both directions can be defined). The get
*     function returns both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_spaui_conf_verify
* TYPE:
*   PROC
* FUNCTION:
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO      *info -
*     SPAUI extensions information - - preamble- ipg
*     (inter-packet gap)- channelized interface configuration-
*     crc mode
* REMARKS:
*     1. This is a SPAUI extension to standard XAUI
*     interface.2. IPG configuration is only applicable in TX
*     direction - will be ignored for RX configuration.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_spaui_conf_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_rx -
*     SPAUI extensions information for rx - - preamble- ipg
*     (inter-packet gap)- channelized interface configuration-
*     crc mode
*  SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_tx -
*     SPAUI extensions information for tx - - preamble- ipg
*     (inter-packet gap)- channelized interface configuration-
*     crc mode
* REMARKS:
*     1. This is a SPAUI extension to standard XAUI
*     interface.2. IPG configuration is only applicable in TX
*     direction - will be ignored for RX configuration.
*     3. Not entirely symmetric to the set function (where only rx,
*     tx or both directions can be defined). The get function returns
*     the both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_rx,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_type_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Gets interface type and channelized indication
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_OUT SOC_PETRA_NIF_TYPE            *nif_type -
*     Interface type (i.e. SGMII).
*  SOC_SAND_OUT uint8                 *is_channelized -
*     TRUE if the interface is channelized.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_TYPE            *nif_type,
    SOC_SAND_OUT uint8                 *is_channelized
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_min_packet_size_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets minimal packet size.
*     Note: Normally, the packet size is limited
*     using soc_petra_mgmt_pckt_size_range_set(), and not this API.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range 0-7.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  uint32                  pckt_size -
*     Minimal packet size in Bytes.
* REMARKS:
*   1. Normally, the packet size is limited using
*      soc_petra_mgmt_pckt_size_range_set(), and not this API.
*   2. For RX direction - the NIF will drop smaller packets.
*      Note that the inspected packet size includes packet CRC.
*   3. For TX direction - the NIF will pad smaller packets with trailing zeros.
*      The packet CRC is calculated on the whole packet, including zeros.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_min_packet_size_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  uint32                  pckt_size
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_min_packet_size_verify
* TYPE:
*   PROC
* FUNCTION:
*     Sets minimal packet size.
*     Note: Normally, the packet size is limited
*     using soc_petra_mgmt_pckt_size_range_set(), and not this API.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range 0-7.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  uint32                  pckt_size -
*     Minimal packet size in Bytes.
* REMARKS:
*   1. Normally, the packet size is limited using
*      soc_petra_mgmt_pckt_size_range_set(), and not this API.
*   2. For RX direction - the NIF will drop smaller packets.
*      Note that the inspected packet size includes packet CRC.
*   3. For TX direction - the NIF will pad smaller packets with trailing zeros.
*      The packet CRC is calculated on the whole packet, including zeros.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_min_packet_size_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  uint32                  pckt_size
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_min_packet_size_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets minimal packet size.
*     Note: Normally, the packet size is limited
*     using soc_petra_mgmt_pckt_size_range_set(), and not this API.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range 0-7.
*  SOC_SAND_OUT uint32                  *pckt_size_rx -
*     Minimal rx packet size in Bytes.
*  SOC_SAND_OUT uint32                  *pckt_size_tx -
*     Minimal tx packet size in Bytes.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*   1. Normally, the packet size is limited using
*      soc_petra_mgmt_pckt_size_range_set(), and not this API.
*   2. For RX direction - the NIF will drop smaller packets.
*      Note that the inspected packet size includes packet CRC.
*   3. For TX direction - the NIF will pad smaller packets with trailing zeros.
*      The packet CRC is calculated on the whole packet, including zeros.
*   4. Not entirely symmetric to the set function (where only rx,
*      tx or both directions can be defined). The get function returns
*      the both directions.
*********************************************************************/
uint32
  soc_petra_nif_min_packet_size_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint32                  *pckt_size_rx,
    SOC_SAND_OUT uint32                  *pckt_size_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_local_fault_ovrd_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  uint8                 is_lf_ovrd -
*     If TRUE, Local Fault is indicated. This triggers the
*     Soc_petra to send Remote Fault indication on the MAL.
* REMARKS:
*     This API handles LF indicated by a source external to
*     the Soc_petra device (e.g. external PHY). Internal LFS is
*     always handled by the Soc_petra.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_lf_ovrd
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_local_fault_ovrd_verify
* TYPE:
*   PROC
* FUNCTION:
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  uint8                 is_lf_ovrd -
*     If TRUE, Local Fault is indicated. This triggers the
*     Soc_petra to send Remote Fault indication on the MAL.
* REMARKS:
*     This API handles LF indicated by a source external to
*     the Soc_petra device (e.g. external PHY). Internal LFS is
*     always handled by the Soc_petra.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_lf_ovrd
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_local_fault_ovrd_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_OUT uint8                 *is_lf_ovrd -
*     If TRUE, Local Fault is indicated. This triggers the
*     Soc_petra to send Remote Fault indication on the MAL.
* REMARKS:
*     This API handles LF indicated by a source external to
*     the Soc_petra device (e.g. external PHY). Internal LFS is
*     always handled by the Soc_petra.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint8                 *is_lf_ovrd
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fat_pipe_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures Fat Pipe interface, including FAP Ports to
*     NIF mapping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info -
*     FAT PIPE Configuration
* REMARKS:
*     After resequencing, the packets that arrived through one
*     of the Fat Pipe ports (FAP ports 1-4) appear as if they
*     arrived through FAP port 1, and must be treated as such
*     downstream (scheduling etc.). In other words, when Fat
*     Pipe enabled, FAP port 1 accumulates the bandwidth of
*     FAP ports 1-4, and FAP ports 2-4 cannot be used for
*     other purposes.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fat_pipe_verify
* TYPE:
*   PROC
* FUNCTION:
*     Configures Fat Pipe interface, including FAP Ports to
*     NIF mapping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info -
*     FAT PIPE Configuration
* REMARKS:
*     After resequencing, the packets that arrived through one
*     of the Fat Pipe ports (FAP ports 1-4) appear as if they
*     arrived through FAP port 1, and must be treated as such
*     downstream (scheduling etc.). In other words, when Fat
*     Pipe enabled, FAP port 1 accumulates the bandwidth of
*     FAP ports 1-4, and FAP ports 2-4 cannot be used for
*     other purposes.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fat_pipe_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures Fat Pipe interface, including FAP Ports to
*     NIF mapping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_NIF_FAT_PIPE_INFO   *info -
*     FAT PIPE Configuration
* REMARKS:
*     After resequencing, the packets that arrived through one
*     of the Fat Pipe ports (FAP ports 1-4) appear as if they
*     arrived through FAP port 1, and must be treated as such
*     downstream (scheduling etc.). In other words, when Fat
*     Pipe enabled, FAP port 1 accumulates the bandwidth of
*     FAP ports 1-4, and FAP ports 2-4 cannot be used for
*     other purposes.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mdio22_write_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     MDIO Clause 22 write function
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  phy_addr -
*     The address of the slave device on the MDIO bus.
*  SOC_SAND_IN  uint32                  reg_addr -
*     The address of the register within the slave device that
*     is to be written.
*  SOC_SAND_IN  uint32                  size_in_words -
*     Size in words (16 bit) of the data to write
*  SOC_SAND_IN  uint16                  *data -
*     Buffer containing the data to write. Must be at least of
*     size "size_in_words".
* REMARKS:
*     If 'size_in_words' is > 1, consecutive addresses are written with the
*     values provided in the 'data' buffer.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mdio22_write_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_IN  uint16                  *data
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mdio22_read_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     MDIO Clause 22 read function
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  phy_addr -
*     The address of the slave device on the MDIO bus.
*  SOC_SAND_IN  uint32                  reg_addr -
*     The address of the register within the slave device that
*     is to be read.
*  SOC_SAND_IN  uint32                  size_in_words -
*     Size in words (16 bit) of the data to read
*  SOC_SAND_OUT uint16                  *data -
*     Buffer containing the data to read. Must be at least of
*     size "size_in_words".
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mdio22_read_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_OUT uint16                  *data
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mdio45_write_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     MDIO Clause 45 write function
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  port_addr -
*     The address of the slave device on the MDIO bus.
*  SOC_SAND_IN  uint32                  dev_addr -
*     The device page address of the slave device.
*  SOC_SAND_IN  uint32                  reg_addr -
*     The address of the register within the slave device that
*     is to be written.
*  SOC_SAND_IN  uint32                  size_in_words -
*     Size in words (16 bit) of the data to write
*  SOC_SAND_IN  uint16                  *data -
*     Buffer containing the data to write. Must be at least of
*     size "size_in_words".
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mdio45_write_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port_addr,
    SOC_SAND_IN  uint32                  dev_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_IN  uint16                  *data
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mdio45_read_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     MDIO Clause 45 read function
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  port_addr -
*     The address of the slave device on the MDIO bus.
*  SOC_SAND_IN  uint32                  dev_addr -
*     The device page address of the slave device.
*  SOC_SAND_IN  uint32                  reg_addr -
*     The address of the register within the slave device that
*     is to be read.
*  SOC_SAND_IN  uint32                  size_in_words -
*     Size in words (16 bit) of the data to read
*  SOC_SAND_OUT uint16                  *data -
*     Buffer for the data to be read. Must be at least of size
*     "size_in_words".
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mdio45_read_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port_addr,
    SOC_SAND_IN  uint32                  dev_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_OUT uint16                  *data
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_quanta_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx -
*     Selects whether the configuration refers to flow control
*     generation, reception or both.
*  SOC_SAND_IN  uint32                  pause_quanta -
*     Pause quanta: - FC Reception - pause quanta to be used
*     upon receiving link-level flow control via the BCT,
*     ignored for other modes. - FC Generation - pause quanta
*     to send. Units: 512 bit times (as defined in IEEE 802.3x
*     standard).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_IN  uint32                  pause_quanta
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_quanta_verify
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx -
*     Selects whether the configuration refers to flow control
*     generation, reception or both.
*  SOC_SAND_IN  uint32                  pause_quanta -
*     Pause quanta: - FC Reception - pause quanta to be used
*     upon receiving link-level flow control via the BCT,
*     ignored for other modes. - FC Generation - pause quanta
*     to send. Units: 512 bit times (as defined in IEEE 802.3x
*     standard).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_IN  uint32                  pause_quanta
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_quanta_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx -
*     Selects whether the configuration refers to flow control
*     generation, reception or both.
*  SOC_SAND_OUT uint32                  *pause_quanta -
*     Pause quanta: - FC Reception - pause quanta to be used
*     upon receiving link-level flow control via the BCT,
*     ignored for other modes. - FC Generation - pause quanta
*     to send. Units: 512 bit times (as defined in IEEE 802.3x
*     standard).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_OUT uint32                  *pause_quanta
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_frame_src_addr_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS       *mac_addr -
*     Source MAC address, part of 802.3 pause frame.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS       *mac_addr
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_frame_src_addr_verify
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS       *mac_addr -
*     Source MAC address, part of 802.3 pause frame.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS       *mac_addr
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_frame_src_addr_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS       *mac_addr -
*     Source MAC address, part of 802.3 pause frame.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS       *mac_addr
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fc_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info -
*     Flow control configuration.
* REMARKS:
*     This API only covers flow control handling on a NIF port
*     level. Please refer to device-level flow control API-s
*     in the flow_control module.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fc_verify
* TYPE:
*   PROC
* FUNCTION:
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info -
*     Flow control configuration.
* REMARKS:
*     This API only covers flow control handling on a NIF port
*     level. Please refer to device-level flow control API-s
*     in the flow_control module.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fc_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_NIF_FC_INFO         *info -
*     Flow control configuration.
* REMARKS:
*     This API only covers flow control handling on a NIF port
*     level. Please refer to device-level flow control API-s
*     in the flow_control module.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_FC_INFO         *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_loopback_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set all Network Interface SerDes in loopback.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  uint8                 enable -
*     If TRUE, all the NIF SerDes lanes are set in loopback,
*     NSILB mode. Otherwise - all NIF SerDes lane are set not
*     to be in loopback.
* REMARKS:
*     The SerDes loopback mode that is used is NSILB.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_loopback_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_loopback_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set all Network Interface SerDes in loopback.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  uint8                 enable -
*     If TRUE, all the NIF SerDes lanes are set in loopback,
*     NSILB mode. Otherwise - all NIF SerDes lane are set not
*     to be in loopback.
* REMARKS:
*     The SerDes loopback mode that is used is NSILB.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_loopback_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_loopback_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set all Network Interface SerDes in loopback.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_OUT uint8                 *enable -
*     If TRUE, all the NIF SerDes lanes are set in loopback,
*     NSILB mode. Otherwise - all NIF SerDes lane are set not
*     to be in loopback.
* REMARKS:
*     The SerDes loopback mode that is used is NSILB.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_loopback_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT uint8                 *enable
  );

uint32
  soc_petra_nif_id_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx
  );


/*********************************************************************
* NAME:
*     soc_petra_nif_counter_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Gets counter value of the NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  uint32                 port_ndx -
*     Channel Id, Range: For SGMII, 0-3 For XAUI 0-0.
*  SOC_SAND_IN  SOC_PETRA_NIF_COUNTER_TYPE  counter_type -
*     Counter Type.
*  SOC_SAND_OUT SOC_SAND_64CNT                *counter -
*     counter Value
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_counter_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_COUNTER_TYPE  counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                   *counter_val
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_all_counters_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Gets counter value of the NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  uint32                 channel_ndx -
*     Channel Id, Range: For SGMII, 0-3 For XAUI 0-0.
*  SOC_SAND_IN  SOC_PETRA_NIF_COUNTER_TYPE  counter_type -
*     Counter Type.
*  SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_NIF_NOF_COUNTERS] -
*     all NIF counters Values, order according to SOC_PETRA_NIF_COUNTER_TYPE.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_all_counters_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_NIF_NOF_COUNTERS]
  );


uint32
  soc_petra_nif_all_nifs_all_counters_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_IF_NOF_NIFS][SOC_PETRA_NIF_NOF_COUNTERS]
  );


/*********************************************************************
* NAME:
*     soc_petra_nif_link_status_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Gets link status, and whether was change in the status.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  uint32                 channel_ndx -
*     Channel Id, Range: For SGMII, 0-3 For XAUI 0-0.
*  SOC_SAND_OUT SOC_PETRA_NIF_LINK_STATUS        *link_status -
*     link status, and whether was change in the status.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_link_status_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_LINK_STATUS        *link_status
  );
uint32
  soc_petra_nif_is_channelized_unsafe(
    SOC_SAND_IN   int         unit,
    SOC_SAND_IN   SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT  uint8         *is_channelized
  );

/************************************************************************/
/* Checks is FAT pipe interface is currently enabled                    */
/************************************************************************/
uint32
  soc_petra_nif_fat_pipe_enable_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *is_enabled
  );

uint32
  soc_petra_nif_is_fat_pipe_port(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 port_ndx,
    SOC_SAND_OUT uint8 *is_fatp_port
  );

uint32
  soc_petra_nif_is_fat_pipe_mal(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 mal_ndx,
    SOC_SAND_OUT uint8 *is_fatp_mal
  );

int soc_petra_port_init(int unit);
int soc_petra_port_post_init(int unit, soc_pbmp_t* ports);
int soc_petra_port_deinit(int unit);
int soc_petra_port_enable_set(int unit, soc_port_t port, uint32 flags, int enable);
int soc_petra_port_enable_get(int unit, soc_port_t port, uint32 flags, int* enable);
int soc_petra_port_speed_set(int unit, soc_port_t port, int speed);
int soc_petra_port_speed_get(int unit, soc_port_t port, int* speed);
int soc_petra_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
int soc_petra_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_NIF_INCLUDED__*/
#endif
