/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
* @filename  mcast_ckpt.c
*
* @purpose   Checkpoint of MCAST Data
*
* @component MCAST
*
* @comments  On nonstop forwarding systems, IP Multicast checkpoints data
*            about the IP multicast forwarding entries.
*
* @create    7/8/2009
*
* @end
*
**********************************************************************/

#include "flex.h"

#ifdef L7_NSF_PACKAGE

#include "osapi.h"
#include "osapi_support.h"
#include "cnfgr.h"

#include "rtmbuf.h"
#include "l7_ip_api.h"
#include "mfc_api.h"
#include "mfc.h"
#include "ckpt_api.h"
#include "mcast_debug.h"
#include "mcast_ckpt.h"
#include "mcast_util.h"

mcastCkptInfo_t *pMcastCkptInfo;

static L7_RC_t mcastCkptEntryLookup(mcastCkptRecord_t *mcastCkptData, mcastCkptRecord_t **mcastCkptEntry);
L7_RC_t mcastCkptEntryNextGet(mcastCkptRecord_t *mcastCkptData, mcastCkptRecord_t **mcastCkptEntry);
static L7_RC_t mcastCkptEntryAdd(mcastCkptRecord_t *mcastCkptData);
L7_RC_t mcastCkptEntryRemove(mcastCkptRecord_t *mcastCkptData);
static L7_int32 mcastCkptEntryCompare(const void *pData1, const void *pData2, L7_uint32 size);

void dumpBuffer(L7_uchar8 *buffer, L7_uint32 length);
/****************************************************************************/
/*                                 General                                  */
/****************************************************************************/

/*********************************************************************
* @purpose  Init MCAST checkpoint code.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mcastCkptInit()
{
  if ((pMcastCkptInfo = (mcastCkptInfo_t *)osapiMalloc(L7_FLEX_MCAST_MAP_COMPONENT_ID, sizeof(mcastCkptInfo_t))) == L7_NULL)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_CKPT_EVENTS, "Unable to allocate IP Multicast checkpoint data structure.");
    return L7_FAILURE;
  }

  pMcastCkptInfo->mcastCkptPendingSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (pMcastCkptInfo->mcastCkptPendingSema == L7_NULL)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_CKPT_EVENTS, "Unable to create IP Multicast checkpoint semaphore.");
    return L7_FAILURE;
  }

  pMcastCkptInfo->mcastCkptDbSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  if (pMcastCkptInfo->mcastCkptDbSema == L7_NULL)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_CKPT_EVENTS, "Unable to create IP Multicast checkpoint semaphore.");
    return L7_FAILURE;
  }

  pMcastCkptInfo->mcastCkptAvlTreeMaxEntries = MCAST_CKPT_TABLE_SIZE;
  avlAllocAndCreateAvlTree(&pMcastCkptInfo->mcastCkptAvlTree,
                           L7_FLEX_MCAST_MAP_COMPONENT_ID,
                           pMcastCkptInfo->mcastCkptAvlTreeMaxEntries,
                           sizeof(mcastCkptRecord_t),
                           MCAST_CKPT_AVL_TREE_TYPE,
                           mcastCkptEntryCompare,
                           sizeof(L7_inet_addr_t) * 2);    

  if (pMcastCkptInfo->mcastCkptAvlTree.semId == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_CKPT_EVENTS, "Failed to allocate and create IP Multicast checkpoint AVL tree.\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Fini MCAST checkpoint code.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mcastCkptFini()
{
  L7_RC_t rc = L7_SUCCESS;

  if (pMcastCkptInfo->mcastCkptPendingSema != L7_NULL)
  {
    if (osapiSemaDelete(pMcastCkptInfo->mcastCkptPendingSema) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
    }
  }

  if (pMcastCkptInfo->mcastCkptDbSema != L7_NULL)
  {
    if (osapiSemaDelete(pMcastCkptInfo->mcastCkptDbSema) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
    }
  }

  if (pMcastCkptInfo->mcastCkptAvlTree.semId != L7_NULLPTR)
  {
    avlDeleteAvlTree(&pMcastCkptInfo->mcastCkptAvlTree);
  }

  return rc;
}

/*********************************************************************
* @purpose  Take the semaphore for the MCAST ckpt DB
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void mcastCkptDbSemaTake()
{
  osapiSemaTake(pMcastCkptInfo->mcastCkptDbSema, L7_WAIT_FOREVER);
}

/*********************************************************************
* @purpose  Give the semaphore for the MCAST ckpt DB
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void mcastCkptDbSemaGive()
{
  osapiSemaGive(pMcastCkptInfo->mcastCkptDbSema);
}

/*********************************************************************
* @purpose  Handle election of new backup unit
*
* @param    void
*
* @returns  void
*
* @notes    runs on checkpoint service thread
*
* @end
*********************************************************************/
void mcastCkptNewBackupManager()
{
  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  /* Handle new backup manager here */
  pMcastCkptInfo->mcastCkptBackupManagerElected = L7_TRUE;

  /* Flush the backup data first, then rebuild it. */
  mcastCkptFlush(L7_FALSE);
  mcastCkptRebuild();

  /* We must start a checkpoint operation, even if one is already pending. */
  mcastCkptStart(L7_TRUE);

  return;
}

