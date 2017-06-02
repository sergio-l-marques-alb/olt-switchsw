/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpcm_api.c
*
* @purpose      Captive Portal Client Credential Manager (CPCM)
*
* @component    CPCM
*
* @comments     none
*
* @create       08/09/2007
*
* @author       darsenault
*
* @end
*
**********************************************************************/
#include <string.h>
#include "l7utils_api.h"
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "captive_portal_commdefs.h"
#include "intf_cb_api.h"
#include "captive_portal_defaultconfig.h"
#include "captive_portal_common.h"
#include "cpdm_connstatus_api.h"
#include "datatypes.h"
#include "cpdm_user_api.h"
#include "cpdm_api.h"
#include "cpcm_api.h"
#include "cpim_api.h"
#include "cpcm.h"
#include "cpdm.h"
#include "radius_api.h"
#include "cpdm_clustering_api.h"
#include "clustering_api.h"
#include "cpdm_connstatus_util.h"

avlTree_t                aipStatusTree;
static L7_BOOL           aipStatusTreeReady = L7_FALSE;
static avlTreeTables_t * aipStatusTreeHeap  = L7_NULLPTR;
static void            * aipStatusDataHeap  = L7_NULLPTR;
static L7_BOOL           sendAuthReqMsg     = L7_TRUE;

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  L7_LOG_ERROR(0);  
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  L7_LOG_ERROR(0);  


/* Our "prepare to authenticate" call must be handled asynchronously, so we'll make 
 * it a point to do the actual work in the context of the Captive Portal task.
 * Our NIM "wrapper" will place an item on a work queue and wake the task.
 */

typedef struct authWorkOrder_s
{
  L7_IP_ADDR_t     ipAddr;
  L7_enetMacAddr_t mac;
  L7_uint32        intIfNum;
} authWorkOrder_t;

typedef struct authWorkOrderQ_s
{
  authWorkOrder_t * pAll;   /* pointer to buffer with max # work orders */
  L7_uint32  front, back, size; 
} authWorkOrderQ_t;

static authWorkOrderQ_t  authWOQ =  { L7_NULLPTR, 0, 0, 0};

static L7_RC_t  authWOQInit(L7_uint32 maxOrders);
static void     authWOQFinalize(void);
static L7_RC_t  authWOGet(L7_IP_ADDR_t * ipAddr, 
                          L7_enetMacAddr_t * mac,
                          L7_uint32 * intIfNum);
static L7_RC_t  authWOAdd(L7_IP_ADDR_t ipAddr, 
                          L7_enetMacAddr_t * mac,
                          L7_uint32 intIfNum);

static L7_RC_t cpcmProcessAuthPreparation(L7_IP_ADDR_t       ipAddr, 
                                          L7_enetMacAddr_t * mac,
                                          L7_uint32          intfId);

static L7_RC_t cpcmAIPStatusUserSpecificInfoAdd(L7_IP_ADDR_t       ipAddr, 
                                                L7_uint8 *         uid,
                                                L7_uint8 *         pwd,
                                                CP_radiusEntry_t * radiusEntry);

static L7_RC_t cpcmRADIUSGroupCheck(L7_IP_ADDR_t       ipAddr, 
                                    CP_radiusEntry_t * radiusEntry);


static L7_RC_t cpcmUserAuthFailed(L7_IP_ADDR_t ipAddr);

