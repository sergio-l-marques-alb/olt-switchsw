/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename     cpdm_api.h
 *
 * @purpose      Captive Portal Data Manager (WDM) API header
 *
 * @component    CPDM
 *
 * @comments     APIs for usmDb layer, these APIs can be called via
 *               usmDb and captive portal components.
 *
 * @create       06/28/2007
 *
 * @author       darsenault
 *
 * @end
 *
 **********************************************************************/
#ifndef INCLUDE_CPDM_API_H
#define INCLUDE_CPDM_API_H

#include "datatypes.h"
#include "captive_portal_commdefs.h"

typedef struct cpdmCounters64_s
{
  L7_uint64 clear; /* value when stat was last cleared */
  L7_uint64 final; /* value reported to the user */
} cpdmCounters64_t;

typedef struct cpdmCounters32_s
{
  L7_uint32 clear; /* value when stat was last cleared */
  L7_uint32 final; /* value reported to the user */
} cpdmCounters32_t;

/*********************************************************************
 * Global Captive Portal Configuration
 *********************************************************************/

/*********************************************************************
*
* @purpose  Set the CP mode WITHOUT waking the CP task
*
* @param    L7_uint32 mode @b{(input)} CP global mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Should not be in this file for the long term. 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalModeApply(L7_uint32 mode, L7_BOOL force);

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
L7_RC_t cpdmGlobalModeSet(L7_uint32 mode);

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
L7_RC_t cpdmGlobalModeGet(L7_uint32 *mode);


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

L7_RC_t cpdmCfgGlobalModeGet(L7_uint32 *mode);


/*********************************************************************
*
* @purpose  Set the CP mode
*
* @param    L7_uint32 mode @b{(input)} CP global mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments NOT TO BE USED OUTSIDE CAPTIVE PORTAL NO MATTER WHAT
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalModeUpdate(L7_CP_MODE_STATUS_t newStatus);

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
L7_RC_t cpdmGlobalStatusPeerSwStatsReportIntervalSet(L7_uint32 interval);

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
L7_RC_t cpdmGlobalStatusSessionTimeoutSet(L7_uint32 timeout);

/*********************************************************************
 *
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
 *
 *********************************************************************/
