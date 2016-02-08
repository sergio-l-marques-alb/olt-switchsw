
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

#include <string.h>
#include "l7_common.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "osapi.h"
#include "captive_portal_commdefs.h"
#include "cpdm_connstatus_api.h"

#include "clustering_api.h"


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
L7_RC_t usmDbCpdmCPConnStatusGet(cpId_t cpId, L7_enetMacAddr_t * mac)
{
  return cpdmCPConnStatusGet(cpId, mac);
}


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
L7_RC_t usmDbCpdmCPConnStatusNextGet(cpId_t cpId, 
                                     L7_enetMacAddr_t * mac,
                                     cpId_t * pCpId,
                                     L7_enetMacAddr_t * pMac)
{
  return cpdmCPConnStatusNextGet(cpId, mac, pCpId, pMac);
}



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
usmDbCpdmInterfaceConnStatusGet(L7_uint32 intIfNum, 
                                L7_enetMacAddr_t * mac)
{
  return cpdmInterfaceConnStatusGet(intIfNum, mac);
}

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
usmDbCpdmInterfaceConnStatusNextGet(L7_uint32 intIfNum, 
                                    L7_enetMacAddr_t * mac,
                                    L7_uint32 *pIntIfNum, 
                                    L7_enetMacAddr_t * pMac)
{
  return cpdmInterfaceConnStatusNextGet(intIfNum, mac, pIntIfNum, pMac);
}

/*********************************************************************
*
*  CLIENT CONNECTION STATUS DB FUNCTIONS
*
*********************************************************************/

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
* @comments Can't imagine why you wouldn't deauth a user, but who
*           knows..
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmClientConnStatusByNameDelete(L7_uchar8 * userName)
{
   return cpdmClientConnStatusByNameDelete(userName);
}


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
L7_RC_t usmDbCpdmClientConnStatusGet(L7_enetMacAddr_t * macAddr)
{
   return cpdmClientConnStatusGet(macAddr);
}


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
L7_RC_t usmDbCpdmClientConnStatusNextGet(L7_enetMacAddr_t * macAddr, L7_enetMacAddr_t * nextMac)
{
   return cpdmClientConnStatusNextGet(macAddr, nextMac);
}


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
L7_RC_t usmDbCpdmClientConnStatusDelete(L7_enetMacAddr_t * mac)
{
  cpdmClientConnStatusDeleteUserRequestSet();
  return cpdmClientConnStatusDelete(mac);
}

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
L7_RC_t usmDbCpdmClientConnStatusDeleteAll(void)
{
  return cpdmClientConnStatusDeleteAll();
}

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
L7_RC_t usmDbCpdmClientConnStatusByCPIdDeleteAll(cpId_t cpId)
{
  return cpdmClientConnStatusByCPIdDeleteAll(cpId);
}

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
L7_RC_t usmDbCpdmClientConnStatusIpGet(L7_enetMacAddr_t * macAddr, L7_IP_ADDR_t * ip)
{
   return cpdmClientConnStatusIpGet(macAddr, ip);
}


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
L7_RC_t usmDbCpdmClientConnStatusSwitchMacAddrGet(L7_enetMacAddr_t * macAddr,
				  	     L7_enetMacAddr_t * switchMacAddr)
{
  return cpdmClientConnStatusSwitchMacAddrGet(macAddr, switchMacAddr);
}


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
L7_RC_t usmDbCpdmClientConnStatusSwitchIpGet(L7_enetMacAddr_t * macAddr,
				        L7_IP_ADDR_t * switchIp)
{
  return cpdmClientConnStatusSwitchIpGet( macAddr, switchIp);
}


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
L7_RC_t usmDbCpdmClientConnStatusWhichSwitchGet(L7_enetMacAddr_t * macAddr,
				           cpdmAuthWhichSwitch_t * wswitch)
{
   return cpdmClientConnStatusWhichSwitchGet(macAddr, wswitch);
}


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
L7_RC_t usmDbCpdmClientConnStatusUserNameGet(L7_enetMacAddr_t * macAddr, L7_uchar8 * userName)
{
   return cpdmClientConnStatusUserNameGet(macAddr, userName);
}



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
L7_RC_t usmDbCpdmClientConnStatusProtocolModeGet(L7_enetMacAddr_t * macAddr, L7_LOGIN_TYPE_t * protocolMode)
{
   return cpdmClientConnStatusProtocolModeGet(macAddr, protocolMode);
}


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
L7_RC_t usmDbCpdmClientConnStatusVerifyModeGet(L7_enetMacAddr_t * macAddr, CP_VERIFY_MODE_t * verifyMode)
{
   return cpdmClientConnStatusVerifyModeGet(macAddr, verifyMode);
}

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
L7_RC_t usmDbCpdmClientConnStatusUserLogoutFlagGet(L7_enetMacAddr_t * macAddr,
                   L7_CP_USER_LOGOUT_FLAG_t * userLogoutFlag)
{
   return cpdmClientConnStatusUserLogoutFlagGet(macAddr, userLogoutFlag);
}

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
L7_RC_t usmDbCpdmClientConnStatusUserLogoutFlagSet(L7_enetMacAddr_t * macAddr,
                   L7_CP_USER_LOGOUT_FLAG_t userLogoutFlag)
{
   return cpdmClientConnStatusUserLogoutFlagSet(macAddr, userLogoutFlag);
}

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
L7_RC_t usmDbCpdmClientConnStatusIntfIdGet(L7_enetMacAddr_t * macAddr, L7_uint32 * intfId)
{
   return cpdmClientConnStatusIntfIdGet(macAddr, intfId);
}

