/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_intf.h
*
* @purpose    Interface Related Functionality
*
* @component  PIM-DM
*
* @comments
*
* @create     07/06/2006
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_INTF_H_
#define _PIMDM_INTF_H_

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "pimdm_main.h"

/*******************************************************************************
**                        General Definitions                                 **
*******************************************************************************/

/*******************************************************************************
**                        Data Structure Definitions                          **
*******************************************************************************/
/* PIM-DM Interface Structure */
typedef struct pimdmInterface_s
{
  L7_uint32 rtrIfNum;  /* Router Interface Index.
                          * A value of 0 indicates an invalid interface.
                          */

  /* Config parameters */
  L7_uint32 helloInterval;          /* Seconds */
  L7_uint32 triggeredHelloDelay;    /* Seconds */
  L7_uint32 helloHoldTime;          /* Seconds */
  L7_uint32 propagationDelay;       /* Milli-seconds */
  L7_uint32 overrideIntvl;          /* Milli-seconds */ 
  L7_uint32 ttlThreshold;           /* TTL threshold */ 
  L7_inet_addr_t ipAddress;         /* Primary IP address */
  L7_inet_addr_t netMask;           /* Network Mask */

  /* Operational paramters */
  L7_uint32 genID;                  /* Unique ID of this router on this interface */
  L7_BOOL   isLanDelayEnabled;      /* All PIM-DM neighbors on this interface support LAN Prune Delay option? */
  L7_uint32 effPropagationDelay;    /* Effective propagation delay in milli seconds */
  L7_uint32 effOverrideIntvl;       /* Effective prune override interval in milliseconds */ 
  L7_BOOL   isStateRefreshCapable;  /* State Refresh Capable? */
  L7_uint32 effStateRefreshIntvl;   /* Effective State Refresh interval in seconds */

  /* Neighbor related parameters */
  L7_APP_TMR_HNDL_t helloTimer;     /* Hello Timer handle */
  L7_uchar8 nbrBitmap[MCAST_BITX_NUM_BITS(PIMDM_MAX_NEIGHBORS)];
                                    /* Bitmap of neighbors on this interface */
  L7_uint32 numNbrs;                /* Total number of neighbors on this interface */
  L7_uint32 intfTmrHandle;          /* Handle List Timer Handle for the Interface Block */

  L7_uint32  intfStats[PIMDM_CTRL_PKT_MAX][PIMDM_STATS_TYPE_MAX];

  pimdmCB_t *pimdmCB;               /* Back pointer to the PIM-DM Control Block */
} pimdmInterface_t;

#define PIMDM_MAX_PROTOCOL_PACKETS MCAST_MAX_INTERFACES 
#define PIMDM_MAX_JP_PACKETS        MCAST_MAX_INTERFACES
/* Maximum buff size to send or receive packet */
#define PIMDM_PKT_SIZE_MAX                   L7_MULTICAST_MAX_IP_MTU  

#define PIMDM_MAX_JP_MSG_SIZE            PIM_MAX_JP_MSG_SIZE
#define PIMDM_DEFAULT_JP_BUNDLE_TIME     2 
/*
 * The complicated structure used by the more complicated Join/Prune
 * message building
 */
typedef struct pimdmJPMsgStoreBuff_t
{
  L7_BOOL buildJPMsgUse;         /* Is this buffer in Use */
  L7_uchar8 *pJPMsg;             /* The Join/Prune message */
  L7_uint32 jpMsgSize;           /* Size of the Join/Prune message (in bytes) */
  L7_ushort16 holdtime;          /* Join/Prune message holdtime field */
  L7_inet_addr_t currGrpAddr;    /* Current group address */
  L7_uchar8 currGrpMaskLen;      /* Current group masklen */
  L7_uchar8 *pJoinList;          /* The working area for the join addresses */
  L7_uint32 joinListSize;        /* The size of the pJoinList (in bytes) */
  L7_ushort16 numJoinAddrs;      /* No of the join addresses in pJoinList */
  L7_uchar8 *pPruneList;         /* The working area for the prune addresses */
  L7_uint32 pruneListSize;       /* The size of the pPruneList (in bytes) */
  L7_ushort16 numPruneAddrs;     /* No of the prune addresses in pPruneList */
  L7_uchar8 *pNumGrps;           /* Pointer to number_of_groups in pJPMsg */
  L7_inet_addr_t destAddr;       /* Ip packet dest address */
  PIMDM_CTRL_PKT_TYPE_t pktType; /* PIMDM_CTRL_PKT_TYPE_t */
} pimdmJPMsgStoreBuff_t;

typedef struct pimdmSendJoinPruneData_s
{
  L7_inet_addr_t *pNbrAddr;
  L7_ushort16 holdtime;
  L7_inet_addr_t *pSrcAddr;  
  L7_uchar8 srcAddrMaskLen;  
  L7_inet_addr_t *pGrpAddr; 
  L7_uchar8 grpAddrMaskLen;
  L7_uchar8 addrFlags;
  L7_uint32 rtrIfNum;
  PIMDM_CTRL_PKT_TYPE_t pktType;
  L7_BOOL pruneEchoFlag; /* TRUE -> Send Prune-Echo */
  L7_inet_addr_t *destAddr;
} pimdmSendJoinPruneData_t;

