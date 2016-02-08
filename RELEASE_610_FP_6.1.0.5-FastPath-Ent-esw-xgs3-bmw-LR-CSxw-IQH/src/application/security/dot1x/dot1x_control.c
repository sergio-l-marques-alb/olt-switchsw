/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_control.c
*
* @purpose   dot1x Control file
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
#define L7_MAC_EAPOL_PDU

#include "dot1x_include.h"
#include "nimapi.h"
#include "dot1q_api.h"

#include "vlan_mac_api.h"
#include "tlv_api.h"
#include "l7_diffserv_api.h"
#include "dot1ad_l2tp_api.h"
#include "usmdb_common.h"
#include "voice_vlan_api.h"

void *dot1xTaskSyncSema;
static L7_uint32 dot1xTimerHandle;
void *dot1xQueue;      /* reference to the dot1x message queue */
L7_uint32 dot1xTaskId;
L7_uint32 dot1xTimerTaskId;
extern dot1xCnfgrState_t dot1xCnfgrState;
extern L7_VLAN_MASK_t dot1xVlanMask;
extern L7_ushort16 *dot1xVlanIntfCount;
extern L7_VLAN_MASK_t dot1xGuestVlanMask;
extern L7_ushort16 *dot1xGuestVlanIntfCount;
extern L7_tlvHandle_t    dot1xTlvHandle;

L7_char8 *L7_eapolPacketTypeStr[5] = {"EAP","START","LOGOFF","KEY","ENCASFALERT"};

static
L7_RC_t dot1xMacBasedVlanParticipationSet(L7_uint32 lIntIfNum, L7_uint32 vlanId);
static
L7_RC_t dot1xMacBasedVlanParticipationReset(dot1xLogicalPortInfo_t *logicalPortInfo, L7_uint32 vlanId);
static
L7_RC_t dot1xVlanPortAddProcess(L7_uint32 intIfNum,L7_uint32 vlanId);
static
L7_RC_t dot1xVlanPortDeleteProcess(L7_uint32 intIfNum,L7_uint32 vlanId);




