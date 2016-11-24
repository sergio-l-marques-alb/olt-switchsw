/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_api.h
*
* @purpose    PIM-DM APIs/Definitions visible to the Mapping Layer
*
* @component  PIM-DM
*
* @comments   none
*
* @create     23-Sep-05
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDMAPI_H
#define _PIMDMAPI_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/

#include "l7_common.h"
#include "l3_mcast_commdefs.h"
#include "l3_comm_structs.h"
#include "rto_api.h"
#include "ipv6_commdefs.h"
#include "l7_rto6_api.h"
#include "mfc_api.h"

/*******************************************************************************
**                    Extern Variable Declarations                            **
*******************************************************************************/

/* None */

/*******************************************************************************
**                    Data Structure Definitions                              **
*******************************************************************************/
typedef enum
{
  PIMDM_MRT_ENTRY_PRUNE_REASON_OTHER = 1,
  PIMDM_MRT_ENTRY_PRUNE_REASON_PRUNE,
  PIMDM_MRT_ENTRY_PRUNE_REASON_ASSERT,
}PIMDM_MRT_ENTRY_PRUNE_REASON_t;

/*******************************************************************************
**                    Extern Function Prototypes                              **
*******************************************************************************/

/* Mapping Layer Interfacing Routine Declarations.
 */
extern L7_RC_t
pimdmExtenApiNbrUpTimeGet (MCAST_CB_HNDL_t cbHandle,
                           L7_inet_addr_t *nbrAddr,
                           L7_uint32 *nbrCreateTime);                                           

extern L7_RC_t 
pimdmExtenApiNbrUpTimeByIfIndexGet(MCAST_CB_HNDL_t cbHandle, 
                                   L7_inet_addr_t *nbrIpAddr,
                                   L7_uint32 intIfNum, 
                                   L7_uint32 *nbrUpTime);

extern L7_RC_t
pimdmExtenApiNbrExpireTimeGet (MCAST_CB_HNDL_t cbHandle,
                               L7_inet_addr_t *nbrAddr,
                               L7_uint32 *nbrExpiryTime);
extern L7_RC_t 
pimdmExtenApiNbrExpireTimeByIfIndexGet(MCAST_CB_HNDL_t cbHandle, 
                                       L7_inet_addr_t *nbrIpAddr,
                                       L7_uint32 intIfNum, 
                                       L7_uint32 *nbrExpireTime);

extern L7_RC_t
pimdmExtenApiNbrCountGet (MCAST_CB_HNDL_t cbHandle,
                          L7_uint32 rtrIfNum,
                          L7_uint32 *nbrCount);

extern L7_RC_t
pimdmExtenApiNbrIntfIndexGet (MCAST_CB_HNDL_t cbHandle,
                              L7_inet_addr_t *nbrIpAddr,
                              L7_uint32 *rtrIfNum);

extern L7_RC_t
pimdmExtenApiNbrEntryGet (MCAST_CB_HNDL_t cbHandle,
                          L7_inet_addr_t *nbrAddr);

extern L7_RC_t
pimdmExtenApiNbrEntryNextGet (MCAST_CB_HNDL_t cbHandle,
                              L7_inet_addr_t *nbrAddr);

extern L7_RC_t
pimdmExtenApiNbrEntryByIfIndexGet (MCAST_CB_HNDL_t cbHandle,
                                   L7_uint32 intIfNum,
                                   L7_inet_addr_t *nbrAddr);

extern L7_RC_t
pimdmExtenApiNbrEntryByIfIndexNextGet (MCAST_CB_HNDL_t cbHandle,
                                       L7_uint32 *outIntIfNum,
                                       L7_inet_addr_t *nbrAddr);

extern L7_RC_t
pimdmExtenApiIpMRouteAssertMetricGet (MCAST_CB_HNDL_t cbHandle,
                                      L7_inet_addr_t *grpAddr,
                                      L7_inet_addr_t *srcAddr,
                                      L7_inet_addr_t *srcMask,
                                      L7_uint32 *assertMetric);

extern L7_RC_t
pimdmExtenApiIpMRouteAssertMetricPrefGet (MCAST_CB_HNDL_t cbHandle,
                                          L7_inet_addr_t *grpAddr,
                                          L7_inet_addr_t *srcAddr,
                                          L7_inet_addr_t *srcMask,
                                          L7_uint32 *assertMetricPref);

extern L7_RC_t
pimdmExtenApiIpMRouteAssertRPTBitGet (MCAST_CB_HNDL_t cbHandle,
                                      L7_inet_addr_t *grpAddr,
                                      L7_inet_addr_t *srcAddr,
                                      L7_inet_addr_t *srcMask,
                                      L7_uint32 *rptBitStatus);

extern L7_RC_t
pimdmExtenApiIpMRouteUpstreamAssertTimerGet (MCAST_CB_HNDL_t cbHandle,
                                             L7_inet_addr_t *grpAddr,
                                             L7_inet_addr_t *srcAddr,
                                             L7_inet_addr_t *srcMask,
                                             L7_uint32 *upstrmAssertTime);

extern L7_RC_t
pimdmExtenApiIpMRouteEntryGet (MCAST_CB_HNDL_t cbHandle,
                               L7_inet_addr_t *grpAddr,
                               L7_inet_addr_t *srcAddr,
                               L7_inet_addr_t *srcMask);

extern L7_RC_t
pimdmExtenApiIpMRouteEntryNextGet (MCAST_CB_HNDL_t cbHandle,
                                   L7_inet_addr_t *grpAddr,
                                   L7_inet_addr_t *srcAddr,
                                   L7_inet_addr_t *srcMask);

