/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpdm_api.c
*
* @purpose      Captive Portal Data Manager (CPDM) API functions
*
* @component    CPDM
*
* @comments     APIs for usmDb layer. These APIs can be called via usmDb
*               and by the captive portal components.
*
* @create       06/28/2007
*
* @author       darsenault,dcaugherty
*
* @end
*
**********************************************************************/

#include <string.h>
#include "osapi.h"
#include "l7_ip_api.h"
#include "pw_scramble_api.h"
#include "intf_cb_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_common.h"
#include "cpdm.h"
#include "nimapi.h"
#include "cpdm.h"
#include "cpdm_api.h"
#include "cpdm_user_api.h"
#include "cpdm_web_api.h"
#include "cpdm_connstatus_util.h"
#include "cpdm_connstatus_api.h"
#include "cpim_api.h"
#include "log.h"
#include "usmdb_radius_api.h"
#include "usmdb_sim_api.h"
#ifdef L7_MGMT_SECURITY_PACKAGE
#include "usmdb_sslt_api.h"
#endif
#include "l7utils_api.h"

typedef enum {
  FIND_ASSOC_EXACT = 0,
  FIND_ASSOC_NEXT,
  FIND_ASSOC_ADD
} find_assoc_t;

static L7_uint32         assocCount = 0;

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);

static L7_int32 cpdmFindIntfAssocIndex(cpId_t cpId, L7_uint32 ifNum, find_assoc_t how);
static ifNumCPPair_t * cpdmFindIntfAssoc(cpId_t cpId, L7_uint32 ifNum);
static L7_RC_t cpdmCPConfigModeAllUpdate(void);
static L7_RC_t cpdmCPConfigModeUpdate(cpId_t cpId);
static L7_RC_t cpdmCPStatusBlockedUpdate(cpId_t cpId);
static L7_RC_t cpdmCPStatusBlockedAllUpdate(void);
static L7_RC_t cpdmCPConfigIntIfNumUpdate(L7_uint32 intIfNum, L7_CP_MODE_STATUS_t mode);

static void cpdmCPConfigHelperReset(cpdmConfigData_t * pData);
#ifdef L7_MGMT_SECURITY_PACKAGE
static L7_BOOL cpdmGlobalCertStatusChange(void);
#endif

extern void     cpdmActStatusFinalize(void);
extern void     cpcmAIPStatusFinalize(void);
extern L7_RC_t  cpcmAIPStatusCleanup(L7_uint32 intIfNum);
extern L7_RC_t  cpdmActStatusAdd(cpId_t);
extern L7_RC_t  cpdmActStatusDelete(cpId_t);

/*********************************************************************
* Global Captive Portal Configuration
*********************************************************************/


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

