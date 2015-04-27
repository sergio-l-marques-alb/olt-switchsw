
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_radius.c
*
* @purpose   user configuration manager functions for interfacing with RADIUS
*
* @component userMgr component
*
* @comments  none
*
* @create    05/14/03
*
* @author    jflanagan
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#include "l7_common.h"
#include "cli_web_exports.h"
#include "dtlapi.h"
#include "l7_resources.h"
#include "osapi.h"
#include "radius_api.h"
#include "user_mgr_radius.h"
#include "log.h"
#include "sysapi.h"
#include <string.h>

typedef struct L7_radiusAttr_s
{
  L7_uchar8  type;
  L7_uchar8  length;  /* lengh of entire attribute including the type and length fields */
} L7_radiusAttrHeader_t;

void *userMgrQueue;
void *userMgrSyncSema; /* controls access to the RADIUS response queue */

/* Internal function prototypes */
void userMgrRadiusChallengeInfoGet( L7_uchar8 *attributes, L7_uint32 attributesLen,
                                    L7_uchar8 *pState,
                                    L7_BOOL *pChallengeFlag, L7_char8 *pChallengePhrase);

void userMgrRadiusAccessLevelGet( L7_uchar8 *attributes,
                                  L7_uint32 attributesLen,
                                  L7_uint32 *pAccessLevel );

/*********************************************************************
* @purpose  Interaction with Radius and response queue
*
* @param   pUserName (input) ptr to user name to authenticate
* @param   pPwdOrChallengeResponse (input) ptr to user provided password or challenge response
* @param   pState (input/output) output value of previous challenge or new query state
* @param   pMsg (output) ptr to data storage
*
* @returns L7_SUCCESS  If response received successfully
* @returns L7_FAILURE  If system error
*
* @notes The response will be stored in the provided msg buffer
*
* @notes TO DO: On an initial authentication request, state should be set to ????
*
* @end
*********************************************************************/
L7_RC_t
userMgrRadiusRequestSend(L7_char8 *pUserName, L7_char8 *pPwdOrChallengeResponse,
                         L7_uchar8 *pState, userMgrMsg_t *pMsg )
{
  L7_uint32 correlator = 0;
  L7_uint32 rc = L7_SUCCESS;

  /* Take semaphore to prevent other tasks from calling RADIUS */
  /* This will protect access to the response queue so only one task */
  /* will be waiting on a response. */

  osapiSemaTake(userMgrSyncSema, L7_WAIT_FOREVER);
  /* TO DO: Resolve correlator */
     if ( radiusUserAuthenticate(pUserName, pPwdOrChallengeResponse, pState,
                              correlator, L7_USER_MGR_COMPONENT_ID ) == L7_FAILURE ) {
       rc = L7_FAILURE;
  }

  /* Wait on response */
  if ( rc == L7_SUCCESS &&
       osapiMessageReceive(userMgrQueue, (void*)pMsg, sizeof(userMgrMsg_t), L7_WAIT_FOREVER)
       == L7_FAILURE )
    rc = L7_FAILURE;

  /* Release the semaphore to allow other tasks to interface with RADIUS
   * and the message queue.
   */
  osapiSemaGive(userMgrSyncSema);

  return (rc);
}

