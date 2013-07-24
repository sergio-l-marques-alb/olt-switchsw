/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pimsmMain.h
*
* @purpose   Common Macros and Control Block Structs.
*
* @component 
*
* @comments  none
*
* @create   01/01/2006
*
* @author   dsatyanarayana
*
* @end
*
**********************************************************************/
#ifndef _PIMSM_MAIN_H_
#define _PIMSM_MAIN_H_
#include "l3_addrdefs.h"
#include "l7_mcast_api.h"
#include "l7_mgmd_api.h"
#include "l7sll_api.h"
#include "pimsmuictrl.h"
#include "mcast_defs.h"
#include "mfc_api.h"
#include "pim_defs.h"
#include "mcast_api.h"
#include "pimsmdefs.h"
#include "l7apptimer_api.h"
#include "pimsmintf.h"
#include "avl_api.h"
#include "rto_api.h"
#include "l7handle_api.h"
#include "mcast_inet.h"

#define PIMSM_GLOBAL_NUM_TIMERS 1
#define PIMSM_DEFAULT_CHECK_INTERVAL     20 /* datarate/regrate check interval    */
#define PIMSM_DEFAULT_JP_BUNDLE_TIME     2 
struct pimsmStarGNode_s;
struct pimsmSGNode_s;
struct pimsmSGRptNode_s;
struct pimsmRpBlock_s;
struct pimsmBSRBlock_s;
struct pimsmTimerData_s;

#define PIMSM_ALLOC(family,size) \
        heapAlloc(mcastMapHeapIdGet(family),(size),__FILE__,__LINE__)
#define PIMSM_FREE(family,pMem) \
        heapFree(mcastMapHeapIdGet(family),(pMem))

typedef struct pimsmCB_s
{
  struct pimsmBSRBlock_s   *pimsmBsrInfo; /* BSR info. */
  struct pimsmRpBlock_s    *pimsmRpInfo;  /* node type is pimsm_rp_block_t   */ 

  L7_APP_TMR_HNDL_t          pimsmSPTTimer;
  L7_uint32          pimsmSPTTimerHandle;
  struct pimsmTimerData_s        pimsmSPTTimerParam; 
  L7_APP_TMR_HNDL_t          pimsmJPBundleTimer;
  L7_uint32          pimsmJPBundleTimerHandle;    
  struct pimsmTimerData_s           pimsmJPBundleTimerParam; 
  L7_sll_t             pimsmStarStarRpList;
  /* Semaphore to lock StarStarRpList*/
  void       *pimsmStarStarRpListLock;
  avlTreeTables_t        *pimsmStarGTreeHeap; /* space for tree nodes */
  struct pimsmStarGNode_s    *pimsmStarGDataHeap; /* space for data */
  avlTree_t            pimsmStarGTree;
  avlTreeTables_t        *pimsmSGTreeHeap;    /* space for tree nodes */
  struct pimsmSGNode_s     *pimsmSGDataHeap;     /* space for data */
  avlTree_t            pimsmSGTree;
  avlTreeTables_t          *pimsmSGRptTreeHeap;   /* space for tree nodes */
  struct pimsmSGRptNode_s    *pimsmSGRptDataHeap;  /* space for data */
  avlTree_t          pimsmSGRptTree;   
  pimsmInterfaceEntry_t     intfList[MCAST_MAX_INTERFACES];
  L7_uchar8 family;
  L7_uint32                    sockFd;
  L7_inet_addr_t         allPimRoutersGrpAddr;
  L7_uint32 pimsmDataRateBytes;
  L7_uint32 pimsmRegRateBytes;
  L7_uint32 pimsmRateCheckInterval;
  L7_BOOL   isPimsmEnabled;

  L7_BOOL   pimsmRestartInProgress;       /* indicates PIM-SM should apply restart processing behavior */ 

  L7_uchar8 pktRxBuf[PIMSM_PKT_SIZE_MAX]; /* Buffer used to Receive PIM-SM Packets */
  L7_uchar8 pktTxBuf[PIMSM_PKT_SIZE_MAX]; /* Buffer used to Transmit PIM-SM Packets */
  L7_uchar8 pseudoIpv6Hdr[PIMSM_PKT_SIZE_MAX];
  L7_uint32 pimsmIpv6MsgBuffer;
  L7_uint32 pimsmNbrBufferPoolId;  
  L7_uint32 pimsmCacheBufferId;
  void  *pimsmRouteChangeBuf;
  L7_APP_TMR_CTRL_BLK_t      timerCb;/*Timer Node */
  L7_uint32                  maxNumOfTimers;
  handle_list_t              *handleList;  
  L7_APP_TMR_HNDL_t          rtoTimer;
  void                       *rtoTimerSemId; /* Lock for RTO Timer as it will
                                              * be accessed both in RTO and
                                              * PIMSM-MAP Task contexts.
                                              */
  void                       *pimsmNbrLock; /* single lock for all neighbors on all interfaces */
  L7_uint32               pimsmStarStarRpNextHopUpdateCnt;
  L7_uint32               pimsmSGNextHopUpdateCnt;
  L7_uint32               pimsmStarGNextHopUpdateCnt;
  L7_uint32               pimsmSGRptNextHopUpdateCnt;

} pimsmCB_t;

/*------------------------------------------------*/
/*            Function Prototypes                 */
/*------------------------------------------------*/
#endif /*_PIMSM_MAIN_H_*/

