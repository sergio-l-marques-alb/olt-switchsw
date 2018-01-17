/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_cpdm_api.h
*
* @purpose      Captive Portal Data Manager (WDM) USMDB API functions
*
* @component    CPDM
*
* @comments     APIs for usmDb layer. These APIs can be called via usmDb
*               and by the captive portal components.
*
* @create       6/26/2007
*
* @authors      darsenault,dcaugherty,rjindal
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_USMDB_CPDM_API_H
#define INCLUDE_USMDB_CPDM_API_H

#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_cpdm_web_api.h"
#include "captive_portal_commdefs.h"


L7_RC_t usmDbCpdmCPConfigWebIdAdd(cpId_t cpId, webId_t webId);
L7_RC_t usmDbCpdmCPConfigWebImageNameGet(cpId_t cpId, webId_t webId, L7_char8 *imageName);

/*********************************************************************
* Global Captive Portal Configuration
*********************************************************************/

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the CP mode
*
* @param    L7_uint32 *mode @b{(output)} CP global mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the global peer switch stats report interval for captive
*           portal.
*
* @param    L7_uint32 mode @b{(input)} report interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalStatusPeerSwStatsReportIntervalSet(L7_uint32 interval);

/*********************************************************************
*
* @purpose  Set the global session timeout for captive portal.
*
* @param    L7_uint32 mode @b{(input)} session timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalStatusSessionTimeoutSet(L7_uint32 timeout);

/*********************************************************************
*
* @purpose  Get the global admin mode status for captive portal
*
* @param    L7_CP_MODE_STATUS_t *mode @b{(output)} admin mode status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalStatusModeGet(L7_CP_MODE_STATUS_t *mode);

/*********************************************************************
*
* @purpose  Set the global peer switch stats report interval for captive
*           portal.
*
* @param    L7_unint32 @b{(output)} interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalStatusPeerSwStatsReportIntervalGet(L7_uint32 *interval);

/*********************************************************************
*
* @purpose  Get the global session timeout for captive portal.
*
* @param    L7_unint32 @b{(output)} timeout.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalStatusSessionTimeoutGet(L7_uint32 *timeout);

/*********************************************************************
*
* @purpose  Get the global trap flags for captive portal.
*
* @param    CP_TRAP_FLAGS_t @b{(input)} traps to enable/disable
* @param    L7_unint32    * @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If more than one flag is specified, returns L7_ENABLE
*           if ANY are set.  So.. don't do that.
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalTrapModeGet(CP_TRAP_FLAGS_t trapToTest, 
				   L7_uint32 * mode);

/*********************************************************************
*
* @purpose  Set the global trap flags for captive portal.
*
* @param    CP_TRAP_FLAGS_t @b{(input)} traps to enable/disable
* @param    L7_unint32      @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalTrapModeSet(CP_TRAP_FLAGS_t traps, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Set the HTTP port
*
* @param    L7_uint32 port @b{(input)} HTTP port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalHTTPPortSet(L7_uint32 port);

/*********************************************************************
*
* @purpose  Get the HTTP port
*
* @param    L7_uint32 *port @b{(output)} HTTP port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalHTTPPortGet(L7_uint32 *port);

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Set the default HTTP secure port
*
* @param    L7_uint32 securePort @b{(input)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalHTTPSecurePort1Set(L7_uint32 securePort);
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Get the default HTTP secure port
*
* @param    L7_uint32 *securePort @b{(output)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalHTTPSecurePort1Get(L7_uint32 *securePort);
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Set the additional HTTP secure port
*
* @param    L7_uint32 securePort @b{(input)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalHTTPSecurePort2Set(L7_uint32 securePort);
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Get the additional HTTP secure port
*
* @param    L7_uint32 *securePort @b{(output)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalHTTPSecurePort2Get(L7_uint32 *securePort);
#endif

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalCPDisableReasonGet(L7_CP_MODE_REASON_t *reason);

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalCPIPAddressGet(L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose  Get the number of supported CPs in the system
*
* @param    L7_uint32 *supportedCPs @b{(output)} number of CP instances
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalSupportedCPInstancesGet(L7_uint32 *supportedCPs);

/*********************************************************************
*
* @purpose  Get the number of CP instances that are administratively enabled
*
* @param    L7_uint32 *configCPs @b{(output)} number of CP instances
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalConfiguredCPInstancesGet(L7_uint32 *configCPs);

/*********************************************************************
*
* @purpose  Get the number of CP instances that are operationally enabled
*
* @param    L7_uint32 *activeCPs @b{(output)} number of CP instances
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalActiveCPInstancesGet(L7_uint32 *activeCPs);

/*********************************************************************
*
* @purpose  Get the number of authenticated users the system can support
*
* @param    L7_uint32 *supportedUsers @b{(output)} number of supported users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalSupportedUsersGet(L7_uint32 *supportedUsers);

/*********************************************************************
*
* @purpose  Get the number of local users the system can support
*
* @param    L7_uint32 *localUsers @b{(output)} number of local users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalLocalUsersGet(L7_uint32 *localUsers);

/*********************************************************************
*
* @purpose  Get the number of users currently authenticated to all CP 
*           instances on this switch. For the WIDS Controller the number 
*           includes users authenticated on all switches in the peer group.
*
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
L7_RC_t usmDbCpdmGlobalAuthenticatedUsersGet(L7_uint32 *authUsers);

/*********************************************************************
*
* @purpose  Get the maximum number of users that can be stored in the 
*           Authorization Failure table
*
* @param    L7_uint32 *authFailSize @b{(output)} max number of users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalAuthFailureMaxUsersGet(L7_uint32 *authFailSize);

/*********************************************************************
*
* @purpose  Get the number of users in the Authorization Failure table
*
* @param    L7_uint32 *authFailUsers @b{(output)} number of users
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalAuthFailureUsersGet(L7_uint32 *authFailUsers);

/*********************************************************************
*
* @purpose  Get the maximum number of entries in the activity log
*
* @param    L7_uint32 *actLogSize @b{(output)} max number of entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalActivityLogMaxEntriesGet(L7_uint32 *actLogSize);

/*********************************************************************
*
* @purpose  Get the number of entries in the Activity table
*
* @param    L7_uint32 *actLogEntries @b{(output)} number of entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmGlobalActivityLogEntriesGet(L7_uint32 *actLogEntries);

/*********************************************************************
* CP Configuration 
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigAdd(cpId_t cpId);
L7_RC_t usmDbCpdmCPConfigDelete(cpId_t cpId);
L7_RC_t usmDbCpdmCPConfigGet(cpId_t cpId);
L7_RC_t usmDbCpdmCPConfigReset(cpId_t cpId);



L7_RC_t usmDbCpdmCPConfigIdGet(L7_uint32 intf, L7_short16 *cpId);

L7_RC_t usmDbCpdmCPConfigNextGet(cpId_t cpId, cpId_t *nextCpId);
L7_RC_t usmDbCpdmCPConfigSet();

/********************************************************************* 
* @purpose  Get CP config name
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *name @b{(output)} - corresponds to
*           "name" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigNameGet(cpId_t cpId, L7_char8 *name);

/********************************************************************* 
* @purpose  Get CP enabled mode
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_CP_MODE_STATUS_t * mode @b{(output)} value for
*           config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigModeGet(cpId_t cpId, L7_CP_MODE_STATUS_t * mode);


/*********************************************************************
*
* @purpose  Get CP config protocol mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_LOGIN_TYPE_t *protocolMode @b{(output)} protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigProtocolModeGet(cpId_t cpId, L7_LOGIN_TYPE_t *protocolMode);

/*********************************************************************
*
* @purpose  Get CP config verify mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    CP_VERIFY_MODE_t *verifyMode @b{(output)} verify mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigVerifyModeGet(cpId_t cpId, CP_VERIFY_MODE_t *verifyMode);

/*********************************************************************
*
* @purpose  Get CP config user logout mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uchar8 *userLogoutMode @b{(output)} logout request mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigUserLogoutModeGet(cpId_t cpId, L7_uchar8 *userLogoutMode);

/*********************************************************************
*
* @purpose  Get CP config RADIUS accounting
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_BOOL *radiusAccounting @b{(output)} verify mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigRadiusAccountingGet(cpId_t cpId, L7_BOOL *radiusAccounting);


/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigRedirectModeGet(cpId_t cpId, L7_uchar8 *redirectMode);


/*********************************************************************
*
* @purpose  Get CP config user upstream bandwidth
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *userUpRate @b{(output)} upstream rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigUserUpRateGet(cpId_t cpId, L7_uint32  *userUpRate);


/*********************************************************************
*
* @purpose  Get CP config user downstream bandwidth
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *userDownRate @b{(output)} downstream rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigUserDownRateGet(cpId_t cpId, L7_uint32  *userDownRate);



/*********************************************************************
*
* @purpose  Get CP config user max input octets
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *maxInputOctets @b{(output)} max input octets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxInputOctetsGet(cpId_t cpId, L7_uint32  *maxInputOctets);


/*********************************************************************
*
* @purpose  Get CP config user max output octets
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *maxOutputOctets @b{(output)} max output octets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxOutputOctetsGet(cpId_t cpId, L7_uint32 *maxOutputOctets);

/*********************************************************************
*
* @purpose  Get CP config user max total octets
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *maxTotalOctets @b{(output)} max output octets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxTotalOctetsGet(cpId_t cpId, L7_uint32 *maxTotalOctets);

/*********************************************************************
*
* @purpose  Get CP config user session timeout
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *sessionTimeout @b{(output)} session timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigSessionTimeoutGet(cpId_t cpId, L7_uint32  *sessionTimeout);


/*********************************************************************
*
* @purpose  Get CP config user idle timeout
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *idleTimeout @b{(output)} idle timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigIdleTimeoutGet(cpId_t cpId, L7_uint32  *idleTimeout);


/*********************************************************************
*
* @purpose  Get CP config user intrusion threshold
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *intrusionThreshold @b{(output)} intrusion
*           threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigIntrusionThresholdGet(cpId_t cpId, L7_uint32  *intrusionThreshold);


/********************************************************************* 
* @purpose  Get CP config foregroundColor
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *foregroundColor @b{(output)} - corresponds to
*           "foregroundColor" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The color is a string as its capable of hex codes
*           or color names.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigForegroundColorGet(cpId_t cpId, L7_char8 *foregroundColor);

/********************************************************************* 
* @purpose  Get CP config backgroundColor
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *backgroundColor @b{(output)} - corresponds to
*           "backgroundColor" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The color is a string as its capable of hex codes
*           or color names.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigBackgroundColorGet(cpId_t cpId, L7_char8 *backgroundColor);

/********************************************************************* 
* @purpose  Get CP config separatorColor
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *separatorColor @b{(output)} - corresponds to
*           "separatorColor" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The color is a string as its capable of hex codes
*           or color names.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigSeparatorColorGet(cpId_t cpId, L7_char8 *separatorColor);

/********************************************************************* 
* @purpose  Get CP config group id
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    gpId_t *gpId @b{(output)} - Group id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigGpIdGet(cpId_t cpId, gpId_t *gpId);

/********************************************************************* 
* @purpose  Get CP config RADIUS Authentication server
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *radiusServer @b{(output)} - corresponds to
*           "radiusAuthServer" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigRadiusAuthServerGet(cpId_t cpId, L7_char8 *radiusServer);

/*********************************************************************
*
* @purpose  Get CP config user redirect URL
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint8 *groupName @b{(output)} redirect URL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigRedirectURLGet(cpId_t cpId, L7_uint8  *redirectURL);


/********************************************************************* 
* @purpose  Set CP config name
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *name @b{(input)} - new
*           "name" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigNameSet(cpId_t cpId, L7_char8 *name);


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
* @comments only L7_CP_MODE_ENABLED, .._DISABLED are acceptable
*           as inputs.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigModeSet(cpId_t cpId, L7_CP_MODE_STATUS_t mode);



/********************************************************************* 
* @purpose  Set CP config group id
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    gpId_t gpId @b{(input)} GP id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigGpIdSet(cpId_t cpId, gpId_t gpId);

/*********************************************************************
 *
 * @purpose  Set CP config protocol mode
 *
 * @param    cpId_t cpId @b{(input)} CP config id
 * @param    L7_LOGIN_TYPE_t protocolMode @b{(input)} protocol mode
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbCpdmCPConfigProtocolModeSet(cpId_t cpId, L7_LOGIN_TYPE_t protocolMode);

/*********************************************************************
 *
 * @purpose  Set CP config verify mode
 *
 * @param    cpId_t cpId @b{(input)} CP config id
 * @param    CP_VERIFY_MODE_t verifyMode @b{(input)} verify mode
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbCpdmCPConfigVerifyModeSet(cpId_t cpId, CP_VERIFY_MODE_t verifyMode);

/*********************************************************************
*
* @purpose  Set CP config user logout mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uchar8 userLogoutMode @b{(output)} logout request mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigUserLogoutModeSet(cpId_t cpId, L7_uchar8 userLogoutMode);

/*************************************************************
* @purpose  Set value of field "radiusAccounting"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *radiusAccounting @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigRadiusAccountingSet(cpId_t cpId, L7_BOOL radiusAccounting);

/********************************************************************* 
* @purpose  Set CP config RADIUS Authentication server
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *radiusServer @b{(output)} - corresponds to
*           "radiusAuthServer" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigRadiusAuthServerSet(cpId_t cpId, L7_char8 *radiusServer);

/*************************************************************
* @purpose  Set value of field "redirectURL"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *redirectURL @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigRedirectURLSet(cpId_t cpId, L7_char8 *redirectURL);

/*************************************************************
* @purpose  Set value of field "redirectMode"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *redirectMode @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigRedirectModeSet(cpId_t cpId, L7_uchar8 redirectMode);

/*************************************************************
* @purpose  Set value of field "userUpRate"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *userUpRate @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigUserUpRateSet(cpId_t cpId, L7_uint32 userUpRate);

/*************************************************************
* @purpose  Set value of field "userDownRate"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *userDownRate @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigUserDownRateSet(cpId_t cpId, L7_uint32 userDownRate);

/*************************************************************
* @purpose  Set value of field "maxInputOctets"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *maxInputOctets @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxInputOctetsSet(cpId_t cpId, L7_uint32 maxInputOctets);

/*************************************************************
* @purpose  Set value of field "maxOutputOctets"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *maxOutputOctets @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxOutputOctetsSet(cpId_t cpId, L7_uint32 maxOutputOctets);

/*************************************************************
* @purpose  Set value of field "maxTotalOctets"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_uint32 *maxTotalOctets @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxTotalOctetsSet(cpId_t cpId, L7_uint32 maxTotalOctets);

/*************************************************************
* @purpose  Set value of field "sessionTimeout"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *sessionTimeout @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigSessionTimeoutSet(cpId_t cpId, L7_uint32 sessionTimeout);

/*************************************************************
* @purpose  Set value of field "idleTimeout"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *idleTimeout @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigIdleTimeoutSet(cpId_t cpId, L7_uint32 idleTimeout);

/*************************************************************
* @purpose  Set value of field "intrusionThreshold"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *intrusionThreshold @b{(input)} - new value
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigIntrusionThresholdSet(cpId_t cpId, L7_uint32 intrusionThreshold);

/********************************************************************* 
* @purpose  Set value of locale field "foregroundColor"
*
* @param    cpId_t   cpId @b{(input)} CP config id
* @param    L7_char8 *foregroundColor @b{(input)} - new
*           "foregroundColor" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The color is a string as its capable of hex codes
*           or color names.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigForegroundColorSet(cpId_t cpId, L7_char8 *foregroundColor);

/********************************************************************* 
* @purpose  Set value of locale field "backgroundColor"
*
* @param    cpId_t   cpId @b{(input)} CP config id
* @param    L7_char8 *backgroundColor @b{(input)} - new
*           "backgroundColor" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The color is a string as its capable of hex codes
*           or color names.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigBackgroundColorSet(cpId_t cpId, L7_char8 *backgroundColor);

/********************************************************************* 
* @purpose  Set value of locale field "separatorColor"
*
* @param    cpId_t   cpId @b{(input)} CP config id
* @param    L7_char8 *separatorColor @b{(input)} - new
*           "separatorColor" of config item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The color is a string as its capable of hex codes
*           or color names.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigSeparatorColorSet(cpId_t cpId, L7_char8 *separatorColor);

/********************************************************************* 
* @purpose  Verify existence of CP ID / internal I/F association
*
* @param    cpId_t     cpId      @b{(input)} CP config id
* @param    L7_uint32  intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbCpdmCPConfigIntIfNumGet(L7_uint32 cpId, L7_uint32 intIfNum);

/********************************************************************* 
* @purpose  Get next CP ID / internal I/F association
*
* @param    cpId_t     cpId       @b{(input)} CP config id
* @param    L7_uint32  intIfNum   @b{(input)} interface id
* @param    cpId_t     *pCpId     @b{(output)} CP config id
* @param    L7_uint32  *pIntIfNum @b{(output)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_FAILURE return code implies end of data
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbCpdmCPConfigIntIfNumNextGet(cpId_t cpId, L7_uint32 intIfNum,
                                 cpId_t * pCpId, L7_uint32 * pIntIfNum);

/********************************************************************* 
* @purpose  Add new CP ID / internal I/F association
*
* @param    cpId_t     cpId       @b{(input)} CP config id
* @param    L7_uint32  intIfNum   @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbCpdmCPConfigIntIfNumAdd(L7_uint32 cpId, L7_uint32 intIfNum);


/********************************************************************* 
* @purpose  Delete existing CP ID / internal I/F association
*
* @param    cpId_t     cpId       @b{(input)} CP config id
* @param    L7_uint32  intIfNum   @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbCpdmCPConfigIntIfNumDelete(L7_uint32 cpId, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Find the CP ID corresponding to an internal I/F number
*
* @param    L7_uint32    intIfNum @b{(input)} internal I/F number
* @param    L7_uint32    *cpId    @b{(output)} CP ID 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigIntIfNumFind(L7_uint32 intIfNum, cpId_t * cpId);


/********************************************************************* 
* @purpose  Get interface capability field: state
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *state @b{(output)} pointer to state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityStateGet(L7_uint32 intfId, L7_uchar8 *state);

/********************************************************************* 
* @purpose  Get interface capability field: sessionTimeout
*
* @param    L7_uint32 intfId          @b{(input)} interface ID
* @param    L7_uchar8 *sessionTimeout @b{(output)} pointer to sessionTimeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilitySessionTimeoutGet(L7_uint32 intfId, L7_uchar8 *sessionTimeout);

/********************************************************************* 
* @purpose  Get interface capability field: idleTimeout
*
* @param    L7_uint32 intfId       @b{(input)} interface ID
* @param    L7_uchar8 *idleTimeout @b{(output)} pointer to idleTimeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityIdleTimeoutGet(L7_uint32 intfId, L7_uchar8 *idleTimeout);

/********************************************************************* 
* @purpose  Get interface capability field: bandwidthUpRateControl
*
* @param    L7_uint32 intfId           @b{(input)} interface ID
* @param    L7_uchar8 *bwUpRateControl @b{(output)} pointer to Rate Control
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityBandwidthUpRateControlGet(L7_uint32 intfId, L7_uchar8 *bwUpRateControl);

/********************************************************************* 
* @purpose  Get interface capability field: bandwidthDownRateControl
*
* @param    L7_uint32 intfId             @b{(input)} interface ID
* @param    L7_uchar8 *bwDownRateControl @b{(output)} pointer to Rate Control
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityBandwidthDownRateControlGet(L7_uint32 intfId, L7_uchar8 *bwDownRateControl);

/********************************************************************* 
* @purpose  Get interface capability field: maxInputOctetMonitor
*
* @param    L7_uint32 intfId             @b{(input)} interface ID
* @param    L7_uchar8 *inputOctetMonitor @b{(output)} pointer to monitor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityMaxInputOctetMonitorGet(L7_uint32 intfId, L7_uchar8 *inputOctetMonitor);

/********************************************************************* 
* @purpose  Get interface capability field: maxOutputOctetMonitor
*
* @param    L7_uint32 intfId              @b{(input)} interface ID
* @param    L7_uchar8 *outputOctetMonitor @b{(output)} pointer to monitor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityMaxOutputOctetMonitorGet(L7_uint32 intfId, L7_uchar8 *outputOctetMonitor);

/********************************************************************* 
* @purpose  Get interface capability field: bytesReceivedCounter
*
* @param    L7_uint32 intfId          @b{(input)} interface ID
* @param    L7_uchar8 *bytesRxCounter @b{(output)} pointer to counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityBytesReceivedCounterGet(L7_uint32 intfId, L7_uchar8 *bytesRxCounter);

/********************************************************************* 
* @purpose  Get interface capability field: bytesTransmittedCounter
*
* @param    L7_uint32 intfId          @b{(input)} interface ID
* @param    L7_uchar8 *bytesTxCounter @b{(output)} pointer to counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityBytesTransmittedCounterGet(L7_uint32 intfId, L7_uchar8 *bytesTxCounter);

/********************************************************************* 
* @purpose  Get interface capability field: packetsReceivedCounter
*
* @param    L7_uint32 intfId         @b{(input)} interface ID
* @param    L7_uchar8 *pktsRxCounter @b{(output)} pointer to counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityPacketsReceivedCounterGet(L7_uint32 intfId, L7_uchar8 *pktsRxCounter);

/********************************************************************* 
* @purpose  Get interface capability field: packetsTransmittedCounter
*
* @param    L7_uint32 intfId         @b{(input)} interface ID
* @param    L7_uchar8 *pktsTxCounter @b{(output)} pointer to counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityPacketsTransmittedCounterGet(L7_uint32 intfId, L7_uchar8 *pktsTxCounter);

/********************************************************************* 
* @purpose  Get interface capability field: roamingSupport
*
* @param    L7_uint32 intfId          @b{(input)} interface ID
* @param    L7_uchar8 *roamingSupport @b{(output)} pointer to roamingSupport
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmIntfCapabilityRoamingSupportGet(L7_uint32 intfId, L7_uchar8 *roamingSupport);


/********************************************************************* 
* @purpose  Get the next interface not associated to any CP config
*
* @param    L7_uint32  intf      @b{(input)} input interface
* @param    L7_uint32  *nextIntf @b{(output)} next interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Input interface 0 (zero) gets the first interface.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigIntfNotAssocNextGet(L7_uint32 intf, L7_uint32 *nextIntf);

/*********************************************************************
 *
 * @purpose  Used to set data changed flag.
 *
 *
 * @returns  none
 * @returns  none
 *
 * @notes   This is primarily used by the web server whenever it creates
 *          (or deletes) an image file.
 *
 * @end
 *********************************************************************/
