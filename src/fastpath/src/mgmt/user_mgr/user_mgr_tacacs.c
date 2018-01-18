/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_tacacs.c
*
* @purpose   user configuration manager functions for interfacing with TACACS+
*
* @component userMgr component
*
* @comments  none
*
* @create    06/03/05
*
* @author    dfowler
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include "l7_common.h"
#include "user_manager_exports.h"
#include "osapi.h"
#include "tacacs_api.h"
#include "user_mgr_tacacs.h"
#include "user_mgr_radius.h"  /* auth messages */
#include "log.h"
#include "sysapi.h"
#include <string.h>

/* queue and sema are created in userMgrRadiusPhaseOneInit, no need
   to create another queue for TACACS, just use the same one and
   synchronize access with the semaphore, we don't need to handle
   enough simultaneous login requests to require another queue */
extern void *userMgrQueue;
extern void *userMgrSyncSema; /* controls access to the response queue */

/**************************************************************************
* @purpose   Handle TACACS client callbacks
*
* @param     status         @b{(input)} status of RADIUS response (accept, reject, challenge, etc)
* @param     correlator     @b{(input)} correlates responses to requests; for dot1x, this is ifIndex
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
L7_uint32 userMgrTacacsCallback(L7_uint32 status,
                                L7_uint32 correlator,
                                L7_uint32 accessLevel)
{
  userMgrMsg_t msg;

  memset(&msg, 0, sizeof(userMgrMsg_t));
  msg.event = USER_MGR_REMOTE_TACACS_AUTH_RESPONSE;
  msg.correlator = correlator;
  msg.data.userMgrTacacsAuthMsg.status = status;
  msg.data.userMgrTacacsAuthMsg.accessLevel = accessLevel;

  /* send message */
  if (osapiMessageSend(userMgrQueue,
                       &msg,
                       sizeof(userMgrMsg_t),
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "User Mgr Msg Queue is full!!!! Event: %d, correlator: %d\n",
                  USER_MGR_REMOTE_TACACS_AUTH_RESPONSE, correlator);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Interaction with TACACS and response queue
*
* @param   L7_char8     *pUserName @((input))   ptr to user name to authenticate
* @param   L7_char8     *pPwd      @((input))   ptr to user provided password
* @param   userMgrMsg_t *pMsg      @((output))  ptr to response data
*
* @returns L7_SUCCESS  If response received successfully
* @returns L7_FAILURE  If system error
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t userMgrTacacsRequestSend(userMgrAuthRequest_t *request,
                                        userMgrMsg_t *pMsg)
{
  L7_RC_t   rc     = L7_ERROR;
  L7_RC_t   status = L7_FAILURE;

  status = L7_FAILURE;

#ifdef L7_TACACS_PACKAGE
  L7_uint32 correlator = 0;   /* we only send one auth request at a time,
                                 so we can use the one correlator value */

  /* synchronize RADIUS/TACACS access to response queue */
  osapiSemaTake(userMgrSyncSema, L7_WAIT_FOREVER);

  if ( L7_SERVICE_AUTHOR == request->servType)
  {
    status = tacacsUserCmdAuthorize(request, correlator);
  }
  else
  {
    status = tacacsUserAuthenticate(request, correlator);
  }

  if (L7_SUCCESS == status)
  {
    /* Wait on response from TACACS component */
    if ((osapiMessageReceive(userMgrQueue, (void *)pMsg,
                             sizeof(userMgrMsg_t), L7_WAIT_FOREVER) == L7_SUCCESS) &&
        (pMsg->event == USER_MGR_REMOTE_TACACS_AUTH_RESPONSE) &&
        (pMsg->correlator == correlator))
    {
      rc = L7_SUCCESS;
      if ((TACACS_STATUS_AUTH_SUCCESS != pMsg->data.userMgrTacacsAuthMsg.status) &&
          (TACACS_STATUS_AUTH_CHALLENGE != pMsg->data.userMgrTacacsAuthMsg.status))
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_USER_MGR_COMPONENT_ID,
                "Received failure msg id %d from TACACS+ component.", pMsg->data.userMgrTacacsAuthMsg.status);
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_USER_MGR_COMPONENT_ID,
              "Received wrong message from TACACS+ component, event = %d.", pMsg->event);
    }
  }
  else
  {
    pMsg->data.userMgrTacacsAuthMsg.status = status;
  }

  /* allow access to RADIUS/TACACS message queue */
  osapiSemaGive(userMgrSyncSema);
