/* $Id: petra_api_fabric.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_api_fabric.h
*
* MODULE PREFIX:  soc_petra_fabric
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


#ifndef __SOC_PETRA_API_FABRIC_INCLUDED__
/* { */
#define __SOC_PETRA_API_FABRIC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_fabric.h>
#include <soc/dpp/Petra/petra_api_serdes.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximal number of devices in coexist mode               */

/*     Coexist mode devices - each entry identifies 2
 *     consecutive devices                                     */
#define  SOC_PETRA_FABRIC_NOF_COEXIST_DEV_ENTRIES (SOC_TMC_FABRIC_NOF_COEXIST_DEV_ENTRIES)

/*     Maximal number of fap20v devices in the system          */
#define  SOC_PETRA_FABRIC_NOF_FAP20_DEVICES (SOC_TMC_FABRIC_NOF_FAP20_DEVICES)

/*     Maximal number of devices in mesh mode                  */

#define SOC_PETRA_NOF_FABRIC_REGULAR_LINKS (28)
#define SOC_PETRA_NOF_FABRIC_LINKS_IN_COMBO (4)
#define SOC_PETRA_FIRST_FABRIC_COMBO_PORT(combo) \
    (FABRIC_LOGICAL_PORT_BASE(unit) \
        + SOC_PETRA_NOF_FABRIC_REGULAR_LINKS \
        + SOC_PETRA_NOF_FABRIC_LINKS_IN_COMBO * (combo))

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

#define SOC_PETRA_LINK_ERR_TYPE_CRC                            SOC_TMC_LINK_ERR_TYPE_CRC
#define SOC_PETRA_LINK_ERR_TYPE_SIZE                           SOC_TMC_LINK_ERR_TYPE_SIZE
#define SOC_PETRA_LINK_ERR_TYPE_MISALIGN                       SOC_TMC_LINK_ERR_TYPE_MISALIGN
#define SOC_PETRA_LINK_ERR_TYPE_CODE_GROUP                     SOC_TMC_LINK_ERR_TYPE_CODE_GROUP
typedef SOC_TMC_LINK_ERR_TYPE                                  SOC_PETRA_LINK_ERR_TYPE;

#define SOC_PETRA_LINK_INDICATE_TYPE_SIG_LOCK                  SOC_TMC_LINK_INDICATE_TYPE_SIG_LOCK
#define SOC_PETRA_LINK_INDICATE_TYPE_ACCEPT_CELL               SOC_TMC_LINK_INDICATE_TYPE_ACCEPT_CELL
#define SOC_PETRA_LINK_INDICATE_INTRNL_FIXED                   SOC_TMC_LINK_INDICATE_INTRNL_FIXED
typedef SOC_TMC_LINK_INDICATE_TYPE                             SOC_PETRA_LINK_INDICATE_TYPE;

#define SOC_PETRA_FABRIC_CONNECT_MODE_FE                       SOC_TMC_FABRIC_CONNECT_MODE_FE
#define SOC_PETRA_FABRIC_CONNECT_MODE_BACK2BACK                SOC_TMC_FABRIC_CONNECT_MODE_BACK2BACK
#define SOC_PETRA_FABRIC_CONNECT_MODE_MESH                     SOC_TMC_FABRIC_CONNECT_MODE_MESH
#define SOC_PETRA_FABRIC_CONNECT_MODE_MULT_STAGE_FE            SOC_TMC_FABRIC_CONNECT_MODE_MULT_STAGE_FE
#define SOC_PETRA_FABRIC_CONNECT_MODE_SINGLE_FAP               SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP
#define SOC_PETRA_FABRIC_NOF_CONNECT_MODES                     SOC_TMC_FABRIC_NOF_CONNECT_MODES
typedef SOC_TMC_FABRIC_CONNECT_MODE                            SOC_PETRA_FABRIC_CONNECT_MODE;

#define SOC_PETRA_LINK_STATE_ON                                SOC_TMC_LINK_STATE_ON
#define SOC_PETRA_LINK_STATE_OFF                               SOC_TMC_LINK_STATE_OFF
#define SOC_PETRA_LINK_NOF_STATES                              SOC_TMC_LINK_NOF_STATES
typedef SOC_TMC_LINK_STATE                                     SOC_PETRA_LINK_STATE;

typedef SOC_TMC_FABRIC_LINE_CODING                             SOC_PETRA_FABRIC_LINE_CODING;

typedef SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO             SOC_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO;
typedef SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR                 SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR;
typedef SOC_TMC_FABRIC_CELL_FORMAT                             SOC_PETRA_FABRIC_CELL_FORMAT;
typedef SOC_TMC_FABRIC_FC                                      SOC_PETRA_FABRIC_FC;
typedef SOC_TMC_FABRIC_COEXIST_INFO                            SOC_PETRA_FABRIC_COEXIST_INFO;
typedef SOC_TMC_FABRIC_LINKS_STATUS_SINGLE                     SOC_PETRA_FABRIC_LINKS_STATUS_SINGLE;
typedef SOC_TMC_FABRIC_LINKS_STATUS_ALL                        SOC_PETRA_FABRIC_LINKS_STATUS_ALL;
typedef SOC_TMC_LINK_STATE_INFO                                SOC_PETRA_LINK_STATE_INFO;

