
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_mrt.h
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_MRT_H
#define _PIMDM_MRT_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l7apptimer_api.h"
#include "l3_addrdefs.h"
#include "l7sll_api.h"
#include "mcast_bitset.h"
#include "mcast_wrap.h"
#include "mcast_api.h"
#include "pimdm_ctrl.h"
#include "pimdm_main.h"
#include "pimdm_pkt.h"

/*******************************************************************************
**                        Implementation Specific Definitions                 **
*******************************************************************************/

/* Number of retrires before we receive a Graft-Ack from Upstream.
 */
#define PIMDM_DEFAULT_GRAFT_RETRY_LIMIT     5

/*******************************************************************************
**                     Data Structure Definitions                             **
*******************************************************************************/

/*********************************************************************
*                 PIM-DM Upstream State Machine State
*********************************************************************/

typedef enum
{
  PIMDM_UPSTRM_STATE_FORWD = 0,
  PIMDM_UPSTRM_STATE_PRUNE,
  PIMDM_UPSTRM_STATE_ACKPN,
  PIMDM_UPSTRM_MAX_STATES,
}PIMDM_UPSTRM_STATE_t;

/*********************************************************************
*              PIM-DM State Refresh State Machine States
*********************************************************************/
typedef enum
{
  PIMDM_STATE_RFR_STATE_NO_ORIG = 0,
  PIMDM_STATE_RFR_STATE_ORIG,
  PIMDM_STATE_RFR_MAX_STATES,
}PIMDM_STATE_RFR_STATE_t;

/*********************************************************************
*              PIM-DM Downstream State Machine States
*********************************************************************/
typedef enum
{
  PIMDM_DNSTRM_STATE_NOINFO = 0,
  PIMDM_DNSTRM_STATE_PRUNE_PENDING,
  PIMDM_DNSTRM_STATE_PRUNED,
  PIMDM_DNSTRM_MAX_STATES
}PIMDM_DNSTRM_STATE_t;

/*********************************************************************
*                 PIM-DM Assert State Machine States
*********************************************************************/
typedef enum
{
  PIMDM_ASSERT_STATE_NOINFO = 0,
  PIMDM_ASSERT_STATE_ASSERT_WINNER,
  PIMDM_ASSERT_STATE_ASSERT_LOSER,
  PIMDM_ASSERT_MAX_STATES,
}PIMDM_ASSERT_STATE_t;

/*********************************************************************
*                 PIM-DM Type of MRT change
*********************************************************************/
typedef enum
{
  PIMDM_MRT_CHANGE_LOCAL_RCV = 0,
  PIMDM_MRT_CHANGE_NBR_LIST,
  PIMDM_MRT_CHANGE_BOUNDARY,
  PIMDM_MRT_CHANGE_INTERNAL,
  PIMDM_MRT_CHANGE_MAX_STATES,
}PIMDM_MRT_CHANGE_TYPE_t;

/*********************************************************************
*                Upstream Graft/Prune State Info
*********************************************************************/
typedef struct pimdmGraftPruneState_s
{
  PIMDM_UPSTRM_STATE_t grfPrnState;    /* Current State */
  L7_APP_TMR_HNDL_t    grftRetryTimer; /* Graft Retry Timer, GRT(S,G) */
  L7_APP_TMR_HNDL_t    overrideTimer;  /* Override Timer, OT(S,G) */
  L7_APP_TMR_HNDL_t    prnLmtTimer;    /* Prune Limit Timer, PLT(S,G) */
}pimdmGraftPruneState_t;

/*********************************************************************
*                 Upstream Originator State Info
*********************************************************************/
typedef struct pimdmOrigState_s
{
  PIMDM_STATE_RFR_STATE_t origState;      /* Current State */
  L7_APP_TMR_HNDL_t       srcActiveTimer; /* Source Active Timer, SAT(S,G) */
  L7_APP_TMR_HNDL_t       stateRfrTimer;  /* State Refresh Timer, SRT(S,G) */ 
  L7_uint32               ttl;
  L7_uint32               timeSinceLastSRMsg; /* Seconds elapsed since last
                                                 we last received SR Msg */
  L7_uint32               msgSentCount;  /* Count of the Sent State Refresh Msgs */
}pimdmOrigState_t;

