/* $Id: soc_pb_pmf_low_level_diag_intern.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_PMF_LOW_LEVEL_DIAG_INTERN_INCLUDED__
/* { */
#define __SOC_PB_PMF_LOW_LEVEL_DIAG_INTERN_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_diag.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>

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
 *   soc_pb_pmf_diag_force_prog_set_unsafe
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
  soc_pb_pmf_diag_force_prog_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                 prog_id,
    SOC_SAND_IN  uint8                enable
  );

uint32
  soc_pb_pmf_diag_force_prog_set_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                 prog_id,
    SOC_SAND_IN  uint8                enable
  );

uint32
  soc_pb_pmf_diag_force_prog_get_verify(
    SOC_SAND_IN  int                unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_diag_force_prog_set_unsafe" API.
 *     Refer to "soc_pb_pmf_diag_force_prog_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_prog_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT uint32                 *prog_id,
    SOC_SAND_OUT uint8                *enable
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_selected_progs_get_unsafe
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
  soc_pb_pmf_diag_selected_progs_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT uint32                 *progs_bmp
  );

uint32
  soc_pb_pmf_diag_selected_progs_get_verify(
    SOC_SAND_IN  int                unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_force_action_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Force PMF to perform the given action.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO  *action_info -
 *     type to SOC_PB_FP_ACTION_TYPE_NOP
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_action_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO  *action_info
  );

uint32
  soc_pb_pmf_diag_force_action_set_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO  *action_info
  );

uint32
  soc_pb_pmf_diag_force_action_get_verify(
    SOC_SAND_IN  int                unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_diag_force_action_set_unsafe" API.
 *     Refer to "soc_pb_pmf_diag_force_action_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_action_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_ACTION_INFO  *action_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_built_keys_get_unsafe
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
  soc_pb_pmf_diag_built_keys_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_KEYS_INFO    *built_keys
  );

uint32
  soc_pb_pmf_diag_built_keys_get_verify(
    SOC_SAND_IN  int                unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_fem_freeze_set_unsafe
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
  soc_pb_pmf_diag_fem_freeze_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  );

uint32
  soc_pb_pmf_diag_fem_freeze_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  );

uint32
  soc_pb_pmf_diag_fem_freeze_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_diag_fem_freeze_set_unsafe" API.
 *     Refer to "soc_pb_pmf_diag_fem_freeze_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pmf_diag_fem_freeze_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_diag_fems_info_get_unsafe
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
 *   SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_INFO        *fems_info -
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
  soc_pb_pmf_diag_fems_info_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint8                   release,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_INFO        fems_info[8]
  );

uint32
  soc_pb_pmf_diag_fems_info_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint8                   release
  );

uint32
  SOC_PB_PMF_DIAG_BUFFER_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_BUFFER *info
  );

uint32
  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL *info
  );

uint32
  SOC_PB_PMF_DIAG_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_VAL *info
  );

uint32
  SOC_PB_PMF_DIAG_KEYS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_KEYS_INFO *info
  );

uint32
  SOC_PB_PMF_DIAG_ACTION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO *info
  );

uint32
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *info
  );

uint32
  SOC_PB_PMF_DIAG_FEM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PMF_LOW_LEVEL_DIAG_INTERN_INCLUDED__*/
#endif

