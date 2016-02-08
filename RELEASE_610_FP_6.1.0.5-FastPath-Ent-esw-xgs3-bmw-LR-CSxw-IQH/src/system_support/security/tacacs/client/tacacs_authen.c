/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_authen.c
*
* @purpose All functions related TACACS+ Client Authentication.
*
* @component tacacs+
*
* @comments none
*
* @create 03/17/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#include "string.h"
#include "l7_common.h"
#include "log.h"
#include "tacacs_api.h"
#include "tacacs_cfg.h"
#include "tacacs_aaa.h"
#include "tacacs_authen.h"
#include "tacacs_control.h"
#include "tacacs_txrx.h"
#include "osapi_support.h"
#include "l7utils_inet_addr_api.h"

extern void                *tacacsSemaphore;
extern tacacsOprData_t     *tacacsOprData;
extern tacacsCfg_t         *tacacsCfg;
extern tacacsNotifyEntry_t *tacacsNotifyList;

static void tacacsAuthStateConnectedProcess(tacacsAuthSession_t *session);
static L7_RC_t tacacsDomainNameNextGet(L7_uchar8 * serverAddress,
                                L7_uchar8  *pNextServerAddress);

/*********************************************************************
*
* @purpose To close the TCP connection for a session.
*
* @param tacacsAuthSession_t *session @b{(input)} auth session state
*
* @returns none
*
* @comments
*
* @end
*
*********************************************************************/
static void tacacsConnectionClose(tacacsAuthSession_t *session)
{
  if (session->socket != 0)
  {
    osapiSocketClose(session->socket);
    session->socket = 0;
  }
  return;
}

/*********************************************************************
* @purpose  Clean up session buffer
*
* @param    tacacsAuthSession_t *session   @b((input))  session state
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void tacacsAuthSessionDelete(tacacsAuthSession_t *session)
{
  tacacsConnectionClose(session);
  memset(session, 0, sizeof(tacacsAuthSession_t));
  return;
}

/*********************************************************************
* @purpose  Create the authentication session state
*
* @param L7_uint32       componentId  @b{(input)}   component Id.
* @param L7_uint32       correlator   @b{(input)}   correlator.
* @param L7_char8        *username    @b{(input)}   name of user to be authenticated.
* @param L7_char8        *password    @b{(input)}   password for the user.
* @param tacacsSession_t **session    @b((output))  session state
*
* @returns  L7_SUCESS, session generated
*           L7_FAILURE, no session buffers available
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t tacacsAuthSessionGet(userMgrAuthRequest_t *request,
                             L7_uint32             correlator,
                             tacacsAuthSession_t **session)
{
  tacacsAuthSession_t *sess = L7_NULLPTR;
  L7_uint32            i = 0;

  for (i=0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
  {
    if (tacacsOprData->sessions[i].sessionId == 0)
    {
      sess = &tacacsOprData->sessions[i];
      break;
    }
  }
  if (sess == L7_NULLPTR)
  {
    /* no session buffers available */
    LOG_MSG("TACACS+: failed to create authentication session, no buffers available.");
    return L7_FAILURE;
  }
  memset(sess, 0, sizeof(tacacsAuthSession_t));
  sess->server = 0;
  sess->sessionId = tacacsSessionIdGenerate();  /* dfowler - FIX THIS!!! */
  sess->state = TACACS_AUTH_STATE_BEGIN;
  sess->accessLevel = request->accessLevel;
  sess->componentId = request->component;
  sess->correlator = correlator;
  sess->servType = request->servType;

  /* we already verified the lengths on input */
  if (L7_NULLPTR != request->pUserName)
  {
    osapiStrncpy(sess->username, request->pUserName, sizeof(sess->username));
  }
  else
  {
    osapiStrncpy(sess->username, "", sizeof(sess->username));
  }
  if (L7_NULLPTR != request->pPwd)
  {
    osapiStrncpy(sess->password, request->pPwd, sizeof(sess->password));
  }
  else
  {
    osapiStrncpy(sess->password, "", sizeof(sess->password));
  }
  sess->mode             =  request->mode;
  sess->port             =  request->port;
  sess->rem_addr         =  request->rem_addr;
  sess->pState           =  request->pState;
  sess->mayChallenge     =  request->mayChallenge;
  sess->isChallenged     = &request->isChallenged;
  sess->pChallengePhrase =  request->pChallengePhrase;
  sess->challengeFlags   = &request->challengeFlags;

  request->isChallenged   = L7_FALSE;
  request->challengeFlags = 0;

  *session = sess;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send failure status back to requesting component
*
* @param    tacacsAuthSession_t    *session     @b((input))  session state
* @param    tacacsAuthStatus_t      status      @b((input))  failure status
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void tacacsAuthFailureSend(tacacsAuthSession_t *session,
                           tacacsAuthStatus_t   status)
{
  if (tacacsNotifyList[session->componentId].notifyFunction != L7_NULLPTR)
  {
    tacacsNotifyList[session->componentId].notifyFunction(status,
                                                          session->correlator,
                                                          L7_LOGIN_ACCESS_NONE);
  }
  else
  {
    LOG_MSG("TACACS+: Failed to send failure response to component %i, not registered",
            session->componentId);
  }

  /* clean up session */
  tacacsAuthSessionDelete(session);
  return;
}

