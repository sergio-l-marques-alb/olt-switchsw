/* $Id: sand_link.h,v 1.9 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifndef __DNX_SAND_LINK_INCLUDED__
/* { */
#define __DNX_SAND_LINK_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!"
#endif

/*************
 * INCLUDES  *
 *************/
/* { */
#include        <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include        <sal/types.h>
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
  dnx_sand_link_fe_bkt_fill_rate_get(void);

uint32
  dnx_sand_link_fe_up_link_th_get(void);

uint32
  dnx_sand_link_fe_dn_link_th_get(void);

uint32
  dnx_sand_link_fe_reachability_rate_get(
    DNX_SAND_IN  uint32                            core_rate,
    DNX_SAND_IN  uint32                            nof_links,
    DNX_SAND_IN  uint32                            nof_rmgr_units
  );

uint32
  dnx_sand_link_fe_reachability_watchdog_period_get(
    DNX_SAND_IN  uint32                            core_rate,
    DNX_SAND_IN  uint32                            nof_links
  );

/* } */

/* } __DNX_SAND_LINK_INCLUDED__*/
#endif
