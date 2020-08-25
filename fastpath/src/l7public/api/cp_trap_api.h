/*********************************************************************
* 
* (C) Copyright Broadcom Corporation 2003-2007
* All Rights Reserved.
* 
* 
**********************************************************************
*
* @filename     cp_trap_api.h
*
* @purpose      Captive Portal trap API header
*
* @component    Captive Portal
*
* @comments     none
*
* @create       11/28/2007
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_CP_TRAP_API_H
#define INCLUDE_CP_TRAP_API_H

#include "datatypes.h"
#include "comm_structs.h"




/*********************************************************************
*
* @purpose  Send SNMP trap for client connection
*
* @param    *macAddr       @b{(input)} AP MAC address
* @param    ipAddr         @b{(input)} client IP address
* @param    *switchMacAddr @b{(input)} MAC address of authenticating switch
* @param    cpId           @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex @b{(input)} ifIndex of port on authenticating switch
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapClientConnected(L7_enetMacAddr_t * macAddr, L7_uint32 ipAddr,
                           L7_enetMacAddr_t * switchMacAddr, cpId_t cpId,
													 L7_uint32 ifIndex);


/*********************************************************************
*
* @purpose  Send SNMP trap for client disconnection
*
* @param    *macAddr       @b{(input)} AP MAC address
* @param    ipAddr         @b{(input)} client IP address
* @param    *switchMacAddr @b{(input)} MAC address of authenticating switch
* @param    cpId           @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex @b{(input)} ifIndex of port on authenticating switch
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapClientDisconnected(L7_enetMacAddr_t * macAddr, L7_uint32 ipAddr,
                              L7_enetMacAddr_t * switchMacAddr, cpId_t cpId,
                              L7_uint32 ifIndex);


/*********************************************************************
*
* @purpose  Send a Captive Portal Client Authentication Failure trap
*
* @param    macAddr        @b{(input)} client MAC address
* @param    ipAddr         @b{(input)} client IP address
* @param    userName       @b{(input)} client user name
* @param    switchMacAddr  @b{(input)} MAC address of authenticating switch
* @param    cpId           @b{(input)} captive portal instance handling the authentication
* @param    ifIndex        @b{(input)} ifIndex of port on authenticating switch
* @param    attempts       @b{(input)} number of connection attempts
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
void
cpTrapClientAuthFailure( L7_enetMacAddr_t * macAddr, L7_uint32 ipAddr,
                         L7_uchar8 *userName, L7_enetMacAddr_t * switchMacAddr, 
                         cpId_t cpId, L7_uint32 ifIndex, 
                         L7_uint32 attempts);


/*********************************************************************
*
* @purpose  Send SNMP trap for client connection DB reaching 
*           max capacity
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapClientConnectionDatabaseFull(void);



/*********************************************************************
*
* @purpose  Send SNMP trap for wrapping of authentication 
*           failure list
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapAuthFailureLogWrap(void);



/*********************************************************************
*
* @purpose  Send SNMP trap for wrapping of activity log
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapActivityLogWrap(void);


#endif /* INCLUDE_CP_TRAP_API_H  */
