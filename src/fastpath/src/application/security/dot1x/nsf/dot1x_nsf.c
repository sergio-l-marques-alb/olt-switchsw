/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_ih.h
*
* @purpose   dot1x Interface Handler
*
* @component dot1x
*
* @comments  none
*
* @create    01/31/2003
*
* @author    mfiorito
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "dot1x_nsf.h"
#include "dot1x_cfg.h"
#include "osapi_support.h"
#include "dot1x_api.h"
#include "dot1x_debug.h"
#include "dot1x_include.h"

static avlTree_t                dot1xCkptDataTreeDb;
static avlTreeTables_t *dot1xCkptDataTreeHeap;
static dot1xLPortCheckPointData_t *dot1xCkptLPortDataHeap;
static void *dot1xCkptMaskSema = L7_NULLPTR;
static L7_BOOL dot1xBackupMgrElected = L7_FALSE;
static dot1xPhyPortCheckPointData_t *dot1xCkptPhyPort = L7_NULLPTR;
static L7_INTF_MASK_t dot1xPhyPortCkptMask;

extern L7_BOOL dot1xSwitchoverInProgress;


/*********************************************************************
* @purpose Initialize the Non stop forwarding sub system for dot1x
*
* @param    None
*
* @returns  L7_SUCCESS   on success
*           L7_FAILURE   otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xNsfInit()
{
  L7_uint32 nodeCount = dot1xMaxAllocMemGet();
  /* Allocate the Heap structures */
  dot1xCkptDataTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_DOT1X_COMPONENT_ID,
                                           nodeCount * sizeof(avlTreeTables_t));

  dot1xCkptLPortDataHeap  = (dot1xLPortCheckPointData_t *)osapiMalloc(L7_DOT1X_COMPONENT_ID,
                                           nodeCount * sizeof(dot1xLPortCheckPointData_t));

  /* validate the pointers*/
  if ((dot1xCkptDataTreeHeap == L7_NULLPTR) ||(dot1xCkptLPortDataHeap == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1X_COMPONENT_ID,
           "dot1xLogicalPortInfoDBInit: Error allocating data for dot1x DB Tree."
           " Error allocating memory for the Dot1x database. Possible cause Insufficient memory.");
    return L7_FAILURE;
  }

  dot1xCkptPhyPort  = osapiMalloc(L7_DOT1X_COMPONENT_ID,
                 sizeof(dot1xPhyPortCheckPointData_t) * L7_DOT1X_INTF_MAX_COUNT);
  if (dot1xCkptPhyPort == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1X_COMPONENT_ID,
           " Error allocating memory for the Dot1x database. Possible cause Insufficient memory.");
    return L7_FAILURE;
  }

  memset(dot1xCkptPhyPort, 0, sizeof(dot1xPhyPortCheckPointData_t) * L7_DOT1X_INTF_MAX_COUNT);
  memset(&dot1xPhyPortCkptMask, 0, sizeof(dot1xPhyPortCkptMask));

  /* AVL Tree creations - dot1xLogicalPortTreeDb*/
  avlCreateAvlTree(&(dot1xCkptDataTreeDb),  dot1xCkptDataTreeHeap,
                   dot1xCkptLPortDataHeap, nodeCount,
                   sizeof(dot1xLPortCheckPointData_t), 0x10,
                   sizeof(L7_uint32));

  dot1xCkptMaskSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (dot1xCkptMaskSema == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
            "Unable to create dot1s checkpoint semaphore()\n");
    return(L7_FAILURE);
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose Release the resources acquired for the NSF subsytem of dot1x
*
* @param    None
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void dot1xNsfFini()
{
  avlDeleteAvlTree(&dot1xCkptDataTreeDb);
  /* Give up the memory */
  osapiFree(L7_DOT1X_COMPONENT_ID, dot1xCkptDataTreeHeap);
  dot1xCkptDataTreeHeap = L7_NULLPTR;

  osapiFree(L7_DOT1X_COMPONENT_ID, dot1xCkptLPortDataHeap);
  dot1xCkptLPortDataHeap = L7_NULLPTR;

  osapiFree(L7_DOT1X_COMPONENT_ID, dot1xCkptPhyPort);
  dot1xCkptPhyPort = L7_NULLPTR;

  osapiSemaDelete(dot1xCkptMaskSema);
}

