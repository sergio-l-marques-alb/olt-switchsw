
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
* @filename  ipmap_arp_ckpt.c
*
* @purpose   Checkpoint of dynamic ARP entries
*
* @component ARP Mapping Layer
*
* @comments  On nonstop forwarding systems, ARP MAP checkpoints dynamic
*            ARP entries from the management unit to the backup unit.
*            Checkpointing arp entries allows the backup unit to take over
*            with a full arp cache
*
* @create    02/05/2009
*
* @author    Kiran Kumar Kella
*
* @end
*
**********************************************************************/

#include "flex.h"

#ifdef L7_NSF_PACKAGE

#include <stdlib.h>

#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "ipmap_arp.h"
#include "ipmap_arp_ckpt.h"
#include "ckpt_api.h"
#include "avl_api.h"
#include "timer.ext"

/* format of an arp TLV */
typedef struct arpTlv_s
{
  L7_ushort16 changeType;               /* one of ARP_EVENT_TYPE_t */
  L7_ushort16 tlvLen;                   /* length of TLV in bytes, not incl changeType and tlvLen fields */
  L7_uint32   ipAddr;                   /* ARP entry's IP address */
  L7_uint32   intIfNum;                 /* ARP entry's internal interface number */
  L7_uchar8   macAddr[L7_MAC_ADDR_LEN]; /* ARP entry's MAC address - filled in case of Add event */
  L7_uchar8   padding[2];               /* padding to align to 4-byte boundary */
} arpTlv_t;

/* checkpoint message type */
static const L7_ushort16 ARPMAP_CKPT_MSG_TYPE = 1;

/* message header len in bytes. msg type + msg len */
static const L7_uint32 ARPMAP_CKPT_MSG_HDR_LEN = 4;

/* tlv header len in bytes. tlv type + tlv len */
static const L7_uint32 ARPMAP_CKPT_TLV_HDR_LEN = 4;

/* number of bytes in type and length fields of arp add msg TLV */
static const L7_uint32 ARPMAP_CKPT_ADD_ENTRY_LEN = 16;

/* number of bytes in type and length fields of arp delete msg TLV */
static const L7_uint32 ARPMAP_CKPT_DELETE_ENTRY_LEN = 8;

static L7_uchar8 *arpEventStr[ARP_ENTRY_EVENT_LAST] = {"ADD", "MOD", "DEL", "FLUSH" };

static L7_BOOL flushAllPending = L7_FALSE;
static L7_BOOL noAddAfterFlush = L7_FALSE;

/* debug stats */
static L7_uint32 arpCkptMsgsSent = 0;
static L7_uint32 arpCkptEntriesSent = 0;
static L7_uint32 arpCkptAddEntriesSent = 0;
static L7_uint32 arpCkptDeleteEntriesSent = 0;
static L7_uint32 arpCkptFlushEntriesSent = 0;
static L7_uint32 arpCkptBytesSent = 0;

static L7_uint32 arpCkptMsgsRecd = 0;
static L7_uint32 arpCkptEntriesRecd = 0;
static L7_uint32 arpCkptAddEntriesRecd = 0;
static L7_uint32 arpCkptDeleteEntriesRecd = 0;
static L7_uint32 arpCkptFlushEntriesRecd = 0;
static L7_uint32 arpCkptBytesRecd = 0;

static L7_uint32 arpEntriesValid = 0;
static L7_uint32 arpEntriesInvalid = 0;

/* extern variable declarations */
extern L7_uint32       ipMapTraceFlags;
extern ipMapArpCtx_t   ipMapArpCtx_g;

/* extern function declarations */
extern t_ADR *_createAddrEntry(t_ARP *p_A, t_IPAddr *ipAddr, t_MACAddr *macAddr,
                               word entryType, word flags);
extern e_Err ARP_SetTimer(t_ADR *p_Adr, L7_uint32 timeout);

/* global function declarations */
L7_uint32 arpMapCheckpointArpEntryProcess(arpTlv_t *arpTlv);
L7_uint32 arpMapAddFlushEntryToCkptMsg(L7_uchar8 *pos);
L7_uint32 arpMapAddArpEntryToCkptMsg(L7_uchar8 *pos, ipMapArpCkptNode_t *pNode);

void arpMapCheckpoint(L7_CKPT_MGR_CALLBACK_t  callbackType,
                      L7_uchar8  *checkpointData,
                      L7_uint32 maxDataLen,
                      L7_uint32 *retDataLen,
                      L7_BOOL  *moreData);

void arpMapCheckpointBu(L7_CKPT_BACKUP_MGR_CALLBACK_t  callbackType,
                        L7_uchar8  *ckptData,
                        L7_uint32 dataLen);