/*********************************************************************
* @purpose  Callback invoked by the checkpoint service on the management unit.
*
* @param  checkpointData  @b{(input)}  Checkpoint message buffer. Only non-NULL
*                                        for next element callback.
* @param  maxDataLen      @b{(input)}  Number of bytes that MCAST can write to
*                                        checkpoint message
* @param  msgLen          @b{(output)} Number of bytes written to checkpoint msg
* @param  moreData        @b{(output)} Set to L7_TRUE if there is more data to
*                                        checkpoint
*
* @returns  void
*
* @notes  runs on the checkpoint service thread
*
* @end
*********************************************************************/
void mcastCkptMsgWrite(L7_uchar8 *checkpointData,
                       L7_uint32  maxDataLen,
                       L7_uint32 *msgLen,
                       L7_BOOL   *moreData)
{
  CkptMsg_t          *ckptMsgHeader;
  L7_uchar8          *currBuffPtr;
  L7_uint32           tlvLength;
  L7_ushort16         entryLength;
  L7_ushort16         entryType;
  L7_BOOL             bufferFull;
  mcastCkptRecord_t   entryKey, *pEntry;

  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  /* reserve space for the message header */
  *msgLen = 4;
  /* did caller leave room in buffer for the header? */
  if (maxDataLen > *msgLen)
  {
    /* load the header data we know now */
    ckptMsgHeader = (CkptMsg_t *)checkpointData;
    ckptMsgHeader->message_type = osapiHtons(MCAST_CKPT_MESSAGE_TYPE_1);

    /* update current write position and amount of space left in buffer */
    currBuffPtr = checkpointData + 4;
    maxDataLen -= 4;

    bufferFull = L7_FALSE;

    memset(&entryKey, 0, sizeof(entryKey));
    pEntry = &entryKey;

    while ((bufferFull != L7_TRUE) && 
           (mcastCkptEntryNextGet(pEntry, &pEntry) == L7_SUCCESS))
    {
      if (pEntry->ckptFlags != 0)
      {
        /* calculate length of TLV data and space neede in buffer to hold TLV */
        entryLength = sizeof(mcastCkptRecord_t);
        tlvLength = sizeof(L7_ushort16) +        /* Type   */
                    sizeof(L7_ushort16) +        /* Length */
                    entryLength;                 /* Data   */
  
        /* only add to buffer if there is room */
        if (tlvLength <= maxDataLen)
        {
          maxDataLen -= tlvLength;
  
          /* Type */
          entryType = (L7_ushort16) MCAST_CKPT_TYPE_MFC_ENTRY;
          entryType = osapiHtons(entryType);
          memcpy(currBuffPtr, &entryType, sizeof(L7_ushort16));
          *msgLen += sizeof(L7_ushort16);
          currBuffPtr += sizeof(L7_ushort16);
  
          /* Length */
          entryLength = osapiHtons(entryLength);
          memcpy(currBuffPtr, &entryLength, sizeof(entryLength));
          *msgLen += sizeof(entryLength);
          currBuffPtr += sizeof(entryLength);
  
          /* Data */
          memcpy(currBuffPtr, pEntry, sizeof(mcastCkptRecord_t));
          *msgLen += sizeof(mcastCkptRecord_t);
          currBuffPtr += sizeof(mcastCkptRecord_t);
  
          /* If ckpt item is a delete we can now remove this entry from our list. */
          if (pEntry->ckptFlags & MCAST_CKPT_FLAG_DELETE_PENDING)
          {
            mcastCkptEntryRemove(pEntry);
          }
          else
          {
            pEntry->ckptFlags = 0;
          }
        }
        else
        {
          /* flag to exit loop scanning checkpoint database when message buffer full */
          bufferFull = L7_TRUE;
        }
      }
    }
  }

  /* check if we added any data to the message above */
  if (*msgLen == 4)
  {
    mcastCkptIsNotPending();
    *msgLen = 0;
    *moreData = L7_FALSE;
  }
  else
  {
    ckptMsgHeader->message_length = *msgLen;
    *moreData = L7_TRUE;
  }

  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "wrote %u bytes to checkpointData buffer, returning moreData = %s\n",
                  *msgLen, *moreData ? "TRUE":"FALSE");
}

