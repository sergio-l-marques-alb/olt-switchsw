/* $Id: pb_nif.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PB_NIF_INCLUDED__
/* { */
#define __SOC_PB_NIF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_NIF_ILKN_A_NOF_LANES_MAX                            (24)
#define SOC_PB_NIF_ILKN_B_NOF_LANES_MAX                            (12)
#define SOC_PB_NIF_ILKN_NOF_LANES_MAX                              SOC_SAND_MAX(SOC_PB_NIF_ILKN_A_NOF_LANES_MAX, SOC_PB_NIF_ILKN_B_NOF_LANES_MAX)
/* } */
/*************
 * MACROS    *
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
* NAME:
*     soc_pb_nif_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN SOC_PB_HW_ADJ_MDIO             *info -
*    MDIO clock frequency
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_pb_nif_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  mdio_clk_freq_khz,
    SOC_SAND_IN  uint8                 is_malgb_enabled
  );

/* $Id: pb_nif.h,v 1.7 Broadcom SDK $
 *	Set Interlaken clock configuration
 *  Must be done before SerDes initialization
 */
uint32
  soc_pb_nif_ilkn_clk_config(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  );

/*
 *	Set GMII clock configuration
 *  Must be done before SerDes initialization
 */
uint32
  soc_pb_nif_gmii_clk_config(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mal_ndx,
    SOC_SAND_IN uint8 is_sgmii_not_qsgmii,
    SOC_SAND_IN uint8 is_qsgmii_alt
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_on_off_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Turns the interface on/off. Optionally, powers up/down
 *   the attached SerDes also.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID              nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info -
 *     Chooses whether to turn the NIF on or off, and whether
 *     to power up/down the attached SerDes also.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_on_off_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info
  );

uint32
  soc_pb_nif_on_off_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info
  );

uint32
  soc_pb_nif_on_off_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_on_off_set_unsafe" API.
 *     Refer to "soc_pb_nif_on_off_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_on_off_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID                nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_STATE_INFO              *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_loopback_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the specified Network Interface to loopback, by
 *   setting all its SerDeses in loopback (NSILB)
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  uint8                      enable -
 *     If TRUE, the loopback is enabled
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_loopback_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      enable
  );

uint32
  soc_pb_nif_loopback_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      enable
  );

uint32
  soc_pb_nif_loopback_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_loopback_set_unsafe" API.
 *     Refer to "soc_pb_nif_loopback_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_loopback_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT uint8                      *enable
  );


/************************************************************************/
/* Global verify                                                        */
/************************************************************************/
uint32
  soc_pb_nif_mal_id_verify(
    SOC_SAND_IN  uint32                      mal_ndx
  );

uint32
  soc_pb_nif_id_verify(
    SOC_SAND_IN SOC_PETRA_INTERFACE_ID nif_ndx
  );

uint32
  soc_pb_nif_type_verify(
    SOC_SAND_IN SOC_PB_NIF_TYPE nif_type
  );

/************************************************************************/
/* Interlaken Reset:                                                    */
/*   Resets the ILKN FIFO, and if requested, ILKN PORT                  */
/*   Note: the ilkn_id is 0/1, and not the logical id (ILKN-A/B)        */
/************************************************************************/
uint32
  soc_pb_nif_ilkn_reset(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      ilkn_id,
    SOC_SAND_IN  uint8                      is_out_of_reset,
    SOC_SAND_IN  uint8                      is_fifo_only,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_mal_basic_conf_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets basic MAL configuration. This configuration defines
 *   MAL to SerDes mapping topology. Following setting basic
 *   configuration, per-NIF-type (e.g. SPAUI/SGMII etc.)
 *   configuration must be set before enabling the MAL.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      mal_ndx -
 *     MAL index. Range: 0 - 15
 *   SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO          *info -
 *     Basic MAL figuration: Type:
 *     XAUI/RXAUI/SGMII/QSGMII. Topology-related: lanes swap,
 *     alternative-QSGMII mapping
 * REMARKS:
 *   1. Refer to the Soc_petra-B User Manual for a detailed
 *   description of the supported NIF topologies 2. After MAL
 *   configuration, and enabling the requested MAL, the
 *   topology can be validated by calling
 *   soc_petra_nif_topology_validate API 3. This API should not
 *   be used for ILKN interface configuration. ILKN interface
 *   is configured using soc_pb_nif_ilkn_set API 4. This API does not set
 *   SerDes configuration (rate, RX/TX physical parameters
 *   etc.). The SerDes is expected to be initialized prior to
 *   calling this API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_mal_basic_conf_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO          *info
  );

