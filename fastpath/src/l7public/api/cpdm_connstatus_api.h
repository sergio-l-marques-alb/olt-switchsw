#ifndef CPDM_CONNSTATUS_API_H
#define CPDM_CONNSTATUS_API_H

/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename     cpdm.h
 *
 * @purpose      Captive Portal Data Manager (CPDM) header
 *               for various connection status DBs
 *
 * @component    CPDB
 *
 * @comments     none
 *
 * @create       8/1/2007
 *
 * @author       dcaugherty
 *
 * @end
 *
 **********************************************************************/

#include "datatypes.h"
#include "intf_cb_api.h"
#include "captive_portal_commdefs.h"
#include "cpdm.h"

/*********************************************************************
*
*  VARIOUS STATUS DB FUNCTIONS required for SNMP Support
* 
*********************************************************************/


/*********************************************************************
*
* @purpose  Check for existing CP instance/client MAC association
*
* @param  cpId_t           cpId @b{(input)} CP instance to find
* @param  L7_enetMacAddr_t *mac @b{(input)} MAC address to find
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConnStatusGet(cpId_t cpId, L7_enetMacAddr_t * mac);



/*********************************************************************
*
* @purpose  Find NEXT association of CP instance and client MAC
*
* @param  cpId_t           cpId @b{(input)} CP instance to find
* @param  L7_enetMacAddr_t *mac @b{(input)} MAC address to find
* @param  cpId_t           *pCpId @b{(output)} next CP instance
* @param  L7_enetMacAddr_t *pMmac @b{(output)} next MAC address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConnStatusNextGet(cpId_t cpId, 
                                L7_enetMacAddr_t * mac,
                                cpId_t * pCpId,
                                L7_enetMacAddr_t * pMac);



/*********************************************************************
*
* @purpose  Get entry in the interface connection status table
*
* @param    L7_uint32  intIfNum @{(input)} internal interface number
* @param    L7_enetMacAddr *  mac @{(input)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t 
cpdmInterfaceConnStatusGet(L7_uint32 intIfNum, 
                           L7_enetMacAddr_t * mac);


/*********************************************************************
*
* @purpose  Get next entry in the interface connection status table
*
* @param  L7_uint32    intIfNum @{(input)} internal interface number
* @param  L7_enetMacAddr   *mac @{(input)} MAC address
* @param  L7_uint32  *pIntIfNum @{(output)} internal interface number
* @param  L7_enetMacAddr  *pMac @{(output)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t 
cpdmInterfaceConnStatusNextGet(L7_uint32 intIfNum, 
                               L7_enetMacAddr_t * mac,
                               L7_uint32 *pIntIfNum, 
                               L7_enetMacAddr_t * pMac);


/*********************************************************************
*
* @purpose  Get entry in the interface connection status table
*
* @param    L7_uint32  intIfNum @{(input)} internal interface number
* @param    L7_enetMacAddr *  mac @{(input)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t 
cpdmInterfaceAssocConnStatusGet(cpId_t cpId, L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Get next entry in the interface connection status table
*
* @param  cpId_t     cpId     @{(input)} CP instance ID
* @param  L7_uint32  intIfNum @{(input)} internal interface number
* @param  cpId_t     *pCpId     @{(output)} next CP instance ID
* @param  L7_uint32  *pIntIfNum @{(output)} internal interface number

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t 
cpdmInterfaceAssocConnStatusNextGet(cpId_t    cpId,
                                    L7_uint32 intIfNum, 
                                    cpId_t    *pCpId,
                                    L7_uint32 *pIntIfNum);


/*********************************************************************
*
*  CLIENT CONNECTION STATUS DB FUNCTIONS
*
*********************************************************************/