/*********************************************************************
* @purpose  Handle notification that backup manager is gone.
*
* @param    void
*
* @returns  void
*
* @notes    runs on the checkpoint service thread
*
* @end
*********************************************************************/
void mcastCkptBackupManagerGone()
{
  pMcastCkptInfo->mcastCkptBackupManagerElected = L7_FALSE;

  /* Flush the MCAST checkpoint data. */
  mcastCkptFlush(L7_FALSE);

  return;
}

/*********************************************************************
* @purpose  Callback invoked by the checkpoint service on the management unit.
*
* @param  callbackType    @b{(input)}  New backup, no backup, or next element
* @param  checkpointData  @b{(input)}  Checkpoint message buffer. Only non-NULL
*                                        for next element callback.
* @param  maxDataLen      @b{(input)}  Number of bytes that MCAST can write to
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
void mcastCkptManagerCallback(L7_CKPT_MGR_CALLBACK_t callbackType,
                              L7_uchar8             *checkpointData,
                              L7_uint32              maxDataLen, 
                              L7_uint32             *retDataLen,
                              L7_BOOL               *moreData)
{
  mcastCkptDbSemaTake();

  switch (callbackType)
  {
  case CKPT_NEW_BACKUP_MANAGER:
    mcastCkptNewBackupManager();
    break;

  case CKPT_NEXT_ELEMENT:
    mcastCkptMsgWrite(checkpointData, maxDataLen, retDataLen, moreData);
    break;

  case CKPT_BACKUP_MANAGER_GONE:
    mcastCkptBackupManagerGone();
    break;

  default:
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_MCAST_MAP_COMPONENT_ID,
            "Invalid checkpoint callback type %d", callbackType);
  }

  mcastCkptDbSemaGive();

  return;
}

/*********************************************************************
* @purpose  Process a received checkpoint message on the backup unit.
*
* @param  ckptData        @b{(input)}  Checkpoint message. Only non-NULL
*                                        for SYNC_DATA callback.
* @param  entryLength     @b{(input)}  Length of data
*
* @returns  void
*
* @notes    This function executes while MCAST is in WMU state. It runs
*           on the checkpoint service thread.
*
* @end
*********************************************************************/
void mcastCkptEntryMsgProcess(L7_uchar8 *ckptData, L7_ushort16 entryLength)
{
  mcastCkptRecord_t  tmpEntry;
  mcastCkptRecord_t *pEntry;

  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, " invoked\n");

  memcpy(&tmpEntry, ckptData, sizeof(mcastCkptRecord_t));

  if (tmpEntry.ckptFlags & MCAST_CKPT_FLAG_DELETE_PENDING)
  {
    /* Delete the entry from our database. */

    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  deleting checkpoint entry.\n");

    if (mcastCkptEntryLookup(&tmpEntry, &pEntry) == L7_SUCCESS)
    {
      mcastCkptEntryRemove(pEntry);
    }
    else
    {
      MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  did not find checkpoint entry for deletion.\n");
    }
  }
  else if (tmpEntry.ckptFlags & MCAST_CKPT_FLAG_SYNC_PENDING)
  {
    /* Add the entry to our database. */

    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  adding checkpoint entry.\n");

    if (mcastCkptEntryLookup(&tmpEntry, &pEntry) != L7_SUCCESS)
    {
      if (mcastCkptEntryAdd(&tmpEntry) != L7_SUCCESS)
      {
        MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "Could not add entry\n");
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_MCAST_MAP_COMPONENT_ID, "Could not add entry");
        return;
      }
    }
    else
    {
      MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, " checkpoint entry already in table.\n");
      pEntry->intIfNum = osapiNtohl(tmpEntry.intIfNum);
      pEntry->rxPort = osapiNtohl(tmpEntry.rxPort);
    }
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
* @notes    This function executes while MCAST is in WMU state. It runs
*           on the checkpoint service thread.
*
* @end
*********************************************************************/
void mcastCkptMsgProcess(L7_uchar8 *ckptData, L7_uint32 dataLen)
{
  L7_uint32         bytesConsumed = 0;
  CkptMsg_t        *ckptMsgHeader;
  L7_ushort16       ckptMsgType;
  L7_ushort16       ckptMsgLength;
  L7_ushort16       tlvType;
  L7_ushort16       entryLength;
  L7_uchar8        *entryData;

  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  /* process checkpoint message header data */
  ckptMsgHeader = (CkptMsg_t *)ckptData;
  ckptMsgType = osapiNtohs(ckptMsgHeader->message_type);
  ckptMsgLength = osapiNtohs(ckptMsgHeader->message_length);

  if (ckptMsgType != MCAST_CKPT_MESSAGE_TYPE_1)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "Error: unknown ckptMsgType (%X)\n", ckptMsgType);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_FLEX_MCAST_MAP_COMPONENT_ID, "Unknown checkpoint message type received (%x)", ckptMsgType);
    return;
  }

  if (ckptMsgLength > dataLen)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "Error: ckptMsgLength (%d) greater than dataLen (%d)\n", 
                ckptMsgLength, dataLen);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_FLEX_MCAST_MAP_COMPONENT_ID, "Checkpoint message expected length exceeds buffer length");
    return;
  }

  bytesConsumed += 4;

  while (bytesConsumed < dataLen)
  {
    memcpy(&tlvType, &ckptData[bytesConsumed], sizeof(L7_ushort16));
    tlvType = osapiNtohs(tlvType);
    bytesConsumed += sizeof(L7_ushort16);

    memcpy(&entryLength, &ckptData[bytesConsumed], sizeof(entryLength));
    entryLength = osapiNtohs(entryLength);
    bytesConsumed += sizeof(entryLength);

    entryData = &ckptData[bytesConsumed];

    bytesConsumed += entryLength;

    switch (tlvType)
    {
    case MCAST_CKPT_TYPE_MFC_ENTRY:
      mcastCkptEntryMsgProcess(entryData, entryLength);
      break;

    default:
      MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "Invalid checkpoint type %d\n", tlvType);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_MCAST_MAP_COMPONENT_ID,
              "Invalid checkpoint type %d", tlvType);
      break;
    }
  }
}