uint32
  soc_pb_nif_mal_basic_conf_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO          *info
  );

uint32
  soc_pb_nif_mal_basic_conf_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_mal_basic_conf_set_unsafe" API.
 *     Refer to "soc_pb_nif_mal_basic_conf_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_nif_mal_basic_conf_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_PB_NIF_MAL_BASIC_INFO          *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_min_packet_size_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets minimal packet size as limited by the NIF. Note:
 *   Normally, the packet size is limited using
 *   soc_petra_mgmt_pckt_size_range_set(), and not this API.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      mal_ndx -
 *     MAL index. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx -
 *     Defines whether the configuration refers to RX, TX or
 *     both.
 *   SOC_SAND_IN  uint32                       pckt_size -
 *     Minimal packet size in Bytes. Range: 0 - 255.
 * REMARKS:
 *   1. Normally, the packet size is limited using
 *   soc_petra_mgmt_pckt_size_range_set(), and not this API. 2.
 *   For RX direction - the NIF will drop smaller packets.
 *   Note that the inspected packet size includes packet CRC.
 *   3. For TX direction - the NIF will pad smaller packets
 *   with trailing zeros. The packet CRC is calculated on the
 *   whole packet, including zeros. 4. The get function is
 *   not symmetric to the set function: both RX and TX
 *   settings are returned (direction_ndx is not passed).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_min_packet_size_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  uint32                       pckt_size
  );

uint32
  soc_pb_nif_min_packet_size_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  uint32                       pckt_size
  );

uint32
  soc_pb_nif_min_packet_size_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_min_packet_size_set_unsafe" API.
 *     Refer to "soc_pb_nif_min_packet_size_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_nif_min_packet_size_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT uint32                       *pckt_size_rx,
    SOC_SAND_OUT uint32                       *pckt_size_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_spaui_conf_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets SPAUI configuration - configure SPAUI extensions
 *   for XAUI interface.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Interface index. SPAUI Range: 1000 - 1007.
 *     (SOC_PB_NIF_ID(XAUI, 0-7)). RSPAUI Range: 2000 - 2015.
 *     (SOC_PB_NIF_ID(RXAUI, 0-15))
 *   SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx -
 *     Defines whether the configuration refers to RX, TX or
 *     both.
 *   SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO              *info -
 *     SPAUI extensions information - - preamble- ipg
 *     (inter-packet gap)- channelized interface configuration-
 *     crc mode
 * REMARKS:
 *   1. This is a SPAUI extension to standard XAUI
 *   interface.2. IPG configuration is only applicable in TX
 *   direction - will be ignored for RX configuration.3. The
 *   get function is not symmetric to the set function: both
 *   RX and TX settings are returned (direction_ndx is not
 *   passed).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_spaui_conf_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO              *info
  );

uint32
  soc_pb_nif_spaui_conf_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO              *info
  );

uint32
  soc_pb_nif_spaui_conf_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_spaui_conf_set_unsafe" API.
 *     Refer to "soc_pb_nif_spaui_conf_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_spaui_conf_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO              *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO              *info_tx
  );

/*
 *  Enable/Disable SGMII in the specified direction
 */
uint32
  soc_pb_nif_gmii_enable_state_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID          nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION  direction_ndx,
    SOC_SAND_IN  uint8                   is_enabled
  );

uint32
  soc_pb_nif_gmii_enable_state_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID          nif_ndx,
    SOC_SAND_OUT  uint8                  *is_enabled_rx,
    SOC_SAND_OUT  uint8                  *is_enabled_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_gmii_conf_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets SGMII/1000BASE-X interface configuration
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Interface index. SGMII Range: 3000 - 3031.
 *     (SOC_PB_NIF_ID(SGMII, 0-31)). QSGMII Range: 4000 - 4063.
 *     (SOC_PB_NIF_ID(QSGMII, 0-63))
 *   SOC_SAND_IN  SOC_PB_NIF_GMII_INFO           *info -
 *     SGMII-specific configuration.
 * REMARKS:
 *   1. This API does not configure the SerDes. Dedicated API
 *   must be used for SerDes configuration. 2. The rate can
 *   also be configured via a separate API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_gmii_conf_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO              *info
  );