/**************************************************************************
* @purpose   Process RADIUS Server responses
*
* @param     status         @b{(input)} status of RADIUS response (accept, reject, challenge, etc)
* @param     *attributes    @b{(input)} RADIUS attribute data
* @param     attributesLen  @b{(input)} length of RADIUS attribute data
* @param     pState (input/output) output value of previous challenge or new query state
* @param     pAccessLevel (output) access of authenticated user
* @param     pChallengeFlag (output) indicates if failure caused by challenge (L7_TRUE = challenged)
* @param     pChallengePhrase (output) challenge phrase to display to the user.
*
* @returns   L7_SUCCESS if user authenticated
* @returns   L7_FAILURE if authentication failed (or was challenged)
* @returns   L7_ERROR if timeout
*
* @notes Challenges are not supported.  If a challenge attribute is detected
*        it is ignored and the authentication attempt is considered a failure.
*
* @end
*************************************************************************/
L7_RC_t userMgrRadiusResponseProcess(L7_uint32 status, L7_uchar8 *attributes,
                                     L7_uint32 attributesLen,
                                     L7_uchar8 *pState, L7_uint32 *pAccessLevel,
                                     L7_BOOL *pChallengeFlag, L7_char8 *pChallengePhrase)
{
  L7_RC_t rc = L7_FAILURE;

  /* Initialize challenge flag output parameter */
  *pChallengeFlag = L7_FALSE;

  switch (status)
  {
    case RADIUS_STATUS_SUCCESS:
      /* Get the access level */
      userMgrRadiusAccessLevelGet( attributes, attributesLen, pAccessLevel );
      rc = L7_SUCCESS;
      break;

    case RADIUS_STATUS_CHALLENGED:
      /* Get the challenge phrase and state */
      userMgrRadiusChallengeInfoGet( attributes, attributesLen, pState,
                                     pChallengeFlag, pChallengePhrase );
      rc = L7_FAILURE;
      break;

    case RADIUS_STATUS_AUTHEN_FAILURE:
      rc = L7_FAILURE;
      break;

    case RADIUS_STATUS_COMM_FAILURE:
    case RADIUS_STATUS_REQUEST_TIMED_OUT:
      rc = L7_ERROR;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  /* Need to free the buffer that was passed to us */
  if (attributes != L7_NULLPTR)
    osapiFree(L7_USER_MGR_COMPONENT_ID, attributes);

  return (rc);
}

/**************************************************************************
* @purpose   Parse the challenge phrase and state from the RADIUS response
*
* @param     *attributes    @b{(input)} RADIUS attribute data
* @param     attributesLen  @b{(input)} length of RADIUS attribute data
* @param     pState (input/output) output value of previous challenge or new query state
* @param     pChallengeFlag (output) indicates if failure caused by challenge (L7_TRUE = challenged)
* @param     pChallengePhrase (output) challenge phrase to display to the user.
*
* @returns   void
*
* @comments
*
* @end
*************************************************************************/
void userMgrRadiusChallengeInfoGet( L7_uchar8 *attributes, L7_uint32 attributesLen,
                                    L7_uchar8 *pState,
                                    L7_BOOL *pChallengeFlag, L7_char8 *pChallengePhrase)
{
  L7_radiusAttrHeader_t *radiusAttr;
  L7_BOOL done = L7_FALSE;

  /* Initialize pChallengeFlag to false in case the challenge phrase is not found. */
  *pChallengeFlag = L7_FALSE;
  memset( pChallengePhrase, 0, sizeof(pChallengePhrase));

  /* If no attributes found, exit. */
  if (attributesLen <= sizeof(L7_radiusAttrHeader_t))
    return;

  radiusAttr = (L7_radiusAttrHeader_t *)attributes;

  while (done == L7_FALSE)
  {
    if (radiusAttr->type == RADIUS_ATTR_TYPE_REPLY_MESSAGE) /* Reply-Message contains challenge phrase */
    {
      /* Get the access level returned from RADIUS */
      memcpy(pChallengePhrase, (L7_char8 *)radiusAttr + sizeof(L7_radiusAttrHeader_t),
             radiusAttr->length - sizeof(L7_radiusAttrHeader_t));

      /* Challenge phrase was found.  Set the challenge flag to true.*/
      *pChallengeFlag = L7_TRUE;
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_STATE) /* State must be supplied on challenge response */
    {
      /* Get the access level returned from RADIUS */
      memcpy(pState, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttrHeader_t),
             radiusAttr->length - sizeof(L7_radiusAttrHeader_t));

    }

    attributesLen -= radiusAttr->length;
    if (attributesLen <= sizeof(L7_radiusAttrHeader_t))
    {
      done = L7_TRUE;
    }
    else
      radiusAttr = (L7_radiusAttrHeader_t *)((L7_uchar8 *)radiusAttr + radiusAttr->length);

    if (radiusAttr->length <= 0)
      done = L7_TRUE;
  }

  return;
}

