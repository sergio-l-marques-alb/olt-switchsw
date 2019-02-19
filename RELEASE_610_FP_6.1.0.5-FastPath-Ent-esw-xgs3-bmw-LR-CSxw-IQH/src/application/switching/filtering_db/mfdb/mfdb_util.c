/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\layer2\mfdb\mfdb_util.c
*
* @purpose Multicast Forwarding Database Utilities File
*
* @component Multicast Forwarding Database
*
* @comments None
*
* @create 03/20/2002
*
* @author rjindal
*
* @end
*
*********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <string.h>
#include "flex.h"
#include "l7_common.h"
#include "comm_mask.h"
#include "osapi.h"
#include "nimapi.h"
#include "log.h"
#include "dtlapi.h"
#include "avl_api.h"
#include "defaultconfig.h"
#include "mfdb_api.h"
#include "mfdb.h"


avlTree_t           mfdbTree;
avlTreeTables_t     *mfdbTreeHeap = L7_NULLPTR;
mfdbData_t          *mfdbDataHeap = L7_NULLPTR;
mfdbStats_t         mfdbStats;
mfdbNotifyRequest_t *mfdbNotifyRequest = L7_NULLPTR;
extern mfdbCnfgrState_t mfdbCnfgrState;


/*********************************************************************
*
* @purpose  To find a VLANID-MAC entry in the MFDB table.
*
* @parm     mfdbData_t  *pData     pointer to the data structure
* @param    L7_uint32   matchType  L7_MATCH_EXACT or L7_MATCH_GETNEXT
*
* @returns  L7_SUCCESS, if the entry is found
* @returns  L7_NOT_EXIST, if the entry is not found
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryFind(mfdbData_t *pData, L7_uint32 matchType)
{
  mfdbData_t *ptrData;

  ptrData = avlSearchLVL7(&mfdbTree, pData->mfdbVidMacAddr, matchType);
  if (ptrData != L7_NULL)
  {
    memcpy(pData, ptrData, sizeof(mfdbData_t));
    return L7_SUCCESS;
  }

  return L7_NOT_EXIST;
}


/*********************************************************************
*
* @purpose  To insert a VLANID-MAC entry in the avl tree.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if the entry is inserted
* @returns  L7_FAILURE, if the entry is not inserted
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbTreeEntryInsert(mfdbData_t *pData)
{
  L7_uint32 currEntries;
  mfdbData_t *ptrData;

  (void)osapiSemaTake(mfdbTree.semId, L7_WAIT_FOREVER);
  ptrData = avlInsertEntry(&mfdbTree, pData);
  (void)osapiSemaGive(mfdbTree.semId);
    
  if (ptrData == L7_NULL)
  {
    currEntries = mfdbTree.count;
    if (currEntries > mfdbStats.most_entries)
    {
      mfdbStats.most_entries = currEntries;
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  To delete a VLANID-MAC entry from the avl tree.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if the entry is deleted
* @returns  L7_NOT_EXIST, if the entry is not found
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbTreeEntryDelete(mfdbData_t *pData)
{
  mfdbData_t *ptrData;

  (void)osapiSemaTake(mfdbTree.semId, L7_WAIT_FOREVER);
  ptrData = avlDeleteEntry(&mfdbTree, pData);
  (void)osapiSemaGive(mfdbTree.semId);

  if (ptrData != L7_NULL)
  {
    return L7_SUCCESS;
  }

  LOG_MSG("mfdbTreeEntryDelete: entry does not exist\n");
  return L7_NOT_EXIST;
}


/*********************************************************************
*
* @purpose  To update a VLANID-MAC entry in the avl tree.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if the entry is updated
* @returns  L7_FAILURE, if the entry is not found
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbTreeEntryUpdate(mfdbData_t *pData)
{
  mfdbData_t *ptrData;

  (void)osapiSemaTake(mfdbTree.semId, L7_WAIT_FOREVER);
  ptrData = avlSearchLVL7(&mfdbTree, pData->mfdbVidMacAddr, L7_MATCH_EXACT);
  if (ptrData != L7_NULL)
  {
    memcpy(ptrData, pData, sizeof(mfdbData_t));
    (void)osapiSemaGive(mfdbTree.semId);

    return L7_SUCCESS;
  }
  (void)osapiSemaGive(mfdbTree.semId);

  L7_LOG(L7_LOG_SEVERITY_INFO, L7_MFDB_COMPONENT_ID,
      "mfdbTreeEntryUpdate: entry does not exist."
      " Trying to update a non existing entry");
  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  To add a VLANID-MAC entry to the NPU.
*           This function is also used to modify an existing entry.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if entry is added (or modified)
* @returns  L7_FAILURE, if entry is not added (or modified)
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbDtlAdd(mfdbData_t *pData)
{ 
  L7_uint32 vlanId;
  L7_ushort16 vid;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_RC_t rc;
  L7_INTF_MASK_t absMask;
  DTL_L2_MCAST_ENTRY_t grpEntry;

  memcpy(&vid, pData->mfdbVidMacAddr, L7_MFDB_VLANID_LEN);
  vlanId = (L7_uint32)vid;
  memset(mac, 0x00, L7_MAC_ADDR_LEN);
  memcpy(mac, &pData->mfdbVidMacAddr[L7_MFDB_VLANID_LEN], L7_MAC_ADDR_LEN);

  memset(&grpEntry, 0, sizeof(DTL_L2_MCAST_ENTRY_t));
  grpEntry.vlanId = vlanId;
  grpEntry.macAddr = mac;
  rc = mfdbEntryAbsolutePortMaskGet(pData->mfdbVidMacAddr, &absMask);

  grpEntry.fwdMask = (NIM_INTF_MASK_t *)&absMask;
  grpEntry.filterMask = L7_NULLPTR;
  grpEntry.type = DTL_L2_MCAST_ABSOLUTE;
  
  /* call dtl */
  rc = dtlL2McastEntryAdd(&grpEntry);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("mfdbDtlAdd: dtlL2McastEntryAdd failed\n");
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  To remove a VLANID-MAC entry from the NPU.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if entry is removed
* @returns  L7_FAILURE, if entry is not removed
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbDtlRemove(mfdbData_t *pData)
{
  L7_uint32 vlanId;
  L7_ushort16 vid;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_RC_t rc = L7_SUCCESS;

  memcpy(&vid, pData->mfdbVidMacAddr, L7_MFDB_VLANID_LEN);
  vlanId = (L7_uint32)vid;
  memset(mac, 0x00, L7_MAC_ADDR_LEN);
  memcpy(mac, &pData->mfdbVidMacAddr[L7_MFDB_VLANID_LEN], L7_MAC_ADDR_LEN);

  /* call dtl */
rc=dtlL2McastEntryDelete(vlanId,mac,DTL_L2_MCAST_ABSOLUTE);
if (rc != L7_SUCCESS)
   {
    LOG_MSG("mfdbDtlRemove: dtlL2McastEntryDelete failed\n");
   }

  return rc;
}