/*********************************************************************
* @purpose  Create the AVL tree for the checkpointed arp data
*
* @param  void
*
* @returns  L7_SUCCESS if callback registrations are successful
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t arpMapCkptTableCreate(void)
{
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;
  L7_uint32     treeHeapSize, dataNodeSize, dataHeapSize;

  if (!cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID))
  {
    return L7_SUCCESS;
  }

  pArpCtx->ckptTbl.treeEntryMax = platRtrArpMaxEntriesGet();

  /* calculate the amount of memory needed... */
  treeHeapSize = pArpCtx->ckptTbl.treeEntryMax * (L7_uint32)sizeof(avlTreeTables_t);
  dataNodeSize = (L7_uint32) sizeof(ipMapArpCkptNode_t);
  dataHeapSize = pArpCtx->ckptTbl.treeEntryMax * dataNodeSize;

  /* ...and allocate it from the system heap */
  pArpCtx->ckptTbl.treeHeapSize = treeHeapSize;
  pArpCtx->ckptTbl.treeHeap = osapiMalloc(L7_ARP_MAP_COMPONENT_ID, treeHeapSize);
  if (pArpCtx->ckptTbl.treeHeap == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
           "Unable to allocate arp checkpoint table tree heap");
    return L7_FAILURE;
  }

  pArpCtx->ckptTbl.dataHeapSize = dataHeapSize;
  pArpCtx->ckptTbl.dataHeap = osapiMalloc(L7_ARP_MAP_COMPONENT_ID, dataHeapSize);
  if (pArpCtx->ckptTbl.dataHeap == L7_NULLPTR)
  {
    osapiFree(L7_ARP_MAP_COMPONENT_ID, pArpCtx->ckptTbl.treeHeap);
    pArpCtx->ckptTbl.treeHeap = L7_NULLPTR;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
           "Unable to allocate arp checkpoint table data heap");
    return L7_FAILURE;
  }

  memset(pArpCtx->ckptTbl.treeHeap, 0, (size_t)treeHeapSize);
  memset(pArpCtx->ckptTbl.dataHeap, 0, (size_t)dataHeapSize);
  memset(&pArpCtx->ckptTbl.treeData, 0, sizeof(pArpCtx->ckptTbl.treeData));

  avlCreateAvlTree(&pArpCtx->ckptTbl.treeData, pArpCtx->ckptTbl.treeHeap,
                   pArpCtx->ckptTbl.dataHeap,
                   pArpCtx->ckptTbl.treeEntryMax, dataNodeSize, 0x10,
                   (L7_uint32)(2 * sizeof(L7_uint32)));

  (void)avlSetAvlTreeComparator(&pArpCtx->ckptTbl.treeData, avlCompareIPAddr);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear all checkpointed data.
*
* @param  void
*
* @returns  void
*
* @notes    Can be called when a new backup unit is reported or the backup
*           manager is gone. Can also happen if we do a cold restart and
*           need to throw away any data that happened to be checkpointed.
*
* @end
*********************************************************************/
void arpMapCheckpointDataClear(void)
{
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    ipMapTraceWrite("ARP CKPT - clearing checkpoint data");
  }

  /* remove all entries from the AVL tree */
  avlPurgeAvlTree(&pArpCtx->ckptTbl.treeData, pArpCtx->ckptTbl.treeEntryMax);
}

/*********************************************************************
* @purpose  Populate the checkpoint table with the available
*           dynamic ARP cache entries from ARP cache
*
* @param  void
*
* @returns  void
*
* @notes    Can be called when a new backup unit is reported
*           Checkpoint table should have been flushed before calling
*           this function
* @end
*********************************************************************/
void arpMapCheckpointTablePopulate()
{
  t_ARP              *p_A = (t_ARP *)ipMapArpCtx_g.arpHandle;
  t_ADR              *p_Adr;
  e_Err              e;
  ipMapArpCkptNode_t ipNode;
  L7_uint32          tmpIp;
  L7_uchar8          *tmp;

  e = ipMapArpExtenCacheGetFirst(p_A->arpCacheId, &p_Adr);
  while ((e == E_OK) && p_Adr)
  {
    /* Checkpoint only the dynamic and gateway entries */
    if((p_Adr->entryType == ARP_ENTRY_TYPE_DYNAMIC) ||
       (p_Adr->entryType == ARP_ENTRY_TYPE_GATEWAY))
    {
      memset(&ipNode, 0, sizeof(ipNode));
      memcpy(&(ipNode.ipAddr), p_Adr->ipAddr, sizeof(p_Adr->ipAddr));
      tmp = p_Adr->ipAddr;
      tmpIp = (tmp[0] << 24) | (tmp[1] << 16) | (tmp[2] << 8) | (tmp[3]);
      ipNode.ipAddr = (L7_uint32) osapiNtohl(tmpIp);
      ipNode.intIfNum = p_Adr->interfaceNumber;
      memcpy(ipNode.macAddr, p_Adr->macAddr, L7_MAC_ADDR_LEN);

      avlInsertEntry(&ipMapArpCtx_g.ckptTbl.treeData, &ipNode);
    }
    e = ipMapArpExtenCacheGetNext(p_A->arpCacheId, p_Adr, &p_Adr);
  }
}

