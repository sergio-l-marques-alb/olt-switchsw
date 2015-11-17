/* $Id: pb_api_action_cmd.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_api_action_cmd.h
*
* MODULE PREFIX:  soc_pb_action
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

#ifndef __SOC_PB_API_ACTION_CMD_INCLUDED__
/* { */
#define __SOC_PB_API_ACTION_CMD_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/TMC/tmc_api_action_cmd.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

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

#define SOC_PB_ACTION_CMD_SIZE_BYTES_64                        SOC_TMC_ACTION_CMD_SIZE_BYTES_64
#define SOC_PB_ACTION_CMD_SIZE_BYTES_128                       SOC_TMC_ACTION_CMD_SIZE_BYTES_128
#define SOC_PB_ACTION_CMD_SIZE_BYTES_192                       SOC_TMC_ACTION_CMD_SIZE_BYTES_192
#define SOC_PB_ACTION_CMD_SIZE_BYTES_ALL_PCKT                  SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT
#define SOC_PB_ACTION_NOF_CMD_SIZE_BYTESS                      SOC_TMC_ACTION_NOF_CMD_SIZE_BYTESS
typedef SOC_TMC_ACTION_CMD_SIZE_BYTES                          SOC_PB_ACTION_CMD_SIZE_BYTES;

typedef SOC_TMC_ACTION_CMD_OVERRIDE                            SOC_PB_ACTION_CMD_OVERRIDE;
typedef SOC_TMC_ACTION_CMD                                     SOC_PB_ACTION_CMD;
typedef SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO                          SOC_PB_ACTION_CMD_SNOOP_INFO;
typedef SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO                         SOC_PB_ACTION_CMD_MIRROR_INFO;
typedef SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO                   SOC_PB_ACTION_CMD_SNOOP_MIRROR_INFO;

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
 *   soc_pb_action_cmd_snoop_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set a snoop action profile in the snoop action profile
 *   table.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              action_ndx -
 *     Action profile index in the snoop action profile table.
 *     The action profile is relevant for packets with a snoop
 *     action field of the same value. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PB_ACTION_CMD_SNOOP_INFO               *info -
 *     Snoop action profile parameters
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_action_cmd_snoop_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_IN  SOC_PB_ACTION_CMD_SNOOP_INFO               *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_action_cmd_snoop_set" API.
 *     Refer to "soc_pb_action_cmd_snoop_set" API for details.
*********************************************************************/
uint32
  soc_pb_action_cmd_snoop_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_OUT SOC_PB_ACTION_CMD_SNOOP_INFO               *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_action_cmd_mirror_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set a mirror action profile in the mirror action profile
 *   table.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              action_ndx -
 *     Action profile index in the mirror action profile table.
 *     Relevant for packets with a mirror action field of the
 *     same value. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PB_ACTION_CMD_MIRROR_INFO              *info -
 *     Mirror action profile parameters.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_action_cmd_mirror_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_IN  SOC_PB_ACTION_CMD_MIRROR_INFO              *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_action_cmd_mirror_set" API.
 *     Refer to "soc_pb_action_cmd_mirror_set" API for details.
*********************************************************************/
uint32
  soc_pb_action_cmd_mirror_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_OUT SOC_PB_ACTION_CMD_MIRROR_INFO              *info
  );

void
  SOC_PB_ACTION_CMD_OVERRIDE_clear(
    SOC_SAND_OUT SOC_PB_ACTION_CMD_OVERRIDE *info
  );

void
  SOC_PB_ACTION_CMD_clear(
    SOC_SAND_OUT SOC_PB_ACTION_CMD *info
  );

void
  SOC_PB_ACTION_CMD_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_PB_ACTION_CMD_SNOOP_INFO *info
  );

void
  SOC_PB_ACTION_CMD_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_PB_ACTION_CMD_MIRROR_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_ACTION_CMD_SIZE_BYTES_to_string(
    SOC_SAND_IN  SOC_PB_ACTION_CMD_SIZE_BYTES enum_val
  );

void
  SOC_PB_ACTION_CMD_OVERRIDE_print(
    SOC_SAND_IN  SOC_PB_ACTION_CMD_OVERRIDE *info
  );

void
  SOC_PB_ACTION_CMD_print(
    SOC_SAND_IN  SOC_PB_ACTION_CMD *info
  );

void
  SOC_PB_ACTION_CMD_SNOOP_INFO_print(
    SOC_SAND_IN  SOC_PB_ACTION_CMD_SNOOP_INFO *info
  );

void
  SOC_PB_ACTION_CMD_MIRROR_INFO_print(
    SOC_SAND_IN  SOC_PB_ACTION_CMD_MIRROR_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_ACTION_CMD_INCLUDED__*/
#endif
