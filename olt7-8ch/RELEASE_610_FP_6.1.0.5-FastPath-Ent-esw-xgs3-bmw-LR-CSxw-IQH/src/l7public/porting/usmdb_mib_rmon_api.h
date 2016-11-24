/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_mib_rmon_api.h
*
* @purpose usmdb support for LVL7 extensions to RFC1271 RMON support
*
* @component unitmgr
*
* @comments tba
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_MIB_RMON_API_H
#define USMDB_MIB_RMON_API_H
    
/* etherStats */
/*********************************************************************
* @purpose  Get a particular Ethernet interface statistics.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next Ethernet interface statistics.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the data source that this etherStats entry is configured to analyze.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        
* @param    *buf_len
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This source can be any ethernet interface on the device.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsDataSourceGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val);

/*********************************************************************
* @purpose  Returns the number of dropped events.   
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        dropped events
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The total number of events in which packets were dropped 
*           by the probe due to lack of resources. 
*       
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsDropEventsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of octets of data.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Octets of data (including those in bad packets) received on the
*           network (excluding framing bits but including FCS octets).
*       
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsOctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of packets (including error packets) received.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*       
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of good Broadcast packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Number includes packets that are directed to the broadcast address.
*       
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsBroadcastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of good Multicast packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Number includes packets that are directed to the multicast address only.
*       
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsMulticastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets received have a length (excluding framing bits,
*           including FCS octets) of between 64 and 1518 octets inclusive.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsCRCAlignErrorsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of Undersize packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are less than 64 octets long (excluding framing bits,
*           including FCS octets).
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsUndersizePktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of Oversize packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are longer than 1518 octets (excluding framing bits,
*           including FCS octets). 
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsOversizePktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of Fragment packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are not an integral number of octets in length or had a bad
*           Frame Check Sequence (FCS), and are less than 64 octets in length  
*           (excluding framing bits, including FCS octets).
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsFragmentsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of Jabber packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are longer than 1518 octets (excluding framing bits, 
*           including FCS octets), and are not an integral number of octets 
*           in length or had a bad Frame Check Sequence (FCS).
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsJabbersGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of collisions on the Ethernet segment.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of collisions
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsCollisionsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of specified octet length packets including error packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are 64 octets in length (excluding framing bits, including FCS octets)
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsPkts64OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of specified octet range packets including error packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are between 65 and 127 octets in length inclusive,
*           (excluding framing bits, including FCS octets)
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsPkts65to127OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of specified octet range packets including error packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are between 128 and 255 octets in length inclusive,
*           (excluding framing bits, including FCS octets)
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsPkts128to255OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of specified octet range packets including error packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are between 256 and 511 octets in length inclusive,
*           (excluding framing bits, including FCS octets)
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsPkts256to511OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of specified octet range packets including error packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are between 512 and 1023 octets in length inclusive,
*           (excluding framing bits, including FCS octets)
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsPkts512to1023OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of specified octet range packets including error packets.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are between 1024 and 1518 octets in length inclusive,
*           (excluding framing bits, including FCS octets)
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsPkts1024to1518OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the entity that configured this entry.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *buf        entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the etherStats entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the data source that this etherStats entry is configured to analyze.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        
* @param    buf_len     
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsDataSourceSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 ifIndex);

/*********************************************************************
* @purpose  Set the entity that configured this entry.
*
* @param    UnitIndex   Unit
* @param    Index       interface
* @param    *buf        entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the etherStats entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherStatsStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* historyControl */
/*********************************************************************
* @purpose  Get the list of parameters that set up a periodic sampling of statistics.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next list of parameters that set up a periodic sampling of statistics.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the data source for which historical data was collected.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        
* @param    *buf_len    
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This source can be any interface on this device.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlDataSourceGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the requested number of discrete time intervals
*           over which data is to be saved.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of time intervals
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Data is to be saved in part of media-specific table associated 
*           with this historyControl entry.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlBucketsRequestedGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of discrete sampling intervals
*           over which data shall be saved.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of sampling intervals
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Data is to be saved in part of media-specific table associated 
*           with this historyControl entry.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlBucketsGrantedGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the control time interval in seconds over which 
*           the data is sampled for each bucket.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of time intervals
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Data is sampled for each bucket in part of media-specific table  
*           associated with this historyControl entry. 
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlIntervalGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the entity that configured this entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the status of this historyControl entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the status of this historyControl entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlInternalInterfaceNumGet ( L7_int32 Index, L7_int32 *val );
/*********************************************************************
* @purpose  Set the data source.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        
* @param    buf_len    
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This source can be any interface on this device.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlDataSourceSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 val);

/*********************************************************************
* @purpose  Set the requested number of discrete time intervals
*           over which data is to be saved.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         number of time intervals
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlBucketsRequestedSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the control time interval in seconds over which 
*           the data is sampled for each bucket.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of sampling intervals
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Data is sampled for each bucket in part of media-specific table associated 
*           with this historyControl entry. This interval can be set to any number  
*           of seconds between 1 and 3600 (1 hour).
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlIntervalSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the entity that configured this entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the status of this historyControl entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHistoryControlStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* etherHistory */
/*********************************************************************
* @purpose  Get the historical sample of Ethernet statistics on a
*           particular Ethernet interface.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    SampleIndex sample of Ethernet statistics
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This sample is associated with the historyControlEntry which set
*           up the parameters for a regular collection of these samples.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryEntryGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex );

/*********************************************************************
* @purpose  Get the next historical sample of Ethernet statistics on a
*           particular Ethernet interface.  
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *Index          interface
* @param    *SampleIndex    sample of Ethernet statistics
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This sample is associated with the historyControlEntry which set
*           up the parameters for a regular collection of these samples.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryEntryNext ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *SampleIndex );

/*********************************************************************
* @purpose  Get the value of sysUpTime at the start of interval
*           over which this sample was measured.  
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          sysUpTime value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If the probe keeps track of the time of day, it should start the first 
*           sample of the history at a time such that when the next hour of the 
*           day begins, a sample is started at that instant.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryIntervalStartGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of dropped packets.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of dropped packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This number is not necessarily the number of packets dropped, 
*           it is just the number of times this condition has been detected.   
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryDropEventsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of octets of data.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of octets of data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This number includes data in bad packets received on the network,
*           (excluding framing bits, including FCS octets).
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryOctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of EtherHistory packets received during sampling interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of EtherHistory packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Number of packets include error packets.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of good Broadcast packets received during sampling interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of Broadcast packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are directed to the broadcast address.    
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryBroadcastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of good Multicast packets received during sampling interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of Multicast packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are directed to the multicast address.    
*           This number does not include packets addressed to the broadcast address.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryMulticastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of specified octet length packets received during sampling interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of specified octet length packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets have a length (excluding framing bits, including FCS octets) 
*           between 64 and 1518 octets, inclusive, but are not an integral 
*           number of octets in length or have a bad Frame Check Sequence.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryCRCAlignErrorsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of specified octet length Undersize packets
*           received during sampling interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of Undersize packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are less than 64 octets long (excluding framing bits, 
*           including FCS octets) and are otherwise well formed.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryUndersizePktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of specified octet length Oversize packets
*           received during sampling interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of Oversize packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are longer than 1518 octets (excluding framing bits, 
*           including FCS octets) and are otherwise well formed.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryOversizePktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of fragment packets received during sampling interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of fragment packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are not an integral number of octets in length or had a 
*           bad Frame Check Sequence (FCS), and are less than 64 octets 
*           in length (excluding framing bits, including FCS octets).
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryFragmentsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of Jabber packets received during this interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of Jabber packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are longer than 1518 octets (excluding framing bits.
*           including FCS octets), and are not an integral number of 
*           octets in length or have a bad Frame Check Sequence (FCS).
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryJabbersGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the total number of collisions on the Ethernet segment during this interval.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          number of collisions
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryCollisionsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the mean physical layer network utilization on this interface.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    Index         interface
* @param    SampleIndex   sample of Ethernet statistics
* @param    *val          network utilization
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Utilization on the interface during this interval is in hundredths of a percent.
*
* @end
*********************************************************************/
L7_RC_t usmDbEtherHistoryUtilizationGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 SampleIndex, L7_int32 *val );


