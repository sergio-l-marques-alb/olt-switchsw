/* $Id: ppd_api_eg_ac.h,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_eg_ac.h
*
* MODULE PREFIX:  soc_ppd_eg
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

#ifndef __SOC_PPD_API_EG_AC_INCLUDED__
/* { */
#define __SOC_PPD_API_EG_AC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_eg_ac.h>

#include <soc/dpp/PPD/ppd_api_general.h>

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

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PPD_EG_AC_INFO_SET = SOC_PPD_PROC_DESC_BASE_EG_AC_FIRST,
  SOC_PPD_EG_AC_INFO_SET_PRINT,
  SOC_PPD_EG_AC_INFO_GET,
  SOC_PPD_EG_AC_INFO_GET_PRINT,
  SOC_PPD_EG_AC_MP_INFO_SET,
  SOC_PPD_EG_AC_MP_INFO_SET_PRINT,
  SOC_PPD_EG_AC_MP_INFO_GET,
  SOC_PPD_EG_AC_MP_INFO_GET_PRINT,
  SOC_PPD_EG_AC_PORT_VSI_INFO_ADD,
  SOC_PPD_EG_AC_PORT_VSI_INFO_ADD_PRINT,
  SOC_PPD_EG_AC_PORT_VSI_INFO_REMOVE,
  SOC_PPD_EG_AC_PORT_VSI_INFO_REMOVE_PRINT,
  SOC_PPD_EG_AC_PORT_VSI_INFO_GET,
  SOC_PPD_EG_AC_PORT_VSI_INFO_GET_PRINT,
  SOC_PPD_EG_AC_PORT_CVID_INFO_ADD,
  SOC_PPD_EG_AC_PORT_CVID_INFO_ADD_PRINT,
  SOC_PPD_EG_AC_PORT_CVID_INFO_REMOVE,
  SOC_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PRINT,
  SOC_PPD_EG_AC_PORT_CVID_INFO_GET,
  SOC_PPD_EG_AC_PORT_CVID_INFO_GET_PRINT,
  SOC_PPD_EG_AC_PORT_CVID_MAP,
  SOC_PPD_EG_AC_PORT_CVID_MAP_PRINT,
  SOC_PPD_EG_AC_PORT_VSI_MAP,
  SOC_PPD_EG_AC_PORT_VSI_MAP_PRINT,
  SOC_PPD_EG_AC_GET_PROCS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PPD_EG_AC_PROCEDURE_DESC_LAST
} SOC_PPD_EG_AC_PROCEDURE_DESC;

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
 *   soc_ppd_eg_ac_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the editing information for packets
 *   associated with AC.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx -
 *     AC ID. Should be equal to the LIF ID in the incoming LIF
 *     module.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info -
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
  soc_ppd_eg_ac_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_ppd_eg_ac_info_set"
 *     API.
 *     Refer to "soc_ppd_eg_ac_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_eg_ac_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_ac_mp_info_set
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
 *   SOC_SAND_IN  SOC_PPC_AC_ID                          out_ac_ndx -
 *     AC ID. Should be equal to the LIF ID in the incoming LIF
 *     module.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO                  *info -
 *     MP configuration information.
 * REMARKS:
 *   Should only be used if the MP is down MEP or MIP.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                          out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO                  *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_ac_mp_info_set" API.
 *     Refer to "soc_ppd_eg_ac_mp_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_eg_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                          out_ac_ndx,
    SOC_SAND_OUT SOC_PPC_EG_AC_MP_INFO                  *info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_ac_port_vsi_info_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the editing information for packets NOT
 *   associated with AC and to be transmitted from VBP port
 *   (not CEP port).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx -
 *     AC ID, for T20E: this is not relevant and may be
 *     ignored. For Soc_petra-B this is the out-AC that the port x
 *     VSI is mapped to.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key -
 *     The key (port (vlan-domain) x VSI) to set editing
 *     information to. In Soc_petra this is also is mapped to the
 *     give AC.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
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
  soc_ppd_eg_ac_port_vsi_info_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_ac_port_vsi_info_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Removes editing information of packets NOT
 *   associated with AC and to be transmitted from VBP port
 *   (not CEP port).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key -
 *     The key (port (vlan-domain) x VSI) to set editing
 *     information to. In Soc_petra this is also is mapped to the
 *     give AC.
 *   SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac -
 *     AC ID associated with the Vbp_key, for T20E: this is not
 *     relevant and may be ignored. For Soc_petra-B this is the
 *     out-AC that the port x VSI is mapped to.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_vsi_info_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_ac_port_vsi_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the editing information for packets NOT
 *   associated with AC and to be transmitted from VBP port
 *   (not CEP port).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key -
 *     The key (port(vlan-domain)x VSI) to set editing
 *     information to. In Soc_petra this is also is mapped to the
 *     give AC.
 *   SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac -
 *     AC ID associated with the Vbp_key, for T20E: this is not
 *     relevant and may be ignored. For Soc_petra-B this is the
 *     out-AC that the port x VSI is mapped to.
 *   SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 *   SOC_SAND_OUT uint8                               *found -
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
  soc_ppd_eg_ac_port_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                               *found
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_ac_port_cvid_info_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the editing information for packets NOT
 *   associated with AC and to be transmitted from CEP port
 *   (not VBP port).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx -
 *     AC ID, for T20E: this is not relevant and may be
 *     ignored. For Soc_petra-B this is the out-AC that the port x
 *     CVID is mapped to.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key -
 *     The key (port(vlan-domain) x CVID) to set editing
 *     information to. In Soc_petra this is also is mapped to the
 *     give AC.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
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
  soc_ppd_eg_ac_port_cvid_info_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_ac_port_cvid_info_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Removes editing information of packets NOT
 *   associated with AC and to be transmitted from CEP port
 *   (not VBP port).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key -
 *     The key (port(vlan-domain) x CVID) to set editing
 *     information to. In Soc_petra this is also is mapped to the
 *     give AC.
 *   SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac -
 *     AC ID associated with the Vbp_key, for T20E: this is not
 *     relevant and may be ignored. For Soc_petra-B this is the
 *     out-AC that the port x CVID is mapped to.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_cvid_info_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_ac_port_cvid_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the editing information for packets NOT
 *   associated with AC and to be transmitted from CEP port
 *   (not VBP port).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key -
 *     The key (port(vlan-domain) x CVID) to set editing
 *     information to. In Soc_petra this is also is mapped to the
 *     give AC.
 *   SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac -
 *     AC ID associated with the Vbp_key, for T20E: this is not
 *     relevant and may be ignored. For Soc_petra-B this is the
 *     out-AC that the port x CVID is mapped to.
 *   SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info -
 *     Information according to which to edit the
 *     packet.
 *   SOC_SAND_OUT uint8                               *found -
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
  soc_ppd_eg_ac_port_cvid_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                               *found
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_EG_AC_INCLUDED__*/
#endif

