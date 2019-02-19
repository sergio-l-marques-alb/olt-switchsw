/* $Id: ppd_api_llp_sa_auth.h,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_llp_sa_auth.h
*
* MODULE PREFIX:  soc_ppd_llp
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

#ifndef __SOC_PPD_API_LLP_SA_AUTH_INCLUDED__
/* { */
#define __SOC_PPD_API_LLP_SA_AUTH_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_llp_sa_auth.h>

#include <soc/dpp/PPD/ppd_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Use to accept SA from all VIDS. Note that when SA is not
 *     found in the authentication DB, then it will be
 *     accepted.                                               */
#define  SOC_PPD_LLP_SA_AUTH_ACCEPT_ALL_VIDS (SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS)

/*     Use to accept SA from all source system ports. Note that
 *     when SA is not found in the authentication DB, then it
 *     will be accepted.                                       */
#define  SOC_PPD_LLP_SA_AUTH_ACCEPT_ALL_PORTS (SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS)

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
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_SET = SOC_PPD_PROC_DESC_BASE_LLP_SA_AUTH_FIRST,
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_SET_PRINT,
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_GET,
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_GET_PRINT,
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_SET,
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_SET_PRINT,
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_GET,
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_GET_PRINT,
  SOC_PPD_LLP_SA_AUTH_GET_BLOCK,
  SOC_PPD_LLP_SA_AUTH_GET_BLOCK_PRINT,
  SOC_PPD_LLP_SA_AUTH_GET_PROCS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PPD_LLP_SA_AUTH_PROCEDURE_DESC_LAST
} SOC_PPD_LLP_SA_AUTH_PROCEDURE_DESC;

#define SOC_PPD_LLP_SA_AUTH_MATCH_RULE_TYPE_PORT               SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_PORT
#define SOC_PPD_LLP_SA_AUTH_MATCH_RULE_TYPE_VID                SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_VID
#define SOC_PPD_LLP_SA_MATCH_RULE_TYPE_ALL                     SOC_PPC_LLP_SA_MATCH_RULE_TYPE_ALL
typedef SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE                    SOC_PPD_LLP_SA_AUTH_MATCH_RULE_TYPE;

typedef SOC_PPC_LLP_SA_AUTH_PORT_INFO                          SOC_PPD_LLP_SA_AUTH_PORT_INFO;
typedef SOC_PPC_LLP_SA_AUTH_MAC_INFO                           SOC_PPD_LLP_SA_AUTH_MAC_INFO;
typedef SOC_PPC_LLP_SA_AUTH_MATCH_RULE                         SOC_PPD_LLP_SA_AUTH_MATCH_RULE;

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
 *   soc_ppd_llp_sa_auth_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets port information for SA authentication, including
 *   whether to enable SA authentication on this port.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_PORT_INFO               *port_auth_info -
 *     The port information for SA Authentication.
 * REMARKS:
 *   - Soc_petraB only API
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_llp_sa_auth_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_llp_sa_auth_port_info_set" API.
 *     Refer to "soc_ppd_llp_sa_auth_port_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_llp_sa_auth_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPD_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_llp_sa_auth_mac_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set authentication information for a MAC address,
 *   including the expected VLAN/ports the MAC address has to
 *   come with.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key -
 *     MAC address to set authentication over it.
 *   SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MAC_INFO                *mac_auth_info -
 *     Authentication information for the given MAC.
 *   SOC_SAND_IN  uint8                               enable -
 *     Set to TRUE to add the entry, and to FALSE to remove the
 *     entry from the DB.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there is no place in the SA Auth DB.
 * REMARKS:
 *   - The DB used for SA Authorization is also shared for
 *   SA-base VID assignment. When setting Authorization over
 *   an SA MAC according to VID, then this is also the VID
 *   that may be used for VID assignment.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_llp_sa_auth_mac_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key,
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_IN  uint8                               enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_llp_sa_auth_mac_info_set" API.
 *     Refer to "soc_ppd_llp_sa_auth_mac_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_llp_sa_auth_mac_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key,
    SOC_SAND_OUT SOC_PPD_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_OUT uint8                               *enable
  );

/*********************************************************************
* NAME:
 *   soc_ppd_llp_sa_auth_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get SA authentation information according to source MAC
 *   address.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MATCH_RULE              *rule -
 *     Get only entries that match this rule.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range -
 *     Range for iteration
 *   SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key_arr -
 *     MAC address array.
 *   SOC_SAND_OUT SOC_PPD_LLP_SA_AUTH_MAC_INFO                *auth_info_arr -
 *     Authentication info
 *   SOC_SAND_OUT uint32                                *nof_entries -
 *     Number of returned enties in *auth_info_arr
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_llp_sa_auth_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MATCH_RULE              *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key_arr,
    SOC_SAND_OUT SOC_PPD_LLP_SA_AUTH_MAC_INFO                *auth_info_arr,
    SOC_SAND_OUT uint32                                *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_llp_sa_auth_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_llp_sa_auth module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_llp_sa_auth_get_procs_ptr(void);

void
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_SA_AUTH_PORT_INFO *info
  );

void
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_SA_AUTH_MAC_INFO *info
  );

void
  SOC_PPD_LLP_SA_AUTH_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPD_LLP_SA_AUTH_MATCH_RULE *info
  );

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_LLP_SA_AUTH_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MATCH_RULE_TYPE enum_val
  );

void
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_PORT_INFO *info
  );

void
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MAC_INFO *info
  );

void
  SOC_PPD_LLP_SA_AUTH_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MATCH_RULE *info
  );

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_llp_sa_auth_port_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  );

void
  soc_ppd_llp_sa_auth_port_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx
  );

void
  soc_ppd_llp_sa_auth_mac_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key,
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_IN  uint8                               enable
  );

void
  soc_ppd_llp_sa_auth_mac_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key
  );

void
  soc_ppd_llp_sa_auth_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_SA_AUTH_MATCH_RULE              *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  );

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_LLP_SA_AUTH_INCLUDED__*/
#endif

