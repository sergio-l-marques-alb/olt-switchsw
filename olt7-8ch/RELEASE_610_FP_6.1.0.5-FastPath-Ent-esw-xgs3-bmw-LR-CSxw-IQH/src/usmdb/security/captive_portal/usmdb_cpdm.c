/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_cpdm.c
*
* @purpose Captive Portal Data Manager (CPDM) USMDB API functions
*
* @component CPDM
*
* @comments none
*
* @create 06/26/2007
*
* @authors  darsenault,dcaugherty,rjindal
*
* @end
*             
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "usmdb_cpdm_api.h"
#include "osapi.h"
#include "captive_portal_commdefs.h"
#include "cpdm_api.h"

#include "usmdb_cpim_api.h"


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
L7_RC_t usmDbCpdmGlobalModeSet(L7_uint32 mode)
{
  return cpdmGlobalModeSet(mode);
}

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
L7_RC_t usmDbCpdmGlobalModeGet(L7_uint32 *mode)
{
  return cpdmGlobalModeGet(mode);
}

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
L7_RC_t usmDbCpdmGlobalStatusPeerSwStatsReportIntervalSet(L7_uint32 interval)
{
  return cpdmGlobalStatusPeerSwStatsReportIntervalSet(interval);
}

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
L7_RC_t usmDbCpdmGlobalStatusSessionTimeoutSet(L7_uint32 timeout)
{
  return cpdmGlobalStatusSessionTimeoutSet(timeout);
}

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
L7_RC_t usmDbCpdmGlobalStatusModeGet(L7_CP_MODE_STATUS_t *mode)
{
  return cpdmGlobalStatusModeGet(mode);
}

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
L7_RC_t usmDbCpdmGlobalStatusPeerSwStatsReportIntervalGet(L7_uint32 *interval)
{
  return cpdmGlobalStatusPeerSwStatsReportIntervalGet(interval);
}

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
L7_RC_t usmDbCpdmGlobalStatusSessionTimeoutGet(L7_uint32 *timeout)
{
  return cpdmGlobalStatusSessionTimeoutGet(timeout);
}

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
L7_RC_t usmDbCpdmGlobalHTTPPortSet(L7_uint32 port)
{
  return cpdmGlobalHTTPPortSet(port);
}

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
L7_RC_t usmDbCpdmGlobalHTTPPortGet(L7_uint32 *port)
{
  return cpdmGlobalHTTPPortGet(port);
}

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
L7_RC_t usmDbCpdmGlobalHTTPSecurePort1Set(L7_uint32 securePort)
{
  return cpdmGlobalHTTPSecurePort1Set(securePort);
}
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
L7_RC_t usmDbCpdmGlobalHTTPSecurePort1Get(L7_uint32 *securePort)
{
  return cpdmGlobalHTTPSecurePort1Get(securePort);
}
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
L7_RC_t usmDbCpdmGlobalHTTPSecurePort2Set(L7_uint32 securePort)
{
  return cpdmGlobalHTTPSecurePort2Set(securePort);
}
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
L7_RC_t usmDbCpdmGlobalHTTPSecurePort2Get(L7_uint32 *securePort)
{
  return cpdmGlobalHTTPSecurePort2Get(securePort);
}
#endif

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
			      L7_uint32 * mode)
{
  return cpdmGlobalTrapModeGet(trapToTest, mode);
}

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
L7_RC_t usmDbCpdmGlobalTrapModeSet(CP_TRAP_FLAGS_t traps, L7_uint32 mode)
{
  return cpdmGlobalTrapModeSet(traps, mode);
}

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
L7_RC_t usmDbCpdmGlobalCPDisableReasonGet(L7_CP_MODE_REASON_t *reason)
{
  return cpdmGlobalCPDisableReasonGet(reason);
}

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
L7_RC_t usmDbCpdmGlobalCPIPAddressGet(L7_IP_ADDR_t *ipAddr)
{
  L7_uint32 tempIpMask;
  return cpdmGlobalCPIPAddressGet(ipAddr, &tempIpMask);
}

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
L7_RC_t usmDbCpdmGlobalSupportedCPInstancesGet(L7_uint32 *supportedCPs)
{
  return cpdmGlobalSupportedCPInstancesGet(supportedCPs);
}

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
L7_RC_t usmDbCpdmGlobalConfiguredCPInstancesGet(L7_uint32 *configCPs)
{
  return cpdmGlobalConfiguredCPInstancesGet(configCPs);
}

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
L7_RC_t usmDbCpdmGlobalActiveCPInstancesGet(L7_uint32 *activeCPs)
{
  return cpdmGlobalActiveCPInstancesGet(activeCPs);
}

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
L7_RC_t usmDbCpdmGlobalSupportedUsersGet(L7_uint32 *supportedUsers)
{
  return cpdmGlobalSupportedUsersGet(supportedUsers);
}

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
L7_RC_t usmDbCpdmGlobalLocalUsersGet(L7_uint32 *localUsers)
{
  return cpdmGlobalLocalUsersGet(localUsers);
}

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
L7_RC_t usmDbCpdmGlobalAuthenticatedUsersGet(L7_uint32 *authUsers)
{
  return cpdmGlobalAuthenticatedUsersGet(authUsers);
}

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
L7_RC_t usmDbCpdmGlobalAuthFailureMaxUsersGet(L7_uint32 *authFailSize)
{
  return cpdmGlobalAuthFailureMaxUsersGet(authFailSize);
}

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
L7_RC_t usmDbCpdmGlobalAuthFailureUsersGet(L7_uint32 *authFailUsers)
{
  return cpdmGlobalAuthFailureUsersGet(authFailUsers);
}

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
L7_RC_t usmDbCpdmGlobalActivityLogMaxEntriesGet(L7_uint32 *actLogSize)
{
  return cpdmGlobalActivityLogMaxEntriesGet(actLogSize);
}

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
L7_RC_t usmDbCpdmGlobalActivityLogEntriesGet(L7_uint32 *actLogEntries)
{
  return cpdmGlobalActivityLogEntriesGet(actLogEntries);
}

