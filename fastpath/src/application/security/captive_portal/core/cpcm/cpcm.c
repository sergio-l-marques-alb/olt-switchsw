/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpcm.c
*
* @purpose      Captive Portal Client Credential Manager (CPCM)
*
* @component    CPCM
*
* @comments     none
*
* @create       08/08/2007
*
* @author       darsenault
*
* @end
*
**********************************************************************/
#include <string.h>
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "captive_portal_commdefs.h"
#include "intf_cb_api.h"
#include "captive_portal_defaultconfig.h"
#include "captive_portal_common.h"
#include "cpdm_connstatus_api.h"
#include "cpdm_user_api.h"
#include "cp_trap_api.h"
#include "cpdm_api.h"
#include "cpcm_api.h"
#include "cpdm.h"
#include "cli_web_user_mgmt.h"
#include "log.h"
#include "radius_api.h"
#include "cpim_api.h"
#include "usmdb_util_api.h"

extern L7_RC_t ewaCpConnListAnd(L7_inet_addr_t remote);
extern L7_RC_t ewaCpConnListDelete(L7_inet_addr_t remote);

static L7_BOOL fAuthBusy   = L7_FALSE;

/* Statically defined functions */
static
L7_RC_t  cpcmClientConnStatusTimeoutsUpdate(L7_enetMacAddr_t * mac, 
                                           cpId_t cpId,
                                           uId_t uId);

/* Local forward declarations */
L7_RC_t cpcmGuestUserAuth(L7_char8 *loginName);
L7_RC_t cpcmLocalUserAuth(L7_char8 *loginName, L7_char8 *pwd);
L7_RC_t cpcmRADIUSUserAuth(cpId_t cpId, L7_char8 *loginName, L7_char8 *pwd, L7_IP_ADDR_t ipAddr);