L7_RC_t
pimdmExtenApiIpMRouteNextHopEntryGet (MCAST_CB_HNDL_t cbHandle,
                                      L7_inet_addr_t *nextHopGrpAddr,
                                      L7_inet_addr_t *nextHopSrcAddr,
                                      L7_inet_addr_t *nextHopSrcMask,
                                      L7_uint32 nextHopRtrIfNum,
                                      L7_inet_addr_t *nextHopAddress);

extern L7_RC_t
pimdmExtenApiIpMRouteNextHopEntryNextGet (MCAST_CB_HNDL_t cbHandle,
                                          L7_inet_addr_t *nextHopGrpAddr,
                                          L7_inet_addr_t *nextHopSrcAddr,
                                          L7_inet_addr_t *nextHopSrcMask,
                                          L7_uint32 *nextHopRtrIfNum,
                                          L7_inet_addr_t *nextHopAddress);

extern L7_RC_t
pimdmExtenApiIpMRouteNextHopPruneReasonGet (MCAST_CB_HNDL_t cbHandle,
                                            L7_inet_addr_t *nextHopGrpAddr,
                                            L7_inet_addr_t *nextHopSrcAddr,
                                            L7_inet_addr_t *nextHopSrcMask,
                                            L7_uint32 nextHopRtrIfNum,
                                            L7_inet_addr_t *nextHopAddress,
                                            L7_uint32 *pruneReason);

extern L7_RC_t
pimdmExtenApiIpMRouteFlagsGet (MCAST_CB_HNDL_t cbHandle,
                               L7_inet_addr_t *grpAddr,
                               L7_inet_addr_t *srcAddr,
                               L7_inet_addr_t *srcMask,
                               L7_uint32 *entryFlags);

extern L7_RC_t
pimdmExtenApiIpMRouteOIFNextGet (MCAST_CB_HNDL_t cbHandle,
                                               L7_inet_addr_t *grpAddr,
                                               L7_inet_addr_t *srcAddr,
                                               L7_inet_addr_t *srcMask,
                                               L7_uint32 *rtrIfNum,
                                               L7_uint32 *entryState,
                                               L7_uint32 *entryMode,
                                               L7_uint32 *entryUPTime,
                                               L7_uint32 *entryExpireTime);

extern L7_RC_t
pimdmExtenApiIpRouteOutIntfEntryNextGet (MCAST_CB_HNDL_t cbHandle,
                                         L7_inet_addr_t *grpAddr,
                                         L7_inet_addr_t *srcAddr,
                                         L7_inet_addr_t *srcMask,
                                         L7_uint32 *outRtrIfNum);

extern L7_RC_t
pimdmExtenApiRouteIfIndexGet (MCAST_CB_HNDL_t cbHandle,
                              L7_inet_addr_t *grpAddr,
                              L7_inet_addr_t *srcAddr,
                              L7_inet_addr_t *srcMask,
                              L7_uint32 *rtrIfNum);

extern L7_RC_t
pimdmExtenApiRouteUptimeGet (MCAST_CB_HNDL_t cbHandle,
                             L7_inet_addr_t *grpAddr,
                             L7_inet_addr_t *srcAddr,
                             L7_inet_addr_t *srcMask,
                             L7_uint32 *entryUPTime);

extern L7_RC_t
pimdmExtenApiRouteExpiryTimeGet (MCAST_CB_HNDL_t cbHandle,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_uint32 *entryExpireTime);

extern L7_RC_t
pimdmExtenApiIpMRouteRtAddrGet (MCAST_CB_HNDL_t cbHandle,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_inet_addr_t *rpfAddr);

extern L7_RC_t
pimdmExtenApiIpMRouteRtMaskGet (MCAST_CB_HNDL_t cbHandle,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_inet_addr_t *rpfMask);

extern L7_RC_t
pimdmExtenApiIpMRouteRtTypeGet (MCAST_CB_HNDL_t cbHandle,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_uint32 *routeType);

extern L7_RC_t
pimdmExtenApiIpMRouteRtProtoGet (MCAST_CB_HNDL_t cbHandle,
                                 L7_inet_addr_t *grpAddr,
                                 L7_inet_addr_t *srcAddr,
                                 L7_inet_addr_t *srcMask,
                                 L7_uint32 *routeProto);
extern L7_RC_t
pimdmExtenApiIpMRouteRpfAddrGet (MCAST_CB_HNDL_t cbHandle,
                                 L7_inet_addr_t *grpAddr,
                                 L7_inet_addr_t *srcAddr,
                                 L7_inet_addr_t *srcMask,
                                 L7_inet_addr_t *rpfAddr);

extern L7_RC_t
pimdmExtenApiIpMRouteEntryCountGet (MCAST_CB_HNDL_t cbHandle,
                                    L7_uint32 *entryCount);

extern L7_RC_t
pimdmExtenApiIpMRouteHighestEntryCountGet (MCAST_CB_HNDL_t cbHandle,
                                           L7_uint32 *highEntryCount);

extern L7_RC_t
pimdmExtenApiRouteDelete (MCAST_CB_HNDL_t cbHandle,
                          L7_inet_addr_t *grpAddr,
                          L7_inet_addr_t *srcAddr);

extern L7_RC_t
pimdmExtenApiRouteDeleteAll (MCAST_CB_HNDL_t cbHandle);

#endif /* _PIMDMAPI_H */