/*********************************************************************
* CP Configuration 
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigAdd(cpId_t cpId)
{
  return cpdmCPConfigAdd(cpId);
}

L7_RC_t usmDbCpdmCPConfigDelete(cpId_t cpId)
{
  return cpdmCPConfigDelete(cpId);
}

L7_RC_t usmDbCpdmCPConfigGet(cpId_t cpId)
{
  return cpdmCPConfigGet(cpId);
}

L7_RC_t usmDbCpdmCPConfigReset(cpId_t cpId)
{
  return cpdmCPConfigReset(cpId);
}



L7_RC_t usmDbCpdmCPConfigNextGet(cpId_t cpId, cpId_t * nextCpId)
{
  return cpdmCPConfigNextGet(cpId, nextCpId);
}


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

L7_RC_t usmDbCpdmCPConfigModeGet(cpId_t cpId, L7_CP_MODE_STATUS_t * mode)
{
  return cpdmCPConfigModeGet(cpId, mode);
}


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
L7_RC_t usmDbCpdmCPConfigProtocolModeGet(cpId_t cpId, L7_LOGIN_TYPE_t *protocolMode)
{
  return cpdmCPConfigProtocolModeGet(cpId, protocolMode);
}

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
L7_RC_t usmDbCpdmCPConfigVerifyModeGet(cpId_t cpId, CP_VERIFY_MODE_t *verifyMode)
{
  return cpdmCPConfigVerifyModeGet(cpId, verifyMode);
}

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
L7_RC_t usmDbCpdmCPConfigUserLogoutModeGet(cpId_t cpId, L7_uchar8 *userLogoutMode)
{
  return cpdmCPConfigUserLogoutModeGet(cpId, userLogoutMode);
}

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
L7_RC_t usmDbCpdmCPConfigRadiusAccountingGet(cpId_t cpId, L7_BOOL *radiusAccounting)
{
  return cpdmCPConfigRadiusAccountingGet(cpId, radiusAccounting);
}

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
L7_RC_t usmDbCpdmCPConfigRedirectModeGet(cpId_t cpId, L7_uchar8 *redirectMode)
{
  return cpdmCPConfigRedirectModeGet(cpId, redirectMode);
}


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
L7_RC_t usmDbCpdmCPConfigUserUpRateGet(cpId_t cpId, L7_uint32  *userUpRate)
{
  return cpdmCPConfigUserUpRateGet(cpId, userUpRate);
}

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
L7_RC_t usmDbCpdmCPConfigUserDownRateGet(cpId_t cpId, L7_uint32  *userDownRate)
{
  return cpdmCPConfigUserDownRateGet(cpId, userDownRate);
}


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
L7_RC_t usmDbCpdmCPConfigMaxInputOctetsGet(cpId_t cpId, L7_uint32 *maxInputOctets)
{
  return cpdmCPConfigMaxInputOctetsGet(cpId, maxInputOctets);
}

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
L7_RC_t usmDbCpdmCPConfigMaxOutputOctetsGet(cpId_t cpId, L7_uint32 *maxOutputOctets)
{
  return cpdmCPConfigMaxOutputOctetsGet(cpId, maxOutputOctets);
}

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
L7_RC_t usmDbCpdmCPConfigMaxTotalOctetsGet(cpId_t cpId, L7_uint32 *maxTotalOctets)
{
  return cpdmCPConfigMaxTotalOctetsGet(cpId, maxTotalOctets);
}

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
L7_RC_t usmDbCpdmCPConfigSessionTimeoutGet(cpId_t cpId, L7_uint32  *sessionTimeout)
{
  return cpdmCPConfigSessionTimeoutGet(cpId, sessionTimeout);
}

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
L7_RC_t usmDbCpdmCPConfigIdleTimeoutGet(cpId_t cpId, L7_uint32  *idleTimeout)
{
  return cpdmCPConfigIdleTimeoutGet(cpId, idleTimeout);
}

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
L7_RC_t usmDbCpdmCPConfigIntrusionThresholdGet(cpId_t cpId, L7_uint32  *intrusionThreshold)
{
  return cpdmCPConfigIntrusionThresholdGet(cpId, intrusionThreshold);
}


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
L7_RC_t usmDbCpdmCPConfigForegroundColorGet(cpId_t cpId, L7_char8 *foregroundColor)
{
  return cpdmCPConfigForegroundColorGet(cpId, foregroundColor);
}

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
L7_RC_t usmDbCpdmCPConfigBackgroundColorGet(cpId_t cpId, L7_char8 *backgroundColor)
{
  return cpdmCPConfigBackgroundColorGet(cpId, backgroundColor);
}

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
L7_RC_t usmDbCpdmCPConfigSeparatorColorGet(cpId_t cpId, L7_char8 *separatorColor)
{
  return cpdmCPConfigSeparatorColorGet(cpId, separatorColor);
}

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
L7_RC_t usmDbCpdmCPConfigNameGet(cpId_t cpId, L7_char8 *name)
{
  return cpdmCPConfigNameGet(cpId, name);
}

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
L7_RC_t usmDbCpdmCPConfigGpIdGet(cpId_t cpId, gpId_t *gpId)
{
  return cpdmCPConfigGpIdGet(cpId, gpId);
}

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
L7_RC_t usmDbCpdmCPConfigRadiusAuthServerGet(cpId_t cpId, L7_char8 *radiusServer)
{
  return cpdmCPConfigRadiusAuthServerGet(cpId, radiusServer);
}

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
L7_RC_t usmDbCpdmCPConfigRedirectURLGet(cpId_t cpId, L7_uint8  *redirectURL)
{
  return cpdmCPConfigRedirectURLGet(cpId, redirectURL);
}


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
L7_RC_t usmDbCpdmCPConfigNameSet(cpId_t cpId, L7_char8 *name) 
{
  return cpdmCPConfigNameSet(cpId, name); 
}



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
L7_RC_t usmDbCpdmCPConfigModeSet(cpId_t cpId, L7_CP_MODE_STATUS_t mode)
{
  return cpdmCPConfigModeSet(cpId, mode);
}

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
L7_RC_t usmDbCpdmCPConfigGpIdSet(cpId_t cpId, gpId_t gpId)
{
  return cpdmCPConfigGpIdSet(cpId, gpId);
}

/*************************************************************
* @purpose  Set value of field "radiusAccounting"
*
* @param cpid_t cpid @b{(input)} CP config id
*
* @param L7_char8 *radiusAccounting @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigRadiusAccountingSet(cpId_t cpId, L7_BOOL radiusAccounting)
{
  return cpdmCPConfigRadiusAccountingSet(cpId, radiusAccounting);
}

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
L7_RC_t usmDbCpdmCPConfigRadiusAuthServerSet(cpId_t cpId, L7_char8 *radiusServer)
{
  return cpdmCPConfigRadiusAuthServerSet(cpId, radiusServer);
}

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigProtocolModeSet(cpId_t cpId, L7_LOGIN_TYPE_t protocolMode)
{
  return cpdmCPConfigProtocolModeSet(cpId, protocolMode);
}

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t usmDbCpdmCPConfigVerifyModeSet(cpId_t cpId, CP_VERIFY_MODE_t verifyMode)
{
  return cpdmCPConfigVerifyModeSet(cpId, verifyMode);
}

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
L7_RC_t usmDbCpdmCPConfigUserLogoutModeSet(cpId_t cpId, L7_uchar8 userLogoutMode)
{
  return cpdmCPConfigUserLogoutModeSet(cpId, userLogoutMode);
}

/*************************************************************
* @purpose  Set value of field "redirectURL"
*
* @param cpid_t cpid @b{(input)} CP config id*
* @param L7_char8 *redirectURL @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigRedirectURLSet(cpId_t cpId, L7_char8 *redirectURL)
{
  return cpdmCPConfigRedirectURLSet(cpId, redirectURL);
}

/*************************************************************
* @purpose  Set value of field "redirectMode"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *redirectMode @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigRedirectModeSet(cpId_t cpId, L7_uchar8 redirectMode)
{
  return cpdmCPConfigRedirectModeSet(cpId, redirectMode);
}

/*************************************************************
* @purpose  Set value of field "userUpRate"
*
* @param cpid_t cpid @b{(input)} CP config id*
* @param L7_char8 *userUpRate @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigUserUpRateSet(cpId_t cpId, L7_uint32 userUpRate)
{
  return cpdmCPConfigUserUpRateSet(cpId, userUpRate);
}

/*************************************************************
* @purpose  Set value of field "userDownRate"
*
* @param cpid_t cpid @b{(input)} CP config id
  @param L7_char8 *userDownRate @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigUserDownRateSet(cpId_t cpId, L7_uint32 userDownRate)
{
  return cpdmCPConfigUserDownRateSet(cpId, userDownRate);
}

/*************************************************************
* @purpose  Set value of field "maxInputOctets"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_uint32 *maxInputOctets @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxInputOctetsSet(cpId_t cpId, L7_uint32 maxInputOctets)
{
  return cpdmCPConfigMaxInputOctetsSet(cpId, maxInputOctets);
}

/*************************************************************
* @purpose  Set value of field "maxOutputOctets"
*
* @param cpid_t cpid @b{(input)} CP config id
  @param L7_uint32 *maxOutputOctets @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxOutputOctetsSet(cpId_t cpId, L7_uint32 maxOutputOctets)
{
  return cpdmCPConfigMaxOutputOctetsSet(cpId, maxOutputOctets);
}

/*************************************************************
* @purpose  Set value of field "maxTotalOctets"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_uint32 *maxTotalOctets @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigMaxTotalOctetsSet(cpId_t cpId, L7_uint32 maxTotalOctets)
{
  return cpdmCPConfigMaxTotalOctetsSet(cpId, maxTotalOctets);
}

/*************************************************************
* @purpose  Set value of field "sessionTimeout"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *sessionTimeout @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigSessionTimeoutSet(cpId_t cpId, L7_uint32 sessionTimeout)
{
  return cpdmCPConfigSessionTimeoutSet(cpId, sessionTimeout);
}

/*************************************************************
* @purpose  Set value of field "idleTimeout"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *idleTimeout @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigIdleTimeoutSet(cpId_t cpId, L7_uint32 idleTimeout)
{
  return cpdmCPConfigIdleTimeoutSet(cpId,  idleTimeout);
}

/*************************************************************
* @purpose  Set value of field "intrusionThreshold"
*
* @param cpid_t cpid @b{(input)} CP config id
* @param L7_char8 *intrusionThreshold @b{(input)} - new value
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*************************************************************/
L7_RC_t usmDbCpdmCPConfigIntrusionThresholdSet(cpId_t cpId, L7_uint32 intrusionThreshold)
{
  return cpdmCPConfigIntrusionThresholdSet(cpId, intrusionThreshold);
}

