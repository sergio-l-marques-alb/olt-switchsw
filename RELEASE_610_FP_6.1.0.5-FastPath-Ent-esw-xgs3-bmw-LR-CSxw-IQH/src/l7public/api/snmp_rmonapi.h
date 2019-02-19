
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snmp_rmonapi.h
*
* @purpose  this is the header file for snmp_rmon_api.c           
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


 
#ifndef SNMP_RMONAPI_H
#define SNMP_RMONAPI_H

#include "l7_common.h"

#include "snmp_util_api.h"

/* Begin Function Prototypes */
 
 
/********************************** For Statistics Group***********************************/ 
/*********************************************************************
*
* @purpose    Checks index for snmpRmon GET requests
*
* @param      intSearchType SearchType, whether exact or next
*             UnitIndex     Index of current unit
*             Index         Index of EtherStatsEntry to check
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
snmpRmonEtherStatsEntryGet( L7_uint32 UnitIndex, L7_uint32 Index);

/*********************************************************************
*
* @purpose    Checks index for snmpRmon GET NEXT requests
*
* @param      UnitIndex     Index of current unit
*             Index         Index of EtherStatsEntry to check
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
snmpRmonEtherStatsEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *Index);

/*********************************************************************
*
* @purpose    Gets the  internal interfcae number corresponding to Index 
*
* @param      Index                 Index of the EtherStatsEntry 
*             intIfIndex(output)    internal Interface number 
*
* @returntype L7_SUCCESS    if internal Interface number found
*             L7_FAILURE    if internal Interface number not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
snmpRmonIntIfNumGet(L7_uint32 Index, L7_uint32 *intIfIndex);

/*********************************************************************
*
* @purpose    Converts the external interface number(ifIndex number) corresponding to Index 
*             to internal interfcae number
*
* @param      Index                 Index of the EtherStatsEntry 
*             intIfIndex(output)    internal Interface number 
*
* @returntype L7_SUCCESS    if internal Interface number found
*             L7_FAILURE    if internal Interface number not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
snmpRmonIntIfNumFromExtNum(L7_uint32 Index, L7_uint32 *intIfIndex);

/*********************************************************************
*
* @purpose    Gets value of DataSource for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t
snmpRmonEtherStatsDataSourceGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of DropEvents for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsDropEventsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);
 
/*********************************************************************
*
* @purpose    Gets value of Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsOctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Pkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
***************************************************************************/
L7_RC_t
snmpRmonEtherStatsPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);


/*********************************************************************
*
* @purpose    Gets value of Broadcast Pkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsBroadcastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Multicast Pkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsMulticastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of CRCAlignErrors for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsCRCAlignErrorsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of UndersizePkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsUndersizePktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of OversizePkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsOversizePktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Fragments for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsFragmentsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);
 
/*********************************************************************
*
* @purpose    Gets value of Jabbers for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsJabbersGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Collisions for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsCollisionsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Pkts64Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsPkts64OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Pkts65to127Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsPkts65to127OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Pkts128to255Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsPkts128to255OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Pkts256to511Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsPkts256to511OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Pkts512to1023Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsPkts512to1023OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Pkts1024to1518Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsPkts1024to1518OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Owner for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         index  for the EtherStatEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsOwnerGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of Status for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         index  for the EtherStatEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
snmpRmonEtherStatsStatusGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);


/* SET calls for EtherStats Entries  */

/*********************************************************************
*
* @purpose    Sets value of DataSource for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the EtherStatEntry
*             val(input)    L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t
snmpRmonEtherStatsDataSourceSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Owner for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the EtherStatEntry
*             buf(input)    L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/

L7_RC_t
snmpRmonEtherStatsOwnerSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of Status for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the EtherStatEntry
*             val(input)    L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t
snmpRmonEtherStatsStatusSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);


/********************************** For History Group***********************************/


 /*********************************************************************
*
* @purpose    Checks index for snmpRmon GET requests
*
*             UnitIndex     Index of current unit
*             Index         Index of HistoryControlEntry to check
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
snmpRmonHistoryControlEntryGet(L7_uint32 UnitIndex, L7_uint32 Index);

/*********************************************************************
*
* @purpose    Checks index for snmpRmon GET NEXT requests
*
* @param      UnitIndex     Index of current unit
*             Index         Index of HistoryControlEntry to check
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
snmpRmonHistoryControlEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *Index);

/*********************************************************************
*
* @purpose    Gets value of DataSource for HistoryControl entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlDataSourceGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of HistoryControlBucketsRequested for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlBucketsRequestedGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of HistoryControlBucketsGranted for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlBucketsGrantedGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of HistoryControlInterval for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlIntervalGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of HistoryControlOwner for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlOwnerGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of HistoryControlStatus for HistoryControlEntry
*
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlStatusGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);


/* SET calls for HistoryControl Entries */


/*********************************************************************
*
* @purpose    Sets value of HistoryControlDataSource for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlDataSourceSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of HistoryControlBucketsRequested for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlBucketsRequestedSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of HistoryControlInterval for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlIntervalSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of HistoryControlOwner for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlOwnerSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of HistoryControlStatus for HistoryControlEntry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
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
snmpRmonHistoryControlStatusSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);


/* GET calls for EtherHistory entries */