/*********************************************************************
*                 Assert Info
*********************************************************************/
typedef struct pimdmAssertInfo_s
{
  PIMDM_ASSERT_STATE_t assertState;
  L7_APP_TMR_HNDL_t    assertTimer;         /* Assert Timer, AT(S,G,I) */
  L7_inet_addr_t       assertWnrAddr;       /* Assert Winner's IP Address */
  L7_uint32            assertWnrAsrtMetric; /* Assert Winner's Metric to the Source */
  L7_uint32            assertWnrAsrtPref;   /* Assert preference */
  L7_BOOL              amIAssertWinner;     /* Am I assert winner */
  L7_BOOL              couldAssert;       /* could assert flag for the interface */
}pimdmAssertInfo_t;

/*********************************************************************
*                 Downstream Interface State Info
*********************************************************************/
typedef struct pimdmDownstrmIntfInfo_s
{
  L7_uint32            rtrIfNum;

  /* Prune State info */
  PIMDM_DNSTRM_STATE_t pruneState;
  L7_APP_TMR_HNDL_t    prunePndTimer; /* Prune Pending Timer, PPT(S,G,I) */
  L7_APP_TMR_HNDL_t    pruneTimer;    /* Prune Timer, PT(S,G,I) */
  L7_uint32            maxPruneHoldTime;  /* The max prune hold time */ 

  /* Assert State info */
  pimdmAssertInfo_t    downstrmAssertInfo; 

  /*Timer handle */
  L7_uint32            mrtEntryDnstrmTmrHndlParam; /* Timer Handle Param for the MRT Entry's Downstream Timers */

  pimdmStRfrMsg_t      stRfrMsg; /* Storage for the recently relayed State Refresh
                                  * message that will be used to replay when the
                                  * Gen-Id of the Neighbor on the Downstream
                                  * interface is changed.
                                  */
  struct pimdmMrtEntry_s  *mrtEntry;               /*Back pointer */
}pimdmDownstrmIntfInfo_t;

typedef struct pimdmRPFInfo_s 
{
  L7_uint32        rpfRtrIfNum;
  L7_inet_addr_t   rpfNextHop;
  L7_uint32        rpfMaskLen;
  L7_RTO_PROTOCOL_INDICES_t rpfRouteProto;
  L7_uint32        rpfRouteMetricPref;
  L7_uint32        rpfRouteMetric;
} pimdmRPFInfo_t;

