/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ipmap_arp.h
* @purpose     ARP data structures and defines for internal component use
* @component   IP Mapping Layer -- ARP
* @comments    none
* @create      03/21/2001
* @author      gpaussa
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef _IPMAP_ARP_H_
#define _IPMAP_ARP_H_


/* header includes */
#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"
#include "ip_util.h"
#include "rto_api.h"
#include "l7_ipmap_arp_api.h"
#include "ipmap_arp_exten.h"
#include "avl_api.h"
#include "l7_socket.h"

/* Routing headers */
#include "std.h"
#include "frame.ext"
#include "arp.h"
#include "arp.ext"

/* NOTE:  must place this after std.h due to E_OK name conflict */
#include <log.h>

/* IPM ARP constants */
#define IPM_ARP_INTF_MAX                (L7_RTR_MAX_RTR_INTERFACES +1)
#define IPM_ARP_ENET_FRAME_LEN_MIN      14
#define IPM_ARP_ENET_FRAME_LEN_MAX      64
#define IPM_ARP_PKT_LEN                 sizeof(ipMapArpPkt_t)
#define IPM_ARP_GATEWAYS_MAX            ((L7_uint32)L7_L3_ROUTE_TBL_SIZE_TOTAL)
#define IPM_ARP_GW_REFCNT_START         1       /* always start at 1 */

/* ARP Reissue Task definitions */
#define IPM_ARP_REISSUE_TASK_NAME       "tArpReissue"
#define IPM_ARP_REISSUE_MSG_Q_NAME      "arpReissueQueue"
#define IPM_ARP_REISSUE_DEV_FAIL_MAX    (3 +1)  /* the +1 is for the initial add attempt */

/* ARP Callback Task definitions */
#define IPM_ARP_CALLBACK_TASK_NAME      "tArpCallback"
#define IPM_ARP_CALLBACK_MSG_Q_NAME     "arpCallbackQueue"

/* ARP TimerExpire Task definitions */
#define IPM_ARP_TIMEREXP_TASK_NAME      "tArpTimerExp"
#define IPM_ARP_TIMEREXP_MSG_Q_NAME     "arpTimerExpQueue"

/* ARP Table change actions */
typedef enum
{
  IPM_ARP_TABLE_INSERT = 1,
  IPM_ARP_TABLE_REISSUE,
  IPM_ARP_TABLE_UPDATE,
  IPM_ARP_TABLE_REMOVE,
  IPM_ARP_TABLE_NO_ACTION               /* used internally (not a valid event)*/
} ipMapArpTableEvent_t;

/* Debug message levels */
#define IPM_ARP_MSGLVL_ON     0         /* use where printf is desired */
#define IPM_ARP_MSGLVL_HI     1
#define IPM_ARP_MSGLVL_MED    50
#define IPM_ARP_MSGLVL_LO     100
#define IPM_ARP_MSGLVL_LO_2   200
#define IPM_ARP_MSGLVL_OFF    10000     /* some arbitrarily large value */

/* Indices for ARP age out message queues. 
 *Timer Events go in the event queue. */
typedef enum e_ipMapArpQueueType
{
  IPM_ARP_EXP_EVENT_QUEUE  = 0,
  IPM_ARP_NUM_QUEUES
} e_ipMapArpQueueType;

/* ARP event trace buffer definition (local to this file) */
#define IPM_ARP_TRACE_MAX     64
typedef struct
{
  L7_uint32     event;
  L7_uint32     deviceEvent;
  L7_uint32     ipAddr;
  L7_uint32     intIfNum;
  L7_uint32     flags;
  L7_BOOL       inDeviceBefore;
  L7_BOOL       inDeviceAfter;
  L7_uint32     failedDevAdds;
  L7_uint32     timestamp;              /* filled in by traceWrite */
} ipMapArpTrace_t;


/* Definition of an ARP packet (layer 3 portion) */
typedef struct
{
  L7_uchar8     hwType[2];                      /* hardware type */
  L7_uchar8     protType[2];                    /* protocol type */
  L7_uchar8     hwLen;                          /* hardware address length */
  L7_uchar8     protLen;                        /* protocol address length */
  L7_uchar8     opCode[2];                      /* operation */
  L7_uchar8     srcMac[L7_MAC_ADDR_LEN];        /* source MAC address */
  L7_uchar8     srcIp[L7_IP_ADDR_LEN];          /* source IP address */
  L7_uchar8     dstMac[L7_MAC_ADDR_LEN];        /* target MAC address */
  L7_uchar8     dstIp[L7_IP_ADDR_LEN];          /* target IP address */
} ipMapArpPkt_t;


