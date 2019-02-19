/* $Id: ppd_api_vsi.h,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_vsi.h
*
* MODULE PREFIX:  soc_ppd_vsi
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

#ifndef __SOC_PPD_API_VSI_INCLUDED__
/* { */
#define __SOC_PPD_API_VSI_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_vsi.h>

#include <soc/dpp/PPD/ppd_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PPD_VSI_FID_IS_VSID                              (SOC_PPC_VSI_FID_IS_VSID)

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
  SOC_PPD_VSI_MAP_ADD = SOC_PPD_PROC_DESC_BASE_VSI_FIRST,
  SOC_PPD_VSI_MAP_ADD_PRINT,
  SOC_PPD_VSI_MAP_REMOVE,
  SOC_PPD_VSI_MAP_REMOVE_PRINT,
  SOC_PPD_VSI_DEFAULT_FRWRD_INFO_SET,
  SOC_PPD_VSI_DEFAULT_FRWRD_INFO_SET_PRINT,
  SOC_PPD_VSI_DEFAULT_FRWRD_INFO_GET,
  SOC_PPD_VSI_DEFAULT_FRWRD_INFO_GET_PRINT,
  SOC_PPD_VSI_INFO_SET,
  SOC_PPD_VSI_INFO_SET_PRINT,
  SOC_PPD_VSI_INFO_GET,
  SOC_PPD_VSI_INFO_GET_PRINT,
  SOC_PPD_VSI_GET_PROCS_PTR,
  SOC_PPD_VSI_EGRESS_PROFILE_SET,
  SOC_PPD_VSI_EGRESS_PROFILE_SET_PRINT,
  SOC_PPD_VSI_EGRESS_PROFILE_GET,
  SOC_PPD_VSI_EGRESS_PROFILE_GET_PRINT,
  /*
   * } Auto generated. Do not edit previous section.
   */
  /*
   * Last element. Do no touch.
   */
  SOC_PPD_VSI_PROCEDURE_DESC_LAST
} SOC_PPD_VSI_PROCEDURE_DESC;

typedef SOC_PPC_VSI_INFO                                       SOC_PPD_VSI_INFO;
typedef SOC_PPC_VSI_DEFAULT_FRWRD_KEY                          SOC_PPD_VSI_DEFAULT_FRWRD_KEY;

typedef SOC_PPC_VSI_L2CP_KEY                                   SOC_PPD_VSI_L2CP_KEY;
typedef SOC_PPC_VSI_L2CP_HANDLE_TYPE                           SOC_PPD_VSI_L2CP_HANDLE_TYPE;

#define SOC_PPD_VSI_L2CP_HANDLE_TYPE_NORMAL					SOC_PPC_VSI_L2CP_HANDLE_TYPE_NORMAL
#define SOC_PPD_VSI_L2CP_HANDLE_TYPE_TUNNEL					SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL
#define SOC_PPD_VSI_L2CP_HANDLE_TYPE_PEER					SOC_PPC_VSI_L2CP_HANDLE_TYPE_PEER
#define SOC_PPD_VSI_L2CP_HANDLE_TYPE_DROP					SOC_PPC_VSI_L2CP_HANDLE_TYPE_DROP
#define SOC_PPD_VSI_L2CP_HANDLE_TYPES					    SOC_PPC_VSI_L2CP_HANDLE_TYPES

#define SOC_PPD_VSI_LOW_CNFG_VALUE                          (1)

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
 *   soc_ppd_vsi_map_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map VSI to sys-VSI and system VSI to egress VSI
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx -
 *     Local VSID. Soc_petra-B Range: 0-16K
 *   SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid -
 *     System VSID. Range: 0-64K
 *   SOC_SAND_IN  SOC_PPD_VSI_ID                              eg_local_vsid -
 *     Local VSID. During the egress processing, packets that
 *     arrive from the fabric with 'sys_vsid' are mapped to
 *     'eg_local_vsid'. Typically, 'eg_local_vsid' equals
 *     'local_vsi_ndx'
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     egress VSI EMSAND_FAILURE_REMOVE_ENTRY_FIRST: The local
 *     VSI is already mapped to other system VSI. The user is
 *     expected to disable the local VSI before remapping.
 * REMARKS:
 *   - In other PPD APIs, VSID stand for system VSI, unless
 *   the user uses the MACRO SOC_PPD_USE_LOCAL_VSID(local_vsid).-
 *   Set the System VSID in the VSI table, and add Egress
 *   mapping from the system VSI to local VSI, in the VSI
 *   Exact Match.- Unless specified otherwise, in the PPD the
 *   VSID is always the system VSID. When the device is
 *   Soc_petra-B, the PPD translates the Sys-VSID to local-VSID
 *   before calling the Soc_petra-B API.- The local VSI is also
 *   mapped to the MACT FID. Therefore, the VSI mapping
 *   cannot be updated on the fly, and needs to follow MACT
 *   flush / transplant.- T20E: System-VSIs are equal to
 *   local-VSIs. The user is expected to call this function
 *   with (vsi_ndx == sys_vsid).- Soc_petra-B: Local VSIs 4K-16K
 *   are mapped to system-VSIDs 4K-16K. The user expected to
 *   call this function with (vsi_ndx == sys_vsid).- The
 *   local VSID is the index to the device local databases.-
 *   For VSIs that have an interface to the router,Sys-RIF =
 *   Sys-VSI = VSI. The user is expected to map those VSIs in
 *   all the devices in the system.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_vsi_map_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              eg_local_vsid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_vsi_map_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove mapping of local VSI to system VSI, and system
 *   VSI to local VSI.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx -
 *     Local VSID. Soc_petra-B Range: 0-16K
 *   SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid -
 *     System VSID. Range: 0-64K.
 * REMARKS:
 *   - The user is expected to commit MACT flushing, and
 *   remove logical interfaces that attached to the removed
 *   VSID- Ignored when called for T20E device
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_vsi_map_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid
  );

