/**********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename     cpdm_connstatus_util.c
 *
 * @purpose      Captive Portal Session Monitor (CPSM) utility funcs
 *
 * @component    CPSM
 *
 * @comments     none
 *
 * @create       8/21/2007
 *
 * @authors      dcaugherty
 *
 * @end
 *
 **********************************************************************/

#include <time.h>
#include "commdefs.h"
#include "datatypes.h"
#include "sysapi.h"
#include "default_cnfgr.h"
#include "l7_cnfgr_api.h"
#include "intf_cb_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_common.h"
#include "cpdm.h"
#include "cpdm_api.h"
#include "cpdm_user_api.h"
#include "cpdm_connstatus_api.h"
#include "cpim_api.h"

#include "cpdm_connstatus_util.h"


/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);  
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);  

/******************************************************
 * 
 *       Definitions of file-specific types
 * 
 ******************************************************/

/* Function type for checking the validity of a connection.
 * A return value of L7_TRUE implies that the connection 
 * passes the test in question, and would still be valid.
 */
typedef L7_BOOL (*check_fn)(L7_enetMacAddr_t * mac);

/* Enum corresponding to the known checks we'll need to 
 * perform.  Right now we only support session time checks
 * and a very limited form of idle time checking.
 */

typedef enum {
  FIRST_CHECK = 0,          /* do not change */
  SESSION_TIME = FIRST_CHECK, 
     /* first value, must always == FIRST_CHECK */
  IDLE_TIME,
  USER_LOGOUT,              /* User wants out */
  MAX_BYTES,
  NUM_CHECKS                /* must always be last */
} check_t;


/* Used by our functions for updating connection timeouts.
 * Part of a means for avoiding code-cloning (aka pure evil).
 */

typedef enum {
  UPDT_IDLE = 0,
  UPDT_SESSION,
  UPDT_MAX_INPUT_OCTETS,
  UPDT_MAX_OUTPUT_OCTETS,
  UPDT_MAX_TOTAL_OCTETS
} updt_t;



/* Queue of new connections by MAC for clustering support.
 * (Blatantly cloned from cpcm_api.c, but made generic.)
 * TODO: Move this type to a common system header file for
 * all to share and enjoy.
 */

typedef struct q_s
{
  L7_uchar8 * pAll;
  L7_uint32 front, back, size, eltSize;
} q_t;



static L7_RC_t qInit(q_t * pNewQ, 
		     L7_COMPONENT_IDS_t id,
		     L7_uint32 maxElts, L7_uint32 eltSize);
static void    qFinalize(q_t * pQ);
static L7_RC_t qTop(q_t * pQ, void * pEltCopy);
static void    qPop(q_t * pQ);
static L7_RC_t qAdd(q_t * pQ, L7_uchar8 * pElt);



/* Begin type-specific queue types */


static q_t transQ      = { L7_NULLPTR, 0, 0, 0 };


L7_RC_t    connTransQInit(L7_uint32 maxNewConns);
void       connTransQFinalize(void);
L7_RC_t    connTransQTop(cpConnTransaction_t * pCopy);
void       connTransQPop(void);

L7_RC_t    newConnAdd(L7_enetMacAddr_t * pMac);
L7_RC_t    deleteConnAdd(L7_enetMacAddr_t * pMac);
L7_RC_t    deauthConnAdd(L7_enetMacAddr_t * pPeerMac, L7_enetMacAddr_t * pClientMac);





/******************************************************
 * 
 * Declaration of module-specific (static) functions.
 * 
 ******************************************************/

/*
 *  Check functions  
 */ 

static L7_BOOL      session_time_check(L7_enetMacAddr_t * mac);
static L7_BOOL      user_logout_check(L7_enetMacAddr_t * mac);
static L7_BOOL      idle_time_check(L7_enetMacAddr_t * mac);
static L7_BOOL      max_bytes_check(L7_enetMacAddr_t * mac);

/* 
 *  The one function that tries all check functions for
 *  a given MAC address
 */
static L7_BOOL      session_still_ok(L7_enetMacAddr_t * mac);


/* Used for timeout value updates for existing connections */

static
L7_RC_t 
cpdmClientConnStatusByCpIdUpdate(cpId_t cpId,
                                        L7_uint32 newVal,
                                        updt_t updt);

static
L7_RC_t 
cpdmClientConnStatusByUIdUpdate(uId_t uId,
                                       L7_uint32 newVal,
                                       updt_t updt);

/******************************************************
 * 
 *             Declaration of static data 
 * 
 ******************************************************/

/* The array indices of the elements here match the 
 * enum above -- what a coincidence!
 */

static check_fn checkFnTable[NUM_CHECKS] = {
  session_time_check,   /* SESSION_TIME     */
  idle_time_check,      /* IDLE_TIME        */
  user_logout_check,    /* USER_LOGOUT      */
  max_bytes_check       /* MAX_BYTES        */
};

