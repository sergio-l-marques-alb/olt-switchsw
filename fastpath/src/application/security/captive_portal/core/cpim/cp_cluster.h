/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   cp_cluster.h
*
* @purpose    Captive Portal (CP) Utilities Header File
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

#ifndef INCLUDE_CP_CLUSTER_H
#define INCLUDE_CP_CLUSTER_H

#include "clustering_commdefs.h"

typedef enum
{
  CP_CLUSTER_SWITCH_NONE = 0,
  CP_CLUSTER_SWITCH_MEMBER,
  CP_CLUSTER_SWITCH_CONTROLLER
} CP_CLUSTER_SWITCH_STATUS_t;

typedef enum
{
  CP_CLUSTER_CLIENT_DISCONNECTED = 0,
  CP_CLUSTER_CLIENT_CONNECTED
} CP_CLUSTER_CLIENT_STATUS_t;

typedef enum
{
  CP_CLUSTER_CP_DISABLED = 0,
  CP_CLUSTER_CP_ENABLED
} CP_CLUSTER_CP_STATUS_t;

typedef enum
{
  CP_CLUSTER_INTF_DISABLED = 0,
  CP_CLUSTER_INTF_ENABLED
} CP_CLUSTER_INTF_STATUS_t;

typedef enum
{
  CP_CLUSTER_RESEND_CLIENT_NOTIFICATION = 0,
  CP_CLUSTER_RESEND_CONN_CLIENTS_UPDATE,
  CP_CLUSTER_RESEND_INST_INTF_UPDATE
} CP_CLUSTER_RESEND_t;

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
L7_BOOL cpClusterValidMacAddr(L7_uchar8 *macAddr);

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
void cpClusterMemberEventCallback(clusterEvent eventType, clusterMemberID_t *memberID);

/*********************************************************************
* @purpose  Send CP client notification message to a peer switch
*
* @param    L7_enetMacAddr_t peerMacAddr @b{(input)} peer MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sends client notification message for all
*           authenticated clients to new peer switch when it joins.
*
* @end
*********************************************************************/
L7_RC_t cpClusterClientNotificationMsgPeerSend(L7_enetMacAddr_t peerMacAddr);

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
L7_RC_t cpClusterClientNotificationMsgSend(L7_enetMacAddr_t clientMacAddr, L7_BOOL authStatus);

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
L7_RC_t cpClusterClientAuthReqMsgSend(L7_enetMacAddr_t swMacAddr,
                                      L7_enetMacAddr_t clientMacAddr);

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
L7_RC_t cpClusterClientAuthReplyMsgSend(L7_enetMacAddr_t swMacAddr,
                                        L7_enetMacAddr_t clientMacAddr);

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
* @comments This function sends controller command message to peer switches.
*
* @end
*********************************************************************/
L7_RC_t cpClusterControllerCmdMsgSend(L7_enetMacAddr_t swMacAddr,
                                      L7_enetMacAddr_t clientMacAddr, L7_uint32 cpId,
                                      L7_uint32 cpBlkUnblk, L7_BOOL blkUnblkStatus);

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
                                          L7_enetMacAddr_t ctrlMacAddr);

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
                                       L7_enetMacAddr_t ctrlMacAddr);

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
L7_RC_t cpClusterStatsUpdateMsgSend(L7_enetMacAddr_t ctrlMacAddr);

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
L7_RC_t cpClusterConfigSyncMsgSend(L7_enetMacAddr_t swMacAddr);

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
L7_RC_t cpClusterResendMsgSend(L7_uchar8 type);

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
                          L7_uint32 msgLen, L7_uchar8 *buf);

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
void cpClusterClientNotificationMsgProcess(L7_uchar8 *buf);

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
void cpClusterClientAuthRequestMsgProcess(L7_uchar8 *buf);

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
void cpClusterClientAuthReplyMsgProcess(L7_uchar8 *buf);

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
void cpClusterControllerCmdMsgProcess(L7_uchar8 *buf);

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
void cpClusterConnClientsUpdateMsgProcess(L7_uchar8 *buf);

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
void cpClusterInstIntfUpdateMsgProcess(L7_uchar8 *buf);

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
void cpClusterStatsUpdateMsgHandle(L7_uchar8 *buf, L7_uint32 msgLen);

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
void cpClusterStatsUpdateMsgProcess(L7_uchar8 *buf, L7_uint32 msgLen);

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
void cpClusterConfigSyncMsgProcess(L7_uchar8 *buf);

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
void cpClusterResendMsgProcess(L7_uchar8 *buf);

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
L7_RC_t cpClusterConfigPushStatusGet(L7_uint32 *status);

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
L7_RC_t cpClusterConfigRxCheck();

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
void cpClusterConfigRxApply();

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
void cpClusterConfigSendCallback(clusterMemberID_t *memberID);

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
void cpClusterConfigRxCallback(clusterConfigCmd cmd);

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
void cpClusterConfigSyncCmdProcess(clusterConfigCmd cmd);

#endif /* INCLUDE_CP_CLUSTER_H */

