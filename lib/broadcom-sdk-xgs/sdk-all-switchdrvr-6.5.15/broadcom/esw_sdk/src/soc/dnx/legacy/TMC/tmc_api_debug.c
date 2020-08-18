/* $Id: jer2_tmc_api_debug.c,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_tmc/src/soc_jer2_tmcapi_debug.c
*
* MODULE PREFIX:  Soc_petra
*
* SYSTEM:         DuneDriver/jer2_tmc/src/
*
* FILE DESCRIPTION: Different APIs for helping for debugging.
*
* REMARKS:  None.
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
*******************************************************************/
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

/*************
 * INCLUDES  *
 *************/
/* { */


#include <shared/bsl.h>

#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnx/legacy/TMC/tmc_api_debug.h>

#include <soc/dnxc/legacy/error.h>

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
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_DBG_FORCE_MODE_NONE:
    str = "none";
  break;
  case SOC_DNX_DBG_FORCE_MODE_LOCAL:
    str = "local";
  break;
  case SOC_DNX_DBG_FORCE_MODE_FABRIC:
    str = "fabric";
  break;
  case SOC_DNX_DBG_NOF_FORCE_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_DNX_DBG_FLUSH_MODE_to_string(
    DNX_SAND_IN  SOC_DNX_DBG_FLUSH_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_DNX_DBG_FLUSH_MODE_DEQUEUE:
    str = "dequeue";
  break;
  case SOC_DNX_DBG_FLUSH_MODE_DELETE:
    str = "delete";
  break;
  case SOC_DNX_DBG_NOF_FLUSH_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}


/* } */


