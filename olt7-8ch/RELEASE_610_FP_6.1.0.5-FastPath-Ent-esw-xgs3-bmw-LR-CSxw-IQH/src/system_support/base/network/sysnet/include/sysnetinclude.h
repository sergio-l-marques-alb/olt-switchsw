
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  sysnetinclude.h
*
* @purpose   The purpose of this file is to have a central location for
* @purpose   all sysnet includes and definitions.
*
* @component Sysnet Utility Functions
*
* @comments  none
*
* @create    03/23/2001
*
* @author    anayar
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_SYSNET_INCLUDE_H
#define INCLUDE_SYSNET_INCLUDE_H


/*
***********************************************************************
*                     COMMON INCLUDES
***********************************************************************
*/
#include <string.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "log.h"
#include "sysapi.h"
#include "l7_packet.h"

#include "usmdb_mib_bridge_api.h"
#include "simapi.h"
#include "osapi.h"
#include "nimapi.h"
#include "dtlapi.h"
#include "default_cnfgr.h"

/*
********************************************************************
*                     APIs and FUNCTION PROTOTYPES
********************************************************************
*/

#include "sysnet_api.h"

/*
**********************************************************************
*                     SYSNET HEADER FILES
**********************************************************************
*/

#include "sysnet_util.h"

/*
********************************************************************
*                     SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldom occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/
#include <sysnetinclude.h>
#elif defined(INCLUDE_SYSNET_INCLUDE_H) && !defined(PASS2_INCLUDE_SYSNET_INCLUDE_H)
#define PASS2_INCLUDE_SYSNET_INCLUDE_H


#endif /* PASS2_INCLUDE_SYSNET_INCLUDE_H */