/*********************************************************************
*
* @purpose  Add new connection information if available
*
* @param    L7_enetMacAddr_t* macAddr  @b{(input)} MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new information can be added,
*           or if parameter is garbage.
*
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusNewAdd(L7_enetMacAddr_t * mac);


/*********************************************************************
*
* @purpose  Add delete connection information if available
*
* @param    L7_enetMacAddr_t* macAddr  @b{(input)} MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new information can be added,
*           or if parameter is garbage.
*
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusDeleteAdd(L7_enetMacAddr_t * mac);


/*********************************************************************
*
* @purpose  Add deauth connection information if available
*
* @param    L7_enetMacAddr_t*  @b{(input)} peer MAC addr
* @param    L7_enetMacAddr_t*  @b{(input)} client MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new information can be added,
*           or parameters are garbage.
*
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusDeauthAdd(L7_enetMacAddr_t * pPeerMac,
			      L7_enetMacAddr_t * pClientMac);


/*********************************************************************
*
* @purpose  Find next connection transaction type, if it exists
*
* @param    cpConnTransactionType_t * pType  @b{(output)} trans type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new transaction available,
*           or if parameter is garbage. Does NOT remove transaction
*           from queue.
* @end
*
*********************************************************************/
L7_RC_t
cpdmClientConnStatusNextTransTypeGet(L7_uint32 * pType);

/*********************************************************************
*
* @purpose  Purge all pending transactions
*
* @param    void
*
* @returns  void
*
* @end
*
*********************************************************************/
void
cpdmClientConnStatusNextTransPurge(void);


/*********************************************************************
*
* @purpose  Get next new connection transaction info, if it exists
*
* @param    L7_enetMacAddr_t * mac  @b{(output)} client MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new transaction available,
*           if next transaction is not for the addition of a new
*           client, or if parameter is garbage.  Removes transaction
*           from queue.
* @end
*
*********************************************************************/

L7_RC_t
cpdmClientConnStatusNewGet(L7_enetMacAddr_t * mac);

/*********************************************************************
*
* @purpose  Get next delete connection transaction info, if it exists
*
* @param    L7_enetMacAddr_t * mac  @b{(output)} client MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new transaction available,
*           if next transaction is not for the deletion of a 
*           client, or if parameter is garbage.  Removes transaction
*           from queue.
* @end
*
*********************************************************************/

L7_RC_t
cpdmClientConnStatusDeleteGet(L7_enetMacAddr_t * mac);


/*********************************************************************
*
* @purpose  Get next deauth connection transaction info, if it exists
*
* @param    L7_enetMacAddr_t * pPeerMac   @b{(output)} peer sw MAC
* @param    L7_enetMacAddr_t * pClientMac @b{(output)} client MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns L7_FAILURE if no new transaction available,
*           if next transaction is not for the deauthentication of a 
*           client, or if parameter is garbage.  Removes transaction
*           from queue.
* @end
*
*********************************************************************/

L7_RC_t
cpdmClientConnStatusDeauthGet(L7_enetMacAddr_t * pPeerMac,
			      L7_enetMacAddr_t * pClientMac);