/*********************************************************************
* @purpose  Send authentication success back to requesting component
*
* @param    tacacsAuthSession_t *session     @b((input))  session state
* @param    L7_uint32           accessLevel  @b((input))  authenticated user access level
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void tacacsAuthSuccessSend(tacacsAuthSession_t *session)
{
  if (tacacsNotifyList[session->componentId].notifyFunction != L7_NULLPTR)
  {
    tacacsNotifyList[session->componentId].notifyFunction(TACACS_STATUS_AUTH_SUCCESS,
                                                          session->correlator,
                                                          session->accessLevel);
  }
  else
  {
    LOG_MSG("TACACS+: Failed to send success response to component %i, not registered",
            session->componentId);
  }

  /* clean up the session */
  tacacsAuthSessionDelete(session);
  return;
}

/*********************************************************************
* @purpose  Send authentication success back to requesting component
*
* @param    tacacsAuthSession_t *session     @b((input))  session state
* @param    L7_uint32           accessLevel  @b((input))  authenticated user access level
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void tacacsAuthChallengeSend(tacacsAuthSession_t *session, L7_uchar8 *pkt)
{
  L7_ushort16 msgLen;

  if (L7_TRUE != session->mayChallenge)
  {
    tacacsAuthFailureSend(session, TACACS_STATUS_AUTH_FAILURE);
    return;
  }

  memset(session->pChallengePhrase, 0, L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH);
  msgLen = osapiNtohs(*((L7_ushort16 *)(pkt + L7_TACACS_REPLY_SRV_MSG_LEN_OFFSET)));
  if (msgLen > 0)
  {
    if (msgLen > L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH)
    {
      msgLen = L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH;
    }
    *(session->challengeFlags) = *(pkt + L7_TACACS_REPLY_FLAGS_OFFSET);

    memcpy(session->pChallengePhrase, pkt + L7_TACACS_REPLY_SRV_MSG_OFFSET, msgLen);
  }

  *(session->isChallenged) = L7_TRUE;
  *((void **)session->pState) = session;

  if (tacacsNotifyList[session->componentId].notifyFunction != L7_NULLPTR)
  {
    tacacsNotifyList[session->componentId].notifyFunction(TACACS_STATUS_AUTH_CHALLENGE,
                                                          session->correlator,
                                                          session->accessLevel);
  }
  else
  {
    LOG_MSG("TACACS+: Failed to send challenge response to component %i, not registered",
            session->componentId);
    tacacsAuthSessionDelete(session);
  }

  /* do NOT clean up the session */

  return;
}

/*********************************************************************
*
* @purpose To construct the header of an authentication packet.
*
* @param   L7_uchar8 *packet           @b{(output)} pointer to packet buffer
* @param   L7_uint32  length           @b{(input)}  total packet length
* @param   L7_uint32  sessionId        @b{(input)}  session random identifier
* @param   L7_uchar8  seqNo            @b((input))  current sequence number
* @param   L7_BOOL    singleConnection @b((input))  request single connection
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static void tacacsAuthPacketHeaderConstruct(L7_uchar8 *packet,
                                            L7_uchar8  type,
                                            L7_uint32  length,
                                            L7_uint32  sessionId,
                                            L7_uchar8  seqNo,
                                            L7_BOOL    singleConnection)
{
  *(packet + L7_TACACS_PKT_VERSION_OFFSET) = L7_TACACS_MAJOR_VER << L7_TACACS_MAJOR_VER_SHIFT;
  /* we are not supporting any auth messages with TAC_PLUS_MINOR_VER_ONE */
  *(packet + L7_TACACS_PKT_VERSION_OFFSET) |= L7_TACACS_MINOR_VER_DEFAULT;
  *(packet + L7_TACACS_PKT_TYPE_OFFSET) = type;
  *(packet + L7_TACACS_PKT_SEQ_NO_OFFSET) = seqNo;

  if (tacacsOprData->unencrypted == L7_TRUE)
  {
    *(packet + L7_TACACS_PKT_FLAGS_OFFSET) |= L7_TACACS_UNENCRYPTED_FLAG;
  }
  if (singleConnection == L7_TRUE)
  {
    *(packet + L7_TACACS_PKT_FLAGS_OFFSET) |= L7_TACACS_SINGLE_CONNECT_FLAG;
  }

  *((L7_uint32 *)(packet + L7_TACACS_PKT_SESSION_ID_OFFSET)) = osapiHtonl(sessionId);
  *((L7_uint32 *)(packet + L7_TACACS_PKT_LENGTH_OFFSET)) = osapiHtonl(length);

  return;
}