/*********************************************************************
* @purpose  Initialize dot1x tasks and data
*
* @param    parmType  @b{(input)} describes 2nd parm (filename or structure)
* @param    buffer    @b{(input)} pointer to structure of filename
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xStartTasks()
{
  /* semaphore creation for task protection over the common data */
  dot1xTaskSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot1xTaskSyncSema == L7_NULL)
  {
    LOG_MSG("Unable to create dot1x task semaphore\n");
    return L7_FAILURE;
  }

  /* create dotxTask - to service dot1x message queue */
  dot1xTaskId = (L7_uint32)osapiTaskCreate("dot1xTask", (void *)dot1xTask, 0, 0,
                                           dot1xSidDefaultStackSize(),
                                           dot1xSidDefaultTaskPriority(),
                                           dot1xSidDefaultTaskSlice());

  if (dot1xTaskId == L7_ERROR)
  {
    LOG_MSG("Failed to create dot1x task\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_DOT1X_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Unable to initialize dot1xTask()\n");
    return L7_FAILURE;
  }


  /* create dotxTimerTask - to service Timer queue */
  dot1xTimerTaskId = (L7_uint32)osapiTaskCreate("dot1xTimerTask", (void *)dot1xTimerTask, 0, 0,
                                                dot1xSidDefaultStackSize(),
                                                dot1xSidDefaultTaskPriority(),
                                                dot1xSidDefaultTaskSlice());

  if (dot1xTimerTaskId == L7_ERROR)
  {
    LOG_MSG("Failed to create dot1x timer task\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_DOT1X_TIMER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Unable to initialize dot1xTimerTask()\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  dot1x task which serves the request queue
*
* @param    none
*
* @returns  void
*
* @comments User-interface writes and PAE PDUs are serviced off
*           of the dot1xQueue
*
* @end
*********************************************************************/
void dot1xTask()
{
  dot1xMsg_t msg;

  (void)osapiTaskInitDone(L7_DOT1X_TASK_SYNC);

  for (;;)
  {
    if (osapiMessageReceive(dot1xQueue, (void*)&msg, (L7_uint32)sizeof(dot1xMsg_t),
                            L7_WAIT_FOREVER) == L7_SUCCESS)
    {
        if (osapiSemaTake(dot1xTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
        {
          (void)dot1xDispatchCmd(&msg);
          if (osapiSemaGive(dot1xTaskSyncSema) != L7_SUCCESS)
            DOT1X_ERROR_SEVERE("dot1xTask: osapiSemaGive failed\n");
        }
        else
          DOT1X_ERROR_SEVERE("dot1xTask: osapiSemaTake failed\n");

    }
    else
     DOT1X_ERROR_SEVERE("dot1xTask: Failed to receive message on dot1xQueue\n");

  }
}

/*********************************************************************
* @purpose  dot1x timer task which serves the expired timers
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1xTimerTask()
{
  (void)osapiTaskInitDone(L7_DOT1X_TIMER_TASK_SYNC);

  if (osapiPeriodicUserTimerRegister(DOT1X_TIMER_TICK, &dot1xTimerHandle) != L7_SUCCESS)
    DOT1X_ERROR_SEVERE("dot1xTimerTask: osapiPeriodicUserTimerRegister failed\n");

  for (;;)
  {
    osapiPeriodicUserTimerWait(dot1xTimerHandle);

    if (osapiSemaTake(dot1xTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      (void)dot1xTimerAction();
      if (osapiSemaGive(dot1xTaskSyncSema) != L7_SUCCESS)
        DOT1X_ERROR_SEVERE("dot1xTimerTask: osapiSemaGive failed\n");
    }
    else
      DOT1X_ERROR_SEVERE("dot1xTimerTask: osapiSemaTake failed\n");
  }
}

/*********************************************************************
* @purpose  Save the data in a message to a shared memory
*
* @param    event   @b{(input)} event type
* @param    *data   @b{(input)} pointer to data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Once the message is serviced, this variable size data will
*           be retrieved
*
* @end
*********************************************************************/
L7_RC_t dot1xFillMsg(void *data, dot1xMsg_t *msg)
{
  switch (msg->event)
  {
  /* events originating from UI */
  case dot1xMgmtAdminModeEnable:
  case dot1xMgmtAdminModeDisable:
  case dot1xMgmtPortInitializeSet:
  case dot1xMgmtLogicalPortInitializeSet:
  case dot1xMgmtPortReauthenticateSet:
  case dot1xMgmtLogicalPortReauthenticateSet:
  case dot1xMgmtPortAdminControlledDirectionsSet:
  case dot1xMgmtPortControlModeSet:
  case dot1xMgmtPortPaeCapabilitySet:
  case dot1xMgmtPortQuietPeriodSet:
  case dot1xMgmtPortTxPeriodSet:
  case dot1xMgmtPortSuppTimeoutSet:
  case dot1xMgmtPortServerTimeoutSet:
  case dot1xMgmtPortMaxReqSet:
  case dot1xMgmtPortReAuthPeriodSet:
  case dot1xMgmtPortReAuthEnabledSet:
  case dot1xMgmtPortKeyTransmissionEnabledSet:
  case dot1xMgmtPortGuestVlanModeEnable:
  case dot1xMgmtPortGuestVlanModeDisable:
  case dot1xMgmtPortGuestVlanPeriod:
  case dot1xMgmtPortMaxUsersSet:
  case dot1xMgmtPortUnauthenticatedVlanSet:
  case dot1xMgmtSupplicantPortControlModeSet:
  case dot1xMgmtPortMABEnabledSet:
  case dot1xMgmtPortMABDisabledSet:
    /* add to queue L7_uint32 size */
    memcpy(&msg->data.msgParm, data, sizeof(L7_uint32));
    break;

  case dot1xVlanDeleteEvent:
  case dot1xVlanAddEvent:
  case dot1xVlanAddPortEvent:
  case dot1xVlanDeletePortEvent:
  case dot1xVlanPvidChangeEvent:
    memcpy(&msg->data.vlanData, data, sizeof(dot1qNotifyData_t));
    break;

  case dot1xIntfChange:
    /* add to queue a NIM correlator */
    memcpy(&msg->data.dot1xIntfChangeParms, data, sizeof(dot1xIntfChangeParms_t));

  case dot1xPduReceived:
    /* add to queue a bufhandle */
    memcpy(&msg->data.bufHandle, data, sizeof(L7_netBufHandle));
    break;

  case dot1xAaaInfoReceived:
    /* add to queue a char pointer */
    memcpy(&msg->data.dot1xAaaMsg, data, sizeof(dot1xAaaMsg_t));
    break;

  case apmInitialize:                                   /*E1.apm*/
  case apmPortControlEqualAutoPortModeNotEqualPortCtrl: /*E2*/
  case apmNotPortEnabled:                               /*E3.apm*/
  case apmEapLogoff:                                    /*E4*/
  case apmReAuthCountGreaterThanReAuthMax:              /*E5*/
  case apmQuietWhileEqualZero:                          /*E6*/
  case apmTxWhenZeroReAuthCountLessThanEqualReAuthMax:  /*E7*/
  case apmEapStartReAuthCountLessThanEqualReAuthMax:    /*E8*/
  case apmReauthAndReAuthCountLessThanEqualReAuthMax:   /*E9*/
  case apmNotEapLogoffAndNotAuthAbort:                  /*E11*/
  case apmEapLogoffAndNotAuthAbort:                     /*E12*/
  case apmEapStart:                                     /*E13*/
  case apmReAuthenticate:                               /*E14*/
  case apmAuthSuccess:                                  /*E15*/
  case apmAuthFail:                                     /*E16*/
  case apmAuthTimeout:                                  /*E17*/
  case apmPortControlEqualForceAuthorized:              /*E18*/
  case apmPortControlEqualForceUnauthorized:            /*E19*/
  case ktxInitialize:                                   /*E1.ktx*/
  case ktxPortControlNotEqualAuto:                      /*E20.ktx*/
  case ktxKeyTxEnabledAndKeyAvailableAndPortAuthorized: /*E21*/
  case ktxKeyAvailable:                                 /*E22*/
  case ktxNotKeyTxEnabled:                              /*E23*/
  case ktxPortStatusEqualUnauthorized:                  /*E24.ktx*/
  case rtmInitialize:                                   /*E1.rtm*/
  case rtmPortControlNotEqualAuto:                      /*E20.rtm*/
  case rtmPortStatusEqualUnauthorized:                  /*E24.rtm*/
  case rtmNotReauthEnabled:                             /*E25*/
  case rtmReauthWhenEqualZero:                          /*E26*/
  case bamInitialize:                                   /*E1.bam*/
  case bamPortControlNotEqualAuto:                      /*E20.bam*/
  case bamAuthAbort:                                    /*E27*/
  case bamAWhileEqualZeroAndReqCountNotEqualMaxReq:     /*E28*/
  case bamAWhileZeroAndReqCountGreaterThanEqualMaxReq:  /*E30*/
  case bamASuccess:                                     /*E31*/
  case bamAFail:                                        /*E32*/
  case bamAWhileEqualZero:                              /*E33*/
  case krxInitialize:                                   /*E1.krx*/
  case krxNotPortEnabled:                               /*E3.krx*/
  case dot1xMgmtPortStatsClear:
  case dot1xMgmtApplyConfigData:
  case dot1xMgmtApplyPortConfigData:
  case dot1xMgmtPortGuestVlanTimerExpired:
  case dot1xMgmtPortMABTimerExpired:
  case spmStartWhenZeroStartCountLessThanMaxStart:
  case sbmAuthWhileZero:
  case spmHeldWhileEqualZero:
    break; /* NULL data, proceed */

  case apmRxRespIdAndReAuthCountLessThanEqualReAuthMax: /*E10*/
  case bamRxResp:                                       /*E29*/
  case bamAReq:                                         /*E34*/
  case bamAuthStart:                                    /*E35*/
  case krxRxKey:                                        /*E36*/
    /* PDU needs to be passed along */
    if (data != L7_NULLPTR)
      memcpy(&msg->data.bufHandle, data, sizeof(L7_netBufHandle));
    else
      msg->data.bufHandle = L7_NULL;
    break;

  case dot1xCnfgr:
    /* add to queue a configurator cmd structure */
    memcpy(&msg->data.CmdData, data, sizeof(L7_CNFGR_CMD_DATA_t));
    break;

  case dot1xUnauthAddrCallBackEvent:
    memcpy(&msg->data.unauthParms,data,sizeof(dot1xUnauthCallbackParms_t));
    break;

  case dot1xVoiceVlanModeEvent:
    memcpy(&msg->data.voiceVlanParms,data,sizeof(dot1xVoiceVlanModeParms_t));

  default:
    /* unmatched event */
    return L7_FAILURE;

  } /* switch */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send a command to dot1x queue
*
* @param    event      @b{(input)} event type
* @param    intIfNum   @b{(input)} internal interface number
* @param    *data      @b{(input)} pointer to data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Command is queued for service
*
* @end
*********************************************************************/
L7_RC_t dot1xIssueCmd(L7_uint32 event, L7_uint32 intIfNum, void *data)
{
  dot1xMsg_t msg;
  L7_RC_t rc;

  /* copy event, intIfNum and data ptr to msg struct */
  msg.event = event;
  msg.intf = intIfNum;

  if (data != L7_NULLPTR)
    (void)dot1xFillMsg(data, &msg);

  /* send message */
  rc = osapiMessageSend(dot1xQueue, &msg, (L7_uint32)sizeof(dot1xMsg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    DOT1X_ERROR_SEVERE("Failed to send to dot1xQueue! Event: %u, interface: %u\n", event, intIfNum);
  }

  return rc;
}

/*********************************************************************
* @purpose  Route the event to a handling function and grab the parms
*
* @param    msg   @b{(input)} message containing event and interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xDispatchCmd(dot1xMsg_t *msg)
{
  L7_RC_t rc;

  switch (msg->event)
  {
  case apmInitialize:                                   /*E1.apm*/
  case apmPortControlEqualAutoPortModeNotEqualPortCtrl: /*E2*/
  case apmNotPortEnabled:                               /*E3.apm*/
  case apmEapLogoff:                                    /*E4*/
  case apmReAuthCountGreaterThanReAuthMax:              /*E5*/
  case apmQuietWhileEqualZero:                          /*E6*/
  case apmTxWhenZeroReAuthCountLessThanEqualReAuthMax:  /*E7*/
  case apmEapStartReAuthCountLessThanEqualReAuthMax:    /*E8*/
  case apmReauthAndReAuthCountLessThanEqualReAuthMax:   /*E9*/
  case apmNotEapLogoffAndNotAuthAbort:                  /*E11*/
  case apmEapLogoffAndNotAuthAbort:                     /*E12*/
  case apmEapStart:                                     /*E13*/
  case apmReAuthenticate:                               /*E14*/
  case apmAuthSuccess:                                  /*E15*/
  case apmAuthFail:                                     /*E16*/
  case apmAuthTimeout:                                  /*E17*/
  case apmPortControlEqualForceAuthorized:              /*E18*/
  case apmPortControlEqualForceUnauthorized:            /*E19*/
  case ktxInitialize:                                   /*E1.ktx*/
  case ktxPortControlNotEqualAuto:                      /*E20.ktx*/
  case ktxKeyTxEnabledAndKeyAvailableAndPortAuthorized: /*E21*/
  case ktxKeyAvailable:                                 /*E22*/
  case ktxNotKeyTxEnabled:                              /*E23*/
  case ktxPortStatusEqualUnauthorized:                  /*E24.ktx*/
  case rtmInitialize:                                   /*E1.rtm*/
  case rtmPortControlNotEqualAuto:                      /*E20.rtm*/
  case rtmPortStatusEqualUnauthorized:                  /*E24.rtm*/
  case rtmNotReauthEnabled:                             /*E25*/
  case rtmReauthWhenEqualZero:                          /*E26*/
  case bamInitialize:                                   /*E1.bam*/
  case bamPortControlNotEqualAuto:                      /*E20.bam*/
  case bamAuthAbort:                                    /*E27*/
  case bamAWhileEqualZeroAndReqCountNotEqualMaxReq:     /*E28*/
  case bamAWhileZeroAndReqCountGreaterThanEqualMaxReq:  /*E30*/
  case bamASuccess:                                     /*E31*/
  case bamAFail:                                        /*E32*/
  case bamAWhileEqualZero:                              /*E33*/
  case krxInitialize:                                   /*E1.krx*/
  case krxNotPortEnabled:                               /*E3.krx*/
  case spmStartWhenZeroStartCountLessThanMaxStart:      /*E5.spm*/
  case sbmAuthWhileZero:                                /*E7.sbm*/
  case spmHeldWhileEqualZero:                                /*E7.sbm*/
    rc = dot1xStateMachineClassifier(msg->event, msg->intf, L7_NULL, L7_NULLPTR);
    break; /* NULL data, proceed */

  case apmRxRespIdAndReAuthCountLessThanEqualReAuthMax: /*E10*/
  case bamRxResp:                                       /*E29*/
  case bamAReq:                                         /*E34*/
  case bamAuthStart:                                    /*E35*/
  case krxRxKey:                                        /*E36*/
    /* PDU needs to be passed along */
    rc = dot1xStateMachineClassifier(msg->event, msg->intf, msg->data.bufHandle, L7_NULLPTR);
    break;

  case dot1xPduReceived:
    rc = dot1xProcessRxPdu(msg->intf, msg->data.bufHandle);
    break;

  case dot1xIntfChange:
    rc = dot1xIhProcessIntfChange(msg->intf,
                                  msg->data.dot1xIntfChangeParms.intfEvent,
                                  msg->data.dot1xIntfChangeParms.nimCorrelator);
    break;

  case dot1xMgmtAdminModeEnable:
    rc = dot1xCtlAdminModeEnable();
    break;

  case dot1xMgmtAdminModeDisable:
    rc = dot1xCtlAdminModeDisable();
    break;

  case dot1xMgmtPortInitializeSet:
    rc = dot1xCtlPortInitializeSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtLogicalPortInitializeSet:
    rc = dot1xCtlLogicalPortInitializeSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortReauthenticateSet:
    rc = dot1xCtlPortReauthenticateSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtLogicalPortReauthenticateSet:
    rc = dot1xCtlLogicalPortReauthenticateSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortAdminControlledDirectionsSet:
    rc = dot1xCtlPortAdminControlledDirectionsSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortControlModeSet:
    rc = dot1xCtlPortControlModeSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtSupplicantPortControlModeSet:
    rc = dot1xCtlSupplicantPortControlModeSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortPaeCapabilitySet:
    rc = dot1xCtlPortPaeCapabilitiesSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortQuietPeriodSet:
    rc = dot1xCtlPortQuietPeriodSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortTxPeriodSet:
    rc = dot1xCtlPortTxPeriodSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortGuestVlanModeEnable:
    rc = dot1xCtlPortGuestVlanMode(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortGuestVlanTimerExpired:
    rc = dot1xCtlPortGuestVlanOperational(msg->intf);
    break;

  case dot1xMgmtPortGuestVlanModeDisable:
    rc = dot1xCtlPortGuestVlanModeDisable(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortGuestVlanPeriod:
    rc = dot1xCtlPortGuestVlanPeriodSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortSuppTimeoutSet:
    rc = dot1xCtlPortSuppTimeoutSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortServerTimeoutSet:
    rc = dot1xCtlPortServerTimeoutSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortMaxReqSet:
    rc = dot1xCtlPortMaxReqSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortReAuthPeriodSet:
    rc = dot1xCtlPortReAuthPeriodSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortReAuthEnabledSet:
    rc = dot1xCtlPortReAuthEnabledSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortKeyTransmissionEnabledSet:
    rc = dot1xCtlPortKeyTransmissionEnabledSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortStatsClear:
    rc =  dot1xCtlPortStatsClear(msg->intf);
    break;

  case dot1xAaaInfoReceived:
    rc = dot1xRadiusResponseProcess(msg->intf,
                                    msg->data.dot1xAaaMsg.status,
                                    msg->data.dot1xAaaMsg.pResponse,
                                    msg->data.dot1xAaaMsg.respLen);
    break;

  case dot1xMgmtApplyConfigData:
    rc = dot1xCtlApplyConfigData();
    break;

  case dot1xMgmtApplyPortConfigData:
    rc = dot1xCtlApplyPortConfigData(msg->intf);
    break;

  case dot1xCnfgr:
    dot1xCnfgrParse(&(msg->data.CmdData));
    rc = L7_SUCCESS;
    break;

  case dot1xVlanDeleteEvent:
  case dot1xVlanAddEvent:
  case dot1xVlanAddPortEvent:
  case dot1xVlanDeletePortEvent:
  case dot1xVlanPvidChangeEvent:
    dot1xVlanChangeProcess(msg->event,msg->intf,&(msg->data.vlanData));
    rc = L7_SUCCESS;
    break;

  case dot1xMgmtPortMaxUsersSet:
    rc = dot1xCtlPortMaxUsersSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xMgmtPortUnauthenticatedVlanSet:
    rc = dot1xCtlPortUnauthenticatedVlanSet(msg->intf, msg->data.msgParm);
    break;

  case dot1xUnauthAddrCallBackEvent:
    rc = dot1xCtlPortUnauthAddrCallbackProcess(msg->intf,msg->data.unauthParms.macAddr,msg->data.unauthParms.vlanId);
    break;

  case dot1xVoiceVlanModeEvent:
    rc = dot1xCtlPortVoiceVlanOperational(msg->intf,
                                          msg->data.voiceVlanParms.macAddr,
                                          msg->data.voiceVlanParms.vlanId,
                                          msg->data.voiceVlanParms.flag);
    break;

  case dot1xMgmtPortMABEnabledSet:
    rc = dot1xCtlPortMABEnableSet(msg->intf);
    break;

  case dot1xMgmtPortMABDisabledSet:
    rc = dot1xCtlPortMABDisableSet(msg->intf);
    break;

  case dot1xMgmtPortMABTimerExpired:
    rc = dot1xCtlLogicalPortMABOperational(msg->intf);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Receive dot1x PDUs from Supplicant and process them
*
* @param    lIntIfNum   @b{(input)} logical interface number that this PDU was received on
* @param    bufHandle  @b{(input)} buffer handle to the PDU
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Frame has to be adjusted for VLAN header and endianness
*
* @end
*********************************************************************/
L7_RC_t dot1xProcessRxPdu(L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{

  L7_uchar8 *data;
  L7_uint32 dataLength,maxFrameSize, lIntIfNum;
  L7_ushort16 protocolType, temp16;
  L7_uint32 vlanId ,vlanTag;
  L7_8021QTag_t       *vlanTagPtr;
  L7_RC_t              rc=L7_FAILURE;
  L7_enetHeader_t      *enetHdr;
  L7_uchar8             capabilities=0;




  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Determine the protocol type */
  memcpy ((L7_uchar8 *)&protocolType, data+L7_ENET_HDR_SIZE, sizeof(L7_ushort16));
  temp16 = osapiNtohs(protocolType);
  protocolType = temp16;

  if (protocolType == L7_ETYPE_8021Q)
  {
    vlanTagPtr = (L7_8021QTag_t*)(data+L7_ENET_HDR_SIZE);
    vlanTag = vlanTagPtr->tci & L7_VLAN_TAG_VLAN_ID_MASK;
    memcpy ((L7_uchar8 *)&protocolType, data+L7_ENET_HDR_SIZE+L7_8021Q_ENCAPS_HDR_SIZE, sizeof(L7_ushort16));
    temp16 = osapiNtohs(protocolType);
    protocolType = temp16;

  }
  else
  {
    (void) dot1qQportsVIDGet(intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &vlanTag);


  }


 (void) dot1qQportsVIDGet(intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &vlanId);



  if (nimGetIntfMaxFrameSize(intIfNum, &maxFrameSize) != L7_SUCCESS)
      return L7_FAILURE;

  enetHdr = (L7_enetHeader_t *)data;


  /*DOT1X_EVENT_TRACE(DOT1X_TRACE_PDU,intIfNum ,"\n%s:%d: EAPOL packet arrived from %02x:%02x:%02x:%02x:%02x:%02x with vlan tag : %d\n",
                      __FUNCTION__,__LINE__,
                      enetHdr->src.addr[0],
                      enetHdr->src.addr[1],
                      enetHdr->src.addr[2],
                      enetHdr->src.addr[3],
                      enetHdr->src.addr[4],
                      enetHdr->src.addr[5],
                      vlanTag);*/
    /* Remove any header tagging */

     /*dot1xDebugPacketDump(intIfNum,bufHandle);*/
    dot1xTxRxHeaderTagRemove(bufHandle);
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
    SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);


    if ( dot1xPortPaeCapabilitiesGet(intIfNum,&capabilities) == L7_FAILURE)
    {
        SYSAPI_NET_MBUF_FREE(bufHandle);
        rc = L7_FAILURE;
        return rc;
    }

    /*dot1xDebugPacketDump(intIfNum,bufHandle);*/
    /* Accomodate for Endianess */
    if (dot1xTxRxHostConvert(bufHandle) == L7_SUCCESS)
    {
      L7_eapolPacket_t *eapolPkt;

      /* Ensure packet body length is less than MTU minus header lengths */
      eapolPkt = (L7_eapolPacket_t *)(data + L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE);

      DOT1X_EVENT_TRACE(DOT1X_TRACE_PDU,intIfNum ,"\n%s:%d: EAPOL %s packet arrived from %02x:%02x:%02x:%02x:%02x:%02x with vlan tag : %d\n",
                      __FUNCTION__,__LINE__,
                      (eapolPkt->packetType<=EAPOL_ENCASFALERT)?L7_eapolPacketTypeStr[eapolPkt->packetType]:"(type unknown)",
                      enetHdr->src.addr[0],
                      enetHdr->src.addr[1],
                      enetHdr->src.addr[2],
                      enetHdr->src.addr[3],
                      enetHdr->src.addr[4],
                      enetHdr->src.addr[5],
                      vlanTag);

      if ( eapolPkt->packetBodyLength >
           (maxFrameSize - L7_ENET_HDR_SIZE - L7_ENET_ENCAPS_HDR_SIZE - sizeof(L7_eapolPacket_t)) )
      {
        if (capabilities == L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
        {
          dot1xPortStats[intIfNum].authEapLengthErrorFramesRx++;
        }
        else if (capabilities == L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
        {
          dot1xPortStats[intIfNum].suppEapLengthErrorFramesRx++;
        }
        SYSAPI_NET_MBUF_FREE(bufHandle);
        rc = L7_SUCCESS;
      }
      else
      {

        if (capabilities == L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
        {
           L7_BOOL exists=L7_FALSE ;

          /*Check if pdu is to be processed considering logical port use and availability*/
          if (dot1xCheckMapPdu(intIfNum, enetHdr->src.addr, &lIntIfNum,&exists) == L7_SUCCESS)
          {
            dot1xLogicalPortInfo_t *logicalPortInfo;
            logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);

            /* Coverity Fix to handle the possible NULLL ptr from the dot1xLogicalPortInfoGet */
            if (logicalPortInfo == L7_NULLPTR)
            {
               /* Coverity defect fix. should never come here as dot1xCheckMapPdu would have assigned a new node if a new client is detected
                 or returned existing logical interface number*/
               SYSAPI_NET_MBUF_FREE(bufHandle);
               return L7_FAILURE;
            }
            logicalPortInfo->vlanId = vlanId;

            /*Code to check if port is already authenticated and does not need*/
            /*to negotiate further*/
            if ((dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)&&
              (logicalPortInfo->apmState == APM_AUTHENTICATED &&
              ((eapolPkt->packetType != EAPOL_LOGOFF) &&
               (exists!=L7_TRUE ||(exists==L7_TRUE && eapolPkt->packetType != EAPOL_START)))))
            {
              dot1xTxCannedSuccess(lIntIfNum, DOT1X_LOGICAL_PORT);
              /* free the buffer */
              SYSAPI_NET_MBUF_FREE(bufHandle);
              rc = L7_SUCCESS;
            }
            else if ((dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO)&&
                   (logicalPortInfo->apmState == APM_AUTHENTICATED &&
                    (exists!=L7_TRUE && eapolPkt->packetType == EAPOL_START)))
            {
               dot1xTxCannedSuccess(lIntIfNum, DOT1X_LOGICAL_PORT);
              /* free the buffer */
               SYSAPI_NET_MBUF_FREE(bufHandle);
              rc = L7_SUCCESS;
            }
            else if(dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_FORCE_UNAUTHORIZED)
            {
              dot1xTxCannedFail(lIntIfNum, DOT1X_LOGICAL_PORT);
              /* free the buffer */
              SYSAPI_NET_MBUF_FREE(bufHandle);
              rc = L7_SUCCESS;
            }
            else if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_FORCE_AUTHORIZED)
            {
               if (EAPOL_START == eapolPkt->packetType)
               {
                 /* Only send "success" when a start is encountered */
                 dot1xTxCannedSuccess(lIntIfNum,DOT1X_LOGICAL_PORT);
               }
               /* free the buffer */
               SYSAPI_NET_MBUF_FREE(bufHandle);
              rc = L7_SUCCESS;
            }
            /* Process the frame */
            else
            {
               rc = dot1xStateMachineRxPdu(lIntIfNum,bufHandle);
            }

          }
          else
          {
            /* free the buffer */
            SYSAPI_NET_MBUF_FREE(bufHandle);
            rc = L7_SUCCESS;
          }
        }
        else if (capabilities == L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
        {
          if(dot1xSupplicantPortInfo[intIfNum].sPortMode == L7_DOT1X_PORT_AUTO)
          {
            DOT1X_EVENT_TRACE(DOT1X_TRACE_PDU,intIfNum ,"\n%s:%d:EAPOL packet arrived on Supplicant port with port mode %d\n", __FUNCTION__,__LINE__,dot1xSupplicantPortInfo[intIfNum].sPortMode);

            rc = dot1xSupplicantStateMachineRxPdu(intIfNum,bufHandle);
          }
          else if ( (dot1xSupplicantPortInfo[intIfNum].sPortMode == L7_DOT1X_PORT_FORCE_AUTHORIZED) ||
                    (dot1xSupplicantPortInfo[intIfNum].sPortMode == L7_DOT1X_PORT_FORCE_UNAUTHORIZED)
                  )
          {
            /* free the buffer */
            SYSAPI_NET_MBUF_FREE(bufHandle);
            rc = L7_SUCCESS;
          }
        }
      }
    }
    else
    {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_PDU,intIfNum ,"\n%s:%d: EAPOL packet arrived from %02x:%02x:%02x:%02x:%02x:%02x with vlan tag : %d\n",
                      __FUNCTION__,__LINE__,
                      enetHdr->src.addr[0],
                      enetHdr->src.addr[1],
                      enetHdr->src.addr[2],
                      enetHdr->src.addr[3],
                      enetHdr->src.addr[4],
                      enetHdr->src.addr[5],
                      vlanTag);

      /* increment invalid receive counter */
      dot1xPortStats[intIfNum].authInvalidEapolFramesRx++;
      /* free the buffer */
      SYSAPI_NET_MBUF_FREE(bufHandle);
      rc = L7_SUCCESS;
    }

   return rc;
}
/*********************************************************************
* @purpose  Receive dot1x PDUs from Supplicant and pass them to the classifier
*
* @param    lIntIfNum   @b{(input)} logical interface number that this PDU was received on
* @param    bufHandle  @b{(input)} buffer handle to the PDU
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Frame has already been adjusted for VLAN header and endianness
*
* @end
*********************************************************************/
L7_RC_t dot1xStateMachineRxPdu(L7_uint32 lIntIfNum, L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_eapolPacket_t *eapolPkt;
  L7_uint32 pduDataOffset;
  L7_uchar8 tmpMac[L7_ENET_MAC_ADDR_LEN];
  L7_BOOL freeBuffer = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physIntf;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }
  /* get the physPort info */
  physIntf = logicalPortInfo->physPort;

  /* Make sure port is up and enabled */
  if (dot1xPortInfo[physIntf].portEnabled != L7_TRUE)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  dot1xPortStats[physIntf].authEapolFramesRx++;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  pduDataOffset = sysNetDataOffsetGet(data);
  eapolPkt = (L7_eapolPacket_t *)(data + pduDataOffset);

  dot1xPortStats[physIntf].authLastEapolFrameVersion = (L7_uint32)eapolPkt->protocolVersion;
  enetHdr = (L7_enetHeader_t *)(data);
  memcpy(dot1xPortStats[physIntf].authLastEapolFrameSource.addr, enetHdr->src.addr, L7_ENET_MAC_ADDR_LEN);

  /* Add client Mac address to the MAC DB*/
  if ((dot1xPortInfo[physIntf].portControlMode == L7_DOT1X_PORT_AUTO)&&
      (dot1xPortInfo[physIntf].portMacAddrAdded == L7_FALSE))
  {
      if (dot1xMacAddrInfoAdd(&(logicalPortInfo->suppMacAddr),logicalPortInfo->logicalPortNumber)== L7_SUCCESS)
      {
          dot1xPortInfo[physIntf].portMacAddrAdded = L7_TRUE;
      }

  }
  dot1xDebugPacketRxTrace(physIntf,bufHandle);

  switch (eapolPkt->packetType)
  {
  case EAPOL_EAPPKT:
    /* Ignore guest vlan for this user since authentication has begun. */
    if (logicalPortInfo->guestVlanTimer != 0)
    {
      logicalPortInfo->guestVlanTimer = 0;
       DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,physIntf,
                        "\n%s:Reset guest vlan timer for linterface = %d as EAPOL_EAPPKT was recieved. \n",
                        __FUNCTION__,lIntIfNum);
    }
    rc = dot1xEapPktProcess(logicalPortInfo, bufHandle);
    break;
  case EAPOL_START:
      /*
       * If the client sends a Start frame but is on the guest VLAN, then ignore
       * the start frame.  There are some clients which send Start frames even
       * though authentication is disabled.
       */
      if ((APM_AUTHENTICATED != logicalPortInfo->apmState) ||
          (0 == logicalPortInfo->guestVlanId) || (logicalPortInfo->isMABClient == L7_TRUE) )
      {
        /* Save the source MAC so that we can send to the unicast address of the Supplicant */
        memcpy(logicalPortInfo->suppMacAddr.addr, enetHdr->src.addr, L7_ENET_MAC_ADDR_LEN);

        logicalPortInfo->eapStart = L7_TRUE;

        if (logicalPortInfo->reAuthCount <= dot1xPortInfo[physIntf].reAuthMax)
        {
          rc = dot1xStateMachineClassifier(apmEapStartReAuthCountLessThanEqualReAuthMax,
                                           lIntIfNum, L7_NULL, L7_NULLPTR);
        }
        rc = dot1xStateMachineClassifier(apmEapStart, lIntIfNum, L7_NULL, L7_NULLPTR);
        DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,physIntf,
                        "\n%s:EAPOL_START received on linterface = %d . \n",
                        __FUNCTION__,lIntIfNum);
      }
      dot1xPortStats[physIntf].authEapolStartFramesRx++;
      freeBuffer = L7_TRUE;
      break;
  case EAPOL_LOGOFF:
    /* If we've acquired a Supplicant, validate the source MAC before processing the Logoff */
    memset(tmpMac, 0, L7_ENET_MAC_ADDR_LEN);
    if ( (memcmp(tmpMac, enetHdr->src.addr, 0) == 0) ||
         (memcmp(logicalPortInfo->suppMacAddr.addr, enetHdr->src.addr, L7_ENET_MAC_ADDR_LEN) == 0) )
    {
      logicalPortInfo->eapLogoff = L7_TRUE;

      /* reset port in any dot1x assigned vlans*/
      dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

      if (logicalPortInfo->authAbort == L7_FALSE)
        rc = dot1xStateMachineClassifier(apmNotEapLogoffAndNotAuthAbort, lIntIfNum, L7_NULL, L7_NULLPTR);
      rc = dot1xStateMachineClassifier(apmEapLogoff, lIntIfNum, L7_NULL, L7_NULLPTR);
    }
    dot1xPortStats[physIntf].authEapolLogoffFramesRx++;
    freeBuffer = L7_TRUE;
    break;
  case EAPOL_KEY:
    rc = dot1xStateMachineClassifier(krxRxKey, lIntIfNum, bufHandle, L7_NULLPTR);
    break;
  case EAPOL_ENCASFALERT:
    rc = dot1xEapolAlertPktProcess(lIntIfNum, bufHandle);
    break;
  default:
    dot1xPortStats[physIntf].authInvalidEapolFramesRx++;
    /* TBD: check this line out dot1xLogicalPortInfoInitialize(lIntIfNum); */
    freeBuffer = L7_TRUE;
    rc = L7_FAILURE;
    break;
  }

  if (freeBuffer == L7_TRUE)
    SYSAPI_NET_MBUF_FREE(bufHandle);

  return rc;
}

/*********************************************************************
* @purpose  Process EAP Packets from Supplicant and pass them to the classifier
*
* @param    logicalPortInfo  @b{(input))  Logical Port Info node that this PDU was received on
* @param    bufHandle  @b{(input)} buffer handle to the PDU
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Frame has already been adjusted for VLAN header and endianness.
*           Interface has already beed verified
*
* @end
*********************************************************************/
L7_RC_t dot1xEapPktProcess(dot1xLogicalPortInfo_t *logicalPortInfo, L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_uint32 pduDataOffset;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 physIntf;

  /* get the physPort info */
  physIntf = logicalPortInfo->physPort;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  enetHdr = (L7_enetHeader_t *)(data);
  pduDataOffset = sysNetDataOffsetGet(data);
  eapolPkt = (L7_eapolPacket_t *)(data + pduDataOffset);
  eapPkt = (L7_eapPacket_t *)(data + pduDataOffset + sizeof(L7_eapolPacket_t));

  /* Make sure this is an EAP response and that the IDs match */
  if (eapPkt->code != EAP_RESPONSE ||
      eapPkt->id != logicalPortInfo->currentIdL /*&& eapPkt->id != dot1xPortInfo[physIntf].currentId */)
  {
     DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,0,
                      "%s incorrect code or current id %d instead of %d port-%d  and phyid- %d\n",
                      __FUNCTION__,eapPkt->id,logicalPortInfo->currentIdL,physIntf,dot1xPortInfo[physIntf].currentId);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }
  logicalPortInfo->currentIdL = eapPkt->id;

  eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));
  if (eapRrPkt->type == EAP_RRIDENTITY)
  {
    /* Get user name and user name length... store user name in port info structure. */
    L7_uchar8 *userName = ((L7_uchar8 *)eapRrPkt) + sizeof(L7_eapRrPacket_t);
    L7_uint32 userNameLen = eapPkt->length - sizeof(L7_eapPacket_t) - sizeof(L7_eapRrPacket_t);

    /* Save the source MAC so that we can send to the unicast address of the Supplicant */
    memcpy(logicalPortInfo->suppMacAddr.addr, enetHdr->src.addr, L7_ENET_MAC_ADDR_LEN);


    if (userNameLen > DOT1X_USER_NAME_LEN-1)
      userNameLen = DOT1X_USER_NAME_LEN-1;
    memset(logicalPortInfo->dot1xUserName,0,DOT1X_USER_NAME_LEN);
    memcpy(logicalPortInfo->dot1xUserName, userName, userNameLen);

    logicalPortInfo->dot1xUserNameLength = userNameLen;
    logicalPortInfo->rxRespId = L7_TRUE;
    dot1xPortStats[physIntf].authEapRespIdFramesRx++;
    if (logicalPortInfo->reAuthCount <= dot1xPortInfo[physIntf].reAuthMax)
      rc = dot1xStateMachineClassifier(apmRxRespIdAndReAuthCountLessThanEqualReAuthMax,
                                       logicalPortInfo->logicalPortNumber, bufHandle, L7_NULLPTR);
  }
  else
  {
    logicalPortInfo->rxResp = L7_TRUE;
    dot1xPortStats[physIntf].authEapResponseFramesRx++;
    if (eapRrPkt->type != EAP_RRNAK)
      dot1xPortStats[physIntf].authBackendNonNakResponsesFromSupplicant++;
    /*dot1xDebugPacketDump(logicalPortInfo->physPort,bufHandle);*/
    rc = dot1xStateMachineClassifier(bamRxResp, logicalPortInfo->logicalPortNumber, bufHandle, L7_NULLPTR);
  }

  return rc;
}

/*********************************************************************
* @purpose  Receives EAPOL ASF Alert Packets and passes them to the classifier
*
* @param    intIfNum   @b{(input)} interal interface number that this PDU was received on
* @param    bufHandle  @b{(input)} buffer handle to the PDU
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Frame has already been adjusted for VLAN header and endianness.
*           Interface has already beed verified.
*
* @end
*********************************************************************/
L7_RC_t dot1xEapolAlertPktProcess(L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine decrements all the timer counters for all ports
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments The events generated are directly sent to state machine classifier.
*
* @end
*********************************************************************/
L7_RC_t dot1xTimerAction()
{
 L7_uint32 phyIntf=0;
 L7_RC_t rc = L7_SUCCESS, nimRc;
 L7_uint32 lIntIfNum;
 dot1xLogicalPortInfo_t *logicalPortInfo;
 dot1xPortCfg_t         *pCfg;

  if (!DOT1X_IS_READY)
      return L7_SUCCESS;


  if (dot1xCfg->dot1xAdminMode != L7_ENABLE)
        return L7_SUCCESS;

  nimRc = dot1xFirstValidIntfNumber(&phyIntf);
  while (nimRc == L7_SUCCESS)
  {
    if (dot1xPortInfo[phyIntf].paeCapabilities ==
                               L7_DOT1X_PAE_PORT_AUTH_CAPABLE) /* Authenticator timer actions */
    {
      /* iterate through all the logical interfaces of the physical interface */
      lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
      while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(phyIntf,&lIntIfNum))!= L7_NULLPTR)
      {
        if (logicalPortInfo->inUse == L7_TRUE)
        {
          /* It is in use so operate on this entry */
          if (dot1xPortInfo[phyIntf].portEnabled == L7_TRUE &&
            (dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO ||
             dot1xPortInfo[phyIntf].portControlMode  == L7_DOT1X_PORT_AUTO_MAC_BASED))
          {

            /* Guest Vlan Timer*/
            if (logicalPortInfo->guestVlanTimer!= 0)
            {
              logicalPortInfo->guestVlanTimer--;

              if (logicalPortInfo->guestVlanTimer == 0)
              {
                if ((dot1xIntfIsConfigurable(phyIntf, &pCfg) == L7_TRUE)&&
                    (pCfg->mabEnabled == L7_ENABLE)&&
                    (dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED))
                {
                  DOT1X_EVENT_TRACE(DOT1X_TRACE_TIMER,phyIntf,"\n%s:%d MAB Timer timed out for lport %d\n", __FUNCTION__, __LINE__,
                                                   lIntIfNum);
                  rc = dot1xIssueCmd(dot1xMgmtPortMABTimerExpired, lIntIfNum,L7_NULLPTR);
                }
                else
              {
                DOT1X_EVENT_TRACE(DOT1X_TRACE_TIMER,phyIntf,"\n%s:%d Guest Vlan Timer timed out for lport %d\n", __FUNCTION__, __LINE__,
                                                 lIntIfNum);
                rc = dot1xIssueCmd(dot1xMgmtPortGuestVlanTimerExpired, lIntIfNum,L7_NULLPTR);
                /*
                 * Client is being placed in the guest VLAN so ensure that
                 * this timer does not expire below.
                 */
                logicalPortInfo->txWhenLP = 0;
                }

              }

           }

           /* Process Session-Timeout value sent by server */
           if (logicalPortInfo->sessionTimeout != 0)
           {
             logicalPortInfo->sessionTimeout--;
             if (logicalPortInfo->sessionTimeout == 0)
             {
                L7_BOOL trueVal = L7_TRUE;

                DOT1X_EVENT_TRACE(DOT1X_TRACE_TIMER,phyIntf,"\n%s:%d Session timeout for port %d\n", __FUNCTION__, __LINE__,
                                                 logicalPortInfo->logicalPortNumber);

                if (dot1xRadiusAccountingStop(lIntIfNum, RADIUS_ACCT_TERM_CAUSE_SESSION_TIMEOUT) != L7_SUCCESS)
                {
                  LOG_MSG("dot1xRadiusAcceptProcess: failed sending terminate cause");
                }

                if (logicalPortInfo->terminationAction == RADIUS_TERMINATION_ACTION_DEFAULT)
                {
                  rc = dot1xIssueCmd(dot1xMgmtLogicalPortInitializeSet, lIntIfNum, &trueVal);
                }
                else if (logicalPortInfo->terminationAction == RADIUS_TERMINATION_ACTION_RADIUS)
                 rc = dot1xIssueCmd(dot1xMgmtLogicalPortReauthenticateSet, lIntIfNum, &trueVal);
                else
                 DOT1X_ERROR_SEVERE("Unknown termination action value %u for logical interface %u\n",
                                   logicalPortInfo->terminationAction, lIntIfNum);
             }
           }

          /* Supplicant or Authentication Server timeout */
          if (logicalPortInfo->aWhile != 0)
          {
            logicalPortInfo->aWhile--;
            if (logicalPortInfo->aWhile == 0)
            {
              if (logicalPortInfo->reqCount >= dot1xPortInfo[phyIntf].maxReq)
                (void)dot1xIssueCmd(bamAWhileZeroAndReqCountGreaterThanEqualMaxReq, lIntIfNum, L7_NULLPTR);
              else
                (void)dot1xIssueCmd(bamAWhileEqualZeroAndReqCountNotEqualMaxReq, lIntIfNum, L7_NULLPTR);

              rc = dot1xIssueCmd(bamAWhileEqualZero, lIntIfNum, L7_NULLPTR);
            }
          }

          /* Time period in which Authenticator will not attempt to acquire a Supplicant */
          if (logicalPortInfo->quietWhile != 0)
          {
            logicalPortInfo->quietWhile--;
            if (logicalPortInfo->quietWhile == 0)
              rc = dot1xIssueCmd(apmQuietWhileEqualZero, lIntIfNum, L7_NULLPTR);
          }
          else if (logicalPortInfo->apmState == APM_HELD)
          {
            rc = dot1xIssueCmd(apmQuietWhileEqualZero, lIntIfNum, L7_NULLPTR);
          }

          /* Supplicant reauthentication timer.  Only decrement the timer if reauthentcation is
             enabled and the port is currently authorized. */
          if (logicalPortInfo->reAuthWhen != 0 &&
              dot1xPortInfo[phyIntf].reAuthEnabled == L7_TRUE &&
              logicalPortInfo->logicalPortStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED &&
              logicalPortInfo->guestVlanId == 0 && logicalPortInfo->voiceVlanId == 0)
          {
            logicalPortInfo->reAuthWhen--;
            if (logicalPortInfo->reAuthWhen == 0)
              rc = dot1xIssueCmd(rtmReauthWhenEqualZero, lIntIfNum, L7_NULLPTR);
          }

          /* EAP Request/Identity transmission timer for the logical port */
          if (logicalPortInfo->txWhenLP != 0)
          {
            logicalPortInfo->txWhenLP--;
            if (logicalPortInfo->txWhenLP == 0)
            {
              if (logicalPortInfo->reAuthCount <= dot1xPortInfo[phyIntf].reAuthMax)
              {
                  DOT1X_EVENT_TRACE(DOT1X_TRACE_TIMER,phyIntf,"\n%s:%d EAP Request/Identity transmission timer timed out for the logical port %d\n", __FUNCTION__, __LINE__,
                                    logicalPortInfo->logicalPortNumber);

                  rc = dot1xIssueCmd(apmTxWhenZeroReAuthCountLessThanEqualReAuthMax, lIntIfNum, L7_NULLPTR);
                /*dot1xPortInfo[phyIntf].txWhenPort = dot1xPortInfo[phyIntf].txPeriod;*/
              }
            }
          }

          /* Client Session timeout */
          if (logicalPortInfo->clientTimeout != 0)
          {
              logicalPortInfo->clientTimeout--;
              if (logicalPortInfo->clientTimeout == 0)
              {
                   L7_BOOL trueVal = L7_TRUE;
                  if (dot1xPortInfo[phyIntf].portControlMode  == L7_DOT1X_PORT_AUTO_MAC_BASED)
                  {
                       L7_BOOL timedout = L7_FALSE;
                       if (dtlDot1xIntfClientTimeoutGet(phyIntf,logicalPortInfo->suppMacAddr,&timedout)== L7_SUCCESS)
                       {
                           if (timedout == L7_TRUE)
                           {
                               DOT1X_EVENT_TRACE(DOT1X_TRACE_TIMER,phyIntf,"\n%s:%d Client timed out. Sending initialize to port %d\n", __FUNCTION__, __LINE__,
                                                 logicalPortInfo->logicalPortNumber);
                               rc = dot1xIssueCmd(dot1xMgmtLogicalPortInitializeSet, lIntIfNum, &trueVal);
                           }
                           else if (timedout == L7_FALSE) {
                               /* reset timeout value */
                               logicalPortInfo->clientTimeout = DOT1X_CLIENT_TIMEOUT;
                           }

                       }

                  }
              }
          }
        }
      }

      /* loop only when mac-based in enabled */
      if (dot1xPortInfo[phyIntf].portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)
      {
        break;
      }

    } /* end of loop for the logical interfaces */

   }
   else if (dot1xPortInfo[phyIntf].paeCapabilities == L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
   {
     if (dot1xSupplicantPortInfo[phyIntf].startWhen != 0)
     {
       dot1xSupplicantPortInfo[phyIntf].startWhen--;
       if (dot1xSupplicantPortInfo[phyIntf].startWhen ==0)
       {
          rc = dot1xIssueCmd(spmStartWhenZeroStartCountLessThanMaxStart, phyIntf, L7_NULLPTR);
       }
     }
     if (dot1xSupplicantPortInfo[phyIntf].authWhile != 0)
     {
       dot1xSupplicantPortInfo[phyIntf].authWhile--;
       if (dot1xSupplicantPortInfo[phyIntf].authWhile ==0)
       {
          rc = dot1xIssueCmd(sbmAuthWhileZero, phyIntf, L7_NULLPTR);
       }

     }
     if (dot1xSupplicantPortInfo[phyIntf].heldWhile != 0)
     {
       dot1xSupplicantPortInfo[phyIntf].heldWhile--;
       if (dot1xSupplicantPortInfo[phyIntf].heldWhile ==0)
       {
          rc = dot1xIssueCmd(spmHeldWhileEqualZero, phyIntf, L7_NULLPTR);
       }

     }

   }
    nimRc = dot1xNextValidIntf(phyIntf, &phyIntf);
  } /* End of While loop for all the physical interfaces*/
  return rc;
}

/*********************************************************************
* @purpose  Enable administrative mode setting for dot1x
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlAdminModeEnable()
{
  L7_uint32 intIfNum;
  L7_RC_t nimRc;
  dot1xPortCfg_t *pCfg;

  if (!(DOT1X_IS_READY))
    return L7_SUCCESS;

  if (dtlDot1xModeSet(L7_ENABLE) != L7_SUCCESS)
    return L7_FAILURE;



  /*
   * Notify voice VLAN that dot1x is enabled.
   */
  if (L7_TRUE == cnfgrIsComponentPresent(L7_VOICE_VLAN_COMPONENT_ID))
  {
    voiceVlanDot1xAdminModeCallback(L7_ENABLE);
  }


  nimRc = dot1xFirstValidIntfNumber(&intIfNum);
  while (nimRc == L7_SUCCESS)
  {
    if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    if (pCfg->paeCapabilities == L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
    {
      (void)dot1xPortInfoInitialize(intIfNum,L7_TRUE);
      (void)dot1xCtlApplyPortConfigData(intIfNum);
      (void)dot1xCtlPortGuestVlanTimerStart(intIfNum,pCfg->guestVlanId, DOT1X_PHYSICAL_PORT);
    }
    else if (pCfg->paeCapabilities == L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
    {
       dot1xPortInfo[intIfNum].paeCapabilities = L7_DOT1X_PAE_PORT_SUPP_CAPABLE;
      (void)dot1xSupplicantPortInfoInitialize(intIfNum,L7_TRUE);
      (void)dot1xCtlApplySupplicantPortConfigData(intIfNum);
    }
    nimRc = dot1xNextValidIntf( intIfNum, &intIfNum);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable administrative mode setting for dot1x
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlAdminModeDisable()
{
  L7_uint32 phyIntf,lIntIfNum=0;
  L7_RC_t nimRc;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  /*
   * Notify voice VLAN that dot1x is disabled.
   */
  if (L7_TRUE == cnfgrIsComponentPresent(L7_VOICE_VLAN_COMPONENT_ID))
  {
    voiceVlanDot1xAdminModeCallback(L7_DISABLE);
  }

  if (!(DOT1X_IS_READY))
  {
    if (dtlDot1xModeSet(L7_DISABLE) != L7_SUCCESS)
      return L7_FAILURE;
    return L7_SUCCESS;
  }

  nimRc = dot1xFirstValidIntfNumber(&phyIntf);
  while (nimRc == L7_SUCCESS)
  {
    if (dot1xPortInfo[phyIntf].paeCapabilities == L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
    {
      dot1xPortVlanAssignmentDisable(phyIntf);

      lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
      while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(phyIntf,&lIntIfNum))!= L7_NULLPTR)
      {
        if (logicalPortInfo->inUse == L7_TRUE)
        {

          dot1xIhPortStatusSet(lIntIfNum,L7_DOT1X_PORT_STATUS_UNAUTHORIZED);

          /*remove supplicant mac address from Mac address Database*/
          dot1xMacAddrInfoRemove(&(logicalPortInfo->suppMacAddr));

          /* TBD: Need to address Logocal port vlan assignment */
          dot1xTxCannedSuccess(lIntIfNum, DOT1X_LOGICAL_PORT);
          /* Deallocate memory for Clients */
          dot1xLogicalPortInfoDeAlloc(logicalPortInfo);
        }
     }
     (void) dot1xPortInfoInitialize(phyIntf,L7_TRUE);
     logicalPortInfo= dot1xPhysicalPortInfoNodeGet(phyIntf);
     logicalPortInfo->inUse = L7_TRUE;
     dot1xIhPortStatusSet(logicalPortInfo->logicalPortNumber,L7_DOT1X_PORT_STATUS_AUTHORIZED);
     dot1xTxCannedSuccess(phyIntf, DOT1X_PHYSICAL_PORT);
   }
   else if (dot1xPortInfo[phyIntf].paeCapabilities == L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
   {
     (void) dot1xSupplicantPortInfoInitialize(phyIntf,L7_TRUE);
      dot1xIhPhyPortStatusSet(phyIntf,L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  L7_TRUE);
   }
    nimRc = dot1xNextValidIntf(phyIntf, &phyIntf);
  }
  if (dtlDot1xModeSet(L7_DISABLE) != L7_SUCCESS)
    return L7_FAILURE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set initialize value for a port
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    initialize @b{(input)) initialize value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value is set to L7_TRUE by management in order to force
*           initialization of a port.  It is re-set to L7_FALSE after
*           initialization has completed.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortInitializeSet(L7_uint32 intIfNum, L7_BOOL initialize)
{
  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,intIfNum,"%s:Intialize physical port-%d \n",
                      __FUNCTION__,intIfNum);

  /*if(dot1xPortInfo[intIfNum].guestVlanId != 0)
    (void)dot1xCtlPortGuestVlanTimerStop(intIfNum);*/

  (void)dot1xPortReset(intIfNum);
  return dot1xCtlApplyPortConfigData(intIfNum);
}


/*********************************************************************
* @purpose  Set initialize value for a port
*
* @param    lIntIfNum   @b{(input)) Logical internal interface number
* @param    initialize @b{(input)) initialize value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value is set to L7_TRUE by management in order to force
*           initialization of a port.  It is re-set to L7_FALSE after
*           initialization has completed.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortInitializeSet(L7_uint32 lIntIfNum, L7_BOOL initialize)
{
  L7_uint32 physIntf;
  dot1xLogicalPortInfo_t *logicalPortInfo;


  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
    if(logicalPortInfo)
  {
     physIntf = logicalPortInfo->physPort;
     DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,physIntf,"%s:Intialize Logical port-%d \n",
                      __FUNCTION__,lIntIfNum);
    (void)dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);
    (void)dot1xLogicalPortReset(logicalPortInfo);


    if (dot1xPortInfo[physIntf].portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED &&
        logicalPortInfo->logicalPortNumber == DOT1X_LOGICAL_PORT_START(logicalPortInfo->physPort)
         && initialize == L7_TRUE)
    {
       logicalPortInfo->inUse = L7_TRUE;
    }
    return dot1xCtlApplyLogicalPortConfigData(lIntIfNum);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set reauthentication value for a port
*
* @param    intIfNum       @b{(input)) internal interface number
* @param    reauthenticate @b{(input)) reauthentication value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value is set to L7_TRUE by management in order to force
*           reauthentication of a port.  It is re-set to L7_FALSE after
*           reauthentication has completed.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortReauthenticateSet(L7_uint32 intIfNum, L7_BOOL reauthenticate)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 lIntIfNum;
  /*
  dot1xLogicalPortInfo_t *logicalPortInfo;
  */
  if (dot1xCfg->dot1xAdminMode != L7_ENABLE)
    return L7_SUCCESS;

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while (dot1xLogicalPortInfoGetNextNode(intIfNum, &lIntIfNum) != L7_NULLPTR)
  {
    rc = dot1xCtlLogicalPortReauthenticateSet(lIntIfNum,reauthenticate);
  }
  return rc;
}

/*********************************************************************
* @purpose  Set reauthentication value for a port
*
* @param    intIfNum       @b{(input)) internal interface number
* @param    reauthenticate @b{(input)) reauthentication value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value is set to L7_TRUE by management in order to force
*           reauthentication of a port.  It is re-set to L7_FALSE after
*           reauthentication has completed.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortReauthenticateSet(L7_uint32 lIntIfNum, L7_BOOL reauthenticate)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 physPort;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  if (dot1xCfg->dot1xAdminMode != L7_ENABLE)
    return L7_SUCCESS;

  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);


  if (logicalPortInfo!= L7_NULLPTR)
  {
    physPort = logicalPortInfo->physPort;
    if(logicalPortInfo->inUse == L7_TRUE)
    {
      logicalPortInfo->reAuthenticate = L7_TRUE;

      if (logicalPortInfo->apmState == APM_CONNECTING &&
          logicalPortInfo->reAuthCount <= dot1xPortInfo[physPort].reAuthMax)

      {
          rc = dot1xStateMachineClassifier(apmReauthAndReAuthCountLessThanEqualReAuthMax, lIntIfNum,
                                            L7_NULL, L7_NULLPTR);
      }
      else
      {

        if(((logicalPortInfo->isMABClient == L7_FALSE)&&(logicalPortInfo->guestVlanId == 0))||
           (logicalPortInfo->isMABClient == L7_TRUE))
        {
          rc = dot1xStateMachineClassifier(apmReAuthenticate, lIntIfNum,
                                           L7_NULL, L7_NULLPTR);

          if (logicalPortInfo->isMABClient == L7_TRUE)
          {
            dot1xCtlLogicalPortMABOperational(lIntIfNum);
          }

        }
      }
    }
  }
  else
      rc=L7_FAILURE;
  return rc;
}

/*********************************************************************
* @purpose  Set admin value of controlled directions
*
* @param    intIfNum                  @b{(input)) internal interface number
* @param    adminControlledDirections @b{(input)) controlled directions value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Uni-directional control is currently not supported.
*           The dot1xAuthAdminControlledDirections MIB object is supported
*           as read-only.  Therefore, the API layer will not generate the
*           event which causes this code to run.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortAdminControlledDirectionsSet(L7_uint32 intIfNum,
                                              L7_DOT1X_PORT_DIRECTION_t adminControlledDirections)
{
  if (adminControlledDirections != L7_DOT1X_PORT_DIRECTION_BOTH)
    return L7_FAILURE;

  dot1xPortInfo[intIfNum].adminControlledDirections = adminControlledDirections;
  dot1xPortInfo[intIfNum].operControlledDirections = adminControlledDirections;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set port control mode
*
* @param    intIfNum    @b{(input)) internal interface number
* @param    portControl @b{(input)) port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortControlModeSet(L7_uint32 intIfNum, L7_DOT1X_PORT_CONTROL_t portControl)
{
  L7_uint32 adminState, linkState, session;
  L7_uint32 lIntIfNum;
  L7_RC_t rc = L7_SUCCESS;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  if (portControl == dot1xPortInfo[intIfNum].portControlMode)
    return L7_SUCCESS;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,intIfNum,"%s: control mode set port-%d \n",
                    __FUNCTION__,intIfNum);

  #if defined L7_DOT1AD_PACKAGE
  {
    L7_uint32 nniIntf;
    L7_uchar8 suppStatus;
    dot1adNniInterfaceGet(&nniIntf);
    if (dot1xSupplicantPortSpmStatusGet (nniIntf,&suppStatus) == L7_SUCCESS)
    {
      if (suppStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
      {
         if (intIfNum != nniIntf)
         return rc;
      }
    }
  }
 #endif


  if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
  {
    /* Process control mode only if link is up and enabled */
    if ( (nimGetIntfAdminState(intIfNum, &adminState) == L7_SUCCESS) &&
        (adminState == L7_ENABLE) &&
        (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS) &&
        (linkState == L7_UP) )
    {
      if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
        return L7_SUCCESS;

      /* If this is the probe (mirror dest) port or a member of a LAG, set to force authorized */
      if ((mirrorIsDestConfigured(intIfNum, &session) == L7_TRUE) ||
           (dot3adIsLagMember(intIfNum) == L7_TRUE))
      {
        (void)dot1xStateMachineClassifier(apmPortControlEqualForceAuthorized,
                                          DOT1X_LOGICAL_PORT_START(intIfNum),
                                          L7_NULL, L7_NULLPTR);
        return rc;
      }

      dot1xPortVlanAssignmentDisable(intIfNum);


      if(dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED ||
         portControl == L7_DOT1X_PORT_AUTO_MAC_BASED)
      {
        /* reset all the clients associated with the port */
        dot1xPortInfoInitialize(intIfNum,L7_FALSE);
        for(lIntIfNum = DOT1X_LOGICAL_PORT_START(intIfNum);
            lIntIfNum < DOT1X_LOGICAL_PORT_END(intIfNum);lIntIfNum++)
        {
          dot1xCtlLogicalPortInitializeSet(lIntIfNum,L7_FALSE);
        }
      }
      logicalPortInfo = dot1xPhysicalPortInfoNodeGet(intIfNum);
      lIntIfNum = logicalPortInfo->logicalPortNumber;
      if(portControl != L7_DOT1X_PORT_AUTO_MAC_BASED)
      {
        logicalPortInfo->inUse = L7_TRUE;
      }


      /*
       * Inform voice VLAN that the port control mode has changed.
       */
      if (L7_TRUE == cnfgrIsComponentPresent(L7_VOICE_VLAN_COMPONENT_ID))
      {
        voiceVlanDot1xPortControlCallback(intIfNum, portControl);
      }


      if (portControl == L7_DOT1X_PORT_AUTO)
      {
        (void)dot1xStateMachineClassifier(apmPortControlEqualAutoPortModeNotEqualPortCtrl,
                                          lIntIfNum,L7_NULL, L7_NULLPTR);

      }
      else
      {
        if (portControl == L7_DOT1X_PORT_FORCE_AUTHORIZED)

        {
          (void)dot1xStateMachineClassifier(apmPortControlEqualForceAuthorized, lIntIfNum,
                                            L7_NULL, L7_NULLPTR);
          (void)dot1xCtlPortGuestVlanTimerStop(lIntIfNum);
        }
        else if (portControl == L7_DOT1X_PORT_FORCE_UNAUTHORIZED)
        {
          (void)dot1xStateMachineClassifier(apmPortControlEqualForceUnauthorized, lIntIfNum,
                                            L7_NULL, L7_NULLPTR);
        }
        else
        {
          (void)dot1xStateMachineClassifier(apmPortControlEqualAutoPortModeNotEqualPortCtrl,
                                            lIntIfNum,L7_NULL, L7_NULLPTR);
        }

        (void)dot1xStateMachineClassifier(ktxPortControlNotEqualAuto, lIntIfNum,
                                         L7_NULL, L7_NULLPTR);

        (void)dot1xStateMachineClassifier(rtmPortControlNotEqualAuto, lIntIfNum,
                                         L7_NULL, L7_NULLPTR);

        (void)dot1xStateMachineClassifier(bamPortControlNotEqualAuto, lIntIfNum,
                                         L7_NULL, L7_NULLPTR);

      }
      dot1xPortInfo[intIfNum].portControlMode = portControl;

      if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO)
      {
        (void)dot1xCtlPortGuestVlanTimerStart(intIfNum,pCfg->guestVlanId,DOT1X_PHYSICAL_PORT);
      }

      if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
      {
        dot1xIhPhyPortViolationCallbackSet(intIfNum,L7_TRUE);

        if (pCfg->mabEnabled == L7_ENABLE)
        {
          dot1xCtlPortMABEnableSet(intIfNum);
        }
      }
      else
      {
        if (dot1xPortInfo[intIfNum].mabEnabled == L7_ENABLE)
        {
          dot1xCtlPortMABDisableSet(intIfNum);
        }
      }

    }

  }



  return rc;

}

/*********************************************************************
* @purpose  Set Supplicant port control mode
*
* @param    intIfNum    @b{(input)) internal interface number
* @param    portControl @b{(input)) port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlSupplicantPortControlModeSet(L7_uint32 intIfNum,
                                   L7_DOT1X_PORT_CONTROL_t portControl)
{
  L7_uint32 adminState, linkState;
  L7_RC_t rc = L7_SUCCESS;
  dot1xPortCfg_t *pCfg;

  if (portControl == dot1xSupplicantPortInfo[intIfNum].sPortMode)
    return L7_SUCCESS;

  if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
  {
    /* Process control mode only if link is up and enabled */
    if ( (nimGetIntfAdminState(intIfNum, &adminState) == L7_SUCCESS) &&
        (adminState == L7_ENABLE) &&
        (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS) &&
        (linkState == L7_UP) )
    {
      if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
        return L7_SUCCESS;

      if (portControl == L7_DOT1X_PORT_AUTO)
      {

        (void)dot1xStateMachineClassifier(spmPortControlEqualAutoPortModeNotEqualPortCtrl,
                                          intIfNum,
                                          L7_NULL, L7_NULLPTR);
      }
      else
      {
        if (portControl == L7_DOT1X_PORT_FORCE_AUTHORIZED)
        {
          (void)dot1xStateMachineClassifier(spmPortControlEqualForceAuthorized, intIfNum,
                                            L7_NULL, L7_NULLPTR);
          (void)dot1xStateMachineClassifier(sbmPortControlNotEqualAuto, intIfNum,
                                            L7_NULL, L7_NULLPTR);

        }
        else
        {
          (void)dot1xStateMachineClassifier(spmPortControlEqualForceUnauthorized, intIfNum,
                                            L7_NULL, L7_NULLPTR);
          (void)dot1xStateMachineClassifier(sbmPortControlNotEqualAuto, intIfNum,
                                            L7_NULL, L7_NULLPTR);
        }
      }

    }
  }

  return rc;

}



