/* $Id: soc_petra_api_ofp_rates.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PETRA_API_OFP_RATES_INCLUDED__
/* { */
#define __SOC_PETRA_API_OFP_RATES_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_ofp_rates.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/* $Id: soc_petra_api_ofp_rates.h,v 1.6 Broadcom SDK $
 *  Setting  this value as maximal burst will result in no burst limitation
 */
#define SOC_PETRA_OFP_RATES_BURST_LIMIT_MAX     (0xFFFF)
#define SOC_PETRA_OFP_RATES_ILLEGAL_PORT_ID     (SOC_PETRA_NOF_FAP_PORTS)

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

#define SOC_PETRA_OFP_RATES_CAL_SET_A                          SOC_TMC_OFP_RATES_CAL_SET_A
#define SOC_PETRA_OFP_RATES_CAL_SET_B                          SOC_TMC_OFP_RATES_CAL_SET_B
#define SOC_PETRA_OFP_NOF_RATES_CAL_SETS                       SOC_TMC_OFP_NOF_RATES_CAL_SETS
typedef SOC_TMC_OFP_RATES_CAL_SET                              SOC_PETRA_OFP_RATES_CAL_SET;

#define SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS            SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS
#define SOC_PETRA_OFP_SHPR_UPDATE_MODE_OVERRIDE                SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE
#define SOC_PETRA_OFP_SHPR_UPDATE_MODE_DONT_TUCH               SOC_TMC_OFP_SHPR_UPDATE_MODE_DONT_TUCH
#define SOC_PETRA_OFP_NOF_SHPR_UPDATE_MODES                    SOC_TMC_OFP_NOF_SHPR_UPDATE_MODES
typedef SOC_TMC_OFP_SHPR_UPDATE_MODE                           SOC_PETRA_OFP_SHPR_UPDATE_MODE;

typedef SOC_TMC_OFP_RATES_MAL_SHPR                             SOC_PETRA_OFP_RATES_MAL_SHPR;
typedef SOC_TMC_OFP_RATES_MAL_SHPR_INFO                        SOC_PETRA_OFP_RATES_MAL_SHPR_INFO;
typedef SOC_TMC_OFP_RATE_INFO                                  SOC_PETRA_OFP_RATE_INFO;
typedef SOC_TMC_OFP_RATES_TBL_INFO                             SOC_PETRA_OFP_RATES_TBL_INFO;
typedef SOC_TMC_OFP_FAT_PIPE_RATE_INFO                         SOC_PETRA_OFP_FAT_PIPE_RATE_INFO;

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


int
  soc_petra_ofp_rates_sch_single_port_rate_sw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 rate
  );

int
  soc_petra_ofp_rates_sch_single_port_rate_hw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port

  );

uint32
  soc_petra_ofp_rates_egq_single_port_rate_sw_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  uint32                 rate
  );

int
  soc_petra_ofp_rates_egq_single_port_rate_sw_set_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  uint32              rate
  );

uint32
  soc_petra_ofp_rates_egq_single_port_rate_hw_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              ofp_ndx
  );

int
  soc_petra_ofp_rates_egq_single_port_rate_hw_set_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port
  );

uint32
  soc_petra_ofp_rates_single_port_max_burst_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  uint32                 max_burst
  );

int
  soc_petra_ofp_rates_sch_single_port_rate_hw_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *rate
  );

int
  soc_petra_ofp_rates_egq_single_port_rate_hw_get_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *rate
  );

uint32
  soc_petra_ofp_rates_egq_single_port_rate_hw_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT uint32                 *rate
  );
uint32
  soc_petra_ofp_rates_single_port_max_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT uint32                 *max_burst
  );
uint32
  soc_petra_ofp_rates_sch_interface_shaper_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  uint32                 if_shaper_rate,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode
  );
uint32
  soc_petra_ofp_rates_egq_interface_shaper_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  uint32                 if_shaper_rate,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode
  );
int
  soc_petra_ofp_rates_egq_interface_shaper_set_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              ofp_ndx,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                 if_shaper_rate
  );
uint32
  soc_petra_ofp_rates_sch_interface_shaper_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  ofp_ndx,
    SOC_SAND_OUT  uint32                 *if_shaper_rate
  );
uint32
  soc_petra_ofp_rates_egq_interface_shaper_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  ofp_ndx,
    SOC_SAND_OUT  uint32                 *if_shaper_rate
  );

int
  soc_petra_ofp_rates_egq_interface_shaper_get_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              ofp_ndx,
    SOC_SAND_OUT  uint32             *if_shaper_rate
  );