/*********************************************************************
*
* @purpose To construct an authentication START packet
*
* @param   L7_uchar8 *packet           @b{(output)} pointer to packet buffer
* @param   L7_uint32  sessionId        @b{(input)}  session random identifier
* @param   L7_uchar8  seqNo            @b((input))  current sequence number
* @param   L7_BOOL    singleConnection @b((input))  request single connection
*
* @returns L7_uint32 length of packet body, does not include header.
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_uint32 tacacsAuthStartPacketConstruct(tacacsAuthSession_t *session,
                                                L7_BOOL              singleConnection)
{
  L7_uchar8 *p = L7_NULLPTR;
  L7_uchar8  userLen = 0;
  L7_uchar8  portLen = 0;
  L7_uchar8  addrLen = 0;
  L7_uint32   length = L7_TACACS_START_FIXED_FIELDS;
  L7_uchar8 *portname;

  /*
   * Only send the user name for enable since there will be no GETUSER response.
   */
  if (ACCESS_LEVEL_ENABLE == session->mode)
  {
    userLen = strlen(session->username);
  }

  if (L7_NULLPTR != session->port)
  {
    portLen = strlen(session->port);
    portname = session->port;
  }
  else
  {
    portLen = strlen(L7_TACACS_PORT_NAME);
    portname = L7_TACACS_PORT_NAME;
  }

  if (L7_NULLPTR != session->rem_addr)
  {
    addrLen = strlen(session->rem_addr);
  }

  length += userLen + portLen + addrLen;

  memset(session->txPacket.packet, 0, sizeof(session->txPacket.packet));

  p = session->txPacket.packet;

  tacacsAuthPacketHeaderConstruct(p, L7_TACACS_AUTHEN, length, session->sessionId,
                                  session->sequenceNo, singleConnection);

  /* only fill in what we need, unsupported fields are zero */
  *(p + L7_TACACS_START_ACTION_OFFSET) = L7_TACACS_AUTHEN_LOGIN;
  *(p + L7_TACACS_START_PRIVLVL_OFFSET) = session->accessLevel;
  *(p + L7_TACACS_START_AUTHEN_TYPE_OFFSET) = session->sequenceNo;
  if (ACCESS_LEVEL_ENABLE == session->mode)
  {
    *(p + L7_TACACS_START_SERVICE_OFFSET) = L7_TACACS_AUTHEN_SVC_ENABLE;
  }
  else
  {
    *(p + L7_TACACS_START_SERVICE_OFFSET) = L7_TACACS_AUTHEN_SVC_LOGIN;
  }

  *(p + L7_TACACS_START_USR_LEN_OFFSET) = userLen;
  *(p + L7_TACACS_START_PORT_LEN_OFFSET) = portLen;
  *(p + L7_TACACS_START_REM_ADDR_LEN_OFFSET) = addrLen;

  p += L7_TACACS_START_USER_OFFSET;
  memcpy(p, session->username, userLen);
  p += userLen;
  memcpy(p, portname, portLen);
  p += portLen;
  memcpy(p, session->rem_addr, addrLen);
  return length;
}

/*********************************************************************
*
* @purpose To construct authentication Continue Packet
*
* @param   pAuthenSessionInfo  @b{(input)}  Authentication session info
* @param   pAuthenInput        @b{(input)}  Authentication Input request
* @param   pTacacsPacket       @b{(output)} Authentication packet filled
*                                           for authentication start or
*                                           continue
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_uint32 tacacsAuthContinuePacketConstruct(L7_uchar8 *packet,
                                                   L7_uint32  sessionId,
                                                   L7_uchar8  seqNo,
                                                   L7_BOOL    singleConnection,
                                                   L7_BOOL    terminate,
                                                   L7_char8  *message)
{
  L7_ushort16 msgLen = strlen(message);
  L7_uint32   length = L7_TACACS_CONT_FIXED_FIELDS + msgLen;

  memset(packet, 0, TACACS_TX_PKT_BUF_SIZE);

  tacacsAuthPacketHeaderConstruct(packet, L7_TACACS_AUTHEN, length, sessionId,
                                  seqNo, singleConnection);

  /* the data fields are not used in the ASCII login type
     we are handling, we only need the message fields */
  *((L7_short16 *)(packet + L7_TACACS_CONT_USER_MSG_LEN_OFFSET)) = osapiHtons(msgLen);
  memcpy((packet + L7_TACACS_CONT_USER_MSG_OFFSET), message, msgLen);

  if (terminate == L7_TRUE)
  {
    *(packet + L7_TACACS_CONT_FLAGS_OFFSET) |= L7_TACACS_CONT_ABORT_FLAG;
  }
  return length;
}

/*********************************************************************
*
* @purpose To construct an authorization request packet
*
* @param   L7_uchar8 *packet           @b{(output)} pointer to packet buffer
* @param   L7_uint32  sessionId        @b{(input)}  session random identifier
* @param   L7_uchar8  seqNo            @b((input))  current sequence number
* @param   L7_BOOL    singleConnection @b((input))  request single connection
*
* @returns L7_uint32 length of packet body, does not include header.
*
* @comments we don't support much authorization,
*           currently this is only used to see if user has exec prv-lvl 15,
*           so that we can determine L7_LOGIN_ACCESS_READ_WRITE.
*
* @end
*
*********************************************************************/
static L7_uint32 tacacsAuthRequestPacketConstruct(tacacsAuthSession_t *session,
                                                  L7_BOOL    singleConnection)
{
  L7_uchar8 *packet = L7_NULLPTR;
  L7_uchar8  userLen = strlen(session->username);
  L7_uchar8  portLen = strlen(L7_TACACS_PORT_NAME);
  L7_uchar8  arg1Len = strlen(L7_TACACS_REQUEST_ATTR_VALUE_SERVICE);
  L7_uchar8  arg2Len = strlen(L7_TACACS_REQUEST_ATTR_VALUE_CMD);
  L7_uchar8  arg3Len = strlen(L7_TACACS_REQUEST_ATTR_VALUE_PRIV_LVL);
  L7_uint32  length = L7_TACACS_REQUEST_FIXED_FIELDS + \
                      L7_TACACS_REQUEST_ATTR_VALUE_ARG_CNT + \
                      userLen + portLen + arg1Len + arg2Len + arg3Len;

  memset(session->txPacket.packet, 0, TACACS_TX_PKT_BUF_SIZE);

  packet = session->txPacket.packet;

  tacacsAuthPacketHeaderConstruct(packet, L7_TACACS_AUTHOR, length, session->sessionId,
                                  session->sequenceNo, singleConnection);

  /* only fill in what we need, unsupported fields are zero */
  *(packet + L7_TACACS_REQUEST_AUTHEN_METHOD_OFFSET) = L7_TACACS_AUTHEN_METH_TACPLUS;
  *(packet + L7_TACACS_START_PRIVLVL_OFFSET) = L7_TACACS_PRIV_LVL_READ_ONLY;
  *(packet + L7_TACACS_START_AUTHEN_TYPE_OFFSET) = L7_TACACS_AUTHEN_TYPE_ASCII;
  *(packet + L7_TACACS_START_SERVICE_OFFSET) = L7_TACACS_AUTHEN_SVC_LOGIN;
  *(packet + L7_TACACS_START_USR_LEN_OFFSET) = userLen;
  *(packet + L7_TACACS_START_PORT_LEN_OFFSET) = portLen;

  /* number of REQUEST arguments */
  *(packet + L7_TACACS_REQUEST_ARG_CNT_OFFSET) = L7_TACACS_REQUEST_ATTR_VALUE_ARG_CNT;
  *(packet + L7_TACACS_REQUEST_ARG_CNT_OFFSET+1) = arg1Len;
  *(packet + L7_TACACS_REQUEST_ARG_CNT_OFFSET+2) = arg2Len;
  *(packet + L7_TACACS_REQUEST_ARG_CNT_OFFSET+3) = arg3Len;
  packet   += L7_TACACS_REQUEST_USER_OFFSET;
  memcpy(packet, session->username, userLen);
  packet += userLen;
  memcpy(packet, L7_TACACS_PORT_NAME, portLen);
  packet += portLen;
  memcpy(packet, L7_TACACS_REQUEST_ATTR_VALUE_SERVICE, arg1Len);
  packet += arg1Len;
  memcpy(packet, L7_TACACS_REQUEST_ATTR_VALUE_CMD, arg2Len);
  packet += arg2Len;
  memcpy(packet, L7_TACACS_REQUEST_ATTR_VALUE_PRIV_LVL, arg3Len);

  return length;
}