/*********************************************************************
* @purpose  Set port PAE capabilities (Supplicant/Authenticator)
*
* @param    intIfNum    @b{(input)) internal interface number
* @param    portControl @b{(input)) port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortPaeCapabilitiesSet(L7_uint32 intIfNum,
                                     L7_uint32 paeCapabilities)
{
  dot1xPortCfg_t *pCfg;
  L7_uint32 lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    LOG_MSG ("DOT1x: dot1xCtlPortPaeCapabilitiesSet function failed at  --%d\r\n",__LINE__);
    return L7_FAILURE;
  }
  if (paeCapabilities == L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
  {
    /* Disable previous Supplicant functionality */
    (void) dot1xStateMachineClassifier(sbmSuppAbort, intIfNum, L7_NULL, L7_NULLPTR);
    (void) dot1xSupplicantPortInfoInitialize(intIfNum,L7_TRUE);
    #if defined L7_DOT1AD_PACKAGE
    if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,
                              L7_FEAT_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS))
    {
      DOT1AD_INTFERFACE_TYPE_t  intfType;
      dot1adInterfaceTypeGet(intIfNum, &intfType);
      if(intfType == DOT1AD_INTFERFACE_TYPE_NNI)
      {
         dot1xEnableAuthenticatorPorts(intIfNum);
      }
    }
   #endif
     dot1xIhPhyPortStatusSet(intIfNum,L7_DOT1X_PORT_STATUS_AUTHORIZED,
                                  L7_TRUE);
     dot1xBuildDefaultSupplicantIntfConfigData (&pCfg->configId, pCfg);



    /* Enable back the Authenticator functinality */
    pCfg->paeCapabilities = (L7_uchar8)paeCapabilities;
    dot1xPortInfo[intIfNum].paeCapabilities = (L7_uchar8)paeCapabilities;
    (void)dot1xPortInfoInitialize(intIfNum,L7_TRUE);
    (void)dot1xCtlApplyPortConfigData(intIfNum);
    (void)dot1xCtlPortGuestVlanTimerStart(intIfNum,pCfg->guestVlanId, DOT1X_PHYSICAL_PORT);
  }
  else if(paeCapabilities == L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
  {
    /* Disable previous Authenticator functionality */
    dot1xPortVlanAssignmentDisable(intIfNum);

    lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
    while((logicalPortInfo = dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
    {
      if (logicalPortInfo->inUse == L7_TRUE)
      {

        dot1xIhPortStatusSet(intIfNum,L7_DOT1X_PORT_STATUS_UNAUTHORIZED);

        /*remove supplicant mac address from Mac address Database*/
        dot1xMacAddrInfoRemove(&(logicalPortInfo->suppMacAddr));

        /* TBD: Need to address Logocal port vlan assignment */
        dot1xTxCannedSuccess(lIntIfNum, DOT1X_LOGICAL_PORT);
        /* Deallocate memory for Clients */
        dot1xLogicalPortInfoDeAlloc(logicalPortInfo);
      }
    }
    (void) dot1xPortInfoInitialize(intIfNum,L7_TRUE);
    logicalPortInfo= dot1xPhysicalPortInfoNodeGet(intIfNum);
    logicalPortInfo->inUse = L7_TRUE;
    dot1xIhPortStatusSet(logicalPortInfo->logicalPortNumber,L7_DOT1X_PORT_STATUS_AUTHORIZED);
    dot1xTxCannedSuccess(intIfNum, DOT1X_PHYSICAL_PORT);
    dot1xBuildDefaultIntfConfigData (&pCfg->configId, pCfg);

    /* Enable default supplicant functionality */
    pCfg->paeCapabilities = (L7_uchar8)paeCapabilities;
    dot1xPortInfo[intIfNum].paeCapabilities = (L7_uchar8)paeCapabilities;
    dot1xSupplicantPortInfoInitialize(intIfNum, L7_TRUE);
    dot1xCtlApplySupplicantPortConfigData(intIfNum);

  }
  dot1xCfg->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set quiet period value
*
* @param    intIfNum    @b{(input)) internal interface number
* @param    quietPeriod @b{(input)) quiet period
*
* @returns  L7_SUCCESS
*
* @comments The quietPeriod is the initialization value for quietWhile,
*           which is a timer used by the Authenticator state machine
*           to define periods of time in which it will not attempt to
*           acquire a Supplicant.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortQuietPeriodSet(L7_uint32 intIfNum, L7_uint32 quietPeriod)
{
  L7_uint32  lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  dot1xPortInfo[intIfNum].quietPeriod = quietPeriod;

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
  {
    if(logicalPortInfo->inUse == L7_TRUE)
    {
      logicalPortInfo->quietWhile = quietPeriod;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set tx period value
*
* @param    intIfNum @b{(input)) internal interface number
* @param    txPeriod @b{(input)) tx period
*
* @returns  L7_SUCCESS
*
* @comments The txPeriod is the initialization value for txWhen,
*           which is a timer used by the Authenticator state machine
*           to determine when to send an EAPOL EAP Request/Identity
*           frame to the Supplicant.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortTxPeriodSet(L7_uint32 intIfNum, L7_uint32 txPeriod)
{
  L7_uint32  lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  dot1xPortInfo[intIfNum].txPeriod = txPeriod;

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
  {
    if(logicalPortInfo->inUse == L7_TRUE)
    {
      logicalPortInfo->txWhenLP = txPeriod;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set Supplicant timeout value
*
* @param    intIfNum    @b{(input)) internal interface number
* @param    suppTimeout @b{(input)) Supplicant timeout
*
* @returns  L7_SUCCESS
*
* @comments The suppTimeout is the initialization value for aWhile,
*           which is a timer used by the Authenticator state machine
*           to time out the Supplicant.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortSuppTimeoutSet(L7_uint32 intIfNum, L7_uint32 suppTimeout)
{
  L7_uint32  lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  dot1xPortInfo[intIfNum].suppTimeout = suppTimeout;

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
  {
    if(logicalPortInfo->inUse == L7_TRUE &&
       logicalPortInfo->bamState == BAM_REQUEST)
    {
      logicalPortInfo->aWhile = suppTimeout;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set Authentiation Server timeout value
*
* @param    intIfNum      @b{(input)) internal interface number
* @param    serverTimeout @b{(input)) Authentication Server timeout
*
* @returns  L7_SUCCESS
*
* @comments The serverTimeout is the initialization value for aWhile,
*           which is a timer used by the Authenticator state machine
*           to time out the Authentiation Server.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortServerTimeoutSet(L7_uint32 intIfNum, L7_uint32 serverTimeout)
{
  L7_uint32  lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  dot1xPortInfo[intIfNum].serverTimeout = serverTimeout;

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
  {
    if(logicalPortInfo->inUse == L7_TRUE &&
       logicalPortInfo->bamState == BAM_RESPONSE)
    {
      logicalPortInfo->aWhile = serverTimeout;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set Maximum Request value
*
* @param    intIfNum @b{(input)) internal interface number
* @param    maxReq   @b{(input)) maximum request value
*
* @returns  L7_SUCCESS
*
* @comments The maxReq value is the number of times the Authenticator
*           state machine will retransmit an EAPOL EAP Request/Identity
*           before timing out the Supplicant.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortMaxReqSet(L7_uint32 intIfNum, L7_uint32 maxReq)
{
  dot1xPortInfo[intIfNum].maxReq = maxReq;
  dot1xPortInfo[intIfNum].reAuthMax = maxReq;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Reauthentication period
*
* @param    intIfNum      @b{(input)) internal interface number
* @param    reAuthPeriod  @b{(input)) reauthentication period
*
* @returns  L7_SUCCESS
*
* @comments The reAuthPeriod is the initialization value for reAuthWhen,
*           which is a timer used by the Authenticator state machine to
*           determine when reauthentication of the Supplicant takes place.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortReAuthPeriodSet(L7_uint32 intIfNum, L7_uint32 reAuthPeriod)
{
  L7_uint32  lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  dot1xPortInfo[intIfNum].reAuthPeriod = reAuthPeriod;

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
  {
    if(logicalPortInfo->inUse == L7_TRUE)
    {
      logicalPortInfo->reAuthWhen = reAuthPeriod;
    }
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the Reauthentication mode
*
* @param    intIfNum       @b{(input)) internal interface number
* @param    reAuthEnabled  @b{(input)) reauthentication mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The reAuthEnabled mode determines whether reauthentication
*           of the Supplicant takes place.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortReAuthEnabledSet(L7_uint32 intIfNum, L7_BOOL reAuthEnabled)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32  lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  dot1xPortInfo[intIfNum].reAuthEnabled = reAuthEnabled;

  /* Whenever the reAuthEnabled setting is changed, reset the reAuthWhen timer */

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
  {
    logicalPortInfo->reAuthWhen = dot1xPortInfo[intIfNum].reAuthPeriod;

    if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
    {
      if (reAuthEnabled == L7_FALSE)

        rc = dot1xStateMachineClassifier(rtmNotReauthEnabled, lIntIfNum,
            L7_NULL, L7_NULLPTR);

    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the Key Transmission mode
*
* @param    intIfNum     @b{(input)) internal interface number
* @param    keyTxEnabled @b{(input)) key transmission mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The keyTranmissionEnabled mode determines whether key
*           transmission to the Supplicant takes place.  Note that
*           key transmission is not supported.  The dot1xAuthKeyTxEnabled
*           MIB object is supported as read-only.  Therefore, the API
*           layer will not generate the event which causes this code to run.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortKeyTransmissionEnabledSet(L7_uint32 intIfNum, L7_BOOL keyTxEnabled)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32  lIntIfNum;

  dot1xPortInfo[intIfNum].keyTxEnabled = keyTxEnabled;

  if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
  {
    if (keyTxEnabled == L7_FALSE)
    {
      for(lIntIfNum = DOT1X_LOGICAL_PORT_START(intIfNum);
          lIntIfNum < DOT1X_LOGICAL_PORT_END(intIfNum);lIntIfNum++)
      {
        rc = dot1xStateMachineClassifier(ktxNotKeyTxEnabled, lIntIfNum, L7_NULL, L7_NULLPTR);
      }

    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Clear dot1x stats for specified port
*
* @param    intIfNum     @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortStatsClear(L7_uint32 intIfNum)
{
  bzero((L7_char8 *)&dot1xPortStats[intIfNum], sizeof(dot1xPortStats_t));
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply dot1x config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlApplyConfigData(void)
{
  L7_uint32 cfgIndex;
  L7_uint32 intIfNum;
  nimConfigID_t configIdNull;
  L7_RC_t rc;

  /* Apply the global admin mode for dot1x */
  if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
  {
    rc = dot1xCtlAdminModeEnable();
  }
  else
    rc = dot1xCtlAdminModeDisable();

  if (rc != L7_SUCCESS)
  {
    L7_uchar8 *modeStr = (dot1xCfg->dot1xAdminMode == L7_ENABLE) ? (L7_uchar8 *)"enable" : (L7_uchar8 *)"disable";
    LOG_MSG("dot1xApplyConfigData: Unable to %s dot1x in driver\n", (L7_char8 *)modeStr);
  }

  /* Now check for ports that are valid for dot1x.  All non-zero values in the
   * config structure contain a port config ID that is valid for dot1x.
   * Convert this to internal interface number and apply the port config.
   * The internal interface number is used as an index into the operational
   * interface data.
   */
  if (DOT1X_IS_READY)
  {
    memset(&configIdNull, 0, sizeof(nimConfigID_t));

    for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&dot1xCfg->dot1xPortCfg[cfgIndex].configId, &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&(dot1xCfg->dot1xPortCfg[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      if (dot1xCfg->dot1xPortCfg[cfgIndex].paeCapabilities == L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
      {
        if (dot1xCtlApplyPortConfigData(intIfNum) != L7_SUCCESS)
         {
          LOG_MSG("Failure in applying config data for interface %u\n", intIfNum);
         }
      }
      else
      {
         dot1xPortInfo[intIfNum].paeCapabilities=L7_DOT1X_PAE_PORT_SUPP_CAPABLE;
         dot1xSupplicantPortInfoInitialize(intIfNum, L7_TRUE);
         dot1xCtlApplySupplicantPortConfigData(intIfNum);
     }
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Apply dot1x config data to specified interface
*
* @param    intIfNum     @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlApplyPortConfigData(L7_uint32 intIfNum)
{
  dot1xPortCfg_t *pCfg;
  L7_uint32 session,lIntIfNum;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_SUCCESS;

  logicalPortInfo = dot1xPhysicalPortInfoNodeGet(intIfNum);
  if(logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Copy config data into operational data */
  if (pCfg->portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)
 {
    dot1xPortInfo[intIfNum].maxUsers = 1;
 }
  else
 {
    dot1xPortInfo[intIfNum].maxUsers = pCfg->maxUsers;
  }

  dot1xPortInfo[intIfNum].paeCapabilities = pCfg->paeCapabilities;
  dot1xPortInfo[intIfNum].adminControlledDirections = pCfg->adminControlledDirections;
  dot1xPortInfo[intIfNum].quietPeriod = pCfg->quietPeriod;
  dot1xPortInfo[intIfNum].txPeriod = pCfg->txPeriod;
  dot1xPortInfo[intIfNum].suppTimeout = pCfg->suppTimeout;
  dot1xPortInfo[intIfNum].serverTimeout = pCfg->serverTimeout;
  dot1xPortInfo[intIfNum].maxReq = pCfg->maxReq;
  dot1xPortInfo[intIfNum].reAuthPeriod = pCfg->reAuthPeriod;
  dot1xPortInfo[intIfNum].reAuthEnabled = pCfg->reAuthEnabled;
  dot1xPortInfo[intIfNum].keyTxEnabled = pCfg->keyTxEnabled;
  dot1xPortInfo[intIfNum].guestVlanPeriod = pCfg->guestVlanPeriod;


  /*(void)dot1xPortReset(intIfNum);*/

  if (dot1xCfg->dot1xAdminMode != L7_ENABLE )
  {
     logicalPortInfo->inUse = L7_TRUE;
     dot1xIhPortStatusSet(logicalPortInfo->logicalPortNumber,L7_DOT1X_PORT_STATUS_AUTHORIZED);
     dot1xPortInfo[intIfNum].portControlMode = pCfg->portControlMode;
     return L7_SUCCESS;
  }
  if (pCfg->portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)
  {
    logicalPortInfo->inUse = L7_TRUE;
    dot1xPortInfo[intIfNum].portControlMode = pCfg->portControlMode;
  }
  else
  {
    /* called to set the port status correctly in the hardware and send nim event */
    dot1xPortInfo[intIfNum].portControlMode = 0;
    dot1xIhPhyPortStatusSet(intIfNum,L7_DOT1X_PORT_STATUS_UNAUTHORIZED,L7_TRUE);
    dot1xPortInfo[intIfNum].portControlMode = pCfg->portControlMode;
    dot1xIhPhyPortViolationCallbackSet(intIfNum,L7_TRUE);
  }


  /* If this is the probe (mirror dest) port, set to force authorized */
  if ((mirrorIsDestConfigured(intIfNum, &session) == L7_TRUE) ||
       (dot3adIsLagActiveMember(intIfNum) == L7_TRUE))
  {
     (void)dot1xStateMachineClassifier(apmPortControlEqualForceAuthorized, logicalPortInfo->logicalPortNumber,
                                     L7_NULL, L7_NULLPTR);
  }
  else
  {
    /* Initialize state machines */
    lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
    while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
    {
      if(logicalPortInfo->inUse == L7_TRUE)
        dot1xCtlApplyLogicalPortConfigData(lIntIfNum);
    }
  }

  if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
  {
     dot1xPortInfo[intIfNum].mabEnabled = pCfg->mabEnabled;
  }
  /* Return success. The underlying routines will log appropriate messages */
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Process the Vlan Assignment
*
* @param    vlanId    @b{(input)} Vlan Id assigned.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1xRadiusServerVlanAssignmentHandle(L7_uint32 intIfNum, L7_uint32 vlanId)
{
  DOT1Q_SWPORT_MODE_t curr_mode;
  /* Radius server has sent the vlan assignment for the authentication */
  /* validate the vlan id given by the server */
  if ((dot1qStaticVlanCheckValid(vlanId) == L7_SUCCESS) &&
      (dot1qSwitchPortModeGet(intIfNum,&curr_mode)== L7_SUCCESS) &&
      (((dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED) &&
        (curr_mode == DOT1Q_SWPORT_MODE_GENERAL)) ||
       ((dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO) &&
        (curr_mode != DOT1Q_SWPORT_MODE_TRUNK))))
  {
    /* valid vlan is assigned ok */
    return L7_SUCCESS;
  }
  /* Assigned vlan is not valid so generate the E37 event */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Apply dot1x vlan assignment to a specified interface
*
* @param    llogicalPortInfo  @b{(input))  Logical Port Info node
* @param    vlanId     @b{(input)) vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xVlanAssignmentEnable(dot1xLogicalPortInfo_t *logicalPortInfo,L7_uint32 vlanId)
{
  L7_uint32 currentVlanId = 0;
  L7_uint32 intIfNum  = logicalPortInfo->physPort;


  currentVlanId = logicalPortInfo->vlanAssigned;

  if (currentVlanId == 0 && vlanId != 0)
  {
    dot1xVlanIntfCount[vlanId]++;
    L7_VLAN_SETMASKBIT(dot1xVlanMask,vlanId);
  }
  else if (currentVlanId != 0 && vlanId == 0)
  {
    dot1xVlanIntfCount[currentVlanId]--;
    if (dot1xVlanIntfCount[currentVlanId] == 0)
    {
      L7_VLAN_CLRMASKBIT(dot1xVlanMask, currentVlanId);
    }
  }
  else if (currentVlanId != 0 && vlanId != 0)
  {
    dot1xVlanIntfCount[currentVlanId]--;
    if (dot1xVlanIntfCount[currentVlanId] == 0)
    {
      L7_VLAN_CLRMASKBIT(dot1xVlanMask, currentVlanId);
    }
    dot1xVlanIntfCount[vlanId]++;
    L7_VLAN_SETMASKBIT(dot1xVlanMask,vlanId);
  }


  logicalPortInfo->vlanAssigned = vlanId;

  /*reset values of other vlans in the structure*/
  if (logicalPortInfo->unauthVlan !=0)
  {
    dot1xCtlLogicalPortUnauthenticatedVlanReset(logicalPortInfo->logicalPortNumber);
  }


  if (logicalPortInfo->vlanId !=0)
    logicalPortInfo->vlanId=0;

  if(dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO)
  {
     dot1xCtlPortDot1xAcquire(intIfNum,vlanId);
  }
  else
  {
    /* Need to delete this port from vlan participation */
    return dot1xMacBasedVlanParticipationSet(logicalPortInfo->logicalPortNumber,vlanId);

  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply dot1x vlan assignment to a specific logical interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    vlanId     @b{(input)) vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This call should happen only in mac-based mode.
*
* @end
*********************************************************************/
static
L7_RC_t dot1xVlanPortAddProcess(L7_uint32 intIfNum,L7_uint32 vlanId)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  remove dot1x vlan assignment to a specific logical interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    vlanId     @b{(input)) vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This call should happen only in mac-based mode.
*
* @end
*********************************************************************/
static
L7_RC_t dot1xVlanPortDeleteProcess(L7_uint32 intIfNum,L7_uint32 vlanId)
{
  L7_uint32              lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE,mode;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  /* check to see if the delete event is because dot1q set it to forbidden mode */
  if(dot1qVlanMemberGet(vlanId,intIfNum,&mode)==L7_SUCCESS && mode == L7_DOT1Q_FORBIDDEN)
  {
    while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
    {
      /* if the vlan id is being used in any of the modes then disable */
      if((logicalPortInfo->vlanAssigned == vlanId)||
         (logicalPortInfo->unauthVlan == vlanId) ||
         (logicalPortInfo->voiceVlanId == vlanId)||
         (logicalPortInfo->vlanId == vlanId)||
         ((logicalPortInfo->isMABClient == L7_TRUE) && (logicalPortInfo->guestVlanId == vlanId)))
      {
        logicalPortInfo->vlanAssigned = logicalPortInfo->unauthVlan = logicalPortInfo->vlanId = logicalPortInfo->voiceVlanId = 0;
        logicalPortInfo->unauthReason = 0;
        if ((logicalPortInfo->isMABClient == L7_TRUE) && (logicalPortInfo->guestVlanId == vlanId))
        {
          logicalPortInfo->guestVlanId =0;
        }
        /* Generate the vlan delete event to reset the state machines */
        dot1xStateMachineClassifier(bamAssignedVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
        dot1xStateMachineClassifier(apmAssignedVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply dot1x vlan assignment to a specific vlan
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    vlanId     @b{(input)) vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xVlanAssignmentDisable(L7_uint32 intIfNum,L7_uint32 vlanId)
{
  L7_RC_t                 nimRc, rc = L7_SUCCESS;
  L7_uint32               intf = 0;
  L7_uint32               lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  nimRc = dot1xFirstValidIntfNumber(&intf);
  while (nimRc == L7_SUCCESS)
  {
    if ((dot1xPortInfo[intf].portControlMode == L7_DOT1X_PORT_AUTO ||
         dot1xPortInfo[intf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)&&
        dot1xPortInfo[intf].portEnabled == L7_TRUE)
    {
      lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
      while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intf,&lIntIfNum))!= L7_NULLPTR)
      {
       /* add check for unauthenticated vlan*/
        if ((logicalPortInfo->vlanAssigned == vlanId)||
            (logicalPortInfo->unauthVlan == vlanId) ||
            (logicalPortInfo->voiceVlanId == vlanId) ||
            (logicalPortInfo->vlanId == vlanId)||
            ((logicalPortInfo->isMABClient == L7_TRUE)&&(logicalPortInfo->guestVlanId == vlanId)))
        {
            if (logicalPortInfo->vlanAssigned == vlanId)
            {
                 logicalPortInfo->vlanAssigned = 0;

                 dot1xVlanIntfCount[vlanId]--;
                 if (dot1xVlanIntfCount[vlanId] == 0)
                 {
                   L7_VLAN_CLRMASKBIT(dot1xVlanMask, vlanId);
                 }
            }
            else if (logicalPortInfo->unauthVlan == vlanId)
            {
                logicalPortInfo->unauthVlan = 0;
                logicalPortInfo->unauthReason = 0;
            }
            else if (logicalPortInfo->vlanId == vlanId)
            {
                if (dot1xPortInfo[intf].portControlMode == L7_DOT1X_PORT_AUTO)
                {
                    logicalPortInfo->vlanId = L7_DOT1Q_DEFAULT_VLAN;
                    continue;
                }
                else if (dot1xPortInfo[intf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
                {
                    logicalPortInfo->vlanId = 0;
                }

            }
          if (logicalPortInfo->voiceVlanId == vlanId)
          {
            logicalPortInfo->voiceVlanId = 0;

            voiceVlanPortClientAuthSet(intIfNum, logicalPortInfo->suppMacAddr.addr, L7_FALSE);
            dot1xMacBasedVlanParticipationReset(logicalPortInfo,vlanId);
            dot1xStateMachineClassifier(apmAssignVoiceVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
          }
          else if ((logicalPortInfo->isMABClient == L7_TRUE) && (logicalPortInfo->guestVlanId == vlanId))
          {
             logicalPortInfo->guestVlanId =0;
          }
          else
          {
              /* Generate the vlan delete event to reset the state machines */
              dot1xStateMachineClassifier(bamAssignedVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
              dot1xStateMachineClassifier(apmAssignedVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);

              L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                "Logical Port-%u moved from authenticated state as assigned vlan %u is deleted."
                " Dot1x assigned vlan to the logical port (client) was deleted",
                      lIntIfNum,vlanId);


              if(dot1xPortInfo[intf].portControlMode == L7_DOT1X_PORT_AUTO &&
                 dot1xPortInfo[intf].dot1xAcquire == L7_TRUE)
              {
                  dot1xPortVlanMembershipSet(intf,vlanId,L7_FALSE);
                  dot1xCtlPortDot1xRelease(intf,vlanId);
              }
              else if(dot1xPortInfo[intf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
              {
                  /* Need to delete this port from vlan participation */
                  dot1xMacBasedVlanParticipationReset(logicalPortInfo,vlanId);

              }
            }
          }


      }
    }
    nimRc = dot1xNextValidIntf(intf, &intf);
  }
  return rc;
}


/*********************************************************************
* @purpose  Disable dot1x vlan assignment to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xPortVlanAssignmentDisable(L7_uint32 intIfNum)
{
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       vlanId = 0;
  L7_uint32       lIntIfNum=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
  {
    if ((logicalPortInfo->vlanAssigned != 0) || (logicalPortInfo->unauthVlan !=0) ||
        (logicalPortInfo->vlanId != 0) || (logicalPortInfo->guestVlanId != 0) ||
        (logicalPortInfo->voiceVlanId != 0))
    {
      vlanId =0;
      if (logicalPortInfo->vlanAssigned != 0)
      {
        vlanId = logicalPortInfo->vlanAssigned;
        logicalPortInfo->vlanAssigned = 0;
      }
      else if (logicalPortInfo->unauthVlan !=0)
      {
        vlanId = logicalPortInfo->unauthVlan;
        logicalPortInfo->unauthVlan=0;
        logicalPortInfo->unauthReason = 0;
      }
      else if (logicalPortInfo->guestVlanId !=0)
      {
        vlanId = logicalPortInfo->guestVlanId;
        logicalPortInfo->guestVlanId=0;
      }
      else if (logicalPortInfo->vlanId != 0)
      {
        vlanId = logicalPortInfo->vlanId;
        logicalPortInfo->vlanId = 0;
      }

      if (logicalPortInfo->voiceVlanId !=0)
      {
        vlanId = logicalPortInfo->voiceVlanId;
        logicalPortInfo->voiceVlanId=0;

        voiceVlanPortClientAuthSet(intIfNum, logicalPortInfo->suppMacAddr.addr, L7_FALSE);
        dot1xMacBasedVlanParticipationReset(logicalPortInfo,vlanId);
        dot1xStateMachineClassifier(apmAssignVoiceVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
      else
      {

          /* Generate the vlan delete event to reset the state machines */
          dot1xStateMachineClassifier(bamAssignedVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
          dot1xStateMachineClassifier(apmAssignedVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);

          if(dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO &&
             dot1xPortInfo[intIfNum].dot1xAcquire == L7_TRUE)
          {
              dot1xPortVlanMembershipSet(intIfNum,vlanId,L7_FALSE);
              dot1xCtlPortDot1xRelease(intIfNum,vlanId);
          }
          else if(dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
          {
              /* Need to delete this port from vlan participation */
              dot1xMacBasedVlanParticipationReset(logicalPortInfo,vlanId);

          }
      }
    }
  }
  return rc;

}
/*********************************************************************
* @purpose  Processes Dot1x-related event initiated by text based
*           configuration
*
* @param (in)    event
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dot1xApplyConfigCompleteCb(L7_uint32 event)
{

  if ( (event != TXT_CFG_APPLY_FAILURE) &&
       (event != TXT_CFG_APPLY_SUCCESS) )
  {
    return L7_SUCCESS;
  }
  return dot1xAdminModeSet(L7_ENABLE);
}


/*********************************************************************
* @purpose  Processes Dot1x-related event initiated by Dot1Q
*
* @param (in)    vlanId    Virtual LAN Id
* @param (in)    intIfNum  Interface Number
* @param (in)    event
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dot1xVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event)
{
  if (!(DOT1X_IS_READY))
  {
    LOG_MSG("dot1xVlanChangeCallback: Received an vlan change callback while not ready to receive it");
    return L7_FAILURE;
  }

  DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,0,
                      "dot1xVlanChangeCallback:Received Vlan event %d for interface %d \n",
                       event,intIfNum);
  if ( dot1xCfg->dot1xAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }

  if ((intIfNum!=0) && (dot1xPortInfo[intIfNum].portEnabled != L7_TRUE) )
  {
    return L7_SUCCESS;
  }

  switch (event)
  {
  case VLAN_DELETE_PENDING_NOTIFY:
    DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,0,
                      "Received Vlan Delete Notify \n");
    dot1xIssueCmd(dot1xVlanDeleteEvent,intIfNum,vlanData);
    break;

  case VLAN_ADD_NOTIFY:
    DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,0,
                      "Received Vlan Add Notify \n");
    dot1xIssueCmd(dot1xVlanAddEvent,intIfNum,vlanData);
    break;

  case VLAN_ADD_PORT_NOTIFY:
    if (dot1xCfg->vlanAssignmentMode == L7_ENABLE &&
        dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
    {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,intIfNum,
                        "Received Vlan Add Port Notify for Port %d \n",
                        intIfNum);
      dot1xIssueCmd(dot1xVlanAddPortEvent,intIfNum,vlanData);
    }
    break;

  case VLAN_DELETE_PORT_NOTIFY:
    if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
    {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,intIfNum,
                        "Received Vlan Delete Port Notify for Port %d\n",
                        intIfNum);
      dot1xIssueCmd(dot1xVlanDeletePortEvent,intIfNum,vlanData);
    }
    break;

  case VLAN_PVID_CHANGE_NOTIFY:
      if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
      {
          DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,intIfNum,
                        "Received Vlan PVID Change Notify for Port %d \n",
                        intIfNum);
          dot1xIssueCmd(dot1xVlanPvidChangeEvent,intIfNum,vlanData);
      }
  default:
    break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable  dot1x guest vlan to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    guestVlanId @b{(input)} guest vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortGuestVlanMode(L7_uint32 intIfNum,L7_uint32 VlanId)
{
  L7_uint32 retVal = L7_SUCCESS;
  dot1xLogicalPortInfo_t *logicalPortInfo = dot1xPhysicalPortInfoNodeGet(intIfNum);

  if(logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* If the port is in mac-based mode then clean up the state machines */
  if(dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
  {
    /* This is to flush the learnt fdb entries */
    dtlDot1sFlush(intIfNum);
  }

  if (dot1xPortInfo[intIfNum].portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED &&
      logicalPortInfo->guestVlanId  != 0)
  {


    if (dot1qStaticVlanCheckValid(VlanId) == L7_SUCCESS)
    {
      dot1xPortVlanMembershipSet(intIfNum,logicalPortInfo->guestVlanId,L7_FALSE);
      retVal =dot1xCtlPortDot1xAcquire(intIfNum,VlanId);
      if (retVal == L7_SUCCESS)
      {
        logicalPortInfo->guestVlanId = VlanId;
        dot1xPortVlanMembershipSet(intIfNum,VlanId,L7_TRUE);
      }
    }
    else
    {
      return dot1xCtlPortGuestVlanRemove(DOT1X_LOGICAL_PORT_START(intIfNum),0);
    }
  }
  else if (dot1xPortInfo[intIfNum].portStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
  {
    (void)dot1xCtlPortGuestVlanTimerStart(intIfNum,VlanId,DOT1X_PHYSICAL_PORT);

  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable  dot1x guest vlan to a specified Logical interface
*
* @param    logicalPortInfo   @b{(input)) logical port interface node
* @param    guestVlanId       @b{(input)} guest vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortGuestVlanMode(dot1xLogicalPortInfo_t *logicalPortInfo,L7_uint32 VlanId)
{
  L7_uint32 intIfNum =  logicalPortInfo->physPort;

  if (dot1xPortInfo[intIfNum].portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED &&
      logicalPortInfo->guestVlanId  != 0)
  {
    dot1xCtlPortGuestVlanRemove(logicalPortInfo->logicalPortNumber,logicalPortInfo->guestVlanId);
    if (dot1qStaticVlanCheckValid(VlanId) == L7_SUCCESS)
    {
      /* post a event to restart the guest vlan timer */
      dot1xCtlPortGuestVlanTimerStart(logicalPortInfo->logicalPortNumber,VlanId,DOT1X_LOGICAL_PORT);
    }
  }
  else
  {
    (void)dot1xCtlPortGuestVlanTimerStart(logicalPortInfo->logicalPortNumber,VlanId,DOT1X_LOGICAL_PORT);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set  dot1x guest vlan to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    guestVlanId @b{(input)} guest vlan id
* @param    type        @b{(input)} Port type physical or logical
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortGuestVlanTimerStart(L7_uint32 ifindex,L7_uint32 VlanId,dot1xPortType_t type)
{

  dot1xPortCfg_t *pCfg;
  L7_uint32 intIfNum,lIntIfNum;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  if (type == DOT1X_LOGICAL_PORT)
  {
    lIntIfNum = ifindex;
    intIfNum = dot1xPhysPortGet(ifindex);
  }
  else
  {
    intIfNum = ifindex;
    lIntIfNum = DOT1X_LOGICAL_PORT_START(intIfNum);
  }

  if ((logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum))== L7_NULLPTR)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d Failed to start the Guest Vlan Timer for %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,logicalPortInfo->physPort,
                    "%s:%d Called for %d with type - %d \n",
                    __FUNCTION__,__LINE__,lIntIfNum,type);

  if ((dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE) ||
      (dot1xPortInfo[intIfNum].portEnabled != L7_TRUE)      ||
      (dot1qStaticVlanCheckValid(VlanId) != L7_SUCCESS)     ||
      (pCfg->guestVlanId != VlanId))
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d Failed to start the Guest Vlan Timer for %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }

  if (type == DOT1X_PHYSICAL_PORT &&
      dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO)
  {
    if (dot1xPortInfo[intIfNum].portStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
    {
      /*start the timer */
      dot1xPortInfo[intIfNum].guestVlanPeriod = pCfg->guestVlanPeriod;
      logicalPortInfo->guestVlanTimer = dot1xPortInfo[intIfNum].guestVlanPeriod;
      DOT1X_EVENT_TRACE(DOT1X_TRACE_TIMER,intIfNum,
                        "%s:Started the GuestVlan Timer with value = %d for %d \n",
                        __FUNCTION__,dot1xPortInfo[intIfNum].guestVlanPeriod,intIfNum);
    }
    else if (dot1xPortInfo[intIfNum].portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
    {
      /* need to replace the Guest vlan id */
      if (logicalPortInfo->guestVlanId != 0)
        dot1xCtlPortGuestVlanMode(intIfNum,VlanId);
    }
  }
  else if (type == DOT1X_LOGICAL_PORT &&
           dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
  {
    if (logicalPortInfo->guestVlanId == 0)
    {
      /* start the timer */
      logicalPortInfo->guestVlanTimer = dot1xPortInfo[intIfNum].guestVlanPeriod;
      DOT1X_EVENT_TRACE(DOT1X_TRACE_TIMER,intIfNum,
                        "%s:Started the GuestVlan Timer with value = %d for linterface = %d \n",
                        __FUNCTION__,dot1xPortInfo[intIfNum].guestVlanPeriod,lIntIfNum);
    }

    else
    {
      /* need to replace the Guest vlan id */
      dot1xCtlLogicalPortGuestVlanMode(logicalPortInfo,VlanId);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Stop dot1x guest vlan timer on the specified interface
*
* @param    intIfNum   @b{(input)) Logical internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortGuestVlanTimerStop(L7_uint32 lIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  if ((logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum))== L7_NULLPTR)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE, 0,
                      "%s:%d Failed to start the Guest Vlan Timer for %d \n",
                      __FUNCTION__, __LINE__, lIntIfNum);
    return L7_FAILURE;
  }
  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,logicalPortInfo->physPort,
                      "%s:%d Called for %d \n",
                      __FUNCTION__,__LINE__,lIntIfNum);

  logicalPortInfo->guestVlanTimer = 0;
  if(logicalPortInfo->guestVlanId != 0)
  {

    if(dot1xPortInfo[logicalPortInfo->physPort].portControlMode == L7_DOT1X_PORT_AUTO)
    {
      dot1xPortVlanMembershipSet(logicalPortInfo->physPort,logicalPortInfo->guestVlanId,L7_FALSE);
      rc = dot1xCtlPortDot1xRelease(logicalPortInfo->physPort,logicalPortInfo->guestVlanId);
    }
    else
    {
      rc = dot1xMacBasedVlanParticipationReset(logicalPortInfo,logicalPortInfo->guestVlanId);
    }
    logicalPortInfo->guestVlanId = 0;
  }

  return rc;
}

/*********************************************************************
* @purpose  Set  dot1x guest vlan to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    guestVlanId @b{(input)} guest vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortGuestVlanOperational(L7_uint32 lIntIfNum)
{
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physIntf;
  L7_RC_t rc = L7_SUCCESS;


  if((logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum))==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  physIntf =  logicalPortInfo->physPort;
  if (dot1xIntfIsConfigurable(physIntf, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if(dot1qStaticVlanCheckValid(pCfg->guestVlanId) == L7_SUCCESS)
  {
    if(dot1xPortInfo[physIntf].portControlMode == L7_DOT1X_PORT_AUTO)
    {
      dot1xCtlPortDot1xAcquire(logicalPortInfo->physPort,pCfg->guestVlanId);
      rc = dot1xPortVlanMembershipSet(physIntf,pCfg->guestVlanId,L7_TRUE);

    }
    else
    {
      /* Mac-based mode */
      rc = dot1xMacBasedVlanParticipationSet(lIntIfNum,pCfg->guestVlanId);
    }
    if (rc == L7_SUCCESS)
    {
      logicalPortInfo->guestVlanId = pCfg->guestVlanId;
      DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,logicalPortInfo->physPort,"Authenticated the logical port %d on Guest Vlan\n",lIntIfNum);
      dot1xStateMachineClassifier(bamGuestVlanTimerExpired,
                                  lIntIfNum, L7_NULL, L7_NULLPTR);
      dot1xStateMachineClassifier(apmGuestVlanTimerExpired,
                                  lIntIfNum,L7_NULL,L7_NULLPTR);
    }
    else
    {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,logicalPortInfo->physPort,
                        "Could not authenticate logical port %d and assign port to guest vlan as vlan assignment on port %d failed.\n",
                        lIntIfNum,logicalPortInfo->physPort);
       dot1xStateMachineClassifier(apmEapLogoff,
                                  lIntIfNum,L7_NULL,L7_NULLPTR);
    }
  }
  return rc;
}

/*********************************************************************
 * @purpose  Disable  dot1x guest vlan to a specified interface
 *
 * @param    lIntIfNum   @b{(input)) Logical internal interface number
* @param    guestVlanId @b{(input)} guest vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortGuestVlanModeDisable(L7_uint32 intIfNum,L7_uint32 guestVlanId)
{
  L7_uint32 lIntIfNum;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  if(dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO)
  {
    dot1xCtlPortGuestVlanRemove(DOT1X_LOGICAL_PORT_START(intIfNum),guestVlanId);
  }
  else if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
  {
    lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
    while ((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
    {
      if (logicalPortInfo->inUse == L7_TRUE && logicalPortInfo->guestVlanId != 0)
        dot1xCtlPortGuestVlanRemove(logicalPortInfo->logicalPortNumber,logicalPortInfo->guestVlanId);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Disable  dot1x guest vlan to a specified interface
 *
 * @param    lIntIfNum   @b{(input)) Logical internal interface number
* @param    guestVlanId @b{(input)} guest vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortGuestVlanRemove(L7_uint32 lIntIfNum, L7_uint32 guestVlanId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32       vlanId = 0;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;


  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (dot1xIntfIsConfigurable(logicalPortInfo->physPort, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (logicalPortInfo->guestVlanId == 0)
  {
    /* port not in guest vlan , return success*/
    return L7_SUCCESS;
  }
  vlanId = logicalPortInfo->guestVlanId;

  logicalPortInfo->guestVlanId = 0;

  if(logicalPortInfo->apmState == APM_CONNECTING)
  {
    logicalPortInfo->guestVlanTimer = 0;
  }
  else
  {
    logicalPortInfo->guestVlanTimer = 0;
    dot1xStateMachineClassifier(apmAssignedVlanDelete,lIntIfNum, L7_NULL, L7_NULLPTR);
  }

  /* Generate the vlan delete event to reset the state machines */
  dot1xStateMachineClassifier(bamAssignedVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
  dot1xStateMachineClassifier(rtmInitialize,lIntIfNum, L7_NULL,L7_NULLPTR);

  if (pCfg->reAuthEnabled == L7_TRUE)
  {
    logicalPortInfo->reAuthenticate = L7_TRUE;
  }

  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
      "Port-%u moved from authenticated state as GuestVLAN is not Operational."
      " Guest Vlan deleted.", lIntIfNum);
  if (dot1xPortInfo[logicalPortInfo->physPort].portControlMode == L7_DOT1X_PORT_AUTO)
  {
    dot1xPortVlanMembershipSet(logicalPortInfo->physPort, vlanId, L7_FALSE);
    rc = dot1xCtlPortDot1xRelease(logicalPortInfo->physPort, vlanId);
  }
  else
  {
    /* Mac-based mode */
    dot1xMacBasedVlanParticipationReset(logicalPortInfo, vlanId);
  }

  return rc;
}


/*********************************************************************
* @purpose  Set  dot1x Guest VLAN Period
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    guestVlanPeriod @b{(input)} Guest Vlan Period
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortGuestVlanPeriodSet(L7_uint32 intIfNum,L7_uint32 guestVlanPeriod)
{
  dot1xPortCfg_t *pCfg;

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  dot1xPortInfo[intIfNum].guestVlanPeriod = guestVlanPeriod;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Notifies dot1x has acquired the interface.
*
* @param    intIfNum   @b{(input)} internal interface number (Physical Port)
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortDot1xAcquire(L7_uint32 intIfNum,L7_uint32 vlanId)
{
  NIM_HANDLE_t  handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;

  if(dot1xPortInfo[intIfNum].dot1xAcquire == L7_TRUE)
    return L7_SUCCESS;

  eventInfo.component     = L7_DOT1X_COMPONENT_ID;
  eventInfo.pCbFunc       = L7_NULLPTR;
  eventInfo.event         = L7_DOT1X_ACQUIRE;
  eventInfo.intIfNum      = intIfNum;

  dot1xPortInfo[intIfNum].dot1xAcquire =  L7_TRUE;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,intIfNum,
                    "NIM Notify- Acquire port -%d for vlan - %d \n",intIfNum,vlanId);

  /* notify of the guest vlan has been enabled that dot1x has acquired this port */
  return nimEventIntfNotify(eventInfo,&handle);
}

/*********************************************************************
* @purpose  Notifies  dot1x has released the interface.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortDot1xRelease(L7_uint32 intIfNum,L7_uint32 vlanId)
{
  NIM_HANDLE_t  handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;

  if (dot1xPortInfo[intIfNum].dot1xAcquire == L7_FALSE)
    return L7_SUCCESS;

  dot1xPortInfo[intIfNum].dot1xAcquire =  L7_FALSE;


  eventInfo.component     = L7_DOT1X_COMPONENT_ID;
  eventInfo.pCbFunc       = L7_NULLPTR;
  eventInfo.event         = L7_DOT1X_RELEASE;
  eventInfo.intIfNum      = intIfNum;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,intIfNum,
                    "NIM Notify- Release port -%d for vlan - %d \n",intIfNum,vlanId);

  /* notify of the vlan assignment that dot1x has acquired this port */
  return nimEventIntfNotify(eventInfo,&handle);
}

/*********************************************************************
* @purpose  Processes  Vlan Add Notify event.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortDot1xGuestVlanAdd(L7_uint32 intIfNum,L7_uint32 VlanId)
{
  L7_uint32       intf = 0;
  L7_RC_t nimRc;
  dot1xPortCfg_t *pCfg;

  nimRc = dot1xFirstValidIntfNumber(&intf);
  while (nimRc == L7_SUCCESS)
  {
    if (dot1xIntfIsConfigurable(intf, &pCfg) == L7_TRUE)
    {

        if((dot1qStaticVlanCheckValid(pCfg->guestVlanId) == L7_SUCCESS)&&
            (pCfg->guestVlanId == VlanId))
        {
          (void)dot1xCtlPortGuestVlanTimerStart(intf,VlanId,DOT1X_PHYSICAL_PORT);
        }


    }
    nimRc = dot1xNextValidIntf(intf, &intf);

  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Processes  Vlan Delete Notify event.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortDot1xGuestVlanDelete(L7_uint32 intIfNum,L7_uint32 VlanId)
{
  L7_uint32 intf = 0,lIntIfNum;
  L7_RC_t nimRc;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  nimRc = dot1xFirstValidIntfNumber(&intf);
  while (nimRc == L7_SUCCESS)
  {
    if (dot1xIntfIsConfigurable(intf, &pCfg) != L7_TRUE)
    {
        nimRc = dot1xNextValidIntf(intf, &intf);
        continue;
    }

    if (pCfg->guestVlanId == VlanId)
    {


      lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
      while ((logicalPortInfo = dot1xLogicalPortInfoGetNextNode(intf, &lIntIfNum)) != L7_NULLPTR)
      {
        if ((logicalPortInfo->inUse == L7_TRUE) &&
            (logicalPortInfo->guestVlanId != 0))
          dot1xCtlPortGuestVlanRemove(logicalPortInfo->logicalPortNumber, VlanId);
      }
    }
    nimRc = dot1xNextValidIntf(intf, &intf);
  }
  return L7_SUCCESS;
}

void dot1xVlanChangeProcess(L7_uint32 event, L7_uint32 intIfNum, dot1qNotifyData_t *vlanData)
{
  L7_uint32      i = 0, vlanId = 0, numVlans = 0;
  L7_RC_t rc;

  for (i = 1; i<=L7_VLAN_MAX_MASK_BIT; i++)
  {
    if (vlanData->numVlans == 1)
    {
        vlanId = vlanData->data.vlanId;
        /* For any continue, we will break out */
        i = L7_VLAN_MAX_MASK_BIT + 1;
    }
    else
    {
        if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,i))
        {
            vlanId = i;
        }
        else
        {
            if (numVlans == vlanData->numVlans)
            {
                /* Already taken care of all the bits in the mask so break out of for loop */
                break;
            }
            else
            {
                /* Vlan is not set check for the next bit since there are more bits that are set*/
                continue;
            }
        }
    }

    switch (event)
    {
      case dot1xVlanDeleteEvent:
        if (L7_VLAN_ISMASKBITSET(dot1xVlanMask,vlanId))
        {
          dot1xVlanAssignmentDisable(intIfNum,vlanId);
        }
        if (L7_VLAN_ISMASKBITSET(dot1xGuestVlanMask,vlanId))
        {
          rc = dot1xCtlPortDot1xGuestVlanDelete(intIfNum,vlanId);
        }
      break;

      case dot1xVlanAddEvent:
        if (L7_VLAN_ISMASKBITSET(dot1xGuestVlanMask,vlanId))
        {
          rc = dot1xCtlPortDot1xGuestVlanAdd(intIfNum,vlanId);
        }
      break;

      case dot1xVlanAddPortEvent:
        rc = dot1xVlanPortAddProcess(intIfNum,vlanId);
      break;

    case dot1xVlanDeletePortEvent:
        rc = dot1xVlanPortDeleteProcess(intIfNum,vlanId);
      break;

      case dot1xVlanPvidChangeEvent:
        rc = dot1xVlanPVIDChangeEventProcess(intIfNum,vlanId);
      break;
    }
    DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,0,
                      "dot1xEvent %d port %d \n", event, intIfNum);
    numVlans++;
  }
  return;
}


/*********************************************************************
* @purpose  Check if pdu is to be processed considering logical port
*           use and availability
*
* @param    intIfNum     @b{(input)} physical interface
* @param    *srcMac      @b{(input)} supplicant mac address
* @param    logicalPort  @b{(input)} logical port
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Command is queued for service
*
* @end
*********************************************************************/
L7_RC_t dot1xCheckMapPdu(L7_uint32 intIfNum, L7_char8 *srcMac, L7_uint32 *logicalPort,L7_BOOL *existing_node)
{
  L7_uint32 lIndex;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uchar8 tmpMac[L7_ENET_MAC_ADDR_LEN];

  DOT1X_EVENT_TRACE(DOT1X_TRACE_PDU,intIfNum ,"\n%s:%d: source Mac: %02X:%02X:%02X:%02X:%02X:%02X  Interface:%d \n",
                      __FUNCTION__,__LINE__,
                      srcMac[0],
                      srcMac[1],
                      srcMac[2],
                      srcMac[3],
                      srcMac[4],
                      srcMac[5],
                      intIfNum);

  /* Get the port mode */
  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if ((pCfg->portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED) ||
      /* check for scenario where mac based port has been accquired by lag or
       * mirroring session and treat as a force-authorized port */
      ((pCfg->portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED) &&
       (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_FORCE_AUTHORIZED)))
  {
    logicalPortInfo = dot1xPhysicalPortInfoNodeGet(intIfNum);
    if(logicalPortInfo)
    {
      *logicalPort = logicalPortInfo->logicalPortNumber;
      memset(tmpMac,0,L7_ENET_MAC_ADDR_LEN);
      if ((memcmp(logicalPortInfo->suppMacAddr.addr,tmpMac,L7_ENET_MAC_ADDR_LEN)!= 0)&&
          (memcmp(logicalPortInfo->suppMacAddr.addr,srcMac,L7_ENET_MAC_ADDR_LEN)!=0))
      {
            *existing_node = L7_FALSE;
      }
      else
      {
          *existing_node = L7_TRUE;
      }


      return L7_SUCCESS;
    }
    return L7_FAILURE;
  }

  /* loop based on the intIfNum */
  lIndex = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIndex))!= L7_NULLPTR)
  {
    if (memcmp(srcMac, logicalPortInfo->suppMacAddr.addr,L7_MAC_ADDR_LEN) == 0)
    {
      *logicalPort = lIndex;
      *existing_node = L7_TRUE;
      return L7_SUCCESS;
    }
  }

  if (dot1xPortInfo[intIfNum].numUsers >= dot1xPortInfo[intIfNum].maxUsers)
  {
    return L7_FAILURE;
  }

  /* allocate a new logical port for this supplicant */
  logicalPortInfo = dot1xLogicalPortInfoAlloc(intIfNum);

  if(logicalPortInfo == L7_NULLPTR)
  {
     return L7_FAILURE;
  }
  else
  {
    dot1xLogicalPortInfoSetPortInfo(logicalPortInfo);

    logicalPortInfo->inUse = L7_TRUE;
    memcpy(logicalPortInfo->suppMacAddr.addr, srcMac, L7_MAC_ADDR_LEN);
    *logicalPort = logicalPortInfo->logicalPortNumber;
    *existing_node = L7_FALSE;
    logicalPortInfo->currentIdL = dot1xPortInfo[intIfNum].currentId;
    if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_FORCE_AUTHORIZED)
    {
      dot1xIhPortStatusSet(logicalPortInfo->logicalPortNumber, L7_DOT1X_PORT_STATUS_AUTHORIZED);
    }
    dot1xPortInfo[intIfNum].numUsers++;
    dot1xCtlApplyLogicalPortConfigData(logicalPortInfo->logicalPortNumber);
    dot1xPortInfo[intIfNum].incCurrentId = L7_TRUE;

    /* add mac address to Mac Addr Database*/
    dot1xMacAddrInfoAdd(&(logicalPortInfo->suppMacAddr),logicalPortInfo->logicalPortNumber);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set max users value
*
* @param    intIfNum @b{(input)) internal interface number
* @param    maxUsers @b{(input)) max users
*
* @returns  L7_SUCCESS
*
* @comments The maxUsers is the maximum number of hosts that can be
*           authenticated on a port using mac based authentication
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortMaxUsersSet(L7_uint32 intIfNum, L7_uint32 maxUsers)
{
  L7_RC_t rc=L7_SUCCESS;

  dot1xPortInfo[intIfNum].maxUsers = maxUsers;

  if(maxUsers < dot1xPortInfo[intIfNum].numUsers)
  {
    /* The current number of users are overrided by the new config */
    dot1xPortReset(intIfNum);
    rc = dot1xCtlApplyPortConfigData(intIfNum);
  }

  dot1xCfg->cfgHdr.dataChanged = L7_TRUE;
  return rc;
}

/*********************************************************************
* @purpose  Apply dot1x logical config data to specified interface
*
* @param    lIntIfNum     @b{(input)) Logical internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlApplyLogicalPortConfigData(L7_uint32 lIntIfNum)
{
  L7_uint32 physPort;

  physPort = dot1xPhysPortGet(lIntIfNum);

  /* Initialize state machines */
  if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO ||
      dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED ||
      dot1xPortInfo[physPort].portEnabled == L7_FALSE)
      (void)dot1xStateMachineClassifier(apmInitialize, lIntIfNum, L7_NULL, L7_NULLPTR);
  else if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_FORCE_AUTHORIZED)
      (void)dot1xStateMachineClassifier(apmPortControlEqualForceAuthorized, lIntIfNum,
                                       L7_NULL, L7_NULLPTR);
  else
      (void)dot1xStateMachineClassifier(apmPortControlEqualForceUnauthorized, lIntIfNum,
                                       L7_NULL, L7_NULLPTR);

  (void)dot1xStateMachineClassifier(ktxInitialize, lIntIfNum, L7_NULL, L7_NULLPTR);
  (void)dot1xStateMachineClassifier(rtmInitialize, lIntIfNum, L7_NULL, L7_NULLPTR);
  (void)dot1xStateMachineClassifier(bamInitialize, lIntIfNum, L7_NULL, L7_NULLPTR);
  (void)dot1xStateMachineClassifier(krxInitialize, lIntIfNum, L7_NULL, L7_NULLPTR);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Reset dot1x session data to specified interface
*
* @param    intIfNum     @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlResetPortSessionData(L7_uint32 intIfNum)
{
  L7_uint32 bytesRcvd;
  L7_uint32 bytesTx;
  L7_uint32 gigbytesRcvd;
  L7_uint32 gigbytesTx;
  L7_uint32 unit = 1;

   /*Get the Total Bytes Received */
  usmDbStatGet64(unit, L7_PLATFORM_CTR_RX_TOTAL_BYTES, intIfNum, &gigbytesRcvd,&bytesRcvd);

  dot1xPortSessionStats[intIfNum].sessionOctetsRx = bytesRcvd;
  dot1xPortSessionStats[intIfNum].sessionOctetsGbRx = gigbytesRcvd;

   /*Get the Total Bytes Transmitted */
  usmDbStatGet64(unit, L7_PLATFORM_CTR_TX_TOTAL_BYTES, intIfNum, &gigbytesTx,&bytesTx);

  dot1xPortSessionStats[intIfNum].sessionOctetsTx = bytesTx;
  dot1xPortSessionStats[intIfNum].sessionOctetsGbTx = gigbytesTx;

  dot1xPortSessionStats[intIfNum].sessionTime = osapiUpTimeRaw();

  dot1xPortSessionStats[intIfNum].sessionId[0] = L7_NULL;
  dot1xPortSessionStats[intIfNum].userName[0] = L7_NULL;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Reset dot1x session data to specified interface
*
* @param    logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlResetLogicalPortSessionData(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  logicalPortInfo->sessionTime = osapiUpTimeRaw();
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Reset dot1x session data to specified interface
*
* @param    intIfNum     @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlStopPortSessionData(L7_uint32 intIfNum)
{
  L7_uint32 tempU32 = osapiUpTimeRaw();
  L7_uint32 bytesRcvd;
  L7_uint32 bytesTx;
  L7_uint32 gigbytesRcvd;
  L7_uint32 gigbytesTx;
  L7_uint32 unit = 1;

   /*Get the Total Bytes Received */
  usmDbStatGet64(unit, L7_PLATFORM_CTR_RX_TOTAL_BYTES, intIfNum, &gigbytesRcvd, &bytesRcvd);

  if (gigbytesRcvd >= dot1xPortSessionStats[intIfNum].sessionOctetsGbRx)
  {
    dot1xPortSessionStats[intIfNum].sessionOctetsGbRx =
        gigbytesRcvd - dot1xPortSessionStats[intIfNum].sessionOctetsGbRx;
  }
  else
  {
    dot1xPortSessionStats[intIfNum].sessionOctetsGbRx =
            0xffffffff - dot1xPortSessionStats[intIfNum].sessionOctetsGbRx +
            gigbytesRcvd;
  }

  if (bytesRcvd >= dot1xPortSessionStats[intIfNum].sessionOctetsRx)
  {
    dot1xPortSessionStats[intIfNum].sessionOctetsRx =
        bytesRcvd - dot1xPortSessionStats[intIfNum].sessionOctetsRx;
  }
  else
  {
    dot1xPortSessionStats[intIfNum].sessionOctetsRx =
            0xffffffff - dot1xPortSessionStats[intIfNum].sessionOctetsRx +
            bytesRcvd;
    if(dot1xPortSessionStats[intIfNum].sessionOctetsGbRx >0)
      dot1xPortSessionStats[intIfNum].sessionOctetsGbRx = dot1xPortSessionStats[intIfNum].sessionOctetsGbRx - 1;

  }

   /*Get the Total Bytes Transmitted */
  usmDbStatGet64(unit, L7_PLATFORM_CTR_TX_TOTAL_BYTES, intIfNum, &gigbytesTx,&bytesTx);

  if (gigbytesTx >= dot1xPortSessionStats[intIfNum].sessionOctetsGbTx)
  {
    dot1xPortSessionStats[intIfNum].sessionOctetsGbTx =
        gigbytesTx - dot1xPortSessionStats[intIfNum].sessionOctetsGbTx;
  }
  else
  {
    dot1xPortSessionStats[intIfNum].sessionOctetsGbTx =
            0xffffffff - dot1xPortSessionStats[intIfNum].sessionOctetsGbTx +
            gigbytesTx;
  }

  if (bytesTx >= dot1xPortSessionStats[intIfNum].sessionOctetsTx)
  {
    dot1xPortSessionStats[intIfNum].sessionOctetsTx =
        bytesTx - dot1xPortSessionStats[intIfNum].sessionOctetsTx;
  }
  else
  {
    dot1xPortSessionStats[intIfNum].sessionOctetsTx =
            0xffffffff - dot1xPortSessionStats[intIfNum].sessionOctetsTx +
            bytesTx;
    if(dot1xPortSessionStats[intIfNum].sessionOctetsGbTx >0)
    {
      dot1xPortSessionStats[intIfNum].sessionOctetsGbTx =
        dot1xPortSessionStats[intIfNum].sessionOctetsGbTx - 1;
    }
  }

  if (tempU32 > dot1xPortSessionStats[intIfNum].sessionTime)
  {
    dot1xPortSessionStats[intIfNum].sessionTime =
           tempU32 - dot1xPortSessionStats[intIfNum].sessionTime;
  }
  else
  {
    dot1xPortSessionStats[intIfNum].sessionTime =
    0xffffffff - dot1xPortSessionStats[intIfNum].sessionTime + tempU32;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset dot1x session data to specified interface
*
* @param    logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlStopLogicalPortSessionData(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32 tempU32 = osapiUpTimeRaw();
  if (tempU32 > logicalPortInfo->sessionTime)
  {
    logicalPortInfo->sessionTime =
      tempU32 - logicalPortInfo->sessionTime;
  }
  else
  {
    logicalPortInfo->sessionTime = 0xffffffff -
                                      logicalPortInfo->sessionTime + tempU32;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set Supplicant timeout value for logical port
*
* @param    logicalPortInfo  @b{(input))  Logical Port Info node
* @param    suppTimeout @b{(input)) Supplicant timeout
*
* @returns  L7_SUCCESS
*
* @comments The suppTimeout is the initialization value for aWhile,
*           which is a timer used by the Authenticator state machine
*           to time out the Supplicant.
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortSuppTimeoutSet(dot1xLogicalPortInfo_t *logicalPortInfo, L7_uint32 suppTimeout)
{
  if (logicalPortInfo->inUse == L7_TRUE &&
      logicalPortInfo->bamState == BAM_REQUEST)
    logicalPortInfo->aWhile = suppTimeout;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set Dot1q parameters for a physical port
*
* @param    intIfNum    @b{(input)) internal interface number
* @param    vlanId      @b{(input)) vlan Id
* @param    flag        @b{(input)) TRUE/FALSE to set/reset
*
* @returns  L7_SUCCESS/L7_FAILRE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xPortVlanMembershipSet(L7_uint32 intIfNum, L7_uint32 vlanId,L7_BOOL flag)
{
  L7_RC_t rc = L7_FAILURE;
  DOT1Q_SWPORT_MODE_t curr_mode = DOT1Q_SWPORT_MODE_GENERAL;

  dot1qSwitchPortModeGet(intIfNum,&curr_mode);

  if(flag == L7_FALSE)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,intIfNum,
                      "%s:Resetting Vlan Membership for port -%d and Vlan %d\n",
                      __FUNCTION__,intIfNum,vlanId);
    if(curr_mode == DOT1Q_SWPORT_MODE_GENERAL)
    {
      rc = dot1qVlanMemberSet(vlanId,intIfNum,L7_DOT1Q_FORBIDDEN,DOT1Q_DOT1X,DOT1Q_SWPORT_MODE_NONE);
      dot1qQportsVIDSet(intIfNum, 1, L7_DOT1Q_DEFAULT_VID_INDEX,DOT1Q_DOT1X);
    }
    else if(curr_mode == DOT1Q_SWPORT_MODE_ACCESS)
    {
      rc = dot1qSwitchPortAccessVlanSet(intIfNum,L7_DOT1Q_DEFAULT_VLAN,DOT1Q_DOT1X);
    }
  }
  else
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,intIfNum,
                      "%s:Setting Vlan Membership for port -%d and Vlan %d\n",
                      __FUNCTION__,intIfNum,vlanId);
    if(curr_mode == DOT1Q_SWPORT_MODE_GENERAL)
    {
      rc = dot1qVlanMemberSet(vlanId,intIfNum,L7_DOT1Q_FIXED,DOT1Q_DOT1X,DOT1Q_SWPORT_MODE_NONE);
      dot1qQportsVIDSet(intIfNum, vlanId, L7_DOT1Q_DEFAULT_VID_INDEX,DOT1Q_DOT1X);
    }
    else if(curr_mode == DOT1Q_SWPORT_MODE_ACCESS)
    {
      rc = dot1qSwitchPortAccessVlanSet(intIfNum,vlanId,DOT1Q_DOT1X);
    }

  }
  
  return rc;
}

/*********************************************************************
* @purpose  Set vlan participation for the logical Interface
*
* @param    lIntIfNum    @b{(input)) logical interface number
* @param    vlanId      @b{(input)) vlan Id
*
* @returns  L7_SUCCESS/L7_FAILRE
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t dot1xMacBasedVlanParticipationSet(L7_uint32 lIntIfNum, L7_uint32 vlanId)
{
  L7_uint32 mode;
  L7_uint32 phyPort = dot1xPhysPortGet(lIntIfNum);

  DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,phyPort,
                    "\nSetting Vlan Membership for port -%d and Vlan %d\n",phyPort,vlanId);

  /* check the dot1q vlan participation */
  if (dot1qOperVlanMemberGet(vlanId,phyPort,&mode) == L7_SUCCESS && mode != L7_DOT1Q_FIXED)
  {
    return dot1qVlanMemberSet(vlanId,phyPort,L7_DOT1Q_FIXED,DOT1Q_DOT1X,DOT1Q_SWPORT_MODE_NONE);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset vlan participation for the logical Interface
*
* @param    logicalPortInfo  @b{(input))  Logical Port Info node
* @param    vlanId      @b{(input)) vlan Id
*
* @returns  L7_SUCCESS/L7_FAILRE
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t dot1xMacBasedVlanParticipationReset(dot1xLogicalPortInfo_t *logicalPortInfo, L7_uint32 vlanId)
{
  L7_uint32 lIndex=0;
  L7_BOOL   delParticiaption = L7_TRUE;
  L7_uint32 phyPort = logicalPortInfo->physPort;
  dot1xLogicalPortInfo_t *localPortInfo;

  lIndex = DOT1X_LOGICAL_PORT_ITERATE;
  while((localPortInfo=dot1xLogicalPortInfoGetNextNode(phyPort,&lIndex))!= L7_NULLPTR)
  {
    if(localPortInfo->inUse == L7_TRUE && lIndex != logicalPortInfo->logicalPortNumber &&
       (vlanId ==  localPortInfo->vlanAssigned || vlanId == localPortInfo->unauthVlan))
    {
      delParticiaption = L7_FALSE;
      break;
    }
  }

  if(delParticiaption == L7_TRUE)
  {
    dot1qVlanMemberRevert(vlanId,phyPort);
    DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,phyPort,
                      "Reseting Vlan Membership for port -%d and Vlan %d\n",phyPort,vlanId);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Processes  Vlan PVID Change Notify event.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    vlanId     @b{(input)} VlanId (original PVID on the port)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xVlanPVIDChangeEventProcess(L7_uint32 intIfNum,L7_uint32 vlanId)
{
    L7_RC_t rc = L7_FAILURE;
    dot1xLogicalPortInfo_t *logicalPortInfo;
    L7_uint32 lIntIfNum;
    L7_uint32 curr_pvid;

    if (dot1xCfg->dot1xAdminMode != L7_ENABLE)
      return L7_SUCCESS;

    if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
    {
        lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
        while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
        {
            /* if client is authorized */
            if ((logicalPortInfo->inUse == L7_TRUE) && (logicalPortInfo->logicalPortStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED))
            {
                if ((logicalPortInfo->vlanId == vlanId) || (logicalPortInfo->vlanId == L7_MAX_VLAN_ID))
                {
                    curr_pvid =0;
                    (void) dot1qQportsCurrentVIDGet(intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &curr_pvid);
                    if (curr_pvid != vlanId)
                    {

                        if (logicalPortInfo->apmState == APM_AUTHENTICATED)
                        {
                           if (dot1xIhPortStatusSet(logicalPortInfo->logicalPortNumber, L7_DOT1X_PORT_STATUS_UNAUTHORIZED) != L7_SUCCESS)
                                LOG_MSG("dot1xApmAuthenticatedAction: could not set state to unauthorized for applying changes during pvid change, intf %u\n",
                                         logicalPortInfo->logicalPortNumber);

                           if(dot1qStaticVlanCheckValid(curr_pvid) == L7_SUCCESS)
                                logicalPortInfo->vlanId = curr_pvid;
                            else
                                logicalPortInfo->vlanId = L7_MAX_VLAN_ID;


                            if (dot1xIhPortStatusSet(logicalPortInfo->logicalPortNumber, L7_DOT1X_PORT_STATUS_AUTHORIZED) != L7_SUCCESS)
                                L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
                                    "dot1xApmAuthenticatedAction: could not set state to authorized for applying changes during pvid change, intf %u vlan:%u."
                                    " Could not reset the port to authorized state after applying PVID  change to the client details",
                                        logicalPortInfo->logicalPortNumber,curr_pvid);

                        }

                    }/* curr_pvid != vlanId*/
                }/* client in given vlan*/
            }
        }
    }
    else if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO)
    {
        L7_uint32 curr_pvid =0;
        logicalPortInfo = dot1xPhysicalPortInfoNodeGet(intIfNum);
        if (logicalPortInfo != L7_NULLPTR)
        {
           if (logicalPortInfo->vlanId == vlanId)
           {
               (void) dot1qQportsVIDGet(intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &curr_pvid);
               logicalPortInfo->vlanId = curr_pvid;
               return L7_SUCCESS;
           }
        }


    }
    else
        rc = L7_SUCCESS;
    return rc;
}
/*********************************************************************
* @purpose  Apply the assigned policy
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    vlanId        @b{(input)} VlanId
* @param    macAddr       @b{(input)} Mac address
* @param    policyString  @b{(input)} policy String
* @param    flag          @b{(input)} set or reset the policy
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApplyAssignedPolicy(L7_uint32 intIfNum,L7_uint32 vlanId,
                                 L7_uchar8 *macAddr, L7_uchar8 *policyString,
                                 L7_BOOL  flag)
{
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
    L7_uint32     policyIndex;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE)
  {
      if(diffServPolicyNameToIndex(policyString,&policyIndex) == L7_SUCCESS)
      {
          return L7_SUCCESS;
      }
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "%s:Error to convert policy to index for %s."
              " Error converting RADIUS assigned filter ID to "
              "configured Diffserv policy ID.",__FUNCTION__,policyString);
      return L7_FAILURE;
  }
#else
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
      "%s:Error to convert policy to index for %s.Diffserv not present."
      " Error getting the policy ID for RADIUS assigned Filter ID as Diffserv"
      " is not present .",__FUNCTION__,policyString);
#endif
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Set  dot1x unauthenticated vlan to a specified interface
*
* @param    logicalPortInfo  @b{(input))  Logical Port Info node
* @param    reason      @b{(input)) Reason for the assignment
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortUnauthenticatedVlanOperational(dot1xLogicalPortInfo_t *logicalPortInfo,L7_uint32 reason)
{
  dot1xPortCfg_t   *pCfg;
  L7_uint32        phyIntf = logicalPortInfo->physPort;
  L7_uint32        vlanId;
  L7_uint32        lIntIfNum = logicalPortInfo->logicalPortNumber;
  DOT1Q_SWPORT_MODE_t curr_mode;

  if(dot1xIntfIsConfigurable(phyIntf, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  vlanId = pCfg->unauthenticatedVlan;

   /* check if the unauthenticated vlan is being set as a result of a re-authentication*/
   if ((logicalPortInfo->unauthVlan !=0) &&
       (logicalPortInfo->unauthVlan != vlanId))
    {
         DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,phyIntf,
                        "\n%s:In Unauthenticated Vlan Set. Resetting unauthenticated vlan %u  for port %u -  \n",
                        __FUNCTION__,logicalPortInfo->unauthVlan,phyIntf);
        logicalPortInfo->reauth_auth_apply = L7_TRUE;

        dot1xCtlLogicalPortUnauthenticatedVlanReset(logicalPortInfo->logicalPortNumber);

    }

  if((dot1qStaticVlanCheckValid(vlanId) == L7_SUCCESS) &&
     (dot1qSwitchPortModeGet(phyIntf,&curr_mode)== L7_SUCCESS)&&
     (curr_mode == DOT1Q_SWPORT_MODE_GENERAL))
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,phyIntf,
                      "\n%s Setting unauthenticated vlanid - %d for int- %d  \n",
                      __FUNCTION__,vlanId,lIntIfNum);

    /* set the vlan */

    logicalPortInfo->unauthVlan = vlanId;
    logicalPortInfo->unauthReason = reason;
    /* reset other vlan Ids*/
    if (logicalPortInfo->vlanAssigned !=0)
    {

        dot1xCtlLogicalPortVlanAssignedReset(lIntIfNum);
        /*port moving from Radius assigned vlan to unauthenticated vlan*/
        if (dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
        {
            logicalPortInfo->reauth_auth_apply = L7_TRUE;
        }
    }

    if (logicalPortInfo->vlanId !=0)
    {
        /* port moving from default vlan to unauthenticated vlan*/
        if ((dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)&&
             (logicalPortInfo->vlanId != logicalPortInfo->unauthVlan))
        {
            logicalPortInfo->reauth_auth_apply = L7_TRUE;
        }
    }
     /* zero out the server state vlaue store during the RADIUS Challenge process */
    if (logicalPortInfo->serverStateLen != 0)
    {
       memset(logicalPortInfo->serverState,0, DOT1X_SERVER_STATE_LEN);
       logicalPortInfo->serverStateLen = 0;
    }

    /* Generate the events */
    dot1xStateMachineClassifier(bamAssignUnauthenticatedVlan,
                                lIntIfNum, L7_NULL, L7_NULLPTR);
    dot1xStateMachineClassifier(apmAssignUnauthenticatedVlan,
                                lIntIfNum,L7_NULL,L7_NULLPTR);

    if (dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
    {
      /* Need to set this port from vlan participation */
      dot1xMacBasedVlanParticipationSet(lIntIfNum,vlanId);

    }
    else if (dot1xPortInfo[phyIntf].portControlMode == L7_DOT1X_PORT_AUTO &&
             dot1xPortInfo[phyIntf].dot1xAcquire != L7_TRUE)
    {
      dot1xPortVlanMembershipSet(phyIntf,vlanId,L7_TRUE);
      dot1xCtlPortDot1xAcquire(phyIntf,vlanId);

       /* reset session statictics for the port */
      dot1xCtlResetPortSessionData(phyIntf);
    }

    /* reset session statictics for the logicalport*/
     dot1xCtlResetLogicalPortSessionData(logicalPortInfo);


    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Enable  dot1x unauthenticated vlan to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    vlanId @b{(input)} unauthenticated vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortUnauthenticatedVlanSet(L7_uint32 intIfNum,L7_uint32 vlanId)
{
  L7_uint32   lIntIfNum=DOT1X_LOGICAL_PORT_ITERATE;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_RC_t rc = L7_FAILURE;

  if (dot1xPortInfo[intIfNum].portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
    while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
    {

       if (logicalPortInfo->unauthVlan !=0)
       {
             if (logicalPortInfo->apmState == APM_AUTHENTICATED)
              {
                 /*logicalPortInfo->unauthVlan = 0; */

                  rc = dot1xLogicalPortReauthenticateSet(lIntIfNum,L7_TRUE);

              }
       }

    }/*while*/
  }/* authorized*/

  return rc;
}

/*********************************************************************
* @purpose  Disable  dot1x unauthenticated vlan to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortUnauthenticatedVlanReset(L7_uint32 lIntIfNum)
{
  L7_uint32   curr_vlanId;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 physPort;


    logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo != L7_NULLPTR)
  {
    physPort = logicalPortInfo->physPort;
    if (logicalPortInfo->inUse == L7_TRUE)
    {
      if (logicalPortInfo->unauthVlan !=0)
      {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL, physPort,
                      "%s Resetting unauthenticated vlanid - %d for int- %d  \n",
                          __FUNCTION__, logicalPortInfo->unauthVlan, lIntIfNum);

        curr_vlanId = logicalPortInfo->unauthVlan;
        logicalPortInfo->unauthVlan = 0;
        logicalPortInfo->unauthReason = 0;

        if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
        {
          /* Need to delete this port from vlan participation */
          dot1xMacBasedVlanParticipationReset(logicalPortInfo, curr_vlanId);
        }
        else if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO &&
                 dot1xPortInfo[physPort].dot1xAcquire == L7_TRUE)
        {
          dot1xPortVlanMembershipSet(physPort, curr_vlanId, L7_FALSE);
          dot1xCtlPortDot1xRelease(physPort, curr_vlanId);
        }

      }/* unauthvlan !=0 */
    }/* inuse*/
  }


  return rc;
}


/*********************************************************************
* @purpose  Disable  radius assigned vlan on a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortVlanAssignedReset(L7_uint32 lIntIfNum)
{
  L7_uint32   curr_vlanId;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 physPort;


    logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
    if(logicalPortInfo != L7_NULLPTR)
    {
      physPort = logicalPortInfo->physPort;
      if (logicalPortInfo->inUse == L7_TRUE)
      {
         if (logicalPortInfo->vlanAssigned !=0)
         {
            DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,physPort,
                      "\n%s Resetting RADIUS assigned vlanid - %d for int- %d  \n",
                      __FUNCTION__,logicalPortInfo->vlanAssigned,lIntIfNum);

             curr_vlanId = logicalPortInfo->vlanAssigned;
             logicalPortInfo->vlanAssigned = 0;


             if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
             {
                 /* Need to delete this port from vlan participation */
                 dot1xMacBasedVlanParticipationReset(logicalPortInfo,curr_vlanId);
             }
             else if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO &&
                      dot1xPortInfo[physPort].dot1xAcquire == L7_TRUE)
             {
                 dot1xPortVlanMembershipSet(physPort,curr_vlanId,L7_FALSE);
                 dot1xCtlPortDot1xRelease(physPort,curr_vlanId);
             }


           }/* vlanAssigned !=0 */
         }/* inuse*/
      }


  return rc;
}

/*********************************************************************
* @purpose  Process the unauthenticated Users on the port
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    vlanId @b{(input)} unauthenticated vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortUnauthAddrCallbackProcess(L7_uint32 intIfNum,L7_enetMacAddr_t macAddr,L7_ushort16 vlanId)
{
  L7_uint32 lIntIfNum;
  L7_BOOL   exists;
  dot1xPortCfg_t *pCfg;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,intIfNum,
                    "Received dot1xCtlPortUnauthAddrCallbackProcess for MAC: %02X:%02X:%02X:%02X:%02X:%02X Vlan:%d \n",
                    macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],
                    macAddr.addr[5],vlanId);

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_SUCCESS;

  /* search the Mac address in the list of clients on the port */
  if (dot1xCheckMapPdu(intIfNum, macAddr.addr, &lIntIfNum, &exists)!=L7_SUCCESS)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE, intIfNum,
                      "Failed to Process the unauth Addr Callback \n");
    return L7_SUCCESS;
  }

  if (exists == L7_FALSE)
  {
    dot1xLogicalPortInfo_t *logicalPortInfo;
    logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
    if (logicalPortInfo == L7_NULL)
    {
        /*Coverity 88470 fix: Should not come here as logical port would have been created and assigned in dot1xCheckMapPdu function*/
       DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE, intIfNum,
                      "Failed to create logiucal port %d \n",lIntIfNum);
       return L7_SUCCESS;
    }
    logicalPortInfo->vlanId = vlanId;


    /* Start the MAB or the Guest vlan timer for this logical interface */
    if (dot1xPortInfo[intIfNum].mabEnabled == L7_ENABLE)
    {
      dot1xCtlLogicalPortMABTimerStart(lIntIfNum);
    }
    else if(pCfg->guestVlanId != 0)
    {
      dot1xCtlPortGuestVlanTimerStart(lIntIfNum, pCfg->guestVlanId, DOT1X_LOGICAL_PORT);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable/Disable Voip device connection
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    macAddr    @b{(input)} Mac Address of the Voip Device
* @param    vlanId     @b{(input)) vlan id of the voip device
* @param    flag       @b{(input)} TRUE - Enable, FALSE - DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortVoiceVlanOperational(L7_uint32 intIfNum,
                                         L7_enetMacAddr_t macAddr,
                                         L7_ushort16 vlanId,
                                         L7_BOOL   flag)
{
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 lIntIfNum;
  L7_BOOL   exists;
  L7_uint32 physPort;
  L7_RC_t rc = L7_FAILURE;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,intIfNum,
                    "Received %s for MAC: %02X:%02X:%02X:%02X:%02X:%02X \n",__FUNCTION__,
                    macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],
                    macAddr.addr[5]);

  if (flag == L7_TRUE)
  {
    /* Check if the mac address exists in the databse */
    if (dot1xCheckMapPdu(intIfNum,macAddr.addr,&lIntIfNum,&exists) != L7_SUCCESS)
    {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                        "%s: Failed as the maximum number of clients reached on the port - %d \n",
                        __FUNCTION__,intIfNum);
      return L7_FAILURE;
    }
    logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
    if (logicalPortInfo == L7_NULL)
    {
      /* Coverity defect 88471: should not get here as a logical port would have been assigned in dot1xCheckMapPdu */

      DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                        "%s: Failed to find the logical port - %d \n",
                        __FUNCTION__,lIntIfNum);
      return L7_FAILURE;
    }
    physPort = logicalPortInfo->physPort;

    /* Need to enable the voip device */
    if (logicalPortInfo->apmState != APM_AUTHENTICATED)
    {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,intIfNum,
                        "%s: Enabling the Voice vlan device on Interface \n",
                        __FUNCTION__,intIfNum);

      logicalPortInfo->voiceVlanId = vlanId;

      rc = dot1xMacBasedVlanParticipationSet(lIntIfNum, vlanId);

      if (rc == L7_SUCCESS)
      {
        /* In case Guest vlan timer is running then stop it */
        logicalPortInfo->guestVlanTimer = 0;

        /* Use the guest vlan timer as we need to move the state from connecting to authenticated state */
        dot1xStateMachineClassifier(bamGuestVlanTimerExpired, lIntIfNum, L7_NULL, L7_NULLPTR);
        dot1xStateMachineClassifier(apmGuestVlanTimerExpired, lIntIfNum, L7_NULL, L7_NULLPTR);
      }
    }
  }
  else
  {
    /* Disable the voip device */
    lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
    while ((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIntIfNum))!= L7_NULLPTR)
    {
      if (memcmp(macAddr.addr, logicalPortInfo->suppMacAddr.addr,L7_MAC_ADDR_LEN) == 0)
      {
        /* Delete this entry */
        if (logicalPortInfo->voiceVlanId != 0)
        {
          physPort = logicalPortInfo->physPort;
          DOT1X_EVENT_TRACE(DOT1X_TRACE_EVENTS,intIfNum,
                            "%s: Disabling the Voice vlan device on Interface \n",
                            __FUNCTION__,intIfNum);

          dot1xMacBasedVlanParticipationReset(logicalPortInfo,logicalPortInfo->voiceVlanId);

          logicalPortInfo->voiceVlanId = 0;

          /* Generate the vlan delete event to reset the state machines */
          dot1xStateMachineClassifier(apmAssignVoiceVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
        }
        break;
      }
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Apply dot1x vlan assignment for a specific logicalPort
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    vlanId     @b{(input)) vlan id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortVlanAssignmentDisable(dot1xLogicalPortInfo_t *logicalPortInfo)
{

  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (logicalPortInfo->vlanAssigned != 0)
  {
    dot1xCtlLogicalPortVlanAssignedReset(logicalPortInfo->logicalPortNumber);
  }
  else if (logicalPortInfo->unauthVlan != 0)
  {
    dot1xCtlLogicalPortUnauthenticatedVlanReset(logicalPortInfo->logicalPortNumber);
  }
  else if (logicalPortInfo->guestVlanId != 0)
  {
      if (dot1xPortInfo[logicalPortInfo->physPort].portControlMode == L7_DOT1X_PORT_AUTO )
      {
           dot1xPortVlanMembershipSet(logicalPortInfo->physPort,logicalPortInfo->guestVlanId,L7_FALSE);
           dot1xCtlPortDot1xRelease(logicalPortInfo->physPort,logicalPortInfo->guestVlanId);
      }
      else
      {
          dot1xMacBasedVlanParticipationReset(logicalPortInfo, logicalPortInfo->guestVlanId);
      }
      logicalPortInfo->guestVlanId = 0;
  }
  else if (logicalPortInfo->voiceVlanId != 0)
  {
    dot1xMacBasedVlanParticipationReset(logicalPortInfo, logicalPortInfo->voiceVlanId);
    voiceVlanPortClientAuthSet(logicalPortInfo->physPort, logicalPortInfo->suppMacAddr.addr, L7_FALSE);
    logicalPortInfo->voiceVlanId =0;
  }
  else if (logicalPortInfo->vlanId != 0)
  {
    logicalPortInfo->vlanId = 0;
  }

  return L7_SUCCESS;
}

/* Supplicant APIs */

/*********************************************************************
* @purpose  Apply dot1x Supplicant config data to specified interface
*
* @param    intIfNum     @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlApplySupplicantPortConfigData(L7_uint32 intIfNum)
{
  dot1xPortCfg_t *pCfg;

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_SUCCESS;

  /* Initialize state machines */
   if ( (pCfg->supplicantPortCfg.portControlMode == L7_DOT1X_PORT_AUTO) ||
        (dot1xSupplicantPortInfo[intIfNum].portEnabled == L7_FALSE)
      )
   {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                    "SPM Initialize evernt for port - %d with portControlMode - %d and portEnable state -%d\r\n",
                     intIfNum,pCfg->supplicantPortCfg.portControlMode,dot1xPortInfo[intIfNum].portEnabled);

        (void)dot1xStateMachineClassifier(spmInitialize, intIfNum, L7_NULL, L7_NULLPTR);
   }
   else if (pCfg->supplicantPortCfg.portControlMode == L7_DOT1X_PORT_FORCE_AUTHORIZED)
   {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                   "SPM force authorized evernt for port - %d \r\n", intIfNum);

        (void)dot1xStateMachineClassifier(spmPortControlEqualForceAuthorized, intIfNum,
                                         L7_NULL, L7_NULLPTR);
   }
   else
   {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_FSM_EVENTS,intIfNum,
                   "SPM force unauthorized evernt for port - %d \r\n", intIfNum);
        (void)dot1xStateMachineClassifier(spmPortControlEqualForceUnauthorized, intIfNum,
                                         L7_NULL, L7_NULLPTR);
   }
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Receive dot1x PDUs from Authenticator and pass them to the
*           classifier
*
* @param    intIfNum   @b{(input)} interal interface number that this PDU
*                      was received on
* @param    bufHandle  @b{(input)} buffer handle to the PDU
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Frame has already been adjusted for VLAN header and
*           endianness
*
* @end
*********************************************************************/
L7_RC_t dot1xSupplicantStateMachineRxPdu(L7_uint32 intIfNum,
                                         L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt;
  L7_uint32 pduDataOffset;
  L7_BOOL freeBuffer = L7_TRUE;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 challangeLen;
  L7_uchar8 *challange;

  /* Make sure port is up and enabled */
  if (dot1xSupplicantPortInfo[intIfNum].portEnabled != L7_TRUE)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  dot1xPortStats[intIfNum].suppEapolFramesRx++;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  pduDataOffset = sysNetDataOffsetGet(data);
  eapolPkt = (L7_eapolPacket_t *)(data + pduDataOffset);
  eapPkt = (L7_eapPacket_t *)(data + pduDataOffset + sizeof(L7_eapolPacket_t));

  dot1xPortStats[intIfNum].suppLastEapolFrameVersion = (L7_uint32)eapolPkt->protocolVersion;

  enetHdr = (L7_enetHeader_t *)(data);

  memcpy(dot1xPortStats[intIfNum].suppLastEapolFrameSource.addr, enetHdr->src.addr, L7_ENET_MAC_ADDR_LEN);

  dot1xDebugPacketRxTrace(intIfNum,bufHandle);

  switch (eapolPkt->packetType)
  {
  case EAPOL_EAPPKT:
    switch (eapPkt->code) /* Validate the EAP messages */
    {
    case EAP_REQUEST:
      eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));
      switch (eapRrPkt->type)
      {
      case EAP_RRIDENTITY:
         dot1xPortStats[intIfNum].suppEapReqIdFramesRx++;
        dot1xSupplicantPortInfo[intIfNum].currentId = eapPkt->id;
        dot1xSupplicantPortInfo[intIfNum].requestType = EAP_RRIDENTITY;
        dot1xSupplicantPortInfo[intIfNum].eapSuccess = L7_FALSE;
        dot1xSupplicantPortInfo[intIfNum].eapFail = L7_FALSE;
        dot1xSupplicantPortInfo[intIfNum].eapolEap = L7_TRUE;

        if ( (dot1xSupplicantPortInfo[intIfNum].spmState == DOT1X_SPM_CONNECTING) ||
             (dot1xSupplicantPortInfo[intIfNum].spmState == DOT1X_SPM_HELD)
           )
        {

           (void)dot1xStateMachineClassifier(spmConnectingEapolEap, intIfNum,
                                     bufHandle, L7_NULLPTR);
        }
        else if (dot1xSupplicantPortInfo[intIfNum].spmState == DOT1X_SPM_AUTHENTICATED)
        {
           (void)dot1xStateMachineClassifier(spmAuthenticatedEapolEap, intIfNum,
                                     bufHandle, L7_NULLPTR);

        }
        else
        {
           LOG_MSG ("Supplicant: EAP_REQUEST is received on port --%d--%s--%d with state %d\r\n",
                      intIfNum, __FUNCTION__, __LINE__,dot1xSupplicantPortInfo[intIfNum].spmState);
           rc = L7_FAILURE;
        }
      break;
      case EAP_RRMD5:
        dot1xPortStats[intIfNum].suppEapReqFramesRx++;
        challange = (L7_uchar8 *) ( (L7_uchar8 *)eapRrPkt +sizeof(L7_eapRrPacket_t));
        dot1xSupplicantPortInfo[intIfNum].currentId = eapPkt->id;
        dot1xSupplicantPortInfo[intIfNum].requestType = EAP_RRMD5;
        challangeLen = *challange;;
        dot1xSupplicantPortInfo[intIfNum].challangeLen = challangeLen;
        memcpy (dot1xSupplicantPortInfo[intIfNum].dot1xChallange,
                challange+1,challangeLen);

        (void)dot1xStateMachineClassifier(sbmEapolEap, intIfNum,
                                      bufHandle, L7_NULLPTR);
        rc = L7_SUCCESS;
      break;
      default :
        /* send out the EAP_RRNAK packet */
        dot1xSupplicantPortInfo[intIfNum].currentId = eapPkt->id;
        dot1xSupplicantPortInfo[intIfNum].requestType = EAP_RRNAK;
        (void)dot1xStateMachineClassifier(sbmEapolEap, intIfNum,
                                      bufHandle, L7_NULLPTR);
        dot1xPortStats[intIfNum].suppInvalidEapolFramesRx++;
       rc = L7_FAILURE;
      break;
      }
    break;
    case EAP_RESPONSE:
      dot1xPortStats[intIfNum].suppInvalidEapolFramesRx++;
      LOG_MSG ("Supplicant: EAP_RESPONSE is received on port --%d--%s--%d\r\n",
                      intIfNum, __FUNCTION__, __LINE__);
      rc = L7_FAILURE;
    break;
    case EAP_SUCCESS:
      if (dot1xSupplicantPortInfo[intIfNum].suppStart == L7_TRUE)
      {

        (void)dot1xStateMachineClassifier(sbmEapSuccess, intIfNum,
                                      bufHandle, L7_NULLPTR);
      }
      else /* SPM state should be in CONNECTING state */
      {
        dot1xSupplicantPortInfo[intIfNum].eapSuccess = L7_TRUE;
        dot1xSupplicantPortInfo[intIfNum].eapFail = L7_FALSE;
        dot1xSupplicantPortInfo[intIfNum].eapolEap = L7_FALSE;
        (void)dot1xStateMachineClassifier(spmEapSuccess, intIfNum,
                                     bufHandle, L7_NULLPTR);
      }
      rc = L7_SUCCESS;
    break;
    case EAP_FAILURE:
      if (dot1xSupplicantPortInfo[intIfNum].suppStart == L7_TRUE)
      {

        (void)dot1xStateMachineClassifier(sbmEapFail, intIfNum,
                                      bufHandle, L7_NULLPTR);
      }
      else
      {
        dot1xSupplicantPortInfo[intIfNum].eapFail = L7_TRUE;
        dot1xSupplicantPortInfo[intIfNum].eapSuccess = L7_FALSE;
        dot1xSupplicantPortInfo[intIfNum].eapolEap = L7_FALSE;
        (void)dot1xStateMachineClassifier(spmEapFail, intIfNum,
                                     bufHandle, L7_NULLPTR);
      }
      rc = L7_SUCCESS;
    break;
    default:
      dot1xPortStats[intIfNum].suppInvalidEapolFramesRx++;
     rc = L7_FAILURE;
    break;
   }

  default:
#if 0
    dot1xPortStats[intIfNum].authInvalidEapolFramesRx++;
#endif
    rc = L7_FAILURE;
  break;
  }

  if (freeBuffer == L7_TRUE)
    SYSAPI_NET_MBUF_FREE(bufHandle);

  return rc;
}

/*********************************************************************
* @purpose  Start the  dot1x guest vlan timer on a specified logical
*          interface in order to enable MAB
*
* @param    lIntIfNum   @b{(input)) logical interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortMABTimerStart(L7_uint32 lIntIfNum)
{

  dot1xPortCfg_t *pCfg;
  L7_uint32 intIfNum;
  dot1xLogicalPortInfo_t *logicalPortInfo;


   intIfNum = dot1xPhysPortGet(lIntIfNum);

  if ((logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum))== L7_NULLPTR)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d Failed to start the MAB Timer for  %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }



  DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,
                    "%s:%d Called for %d  \n",
                    __FUNCTION__,__LINE__,lIntIfNum);

  if ((dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE) ||
      (dot1xPortInfo[intIfNum].portEnabled != L7_TRUE)      ||
      (dot1xPortInfo[intIfNum].portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED))
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d Failed to start the MAB Timer for %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }

  if ((dot1xPortInfo[intIfNum].mabEnabled == L7_ENABLE) && (logicalPortInfo->isMABClient == L7_FALSE))
  {
      /* start the timer */
      logicalPortInfo->guestVlanTimer = dot1xPortInfo[intIfNum].guestVlanPeriod;
      DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,intIfNum,
                        "%s:Started the MAB Timer with value = %d for linterface = %d \n",
                        __FUNCTION__,dot1xPortInfo[intIfNum].guestVlanPeriod,lIntIfNum);
  }
  else
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,intIfNum,
                        "%s:Could not start the MAB Timer with value = %d for interface = %d , MabEnabled:%d IsMABClient:%d\n",
                        __FUNCTION__,dot1xPortInfo[intIfNum].guestVlanPeriod,lIntIfNum,
                        dot1xPortInfo[intIfNum].mabEnabled,logicalPortInfo->isMABClient);
  }



  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Enable MAB operationally on a physical port
*
* @param    intIfNum   @b{(input)) internal interface number
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortMABEnableSet(L7_uint32 intIfNum)
{
  dot1xPortCfg_t *pCfg;


  if (dot1xCfg->dot1xAdminMode != L7_ENABLE)
  {

    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d Dot1x not enabled  %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
      return L7_FAILURE;
  }

  if (dot1xIsValidIntf(intIfNum) != L7_TRUE)
  {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d Dot1x Interface[%d] not a valid interface.\n",
                      __FUNCTION__,__LINE__,intIfNum);
        return L7_FAILURE;
  }

  if ((dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)||
       (pCfg->mabEnabled != L7_ENABLE))
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d MAB not enabled for interface %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }

  if ((dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)&&(pCfg->mabEnabled == L7_ENABLE))
  {
    dot1xPortInfo[intIfNum].mabEnabled = pCfg->mabEnabled;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Disable MAB on a physical port
*
* @param    intIfNum   @b{(input)) internal interface number
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlPortMABDisableSet(L7_uint32 intIfNum)
{
  /*dot1xPortCfg_t *pCfg;*/
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32  lIntIfNum;
  L7_RC_t  rc= L7_FAILURE;

  if (dot1xCfg->dot1xAdminMode != L7_ENABLE)
  {

    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d Dot1x not enabled  %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
      return L7_FAILURE;
  }

  if (dot1xIsValidIntf(intIfNum) != L7_TRUE)
  {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d Dot1x Interface[%d] not a valid interface.\n",
                      __FUNCTION__,__LINE__,intIfNum);
        return L7_FAILURE;
  }

 /* if ((dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE) ||
      (pCfg->mabEnabled  != L7_ENABLE))
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d MAB not enabled for %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }*/

  if (dot1xPortInfo[intIfNum].mabEnabled != L7_ENABLE)
  {
     DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,intIfNum,
                      "%s:%d MAB not enabled for %d \n",
                      __FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }

  lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
  while ((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum, &lIntIfNum)) != L7_NULLPTR)
  {
    if (logicalPortInfo->isMABClient == L7_TRUE)
    {
      rc = dot1xCtlLogicalPortMABRemove(lIntIfNum);
    }
  }

  dot1xPortInfo[intIfNum].mabEnabled = L7_DISABLE;

  return rc;
}

/*********************************************************************
* @purpose  Remove a logical interface that has been authenticated
*           through MAB
*
* @param    lIntIfNum   @b{(input)) logical interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortMABRemove(L7_uint32 lIntIfNum)
{
  dot1xLogicalPortInfo_t *logicalPortInfo;
  dot1xPortCfg_t    *pCfg = L7_NULLPTR;


  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (dot1xIntfIsConfigurable(logicalPortInfo->physPort, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (logicalPortInfo->isMABClient != L7_TRUE)
  {
    /* port is not MAB client , return success*/
    return L7_SUCCESS;
  }

  logicalPortInfo->isMABClient = L7_FALSE;

  /*remove fromm any assigned vlans i.e. either radius assigned or guest vlan*/
  dot1xCtlLogicalPortVlanAssignmentDisable(logicalPortInfo);

  if(logicalPortInfo->apmState == APM_CONNECTING)
  {
    logicalPortInfo->guestVlanTimer = 0;
  }
  else
  {
    logicalPortInfo->guestVlanTimer = 0;
    dot1xStateMachineClassifier(apmMABDisable,lIntIfNum, L7_NULL, L7_NULLPTR);
  }

  /* Generate the vlan delete event to reset the state machines */
  dot1xStateMachineClassifier(bamAssignedVlanDelete, lIntIfNum, L7_NULL, L7_NULLPTR);
  dot1xStateMachineClassifier(rtmInitialize,lIntIfNum, L7_NULL,L7_NULLPTR);

  LOG_MSG("Port-%u moved from authenticated state as MAB is not Operational on the port\n",
          lIntIfNum);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Generate Response to Radius Challenge
*
* @param    lIntIfNum   @b{(input)) logical interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortMABGenRequest(L7_uint32 lIntIfNum,L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt;
  L7_uint32 length=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uchar8 *userData;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,0,
                      "%s:%d:  In dot1xCtlLogicalPortMABGeneratePDU  \n",
                      __FUNCTION__,__LINE__,lIntIfNum);

  if((logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum))==L7_NULLPTR)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,0,
                      "%s:%d:  Could not get logical Interface structure for %d \n",
                      __FUNCTION__,__LINE__,lIntIfNum);
    return L7_FAILURE;
  }

  if (bufHandle == L7_NULL)
  {
    return L7_FAILURE;
  }

  DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,
                      "%s:%d:  Generating packet for interface[%d]  \n",
                      __FUNCTION__,__LINE__,lIntIfNum);

   SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr,logicalPortInfo->suppMacAddr.addr, L7_MAC_ADDR_LEN);

   /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;
  /*eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t)) );*/

  /* Set EAP header */
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapPkt->code = EAP_RESPONSE;
  eapPkt->id = logicalPortInfo->currentIdL;

  /* Set EAP Request/Response header */
  eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));

  eapRrPkt->type = EAP_RRIDENTITY;
  userData = ((L7_uchar8 *)eapRrPkt) + sizeof(L7_eapRrPacket_t);

  eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t)+
                                  logicalPortInfo->dot1xUserNameLength) );
  eapPkt->length = eapolPkt->packetBodyLength;
  memset (userData,'\0', logicalPortInfo->dot1xUserNameLength+1);
  memcpy (userData,
          logicalPortInfo->dot1xUserName,
          logicalPortInfo->dot1xUserNameLength);

  length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
          sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) + logicalPortInfo->dot1xUserNameLength );

  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);


   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Generate Response to Radius Challenge