/* IP Mapping layer ARP local routing interface attributes */
typedef struct ipMapArpIntf_s
{
  void          *next;                  /* RESERVED: list chain pointer */
  void          *prev;                  /* RESERVED: list chain pointer */

  L7_uint32     index;                  /* assigned interface index number */
  L7_uint32     intIfNum;               /* internal interface number */
  t_Lay3Addr    l3Addr;                 /* layer 3 addr of router interface */
  L7_uint32     ipAddr;                 /* IP addr of router interface */
  L7_uint32     netMask;                /* subnet mask */
  L7_BOOL       unnumbered;             /* unnumbered intf flag */
  L7_BOOL       proxyArp;               /* L7_TRUE if proxy ARP enabled on intf */
  L7_BOOL       localProxyArp;          /* L7_TRUE if local proxy ARP enabled */

  /* Secondary IP addresses on the interface. Only includes those
   * secondaries that have been registered with ARP. Note that this
   * array does NOT contain the primary IP address and that the indices
   * may not match the indices in IP MAP's addrs[] array. The array may
   * have holes. When iterating, can't stop at first 0. */
  L7_rtrIntfIpAddr_t  secondaries[L7_L3_NUM_SECONDARIES];

  L7_uint32     netId;                  /* network number (ipAddr & netmask) */
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN]; /* L2 MAC addr of router interface */
  ipmRawSend_ft rawSendFn;              /* address of intf raw send function */
  L7_uint32     sendBufSize;            /* size of send buffers being used */
  L7_uint32     intfHandle;             /* routing interface handle */
} ipMapArpIntf_t;


/* Holds the ARP resolution callback parameters provided by
 * the original requestor for the case where the response is
 * asynchronous
 */
typedef struct ipMapArpResCallbackParms_t
{
  ipMapArpResCallback_ft  pCallbackFn;    /* callback function */
  L7_uint32     cbParm1;                /* callback parm 1 (e.g., pEndObj) */
  L7_uint32     cbParm2;                /* callback parm 2 (e.g., pMblk) */
  L7_uchar8     *pMacBuffer;            /* buffer ptr for storing MAC addr */
  L7_RC_t       asyncRc;                /* indicates if async was successful */
} ipMapArpResCallbackParms_t;


/* IP Mapping layer node definition for gateway table */
typedef struct
{
  /* NOTE:  The search key MUST be the first item in this structure */
  L7_uint32       ipAddr;               /* SEARCH KEY: gateway IP address */
  L7_uint32       intIfNum;             /* SEARCH KEY: Internal Interface no */
  L7_uint32       refCnt;               /* route reference count */
  L7_BOOL         holdover;             /* deleted ARP entry retained in FIB */
  L7_BOOL         staticRouteGw;        /* if this is gateway of a static route */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the last element */
  void            *next;                /* (RESERVED FOR AVL USE) */
} ipMapArpGwNode_t;

/* IP Mapping layer ARP aging list anchor */
typedef struct ipMapArpAgeList_s
{
  struct ipMapArpIpNode_s   *head;      /* oldest entry */
  struct ipMapArpIpNode_s   *tail;      /* newest entry */
} ipMapArpAgeList_t;

/* IP Mapping layer ARP aging control struct */
typedef struct ipMapArpAgeCtrl_s
{
  struct ipMapArpIpNode_s   *prev;      /* previous entry chain ptr (oldest is first in list) */
  struct ipMapArpIpNode_s   *next;      /* next entry chain ptr */
} ipMapArpAgeCtrl_t;

/* IP Mapping layer ARP Entry node definition */
typedef struct ipMapArpIpNode_s
{
  /* NOTE:  The search key MUST be the first item in this structure */
  L7_uint32       key;                  /* SEARCH KEY: IP address */
  L7_uint32       intIfNum;             /* SEARCH KEY: Internal Interface no */
  ipMapArpAgeCtrl_t ageCtrl;            /* entry aging control struct */
  L7_BOOL         inDevice;             /* ARP entry has been stored in device table */
  L7_uint32       failedDevAdds;        /* count of unsuccessful device add attempts */
  t_ADR           arpEntry;             /* ARP entry contents (vendor defined)*/
  /* NOTE:  The AVL utility REQUIRES a next ptr as the last element */
  void            *next;                /* (RESERVED FOR AVL USE) */
} ipMapArpIpNode_t;

/* IP Mapping layer ARP lock control info */
#define ARP_LOCK_OWNER_LEN 20
typedef struct ipMapArpLockCtrl_s
{
  void            *semId;               /* semaphore id */
  L7_int32        ownerTaskId;          /* current lock owner task */
  L7_int32        ownerLockCount;       /* nested lock count for current owner (signed value) */
  L7_uint32       lockTotal;            /* total number of times locked        */
  L7_uint32       unlockTotal;          /* total number of times unlocked      */

  /* additional debug */
  L7_char8        ownerFile[ARP_LOCK_OWNER_LEN+1];
  L7_uint32       ownerLine;
} ipMapArpLockCtrl_t;


/* ARP Reissue Msg Queue message definition */
typedef struct
{
  L7_uint32     ipAddr;                 /* IP address to re-issue to device */
  L7_uint32     intIfNum;               /* Internal interface number */
} ipMapArpReissueMsg_t;

/*  ARP Map timer expire queue message IDs */
typedef enum
{
  IPM_ARP_TIMEOUT = 1,                  /* ARP timeout */
  IPM_ARP_AGETIME,                      /* ARP entry aged out */
  IPM_ARP_TIMER_LAST_MSG
} ipMapArpTimerExpMsgId_t;

/*  ARP Map callback queue message IDs */
typedef enum
{
  IPM_ARP_SEND = 1,                     /* ARP send */
  IPM_ARP_RES,                          /* ARP resolution */
  IPM_ARP_LAST_MSG
} ipMapArpCbkMsgId_t;

