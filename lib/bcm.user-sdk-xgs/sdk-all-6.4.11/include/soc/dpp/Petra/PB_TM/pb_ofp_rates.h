/* $Id: soc_pb_ofp_rates.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_OFP_RATES_INCLUDED__
/* { */
#define __SOC_PB_OFP_RATES_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_ofp_rates.h>

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

typedef enum
{
  SOC_PB_OFP_RATES_EGQ_MAL_00   =  0,
  SOC_PB_OFP_RATES_EGQ_MAL_01   =  1,
  SOC_PB_OFP_RATES_EGQ_MAL_02   =  2,
  SOC_PB_OFP_RATES_EGQ_MAL_03   =  3,
  SOC_PB_OFP_RATES_EGQ_MAL_04   =  4,
  SOC_PB_OFP_RATES_EGQ_MAL_05   =  5,
  SOC_PB_OFP_RATES_EGQ_MAL_06   =  6,
  SOC_PB_OFP_RATES_EGQ_MAL_07   =  7,
  SOC_PB_OFP_RATES_EGQ_MAL_08   =  8,
  SOC_PB_OFP_RATES_EGQ_MAL_09   =  9,
  SOC_PB_OFP_RATES_EGQ_MAL_10   = 10,
  SOC_PB_OFP_RATES_EGQ_MAL_11   = 11,
  SOC_PB_OFP_RATES_EGQ_MAL_12   = 12,
  SOC_PB_OFP_RATES_EGQ_MAL_13   = 13,
  SOC_PB_OFP_RATES_EGQ_MAL_14   = 14,
  SOC_PB_OFP_RATES_EGQ_MAL_15   = 15,
  SOC_PB_OFP_RATES_EGQ_MAL_CPU  = 16,
  SOC_PB_OFP_RATES_EGQ_MAL_RCY  = 17,
  SOC_PB_OFP_RATES_EGQ_NOF_MALS = 18,
  SOC_PB_OFP_RATES_EGQ_MAL_INVALID = 0xff
}SOC_PB_OFP_RATES_EGQ_MAL_ID;

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

/* $Id: soc_pb_ofp_rates.h,v 1.5 Broadcom SDK $
 *  Get the MAL index or equivalent of the MAL
 *  to which the given port is mapped at the egress
 */
uint32
  soc_pb_ofp_rates_port_mal_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID ofp_ndx,
    SOC_SAND_OUT uint32         *mal_ndx
  );

uint32
  soc_pb_ofp_rates_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_rates_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures Outgoing FAP Ports (OFP) rates, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     calculates from the given table the calendars