/*********************************************************************
* @purpose  Clear all checkpointed data.
*
* @param    void
*
* @returns  void
*
* @notes    Can be called when a new backup unit is reported. Can also
*           happen if we do a cold restart and need to throw away any
*           data that happened to be checkpointed.
*
* @end
*********************************************************************/
void mcastCkptFlush(L7_BOOL purge)
{
  mcastCkptRecord_t   entryKey, *pEntry;
  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  memset(&entryKey, 0, sizeof(entryKey));
  pEntry = &entryKey;

  while (mcastCkptEntryNextGet(pEntry, &pEntry) == L7_SUCCESS)
  {
    if ((purge == L7_TRUE) ||
        (pEntry->ckptFlags & MCAST_CKPT_FLAG_DELETE_PENDING))
    {
      mcastCkptEntryRemove(pEntry);
    }
    else
    {
      pEntry->ckptFlags = 0;
    }
  }
}


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
* @notes    This function executes while MCAST is in WMU state. It runs
*           on the checkpoint service thread.
*
* @end
*********************************************************************/
void mcastCkptBackupCallback(L7_CKPT_BACKUP_MGR_CALLBACK_t  callbackType,
                             L7_uchar8                     *ckptData, 
                             L7_uint32                      dataLen)
{
  mcastCkptDbSemaTake();

  switch (callbackType)
  {
  case CKPT_BACKUP_SYNC_DATA:
    mcastCkptMsgProcess(ckptData, dataLen);
    break;

  case CKPT_BACKUP_NEW_MANAGER:
    mcastCkptFlush(L7_TRUE);
    break;

  default:
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_MCAST_MAP_COMPONENT_ID,
            "Unexpected checkpoint service callback type %d.",
            callbackType);
  }

  mcastCkptDbSemaGive();

  return;

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
L7_RC_t mcastCkptCallbacksRegister(void)
{                       
  /* don't register if NSF feature not supported */
  if (!cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID))
    return L7_SUCCESS;

  if (ckptMgrCheckpointCallbackRegister(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                        mcastCkptManagerCallback) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_MCAST_MAP_COMPONENT_ID,
            "Failed to register with checkpoint callback for management unit.");
    return L7_FAILURE;
  }

  if (ckptBackupMgrCheckpointCallbackRegister(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                              mcastCkptBackupCallback) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_MCAST_MAP_COMPONENT_ID,
            "Failed to register with checkpoint callback for backup unit.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Indicate that a MCAST checkpoint is now pending.
*
* @param  void
*
* @returns  L7_TRUE if a MCAST checkpoint is already pending.
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL mcastCkptIsPending()
{
  L7_BOOL ckptAlreadyPending = L7_FALSE;

  if (osapiSemaTake(pMcastCkptInfo->mcastCkptPendingSema, L7_NO_WAIT) != L7_SUCCESS)
  {
    ckptAlreadyPending = L7_TRUE;
  }

  return ckptAlreadyPending;
}

/*********************************************************************
* @purpose  Indicate that a MCAST checkpoint is not pending.
*
* @param 
*
* @returns 
*
* @notes  
*
* @end
*********************************************************************/
void mcastCkptIsNotPending()
{
  osapiSemaGive(pMcastCkptInfo->mcastCkptPendingSema);
}

/*********************************************************************
* @purpose  Applies the checkpointed data to the operational database.
*
* @param 
*
* @returns  L7_RC_t
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mcastCkptApplyData()
{
  mcastCkptRecord_t mcastCkptData;
  mcastCkptRecord_t *mcastCkptEntry;
  L7_RC_t rc = L7_SUCCESS;

  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  mcastCkptDbSemaTake();

  pMcastCkptInfo->mcastCkptBackupManagerElected = L7_FALSE;

  memset(&mcastCkptData, 0, sizeof(mcastCkptData));
  mcastCkptEntry = &mcastCkptData;

  while (mcastCkptEntryNextGet(mcastCkptEntry, &mcastCkptEntry) == L7_SUCCESS)
  {
    mfcEntry_t     mfcEntry;
    struct rtmbuf  m;
    L7_uint32      rtrIntfNum;

    if ((rc = ipMapIntIfNumToRtrIntf(mcastCkptEntry->intIfNum, &rtrIntfNum)) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  failure from ipMapIntIfNumToRtrIntf rc=%d, intIfNum\n", 
                      rc, mcastCkptEntry->intIfNum);
      continue;
    }
    memset(&mfcEntry, 0, sizeof(mfcEntry));
    memset(&m, 0, sizeof(m));

    mfcEntry.mcastProtocol = L7_MRP_UNKNOWN;
    inetCopy(&mfcEntry.source, &mcastCkptEntry->source);
    inetCopy(&mfcEntry.group, &mcastCkptEntry->group);
    mfcEntry.iif = rtrIntfNum;
    mfcEntry.dataTTL = 0;  

    mfcEntry.m   = &m;
    m.rtm_pkthdr.rxPort = mcastCkptEntry->rxPort;

    rc = mfcMroutePktForward(&mfcEntry);
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "mfcMroutePktForward() returns rc=%d\n", rc);
  }

  mcastCkptDbSemaGive();
  return rc;
}

/*********************************************************************
* @purpose  Indicates if a backup manager has been elected.
*
* @param 
*
* @returns L7_BOOL
*
* @notes  
*
* @end
*********************************************************************/
L7_BOOL mcastCkptBackupManagerIsElected()
{
  return pMcastCkptInfo->mcastCkptBackupManagerElected;
}

/*********************************************************************
* @purpose  If a checkpoint is not already pending, start a checkpoint.
*
* @param    forceCheckpoint - If L7_TRUE, then force a checkpoint even
*                             if there is already a checkpoint pending.
*
* @returns L7_RC_t
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mcastCkptStart(L7_BOOL forceCheckpoint)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL ckptAlreadyPending;

  ckptAlreadyPending = mcastCkptIsPending();

  if ((ckptAlreadyPending == L7_FALSE) || (forceCheckpoint == L7_TRUE))
  {
    rc = ckptMgrCheckpointStart(L7_FLEX_MCAST_MAP_COMPONENT_ID);
    if (rc != L7_SUCCESS)
    {
      mcastCkptIsNotPending();
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Compare two mcast check point entries.
*
* @param    pData1   @b{(input)}pointer to the first entry
* @param    pData2   @b{(input)}pointer to the second entry
* @param    size     @b{(input)}size of the key in each entry (ignored as of now).
*
* @returns  >0, if pData1 is greater than pData2.
* @returns  =0, if pData1 is equal to pData2.
* @returns  <0, if pData1 is less than pData2.
*
* @comments
*
* @end
*
*********************************************************************/
static 
L7_int32 mcastCkptEntryCompare(const void *pData1, const void *pData2, 
                               L7_uint32 size)
{
  mcastCkptRecord_t *pKey1 = (mcastCkptRecord_t *)pData1;
  mcastCkptRecord_t *pKey2 = (mcastCkptRecord_t *)pData2;
  register L7_int32 retVal = 0;

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  retVal = L7_INET_ADDR_COMPARE(&(pKey1->source), &(pKey2->source));

  if (retVal != 0)
  {
    return retVal;
  }

  return(L7_INET_ADDR_COMPARE(&(pKey1->group), &(pKey2->group)));
}

/*********************************************************************
* @purpose  Searches the checkpoint database for an entry matching
*           the data provided.
*
* @param    *mcastCkptData   - record containing data to be searched for
*          **mcastCkptEntry  - updated to point to matching record if found
*
* @returns L7_RC_t          - L7_SUCCESS if matching entry found
*                             L7_FAILURE if no matching entry found
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mcastCkptEntryLookup(mcastCkptRecord_t *mcastCkptData,
                             mcastCkptRecord_t **mcastCkptEntry)
{
  L7_RC_t rc = L7_FAILURE;
  mcastCkptRecord_t *pEntry;

  if ((pEntry = avlSearchLVL7(&pMcastCkptInfo->mcastCkptAvlTree, mcastCkptData, AVL_EXACT)) != L7_NULL)
  {
    *mcastCkptEntry = pEntry;
    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Searches the checkpoint database for the next entry after
*           the data provided.
*
* @param    *mcastCkptData   - record containing data to be searched for
*          **mcastCkptEntry  - updated to point to matching record if found
*
* @returns L7_RC_t          - L7_SUCCESS if matching entry found
*                             L7_FAILURE if no matching entry found
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mcastCkptEntryNextGet(mcastCkptRecord_t *mcastCkptData,
                              mcastCkptRecord_t **mcastCkptEntry)
{
  L7_RC_t rc = L7_FAILURE;
  mcastCkptRecord_t *pEntry;

  if ((pEntry = avlSearchLVL7(&pMcastCkptInfo->mcastCkptAvlTree, mcastCkptData, AVL_NEXT)) != L7_NULL)
  {
    *mcastCkptEntry = pEntry;
    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Allocates and populates an entry in the checkpoint database for 
*           the data provided.
*
* @param    *mcastCkptData   - record containing data to be added
*
* @returns L7_RC_t          - L7_SUCCESS if entry added
*                             L7_FAILURE if entry not added
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mcastCkptEntryAdd(mcastCkptRecord_t *mcastCkptData)
{
  L7_RC_t rc = L7_FAILURE;

  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  if (avlInsertEntry(&pMcastCkptInfo->mcastCkptAvlTree, mcastCkptData) == L7_NULL)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  added entry in checkpoint table\n");
    rc = L7_SUCCESS;
  }
  else
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  failed to add entry in checkpoint table\n");
  }

  return(rc);
}

/*********************************************************************
* @purpose  Allocates and populates an entry in the checkpoint database for 
*           the data provided.
*
* @param    *mcastCkptData   - record containing data to be added
*         **mcastCkptEntry   - if successfully added, points to record 
*
* @returns L7_RC_t          - L7_SUCCESS if entry added
*                             L7_FAILURE if entry not added
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mcastCkptEntryRemove(mcastCkptRecord_t *mcastCkptData)
{
  L7_RC_t rc = L7_FAILURE;

  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  if (avlDeleteEntry(&pMcastCkptInfo->mcastCkptAvlTree, mcastCkptData) != L7_NULL)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  entry deleted successfully\n");
    rc = L7_SUCCESS;
  }
  else
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  failed to find entry in checkpoint table\n");
  }

  return(rc);
}

/*********************************************************************
* @purpose Use the checkpoint service to sync mcast info.
*
* @param   L7_BOOL          checkpointImmediately 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mcastCkptEntrySync(L7_inet_addr_t *source,
                           L7_inet_addr_t *group,
                           L7_uint32       iif,
                           L7_uint32       rxPort,
                           L7_BOOL         checkpointImmediately)
{
  L7_RC_t            rc;
  mcastCkptRecord_t  mcastCkptData;
  mcastCkptRecord_t *mcastCkptEntry;
  L7_uint32          incomingIntIfNum;

  mcastCkptDbSemaTake();

  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  memset(&mcastCkptData, 0, sizeof(mcastCkptData));
  inetCopy(&mcastCkptData.source, source);
  inetCopy(&mcastCkptData.group, group);

  /* convert iif which is a router interface index into NIM internal interface number for checkpoining, 
   * internal interface numbers are preserved across a fail-over while router interface numbers are not 
   */
  if ((rc = ipMapRtrIntfToIntIfNum(iif, &incomingIntIfNum)) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  failure from ipMapRtrIntfToIntIfNum() rc=%d, iif=%d\n", rc, iif);
    mcastCkptDbSemaGive();
    return rc;
  }


  /* See if the entry already exists. */
  rc = mcastCkptEntryLookup(&mcastCkptData, &mcastCkptEntry);

  /* If not, add the entry. */
  if (rc != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  adding new checkpoint DB entry\n");
    mcastCkptData.intIfNum = incomingIntIfNum;
    mcastCkptData.rxPort = rxPort;
    mcastCkptData.ckptFlags = MCAST_CKPT_FLAG_SYNC_PENDING;
    rc = mcastCkptEntryAdd(&mcastCkptData);
    if (rc != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  new checkpoint DB entry add failed rc=%d\n", rc);
      mcastCkptDbSemaGive();
      return rc;
    }
  }
  else
  {
    /* entry already in table, update parameters */
    mcastCkptEntry->intIfNum = incomingIntIfNum;
    mcastCkptEntry->rxPort = rxPort;
    mcastCkptEntry->ckptFlags = MCAST_CKPT_FLAG_SYNC_PENDING;
  }

  if (mcastCkptBackupManagerIsElected() == L7_FALSE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  no backup manager, returning\n");
    mcastCkptDbSemaGive();
    return L7_SUCCESS;
  }

  /* Start the checkpoint service. */
  if (checkpointImmediately)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "  calling mcastCkptStart()\n");
    rc = mcastCkptStart(L7_FALSE);
  }

  mcastCkptDbSemaGive();

  return rc;
}

