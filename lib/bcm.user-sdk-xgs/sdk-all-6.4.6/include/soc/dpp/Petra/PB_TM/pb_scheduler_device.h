/* $Id: pb_scheduler_device.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_pb_scheduler_device.h
*
* MODULE PREFIX:  soc_pb_scheduler_device
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


#ifndef __SOC_PB_SCHEDULER_DEVICE_H_INCLUDED__
/* { */
#define __SOC_PB_SCHEDULER_DEVICE_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
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
*     soc_pb_sch_if_shaper_rate_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling & CPU) its maximal credit rate. This API is
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS
*     section below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_IN  uint32                  if_rate -
*     Maximum credit rate in Kilo-Bit-Sec.
*  SOC_SAND_OUT uint32                  *exact_if_rate -
*     Loaded with the actual written values. These can differ
*     from the given values due to rounding.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*   This function must only be called for Channelized interfaces.
*   For one-port-interfaces, including OLP and ERP, rate configuration
*   is done according to port_rate, as part of
*   egress port configuration.
*********************************************************************/
uint32
  soc_pb_sch_if_shaper_rate_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx,
    SOC_SAND_IN  uint32              if_rate,
    SOC_SAND_OUT uint32              *exact_if_rate
  );

/*********************************************************************
* NAME:
*     soc_pb_sch_if_shaper_rate_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling & CPU) its maximal credit rate. This API is
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS
*     section below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_OUT uint32                  *if_rate -
*     Maximum credit rate in Kilo-Bit-Sec.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*   This function must only be called for Channelized interfaces.
*   For one-port-interfaces, including OLP and ERP, rate configuration
*   is done according to port_rate, as part of
*   egress port configuration.
*********************************************************************/
uint32
  soc_pb_sch_if_shaper_rate_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT uint32                  *if_rate
  );

uint32
  soc_pb_sch_mal_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     uint32                mal_ndx,
    SOC_SAND_IN     uint32                rate,
    SOC_SAND_OUT    uint32                *exact_rate
  );

uint32
  soc_pb_sch_mal_rate_get_unsafe(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     uint32            mal_ndx,
    SOC_SAND_OUT    uint32            *rate
  );

uint32
  soc_pb_sch_device_if_weight_idx_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT uint32                  *weight_index
  );
/* } */

uint32
  soc_pb_sch_device_if_weight_idx_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  uint32                  weight_index
  );
#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_SCHEDULER_DEVICE_H_INCLUDED__*/
#endif
