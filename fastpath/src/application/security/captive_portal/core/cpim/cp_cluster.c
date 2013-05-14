/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2008
 *
 **********************************************************************
 *
 * @filename   cp_util.c
 *
 * @purpose    Captive Portal (CP) Utilities File
 *
 * @component  CP
 *
 * @comments   None
 *
 * @create     01/16/2008
 *
 * @author     rjindal
 *
 * @end
 *
 *********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "osapi_support.h"
#include "avl_api.h"
#include "log.h"
#include "cpdm_connstatus_api.h"
#include "cp_cluster_msg.h"
#include "clustering_api.h"
#include "clustering_commdefs.h"
#include "cp_cluster.h"
#include "cpdm.h"
#include "cpdm_clustering_api.h"
#include "cpdm_api.h"
#include "captive_portal_common.h"
#include "cpdm_user_api.h"
#include "cpdm_web_api.h"
#include "trap_captive_portal_api.h"
#include "wdm_api.h"

#define SEMA_TAKE(access)   \
  if (CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);
#define SEMA_GIVE(access)   \
  if (CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);


/* What is the size of one more item of client connection information? */

#define CONN_CLIENT_INFO_SIZE              \
  ((9 * CP_CLUSTER_IE_HDR_SIZE)            \
   + CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE \
   + CP_CLUSTER_IE_CLIENT_STATUS_SIZE      \
   + CP_CLUSTER_IE_CLIENT_IP_SIZE          \
   + CP_CLUSTER_IE_USERNAME_SIZE           \
   + CP_CLUSTER_IE_PROTOCOL_MODE_SIZE      \
   + CP_CLUSTER_IE_VERIFY_MODE_SIZE        \
   + CP_CLUSTER_IE_CP_INTERFACE_INDEX_SIZE \
   + CP_CLUSTER_IE_CP_INST_SIZE            \
   + CP_CLUSTER_IE_STATS_DATA_SIZE)

/* What is the size of one more item of client notification information? */
#define CONN_CLIENT_NOTIFY_SIZE            \
  ((4 * CP_CLUSTER_IE_HDR_SIZE)            \
   + CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE \
   + CP_CLUSTER_IE_IP_ADDRESS_TYPE         \
   + CP_CLUSTER_IE_CP_INST_SIZE            \
   + CP_CLUSTER_IE_CLIENT_STATUS_SIZE      )


/* What is the size of one more item of client status information? */

#define CONN_CLIENT_STATS_SIZE             \
  ((2 * CP_CLUSTER_IE_HDR_SIZE)            \
   + CP_STATISTICS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE \
   + CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE)


L7_BOOL cpClusterMsgError = L7_FALSE;

extern cpdmCfgData_t    *cpdmCfgData;
extern cpdmOprData_t    *cpdmOprData;
extern avlTree_t         peerSwClientTree;
extern avlTree_t         peerSwTree;

extern L7_enetMacAddr_t *resendPeer;
extern void             *pCPStatsQ;

extern cpdmGlobal_t             *ptrCpGlobalData;
extern cpdmConfigData_t         *ptrCpConfigData;
extern L7_uint32                 numCP;
extern cpLocaleKey_t            *ptrCpWebID;
extern cpLocaleCodeLink_t       *ptrCpCodeLink;
extern ifNumCPPair_t            *ptrCpInterfaceAssocData;
extern cpdmUserGroupData_t      *ptrCpUserGroupData;
extern L7_uint32                 numGroup;
extern cpdmUser_t               *ptrCpUsers;
extern L7_uint32                 numUser;
extern cpdmUserGroupAssocData_t *ptrCpUserGroupAssocData;

extern L7_RC_t cpdmUserEntryPurge(void);
extern L7_RC_t cpdmCPConfigInit(L7_uint32 maxConfigs);
extern L7_RC_t cpIntfAssocInit(L7_uint32 maxAssocs);
extern L7_RC_t cpdmUserGroupEntryInit(void);
extern L7_RC_t cpdmUserGroupAssocEntryInit(void);

static L7_uint32 webIndex;
static L7_uint32 ifIndex;
static L7_uint32 ugIndex;
static L7_char8           *failed_to_get = "Failed to get";


#if 0
void printMsg(const L7_uchar8 * tag, L7_uchar8 * buf, L7_uint32 len)
{
  L7_uint32 ctr;

  printf("\n ----  %s ----", tag);
  printf("\n ------------------------------------------------\n");
  for(ctr = 0; ctr < len; ctr++) {
    if (ctr % 8 == 0)
      printf(" ");
    if (ctr % 16 == 0)
      printf("\n");
    printf("%02x ", buf[ctr]);
  }
  printf("\n ------------------------------------------------\n");
}
#endif


