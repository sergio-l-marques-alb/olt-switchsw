/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_ctrl_stats.h
*
* @purpose This header file contains definitions to support the       
*          statistics from various components: NIM and VLAN.      
*
* @component Device Transformation Layer
*
* @comments none
*
* @author   Shekhar Kalyanam 3/14/2001
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTLCTRL_STATS_H
#define INCLUDE_DTLCTRL_STATS_H
/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/



/*
***********************************************************************
*                           DATA TYPES
***********************************************************************
*/


/*
**********************************************************************
*                           GLOBAL DATA
**********************************************************************
*/
#ifdef  DTLCTRL_STATS_GLOBALS
  #define DTLCTRL_STATS_EXT
#else 
  #define DTLCTRL_STATS_EXT extern
#endif  /*DTLCTRL_STATS_GLOBALS*/




/*
***********************************************************************
*                       FUNCTION PROTOTYPES -- DTL INTERNAL USE ONLY
***********************************************************************
*/

/*********************************************************************
* @purpose  Registers statistics counters with Statistics Manager
*          
* @param 		
*	
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlStatsCreate();

/*********************************************************************
* @purpose  gets or resets the dtl counters
*          
* @param 		counter      Pointer to storage allocated by stats
*                        manager to hold the results
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlStatsCounter(pCounterValue_entry_t counter);

/*********************************************************************
* @purpose  Get the driver statistics for this interface
*
* @param    intfNUM     @b{(input)}physical or logical interface to be configured
* @param    counterID   @b{(input)}specific counter
* @param    val         @b{(output)}pointer to the value
*
* @returns  L7_SUCCESS, if counters is successfully obtained
* @returns  L7_FAILURE, otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlGetStats(L7_uint32 intIfNum, DAPI_INTF_STATS_t counterID, L7_ulong64 *val);

/*********************************************************************
* @purpose  Get the driver vlan statistics for this interface
*
* @param    intfNUM     @b{(input)}physical or logical interface to be configured
* @param    counterID   @b{(input)}specific counter
* @param    vlanID      @b{(input)} vlan id
* @param    val         @b{(output)}pointer to the value
*
* @returns  L7_SUCCESS, if counters is successfully obtained
* @returns  L7_FAILURE, otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlGetVlanStats(L7_uint32 intIfNum, DAPI_INTF_STATS_t counterID, L7_ulong32 vlanID, L7_ulong64 *val);

/*********************************************************************
* @purpose  increments dtl counters
*          
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param 		counter      @b{(input)} type of counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlStatsIncrement(L7_uint32 intIfNum, L7_uint32 counter);
#endif    /*INCLUDE_DTLCTRL_STATS_H*/