uint32
  soc_pb_nif_gmii_conf_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO              *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_gmii_conf_set_unsafe" API.
 *     Refer to "soc_pb_nif_gmii_conf_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_gmii_conf_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_INFO               *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_gmii_status_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets SGMII/1000BASE-X interface diagnostics status
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Interface index. Range: 3000 - 3031. (SOC_PB_NIF_ID(SGMII,
 *     0-31))
 *   SOC_SAND_OUT SOC_PB_NIF_GMII_STAT              *status -
 *     SGMII interface status.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_gmii_status_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_STAT              *status
  );

uint32
  soc_pb_nif_gmii_status_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_gmii_rate_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets SGMII interface link-rate.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Interface index. Range: 3000 - 3031. (SOC_PB_NIF_ID(SGMII,
 *     0-31))
 *   SOC_SAND_IN  SOC_PB_NIF_GMII_RATE              rate -
 *     SGMII rate (10/100/1000 Mbps).
 * REMARKS:
 *   1. The SGMII rate is typically configured upon
 *   initialization by the sgmii_conf_set API, called by
 *   init_sequence_phase1.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_gmii_rate_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE              rate
  );

uint32
  soc_pb_nif_gmii_rate_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE              rate
  );

uint32
  soc_pb_nif_gmii_rate_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_gmii_rate_set_unsafe" API.
 *     Refer to "soc_pb_nif_gmii_rate_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_gmii_rate_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_RATE              *rate
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ilkn_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Function description
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx -
 *     Interlaken Index. Range: 5000 - 5001. (ILKN-A/ILKN-B).
 *     Can also use SOC_PB_NIF_ID(ILKN, 0 - 1).
 *   SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx -
 *     Defines whether the configuration refers to RX, TX or
 *     both.
 *   SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info -
 *     Interlaken Configuration
 * REMARKS:
 *   This API does not set Flow-Control-related configuration
 *   for Interlaken. This configuration is covered in the FC
 *   module.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ilkn_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  );

uint32
  soc_pb_nif_ilkn_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  );

uint32
  soc_pb_nif_ilkn_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_ilkn_set_unsafe" API.
 *     Refer to "soc_pb_nif_ilkn_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_ilkn_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO               *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO               *info_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ilkn_diag_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Reads the Interlaken status from the ILKN interrupt
 *   register and from other status registers. Clears the
 *   interrupts only if requested.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx -
 *     Interlaken Index. Range: 5000 - 5001. (ILKN-A/ILKN-B).
 *     Can also use SOC_PB_NIF_ID(ILKN, 0 - 1).
 *   SOC_SAND_IN  uint8                      clear_interrupts -
 *     If TRUE, the interrupts are cleared after reading
 *   SOC_SAND_OUT SOC_PB_NIF_ILKN_DIAG_INFO          *info -
 *     Interlaken Diagnostics information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ilkn_diag_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  uint8                      clear_interrupts,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_DIAG_INFO          *info
  );

uint32
  soc_pb_nif_ilkn_diag_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  uint8                      clear_interrupts
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_stat_activity_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Indicates whether the interface is configured and
 *   powered up
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS         *info -
 *     NIF-port link status and activity
 * REMARKS:
 *   1. Not applicable for ILKN 2. All the indications are
 *   cleared on read 3. Only the indications for the
 *   specified interface are cleared 4. For a status
 *   read/clear on all NIFs in single-access (per MALG), use
 *   soc_pb_nif_stat_activity_all_get
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_stat_activity_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS         *info
  );

uint32
  soc_pb_nif_stat_activity_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_stat_activity_all_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Indicates whether the interface is configured and
 *   powered up. All interfaces are read at once.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS_ALL     *info -
 *     Per NIF-port link status and activity
 * REMARKS:
 *   1. Not applicable for ILKN 2. All the indications are
 *   cleared on read
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_stat_activity_all_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS_ALL     *info
  );

uint32
  soc_pb_nif_stat_activity_all_ndx_verify(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_link_status_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets link status, and whether there was change in the
 *   status.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Network interface index. Range: 0 - 31.
 *   SOC_SAND_OUT SOC_PB_NIF_LINK_STATUS             *link_status -
 *     link status, and whether there was change in the status
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_link_status_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_LINK_STATUS             *link_status
  );