/*********************************************************************
*
* @purpose Cancel connection in process, called on connection timeout.
*
* @param tacacsAuthSession_t *session @((input)) session state
*
* @returns none
*
* @comments  none
*
* @end
*
*********************************************************************/
static void tacacsConnectCancel(tacacsAuthSession_t *session)
{
  if (session->connectTimer != 0)
  {
    tacacsTimerDelete();
    session->connectTimer = 0;
  }

  if (session->connectTaskId != 0)
  {
    osapiTaskDelete(session->connectTaskId);
    session->connectTaskId = 0;
  }

  /* session->socket not set until connected */
  if (session->connectArgv[2] != 0)
  {
    osapiSocketClose(session->connectArgv[2]);
    session->connectArgv[2] = 0;
  }

  session->connectArgv[0] = 0;
  session->connectArgv[1] = 0;
  return;
}

/*********************************************************************
*
* @purpose Contact next server on an error.
*
* @param   tacacsAuthSession_t *session  @b{(input)}  session state.
*
* @returns void
*
* @comments done with packet after this function
*
* @end
*
*********************************************************************/
void tacacsContactNextServer(tacacsAuthSession_t *session)
{
  L7_uint32 serverIdx = 0;

  if (session->state == TACACS_AUTH_STATE_CONNECT_WAIT)
  {
    tacacsConnectCancel(session);
  } else
  {
    tacacsConnectionClose(session);
  }

  if (tacacsServerToContactNextGet(session->server, &serverIdx) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_WARNING,  L7_TACACS_COMPONENT_ID, "TACACS+: authentication error, no "
					"server to contact. TACACS+ request needed, but no servers are configured.");
    tacacsAuthFailureSend(session, TACACS_STATUS_SERVER_FAILURE);
    return;
  }

  session->state = TACACS_AUTH_STATE_BEGIN;
  session->server = tacacsCfg->servers[serverIdx].ip;
  tacacsAuthStateBeginProcess(session);
  return;
}

