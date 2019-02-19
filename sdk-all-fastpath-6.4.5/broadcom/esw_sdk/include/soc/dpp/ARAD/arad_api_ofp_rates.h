/* $Id: arad_api_ofp_rates.h,v 1.8 Broadcom SDK $
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

#ifndef __ARAD_API_OFP_RATES_INCLUDED__
/* { */
#define __ARAD_API_OFP_RATES_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_ofp_rates.h>
#include <soc/dpp/port_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/*
 *  Setting  this value as maximal burst will result in no burst limitation
 */
#define ARAD_OFP_RATES_SCH_BURST_LIMIT_MAX          (SOC_TMC_OFP_RATES_SCH_BURST_LIMIT_MAX)
#define ARAD_OFP_RATES_BURST_LIMIT_MAX              (SOC_TMC_OFP_RATES_BURST_LIMIT_MAX)
#define ARAD_OFP_RATES_BURST_DEAULT                 (SOC_TMC_OFP_RATES_BURST_DEFAULT)
#define ARAD_OFP_RATES_EMPTY_Q_BURST_LIMIT_MAX      (SOC_TMC_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX)
#define ARAD_OFP_RATES_FC_Q_BURST_LIMIT_MAX         (SOC_TMC_OFP_RATES_BURST_FC_Q_LIMIT_MAX)     
#define ARAD_OFP_RATES_CHNIF_BURST_LIMIT_MAX        (0xFFC0)

#define ARAD_OFP_RATES_ILLEGAL_PORT_ID              (ARAD_NOF_FAP_PORTS)

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

#define ARAD_OFP_RATES_CAL_SET_A                          SOC_TMC_OFP_RATES_CAL_SET_A
#define ARAD_OFP_RATES_CAL_SET_B                          SOC_TMC_OFP_RATES_CAL_SET_B
#define ARAD_OFP_NOF_RATES_CAL_SETS                       SOC_TMC_OFP_NOF_RATES_CAL_SETS
typedef SOC_TMC_OFP_RATES_CAL_SET                         ARAD_OFP_RATES_CAL_SET;
typedef SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE         ARAD_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE;

#define ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB                   SOC_TMC_OFP_RATES_EGQ_CAL_CHAN_ARB
#define ARAD_OFP_RATES_EGQ_CAL_TCG                        SOC_TMC_OFP_RATES_EGQ_CAL_TCG
#define ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY              SOC_TMC_OFP_RATES_EGQ_CAL_PORT_PRIORITY
typedef SOC_TMC_OFP_RATES_EGQ_CAL_TYPE                    ARAD_OFP_RATES_EGQ_CAL_TYPE;
typedef SOC_TMC_OFP_RATES_CAL_INFO                        ARAD_OFP_RATES_CAL_INFO;

#define ARAD_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS            SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS
#define ARAD_OFP_SHPR_UPDATE_MODE_OVERRIDE                SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE
#define ARAD_OFP_SHPR_UPDATE_MODE_DONT_TUCH               SOC_TMC_OFP_SHPR_UPDATE_MODE_DONT_TUCH
#define ARAD_OFP_NOF_SHPR_UPDATE_MODES                    SOC_TMC_OFP_NOF_SHPR_UPDATE_MODES
typedef SOC_TMC_OFP_SHPR_UPDATE_MODE                           ARAD_OFP_SHPR_UPDATE_MODE;

typedef SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO                  ARAD_OFP_RATES_INTERFACE_SHPR_INFO;
typedef SOC_TMC_OFP_RATE_INFO                                  ARAD_OFP_RATE_INFO;

typedef enum
{
  ARAD_OFP_RATES_EGQ_CHAN_ARB_00   =  0,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_01   =  1,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_02   =  2,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_03   =  3,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_04   =  4,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_05   =  5,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_06   =  6,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_07   =  7,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_CPU  = 8,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_08 = 8,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_RCY  = 9,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_09 = 9,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN = 10,
  ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB = 11,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_INVALID = INVALID_CALENDAR
}ARAD_OFP_RATES_EGQ_CHAN_ARB_ID;

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