/*********************************************************************
* @purpose Use the checkpoint service to sync mcast info.
*
* @param   L7_BOOL          checkpointImmediately 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mcastCkptEntryDelete(L7_inet_addr_t *source,
                             L7_inet_addr_t *group)
{
  L7_RC_t            rc = L7_SUCCESS;
  mcastCkptRecord_t  mcastCkptData;
  mcastCkptRecord_t *mcastCkptEntry;

  mcastCkptDbSemaTake();

  memset(&mcastCkptData, 0, sizeof(mcastCkptData));
  inetCopy(&mcastCkptData.source, source);
  inetCopy(&mcastCkptData.group, group);

  /* See if the entry already exists. */
  rc = mcastCkptEntryLookup(&mcastCkptData, &mcastCkptEntry);

  /* If not, add the entry. */
  if (rc != L7_SUCCESS)
  {
    mcastCkptData.ckptFlags = MCAST_CKPT_FLAG_DELETE_PENDING;
    rc = mcastCkptEntryAdd(&mcastCkptData);
    if (rc != L7_SUCCESS)
    {
      mcastCkptDbSemaGive();
      return rc;
    }
  }
  else
  {
    /* entry already present, update information */
    mcastCkptEntry->ckptFlags = MCAST_CKPT_FLAG_DELETE_PENDING;
  }

  if (mcastCkptBackupManagerIsElected() == L7_FALSE)
  {
    mcastCkptDbSemaGive();
    return L7_SUCCESS;
  }

  /* Start the checkpoint service. */
  rc = mcastCkptStart(L7_FALSE);

  mcastCkptDbSemaGive();

  return rc;
}


