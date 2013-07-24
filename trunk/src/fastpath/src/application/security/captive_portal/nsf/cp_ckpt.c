/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2009
*
**********************************************************************
*
* @filename   cp_ckpt.c
*
* @purpose    Captive Portal Checkpoint Data File
*
* @component  Captive Portal
*
* @comments   None
*
* @create     04/22/2009
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#include "flex.h"

#ifdef L7_NSF_PACKAGE

#include <string.h>
#include "commdefs.h"
#include "osapi.h"
#include "osapi_support.h"
#include "simapi.h"
#include "log.h"
#include "avl_api.h"
#include "nimapi.h"
#include "comm_mask.h"
#include "dot1q_api.h"
#include "cpdm.h"
#include "cp_cluster_msg.h"
#include "cpdm_connstatus_api.h"
#include "cpdm_connstatus_util.h"
#include "cpdm_api.h"
#include "cpdm_user_api.h"
#include "intf_cb_api.h"
#include "cpim_api.h"
#include "cpwio_api.h"
#ifdef L7_CP_WIO_PACKAGE
 #include "wio_outcalls.h"
 #include "wio_util.h"
#endif
#include "ckpt_api.h"
#include "cp_ckpt.h"

#define SEMA_TAKE(access)   \
  if (CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);
#define SEMA_GIVE(access)   \
  if (CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);

extern cpdmOprData_t   *cpdmOprData;
extern avlTree_t        clientConnStatusTree;
extern L7_VLAN_MASK_t   dot1qIntfVidList[L7_MAX_INTERFACE_COUNT +1];
static L7_enetMacAddr_t prevAddr;
static L7_BOOL          firstPass;

/*********************************************************************
                MANAGEMENT UNIT FUNCTIONS
*********************************************************************/