/*********************************************************************
* @purpose Initialize the Non stop forwarding sub system for dot1s
*
* @param    None
*
* @returns  L7_SUCCESS   on success
*           L7_NO_MEM    if memory not sufficient
*           L7_FAILURE   otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xNsfCallbacksRegister()
{
  L7_RC_t rc;
  if ((rc =ckptMgrCheckpointCallbackRegister(L7_DOT1X_COMPONENT_ID,
                                             dot1xManagerCheckpoint)) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Failed to register to check point service\n");
  }
  else if ((rc =ckptBackupMgrCheckpointCallbackRegister(L7_DOT1X_COMPONENT_ID,
                                                        dot1xBackupUnitCheckpoint)) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Failed to register with check point service\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  Clean stale check pointed data (if any)
*
* @param
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xCheckPtDataReset()
{
  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
  {
    L7_uint32 nodeCount = dot1xMaxAllocMemGet();
    avlPurgeAvlTree(&dot1xCkptDataTreeDb, nodeCount);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset the check pointed data.
*
* @param
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xNsfCheckpointPrepare()
{
  L7_uint32 lIntIfNum = 0, intIfNum = 0;
  avlTree_t         *pTree = &dot1xCkptDataTreeDb;
  dot1xLPortCheckPointData_t *pDot1xCkptNode;
  L7_RC_t rc;

  while ((pDot1xCkptNode = avlSearchLVL7(pTree, &lIntIfNum, AVL_NEXT)) != L7_NULLPTR)
  {
    /* update search key for next pass */
    lIntIfNum = pDot1xCkptNode->logicalPortNumber;

    if (pDot1xCkptNode->recordType == DOT1X_CKPT_RECORD_DELETED)
    {
      /* Handle the delete */
      avlDeleteEntry(pTree, pDot1xCkptNode);
    }
    else
    {
      pDot1xCkptNode->recordType = DOT1X_CKPT_RECORD_NEW;
    }
  }

  /* Set the physical ports TLV*/
  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    if (dot1xIsValidIntf(intIfNum) == L7_TRUE)
    {
      L7_INTF_SETMASKBIT(dot1xPhyPortCkptMask, intIfNum);
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Populates the avl tree used to ckpt data with the
*           values.
*
* @param
*
* @returns  L7_SUCCESS
*
* @notes    Works only for the logical port TLVs. The physical port
*           does not need a separate function
*
* @end
*********************************************************************/
L7_RC_t dot1xNsfPopulateCkptDb(dot1xLogicalPortInfo_t *operLPortData,
                            dot1xCheckpointTlvs_t recordType)
{
  dot1xLPortCheckPointData_t tempLportCkptData, *retNode;
  avlTree_t         *pTree = &dot1xCkptDataTreeDb;
  L7_RC_t rc = L7_FAILURE;

  retNode = avlSearchLVL7(pTree, &operLPortData->logicalPortNumber,AVL_EXACT);

  if (recordType == DOT1X_LOG_PORT_DELETE_TLV)
  {
    if (retNode != L7_NULLPTR)
    {
      retNode->recordType = DOT1X_CKPT_RECORD_DELETED;
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  memset(&tempLportCkptData, 0, sizeof(tempLportCkptData));

  tempLportCkptData.logicalPortNumber = operLPortData->logicalPortNumber;
  tempLportCkptData.currentIdL = operLPortData->currentIdL;
  tempLportCkptData.logicalPortStatus = operLPortData->logicalPortStatus;
  tempLportCkptData.serverStateLen = operLPortData->serverStateLen;
  memcpy(&tempLportCkptData.serverState, &operLPortData->serverState,
         DOT1X_SERVER_STATE_LEN);
  memcpy(&tempLportCkptData.serverClass, &operLPortData->serverClass,
         DOT1X_SERVER_CLASS_LEN);
  tempLportCkptData.serverClassLen = operLPortData->serverClassLen;
  tempLportCkptData.sessionTimeout = operLPortData->sessionTimeout;
  memcpy(&tempLportCkptData.suppMacAddr, &operLPortData->suppMacAddr,
         sizeof(L7_enetMacAddr_t));
  memcpy(&tempLportCkptData.dot1xUserName, &operLPortData->dot1xUserName,
         DOT1X_USER_NAME_LEN);
  tempLportCkptData.dot1xUserNameLength = operLPortData->dot1xUserNameLength;
  memcpy(&tempLportCkptData.dot1xChallenge, &operLPortData->dot1xChallenge,
         DOT1X_CHALLENGE_LEN);
  tempLportCkptData.dot1xChallengelen = operLPortData->dot1xChallengelen;
  tempLportCkptData.terminationAction = operLPortData->terminationAction;
  tempLportCkptData.vlanId = operLPortData->vlanId;
  tempLportCkptData.blockVlanId = operLPortData->blockVlanId;
  tempLportCkptData.vlanAssigned = operLPortData->vlanAssigned;
  memcpy(&tempLportCkptData.filterName, &operLPortData->filterName,
         DOT1X_FILTER_NAME_LEN);
  tempLportCkptData.unauthVlan = operLPortData->unauthVlan;
  tempLportCkptData.guestVlanId = operLPortData->guestVlanId;
  tempLportCkptData.voiceVlanId = operLPortData->voiceVlanId;
  tempLportCkptData.isMABClient = operLPortData->isMABClient;
  tempLportCkptData.defaultVlanId = operLPortData->defaultVlanId;
  tempLportCkptData.isMonitorModeClient = operLPortData->isMonitorModeClient;

  /* Is the entry present ?*/
  if (retNode == L7_NULLPTR)
  {
    /* cannot modify a non existant entry*/
    if (recordType == DOT1X_LOG_PORT_ADD_TLV)
    {
      tempLportCkptData.recordType = DOT1X_CKPT_RECORD_NEW;
      /* If tlv type is add add the entry to the avl tree*/
      retNode  = avlInsertEntry(pTree,&tempLportCkptData);
      if (retNode == L7_NULLPTR)
      {
        rc = L7_SUCCESS;
      }
    }
  }
  else
  {
    if ((recordType == DOT1X_LOG_PORT_MODIFY_TLV) ||
        (recordType == DOT1X_LOG_PORT_ADD_TLV))
    {
      memcpy(retNode,&tempLportCkptData , sizeof(*retNode));
      retNode->recordType = DOT1X_CKPT_RECORD_UPDATE;
      rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Build a single TLV into the buffer provided
*
* @param
*
* @returns  L7_SUCCESS
*
* @notes    Assumes memory is allocated by the caller
*
* @end
*********************************************************************/
void dot1xNsfBuildLportTLV(L7_uchar8 *dot1xCkptBuff,
                       dot1xLPortCheckPointData_t *pDot1xCkptNode,
                       dot1xCheckpointTlvs_t tlvType)
{
  dot1x_nsf_tlv_header_t *tlvHeader;
  dot1xLPortCheckPointData_t *tlvCkptData;

  tlvHeader = (dot1x_nsf_tlv_header_t *)dot1xCkptBuff;
  tlvHeader->tlvType = osapiHtons(tlvType);
  switch (tlvType)
  {
    case DOT1X_LOG_PORT_ADD_TLV:
    case DOT1X_LOG_PORT_MODIFY_TLV:
      tlvHeader->index = osapiHtonl(pDot1xCkptNode->logicalPortNumber);
      tlvCkptData = (dot1xLPortCheckPointData_t *)&(tlvHeader->tlvData);
      memcpy(tlvCkptData, pDot1xCkptNode, sizeof(dot1xLPortCheckPointData_t));
      /* Endian safe */
      tlvCkptData->logicalPortNumber = osapiHtonl(tlvCkptData->logicalPortNumber);
      tlvCkptData->logicalPortStatus = osapiHtonl(tlvCkptData->logicalPortStatus);
      tlvCkptData->serverStateLen = osapiHtonl(tlvCkptData->serverStateLen);
      tlvCkptData->serverClassLen = osapiHtonl(tlvCkptData->serverClassLen);
      tlvCkptData->sessionTimeout = osapiHtonl(tlvCkptData->sessionTimeout);
      tlvCkptData->vlanId = osapiHtonl(tlvCkptData->vlanId);
      tlvCkptData->blockVlanId = osapiHtonl(tlvCkptData->blockVlanId);
      tlvCkptData->unauthVlan = osapiHtonl(tlvCkptData->unauthVlan);
      tlvCkptData->guestVlanId = osapiHtonl(tlvCkptData->guestVlanId);
      tlvCkptData->voiceVlanId = osapiHtonl(tlvCkptData->voiceVlanId);
      tlvCkptData->isMABClient = osapiHtonl(tlvCkptData->isMABClient);
      tlvCkptData->dot1xUserNameLength = osapiHtonl(tlvCkptData->dot1xUserNameLength);
      tlvCkptData->dot1xChallengelen = osapiHtonl(tlvCkptData->dot1xChallengelen);
      tlvCkptData->terminationAction = osapiHtonl(tlvCkptData->terminationAction);
      tlvCkptData->vlanAssigned = osapiHtonl(tlvCkptData->vlanAssigned);
      tlvCkptData->defaultVlanId = osapiHtonl(tlvCkptData->defaultVlanId);
      tlvCkptData->isMonitorModeClient = osapiHtonl(tlvCkptData->isMonitorModeClient);
      tlvHeader->tlvLen = DOT1X_LOG_PORT_ADD_TLV_LEN;
      break;
    case DOT1X_LOG_PORT_DELETE_TLV:
      tlvHeader->index = osapiHtonl(pDot1xCkptNode->logicalPortNumber);
      tlvHeader->tlvLen = DOT1X_LOG_PORT_DELETE_TLV_LEN;
      break;
    default:
      break;

  }
  return;
}

/*********************************************************************
* @purpose  Build the physical port TLV into the buffer provided
*
* @param
*
* @returns  L7_SUCCESS
*
* @notes    Assumes memory is allocated by the caller
*
* @end
*********************************************************************/
void dot1xNsfBuildPhyPortTLV(L7_uchar8 *dot1xCkptBuff, L7_uint32 intIfNum)
{
  dot1x_nsf_tlv_header_t *tlvHeader;
  dot1xPhyPortCheckPointData_t *tlvCkptData;

  tlvHeader = (dot1x_nsf_tlv_header_t *)dot1xCkptBuff;
  tlvHeader->tlvType = osapiHtons(DOT1X_PHY_PORT_TLV);

  tlvHeader->index = osapiHtonl(intIfNum);
  tlvCkptData = (dot1xPhyPortCheckPointData_t *)&(tlvHeader->tlvData);
  /*
  memcpy(tlvCkptData, &dot1xCkptPhyPort[intIfNum], sizeof(dot1xLPortCheckPointData_t));
  */
    /* Endian safe */
  /*
  tlvCkptData->authCount = osapiHtonl(tlvCkptData->authCount);
  tlvCkptData->portMacAddrAdded = osapiHtonl(tlvCkptData->portMacAddrAdded);
  */
  tlvCkptData->authCount = osapiHtonl(dot1xCkptPhyPort[intIfNum].authCount);
  tlvCkptData->portMacAddrAdded = osapiHtonl(dot1xCkptPhyPort[intIfNum].portMacAddrAdded);

  tlvHeader->tlvLen = DOT1X_PHY_PORT_TLV_LEN;

  return;
}


/*********************************************************************
* @purpose  Build checkpoint message into the buffer provided
*
* @param    checkpoint_data  @b{(input)}   buffer in which the message is to be populated
* @param    max_data_len    @b{(inout)}   Max len allocated for the data
* @param    ret_data_len  @b{(output)}  Actual length copied
* @param    more_data  @b{(output)}  Boolean to indicate if there is data available to checkpoint
*
*
* @returns  L7_SUCCESS
*
* @notes    Assumes memory is allocated by the caller
*
* @end
*********************************************************************/
L7_RC_t dot1xBuildCheckpointMessage(L7_uchar8 *checkpoint_data,
                                    L7_uint32 max_data_len,
                                    L7_uint32 *ret_data_len,
                                    L7_BOOL   *more_data)
{
  L7_uint32 lIntIfNum = 0;
  avlTree_t         *pTree = &dot1xCkptDataTreeDb;
  dot1xLPortCheckPointData_t *pDot1xCkptNode;
  L7_uchar8 *dot1xCkptBuff;
  dot1x_ckpt_header_t *msgHeader;
  L7_uint32 tempCurrLen;
  dot1xCheckpointTlvs_t tlvType = DOT1X_TLV_NONE;
  L7_uint32 tlvLen = 0;
  L7_INTF_MASK_t tempMask;
  L7_uint32 index;

  /* Walk through the AVL tree
     For each entry in the tree with need to checkpoint plag set or
     deleted flag set add that entry to the message
     reset the appropriate flag (if entry is deleted  remove the entry from
     the avl tree)
     if there is still data left set more_data to true
  */

  DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT_DETAIL,0, "%s:Entered\n",
                    __FUNCTION__);
  if (checkpoint_data == L7_NULLPTR)
  {
    /* log message */
    DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT_DETAIL,0, "%s:Failed, exit at line %d\n",
                      __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  *more_data = L7_FALSE; /* Initialize */
  *ret_data_len = 0;
  dot1xCkptBuff = checkpoint_data;

  msgHeader = (dot1x_ckpt_header_t *)dot1xCkptBuff;
  msgHeader->version = osapiHtons(DOT1X_CHECKPOINT_MESSAGE_VER);
  msgHeader->messageLen = 0;

  dot1xCkptBuff += sizeof(dot1x_ckpt_header_t);

  osapiSemaTake(dot1xCkptMaskSema, L7_WAIT_FOREVER);
  /* Get each element in the ckpointed tree*/
  while ((pDot1xCkptNode = avlSearchLVL7(pTree, &lIntIfNum, AVL_NEXT)) != L7_NULLPTR)
  {
    /* update search key for next pass */
    lIntIfNum = pDot1xCkptNode->logicalPortNumber;

    switch (pDot1xCkptNode->recordType)
    {
      case (DOT1X_CKPT_RECORD_DELETED):
        tlvType = DOT1X_LOG_PORT_DELETE_TLV;
        tlvLen = DOT1X_LOG_PORT_DELETE_TLV_LEN;
        break;
      case (DOT1X_CKPT_RECORD_NEW):
        tlvType = DOT1X_LOG_PORT_ADD_TLV;
        tlvLen = DOT1X_LOG_PORT_ADD_TLV_LEN;
        break;
      case (DOT1X_CKPT_RECORD_UPDATE):
        tlvType = DOT1X_LOG_PORT_MODIFY_TLV;
        tlvLen = DOT1X_LOG_PORT_ADD_TLV_LEN;
        break;
      default:
        break;
    }
    /* There is valid data to send*/
    if (tlvType != DOT1X_TLV_NONE)
    {
      tempCurrLen = dot1xCkptBuff - checkpoint_data;
      /* Do we have space left in the message*/
      if ((tempCurrLen + tlvLen)  < max_data_len )
      {
        /* Build the message based on TLV*/
        dot1xNsfBuildLportTLV(dot1xCkptBuff, pDot1xCkptNode, tlvType);
        dot1xCkptBuff += (DOT1X_TLV_HEADER_LEN + tlvLen);

        if (pDot1xCkptNode->recordType == DOT1X_CKPT_RECORD_DELETED)
        {
          avlDeleteEntry(pTree, pDot1xCkptNode);
        }
        else
        {
          pDot1xCkptNode->recordType = DOT1X_CKPT_RECORD_CKPTED;
        }
      }
      else
      {

        *more_data = L7_TRUE;
        break;
      }
    }
  }

  tlvLen = DOT1X_TLV_HEADER_LEN + DOT1X_PHY_PORT_TLV_LEN;
  memcpy(&tempMask, &dot1xPhyPortCkptMask, sizeof(tempMask));
  L7_INTF_FHMASKBIT(tempMask, index);

  while (index != 0)
  {
    tempCurrLen = dot1xCkptBuff - checkpoint_data;
    if ((tempCurrLen + tlvLen) < max_data_len)
    {
      dot1xNsfBuildPhyPortTLV(dot1xCkptBuff, index);
      dot1xCkptBuff += tlvLen;
      L7_INTF_CLRMASKBIT(dot1xPhyPortCkptMask, index);
    }
    else
    {
      *more_data = L7_TRUE;
      break;
    }

    L7_INTF_CLRMASKBIT(tempMask,index);
    L7_INTF_FHMASKBIT(tempMask, index);
  }

  osapiSemaGive(dot1xCkptMaskSema);
  tempCurrLen = dot1xCkptBuff - checkpoint_data;
  if (tempCurrLen > sizeof(dot1x_ckpt_header_t))
  {
    *ret_data_len = tempCurrLen;
    msgHeader->messageLen = osapiHtons(tempCurrLen);
  }

  DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT_DETAIL,0, "%s:Success, exit at line %d\n",
                    __FUNCTION__, __LINE__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Callback from the checkpoint service
*
* @param    callback_type  @b{(input)}   callback type CKPT_NEW_BACKUP_MANAGER,
*                                       CKPT_NEXT_ELEMENT or CKPT_BACKUP_MANAGER_GONE
* @param    checkpoint_data  @b{(input)}   buffer in which the message is to be populated
* @param    max_data_len    @b{(inout)}   Max len allocated for the data
* @param    ret_data_len  @b{(output)}  Actual length copied
* @param    more_data  @b{(output)}  Boolean to indicate if there is data available to checkpoint
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void dot1xManagerCheckpoint(L7_CKPT_MGR_CALLBACK_t  callback_type,
                            L7_uchar8              *checkpoint_data,
                            L7_uint32               max_data_len,
                            L7_uint32              *ret_data_len,
                            L7_BOOL                *more_data)
{

  DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT,0, "%s:Check point callback type %d\n",
                    __FUNCTION__,callback_type);
  switch (callback_type)
  {
    case CKPT_NEW_BACKUP_MANAGER:
      osapiSemaTake(dot1xCkptMaskSema, L7_WAIT_FOREVER);
      dot1xBackupMgrElected = L7_TRUE;
      /* walk trhough the AVL tree and set need to checkpoint
      to true for each entry*/
      dot1xNsfCheckpointPrepare();

      osapiSemaGive(dot1xCkptMaskSema);
      if(ckptMgrCheckpointStart(L7_DOT1X_COMPONENT_ID) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
                "For callback type(%d) call to checkpoint service to start failed",
                callback_type);

        DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT, 0,
                          "%s:For callback type(%d) call to checkpoint service to start failed\n",
                          __FUNCTION__,callback_type);
      }
      break;
    case CKPT_NEXT_ELEMENT:
      /* build the message*/
      dot1xBuildCheckpointMessage(checkpoint_data, max_data_len,
                                  ret_data_len, more_data);

      break;
    case CKPT_BACKUP_MANAGER_GONE:
      osapiSemaTake(dot1xCkptMaskSema, L7_WAIT_FOREVER);
      dot1xBackupMgrElected = L7_FALSE;

      osapiSemaGive(dot1xCkptMaskSema);
      break;
    default:
      break;
  }


  return;
}

/*********************************************************************
* @purpose  Notify the checkpoint service about availability of data
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    opcode  @b{(input)}  checkpointed data mode of type DOT3AD_NSF_OPCODE_t
*                                type of values DOT3AD_NSF_OPCODE_ADD,
*                                DOT3AD_NSF_OPCODE_MODIFY or DOT3AD_NSF_OPCODE_DELETE
* @returns  L7_SUCCESS
*
* @notes    a. sets the bitmask for the appropriate interface
*           b. copies the data from operational structure to checkpoint strucutres
*
* @end
*********************************************************************/
L7_RC_t dot1xCallCheckpointService(L7_uint32 lIntIfNum, dot1xCheckpointTlvs_t recordType)
{
  L7_RC_t rc = L7_SUCCESS;
  dot1xLogicalPortInfo_t *operLPortData;
  L7_uint32 physPort;

  /* Do not propogate any changes if we are in the process of switching over*/
  if (dot1xSwitchoverInProgress == L7_TRUE)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT,0,
                      "%s:Called while dot1xSwitchoverInProgress == L7_TRUE: lIntIfNum = %d record %s\n",
                      __FUNCTION__, lIntIfNum, dot1xNsfDebugTlvStringGet(recordType));
    return rc;
  }

  operLPortData = dot1xLogicalPortInfoGet(lIntIfNum);
  if(operLPortData == L7_NULLPTR)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT,0,
                      "%s:Call to dot1xLogicalPortInfoGet() returned L7_NULLPTR for: lIntIfNum = %d record %s\n",
                      __FUNCTION__, lIntIfNum, dot1xNsfDebugTlvStringGet(recordType));
    return L7_FAILURE;
  }

  physPort = operLPortData->physPort;
  DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT,physPort,
                    "%s:Check pointing data for Logical port-%d record %s\n",
                      __FUNCTION__,lIntIfNum,dot1xNsfDebugTlvStringGet(recordType));
  switch (recordType)
  {
    case DOT1X_PHY_PORT_TLV:
      /* Copy the physical port information in ur structure.
         set the bit mask
      */
      if (dot1xIsValidIntf(physPort) != L7_TRUE)
      {
        return L7_FAILURE;
      }
      dot1xCkptPhyPort[physPort].authCount = dot1xPortInfo[physPort].authCount;
      dot1xCkptPhyPort[physPort].portMacAddrAdded = dot1xPortInfo[physPort].portMacAddrAdded;
      L7_INTF_SETMASKBIT(dot1xPhyPortCkptMask, physPort);

      break;

    case DOT1X_LOG_PORT_ADD_TLV:
    case DOT1X_LOG_PORT_DELETE_TLV:
    case DOT1X_LOG_PORT_MODIFY_TLV:

      /* copy the operational structure to the checkpoint db
         set the appropriate record type.
      */
      rc = dot1xNsfPopulateCkptDb(operLPortData, recordType);
      if (rc != L7_SUCCESS)
      {
        return rc;
      }
      break;

    default:
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
              "Unsupported opcode received %d ",recordType);
      return L7_FAILURE;
  }

  if (dot1xBackupMgrElected == L7_TRUE)
  {
    rc = ckptMgrCheckpointStart(L7_DOT1X_COMPONENT_ID);
    if (rc != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
                "Logical intfNum (%d) opcode(%d) call to checkpoint service to start failed(%d)",
                lIntIfNum, recordType, rc);

        DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT, 0,
                          "%s:IntfNum (%d) opcode(%d) call to checkpoint service to start failed(%d)\n",
                          __FUNCTION__, lIntIfNum, recordType, rc);
    }
  }
  return rc;
}

