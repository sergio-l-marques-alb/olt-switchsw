/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2008
*
**********************************************************************
*
* @filename     cpdm_clustering_api.h
*
* @purpose      Captive Portal Data Manager (CPDM) Clustering API Header
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

#ifndef INCLUDE_CPDM_CLUSTERING_API_H
#define INCLUDE_CPDM_CLUSTERING_API_H

#include "clustering_commdefs.h"
#include "cpdm.h"
#include "cp_cluster.h"

/*********************************************************************
* @purpose  Add or update a peer switch status entry
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} switch MAC address
* @param    CP_CLUSTER_SWITCH_STATUS_t status @b{(input)} switch status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments This function adds a new entry or updates an existing entry.
*
* @end
*********************************************************************/
L7_RC_t cpClusterSwitchEntryAddUpdate(L7_enetMacAddr_t macAddr, CP_CLUSTER_SWITCH_STATUS_t status);

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
                                         L7_uint32 intf, L7_uchar8 ifStatus, L7_uint32 ifUsers);

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
L7_RC_t cpClusterSwitchEntryDelete(L7_enetMacAddr_t macAddr);

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
L7_RC_t cpClusterSwitchEntryGet(L7_enetMacAddr_t macAddr);

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
L7_RC_t cpClusterSwitchEntriesPurge(void);

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
                                            L7_enetMacAddr_t swMac);

/*********************************************************************
* @purpose  Remove a peer switch client status entry
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
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
                                         L7_enetMacAddr_t swMacAddr);

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
L7_RC_t cpClusterSwitchClientPeerClientsDelete(L7_enetMacAddr_t swMacAddr);

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
L7_RC_t cpClusterSwitchClientEntryGet(L7_enetMacAddr_t macAddr);

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
L7_RC_t cpClusterSwitchClientCPIDGet(L7_enetMacAddr_t macAddr, cpId_t *cpId);

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
L7_RC_t cpClusterSwitchClientSwitchMACGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *swMac);

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
L7_RC_t cpClusterSwitchClientEntryByIPGet(L7_IP_ADDR_t ipAddr, L7_enetMacAddr_t *clientMacAddr);

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
L7_RC_t cpClusterSwitchClientEntriesPurge(void);

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
L7_RC_t cpClusterFlagSet(CP_CLUSTER_FLAG_t flag);

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
L7_RC_t cpClusterFlagGet(CP_CLUSTER_FLAG_t *flag);

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
L7_RC_t cpClusterFlagClear(CP_CLUSTER_FLAG_t flag);

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
L7_RC_t cpClusterResendFlagSet(CP_CLUSTER_RESEND_FLAG_t flag);

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
L7_RC_t cpClusterResendFlagGet(CP_CLUSTER_RESEND_FLAG_t *flag);

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
L7_RC_t cpClusterResendFlagClear(CP_CLUSTER_RESEND_FLAG_t flag);

#endif /* INCLUDE_CPDM_CLUSTERING_API_H */