/*********************************************************************
*
* @purpose    Checks index for snmpRmon GET requests
*
* @param      UnitIndex     Index of current unit
*             Index         Index of HistoryControlEntry to check
*             SampleIndex   Index of EtherHistoryEntry(History Bucket) to check
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
snmpRmonEtherHistoryEntryGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex);

/*********************************************************************
*
* @purpose    Checks index for snmpRmon GET NEXT requests
*
* @param      UnitIndex     Index of current unit
*             Index         Index of HistoryControlEntry to check
*             SampleIndex   Index of EtherHistoryEntry(History Bucket) to check
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
snmpRmonEtherHistoryEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *Index, 
                                 L7_uint32 *SampleIndex);

/*********************************************************************
*
* @purpose    Gets value of historySampleIndex for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistorControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistorySampleIndexGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);


/*********************************************************************
*
* @purpose    Gets value of IntervalStart for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistorControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryIntervalStartGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of DropEvents for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryDropEventsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Octets for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryOctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Pkts for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of BroadcastsPkts for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryBroadcastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of MulticastPkts for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryMulticastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of CRCAlignErrors for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryCRCAlignErrorsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex,L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of UndersizePkts for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryUndersizePktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of OversizePkts for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryOversizePktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex,  L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of FrgmentsPkts for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryFragmentsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of JabbersPkts for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryJabbersGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Collisions for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryCollisionsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Utilization for History Bucket entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControlEntry to set
*             SampleIndex   index of History Bucket to set
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
snmpRmonEtherHistoryUtilizationGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val);


/********************************** For Alarm Group***********************************/ 
/*********************************************************************
*
* @purpose    Checks index for the valid Alarm Entry
*
* @param      UnitIndex     Index of current unit
*             Index         Index of AlarmEntry to check
*             searchType    L7_uint32
*             integerValue  L7_int32
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
snmpRmonAlarmEntryGet(L7_uint32 searchType, L7_uint32 UnitIndex, L7_uint32 Index);

/*********************************************************************
*
* @purpose    Checks index for  GET NEXT requests
*
* @param      UnitIndex     Index of current unit
*             Index         Index of AlarmEntry to check
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
snmpRmonAlarmEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *Index);

/*********************************************************************
*
* @purpose    Gets value of Interval for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmIntervalGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Variable for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             buf(output)   pointer to L7_char8
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmVariableGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of SampleType for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmSampleTypeGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of AlarmValue for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmValueGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of StartUpAlarm for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmStartupAlarmGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of StartUpAlarm for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmStartupAlarmGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of RisingThreshold for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmRisingThresholdGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of FallingThreshold for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmFallingThresholdGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of RisingEventIndex for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmRisingEventIndexGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of FallingEventIndex for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmFallingEventIndexGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Owner for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmOwnerGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of Status for Alarm Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of AlarmEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmStatusGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Sets value of Interval for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmIntervalSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Variable for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             buf(input)   pointer to L7_char8
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmVariableSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of SampleType for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmSampleTypeSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of StartUpAlarm for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmStartupAlarmSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of StartUpAlarm for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmStartupAlarmSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Rising Threshold for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmRisingThresholdSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Falling Threshold for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmFallingThresholdSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Rising Event Index for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmRisingEventIndexSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Falling Event Index for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmFallingEventIndexSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Owner for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             buf(input)    pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmOwnerSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of Status for Alarm entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the AlarmEntry
*             val(input)    L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonAlarmStatusSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/********************************** For Event Group***********************************/ 
/*********************************************************************
*
* @purpose    Checks index for the valid Event Entry
*
* @param      UnitIndex     Index of current unit
*             Index         Index of EventEntry to check
*             searchType    L7_uint32
*             integerValue  L7_int32
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
snmpRmonEventEntryGet(L7_uint32 searchType, L7_uint32 UnitIndex, L7_uint32 Index);

/*********************************************************************
*
* @purpose    Checks index for the next valid Event Entry
*             immediately after the Index 
*
* @param      UnitIndex     Index of current unit
*             Index         Index of EventEntry to check
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
snmpRmonEventEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *Index);

/*********************************************************************
*
* @purpose    Gets value of Description for Event Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of EventEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventDescriptionGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of EventType for Event Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventTypeGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Community for Event Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of EventEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventCommunityGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of LastTimeSent for Event Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventLastTimeSentGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Owner for Event Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of EventEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventOwnerGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of Status for Event Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventStatusGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Sets value of Description for Event entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the EventEntry
*             buf(input)    pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventDescriptionSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of EventType for Event entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the EventEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventTypeSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of Community for Event entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the EventEntry
*             buf(input)    pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventCommunitySet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of Owner for Event entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the EventEntry
*             buf(input)    pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventOwnerSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of Status for Event entry
*
* @param      UnitIndex     index of current unit
*             Index         index of the EventEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonEventStatusSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);


/********************************** For LOG Table***********************************/ 


/*********************************************************************
*
* @purpose    Checks index for the valid Log Entry
*
* @param      UnitIndex     Index of current unit
*             Index         Index of LogEntry to check
*             searchType    L7_uint32
*             integerValue  L7_int32
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
snmpRmonLogEntryGet(L7_uint32 searchType, L7_uint32 UnitIndex, L7_uint32 logEventIndex, L7_uint32 Index);

/*********************************************************************
*
* @purpose    Checks index for the next valid Log Entry
*             immediately after the Index 
*
* @param      UnitIndex     Index of current unit
*             Index         Index of LogEntry to check
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
snmpRmonLogEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *logEventIndex, L7_uint32 *Index);

/*********************************************************************
*
* @purpose    Gets value of LogTime for Log Table Entry
*
* @param      UnitIndex     index of current unit
*             Index         index of LogEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
snmpRmonLogTimeGet(L7_uint32 UnitIndex, L7_uint32 logEventIndex, L7_uint32 Index, L7_uint32 *val);

L7_RC_t
snmpRmonLogDescriptionGet(L7_uint32 UnitIndex, L7_uint32 logEventIndex, L7_uint32 Index, L7_char8 *buf);


#endif /* SNMP_RMONAPI_H */
