/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
 *
 * @filename  captive_portal_config.c
 *
 * @purpose   Contains definitions for configuration and operational
 *            code
 *
 * @component captive portal
 *
 * @comments
 *
 * @create    7/9/2007
 *
 * @author    dcaugherty
 *
 * @end
 *
 **********************************************************************/

#include <string.h>

#include "l7_common.h"
#include "sysapi.h"
#include "simapi.h"
#include "osapi.h"
#include "rlim_api.h"
#include "l7_ip_api.h"
#include "nvstoreapi.h"
#include "intf_cb_api.h"
#include "captive_portal_common.h"
#include "captive_portal_config.h"
#include "cp_cluster.h"
#include "cpdm_api.h"
#include "log.h"

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);  
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);  

extern cpdmCfgData_t            *cpdmCfgData;   
extern cpdmOprData_t            *cpdmOprData;
extern cpdmRAMCfgDataToStore_t  *cpdmRAMCfgDataToStore;
extern void *cpdmSema; 

extern L7_RC_t cpdmCPConfigInit(L7_uint32 maxConfigs);
extern L7_RC_t cpIntfAssocInit(L7_uint32 maxAssocs);
extern void    cpIntfAssocPurge(void);
extern L7_RC_t cpIntfCapabStatusInit(L7_uint32 maxAssocs);
extern L7_RC_t cpcmAIPStatusInit(L7_uint32 maxAIPStatus);
extern L7_RC_t cpdmCACStatusInit(L7_uint32 maxCACStatus);
extern L7_RC_t cpdmClientConnStatusInit(L7_uint32 maxCACStatus);
extern L7_RC_t cpdmIntfStatusInit(L7_uint32 maxIntfStatus);
extern L7_RC_t cpdmActStatusInit(L7_uint32 maxActivityStatus);
extern L7_RC_t cpdmUserGroupEntryInit(void);
extern L7_RC_t cpdmUserGroupAssocEntryInit(void);

/* routines for retrieving and saving configuration data */