L7_RC_t cpdmGlobalStatusModeGet(L7_CP_MODE_STATUS_t *mode);

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
L7_RC_t cpdmGlobalStatusPeerSwStatsReportIntervalGet(L7_uint32 *interval);

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
L7_RC_t cpdmGlobalStatusSessionTimeoutGet(L7_uint32 *timeout);

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
L7_RC_t cpdmGlobalTrapModeGet(CP_TRAP_FLAGS_t trapToTest, 
			      L7_uint32 * mode);

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
L7_RC_t cpdmGlobalHTTPPortSet(L7_uint32 port);

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Set the default HTTP secure port
*
* @param    L7_uint32 httpSecurePort @b{(input)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalHTTPSecurePort1Set(L7_uint32 httpSecurePort);
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Set the additional HTTP secure port
*
* @param    L7_uint32 httpSecurePort @b{(input)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalHTTPSecurePort2Set(L7_uint32 httpSecurePort);
#endif

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
L7_RC_t cpdmGlobalTrapModeSet(CP_TRAP_FLAGS_t traps, L7_uint32 mode);

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
L7_RC_t cpdmGlobalHTTPPortGet(L7_uint32 *port);

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Get the default HTTP secure port 1
*
* @param    L7_uint32 *httpSecurePort @b{(output)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalHTTPSecurePort1Get(L7_uint32 *httpSecurePort);
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Get the additional HTTP secure port 2
*
* @param    L7_uint32 *httpSecurePort @b{(output)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalHTTPSecurePort2Get(L7_uint32 *httpSecurePort);
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
L7_RC_t cpdmGlobalCPDisableReasonGet(L7_CP_MODE_REASON_t *reason);

/*********************************************************************
*
* @purpose  Get the CP IP address
*
* @param    L7_IP_ADDR_t *ipAddr @b{(output)} ip address
* @param    L7_uint32    *ipMask @b{(output)} ip subnet mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalCPIPAddressGet(L7_IP_ADDR_t *ipAddr, L7_uint32 *ipMask);

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
L7_RC_t cpdmGlobalSupportedCPInstancesGet(L7_uint32 *supportedCPs);

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
L7_RC_t cpdmGlobalConfiguredCPInstancesGet(L7_uint32 *configCPs);

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
L7_RC_t cpdmGlobalActiveCPInstancesGet(L7_uint32 *activeCPs);

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
L7_RC_t cpdmGlobalSupportedUsersGet(L7_uint32 *supportedUsers);

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
L7_RC_t cpdmGlobalLocalUsersGet(L7_uint32 *localUsers);

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
L7_RC_t cpdmGlobalAuthenticatedUsersGet(L7_uint32 *authUsers);

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
L7_RC_t cpdmGlobalAuthFailureMaxUsersGet(L7_uint32 *authFailSize);

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
L7_RC_t cpdmGlobalAuthFailureUsersGet(L7_uint32 *authFailUsers);

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
L7_RC_t cpdmGlobalActivityLogMaxEntriesGet(L7_uint32 *actLogSize);

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
L7_RC_t cpdmGlobalActivityLogEntriesGet(L7_uint32 *actLogEntries);


/*********************************************************************
*
* @purpose  Set the reason for the CP to be disabled
*
* @param    L7_CP_MODE_REASON_t reason @b{(input)} disable reason
*
* @returns  void
*
* @comments NOT TO BE USED OUTSIDE OF CPDM NO MATTER WHAT
*
* @end
*
*********************************************************************/
void cpdmGlobalCPDisableReasonSet(L7_CP_MODE_REASON_t reason);

/*********************************************************************
*
* @purpose  Get the CP IP address
*
* @param    L7_IP_ADDR_t ipAddr @b{(input)} ip address
* @param    L7_uint32    ipMask @b{(input)} ip subnet mask
*
* @returns  void
*
* @comments NOT TO BE EXPORTED OUTSIDE OF CPDM NO MATTER WHAT
*
* @end
*
*********************************************************************/
void cpdmGlobalCPIPAddressSet(L7_IP_ADDR_t ipAddr, 
                              L7_uint32    ipMask);


/*********************************************************************
 * CP Configuration 
 *********************************************************************/
/*********************************************************************
 *
 * @purpose  Add a new user entry
 *
 * @param    cpId_t   cpId @b{(input)} new CP ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @returns  L7_ALREADY_CONFIGURED
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigAdd(cpId_t cpId);

/*********************************************************************
*
* @purpose  Reset a CP ID entry
*
* @param    cpId_t   cpId @b{(input)} new CP ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ALREADY_CONFIGURED
*
* @comments Wipes out locales of CP ID, resets values of instance
*           to defaults, instates default locale set.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigReset(cpId_t cpId);


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
L7_RC_t cpdmCPConfigIntIfNumGet(cpId_t cpId, L7_uint32 intIfNum);



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
L7_RC_t cpdmCPConfigIntIfNumNextGet(cpId_t cpId, L7_uint32 intIfNum,
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
L7_RC_t cpdmCPConfigIntIfNumAdd(cpId_t cpId, L7_uint32 intIfNum);


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
L7_RC_t cpdmCPConfigIntIfNumDelete(cpId_t cpId, L7_uint32 intIfNum);


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
L7_RC_t cpdmCPConfigIntIfNumFind(L7_uint32 intIfNum, cpId_t * cpId);

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
L7_RC_t cpdmCPConfigIntfNotAssocNextGet(L7_uint32 intf, L7_uint32 *nextIntf);




/*********************************************************************
*
* @purpose  Add a default cpId, WebID entries
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments returns L7_SUCCESS if defaults were either added 
*           successfully OR already present
*
* @end
*
*********************************************************************/

L7_RC_t cpdmCPConfigAddDefaults(void);

/*********************************************************************
 *
 * @purpose  Delete existing cp config entry
 *
 * @param    cpId_t cpId @b{(input)} ID of victim
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigDelete(cpId_t cpId);

/*********************************************************************
 *
 * @purpose  Check for existing config data
 *
 * @param    cpId_t cpId @b{(input)} CP ID to find
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigGet(cpId_t cpId);


/*********************************************************************
 *
 * @purpose  Get the next config entry
 *
 * @param    cpId_t cpId @b{(input)} current CP ID 
 * @param    cpId_t * nextCpId @b{(output)} next CP ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigNextGet(cpId_t cpId, cpId_t * nextCpId);


/********************************************************************* 
* @purpose  Get CP config name
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *groupName @b{(output)} - corresponds to
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
L7_RC_t cpdmCPConfigNameGet(cpId_t cpId, L7_char8 *name);

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

L7_RC_t cpdmCPConfigModeGet(cpId_t cpId, L7_CP_MODE_STATUS_t * mode);

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
L7_RC_t cpdmCPConfigProtocolModeGet(cpId_t cpId, L7_LOGIN_TYPE_t *protocolMode);

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
L7_RC_t cpdmCPConfigVerifyModeGet(cpId_t cpId, CP_VERIFY_MODE_t *verifyMode);

/*********************************************************************
*
* @purpose  Get CP config user logout mode
*
* @param    cpId_t cpId @b{(input)} CP config id
* @param    L7_uint32 *userLogoutMode @b{(output)} logout request mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigUserLogoutModeGet(cpId_t cpId, L7_uchar8 *userLogoutMode);

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
L7_RC_t cpdmCPConfigGpIdGet(cpId_t cpId, gpId_t *gpId);

/********************************************************************* 
 * @purpose  Get CP config radiusAccounting
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_BOOL *radiusAccounting @b{(output)} radiusAccounting of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigRadiusAccountingGet(cpId_t cpId, L7_BOOL *radiusAccounting);

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
L7_RC_t cpdmCPConfigRadiusAuthServerGet(cpId_t cpId, L7_char8 *radiusServer);

/********************************************************************* 
 * @purpose  Get CP config redirectURL
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_char8 *redirectURL @b{(output)} - corresponds to
 *           "redirectURL" of config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigRedirectURLGet(cpId_t cpId, L7_char8 *redirectURL);

/********************************************************************* 
 * @purpose  Get CP config redirectMode
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uchar8 *redirectMode @b{(output)} redirectMode of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

  L7_RC_t cpdmCPConfigRedirectModeGet(cpId_t cpId, L7_uchar8 *redirectMode);

/********************************************************************* 
 * @purpose  Get CP config userUpRate
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 *userUpRate @b{(output)} userUpRate of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigUserUpRateGet(cpId_t cpId, L7_uint32 *userUpRate);

/********************************************************************* 
 * @purpose  Get CP config userDownRate
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 *userDownRate @b{(output)} userDownRate of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigUserDownRateGet(cpId_t cpId, L7_uint32 *userDownRate);

/********************************************************************* 
 * @purpose  Get CP config maxInputOctets
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 *maxInputOctets @b{(output)} maxInputOctets of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigMaxInputOctetsGet(cpId_t cpId, L7_uint32 *maxInputOctets);

/********************************************************************* 
 * @purpose  Get CP config maxOutputOctets
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 *maxOutputOctets @b{(output)} maxOutputOctets of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigMaxOutputOctetsGet(cpId_t cpId, L7_uint32 *maxOutputOctets);

/********************************************************************* 
 * @purpose  Get CP config maxTotalOctets
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 *maxTotalOctets @b{(output)} maxTotalOctets of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigMaxTotalOctetsGet(cpId_t cpId, L7_uint32 *maxTotalOctets);

/********************************************************************* 
 * @purpose  Get CP config sessionTimeout
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 *sessionTimeout @b{(output)} sessionTimeout of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigSessionTimeoutGet(cpId_t cpId, L7_uint32 *sessionTimeout);

/********************************************************************* 
 * @purpose  Get CP config idleTimeout
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 *idleTimeout @b{(output)} idleTimeout of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigIdleTimeoutGet(cpId_t cpId, L7_uint32 *idleTimeout);

/********************************************************************* 
 * @purpose  Get CP config intrusionThreshold
 *
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 *intrusionThreshold @b{(output)} intrusionThreshold of 
 *           config item with given ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmCPConfigIntrusionThresholdGet(cpId_t cpId, L7_uint32 *intrusionThreshold);


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
L7_RC_t cpdmCPConfigForegroundColorGet(cpId_t cpId, L7_char8 *foregroundColor);

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
L7_RC_t cpdmCPConfigBackgroundColorGet(cpId_t cpId, L7_char8 *backgroundColor);

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
L7_RC_t cpdmCPConfigSeparatorColorGet(cpId_t cpId, L7_char8 *separatorColor);

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

L7_RC_t cpdmCPConfigModeSet(cpId_t cpId, L7_CP_MODE_STATUS_t mode);


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

L7_RC_t cpdmCPConfigNameSet(cpId_t cpId, L7_char8 *name);

/*********************************************************************
 *
 * @purpose  Set CP config protocol mode
 *
 * @param    cpId_t cpId @b{(input)} CP config id
 * @param    L7_LOGIN_TYPE_t *protocolMode@b{(input)} protocol mode
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigProtocolModeSet(cpId_t cpId, L7_LOGIN_TYPE_t protocolMode);

/*********************************************************************
 *
 * @purpose  Set CP config verify mode
 *
 * @param    cpId_t cpId @b{(input)} CP config id
 * @param    CP_VERIFY_MODE_t *verifyMode@b{(input)} verify mode
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigVerifyModeSet(cpId_t cpId, CP_VERIFY_MODE_t verifyMode);

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
L7_RC_t cpdmCPConfigUserLogoutModeSet(cpId_t cpId, L7_uchar8 userLogoutMode);

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
L7_RC_t cpdmCPConfigGpIdSet(cpId_t cpId, gpId_t gpId);

/********************************************************************* 
 * @purpose  Set CP config radiusAccounting
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_BOOL radiusAccounting @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigRadiusAccountingSet(cpId_t cpId, L7_BOOL radiusAccounting);

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
L7_RC_t cpdmCPConfigRadiusAuthServerSet(cpId_t cpId, L7_char8 *radiusServer);

/********************************************************************* 
 * @purpose  Set CP config redirectURL
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_char8 *redirectURL @b{(input)} - new
 *           "redirectURL" of config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigRedirectURLSet(cpId_t cpId, L7_char8 *redirectURL);

/********************************************************************* 
 * @purpose  Set CP config redirectMode
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uchar8 redirectMode @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigRedirectModeSet(cpId_t cpId, L7_uchar8 redirectMode);

/********************************************************************* 
 * @purpose  Set CP config userUpRate
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 userUpRate @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigUserUpRateSet(cpId_t cpId, L7_uint32 userUpRate);

/********************************************************************* 
 * @purpose  Set CP config userDownRate
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 userDownRate @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigUserDownRateSet(cpId_t cpId, L7_uint32 userDownRate);

/********************************************************************* 
 * @purpose  Set CP config maxInputOctets
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 maxInputOctets @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigMaxInputOctetsSet(cpId_t cpId, L7_uint32 maxInputOctets);


/********************************************************************* 
 * @purpose  Set CP config maxOutputOctets
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 maxOutputOctets @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigMaxOutputOctetsSet(cpId_t cpId, L7_uint32 maxOutputOctets);

/********************************************************************* 
 * @purpose  Set CP config maxTotalOctets
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 maxTotalOctets @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigMaxTotalOctetsSet(cpId_t cpId, L7_uint32 maxTotalOctets);

/********************************************************************* 
 * @purpose  Set CP config sessionTimeout
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 sessionTimeout @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigSessionTimeoutSet(cpId_t cpId, L7_uint32 sessionTimeout);

/********************************************************************* 
 * @purpose  Set CP config idleTimeout
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 idleTimeout @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigIdleTimeoutSet(cpId_t cpId, L7_uint32 idleTimeout);

/********************************************************************* 
 * @purpose  Set CP config intrusionThreshold
 * @param    cpId_t  cpId @b{(input)} CP config id
 * @param    L7_uint32 intrusionThreshold @b{(input)} value for
 *           config item with given ID
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @comments none
 * @end
 *********************************************************************/
L7_RC_t cpdmCPConfigIntrusionThresholdSet(cpId_t cpId, L7_uint32 intrusionThreshold);

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
L7_RC_t cpdmCPConfigForegroundColorSet(cpId_t cpId, L7_char8 *foregroundColor);

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
L7_RC_t cpdmCPConfigBackgroundColorSet(cpId_t cpId, L7_char8 *backgroundColor);

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
L7_RC_t cpdmCPConfigSeparatorColorSet(cpId_t cpId, L7_char8 *separatorColor);

/*********************************************************************
*
* @purpose  Get the operational status of a CP instance.
*
* @param    cpid_t                   cpid    @b{(input)} CP config id
* @param    L7_CP_INST_OPER_STATUS_t *reason @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigOperStatusGet(cpId_t cpId, L7_CP_INST_OPER_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the disable reason of a disabled CP instance.
*
* @param    cpid_t                      cpid    @b{(input)} CP config id
* @param    L7_CP_INST_DISABLE_REASON_t *reason @b{(output)} disable reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigDisableReasonGet(cpId_t cpId, L7_CP_INST_DISABLE_REASON_t *reason);

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
L7_RC_t cpdmCPConfigBlockedStatusGet(cpId_t cpId, L7_CP_INST_BLOCK_STATUS_t *status);

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
L7_RC_t cpdmCPConfigAuthenticatedUsersGet(cpId_t cpId, L7_uint32 *authUsers);

/*********************************************************************
*
* @purpose  Update config count 
*
* @param    void
*
* @returns  void
*
* @comments updates count of active configurations after save/restore
*
* @end
*
*********************************************************************/
void cpdmCPConfigCountUpdate(void);



/*********************************************************************
*
* @purpose  Update interface count 
*
* @param    void
*
* @returns  void
*
* @comments updates count of active intf's after save/restore
*
* @end
*
*********************************************************************/
void cpdmCPIntfCountUpdate(void);

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
void cpdmSetImageFileChanged(void);

/*********************************************************************
*
* @purpose  Create CP image files on OS RAM from CP data structure
*
* @param    void
* 
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCopyImageDataToFs(L7_uchar8 *images);

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
L7_uchar8 * cpdmImageDataGet(L7_uint32 *eoi);

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
L7_RC_t cpdmDecodedImageSizeSet(L7_uint32 eoi);

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
L7_RC_t cpdmDecodeTextAndImageCreate(L7_uchar8 *text);



#endif /* INCLUDE_CPDM_API_H */