/*********************************************************************
* @purpose  Register callback functions for both management and backup roles.
*
* @param  void
*
* @returns  L7_SUCCESS if callback registrations are successful
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t arpMapCkptCallbacksRegister(void)
{
  if (!cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID))
  {
    return L7_SUCCESS;
  }

  if (ckptMgrCheckpointCallbackRegister(L7_ARP_MAP_COMPONENT_ID,
                                        arpMapCheckpoint) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_ARP_MAP_COMPONENT_ID,
           "Failed to register with checkpoint callback for management unit.");
    return L7_FAILURE;

  }

  if (ckptBackupMgrCheckpointCallbackRegister(L7_ARP_MAP_COMPONENT_ID,
                                              arpMapCheckpointBu) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_ARP_MAP_COMPONENT_ID,
           "Failed to register with checkpoint callback for backup unit.");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/* Management Unit */

/*********************************************************************
* @purpose  Callback invoked by the checkpoint service on the management unit.
*
* @param  callbackType    @b{(input)}  New backup, no backup, or next element
* @param  checkpointData  @b{(input)}  Checkpoint message buffer. Only non-NULL
*                                        for next element callback.
* @param  maxDataLen      @b{(input)}  Number of bytes that ARP MAP can write to
*                                        checkpoint message (i.e. size of buffer)
* @param  retDataLen      @b{(output)} Number of bytes written to checkpoint msg
* @param  moreData        @b{(output)} Set to L7_TRUE if there is more data to
*                                        checkpoint (i.e., checkpoint service
*                                        should invoke callback again).
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void arpMapCheckpoint(L7_CKPT_MGR_CALLBACK_t  callbackType,
                      L7_uchar8  *checkpointData,
                      L7_uint32 maxDataLen,
                      L7_uint32 *retDataLen,
                      L7_BOOL  *moreData)
{
  switch (callbackType)
  {
    case CKPT_NEW_BACKUP_MANAGER:
      arpMapNewBackupManager();
      break;
    case CKPT_NEXT_ELEMENT:
      arpMapCheckpointMsgWrite(checkpointData, maxDataLen, retDataLen, moreData);
      break;
    case CKPT_BACKUP_MANAGER_GONE:
      arpMapBackupManagerGone();
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
              "Invalid checkpoint callback type %d", callbackType);
  }
}

/*********************************************************************
* @purpose  Handle election of new backup unit
*
* @param    void
*
* @returns  void
*
* @notes   runs on checkpoint service thread
*
* @end
*********************************************************************/
void arpMapNewBackupManager(void)
{
  L7_RC_t rc;

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    ipMapTraceWrite("ARP CKPT - New backup unit elected");
  }

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if (!ipMapArpCtx_g.backupMgrKnown)
  {
    ipMapArpCtx_g.backupMgrKnown = L7_TRUE;
  }

  /* Clear the ARP check point table */
  arpMapCheckpointDataClear();

  /* Generate a FLUSH message to delete any stale entries on the backup.
   * To handle the case for eg., when we get CKPT_NEW_BACKUP_MANAGER
   * during "clear config".
   * During "clear config" any DELETE checkpoint messages would not have
   * reached the Backup for sure. So, just send FLUSH message initially to
   * the new backup. No harm in doing so.
   * flushAllPending is the first thing handled during NEXT_ELEMENT call */
  flushAllPending = L7_TRUE;

  /* Populate the check point table at once with all the dynamic ARP entries
   * from ARP cache */
  arpMapCheckpointTablePopulate();

  /* Each checkpoint service client is required to call ckptMgrCheckpointStart() when
   * a new backkup manager is elected, so that the checkpoint service can tell when
   * an initial checkpoint is done.
   * If there are no arp entries, we still need to make this call. So do it here. If
   * we call twice, no harm done. */
  if (osapiSemaTake(ipMapArpCtx_g.ckptPendingSema, L7_NO_WAIT) == L7_SUCCESS)
  {
    /* If sem take fails, then a checkpoint is already outstanding for ARP MAP */
    rc = ckptMgrCheckpointStart(L7_ARP_MAP_COMPONENT_ID);
    if (rc != L7_SUCCESS)
    {
      osapiSemaGive(ipMapArpCtx_g.ckptPendingSema);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
              "Request for checkpoint message callback failed with error %d", rc);
    }
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
}

