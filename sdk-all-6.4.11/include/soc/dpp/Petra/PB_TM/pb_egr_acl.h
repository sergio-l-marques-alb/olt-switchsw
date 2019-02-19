/* $Id: soc_pb_egr_acl.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_EGR_ACL_INCLUDED__
/* { */
#define __SOC_PB_EGR_ACL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_key.h>

#include <soc/dpp/TMC/tmc_api_egr_acl.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of Drop Precedence values                        */
#define  SOC_PB_EGR_ACL_NOF_DP_VALUES (SOC_TMC_EGR_ACL_NOF_DP_VALUES)

#define SOC_PB_EGR_ACL_DB_TYPE_MAX                                 (SOC_PB_EGR_NOF_ACL_DB_TYPES-1)


#define SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE_LSB             (139)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT_LSB            (134)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_OUTER_TAG_LSB                 (118)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_INNER_TAG_LSB                 (102)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_SA_LSB                        (54)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_DA_LSB                        (6)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA_LSB      (0)

#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_TOS_LSB                     (86)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE_LSB             (82)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_SIP_LSB                     (50)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_DIP_LSB                     (18)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID_LSB               (6)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA_LSB    (0)

#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_FTMH_LSB                    (76)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA_LSB                (38)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_LABEL_LSB                   (6+8+4)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_EXP_LSB                     (15)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_TTL_LSB                     (6)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA_LSB    (0)

#define SOC_PB_EGR_ACL_TCAM_FLD_TM_FTMH_LSB                      (76)
#define SOC_PB_EGR_ACL_TCAM_FLD_TM_HDR_DATA_LSB                  (6)
#define SOC_PB_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA_LSB      (0)

#define SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE_SIZE             (4)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT_SIZE            (5)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_OUTER_TAG_SIZE                 (16)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_INNER_TAG_SIZE                 (16)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_SA_SIZE                        (48)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_DA_SIZE                        (48)
#define SOC_PB_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA_SIZE      (6)

#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_TOS_SIZE                     (8)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE_SIZE             (4)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_SIP_SIZE                     (32)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_DIP_SIZE                     (32)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID_SIZE               (12)
#define SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA_SIZE    (6)

#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_FTMH_SIZE                    (48+16)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA_SIZE                (38)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_LABEL_SIZE                   (20)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_EXP_SIZE                     (3)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_TTL_SIZE                     (8)
#define SOC_PB_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA_SIZE    (6)

#define SOC_PB_EGR_ACL_TCAM_FLD_TM_FTMH_SIZE                      (48 + 16)
#define SOC_PB_EGR_ACL_TCAM_FLD_TM_HDR_DATA_SIZE                  (70)
#define SOC_PB_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA_SIZE      (6)

#define SOC_PB_FP_TCAM_DB_ALLOCATION_BASE                         (3)


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

typedef SOC_TMC_EGR_ACL_DP_VALUES                              SOC_PB_EGR_ACL_DP_VALUES;
typedef SOC_TMC_EGR_ACL_PORT_INFO                              SOC_PB_EGR_ACL_PORT_INFO;
typedef SOC_TMC_EGR_ACL_ACTION_VAL                             SOC_PB_EGR_ACL_ACTION_VAL;
typedef SOC_TMC_EGR_ACL_ENTRY_INFO                             SOC_PB_EGR_ACL_ENTRY_INFO;

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
  soc_pb_egr_fp_tcam_db_id_get(
    SOC_SAND_IN uint32 db_id
  );

uint32
  soc_pb_egr_fp_db_id_from_tcam_db_id_get(
    SOC_SAND_IN uint32 tcam_db_id
  );