typedef struct
{
  ipmRawSend_ft     pRawSendFn;         /* address of intf raw send function */
  L7_uint32         intIfNum;           /* internal interface num to send the packet */
  L7_uint32         arpFrameSize;       /* size of ARP frame to send */
  L7_uchar8         arpFrame[IPM_ARP_ENET_FRAME_LEN_MAX];  /* copy of ARP frame to send */
} ipMapArpCbkSendMsg_t;

typedef struct
{
  ipMapArpResCallback_ft  pCallbackFn;  /* callback function ptr */
  L7_uint32         cbParm1;            /* callback parm 1 */
  L7_uint32         cbParm2;            /* callback parm 2 */
  L7_uchar8         macAddr[L7_MAC_ADDR_LEN]; /* ARP entry MAC addr */
  L7_RC_t           asyncRc;            /* asynchronous return code */
} ipMapArpCbkResMsg_t;

typedef struct
{
  ipMapArpCbkMsgId_t          id;       /* message type identifier */
  union
  {
    ipMapArpCbkSendMsg_t      send;     /* ARP send callback msg */
    ipMapArpCbkResMsg_t       res;      /* ARP resolution callback msg */
  } u;
} ipMapArpCbkMsg_t;

typedef struct
{
  ipMapArpTimerExpMsgId_t   id;       /* message type identifier */

  void                      *adrId;
  void                      *timer;
  L7_ushort16               flags;

} ipMapArpTimerExpMsg_t;

typedef struct
{
  L7_int32        taskId;               /* system task identifier */
  void            *pMsgQ;               /* task msg queue ptr */
  L7_uint32       totalCt;              /* total msgs processed by task */
  L7_uint32       peakCt;               /* peak msgs processed per task iteration */
  L7_uint32       maxCt;                /* max msgs avail in task msg queue */
  L7_uint32       fullQCt;              /* task msg queue full (when sending) */
  L7_uint32       failCt;               /* general failure (task-specific meaning) */
} ipMapArpTaskInfo_t;

/*
 * Maximum number of registrations for the ARP Cache
 */
#define IPM_ARP_CACHE_REG_MAX 10

/* IP Mapping layer context structure for ARP operation */
typedef struct
{
  t_Handle        arpHandle;            /* L3 ARP Mgr object instance handle */
  t_Handle        regUserId;            /* ARP user registration ID */
  t_Handle        pIntfList;            /* anchor for list of ipMapArpIntf_t */
  t_Handle        gwArpRqstTimer;       /* Timer to send repititive ARP requests
                                         * for GW entries, to avoid GW MAC entries from
                                         * aging out in the h/w L2 table */
  ipMapArpTaskInfo_t  reissue;          /* ARP Reissue task info */
  ipMapArpTaskInfo_t  callback;         /* ARP Callback task info */
  ipMapArpTaskInfo_t  timerExp;         /* ARP TimerExp task info */
  L7_uint32       msgLvl;               /* debug message control */
  L7_uint32       cacheCurrCt;          /* current num ARP cache entries*/
  L7_uint32       cachePeakCt;          /* peak num ARP cache entries */
  L7_uint32       cacheMaxCt;           /* maximum allowed ARP cache entries */
  L7_uint32       staticCurrCt;         /* current num static ARP cache entries */
  L7_uint32       staticMaxCt;          /* maximum allowed static ARP cache entries */
  L7_uint32       inSrcDiscard;         /* total ARPs discarded due to bad src IP*/
  L7_uint32       inTgtDiscard;         /* total ARPs discarded because of bad target IP */
  L7_uint32       arpQueryTaskId;       /* ID of ARP Query task */


  /* Gateway Table AVL tree support */
  struct
  {
    L7_uint32           treeEntryMax;
    avlTreeTables_t     *treeHeap;
    L7_uint32           treeHeapSize;
    ipMapArpGwNode_t    *dataHeap;
    L7_uint32           dataHeapSize;
    avlTree_t           treeData;
  } gwTbl;

  /* ARP IP Lookup AVL tree support */
  struct
  {
    L7_uint32           treeEntryMax;
    avlTreeTables_t     *treeHeap;
    L7_uint32           treeHeapSize;
    ipMapArpIpNode_t    *dataHeap;
    L7_uint32           dataHeapSize;
    avlTree_t           treeData;
  } ipTbl;

  /* ARP component lock */
  ipMapArpLockCtrl_t  arpLock;

  /* ARP Cache registrations */
  ipMapArpCacheCallback_ft arpCacheCallback[IPM_ARP_CACHE_REG_MAX];

} ipMapArpCtx_t;


/* DEBUG build only: general printf-style macro for debug messages */
#define IPM_ARP_PRT(mlvl, fmt, args...) \
          { \
          if (ipMapArpCtx_g.msgLvl >= (mlvl)) \
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, fmt, ##args); \
          }

/* Semaphore take/give macros
 *
 * NOTE: DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
 *       where awareness of the time expiration is needed.  Make a direct
 *       call to osapiSemaTake() instead.
 */