/*********************************************************************
*
* @purpose  Prepare to authenticate a new user on an interface. This
*           function essentially adds a new authentication in progress
*           entry into the AIPStatus table.
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
* @param    L7_enetMacAddr_t* macAddr @b{(input)} MAC addr
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
void cpcmPrepareToAuthenticate(void)
{
  L7_RC_t moreWork = L7_FAILURE;
  L7_IP_ADDR_t ipAddr;
  L7_enetMacAddr_t mac;
  L7_uint32 intIfNum;

  moreWork = authWOGet(&ipAddr, &mac, &intIfNum);
  while (L7_SUCCESS == moreWork)
  {
    if (L7_SUCCESS != cpcmProcessAuthPreparation(ipAddr, &mac, intIfNum))
    {
		CP_DLOG(CPD_LEVEL_LOG,
         "%s: Can't prep IP 0x%08x for authentication\n",
         __FUNCTION__,ipAddr);
    }
    moreWork = authWOGet(&ipAddr, &mac, &intIfNum);
  }
}

/*********************************************************************
*
* @purpose  Prepare to authenticate a new user on an interface. This
*           function essentially adds a new authentication in progress
*           entry into the AIPStatus table.
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
* @param    L7_enetMacAddr_t* macAddr @b{(input)} MAC addr
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
L7_RC_t cpcmProcessAuthPreparation(L7_IP_ADDR_t       ipAddr, 
                                   L7_enetMacAddr_t * mac,
                                   L7_uint32          intfId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_enetMacAddr_t macMatched;
  intfCpCallbacks_t * pCBs = L7_NULLPTR;
  L7_uint32 tmpIntfId;
  cpId_t cpId, matchCpId;
  time_t now = 0;
  nimUSP_t usp, auth_usp;
  L7_enetMacAddr_t swMac;
  L7_int32 timeElapsed;
  L7_uint32 maxBandwidthUp = 0;
  L7_uint32 maxBandwidthDown = 0;

#if 0
  CP_DLOG(CPD_LEVEL_LOG,
          "%s:",__FUNCTION__);
#endif

  if ((L7_NULLPTR == mac) || ((L7_IP_ADDR_t) 0 == ipAddr))
  {
    return L7_FAILURE;
  }

  pCBs = cpimIntfOwnerCallbacksGet(intfId);
  if (L7_NULLPTR == pCBs)
  {
    CP_DLOG(CPD_LEVEL_LOG,
            "%s: No callbacks?",__FUNCTION__);
    return L7_FAILURE;
  }

  /* In a clustering environment, if the same IP entry address already exists 
   * in client connection table, tell AP to set its state to "authenticated".
   * This is necessary for roaming.
   */
  if (clusterSupportGet() == L7_SUCCESS)
  {
    /* check if MAC is still waiting for authentication and also verify 
     * interface + cp instance reported by peer match the local information
     */
    if (sendAuthReqMsg == L7_TRUE)
    {
      /* If the same IP entry address already exists in peer client status table,
       * send request message to the peer for authentication information.
       */
      memset(&macMatched, 0, sizeof(L7_enetMacAddr_t));
      if (cpClusterSwitchClientEntryByIPGet(ipAddr, &macMatched) == L7_SUCCESS)
      {
        if (memcmp(&macMatched, mac, sizeof(L7_enetMacAddr_t)) == 0)
        {
          if (cpClusterSwitchClientSwitchMACGet(macMatched, &swMac) == L7_SUCCESS)
          {
            /* send client auth request to previous client authenticator switch */
            if (cpClusterClientAuthReqMsgSend(swMac, macMatched) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "cpcmProcessAuthPreparation: Failed to send client auth request to switch %02x:%02x:%02x:%02x:%02x:%02x.\n",
                      swMac.addr[0], swMac.addr[1], swMac.addr[2],
                      swMac.addr[3], swMac.addr[4], swMac.addr[5]);
              return L7_SUCCESS;
            }

            cpdmOprData->cpLocalSw.authReplyTime = 0;
            cpdmOprData->cpLocalSw.authReqTime = osapiUpTimeRaw();
            cpdmOprData->cpClientAuthInfo.authPending = L7_TRUE;
            sendAuthReqMsg = L7_FALSE;
          }
        }
      }
    }

    while (cpdmOprData->cpClientAuthInfo.authPending == L7_TRUE)
    {
      sendAuthReqMsg = L7_TRUE;
      timeElapsed = cpdmOprData->cpLocalSw.authReplyTime - cpdmOprData->cpLocalSw.authReqTime;
      if ((timeElapsed >= 0) && (timeElapsed <= CP_CLIENT_MAC_AUTH_REQUEST_TIMEOUT))
      {
        if ((cpClusterSwitchClientCPIDGet(*mac, &cpId) == L7_SUCCESS) &&
            (nimGetUnitSlotPort(intfId, &usp) == L7_SUCCESS) &&
            (nimGetUnitSlotPort(cpdmOprData->cpClientAuthInfo.intf, &auth_usp) == L7_SUCCESS))
        {
          if ((cpdmOprData->cpClientAuthInfo.cpId == (L7_uint32)cpId) &&
              (usp.port == auth_usp.port))
          {
            cpdmOprData->cpClientAuthInfo.authPending = L7_FALSE;

            /* if client parameters match, tell interface owner that client is authenticated,
               move client to "Connected Clients" database and tell peer switches it now owns client.
             */
            cpdmClientConnStatusMaxBandwidthUpGet(mac,&maxBandwidthUp);
            cpdmClientConnStatusMaxBandwidthDownGet(mac,&maxBandwidthDown);
            if (pCBs->cpAuthenUnauthenClient(CP_CLIENT_AUTH, *mac, maxBandwidthUp, maxBandwidthDown) == L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG,
                      "%s: Found IP 0x%08x match in peer switch client table, send CP_CLIENT_AUTH. up:%u down:%u"
                      ,__FUNCTION__,ipAddr,maxBandwidthUp,maxBandwidthDown);
              if (cpcmAIPStatusGet(ipAddr) == L7_SUCCESS) 
              {
                L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
                nimGetIntfName(intfId, L7_SYSNAME, ifName);

                if (cpcmAIPStatusIntfIdSet(ipAddr, intfId) == L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "cpcmProcessAuthPreparation: AIP entry IP 0x%x updated intfId %d, %s.\n", ipAddr, intfId, ifName);
                  return L7_SUCCESS;
                }
                else
                {
                  CP_DLOG(CPD_LEVEL_LOG, "cpcmProcessAuthPreparation: Failed to update AIP entry IP 0x%x, intfId %d, %s.\n", ipAddr, intfId, ifName);
                  return L7_FAILURE;      
                }
              }

              rc = cpcmAIPStatusAdd(ipAddr, mac, intfId);
              if (rc == L7_SUCCESS)
              {
                now = time(NULL);
                cpcmAIPStatusConnTimeSet(ipAddr, &now);
              }
              else
              {
                CP_DLOG(CPD_LEVEL_LOG, "cpcmProcessAuthPreparation: Failed to add AIP entry IP 0x%x.\n", ipAddr);    
              }
              cpcmAIPStatusUIDSet(ipAddr, cpdmOprData->cpClientAuthInfo.uName);
              cpcmAIPStatusPwdSet(ipAddr, cpdmOprData->cpClientAuthInfo.pwd);
              cpcmAIPStatusFlagSet(ipAddr, CP_WIP);
              return L7_SUCCESS;
            }
            else
            {
              CP_DLOG(CPD_LEVEL_LOG, "cpcmProcessAuthPreparation: can't reauthenticate client.\n");
              return L7_FAILURE;
            }
          }
        }
      }
      else if (timeElapsed > CP_CLIENT_MAC_AUTH_REQUEST_TIMEOUT)
      {
        cpdmOprData->cpClientAuthInfo.authPending = L7_FALSE;
      }
    }
  }
  
  /* If the same IP entry address already exists in our client connection
  * status table, tell AP to set its state to "authenticated". This is
  * necessary for roaming.
  */
  memset(&macMatched,0,sizeof(L7_enetMacAddr_t));
  if (L7_SUCCESS==cpdmClientConnStatusFirstIPMatch(ipAddr,&macMatched))
  {
    do 
    {
      if (0==memcmp(&macMatched,mac,sizeof(L7_enetMacAddr_t)))
      {
        /* Defect 69178: also check cpId for correct CP instance */
        if ( (L7_SUCCESS==cpdmClientConnStatusIntfIdGet(mac,&tmpIntfId))  &&
             (L7_SUCCESS==cpdmClientConnStatusCpIdGet(&macMatched,&matchCpId)) &&
             (L7_SUCCESS==cpdmClientConnStatusCpIdGet(mac,&cpId)))
        {
          if ((intfId==tmpIntfId) && (cpId == matchCpId))
          {
            cpdmClientConnStatusMaxBandwidthUpGet(mac,&maxBandwidthUp);
            cpdmClientConnStatusMaxBandwidthDownGet(mac,&maxBandwidthDown);
            if (L7_SUCCESS==pCBs->cpAuthenUnauthenClient(CP_CLIENT_AUTH,*mac,maxBandwidthUp,maxBandwidthDown))
            {
              CP_DLOG(CPD_LEVEL_LOG,
                      "%s: Found IP 0x%08x match Client connection table, send CP_CLIENT_AUTH. up:%u down:%u"
                      ,__FUNCTION__,ipAddr,maxBandwidthUp,maxBandwidthDown);
  
              if (L7_SUCCESS != cpdmClientConnStatusStatisticsUpdate(mac, L7_NULLPTR, L7_TRUE)) 
              {
                CP_DLOG(CPD_LEVEL_LOG,
                        "Could not update statistics after roam");
              }
  
              if (L7_SUCCESS==cpcmAIPStatusDelete(ipAddr))
              {
                CP_DLOG(CPD_LEVEL_LOG,
                        "Removed from AIP table");
              }
              return L7_SUCCESS;
            }
            else
            {
              CP_DLOG(CPD_LEVEL_LOG, "can't reauthenticate client\n");
              return L7_FAILURE;
            }
          }
          else
          {
            /* If a wired client already exists in client connection table,
             * delete the client and force it to re-authenticate with updated information. 
             * This is necessary for interface changes.
             */
            if (cpdmIsClientWired(tmpIntfId) == L7_TRUE)
            {
              if (intfId != tmpIntfId)
              {
                if (cpdmClientConnStatusDelete(mac) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "Failed to delete client IP 0x%08x from intf %d", ipAddr, tmpIntfId);
                  return L7_FAILURE;
                }
                return L7_SUCCESS;
              }

              #if 0
              if (cpdmClientConnStatusIntfIdSet(mac, tmpIntfId, intfId) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG,
                        "Failed to update client IP 0x%08x intf", ipAddr);
                return L7_FAILURE;
              }

              cpdmClientConnStatusMaxBandwidthUpGet(mac, &maxBandwidthUp);
              cpdmClientConnStatusMaxBandwidthDownGet(mac, &maxBandwidthDown);
              if (pCBs->cpAuthenUnauthenClient(CP_CLIENT_AUTH, *mac,
                                               maxBandwidthUp, maxBandwidthDown) == L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG,
                        "Found IP 0x%08x in client connection table, send CP_CLIENT_AUTH up:%u down:%u",
                        ipAddr, maxBandwidthUp, maxBandwidthDown);

                if (cpcmAIPStatusDelete(ipAddr) == L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG,
                          "Removed IP 0x%08x from AIP table", ipAddr);
                }
                return L7_SUCCESS;
              }
              else
              {
                CP_DLOG(CPD_LEVEL_LOG,
                        "Failed to reauthenticate client IP 0x%08x", ipAddr);
                return L7_FAILURE;
              }
              #endif
            }
          }
        }
      }
      CP_DLOG(CPD_LEVEL_LOG,
              "%s: Get next client connection table entry looking for IP = 0x%08x",
                __FUNCTION__,ipAddr);
    } while (L7_SUCCESS==cpdmClientConnStatusNextIPMatch(ipAddr, &macMatched));
  }

  /* If we get here, the IP address is NOT in our client connection status table,
   * or the peer client status table, in case clustering is supported,
   * so tell AP to set state to "un-authenticated".  This actually tells the AP
   * to start redirecting traffic.
   */
  if (L7_SUCCESS != pCBs->cpAuthenUnauthenClient(CP_CLIENT_NOT_AUTH, *mac, 0, 0))
  {
    CP_DLOG(CPD_LEVEL_LOG,
            "%s: Can't inform AP of un-auth status (IP = 0x%08x)",
            __FUNCTION__,ipAddr);
  }

  if (L7_SUCCESS == cpcmAIPStatusGet(ipAddr)) 
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intfId, L7_SYSNAME, ifName);

    CP_DLOG(CPD_LEVEL_LOG,
            "%s: AIP table already contains IP 0x%x.",
            __FUNCTION__,ipAddr);
    if (L7_SUCCESS== cpcmAIPStatusIntfIdSet(ipAddr,intfId))
    {
      CP_DLOG(CPD_LEVEL_LOG,
              "%s: AIP table entry IP 0x%x updated intfId %d, %s.",
              __FUNCTION__,ipAddr,intfId,ifName);
      return L7_SUCCESS;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update AIP table entry IP 0x%x, intfId %d, %s.",
              __FUNCTION__,ipAddr,intfId,ifName);
      return L7_FAILURE;      
    }
  }

  rc = cpcmAIPStatusAdd(ipAddr,mac,intfId);
  if (L7_SUCCESS==rc)
  {
    now = time(NULL);
    cpcmAIPStatusConnTimeSet(ipAddr,&now);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "Can't add IP 0x%x for auth in prog\n", ipAddr);    
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Prepare to authenticate a new user on an interface. This
*           teeny tiny function serves as a small wrapper for NIM.
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
* @param    L7_enetMacAddr_t  macAddr @b{(input)} MAC addr
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
L7_RC_t cpcmPrepareToAuthenticate_NIMWrapper(L7_uint32          intIfNum,
                                             L7_enetMacAddr_t   mac,
                                             cpRedirIpAddr_t    ipAddr)
{
  L7_RC_t rc = L7_FAILURE;

#if 0
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  CP_DLOG(CPD_LEVEL_LOG,
          "%s: NIM wrapper for PrepAuth: i/f, ip = %d, %s, 0x%08x",
          __FUNCTION__,intIfNum,ifName,ipAddr.ipAddr.redirIpv4Addr);
#endif

  if (CP_IPV4 != ipAddr.redirIpAddrType)
  {
    CP_DLOG(CPD_LEVEL_LOG,
            "Captive Portal doesn't support non-IPv4 addresses yet. Sorry.\n");
    return L7_FAILURE;
  }

  rc = authWOAdd(ipAddr.ipAddr.redirIpv4Addr, &mac, intIfNum);
  if (L7_FAILURE == rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: cannot prepare work item\n", __FUNCTION__);
  }
  else
  {
    captivePortalClientAuthHandle();
  }
  return rc;
}

