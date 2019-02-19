/* $Id: petra_fabric.h,v 1.6 Broadcom SDK $
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


#ifndef __SOC_PETRA_FABRIC_INCLUDED__
/* { */
#define __SOC_PETRA_FABRIC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_fabric.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 * MACROS    *
 *************/
/* { */
#define SOC_PETRA_FBC_LINK_IN_MAC(link_id) ((link_id) % SOC_DPP_NOF_FABRIC_LINKS_IN_MAC_PETRAB)
#define SOC_PETRA_FBC_MAC_OF_LINK(link_id) ((link_id) / SOC_DPP_NOF_FABRIC_LINKS_IN_MAC_PETRAB)

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
*     soc_petra_fabric_init
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
  soc_petra_fabric_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_srd_qrtt_reset_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This procedure handles the SerDes configuration of a
*     SerDes quartet attached to the fabric interface. It is
*     called during fabric initialization sequence. Also, in
*     the case that a SerDes quartet is reset (e.g. in order
*     to change the SerDes rate), this API must be called to
*     first shut down all the relevant links (is_oor ==
*     FALSE), and then, after serdes quartet reset, with
*     is_oor == TRUE.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 srd_qrtt_ndx -
*     The SerDes quartet index. Range: 3 and 7 according to
*     Combo configuration, 8 - 14.
*  SOC_SAND_IN  uint8                 is_oor -
*     Is Out-Of-Reset. If FALSE, means In-Reset. Performs the
*     following: If TRUE, configures the SerDes quartet to
*     fabric-compatable configuration. If FALSE, turns down
*     the appropriate links in the Fabric.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_fabric_srd_qrtt_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 srd_qrtt_ndx,
    SOC_SAND_IN  uint8                 is_oor
  );