/*********************************************************************
*
* @purpose  Add MAC to the client connection status table
*
* @param    L7_enetMacAddr_t* macAddr  @b{(input)} MAC addr
* @param    L7_uint32 intIfNum  @b{(input)} internal interface number
* @param    L7_uchar8 *userName @b{(input)} name of session's user
* @param    L7_IP_ADDR_t ipAddr @b{(input)} IP address of client
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusAdd(L7_enetMacAddr_t *mac,
        L7_uchar8        *userName,
        L7_uint32         intIfNum,
        L7_IP_ADDR_t      ipAddr);

/*********************************************************************
* @purpose  Update client status upon successful authentication.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusRoamUpdate(L7_enetMacAddr_t *macAddr);

/*********************************************************************
* @purpose  Update client in the controller connection status table
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_enetMacAddr_t swMac         @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusCrtlUpdate(L7_enetMacAddr_t clientMacAddr,
                                       L7_enetMacAddr_t swMac);

/*********************************************************************
* @purpose  Add peer switch client to the connection status table
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_IP_ADDR_t     ipAddr        @b{(input)} client IP address
* @param    L7_enetMacAddr_t swMac         @b{(input)} switch MAC address
* @param    L7_uchar8        userName      @b{(input)} user name
* @param    L7_LOGIN_TYPE_t  prtlMode      @b{(input)} protocol mode
* @param    CP_VERIFY_MODE_t verifyMode    @b{(input)} verify mode
* @param    L7_uint32        intf          @b{(input)} internal interface number
* @param    L7_uint32        cpId          @b{(input)} instance index
* @param    txRxCount_t      stats         @b{(input)} statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerAddUpdate(L7_enetMacAddr_t clientMacAddr, L7_IP_ADDR_t ipAddr,
                                          L7_enetMacAddr_t swMac, L7_uchar8 *userName,
                                          L7_LOGIN_TYPE_t prtlMode, CP_VERIFY_MODE_t verifyMode,
                                          L7_uint32 intf, L7_uint32 cpId, txRxCount_t *stats);

/*********************************************************************
* @purpose  Set flag to delete a peer switch client(s)
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    cpId_t           cpId          @b{(input)} CP instance
* @param    L7_BOOL          all           @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerDeleteFlagSet(L7_enetMacAddr_t clientMacAddr,
                                              cpId_t cpId, L7_BOOL all);

/*********************************************************************
* @purpose  Delete peer switch client from the connection status table
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    L7_enetMacAddr_t swMacAddr     @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerDelete(L7_enetMacAddr_t clientMacAddr,
                                       L7_enetMacAddr_t swMacAddr);

/*********************************************************************
* @purpose  Delete all clients for the specified peer switch from the 
*           connection status table
*
* @param    L7_enetMacAddr_t swMacAddr @b{(input)} switch MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerAllDelete(L7_enetMacAddr_t swMacAddr);

/*********************************************************************
* @purpose  Delete all peer switch clients from connection status table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function is called when the local switch leaves the 
*           cluster or when a switch is no longer the cluster controller.
*           UI should never call this function.
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerPurge(void);

/*********************************************************************
* @purpose  Update peer switch client statistics
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} client MAC address
* @param    txRxCount_t      stats         @b{(input)} pointer to statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusPeerStatsUpdate(L7_enetMacAddr_t clientMacAddr,
                                            txRxCount_t *stats);

/*********************************************************************
* @purpose  Set user initiation flag for client deletion
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpdmClientConnStatusDeleteUserRequestSet(void);


/*********************************************************************
*
* @purpose  Helper routine to update various parameters used primarily
*           upon successful authentication.
*
* @param    L7_enetMacAddr_t* macAddr @b{(input)} existing client
* @param    cpdmClientConnStatusData_t* macAddr @b{(input)} new data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUpdate(L7_enetMacAddr_t * mac,
                                   cpdmClientConnStatusData_t * client);

/*********************************************************************
*
* @purpose  Wipe MAC from the client connection status table
*
* @param    L7_enetMacAddr_t* macAddr @b{(input)} MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusDelete(L7_enetMacAddr_t * mac);


/*********************************************************************
*
* @purpose  Delete ALL entries in client connection table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusDeleteAll(void);


/*********************************************************************
*
* @purpose  Delete entries in client connection table used by 
*           a captive portal instance
*
* @param    cpId_t cpId, input, id of the CP instance to unplug
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusByCPIdDeleteAll(cpId_t cpId);


/*********************************************************************
*
* @purpose  Find the next connection status entry matching the 
*           given CP ID
*
* @param    cpId_t  cpId  @b{(input)} CP ID to match
* @param    L7_enetMacAddr_t *  macAddr  @b{(input)} macAddr
* @param    L7_enetMacAddr_t *  nextMac  @b{(output)} next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cpdmClientConnStatusByCPIDFind(cpId_t cpId, 
                                       L7_enetMacAddr_t * macAddr, 
                                       L7_enetMacAddr_t * nextMac);

/*********************************************************************
*
* @purpose  Find the next connection with the given uid
*
* @param    uId_t @b{(input)} uid - user id
* @param    L7_enetMacAddr_t * @b{(input)} mac - starting key
* @param    L7_enetMacAddr_t * @b{(output)} mac - resulting key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only makes sense with locally-verified connections
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusByUIdFind(uId_t uid, 
																			L7_enetMacAddr_t * mac,
																			L7_enetMacAddr_t * nextMac);

/*********************************************************************
*
* @purpose  Delete entries in client connection table used by 
*           a particular interface
*
* @param    L7_uint32 intIfNum, input, number of instance to unplug
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusByIntfDeleteAll(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Delete entries in client connection table by name
*
* @param    L7_uchar8*  userName @b{(input)} name of user
* @param    L7_BOOL     deAuth  @b{(input)} also deauthenticate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusByNameDelete(L7_uchar8 * userName);

/*********************************************************************
*
* @purpose  Delete the mac entry from the client connection status table
*           but don't deauthenticate or decrement
*
* @param    L7_enetMacAddr_t* macAddr @b{(input)} MAC addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusDeleteNoDeauth(L7_enetMacAddr_t * mac);


/*********************************************************************
*
* @purpose  Check for existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusGet(L7_enetMacAddr_t * macAddr);


/*********************************************************************
*
* @purpose  Check for NEXT MAC addr associated with a
*           connection status entry, given the current MAC addr
*
* @param    L7_enetMacAddr_t *  macAddr  @b{(input)} macAddr
* @param    L7_enetMacAddr_t *  nextMac  @b{(output)} next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusNextGet(L7_enetMacAddr_t * macAddr, L7_enetMacAddr_t * nextMac);


/*********************************************************************
*
* @purpose  Check for NEXT internal I/F id associated with a
*           connection, given the current I/F id
*
* @param    L7_uint32    intfId      @b{(input)} intfId
* @param    L7_uint32    nextIntfId  @b{(output)} next id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIntfNextGet(L7_uint32 intfId, 
                            L7_uint32 * nextIntfId,
                             L7_enetMacAddr_t * mac);

/*********************************************************************
*
* @purpose  Get ip with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_IP_ADDR_t * ip @b{(output)} value of "ip"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIpGet(L7_enetMacAddr_t * macAddr,
              L7_IP_ADDR_t * ip);


/*********************************************************************
*
* @purpose  Get MAC address of authenticating switch 
*           with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_enetMacAddr_t * switchMacAddr  @b{(output)} switch MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSwitchMacAddrGet(L7_enetMacAddr_t * macAddr,
                 L7_enetMacAddr_t * switchMacAddr);


/*********************************************************************
*
* @purpose  Get ip of authenticating switch 
*           with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_IP_ADDR_t * ip @b{(output)} value of "ip"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSwitchIpGet(L7_enetMacAddr_t * macAddr,
                L7_IP_ADDR_t * switchIp);


/*********************************************************************
*
* @purpose  Get which switch is authenticating with existing MAC 
*           for connection status
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    cpdmAuthWhichSwitch_t * whichSwitch @b{(output)} 
*            value of "whichSwitch"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusWhichSwitchGet(L7_enetMacAddr_t * macAddr,
                   cpdmAuthWhichSwitch_t * wswitch);


/*********************************************************************
*
* @purpose  Get user id associated with connection
*           with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    uId_t * uid @b{(output)} value of "uid"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only meaningful for locally verified connections. 
*           (Use the userName field for RADIUS or guest verification.)
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUIdGet(L7_enetMacAddr_t * macAddr,
                                   uId_t * uId);

/*********************************************************************
*
* @purpose  Get userName with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uchar8 * userName @b{(output)} value of "userName"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUserNameGet(L7_enetMacAddr_t * macAddr,
                L7_uchar8 * userName);



/*********************************************************************
*
* @purpose  Get protocolMode with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_LOGIN_TYPE_t * protocolMode @b{(output)} value of "protocolMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusProtocolModeGet(L7_enetMacAddr_t * macAddr,
              L7_LOGIN_TYPE_t * protocolMode);


/*********************************************************************
*
* @purpose  Get verifyMode with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    CP_VERIFY_MODE_t * verifyMode @b{(output)} value of "verifyMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusVerifyModeGet(L7_enetMacAddr_t * macAddr,
              CP_VERIFY_MODE_t * verifyMode);


/*********************************************************************
*
* @purpose  Get userLogoutFlag with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_CP_USER_LOGOUT_FLAG_t * userLogoutFlag @b{(output)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUserLogoutFlagGet(L7_enetMacAddr_t * macAddr,
              L7_CP_USER_LOGOUT_FLAG_t * userLogoutFlag);

/*********************************************************************
*
* @purpose  Set userLogoutFlag with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_CP_USER_LOGOUT_FLAG_t userLogoutFlag @b{(output)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUserLogoutFlagSet(L7_enetMacAddr_t * macAddr,
              L7_CP_USER_LOGOUT_FLAG_t userLogoutFlag);

/*********************************************************************
*
* @purpose  Get intfId with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * intfId @b{(output)} value of "intfId"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIntfIdGet(L7_enetMacAddr_t * macAddr,
              L7_uint32 * intfId);

/*********************************************************************
*
* @purpose  Get cpId with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    cpId_t * cpId @b{(output)} value of "cpId"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusCpIdGet(L7_enetMacAddr_t * macAddr,
            cpId_t * cpId);


/*********************************************************************
*
* @purpose  Get session timeout with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * session timeout @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSessionTimeoutGet(L7_enetMacAddr_t * macAddr,
             L7_uint32 * timeout);

/*********************************************************************
*
* @purpose  Get idle timeout with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * session timeout @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIdleTimeoutGet(L7_enetMacAddr_t * macAddr,
             L7_uint32 * timeout);

/*********************************************************************
*
* @purpose  Get sessionTime with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * sessionTime @b{(output)} value of "sessionTime"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSessionTimeGet(L7_enetMacAddr_t * macAddr,
             L7_uint32 * sessionTime);

/*********************************************************************
*
* @purpose  Get sessionTime with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * sessionTime @b{(output)} value of "sessionTime"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIdleTimeGet(L7_enetMacAddr_t * macAddr,
                L7_uint32 * sessionTime);


/********************************************************************* 
* @purpose  Get maxBandwidthUp for this client
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client
* @param    L7_uint32 *maxBandwidthUp @b{(output)} max up stream
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxBandwidthUpGet(L7_enetMacAddr_t * macAddr,
                                             L7_uint32 * maxBandwidthUp);

/********************************************************************* 
* @purpose  Get maxBandwidthDown for this client
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client
* @param    L7_uint32 *maxBandwidthDown @b{(output)} max down stream
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxBandwidthDownGet(L7_enetMacAddr_t * macAddr,
                                             L7_uint32 * maxBandwidthDown);

/*********************************************************************
*
* @purpose  Get last refresh timestamp of existing MAC 
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * connectTime @b{(output)} value of "lastRefresh"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusLastRefreshGet(L7_enetMacAddr_t * macAddr,
                   L7_uint32 * lastRefresh);

/********************************************************************* 
* @purpose  Get maxInputOctets for this client
*
* @param    L7_enetMacAddr_t  maxAddr @b{(input)} Client
* @param    L7_uint32 *maxInputOctets @b{(output)} Resolved max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxInputOctetsGet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 * maxInputOctets);

/********************************************************************* 
* @purpose  Get maxOutputOctets for this client
*
* @param    L7_enetMacAddr_t  maxAddr @b{(input)} Client
* @param    L7_uint32 *maxOutputOctets @b{(output)} Resolved max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxOutputOctetsGet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 * maxOutputOctets);

/********************************************************************* 
* @purpose  Get maxTotalOctets for this client
*
* @param    L7_enetMacAddr_t  maxAddr @b{(input)} Client
* @param    L7_uint32 *maxTotalOctets @b{(output)} Resolved max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxTotalOctetsGet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 * maxTotalOctets);

/*********************************************************************
*
* @purpose  Get session statistics for connection with given MAC
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_uint32 * bytesTransmitted @b{(output)} bytes tx'ed
* @param    L7_uint32 * bytesReceived    @b{(output)} bytes rx'ed
* @param    L7_uint32 * pktsTransmitted @b{(output)} packets tx'ed
* @param    L7_uint32 * pktsReceived    @b{(output)} packets rx'ed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusStatisticsGet(L7_enetMacAddr_t * macAddr,
                                          L7_uint64 * bytesTransmitted,
                                          L7_uint64 * bytesReceived,
                                          L7_uint64 * pktsTransmitted,
                                          L7_uint64 * pktsReceived);


/*********************************************************************
*
* @purpose  Find first entry that matches given IP address
*
* @param    L7_IP_ADDR_t * ip @b{(input)} IP to search for
* @param    L7_enetMacAddr_t * macAddr  @b{(output)} first
*                                 matching MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr can be NULL; output value is discarded.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusFirstIPMatch(L7_IP_ADDR_t ip,
           L7_enetMacAddr_t * macAddr);


/*********************************************************************
*
* @purpose  Find next entry that matches given IP address
*
* @param    L7_IP_ADDR_t * ip @b{(input)} IP to search for
* @param    L7_enetMacAddr_t * macAddr  @b{(output)} MAC
*                                from which to start
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
* @comments macAddr can be NULL; output value is discarded.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusNextIPMatch(L7_IP_ADDR_t  ip,
          L7_enetMacAddr_t * macAddr);

/********************************************************************* 
* @purpose  Get the next client on a given interface
*
* @param    L7_uint32         intf         @b{(input)} input interface
* @param    L7_enetMacAddr_t  macAddr      @b{(input)} input MAC
* @param    L7_enetMacAddr_t  *nextMacAddr @b{(output)} next MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIntfClientNextGet(L7_uint32 intf, L7_enetMacAddr_t macAddr, 
                                              L7_enetMacAddr_t *nextMacAddr);

/********************************************************************* 
* @purpose  Get the next client associated to a given CP Configuration
*
* @param    cpId_t            cpId         @b{(input)} CP ID
* @param    L7_enetMacAddr_t  macAddr      @b{(input)} input MAC
* @param    L7_enetMacAddr_t  *nextMacAddr @b{(output)} next MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusCPIDClientNextGet(cpId_t cpId, L7_enetMacAddr_t macAddr, 
                                              L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Set authenticating switch MAC address 
*           with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_enetMacAddr_t * switchMacAddr  @b{(input)} switch MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSwitchMacAddrSet(L7_enetMacAddr_t * macAddr,
                 L7_enetMacAddr_t * switchMacAddr);


/*********************************************************************
*
* @purpose  Set switch ip with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_IP_ADDR_t  ip @b{(input)} new value of "switchIp"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSwitchIpSet(L7_enetMacAddr_t * macAddr,
                  L7_IP_ADDR_t  switchIp);


/*********************************************************************
*
* @purpose  Set which switch is authenticating with existing MAC 
*           for connection status
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    cpdmAuthWhichSwitch_t  whichSwitch @b{(input)} 
*            value of "whichSwitch"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusWhichSwitchSet(L7_enetMacAddr_t * macAddr,
                   cpdmAuthWhichSwitch_t wswitch);


/*********************************************************************
*
* @purpose  Set uid  for connection status
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    uid_t   uid @b{(input)} user id to use
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only has meaning for locally verified connections.
*           TODO: We really shouldn't export this function as an
*           API.  Nuke this when we restructure the code again.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusUIdSet(L7_enetMacAddr_t * macAddr,
                                   uId_t uId);

/*********************************************************************
*
* @purpose  Set protocolMode with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_LOGIN_TYPE_t  protocolMode @b{(input)} new value of "protocolMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusProtocolModeSet(L7_enetMacAddr_t * macAddr,
              L7_LOGIN_TYPE_t  protocolMode);


/*********************************************************************
*
* @purpose  Set verifyMode with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    CP_VERIFY_MODE_t  verifyMode @b{(input)} new value of "verifyMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusVerifyModeSet(L7_enetMacAddr_t * macAddr,
            CP_VERIFY_MODE_t  verifyMode);

/*********************************************************************
* @purpose  Set new interface with existing MAC for connection status
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} pointer to MAC addr
* @param    L7_uint32        prevIntf @b{(input)} old interface
* @param    L7_uint32        newIntf  @b{(input)} new interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function is implemented to be used ONLY by the 
*           Captive Portal application when a wired client reconnects 
*           on a different interface. It is NOT to be used by any UI.
*
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusIntfIdSet(L7_enetMacAddr_t *macAddr,
                                      L7_uint32 prevIntf, L7_uint32 newIntf);

/*********************************************************************
*
* @purpose  Set cpId with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    cpId_t  cpId @b{(input)} new value of "cpId"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusCpIdSet(L7_enetMacAddr_t * macAddr,
                   cpId_t  cpId);

/*********************************************************************
*
* @purpose  Set session timeout with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     timeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusSessionTimeoutSet(L7_enetMacAddr_t * macAddr,
                   L7_uint32 timeout);

/*********************************************************************
*
* @purpose  Set idle timeout with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     timeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIdleTimeoutSet(L7_enetMacAddr_t * macAddr,
                   L7_uint32 timeout);

/*********************************************************************
*
* @purpose  Set max input octets with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     octets @b{(input)} max bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxInputOctetsSet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 octets);

/*********************************************************************
*
* @purpose  Set max output octets with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     octets @b{(input)} max bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxOutputOctetsSet(L7_enetMacAddr_t * macAddr,
                                               L7_uint32 octets);

/*********************************************************************
*
* @purpose  Set max total octets with existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32     octets @b{(input)} max bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusMaxTotalOctetsSet(L7_enetMacAddr_t * macAddr,
                                              L7_uint32 octets);

/*********************************************************************
*
* @purpose  Update session statistics for connection with given MAC
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to try
* @param    L7_BOOL bPreviousStats @b{(input)} act as if we've never
*           collected statistics (useful for roaming)
* @param    txRxCount_t * txRx  @b{(output)} current statistics set
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  txRx can be NULL if new stats are not desired
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusStatisticsUpdate(L7_enetMacAddr_t * macAddr,
                   txRxCount_t * txRx, L7_BOOL bNoPreviousStats );
  
/*********************************************************************
*
* @purpose  Reset idle time for existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
* @param    L7_uint32  idleTime @b{(input)} new value of "idleTime"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusIdleStartReset(L7_enetMacAddr_t * macAddr);
          

/*********************************************************************
*
* @purpose  reset last refresh time for existing MAC for connection status
*
* @param    L7_enetMacAddr_t * macAddr  @b{(input)} MAC addr to find
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmClientConnStatusLastRefreshReset(L7_enetMacAddr_t * macAddr);


/*********************************************************************
*
* @purpose  Return if CAC status tree is full
*
* @param    void
*
* @returns  L7_TRUE if full, L7_FALSE if not
*
* @comments none
*
* @end
*
*********************************************************************/
L7_BOOL cpdmClientConnStatusConnectFull(void);