/*********************************************************************
*
* @purpose Process server removed from configuration, find sessions using
*          this server and contact next.
*
* @param L7_uint32 ip     @((input)) server ip
*
* @returns none
*
* @comments none
*
* @end
*
*********************************************************************/
void tacacsServerRemoveProcess(L7_uint32 ip)
{
  L7_uint32 i = 0;
  /* walk through sessions, update any sessions using this server */
  for (i=0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
  {
    tacacsAuthSession_t *session = &tacacsOprData->sessions[i];
    if (session->server == ip)
    {
      session->server = 0;
      tacacsContactNextServer(session);
      break;
    }
  }
  return;
}

/*********************************************************************
*
* @purpose Process a server connection failure, find session that is waiting
*           on this connection and process state appropriately.
*
* @param L7_uint32 ip     @((input)) server ip connected
* @param L7_uint32 port   @((input)) server port connected
* @param L7_uint32 socket @((input)) server socket connected
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
void tacacsServerConnectFailedProcess(L7_uint32 ip,
                                      L7_uint32 port,
                                      L7_uint32 socket)
{
  L7_uint32 i = 0;
  /* walk through sessions, find session that requested this connection */
  for (i=0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
  {
    tacacsAuthSession_t *session = &tacacsOprData->sessions[i];
    if ((session->state == TACACS_AUTH_STATE_CONNECT_WAIT) &&
        (session->server == ip) &&
        (session->connectTaskId != 0) &&
        (session->connectArgv[0] == ip) &&
        (session->connectArgv[1] == port) &&
        (session->connectArgv[2] == socket))
    {
      session->connectTaskId = 0;   /* if we get here connection task completed */
	  L7_LOGF(L7_LOG_SEVERITY_WARNING,  L7_TACACS_COMPONENT_ID, "TACACS+: connection failed to server %s. i"
								          "TACACS+ request sent to server %s but no response was received.", 
										  osapiInet_ntoa(ip), osapiInet_ntoa(ip));
      /* tacacsConnectCancel will be called based on wait state */
      tacacsContactNextServer(session);
      return;
    }
  }
  /* this shouldn't ever happen, but make sure we close the socket */
  osapiSocketClose(socket);
  LOG_MSG("failed to find session for server connection.");
  return;
}

/*********************************************************************
*
* @purpose Process a server connection, find session that is waiting
*           on this connection and process state appropriately.
*
* @param L7_uint32 ip     @((input)) server ip connected
* @param L7_uint32 port   @((input)) server port connected
* @param L7_uint32 socket @((input)) server socket connected
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments ip, port, socket must be verified against current config,
*           the connect task processes the connection, does not hold
*           the semaphore, something could change.
*
* @end
*
*********************************************************************/
void tacacsServerConnectedProcess(L7_uint32 ip,
                                  L7_uint32 port,
                                  L7_uint32 socket)
{
  L7_uint32 i = 0;
  L7_uint32 serverIdx = 0;
  L7_BOOL   error = L7_FALSE;

  if ((tacacsServerIndexGet(ip, &serverIdx) != L7_SUCCESS) ||
      (tacacsCfg->servers[serverIdx].port != port))
  {
    /* server configuration changed, need to start over */
    error = L7_TRUE;
  }

  /* walk through sessions, find session that requested this connection */
  for (i=0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
  {
    tacacsAuthSession_t *session = &tacacsOprData->sessions[i];
    /* probably don't need to check all these values, but can't hurt */
    if ((session->state == TACACS_AUTH_STATE_CONNECT_WAIT) &&
        (session->server == ip) &&
        (session->connectTaskId != 0) &&
        (session->connectArgv[0] == ip) &&
        (session->connectArgv[1] == port) &&
        (session->connectArgv[2] == socket))
    {
      session->connectTaskId = 0; /* if we get here connection task completed */
      if (error)
      {
        /* server configuration changed, close socket, start over */
        /* tacacsConnectCancel will be called based on wait state */
        tacacsContactNextServer(session);
        return;
      }
      else
      {
        session->socket = socket;
        session->state = TACACS_AUTH_STATE_CONNECTED;
        session->connectTimer = 0;
        tacacsTimerDelete();
        session->connectArgv[0] = 0;
        session->connectArgv[1] = 0;
        session->connectArgv[2] = 0;
        tacacsAuthStateConnectedProcess(session);
        return;
      }
      break;
    }
  }
  /* this shouldn't ever happen, but make sure we close the socket */
  osapiSocketClose(socket);
  LOG_MSG("failed to find session for server connection.");
  return;
}

/*********************************************************************
*
* @purpose Open connection to a server based on session state.
*
* @param    tacacsAuthSession_t *session     @b((input))  session state
* @param    L7_uint32            serverIdx   @b((input))  server configuration index
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
static void tacacsConnectionOpen(tacacsAuthSession_t *session,
                                 L7_uint32            serverIdx)
{
  L7_uint32 socket = 0;
  L7_uint32 taskId = L7_ERROR;

  if (tacacsServerSocketOpen(&tacacsCfg->servers[serverIdx],
                             &socket) != L7_SUCCESS)
  {
    /* try another server */
    LOG_MSG("TACACS+: failed to open socket to server %s.",
            osapiInet_ntoa(tacacsCfg->servers[serverIdx].ip));
    tacacsContactNextServer(session);
    return;
  }

  /* establish connection for this server */
  /* store copies of ip, port, socket with session, connection task does not
     use a semaphore, config data and opr data can change while its running
     we'll verify values again when connection is established */
  session->connectArgv[0] = session->server;
  session->connectArgv[1] = tacacsCfg->servers[serverIdx].port;
  session->connectArgv[2] = socket;
  taskId = (L7_uint32)osapiTaskCreate("tacacsConnectTask",
                                      (void *)tacacsConnectTask,
                                      3,
                                      session->connectArgv,
                                      L7_DEFAULT_STACK_SIZE,
                                      L7_DEFAULT_TASK_PRIORITY,
                                      L7_DEFAULT_TASK_SLICE);
  if (taskId == L7_ERROR)
  {
    /* if we fail to create a task, something bad happened, just fail */
    LOG_MSG("TACACS+: authentication error, failed to create connection task.");
    tacacsAuthFailureSend(session, TACACS_STATUS_OTHER_FAILURE);
    return;
  }
  session->state = TACACS_AUTH_STATE_CONNECT_WAIT;
  session->connectTaskId = taskId;  /* save id for connection timeout */
  session->connectTimer = tacacsCfg->servers[serverIdx].useGblTimeout ?
    tacacsCfg->timeout :
    tacacsCfg->servers[serverIdx].timeout;
  tacacsTimerAdd();  /* start connection timer */
  return;
}

/*********************************************************************
*
* @purpose Begin an authentication session, establish a connection to
*          the appropriate configured server, called on component request
*          and to try another server on connection/server failures.
*
* @param    tacacsAuthSession_t *session     @b((input))  session state
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void tacacsAuthStateBeginProcess(tacacsAuthSession_t *session)
{
  L7_uint32 serverIdx = 0;

  if (tacacsServerIndexGet(session->server, &serverIdx) != L7_SUCCESS)
  {
    /* server configuration must have changed */
    tacacsContactNextServer(session);
    return;
  }

  /* open a connection to the server */
  tacacsConnectionOpen(session, serverIdx);
  return;
}

