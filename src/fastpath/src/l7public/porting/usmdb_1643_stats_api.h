/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_1643_stats_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments FOR RFC1643 ETHERNET-LIKE MIB
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_1643_STATS_API_H
#define USMDB_1643_STATS_API_H
     
/*********************************************************************
* @purpose Returns the total number of packets received that 
* @purpose were directed to the broadcast address. 
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   Note that this does not include multicast packets
*
* @end
*********************************************************************/
extern L7_RC_t usmDbMgtEtherStatsBroadcastPktsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose Returns the total number of good packets received that 
* @purpose were directed to a multicast address. 
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   Note that this number does not include packets 
* @notes   directed to the broadcast address
*
* @end
*********************************************************************/
extern L7_RC_t usmDbMgtEtherStatsMulticastPktsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose Returns The total number of octets of data received.
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes   includes bad packets, broadcast packets, and 
* @notes   multicast packets 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbMgtEtherStatsPktsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose Returns the number of good packets received
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes    
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t usmDbRxGoodPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose Returns the number of multicast packets transmitted
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
*********************************************************************/
extern L7_RC_t usmDbTxMcastPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose Returns the number of transmitted packets to the cpu
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
*********************************************************************/
extern L7_RC_t usmDbTxPktsToCpuGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose Returns the total number of outbound packets that were discarded
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
*********************************************************************/
extern L7_RC_t usmDbTxTotalDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose Returns the total number of errored frames transmitted
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
*********************************************************************/
extern L7_RC_t usmDbTxTotalErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of frames received on a particular interface
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
* @notes    More specifically that are not an integral number of octets in 
* @notes    length and do not pass the FCS check.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsAlignmentErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of frames for which transmission on an interface 
*		    
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns       		    values are invalid
*
* @notes    More specifically the interface fails due to excessive collisions.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsExcessiveCollisionsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of frames received on a particular interface 
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
* @notes    More specifically that are an integral number of octets 
* @notes    in length but do not pass the FCS check.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsFCSErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of successfully transmitted frames on an interface 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns 	    		    values are invalid
*
* @notes    More specifically a particular interface for which transmission 
* @notes    is inhibited by more than one collision.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsMultipleCollisionFramesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of successfully transmitted frames on an interface 
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
* @notes    More specifically for which transmission is inhibited by 
* @notes    exactly one collision.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsSingleCollisionFramesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of times that the SQE TEST ERROR message is generated.
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
* @notes    More specifically by the PLS sublayer for a particular interface. 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsSQETestErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of frames for which the first transmission attempt is delayed 
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
* @notes    More specifically the particular interface is delayed because the 
* @notes    medium is busy
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsDeferredTransmissionsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of times that a collision is detected on an interface 
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
* @notes    More specifically a particular interface later than 512 bit-times into 
* @notes	the transmission of a packet.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsLateCollisionsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of frames for which the first transmission attempt is delayed 
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
* @notes    More specifically on a particular interface that is delayed 
* @notes    because the medium is busy
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsInternalMacTransmitErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of times that the carrier sense condition was lost 
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
* @notes    Or more specifically never asserted when attempting to
* @notes	transmit a frame on a particular interface.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsCarrierSenseErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of frames that exceeded the max permitted frame size
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
extern L7_RC_t usmDbDot3StatsFrameTooLongsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of frames for which transmission on an interface fails
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
*
* @notes    More specifically the particular interface fails due to an 
* @notes    internal MAC sublayer transmit error.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsInternalMacReceiveErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the chipset on which this interface is built.  
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *id         a ptr to the identifier of the chip manufacturer
*
* @returns  L7_SUCCESS      
* @returns  L7_ERROR     
* @returns  L7_FAILURE   
*
* @notes    Not supported at this time.   
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3StatsEtherChipSetGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *id);

/* TBD */

/*********************************************************************
* @purpose   A count of MAC Control frames received on this interface
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
* @notes    More specifically with an opcode indicating the PAUSE 
* @notes    operation.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3InPauseFramesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  A count of MAC Control frames transmitted on this interface 
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value of counter
*
* @returns  L7_SUCCESS  :   If the counter was successfully 
* @returns	                found and stored in the space provided
* @returns  L7_ERROR    :   If the counter requested does not exisit 
* @returns  L7_FAILURE  :   Catastrophic failure i.e. all the returned 
* @returns         		    values are invalid
*
* @notes    More specifically with an opcode indicating the PAUSE operation.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3OutPauseFramesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose Returns the number of broadcast packets transmitted
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
*********************************************************************/
extern L7_RC_t usmDbTxBcastPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/**************************************************************************
* @purpose  Returns the statistics for a particular interface to an
*           ethernet medium.
*
* @param    UnitIndex   Unit  
* @param    Index 
*
* @returns  L7_FAILURE
*
* @notes    " Statistics for a particular interface to an ethernet-like
*             medium. "
*
* @end
*************************************************************************/
extern L7_RC_t usmDbDot3StatsEntryGet(L7_uint32 UnitIndex, L7_int32 Index); 

/**************************************************************************
* @purpose  Returns the statistics for the next interface to an
*           ethernet medium.
*
* @param    UnitIndex   Unit  
* @param    Index 
*
* @returns  L7_FAILURE
*
* @notes    " Statistics for a particular interface to an ethernet-like
*             medium. "
*
* @end
*************************************************************************/
extern L7_RC_t usmDbDot3StatsEntryNext(L7_uint32 UnitIndex, L7_uint32 *Index);

/**************************************************************************
* @purpose  Returns an individual entry.
*
* @param    UnitIndex   Unit
* @param    interface   interface number
* @param    Count
*
* @returns  L7_FAILURE
*
* @notes    " A cell in the histogram of per-frame collisions for a
*             particular interface.  An instance of this object
*             represents the frequency of individual MAC frames for
*             which the transmission (successful or otherwise) on a
*             particular interface is accompanied by a particular
*             number of media collisions."
*
* @end
*************************************************************************/
extern L7_RC_t usmDbDot3CollEntryGet(L7_uint32 UnitIndex,L7_uint32 interface, L7_int32 Count );

/**************************************************************************
* @purpose  Returns the next individual entry.
*
* @param    UnitIndex   Unit
* @param    interface   interface number
* @param    Count
*
* @returns  L7_FAILURE
*
* @notes    " A cell in the histogram of per-frame collisions for a
*             particular interface.  An instance of this object
*             represents the frequency of individual MAC frames for
*             which the transmission (successful or otherwise) on a
*             particular interface is accompanied by a particular
*             number of media collisions."
*
* @end
*************************************************************************/
extern L7_RC_t usmDbDot3CollEntryNext ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *Count );

/*********************************************************************
* @purpose  A count of individual MAC frames. 
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
*
* @notes    Specifically those for which the transmission 
* @notes    (successful or otherwise) on a particular interface 
* @notes    occurs after the frame has experienced exactly the 
* @notes    number of collisions in the associated
* @notes    dot3CollCount object.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot3CollFrequenciesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 Count, L7_uint32 *val );

#endif