/*********************************************************************
*
* @purpose  to monitor all client connections for given resource
*           constraints (e.g. session time limits, transmit/receive)
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments For every entry in the clientConnectionStatus table
*           check the session status and deauthenticate and destroy
*           session if necessary.
*
* @notes    This client checks are only performed if this local switch
*           is the authenticator.
*
* @end
*
*********************************************************************/
void  cpdmMonitorSessions_private(void)
{
  cpdmAuthWhichSwitch_t swType;
  L7_enetMacAddr_t mac;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t loop_rc = L7_SUCCESS;

  memset(&mac,0,sizeof(L7_enetMacAddr_t));
  loop_rc = cpdmClientConnStatusNextGet(&mac, &mac);

  while(L7_SUCCESS == loop_rc) 
  {
    /* Perform checks only if this switch is the authenticator */
    cpdmClientConnStatusWhichSwitchGet(&mac, &swType);
    if (CP_AUTH_ON_LOCAL_SWITCH == swType)
    {
      if (!session_still_ok(&mac))
      {
        rc = cpdmClientConnStatusDelete(&mac);
        if (L7_SUCCESS != rc)
        {
          LOG_MSG("Could not deauth client @ MAC "
                  "%02x:%02x:%02x:%02x:%02x:%02x\n",
                  mac.addr[0], mac.addr[1], mac.addr[2], 
                  mac.addr[3], mac.addr[4], mac.addr[5]);
        }
        else
        {
          LOG_MSG("Deauth'ed client @ MAC "
                  "%02x:%02x:%02x:%02x:%02x:%02x\n",
                  mac.addr[0], mac.addr[1], mac.addr[2], 
                  mac.addr[3], mac.addr[4], mac.addr[5]);
        }
      }
    }
    loop_rc = cpdmClientConnStatusNextGet(&mac, &mac);
  }
}


/*********************************************************************
*
* @purpose  To check that one session is still valid
*
* @param    L7_enetMacAddr_t *mac @b{(input)} MAC address of session
*
* @returns  L7_TRUE  if session still passes muster
* @returns  L7_FALSE if session has failed any one check
*
* @comments none
*
* @end
*
*********************************************************************/
L7_BOOL session_still_ok(L7_enetMacAddr_t * mac)
{
  check_t check = FIRST_CHECK;
  L7_BOOL skipCheck;
  L7_uint32 intf;
  L7_uchar8 val;

  cpdmClientConnStatusIntfIdGet(mac, &intf);

  for (check = FIRST_CHECK, val = L7_DISABLE, skipCheck = L7_FALSE;
       check < NUM_CHECKS;
       check++, val = L7_DISABLE, skipCheck = L7_FALSE) 
  {
    switch (check)
    {
      case SESSION_TIME:
      {
        cpimIntfCapabilitySessionTimeoutGet(intf, &val);
        if (val != L7_ENABLE)
        {
          skipCheck = L7_TRUE;
          break;
        }    
        break;
      }
      case IDLE_TIME:
      {
        cpimIntfCapabilityIdleTimeoutGet(intf, &val);
        if (val != L7_ENABLE)
        {
          skipCheck = L7_TRUE;
          break;
        }
        break;
      }
      case USER_LOGOUT:
      {
        skipCheck = L7_FALSE;
      }
      default:
      {
        /*
        TODO
        We need to do a little work here when we add the 'wired' interfaces to captive portal.
        DLINK currently supports 'wired', but its current implementation does not support any
        byte limitation (wired or wireless).
        When the time comes, we could/should pass a flag (bitmask?) into the max_bytes_check
        function based on the interface capabilities.
        - cpimIntfCapabilityBytesReceivedCounterGet
        - cpimIntfCapabilityBytesTransmittedCounterGet
        - cpimIntfCapabilityPacketsReceivedCounterGet
        - cpimIntfCapabilityPacketsTransmittedCounterGet
        */
        break;
      }
    }

    /* Perform checks only if this interface is capable */
    if (L7_TRUE == skipCheck)
    {
      continue;
    }

    if (! checkFnTable[check](mac))
    {
      return L7_FALSE;
    }
  }
  return L7_TRUE;
}