/*********************************************************************
* @purpose  Function ARP MAP checkpoint code registers with ARP core
*           to be notified of arp cache changes.
*           Tell the checkpoint service that we have arp entries
*           to checkpoint.
*
* @param    arpEvent  @b{(input)} type of ARP entry event
* @param    ipAddr    @b{(input)} ARP entry's IP Address
* @param    intIfNum  @b{(input)} Internal interface number
* @param    macAddr   @b{(input)} ARP entry's MAC Address
*
* @returns  void
*
* @notes    arpLock already taken before calling this function
*           don't take it again in this function.
*
* @end
*********************************************************************/
void arpMapCheckpointCallback(ARP_EVENT_TYPE_t arpEvent,
                              L7_uint32 ipAddr,
                              L7_uint32 intIfNum,
                              L7_uchar8 *macAddr)
{
  L7_uint32 rc;
  ipMapArpCkptNode_t *pNode = L7_NULLPTR, ipNode;

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    L7_char8 prefixStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(ipAddr, prefixStr);
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "ARP CKPT - request to send %s checkpoint msg for %s",
                    arpEventStr[arpEvent], prefixStr);
    ipMapTraceWrite(traceBuf);
  }

  if (ipMapArpCtx_g.backupMgrKnown)
  {
    /* Update the Checkpoint table with add/delete event of ARP entry */

    memset(&ipNode, 0, sizeof(ipNode));
    ipNode.ipAddr = ipAddr;
    ipNode.intIfNum = intIfNum;

    if(flushAllPending)
    {
      if(ARP_ENTRY_DELETE == arpEvent)
      {
        if(noAddAfterFlush)
        {
          /* This is a delete received as part of the flush operation.
           * No need to checkpoint such deletes covered by flush operation */
          return;
        }
        /* If it reaches here, it means a delete is received for an
         * entry that was added after the flush operation. We need
         * to checkpoint such deletes */
      }
      else if(ARP_ENTRY_ADD == arpEvent)
      {
        /* It means flush is complete (but not yet checkpointed) and
         * an add is received */
        noAddAfterFlush = L7_FALSE;
      }
    }

    if(ARP_ENTRY_FLUSH_ALL == arpEvent)
    {
      flushAllPending = L7_TRUE;  /* set a flag to indicate checkpoint for flush operation */
      noAddAfterFlush = L7_TRUE;

      /* remove all entries from the AVL tree */
      avlPurgeAvlTree(&ipMapArpCtx_g.ckptTbl.treeData, ipMapArpCtx_g.ckptTbl.treeEntryMax);
    }
    else
    {
      pNode = avlSearchLVL7(&ipMapArpCtx_g.ckptTbl.treeData, &ipNode, AVL_EXACT);
      /* Update the delete flag for deletion event */
      if(pNode != L7_NULLPTR)
      {
        memcpy(pNode->macAddr, macAddr, L7_MAC_ADDR_LEN);
        pNode->deleteFlag = ((arpEvent == ARP_ENTRY_DELETE) ? 1 : 0);
      }
      else
      {
        memcpy(ipNode.macAddr, macAddr, L7_MAC_ADDR_LEN);
        ipNode.deleteFlag = ((arpEvent == ARP_ENTRY_DELETE) ? 1 : 0);

        /* inserting new entry in the checkpoint table */
        avlInsertEntry(&ipMapArpCtx_g.ckptTbl.treeData, &ipNode);
      }
    }

    if (osapiSemaTake(ipMapArpCtx_g.ckptPendingSema, L7_NO_WAIT) == L7_SUCCESS)
    {
      /* If sem take fails, a checkpoint is already pending */
      rc = ckptMgrCheckpointStart(L7_ARP_MAP_COMPONENT_ID);
      if (rc != L7_SUCCESS)
      {
        osapiSemaGive(ipMapArpCtx_g.ckptPendingSema);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
                "Request for checkpoint message callback failed with error %d", rc);
      }
    }
  }

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    ipMapTraceWrite("ARP CKPT - checkpoint table updated");
  }
}

/*********************************************************************
* @purpose  Handle notification that backup manager is gone.
*
* @param  void
*
* @returns  void
*
* @notes    runs on the checkpoint service thread
*
* @end
*********************************************************************/
void arpMapBackupManagerGone(void)
{
  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    ipMapTraceWrite("ARP CKPT - Backup unit gone");
  }

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* Clear the ARP check point table */
  arpMapCheckpointDataClear();

  ipMapArpCtx_g.backupMgrKnown = L7_FALSE;

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  /* Giving the pending semaphore. Just try giving it even if it fails.
   *
   * Reason:-
   * If at all the checkpoint service didn't call for NEXT_ELEMENT
   * for some reason during "clear config" for example, we might
   * end up stuck holding the semaphore and hence the latter
   * checkpointing won't work */
  osapiSemaGive(ipMapArpCtx_g.ckptPendingSema);
}

