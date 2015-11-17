/* $Id: soc_petra_scheduler_device.h,v 1.5 Broadcom SDK $
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


#ifndef __SOC_PETRA_SCHEDULER_DEVICE_H_INCLUDED__
/* { */
#define __SOC_PETRA_SCHEDULER_DEVICE_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_scheduler_device.h,v 1.5 Broadcom SDK $
 * The device rate is determined by the interval between 2 consecutive credits
 * configured in 1/64th of a clock period units.
 */
#define SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_RESOLUTION (64)

/*
 * Minimum Device Rate Entry interval value (if enabled) is 64,
 * which results in 64 * (clock/64) = 1 clock (eq to ~250 Gbit/sec in 256 credit size)
 */
#define SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_MIN (SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_RESOLUTION)

/*
 * Maximum Device Rate Entry interval value (if enabled) is 262143,
 * which results in 262143 clock/64 = 4096 clock (eq to ~41 Mbit/sec in 256 credit size)
 */

#define SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_MAX (262143)

/*
 * Device interface weight - maximal value corresponds to lowest priority.
 * The weight  is in range 0 - 1023, where 0 means the weight is
 * disabled and 1 is the highest priority.
 */
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_MIN 0
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_MAX 1023

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
*     Converts from interface index as represented by
*     SOC_PETRA_INTERFACE_ID (when used in Soc_petra-A mode) or internal id
*     (SOC_PETRA_INTERFACE_ID in soc_petra-B mode converted to internal using
*     soc_pb_nif2intern_id), to the offset for the relevant
*     scheduler register (channelized or 1-port).
*********************************************************************/
uint32
  soc_petra_sch_if2sched_offset(
    uint32 nif_id
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_rate_entry_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function sets an entry in the device rate table.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver writes to
*     the following tables: Device Rate Memory (DRM)
* INPUT:
*  SOC_SAND_IN  uint32                 rci_level_ndx -
*     RCI bucket level. Range: 0 - 7
*  SOC_SAND_IN  uint32                 nof_active_links_ndx -
*     Number of current active links range: 0 - 36
*  SOC_SAND_IN  uint32                  rate -
*     The credit generation rate, in Mega-Bit-Sec. If 0 - no
*     credits are generated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_rate_entry_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_IN  uint32                  rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_rate_entry_verify
* TYPE:
*   PROC
* FUNCTION:
*     This function sets an entry in the device rate table.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver writes to
*     the following tables: Device Rate Memory (DRM)
* INPUT:
*  SOC_SAND_IN  uint32                 rci_level_ndx -
*     RCI bucket level. Range: 0 - 7
*  SOC_SAND_IN  uint32                 nof_active_links_ndx -
*     Number of current active links range: 0 - 36
*  SOC_SAND_IN  uint32                  rate -
*     The credit generation rate, in Mega-Bit-Sec. If 0 - no
*     credits are generated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_rate_entry_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_IN  uint32                  rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_rate_entry_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function sets an entry in the device rate table.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver writes to
*     the following tables: Device Rate Memory (DRM)
* INPUT:
*  SOC_SAND_IN  uint32                 rci_level_ndx -
*     RCI bucket level. Range: 0 - 7
*  SOC_SAND_IN  uint32                 nof_active_links_ndx -
*     Number of current active links range: 0 - 36
*  SOC_SAND_OUT uint32                  *rate -
*     The credit generation rate, in Mega-Bit-Sec. If 0 - no
*     credits are generated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_rate_entry_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_OUT uint32                  *rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_shaper_rate_set_unsafe
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
  soc_petra_sch_if_shaper_rate_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx,
    SOC_SAND_IN  uint32              if_rate,
    SOC_SAND_OUT uint32              *exact_if_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_shaper_rate_verify
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
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*   This function must only be called for Channelized interfaces.
*   For one-port-interfaces, including OLP and ERP, rate configuration
*   is done according to port_rate, as part of
*   egress port configuration.
*********************************************************************/
uint32
  soc_petra_sch_if_shaper_rate_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx,
    SOC_SAND_IN  uint32              if_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_shaper_rate_get_unsafe
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
  soc_petra_sch_if_shaper_rate_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT uint32                  *if_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_if_weight_idx_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_IN  uint32                  weight_index -
*     Interface weight index. Range: 0-7. Selects one of 8
*     configurable weights for interfaces WFQ.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_if_weight_idx_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx,
    SOC_SAND_IN  uint32                  weight_index
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_if_weight_idx_verify
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_IN  uint32                  weight_index -
*     Interface weight index. Range: 0-7. Selects one of 8
*     configurable weights for interfaces WFQ.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_if_weight_idx_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx,
    SOC_SAND_IN  uint32                  weight_index
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_device_if_weight_idx_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index, consists of interface type, and an
*     interface index for network interfaces.
*  SOC_SAND_OUT uint32                  *weight_index -
*     Interface weight index. Range: 0-7. Selects one of 8
*     configurable weights for interfaces WFQ.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_device_if_weight_idx_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx,
    SOC_SAND_OUT uint32                  *weight_index
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_weight_conf_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     This function sets the device interfaces scheduler
*     weight configuration. Up to 8 weight configuration can
*     be pre-configured. Each scheduler interface will be
*     configured to use one of these pre-configured weights.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_IF_WEIGHTS      *if_weights -
*     The weighs configuration for the device interfaces
*     scheduler.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_if_weight_conf_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_IF_WEIGHTS      *if_weights
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_weight_conf_verify
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     This function sets the device interfaces scheduler
*     weight configuration. Up to 8 weight configuration can
*     be pre-configured. Each scheduler interface will be
*     configured to use one of these pre-configured weights.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_IF_WEIGHTS      *if_weights -
*     The weighs configuration for the device interfaces
*     scheduler.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_if_weight_conf_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_IF_WEIGHTS      *if_weights
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_if_weight_conf_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec  2 2007
* FUNCTION:
*     This function sets the device interfaces scheduler
*     weight configuration. Up to 8 weight configuration can
*     be pre-configured. Each scheduler interface will be
*     configured to use one of these pre-configured weights.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_OUT SOC_PETRA_SCH_IF_WEIGHTS      *if_weights -
*     The weighs configuration for the device interfaces
*     scheduler.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_if_weight_conf_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SCH_IF_WEIGHTS      *if_weights
  );


/*********************************************************************
*     Sets, for a specified device MAC LANE or equivalent, (NIF-Ports,
*     recycling & CPU) its actual credit rate (sum of ports). This API is
*     only valid for Channelized interfaces - see REMARKS
*     section below.
*     Note: for SGMII - configures only the first interface in MAL
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_mal_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     uint32                mal_ndx,
    SOC_SAND_IN     uint32                rate,
    SOC_SAND_OUT    uint32                *exact_rate
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_SCHEDULER_DEVICE_H_INCLUDED__*/
#endif