/*********************************************************************
*
* @purpose  To check that session time has not exceed alloted amount
*
* @param    L7_enetMacAddr_t *mac @b{(input)} MAC address of session
*
* @returns  L7_TRUE if session time has not exceeded
* @returns  L7_FALSE if session time has exceeded maximum span
*
* @comments Get the session timeout from the client connection table
*           first. If it's 0, then get the timeout from the CP
*           instance.
*
* @end
*
*********************************************************************/
L7_BOOL session_time_check(L7_enetMacAddr_t * mac)
{
  cpId_t    cpId  = (cpId_t) 0;
  L7_uint32 sessionTime = 0;
  L7_uint32 sessionTO = 0;
  L7_BOOL   rc = L7_FALSE;

  if (L7_NULLPTR == mac)
  {
    return L7_TRUE; /* don't try to deauth this one */
  }

  do 
  {
    /* get session start time for this client */
    if (L7_SUCCESS != cpdmClientConnStatusSessionTimeGet(mac, &sessionTime))
    {
      LOG_MSG("%s: %02x:%02x:%02x:%02x:%02x:%02x : can't get session time; killing it",
              __FUNCTION__,
              mac->addr[0], mac->addr[1], mac->addr[2], 
              mac->addr[3], mac->addr[4], mac->addr[5]);
      break;
    }

    /* get session timeout for this client */
    cpdmClientConnStatusSessionTimeoutGet(mac, &sessionTO); 

    /* Else get CP instance timeout */
    if (0 == sessionTO)
    {
      if (L7_SUCCESS != cpdmClientConnStatusCpIdGet(mac,&cpId))
      {
        LOG_MSG("%s: %02x:%02x:%02x:%02x:%02x:%02x : no CP ID for this MAC",
                __FUNCTION__,
                mac->addr[0], mac->addr[1], mac->addr[2], 
                mac->addr[3], mac->addr[4], mac->addr[5]);
        break;
      }
      else if (L7_SUCCESS != cpdmCPConfigSessionTimeoutGet(cpId, &sessionTO))
      {
        LOG_MSG("%s: %02x:%02x:%02x:%02x:%02x:%02x : no CP instance session timeout",
                __FUNCTION__,
                mac->addr[0], mac->addr[1], mac->addr[2], 
                mac->addr[3], mac->addr[4], mac->addr[5]);
        break;
      }
    }

    if ((0 == sessionTO) || (sessionTime < sessionTO))
    {
      rc = L7_TRUE;
    }
  } while(0);

#if 0
  L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
          "%s: %02x:%02x:%02x:%02x:%02x:%02x : session time (%d) session timeout (%d) %s",
          __FUNCTION__,
          mac->addr[0], mac->addr[1], mac->addr[2], 
          mac->addr[3], mac->addr[4], mac->addr[5], 
          sessionTime, sessionTO,
          (L7_FALSE==rc)?"deauthenticate":"");
#endif

  return rc;
}

/*********************************************************************
*
* @purpose  To check if session idle time exceeds configured maximum
*
* @param    L7_enetMacAddr_t *mac @b{(input)} MAC address of session
*
* @returns  L7_TRUE if idle time has not exceed max threshold
* @returns  L7_FALSE if session has been idle for too long
*
* @comments none
*
* @end
*
*********************************************************************/
L7_BOOL idle_time_check(L7_enetMacAddr_t * mac)
{ 
  cpId_t    cpId  = (cpId_t) 0;
  L7_uint32 idleTO = 0;
  L7_uint32 idleTime = 0;
  txRxCount_t txRxOld;
  txRxCount_t txRxNew;
  L7_BOOL rc = L7_FALSE;

  if (L7_NULLPTR == mac)
  {
    return L7_TRUE; /* don't try to deauth this one */
  }

  /* get idle timeout for this client */
  cpdmClientConnStatusIdleTimeoutGet(mac, &idleTO);

  /* Else get CP instance timeout */
  if (0 == idleTO)
  {
    if (L7_SUCCESS != cpdmClientConnStatusCpIdGet(mac,&cpId))
    {
      LOG_MSG("%s: %02x:%02x:%02x:%02x:%02x:%02x : no CP ID for this MAC",
              __FUNCTION__,
              mac->addr[0], mac->addr[1], mac->addr[2], 
              mac->addr[3], mac->addr[4], mac->addr[5]);
      return L7_FALSE;
    }
    else if (L7_SUCCESS != cpdmCPConfigIdleTimeoutGet(cpId, &idleTO))
    {
      LOG_MSG("%s: %02x:%02x:%02x:%02x:%02x:%02x : "
              "no CP instance idle timeout",
              __FUNCTION__,
              mac->addr[0], mac->addr[1], mac->addr[2], 
              mac->addr[3], mac->addr[4], mac->addr[5]);
      return L7_FALSE;
    }
  }

  /* If we get here, we have real work to do. */
  do 
  {
    /* Get counts of bytes, packets TX'ed and RX'ed from table entry */
    if (L7_SUCCESS != cpdmClientConnStatusStatisticsGet(mac,
                                  &(txRxOld.bytesReceived),
                                  &(txRxOld.bytesTransmitted),
                                  &(txRxOld.packetsReceived),
                                  &(txRxOld.packetsTransmitted)))
    {
#if 0
      LOG_MSG("%s: Could not get connection stats",__FUNCTION__);
#endif
      break;
    }

    /* Get counts from callback */
    if (L7_SUCCESS != cpdmClientConnStatusStatisticsUpdate(mac, &txRxNew, L7_FALSE))
    {
      /* Now if we can't update our statistics, it could be that the AP
       * was powered down.  It may come back up before this session's time
       * time has expired!  So just let the session age out gracefully.
       */
#if 0
      LOG_MSG("%s: Could not get last refresh data, let session age out", __FUNCTION__);
#endif
    }

    if (L7_SUCCESS != cpdmClientConnStatusIdleTimeGet(mac, &idleTime))
    {
      LOG_MSG("%s: %02x:%02x:%02x:%02x:%02x:%02x: can't get idle time",
              __FUNCTION__,
              mac->addr[0], mac->addr[1], mac->addr[2], 
              mac->addr[3], mac->addr[4], mac->addr[5]);
      break;
    }

    if ((0 == idleTO) || (idleTime < idleTO))
    {
      rc = L7_TRUE;
    }
  } while(0);

#if 0
  L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
          "%s: %02x:%02x:%02x:%02x:%02x:%02x : idle time (%d); idle timeout (%d) %s",
          __FUNCTION__,
          mac->addr[0], mac->addr[1], mac->addr[2], 
          mac->addr[3], mac->addr[4], mac->addr[5],
          idleTime, idleTO,
          (L7_FALSE==rc)?"deauthenticate":"");
