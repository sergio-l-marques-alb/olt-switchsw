
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  snmpinclude.h
*
* @purpose   The purpose of this file is to have a central location for
* @purpose   all snmp includes and definitions.
*
* @component snmp component
*
* @comments  none
*
* @create    03/02/2001
*
* @author    asuthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_SNMP_H
#define INCLUDE_SNMP_H


/*
***********************************************************************
*                     COMMON INCLUDES
***********************************************************************
*/
#include <string.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "log.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "sysapi.h"
#include "registry.h"


/*
********************************************************************
*                     APIs and FUNCTION PROTOTYPES
********************************************************************
*/
#include "snmp_api.h"                      /* snmp public methods */

/*
********************************************************************
*                     SNMP FACTORY DEFAULTS
********************************************************************
*/
#include "defaultconfig.h"

/*
**********************************************************************
*                     SNMP HEADER FILES
**********************************************************************
*/

#include "snmp_util.h"				   
#include "snmpconfig.h"
#include "snmpconfigv3.h"
#include "snmptrap.h"
#include "basetrap.h"
#include "snmptask.h"
#include "snmptask_ct.h"
#include "snmptask_cfg.h"

#ifdef L7_STACKING_PACKAGE
#include "entity_api.h"
#endif /* L7_STACKING_PACKAGE */

/*
********************************************************************
*                     SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldom occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/
#include <snmpinclude.h>
#elif defined(INCLUDE_SNMP_H) && !defined(PASS2_INCLUDE_SNMP_H)
#define PASS2_INCLUDE_SNMP_H


#endif /* PASS2_INCLUDE_SNMP_H */
