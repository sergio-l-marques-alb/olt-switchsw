/* $Id: pb_pmf_low_level_diag.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_pmf_low_level_diag.h
*
* MODULE PREFIX:  soc_pb_pmf
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

#ifndef __SOC_PB_PMF_LOW_LEVEL_DIAG_INCLUDED__
/* { */
#define __SOC_PB_PMF_LOW_LEVEL_DIAG_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>


#include <soc/dpp/Petra/PB_TM/pb_api_general.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum size in longs of buffer                         */
#define  SOC_PB_PMF_DIAG_BUFF_MAX_SIZE (9)

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
   *  Action on the Destination field
   */
  SOC_PB_PMF_DIAG_FORCE_DEST = 0,
  /*
   *  Action on the Drop Precedence field
   */
  SOC_PB_PMF_DIAG_FORCE_TC = 1,
  /*
   *  Action on the DP field
   */
  SOC_PB_PMF_DIAG_FORCE_DP = 2,
  /*
   *  invalid action cannot be used
   */
  SOC_PB_PMF_DIAG_FORCE_INVALID = 3,
  /*
   *  Action on the DP-meter field
   */
  SOC_PB_PMF_DIAG_FORCE_DP_METER = 4,
  /*
   *  Action on the forward strength field
   */
  SOC_PB_PMF_DIAG_FORCE_FWD_STRENGTH = 5,
  /*
   *  Action on the Meter field
   */
  SOC_PB_PMF_DIAG_FORCE_METER_A = 6,
  /*
   *  Action on the Meter field
   */
  SOC_PB_PMF_DIAG_FORCE_METER_B = 7,
  /*
   *  Action on the Counter field
   */
  SOC_PB_PMF_DIAG_FORCE_COUNTER_A = 8,
  /*
   *  Action on the Counter field
   */
  SOC_PB_PMF_DIAG_FORCE_COUNTER_B = 9,
  /*
   *  Action on the trap code field
   */
  SOC_PB_PMF_DIAG_FORCE_CPU_TRAP_CODE = 10,
  /*
   *  Action on the snoop code
   */
  SOC_PB_PMF_DIAG_FORCE_SNOOP_CODE = 11,
  /*
   *  Action on the Outbound-Mirror-Disable field
   */
  SOC_PB_PMF_DIAG_FORCE_OUTBOUND_MIRROR_DIS = 12,
  /*
   *  Action on the Exclude-Source field
   */
  SOC_PB_PMF_DIAG_FORCE_EXCLUDE_SRC = 13,
  /*
   *  Action on the Ingress Shaping field
   */
  SOC_PB_PMF_DIAG_FORCE_INGRESS_SHAPING = 14,
  /*
   *  Action on the mirror action
   */
  SOC_PB_PMF_DIAG_FORCE_MIRROR_ACTION = 15,
  /*
   *  Action on the bytes to remove
   */
  SOC_PB_PMF_DIAG_FORCE_BYTES_TO_REMOVE = 16,
  /*
   *  Action on the Counter field
   */
  SOC_PB_PMF_DIAG_FORCE_HEADER_PROFILE_NDX = 17,
  /*
   *  Action on the sequence namber
   */
  SOC_PB_PMF_DIAG_FORCE_SEQ_NUM_TAG = 18,
  /*
   *  Action on the Statistic-Tag field
   */
  SOC_PB_PMF_DIAG_FORCE_STATISTICS_TAG = 19,
  /*
   *  Action on the LAG Load-Balancing field
   */
  SOC_PB_PMF_DIAG_FORCE_LB_KEY = 20,
  /*
   *  Action on the ECMP Load-Balancing field
   */
  SOC_PB_PMF_DIAG_FORCE_ECMP_KEY = 21,
  /*
   *  Action on the Stacking-Route-History field
   */
  SOC_PB_PMF_DIAG_FORCE_STACKING_ROUTE = 22,
  /*
   *  Action on the Out-LIF field
   */
  SOC_PB_PMF_DIAG_FORCE_OUTLIF = 23,
  /*
   *  No action
   */
  SOC_PB_PMF_DIAG_FORCE_NOP = 24,
  /*
   *  Number of types in SOC_PB_PMF_DIAG_ACTION_TYPE
   */
  SOC_PB_PMF_NOF_DIAG_FORCES = 25
}SOC_PB_PMF_DIAG_FORCE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Buffer used by diagnostics APIs to hold packets header
   *  and other information. Information/packet is copied to
   *  buff starting from buff[0] lsb.
   */
  uint32 buff[SOC_PB_PMF_DIAG_BUFF_MAX_SIZE];
  /*
   *  the actual length of the returned buffer (in bits)
   */
  uint32 buff_len;

} SOC_PB_PMF_DIAG_BUFFER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  force Action type done by the entry.
   */
  SOC_PB_PMF_DIAG_FORCE type;
  /*
   *  Action value, according to the action type. E.g.: When
   *  the type is Meter-update, and 'action_val' is 6, the
   *  action is attaching the packet to meter #6.
   */
  uint32 value;
  /*
   *  Is valid action value
   */
  uint8 is_valid;

} SOC_PB_PMF_DIAG_FORCE_ACTION_VAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Action type done by the entry.
   */
  SOC_PB_PMF_FEM_ACTION_TYPE type;
  /*
   *  Action value, according to the action type. E.g.: When
   *  the type is Meter-update, and 'action_val' is 6, the
   *  action is attaching the packet to meter #6.
   */
  uint32 value;
  /*
   *  Is valid action value
   */
  uint8 is_valid;

} SOC_PB_PMF_DIAG_ACTION_VAL;
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Ethernet key.
   */
  SOC_PB_PMF_DIAG_BUFFER eth;
  /*
   *  IPV4 key.
   */
  SOC_PB_PMF_DIAG_BUFFER ipv4;
  /*
   *  IPV6 key.
   */
  SOC_PB_PMF_DIAG_BUFFER ipv6;
  /*
   *  Custom Key-A.
   */
  SOC_PB_PMF_DIAG_BUFFER custom_a;
  /*
   *  Custom Key-B.
   */
  SOC_PB_PMF_DIAG_BUFFER custom_b;

} SOC_PB_PMF_DIAG_KEYS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Actions to perform, set action type to 'NOP' to
   *  invalidate action.
   */
  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL actions[4];
  /*
   *  Forwarding strength relevant when action type is one of
   *  the following:- SOC_PB_PMF_FEM_ACTION_TYPE_DEST-
   *  SOC_PB_PMF_FEM_ACTION_TYPE_DP- SOC_PB_PMF_FEM_ACTION_TYPE_TC-
   *  SOC_PB_PMF_FEM_ACTION_TYPE_TRAP- SOC_PB_PMF_FEM_ACTION_TYPE_SNP
   */
  uint32 strength;

} SOC_PB_PMF_DIAG_ACTION_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  TRUE then the FEM is ready for freeze
   */
  uint8 freeze;
  /*
   *  Pass to freeze FEMs on. can be first/second. Range: 0 -
   *  1.
   */
  uint32 pass_num;
  /*
   *  Program to freeze FEMs no Range: 0-7.
   */
  uint32 prg;
  /*
   *  FALSE then the FEM is freeze for any programs
   */
  uint8 is_prg_valid;

} SOC_PB_PMF_DIAG_FEM_FREEZE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Input key for FEM.
   */
  uint32 in_key;
  /*
   *  Input Program can be Range: 0-7.
   */
  uint32 in_prg;
  /*
   *  Output action
   */
  SOC_PB_PMF_DIAG_ACTION_VAL out_action;

} SOC_PB_PMF_DIAG_FEM_INFO;


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
 *   soc_pb_pmf_diag_force_prog_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Force PMF to execute the given program.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                 prog_id -
 *     Program ID to enforce. Relevant only when enable=TRUE.
 *     Range: 0 - 31. (Soc_petra-B)
 *   SOC_SAND_IN  uint8                enable -
 *     Program force enabled. If TRUE, then the PMF will execute
 *     program-id == prog_id. If FALSE, then the PMF will
 *     execute the program selected according to
 *     'program-selection'.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_prog_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                 prog_id,
    SOC_SAND_IN  uint8                enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_diag_force_prog_set" API.
 *     Refer to "soc_pb_pmf_diag_force_prog_set" API for details.
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_prog_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT uint32                 *prog_id,
    SOC_SAND_OUT uint8                *enable
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_selected_progs_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns used programs for all packets since last call.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT uint32                 *progs_bmp -
 *     Selected programs as bitmap
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_diag_selected_progs_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT uint32                 *progs_bmp
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_force_action_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Force PMF to perform the given action.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO  *action_info -
 *     type to SOC_PB_PMF_FEM_ACTION_TYPE_NOP
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_action_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO  *action_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_diag_force_action_set" API.
 *     Refer to "soc_pb_pmf_diag_force_action_set" API for details.
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_action_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_ACTION_INFO  *action_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_built_keys_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the keys built by the PMF for the last packet
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PMF_DIAG_KEYS_INFO    *built_keys -
 *     SOC_SAND_OUT SOC_PB_PMF_DIAG_KEYS_INFO *built_keys
 * REMARKS:
 *   - Diag Type: last packet.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_diag_built_keys_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_KEYS_INFO    *built_keys
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_fem_freeze_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set pre-configuration so freeze inputs/outputs will be
 *   snapshoot according to this information
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info -
 *     Freeze information including which pass to freeze, and
 *     for which program.
 * REMARKS:
 *   - use soc_pb_pmf_diag_fems_info_get to retrieve FEM
 *   information
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_diag_fem_freeze_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_diag_fem_freeze_set" API.
 *     Refer to "soc_pb_pmf_diag_fem_freeze_set" API for details.