#endif

  return rc;
}

/*********************************************************************
*
* @purpose  Check if session has been flagged for de-authentication.
*
* @param    L7_enetMacAddr_t *mac @b{(input)} MAC address of session
*
* @returns  L7_TRUE if session has not been flagged for logout
* @returns  L7_FALSE if session is ready for de-authentication
*
* @comments Note that L7_CP_USER_LOGOUT_FLAG_t could contain a pending
*           flag that could be used if we clobber the session before
*           the web server has a chance to server up the user logout
*           success page. In this case, the logout function would set
*           the pending flag and we could toggle it here to logout
*           which would force a loop cycle before the de-authentication.
*
* @end
*
*********************************************************************/
L7_BOOL user_logout_check(L7_enetMacAddr_t * mac)
{
  L7_BOOL rc = L7_FALSE;
  L7_CP_USER_LOGOUT_FLAG_t userLogoutFlag = L7_CP_USER_LOGOUT_CLEAR;

  if (L7_NULLPTR == mac)
  {
    return L7_TRUE; /* don't try to deauth this one */
  }

  do 
  {
    /* get session logout flag for this client */     
    if (L7_SUCCESS != cpdmClientConnStatusUserLogoutFlagGet(mac,&userLogoutFlag))
    {
      LOG_MSG("%s: %02x:%02x:%02x:%02x:%02x:%02x : can't get session user logout flag; killing it",
              __FUNCTION__,
              mac->addr[0], mac->addr[1], mac->addr[2], 
              mac->addr[3], mac->addr[4], mac->addr[5]);
      break;
    }
    rc = (L7_CP_USER_LOGOUT!=userLogoutFlag)?L7_TRUE:L7_FALSE;
  } while(0);

  return rc;
}

/*********************************************************************
*
* @purpose  To check that maximum byte counts have not been exceeded
*
* @param    L7_enetMacAddr_t *mac @b{(input)} MAC address of session
*
* @returns  L7_TRUE if has not exceeded
* @returns  L7_FALSE if has exceeded maximum span
*
* @comments Get the counts from the client connection table first.
*           If it's 0, then get the count from the CP instance.
*
* @end
*
*********************************************************************/
L7_BOOL max_bytes_check(L7_enetMacAddr_t * mac)
{
  txRxCount_t txRx;
  L7_uint32 maxInputOctets = 0;
  L7_uint32 maxOutputOctets = 0;
  L7_uint32 maxTotalOctets = 0;
  L7_BOOL rc = L7_FALSE;

  if (L7_NULLPTR == mac)
  {
    return rc;
  }

  do 
  {
    /* Get counts of bytes, packets TX'ed and RX'ed from table entry */
    if (L7_SUCCESS != cpdmClientConnStatusStatisticsGet(mac,
                                  &(txRx.bytesReceived),
                                  &(txRx.bytesTransmitted),
                                  &(txRx.packetsReceived),
                                  &(txRx.packetsTransmitted)))
    {
#if 0
      LOG_MSG("%s: Could not get connection stats",__FUNCTION__);
#endif
      break;
    }

    cpdmClientConnStatusMaxInputOctetsGet(mac, &maxInputOctets);
    cpdmClientConnStatusMaxOutputOctetsGet(mac, &maxOutputOctets);
    cpdmClientConnStatusMaxTotalOctetsGet(mac, &maxTotalOctets);

    if ((0 != maxInputOctets) && (maxInputOctets < txRx.bytesTransmitted))
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
              "%s: %02x:%02x:%02x:%02x:%02x:%02x : Maximum input bytes exceeded, De-authenticate!",
              __FUNCTION__,
              mac->addr[0], mac->addr[1], mac->addr[2], 
              mac->addr[3], mac->addr[4], mac->addr[5]);
      break;
    }

    if (L7_TRUE != rc)
    {
      if ((0 != maxOutputOctets) && (maxOutputOctets < txRx.bytesReceived))
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                "%s: %02x:%02x:%02x:%02x:%02x:%02x : Maximum output bytes exceeded, De-authenticate!",
                __FUNCTION__,
                mac->addr[0], mac->addr[1], mac->addr[2], 
                mac->addr[3], mac->addr[4], mac->addr[5]);
        break;
      }
    }

    if (L7_TRUE != rc)
    {
      if ((0 != maxTotalOctets) && (maxTotalOctets < (txRx.bytesReceived+txRx.bytesTransmitted)))
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                "%s: %02x:%02x:%02x:%02x:%02x:%02x : Maximum total bytes exceeded, De-authenticate!",
                __FUNCTION__,
                mac->addr[0], mac->addr[1], mac->addr[2], 
                mac->addr[3], mac->addr[4], mac->addr[5]);
        break;
      }
    }

    rc = L7_TRUE;  
  } while(0);

  if (L7_TRUE == rc)
  {
    /* Update counts if client can have more bytes */
    if (L7_SUCCESS != cpdmClientConnStatusStatisticsUpdate(mac, &txRx, L7_FALSE))
    {
      /* Now if we can't update our statistics, it could be that the AP
       * was powered down.  It may come back up before this session's time
       * time has expired!  So just let the session age out gracefully.
       */
#if 0
      LOG_MSG("%s: Could not get last refresh data, let session age out",__FUNCTION__);
#endif
    }
  }