/*********************************************************************
* @purpose  Callback invoked by the checkpoint service on the management unit.
*
* @param  checkpointData  @b{(input)}  Checkpoint message buffer.
* @param  maxDataLen      @b{(input)}  Number of bytes that ARP MAP can write to
*                                      checkpoint message
* @param  msgLen          @b{(output)} Number of bytes written to checkpoint msg
* @param  moreData        @b{(output)} Set to L7_TRUE if there is more data to
*                                      checkpoint
*
* @returns  void
*
* @notes  runs on the checkpoint service thread
*         To avoid allocating a large buffer, get one arp change at a time.
*
* @end
*********************************************************************/
void arpMapCheckpointMsgWrite(L7_uchar8  *checkpointData,
                             L7_uint32 maxDataLen,
                             L7_uint32 *msgLen,
                             L7_BOOL  *moreData)
{
  const L7_uint32 arpEntryLen = sizeof(arpTlv_t);
  ipMapArpCkptNode_t *pNode, ipNode;
  L7_uint32 tlvLen    = 0;
  L7_uchar8   *pos    = checkpointData;     /* Next byte to write in checkpoint message */
  L7_ushort16 mLength = 0;                  /* message length in network byte order */
  L7_ushort16 msgType = osapiHtons(ARPMAP_CKPT_MSG_TYPE);
  L7_BOOL     flushEventSent = L7_FALSE;

  /* message type */
  memcpy(pos, &msgType, sizeof(msgType));
  pos += ARPMAP_CKPT_MSG_HDR_LEN;        /* skip to start of arp TLV */
  *msgLen = 0;

  memset(&ipNode, 0, sizeof(ipNode));
  ipNode.ipAddr   = 0;
  ipNode.intIfNum = L7_INVALID_INTF;

  *moreData = L7_FALSE;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if(L7_TRUE == flushAllPending)
  {
    /* Flush event is pending. Send it first */
    flushAllPending = L7_FALSE; /* Reset the flag */

    tlvLen = arpMapAddFlushEntryToCkptMsg(pos);
    pos += tlvLen;
    *msgLen += tlvLen;

    flushEventSent = L7_TRUE;
  }

  pNode = avlSearchLVL7(&ipMapArpCtx_g.ckptTbl.treeData, (void *)&ipNode, AVL_NEXT);

  if((pNode == L7_NULLPTR) && (! flushEventSent))
  {
    osapiSemaGive(ipMapArpCtx_g.ckptPendingSema);
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return;
  }

  /* keep adding arp events to checkpoint message until either Ckpt table has no more
   * entries or the checkpoint message is full. */
  while((pNode != L7_NULLPTR) && ((pos + arpEntryLen) < (checkpointData + maxDataLen)))
  {
    ipNode.ipAddr   = pNode->ipAddr;
    ipNode.intIfNum = pNode->intIfNum;

    tlvLen = arpMapAddArpEntryToCkptMsg(pos, pNode);

    pos += tlvLen;
    *msgLen += tlvLen;

    /* delete the checkpointed node from the AVL tree */
    if (avlDeleteEntry(&ipMapArpCtx_g.ckptTbl.treeData, pNode) != pNode)
    {
      L7_char8 prefixStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(pNode->ipAddr, prefixStr);

      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
              "Failed to delete arp entry for %s from checkpoint table on master", prefixStr);
    }

    pNode = avlSearchLVL7(&ipMapArpCtx_g.ckptTbl.treeData, (void *)&ipNode, AVL_NEXT);
  }
  if(pNode != L7_NULLPTR)
  {
    *moreData = L7_TRUE;
  }
  if(*msgLen) /* If there are messages to be checkpointed */
  {
    *msgLen += ARPMAP_CKPT_MSG_HDR_LEN;     /* includes msg length field */
  }
  mLength = osapiHtons(*msgLen);
  memcpy(checkpointData + 2, &mLength, sizeof(mLength));

  arpCkptMsgsSent++;
  arpCkptBytesSent += *msgLen;

  if(! (*moreData))
  {
    /* No more entries to be checkpointed */
    osapiSemaGive(ipMapArpCtx_g.ckptPendingSema);
  }

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "ARP CKPT - sending %u byte checkpoint message. More data is %s.",
                  *msgLen, *moreData ? "TRUE" : "FALSE");
    ipMapTraceWrite(traceBuf);
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
}

/*********************************************************************
* @purpose  Add a flush all event to a checkpoint message
*
* @param  pos    @b{(input)}  Address in message buffer where next entry begins
*
* @returns  number of bytes written to checkpoint message
*
* @notes  runs on the checkpoint service thread
*
* @end
*********************************************************************/
L7_uint32 arpMapAddFlushEntryToCkptMsg(L7_uchar8 *pos)
{
  arpTlv_t *arpTlv;
  L7_uchar8 *tlvStart = pos;

  arpTlv = (arpTlv_t*) tlvStart;

  memset(arpTlv, 0, sizeof(arpTlv_t));

  arpTlv->changeType = osapiHtons(ARP_ENTRY_FLUSH_ALL);
  arpTlv->tlvLen     = osapiHtons(ARPMAP_CKPT_DELETE_ENTRY_LEN);
  arpCkptFlushEntriesSent++;

  pos += ARPMAP_CKPT_TLV_HDR_LEN + arpTlv->tlvLen;

  arpCkptEntriesSent++;

  return (L7_uint32) (pos - tlvStart);
}

