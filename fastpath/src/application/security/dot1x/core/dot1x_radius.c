/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_radius.c
*
* @purpose   dot1x RADIUS client interface
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
**********************************************************************/

#define L7_MAC_EAPOL_PDU

#include "dot1x_include.h"
#include "dot1q_api.h"

extern dot1xCnfgrState_t dot1xCnfgrState;

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
L7_RC_t dot1xRadiusResponseCallback(L7_uint32 status, L7_uint32 correlator,
                                      L7_uchar8 *attributes, L7_uint32 attributesLen)
{
  L7_uint32 lIntIfNum;
  dot1xAaaMsg_t dot1xAaaMsg;

  lIntIfNum = correlator;

  /* Fill in AAA message structure */
  bzero((L7_uchar8 *)&dot1xAaaMsg, sizeof(dot1xAaaMsg_t));
  dot1xAaaMsg.status = status;
  dot1xAaaMsg.respLen = attributesLen;

  /* Allocate memory for attributes which gets freed by dot1xTask */
  dot1xAaaMsg.pResponse = osapiMalloc(L7_DOT1X_COMPONENT_ID, attributesLen);
  memcpy(dot1xAaaMsg.pResponse, attributes, attributesLen);

  return dot1xIssueCmd(dot1xAaaInfoReceived, lIntIfNum, &dot1xAaaMsg);
}

/**************************************************************************
* @purpose   Process RADIUS Server responses
*
* @param     lIntIfNum       @b{(input)} Logical internal interface number of port being authenticated
* @param     status         @b{(input)} status of RADIUS response (accept, reject, challenge, etc)
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
L7_RC_t dot1xRadiusResponseProcess(L7_uint32 lIntIfNum, L7_uint32 status, L7_uchar8 *attributes,
                                   L7_uint32 attributesLen)
{
  L7_RC_t rc = L7_FAILURE;
  dot1xFailureReason_t  reason;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort, reasonCode;

  if (!(DOT1X_IS_READY))
    return L7_SUCCESS;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  physPort = logicalPortInfo->physPort;

  if (dot1xIntfIsConfigurable(physPort, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,
                    "%s:Recieved Radius response message on logicalPort:[%d] with status[%d]\n\r",
                    __FUNCTION__,lIntIfNum, status);

  /* Ensure we are expecting a response from the server */
  if (logicalPortInfo->bamState == BAM_RESPONSE)
  {
    switch (status)
    {
    case RADIUS_STATUS_SUCCESS:
      rc = dot1xRadiusAcceptProcess(lIntIfNum, attributes, attributesLen);
      break;

    case RADIUS_STATUS_CHALLENGED:
      rc = dot1xRadiusChallengeProcess(lIntIfNum, attributes, attributesLen);
      break;

    case RADIUS_STATUS_AUTHEN_FAILURE:
         reason =  DOT1X_FAIL_REASON_AUTH_FAILED;
         dot1xRadiusSendNotification(lIntIfNum, attributes, attributesLen);

        if((rc=dot1xCtlPortUnauthenticatedVlanOperational(logicalPortInfo,reason))==L7_FAILURE)
        {
          /* reset port in any dot1x assigned vlans */
          dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);
           
          /* If Monitor Mode is enabled, Authenticate the client on Monitor Mode VLAN */
          if((dot1xCfg->monitorMode == L7_ENABLE) &&
             (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
          {
            /* Log the information */
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                    "Radius Authentication Failed on physPort:[%d] lIntIfNum:[%d]. Authenticated through Monitor Mode\n\r",
                    logicalPortInfo->physPort, lIntIfNum);
            dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, DOT1X_AUTH_REASON_RADIUS_AUTH_FAILURE,
                                    L7_DOT1X_PORT_STATUS_AUTHORIZED, logicalPortInfo);
            break;
          }
          /* Log the information */
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                  "Radius Authentication Failed on physPort:[%d] lIntIfNum:[%d].", logicalPortInfo->physPort, lIntIfNum);
          dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, DOT1X_AUTH_REASON_RADIUS_AUTH_FAILURE,
                                  L7_DOT1X_PORT_STATUS_UNAUTHORIZED, logicalPortInfo);
          rc = dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
        }
        else
        {
          /* Log the information */
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                 "Radius Authenticated through unauthenticated VLAN on physPort:[%d] lIntIfNum:[%d].\n\r",
                  logicalPortInfo->physPort, lIntIfNum);
           
          dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, DOT1X_AUTH_REASON_RADIUS_AUTH_FAILURE,
                                  L7_DOT1X_PORT_STATUS_AUTHORIZED, logicalPortInfo);
        } 
    break;
    case RADIUS_STATUS_REQUEST_TIMED_OUT:
    case RADIUS_STATUS_COMM_FAILURE:

      DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,
          "Reason:'%s' on physPort:[%d] lIntIfNum:[%d]\n\r",
                       (status == RADIUS_STATUS_COMM_FAILURE)?"RADIUS_STATUS_COMM_FAILURE":"RADIUS_STATUS_REQUEST_TIMED_OUT",
                       physPort, lIntIfNum);

      dot1xRadiusSendNotification(lIntIfNum, attributes, attributesLen);

      /* reset port in any dot1x assigned vlans*/
      dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

      if(status == RADIUS_STATUS_REQUEST_TIMED_OUT)
      {
        reasonCode = DOT1X_AUTH_REASON_RADIUS_REQ_TIMEOUT;
      }
      else
      {
        reasonCode = DOT1X_AUTH_REASON_RADIUS_AUTH_COMM_FAILURE;
      }
      
      /* If Monitor Mode is enabled, Authenticate the client on Monitor Mode VLAN */
      if((dot1xCfg->monitorMode == L7_ENABLE)
         && (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
      {
        /* Log the information */
        if(reasonCode == DOT1X_AUTH_REASON_RADIUS_REQ_TIMEOUT)
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
          "Failed to authenticate on lIntIfNum:[%d] - Radius Request Timed Out. Authenticated through Monitor Mode", lIntIfNum);
        }
        else
        { 
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
           "Failed to authenticate on lIntIfNum:[%d] - Radius Comm Failure. Authenticated through Monitor Mode", lIntIfNum);
        }
        dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, reasonCode, L7_DOT1X_PORT_STATUS_AUTHORIZED, logicalPortInfo);
        break;
      }
      /* Log the information */
      if(reasonCode == DOT1X_AUTH_REASON_RADIUS_REQ_TIMEOUT)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "Failed to authenticate on lIntIfNum:[%d] - Radius Request Timed Out.", lIntIfNum);
      }
      else
      { 
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "Failed to authenticate on lIntIfNum:[%d] - Radius Comm Failure.", lIntIfNum);
      }

      dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, reasonCode, L7_DOT1X_PORT_STATUS_UNAUTHORIZED, logicalPortInfo);
      rc = dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      break;

    default:
      rc = L7_FAILURE;
      /* Log the information */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "Failed to authenticate on lIntIfNum:[%d] - Received Invalid Radius Status Type", lIntIfNum);
      dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                              DOT1X_AUTH_REASON_RADIUS_INVALID_RADIUS_STATUS, 
                              L7_DOT1X_PORT_STATUS_NA, logicalPortInfo);
      break;
    }
  }

  /* Need to free the buffer that was passed to us */
  if (attributes != L7_NULLPTR)
    osapiFree(L7_DOT1X_COMPONENT_ID, attributes);

  return rc;
}

