/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2008
*
**********************************************************************
*
* @filename     cpdm_clustering_api.c
*
* @purpose      Captive Portal Data Manager (CPDM) Clustering API functions
*
* @component    CPDM
*
* @comments     APIs for usmDb layer. These APIs can be called via usmDb
*               and by the captive portal components.
*
* @create       01/16/2008
*
* @author       rjindal
*
* @end
*
*********************************************************************/

#include "clustering_commdefs.h"
#include "cpdm.h"
#include "cpdm_connstatus_api.h"
#include "clustering_api.h"
#include "cpdm_clustering_api.h"
#include "cp_cluster.h"

#define SEMA_TAKE(access)   \
  if (CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);
#define SEMA_GIVE(access)   \
  if (CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);

extern avlTree_t  peerSwClientTree;
extern avlTree_t  peerSwTree;

/*********************************************************************
* @purpose  Add or update a peer switch status entry
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} switch MAC address
* @param    CP_CLUSTER_SWITCH_STATUS_t status  @b{(input)} switch status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments This function adds a new entry or updates an existing entry.
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchEntryAddUpdate(L7_enetMacAddr_t macAddr, CP_CLUSTER_SWITCH_STATUS_t status)
{
  cpdmPeerSwStatus_t data, *ptrData = L7_NULLPTR;
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(WRITE);
  ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_EXACT);
  if (ptrData == L7_NULLPTR)
  {
    /* add entry */
    if (peerSwTree.count == (clusterMaxMembersGet()-1)) /* excluding local switch */
    {
      SEMA_GIVE(WRITE);
      return L7_TABLE_IS_FULL;
    }

    memset(&data, 0x00, sizeof(data));
    memcpy(&data.macAddr, &macAddr, sizeof(L7_enetMacAddr_t));
    data.status = status;

    for (i=0; i<CP_ID_MAX; i++)
    {
      data.cp[i].status = CP_CLUSTER_CP_DISABLED;
      data.cp[i].users = 0;
    }

    for (i=0; i<CP_INTERFACE_MAX /*64*/; i++)
    {
      data.cpIf[i].status = CP_CLUSTER_INTF_DISABLED;
      data.cpIf[i].users = 0;
    }

    ptrData = avlInsertEntry(&peerSwTree, &data);
    if (ptrData == L7_NULLPTR)
    {
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  else
  {
    /* update entry */
    ptrData->status = status;
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Update peer switch entry parameters
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} switch MAC address
* @param    L7_uint32        cpId     @b{(input)} CP instance ID
* @param    L7_uchar8        cpStatus @b{(input)} CP status
* @param    L7_uint32        cpUsers  @b{(input)} number of CP users
* @param    L7_uint32        intf     @b{(input)} CP interface
* @param    L7_uchar8        ifStatus @b{(input)} interface status
* @param    L7_uint32        ifUsers  @b{(input)} number of interface users
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchEntryParamsUpdate(L7_enetMacAddr_t macAddr,
                                         L7_uint32 cpId, L7_uchar8 cpStatus, L7_uint32 cpUsers,
                                         L7_uint32 intf, L7_uchar8 ifStatus, L7_uint32 ifUsers)
{
  cpdmPeerSwStatus_t *ptrData = L7_NULLPTR;
  L7_uint32 firstIntf;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(WRITE);
  ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    ptrData->cp[cpId-1].status = cpStatus;
    ptrData->cp[cpId-1].users = cpUsers;

    /* get the first wireless intf */
    if (nimFirstValidIntfNumberByType(L7_WIRELESS_INTF, &firstIntf) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "Failed to get first wireless interface.\n");
    }

    ptrData->cpIf[intf-firstIntf].status = ifStatus;
    ptrData->cpIf[intf-firstIntf].users = ifUsers;

    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Remove a peer switch status entry
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} switch MAC address
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchEntryDelete(L7_enetMacAddr_t macAddr)
{
  cpdmPeerSwStatus_t *ptrData = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(WRITE);
  ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    avlDeleteEntry(&peerSwTree, ptrData);
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Check if a peer switch status entry exists
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchEntryGet(L7_enetMacAddr_t macAddr)
{
  cpdmPeerSwStatus_t *ptrData = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(READ);
  ptrData = avlSearchLVL7(&peerSwTree, &macAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
* @purpose  Purge all peer switch status entries
*
* @param    void
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments This function is called when the local switch leaves the 
*           cluster. UI should never call this function.
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchEntriesPurge(void)
{
  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(WRITE);
  avlPurgeAvlTree(&peerSwTree, (clusterMaxMembersGet()-1));
  SEMA_GIVE(WRITE);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add or update a peer switch client status entry
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    L7_uint32        ipAddr  @b{(input)} client IP address
* @param    L7_ushort16      cpId    @b{(input)} client cpId
* @param    L7_enetMacAddr_t swMac   @b{(input)} switch MAC address
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments This function adds a new entry or updates an existing entry's  
*           switch MAC address to indicate new authenticator of client.
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchClientEntryAddUpdate(L7_enetMacAddr_t macAddr,
                                            L7_uint32 ipAddr, L7_ushort16 cpId,
                                            L7_enetMacAddr_t swMac)
{
  cpdmPeerSwClientStatus_t data, *ptrData = L7_NULLPTR;
  clusterMemberID_t memberID;
  L7_IP_ADDR_t swIP;
  L7_in6_addr_t ipV6;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  memcpy(&memberID, &swMac, sizeof(L7_enetMacAddr_t));
  if (clusterMemberIdentityGet(&memberID, &swIP, &ipV6) != L7_SUCCESS)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to get switch %02x:%02x:%02x:%02x:%02x:%02x IP address.\n",
	    __FUNCTION__,
            swMac.addr[0],swMac.addr[1],swMac.addr[2],
            swMac.addr[3],swMac.addr[4],swMac.addr[5]);
    return rc;
  }

  memset(&data, 0x00, sizeof(cpdmPeerSwClientStatus_t));
  memcpy(&data.macAddr, &macAddr, sizeof(L7_enetMacAddr_t));
  data.ip = ipAddr;
  data.cpId = cpId;
  memcpy(&data.swMac, &swMac, sizeof(L7_enetMacAddr_t));
  data.swIP = swIP;

  SEMA_TAKE(WRITE);
  ptrData = avlInsertEntry(&peerSwClientTree, &data);
  if (ptrData == L7_NULLPTR)
  {
    /* added entry */
    CP_DLOG(CPD_LEVEL_LOG, "Added peer switch client %02x:%02x:%02x:%02x:%02x:%02x.\n",
	    macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],
	    macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);
  }
  else /* updating entry */
  {
    memcpy(&ptrData->swMac, &swMac, sizeof(L7_enetMacAddr_t));
    ptrData->swIP = swIP;
  }

  /* let's make sure the add/update happened */
  if (L7_NULLPTR != avlSearchLVL7(&peerSwClientTree, &macAddr, AVL_EXACT))
  {
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
* @purpose  Remove a peer switch client status entry
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)} client MAC address
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchClientEntryDelete(L7_enetMacAddr_t macAddr,
                                         L7_enetMacAddr_t swMacAddr)
{
  cpdmPeerSwClientStatus_t *ptrData = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(WRITE);
  ptrData = avlSearchLVL7(&peerSwClientTree, &macAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    if (memcmp(&ptrData->swMac, &swMacAddr, sizeof(L7_enetMacAddr_t)) == 0)
    {
      avlDeleteEntry(&peerSwClientTree, ptrData);
    }
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Remove all clients for a specified peer switch
*
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchClientPeerClientsDelete(L7_enetMacAddr_t swMacAddr)
{
  cpdmPeerSwClientStatus_t *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t macAddr;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(WRITE);
  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  while ((ptrData=avlSearchLVL7(&peerSwClientTree, &macAddr, AVL_NEXT)) != L7_NULLPTR)
  {
    memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
    if (memcmp(&ptrData->swMac, &swMacAddr, sizeof(L7_enetMacAddr_t)) == 0)
    {
      avlDeleteEntry(&peerSwClientTree, ptrData);
    }
  }
  SEMA_GIVE(WRITE);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if a peer switch client status entry exists
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchClientEntryGet(L7_enetMacAddr_t macAddr)
{
  cpdmPeerSwClientStatus_t *ptrData = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(READ);
  ptrData = avlSearchLVL7(&peerSwClientTree, &macAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
* @purpose  Get the CP ID of the client
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    cpId_t           cpId    @b{(output)} pointer to cpId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchClientCPIDGet(L7_enetMacAddr_t macAddr, cpId_t *cpId)
{
  cpdmPeerSwClientStatus_t *ptrData = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(READ);
  ptrData = avlSearchLVL7(&peerSwClientTree, &macAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    *cpId = ptrData->cpId;
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
* @purpose  Get the MAC address of the client authenticator switch
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    L7_enetMacAddr_t swMac   @b{(output)} pointer to switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchClientSwitchMACGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *swMac)
{
  cpdmPeerSwClientStatus_t *ptrData = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(READ);
  ptrData = avlSearchLVL7(&peerSwClientTree, &macAddr, AVL_EXACT);
  if (ptrData != L7_NULLPTR)
  {
    memcpy(swMac, &ptrData->swMac, sizeof(L7_enetMacAddr_t));
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
* @purpose  Get MAC address of peer switch client identified by IP address
*
* @param    L7_IP_ADDR_t     ipAddr  @b{(input)} client IP address
* @param    L7_enetMacAddr_t macAddr @b{(output)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchClientEntryByIPGet(L7_IP_ADDR_t ipAddr, L7_enetMacAddr_t *clientMacAddr)
{
  cpdmPeerSwClientStatus_t *ptrData = L7_NULLPTR;
  L7_enetMacAddr_t macAddr;
  L7_RC_t rc = L7_FAILURE;

  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(READ);
  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  while ((ptrData=avlSearchLVL7(&peerSwClientTree, &macAddr, AVL_NEXT)) != L7_NULLPTR)
  {
    if (ptrData->ip == ipAddr)
    {
      memcpy(clientMacAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
      rc = L7_SUCCESS;
      break;
    }
    else
    {
      memcpy(&macAddr, &ptrData->macAddr, sizeof(L7_enetMacAddr_t));
    }
  }
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
* @purpose  Purge all peer switch client status entries
*
* @param    void
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments This function is called when the local switch leaves the 
*           cluster. UI should never call this function.
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchClientEntriesPurge(void)
{
  if (clusterSupportGet() != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  SEMA_TAKE(WRITE);
  avlPurgeAvlTree(&peerSwClientTree, CP_CLIENT_CONN_STATUS_MAX*2);
  SEMA_GIVE(WRITE);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set pending flag for CP clustering
*
* @param    CP_CLUSTER_FLAG_t flag @b{(input)} type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterFlagSet(CP_CLUSTER_FLAG_t flag)
{
  cpdmOprData->cpClusterFlag |= flag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get pending flag for CP clustering
*
* @param    CP_CLUSTER_FLAG_t flag @b{(input)} type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterFlagGet(CP_CLUSTER_FLAG_t *flag)
{
  *flag = cpdmOprData->cpClusterFlag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear pending flag for CP clustering
*
* @param    CP_CLUSTER_FLAG_t flag @b{(input)} type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterFlagClear(CP_CLUSTER_FLAG_t flag)
{
  cpdmOprData->cpClusterFlag &= ~flag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set pending resend flag for CP clustering
*
* @param    CP_CLUSTER_RESEND_FLAG_t flag @b{(input)} type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterResendFlagSet(CP_CLUSTER_RESEND_FLAG_t flag)
{
  cpdmOprData->cpClusterResendFlag |= flag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get pending resend flag for CP clustering
*
* @param    CP_CLUSTER_RESEND_FLAG_t flag @b{(input)} type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterResendFlagGet(CP_CLUSTER_RESEND_FLAG_t *flag)
{
  *flag = cpdmOprData->cpClusterResendFlag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear pending resend flag for CP clustering
*
* @param    CP_CLUSTER_RESEND_FLAG_t flag @b{(input)} type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpClusterResendFlagClear(CP_CLUSTER_RESEND_FLAG_t flag)
{
  cpdmOprData->cpClusterResendFlag &= ~flag;
  return L7_SUCCESS;
}