void mcastCkptWarmRestartTimerEvent(void)
{
  (void)mcastCkptApplyData();
}

/*********************************************************************
* @purpose  Rebuild all checkpointed data.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void mcastCkptRebuild()
{
  mcastCkptRecord_t   entryKey, *pEntry;
  MCAST_MAP_DEBUG(MCAST_MAP_CKPT_EVENTS, "invoked\n");

  memset(&entryKey, 0, sizeof(entryKey));
  pEntry = &entryKey;

  while (mcastCkptEntryNextGet(pEntry, &pEntry) == L7_SUCCESS)
  {
    pEntry->ckptFlags = MCAST_CKPT_FLAG_SYNC_PENDING;
  }
}

/*********************************************************************
* @purpose  Debug routine to dump the MCAST checkpoint data.
*
* @param 
*
* @returns 
*
* @notes  
*
* @end
*********************************************************************/
void mcastCkptRecordDump(mcastCkptRecord_t *pRecord)
{
  L7_uchar8            sourceIpAddrString[64];     /* leave plenty of room for IPv6 address strings */
  L7_uchar8            groupIpAddrString[64];

  if ((osapiInetNtop(pRecord->source.family, (L7_uchar8 *)&pRecord->source.addr, sourceIpAddrString, sizeof(sourceIpAddrString)) != L7_NULLPTR) && 
      (osapiInetNtop(pRecord->group.family, (L7_uchar8 *)&pRecord->group.addr, groupIpAddrString, sizeof(groupIpAddrString)) != L7_NULLPTR))
  {
    sysapiPrintf("  source = %s, group = %s, intIfNum = %d, rxPort = %d, flags = %x\n",
                 sourceIpAddrString,
                 groupIpAddrString,
                 pRecord->intIfNum,
                 pRecord->rxPort,
                 pRecord->ckptFlags);
  }
  else
  {
    sysapiPrintf("mcastCkptRecordDump - ERROR: on conversion of source or group address record to string\n");
  }
}