*********************************************************************/
uint32
  soc_pb_pmf_diag_fem_freeze_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_fems_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the FEM information including inputs and outputs to
 *   each FEM,
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   release -
 *     If true then FEMs are released from the previous freeze,
 *     otherwise FEMs stay in freeze with the previous
 *     configured information.
 *   SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_INFO        fems_info -
 *     Input/output of each FEM.
 * REMARKS:
 *   - if soc_pb_pmf_diag_fem_freeze_set() was called previously
 *   then the returned information is for first packet pass
 *   after soc_pb_pmf_diag_fem_freeze_set() call.- otherwise then
 *   the returned information is for last packet.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_diag_fems_info_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint8                   release,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_INFO        fems_info[8]
  );

void
  SOC_PB_PMF_DIAG_BUFFER_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_BUFFER *info
  );

void
  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FORCE_ACTION_VAL *info
  );

void
  SOC_PB_PMF_DIAG_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_ACTION_VAL *info
  );

void
  SOC_PB_PMF_DIAG_KEYS_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_KEYS_INFO *info
  );

void
  SOC_PB_PMF_DIAG_ACTION_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_ACTION_INFO *info
  );

void
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *info
  );

void
  SOC_PB_PMF_DIAG_FEM_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PMF_DIAG_FORCE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FORCE enum_val
  );

void
  SOC_PB_PMF_DIAG_BUFFER_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_BUFFER *info
  );

void
  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL *info
  );

void
  SOC_PB_PMF_DIAG_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_VAL *info
  );

void
  SOC_PB_PMF_DIAG_KEYS_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_KEYS_INFO *info
  );

void
  SOC_PB_PMF_DIAG_ACTION_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO *info
  );

void
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *info
  );

void
  SOC_PB_PMF_DIAG_FEM_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PMF_LOW_LEVEL_DIAG_INCLUDED__*/
#endif

