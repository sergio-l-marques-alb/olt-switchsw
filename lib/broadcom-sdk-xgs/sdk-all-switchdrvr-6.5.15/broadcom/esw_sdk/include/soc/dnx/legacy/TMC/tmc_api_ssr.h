/* $Id: jer2_jer2_jer2_tmc_api_ssr.h,v 1.3 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_jer2_jer2_tmc/include/soc_jer2_jer2_jer2_tmcapi_ssr.h
*
* MODULE PREFIX:  soc_jer2_jer2_jer2_tmcssr
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

#ifndef __SOC_DNX_API_SSR_H_INCLUDED__
/* { */
#define __SOC_DNX_API_SSR_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dnx/legacy/SAND/Management/sand_api_ssr.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

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

typedef enum{
  SOC_DNX_SW_DB_CFG_VERSION_INVALID = 0,
  SOC_DNX_SW_DB_CFG_VERSION_NUM_0 = 1,
  SOC_DNX_SW_DB_CFG_NOF_VERSIONS
}SOC_DNX_SW_DB_CFG_VERSION_NUM;


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

/* } */


/* } __SOC_DNX_API_SSR_H_INCLUDED__*/
#endif