/*********************************************************************
* @purpose  Checkpoint Service callback to process mgmt unit events
*
* @param    L7_CKPT_MGR_CALLBACK_t cbType   @b{(input)} callback type
* @param    L7_uchar8              ckptData @b{(input)} ckpt msg
* @param    L7_uint32              bufLen   @b{(input)} buffer length
* @param    L7_uint32              msgLen   @b{(output)} bytes written
* @param    L7_BOOL                moreData @b{(output)} true or false
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpCkptManagerCallback(L7_CKPT_MGR_CALLBACK_t cbType,
                           L7_uchar8 *ckptData, L7_uint32 bufLen,
                           L7_uint32 *msgLen, L7_BOOL *moreData)
{
  switch (cbType)
  {
  case CKPT_NEW_BACKUP_MANAGER:
    cpCkptNewBackupManager();
    break;
  case CKPT_NEXT_ELEMENT:
    if (firstPass == L7_TRUE)
    {
      memset(&prevAddr, 0x00, sizeof(L7_enetMacAddr_t));
      firstPass = L7_FALSE;
    }
    cpCkptMsgWrite(ckptData, bufLen, msgLen, moreData);
    break;
  case CKPT_BACKUP_MANAGER_GONE:
    cpCkptBackupManagerGone();
    break;
  default:
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: Invalid callback type %d.", __FUNCTION__, cbType);
    break;
  }

  return;
}

/*********************************************************************
* @purpose  Process election of new backup unit
*
* @param    void
*
* @returns  void
*
* @comments Runs on the checkpoint service thread.
*
* @end
*********************************************************************/
void cpCkptNewBackupManager(void)
{
  if (osapiSemaTake(cpdmOprData->cpCkptPendingSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if (cpdmOprData->cpBackupElected == L7_FALSE)
    {
      cpdmOprData->cpBackupElected = L7_TRUE;
    }
  
    if (ckptMgrCheckpointStart(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: ckptMgrCheckpointStart call failed.", __FUNCTION__);
      (void)osapiSemaGive(cpdmOprData->cpCkptPendingSema);
      return;
    }

    cpdmOprData->cpCkptInProg = L7_TRUE;
    firstPass = L7_TRUE;
    (void)osapiSemaGive(cpdmOprData->cpCkptPendingSema);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: Ckpt already in progress.", __FUNCTION__);
  }

  return;
}

/*********************************************************************
* @purpose  Build CP client ckpt message
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)} client MAC address
* @param    L7_BOOL          macAddDel @b{(input)} L7_TRUE, if mac is to be added
*                                                  L7_FALSE, if mac is to be deleted
* @param    L7_uint32        timeAtClientConn @b{(input)} time when client first connects
* @param    L7_uchar8        buf       @b{(input/output)} ptr to store msg
* @param    L7_uint32        offset    @b{(input/output)} ptr to offset into buf
* @param    L7_ushort16      len       @b{(input/output)} ptr to msg length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
static L7_RC_t cpCkptClientMsgBuild(L7_enetMacAddr_t macAddr, L7_BOOL macAddDel,
                                    L7_uint32 timeAtClientConn,
                                    L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  L7_ushort16           pktLen = 0;
  cpCkptMsgElementHdr_t element;
  L7_IP_ADDR_t          ip = 0;
  L7_uint32             intf = 0, tmp32 = 0;
  L7_uchar8             flag = 0, uName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_uint32             stackUpTime = 0, sTimeout = 0;
  L7_uchar8            *bufPtr;
  L7_uint32             off_set;

  if ((buf == L7_NULLPTR) || (offset == L7_NULLPTR) || (len == L7_NULLPTR))
  {
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: NULL ckpt msg buffer pointer.", __FUNCTION__);
    return L7_FAILURE;
  }

  bufPtr = buf;
  off_set = *offset;

  CP_CKPT_ELEMENT_HDR_SET(element, CP_CKPT_IE_TYPE_CLIENT_MAC_ADDRESS,
                          CP_CKPT_IE_CLIENT_MAC_ADDRESS_SIZE, bufPtr, off_set);
  pktLen += CP_CKPT_IE_HDR_SIZE + CP_CKPT_IE_CLIENT_MAC_ADDRESS_SIZE;
  CP_PKT_MAC_ADDR_SET(macAddr.addr, bufPtr, off_set);

  CP_CKPT_ELEMENT_HDR_SET(element, CP_CKPT_IE_TYPE_DATA,
                          CP_CKPT_IE_DATA_SIZE, bufPtr, off_set);
  pktLen += CP_CKPT_IE_HDR_SIZE + CP_CKPT_IE_DATA_SIZE;

  if (macAddDel == L7_TRUE)
  {
    if (cpdmClientConnStatusIpGet(&macAddr, &ip) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: Failed to get client IP.", __FUNCTION__);
      return L7_FAILURE;
    }

    if (cpdmClientConnStatusIntfIdGet(&macAddr, &intf) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: Failed to get client intf.", __FUNCTION__);
      return L7_FAILURE;
    }

    if (cpdmClientConnStatusUserNameGet(&macAddr, uName) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: Failed to get client username.", __FUNCTION__);
      return L7_FAILURE;
    }

    if (cpdmClientConnStatusSessionTimeoutGet(&macAddr, &sTimeout) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: Failed to get client session timeout.", __FUNCTION__);
      return L7_FAILURE;
    }

    if (sTimeout > 0)
    {
      stackUpTime = simSystemUpTimeGet();
    }
  }

  tmp32 = (L7_uint32)ip;
  CP_PKT_INT32_SET(tmp32, bufPtr, off_set);

  CP_PKT_INT32_SET(intf, bufPtr, off_set);

  CP_PKT_STRING_SET(uName, CP_USER_LOCAL_USERNAME_MAX, bufPtr, off_set);

  tmp32 = stackUpTime + sTimeout;            
  CP_PKT_INT32_SET(tmp32, bufPtr, off_set);

  CP_PKT_INT32_SET(timeAtClientConn, bufPtr, off_set);

  flag = (L7_uchar8)macAddDel;
  CP_PKT_INT8_SET(flag, bufPtr, off_set);

  *len += pktLen;
  *offset = off_set;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process Next Element event invoked by checkpoint service
*
* @param    L7_uchar8  ckptData  @b{(input)} ckpt msg
* @param    L7_uint32  bufLen    @b{(input)} buffer length
* @param    L7_uint32  msgLen    @b{(output)} bytes written to buffer
* @param    L7_BOOL    moreData  @b{(output)} true or false
*
* @returns  void
*
* @comments Runs on the checkpoint service thread.
*
* @end
*********************************************************************/
void cpCkptMsgWrite(L7_uchar8 *ckptData, L7_uint32 bufLen,
                    L7_uint32 *msgLen, L7_BOOL *moreData)
{
  cpCkptMsgHdr_t             *message = L7_NULLPTR;
  L7_enetMacAddr_t            macAddr;
  L7_uint32                   remLen = bufLen;
  L7_ushort16                 mLen = 0;
  cpdmCkptData_t             *ptrData = L7_NULLPTR;
  L7_uchar8                  *bufPtr;
  L7_uint32                   off_set;

  *msgLen = 0;
  *moreData = L7_TRUE;

  bufPtr = ckptData;
  off_set = 0;
  message = (cpCkptMsgHdr_t *)(bufPtr + off_set);
  message->msgType = osapiHtons(CP_CKPT_MSG);
  message->msgLen = CP_CKPT_MSG_HDR_SIZE;
  off_set += CP_CKPT_MSG_HDR_SIZE;
  remLen -= message->msgLen;

  if (cpdmOprData->cpCkptReqd != L7_TRUE)
  {
    memcpy(ckptData + CP_CKPT_MSG_TYPE_SIZE, &mLen, CP_CKPT_MSG_LEN_SIZE);
    *moreData = L7_FALSE;
    cpdmOprData->cpCkptInProg = L7_FALSE;
    (void)osapiSemaGive(cpdmOprData->cpCkptPendingSema);
    return;
  }

  memcpy(&macAddr, &prevAddr, sizeof(L7_enetMacAddr_t));
  while ((remLen >= CP_CKPT_MSG_TOTAL_IE_SIZE) && (*moreData == L7_TRUE))
  {
    if ((ptrData=avlSearchLVL7(&cpdmOprData->cpCkptTree, &macAddr, AVL_NEXT)) != L7_NULLPTR)
    {
      if (cpCkptClientMsgBuild(ptrData->macAddr, ptrData->flag, ptrData->stackUpTimeAtClientConn,
                               bufPtr, &off_set, &message->msgLen) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_ERROR,
                "%s: Failed to build wired client %02x:%02x:%02x:%02x:%02x:%02x ckpt msg.",
                __FUNCTION__,
                ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
                ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5]);
        memcpy(ckptData + CP_CKPT_MSG_TYPE_SIZE, &mLen, CP_CKPT_MSG_LEN_SIZE);
        *msgLen = 0;
        *moreData = L7_FALSE;
        (void)osapiSemaGive(cpdmOprData->cpCkptPendingSema);
        return;
      }
      remLen -= CP_CKPT_MSG_TOTAL_IE_SIZE;
    }
    else
    {
      /* no more data to ckpt */
      cpdmOprData->cpCkptReqd = L7_FALSE;
      break;
    }

    memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));

    /* check if this client was marked for deletion */
    if (ptrData->flag == L7_FALSE)
    {
      if (avlDeleteEntry(&cpdmOprData->cpCkptTree, ptrData) == NULL)
      {
        CP_DLOG(CPD_LEVEL_ERROR,
                "%s: Ckptd client %02x:%02x:%02x:%02x:%02x:%02x not deleted.",
                __FUNCTION__,
                ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
                ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5]);
      }
    }
  }

  *msgLen = message->msgLen;
  message->msgLen = osapiHtons(message->msgLen);
  memcpy(&prevAddr, &macAddr, sizeof(L7_enetMacAddr_t));
  CP_DLOG(CPD_LEVEL_LOG,
          "%s: CP wired client ckpt msg sent.", __FUNCTION__);
  return;
}