L7_RC_t cpdmCfgGlobalModeGet(L7_uint32 *mode)
{
  if (mode == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);
  *mode = cpdmCfgData->cpGlobalData.cpMode;
  SEMA_GIVE(READ);

  return L7_SUCCESS;
}



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
L7_RC_t cpdmGlobalModeApply(L7_uint32 mode, L7_BOOL force)
{
  L7_CP_MODE_STATUS_t old_status, status;
  L7_RC_t rc = L7_SUCCESS;

  if (L7_NULLPTR == cpdmOprData)
  {
    return L7_FAILURE;
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
{
  L7_uint32 port;
  L7_uint32 unit = usmDbThisUnitGet();

  if (L7_SUCCESS == cpdmGlobalHTTPSecurePort1Get(&port))
  {
    if (L7_SUCCESS != usmDbssltAuxSecurePort1Set(unit,port))
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update SSLT with default auxiliary port:%d\n", __FUNCTION__,port);
    }
  }
  if (L7_SUCCESS == cpdmGlobalHTTPSecurePort2Get(&port))
  {
    if (L7_SUCCESS != usmDbssltAuxSecurePort2Set(unit,port))
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update SSLT with additional auxiliary port:%d\n", __FUNCTION__,port);
    }
  }
  if (L7_SUCCESS != usmDbssltAuxModeSet(mode))
  {
    /* Don't complain here.
       The set could fail simply because the mode hasn't changed or
       the certificate might be bad for which there are other indicators
    */
  }
}
#endif

  SEMA_TAKE(READ);
  old_status = cpdmOprData->cpGlobalStatus.status;
  SEMA_GIVE(READ);

  switch(mode)
  {
  case L7_DISABLE:
    status = L7_CP_MODE_DISABLED;
    break;
  case L7_ENABLE:
    status = L7_CP_MODE_ENABLED;
    break;
  default:
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  cpdmOprData->cpGlobalStatus.status = status;
  SEMA_GIVE(WRITE);

  /* call below will zap client connections */

  if ((status != old_status) ||
#ifdef L7_MGMT_SECURITY_PACKAGE
      (L7_TRUE == cpdmGlobalCertStatusChange()) ||
#endif
      (L7_TRUE == force))
  {
    rc = cpdmCPConfigModeAllUpdate();
  }

  if (L7_SUCCESS == rc)
  {
    rc = cpdmCPStatusBlockedAllUpdate();
  }

  return L7_SUCCESS;
}


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
L7_RC_t cpdmGlobalModeSet(L7_uint32 mode)
{
  if ((L7_NULLPTR == cpdmCfgData) ||
      (L7_FALSE == captivePortalCnfgrComplete()))
  {
    return L7_FAILURE;
  }

  switch(mode)
  {
  case L7_DISABLE:
  case L7_ENABLE:
    break;
  default:
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  if (cpdmCfgData->cpGlobalData.cpMode != mode)
  {
    cpdmCfgData->cpGlobalData.cpMode = mode;
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
  }
  SEMA_GIVE(WRITE);
  (void) captivePortalTaskWake(); /* don't need return value here */
  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalModeGet(L7_uint32 *mode)
{
  if (mode == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (L7_TRUE == captivePortalCnfgrComplete())
  {
    SEMA_TAKE(READ);
    *mode = cpdmCfgData->cpGlobalData.cpMode;
    SEMA_GIVE(READ);
  }
  else
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the global peer switch stats report interval for captive
*           portal.
*
* @param    L7_unint32 @b{(input)} interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalStatusPeerSwStatsReportIntervalSet(L7_uint32 interval)
{
  SEMA_TAKE( WRITE)
  if (cpdmCfgData->cpGlobalData.peerSwitchStatsReportInterval != interval)
  {
    cpdmCfgData->cpGlobalData.peerSwitchStatsReportInterval = interval;
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
  }
  SEMA_GIVE( WRITE)
  return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose  Set the global session timeout for captive portal.
*
* @param    L7_unint32 @b{(input)} timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalStatusSessionTimeoutSet(L7_uint32 timeout)
{
  if ((timeout < CP_AUTH_SESSION_TIMEOUT_MIN) ||
      (timeout > CP_AUTH_SESSION_TIMEOUT_MAX))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE( WRITE)
  if (cpdmCfgData->cpGlobalData.sessionTimeout != timeout)
  {
    cpdmCfgData->cpGlobalData.sessionTimeout = timeout;
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
  }
  SEMA_GIVE( WRITE)
  return L7_SUCCESS;
} /* cpdmGlobalStatusSessionTimeoutSet */


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
L7_RC_t cpdmGlobalTrapModeSet(CP_TRAP_FLAGS_t traps,  L7_uint32 mode)
{
  L7_RC_t  rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do
  {
    L7_uint32 oldval = cpdmCfgData->cpGlobalData.trapFlags;
    switch (mode)
    {
      case L7_ENABLE:
        cpdmCfgData->cpGlobalData.trapFlags |= traps;
        rc = L7_SUCCESS;
    break;
      case L7_DISABLE:
        cpdmCfgData->cpGlobalData.trapFlags &= ~(traps);
        rc = L7_SUCCESS;
    break;
      default: /* will return FAILURE in this case */
    break;
    }
    if (oldval != cpdmCfgData->cpGlobalData.trapFlags)
    {
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
  } while (0);
  SEMA_GIVE(WRITE);

  return rc;
}


/*********************************************************************
*
* @purpose  Get the global admin mode status for captive portal.
*
* @param    mode @b{(output)} mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalStatusModeGet(L7_CP_MODE_STATUS_t *mode)
{
  if (mode == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);
  if (L7_TRUE == captivePortalCnfgrComplete())
  {
    *mode = cpdmOprData->cpGlobalStatus.status;
  }
  else
  {
    *mode = L7_CP_MODE_DISABLED;
  }

  SEMA_GIVE(READ);
  return L7_SUCCESS;

} /* cpdmGlobalStatusModeGet */

/*********************************************************************
*
* @purpose  Set the global peer switch stats report interval for captive
*           portal.
*
* @param    interval @b{(output)} peer WS stats report interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalStatusPeerSwStatsReportIntervalGet(L7_uint32 *interval)
{
  if (interval == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *interval = cpdmCfgData->cpGlobalData.peerSwitchStatsReportInterval;
  SEMA_GIVE(READ)
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose  Get the global session timeout for captive portal.
*
* @param    timeout @b{(output)} session timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalStatusSessionTimeoutGet(L7_uint32 *timeout)
{
  if ((L7_NULLPTR == cpdmCfgData) || (L7_NULLPTR == timeout))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(READ)
  *timeout  = cpdmCfgData->cpGlobalData.sessionTimeout;
  SEMA_GIVE(READ)
  return L7_SUCCESS;
} /* cpdmGlobalStatusSessionTimeoutGet */

/*********************************************************************
*
* @purpose  Set the additional HTTP port
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
L7_RC_t cpdmGlobalHTTPPortSet(L7_uint32 httpPort)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 intfId = 0;
  L7_RC_t   loop_rc = L7_SUCCESS;
  intfCpCallbacks_t  * pCBs = L7_NULLPTR;
  static L7_uchar8 * partialFmt =
    "%s set Captive Portal auth port to %d (intf %d)\n";


  if (cpdmCfgData->cpGlobalData.httpPort != httpPort)
  {

    /* Now set the port value on all known CP-capable ports */

    rc = cpdmIntfStatusNextGet(intfId, &intfId);

    while (L7_SUCCESS == rc)
    {
      /* get NIM callbacks for this intf id */
      pCBs = cpimIntfOwnerCallbacksGet(intfId);

      /* invoke if they exist; log a msg if they don't */
      if (!pCBs || !pCBs->cpSetAuthPort)
      {
        loop_rc = L7_FAILURE;
        CP_DLOG(CPD_LEVEL_LOG, partialFmt, "No way to", httpPort, intfId);
      }
      else if (L7_SUCCESS != pCBs->cpSetAuthPort(intfId, httpPort))
      {
        loop_rc = L7_FAILURE;
        CP_DLOG(CPD_LEVEL_LOG, partialFmt, "Can't", httpPort, intfId);
      }
      rc = cpdmIntfStatusNextGet(intfId, &intfId);
    }
    rc = loop_rc;

    if (L7_SUCCESS == rc)
    {
      SEMA_TAKE(WRITE);
      if (cpdmCfgData->cpGlobalData.httpPort != httpPort)
      {
        cpdmCfgData->cpGlobalData.httpPort = httpPort;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      SEMA_GIVE(WRITE);
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the additional HTTP port
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
L7_RC_t cpdmGlobalHTTPPortGet(L7_uint32 *httpPort)
{
  if (L7_NULLPTR == httpPort)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *httpPort = cpdmCfgData->cpGlobalData.httpPort;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
}

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
* @comments Modified so we can set the port if we have at least one
*           interface we set. We shouldn't disable the secure port
*           for the system because of a non-compatible interface.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalHTTPSecurePort1Set(L7_uint32 httpSecurePort)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 intfId = 0;
  L7_RC_t   loop_rc = L7_FAILURE;
  intfCpCallbacks_t  * pCBs = L7_NULLPTR;
  static L7_uchar8 * partialFmt =
    "%s set Captive Portal additional secure port to %d (intf %d)\n";
  L7_uint32 unit = usmDbThisUnitGet();

  if (cpdmCfgData->cpGlobalData.httpsPort1 != httpSecurePort)
  {
    /* Now set the port value on all known CP-capable ports */
    rc = cpdmIntfStatusNextGet(intfId, &intfId);

    while (L7_SUCCESS == rc)
    {
      /* get NIM callbacks for this intf id */
      pCBs = cpimIntfOwnerCallbacksGet(intfId);

      /* invoke if they exist; log a msg if they don't */
      if (!pCBs || !pCBs->cpSetAuthSecurePort1)
      {
        CP_DLOG(CPD_LEVEL_LOG, partialFmt, "No way to", httpSecurePort, intfId);
      }
      else if (L7_SUCCESS != pCBs->cpSetAuthSecurePort1(intfId, httpSecurePort))
      {
        CP_DLOG(CPD_LEVEL_LOG, partialFmt, "Can't", httpSecurePort, intfId);
      }
      rc = cpdmIntfStatusNextGet(intfId, &intfId);
      if (L7_SUCCESS == rc)
      {
        loop_rc = L7_SUCCESS; /* If we set at least one interface */
      }
    }
    rc = loop_rc;

    if (L7_SUCCESS == rc)
    {
      SEMA_TAKE(WRITE);
      if (cpdmCfgData->cpGlobalData.httpsPort1 != httpSecurePort)
      {
        cpdmCfgData->cpGlobalData.httpsPort1 = httpSecurePort;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      SEMA_GIVE(WRITE);
      rc = usmDbssltAuxSecurePort1Set(unit,httpSecurePort);
    }
  }

  return rc;
}
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Get the default HTTP secure port
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
L7_RC_t cpdmGlobalHTTPSecurePort1Get(L7_uint32 *httpSecurePort)
{
  if (L7_NULLPTR == httpSecurePort)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *httpSecurePort = cpdmCfgData->cpGlobalData.httpsPort1;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
}
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Set the additonal HTTP secure port
*
* @param    L7_uint32 httpSecurePort @b{(input)} HTTP secure port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Modified so we can set the port if we have at least one
*           interface we set. We shouldn't disable the secure port
*           for the system because of a non-compatible interface.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalHTTPSecurePort2Set(L7_uint32 httpSecurePort)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 intfId = 0;
  L7_RC_t   loop_rc = L7_FAILURE;
  intfCpCallbacks_t  * pCBs = L7_NULLPTR;
  static L7_uchar8 * partialFmt =
    "%s set Captive Portal additional secure port to %d (intf %d)\n";
  L7_uint32 unit = usmDbThisUnitGet();

  if (cpdmCfgData->cpGlobalData.httpsPort2 != httpSecurePort)
  {
    /* Now set the port value on all known CP-capable ports */
    rc = cpdmIntfStatusNextGet(intfId, &intfId);

    while (L7_SUCCESS == rc)
    {
      /* get NIM callbacks for this intf id */
      pCBs = cpimIntfOwnerCallbacksGet(intfId);

      /* invoke if they exist; log a msg if they don't */
      if (!pCBs || !pCBs->cpSetAuthSecurePort2)
      {
        CP_DLOG(CPD_LEVEL_LOG, partialFmt, "No way to", httpSecurePort, intfId);
      }
      else if (L7_SUCCESS != pCBs->cpSetAuthSecurePort2(intfId, httpSecurePort))
      {
        CP_DLOG(CPD_LEVEL_LOG, partialFmt, "Can't", httpSecurePort, intfId);
      }
      rc = cpdmIntfStatusNextGet(intfId, &intfId);
      if (L7_SUCCESS == rc)
      {
        loop_rc = L7_SUCCESS; /* If we set at least one interface */
      }
    }
    rc = loop_rc;

    if (L7_SUCCESS == rc)
    {
      SEMA_TAKE(WRITE);
      if (cpdmCfgData->cpGlobalData.httpsPort2 != httpSecurePort)
      {
        cpdmCfgData->cpGlobalData.httpsPort2 = httpSecurePort;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      SEMA_GIVE(WRITE);
      rc = usmDbssltAuxSecurePort2Set(unit,httpSecurePort);
    }
  }
  return rc;
}
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Get the additional HTTP secure port
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
L7_RC_t cpdmGlobalHTTPSecurePort2Get(L7_uint32 *httpSecurePort)
{
  if (L7_NULLPTR == httpSecurePort)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *httpSecurePort = cpdmCfgData->cpGlobalData.httpsPort2;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalTrapModeGet(CP_TRAP_FLAGS_t trapToTest,
                  L7_uint32 * mode)
{
  L7_RC_t  rc = L7_FAILURE;


  SEMA_TAKE(READ);
  do
  {
    if (L7_NULLPTR == mode)
    {
      break;
    }

    if (0 != (cpdmCfgData->cpGlobalData.trapFlags & trapToTest))
    {
      *mode = L7_ENABLE;
    }
    else
    {
      *mode = L7_DISABLE;
    }
    rc = L7_SUCCESS;
  } while (0);
  SEMA_GIVE(READ);

  return rc;
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
L7_RC_t cpdmGlobalCPDisableReasonGet(L7_CP_MODE_REASON_t *reason)
{
  if (L7_NULLPTR == reason)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *reason = cpdmOprData->cpGlobalStatus.reason;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
}


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
void cpdmGlobalCPDisableReasonSet(L7_CP_MODE_REASON_t reason)
{
  SEMA_TAKE(READ)
  cpdmOprData->cpGlobalStatus.reason = reason;
  SEMA_GIVE(READ)
}

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
L7_RC_t cpdmGlobalCPIPAddressGet(L7_IP_ADDR_t *ipAddr, L7_uint32 *ipMask)
{
  if (L7_NULLPTR == ipAddr)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *ipAddr = cpdmOprData->cpGlobalStatus.address;
  *ipMask = cpdmOprData->cpGlobalStatus.mask;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
}


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
void cpdmGlobalCPIPAddressSet(L7_IP_ADDR_t ipAddr, L7_uint32 ipMask)
{
  SEMA_TAKE(WRITE);
  cpdmOprData->cpGlobalStatus.address = ipAddr;
  cpdmOprData->cpGlobalStatus.mask = ipMask;
  SEMA_GIVE(WRITE);
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
L7_RC_t cpdmGlobalSupportedCPInstancesGet(L7_uint32 *supportedCPs)
{
  if (L7_NULLPTR == supportedCPs)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *supportedCPs = cpdmOprData->cpGlobalStatus.portals.supported;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalConfiguredCPInstancesGet(L7_uint32 *configCPs)
{
  if (L7_NULLPTR == configCPs)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *configCPs = cpdmOprData->cpGlobalStatus.portals.configured;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalActiveCPInstancesGet(L7_uint32 *activeCPs)
{
  L7_uint32 mode;

  if ((L7_NULLPTR == activeCPs) ||
      (L7_SUCCESS != cpdmCfgGlobalModeGet(&mode)))
  {
    return L7_FAILURE;
  }

  if (L7_CP_MODE_ENABLED == mode)
  {
    cpdmActStatusOperEnabledCount(activeCPs);
  }
  else
  {
    *activeCPs = 0;
  }
  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalSupportedUsersGet(L7_uint32 *supportedUsers)
{
  if (L7_NULLPTR == supportedUsers)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *supportedUsers = cpdmOprData->cpGlobalStatus.users.supported;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalLocalUsersGet(L7_uint32 *localUsers)
{
  if (L7_NULLPTR == localUsers)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *localUsers = cpdmOprData->cpGlobalStatus.users.local;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalAuthenticatedUsersGet(L7_uint32 *authUsers)
{
  if (L7_NULLPTR == authUsers)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *authUsers = cpdmOprData->cpGlobalStatus.users.authenticated;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalAuthFailureMaxUsersGet(L7_uint32 *authFailSize)
{
  if (L7_NULLPTR == authFailSize)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(READ)
  *authFailSize = cpdmOprData->cpGlobalStatus.authFailureUsers.capacity;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalAuthFailureUsersGet(L7_uint32 *authFailUsers)
{
  if (L7_NULLPTR == authFailUsers)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(READ)
  *authFailUsers = cpdmOprData->cpGlobalStatus.authFailureUsers.size;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalActivityLogMaxEntriesGet(L7_uint32 *actLogSize)
{
  if (L7_NULLPTR == actLogSize)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *actLogSize = cpdmOprData->cpGlobalStatus.activityLog.capacity;
  SEMA_GIVE(READ)

  return L7_SUCCESS;
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
L7_RC_t cpdmGlobalActivityLogEntriesGet(L7_uint32 *actLogEntries)
{
  if (L7_NULLPTR == actLogEntries)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ)
  *actLogEntries = cpdmOprData->cpGlobalStatus.activityLog.size;
  SEMA_GIVE(READ)
  return L7_SUCCESS;
}

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Update the CP instances on HTTPS certificate change.
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
L7_BOOL cpdmGlobalCertStatusChange(void)
{
  L7_BOOL certStatus;
  L7_BOOL gCertStatus;
  L7_BOOL rc = L7_FALSE;

  SEMA_TAKE(READ)
  certStatus = cpdmOprData->cpGlobalStatus.certStatus;
  SEMA_GIVE(READ)

  gCertStatus = (L7_SUCCESS==usmDbssltCertificateExists(1))?L7_TRUE:L7_FALSE;

  if (certStatus!=gCertStatus)
  {
    SEMA_TAKE(WRITE)
    cpdmOprData->cpGlobalStatus.certStatus = gCertStatus;
    SEMA_GIVE(WRITE)
    rc = L7_TRUE;
  }
  return rc;
}
#endif

/*********************************************************************
* CP Configuration
*********************************************************************/


/*********************************************************************
*
* @purpose  Initialize our CP config array
*
* @param    L7_uint32  maxCPConfigs  @b{(input)} max CPs to support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigInit(void)
{
  memset(&(cpdmCfgData->cpConfigData[0]), 0,
   FD_CP_CONFIG_MAX * sizeof(cpdmConfigData_t));
  SEMA_TAKE(WRITE);
  cpdmOprData->cpGlobalStatus.portals.configured = 0;
  SEMA_GIVE(WRITE);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Finalize our interface association conn status tree
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmCPConfigFinalize(void)
{
  cpdmActStatusFinalize();
  memset(cpdmCfgData->cpConfigData, 0, FD_CP_CONFIG_MAX * sizeof(cpdmConfigData_t));
  SEMA_TAKE(WRITE);
  cpdmOprData->cpGlobalStatus.portals.configured = 0;
  SEMA_GIVE(WRITE);
}



/*********************************************************************
*
* @purpose  Find a tree entry with the given CP id as a key
*
* @param    cpId_t  cpId    @b{(input)} the key, of course
* @param    L7_BOOL getNext @b{(input)} use next value if not found?
*
* @returns  void  pointer to cpdmClientAssocConnStatusData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/

L7_int32 cpdmFindConfigIndex(cpId_t cpId, L7_BOOL getNext)
{
  L7_uint32 idx = 0, configCount;
  L7_int32  rc  = -1;

  if (L7_NULLPTR == cpdmCfgData)
    return rc;

  configCount = cpdmOprData->cpGlobalStatus.portals.configured;
  while(idx < configCount)
  {
    if (0 == cpdmCfgData->cpConfigData[idx].cpId)
    {
      break; /* Out of values */
    }

    if (L7_TRUE == getNext)
    {
      if (cpId < cpdmCfgData->cpConfigData[idx].cpId)
      {
        rc = (L7_int32) idx;
        break;
      }
    }
    else
    {
      if (cpId == cpdmCfgData->cpConfigData[idx].cpId)
      {
        rc = (L7_int32) idx;
        break;
      }
    }
    idx++;
  }
  return rc;
}


cpdmConfigData_t *
cpdmFindConfig(cpId_t cpId, L7_BOOL getNext)
{
  cpdmConfigData_t * pC = L7_NULLPTR;
  L7_int32 idx = cpdmFindConfigIndex(cpId, getNext);

  if (0 <= idx)
  {
    pC = &(cpdmCfgData->cpConfigData[idx]);
  }
  return pC;
}


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
void cpdmCPConfigCountUpdate(void)
{
  L7_uint32 conf_count = 0;

  SEMA_TAKE(WRITE);
  if (L7_NULLPTR != cpdmCfgData)
  {
    for(conf_count = 0; conf_count < CP_ID_MAX; conf_count++)
    {
      if (0 == cpdmCfgData->cpConfigData[conf_count].cpId)
      {
        break;
      }
    }
  }
  cpdmOprData->cpGlobalStatus.portals.configured = conf_count;
  SEMA_GIVE(WRITE);
}


/**************************************************/
/* TODO: Put these 2 routine in a _util.c file.   */
/**************************************************/

static void cpdmCPConfigHelperReset(cpdmConfigData_t * pData)
{
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uchar8 hostName[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uchar8 hostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uint32 unit;

  if (L7_NULLPTR == pData)
  {
    return;
  }

  osapiStrncpySafe(pData->name, CP_DEF_NAME, CP_NAME_MAX);
  osapiStrncpySafe(pData->redirectURL, CP_DEF_URL, CP_WELCOME_URL_MAX);
  osapiStrncpySafe(pData->foregroundColor, CP_DEF_FOREGROUND_COLOR, CP_FOREGROUND_COLOR_MAX);
  osapiStrncpySafe(pData->backgroundColor, CP_DEF_BACKGROUND_COLOR, CP_BACKGROUND_COLOR_MAX);
  osapiStrncpySafe(pData->separatorColor, CP_DEF_SEPARATOR_COLOR, CP_SEPARATOR_COLOR_MAX);

  unit = usmDbThisUnitGet();
  memset(hostAddr,0,sizeof(hostAddr));
  memset(hostName,0,sizeof(hostName));
  if (L7_SUCCESS == usmDbRadiusServerFirstIPHostNameGet(unit,hostAddr,&addrType))
  {
    usmDbRadiusServerHostNameGet(unit,hostAddr,addrType,hostName);
  }
  else
  {
    strcpy(hostName,L7_RADIUS_SERVER_DEFAULT_NAME_AUTH);
  }
  osapiStrncpySafe(pData->radiusAuthServer,hostName,sizeof(pData->radiusAuthServer));

  pData->gpId = GP_ID_MIN;
  pData->httpPort = 0;
  pData->enabled = L7_CP_MODE_ENABLED;
  pData->protocolMode = CP_DEF_PROTOCAL_MODE;
  pData->verifyMode = CP_DEF_VERIFY_MODE;
  pData->userLogoutMode = CP_DEF_USER_LOGOUT_MODE;
  pData->radiusAccounting = CP_DEF_RADIUS_ACCOUNTING;
  pData->redirectMode = CP_DEF_REDIRECT_MODE;
  pData->userUpRate   = CP_DEF_USER_UP_RATE;
  pData->userDownRate = CP_DEF_USER_DOWN_RATE;
  pData->maxInputOctets  = CP_DEF_USER_INPUT_OCTETS;
  pData->maxOutputOctets = CP_DEF_USER_OUTPUT_OCTETS;
  pData->maxTotalOctets  = CP_DEF_TOTAL_OCTETS;
  pData->sessionTimeout = CP_DEF_SESSION_TIMEOUT;
  pData->idleTimeout = CP_DEF_IDLE_TIMEOUT;
  pData->intrusionThreshold = CP_DEF_INTRUSION_THRESHOLD;
}

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

L7_RC_t cpdmCPConfigAdd(cpId_t cpId)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmConfigData_t *pData = L7_NULLPTR;
  cpdmConfigData_t *pBase = L7_NULLPTR;
  webId_t defWebId = 1;
  L7_uint32 idx;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: null pointers\n", __FUNCTION__);
    return rc;
  }

  if ((cpId < CP_ID_MIN) || (cpId > CP_ID_MAX))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: BAD cp: %d\n", __FUNCTION__, cpId);
    return rc;
  }

  SEMA_TAKE(WRITE);
  do
  {
    L7_uint32 configCount = cpdmOprData->cpGlobalStatus.portals.configured;
    if (CP_ID_MAX == configCount)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: full house\n", __FUNCTION__);
      break;
    }

    pBase = &(cpdmCfgData->cpConfigData[0]);
    for(idx = 0; idx < configCount; idx++)
    {
      pData = &(pBase[idx]);
      if (0 == pData->cpId)
      {
        break; /* at end of entries, found our index */
      }
      else if (cpId == pData->cpId)
      {
        rc = L7_ALREADY_CONFIGURED;
        break;
      }
      else if (cpId < pData->cpId)
      {
        break;
      }
    }

    if (L7_ALREADY_CONFIGURED == rc)
    {
      CP_DLOG(CPD_LEVEL_LOG,
         "%s: %d already here\n", __FUNCTION__, cpId);
      break;
    }

    if (configCount != idx)
    {
      /* push up later entries */
      memmove(&(pBase[idx + 1]), &(pBase[idx]),
        (configCount - idx) * sizeof(cpdmConfigData_t));
    }
    else
    {
      pData = &(pBase[idx]);
    }

    memset(pData, 0, sizeof(cpdmConfigData_t));
    pData->cpId = cpId;

    cpdmCPConfigHelperReset(pData);
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    cpdmOprData->cpGlobalStatus.portals.configured++;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  /* New CP Config, so begin a new WEB Id */
  if (L7_SUCCESS == rc)
  {
    rc = cpdmCPConfigWebIdAdd(cpId, defWebId);
    if (L7_SUCCESS != rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "Could not add default Web Id for CP Id %d",cpId);
      return rc;
    }
  }

  if (L7_SUCCESS == rc)
  {
    rc = cpdmCPConfigWebAddDefaults(cpId, defWebId, WEB_DEF1_LANG_CODE, CP_ALL);
  }

  if (L7_SUCCESS == rc)
  {
    do {
      rc = cpdmActStatusAdd(cpId);
      if (L7_SUCCESS != rc)
      {
        CP_DLOG(CPD_LEVEL_LOG, "Failed to create intf assoc status table for CP ID %d",cpId);
        break;
      }
      else
      {
        rc = cpdmCPConfigOperStatusSet(cpId, L7_CP_INST_OPER_STATUS_DISABLED);
      }
    } while (0);
  }

  if ((L7_SUCCESS == rc) && captivePortalCnfgrComplete())
  {
    rc = cpdmCPConfigModeUpdate(cpId);
  }

  return rc;
}


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
L7_RC_t cpdmCPConfigReset(cpId_t cpId)
{
  L7_RC_t rc = L7_FAILURE;
  webId_t defWebId = 1;

  if (L7_NULLPTR == cpdmCfgData)
  {
    return rc;
  }

  /* First, delete all locales associated with this CP ID.
   * If the CP ID does not exist, no harm done.
   */
  (void) cpdmCPConfigWebIdAllDelete(cpId);

  SEMA_TAKE(WRITE);
  do
  {
    cpdmConfigData_t  *pConf = L7_NULLPTR;
    /* Find the CP ID entry, fail if not found */
    pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    /* apply default values */
    cpdmCPConfigHelperReset(pConf);
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  /* Create a new WEB Id */
  if (L7_SUCCESS == rc)
  {
    rc = cpdmCPConfigWebIdAdd(cpId, defWebId);
    if (L7_SUCCESS != rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "Could not add default Web Id for CP Id %d",cpId);
      return rc;
    }
  }

  if (L7_SUCCESS == rc)
  {
    rc = cpdmCPConfigWebAddDefaults(cpId, defWebId, WEB_DEF1_LANG_CODE, CP_ALL);
  }

  cpdmClientConnStatusByCPIdDeleteAll(cpId);

  if ((L7_SUCCESS == rc) && captivePortalCnfgrComplete())
  {
    rc = cpdmCPConfigModeUpdate(cpId);
  }
  return rc;
}


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
L7_RC_t cpdmCPConfigDelete(cpId_t cpId)
{
  cpdmConfigData_t   *pBase = L7_NULLPTR;
  L7_uint32          locales = 0;
  L7_RC_t            rc = L7_FAILURE;
  L7_uint32          intIfNum = 0;
  cpId_t             nextCpId = 0;

  do
  {
    if ((L7_NULLPTR == cpdmOprData) || (L7_NULLPTR == cpdmCfgData))
      break;

    if (CP_ID_MIN == cpId)
    {
      CP_DLOG(CPD_LEVEL_LOG, "Refusing to remove default ID");
      break;
    }

    /* First, wipe out any and all connections using this CP ID.
     * Set the mode of this CP to disabled to do this.
     */

    if (L7_SUCCESS != cpdmCPConfigModeSet(cpId, L7_CP_MODE_DISABLED))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Can't update CP ID %d",  cpId);
      break;
    }

    while (L7_SUCCESS ==
      cpdmCPConfigIntIfNumNextGet(cpId, intIfNum, &nextCpId, &intIfNum))
    {
      if (nextCpId != cpId)
        break; /* out of inner while */
      if (cpdmCPConfigIntIfNumDelete(cpId, intIfNum) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        CP_DLOG(CPD_LEVEL_LOG, "Could not disable intf %d, %s", intIfNum, ifName);
      }
    }

    rc = cpdmActStatusDelete(cpId);
    if (L7_SUCCESS != rc)
    {
       CP_DLOG(CPD_LEVEL_LOG, "Failed to delete intf assoc status table for CP ID %d",cpId);
       break;
    }

    rc = L7_SUCCESS; /* so far so good anyway */
  } while(0);

  if (L7_SUCCESS != rc)
  {
    return rc;
  }
  rc = L7_FAILURE; /* Now try the rest */

  locales = cpdmCPConfigWebIdAllDelete(cpId);

  SEMA_TAKE(WRITE);
  pBase = &(cpdmCfgData->cpConfigData[0]);
  do
  {
    L7_int32   idx = cpdmFindConfigIndex(cpId, L7_FALSE);
    L7_uint32  *configCount = &(cpdmOprData->cpGlobalStatus.portals.configured);

    if (0 > idx)
      break; /* Doesn't exist, leave now */

    if (pBase[idx].localeCount != locales)  /* should be equal */
    {
      CP_DLOG(CPD_LEVEL_LOG,
         "%s: CP ID(%u): webID count = %u, deleted %u\n",
         __FUNCTION__, cpId, pBase[idx].localeCount, locales);
    }

    memmove(&(pBase[idx]), &(pBase[idx + 1]),
      (*configCount - idx - 1) * sizeof(cpdmConfigData_t));
    memset(&(pBase[*configCount]), 0, sizeof(cpdmConfigData_t));
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    *configCount -= 1;
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(WRITE);

  return rc;
}

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

L7_RC_t cpdmCPConfigGet(cpId_t cpId)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmConfigData_t  *pConf = L7_NULLPTR;

  SEMA_TAKE(READ);
  pConf = cpdmFindConfig(cpId, L7_FALSE);
  SEMA_GIVE(READ);
  if (L7_NULLPTR != pConf)
    rc = L7_SUCCESS;
  return rc;
}

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
L7_RC_t cpdmCPConfigNextGet(cpId_t cpId, cpId_t * nextCpId)
{
  L7_RC_t rc = L7_FAILURE;

  if ( (L7_NULLPTR == nextCpId) || (L7_NULLPTR == cpdmCfgData))
    return rc;

  SEMA_TAKE(READ);
  do
  {
    L7_int32 idx;
    cpdmConfigData_t *pBase = &(cpdmCfgData->cpConfigData[0]);
    cpdmConfigData_t *pConf = L7_NULLPTR;
    L7_uint32 configCount = cpdmOprData->cpGlobalStatus.portals.configured;
    if (0 == configCount)
      break;
    if (0 == cpId)
    {
      idx = 0;
      pConf = pBase;
    }
    else
    {
      idx = cpdmFindConfigIndex(cpId, L7_TRUE);

      if (0 > idx) /* no such user? */
        break;
      pConf = &(pBase[idx]);
      if (0 == pConf->cpId)
        break;
    }

    *nextCpId = pConf->cpId;
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(READ);
  return rc;
}



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
L7_RC_t cpdmCPConfigNameGet(cpId_t cpId, L7_char8 *name)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == name)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
       break;
    osapiStrncpySafe(name, pConf->name, CP_NAME_MAX+1);
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(READ);
  return rc;
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

L7_RC_t cpdmCPConfigModeGet(cpId_t cpId, L7_CP_MODE_STATUS_t * mode)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == mode)
  {
    return rc;
  }

  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf) break;
    *mode = pConf->enabled;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get CP config protocolMode
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_LOGIN_TYPE_t *protocolMode @b{(output)} protocolMode of
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

L7_RC_t cpdmCPConfigProtocolModeGet(cpId_t cpId, L7_LOGIN_TYPE_t *protocolMode)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == protocolMode)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *protocolMode = pConf->protocolMode;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}


/*********************************************************************
* @purpose  Get CP config verifyMode
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    CP_VERIFY_NODE_t *verifyMode @b{(output)} verifyMode of
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

L7_RC_t cpdmCPConfigVerifyModeGet(cpId_t cpId, CP_VERIFY_MODE_t *verifyMode)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == verifyMode)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *verifyMode = pConf->verifyMode;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
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
L7_RC_t cpdmCPConfigUserLogoutModeGet(cpId_t cpId, L7_uchar8 *userLogoutMode)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == userLogoutMode)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *userLogoutMode = pConf->userLogoutMode;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
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

L7_RC_t cpdmCPConfigGpIdGet(cpId_t cpId, gpId_t *gpId)
{

  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == gpId)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    *gpId = pConf->gpId;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
* @purpose  Get CP config HTTP port
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_ushort16 *httpPort @b{(output)} HTTP Port of
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

L7_RC_t cpdmCPConfigHttpPortGet(cpId_t cpId, L7_ushort16 *httpPort)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == httpPort)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *httpPort = pConf->httpPort;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

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

L7_RC_t cpdmCPConfigRadiusAccountingGet(cpId_t cpId, L7_BOOL *radiusAccounting)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == radiusAccounting)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *radiusAccounting = pConf->radiusAccounting;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
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
L7_RC_t cpdmCPConfigRadiusAuthServerGet(cpId_t cpId, L7_char8 *radiusServer)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == radiusServer)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    osapiStrncpySafe(radiusServer, pConf->radiusAuthServer, CP_RADIUS_AUTH_SERVER_MAX);
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(READ);
  return rc;
}

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

L7_RC_t cpdmCPConfigRedirectURLGet(cpId_t cpId, L7_char8 *redirectURL)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == redirectURL)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
       break;
    osapiStrncpySafe(redirectURL, pConf->redirectURL, CP_WELCOME_URL_MAX+1);
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(READ);
  return rc;
}

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

L7_RC_t cpdmCPConfigRedirectModeGet(cpId_t cpId, L7_uchar8 *redirectMode)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == redirectMode)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *redirectMode = pConf->redirectMode;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

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

L7_RC_t cpdmCPConfigUserUpRateGet(cpId_t cpId, L7_uint32 *userUpRate)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == userUpRate)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *userUpRate = pConf->userUpRate;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

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

L7_RC_t cpdmCPConfigUserDownRateGet(cpId_t cpId, L7_uint32 *userDownRate)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == userDownRate)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *userDownRate = pConf->userDownRate;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

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

L7_RC_t cpdmCPConfigMaxInputOctetsGet(cpId_t cpId, L7_uint32 *maxInputOctets)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == maxInputOctets)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    *maxInputOctets = pConf->maxInputOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}

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

L7_RC_t cpdmCPConfigMaxOutputOctetsGet(cpId_t cpId, L7_uint32 *maxOutputOctets)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == maxOutputOctets)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    *maxOutputOctets = pConf->maxOutputOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}

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
L7_RC_t cpdmCPConfigMaxTotalOctetsGet(cpId_t cpId, L7_uint32 *maxTotalOctets)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == maxTotalOctets)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    *maxTotalOctets = pConf->maxTotalOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}

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

L7_RC_t cpdmCPConfigSessionTimeoutGet(cpId_t cpId, L7_uint32 *sessionTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == sessionTimeout)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *sessionTimeout = pConf->sessionTimeout;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

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

L7_RC_t cpdmCPConfigIdleTimeoutGet(cpId_t cpId, L7_uint32 *idleTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == idleTimeout)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *idleTimeout = pConf->idleTimeout;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

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

L7_RC_t cpdmCPConfigIntrusionThresholdGet(cpId_t cpId, L7_uint32 *intrusionThreshold)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == intrusionThreshold)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)   break;
    *intrusionThreshold = pConf->intrusionThreshold;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
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
L7_RC_t cpdmCPConfigForegroundColorGet(cpId_t cpId, L7_char8 *foregroundColor)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == foregroundColor)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf) break;
    osapiStrncpySafe(foregroundColor, pConf->foregroundColor, CP_FOREGROUND_COLOR_MAX);
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(READ);
  return rc;
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
L7_RC_t cpdmCPConfigBackgroundColorGet(cpId_t cpId, L7_char8 *backgroundColor)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == backgroundColor)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf) break;
    osapiStrncpySafe(backgroundColor, pConf->backgroundColor, CP_BACKGROUND_COLOR_MAX);
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(READ);
  return rc;
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
L7_RC_t cpdmCPConfigSeparatorColorGet(cpId_t cpId, L7_char8 *separatorColor)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == separatorColor)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf) break;
    osapiStrncpySafe(separatorColor, pConf->separatorColor, CP_SEPARATOR_COLOR_MAX);
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(READ);
  return rc;
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

L7_RC_t cpdmCPConfigNameSet(cpId_t cpId, L7_char8 *name)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 len;

  do
  {
    if (L7_NULLPTR == name)
      break;
    len = strlen(name);
    if (len > CP_NAME_MAX)
      break;
    {
      cpdmConfigData_t *pConf = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pConf = cpdmFindConfig(cpId, L7_FALSE);
      if (L7_NULLPTR != pConf)
      {
        if (osapiStrncmp(pConf->name,name,CP_NAME_MAX) != 0)
        {
          osapiStrncpySafe(pConf->name, name, CP_NAME_MAX+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE);
    }
  } while(0);

  return rc;
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
L7_RC_t cpdmCPConfigGpIdSet(cpId_t cpId, gpId_t gpId)
{
  L7_RC_t rc = L7_FAILURE;

  if ((cpId < CP_ID_MIN) ||
      (cpId > CP_ID_MAX))
  {
    return L7_FAILURE;
  }

  if ((gpId < GP_ID_MIN) ||
      (gpId > GP_ID_MAX))
  {
    return L7_FAILURE;
  }

  do
  {
    cpdmConfigData_t *pConf = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR != pConf)
    {
      if (pConf->gpId != gpId)
      {
        pConf->gpId = gpId;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while(0);

  return rc;
}

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
L7_RC_t cpdmCPConfigProtocolModeSet(cpId_t cpId, L7_LOGIN_TYPE_t protocolMode)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->protocolMode != protocolMode)
    {
      pConf->protocolMode = protocolMode;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

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
L7_RC_t cpdmCPConfigVerifyModeSet(cpId_t cpId, CP_VERIFY_MODE_t verifyMode)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->verifyMode != verifyMode)
    {
      pConf->verifyMode = verifyMode;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
 *
 * @purpose  Set CP config user logout mode
 *
 * @param    cpId_t cpId @b{(input)} CP config id
 * @param    L7_uchar8 *userLogoutMode @b{(input)} logout mode
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmCPConfigUserLogoutModeSet(cpId_t cpId, L7_uchar8 userLogoutMode)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do
  {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }

    if (pConf->userLogoutMode != userLogoutMode)
    {
      pConf->userLogoutMode = userLogoutMode;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;

  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
* @purpose  Set CP config HTTP port
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_ushort16 HTTP port @b{(input)} value for
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
L7_RC_t cpdmCPConfigHttpPortSet(cpId_t cpId, L7_ushort16 httpPort)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->httpPort != httpPort)
    {
      pConf->httpPort = httpPort;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Set CP config radiusAccounting
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_BOOL radiusAccounting @b{(input)} value for
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
L7_RC_t cpdmCPConfigRadiusAccountingSet(cpId_t cpId, L7_BOOL radiusAccounting)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->radiusAccounting != radiusAccounting)
    {
      pConf->radiusAccounting = radiusAccounting;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
* @purpose  Set CP config RADIUS authentication server
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *radiusServer @b{(input)} - new
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
L7_RC_t cpdmCPConfigRadiusAuthServerSet(cpId_t cpId, L7_char8 *radiusServer)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 len;

  do
  {
    if (L7_NULLPTR == radiusServer)
    {
      break;
    }
    len = strlen(radiusServer);
    if (len > CP_RADIUS_AUTH_SERVER_MAX)
    {
      break;
    }

    {
      cpdmConfigData_t *pConf = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pConf = cpdmFindConfig(cpId, L7_FALSE);
      if (L7_NULLPTR != pConf)
      {
        if (osapiStrncmp(pConf->radiusAuthServer,radiusServer,CP_RADIUS_AUTH_SERVER_MAX) != 0)
        {
          osapiStrncpySafe(pConf->radiusAuthServer,radiusServer,CP_RADIUS_AUTH_SERVER_MAX);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE);
    }
  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Set CP config redirectURL
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_char8 *redirectURL @b{(input)} - new
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
L7_RC_t cpdmCPConfigRedirectURLSet(cpId_t cpId, L7_char8 *redirectURL)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 len;

  do
  {
    if (L7_NULLPTR == redirectURL)
      break;
    len = strlen(redirectURL);
    if (len > CP_WELCOME_URL_MAX)
      break;
    {
      cpdmConfigData_t *pConf = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pConf = cpdmFindConfig(cpId, L7_FALSE);
      if (L7_NULLPTR != pConf)
      {
        if (osapiStrncmp(pConf->redirectURL,redirectURL,CP_WELCOME_URL_MAX) != 0)
        {
          osapiStrncpySafe(pConf->redirectURL,redirectURL,CP_WELCOME_URL_MAX+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE);
    }
  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Set CP config redirectMode
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uchar8 redirectMode @b{(input)} value for
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
L7_RC_t cpdmCPConfigRedirectModeSet(cpId_t cpId, L7_uchar8 redirectMode)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->redirectMode != redirectMode)
    {
      pConf->redirectMode = redirectMode;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
* @purpose  Set CP config userUpRate
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uint32 userUpRate @b{(input)} value for
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
L7_RC_t cpdmCPConfigUserUpRateSet(cpId_t cpId, L7_uint32 userUpRate)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->userUpRate != userUpRate)
    {
      pConf->userUpRate = userUpRate;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
* @purpose  Set CP config userDownRate
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uint32 userDownRate @b{(input)} value for
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
L7_RC_t cpdmCPConfigUserDownRateSet(cpId_t cpId, L7_uint32 userDownRate)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->userDownRate != userDownRate)
    {
      pConf->userDownRate = userDownRate;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
* @purpose  Set CP config maxInputOctets
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uint32 maxInputOctets @b{(input)} value for
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
L7_RC_t cpdmCPConfigMaxInputOctetsSet(cpId_t cpId, L7_uint32 maxInputOctets)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->maxInputOctets != maxInputOctets)
    {
      pConf->maxInputOctets = maxInputOctets;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusMaxInputOctetsByCpIdUpdate(cpId, maxInputOctets);
  }

  return rc;
}

/*********************************************************************
* @purpose  Set CP config maxOutputOctets
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uint32 maxOutputOctets @b{(input)} value for
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
L7_RC_t cpdmCPConfigMaxOutputOctetsSet(cpId_t cpId, L7_uint32 maxOutputOctets)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->maxOutputOctets != maxOutputOctets)
    {
      pConf->maxOutputOctets = maxOutputOctets;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusMaxOutputOctetsByCpIdUpdate(cpId, maxOutputOctets);
  }

  return rc;
}

/*********************************************************************
* @purpose  Set CP config maxTotalOctets
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uint32 maxTotalOctets @b{(input)} value for
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
L7_RC_t cpdmCPConfigMaxTotalOctetsSet(cpId_t cpId, L7_uint32 maxTotalOctets)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->maxTotalOctets != maxTotalOctets)
    {
      pConf->maxTotalOctets =  maxTotalOctets;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusMaxTotalOctetsByCpIdUpdate(cpId, maxTotalOctets);
  }

  return rc;
}

/*********************************************************************
* @purpose  Set CP config sessionTimeout
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uint32 sessionTimeout @b{(input)} value for
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
L7_RC_t cpdmCPConfigSessionTimeoutSet(cpId_t cpId, L7_uint32 sessionTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->sessionTimeout != sessionTimeout)
    {
      pConf->sessionTimeout = sessionTimeout;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusSessionTimeoutByCpIdUpdate(cpId, sessionTimeout);
  }
  return rc;
}

/*********************************************************************
* @purpose  Set CP config idleTimeout
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uint32 idleTimeout @b{(input)} value for
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
L7_RC_t cpdmCPConfigIdleTimeoutSet(cpId_t cpId, L7_uint32 idleTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->idleTimeout != idleTimeout)
    {
      pConf->idleTimeout = idleTimeout;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusIdleTimeoutByCpIdUpdate(cpId, idleTimeout);
  }

  return rc;
}

/*********************************************************************
* @purpose  Set CP config intrusionThreshold
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_uint32 intrusionThreshold @b{(input)} value for
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
L7_RC_t cpdmCPConfigIntrusionThresholdSet(cpId_t cpId, L7_uint32 intrusionThreshold)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->intrusionThreshold != intrusionThreshold)
    {
      pConf->intrusionThreshold = intrusionThreshold;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
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
L7_RC_t cpdmCPConfigForegroundColorSet(cpId_t cpId, L7_char8 *foregroundColor)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 len;

  do
  {
    if (L7_NULLPTR == foregroundColor)
      break;
    len = strlen(foregroundColor);
    if (len > CP_FOREGROUND_COLOR_MAX)
      break;
    {
      cpdmConfigData_t *pConf = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pConf = cpdmFindConfig(cpId, L7_FALSE);
      if (L7_NULLPTR != pConf)
      {
        if (osapiStrncmp(pConf->foregroundColor,foregroundColor,CP_FOREGROUND_COLOR_MAX) != 0)
        {
          osapiStrncpySafe(pConf->foregroundColor, foregroundColor, CP_FOREGROUND_COLOR_MAX);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE);
    }
  } while(0);

  return rc;
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
L7_RC_t cpdmCPConfigBackgroundColorSet(cpId_t cpId, L7_char8 *backgroundColor)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 len;

  do
  {
    if (L7_NULLPTR == backgroundColor)
      break;
    len = strlen(backgroundColor);
    if (len > CP_BACKGROUND_COLOR_MAX)
      break;
    {
      cpdmConfigData_t *pConf = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pConf = cpdmFindConfig(cpId, L7_FALSE);
      if (L7_NULLPTR != pConf)
      {
        if (osapiStrncmp(pConf->backgroundColor,backgroundColor,CP_BACKGROUND_COLOR_MAX) != 0)
        {
          osapiStrncpySafe(pConf->backgroundColor, backgroundColor, CP_BACKGROUND_COLOR_MAX);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE);
    }
  } while(0);

  return rc;
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
L7_RC_t cpdmCPConfigSeparatorColorSet(cpId_t cpId, L7_char8 *separatorColor)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 len;

  do
  {
    if (L7_NULLPTR == separatorColor)
      break;
    len = strlen(separatorColor);
    if (len > CP_SEPARATOR_COLOR_MAX)
      break;
    {
      cpdmConfigData_t *pConf = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pConf = cpdmFindConfig(cpId, L7_FALSE);
      if (L7_NULLPTR != pConf)
      {
        if (osapiStrncmp(pConf->separatorColor,separatorColor,CP_SEPARATOR_COLOR_MAX) != 0)
        {
          osapiStrncpySafe(pConf->separatorColor, separatorColor, CP_SEPARATOR_COLOR_MAX);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE);
    }
  } while(0);

  return rc;
}

/*********************************************************************
* @purpose  Force the disabling of all CP instances and interfaces,
*           and terminate all client connections.
*
* @param    cpId_t  cpId @b{(input)} CP config id
* @param    L7_CP_MODE_STATUS_t mode @b{(input)} value for
*           onfig item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only for use in the configurator.  This doesn't belong
*           in an ..api.c file.  Therefore, we will NOT add the
*           prototype of this function to the cpdm_api.h.
*
*           What's more, this function should only really be called
*           during the CAPTIVE_PORTAL_PHASE_UNCONFIG_1 phase of
*           configuration.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigModeDisableForce(void)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR != cpdmOprData)
  {
    SEMA_TAKE(WRITE);
    cpdmOprData->cpGlobalStatus.status = L7_CP_MODE_DISABLED;
    SEMA_GIVE(WRITE);
    rc = cpdmCPConfigOperStatusAllSet(L7_CP_INST_OPER_STATUS_DISABLED);
  }

  if (L7_SUCCESS == rc)
  {
    L7_uint32 intIfNum = 0;

    cpcmAIPStatusFinalize(); /* nothing should be in progress now! */

    while(L7_SUCCESS == cpdmIntfStatusNextGet(intIfNum, &intIfNum))
    {
      (void) cpimIntfDisable(intIfNum);
      (void) cpdmIntfStatusSetIsEnabled(intIfNum, L7_FALSE);
      (void) cpdmIntfStatusSetWhy(intIfNum, L7_INTF_WHY_DISABLED_ADMIN);
    }
    rc =  cpdmClientConnStatusDeleteAll();
  }


  return rc;
}

/*********************************************************************
* @purpose  Update the interface status to match ALL CP ID configs
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
static
L7_RC_t cpdmCPConfigModeAllUpdate(void)
{
  cpId_t cpId = (cpId_t) 0;
  L7_RC_t rc = L7_SUCCESS;

  if (L7_FALSE == captivePortalCnfgrComplete())
  {
    return rc;
  }

  while(L7_SUCCESS == cpdmCPConfigNextGet(cpId, &cpId))
  {
    if (L7_SUCCESS != cpdmCPConfigModeUpdate(cpId))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Can't update I/F modes for CP ID %d", cpId);
    }
  }
  return rc;
}


/*********************************************************************
* @purpose  Update association status to match CP config mode
*
* @param    L7_uint32  intIfNum @b{(input)} internal I/F number
* @param    L7_CP_MODE_STATUS_t mode @b{(input)} value for
*           onfig item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments only L7_CP_MODE_ENABLED, .._DISABLED are acceptable
*           as inputs.  Also, association between CP ID @i{cpId}
*           and interface @i{intIfNum} MUST exist before this
*           function is evoked!
* @end
*
*********************************************************************/
static
L7_RC_t cpdmCPConfigIntIfNumUpdate(L7_uint32 intIfNum, L7_CP_MODE_STATUS_t mode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t (*intf_func)(L7_uint32) = L7_NULLPTR;
  L7_BOOL enabling;

  switch(mode)
  {
    case L7_CP_MODE_ENABLED:
      intf_func = cpimIntfEnable;
      enabling = L7_TRUE;
      break;
    case L7_CP_MODE_DISABLED:
      intf_func = cpimIntfDisable;
      enabling = L7_FALSE;
      break;
    default:
      CP_DLOG(CPD_LEVEL_LOG, "%s: only ENABLED, DISABLED supported (got %d)", __FUNCTION__, mode );
      return rc;
  }

  do
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    if ((L7_FALSE == enabling) &&
         (L7_SUCCESS != cpdmClientConnStatusByIntfDeleteAll(intIfNum)))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Can't delete clients using interface %d, %s",intIfNum, ifName);
      break;
    }

    if (L7_SUCCESS == cpdmIntfStatusGet(intIfNum))
    {
      if (L7_SUCCESS != intf_func(intIfNum))
      {
        CP_DLOG(CPD_LEVEL_LOG, "Could not %sable intf %d, %s\n",
                (L7_CP_MODE_ENABLED == mode) ? "en" : "dis", intIfNum, ifName);
        break;
      }

      if (L7_SUCCESS != cpdmIntfStatusSetIsEnabled(intIfNum, enabling))
      {
        CP_DLOG(CPD_LEVEL_LOG, "Could not set status of intf %d, %s\n", intIfNum, ifName);
        break;
      }
    }
    rc = L7_SUCCESS;
  } while(0);
  return rc;
}


/*********************************************************************
* @purpose  Update the interface status to match CP config mode
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
static
L7_RC_t cpdmCPConfigModeUpdate(cpId_t cpId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum = (L7_uint32) 0;
  L7_CP_MODE_STATUS_t mode;
  L7_INTF_WHY_DISABLED_t why_disabled = L7_INTF_WHY_DISABLED_NONE;
  L7_uint32 next_intIfNum = (L7_uint32) 0;
  cpId_t next_cpId = 0;

  if (L7_SUCCESS != cpdmCPConfigModeGet(cpId, &mode))
  {
    CP_DLOG(CPD_LEVEL_LOG, "Couldn't get mode for CP ID %d", cpId);
    return rc;
  }

  if (L7_CP_MODE_ENABLED == mode)
  {
    if (L7_SUCCESS != cpdmGlobalModeGet(&mode))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Couldn't get global mode");
      return rc;
    }

    /* Use corresponding interface modes for global modes */
    mode = (L7_ENABLE==mode)?L7_CP_MODE_ENABLED:L7_CP_MODE_DISABLED;

    if (L7_CP_MODE_ENABLED==mode)
    {
      if (L7_SUCCESS != cpdmCPConfigIntIfNumNextGet(cpId, 0, &next_cpId, &next_intIfNum))
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: no associated interface for CP ID %d, can't enable\n",
                __FUNCTION__, cpId);
        mode = L7_CP_MODE_DISABLED;
        why_disabled = L7_INTF_WHY_DISABLED_UNATTACHED;
      }
    }
    else
    {
      why_disabled = L7_INTF_WHY_DISABLED_ADMIN;
    }
  }
  else
  {
    why_disabled = L7_INTF_WHY_DISABLED_ADMIN;
  }

  while ((L7_SUCCESS ==
          cpdmCPConfigIntIfNumNextGet(cpId, intIfNum, &next_cpId, &intIfNum)) &&
         (cpId == next_cpId))
  {
      (void) cpdmCPConfigIntIfNumUpdate(intIfNum, mode);
  }

  if (L7_CP_MODE_ENABLED == mode)
  {
    rc = cpdmCPConfigOperStatusSet(cpId, L7_CP_INST_OPER_STATUS_ENABLED);
  }
  else
  {

    switch (why_disabled)
    {
      case L7_INTF_WHY_DISABLED_UNATTACHED:
        (void) cpdmCPConfigDisableReasonSet(cpId,
                  L7_CP_INST_DISABLE_REASON_NOT_ASSOC_INTF);
        break;
      case L7_INTF_WHY_DISABLED_ADMIN:
        (void) cpdmCPConfigDisableReasonSet(cpId,
          L7_CP_INST_DISABLE_REASON_ADMIN);
        break;
      default:
        break;
    }

    rc = cpdmCPConfigOperStatusSet(cpId, L7_CP_INST_OPER_STATUS_DISABLED);
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (L7_CP_MODE_ENABLED == mode)
  {
    L7_LOGIN_TYPE_t protocol;

    /* if https is configured and we don't have a cert, then disable */
    if (cpdmCPConfigProtocolModeGet(cpId, &protocol) == L7_SUCCESS)
    {
      if (L7_LOGIN_TYPE_HTTPS==protocol)
      {
        if (L7_SUCCESS!=usmDbssltCertificateExists(1))
        {
          rc = cpdmCPConfigOperStatusSet(cpId, L7_CP_INST_OPER_STATUS_DISABLED);
          if (L7_SUCCESS==rc)
          {
            rc = cpdmCPConfigDisableReasonSet(cpId, L7_CP_INST_DISABLE_REASON_NO_VALID_CERT);
          }
        }
      }
    }
  }
#endif

  return rc;
}


/*********************************************************************
* @purpose  Update the interface status to match ALL CP ID configs
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

L7_RC_t cpdmCPStatusBlockedAllUpdate(void)
{
  cpId_t cpId = (cpId_t) 0;
  L7_RC_t rc = L7_SUCCESS;

  if (L7_FALSE == captivePortalCnfgrComplete())
  {
    return rc;
  }

  while(L7_SUCCESS == cpdmCPConfigNextGet(cpId, &cpId))
  {
    if (L7_SUCCESS != cpdmCPStatusBlockedUpdate(cpId))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Can't update blocking status for CP ID %d", cpId);
    }
  }
  return rc;
}

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
L7_RC_t cpdmCPStatusBlockedUpdate(cpId_t cpId)
{
  L7_CP_INST_BLOCK_STATUS_t currVal, newVal;
  L7_RC_t rc = L7_FAILURE;

  do
  {
    L7_RC_t (*block_func)(L7_uint32) = L7_NULLPTR;
    L7_uint32 intIfNum = 0;
    cpId_t next_cpId = 0;

    if (L7_SUCCESS != cpdmCPConfigBlockedStatusGet(cpId, &currVal))
      break;

    switch (currVal)
    {
       case L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING:
         block_func = cpimIntfBlock;
         newVal = L7_CP_INST_BLOCK_STATUS_BLOCKED;
         break;
       case L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING:
         block_func = cpimIntfUnblock;
         newVal = L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED;
         break;
       case L7_CP_INST_BLOCK_STATUS_BLOCKED:
       case L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED:
         return L7_SUCCESS; /* nothing to do, we're up-to-date */
       default:
         return L7_FAILURE; /* something very bad happened     */
    }

    while ((L7_SUCCESS ==
            cpdmCPConfigIntIfNumNextGet(cpId, intIfNum, &next_cpId, &intIfNum)) &&
           (next_cpId == cpId))
    {
      rc = block_func(intIfNum);
      if (L7_SUCCESS != rc) /* Drop out with bad return code */
        break;
    }

    if (L7_SUCCESS == rc)
    {
       rc = cpdmCPConfigBlockedStatusSet(cpId, newVal);
    }
  } while(0);

  return rc;
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

