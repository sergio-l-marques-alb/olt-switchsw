/* $Id: pcp_oam_api_eth.h,v 1.4 Broadcom SDK $
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

#ifndef __SOC_PCP_OAM_API_ETH_INCLUDED__
/* { */
#define __SOC_PCP_OAM_API_ETH_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PCP/pcp_oam_api_general.h>

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

/*
 *  When set disables the generation of VLAN tag on outgoing
 *  CFM packets for the configured MEP (up/down)
 */
typedef uint32 PCP_OAM_ETH_NO_VID;


/*
 *  When set disables the generation of out-AC on outgoing
 *  CFM packets for the configured MEP (down only)
 */
typedef uint32 PCP_OAM_ETH_NO_OUT_AC;


/*
 *  Class Of Service profile identifier. Range: 0-3
 */
typedef uint32 PCP_OAM_ETH_COS_PROFILE_ID;


/*
 *  Maintenance point level. Range: 0-7
 */
typedef uint32 PCP_OAM_ETH_MP_LEVEL;


/*
 *  MEP DB identifier. Range: 0-4K
 */
typedef uint32 PCP_OAM_ETH_ACC_MEP_ID;


/*
 *  Accelerated LMM identifier. Range: 0-127
 */
typedef uint32 PCP_OAM_ETH_ACC_LMM_ID;


/*
 *  Accelerated DLM identifier. Range: 0-127
 */
typedef uint32 PCP_OAM_ETH_ACC_DLM_ID;


typedef enum
{
  /*
   *  Up MEP is a MEP that is external to the monitored
   *  domain.
   */
  PCP_OAM_ETH_MEP_DIRECTION_UP = 0,
  /*
   *  Down MEP is a MEP that is internal to the monitored
   *  domain.
   */
  PCP_OAM_ETH_MEP_DIRECTION_DOWN = 1,
  /*
   *  Number of types in PCP_OAM_ETH_MEP_DIRECTION
   */
  PCP_OAM_NOF_ETH_MEP_DIRECTIONS = 2
}PCP_OAM_ETH_MEP_DIRECTION;

