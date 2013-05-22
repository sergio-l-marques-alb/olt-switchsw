/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_local.c
*
* @purpose   dot1x local authentication routines
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*
**********************************************************************/

#include <time.h>
#include "dot1x_include.h"
#include "md5_api.h"

L7_RC_t dot1xLocalAuthSendAuthReq(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32 lIntIfNum = logicalPortInfo->logicalPortNumber;

  /* Generate an event that will cause a request (MD5 challenge) to be sent to the supplicant.
   * The bufHandle is L7_NULLPTR in this case since we didn't actually receive the request
   * from a server.  So, we will need to generate the request locally.
   */
  if (logicalPortInfo->isMABClient == L7_TRUE)
  {
    /* if MAB client generate and store dot1x challenge*/
    memset(logicalPortInfo->dot1xChallenge,0,DOT1X_CHALLENGE_LEN);
    dot1xLocalAuthChallengeGenerate(logicalPortInfo->dot1xChallenge, DOT1X_CHALLENGE_LEN);
    logicalPortInfo->dot1xChallengelen = DOT1X_CHALLENGE_LEN;
  
  }
  return dot1xStateMachineClassifier(bamAReq, lIntIfNum, L7_NULL, L7_NULLPTR);
}

/**************************************************************************
* @purpose   Process EAP Response and Response/Identity frames
*
* @param     logicalPortInfo  @b{(input))  Logical Port Info node
* @param     bufHandle  @b{(input)} the handle to the dot1x PDU
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xLocalAuthResponseProcess(dot1xLogicalPortInfo_t *logicalPortInfo, L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt;
  L7_BOOL portAllow;
  L7_RC_t rc = L7_SUCCESS, authRc;
  dot1xPortCfg_t * pCfg;
  L7_uint32 physPort = logicalPortInfo->physPort;
  L7_uint32 lIntIfNum = logicalPortInfo->logicalPortNumber;
  dot1xFailureReason_t  reason;
  L7_int32   reasonCode=L7_NULL;
  L7_uint32  portStatus;

  if (dot1xIntfIsConfigurable(physPort, &pCfg) != L7_TRUE)
     return L7_FAILURE;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  enetHdr = (L7_enetHeader_t *)data;
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE);
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));

  logicalPortInfo->idFromServer = logicalPortInfo->currentIdL;

  if (eapRrPkt->type == EAP_RRIDENTITY)
  {
    logicalPortInfo->dot1xUserIndex = DOT1X_USER_INDEX_INVALID;
    /*
     * Verify if user entry exists in IAS user database
     */
    if (L7_AUTH_METHOD_IAS == logicalPortInfo->authMethod)
    {
      if (L7_TRUE == cnfgrIsComponentPresent(L7_DOT1X_AUTH_SERV_COMPONENT_ID))
      {
        if (dot1xAuthServUserDBUserIndexGet((L7_char8 *)logicalPortInfo->dot1xUserName,
                                 &logicalPortInfo->dot1xUserIndex) != L7_SUCCESS)
        {
          reasonCode = DOT1X_AUTH_REASON_LOCAL_AUTH_USER_NOT_FOUND;
          logicalPortInfo->dot1xUserIndex = DOT1X_USER_INDEX_INVALID;
        }
      }
    }
    else if (L7_AUTH_METHOD_LOCAL == logicalPortInfo->authMethod)
    {

     /* User name was stored in port info structure when EAP-Response/Identity frame was received.
      * See if this user name matches a locally configured user.  If we find a match, generate an
      * event that will cause an MD5 challenge to be sent to the supplicant.  If we don't find a
      * match, generate an authentication failure event.
      */
      if (userMgrLoginIndexGet((L7_char8 *)logicalPortInfo->dot1xUserName,
                             &logicalPortInfo->dot1xUserIndex) != L7_SUCCESS)
      {
        reasonCode = DOT1X_AUTH_REASON_LOCAL_AUTH_USER_NOT_FOUND;
        logicalPortInfo->dot1xUserIndex = DOT1X_USER_INDEX_INVALID;
      }
    }

    if (logicalPortInfo->dot1xUserIndex != DOT1X_USER_INDEX_INVALID)
    {
      /* Make sure user has access to this port */
      if (userMgrPortUserAccessGet(physPort, logicalPortInfo->dot1xUserName, &portAllow) == L7_SUCCESS) 
      {
        if(portAllow == L7_TRUE) 
        {
          rc = dot1xLocalAuthSendAuthReq(logicalPortInfo);
        }
        else
        {
          reasonCode = DOT1X_AUTH_REASON_LOCAL_AUTH_USER_NO_ACCESS;
        }
      }
      else
      {
        reasonCode = DOT1X_AUTH_REASON_LOCAL_AUTH_USER_NOT_FOUND;
      } 
    }
  }
  else if (eapRrPkt->type == EAP_RRMD5)
  {
    L7_uchar8 *response = ((L7_uchar8 *)eapRrPkt) + sizeof(L7_eapRrPacket_t);
    authRc = dot1xLocalAuthMd5ResponseValidate(logicalPortInfo, response);
    if (authRc == L7_SUCCESS)
    {
      /*reset unauthenticated Vlan*/
      if (logicalPortInfo->unauthVlan !=0)
      {
        dot1xCtlLogicalPortUnauthenticatedVlanReset(logicalPortInfo->logicalPortNumber);
      }

      if ((logicalPortInfo->isMABClient == L7_TRUE) && (logicalPortInfo->guestVlanId != 0))
      {
        dot1xCtlLogicalPortMABGuestVlanReset(logicalPortInfo->logicalPortNumber);
      }

      if ((logicalPortInfo->isMonitorModeClient == L7_TRUE) && (logicalPortInfo->defaultVlanId != 0))
      {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE, logicalPortInfo->physPort,
                   "\n%s: Resetting Monitor Mode VLAN %u for port %u -  \n",
                    __FUNCTION__,logicalPortInfo->defaultVlanId, logicalPortInfo->physPort);

        dot1xCtlLogicalPortMonitorModeVlanReset(logicalPortInfo->logicalPortNumber);
      }

      rc = dot1xStateMachineClassifier(bamASuccess, lIntIfNum, L7_NULL, L7_NULLPTR);
        
      /* Log the information */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                 "Local Dot1x Authenticated Successfully.");
      dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                              DOT1X_AUTH_REASON_LOCAL_SUCCESS,
                              L7_DOT1X_PORT_STATUS_AUTHORIZED, logicalPortInfo);
    }
    else
    {
      /* FAILED Unauthenticated vlan needs to be assigned */
      reason =  DOT1X_FAIL_REASON_AUTH_FAILED;
      if((rc = dot1xCtlPortUnauthenticatedVlanOperational(logicalPortInfo,reason))==L7_FAILURE)
      {
        reasonCode = DOT1X_AUTH_REASON_LOCAL_AUTH_MD5_VALIDATION_FAILURE;
      }
    }
  }
  else
  {
    /* Since we're doing local authentication here, only Response/Identity and Response/MD5
     * are valid... so send a failure to Supplicant.
     */
    reasonCode = DOT1X_AUTH_REASON_LOCAL_AUTH_INVALID_EAP_TYPE;
  }

  if(reasonCode != L7_NULL)
  {
    /* reset port in any dot1x assigned vlans*/
    dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

    /* If Monitor Mode is enabled, Log an information */
    if((dot1xCfg->monitorMode == L7_ENABLE) &&
       (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
    {
      portStatus = L7_DOT1X_PORT_STATUS_AUTHORIZED;
    }
    else
    {
      rc = dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      portStatus = L7_DOT1X_PORT_STATUS_UNAUTHORIZED;
    }
    
    /* Log the information */
    switch(reasonCode)
    {
      case DOT1X_AUTH_REASON_LOCAL_AUTH_USER_NOT_FOUND:
           L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "Failed to Process Local Auth Response, User Not Found."
              "%s",
              (portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)?" Authenticated through Monitor Mode.":"");
           break;
      case DOT1X_AUTH_REASON_LOCAL_AUTH_USER_NO_ACCESS: 
           L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
             "Failed to Process Local Auth Response, User does not have access."
             "%s",
             (portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)?" Authenticated through Monitor Mode.":"");
           break;
      case DOT1X_AUTH_REASON_LOCAL_AUTH_MD5_VALIDATION_FAILURE:
           L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
             "Failed to process Local Auth Response - MD5 Validation Failure."
             "%s",
             (portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)?" Authenticated through Monitor Mode.":"");
           break;
      case DOT1X_AUTH_REASON_LOCAL_AUTH_INVALID_EAP_TYPE:
           L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
             "Failed to process Local Auth Response - Invalid EAP Type."
             "%s", 
             (portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)?" Authenticated through Monitor Mode.":"");
           break;
    } 
    dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, reasonCode, portStatus, logicalPortInfo);
  }

  SYSAPI_NET_MBUF_FREE(bufHandle);
  return rc;
}

