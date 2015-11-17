/* $Id: arad_pp_frwrd_extend_p2p.h,v 1.7 Broadcom SDK $
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

#ifndef __ARAD_PP_FRWRD_EXTEND_P2P_INCLUDED__
/* { */
#define __ARAD_PP_FRWRD_EXTEND_P2P_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_extend_p2p.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define ARAD_PP_FRWRD_P2P_KEY_TYPE_AC (1)
#define ARAD_PP_FRWRD_P2P_KEY_TYPE_PWE (2)
#define ARAD_PP_FRWRD_P2P_KEY_TYPE_MIM (4)


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
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET = ARAD_PP_PROC_DESC_BASE_FRWRD_EXTEND_P2P_FIRST,
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET,
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_GET,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_GET_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_GET_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_GET_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_ADD,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_ADD_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_ADD_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_ADD_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_GET,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_GET_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_GET_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_GET_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_REMOVE,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_REMOVE_PRINT,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_EXTEND_P2P_MIM_REMOVE_VERIFY,
  ARAD_PP_FRWRD_EXTEND_P2P_GET_PROCS_PTR,
  ARAD_PP_FRWRD_EXTEND_P2P_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_FRWRD_EXTEND_P2P_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_EXTEND_P2P_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_FRWRD_EXTEND_P2P_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_EXTEND_P2P_FIRST,
  ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_LEN_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  ARAD_PP_FRWRD_EXTEND_P2P_MPLS_CMD_IS_NOT_PUSH_ERR,
  ARAD_PP_FRWRD_EXTEND_P2P_MPLS_CMD_PUSH_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_EXTEND_P2P_INFO_FIELD_NOT_SUPPORTED_FOR_AC_ERR,
  ARAD_PP_FRWRD_EXTEND_P2P_KEY_PWE_FIELD_NOT_SUPPORTED_ERR,
  ARAD_PP_FRWRD_EXTEND_P2P_ALWAYS_ENABLED_ERR,


  /*
   * Last element. Do no touch.
   */
  ARAD_PP_FRWRD_EXTEND_P2P_ERR_LAST
} ARAD_PP_FRWRD_EXTEND_P2P_ERR;

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

uint32
  arad_pp_frwrd_extend_p2p_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_glbl_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the Extended P2P services,
 *   including enabling this mode and the profiles to assign
 *   for these services
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO          *glbl_info -
 *     Global information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO          *glbl_info
  );

uint32
  arad_pp_frwrd_extend_p2p_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO          *glbl_info
  );

uint32
  arad_pp_frwrd_extend_p2p_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_extend_p2p_glbl_info_set_unsafe" API.
 *     Refer to "arad_pp_frwrd_extend_p2p_glbl_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO          *glbl_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_ac_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add P2P service for packets entering from the access
 *   side with AC key = (vlan-domain x VID x VID). Thus,
 *   these packets will have the same processing and
 *   forwarding regardless of the MAC.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key (port x VID x VID)
 *   SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *ac_p2p_info -
 *     Processing and forwarding information.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there is no place in the DB.
 * REMARKS:
 *   - The AC key must not exist in the LIF DB; otherwise,
 *   packets matching this key will be mapped to the AC and
 *   so will be processed and forwarded according to the
 *   AC/VSI configuration. - Packets assigned the Extended
 *   P2P service are mapped to the same VSI/AC configured by
 *   soc_ppd_frwrd_extend_p2p_glbl_info_set. - In-Port (also the
 *   port in the AC key) has to be set to
 *   enable-in-extended-P2P. - Out-Port (the packet
 *   designated to) has to be set to enable-out-extended-P2P.
 *   - Split horizon check is not needed for P2P Services.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_ac_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *ac_p2p_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_extend_p2p_ac_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *ac_p2p_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_ac_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the P2P service for packets entering from the
 *   access side with AC key = (vlan-domain x VID x VID).
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key -
 *     AC key (port x VID x VID)
 *   SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *ac_p2p_info -
 *     Processing and forwarding information.
 *   SOC_SAND_OUT uint8                                *found -
 *     Whether the key exists
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_ac_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *ac_p2p_info,
    SOC_SAND_OUT uint8                                *found
  );

uint32
  arad_pp_frwrd_extend_p2p_ac_get_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_ac_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove P2P service for packets entering from the access
 *   side with AC key = (vlan-domain x VID x VID).
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key -
 *     AC key (port x VID x VID)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_ac_remove_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key
  );

