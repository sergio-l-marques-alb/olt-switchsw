/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_include.h
*
* @purpose    Configuraror component include all header file
*
* @component  cnfgr
*
* @comments   none
*
* @create     03/17/2003
*
* @author     avasquez
* @end
*
**********************************************************************/
#ifndef INCLUDE_CNFGR_INCLUDE_H
#define INCLUDE_CNFGR_INCLUDE_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

/*  
   Use the following constants to include or remove configurator 
   nodules. Once a module is remove, stub functions MAY be compiled
   in.
*/   
#define CNFGR_PRESENT       1           /* Compile                   */
#define CNFGR_NOT_PRESENT   0           /* Do not compile            */ 

#define CNFGR_MODULE               CNFGR_PRESENT
#define CNFGR_MODULE_INIT          CNFGR_PRESENT
#define CNFGR_MODULE_CR            CNFGR_PRESENT
#define CNFGR_MODULE_TALLY         CNFGR_PRESENT
#define CNFGR_MODULE_CTRL          CNFGR_PRESENT
#define CNFGR_MODULE_MSG           CNFGR_PRESENT
#define CNFGR_MODULE_DBG           CNFGR_PRESENT

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
#include <string.h>
#include <errno.h>
/*
***********************************************************************
*                          Common Headers 
***********************************************************************
*/
#include "l7_common.h"
#include "osapi.h"
#include "log.h"
#include "dll_api.h"
#include "sysapi.h"

/*
***********************************************************************
*                          Configurator API
***********************************************************************
*/
#include "l7_cnfgr_api.h" 

/*
***********************************************************************
*                          Building Block Header Files
***********************************************************************
*/

#if      (CNFGR_MODULE == CNFGR_PRESENT) 
#include "cnfgr_sid.h"         /* Structural Initialization Data */
#endif

#if      (CNFGR_MODULE_INIT == CNFGR_PRESENT) 
#include "cnfgr_init.h"
#endif
 
#if      (CNFGR_MODULE_CR == CNFGR_PRESENT)
#include "cnfgr_cr.h"
#endif

#if      (CNFGR_MODULE_TALLY == CNFGR_PRESENT)
#include "cnfgr_tally.h"
#endif

#if      (CNFGR_MODULE_CTRL == CNFGR_PRESENT)
#include "cnfgr_ctrl.h"
#endif

#if      (CNFGR_MODULE_MSG == CNFGR_PRESENT)
#include "cnfgr_msg.h"
#endif

#if      (CNFGR_MODULE_DBG == CNFGR_PRESENT)
#include "cnfgr_debug.h"
#endif

#include "cnfgr_trace.h"
#include "cnfgr_hw_tally.h"

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
#include "cnfgr_include.h"
#elif defined(INCLUDE_CNFGR_INCLUDE_H) && !defined(PASS2_INCLUDE_CNFGR_INCLUDE_H)
#define PASS2_INCLUDE_CNFGR_INCLUDE_H


#ifdef   __cplusplus
extern "C" {
#endif
/* place second pass  include here */


#ifdef   __cplusplus
}
#endif

#endif /* PASS2_INCLUDE_CNFGR_INCLUDE_H */