*
* @param    lIntIfNum   @b{(input)) logical interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortMABGenResp(L7_uint32 lIntIfNum)
{
  L7_netBufHandle bufHandle;
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt;
  L7_uint32 length=0;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uchar8  answer[DOT1X_MD5_LEN+1], *responseData;
  L7_uint32  responseDataLen;
  L7_uchar8  password[L7_PASSWORD_SIZE];
  L7_uchar8 *userData;
  L7_uint32 challengelen;
  L7_RC_t rc = L7_FAILURE;


  DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,0,
                      "%s:%d:  In dot1xCtlLogicalPortMABGenResp  \n",
                      __FUNCTION__,__LINE__,lIntIfNum);

  if((logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum))==L7_NULLPTR)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,0,
                      "%s:%d:  Could not get logical Interface structure for %d \n",
                      __FUNCTION__,__LINE__,lIntIfNum);
    return L7_FAILURE;
  }

   /*generate the EAPOL PDU*/
  bufHandle = sysapiNetMbufGet();
  if (bufHandle == L7_NULL)
  {
    LOG_MSG("%s: Out of System buffers\n",__FUNCTION__);
    DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,
                      "%s:%d Out of System buffers\n",
                      __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,
                      "%s:%d:  Generating packet for interface[%d]  \n",
                      __FUNCTION__,__LINE__,lIntIfNum);

   SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr,logicalPortInfo->suppMacAddr.addr, L7_MAC_ADDR_LEN);

   /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;
  eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t)) );

  /* Set EAP header */
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapPkt->code = EAP_RESPONSE;
  eapPkt->id = logicalPortInfo->currentIdL;

  /* Set EAP Request/Response header */
  eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));

  eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) +
                                  sizeof(L7_eapRrPacket_t)+DOT1X_MD5_LEN+1));
  eapPkt->length =  eapolPkt->packetBodyLength ;
  eapRrPkt->type = EAP_RRMD5;
  userData = ((L7_uchar8 *)eapRrPkt) + sizeof(L7_eapRrPacket_t);

   /* generate password */
   memset(password,0,sizeof(password));
   memcpy(password,logicalPortInfo->dot1xUserName,logicalPortInfo->dot1xUserNameLength);

   /*generate challenge*/
   challengelen= logicalPortInfo->dot1xChallengelen;

   responseDataLen = 1 + strlen((L7_char8 *)password) + challengelen;

   responseData = (L7_uchar8 *)osapiMalloc(L7_DOT1X_COMPONENT_ID, responseDataLen);
   if (responseData == L7_NULLPTR)
   {
     LOG_MSG("dot1xCtlLogicalPortMABGenResp: couldn't allocate responseData\n");
   }

   bzero(responseData, responseDataLen);
   responseData[0] = logicalPortInfo->currentIdL;

   memcpy(&responseData[1],password, strlen((L7_char8 *)password));
   memcpy(&responseData[1+strlen((L7_char8 *)password)],logicalPortInfo->dot1xChallenge,challengelen);

  DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,"%s:%d Printing Response Data\n",
                      __FUNCTION__,__LINE__);
   dot1xDebugDataDump(DOT1X_TRACE_MAB,logicalPortInfo->physPort,responseData,responseDataLen);
   dot1xLocalMd5Calc(responseData, responseDataLen, answer);
   osapiFree(L7_DOT1X_COMPONENT_ID, responseData);

   *userData = DOT1X_MD5_LEN;
   userData++;
   memcpy (userData, answer,DOT1X_MD5_LEN);
   length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
           sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) +DOT1X_MD5_LEN+1);

   SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

   DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,logicalPortInfo->physPort,
                      "%s:%d:  Generated PDU :%s \n",
                      __FUNCTION__,__LINE__,data);

   dot1xDebugPacketDump(DOT1X_TRACE_MAB,logicalPortInfo->physPort,logicalPortInfo->physPort,bufHandle);

   logicalPortInfo->rxResp = L7_TRUE;
   rc = dot1xStateMachineClassifier(bamRxResp, lIntIfNum, bufHandle, L7_NULLPTR);
   return rc;
}