/*********************************************************************
*
* @purpose  To get the maximum number of entries that the MFDB table
*           can hold.
*
* @param    L7_uint32  *maxEntries  total number of entries
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbMaxTableEntriesGet(L7_uint32 *maxEntries)
{
  *maxEntries = L7_MFDB_MAX_MAC_ENTRIES;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To get the most number of entries ever in the MFDB table.
*
* @param    L7_uint32  *mostEntries  most number of entries ever
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbMostEntriesGet(L7_uint32 *mostEntries)
{
  *mostEntries = mfdbStats.most_entries;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To get the current number of entries in the MFDB table.
*
* @param    L7_uint32  *currEntries  number of current entries
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbCurrEntriesGet(L7_uint32 *currEntries)
{
  *currEntries = mfdbTree.count;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mfdbCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mfdbRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;
  mfdbRC     = L7_SUCCESS;

  mfdbTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_MFDB_COMPONENT_ID, (sizeof(avlTreeTables_t))*L7_MFDB_MAX_MAC_ENTRIES);
  mfdbDataHeap = (mfdbData_t *)osapiMalloc(L7_MFDB_COMPONENT_ID, (sizeof(mfdbData_t))*L7_MFDB_MAX_MAC_ENTRIES);
  mfdbNotifyRequest = (mfdbNotifyRequest_t *)osapiMalloc(L7_MFDB_COMPONENT_ID, (L7_uint32)((sizeof(mfdbNotifyRequest_t))*
                                                                     L7_LAST_COMPONENT_ID));
  if (mfdbTreeHeap      == L7_NULLPTR ||
      mfdbDataHeap      == L7_NULLPTR ||
      mfdbNotifyRequest == L7_NULLPTR)
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    mfdbRC     = L7_ERROR;

    return mfdbRC;
  }

  memset((void *)mfdbTreeHeap, 0, (sizeof(avlTreeTables_t))*L7_MFDB_MAX_MAC_ENTRIES);
  memset((void *)mfdbDataHeap, 0, (sizeof(mfdbData_t))*L7_MFDB_MAX_MAC_ENTRIES);
  memset((void *)mfdbNotifyRequest, 0, sizeof(mfdbNotifyRequest_t) * L7_LAST_COMPONENT_ID);

  avlCreateAvlTree(&mfdbTree, mfdbTreeHeap, mfdbDataHeap, L7_MFDB_MAX_MAC_ENTRIES, 
                   (L7_uint32)sizeof(mfdbData_t), 0x10, sizeof(L7_uchar8)*L7_MFDB_KEY_SIZE);

  mfdbCnfgrState = MFDB_PHASE_INIT_1;

  return mfdbRC;

}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* 
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mfdbCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mfdbRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;
  mfdbRC     = L7_SUCCESS;

  mfdbCnfgrState = MFDB_PHASE_INIT_2;

  return mfdbRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mfdbCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mfdbRC;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;
  mfdbRC     = L7_SUCCESS;

  mfdbCnfgrState = MFDB_PHASE_INIT_3;

  return mfdbRC;
}


/*********************************************************************
* @purpose  This function undoes mfdbCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mfdbCnfgrFiniPhase1Process()
{
  if (mfdbTreeHeap != L7_NULLPTR)
    osapiFree(L7_MFDB_COMPONENT_ID, (void *)mfdbTreeHeap);

  if (mfdbDataHeap != L7_NULLPTR)
    osapiFree(L7_MFDB_COMPONENT_ID, (void *)mfdbDataHeap);

  if (mfdbNotifyRequest != L7_NULLPTR)
    osapiFree(L7_MFDB_COMPONENT_ID, (void *)mfdbNotifyRequest);

  avlPurgeAvlTree(&mfdbTree, L7_MFDB_MAX_MAC_ENTRIES);
  if (mfdbTree.semId != L7_NULLPTR)
    if (osapiSemaDelete((void *)mfdbTree.semId) != L7_SUCCESS)
      LOG_MSG("mfdbCnfgrFiniPhase1Process:  could not delete avl semaphore\n");

  mfdbCnfgrState = MFDB_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function undoes mfdbCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mfdbCnfgrFiniPhase2Process()
{
  mfdbCnfgrState = MFDB_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes mfdbCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mfdbCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func should place mfdbCnfgrState to WMU */
  (void)mfdbCnfgrUconfigPhase2(&response, &reason);
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse - @b{(output)}  Response always command complete.
*     
* @param    pReason   - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mfdbCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mfdbRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;
  return(mfdbRC);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/