void dot1xReadLPortCkptData(L7_uchar8  *checkpoint_data,
                       dot1xLPortCheckPointData_t *pCurrCkptData)
{
  /* The received Data */
  dot1xLPortCheckPointData_t *rxCkptData;
  L7_uchar8  *lPortTlvDataStart = checkpoint_data + sizeof(L7_uint32);

  rxCkptData = (dot1xLPortCheckPointData_t *)lPortTlvDataStart;

  memcpy(pCurrCkptData, rxCkptData, sizeof(*pCurrCkptData));

  /* Endian safe */
  pCurrCkptData->logicalPortNumber = osapiNtohl(rxCkptData->logicalPortNumber);
  pCurrCkptData->logicalPortStatus = osapiNtohl(rxCkptData->logicalPortStatus);
  pCurrCkptData->serverStateLen = osapiNtohl(rxCkptData->serverStateLen);
  pCurrCkptData->serverClassLen = osapiNtohl(rxCkptData->serverClassLen);
  pCurrCkptData->sessionTimeout = osapiNtohl(rxCkptData->sessionTimeout);
  pCurrCkptData->vlanId = osapiNtohl(rxCkptData->vlanId);
  pCurrCkptData->blockVlanId = osapiNtohl(rxCkptData->blockVlanId);
  pCurrCkptData->unauthVlan = osapiNtohl(rxCkptData->unauthVlan);
  pCurrCkptData->guestVlanId = osapiNtohl(rxCkptData->guestVlanId);
  pCurrCkptData->voiceVlanId = osapiNtohl(rxCkptData->voiceVlanId);
  pCurrCkptData->isMABClient = osapiNtohl(rxCkptData->isMABClient);
  pCurrCkptData->dot1xUserNameLength = osapiNtohl(rxCkptData->dot1xUserNameLength);
  pCurrCkptData->dot1xChallengelen = osapiNtohl(rxCkptData->dot1xChallengelen);
  pCurrCkptData->terminationAction = osapiNtohl(rxCkptData->terminationAction);
  pCurrCkptData->vlanAssigned = osapiNtohl(rxCkptData->vlanAssigned);
  pCurrCkptData->defaultVlanId = osapiNtohl(rxCkptData->defaultVlanId);
  pCurrCkptData->isMonitorModeClient = osapiNtohl(rxCkptData->isMonitorModeClient);

}