/**************************************************************************
* @purpose   Process RADIUS Accept from server
*
* @param     lIntIfNum       @b{(input)} Logical internal interface number of port being authenticated
* @param     *radiusPayload  @b{(input)} payload of RADIUS frame (attributes)
* @param     payloadLen      @b{(input)} length of RADIUS payload
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xRadiusAcceptProcess(L7_uint32 lIntIfNum, L7_uchar8 *radiusPayload, L7_uint32 payloadLen)
{
  L7_radiusAttr_t *radiusAttr;
  L7_uint32 attrVal, ifIndex=0;
  L7_eapPacket_t *eapPkt;
  L7_BOOL done = L7_FALSE, gotSessionTimeout = L7_FALSE, gotTermAction = L7_FALSE, gotEap = L7_FALSE;
  L7_uint32 rc = L7_SUCCESS;
  L7_uint32 vlanAssigned=0,vlanAssignmentFlag=0;
  dot1xPortCfg_t *pCfg;
  L7_char8 *tempradiusPayload=radiusPayload;
  L7_uint32 tempPayloadLen =payloadLen;
  L7_BOOL filterIdAssignedFlag = L7_FALSE;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort;
  L7_uint32 curr_unauth_vlan, reasonCode=0;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  physPort = logicalPortInfo->physPort;

  if (dot1xIntfIsConfigurable(physPort, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,
                    "%s:Recieved Radius Accept message for logical - %d \n",
                    __FUNCTION__,lIntIfNum);

  logicalPortInfo->terminationAction = RADIUS_TERMINATION_ACTION_DEFAULT;
  logicalPortInfo->sessionTimeout = 0;

  radiusAttr = (L7_radiusAttr_t *)radiusPayload;

  /* if reauthenticating and client was in unauthenticated vlan and is now
   * authenticated, then remove from unauthenticated vlan */
  if (logicalPortInfo->unauthVlan != 0)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL, physPort,
                      "%s:In Radius Accept message removing port %u from unauthenticated vlan %u  -  \n",
                      __FUNCTION__, lIntIfNum, logicalPortInfo->unauthVlan);



    curr_unauth_vlan = logicalPortInfo->unauthVlan;
    dot1xCtlLogicalPortUnauthenticatedVlanReset(lIntIfNum);

    if (logicalPortInfo->vlanAssigned == 0)
    {
      /* port was in unauthenticated vlan and if radius has not assigned a vlan, maybe put in default vlan*/
      /*set reauth_auth_apply*/
      if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
      {
        logicalPortInfo->reauth_auth_apply = L7_TRUE;
      }
    }
  }

  if ((logicalPortInfo->isMABClient) && (logicalPortInfo->guestVlanId != 0))
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL, physPort,
                      "%s:In Radius Accept message removing port %u from MAB vlan %u  -  \n",
                      __FUNCTION__, lIntIfNum, logicalPortInfo->guestVlanId);
    dot1xCtlLogicalPortMABGuestVlanReset(lIntIfNum);
    logicalPortInfo->reauth_auth_apply = L7_TRUE;
  }

  while (done == L7_FALSE)
  {
    if (radiusAttr->type == RADIUS_ATTR_TYPE_NAS_PORT) /* NAS-Port */
    {
      /* Verify that the NAS-Port is equal to the port we are authenticating */
      /*if (nimGetIntfIfIndex(intIfNum, &ifIndex) == L7_SUCCESS)*/
      ifIndex = lIntIfNum;

      memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));

      if (attrVal != ifIndex)
      {
        /* reset port in any dot1x assigned vlans*/
        dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

        /* If Monitor Mode is enabled, Authenticate the client on Monitor Mode VLAN */
        if((dot1xCfg->monitorMode == L7_ENABLE) &&
           (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
        {
          /* Log the information */
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "Failed to authenticate - Received Invalid NAS Port, Authenticating through Monitor Mode");
          dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                  DOT1X_AUTH_REASON_RADIUS_ACCEPT_PROCESS_INVALID_NAS_PORT,
                                  L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  logicalPortInfo);
          return L7_SUCCESS;
        }

        /* Log the information */
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "Failed to authenticate - Received Invalid NAS Port");
        dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                DOT1X_AUTH_REASON_RADIUS_ACCEPT_PROCESS_INVALID_NAS_PORT,
                                L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                                logicalPortInfo);
        return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
    }
    if (radiusAttr->type == RADIUS_ATTR_TYPE_REPLY_MESSAGE)
    {
      /* send EAP/Notification for reply message */
      dot1xRadiusSendNotification(lIntIfNum, tempradiusPayload, tempPayloadLen);
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_STATE) /* State */
    {
      memset(logicalPortInfo->serverState,0, DOT1X_SERVER_STATE_LEN);
      memcpy(logicalPortInfo->serverState,
             (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));
      logicalPortInfo->serverStateLen = radiusAttr->length - sizeof(L7_radiusAttr_t);
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_SESSION_TIMEOUT) /* Session-Timeout */
    {
      /* Set the sessionTimeout value which will be picked up by the timer state machine */
      memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));
      logicalPortInfo->sessionTimeout = attrVal;
      gotSessionTimeout = L7_TRUE;
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_TERMINATION_ACTION) /* Termination-Action */
    {
      /* Set the terminationAction value which will be picked up by the timer state machine */
      memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));
      logicalPortInfo->terminationAction = attrVal;
      gotTermAction = L7_TRUE;
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_EAP_MESSAGE) /* EAP message */
    {
      /* If this is the first EAP msg in the frame, save the ID and set flag. */
      if (gotEap == L7_FALSE)
      {
        eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t));
        logicalPortInfo->idFromServer = eapPkt->id;
        gotEap = L7_TRUE;
      }
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_VENDOR) /* Vendor-Specific */
    {
      /* Process Vendor-Specific attributes here.  For example, MS-MPPE-Send-Key and
       * MS-MPPE-Recv-Key would be handle here for platforms that support encrypting
       * frames between NAS (i.e. Access Point) and client (Supplicant).
       */
      while(0);
    }

    if(radiusAttr->type == RADIUS_ATTR_TYPE_FILTER_ID)
    {
      if(radiusAttr->length > sizeof(L7_radiusAttr_t))
      {
        /* check if any radius assigned parameters have changed while reauthenticating or client restarting*/

        if ((dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)&&
            ((logicalPortInfo->reAuthenticating == L7_TRUE) ||
             (logicalPortInfo->suppRestarting == L7_TRUE))&&
            (memcmp(logicalPortInfo->filterName,(L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
                    radiusAttr->length - sizeof(L7_radiusAttr_t))!=0))
        {
          logicalPortInfo->reauth_auth_apply = L7_TRUE;
        }
        memcpy(logicalPortInfo->filterName,(L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
               radiusAttr->length - sizeof(L7_radiusAttr_t));
        filterIdAssignedFlag = L7_TRUE;
      }
    }

    if(radiusAttr->type == RADIUS_ATTR_TYPE_TUNNEL_TYPE)
    {
      /* get the tunnel type */
      memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));
      if((attrVal & 0x00FFFFFF) == RADIUS_TUNNEL_TYPE_VLAN)
      {
        vlanAssignmentFlag |= 0x1;
      }
    }

    if(radiusAttr->type == RADIUS_ATTR_TYPE_TUNNEL_MEDIUM_TYPE)
    {
      /* get the tunnel type */
      memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));
      if((attrVal & 0x00FFFFFF) == RADIUS_TUNNEL_MEDIUM_TYPE_802)
      {
        vlanAssignmentFlag |= 0x2;
      }
    }

    if(radiusAttr->type == RADIUS_ATTR_TYPE_TUNNEL_PRIVATE_GROUP_ID)
    {
      L7_uchar8  vlanString[DOT1X_RADIUS_VLAN_ASSIGNED_LEN+1];
      L7_uchar8  tagField;
      L7_uint32  len=1;
      L7_uint32  stringLen=0;    
      /* get the tunnel type */
      memset(vlanString,0,sizeof(vlanString));
      /* ignore the tag 1 byte */
      memcpy((L7_uchar8 *)&tagField,(L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),1);

      if(tagField > 0x1F)   /* RFC 2868 Section 3.6 */
      {
        len = 0;
      }

      stringLen=(DOT1X_RADIUS_VLAN_ASSIGNED_LEN < ((radiusAttr->length - sizeof(L7_radiusAttr_t))-len))?
                DOT1X_RADIUS_VLAN_ASSIGNED_LEN:((radiusAttr->length - sizeof(L7_radiusAttr_t))-len);
  
      /* get the vlan id */
      stringLen = strlen((L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t)+len);

      if(stringLen > DOT1X_RADIUS_VLAN_ASSIGNED_LEN)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                "dot1xRadiusAcceptProcess: Invalid Vlan Id - %s received from Radius Server",
                vlanString);
        /* reset port in any dot1x assigned vlans*/
        dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);
        /* send failure event - as vlan assignment failed*/
        return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      }

      memcpy((L7_uchar8 *)vlanString, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t)+len,
             stringLen);

      vlanAssignmentFlag |= 0x4;
      #if L7_FEAT_NAME_BASED_VLAN_ASSIGNMENT
      if(dot1xRadiusServerVlanConversionHandle(vlanString, &vlanAssigned) == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                "dot1xRadiusAcceptProcess: Invalid Vlan Id - %s received from Radius Server",
                vlanString);

        /* reset port in any dot1x assigned vlans*/
        dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

        /* send failure event - as vlan assignment failed*/
        return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
      #else
      vlanAssigned = atoi(vlanString);
      #endif
    }

    payloadLen -= radiusAttr->length;
    if (payloadLen <= (L7_uint32)sizeof(L7_radiusAttr_t))
    {
      if (gotEap == L7_FALSE)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
               "dot1xRadiusAcceptProcess: EAP message not received from server."
               " RADIUS server did not send required EAP message");
        /* reset port in any dot1x assigned vlans*/
        dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);


        /* If Monitor Mode is enabled, Authenticate the client on Monitor Mode VLAN */
        if((dot1xCfg->monitorMode == L7_ENABLE) &&
           (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
        {
          /* Log the information */
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "Failed to authenticate - EAP message not received from server, Authenticating through Monitor Mode");
          dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                  DOT1X_AUTH_REASON_RADIUS_ACCEPT_PROCESS_WRONG_EAP_MSG,
                                  L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  logicalPortInfo);
          return L7_SUCCESS; 
        }

        /* Log the information */
        dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                DOT1X_AUTH_REASON_RADIUS_ACCEPT_PROCESS_WRONG_EAP_MSG,
                                L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                                logicalPortInfo);
        return( dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR));
      }
      done = L7_TRUE;
    }
    else
    {
      radiusAttr = (L7_radiusAttr_t *)((L7_uchar8 *)radiusAttr + radiusAttr->length);
    }

    if (radiusAttr->length <= 0)
    {
      done = L7_TRUE;
    }
  }

  /* apply the filter id */
  if(filterIdAssignedFlag == L7_TRUE)
  {
    /* check if Diffserv in QoS package present */
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                              L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE)
    {
      if (dot1xApplyAssignedPolicy(physPort,vlanAssigned,
                                   logicalPortInfo->suppMacAddr.addr,
                                   logicalPortInfo->filterName,
                                   L7_TRUE) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                "dot1xRadiusAcceptProcess: error in applying the filter id %s."
                " Error getting corresponding Diffserv policy ID for RADIUS "
                "assigned Filter ID.", logicalPortInfo->filterName);

        /* reset port in any dot1x assigned vlans*/
        dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

        /* If Monitor Mode is enabled, Authenticate the client on Monitor Mode VLAN */
        if((dot1xCfg->monitorMode == L7_ENABLE) &&
           (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
        {
          /* Log the information */
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                 "Filter Assignment failure. Authenticating through Monitor Mode");
          dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                  DOT1X_AUTH_REASON_RADIUS_ACCEPT_FILTER_ASSIGNMENT_FAILURE,
                                  L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  logicalPortInfo);
          return L7_SUCCESS;
        }

        /* Log the information */
        dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                DOT1X_AUTH_REASON_RADIUS_ACCEPT_FILTER_ASSIGNMENT_FAILURE,
                                L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                                logicalPortInfo);

        /* send failure event - as vlan assignment failed*/
        return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "dot1xRadiusAcceptProcess: Cannot apply the filter id %s as Diffserv is not present."
              " Cannot apply RADIUS assigned Filter ID , if Diffserv is not present",
              logicalPortInfo->filterName);
      memset(logicalPortInfo->filterName,0,DOT1X_FILTER_NAME_LEN);
      return L7_SUCCESS;
    }
