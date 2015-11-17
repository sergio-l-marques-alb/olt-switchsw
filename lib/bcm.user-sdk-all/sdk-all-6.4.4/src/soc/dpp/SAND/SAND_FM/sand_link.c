/* $Id: sand_link.c,v 1.8 Broadcom SDK $
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
* FILENAME:       soc_sand_link.c
*
* MODULE PREFIX:  soc_sand_link
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_SAND_LINK_FE_BKT_FILL_RATE                      (6)
#define SOC_SAND_LINK_FE_UP_LINK_TH                         (32)
#define SOC_SAND_LINK_FE_DN_LINK_TH                         (16)
#define SOC_SAND_LINK_FE_REACHABILITY_RATE                  (50)    /*micro sec*/
#define SOC_SAND_LINK_FE_REACHABILITY_WATCHDOG_PERIOD       (100)   /*micro sec*/
#define SOC_SAND_LINK_FAP_BKT_FILL_RATE                     (6)
#define SOC_SAND_LINK_FAP_UP_LINK_TH                        (32)
#define SOC_SAND_LINK_FAP_DN_LINK_TH                        (16)
#define SOC_SAND_LINK_FAP_REACHABILITY_RATE                 (50)    /*micro sec*/
#define SOC_SAND_LINK_FAP_REACHABILITY_WATCHDOG_PERIOD      (100)   /*micro sec*/


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

static uint32
   Soc_sand_link_fe_bkt_fill_rate       = SOC_SAND_LINK_FE_BKT_FILL_RATE;
static uint32
   Soc_sand_link_fe_up_link_th          = SOC_SAND_LINK_FE_UP_LINK_TH;
static uint32
   Soc_sand_link_fe_dn_link_th          = SOC_SAND_LINK_FE_DN_LINK_TH;
static uint32
   Soc_sand_link_fe_reachability_rate  = SOC_SAND_LINK_FE_REACHABILITY_RATE;
static uint32
   Soc_sand_link_fe_reachability_watchdog_period  = SOC_SAND_LINK_FE_REACHABILITY_WATCHDOG_PERIOD;
static uint32
   Soc_sand_link_fap_bkt_fill_rate      = SOC_SAND_LINK_FAP_BKT_FILL_RATE;
static uint32
   Soc_sand_link_fap_up_link_th         = SOC_SAND_LINK_FAP_UP_LINK_TH;
static uint32
   Soc_sand_link_fap_dn_link_th         = SOC_SAND_LINK_FAP_DN_LINK_TH;
static uint32
   Soc_sand_link_fap_reachability_rate  = SOC_SAND_LINK_FAP_REACHABILITY_RATE;
static uint32
   Soc_sand_link_fap_reachability_watchdog_period  = SOC_SAND_LINK_FAP_REACHABILITY_WATCHDOG_PERIOD;


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_sand_link_fe_bkt_fill_rate_get(void)
{
  return Soc_sand_link_fe_bkt_fill_rate;
}

uint32
  soc_sand_link_fe_up_link_th_get(void)
{
  return Soc_sand_link_fe_up_link_th;
}

uint32
  soc_sand_link_fe_dn_link_th_get(void)
{
  return Soc_sand_link_fe_dn_link_th;
}

uint32
  soc_sand_link_fe_reachability_rate_get(
    SOC_SAND_IN  uint32                            core_rate,
    SOC_SAND_IN  uint32                            nof_links,
    SOC_SAND_IN  uint32                            nof_rmgr_units
  )
{
  uint32 rmgr_units_total;
  uint32 rmgr_val;
  uint32 rmgr_at_core_clock_steps;

  rmgr_units_total = nof_rmgr_units * nof_links;
  rmgr_at_core_clock_steps = (Soc_sand_link_fe_reachability_rate /*micro sec*/  * (core_rate /*per micro sec*//1000000));
  rmgr_val = SOC_SAND_DIV_ROUND_DOWN(rmgr_at_core_clock_steps, rmgr_units_total);
  return rmgr_val;
}

uint32
  soc_sand_link_fe_reachability_watchdog_period_get(
    SOC_SAND_IN  uint32                            core_rate,
    SOC_SAND_IN  uint32                            nof_links
  )
{

  uint32 wp_val;
  uint32 wp_at_core_clock_steps;
  wp_at_core_clock_steps = (Soc_sand_link_fe_reachability_watchdog_period /*micro sec*/ * (core_rate /*per micro sec*//1000000));
  wp_val = SOC_SAND_DIV_ROUND_UP(wp_at_core_clock_steps, 4096);
  return wp_val;
}


uint32
  soc_sand_link_fap_bkt_fill_rate_get(void)
{
  return Soc_sand_link_fap_bkt_fill_rate;
}

uint32
  soc_sand_link_fap_up_link_th_get(void)
{
  return Soc_sand_link_fap_up_link_th;
}

uint32
  soc_sand_link_fap_dn_link_th_get(void)
{
  return Soc_sand_link_fap_dn_link_th;
}

uint32
  soc_sand_link_fap_reachability_rate_get(
    SOC_SAND_IN  uint32                            core_rate,
    SOC_SAND_IN  uint32                            nof_links,
    SOC_SAND_IN  uint32                            rmgr_units
  )
{
  uint32 rmgr_units_total;
  uint32 rmgr_val;
  uint32 rmgr_at_core_clock_steps;

  rmgr_units_total = rmgr_units * nof_links;
  rmgr_at_core_clock_steps = (Soc_sand_link_fap_reachability_rate /*micro sec*/  * (core_rate /*per micro sec*//1000000));
  rmgr_val = SOC_SAND_DIV_ROUND_DOWN(rmgr_at_core_clock_steps, rmgr_units_total);
  return rmgr_val;
}

uint32
  soc_sand_link_fap_reachability_watchdog_period_get(
    SOC_SAND_IN  uint32                            core_rate
  )
{

  uint32 wp_val;
  uint32 wp_at_core_clock_steps;
  wp_at_core_clock_steps = (Soc_sand_link_fap_reachability_watchdog_period /*micro sec*/ * (core_rate /*per micro sec*//1000000));
  wp_val = SOC_SAND_DIV_ROUND_UP(wp_at_core_clock_steps, 4096);
  return wp_val;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