/*********************************************************************
*
* @purpose  Delete ALL entries in client connection table for a given
*           interface.
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
L7_RC_t usmDbCpdmClientConnStatusIntfDeleteAll(L7_uint32 intfId)
{
  return cpdmClientConnStatusByIntfDeleteAll(intfId);
}

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
L7_RC_t usmDbCpdmClientConnStatusCpIdGet(L7_enetMacAddr_t * macAddr, cpId_t * cpId)
{
   return cpdmClientConnStatusCpIdGet(macAddr, cpId);
}



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
L7_RC_t usmDbCpdmClientConnStatusSessionTimeGet(L7_enetMacAddr_t * macAddr, L7_uint32 * sessionTime)
{
   return cpdmClientConnStatusSessionTimeGet(macAddr, sessionTime);
}



/*********************************************************************
*
* @purpose  Find first entry that matches given IP address
*
* @param    L7_IP_ADDR_t  ip @b{(input)} IP to search for
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
L7_RC_t usmDbCpdmClientConnStatusFirstIPMatch(L7_IP_ADDR_t ip,
					      L7_enetMacAddr_t * macAddr)
{
  return cpdmClientConnStatusFirstIPMatch(ip, macAddr);
}


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
* @comments macAddr can be NULL; output value is discarded.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmClientConnStatusNextIPMatch(L7_IP_ADDR_t  ip,
					L7_enetMacAddr_t * macAddr)
{
   return cpdmClientConnStatusNextIPMatch(ip, macAddr);
}

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
L7_RC_t usmDbCpdmClientConnStatusIntfClientNextGet(L7_uint32 intf, L7_enetMacAddr_t macAddr, 
                                                   L7_enetMacAddr_t *nextMacAddr)
{
  return cpdmClientConnStatusIntfClientNextGet(intf, macAddr, nextMacAddr);
}

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
L7_RC_t usmDbCpdmClientConnStatusCPIDClientNextGet(cpId_t cpId, L7_enetMacAddr_t macAddr, 
                                                   L7_enetMacAddr_t *nextMacAddr)
{
  return cpdmClientConnStatusCPIDClientNextGet(cpId, macAddr, nextMacAddr);
}


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
L7_RC_t usmDbCpdmClientConnStatusStatisticsGet(L7_enetMacAddr_t * macAddr,
                                          L7_uint64 * bytesTransmitted,
                                          L7_uint64 * bytesReceived,
                                          L7_uint64 * pktsTransmitted,
                                          L7_uint64 * pktsReceived)
{
  return  cpdmClientConnStatusStatisticsGet(macAddr, bytesTransmitted,
                                            bytesReceived, pktsTransmitted,
                                            pktsReceived);
}


/*********************************************************************
 * 
 *                        INTERFACE STATUS API
 *
 *********************************************************************/


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
L7_RC_t usmDbCpdmIntfStatusAdd(L7_uint32   intfId)
{
  return cpdmIntfStatusAdd(intfId);
}

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
L7_RC_t usmDbCpdmIntfStatusDelete(L7_uint32 intfId)
{
  return cpdmIntfStatusDelete(intfId);
}


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
L7_RC_t usmDbCpdmIntfStatusGet(L7_uint32 intfId)
{
  return cpdmIntfStatusGet(intfId);
}


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
L7_RC_t usmDbCpdmIntfStatusNextGet(L7_uint32 intfId, L7_uint32 * nextIntfId)
{
  return cpdmIntfStatusNextGet(intfId, nextIntfId);
}


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
L7_RC_t usmDbCpdmIntfStatusGetIsEnabled(L7_uint32 intfId, L7_BOOL * isEnabled)
{
  return cpdmIntfStatusGetIsEnabled(intfId, isEnabled);
}

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
L7_RC_t usmDbCpdmIntfStatusGetWhy(L7_uint32 intfId, 
				  L7_INTF_WHY_DISABLED_t * why)
{
  return cpdmIntfStatusGetWhy(intfId, why);
}


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
L7_RC_t usmDbCpdmIntfStatusGetUsers(L7_uint32 intfId, L7_uint32 * users)
{
   return cpdmIntfStatusGetUsers(intfId, users);
}

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
L7_RC_t usmDbCpdmIntfStatusSetIsEnabled(L7_uint32 intfId, L7_BOOL isEnabled)
{
  return cpdmIntfStatusSetIsEnabled(intfId, isEnabled);
}



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
L7_RC_t usmDbCpdmIntfStatusSetWhy(L7_uint32 intfId, L7_INTF_WHY_DISABLED_t why)
{
  return cpdmIntfStatusSetWhy(intfId, why);
}

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
L7_RC_t usmDbCpdmIntfStatusSetUsers(L7_uint32 intfId, L7_uint32  users)
{
  return cpdmIntfStatusSetUsers(intfId, users);
}



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
L7_RC_t usmDbCpdmCPConfigOperStatusGet(cpId_t cpId, 
				  L7_CP_INST_OPER_STATUS_t *status)
{
  return cpdmCPConfigOperStatusGet(cpId, status);
}


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
L7_RC_t usmDbCpdmCPConfigBlockedStatusGet(cpId_t cpId, 
				     L7_CP_INST_BLOCK_STATUS_t *blocked)
{
  return cpdmCPConfigBlockedStatusGet(cpId, blocked);
}

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
L7_RC_t usmDbCpdmCPConfigDisableReasonGet(cpId_t cpId, 
				     L7_CP_INST_DISABLE_REASON_t *reason)
{
  return cpdmCPConfigDisableReasonGet(cpId, reason);
}

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
L7_RC_t usmDbCpdmCPConfigAuthenticatedUsersGet(cpId_t cpId, 
					  L7_uint32 *authUsers)
{
  return cpdmCPConfigAuthenticatedUsersGet(cpId, authUsers);
}

/*********************************************************************
*
* @purpose  Set blocked status for CP ID
*
* @param    cpId_t cpId    @b{(input)} CP ID
* @param    L7_CP_INST_BLOCK_STATUS_t   status @b{(input)} blocked
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigBlockedStatusSet(cpId_t cpId, L7_CP_INST_BLOCK_STATUS_t blocked)
{
  return cpdmCPConfigBlockedStatusSet(cpId, blocked);
}

/*********************************************************************
* @purpose  Check if clustering is supported on the switch
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED
*
* @end
*********************************************************************/
L7_RC_t usmDbCpdmClusterSupportGet(void)
{
  return clusterSupportGet();
}