L7_RC_t mfdbCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mfdbRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  mfdbRC      = L7_SUCCESS;

  mfdbCnfgrState = MFDB_PHASE_WMU;

  return mfdbRC;
}

/*********************************************************************
*
* @purpose  To notify all the MFDB user components of any change
*           in the MFDB table.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
* @param    L7_uint32   index   index of the componentId
* @param    L7_uint32   event   the event notified to all users:
*                                        MFDB_ENTRY_ADD_NOTIFY,
*                                        MFDB_ENTRY_DELETE_NOTIFY,
*                                        MFDB_ENTRY_PORT_ADD_NOTIFY,
*                                        MFDB_ENTRY_PORT_DELETE_NOTIFY
*
* @returns  Void.
*
* @comments None.
*       
* @end
*
*********************************************************************/
void mfdbUserComponentsNotify(mfdbData_t *pData, L7_uint32 index, L7_uint32 event)
{
  L7_uint32 i;
  L7_uint32 vlanId;
  L7_ushort16 vid;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_RC_t rc;

  memcpy(&vid, pData->mfdbVidMacAddr, L7_MFDB_VLANID_LEN);
  vlanId = (L7_uint32)vid;
  memcpy(mac, &pData->mfdbVidMacAddr[L7_MFDB_VLANID_LEN], L7_MAC_ADDR_LEN);

  for (i = 1; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (mfdbNotifyRequest[i].registrarID != L7_NULL)
    {
      rc = (mfdbNotifyRequest[i].notify_request(vlanId, mac, 
                                                pData->mfdbUser[index].fwdPorts, 
                                                pData->mfdbUser[index].fltPorts, 
                                                event, 
                                                pData->mfdbUser[index].componentId));
      if (rc != L7_SUCCESS)
        LOG_MSG("Bad rc %d in mfdbUserComponentsNotify for component %d\n", rc,
                (L7_int32)pData->mfdbUser[index].componentId);
    }
  }
}


