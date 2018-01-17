/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmNeighbor.h
*
* @purpose Contains structs for Neighbor list.
*
* @component pimsm
*
* @comments 
*
* @create 01/01/2006
*
* @author vmurali/dsatyanarayana
* @end
*
**********************************************************************/
#ifndef _PIMSM_NEIGHBOR_INTF_H_
#define _PIMSM_NEIGHBOR_INTF_H_
#include "l7sll_api.h"
#include "l3_addrdefs.h"
/*#include "pimsmmain.h"*/
#include "l7apptimer_api.h"
#include "pimsmtimer.h"

#define PIMSM_MAX_NBR_SECONDARY_ADDR L7_MCAST_MAX_INTF_ADDRS
/*
 * The complicated structure used by the more complicated Join/Prune
 * message building
 */
typedef struct pimsmJPMsgStoreBuff_t
{
  L7_BOOL  buildJPMsgUse;
  L7_uchar8 *pJPMsg;       /* The Join/Prune message                     */
  L7_uint32 jpMsgSize;  /* Size of the Join/Prune message (in bytes)  */
  L7_ushort16 holdtime;         /* Join/Prune message holdtime field          */
  L7_inet_addr_t currGrpAddr;       /* Current group address     */
  L7_uchar8  currGrpMaskLen;/* Current group masklen                      */
  L7_uchar8 *pJoinList;        /* The working area for the join addresses    */
  L7_uint32 joinListSize;   /* The size of the pJoinList (in bytes)       */
  L7_ushort16 numJoinAddrs; /* Number of the join addresses in pJoinList  */
  L7_uchar8 *pPruneList;       /* The working area for the prune addresses   */
  L7_uint32 pruneListSize;  /* The size of the pPruneList (in bytes)      */
  L7_ushort16 numPruneAddrs;/* Number of the prune addresses in pPruneList*/
  L7_uchar8 *pRpJoinList;     /* The working area for RP join addresses     */
  L7_uint32 rpJoinListSize;/* The size of the pRpJoinList (in bytes)    */
  L7_ushort16 numRpJoinAddrs;/* Number of RP addresses in pRpJoinList   */
  L7_uchar8 *pRpPruneList;     /* The working area for RP prune addresses   */
  L7_uint32 rpPruneListSize;/* The size of the pRpPruneList (in bytes)  */
  L7_ushort16 numRpPruneAddrs;/* Number of RP addresses in pRpPruneList */
  L7_uchar8 *pNumGrps;   /* Pointer to number_of_groups in pJPMsg  */
} pimsmJPMsgStoreBuff_t;   


typedef struct pimsmNeighborAddrList_s
{
  L7_inet_addr_t  pimsmPrimaryAddress;
  L7_inet_addr_t  pimsmSecondaryAddrList[PIMSM_MAX_NBR_SECONDARY_ADDR];
  L7_BOOL  pimsmSecondaryAddrActive[PIMSM_MAX_NBR_SECONDARY_ADDR];
}pimsmNeighborAddrList_t;

typedef struct pimsmNeighborEntry_s
{
  L7_sll_member_t * next;
  L7_uint32   pimsmNeighborIfIndex;
  pimsmNeighborAddrList_t pimsmNeighborAddrList;
  /*L7_uint32      pimsmNeighborUpTime;*/
  L7_uint32   pimsmNeighborCreateTime;
  L7_APP_TMR_HNDL_t pimsmNeighborExpiryTimer;
  L7_uint32 pimsmNeighborExpiryTimerHandle;
  pimsmTimerData_t  pimsmNeighborExpiryTimerParam;
  L7_uint32   pimsmNeighborGenerationIDValue;
  L7_uint32   pimsmNeighborDRPriority;    
  pimsmJPMsgStoreBuff_t pimsmBuildJPMsg; /* A structure for fairly
                     * complicated Join/Prune
                     * message construction.
                     */
  L7_BOOL     pimsmNeighborLANPruneDelayPresent;                     
  L7_BOOL     pimsmNeighborTrackingSupportPresent;  
  L7_uint32   pimsmNeighborPropagationDelay;
  L7_uint32   pimsmNeighborOverrideInterval;
  L7_uint32   pimsmNbrBufferPoolId;                     
  /*struct pimsmNeighborEntry_s     *pSelf;    For sanity checks */                      
}pimsmNeighborEntry_t;


struct pimsmCB_s;
struct pimsmHelloParams_s;
struct pimsmInterfaceEntry_s;

extern L7_RC_t  pimsmNeighborCreate(struct pimsmCB_s * pimsmCb, 
                                    L7_inet_addr_t * pSrcAddr,  
                                    struct pimsmInterfaceEntry_s *pIntfEntry,
                                    struct pimsmHelloParams_s *pHelloParams);
extern L7_RC_t  pimsmNeighborUpdate(struct pimsmCB_s * pimsmCb,
                                    struct pimsmInterfaceEntry_s *pIntfEntry,
                                    pimsmNeighborEntry_t * pNbrEntry,
                                    struct pimsmHelloParams_s *pHelloParams);
extern L7_RC_t pimsmNeighborInit(struct pimsmCB_s * pimsmCb, 
                                 struct pimsmInterfaceEntry_s *pIntfEntry);
extern L7_RC_t pimsmNeighborDeInit(struct pimsmCB_s * pimsmCb, 
                                   struct pimsmInterfaceEntry_s *pIntfEntry);
extern L7_RC_t pimsmNeighborDelete(struct pimsmCB_s * pimsmCb, 
                                   struct pimsmInterfaceEntry_s *pIntfEntry, 
                                   pimsmNeighborEntry_t  * pNbrEntry);
extern L7_RC_t pimsmNeighborGetFirst(struct pimsmCB_s * pimsmCb, 
                                     struct pimsmInterfaceEntry_s *pIntfEntry, 
                                     pimsmNeighborEntry_t **pNbrEntry);
extern L7_RC_t pimsmNeighborNextGet(struct pimsmCB_s * pimsmCb, 
                                    struct pimsmInterfaceEntry_s *pIntfEntry,
                                    pimsmNeighborEntry_t  * pCurrNbrEntry, 
                                    pimsmNeighborEntry_t  ** ppNextNbrEntry);
extern L7_RC_t pimsmNeighborFind(struct pimsmCB_s * pimsmCb, 
                                 struct pimsmInterfaceEntry_s *pIntfEntry, 
                                 L7_inet_addr_t *pSrcAddr, 
                                 pimsmNeighborEntry_t ** pNbrEntry);            
extern L7_RC_t pimsmNbrPrimaryAddrGet(struct pimsmCB_s * pimsmCb, 
                         pimsmNeighborEntry_t  *pimsmNbrEntry,
                         L7_inet_addr_t *secAddr,
                         L7_inet_addr_t *primaryAddr);

#endif /* _PIMSM_NEIGHBOR_INTF_H_ */