/*********************************************************************
*                    PIM-DM MRT (S,G) Entry
*********************************************************************/
typedef struct pimdmMrtEntry_s
{
  L7_inet_addr_t srcAddr; /* Index for the MRT Entry*/
  L7_inet_addr_t grpAddr; /* Index for the MRT Entry*/
  L7_uint32      upstrmRtrIfNum; /* Upstream Router interface index */

  /* Upstream Interface State Information */
  pimdmGraftPruneState_t  upstrmGraftPruneStateInfo;

  /* Originator State Information */
  pimdmOrigState_t        origStateInfo;

  /* RPF & Upstream Neighbor Information */
  pimdmRPFInfo_t          rpfInfo;
  pimdmAssertInfo_t       upstrmNbrInfo;
  L7_BOOL                 sourceDirectlyConnected;   /* Is Source directly connected */

  /* Downstream Interface State Information */
  pimdmDownstrmIntfInfo_t *downstrmStateInfo[PIMDM_MAX_INTERFACES];

  /* OIF List related info */
  interface_bitset_t oifList;           /* outgoing interface list bitmask */
  interface_bitset_t immediateOifList;  /* immediate outgoing interface list bitmask */
  interface_bitset_t downstrmPruneMask; /* downstream prune bitmask */
  interface_bitset_t lostAssertMask;    /* downstream lost assert bitmask */
  interface_bitset_t boundaryGMask;     /* Adminscope Boundary bitmask for a Group */
  interface_bitset_t pimIncludeStarG;   /* Include <*,G> local membership bitmap */
  interface_bitset_t pimIncludeSG;      /* Include <S,G> local membership bitmap */
  interface_bitset_t pimExcludeSG;      /* Exclude <S,G> local membership bitmap */

  /* Entry's Timer/Handle related Information */
  L7_uint32         mrtEntryUpstrmTmrHndlParam; /* Timer Handle Param for the MRT Entry's Upstream Timers */
  L7_APP_TMR_HNDL_t mrtEntryExpiryTimer;        /* MRT Entry Expiry Timer */
  L7_uint32         mrtEntryExpiryTimerHandle;  /* Timer Handle for the MRT Entry's Expiry Timer */
  L7_uint32         entryUpTime;                /* Entry's Create Time */

  /* Additional Information */
  pimdmCB_t *pimdmCB; /* Pointer to the PIM-DM Control Block. */

  void *avlData; /* AVL TREE requires this as last */
}pimdmMrtEntry_t;

/*********************************************************************
*                 State Refresh Message Forward Type
*********************************************************************/
typedef enum
{
  STRFR_MSG_ORIGINATOR = 0,
  STRFR_MSG_FORWARDER,
  STRFR_MSG_FORWD_TYPE_MAX
}PIMDM_STRFR_MSG_FORWD_TYPE_t;

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t pimdmMrtSGTreeInit (pimdmCB_t *pimdmCB);

extern L7_RC_t pimdmMrtSGTreeDeInit (pimdmCB_t *pimdmCB);

extern pimdmMrtEntry_t*pimdmMrtEntryGet(pimdmCB_t *pimdmCB,
                                        L7_inet_addr_t *source,
                                        L7_inet_addr_t *group);

extern pimdmMrtEntry_t *pimdmMrtEntryNextGet(pimdmCB_t *pimdmCB,
                                             L7_inet_addr_t *source,
                                             L7_inet_addr_t *group);

extern L7_RC_t pimdmMrtEntryRemove(pimdmCB_t *pimdmCB,
                                   pimdmMrtEntry_t *mrtEntry);

extern L7_RC_t pimdmMrtOifListCompute(pimdmCB_t *pimdmCB,
                                      L7_inet_addr_t *source,
                                      L7_inet_addr_t *group,
                                      PIMDM_MRT_CHANGE_TYPE_t type);
                                        
extern L7_RC_t pimdmMrtIntfDownUpdate (pimdmCB_t *pimdmCB,
                                       L7_uint32         rtrIfNum);

extern L7_RC_t pimdmMrtIntfUpUpdate (pimdmCB_t *pimdmCB,
                                       L7_uint32         rtrIfNum);

L7_RC_t
pimdmMrtEntryControlPacketSend(pimdmMrtEntry_t *mrtEntry,
                               PIMDM_CTRL_PKT_TYPE_t pimPktType,
                               L7_inet_addr_t  *upstrmNbrAddr,
                               L7_inet_addr_t  *destAddr,
                               L7_uint32       rtrIfNum,
                               L7_uint32       holdTime,
                               L7_uint32       stRfrTTL,
                               PIMDM_STRFR_MSG_FORWD_TYPE_t msgForwdFlag,
                               L7_BOOL upstrmPruneIndFlag);
       
extern L7_inet_addr_t*  pimdmMrtRpfNbrEval(pimdmMrtEntry_t *entry);

extern void  pimdmMrtDnstrmPruneMaskEval (pimdmMrtEntry_t *entry);

