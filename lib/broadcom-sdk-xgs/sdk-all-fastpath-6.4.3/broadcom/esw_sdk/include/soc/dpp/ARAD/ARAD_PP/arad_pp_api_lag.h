/* $Id: arad_pp_api_lag.h,v 1.8 Broadcom SDK $
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

#ifndef __ARAD_PP_API_LAG_INCLUDED__
/* { */
#define __ARAD_PP_API_LAG_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_lag.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximal LAG ID                                          */
#define  ARAD_PP_LAG_MAX_ID (SOC_PPC_LAG_MAX_ID)

/*     Maximal number of members in LAG                        */
#define  ARAD_PP_LAG_MEMBERS_MAX (SOC_PPC_LAG_MEMBERS_MAX)


#define ARAD_PP_LAG_LB_CRC_0x14D    SOC_PPC_LAG_LB_CRC_0x14D  
#define ARAD_PP_LAG_LB_CRC_0x1C3    SOC_PPC_LAG_LB_CRC_0x1C3  
#define ARAD_PP_LAG_LB_CRC_0x1CF    SOC_PPC_LAG_LB_CRC_0x1CF  
#define ARAD_PP_LAG_LB_KEY          SOC_PPC_LAG_LB_KEY           /* Use LB-Key-Packet-Data directly */          
#define ARAD_PP_LAG_LB_ROUND_ROBIN  SOC_PPC_LAG_LB_ROUND_ROBIN   /* Use counter incremented every packet */
#define ARAD_PP_LAG_LB_2_CLOCK      SOC_PPC_LAG_LB_2_CLOCK        /* User counter incremented every two clocks */
#define ARAD_PP_LAG_LB_CRC_0x10861  SOC_PPC_LAG_LB_CRC_0x10861
#define ARAD_PP_LAG_LB_CRC_0x10285  SOC_PPC_LAG_LB_CRC_0x10285
#define ARAD_PP_LAG_LB_CRC_0x101A1  SOC_PPC_LAG_LB_CRC_0x101A1
#define ARAD_PP_LAG_LB_CRC_0x12499  SOC_PPC_LAG_LB_CRC_0x12499
#define ARAD_PP_LAG_LB_CRC_0x1F801  SOC_PPC_LAG_LB_CRC_0x1F801
#define ARAD_PP_LAG_LB_CRC_0x172E1  SOC_PPC_LAG_LB_CRC_0x172E1
#define ARAD_PP_LAG_LB_CRC_0x1EB21  SOC_PPC_LAG_LB_CRC_0x1EB21
#define ARAD_PP_LAG_LB_CRC_0x13965  SOC_PPC_LAG_LB_CRC_0x13965
#define ARAD_PP_LAG_LB_CRC_0x1698D  SOC_PPC_LAG_LB_CRC_0x1698D
#define ARAD_PP_LAG_LB_CRC_0x1105D  SOC_PPC_LAG_LB_CRC_0x1105D
#define ARAD_PP_LAG_LB_CRC_0x8003   SOC_PPC_LAG_LB_CRC_0x8003
#define ARAD_PP_LAG_LB_CRC_0x8011   SOC_PPC_LAG_LB_CRC_0x8011
#define ARAD_PP_LAG_LB_CRC_0x8423   SOC_PPC_LAG_LB_CRC_0x8423
#define ARAD_PP_LAG_LB_CRC_0x8101   SOC_PPC_LAG_LB_CRC_0x8101
#define ARAD_PP_LAG_LB_CRC_0x84a1   SOC_PPC_LAG_LB_CRC_0x84a1
#define ARAD_PP_LAG_LB_CRC_0x9019   SOC_PPC_LAG_LB_CRC_0x9019

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

