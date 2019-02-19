/* $Id: arad_diagnostics.h,v 1.7 Broadcom SDK $
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

#ifndef __ARAD_DIAGNOSTICS_INCLUDED__
/* { */
#define __ARAD_DIAGNOSTICS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>
#include <soc/dpp/ARAD/arad_api_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_api_egr_queuing.h>
#include <soc/dpp/ARAD/arad_api_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_general.h>

#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/* length of debug value */
#define ARAD_DIAG_DBG_VAL_LEN        (24)

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

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Register name of the register this field belongs to.
   */
  uint32 base;
  /*
   *  Field Most Significant Bit in the register.
   */
  uint32 msb;
  /*
   *  Field Least Significant Bit in the register.
   */
  uint32 lsb;

} ARAD_DIAG_REG_FIELD;

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
 *   arad_diag_last_packet_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns diagnostics information regarding the last
 *   packet: the incoming TM port and the corresponding PP
 *   port, port header processing type, packet headers and
 *   payload (first 128 Bytes). In case of TM port, the ITMH,
 *   which is part of that buffer, is parsed.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_DIAG_LAST_PACKET_INFO *last_packet -
 *     Fields of the last packet.
 * REMARKS:
 *   1. If the packet is processed with ingress shaping, then
 *   the returned ITMH corresponds to the one of the ingress
 *   shaping2. This API does not retrieve PP-related
 *   information and does not parse PP-headers, e.g. Ethernet
 *   header in the case of Ethernet port. For this reason,
 *   this API targets mainly TM ports diagnostics. For PP
 *   diagnostics, use the PPD API 'received_packet_info_get'
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_diag_last_packet_info_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT ARAD_DIAG_LAST_PACKET_INFO *last_packet
  );

uint32
  arad_diag_last_packet_info_get_verify(
    SOC_SAND_IN  int                unit
  );


/*********************************************************************
* NAME:
 *   arad_diag_sample_enable_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable diagnostic APIs.affects only APIs with
 *   type: need_sample
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                               enable -
 *     TRUE: diag APIs are enabled, FALSE diag APIs are
 *     disabled.
 * REMARKS:
 *   - when enabled will affect device power consuming
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint8                               enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_diag_sample_enable_get_unsafe" API.
 *     Refer to "arad_diag_sample_enable_get_unsafe" API for details.
*********************************************************************/
uint32
  arad_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT uint8                                *enable
  );

#if ARAD_DEBUG

/*********************************************************************
* NAME:
*     arad_diag_signals_dump_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     dump signals from the device for last packet
* INPUT:
*  SOC_SAND_IN  uint32   flags -
* REMARKS:
*  has to call to arad_diag_sample_enable_set_unsafe() with enable = true.
*  before calling this API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  arad_diag_signals_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   flags
  );

uint32
  ARAD_DIAG_LAST_PACKET_INFO_verify(
    SOC_SAND_IN  ARAD_DIAG_LAST_PACKET_INFO *info
  );


#endif /* ARAD_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_DIAGNOSTICS_INCLUDED__*/
#endif



