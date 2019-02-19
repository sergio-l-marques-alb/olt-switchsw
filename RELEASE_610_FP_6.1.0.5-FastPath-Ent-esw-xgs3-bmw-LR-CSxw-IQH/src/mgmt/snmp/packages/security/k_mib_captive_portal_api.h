/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename k_mib_captive_portal_api.h
*
* @purpose Captive Portal Data Manager (CPDM) USMDB API functions
*
* @component CPDM
*
* @comments none
*
* @create 06/26/2007
*
* @authors  wjacobs, rjindal
*
* @end
*             
**********************************************************************/

#ifndef K_MIB_CAPTIVE_PORTAL_API_H
#define K_MIB_CAPTIVE_PORTAL_API_H

#include "captive_portal_commdefs.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_user_api.h"
#include "usmdb_cpdm_web_api.h"
#include "usmdb_cpim_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "usmdb_snmp_trap_captive_portal_api.h"
#include "osapi.h"
#include "snmpapi.h"

/*********************************************************************
* @purpose  Get the CP mode
*
* @param    L7_uint32  *mode  @b{(output)} CP global mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the CP mode
*
* @param    L7_uint32 mode @b{(input)} CP global mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalModeSet(L7_uint32 mode);

/********************************************************************* 
* @purpose  Set CP config mode
*
* @param    cpId @b{(input)} CP config id
* @param    mode @b{(input)} value for config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments only L7_CP_MODE_ENABLED, .._DISABLED are acceptable as inputs.
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigModeGet(cpId_t cpId, L7_uint32 *mode);

/*********************************************************************
* @purpose  Get CP config protocol mode
*
* @param    cpId @b{(input)} CP config id
* @param    *protocolMode @b{(output)} protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigProtocolModeGet(cpId_t cpId, L7_uint32 *protocolMode);

/*********************************************************************
* @purpose  Get CP config verify mode
*
* @param    cpId @b{(input)} CP config id
* @param    *verifyMode @b{(output)} verify mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigVerifyModeGet(cpId_t cpId, L7_uint32 *verifyMode);

/*********************************************************************
* @purpose  Get CP config user group
*
* @param    cpId @b{(input)} CP config id
* @param    *group @b{(output)} group id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigGroupIDGet(cpId_t cpId, L7_uint32 *group);

/*********************************************************************
* @purpose  Get CP config user logout Mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_BOOL *userLogoutMode @b{(output)} user logout mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigUserLogoutModeGet(cpId_t cpId, L7_uint32 *userLogoutMode);

/*********************************************************************
* @purpose  Get CP config redirectMode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_BOOL *redirectMode @b{(output)} verify mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRedirectModeGet(cpId_t cpId, L7_uint32 *redirectMode);

/*********************************************************************
* @purpose  Get CP config redirect url
*
* @param    cpId @b{(input)} CP config id
* @param    *url @b{(output)} redirect url
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRedirectURLGet(cpId_t cpId, L7_char8 *url);

/********************************************************************* 
* @purpose  Set CP config mode
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_CP_MODE_STATUS_t mode @b{(input)} value for
*           onfig item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments only L7_CP_MODE_ENABLED, .._DISABLED are acceptable as inputs.
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigModeSet(cpId_t cpId, L7_uint32 mode);

/*********************************************************************
* @purpose  Set CP config protocol mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_LOGIN_TYPE_t *protocolMode @b{(input)} protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigProtocolModeSet(cpId_t cpId, L7_uint32 protocolMode);

/*********************************************************************
* @purpose  Set CP config verify mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    CP_VERIFY_MODE_t *verifyMode @b{(input)} verify mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigVerifyModeSet(cpId_t cpId, L7_uint32 verifyMode);

/*********************************************************************
* @purpose  Set CP config user logout mode
*
* @param    cpId_t    cpId          @b{(input)} CP config id
* @param    L7_uint32 userLogoutMode  @b{(input)} logout mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigUserLogoutModeSet(cpId_t cpId, L7_uint32 userLogoutMode);

/*********************************************************************
* @purpose  Set CP config redirect mode
*
* @param    cpId_t    cpId          @b{(input)} CP config id
* @param    L7_uint32 redirectMode  @b{(input)} redirect mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRedirectModeSet(cpId_t cpId, L7_uint32 redirectMode);

/*********************************************************************
* @purpose  Get the global admin mode for captive portal
*
* @param    L7_CP_MODE_STATUS_t @b{(output)} admin mode status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalStatusModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the reason for the CP to be disabled
*
* @param    L7_CP_MODE_REASON_t *reason @b{(output)} disable reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalCPDisableReasonGet(L7_uint32 *reason);

/*********************************************************************
* @purpose  Get the CP IP address
*
* @param    L7_IP_ADDR_t *ipAddr @b{(output)} ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmGlobalCPIPAddressGet(L7_IP_ADDR_t *ipAddr);

/*********************************************************************
* @purpose  Get the operational status of a CP instance.
*
* @param    cpid    @b{(input)} CP config id
* @param    *status @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigOperStatusGet(cpId_t cpId, L7_uint32 *status);

/*********************************************************************
* @purpose  Get the disable reason of a disabled CP instance.
*
* @param    cpid    @b{(input)} CP config id
* @param    *reason @b{(output)} disable reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
********************************************************************/
L7_RC_t snmpCpdmCPConfigDisableReasonGet(cpId_t cpId, L7_uint32 *reason);

