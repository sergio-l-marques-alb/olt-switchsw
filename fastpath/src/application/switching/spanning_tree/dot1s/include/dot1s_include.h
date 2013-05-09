/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_include.h
*
* @purpose   Multiple Spanning tree common include file
*
* @component dot1s
*
* @comments  
*
* @create    10/15/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_H
#define INCLUDE_DOT1S_H


/*
 ***********************************************************************
 *                           COMMON INCLUDES
 ***********************************************************************
 */

#include "l7_common.h"
#include "l7_resources.h"
#include "osapi.h" 
#include "osapi_support.h"
#include "sysapi.h"
#include "nimapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "trace_api.h"
#include "sysnet_api.h"
#include "dot1q_api.h"
#include "fdb_api.h"
#include "log.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "hmac_api.h"
#include "filter_api.h"
#include "datatypes.h"
#include "trapapi.h"
#include "dot3ad_api.h"

/*
 ***************************************************************************
 *                           DEFINES FOR COMPONENTS
 ***************************************************************************
 */
#define DOT1S_COMPONENT_CFG         1
#define DOT1S_COMPONENT_CONTROL     1
#define DOT1S_COMPONENT_SM          1
#define DOT1S_COMPONENT_DB          1
#define DOT1S_COMPONENT_MASK        1
#define DOT1S_COMPONENT_IH          1
#define DOT1S_COMPONENT_TXRX        1
#define DOT1S_COMPONENT_DEBUG       1


/*
 **********************************************************************
 *                           STANDARD LIBRARIES
 **********************************************************************
 */
#ifdef __cplusplus
  #include <stl.h>                        /* Standard Template Library */
using namespace std;
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*
 ********************************************************************
 *                           APIs 
 ********************************************************************
 */
#include "dot1s_api.h"
#include "hmac_api.h"

/*
 **********************************************************************
 *                   DOT1S HEADER FILES
 **********************************************************************
 */

#if DOT1S_COMPONENT_MASK
  #include "dot1s_mask.h"
#endif

#if DOT1S_COMPONENT_DB
  #include "dot1s_db.h"
#endif

#if DOT1S_COMPONENT_CFG
  #include "dot1s_cfg.h"
  #include "dot1s_cnfgr.h"
#endif

#if DOT1S_COMPONENT_CONTROL
  #include "dot1s_control.h"
#endif

#if DOT1S_COMPONENT_IH
  #include "dot1s_ih.h"
#endif

#if DOT1S_COMPONENT_SM
  #include "dot1s_sm.h"
#endif

#if DOT1S_COMPONENT_TXRX
  #include "dot1s_txrx.h"
#endif

#if DOT1S_COMPONENT_DEBUG
  #include "dot1s_debug.h"
#endif

#endif  /*INCLUDE_DOT1S_H*/