/*********************************************************************
*
* @purpose Start authentication by Calling Being Start State function.
*          Prior to authentication re-resolve all Host Names
*
* @param    tacacsAuthSession_t *session     @b((input))  session state
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void tacacsAuthenticationStart(tacacsAuthSession_t *session)
{
  L7_uchar8 hostName[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uchar8 nextHostName[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uint32 serverIdx = 0;

  memset(hostName, 0, L7_DNS_HOST_NAME_LEN_MAX);
  memset(nextHostName, 0, L7_DNS_HOST_NAME_LEN_MAX);

  /* Before starting the authentication re resolve all the IP addresses.
     If any change in IP address than IP address is updated */

  while( tacacsDomainNameNextGet(hostName, nextHostName) == L7_SUCCESS)
  {
    tacacsServerResolveHostName(nextHostName);
    strcpy(hostName, nextHostName);
  }

  if (tacacsServerToContactNextGet(0, &serverIdx) != L7_SUCCESS)
  {
    LOG_MSG("TACACS+: failed to create authentication session, no server to contact.");
    tacacsAuthFailureSend(session, TACACS_STATUS_SERVER_FAILURE);
    return;
  }
  session->server = tacacsCfg->servers[serverIdx].ip;

  /* Begin the authentication with the session identifier passed */
  tacacsAuthStateBeginProcess(session);
  return;
}

/*********************************************************************
*
* @purpose Return the next configured host name of tacacs server
*
* @param    L7_uchar8 *serverAddress       @b((input)) Current server address
* @param    L7_uchar8 *pNextServerAddress  @b((output)) Next server address
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsDomainNameNextGet(L7_uchar8 * serverAddress,
                                L7_uchar8  *pNextServerAddress)
{
  L7_uint32 serverIdx = 0;
  L7_RC_t rc = L7_FAILURE;

  if (serverAddress[0] == 0)
  {
    while(serverIdx < L7_TACACS_MAX_SERVERS)
    {
      if(tacacsCfg->servers[serverIdx].hostName.hostAddrType
           == L7_IP_ADDRESS_TYPE_DNS)
      {
        rc = L7_SUCCESS;
        strcpy(pNextServerAddress,
               tacacsCfg->servers[serverIdx].hostName.host.hostName);
        break;
      }
      serverIdx++;
    }
  }
  else if (serverAddress[0] != 0)
  {
    if (tacacsServerHostIndexGet( serverAddress,
                                       &serverIdx) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    while(++serverIdx < L7_TACACS_MAX_SERVERS)
    {
      if(tacacsCfg->servers[serverIdx].hostName.hostAddrType
           == L7_IP_ADDRESS_TYPE_DNS)
      {
        rc = L7_SUCCESS;
        strcpy(pNextServerAddress,
               tacacsCfg->servers[serverIdx].hostName.host.hostName);
        break;
      }
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose Start sending packets on connected session.
*
* @param    tacacsAuthSession_t *session     @b((input))  session state
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
static void tacacsAuthStateConnectedProcess(tacacsAuthSession_t *session)
{
  L7_uint32 serverIdx = 0;
  L7_uint32 length = 0; /* packet body length */

  if (tacacsServerIndexGet(session->server, &serverIdx) != L7_SUCCESS)
  {
    /* server configuration must have changed */
    tacacsContactNextServer(session);
    return;
  }

  /* construct authentication packet based on session state */
  session->sequenceNo = 1;  /* always 1 for first packet */

  if (session->servType == L7_SERVICE_AUTHOR)
  {
     length = tacacsAuthRequestPacketConstruct(session, L7_FALSE);
  }
  else
  {
    length = tacacsAuthStartPacketConstruct(session, L7_FALSE);
  }

  /* encrypt the packet, check opr encrypt flag for debug/support */
  if (tacacsOprData->unencrypted != L7_TRUE)
  {
    L7_char8 *key = tacacsCfg->servers[serverIdx].useGblKey ? tacacsCfg->key :
      tacacsCfg->servers[serverIdx].key;
    if (strlen(key) == 0)
    {
      /* no key to use, try another server */
      LOG_MSG("TACACS+: no key configured to encrypt packet for server %s.",
              osapiInet_ntoa(tacacsCfg->servers[serverIdx].ip));
      tacacsContactNextServer(session);
      return;
    }
    tacacsEncryptDecrypt(session->txPacket.packet, length, key);
  }

  /* tx/rx task will pick up packet and send when socket is ready */
  session->txPacket.length = length + L7_TACACS_PKT_HEADER_LEN;  /* total bytes to write to socket */
  session->txPacket.bytesWritten = 0;
  session->state = TACACS_AUTH_STATE_READY_TO_SEND;
  return;
}

