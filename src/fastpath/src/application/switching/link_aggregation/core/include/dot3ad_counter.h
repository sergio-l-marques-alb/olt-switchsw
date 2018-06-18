/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3ad_counter.h                        
*
* @purpose dot3ad counter includes and prototypes
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @notes   
*
* @create 11/05/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef INCLUDE_DOT3AD_COUNTER_H
#define INCLUDE_DOT3AD_COUNTER_H

/*************************************************************************
                            FUNCTION PROTOTYPES
*************************************************************************/
/*********************************************************************
* @purpose  Get one of the LAG external counters for this interface.
*
* @param    *pCtr       Pointer to counter specifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is only intended to be called directly by the 
*           Stats Manager component, since this function pointer was
*           provided by the dot3ad component when the external counters
*           were created.  It is not a public API function.
*      
* @end
*********************************************************************/
L7_RC_t dot3adCounterExternalGet(counterValue_entry_t *pCtr);
/*********************************************************************
* @purpose  Creates the set of counters for the specified LAG interface.
*           This covers both mutant and non-mutant varieties.
*
* @param    intIfNum    internal interface number of the LAG
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The mutant counters created here are initially empty, that is,
*           they have no associated mutlings.  Each time the LAG
*           membership is updated, the counters associated with the 
*           member interfaces are added/deleted as mutlings through
*           the Stats API.
*       
* @end
*********************************************************************/
L7_RC_t dot3adCounterCreate(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  adds mutlings of the added LAG members
*
* @param    lagIntf    internal interface number of the LAG interface
* @param    memIntf    internal interface number of the member to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adCounterMutlingAdd(L7_uint32 lagIntf, L7_uint32 memIntf);
/*********************************************************************
* @purpose  Removes multings of deleted LAG members from the set of 
*           LAG mutant counters 
*
* @param    lagIntf    internal interface number of the LAG interface
* @param    memIntf    internal interface number of the member to be deleted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adCounterMutlingDelete(L7_uint32 lagIntf, L7_uint32 memIntf);
/*********************************************************************
* @purpose  Resets all mutant counters for the specified LAG interface.
*
* @param    intIfNum      internal interface number of the LAG
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is not a public API function, but is intended for use
*           by the LAG component when it needs to clear out the set
*           of mutant counters for the specified LAG.  This might be
*           useful when a LAG is (re)created so that it doesn't carry
*           over obsolete information.
*       
* @notes    The LAG *external* counters are all non-resettable.
*
* @end
*********************************************************************/
L7_RC_t dot3adCounterReset(L7_uint32 intIfNum);


#endif /*INCLUDE_DOT3AD_COUNTER_H*/