#define IPM_ARP_SEMA_TAKE(_lc, _t) \
  ipMapArpSemaTake((_lc), (_t), __FILE__, __LINE__)

#define IPM_ARP_SEMA_GIVE(_lc) \
  ipMapArpSemaGive((_lc), __FILE__, __LINE__)


/* ARP Lock Debug Support */
void arpLogLockAction(L7_char8 * fname, L7_char8 * file, L7_uint32 line, L7_uint32 lockid);
void arpLockShowStack(L7_char8 * fname, L7_uint32 lockid, L7_char8 * file, L7_uint32 line);
extern L7_int32  arpLockDebugTimeout;

#define ARP_LOCK_DEBUG_LOG(file, line, lockid)                   \
  {                                                                     \
    if(arpLockDebugTimeout != L7_WAIT_FOREVER)                          \
      arpLogLockAction((L7_char8 *)__FUNCTION__, file, line, (L7_uint32)(lockid));  \
  }

#define ARP_LOCK_DEBUG_DUMP(lockid, file, line)                         \
  {                                                                     \
    if(arpLockDebugTimeout != L7_WAIT_FOREVER)                          \
      arpLockShowStack((L7_char8 *)__FUNCTION__, (L7_uint32)(lockid), file, line);  \
  }


/* ARP Debug Trace Structure */
#define ARP_DEBUG_CFG_FILENAME "arp_debug.cfg"
#define ARP_DEBUG_CFG_VER_1          0x1
#define ARP_DEBUG_CFG_VER_CURRENT    ARP_DEBUG_CFG_VER_1

typedef struct arpDebugCfgData_s
{
  L7_BOOL arpDebugPacketTraceFlag;
} arpDebugCfgData_t;

typedef struct arpDebugCfg_s
{
  L7_fileHdr_t          hdr;
  arpDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} arpDebugCfg_t;





/*------------------------------------------------*/
/* Internal IPM ARP component function prototypes */
/*------------------------------------------------*/

/*-----------*/
/* ipm_arp.c */
/*-----------*/

/*********************************************************************
* @purpose  Initializes IPM ARP context structure
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapArpCtxInit(void);

/*********************************************************************
* @purpose  Initialize ARP task
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpStartTasks(void);

/*********************************************************************
* @purpose  Kill ARP task
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpDeleteTasks(void);

/*********************************************************************
* @purpose  Start up the ARP code
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Calls routingInit() to initialize the routing subsystem.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpL7Start(void);

/*********************************************************************
* @purpose  Common send callback function used by vendor ARP code for
*           address resolution
*
* @param    userId      ptr to interface instance structure
* @param    *pFrame     ptr to Ethernet frame to be transmitted
* @param    flags       contains the interface index number to send the frame
* @param    reservHandle  resource reservation handle for packet data flow
*
* @returns  E_OK
* @returns  E_xxxxxx    anything other than E_OK (see std.h for list)
*
* @notes    This function must copy the frame contents from the pFrame
*           format to an Mblk format used by the routing END object.
*
* @end
*********************************************************************/
e_Err ipMapArpSendCallback(t_Handle userId, void *pFrame, word flags,
                           t_Handle reservHandle);

/*********************************************************************
* @purpose  Common address resolution callback function used by vendor ARP code
*
* @param    userId      pointer to address resolution callback parameter
*                         block (contains original requestor's callback
*                         information)
* @param    arpInfo     ARP resolution response information (i.e., MAC addr)
*
* @returns  E_OK
*
* @notes    Invokes original requestor's callback function with its
*           own callback parameters.  This information was saved in a
*           context block that was allocated from the heap and is pointed
*           to by the userId parm.
*
* @notes    Assumes an unsuccessful resolution attempt is denoted by a
*           dllHeaderSize of 0 in the arpInfo structure.
*
* @end
*********************************************************************/
e_Err ipMapArpResCallbackInvoke(t_Handle userId, t_ARPResInfo *arpInfo);

/*********************************************************************
* @purpose  Apply ARP IP Table events to the device ARP table
*
* @param    *pIp        ARP IP entry node ptr
* @param    event       ARP table event (e.g., insert, update, remove)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes caller maintains proper ARP IP table semaphore control.
*
* @notes    This function occurs in conjunction with the ARP table operation
*           and not as part of a callback.  This means the ARP entry node
*           info is intact, even in the case of an entry remove.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpDeviceTableApply(ipMapArpIpNode_t *pIp,
                                 ipMapArpTableEvent_t event);

/*********************************************************************
* @purpose  Common function to send message to ARP Reissue task queue
*
* @param    *msg        ptr to ARP Reissue task message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapArpReissueTaskMsgSend(ipMapArpReissueMsg_t *msg);

/*********************************************************************
* @purpose  ARP entry reissue task
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Tries to reissue a device ARP add for entries that had
*           previously failed during the DTL call.
*
* @end
*********************************************************************/
void ipMapArpReissueTask(void);

/*********************************************************************
* @purpose  ARP Callback Task
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Handles work deferred to the ARP callback task.
*
* @end
*********************************************************************/
void ipMapArpCallbackTask(void);