*     granularity, writes it to the device and changes the
*     active calendars. It also saves the values in the
*     software database for single-entry changes in the
*     future. For ports mapped to Network Interfaces, The
*     configuration is per MAC Lane. This indicates a single
*     interface for SPAUI/XAUI, or 4 SGMII interfaces.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC lane index for network interfaces, or equivalent.
*     Range: 0 - 7 for NIF, SOC_PETRA_MAL_ID_CPU,
*     SOC_PETRA_MAL_ID_RCY, SOC_PETRA_MAL_ID_OLP, SOC_PETRA_MAL_ID_ERP.
*  SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper -
*     MAL-level shaper can be updated according to the
*     accumulated ports rate on the MAL, or according to a
*     specified override value. Alternatively - MAL-level
*     shaper configuration may be not changed by the API, and
*     may be changed via dedicated API
*     (soc_petra_ofp_rates_mal_shaper_set).
*  SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl -
*     Table with per-port configuration for all OFP-s - rate
*     and burst.
*  SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *exact_shaper -
*     The actual values may be different due to rounding.
*  SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO  *exact_ofp_rate_tbl -
*     Exact values may vary due to rounding.
* REMARKS:
*     1. The API verifies that all the OFP-s are mapped to the
*     MAL (or equivalent for non-network interfaces) as
*     specified by mal_ndx. If not - the API exits with error
*     indication. The mapping is performed via dedicated API -
*     soc_pb_port_to_interface_map_set, prior to calling this
*     API. 2. Ports mapped to non-channelized ports belonging
*     to the same MAL (MAC Lane) share a single egress
*     calendar. The reason for this is that MAC Lanes together
*     with the SerDeses are the basic resources of the Soc_petra
*     Network Interface. For example, 4 non-channelized NIF-s
*     with indexes 0-3 areshaped using a common calendar. The
*     calendar that is used is the calendar used for shaping
*     FAP ports mapped to channelized NIF with index 0.3.
*     MAL-level shaper rate is typically identical to the
*     accumulated ports rate on the MAL. In this case, shaper
*     update mode may be set to SUM_OF_PORTS, and the
*     MAL-level shaper/burst rate is derived from accumulated
*     port-level configuration. In some cases, it is useful to
*     set the shaper rate to a lower value - e.g. if overall
*     (MAL-level) allowed port rate is bigger then the
*     accumulated port rate (port-level oversubscribtion). 4.
*     End-to-end scheduler also has per-interface shaper. This
*     shaper is configured in the end-to-end scheduler module.
*     5. This API relys on NIF type configuration (as appears
*     in soc_petra_nif_mal_basic_conf_set API). If NIF type is
*     changed on-the-fly, OFP rates must be re-configured.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_rates_verify
* TYPE:
*   PROC
* FUNCTION:
*     Configures Outgoing FAP Ports (OFP) rates, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     calculates from the given table the calendars
*     granularity, writes it to the device and changes the
*     active calendars. It also saves the values in the
*     software database for single-entry changes in the
*     future. For ports mapped to Network Interfaces, The
*     configuration is per MAC Lane. This indicates a single
*     interface for SPAUI/XAUI, or 4 SGMII interfaces.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC lane index for network interfaces, or equivalent.
*     Range: 0 - 7 for NIF, SOC_PETRA_MAL_ID_CPU,
*     SOC_PETRA_MAL_ID_RCY, SOC_PETRA_MAL_ID_OLP, SOC_PETRA_MAL_ID_ERP.
*  SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper -
*     MAL-level shaper can be updated according to the
*     accumulated ports rate on the MAL, or according to a
*     specified override value. Alternatively - MAL-level
*     shaper configuration may be not changed by the API, and
*     may be changed via dedicated API
*     (soc_petra_ofp_rates_mal_shaper_set).
*  SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl -
*     Table with per-port configuration for all OFP-s - rate
*     and burst.
* REMARKS:
*     1. The API verifies that all the OFP-s are mapped to the
*     MAL (or equivalent for non-network interfaces) as
*     specified by mal_ndx. If not - the API exits with error
*     indication. The mapping is performed via dedicated API -
*     soc_pb_port_to_interface_map_set, prior to calling this
*     API. 2. Ports mapped to non-channelized ports belonging
*     to the same MAL (MAC Lane) share a single egress
*     calendar. The reason for this is that MAC Lanes together
*     with the SerDeses are the basic resources of the Soc_petra
*     Network Interface. For example, 4 non-channelized NIF-s
*     with indexes 0-3 areshaped using a common calendar. The
*     calendar that is used is the calendar used for shaping
*     FAP ports mapped to channelized NIF with index 0.3.
*     MAL-level shaper rate is typically identical to the
*     accumulated ports rate on the MAL. In this case, shaper
*     update mode may be set to SUM_OF_PORTS, and the
*     MAL-level shaper/burst rate is derived from accumulated
*     port-level configuration. In some cases, it is useful to
*     set the shaper rate to a lower value - e.g. if overall
*     (MAL-level) allowed port rate is bigger then the
*     accumulated port rate (port-level oversubscribtion). 4.
*     End-to-end scheduler also has per-interface shaper. This
*     shaper is configured in the end-to-end scheduler module.
*     5. This API relys on NIF type configuration (as appears
*     in soc_petra_nif_mal_basic_conf_set API). If NIF type is
*     changed on-the-fly, OFP rates must be re-configured.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_rates_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures Outgoing FAP Ports (OFP) rates, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     calculates from the given table the calendars
*     granularity, writes it to the device and changes the
*     active calendars. It also saves the values in the
*     software database for single-entry changes in the
*     future. For ports mapped to Network Interfaces, The
*     configuration is per MAC Lane. This indicates a single
*     interface for SPAUI/XAUI, or 4 SGMII interfaces.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC lane index for network interfaces, or equivalent.
*     Range: 0 - 7 for NIF, SOC_PETRA_MAL_ID_CPU,
*     SOC_PETRA_MAL_ID_RCY, SOC_PETRA_MAL_ID_OLP, SOC_PETRA_MAL_ID_ERP.
*  SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper -
*     MAL-level shaper can be updated according to the
*     accumulated ports rate on the MAL, or according to a
*     specified override value. Alternatively - MAL-level
*     shaper configuration may be not changed by the API, and
*     may be changed via dedicated API
*     (soc_petra_ofp_rates_mal_shaper_set).
*  SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl -
*     Table with per-port configuration for all OFP-s - rate
*     and burst.
* REMARKS:
*     1. The API verifies that all the OFP-s are mapped to the
*     MAL (or equivalent for non-network interfaces) as
*     specified by mal_ndx. If not - the API exits with error
*     indication. The mapping is performed via dedicated API -
*     soc_pb_port_to_interface_map_set, prior to calling this
*     API. 2. Ports mapped to non-channelized ports belonging
*     to the same MAL (MAC Lane) share a single egress
*     calendar. The reason for this is that MAC Lanes together
*     with the SerDeses are the basic resources of the Soc_petra
*     Network Interface. For example, 4 non-channelized NIF-s
*     with indexes 0-3 areshaped using a common calendar. The
*     calendar that is used is the calendar used for shaping
*     FAP ports mapped to channelized NIF with index 0.3.
*     MAL-level shaper rate is typically identical to the
*     accumulated ports rate on the MAL. In this case, shaper
*     update mode may be set to SUM_OF_PORTS, and the
*     MAL-level shaper/burst rate is derived from accumulated
*     port-level configuration. In some cases, it is useful to
*     set the shaper rate to a lower value - e.g. if overall
*     (MAL-level) allowed port rate is bigger then the
*     accumulated port rate (port-level oversubscribtion). 4.
*     End-to-end scheduler also has per-interface shaper. This
*     shaper is configured in the end-to-end scheduler module.
*     5. This API relys on NIF type configuration (as appears
*     in soc_petra_nif_mal_basic_conf_set API). If NIF type is
*     changed on-the-fly, OFP rates must be re-configured.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  );