/*Auxiliary*/
uint32
    arad_ofp_rates_port2chan_arb(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                fap_port,
    SOC_SAND_OUT  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID *chan_arb_id
    );
/*Arad+ max burst features*/
uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 max_burst_fc_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT  uint32 *max_burst_fc_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 max_burst_empty_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT  uint32 *max_burst_empty_queues
    );
/*Single port rate setting*/
int
  arad_ofp_rates_egq_single_port_rate_sw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 rate
  );
int
  arad_ofp_rates_egq_single_port_rate_hw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port
  );
int
  arad_ofp_rates_sch_single_port_rate_hw_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port

  );
int
  arad_ofp_rates_egq_single_port_rate_hw_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *rate
  );
/*Single port max burst setting*/
uint32
  arad_ofp_rates_single_port_max_burst_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 max_burst
  );
uint32
  arad_ofp_rates_single_port_max_burst_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *max_burst
  );
/*Interface rate setting*/
int
  arad_ofp_rates_egq_interface_shaper_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                 if_shaper_rate
  );
int
  arad_ofp_rates_egq_interface_shaper_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT  uint32                *if_shaper_rate
  );
/*TCG max burst setting*/
uint32
    arad_ofp_rates_egq_tcg_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_egq_tcg_max_burst_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_OUT  uint32               *max_burst
    );
uint32
    arad_ofp_rates_sch_tcg_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_sch_tcg_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *max_burst
    );
/*TCG rate setting*/
uint32
    arad_ofp_rates_egq_tcg_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                rate
    );
uint32
    arad_ofp_rates_egq_tcg_rate_hw_set(
    SOC_SAND_IN  int                   unit
    );
uint32
    arad_ofp_rates_egq_tcg_rate_hw_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *rate
    );

uint32
    arad_ofp_rates_sch_tcg_rate_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                rate
    );
uint32
    arad_ofp_rates_sch_tcg_rate_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_OUT uint32                *rate
    );

/*PTC rate setting*/
uint32
    arad_ofp_rates_egq_port_priority_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                rate
    );
uint32
    arad_ofp_rates_egq_port_priority_rate_hw_set(
    SOC_SAND_IN  int                   unit
    );
uint32
    arad_ofp_rates_egq_port_priority_rate_hw_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_OUT uint32             *rate
    );
uint32
    arad_ofp_rates_sch_port_priority_rate_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                rate
    );

uint32
    arad_ofp_rates_sch_port_priority_rate_sw_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_IN  uint32             rate
    );

uint32
    arad_ofp_rates_sch_port_priority_hw_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core
    );

uint32
    arad_ofp_rates_sch_port_priority_rate_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_OUT uint32                *rate
    );
/*PTC max burst setting*/
uint32
    arad_ofp_rates_egq_port_priority_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_egq_port_priority_max_burst_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_OUT  uint32               *max_burst
    );
uint32
    arad_ofp_rates_sch_port_priority_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_sch_port_priority_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_OUT uint32             *max_burst
    );

void
  arad_ARAD_OFP_RATES_INTERFACE_SHPR_INFO_clear(
    SOC_SAND_OUT ARAD_OFP_RATES_INTERFACE_SHPR_INFO *info
  );

void
  arad_ARAD_OFP_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_OFP_RATE_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_OFP_RATES_CAL_SET_to_string(
    SOC_SAND_IN ARAD_OFP_RATES_CAL_SET enum_val
  );

const char*
  arad_ARAD_OFP_SHPR_UPDATE_MODE_to_string(
    SOC_SAND_IN ARAD_OFP_SHPR_UPDATE_MODE enum_val
  );

void
  arad_ARAD_OFP_RATES_INTERFACE_SHPR_INFO_print(
    SOC_SAND_IN ARAD_OFP_RATES_INTERFACE_SHPR_INFO *info
  );

void
  arad_ARAD_OFP_RATE_INFO_print(
    SOC_SAND_IN ARAD_OFP_RATE_INFO *info
  );

#endif /* ARAD_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_API_OFP_RATES_INCLUDED__*/
#endif