uint32
  arad_pp_frwrd_extend_p2p_ac_remove_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_pwe_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add P2P service for packets entering with PWE key =
 *   (in-RIF x VC label). Thus, these packets will have the
 *   same processing and forwarding regardless of the MAC.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY                *pwe_key -
 *     PWE key (VC label x [inRIF] x [port])
 *   SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *pwe_p2p_info -
 *     Processing and forwarding information.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there is no place in the DB.
 * REMARKS:
 *   - The PWE key must not exist in the LIF DB; otherwise,
 *   packets matching this key is mapped to the PWE and then
 *   processed and forwarded according to the PWE/VSI
 *   configuration. - Packets assigned the Extended P2P
 *   service are mapped to the same VSI/AC configured by
 *   soc_ppd_frwrd_extend_p2p_glbl_info_set. - In-Port has to be
 *   set to enable-in-extended-P2P. - Out-Port (the packet
 *   designated to) has to be set to enable-out-extended-P2P.
 *   - Split horizon check is not needed for P2P Services.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_pwe_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY                *pwe_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *pwe_p2p_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_extend_p2p_pwe_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY                *pwe_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *pwe_p2p_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_pwe_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the P2P service for packets entering with PWE key =
 *   (in-RIF x VC label).
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key -
 *     PWE key (VC label x [inRIF] x [port])
 *   SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *pwe_p2p_info -
 *     Processing and forwarding information.
 *   SOC_SAND_OUT uint8                                *found -
 *     Whether the key exists.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_pwe_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *pwe_p2p_info,
    SOC_SAND_OUT uint8                                *found
  );

uint32
  arad_pp_frwrd_extend_p2p_pwe_get_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_pwe_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Removes P2P service for packets entering with PWE key =
 *   (in-RIF x VC label).
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key -
 *     PWE key (VC label x [inRIF] x [port])
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_pwe_remove_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key
  );

uint32
  arad_pp_frwrd_extend_p2p_pwe_remove_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_mim_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add P2P service for packets entering with MIM
 *   (MAC-in_MAC) key = (ISID x ISID-domain) from PBP port.
 *   Thus, these packets will have the same processing and
 *   forwarding, regardless of the Customer MAC address.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *mim_key -
 *     MIM (MAC-in-MAC) key (ISID x ISID-domain)
 *   SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *mim_p2p_info -
 *     Processing and forwarding information.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there is no place in the DB.
 * REMARKS:
 *   - Packets assigned Extended P2P service are mapped to
 *   same VSI/AC configured by
 *   soc_ppd_frwrd_extend_p2p_glbl_info_set. - In-port has to be
 *   of type PBP. - In-Port has to be set to
 *   enable-in-extended-P2P. - Out-Port(the packet designated
 *   to) has to be set to enable-out-extended-P2P. - Split
 *   horizon check is not needed for P2P Services.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_mim_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *mim_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *mim_p2p_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_extend_p2p_mim_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *mim_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *mim_p2p_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_mim_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the P2P service for packets entering with MIM
 *   (MAC-in_MAC) key = (ISID x ISID-domain) from PBP port.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *mim_key -
 *     MIM (MAC-in-MAC) key (ISID x ISID-domain)
 *   SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *mim_p2p_info -
 *     Processing and forwarding information.
 *   SOC_SAND_OUT uint8                                *found -
 *     Whether the key exists
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_mim_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *mim_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *mim_p2p_info,
    SOC_SAND_OUT uint8                                *found
  );

uint32
  arad_pp_frwrd_extend_p2p_mim_get_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *mim_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_mim_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove P2P service for packets entering with MIM
 *   (MAC-in_MAC) key = (ISID x ISID-domain) from PBP port.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *mim_key -
 *     MIM (MAC-in-MAC) key (ISID x ISID-domain)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_mim_remove_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *mim_key
  );

uint32
  arad_pp_frwrd_extend_p2p_mim_remove_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *mim_key
  );


uint32
  arad_pp_frwrd_p2p_info_from_lem_buffer(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                                  key_type,
      SOC_SAND_OUT  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *p2p_info,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_PAYLOAD                   *payload
);


/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_frwrd_extend_p2p module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_extend_p2p_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_extend_p2p_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_frwrd_extend_p2p module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_extend_p2p_get_errs_ptr(void);

uint32
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO *info
  );

uint32
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO *info
  );

uint32
  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO *info,
    SOC_SAND_IN  uint32                          key_type
  );

uint32
  ARAD_PP_FRWRD_EXTEND_PWE_KEY_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_FRWRD_EXTEND_P2P_INCLUDED__*/
#endif