/* alarm */
/*********************************************************************
* @purpose  Get the Alarm Entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a list of parameters that set up a periodic checking
*           for alarm conditions.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next Alarm Entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the alarm interval in seconds.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        time interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    During this interval data is sampled and compared 
*           with rising and falling thresholds.     
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmIntervalGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the alarm variable.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        
* @param    *buf_len    
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The object identifier of the particular variable to be sampled.  
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmVariableGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf);

/*********************************************************************
* @purpose  Get the alarm sample type.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        object value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If value of this object is absoluteValue(1), the value of selected variable
*               will be compared directly with the thresholds at end of the sampling interval.  
*           If value of this object is deltaValue(2), the value of selected variable
*               at the last sample will be subtracted from current value, 
*               and difference compared with the thresholds.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmSampleTypeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the alarm value, the value of statistic during 
*           the last sampling period.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        object value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmValueGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the alarm Startup Alarm.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        Startup Alarm
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The alarm that may be sent when the entry is first set to valid.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmStartupAlarmGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the alarm rising threshold for the sampled statistic.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        alarm rising threshold
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes      When the current sampled value is greater than or equal to
*             this threshold, and value at last sampling interval was 
*             less than this threshold, a single event will be generated.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmRisingThresholdGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the alarm falling threshold for the sampled statistic.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        alarm falling threshold
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes      When the current sampled value is less than or equal to
*             this threshold, and value at last sampling interval was 
*             greater than this threshold, a single event will be generated.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmFallingThresholdGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the index of eventEntry that is used when a rising threshold is crossed.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        alarm rising event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this value is zero, no associated event will be generated,
*           as zero is not a valid event index.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmRisingEventIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the index of eventEntry that is used when a falling threshold is crossed.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        alarm falling event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this value is zero, no associated event will be generated,
*           as zero is not a valid event index.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmFallingEventIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the entity that configured the entry and is using the resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the status of the alarm entry. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        alarm status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the alarm interval.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         time interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    During this interval data is sampled and compared 
*           with rising and falling thresholds.     
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmIntervalSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the alarm variable.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        
* @param    buf_len    
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The object identifier of the particular variable to be sampled.  
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmVariableSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf);

/*********************************************************************
* @purpose  Set the alarm sample type.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         object value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If value of this object is absoluteValue(1), the value of selected variable
*               will be compared directly with the thresholds at end of the sampling interval.  
*           If value of this object is deltaValue(2), the value of selected variable
*               at the last sample will be subtracted from current value, 
*               and difference compared with the thresholds.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmSampleTypeSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the alarm Startup Alarm.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         Startup Alarm
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The alarm that may be sent when this entry is first set to valid.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmStartupAlarmSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the alarm rising threshold for the sampled statistic.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         alarm rising threshold
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes      When the current sampled value is greater than or equal to
*             this threshold, and value at last sampling interval was 
*             less than this threshold, a single event will be generated.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmRisingThresholdSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the alarm falling threshold for the sampled statistic.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         alarm falling threshold
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes      When the current sampled value is less than or equal to
*             this threshold, and value at last sampling interval was 
*             greater than this threshold, a single event will be generated.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmFallingThresholdSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the index of eventEntry that is used when a rising threshold is crossed.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         alarm rising event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this value is zero, no associated event will be generated,
*           as zero is not a valid event index.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmRisingEventIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the index of eventEntry that is used when a falling threshold is crossed.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         alarm falling event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this value is zero, no associated event will be generated,
*           as zero is not a valid event index.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmFallingEventIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the entity that configured the entry and is using the resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the status of the alarm entry. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        alarm status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbAlarmStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* hostControl */
/*********************************************************************
* @purpose  Get the Host Control Entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a list of parameters that set up the discovery of hosts on a
*           particular interface and the collection of statistics about these hosts.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next Host Control Entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a list of parameters that set up the discovery of hosts on a
*           particular interface and the collection of statistics about these hosts.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the Host Control Data Source.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        
* @param    *buf_len
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This source can be any interface on the device.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlDataSourceGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *buf, L7_uint32 *buf_len );

/*********************************************************************
* @purpose  Get the number of hostEntries in the hostTable and 
*           hostTimeTable associated with the hostControlEntry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of hostEntries
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlTableSizeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the value of sysUpTime when last entry was deleted from
*           the hostTable associated with the hostControlEntry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        delete time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If no deletions have occurred, the value shall be zero.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlLastDeleteTimeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the hostControl entry status. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the Host Control Data Source.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        
* @param    buf_len
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This source can be any interface on the device.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlDataSourceSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *buf, L7_uint32 buf_len );

/*********************************************************************
* @purpose  Set the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the hostControl entry status. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostControlStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* host */
/*********************************************************************
* @purpose  Get the host entry. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a collection of statistics for a particular host
*           that has been discovered on an interface of the device.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostEntryGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address );

/*********************************************************************
* @purpose  Get the next host entry. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
* @param    *Address    host address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a collection of statistics for a particular host
*           that has been discovered on an interface of the device.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index, L7_char8 *Address );

/*********************************************************************
* @purpose  Get the host creation order, relative ordering of creation 
*           time of hosts captured for a particular hostControlEntry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host address 
* @param    *val        creation time of hosts
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbHostCreationOrderGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of packets without errors transmitted to
*           the address since it was added to the hostTable.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host address 
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostInPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of packets including errors transmitted to
*           the address since it was added to the hostTable.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host address 
* @param    *val        number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostOutPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of good octets transmitted to the address since it was
*           added to the hostTable (excluding framing bits, including FCS octets)
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host address 
* @param    *val        number of good octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostInOctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of all octets transmitted to the address since it was
*           added to the hostTable (excluding framing bits, including FCS octets)
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host address 
* @param    *val        number of octets including those with errors
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostOutOctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of error packets transmitted by the
*           address since the host was added to the hostTable.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host address 
* @param    *val        number of error packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostOutErrorsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of good Broadcast packets.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host address 
* @param    *val        number of Broadcast packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are transmitted by the address and are directed to the 
*           broadcast address since the host was added to the hostTable.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostOutBroadcastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of good Multicast packets.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *Address    host address 
* @param    *val        number of Multicast packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are transmitted by the address and are directed to the 
*           broadcast address since the host was added to the hostTable.
*           This number does not include packets directed to the broadcast address.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostOutMulticastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *Address, L7_uint32 *val );


/* hostTime */
/*********************************************************************
* @purpose  Get the Host Time Entry. 
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a collection of statistics for a particular host
*           that has been discovered on an interface of the device.      
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeEntryGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder );

/*********************************************************************
* @purpose  Get the next Host Time Entry. 
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a collection of statistics for a particular host
*           that has been discovered on an interface of the device.      
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index, L7_int32 *CreationOrder );

/*********************************************************************
* @purpose  Get the physical address of this host.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time 
* @param    *buf            physical address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeAddressGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the number of packets without errors transmitted to
*           the address since it was added to the hostTimeTable.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time 
* @param    *val            number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeInPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of packets including errors transmitted to
*           the address since it was added to the hostTimeTable.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time 
* @param    *val            number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeOutPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of good octets transmitted to the address since it was
*           added to the hostTimeTable (excluding framing bits, including FCS octets).
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time 
* @param    *val            number of good octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeInOctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of all octets transmitted to the address since it was
*           added to the hostTimeTable (excluding framing bits, including FCS octets).
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time 
* @param    *val            number of octets including those with errors
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeOutOctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of error packets transmitted by the
*           address since the host was added to the hostTimeTable.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time 
* @param    *val            number of error packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeOutErrorsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of good Broadcast packets.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time 
* @param    *val            number of Broadcast packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are transmitted by the address and are directed to the 
*           broadcast address since the host was added to the hostTimeTable.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeOutBroadcastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of good Multicast packets.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
* @param    CreationOrder   host time 
* @param    *val            number of Multicast packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Packets are transmitted by the address and are directed to the 
*           broadcast address since the host was added to the hostTimeTable.
*           This number does not include packets directed to the broadcast address.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTimeOutMulticastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 CreationOrder, L7_uint32 *val );


/* hostTopNControl */
/*********************************************************************
* @purpose  Get the top N host control entry.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    Index           interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters that controls the creation of a
*           report of the top N hosts according to several metrics.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNControlEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next top N host control entry.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *Index          interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters that controls the creation of a
*           report of the top N hosts according to several metrics.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNControlEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the top N host index, the host table for which a top N 
*           report will be prepared on behalf of the entry.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        host index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNHostIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the variable for each host that the hostTopNRate
*           variable is based upon.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        host variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNRateBaseGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of seconds left in the report currently being collected.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    While the value of this object is non-zero, it decrements by 
*           one per second until it reaches zero. During this time, all
*           associated hostTopNEntries shall remain inaccessible.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNTimeRemainingGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of seconds that this report has (or is being) 
*           collected during the last (or this) sampling interval.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNDurationGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the maximum number of hosts requested for the top N table.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        maximum number of requested hosts
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNRequestedSizeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the maximum number of hosts in the top N table.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        maximum number of hosts
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNGrantedSizeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the value of sysUpTime when the top N report was last started.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        value of sysUpTime
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNStartTimeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the entity that configured the entry and is using the resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity resource
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the hostTopNControl entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the top N host index, the host table for which a top N 
*           report will be prepared on behalf of the entry.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         host index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNHostIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the variable for each host that the hostTopNRate
*           variable is based upon.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         host variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNRateBaseSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the number of seconds left in the report currently being collected.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         number of seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    When this object is set to a non-zero value, any associated hostTopNEntries
*           shall be made inaccessible by the monitor. While the value of this object
*           is non-zero, it decrements by one per second until it reaches zero.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNTimeRemainingSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the maximum number of hosts requested for the top N table.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         maximum number of hosts
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNRequestedSizeSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the entity that configured the entry and is using the resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity resource
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the hostTopNControl entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.  
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* hostTopN */
/*********************************************************************
* @purpose  Get the top N host entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Report      host report
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of statistics for a host that is part of a top N report.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNEntryGet ( L7_uint32 UnitIndex, L7_int32 Report, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next top N host entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Report     host report
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of statistics for a host that is part of a top N report.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNEntryNext ( L7_uint32 UnitIndex, L7_int32 *Report, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the physical address of the host.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Report      host report
* @param    Index       interface
* @param    *buf        physical address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNAddressGet ( L7_uint32 UnitIndex, L7_int32 Report, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the amount of change in the selected variable during sampling interval.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Report      host report
* @param    Index       interface
* @param    *val        amount of change (a variable)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The selected variable is the host's instance of object selected by hostTopNRateBase.
*
* @end
*********************************************************************/
L7_RC_t usmDbHostTopNRateGet ( L7_uint32 UnitIndex, L7_int32 Report, L7_int32 Index, L7_int32 *val );