/*****************************************************************************
*
* @purpose  This function is called from the common CP task when it receives
*           an authentication request. This function cycles through the aip
*           table and loops until there are no more wip status entries. This
*           prevents any user request from getting lost while we're busy.
*           Upon finding a WIP entry, authentication will be performed and 
*           as a result, the associated tables are updated appropriately.
*           Specifically, if authentication is successful, then the user
*           is deleted from the AIP table and added to the connected client
*           table. If authentication failed for any reason, the AIP status
*           is updated. The AIP entries will eventually get removed by the
*           common CP task for those lingering.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
L7_RC_t cpcmTryAuth(void)
{
  cpdmClientConnStatusData_t newClient;
  CP_VERIFY_MODE_t verifyMode;
  cpcmAuthInProgressStatusDesc_t desc;
  L7_uint32 value = 0, authsProcessed, authsAttempted;
  L7_uchar8 swMac[L7_MAC_ADDR_LEN];
  L7_IP_ADDR_t ipAddr;
  L7_RC_t status = L7_FAILURE;
  cpId_t cpId = 0, gpId = 0;
  uId_t uId = 0;
  time_t now = 0;
  intfCpCallbacks_t *pCBs = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if (L7_TRUE == fAuthBusy)
  {
    CP_DLOG(CPD_LEVEL_LOG,
            "%s: cpcmTryAuth:, busy, leaving",__FUNCTION__);
    return L7_FAILURE;
  }

  /* We used to iterate through the AIP status table only once.  Now
   * we'll keep going until nothing is left, yielding the processor
   * every X iterations in case we're overwhelmed by authentications.
   */

  fAuthBusy = L7_TRUE;
  while (L7_TRUE == fAuthBusy)
  {
    ipAddr = 0;
    authsProcessed = 0;
    for(authsAttempted = 0;
        ((L7_SUCCESS == cpcmAIPStatusNextGet(ipAddr, &ipAddr)) &&
         (L7_SUCCESS == cpcmAIPStatusFlagGet(ipAddr, &desc.flag)));
        authsAttempted++) 
    {
      if ((CPCM_MAX_AUTH_ATTEMPTS_B4_YIELD - 1) == 
	  (authsAttempted % CPCM_MAX_AUTH_ATTEMPTS_B4_YIELD))
      {
        osapiTaskYield(); /* Avoid hogging CPU */
      }
      if ((CP_WIP != desc.flag) && (CP_RADIUS_WIP != desc.flag))
      {
        continue;
      }

      CP_DLOG(CPD_LEVEL_LOG,
              "%s: Client IP 0x%08x retrieving parameters from AIP table.",__FUNCTION__,ipAddr);

      cpcmAIPStatusDescGet(ipAddr,&desc);
      now = time(NULL);
      cpdmGlobalStatusSessionTimeoutGet(&value);
      value += desc.connTime;

      CP_DLOG(CPD_LEVEL_LOG,
              "%s: Checking Client IP 0x%08x auth time, is now[%d] > timeout[%d]?",
              __FUNCTION__,ipAddr,now,value);

      if (now > value)
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x auth timed out: TIMEOUT..",__FUNCTION__,ipAddr);
        cpcmAIPStatusFlagSet(ipAddr,CP_TIMEOUT);
        /* cpdmGlobalActivityLogEntryAdd("Timeout: Client authentication failed; session timed out."); */
        continue;
      }

      if (L7_TRUE==cpdmClientConnStatusConnectFull())
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x no more connections: RESOURCES..",__FUNCTION__,ipAddr);
        cpcmAIPStatusFlagSet(ipAddr,CP_RESOURCE);

        /* if we couldn't add an entry for the client connection, 
         * send a trap.
         */
        cpTrapClientConnectionDatabaseFull();

        /* cpdmGlobalActivityLogEntryAdd("Resource: Authentication failed; exceeded client connection limit."); */
        continue;
      }
      
      if ( (L7_SUCCESS != cpdmCPConfigIntIfNumFind(desc.intfId, &cpId)) ||
           (L7_SUCCESS != cpdmCPConfigVerifyModeGet(cpId,&verifyMode))) 
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(desc.intfId, L7_SYSNAME, ifName);

        CP_DLOG(CPD_LEVEL_LOG, "%s: Can't get verify mode from interface ID (%d, %s)",__FUNCTION__, desc.intfId, ifName);
        continue;
      }

      if (CP_VERIFY_MODE_LOCAL==verifyMode)
      {
        cpdmUserEntryByNameGet(desc.uid,&uId);
        cpdmCPConfigGpIdGet(cpId,&gpId);
        if (L7_SUCCESS!=cpdmUserGroupAssocEntryGet(uId,gpId))
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x auth failed: Not a member of group id %d",
                  __FUNCTION__,ipAddr,gpId);
          cpcmAIPStatusFlagSet(ipAddr,CP_DENIED);
          authsProcessed++;
          continue;
        }
      }

      status = L7_FAILURE;
      if (CP_WIP==desc.flag)
      {
        switch (verifyMode)
        {
          case CP_VERIFY_MODE_LOCAL:
          {
              status = cpcmLocalUserAuth(desc.uid, desc.pwd);
              break;
          }
          case CP_VERIFY_MODE_RADIUS:
          {
              status = cpcmRADIUSUserAuth(cpId, desc.uid, desc.pwd, ipAddr);
              if (L7_SUCCESS == status)
              {
                  /* Prevent multiple auth requests while waiting for RADIUS callback */
                  cpcmAIPStatusFlagSet(ipAddr,CP_WIP_PENDING);
    
                  authsProcessed++;
                  continue;
              }
              else
              {
                  CP_DLOG(CPD_LEVEL_LOG, "%s: unable to post authentication to RADIUS", __FUNCTION__);
                  break;      /* Initial RADIUS post failed. */
              }
          }
          case CP_VERIFY_MODE_GUEST:
          {
              status = cpcmGuestUserAuth(desc.uid);
              break;
          }
          default: 
          {
              CP_DLOG(CPD_LEVEL_LOG, "%s: unknown verify mode (%d)", verifyMode, __FUNCTION__);
              break; /* status will == L7_FAILURE, cause fall-through to "auth DENIED" case below */
          }
        }

        if (L7_SUCCESS == status)
        {
          authsProcessed++;
        }
      }

      if ((L7_SUCCESS==status) || (CP_RADIUS_WIP==desc.flag))
      {
        CP_DLOG(CPD_LEVEL_LOG,
                "%s: Client IP 0x%08x auth success.",__FUNCTION__,ipAddr);
        pCBs = cpimIntfOwnerCallbacksGet(desc.intfId);
        if (pCBs) 
        {
          /*
            If the client is already authenticated, remove from the client connection table
            but don't send a deauth to the AP.
          */
          if (L7_SUCCESS==cpdmClientConnStatusGet(&desc.macAddr))
          {
            rc = cpdmClientConnStatusDeleteNoDeauth(&desc.macAddr);
          }
          else
          {
            rc = L7_SUCCESS;
          }

          if ((L7_SUCCESS==rc) &&
              (L7_SUCCESS==cpdmClientConnStatusAdd(&desc.macAddr,desc.uid,desc.intfId,ipAddr)))
          {
            /* Update the new client connection entry */
            memset(&newClient,0,sizeof(cpdmClientConnStatusData_t));
            simGetSystemIPBurnedInMac(swMac);
            memcpy(&(newClient.switchMacAddr), swMac, sizeof(L7_enetMacAddr_t));
            newClient.switchIp = simGetSystemIPAddr();
            newClient.whichSwitch = CP_AUTH_ON_LOCAL_SWITCH;
            newClient.verifyMode = verifyMode;
            newClient.uId = uId;
            cpdmCPConfigProtocolModeGet(cpId,&newClient.protocolMode);
            cpcmAIPStatusLimitsGet(ipAddr, &newClient.limits);
            cpdmClientConnStatusUpdate(&desc.macAddr,&newClient);

            if (memcmp(&cpdmOprData->cpClientAuthInfo.macAddr, &desc.macAddr,
                       sizeof(L7_enetMacAddr_t)) == 0)
            {
              if (cpdmClientConnStatusRoamUpdate(&desc.macAddr) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to update status of client %02x:%02x:%02x:%02x:%02x:%02x.",
                        __FUNCTION__,desc.macAddr.addr[0], desc.macAddr.addr[1], 
                        desc.macAddr.addr[2], desc.macAddr.addr[3], 
                        desc.macAddr.addr[4], desc.macAddr.addr[5]);
              }
            }

            if (CP_VERIFY_MODE_LOCAL == verifyMode)
            {
              /* Just in case the CP ID or userID timeouts (for session or
               * idle time) have changed between the start and finish of
               * authorization, we run a last check.
               */
              cpcmClientConnStatusTimeoutsUpdate(&desc.macAddr, cpId, uId);
            }

            CP_DLOG(CPD_LEVEL_LOG,
                    "%s: Client IP 0x%08x added entry to client connection table.",
                    __FUNCTION__,ipAddr);

            /* Send AUTH message ASAP */
            pCBs->cpAuthenUnauthenClient(CP_CLIENT_AUTH, desc.macAddr, 
                                         newClient.limits.maxBandwidthUp, 
                                         newClient.limits.maxBandwidthDown);
            CP_DLOG(CPD_LEVEL_LOG,
                    "%s: sent CP_CLIENT_AUTH message for MAC %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x, up:%u down:%u",
                    __FUNCTION__,
                    desc.macAddr.addr[0], desc.macAddr.addr[1], desc.macAddr.addr[2],
                    desc.macAddr.addr[3], desc.macAddr.addr[4], desc.macAddr.addr[5],
                    newClient.limits.maxBandwidthUp, newClient.limits.maxBandwidthDown);

            /* Remove from AIP */
            cpcmAIPStatusDelete(ipAddr);
            CP_DLOG(CPD_LEVEL_LOG,
                    "%s: Client IP 0x%08x removed from AIP table.",__FUNCTION__,ipAddr);

            cpTrapClientConnected(&desc.macAddr, ipAddr, &(newClient.switchMacAddr), cpId, desc.intfId);
            /* TBD - retrieve both ip/mac sysapiClusterMemberIdentityGet(swIpAddr,swMacAddr); */
            
            authsProcessed++;
          }
          else
          {
            CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x failed to add entry to client connection table.",
                    __FUNCTION__,ipAddr);
          }
        }
        else
        {
          CP_DLOG(CPD_LEVEL_LOG, "%s: unable to send cpAuthenUnauthenClient message", __FUNCTION__);
        }
        continue;
      }
      else
      {
        CP_DLOG(CPD_LEVEL_LOG, "%s: Client IP 0x%08x auth failed: DENIED..",__FUNCTION__,ipAddr);
        /* Add or update client authentication failure */
        cpcmAIPStatusFlagSet(ipAddr, CP_DENIED);
        authsProcessed++;
        continue;
      }
    }   /* end inner while */

    if (0 == authsProcessed)
    {
      break; /* nothing processed, so no more work to do right now */
    }

  }     /* end outer while */
  fAuthBusy = L7_FALSE;
  return L7_SUCCESS;
}