/*********************************************************************
* @purpose  Add an arp entry to a checkpoint message
*
* @param  pos    @b{(input)}  Address in message buffer where next entry begins
* @param  pNode  @b{(input)}  arp entry to convert to a checkpoint entry
*
* @returns  number of bytes written to checkpoint message
*
* @notes  runs on the checkpoint service thread
*
* @end
*********************************************************************/
L7_uint32 arpMapAddArpEntryToCkptMsg(L7_uchar8 *pos, ipMapArpCkptNode_t *pNode)
{
  arpTlv_t *arpTlv;
  L7_uchar8 *tlvStart = pos;

  arpTlv = (arpTlv_t*) tlvStart;

  memset(arpTlv, 0, sizeof(arpTlv_t));

  arpTlv->ipAddr   = osapiHtonl(pNode->ipAddr);
  arpTlv->intIfNum = osapiHtonl(pNode->intIfNum);

  if(0 == pNode->deleteFlag)
  {
    memcpy(arpTlv->macAddr, pNode->macAddr, sizeof(arpTlv->macAddr));
    arpTlv->changeType = osapiHtons(ARP_ENTRY_ADD);
    arpTlv->tlvLen     = osapiHtons(ARPMAP_CKPT_ADD_ENTRY_LEN);
    arpCkptAddEntriesSent++;
  }
  else
  {
    arpTlv->changeType = osapiHtons(ARP_ENTRY_DELETE);
    arpTlv->tlvLen     = osapiHtons(ARPMAP_CKPT_DELETE_ENTRY_LEN);
    arpCkptDeleteEntriesSent++;
  }

  pos += ARPMAP_CKPT_TLV_HDR_LEN + arpTlv->tlvLen;

  arpCkptEntriesSent++;

  return (L7_uint32) (pos - tlvStart);
}

/*********************************************************************
* @purpose  On a warm restart, remove dynamic ARP entries associated with
*           failed interfaces, such as interfaces on the former management unit.
*
* @param  void
*
* @returns  void
*
* @notes    This function runs on the IP MAP processing thread
*
* @end
*********************************************************************/
L7_RC_t arpMapCheckpointDataApply(void)
{
  L7_uint32 ipAddr, localAddr, localMask;
  ipMapArpCkptNode_t ipNode, *pNode = L7_NULLPTR;
  ipMapArpIntf_t  *pIntf;
  t_ARP *p_A = (t_ARP *)ipMapArpCtx_g.arpHandle;
  t_ADR *p_Adr;

  if (!cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID))
  {
    return L7_SUCCESS;
  }

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "ARP CKPT - applying checkpoint data");
    ipMapTraceWrite(traceBuf);
  }

  memset(&ipNode, 0, sizeof(ipNode));
  ipNode.ipAddr   = 0;
  ipNode.intIfNum = L7_INVALID_INTF;

  pNode = avlSearchLVL7(&ipMapArpCtx_g.ckptTbl.treeData, (void *)&ipNode, AVL_NEXT);
  while(pNode != L7_NULLPTR)
  {
    L7_char8 prefixStr[OSAPI_INET_NTOA_BUF_SIZE];

    ipAddr   = osapiHtonl(pNode->ipAddr);

    osapiInetNtoa(pNode->ipAddr, prefixStr);

    /* Add only valid dynamic arp address entries into the ARP Cache */
    if(ipMapArpSubnetFind(ipAddr, &pIntf, &localAddr, &localMask) == L7_SUCCESS)
    {
      if((p_Adr = _createAddrEntry(p_A, (t_IPAddr *)&ipAddr,
                                   (t_MACAddr *)((L7_uchar8 *)pNode->macAddr),
                                   ARP_ENTRY_TYPE_DYNAMIC, pIntf->index)) == L7_NULLPTR)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
                "Failed to create arp entry for %s in ARP application during warm restart on backup",
                prefixStr);
        arpEntriesInvalid++;
      }
      else
      {
        arpEntriesValid++;

        /* ARP does not maintain ARP entry age times across a restart.
         * The age time of checkpointed entries is reset after a warm restart.
         * ARP sets the timer to age out checkpointed entries anywhere from
         * 1/4 to 1/2 the configured maximum age. */
        if(ARP_SetTimer(p_Adr, TimerFractVarLessThanHalfAgeTime(p_A->arpAgeTime)) != E_OK)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
                  "Failed to set age for arp entry of %s during warm restart on backup", prefixStr);
        }
      }
    }
    else
    {
      arpEntriesInvalid++;
    }
    pNode = avlSearchLVL7(&ipMapArpCtx_g.ckptTbl.treeData, (void *)pNode, AVL_NEXT);
  }

  /* Clear the ARP checkpointed table contents. We are done with synchronizing */
  arpMapCheckpointDataClear();

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "ARP CKPT - finished applying checkpoint data");
    ipMapTraceWrite(traceBuf);
  }

  /* Tell Configurator that hardware update by ARP component is done */
  cnfgrApiComponentHwUpdateDone(L7_ARP_MAP_COMPONENT_ID, L7_CNFGR_HW_APPLY_L3);

  return L7_SUCCESS;
}

/* Backup unit */

/*********************************************************************
* @purpose  Callback invoked by the checkpoint service on the backup unit.
*
* @param  callbackType    @b{(input)}  New backup or checkpoint message
* @param  ckptData        @b{(input)}  Checkpoint message. Only non-NULL
*                                        for SYNC_DATA callback.
* @param  dataLen         @b{(input)}  Number of bytes in checkpoint message
*
* @returns  void
*
* @notes    This function executes while ARP MAP is in WMU state. It runs
*           on the checkpoint service thread.
*
* @end
*********************************************************************/
void arpMapCheckpointBu(L7_CKPT_BACKUP_MGR_CALLBACK_t  callbackType,
                       L7_uchar8  *ckptData,
                       L7_uint32 dataLen)
{
  switch (callbackType)
  {
    case CKPT_BACKUP_SYNC_DATA:
      arpMapCheckpointMsgProcess(ckptData, dataLen);
      break;

    case CKPT_BACKUP_NEW_MANAGER:
      arpMapCheckpointDataClear();
      break;

    default:
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
              "Unexpected checkpoint service callback type %d.",
              callbackType);
  }
}