/**************************************************************************
* @purpose   Parse the access level from the radius response
*
* @param     *attributes    @b{(input)} RADIUS attribute data
* @param     attributesLen  @b{(input)} length of RADIUS attribute data
* @param     pAccessLevel (output) access of authenticated user
*
* @returns   void
*
* @comments  The access levels received from RADIUS will be mapped as follows:
*             RADIUS_SERVICE_TYPE_NAS_PROMPT = L7_LOGIN_ACCESS_READ_ONLY
*             RADIUS_SERVICE_TYPE_ADMIN = L7_LOGIN_ACCESS_READ_WRITE
*             other = L7_LOGIN_ACCESS_READ_ONLY
*
* @end
*************************************************************************/
void userMgrRadiusAccessLevelGet( L7_uchar8 *attributes,
                                  L7_uint32 attributesLen,
                                  L7_uint32 *pAccessLevel )
{
  L7_radiusAttrHeader_t *radiusAttr;
  L7_BOOL done = L7_FALSE;
  L7_uint32 radiusAccessLevel;

  /* Initialize the access level in case the attribute is not found. */
  *pAccessLevel = L7_LOGIN_ACCESS_READ_ONLY;

  /* If no attributes received, exit */
  if (attributesLen <= sizeof(L7_radiusAttrHeader_t))
    return;

  radiusAttr = (L7_radiusAttrHeader_t *)attributes;

  while (done == L7_FALSE)
  {
    if (radiusAttr->type == RADIUS_ATTR_TYPE_SERVICE_TYPE) /* Service-Type */
    {
      /* Get the access level returned from RADIUS */
      memcpy((L7_uchar8 *)&radiusAccessLevel, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttrHeader_t),
             radiusAttr->length - sizeof(L7_radiusAttrHeader_t));

      switch (radiusAccessLevel)
      {
        case RADIUS_SERVICE_TYPE_NAS_PROMPT:
          *pAccessLevel = L7_LOGIN_ACCESS_READ_ONLY;
          break;
        case RADIUS_SERVICE_TYPE_ADMIN:
          *pAccessLevel = L7_LOGIN_ACCESS_READ_WRITE;
          break;
        default:
          *pAccessLevel = L7_LOGIN_ACCESS_READ_ONLY;
          break;
      }
    }

    attributesLen -= radiusAttr->length;
    if (attributesLen <= sizeof(L7_radiusAttrHeader_t))
    {
      done = L7_TRUE;
    }
    else
      radiusAttr = (L7_radiusAttrHeader_t *)((L7_uchar8 *)radiusAttr + radiusAttr->length);

    if (radiusAttr->length <= 0)
      done = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Determine if a user is authenticated by RADIUS
*
* @param   pUserName (input) ptr to user name to authenticate
* @param   pPwdOrChallengeResponse (input) ptr to user provided password or challenge response
* @param   pState (input/output) output value of previous challenge or new query state
* @param   pAccessLevel (output) access of authenticated user
* @param   pChallengeFlag (output) indicates if failure caused by challenge (L7_TRUE = challenged)
* @param   pChallengePhrase (output) challenge phrase to display to the user.
*
* @returns L7_SUCCESS  If user is authenticated
* @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
*                      challenge (indicated by challenge flag), or system error
* @returns L7_ERROR  If attempt timed out
*
* @notes
*
* @notes TO DO: On an initial authentication request, state should be set to ????
*
* @end
*********************************************************************/
L7_RC_t
userMgrAuthenticateUserRadius(L7_char8 *pUserName, L7_char8 *pPwdOrChallengeResponse,
                              L7_uchar8 *pState, L7_uint32 *pAccessLevel,
                              L7_BOOL *pChallengeFlag, L7_char8 *pChallengePhrase)
{
  userMgrMsg_t msg;
  L7_RC_t rc = L7_SUCCESS;

  rc =  userMgrRadiusRequestSend(pUserName, pPwdOrChallengeResponse, pState, &msg);
  if(rc == L7_FAILURE || rc == L7_ERROR) {
     return rc;
  }

  return (userMgrRadiusResponseProcess(msg.data.userMgrAuthMsg.status,
                                       msg.data.userMgrAuthMsg.pResponse,
                                       msg.data.userMgrAuthMsg.respLen,
                                       pState, pAccessLevel,
                                       pChallengeFlag, pChallengePhrase));
}

/*********************************************************************
* @purpose  Send a command to the user manager queue
*
* @param    event      @b{(input)} event type
* @param    correlator @b{(input)} identify recipient
* @param    *data      @b{(input)} pointer to data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Command is queued for service
*
* @end
*********************************************************************/
L7_RC_t userMgrWriteToQueue(L7_uint32 event, L7_uint32 correlator, void *data)
{
  userMgrMsg_t msg;
  L7_RC_t rc;

  /* copy event, intIfNum and data ptr to msg struct */
  msg.event = event;
  msg.correlator = correlator;

  memcpy(&msg.data.userMgrAuthMsg, data, sizeof(userMgrAuthMessage_t));

  /* send message */
  rc = osapiMessageSend(userMgrQueue, &msg, sizeof(userMgrMsg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "User Mgr Msg Queue is full!!!! Event: %d, correlator: %d\n", event, correlator);
  }

  return (rc);
}


/**************************************************************************
* @purpose   Handle RADIUS client callbacks
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
L7_RC_t userMgrRadiusCallback(L7_uint32 status, L7_uint32 correlator,
                                L7_uchar8 *attributes, L7_uint32 attributesLen)
{
  userMgrAuthMessage_t message;

  /* To Do: Correlator for user manager is ?? */

  /* Fill in message structure */
  memset((L7_uchar8 *)&message, 0, sizeof(userMgrAuthMessage_t));
  message.status = status;
  message.respLen = attributesLen;

  /* Allocate memory for attributes which gets freed by userMgrRadiusResponseProcess */
  message.pResponse = osapiMalloc(L7_USER_MGR_COMPONENT_ID, attributesLen);
  memcpy(message.pResponse, attributes, attributesLen);

  return (userMgrWriteToQueue(USER_MGR_REMOTE_AUTH_RESPONSE, correlator, &message));
}