void usmDbCpdmSetImageFileChanged(void);

/*********************************************************************
*
* @purpose  Return the current images pointer and indicate the end of
*           image data (eoi) as this is not a null terminated string
*           but is rather formatted.
*
* @param    L7_uint32    *eoi    @b{(output)} Current end of image
*
* @returns  L7_uchar8 * image stream or L7_NULLPTR
*
* @end
*
*********************************************************************/
L7_uchar8 * usmDbCpdmImageDataGet(L7_uint32 *eoi);

/*********************************************************************
*
* @purpose  Set the size of the decoded image structure.
*
* @param    L7_unint32 @b{(input)} size
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmDecodedImageSizeSet(L7_uint32 eoi);

/*********************************************************************
*
* @purpose  Decode the incoming base64 text and append the binary
*           data to the images storage structure. Save the images by
*           creating the actual files on the disl when we done
*           decoding.
*
* @param    L7_uchar8 @b{(text)} encoded base64 text
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Incoming text is a null terminated string in base64
*           encoded format. Each encoded string is decoded and
*           appended to the end of the previous iteration based on
*           the end of text (eot) byte count. The end of the encoded
*           stream is determined by the expected end of image (eoi)
*           byte count. This is because the base64 format doesn't
*           define a terminator. When we reach the expected eoi byte
*           count, we simply write out the images data.
*
* @notes    This function supports FP text based configurations.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpdmDecodeTextAndImageCreate(L7_uchar8 *text);


#endif /* INCLUDE_USMDB_CPDM_API_H */