uint32
  soc_pb_nif_link_status_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_synce_clk_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The transmit clocks of all network interfaces are locked to a
 *   reference clock provided by an external CLKREF input pin.
 *   This function sets the SyncE recovered (reference) clock configuration.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx -
 *     SOC_SAND_IN SOC_PB_NIF_SYNCE_CLK_ID clk_ndx
 *   SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK               *clk -
 *     ndx - Recovered Clock Index. Range: 0 - 3. SOC_SAND_IN
 *     SOC_PB_NIF_SYNCE_CLK *clk - Recovered Clock configuration
 * REMARKS:
 *   1. soc_pb_nif_synce_mode_set must be used before this API to
 *   set the SYNCE signal mode. 2. If used, MAL Group B SYNCE
 *   clocks must be explicitly enabled at the initialization
 *   stage.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_synce_clk_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK               *clk
  );

uint32
  soc_pb_nif_synce_clk_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK               *clk
  );

uint32
  soc_pb_nif_synce_clk_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_synce_clk_set_unsafe" API.
 *     Refer to "soc_pb_nif_synce_clk_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_synce_clk_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_CLK               *clk
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_synce_mode_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Define the Recovered Clock signal to be transmitted on
 *   the Soc_petra external pins
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE              mode -
 *     The SYNCE signal mode.
 * REMARKS:
 *   If used, MAL Group B SYNCE clocks must be explicitly
 *   enabled at the initialization stage.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_synce_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE              mode
  );

uint32
  soc_pb_nif_synce_mode_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE              mode
  );

uint32
  soc_pb_nif_synce_mode_ndx_verify(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_synce_mode_set_unsafe" API.
 *     Refer to "soc_pb_nif_synce_mode_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_synce_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_MODE              *mode
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ptp_clk_reset_value_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Reads the PTP clock current reset time value at high
 *   resolution (nanoseconds).
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_RST_VAL         *info -
 *     The PTP clock current reset time value. Units:
 *     Nanoseconds.
 * REMARKS:
 *   The current reset value can also be read using the
 *   nif_ptp_clk_get API, at lower resolution (msec)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_reset_value_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_PTP_CLK_RST_VAL         *info
  );

uint32
  soc_pb_nif_ptp_clk_reset_value_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_PTP_CLK_RST_VAL         *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ptp_clk_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the PTP Clock configuration for the Precision Time
 *   Protocol
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO            *info -
 *     PTP Clock Configuration: frequency, reset-value and
 *     auto-increment.
 * REMARKS:
 *   When reading the configuration using het
 *   nif_ptp_clk_get, the reset value indicates current value
 *   and not originally-configured. This value can also be
 *   read using nif_ptp_clk_reset_value_get.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO            *info
  );

uint32
  soc_pb_nif_ptp_clk_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO            *info
  );

uint32
  soc_pb_nif_ptp_clk_ndx_verify(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_ptp_clk_set_unsafe" API.
 *     Refer to "soc_pb_nif_ptp_clk_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ptp_conf_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable and configure parameters for the
 *   Precision Time Protocol
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx -
 *     Defines whether the configuration refers to RX, TX or
 *     both.
 *   SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info -
 *     Enable/disable, and set configuration, of the Precision
 *     Time Protocol (PTP) on the specified MAL in the
 *     specified direction.
 * REMARKS:
 *   The actual configuration is per-MAL.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ptp_conf_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info
  );

uint32
  soc_pb_nif_ptp_conf_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info
  );

uint32
  soc_pb_nif_ptp_conf_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  );

/*
 *  Same as soc_pb_nif_ptp_conf_set_unsafe, which is actualy per-MAL
 */
uint32
  soc_pb_nif_ptp_conf_mal_config(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_TYPE                    nif_type,
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info
  );

/*********************************************************************
 *     Gets the configuration set by the
 *     "soc_pb_nif_ptp_conf_set_unsafe" API.
 *     Refer to "soc_pb_nif_ptp_conf_set_unsafe" API for details.
 *********************************************************************/
uint32
  soc_pb_nif_ptp_conf_get_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO *info_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_elk_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Select the MALG used for the External Lookup interface
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_ELK_INFO                *info -
 *     External Lookup Interface (ELK) configuration
 * REMARKS:
 *   1. The consumed MAL cannot be used for any Network
 *   Interface 2. If ILKN-A is enabled, MAL-0 cannot be used
 *   for ELK. If ILKN-B is enabled, ELK can be set on MAL-14
 *   (only).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_elk_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO                *info
  );

