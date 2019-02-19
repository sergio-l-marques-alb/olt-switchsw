/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_mib_bgp4_api.h
*
* @purpose usmdb support for LVL7 BGP4 MIB
*
* @component BGP4 (Layer 3)
*
* @comments
*
* @create 01/29/2002
*
* @author Mahe.korukonda
* @end
*
**********************************************************************/

#ifndef USMDB_MIB_BGP4_API_H
#define USMDB_MIB_BGP4_API_H
#include "usmdb_common.h"

L7_RC_t usmDbBgp4VersionGet(L7_uint32 UnitIndex, L7_ushort16* version);
L7_RC_t usmDbBgp4LocalAsGet(L7_uint32 UnitIndex, L7_ushort16* localAs);

L7_RC_t usmDbBgp4LocalIdGet(L7_uint32 UnitIndex, L7_uint32* localId);
L7_RC_t usmDbBgp4LocalIdSet(L7_uint32 UnitIndex, L7_uint32 localId);


L7_RC_t usmDbBgp4PeerAdminStatusGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* mode);
L7_RC_t usmDbBgp4PeerAdminStatusSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 mode);
L7_RC_t usmDbBgp4PeerIdSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32 peerId);
L7_RC_t usmDbBgp4PeerConnRetryIntervalGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* connRetryInt);
L7_RC_t usmDbBgp4PeerConnRetryIntervalSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 connRetryInt);
L7_RC_t usmDbBgp4PeerHoldTimeGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* holdTime);
L7_RC_t usmDbBgp4PeerKeepAliveGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* keepAlive);
L7_RC_t usmDbBgp4PeerLocalAddressGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32* localAddr);
L7_RC_t usmDbBgp4PeerLocalPortGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* localPort);
L7_RC_t usmDbBgp4PeerLocalPortSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 localPort);
L7_RC_t usmDbBgp4PeerRemotePortGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* remotePort);
L7_RC_t usmDbBgp4PeerRemotePortSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 remotePort);
L7_RC_t usmDbBgp4PeerIdGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32* peerId);
L7_RC_t usmDbBgp4PeerStateGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* state);
L7_RC_t usmDbBgp4PeerNegotiatedVersionGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ulong32* negotiatedVersion);
L7_RC_t usmDbBgp4PeerRemoteAddressGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr);
L7_RC_t usmDbBgp4PeerRemoteAddressGetNext(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32* remoteAddress);
L7_RC_t usmDbBgp4PeerRemoteAsGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* remoteAs);
L7_RC_t usmDbBgp4PeerInUpdatesGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ulong32* inUpdates);
L7_RC_t usmDbBgp4PeerOutUpdatesGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ulong32* outUpdates);
L7_RC_t usmDbBgp4PeerInTotalMessagesGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ulong32* inTotalMessages);
L7_RC_t usmDbBgp4PeerOutTotalMessagesGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ulong32* outTotalMessages);
L7_RC_t usmDbBgp4PeerLastErrorGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_char8* lastError);
L7_RC_t usmDbBgp4PeerFsmEstablishedTransitionsGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ulong32* fsmEstdTransitions);
L7_RC_t usmDbBgp4PeerFsmEstablishedTimeGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, usmDbTimeSpec_t* fsmEstdTime);
L7_RC_t usmDbBgp4PeerHoldTimeConfiguredGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* holdTime);
L7_RC_t usmDbBgp4PeerHoldTimeConfiguredSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32 holdTime);
L7_RC_t usmDbBgp4PeerKeepAliveConfiguredGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* keepAliveConfigured);
L7_RC_t usmDbBgp4PeerKeepAliveConfiguredSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32 keepAliveConfigured);
L7_RC_t usmDbBgp4PeerAuthTypeGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 *authType);
L7_RC_t usmDbBgp4PeerAuthTypeSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 authType);
L7_RC_t usmDbBgp4PeerAuthKeyGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uchar8 *authKeyStr);
L7_RC_t usmDbBgp4PeerAuthKeySet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uchar8 *authKeyStr);

/*L7_RC_t usmDbBgp4PeerMinAsOriginIntervalGet(L7_uint32 UnitIndex, L7_uint32 peerId, L7_ushort16* minAsOrigInterval);
L7_RC_t usmDbBgp4PeerMinAsOriginIntervalSet(L7_uint32 UnitIndex, L7_uint32 peerId, L7_ushort16 minAsOrigInterval);
L7_RC_t usmDbBgp4PeerMinRouteAdvIntervalGet(L7_uint32 UnitIndex, L7_uint32 peerId, L7_ushort16* minRouteAdvInterval);
L7_RC_t usmDbBgp4PeerMinRouteAdvIntervalSet(L7_uint32 UnitIndex, L7_uint32 peerId, L7_ushort16 minRouteAdvInterval);
*/
L7_RC_t usmDbBgp4PeerInUpdateElapsedTimeGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, usmDbTimeSpec_t *inUpdateElapsedTime);
L7_RC_t usmDbBgp4PeerReset(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr);

L7_RC_t usmDbBgp4PathAttrEntryGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerId);
L7_RC_t usmDbBgp4PathAttrEntryGetNext(L7_uint32 UnitIndex, L7_uint32* prefix, L7_int32* prefixLen, L7_uint32* remotePeerId);
L7_RC_t usmDbBgp4PathAttrOriginGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_ushort16* origin);
L7_RC_t usmDbBgp4PathAttrAsPathSegmentGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_uchar8* asPathSegment, L7_uint32* asPathSegLen);
L7_RC_t usmDbBgp4PathAttrNextHopGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_uint32* nextHop);
L7_RC_t usmDbBgp4PathAttrMEDGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_long32* med);
L7_RC_t usmDbBgp4PathAttrLocalPrefGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_long32* localPref);
L7_RC_t usmDbBgp4PathAttrAtomicAggregateGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_ushort16* atomicAggregate);
L7_RC_t usmDbBgp4PathAttrAggregatorAsGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_ushort16* aggregatorAs);
L7_RC_t usmDbBgp4PathAttrAggregatorAddrGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_uint32* aggregatorAddr);
L7_RC_t usmDbBgp4PathAttrCalcLocalPrefGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_long32* calcLocalPref);
L7_RC_t usmDbBgp4PathAttrCommunitiesGet(L7_uint32 UnitIndex, L7_uint32 prefix, 
                                        L7_int32 prefixLen, L7_uint32 remotePeerAddr, 
                                        L7_uint32 *communities, 
                                        L7_uint32 *numCommunities);
L7_RC_t usmDbBgp4PathAttrBestGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_ushort16* best);
L7_RC_t usmDbBgp4PathAttrUnknownGet(L7_uint32 UnitIndex, L7_uint32 prefix, L7_int32 prefixLen, L7_uint32 remotePeerAddr, L7_uchar8* unknown, L7_uint32* unknownLen);

#endif