#if 0
  L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
          "%s: %02x:%02x:%02x:%02x:%02x:%02x : byte check %s",
          __FUNCTION__,
          mac->addr[0], mac->addr[1], mac->addr[2], 
          mac->addr[3], mac->addr[4], mac->addr[5],
          (L7_TRUE==rc)?"OK":"Exceeded!");
#endif

  return rc;
}

/*********************************************************************
*
* @purpose  Update all the client connection records for the CP
*           configuration for the given integer parameter based on
*           updt_t.
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how/what to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments CP parameters for locally administrated connections are
*           obtained using the uId. For all others (guest and RADIUS),
*           simply use the given newVal value for updates.
*
* @end
*
*********************************************************************/
static
L7_RC_t 
cpdmClientConnStatusByCpIdUpdate(cpId_t cpId,
                                 L7_uint32 newVal,
                                 updt_t updt)
{
  L7_RC_t rc = L7_FAILURE;
  L7_enetMacAddr_t  mac;
  CP_VERIFY_MODE_t verifyMode;
  uId_t uId;
  cpId_t curr_cpId;
  L7_uint32 newTO = 0;
  L7_RC_t (*user_get)(uId_t, L7_uint32 *);
  L7_RC_t (*cc_set)(L7_enetMacAddr_t *, L7_uint32);

  switch (updt)
  {
    case UPDT_IDLE:
      user_get = &cpdmUserEntryIdleTimeoutGet;
      cc_set = &cpdmClientConnStatusIdleTimeoutSet;
      break;
    case UPDT_SESSION:
      user_get = &cpdmUserEntrySessionTimeoutGet;
      cc_set = &cpdmClientConnStatusSessionTimeoutSet;
      break;
    case UPDT_MAX_INPUT_OCTETS:
      user_get = &cpdmUserEntryMaxInputOctetsGet;
      cc_set = &cpdmClientConnStatusMaxInputOctetsSet;
      break;
    case UPDT_MAX_OUTPUT_OCTETS:
      user_get = &cpdmUserEntryMaxOutputOctetsGet;
      cc_set = &cpdmClientConnStatusMaxOutputOctetsSet;
      break;
    case UPDT_MAX_TOTAL_OCTETS:
      user_get = &cpdmUserEntryMaxTotalOctetsGet;
      cc_set = &cpdmClientConnStatusMaxTotalOctetsSet;
      break;
    default:
      return rc;
      break;    /* Shut up, compiler. */
  }
  memset(&mac, 0, sizeof(L7_enetMacAddr_t));

  rc = L7_SUCCESS; /* might not be anything to change, after all */
  while(L7_SUCCESS == cpdmClientConnStatusNextGet(&mac, &mac))
  {
    rc = cpdmClientConnStatusVerifyModeGet(&mac, &verifyMode);
    if (L7_SUCCESS != rc)
    {
      return L7_FAILURE;
    }
    /* If a local user, get the current timeout using the uId */
    if (CP_VERIFY_MODE_LOCAL == verifyMode)
    {
      rc = cpdmClientConnStatusCpIdGet(&mac, &curr_cpId);
      if (L7_SUCCESS != rc)
      {
        return L7_FAILURE;
      }
      if (cpId != curr_cpId)
      {
        continue; 
      }
      rc = cpdmClientConnStatusUIdGet(&mac, &uId);
      if (L7_SUCCESS != rc)
      {
        return L7_FAILURE;
      }
      if (L7_SUCCESS != user_get(uId, &newTO))
      {
        return L7_FAILURE;
      }
    }
    if (0 == newTO) /* Then use CP ID timeout */
    {
      newTO = newVal;
    }

    rc = cc_set(&mac, newTO);      
  }
  return rc;
}