/*********************************************************************
* NAME:
*     soc_petra_fabric_fc_enable_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure enables/disables flow-control on fabric
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION    direction_ndx -
*     Direction index - RX, TX or Both.
*  SOC_SAND_IN  SOC_PETRA_FABRIC_FC           *info -
*     Struct holds the enable/disable flow control
*     information.
* REMARKS:
*     None.
* RETURNS:
*     The get function is not entirely symmetric to the set function
*     (where only rx, tx or both directions can be defined). The get
*     function returns the both directions.
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_FABRIC_FC            *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fc_enable_verify
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure enables/disables flow-control on fabric
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION    direction_ndx -
*     Direction index - RX, TX or Both.
*  SOC_SAND_IN  SOC_PETRA_FABRIC_FC           *info -
*     Struct holds the enable/disable flow control
*     information.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_FABRIC_FC            *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fc_enable_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure enables/disables flow-control on fabric
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_FABRIC_FC           *info_rx -
*     Struct holds the enable/disable flow control
*     information for rx.
*  SOC_SAND_OUT SOC_PETRA_FABRIC_FC           *info_tx -
*     Struct holds the enable/disable flow control
*     information for tx.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     Not entirely symmetric to the set function (where only rx,
*     tx or both directions can be defined). The get function returns
*     the both directions.
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC            *info_rx,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC            *info_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_cell_format_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure sets fabric links operation mode.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_FABRIC_CELL_FORMAT  *info -
*     Fabric links operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_cell_format_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_CELL_FORMAT  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_cell_format_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure sets fabric links operation mode.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_FABRIC_CELL_FORMAT  *info -
*     Fabric links operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_cell_format_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_CELL_FORMAT  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_coexist_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Sets coexist parameters in order to distribute traffic
*     more evenly over links. Coexist mode is a mode in which
*     the fabric connects a soc_petra device to fap20/21 device.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info -
*     Fabric links coexist configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_coexist_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_coexist_verify
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Sets coexist parameters in order to distribute traffic
*     more evenly over links. Coexist mode is a mode in which
*     the fabric connects a soc_petra device to fap20/21 device.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info -
*     Fabric links coexist configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_coexist_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_coexist_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Sets coexist parameters in order to distribute traffic
*     more evenly over links. Coexist mode is a mode in which
*     the fabric connects a soc_petra device to fap20/21 device.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_FABRIC_COEXIST_INFO *info -
*     Fabric links coexist configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_coexist_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_COEXIST_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_stand_alone_fap_mode_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Configures a FAP to be in stand_alone mode (internal
*     switching). to the presence of FE in the system. If the
*     system is designed to be with ONE fap: - set STANDALONE
*     mode constantly. If the system is designed to be with
*     more than one FAP: - if it starts from ONE fap, set the
*     FAP to STANDALONE mode. - when at least one more FAP is
*     in the system, unset the mode STANDALONE mode. - if the
*     system is with many FAPs, and all FAPs (but one) are
*     removed, set the remaining FAP to be in STANDALONE mode
*     (again, change it when another FAP is in).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint8                 is_single_fap_mode -
*     Indicator. TRUE - Device is in STANDALONE mode. FALSE -
*     Device is NOT in STANDALONE mode.
* REMARKS:
*     1. Stand alone mode represents a current state, and not
*     system HW configuration - i.e. a FAP can be in stand
*     alone mode in a system with FE and multiple FAP-s. 2.
*     See soc_petra_fabric_standalone_fap_mode_detect for
*     standalone mode detection.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_stand_alone_fap_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_single_fap_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_stand_alone_fap_mode_verify
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Configures a FAP to be in stand_alone mode (internal
*     switching). to the presence of FE in the system. If the
*     system is designed to be with ONE fap: - set STANDALONE
*     mode constantly. If the system is designed to be with
*     more than one FAP: - if it starts from ONE fap, set the
*     FAP to STANDALONE mode. - when at least one more FAP is
*     in the system, unset the mode STANDALONE mode. - if the
*     system is with many FAPs, and all FAPs (but one) are
*     removed, set the remaining FAP to be in STANDALONE mode
*     (again, change it when another FAP is in).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint8                 is_single_fap_mode -
*     Indicator. TRUE - Device is in STANDALONE mode. FALSE -
*     Device is NOT in STANDALONE mode.
* REMARKS:
*     1. Stand alone mode represents a current state, and not
*     system HW configuration - i.e. a FAP can be in stand
*     alone mode in a system with FE and multiple FAP-s. 2.
*     See soc_petra_fabric_standalone_fap_mode_detect for
*     standalone mode detection.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_stand_alone_fap_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_single_fap_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_stand_alone_fap_mode_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Configures a FAP to be in stand_alone mode (internal
*     switching). to the presence of FE in the system. If the
*     system is designed to be with ONE fap: - set STANDALONE
*     mode constantly. If the system is designed to be with
*     more than one FAP: - if it starts from ONE fap, set the
*     FAP to STANDALONE mode. - when at least one more FAP is
*     in the system, unset the mode STANDALONE mode. - if the
*     system is with many FAPs, and all FAPs (but one) are
*     removed, set the remaining FAP to be in STANDALONE mode
*     (again, change it when another FAP is in).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT uint8                 *is_single_fap_mode -
*     Indicator. TRUE - Device is in STANDALONE mode. FALSE -
*     Device is NOT in STANDALONE mode.
* REMARKS:
*     1. Stand alone mode represents a current state, and not
*     system HW configuration - i.e. a FAP can be in stand
*     alone mode in a system with FE and multiple FAP-s. 2.
*     See soc_petra_fabric_standalone_fap_mode_detect for
*     standalone mode detection.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_stand_alone_fap_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_single_fap_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_standalone_fap_mode_detect_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This function should be called before calling to to
*     soc_petra_set_single_fap_mode() The value in 'is_standalone'
*     indicates on-line if the device is in single FAP mode.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT uint8                 *is_standalone -
*     Indicator. TRUE - Device should be set to STANDALONE
*     mode. FALSE - Device should NOT be set to STANDALONE
*     mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_standalone_fap_mode_detect_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_standalone
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_connect_mode_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Configure the fabric mode to work in one of the
*     following modes: FE, back to back, mesh or multi stage
*     FE.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE fabric_mode -
*     The fabric connection mode to set.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_connect_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE fabric_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_connect_mode_verify
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Configure the fabric mode to work in one of the
*     following modes: FE, back to back, mesh or multi stage
*     FE.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE fabric_mode -
*     The fabric connection mode to set.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_connect_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE fabric_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_connect_mode_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Configure the fabric mode to work in one of the
*     following modes: FE, back to back, mesh or multi stage
*     FE.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_FABRIC_CONNECT_MODE *fabric_mode -
*     The fabric connection mode to set.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_connect_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_CONNECT_MODE *fabric_mode
  );

/* $Id: petra_fabric.h,v 1.6 Broadcom SDK $
 *	'is_mesh' is TRUE if fabric connection mode
 *  is either MESH or BACK2BACK.
 *  Most configurations are identical for these modes,
 *  and both are often referred to as 'MESH'
 *  'is_single_context' is TRUE if the system is co-exist
 *  with SOC_SAND_FE200/Fap20V/Fap21V.
 *  Otherwise, multiple read contexts are supported
 */
