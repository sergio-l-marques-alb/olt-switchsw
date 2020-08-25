/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename statsinclude.h
*
* @purpose The purpose of this file is to have a central location for
*          all stats includes and definitions.
*
* @component Statistics Manager
*
* @comments none
*
* @create 10/6/2000
*
* @author Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_STATS_H
#define INCLUDE_STATS_H


/*
***********************************************************************
*                           COMMON INCLUDES
***********************************************************************
*/
extern "C++" {
#include <commdefs.h>                   /* lvl7 common definitions   */
#include <datatypes.h>                  /* lvl7 data types definition*/
#include <osapi.h>                      /* lvl7 operating system apis */


/*
**********************************************************************
*                           STANDARD LIBRARIES
**********************************************************************
*/
#ifdef __cplusplus
#include <vector>                       /* Standard Template Library */
#include <list>                         /* Standard Template Library */
#include <set>                          /* Standard Template Library */
#include <map>                          /* Standard Template Library */
#include <algorithm>                    /* Standard Template Library */
using namespace std;
#endif
}


/*
********************************************************************
*                           APIs and FUNCTION PROTOTYPES
********************************************************************
*/
#include <statsapi.h>                      /* stats public methods */
#include <counter64.h>



/*
**********************************************************************
*                   STATISTICS MANAGER HEADER FILES
**********************************************************************
*/

#include <log.h>
#include <collector.h>				   
#include <presenter.h>
#include <statsconfig.h>

/*
********************************************************************
*                    SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldon occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/
#include <statsinclude.h>
#elif defined(INCLUDE_STATS_H) && !defined(PASS2_INCLUDE_STATS_H)
#define PASS2_INCLUDE_STATS_H


#endif /* PASS2_INCLUDE_STATS_H */