#else
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xRadiusAcceptProcess: Cannot apply the filter id %s as Diffserv is not present."
            " Cannot apply RADIUS assigned Filter ID , if Diffserv is not present",
            logicalPortInfo->filterName);
    memset(logicalPortInfo->filterName,0,DOT1X_FILTER_NAME_LEN);

    /* reset port in any dot1x assigned vlans*/
    dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

    /* If Monitor Mode is enabled, Authenticate the client on Monitor Mode VLAN */
    if((dot1xCfg->monitorMode == L7_ENABLE) &&
       (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
    {
      /* Log the information */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "Diffserv Not Present. Authenticating through Monitor Mode");
      dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                              DOT1X_AUTH_REASON_RADIUS_ACCEPT_DIFFSERV_NOT_PRESENT,
                              L7_DOT1X_PORT_STATUS_AUTHORIZED,
                              logicalPortInfo);
      return L7_SUCCESS;
    }
      
    /* Log the information */
    dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                            DOT1X_AUTH_REASON_RADIUS_ACCEPT_DIFFSERV_NOT_PRESENT,
                            L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                            logicalPortInfo);

    /* send failure event - as filter Id assignment failed*/
    return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
#endif
  }
  else
  {
    /*No Filter ID assignment*/

    /* Check if filter ID was assigned before reauthentication,and if it was reset the fiter ID.*/
    if ((dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)&&
        ((logicalPortInfo->reAuthenticating == L7_TRUE) ||
         (logicalPortInfo->suppRestarting == L7_TRUE)))
    {
      L7_uchar8    emptyString[DOT1X_FILTER_NAME_LEN];
      memset(emptyString,0,DOT1X_FILTER_NAME_LEN);
      if (memcmp(logicalPortInfo->filterName,emptyString,DOT1X_FILTER_NAME_LEN) !=0)
      {
        /* reset the filter name*/
        memset(logicalPortInfo->filterName,0,DOT1X_FILTER_NAME_LEN);
        logicalPortInfo->reauth_auth_apply = L7_TRUE;
      }
    }
  }

  /* Assign the vlan given by radius */
  if(vlanAssignmentFlag == 0x7)
  {
    L7_uint32 vlanMode;
    dot1xVlanAssignmentModeGet(&vlanMode);

    if(vlanMode == L7_ENABLE)
    {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,physPort,
                        "%s:Recieved vlan Id : %u for logical port:%u  -  \n",
                        __FUNCTION__,vlanAssigned,lIntIfNum);
      /* check if any radius assigned parameters have changed while reauthenticating */
      if (((logicalPortInfo->reAuthenticating == L7_TRUE) ||
           (logicalPortInfo->suppRestarting == L7_TRUE))&&
          (logicalPortInfo->vlanAssigned !=0) &&
          (logicalPortInfo->vlanAssigned != vlanAssigned))
      {
        dot1xCtlLogicalPortVlanAssignedReset(logicalPortInfo->logicalPortNumber);
        if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
        {
          logicalPortInfo->reauth_auth_apply = L7_TRUE;
        }

      }
      if(dot1xRadiusServerVlanAssignmentHandle(logicalPortInfo->physPort,vlanAssigned) == L7_FAILURE ||
         dot1xVlanAssignmentEnable(logicalPortInfo,vlanAssigned) == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                "dot1xRadiusAcceptProcess: error in assigning VLAN %u to port/client.",
                vlanAssigned);

        if(logicalPortInfo->vlanAssigned !=0) 
        {
          (void)dot1xCtlLogicalPortVlanAssignedReset(lIntIfNum);
        }

        /* If Monitor Mode is enabled, Authenticate the client on Monitor Mode VLAN */
        if((dot1xCfg->monitorMode == L7_ENABLE) &&
           (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "VLAN Assignment Failure. Authenticated through Monitor Mode");
          /* Log the information */
          dot1xAuthHistoryInfoLog(L7_NULL, vlanAssigned, 
                                  DOT1X_AUTH_REASON_RADIUS_ACCEPT_VLAN_ASSIGNMENT_FAILURE,
                                  L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  logicalPortInfo);
          return L7_SUCCESS;
        }
          
        /* Log the information */
        dot1xAuthHistoryInfoLog(L7_NULL, vlanAssigned, 
                                DOT1X_AUTH_REASON_RADIUS_ACCEPT_VLAN_ASSIGNMENT_FAILURE,
                                L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                                logicalPortInfo);

        /* send failure event - as vlan assignment failed*/
        return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
      else
      {
        if ((dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED) &&
            ((logicalPortInfo->reAuthenticating == L7_TRUE) ||
             (logicalPortInfo->suppRestarting == L7_TRUE)))
        {
          if (logicalPortInfo->vlanId != 0)
          {
            logicalPortInfo->vlanId = 0;
          }
          if (logicalPortInfo->reauth_auth_apply != L7_TRUE)
            logicalPortInfo->reauth_auth_apply = L7_TRUE;
        }
      }
    }
    else
    {
      if(logicalPortInfo->vlanAssigned != 0) 
      {
        (void)dot1xCtlLogicalPortVlanAssignedReset(lIntIfNum);
      }
      /* Log the information that VLAN Assignment is not enabled on the switch */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "VLAN Assignment Feature is not enabled. Dot1x Server assigned VLANID not applied!");
      reasonCode = DOT1X_AUTH_REASON_VLAN_ASSIGNMENT_FEATURE_NOT_ENABLED;
    
    }
  }
  else
  {
    /*No Radius vlan assignemnt*/
    if (logicalPortInfo->vlanAssigned != 0)
    {
      /* remove port from old radius assigned vlan , since port should now be in default vlan */
      dot1xCtlLogicalPortVlanAssignedReset(logicalPortInfo->logicalPortNumber);

      if ((dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED) &&
          ((logicalPortInfo->reAuthenticating == L7_TRUE) ||
           (logicalPortInfo->suppRestarting == L7_TRUE)))
      {
        logicalPortInfo->reauth_auth_apply = L7_TRUE;
      }
    }
  }

  /* Issue the EAP-Success before processing the sessionTimeout == 0 below */
  rc =  dot1xStateMachineClassifier(bamASuccess, lIntIfNum, L7_NULL, L7_NULLPTR);

  if (rc == L7_SUCCESS)
  {
    /* Per RFC2865, when an Access-Accept includes a Termination-Action Attribute
     * with the value of RADIUS-Request, the State value needs to be included in
     * a new Access-Request that is sent upon termination of the current session.
     * If this is not the case, zero-out any State value we have stored.
     */
    if (gotTermAction == L7_FALSE ||
        logicalPortInfo->terminationAction != RADIUS_TERMINATION_ACTION_RADIUS)
    {
      memset(logicalPortInfo->serverState,0, DOT1X_SERVER_STATE_LEN);
      logicalPortInfo->serverStateLen = 0;
    }

    /* Per draft-congdon-radius-8021x-27.txt, if we got a Session-Timeout of zero and a
     * Termination-Action of RADIUS Request, we need to reauthenticate immediately.
     * If Session-Timeout is zero and Termination-Action is the default, then we
     * initialize immediately.
     */
    if (gotSessionTimeout == L7_TRUE &&
        logicalPortInfo->sessionTimeout == 0)
    {
      if (dot1xRadiusAccountingStop(lIntIfNum, RADIUS_ACCT_TERM_CAUSE_SESSION_TIMEOUT) != L7_SUCCESS)
      {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,physPort,"%s:%d:Failed sending Termination for %d \n",
                          __FUNCTION__,__LINE__,lIntIfNum);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
                "dot1xRadiusAcceptProcess: failed sending terminate cause");
      }
      if (logicalPortInfo->terminationAction == RADIUS_TERMINATION_ACTION_RADIUS)
      {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,physPort,
                          "%s:%d:Termination action for %d as sessionTimeout is zero \n",
                          __FUNCTION__,__LINE__,lIntIfNum);
        rc = dot1xLogicalPortReauthenticateSet(lIntIfNum, L7_TRUE);
      }
      else if (logicalPortInfo->terminationAction == RADIUS_TERMINATION_ACTION_DEFAULT)
      {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,physPort,
                          "%s:%d:Termination default for %d as sessionTimeout is zero \n",
                          __FUNCTION__,__LINE__,lIntIfNum);
        rc = dot1xLogicalPortInitializeSet(lIntIfNum, L7_TRUE);
      }

    }

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "Dot1x Authenticated Successfully");
    if(reasonCode == 0)
    {
      reasonCode = DOT1X_AUTH_REASON_RADIUS_SUCCESS;
    }
    /* Log the information */
    dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                            reasonCode,
                            L7_DOT1X_PORT_STATUS_AUTHORIZED,
                            logicalPortInfo);
    vlanAssigned = 0;  
    dot1qQportsCurrentVIDGet(logicalPortInfo->physPort, L7_DOT1Q_DEFAULT_VID_INDEX, &vlanAssigned);
  }

  return rc;
}