/*********************************************************************
* @purpose  Process leaving of backup unit
*
* @param    void
*
* @returns  void
*
* @comments Runs on the checkpoint service thread.
*
* @end
*********************************************************************/
void cpCkptBackupManagerGone(void)
{
  L7_enetMacAddr_t  macAddr;
  cpdmCkptData_t   *ptrData = L7_NULLPTR;

  SEMA_TAKE(WRITE);
  if (cpdmOprData->cpBackupElected == L7_TRUE)
  {
    cpdmOprData->cpBackupElected = L7_FALSE;
    cpdmOprData->cpCkptInProg = L7_FALSE;

    memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
    if ((ptrData=avlSearchLVL7(&cpdmOprData->cpCkptTree, &macAddr, AVL_NEXT)) != L7_NULLPTR)
    {
      cpdmOprData->cpCkptReqd = L7_TRUE;
    }
  }
  SEMA_GIVE(WRITE);
  return;
}

/*********************************************************************
* @purpose  Checkpoint CP client info changes
*
* @param    void
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
void cpCkptClientInfoCkpt(void)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
  {
    if (osapiSemaTake(cpdmOprData->cpCkptPendingSema, L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      if (ckptMgrCheckpointStart(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_ERROR,
                "%s: ckptMgrCheckpointStart call failed.", __FUNCTION__);
        (void)osapiSemaGive(cpdmOprData->cpCkptPendingSema);
        return;
      }
  
      cpdmOprData->cpCkptInProg = L7_TRUE;
      firstPass = L7_TRUE;
    (void)osapiSemaGive(cpdmOprData->cpCkptPendingSema);
    }
  }

  return;
}


/*********************************************************************
                BACKUP UNIT FUNCTIONS
*********************************************************************/

/*********************************************************************
* @purpose  Checkpoint Service callback to process backup unit events
*
* @param    L7_CKPT_BACKUP_MGR_CALLBACK_t cbType   @b{(input)} callback type
* @param    L7_uchar8                     ckptData @b{(input)} ckpt msg
* @param    L7_uint32                     msgLen   @b{(input)} msg length
*
* @returns  void
*
* @comments This function executes while CP is in WMU state.
*           It runs on the checkpoint service thread.
*
* @end
*********************************************************************/
void cpCkptBackupCallback(L7_CKPT_BACKUP_MGR_CALLBACK_t cbType,
                          L7_uchar8 *ckptData, L7_uint32 msgLen)
{
  switch (cbType)
  {
  case CKPT_BACKUP_SYNC_DATA:
    cpCkptMsgProcess(ckptData, msgLen);
    break;
  case CKPT_BACKUP_NEW_MANAGER:
    cpCkptDataPurge();
    break;
  default:
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: Invalid callback type %d.", __FUNCTION__, cbType);
    break;
  }
  return;
}