uint32
  soc_pb_nif_elk_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO                *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_elk_set_unsafe" API.
 *     Refer to "soc_pb_nif_elk_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_elk_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_ELK_INFO                *info
  );

/*
 *	Per Fat-pipe mode,
 *  return the number of Fat-pipes and
 *  the maximal allowed number of ports per Fat-pipe
 */
void
  soc_pb_nif_fatp_mode2count
  (
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE   fatp_mode,
    SOC_SAND_OUT uint32          *nof_fatps,
    SOC_SAND_OUT uint32          *nof_ports_per_fatp
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_fatp_mode_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Fat-pipe configuration mode
 * INPUT:
 *   SOC_SAND_IN  int                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO      *info -
 *     Fat-pipe configuration mode. Can be disabled, or enabled
 *     in one of the supported modes.
 * REMARKS:
 *   This API must be called before setting any Fat-pipe
 *   related configuration
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_fatp_mode_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO      *info
  );

uint32
  soc_pb_nif_fatp_mode_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO      *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_fatp_mode_set_unsafe" API.
 *     Refer to "soc_pb_nif_fatp_mode_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_fatp_mode_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PB_NIF_FATP_MODE_INFO      *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_fatp_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Fat-pipe configuration mode
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx -
 *     Fat-pipe index. According to the NIF_FATP_MODE: For 3x4:
 *     Range: 1 - 3. For 2x6: Range: 1 - 2. For 1x12: Range: 1 -
 *     1.
 *   SOC_SAND_IN  SOC_PB_NIF_FATP_INFO               *info -
 *     Fat-pipe configuration.
 * REMARKS:
 *   Fat-pipe mode must be set (soc_pb_nif_fatp_mode_set API)
 *   before calling this API.
 *  If the TM-port processing is Ethernet, the mapping from TM-Port
 *  to PP-Port must be set before calling this API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_fatp_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx,
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO               *info
  );

uint32
  soc_pb_nif_fatp_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx,
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO               *info
  );

uint32
  soc_pb_nif_fatp_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_fatp_set_unsafe" API.
 *     Refer to "soc_pb_nif_fatp_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_nif_fatp_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx,
    SOC_SAND_OUT SOC_PB_NIF_FATP_INFO               *info
  );

/************************************************************************/
/* 'is_fatp_port' is set to TRUE if the port is part of a Fat-pipe      */
/************************************************************************/
uint32
  soc_pb_nif_is_fatp_port_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   port_ndx,
    SOC_SAND_OUT uint8   *is_fatp_port
  );
/*********************************************************************
* NAME:
 *   soc_pb_nif_counter_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets Value of statistics counter of the NIF.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE            counter_type -
 *     Counter Type.
 *   SOC_SAND_OUT SOC_SAND_64CNT                     *counter_val -
 *     Counter Value.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_counter_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE            counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                     *counter_val
  );

uint32
  soc_pb_nif_counter_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE            counter_type
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_all_counters_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets Value of statistics counter of the NIF.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  uint8            non_data_also -
 *     If set, reads also Pause Frames and PTP counters
 *   SOC_SAND_OUT SOC_SAND_64CNT                     counter_val -
 *     SOC_PB_NIF_NOF_COUNTERS] - Counters Values
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_all_counters_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counter_val[SOC_PB_NIF_NOF_COUNTERS]
  );

uint32
  soc_pb_nif_all_counters_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_all_nifs_all_counters_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets Statistics Counters for all the NIF-s in the
 *   device.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8            non_data_also -
 *     If set, reads also Pause Frames and PTP counters
 *   SOC_SAND_OUT SOC_SAND_64CNT                     counters_val -
 *     [SOC_PETRA_IF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS] - Counters
 *     Values, all NIF-s.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_all_nifs_all_counters_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counters_val[SOC_PB_NIF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS]
  );

/*********************************************************************
* NAME:
*     soc_pb_nif_type_get - DEPRICATED
* TYPE:
*   PROC
* FUNCTION:
*     Gets interface type
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Interface ID.
* REMARKS:
*     None.
* RETURNS:
*  SOC_PETRA_NIF_TYPE
*     Interface type (i.e. SGMII).
*********************************************************************/

/************************************************************************/
/* Declared depricated; given a SOC_PETRA_INTERFACE_ID, better to use       */
/* soc_pb_nif_id2type(nif_ndx)  - no need to access the device              */
/************************************************************************/