extern L7_BOOL pimdmMrtAsrtMetricCompare(L7_uint32 localPreference, 
                                         L7_uint32 localMetric,
                                         L7_inet_addr_t *localIp,
                                         L7_uint32 remotePreference, 
                                         L7_uint32 remoteMetric,
                                         L7_inet_addr_t *remoteIp);

extern void  pimdmMrtDnstrmAssertMaskEval (pimdmMrtEntry_t *entry, 
                                           L7_uint32 rtrIfNum, L7_BOOL bFlag);

extern L7_RC_t pimdmMrtMfcNoCacheEventProcess (pimdmCB_t      *pimdmCB,
                                               L7_inet_addr_t *source,
                                               L7_inet_addr_t *group,
                                               L7_uint32      rtrIfNum,
                                               L7_uint32      ttl);

extern L7_RC_t pimdmMrtMfcExpiryEventProcess(pimdmCB_t      *pimdmCB,
                                             L7_inet_addr_t *source,
                                             L7_inet_addr_t *group);

extern L7_RC_t pimdmMrtMfcWrongIfEventProcess (pimdmCB_t      *pimdmCB,
                                               L7_inet_addr_t *source,
                                               L7_inet_addr_t *group,
                                               L7_uint32      rtrIfNum);


extern L7_RC_t pimdmMrtRTOBestRouteChangeProcess (pimdmCB_t *pimdmCB,
                                                  mcastRPFInfo_t *routeInfo);

extern L7_RC_t
pimdmMrtMaxPruneHoldTimeUpdate (pimdmMrtEntry_t *mrtEntry, L7_uint32 rtrIfNum,
                                L7_uint32 holdTime);

extern L7_RC_t
pimdmMrtNLTExpiryProcess (pimdmCB_t *pimdmCB, L7_inet_addr_t *nbrAddr,
                          L7_uint32 rtrIfNum,L7_uint32 nbrCnt);

extern L7_RC_t
pimdmMrtUpstreamAssertProcess (pimdmMrtEntry_t *mrtEntry,
                      L7_inet_addr_t *asrtNbrAddr, L7_uint32 rtrIfNum,
                      L7_uint32 asrtMetricPref, L7_uint32 asrtMetric);

extern void
pimdmMrtTableInfoShow (pimdmCB_t *pimdmCB, L7_BOOL inBrief,L7_uint32 count);

void
pimdmMrtTableSrcGrpInfoShow (pimdmCB_t *pimdmCB,
                             L7_inet_addr_t *srcAddr,
                             L7_inet_addr_t *grpAddr,
                             L7_BOOL inBrief);

extern pimdmMrtEntry_t* pimdmMrtEntryAdd(pimdmCB_t *pimdmCB,
                                         L7_inet_addr_t *source,
                                         L7_inet_addr_t *group,
                                         L7_uint32      rtrIfNum);
extern L7_RC_t
pimdmMrtDnstrmIntfInit (pimdmMrtEntry_t *mrtEntry,
                        L7_uint32 rtrIfNum);
extern L7_RC_t
pimdmMrtDnstrmIntfDeInit (pimdmMrtEntry_t *mrtEntry,
                          L7_uint32 rtrIfNum,
                          L7_BOOL forceFree);
extern L7_RC_t pimdmNbrAddMrtOifListCompute(pimdmCB_t *pimdmCB,
                                            L7_uint32 rtrIfNum);
extern L7_RC_t  pimdmMrtEntryAdminScopeUpdate (pimdmCB_t *pimdmCB,
                                        L7_inet_addr_t *grpAddr,
                                        L7_inet_addr_t *grpMask);
extern L7_RC_t
pimdmMrtAdminDownAction (pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmMrtNbrGenIDChangeProcess (pimdmCB_t *pimdmCB,
                               L7_uint32 rtrIfNum,
                               L7_inet_addr_t *nbrAddr);

#endif  /* _PIMDM_MRT_H */