#define ARAD_PP_LAG_HASH_FRST_HDR_FARWARDING                 SOC_PPC_LAG_HASH_FRST_HDR_FARWARDING
#define ARAD_PP_LAG_HASH_FRST_HDR_LAST_TERMINATED            SOC_PPC_LAG_HASH_FRST_HDR_LAST_TERMINATED
#define ARAD_PP_NOF_LAG_HASH_FRST_HDRS                       SOC_PPC_NOF_LAG_HASH_FRST_HDRS
typedef SOC_PPC_LAG_HASH_FRST_HDR                              ARAD_PP_LAG_HASH_FRST_HDR;

#define ARAD_PP_LAG_LB_TYPE_HASH                             SOC_PPC_LAG_LB_TYPE_HASH
#define ARAD_PP_LAG_LB_TYPE_SMOOTH_DIVISION                  SOC_PPC_LAG_LB_TYPE_SMOOTH_DIVISION
#define ARAD_PP_LAG_LB_TYPE_ROUND_ROBIN                      SOC_PPC_LAG_LB_TYPE_ROUND_ROBIN
#define ARAD_PP_LAG_LB_TYPE_RESILIENT_HASH                   SOC_PPC_LAG_LB_TYPE_RESILIENT_HASH
#define ARAD_PP_NOF_LAG_LB_TYPES                             SOC_PPC_NOF_LAG_LB_TYPES
typedef SOC_PPC_LAG_LB_TYPE                                    ARAD_PP_LAG_LB_TYPE;

typedef SOC_PPC_LAG_HASH_GLOBAL_INFO                           ARAD_PP_LAG_HASH_GLOBAL_INFO;
typedef SOC_PPC_LAG_HASH_PORT_INFO                             ARAD_PP_LAG_HASH_PORT_INFO;
typedef SOC_PPC_HASH_MASK_INFO                                 ARAD_PP_HASH_MASK_INFO;
typedef SOC_PPC_LAG_MEMBER                                     ARAD_PP_LAG_MEMBER;
typedef SOC_PPC_LAG_INFO                                       ARAD_PP_LAG_INFO;


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
 *   arad_pp_lag_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure a LAG. A LAG is defined by a group of System
 *   Physical Ports that compose it. This configuration
 *   affects 1. LAG resolution: when the destination of
 *   packet is LAG 2. Learning: when packet source port
 *   belongs to LAG, then the LAG is learnt.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      lag_ndx -
 *     LAG ID. Range: 0 - 255.
 *   SOC_SAND_IN  ARAD_PP_LAG_INFO                                *lag_info -
 *     Lag members. Maximal number of out-going LAG members is
 *     16. The number of incoming LAG members is not limited,
 *     and it can be the number of Local FAP ports in each
 *     device.
 * REMARKS:
 *   1. Local to system port mapping must be configured
 *   before using this API (Incoming and Outgoing) - for LAG
 *   pruning. 2. LAG configuration must be consistent
 *   system-wide, for incoming and outgoing ports. 3. The
 *   member index inside the LAG (0-255) is defined by the
 *   index of the appropriate port in the members array. 4.
 *   Setting LAG with a group of system ports, will first
 *   clean-up any previous configuration of the LAG. For
 *   example, setting LAG 1 with system members 1,2,3,4 and
 *   then setting the same LAG with members 3,4,5,6 will
 *   clean up the effect of the previous configuration and
 *   set up the new configuration.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_INFO                                *lag_info
  );

/*********************************************************************
*     Gets the configuration set by the "arad_pp_lag_set" API.
 *     Refer to "arad_pp_lag_set" API for details.
*********************************************************************/
uint32
  arad_pp_lag_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lag_ndx,
    SOC_SAND_OUT ARAD_PP_LAG_INFO                                *lag_info
  );


