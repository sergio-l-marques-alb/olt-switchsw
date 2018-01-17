/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpdm_test.c
*
* @purpose      Captive Portal Data Manager (CPDM) test functions.
*
* @component    CP
*
* @comments     This file contains all necessary test functions to
*               verify CPDM functionality without other captive
*               portal components.  These functions are only included
*               when L7_CPDM_TEST is defined. The definition can be
*               added to the captive portal make.pkg file as follows;
*               
*               # Additional compiler flags.
*               #
*               CFLAGSEXTRA += \
*               	-DL7_CPDM_TEST
*
* @create       06/28/2007
*
* @author       darsenault
*
* @end
*
**********************************************************************/
#ifdef L7_CPDM_TEST

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "l7utils_api.h"
#include "osapi.h"
#include "osapi_support.h"
#include "cpdm.h"
#include "cpdm_api.h"
#include "sysapi.h"


static L7_uchar8 * call_to_set_failed  = "Call to set %s failed\n";
static L7_uchar8 * login_name_is       = "Login name = %s\n";
static L7_uchar8 * cp_is               = "cp id = %d\n";
static L7_uchar8 * cp_and_web_id_are   = "cp id = %d, web id = %d\n";
static L7_uchar8 * null_login_name_msg = "Login name ptr is NULL!\n";
static L7_uchar8 * bad_input_ptr       = "Input ptr is NULL!\n";


/*********************************************************************
*
* @purpose  No-op function to include all cpdmTest* functions for devshell.
*
* @param    none
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmTestNoop()
{
  return;
}

/*********************************************************************
*
* @purpose  Set global status enabled mode for captive portal
*
* @param    L7_char8 mode ((input)) L7_ENABLE, L7_DISABLE
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmTestGlobalStatusModeSet(L7_char8 mode)
{
  if ( cpdmGlobalModeSet(mode) != L7_SUCCESS)
  {
      sysapiPrintf("CP: Global Status Mode Set failed\n" );
  }
} /* cpdmTestGlobalStatusModeSet */

/*********************************************************************
*
* @purpose  Set global report status report interval for captive portal
*
* @param    L7_unint32 @b{(input)} interval
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmTestGlobalStatusReportIntervalSet(L7_uint32 interval)
{
  if ( cpdmGlobalStatusReportIntervalSet(interval) != L7_SUCCESS)
  {
      sysapiPrintf("CP: Global Status Report Interval Set failed\n" );
  }
} /* cpdmTestGlobalStatusReportIntervalSet */

/*********************************************************************
*
* @purpose  Set the global status session timeout for captive portal
*
* @param    L7_unint32 @b{(input)} timeout
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmTestGlobalStatusSessionTimeoutSet(L7_uint32 timeout)
{
  if ( cpdmGlobalStatusSessionTimeoutSet(timeout) != L7_SUCCESS)
  {
      sysapiPrintf("CP: Global Status Report Session Timeout Set failed\n" );
  }
} /* cpdmTestGlobalStatusSessionTimeoutSet */