/*****************************************************************************
*
* @purpose  This function services UI authentication requests. It first
*           searches the client connection status table to determine if this
*           user has been authenticated. If so, it returns authentication
*           success. If not found in the client connection status table, it
*           should be in the aip table. If it is, and this function was called
*           as the initial request (indicated by the user->flag w/validate)
*           then the aip table entry is updated to indicate an authentication
*           request that includes the uid, pwd, connection time and wip status
*           flag. If its not an initial request (then its a wip refresh), so
*           simply return the latest status flag from the aip table.
*
* @param    cpUserAuth_t            user   @b{(input)} user related params
* @param    CP_AUTH_STATUS_FLAG_t   *flag  @b{(output)} current auth flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
L7_RC_t cpcmUserAuthRequest(cpUserAuth_t *user, CP_AUTH_STATUS_FLAG_t *flag)
{
  L7_RC_t status = L7_FAILURE;
  L7_IP_ADDR_t            ipAddr;       /* AVL KEY, Client MAC waiting for authorization */
  L7_enetMacAddr_t        macAddr;
  CP_AUTH_STATUS_FLAG_t   localFlag = CP_DEFAULT; 

  if (!user || !flag)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad parms", __FUNCTION__);
    return status;
  }

  CP_DLOG(CPD_LEVEL_LOG,
          "%s: user auth: uid = %s",__FUNCTION__,user->uid);

  CP_DLOG(CPD_LEVEL_LOG,
          "%s: user auth: user flag = %d",__FUNCTION__,user->flag);

  ipAddr = user->ipAddr;
  cpcmAIPStatusFlagGet(ipAddr, &localFlag);

  CP_DLOG(CPD_LEVEL_LOG,
          "%s: user auth: AIP status flag = %d",__FUNCTION__,localFlag);

  if (L7_SUCCESS==cpcmAIPStatusGet(ipAddr))
  {
    CP_DLOG(CPD_LEVEL_LOG,
            "%s: AIP table contains IP 0x%08x.",
            __FUNCTION__,ipAddr);
    /*
     * Go ahead and authenticate as long as we havn't timed out.
     * This is denoted by the AIP status flag being CP_DEFAULT
     * which means that the timer was exhausted, or CP_TIMEOUT.
     * CP_TIMEOUT is the state where the client timed out but has
     * yet to make a new connection.
    */
    if (((CP_VALIDATE == user->flag) && (CP_VALIDATE == localFlag)) ||
       (((CP_VALIDATE == user->flag) && (CP_DEFAULT != localFlag)) &&
        ((CP_VALIDATE == user->flag) && (CP_TIMEOUT != localFlag))))
    {
      /* kickoff authentication request */
      CP_DLOG(CPD_LEVEL_LOG,
              "%s: Kickoff auth req for ip 0x%08x, user %s",
              __FUNCTION__,ipAddr,user->uid);

      *flag = CP_WIP;
      cpcmAIPStatusFlagSet(ipAddr,CP_WIP);
      cpcmAIPStatusUserSpecificInfoAdd(ipAddr, user->uid, user->pwd, L7_NULL);
      captivePortalTaskWake(); /* get CP task to finish authentication ASAP */
    }
    else
    {
      /*
       * State in serve function wasn't set to CP_VALIDATE (localFlag wasn't
       * equal to CP_VALIDATE). This means that it's new connection and
       * autentification timeout state should be set. Please note that the
       * AIPStatusFlag is reset to default when the session times out.
      */
      if ((CP_VALIDATE == user->flag) && (CP_DEFAULT == localFlag)) 
      {
        *flag = localFlag = CP_TIMEOUT;
        cpcmAIPStatusFlagSet(ipAddr, localFlag);
      }

      CP_DLOG(CPD_LEVEL_LOG,
              "%s: AIP entry 0x%08x: status flag is %d",
              __FUNCTION__,ipAddr,localFlag);
      switch (localFlag)
      {
        case CP_TIMEOUT:
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x auth timed out: TIMEOUT..",__FUNCTION__,ipAddr);
          break;
        }
        case CP_RESOURCE:
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x no more resources: RESOURCES..",__FUNCTION__,ipAddr);
          break;
        }
        case CP_RADIUS_WIP:
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x auth, radius waiting for AUTH command..",__FUNCTION__,ipAddr);
          break;
        }
        case CP_DENIED:
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x auth, auth failed: DENIED..",__FUNCTION__,ipAddr);
          cpcmUserAuthFailed(ipAddr);
          break;
        }
        default:
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x auth, default switch flag = %d..",__FUNCTION__,ipAddr,localFlag);
              break;
        }
      }
      *flag = localFlag;
    }
    status = L7_SUCCESS;
  }
  else if (L7_SUCCESS==cpdmClientConnStatusFirstIPMatch(ipAddr,&macAddr))
  {
    CP_DLOG(CPD_LEVEL_LOG,
            "%s: Client connection table contains IP 0x%08x, return success.",
            __FUNCTION__,ipAddr);
    *flag = CP_SUCCESS;
    status = L7_SUCCESS;
  }
  else
  {
    /* user not in connected or aip db (could have timed out) */
    CP_DLOG(CPD_LEVEL_LOG, "%s: User uid %s is neither connected nor in progress",
            __FUNCTION__,user->uid);
    *flag = CP_RESOURCE;
    status = L7_FAILURE;
  }
  return status;
}