void mcastCkptDBDump(L7_uint32 detail)
{
  sysapiPrintf("Multicast Forwarding DB Checkpoint Data Table\n");

  if (pMcastCkptInfo->mcastCkptAvlTree.semId == L7_NULL)
  {
    sysapiPrintf("  AVL tree not allocated\n");
    return;
  }

  sysapiPrintf("  table memory: %u entries, %u bytes/entry, %u bytes total (plus AVL tree overhead)\n",               
               pMcastCkptInfo->mcastCkptAvlTreeMaxEntries, 
               sizeof(mcastCkptRecord_t), 
               sizeof(mcastCkptRecord_t) * pMcastCkptInfo->mcastCkptAvlTreeMaxEntries);

  sysapiPrintf("  table entries in use = %u\n\n", avlTreeCount(&pMcastCkptInfo->mcastCkptAvlTree));

  if (detail != 0)
  {
    mcastCkptRecord_t entryKey, *pEntry;

    memset(&entryKey, 0, sizeof(entryKey));
    pEntry = &entryKey;

    while (mcastCkptEntryNextGet(pEntry, &pEntry) == L7_SUCCESS)
    {
      mcastCkptRecordDump(pEntry);
    }
  }
  else
  {
    sysapiPrintf("\nUse devshell mcastCkptDBDump(1) to see contents of entries\n");
  }

}

void dumpBuffer(L7_uchar8 *buffer, L7_uint32 length)
{
  L7_uint32 count;

  sysapiPrintf("\n     -  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
  sysapiPrintf("\n-------------------------------------------------------");
  for (count = 0; count < length; count++)
  {
    if (count%16 == 0)
    {
      /* start of new row */
      sysapiPrintf("\n%04x - ", count);
    }
    sysapiPrintf("%02x ", buffer[count]);

  }
  sysapiPrintf("\n");
}
#endif   /* L7_NSF_PACKAGE */