/*********************************************************************
* @purpose  Process a received checkpoint message on the backup unit.
*
* @param  ckptData        @b{(input)}  Checkpoint message. Only non-NULL
*                                        for SYNC_DATA callback.
* @param  dataLen         @b{(input)}  Number of bytes in checkpoint message
*
* @returns  void
*
* @notes    This function executes while ARP MAP is in WMU state. It runs
*           on the checkpoint service thread.
*
* @end
*********************************************************************/
void arpMapCheckpointMsgProcess(L7_uchar8 *ckptData, L7_uint32 dataLen)
{
  L7_ushort16 msgLen = 0;
  L7_ushort16 msgType = 0;
  L7_uchar8 *pos = ckptData;
  arpTlv_t *arpTlv;

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "ARP CKPT - received %u byte checkpoint message.", dataLen);
    ipMapTraceWrite(traceBuf);
  }

  if (!ckptData)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
            "NULL checkpoint message buffer pointer.");
    return;
  }

  arpCkptMsgsRecd++;
  arpCkptBytesRecd += dataLen;

  /* Minimum checkpoint message size is one route plus the 4-byte message header */
  if (dataLen < (ARPMAP_CKPT_MSG_HDR_LEN + ARPMAP_CKPT_TLV_HDR_LEN + ARPMAP_CKPT_DELETE_ENTRY_LEN))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
            "Checkpoint message too short (%u bytes).", dataLen);
    return;
  }

  /* Make sure length of message as reported by checkpoint service matches
   * the length reported in the message itself. */
  memcpy(&msgLen, ckptData + 2, 2);
  if (osapiNtohs(msgLen) != dataLen)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
            "Checkpoint service reports message length is %u bytes. Message claims to be %u bytes.",
            dataLen, osapiNtohl(msgLen));
    /* Drop checkpoint message */
    return;
  }

  /* The only message type ARP MAP currently sends is 1. If something else received,
   * note the error. */
  memcpy(&msgType, ckptData, 2);
  msgType = osapiNtohs(msgType);
  if (msgType != ARPMAP_CKPT_MSG_TYPE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
            "Unknown checkpoint message type %u", osapiNtohs(msgType));
    return;   /* drop msg */
  }

  pos += ARPMAP_CKPT_MSG_HDR_LEN;   /* skip past msg type and length fields */
  while (pos < (ckptData + dataLen))
  {
    arpTlv = (arpTlv_t*) pos;
    pos += arpMapCheckpointArpEntryProcess(arpTlv);
  }
  return;
}

/*********************************************************************
* @purpose  Process a received checkpoint arp entry on the backup unit.
*
* @param  arpTlv   @b{(input)}  Start of an arp TLV in a checkpoint message
*
* @returns  Number of bytes processed from the checkpoint message
*
* @notes    This function executes while ARP MAP is in WMU state. It runs
*           on the checkpoint service thread.
*
* @end
*********************************************************************/
L7_uint32 arpMapCheckpointArpEntryProcess(arpTlv_t *arpTlv)
{
  L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
  L7_char8 prefixStr[OSAPI_INET_NTOA_BUF_SIZE];

  ipMapArpCkptNode_t ipNode, *pNode = L7_NULLPTR;
  ARP_EVENT_TYPE_t changeType;

  changeType = (ARP_EVENT_TYPE_t) osapiNtohs(arpTlv->changeType);

  memset(&ipNode, 0, sizeof(ipNode));
  ipNode.ipAddr   = osapiNtohl(arpTlv->ipAddr);
  ipNode.intIfNum = osapiNtohl(arpTlv->intIfNum);

  osapiInetNtoa(ipNode.ipAddr, prefixStr);
  if (ipMapTraceFlags & IPMAP_TRACE_ARP_CKPT)
  {
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "ARP CKPT - recvd checkpoint %s arp for %s",
                  arpEventStr[changeType], prefixStr);
    ipMapTraceWrite(traceBuf);
  }

  if(changeType == ARP_ENTRY_FLUSH_ALL)
  {
    arpCkptFlushEntriesRecd++;

    /* remove all entries from the AVL tree */
    avlPurgeAvlTree(&ipMapArpCtx_g.ckptTbl.treeData, ipMapArpCtx_g.ckptTbl.treeEntryMax);
  }
  else
  {
    pNode = avlSearchLVL7(&ipMapArpCtx_g.ckptTbl.treeData, &ipNode, AVL_EXACT);

    /* Deletes only include the ipAddr and intIfNum */
    if ((changeType == ARP_ENTRY_ADD) || (changeType == ARP_ENTRY_MODIFY))
    {
      arpCkptAddEntriesRecd++;
      memcpy(ipNode.macAddr, arpTlv->macAddr, sizeof(arpTlv->macAddr));
      if(pNode != L7_NULLPTR)
      {
        memcpy(pNode, &ipNode, sizeof(ipNode));
      }
      else
      {
        avlInsertEntry(&ipMapArpCtx_g.ckptTbl.treeData, &ipNode);
      }
    }
    else if(changeType == ARP_ENTRY_DELETE)
    {
      arpCkptDeleteEntriesRecd++;
      if(avlDeleteEntry(&ipMapArpCtx_g.ckptTbl.treeData, pNode) != pNode)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
                "Failed to delete arp entry for %s from checkpoint table on backup", prefixStr);
      }
    }
  }

  arpCkptEntriesRecd++;
  return (ARPMAP_CKPT_TLV_HDR_LEN + osapiNtohs(arpTlv->tlvLen));
}