uint32
  soc_pb_ofp_all_ofp_rates_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 get_exact,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_rates_single_port_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures a single Outgoing FAP Port (OFP) rate, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     re-calculates the appropriate values from the current
*     values and the updated info. It also saves the values in
*     the software database for single-entry changes in the
*     future. For Network Interfaces, The configuration is per
*     MAC Lane. This indicates a single interface for
*     SPAUI/XAUI, or 4 SGMII interfaces.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 ofp_ndx -
*     Outgoing FAP Port index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO       *ofp_rate -
*     Single OFP - rate and burst.
* REMARKS:
*     1. All OFP-s belonging to the MAL (or non-network
*     interface equivalent) must be configured at least once
*     prior to calling this API.2. Ports mapped to
*     non-channelized ports belonging to the same MAL (MAC
*     Lane) share a single calendar. The reason for this is
*     that MAC Lanes together with the SerDeses are the basic
*     resources of the Soc_petra Network Interface. For example, 4
*     non-channelized NIF-s with indexes 0-3 areshaped using a
*     common calendar. The calendar that is used is the
*     calendar used for shaping FAP ports mapped to
*     channelized NIF with index 0.3. MAL-level shaper rate is
*     typically identical to the accumulated ports rate on the
*     MAL. In this case, shaper update mode may be set to
*     SUM_OF_PORTS, and the MAL-level shaper/burst rate is
*     derived from accumulated port-level configuration. In
*     some cases, it is useful to set the shaper rate to a
*     lower value - e.g. if overall (MAL-level) allowed port
*     rate is bigger then the accumulated port rate
*     (port-level oversubscribtion). 4. End-to-end scheduler
*     also has per-interface shaper. This shaper is configured
*     in the end-to-end scheduler module. 5. This API relys on
*     NIF type configuration (as appears in
*     soc_pb_nif_mal_basic_conf_set API). If NIF type is
*     changed on-the-fly, OFP rates must be re-configured.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_single_port_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO          *ofp_rate
  );