/**************************************************************************
* @purpose   Process RADIUS Challenge from server
*
* @param     lIntIfNum       @b{(input)} Logical  interface number of port being authenticated
* @param     *radiusPayload  @b{(input)} payload of RADIUS frame (attributes)
* @param     payloadLen      @b{(input)} length of RADIUS payload
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xRadiusChallengeProcess(L7_uint32 lIntIfNum, L7_uchar8 *radiusPayload, L7_uint32 payloadLen)
{
  L7_netBufHandle bufHandle;
  L7_uchar8 *data, *eapBuf,*challenge=L7_NULLPTR;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_uchar8 srcMac[L7_MAC_ADDR_LEN];
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_radiusAttr_t *radiusAttr,*eapTlv;
  L7_uint32 length, attrVal, ifIndex;
  L7_BOOL done = L7_FALSE, gotEap = L7_FALSE;
  L7_RC_t rc;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort;
  L7_uchar8 ch;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  physPort = logicalPortInfo->physPort;

  if (dot1xIntfIsConfigurable(physPort, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,
                    "%s:Recieved Radius Challenge message for logical - %d \n",
                    __FUNCTION__,lIntIfNum);

  /* Get base MAC address to use for the src MAC */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(srcMac);
  else
    simGetSystemIPLocalAdminMac(srcMac);

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
        "dot1xRadiusChallengeProcess: Out of System buffers."
        " 802.1X cannot process/transmit message due to lack of internal buffers");
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);
  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr, srcMac, L7_MAC_ADDR_LEN);

  /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;

  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapBuf = (L7_uchar8 *)eapPkt;

  radiusAttr = (L7_radiusAttr_t *)radiusPayload;

  while (done == L7_FALSE)
  {
    if (radiusAttr->type == RADIUS_ATTR_TYPE_NAS_PORT) /* NAS-Port */
    {
      /* Verify that the NAS-Port is equal to the port we are attempting to authenticate */
      /* if (nimGetIntfIfIndex(intIfNum, &ifIndex) == L7_SUCCESS)*/
      ifIndex = lIntIfNum;
      {
        memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
               radiusAttr->length - sizeof(L7_radiusAttr_t));
        if(attrVal != ifIndex)
        {
          SYSAPI_NET_MBUF_FREE(bufHandle);
           /* reset port in any dot1x assigned vlans*/
          dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

          /* If Monitor Mode is enabled, Authorize the port and Log an information */
          if((dot1xCfg->monitorMode == L7_ENABLE) &&
             (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
          {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                       "Failed to process due to invalid NAS Port. Authenticated through Monitor Mode");
            /* Log the information */
            dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                    DOT1X_AUTH_REASON_RADIUS_CHALLENGE_PROCESS_INVALID_NAS_PORT,
                                    L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                    logicalPortInfo);
            return L7_SUCCESS; 
          }
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                     "Failed to authenticate due to invalid NAS Port.");
          rc = dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
          /* Log the information */
          dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                  DOT1X_AUTH_REASON_RADIUS_CHALLENGE_PROCESS_INVALID_NAS_PORT,
                                  L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                                  logicalPortInfo);
          return rc; 
        }
      }
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_SESSION_TIMEOUT) /* Session-Timeout */
    {
      memcpy((L7_uchar8 *)&attrVal, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));
      /* Per draft-congdon-radius-8021x-27.txt, load the suppTimeout constant */
      rc = dot1xCtlLogicalPortSuppTimeoutSet(logicalPortInfo, attrVal);
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_EAP_MESSAGE) /* EAP message */
    {
      memcpy(eapBuf, (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));

      /* If this is the first EAP msg in the frame, save the ID and set flag. */
      if (gotEap == L7_FALSE)
      {
        logicalPortInfo->idFromServer = eapPkt->id;
        if (logicalPortInfo->isMABClient == L7_TRUE)
        {
          eapTlv = (L7_radiusAttr_t *)((L7_uchar8 *) eapBuf + sizeof(L7_eapPacket_t));
          ch = (L7_uchar8 )EAP_RRMD5;
          if (memcmp(&eapTlv->type,(L7_uchar8 *)&ch,sizeof(L7_uchar8))==0)
          {
            ch = (L7_uchar8 )DOT1X_CHALLENGE_LEN;
            if (memcmp(&eapTlv->length,(L7_uchar8 *)&ch,sizeof(L7_uchar8))<=0)
            {
               logicalPortInfo->dot1xChallengelen = (L7_uint32)eapTlv->length;
               challenge = (L7_uchar8 *) ((L7_uchar8 *) eapTlv + sizeof(L7_radiusAttr_t));
               DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,"\r\n%s: Challenge Len : %d",__FUNCTION__,logicalPortInfo->dot1xChallengelen);
               memset(logicalPortInfo->dot1xChallenge ,0,DOT1X_CHALLENGE_LEN);
               memcpy(logicalPortInfo->dot1xChallenge,challenge,logicalPortInfo->dot1xChallengelen);
               DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,"\r\n%s: Challenge :",__FUNCTION__);
               dot1xDebugDataDump(DOT1X_TRACE_MAB,logicalPortInfo->physPort,logicalPortInfo->dot1xChallenge,logicalPortInfo->dot1xChallengelen);
            }
            else
            {
              DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,"\r\n%s: Error Challenge length sent by server excceds capacity [%d]",
                                __FUNCTION__,DOT1X_CHALLENGE_LEN);
              /*will fail as serve challenge exceeds capacity . copy upto DOT1X_CHALLENGE_LEN bytes in challenge*/
              challenge = (L7_uchar8 *) ((L7_uchar8 *) eapTlv + sizeof(L7_radiusAttr_t));
              logicalPortInfo->dot1xChallengelen = DOT1X_CHALLENGE_LEN;
              memcpy(logicalPortInfo->dot1xChallenge,challenge,logicalPortInfo->dot1xChallengelen);
              DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,"\r\n%s: Challenge :",__FUNCTION__);
              dot1xDebugDataDump(DOT1X_TRACE_MAB,logicalPortInfo->physPort,logicalPortInfo->dot1xChallenge,logicalPortInfo->dot1xChallengelen);
            }
          }
          else
            DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,"\n %s: Not a MD5 Challenge packet.",__FUNCTION__);
        }
        gotEap = L7_TRUE;
      }
      eapBuf += radiusAttr->length - sizeof(L7_radiusAttr_t);
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_STATE) /* State */
    {
      memset(logicalPortInfo->serverState,0, DOT1X_SERVER_STATE_LEN);
      memcpy(logicalPortInfo->serverState,
             (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));
      logicalPortInfo->serverStateLen = radiusAttr->length - sizeof(L7_radiusAttr_t);
    }

    if (radiusAttr->type == RADIUS_ATTR_TYPE_CLASS) /* Class */
    {
      memset(logicalPortInfo->serverClass,0, DOT1X_SERVER_CLASS_LEN);
      memcpy(logicalPortInfo->serverClass,
             (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
             radiusAttr->length - sizeof(L7_radiusAttr_t));
      logicalPortInfo->serverClassLen = radiusAttr->length - sizeof(L7_radiusAttr_t);
    }

    payloadLen -= radiusAttr->length;
    if (payloadLen <= (L7_uint32)sizeof(L7_radiusAttr_t))
    {
      if (gotEap == L7_FALSE)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xRadiusChallengeProcess: EAP message not received from server."
            " RADIUS server did not send required EAP message");
        SYSAPI_NET_MBUF_FREE(bufHandle);
         /* reset port in any dot1x assigned vlans*/
        dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

        /* If Monitor Mode is enabled, Authorize the logical interface and Log an information */
        if((dot1xCfg->monitorMode == L7_ENABLE) &&
           (dot1xCtlPortMonitorModeOperational(lIntIfNum) == L7_SUCCESS))
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                  "Failed to process due to EAP message not received from server. Authenticated through Monitor Mode");
          
          /* Log the information */
          dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                  DOT1X_AUTH_REASON_RADIUS_CHALLENGE_PROCESS_INVALID_NAS_PORT,
                                  L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  logicalPortInfo);
          return L7_SUCCESS;
        }
        /* Log the information */
        dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                                  DOT1X_AUTH_REASON_RADIUS_CHALLENGE_PROCESS_INVALID_NAS_PORT,
                                  L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                                  logicalPortInfo);
        return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
      done = L7_TRUE;
    }
    else
    {
      radiusAttr = (L7_radiusAttr_t *)((L7_uchar8 *)radiusAttr + radiusAttr->length);
    }

    if (radiusAttr->length <= 0)
      done = L7_TRUE;
  }

  eapolPkt->packetBodyLength = eapPkt->length; /* already in network byte order */

  length = L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
           osapiNtohs(eapolPkt->packetBodyLength);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

  if (logicalPortInfo->isMABClient == L7_TRUE)
  {
    dot1xDebugPacketDump(DOT1X_TRACE_MAB,logicalPortInfo->physPort,logicalPortInfo->physPort,bufHandle);
  }

  rc = dot1xStateMachineClassifier(bamAReq, lIntIfNum, bufHandle, L7_NULLPTR);

  return rc;
}