/*****************************************************************************
*
* @purpose  This function authenticates guest user. Authentication isn't
*           necessary per say, but the user must enter a valid loginName if
*           its required (but this was already verified by the UI).
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
L7_RC_t cpcmGuestUserAuth(L7_char8 *loginName)
{
  return L7_SUCCESS;
}

/*****************************************************************************
*
* @purpose  This function authenticates a local user. Retrieve the db password
*           in its encrypted form for the authentication function.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
L7_RC_t cpcmLocalUserAuth(L7_char8 *loginName, L7_char8 *pwd)
{
  L7_char8 dbPwd[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_RC_t status = L7_FAILURE;
  uId_t uId;

  if ((L7_NULLPTR == loginName) || (L7_NULLPTR == pwd))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad parms", __FUNCTION__);
    return status;
  }
  memset(dbPwd,0,sizeof(dbPwd));
  status = cpdmUserEntryByNameGet(loginName,&uId);
  if (L7_SUCCESS == status)
  {
    status = cpdmUserEntryEncryptedPasswordGet(uId,dbPwd);
  }
  if (L7_SUCCESS == status)
  {
    status = cliWebLoginUserPasswordIsValid(dbPwd,pwd,L7_CAPTIVE_PORTAL_PASSWORD_ENCRYPT_ALG);
  }
  CP_DLOG(CPD_LEVEL_LOG,
          "%s: user %s authenticated.",__FUNCTION__,(L7_SUCCESS==status)?"IS":"IS NOT");
          
  return status;
}

/*****************************************************************************
*
* @purpose  This function authenticates a RADIUS user
*
* @param cpId_t cpId         @b{(input)} Captive portal instance
* @param L7_char8 *loginName @b{(input)} client login name
* @param L7_char8 *pwd       @b{(input)} client password
* @param L7_IP_ADDR_t ipAddr @b{(input)} client ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
L7_RC_t cpcmRADIUSUserAuth(cpId_t cpId, L7_char8 *loginName, L7_char8 *pwd, L7_IP_ADDR_t ipAddr)
{
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_char8 serverName[CP_RADIUS_AUTH_SERVER_MAX+1];
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == loginName) || (L7_NULLPTR == pwd))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: Bad parms", __FUNCTION__);
    return rc;
  }

  memset(serverName,0,sizeof(serverName));
  cpdmCPConfigRadiusAuthServerGet(cpId, serverName);

  rc = radiusNamedUserAuthenticate(loginName,
                                   pwd,
                                   L7_NULL,
                                   ipAddr,
                                   L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                   serverName,
                                   addrType,
                                   L7_NULL);
  if (L7_SUCCESS==rc)
  {
    CP_DLOG(CPD_LEVEL_LOG,
            "%s: Requested RADIUS auth for user %s.",
            __FUNCTION__,loginName);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG,
            "%s: Request RADIUS auth failed for server:%s user:%s!",
            __FUNCTION__,serverName,loginName);
  }

  return rc;
}


/*****************************************************************************
*
* @purpose  Update timeouts for a connection entry
*
* @param    L7_enetMacAddr_t * @b{(input)} mac -- connection entry key
* @param    cpId_t @b{(input)} cpId -- cpId of connection
* @param    uId_t @b{(input)} uId -- uId of connection
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Obviously, you only want to use the CP ID and user ID of 
*           the connection corresponding to the given MAC. (The only 
*           reason why we don't enforce this explicitly is for 
*           efficiency's sake -- another good reason to keep this 
*           function static!) 
*           Also, the connection entry in question should have used
*           ONLY local verification.
*
*****************************************************************************/
static
L7_RC_t  cpcmClientConnStatusTimeoutsUpdate(L7_enetMacAddr_t * mac, 
                                           cpId_t cpId,
                                           uId_t uId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 sess_to, idle_to;

  do 
  {
    /* Find what timeouts should be based on uId, cpId */
    if (L7_SUCCESS != cpdmUserEntrySessionTimeoutGet(uId, &sess_to))
      break;
    if (L7_SUCCESS != cpdmUserEntryIdleTimeoutGet(uId, &idle_to))
      break;
    if ((0 == sess_to) &&
        (L7_SUCCESS != cpdmCPConfigSessionTimeoutGet(cpId, &sess_to)))
      break;
    if ((0 == idle_to) &&
        (L7_SUCCESS != cpdmCPConfigIdleTimeoutGet(cpId, &idle_to)))
      break;
  
    if ((L7_SUCCESS != cpdmClientConnStatusIdleTimeoutSet(mac, idle_to)) ||
        (L7_SUCCESS != cpdmClientConnStatusSessionTimeoutSet(mac, sess_to)))
      break;
    rc = L7_SUCCESS;
  } while (0);
  return rc;
}

