/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cardmgr_include.h
*
* @purpose    Card Manager component include all header file
*
* @component  cnfgr
*
* @comments   none
*
* @create     06/18/2003
*
* @author     avasquez
* @end
*
**********************************************************************/
#ifndef INCLUDE_CARDMGR_INCLUDE_H
#define INCLUDE_CARDMGR_INCLUDE_H



/*
***********************************************************************
*                          Standard libraries
***********************************************************************
*/

#ifdef   __cplusplus
#include <stl.h>                        /* Standard Template Library */
using namespace std;

extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
/*
***********************************************************************
*                          Common Headers 
***********************************************************************
*/
#include "l7_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "dll_api.h"
#include "l7_cnfgr_api.h" 
#include "sysnet_api.h"
#include "nimapi.h" 
#include "sysapi.h"
#include "sysapi_hpc.h"
#include "sysapi_hpc_chassis.h"
#include "platform_config.h"
#include "dtlapi.h"
#include "usmdb_cardmgr_api.h" 
#include "nvstoreapi.h"
#include "unitmgr_api.h"

/*
***********************************************************************
*                          Card Manager API
***********************************************************************
*/
#include "cardmgr_api.h"

/*
***********************************************************************
*                          Building Block Header Files
***********************************************************************
*/
#include "cardmgr_sid.h"
#include "cardmgr.h"
#include "cardmgr_cnfgr.h"
#include "cardmgr_trace.h"

#ifdef __cplusplus
}
#endif
/*
********************************************************************
*                    Second Pre-processor Pass 
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldon occurs, but if
* needed place the second pass included in this section. Avoid this.
********************************************************************
*/
#include "cardmgr_include.h"
#elif defined(INCLUDE_CARDMGR_INCLUDE_H) && !defined(PASS2_INCLUDE_CARDMGR_INCLUDE_H)
#define PASS2_INCLUDE_CARDMGR_INCLUDE_H


#ifdef   __cplusplus
extern "C" {
#endif
/* place second pass  include here */


#ifdef   __cplusplus
}
#endif

#endif /* PASS2_INCLUDE_CARDMGR_INCLUDE_H */
