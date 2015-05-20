/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  l7_ospfinclude.h
*
* @purpose   The purpose of this file is to have a central location for
* @purpose   all ospf includes and definitions.
*
* @component Ospf Mapping Layer
*
* @comments  none
*
* @create    03/19/2001
*
* @author    asuthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_OSPF_INCLUDE_H
#define INCLUDE_OSPF_INCLUDE_H


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
#include "l7_ip_api.h"
#include "l3_defaultconfig.h"
#include "ospf_vend_ctrl.h"
#include "ip_config.h"
#include "log.h"
#include "ospf_vend_exten.h"
#include "trap_layer3_api.h"
#include "rto_api.h"
/*
********************************************************************
*                     PHASE2 PORT HEADER FILES
********************************************************************
*/


/*
********************************************************************
*                     APIs and FUNCTION PROTOTYPES
********************************************************************
*/

/*
********************************************************************
*                     OSPF FACTORY DEFAULTS
********************************************************************
*/
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"

/*
**********************************************************************
*                     OSPF HEADER FILES
**********************************************************************
*/

#include "ospf_config.h"
#include "ospf_util.h"

/*
********************************************************************
*                     SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldom occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/
#include <l7_ospfinclude.h>
#elif defined(INCLUDE_OSPF_INCLUDE_H) && !defined(PASS2_INCLUDE_OSPF_INCLUDE_H)
#define PASS2_INCLUDE_OSPF_INCLUDE_H


#endif /* PASS2_INCLUDE_OSPF_INCLUDE_H */