typedef enum
{
  /*
   *  Disable CCM Transmission
   */
  PCP_OAM_ETH_INTERVAL_0 = 0,
  /*
   *  CCM every 3.3 milliseconds
   */
  PCP_OAM_ETH_INTERVAL_1 = 1,
  /*
   *  CCM every 10 milliseconds
   */
  PCP_OAM_ETH_INTERVAL_2 = 2,
  /*
   *  CCM every 100 milliseconds
   */
  PCP_OAM_ETH_INTERVAL_3 = 3,
  /*
   *  CCM every 1 seconds
   */
  PCP_OAM_ETH_INTERVAL_4 = 4,
  /*
   *  CCM every 10 seconds
   */
  PCP_OAM_ETH_INTERVAL_5 = 5,
  /*
   *  CCM every 1 minutes
   */
  PCP_OAM_ETH_INTERVAL_6 = 6,
  /*
   *  CCM every 10 minutes
   */
  PCP_OAM_ETH_INTERVAL_7 = 7,
  /*
   *  Number of types in PCP_OAM_ETH_INTERVAL
   */
  PCP_OAM_NOF_ETH_INTERVALS = 8
}PCP_OAM_ETH_INTERVAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  41 upper bits of the source MAC address of the CFM
   *  packets (down/up MEP)
   */
  SOC_SAND_PP_MAC_ADDRESS mep_sa_msb;
  /*
   *  Determines the size (in bytes) of the user custom
   *  header. Applicable for all accelerated MEPs (See
   *  Procedure pcp_oam_eth_acc_mep_custom_header_set ()). Set
   *  this value to zero to disable custom header mechanism.
   *  Range: 0-4
   */
  uint8 custom_header_size;

} PCP_OAM_ETH_GENERAL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  ITMH Traffic class. Relevant only for down MEPs. Range:
   *  0-7
   */
  uint32 tc;
  /*
   *  ITMH drop precedence. Relevant only for down MEPs.
   *  Range: 0-3
   */
  uint32 dp;

} PCP_OAM_ETH_COS_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
   /*
   *	AC index. Range: 0 - 16K
   */
  SOC_PPD_LIF_ID ac_ndx;
  /*
   *  CFM level. Range: 0 - 7
   */
  PCP_OAM_ETH_MP_LEVEL level;
  /*
   *  MP direction
   */
  PCP_OAM_ETH_MEP_DIRECTION direction;

} PCP_OAM_ETH_MP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  T - Enable MP for this ACF - Disable MP for this AC
   */
  uint8 valid;
  /*
   *  The accelerated MEP handle to associate with the
   *  specified attachment circuit and level. Relevant only to
   *  APIs of accelerated MEPs. (E.g. CCM). Range: 0-4K
   */
  PCP_OAM_ETH_ACC_MEP_ID acc_mep_id;

} PCP_OAM_ETH_MP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  7 lower bits of the source MAC address of the CFM
   *  packet.
   */
  SOC_SAND_PP_MAC_ADDRESS dn_sa_lsb;
  /*
   *  This field determines the vid of generated CFM packet.
   *  If set to Type Definition PCP_OAM_ETH_NO_VID no vid is
   *  generated. Range: 0 - 4k.
   */
  uint32 dn_vid;
  /*
   *  Used by the OAMP to determine the destination to stamp
   *  on the ITMH (unicast flow/unicast direct). Refer to
   *  Soc_petra data sheet for more information.
   */
  SOC_TMC_DEST_INFO destination;
  /*
   *  This field determines the out_ac of generated CFM
   *  packet. If set to Type Definition PCP_OAM_ETH_NO_OUT_AC
   *  no out-AC is generated. Range: 0 - 16k.
   */
  uint32 ac;
  /*
   *  Used to map traffic class and drop precedence. See
   *  Procedure pcp_oam_eth_cos_mapping_info_set()
   */
  PCP_OAM_ETH_COS_PROFILE_ID cos_profile;

} PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  7 lower bits of the source MAC address of the CFM
   *  packet.
   */
  SOC_SAND_PP_MAC_ADDRESS up_sa_lsb;
  /*
   *  This field determines the vid of generated CFM packet.
   *  If set to zero no vid is generated
   */
  uint32 up_vid;

} PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Up MEP configuration
   */
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO up_mep;
  /*
   *  Down MEP configuration
   */
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO down_mep;

} PCP_OAM_ETH_ACC_MEP_FWD_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Interval between consecutive CCM messages
   */
  PCP_OAM_ETH_INTERVAL ccm_interval;
  /*
   *  Determines the priority of the generated CFM packet
   */
  uint8 ccm_priority;

} PCP_OAM_ETH_ACC_MEP_CCM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  T - enable, F - disable
   */
  uint8 lmm_enable;
  /*
   *  Accelerated LMM index. Range: 0 - 127
   */
  PCP_OAM_ETH_ACC_LMM_ID lmm_index;
  /*
   *  Dual-ended. T - The configured MEP generates LM packets
   *  over CCM (according to ITU Y.1731).
   */
  uint8 is_dual;
   /*
   *  T - far end identification, F - Disabled
   */
  uint8 lmr_enable;

} PCP_OAM_ETH_ACC_MEP_LMM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  T - enableF - disable
   */
  uint8 dlm_enable;
  /*
   *  Accelerated DLM index. Range: 0 - 127
   */
  PCP_OAM_ETH_ACC_DLM_ID dlm_index;

} PCP_OAM_ETH_ACC_MEP_DLM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  CCM configuration
   */
  PCP_OAM_ETH_ACC_MEP_CCM_INFO ccm_info;
  /*
   *  LMM configuration
   */
  PCP_OAM_ETH_ACC_MEP_LMM_INFO lmm_info;
  /*
   *  DLM configuration
   */
  PCP_OAM_ETH_ACC_MEP_DLM_INFO dlm_info;

} PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Maintenance Association is set of MEPs, each configured
   *  with the same ma_id and maintenance domain Level, Range:
   *  0-64K
   */
  uint32 ma_id;
  /*
   *  The identifier of the transmitting MEP. Stamped on the
   *  CFM packet. Unique for all MEPs in the same maintenance
   *  association (MA). Range: 0-8K
   */
  uint32 mep_id;
  /*
   *  The maintenance domain level of the configured MEP.
   *  Range: 0-7
   */
  PCP_OAM_ETH_MP_LEVEL md_level;
  /*
   *  MEP direction (up/down)
   */
  PCP_OAM_ETH_MEP_DIRECTION direction;
  /*
   *  Packet forwarding info.
   */
  PCP_OAM_ETH_ACC_MEP_FWD_INFO frwd_info;
  /*
   *  OAM performance monitoring configuration
   */
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO peformance_monitoring_info;

} PCP_OAM_ETH_ACC_MEP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Accelerated MEP index as defined by Procedure
   *  pcp_oam_eth_acc_mep_info_set(). Range: 0 - 4K
   */
  PCP_OAM_ETH_ACC_MEP_ID acc_mep_id;
  /*
   *  Remote MEP index as arrives on the CFM packet. Range: 0
   *  - 8K
   */
  uint32 rmep_id;

} PCP_OAM_ETH_RMEP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Interval between consecutive CCM messages. Used to calc
   *  CCM timeout for remote MEPs
   */
  PCP_OAM_ETH_INTERVAL interval;
  /*
   *  Remote MEP RDI state. T - Remote MEP is in RDI-set state.
   *  Suppress further RDI-set interrupts for this RMEP F -
   *  Remote MEP is in RDI-clear state. Suppress further
   *  RDI-clear interrupts for this RMEP
   */
  uint8 rdi_received;
  /*
   *  T - CCM session from this RMEP has timed out. When CPU
   *  sets this flag, timeout interrupts are suppressed. F -
   *  No timeout detected for this RMEP. If such occurs an
   *  interrupt will be asserted
   */
  uint8 local_defect;

} PCP_OAM_ETH_RMEP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Interval between consecutive DLM messages
   */
  PCP_OAM_ETH_INTERVAL interval;
  /*
   *  Destination MAC address of the DLM packet
   */
  SOC_SAND_PP_MAC_ADDRESS da;
  /*
   *  Determines the priority of the generated CFM packets
   */
  uint8 priority;

} PCP_OAM_ETH_ACC_DLM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Interval between consecutive LMM messages.
   */
  PCP_OAM_ETH_INTERVAL interval;
  /*
   *  LMM destination MAC address. Used when OAMP injects LMM
   *  packets.
   */
  SOC_SAND_PP_MAC_ADDRESS da;
  /*
   *  Determines the priority of the generated CFM packets
   */
  uint8 priority;
  /*
   *  11 msb bits of pointer to receive counters in statistics DB
   */
  uint32 rx_counter;
  /*
   *  11 msb bits of pointer to transmit counters in statistics DB
   */
  uint32 tx_counter;

} PCP_OAM_ETH_ACC_LMM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  As defined in ITU Y.1731
   */
  uint32 tx_fcf_curr;
  /*
   *  As defined in ITU Y.1731
   */
  uint32 tx_fcf_prev;
  /*
   *  As defined in ITU Y.1731
   */
  uint32 rx_fcb_curr;
  /*
   *  As defined in ITU Y.1731
   */
  uint32 rx_fcb_prev;

} PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Local MEP counter set
   */
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO local;
  /*
   *  Peer MEP counter set
   */
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO peer;

} PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO;


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
 *   pcp_oam_eth_general_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets general configuration of Ethernet
 *   OAM.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO        *info -
 *     General configuration information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_general_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO        *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_general_info_set" API.
 *     Refer to "pcp_oam_eth_general_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_eth_general_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_ETH_GENERAL_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_cos_mapping_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function maps the cos profile and the user priority
 *   (both are MEP attributes) to TC and DP for the ITMH
 *   (relevant for down MEP injection)
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx -
 *     Class of service. Range: 0-3
 *   SOC_SAND_IN  uint32                        prio_ndx -
 *     VLAN tag's user priority. Range: 0 - 7
 *   SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO    *info -
 *     COS mapping configuration information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_cos_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO    *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_cos_mapping_info_set" API.
 *     Refer to "pcp_oam_eth_cos_mapping_info_set" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_cos_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_COS_MAPPING_INFO    *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function is used to define a Maintenance Point (MP)
 *   on an Attachment-Circuit (incoming/outgoing), MD-level,
 *   and MP direction (up/down). If the MP is one of the 4K
 *   accelerated MEPs, the function configures the related
 *   OAMP databases and associates the AC, MD-Level and the
 *   MP type with a user-provided handle. This handle is
 *   later used by user to access OAMP database for this MEP.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key -
 *     Identifier of the key (direction, attachment-circuit,
 *     level) on which the MP is set.
 *   SOC_SAND_IN  PCP_OAM_ETH_MP_INFO             *info -
 *     MP configuration information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_mp_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO             *info
  );