/*********************************************************************
*
* @purpose  Show captive portal global status configuration values.
*
* @param    none
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmTestGlobalStatusShow()
{
  L7_char8                      cpMode = L7_DISABLE;
  L7_uint32                     reportInterval = 0;
  L7_uint32                     sessionTimeout = 0;

  if (L7_SUCCESS == cpdmGlobalModeGet(&cpMode))
      sysapiPrintf("CP Global Status Enabled Mode: %d\n", cpMode);
  if (L7_SUCCESS == cpdmGlobalStatusReportIntervalGet(&reportInterval))
      sysapiPrintf("CP Global Status Report Interval: %d\n", reportInterval);
  if (L7_SUCCESS == cpdmGlobalStatusSessionTimeoutGet(&sessionTimeout))
      sysapiPrintf("CP Global Status Session Timeout: %d\n", sessionTimeout);

  sysapiPrintf("\n\n");
} /* cpdmTestGlobalStatusShow */


 
 /*********************************************************************
 *
 * @purpose  Test access of password in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uchar8 * password @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntryPasswordSet(L7_uchar8 *loginName, L7_char8 *password)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else if (!password)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmUserEntryPasswordSet(loginName, fieldName);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "password");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of groupName in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uchar8 * groupName @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntryGroupNameSet(L7_uchar8 *loginName, L7_char8 *groupName)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else if (!groupName)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmUserEntryGroupNameSet(loginName, fieldName);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "groupName");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of sessionTimeout in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uint32 sessionTimeout @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntrySessionTimeoutSet(L7_uchar8 *loginName, L7_uint32 sessionTimeout)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else
   {
      rc = cpdmUserEntrySessionTimeoutSet(loginName, sessionTimeout);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "sessionTimeout");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of idleTimeout in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uint32 idleTimeout @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntryIdleTimeoutSet(L7_uchar8 *loginName, L7_uint32 idleTimeout)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else
   {
      rc = cpdmUserEntryIdleTimeoutSet(loginName, idleTimeout);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "idleTimeout");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of maxBandwidthUp in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uint32 maxBandwidthUp @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntryMaxBandwidthUpSet(L7_uchar8 *loginName, L7_uint32 maxBandwidthUp)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else
   {
      rc = cpdmUserEntryMaxBandwidthUpSet(loginName, maxBandwidthUp);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "maxBandwidthUp");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of maxBandwidthDown in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uint32 maxBandwidthDown @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntryMaxBandwidthDownSet(L7_uchar8 *loginName, L7_uint32 maxBandwidthDown)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else
   {
      rc = cpdmUserEntryMaxBandwidthDownSet(loginName, maxBandwidthDown);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "maxBandwidthDown");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of maxInputOctets in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uint32 maxInputOctets @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntryMaxInputOctetsSet(L7_uchar8 *loginName, L7_uint32 maxInputOctets)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else
   {
      rc = cpdmUserEntryMaxInputOctetsSet(loginName, maxInputOctets);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "maxInputOctets");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of maxOutputOctets in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uint32 maxOutputOctets @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntryMaxOutputOctetsSet(L7_uchar8 *loginName, L7_uint32 maxOutputOctets)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else
   {
      rc = cpdmUserEntryMaxOutputOctetsSet(loginName, maxOutputOctets);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "maxOutputOctets");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of maxTotalOctets in user entry 
 *
 * @param    loginName  @b{(input)} login name for user entry
 * @param    L7_uint32 maxTotalOctets @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestUserEntryMaxTotalOctetsSet(L7_uchar8 *loginName, L7_uint32 maxTotalOctets)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!loginName)
   { 
      sysapiPrintf(null_login_name_msg);
   }
   else
   {
      rc = cpdmUserEntryMaxTotalOctetsSet(loginName, maxTotalOctets);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "maxTotalOctets");
      if (loginName) sysapiPrintf( login_name_is, loginName);
   }
   return rc;
}



 /*********************************************************************
 *
 * @purpose  Test access of name in config entry 
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    L7_uchar8 * name @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigNameSet(cpId_t cpId, L7_char8 *name)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!name)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPConfigNameSet(cpId, name);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "name");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of groupName in config entry 
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    L7_uchar8 * groupName @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigGroupNameSet(cpId_t cpId, L7_char8 *groupName)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!groupName)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPConfigGroupNameSet(cpId, groupName);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "groupName");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of HTTP port in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_ushort16 httpPort @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigHttpPortSet(cpId_t cpId, L7_uint16 httpPort)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigHttpPortSet(cpId, httpPort);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "httpPort");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of radiusAccounting in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_BOOL radiusAccounting @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigRadiusAccountingSet(cpId_t cpId, L7_BOOL radiusAccounting)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigRadiusAccountingSet(cpId, radiusAccounting);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "radiusAccounting");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of redirectURL in config entry 
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    L7_uchar8 * redirectURL @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigRedirectURLSet(cpId_t cpId, L7_char8 *redirectURL)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!redirectURL)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPConfigRedirectURLSet(cpId, redirectURL);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "redirectURL");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of redirectMode in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uchar8 redirectMode @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigRedirectModeSet(cpId_t cpId, L7_uchar8 redirectMode)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigRedirectModeSet(cpId, redirectMode);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "redirectMode");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of userUpRate in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uint32 userUpRate @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigUserUpRateSet(cpId_t cpId, L7_uint32 userUpRate)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigUserUpRateSet(cpId, userUpRate);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "userUpRate");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of userDownRate in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uint32 userDownRate @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigUserDownRateSet(cpId_t cpId, L7_uint32 userDownRate)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigUserDownRateSet(cpId, userDownRate);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "userDownRate");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of maxInputOctets in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uint32 maxInputOctets @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigMaxInputOctetsSet(cpId_t cpId, L7_uint32 maxInputOctets)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigMaxInputOctetsSet(cpId, maxInputOctets);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "maxInputOctets");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of maxOutputOctets in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uint32 maxOutputOctets @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigMaxOutputOctetsSet(cpId_t cpId, L7_uint32 maxOutputOctets)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigMaxOutputOctetsSet(cpId, maxOutputOctets);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "maxOutputOctets");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of maxTotalOctets in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uint32 maxTotalOctets @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigMaxTotalOctetsSet(cpId_t cpId, L7_uint32 maxTotalOctets)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigMaxTotalOctetsSet(cpId, maxTotalOctets);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "maxTotalOctets");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of sessionTimeout in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uint32 sessionTimeout @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigSessionTimeoutSet(cpId_t cpId, L7_uint32 sessionTimeout)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigSessionTimeoutSet(cpId, sessionTimeout);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "sessionTimeout");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of idleTimeout in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uint32 idleTimeout @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigIdleTimeoutSet(cpId_t cpId, L7_uint32 idleTimeout)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigIdleTimeoutSet(cpId, idleTimeout);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "idleTimeout");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}

 /*********************************************************************
 *
 * @purpose  Test access of intrusionThreshold in CP config entry
 *
 * @param    cpId  @b{(input)} CP id for config entry
 * @param    L7_uint32 intrusionThreshold @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPConfigIntrusionThresholdSet(cpId_t cpId, L7_uint32 intrusionThreshold)
{
   L7_RC_t rc = L7_FAILURE;
  
   rc = cpdmCPConfigIntrusionThresholdSet(cpId, intrusionThreshold);
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "intrusionThreshold");
      sysapiPrintf( cp_id_is, cpId);
   }
   return rc;
}



 /*********************************************************************
 *
 * @purpose  Test access of imageName in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * imageName @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebImageNameSet(cpId_t cpId, L7_short16 webId, L7_char8 *imageName)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!imageName)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebImageNameSet(cpId, webId, imageName);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "imageName");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of browserTitleText in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * browserTitleText @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebBrowserTitleTextSet(cpId_t cpId, L7_short16 webId, L7_char8 *browserTitleText)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!browserTitleText)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebBrowserTitleTextSet(cpId, webId, browserTitleText);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "browserTitleText");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of langCode in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * langCode @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebLangCodeSet(cpId_t cpId, L7_short16 webId, L7_char8 *langCode)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!langCode)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebLangCodeSet(cpId, webId, langCode);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "langCode");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of localeLink in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * localeLink @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebLocaleLinkSet(cpId_t cpId, L7_short16 webId, L7_char8 *localeLink)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!localeLink)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebLocaleLinkSet(cpId, webId, localeLink);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "localeLink");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of titleText in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * titleText @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebTitleTextSet(cpId_t cpId, L7_short16 webId, L7_char8 *titleText)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!titleText)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebTitleTextSet(cpId, webId, titleText);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "titleText");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of accountLabel in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * accountLabel @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebAccountLabelSet(cpId_t cpId, L7_short16 webId, L7_char8 *accountLabel)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!accountLabel)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebAccountLabelSet(cpId, webId, accountLabel);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "accountLabel");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of userLabel in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * userLabel @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebUserLabelSet(cpId_t cpId, L7_short16 webId, L7_char8 *userLabel)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!userLabel)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebUserLabelSet(cpId, webId, userLabel);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "userLabel");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of passwordLabel in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * passwordLabel @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebPasswordLabelSet(cpId_t cpId, L7_short16 webId, L7_char8 *passwordLabel)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!passwordLabel)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebPasswordLabelSet(cpId, webId, passwordLabel);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "passwordLabel");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of buttonLabel in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * buttonLabel @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebButtonLabelSet(cpId_t cpId, L7_short16 webId, L7_char8 *buttonLabel)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!buttonLabel)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebButtonLabelSet(cpId, webId, buttonLabel);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "buttonLabel");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of instructionalText in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * instructionalText @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebInstructionalTextSet(cpId_t cpId, L7_short16 webId, L7_char8 *instructionalText)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!instructionalText)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebInstructionalTextSet(cpId, webId, instructionalText);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "instructionalText");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of aupText in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * aupText @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebAupTextSet(cpId_t cpId, L7_short16 webId, L7_char8 *aupText)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!aupText)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebAupTextSet(cpId, webId, aupText);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "aupText");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of acceptText in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * acceptText @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebAcceptTextSet(cpId_t cpId, L7_short16 webId, L7_char8 *acceptText)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!acceptText)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebAcceptTextSet(cpId, webId, acceptText);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "acceptText");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of noAcceptMsg in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * noAcceptMsg @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebNoAcceptMsgTextSet(cpId_t cpId, L7_short16 webId, L7_char8 *noAcceptMsg)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!noAcceptMsg)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebNoAcceptMsgTextSet(cpId, webId, noAcceptMsg);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "noAcceptMsg");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of wipMsg in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * wipMsg @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebWipMsgSet(cpId_t cpId, L7_short16 webId, L7_char8 *wipMsg)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!wipMsg)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebWipMsgSet(cpId, webId, wipMsg);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "wipMsg");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of deniedMsg in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * deniedMsg @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebDeniedMsgSet(cpId_t cpId, L7_short16 webId, L7_char8 *deniedMsg)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!deniedMsg)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebDeniedMsgSet(cpId, webId, deniedMsg);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "deniedMsg");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of resourceMsg in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * resourceMsg @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebResourceMsgSet(cpId_t cpId, L7_short16 webId, L7_char8 *resourceMsg)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!resourceMsg)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebResourceMsgSet(cpId, webId, resourceMsg);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "resourceMsg");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of timeoutMsg in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * timeoutMsg @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebTimeoutMsgSet(cpId_t cpId, L7_short16 webId, L7_char8 *timeoutMsg)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!timeoutMsg)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebTimeoutMsgSet(cpId, webId, timeoutMsg);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "timeoutMsg");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of welcomeTitleText in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * welcomeTitleText @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebWelcomeTitleTextSet(cpId_t cpId, L7_short16 webId, L7_char8 *welcomeTitleText)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!welcomeTitleText)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebWelcomeTitleTextSet(cpId, webId, welcomeTitleText);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "welcomeTitleText");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of welcomeText in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 * welcomeText @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWebWelcomeTextSet(cpId_t cpId, L7_short16 webId, L7_char8 *welcomeText)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!welcomeText)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWebWelcomeTextSet(cpId, webId, welcomeText);
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "welcomeText");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}


 /*********************************************************************
 *
 * @purpose  Test access of  in custom locale
 *
 * @param    cpId  @b{(input)} cp id of config entry
 * @param    webId  @b{(input)} web id of locale
 * @param    L7_uchar8 *  @b{(input)} new value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cpdmTestCPWeb(cpId_t cpId, L7_short16 webId, L7_char8 *)
{
   L7_RC_t rc = L7_FAILURE;
  
   if (!)
   { 
      sysapiPrintf(bad_input_ptr);
   }
   else
   {
      rc = cpdmCPWeb(cpId, webId, );
   }
 
   if (rc != L7_SUCCESS)
   {
      sysapiPrintf( call_to_set_failed, "");
      sysapiPrintf( cp_and_web_id_are, cpId, webId);
   }
   return rc;
}




#endif  /* L7_CPDM_TEST */