/*********************************************************************
*
* @purpose To process a user name request.
*
* @param   tacacsAuthSession_t *session  @b{(input)}  authentication response packet.
*
* @returns void
*
* @comments done with packet after this function
*
* @end
*
*********************************************************************/
static void tacacsAuthStatusGetUserProcess(tacacsAuthSession_t *session, L7_uchar8 *pkt)
{
  L7_uint32 length = 0;     /* packet body length */
  L7_uint32 serverIdx = 0;

  if (tacacsServerIndexGet(session->server, &serverIdx) != L7_SUCCESS)
  {
    /* server configuration must have changed */
    tacacsContactNextServer(session);
    return;
  }

  if (0 == strlen(session->username))
  {
    /*
     * Need to get a user name.
     */
    tacacsAuthChallengeSend(session, pkt);
    return;
  }

  /* construct authentication continue packet based on session state */
  session->sequenceNo += 2;  /* we use every other number */

  length = tacacsAuthContinuePacketConstruct(session->txPacket.packet,
                                             session->sessionId,
                                             session->sequenceNo,
                                             L7_FALSE,  /* single connection not implemented */
                                             L7_FALSE,
                                             session->username);

  /* encrypt the packet, check opr encrypt flag for debug/support */
  if (tacacsOprData->unencrypted != L7_TRUE)
  {
    L7_char8 *key = tacacsCfg->servers[serverIdx].useGblKey ? tacacsCfg->key :
      tacacsCfg->servers[serverIdx].key;
    if (strlen(key) == 0)
    {
      /* no key to use, try another server */
      L7_LOGF(L7_LOG_SEVERITY_WARNING,  L7_TACACS_COMPONENT_ID, "TACACS+: no key configured to encrypt "
                                                                "packet for server %s. No key configured for the specified server.",  
                                                                 osapiInet_ntoa(tacacsCfg->servers[serverIdx].ip));

      tacacsContactNextServer(session);
      return;
    }
    tacacsEncryptDecrypt(session->txPacket.packet, length, key);
  }

  /* tx/rx task will pick up packet and send when socket is ready */
  session->txPacket.length = length + L7_TACACS_PKT_HEADER_LEN;  /* total bytes to write to socket */
  session->txPacket.bytesWritten = 0;
  session->state = TACACS_AUTH_STATE_READY_TO_SEND;
  return;
}

/*********************************************************************
*
* @purpose To process a password request.
*
* @param   tacacsAuthSession_t *session  @b{(input)}  authentication response packet.
*
* @returns void
*
* @comments done with packet after this function
*
* @end
*
*********************************************************************/
static void tacacsAuthStatusGetPassProcess(tacacsAuthSession_t *session,
                                           L7_uchar8 *pkt)
{
  L7_uint32 length = 0;     /* packet body length */
  L7_uint32 serverIdx = 0;

  if (tacacsServerIndexGet(session->server, &serverIdx) != L7_SUCCESS)
  {
    /* server configuration must have changed */
    tacacsContactNextServer(session);
    return;
  }

  if (0 == strlen(session->password))
  {
    /*
     * Need to get a password.
     */
    tacacsAuthChallengeSend(session, pkt);
    return;
  }

  /* construct authentication continue packet based on session state */
  session->sequenceNo += 2;  /* we use every other number */

  length = tacacsAuthContinuePacketConstruct(session->txPacket.packet,
                                             session->sessionId,
                                             session->sequenceNo,
                                             L7_FALSE,  /* single connection not implemented */
                                             L7_FALSE,
                                             session->password);

  /* encrypt the packet, check opr encrypt flag for debug/support */
  if (tacacsOprData->unencrypted != L7_TRUE)
  {
    L7_char8 *key = tacacsCfg->servers[serverIdx].useGblKey ? tacacsCfg->key :
      tacacsCfg->servers[serverIdx].key;
    if (strlen(key) == 0)
    {
      /* no key to use, try another server */
      LOG_MSG("TACACS+: no key configured to encrypt CONTINUE packet for server %s.",
              osapiInet_ntoa(tacacsCfg->servers[serverIdx].ip));
      tacacsContactNextServer(session);
      return;
    }
    tacacsEncryptDecrypt(session->txPacket.packet, length, key);
  }

  /* tx/rx task will pick up packet and send when socket is ready */
  session->txPacket.length = length + L7_TACACS_PKT_HEADER_LEN;  /* total bytes to write to socket */
  session->txPacket.bytesWritten = 0;
  session->state = TACACS_AUTH_STATE_READY_TO_SEND;
  return;
}