/**************************************************************************
* @purpose   Build VP list and send Access Request to RADIUS client
*
* @param     lIntIfNum       @b{(input)} Logical interface number of port being authenticated
* @param     *suppEapData  @b{(input)} EAP info received from supplicant
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xRadiusAccessRequestSend(L7_uint32 lIntIfNum, L7_uchar8 *suppEapData)
{
  L7_uint32 ifIndex, framedMtu, nasPortType;
  L7_uint32 eapLen, eapFragLen;
  L7_BOOL   mode = L7_FALSE;
  L7_uint32 ipAddr = L7_NULL;
  radiusValuePair_t *vp;
  L7_uchar8 calledId[DOT1X_MAC_ADDR_STR_LEN+1],
            callingId[DOT1X_MAC_ADDR_STR_LEN+1],
            nasId[DOT1X_MAC_ADDR_STR_LEN+1];
  L7_uchar8 localMac[L7_MAC_ADDR_LEN], baseMac[L7_MAC_ADDR_LEN];
  L7_eapPacket_t *eapPkt = L7_NULLPTR;
  L7_RC_t rc;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort;

  if (suppEapData != L7_NULL)
     eapPkt = (L7_eapPacket_t *)suppEapData;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  physPort = logicalPortInfo->physPort;


  DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,
                    "%s:Recieved Radius send Access Request message for logical - %d \n",
                    __FUNCTION__,lIntIfNum);

  if (dot1xIntfIsConfigurable(physPort, &pCfg) != L7_TRUE)
    return L7_FAILURE;


  if (nimGetIntfIfIndex(physPort, &ifIndex) != L7_SUCCESS)
    return L7_FAILURE;

  radiusAttrValuePairListInit(&vp);

  /* User-Name */
  rc = radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_USER_NAME, 0, logicalPortInfo->dot1xUserName,
                        (L7_uint32)(strlen((L7_char8 *)logicalPortInfo->dot1xUserName)));


  /* Called-Station-Id */
  bzero(localMac, L7_MAC_ADDR_LEN);
  if (nimGetIntfAddress(physPort, L7_NULL, localMac) == L7_SUCCESS)
  {
    bzero(calledId, DOT1X_MAC_ADDR_STR_LEN+1);
    osapiSnprintf((L7_char8 *)calledId, sizeof(calledId), "%02x-%02x-%02x-%02x-%02x-%02x", localMac[0], localMac[1], localMac[2],
                                                                   localMac[3], localMac[4], localMac[5]);
  }
  else
    rc = L7_FAILURE;
  rc = rc != L7_SUCCESS ? rc :
    radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CALLED_STATION_ID, 0, calledId, DOT1X_MAC_ADDR_STR_LEN);

  /* Calling-Station-Id */
  bzero(callingId, DOT1X_MAC_ADDR_STR_LEN+1);
  osapiSnprintf((L7_char8 *)callingId, sizeof(callingId), "%02x:%02x:%02x:%02x:%02x:%02x",
          logicalPortInfo->suppMacAddr.addr[0],
          logicalPortInfo->suppMacAddr.addr[1],
          logicalPortInfo->suppMacAddr.addr[2],
          logicalPortInfo->suppMacAddr.addr[3],
          logicalPortInfo->suppMacAddr.addr[4],
          logicalPortInfo->suppMacAddr.addr[5]);
  rc = rc != L7_SUCCESS ? rc :
    radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CALLING_STATION_ID, 0, callingId, DOT1X_MAC_ADDR_STR_LEN);

  /* NAS-Identifier */
  bzero(baseMac, L7_MAC_ADDR_LEN);
  /* Retrieve the Base MAC Address */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);
  bzero((L7_char8 *)nasId, DOT1X_MAC_ADDR_STR_LEN+1);
  sprintf((L7_char8 *)nasId, "%02x-%02x-%02x-%02x-%02x-%02x", baseMac[0], baseMac[1], baseMac[2],
                                                  baseMac[3], baseMac[4], baseMac[5]);
  rc = rc != L7_SUCCESS ? rc :
    radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_NAS_IDENTIFIER, 0, nasId, DOT1X_MAC_ADDR_STR_LEN);

  /*NAS-IP Address */
  radiusAttribute4Get(&mode,&ipAddr);
  if(mode == L7_TRUE)
  {
    rc = rc != L7_SUCCESS ? rc :
       radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_NAS_IP_ADDRESS, 0, &ipAddr, (L7_uint32)sizeof(L7_uint32));
  }

  /* Vlan Id */