/*********************************************************************
* NAME:
 *   arad_pp_lag_member_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add a system port as a member in LAG.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      lag_ndx -
 *     LAG ID. Range: 0 - 255.
 *   SOC_SAND_IN  ARAD_PP_LAG_MEMBER                              *member -
 *     System port to be added as a member, and the
 *     member-index.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there is no place in the SA Auth DB.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_member_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER                              *member,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_member_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a system port from a LAG.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      lag_ndx -
 *     LAG ID. Range: 0 - 255.
 *   SOC_SAND_IN  uint32                                      sys_port -
 *     System port to be removed as a member.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_member_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER                         *member
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_hashing_global_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the LAG hashing global attributes
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_LAG_HASH_GLOBAL_INFO                    *glbl_hash_info -
 *     LAG Hashing global settings
 * REMARKS:
 *   The hashing function result is 8 bit value. The TM use
 *   the value to choose the LAG port member, to which the
 *   packet is sent
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_hashing_global_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_GLOBAL_INFO                    *glbl_hash_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lag_hashing_global_info_set" API.
 *     Refer to "arad_pp_lag_hashing_global_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lag_hashing_global_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT ARAD_PP_LAG_HASH_GLOBAL_INFO                    *glbl_hash_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_hashing_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the LAG hashing per-lag attributes
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                port_ndx -
 *     In-PP Port ID
 *   SOC_SAND_IN  ARAD_PP_LAG_HASH_PORT_INFO                  *lag_hash_info -
 *     LAG Hashing per-ingress port settings
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_hashing_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_PORT_INFO                  *lag_hash_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lag_hashing_port_info_set" API.
 *     Refer to "arad_pp_lag_hashing_port_info_set" API for
 *     details.
*********************************************************************/

uint32
  arad_pp_lag_hashing_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_OUT ARAD_PP_LAG_HASH_PORT_INFO                  *lag_hash_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_hashing_mask_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the key used by hash functions for LAG/ECMP load
 *   balancing.
 * INPUT:
 *   SOC_SAND_IN  int                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_HASH_MASK_INFO       *mask_info -
 *     how to build the key used as input by hash functions for
 *     LAG/ECMP load balancing
 * REMARKS:
 *   This setting is mutual to the ECMP hashing function
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_hashing_mask_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_HASH_MASK_INFO       *mask_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lag_hashing_mask_set" API.
 *     Refer to "arad_pp_lag_hashing_mask_set" API for details.
*********************************************************************/
uint32
  arad_pp_lag_hashing_mask_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT ARAD_PP_HASH_MASK_INFO       *mask_info
  );

uint32
  arad_pp_lag_lb_key_range_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_LAG_INFO                                *lag_info);

void
  ARAD_PP_LAG_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LAG_HASH_GLOBAL_INFO *info
  );

void
  ARAD_PP_LAG_HASH_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LAG_HASH_PORT_INFO *info
  );

void
  ARAD_PP_HASH_MASK_INFO_clear(
    SOC_SAND_OUT ARAD_PP_HASH_MASK_INFO *info
  );

void
  ARAD_PP_LAG_MEMBER_clear(
    SOC_SAND_OUT ARAD_PP_LAG_MEMBER *info
  );


void
  ARAD_PP_LAG_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LAG_INFO *info
  );


#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_LAG_HASH_FRST_HDR_to_string(
    SOC_SAND_IN  ARAD_PP_LAG_HASH_FRST_HDR enum_val
  );

const char*
  ARAD_PP_LAG_LB_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_LAG_LB_TYPE enum_val
  );

void
  ARAD_PP_LAG_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  ARAD_PP_LAG_HASH_GLOBAL_INFO *info
  );

void
  ARAD_PP_LAG_HASH_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_LAG_HASH_PORT_INFO *info
  );

void
  ARAD_PP_HASH_MASK_INFO_print(
    SOC_SAND_IN  ARAD_PP_HASH_MASK_INFO *info
  );

void
  ARAD_PP_LAG_MEMBER_print(
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER *info
  );

void
  ARAD_PP_LAG_INFO_print(
    SOC_SAND_IN  ARAD_PP_LAG_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_LAG_INCLUDED__*/
#endif


