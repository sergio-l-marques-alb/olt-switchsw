/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename boxs_defaultconfig.h
*
* @purpose All boxs default configurations are in this file.
*
* @component none
*
* @comments This file contains default values for end user configuration
*           parameters.  These values are used to build default configuration
*           files for components.
*
* @create 15/01/2008
*
* @author vkozlov
* @end
*
**********************************************************************/
#ifndef INCLUDE_BOXS_CONFIG_DEFAULTS
#define INCLUDE_BOXS_CONFIG_DEFAULTS

#include "datatypes.h"
#include "flex.h"

/*-------------------------------------*/
/* Start Boxs Global Defaults          */
/*-------------------------------------*/

#define FD_BOXS_DEFAULT_MIN_ALLOWED_TEMP          0
#define FD_BOXS_DEFAULT_MAX_ALLOWED_TEMP          45

#define FD_BOXS_DEFAULT_TEMPSTATUS_TRAP_ENABLED                 L7_ENABLE
#define FD_BOXS_DEFAULT_FANSTATUS_TRAP_ENABLED                  L7_ENABLE
#define FD_BOXS_DEFAULT_POWSUPPLYSTATUS_TRAP_ENABLED            L7_ENABLE
#define FD_BOXS_DEFAULT_SFP_TRAP_ENABLED                        L7_ENABLE
#define FD_BOXS_DEFAULT_XFP_TRAP_ENABLED                        L7_ENABLE

/*-------------------------------------*/
/* End Boxs Global Defaults            */
/*-------------------------------------*/

#ifdef INCLUDE_BOXS_DEFAULTCONFIG_OVERRIDES
#include "boxs_defaultconfig_overrides.h"
#endif

#endif /* INCLUDE_BOXS_CONFIG_DEFAULTS */