L7_RC_t cpdmCPConfigModeSet(cpId_t cpId, L7_CP_MODE_STATUS_t mode)
{
  L7_RC_t rc = L7_FAILURE;

  switch(mode)
  {
  case L7_CP_MODE_ENABLED:
    break;
  case L7_CP_MODE_DISABLED:
    break;
  default:
    CP_DLOG(CPD_LEVEL_LOG, "%s: only ENABLED, DISABLED supported (got %d)",
       __FUNCTION__, mode );
    return rc;
  }

  SEMA_TAKE(WRITE);
  do {
    cpdmConfigData_t *pConf = cpdmFindConfig(cpId, L7_FALSE);
    if (L7_NULLPTR == pConf)
    {
      break;
    }
    if (pConf->enabled != mode)
    {
      pConf->enabled = mode;
      cpdmCfgData->hdr.dataChanged = L7_TRUE;
    }
  } while(0);
  SEMA_GIVE(WRITE);

  return cpdmCPConfigModeUpdate(cpId);
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
L7_RC_t cpdmCPConfigIntfNotAssocNextGet(L7_uint32 intf, L7_uint32 *nextIntf)
{
  cpId_t cpId = 0;
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if (L7_NULLPTR == nextIntf)
      break;

    while (L7_SUCCESS == cpdmIntfStatusNextGet(intf, &intf))
    {
      if (L7_SUCCESS != cpdmCPConfigIntIfNumFind(intf, &cpId))
      {
        *nextIntf = intf;
        rc = L7_SUCCESS;
        break;
      }
    }
  } while(0);
  return rc;
}

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
*           successfully OR already present.
*
*           if we don't have a CP instance available,
*           create one with our minimum CP id value in addition
*           create a default web locale.
*
*           This function really doesn't belong in an API anywhere,
*           and frankly, it shouldn't be calling external APIs to
*           to do its work either.  But a rewrite right now would
*           simply be too costly.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigAddDefaults(void)
{
  cpId_t cpId = 0;
  L7_RC_t rc = L7_FAILURE;

  if (L7_SUCCESS != cpdmCPConfigNextGet(cpId, &cpId))
  {
    rc = cpdmCPConfigAdd(CP_ID_MIN);
    if (L7_SUCCESS != rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "Could not add default CP ID");
      return rc;
    }
    rc = cpdmCPConfigNameSet(CP_ID_MIN, CP_DEF_NAME);
    if (L7_SUCCESS != rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "Could not set name of default CP ID");
      return rc;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize our intf id/ cp id association tree
*
* @param    L7_uint32  maxAssocs    @b{(input)} max assocs to support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cpIntfAssocInit(void)
{
  if (L7_NULLPTR == cpdmCfgData)
    return L7_FAILURE;
  memset(&(cpdmCfgData->cpInterfaceAssocData[0]), 0, CP_INTERFACE_MAX);
  assocCount = 0;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Finalize our intf id/ cp id association tree
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpIntfAssocFinalize(void)
{
  if (L7_NULLPTR != cpdmCfgData)
  {
     memset(&(cpdmCfgData->cpInterfaceAssocData[0]), 0, CP_INTERFACE_MAX);
     assocCount = 0;
  }
}



/*********************************************************************
 *
 *  INTERFACE ASSOCIATION LOOKUP FUNCTIONS BEGIN HERE
 *
 *********************************************************************/


/*********************************************************************
*
* @purpose  Find a tree entry with the given intf id as a key
*
* @param    cpId_t     cpId     @b{(input)} CP portion of key
* @param    L7_uint32  intIfNum @b{(input)} I/F portion of key
*
* @returns  negative integer on error, index of entry otherwise
*
* @comments strictly for use within its own module, needs
*           sema protection from caller
* @end
*
*********************************************************************/

static
L7_int32 cpdmFindIntfAssocIndex(cpId_t cpId,
                                L7_uint32 ifNum,
                                find_assoc_t how)
{
  L7_uint32 idx = -1;
  ifNumCPPair_t * pBase;
  L7_BOOL exact_match = L7_FALSE;

  if (L7_NULLPTR == cpdmCfgData)
    return idx;

  pBase = &(cpdmCfgData->cpInterfaceAssocData[0]);
  for(idx = 0; idx < assocCount; idx++)
  {
    if (pBase[idx].cpId < cpId)
    {
      continue;                      /* Keep looking */
    }

    if ((pBase[idx].cpId == cpId) &&
        (pBase[idx].intIfNum < ifNum))
    {
      continue;                      /* Keep looking */
    }

    exact_match = ((pBase[idx].cpId == cpId) &&
                   (pBase[idx].intIfNum == ifNum));

    if (FIND_ASSOC_EXACT == how)
    {
      if (exact_match)
        return idx;
      else
        return -2;
    }
    else if (exact_match)
      continue;

    break; /* Okay, we found something interesting */
  }

  if (idx == assocCount) /* If we've overshot our entries */
  {
    if (FIND_ASSOC_ADD == how)
    {
      if (assocCount == CP_INTERFACE_MAX)
        return -3;
    }
    else
      return -2;
  }
  return idx;
}

/*********************************************************************
*
* @purpose  Find a tree entry with the given intf id as a key
*
* @param    L7_uint32  intfId   @b{(input)} the key, of course
*
* @returns  void  pointer to ifNumCPPair_t (the value)
*
* @comments strictly for use within its own module, needs
*           sema protection from caller
* @end
*
*********************************************************************/
static ifNumCPPair_t * cpdmFindIntfAssoc(cpId_t cpId, L7_uint32 ifNum)
{
  L7_int32 idx = cpdmFindIntfAssocIndex(cpId, ifNum, FIND_ASSOC_EXACT);
  if (0 > idx)
    return L7_NULLPTR;
  return &(cpdmCfgData->cpInterfaceAssocData[idx]);
}


/*********************************************************************
*
* @purpose  Check for existing interface association
*
* @param    cpId_t    cpId    @b{(input)} CP ID
* @param    L7_uint32 ifNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigIntIfNumGet(cpId_t cpId, L7_uint32 ifNum)
{
  ifNumCPPair_t *pEntry = L7_NULLPTR;

  SEMA_TAKE(READ);
  pEntry = cpdmFindIntfAssoc(cpId, ifNum);
  SEMA_GIVE(READ);

  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}



/*********************************************************************
*
* @purpose  Get "next" interface association
*
* @param    cpId_t    cpId   @b{(input)} CP ID
* @param    L7_uint32 ifNum  @b{(input)} interface id
* @param    cpId_t    pCpId  @b{(output)} next CP ID
* @param    L7_uint32 pIfNum @b{(output)} next interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigIntIfNumNextGet(cpId_t cpId,   L7_uint32 ifNum,
                                    cpId_t *pCpId, L7_uint32 *pIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  if (!pCpId || !pIfNum)
    return L7_FAILURE;

  SEMA_TAKE(READ);
  do
  {
    ifNumCPPair_t *pEntry = L7_NULLPTR;
    L7_int32 idx = cpdmFindIntfAssocIndex(cpId, ifNum, FIND_ASSOC_NEXT);
    if (idx < 0)
      break;
    pEntry  = &(cpdmCfgData->cpInterfaceAssocData[idx]);
    *pCpId  = pEntry->cpId;
    *pIfNum = pEntry->intIfNum;
    rc = L7_SUCCESS;
  } while (0);
  SEMA_GIVE(READ);

  return rc;
}


/*********************************************************************
*
* @purpose  Add an interface association
*
* @param    cpId_t    cpId   @b{(input)} CP ID
* @param    L7_uint32 ifNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Fails if we have either the maximum associations
*           already, or if there is a duplicate entry.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigIntIfNumAdd(cpId_t cpId, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32 idx = 0;

  if (L7_NULLPTR == cpdmCfgData)
    return rc;

  if (L7_FALSE == captivePortalCnfgrComplete())
  {
    return rc; /* Better safe than sorry */
  }

  SEMA_TAKE(WRITE);
  do
  {
    ifNumCPPair_t *pBase =
      &(cpdmCfgData->cpInterfaceAssocData[0]);

    if (CP_INTERFACE_MAX == assocCount)
      break; /* if we can */

    idx = cpdmFindIntfAssocIndex(cpId, intIfNum, FIND_ASSOC_ADD);

    if ((idx > 0) || ((0==idx) && (0<assocCount)))
    {
      /* push up any later entries, else we're appending an entry */
      memmove(&(pBase[idx + 1]), &(pBase[idx]),
        (assocCount - idx) * sizeof(ifNumCPPair_t));
    }

    pBase[idx].intIfNum = intIfNum;
    pBase[idx].cpId     = cpId;
    assocCount++;
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(WRITE);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmCPConfigModeUpdate(cpId);
    if (L7_SUCCESS != rc)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      CP_DLOG(CPD_LEVEL_LOG, "Could not update CP data, intf %d, %s", intIfNum, ifName);
    }
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Delete an interface association
*
* @param    cpId_t    cpId   @b{(input)} CP ID
* @param    L7_uint32 ifNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Fails if association doesn't exist.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmCPConfigIntIfNumDelete(cpId_t cpId, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32 idx = 0;

  if (L7_NULLPTR == cpdmCfgData)
    return rc;

  if (L7_FALSE == captivePortalCnfgrComplete())
  {
    return rc; /* Better safe than sorry */
  }

  /* Disable the interface before we delete it from our association table */
  cpdmCPConfigIntIfNumUpdate(intIfNum, L7_CP_MODE_DISABLED);

  /* Remove any AIP entries attached to this interface */
  cpcmAIPStatusCleanup(intIfNum);

  SEMA_TAKE(WRITE);
  do
  {
    ifNumCPPair_t *pBase =
      &(cpdmCfgData->cpInterfaceAssocData[0]);

    idx = cpdmFindIntfAssocIndex(cpId, intIfNum, FIND_ASSOC_EXACT);
    if (idx < 0)
      break;

    assocCount--;
    memmove(&(pBase[idx]), &(pBase[idx + 1]),
      (assocCount - idx) * sizeof(ifNumCPPair_t));
    memset(&(pBase[assocCount]), 0, sizeof(ifNumCPPair_t));
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  /* Now propagate change throughout the system */
  if (L7_SUCCESS != cpdmCPConfigModeUpdate(cpId))
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    CP_DLOG(CPD_LEVEL_LOG, "Could not update CP data, intf %d, %s", intIfNum, ifName);
  }

  return rc;
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
L7_RC_t cpdmCPConfigIntIfNumFind(L7_uint32 intIfNum, cpId_t * cpId)
{
  ifNumCPPair_t * pBase;
  L7_int32 idx = 0;

  if ((L7_NULLPTR == cpdmCfgData) || (L7_NULLPTR == cpId))
    return  L7_FAILURE;

  pBase = &(cpdmCfgData->cpInterfaceAssocData[0]);
  for(idx = 0; idx < assocCount; idx++)
  {
    if (intIfNum == pBase[idx].intIfNum)
    {
      *cpId = pBase[idx].cpId;
      return L7_SUCCESS;
    }
  }
  return  L7_FAILURE;
}

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
void cpdmCPIntfCountUpdate(void)
{
  L7_uint32 count = 0;

  SEMA_TAKE(WRITE);
  if (L7_NULLPTR != cpdmCfgData)
  {
    for(count = 0; count < CP_INTERFACE_MAX; count++)
    {
      if (0 == cpdmCfgData->cpInterfaceAssocData[count].intIfNum)
      {
        break;
      }
    }
  }
  assocCount = count;
  SEMA_GIVE(WRITE);
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
void cpdmSetImageFileChanged(void)
{
  if (L7_NULLPTR != cpdmRAMCfgDataToStore)
  {
    SEMA_TAKE(WRITE);
    cpdmRAMCfgDataToStore->hdr.dataChanged = L7_TRUE;
    SEMA_GIVE(WRITE);
  }
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
L7_uchar8 * cpdmImageDataGet(L7_uint32 *eoi)
{
  *eoi = cpdmRAMCfgDataToStore->eoi;
  if (0 < *eoi)
  {
    return &(cpdmRAMCfgDataToStore->images[0]);
  }

  return L7_NULLPTR;
}

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
L7_RC_t cpdmCopyImageDataToFs(L7_uchar8 *images)
{
  L7_uint32 total;
  L7_uint32 cnt;
  L7_uint32 idx ;
  L7_char8 *filePath = RAM_CP_PATH RAM_CP_NAME;
  L7_char8 fileName[CP_FILE_NAME_MAX+1];
  L7_uint32 fileNameLen;
  L7_uint32 FQF_MAX = 256;
  L7_char8 fqf[FQF_MAX];
  L7_uint32 fileSize;
  L7_RC_t rc = L7_FAILURE;

  if (images == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  idx = 0;
  fileSize = 4;

  SEMA_TAKE(READ);
  total = l7utilsBytesToInt(&images[0]);

  for (cnt=0;cnt<total;cnt++)
  {
    fileNameLen = l7utilsBytesToInt(&images[idx+=fileSize]);
    osapiStrncpy(fileName,&images[idx+=4],fileNameLen);
    fileName[fileNameLen] = '\0';
    memset(fqf,0,FQF_MAX);
    strncpy(fqf,filePath,FQF_MAX);
    strncat(fqf,"/",FQF_MAX);
    strncat(fqf,fileName,FQF_MAX);
    fileSize = l7utilsBytesToInt(&images[idx+=fileNameLen]);
    rc = osapiFsWrite(fqf,&images[idx+=4],fileSize);
    if (L7_SUCCESS != rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to write file:%s.",__FUNCTION__,fileName);
      SEMA_GIVE(READ);
      return L7_FAILURE;
    }
  }
  cpdmRAMCfgDataToStore->hdr.dataChanged = L7_FALSE;
  SEMA_GIVE(READ);
  return rc;
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
L7_RC_t cpdmDecodedImageSizeSet(L7_uint32 eoi)
{
  SEMA_TAKE( WRITE)
  if (L7_NULLPTR != cpdmRAMCfgDataToStore)
  {
    if (cpdmRAMCfgDataToStore->eoi != eoi)
    {
      cpdmRAMCfgDataToStore->eoi = eoi;
      cpdmRAMCfgDataToStore->hdr.dataChanged = L7_TRUE;
    }
  }
  SEMA_GIVE( WRITE)
  return L7_SUCCESS;
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
L7_RC_t cpdmDecodeTextAndImageCreate(L7_uchar8 *text)
{
  L7_uint32 len;                /* Length of incoming encoded text stream */
  L7_uint32 idx;                /* decoded images index */
  L7_uint32 jdx;                /* encoded text index */
  L7_int8 c = 0;                /* encoded text input character */
  L7_uint32 partial_value = 0;  /* the working quad */
  L7_int8 v;                    /* the working bit */
  L7_int8 chars_in = 0;         /* number of characters in the group of 4
                                 * that have already been processed.
                                 * value = 0, 1, 2, or 3 */

  if (L7_NULLPTR != cpdmRAMCfgDataToStore)
  {
    if ((L7_NULLPTR==text) || (0==strlen(text)))
    {
      /* Initializer */
      memset((void *) cpdmRAMCfgDataToStore, 0, sizeof(cpdmRAMCfgDataToStore_t));
      SEMA_TAKE( WRITE)
      cpdmRAMCfgDataToStore->eoi = 0;
      cpdmRAMCfgDataToStore->eot = 0;
      SEMA_GIVE( WRITE)
    }
    else /* Decode and append */
    {
      len = strlen(text);

      for (idx = cpdmRAMCfgDataToStore->eot, jdx=0;
          ((idx<=cpdmRAMCfgDataToStore->eoi) && (jdx<=len));
          jdx++)
      {
        c = text[jdx];

        if ('A' <= c && 'Z' >= c)
        {
          v = c - 'A';
        }
        else if ('a' <= c && 'z' >= c)
        {
          v = c - 'a' + 26;
        }
        else if ('0' <= c && '9' >= c)
        {
          v = c - '0' + 52;
        }
        else if ('+' == c)
        {
          v = 62;
        }
        else if ('/' == c)
        {
          v = 63;
        }
        else if ('=' == c)
        {
          break; /* '=' signals the end of the string. */
        }
        else
        {
          break; /* End of chunk (could also be end of data) */
        }
        /*
        * Incorporate the new bits into the current partial value
        * and write any complete bytes out.
        */
        partial_value = (partial_value << 6) | v;
        switch (chars_in)
        {
          case 0:
          {
            chars_in = 1;
            break;
          }
          case 1:
          {
            /* 12 bits are in partial_value */
            cpdmRAMCfgDataToStore->images[idx++] = (partial_value >> 4) & 0xFF;
            partial_value &= 0x0F;
            chars_in = 2;
            break;
          }
          case 2:
          {
            /* 10 bits are in partial_value */
            cpdmRAMCfgDataToStore->images[idx++] = (partial_value >> 2) & 0xFF;
            partial_value &= 0x03;
            chars_in = 3;
            break;
          }
          case 3:
          {
            /* 8 bits are in partial_value */
            cpdmRAMCfgDataToStore->images[idx++] = partial_value & 0xFF;
            partial_value = 0;
            chars_in = 0;
            break;
          }
        }
      }

      cpdmRAMCfgDataToStore->eot = idx; /* Setup for the next chunk */

      if (cpdmRAMCfgDataToStore->eot == cpdmRAMCfgDataToStore->eoi)
      {
        /* Create the files if we are done */
        cpdmCopyImageDataToFs(cpdmRAMCfgDataToStore->images);
      }
    }
  }

  return L7_SUCCESS;
}
