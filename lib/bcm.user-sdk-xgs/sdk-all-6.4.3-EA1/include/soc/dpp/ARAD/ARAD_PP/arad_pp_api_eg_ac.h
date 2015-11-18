
/* $Id: arad_pp_api_eg_ac.h,v 1.5 Broadcom SDK $
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

#ifndef __ARAD_PP_API_EG_AC_INCLUDED__
/* { */
#define __ARAD_PP_API_EG_AC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_eg_ac.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

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

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef SOC_PPC_EG_AC_VBP_KEY                                  ARAD_PP_EG_AC_VBP_KEY;
typedef SOC_PPC_EG_AC_CEP_PORT_KEY                             ARAD_PP_EG_AC_CEP_PORT_KEY;
typedef SOC_PPC_EG_VLAN_EDIT_CEP_INFO                          ARAD_PP_EG_VLAN_EDIT_CEP_INFO;
typedef SOC_PPC_EG_VLAN_EDIT_VLAN_INFO                         ARAD_PP_EG_VLAN_EDIT_VLAN_INFO;
typedef SOC_PPC_EG_AC_VLAN_EDIT_INFO                           ARAD_PP_EG_AC_VLAN_EDIT_INFO;
typedef SOC_PPC_EG_AC_INFO                                     ARAD_PP_EG_AC_INFO;
typedef SOC_PPC_EG_AC_MP_INFO                                  ARAD_PP_EG_AC_MP_INFO;

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
 *   arad_pp_eg_ac_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the editing information for packets
 *   associated with AC.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx -
 *     AC ID. Should be equal to the LIF ID in the incoming LIF
 *     module.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 * REMARKS:
 *   - Packets associated with AC by the ingress are
 *   manipulated according to this setting. For packets
 *   without out-AC-ID associated, see
 *   soc_ppd_eg_ac_port_vsi_info()_set and
 *   soc_ppd_eg_ac_port_cvid_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_ac_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  ARAD_PP_EG_AC_INFO                          *ac_info
  );