uint32
  soc_petra_fabric_is_mesh(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *is_mesh,
    SOC_SAND_OUT uint8 *is_single_context
  );

/*
 *	'is_fabric' is TRUE if fabric connection mode
 *  is either FE or MULT_STAGE_FE.
 *  Most configurations are identical for these modes,
 *  and both are often referred to as 'Fabric'
 */
uint32
  soc_petra_fabric_is_fabric(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *is_fabric
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fap20_map_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Set/unset fap20 device identification. Cells to fap20
*     devices have different format.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                 other_device_ndx -
*     The device id of the device to set/unset fap20
*     indication for. Range: 0-127 (only these device indexes
*     are allowed).
*  SOC_SAND_IN  uint8                 is_fap20_device -
*     If set, the device with id = other_unit is a fap20
*     device. This determines the cell format for cells with
*     this destination
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_fap20_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_IN  uint8                 is_fap20_device
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fap20_map_verify
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Set/unset fap20 device identification. Cells to fap20
*     devices have different format.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                 other_device_ndx -
*     The device id of the device to set/unset fap20
*     indication for. Range: 0-127 (only these device indexes
*     are allowed).
*  SOC_SAND_IN  uint8                 is_fap20_device -
*     If set, the device with id = other_unit is a fap20
*     device. This determines the cell format for cells with
*     this destination
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_fap20_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_IN  uint8                 is_fap20_device
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fap20_map_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     Set/unset fap20 device identification. Cells to fap20
*     devices have different format.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                 other_device_ndx -
*     The device id of the device to set/unset fap20
*     indication for. Range: 0-127 (only these device indexes
*     are allowed).
*  SOC_SAND_OUT uint8                 *is_fap20_device -
*     If set, the device with id = other_unit is a fap20
*     device. This determines the cell format for cells with
*     this destination
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_fap20_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_OUT uint8                 *is_fap20_device
  );

/*
 *  Returns TRUE if and only if the Fabric link
 *  with index 'link_ndx' is towards the fabric.
 *  This depends on the index, and the COMBO
 *  quartet configuration (whether dedicated to the Fabric or the NIF)
 */
uint8
  soc_petra_fabric_is_fabric_link(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 link_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_topology_status_connectivity_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Retrieve the connectivity map from the device.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  int                 link_index_min -
*     The first link index which this API reterives the info
*  SOC_SAND_IN  int                 link_index_max -
*     The last link index which this API reterives the info
*  SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map -
*     The connectivity map
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_topology_status_connectivity_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    link_index_min,
    SOC_SAND_IN  int                    link_index_max,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_topology_status_connectivity_print_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Print the connectivity ma.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map -
*     The connectivity map
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_topology_status_connectivity_print_unsafe(
    SOC_SAND_IN  SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_links_status_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Get Real-time lower level indications and errors of the
*     links
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 read_crc_counter -
*     If TRUE, the link CRC error counter is read.
*     Since the CRC counters are clear-on-read, if the application
*     relies on periodically gathering the counter information,
*     the CRC counters should not be read by the API.
*     In this case, only the CRC error interrupt indication is read,
*     and identifies whether any CRC errors are present on the link.
*  SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_ALL    *links_status -
*     Real time links information.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fabric_links_status_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 read_crc_counter,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_ALL    *links_status
  );

/*********************************************************************
* NAME:
*     soc_petra_link_on_off_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO     *info -
*     Fabric link on/off state.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_link_on_off_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO     *info
  );

/*********************************************************************
* NAME:
*     soc_petra_link_on_off_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO     *info -
*     Fabric link on/off state.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_link_on_off_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO     *info
  );

/*********************************************************************
* NAME:
*     arad_fabric_nof_links_get
* TYPE:
*   PROC
* FUNCTION:
*     get nof fabric links
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT  int     *nof_links -
*     nof fabric links
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_fabric_nof_links_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT int                    *nof_links
  );

/*********************************************************************
* NAME:
*     soc_petra_link_on_off_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  SOC_SAND_OUT SOC_PETRA_LINK_STATE_INFO     *info -
*     Fabric link on/off state.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_link_on_off_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_OUT SOC_PETRA_LINK_STATE_INFO     *info
  );

uint8
  soc_petra_is_fabric_quartet(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 qrtt_ndx
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_FABRIC_INCLUDED__*/
#endif