/*********************************************************************
* @purpose  Get the blocked status of a CP instance
*
* @param    cpId_t     cpId          @b{(input)} CP config id
* @param    L7_uint32  *blockStatus  @b{(output)} block status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t snmpCpdmCPConfigBlockStatusGet(cpId_t cpId, L7_uint32 *blockStatus);

/*********************************************************************
* @purpose  Set the block status of a CP instance
*
* @param    cpId_t    cpId         @b{(input)} CP config id
* @param    L7_uint32 blockStatus  @b{(input)} block status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigBlockStatusSet(cpId_t cpId, L7_uint32 blockStatus);

/*********************************************************************
* @purpose  Retrieve enabled status for interface
*
* @param    L7_uint32  intf     @b{(input)} interface number
* @param    L7_uint32  *status  @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPIntfStatusGet(L7_uint32 intf, L7_uint32 *status);

/*********************************************************************
* @purpose  Retrieve reason for disabling of interface
*
* @param    L7_uint32               intf  @b{(input)} interface number
* @param    L7_INTF_WHY_DISABLED_t  *why  @b{(output)} intf disable reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPIntfStatusGetWhy(L7_uint32 intf, L7_uint32 *why);

/*********************************************************************
* @purpose  Get the blocked status of a CP interface
*
* @param    L7_uint32  intf          @b{(input)} CP interface
* @param    L7_uint32  *blockStatus  @b{(output)} block status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t snmpCpdmCPIntfBlockStatusGet(L7_uint32 intf, L7_uint32 *blockStatus);

/********************************************************************* 
* @purpose  Get interface capability mask
*
* @param    L7_uint32  intf   @b{(input)} interface
* @param    L7_uchar8  *mask  @b{(output)} pointer to capability mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpCaptivePortalIntfCapabilitiesGet(L7_uint32 intf, L7_uchar8 *buf, L7_uint32 *buf_len);

/*********************************************************************
* @purpose  Get protocolMode with existing MAC for connection status
*
* @param    *macAddr      @b{(input)} MAC addr to try
* @param    protocolMode  @b{(output)} value of "protocolMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusProtocolModeGet(L7_enetMacAddr_t *macAddr, L7_uint32 *protocolMode);

/*********************************************************************
* @purpose  Get verifyMode with existing MAC for connection status
*
* @param    *macAddr     @b{(input)} MAC addr to try
* @param    *verifyMode  @b{(output)} value of "verifyMode"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusVerifyModeGet(L7_enetMacAddr_t *macAddr, L7_uint32 *verifyMode);

/*********************************************************************
* @purpose  Get intfId with existing MAC for connection status
*
* @param    *macAddr  @b{(input)} MAC addr to try
* @param    *ifIndex  @b{(output)} value of "intfId"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusIntfIdGet(L7_enetMacAddr_t *macAddr, L7_uint32 *ifIndex);

/*********************************************************************
* @purpose  Get sessionTime with existing MAC for connection status
*
* @param    L7_enetMacAddr_t  *macAddr      @b{(input)} MAC addr to try
* @param    L7_uint32         *sessionTime  @b{(output)} value of "sessionTime"
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusSessionTimeGet(L7_enetMacAddr_t *macAddr, L7_uint32 *sessionTime);

/*********************************************************************
* @purpose  Get the switch type with existing MAC for connection status
*
* @param    *macAddr  @b{(input)} MAC addr to try
* @param    type      @b{(output)} switch type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnStatusWhichSwitchGet(L7_enetMacAddr_t *macAddr, L7_uint32 *type);

/*********************************************************************
* @purpose  Convert a 64-bit interger
*
* @param    L7_uint64 value  @b{(input)} 64-bit integer
* @param    L7_uint32 *high  @b{(input)} high end
* @param    L7_uint32 *low   @b{(input)} low end
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCPConvert64BitUnsignedInteger(L7_uint64 value, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Deauthenticate connected client
*
* @param    L7_uchar8 *mac  @b{(input)} MAC addr to try
* @param    L7_uint32 val   @b{(input)} action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDeauthenticateAction(L7_uchar8 *mac, L7_int32 val);

/*********************************************************************
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
*********************************************************************/
L7_RC_t snmpCpdmCPConnStatusGet(L7_uint32 cpId, L7_enetMacAddr_t *mac);

/*********************************************************************
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
*********************************************************************/
L7_RC_t snmpCpdmCPConnStatusNextGet(L7_uint32 cpId, L7_enetMacAddr_t *mac, 
                                    L7_uint32 *pCpId, L7_enetMacAddr_t *pMac);

/*********************************************************************
* @purpose  Get Trap Manager's Captive Portal trap flag
*
* @param    L7_uint32  *mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpTrapCaptivePortalGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set Trap Manager's Captive Portal trap flag
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpTrapCaptivePortalSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the CP client authentication failure trap mode
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientAuthFailureTrapModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the CP client authentication failure trap mode
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientAuthFailureTrapModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the CP client connection trap mode
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnectTrapModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the CP client connection trap mode
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientConnectTrapModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the CP client database full trap mode
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDBFullTrapModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the CP client database full trap mode
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDBFullTrapModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the CP client disconnection trap mode
*
* @param    L7_uint32  *mode  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDisconnectTrapModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the CP client disconnection trap mode
*
* @param    L7_uint32  mode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t snmpCpdmClientDisconnectTrapModeSet(L7_uint32 mode);

#if FASTPATH_BUT_NOT_DLINK
/*********************************************************************
* @purpose  Get CP config RADIUS accounting
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *radiusAccounting @b{(output)} RADIUS accounting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRadiusAccountingGet(cpId_t cpId, L7_uint32 *radiusAccounting);

/*********************************************************************
* @purpose  Set CP config RADIUS accounting
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 radiusAccounting @b{(input)} RADIUS accounting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snmpCpdmCPConfigRadiusAccountingSet(cpId_t cpId, L7_uint32 radiusAccounting);
#endif

#endif /* K_MIB_CAPTIVE_PORTAL_API_H */