/*********************************************************************
*     Gets the configuration set by the "arad_pp_eg_ac_info_set"
 *     API.
 *     Refer to "arad_pp_eg_ac_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_eg_ac_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT ARAD_PP_EG_AC_INFO                          *ac_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_ac_mp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function is used to define a Maintenance Point (MP)
 *   on an outgoing Attachment-Circuit (port x VSI) and
 *   MD-level, and to determine the action to perform. If the
 *   MP is one of the 4K accelerated MEPs, the function
 *   configures the related OAMP databases and associates the
 *   AC and MD-Level with a user-provided handle. This handle
 *   is later used by user to access OAMP database for this
 *   MEP.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_AC_ID                        out_ac_ndx -
 *     AC ID. Should be equal to the LIF ID in the incoming LIF
 *     module.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_MP_INFO                *info -
 *     MP configuration information.
 * REMARKS:
 *   Should only be used if the MP is down MEP or MIP.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  ARAD_PP_EG_AC_MP_INFO                *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_eg_ac_mp_info_set" API.
 *     Refer to "arad_pp_eg_ac_mp_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_eg_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT ARAD_PP_EG_AC_MP_INFO                *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_ac_port_vsi_info_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the editing information for packets NOT
 *   associated with AC and to be transmitted from VBP port
 *   (not CEP port).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx -
 *     AC ID, for T20E: this is not relevant and may be
 *     ignored. For Arad-B this is the out-AC that the port x
 *     VSI is mapped to.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                       *vbp_key -
 *     The key (port (vlan-domain) x VSI) to set editing
 *     information to. In Arad this is also is mapped to the
 *     give AC.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Operation may
 *     fail upon unavailable resources (Exact Match). In T20E
 *     this operation always success.
 * REMARKS:
 *   - Packets NOT associated with AC by the ingress and to
 *   be transmitted out through VBP port are manipulated
 *   according to this setting.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_ac_port_vsi_info_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_IN  ARAD_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_ac_port_vsi_info_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Removes editing information of packets NOT
 *   associated with AC and to be transmitted from VBP port
 *   (not CEP port).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                       *vbp_key -
 *     The key (port (vlan-domain) x VSI) to set editing
 *     information to. In Arad this is also is mapped to the
 *     give AC.
 *   SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac -
 *     AC ID associated with the Vbp_key, for T20E: this is not
 *     relevant and may be ignored. For Arad-B this is the
 *     out-AC that the port x VSI is mapped to.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_ac_port_vsi_info_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_ac_port_vsi_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the editing information for packets NOT
 *   associated with AC and to be transmitted from VBP port
 *   (not CEP port).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                       *vbp_key -
 *     The key (port(vlan-domain)x VSI) to set editing
 *     information to. In Arad this is also is mapped to the
 *     give AC.
 *   SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac -
 *     AC ID associated with the Vbp_key, for T20E: this is not
 *     relevant and may be ignored. For Arad-B this is the
 *     out-AC that the port x VSI is mapped to.
 *   SOC_SAND_OUT ARAD_PP_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: The entry was found, 'ac_info' is validFALSE: The
 *     entry was not found, 'ac_info' is invalid
 * REMARKS:
 *   - Packets NOT associated with AC by the ingress and to
 *   be transmitted out through VBP port are manipulated
 *   according to this setting.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_ac_port_vsi_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac,
    SOC_SAND_OUT ARAD_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                 *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_ac_port_cvid_info_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the editing information for packets NOT
 *   associated with AC and to be transmitted from CEP port
 *   (not VBP port).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx -
 *     AC ID, for T20E: this is not relevant and may be
 *     ignored. For Arad-B this is the out-AC that the port x
 *     CVID is mapped to.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key -
 *     The key (port(vlan-domain) x CVID) to set editing
 *     information to. In Arad this is also is mapped to the
 *     give AC.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Operation may
 *     fail upon unavailable resources (Exact Match). In T20E
 *     this operation always success.
 * REMARKS:
 *   - Packets NOT associated with AC by the ingress and to
 *   be transmitted out through CEP port are manipulated
 *   according to this setting.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_ac_port_cvid_info_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  ARAD_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_ac_port_cvid_info_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Removes editing information of packets NOT
 *   associated with AC and to be transmitted from CEP port
 *   (not VBP port).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key -
 *     The key (port(vlan-domain) x CVID) to set editing
 *     information to. In Arad this is also is mapped to the
 *     give AC.
 *   SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac -
 *     AC ID associated with the Vbp_key, for T20E: this is not
 *     relevant and may be ignored. For Arad-B this is the
 *     out-AC that the port x CVID is mapped to.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_ac_port_cvid_info_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_ac_port_cvid_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the editing information for packets NOT
 *   associated with AC and to be transmitted from CEP port
 *   (not VBP port).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key -
 *     The key (port(vlan-domain) x CVID) to set editing
 *     information to. In Arad this is also is mapped to the
 *     give AC.
 *   SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac -
 *     AC ID associated with the Vbp_key, for T20E: this is not
 *     relevant and may be ignored. For Arad-B this is the
 *     out-AC that the port x CVID is mapped to.
 *   SOC_SAND_OUT ARAD_PP_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: The entry was found, 'ac_info' is validFALSE: The
 *     entry was not found, 'ac_info' is invalid
 * REMARKS:
 *   - Packets NOT associated with AC by the ingress and to
 *   be transmitted out through CEP port are manipulated
 *   according to this setting.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_ac_port_cvid_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac,
    SOC_SAND_OUT ARAD_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                 *found
  );
void
  ARAD_PP_EG_AC_VBP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_VBP_KEY *info
  );

void
  ARAD_PP_EG_AC_CEP_PORT_KEY_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_CEP_PORT_KEY *info
  );

void
  ARAD_PP_EG_VLAN_EDIT_CEP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_VLAN_EDIT_CEP_INFO *info
  );

void
  ARAD_PP_EG_VLAN_EDIT_VLAN_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_VLAN_EDIT_VLAN_INFO *info
  );

void
  ARAD_PP_EG_AC_VLAN_EDIT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_VLAN_EDIT_INFO *info
  );

void
  ARAD_PP_EG_AC_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_INFO *info
  );

void
  ARAD_PP_EG_AC_MP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_MP_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

void
  ARAD_PP_EG_AC_VBP_KEY_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY *info
  );

void
  ARAD_PP_EG_AC_CEP_PORT_KEY_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY *info
  );

void
  ARAD_PP_EG_VLAN_EDIT_CEP_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_VLAN_EDIT_CEP_INFO *info
  );

void
  ARAD_PP_EG_VLAN_EDIT_VLAN_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_VLAN_EDIT_VLAN_INFO *info
  );

void
  ARAD_PP_EG_AC_VLAN_EDIT_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_VLAN_EDIT_INFO *info
  );

void
  ARAD_PP_EG_AC_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_INFO *info
  );

void
  ARAD_PP_EG_AC_MP_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_MP_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_EG_AC_INCLUDED__*/
#endif