/*********************************************************************
* @purpose  Process received ckpt message on backup unit
*
* @param    L7_uchar8 ckptData @b{(input)} ckpt msg
* @param    L7_uint32 msgLen   @b{(input)} msg length
*
* @returns  void
*
* @comments This function executes while CP is in WMU state.
*           It runs on the checkpoint service thread.
*
* @end
*********************************************************************/
void cpCkptMsgProcess(L7_uchar8 *ckptData, L7_uint32 msgLen)
{
  L7_uchar8      *msgCkptBuf, flag = 0;
  cpCkptMsgHdr_t  hdr;
  L7_ushort16     mType = 0, mLen = 0;
  L7_int32        totalIELen;
  L7_uint32       ieCount = 0, tmp32 = 0;
  cpdmCkptData_t  data, *ptrData = L7_NULLPTR;

  if (ckptData == L7_NULLPTR) 
  {
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: NULL ckpt msg buffer pointer.", __FUNCTION__);
    return;
  }

  msgCkptBuf = ckptData;
  memcpy(&hdr, msgCkptBuf, sizeof(cpCkptMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;

  if (osapiNtohs(mType) != CP_CKPT_MSG)
  {
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: Invalid ckpt msg type %d, msg dropped.",
            __FUNCTION__, osapiNtohs(mType));
    return;
  }

  /* minimum ckpt msg length is msg header + one client info */
  if (msgLen < CP_CKPT_MSG_SIZE)
  {
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: Short ckpt msg length (%d), msg dropped.", __FUNCTION__, msgLen);
    return;
  }

  if (msgLen != osapiNtohs(mLen))
  {
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: Ckpt reports length %d and Msg is %d bytes, msg dropped.",
            __FUNCTION__, msgLen, osapiNtohl(mLen));
    return;
  }

  msgCkptBuf += CP_CKPT_MSG_HDR_SIZE;
  totalIELen = mLen - CP_CKPT_MSG_HDR_SIZE;

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CKPT_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCkptBuf, CP_CKPT_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCkptBuf += CP_CKPT_IE_ID_SIZE;
      totalIELen -= CP_CKPT_IE_ID_SIZE;

      memcpy(&mLen, msgCkptBuf, CP_CKPT_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCkptBuf += CP_CKPT_IE_LEN_SIZE;
      totalIELen -= CP_CKPT_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: Failed to parse ckpt msg.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_CKPT_IE_TYPE_CLIENT_MAC_ADDRESS:
      if (mLen == CP_CKPT_IE_CLIENT_MAC_ADDRESS_SIZE)
      {
        memset(&data, 0x00, sizeof(cpdmCkptData_t));
        memcpy(&data.macAddr.addr, msgCkptBuf, mLen);
        msgCkptBuf += mLen;
        ieCount++;
      }
      break;
    case CP_CKPT_IE_TYPE_DATA:
      if (mLen == CP_CKPT_IE_DATA_SIZE)
      {
        memcpy(&data.ip, msgCkptBuf, CP_CKPT_IE_IPv4_SIZE);
        data.ip = osapiNtohl(data.ip);
        msgCkptBuf += CP_CKPT_IE_IPv4_SIZE;

        memcpy(&data.intf, msgCkptBuf, CP_CKPT_IE_INT32_SIZE);
        data.intf = osapiNtohl(data.intf);
        msgCkptBuf += CP_CKPT_IE_INT32_SIZE;

        osapiStrncpy(data.uName, msgCkptBuf, CP_USER_LOCAL_USERNAME_MAX);
        data.uName[CP_USER_LOCAL_USERNAME_MAX] = '\0';
        msgCkptBuf += CP_USER_LOCAL_USERNAME_MAX;

        memcpy(&tmp32, msgCkptBuf, CP_CKPT_IE_INT32_SIZE);
        tmp32 = osapiNtohl(tmp32);
        data.sessionTimeout = tmp32;
        msgCkptBuf += CP_CKPT_IE_INT32_SIZE;

        memcpy(&tmp32, msgCkptBuf, CP_CKPT_IE_INT32_SIZE);
        tmp32 = osapiNtohl(tmp32);
        data.stackUpTimeAtClientConn = tmp32;
        msgCkptBuf += CP_CKPT_IE_INT32_SIZE;

        flag = *msgCkptBuf;
        data.flag = flag;
        msgCkptBuf += CP_CKPT_IE_INT8_SIZE;
        ieCount++;
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: Invalid ckpt msg IE type %d.", __FUNCTION__, mType);
      break;
    }

    totalIELen -= mLen;

    if (ieCount == CP_CKPT_MSG_TOTAL_IE_COUNT)
    {
      SEMA_TAKE(WRITE);
      if (flag == L7_TRUE)
      {
        if ((ptrData=avlSearchLVL7(&cpdmOprData->cpCkptTree,
                                   data.macAddr.addr, AVL_EXACT)) != L7_NULLPTR)
        {
          ptrData->ip = data.ip;
          ptrData->intf = data.intf;
          osapiStrncpy(ptrData->uName, data.uName, CP_USER_LOCAL_USERNAME_MAX);
          ptrData->uName[CP_USER_LOCAL_USERNAME_MAX] = '\0';
          ptrData->sessionTimeout = data.sessionTimeout;
          ptrData->flag = data.flag;
        }
        else
        {
          if (avlInsertEntry(&cpdmOprData->cpCkptTree, &data) != NULL)
          {
            CP_DLOG(CPD_LEVEL_ERROR,
                    "%s: Failed to add ckpt client %02x:%02x:%02x:%02x:%02x:%02x.",
                    __FUNCTION__,
                    data.macAddr.addr[0], data.macAddr.addr[1], data.macAddr.addr[2],
                    data.macAddr.addr[3], data.macAddr.addr[4], data.macAddr.addr[5]);
          }
        }
      }
      else
      {
        if (avlDeleteEntry(&cpdmOprData->cpCkptTree, &data) == NULL)
        {
          CP_DLOG(CPD_LEVEL_ERROR,
                  "%s: Ckpt client %02x:%02x:%02x:%02x:%02x:%02x not found.",
                  __FUNCTION__,
                  data.macAddr.addr[0], data.macAddr.addr[1], data.macAddr.addr[2],
                  data.macAddr.addr[3], data.macAddr.addr[4], data.macAddr.addr[5]);
        }
      }
      SEMA_GIVE(WRITE);
      ieCount = 0;
    }
  }

  return;
}