/*********************************************************************
*
* @purpose  Do the dirty work of authentication in the CP task
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmTryAuthentication(void)
{
  return cpcmTryAuth();
}

/**************************************************************************
* @purpose   Process RADIUS Accept from server
*
* @param     cpdmVSRadiusAttr_t @b{(input)} generic RADIUS AVP
* @param     cpdmRadiusEntry_t @b{(input)} CP related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t cpcmRadiusVendorAttrProcess(CP_vsRadiusAttr_t *radiusAttr,
                                    CP_radiusEntry_t *radiusEntry)
{
  CP_DLOG(CPD_LEVEL_LOG, "%s: Vendor-specific attr vendor %d, type: %d, length %d",
         __FUNCTION__,
         osapiNtohl(radiusAttr->vendor),
         radiusAttr->vType,
         radiusAttr->vLength);

  if (osapiNtohl(radiusAttr->vendor) == RADIUS_VENDOR_ID_WISPR)
  {
    switch (radiusAttr->vType)
    {
      case RADIUS_VENDOR_ATTR_WISPR_BANDWIDTH_MAX_UP:
      {
        radiusEntry->maxBandwidthUp = (L7_uint32) osapiNtohl(*((L7_uint32 *)
                                                               (((L7_uchar8 *)radiusAttr) +
                                                                CP_RAD_VEN_ATTR_SIZE)));
        break;
      }
      case RADIUS_VENDOR_ATTR_WISPR_BANDWIDTH_MAX_DOWN:
      {
        radiusEntry->maxBandwidthDown = (L7_uint32) osapiNtohl(*((L7_uint32 *)
                                                                 (((L7_uchar8 *)radiusAttr) +
                                                                  CP_RAD_VEN_ATTR_SIZE)));
        break;
      }
      default:
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid Attribute type %d\n",__FUNCTION__,radiusAttr->vType);
        return L7_FAILURE;
      }
    }
  }
  else if (osapiNtohl(radiusAttr->vendor) == RADIUS_LVL7_VENDOR_ID)
  {
    switch (radiusAttr->vType)
    {
      case LVL7_MAX_INPUT_OCTETS_VATTR:
      {
        radiusEntry->maxInputOctets = (L7_uint32) osapiNtohl(*((L7_uint32 *)
                                                               (((L7_uchar8 *)radiusAttr) +
                                                                CP_RAD_VEN_ATTR_SIZE)));
        break;
      }
      case LVL7_MAX_OUTPUT_OCTETS_VATTR:
      {
        radiusEntry->maxOutputOctets = (L7_uint32) osapiNtohl(*((L7_uint32 *)
                                                                (((L7_uchar8 *)radiusAttr) +
                                                                 CP_RAD_VEN_ATTR_SIZE)));
        break;
      }
      case LVL7_MAX_TOTAL_OCTETS_VATTR:
      {
        radiusEntry->maxTotalOctets = (L7_uint32) osapiNtohl(*((L7_uint32 *)
                                                               (((L7_uchar8 *)radiusAttr) +
                                                                CP_RAD_VEN_ATTR_SIZE)));
        break;
      }
      case LVL7_CAPTIVE_PORTAL_GROUPS_VATTR:
      {
        memcpy(radiusEntry->userGroups,
               (((L7_uchar8 *)radiusAttr) + CP_RAD_VEN_ATTR_SIZE),
               ((radiusAttr->vLength - L7_CP_RAD_BASE_ATTR_SIZE >
                 CP_USER_RADIUS_USERGROUPS_MAX) ? CP_USER_RADIUS_USERGROUPS_MAX :
                (radiusAttr->vLength - L7_CP_RAD_BASE_ATTR_SIZE)));

        if (radiusAttr->vLength - L7_CP_RAD_BASE_ATTR_SIZE > CP_USER_RADIUS_USERGROUPS_MAX) 
            radiusEntry->userGroups[CP_USER_RADIUS_USERGROUPS_MAX] = '\0';
        else
            radiusEntry->userGroups[radiusAttr->vLength - L7_CP_RAD_BASE_ATTR_SIZE] = '\0';
        break;
      }
      default:
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Invalid Attribute type %d\n",__FUNCTION__,radiusAttr->vType);
        return L7_FAILURE;
      }
    }
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Vendor ID 0x%08x unrecognized\n",__FUNCTION__,osapiNtohl(radiusAttr->vendor));
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/**************************************************************************
*
* @purpose   RADIUS callback - For each response, update the AIP table accordingly
*
* @param     status         @b{(input)} status of RADIUS response (accept, reject, challenge, etc)
* @param     correlator     @b{(input)} correlates responses to requests; for captive portal its the client IP
* @param     *attributes    @b{(input)} RADIUS attribute data
* @param     attributesLen  @b{(input)} length of RADIUS attribute data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  
*         
* @end
*************************************************************************/
L7_RC_t cpcmRadiusResponseCallback(L7_uint32 status,
                                   L7_uint32 correlator,
                                   L7_uchar8 *attributes,
                                   L7_uint32 attributesLen)
{
  CP_AUTH_STATUS_FLAG_t flag;
  CP_radiusAttr_t *radiusAttr;
  CP_radiusEntry_t radiusEntry = {0, 0, 0, 0, 0, 0, 0, ""};
  L7_uint32 attrVal;
  L7_BOOL fDone = L7_FALSE;

  radiusAttr = (CP_radiusAttr_t *)attributes;

  switch (status)
  {
    case RADIUS_STATUS_SUCCESS:
    {
      while (fDone == L7_FALSE)
      {
        if (radiusAttr->type == RADIUS_ATTR_TYPE_SESSION_TIMEOUT) /* Session-Timeout */
        {
          memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(CP_radiusAttr_t),
               radiusAttr->length - sizeof(CP_radiusAttr_t));
          radiusEntry.sessionTimeout = attrVal;
        }
        else if (radiusAttr->type == RADIUS_ATTR_TYPE_IDLE_TIMEOUT) /* Idle-Timeout */
        {
          memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(CP_radiusAttr_t),
               radiusAttr->length - sizeof(CP_radiusAttr_t));
          radiusEntry.idleTimeout = attrVal;
        }
        else if (radiusAttr->type == RADIUS_ATTR_TYPE_VENDOR)
        {
          cpcmRadiusVendorAttrProcess((CP_vsRadiusAttr_t *)(radiusAttr+1),&radiusEntry);
        }

        attributesLen -= radiusAttr->length;
        if (attributesLen <= (L7_uint32)sizeof(CP_radiusAttr_t))
        {
          fDone = L7_TRUE;
        }
        else
        {
          radiusAttr = (CP_radiusAttr_t *)((L7_uchar8 *)radiusAttr + radiusAttr->length);
        }
        if (radiusAttr->length <= 0)
        {
          fDone = L7_TRUE;
        }
      }

      cpcmAIPStatusUserSpecificInfoAdd(correlator, L7_NULL, L7_NULL, &radiusEntry);

      flag = (L7_SUCCESS == cpcmRADIUSGroupCheck(correlator,&radiusEntry))?CP_RADIUS_WIP:CP_DENIED;
      break;
    }
    case RADIUS_STATUS_AUTHEN_FAILURE:
    {
      CP_DLOG(CPD_LEVEL_LOG,
              "%s: Client IP 0x%08x RADIUS auth failed: DENIED..",__FUNCTION__,correlator);
      flag = CP_DENIED;
      break;
    }
    case RADIUS_STATUS_REQUEST_TIMED_OUT:
    {
      CP_DLOG(CPD_LEVEL_LOG,
              "%s: Client IP 0x%08x RADIUS auth timed out: TIMEOUT..",__FUNCTION__,correlator);
      flag = CP_TIMEOUT;
      break;
    }
    default:
    {
      CP_DLOG(CPD_LEVEL_LOG,
              "%s: Client IP 0x%08x RADIUS auth failed: RESOURCES..",__FUNCTION__,correlator);
      flag = CP_RESOURCE;
      break;
    }
  }

  return cpcmAIPStatusFlagSet(correlator,flag);
}