/*********************************************************************
* @purpose Initialize the userMgr for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t userMgrRadiusPhaseOneInit()
{
  L7_RC_t rc = L7_SUCCESS;

  do
  {
    userMgrQueue = (void*)osapiMsgQueueCreate("userMgrQueue", USER_MGR_MSG_COUNT, sizeof(userMgrMsg_t));

    if (userMgrQueue == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USER_MGR_COMPONENT_ID,
              "UserMgr: Unable to create msg queue\n");
      rc = (L7_FAILURE);
      break;
    }

    /* semaphore creation for task protection over the shared queue */
    userMgrSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (userMgrSyncSema == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USER_MGR_COMPONENT_ID,
              "Unable to create User Manager semaphore\n");
      rc = (L7_FAILURE);
      break;
    }
  } while ( 0 );

  return(rc);
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
L7_RC_t userMgrRadiusPhaseTwoInit()
{
  L7_RC_t rc = L7_SUCCESS;

  if (radiusResponseRegister(L7_USER_MGR_COMPONENT_ID, userMgrRadiusCallback)==L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USER_MGR_COMPONENT_ID,
            "Unable to register User Manager for RADIUS response callback\n");
    rc = L7_FAILURE;
  }

  return(rc);
}

/*********************************************************************
* @purpose Initialize the userMgr for Phase 1
*
* @param   void
*
* @returns void
*
* @end
*********************************************************************/
void userMgrRadiusPhaseOneFini()
{
  if (userMgrQueue != L7_NULLPTR)
  {
    (void)osapiMsgQueueDelete(userMgrQueue);
  }

  if (userMgrSyncSema != L7_NULL)
  {
    (void)osapiSemaDelete(userMgrSyncSema);
  }

  return;
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
L7_RC_t userMgrRadiusPhaseTwoFini()
{
  L7_RC_t rc = L7_SUCCESS;

  /* de-register by registering a null function ptr */
  if (radiusResponseRegister(L7_USER_MGR_COMPONENT_ID, L7_NULLPTR)==L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USER_MGR_COMPONENT_ID,
            "Unable to register User Manager for RADIUS response callback\n");
    rc = L7_FAILURE;
  }

  return(rc);
}