#define SOC_PETRA_FAR_NOF_DEVICE_TYPES                         SOC_TMC_FAR_NOF_DEVICE_TYPES

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
*     soc_petra_fabric_srd_qrtt_reset
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
  soc_petra_fabric_srd_qrtt_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 srd_qrtt_ndx,
    SOC_SAND_IN  uint8                 is_oor
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fc_enable_set
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
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     The get function is not entirely symmetric to the set function
*     (where only rx, tx or both directions can be defined). The get
*     function returns the both directions.
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_FABRIC_FC            *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fc_enable_get
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure enables/disables flow-control on fabric
*     links.
* INPUT:
*  SOC_SAND_IN  int                     unit -
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
  soc_petra_fabric_fc_enable_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC           *info_rx,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC           *info_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_cell_format_get
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
  soc_petra_fabric_cell_format_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_CELL_FORMAT  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_coexist_set
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
  soc_petra_fabric_coexist_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_coexist_get
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
  soc_petra_fabric_coexist_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_COEXIST_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_stand_alone_fap_mode_get
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
  soc_petra_fabric_stand_alone_fap_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_single_fap_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_connect_mode_set
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
  soc_petra_fabric_connect_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE fabric_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_connect_mode_get
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
  soc_petra_fabric_connect_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_CONNECT_MODE *fabric_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fap20_map_set
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
  soc_petra_fabric_fap20_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_IN  uint8                 is_fap20_device
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_fap20_map_get
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
  soc_petra_fabric_fap20_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_OUT uint8                 *is_fap20_device
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_topology_status_connectivity_get
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
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     The far_link_id (link number at the connected device) indication
*     is modulo 64
*********************************************************************/
uint32
  soc_petra_fabric_topology_status_connectivity_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    link_index_min,
    SOC_SAND_IN  int                    link_index_max,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  );

/*********************************************************************
* NAME:
*     soc_petra_fabric_links_status_get
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
  soc_petra_fabric_links_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 read_crc_counter,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_ALL    *links_status
  );

/*********************************************************************
* NAME:
*     soc_petra_link_on_off_set
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
  soc_petra_link_on_off_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO     *info
  );

/*********************************************************************
* NAME:
*     soc_petra_link_on_off_get
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
  soc_petra_link_on_off_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_OUT SOC_PETRA_LINK_STATE_INFO     *info
  );

void
  soc_petra_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info
  );

void
  soc_petra_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *info
  );

void
  soc_petra_PETRA_FABRIC_CELL_FORMAT_clear(
    SOC_SAND_OUT SOC_PETRA_FABRIC_CELL_FORMAT *info
  );

void
  soc_petra_PETRA_FABRIC_FC_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC *info
  );

void
  soc_petra_PETRA_FABRIC_COEXIST_INFO_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_COEXIST_INFO *info
  );

void
  soc_petra_PETRA_FABRIC_LINKS_STATUS_SINGLE_clear(
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_SINGLE *info
  );

void
  soc_petra_PETRA_FABRIC_LINKS_STATUS_ALL_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_ALL *info
  );

void
  soc_petra_PETRA_LINK_STATE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_LINK_STATE_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

/*********************************************************************
* NAME:
*     soc_petra_fabric_topology_status_connectivity_print
* TYPE:
*   PROC
* FUNCTION:
*     Print the connectivity ma.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map -
*     The connectivity map
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     The far_link_id (link number at the connected device) indication
*     is modulo 64
*********************************************************************/
uint32
  soc_petra_fabric_topology_status_connectivity_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  );

const char*
  soc_petra_PETRA_LINK_ERR_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_LINK_ERR_TYPE enum_val
  );



const char*
  soc_petra_PETRA_LINK_INDICATE_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_LINK_INDICATE_TYPE enum_val
  );



const char*
  soc_petra_PETRA_FABRIC_CONNECT_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_FABRIC_CONNECT_MODE enum_val
  );



const char*
  soc_petra_PETRA_LINK_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_LINK_STATE enum_val
  );



void
  soc_petra_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_print(
    SOC_SAND_IN SOC_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info,
    SOC_SAND_IN uint8                         table_format
  );



void
  soc_petra_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *info,
    SOC_SAND_IN uint8                     table_format
  );



void
  soc_petra_PETRA_FABRIC_CELL_FORMAT_print(
    SOC_SAND_IN SOC_PETRA_FABRIC_CELL_FORMAT *info
  );



void
  soc_petra_PETRA_FABRIC_FC_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_PETRA_FABRIC_FC *info
  );



void
  soc_petra_PETRA_FABRIC_COEXIST_INFO_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_PETRA_FABRIC_COEXIST_INFO *info
  );


void
  soc_petra_PETRA_FABRIC_LINKS_STATUS_ALL_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_PETRA_FABRIC_LINKS_STATUS_ALL *info,
    SOC_SAND_IN SOC_SAND_PRINT_FLAVORS     print_flavour
  );

void
  soc_petra_PETRA_LINK_STATE_INFO_print(
    SOC_SAND_IN SOC_PETRA_LINK_STATE_INFO *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */


/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_FABRIC_INCLUDED__*/
#endif
