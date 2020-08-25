/* $Id: jer2_arad_debug.h,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifndef __JER2_ARAD_DEBUG_INCLUDED__
/* { */
#define __JER2_ARAD_DEBUG_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dnx/legacy/TMC/tmc_api_debug.h>


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

/*********************************************************************
* NAME:
*     jer2_arad_dbg_egress_shaping_enable_set
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable the egress shaping.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint8                 enable -
*     If TRUE, the egress shaping is enabled. Otherwise, it is
*     disabled.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  jer2_arad_dbg_egress_shaping_enable_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     jer2_arad_dbg_egress_shaping_enable_get
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable the egress shaping.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT uint8                 *enable -
*     If TRUE, the egress shaping is enabled. Otherwise, it is
*     disabled.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  jer2_arad_dbg_egress_shaping_enable_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT uint8                 *enable
  );

#endif