/*********************************************************************
* @purpose  Build CP client notification message
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_BOOL          authStatus    @b{(input)} L7_TRUE or L7_FALSE
* @param    L7_uchar8        buf           @b{(input/output)} Pointer to store msg
* @param    L7_uint32        offset        @b{(input/output)} Pointer to offset into buffer
* @param    L7_ushort16      len           @b{(input/output)} Pointer to msg length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  If you change this function, you MUST change the #definition
*            of CONN_CLIENT_NOTIFY_SIZE.  Ignore this at your peril.
* @end
*********************************************************************/
static L7_RC_t cpClientNotificationMsgBuild(L7_enetMacAddr_t clientMacAddr, L7_BOOL authStatus,
                                            L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  cpClusterPduHdl_t        pdu;
  L7_ushort16              pktLen = 0;
  cpClusterMsgElementHdr_t element;
  L7_uchar8                status;
  L7_IP_ADDR_t             ipAddr = 0;
  cpId_t                   cpId = 0;
  L7_uint32                tmp32 = 0;

  if ((L7_NULLPTR == buf) || (L7_NULLPTR == offset) || (L7_NULLPTR == len))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: bad parms", __FUNCTION__);
    return L7_FAILURE;
  }

  if (authStatus == L7_TRUE)
  {
    if (cpdmClientConnStatusIpGet(&clientMacAddr, &ipAddr) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s client IP address", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }

    if (cpdmClientConnStatusCpIdGet(&clientMacAddr, &cpId) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s client CPID.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
  }

  pdu.bufPtr = buf;
  pdu.offset = *offset;

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_TYPE,
                         CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE;
  CP_PKT_MAC_ADDR_SET(clientMacAddr.addr, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_IP_ADDRESS_TYPE,
                         CP_CLUSTER_IE_IP_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_IP_ADDRESS_SIZE;
  tmp32 = (L7_uint32)ipAddr;
  CP_PKT_INT32_SET(tmp32, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INST_TYPE,
                         CP_CLUSTER_IE_CP_INST_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INST_SIZE;
  tmp32 = (L7_uint32)cpId;
  CP_PKT_INT32_SET(tmp32, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_STATUS_TYPE,
                         CP_CLUSTER_IE_CLIENT_STATUS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_STATUS_SIZE;
  status = (L7_uchar8)authStatus;
  CP_PKT_INT8_SET(status, pdu.bufPtr, pdu.offset);

  *len += pktLen;
  *offset = pdu.offset;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build CP client authentication request message
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_uchar8        buf           @b{(input/output)} Pointer to store msg
* @param    L7_uint32        offset        @b{(input/output)} Pointer to offset into buffer
* @param    L7_ushort16      len           @b{(input/output)} Pointer to msg length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t cpClientAuthReqMsgBuild(L7_enetMacAddr_t clientMacAddr,
                                       L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  cpClusterPduHdl_t        pdu;
  L7_ushort16              pktLen = 0;
  cpClusterMsgElementHdr_t element;

  pdu.bufPtr = buf;
  pdu.offset = *offset;

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_TYPE,
                         CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE;
  CP_PKT_MAC_ADDR_SET(clientMacAddr.addr, pdu.bufPtr, pdu.offset);

  *len += pktLen;
  *offset = pdu.offset;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build CP client authentication reply message
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_uchar8        buf           @b{(input/output)} Pointer to store msg
* @param    L7_uint32        offset        @b{(input/output)} Pointer to offset into buffer
* @param    L7_ushort16      len           @b{(input/output)} Pointer to store msg length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t cpClientAuthReplyMsgBuild(L7_enetMacAddr_t clientMacAddr,
                                         L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  cpClusterPduHdl_t        pdu;
  L7_ushort16              pktLen = 0;
  cpClusterMsgElementHdr_t element;
  cpId_t                   cpId16 = 0;
  L7_uint32                intf = 0, cpId = 0, time = 0;
  L7_uchar8                uName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_uchar8                pwd[CP_USER_LOCAL_PASSWORD_MAX+1];
  uId_t                    uId = 0;
  L7_uint64                bTx = 0, bRx = 0, pTx = 0, pRx = 0;
  L7_uint32                upRate = 0, dnRate = 0;

  pdu.bufPtr = buf;
  pdu.offset = *offset;

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_TYPE,
                         CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE;
  CP_PKT_MAC_ADDR_SET(clientMacAddr.addr, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_AUTHENTICATION_DATA_TYPE,
                         CP_CLUSTER_IE_AUTHENTICATION_DATA_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_AUTHENTICATION_DATA_SIZE;
  if (cpdmClientConnStatusIntfIdGet(&clientMacAddr, &intf) == L7_SUCCESS)
  {
    CP_PKT_INT32_SET(intf, pdu.bufPtr, pdu.offset);
  }

  if (cpdmClientConnStatusCpIdGet(&clientMacAddr, &cpId16) == L7_SUCCESS)
  {
    cpId = (L7_uint32)cpId16;
    CP_PKT_INT32_SET(cpId, pdu.bufPtr, pdu.offset);
  }

  if (cpdmClientConnStatusSessionTimeoutGet(&clientMacAddr, &time) == L7_SUCCESS)
  {
    CP_PKT_INT32_SET(time, pdu.bufPtr, pdu.offset);
  }

  if (cpdmClientConnStatusUserNameGet(&clientMacAddr, uName) == L7_SUCCESS)
  {
    CP_PKT_STRING_SET(uName, CP_CLUSTER_IE_USERNAME_SIZE, pdu.bufPtr, pdu.offset);

    if ((cpdmUserEntryByNameGet(uName, &uId) == L7_SUCCESS) &&
        (cpdmUserEntryPasswordGet(uId, pwd) == L7_SUCCESS))
    {
      CP_PKT_STRING_SET(pwd, CP_USER_LOCAL_PASSWORD_MAX, pdu.bufPtr, pdu.offset);
    }
  }

  if (cpdmClientConnStatusStatisticsGet(&clientMacAddr, &bTx, &bRx, &pTx, &pRx) == L7_SUCCESS)
  {
    CP_PKT_INT64_SET(bTx, pdu.bufPtr, pdu.offset);
    CP_PKT_INT64_SET(bRx, pdu.bufPtr, pdu.offset);
    CP_PKT_INT64_SET(pTx, pdu.bufPtr, pdu.offset);
    CP_PKT_INT64_SET(pRx, pdu.bufPtr, pdu.offset);
  }

  if (cpdmCPConfigUserUpRateGet(cpId16, &upRate) == L7_SUCCESS)
  {
    CP_PKT_INT32_SET(upRate, pdu.bufPtr, pdu.offset);
  }

  if (cpdmCPConfigUserDownRateGet(cpId16, &dnRate) == L7_SUCCESS)
  {
    CP_PKT_INT32_SET(dnRate, pdu.bufPtr, pdu.offset);
  }

  *len += pktLen;
  *offset = pdu.offset;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build CP controller command message
*
* @param    L7_enetMacAddr_t clientMacAddr  @b{(input)} client MAC address
* @param    L7_uint32        cpId           @b{(input)} CP instance
* @param    L7_uint32        cpBlkUnblk     @b{(input)} CP instance to block/unblock
* @param    L7_BOOL          blkUnblkStatus @b{(input)} L7_TRUE or L7_FALSE
* @param    L7_uchar8        buf            @b{(input/output)} Pointer to store msg
* @param    L7_uint32        offset         @b{(input/output)} Pointer to offset into buffer
* @param    L7_ushort16      len            @b{(input/output)} Pointer to store msg length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t cpControllerCmdMsgBuild(L7_enetMacAddr_t clientMacAddr, L7_uint32 cpId,
                                       L7_uint32 cpBlkUnblk, L7_BOOL blkUnblkStatus,
                                       L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  cpClusterPduHdl_t         pdu;
  L7_ushort16               pktLen = 0;
  cpClusterMsgElementHdr_t  element;
  L7_CP_INST_BLOCK_STATUS_t status = L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED;
  L7_uint32                 blkVal = 0, unblkVal = 0;

  pdu.bufPtr = buf;
  pdu.offset = *offset;

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_TYPE,
                         CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE;
  CP_PKT_MAC_ADDR_SET(clientMacAddr.addr, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INST_TYPE,
                         CP_CLUSTER_IE_CP_INST_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INST_SIZE;
  CP_PKT_INT32_SET(cpId, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_DEAUTHENTICATE_ALL_TYPE,
                         CP_CLUSTER_IE_CP_DEAUTHENTICATE_ALL_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_DEAUTHENTICATE_ALL_SIZE;

  if (blkUnblkStatus == L7_TRUE)
  {
    if (cpdmCPConfigBlockedStatusGet((cpId_t)cpBlkUnblk, &status) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s cpId %d block status", __FUNCTION__, failed_to_get, cpBlkUnblk);
      return L7_FAILURE;
    }

    if (status == L7_CP_INST_BLOCK_STATUS_BLOCKED ||
        status == L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING)
    {
      blkVal = cpBlkUnblk;
    }
    else
    {
      unblkVal = cpBlkUnblk;
    }
  }

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_BLOCK_CP_INSTANCE_TYPE,
                         CP_CLUSTER_IE_BLOCK_CP_INSTANCE_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_BLOCK_CP_INSTANCE_SIZE;
  CP_PKT_INT32_SET(blkVal, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_UNBLOCK_CP_INSTANCE_TYPE,
                         CP_CLUSTER_IE_UNBLOCK_CP_INSTANCE_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_UNBLOCK_CP_INSTANCE_SIZE;
  CP_PKT_INT32_SET(unblkVal, pdu.bufPtr, pdu.offset);

  *len += pktLen;
  *offset = pdu.offset;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Build CP connected clients update message
 *
 * @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
 * @param    L7_BOOL          authStatus    @b{(input)} L7_TRUE or L7_FALSE
 * @param    L7_uchar8        buf           @b{(input/output)} Pointer to store msg
 * @param    L7_uint32        offset        @b{(input/output)} Pointer to offset into buffer
 * @param    L7_ushort16      len           @b{(input/output)} Pointer to store msg length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
static L7_RC_t cpConnClientsUpdateMsgBuild(L7_enetMacAddr_t clientMacAddr, L7_BOOL authStatus,
                                           L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  cpClusterPduHdl_t        pdu;
  L7_ushort16              pktLen = 0;
  cpClusterMsgElementHdr_t element;
  L7_uchar8                status;
  L7_IP_ADDR_t             ipAddr = 0;
  L7_uchar8                userName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_LOGIN_TYPE_t          prtlMode = L7_LOGIN_TYPE_HTTP;
  CP_VERIFY_MODE_t         verifyMode = CP_VERIFY_MODE_GUEST;
  cpId_t                   cpId16 = 0;
  L7_uint32                intf = 0, cpId = 0;
  L7_uint64                bTx = 0, bRx = 0, pTx = 0, pRx = 0;

  /* If you update this function, YOU MUST also update the CONN_CLIENT_INFO_SIZE
   * #definition near the top of this file!  Failure to do so invites hard crashes.
   */

  pdu.bufPtr = buf;
  pdu.offset = *offset;
  memset(userName, 0, sizeof(userName));

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_TYPE,
                         CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE;
  CP_PKT_MAC_ADDR_SET(clientMacAddr.addr, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_STATUS_TYPE,
                         CP_CLUSTER_IE_CLIENT_STATUS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_STATUS_SIZE;
  status = (L7_uchar8)authStatus;
  CP_PKT_INT8_SET(status, pdu.bufPtr, pdu.offset);

  if (authStatus == L7_TRUE)
  {
    if (cpdmClientConnStatusIpGet(&clientMacAddr, &ipAddr) != L7_SUCCESS ||
        cpdmClientConnStatusUserNameGet(&clientMacAddr, userName) != L7_SUCCESS ||
        cpdmClientConnStatusProtocolModeGet(&clientMacAddr, &prtlMode) != L7_SUCCESS ||
        cpdmClientConnStatusVerifyModeGet(&clientMacAddr, &verifyMode) != L7_SUCCESS ||
        cpdmClientConnStatusIntfIdGet(&clientMacAddr, &intf) != L7_SUCCESS ||
        cpdmClientConnStatusCpIdGet(&clientMacAddr, &cpId16) != L7_SUCCESS ||
        cpdmClientConnStatusStatisticsGet(&clientMacAddr, &bTx, &bRx, &pTx, &pRx) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s client %02x:%02x:%02x:%02x:%02x:%02x info.\n",
              __FUNCTION__, failed_to_get,
              clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
              clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
      return L7_FAILURE;
    }
  }

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_IP_TYPE,
                         CP_CLUSTER_IE_CLIENT_IP_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_IP_SIZE;
  CP_PKT_INT32_SET(ipAddr, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USERNAME_TYPE,
                         CP_CLUSTER_IE_USERNAME_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USERNAME_SIZE;
  CP_PKT_STRING_SET(userName, CP_CLUSTER_IE_USERNAME_SIZE, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_PROTOCOL_MODE_TYPE,
                         CP_CLUSTER_IE_PROTOCOL_MODE_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_PROTOCOL_MODE_SIZE;
  CP_PKT_INT32_SET(prtlMode, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_VERIFY_MODE_TYPE,
                         CP_CLUSTER_IE_VERIFY_MODE_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_VERIFY_MODE_SIZE;
  CP_PKT_INT32_SET(verifyMode, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INTERFACE_INDEX_TYPE,
                         CP_CLUSTER_IE_CP_INTERFACE_INDEX_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INTERFACE_INDEX_SIZE;
  CP_PKT_INT32_SET(intf, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INST_TYPE,
                         CP_CLUSTER_IE_CP_INST_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INST_SIZE;
  cpId = (L7_uint32)cpId16;
  CP_PKT_INT32_SET(cpId, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_STATS_DATA_TYPE,
                         CP_CLUSTER_IE_STATS_DATA_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_STATS_DATA_SIZE;
  CP_PKT_INT64_SET(bTx, pdu.bufPtr, pdu.offset);
  CP_PKT_INT64_SET(bRx, pdu.bufPtr, pdu.offset);
  CP_PKT_INT64_SET(pTx, pdu.bufPtr, pdu.offset);
  CP_PKT_INT64_SET(pRx, pdu.bufPtr, pdu.offset);

  *len += pktLen;
  *offset = pdu.offset;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Build CP instance interface update message
 *
 * @param    L7_uint32   cpId          @b{(input)} CP ID
 * @param    L7_BOOL     cpIdStatus    @b{(input)} L7_TRUE or L7_FALSE
 * @param    L7_uint32   cpIdAuthUsers @b{(input)} CP instance authenticated users
 * @param    L7_uint32   intf          @b{(input)} CP interface
 * @param    L7_uchar8   buf           @b{(input/output)} Pointer to store msg
 * @param    L7_uint32   offset        @b{(input/output)} Pointer to offset into buffer
 * @param    L7_ushort16 len           @b{(input/output)} Pointer to store msg length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
static L7_RC_t cpInstIntfUpdateMsgBuild(L7_uint32 cpId, L7_BOOL cpIdStatus,
                                        L7_uint32 cpIdAuthUsers, L7_uint32 intf,
                                        L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  cpClusterPduHdl_t        pdu;
  L7_ushort16              pktLen = 0;
  cpClusterMsgElementHdr_t element;
  cpId_t                   nextCpId;
  L7_uchar8                cpStatus, ifStatus;
  L7_BOOL                  intfStatus, done = L7_TRUE;
  L7_uint32                intfAuthUsers = 0;

  pdu.bufPtr = buf;
  pdu.offset = *offset;

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INST_TYPE,
                         CP_CLUSTER_IE_CP_INST_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INST_SIZE;
  CP_PKT_INT32_SET(cpId, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INST_STATUS_TYPE,
                         CP_CLUSTER_IE_CP_INST_STATUS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INST_STATUS_SIZE;
  cpStatus = (L7_uchar8)cpIdStatus;
  CP_PKT_INT8_SET(cpStatus, pdu.bufPtr, pdu.offset);

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INST_AUTH_USERS_TYPE,
                         CP_CLUSTER_IE_CP_INST_AUTH_USERS_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INST_AUTH_USERS_SIZE;
  CP_PKT_INT32_SET(cpIdAuthUsers, pdu.bufPtr, pdu.offset);

  /* if 'intf' is 0, loop thru all cpId associated interfaces, else only that 'intf' */
  do
  {
    static L7_char8 * failed = "Failed to get intf";
    if (intf != 0)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intf, L7_SYSNAME, ifName);

      if (cpdmIntfStatusGetIsEnabled(intf, &intfStatus) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: %s %d, %s status", __FUNCTION__, failed, intf, ifName);
        return L7_FAILURE;
      }

      if (cpdmIntfStatusGetUsers(intf, &intfAuthUsers) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: %s %d, %s auth users", __FUNCTION__, failed, intf, ifName);
        return L7_FAILURE;
      }

      CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INTERFACE_INDEX_TYPE,
                             CP_CLUSTER_IE_CP_INTERFACE_INDEX_SIZE, pdu.bufPtr, pdu.offset);
      pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INTERFACE_INDEX_SIZE;
      CP_PKT_INT32_SET(intf, pdu.bufPtr, pdu.offset);

      CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INST_INTF_STATUS_TYPE,
                             CP_CLUSTER_IE_CP_INST_INTF_STATUS_SIZE, pdu.bufPtr, pdu.offset);
      pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INST_INTF_STATUS_SIZE;
      ifStatus = (L7_uchar8)intfStatus;
      CP_PKT_INT8_SET(ifStatus, pdu.bufPtr, pdu.offset);

      CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INST_INTF_AUTH_USERS_TYPE,
                             CP_CLUSTER_IE_CP_INST_INTF_AUTH_USERS_SIZE, pdu.bufPtr, pdu.offset);
      pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INST_INTF_AUTH_USERS_SIZE;
      CP_PKT_INT32_SET(intfAuthUsers, pdu.bufPtr, pdu.offset);

      if (done == L7_TRUE)
      {
        *len += pktLen;
        *offset = pdu.offset;
        return L7_SUCCESS;
      }
    }
    done = L7_FALSE;

  } while (cpdmCPConfigIntIfNumNextGet((cpId_t)cpId, intf, &nextCpId, &intf) == L7_SUCCESS &&
           nextCpId == cpId);

  *len += pktLen;
  *offset = pdu.offset;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Build CP statistics update message
 *
 * @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
 * @param    L7_uchar8        buf           @b{(input/output)} Pointer to store msg
 * @param    L7_uint32        offset        @b{(input/output)} Pointer to offset into buffer
 * @param    L7_ushort16      len           @b{(input/output)} Pointer to store msg length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
static L7_RC_t cpStatsUpdateMsgBuild(L7_enetMacAddr_t clientMacAddr,
                                     L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  cpClusterPduHdl_t        pdu;
  L7_ushort16              pktLen = 0;
  cpClusterMsgElementHdr_t element;
  L7_uint64                bTx = 0, bRx = 0, pTx = 0, pRx = 0;

  pdu.bufPtr = buf;
  pdu.offset = *offset;

  CP_PKT_ELEMENT_HDR_SET(element, CP_STATISTICS_UPDATE_IE_TYPE_CLIENT_MAC_ADDRESS,
                         CP_STATISTICS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE,
                         pdu.bufPtr, pdu.offset);
  CP_PKT_MAC_ADDR_SET(clientMacAddr.addr, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_STATISTICS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE;


  CP_PKT_ELEMENT_HDR_SET(element, CP_STATISTICS_UPDATE_IE_TYPE_STATS_DATA,
                         CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE, pdu.bufPtr, pdu.offset);
  if (cpdmClientConnStatusStatisticsGet(&clientMacAddr, &bTx, &bRx, &pTx, &pRx) == L7_SUCCESS)
  {
    CP_PKT_INT64_SET(bTx, pdu.bufPtr, pdu.offset);
    CP_PKT_INT64_SET(bRx, pdu.bufPtr, pdu.offset);
    CP_PKT_INT64_SET(pTx, pdu.bufPtr, pdu.offset);
    CP_PKT_INT64_SET(pRx, pdu.bufPtr, pdu.offset);
  }
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE;

  *len += pktLen;
  *offset = pdu.offset;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP global config message
*
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t cpConfigMsgGlobalBuildSend(L7_enetMacAddr_t swMacAddr)
{
  L7_uchar8                 tmp8 = 0, buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_uint32                 val;

  memset(buf, 0, sizeof(buf));
  pdu.bufPtr = buf;
  pdu.offset = 0;

  message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
  message->msgType = osapiHtons(CP_CLUSTER_CP_CONFIG_SYNC_MSG);
  message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
  pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;


  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_CONFIG_MSG_IE_GLOBAL_TYPE,
                         CP_CLUSTER_CONFIG_MSG_IE_GLOBAL_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_CONFIG_MSG_IE_GLOBAL_SIZE;

  /* cp mode */
  if (cpdmGlobalModeGet(&val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP global mode", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_GLOBAL_MODE_TYPE,
                         CP_CLUSTER_IE_CP_GLOBAL_MODE_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_GLOBAL_MODE_SIZE;
  tmp8 = (L7_uchar8)val;
  CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

  /* additional http port */
  if (cpdmGlobalHTTPPortGet(&val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s global http port", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_HTTP_PORT_TYPE,
                         CP_CLUSTER_IE_HTTP_PORT_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_HTTP_PORT_SIZE;
  CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

#ifdef L7_MGMT_SECURITY_PACKAGE
  /* default http secure port 1 */
  if (cpdmGlobalHTTPSecurePort1Get(&val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s global additional http secure port1", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT1_TYPE,
                         CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT1_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT1_SIZE;
  CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

  /* additional http secure port 2 */
  if (cpdmGlobalHTTPSecurePort2Get(&val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s global additional http secure port2", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT2_TYPE,
                         CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT2_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT2_SIZE;
  CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);
#endif

  /* peer switch statistics reporting interval */
  if (cpdmGlobalStatusPeerSwStatsReportIntervalGet(&val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s peer switch stats reporting interval", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_PEER_STATS_REPORT_INTVL_TYPE,
                         CP_CLUSTER_IE_PEER_STATS_REPORT_INTVL_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_PEER_STATS_REPORT_INTVL_SIZE;
  CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

  /* auth session timeout */
  if (cpdmGlobalStatusSessionTimeoutGet(&val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s auth session timeout", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_AUTH_SESSION_TIMEOUT_TYPE,
                         CP_CLUSTER_IE_AUTH_SESSION_TIMEOUT_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_AUTH_SESSION_TIMEOUT_SIZE;
  CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

  /* client authentication failure traps */
  if (cpdmGlobalTrapModeGet(CP_TRAP_AUTH_FAILURE, &val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s client authentication failure trap mode", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_AUTH_FAILURE_TRAP_TYPE,
                         CP_CLUSTER_IE_CLIENT_AUTH_FAILURE_TRAP_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_AUTH_FAILURE_TRAP_SIZE;
  tmp8 = (L7_uchar8)val;
  CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

  /* client connection traps */
  if (cpdmGlobalTrapModeGet(CP_TRAP_CLIENT_CONNECTED, &val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s client connection trap mode", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_CONN_TRAP_TYPE,
                         CP_CLUSTER_IE_CLIENT_CONN_TRAP_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_CONN_TRAP_SIZE;
  tmp8 = (L7_uchar8)val;
  CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

  /* client database full traps */
  if (cpdmGlobalTrapModeGet(CP_TRAP_CONNECTION_DB_FULL, &val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s client database full trap mode", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_DB_FULL_TRAP_TYPE,
                         CP_CLUSTER_IE_CLIENT_DB_FULL_TRAP_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_DB_FULL_TRAP_SIZE;
  tmp8 = (L7_uchar8)val;
  CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

  /* client disconnection traps */
  if (cpdmGlobalTrapModeGet(CP_TRAP_CLIENT_DISCONNECTED, &val) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s client disconnection trap mode", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CLIENT_DISCONN_TRAP_TYPE,
                         CP_CLUSTER_IE_CLIENT_DISCONN_TRAP_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CLIENT_DISCONN_TRAP_SIZE;
  tmp8 = (L7_uchar8)val;
  CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

  CP_DLOG(CPD_LEVEL_LOG, "%s: Sending CP global cfg msg of size %d to switch %02x:%02x:%02x:%02x:%02x:%02x",
          __FUNCTION__,
          message->msgLen, swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
          swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);

  message->msgLen = osapiHtons(message->msgLen);

  if (cpClusterSwitchEntryGet(swMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg not sent",
            __FUNCTION__,
            swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
            swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
    return L7_FAILURE;
  }

  if (clusterMsgSend(&swMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                     CP_CLUSTER_CP_CONFIG_SYNC_MSG, pdu.offset, buf) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send CP global cfg msg to switch %02x:%02x:%02x:%02x:%02x:%02x",
            __FUNCTION__,
            swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
            swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP instances config message
*
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t cpConfigMsgCPsBuildSend(L7_enetMacAddr_t swMacAddr)
{
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu, tmpPdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_uint32                 val, numWebID = 0, intf, numIntf = 0;
  cpId_t                    cpId = 0, next_cpId;
  L7_char8                  name[CP_NAME_MAX+1];
  L7_char8                  url[CP_WELCOME_URL_MAX+1];
  L7_char8                  radius[CP_RADIUS_AUTH_SERVER_MAX+1];
  gpId_t                    gId;
  L7_char8                  fColor[CP_FOREGROUND_COLOR_MAX+1];
  L7_char8                  bColor[CP_BACKGROUND_COLOR_MAX+1];
  L7_char8                  sColor[CP_SEPARATOR_COLOR_MAX+1];
  webId_t                   webId;
  L7_char8                  code[CP_LANG_CODE_MAX+1];
  L7_char8                  link[CP_LOCALE_LINK_MAX+1];
  L7_uchar8                 tmp8 = 0;

  cpId = 0;
  while (cpdmCPConfigNextGet(cpId, &cpId) == L7_SUCCESS)
  {
    memset(buf, 0, sizeof(buf));
    pdu.bufPtr = buf;
    pdu.offset = 0;

    message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
    message->msgType = osapiHtons(CP_CLUSTER_CP_CONFIG_SYNC_MSG);
    message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
    pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_CONFIG_MSG_IE_CP_TYPE,
                           CP_CLUSTER_CONFIG_MSG_IE_CP_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_CONFIG_MSG_IE_CP_SIZE;

    /* configuration id */
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_ID_TYPE,
                           CP_CLUSTER_IE_CP_ID_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_ID_SIZE;
    tmp8 = (L7_uchar8)cpId;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

    /* configuration name */
    if (cpdmCPConfigNameGet(cpId, name) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s configuration name", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_NAME_TYPE,
                           CP_CLUSTER_IE_CP_NAME_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_NAME_SIZE;
    CP_PKT_STRING_SET(name, CP_CLUSTER_IE_CP_NAME_SIZE, pdu.bufPtr, pdu.offset);

    /* configuration mode */
    if (cpdmCPConfigModeGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s configuration mode", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_MODE_TYPE,
                           CP_CLUSTER_IE_CP_MODE_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_MODE_SIZE;
    tmp8 = (L7_uchar8)val;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

    /* protocol mode */
    if (cpdmCPConfigProtocolModeGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s protocol mode", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_PROTOCOL_TYPE,
                           CP_CLUSTER_IE_CP_PROTOCOL_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_PROTOCOL_SIZE;
    tmp8 = (L7_uchar8)val;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

    /* verification mode */
    if (cpdmCPConfigVerifyModeGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s verification mode", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_VERIFY_TYPE,
                           CP_CLUSTER_IE_CP_VERIFY_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_VERIFY_SIZE;
    tmp8 = (L7_uchar8)val;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

    /* redirect mode */
    if (cpdmCPConfigRedirectModeGet(cpId, &tmp8) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s redirect mode", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_REDIRECT_MODE_TYPE,
                           CP_CLUSTER_IE_CP_REDIRECT_MODE_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_REDIRECT_MODE_SIZE;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

    /* redirect url */
    if (cpdmCPConfigRedirectURLGet(cpId, url) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s redirect url", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_REDIRECT_URL_TYPE,
                           CP_CLUSTER_IE_CP_REDIRECT_URL_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_REDIRECT_URL_SIZE;
    CP_PKT_STRING_SET(url, CP_CLUSTER_IE_CP_REDIRECT_URL_SIZE, pdu.bufPtr, pdu.offset);

    /* radius auth server */
    if (cpdmCPConfigRadiusAuthServerGet(cpId, radius) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s radius server", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_RADIUS_AUTH_SERVER_TYPE,
                           CP_CLUSTER_IE_RADIUS_AUTH_SERVER_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_RADIUS_AUTH_SERVER_SIZE;
    CP_PKT_STRING_SET(radius, CP_CLUSTER_IE_RADIUS_AUTH_SERVER_SIZE, pdu.bufPtr, pdu.offset);

    /* user group */
    if (cpdmCPConfigGpIdGet(cpId, &gId) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s groupID", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_GROUP_ID_TYPE,
                           CP_CLUSTER_IE_GROUP_ID_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_GROUP_ID_SIZE;
    tmp8 = (L7_uchar8)gId;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

    /* session timeout */
    if (cpdmCPConfigSessionTimeoutGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s session timeout", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_SESSION_TIMEOUT_TYPE,
                           CP_CLUSTER_IE_CP_SESSION_TIMEOUT_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_SESSION_TIMEOUT_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* idle timeout */
    if (cpdmCPConfigIdleTimeoutGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s idle timeout", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_IDLE_TIMEOUT_TYPE,
                           CP_CLUSTER_IE_CP_IDLE_TIMEOUT_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_IDLE_TIMEOUT_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* bandwidth up */
    if (cpdmCPConfigUserUpRateGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP bandwidth up rate", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_BAND_UP_TYPE,
                           CP_CLUSTER_IE_CP_BAND_UP_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_BAND_UP_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* bandwidth down */
    if (cpdmCPConfigUserDownRateGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP bandwidth down rate", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_BAND_DOWN_TYPE,
                           CP_CLUSTER_IE_CP_BAND_DOWN_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_BAND_DOWN_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* max input octets */
    if (cpdmCPConfigMaxInputOctetsGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP max input octets", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INPUT_OCTET_TYPE,
                           CP_CLUSTER_IE_CP_INPUT_OCTET_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INPUT_OCTET_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* max output octets */
    if (cpdmCPConfigMaxOutputOctetsGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP max output octets", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_OUTPUT_OCTET_TYPE,
                           CP_CLUSTER_IE_CP_OUTPUT_OCTET_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_OUTPUT_OCTET_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* max total octets */
    if (cpdmCPConfigMaxTotalOctetsGet(cpId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP max total octets", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_TOTAL_OCTET_TYPE,
                           CP_CLUSTER_IE_CP_TOTAL_OCTET_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_TOTAL_OCTET_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* foreground color */
    if (cpdmCPConfigForegroundColorGet(cpId, fColor) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s foreground color", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_FOREGROUND_COLOR_TYPE,
                           CP_CLUSTER_IE_FOREGROUND_COLOR_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_FOREGROUND_COLOR_SIZE;
    CP_PKT_STRING_SET(fColor, CP_CLUSTER_IE_FOREGROUND_COLOR_SIZE, pdu.bufPtr, pdu.offset);

    /* background color */
    if (cpdmCPConfigBackgroundColorGet(cpId, bColor) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s background color", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_BACKGROUND_COLOR_TYPE,
                           CP_CLUSTER_IE_BACKGROUND_COLOR_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_BACKGROUND_COLOR_SIZE;
    CP_PKT_STRING_SET(bColor, CP_CLUSTER_IE_BACKGROUND_COLOR_SIZE, pdu.bufPtr, pdu.offset);

    /* separator color */
    if (cpdmCPConfigSeparatorColorGet(cpId, sColor) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s separator color", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_SEPARATOR_COLOR_TYPE,
                           CP_CLUSTER_IE_SEPARATOR_COLOR_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SEPARATOR_COLOR_SIZE;
    CP_PKT_STRING_SET(sColor, CP_CLUSTER_IE_SEPARATOR_COLOR_SIZE, pdu.bufPtr, pdu.offset);

    /* languages */
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_NUM_LANGUAGES_TYPE,
                           CP_CLUSTER_IE_CP_NUM_LANGUAGES_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_NUM_LANGUAGES_SIZE;
    tmpPdu.bufPtr = pdu.bufPtr;
    tmpPdu.offset = pdu.offset;
    tmp8 = 0;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);
    numWebID = 0;
    webId = 0;
    while (cpdmCPConfigWebIdNextGet(cpId, webId, &webId) == L7_SUCCESS)
    {
      CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_WEB_ID_TYPE,
                             CP_CLUSTER_IE_CP_WEB_ID_SIZE, pdu.bufPtr, pdu.offset);
      message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_WEB_ID_SIZE;
      tmp8 = (L7_uchar8)webId;
      CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

      memset(code, 0, sizeof(code));
      if (cpdmCPConfigWebLangCodeGet(cpId, webId, code) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: %s language code", __FUNCTION__, failed_to_get);
        return L7_FAILURE;
      }
      CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_WEB_CODE_TYPE,
                             CP_CLUSTER_IE_CP_WEB_CODE_SIZE, pdu.bufPtr, pdu.offset);
      message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_WEB_CODE_SIZE;
      CP_PKT_STRING_SET(code, CP_CLUSTER_IE_CP_WEB_CODE_SIZE, pdu.bufPtr, pdu.offset);

      memset(link, 0, sizeof(link));
      if (cpdmCPConfigWebLocaleLinkGet(cpId, webId, link) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: %s locale link", __FUNCTION__, failed_to_get);
        return L7_FAILURE;
      }
      CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_WEB_LINK_TYPE,
                             CP_CLUSTER_IE_CP_WEB_LINK_SIZE, pdu.bufPtr, pdu.offset);
      message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_WEB_LINK_SIZE;
      CP_PKT_STRING_SET(link, CP_CLUSTER_IE_CP_WEB_LINK_SIZE, pdu.bufPtr, pdu.offset);

      numWebID++;
    }
    /* update number of web ids, previously set to zero */
    tmp8 = (L7_uchar8)numWebID;
    CP_PKT_INT8_SET(tmp8, tmpPdu.bufPtr, tmpPdu.offset);

    /* interfaces */
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_NUM_INTF_TYPE,
                           CP_CLUSTER_IE_CP_NUM_INTF_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_NUM_INTF_SIZE;
    tmpPdu.bufPtr = pdu.bufPtr;
    tmpPdu.offset = pdu.offset;
    numIntf = 0;
    CP_PKT_INT32_SET(numIntf, pdu.bufPtr, pdu.offset);
    intf = 0;
    while ((cpdmCPConfigIntIfNumNextGet(cpId, intf, &next_cpId, &intf) == L7_SUCCESS) &&
           (next_cpId == cpId))
    {
      CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_CP_INTF_ID_TYPE,
                             CP_CLUSTER_IE_CP_INTF_ID_SIZE, pdu.bufPtr, pdu.offset);
      message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_CP_INTF_ID_SIZE;
      CP_PKT_INT32_SET(intf, pdu.bufPtr, pdu.offset);
      numIntf++;
    }
    /* update number of interfaces, previously set to zero */
    CP_PKT_INT32_SET(numIntf, tmpPdu.bufPtr, tmpPdu.offset);

    CP_DLOG(CPD_LEVEL_LOG, "%s: Sending CP instance config message of size %d to switch %02x:%02x:%02x:%02x:%02x:%02x",
            __FUNCTION__,
            message->msgLen, swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
            swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);

    message->msgLen = osapiHtons(message->msgLen);

    if (cpClusterSwitchEntryGet(swMacAddr) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg not sent",
               __FUNCTION__,
              swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
              swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
      return L7_FAILURE;
    }

    if (clusterMsgSend(&swMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                       CP_CLUSTER_CP_CONFIG_SYNC_MSG, pdu.offset, buf) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send CP instance config message to switch %02x:%02x:%02x:%02x:%02x:%02x",
              __FUNCTION__,
              swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
              swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP user groups config message
*
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t cpConfigMsgUserGroupBuildSend(L7_enetMacAddr_t swMacAddr)
{
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  gpId_t                    gId = 0;
  L7_char8                  groupName[CP_USER_LOCAL_USERGROUP_MAX+1];
  L7_uchar8                 tmp8 = 0;

  gId = 0;
  while (cpdmUserGroupEntryNextGet(gId, &gId) == L7_SUCCESS)
  {
    memset(buf, 0, sizeof(buf));
    pdu.bufPtr = buf;
    pdu.offset = 0;

    message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
    message->msgType = osapiHtons(CP_CLUSTER_CP_CONFIG_SYNC_MSG);
    message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
    pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_CONFIG_MSG_IE_GROUP_TYPE,
                           CP_CLUSTER_CONFIG_MSG_IE_GROUP_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_CONFIG_MSG_IE_GROUP_SIZE;

    /* group id */
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_GROUP_ID_TYPE,
                           CP_CLUSTER_IE_GROUP_ID_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_GROUP_ID_SIZE;
    tmp8 = (L7_uchar8)gId;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

    /* group name */
    if (cpdmUserGroupEntryNameGet(gId, groupName) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s group name", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_GROUP_NAME_TYPE,
                           CP_CLUSTER_IE_GROUP_NAME_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_GROUP_NAME_SIZE;
    CP_PKT_STRING_SET(groupName, CP_CLUSTER_IE_GROUP_NAME_SIZE, pdu.bufPtr, pdu.offset);

    CP_DLOG(CPD_LEVEL_LOG, "%s: Sending CP user group config message of size %d to switch %02x:%02x:%02x:%02x:%02x:%02x", __FUNCTION__,
            message->msgLen, swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
            swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);

    message->msgLen = osapiHtons(message->msgLen);

    if (cpClusterSwitchEntryGet(swMacAddr) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg not sent", __FUNCTION__,
              swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
              swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
      return L7_FAILURE;
    }

    if (clusterMsgSend(&swMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                       CP_CLUSTER_CP_CONFIG_SYNC_MSG, pdu.offset, buf) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send CP user group config message to switch %02x:%02x:%02x:%02x:%02x:%02x",
              __FUNCTION__,
              swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
              swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP local users config message
*
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t cpConfigMsgLocalUserBuildSend(L7_enetMacAddr_t swMacAddr)
{
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu, tmpPdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_uint32                 val, numUserGroup = 0;
  uId_t                     uId = 0, next_uId;
  gpId_t                    gId = 0;
  L7_char8                  userName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8                  pwd[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_uchar8                 tmp8 = 0;



  uId = 0;
  while (cpdmUserEntryNextGet(uId, &uId) == L7_SUCCESS)
  {
    memset(buf, 0, sizeof(buf));
    pdu.bufPtr = buf;
    pdu.offset = 0;

    message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
    message->msgType = osapiHtons(CP_CLUSTER_CP_CONFIG_SYNC_MSG);
    message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
    pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_CONFIG_MSG_IE_USER_TYPE,
                           CP_CLUSTER_CONFIG_MSG_IE_USER_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_CONFIG_MSG_IE_USER_SIZE;

    /* user id */
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_ID_TYPE,
                           CP_CLUSTER_IE_USER_ID_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_ID_SIZE;
    tmp8 = (L7_uchar8)uId;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

    /* user name */
    if (cpdmUserEntryLoginNameGet(uId, userName) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user name.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_NAME_TYPE,
                           CP_CLUSTER_IE_USER_NAME_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_NAME_SIZE;
    CP_PKT_STRING_SET(userName, CP_CLUSTER_IE_USER_NAME_SIZE, pdu.bufPtr, pdu.offset);

    /* password */
    if (cpdmUserEntryEncryptedPasswordGet(uId, pwd) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user pwd.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_PWD_TYPE,
                           CP_CLUSTER_IE_USER_PWD_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_PWD_SIZE;
    CP_PKT_STRING_SET(pwd, CP_CLUSTER_IE_USER_PWD_SIZE, pdu.bufPtr, pdu.offset);

    /* session timeout */
    if (cpdmUserEntrySessionTimeoutGet(uId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user session timeout.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_SESSION_TIMEOUT_TYPE,
                           CP_CLUSTER_IE_USER_SESSION_TIMEOUT_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_SESSION_TIMEOUT_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* idle timeout */
    if (cpdmUserEntryIdleTimeoutGet(uId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user idle timeout.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_IDLE_TIMEOUT_TYPE,
                           CP_CLUSTER_IE_USER_IDLE_TIMEOUT_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_IDLE_TIMEOUT_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* bandwidth up */
    if (cpdmUserEntryMaxBandwidthUpGet(uId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user bandwidth up rate.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_BAND_UP_TYPE,
                           CP_CLUSTER_IE_USER_BAND_UP_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_BAND_UP_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* bandwidth down */
    if (cpdmUserEntryMaxBandwidthDownGet(uId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user bandwidth down rate.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_BAND_DOWN_TYPE,
                           CP_CLUSTER_IE_USER_BAND_DOWN_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_BAND_DOWN_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* input octets */
    if (cpdmUserEntryMaxInputOctetsGet(uId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user max input octets.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_INPUT_OCTET_TYPE,
                           CP_CLUSTER_IE_USER_INPUT_OCTET_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_INPUT_OCTET_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* output octets */
    if (cpdmUserEntryMaxOutputOctetsGet(uId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user max output octets.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_OUTPUT_OCTET_TYPE,
                           CP_CLUSTER_IE_USER_OUTPUT_OCTET_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_OUTPUT_OCTET_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* total octets */
    if (cpdmUserEntryMaxTotalOctetsGet(uId, &val) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s user max total octets.", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_TOTAL_OCTET_TYPE,
                           CP_CLUSTER_IE_USER_TOTAL_OCTET_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_TOTAL_OCTET_SIZE;
    CP_PKT_INT32_SET(val, pdu.bufPtr, pdu.offset);

    /* groups */
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_NUM_GROUP_TYPE,
                           CP_CLUSTER_IE_USER_NUM_GROUP_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_NUM_GROUP_SIZE;
    tmpPdu.bufPtr = pdu.bufPtr;
    tmpPdu.offset = pdu.offset;
    tmp8 = 0;
    CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);
    numUserGroup = 0;
    gId = 0;
    while ((cpdmUserGroupAssocEntryNextGet(uId, gId, &next_uId, &gId) == L7_SUCCESS) &&
           (next_uId == uId))
    {
      CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_USER_GROUP_ID_TYPE,
                             CP_CLUSTER_IE_USER_GROUP_ID_SIZE, pdu.bufPtr, pdu.offset);
      message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_USER_GROUP_ID_SIZE;
      tmp8 = (L7_uchar8)gId;
      CP_PKT_INT8_SET(tmp8, pdu.bufPtr, pdu.offset);

      numUserGroup++;
    }
    /* update number of groups, previously set to zero */
    tmp8 = (L7_uchar8)numUserGroup;
    CP_PKT_INT8_SET(tmp8, tmpPdu.bufPtr, tmpPdu.offset);

    CP_DLOG(CPD_LEVEL_LOG, "%s: Sending CP local user config message of size %d to switch %02x:%02x:%02x:%02x:%02x:%02x",
            __FUNCTION__,
            message->msgLen, swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
            swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);

    message->msgLen = osapiHtons(message->msgLen);

    if (cpClusterSwitchEntryGet(swMacAddr) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg not sent",
              __FUNCTION__,
              swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
              swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
      return L7_FAILURE;
    }

    if (clusterMsgSend(&swMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                       CP_CLUSTER_CP_CONFIG_SYNC_MSG, pdu.offset, buf) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send CP local user config message to switch %02x:%02x:%02x:%02x:%02x:%02x",
              __FUNCTION__,
              swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
              swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build CP resend message
*
* @param    L7_uchar8   type   @b{(input)} type
* @param    L7_uchar8   buf    @b{(input/output)} Pointer to store msg
* @param    L7_uint32   offset @b{(input/output)} Pointer to offset into buffer
* @param    L7_ushort16 len    @b{(input/output)} Pointer to msg length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t cpResendMsgBuild(L7_uchar8 type,
                                L7_uchar8 *buf, L7_uint32 *offset, L7_ushort16 *len)
{
  cpClusterPduHdl_t        pdu;
  L7_ushort16              pktLen = 0;
  cpClusterMsgElementHdr_t element;



  pdu.bufPtr = buf;
  pdu.offset = *offset;

  CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_RESEND_TYPE,
                         CP_CLUSTER_IE_RESEND_SIZE, pdu.bufPtr, pdu.offset);
  pktLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_RESEND_SIZE;
  CP_PKT_INT8_SET(type, pdu.bufPtr, pdu.offset);

  *len += pktLen;
  *offset = pdu.offset;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process CP global config message info
*
* @param    L7_uchar8 msgCfgSyncBuf @b{(input)} pointer to msg buffer
* @param    L7_int32  totalIELen    @b{(input)} msg length
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void cpConfigMsgGlobalProcess(L7_uchar8 *msgCfgSyncBuf, L7_int32 totalIELen)
{
  L7_ushort16  mType;
  L7_ushort16  mLen;
  L7_uint32    tmp32 = 0;
  L7_uchar8    tmp8 = 0;

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCfgSyncBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCfgSyncBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgCfgSyncBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCfgSyncBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP global config msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_CLUSTER_IE_CP_GLOBAL_MODE_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_GLOBAL_MODE_SIZE)
      {
        tmp8 = *msgCfgSyncBuf;
        ptrCpGlobalData->cpMode = (L7_uint32)tmp8;
      }
      break;
    case CP_CLUSTER_IE_HTTP_PORT_TYPE:
      if (mLen == CP_CLUSTER_IE_HTTP_PORT_SIZE)
      {
        memcpy(&tmp32, msgCfgSyncBuf, mLen);
        tmp32 = osapiNtohl(tmp32);
        ptrCpGlobalData->httpPort = tmp32;
      }
      break;
#ifdef L7_MGMT_SECURITY_PACKAGE
    case CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT1_TYPE:
      if (mLen == CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT1_SIZE)
      {
        memcpy(&tmp32, msgCfgSyncBuf, mLen);
        tmp32 = osapiNtohl(tmp32);
        ptrCpGlobalData->httpsPort1 = tmp32;
      }
      break;
    case CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT2_TYPE:
      if (mLen == CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT2_SIZE)
      {
        memcpy(&tmp32, msgCfgSyncBuf, mLen);
        tmp32 = osapiNtohl(tmp32);
        ptrCpGlobalData->httpsPort2 = tmp32;
      }
      break;
#endif
    case CP_CLUSTER_IE_PEER_STATS_REPORT_INTVL_TYPE:
      if (mLen == CP_CLUSTER_IE_PEER_STATS_REPORT_INTVL_SIZE)
      {
        memcpy(&tmp32, msgCfgSyncBuf, mLen);
        tmp32 = osapiNtohl(tmp32);
        ptrCpGlobalData->peerSwitchStatsReportInterval = tmp32;
      }
      break;
    case CP_CLUSTER_IE_AUTH_SESSION_TIMEOUT_TYPE:
      if (mLen == CP_CLUSTER_IE_AUTH_SESSION_TIMEOUT_SIZE)
      {
        memcpy(&tmp32, msgCfgSyncBuf, mLen);
        tmp32 = osapiNtohl(tmp32);
        ptrCpGlobalData->sessionTimeout = tmp32;
      }
      break;
    case CP_CLUSTER_IE_CLIENT_AUTH_FAILURE_TRAP_TYPE:
      if (mLen == CP_CLUSTER_IE_CLIENT_AUTH_FAILURE_TRAP_SIZE)
      {
        tmp8 = *msgCfgSyncBuf;
        if (tmp8 == L7_ENABLE)
        {
          ptrCpGlobalData->trapFlags |= CP_TRAP_AUTH_FAILURE;
        }
        else
        {
          ptrCpGlobalData->trapFlags &= ~CP_TRAP_AUTH_FAILURE;
        }
      }
      break;
    case CP_CLUSTER_IE_CLIENT_CONN_TRAP_TYPE:
      if (mLen == CP_CLUSTER_IE_CLIENT_CONN_TRAP_SIZE)
      {
        tmp8 = *msgCfgSyncBuf;
        if (tmp8 == L7_ENABLE)
        {
          ptrCpGlobalData->trapFlags |= CP_TRAP_CLIENT_CONNECTED;
        }
        else
        {
          ptrCpGlobalData->trapFlags &= ~CP_TRAP_CLIENT_CONNECTED;
        }
      }
      break;
    case CP_CLUSTER_IE_CLIENT_DB_FULL_TRAP_TYPE:
      if (mLen == CP_CLUSTER_IE_CLIENT_DB_FULL_TRAP_SIZE)
      {
        tmp8 = *msgCfgSyncBuf;
        if (tmp8 == L7_ENABLE)
        {
          ptrCpGlobalData->trapFlags |= CP_TRAP_CONNECTION_DB_FULL;
        }
        else
        {
          ptrCpGlobalData->trapFlags &= ~CP_TRAP_CONNECTION_DB_FULL;
        }
      }
      break;
    case CP_CLUSTER_IE_CLIENT_DISCONN_TRAP_TYPE:
      if (mLen == CP_CLUSTER_IE_CLIENT_DISCONN_TRAP_SIZE)
      {
        tmp8 = *msgCfgSyncBuf;
        if (tmp8 == L7_ENABLE)
        {
          ptrCpGlobalData->trapFlags |= CP_TRAP_CLIENT_DISCONNECTED;
        }
        else
        {
          ptrCpGlobalData->trapFlags &= ~CP_TRAP_CLIENT_DISCONNECTED;
        }
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n", __FUNCTION__, mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgCfgSyncBuf += mLen;
    }
  }

  return;
}

/*********************************************************************
* @purpose  Process CP config message info
*
* @param    L7_uchar8 msgCfgSyncBuf @b{(input)} pointer to msg buffer
* @param    L7_int32  totalIELen    @b{(input)} msg length
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void cpConfigMsgCPProcess(L7_uchar8 *msgCfgSyncBuf, L7_int32 totalIELen)
{
  L7_ushort16  mType;
  L7_ushort16  mLen;
  L7_uint32    i, j, k=0, cpNumIntf=0, tmp32=0;
  L7_char8     cpName[CP_NAME_MAX+1];
  L7_char8     url[CP_WELCOME_URL_MAX+1];
  L7_char8     radius[CP_RADIUS_AUTH_SERVER_MAX+1];
  L7_uchar8    cpId=0, mode=0, prot=0, verify=0, rMode=0, cpNumWebid=0;
  L7_uchar8    wId[FD_CP_CUSTOM_LOCALE_MAX], gId=0;
  L7_char8     fColor[CP_FOREGROUND_COLOR_MAX+1];
  L7_char8     bColor[CP_BACKGROUND_COLOR_MAX+1];
  L7_char8     sColor[CP_SEPARATOR_COLOR_MAX+1];
  L7_char8     code[FD_CP_CUSTOM_LOCALE_MAX][CP_LANG_CODE_MAX+1];
  L7_char8     link[FD_CP_CUSTOM_LOCALE_MAX][CP_LOCALE_LINK_MAX+1];
  L7_uint32    sTime=0, iTime=0;
  L7_uint32    bUp=0, bDown=0, inOctet=0, outOctet=0, totalOctet=0;
  L7_uint32    intf[CP_INTERFACE_MAX];



  for (i=0; i<FD_CP_CUSTOM_LOCALE_MAX; i++)
  {
    wId[i] = 0;
    memset(code[i], 0, sizeof(code[i]));
    memset(link[i], 0, sizeof(link[i]));
  }

  for (i=0; i<CP_INTERFACE_MAX; i++)
  {
    intf[i] = 0;
  }

  i = j = 0;
  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCfgSyncBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCfgSyncBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgCfgSyncBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCfgSyncBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP config msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_CLUSTER_IE_CP_ID_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_ID_SIZE)
      {
        cpId = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_CP_NAME_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_NAME_SIZE)
      {
        osapiStrncpy(cpName, msgCfgSyncBuf, mLen);
        cpName[mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_CP_MODE_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_MODE_SIZE)
      {
        mode = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_CP_PROTOCOL_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_PROTOCOL_SIZE)
      {
        prot = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_CP_VERIFY_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_VERIFY_SIZE)
      {
        verify = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_CP_REDIRECT_MODE_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_REDIRECT_MODE_SIZE)
      {
        rMode = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_CP_REDIRECT_URL_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_REDIRECT_URL_SIZE)
      {
        osapiStrncpy(url, msgCfgSyncBuf, mLen);
        url[mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_RADIUS_AUTH_SERVER_TYPE:
      if (mLen == CP_CLUSTER_IE_RADIUS_AUTH_SERVER_SIZE)
      {
        osapiStrncpy(radius, msgCfgSyncBuf, mLen);
        radius[mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_GROUP_ID_TYPE:
      if (mLen == CP_CLUSTER_IE_GROUP_ID_SIZE)
      {
        gId = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_CP_SESSION_TIMEOUT_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_SESSION_TIMEOUT_SIZE)
      {
        memcpy(&sTime, msgCfgSyncBuf, mLen);
        sTime = osapiNtohl(sTime);
      }
      break;
    case CP_CLUSTER_IE_CP_IDLE_TIMEOUT_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_IDLE_TIMEOUT_SIZE)
      {
        memcpy(&iTime, msgCfgSyncBuf, mLen);
        iTime = osapiNtohl(iTime);
      }
      break;
    case CP_CLUSTER_IE_CP_BAND_UP_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_BAND_UP_SIZE)
      {
        memcpy(&bUp, msgCfgSyncBuf, mLen);
        bUp = osapiNtohl(bUp);
      }
      break;
    case CP_CLUSTER_IE_CP_BAND_DOWN_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_BAND_DOWN_SIZE)
      {
        memcpy(&bDown, msgCfgSyncBuf, mLen);
        bDown = osapiNtohl(bDown);
      }
      break;
    case CP_CLUSTER_IE_CP_INPUT_OCTET_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_INPUT_OCTET_SIZE)
      {
        memcpy(&inOctet, msgCfgSyncBuf, mLen);
        inOctet = osapiNtohl(inOctet);
      }
      break;
    case CP_CLUSTER_IE_CP_OUTPUT_OCTET_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_OUTPUT_OCTET_SIZE)
      {
        memcpy(&outOctet, msgCfgSyncBuf, mLen);
        outOctet = osapiNtohl(outOctet);
      }
      break;
    case CP_CLUSTER_IE_CP_TOTAL_OCTET_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_TOTAL_OCTET_SIZE)
      {
        memcpy(&totalOctet, msgCfgSyncBuf, mLen);
        totalOctet = osapiNtohl(totalOctet);
      }
      break;
    case CP_CLUSTER_IE_FOREGROUND_COLOR_TYPE:
      if (mLen == CP_CLUSTER_IE_FOREGROUND_COLOR_SIZE)
      {
        osapiStrncpy(fColor, msgCfgSyncBuf, mLen);
        fColor[mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_BACKGROUND_COLOR_TYPE:
      if (mLen == CP_CLUSTER_IE_BACKGROUND_COLOR_SIZE)
      {
        osapiStrncpy(bColor, msgCfgSyncBuf, mLen);
        bColor[mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_SEPARATOR_COLOR_TYPE:
      if (mLen == CP_CLUSTER_IE_SEPARATOR_COLOR_SIZE)
      {
        osapiStrncpy(sColor, msgCfgSyncBuf, mLen);
        sColor[mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_CP_NUM_LANGUAGES_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_NUM_LANGUAGES_SIZE)
      {
        cpNumWebid = *msgCfgSyncBuf;
        if (cpNumWebid > FD_CP_CUSTOM_LOCALE_MAX)
        {
          cpClusterMsgError = L7_TRUE;
          return;
        }
      }
      break;
    case CP_CLUSTER_IE_CP_WEB_ID_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_WEB_ID_SIZE)
      {
        wId[i] = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_CP_WEB_CODE_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_WEB_CODE_SIZE)
      {
        osapiStrncpy(code[i], msgCfgSyncBuf, mLen);
        code[i][mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_CP_WEB_LINK_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_WEB_LINK_SIZE)
      {
        osapiStrncpy(link[i], msgCfgSyncBuf, mLen);
        link[i][mLen] = '\0';
        i++;
      }
      break;
    case CP_CLUSTER_IE_CP_NUM_INTF_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_NUM_INTF_SIZE)
      {
        memcpy(&cpNumIntf, msgCfgSyncBuf, mLen);
        cpNumIntf = osapiNtohl(cpNumIntf);
        if (cpNumIntf > CP_INTERFACE_MAX)
        {
          cpClusterMsgError = L7_TRUE;
          return;
        }
      }
      break;
    case CP_CLUSTER_IE_CP_INTF_ID_TYPE:
      if (mLen == CP_CLUSTER_IE_CP_INTF_ID_SIZE)
      {
        memcpy(&tmp32, msgCfgSyncBuf, mLen);
        tmp32 = osapiNtohl(tmp32);
        intf[j] = tmp32;
        j++;
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n", __FUNCTION__, mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgCfgSyncBuf += mLen;
    }
    else
    {
      for (i=0; i<FD_CP_CONFIG_MAX; i++)
      {
        if (ptrCpConfigData[i].cpId == 0)
        {
          ptrCpConfigData[i].cpId = (cpId_t)cpId;

          osapiStrncpy(ptrCpConfigData[i].name, cpName, CP_NAME_MAX);
          ptrCpConfigData[i].name[CP_NAME_MAX] = '\0';

          ptrCpConfigData[i].enabled = (L7_CP_MODE_STATUS_t)mode;
          ptrCpConfigData[i].protocolMode = (L7_LOGIN_TYPE_t)prot;
          ptrCpConfigData[i].verifyMode = (CP_VERIFY_MODE_t)verify;
          ptrCpConfigData[i].redirectMode = rMode;

          osapiStrncpy(ptrCpConfigData[i].redirectURL, url, CP_WELCOME_URL_MAX);
          ptrCpConfigData[i].redirectURL[CP_WELCOME_URL_MAX] = '\0';

          osapiStrncpy(ptrCpConfigData[i].radiusAuthServer, radius, CP_RADIUS_AUTH_SERVER_MAX);
          ptrCpConfigData[i].radiusAuthServer[CP_RADIUS_AUTH_SERVER_MAX] = '\0';

          ptrCpConfigData[i].gpId = (gpId_t)gId;
          ptrCpConfigData[i].sessionTimeout = sTime;
          ptrCpConfigData[i].idleTimeout = iTime;
          ptrCpConfigData[i].userUpRate = bUp;
          ptrCpConfigData[i].userDownRate = bDown;
          ptrCpConfigData[i].maxInputOctets = inOctet;
          ptrCpConfigData[i].maxOutputOctets = outOctet;
          ptrCpConfigData[i].maxTotalOctets = totalOctet;
          ptrCpConfigData[i].localeCount = (L7_ushort16)cpNumWebid;

          osapiStrncpy(ptrCpConfigData[i].foregroundColor, fColor, CP_FOREGROUND_COLOR_MAX);
          ptrCpConfigData[i].foregroundColor[CP_FOREGROUND_COLOR_MAX] = '\0';
          osapiStrncpy(ptrCpConfigData[i].backgroundColor, bColor, CP_BACKGROUND_COLOR_MAX);
          ptrCpConfigData[i].backgroundColor[CP_BACKGROUND_COLOR_MAX] = '\0';
          osapiStrncpy(ptrCpConfigData[i].separatorColor, sColor, CP_SEPARATOR_COLOR_MAX);
          ptrCpConfigData[i].separatorColor[CP_SEPARATOR_COLOR_MAX] = '\0';

          k = 0;
          for (j=webIndex; j<(FD_CP_CONFIG_MAX*FD_CP_CUSTOM_LOCALE_MAX); j++)
          {
            if (k == cpNumWebid)
            {
              break;
            }
            if (ptrCpWebID[j].cpId == 0)
            {
              ptrCpWebID[j].cpId = (cpId_t)cpId;
              ptrCpWebID[j].webId = (webId_t)wId[k];
            }

            osapiStrncpy(ptrCpCodeLink[j].code, code[k], CP_LANG_CODE_MAX);
            ptrCpCodeLink[j].code[CP_LANG_CODE_MAX] = '\0';

            osapiStrncpy(ptrCpCodeLink[j].link, link[k], CP_LOCALE_LINK_MAX);
            ptrCpCodeLink[j].link[CP_LOCALE_LINK_MAX] = '\0';

            k++;
          }
          webIndex = j;

          k = 0;
          for (j=ifIndex; j<(FD_CP_CONFIG_MAX*CP_INTERFACE_MAX); j++)
          {
            if (k == cpNumIntf)
            {
              break;
            }
            if (ptrCpInterfaceAssocData[j].cpId == 0)
            {
              ptrCpInterfaceAssocData[j].cpId = (cpId_t)cpId;
              ptrCpInterfaceAssocData[j].intIfNum = intf[k];
            }
            k++;
          }
          ifIndex = j;
          numCP++;
          return;
        }
      }

      if (i == FD_CP_CONFIG_MAX)
      {
        cpClusterMsgError = L7_TRUE;
        return;
      }
    }
  }
}

/*********************************************************************
* @purpose  Process CP group config message info
*
* @param    L7_uchar8 msgCfgSyncBuf @b{(input)} pointer to msg buffer
* @param    L7_int32  totalIELen    @b{(input)} msg length
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void cpConfigMsgGroupProcess(L7_uchar8 *msgCfgSyncBuf, L7_int32 totalIELen)
{
  L7_ushort16  mType;
  L7_ushort16  mLen;
  L7_uint32    i;
  L7_uchar8    gId=0;
  L7_char8     groupName[CP_USER_LOCAL_USERGROUP_MAX+1];



  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCfgSyncBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCfgSyncBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgCfgSyncBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCfgSyncBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP group config msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_CLUSTER_IE_GROUP_ID_TYPE:
      if (mLen == CP_CLUSTER_IE_GROUP_ID_SIZE)
      {
        gId = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_GROUP_NAME_TYPE:
      if (mLen == CP_CLUSTER_IE_GROUP_NAME_SIZE)
      {
        osapiStrncpy(groupName, msgCfgSyncBuf, mLen);
        groupName[mLen] = '\0';
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n", __FUNCTION__, mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgCfgSyncBuf += mLen;
    }
    else
    {
      for (i=0; i<FD_CP_USER_GROUP_MAX; i++)
      {
        if (ptrCpUserGroupData[i].gpId == 0)
        {
          ptrCpUserGroupData[i].gpId = (gpId_t)gId;

          osapiStrncpy(ptrCpUserGroupData[i].groupName, groupName, CP_USER_LOCAL_USERGROUP_MAX);
          ptrCpUserGroupData[i].groupName[CP_USER_LOCAL_USERGROUP_MAX] = '\0';

          numGroup++;
          return;
        }
      }

      if (i == FD_CP_USER_GROUP_MAX)
      {
        cpClusterMsgError = L7_TRUE;
        return;
      }
    }
  }
}

/*********************************************************************
* @purpose  Process CP user config message info
*
* @param    L7_uchar8 msgCfgSyncBuf @b{(input)} pointer to msg buffer
* @param    L7_int32  totalIELen    @b{(input)} msg length
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void cpConfigMsgUserProcess(L7_uchar8 *msgCfgSyncBuf, L7_int32 totalIELen)
{
  L7_ushort16  mType;
  L7_ushort16  mLen;
  L7_uint32    i, j, k;
  L7_char8     userName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8     pwd[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_uchar8    uId=0, userNumGroup=0;
  L7_uchar8    gId[FD_CP_USER_GROUP_MAX];
  L7_uint32    sTime=0, iTime=0;
  L7_uint32    bUp=0, bDown=0, inOctet=0, outOctet=0, totalOctet=0;

  for (i=0; i<FD_CP_USER_GROUP_MAX; i++)
  {
    gId[i] = 0;
  }

  i = 0;
  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCfgSyncBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCfgSyncBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgCfgSyncBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCfgSyncBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP user config msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_CLUSTER_IE_USER_ID_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_ID_SIZE)
      {
        uId = *msgCfgSyncBuf;
      }
      break;
    case CP_CLUSTER_IE_USER_NAME_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_NAME_SIZE)
      {
        osapiStrncpy(userName, msgCfgSyncBuf, mLen);
        userName[mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_USER_PWD_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_PWD_SIZE)
      {
        osapiStrncpy(pwd, msgCfgSyncBuf, mLen);
        pwd[mLen] = '\0';
      }
      break;
    case CP_CLUSTER_IE_USER_SESSION_TIMEOUT_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_SESSION_TIMEOUT_SIZE)
      {
        memcpy(&sTime, msgCfgSyncBuf, mLen);
        sTime = osapiNtohl(sTime);
      }
      break;
    case CP_CLUSTER_IE_USER_IDLE_TIMEOUT_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_IDLE_TIMEOUT_SIZE)
      {
        memcpy(&iTime, msgCfgSyncBuf, mLen);
        iTime = osapiNtohl(iTime);
      }
      break;
    case CP_CLUSTER_IE_USER_BAND_UP_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_BAND_UP_SIZE)
      {
        memcpy(&bUp, msgCfgSyncBuf, mLen);
        bUp = osapiNtohl(bUp);
      }
      break;
    case CP_CLUSTER_IE_USER_BAND_DOWN_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_BAND_DOWN_SIZE)
      {
        memcpy(&bDown, msgCfgSyncBuf, mLen);
        bDown = osapiNtohl(bDown);
      }
      break;
    case CP_CLUSTER_IE_USER_INPUT_OCTET_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_INPUT_OCTET_SIZE)
      {
        memcpy(&inOctet, msgCfgSyncBuf, mLen);
        inOctet = osapiNtohl(inOctet);
      }
      break;
    case CP_CLUSTER_IE_USER_OUTPUT_OCTET_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_OUTPUT_OCTET_SIZE)
      {
        memcpy(&outOctet, msgCfgSyncBuf, mLen);
        outOctet = osapiNtohl(outOctet);
      }
      break;
    case CP_CLUSTER_IE_USER_TOTAL_OCTET_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_TOTAL_OCTET_SIZE)
      {
        memcpy(&totalOctet, msgCfgSyncBuf, mLen);
        totalOctet = osapiNtohl(totalOctet);
      }
      break;
    case CP_CLUSTER_IE_USER_NUM_GROUP_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_NUM_GROUP_SIZE)
      {
        userNumGroup = *msgCfgSyncBuf;
        if (userNumGroup > FD_CP_USER_GROUP_MAX)
        {
          cpClusterMsgError = L7_TRUE;
          return;
        }
      }
      break;
    case CP_CLUSTER_IE_USER_GROUP_ID_TYPE:
      if (mLen == CP_CLUSTER_IE_USER_GROUP_ID_SIZE)
      {
        gId[i] = *msgCfgSyncBuf;
        i++;
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n", __FUNCTION__, mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgCfgSyncBuf += mLen;
    }
    else
    {
      for (i=0; i<FD_CP_LOCAL_USERS_MAX; i++)
      {
        if (ptrCpUsers[i].uId == 0)
        {
          ptrCpUsers[i].uId = (uId_t)uId;

          osapiStrncpy(ptrCpUsers[i].loginName, userName, CP_USER_LOCAL_USERNAME_MAX);
          ptrCpUsers[i].loginName[CP_USER_LOCAL_USERNAME_MAX] = '\0';

          osapiStrncpy(ptrCpUsers[i].password, pwd, L7_ENCRYPTED_PASSWORD_SIZE-1);
          ptrCpUsers[i].password[L7_ENCRYPTED_PASSWORD_SIZE-1] = '\0';

          ptrCpUsers[i].sessionTimeout = sTime;
          ptrCpUsers[i].idleTimeout = iTime;
          ptrCpUsers[i].maxBandwidthUp = bUp;
          ptrCpUsers[i].maxBandwidthDown = bDown;
          ptrCpUsers[i].maxInputOctets = inOctet;
          ptrCpUsers[i].maxOutputOctets = outOctet;
          ptrCpUsers[i].maxTotalOctets = totalOctet;

          k = 0;
          for (j=ugIndex; j<FD_CP_USER_GROUP_ASSOC_MAX; j++)
          {
            if (k == userNumGroup)
            {
              break;
            }
            if (ptrCpUserGroupAssocData[j].uId == 0)
            {
              ptrCpUserGroupAssocData[j].uId = (uId_t)uId;
              ptrCpUserGroupAssocData[j].gpId = (gpId_t)gId[k];
            }
            k++;
          }
          ugIndex = j;
          numUser++;
          return;
        }
      }

      if (i == FD_CP_LOCAL_USERS_MAX)
      {
        cpClusterMsgError = L7_TRUE;
        return;
      }
    }
  }
}

/*********************************************************************
* @purpose  Update CP web locale config
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void cpdmWebLocaleUpdate(void)
{
  cpId_t    cpId = 0;
  webId_t   webId;
  L7_char8  code[CP_LANG_CODE_MAX+1];

  while (cpdmCPConfigNextGet(cpId, &cpId) == L7_SUCCESS)
  {
    webId = 0;
    while (cpdmCPConfigWebIdNextGet(cpId, webId, &webId) == L7_SUCCESS)
    {
      memset(code, 0, sizeof(code));
      if (cpdmCPConfigWebLangCodeGet(cpId, webId, code) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: %s language code.", __FUNCTION__, failed_to_get);
      }
      if (cpdmCPConfigWebAddDefaults(cpId, webId, code, CP_ALL) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to add locale defaults.", __FUNCTION__);
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Update CP config
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void cpConfigApplyUpdate(void)
{
  cpdmCPConfigCountUpdate();
  cpdmActStatusUpdate();
  cpdmCPIntfCountUpdate();
  cpdmUserEntryCountUpdate();
  cpdmWebIdCountUpdate();
  cpdmWebLocaleUpdate();
  cpdmUserGroupEntryCountUpdate();
  cpdmUserGroupAssocEntryCountUpdate();
}

/*********************************************************************
* @purpose  Check validity of MAC address
*
* @param    L7_uchar8 macAddr @b{(input)} pointer to MAC address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL cpClusterValidMacAddr(L7_uchar8 *macAddr)
{
  L7_uint32 i;

  for (i=0; i<L7_ENET_MAC_ADDR_LEN; i++)
  {
    if (macAddr[i] != 0)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Cluster callback to process cluster events
*
* @param    clusterEvent       eventType @b{(input)} clusterEvent events
* @param    clusterMemberID_t *memberID  @b{(input)} pointer to switch MAC
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterMemberEventCallback(clusterEvent eventType, clusterMemberID_t *memberID)
{
  L7_enetMacAddr_t macAddr;
  L7_enetMacAddr_t prevCtrlMac;
  L7_uint32        i;
  CP_CLUSTER_FLAG_t flag;
  CP_CLUSTER_RESEND_FLAG_t resendFlag;

  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr, memberID, sizeof(L7_enetMacAddr_t));
  memset(&prevCtrlMac, 0x00, sizeof(L7_enetMacAddr_t));



  switch (eventType)
  {
  case CLUSTER_EVENT_LOCAL_SWITCH_JOINED:
    /* local switch MAC address may not be same each time, it depends on interface used for cluster */
    if (memcmp(&cpdmOprData->cpLocalSw.macAddr, &macAddr, sizeof(L7_enetMacAddr_t)) != 0)
    {
      memcpy(&cpdmOprData->cpLocalSw.macAddr, &macAddr, sizeof(L7_enetMacAddr_t));
    }
    cpdmOprData->cpLocalSw.status = CP_CLUSTER_SWITCH_MEMBER;
    for (i=0; i<CP_CLUSTER_MEMBERS; i++)
    {
      memset(&resendPeer[i], 0x00, sizeof(L7_enetMacAddr_t));
    }
    break;

  case CLUSTER_EVENT_LOCAL_SWITCH_LEFT:
    if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
    {
      if (cpdmClientConnStatusPeerPurge() != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to purge peer switch clients from connected clients table.", __FUNCTION__);
      }
    }

    memset(&cpdmOprData->cpLocalSw.macAddr, 0x00, sizeof(L7_enetMacAddr_t));
    cpdmOprData->cpLocalSw.status = CP_CLUSTER_SWITCH_NONE;
    cpdmOprData->cpLocalSw.cpId = 0;
    cpdmOprData->cpLocalSw.intf = 0;
    memset(&cpdmOprData->cpLocalSw.authClient, 0x00, sizeof(L7_enetMacAddr_t));
    memset(&cpdmOprData->cpLocalSw.peer, 0x00, sizeof(L7_enetMacAddr_t));
    cpdmOprData->cpLocalSw.authReqTime = 0;
    cpdmOprData->cpLocalSw.authReplyTime = 0;
    cpdmOprData->cpLocalSw.userInitiated = L7_FALSE;

    if (cpClusterSwitchClientEntriesPurge() != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to purge peer switch client table.", __FUNCTION__);
    }

    if (cpClusterSwitchEntriesPurge() != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to purge peer switch table.", __FUNCTION__);
    }

    memset(&cpdmOprData->cpNewPeer, 0x00, sizeof(L7_enetMacAddr_t));
    memset(&cpdmOprData->cpCtrlMacAddr, 0x00, sizeof(L7_enetMacAddr_t));

    memset(&cpdmOprData->cpClientAuthInfo.swMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memset(&cpdmOprData->cpClientAuthInfo.macAddr, 0x00, sizeof(L7_enetMacAddr_t));
    cpdmOprData->cpClientAuthInfo.cpId = 0;
    cpdmOprData->cpClientAuthInfo.intf = 0;
    cpdmOprData->cpClientAuthInfo.sessionTime = 0;
    memset(&cpdmOprData->cpClientAuthInfo.uName, 0, CP_USER_LOCAL_USERNAME_MAX);
    memset(&cpdmOprData->cpClientAuthInfo.pwd, 0, CP_USER_LOCAL_PASSWORD_MAX);
    cpdmOprData->cpClientAuthInfo.userUpRate = 0;
    cpdmOprData->cpClientAuthInfo.userDownRate = 0;
    cpdmOprData->cpClientAuthInfo.bytesTx = 0;
    cpdmOprData->cpClientAuthInfo.bytesRx = 0;
    cpdmOprData->cpClientAuthInfo.pktsTx = 0;
    cpdmOprData->cpClientAuthInfo.pktsRx = 0;
    cpdmOprData->cpClientAuthInfo.authPending = L7_FALSE;

    memset(&cpdmOprData->cpCfgPeerTx, 0x00, sizeof(L7_enetMacAddr_t));

    flag = CP_CLUSTER_PEER_JOINED | CP_CLUSTER_CONTROLLER_ELECTED |
           CP_CLUSTER_INST_STATUS_SEND | CP_CLUSTER_INST_INTF_STATUS_SEND |
           CP_CLUSTER_DEAUTH_INST_CLIENTS_SEND | CP_CLUSTER_DEAUTH_ALL_CLIENTS_SEND |
           CP_CLUSTER_INST_BLOCK_STATUS_SEND | CP_CLUSTER_CLIENT_AUTH_REPLY_SEND |
           CP_CLUSTER_CONFIG_SEND | CP_CLUSTER_CONFIG_RX;
    (void)cpClusterFlagClear(flag);

    resendFlag = CP_CLUSTER_RESEND_REQ_CLIENT_NOTIFY | CP_CLUSTER_RESEND_REQ_CONN_CLIENTS |
                 CP_CLUSTER_RESEND_REQ_INST_INTF | CP_CLUSTER_RESEND_CLIENT_NOTIFY |
                 CP_CLUSTER_RESEND_CONN_CLIENTS | CP_CLUSTER_RESEND_INST_INTF;
    (void)cpClusterResendFlagClear(resendFlag);
    break;

  case CLUSTER_EVENT_SWITCH_JOINED:
    memcpy(&cpdmOprData->cpNewPeer, &macAddr, sizeof(L7_enetMacAddr_t));

    /* initially, until elected controller */
    if (cpClusterSwitchEntryAddUpdate(macAddr, CP_CLUSTER_SWITCH_MEMBER) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to add peer switch %02x:%02x:%02x:%02x:%02x:%02x.\n",
              __FUNCTION__,
              macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
      return;
    }

    if (cpClusterFlagSet(CP_CLUSTER_PEER_JOINED) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set peer joined flag.", __FUNCTION__);
      memset(&cpdmOprData->cpNewPeer, 0, sizeof(L7_enetMacAddr_t));
    }
    break;

  case CLUSTER_EVENT_SWITCH_LEFT:
    if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
    {
      if (cpdmClientConnStatusPeerAllDelete(macAddr) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to delete peer switch %02x:%02x:%02x:%02x:%02x:%02x clients from CC database.\n",
                __FUNCTION__,
                macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
        return;
      }
    }

    if (cpClusterSwitchClientPeerClientsDelete(macAddr) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to delete peer switch %02x:%02x:%02x:%02x:%02x:%02x clients.\n",
              __FUNCTION__,
              macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
      return;
    }

    if (cpClusterSwitchEntryDelete(macAddr) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to delete peer switch %02x:%02x:%02x:%02x:%02x:%02x.\n",
              __FUNCTION__,
              macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
      return;
    }
    break;

  case CLUSTER_EVENT_CONTROLLER_ELECTED:
    memcpy(&prevCtrlMac, &cpdmOprData->cpCtrlMacAddr, sizeof(L7_enetMacAddr_t));
    memcpy(&cpdmOprData->cpCtrlMacAddr, &macAddr, sizeof(L7_enetMacAddr_t));

    if (memcmp(&cpdmOprData->cpLocalSw.macAddr, &macAddr, sizeof(L7_enetMacAddr_t)) == 0)
    {
      cpdmOprData->cpLocalSw.status = CP_CLUSTER_SWITCH_CONTROLLER;
      if (cpClusterValidMacAddr(prevCtrlMac.addr) == L7_TRUE)
      {
        if (cpClusterSwitchEntryAddUpdate(prevCtrlMac, CP_CLUSTER_SWITCH_MEMBER) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update peer switch %02x:%02x:%02x:%02x:%02x:%02x status.\n",
                  __FUNCTION__,
                  prevCtrlMac.addr[0],prevCtrlMac.addr[1],prevCtrlMac.addr[2],
                  prevCtrlMac.addr[3],prevCtrlMac.addr[4],prevCtrlMac.addr[5]);
        }
      }
    }
    else
    {
      cpdmOprData->cpLocalSw.status = CP_CLUSTER_SWITCH_MEMBER;

      if (memcmp(&cpdmOprData->cpLocalSw.macAddr, &prevCtrlMac, sizeof(L7_enetMacAddr_t)) == 0)
      {
        if (cpdmClientConnStatusPeerPurge() != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to purge peer switch clients from connected clients table.", __FUNCTION__);
        }
      }

      if (cpClusterSwitchEntryAddUpdate(macAddr, CP_CLUSTER_SWITCH_CONTROLLER) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update peer switch %02x:%02x:%02x:%02x:%02x:%02x status.\n",
                __FUNCTION__,
                macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],
                macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
      }

      if (cpClusterFlagSet(CP_CLUSTER_CONTROLLER_ELECTED) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set CP cluster controller elected flag.", __FUNCTION__);
        memset(&cpdmOprData->cpCtrlMacAddr, 0, sizeof(L7_enetMacAddr_t));
      }
    }
    break;

  default:
    break;
  }

}

/*********************************************************************
* @purpose  Send CP client notification message to a peer switch
*
* @param    L7_enetMacAddr_t peerMacAddr @b{(input)} peer MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends client notification message for all
*           switch authenticated clients to new peer switch when it joins.
*
* @end
*********************************************************************/

L7_RC_t cpClusterClientNotificationMsgPeerSend(L7_enetMacAddr_t peerMac)
{
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_ushort16               totalClients = 0;
  L7_ushort16               clientCount = 0;
  L7_enetMacAddr_t          macAddr;
  L7_enetMacAddr_t          switchMacAddr;



  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if ((cpdmClientConnStatusTotalGet(&totalClients) == L7_SUCCESS) && (totalClients == 0))
  {
    return L7_SUCCESS;
  }

  while (totalClients > 0)
  {
    memset(buf, 0, sizeof(buf));
    clientCount = 0;

    pdu.bufPtr = buf;
    pdu.offset = 0;
    message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
    message->msgType = osapiHtons(CP_CLUSTER_CP_CLIENT_NOTIFICATION_MSG);
    message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
    pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

    memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
    while (cpdmClientConnStatusNextGet(&macAddr, &macAddr) == L7_SUCCESS)
    {
      if (clientCount == 0)
      {
        memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
        if (cpdmClientConnStatusSwitchMacAddrGet(&macAddr, &switchMacAddr) == L7_SUCCESS)
        {
          CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_TYPE,
                                 CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
          message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
          CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);
        }
        else
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: %s client %02x:%02x:%02x:%02x:%02x:%02x switch MAC address",
                  __FUNCTION__, failed_to_get,
                  macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],
                  macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
          return L7_FAILURE;
        }
      }

      if (cpClientNotificationMsgBuild(macAddr, L7_TRUE, pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to build client %02x:%02x:%02x:%02x:%02x:%02x notification msg.\n",
                __FUNCTION__,
                macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],
                macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
        return L7_FAILURE;
      }

      clientCount++;
      if (clientCount >= CP_NUM_CLIENT_PER_TCP_MSG_MAX)
      {
        break;
      }
    }

    if (clientCount == 0)
    {
      return L7_SUCCESS;
    }
    else
    {
      message->msgLen = osapiHtons(message->msgLen);
      totalClients -= clientCount;
    }

    if (clusterMsgSend(&peerMac, CLUSTER_MSG_DELIVERY_RELIABLE,
                       CP_CLUSTER_CP_CLIENT_NOTIFICATION_MSG, pdu.offset, buf) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send client notification msg to switch %02x:%02x:%02x:%02x:%02x:%02x",
              __FUNCTION__,
              peerMac.addr[0],peerMac.addr[1],peerMac.addr[2],
              peerMac.addr[3],peerMac.addr[4],peerMac.addr[5]);
      return L7_FAILURE;
    }
  }

  CP_DLOG(CPD_LEVEL_LOG, "%s: CP client notification msg sent to switch %02x:%02x:%02x:%02x:%02x:%02x",
          __FUNCTION__,
          peerMac.addr[0],peerMac.addr[1],peerMac.addr[2],
          peerMac.addr[3],peerMac.addr[4],peerMac.addr[5]);
  return L7_SUCCESS;
}




/*********************************************************************
* @purpose  Send CP client notification message to all peer switches
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_BOOL          authStatus    @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends client notification message to all
*           peer switches in cluster after client is authenticated.
*
* @end
*********************************************************************/
L7_RC_t cpClusterClientNotificationMsgSend(L7_enetMacAddr_t clientMacAddr, L7_BOOL authStatus)
{
  cpdmPeerSwStatus_t       *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t          macAddr;
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_enetMacAddr_t          tmpMacAddr;
  L7_enetMacAddr_t          switchMacAddr;


  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memset(&tmpMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memset(buf, 0, sizeof(buf));
  pdu.bufPtr = buf;
  pdu.offset = 0;

  message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
  message->msgType = osapiHtons(CP_CLUSTER_CP_CLIENT_NOTIFICATION_MSG);
  message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
  pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

  memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));
  if (memcmp(&switchMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
  {
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_TYPE,
                           CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
    CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address.", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }

  if (cpClientNotificationMsgBuild(clientMacAddr, authStatus,
                                   pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to build client %02x:%02x:%02x:%02x:%02x:%02x notification msg",
              __FUNCTION__,
            clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
            clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
    return L7_FAILURE;
  }

  message->msgLen = osapiHtons(message->msgLen);

  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  {
    L7_BOOL gotAddr = L7_FALSE;
    SEMA_TAKE(READ);
    ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_NEXT);
    gotAddr = (ptrData != L7_NULLPTR) ? L7_TRUE : L7_FALSE;
    if (L7_TRUE == gotAddr)
    {
      memcpy(&macAddr, &(ptrData->macAddr), sizeof(L7_enetMacAddr_t));
    }

    SEMA_GIVE(READ);
    while (gotAddr == L7_TRUE)
    {
      if (clusterMsgSend(&macAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                         CP_CLUSTER_CP_CLIENT_NOTIFICATION_MSG, pdu.offset, buf) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: CP client notification to switch %02x:%02x:%02x:%02x:%02x:%02x failed",
                __FUNCTION__,
                macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],
                macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
        return L7_FAILURE;
      }
      SEMA_TAKE(READ);
      ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_NEXT);
      gotAddr = (ptrData != L7_NULLPTR) ? L7_TRUE : L7_FALSE;
      if (L7_TRUE == gotAddr)
      {
        memcpy(&macAddr, &(ptrData->macAddr), sizeof(L7_enetMacAddr_t));
      }
      SEMA_GIVE(READ);
    }

  }
  CP_DLOG(CPD_LEVEL_LOG, "%s: CP client notification msg sent to all peers.", __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP client authentication request message to peer switch
*
* @param    L7_enetMacAddr_t swMacAddr     @b{(input)} switch MAC address
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends a request message to previous client
*           authenticator switch for client authentication information.
*
* @end
*********************************************************************/
L7_RC_t cpClusterClientAuthReqMsgSend(L7_enetMacAddr_t swMacAddr, L7_enetMacAddr_t clientMacAddr)
{
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_enetMacAddr_t          tmpMacAddr;
  L7_enetMacAddr_t          switchMacAddr;



  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memset(&tmpMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memset(buf, 0, sizeof(buf));
  pdu.bufPtr = buf;
  pdu.offset = 0;

  message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
  message->msgType = osapiHtons(CP_CLUSTER_CP_CLIENT_AUTH_REQUEST_MSG);
  message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
  pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

  memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));

  if (memcmp(&switchMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
  {
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_TYPE,
                           CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
    CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address.", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }

  if (cpClientAuthReqMsgBuild(clientMacAddr, pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to build client %02x:%02x:%02x:%02x:%02x:%02x auth req msg",
            __FUNCTION__,
            clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
            clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
    return L7_FAILURE;
  }

  message->msgLen = osapiHtons(message->msgLen);

  if (clusterMsgSend(&swMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                     CP_CLUSTER_CP_CLIENT_AUTH_REQUEST_MSG, pdu.offset, buf) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send CP client auth request to switch %02x:%02x:%02x:%02x:%02x:%02x",
            __FUNCTION__,
            swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
            swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
    return L7_FAILURE;
  }

  CP_DLOG(CPD_LEVEL_LOG, "%s: CP client auth request msg sent to peer.", __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP client authentication reply message to peer switch
*
* @param    L7_enetMacAddr_t swMacAddr     @b{(input)} switch MAC address
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterClientAuthReplyMsgSend(L7_enetMacAddr_t swMacAddr, L7_enetMacAddr_t clientMacAddr)
{
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_enetMacAddr_t          tmpMacAddr;
  L7_enetMacAddr_t          switchMacAddr;



  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memset(&tmpMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memset(buf, 0, sizeof(buf));
  pdu.bufPtr = buf;
  pdu.offset = 0;

  message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
  message->msgType = osapiHtons(CP_CLUSTER_CP_CLIENT_AUTH_REPLY_MSG);
  message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
  pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

  memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));

  if (memcmp(&switchMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
  {
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_TYPE,
                           CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
    CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address.", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }

  if (cpClientAuthReplyMsgBuild(clientMacAddr, pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to build client %02x:%02x:%02x:%02x:%02x:%02x auth reply msg", __FUNCTION__,
            clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
            clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
    return L7_FAILURE;
  }

  message->msgLen = osapiHtons(message->msgLen);

  if (clusterMsgSend(&swMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                     CP_CLUSTER_CP_CLIENT_AUTH_REPLY_MSG, pdu.offset, buf) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send CP client auth reply to switch %02x:%02x:%02x:%02x:%02x:%02x",
            __FUNCTION__,
            swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
            swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
    return L7_FAILURE;
  }

  CP_DLOG(CPD_LEVEL_LOG, "%s: CP client auth reply msg sent to peer.", __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP controller command message
*
* @param    L7_enetMacAddr_t swMacAddr      @b{(input)} switch MAC address
* @param    L7_enetMacAddr_t clientMacAddr  @b{(input)} client MAC address
* @param    L7_uint32        cpId           @b{(input)} CP instance
* @param    L7_uint32        cpBlkUnblk     @b{(input)} CP instance to block/unblock
* @param    L7_BOOL          blkUnblkStatus @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends message to peer switch(es).
*
* @end
*********************************************************************/
L7_RC_t cpClusterControllerCmdMsgSend(L7_enetMacAddr_t swMacAddr,
                                      L7_enetMacAddr_t clientMacAddr, L7_uint32 cpId,
                                      L7_uint32 cpBlkUnblk, L7_BOOL blkUnblkStatus)
{
  cpdmPeerSwStatus_t       *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t          macAddr;
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_enetMacAddr_t          tmpMacAddr;
  L7_enetMacAddr_t          switchMacAddr;


  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memset(&tmpMacAddr, 0, sizeof(L7_enetMacAddr_t));
  memset(buf, 0, sizeof(buf));
  pdu.bufPtr = buf;
  pdu.offset = 0;

  message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
  message->msgType = osapiHtons(CP_CLUSTER_CP_CONTROLLER_COMMAND_MSG);
  message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
  pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

  memset(&switchMacAddr, 0, sizeof(L7_enetMacAddr_t));
  memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));

  if (memcmp(&switchMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
  {
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_TYPE,
                           CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
    CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }

  if (cpControllerCmdMsgBuild(clientMacAddr, cpId, cpBlkUnblk, blkUnblkStatus,
                              pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to build controller command msg", __FUNCTION__);
    return L7_FAILURE;
  }

  message->msgLen = osapiHtons(message->msgLen);

  if (memcmp(&swMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
  {
    if (clusterMsgSend(&swMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                       CP_CLUSTER_CP_CONTROLLER_COMMAND_MSG, pdu.offset, buf) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Can't send controller cmd to switch %02x:%02x:%02x:%02x:%02x:%02x",
              __FUNCTION__,
              swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
              swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
      return L7_FAILURE;
    }
  }
  else
  {
    L7_BOOL gotAddr = L7_FALSE;
    memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));

    SEMA_TAKE(READ);
    ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_NEXT);
    gotAddr = (L7_NULLPTR != ptrData) ? L7_TRUE : L7_FALSE;
    if (L7_TRUE == gotAddr)
    {
      memcpy(&macAddr, &(ptrData->macAddr), sizeof(L7_enetMacAddr_t));
    }
    SEMA_GIVE(READ);

    while (gotAddr)
    {
      if (clusterMsgSend(&macAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                         CP_CLUSTER_CP_CONTROLLER_COMMAND_MSG, pdu.offset, buf) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Can't send controller cmd to switch %02x:%02x:%02x:%02x:%02x:%02x",
                __FUNCTION__,
                macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],
                macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
        return L7_FAILURE;
      }
      SEMA_TAKE(READ);
      ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_NEXT);
      gotAddr = (ptrData != L7_NULLPTR) ? L7_TRUE : L7_FALSE;
      if (L7_TRUE == gotAddr)
      {
        memcpy(&macAddr, &(ptrData->macAddr), sizeof(L7_enetMacAddr_t));
      }
      SEMA_GIVE(READ);
    }
  }

  CP_DLOG(CPD_LEVEL_LOG, "%s: CP controller cmd msg sent to peer", __FUNCTION__);
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Send CP connected client update message
*
* @param    L7_enetMacAddr_t ctrlMacAddr   @b{(input)} controller MAC Address
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_BOOL          authStatus    @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends connected clients update message to
*           the cluster controller for ONE client.
*
* @end
*********************************************************************/
static
L7_RC_t cpConnClientsUpdateOneMsgSend(L7_enetMacAddr_t *ctrlMacAddr,
                                      L7_enetMacAddr_t *clientMacAddr,
                                      L7_BOOL authStatus)
{
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == ctrlMacAddr) || (L7_NULLPTR == clientMacAddr))
  {
    return rc;
  }

do {
    L7_uchar8   buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
    cpClusterPduHdl_t  pdu;
    cpClusterMsgHdr_t  *message = L7_NULLPTR;
    cpClusterMsgElementHdr_t  element;
    L7_enetMacAddr_t   tmpMacAddr, switchMacAddr;

    memset(&tmpMacAddr, 0, sizeof(L7_enetMacAddr_t));
    memset(buf, 0, sizeof(buf));
    pdu.bufPtr = buf;
    pdu.offset = 0;

    memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));
    if (memcmp(&switchMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) == 0)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }

    message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
    message->msgType = osapiHtons(CP_CLUSTER_CP_CONNECTED_CLIENTS_UPDATE_MSG);
    message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
    pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;
    CP_PKT_ELEMENT_HDR_SET(element, CP_CONN_CLIENTS_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS,
             CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
    CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);
    if (L7_SUCCESS !=
        cpConnClientsUpdateMsgBuild(*clientMacAddr, authStatus,
                                    pdu.bufPtr, &pdu.offset, &message->msgLen))
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Can't build client %02x:%02x:%02x:%02x:%02x:%02x update msg",
              __FUNCTION__,
              clientMacAddr->addr[0],clientMacAddr->addr[1],clientMacAddr->addr[2],
              clientMacAddr->addr[3],clientMacAddr->addr[4],clientMacAddr->addr[5]);
      break;
    }
    message->msgLen = osapiHtons(message->msgLen);

    rc = clusterMsgSend(ctrlMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                        CP_CLUSTER_CP_CONNECTED_CLIENTS_UPDATE_MSG, pdu.offset, buf);
    if (L7_SUCCESS != rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Can't update ctrllr %02x:%02x:%02x:%02x:%02x:%02x failed",
              __FUNCTION__,
              ctrlMacAddr->addr[0],ctrlMacAddr->addr[1],ctrlMacAddr->addr[2],
              ctrlMacAddr->addr[3],ctrlMacAddr->addr[4],ctrlMacAddr->addr[5]);
    }
  } while(0);
  return rc;
}

/*********************************************************************
* @purpose  Send CP connected client update message
*
* @param    L7_enetMacAddr_t ctrlMacAddr   @b{(input)} controller MAC Address
* @param    L7_BOOL          authStatus    @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends connected clients update message to
*           the cluster controller for ALL known clients.
*
* @end
*********************************************************************/
static
L7_RC_t cpConnClientsUpdateAllMsgSend(L7_enetMacAddr_t * ctrlMacAddr,
                                      L7_BOOL authStatus)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == ctrlMacAddr)
  {
    return rc;
  }

  do {
    L7_uchar8          buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
    cpClusterPduHdl_t  pdu;
    cpClusterMsgHdr_t  *message = L7_NULLPTR;
    cpClusterMsgElementHdr_t  element;
    L7_enetMacAddr_t   clientMac, tmpMacAddr, switchMacAddr;

    memset(&tmpMacAddr, 0, sizeof(L7_enetMacAddr_t));
    memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));

    if (memcmp(&switchMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) == 0)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address", __FUNCTION__, failed_to_get);
      return L7_FAILURE;
    }

    memset(buf, 0, CLUSTER_MAX_RELIABLE_MSG_SIZE);
    pdu.bufPtr = buf;

    memset(&clientMac, 0, sizeof(L7_enetMacAddr_t));

    pdu.offset = CP_CLUSTER_MSG_HDR_SIZE;

    message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
    message->msgType = osapiHtons(CP_CLUSTER_CP_CONNECTED_CLIENTS_UPDATE_MSG);
    message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
    CP_PKT_ELEMENT_HDR_SET(element,
      CP_CONN_CLIENTS_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS,
      CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);

    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
    CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);

    /* while there are clients to update */
    while (L7_SUCCESS ==
           cpdmClientConnStatusNextGet(&clientMac, &clientMac))
    {
      /* add this client */
        if (L7_SUCCESS !=
           cpConnClientsUpdateMsgBuild(clientMac, authStatus,
                                         pdu.bufPtr, &pdu.offset, &message->msgLen))
        {
           CP_DLOG(CPD_LEVEL_LOG, "%s: Can't add client %02x:%02x:%02x:%02x:%02x:%02x to msg",
                __FUNCTION__,
                clientMac.addr[0],clientMac.addr[1],clientMac.addr[2],
                clientMac.addr[3],clientMac.addr[4],clientMac.addr[5]);
           return rc;
        }

        /*  if not enough space in this message for one more entry.. */
        if ((message->msgLen + CONN_CLIENT_INFO_SIZE) >= CLUSTER_MAX_RELIABLE_MSG_SIZE)
        {
          /* .. then send message   */
          if (L7_SUCCESS !=
              clusterMsgSend(ctrlMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                             CP_CLUSTER_CP_CONNECTED_CLIENTS_UPDATE_MSG, pdu.offset, buf))
          {
            CP_DLOG(CPD_LEVEL_LOG, "%s: Connected clients update to ctrllr %02x:%02x:%02x:%02x:%02x:%02x failed",
                    __FUNCTION__,
                    ctrlMacAddr->addr[0],ctrlMacAddr->addr[1],ctrlMacAddr->addr[2],
                    ctrlMacAddr->addr[3],ctrlMacAddr->addr[4],ctrlMacAddr->addr[5]);
            return L7_FAILURE;
          }

          /* .. and reset message body & counters to prepare for another msg */
          memset(&buf[CP_CLUSTER_MSG_HDR_SIZE], 0,
                 CLUSTER_MAX_RELIABLE_MSG_SIZE - CP_CLUSTER_MSG_HDR_SIZE -
                 CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE);
          message->msgLen = pdu.offset =
            CP_CLUSTER_MSG_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
        }
    }

    /* if message is not empty */
    if (pdu.offset > (CP_CLUSTER_MSG_HDR_SIZE +
                      CP_CLUSTER_MSG_TYPE_SIZE +
                      CP_CLUSTER_MSG_LEN_SIZE +
                      CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE))
    {
      /*    send it */
      rc = clusterMsgSend(ctrlMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                          CP_CLUSTER_CP_CONNECTED_CLIENTS_UPDATE_MSG, pdu.offset, buf);

      if (L7_SUCCESS != rc)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Connected clients update to ctrllr %02x:%02x:%02x:%02x:%02x:%02x failed",
                __FUNCTION__,
                ctrlMacAddr->addr[0],ctrlMacAddr->addr[1],ctrlMacAddr->addr[2],
                ctrlMacAddr->addr[3],ctrlMacAddr->addr[4],ctrlMacAddr->addr[5]);
        break;
      }
    }
    rc = L7_SUCCESS;

  } while(0);
  return rc;
}

/*********************************************************************
* @purpose  Send CP connected client update message
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_BOOL          authStatus    @b{(input)} L7_TRUE or L7_FALSE
* @param    L7_enetMacAddr_t ctrlMacAddr   @b{(input)} controller MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends connected clients update message to
*           the cluster controller.
*
* @end
*********************************************************************/

L7_RC_t cpClusterConnClientsUpdateMsgSend(L7_enetMacAddr_t clientMacAddr, L7_BOOL authStatus,
                                          L7_enetMacAddr_t ctrlMacAddr)
{
  L7_enetMacAddr_t  switchMacAddr, nullMacAddr;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memset(&nullMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));

  if (memcmp(&switchMacAddr, &nullMacAddr, sizeof(L7_enetMacAddr_t)) == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address.", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }

  if (memcmp(&clientMacAddr, &nullMacAddr, sizeof(L7_enetMacAddr_t)) == 0)
  {
    return cpConnClientsUpdateAllMsgSend(&ctrlMacAddr, L7_TRUE);
  }
  return cpConnClientsUpdateOneMsgSend(&ctrlMacAddr, &clientMacAddr, authStatus);
}

/*********************************************************************
* @purpose  Send CP instance interface update message
*
* @param    L7_uint32        cpId        @b{(input)} CP ID
* @param    L7_uint32        intf        @b{(input)} CP interface
* @param    L7_enetMacAddr_t ctrlMacAddr @b{(input)} controller MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends instance interface update message to
*           the cluster controller.
*
* @end
*********************************************************************/
L7_RC_t cpClusterInstIntfUpdateMsgSend(L7_uint32 cpId, L7_uint32 intf,
                                       L7_enetMacAddr_t ctrlMacAddr)
{
  L7_uchar8          buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_enetMacAddr_t          tmpMacAddr;
  L7_enetMacAddr_t          switchMacAddr;
  cpId_t                    cp_id = 0;
  L7_CP_INST_OPER_STATUS_t  status;
  L7_BOOL                   cpIdStatus = L7_FALSE;
  L7_uint32                 cpIdAuthUsers = 0;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memset(&tmpMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memset(buf, 0, sizeof(buf));
  pdu.bufPtr = buf;
  pdu.offset = 0;

  message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
  message->msgType = osapiHtons(CP_CLUSTER_CP_INSTANCE_INTERFACE_UPDATE_MSG);
  message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
  pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

  memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));

  if (memcmp(&switchMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
  {
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_TYPE,
                           CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
    CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }


  /* if cpId is 0, it means all configured instances, else the specified instance */
  if (cpId == 0)
  {
    while (cpdmCPConfigNextGet(cp_id, &cp_id) == L7_SUCCESS)
    {
      if (cpdmCPConfigOperStatusGet(cp_id, &status) == L7_SUCCESS)
      {
        if (status == L7_CP_INST_OPER_STATUS_ENABLED)
        {
          cpIdStatus = L7_TRUE;
        }
        else
        {
          cpIdStatus = L7_FALSE;
        }
      }

      if (cpdmCPConfigAuthenticatedUsersGet(cp_id, &cpIdAuthUsers) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP ID %d authenticated users", __FUNCTION__, failed_to_get, cp_id);
        return L7_FAILURE;
      }

      cpId = (L7_uint32)cp_id;
      if (cpInstIntfUpdateMsgBuild(cpId, cpIdStatus, cpIdAuthUsers, intf,
                                   pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to build CPID %d update msg",  __FUNCTION__, cpId);
        return L7_FAILURE;
      }
    }
  }
  else
  {
    cp_id = (cpId_t)cpId;
    if (cpdmCPConfigOperStatusGet(cp_id, &status) == L7_SUCCESS)
    {
      if (status == L7_CP_INST_OPER_STATUS_ENABLED)
      {
        cpIdStatus = L7_TRUE;
      }
      else
      {
        cpIdStatus = L7_FALSE;
      }
    }

    if (cpdmCPConfigAuthenticatedUsersGet(cp_id, &cpIdAuthUsers) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP ID %d authenticated users",  __FUNCTION__, failed_to_get, cp_id);
      return L7_FAILURE;
    }

    if (cpInstIntfUpdateMsgBuild(cpId, cpIdStatus, cpIdAuthUsers, intf,
                                 pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to build CPID %d update msg",  __FUNCTION__, cpId);
      return L7_FAILURE;
    }
  }

  message->msgLen = osapiHtons(message->msgLen);

  if (clusterMsgSend(&ctrlMacAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                     CP_CLUSTER_CP_INSTANCE_INTERFACE_UPDATE_MSG, pdu.offset, buf) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send CP instance interface update to controller %02x:%02x:%02x:%02x:%02x:%02x",
            __FUNCTION__,
            ctrlMacAddr.addr[0],ctrlMacAddr.addr[1],ctrlMacAddr.addr[2],
            ctrlMacAddr.addr[3],ctrlMacAddr.addr[4],ctrlMacAddr.addr[5]);
    return L7_FAILURE;
  }

  CP_DLOG(CPD_LEVEL_LOG, "%s: CP instance interface update msg sent to cluster controller.", __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP statistics update message
*
* @param    L7_enetMacAddr_t ctrlMacAddr @b{(input)} controller MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends statistics update message to
*           the cluster controller.
*
* @end
*********************************************************************/



L7_RC_t cpClusterStatsUpdateMsgSend(L7_enetMacAddr_t ctrlMacAddr)
{
  L7_uchar8                 buf[CLUSTER_MAX_DATAGRAM_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_enetMacAddr_t          macAddr;
  L7_enetMacAddr_t          switchMacAddr;
  L7_uchar8                 *cantSend = "Can't send stat update msg to controller";


  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));
  {
    L7_enetMacAddr_t          nullMacAddr;
    memset(&nullMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    if (memcmp(&switchMacAddr, &nullMacAddr, sizeof(L7_enetMacAddr_t)) == 0)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Can't get local switch MAC addr", __FUNCTION__);
      return L7_FAILURE;
    }
  }
  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  memset(buf, 0, sizeof(buf));
  pdu.bufPtr = buf;
  pdu.offset = 0;
  message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
  message->msgType = osapiHtons(CP_CLUSTER_CP_STATISTICS_UPDATE_MSG);
  message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
  pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

  CP_PKT_ELEMENT_HDR_SET(element, CP_STATISTICS_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS,
                         CP_STATISTICS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
  message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_STATISTICS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE;
  CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);

  while (cpdmClientConnStatusNextGet(&macAddr, &macAddr) == L7_SUCCESS)
  {
     if (cpStatsUpdateMsgBuild(macAddr, pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
     {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Can't build client %02x:%02x:%02x:%02x:%02x:%02x stats update msg",
                __FUNCTION__,
                macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],
                macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
        return L7_FAILURE;
     }

     /* if not enough space in this message for one more entry.. */
     if ((message->msgLen + CONN_CLIENT_STATS_SIZE) >= CLUSTER_MAX_DATAGRAM_MSG_SIZE)
     {
       /* ..then send message */

       if (L7_SUCCESS != clusterMsgSend(&ctrlMacAddr, CLUSTER_MSG_DELIVERY_DATAGRAM,
                                        CP_CLUSTER_CP_STATISTICS_UPDATE_MSG,
                                        pdu.offset, buf))
       {
         CP_DLOG(CPD_LEVEL_LOG, cantSend);
         return L7_FAILURE;
       }
         /* .. and reset message body & counters to prepare for another msg */
       message->msgLen = CP_CLUSTER_MSG_HDR_SIZE +
                         CP_CLUSTER_IE_HDR_SIZE +
                         CP_STATISTICS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE;
       pdu.offset = message->msgLen;
     }
  }

  if (pdu.offset > (CP_CLUSTER_MSG_HDR_SIZE +
                    CP_CLUSTER_IE_HDR_SIZE +
                    CP_STATISTICS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE))
  {
     /* if message is not empty, send it */
    if (L7_SUCCESS !=
        clusterMsgSend(&ctrlMacAddr, CLUSTER_MSG_DELIVERY_DATAGRAM,
                       CP_CLUSTER_CP_STATISTICS_UPDATE_MSG, pdu.offset, buf))
    {
      CP_DLOG(CPD_LEVEL_LOG, cantSend);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP config sync message
*
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends config sync message to peer switch.
*
* @end
*********************************************************************/
L7_RC_t cpClusterConfigSyncMsgSend(L7_enetMacAddr_t swMacAddr)
{
  L7_uint32 status = L7_DISABLE;
  L7_RC_t rc = L7_SUCCESS;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if ((cpClusterConfigPushStatusGet(&status) == L7_SUCCESS) &&
      (status == L7_ENABLE))
  {
    rc = cpConfigMsgGlobalBuildSend(swMacAddr);

    if (rc == L7_SUCCESS)
    {
      rc = cpConfigMsgCPsBuildSend(swMacAddr);
    }

    if (rc == L7_SUCCESS)
    {
      rc = cpConfigMsgUserGroupBuildSend(swMacAddr);
    }

    if (rc == L7_SUCCESS)
    {
      rc = cpConfigMsgLocalUserBuildSend(swMacAddr);
    }

    if (rc != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send config message to switch %02x:%02x:%02x:%02x:%02x:%02x.\n",
              __FUNCTION__,
              swMacAddr.addr[0],swMacAddr.addr[1],swMacAddr.addr[2],
              swMacAddr.addr[3],swMacAddr.addr[4],swMacAddr.addr[5]);
      rc = L7_ERROR;
    }
  }

  if (clusterConfigSendDone(CLUSTER_CFG_ID(CAPTIVE_PORTAL), rc) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: clusterConfigSendDone() failed.", __FUNCTION__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send CP resend message
*
* @param    L7_uchar8 type @b{(input)} type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends resend message to one or more
*           peer switches in cluster.
*
* @end
*********************************************************************/
L7_RC_t cpClusterResendMsgSend(L7_uchar8 type)
{
  cpdmPeerSwStatus_t       *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t          macAddr;
  L7_uchar8                 buf[CLUSTER_MAX_RELIABLE_MSG_SIZE];
  cpClusterPduHdl_t         pdu;
  cpClusterMsgHdr_t        *message = L7_NULLPTR;
  cpClusterMsgElementHdr_t  element;
  L7_enetMacAddr_t          tmpMacAddr;
  L7_enetMacAddr_t          switchMacAddr;
  L7_uint32                 i;



  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memset(&tmpMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memset(buf, 0, sizeof(buf));
  pdu.bufPtr = buf;
  pdu.offset = 0;

  message = (cpClusterMsgHdr_t *)(pdu.bufPtr + pdu.offset);
  message->msgType = osapiHtons(CP_CLUSTER_CP_RESEND_MSG);
  message->msgLen = CP_CLUSTER_MSG_HDR_SIZE;
  pdu.offset += CP_CLUSTER_MSG_HDR_SIZE;

  memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&switchMacAddr, &cpdmOprData->cpLocalSw.macAddr, sizeof(L7_enetMacAddr_t));
  if (memcmp(&switchMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
  {
    CP_PKT_ELEMENT_HDR_SET(element, CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_TYPE,
                           CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE, pdu.bufPtr, pdu.offset);
    message->msgLen += CP_CLUSTER_IE_HDR_SIZE + CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE;
    CP_PKT_MAC_ADDR_SET(switchMacAddr.addr, pdu.bufPtr, pdu.offset);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s local switch MAC address", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }

  if (cpResendMsgBuild(type, pdu.bufPtr, &pdu.offset, &message->msgLen) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to build resend msg", __FUNCTION__);
    return L7_FAILURE;
  }

  message->msgLen = osapiHtons(message->msgLen);

  if (type == CP_CLUSTER_RESEND_CLIENT_NOTIFICATION)
  {
    for (i=0; i<CP_CLUSTER_MEMBERS; i++)
    {
      if (memcmp(&resendPeer[i], &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
      {
        if (clusterMsgSend(&(resendPeer[i]), CLUSTER_MSG_DELIVERY_RELIABLE,
                           CP_CLUSTER_CP_RESEND_MSG, pdu.offset, buf) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send resend request to switch %02x:%02x:%02x:%02x:%02x:%02x",
                  __FUNCTION__,
                  resendPeer[i].addr[0],resendPeer[i].addr[1],resendPeer[i].addr[2],
                  resendPeer[i].addr[3],resendPeer[i].addr[4],resendPeer[i].addr[5]);
          return L7_FAILURE;
        }
        memset(&resendPeer[i], 0x00, sizeof(L7_enetMacAddr_t));
      }
    }
  }
  else
  {
    L7_BOOL gotAddr = L7_FALSE;

    memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
    SEMA_TAKE(READ);
    ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_NEXT);
    gotAddr = (ptrData != L7_NULLPTR) ? L7_TRUE : L7_FALSE;
    if (L7_TRUE == gotAddr)
    {
      memcpy(&macAddr, &(ptrData->macAddr), sizeof(L7_enetMacAddr_t));
    }
    SEMA_GIVE(READ);
    while (gotAddr == L7_TRUE)
    {
      if (clusterMsgSend(&macAddr, CLUSTER_MSG_DELIVERY_RELIABLE,
                         CP_CLUSTER_CP_RESEND_MSG, pdu.offset, buf) != L7_SUCCESS)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send resend request to switch %02x:%02x:%02x:%02x:%02x:%02x",
                __FUNCTION__,
                macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],
                macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);

        return L7_FAILURE;
      }
      SEMA_TAKE(READ);
      ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_NEXT);
      gotAddr = (ptrData != L7_NULLPTR) ? L7_TRUE : L7_FALSE;
      if (L7_TRUE == gotAddr)
      {
        memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
      }
      SEMA_GIVE(READ);
    }

  }

  CP_DLOG(CPD_LEVEL_LOG, "%s: CP resend request msg sent", __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Cluster callback to process messages
*
* @param    L7_ushort16              msgType @b{(input)} msg id
* @param    clusterMsgDeliveryMethod method  @b{(input)} reliable
* @param    L7_uint32                msgLen  @b{(input)} overall msg length
* @param    L7_uchar8                buf     @b{(input)} pointer to msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterMsgCallback(L7_ushort16 msgType, clusterMsgDeliveryMethod method,
                          L7_uint32 msgLen, L7_uchar8 *buf)
{
  static L7_uchar8 * tooLong = "Rx msg length (%d) exceeds permitted length %d bytes!";
  static L7_uchar8 * msgTypeStr = "Msg type: 0x%x, %s msg";


  if ((method == CLUSTER_MSG_DELIVERY_RELIABLE) && (msgLen > CLUSTER_MAX_RELIABLE_MSG_SIZE))
  {
    CP_DLOG(CPD_LEVEL_LOG, tooLong, msgLen, CLUSTER_MAX_RELIABLE_MSG_SIZE);
    return;
  }

  if ((method == CLUSTER_MSG_DELIVERY_DATAGRAM) && (msgLen > CLUSTER_MAX_DATAGRAM_MSG_SIZE))
  {
    CP_DLOG(CPD_LEVEL_LOG, tooLong, msgLen, CLUSTER_MAX_DATAGRAM_MSG_SIZE);
    return;
  }

  switch (msgType)
  {
  case CP_CLUSTER_CP_CLIENT_NOTIFICATION_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "Client Notification");
    cpClusterClientNotificationMsgProcess(buf);
    break;

  case CP_CLUSTER_CP_CLIENT_AUTH_REQUEST_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "Client Auth Req");
    cpClusterClientAuthRequestMsgProcess(buf);
    break;

  case CP_CLUSTER_CP_CLIENT_AUTH_REPLY_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "Client Auth Reply");
    cpdmOprData->cpLocalSw.authReplyTime = osapiUpTimeRaw();
    cpClusterClientAuthReplyMsgProcess(buf);
    break;

  case CP_CLUSTER_CP_CONTROLLER_COMMAND_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "Controller Cmd");
    cpClusterControllerCmdMsgProcess(buf);
    break;

  case CP_CLUSTER_CP_CONNECTED_CLIENTS_UPDATE_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "Connected Clients Update");
    cpClusterConnClientsUpdateMsgProcess(buf);
    break;

  case CP_CLUSTER_CP_INSTANCE_INTERFACE_UPDATE_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "CP Inst Intf Update");
    cpClusterInstIntfUpdateMsgProcess(buf);
    break;

  case CP_CLUSTER_CP_STATISTICS_UPDATE_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "CP Statistics Update");
    cpClusterStatsUpdateMsgHandle(buf, msgLen);
    break;

  case CP_CLUSTER_CP_CONFIG_SYNC_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "CP Config Sync");
    if (cpClusterMsgError == L7_FALSE)
    {
      cpClusterConfigSyncMsgProcess(buf);
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "Error exists! Msg type 0x%x is ignored.", msgType);
    }
    break;

  case CP_CLUSTER_CP_RESEND_MSG:
    CP_DLOG(CPD_LEVEL_LOG, msgTypeStr, msgType, "Resend");
    cpClusterResendMsgProcess(buf);
    break;

  default:
    CP_DLOG(CPD_LEVEL_LOG, "Msg type: 0x%x UNKNOWN", msgType);
    break;
  }
}

/*********************************************************************
* @purpose  Process CP client notification message
*
* @param    L7_uchar8 buf @b{(input)} pointer to start of msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterClientNotificationMsgProcess(L7_uchar8 *buf)
{
  cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType, mLen, cpId = 0;
  L7_int32           totalIELen;
  L7_enetMacAddr_t   tmpMacAddr,switchMacAddr,clientMacAddr;
  L7_uchar8          clientStatus = 0;
  L7_uint32          i, clientIPAddr = 0, tmp32 = 0,ieCount = 0;
  L7_uchar8          *msgCltNotifyBuf;

  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad buffer", __FUNCTION__);
    return;
  }

  memset(&tmpMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  msgCltNotifyBuf = buf;
  memcpy(&hdr, msgCltNotifyBuf, sizeof(cpClusterMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;


  if (((mLen-CP_CLUSTER_MSG_HDR_SIZE) <= 0) ||
      (mLen > CLUSTER_MAX_RELIABLE_MSG_SIZE))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped.\n", __FUNCTION__, mLen);
    return;
  }

  msgCltNotifyBuf += CP_CLUSTER_MSG_HDR_SIZE;
  totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped.\n", __FUNCTION__ );
    return;
  }

  memcpy(&mType, msgCltNotifyBuf, CP_CLUSTER_IE_ID_SIZE);
  mType = osapiNtohs(mType);
  msgCltNotifyBuf += CP_CLUSTER_IE_ID_SIZE;
  totalIELen -= CP_CLUSTER_IE_ID_SIZE;

  memcpy(&mLen, msgCltNotifyBuf, CP_CLUSTER_IE_LEN_SIZE);
  mLen = osapiNtohs(mLen);
  msgCltNotifyBuf += CP_CLUSTER_IE_LEN_SIZE;
  totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  if (mType == CP_CLIENT_NOTIFICATION_IE_TYPE_SWITCH_MAC_ADDRESS)
  {
    memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memcpy(switchMacAddr.addr, msgCltNotifyBuf, mLen);
    msgCltNotifyBuf += mLen;
    totalIELen -= mLen;
    ieCount++;
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg, msg dropped.\n", __FUNCTION__ );
    return;
  }

  if (cpClusterSwitchEntryGet(switchMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg dropped", __FUNCTION__ ,
            switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
            switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
    return;
  }

  if (cpdmOprData->cpGlobalStatus.status != L7_CP_MODE_ENABLED)
  {
    for (i=0; i<CP_CLUSTER_MEMBERS; i++)
    {
      if (memcmp(&resendPeer[i], &tmpMacAddr, sizeof(L7_enetMacAddr_t)) == 0)
      {
        memcpy(&resendPeer[i], &switchMacAddr, sizeof(L7_enetMacAddr_t));
        break;
      }
    }
    if (cpClusterResendFlagSet(CP_CLUSTER_RESEND_REQ_CLIENT_NOTIFY) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set CP clients notification resend request flag", __FUNCTION__ );
    }
    return;
  }

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCltNotifyBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCltNotifyBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgCltNotifyBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCltNotifyBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP client notification msg failed.", __FUNCTION__ );
      return;
    }

    switch (mType)
    {
    case CP_CLIENT_NOTIFICATION_IE_TYPE_CLIENT_MAC_ADDRESS:
      if (mLen == CP_CLIENT_NOTIFICATION_IE_CLIENT_MAC_ADDRESS_SIZE)
      {
        memset(&clientMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
        memcpy(clientMacAddr.addr, msgCltNotifyBuf, mLen);
        ieCount++;
      }
      break;
    case CP_CLIENT_NOTIFICATION_IE_TYPE_CLIENT_IP_ADDRESS:
      if (mLen == CP_CLIENT_NOTIFICATION_IE_CLIENT_IP_ADDRESS_SIZE)
      {
        memcpy(&clientIPAddr, msgCltNotifyBuf, mLen);
        clientIPAddr = osapiNtohl(clientIPAddr);
        ieCount++;
      }
      break;
    case CP_CLIENT_NOTIFICATION_IE_TYPE_CLIENT_CPID:
      if (mLen == CP_CLIENT_NOTIFICATION_IE_CLIENT_CPID_SIZE)
      {
        memcpy(&tmp32, msgCltNotifyBuf, mLen);
        tmp32 = osapiNtohl(tmp32);
        cpId = (L7_ushort16)tmp32;
        ieCount++;
      }
      break;
    case CP_CLIENT_NOTIFICATION_IE_TYPE_CLIENT_STATUS:
      if (mLen == CP_CLIENT_NOTIFICATION_IE_CLIENT_STATUS_SIZE)
      {
        clientStatus = *msgCltNotifyBuf;
        ieCount++;
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type: 0x%x.", __FUNCTION__ , mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgCltNotifyBuf += mLen;
    }

    if (ieCount == CP_CLIENT_NOTIFICATION_MSG_TOTAL_IE_COUNT)
    {
      if (clientStatus == CP_CLUSTER_CLIENT_CONNECTED)
      {
        if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
        {
          if (cpdmClientConnStatusGet(&clientMacAddr) == L7_SUCCESS)
          {
            cpdmOprData->cpLocalSw.userInitiated = L7_FALSE;
            if (cpdmClientConnStatusDelete(&clientMacAddr) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to delete connected client %02x:%02x:%02x:%02x:%02x:%02x.\n",
                      __FUNCTION__ ,
                      clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                      clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
            }
          }
        }

        if (cpClusterSwitchClientEntryGet(clientMacAddr) != L7_SUCCESS)
        {
          if (peerSwClientTree.count == (CP_CLIENT_CONN_STATUS_MAX*2))
          {
            CP_DLOG(CPD_LEVEL_LOG, "%s: Peer Switch Client database full, msg dropped.", __FUNCTION__ );
            return;
          }
        }

          /* add/update client switch info */
        if (cpClusterSwitchClientEntryAddUpdate(clientMacAddr, clientIPAddr, cpId, switchMacAddr) != L7_SUCCESS)
        {
            CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update peer switch client %02x:%02x:%02x:%02x:%02x:%02x.\n",
                    __FUNCTION__ ,
                    clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                    clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
        }
      }
      else
      {
        if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
        {
          if (cpdmClientConnStatusGet(&clientMacAddr) == L7_SUCCESS)
          {
            cpdmOprData->cpLocalSw.userInitiated = L7_FALSE;
            if (cpdmClientConnStatusDelete(&clientMacAddr) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to delete connected client %02x:%02x:%02x:%02x:%02x:%02x.\n",
                      __FUNCTION__ ,
                      clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                      clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
            }
          }
        }

        if (cpClusterSwitchClientEntryDelete(clientMacAddr, switchMacAddr) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to delete peer switch client %02x:%02x:%02x:%02x:%02x:%02x.\n",
                  __FUNCTION__ ,
                  clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                  clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
        }
      }
      ieCount = 1;
      clientStatus = 0;
    }
  }
}

/*********************************************************************
* @purpose  Process CP client authentication request message
*
* @param    L7_uchar8 buf @b{(input)} pointer to start of msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterClientAuthRequestMsgProcess(L7_uchar8 *buf)
{
  cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType;
  L7_ushort16        mLen;
  L7_int32           totalIELen;
  L7_uint32          ieCount = 0;
  L7_enetMacAddr_t   switchMacAddr;
  L7_enetMacAddr_t   clientMacAddr;
  L7_uchar8          *msgCltAuthReqBuf;



  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: bad buffer", __FUNCTION__);
    return;
  }

  if (cpdmOprData->cpGlobalStatus.status != L7_CP_MODE_ENABLED)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: CP disabled, msg dropped.", __FUNCTION__);
    return;
  }

  msgCltAuthReqBuf = buf;
  memcpy(&hdr, msgCltAuthReqBuf, sizeof(cpClusterMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;

  msgCltAuthReqBuf += CP_CLUSTER_MSG_HDR_SIZE;

  if ((mLen <=0) || ((mLen-CP_CLUSTER_MSG_HDR_SIZE) <= 0))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped.\n", __FUNCTION__, mLen);
    return;
  }

  totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped.", __FUNCTION__);
    return;
  }

  memcpy(&mType, msgCltAuthReqBuf, CP_CLUSTER_IE_ID_SIZE);
  mType = osapiNtohs(mType);
  msgCltAuthReqBuf += CP_CLUSTER_IE_ID_SIZE;
  totalIELen -= CP_CLUSTER_IE_ID_SIZE;

  memcpy(&mLen, msgCltAuthReqBuf, CP_CLUSTER_IE_LEN_SIZE);
  mLen = osapiNtohs(mLen);
  msgCltAuthReqBuf += CP_CLUSTER_IE_LEN_SIZE;
  totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  if (mType == CP_CLIENT_AUTH_REQUEST_IE_TYPE_SWITCH_MAC_ADDRESS)
  {
    memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memcpy(switchMacAddr.addr, msgCltAuthReqBuf, mLen);
    msgCltAuthReqBuf += mLen;
    totalIELen -= mLen;
    ieCount++;
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg, msg dropped.", __FUNCTION__);
    return;
  }

  if (cpClusterSwitchEntryGet(switchMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg dropped.\n",
            __FUNCTION__,
            switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
            switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
    return;
  }

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCltAuthReqBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCltAuthReqBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgCltAuthReqBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCltAuthReqBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP client auth request msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_CLIENT_AUTH_REQUEST_IE_TYPE_CLIENT_MAC_ADDRESS:
      if (mLen == CP_CLIENT_AUTH_REQUEST_IE_CLIENT_MAC_ADDRESS_SIZE)
      {
        memset(&clientMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
        memcpy(clientMacAddr.addr, msgCltAuthReqBuf, mLen);
        ieCount++;
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n", __FUNCTION__, mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgCltAuthReqBuf += mLen;
    }

    if (ieCount == CP_CLIENT_AUTH_REQUEST_MSG_TOTAL_IE_COUNT)
    {
      /* check if client is in CC database */
      if (cpdmClientConnStatusGet(&clientMacAddr) == L7_SUCCESS)
      {
        memcpy(&cpdmOprData->cpLocalSw.peer, &switchMacAddr, sizeof(L7_enetMacAddr_t));
        memcpy(&cpdmOprData->cpLocalSw.authClient, &clientMacAddr, sizeof(L7_enetMacAddr_t));
        if (cpClusterFlagSet(CP_CLUSTER_CLIENT_AUTH_REPLY_SEND) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set client auth reply send flag.", __FUNCTION__);
          memset(&cpdmOprData->cpLocalSw.peer, 0, sizeof(L7_enetMacAddr_t));
          memset(&cpdmOprData->cpLocalSw.authClient, 0, sizeof(L7_enetMacAddr_t));
          return;
        }
      }

      ieCount = 1;
    }
  }
}

/*********************************************************************
* @purpose  Process CP client authentication reply message
*
* @param    L7_uchar8 buf @b{(input)} pointer to start of msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterClientAuthReplyMsgProcess(L7_uchar8 *buf)
{
  L7_uchar8          *msgCltAuthReplyBuf;
  cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType;
  L7_ushort16        mLen;
  L7_int32           totalIELen;
  L7_uint32          ieCount = 0;
  L7_enetMacAddr_t   switchMacAddr;
  L7_enetMacAddr_t   clientMacAddr;
  L7_uint32          intf = 0, cpId = 0, time = 0;
  L7_char8           uName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8           pwd[CP_USER_LOCAL_PASSWORD_MAX+1];
  L7_uint64          bTx = 0, bRx = 0, pTx = 0, pRx = 0;
  L7_uint32          upRate = 0, dnRate = 0;




  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: bad buffer", __FUNCTION__);
    return;
  }

  memset(uName, 0x00, sizeof(uName));
  memset(pwd, 0x00, sizeof(pwd));

  if (cpdmOprData->cpGlobalStatus.status != L7_CP_MODE_ENABLED)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: CP disabled, msg dropped.", __FUNCTION__);
    return;
  }

  msgCltAuthReplyBuf = buf;
  memcpy(&hdr, msgCltAuthReplyBuf, sizeof(cpClusterMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;

  msgCltAuthReplyBuf += CP_CLUSTER_MSG_HDR_SIZE;

  if ((mLen <=0) || ((mLen-CP_CLUSTER_MSG_HDR_SIZE) <= 0))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped", __FUNCTION__, mLen);
    return;
  }

  totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped.", __FUNCTION__);
    return;
  }

  memcpy(&mType, msgCltAuthReplyBuf, CP_CLUSTER_IE_ID_SIZE);
  mType = osapiNtohs(mType);
  msgCltAuthReplyBuf += CP_CLUSTER_IE_ID_SIZE;
  totalIELen -= CP_CLUSTER_IE_ID_SIZE;

  memcpy(&mLen, msgCltAuthReplyBuf, CP_CLUSTER_IE_LEN_SIZE);
  mLen = osapiNtohs(mLen);
  msgCltAuthReplyBuf += CP_CLUSTER_IE_LEN_SIZE;
  totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  if (mType == CP_CLIENT_AUTH_REPLY_IE_TYPE_SWITCH_MAC_ADDRESS)
  {
    memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memcpy(switchMacAddr.addr, msgCltAuthReplyBuf, mLen);
    msgCltAuthReplyBuf += mLen;
    totalIELen -= mLen;
    ieCount++;
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg, msg dropped.", __FUNCTION__);
    return;
  }

  if (cpClusterSwitchEntryGet(switchMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg dropped.\n",
            __FUNCTION__,
            switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
            switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
    return;
  }

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCltAuthReplyBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCltAuthReplyBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgCltAuthReplyBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCltAuthReplyBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP client auth reply msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_CLIENT_AUTH_REPLY_IE_TYPE_CLIENT_MAC_ADDRESS:
      if (mLen == CP_CLIENT_AUTH_REPLY_IE_CLIENT_MAC_ADDRESS_SIZE)
      {
        memset(&clientMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
        memcpy(clientMacAddr.addr, msgCltAuthReplyBuf, mLen);
        ieCount++;
      }
      break;
    case CP_CLIENT_AUTH_REPLY_IE_TYPE_AUTH_DATA:
      if (mLen == CP_CLIENT_AUTH_REPLY_IE_AUTH_DATA_SIZE)
      {
        memcpy(&intf, msgCltAuthReplyBuf, CP_CLUSTER_IE_CP_INTERFACE_INDEX_SIZE);
        intf = osapiNtohl(intf);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_CP_INTERFACE_INDEX_SIZE;

        memcpy(&cpId, msgCltAuthReplyBuf, CP_CLUSTER_IE_CP_INST_SIZE);
        cpId = osapiNtohl(cpId);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_CP_INST_SIZE;

        memcpy(&time, msgCltAuthReplyBuf, CP_CLUSTER_IE_COUNTER32_SIZE);
        time = osapiNtohl(time);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_COUNTER32_SIZE;

        osapiStrncpy(uName, msgCltAuthReplyBuf, CP_USER_LOCAL_USERNAME_MAX);
        uName[CP_USER_LOCAL_USERNAME_MAX] = '\0';
        msgCltAuthReplyBuf += CP_USER_LOCAL_USERNAME_MAX;

        osapiStrncpy(pwd, msgCltAuthReplyBuf, CP_USER_LOCAL_PASSWORD_MAX);
        pwd[CP_USER_LOCAL_PASSWORD_MAX] = '\0';
        msgCltAuthReplyBuf += CP_USER_LOCAL_PASSWORD_MAX;

        memcpy(&bTx, msgCltAuthReplyBuf, CP_CLUSTER_IE_COUNTER64_SIZE);
        bTx = osapiNtohll(bTx);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_COUNTER64_SIZE;

        memcpy(&bRx, msgCltAuthReplyBuf, CP_CLUSTER_IE_COUNTER64_SIZE);
        bRx = osapiNtohll(bRx);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_COUNTER64_SIZE;

        memcpy(&pTx, msgCltAuthReplyBuf, CP_CLUSTER_IE_COUNTER64_SIZE);
        pTx = osapiNtohll(pTx);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_COUNTER64_SIZE;

        memcpy(&pRx, msgCltAuthReplyBuf, CP_CLUSTER_IE_COUNTER64_SIZE);
        pRx = osapiNtohll(pRx);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_COUNTER64_SIZE;

        memcpy(&upRate, msgCltAuthReplyBuf, CP_CLUSTER_IE_COUNTER32_SIZE);
        upRate = osapiNtohl(upRate);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_COUNTER32_SIZE;

        memcpy(&dnRate, msgCltAuthReplyBuf, CP_CLUSTER_IE_COUNTER32_SIZE);
        dnRate = osapiNtohl(dnRate);
        msgCltAuthReplyBuf += CP_CLUSTER_IE_COUNTER32_SIZE;

        ieCount++;
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n", __FUNCTION__,  mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgCltAuthReplyBuf += mLen;
    }

    if (ieCount == CP_CLIENT_AUTH_REPLY_MSG_TOTAL_IE_COUNT)
    {
      /* store info for use later */
      memcpy(&cpdmOprData->cpClientAuthInfo.swMacAddr, &switchMacAddr, sizeof(L7_enetMacAddr_t));
      memcpy(&cpdmOprData->cpClientAuthInfo.macAddr, &clientMacAddr, sizeof(L7_enetMacAddr_t));
      cpdmOprData->cpClientAuthInfo.cpId = cpId;
      cpdmOprData->cpClientAuthInfo.intf = intf;
      cpdmOprData->cpClientAuthInfo.sessionTime = time;

      strncpy(cpdmOprData->cpClientAuthInfo.uName, uName, CP_USER_LOCAL_USERNAME_MAX);
      cpdmOprData->cpClientAuthInfo.uName[CP_USER_LOCAL_USERNAME_MAX] = '\0';
      strncpy(cpdmOprData->cpClientAuthInfo.pwd, pwd, CP_USER_LOCAL_PASSWORD_MAX);
      cpdmOprData->cpClientAuthInfo.pwd[CP_USER_LOCAL_PASSWORD_MAX] = '\0';

      cpdmOprData->cpClientAuthInfo.bytesTx = bTx;
      cpdmOprData->cpClientAuthInfo.bytesRx = bRx;
      cpdmOprData->cpClientAuthInfo.pktsTx = pTx;
      cpdmOprData->cpClientAuthInfo.pktsRx = pRx;
      cpdmOprData->cpClientAuthInfo.userUpRate = upRate;
      cpdmOprData->cpClientAuthInfo.userDownRate = dnRate;

      cpdmOprData->cpClientAuthInfo.authPending = L7_TRUE;

      ieCount = 1;
    }
  }
}

/*********************************************************************
* @purpose  Process CP controller command message
*
* @param    L7_uchar8 buf @b{(input)} pointer to start of msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterControllerCmdMsgProcess(L7_uchar8 *buf)
{
    L7_uchar8          *msgCtrlCmdBuf;
    cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType;
  L7_ushort16        mLen;
  L7_int32           totalIELen;
  L7_uint32          ieCount = 0;
  L7_enetMacAddr_t   tmpMacAddr;
  L7_enetMacAddr_t   switchMacAddr;
  L7_enetMacAddr_t   clientMacAddr;
    L7_uint32          cpId = 0, cpBlock = 0, cpUnblock = 0;


  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad buffer", __FUNCTION__);
    return;
  }

  if (cpdmOprData->cpGlobalStatus.status != L7_CP_MODE_ENABLED)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: CP disabled, msg dropped.", __FUNCTION__);
    return;
    }

    msgCtrlCmdBuf = buf;
    memcpy(&hdr, msgCtrlCmdBuf, sizeof(cpClusterMsgHdr_t));
    mType = hdr.msgType;
    mLen = hdr.msgLen;

    msgCtrlCmdBuf += CP_CLUSTER_MSG_HDR_SIZE;

  if ((mLen <=0) || ((mLen-CP_CLUSTER_MSG_HDR_SIZE) <= 0))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped.\n", __FUNCTION__, mLen);
    return;
    }

    totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped.", __FUNCTION__);
    return;
  }

  memset(&tmpMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memcpy(&mType, msgCtrlCmdBuf, CP_CLUSTER_IE_ID_SIZE);
    mType = osapiNtohs(mType);
    msgCtrlCmdBuf += CP_CLUSTER_IE_ID_SIZE;
    totalIELen -= CP_CLUSTER_IE_ID_SIZE;

    memcpy(&mLen, msgCtrlCmdBuf, CP_CLUSTER_IE_LEN_SIZE);
    mLen = osapiNtohs(mLen);
    msgCtrlCmdBuf += CP_CLUSTER_IE_LEN_SIZE;
    totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  if (mType == CP_CTRL_CMD_IE_TYPE_SWITCH_MAC_ADDRESS)
  {
    memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memcpy(switchMacAddr.addr, msgCtrlCmdBuf, mLen);
    msgCtrlCmdBuf += mLen;
    totalIELen -= mLen;
    ieCount++;
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg, msg dropped.", __FUNCTION__);
    return;
  }

  if (cpClusterSwitchEntryGet(switchMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg dropped",
            __FUNCTION__,
            switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
            switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
    return;
  }

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgCtrlCmdBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgCtrlCmdBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgCtrlCmdBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgCtrlCmdBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP controller command msg failed.", __FUNCTION__);
        return;
    }

    switch (mType)
    {
        case CP_CTRL_CMD_IE_TYPE_CLIENT_MAC_ADDRESS:
      if (mLen == CP_CTRL_CMD_IE_CLIENT_MAC_ADDRESS_SIZE)
      {
        memset(&clientMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
            memcpy(clientMacAddr.addr, msgCtrlCmdBuf, mLen);
            ieCount++;
        }
        break;
        case CP_CTRL_CMD_IE_TYPE_INST_INDEX:
      if (mLen == CP_CTRL_CMD_IE_INST_INDEX_SIZE)
      {
            memcpy(&cpId, msgCtrlCmdBuf, mLen);
            cpId = osapiNtohl(cpId);
            ieCount++;
        }
        break;
        case CP_CTRL_CMD_IE_TYPE_DEAUTH_ALL_CLIENTS:
      if (mLen == CP_CTRL_CMD_IE_DEAUTH_ALL_CLIENTS_SIZE)
      {
            ieCount++;
        }
        break;
        case CP_CTRL_CMD_IE_TYPE_INST_BLOCK:
      if (mLen == CP_CTRL_CMD_IE_INST_BLOCK_SIZE)
      {
            memcpy(&cpBlock, msgCtrlCmdBuf, mLen);
            cpBlock = osapiNtohl(cpBlock);
            ieCount++;
        }
        break;
        case CP_CTRL_CMD_IE_TYPE_INST_UNBLOCK:
      if (mLen == CP_CTRL_CMD_IE_INST_UNBLOCK_SIZE)
      {
            memcpy(&cpUnblock, msgCtrlCmdBuf, mLen);
            cpUnblock = osapiNtohl(cpUnblock);
            ieCount++;
        }
        break;
        default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d", __FUNCTION__, mType);
        break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
        msgCtrlCmdBuf += mLen;
    }

    if (ieCount == CP_CTRL_CMD_MSG_TOTAL_IE_COUNT)
    {
      if (cpBlock != 0)
      {
        if (cpdmCPConfigBlockedStatusSet((cpId_t)cpBlock,
                                         L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to block instance %d", __FUNCTION__, cpBlock);
        }
      }
      else if (cpUnblock != 0)
      {
        if (cpdmCPConfigBlockedStatusSet((cpId_t)cpUnblock,
                                         L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to unblock instance %d", __FUNCTION__, cpUnblock);
        }
      }
      else
      {
        if (memcmp(&clientMacAddr, &tmpMacAddr, sizeof(L7_enetMacAddr_t)) != 0)
        {
          if (cpdmClientConnStatusGet(&clientMacAddr) == L7_SUCCESS)
          {
            cpdmOprData->cpLocalSw.userInitiated = L7_FALSE;
            if (cpdmClientConnStatusDelete(&clientMacAddr) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to deauth client %02x:%02x:%02x:%02x:%02x:%02x",
                      __FUNCTION__,
                      clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                      clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
            }
          }
        }
        else
        {
          if (cpId != 0)
          {
            if (cpdmClientConnStatusByCPIdDeleteAll((cpId_t)cpId) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to deauth clients on instance %d", __FUNCTION__, cpId);
            }
          }
          else
          {
            if (cpdmClientConnStatusDeleteAll() != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to deauth all clients.", __FUNCTION__);
            }
          }
        }
      }

        ieCount = 1;
    }
    }
}

/*********************************************************************
* @purpose  Process CP connected client update message
*
* @param    L7_uchar8 buf @b{(input)} pointer to start of msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterConnClientsUpdateMsgProcess(L7_uchar8 *buf)
{
  L7_uchar8          *msgConnClientsBuf, clientStatus = 0;
  L7_uchar8          userName[CP_USER_LOCAL_USERNAME_MAX+1];
  cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType, mLen, totalClients = 0;
  L7_int32           totalIELen;
  L7_uint32          ieCount = 0, intf = 0, cpId = 0;
  L7_enetMacAddr_t   switchMacAddr, clientMacAddr;
  cpdmAuthWhichSwitch_t swType;
  L7_IP_ADDR_t       ipAddr = 0;
  L7_LOGIN_TYPE_t    prtlMode = 0;
  CP_VERIFY_MODE_t   verifyMode = 0;
  txRxCount_t        peerStats;


  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad buffer", __FUNCTION__);
    return;
  }

  if (cpdmOprData->cpLocalSw.status != CP_CLUSTER_SWITCH_CONTROLLER)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Not a cluster controller, msg dropped.", __FUNCTION__);
    return;
  }

  if (cpdmOprData->cpGlobalStatus.status != L7_CP_MODE_ENABLED)
  {
    if (cpClusterResendFlagSet(CP_CLUSTER_RESEND_REQ_CONN_CLIENTS) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set CP connected clients resend request flag.", __FUNCTION__);
    }
    return;
  }

  msgConnClientsBuf = buf;
  memcpy(&hdr, msgConnClientsBuf, sizeof(cpClusterMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;

  msgConnClientsBuf += CP_CLUSTER_MSG_HDR_SIZE;

  if ((mLen <=0) || ((mLen-CP_CLUSTER_MSG_HDR_SIZE) <= 0))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped.\n", __FUNCTION__,mLen);
    return;
  }

  totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped.", __FUNCTION__);
    return;
  }

  memcpy(&mType, msgConnClientsBuf, CP_CLUSTER_IE_ID_SIZE);
  mType = osapiNtohs(mType);
  msgConnClientsBuf += CP_CLUSTER_IE_ID_SIZE;
  totalIELen -= CP_CLUSTER_IE_ID_SIZE;

  memcpy(&mLen, msgConnClientsBuf, CP_CLUSTER_IE_LEN_SIZE);
  mLen = osapiNtohs(mLen);
  msgConnClientsBuf += CP_CLUSTER_IE_LEN_SIZE;
  totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  if (mType == CP_CONN_CLIENTS_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS)
  {
    memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memcpy(switchMacAddr.addr, msgConnClientsBuf, mLen);
    msgConnClientsBuf += mLen;
    totalIELen -= mLen;
    ieCount++;
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg, msg dropped.", __FUNCTION__);
    return;
  }

  if (cpClusterSwitchEntryGet(switchMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg dropped.\n", __FUNCTION__,
            switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
            switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
    return;
  }

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgConnClientsBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgConnClientsBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgConnClientsBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgConnClientsBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP connected client update msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_CLIENT_MAC_ADDRESS:
      if (mLen == CP_CONN_CLIENTS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE)
      {
        memset(&clientMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
        memcpy(clientMacAddr.addr, msgConnClientsBuf, mLen);
        ieCount++;
      }
      break;
    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_CLIENT_STATUS:
      if (mLen == CP_CONN_CLIENTS_UPDATE_IE_CLIENT_STATUS_SIZE)
      {
        clientStatus = *msgConnClientsBuf;
        ieCount++;
      }
      break;

    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_CLIENT_IP:
      if (mLen == CP_CONN_CLIENTS_UPDATE_IE_CLIENT_IP_SIZE)
      {
        memcpy(&ipAddr, msgConnClientsBuf, mLen);
        ipAddr = osapiNtohl(ipAddr);
        ieCount++;
      }
      break;
    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_USERNAME:
      if (mLen <= CP_CONN_CLIENTS_UPDATE_IE_USERNAME_SIZE)
      {
        osapiStrncpy(userName, msgConnClientsBuf, mLen);
        userName[mLen] = '\0';
        ieCount++;
      }
      break;
    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_PROTOCOL:
      if (mLen == CP_CONN_CLIENTS_UPDATE_IE_PROTOCOL_SIZE)
      {
        memcpy(&prtlMode, msgConnClientsBuf, mLen);
        prtlMode = osapiNtohl(prtlMode);
        ieCount++;
      }
      break;
    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_VERIFY:
      if (mLen == CP_CONN_CLIENTS_UPDATE_IE_VERIFY_SIZE)
      {
        memcpy(&verifyMode, msgConnClientsBuf, mLen);
        verifyMode = osapiNtohl(verifyMode);
        ieCount++;
      }
      break;
    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_INTF:
      if (mLen == CP_CONN_CLIENTS_UPDATE_IE_INTF_SIZE)
      {
        memcpy(&intf, msgConnClientsBuf, mLen);
        intf = osapiNtohl(intf);
        ieCount++;
      }
      break;
    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_CPID:
      if (mLen == CP_CONN_CLIENTS_UPDATE_IE_CPID_SIZE)
      {
        memcpy(&cpId, msgConnClientsBuf, mLen);
        cpId = osapiNtohl(cpId);
        ieCount++;
      }
      break;
    case CP_CONN_CLIENTS_UPDATE_IE_TYPE_STATS_DATA:
      if (mLen == CP_CONN_CLIENTS_UPDATE_IE_STATS_DATA_SIZE)
      {
        memset(&peerStats, 0, sizeof(txRxCount_t));

        memcpy(&peerStats.bytesTransmitted, msgConnClientsBuf, CP_CLUSTER_IE_COUNTER64_SIZE);
        peerStats.bytesTransmitted = osapiNtohll(peerStats.bytesTransmitted);
        msgConnClientsBuf += CP_CLUSTER_IE_COUNTER64_SIZE;

        memcpy(&peerStats.bytesReceived, msgConnClientsBuf, CP_CLUSTER_IE_COUNTER64_SIZE);
        peerStats.bytesReceived = osapiNtohll(peerStats.bytesReceived);
        msgConnClientsBuf += CP_CLUSTER_IE_COUNTER64_SIZE;

        memcpy(&peerStats.packetsTransmitted, msgConnClientsBuf, CP_CLUSTER_IE_COUNTER64_SIZE);
        peerStats.packetsTransmitted = osapiNtohll(peerStats.packetsTransmitted);
        msgConnClientsBuf += CP_CLUSTER_IE_COUNTER64_SIZE;

        memcpy(&peerStats.packetsReceived, msgConnClientsBuf, CP_CLUSTER_IE_COUNTER64_SIZE);
        peerStats.packetsReceived = osapiNtohll(peerStats.packetsReceived);
        msgConnClientsBuf += CP_CLUSTER_IE_COUNTER64_SIZE;

        ieCount++;
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n", __FUNCTION__, mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgConnClientsBuf += mLen;
    }

    if (ieCount == CP_CONN_CLIENTS_UPDATE_MSG_TOTAL_IE_COUNT)
    {
      if (clientStatus == CP_CLUSTER_CLIENT_CONNECTED)
      {
        if (cpdmClientConnStatusTotalGet(&totalClients) == CP_CLIENT_CONN_STATUS_MAX)
        {
          if (L7_SUCCESS != cpdmClientConnStatusDeauthAdd(&switchMacAddr, &clientMacAddr))
          {
            CP_DLOG(CPD_LEVEL_LOG, "%s: Can't enq client for deauth?", __FUNCTION__);
          }
          return;
        }

        if (cpdmClientConnStatusGet(&clientMacAddr) == L7_SUCCESS)
        {
          if (cpdmClientConnStatusWhichSwitchGet(&clientMacAddr, &swType) != L7_SUCCESS)
          {
            CP_DLOG(CPD_LEVEL_LOG, "%s: %s client switch type.", __FUNCTION__, failed_to_get);
          }

          /* update client info */
          if (swType == CP_AUTH_ON_PEER_SWITCH)
          {
            if (cpdmClientConnStatusPeerAddUpdate(clientMacAddr, ipAddr, switchMacAddr, userName,
                                                  prtlMode, verifyMode, intf, cpId, &peerStats) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update peer client %02x:%02x:%02x:%02x:%02x:%02x",
                       __FUNCTION__,
                      clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                      clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
            }
          }
          else
          {
            if (cpdmClientConnStatusCrtlUpdate(clientMacAddr, switchMacAddr) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update ctrl client %02x:%02x:%02x:%02x:%02x:%02x",
                       __FUNCTION__,
                      clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                      clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
            }
          }
        }
        else
        {
          if (L7_SUCCESS !=
              cpdmClientConnStatusPeerAddUpdate(clientMacAddr, ipAddr, switchMacAddr, userName,
                                                prtlMode, verifyMode, intf, cpId, &peerStats))
          {
            CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to add peer client %02x:%02x:%02x:%02x:%02x:%02x.\n",__FUNCTION__,
                    clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                    clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
          }
        }
      }
      else
      {
        if (cpdmClientConnStatusGet(&clientMacAddr) == L7_SUCCESS)
        {
          if (cpdmClientConnStatusPeerDelete(clientMacAddr, switchMacAddr) != L7_SUCCESS)
          {
            CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to delete connected client %02x:%02x:%02x:%02x:%02x:%02x.\n",
                    __FUNCTION__,
                    clientMacAddr.addr[0],clientMacAddr.addr[1],clientMacAddr.addr[2],
                    clientMacAddr.addr[3],clientMacAddr.addr[4],clientMacAddr.addr[5]);
          }
        }
      }

      ieCount = 1;
      clientStatus = 0;
    }
  }
}

/*********************************************************************
* @purpose  Process CP instance interface update message
*
* @param    L7_uchar8 buf @b{(input)} pointer to start of msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterInstIntfUpdateMsgProcess(L7_uchar8 *buf)
{
  L7_uchar8        *msgInstIntfBuf;
  cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType;
  L7_ushort16        mLen;
  L7_int32           totalIELen;
  L7_BOOL            cpIEDone = L7_TRUE;
  L7_uint32          ieCount = 0;
  L7_enetMacAddr_t   switchMacAddr;
  L7_uint32          cpId = 0, cpUsers = 0;
  L7_uchar8          cpStatus = 0;
  L7_uint32          intf = 0, ifUsers = 0;
  L7_uchar8          ifStatus = 0;


  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad buffer", __FUNCTION__);
    return;
  }

  if (cpdmOprData->cpLocalSw.status != CP_CLUSTER_SWITCH_CONTROLLER)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Not a cluster controller, msg dropped.", __FUNCTION__);
    return;
  }

  if (cpdmOprData->cpGlobalStatus.status != L7_CP_MODE_ENABLED)
  {
    if (cpClusterResendFlagSet(CP_CLUSTER_RESEND_REQ_INST_INTF) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set CP instance interface resend request flag.", __FUNCTION__);
    }
    return;
  }

  msgInstIntfBuf = buf;
  memcpy(&hdr, msgInstIntfBuf, sizeof(cpClusterMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;

  msgInstIntfBuf += CP_CLUSTER_MSG_HDR_SIZE;

  if ((mLen <=0) || ((mLen-CP_CLUSTER_MSG_HDR_SIZE) <= 0))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped.\n", __FUNCTION__, mLen);
    return;
  }

  totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped.", __FUNCTION__);
    return;
  }

  memcpy(&mType, msgInstIntfBuf, CP_CLUSTER_IE_ID_SIZE);
  mType = osapiNtohs(mType);
  msgInstIntfBuf += CP_CLUSTER_IE_ID_SIZE;
  totalIELen -= CP_CLUSTER_IE_ID_SIZE;

  memcpy(&mLen, msgInstIntfBuf, CP_CLUSTER_IE_LEN_SIZE);
  mLen = osapiNtohs(mLen);
  msgInstIntfBuf += CP_CLUSTER_IE_LEN_SIZE;
  totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  if (mType == CP_INST_INTF_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS)
  {
    memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memcpy(switchMacAddr.addr, msgInstIntfBuf, mLen);
    msgInstIntfBuf += mLen;
    totalIELen -= mLen;
    ieCount++;
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg, msg dropped.", __FUNCTION__);
    return;
  }

  if (cpClusterSwitchEntryGet(switchMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg dropped.\n",  __FUNCTION__,
            switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
            switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
    return;
  }

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgInstIntfBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgInstIntfBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgInstIntfBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgInstIntfBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP instance interface update msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_INST_INTF_UPDATE_IE_TYPE_CP_INST:
      if (mLen == CP_INST_INTF_UPDATE_IE_CP_INST_SIZE)
      {
        memcpy(&cpId, msgInstIntfBuf, mLen);
        cpId = osapiNtohl(cpId);
        ieCount++;
      }
      break;
    case CP_INST_INTF_UPDATE_IE_TYPE_CP_INST_STATUS:
      if (mLen == CP_INST_INTF_UPDATE_IE_CP_INST_STATUS_SIZE)
      {
        cpStatus = *msgInstIntfBuf;
        ieCount++;
      }
      break;
    case CP_INST_INTF_UPDATE_IE_TYPE_CP_INST_AUTH_USERS:
      if (mLen == CP_INST_INTF_UPDATE_IE_CP_INST_AUTH_USERS_SIZE)
      {
        memcpy(&cpUsers, msgInstIntfBuf, mLen);
        cpUsers = osapiNtohl(cpUsers);
        ieCount++;
        cpIEDone = L7_FALSE;
      }
      break;
    default: /* Not a CP instance message */
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgInstIntfBuf += mLen;
    }

    while (cpIEDone != L7_TRUE)
    {
      if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
      {
        memcpy(&mType, msgInstIntfBuf, CP_CLUSTER_IE_ID_SIZE);
        mType = osapiNtohs(mType);
        msgInstIntfBuf += CP_CLUSTER_IE_ID_SIZE;
        totalIELen -= CP_CLUSTER_IE_ID_SIZE;

        memcpy(&mLen, msgInstIntfBuf, CP_CLUSTER_IE_LEN_SIZE);
        mLen = osapiNtohs(mLen);
        msgInstIntfBuf += CP_CLUSTER_IE_LEN_SIZE;
        totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
      }
      else
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP instance %d IE failed.\n",  __FUNCTION__, cpId);
        return;
      }

      switch (mType)
      {
      case CP_INST_INTF_UPDATE_IE_TYPE_CP_INTF_INDEX:
        if (mLen == CP_INST_INTF_UPDATE_IE_CP_INTF_INDEX_SIZE)
        {
          memcpy(&intf, msgInstIntfBuf, mLen);
          intf = osapiNtohl(intf);
          ieCount++;
        }
        break;
      case CP_INST_INTF_UPDATE_IE_TYPE_CP_INST_INTF_STATUS:
        if (mLen == CP_INST_INTF_UPDATE_IE_CP_INST_INTF_STATUS_SIZE)
        {
          ifStatus = *msgInstIntfBuf;
          ieCount++;
        }
        break;
      case CP_INST_INTF_UPDATE_IE_TYPE_CP_INST_INTF_AUTH_USERS:
        if (mLen == CP_INST_INTF_UPDATE_IE_CP_INST_INTF_AUTH_USERS_SIZE)
        {
          memcpy(&ifUsers, msgInstIntfBuf, mLen);
          ifUsers = osapiNtohl(ifUsers);
          ieCount++;
        }
        break;
      default:
        CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n",  __FUNCTION__,mType);
        break;
      }

      totalIELen -= mLen;
      if (totalIELen > 0)
      {
        msgInstIntfBuf += mLen;
      }

      if (ieCount == CP_INST_INTF_UPDATE_MSG_TOTAL_IE_COUNT)
      {
        if (cpClusterSwitchEntryParamsUpdate(switchMacAddr, cpId, cpStatus, cpUsers,
                                             intf, ifStatus, ifUsers) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update Peer Switch %02x:%02x:%02x:%02x:%02x:%02x parms",
                  __FUNCTION__,
                  switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
                  switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
          continue;
        }

        ieCount = 1;
        cpId = cpUsers = 0;
        cpStatus = 0;
        intf = ifUsers = 0;
        ifStatus = 0;
        cpIEDone = L7_TRUE;
      }
    }
  }
}

/*********************************************************************
* @purpose  Handle CP statistics update message
*
* @param    L7_uchar8 buf    @b{(input)} pointer to start of msg buffer
* @param    L7_uint32 msgLen @b{(input)} overall msg length
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterStatsUpdateMsgHandle(L7_uchar8 *buf, L7_uint32 msgLen)
{
  cpClusterStatsUpdateMsg_t msg;

  if (pCPStatsQ == L7_NULLPTR)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: CP msg q not created.", __FUNCTION__);
    return;
  }

  memset(&msg, 0, sizeof(cpClusterStatsUpdateMsg_t));
  memcpy(msg.buf, buf, msgLen);
  msg.msgLen = msgLen;

  if (osapiMessageSend(pCPStatsQ, &msg, sizeof(cpClusterStatsUpdateMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: osapiMessageSend failed.", __FUNCTION__);
  }
}

/*********************************************************************
* @purpose  Process CP statistics update message
*
* @param    L7_uchar8 buf    @b{(input)} pointer to start of msg buffer
* @param    L7_uint32 msgLen @b{(input)} overall msg length
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterStatsUpdateMsgProcess(L7_uchar8 *buf, L7_uint32 msgLen)
{
  L7_uchar8          *msgStatsBuf;
  cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType, mLen = 0;
  L7_int32           totalIELen;
  L7_enetMacAddr_t   switchMacAddr, clientMacAddr;
  txRxCount_t        peerStats;

  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad buffer", __FUNCTION__);
    return;
  }
  msgStatsBuf = buf;

  memcpy(&hdr, msgStatsBuf, sizeof(cpClusterMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;

  msgStatsBuf += CP_CLUSTER_MSG_HDR_SIZE;

  if ((mLen - CP_CLUSTER_MSG_HDR_SIZE) <= 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped.",  __FUNCTION__, msgLen);
    return;
  }

  totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped.", __FUNCTION__);
    return;
  }

  if (msgLen < totalIELen)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Actual msg length (%d) less than stated in msg (%d).",
            __FUNCTION__, msgLen, totalIELen);
    return;
  }

  memcpy(&mType, msgStatsBuf, CP_CLUSTER_IE_ID_SIZE);
  mType = osapiNtohs(mType);
  msgStatsBuf += CP_CLUSTER_IE_ID_SIZE;
  totalIELen -= CP_CLUSTER_IE_ID_SIZE;

  memcpy(&mLen, msgStatsBuf, CP_CLUSTER_IE_LEN_SIZE);
  mLen = osapiNtohs(mLen);
  msgStatsBuf += CP_CLUSTER_IE_LEN_SIZE;
  totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  if (mType != CP_STATISTICS_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg: expected type %d, got %d. Dropped.",
            __FUNCTION__, CP_STATISTICS_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS, mType);
    return;
  }

  memcpy(switchMacAddr.addr, msgStatsBuf, sizeof(switchMacAddr.addr));
  if (cpClusterSwitchEntryGet(switchMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg dropped",
            __FUNCTION__,
            switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
            switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
    return;
  }
  msgStatsBuf += mLen;
  totalIELen -= mLen;


  while(totalIELen >= CONN_CLIENT_STATS_SIZE)
  {
     /* Expect client mac, then statistics, in pairs, from here on.
      * Choke on anything else.
      */

     memcpy(&mType, msgStatsBuf, CP_CLUSTER_IE_ID_SIZE);
     mType = osapiNtohs(mType);
     msgStatsBuf += CP_CLUSTER_IE_ID_SIZE;
     totalIELen -= CP_CLUSTER_IE_ID_SIZE;

     memcpy(&mLen, msgStatsBuf, CP_CLUSTER_IE_LEN_SIZE);
     mLen = osapiNtohs(mLen);
     msgStatsBuf += CP_CLUSTER_IE_LEN_SIZE;
     totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

     if ((CP_STATISTICS_UPDATE_IE_TYPE_CLIENT_MAC_ADDRESS != mType) ||
         (CP_STATISTICS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE != mLen))
     {
       CP_DLOG(CPD_LEVEL_LOG, "Malformed IE: expected type %d, length %d: got %d, %d instead",
              CP_STATISTICS_UPDATE_IE_TYPE_CLIENT_MAC_ADDRESS,
              CP_STATISTICS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE, mType, mLen);
       return;
     }

     memcpy(clientMacAddr.addr, msgStatsBuf, sizeof(clientMacAddr.addr));
     msgStatsBuf += CP_STATISTICS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE;
     totalIELen  -= CP_STATISTICS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE;

     /* Now get the stats */
     memcpy(&mType, msgStatsBuf, CP_CLUSTER_IE_ID_SIZE);
     mType = osapiNtohs(mType);
     msgStatsBuf += CP_CLUSTER_IE_ID_SIZE;
     totalIELen -= CP_CLUSTER_IE_ID_SIZE;

     memcpy(&mLen, msgStatsBuf, CP_CLUSTER_IE_LEN_SIZE);
     mLen = osapiNtohs(mLen);
     msgStatsBuf += CP_CLUSTER_IE_LEN_SIZE;
     totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

     if ((CP_STATISTICS_UPDATE_IE_TYPE_STATS_DATA != mType) ||
         (CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE != mLen))
     {
       CP_DLOG(CPD_LEVEL_LOG, "Malformed IE: expected type %d, length %d: got %d, %d instead",
               CP_STATISTICS_UPDATE_IE_TYPE_STATS_DATA,
               CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE, mType, mLen);
       return;
     }
     memset(&peerStats, 0, sizeof(txRxCount_t));

     {
       /* just in case someone pulls a fast one, and expands our statistics
        * structure unwittingly, we'll use a temporary pointer here.  This would
        * still be a bug, but we'll at least process more data this way.
        */

       L7_uchar8 * tmp = msgStatsBuf;

       memcpy(&peerStats.bytesTransmitted, tmp, CP_CLUSTER_IE_COUNTER64_SIZE);
       peerStats.bytesTransmitted = osapiNtohll(peerStats.bytesTransmitted);
       tmp += CP_CLUSTER_IE_COUNTER64_SIZE;

       memcpy(&peerStats.bytesReceived, tmp, CP_CLUSTER_IE_COUNTER64_SIZE);
       peerStats.bytesReceived = osapiNtohll(peerStats.bytesReceived);
       tmp += CP_CLUSTER_IE_COUNTER64_SIZE;

       memcpy(&peerStats.packetsTransmitted, tmp, CP_CLUSTER_IE_COUNTER64_SIZE);
       peerStats.packetsTransmitted = osapiNtohll(peerStats.packetsTransmitted);
       tmp += CP_CLUSTER_IE_COUNTER64_SIZE;

       memcpy(&peerStats.packetsReceived, tmp, CP_CLUSTER_IE_COUNTER64_SIZE);
       peerStats.packetsReceived = osapiNtohll(peerStats.packetsReceived);
       tmp += CP_CLUSTER_IE_COUNTER64_SIZE;
     }
     totalIELen  -= CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE;
     msgStatsBuf += CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE;

     /* Okay, process the newly parsed pair */
     if (cpdmClientConnStatusPeerStatsUpdate(clientMacAddr, &peerStats) != L7_SUCCESS)
     {
        CP_DLOG(CPD_LEVEL_LOG, "Failed to update peer switch stats.");
     }
  }
}

/*********************************************************************
* @purpose  Process CP config sync message
*
* @param    L7_uchar8 buf @b{(input)} pointer to start of msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterConfigSyncMsgProcess(L7_uchar8 *buf)
{
  L7_uchar8        *msgCfgSyncBuf;
  cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType, mLen;
  L7_int32           totalIELen;

  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad buffer", __FUNCTION__);
    return;
  }

  msgCfgSyncBuf = buf;
  memcpy(&hdr, msgCfgSyncBuf, sizeof(cpClusterMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;

  msgCfgSyncBuf += CP_CLUSTER_MSG_HDR_SIZE;

  if ((mLen <=0) || ((mLen-CP_CLUSTER_MSG_HDR_SIZE) <= 0))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped",  __FUNCTION__, mLen);
    return;
  }

  totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped", __FUNCTION__);
    return;
  }

  memcpy(&mType, msgCfgSyncBuf, CP_CLUSTER_IE_ID_SIZE);
  mType = osapiNtohs(mType);
  msgCfgSyncBuf += CP_CLUSTER_IE_ID_SIZE;
  totalIELen -= CP_CLUSTER_IE_ID_SIZE;

  memcpy(&mLen, msgCfgSyncBuf, CP_CLUSTER_IE_LEN_SIZE);
  mLen = osapiNtohs(mLen);
  msgCfgSyncBuf += CP_CLUSTER_IE_LEN_SIZE;
  totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  switch (mType)
  {
  case CP_CLUSTER_CONFIG_MSG_IE_GLOBAL_TYPE:
    cpConfigMsgGlobalProcess(msgCfgSyncBuf, totalIELen);
    CP_DLOG(CPD_LEVEL_LOG, "Processed CP Globals sync msg, IE type: %d", mType);
    break;
  case CP_CLUSTER_CONFIG_MSG_IE_CP_TYPE:
    cpConfigMsgCPProcess(msgCfgSyncBuf, totalIELen);
    CP_DLOG(CPD_LEVEL_LOG, "Processed CP Instances sync msg, IE type: %d",  mType);
    break;
  case CP_CLUSTER_CONFIG_MSG_IE_GROUP_TYPE:
    cpConfigMsgGroupProcess(msgCfgSyncBuf, totalIELen);
    CP_DLOG(CPD_LEVEL_LOG, "Processed CP Groups sync msg, IE type: %d", mType);
    break;
  case CP_CLUSTER_CONFIG_MSG_IE_USER_TYPE:
    cpConfigMsgUserProcess(msgCfgSyncBuf, totalIELen);
    CP_DLOG(CPD_LEVEL_LOG, "Processed CP Local Users sync msg, IE type: %d", mType);
    break;
  default:
    CP_DLOG(CPD_LEVEL_LOG, "%s:Unknown Sync msg IE type: %d", __FUNCTION__, mType);
    break;
  }
}

/*********************************************************************
* @purpose  Process CP resend message
*
* @param    L7_uchar8 buf @b{(input)} pointer to start of msg buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterResendMsgProcess(L7_uchar8 *buf)
{
  L7_uchar8          *msgResendBuf, type = 0;
  cpClusterMsgHdr_t  hdr;
  L7_ushort16        mType, mLen;
  L7_int32           totalIELen;
  L7_uint32          ieCount = 0;
  L7_enetMacAddr_t   tmpMacAddr, switchMacAddr;

  if (L7_NULLPTR == buf)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad buffer", __FUNCTION__);
    return;
  }

  if (cpdmOprData->cpGlobalStatus.status != L7_CP_MODE_ENABLED)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: CP disabled, msg dropped.", __FUNCTION__);
    return;
  }

  memset(&tmpMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
  msgResendBuf = buf;
  memcpy(&hdr, msgResendBuf, sizeof(cpClusterMsgHdr_t));
  mType = hdr.msgType;
  mLen = hdr.msgLen;

  msgResendBuf += CP_CLUSTER_MSG_HDR_SIZE;

  if ((mLen <=0) || ((mLen-CP_CLUSTER_MSG_HDR_SIZE) <= 0))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg length %d, msg dropped.\n",  __FUNCTION__, mLen);
    return;
  }

  totalIELen = mLen - CP_CLUSTER_MSG_HDR_SIZE;
  if (totalIELen == 0)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No IEs in msg, msg dropped.", __FUNCTION__);
    return;
  }

  memcpy(&mType, msgResendBuf, CP_CLUSTER_IE_ID_SIZE);
  mType = osapiNtohs(mType);
  msgResendBuf += CP_CLUSTER_IE_ID_SIZE;
  totalIELen -= CP_CLUSTER_IE_ID_SIZE;

  memcpy(&mLen, msgResendBuf, CP_CLUSTER_IE_LEN_SIZE);
  mLen = osapiNtohs(mLen);
  msgResendBuf += CP_CLUSTER_IE_LEN_SIZE;
  totalIELen -= CP_CLUSTER_IE_LEN_SIZE;

  if (mType == CP_RESEND_IE_TYPE_SWITCH_MAC_ADDRESS)
  {
    memset(&switchMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
    memcpy(switchMacAddr.addr, msgResendBuf, mLen);
    msgResendBuf += mLen;
    totalIELen -= mLen;
    ieCount++;
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid msg, msg dropped.", __FUNCTION__);
    return;
  }

  if (cpClusterSwitchEntryGet(switchMacAddr) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Switch %02x:%02x:%02x:%02x:%02x:%02x not in cluster, msg dropped.\n",
            __FUNCTION__,
            switchMacAddr.addr[0],switchMacAddr.addr[1],switchMacAddr.addr[2],
            switchMacAddr.addr[3],switchMacAddr.addr[4],switchMacAddr.addr[5]);
    return;
  }

  while (totalIELen > 0)
  {
    if (totalIELen >= CP_CLUSTER_IE_HDR_SIZE)
    {
      memcpy(&mType, msgResendBuf, CP_CLUSTER_IE_ID_SIZE);
      mType = osapiNtohs(mType);
      msgResendBuf += CP_CLUSTER_IE_ID_SIZE;
      totalIELen -= CP_CLUSTER_IE_ID_SIZE;

      memcpy(&mLen, msgResendBuf, CP_CLUSTER_IE_LEN_SIZE);
      mLen = osapiNtohs(mLen);
      msgResendBuf += CP_CLUSTER_IE_LEN_SIZE;
      totalIELen -= CP_CLUSTER_IE_LEN_SIZE;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Parsing CP resend msg failed.", __FUNCTION__);
      return;
    }

    switch (mType)
    {
    case CP_RESEND_IE_TYPE_MSG_TYPE:
      if (mLen == CP_RESEND_IE_MSG_TYPE_SIZE)
      {
        type = *msgResendBuf;
        ieCount++;
      }
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown msg IE type:%d.\n", __FUNCTION__, mType);
      break;
    }

    totalIELen -= mLen;
    if (totalIELen > 0)
    {
      msgResendBuf += mLen;
    }

    if (ieCount == CP_RESEND_MSG_TOTAL_IE_COUNT)
    {
      if (type == CP_CLUSTER_RESEND_CLIENT_NOTIFICATION)
      {
        memcpy(&cpdmOprData->cpNewPeer, &switchMacAddr, sizeof(L7_enetMacAddr_t));
        if (cpClusterResendFlagSet(CP_CLUSTER_RESEND_CLIENT_NOTIFY) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set client notification resend flag.", __FUNCTION__);
          memset(&cpdmOprData->cpNewPeer, 0, sizeof(L7_enetMacAddr_t));
          return;
        }
      }
      else if (type == CP_CLUSTER_RESEND_CONN_CLIENTS_UPDATE)
      {
        if (cpClusterResendFlagSet(CP_CLUSTER_RESEND_CONN_CLIENTS) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set connected clients resend flag.", __FUNCTION__);
          return;
        }
      }
      else
      {
        if (cpClusterResendFlagSet(CP_CLUSTER_RESEND_INST_INTF) != L7_SUCCESS)
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set instance interface resend flag.", __FUNCTION__);
          return;
        }
      }

      ieCount = 1;
      type = 0;
    }
  }
}

/*********************************************************************
* @purpose  Get CP configuration push status
*
* @param    L7_uint32 status @b{(output)} CP config push status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterConfigPushStatusGet(L7_uint32 *status)
{
  if (status == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

#ifdef L7_WIRELESS_PACKAGE
  if (wdmWSConfigPushGet(CLUSTER_CFG_ID(CAPTIVE_PORTAL), status) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: %s CP config push status.", __FUNCTION__, failed_to_get);
    return L7_FAILURE;
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check all configuration parameters are received successfully
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterConfigRxCheck()
{
  if ((numCP > FD_CP_CONFIG_MAX) ||
      (numGroup > FD_CP_USER_GROUP_MAX) ||
      (numUser > FD_CP_LOCAL_USERS_MAX))
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply CP config received from peer switch
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterConfigRxApply()
{
  L7_uint32 i;

  /* Init CP configurations before we start */
  cpdmUserEntryPurge();
  if (L7_FAILURE == cpdmCPConfigInit(CP_ID_MAX))
  {
    LOG_ERROR(0);
  }
  if (L7_FAILURE == cpdmUserGroupEntryInit())
  {
    LOG_ERROR(0);
  }
  if (L7_FAILURE == cpdmUserGroupAssocEntryInit())
  {
    LOG_ERROR(0);
  }
  if (L7_FAILURE == cpIntfAssocInit(CP_INTERFACE_MAX))
  {
    LOG_ERROR(0); /* suicide is the answer */
  }

  cpdmCfgData->cpGlobalData.cpMode = ptrCpGlobalData->cpMode;
  cpdmCfgData->cpGlobalData.httpPort = ptrCpGlobalData->httpPort;
  cpdmCfgData->cpGlobalData.httpsPort1 = ptrCpGlobalData->httpsPort1;
  cpdmCfgData->cpGlobalData.httpsPort2 = ptrCpGlobalData->httpsPort2;
  cpdmCfgData->cpGlobalData.sessionTimeout = ptrCpGlobalData->sessionTimeout;
  cpdmCfgData->cpGlobalData.peerSwitchStatsReportInterval = ptrCpGlobalData->peerSwitchStatsReportInterval;
  cpdmCfgData->cpGlobalData.trapFlags = ptrCpGlobalData->trapFlags;

  for (i=0; i<numCP; i++)
  {
    cpdmCfgData->cpConfigData[i].cpId = ptrCpConfigData[i].cpId;

    osapiStrncpy(cpdmCfgData->cpConfigData[i].name, ptrCpConfigData[i].name, CP_NAME_MAX);
    cpdmCfgData->cpConfigData[i].name[CP_NAME_MAX] = '\0';

    cpdmCfgData->cpConfigData[i].enabled = ptrCpConfigData[i].enabled;
    cpdmCfgData->cpConfigData[i].protocolMode = ptrCpConfigData[i].protocolMode;
    cpdmCfgData->cpConfigData[i].verifyMode = ptrCpConfigData[i].verifyMode;
    cpdmCfgData->cpConfigData[i].redirectMode = ptrCpConfigData[i].redirectMode;

    osapiStrncpy(cpdmCfgData->cpConfigData[i].redirectURL, ptrCpConfigData[i].redirectURL, CP_WELCOME_URL_MAX);
    cpdmCfgData->cpConfigData[i].redirectURL[CP_WELCOME_URL_MAX] = '\0';

    osapiStrncpy(cpdmCfgData->cpConfigData[i].radiusAuthServer, ptrCpConfigData[i].radiusAuthServer, CP_RADIUS_AUTH_SERVER_MAX);
    cpdmCfgData->cpConfigData[i].radiusAuthServer[CP_RADIUS_AUTH_SERVER_MAX] = '\0';

    cpdmCfgData->cpConfigData[i].gpId = ptrCpConfigData[i].gpId;
    cpdmCfgData->cpConfigData[i].sessionTimeout = ptrCpConfigData[i].sessionTimeout;
    cpdmCfgData->cpConfigData[i].idleTimeout = ptrCpConfigData[i].idleTimeout;
    cpdmCfgData->cpConfigData[i].userUpRate = ptrCpConfigData[i].userUpRate;
    cpdmCfgData->cpConfigData[i].userDownRate = ptrCpConfigData[i].userDownRate;
    cpdmCfgData->cpConfigData[i].maxInputOctets = ptrCpConfigData[i].maxInputOctets;
    cpdmCfgData->cpConfigData[i].maxOutputOctets = ptrCpConfigData[i].maxOutputOctets;
    cpdmCfgData->cpConfigData[i].maxTotalOctets = ptrCpConfigData[i].maxTotalOctets;
    cpdmCfgData->cpConfigData[i].localeCount = ptrCpConfigData[i].localeCount;

    osapiStrncpy(cpdmCfgData->cpConfigData[i].foregroundColor, ptrCpConfigData[i].foregroundColor, CP_FOREGROUND_COLOR_MAX);
    cpdmCfgData->cpConfigData[i].foregroundColor[CP_FOREGROUND_COLOR_MAX] = '\0';
    osapiStrncpy(cpdmCfgData->cpConfigData[i].backgroundColor, ptrCpConfigData[i].backgroundColor, CP_BACKGROUND_COLOR_MAX);
    cpdmCfgData->cpConfigData[i].backgroundColor[CP_BACKGROUND_COLOR_MAX] = '\0';
    osapiStrncpy(cpdmCfgData->cpConfigData[i].separatorColor, ptrCpConfigData[i].separatorColor, CP_SEPARATOR_COLOR_MAX);
    cpdmCfgData->cpConfigData[i].separatorColor[CP_SEPARATOR_COLOR_MAX] = '\0';
  }

  for (i=0; i<webIndex; i++)
  {
    cpdmCfgData->cpLocaleData[i].key.cpId = ptrCpWebID[i].cpId;
    cpdmCfgData->cpLocaleData[i].key.webId = ptrCpWebID[i].webId;

    osapiStrncpy(cpdmCfgData->cpLocaleData[i].langCode, ptrCpCodeLink[i].code, CP_LANG_CODE_MAX);
    cpdmCfgData->cpLocaleData[i].langCode[CP_LANG_CODE_MAX] = '\0';

    osapiStrncpy(cpdmCfgData->cpLocaleData[i].localeLink, ptrCpCodeLink[i].link, CP_LOCALE_LINK_MAX);
    cpdmCfgData->cpLocaleData[i].localeLink[CP_LOCALE_LINK_MAX] = '\0';
  }

  for (i=0; i<ifIndex; i++)
  {
    cpdmCfgData->cpInterfaceAssocData[i].cpId = ptrCpInterfaceAssocData[i].cpId;
    cpdmCfgData->cpInterfaceAssocData[i].intIfNum = ptrCpInterfaceAssocData[i].intIfNum;
  }

  for (i=0; i<numGroup; i++)
  {
    cpdmCfgData->cpUserGroupData[i].gpId = ptrCpUserGroupData[i].gpId;

    osapiStrncpy(cpdmCfgData->cpUserGroupData[i].groupName,
                 ptrCpUserGroupData[i].groupName, CP_USER_LOCAL_USERGROUP_MAX);
    cpdmCfgData->cpUserGroupData[i].groupName[CP_USER_LOCAL_USERGROUP_MAX] = '\0';
  }

  for (i=0; i<numUser; i++)
  {
    cpdmCfgData->cpUsersData.cpUsers[i].uId = ptrCpUsers[i].uId;

    osapiStrncpy(cpdmCfgData->cpUsersData.cpUsers[i].loginName,
                 ptrCpUsers[i].loginName, CP_USER_LOCAL_USERNAME_MAX);
    cpdmCfgData->cpUsersData.cpUsers[i].loginName[CP_USER_LOCAL_USERNAME_MAX] = '\0';

    osapiStrncpy(cpdmCfgData->cpUsersData.cpUsers[i].password,
                 ptrCpUsers[i].password, L7_ENCRYPTED_PASSWORD_SIZE-1);
    cpdmCfgData->cpUsersData.cpUsers[i].password[L7_ENCRYPTED_PASSWORD_SIZE-1] = '\0';

    cpdmCfgData->cpUsersData.cpUsers[i].sessionTimeout = ptrCpUsers[i].sessionTimeout;
    cpdmCfgData->cpUsersData.cpUsers[i].idleTimeout = ptrCpUsers[i].idleTimeout;
    cpdmCfgData->cpUsersData.cpUsers[i].maxBandwidthUp = ptrCpUsers[i].maxBandwidthUp;
    cpdmCfgData->cpUsersData.cpUsers[i].maxBandwidthDown = ptrCpUsers[i].maxBandwidthDown;
    cpdmCfgData->cpUsersData.cpUsers[i].maxInputOctets = ptrCpUsers[i].maxInputOctets;
    cpdmCfgData->cpUsersData.cpUsers[i].maxOutputOctets = ptrCpUsers[i].maxOutputOctets;
    cpdmCfgData->cpUsersData.cpUsers[i].maxTotalOctets = ptrCpUsers[i].maxTotalOctets;
  }

  for (i=0; i<ugIndex; i++)
  {
    cpdmCfgData->cpUserGroupAssocData[i].uId = ptrCpUserGroupAssocData[i].uId;
    cpdmCfgData->cpUserGroupAssocData[i].gpId = ptrCpUserGroupAssocData[i].gpId;
  }

  SEMA_TAKE(WRITE);
  cpdmCfgData->hdr.dataChanged = L7_TRUE;
  SEMA_GIVE(WRITE);

  cpConfigApplyUpdate();
}

/*********************************************************************
* @purpose  Cluster callback to send CP config message
*
* @param    clusterMemberID_t *memberID @b{(input)} pointer to switch MAC
*
* @returns  void
*
* @comments This callback passes the switch MAC address to which the
*           configuration needs to be sent.
*
* @end
*********************************************************************/
void cpClusterConfigSendCallback(clusterMemberID_t *memberID)
{
  L7_enetMacAddr_t macAddr;

  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr, memberID, sizeof(L7_enetMacAddr_t));
  memcpy(&cpdmOprData->cpCfgPeerTx, &macAddr, sizeof(L7_enetMacAddr_t));

  if (cpClusterFlagSet(CP_CLUSTER_CONFIG_SEND) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set CP config send flag.", __FUNCTION__);
    memset(&cpdmOprData->cpCfgPeerTx, 0, sizeof(L7_enetMacAddr_t));
  }
}

/*********************************************************************
* @purpose  Cluster callback to receive CP config message
*
* @param    clusterConfigCmd cmd @b{(input)} config command
*
* @returns  void
*
* @comments This function notifies CP that remote switch has initiated
*           config transfer.
*
* @end
*********************************************************************/
void cpClusterConfigRxCallback(clusterConfigCmd cmd)
{
  cpdmOprData->cpConfigCmd.cmd = cmd;
  if (cpClusterFlagSet(CP_CLUSTER_CONFIG_RX) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to set CP config received flag.", __FUNCTION__);
  }
  captivePortalTaskWake();
}

/*********************************************************************
* @purpose  Process CP config sync command
*
* @param    clusterConfigCmd cmd @b{(input)} config command
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpClusterConfigSyncCmdProcess(clusterConfigCmd cmd)
{
  L7_RC_t rc = L7_FAILURE;

  switch (cmd)
  {
  case CLUSTER_CONFIG_CMD_START_TRANSFER:
  case CLUSTER_CONFIG_CMD_ABORT:
    cpClusterMsgError = L7_FALSE;
    memset(ptrCpGlobalData, 0, sizeof(cpdmGlobal_t));
    memset(ptrCpConfigData, 0, sizeof(cpdmConfigData_t)*FD_CP_CONFIG_MAX);
    numCP = 0;
    memset(ptrCpWebID, 0, sizeof(cpLocaleKey_t)*FD_CP_CONFIG_MAX*FD_CP_CUSTOM_LOCALE_MAX);
    memset(ptrCpCodeLink, 0, sizeof(cpLocaleCodeLink_t)*FD_CP_CONFIG_MAX*FD_CP_CUSTOM_LOCALE_MAX);
    memset(ptrCpInterfaceAssocData, 0, sizeof(ifNumCPPair_t)*FD_CP_CONFIG_MAX*CP_INTERFACE_MAX);
    memset(ptrCpUserGroupData, 0, sizeof(cpdmUserGroupData_t)*FD_CP_USER_GROUP_MAX);
    numGroup = 0;
    memset(ptrCpUsers, 0, sizeof(cpdmUser_t)*FD_CP_LOCAL_USERS_MAX);
    numUser = 0;
    memset(ptrCpUserGroupAssocData, 0, sizeof(cpdmUserGroupAssocData_t)*FD_CP_USER_GROUP_ASSOC_MAX);
    webIndex = 0;
    ifIndex = 0;
    ugIndex = 0;
    break;
  case CLUSTER_CONFIG_CMD_END_TRANSFER:
    if (cpClusterMsgError == L7_TRUE)
    {
      rc = L7_FAILURE;
    }
    else
    {
      rc = cpClusterConfigRxCheck();
    }

    if (clusterConfigRxDone(CLUSTER_CFG_ID(CAPTIVE_PORTAL), rc) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: clusterConfigRxDone() failed.", __FUNCTION__);
    }
    break;
  case CLUSTER_CONFIG_CMD_APPLY:
    cpClusterConfigRxApply();
    clusterConfigRxApplyDone(CLUSTER_CFG_ID(CAPTIVE_PORTAL));
    break;
  default:
    CP_DLOG(CPD_LEVEL_LOG, "%s: Unknown cmd %d received.\n", __FUNCTION__, cmd);
    break;
  }
}