/*****************************************************************************
*
* @purpose  This function takes care of user authentication failures
*
* @param    L7_IP_ADDR_t ipAddr @b{(input)} failed client 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
L7_RC_t cpcmUserAuthFailed(L7_IP_ADDR_t ipAddr)
{
  L7_uchar8 uid[CP_USER_LOCAL_USERNAME_MAX + 1];
  L7_enetMacAddr_t macAddr, swMacAddr;
  L7_uint32 intfId = 0;
  cpId_t cpId = 0;
  L7_RC_t rc;

  CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x auth failed: DENIED..",__FUNCTION__,ipAddr);

  memset(&macAddr,0,sizeof(L7_enetMacAddr_t));
  memset(&swMacAddr,0,sizeof(L7_enetMacAddr_t));
  memset(uid,0,sizeof(uid));

  rc = cpcmAIPStatusMacAddrGet(ipAddr,&macAddr);
  if (rc == L7_SUCCESS)
  {
    rc = cpcmAIPStatusUIDGet(ipAddr,uid);
  }
  if (rc == L7_SUCCESS)
  {
    rc = cpcmAIPStatusIntfIdGet(ipAddr,&intfId);
  }
  if (rc == L7_SUCCESS)
  {
    rc = cpdmCPConfigIntIfNumFind(intfId,&cpId);
  }
  if (rc == L7_SUCCESS)
  {
    /* We don't currently track the number of authentication failures. */
    memcpy(&(swMacAddr),&macAddr,sizeof(L7_enetMacAddr_t));
    cpTrapClientAuthFailure(&macAddr,ipAddr,uid,&swMacAddr,cpId,intfId,1);
  }

#if 0
  TBD - client authorization failure status table
  if (L7_SUCCESS==cpdmClientAuthFailStatusGet(aip.macAddr))
  {
    // update if failure entry already exists
    cpdmClientAuthFailStatusLastConnAttemptSet(aip.macAddr,time(NULL));
    cpdmClientAuthFailStatusConnAttemptGet(aip.macAddr,&connAttempt);
    cpdmClientAuthFailStatusConnAttemptSet(aip.macAddr,++connAttempt);
  }
  else
  {
    // create new client authentication failure
    cpdmClientAuthFailStatusAdd(aip.macAddr);
    cpdmClientAuthFailStatusLastConnAttemptSet(aip.macAddr,time(NULL));
    cpdbClientAuthFailStatusCreateTime(aip.macAddr,time(NULL));
    cpdmClientAuthFailStatusIpSet(aip.macAddr,aip.ipAddr);
    cpdmClientAuthFailStatusConnAttemptSet(aip.macAddr,1);
    cpdmClientAuthFailStatusSwitchMacAddrSet(aip.macAddr,0);
    cpdmClientAuthFailStatusSwitchIpSet(aip.macAddr,ews.ipAddr);
    cpdmClientAuthFailStatusWhichSwitchSet(CP_AUTH_ON_LOCAL_SWITCH);
    cpdmClientAuthFailStatusUserNameSet(aip.macAddr,aip.loginName);
    cpdmCPConfigProtocolModeGet(cpId,&protocolMode);
    cpdmClientAuthFailStatusProtocolModeSet(aip.macAddr,protocolMode);
    cpdmClientAuthFailStatusVerifyModeSet(aip.macAddr,verifyMode);
    cpdmClientAuthFailStatusIntfIdSet(aip.macAddr,aip.intfId);
    cpdmClientAuthFailStatusCpIdSet(aip.macAddr,cpId);
  }
  cpdmGlobalActivityLogEntryAdd("Denied: Client authentication failed; invalid credentials.");
#endif

  return rc;
}


/*********************************************************************
*
*
*  AUTHORIZATION IN PROGRESS STATUS DB FUNCTIONS
*
*
*********************************************************************/