/*********************************************************************
* @purpose  Queries the ARP entry from the hardware
*
* @param    pArp @b{(input)} ARP Query Entry
*
* @returns  L7_SUCCESS  on successful query of arp entry
* @returns  L7_FAILURE  if the query fails
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryQuery(L7_arpQuery_t *pArp);

/*********************************************************************
* @purpose  Determines if an aged-out ARP entry should be renewed
*
* @param    ipAddr      IP address of ARP entry being evaluated
* @param    intIfNum    Internal interface number for entry
*
* @returns  L7_TRUE     keep ARP entry in the ARP table
* @returns  L7_FALSE    allow ARP entry to expire
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ipMapArpEntryKeep(L7_uint32 ipAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Fills in an ARP entry structure in a format expected by DTL
*
* @param    pArpEntry   pointer to ARP entry structure to be filled in
* @param    pIpAddr     pointer to IP address
* @param    pMacAddr    pointer to Ethernet MAC address
* @param    vlanId      VLAN associated with the routing circuit
* @param    intIfNum    internal interface number for the routing circuit
*
* @returns  void
*
* @notes    Only a generic flags value is setup here.  All ARP entries
*           are assumed to be resolved here.  The caller must set any
*           additional flags, as needed.
*
* @end
*********************************************************************/
void ipMapArpDtlEntryBuild(L7_arpEntry_t *pArpEntry,
                           L7_uint32 *pIpAddr, L7_uchar8 *pMacAddr,
                           L7_uint32 vlanId, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Build the ARP entry structure flags from the IP table entry info
*
* @param    pIp         @b{(Input)}  pointer to ARP IP table entry node
* @param    pFlags      @b{(Output)} pointer to flags output location
*
* @returns  void
*
* @comments The ARP table does not contain any net directed broadcast entries,
*           so that flag is never set here.
*
* @end
*********************************************************************/
void ipMapArpEntryFlagsBuild(ipMapArpIpNode_t *pIp, L7_uint32 *pFlags);

/*********************************************************************
* @purpose  Converts an internal interface number to an ARP interface
*           instance
*
* @param    intIfNum    internal interface number
* @param    *ppIntf     ptr to output location
*                         (@b{Output:} pointer to corresponding interface
*                          instance)
*
* @returns  L7_SUCCESS  found matching interface instance
* @returns  L7_FAILURE  interface instance not found
*
* @notes    An internal interface number of 0 is normally not a valid
*           value, but is used in certain cases when an interface-independent
*           ARP value is involved (such as certain multicast addresses).
*           For this reason, an internal interface number of 0 is translated
*           to the special interface instance 0.
*
* @notes    The *ppIntf output location is set to L7_NULL for a failure,
*           indicating a matching interface instance was not found.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIntfInstanceGet(L7_uint32 intIfNum, ipMapArpIntf_t **ppIntf);

/*********************************************************************
* @purpose  Find the ARP interface configured with an IP subnet 
*           that contains a given unicast IP address. 
*
* @param    ipAddr      IP address of interest
* @param    ppIntf      handle to ARP interface
* @param    localAddr   Local IP address in matching subnet
* @param    localMask   Network mask on matching subnet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Failure to find any matching interface instance results in
*           the *ppIntf output location being set to L7_NULL.
*
*           Since FASTPATH doesn't allow the operator to configure
*           different interfaces with the same or overlapping subnets,
*           there can only be one match.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpSubnetFind(L7_uint32 ipAddr, ipMapArpIntf_t **ppIntf,
                           L7_uint32 *localAddr, L7_uint32 *localMask);

/*********************************************************************
* @purpose  Searches all router interface instances for a subnet which
*           matches the IP address parameter
*
* @param    ipAddr      IP address of interest
* @param    *ppIntf     ptr to output location
*                         (@b{Output:} matching router interface instance
*                          number, or L7_NULL if not found)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Must handle multicast IP (e.g. 224.x.x.x) addresses as a
*           special case by outputting interface instance 0.  The ARP
*           mapping layer and DTL are designed to treat this as a "system"
*           interface.
*
* @notes    Failure to find any matching interface instance results in
*           the *ppIntf output location being set to L7_NULL.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpNetidSearch(L7_uint32 ipAddr, ipMapArpIntf_t **ppIntf);

/*********************************************************************
* @purpose  Checks if the specified IP address matches the net directed
*           broadcast address of one of the IP addresses configured
*           on the given interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddr      @b{(input)} IP address to check
* @param    pMacAddr    @b{(output)} Pointer to a buffer to hold the Net
*                       Directed Broadcast MAC Addresses if a match is
*                       found
*
* @returns  L7_SUCCESS  Match found
* @returns  L7_FAILURE  Could not find a match
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t  ipMapArpNetDirBcastMatchCheck(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                                       L7_uchar8 *pMacAddr);

/**************************************************************************
* @purpose  Dispatch function for ARP Cache updates
*
* @param    event     @{(input)} Type of event
* @param    pArpEntry @{(input)} ARP Entry
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapArpCacheEventNotify(ipMapArpCacheEvent_t event,
                              L7_arpEntry_t *pArpEntry);

/**************************************************************************
* @purpose  Take a semaphore and check return code
*
* @param    pLock   @{(input)} Lock control info ptr (references semaphore ID)
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  void
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiSemaTake() instead.
*
* @end
*************************************************************************/
void ipMapArpSemaTake(ipMapArpLockCtrl_t *pLock, L7_int32 timeout,
                      char *file, L7_ulong32 line);

/**************************************************************************
* @purpose  Give a semaphore and check return code
*
* @param    pLock   @{(input)} Lock control info ptr (references semaphore ID)
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  void
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
void ipMapArpSemaGive(ipMapArpLockCtrl_t *pLock, char *file, L7_ulong32 line);

/*********************************************************************
* @purpose  Checks if a MAC address value is all zeros
*
* @param    pMac        ptr to Ethernet MAC address
*
* @returns  L7_TRUE     MAC address is all zeros
* @returns  L7_FALSE    MAC address is non-zero
*
* @notes    Checks byte by byte to avoid any alignment restrictions
*           on certain CPUs.
*
* @end
*********************************************************************/
L7_BOOL ipMapArpIsMacZero(L7_uchar8 *pMac);

/*********************************************************************
* @purpose  Sets the level of IPM ARP debugging messages to display
*
* @param    msgLvl      message level (0 = off, >0 on)
*
* @returns  void
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @end
*********************************************************************/
void ipMapArpMsgLvlSet(L7_uint32 msgLvl);

/*********************************************************************
* @purpose  Records an ARP event entry in the ARP trace buffer
*
* @param    *pTrcData   ptr to trace point data
*
* @returns  void
*
* @notes    The ARP trace buffer is circular.  There is no semaphore
*           protection at present (can be added later, if needed).
*
* @end
*********************************************************************/
void ipMapArpTraceWrite(ipMapArpTrace_t *pTrcData);

/*********************************************************************
* @purpose  Displays the specified number of most recent ARP trace buffer
*           entries
*
* @param    amt         number of ARP trace buffer entries to display
*
* @returns  void
*
* @notes    Entering a 0 (or an out-of-range value) for the amt parameter
*           causes the entire ARP trace to be displayed.
*
* @end
*********************************************************************/
void ipMapArpTraceShow(L7_uint32 amt);

/*********************************************************************
* @purpose  Displays ARP lock information
*
* @param    void
*
* @returns  void
*
* @notes    Intended for engineering debug use only.
*
* @end
*********************************************************************/
void ipMapArpLockShow(void);

/*********************************************************************
* @purpose  Displays the global ARP context info
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipMapArpCtxShow(void);

/*********************************************************************
* @purpose  Dumps all ARP-related debug information
*
* @param    amt         number of entries to display (in certain items)
*
* @returns  void
*
* @notes    This function intended for debugging use only.
*
* @end
*********************************************************************/
void ipMapArpAllShow(L7_uint32 amt);

/*********************************************************************
* @purpose  Internal function that attempts to resolve an IP address 
*           to an Ethernet MAC address. Assumes IPM_ARP_SEMA_TAKE has 
*           been invoked and IPM_ARP_SEMA_GIVE will be invoked by the 
*           calling function.
*
* @param    intIfNum    internal interface number
* @param    ipAddr      layer 3 IP address to resolve
* @param    *pMacAddr   ptr to output buffer where MAC address is returned if 
*                       ipAddr is already in the ARP cache
* @param    pCallbackFn address of callback function for asynchronous response
* @param    cbParm1     callback parameter 1 
* @param    cbParm2     callback parameter 2 
*
* @returns  L7_SUCCESS          address was resolved, MAC addr returned via pMacAddr
* @returns  L7_ASYNCH_RESPONSE  resolution in progress. will notify asynchronously 
*                               through callback.
* @returns  L7_FAILURE          cannot resolve address
*
* @notes    An L7_ASYNCH_RESPONSE return code indicates the address was not found in
*           the ARP cache and is in the process of being resolved.  If the
*           pCallbackFn pointer is non-null, an asynchronous callback will be 
*           made indicating the outcome of the resolution attempt.  If the 
*           pCallbackFn is null, there will not be a notification should the 
*           address get resolved.
*
* @notes    An L7_FAILURE can occur due to temporary resource constraints
*           (such as a full queue), or other programming/usage errors.
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpAddrResolveInternal(L7_uint32 intIfNum, L7_uint32 ipAddr,
                            L7_uchar8 *pMacAddr, 
                            ipMapArpResCallback_ft pCallbackFn,
                            L7_uint32 cbParm1, L7_uint32 cbParm2);

/*-----------------*/
/* ipm_arp_table.c */
/*-----------------*/

/*********************************************************************
* @purpose  Create the gateway table using an AVL tree
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableCreate(void);

/*********************************************************************
* @purpose  Delete the gateway table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableDelete(void);

/*********************************************************************
* @purpose  Destroy the gateway table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableDestroy(void);

/*********************************************************************
* @purpose  Insert an entry into the gateway table
*
* @param    ipAddr   @b{(input)}   gateway IP address to add to table
* @param    intIfNum  @b{(input)}  Interface number for the entry
* @param    staticRouteGw @b{(input)}  if this is gateway of static route
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Attempts to insert a new gateway node into the gateway
*           table.  If the gateway already exists in the table, its
*           existing node is used instead.
*
* @notes    The route reference count is set to 1 for a new gateway
*           node and is incremented for an existing node.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableInsert(L7_uint32 ipAddr, L7_uint32 intIfNum, 
                              L7_BOOL staticRouteGw);

/*********************************************************************
* @purpose  Send an ARP request to each static entry in the ARP gw table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableStaticRenew(void);

/*********************************************************************
* @purpose  Remove an entry from the gateway table
*
* @param    ipAddr      gateway IP address to remove from table
* @param    intIfNum    internal interface number associated with
*                       ARP entry.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Checks if a particular IP address exists in the gateway
*           table.  If so, its route reference count is decremented.
*           When this count reaches zero, the node is removed from
*           the table.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableRemove(L7_uint32 ipAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Search the gateway table for a specific entry
*
* @param    ipAddr      {@b(Input)}  IP address search key
* @param    intIfNum    {@b(Input)}  Interface number search key
* @param    mode        {@b(Input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
* @param    **ppGw      {@b(Output)} ptr to GW table node pointer location
*
* @returns  L7_SUCCESS  found gateway IP address of interest
* @returns  L7_FAILURE  gateway IP address not found
*
* @notes    Based on the mode flag, can be used to find a specific gateway
*           IP address, or the next sequential address following the one
*           specified.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableSearch(L7_uint32 ipAddr, L7_uint32 intIfNum, 
        L7_uint32 mode, ipMapArpGwNode_t **ppGw);

/*********************************************************************
* @purpose  Display the current gateway table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapArpGwTableShow(void);

/*********************************************************************
* @purpose  Create the ARP IP lookup table using an AVL tree
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  This table replaces the vendor code ARP Table.  It is used
*           for organizing the IP addresses in an ascending search order
*           to provide proper API operation for the get/next functions.
*           It contains all valid ARP entry types (local, static, gateway,
*           and dynamic).
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableCreate(void);

/*********************************************************************
* @purpose  Delete the ARP IP lookup table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableDelete(void);

/*********************************************************************
* @purpose  Destroy the ARP IP lookup table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableDestroy(void);

/*********************************************************************
* @purpose  Insert an entry into the ARP IP lookup table
*
* @param    *pIpInfo    {@b(Input)}  pointer to IP node information
* @param    **ppIp      {@b(Output)} pointer to ARP table entry node ptr
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    entry already exists in table
* @returns  L7_TABLE_IS_FULL  ARP IP table has reached capacity
* @returns  L7_FAILURE
*
* @comment  A table full condition is reported by this function, but no
*           action is taken to remove any entries.
*
* @comment  An entry that is already in the table is treated as an error
*           condition.  Expecting the caller to manage the proper insertion
*           and removal of entries so that this does not occur.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableInsert(ipMapArpIpNode_t *pIpInfo, ipMapArpIpNode_t **ppIp);

/*********************************************************************
* @purpose  Update an entry in the ARP IP lookup table
*
* @param    *pIpInfo    {@b(Input)}  pointer to IP node information
* @param    event       {@b(Input)}  indicates update vs. reissue event
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    entry not found in table
* @returns  L7_FAILURE
*
* @comment  An entry that is not in the table is treated as an error
*           condition.  Expecting the caller to manage the proper insertion
*           and removal of entries so that this does not occur.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableUpdate(ipMapArpIpNode_t *pIpInfo,
                              ipMapArpTableEvent_t event);

/*********************************************************************
* @purpose  Remove an entry from the ARP IP lookup table
*
* @param    *pIpInfo    {@b(Input)} pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    entry not found in table
* @returns  L7_FAILURE
*
* @comment  An entry that is not in the table is treated as an error
*           condition.  Expecting the caller to manage the proper insertion
*           and removal of entries so that this does not occur.
*
* @comment  The caller is advised to make a copy of whatever data it needs
*           from the IP node entry prior to calling this function.  Once
*           the node is deleted from the ARP table, its contents are
*           no longer reliable.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableRemove(ipMapArpIpNode_t *pIpInfo);

/*********************************************************************
* @purpose  Remove the oldest entry from the ARP table
*
* @param    allowGw     {@b(Input))  gateway entry types can be removed
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no removable entry was found
* @returns  L7_FAILURE
*
* @comment  Normally, only a dynamic entry type is considered for removal.
*           If the allowGw parm is set to L7_TRUE, then gateway entries
*           will also be considered after all dynamic entries are gone.
*
* @comment  An empty list is considered an error, since this function is
*           typically called when the list is full, or when there are known
*           entries that need to be removed.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableOldestRemove(L7_BOOL allowGw);

/*********************************************************************
* @purpose  Find the oldest entry currently in the ARP table
*
* @param    allowGw     {@b(Input))  gateway entry types can be removed
* @param    **ppIp      {@b(Output)} pointer to ARP table entry node ptr
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no removable entry was found
* @returns  L7_FAILURE
*
* @comment  Normally, only a dynamic entry type is considered for removal.
*           If the allowGw parm is set to L7_TRUE, then gateway entries
*           will also be considered after all dynamic entries are gone.
*
* @comment  An empty list is considered an error, since this function is
*           typically called when the list is full, or when there are known
*           entries that need to be removed.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableOldestFind(L7_BOOL allowGw, ipMapArpIpNode_t **ppIp);

/*********************************************************************
* @purpose  Search the ARP IP lookup table
*
* @param    ipAddr      {@b(Input)}  IP address search key
* @param    intIfNum    {@b(Input)}  Internal Interface number for the entry
* @param    mode        {@b(Input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
* @param    **ppIp      {@b(Output)} ptr to IP lookup table node output location
*
* @returns  L7_SUCCESS  found IP entry of interest
* @returns  L7_FAILURE  IP entry not found
*
* @comment  Based on the mode flag, can be used to find a specific IP
*           entry, or the next sequential IP address following the one
*           specified.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableSearch(L7_uint32 ipAddr, L7_uint32 intIfNum,
                              L7_uint32 mode, ipMapArpIpNode_t **ppIp);

/*********************************************************************
* @purpose  Change the size of the ARP IP table
*
* @param    maxEnt      {@b(Input)}  maximum number of ARP entries allowed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Only changes the logical (functional) size of the ARP table, but
*           does not change the resource allocation of the underlying AVL tree.
*
* @comment  When the table size is reduced, any dynamic entries in excess of
*           the new size are discarded, starting with the oldest first, then
*           any gateway entries if necessary.  Local and static entries are
*           never discarded here.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableResize(L7_uint32 maxEnt);

/*********************************************************************
* @purpose  Check if the ARP table has reached its maximum capacity
*
* @param    void
*
* @returns  L7_TRUE     table is full
* @returns  L7_FALSE    table not full
*
* @comment  Caller is responsible for any semaphore protection, if desired.
*
* @comment  This function also checks for table overflow error conditions.
*
* @end
*********************************************************************/
L7_BOOL ipMapArpIpTableFullCheck(void);

/*********************************************************************
* @purpose  Check if maximum number of static entries are in ARP table
*
* @param    void
*
* @returns  L7_TRUE     maximum static entries exist
* @returns  L7_FALSE    still room for more static entries
*
* @comment  Caller is responsible for any semaphore protection, if desired.
*
* @comment  This function also checks for table overflow error conditions.
*
* @end
*********************************************************************/
L7_BOOL ipMapArpIpTableStaticMaxCheck(void);

/*********************************************************************
* @purpose  Handle conversion of existing entry to static type
*
* @param    *pIpinfo    {@b(Input)} pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    already a permanent entry
* @returns  L7_FAILURE
*
* @comment  The entry should not already be a permanent (i.e., static)
*           entry.  This is only intended to handle the special case
*           where an existing dynamic/gateway entry is being changed
*           to a static entry so that a delete/add is not needed (avoids
*           potential for routes to become temporarily unresolved).
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableEntryStaticConvert(ipMapArpIpNode_t *pIpInfo);

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param    *pStats     pointer to output location for storing ARP cache stats
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableStatsGet(L7_arpCacheStats_t *pStats);

/*********************************************************************
* @purpose  Display the current ARP IP lookup table contents
*
* @param    intIfNum    internal interface number, or 0 for all interfaces
* @param    numEnt      number of entries to display (0 means all)
*
* @returns  void
*
* @comment
*
* @end
*********************************************************************/
void ipMapArpIpTableShow(L7_uint32 intIfNum, L7_uint32 numEnt);

/*********************************************************************
* @purpose  Initialize the ARP age list
*
* @param    void
*
* @returns  void
*
* @comment
*
* @end
*********************************************************************/
void ipMapArpIpAgeListInit(void);

/*********************************************************************
* @purpose  Add an ARP entry node to the ARP age list
*
* @param    *pIp        pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Assumes caller has obtained the ipMapArpIpTreeData semaphore.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpAgeListAdd(ipMapArpIpNode_t *pIp);

/*********************************************************************
* @purpose  Remove an ARP entry node from the ARP age list
*
* @param    *pIp        pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Assumes caller has obtained the ipMapArpIpTreeData semaphore.
*
* @comment  It is possible for a former dynamic entry to be replaced with
*           a static, so this function cannot assume only dynamic and
*           gateway entries are on the age list.  Such a static entry is
*           removed from the age list here.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpAgeListRemove(ipMapArpIpNode_t *pIp);

/*********************************************************************
* @purpose  Refresh an ARP entry node in the ARP age list
*
* @param    ipAddr      IP address of ARP entry to refresh in age list
* @param    intIfNum    Internal Interface number for the entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Assumes entry node already exists in the ARP age list, so
*           it needs to be moved to the end of the list (i.e., becomes
*           the newest entry).  By definition, the entry is type dynamic
*           or gateway (i.e., non-permanent).
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpAgeListRefresh(L7_uint32 ipAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Display the contents of the ARP age list
*
* @param    intIfNum    {@b(input)} internal interface number, or 0 for all interfaces
* @param    numEnt      {@b(input)} number of entries to display (0 means all)
*
* @returns  void
*
* @comment
*
* @end
*********************************************************************/
void ipMapArpIpAgeListShow(L7_uint32 intIfNum, L7_uint32 numEnt);


#endif /* _IPMAP_ARP_H_ */