/* matrixControl */
/*********************************************************************
* @purpose  Get the entry for the traffic matrix.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is the information about a traffic matrix on a particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next entry for the traffic matrix.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is the information about a traffic matrix on a particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the source of data from which the entry creates a traffic matrix.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        data source
* @param    *buf_len    data length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This source can be any interface on the device.  
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlDataSourceGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *buf, L7_uint32 *buf_len );

/*********************************************************************
* @purpose  Get the number of matrixSDEntries in matrixSDTable for the interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of matrixSDEntries
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This must also be the value of the number of entries in 
*           matrixDSTable for this interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlTableSizeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the value of sysUpTime when last entry was deleted from
*           the matrixSDTable associated with the matrixControlEntry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        value of sysUpTime
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If no deletions have occurred, this value shall be zero.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlLastDeleteTimeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity resource
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the matrixControl entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the source of data from which the entry creates a traffic matrix.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        data source
* @param    buf_len     data length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This source can be any interface on the device.  
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlDataSourceSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *buf, L7_uint32 buf_len );

/*********************************************************************
* @purpose  Set the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entity resource
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the matrixControl entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixControlStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* matrixSD */
/*********************************************************************
* @purpose  Get the traffic matrix entry indexed by source and destination MAC address.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a collection of statistics for communications between
*           two addresses on a particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixSDEntryGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress );

/*********************************************************************
* @purpose  Get the next traffic matrix entry indexed by source and destination MAC address.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a collection of statistics for communications between
*           two addresses on a particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixSDEntryNext ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress );

/*********************************************************************
* @purpose  Get the set of collected matrix statistics of which the entry is a part.  
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
* @param    *val            matrix index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixSDIndexGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of packets (including error packets) transmitted 
*           from source address to destination address.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
* @param    *val            number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixSDPktsGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of octets transmitted from source address to the
*           destination address in all packets.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
* @param    *val            number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixSDOctetsGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of error packets transmitted from the
*           source address to the destination address.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
* @param    *val            number of error packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixSDErrorsGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress, L7_uint32 *val );


/* matrixDS */
/*********************************************************************
* @purpose  Get the traffic matrix entry indexed by destination and source MAC address.
                  
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a collection of statistics for communications between
*           two address on a particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixDSEntryGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress );

/*********************************************************************
* @purpose  Get the next traffic matrix entry indexed by destination and source MAC address.
                  
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a collection of statistics for communications between
*           two address on a particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixDSEntryNext ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress );

/*********************************************************************
* @purpose  Get the set of collected matrix statistics of which the entry is a part.  
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
* @param    *val            matrix statistics
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixDSIndexGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of packets (including error packets) transmitted 
*           from the source address to the destination address.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
* @param    *val            number of packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixDSPktsGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of octets in all packets transmitted from the 
*           source address to the destination address.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
* @param    *val            number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixDSOctetsGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of error packets transmitted from the
*           source address to the destination address.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *SourceAddress  source MAC address
* @param    *DestAddress    destination MAC address
* @param    *val            number of error packets            
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbMatrixDSErrorsGet ( L7_uint32 UnitIndex, L7_char8 *SourceAddress, L7_char8 *DestAddress, L7_uint32 *val );


/* filter */
/*********************************************************************
* @purpose  Get the packet filter entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters for a packet filter applied on a
*           particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next packet filter entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters for a packet filter applied on a
*           particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the channel of which the filter is a part.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        channel index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterChannelIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the packet data offset from the beginning of each packet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        packet data offset
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This offset is measured from the point in physical layer
*           packet after the framing bits, if any.  
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktDataOffsetGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the input packet data.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        packet data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    For each packet received, this filter and accompanying 
*           filterPktDataMask and filterPktDataNotMask will be adjusted for offset.      
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktDataGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the Filter Packet Data Mask.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        packet data mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The mask is applied to the match process.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktDataMaskGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the Filter Packet Data inversion Mask.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        packet data inversion mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The inversion mask is applied to the match process.  
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktDataNotMaskGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the filter packet status that is to be matched with the input packet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        filter packet status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The only bits relevant to this match algorithm are those that 
*           have the corresponding filterPktStatusMask bit equal to one.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the filter packet mask that is applied to the status match process.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        filter packet mask status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Only those bits in the received packet that correspond to bits set in
*           this mask are relevant for further processing by status match algorithm.      
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktStatusMaskGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the filter packet inversion mask that is applied to status match process.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        filter packet inversion mask status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktStatusNotMaskGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entry entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the filter entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        filter entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the channel of which the filter is a part.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         channel index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterChannelIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the packet data offset from the beginning of each packet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         packet data offset
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This offset is measured from the point in physical layer
*           packet after the framing bits, if any.  
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktDataOffsetSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the input packet data.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        packet data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktDataSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the Filter Packet Data Mask.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        packet data mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The mask is applied to the match process.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktDataMaskSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the Filter Packet Data inversion Mask.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        packet data inversion mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The inversion mask is applied to the match process.  
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktDataNotMaskSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the filter packet status that is to be matched with the input packet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         filter packet status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    The only bits relevant to this match algorithm are those that 
*           have the corresponding filterPktStatusMask bit equal to one.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the filter packet mask that is applied to the status match process.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         filter packet mask status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Only those bits in the received packet that correspond to bits set in
*           this mask are relevant for further processing by the status match algorithm.      
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktStatusMaskSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the filter packet inversion mask that is applied to status match process.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         filter packet inversion mask status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterPktStatusNotMaskSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entry entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the filter entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         filter entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* channel */
/*********************************************************************
* @purpose  Get a packet channel entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters for a packet channel applied on a
*           particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next packet channel entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters for a packet channel applied on a
*           particular interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the channel interface index.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        interface index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This object identifies the interface on the remote network 
*           monitoring device to which the associated filters are applied 
*           to allow data into the channel.  
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelIfIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the action of the filters associated with this channel. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        accept type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this object is equal to acceptMatched(1), packets are accepted to channel
*               if they are accepted by both packet data and packet status matches of  
*               an associated filter. 
*           If this object is equal to acceptFailed(2), packets are accepted to channel
*               only if they fail either packet data match or packet status match of
*               each of the associated filters.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelAcceptTypeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the channel's data flow control.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        data control value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this object is on(1), data, status, and events flow through the channel.
*           If this object is off(2), data, status, and events will not flow through the channel.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelDataControlGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the channel event index that is configured to turn the associated
*           channelDataControl from off to on.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelTurnOnEventIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the channel event index that is configured to turn the associated
*           channelDataControl from on to off.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelTurnOffEventIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the channel event index when the associated channelDataControl 
*           is on and a packet is matched.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelEventIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the channel event status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        event status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none. 
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelEventStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of times the channel has matched a packet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of channel packet matches
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This object is updated even when channelDataControl is set to off.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelMatchesGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the channel describing comment.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        channel description
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelDescriptionGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        channel entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the channel entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        channel status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the channel interface index.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         interface index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This object identifies the interface on the remote network 
*           monitoring device to which the associated filters are applied 
*           to allow data into the channel.  
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelIfIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the action of the filters associated with this channel. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         accept type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this object is equal to acceptMatched(1), packets are accepted to channel
*               if they are accepted by both packet data and packet status matches of  
*               an associated filter. 
*           If this object is equal to acceptFailed(2), packets are accepted to channel
*               only if they fail either packet data match or packet status match of
*               each of the associated filters.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelAcceptTypeSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the channel's data flow control.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         data control value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this object is on(1), data, status, and events flow through the channel.
*           If this object is off(2), data, status, and events will not flow through the channel.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelDataControlSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the event index that is configured to turn the associated
*           channelDataControl from off to on.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If no event is intended for the channel, channelTurnOnEventIndex must be
*           set to zero, a non-existent event index.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelTurnOnEventIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the event index that is configured to turn the associated
*           channelDataControl from on to off.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         event index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If no event is intended for the channel, channelTurnOnEventIndex must be
*           set to zero, a non-existent event index.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelTurnOffEventIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the channel event index when the associated channelDataControl 
*           is on and a packet is matched.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         event index        
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If no event is intended for the channel, channelEventIndex must be
*           set to zero, a non-existent event index.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelEventIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the channel event status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         event status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This object may be set to eventAlwaysReady(3), to disable flow control 
*           and allow events to be generated at will. 
*           (But this can result in high network traffic or other performance problems.)
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelEventStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the channel describing comment.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        channel description
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelDescriptionSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        channel entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the channel entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         channel status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbChannelStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* bufferControl */
/*********************************************************************
* @purpose  Get a buffer control entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters that control the collection of
*           a stream of packets that have matched filters.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next buffer control entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Index      interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters that control the collection of
*           a stream of packets that have matched filters.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the control index that identifies the channel that 
*           is the source of packets for the bufferControl table. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        channel control index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlChannelIndexGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the buffer control status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        status value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If status is spaceAvailable(1), buffer is accepting new packets.
*           If status is full(2) and bufferControlFullAction object is wrapWhenFull, 
*               buffer is accepting new packets by deleting oldest packets.  
*           If status is full(2) and bufferControlFullAction object is lockWhenFull,
*               buffer stops collecting packets.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlFullStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the buffer control action state.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        control state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlFullActionGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the maximum number of octets of each packet
*           that will be saved in the capture buffer.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        maximum number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlCaptureSliceSizeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the maximum number of octets of each packet in capture 
*           buffer that will be returned in an SNMP retrieval of that packet.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        maximum number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlDownloadSliceSizeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the offset of the first octet of each packet in capture 
*           buffer that will be returned in an SNMP retrieval of that packet.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        offset of the first octet
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlDownloadOffsetGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the requested maximum number of octets to be saved in
*           the captureBuffer, including any implementation-specific overhead. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        requested maximum number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlMaxOctetsRequestedGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the maximum number of octets that can be saved
*           in the captureBuffer, including overhead.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        requested maximum number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this variable is -1, the capture buffer will save
*           as many octets as possible.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlMaxOctetsGrantedGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of packets currently in this captureBuffer.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        number of packets in capture buffer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlCapturedPacketsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the value of sysUpTime when the capture buffer was first turned on.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        turn on time 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlTurnOnTimeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entry entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the buffer control entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the channel control index.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         channel control index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This object may not be modified if the associated
*           bufferControlStatus object is equal to valid(1).
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlChannelIndexSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the buffer control action state.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         control state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    When in the lockWhenFull(1) state a packet is added to the buffer,
*           bufferControlFullStatus is set to full(2) and buffer stops capturing packets.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlFullActionSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the maximum number of octets of each packet
*           that will be saved in the capture buffer.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         maximum number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This object may not be modified if the associated
*           bufferControlStatus object is equal to valid(1).
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlCaptureSliceSizeSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the maximum number of octets of each packet in capture 
*           buffer that will be returned in an SNMP retrieval of that packet.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         maximum number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If the download slice size is set too large,
*           interoperability or fragmentation problems may occur.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlDownloadSliceSizeSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the offset of the first octet of each packet in capture 
*           buffer that will be returned in an SNMP retrieval of that packet.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         offset of the first octet
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlDownloadOffsetSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the requested maximum number of octets to be saved in
*           the captureBuffer, including any implementation-specific overhead. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         requested maximum number of octets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this variable is set to -1, capture buffer will save as many
*           octets as is possible.
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlMaxOctetsRequestedSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );

/*********************************************************************
* @purpose  Set the entity that configured the entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entry entity
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the buffer control entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbBufferControlStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* captureBuffer */
/*********************************************************************
* @purpose  Get a packet entry captured off of a channel.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    ControlIndex    buffer control index    
* @param    Index           interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a packet captured off of an attached network.
*
* @end
*********************************************************************/
L7_RC_t usmDbCaptureBufferEntryGet ( L7_uint32 UnitIndex, L7_int32 ControlIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next packet entry captured off of a channel.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    *ControlIndex   buffer control index    
* @param    *Index          interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a packet captured off of an attached network.
*
* @end
*********************************************************************/
L7_RC_t usmDbCaptureBufferEntryNext ( L7_uint32 UnitIndex, L7_int32 *ControlIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the index that describes a particular interface's order of packets.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    ControlIndex    buffer control index    
* @param    Index           interface
* @param    *val            order of packets index
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbCaptureBufferPacketIDGet ( L7_uint32 UnitIndex, L7_int32 ControlIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the data inside the capture buffer packet.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    ControlIndex    buffer control index    
* @param    Index           interface
* @param    *buf            buffer packet data
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this is less than zero, the object shall have a zero length.
*
* @end
*********************************************************************/
L7_RC_t usmDbCaptureBufferPacketDataGet ( L7_uint32 UnitIndex, L7_int32 ControlIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the actual packet length stored in the entry, including FCS octets.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    ControlIndex    buffer control index    
* @param    Index           interface
* @param    *val            actual packet length
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbCaptureBufferPacketLengthGet ( L7_uint32 UnitIndex, L7_int32 ControlIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of milliseconds since the capture buffer 
*           was first turned on when the packet was captured.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    ControlIndex    buffer control index    
* @param    Index           interface
* @param    *val            time in milliseconds
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbCaptureBufferPacketTimeGet ( L7_uint32 UnitIndex, L7_int32 ControlIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the value which indicates the packet's error status.
*
* @param    UnitIndex       L7_uint32 the unit for this operation
* @param    ControlIndex    buffer control index    
* @param    Index           interface
* @param    *val            status value
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbCaptureBufferPacketStatusGet ( L7_uint32 UnitIndex, L7_int32 ControlIndex, L7_int32 Index, L7_int32 *val );


/* event */
/*********************************************************************
* @purpose  Get an event entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters that describe an event to be
*           generated when certain conditions are met.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventEntryGet ( L7_uint32 UnitIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next event entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of parameters that describe an event to be
*           generated when certain conditions are met.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventEntryNext ( L7_uint32 UnitIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the description of the event entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entry description
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventDescriptionGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the type of notification that the probe will make about the event.   
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        notification type
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventTypeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Get the octet string specific to a community (for a sent trap).
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        octet string
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventCommunityGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the value of sysUpTime when the event entry last generated an event. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        sysUpTime 
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    If this entry has not generated any events, this value will be zero.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventLastTimeSentGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the entity that configured this entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entry entity
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventOwnerGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Get the event entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *val        entry status
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val );

/*********************************************************************
* @purpose  Set the description of the event entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entry description
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventDescriptionSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the type of notification that the probe will make about the event.   
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         notification type
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventTypeSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 val );

/*********************************************************************
* @purpose  Set the octet string specific to a community (for a sent trap).
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        octet string
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    This object is set to zero length if the mechanism is 
*           to be used to specify destination of the trap.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventCommunitySet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the entity that configured this entry and is using resources assigned to it.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    *buf        entry entity
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventOwnerSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_char8 *buf );

/*********************************************************************
* @purpose  Set the event entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Index       interface
* @param    val         entry status
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbEventStatusSet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 val );


/* log */
/*********************************************************************
* @purpose  Get the entry of a logged event.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    EventIndex  log event index
* @param    Index       interface
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of data describing a logged event.
*
* @end
*********************************************************************/
L7_RC_t usmDbLogEntryGet ( L7_uint32 UnitIndex, L7_int32 EventIndex, L7_int32 Index );

/*********************************************************************
* @purpose  Get the next entry of a logged event.
*
* @param    UnitIndex    L7_uint32 the unit for this operation
* @param    *EventIndex  log event index
* @param    *Index       interface
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    Entry is a set of data describing a logged event.
*
* @end
*********************************************************************/
L7_RC_t usmDbLogEntryNext ( L7_uint32 UnitIndex, L7_int32 EventIndex, L7_int32 *Index );

/*********************************************************************
* @purpose  Get the value of sysUpTime when the log entry was created.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    EventIndex  log event index
* @param    Index       interface
* @param    *val        sysUpTime
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbLogTimeGet ( L7_uint32 UnitIndex, L7_int32 EventIndex, L7_int32 Index, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the description of the event that activated the log entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    EventIndex  log event index
* @param    Index       interface
* @param    *buf        event description
*                                         
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*           
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbLogDescriptionGet ( L7_uint32 UnitIndex, L7_int32 EventIndex, L7_int32 Index, L7_char8 *buf );

#endif
