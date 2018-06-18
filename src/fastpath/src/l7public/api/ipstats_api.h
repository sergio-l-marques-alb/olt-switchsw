
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   ipstats_api.h
*
* @purpose    Ip Statistics Management file
*
* @component  ipstats
*
* @comments   none
*
* @create     05/07/2001
*
* @author     anayar
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_IP_STATS_API_H
#define INCLUDE_IP_STATS_API_H

#include "statsapi.h"

/*********************************************************************
* @purpose  Gets the value of Ip counters requested by the Statistics
*           Manager
*
* @param    *pCtr       Pointer to counter specifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is only intended to be called directly by the 
*           Stats Manager component, since this function pointer was
*           provided by the ipMap component when the external counters
*           were created.  It is not a public API function.
*      
* @end
*********************************************************************/
L7_RC_t ipStatsGet(counterValue_entry_t *pCtr);

/*********************************************************************
* @purpose  Creates the ip counters
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipStatsCreate();

/*********************************************************************
* @purpose  Reset the ip counters
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    Only resets counters that have been created in ipStatsCreate
*
* @end
*********************************************************************/
L7_RC_t ipStatsReset();

/*********************************************************************
* @purpose  Deletes the IP counters created in ipStatsCreate().
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipStatsDelete();

#endif  /* INCLUDE_IP_STATS_API_H */