/* PIM-DM Neighbor Structure */
typedef struct pimdmNeighbor_s
{
  L7_inet_addr_t addr;              /* IP address of the Neighbor */
  L7_uint32      holdTime;          /* Holdtime in seconds */
  L7_BOOL        bLanPruneDelayEnabled; /* LAN Prune Delay enabled? */
  L7_uint32      lanPruneDelay;     /* Prune Delay Interval in milliseconds*/
  L7_uint32      overrideIntvl;     /* Prune override interval in milliseconds */
  L7_BOOL        bStateRefreshCapable;
  L7_uint32      stateRefreshIntvl; /* State Refresh interval in seconds */
  L7_uint32      genID;             /* Generation ID */
  L7_APP_TMR_HNDL_t livenessTimer;  /* Expiry Timer to consider this neighbor */
  L7_uint32      livenessTimerGenId; /* Generation ID of the liveness timer */
  L7_uint32      createTime;        /* Time reference when created */
  L7_uint32      rtrIfNum;        /* Router interface index of the associated interface */
  L7_uint32      nbrTmrHandle;      /* Handle List Timer Handle for the Neighbor Block */
  pimdmCB_t      *pimdmCB;          /* Back pointer to the PIM-DM Control Block */
  pimdmJPMsgStoreBuff_t pimdmBuildJPMsg; /* A structure for fairly
                     * complicated Join/Prune
                     * message construction.
                     */
} pimdmNeighbor_t;

/* PIM-DM Interface Information Structure */
typedef struct pimdmIntfInfo_s
{
  pimdmInterface_t intfs[PIMDM_MAX_INTERFACES];
  pimdmNeighbor_t  nbrs[PIMDM_MAX_NEIGHBORS];
  L7_int32         intIfNum[PIMDM_MAX_INTERFACES];
  void             *semId;
} pimdmIntfInfo_t;

/* Neighbor information from/to a Hello Packet */
typedef struct pimdmIntfNbrInfo_s
{
  L7_inet_addr_t *pNbrAddr;
  L7_uint32      holdTime;
  L7_BOOL        bValidLanPruneDelay;  /* L7_TRUE : If LPD Option present in pkt, L7_FALSE : otherwise */
  L7_uint32      lanPruneDelay;
  L7_uint32      overrideInterval;
  L7_BOOL        bValidGenId;          /* L7_TRUE : If GenID Option present in pkt, L7_FALSE : otherwise */
  L7_uint32      genId;
  L7_BOOL        bStateRefreshCapable; /* L7_TRUE : If StateRefresh Option present in pkt, L7_FALSE : otherwise */
  L7_uint32      stateRefreshInterval;
} pimdmIntfNbrInfo_t;

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_BOOL
pimdmIntfIsEnabled (pimdmCB_t *pimdmCB,
                    L7_uint32 rtrIfNum);

extern L7_RC_t
pimdmIntfIntIfNumGet (pimdmCB_t *pimdmCB,
                      L7_uint32 rtrIfNum,
                      L7_uint32 *pIntIfNum);

extern L7_RC_t
pimdmIntfIpAddressGet (pimdmCB_t      *pimdmCB,
                       L7_uint32      rtrIfNum,
                       L7_inet_addr_t *pIpAddress);

extern L7_RC_t
pimdmIntfNetMaskGet (pimdmCB_t      *pimdmCB,
                     L7_uint32      rtrIfNum,
                     L7_inet_addr_t *pNetMask);

extern L7_RC_t
pimdmIntfHelloIntervalGet (pimdmCB_t *pimdmCB,
                           L7_uint32 rtrIfNum,
                           L7_uint32 *pHelloInterval);

extern L7_BOOL
pimdmIntfIsStateRefreshCapable (pimdmCB_t *pimdmCB,
                                L7_uint32 rtrIfNum);

extern L7_int32
pimdmIntfLanPropagationDelayGet (pimdmCB_t *pimdmCB,
                                 L7_uint32 rtrIfNum);

extern L7_int32
pimdmIntfOverrideIntervalGet (pimdmCB_t *pimdmCB,
                              L7_uint32 rtrIfNum);

extern L7_BOOL
pimdmIntfIsNeighborValid (pimdmCB_t      *pimdmCB,
                          L7_uint32      rtrIfNum,
                          L7_inet_addr_t *pNbrAddr);

extern L7_int32
pimdmIntfNumNeighborsGet (pimdmCB_t *pimdmCB,
                          L7_uint32 rtrIfNum);

extern L7_RC_t
pimdmIntfNeighborRtrIfIndexGet (pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pNbrAddr,
                                L7_uint32      *pRtrIfNum);