/*********************************************************************
*
* @purpose  Initialize our AIPStatus  tree
*
* @param    L7_uint32  maxAIPStatus  @b{(input)} max entries to support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusInit(L7_uint32 maxAIPStatus)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if (L7_SUCCESS != authWOQInit(maxAIPStatus))
    {
       break; /* no point in continuing */
    }

    aipStatusTreeHeap = (avlTreeTables_t *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxAIPStatus * sizeof(avlTreeTables_t));
    if (L7_NULLPTR == aipStatusTreeHeap)
    {
       break; /* no point in continuing */
    }
    aipStatusDataHeap = (void *) 
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                  maxAIPStatus * sizeof(cpcmAuthInProgressStatusData_t));
    if (L7_NULLPTR == aipStatusDataHeap)
    {
       break; /* no point in continuing */
    }
    avlCreateAvlTree(&aipStatusTree, aipStatusTreeHeap, aipStatusDataHeap,
                     maxAIPStatus, sizeof(cpcmAuthInProgressStatusData_t),
                     0,  sizeof(L7_IP_ADDR_t));
    aipStatusTreeReady = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);

  if (L7_SUCCESS != rc)
  {
    if ( (L7_NULLPTR == aipStatusDataHeap) ||
         (L7_NULLPTR == aipStatusTreeHeap)) 
    {
      L7_LOG_ERROR(0); /* no point in continuing */
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Finalize our auth-in-progress tree
*
* @parms    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpcmAIPStatusFinalize(void)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_IP_ADDR_t addr;

  authWOQFinalize();
  if (!aipStatusTreeReady)
  {
    return;
  }

  SEMA_TAKE(WRITE);   
  do 
  {
    addr = (L7_IP_ADDR_t) 0;
    pEntry = avlSearchLVL7(&aipStatusTree, &addr, AVL_NEXT);   

    if (L7_NULLPTR != pEntry) 
    {
      pEntry = avlDeleteEntry(&aipStatusTree, pEntry);
    }
  } while(pEntry != L7_NULLPTR);
  SEMA_GIVE(WRITE);   
}

/*********************************************************************
*
* @purpose  Find a tree entry with the given intf id as a key
*
* @param    L7_uint32  intfId   @b{(input)} the key, of course
*
* @returns  void  pointer to cpcmIntfCapabilityStatusData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
static 
cpcmAuthInProgressStatusData_t *
cpcmFindAIPStatus(L7_IP_ADDR_t ipAddr)
{
  if (!aipStatusTreeReady)
  {
    return L7_NULLPTR;
  }
  return avlSearchLVL7(&aipStatusTree, &ipAddr, AVL_EXACT);   
}

/*********************************************************************
*
* @purpose  Add user-specific info (e.g. timeouts) to an AIP entry.
*           In addition to tracking authentication in progress, this
*           table contains the transient data coming from web session
*           and also the various parameters obtained from RADIUS.
*
* @param    L7_IP_ADDR_t  ipAddr   @b{(input)} IP address of entry
* @param    L7_enetMacAddr_t * mac @b{(input)} MAC address of entry
* @param    L7_uint32     intIfNum @b{(input)} int I/F number of entry
* @param    L7_uint8      * uid    @b{(input)} username
* @param    L7_uint8      * pwd    @b{(input)} password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
static 
L7_RC_t 
cpcmAIPStatusUserSpecificInfoAdd(L7_IP_ADDR_t       ipAddr, 
                                 L7_uint8 *         uid,
                                 L7_uint8 *         pwd,
                                 CP_radiusEntry_t * radiusEntry)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 where = 0;
  cpId_t cpId = 0;
  uId_t userId = 0;
  L7_uint32 intIfNum = 0;
  CP_VERIFY_MODE_t verifyMode;

  do 
  {
    L7_uint32 sessionTimeout = 0;
    L7_uint32 idleTimeout = 0;
    L7_uint32 maxBandwidthUp = 0;
    L7_uint32 maxBandwidthDown = 0;
    L7_uint32 maxInputOctets = 0;
    L7_uint32 maxOutputOctets = 0;
    L7_uint32 maxTotalOctets = 0;

    if ((L7_IP_ADDR_t) 0 == ipAddr)
    {
      where = __LINE__;
      break;
    }

    if ((L7_SUCCESS != cpcmAIPStatusIntfIdGet(ipAddr, &intIfNum)) ||
        (L7_SUCCESS != cpdmCPConfigIntIfNumFind(intIfNum, &cpId)))
    {
      where = __LINE__;
      break;
    }

    cpdmCPConfigVerifyModeGet(cpId,&verifyMode);

    if (radiusEntry == L7_NULLPTR)
    {
      if ((L7_NULLPTR == uid) || (L7_NULLPTR == pwd))
      {
        where = __LINE__;
        break;
      }

      if ((L7_SUCCESS != cpcmAIPStatusUIDSet(ipAddr,uid)) ||
          (L7_SUCCESS != cpcmAIPStatusPwdSet(ipAddr,pwd)))
      {
        where = __LINE__;
        break;
      }

      if (CP_VERIFY_MODE_LOCAL == verifyMode)
      {
        cpdmUserEntryByNameGet(uid,&userId);
        cpdmUserEntrySessionTimeoutGet(userId,&sessionTimeout);
        cpdmUserEntryIdleTimeoutGet(userId,&idleTimeout);
        cpdmUserEntryMaxBandwidthUpGet(userId,&maxBandwidthUp);
        cpdmUserEntryMaxBandwidthDownGet(userId,&maxBandwidthDown);
        cpdmUserEntryMaxInputOctetsGet(userId,&maxInputOctets);
        cpdmUserEntryMaxOutputOctetsGet(userId,&maxOutputOctets);
        cpdmUserEntryMaxTotalOctetsGet(userId,&maxTotalOctets);
      }
    }
    else
    {
      sessionTimeout    = radiusEntry->sessionTimeout;
      idleTimeout       = radiusEntry->idleTimeout;
      maxBandwidthUp    = radiusEntry->maxBandwidthUp;
      maxBandwidthDown  = radiusEntry->maxBandwidthDown;
      maxInputOctets    = radiusEntry->maxInputOctets;
      maxOutputOctets   = radiusEntry->maxOutputOctets;
      maxTotalOctets    = radiusEntry->maxTotalOctets;
      /* No need to add the user groups at this time */
    }

    /* For each param, if user value is 0, get associated value from CP config */
    if (0 == sessionTimeout)
    {
      if (L7_SUCCESS != cpdmCPConfigSessionTimeoutGet(cpId,&sessionTimeout))
      {
        where = __LINE__;
        break;
      }
    }
    if (0 == idleTimeout)
    {
      if (L7_SUCCESS != cpdmCPConfigIdleTimeoutGet(cpId,&idleTimeout))
      {
        where = __LINE__;
        break;
      }
    }
    if (0 == maxBandwidthUp)
    {
      if (L7_SUCCESS != cpdmCPConfigUserUpRateGet(cpId,&maxBandwidthUp))
      {
        where = __LINE__;
        break;
      }
    }
    if (0 == maxBandwidthDown)
    {
      if (L7_SUCCESS != cpdmCPConfigUserDownRateGet(cpId,&maxBandwidthDown))
      {
        where = __LINE__;
        break;
      }
    }
    if (0 == maxInputOctets)
    {
      if (L7_SUCCESS != cpdmCPConfigMaxInputOctetsGet(cpId,&maxInputOctets))
      {
        where = __LINE__;
        break;
      }
    }
    if (0 == maxOutputOctets)
    {
      if (L7_SUCCESS != cpdmCPConfigMaxOutputOctetsGet(cpId,&maxOutputOctets))
      {
        where = __LINE__;
        break;
      }
    }
    if (0 == maxTotalOctets)
    {
      if (L7_SUCCESS != cpdmCPConfigMaxTotalOctetsGet(cpId,&maxTotalOctets))
      {
        where = __LINE__;
        break;
      }
    }

    /* Now save each param in the AIP table */
    if (L7_SUCCESS != cpcmAIPStatusSessionTimeoutSet(ipAddr,sessionTimeout))
    {
      where = __LINE__;
      break;
    }
    if (L7_SUCCESS != cpcmAIPStatusIdleTimeoutSet(ipAddr,idleTimeout))
    {
      where = __LINE__;
      break;
    }
    if (L7_SUCCESS != cpcmAIPStatusMaxBandwidthUpSet(ipAddr,maxBandwidthUp))
    {
      where = __LINE__;
      break;
    }
    if (L7_SUCCESS != cpcmAIPStatusMaxBandwidthDownSet(ipAddr,maxBandwidthDown))
    {
      where = __LINE__;
      break;
    }
    if (L7_SUCCESS != cpcmAIPStatusMaxInputOctetsSet(ipAddr,maxInputOctets))
    {
      where = __LINE__;
      break;
    }
    if (L7_SUCCESS != cpcmAIPStatusMaxOutputOctetsSet(ipAddr,maxOutputOctets))
    {
      where = __LINE__;
      break;
    }
    if (L7_SUCCESS != cpcmAIPStatusMaxTotalOctetsSet(ipAddr,maxTotalOctets))
    {
      where = __LINE__;
      break;
    }

    rc = L7_SUCCESS;
  } while(0);

  if (L7_FAILURE == rc)
  {
    CP_DLOG(CPD_LEVEL_LOG,
      "%s: cpcmAIPStatus set failed near line %d",__FUNCTION__, where);

  }
  return rc;
}

