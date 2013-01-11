/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3adinclude.h
*
* @purpose defines all datatypes used in dot3ad componenet
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @comments none
*
* @create 6/5/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef INCLUDE_DOT3AD_H
#define INCLUDE_DOT3AD_H


/*
***********************************************************************
*                           COMMON INCLUDES
***********************************************************************
*/

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "nimapi.h"
#include "dtlapi.h"
#include "trace_api.h"
#include "sysnet_api.h"
#include "dot1q_api.h"
#include "log.h"

/*
***************************************************************************
*                           DEFINES FOR COMPONENTS
***************************************************************************
*/
#define DOT3AD_COMPONENT_CFG            1
#define DOT3AD_COMPONENT_LAC            1
#define DOT3AD_COMPONENT_LACP           1
#define DOT3AD_COMPONENT_DB             1
#define DOT3AD_COMPONENT_LACPM          1
#define DOT3AD_COMPONENT_LAIH           1
#define DOT3AD_COMPONENT_COUNTER        1


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
#include "dot3ad_api.h"

/*
**********************************************************************
*                   DOT3AD HEADER FILES
**********************************************************************
*/

#if DOT3AD_COMPONENT_DB
  #include "dot3ad_db.h"
#endif

#if DOT3AD_COMPONENT_CFG
  #include "dot3ad_cfg.h"
#endif

#if DOT3AD_COMPONENT_LAC
  #include "dot3ad_lac.h"
#endif

#if DOT3AD_COMPONENT_LAIH
  #include "dot3ad_laih.h"
#endif

#if DOT3AD_COMPONENT_LACP
  #include "dot3ad_lacp.h"
#endif
#if DOT3AD_COMPONENT_LACPM
  #include "dot3ad_lacpm.h"
#endif

#if DOT3AD_COMPONENT_COUNTER
  #include "dot3ad_counter.h"
#endif









#endif  /*INCLUDE_DOT3AD_H*/