/**************************************************************************
* @purpose   Validate an MD5 challenge response
*
* @param     logicalPortInfo  @b{(input))  Logical Port Info node
* @param     *response  @b{(input)} pointer to MD5 response data
*
* @returns   L7_SUCCESS if authenticated
* @returns   L7_FAILURE if not authenticated
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xLocalAuthMd5ResponseValidate(dot1xLogicalPortInfo_t *logicalPortInfo, L7_uchar8 *response)
{
  L7_uint32 suppResponseLen, responseDataLen;
  L7_uchar8 suppAnswer[DOT1X_MD5_LEN], answer[DOT1X_MD5_LEN];
  L7_uchar8 responseData[L7_PASSWORD_SIZE+DOT1X_CHALLENGE_LEN+1];
  L7_uchar8 userPw[L7_PASSWORD_SIZE];
  L7_RC_t rc = L7_FAILURE;

  /* Length is first byte in response field */
  suppResponseLen = (L7_uint32)(*response);
  if (suppResponseLen != DOT1X_MD5_LEN)
    return L7_FAILURE;

  if (logicalPortInfo->dot1xUserIndex == DOT1X_USER_INDEX_INVALID)
    return L7_FAILURE;

  memcpy(suppAnswer, &response[1], suppResponseLen);

  memset(userPw, 0x0, L7_PASSWORD_SIZE);

  if (L7_AUTH_METHOD_IAS == logicalPortInfo->authMethod)
  {
    if (L7_TRUE == cnfgrIsComponentPresent(L7_DOT1X_AUTH_SERV_COMPONENT_ID))
    {
      if (dot1xAuthServUserDBUserPasswordClearTextGet(logicalPortInfo->dot1xUserIndex, (L7_char8 *)userPw, sizeof(userPw)) != L7_SUCCESS)
        return L7_FAILURE;
    }
  }
  else if (L7_AUTH_METHOD_LOCAL == logicalPortInfo->authMethod)
  {
  if (userMgrLoginUserPasswordClearTextGet(logicalPortInfo->dot1xUserIndex, (L7_char8 *)userPw) != L7_SUCCESS)
    return L7_FAILURE; 
  } 

  responseDataLen = 1 + strlen((L7_char8 *)userPw) + DOT1X_CHALLENGE_LEN;

  memset(responseData,0,sizeof(responseData));
  responseData[0] = logicalPortInfo->currentIdL;
  memcpy(&responseData[1], userPw, strlen((L7_char8 *)userPw));
  memcpy(&responseData[1+strlen((L7_char8 *)userPw)], logicalPortInfo->dot1xChallenge, DOT1X_CHALLENGE_LEN);

  dot1xLocalMd5Calc(responseData, responseDataLen, answer);

  if (memcmp(answer, suppAnswer, DOT1X_MD5_LEN) == 0)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}