/*********************************************************************
*
* @purpose  Check to see if one of the assigned RADIUS groups for this
*           user matches the intended CP instance.
*
* @param    L7_IP_ADDR_t ipAddr @b{(input)} Client IP address
* @param    CP_radiusEntry_t * radiusEntry @b{(input)} RADIUS info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Allow default access if the user doesn't have a group.
*           The radiusEntry contains userGroups which is a comma-
*           delimited list of group names. One of these group
*           names must match the intended CP instance.
*
* @end
*
*********************************************************************/
static 
L7_RC_t 
cpcmRADIUSGroupCheck(L7_IP_ADDR_t ipAddr, CP_radiusEntry_t * radiusEntry)
{
  L7_char8 userGroups[CP_USER_RADIUS_USERGROUPS_MAX+1];
  L7_char8 groupName[CP_USER_LOCAL_USERGROUP_MAX+1];
  L7_uint32 intfId = 0;
  cpId_t cpId = 0;
  gpId_t gpId = 0;
  L7_RC_t rc = L7_SUCCESS;

  memset(userGroups,0,sizeof(userGroups));
  osapiStrncpySafe(userGroups,radiusEntry->userGroups,sizeof(userGroups));

  if (0 < strlen(userGroups))
  {
    memset(groupName,0,sizeof(groupName));
    rc = cpcmAIPStatusIntfIdGet(ipAddr,&intfId);
    if (rc == L7_SUCCESS)
    {
      rc = cpdmCPConfigIntIfNumFind(intfId,&cpId);
    }
    if (rc == L7_SUCCESS)
    {
      rc = cpdmCPConfigGpIdGet(cpId,&gpId);
    }
    if (rc == L7_SUCCESS)
    {
      rc = cpdmUserGroupEntryNameGet(gpId,groupName);
    }
    if (rc == L7_SUCCESS)
    {
      osapiSnprintfcat(groupName,sizeof(groupName)-strlen(groupName),"%c",',');
      osapiSnprintfcat(userGroups,sizeof(userGroups)-strlen(userGroups),"%c",',');
      osapiConvertToLowerCase(groupName);
      osapiConvertToLowerCase(userGroups);
    }

    if (strstr(userGroups,groupName)==NULL)
    {
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Add IP/MAC address pair to  authorization in 
*           progress table
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
* @param    L7_enetMacAddr_t* macAddr @b{(input)} MAC addr
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
L7_RC_t cpcmAIPStatusAdd(L7_IP_ADDR_t       ipAddr, 
                         L7_enetMacAddr_t * mac,
                         L7_uint32          intfId)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  static cpcmAuthInProgressStatusData_t entry;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || (L7_NULLPTR == mac))
  {
    CP_DLOG(CPD_LEVEL_LOG, "cpcmAIPStatusAdd: Failed because either tree or MAC is NULL");
    return L7_FAILURE;
  }
  SEMA_TAKE(WRITE);   
  memset(&entry, 0, sizeof(entry));
  entry.ipAddr = ipAddr;
  entry.desc.intfId = intfId;
  memcpy(&entry.desc.macAddr, mac, sizeof(L7_enetMacAddr_t));
  do 
  {
    pEntry = avlInsertEntry(&aipStatusTree, &entry);
    if (L7_NULLPTR != pEntry) /* if item not inserted */
      break;                  /* ..then die           */
    rc = L7_SUCCESS;
#if 0
    CP_DLOG(CPD_LEVEL_LOG,
            "%s: Successfully added IP 0x%08x to AIP table",__FUNCTION__,entry.ipAddr);
#endif
  } while(0);
  SEMA_GIVE(WRITE);   
  return rc;
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
L7_RC_t cpcmAIPStatusDelete(L7_IP_ADDR_t ipAddr)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  static cpcmAuthInProgressStatusData_t entry;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);   
  entry.ipAddr = ipAddr;
  do 
  {
    pEntry = avlSearchLVL7(&aipStatusTree, &entry, AVL_EXACT);
    if (L7_NULLPTR != pEntry) 
    {
      pEntry = avlDeleteEntry(&aipStatusTree, &entry);
      if (L7_NULLPTR == pEntry) 
        break;                  
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   
  return rc;
}

/*********************************************************************
*
* @purpose  Delete all the entries from the AIP table for the given interface
*
* @param    L7_uint32 intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusCleanup(L7_uint32 intIfNum)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  static cpcmAuthInProgressStatusData_t entry;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);   
  memset(&entry, 0, sizeof(cpcmAuthInProgressStatusData_t));
  while ((pEntry = avlSearchLVL7(&aipStatusTree, &entry, AVL_NEXT)) != L7_NULLPTR)
  {
    entry.ipAddr = pEntry->ipAddr;
    if (intIfNum == pEntry->desc.intfId)
    {
      avlDeleteEntry(&aipStatusTree, &entry);
    }
  }

  SEMA_GIVE(WRITE);   
  return rc;
}

/*** GETTERS ***/

/*********************************************************************
*
* @purpose  Check for existing IP address with authorization in 
*           progress
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
L7_RC_t cpcmAIPStatusGet(L7_IP_ADDR_t ipAddr)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;

  if (!aipStatusTreeReady)
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);   
  pEntry = cpcmFindAIPStatus(ipAddr);
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Get description for existing IP address with authorization 
*           in  progress, if it exists
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    cpcmAuthInProgressDesc_t *  @b{(output)} description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusDescGet(L7_IP_ADDR_t ipAddr,
			     cpcmAuthInProgressStatusDesc_t * pDesc)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;

  if (!aipStatusTreeReady || (L7_NULLPTR == pDesc))
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);   
  pEntry = cpcmFindAIPStatus(ipAddr);
  if (pEntry != L7_NULLPTR)
  {
      memcpy(pDesc, &(pEntry->desc), sizeof(cpcmAuthInProgressStatusDesc_t));
  }
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Get limits for existing IP address with authorization 
*           in progress, if it exists
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    cpConnectionLimits_t *  @b{(output)} limits
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusLimitsGet(L7_IP_ADDR_t ipAddr,
			       cpConnectionLimits_t * pLimits)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;

  if (!aipStatusTreeReady || (L7_NULLPTR == pLimits))
  {
    return L7_FAILURE;
  }
  SEMA_TAKE(READ);   
  pEntry = cpcmFindAIPStatus(ipAddr);
  if (pEntry != L7_NULLPTR)
  {
      memcpy(pLimits, &(pEntry->limits), sizeof(cpConnectionLimits_t));
  }
  SEMA_GIVE(READ);   
  return  (L7_NULLPTR != pEntry) ? L7_SUCCESS : L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Check for NEXT IP address with authorization in 
*           progress, given a current IP address
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_IP_ADDR_t  *nextIpAddr  @b{(output)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusNextGet(L7_IP_ADDR_t ipAddr, L7_IP_ADDR_t * nextIpAddr)
{
  L7_RC_t rc = L7_FAILURE;
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;

  if ((!aipStatusTreeReady) || (L7_NULLPTR == nextIpAddr))
    return L7_FAILURE;
  SEMA_TAKE(READ);   
  do 
  {
    pEntry = avlSearchLVL7(&aipStatusTree, &ipAddr, AVL_NEXT);   
    if (L7_NULLPTR == pEntry) 
      break;
    *nextIpAddr = pEntry->ipAddr;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   
  return  rc;
}

/*********************************************************************
*
* @purpose  Retrieve MAC address for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr   @b{(input)} IP addr
* @param    L7_enetMacAddr_t *mac  @b{(output)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMacAddrGet(L7_IP_ADDR_t ipAddr, L7_enetMacAddr_t * mac)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !mac)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    memcpy(mac, &(pEntry->desc.macAddr), sizeof(L7_enetMacAddr_t));
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve interface ID for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32 *   intfId  @b{(output)} I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusIntfIdGet(L7_IP_ADDR_t ipAddr, L7_uint32 * pIntfId)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !pIntfId)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    *pIntfId = pEntry->desc.intfId;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve client connection port for existing IP address
*           with authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     * port  @b{(output)} client port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusPortGet(L7_IP_ADDR_t ipAddr, L7_uint32 * port)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !port)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    *port = pEntry->desc.port;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve session timeout for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   *  timeout  @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusSessionTimeoutGet(L7_IP_ADDR_t ipAddr, L7_uint32 * timeout)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !timeout)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    *timeout = pEntry->limits.sessionTimeout;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve idle timeout for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   *  timeout  @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusIdleTimeoutGet(L7_IP_ADDR_t ipAddr, L7_uint32 * timeout)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !timeout)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    *timeout = pEntry->limits.idleTimeout;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve max bandwidth up rate from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxBandwidthUp  @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxBandwidthUpGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxBandwidthUp)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !maxBandwidthUp)
  {
    return rc;
  }

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    *maxBandwidthUp = pEntry->limits.maxBandwidthUp;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve max bandwidth down rate from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxBandwidthDown @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxBandwidthDownGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxBandwidthDown)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !maxBandwidthDown)
  {
    return rc;
  }

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    *maxBandwidthDown = pEntry->limits.maxBandwidthDown;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve max input octets from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxInputOctets @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxInputOctetsGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxInputOctets)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !maxInputOctets)
  {
    return rc;
  }

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    *maxInputOctets = pEntry->limits.maxInputOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve max output octets from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxOutputOctets @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxOutputOctetsGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxOutputOctets)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !maxOutputOctets)
  {
    return rc;
  }

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    *maxOutputOctets = pEntry->limits.maxOutputOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve max total octets from authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32   * maxTotalOctets @b{(output)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxTotalOctetsGet(L7_IP_ADDR_t ipAddr, L7_uint32 * maxTotalOctets)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !maxTotalOctets)
  {
    return rc;
  }

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    *maxTotalOctets = pEntry->limits.maxTotalOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve connection time for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    time_t   *  connTime  @b{(output)} connection time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusConnTimeGet(L7_IP_ADDR_t ipAddr, time_t * connTime)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !connTime)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    memcpy(connTime, &(pEntry->desc.connTime), sizeof(time_t));
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}


/*********************************************************************
*
* @purpose  Retrieve uid for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uchar8  *  uid  @b{(output)} user ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusUIDGet(L7_IP_ADDR_t ipAddr, L7_uchar8 * uid)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !uid)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    osapiStrncpySafe(uid, pEntry->desc.uid, CP_USER_LOCAL_USERNAME_MAX+1);
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}


/*********************************************************************
*
* @purpose  Retrieve password for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uchar8  *  pwd  @b{(output)} password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusPwdGet(L7_IP_ADDR_t ipAddr, L7_uchar8 * pwd)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !pwd)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    osapiStrncpySafe(pwd, pEntry->desc.pwd, L7_PASSWORD_SIZE);
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}


/*********************************************************************
*
* @purpose  Retrieve authorization flag for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    CP_AUTH_STATUS_FLAG_t * flag  @b{(output)} auth flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusFlagGet(L7_IP_ADDR_t ipAddr, 
                             CP_AUTH_STATUS_FLAG_t * flag)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !flag)
    return rc;

  SEMA_TAKE(READ);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    *flag = pEntry->desc.flag;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);   

  return rc;
}


/*** SETTERS ***/

/*********************************************************************
*
* @purpose  Set MAC address for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr   @b{(input)} IP addr
* @param    L7_enetMacAddr_t *mac  @b{(input)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMacAddrSet(L7_IP_ADDR_t ipAddr, L7_enetMacAddr_t * mac)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !mac)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    memcpy(&(pEntry->desc.macAddr), mac, sizeof(L7_enetMacAddr_t));
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set client connection port for existing IP address
*           with authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32      port    @b{(output)} client port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusPortSet(L7_IP_ADDR_t ipAddr, L7_uint32 port)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->desc.port = port;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set interface ID for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     intfId  @b{(input)} I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusIntfIdSet(L7_IP_ADDR_t ipAddr, L7_uint32 intfId)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->desc.intfId = intfId;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set session timeout for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     timeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusSessionTimeoutSet(L7_IP_ADDR_t ipAddr, L7_uint32 timeout)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->limits.sessionTimeout = timeout;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set idle timeout for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     timeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusIdleTimeoutSet(L7_IP_ADDR_t ipAddr, L7_uint32 timeout)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->limits.idleTimeout = timeout;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set max bandwidth up rate for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxBandwidthUp @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxBandwidthUpSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxBandwidthUp)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
  {
    return rc;
  }

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    pEntry->limits.maxBandwidthUp = maxBandwidthUp;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set max bandwidth down rate for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxBandwidthDown @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxBandwidthDownSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxBandwidthDown)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
  {
    return rc;
  }

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    pEntry->limits.maxBandwidthDown = maxBandwidthDown;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set max input octets for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxInputOctets @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxInputOctetsSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxInputOctets)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
  {
    return rc;
  }

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    pEntry->limits.maxInputOctets = maxInputOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set max output octets for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxOutputOctets @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxOutputOctetsSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxOutputOctets)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
  {
    return rc;
  }

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    pEntry->limits.maxOutputOctets = maxOutputOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set max total octets for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     maxTotalOctets @b{(input)} in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusMaxTotalOctetsSet(L7_IP_ADDR_t ipAddr, L7_uint32 maxTotalOctets)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
  {
    return rc;
  }

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }
    pEntry->limits.maxTotalOctets = maxTotalOctets;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set connection time for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    time_t   *  connTime  @b{(input)} connection time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusConnTimeSet(L7_IP_ADDR_t ipAddr, time_t * connTime)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !connTime)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    memcpy(&(pEntry->desc.connTime), connTime, sizeof(time_t));
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/*********************************************************************
*
* @purpose  Set uid for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr @b{(input)} IP addr
* @param    L7_uchar8  *  uid    @b{(input)} user ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusUIDSet(L7_IP_ADDR_t ipAddr, L7_uchar8 * uid)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !uid)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    osapiStrncpySafe(pEntry->desc.uid, uid, CP_USER_LOCAL_USERNAME_MAX+1);
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}


/*********************************************************************
*
* @purpose  Set password for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr @b{(input)} IP addr
* @param    L7_uchar8  *  pwd    @b{(input)} password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusPwdSet(L7_IP_ADDR_t ipAddr, L7_uchar8 * pwd)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady || !pwd)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    osapiStrncpySafe(pEntry->desc.pwd, pwd, L7_PASSWORD_SIZE);
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}


/*********************************************************************
*
* @purpose  Set authorization flag for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t          ipAddr @b{(input)} IP addr
* @param    CP_AUTH_STATUS_FLAG_t flag   @b{(input)} auth flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpcmAIPStatusFlagSet(L7_IP_ADDR_t ipAddr, 
                             CP_AUTH_STATUS_FLAG_t flag)
{
  cpcmAuthInProgressStatusData_t *pEntry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (!aipStatusTreeReady)
    return rc;

  SEMA_TAKE(WRITE);   
  do {
    pEntry = cpcmFindAIPStatus(ipAddr);
    if (L7_NULLPTR == pEntry)
      break;
    pEntry->desc.flag = flag;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);   

  return rc;
}

/************************************************************************ 
 *
 *    Work queue functions begin here
 *
 ************************************************************************/


L7_RC_t   authWOQInit(L7_uint32 maxOrders)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do
  {
    L7_uint32 size = sizeof(authWorkOrder_t) * (maxOrders + 1);
    authWOQ.pAll = (authWorkOrder_t *)
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, size);

    if (L7_NULLPTR == authWOQ.pAll)
      break;
    memset(authWOQ.pAll, 0, size);
    authWOQ.front = authWOQ.back = 0;
    authWOQ.size = maxOrders;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


void   authWOQFinalize(void)
{
  SEMA_TAKE(WRITE);
  memset(authWOQ.pAll, 0, authWOQ.size);
  authWOQ.front = authWOQ.back = 0;
  SEMA_GIVE(WRITE);
}

L7_RC_t authWOGet(L7_IP_ADDR_t * ipAddr, 
                  L7_enetMacAddr_t * mac,
                  L7_uint32 * intIfNum)
                  
{
  L7_RC_t rc = L7_FAILURE;

  if (!ipAddr || !mac || !intIfNum)
  {
    return L7_ERROR;
  }

  SEMA_TAKE(WRITE);
  do 
  { 
    if (L7_NULLPTR == authWOQ.pAll)
      break;

    if (authWOQ.front == authWOQ.back)
      break;

    *ipAddr   = authWOQ.pAll[authWOQ.front].ipAddr;
    memcpy(mac, &(authWOQ.pAll[authWOQ.front].mac),
           sizeof(L7_enetMacAddr_t));
    *intIfNum = authWOQ.pAll[authWOQ.front].intIfNum;

    authWOQ.front = (authWOQ.front + 1) % authWOQ.size;
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(WRITE);
  return rc;
}

L7_RC_t authWOAdd(L7_IP_ADDR_t ipAddr, 
                  L7_enetMacAddr_t * mac,
                  L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  if (!mac || ((L7_IP_ADDR_t) 0 == ipAddr))
  {
    return rc;
  }

  SEMA_TAKE(WRITE);
  do 
  {
    if (L7_NULLPTR == authWOQ.pAll)
      break;

    if (authWOQ.front == (authWOQ.back + 1) % authWOQ.size)
      break;  /* queue full */

    authWOQ.pAll[authWOQ.back].ipAddr = ipAddr;
    memcpy(&(authWOQ.pAll[authWOQ.back].mac), mac,
           sizeof(L7_enetMacAddr_t));
    authWOQ.pAll[authWOQ.back].intIfNum = intIfNum;

    authWOQ.back = (authWOQ.back + 1) % authWOQ.size;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}