/* debug */
void arpMapCheckpointStatsShow(void)
{
  L7_uint32 count = 0;

  if (!cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID))
  {
    return;
  }

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);
  if (ipMapArpCtx_g.backupMgrKnown)
  {
    sysapiPrintf("\nA backup unit has been elected");
  }
  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  count = avlTreeCount(&ipMapArpCtx_g.ckptTbl.treeData);
  sysapiPrintf("\nSize of Checkpoint Table %u\n", count);

  sysapiPrintf("\nARP transmit checkpoint stats:");
  sysapiPrintf("\n  Ckpt Messages      :  %u", arpCkptMsgsSent);
  sysapiPrintf("\n  Ckpt Bytes         :  %u", arpCkptBytesSent);
  sysapiPrintf("\n  ARP Entries(Total) :  %u", arpCkptEntriesSent);
  sysapiPrintf("\n     ARP Adds        :  %u", arpCkptAddEntriesSent);
  sysapiPrintf("\n     ARP Deletes     :  %u\n", arpCkptDeleteEntriesSent);
  sysapiPrintf("\n     ARP Flushes     :  %u\n", arpCkptFlushEntriesSent);

  sysapiPrintf("\nARP receive checkpoint stats:");
  sysapiPrintf("\n  Ckpt Messages      :  %u", arpCkptMsgsRecd);
  sysapiPrintf("\n  Ckpt Bytes         :  %u", arpCkptBytesRecd);
  sysapiPrintf("\n  ARP Entries(Total) :  %u", arpCkptEntriesRecd);
  sysapiPrintf("\n     ARP Adds        :  %u", arpCkptAddEntriesRecd);
  sysapiPrintf("\n     ARP Deletes     :  %u\n", arpCkptDeleteEntriesRecd);
  sysapiPrintf("\n     ARP Flushes     :  %u\n", arpCkptFlushEntriesRecd);

  sysapiPrintf("\nValid ARP Entries received    : %u\n", arpEntriesValid);
  sysapiPrintf("\nInvalid ARP Entries received  : %u\n", arpEntriesInvalid);
}

void arpMapCheckpointTableShow(void)
{
  L7_uchar8 prefixStr[IPMAP_TRACE_LEN_MAX];
  L7_uint32 i = 1;
  ipMapArpCkptNode_t ipNode, *pNode = L7_NULLPTR;

  if (!cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID))
  {
    return;
  }

  memset(&ipNode, 0, sizeof(ipNode));
  ipNode.ipAddr   = 0;
  ipNode.intIfNum = L7_INVALID_INTF;

  pNode = avlSearchLVL7(&ipMapArpCtx_g.ckptTbl.treeData, (void *)&ipNode, AVL_NEXT);
  while(pNode != L7_NULLPTR)
  {
    osapiInetNtoa(pNode->ipAddr, prefixStr);
    /* Here we are displaying internal interface number as we might not be able
     * to display its u/s/p value in case of vlan thats not created on backup
     * until the unit has taken over */
    sysapiPrintf("[%4d] IP: %s, If: %d, MAC: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x, Delete(on Mgr): %s\n",
                 i, prefixStr, pNode->intIfNum, pNode->macAddr[0], pNode->macAddr[1], pNode->macAddr[2],
                 pNode->macAddr[3], pNode->macAddr[4], pNode->macAddr[5],
                 ((pNode->deleteFlag) ? "T" : "F"));
    pNode = avlSearchLVL7(&ipMapArpCtx_g.ckptTbl.treeData, (void *)pNode, AVL_NEXT);
    i++;
  }
}

void arpMapCheckpointStatsReset(void)
{
  arpCkptMsgsSent = 0;
  arpCkptBytesSent = 0;
  arpCkptEntriesSent = 0;
  arpCkptAddEntriesSent = 0;
  arpCkptDeleteEntriesSent = 0;
  arpCkptFlushEntriesSent = 0;
  arpCkptMsgsRecd = 0;
  arpCkptBytesRecd = 0;
  arpCkptEntriesRecd = 0;
  arpCkptAddEntriesRecd = 0;
  arpCkptDeleteEntriesRecd = 0;
  arpCkptFlushEntriesRecd = 0;
  arpEntriesValid = 0;
  arpEntriesInvalid = 0;
}

#endif   /* L7_NSF_PACKAGE */
