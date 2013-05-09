/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_history.h
*
* @purpose  header file for RMON history group
*           
*
* @component SNMP-RMON
*
*
* @create  08/06/2001
*
* @author  Kumar Manish 
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef RMON_HISTORY_H
#define RMON_HISTORY_H

#include <l7_common.h>
#include <stdio.h>

#include <string.h>
#include <osapi.h>

/*********************************************************************
*
* @purpose    Gets value of intIfNum for HistoryControl entry
*
* @param      Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value of 
*                           internal interface number
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlInternalInteraceNumGet (L7_uint32 Index, L7_uint32 *val);


/*********************************************************************
*
* @purpose    Gets value of Index of the History Control table corresponding to the external interface number
*
* @param      Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      this method is redundant now, if not used, should be removed later
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlInterfaceIndexGet(L7_uint32 ifIndex, L7_uint32 *val);


/*******************************************************************************
*
* @purpose    Checks index for SNMP GET requests
*
*             Index            Index of HistoryControlEntry to check
* 
* @returntype L7_SUCCESS    if valid entry
*             L7_FAILURE    if not a valid entry
*          
* @notes      none
*
* @end
*
**********************************************************************************/
L7_RC_t
rmonHistoryControlEntryGet(L7_uint32 Index);

/*********************************************************************
*
* @purpose    Checks index for SNMP GETNEXT requests
*
* @param      Index         Index of HistoryControlEntry to check
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlEntryNextGet(L7_uint32 *Index);
                                                                            

/* GET calls for HistoryControl Entries  */

/*********************************************************************
*
* @purpose    Gets value of HistoryControlDataSource for HistoryControl entry

*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlDataSourceGet (L7_uint32 Index, L7_uint32 *val);


/*********************************************************************
*
* @purpose    Gets value of HistoryControlBucketsRequested for HistoryControl entry
*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBucketsRequestedGet(L7_uint32 Index, L7_uint32 *val);


/*********************************************************************
*
* @purpose    Gets value of HistoryControlBucketsGranted for HistoryControl entry
*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBucketsGrantedGet(L7_uint32 Index, L7_uint32 *val);


/*********************************************************************
*
* @purpose    Gets value of HistoryControlInterval for HistoryControl entry
*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlIntervalGet(L7_uint32 Index, L7_uint32 *val);


/*********************************************************************
*
* @purpose    Gets value of HistoryControlOwner for HistoryControl entry
*
* @param      Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlOwnerGet (L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of HistoryControlStatus for HistoryControl entry
*
* @param      Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlStatusGet (L7_uint32 Index, L7_uint32 *val);


/* SET calls for HistoryControl Entries  */


/*********************************************************************
*
* @purpose    Sets value of HistoryControlDataSource for HistoryControl entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlDataSourceSet( L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of HistoryControlBucketsRequested for HistoryControl entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBucketsRequestedSet( L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of HistoryControlInterval for HistoryControl entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlIntervalSet( L7_uint32 Index, L7_uint32 val);


/*********************************************************************
*
* @purpose    Sets value of HistoryControlOwner for HistoryControl entry
*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlOwnerSet (L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of HistoryControlStatus for HCB entry
*
* @param      Index         index of HistoryControlEntry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlStatusSet (L7_uint32 Index, L7_uint32 val);


/**************************** Ether History Group *****************************************/

/*********************************************************************************************
*
* @purpose    Checks index for SNMP GET requests
*
* @param      intSearchType    SearchType, whether it is EXACT GET, EXCATSET, or GETNEXT
*             integerValue     it holds the value of nominator or input HistoryControlStatus,
*                              depending upon searchType
*             Index            Index of HistoryControlEntry to check
*             sampleIndex      sample index for the etherHistory entry
* 
* @returntype L7_SUCCESS    if valid entry
*             L7_FAILURE    if not a valid entry
*          
* @notes      none
*
* @end
*
**********************************************************************************************/
L7_RC_t
rmonEtherHistoryEntryGet( L7_uint32 Index, L7_uint32 SampleIndex);


/*******************************************************************************************
*
* @purpose    Checks index for SNMP GET requests
*
* @param      Index         Index of HistoryControlEntry to check
*             SampleIndex   sampleIndex of the etherHistory to check
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************************************/
L7_RC_t
rmonEtherHistoryEntryNextGet(L7_uint32 *Index, L7_uint32 *SampleIndex);


/*********************************************************************
*
* @purpose    Obtains the historySampleIndex member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistorySampleIndexGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);


/*********************************************************************
*
* @purpose    Obtains the IntervalStart member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryIntervalStartGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the no. of dropEvents member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryDropEventsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the Octets member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryOctetsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the Pkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/

L7_RC_t
rmonEtherHistoryPktsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the BroadcastPkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryBroadcastPktsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the MulticastPkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryMulticastPktsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the CRCAlignErrors member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryCRCAlignErrorsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the UndersizePkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryUndersizePktsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the OversizePkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryOversizePktsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the Fragments member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryFragmentsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the Jabbers member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryJabbersGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the Collisions member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             intIfNum      internal interface no. of that interface  
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryCollisionsGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Obtains the Utilization member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryUtilizationGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Callback function for timer to perform polling activities for RMON History group
*           to fill EtherHistory buckets
*
* @param    void
*
* @returns  L7_RC_t
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t 
rmonHistoryTimerCallback(void);

#endif  /* RMON_HISTORY_H */