/*********************************************************************
*     Gets the configuration set by the "pcp_oam_mp_info_set"
 *     API.
 *     Refer to "pcp_oam_mp_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_mp_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_OUT PCP_OAM_ETH_MP_INFO             *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_mep_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures an accelerated MEP.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mp_info_set()). Range:
 *     0-4K
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO        *info -
 *     Accelerated MEP configuration information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO        *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_mep_info_set" API.
 *     Refer to "pcp_oam_eth_acc_mep_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_mep_custom_header_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets user custom header.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mp_info_set()). Range:
 *     0-4K
 *   SOC_SAND_IN  uint32                        header -
 *     Placed at the beginning of the packet
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_custom_header_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint32                        header
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_mep_custom_header_set" API.
 *     Refer to "pcp_oam_eth_acc_mep_custom_header_set" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_custom_header_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT uint32                        *header
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_lmm_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets LMM info for the specified LMM
 *   identifier.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx -
 *     LMM user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_acc_mep_info_set()).
 *     Range: 0-127
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO        *info -
 *     LMM configuration setting
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_lmm_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO        *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_lmm_set" API.
 *     Refer to "pcp_oam_eth_acc_lmm_set" API for details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_lmm_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_LMM_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_dlm_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets DLM info for the specified DLM
 *   identifier.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx -
 *     DLM user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_acc_mep_info_set()).
 *     Range: 0-127
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO        *info -
 *     DLM configuration setting
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_dlm_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO        *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_dlm_set" API.
 *     Refer to "pcp_oam_eth_acc_dlm_set" API for details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_dlm_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_DLM_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_rmep_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function adds a remote MEP to list of remote MEPs
 *   associated with the accelerated MEP identified by
 *   acc_mep_ndx.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key -
 *     Remote MEP key consisting of the remote MEP id of the
 *     arriving CFM packet and the local accelerated MEP handle
 *     as determined by Procedure
 *     pcp_oam_eth_acc_mep_info_set()
 *   SOC_SAND_IN  uint32                        rmep -
 *     Handle to the remote MEP object that is monitored by the
 *     local MP. Range: 0 - 8K
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_rmep_add(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key,
    SOC_SAND_IN  uint32                        rmep
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_rmep_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function removes a remote MEP to list of remote
 *   MEPs associated with the accelerated MEP identified by
 *   acc_mep_ndx.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key -
 *     Remote MEP key consisting of the remote MEP id of the
 *     arriving CFM packet and the local accelerated MEP handle
 *     as determined by Procedure
 *     pcp_oam_eth_acc_mep_info_set()
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_rmep_remove(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_rmep_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This API sets a remote MEP info.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                        rmep_ndx -
 *     Remote MP to be monitored by the local MP. Range: 0 - 8K
 *   SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO           *rmep_info -
 *     Remote MEP configuration information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_rmep_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO           *rmep_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_rmep_info_set" API.
 *     Refer to "pcp_oam_eth_rmep_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_eth_rmep_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_RMEP_INFO           *rmep_info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_mep_tx_rdi_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets/clears remote defect indication on
 *   outgoing CCM messages
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mp_info_set()). Range:
 *     0-4K
 *   SOC_SAND_IN  uint8                       rdi -
 *     Remote defect indication T - Set RDI in outgoing CCM
 *     packetF - Do not set RDI in outgoing CCM packet
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_tx_rdi_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint8                       rdi
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_mep_tx_rdi_set" API.
 *     Refer to "pcp_oam_eth_acc_mep_tx_rdi_set" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_tx_rdi_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT uint8                       *rdi
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_mep_delay_measurement_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function returns the last measured delay. In case
 *   DM was not defined on the provided MEP, an error is
 *   thrown
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mep_info_set()). Range:
 *     0-127
 *   SOC_SAND_OUT uint32                        *dm -
 *     Last delay measurement
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_mep_delay_measurement_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_OUT uint32                        *dm
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_mep_loss_measurement_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function returns the loss measurement counters of
 *   the device (both local and remote). In case LM was not
 *   defined on the provided MEP, an error is thrown
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mep_info_set()). Range:
 *     0-127
 *   SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info -
 *     Loss measurement counters of the device (both local and
 *     remote)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_mep_loss_measurement_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  );

void
  PCP_OAM_ETH_GENERAL_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_GENERAL_INFO *info
  );

void
  PCP_OAM_ETH_COS_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_COS_MAPPING_INFO *info
  );

void
  PCP_OAM_ETH_MP_KEY_clear(
    SOC_SAND_OUT PCP_OAM_ETH_MP_KEY *info
  );

void
  PCP_OAM_ETH_MP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_MP_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_FWD_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_FWD_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_CCM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_CCM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_LMM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LMM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_DLM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_DLM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_INFO *info
  );

void
  PCP_OAM_ETH_RMEP_KEY_clear(
    SOC_SAND_OUT PCP_OAM_ETH_RMEP_KEY *info
  );

void
  PCP_OAM_ETH_RMEP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_RMEP_INFO *info
  );

void
  PCP_OAM_ETH_ACC_DLM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_DLM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_LMM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_LMM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  );

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_OAM_ETH_MEP_DIRECTION_to_string(
    SOC_SAND_IN  PCP_OAM_ETH_MEP_DIRECTION enum_val
  );

const char*
  PCP_OAM_ETH_INTERVAL_to_string(
    SOC_SAND_IN  PCP_OAM_ETH_INTERVAL enum_val
  );

void
  PCP_OAM_ETH_GENERAL_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO *info
  );

void
  PCP_OAM_ETH_COS_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO *info
  );

void
  PCP_OAM_ETH_MP_KEY_print(
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY *info
  );

void
  PCP_OAM_ETH_MP_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_FWD_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_CCM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_CCM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_LMM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LMM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_DLM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_DLM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO *info
  );

void
  PCP_OAM_ETH_RMEP_KEY_print(
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY *info
  );

void
  PCP_OAM_ETH_RMEP_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO *info
  );

void
  PCP_OAM_ETH_ACC_DLM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_LMM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO *info
  );

void
  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  );

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_OAM_API_ETH_INCLUDED__*/
#endif