/*********************************************************************
*
* @purpose  To register a routine to be called when an MFDB table
*           entry is created, deleted, or modified.
*
* @param    *notify  Notification routine with the following parms:
*                    @param  L7_uint32       vlanId   entry vlan id
*                    @param  L7_uchar8       *mac     mac address
*                    @param  L7_INTF_MASK_t  fwdMask  forward mask
*                    @param  L7_INTF_MASK_t  fltMask  filter mask
*                    @param  L7_uint32       event    the event:
*                                       MFDB_ENTRY_ADD_NOTIFY,
*                                       MFDB_ENTRY_DELETE_NOTIFY,
*                                       MFDB_ENTRY_PORT_ADD_NOTIFY, or
*                                       MFDB_ENTRY_PORT_DELETE_NOTIFY
*                    @param  L7_uint32         userId   component's id
*
* @param    componentID  user component's ID (see L7_COMPONENT_IDS_t)
*                                       
* @returns  L7_SUCCESS, if notification is successful
* @returns  L7_FAILURE, if invalid component ID
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbRegisterForEntryChange(L7_RC_t (*notify)(L7_uint32 vlanId, 
                                                     L7_uchar8 *mac, 
                                                     L7_INTF_MASK_t fwdMask, 
                                                     L7_INTF_MASK_t fltMask, 
                                                     L7_uint32 event, 
                                                     L7_uint32 userID), 
                                                    L7_COMPONENT_IDS_t componentID)
{

  bzero((L7_uchar8 *)mfdbNotifyRequest, sizeof(mfdbNotifyRequest));

  if (componentID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("MFDB: component ID %d greater than L7_LAST_COMPONENT_ID\n", (L7_int32)componentID);
    return L7_FAILURE;
  }
  
  else
  {
    mfdbNotifyRequest[componentID].registrarID = componentID;
    mfdbNotifyRequest[componentID].notify_request = notify;
    
    /* Future: save function name */
    bzero(mfdbNotifyRequest[componentID].func_name, MFDB_MAX_FUNC_NAME);

    return L7_SUCCESS;
  }
}

/*********************************************************************
*
* @purpose  To combine a given VLANID and MAC address to get an
*           8-byte VLANID-MAC address combination.
*
* @param    L7_uint32  vlanId   vlan id
* @param    L7_uchar8  *mac     pointer to mac address
* @param    L7_uchar8  *vidMac  pointer to vid-mac address combination
*                               (2-byte vlan id + 6-byte mac address)
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryVidMacCombine(L7_uint32 vlanId, L7_uchar8 *mac, L7_uchar8 *vidMac)
{
  L7_ushort16 vid;
  L7_uchar8 tmp[L7_MFDB_KEY_SIZE];

  memset((void *)tmp, 0x00, L7_MFDB_KEY_SIZE);
  
  vid = (L7_ushort16)vlanId;
  memcpy(tmp, &vid, L7_MFDB_VLANID_LEN);
  memcpy(&tmp[L7_MFDB_VLANID_LEN], mac, L7_MAC_ADDR_LEN);
  
  memcpy(vidMac, tmp, L7_MFDB_KEY_SIZE);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To separate an 8-byte VLANID-MAC address combination into
*           a VLANID and MAC address.
*
* @param    L7_uchar8  *vidMac  pointer to vid-mac address combination
*                               (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *vlanId  pointer to the vlan id
* @param    L7_uchar8  *mac     pointer to mac address
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryVidMacSeparate(L7_uchar8 *vidMac, L7_uint32 *vlanId, L7_uchar8 *mac)
{
  L7_ushort16 vid;
  L7_uchar8 tmp[L7_MAC_ADDR_LEN];

  memset((void *)tmp, 0x00, L7_MAC_ADDR_LEN);
  
  memcpy(&vid, vidMac, L7_MFDB_VLANID_LEN);
  *vlanId = (L7_uint32)vid;
  memcpy(tmp, &vidMac[L7_MFDB_VLANID_LEN], L7_MAC_ADDR_LEN);

  memcpy(mac, tmp, L7_MAC_ADDR_LEN);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To map a specific protocol to its component id.
*
* @param    L7_uint32  protocol  the protocol type
*                                (see L7_MFDB_PROTOCOL_TYPE_t)   
* @param    L7_uint32  *compId   pointer to user component id
*                                (see L7_COMPONENT_IDS_t)   
*
* @returns  Void.
*
* @returns  L7_SUCCESS, if the protocol is mapped to the component id
* @returns  L7_FAILURE, if invalid protocol type
*
* @end
*
*********************************************************************/
L7_RC_t mfdbProtocolToCompID(L7_uint32 protocol, L7_COMPONENT_IDS_t *compId)
{
  /* check validity of the protocol type */
  if (protocol >= L7_MFDB_TOTAL_NUM_PROTOCOLS)
  {
    return L7_FAILURE;
  }
  
  else
  {
    switch (protocol)
    {
    case L7_MFDB_PROTOCOL_STATIC:
      *compId = L7_FILTER_COMPONENT_ID;
      break;

    case L7_MFDB_PROTOCOL_GMRP:
      *compId = L7_GMRP_COMPONENT_ID;
      break;

    case L7_MFDB_PROTOCOL_IGMP:
      *compId = L7_SNOOPING_COMPONENT_ID;
      break;

    default:
      *compId = L7_NULL;
      break;
    }

    return L7_SUCCESS;
  }
}