/*********************************************************************
* NAME:
*     soc_petra_ofp_rates_set
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
*     soc_petra_port_to_interface_map_set, prior to calling this
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
*     6. The function does not set exactly the requested rate
*     but tries to build the best possible calendar. As a result,
*     rates are inaccurate. Since changing the rate of one mal
*     changes the rates of all ports in all other mals, there's
*     no use of returning an exact variable.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ofp_rates_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  );

/*********************************************************************
* NAME:
*     soc_petra_ofp_rates_get
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
*     soc_petra_port_to_interface_map_set, prior to calling this
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
  soc_petra_ofp_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  );

uint32
  soc_petra_ofp_all_ofp_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 get_exact,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO  *ofp_rate_tbl
  );

/*********************************************************************
* NAME:
*     soc_petra_ofp_rates_single_port_set
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
*     non-channelized NIF-s with indexes 0-3 are shaped using a
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
*     in the end-to-end scheduler module. 5. This API relies on
*     NIF type configuration (as appears in
*     soc_petra_nif_mal_basic_conf_set API). If NIF type is
*     changed on-the-fly, OFP rates must be re-configured.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ofp_rates_single_port_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO       *ofp_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_ofp_rates_single_port_get
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
  soc_petra_ofp_rates_single_port_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATE_INFO       *ofp
  );

/*********************************************************************
* NAME:
*     soc_petra_ofp_rates_mal_shaper_set
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
*     soc_petra_ofp_rates_set/update may be used to configure the
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
  soc_petra_ofp_rates_mal_shaper_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *exact_shaper
  );

/*********************************************************************
* NAME:
*     soc_petra_ofp_rates_mal_shaper_get
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
*     soc_petra_ofp_rates_set/update may be used to configure the
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
  soc_petra_ofp_rates_mal_shaper_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper
  );

/*********************************************************************
* NAME:
*     soc_petra_ofp_fat_pipe_rate_set
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
*  SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *exact_fatp_rate -
*     Exact values may vary due to rounding.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ofp_fat_pipe_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_ofp_fat_pipe_rate_get
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
  soc_petra_ofp_fat_pipe_rate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  );

#ifdef LINK_PB_LIBRARIES
/*********************************************************************
* NAME:
*     soc_pb_ofp_fat_pipe_rate_set
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
*  SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *exact_fatp_rate -
*     Exact values may vary due to rounding.
* REMARKS:
*     This API is applicable only under the assumption that
*     the NIF-s that are part of the Fat-pipe are non-channelized.
*     When using channelized NIFs under Fat-pipe, the ofp_rate_set API may be used
*     in the following manner:
*      - For EGQ rate, set the rate for each Port that is part of the Fat-pipe
*        independently (as the proportional portion of the overall Fat-pipe rate.
*      - For SCH rate, set the rate as the overall Fat-pipe rate, only for the
*        Fat-pipe destination port (i.e. OFP 1 for Fat-pipe A, OFP 2 for Fat-pipe B,
*        OFP 3 for Fat-pipe C)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ofp_fat_pipe_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID            fatp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  );

/*********************************************************************
* NAME:
*     soc_pb_ofp_fat_pipe_rate_get
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
  soc_pb_ofp_fat_pipe_rate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID            fatp_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  );

#endif

uint32
  soc_petra_ofp_rates_egq_calendar_validate(
    SOC_SAND_IN  int                    unit
  );

/*********************************************************************
* NAME:
*   soc_petra_ofp_rates_update_device_set
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
  soc_petra_ofp_rates_update_device_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_device_updated
  );

/*********************************************************************
* NAME:
*   soc_petra_ofp_rates_update_device_get
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
  soc_petra_ofp_rates_update_device_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_device_updated
  );

void
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR *info
  );

void
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *info
  );

void
  soc_petra_PETRA_OFP_RATE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_RATE_INFO *info
  );

void
  soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO *info
  );

void
  soc_petra_PETRA_OFP_FAT_PIPE_RATE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_OFP_RATES_CAL_SET_to_string(
    SOC_SAND_IN SOC_PETRA_OFP_RATES_CAL_SET enum_val
  );

const char*
  soc_petra_PETRA_OFP_SHPR_UPDATE_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_OFP_SHPR_UPDATE_MODE enum_val
  );

void
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_print(
    SOC_SAND_IN SOC_PETRA_OFP_RATES_MAL_SHPR *info
  );

void
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_print(
    SOC_SAND_IN SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *info
  );

void
  soc_petra_PETRA_OFP_RATE_INFO_print(
    SOC_SAND_IN SOC_PETRA_OFP_RATE_INFO *info
  );

void
  soc_petra_PETRA_OFP_RATES_TBL_INFO_print(
    SOC_SAND_IN SOC_PETRA_OFP_RATES_TBL_INFO *info
  );

void
  soc_petra_PETRA_OFP_FAT_PIPE_RATE_INFO_print(
    SOC_SAND_IN SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_OFP_RATES_INCLUDED__*/
#endif