/*********************************************************************
*
* @purpose  Update the local user in the client connection table
*
* @param    uId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32 @b{(input)} newVal - new timeout value
* @param    updt_t    @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Connections created via Guest or RADIUS authentication will
*           not be affected.
*
* @end
*
*********************************************************************/
static
L7_RC_t 
cpdmClientConnStatusByUIdUpdate(uId_t uId,
                                L7_uint32 newVal,
                                updt_t updt)
{
  L7_RC_t rc = L7_FAILURE;
  L7_enetMacAddr_t  mac;
  CP_VERIFY_MODE_t verifyMode;
  L7_uint32 newTO = 0;
  cpId_t cpId;
  L7_RC_t (*cp_get)(cpId_t, L7_uint32 *);
  L7_RC_t (*cc_set)(L7_enetMacAddr_t *, L7_uint32);

  switch (updt)
  {
    case UPDT_IDLE:
      cp_get = &cpdmCPConfigIdleTimeoutGet;
      cc_set = &cpdmClientConnStatusIdleTimeoutSet;
      break;
    case UPDT_SESSION:
      cp_get = &cpdmCPConfigSessionTimeoutGet;
      cc_set = &cpdmClientConnStatusSessionTimeoutSet;
      break;
    case UPDT_MAX_INPUT_OCTETS:
      cp_get = &cpdmCPConfigMaxInputOctetsGet;
      cc_set = &cpdmClientConnStatusMaxInputOctetsSet;
      break;
    case UPDT_MAX_OUTPUT_OCTETS:
      cp_get = &cpdmCPConfigMaxOutputOctetsGet;
      cc_set = &cpdmClientConnStatusMaxOutputOctetsSet;
      break;
    case UPDT_MAX_TOTAL_OCTETS:
      cp_get = &cpdmCPConfigMaxTotalOctetsGet;
      cc_set = &cpdmClientConnStatusMaxTotalOctetsSet;
      break;
    default:
      return rc;
      break;    /* Shut up, compiler. */
  }

  memset(&mac, 0, sizeof(L7_enetMacAddr_t));

  rc = L7_SUCCESS; /* might not be anything to change, after all */
  while(L7_SUCCESS == cpdmClientConnStatusByUIdFind(uId, &mac, &mac))
  {
    rc = cpdmClientConnStatusVerifyModeGet(&mac, &verifyMode);
    if (L7_SUCCESS != rc)
      return L7_FAILURE;

    if (CP_VERIFY_MODE_LOCAL != verifyMode) /* don't bother */
      continue;

    newTO = newVal;
    if (0 == newTO) /* then use CP ID value */
    { 
      rc = cpdmClientConnStatusCpIdGet(&mac, &cpId);
      if (L7_SUCCESS != rc)
        return L7_FAILURE;
      if (L7_SUCCESS != cp_get(cpId, &newTO))
        return L7_FAILURE;
    }

    rc = cc_set(&mac, newTO);      
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Recalculate idle timeout for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusIdleTimeoutByCpIdUpdate(cpId_t cpId,
                                            L7_uint32 newVal)
{
  return cpdmClientConnStatusByCpIdUpdate(cpId,
                                          newVal,
                                          UPDT_IDLE);
}

/*********************************************************************
*
* @purpose  Recalculate session timeout for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusSessionTimeoutByCpIdUpdate(cpId_t cpId,
                                               L7_uint32 newVal)
{
  return cpdmClientConnStatusByCpIdUpdate(cpId,
                                          newVal,
                                          UPDT_SESSION);
}


/*********************************************************************
*
* @purpose  Recalculate max octets for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxInputOctetsByCpIdUpdate(cpId_t cpId,
                                               L7_uint32 newVal)
{
  return cpdmClientConnStatusByCpIdUpdate(cpId,
                                          newVal,
                                          UPDT_MAX_INPUT_OCTETS);
}

/*********************************************************************
*
* @purpose  Recalculate max octets for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxOutputOctetsByCpIdUpdate(cpId_t cpId,
                                                L7_uint32 newVal)
{
  return cpdmClientConnStatusByCpIdUpdate(cpId,
                                          newVal,
                                          UPDT_MAX_OUTPUT_OCTETS);
}

/*********************************************************************
*
* @purpose  Recalculate max octets for a client connection 
*
* @param    cpId_t     @b{(input)} cpId - CP ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxTotalOctetsByCpIdUpdate(cpId_t cpId,
                                               L7_uint32 newVal)
{
  return cpdmClientConnStatusByCpIdUpdate(cpId,
                                          newVal,
                                          UPDT_MAX_TOTAL_OCTETS);
}


/*********************************************************************
*
* @purpose  Recalculate session timeout for client connections
*           of a particular user
*
* @param    uId_t      @b{(input)} uId - user id
* @param    L7_uint32  @b{(input)} newVal - new timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ONLY updates connections that have verified locally
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusSessionTimeoutByUIdUpdate(uId_t uId,
                                               L7_uint32 newVal)
{
  return cpdmClientConnStatusByUIdUpdate(uId, newVal,
                                                UPDT_SESSION);
}

/*********************************************************************
*
* @purpose  Recalculate idle timeout for client connections of a
*           particular user
*
* @param    L7_uchar8 * @b{(input)} userId - ID of user
* @param    L7_uint32   @b{(input)} newVal - new timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ONLY updates connections that have been verified locally
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusIdleTimeoutByUIdUpdate(uId_t uId,
                                            L7_uint32 newVal)
{
  return cpdmClientConnStatusByUIdUpdate(uId, newVal,
                                                 UPDT_IDLE);
}

/*********************************************************************
*
* @purpose  Recalculate max octets for specific local user 
*
* @param    uId_t      @b{(input)} uId - User ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxInputOctetsByUIdUpdate(uId_t uId,
                                             L7_uint32 newVal)
{
  return cpdmClientConnStatusByUIdUpdate(uId,
                                         newVal,
                                         UPDT_MAX_INPUT_OCTETS);
}

/*********************************************************************
*
* @purpose  Recalculate max octets for specific local user 
*
* @param    uId_t      @b{(input)} uId - User ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxOutputOctetsByUIdUpdate(uId_t uId,
                                               L7_uint32 newVal)
{
  return cpdmClientConnStatusByUIdUpdate(uId,
                                         newVal,
                                         UPDT_MAX_OUTPUT_OCTETS);
}

/*********************************************************************
*
* @purpose  Recalculate max octets for specific local user 
*
* @param    uId_t      @b{(input)} cpId - User ID of connection
* @param    L7_uint32  @b{(input)} newVal - new timeout value
* @param    updt_t     @b{(input)} updt  - how to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Timeouts for locally administrated connections are obtained
*           using the uId. For all others (guest and RADIUS), simply
*           use the given newVal value for updates.
*
* @end
*
*********************************************************************/

L7_RC_t 
cpdmClientConnStatusMaxTotalOctetsByUIdUpdate(uId_t uId,
                                              L7_uint32 newVal)
{
  return cpdmClientConnStatusByUIdUpdate(uId,
                                         newVal,
                                         UPDT_MAX_TOTAL_OCTETS);
}


/**********************************************************************
 *
 *  New queue functions begin here, for clustering support.
 *
 **********************************************************************/

/**********************************************************************
 *
 *  Generic queue functions
 *
 *  NB: These functions DO NOT USE semaphore protection!  It is up
 *  to their callers to provide that.  Failure to do so will have
 *  dire consequences.  Also, every function below should be protected
 *  by a WRITE-protected semaphore, not just one enabled for read
 *  protection.
 *
 **********************************************************************/

/*********************************************************************
*
* @purpose  Initialize queue
*
* @param    q_t * @b{(output)} existing queue to initialize
* @param    L7_uint32 @b{(input)} max new connections to enqueue
* @param    L7_uint32 @b{(input)} size of individual q element in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*
*            Returns L7_FAILURE if buffer space cannot be allocated.           
*
* @end
*
*********************************************************************/
static 
L7_RC_t qInit(q_t * pNewQ, 
	      L7_COMPONENT_IDS_t id,
	      L7_uint32 maxElts, L7_uint32 eltSize)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 size = eltSize * (maxElts + 1);
  
  do
  {
    if ((L7_NULLPTR == pNewQ) || (0 == size) || (0 == maxElts))
      break; /* reject stupidity */

    pNewQ->pAll = (L7_uchar8 *) osapiMalloc(id, size);

    if (L7_NULLPTR == pNewQ->pAll)
      break;
    memset(pNewQ->pAll, 0, size);
    pNewQ->front = pNewQ->back = 0;
    pNewQ->size = maxElts;
    pNewQ->eltSize = eltSize;
    rc = L7_SUCCESS;
  } while(0);
  return rc;
}


/*********************************************************************
*
* @purpose  Finalizes queue - wipes out entries, prepares for reuse.
*
* @param    q_t * @b{(output)} existing queue to finalize
*
* @returns  void
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!  Allocated memory is retained for later
*            use, it is NOT freed.
*
* @end
*
*********************************************************************/
void       qFinalize(q_t * pQ)
{  
  if (L7_NULLPTR == pQ)
    return;
  memset(pQ->pAll, 0, pQ->eltSize * (pQ->size + 1));
  pQ->front = pQ->back = 0;
}



/*********************************************************************
*
* @purpose  Peeks at top element in queue - if it exists.
*
* @param    q_t * @b{(input)} existing queue to inspect
* @param    void * @b{(output)} pointer to buffer to hold element copy
*
* @returns  void * (pointer to input buffer pointer, or NULL if either
*                   queue is empty or parameters are bad)
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*
* @end
*
*********************************************************************/
static 
L7_RC_t  qTop(q_t * pQ, void * pEltCopy)
{
  L7_RC_t rc = L7_FAILURE;

  
  if ((L7_NULLPTR == pQ) || (L7_NULLPTR == pEltCopy))
  {
    return L7_ERROR;
  }

  do
  {
    if (L7_NULLPTR == pQ->pAll)
      break;

    if (pQ->front == pQ->back) /* empty! */
      break;

    memcpy(pEltCopy, &(pQ->pAll[pQ->front * pQ->eltSize]),
           pQ->eltSize);

    rc = L7_SUCCESS;
  } while(0);

  return rc;
}


/*********************************************************************
*
* @purpose  Pops top element in queue, if it exists.
*
* @param    q_t * @b{(input)} existing queue to inspect
*
* @returns  void - we don't care if the queue is empty
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!  
*
* @end
*
*********************************************************************/
static 
void   qPop(q_t * pQ)
{
  if ((L7_NULLPTR != pQ) && 
      (L7_NULLPTR != pQ->pAll) &&
      (pQ->front != pQ->back)) /* and NOT empty */
  {
    pQ->front = (pQ->front + 1) % pQ->size;
  }
}



/*********************************************************************
*
* @purpose  add new element to queue
*
* @param    q_t * @b{(output)} existing queue to finalize
* @param    L7_uchar8 *  @b{(input)} pointer to new element 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*            
*            Returns L7_SUCCESS if buffer allocated, and space 
*            exists on queue to add new element.
* @end
*
*********************************************************************/
static L7_RC_t qAdd(q_t * pQ, L7_uchar8 * pElt)
{
  L7_RC_t rc = L7_FAILURE;

  
  if ((L7_NULLPTR == pQ) || (L7_NULLPTR == pElt))
  {
    return L7_ERROR;
  }

  do
  {
    if (L7_NULLPTR == pQ->pAll)
      break;

    if (pQ->front == (pQ->back + 1) % pQ->size)
      break;  /* queue full */

    memcpy(&(pQ->pAll[pQ->back * pQ->eltSize]), pElt,
           pQ->eltSize);
    pQ->back = (pQ->back + 1) % pQ->size;
    rc = L7_SUCCESS;
  } while(0);
  return rc;
}





/*********************************************************************
*
* @purpose  Initialize our connections transaction queue
*
* @param    L7_uint32 @b{(input)} max new transactions to enqueue
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!  
*
*            Returns L7_FAILURE if buffer space cannot be allocated.           
*
* @end
*
*********************************************************************/
L7_RC_t    connTransQInit(L7_uint32 maxNewTrans)
{
  return qInit(&transQ, L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
               maxNewTrans,  sizeof(cpConnTransaction_t));
}


/*********************************************************************
*
* @purpose  Finalizes queue - wipes out entries, prepares for reuse.
*
* @param    none
*
* @returns  void
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!  Allocated memory is retained for later
*            use, it is NOT freed.
*
* @end
*
*********************************************************************/
void       connTransQFinalize(void)
{  
  qFinalize(&transQ);
}

/*********************************************************************
*
* @purpose  Get top transaction in queue
*
* @param    cpConnTransaction_t * @b{(output)} copy of next transaction
*
* @returns  L7_SUCCESS if transaction exists
*           L7_FAILURE if no transaction available
*           L7_ERROR if input parameter is faulty
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function! 
*
* @end
*
*********************************************************************/
L7_RC_t    connTransQTop(cpConnTransaction_t * pCopy)
{  
  return qTop(&transQ, (void *) pCopy);
}

/*********************************************************************
*
* @purpose  Pop next transaction off queue
*
* @param    none
*
* @returns  void
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!  
*
* @end
*
*********************************************************************/
void    connTransQPop(void)
{  
  qPop(&transQ);
}


/*********************************************************************
*
* @purpose  add new connection information to queue
*
* @param    L7_enetMacAddr_t * @b{(input)} pMac - new connection
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*            
*            Returns L7_SUCCESS if buffer allocated, and space 
*            exists on queue to add new connection information.
* @end
*
*********************************************************************/
L7_RC_t    newConnAdd(L7_enetMacAddr_t * pMac)
{
  cpConnTransaction_t trans;
  
  if (L7_NULLPTR == pMac)
    return L7_FAILURE;

  trans.trType = CP_CONN_NEW;
  memcpy(&trans.tr.newConn.mac, pMac, sizeof(L7_enetMacAddr_t));
  return qAdd(&transQ, (L7_uchar8 *) &trans);
}


/*********************************************************************
*
* @purpose  add delete connection information to queue
*
* @param    L7_enetMacAddr_t * @b{(input)} pMac - connection to delete
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*            
*            Returns L7_SUCCESS if buffer allocated, and space 
*            exists on queue to add delete connection information.
* @end
*
*********************************************************************/
L7_RC_t    deleteConnAdd(L7_enetMacAddr_t * pMac)
{
  cpConnTransaction_t trans;
  
  if (L7_NULLPTR == pMac)
    return L7_FAILURE;

  trans.trType = CP_CONN_DELETE;
  memcpy(&trans.tr.deleteConn.mac, pMac, sizeof(L7_enetMacAddr_t));
  return qAdd(&transQ, (L7_uchar8 *) &trans);
}


/*********************************************************************
*
* @purpose  add deauth connection information to queue
*
* @param    L7_enetMacAddr_t * @b{(input)} pPeerMac 
* @param    L7_enetMacAddr_t * @b{(input)} pClientMac 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Caller should use WRITE-PROTECTED semaphore with 
*            this function!
*            
*            Returns L7_SUCCESS if buffer allocated, and space 
*            exists on queue to add deauth connection information.
* @end
*
*********************************************************************/
L7_RC_t
deauthConnAdd(L7_enetMacAddr_t * pPeerMac, L7_enetMacAddr_t * pClientMac)
{
  cpConnTransaction_t trans;
  
  if ((L7_NULLPTR == pPeerMac) || (L7_NULLPTR == pClientMac))
    return L7_FAILURE;

  trans.trType = CP_CONN_DEAUTH;
  memcpy(&(trans.tr.deauthConn.peerMac), pPeerMac, sizeof(L7_enetMacAddr_t));
  memcpy(&(trans.tr.deauthConn.clientMac), pClientMac, sizeof(L7_enetMacAddr_t));
  return qAdd(&transQ, (L7_uchar8 *) &trans);
}