#endif

  return rc;
}

/*********************************************************************
* @purpose  Determine if a user is authenticated by TACACS
*
* @param   L7_char8     *pUserName    @((input))   ptr to user name to authenticate
* @param   L7_char8     *pPwd         @((input))   ptr to user provided password
* @param   L7_uint32    *pAccessLevel @((output))  access level of authenticated user
*
* @returns L7_SUCCESS  If user is authenticated
* @returns L7_FAILURE  If user is not authenticated due to unknown user,
*                      bad password, or system error
* @returns L7_ERROR    If attempt timed out
*
* @notes
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t userMgrAuthenticateUserTacacs(userMgrAuthRequest_t *request)
{
  L7_RC_t rc = L7_ERROR;
  userMgrMsg_t msg;

  request->servType = L7_SERVICE_AUTHEN;

  rc = userMgrTacacsRequestSend(request, &msg);
  if (L7_SUCCESS != rc)
  {
    return L7_ERROR;
  }

  switch (msg.data.userMgrTacacsAuthMsg.status)
  {
    case TACACS_STATUS_AUTH_SUCCESS:
      /* return the access level */
      request->accessLevel = msg.data.userMgrTacacsAuthMsg.accessLevel;
      rc = L7_SUCCESS;
      break;

    case TACACS_STATUS_SERVER_FAILURE:
    case TACACS_STATUS_SESSION_TIMEOUT:
    case TACACS_STATUS_OTHER_FAILURE:
      rc = L7_ERROR;
      break;

    case TACACS_STATUS_AUTH_CHALLENGE: /* Will have challenge flag set */
    case TACACS_STATUS_AUTH_FAILURE:
    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Determine if a user cmd is authorized by TACACS
*
* @param   userMgrAuthRequest_t     *request @((input/output))
*
* @returns L7_SUCCESS  If cmd is authorized
* @returns L7_FAILURE  If cmd is not authorized
* @returns L7_ERROR    If attempt timed out/system error
*
* @notes
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t userMgrCmdAuthorTacacs(userMgrAuthRequest_t *request)
{
  L7_RC_t rc = L7_ERROR;
  userMgrMsg_t msg;
  request->servType = L7_SERVICE_AUTHOR;

  if (userMgrTacacsRequestSend(request, &msg) != L7_SUCCESS)
  {
    if ( msg.data.userMgrTacacsAuthMsg.status == L7_REQUEST_DENIED)
    {
      return L7_FAILURE;
    }
    else
    {
      return L7_ERROR;
    }
  }

  switch (msg.data.userMgrTacacsAuthMsg.status)
  {
    case TACACS_STATUS_AUTH_SUCCESS:
      /* return the access level */
      request->accessLevel = L7_LOGIN_ACCESS_READ_WRITE;
      rc = L7_SUCCESS;
      break;

    case TACACS_STATUS_SERVER_FAILURE:
    case TACACS_STATUS_SESSION_TIMEOUT:
    case TACACS_STATUS_OTHER_FAILURE:
      rc = L7_ERROR;
      break;
    case TACACS_STATUS_AUTH_FAILURE:
    default:
      request->accessLevel = L7_LOGIN_ACCESS_READ_ONLY;
      rc = L7_FAILURE;
      break;
  }

  return rc;
}


/*********************************************************************
* @purpose Initialize the userMgr for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t userMgrTacacsPhaseTwoInit()
{
#ifdef L7_TACACS_PACKAGE
  if (tacacsResponseRegister(L7_USER_MGR_COMPONENT_ID,
                             userMgrTacacsCallback) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USER_MGR_COMPONENT_ID,
            "Unable to register User Manager for TACACS response callback\n");
    return L7_FAILURE;
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Clean up phase 2 resources
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t userMgrTacacsPhaseTwoFini()
{
#ifdef L7_TACACS_PACKAGE
  /* deregister auth callback */
  if (tacacsResponseDeregister(L7_USER_MGR_COMPONENT_ID) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USER_MGR_COMPONENT_ID,
            "Unable to deregister User Manager for TACACS response callback\n");
    return L7_FAILURE;
  }
#endif

  return L7_SUCCESS;
}