/*********************************************************************
* NAME:
 *   soc_ppd_vsi_default_frwrd_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the action profile (forwarding/snooping) to assign
 *   for packets upon failure lookup in the MACT (MACT
 *   default forwarding).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key -
 *     Set of parameters that default forwarding may be
 *     assigned to. Includes DA-type (UC/MC/BC) orientation
 *     (Hub/Spoke) and profile.
 *   SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile -
 *     Trap information including snoop/forwarding action.
 *     trap_code range: SOC_PPD_TRAP_CODE_UNKNOWN_DA_0-SOC_PPD_TRAP_CODE_UNKNOWN_DA_7
 * REMARKS:
 *   - Relevant only for Soc_petra-B. Used to set default
 *   forwarding for VSI.- By soc_ppd_vsi_info_set() the user
 *   supplies the default forwarding for the VSI; however, in
 *   soc_petra the user has to set the defaults using this API,
 *   and the destination set by soc_ppd_vsi_info_set must
 *   correspond to one of the action pointers configured
 *   here.- T20E: The default destination may be set fully
 *   for each VSI or LIF. Therefore, there is no need to call
 *   this function. The PPD returns an error when called over
 *   the T20E device.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_vsi_default_frwrd_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_vsi_default_frwrd_info_set" API.
 *     Refer to "soc_ppd_vsi_default_frwrd_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_vsi_default_frwrd_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_OUT SOC_PPD_ACTION_PROFILE                      *action_profile
  );

/*********************************************************************
* NAME:
 *   soc_ppd_vsi_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Virtual Switch Instance information. After
 *   setting the VSI, the user may attach L2 Logical
 *   Interfaces to it: ACs; PWEs
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx -
 *     System VSID. Range: 0-64K
 *   SOC_SAND_IN  SOC_PPD_VSI_INFO                            *vsi_info -
 *     VSI attributes
 * REMARKS:
 *   - Default forwarding destination:T20E: The destination
 *   is fully configurablePetra-B: The destination must
 *   correspond to one of the action pointers configured by
 *   soc_ppd_frwrd_mact_vsi_default_info_set()- Soc_petra-B Flooding:
 *   When the flooding multicast ID mapping from the local
 *   VSI is insufficient, the user may either set the
 *   destination as FEC ID with multicast destination, or
 *   utilize the ingress multicast table to remap the MID.-
 *   Soc_petra-B: When the local VSI is > 4K: The EEI cannot be
 *   used as I-SID; Topology ID. Enable-My-MAC and Enable
 *   routing have to be negated. The FID is the VSID.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_vsi_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PPD_VSI_INFO                            *vsi_info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_ppd_vsi_info_set"
 *     API.
 *     Refer to "soc_ppd_vsi_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_PPD_VSI_INFO                            *vsi_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_vsi_egress_mtu_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mtu val to vsi.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               vsi_profile_ndx -
 *     vsi profile index. Range: 1-3
 *   SOC_SAND_IN  SOC_SAND_IN  SOC_PPD_MTU               mtu_val -
 *     mtu value to set into the profile.
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
********************************************************************/
uint32 soc_ppd_vsi_egress_mtu_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_vsi_egress_vsi_set" API.
 *     Refer to "soc_ppd_vsi_egress_vsi_set" API for details.
*********************************************************************/
uint32
  soc_ppd_vsi_egress_mtu_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint8              is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32             vsi_profile_ndx,
    SOC_SAND_OUT uint32             *mtu_val
  );

/*********************************************************************
 * NAME:
 *   soc_ppd_vsi_l2cp_trap_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *     Sets Trap information for Layer 2 control protocol
 *     frames. Packet is an MEF layer 2 control protocol
 *     service frame When DA matches 01-80-c2-00-00-XX where XX
 * = 8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                 *l2cp_key -
 *     profile and da key.
 *   SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE         handle_type -
 *     type (trap/drop/normal/peer).
 * REMARKS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_vsi_l2cp_trap_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE               handle_type
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_vsi_l2cp_trap_set" API.
 *     Refer to "soc_ppd_vsi_l2cp_trap_set" API for details.
*********************************************************************/
uint32
  soc_ppd_vsi_l2cp_trap_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_OUT SOC_PPD_VSI_L2CP_HANDLE_TYPE               *handle_type
  );

/*********************************************************************
* NAME:
 *   soc_ppd_vsi_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_vsi module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_vsi_get_procs_ptr(void);

void
  SOC_PPD_VSI_L2CP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_VSI_L2CP_KEY *key
  );

void
  SOC_PPD_VSI_INFO_clear(
    SOC_SAND_OUT SOC_PPD_VSI_INFO *info
  );

void
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY_clear(
    SOC_SAND_OUT SOC_PPD_VSI_DEFAULT_FRWRD_KEY *info
  );

#if SOC_PPD_DEBUG_IS_LVL1

void
  SOC_PPD_VSI_INFO_print(
    SOC_SAND_IN  SOC_PPD_VSI_INFO *info
  );

void
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY_print(
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY *info
  );

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_vsi_map_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              eg_local_vsid
  );

void
  soc_ppd_vsi_map_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid
  );

void
  soc_ppd_vsi_default_frwrd_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile
  );

void
  soc_ppd_vsi_default_frwrd_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key
  );

void
  soc_ppd_vsi_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PPD_VSI_INFO                            *vsi_info
  );

void
  soc_ppd_vsi_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx
  );

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_VSI_INCLUDED__*/
#endif

