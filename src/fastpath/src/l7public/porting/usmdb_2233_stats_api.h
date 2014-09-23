/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_2233_stats.h
*
* @purpose externs for USMDB layer
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


#ifndef USMDB_2233_STATS_API_H
#define USMDB_2233_STATS_API_H

/*********************************************************************
*
* @purpose  The total number of octets received on the interface 
* @purpose  including framing characters. 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns	     		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfInOctetsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  The number of packets, delivered by this sub-layer to a 
* @purpose  higher sub-layer 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns			        found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns      		    values are invalid
*
* @notes    More specifically which were not addressed to a multicast or 
* @notes    broadcast address at this sub-layer. 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfInUcastPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  The number of packets delivered by this sub-layer to a 
* @purpose  higher sub-layer
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    More specifically which were addressed to a multicast or 
* @notes    broadcast address at this sub-layer
*
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfInNUcastPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  The number of inbound packets which were chosen to be 
* @purpose  discarded. 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns      		    values are invalid
*
* @notes    More specifically even though no errors had 
* @notes    been detected to prevent their being deliverable 			   
* @notes    to a higher-layer protocol.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfInDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);
extern L7_RC_t usmDbSnmpIfInDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Returns the current value of the counter IfInErrors
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns  		                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns      		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfInErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  the number of packets received via the interface which were 
* @purpose  discarded because of an unknown or unsupported protocol
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
*			    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfInUnknownProtosGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  The total number of octets transmitted out of the interface,
* @purpose  including framing characters
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns	    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOutOctetsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);


/*********************************************************************
*
* @purpose  The total number of packets that higher-level protocols 
* @purpose  requested be transmitted 
*			
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns	    		    values are invalid
*
* @notes    And more specifically which were not addressed to a multicast or 
* @notes	broadcast address at this sub-layer, including
* @notes	those that were discarded or not sent
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOutUcastPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  The total number of packets that higher-level protocols 
* @purpose  requested be transmitted 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns      		    values are invalid
*
* @notes    And more specifically which were not addressed to a multicast or 
* @notes	broadcast address at this sub-layer, including
* @notes	those that were discarded or not sent
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOutNUcastPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);


/*********************************************************************
*
* @purpose  The number of outbound packets which were chosen to be 
* @purpose  discarded 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns	    		    values are invalid
*
* @notes    More specifically even though no errors had 
* @notes    been detected to prevent their being transmitted
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOutDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);


/*********************************************************************
*
* @purpose  For packet-oriented interfaces, the number of outbound 
* @purpose  packets that could not be transmitted because of errors 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns	    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOutErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  ifOutQLen Get routine
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE, other failure
*
* @notes    MIB object is deprecated
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOutQLenGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);


/*********************************************************************
*
* @purpose  ifSpecific Get routine
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE, other failure
*
* @notes    MIB object is deprecated
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSpecificGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *buf, L7_uint32 *buf_len);


/*********************************************************************
* @purpose Returns the number of inbound packets which were 
* @purpose chosen to be discarded. 
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   Even though no errors had been detected to prevent 
* @notes   their being deliverable to a higher-layer protocol
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgtIfInDiscardsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of packets that contained errors.
*
* @param    UnitIndex   Unit
* @param    *val        returned value
*
* @returns L7_RC_t  L7_ERROR
*
* @notes    " For packet-oriented interfaces, the number of inbound
*             packets that contained errors preventing them from 
*             being deliverable to a higher-layer protocol.  For
*             character-oriented or fixed-length interfaces that
*             support protocol multiplexing the number of transmission
*             units received via the interface which were discarded
*             because of an unknown or unsupported protocol.  For any
*             interface that does not support protocol multiplexing
*             this counter will always be 0. 
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbMgtIfInErrorsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of octets received on the interface
*
* @param    UnitIndex   Unit
* @param    *val     
*
* @returns  L7_FAILURE  
*
* @notes    " The total number of octets received on the interface,
*             including framing characters. 
*
*             Discontinuities in the value of this counter can 
*             occur at re-initialization of the management system,
*             and at other times as indicated by the value of
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbMgtIfInOctetsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Returns the number of packets, delivered by this 
* @purpose sub-layer to a higher sub-layer. 
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   These include packets which were not addressed to a 
* @notes   multicast or broadcast address at this sub-layer.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgtIfInUcastPktsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Returns the number of outbound packets 
*          
* @param   UnitIndex  Unit
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   Specifically those which were chosen to be discarded 
* @notes   even though no errors had been detected to prevent 
* @notes   their being transmitted
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgtIfOutDiscardsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Returns the number of packets that could not be transmitted
*           because of errors.
*
* @param    UnitIndex   Unit
* @param    *val     
*
* @returns  L7_ERROR  
*
* @notes    " For packet-oriented interfaces, the number of outbound
*             packets that could not be transmitted because of errors.
*             For character-oriented or fixed-length interfaces, 
*             the number of outbound tramission units that could not
*             be transmitted because of errors.
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of 
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbMgtIfOutErrorsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Returns the total number of octets transmitted out of 
* @purpose the interface, including framing characters.
*		
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgtIfOutOctetsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Returns the total number of packets that 
* @purpose higher-level protocols requested be transmitted. 
*		   
*          
* @param   L7_uint32 UnitIndex
* @param   L7_uint32 interface
* @param   L7_uint32 *val
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   And which were not addressed to a multicast or 
* @notes   broadcast address at this sub-layer, including
* @notes   those that were discarded or not sent
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgtIfOutUcastPktsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Returns the number of packets, delivered by this 
* @purpose sub-layer to a higher sub-layer
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   which were addressed to a broadcast address at this sub-layer
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgtTxBcastPktsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Returns the number of packets, delivered by this 
* @purpose sub-layer to a higher sub-layer
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   which were addressed to a broadcast address at this sub-layer
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgtTxMcastPktsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets the number of valid packets received
*
* @param    UnitIndex   Unit
* @param    *val     
*
* @returns  L7_ERROR  
*
* @notes    none 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbMgtRxGoodPktsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets the value of the high capacity counter associated
*           with ifHCInOctets.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *high
* @param    *low
*
* @returns  L7_SUCCESS      If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR        If the counter requested does not exisit 
* @returns  L7_FAILURE      Catastrophic failure i.e. all the returned 
* @returns					values are invalid 
*
* @notes    " The total number of octets received on the interface
*             including framing characters.  This object is a 64-bit
*             version of ifInOctets.
*       
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCInOctetsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
*
* @purpose  Gets the value of the high capacity counter associated
*           with ifHCInUcastPkts.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *high
* @param    *low
*
* @returns  L7_SUCCESS      If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR        If the counter requested does not exisit 
* @returns  L7_FAILURE      Catastrophic failure i.e. all the returned 
* @returns					values are invalid 
*
* @notes    " The number of packets, delivered by this sub-layer to
*             a higher (sub-)layer, which were not addressed to a 
*             multicast or broadcast address at this sub-layer.
*             This object is a 64 bit version of ifInUcastPkts.
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of 
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCInUcastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
*
* @purpose  Gets the value of the high capacity counter associated
*           with ifHCInMulticastPkts.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *high
* @param    *low
*
* @returns  L7_SUCCESS      If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR        If the counter requested does not exisit 
* @returns  L7_FAILURE      Catastrophic failure i.e. all the returned 
* @returns					values are invalid 
*
* @notes    " The number of packets, delivered by this sub-layer to
*             a higher (sub-)layer, which were addressed to a multicast
*             address at this sub-layer.  For a MAC layer protocol,
*             this includes both Group and Functional addresses.  
*             This object is a 64 bit version of ifInMulticastPkts.
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCInMulticastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *high, L7_uint32 *low);


/*********************************************************************
*
* @purpose  The total number of packets that higher-level protocols
* @purpose  requested be transmitted
*
* @param    UnitIndex   Unit
* @param    interface   interface number
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully
* @returns                  found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned
* @returns                  values are invalid
*
* @notes    And more specifically which were not addressed to a multicast or
* @notes    broadcast address at this sub-layer, including
* @notes    those that were discarded or not sent
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCOutNUcastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_ulong64 *val);


/*********************************************************************
*
* @purpose  The number of packets delivered by this sub-layer to a
* @purpose  higher sub-layer
*
* @param    UnitIndex   Unit
* @param    interface   interface number
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully
* @returns                  found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned
*                           values are invalid
*
* @notes    More specifically which were addressed to a multicast or
* @notes    broadcast address at this sub-layer
*
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCInNUcastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_ulong64 *val);


/*********************************************************************
*
* @purpose  Gets the value of the high capacity counter associated
*           with ifHCInBroadcastPkts.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *high
* @param    *low
*
* @returns  L7_SUCCESS      If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR        If the counter requested does not exisit 
* @returns  L7_FAILURE      Catastrophic failure i.e. all the returned 
* @returns					values are invalid 
*
* @notes    " The number of packets, delivered by this sub-layer to
*             a higher (sub-)layer, which were addressed to a 
*             broadcast address at this sub-layer.  This object is a 
*             64-bit version of ifInBroadcastPkts.
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCInBroadcastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
*
* @purpose  Gets the value of the high capacity counter associated
*           with ifHCOutOctets.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *high
* @param    *low
*
* @returns  L7_SUCCESS      If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR        If the counter requested does not exisit 
* @returns  L7_FAILURE      Catastrophic failure i.e. all the returned 
* @returns					values are invalid 
*
* @notes    " The total number of octets transmitted out of the 
*             interface, including framing characters.  This object
*             is the 64-bit version of ifOutOctets.
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCOutOctetsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
*
* @purpose  Gets the value of the high capacity counter associated
*           with ifHCOutUcastPkts.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *high
* @param    *low
*
* @returns  L7_SUCCESS      If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR        If the counter requested does not exisit 
* @returns  L7_FAILURE      Catastrophic failure i.e. all the returned 
* @returns					values are invalid 
*
* @notes    " The total number of packets that higher-level protocols
*             requested be transmitted, and which were not addressed
*             to multicast or broadcast address at this sub-layer,
*             including those that were discarded or not sent.  This
*             object is a 64-bit version of ifOutUcastPkts.
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCOutUcastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
*
* @purpose  Gets the value of the high capacity counter associated
*           with ifHCOutMulticastPkts.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *high
* @param    *low
*
* @returns  L7_SUCCESS      If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR        If the counter requested does not exisit 
* @returns  L7_FAILURE      Catastrophic failure i.e. all the returned 
* @returns					values are invalid 
*
* @notes    " The total number of packets that higher-level protocols
*             requested be transmitted, and which were addressed
*             to a multicast address at this sub-layer, including
*             those that were discarded or not sent.  For a MAC
*             layer protocol, this includes both Group and Functional
*             addresses.  This object is a 64-bit version of 
*             ifOutMulticastPkts.
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCOutMulticastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
*
* @purpose  Gets the value of the high capacity counter associated
*           with ifHCOutBroadcastPkts.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *high
* @param    *low
*
* @returns  L7_SUCCESS      If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR        If the counter requested does not exisit 
* @returns  L7_FAILURE      Catastrophic failure i.e. all the returned 
* @returns					values are invalid 
*
* @notes    " The total number of packets that higher-level protocols
*             requested be transmitted, and which were addressed
*             to a broadcast address at this sub-layer, including
*             those that were discarded or not sent.  This object
*             is a 64-bit version of ifOutBroadcastPkts.
*
*             Discontinuities in the value of this counter can occur
*             at re-initialization of the management system, and at
*             other times as indicated by the value of
*             ifCounterDiscontinuityTime."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHCOutBroadcastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
*
* @purpose  Multicast received 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns	    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfInMulticastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Broadcast received 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns	    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfInBroadcastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Multicast out 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns                  found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns      		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOutMulticastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Broadcast sent 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns	    		    values are invalid
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOutBroadcastPktsGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *va );

/*********************************************************************
*
* @purpose  Gets either the system name or alias name of the specified 
* @purpose  interface as requested
*
* @param    UnitIndex   Unit
* @param    intIfNum    Internal Interface Number 
* @param    nameType    name type requested (L7_SYSNAME, L7_ALIASNAME or L7_NULL) 
* @param                L7_NULL will return currently configured ifName
* @param    ifName      pointer to Interface Name,
* @param                (@b{Returns: MAX_INTF_NAME byte interface name})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfNameGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uchar8 *buf);

/*********************************************************************
*
* @purpose  Gets the system long form name interface as requested
*
* @param    UnitIndex   Unit
* @param    intIfNum    Internal Interface Number
* @param    nameType    name type requested (L7_SYSNAME, L7_ALIASNAME or L7_NULL)
* @param                L7_NULL will return currently configured ifName
* @param    ifName      pointer to Interface Name,
* @param                (@b{Returns: MAX_INTF_NAME byte interface name})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfNameLongGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uchar8 *buf);

/*********************************************************************
*
* @purpose  Returns the time the last statistics were reset
*
* @param    UnitIndex   Unit
* @param    interface     
* @param    *ts     
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    
* @returns  L7_FAILURE  
*
* @notes    time is in hundredths of a second
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfCounterDiscontinuityTimeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets the current invocation of the interfaces's test
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value
*
* @returns  L7_FAILURE 
*
* @notes    " This object identifies the current invocation of the
*             interface's test."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestIdGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets the current owner of the tests on the interface.
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value
*
* @returns  L7_FAILURE 
*
* @notes    " This object indicates whether or not some manager
*             currently has the necessary 'ownership' required
*             to invoke a test on this interface.  A write to this
*             object is only successful when it changes its value
*             form 'notInUse(1)' to 'inUse(2)'.  After completion
*             of a test, the agent resets the value back to
*             'notInUse(1)'."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestStatusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets a control variable used to start and stop operator-
*           initiated interface tests.
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *buf
* @param    *buf_len        
*
* @returns  L7_FAILURE 
*
* @notes    " A control variable used to start and stop operator
*             initiated interface tests.  Most OBJECT IDENTIFIER
*             values assigned to tests are defined elsewhere, in
*             association with specific types of interfaces.
*             However, this document assigns a value for full-duplex
*             loopback test, and defines the special meanings of the
*             subject identifier:
*                 
*               noTest  OBJECT IDENTIFIER ::= {0,0}
*
*             When the value noTest is written to this object, no
*             action is taken unless a test is in progress, in which
*             case the test is aborted.  Writing any other value to
*             this object is only valid when no test is currently
*             in progress, in which case the indicated test is
*             initiated.
*
*             When read, this object always returns the most recent
*             value that ifTestType was set to.  If it has not been
*             set since the last initialization of the network
*             management subsystem on the agent, a value of noTest
*             is returned."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestTypeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *buf, L7_uint32 *buf_len);

/*********************************************************************
*
* @purpose  Returns the test result.
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value
*
* @returns  L7_FAILURE 
*
* @notes    Types of test results:
*             none(1)
*             success(2)
*             inProgress(3)
*             notSupported(4)
*             unAbleToRun(5)
*             aborted(6)
*             failed(7)
*
*           " This object contains the result of the most recently
*             requested test, or the value none(1) if no tests have
*             been requested since the last reset.  Note that this
*             facility provides no provision for saving the results
*             of one test when starting another, as could be required
*             if used by multiple managers concurrently."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestResultGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Returns a code that contains more specific information
*           about the test results.
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *buf        returned value
* @param    *buf_len    returned buffer length
*
* @returns  L7_FAILURE 
*
* @notes    " This object contains a code which contains more specific
*             information on the test result, for example an error-code
*             after a failed test.  Error codes and other values this
*             object may take are specific to the type of interface
*             and/ or test.  The value may have the semantics of
*             either the AutonomousType or InstancePointer textual
*             conventions as defined in RFC 1903. The identifier:
*
*                 testCodeUnknown OBJECT IDENTIFIER ::= {0 0}
*
*             is defined for use if no additional result code is
*             available."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestCodeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *buf, L7_uint32 *buf_len);

/*********************************************************************
*
* @purpose  Returns the entity with privileges to invoke a test on 
*           the interface.
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *buf
* @param    *buf_len
*
* @returns  L7_FAILURE 
*
* @notes    " The entity which currently has the 'ownership' required
*             to invoke a test on this interface."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestOwnerGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uchar8 *buf);

/*********************************************************************
*
* @purpose  Sets the current invocation of the interfaces's test
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value
*
* @returns  L7_FAILURE 
*
* @notes    " This object identifies the current invocation of the
*             interface's test."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestIdSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 val);

/*********************************************************************
*
* @purpose  Sets the current owner of the tests on the interface.
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value
*
* @returns  L7_FAILURE 
*
* @notes    " This object indicates whether or not some manager
*             currently has the necessary 'ownership' required
*             to invoke a test on this interface.  A write to this
*             object is only successful when it changes its value
*             form 'notInUse(1)' to 'inUse(2)'.  After completion
*             of a test, the agent resets the value back to
*             'notInUse(1)'."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestStatusSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 val);

/*********************************************************************
*
* @purpose  Sets a control variable used to start and stop operator-
*           initiated interface tests.
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *buf
* @param    *buf_len        
*
* @returns  L7_FAILURE 
*
* @notes    " A control variable used to start and stop operator
*             initiated interface tests.  Most OBJECT IDENTIFIER
*             values assigned to tests are defined elsewhere, in
*             association with specific types of interfaces.
*             However, this document assigns a value for full-duplex
*             loopback test, and defines the special meanings of the
*             subject identifier:
*                 
*               noTest  OBJECT IDENTIFIER ::= {0,0}
*
*             When the value noTest is written to this object, no
*             action is taken unless a test is in progress, in which
*             case the test is aborted.  Writing any other value to
*             this object is only valid when no test is currently
*             in progress, in which case the indicated test is
*             initiated.
*
*             When read, this object always returns the most recent
*             value that ifTestType was set to.  If it has not been
*             set since the last initialization of the network
*             management subsystem on the agent, a value of noTest
*             is returned."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestTypeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *buf, L7_uint32 buf_len);

/*********************************************************************
*
* @purpose  Sets the entity with privileges to invoke a test on 
*           the interface.
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *buf
* @param    *buf_len
*
* @returns  L7_FAILURE 
*
* @notes    " The entity which currently has the 'ownership' required
*             to invoke a test on this interface."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTestOwnerSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uchar8 *buf);

/*********************************************************************
*
* @purpose  Gets an ifStackTable entry
*
* @param    UnitIndex   Unit
* @param    HigherLayer       
* @param    LowerLayer
*
* @returns  L7_FAILURE 
*
* @notes    " Information on a particular relationship between two
*             sub-layers, specifying that one sub-layer runs on
*             'top' of the other sub-layer.  Each sub-layer corresponds
*             to a conceptual row in the ifTable."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfStackEntryGet(L7_uint32 UnitIndex, L7_uint32 HigherLayer, L7_uint32 LowerLayer);

/*********************************************************************
*
* @purpose  Gets the next ifStackTable entry
*
* @param    UnitIndex   Unit
* @param    HigherLayer       
* @param    LowerLayer
*
* @returns  L7_FAILURE 
*
* @notes    " Information on a particular relationship between two
*             sub-layers, specifying that one sub-layer runs on
*             'top' of the other sub-layer.  Each sub-layer corresponds
*             to a conceptual row in the ifTable."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfStackEntryNext(L7_uint32 UnitIndex, L7_uint32 *HigherLayer, L7_uint32 *LowerLayer);

/*********************************************************************
*
* @purpose  Gets the status of the relationship between two
*           sub-layers.
*
* @param    UnitIndex   Unit
* @param    HigherLayer       
* @param    LowerLayer
* @param    *val
*
* @returns  L7_FAILURE 
*
* @notes    " The status of the relationship between two sub-layers
*       
*             Changing the value of this object from 'active' to
*             'notInService' or 'destroy' will likely have
*             consequences up and down the interface stack.  Thus,
*             write access to this object is likely to be inappropriate
*             for some types of interfaces, and many implementations
*             will choose not to support write-access for any type
*             of interface."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfStackStatusGet(L7_uint32 UnitIndex, L7_uint32 HigherLayer, L7_uint32 LowerLayer, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Sets the status of the relationship between two
*           sub-layers.
*
* @param    UnitIndex   Unit
* @param    HigherLayer       
* @param    LowerLayer
* @param    *val
*
* @returns  L7_FAILURE 
*
* @notes    " The status of the relationship between two sub-layers
*       
*             Changing the value of this object from 'active' to
*             'notInService' or 'destroy' will likely have
*             consequences up and down the interface stack.  Thus,
*             write access to this object is likely to be inappropriate
*             for some types of interfaces, and many implementations
*             will choose not to support write-access for any type
*             of interface."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfStackStatusSet(L7_uint32 UnitIndex, L7_uint32 HigherLayer, L7_uint32 LowerLayer, L7_uint32 val);

/*********************************************************************
*
* @purpose  Gets an entry in the ifRcvAddressTable
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *Address
*
* @returns  L7_FAILURE 
*
* @notes    " A list of objects identifying an address for which
*             the system will accept packets/ frames on the particular
*             interface identified by the index value ifIndex."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfRcvAddressEntryGet(L7_uint32 UnitIndex, L7_int32 Index, L7_uchar8 *Address);

/*********************************************************************
*
* @purpose  Gets the next entry in the ifRcvAddressTable
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *Address
*
* @returns  L7_FAILURE 
*
* @notes    " A list of objects identifying an address for which
*             the system will accept packets/ frames on the particular
*             interface identified by the index value ifIndex."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfRcvAddressEntryNext(L7_uint32 UnitIndex, L7_uint32 *Index, L7_uchar8 *Address);

/*********************************************************************
*
* @purpose  Gets the status of an entry in the ifRcvAddressTable
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *Address
* @param    *val
*
* @returns  L7_FAILURE 
*
* @notes    " This object is used to create and delete rows in the
*             ifRcvAddressTable."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfRcvAddressStatusGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uchar8 *Address, L7_int32 *val);

/*********************************************************************
*
* @purpose  Returns the type of the entry in the ifRcvAddressTable.
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *Address
* @param    *val
*
* @returns  L7_FAILURE 
*
* @notes    Types of entries:
*             other(1)
*             volatile(2)
*             non-volatile(3)
*
*           " This object has the value nonVolatile(3) for those
*             entries in the table which are valid and will not
*             be deleted by the next restart of the managed system.
*             Entries having the value volatile(2) are valid and
*             exist, but have not been saved, so that will not
*             exist after the next restart of the managed system.
*             Entries having value other(1) are valid and exist but
*             are not classified as to whether they will continue
*             to exist after the next restart."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfRcvAddressTypeGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uchar8 *Address, L7_int32 *val);

/*********************************************************************
*
* @purpose  Sets the status of an entry in the ifRcvAddressTable
*
* @param    UnitIndex   Unit
* @param    Index       
* @param    *Address
* @param    *val
*
* @returns  L7_FAILURE 
*
* @notes    " This object is used to create and delete rows in the
*             ifRcvAddressTable."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfRcvAddressStatusSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uchar8 *Address, L7_int32 val);

#endif