/*********************************************************************
*
* @purpose  This is a UTILITY / DEBUG function that can be used to
*           authenticate an associated client.
*
* @param    L7_uchar8 *ip @b{(input)} IP address (x.x.x.x) of FIRST client
* @param    L7_uint32 count @b{(input)} number of clients to authenticate
* @param    L7_char8 *uid @b{(input)} User name (or id) of the user
* @param    L7_char8 *pwd @b{(input)} Password (decrypted)
*
* @returns  L7_SUCCESS  Authentication successful
* @returns  L7_FAILURE  Authentication failed
*
* @comments Just to reiterate that the client must already be associated
*           to the proper network
*
* @end
*
*********************************************************************/
L7_RC_t cpAuthenticateUsers(L7_uchar8 *ip, L7_uint32 count,  L7_char8 *uid, L7_char8 *pwd)
{
  cpUserAuth_t user;
  L7_IP_ADDR_t ipAddr;
  L7_inet_addr_t remote;
  CP_AUTH_STATUS_FLAG_t flag;
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 intfId = 0;
  cpId_t cpId = 0;
  L7_RC_t rc = L7_FAILURE;

  cpdmGlobalModeGet(&mode);
  if (L7_ENABLE!=mode)
  {
    sysapiPrintf("%s: Failed; Captive Portal is not enabled!", __FUNCTION__);
    return rc;
  }

  if (!ip || !uid || !pwd)
  {
    sysapiPrintf("%s: Failed; Bad parms\n", __FUNCTION__);
    return rc;
  }

  if (L7_SUCCESS!=usmDbInetAton(ip,&ipAddr))
  {
    sysapiPrintf("%s: Failed; Can't parse IP address\n", __FUNCTION__);
    return rc;
  }

  while (count > 0) 
  {
    if (L7_SUCCESS!=cpcmAIPStatusIntfIdGet(ipAddr,&intfId))
    {
      sysapiPrintf("%s: Failed: IP 0x%x has no valid WIP entry\n",__FUNCTION__,ipAddr);
    }
    else if (L7_SUCCESS != cpdmCPConfigIntIfNumFind(intfId,&cpId))
    {
      sysapiPrintf("%s: Failed: Invalid CP assoc for intf Id %d\n",__FUNCTION__,intfId);
    } 
    else
    {
      memset(&remote,0,sizeof(remote));
      remote.addr.ipv4.s_addr = ipAddr;
      if (L7_SUCCESS!=ewaCpConnListAnd(remote))
      {
        sysapiPrintf("%s: Failed; No more web connections!\n",__FUNCTION__);
        return rc;
      }

      user.flag = CP_VALIDATE;
      user.ipAddr = ipAddr;
      osapiStrncpy(user.uid,uid,L7_PASSWORD_SIZE);
      osapiStrncpy(user.pwd,pwd,L7_PASSWORD_SIZE);

      rc = cpcmUserAuthRequest(&user,&flag);
      sysapiPrintf("auth of IP 0x%08x %s\n",user.ipAddr,
                   (L7_SUCCESS == rc) ? "worked" : "FAILED");
      ewaCpConnListDelete(remote);
    }
      /* skip over addresses x.x.x.0 and x.x.x.255 */
    ipAddr++;
    switch (ipAddr & 0xff) {
      case 255:
        ipAddr++; /* no break */
      case 0:
        ipAddr++;
        break;
      default:    /* shut up, coverity */
        break;
    }

    count--;
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  This is a UTILITY / DEBUG function that can be used to
*           authenticate an associated client.
*
* @param    L7_uchar8 *ip @b{(input)} IP address (x.x.x.x) of the client
* @param    L7_char8 *uid @b{(input)} User name (or id) of the user
* @param    L7_char8 *pwd @b{(input)} Password (decrypted)
*
* @returns  L7_SUCCESS  Authentication successful
* @returns  L7_FAILURE  Authentication failed
*
* @comments Just to reiterate that the client must already be associated
*           to the proper network
*
* @end
*
*********************************************************************/
L7_RC_t cpAuthenticateUser(L7_uchar8 *ip, L7_char8 *uid, L7_char8 *pwd)
{
  return cpAuthenticateUsers(ip, 1, uid, pwd);
}

/*********************************************************************
*
* @purpose  This is a UTILITY / DEBUG function that can be used to
*           display the contents of the AIP status table entries
*
* @end
*
*********************************************************************/
L7_RC_t cpDebugAIPTableShow(void)
{
  CP_AUTH_STATUS_FLAG_t flag = CP_DEFAULT;
  L7_char8 ipStr[16];
  L7_enetMacAddr_t macAddr;
  L7_IP_ADDR_t ipAddr = 0;
  L7_uint32 intIfNum = 0;
  L7_uint32 u = 0;
  L7_uint32 s = 0;
  L7_uint32 p = 0;
  time_t now = 0;
  time_t connTime = 0;
  L7_uint32 value = 0;
  L7_uint32 idx = 0;
  L7_RC_t rc = L7_FAILURE;

  sysapiPrintf("\nCP AIP Status Table Entries");
  sysapiPrintf("\n---------------------------");
  while ((L7_SUCCESS==cpcmAIPStatusNextGet(ipAddr,&ipAddr)) &&
   (L7_SUCCESS==cpcmAIPStatusFlagGet(ipAddr,&flag)))
  {
    memset(ipStr,0,sizeof(ipStr));
    rc = usmDbInetNtoa(ipAddr,ipStr);
    if (L7_SUCCESS==rc)
    {
      rc = cpcmAIPStatusMacAddrGet(ipAddr,&macAddr);
    }
    if (L7_SUCCESS==rc)
    {
      rc = cpcmAIPStatusIntfIdGet(ipAddr,&intIfNum);
    }
    if (L7_SUCCESS==rc)
    {
      rc = usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    }
    if (L7_SUCCESS==rc)
    {
      rc = cpcmAIPStatusConnTimeGet(ipAddr,&connTime);
    }
    if (L7_SUCCESS==rc)
    {   
      now = time(NULL);
      rc = cpdmGlobalStatusSessionTimeoutGet(&value);
      value += connTime;
    }
    if (L7_SUCCESS==rc)
    {   
      idx++;

#ifdef L7_STACKING_PACKAGE
      sysapiPrintf("\n(%d) ip:%s mac=%02x:%02x:%02x:%02x:%02x:%02x intf=%u/%u/%u secs remain=%d ",
                   idx,
                   ipStr,
                   macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],macAddr.addr[5],
                   u,s,p,
                   (now-value));
#else
      sysapiPrintf("\n(%d) ip:%s mac=%02x:%02x:%02x:%02x:%02x:%02x intf=%u/%u secs remain=%d ",
                   idx,
                   ipStr,
                   macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],macAddr.addr[5],
                   s,p,
                   (now-value));
#endif
    }
    else
    {
      sysapiPrintf("\nFailed to retrieve AIP entries");
      break;
    }
  }
  return rc;
}

#ifdef L7_CPCM_TEST

extern void cpcmTestNoop(void);

/*****************************************************************************
*
* @purpose  A dummy function used to link cpcmTest* functions for devshell.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*****************************************************************************/
void cpcmTestdummyFunc(void)
{
  cpcmTestNoop();
}

#endif  /* L7_CPCM_TEST */
