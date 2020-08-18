/* $Id$
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifndef __SOC_DNX_API_DEBUG_INCLUDED__
/* { */
#define __SOC_DNX_API_DEBUG_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

#include <soc/dnx/legacy/TMC/tmc_api_general.h>
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
  /*
   *  Traffic route not forced.
   */
  SOC_DNX_DBG_FORCE_MODE_NONE=0,
  /*
   *  Traffic route forced to the local route (never routed
   *  through the fabric).
   */
  SOC_DNX_DBG_FORCE_MODE_LOCAL=1,
  /*
   *  Traffic route forced to the fabric route (always routed
   *  through the fabric).
   */
  SOC_DNX_DBG_FORCE_MODE_FABRIC=2,
  /*
   *  Total number of traffic routing force modes.
   */
  SOC_DNX_DBG_NOF_FORCE_MODES=3
}SOC_DNX_DBG_FORCE_MODE;

typedef enum
{
  /*
   *  The packets in the queue are dequeued and go to their
   *  destination.
   */
  SOC_DNX_DBG_FLUSH_MODE_DEQUEUE=0,
  /*
   *  The packets in the queue are deleted.
   */
  SOC_DNX_DBG_FLUSH_MODE_DELETE=1,
  /*
   *  Total number of flushing modes.
   */
  SOC_DNX_DBG_NOF_FLUSH_MODES=2
}SOC_DNX_DBG_FLUSH_MODE;

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

const char*
  SOC_DNX_DBG_FORCE_MODE_to_string(
    DNX_SAND_IN SOC_DNX_DBG_FORCE_MODE enum_val
  );

const char*
  SOC_DNX_DBG_FLUSH_MODE_to_string(
    DNX_SAND_IN  SOC_DNX_DBG_FLUSH_MODE enum_val
  );

/* } */


/* } __SOC_DNX_API_DEBUG_INCLUDED__*/
#endif
