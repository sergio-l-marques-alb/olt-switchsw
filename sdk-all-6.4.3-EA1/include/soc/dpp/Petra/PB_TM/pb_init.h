/* $Id: pb_init.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_init.h
*
* MODULE PREFIX:  soc_petra_init
*
* FILE DESCRIPTION:
*  Soc_petra initialization
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PB_INIT_INCLUDED__
/* { */
#define __SOC_PB_INIT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>

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

uint32
  soc_pb_mgmt_ihb_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  soc_pb_mgmt_ihp_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  soc_pb_mgmt_egq_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  soc_pb_mgmt_epni_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  );

/*********************************************************************
* NAME:
*     soc_pb_mgmt_init_sequence_phase1_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Initialize the device, including:1. Prevent all the
*     control cells. 2. Initialize the device tables and
*     registers to default values. 3. Initialize
*     board-specific hardware interfaces according to
*     configurable information, as passed in 'hw_adjust'. 4.
*     Perform basic device initialization. The configuration
*     can be enabled/disabled as passed in 'enable_info'.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS      *hw_adjust -
*     Contains user-defined initialization information for
*     hardware interfaces.
*  SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF     *basic_conf -
*     Basic configuration that must be configured for all
*     systems - credit worth, dram buffers configuration etc.
*  SOC_SAND_IN  SOC_PB_INIT_PORTS          *fap_ports -
*     local FAP ports configuration - header parsing type,
*     mapping to NIF etc.
*  SOC_SAND_INOUT SOC_PB_INIT_DROP_AND_FC    *drp_and_fc -
*     local packet drop and flow control configurations -
*     ingress, egress, nif, scheduler etc.
*  SOC_SAND_IN  uint8                 silent -
*     If TRUE, progress printing will be suppressed.
* REMARKS:
*     1. For all configurations that can be done per-direction
*     (e.g. NIF - rx/tx, FAP - incoming/outgoing) - the
*     configuration is performed for both directions if
*     enabled. It may be overridden before phase2 if needed.
*     2. For all input structures, NULL pointer may be passed.
*     If input structure is passed as NULL, the appropriate
*     configuration will not be performed.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF     *basic_conf,
    SOC_SAND_IN  SOC_PB_INIT_PORTS          *fap_ports,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  soc_pb_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_INOUT  SOC_PB_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF     *basic_conf,
    SOC_SAND_IN  SOC_PB_INIT_PORTS          *fap_ports
  );

/*********************************************************************
* NAME:
*     soc_pb_mgmt_init_sequence_phase2_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Out-of-reset sequence. Enable/Disable the device from
*     receiving and transmitting control cells.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PB_INIT_OOR            *oor_info -
*     Out Of Reset configuration. Some blocks need to be set
*     out of reset before traffic can be enabled.
*  SOC_SAND_IN  uint8                 silent -
*     TRUE - Print progress messages. FALSE - Do not print
*     progress messages.
* REMARKS:
*     1. After phase 2 initialization, traffic can be enabled.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_INIT_OOR            *oor_info,
    SOC_SAND_IN  uint8                 silent
  );

/*********************************************************************
* NAME:
*     soc_pb_mgmt_hw_interfaces_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Initialize a sub-set of the HW interfaces of the device.
*     The function might be called more than once, each time
*     with different fields, indicated to be written to the
*     device
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS      *hw_adjust -
*     BSP related information, and a valid bit for each field.
*  SOC_SAND_IN  uint8                 silent -
*     If TRUE, all printing will be suppressed.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_hw_interfaces_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  soc_pb_mgmt_init_sequence_phase2_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_INIT_OOR            *oor_info
  );

/*********************************************************************
* NAME:
*     soc_pb_mgmt_hw_interfaces_verify
* TYPE:
*   PROC
* FUNCTION:
*     Initialize a sub-set of the HW interfaces of the device.
*     The function might be called more than once, each time
*     with different fields, indicated to be written to the
*     device
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS      *hw_adjust -
*     BSP related information, and a valid bit for each field.
*  SOC_SAND_IN  uint8                 silent -
*     If TRUE, all printing will be suppressed.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_hw_interfaces_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS      *hw_adjust
  );

uint32
  soc_pb_mgmt_hw_adjust_ddr_init(
    SOC_SAND_IN int          unit,
    SOC_SAND_IN uint32          instance_idx
  );

uint32
  soc_pb_mgmt_init_qdr_dll_mem_unsafe(
    int unit
  );
/* } */


uint32
  SOC_PB_INIT_OOR_verify(
    SOC_SAND_IN  SOC_PB_INIT_OOR *info
  );
#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_INIT_INCLUDED__*/
#endif
