/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* Name: k_mib_captive_portal.c
*
* Purpose: System-specific code for Captive Portal Private MIB
*
* Created: 07/25/2007
*
* Author: wjacobs, rjindal
*
* Component: SNMP
*
*********************************************************************/

#include "k_private_base.h"
#include "sr_ip.h"
#include "k_mib_captive_portal_api.h"
#include "captive_portal_defaultconfig.h"
#include "usmdb_common.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "usmdb_cpdm_user_api.h"
#include "usmdb_cpim_api.h"
#include "usmdb_radius_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_sslt_api.h"
#include "sslt_exports.h"

cpGlobalConfigGroup_t *
k_cpGlobalConfigGroup_get(int serialNum, ContextInfo *contextInfo, 
                          int nominator)
{
  static cpGlobalConfigGroup_t cpGlobalConfigGroupData;

  ZERO_VALID(cpGlobalConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_cpAdminMode:
    if (snmpCpdmGlobalModeGet(&cpGlobalConfigGroupData.cpAdminMode) == L7_SUCCESS)
      SET_VALID(nominator, cpGlobalConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_cpAdditionalHttpPort:
    if (usmDbCpdmGlobalHTTPPortGet(&cpGlobalConfigGroupData.cpAdditionalHttpPort) == L7_SUCCESS)
      SET_VALID(nominator, cpGlobalConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

#ifdef L7_MGMT_SECURITY_PACKAGE
  case I_cpAdditionalHttpSecurePort:
    if (usmDbCpdmGlobalHTTPSecurePort2Get(&cpGlobalConfigGroupData.cpAdditionalHttpSecurePort) == L7_SUCCESS)
      SET_VALID(nominator, cpGlobalConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif

#ifdef I_cpPeerStatsReportingInterval
  case I_cpPeerStatsReportingInterval:
    if ((usmDbCpdmClusterSupportGet() == L7_SUCCESS) &&
        (usmDbCpdmGlobalStatusPeerSwStatsReportIntervalGet(&cpGlobalConfigGroupData.cpPeerStatsReportingInterval) == L7_SUCCESS))
      SET_VALID(nominator, cpGlobalConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_cpPeerStatsReportingInterval */

  case I_cpAuthTimeout:
    if (usmDbCpdmGlobalStatusSessionTimeoutGet(&cpGlobalConfigGroupData.cpAuthTimeout) == L7_SUCCESS)
      SET_VALID(nominator, cpGlobalConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if ((nominator >= 0) && !VALID(nominator, cpGlobalConfigGroupData.valid))
    return(NULL);

  return(&cpGlobalConfigGroupData);
}

#ifdef SETS
int
k_cpGlobalConfigGroup_test(ObjectInfo *object, ObjectSyntax *value, doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpGlobalConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpGlobalConfigGroup_set(cpGlobalConfigGroup_t *data, ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_cpAdminMode, data->valid))
  {
    if (snmpCpdmGlobalModeSet(data->cpAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpAdminMode, tempValid);
    }
  }

#ifdef I_cpPeerStatsReportingInterval
  if (VALID(I_cpPeerStatsReportingInterval, data->valid))
  {
    if ((usmDbCpdmClusterSupportGet() != L7_SUCCESS) ||
        ((data->cpPeerStatsReportingInterval < CP_PS_STATS_REPORT_INTERVAL_MIN) &&
         (data->cpPeerStatsReportingInterval != 0)) ||
        (data->cpPeerStatsReportingInterval > CP_PS_STATS_REPORT_INTERVAL_MAX) ||
        (usmDbCpdmGlobalStatusPeerSwStatsReportIntervalSet(data->cpPeerStatsReportingInterval) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpPeerStatsReportingInterval, tempValid);
    }
  }
#endif /* I_cpPeerStatsReportingInterval */

  if (VALID(I_cpAuthTimeout, data->valid))
  {
    if ((data->cpAuthTimeout < CP_AUTH_SESSION_TIMEOUT_MIN) || 
        (data->cpAuthTimeout > CP_AUTH_SESSION_TIMEOUT_MAX) || 
        (usmDbCpdmGlobalStatusSessionTimeoutSet(data->cpAuthTimeout) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpAuthTimeout, tempValid);
    }
  }

  if (VALID(I_cpAdditionalHttpPort, data->valid))
  {
    if ((data->cpAdditionalHttpPort < CP_HTTP_PORT_MIN) || 
        (data->cpAdditionalHttpPort > CP_HTTP_PORT_MAX) || 
        (usmDbCpdmGlobalHTTPPortSet(data->cpAdditionalHttpPort) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpAdditionalHttpPort, tempValid);
    }
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (VALID(I_cpAdditionalHttpSecurePort, data->valid))
  {
    L7_uint32 currentPort;
    L7_uint32 currentDefaultSecurePort;
    L7_uint32 currentAdditionalSecurePort;
    L7_uint32 currentAdminSecurePort;
    L7_uint32 unit = usmDbThisUnitGet();

    if (L7_SUCCESS != usmDbCpdmGlobalHTTPPortGet(&currentPort))
    {
      currentPort = CP_HTTP_PORT_MIN;
    }
    if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort1Get(&currentDefaultSecurePort))
    {
      currentDefaultSecurePort = L7_SSLT_SECURE_PORT;
    }
    if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort2Get(&currentAdditionalSecurePort))
    {
      currentAdditionalSecurePort = FD_CP_HTTP_AUX_SECURE_PORT2;
    }
    if (L7_SUCCESS != usmDbssltSecurePortGet(unit,&currentAdminSecurePort))
    {
      currentAdminSecurePort = FD_SSLT_SECURE_PORT;
    }

    if ((data->cpAdditionalHttpSecurePort != 0) &&
        (data->cpAdditionalHttpSecurePort != currentPort) &&
        ((data->cpAdditionalHttpSecurePort ==  CP_STANDARD_HTTP_PORT)
         || (data->cpAdditionalHttpSecurePort == currentDefaultSecurePort)
         || (data->cpAdditionalHttpSecurePort == currentAdditionalSecurePort)
         || (data->cpAdditionalHttpSecurePort == currentAdminSecurePort)
         || (usmDbCpdmGlobalHTTPSecurePort2Set(data->cpAdditionalHttpSecurePort) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpAdditionalHttpSecurePort, tempValid);
    }
  }
#endif

  return NO_ERROR;
}

#ifdef SR_cpGlobalConfigGroup_UNDO
/* add #define SR_cpGlobalConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the cpGlobalConfigGroup family.
 */
int
cpGlobalConfigGroup_undo(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  cpGlobalConfigGroup_t *data = (cpGlobalConfigGroup_t *) doCur->data;
  cpGlobalConfigGroup_t *undodata = (cpGlobalConfigGroup_t *) doCur->undodata;
  cpGlobalConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  if (data == NULL || undodata == NULL)
    return UNDO_FAILED_ERROR;

  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  setdata = undodata;
  function = SR_ADD_MODIFY;

  if ((setdata != NULL) && (k_cpGlobalConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpGlobalConfigGroup_UNDO */
#endif /* SETS */


cpCaptivePortalEntry_t *
k_cpCaptivePortalEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 cpCaptivePortalInstanceId)
{
  static cpCaptivePortalEntry_t cpCaptivePortalEntry;
  static L7_BOOL firstTime = L7_TRUE;
  cpId_t cpId;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 url[CP_WELCOME_URL_MAX+1];
  L7_char8 radiusServer[CP_RADIUS_AUTH_SERVER_MAX+1];

  if (firstTime == L7_TRUE)
  {
    cpCaptivePortalEntry.cpCaptivePortalRedirectURL = MakeOctetString(NULL, 0);
    cpCaptivePortalEntry.cpCaptivePortalRadiusAuthServer = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(cpCaptivePortalEntry.valid);

  cpCaptivePortalEntry.cpCaptivePortalInstanceId = cpCaptivePortalInstanceId;
  cpId = (cpId_t)cpCaptivePortalEntry.cpCaptivePortalInstanceId;
  SET_VALID(I_cpCaptivePortalInstanceId, cpCaptivePortalEntry.valid);

  if ((searchType == EXACT) ? 
      usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS : 
      (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS && 
       usmDbCpdmCPConfigNextGet(cpId, &cpId) != L7_SUCCESS))
  {
    ZERO_VALID(cpCaptivePortalEntry.valid);
    return(NULL);
  }

  cpCaptivePortalEntry.cpCaptivePortalInstanceId = (L7_uint32)cpId;

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_cpCaptivePortalInstanceId:
    if (nominator != -1) break;
       /* else pass through */

  case I_cpCaptivePortalConfigName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbCpdmCPConfigNameGet(cpId, snmp_buffer) == L7_SUCCESS && 
        (SafeMakeOctetStringFromTextExact(&cpCaptivePortalEntry.cpCaptivePortalConfigName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_cpCaptivePortalConfigName, cpCaptivePortalEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalAdminMode:
    if (snmpCpdmCPConfigModeGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalAdminMode) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalAdminMode, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_cpCaptivePortalProtocolMode:
    if (snmpCpdmCPConfigProtocolModeGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalProtocolMode) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalProtocolMode, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_cpCaptivePortalVerificationMode:
    if (snmpCpdmCPConfigVerifyModeGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalVerificationMode) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalVerificationMode, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_cpCaptivePortalUserGroup:
    if (snmpCpdmCPConfigGroupIDGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalUserGroup) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalUserGroup, cpCaptivePortalEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

#ifdef I_cpCaptivePortalUserLogoutMode
  case I_cpCaptivePortalUserLogoutMode:
    if (snmpCpdmCPConfigUserLogoutModeGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalUserLogoutMode) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalUserLogoutMode, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_cpCaptivePortalUserLogoutMode */

  case I_cpCaptivePortalURLRedirectMode:
    if (snmpCpdmCPConfigRedirectModeGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalURLRedirectMode) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalURLRedirectMode, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_cpCaptivePortalRedirectURL:
    bzero(url, CP_WELCOME_URL_MAX+1);
    if ((snmpCpdmCPConfigRedirectURLGet(cpId, url) == L7_SUCCESS) && 
        (SafeMakeOctetStringFromTextExact(&cpCaptivePortalEntry.cpCaptivePortalRedirectURL, url) == L7_TRUE))
      SET_VALID(I_cpCaptivePortalRedirectURL, cpCaptivePortalEntry.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpCaptivePortalRadiusAuthServer:
    bzero(radiusServer, CP_RADIUS_AUTH_SERVER_MAX+1);
    if ((usmDbCpdmCPConfigRadiusAuthServerGet(cpId, radiusServer) == L7_SUCCESS) && 
        (SafeMakeOctetStringFromTextExact(&cpCaptivePortalEntry.cpCaptivePortalRadiusAuthServer, radiusServer) == L7_TRUE))
      SET_VALID(I_cpCaptivePortalRadiusAuthServer, cpCaptivePortalEntry.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpCaptivePortalSessionTimeout:
    if (usmDbCpdmCPConfigSessionTimeoutGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalSessionTimeout) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalSessionTimeout, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */

#ifdef I_cpCaptivePortalIdleTimeout
  case I_cpCaptivePortalIdleTimeout:
    if (usmDbCpdmCPConfigIdleTimeoutGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalIdleTimeout) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalIdleTimeout, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_cpCaptivePortalIdleTimeout */

#if 0
  /* Not yet supported */
  case I_cpCaptivePortalRadiusAccountingMode:
    if (snmpCpdmCPConfigRadiusAccountingGet(cpId, (L7_BOOL *) &cpCaptivePortalEntry.cpCaptivePortalRadiusAccountingMode) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalRadiusAccountingMode, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif

#ifdef I_cpCaptivePortalMaxBandwidthUp
  case I_cpCaptivePortalMaxBandwidthUp:
    if (usmDbCpdmCPConfigUserUpRateGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalMaxBandwidthUp) == L7_SUCCESS)
    {
      /* Convert to bytes */
      cpCaptivePortalEntry.cpCaptivePortalMaxBandwidthUp = cpCaptivePortalEntry.cpCaptivePortalMaxBandwidthUp/8;
      SET_VALID(I_cpCaptivePortalMaxBandwidthUp, cpCaptivePortalEntry.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_cpCaptivePortalMaxBandwidthUp */

#ifdef I_cpCaptivePortalMaxBandwidthDown
  case I_cpCaptivePortalMaxBandwidthDown:
    if (usmDbCpdmCPConfigUserDownRateGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalMaxBandwidthDown) == L7_SUCCESS)
    {
      /* Convert to bytes */
      cpCaptivePortalEntry.cpCaptivePortalMaxBandwidthDown = cpCaptivePortalEntry.cpCaptivePortalMaxBandwidthDown/8;
      SET_VALID(I_cpCaptivePortalMaxBandwidthDown, cpCaptivePortalEntry.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_cpCaptivePortalMaxBandwidthDown */

#ifdef I_cpCaptivePortalMaxInputOctets
  case I_cpCaptivePortalMaxInputOctets:
    if (usmDbCpdmCPConfigMaxInputOctetsGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalMaxInputOctets) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalMaxInputOctets, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_cpCaptivePortalMaxInputOctets */

#ifdef I_cpCaptivePortalMaxOutputOctets
  case I_cpCaptivePortalMaxOutputOctets:
    if (usmDbCpdmCPConfigMaxOutputOctetsGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalMaxOutputOctets) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalMaxOutputOctets, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_cpCaptivePortalMaxOutputOctets */

#ifdef I_cpCaptivePortalMaxTotalOctets
  case I_cpCaptivePortalMaxTotalOctets:
    if (usmDbCpdmCPConfigMaxTotalOctetsGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalMaxTotalOctets) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalMaxTotalOctets, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif

#if 0
  /* Not yet supported */
  case I_cpCaptivePortalIntrusionThreshold:
    if (usmDbCpdmCPConfigIntrusionThresholdGet(cpId, &cpCaptivePortalEntry.cpCaptivePortalIntrusionThreshold) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalIntrusionThreshold, cpCaptivePortalEntry.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif

  case I_cpCaptivePortalRowStatus:
    /* if entry shows up, it's active */
    cpCaptivePortalEntry.cpCaptivePortalRowStatus = D_cpCaptivePortalRowStatus_active;
    SET_VALID(I_cpCaptivePortalRowStatus, cpCaptivePortalEntry.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, cpCaptivePortalEntry.valid) )
    return(NULL);

  return(&cpCaptivePortalEntry);
}

#ifdef SETS
int
k_cpCaptivePortalEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpCaptivePortalEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpCaptivePortalEntry_set_defaults(doList_t *dp)
{
  cpCaptivePortalEntry_t *data = (cpCaptivePortalEntry_t *) (dp->data);

  if ((data->cpCaptivePortalConfigName = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->cpCaptivePortalAdminMode = D_cpCaptivePortalAdminMode_enable;
  data->cpCaptivePortalProtocolMode = D_cpCaptivePortalProtocolMode_http;
  data->cpCaptivePortalVerificationMode = D_cpCaptivePortalVerificationMode_guest;
  data->cpCaptivePortalUserGroup = 1;
  data->cpCaptivePortalURLRedirectMode = D_cpCaptivePortalURLRedirectMode_disable;
  if ((data->cpCaptivePortalRedirectURL = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->cpCaptivePortalRadiusAuthServer = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

#if 0
  data->cpCaptivePortalRadiusAccountingMode = D_cpCaptivePortalRadiusAccountingMode_disable;
  data->cpCaptivePortalIntrusionThreshold = 0;
#endif

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_cpCaptivePortalEntry_set(cpCaptivePortalEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_BOOL isNewRowCreated = L7_FALSE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 mode;
  cpId_t cpId;
  gpId_t gId;

  bzero(tempValid, sizeof(tempValid));

  if ((data->cpCaptivePortalInstanceId == L7_NULL) || (data->cpCaptivePortalInstanceId > CP_ID_MAX))
    return COMMIT_FAILED_ERROR;

  cpId = (cpId_t)data->cpCaptivePortalInstanceId;

  if (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS)
  {
    /* create a new entry */
    if (VALID(I_cpCaptivePortalRowStatus, data->valid))
    {
      if ((data->cpCaptivePortalRowStatus == D_cpCaptivePortalRowStatus_createAndGo) && 
          (usmDbCpdmCPConfigAdd(cpId) != L7_SUCCESS))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_cpCaptivePortalRowStatus, tempValid);
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (data->cpCaptivePortalConfigName != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->cpCaptivePortalConfigName->octet_ptr, data->cpCaptivePortalConfigName->length);
    if (VALID(I_cpCaptivePortalConfigName, data->valid))
    {
      if (usmDbCpdmCPConfigNameSet(cpId, snmp_buffer) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_cpCaptivePortalConfigName, tempValid);
      }
    }
  }

  if (VALID(I_cpCaptivePortalAdminMode, data->valid))
  {
    if (snmpCpdmCPConfigModeSet(cpId, data->cpCaptivePortalAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalAdminMode, tempValid);
    }
  }

  if (VALID(I_cpCaptivePortalProtocolMode, data->valid))
  {
    if (snmpCpdmCPConfigProtocolModeSet(cpId, data->cpCaptivePortalProtocolMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalProtocolMode, tempValid);
    }
  }

  if (VALID(I_cpCaptivePortalVerificationMode, data->valid))
  {
    if (snmpCpdmCPConfigVerifyModeSet(cpId, data->cpCaptivePortalVerificationMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalVerificationMode, tempValid);
    }
  }

  if (VALID(I_cpCaptivePortalUserGroup, data->valid))
  {
    gId = (gpId_t)data->cpCaptivePortalUserGroup;

    if ((usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS) || 
        (usmDbCpdmCPConfigGpIdSet(cpId, gId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_cpCaptivePortalUserGroup, tempValid);
    }
  }

#ifdef I_cpCaptivePortalUserLogoutMode
  if (VALID(I_cpCaptivePortalUserLogoutMode, data->valid))
  {
    if (snmpCpdmCPConfigUserLogoutModeSet(cpId, data->cpCaptivePortalUserLogoutMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalUserLogoutMode, tempValid);
    }
  }
#endif /* I_cpCaptivePortalUserLogoutMode */

  if (VALID(I_cpCaptivePortalURLRedirectMode, data->valid))
  {
    if (snmpCpdmCPConfigRedirectModeSet(cpId, data->cpCaptivePortalURLRedirectMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalURLRedirectMode, tempValid);
    }
  }

  if (data->cpCaptivePortalRedirectURL != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->cpCaptivePortalRedirectURL->octet_ptr, data->cpCaptivePortalRedirectURL->length);
    if (VALID(I_cpCaptivePortalRedirectURL, data->valid))
    {
      if ((snmpCpdmCPConfigRedirectModeGet(cpId, &mode) != L7_SUCCESS) || 
          (mode != D_cpCaptivePortalURLRedirectMode_enable) || 
          (usmDbCpdmCPConfigRedirectURLSet(cpId, snmp_buffer) != L7_SUCCESS))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_cpCaptivePortalRedirectURL, tempValid);
      }
    }
  }

  if (data->cpCaptivePortalRadiusAuthServer != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->cpCaptivePortalRadiusAuthServer->octet_ptr, data->cpCaptivePortalRadiusAuthServer->length);
    if (VALID(I_cpCaptivePortalRadiusAuthServer, data->valid))
    {
      L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
      L7_uint32 unit;
      L7_uint32 val;
      L7_RC_t rc;

      /* Verify that this server is configured in Fastpath */
      unit = usmDbThisUnitGet();
      addrType = L7_IP_ADDRESS_TYPE_IPV4;
      rc = usmDbRadiusHostNameServerEntryTypeGet(unit, snmp_buffer, addrType, &val);
      if (L7_SUCCESS != rc)
      {
        addrType = L7_IP_ADDRESS_TYPE_DNS;
        rc = usmDbRadiusHostNameServerEntryTypeGet(unit, snmp_buffer, addrType, &val);
      }
      if (L7_SUCCESS != rc)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(RESOURCE_UNAVAILABLE_ERROR);
      }
      if (usmDbCpdmCPConfigRadiusAuthServerSet(cpId, snmp_buffer) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_cpCaptivePortalRadiusAuthServer, tempValid);
      }
    }
  }

  if (VALID(I_cpCaptivePortalSessionTimeout, data->valid))
  {
    if ((data->cpCaptivePortalSessionTimeout < CP_USER_LOCAL_SESSION_TIMEOUT_MIN) || 
        (data->cpCaptivePortalSessionTimeout > CP_USER_LOCAL_SESSION_TIMEOUT_MAX) || 
        (usmDbCpdmCPConfigSessionTimeoutSet(cpId, data->cpCaptivePortalSessionTimeout) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalSessionTimeout, tempValid);
    }
  }

#ifdef I_cpCaptivePortalIdleTimeout
  if (VALID(I_cpCaptivePortalIdleTimeout, data->valid))
  {
    if ((data->cpCaptivePortalIdleTimeout < CP_USER_LOCAL_IDLE_TIMEOUT_MIN) || 
        (data->cpCaptivePortalIdleTimeout > CP_USER_LOCAL_IDLE_TIMEOUT_MAX) || 
        (usmDbCpdmCPConfigIdleTimeoutSet(cpId, data->cpCaptivePortalIdleTimeout) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalIdleTimeout, tempValid);
    }
  }
#endif /* I_cpCaptivePortalIdleTimeout */

#if 0
  /* Not yet supported */
  if (VALID(I_cpCaptivePortalRadiusAccountingMode, data->valid))
  {
    if (snmpCpdmCPConfigRadiusAccountingSet(cpId, data->cpCaptivePortalRadiusAccountingMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalRadiusAccountingMode, tempValid);
    }
  }
#endif

#ifdef I_cpCaptivePortalMaxBandwidthUp
  if (VALID(I_cpCaptivePortalMaxBandwidthUp, data->valid))
  {
    if (usmDbCpdmCPConfigUserUpRateSet(cpId, (data->cpCaptivePortalMaxBandwidthUp*8)) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalMaxBandwidthUp, tempValid);
    }
  }
#endif /* I_cpCaptivePortalMaxBandwidthUp */

#ifdef I_cpCaptivePortalMaxBandwidthDown
  if (VALID(I_cpCaptivePortalMaxBandwidthDown, data->valid))
  {
    if (usmDbCpdmCPConfigUserDownRateSet(cpId, (data->cpCaptivePortalMaxBandwidthDown*8)) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalMaxBandwidthDown, tempValid);
    }
  }
#endif /* I_cpCaptivePortalMaxBandwidthDown */

#ifdef I_cpCaptivePortalMaxInputOctets
  if (VALID(I_cpCaptivePortalMaxInputOctets, data->valid))
  {
    if (usmDbCpdmCPConfigMaxInputOctetsSet(cpId, data->cpCaptivePortalMaxInputOctets) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalMaxInputOctets, tempValid);
    }
  }
#endif /* I_cpCaptivePortalMaxInputOctets */

#ifdef I_cpCaptivePortalMaxOutputOctets
  if (VALID(I_cpCaptivePortalMaxOutputOctets, data->valid))
  {
    if (usmDbCpdmCPConfigMaxOutputOctetsSet(cpId, data->cpCaptivePortalMaxOutputOctets) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalMaxOutputOctets, tempValid);
    }
  }
#endif /* I_cpCaptivePortalMaxOutputOctets */

#ifdef I_cpCaptivePortalMaxTotalOctets
  if (VALID(I_cpCaptivePortalMaxTotalOctets, data->valid))
  {
    if (usmDbCpdmCPConfigMaxTotalOctetsSet(cpId, data->cpCaptivePortalMaxTotalOctets) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalMaxTotalOctets, tempValid);
    }
  }
#endif /* I_cpCaptivePortalMaxTotalOctets */

#if 0
  /* Not yet supported */
  if (VALID(I_cpCaptivePortalIntrusionThreshold, data->valid))
  {
    if (usmDbCpdmCPConfigIntrusionThresholdSet(cpId, data->cpCaptivePortalIntrusionThreshold) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalIntrusionThreshold, tempValid);
    }
  }
#endif

  if (VALID(I_cpCaptivePortalRowStatus, data->valid) && 
      !(data->cpCaptivePortalRowStatus == D_cpCaptivePortalRowStatus_createAndGo))
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if ((data->cpCaptivePortalRowStatus != D_cpCaptivePortalRowStatus_active) && 
        (isNewRowCreated != L7_TRUE) && 
        (data->cpCaptivePortalRowStatus != D_cpCaptivePortalRowStatus_destroy || 
         usmDbCpdmCPConfigDelete(cpId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_cpCaptivePortalEntry_UNDO
/* add #define SR_cpCaptivePortalEntry_UNDO in sitedefs.h to
 * include the undo routine for the cpCaptivePortalEntry family.
 */
int
cpCaptivePortalEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  cpCaptivePortalEntry_t *data = (cpCaptivePortalEntry_t *) doCur->data;
  cpCaptivePortalEntry_t *undodata = (cpCaptivePortalEntry_t *) doCur->undodata;
  cpCaptivePortalEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /* Copy valid bits from data to undodata */
  if (undodata != NULL && data != NULL)
    memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if (undodata == NULL)
  {
    /* undoing an add, so delete */
    data->cpCaptivePortalRowStatus = D_cpCaptivePortalRowStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if (undodata->cpCaptivePortalRowStatus == D_cpCaptivePortalRowStatus_notReady || 
        undodata->cpCaptivePortalRowStatus == D_cpCaptivePortalRowStatus_notInService)
    {
      undodata->cpCaptivePortalRowStatus = D_cpCaptivePortalRowStatus_createAndWait;
    }
    else
    {
      if (undodata->cpCaptivePortalRowStatus == D_cpCaptivePortalRowStatus_active)
      {
        undodata->cpCaptivePortalRowStatus = D_cpCaptivePortalRowStatus_createAndGo;
      }
    }

    if (data->cpCaptivePortalRowStatus == D_cpCaptivePortalRowStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_cpCaptivePortalEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpCaptivePortalEntry_UNDO */
#endif /* SETS */


cpLocalUserGroupEntry_t *
k_cpLocalUserGroupEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 cpLocalUserGroupIndex)
{
  static cpLocalUserGroupEntry_t cpLocalUserGroupEntry;
  static L7_BOOL firstTime = L7_TRUE;
  gpId_t gId;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    cpLocalUserGroupEntry.cpLocalUserGroupName = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(cpLocalUserGroupEntry.valid);

  cpLocalUserGroupEntry.cpLocalUserGroupIndex = cpLocalUserGroupIndex;
  gId = (gpId_t)cpLocalUserGroupEntry.cpLocalUserGroupIndex;
  SET_VALID(I_cpLocalUserGroupIndex, cpLocalUserGroupEntry.valid);

  if ((searchType == EXACT) ? 
      usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS : 
      (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS && 
       usmDbCpdmUserGroupEntryNextGet(gId, &gId) != L7_SUCCESS))
  {
    ZERO_VALID(cpLocalUserGroupEntry.valid);
    return(NULL);
  }

  cpLocalUserGroupEntry.cpLocalUserGroupIndex = (L7_int32)gId;

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_cpLocalUserGroupIndex:
    if (nominator != -1) break;
       /* else pass through */

  case I_cpLocalUserGroupName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbCpdmUserGroupEntryNameGet(gId, snmp_buffer) == L7_SUCCESS && 
        (SafeMakeOctetStringFromTextExact(&cpLocalUserGroupEntry.cpLocalUserGroupName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_cpLocalUserGroupName, cpLocalUserGroupEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpLocalUserGroupRowStatus:
    /* if entry shows up, it's active */
    cpLocalUserGroupEntry.cpLocalUserGroupRowStatus = D_cpLocalUserGroupRowStatus_active;
    SET_VALID(I_cpLocalUserGroupRowStatus, cpLocalUserGroupEntry.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, cpLocalUserGroupEntry.valid) )
    return(NULL);

  return(&cpLocalUserGroupEntry);
}

#ifdef SETS
int
k_cpLocalUserGroupEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpLocalUserGroupEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpLocalUserGroupEntry_set_defaults(doList_t *dp)
{
  cpLocalUserGroupEntry_t *data = (cpLocalUserGroupEntry_t *) (dp->data);

  if ((data->cpLocalUserGroupName = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_cpLocalUserGroupEntry_set(cpLocalUserGroupEntry_t *data,
                            ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_BOOL isNewRowCreated = L7_FALSE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  gpId_t gId;

  bzero(tempValid, sizeof(tempValid));

  if ((data->cpLocalUserGroupIndex < GP_ID_MIN) || (data->cpLocalUserGroupIndex > GP_ID_MAX))
    return COMMIT_FAILED_ERROR;

  gId = (gpId_t)data->cpLocalUserGroupIndex;
  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);

  if (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS)
  {
    /* create a new entry */
    if (VALID(I_cpLocalUserGroupRowStatus, data->valid))
    {
      if ((data->cpLocalUserGroupRowStatus == D_cpLocalUserGroupRowStatus_createAndGo) && 
          (usmDbCpdmUserGroupEntryAdd(gId) != L7_SUCCESS))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_cpLocalUserGroupRowStatus, tempValid);
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (data->cpLocalUserGroupName != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->cpLocalUserGroupName->octet_ptr, data->cpLocalUserGroupName->length);
    if (VALID(I_cpLocalUserGroupName, data->valid))
    {
      if (usmDbCpdmUserGroupEntryNameSet(gId, snmp_buffer) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_cpLocalUserGroupName, tempValid);
      }
    }
  }

  if (VALID(I_cpLocalUserGroupRowStatus, data->valid) && 
      !(data->cpLocalUserGroupRowStatus == D_cpLocalUserGroupRowStatus_createAndGo))
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if ((data->cpLocalUserGroupRowStatus != D_cpLocalUserGroupRowStatus_active) && 
        (isNewRowCreated != L7_TRUE) && 
        (data->cpLocalUserGroupRowStatus != D_cpLocalUserGroupRowStatus_destroy || 
         usmDbCpdmUserGroupEntryDelete(gId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_cpLocalUserGroupEntry_UNDO
/* add #define SR_cpLocalUserGroupEntry_UNDO in sitedefs.h to
 * include the undo routine for the cpLocalUserGroupEntry family.
 */
int
cpLocalUserGroupEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
  cpLocalUserGroupEntry_t *data = (cpLocalUserGroupEntry_t *) doCur->data;
  cpLocalUserGroupEntry_t *undodata = (cpLocalUserGroupEntry_t *) doCur->undodata;
  cpLocalUserGroupEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /* Copy valid bits from data to undodata */
  if (undodata != NULL && data != NULL)
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if (undodata == NULL)
  {
    /* undoing an add, so delete */
    data->cpLocalUserGroupRowStatus = D_cpLocalUserGroupRowStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if (undodata->cpLocalUserGroupRowStatus == D_cpLocalUserGroupRowStatus_notReady || 
        undodata->cpLocalUserGroupRowStatus == D_cpLocalUserGroupRowStatus_notInService)
    {
      undodata->cpLocalUserGroupRowStatus = D_cpLocalUserGroupRowStatus_createAndWait;
    }
    else
    {
      if(undodata->cpLocalUserGroupRowStatus == D_cpLocalUserGroupRowStatus_active)
      {
        undodata->cpLocalUserGroupRowStatus = D_cpLocalUserGroupRowStatus_createAndGo;
      }
    }

    if (data->cpLocalUserGroupRowStatus == D_cpLocalUserGroupRowStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_cpLocalUserGroupEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpLocalUserGroupEntry_UNDO */
#endif /* SETS */


cpLocalUserEntry_t *
k_cpLocalUserEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 cpLocalUserIndex)
{
  static cpLocalUserEntry_t cpLocalUserEntry;
  static L7_BOOL firstTime = L7_TRUE;
  uId_t uId;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    cpLocalUserEntry.cpLocalUserName = MakeOctetString(NULL, 0);
    cpLocalUserEntry.cpLocalUserPassword = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(cpLocalUserEntry.valid);

  cpLocalUserEntry.cpLocalUserIndex = cpLocalUserIndex;
  uId = (uId_t)cpLocalUserEntry.cpLocalUserIndex;
  SET_VALID(I_cpLocalUserIndex, cpLocalUserEntry.valid);

  if ((searchType == EXACT) ? 
      usmDbCpdmUserEntryGet(uId) != L7_SUCCESS : 
      (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS && 
       usmDbCpdmUserEntryNextGet(uId, &uId) != L7_SUCCESS))
  {
    ZERO_VALID(cpLocalUserEntry.valid);
    return(NULL);
  }

  cpLocalUserEntry.cpLocalUserIndex = (L7_int32)uId;

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_cpLocalUserIndex:
    if (nominator != -1) break;
       /* else pass through */

  case I_cpLocalUserName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbCpdmUserEntryLoginNameGet(uId, snmp_buffer) == L7_SUCCESS && 
        (SafeMakeOctetStringFromTextExact(&cpLocalUserEntry.cpLocalUserName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_cpLocalUserName, cpLocalUserEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpLocalUserPassword:
    SET_VALID(I_cpLocalUserPassword, cpLocalUserEntry.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpLocalUserSessionTimeout:
    if (usmDbCpdmUserEntrySessionTimeoutGet(uId, &cpLocalUserEntry.cpLocalUserSessionTimeout) == L7_SUCCESS)
      SET_VALID(I_cpLocalUserSessionTimeout, cpLocalUserEntry.valid);
    if (nominator != -1) break;
    /* else pass through */

#ifdef I_cpLocalUserIdleTimeout
  case I_cpLocalUserIdleTimeout:
    if (usmDbCpdmUserEntryIdleTimeoutGet(uId, &cpLocalUserEntry.cpLocalUserIdleTimeout) == L7_SUCCESS)
      SET_VALID(I_cpLocalUserIdleTimeout, cpLocalUserEntry.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_cpLocalUserIdleTimeout */

#ifdef I_cpLocalUserMaxBandwidthUp
  case I_cpLocalUserMaxBandwidthUp:
    if (usmDbCpdmUserEntryMaxBandwidthUpGet(uId, &cpLocalUserEntry.cpLocalUserMaxBandwidthUp) == L7_SUCCESS)
    {
      /* Convert to bytes */
      cpLocalUserEntry.cpLocalUserMaxBandwidthUp = cpLocalUserEntry.cpLocalUserMaxBandwidthUp/8;
      SET_VALID(I_cpLocalUserMaxBandwidthUp, cpLocalUserEntry.valid);
    }
    if ( nominator != -1 ) break; 
    /* else pass through */
#endif /* I_cpLocalUserMaxBandwidthUp */

#ifdef I_cpLocalUserMaxBandwidthDown
  case I_cpLocalUserMaxBandwidthDown:
    if (usmDbCpdmUserEntryMaxBandwidthDownGet(uId, &cpLocalUserEntry.cpLocalUserMaxBandwidthDown) == L7_SUCCESS)
    {
      /* Convert to bytes */
      cpLocalUserEntry.cpLocalUserMaxBandwidthDown = cpLocalUserEntry.cpLocalUserMaxBandwidthDown/8;
      SET_VALID(I_cpLocalUserMaxBandwidthDown, cpLocalUserEntry.valid);
    }
    if ( nominator != -1 ) break; 
    /* else pass through */
#endif /* I_cpLocalUserMaxBandwidthDown */

#ifdef I_cpLocalUserMaxInputOctets
  case I_cpLocalUserMaxInputOctets:
    if (usmDbCpdmUserEntryMaxInputOctetsGet(uId, &cpLocalUserEntry.cpLocalUserMaxInputOctets) == L7_SUCCESS)
      SET_VALID(I_cpLocalUserMaxInputOctets, cpLocalUserEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */
#endif /* I_cpLocalUserMaxInputOctets */

#ifdef I_cpLocalUserMaxOutputOctets
  case I_cpLocalUserMaxOutputOctets:
    if (usmDbCpdmUserEntryMaxOutputOctetsGet(uId, &cpLocalUserEntry.cpLocalUserMaxOutputOctets) == L7_SUCCESS)
      SET_VALID(I_cpLocalUserMaxOutputOctets, cpLocalUserEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */
#endif /* I_cpLocalUserMaxOutputOctets */

#ifdef I_cpLocalUserMaxTotalOctets
  case I_cpLocalUserMaxTotalOctets:
    if (usmDbCpdmUserEntryMaxTotalOctetsGet(uId, &cpLocalUserEntry.cpLocalUserMaxTotalOctets) == L7_SUCCESS)
      SET_VALID(I_cpLocalUserMaxTotalOctets, cpLocalUserEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */
#endif /* I_cpLocalUserMaxTotalOctets */

  case I_cpLocalUserRowStatus:
    /* if entry shows up, it's active */
    cpLocalUserEntry.cpLocalUserRowStatus = D_cpLocalUserRowStatus_active;
    SET_VALID(I_cpLocalUserRowStatus, cpLocalUserEntry.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, cpLocalUserEntry.valid) )
    return(NULL);

  return(&cpLocalUserEntry);
}

#ifdef SETS
int
k_cpLocalUserEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpLocalUserEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpLocalUserEntry_set_defaults(doList_t *dp)
{
  cpLocalUserEntry_t *data = (cpLocalUserEntry_t *) (dp->data);

  if ((data->cpLocalUserName = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->cpLocalUserPassword = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  
  data->cpLocalUserSessionTimeout = 0;

#ifdef I_cpLocalUserIdleTimeout
  data->cpLocalUserIdleTimeout = 0;
#endif
#ifdef I_cpLocalUserMaxBandwidthUp
  data->cpLocalUserMaxBandwidthUp = 0;
#endif
#ifdef I_cpLocalUserMaxBandwidthDown
  data->cpLocalUserMaxBandwidthDown = 0;
#endif
#ifdef I_cpLocalUserMaxInputOctets
  data->cpLocalUserMaxInputOctets = 0;
#endif
#ifdef I_cpLocalUserMaxOutputOctets
  data->cpLocalUserMaxOutputOctets = 0;
#endif
#ifdef I_cpLocalUserMaxTotalOctets
  data->cpLocalUserMaxTotalOctets = 0;
#endif

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_cpLocalUserEntry_set(cpLocalUserEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_BOOL isNewRowCreated = L7_FALSE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  uId_t uId;
  L7_RC_t rc;

  bzero(tempValid, sizeof(tempValid));

  if ((data->cpLocalUserIndex == L7_NULL) || (data->cpLocalUserIndex > 128))
    return COMMIT_FAILED_ERROR;

  uId = (uId_t)data->cpLocalUserIndex;
  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    /* create a new entry */
    if (VALID(I_cpLocalUserRowStatus, data->valid))
    {
      if ((data->cpLocalUserRowStatus == D_cpLocalUserRowStatus_createAndGo) && 
          (usmDbCpdmUserEntryAdd(uId) != L7_SUCCESS))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_cpLocalUserRowStatus, tempValid);
        isNewRowCreated = L7_TRUE;

        rc = usmDbCpdmUserGroupAssocEntryAdd(uId, GP_ID_MIN);
        if ((rc != L7_SUCCESS) && (rc != L7_ALREADY_CONFIGURED))
        {
          return COMMIT_FAILED_ERROR;
        }
      }
    }
  }

  if (data->cpLocalUserName != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->cpLocalUserName->octet_ptr, data->cpLocalUserName->length);
    if (VALID(I_cpLocalUserName, data->valid))
    {
      if (usmDbCpdmUserEntryLoginNameSet(uId, snmp_buffer) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_cpLocalUserName, tempValid);
      }
    }
  }

  if (data->cpLocalUserPassword != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->cpLocalUserPassword->octet_ptr, data->cpLocalUserPassword->length);
    if (VALID(I_cpLocalUserPassword, data->valid))
    {
      if (usmDbCpdmUserEntryPasswordSet(uId, snmp_buffer) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_cpLocalUserPassword, tempValid);
      }
    }
  }

  if (VALID(I_cpLocalUserSessionTimeout, data->valid))
  {
    if ((data->cpLocalUserSessionTimeout < CP_USER_LOCAL_SESSION_TIMEOUT_MIN) || 
        (data->cpLocalUserSessionTimeout > CP_USER_LOCAL_SESSION_TIMEOUT_MAX) || 
        (usmDbCpdmUserEntrySessionTimeoutSet(uId, data->cpLocalUserSessionTimeout) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpLocalUserSessionTimeout, tempValid);
    }
  }

#ifdef I_cpLocalUserIdleTimeout
  if (VALID(I_cpLocalUserIdleTimeout, data->valid))
  {
    if ((data->cpLocalUserIdleTimeout < CP_USER_LOCAL_IDLE_TIMEOUT_MIN) || 
        (data->cpLocalUserIdleTimeout > CP_USER_LOCAL_IDLE_TIMEOUT_MAX) || 
        (usmDbCpdmUserEntryIdleTimeoutSet(uId, data->cpLocalUserIdleTimeout) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpLocalUserIdleTimeout, tempValid);
    }
  }
#endif /* I_cpLocalUserIdleTimeout */

#ifdef I_cpLocalUserMaxBandwidthUp
  if (VALID(I_cpLocalUserMaxBandwidthUp, data->valid))
  {
    if (usmDbCpdmUserEntryMaxBandwidthUpSet(uId, (data->cpLocalUserMaxBandwidthUp*8)) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_cpLocalUserMaxBandwidthUp, tempValid);
    }
  }
#endif /* I_cpLocalUserMaxBandwidthUp */

#ifdef I_cpLocalUserMaxBandwidthDown
  if (VALID(I_cpLocalUserMaxBandwidthDown, data->valid))
  {
    if (usmDbCpdmUserEntryMaxBandwidthDownSet(uId, (data->cpLocalUserMaxBandwidthDown*8)) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_cpLocalUserMaxBandwidthDown, tempValid);
    }
  }
#endif /* I_cpLocalUserMaxBandwidthDown */

#ifdef I_cpLocalUserMaxInputOctets
  if (VALID(I_cpLocalUserMaxInputOctets, data->valid))
  {
    if (usmDbCpdmUserEntryMaxInputOctetsSet(uId, data->cpLocalUserMaxInputOctets) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_cpLocalUserMaxInputOctets, tempValid);
    }
  }
#endif /* I_cpLocalUserMaxInputOctets */

#ifdef I_cpLocalUserMaxOutputOctets
  if (VALID(I_cpLocalUserMaxOutputOctets, data->valid))
  {
    if (usmDbCpdmUserEntryMaxOutputOctetsSet(uId, data->cpLocalUserMaxOutputOctets) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_cpLocalUserMaxOutputOctets, tempValid);
    }
  }
#endif /* I_cpLocalUserMaxOutputOctets */

#ifdef I_cpLocalUserMaxTotalOctets
  if (VALID(I_cpLocalUserMaxTotalOctets, data->valid))
  {
    if (usmDbCpdmUserEntryMaxTotalOctetsSet(uId, data->cpLocalUserMaxTotalOctets) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_cpLocalUserMaxTotalOctets, tempValid);
    }
  }
#endif /* I_cpLocalUserMaxTotalOctets */

  if (VALID(I_cpLocalUserRowStatus, data->valid) && 
      !(data->cpLocalUserRowStatus == D_cpLocalUserRowStatus_createAndGo))
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if ((data->cpLocalUserRowStatus != D_cpLocalUserRowStatus_active) && 
        (isNewRowCreated != L7_TRUE) && 
        (data->cpLocalUserRowStatus != D_cpLocalUserRowStatus_destroy || 
         usmDbCpdmUserEntryDelete(uId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_cpLocalUserEntry_UNDO
/* add #define SR_cpLocalUserEntry_UNDO in sitedefs.h to
 * include the undo routine for the cpLocalUserEntry family.
 */
int
cpLocalUserEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
  cpLocalUserEntry_t *data = (cpLocalUserEntry_t *) doCur->data;
  cpLocalUserEntry_t *undodata = (cpLocalUserEntry_t *) doCur->undodata;
  cpLocalUserEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /* Copy valid bits from data to undodata */
  if (undodata != NULL && data != NULL)
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if (undodata == NULL)
  {
    /* undoing an add, so delete */
    data->cpLocalUserRowStatus = D_cpLocalUserRowStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if(undodata->cpLocalUserRowStatus == D_cpLocalUserRowStatus_notReady || 
       undodata->cpLocalUserRowStatus == D_cpLocalUserRowStatus_notInService)
    {
      undodata->cpLocalUserRowStatus = D_cpLocalUserRowStatus_createAndWait;
    }
    else
    {
      if(undodata->cpLocalUserRowStatus == D_cpLocalUserRowStatus_active)
      {
        undodata->cpLocalUserRowStatus = D_cpLocalUserRowStatus_createAndGo;
      }
    }

    if (data->cpLocalUserRowStatus == D_cpLocalUserRowStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_cpLocalUserEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpLocalUserEntry_UNDO */
#endif /* SETS */


cpLocalUserGroupAssociationEntry_t *
k_cpLocalUserGroupAssociationEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_INT32 cpLocalUserGroupAssociationUserIndex,
                                       SR_INT32 cpLocalUserGroupAssociationGroupIndex)
{
  static cpLocalUserGroupAssociationEntry_t cpLocalUserGroupAssociationEntry;
  uId_t uId;
  gpId_t gId;

  ZERO_VALID(cpLocalUserGroupAssociationEntry.valid);

  cpLocalUserGroupAssociationEntry.cpLocalUserGroupAssociationUserIndex = cpLocalUserGroupAssociationUserIndex;
  uId = (uId_t)cpLocalUserGroupAssociationEntry.cpLocalUserGroupAssociationUserIndex;
  SET_VALID(I_cpLocalUserGroupAssociationUserIndex, cpLocalUserGroupAssociationEntry.valid);

  cpLocalUserGroupAssociationEntry.cpLocalUserGroupAssociationGroupIndex = cpLocalUserGroupAssociationGroupIndex;
  gId = (gpId_t)cpLocalUserGroupAssociationEntry.cpLocalUserGroupAssociationGroupIndex;
  SET_VALID(I_cpLocalUserGroupAssociationGroupIndex, cpLocalUserGroupAssociationEntry.valid);

  if ((searchType == EXACT) ? 
      usmDbCpdmUserGroupAssocEntryGet(uId, gId) != L7_SUCCESS : 
      (usmDbCpdmUserGroupAssocEntryGet(uId, gId) != L7_SUCCESS && 
       usmDbCpdmUserGroupAssocEntryNextGet(uId, gId, &uId, &gId) != L7_SUCCESS))
  {
    ZERO_VALID(cpLocalUserGroupAssociationEntry.valid);
    return(NULL);
  }

  cpLocalUserGroupAssociationEntry.cpLocalUserGroupAssociationUserIndex = (L7_int32)uId;
  cpLocalUserGroupAssociationEntry.cpLocalUserGroupAssociationGroupIndex = (L7_int32)gId;

  switch (nominator)
  {
  case -1:
  case I_cpLocalUserGroupAssociationUserIndex:
  case I_cpLocalUserGroupAssociationGroupIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpLocalUserGroupAssociationRowStatus:
    cpLocalUserGroupAssociationEntry.cpLocalUserGroupAssociationRowStatus = D_cpLocalUserGroupAssociationRowStatus_active;
    SET_VALID(I_cpLocalUserGroupAssociationRowStatus, cpLocalUserGroupAssociationEntry.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, cpLocalUserGroupAssociationEntry.valid))
    return(NULL);

  return(&cpLocalUserGroupAssociationEntry);
}

#ifdef SETS
int
k_cpLocalUserGroupAssociationEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpLocalUserGroupAssociationEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                         doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpLocalUserGroupAssociationEntry_set_defaults(doList_t *dp)
{
  cpLocalUserGroupAssociationEntry_t *data = (cpLocalUserGroupAssociationEntry_t *) (dp->data);

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_cpLocalUserGroupAssociationEntry_set(cpLocalUserGroupAssociationEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_BOOL isNewRowCreated = L7_FALSE;
  uId_t uId;
  gpId_t gId;

  bzero(tempValid, sizeof(tempValid));

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID) == L7_FALSE)
    return NO_SUCH_NAME_ERROR;

  uId = (uId_t)data->cpLocalUserGroupAssociationUserIndex;
  gId = (gpId_t)data->cpLocalUserGroupAssociationGroupIndex;

  if ((data->cpLocalUserGroupAssociationUserIndex < 1) || (data->cpLocalUserGroupAssociationUserIndex > 128) || 
      (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if ((data->cpLocalUserGroupAssociationGroupIndex < GP_ID_MIN) || (data->cpLocalUserGroupAssociationGroupIndex > GP_ID_MAX) || 
      (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (usmDbCpdmUserGroupAssocEntryGet(uId, gId) != L7_SUCCESS)
  {
    if (VALID(I_cpLocalUserGroupAssociationRowStatus, data->valid))
    {
      if (data->cpLocalUserGroupAssociationRowStatus == D_cpLocalUserGroupAssociationRowStatus_createAndGo && 
          usmDbCpdmUserGroupAssocEntryAdd(uId, gId) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_cpLocalUserGroupAssociationRowStatus, tempValid);
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (VALID(I_cpLocalUserGroupAssociationRowStatus, data->valid) && 
      !(data->cpLocalUserGroupAssociationRowStatus == D_cpLocalUserGroupAssociationRowStatus_createAndGo))
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if ((data->cpLocalUserGroupAssociationRowStatus != D_cpLocalUserGroupAssociationRowStatus_active) && 
        (isNewRowCreated != L7_TRUE) && 
        (data->cpLocalUserGroupAssociationRowStatus != D_cpLocalUserGroupAssociationRowStatus_destroy || 
         usmDbCpdmUserGroupAssocEntryDelete(uId, gId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_cpLocalUserGroupAssociationEntry_UNDO
/* add #define SR_cpLocalUserGroupAssociationEntry_UNDO in sitedefs.h to
 * include the undo routine for the cpLocalUserGroupAssociationEntry family.
 */
int
cpLocalUserGroupAssociationEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
  cpLocalUserGroupAssociationEntry_t *data = (cpLocalUserGroupAssociationEntry_t *) doCur->data;
  cpLocalUserGroupAssociationEntry_t *undodata = (cpLocalUserGroupAssociationEntry_t *) doCur->undodata;
  cpLocalUserGroupAssociationEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /* Copy valid bits from data to undodata */
  if (undodata != NULL && data != NULL)
    memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if (undodata == NULL)
  {
    /* undoing an add, so delete */
    data->cpLocalUserGroupAssociationRowStatus = D_cpLocalUserGroupAssociationRowStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if (undodata->cpLocalUserGroupAssociationRowStatus == D_cpLocalUserGroupAssociationRowStatus_notReady || 
        undodata->cpLocalUserGroupAssociationRowStatus == D_cpLocalUserGroupAssociationRowStatus_notInService)
    {
      undodata->cpLocalUserGroupAssociationRowStatus = D_cpLocalUserGroupAssociationRowStatus_createAndWait;
    }
    else
    {
      if (undodata->cpLocalUserGroupAssociationRowStatus == D_cpLocalUserGroupAssociationRowStatus_active)
      {
        undodata->cpLocalUserGroupAssociationRowStatus = D_cpLocalUserGroupAssociationRowStatus_createAndGo;
      }
    }

    if (data->cpLocalUserGroupAssociationRowStatus == D_cpLocalUserGroupAssociationRowStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_cpLocalUserGroupAssociationEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpLocalUserGroupAssociationEntry_UNDO */
#endif /* SETS */


cpInterfaceAssociationEntry_t *
k_cpInterfaceAssociationEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_UINT32 cpIntfAssociationCPID,
                                  SR_INT32 cpIntfAssociationIfIndex)
{
  static cpInterfaceAssociationEntry_t cpInterfaceAssociationEntry;
  cpId_t cpId;

  ZERO_VALID(cpInterfaceAssociationEntry.valid);

  cpInterfaceAssociationEntry.cpIntfAssociationCPID = cpIntfAssociationCPID;
  cpId = (cpId_t)cpInterfaceAssociationEntry.cpIntfAssociationCPID;
  SET_VALID(I_cpIntfAssociationCPID, cpInterfaceAssociationEntry.valid);

  cpInterfaceAssociationEntry.cpIntfAssociationIfIndex = cpIntfAssociationIfIndex;
  SET_VALID(I_cpIntfAssociationIfIndex, cpInterfaceAssociationEntry.valid);

  if ((searchType == EXACT) ? 
      usmDbCpdmCPConfigIntIfNumGet(cpId, cpInterfaceAssociationEntry.cpIntfAssociationIfIndex) != L7_SUCCESS : 
      (usmDbCpdmCPConfigIntIfNumGet(cpId, cpInterfaceAssociationEntry.cpIntfAssociationIfIndex) != L7_SUCCESS && 
       usmDbCpdmCPConfigIntIfNumNextGet(cpId, cpInterfaceAssociationEntry.cpIntfAssociationIfIndex, 
                                        &cpId, &cpInterfaceAssociationEntry.cpIntfAssociationIfIndex) != L7_SUCCESS))
  {
    ZERO_VALID(cpInterfaceAssociationEntry.valid);
    return(NULL);
  }

  cpInterfaceAssociationEntry.cpIntfAssociationCPID = (L7_uint32)cpId;

  switch (nominator)
  {
  case -1:
  case I_cpIntfAssociationCPID:
  case I_cpIntfAssociationIfIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpIntfAssociationRowStatus:
    cpInterfaceAssociationEntry.cpIntfAssociationRowStatus = D_cpIntfAssociationRowStatus_active;
    SET_VALID(I_cpIntfAssociationRowStatus, cpInterfaceAssociationEntry.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, cpInterfaceAssociationEntry.valid))
    return(NULL);

  return(&cpInterfaceAssociationEntry);
}

#ifdef SETS
int
k_cpInterfaceAssociationEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpInterfaceAssociationEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                    doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpInterfaceAssociationEntry_set_defaults(doList_t *dp)
{
  cpInterfaceAssociationEntry_t *data = (cpInterfaceAssociationEntry_t *) (dp->data);

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_cpInterfaceAssociationEntry_set(cpInterfaceAssociationEntry_t *data,
                                  ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_BOOL isNewRowCreated = L7_FALSE;
  cpId_t cpId;
  L7_uint32 intf;

  bzero(tempValid, sizeof(tempValid));

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID) == L7_FALSE)
    return NO_SUCH_NAME_ERROR;

  cpId = (cpId_t)data->cpIntfAssociationCPID;

  if ((data->cpIntfAssociationCPID == L7_NULL) || (data->cpIntfAssociationCPID > CP_ID_MAX) || 
      (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (usmDbIntIfNumFromExtIfNum(data->cpIntfAssociationIfIndex, &intf) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (usmDbCpdmCPConfigIntIfNumGet(cpId, intf) != L7_SUCCESS)
  {
    if (VALID(I_cpIntfAssociationRowStatus, data->valid))
    {
      if (data->cpIntfAssociationRowStatus == D_cpIntfAssociationRowStatus_createAndGo && 
          usmDbCpdmCPConfigIntIfNumAdd(cpId, intf) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_cpIntfAssociationRowStatus, tempValid);
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (VALID(I_cpIntfAssociationRowStatus, data->valid) && 
      !(data->cpIntfAssociationRowStatus == D_cpIntfAssociationRowStatus_createAndGo))
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if ((data->cpIntfAssociationRowStatus != D_cpIntfAssociationRowStatus_active) && 
        (isNewRowCreated != L7_TRUE) && 
        (data->cpIntfAssociationRowStatus != D_cpIntfAssociationRowStatus_destroy || 
         usmDbCpdmCPConfigIntIfNumDelete(cpId, intf) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_cpInterfaceAssociationEntry_UNDO
/* add #define SR_cpInterfaceAssociationEntry_UNDO in sitedefs.h to
 * include the undo routine for the cpInterfaceAssociationEntry family.
 */
int
cpInterfaceAssociationEntry_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
  cpInterfaceAssociationEntry_t *data = (cpInterfaceAssociationEntry_t *) doCur->data;
  cpInterfaceAssociationEntry_t *undodata = (cpInterfaceAssociationEntry_t *) doCur->undodata;
  cpInterfaceAssociationEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /* Copy valid bits from data to undodata */
  if (undodata != NULL && data != NULL)
    memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if (undodata == NULL)
  {
    /* undoing an add, so delete */
    data->cpIntfAssociationRowStatus = D_cpIntfAssociationRowStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if (undodata->cpIntfAssociationRowStatus == D_cpIntfAssociationRowStatus_notReady || 
        undodata->cpIntfAssociationRowStatus == D_cpIntfAssociationRowStatus_notInService)
    {
      undodata->cpIntfAssociationRowStatus = D_cpIntfAssociationRowStatus_createAndWait;
    }
    else
    {
      if (undodata->cpIntfAssociationRowStatus == D_cpIntfAssociationRowStatus_active)
      {
        undodata->cpIntfAssociationRowStatus = D_cpIntfAssociationRowStatus_createAndGo;
      }
    }

    if (data->cpIntfAssociationRowStatus == D_cpIntfAssociationRowStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_cpInterfaceAssociationEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpInterfaceAssociationEntry_UNDO */
#endif /* SETS */


cpCaptivePortalGlobalStatusGroup_t *
k_cpCaptivePortalGlobalStatusGroup_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator)
{
  static cpCaptivePortalGlobalStatusGroup_t cpCaptivePortalGlobalStatusGroup;

  ZERO_VALID(cpCaptivePortalGlobalStatusGroup.valid);

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpCaptivePortalOperStatus:
    if (snmpCpdmGlobalStatusModeGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalOperStatus) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalOperDisabledReason:
    if (snmpCpdmGlobalCPDisableReasonGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalOperDisabledReason) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalIpv4Address:
    if (snmpCpdmGlobalCPIPAddressGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalIpv4Address) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalInstanceMaxCount:
    if (usmDbCpdmGlobalSupportedCPInstancesGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalInstanceMaxCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalInstanceConfiguredCount:
    if (usmDbCpdmGlobalConfiguredCPInstancesGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalInstanceConfiguredCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalInstanceActiveCount:
    if (usmDbCpdmGlobalActiveCPInstancesGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalInstanceActiveCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalAuthenUserMaxCount:
    if (usmDbCpdmGlobalSupportedUsersGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalAuthenUserMaxCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalLocalUserMaxCount:
    if (usmDbCpdmGlobalLocalUsersGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalLocalUserMaxCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalConfiguredLocalUserCount:
    if (usmDbCpdmUserEntryCountGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalConfiguredLocalUserCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalAuthenUserCurrentCount:
    if (usmDbCpdmGlobalAuthenticatedUsersGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalAuthenUserCurrentCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

#if 0
  /* Not supported yet */
  case I_cpCaptivePortalAuthFailureMaxCount:
    if (usmDbCpdmGlobalAuthFailureMaxUsersGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalAuthFailureMaxCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalAuthFailureCurrentCount:
    if (usmDbCpdmGlobalAuthFailureUsersGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalAuthFailureCurrentCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalActivityLogMaxEntries:
    if (usmDbCpdmGlobalActivityLogMaxEntriesGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalActivityLogMaxEntries) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    break;

  case I_cpCaptivePortalActivityLogCurrentCount:
    if (usmDbCpdmGlobalActivityLogEntriesGet(&cpCaptivePortalGlobalStatusGroup.cpCaptivePortalActivityLogCurrentCount) == L7_SUCCESS)
      SET_VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid);
    break;
#endif

  default:
    /* unknown nominator */
    return(NULL);
  }

  if ((nominator >= 0) && !VALID(nominator, cpCaptivePortalGlobalStatusGroup.valid))
    return(NULL);

  return(&cpCaptivePortalGlobalStatusGroup);
}


cpCaptivePortalInstanceStatusEntry_t *
k_cpCaptivePortalInstanceStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                                         int nominator,
                                         int searchType,
                                         SR_UINT32 cpCaptivePortalInstanceId)
{
  static cpCaptivePortalInstanceStatusEntry_t cpCaptivePortalInstanceStatusEntry;
  cpId_t cpId;

  ZERO_VALID(cpCaptivePortalInstanceStatusEntry.valid);

  cpCaptivePortalInstanceStatusEntry.cpCaptivePortalInstanceId = cpCaptivePortalInstanceId;
  cpId = (cpId_t)cpCaptivePortalInstanceStatusEntry.cpCaptivePortalInstanceId;
  SET_VALID(I_cpCaptivePortalInstanceId, cpCaptivePortalInstanceStatusEntry.valid);

  if ((searchType == EXACT) ?
      usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS :
      (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS && 
       usmDbCpdmCPConfigNextGet(cpId, &cpId) != L7_SUCCESS))
  {
    ZERO_VALID(cpCaptivePortalInstanceStatusEntry.valid);
    return(NULL);
  }

  cpCaptivePortalInstanceStatusEntry.cpCaptivePortalInstanceId = (L7_uint32)cpId;

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalInstanceOperStatus:
    if (snmpCpdmCPConfigOperStatusGet(cpId, &cpCaptivePortalInstanceStatusEntry.cpCaptivePortalInstanceOperStatus) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalInstanceOperStatus, cpCaptivePortalInstanceStatusEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalInstanceOperDisabledReason:
    if (snmpCpdmCPConfigDisableReasonGet(cpId, &cpCaptivePortalInstanceStatusEntry.cpCaptivePortalInstanceOperDisabledReason) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalInstanceOperDisabledReason, cpCaptivePortalInstanceStatusEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalInstanceBlockStatus:
    if (snmpCpdmCPConfigBlockStatusGet(cpId, &cpCaptivePortalInstanceStatusEntry.cpCaptivePortalInstanceBlockStatus) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalInstanceBlockStatus, cpCaptivePortalInstanceStatusEntry.valid);
    if ( nominator != -1 ) break; 
    /* else pass through */

   case I_cpCaptivePortalInstanceAuthUserCount:
     if (usmDbCpdmCPConfigAuthenticatedUsersGet(cpId, &cpCaptivePortalInstanceStatusEntry.cpCaptivePortalInstanceAuthUserCount) == L7_SUCCESS)
       SET_VALID(I_cpCaptivePortalInstanceAuthUserCount, cpCaptivePortalInstanceStatusEntry.valid);
     break; 

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ((nominator >= 0) && !VALID(nominator, cpCaptivePortalInstanceStatusEntry.valid))
    return(NULL);

  return(&cpCaptivePortalInstanceStatusEntry);
}

#ifdef SETS
int
k_cpCaptivePortalInstanceStatusEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                          doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpCaptivePortalInstanceStatusEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                           doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpCaptivePortalInstanceStatusEntry_set_defaults(doList_t *dp)
{
  cpCaptivePortalInstanceStatusEntry_t *data = (cpCaptivePortalInstanceStatusEntry_t *) (dp->data);

  data->cpCaptivePortalInstanceOperStatus = D_cpCaptivePortalInstanceOperStatus_disable;
  data->cpCaptivePortalInstanceBlockStatus = D_cpCaptivePortalInstanceBlockStatus_notBlocked;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_cpCaptivePortalInstanceStatusEntry_set(cpCaptivePortalInstanceStatusEntry_t *data,
                                         ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  cpId_t cpId;

  bzero(tempValid, sizeof(tempValid));

  cpId = (cpId_t)data->cpCaptivePortalInstanceId;

  if (VALID(I_cpCaptivePortalInstanceBlockStatus, data->valid))
  {
    if (snmpCpdmCPConfigBlockStatusSet(cpId, data->cpCaptivePortalInstanceBlockStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpCaptivePortalInstanceBlockStatus, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_cpCaptivePortalInstanceStatusEntry_UNDO
/* add #define SR_cpCaptivePortalInstanceStatusEntry_UNDO in sitedefs.h to
 * include the undo routine for the cpCaptivePortalInstanceStatusEntry family.
 */
int
cpCaptivePortalInstanceStatusEntry_undo(doList_t *doHead, doList_t *doCur,
                                        ContextInfo *contextInfo)
{
  cpCaptivePortalInstanceStatusEntry_t *data = (cpCaptivePortalInstanceStatusEntry_t *) doCur->data;
  cpCaptivePortalInstanceStatusEntry_t *undodata = (cpCaptivePortalInstanceStatusEntry_t *) doCur->undodata;
  cpCaptivePortalInstanceStatusEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /* Copy valid bits from data to undodata */
  if (undodata != NULL && data != NULL)
    memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if (undodata == NULL)
  {
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_cpCaptivePortalInstanceStatusEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpCaptivePortalInstanceStatusEntry_UNDO */
#endif /* SETS */


cpCaptivePortalIntfStatusEntry_t *
k_cpCaptivePortalIntfStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_UINT32 cpCaptivePortalInstanceId,
                                     SR_INT32 cpCaptivePortalIntfIfIndex)
{
  static cpCaptivePortalIntfStatusEntry_t cpCaptivePortalIntfStatusEntry;
  cpId_t cpId;

  ZERO_VALID(cpCaptivePortalIntfStatusEntry.valid);

  cpCaptivePortalIntfStatusEntry.cpCaptivePortalInstanceId = cpCaptivePortalInstanceId;
  cpId = (cpId_t)cpCaptivePortalIntfStatusEntry.cpCaptivePortalInstanceId;
  SET_VALID(I_cpCaptivePortalInstanceId, cpCaptivePortalIntfStatusEntry.valid);

  cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex = cpCaptivePortalIntfIfIndex;
  SET_VALID(I_cpCaptivePortalIntfIfIndex, cpCaptivePortalIntfStatusEntry.valid);

  if ((searchType == EXACT) ? 
      usmDbCpdmCPConfigIntIfNumGet(cpId, cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex) != L7_SUCCESS : 
      (usmDbCpdmCPConfigIntIfNumGet(cpId, cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex) != L7_SUCCESS && 
       usmDbCpdmCPConfigIntIfNumNextGet(cpId, cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex, 
                                        &cpId, &cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex) != L7_SUCCESS))
  {
    ZERO_VALID(cpCaptivePortalIntfStatusEntry.valid);
    return(NULL);
  }

  cpCaptivePortalIntfStatusEntry.cpCaptivePortalInstanceId = (L7_uint32)cpId;

  /*
  * if ( nominator != -1 ) condition is added to all the case statements
  * for storing all the values to support the undo functionality.
  */
  
  switch (nominator)
  {
  case -1:
  case I_cpCaptivePortalIntfIfIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpCaptivePortalIntfOperStatus:
    if (snmpCpdmCPIntfStatusGet(cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex, 
                                &cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfOperStatus) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalIntfOperStatus, cpCaptivePortalIntfStatusEntry.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpCaptivePortalIntfOperDisabledReason:
    if (snmpCpdmCPIntfStatusGetWhy(cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex,
                                   &cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfOperDisabledReason) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalIntfOperDisabledReason, cpCaptivePortalIntfStatusEntry.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpCaptivePortalIntfBlockStatus:
    if (snmpCpdmCPIntfBlockStatusGet(cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex,
                                     &cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfBlockStatus) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalIntfBlockStatus, cpCaptivePortalIntfStatusEntry.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_cpCaptivePortalIntfAuthUserCount:
    if (usmDbCpdmIntfStatusGetUsers(cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfIfIndex,
                                    &cpCaptivePortalIntfStatusEntry.cpCaptivePortalIntfAuthUserCount) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalIntfAuthUserCount, cpCaptivePortalIntfStatusEntry.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, cpCaptivePortalIntfStatusEntry.valid) )
    return(NULL);

  return(&cpCaptivePortalIntfStatusEntry);
}


cpCaptivePortalIntfDatabaseEntry_t *
k_cpCaptivePortalIntfDatabaseEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_INT32 cpCaptivePortalIntfIfIndex)
{
  static cpCaptivePortalIntfDatabaseEntry_t cpCaptivePortalIntfDatabaseEntry;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 bufLen;

  if (firstTime == L7_TRUE)
  {
    cpCaptivePortalIntfDatabaseEntry.cpCaptivePortalIntfCapabilities = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(cpCaptivePortalIntfDatabaseEntry.valid);

  cpCaptivePortalIntfDatabaseEntry.cpCaptivePortalIntfIfIndex = cpCaptivePortalIntfIfIndex;
  SET_VALID(I_cpCaptivePortalIntfIfIndex, cpCaptivePortalIntfDatabaseEntry.valid);

  if ((searchType == EXACT) ?
      usmDbCpimIntfGet(cpCaptivePortalIntfDatabaseEntry.cpCaptivePortalIntfIfIndex) != L7_SUCCESS :
      (usmDbCpimIntfGet(cpCaptivePortalIntfDatabaseEntry.cpCaptivePortalIntfIfIndex) != L7_SUCCESS &&
       usmDbCpimIntfNextGet(cpCaptivePortalIntfDatabaseEntry.cpCaptivePortalIntfIfIndex, 
                            &cpCaptivePortalIntfDatabaseEntry.cpCaptivePortalIntfIfIndex) != L7_SUCCESS))
  {
    ZERO_VALID(cpCaptivePortalIntfDatabaseEntry.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break; 
    /* else pass through */

  case I_cpCaptivePortalIntfCapabilities:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (snmpCpCaptivePortalIntfCapabilitiesGet(cpCaptivePortalIntfDatabaseEntry.cpCaptivePortalIntfIfIndex, snmp_buffer, &bufLen) == L7_SUCCESS &&
        (SafeMakeOctetString(&cpCaptivePortalIntfDatabaseEntry.cpCaptivePortalIntfCapabilities, snmp_buffer, bufLen) == L7_TRUE))
      SET_VALID(I_cpCaptivePortalIntfCapabilities, cpCaptivePortalIntfDatabaseEntry.valid);
      break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ((nominator >= 0) && !VALID(nominator, cpCaptivePortalIntfDatabaseEntry.valid))
    return(NULL);

  return(&cpCaptivePortalIntfDatabaseEntry);
}


cpCaptivePortalClientStatusEntry_t *
k_cpCaptivePortalClientStatusEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       OctetString * cpCaptivePortalClientMacAddress)
{
  static cpCaptivePortalClientStatusEntry_t cpCaptivePortalClientStatusEntry;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  cpId_t cpId;
#ifdef I_cpCaptivePortalClientBytesTransmitted
  L7_uint64 bTx;
#endif
#ifdef I_cpCaptivePortalClientBytesReceived
  L7_uint64 bRx;
#endif
#ifdef I_cpCaptivePortalClientPacketsTransmitted
  L7_uint64 pTx;
#endif
#ifdef I_cpCaptivePortalClientPacketsReceived
  L7_uint64 pRx;
#endif

  if (firstTime == L7_TRUE)
  {
    cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress = MakeOctetString(NULL, 0);
    cpCaptivePortalClientStatusEntry.cpCaptivePortalClientUserName = MakeOctetString(NULL, 0);  
#ifdef I_cpCaptivePortalClientSwitchMacAddress
    cpCaptivePortalClientStatusEntry.cpCaptivePortalClientSwitchMacAddress = MakeOctetString(NULL, 0);
#endif
    firstTime = L7_FALSE;
  }

  ZERO_VALID(cpCaptivePortalClientStatusEntry.valid);

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, cpCaptivePortalClientMacAddress->octet_ptr, cpCaptivePortalClientMacAddress->length);

  if(SafeMakeOctetString(&cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress,
                         snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE)
  {
    return NULL;
  }
  SET_VALID(I_cpCaptivePortalClientMacAddress, cpCaptivePortalClientStatusEntry.valid);

  if (((searchType == EXACT) ? 
       usmDbCpdmClientConnStatusGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr) != L7_SUCCESS : 
       (usmDbCpdmClientConnStatusGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr) != L7_SUCCESS && 
        usmDbCpdmClientConnStatusNextGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr, (L7_enetMacAddr_t *)snmp_buffer) != L7_SUCCESS)) || 
      (SafeMakeOctetString(&cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE))
  {
    ZERO_VALID(cpCaptivePortalClientStatusEntry.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_cpCaptivePortalClientMacAddress:
    break;

  case I_cpCaptivePortalClientIpAddress:
    if (usmDbCpdmClientConnStatusIpGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr,
                                       (L7_IP_ADDR_t *)&cpCaptivePortalClientStatusEntry.cpCaptivePortalClientIpAddress) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalClientIpAddress, cpCaptivePortalClientStatusEntry.valid);
    break;

  case I_cpCaptivePortalClientUserName:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (usmDbCpdmClientConnStatusUserNameGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&cpCaptivePortalClientStatusEntry.cpCaptivePortalClientUserName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_cpCaptivePortalClientUserName, cpCaptivePortalClientStatusEntry.valid);
    break;

  case I_cpCaptivePortalClientProtocolMode:
    if (snmpCpdmClientConnStatusProtocolModeGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr,
                                                &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientProtocolMode) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalClientProtocolMode, cpCaptivePortalClientStatusEntry.valid);
    break;

  case I_cpCaptivePortalClientVerificationMode:
    if (snmpCpdmClientConnStatusVerifyModeGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr,
                                              &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientVerificationMode) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalClientVerificationMode, cpCaptivePortalClientStatusEntry.valid);
    break;

  case I_cpCaptivePortalClientAssocIfIndex:
    if (snmpCpdmClientConnStatusIntfIdGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr,
                                          &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientAssocIfIndex) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalClientAssocIfIndex, cpCaptivePortalClientStatusEntry.valid);
    break;

  case I_cpCaptivePortalClientCPID:
    if (usmDbCpdmClientConnStatusCpIdGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr, 
                                         &cpId) == L7_SUCCESS)
    {
      cpCaptivePortalClientStatusEntry.cpCaptivePortalClientCPID = cpId;
      SET_VALID(I_cpCaptivePortalClientCPID, cpCaptivePortalClientStatusEntry.valid);
    }
    break;

  case I_cpCaptivePortalClientSessionTime:
    if (snmpCpdmClientConnStatusSessionTimeGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr,
                                               &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientSessionTime) == L7_SUCCESS)
      SET_VALID(I_cpCaptivePortalClientSessionTime, cpCaptivePortalClientStatusEntry.valid);
    break;

#ifdef I_cpCaptivePortalClientSwitchMacAddress
  case I_cpCaptivePortalClientSwitchMacAddress:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbCpdmClusterSupportGet() == L7_SUCCESS) &&
        (usmDbCpdmClientConnStatusSwitchMacAddrGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr,
                                                   (L7_enetMacAddr_t *)snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetString(&cpCaptivePortalClientStatusEntry.cpCaptivePortalClientSwitchMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE))
      SET_VALID(I_cpCaptivePortalClientSwitchMacAddress, cpCaptivePortalClientStatusEntry.valid);
    break;
#endif /* I_cpCaptivePortalClientSwitchMacAddress */

#ifdef I_cpCaptivePortalClientSwitchIpAddress
  case I_cpCaptivePortalClientSwitchIpAddress:
    if ((usmDbCpdmClusterSupportGet() == L7_SUCCESS) && 
        (usmDbCpdmClientConnStatusSwitchIpGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr,
                                              (L7_IP_ADDR_t *)&cpCaptivePortalClientStatusEntry.cpCaptivePortalClientSwitchIpAddress) == L7_SUCCESS))
      SET_VALID(I_cpCaptivePortalClientSwitchIpAddress, cpCaptivePortalClientStatusEntry.valid);
    break;
#endif /* I_cpCaptivePortalClientSwitchIpAddress */

#ifdef I_cpCaptivePortalClientSwitchType
  case I_cpCaptivePortalClientSwitchType:
    if ((usmDbCpdmClusterSupportGet() == L7_SUCCESS) && 
        (snmpCpdmClientConnStatusWhichSwitchGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr,
                                                &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientSwitchType) == L7_SUCCESS))
      SET_VALID(I_cpCaptivePortalClientSwitchType, cpCaptivePortalClientStatusEntry.valid);
    break;
#endif /* I_cpCaptivePortalClientSwitchType */

  case I_cpCaptivePortalClientDeauthAction:
    cpCaptivePortalClientStatusEntry.cpCaptivePortalClientDeauthAction = D_cpCaptivePortalClientDeauthAction_none;
    SET_VALID(I_cpCaptivePortalClientDeauthAction, cpCaptivePortalClientStatusEntry.valid);
    break;

#ifdef I_cpCaptivePortalClientBytesReceived
  case I_cpCaptivePortalClientBytesReceived:
    if ((cpCaptivePortalClientStatusEntry.cpCaptivePortalClientBytesReceived != NULL || 
         (cpCaptivePortalClientStatusEntry.cpCaptivePortalClientBytesReceived = MakeCounter64(0)) != NULL) && 
        usmDbCpdmClientConnStatusStatisticsGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr, 
                                               &bTx, &bRx, &pTx, &pRx) == L7_SUCCESS)
    {
      snmpCPConvert64BitUnsignedInteger(bRx, &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientBytesReceived->big_end,
                                        &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientBytesReceived->little_end);
      SET_VALID(I_cpCaptivePortalClientBytesReceived, cpCaptivePortalClientStatusEntry.valid);
    }
    break;
#endif /* I_cpCaptivePortalClientBytesReceived */

#ifdef I_cpCaptivePortalClientBytesTransmitted
  case I_cpCaptivePortalClientBytesTransmitted:
    if ((cpCaptivePortalClientStatusEntry.cpCaptivePortalClientBytesTransmitted != NULL || 
         (cpCaptivePortalClientStatusEntry.cpCaptivePortalClientBytesTransmitted = MakeCounter64(0)) != NULL) && 
        usmDbCpdmClientConnStatusStatisticsGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr, 
                                               &bTx, &bRx, &pTx, &pRx) == L7_SUCCESS)
    {
      snmpCPConvert64BitUnsignedInteger(bTx, &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientBytesTransmitted->big_end, 
                                        &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientBytesTransmitted->little_end);
      SET_VALID(I_cpCaptivePortalClientBytesTransmitted, cpCaptivePortalClientStatusEntry.valid);
    }
    break;
#endif /* I_cpCaptivePortalClientBytesTransmitted */

#ifdef I_cpCaptivePortalClientPacketsReceived
  case I_cpCaptivePortalClientPacketsReceived:
    if ((cpCaptivePortalClientStatusEntry.cpCaptivePortalClientPacketsReceived != NULL || 
         (cpCaptivePortalClientStatusEntry.cpCaptivePortalClientPacketsReceived = MakeCounter64(0)) != NULL) && 
        usmDbCpdmClientConnStatusStatisticsGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr, 
                                               &bTx, &bRx, &pTx, &pRx) == L7_SUCCESS)
    {
      snmpCPConvert64BitUnsignedInteger(pRx, &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientPacketsReceived->big_end,
                                        &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientPacketsReceived->little_end);
      SET_VALID(I_cpCaptivePortalClientPacketsReceived, cpCaptivePortalClientStatusEntry.valid);
    }
    break;
#endif /* I_cpCaptivePortalClientPacketsReceived */

#ifdef I_cpCaptivePortalClientPacketsTransmitted
  case I_cpCaptivePortalClientPacketsTransmitted:
    if ((cpCaptivePortalClientStatusEntry.cpCaptivePortalClientPacketsTransmitted != NULL || 
         (cpCaptivePortalClientStatusEntry.cpCaptivePortalClientPacketsTransmitted = MakeCounter64(0)) != NULL) && 
        usmDbCpdmClientConnStatusStatisticsGet((L7_enetMacAddr_t *)cpCaptivePortalClientStatusEntry.cpCaptivePortalClientMacAddress->octet_ptr, 
                                               &bTx, &bRx, &pTx, &pRx) == L7_SUCCESS)
    {
      snmpCPConvert64BitUnsignedInteger(pTx, &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientPacketsTransmitted->big_end,
                                        &cpCaptivePortalClientStatusEntry.cpCaptivePortalClientPacketsTransmitted->little_end);
      SET_VALID(I_cpCaptivePortalClientPacketsTransmitted, cpCaptivePortalClientStatusEntry.valid);
    }
    break;
#endif /* I_cpCaptivePortalClientPacketsTransmitted */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ((nominator >= 0) && !VALID(nominator, cpCaptivePortalClientStatusEntry.valid))
    return(NULL);

  return(&cpCaptivePortalClientStatusEntry);
}

#ifdef SETS
int
k_cpCaptivePortalClientStatusEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpCaptivePortalClientStatusEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                         doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpCaptivePortalClientStatusEntry_set_defaults(doList_t *dp)
{
  cpCaptivePortalClientStatusEntry_t *data = (cpCaptivePortalClientStatusEntry_t *) (dp->data);

  if ((data->cpCaptivePortalClientUserName = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

#ifdef I_cpCaptivePortalClientSwitchMacAddress
  if ((data->cpCaptivePortalClientSwitchMacAddress = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
#endif

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_cpCaptivePortalClientStatusEntry_set(cpCaptivePortalClientStatusEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
  if (VALID(I_cpCaptivePortalClientDeauthAction, data->valid))
  {
    if (snmpCpdmClientDeauthenticateAction(data->cpCaptivePortalClientMacAddress->octet_ptr, 
                                           data->cpCaptivePortalClientDeauthAction) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_cpCaptivePortalClientStatusEntry_UNDO
/* add #define SR_cpCaptivePortalClientStatusEntry_UNDO in sitedefs.h to
 * include the undo routine for the cpCaptivePortalClientStatusEntry family.
 */
int
cpCaptivePortalClientStatusEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpCaptivePortalClientStatusEntry_UNDO */
#endif /* SETS */


cpCaptivePortalIntfClientAssocEntry_t *
k_cpCaptivePortalIntfClientAssocEntry_get(int serialNum, ContextInfo *contextInfo,
                                          int nominator,
                                          int searchType,
                                          SR_INT32 cpCaptivePortalIntfClientIfIndex,
                                          OctetString * cpCaptivePortalIntfClientAssocMacAddress)
{
  static cpCaptivePortalIntfClientAssocEntry_t cpCaptivePortalIntfClientAssocEntry;
  static L7_BOOL firstTime;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientAssocMacAddress = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(cpCaptivePortalIntfClientAssocEntry.valid);

  cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientIfIndex = cpCaptivePortalIntfClientIfIndex;
  SET_VALID(I_cpCaptivePortalIntfClientIfIndex, cpCaptivePortalIntfClientAssocEntry.valid);

  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, cpCaptivePortalIntfClientAssocMacAddress->octet_ptr, cpCaptivePortalIntfClientAssocMacAddress->length);

  if(SafeMakeOctetString(&cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientAssocMacAddress,
                         snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE)
  {
    return NULL;
  }
  SET_VALID(I_cpCaptivePortalIntfClientAssocMacAddress, cpCaptivePortalIntfClientAssocEntry.valid);

  if (((searchType == EXACT) ?
       usmDbCpdmInterfaceConnStatusGet(cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientIfIndex, (L7_enetMacAddr_t *)cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientAssocMacAddress->octet_ptr) != L7_SUCCESS :
       (usmDbCpdmInterfaceConnStatusGet(cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientIfIndex, (L7_enetMacAddr_t *)cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientAssocMacAddress->octet_ptr) != L7_SUCCESS &&
        usmDbCpdmInterfaceConnStatusNextGet(cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientIfIndex, (L7_enetMacAddr_t *)cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientAssocMacAddress->octet_ptr, 
                                            &cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientIfIndex, (L7_enetMacAddr_t *)snmp_buffer) != L7_SUCCESS)) ||
      (SafeMakeOctetString(&cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientAssocMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE))
  {
    ZERO_VALID(cpCaptivePortalIntfClientAssocEntry.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_cpCaptivePortalIntfClientIfIndex:
  case I_cpCaptivePortalIntfClientAssocMacAddress:
    break;

  case I_cpCaptivePortalIntfClientAssocRowStatus:
    if (usmDbCpdmInterfaceConnStatusGet(cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientIfIndex, (L7_enetMacAddr_t *)snmp_buffer) == L7_SUCCESS) 
    {
      /* if entry shows up, it's active */
      cpCaptivePortalIntfClientAssocEntry.cpCaptivePortalIntfClientAssocRowStatus = D_cpCaptivePortalIntfClientAssocRowStatus_active;
      SET_VALID(I_cpCaptivePortalIntfClientAssocRowStatus, cpCaptivePortalIntfClientAssocEntry.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, cpCaptivePortalIntfClientAssocEntry.valid) )
    return(NULL);

  return(&cpCaptivePortalIntfClientAssocEntry);
}


cpCaptivePortalInstanceClientAssocEntry_t *
k_cpCaptivePortalInstanceClientAssocEntry_get(int serialNum, ContextInfo *contextInfo,
                                              int nominator,
                                              int searchType,
                                              SR_INT32 cpCaptivePortalInstanceClientAssocInstanceId,
                                              OctetString * cpCaptivePortalInstanceClientAssocMacAddress)
{
  static cpCaptivePortalInstanceClientAssocEntry_t cpCaptivePortalInstanceClientAssocEntry;
  static L7_BOOL firstTime;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocMacAddress = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(cpCaptivePortalInstanceClientAssocEntry.valid);

  cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocInstanceId = cpCaptivePortalInstanceClientAssocInstanceId;
  SET_VALID(I_cpCaptivePortalInstanceClientAssocInstanceId, cpCaptivePortalInstanceClientAssocEntry.valid);

  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, cpCaptivePortalInstanceClientAssocMacAddress->octet_ptr, cpCaptivePortalInstanceClientAssocMacAddress->length);

  if(SafeMakeOctetString(&cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocMacAddress,
                         snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE)
  {
    return NULL;
  }
  SET_VALID(I_cpCaptivePortalInstanceClientAssocMacAddress, cpCaptivePortalInstanceClientAssocEntry.valid);

  if (((searchType == EXACT) ?
       snmpCpdmCPConnStatusGet(cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocInstanceId, (L7_enetMacAddr_t *)cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocMacAddress->octet_ptr) != L7_SUCCESS :
       (snmpCpdmCPConnStatusGet(cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocInstanceId, (L7_enetMacAddr_t *)cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocMacAddress->octet_ptr) != L7_SUCCESS &&
        snmpCpdmCPConnStatusNextGet(cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocInstanceId, (L7_enetMacAddr_t *)cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocMacAddress->octet_ptr, 
                                    &cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocInstanceId, (L7_enetMacAddr_t *)snmp_buffer) != L7_SUCCESS)) ||
      (SafeMakeOctetString(&cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE))
  {
    ZERO_VALID(cpCaptivePortalInstanceClientAssocEntry.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_cpCaptivePortalInstanceClientAssocInstanceId:
  case I_cpCaptivePortalInstanceClientAssocMacAddress:
    break;

  case I_cpCaptivePortalInstanceClientAssocRowStatus:
    if (snmpCpdmCPConnStatusGet(cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocInstanceId, 
                                (L7_enetMacAddr_t *)snmp_buffer) == L7_SUCCESS)
    {
      /* if entry shows up, it's active */
      cpCaptivePortalInstanceClientAssocEntry.cpCaptivePortalInstanceClientAssocRowStatus = D_cpCaptivePortalInstanceClientAssocRowStatus_active;
      SET_VALID(I_cpCaptivePortalInstanceClientAssocRowStatus, cpCaptivePortalInstanceClientAssocEntry.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, cpCaptivePortalInstanceClientAssocEntry.valid) )
    return(NULL);

  return(&cpCaptivePortalInstanceClientAssocEntry);
}


cpTrapsConfig_t *
k_cpTrapsConfig_get(int serialNum, ContextInfo *contextInfo,
                    int nominator)
{
  static cpTrapsConfig_t cpTrapsConfigData;

  ZERO_VALID(cpTrapsConfigData.valid);

  switch (nominator)
  {
  case -1:
    break;

  case I_cpTrapMode:
    if (snmpTrapCaptivePortalGet(&cpTrapsConfigData.cpTrapMode) == L7_SUCCESS)
      SET_VALID(I_cpTrapMode, cpTrapsConfigData.valid);
    break;

  case I_cpClientAuthenticationFailureTrapMode:
    if (snmpCpdmClientAuthFailureTrapModeGet(&cpTrapsConfigData.cpClientAuthenticationFailureTrapMode)== L7_SUCCESS)
      SET_VALID(I_cpClientAuthenticationFailureTrapMode, cpTrapsConfigData.valid);
    break;

  case I_cpClientConnectTrapMode:
    if (snmpCpdmClientConnectTrapModeGet(&cpTrapsConfigData.cpClientConnectTrapMode)== L7_SUCCESS)
      SET_VALID(I_cpClientConnectTrapMode, cpTrapsConfigData.valid);
    break;

  case I_cpClientDatabaseFullTrapMode:
    if (snmpCpdmClientDBFullTrapModeGet(&cpTrapsConfigData.cpClientDatabaseFullTrapMode)== L7_SUCCESS)
      SET_VALID(I_cpClientDatabaseFullTrapMode, cpTrapsConfigData.valid);
    break;

  case I_cpClientDisconnectTrapMode:
    if (snmpCpdmClientDisconnectTrapModeGet(&cpTrapsConfigData.cpClientDisconnectTrapMode)== L7_SUCCESS)
      SET_VALID(I_cpClientDisconnectTrapMode, cpTrapsConfigData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ((nominator >= 0) && !VALID(nominator, cpTrapsConfigData.valid))
    return(NULL);

  return(&cpTrapsConfigData);
}

#ifdef SETS
int
k_cpTrapsConfig_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_cpTrapsConfig_ready(ObjectInfo *object, ObjectSyntax *value, 
                      doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_cpTrapsConfig_set(cpTrapsConfig_t *data, ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_cpTrapMode, data->valid))
  {
    if (snmpTrapCaptivePortalSet(data->cpTrapMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpTrapMode, tempValid);
    }
  }

  if (VALID(I_cpClientAuthenticationFailureTrapMode, data->valid))
  {
    if (snmpCpdmClientAuthFailureTrapModeSet(data->cpClientAuthenticationFailureTrapMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpClientAuthenticationFailureTrapMode, tempValid);
    }
  }

  if (VALID(I_cpClientConnectTrapMode, data->valid))
  {
    if (snmpCpdmClientConnectTrapModeSet(data->cpClientConnectTrapMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpClientConnectTrapMode, tempValid);
    }
  }

  if (VALID(I_cpClientDatabaseFullTrapMode, data->valid))
  {
    if (snmpCpdmClientDBFullTrapModeSet(data->cpClientDatabaseFullTrapMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpClientDatabaseFullTrapMode, tempValid);
    }
  }

  if (VALID(I_cpClientDisconnectTrapMode, data->valid))
  {
    if (snmpCpdmClientDisconnectTrapModeSet(data->cpClientDisconnectTrapMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_cpClientDisconnectTrapMode, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_cpTrapsConfig_UNDO
/* add #define SR_cpTrapsConfig_UNDO in sitedefs.h to
 * include the undo routine for the cpTrapsConfig family.
 */
int
cpTrapsConfig_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_cpTrapsConfig_UNDO */
#endif /* SETS */