/*********************************************************************
* @purpose  Clear all checkpointed data
*
* @param    void
*
* @returns  void
*
* @comments This function is called when new backup unit is selected.
*           It is also called in case of a cold restart to delete any
*           checkpointed data.
*
* @end
*********************************************************************/
void cpCkptDataPurge(void)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
  {
    SEMA_TAKE(WRITE);
    avlPurgeAvlTree(&cpdmOprData->cpCkptTree, CP_CLIENT_CONN_STATUS_MAX);
    SEMA_GIVE(WRITE);
  }

  return;
}

/*********************************************************************
* @purpose  Apply checkpointed data to backup unit on a warm restart
*
* @param    void
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
void cpCkptDataApply(L7_BOOL *dataExists)
{
  L7_uint32                   intfState;
  L7_enetMacAddr_t            macAddr;
  cpdmCkptData_t             *ptrData = L7_NULLPTR;
  cpdmClientConnStatusData_t *ckptdClient = L7_NULLPTR;
  L7_uchar8                   swMac[L7_MAC_ADDR_LEN];
  L7_uint32                   stackUpTime = 0;
  intfCpCallbacks_t          *pCBs = L7_NULLPTR;

  if (dataExists == L7_NULLPTR)
  {
    return;
  }
  *dataExists = L7_FALSE;

  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) != L7_TRUE)
  {
    return;
  }

  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  while ((ptrData=avlSearchLVL7(&cpdmOprData->cpCkptTree, &macAddr, AVL_NEXT)) != L7_NULLPTR)
  {
    memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));

    if ((nimGetIntfActiveState(ptrData->intf, &intfState) != L7_SUCCESS) ||
        (intfState != L7_ACTIVE))
    {
      /* delete this client from the table as its intf is not active
       * and move on to the next client */
      if (avlDeleteEntry(&cpdmOprData->cpCkptTree, ptrData) == NULL)
      {
        CP_DLOG(CPD_LEVEL_LOG,
                "%s: Failed to delete ckptd client %02x:%02x:%02x:%02x:%02x:%02x.",
                __FUNCTION__,
                ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
                ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5]);
      }
      continue;
    }

    if (*dataExists == L7_FALSE)
    {
      *dataExists = L7_TRUE;
    }

    if (cpdmClientConnStatusAdd(&ptrData->macAddr, ptrData->uName,
                                ptrData->intf, ptrData->ip) == L7_SUCCESS)
    {
      if ((ckptdClient=avlSearchLVL7(&clientConnStatusTree, &ptrData->macAddr, AVL_EXACT)) != L7_NULLPTR)
      {
        SEMA_TAKE(WRITE);
        simGetSystemIPBurnedInMac(swMac);
        memcpy(&(ckptdClient->switchMacAddr), swMac, sizeof(L7_enetMacAddr_t));
        ckptdClient->switchIp = simGetSystemIPAddr();
        ckptdClient->whichSwitch = CP_AUTH_ON_LOCAL_SWITCH;
        SEMA_GIVE(WRITE);

        if (cpdmCPConfigProtocolModeGet(ckptdClient->cpId, &ckptdClient->protocolMode) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_ERROR,
                  "%s: Failed to get cpId %d protocol mode.", __FUNCTION__, ckptdClient->cpId);
          return;
        }

        if (cpdmCPConfigVerifyModeGet(ckptdClient->cpId, &ckptdClient->verifyMode) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_ERROR,
                  "%s: Failed to get cpId %d verification mode.", __FUNCTION__, ckptdClient->cpId);
          return;
        }

        if (ckptdClient->verifyMode == CP_VERIFY_MODE_LOCAL)
        {
          if (cpdmUserEntryByNameGet(ptrData->uName, &ckptdClient->uId) != L7_SUCCESS)
          {
            CP_DLOG(CPD_LEVEL_ERROR,
                    "%s: Failed to get user id from uName %s.", __FUNCTION__, ptrData->uName);
            return;
          }
        }

        SEMA_TAKE(WRITE);
        if (ptrData->sessionTimeout > 0)
        {
          stackUpTime = simSystemUpTimeGet();
        }
        ckptdClient->limits.sessionTimeout = ptrData->sessionTimeout - stackUpTime;

        ckptdClient->sessionStart = ptrData->stackUpTimeAtClientConn;
        SEMA_GIVE(WRITE);

        if (cpdmCPConfigIdleTimeoutGet(ckptdClient->cpId, &ckptdClient->limits.idleTimeout) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_ERROR,
                  "%s: Failed to get cpId %d idle timeout.", __FUNCTION__, ckptdClient->cpId);
          return;
        }

        if ((cpdmCPConfigUserUpRateGet(ckptdClient->cpId, &ckptdClient->limits.maxBandwidthUp) != L7_SUCCESS) ||
            (cpdmCPConfigUserDownRateGet(ckptdClient->cpId, &ckptdClient->limits.maxBandwidthDown) != L7_SUCCESS))
        {
          CP_DLOG(CPD_LEVEL_ERROR,
                  "%s: Failed to get cpId %d rate info.", __FUNCTION__, ckptdClient->cpId);
          return;
        }

        if ((cpdmCPConfigMaxInputOctetsGet(ckptdClient->cpId, &ckptdClient->limits.maxInputOctets) != L7_SUCCESS) ||
            (cpdmCPConfigMaxOutputOctetsGet(ckptdClient->cpId, &ckptdClient->limits.maxOutputOctets) != L7_SUCCESS) ||
            (cpdmCPConfigMaxTotalOctetsGet(ckptdClient->cpId, &ckptdClient->limits.maxTotalOctets) != L7_SUCCESS))
        {
          CP_DLOG(CPD_LEVEL_ERROR,
                  "%s: Failed to get cpId %d user octet info.", __FUNCTION__, ckptdClient->cpId);
          return;
        }

        pCBs = cpimIntfOwnerCallbacksGet(ptrData->intf);
        if (pCBs)
        {
          pCBs->cpAuthenUnauthenClient(CP_CLIENT_AUTH, ptrData->macAddr,
                                       ckptdClient->limits.maxBandwidthUp, 
                                       ckptdClient->limits.maxBandwidthDown);
          CP_DLOG(CPD_LEVEL_ERROR,
                  "%s: Sent CP_CLIENT_AUTH to WIO for client %02x:%02x:%02x:%02x:%02x:%02x, up:%u down:%u",
                  __FUNCTION__,
                  ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
                  ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5],
                  ckptdClient->limits.maxBandwidthUp, ckptdClient->limits.maxBandwidthDown);
        }
      }
      else
      {
        CP_DLOG(CPD_LEVEL_ERROR,
                "%s: Failed to update ckptd client %02x:%02x:%02x:%02x:%02x:%02x info.",
                __FUNCTION__,
                ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
                ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5]);
      }
    }
    else
    {
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: Failed to add ckptd client %02x:%02x:%02x:%02x:%02x:%02x.",
              __FUNCTION__,
              ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
              ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5]);
    }
  }

  return;
}


