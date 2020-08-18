/* $Id: jer2_jer_fabric.h,v 1.30 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifndef __JER2_JER_INGRESS_SCHEDULER_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/* { */

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/error.h>

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
*     jer2_jer_ingress_scheduler_init
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure init ingress scheduler
*     links.
* INPUT:
*  DNX_SAND_IN  int                 unit -
* REMARKS:
*     None.
*********************************************************************/
shr_error_e
  jer2_jer_ingress_scheduler_init(
    DNX_SAND_IN  int                 unit
  );

/* } __JER2_JER_INGRESS_SCHEDULER_INCLUDED__*/
#endif
