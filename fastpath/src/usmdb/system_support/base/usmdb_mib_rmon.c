/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_mib_rmon.c
*
* @purpose Provide interface to hardware API's for RFC1271 RMON-MIB components
*
* @component unitmgr
*
* @comments tba
*
* @create 09/20/2000
*
* @author cpverne
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <l7_common.h>
#include <usmdb_mib_rmon_api.h>

#include <statsapi.h>
#include <usmdb_counters_api.h>
#include "usmdb_common.h"
#include "usmdb_mib_rmon_api.h"

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
L7_RC_t usmDbEtherStatsOctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_TOTAL_BYTES, Index, val);
}

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
L7_RC_t usmDbEtherStatsPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  L7_uint32 tmpVal;
  L7_RC_t rc;

  rc = usmDbStatGet(UnitIndex, L7_CTR_RX_TOTAL_FRAMES, Index, &tmpVal);
  if (rc == L7_SUCCESS)
  {
    *val = tmpVal;
    rc = usmDbStatGet(UnitIndex, L7_CTR_RX_TOTAL_ERROR_FRAMES, Index, &tmpVal);
    if (rc == L7_SUCCESS)
      *val += tmpVal;
  }

  return rc;
}

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
L7_RC_t usmDbEtherStatsBroadcastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_BCAST_FRAMES, Index, val);
}

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
L7_RC_t usmDbEtherStatsMulticastPktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_MCAST_FRAMES, Index, val);
}

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
L7_RC_t usmDbEtherStatsCRCAlignErrorsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_CRC_ERRORS, Index, val);
} 

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
L7_RC_t usmDbEtherStatsUndersizePktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_GOOD_UNDERSIZE, Index, val);
}

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
L7_RC_t usmDbEtherStatsOversizePktsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_GOOD_OVERSIZE, Index, val);
}

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
L7_RC_t usmDbEtherStatsFragmentsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_ERROR_UNDERSIZE, Index, val);
}

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
L7_RC_t usmDbEtherStatsJabbersGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_ERROR_OVERSIZE, Index, val);
}

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
L7_RC_t usmDbEtherStatsCollisionsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_TX_TOTAL_COLLISION_FRAMES, Index, val);
}

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
L7_RC_t usmDbEtherStatsPkts64OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  L7_RC_t rc;

  rc = usmDbStatGet(UnitIndex, L7_CTR_RX_64, Index, val);
  if (rc == L7_ERROR) 
  {
	  rc = usmDbStatGet(UnitIndex, L7_CTR_TX_RX_64, Index, val);
  }

  return rc;
}

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
L7_RC_t usmDbEtherStatsPkts65to127OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  L7_RC_t rc;

  rc = usmDbStatGet(UnitIndex, L7_CTR_RX_65_127, Index, val);
  if (rc == L7_ERROR) 
  {
	  rc = usmDbStatGet(UnitIndex, L7_CTR_TX_RX_65_127, Index, val);
  }

  return rc;
}

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
L7_RC_t usmDbEtherStatsPkts128to255OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  L7_RC_t rc;

  rc = usmDbStatGet(UnitIndex, L7_CTR_RX_128_255, Index, val);
  if (rc == L7_ERROR) 
  {
	  rc = usmDbStatGet(UnitIndex, L7_CTR_TX_RX_128_255, Index, val);
  }

  return rc;
}

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
L7_RC_t usmDbEtherStatsPkts256to511OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  L7_RC_t rc;

  rc = usmDbStatGet(UnitIndex, L7_CTR_RX_256_511, Index, val);
  if (rc == L7_ERROR) 
  {
	  rc = usmDbStatGet(UnitIndex, L7_CTR_TX_RX_256_511, Index, val);
  }

  return rc;
}

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
L7_RC_t usmDbEtherStatsPkts512to1023OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  L7_RC_t rc;

  rc = usmDbStatGet(UnitIndex, L7_CTR_RX_512_1023, Index, val);
  if (rc == L7_ERROR) 
  {
	  rc = usmDbStatGet(UnitIndex, L7_CTR_TX_RX_512_1023, Index, val);
  }

  return rc;
}

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
L7_RC_t usmDbEtherStatsPkts1024to1518OctetsGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_uint32 *val )
{
  L7_RC_t rc;

  rc = usmDbStatGet(UnitIndex, L7_CTR_RX_1024_1518, Index, val);
  if (rc == L7_ERROR) 
  {
	  rc = usmDbStatGet(UnitIndex, L7_CTR_TX_RX_1024_1518, Index, val);
  }

  return rc;
}
