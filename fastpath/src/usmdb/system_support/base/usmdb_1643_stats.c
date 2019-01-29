/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_1643.c
*
* @purpose Provide interface to hardware API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 11/03/2000
*
* @author gaunce
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <l7_common.h>
#include <stdio.h>


#include <stdlib.h>
#include <string.h>
#include <osapi.h>
#include <statsapi.h>
#include <usmdb_counters_api.h>
#include "usmdb_1643_stats_api.h"
#include "usmdb_common.h"

/*********************************************************************
*
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
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMgtEtherStatsBroadcastPktsGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbMgtEtherStatsMulticastPktsGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose Returns The total number of octets of data received.
*   	    
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
*
*********************************************************************/
L7_RC_t usmDbMgtEtherStatsPktsGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose Returns the number of good packets received on an interface
*   	    
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
*
*********************************************************************/
L7_RC_t usmDbRxGoodPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbTxMcastPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbTxTotalDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbTxTotalErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbTxBcastPktsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_ERROR;
}

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
L7_RC_t usmDbDot3StatsEntryGet(L7_uint32 UnitIndex, L7_int32 Index)
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot3StatsEntryNext(L7_uint32 UnitIndex, L7_uint32 *Index)
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot3CollEntryGet(L7_uint32 UnitIndex,L7_uint32 interface, L7_int32 Count )
{
  return L7_FAILURE;
}

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
L7_RC_t usmDbDot3CollEntryNext ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *Count )
{
  return L7_FAILURE;
}

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
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot3CollFrequenciesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 Count, L7_uint32 *val )
{
  /* value not supported in hardware */
  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  A count of times that the SQE TEST ERROR message is generated.
* 
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
L7_RC_t usmDbDot3StatsSQETestErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  /* value not supported in hardware */
  *val = 0;
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot3StatsDeferredTransmissionsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  /* value not supported in hardware */
  *val = 0;
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot3StatsLateCollisionsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  /* not supported in hardware */
  /*
  *val = 0;
  return L7_SUCCESS;
  */
  return usmDbStatGet(UnitIndex, L7_CTR_TX_LATE_COLLISION, interface, val);

}

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
L7_RC_t usmDbDot3StatsInternalMacTransmitErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  /* not supported in hardware */
  *val = 0;
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot3StatsCarrierSenseErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  /* not supported in hardware */
  *val = 0;
  return L7_SUCCESS;
}

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
L7_RC_t usmDbDot3StatsFrameTooLongsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_FRAME_TOO_LONG, interface, val);
}

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
L7_RC_t usmDbDot3StatsInternalMacReceiveErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  /* not supported in hardware */
  *val = 0;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose   A count of MAC Control frames received on this interface
*			   
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
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3InPauseFramesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose  A count of MAC Control frames transmitted on this interface 
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
* @returns         		    values are invalid
*
* @notes    More specifically with an opcode indicating the PAUSE operation.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3OutPauseFramesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose  A count of frames received on a particular interface
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
* @notes    More specifically that are not an integral number of octets in 
* @notes    length and do not pass the FCS check.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3StatsAlignmentErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_ALIGN_ERRORS, interface, val);
}

/*********************************************************************
*
* @purpose  A count of frames received on a particular interface 
*            
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
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3StatsFCSErrorsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_FCS_ERRORS, interface, val);
}

/*********************************************************************
*
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
L7_RC_t usmDbDot3StatsSingleCollisionFramesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return usmDbStatGet(UnitIndex, L7_CTR_TX_ONE_COLLISION, interface, val);
}

/*********************************************************************
*
* @purpose  A count of successfully transmitted frames on an interface 
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
* @returns 	    		    values are invalid
*
* @notes    More specifically a particular interface for which transmission 
* @notes    is inhibited by more than one collision.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3StatsMultipleCollisionFramesGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return usmDbStatGet(UnitIndex, L7_CTR_TX_MULTIPLE_COLLISION, interface, val);
}

/*********************************************************************
*
* @purpose  A count of frames for which transmission on an interface 
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
* @returns       		    values are invalid
*
* @notes    More specifically the interface fails due to excessive collisions.
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3StatsExcessiveCollisionsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  return usmDbStatGet(UnitIndex, L7_CTR_TX_EXCESSIVE_COLLISION, interface, val);
}


/*********************************************************************
* @purpose  Get the chipset on which this interface is built
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *id         a ptr to the identifier of the chip manufacturer
*
* @returns  L7_SUCCESS      
* @returns                  
* @returns  L7_ERROR     
* @returns  L7_FAILURE   
*
* @notes    Not supported at this time.   
*
* @end
*********************************************************************/
L7_RC_t usmDbDot3StatsEtherChipSetGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *id)
{
  *id = 0;
  return L7_FAILURE;
}