/*********************************************************************
* @purpose  Operations to be done when MAB timer expires
*
* @param    lIntIfNum   @b{(input)) logical interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortMABOperational(L7_uint32 lIntIfNum)
{
  L7_uint32 rc = L7_FAILURE;
  dot1xPortCfg_t *pCfg;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physIntf;
  L7_uchar8 username[DOT1X_USER_NAME_LEN];
  L7_uint32 usernamelen=0;
  L7_netBufHandle bufHandle=L7_NULL;


  if((logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum))==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  physIntf =  logicalPortInfo->physPort;

  if (dot1xIntfIsConfigurable(physIntf, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if ((pCfg->mabEnabled != L7_ENABLE)||(dot1xPortInfo[physIntf].portControlMode != L7_DOT1X_PORT_AUTO_MAC_BASED))
  {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_FAILURE,physIntf,
                      "%s:%d:MAB not enabled on the port [%d] or port control mode is not mac based.MAB : %d. Port Control Mode %d  \n",
                      __FUNCTION__,__LINE__,physIntf,pCfg->mabEnabled,dot1xPortInfo[physIntf].portControlMode);
        return L7_FAILURE;
  }

  logicalPortInfo->isMABClient = L7_TRUE;

  /* Construct username form supplicant Mac address and store it*/
  usernamelen=DOT1X_USER_NAME_LEN;
  memset(username,0,DOT1X_USER_NAME_LEN);
  osapiSnprintf(username,usernamelen,"%02X%02X%02X%02X%02X%02X",
                                      logicalPortInfo->suppMacAddr.addr[0],
                                      logicalPortInfo->suppMacAddr.addr[1],
                                      logicalPortInfo->suppMacAddr.addr[2],
                                      logicalPortInfo->suppMacAddr.addr[3],
                                      logicalPortInfo->suppMacAddr.addr[4],
                                      logicalPortInfo->suppMacAddr.addr[5]);

   strcat(username,"\0");
   usernamelen = strlen(username);
   DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,physIntf,
                      "%s:%d:Username :%s Length:%d  \n",
                      __FUNCTION__,__LINE__,username,usernamelen);

   memset(logicalPortInfo->dot1xUserName,0,DOT1X_USER_NAME_LEN);
   memcpy(logicalPortInfo->dot1xUserName, username, usernamelen);
   logicalPortInfo->dot1xUserNameLength = usernamelen;

  /*generate the EAPOL PDU*/
  bufHandle = sysapiNetMbufGet();
  if (bufHandle == L7_NULL)
  {
    LOG_MSG("%s: Out of System buffers\n",__FUNCTION__);
    DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,physIntf,
                      "%s:%d Out of System buffers\n",
                      __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  dot1xCtlLogicalPortMABGenRequest(lIntIfNum,bufHandle);

   /* generate apm event*/
   rc=dot1xStateMachineClassifier(apmMABTimerExpired,
                                lIntIfNum,bufHandle,L7_NULLPTR);
   return rc;
}

/*********************************************************************
* @purpose  Disable  dot1x guest vlan assigned due to MAB authentication
*           failure for a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xCtlLogicalPortMABGuestVlanReset(L7_uint32 lIntIfNum)
{
  L7_uint32   curr_vlanId;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 physPort;


  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo != L7_NULLPTR)
  {
    physPort = logicalPortInfo->physPort;
    if (logicalPortInfo->inUse == L7_TRUE)
    {
      if ((logicalPortInfo->isMABClient == L7_TRUE) && (logicalPortInfo->guestVlanId !=0))
      {
        DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL, physPort,
                      "%s Resetting guest vlanid - %d for int- %d  \n",
                          __FUNCTION__, logicalPortInfo->guestVlanId, lIntIfNum);

        curr_vlanId = logicalPortInfo->guestVlanId;
        logicalPortInfo->guestVlanId  = 0;

        if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)
        {
          /* Need to delete this port from vlan participation */
          dot1xMacBasedVlanParticipationReset(logicalPortInfo, curr_vlanId);
        }
        /* MAB will not be enabled on a port in port-based mode*/

      }/* unauthvlan !=0 */
    }/* inuse*/
  }


  return rc;
}