L7_RC_t dot1xPopulateCheckpointData(L7_uchar8  *checkpoint_data, L7_uint32 data_len)
{

  dot1x_ckpt_header_t msgHeader, *rxHeader;
  dot1x_nsf_tlv_header_t currTlv, *rxTlv;
  /* The entry in the existing ckpt database*/
  dot1xLPortCheckPointData_t *pDot1xCkptNode;
  /* The received Data : Endian safe*/
  dot1xLPortCheckPointData_t currCkptData;
  dot1xPhyPortCheckPointData_t *rxPhyPortTlv, *currPhyPortTlv;
  L7_uint32 currLen;
  L7_uchar8 *currPtr;

  if (data_len == L7_NULL)
  {
    return L7_FAILURE;
  }

  currPtr = checkpoint_data;

  rxHeader = (dot1x_ckpt_header_t *)currPtr;

  msgHeader.version = osapiNtohs(rxHeader->version) ;
  msgHeader.messageLen = osapiNtohs(rxHeader->messageLen) ;


  currLen = msgHeader.messageLen - sizeof(msgHeader);

  currPtr += sizeof(msgHeader);

  if (msgHeader.version != DOT1X_CHECKPOINT_MESSAGE_VER)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
            "Version %d not supported \n", msgHeader.version);
    return L7_SUCCESS;
  }

  osapiSemaTake(dot1xCkptMaskSema, L7_WAIT_FOREVER);
  while (currLen > 0)
  {
    rxTlv = (dot1x_nsf_tlv_header_t *)currPtr;

    currTlv.tlvType = osapiNtohs(rxTlv->tlvType);
    currTlv.tlvLen = osapiNtohs(rxTlv->tlvLen);
    currTlv.index = osapiNtohl(rxTlv->index);
    if (currTlv.tlvLen == 0)
    {
      break;
    }
    currPtr += DOT1X_TLV_HEADER_LEN;
    currLen -= DOT1X_TLV_HEADER_LEN;

    /* For each TLV in the message
     If the TLV is add    : create an AVL entry and add it to the list
                   modify : update an existing entry in the tree
                   delete : delete the entry specified by the key from the tree
    */
    switch (currTlv.tlvType)
    {
      case DOT1X_LOG_PORT_ADD_TLV:
        pDot1xCkptNode = avlSearchLVL7(&dot1xCkptDataTreeDb,
                                       &(currTlv.index), AVL_EXACT);
        if (pDot1xCkptNode != L7_NULLPTR)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
            "Notification(%d) received for entry(%d) already present \n",
                  currTlv.tlvType, currTlv.index);
          break;
        }
        dot1xReadLPortCkptData(currPtr, &currCkptData);
        avlInsertEntry(&dot1xCkptDataTreeDb, &currCkptData);
        break;
      case DOT1X_LOG_PORT_MODIFY_TLV:
        pDot1xCkptNode = avlSearchLVL7(&dot1xCkptDataTreeDb,
                                       &(currTlv.index), AVL_EXACT);
        if (pDot1xCkptNode == L7_NULLPTR)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
            "Notification(%d) received for entry(%d) not present \n",
                  currTlv.tlvType, currTlv.index);
          break;
        }
        dot1xReadLPortCkptData(currPtr, &currCkptData);
        memcpy(pDot1xCkptNode, &currCkptData, sizeof(*pDot1xCkptNode));
        break;
      case DOT1X_LOG_PORT_DELETE_TLV:
        pDot1xCkptNode = avlSearchLVL7(&dot1xCkptDataTreeDb,
                                       &(currTlv.index), AVL_EXACT);
        if (pDot1xCkptNode == L7_NULLPTR)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
            "Notification(%d) received for entry(%d) not present \n",
                  currTlv.tlvType, currTlv.index);
          break;
        }
        avlDeleteEntry(&dot1xCkptDataTreeDb, pDot1xCkptNode);
        break;
      case DOT1X_PHY_PORT_TLV:
        rxPhyPortTlv = (dot1xPhyPortCheckPointData_t *)(currPtr+sizeof(currTlv.index));
        if (currTlv.index < L7_DOT1X_INTF_MAX_COUNT)
        {
          currPhyPortTlv = &dot1xCkptPhyPort[currTlv.index];
          currPhyPortTlv->authCount = osapiNtohl(rxPhyPortTlv->authCount);
          currPhyPortTlv->portMacAddrAdded =
            osapiNtohl(rxPhyPortTlv->portMacAddrAdded);
          L7_INTF_SETMASKBIT(dot1xPhyPortCkptMask, currTlv.index);
        }
        break;
      default:
        /* we have received an incorrect record type. break out of while*/
        currLen  = 0;
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
            "Incorrect notification(%d) received for entry(%d) \n",
                  currTlv.tlvType, currTlv.index);
        break;
    }
    currPtr += currTlv.tlvLen;
    currLen -= currTlv.tlvLen;
  }

  osapiSemaGive(dot1xCkptMaskSema);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Callback for the backup to handle checkpointed data