uint32
  soc_pb_ofp_rates_single_port_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO       *ofp_rate
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_rates_single_port_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Get a single Outgoing FAP Port (OFP) rate/burst
*     configuration.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 ofp_ndx -
*     SOC_SAND_IN uint32 ofp_ndx
*  SOC_SAND_OUT SOC_PETRA_OFP_RATE_INFO       *ofp -
*     ndx - Outgoing FAP Port index. Range: 0 - 79. SOC_SAND_IN
*     SOC_PETRA_OFP_RATES_TBL_INFO *ofp_rate - Single OFP - rate
*     and burst.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_single_port_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATE_INFO       *ofp
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_rates_mal_shaper_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configure MAL-level shaping. This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index or non-network interface MAL equivalent.
*     Range: 0 - 7 for Network Interfaces. For non-network
*     interfaces use the MAL equivalents: SOC_PETRA_MAL_ID_CPU,
*     SOC_PETRA_MAL_ID_OLP, SOC_PETRA_MAL_ID_RCY.
*  SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper -
*     MAC Lane-level shaper - rate and burst.
*  SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *exact_shaper -
*     The actual values may be different due to rounding.
* REMARKS:
*     1. MAL-level shaper rate is typically identical to the
*     accumulated ports rate on the MAL. In this case,
*     soc_pb_ofp_rates_set/update may be used to configure the
*     MAL-level shaper/burst rate derived from accumulated
*     port-level configuration. In some cases, it is useful to
*     set the shaper rate to a lower value - e.g. if overall
*     (MAL-level) allowed port rate is bigger then the
*     accumulated port rate (port-level oversubscribtion). In
*     these cases, this API can be use.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_mal_shaper_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *exact_shaper
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_rates_mal_shaper_verify
* TYPE:
*   PROC
* FUNCTION:
*     Configure MAL-level shaping. This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index or non-network interface MAL equivalent.
*     Range: 0 - 7 for Network Interfaces. For non-network
*     interfaces use the MAL equivalents: SOC_PETRA_MAL_ID_CPU,
*     SOC_PETRA_MAL_ID_OLP, SOC_PETRA_MAL_ID_RCY.
*  SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper -
*     MAC Lane-level shaper - rate and burst.
* REMARKS:
*     1. MAL-level shaper rate is typically identical to the
*     accumulated ports rate on the MAL. In this case,
*     soc_pb_ofp_rates_set/update may be used to configure the
*     MAL-level shaper/burst rate derived from accumulated
*     port-level configuration. In some cases, it is useful to
*     set the shaper rate to a lower value - e.g. if overall
*     (MAL-level) allowed port rate is bigger then the
*     accumulated port rate (port-level oversubscribtion). In
*     these cases, this API can be use.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_mal_shaper_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_rates_mal_shaper_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configure MAL-level shaping. This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index or non-network interface MAL equivalent.
*     Range: 0 - 7 for Network Interfaces. For non-network
*     interfaces use the MAL equivalents: SOC_PETRA_MAL_ID_CPU,
*     SOC_PETRA_MAL_ID_OLP, SOC_PETRA_MAL_ID_RCY.
*  SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper -
*     MAC Lane-level shaper - rate and burst.
* REMARKS:
*     1. MAL-level shaper rate is typically identical to the
*     accumulated ports rate on the MAL. In this case,
*     soc_pb_ofp_rates_set/update may be used to configure the
*     MAL-level shaper/burst rate derived from accumulated
*     port-level configuration. In some cases, it is useful to
*     set the shaper rate to a lower value - e.g. if overall
*     (MAL-level) allowed port rate is bigger then the
*     accumulated port rate (port-level oversubscribtion). In
*     these cases, this API can be use.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_mal_shaper_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_fat_pipe_rate_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate -
*     Fat pipe rate.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_fat_pipe_rate_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID               fatp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_fat_pipe_rate_verify
* TYPE:
*   PROC
* FUNCTION:
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate -
*     Fat pipe rate.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_fat_pipe_rate_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID               fatp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_fat_pipe_rate_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate -
*     Fat pipe rate.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_fat_pipe_rate_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID               fatp_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  );