/*********************************************************************
*
* @purpose  Monitor and deauthenticate sessions 
*
* @param    void
*
* @returns  L7_TRUE if full, L7_FALSE if not
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmMonitorSessions(void);

/*********************************************************************
*
* @purpose  Add status entry to intf assoc status table
*           progress table
*
* @param    L7_uint32         intfId  @b{(input)} internal I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusAdd(L7_uint32   intfId);


/*********************************************************************
*
* @purpose  Delete  authorization in progress table entry
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusDelete(L7_uint32 intfId);


/*********************************************************************
*
* @purpose  Check for existing inteface in the intf status tree
*
* @param    L7_uint32  intfId  @b{(input)} interface ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusGet(L7_uint32 intfId);

/*********************************************************************
*
* @purpose  Check for existing interface in our status tree
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusNextGet(L7_uint32 intfId, L7_uint32 * nextIntfId);

/*********************************************************************
*
* @purpose  Retrieve enabled status for intf
*
* @param    L7_uint32 intfId    @b{(input)} interface number
* @param    L7_BOOL * isEnabled @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusGetIsEnabled(L7_uint32 intfId, L7_BOOL * isEnabled);


/*********************************************************************
*
* @purpose  Retrieve reason for disabling of intf
*
* @param    L7_uint32 intfId    @b{(input)} interface number
* @param    L7_INTF_WHY_DISABLED_t * why @b{(output)} reason for
*                               disabling of I/F
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusGetWhy(L7_uint32 intfId, L7_INTF_WHY_DISABLED_t * why);

/*********************************************************************
*
* @purpose  Retrieve number of users of intf
*
* @param    L7_uint32 intfId  @b{(input)} interface number
* @param    L7_uint32 * users @b{(output)} number of users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusGetUsers(L7_uint32 intfId, L7_uint32 * users);


/*********************************************************************
*
* @purpose  Set enabled status for intf
*
* @param    L7_uint32 intfId    @b{(input)} interface number
* @param    L7_BOOL   isEnabled @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusSetIsEnabled(L7_uint32 intfId, L7_BOOL isEnabled);


/*********************************************************************
*
* @purpose  Set reason for disabling of intf
*
* @param    L7_uint32 intfId    @b{(input)} interface number
* @param    L7_INTF_WHY_DISABLED_t  why @b{(input)} reason for
*                                    disabling of I/F
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusSetWhy(L7_uint32 intfId, L7_INTF_WHY_DISABLED_t why);


/*********************************************************************
*
* @purpose  Retrieve number of users of intf
*
* @param    L7_uint32 intfId  @b{(input)} interface number
* @param    L7_uint32 * users @b{(output)} number of users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmIntfStatusSetUsers(L7_uint32 intfId, L7_uint32  users);



/*********************************************************************
*
*  ACTIVITY STATUS API FUNCTIONS
*
*********************************************************************/