*
* @param    checkpoint_data  @b{(input)}  checkpointed data
* @param    data_len    @b{(inout)}   size of the data
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void dot1xBackupUnitCheckpoint(L7_CKPT_BACKUP_MGR_CALLBACK_t callback_type,
                               L7_uchar8                  *checkpoint_data,
                               L7_uint32                   data_len)
{

  DOT1X_EVENT_TRACE(DOT1X_TRACE_CKPT,0, "%s:Check point callback type %d\n",
                    __FUNCTION__,callback_type);

  switch (callback_type)
  {
    case CKPT_BACKUP_SYNC_DATA:
      /*  Populate the checkpointed data received*/
      dot1xPopulateCheckpointData(checkpoint_data, data_len);
      break;
    case CKPT_BACKUP_NEW_MANAGER:
      /* delete the existing check pointed data */
      osapiSemaTake(dot1xCkptMaskSema, L7_WAIT_FOREVER);
      dot1xCheckPtDataReset();
      osapiSemaGive(dot1xCkptMaskSema);

      break;

    default:
      break;
  }

  return;
}

/*********************************************************************
* @purpose  Restart timers in case of a warm restart
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xNsfTimersRestart(void)
{
  L7_uint32               phyIntf=0, lIntIfNum;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_RC_t                 nimRc, rc=L7_SUCCESS;
  dot1xPortCfg_t         *pCfg;

  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) != L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  nimRc = dot1xFirstValidIntfNumber(&phyIntf);
  while (nimRc == L7_SUCCESS)
  {
    /* Authenticator timer actions */
    if (dot1xPortInfo[phyIntf].paeCapabilities == L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
    {
      lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
      while ((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(phyIntf, &lIntIfNum))!= L7_NULLPTR)
      {
        if (logicalPortInfo->inUse == L7_TRUE)
        {
          if (dot1xPortInfo[phyIntf].portEnabled == L7_TRUE &&
              (dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO ||
               dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED))
          {
            if ((dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO) &&
                (dot1xPortInfo[phyIntf].portStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED) &&
                (dot1xIntfIsConfigurable(phyIntf, &pCfg) == L7_TRUE) &&
                (pCfg->guestVlanId != 0))
            {
              dot1xCtlPortGuestVlanTimerStart(phyIntf, pCfg->guestVlanId, DOT1X_PHYSICAL_PORT);
            }

            if (dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
            {
              logicalPortInfo->clientTimeout = DOT1X_CLIENT_TIMEOUT;
            }
          }
        }

        if (dot1xPortInfo[phyIntf].portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)
        {
          break;
        }
      }
    }

    nimRc = dot1xNextValidIntf(phyIntf, &phyIntf);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the next logical interface from the checkpoint tree
*
* @param    L7_uint32                  lIntIfNum      @b{(input)} logical intf
* @param    dot1xLPortCheckPointData_t pDot1xCkptNode @b{(output)} pointer to node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xCkptLogicalPortNextGet(L7_uint32 lIntIfNum,
                                    dot1xLPortCheckPointData_t *pDot1xCkptNode)
{
  dot1xLPortCheckPointData_t *pNode;

  pNode = avlSearchLVL7(&dot1xCkptDataTreeDb, &lIntIfNum, AVL_NEXT);
  if (pNode != L7_NULL)
  {
    memcpy(pDot1xCkptNode, pNode, sizeof(dot1xLPortCheckPointData_t));
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get a Logical Port Info Node
*
* @param    lIntIfNum  @b{(input)} The logical internal interface number
*
* @returns  Logical Internal Interface node
*
* @comments none
*
* @end
*********************************************************************/
dot1xLPortCheckPointData_t *dot1xCkptLogicalPortInfoGet(L7_uint32 lIntIfNum)
{
  L7_uint32 key = lIntIfNum;
  dot1xLPortCheckPointData_t *entry = L7_NULLPTR;
  entry = (dot1xLPortCheckPointData_t *)avlSearchLVL7(&dot1xCkptDataTreeDb,&key,AVL_EXACT);
  return entry;
}


/******************Debug Routines ********************************************/

L7_uchar8 *dot1xNsfDebugTlvStringGet(dot1xCheckpointTlvs_t tlvType)
{

  switch (tlvType)
  {
    case DOT1X_LOG_PORT_ADD_TLV: return "DOT1X_LOG_PORT_ADD_TLV";
    case DOT1X_LOG_PORT_DELETE_TLV: return "DOT1X_LOG_PORT_DELETE_TLV";
    case DOT1X_LOG_PORT_MODIFY_TLV: return "DOT1X_LOG_PORT_MODIFY_TLV";
    case DOT1X_PHY_PORT_TLV: return "DOT1X_PHY_PORT_TLV";
    default:
      return "Unknown";
  }
}

static L7_uchar8 buff[24];

L7_uchar8 *dot1xNsfDebugCkptTypeStringGet(dot1xCheckpointRecord_t ckptType)
{
  switch (ckptType)
  {
    case DOT1X_CKPT_RECORD_NEW: return "DOT1X_CKPT_RECORD_NEW";
    case DOT1X_CKPT_RECORD_UPDATE: return "DOT1X_CKPT_RECORD_UPDATE";
    case DOT1X_CKPT_RECORD_DELETED: return "DOT1X_CKPT_RECORD_DELETED";
    case DOT1X_CKPT_RECORD_CKPTED: return "DOT1X_CKPT_RECORD_CKPTED";
    default:
      {
        memset(buff, 0, sizeof(buff));
        osapiSnprintf(buff, sizeof(buff), "Unknown (%d)", ckptType);
        return(buff);
      }
  }
}

void dot1xCheckPointDataDump()
{
  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) != L7_TRUE)
  {
    return;
  }

  L7_uint32   lIntIfNum = 0;
  avlTree_t   *pTree = &dot1xCkptDataTreeDb;
  dot1xLPortCheckPointData_t *pDot1xCkptNode;
  L7_INTF_MASK_t tempMask;
  L7_uint32 intIfNum;

  /* Walk through the AVL tree */

  osapiSemaTake(dot1xCkptMaskSema, L7_WAIT_FOREVER);

  sysapiPrintf("dot1xCkptDataTreeDb tree contents:\n");

  /* Get each element in the ckpointed tree*/
  while ((pDot1xCkptNode = avlSearchLVL7(pTree, &lIntIfNum, AVL_NEXT)) != L7_NULLPTR)
  {
    sysapiPrintf("----------------------\n");
    sysapiPrintf("logicalPortNumber = %u; ", pDot1xCkptNode->logicalPortNumber);
    sysapiPrintf("currentIdL = %u; ", pDot1xCkptNode->currentIdL);
    sysapiPrintf("logicalPortStatus = ");
    switch (pDot1xCkptNode->logicalPortStatus)
    {
    case L7_DOT1X_PORT_STATUS_AUTHORIZED:
      sysapiPrintf("AUTH; ");
      break;
    case L7_DOT1X_PORT_STATUS_UNAUTHORIZED:
      sysapiPrintf("UNAUTH; ");
      break;
    default:
      sysapiPrintf("!Invalid!; ");
      break;
    }
    sysapiPrintf("\n");


    sysapiPrintf("serverStateLen = %u; ", pDot1xCkptNode->serverStateLen);
    if (pDot1xCkptNode->serverStateLen > 0)
    {
      L7_uint32 i;
      sysapiPrintf("serverState = ");
      for (i = 0; i < pDot1xCkptNode->serverStateLen; i++)
      {
        sysapiPrintf("%02X", pDot1xCkptNode->serverState[i]);
      }
      sysapiPrintf("\n");
    }
    else
    {
      sysapiPrintf("serverState = NULL\n");
    }

    sysapiPrintf("serverClassLen = %u; ", pDot1xCkptNode->serverClassLen);
    if (pDot1xCkptNode->serverClassLen > 0)
    {
      L7_uint32 i;
      sysapiPrintf("serverClass = ");
      for (i = 0; i < pDot1xCkptNode->serverClassLen; i++)
      {
        sysapiPrintf("%02X", pDot1xCkptNode->serverClass[i]);
      }
      sysapiPrintf("\n");
    }
    else
    {
      sysapiPrintf("serverClass = NULL\n");
    }


    sysapiPrintf("sessionTimeout = %d; ", pDot1xCkptNode->sessionTimeout);

    {
      L7_uchar8 buf[32];

      memset(buf, 0, sizeof(buf));
      osapiSnprintf((L7_char8 *)buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
              pDot1xCkptNode->suppMacAddr.addr[0],
              pDot1xCkptNode->suppMacAddr.addr[1],
              pDot1xCkptNode->suppMacAddr.addr[2],
              pDot1xCkptNode->suppMacAddr.addr[3],
              pDot1xCkptNode->suppMacAddr.addr[4],
              pDot1xCkptNode->suppMacAddr.addr[5]);
      sysapiPrintf("suppMacAddr = %s\n", buf);
    }

    /* user Details */
    sysapiPrintf("dot1xUserNameLength = %d; dot1xUserName = %s\n",
                 pDot1xCkptNode->dot1xUserNameLength, pDot1xCkptNode->dot1xUserName);

    {
      L7_uint i;

      sysapiPrintf("dot1xChallengelen = %d; dot1xChallenge = ", pDot1xCkptNode->dot1xChallengelen );
      for (i=0; i<pDot1xCkptNode->dot1xChallengelen; i++)
      {
        sysapiPrintf("%02X", pDot1xCkptNode->dot1xChallenge[i]);
      }
      sysapiPrintf("\n");
    }

    sysapiPrintf("terminationAction = %d; vlanId = %d; vlanAssigned = %d\n",
                 pDot1xCkptNode->terminationAction,
                 pDot1xCkptNode->vlanId,
                 pDot1xCkptNode->vlanAssigned);

    sysapiPrintf("Blocked Vlan Id = %d \n",pDot1xCkptNode->blockVlanId);

    sysapiPrintf("filterName = %s\n", pDot1xCkptNode->filterName);

    sysapiPrintf("unauthVlan = %d; guestVlanId = %d; voiceVlanId = %d defaultVlanId = %d\n", 
                 pDot1xCkptNode->unauthVlan,
                 pDot1xCkptNode->guestVlanId,
                 pDot1xCkptNode->voiceVlanId,
                 pDot1xCkptNode->defaultVlanId);

    sysapiPrintf("isMABClient = %s\n", pDot1xCkptNode->isMABClient?"TRUE":"FALSE");

    sysapiPrintf("isMonitorModeClient = %s\n", pDot1xCkptNode->isMonitorModeClient?"TRUE":"FALSE");

    sysapiPrintf("recordType = %s\n", dot1xNsfDebugCkptTypeStringGet(pDot1xCkptNode->recordType));

    /* update search keys for next pass */
    lIntIfNum = pDot1xCkptNode->logicalPortNumber;
  }

  sysapiPrintf("\ndot1xCkptPhyPort table contents:\n");

  memcpy(&tempMask, &dot1xPhyPortCkptMask, sizeof(tempMask));
  L7_INTF_FHMASKBIT(tempMask, intIfNum);

  while (intIfNum != 0)
  {
    sysapiPrintf("  intIfNum = %d - ", intIfNum);
    sysapiPrintf("Number of Authorizations = %d, ", dot1xCkptPhyPort[intIfNum].authCount);
    sysapiPrintf("Client MAC in db = %s", dot1xCkptPhyPort[intIfNum].portMacAddrAdded?"TRUE":"FALSE");
    sysapiPrintf("\n");
    L7_INTF_CLRMASKBIT(tempMask,intIfNum);
    L7_INTF_FHMASKBIT(tempMask, intIfNum);
  }

  osapiSemaGive(dot1xCkptMaskSema);
  return;
}