uint32
  soc_pb_ofp_rates_egq_calendar_validate_unsafe(
    SOC_SAND_IN  int                    unit
  );

/*********************************************************************
* NAME:
*   soc_pb_ofp_rates_update_device_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Update the device after the computation of the
*   calendars.
* INPUT:
*   SOC_SAND_IN   int                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN   uint8                 is_device_updated -
*     If True, the device is immediately updated each time the
*     calendars are computed (more time-consuming). Otherwise,
*     the device is not updated and the wanted OFP rates are
*     kept in the software database.
* REMARKS:
*   Optimization to reduce the running time for the
*   computation of the OFP rates. If the global variable
*   corresponding to 'is_device_updated' goes from False to
*   True, then the OFP rates are computed and written to the
*   device with the call to this API. All the MAL shapers are
*   updated with an update mode 'SUM_OF_PORTS' - which can be
*   configured via 'soc_petra_ofp_rates_mal_shaper_set' API.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_update_device_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_device_updated
  );

/*********************************************************************
* NAME:
*   soc_pb_ofp_rates_update_device_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Update the device after the computation of the
*   calendars.
* INPUT:
*   SOC_SAND_IN   int                 unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT  uint8                 *is_device_updated -
*     If True, the device is immediately updated each time the
*     calendars are computed (more time-consuming). Otherwise,
*     the device is not updated and the wanted OFP rates are
*     kept in the software database.
* REMARKS:
*   Optimization to reduce the running time for the
*   computation of the OFP rates. If the global variable
*   corresponding to 'is_device_updated' goes from False to
*   True, then the OFP rates are computed and written to the
*   device with the call to this API.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_rates_update_device_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_device_updated
  );

uint32
  soc_pb_ofp_rates_from_egq_ports_rates_to_calendar(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint8                    recalc,
    SOC_SAND_IN  uint32                     *ports_rates,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  uint32                     total_shaper_bandwidth,
    SOC_SAND_IN  uint32                     max_calendar_len,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_EGQ      *calendar,
    SOC_SAND_OUT uint32                     *calendar_len
    );


uint32
  soc_pb_ofp_rates_from_rates_to_calendar(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                     *ports_rates,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  uint32                     total_credit_bandwidth,
    SOC_SAND_IN  uint32                     max_calendar_len,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_SCH      *calendar,
    SOC_SAND_OUT uint32                     *calendar_len
  );


#if SOC_PETRA_DEBUG

uint32
  soc_pb_ofp_rates_test_settings_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                    silent
  );


uint32
  soc_pb_ofp_rates_test_random_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                    silent
  );

uint32
  soc_pb_ofp_rates_fixed_len_cal_build(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   *port_nof_slots,
    SOC_SAND_IN  uint32                   nof_ports,
    SOC_SAND_IN  uint32                    calendar_len,
    SOC_SAND_IN  uint32                    max_calendar_len,
    SOC_SAND_OUT uint32                   *calendar
  );
#endif

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_OFP_RATES_INCLUDED__*/
#endif
