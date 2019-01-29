
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dtlinclude.h
*
* @purpose The purpose of this file is to have a central location for
*          all dtl includes and definitions.
*
* @component Device Transformation Layer
*
* @comments none
*
* @create 12/21/2000
*
* @author asuthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTL_H
#define INCLUDE_DTL_H


/*
***********************************************************************
*                           COMMON INCLUDES
***********************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "l7_common.h"              /* lvl7 common definitions 
                                            and datatypes */
#include "l7_resources.h"
#include "simapi.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "registry.h"
#include "cfg_change_api.h"
#include "osapi.h"
#include "nimapi.h"
#include "fdb_api.h"
#include "pml_api.h"
#include "log.h"
#include "nimapi.h"
#include "dapi.h"                   
#include "statsapi.h"
#include "avl_api.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "sysnet_api.h"
#include "dot1q_api.h"
#include "pbvlan_api.h"
#include "dot3ad_api.h"
#include "dot1x_api.h"



/*
***************************************************************************
*                           DEFINES FOR COMPONENTS
***************************************************************************
*/
#define DTLCTRL_COMPONENT_CFG           1
#define DTLCTRL_COMPONENT_DRIVER        1
#define DTLCTRL_COMPONENT_EVENT         1
#define DTLCTRL_COMPONENT_INTF          1
#define DTLCTRL_COMPONENT_IP            1
#define DTLCTRL_COMPONENT_L2_DOT1D      1
#define DTLCTRL_COMPONENT_L2_DOT1Q      1
#define DTLCTRL_COMPONENT_L2_FDB        1
#define DTLCTRL_COMPONENT_L2_LAG        1
#define DTLCTRL_COMPONENT_POLICY_MAC    1
#define DTLCTRL_COMPONENT_STATS         1
#define DTLCTRL_COMPONENT_PDU_LANDD     1
#define DTLCTRL_COMPONENT_L2_FILTER     1
#define DTLCTRL_COMPONENT_L2_MCAST      1
#define DTLCTRL_COMPONENT_L2_DOT3AH     1
#define DTLCTRL_COMPONENT_L2_DOT1AD     1
#define DTLCTRL_COMPONENT_PTIN          1     /* PTin added: DTL support */


/*
**********************************************************************
*                           STANDARD LIBRARIES
**********************************************************************
*/
#ifdef __cplusplus
  #include <stl.h>                        /* Standard Template Library */
using namespace std;
#endif


/*
********************************************************************
*                           APIs and FUNCTION PROTOTYPES
********************************************************************
*/
#include "dtlapi.h"


/*
**********************************************************************
*                   DTL HEADER FILES
**********************************************************************
*/

#if DTLCTRL_COMPONENT_CFG
  #include "dtl_cfg.h"
#endif

#if DTLCTRL_COMPONENT_DRIVER
  #include "dtl_driver.h"
#endif

#if DTLCTRL_COMPONENT_EVENT
  #include "dtl_event.h"
#endif

#if DTLCTRL_COMPONENT_INTF
  #include "dtl_intf.h"
#endif

#if DTLCTRL_COMPONENT_L2_DOT1D
  #include "dtl_l2_dot1d.h"
#endif

#if DTLCTRL_COMPONENT_L2_DOT1Q
  #include "dtl_l2_dot1q.h"
#endif

#if DTLCTRL_COMPONENT_L2_FDB
  #include "dtl_l2_fdb.h"
#endif

#if DTLCTRL_COMPONENT_L2_LAG
  #include "dtl_l2_lag.h"
#endif 

#if DTLCTRL_COMPONENT_PDU_LANDD
  #include "dtl_pdu_landd.h"
#endif

#if DTLCTRL_COMPONENT_POLICY_MAC
  #include "dtl_policy_mac.h"
#endif

#if DTLCTRL_COMPONENT_STATS
  #include "dtl_stats.h"
#endif

#if DTLCTRL_COMPONENT_L2_FILTER
  #include "dtl_l2_filter.h"
#endif

#if DTLCTRL_COMPONENT_L2_MCAST
  #include "dtl_l2_mcast.h"
#endif


#if DTLCTRL_COMPONENT_L2_DOT1AD
  #include "dtl_l2_dot1ad.h"
#endif

#if DTLCTRL_COMPONENT_L2_DOT3AH
  #include "dtl_l2_dot3ah.h"
#endif

/* PTin added: DTL support */
#if DTLCTRL_COMPONENT_PTIN
  #include "dtl_ptin.h"
#endif

/*
********************************************************************
*                    SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldon occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/
#include <dtlinclude.h>
#elif defined(INCLUDE_DTL_H) && !defined(PASS2_INCLUDE_DTL_H)
#define PASS2_INCLUDE_DTL_H


#endif /* PASS2_INCLUDE_DTL_H */