/*********************************************************************
*
* @purpose  Create activation & activity status entries for CP instances
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void cpdmActStatusUpdate(void);

/*********************************************************************
*
* @purpose  Check for existing id in the activity status tree
*
* @param    cpId_t  cpId  @b{(input)} CP ID to  check
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusGet(cpId_t cpId);


/*********************************************************************
*
* @purpose  Check for next CP ID in our activity status tree
*
* @param    cpid_t cpId       @b{(input)} CP ID
* @param    cpid_t *nextCpId  @b{(output)} next CP ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusNextGet(cpId_t cpId, cpId_t * nextCpId);


/*********************************************************************
*
* @purpose  Retrieve enabled status for CP
*
* @param    cpId_t cpId    @b{(input)} interface number
* @param    L7_BOOL * isEnabled @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigOperStatusGet(cpId_t cpId, 
          L7_CP_INST_OPER_STATUS_t *status);

/*********************************************************************
*
* @purpose  Find the number of enabled/active CP instances
*
* @param    L7_uint32 * pCount @b{(output)} count of instances
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t cpdmActStatusOperEnabledCount(L7_uint32 * pCount);

/*********************************************************************
*
* @purpose  Get the authentication status of a CP instance.
*
* @param    cpid_t                    cpid    @b{(input)} CP config id
* @param    L7_CP_INST_BLOCK_STATUS_t *status @b{(output)} blocked status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigBlockedStatusGet(cpId_t cpId, 
             L7_CP_INST_BLOCK_STATUS_t *blocked);


/*********************************************************************
*
* @purpose  Retrieve reason for disabling of CP instance
*
* @param    cpId_t cpId    @b{(input)} interface number
* @param    L7_CP_INST_DISABLE_REASON_t * why @b{(output)} reason for
*                                   disabling of CP instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigDisableReasonGet(cpId_t cpId, 
             L7_CP_INST_DISABLE_REASON_t *reason);


/*********************************************************************
*
* @purpose  Get the number of users authenticated to a CP instance.
*
* @param    cpid_t    cpid       @b{(input)} CP config id
* @param    L7_uint32 *authUsers @b{(output)} number of authenticated users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigAuthenticatedUsersGet(cpId_t cpId, 
            L7_uint32 *authUsers);




/*********************************************************************
*
* @purpose  Set operational status for CP ID
*
* @param    cpId_t cpId    @b{(input)} CP ID
* @param    L7_CP_INST_OPER_STATUS_t   status @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigOperStatusSet(cpId_t cpId, 
          L7_CP_INST_OPER_STATUS_t status);


/*********************************************************************
*
* @purpose  Set operational status for ALL CP IDs
*
* @param    cpId_t cpId    @b{(input)} CP ID
* @param    L7_CP_INST_OPER_STATUS_t   status @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigOperStatusAllSet(L7_CP_INST_OPER_STATUS_t status);

/*********************************************************************
*
* @purpose  Set blocked status for CP ID
*
* @param    cpId_t cpId    @b{(input)} CP ID
* @param    L7_CP_INST_BLOCK_STATUS_t   status @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigBlockedStatusSet(cpId_t cpId, 
             L7_CP_INST_BLOCK_STATUS_t blocked);


/*********************************************************************
*
* @purpose  Set reason for disabling of CP instance
*
* @param    cpId_t cpId    @b{(input)} CP instance ID
* @param    L7_CP_WHY_DISABLED_t  why @b{(input)} reason for
*                                    disabling of CP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigDisableReasonSet(cpId_t cpId, 
             L7_CP_INST_DISABLE_REASON_t reason);

/*********************************************************************
*
* @purpose  Retrieve number of users of CP instance
*
* @param    cpId_t cpId  @b{(input)} CP ID
* @param    L7_uint32 * users @b{(output)} number of users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigAuthenticatedUsersSet(cpId_t cpId, L7_uint32 authUsers);

/*********************************************************************
*
* @purpose  Find the next connection status entry matching the 
*           given user name
*
* @param    L7_uchar8 *  userId  @b{(input)} name to match
* @param    L7_enetMacAddr_t *  macAddr  @b{(input)} macAddr
* @param    L7_enetMacAddr_t *  nextMac  @b{(output)} next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cpdmClientConnStatusByNameFind(L7_uchar8 *userName, 
                                       L7_enetMacAddr_t * macAddr, 
                                       L7_enetMacAddr_t * nextMac);


/*********************************************************************
* @purpose  Get the total number of switch authenticated clients
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value 
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t cpdmClientConnStatusTotalGet(L7_ushort16 *value);

#endif