/*  if(dot1qQportsVIDGet(physPort, L7_DOT1Q_DEFAULT_VID_INDEX, &vlanId) == L7_SUCCESS)
  {
    rc = rc != L7_SUCCESS ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_VLAN_ID, 0, &vlanId, (L7_uint32)sizeof(L7_uint32));
  }*/

  /* NAS-Port */
  rc = rc != L7_SUCCESS ? rc :
    radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_NAS_PORT, 0, &ifIndex, (L7_uint32)sizeof(L7_uint32));

  /* Framed-MTU */
  framedMtu = L7_PORT_ENET_ENCAP_DEF_MTU;
  rc = rc != L7_SUCCESS ? rc :
    radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_FRAMED_MTU, 0, &framedMtu, (L7_uint32)sizeof(L7_uint32));

  /* NAS-Port-Type */
  nasPortType = RADIUS_NAS_PORT_TYPE_ETHERNET;
  rc = rc != L7_SUCCESS ? rc :
    radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_NAS_PORT_TYPE, 0, &nasPortType, (L7_uint32)sizeof(L7_uint32));

  /* State */
  if (logicalPortInfo->serverStateLen != 0)
  {
    rc = rc != L7_SUCCESS ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_STATE, 0, logicalPortInfo->serverState,
                       logicalPortInfo->serverStateLen);
  }

  /* Class */
  if (logicalPortInfo->serverClassLen != 0)
  {
    rc = rc != L7_SUCCESS ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CLASS, 0, logicalPortInfo->serverClass,
                       logicalPortInfo->serverClassLen);
  }

  /* EAP-Message */
  eapLen = eapPkt->length; /* Already endian-ized */
  do
  {
    if (eapLen > RADIUS_VALUE_LENGTH)
    {
      eapFragLen = RADIUS_VALUE_LENGTH;
      eapLen -= RADIUS_VALUE_LENGTH;
    }
    else
    {
      eapFragLen = eapLen;
      eapLen = 0;
    }

    rc = rc != L7_SUCCESS ? rc :
    radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_EAP_MESSAGE, 0, (L7_uchar8 *)eapPkt, eapFragLen);
    eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapPkt + eapFragLen);
   } while (eapLen > 0);



  /* If we haven't built the attribute list successfully, free the list and generate an auth failure */
  if(rc != L7_SUCCESS)
  {
    radiusAttrValuePairListFree(vp);
    DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,
                    "%s: Radius send Access Request message failed for logical - %d  as radius attribute list was not built successfully.\n",
                    __FUNCTION__,lIntIfNum);
    /* reset port in any dot1x assigned vlans*/
     dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
          "Failed to send Radius Access Request message.");
    /* Log the information */
    dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, 
                            DOT1X_AUTH_REASON_RADIUS_REQUEST_SEND_MSG_ERROR,
                            L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
                            logicalPortInfo);

    return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
  }

  rc = radiusAccessRequestSend(vp, lIntIfNum, L7_DOT1X_COMPONENT_ID);
  return rc;
}

