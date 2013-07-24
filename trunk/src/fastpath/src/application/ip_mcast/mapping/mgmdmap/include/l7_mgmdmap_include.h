/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  l7_mgmdinclude.h
*
* @purpose   The purpose of this file is to have a central location for
*            all mgmd includes and definitions.
*
* @component Mgmd Mapping Layer
*
* @comments  none
*
* @create
*
* @author  ramakrishna
* @end
*
**********************************************************************/

#ifndef _INCLUDE_MGMD_INCLUDE_H
#define _INCLUDE_MGMD_INCLUDE_H

/*
***********************************************************************
*                     COMMON INCLUDES
***********************************************************************
*/
#include <string.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "nvstoreapi.h"
#include "sysapi.h"
#include "osapi.h"
#include "l7_packet.h"
#include "nimapi.h"
#include "simapi.h"
#include "l3_defaultconfig.h"
#include "l3_addrdefs.h"
#include "log.h"


/*
********************************************************************
*                     MGMD FACTORY DEFAULTS
********************************************************************
*/
#include "l3_defaultconfig.h"
#include "l3_mcast_defaultconfig.h"
#include "l3_default_cnfgr.h"
#include "l3_mcast_default_cnfgr.h"
#include "l3_mcast_commdefs.h"

/*
**********************************************************************
*                     MGMD HEADER FILES
**********************************************************************
*/

#include "mgmdmap_cnfgr.h"
#include "mgmdmap_config.h"
#include "mgmdmap_util.h"
#include "mgmdmap_vend_exten.h"
#include "mgmdmap_proxy_vend_exten.h"
#include "mgmdmap.h"
#include "mgmdmap_debug.h"
#include "mgmdmap_v6_wrappers.h"
/*
********************************************************************
*                     APIs and FUNCTION PROTOTYPES
********************************************************************
*/

#include "l7_mgmd_api.h"                      /* mgmd public methods */
#include "mgmd_proxy_api.h"
#include "mgmd_api.h"
#include "mcast_wrap.h" 
#include "l7_mcast_api.h"

/*
********************************************************************
*                     SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldom occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/
#elif defined(INCLUDE_MGMD_INCLUDE_H) && !defined(PASS2_INCLUDE_MGMD_INCLUDE_H)
#define PASS2_INCLUDE_MGMD_INCLUDE_H


#endif /* PASS2_INCLUDE_MGMD_INCLUDE_H */