/********************************************************************* 
* @purpose  Set value of locale field "foregroundColor"
*
* @param    cpId_t      cpId @b{(input)} CP config id
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
L7_RC_t usmDbCpdmCPConfigForegroundColorSet(cpId_t cpId, L7_char8 *foregroundColor)
{
  return cpdmCPConfigForegroundColorSet(cpId, foregroundColor);
}

/********************************************************************* 
* @purpose  Set value of locale field "backgroundColor"
*
* @param    cpId_t      cpId @b{(input)} CP config id
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
L7_RC_t usmDbCpdmCPConfigBackgroundColorSet(cpId_t cpId, L7_char8 *backgroundColor)
{
  return cpdmCPConfigBackgroundColorSet(cpId, backgroundColor);
}

/********************************************************************* 
* @purpose  Set value of locale field "separatorColor"
*
* @param    cpId_t      cpId @b{(input)} CP config id
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
L7_RC_t usmDbCpdmCPConfigSeparatorColorSet(cpId_t cpId, L7_char8 *separatorColor)
{
  return cpdmCPConfigSeparatorColorSet(cpId, separatorColor);
}

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
usmDbCpdmCPConfigIntIfNumGet(L7_uint32 cpId, L7_uint32 intIfNum)
{
  return cpdmCPConfigIntIfNumGet(cpId, intIfNum);
}



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
                                 cpId_t * pCpId, L7_uint32 * pIntIfNum)
{
  return cpdmCPConfigIntIfNumNextGet(cpId, intIfNum, pCpId, pIntIfNum);
}


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
usmDbCpdmCPConfigIntIfNumAdd(L7_uint32 cpId, L7_uint32 intIfNum)
{
  return cpdmCPConfigIntIfNumAdd(cpId, intIfNum);
}


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
usmDbCpdmCPConfigIntIfNumDelete(L7_uint32 cpId, L7_uint32 intIfNum)
{
  return cpdmCPConfigIntIfNumDelete(cpId, intIfNum);
}

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
L7_RC_t usmDbCpdmCPConfigIntIfNumFind(L7_uint32 intIfNum, cpId_t * cpId)
{
  return cpdmCPConfigIntIfNumFind(intIfNum, cpId);
}

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
L7_RC_t usmDbCpdmCPConfigIntfNotAssocNextGet(L7_uint32 intf, L7_uint32 *nextIntf)
{
  return cpdmCPConfigIntfNotAssocNextGet(intf, nextIntf);
}

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
void usmDbCpdmSetImageFileChanged(void)
{
  return cpdmSetImageFileChanged();
}

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
L7_uchar8 * usmDbCpdmImageDataGet(L7_uint32 *eoi)
{
  return cpdmImageDataGet(eoi);
}

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
L7_RC_t usmDbCpdmDecodedImageSizeSet(L7_uint32 eoi)
{
  return cpdmDecodedImageSizeSet(eoi);
}

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
L7_RC_t usmDbCpdmDecodeTextAndImageCreate(L7_uchar8 *text)
{
  return cpdmDecodeTextAndImageCreate(text);
}