/**************************************************************************
* @purpose   Build VP list and send Accounting Start to RADIUS client
*
* @param     lIntIfNum     @b{(input)} Logical internal interface number of port being authenticated
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xRadiusAccountingStart(L7_uint32 lIntIfNum)
{
  L7_uint32 ifIndex, nasPortType;
  radiusValuePair_t *vp;
  L7_char8 sessionId[DOT1X_SESSION_ID_LEN];
  L7_uchar8 calledId[DOT1X_MAC_ADDR_STR_LEN+1],
            callingId[DOT1X_MAC_ADDR_STR_LEN+1],
            localMac[L7_MAC_ADDR_LEN];
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 vlanId;
  L7_BOOL   mode = L7_FALSE;
  L7_uint32 ipAddr = L7_NULL;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  physPort = logicalPortInfo->physPort;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,"%s: called for port -%d\n",
                    __FUNCTION__,lIntIfNum);

  dot1xCtlResetPortSessionData(physPort);
  dot1xCtlResetLogicalPortSessionData(logicalPortInfo);

  /* If we're not doing RADIUS authentication, just return a success */
  if (logicalPortInfo->authMethod != L7_AUTH_METHOD_RADIUS)
  {
    return L7_SUCCESS;
  }

  if (nimGetIntfIfIndex(physPort, &ifIndex) == L7_SUCCESS)
  {
    radiusAttrValuePairListInit(&vp);

    /* User-Name */
    rc = radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_USER_NAME, 0, logicalPortInfo->dot1xUserName,
                          (L7_uint32)(strlen((L7_char8 *)logicalPortInfo->dot1xUserName)));

    /* Called-Station-Id */
    bzero(localMac, L7_MAC_ADDR_LEN);
    if (nimGetIntfAddress(physPort, L7_NULL, localMac) == L7_SUCCESS)
    {
      bzero((L7_char8 *)calledId, DOT1X_MAC_ADDR_STR_LEN+1);
      sprintf((L7_char8 *)calledId, "%02x-%02x-%02x-%02x-%02x-%02x", localMac[0], localMac[1], localMac[2],
                                                                 localMac[3], localMac[4], localMac[5]);
    }
    else
    {
      rc = L7_FAILURE;
    }

    /* Called-Station-Id */
    rc = (rc != L7_SUCCESS) ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CALLED_STATION_ID, 0, calledId, DOT1X_MAC_ADDR_STR_LEN);

    /* Calling-Station-Id */
    bzero((L7_char8 *)callingId, DOT1X_MAC_ADDR_STR_LEN+1);
    sprintf((L7_char8 *)callingId, "%02x:%02x:%02x:%02x:%02x:%02x",
            logicalPortInfo->suppMacAddr.addr[0],
            logicalPortInfo->suppMacAddr.addr[1],
            logicalPortInfo->suppMacAddr.addr[2],
            logicalPortInfo->suppMacAddr.addr[3],
            logicalPortInfo->suppMacAddr.addr[4],
            logicalPortInfo->suppMacAddr.addr[5]);

    rc = (rc != L7_SUCCESS) ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CALLING_STATION_ID, 0, callingId, DOT1X_MAC_ADDR_STR_LEN);

    /* NAS-Port-Type */
    nasPortType = RADIUS_NAS_PORT_TYPE_ETHERNET;
    rc = (rc != L7_SUCCESS) ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_NAS_PORT_TYPE, 0, &nasPortType, (L7_uint32)sizeof(L7_uint32));

    /* State */
    if (logicalPortInfo->serverStateLen != 0)
    {
      rc = (rc != L7_SUCCESS) ? rc :
        radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_STATE, 0, logicalPortInfo->serverState, logicalPortInfo->serverStateLen);
    }

    /* Class */
    if (logicalPortInfo->serverClassLen != 0)
    {
      rc = (rc != L7_SUCCESS) ? rc :
        radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CLASS, 0, logicalPortInfo->serverClass, logicalPortInfo->serverClassLen);
    }

    /* NAS-IP Address */
    radiusAttribute4Get(&mode,&ipAddr);
    if(mode == L7_TRUE)
    {
      rc = (rc != L7_SUCCESS) ? rc :
         radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_NAS_IP_ADDRESS, 0, &ipAddr, (L7_uint32)sizeof(L7_uint32));
    }

    /* Vlan Id */
    if(dot1qQportsVIDGet(physPort, L7_DOT1Q_DEFAULT_VID_INDEX, &vlanId) == L7_SUCCESS)
    {
      rc = (rc != L7_SUCCESS) ? rc :
        radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_TUNNEL_PRIVATE_GROUP_ID, 0, &vlanId, (L7_uint32)sizeof(L7_uint32));
    }

    /* Send accounting start request */
    if (rc == L7_SUCCESS)
    {
      bzero((L7_char8 *)sessionId, DOT1X_SESSION_ID_LEN);
      sprintf((L7_char8 *)sessionId, "%s:%08X", (L7_char8 *)logicalPortInfo->dot1xUserName, ifIndex);
      strcpy(dot1xPortSessionStats[physPort].sessionId, sessionId);
      strcpy(dot1xPortSessionStats[physPort].userName, logicalPortInfo->dot1xUserName);

      if (radiusAccountingStart(sessionId, ifIndex, vp) != L7_SUCCESS)
      {
        radiusAttrValuePairListFree(vp);

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
                "dot1xRadiusAccountingStart: error calling radiusAccountingStart, ifIndex=%u\n", ifIndex);
      }
    }
    else
    {
      if (vp != L7_NULL)
      {
        radiusAttrValuePairListFree(vp);
      }

      DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS, physPort,
                        "%s: Radius send Accounting Start message failed for logical - %d, attribute list was not built successfully.\n",
                        __FUNCTION__, lIntIfNum);
    }
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose   Build VP list and send Accounting Stop to RADIUS client
*
* @param     lIntIfNum     @b{(input)} Logical interface number of port being authenticated
* @param     termCause    @b{(input)} session termination cause
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xRadiusAccountingStop(L7_uint32 lIntIfNum, L7_uint32 termCause)
{
  L7_uint32 ifIndex, nasPortType;
  radiusValuePair_t *vp;
  L7_char8 sessionId[DOT1X_SESSION_ID_LEN];
  L7_uchar8 calledId[DOT1X_MAC_ADDR_STR_LEN+1],
            callingId[DOT1X_MAC_ADDR_STR_LEN+1],
            localMac[L7_MAC_ADDR_LEN];
  L7_RC_t rc;
  L7_uint32 sessionTime;
  L7_BOOL   mode = L7_FALSE;
  L7_uint32 ipAddr = L7_NULL;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  physPort = logicalPortInfo->physPort;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,"%s: called for port -%d\n",
                    __FUNCTION__,lIntIfNum);

  dot1xCtlStopPortSessionData(physPort);
  dot1xCtlStopLogicalPortSessionData(logicalPortInfo);

  /* If we're not doing RADIUS authentication, just return a success */
  if (logicalPortInfo->authMethod != L7_AUTH_METHOD_RADIUS)
    return L7_SUCCESS;

  if (nimGetIntfIfIndex(physPort, &ifIndex) == L7_SUCCESS)
  {
    radiusAttrValuePairListInit(&vp);

    /* User-Name */
    rc = radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_USER_NAME, 0, logicalPortInfo->dot1xUserName,
                          (L7_uint32)(strlen((L7_char8 *)logicalPortInfo->dot1xUserName)));

    /* Called-Station-Id */
    bzero(localMac, L7_MAC_ADDR_LEN);
    if (nimGetIntfAddress(physPort, L7_NULL, localMac) == L7_SUCCESS)
    {
      bzero(calledId, DOT1X_MAC_ADDR_STR_LEN+1);
      sprintf((L7_char8 *)calledId, "%02x-%02x-%02x-%02x-%02x-%02x", localMac[0], localMac[1], localMac[2],
                                                                 localMac[3], localMac[4], localMac[5]);
    }
    else
      rc = L7_FAILURE;
    rc = rc != L7_SUCCESS ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CALLED_STATION_ID, 0, calledId, DOT1X_MAC_ADDR_STR_LEN);

    /* Calling-Station-Id */
    bzero((L7_char8 *)callingId, DOT1X_MAC_ADDR_STR_LEN+1);
    sprintf((L7_char8 *)callingId, "%02x:%02x:%02x:%02x:%02x:%02x",
            logicalPortInfo->suppMacAddr.addr[0],
            logicalPortInfo->suppMacAddr.addr[1],
            logicalPortInfo->suppMacAddr.addr[2],
            logicalPortInfo->suppMacAddr.addr[3],
            logicalPortInfo->suppMacAddr.addr[4],
            logicalPortInfo->suppMacAddr.addr[5]);
    rc = rc != L7_SUCCESS ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CALLING_STATION_ID, 0, callingId, DOT1X_MAC_ADDR_STR_LEN);

    /* NAS-Port-Type */
    nasPortType = RADIUS_NAS_PORT_TYPE_ETHERNET;
    rc = rc != L7_SUCCESS ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_NAS_PORT_TYPE, 0, &nasPortType,
                             (L7_uint32)sizeof(L7_uint32));

    /* State */
    if (logicalPortInfo->serverStateLen != 0)
    {
      rc = rc != L7_SUCCESS ? rc :
        radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_STATE, 0, logicalPortInfo->serverState,
                         logicalPortInfo->serverStateLen);
    }

    rc = rc != L7_SUCCESS ? rc :
      radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_ACCT_TERMINATE_CAUSE, 0, &termCause,
                             (L7_uint32)sizeof(L7_uint32));

    /* NAS-IP Address */
    radiusAttribute4Get(&mode,&ipAddr);
    if(mode == L7_TRUE)
    {
      rc = rc != L7_SUCCESS ? rc :
         radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_NAS_IP_ADDRESS, 0, &ipAddr, (L7_uint32)sizeof(L7_uint32));
    }

    /* State */
    if (logicalPortInfo->serverStateLen != 0)
    {
      rc = rc != L7_SUCCESS ? rc :
        radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_STATE, 0, logicalPortInfo->serverState,
                         logicalPortInfo->serverStateLen);
    }

    /* Class */
    if (logicalPortInfo->serverClassLen != 0)
    {
      rc = rc != L7_SUCCESS ? rc :
        radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_CLASS, 0, logicalPortInfo->serverClass,
                         logicalPortInfo->serverClassLen);
    }

    /* Session Time */
    if(dot1xPortSessionTimeGet(lIntIfNum, &sessionTime ) == L7_SUCCESS)
    {
      rc = rc != L7_SUCCESS ? rc :
        radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_ACCT_SESSION_TIME, 0, &sessionTime, (L7_uint32)sizeof(L7_uint32));
    }
    /* Input Octets */
    (void)radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_ACCT_INPUT_OCTETS,
                   0, &dot1xPortSessionStats[physPort].sessionOctetsRx,
                  (L7_uint32)sizeof(L7_uint32));
    /* Output Octets */
    (void)radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_ACCT_OUTPUT_OCTETS,
      0, &dot1xPortSessionStats[physPort].sessionOctetsTx,
      (L7_uint32)sizeof(L7_uint32));
    /* input giga bytes*/
    (void)radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_ACCT_G_IBYTES,
      0, &dot1xPortSessionStats[physPort].sessionOctetsGbRx,
      (L7_uint32)sizeof(L7_uint32));
   /* output giga bytes */
    (void)radiusAttrValuePairAdd(&vp, RADIUS_ATTR_TYPE_ACCT_G_OBYTES,
      0, &dot1xPortSessionStats[physPort].sessionOctetsGbTx,
      (L7_uint32)sizeof(L7_uint32));


    /* Send accounting stop request */
    if (rc == L7_SUCCESS)
    {
      bzero(sessionId, DOT1X_SESSION_ID_LEN);
      sprintf((L7_char8 *)sessionId, "%s:%08X", (L7_char8 *)logicalPortInfo->dot1xUserName, ifIndex);
      if (radiusAccountingStop(sessionId, ifIndex, vp) != L7_SUCCESS)
      {
          radiusAttrValuePairListFree(vp);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
                  "dot1xRadiusAccountingStop: error calling radiusAccountingStop, ifIndex=%u\n", ifIndex);
      }

    }
    else
    {
      if (vp != L7_NULL)
      {
         radiusAttrValuePairListFree(vp);
      }
    }
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose   Process EAP Response and Response/Identity frames
*
* @param     lIntIfNum   @b{(input)} Logical internal interface number
* @param     bufHandle  @b{(input)} the handle to the dot1x PDU
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xRadiusSuppResponseProcess(L7_uint32 lIntIfNum, L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt = L7_NULLPTR;
  L7_BOOL portAllow;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort,monitorMode;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  physPort = logicalPortInfo->physPort;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_RADIUS,physPort,"%s: called for port -%d\n",
                    __FUNCTION__,lIntIfNum);

  if(bufHandle != L7_NULL)
  {
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
    enetHdr = (L7_enetHeader_t *)data;
    eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE);
    eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
    eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));
  }

  /* If this is a an EAP/Response-Identity, we need to see if the user has access to this port. */
  if ((eapRrPkt != L7_NULLPTR)&&(eapRrPkt->type == EAP_RRIDENTITY))
  {
    /* User name was stored in port info structure when EAP-Response/Identity frame was received.
     * See if this user name matches a locally configured user.  If the user is locally configured
     * but does not have access to this port, generate an error.
     */
    if (userMgrLoginIndexGet((L7_char8 *)logicalPortInfo->dot1xUserName,
                             &logicalPortInfo->dot1xUserIndex) == L7_SUCCESS)
    {
      /* Make sure user has access to this port */
      if ( (userMgrPortUserAccessGet(physPort, logicalPortInfo->dot1xUserName, &portAllow) == L7_SUCCESS) &&
            (portAllow == L7_FALSE) )
      {
           /* reset port in any dot1x assigned vlans*/
          dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

          /* If Monitor Mode is enabled, Authorize the port and Log an information */
          if(dot1xMonitorModeGet(&monitorMode) && (monitorMode == L7_ENABLE))
          {
            /* Log the information */
            /* Make Monitor Mode Operational */
            return dot1xCtlPortMonitorModeOperational(lIntIfNum);
          }

          return dot1xStateMachineClassifier(bamAFail, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
    }

  }

  /* Send the Supplicant response to the server */
  return dot1xSendRespToServer(lIntIfNum, bufHandle);
}
