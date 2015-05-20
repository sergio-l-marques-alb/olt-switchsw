/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_diffserv_include.h
*
* @purpose    The purpose of this file is to have a central location for
*             all DiffServ includes and definitions for the component
*
* @component  DiffServ
*
* @comments   none
*
* @create     02/18/2002
*             
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_L7_DIFFSERV_INCLUDE_H
#define INCLUDE_L7_DIFFSERV_INCLUDE_H


/*
***********************************************************************
*                     COMMON INCLUDES
***********************************************************************
*/
#include <string.h>
#include "l7_common.h"                  /* lvl7 common definitions */
#include "nvstoreapi.h"
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "avl_api.h"
#include "l7_packet.h"
#include "nimapi.h"
#include "simapi.h"
#include "dot3ad_api.h"
#include "mirror_api.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "log.h"


/*
********************************************************************
*                     APIs and FUNCTION PROTOTYPES
********************************************************************
*/
#include "l7_diffserv_api.h"            /* DiffServ public API functions */

/*
**********************************************************************
*                     DIFFSERV HEADER FILES
**********************************************************************
*/

#include "diffserv_config.h"
#include "diffserv_stdmib.h"
#include "diffserv_util.h"
#include "diffserv_prvtmib.h"
#include "diffserv_distiller.h"
#include "diffserv_trace.h"

/*
**********************************************************************
*                     DIFFSERV CONFIGURATOR HEADER
**********************************************************************
*/

#include "diffserv_cnfgr.h"

#endif /* INCLUDE_L7_DIFFSERV_INCLUDE_H */