/*********************************************************************
                DEBUG FUNCTIONS
*********************************************************************/
#include "unitmgr_api.h"

/*********************************************************************
* @purpose  Display CP checkpointed client info sent by mgmt unit
*
* @param    none
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
void cpCkptTestMgmtShow()
{
  L7_uint32         unit, mgmt, backup;
  cpdmCkptData_t   *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t  macAddr;

  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) != L7_TRUE)
  {
    return;
  }

  if ((unitMgrNumberGet(&unit) != L7_SUCCESS) ||
      (unitMgrMgrNumberGet(&mgmt) != L7_SUCCESS) ||
      (unitMgrBackupNumberGet(&backup) != L7_SUCCESS))
  {
    sysapiPrintf("Failed to get unit numbers.\n");
    return;
  }

  if (unit != mgmt)
  {
    sysapiPrintf("This is not a mgmt unit.\n");
    return;
  }

  sysapiPrintf("Backup elected:   %d\n", cpdmOprData->cpBackupElected);
  sysapiPrintf("Warm restart:     %d\n", cpdmOprData->cpWarmRestart);
  sysapiPrintf("Ckpt required:    %d\n", cpdmOprData->cpCkptReqd);
  sysapiPrintf("Ckpt in progress: %d\n", cpdmOprData->cpCkptInProg);
  sysapiPrintf("\n");

  if (cpdmOprData->cpCkptReqd != L7_TRUE)
  {
    sysapiPrintf ("No data to ckpt.\n");
    return;
  }

  sysapiPrintf ("   MAC Address      IP Address    Intf  Uname   STo  Flag\n");
  sysapiPrintf ("----------------- --------------- ---- ------- ----- ----\n");

  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  while ((ptrData=avlSearchLVL7(&cpdmOprData->cpCkptTree, &macAddr, AVL_NEXT)) != L7_NULLPTR)
  {
    sysapiPrintf("%02x:%02x:%02x:%02x:%02x:%02x ",
                 ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
                 ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5]);
    sysapiPrintf("%-15s ", osapiInet_ntoa(ptrData->ip));
    sysapiPrintf("%-4u ", ptrData->intf);
    sysapiPrintf("%-7s ", ptrData->uName);
    sysapiPrintf("%-5u ", ptrData->sessionTimeout);
    sysapiPrintf("%-4u\n", ptrData->flag);

    memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
  }

  return;
}

/*********************************************************************
* @purpose  Display CP checkpointed data received on backup unit
*
* @param    none
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
void cpCkptDump()
{
  cpdmCkptData_t   *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t  macAddr;

  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) != L7_TRUE)
  {
    return;
  }

  if (&cpdmOprData->cpCkptTree.count == 0)
  {
    sysapiPrintf ("No ckptd data received.\n");
    return;
  }

  sysapiPrintf ("   MAC Address      IP Address    Intf  Uname   STo\n");
  sysapiPrintf ("----------------- --------------- ---- ------- -----\n");

  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  while ((ptrData=avlSearchLVL7(&cpdmOprData->cpCkptTree, &macAddr, AVL_NEXT)) != L7_NULLPTR)
  {
    sysapiPrintf("%02x:%02x:%02x:%02x:%02x:%02x ",
                 ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
                 ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5]);
    sysapiPrintf("%-15s ", osapiInet_ntoa(ptrData->ip));
    sysapiPrintf("%-4u ", ptrData->intf);
    sysapiPrintf("%-7s ", ptrData->uName);
    sysapiPrintf("%-5u\n", ptrData->sessionTimeout);

    memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
  }

  return;
}

/*********************************************************************
* @purpose  Display CP checkpointed client info applied on backup unit
*
* @param    none
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
void cpCkptTestBackupShow()
{
  L7_uint32 unit, mgmt, backup;
  cpdmClientConnStatusData_t *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t            macAddr;

  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_NSF_FEATURE_ID) != L7_TRUE)
  {
    return;
  }

  if ((unitMgrNumberGet(&unit) != L7_SUCCESS) ||
      (unitMgrMgrNumberGet(&mgmt) != L7_SUCCESS) ||
      (unitMgrBackupNumberGet(&backup) != L7_SUCCESS))
  {
    sysapiPrintf("Failed to get unit numbers.\n");
    return;
  }

  if (unit != backup)
  {
    sysapiPrintf("This is not a backup unit.\n");
    return;
  }

  sysapiPrintf("Backup elected:   %d\n", cpdmOprData->cpBackupElected);
  sysapiPrintf("Warm restart:     %d\n", cpdmOprData->cpWarmRestart);
  sysapiPrintf("Ckpt required:    %d\n", cpdmOprData->cpCkptReqd);
  sysapiPrintf("Ckpt in progress: %d\n", cpdmOprData->cpCkptInProg);
  sysapiPrintf("\n");

  sysapiPrintf ("   MAC Address      IP Address    CpId Intf Prtl  Verify Uname SSt  STo\n");
  sysapiPrintf ("----------------- --------------- ---- ---- ----- ------ ----- ---- ---\n");

  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  while ((ptrData=avlSearchLVL7(&clientConnStatusTree, &macAddr, AVL_NEXT)) != L7_NULLPTR)
  {
    sysapiPrintf("%02x:%02x:%02x:%02x:%02x:%02x ",
                 ptrData->macAddr.addr[0], ptrData->macAddr.addr[1], ptrData->macAddr.addr[2],
                 ptrData->macAddr.addr[3], ptrData->macAddr.addr[4], ptrData->macAddr.addr[5]);

    sysapiPrintf("%-15s ", osapiInet_ntoa(ptrData->ip));
    sysapiPrintf("%-4u ", ptrData->cpId);
    sysapiPrintf("%-4u ", ptrData->intfId);

    if (ptrData->protocolMode == L7_LOGIN_TYPE_HTTPS) sysapiPrintf("%-5s ", "https");
    else sysapiPrintf("%-5s ", "http");

    if (ptrData->verifyMode == CP_VERIFY_MODE_LOCAL) sysapiPrintf("%-6s ", "local");
    else if (ptrData->verifyMode == CP_VERIFY_MODE_RADIUS) sysapiPrintf("%-6s ", "radius");
    else sysapiPrintf("%-6s ", "guest");

    sysapiPrintf("%-5s ", ptrData->userName);
    sysapiPrintf("%-4u ", ptrData->sessionStart);
    sysapiPrintf("%-3u\n", ptrData->limits.sessionTimeout);

    memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
  }

  return;
}

#include "l7utils_api.h"

/*********************************************************************
* @purpose  Add a few CP clients
*
* @param    L7_uint32 count    @b{(input)} number of CP clients
* @param    L7_char8  macStart @b{(input)} first MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cpCkptTestClientAdd(L7_uint32 count, L7_char8 *macStart)
{
  L7_enetMacAddr_t macAddr;
  L7_uchar8        uName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_uint32        i;

  if ((count == 0) || (macStart == L7_NULLPTR))
  {
    sysapiPrintf ("Invalid parameters.\n");
    return L7_FAILURE;
  }

  if (l7utilsMacAddrStringToHex(macStart, macAddr.addr) != L7_SUCCESS)
  {
    sysapiPrintf ("Invalid MAC address.\n");
    return L7_FAILURE;
  }

  memset(uName, 0, CP_USER_LOCAL_USERNAME_MAX);
  for (i = 0; i < count; i++)
  {
    osapiSnprintf(uName, sizeof(uName), "user%u", i+1);
    if (cpdmClientConnStatusAdd(&macAddr, uName, i+1, i+1) != L7_SUCCESS)
    {
      sysapiPrintf("Failed to add client %02x:%02x:%02x:%02x:%02x:%02x, count %d.\n",
                   macAddr.addr[0], macAddr.addr[1], macAddr.addr[2],
                   macAddr.addr[3], macAddr.addr[4], macAddr.addr[5], i);
      return L7_FAILURE;
    }

    if (macAddr.addr[5] == 0xFF)
    {
      macAddr.addr[4]++;
      macAddr.addr[5] = 0;
    }
    else
    {
      macAddr.addr[5]++;
    }
  }

  return L7_SUCCESS;
}

#ifdef L7_CP_WIO_PACKAGE
/*********************************************************************
* @purpose  Authenticate a few wired CP clients
*
* @param    L7_uint32 count    @b{(input)} number of CP clients
* @param    L7_char8  macStart @b{(input)} first MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cpCkptTestWiredClientAuth(L7_uint32 count, L7_char8 *macStart)
{
  L7_enetMacAddr_t macAddr;
  L7_uchar8        uName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_uint32        i;

  if ((count == 0) || (macStart == L7_NULLPTR))
  {
    sysapiPrintf ("Invalid parameters.\n");
    return L7_FAILURE;
  }

  if (l7utilsMacAddrStringToHex(macStart, macAddr.addr) != L7_SUCCESS)
  {
    sysapiPrintf ("Invalid MAC address.\n");
    return L7_FAILURE;
  }

  memset(uName, 0, CP_USER_LOCAL_USERNAME_MAX);
  for (i = 0; i < count; i++)
  {
    osapiSnprintf(uName, sizeof(uName), "user%u", i+1);
    if (wioClientAuthReq(i+1, &macAddr, i+1) != L7_SUCCESS)
    {
      sysapiPrintf("Failed to auth wired client %02x:%02x:%02x:%02x:%02x:%02x, count %d.\n",
                   macAddr.addr[0], macAddr.addr[1], macAddr.addr[2],
                   macAddr.addr[3], macAddr.addr[4], macAddr.addr[5], i);
      return L7_FAILURE;
    }

    if (macAddr.addr[5] == 0xFF)
    {
      macAddr.addr[4]++;
      macAddr.addr[5] = 0;
    }
    else
    {
      macAddr.addr[5]++;
    }
  }

  return L7_SUCCESS;
}
#endif

#endif  /* L7_NSF_PACKAGE */