/*********************************************************************
* NAME:
*     soc_pb_egr_acl_init
* FUNCTION:
*     Initialization of the egr_acl configuration.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_pb_egr_acl_init(
    SOC_SAND_IN  int  unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_dp_values_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the predefined Drop Precedence values if a DP
 *   modification must be done.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES        *dp_val -
 *     DP values.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_dp_values_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES        *dp_val
  );

uint32
  soc_pb_egr_acl_dp_values_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES        *dp_val
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_egr_acl_dp_values_set_unsafe" API.
 *     Refer to "soc_pb_egr_acl_dp_values_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_egr_acl_dp_values_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_EGR_ACL_DP_VALUES        *dp_val
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_l4_protocol_code_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Protocol code value according to the L4 Protocol
 *   code index. It determines the protocol field in L3 ACL
 *   Key.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   code_ndx -
 *     Protocol Code index. Range: 1 - 15.
 *   SOC_SAND_IN  uint32                    protocol_code -
 *     Protocol code value. Range: 0 - 255.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_l4_protocol_code_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   code_ndx,
    SOC_SAND_IN  uint32                    protocol_code
  );

uint32
  soc_pb_egr_acl_l4_protocol_code_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   code_ndx,
    SOC_SAND_IN  uint32                    protocol_code
  );

uint32
  soc_pb_egr_acl_l4_protocol_code_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   code_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_egr_acl_l4_protocol_code_set_unsafe" API.
 *     Refer to "soc_pb_egr_acl_l4_protocol_code_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_egr_acl_l4_protocol_code_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   code_ndx,
    SOC_SAND_OUT uint32                    *protocol_code
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_port_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Port ACL properties: ACL profile and ACL Data.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   pp_port_ndx -
 *     PP-Port. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO        *info -
 *     Port ACL parameters.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_port_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   pp_port_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO        *info
  );

uint32
  soc_pb_egr_acl_port_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   pp_port_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO        *info
  );

uint32
  soc_pb_egr_acl_port_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   pp_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_egr_acl_port_set_unsafe" API.
 *     Refer to "soc_pb_egr_acl_port_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_egr_acl_port_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   pp_port_ndx,
    SOC_SAND_OUT SOC_PB_EGR_ACL_PORT_INFO        *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_key_profile_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mapping between the forwarding type and
 *   Port-ACL-profile to the ACL-Key-profile.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type -
 *     Packet forwarding type.
 *   SOC_SAND_IN  uint32                   acl_profile_ndx -
 *     Port ACL profile. Range: 0 - 3.
 *   SOC_SAND_IN  uint32                    db_id -
 *     ACL Key profile. Range: 0 - 7.
 * REMARKS:
 *   None
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_key_profile_map_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type,
    SOC_SAND_IN  uint32                   acl_profile_ndx,
    SOC_SAND_IN  uint32                    fp_hw_db_ndx
  );

uint32
  soc_pb_egr_acl_key_profile_map_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type,
    SOC_SAND_IN  uint32                   acl_profile_ndx,
    SOC_SAND_IN  uint32                    fp_hw_db_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_egr_acl_key_profile_map_set_unsafe" API.
 *     Refer to "soc_pb_egr_acl_key_profile_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_egr_acl_key_profile_map_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type,
    SOC_SAND_IN  uint32                   acl_profile_ndx,
    SOC_SAND_OUT uint32                    *fp_hw_db_ndx
  );

uint32
  soc_pb_egr_acl_key_profile_map_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PKT_FRWRD_TYPE           fwd_type,
    SOC_SAND_IN  uint32                   acl_profile_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_db_create_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Create the Database.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   db_ndx -
 *     Database index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_PB_EGR_ACL_DB_TYPE          db_type -
 *     Database type: indicates the TCAM Key format.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success -
 *     Indicates if all the Databases can be searched at once.
 * REMARKS:
 *   None
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_db_create_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   hw_db_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_DB_TYPE          db_type,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );

uint32
  soc_pb_egr_acl_db_create_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   hw_db_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_DB_TYPE          db_type
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_db_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Database key type.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   db_ndx -
 *     Database index. Range: 0 - 63.
 *   SOC_SAND_OUT SOC_PB_EGR_ACL_DB_TYPE          *db_type -
 *     Database type: indicates the TCAM Key format.
 * REMARKS:
 *   None
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_db_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_OUT SOC_PB_EGR_ACL_DB_TYPE          *db_type
  );

uint32
  soc_pb_egr_acl_db_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_db_destroy_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove the Database.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   db_ndx -
 *     Database index. Range: 0 - 63.
 * REMARKS:
 *   None
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_db_destroy_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   hw_db_ndx
  );

uint32
  soc_pb_egr_acl_db_destroy_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   hw_db_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the Database.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   db_ndx -
 *     Database index. Range: 0 - 63.
 *   SOC_SAND_IN  uint32                   entry_ndx -
 *     Entry id. Range: 0 - 2047.
 *   SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO       *info -
 *     Database type: indicates the TCAM Key format.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success -
 *     Indicates if all the Databases can be searched at once.
 * REMARKS:
 *   None
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_entry_add_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );

uint32
  soc_pb_egr_acl_entry_add_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx,
    SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO       *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   db_ndx -
 *     Database index. Range: 0 - 63.
 *   SOC_SAND_IN  uint32                   entry_ndx -
 *     Entry id. Range: 0 - 2047.
 *   SOC_SAND_OUT SOC_PB_EGR_ACL_ENTRY_INFO       *info -
 *     Database type: indicates the TCAM Key format.
 *   SOC_SAND_OUT uint8                   *is_found -
 *     Indicates if the entry is found in the Database.
 * REMARKS:
 *   None
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_entry_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx,
    SOC_SAND_OUT SOC_PB_EGR_ACL_ENTRY_INFO       *info,
    SOC_SAND_OUT uint8                   *is_found
  );

uint32
  soc_pb_egr_acl_entry_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_acl_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   db_ndx -
 *     Database index. Range: 0 - 63.
 *   SOC_SAND_IN  uint32                   entry_ndx -
 *     Entry id. Range: 0 - 2047.
 * REMARKS:
 *   None
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_acl_entry_remove_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx
  );

uint32
  soc_pb_egr_acl_entry_remove_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   fp_hw_db_ndx,
    SOC_SAND_IN  uint32                   entry_ndx
  );

uint32
  SOC_PB_EGR_ACL_DP_VALUES_verify(
    SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES *info
  );

uint32
  SOC_PB_EGR_ACL_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO *info
  );

uint32
  SOC_PB_EGR_ACL_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_EGR_ACL_ACTION_VAL *info
  );

uint32
  SOC_PB_EGR_ACL_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO *info
  );

void
  SOC_PB_EGR_ACL_DP_VALUES_clear(
    SOC_SAND_OUT SOC_PB_EGR_ACL_DP_VALUES *info
  );

void
  SOC_PB_EGR_ACL_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_EGR_ACL_PORT_INFO *info
  );

void
  SOC_PB_EGR_ACL_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_EGR_ACL_ACTION_VAL *info
  );

void
  SOC_PB_EGR_ACL_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_EGR_ACL_ENTRY_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

void
  SOC_PB_EGR_ACL_DP_VALUES_print(
    SOC_SAND_IN  SOC_PB_EGR_ACL_DP_VALUES *info
  );

void
  SOC_PB_EGR_ACL_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_EGR_ACL_PORT_INFO *info
  );

void
  SOC_PB_EGR_ACL_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_EGR_ACL_ACTION_VAL *info
  );

void
  SOC_PB_EGR_ACL_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_EGR_ACL_ENTRY_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_EGR_ACL_INCLUDED__*/
#endif