/*********************************************************************
*
* @purpose Read session id from response packet.
*
* @param   tacacsRxPacket_t *rxPacket  @b{(input)}  authentication response packet.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 tacacsAuthResponseSessionIdGet(tacacsRxPacket_t *rxPacket)
{
  return osapiNtohl(*((L7_uint32 *)(rxPacket->packet + L7_TACACS_PKT_SESSION_ID_OFFSET)));
}

/*********************************************************************
*
* @purpose To process the received authentication response packet.
*
* @param   tacacsRxPacket_t *rxPacket  @b{(input)}  authentication response packet.
*
* @returns void
*
* @comments done with packet after this function
*
* @end
*
*********************************************************************/
void tacacsAuthResponseProcess(tacacsRxPacket_t *rxPacket)
{
  L7_uchar8           *pkt = rxPacket->packet;
  tacacsAuthSession_t *session = L7_NULLPTR;
  L7_uint32            sessionId = 0;
  L7_uchar8            unencrypt = 0;
  L7_uint32            msgLen = 0;
  L7_uint32            dataLen = 0;
  L7_uchar8            status = 0;  /* reply status */
  L7_uint32            serverIdx = 0;
  L7_uint32            i = 0;
  L7_uchar8            type = 0;

  /* read session id */
  sessionId = tacacsAuthResponseSessionIdGet(rxPacket);

  /* find session with sessionId, verify socket */
  for (i=0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
  {
    if ((tacacsOprData->sessions[i].sessionId == sessionId) &&
        (tacacsOprData->sessions[i].socket == rxPacket->socket))
    {
      session = &tacacsOprData->sessions[i];
      break;
    }
  }
  if (session == L7_NULLPTR)
  {
    LOG_MSG("TACACS+: received response packet for unknown session.");
    return;
  }

  /* once we have a session, all errors should be handled with session */

  /* verify packet type */
  type = (*(pkt + L7_TACACS_PKT_TYPE_OFFSET));

  /* get corresponding server configuration */
  if (tacacsServerIndexGet(session->server, &serverIdx) != L7_SUCCESS)
  {
    LOG_MSG("TACACS+: failed to find server to process response.");
    tacacsContactNextServer(session);
    return;
  }

  /* only allow unencrypted packets if debug flag is set */
  unencrypt = (*(pkt + L7_TACACS_PKT_FLAGS_OFFSET)) & L7_TACACS_UNENCRYPTED_FLAG;
  if ((unencrypt != 0) &&
      (tacacsOprData->unencrypted == L7_FALSE))
  {
    LOG_MSG("TACACS+: received invalid unencrypted packet.");
    tacacsContactNextServer(session);
    return;
  }

  /* decrypt the received packet */
  if (unencrypt == 0)
  {
    L7_char8 *key = tacacsCfg->servers[serverIdx].useGblKey ? tacacsCfg->key :
      tacacsCfg->servers[serverIdx].key;
    if (strlen(key) == 0)
    {
      /* no key to use, try another server */
      LOG_MSG("TACACS+: no key configured to unencrypt REPLY packet for server %s.",
              osapiInet_ntoa(tacacsCfg->servers[serverIdx].ip));
      tacacsContactNextServer(session);
      return;
    }
    tacacsEncryptDecrypt(pkt, rxPacket->length, key);
  }

  /* validate versions, sequence number, etc. */
  if (((*(pkt + L7_TACACS_PKT_VERSION_OFFSET)) >>
       L7_TACACS_MAJOR_VER_SHIFT) != L7_TACACS_MAJOR_VER)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE,  L7_TACACS_COMPONENT_ID, "TACACS+: invalid major version in"
												    " received packet. Major version mismatch.");
    tacacsContactNextServer(session);
    return;
  }
  if (((*(pkt + L7_TACACS_PKT_VERSION_OFFSET))
       & L7_TACACS_MINOR_VER_MASK) != L7_TACACS_MINOR_VER_DEFAULT)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE,  L7_TACACS_COMPONENT_ID, "TACACS+: invalid minor version in"
												    " received packet. Minor version mismatch.");
    tacacsContactNextServer(session);
    return;
  }
  if ((*(pkt + L7_TACACS_PKT_SEQ_NO_OFFSET)) != (session->sequenceNo + 1))
  {
    LOG_MSG("TACACS+: invalid sequece number in received packet.");
    tacacsContactNextServer(session);
    return;
  }

  msgLen = osapiNtohs(*((L7_ushort16 *)(pkt + L7_TACACS_REPLY_SRV_MSG_LEN_OFFSET)));
  dataLen = osapiNtohs(*((L7_ushort16 *)(pkt + L7_TACACS_REPLY_DATA_LEN_OFFSET)));
  if (rxPacket->length != (L7_TACACS_REPLY_FIXED_FIELDS + msgLen + dataLen))
  {
    LOG_MSG("TACACS+: length fields and packet length do not match.\n");
    tacacsContactNextServer(session);
    return;
  }

  /* process session based on status received */
  status = (*(pkt + L7_TACACS_REPLY_STATUS_OFFSET));
  if (type == L7_TACACS_AUTHEN_LOGIN)
  {
    switch (status)
    {
      case L7_TACACS_AUTHEN_STATUS_PASS:
        tacacsAuthSuccessSend(session);
        break;
      case L7_TACACS_AUTHEN_STATUS_FAIL:
        /* just send failure response back to requesting component */
        tacacsAuthFailureSend(session, TACACS_STATUS_AUTH_FAILURE);
        break;
      case L7_TACACS_AUTHEN_STATUS_GETPASS:
        tacacsAuthStatusGetPassProcess(session,
                                       pkt);
        break;
      case L7_TACACS_AUTHEN_STATUS_GETUSER:
        tacacsAuthStatusGetUserProcess(session, pkt);
        break;
      case L7_TACACS_AUTHEN_STATUS_GETDATA:
      case L7_TACACS_AUTHEN_STATUS_RESTART:
      case L7_TACACS_AUTHEN_STATUS_ERROR:
      case L7_TACACS_AUTHEN_STATUS_FOLLOW:
      default:
        LOG_MSG("TACACS+: received unsupported authentication reply status %i.\n",
                status);
        tacacsContactNextServer(session);
        break;
    }
  }
  else
  {
    switch (status)
    {
      case L7_TACACS_AUTHOR_STATUS_PASS_ADD:
        tacacsAuthSuccessSend(session);
        break;
      case L7_TACACS_AUTHOR_STATUS_PASS_REPL:
        /* if we ever need to do more based on priv-lvl,
           here is where we would parse the RESPONSE args */
      case L7_TACACS_AUTHOR_STATUS_FAIL:
        tacacsAuthFailureSend(session, TACACS_STATUS_AUTH_FAILURE);
        break;
      case L7_TACACS_AUTHOR_STATUS_ERROR:
      case L7_TACACS_AUTHOR_STATUS_FOLLOW:
      default:
        LOG_MSG("TACACS+: received unsupported authorization reply status %i.\n",
                status);
        tacacsContactNextServer(session);
        break;
    }
  }
  return;
}