extern L7_RC_t
pimdmIntfNeighborUpTimeGet (pimdmCB_t      *pimdmCB,
                            L7_inet_addr_t *pNbrAddr,
                            L7_uint32       rtrIfNum,
                            L7_uint32      *pUpTime);

extern L7_RC_t
pimdmIntfNeighborExpiryTimeGet (pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pNbrAddr,
                                L7_uint32       rtrIfNum,
                                L7_uint32      *pExpiryTime);

extern L7_RC_t
pimdmIntfNeighborGet(pimdmCB_t      *pimdmCB,
                     L7_inet_addr_t *pNbrAddr);

extern L7_RC_t
pimdmIntfNeighborByIfIndexGet (pimdmCB_t *pimdmCB,
                               L7_uint32 rtrIfNum,
                               L7_inet_addr_t *pNbrAddr);

extern L7_RC_t
pimdmIntfNeighborByIfIndexNextGet (pimdmCB_t      *pimdmCB,
                          L7_inet_addr_t *pNbrAddr,
                          L7_uint32 *rtrIfNum);

extern L7_RC_t
pimdmIntfNeighborNextGet (pimdmCB_t      *pimdmCB,
                          L7_inet_addr_t *pNbrAddr);

extern L7_RC_t
pimdmIntfHelloMsgProcess (pimdmCB_t          *pimdmCB,
                          L7_uint32          rtrIfNum,
                          pimdmIntfNbrInfo_t *pNbrInfo);

extern L7_RC_t
pimdmIntfUp (pimdmCB_t      *pimdmCB,
             L7_uint32      rtrIfNum, 
             L7_uint32      intIfNum,
             L7_uint32      helloInterval,
             L7_uint32      triggeredHelloDelay,
             L7_uint32      helloHoldTime,
             L7_uint32      propagationDelay,
             L7_uint32      overrideIntvl,
             L7_inet_addr_t *pIpAddr,
             L7_inet_addr_t *pNetMask);

extern L7_RC_t
pimdmIntfDown (pimdmCB_t *pimdmCB,
               L7_uint32 rtrIfNum);

L7_RC_t
pimdmIntfPurge (pimdmCB_t *pimdmCB,
                L7_uint32 rtrIfNum);

extern L7_RC_t
pimdmIntfInit(pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmIntfDeInit(pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmIntfStateRefreshIntervalGet (pimdmCB_t *pimdmCB,
                                  L7_uint32 rtrIfNum,
                                  L7_uint32 *effStRfrThreshold);
extern L7_RC_t
pimdmIntfStateRefreshTTLThresholdSet (pimdmCB_t *pimdmCB,
                                      L7_uint32 rtrIfNum,
                                      L7_uint32 ttlThreshold);
extern L7_RC_t
pimdmIntfStateRefreshTTLThresholdGet (pimdmCB_t *pimdmCB,
                                      L7_uint32 rtrIfNum,
                                      L7_uint32 *ttlThreshold);
extern void
pimdmIntfAllShow(pimdmCB_t *pimdmCB);

extern void
pimdmIntfShow(pimdmCB_t *pimdmCB, L7_uint32 rtrIfNum);

extern void
pimdmIntfNbrAllShow(pimdmCB_t *pimdmCB);

extern void
pimdmIntfNbrShow(pimdmCB_t *pimdmCB, L7_uint32 nbrIndex);

extern L7_RC_t
pimdmIntfAdminModeSet (MCAST_CB_HNDL_t pimdmCbHandle,
                       pimdmMapEvent_t *mapLayerInfo);

extern L7_RC_t
pimdmIntfHelloIntervalSet (MCAST_CB_HNDL_t pimdmCbHandle,
                           pimdmMapEvent_t *mapLayerInfo);

extern L7_RC_t pimdmNeighborFind(struct pimdmCB_s * pimdmCB, 
                                 struct pimdmInterface_s *pIntfEntry, 
                                 L7_inet_addr_t *pSrcAddr, 
                                 pimdmNeighbor_t ** pNbrEntry);  
extern L7_RC_t pimdmIntfEntryGet(pimdmCB_t *pimdmCB, L7_uint32 rtrIfNum,
                          pimdmInterface_t **ppIntfEntry);

extern void
pimdmIntfStatsClear (pimdmCB_t *pimdmCB,
                     L7_uint32 rtrIfNum,
                     PIMDM_CTRL_PKT_TYPE_t msgType,
                     PIMDM_STATS_TYPE_t statsType);

extern L7_RC_t
pimdmIntfStatsUpdate (pimdmCB_t *pimdmCB,
                      L7_uint32 rtrIfNum,
                      PIMDM_CTRL_PKT_TYPE_t msgType,
                      PIMDM_STATS_TYPE_t statsType);
extern void
pimdmIntfStatsShow (pimdmCB_t *pimdmCB,
                    L7_uint32 rtrIfNum);

extern void
pimdmIntfStatsDisplay (pimdmCB_t *pimdmCB,
                       L7_uint32 rtrIfNum);

#endif /* _PIMDM_INTF_H_ */
