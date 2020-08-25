/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  l7_ipinclude.h
*
* @purpose   The purpose of this file is to have a central location for
* @purpose   all ip includes and definitions.
*
* @component Ip Mapping Layer
*
* @comments  none
*
* @create    03/12/2001
*
* @author    anayar
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_IP_INCLUDE_H
#define INCLUDE_IP_INCLUDE_H


/*
***********************************************************************
*                     COMMON INCLUDES
***********************************************************************
*/

#include <stdlib.h>
#include <string.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "sysapi.h"
#include "osapi.h"
#include "l3_defaultconfig.h"
#include "log.h"
#include "nvstoreapi.h"
#include "l7_packet.h"
#include "nimapi.h"
#include "dtlapi.h"
#include "dtl_l3_api.h"
#include "platform_counters.h"
#include "statsapi.h"
#include "l3end_api.h"
#include "fdb_api.h"
#include "simapi.h"
#include "l7_ipmap_arp_api.h"
#include "rto_api.h"
#include "usmdb_util_api.h"
#include "usmdb_ip_base_api.h"

#include "usmdb_1213_api.h"
#include "ipstats_api.h"
#include "rtip_var.h"
#include "buff_api.h"
#include "async_event_api.h"
#include "component_mask.h"
#include "l3_commdefs.h"

/*
********************************************************************
*                     VXWORKS PORT HEADER FILES
********************************************************************
*/

#include "l7_socket.h"

/*
********************************************************************
*                     PHASE2 PORT HEADER FILES
********************************************************************
*/

#if L7_LAYER3_PACKAGE
  #include "iproute.h"
#endif

/*
********************************************************************
*                     APIs and FUNCTION PROTOTYPES
********************************************************************
*/
/* #include "l7_ip_api.h"   */                   /* ip public methods */

/*
********************************************************************
*                     IP FACTORY DEFAULTS
********************************************************************
*/
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"

/*
**********************************************************************
*                     IP HEADER FILES
**********************************************************************
*/

#include "ip_config.h" 
#include "ip_util.h" 

/*
********************************************************************
*                     SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldom occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/
#include <l7_ipinclude.h>
#elif defined(INCLUDE_IP_INCLUDE_H) && !defined(PASS2_INCLUDE_IP_INCLUDE_H)
#define PASS2_INCLUDE_IP_INCLUDE_H

#endif /* PASS2_INCLUDE_IP_INCLUDE_H */