/**************************************************************************
* @purpose   Generate an random challenge
*
* @param     *challenge     @b{(output)} pointer to buffer to hold challenge
* @param     challengeLen   @b{(input)} length of challenge buffer
*
* @returns   void
*
* @comments
*
* @end
*************************************************************************/
void dot1xLocalAuthChallengeGenerate(L7_uchar8 *challenge, L7_uint32 challengeLen)
{
  L7_uint32 i, randno, cpLen;
  L7_uchar8 *vector = &challenge[0];

  bzero((L7_uchar8 *)vector, challengeLen);

	srand((L7_uint32)time(0));
	for (i = 0; i < challengeLen;)
	{
    cpLen = (L7_uint32)sizeof(L7_uint32);

		randno = (L7_uint32)rand();
		memcpy ((L7_uchar8 *) vector, (L7_uchar8 *) &randno, cpLen);
		vector += cpLen;
		i += cpLen;
	}

  return;
}

/**************************************************************************
* @purpose   Calculate MD5 for given input buffer and length
*
* @param     *inBuf   @b{(input)} pointer to input buffer run MD5 against
* @param     inLen    @b{(input)} length of input buffer
* @param     *outBuf  @b{(output)} pointer to output buffer to hold MD5 result
*
* @returns   void
*
* @comments  *outBuf must be DOT1X_MD5_LEN (16) bytes
*
* @end
*************************************************************************/
void dot1xLocalMd5Calc(L7_uchar8 *inBuf, L7_uint32 inLen, L7_uchar8 *outBuf)
{
  L7_MD5_CTX_t context;

  bzero(outBuf, DOT1X_MD5_LEN);

  l7_md5_init(&context);
  l7_md5_update(&context, inBuf, inLen);
  l7_md5_final(outBuf, &context);

  return;
}