/*********************************************************************
* NAME:
*     soc_pb_nif_mal_type_get
* TYPE:
*   PROC
* FUNCTION:
*     Gets mal type
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL ID.
*  SOC_SAND_OUT SOC_PB_NIF_TYPE        *nif_type
*     Interface type (i.e. SGMII).
* REMARKS:
*     None.
* RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_mal_type_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint32           mal_ndx,
    SOC_SAND_OUT SOC_PB_NIF_TYPE  *nif_type
  );


/*********************************************************************
* NAME:
*     soc_pb_nif_is_channelized
* TYPE:
*   PROC
* FUNCTION:
*     Gets interface type and channelized indication
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Interface ID.
*   SOC_SAND_OUT uint8                 *is_channelized -
*     TRUE if the interface is channelized.
* REMARKS:
*     None.
* RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_is_channelized(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT uint8 *is_channelized
  );

uint32
  soc_pb_nif_mal_qsgmii_alt_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint8                      enable
  );

uint32
  soc_pb_nif_mal_qsgmii_alt_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT  uint8                     *enable
  );

uint32
  soc_pb_nif_ilkn_actual_nof_lanes_get(
    SOC_SAND_IN  uint32                      ilkn_id,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  );


uint32
  SOC_PB_NIF_STATE_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO *info
  );

uint32
  SOC_PB_NIF_LANES_SWAP_verify(
    SOC_SAND_IN  SOC_PB_NIF_LANES_SWAP *info
  );

uint32
  SOC_PB_NIF_MAL_TOPOLOGY_verify(
    SOC_SAND_IN  SOC_PB_NIF_MAL_TOPOLOGY *info
  );

uint32
  SOC_PB_NIF_MAL_BASIC_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO *info
  );

uint32
  SOC_PB_NIF_PREAMBLE_COMPRESS_verify(
    SOC_SAND_IN  SOC_PB_NIF_PREAMBLE_COMPRESS *info
  );

uint32
  SOC_PB_NIF_IPG_COMPRESS_verify(
    SOC_SAND_IN  SOC_PB_NIF_IPG_COMPRESS *info
  );

uint32
  SOC_PB_NIF_CHANNELIZED_verify(
    SOC_SAND_IN  SOC_PB_NIF_CHANNELIZED *info
  );

uint32
  SOC_PB_NIF_FAULT_RESPONSE_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_FAULT_RESPONSE_INFO *info
  );

uint32
  SOC_PB_NIF_SPAUI_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO *info
  );

uint32
  SOC_PB_NIF_GMII_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO *info
  );

uint32
  SOC_PB_NIF_GMII_STAT_verify(
    SOC_SAND_IN  SOC_PB_NIF_GMII_STAT *info
  );

uint32
  SOC_PB_NIF_ILKN_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO *info
  );

uint32
  SOC_PB_NIF_ILKN_INT_verify(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INT *info
  );

uint32
  SOC_PB_NIF_ILKN_STAT_verify(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_STAT *info
  );

uint32
  SOC_PB_NIF_ILKN_DIAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_DIAG_INFO *info
  );

uint32
  SOC_PB_NIF_STAT_LINK_ACTIVITY_verify(
    SOC_SAND_IN  SOC_PB_NIF_STAT_LINK_ACTIVITY *info
  );

uint32
  SOC_PB_NIF_ACTIVITY_STATUS_verify(
    SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS *info
  );

uint32
  SOC_PB_NIF_ACTIVITY_STATUS_ALL_verify(
    SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS_ALL *info
  );

uint32
  SOC_PB_NIF_LINK_STATUS_verify(
    SOC_SAND_IN  SOC_PB_NIF_LINK_STATUS *info
  );

uint32
  SOC_PB_NIF_SYNCE_CLK_verify(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK *info
  );

uint32
  SOC_PB_NIF_PTP_CLK_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO *info
  );

uint32
  SOC_PB_NIF_PTP_CLK_RST_VAL_verify(
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_RST_VAL *info
  );

uint32
  SOC_PB_NIF_PTP_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO *info
  );

uint32
  SOC_PB_NIF_ELK_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO *info
  );

uint32
  SOC_PB_NIF_FATP_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO *info
  );

uint32
  SOC_PB_NIF_FATP_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE mode,
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_NIF_INCLUDED__*/
#endif
