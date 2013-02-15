/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  cpwio_api.h
*
* @purpose   Public APIs for captive portal wired interface owner
*
* @component captive portal wired interface owner
*
* @comments  yeah, these are public APIs, but the only references to 
*            them will be through wioCallbacks.
*
* @create    2/28/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#ifndef _CPWIO_API_H_
#define _CPWIO_API_H_

#include "l7_common.h"
#include "intf_cb_api.h"
#include "captive_portal_commdefs.h"

/*********************************************************************
* @purpose  Get pointer to Captive Portal call back functions for 
*           wired interface owner
*
* @param    void *cpCbPtr       pointer to a pointer of structure of  
*                               wireless CP call back function pointers
*
* @notes    none
*            
*       
* @end
*********************************************************************/
void wioCpCallbackGet(void **cpCbPtr);

/*********************************************************************
* @purpose  Set a capability on a wired captive portal interface.
*
* @param    intIfNum     Internal Interface Number  
* @param    cpCapType	   CP Capability Type
* @param    cpCap        CP Capability to be set
*
* @returns  L7_FAILURE
*
* @notes    no-op. Not supporting this. Not described in FS.
*       
* @end
*********************************************************************/
L7_RC_t wioCpCapabilitySet(L7_uint32 intIfNum,
                           L7_INTF_PARM_CP_TYPES_t  cpCapType,
                           L7_uint32 cpCap);

/*********************************************************************
* @purpose  Get the set of capabilities on a captive portal wired interface. 
*
* @param    intIfNum     Internal Interface Number  
* @param    cpCapType	   CP Capability Type
* @param    cpCap        CP Capability to be get
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    handled synchronously    
*
* @end
*********************************************************************/
L7_RC_t wioCpCapabilityGet(L7_uint32 intIfNum,
                           L7_INTF_PARM_CP_TYPES_t  cpCapType,
                           L7_uint32 *cpCap);

/*********************************************************************
* @purpose  CP calls this API to tell the wired interface owner that
*           a client is either authenticated or unauthenticated. 
*
* @param    msgType       CP Operation type
* @param    clientMacAddr Client MAC address
* @param    upRate        Up Data Rate 
* @param    downRate      Down Data Rate     
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    notification is handled asynchronously  
*           upRate and downRate intentionally ignored  
*
* @end
*********************************************************************/
L7_RC_t wioCpAuthenUnauthenClient(CP_OP_t cpCmd, 
                                  L7_enetMacAddr_t clientMacAddr, 
                                  L7_uint32 upRate, 
                                  L7_uint32 downRate);

/*********************************************************************
* @purpose  Deauthenticate a captive portal client on a wired interface. 
*
* @param    clientMacAddr Client MAC address 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wioCpDeauthenClient(L7_enetMacAddr_t clientMacAddr);

/*********************************************************************
* @purpose  Block or unblock user access to the network interface 
*
* @param    cpCmd      CP Operation type
* @param    intIfNum   Internal Interface Number
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wioCpBlockUnblockAccess(CP_OP_t cpCmd, 
                                L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Enable or disable captive portal on a wired interface. 
*
* @param    cpCmd       CP Operation type
* @param    intIfNum    Internal Interface Number
* @param    redirIp     IP address of local authentication server 
* @param    redirIpMask IP subnet mask of local authentication server 
* @param    intfMac     mac address with length L7_MAC_ADDR_LEN
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wioCpEnableDisableFeature(CP_OP_t cpCmd, 
                                  L7_uint32 intIfNum, 
                                  cpRedirIpAddr_t redirIp,
                                  L7_uint32 redirIpMask,
                                  L7_uchar8 intfMac[L7_ENET_MAC_ADDR_LEN]);

/*********************************************************************
*
* @purpose  Get Client Captive Portal Statistics
*
* @param    L7_enetMacAddr_t macAddrClient @b{(input)} client MAC address
* @param    txRxCount_t      clientCounter @b{(output)} client stats
* @param    L7_uint32        timePassed @b{(output)} time passed since 
*                                                    last update  
*
* @returns  L7_NOT_SUPPORTED
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wioCpClientStatisticsGet(L7_enetMacAddr_t macAddrClient, 
                                 txRxCount_t *clientCounter, 
                                 L7_uint32 *timePassed);

/*********************************************************************
* @purpose  Set the authentication port 
*
* @param    intIfNum      Internal Interface Number 
* @param    authPortnum   Authentication Port Number to be set      
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wioCpSetAuthPortNum(L7_uint32 intIfNum, L7_ushort16 authPortNum);

/*********************************************************************
* @purpose  Set the first user-configured secure authentication port 
*
* @param    intIfNum      Internal Interface Number 
* @param    authPortNum   Authentication Port Number to be set      
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    A non-zero port number indicates that CPWIO should intercept
*           packets to this port. If authPortNum is 0, CPWIO removes the
*           port number previously learned through this callback. If this
*           callback is invoked twice with non-zero authPortNum, CPWIO
*           overwrites the first port number with the second one.    
*
* @end
*********************************************************************/
L7_RC_t wioCpAuthSecurePort1Num(L7_uint32 intIfNum, L7_ushort16 authPortNum);

/*********************************************************************
* @purpose  Set the second user-configured secure authentication port 
*
* @param    intIfNum      Internal Interface Number 
* @param    authPortNum   Authentication Port Number to be set      
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    A non-zero port number indicates that CPWIO should intercept
*           packets to this port. If authPortNum is 0, CPWIO removes the
*           port number previously learned through this callback. If this
*           callback is invoked twice with non-zero authPortNum, CPWIO
*           overwrites the first port number with the second one.    
*
* @end
*********************************************************************/
L7_RC_t wioCpAuthSecurePort2Num(L7_uint32 intIfNum, L7_ushort16 authPortNum);


#endif   /* _CPWIO_API_H_ */