/*********************************************************************
*
* @purpose  Allocate and initialize configuration info block for
*           Captive Portal task
*
* @param    void
*           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    Calls CpDeleteCfgData if anything should fail.
*
* @end
*********************************************************************/
L7_RC_t cpNewCfgData(void) 
{
  L7_RC_t rc = L7_FAILURE;

  do 
  {
    cpdmCfgData = osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                      sizeof(cpdmCfgData_t));
    if (L7_NULLPTR == cpdmCfgData) 
    {
      LOG_ERROR(0); /* no point in continuing */
    }
    memset((void *) cpdmCfgData, 0, sizeof(cpdmCfgData_t));

    if (L7_FAILURE == cpdmCPConfigInit(CP_ID_MAX))
    {
      LOG_ERROR(0);
    }

    if (L7_FAILURE == cpdmUserGroupEntryInit())
    {
      LOG_ERROR(0);
    }

    if (L7_FAILURE == cpdmUserGroupAssocEntryInit())
    {
      LOG_ERROR(0);
    }

    if (L7_FAILURE == cpIntfAssocInit(CP_INTERFACE_MAX))
    {
      LOG_ERROR(0); /* suicide is the answer */
    }

    cpdmCfgData->hdr.dataChanged                        = L7_FALSE;
    cpdmCfgData->cpGlobalData.cpMode                    = FD_CP_DEFAULT_CP_MODE;

    cpdmCfgData->cpGlobalData.peerSwitchStatsReportInterval 
      = FD_CP_PS_STATS_REPORT_INTERVAL;
    cpdmCfgData->cpGlobalData.sessionTimeout            = FD_CP_AUTH_SESSION_TIMEOUT;

    cpdmOprData->cpGlobalStatus.portals.supported         = FD_CP_CONFIG_MAX;
    cpdmOprData->cpGlobalStatus.portals.configured        = 0;

    cpdmOprData->cpGlobalStatus.users.supported           = CP_CLIENT_CONN_STATUS_MAX;
    cpdmOprData->cpGlobalStatus.users.local               = FD_CP_LOCAL_USERS_MAX;
    cpdmOprData->cpGlobalStatus.users.authenticated       = 0;

    cpdmOprData->cpGlobalStatus.authFailureUsers.capacity = FD_CP_USERS_FAILING_AUTH;
    cpdmOprData->cpGlobalStatus.authFailureUsers.size     = 0;

    cpdmOprData->cpGlobalStatus.activityLog.capacity      = FD_CP_ACTIVITY_LOG_RECORDS_MAX;
    cpdmOprData->cpGlobalStatus.activityLog.size          = 0;

    rc = L7_SUCCESS;
  } while(0);


  if (L7_SUCCESS != rc) 
  {
    cpDeleteCfgData();
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Finalize and deallocate configuration info block for
*           Captive Portal task
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/

void cpDeleteCfgData(void)
{
  cpdmCfgData_t           * pCD  = cpdmCfgData;
  cpdmRAMCfgDataToStore_t * pRCD = cpdmRAMCfgDataToStore;


  cpdmCfgData = (cpdmCfgData_t *) L7_NULLPTR;
  if (L7_NULLPTR != pCD) 
  {
    osapiFree(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, pCD);
  }
  cpdmRAMCfgDataToStore = (cpdmRAMCfgDataToStore_t *) L7_NULLPTR;
  if (L7_NULLPTR != pRCD) 
  {
    osapiFree(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, pRCD);
  }

}


/*********************************************************************
*
* @purpose  Allocate and initialize operational info block for
*           Captive Portal task
*
* @param    void
*           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    Calls CpDeleteOprData if anything should fail.
*
* @end
*********************************************************************/
L7_RC_t cpNewOprData(void) 
{
  L7_RC_t rc = L7_FAILURE;
  void * pSem = (void *) L7_NULLPTR;

  do 
  {
    if (L7_NULLPTR != cpdmOprData)
      /* don't just "break" here, or we'll wipe out existing data */
      return L7_FAILURE; 

    if (L7_NULLPTR == cpdmSema)
    {
      pSem = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
      if (L7_NULLPTR == pSem) 
      {
        LOG_ERROR(0); /* no point in continuing */
      }
      else
      {
        cpdmSema = pSem;
      }
    }

    cpdmOprData = osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                                 sizeof(cpdmOprData_t));
    if (L7_NULLPTR == cpdmOprData) 
    {
      LOG_ERROR(0); /* no point in continuing */
    }
    memset((void *) cpdmOprData, 0, sizeof(cpdmOprData_t));

    if (L7_FAILURE == cpIntfCapabStatusInit(CP_INTERFACE_MAX))
    {
      LOG_ERROR(0); 
    }

    if (L7_FAILURE == cpcmAIPStatusInit(CP_AUTH_IN_PROGRESS_MAX))
    {
      LOG_ERROR(0); 
    }

    if (L7_FAILURE == cpdmCACStatusInit(CP_CLIENT_CONN_STATUS_MAX))
    {
      LOG_ERROR(0); 
    }

    if (L7_FAILURE == cpdmClientConnStatusInit(CP_CLIENT_CONN_STATUS_MAX))
    {
      LOG_ERROR(0); 
    }

    if (L7_FAILURE == cpdmIntfStatusInit(CP_INTERFACE_MAX))
    {
      LOG_ERROR(0); 
    }

    if (L7_FAILURE == cpdmActStatusInit(CP_INTERFACE_MAX))
    {
      LOG_ERROR(0); 
    }

    {
      register cpdmGlobalStatus_t *pGS = &(cpdmOprData->cpGlobalStatus);
      pGS->status   = L7_CP_MODE_DISABLED;
      pGS->reason   = L7_CP_MODE_REASON_ADMIN;
      pGS->address  = L7_NULL_IP_ADDR;
#ifdef L7_ROUTING_PACKAGE
      pGS->cpIntIfNum        = L7_NULL;
      pGS->cpRoutingMode     = L7_DISABLE;
      pGS->cpLoopBackIFInUse = L7_FALSE;
#endif


      register cpdmLocalSwStatus_t *localSw = &(cpdmOprData->cpLocalSw);
      memset(&localSw->macAddr, 0x00, sizeof(L7_enetMacAddr_t));
      localSw->status = CP_CLUSTER_SWITCH_NONE;
      localSw->cpId = 0;
      localSw->intf = 0;
      memset(&localSw->authClient, 0x00, sizeof(L7_enetMacAddr_t));
      memset(&localSw->peer, 0x00, sizeof(L7_enetMacAddr_t));
      localSw->authReqTime = 0;
      localSw->authReplyTime = 0;
      localSw->userInitiated = L7_FALSE;

      register L7_enetMacAddr_t *newPeer = &(cpdmOprData->cpNewPeer);
      memset(&newPeer->addr, 0x00, sizeof(L7_enetMacAddr_t));

      register L7_enetMacAddr_t *ctrlMacAddr = &(cpdmOprData->cpCtrlMacAddr);
      memset(&ctrlMacAddr->addr, 0x00, sizeof(L7_enetMacAddr_t));

      register cpdmPeerSwClientAuthInfo_t *clientAuthInfo = &(cpdmOprData->cpClientAuthInfo);
      memset(&clientAuthInfo->swMacAddr, 0x00, sizeof(L7_enetMacAddr_t));
      memset(&clientAuthInfo->macAddr, 0x00, sizeof(L7_enetMacAddr_t));
      clientAuthInfo->cpId = 0;
      clientAuthInfo->intf = 0;
      clientAuthInfo->sessionTime = 0;
      memset(&clientAuthInfo->uName, 0, CP_USER_LOCAL_USERNAME_MAX);
      memset(&clientAuthInfo->pwd, 0, CP_USER_LOCAL_PASSWORD_MAX);
      clientAuthInfo->bytesTx = 0;
      clientAuthInfo->bytesRx = 0;
      clientAuthInfo->pktsTx = 0;
      clientAuthInfo->pktsRx = 0;
      clientAuthInfo->userUpRate = 0;
      clientAuthInfo->userDownRate = 0;
      clientAuthInfo->authPending = L7_FALSE;

      register L7_enetMacAddr_t *cfgPeerTx = &(cpdmOprData->cpCfgPeerTx);
      memset(&cfgPeerTx->addr, 0x00, sizeof(L7_enetMacAddr_t));

      register cpdmClusterConfigCmd_t *configCmd = &(cpdmOprData->cpConfigCmd);
      configCmd->cmd = 0;

    }

    rc = L7_SUCCESS;
  } while(0);

  return rc;
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
* @comments NOT TO BE EXPORTED OUTSIDE CAPTIVE PORTAL
*
* @end
*
*********************************************************************/
L7_RC_t cpdmOprGlobalModeSet(L7_uint32 mode)
{
  switch(mode)
  {
  case L7_DISABLE:
  case L7_ENABLE:
    break;
  default:
    return L7_FAILURE;
  }
  
  SEMA_TAKE(WRITE);
  cpdmOprData->cpGlobalStatus.status = mode;
  SEMA_GIVE(WRITE);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the Interface MAC address 
*
* intfMAC   @b{(output)} pointer to mac address with length L7_MAC_ADDR_LEN
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalCPMACAddressGet(L7_uchar8 *intfMAC)
{
  SEMA_TAKE(WRITE)
  memcpy(intfMAC, &cpdmOprData->cpGlobalStatus.intIfMac, L7_ENET_MAC_ADDR_LEN);
  SEMA_GIVE(WRITE)

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Set the Interface MAC address 
*
* intfMAC   @b{(input)} pointer to mac address with length L7_MAC_ADDR_LEN
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalCPMACAddressSet(L7_uchar8 *intfMAC)
{
  SEMA_TAKE(WRITE)
  memcpy(&cpdmOprData->cpGlobalStatus.intIfMac, intfMAC, L7_ENET_MAC_ADDR_LEN);
  SEMA_GIVE(WRITE)

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Check to see if the Interface MAC address has been changed
*
* intfMAC   @b{(input)} pointer to mac address with length L7_MAC_ADDR_LEN
*
* @returns  L7_SUCCESS if the MAC address is same.
* @returns  L7_FAILURE otherwise.
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalCPMACAddressCheck(L7_uchar8 *intfMAC)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(READ)
  if (memcmp(&cpdmOprData->cpGlobalStatus.intIfMac, intfMAC, L7_ENET_MAC_ADDR_LEN) == 0)
      rc = L7_SUCCESS;
  SEMA_GIVE(READ)

  return rc;
}


/*********************************************************************
*
* @purpose  Checks for IP address availability for CP task
*           
*
* @param    pIpAddr        @b{(input/output)} current/new IP address
* @param    pIpMask        @b{(input/output)} current/new IP subnet mask
* @param    pRtrMode       @b{(input/output)} current/new routing mode
* @param    pIntIfNum      @b{(input/output)} current/new internal interface number
* @param    pLoopbackInUse @b{(input/output)} indicates if application current/new using loopback
* @param    pCode          @b{(output)} reason code to indicate why no IP address selected
*
* @returns  L7_RC_t - L7_SUCCESS if IP address retrieved,
*           L7_FAILURE otherwise.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t cpIPAddressCheck(L7_IP_ADDR_t        * pIpAddr,
                         L7_uint32           * pIpMask,
                         L7_uint32           * pRtrMode,
                         L7_uint32           * pIntIfNum,
                         L7_BOOL             * pLoopbackInUse,
                         L7_CP_MODE_REASON_t * pCode )
{
  L7_RC_t            rc = L7_FAILURE;
  SYSAPI_APP_IP_REASON_t reason = SYSAPI_APP_IP_REASON_NONE;

  do 
  {
    if ((L7_NULLPTR == pCode) ||(L7_NULLPTR == pIpAddr))
      break;

    *pCode = L7_CP_MODE_REASON_ADMIN;   

    /* 
     LOG_MSG("..select call parms: addr, rMode, ifNum, inUse, reason = 0x%08x, %d, %d, %d, %d",
    *pIPAddr, *pRtrMode, *pIntIfNum, *pLoopbackInUse, reason); 
    */

    rc = sysapiApplicationIPAddressSelect(pIpAddr, 
                                          pIpMask,  
                                          pRtrMode,
                                          pIntIfNum, 
                                          pLoopbackInUse,
                                          &reason);

    if (L7_FAILURE == rc)
    {
      switch (reason)
      {
        case SYSAPI_APP_IP_REASON_NO_IP_ADDRESS:
          *pCode = L7_CP_MODE_REASON_NO_IP_ADDR;
          break;
#ifdef L7_ROUTING_PACKAGE
        case SYSAPI_APP_IP_REASON_NO_LOOPBACK_INTERFACE:
          *pCode = L7_CP_MODE_REASON_ROUTING_ENABLED_NO_IP_RT_INTF;
          break;
        case SYSAPI_APP_IP_REASON_GLOBAL_ROUTING_DISABLED:
          *pCode = L7_CP_MODE_REASON_ROUTING_DISABLED;
          /* TODO: UI will need to know what to do with this new constant! */
          break;
#endif
        default:
          *pCode = L7_CP_MODE_REASON_NONE;
          break;
      }
      /*  LOG_MSG("..addr select failed, reason = %d", reason); */
    }
  } while(0);
  return rc;
}



/*********************************************************************
*
* @purpose  Gets operational mode of Captive Portal task
*
* @param    void
*
* @returns  L7_CP_MODE_STATUS_t - either L7_CP_MODE_ENABLED 
*           or L7_CP_MODE_DISABLED.
*
* @notes    Call should be protected by semaphore. 
*
* @end
*********************************************************************/

L7_CP_MODE_STATUS_t  cpGetOprMode()
{
  L7_CP_MODE_STATUS_t status = L7_CP_MODE_DISABLED;

  /* This function should either return a boolean, or the "enabled"
   * field should be retrofitted to be of type L7_CP_MODE_STATUS_t.
   * For now, we map the value for backward compatibility's sake.
   */
  SEMA_TAKE(READ);
  if (L7_NULLPTR != cpdmOprData)
  {
    status = cpdmOprData->cpGlobalStatus.status;
  }
  SEMA_GIVE(READ);
  return status;
}

/*********************************************************************
*
* @purpose  Sets operational mode of Captive Portal task 
*
* @param    L7_uint32 - either L7_ENABLE or L7_DISABLE
*
* @returns  L7_RC_t - either L7_SUCCESS or L7_FAILURE
*
* @notes    Call should be protected by semaphore. 
*
* @end
*********************************************************************/

L7_RC_t  cpSetOprMode(L7_uint32 enableOrDisable)
{
  L7_RC_t  rc       = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do 
  {
    if ((L7_ENABLE  != enableOrDisable) &&
        (L7_DISABLE != enableOrDisable))
    {
      LOG_MSG("%s: garbage in: %u\n", __FUNCTION__, enableOrDisable);
      break;
    }
    if (L7_NULLPTR == cpdmCfgData)
    {
      LOG_MSG("%s: no cfg data?\n", __FUNCTION__);
      break;
    }
    cpdmCfgData->cpGlobalData.cpMode = enableOrDisable;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  if (L7_SUCCESS == rc) 
  {
    captivePortalTaskWake();
  }
  return rc;
}

