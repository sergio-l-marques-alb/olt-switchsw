/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename dot1s_control.c
*
* @purpose   Multiple Spanning tree Control file
*
* @component dot1s
*
* @comments 
*
* @create    10/14/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/

#include "dot1s_include.h"
#include "dot3ad_api.h"
#include "buff_api.h"

extern void *dot1sTaskSyncSema;
/*static osapiTimerDescr_t	*dot1sTimer;*/
static L7_uint32 dot1sTimerHandle = 0;
extern void *dot1s_queue;  /* reference to the dot1s message queue */
extern void *dot1s_timer_queue; /* reference to the dot1s timer message queue */
extern DOT1S_INSTANCE_INFO_t		*dot1sInstance;
extern dot1sCfg_t *dot1sCfg;
static L7_uint32 missedMsgCount = 0;
extern DOT1S_PORT_COMMON_t			*dot1sPort;
extern dot1sDeregister_t dot1sDeregister;
extern dot1sCnfgrState_t dot1sCnfgrState;

static void * dot1sQueueSyncSem = 0;
extern L7_uint32 dot1sBPDUBufferPoolId;

extern  DOT1S_VLAN_MASK_t           dot1sVlanMask;  

L7_uint32 dot1sQueueTime[10];     /* store time spent in dot1s Queue*/
L7_uint32 dot1sBPDUProcessTime[10];  /* store time taken to process last 10 bpdus*/

static L7_uint32 Qcnt =0;   /* counter for Queue time array*/
static L7_uint32 MsgCnt=0;  /* counter for BPDU process time array*/
/*********************************************************************
* @purpose  Initialize dot1s tasks and data
*
* @param    parmType    @b{(input)} Describes 2nd parm (filename or structure)
* @param    buffer      @b{(input)} pointer to structure of filename
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t dot1sStartTasks(L7_PARM_TYPES_t parmType, L7_char8 *buffer)
{
  L7_RC_t rc;
  
  rc = L7_SUCCESS;

  return(rc);
}

/*********************************************************************
* @purpose  dot1s task which serves the request queue
*
* @param    
*
* @returns  void
*
* @comments User-interface writes and LACPDUs are serviced off
*           of the dot1s_queue
*       
* @end
*********************************************************************/
void dot1s_task()
{
  DOT1S_MSG_t msg;
  L7_RC_t rc;
  L7_uint32 status;

  rc = osapiTaskInitDone(L7_DOT1S_TASK_SYNC);

  /* Wait for both NIM and DOT1Q to initialize before applying our config.
   * Note: This should not be necessary but no other mechanism exists to
   * notify DOT1S of their readiness. Sleep of 5 seconds was not sufficient
   * time to initialize, 7 is.
   */
  /*osapiSleep(7);*/

  /* apply the stored config only if we do not support lag interfaces
   * if lag are supported dot1sLagIntfInit will apply config when
   * all the lag interfaces are created
   */
  /*
  if (L7_MAX_NUM_LAG_INTF == 0)
  {
	rc = dot1sApplyConfigData();
  }
  */
  
  for (;;)
  {
    status = (L7_uint32)osapiMessageReceive(dot1s_queue,
                                            (void *)&msg,
                                            (L7_uint32)sizeof(DOT1S_MSG_t),
                                            L7_WAIT_FOREVER);

    rc = osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);
    rc = dot1sDispatchCmd(msg);
	rc = osapiSemaGive(dot1sTaskSyncSema);
  }
}

/*********************************************************************
* @purpose  dot1s timer task which serves the expired timers
*
* @param    
*
* @returns  void
*
* @comments    
*           
*       
* @end
*********************************************************************/
void dot1s_timer_task()
{
  L7_uchar8 msg;
  L7_uint32 status;
  L7_RC_t	rc;
  
  rc = osapiTaskInitDone(L7_DOT1S_TIMER_TASK_SYNC);

  rc = osapiPeriodicUserTimerRegister(DOT1S_TIMER_TICK, &dot1sTimerHandle);
  
  for (;;)
  {
	osapiPeriodicUserTimerWait(dot1sTimerHandle);
	/*put a msg in the timer queue*/
	rc = osapiMessageSend(dot1s_timer_queue, &msg, (L7_uint32)sizeof(L7_uchar8), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    status = (L7_uint32)osapiMessageReceive(dot1s_timer_queue,
                                            (void *)&msg,
                                            (L7_uint32)sizeof(msg),
                                            L7_WAIT_FOREVER);
	
	rc = osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);
    rc = dot1sTimerAction();
	rc = osapiSemaGive(dot1sTaskSyncSema);
  }
}

/*********************************************************************
* @purpose  Save the data in a message to a shared memory
*
* @param    *data       @b{(input)} pointer to data
* @param    *msg        @b{(input)} pointer to dot1s message
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments Once the message is serviced, this variable size data will
*           be retrieved
*       
* @end
*********************************************************************/
L7_RC_t dot1sFillMsg(void *data, DOT1S_MSG_t *msg)
{
  switch (msg->event)
  {
  /* events originating from users, CLI, WEB, etc.. */
  case switchModeEnable:
  case switchModeDisable:
  case switchForceVersionSet:
  case switchConfigFormatSelectorSet:
  case switchConfigRevisionSet:
  case commonPrioritySet:
  case commonBridgeMaxAgeSet:
  case commonBridgeMaxHopSet:
  case commonBridgeTxHoldCountSet:
  case commonBridgeHelloTimeSet:
  case commonBridgeFwdDelaySet:
  case commonPortPathCostSet:
  case commonPortPrioritySet:
  case commonPortEdgePortEnable:
  case commonPortEdgePortDisable:
  case instancePortPrioritySet:
  case instanceVlanAdd:
  case instanceVlanRemove:
  case instancePortPathCostSet:
  case instancePrioritySet:
  case dot1sIntfStateSet:
  case commonPortExtPathCostSet:
  case commonPortAdminHelloTimeSet:
    /* add to queue L7_uint32 size */
	if (data != L7_NULL)
	{
		memcpy(&msg->data.p, data, sizeof(L7_uint32));
	}
    break;

  case switchConfigNameSet:
    /* add to queue agg name */
	if (data != L7_NULL)
	{
		memcpy(&msg->data.name, data, L7_DOT1S_MAX_CONFIG_NAME);
	}
    break;
  case commonPortRestrictedRoleSet:
  case commonPortLoopGuardSet:  
  case commonPortRestrictedTcnSet:
  case commonPortAutoEdgeSet:
	  if (data != L7_NULL)
	  {
		  msg->data.mode =  *(L7_BOOL *)data ;
	  }
      break;

  case dot1sBpduReceived:

	msg->data.bpdu = data;
    break;

  case switchBpduGuardModeSet:
  case switchBpduFilterModeSet:
  case commonPortBpduFilterModeSet:
  case commonPortBpduFloodModeSet:
    if(data != L7_NULL)
    {
      memcpy(&msg->data.mode,data,sizeof(L7_BOOL));
    }
    break;

  /* add to queue a mstid */
  case switchPortAdminModeDisable:
  case switchPortAdminModeEnable:
  case instanceCreate:
  case instanceDelete:
  case switchPortStateSet:
  case switchPortMigrationCheck:
  case switchMacAddrChange:
    break; /* NULL data, proceed */

  /*events originating from the sate machine and meant for the state machines*/
  case pimRcvdMsg:                                      /*E2*/
  case pimRcvdXstMsgNotUpdtXstInfo:                     /*E8*/
  case pimSupDsgInfo:                                   /*E4*/
  case pimRepDsgInfo:                                   /*E5*/
  case pimInferiorDesignatedInfo:                       /*E5A*/
  case pimRootAlternateInfo:                            /*E6*/
  case pimOtherInfo:                                    /*E7*/
    /*copy the cist or msti msg*/
	if (data != L7_NULL)
	{

        if (msg->instance == DOT1S_CIST_INDEX)
        {
          memcpy(&msg->data.cistMsg, data, sizeof(DOT1S_CIST_MSG_t));
        }
        else
        {
          memcpy(&msg->data.mstiMsg, data, sizeof(DOT1S_MSTI_MSG_t));
        }
	}
    break;
  case pimBegin:                                  		/*E1.pim*/
  case pimUpdtInfo:                                     /*E3*/
  case pimSelectedUpdtInfo:                             /*E9*/
  case pimRcvdRcvdInfoWhileZeroNotUpdtInfoNotRcvdXstInfo:  /*E10*/
  case pimPortEnabled:                         			/*E11*/
  case pimPortDisabledInfoIsNotEqualDisabled:          /*E12*/
  case prsBegin:                                  		/*E1.prs*/
  case prsReselect:                                    /*E13*/
  case prtBegin:                                  		/*E1.prt*/
  case prtDisabledPortRoleNotEqualSelectedRole:        /*E14*/
  case prtAlternatePortRoleNotEqualSelectedRole:       /*E15*/
  case prtDisabledNotLearningNotForwarding:
  case prtDisabledSync:
  case prtDisabledReroot:
  case prtDisabledNotSynced:
  case prtDisabledFdWhileNotMaxAge:
  case prtMasterPortRoleNotEqualSelectedRole:          /*E25*/
  case prtMasterProposedNotAgree:                            /*E26*/
  case prtMasterProposedAgree:                            /*E26*/
  case prtMasterAllSyncedNotAgree:                           /*E29*/
  case prtMasterNotLrnNotFwdNotSynced:       /*E30*/
  case prtMasterAgreedNotSynced:                             /*E31*/
  case prtMasterEdgeNotSynced:                               /*E32*/
  case prtMasterSyncSynced:                                  /*E33*/
  case prtMasterReRootRrWhileZero:                           /*E37*/
  case prtMasterListen:/*need more explicit events*/         /*E38*/
  case prtMasterLearn: /*need more explicit events*/         /*E39*/
  case prtMasterForward:/*need more explicit events*/        /*E40*/
  case prtDesigPortRoleNotEqualSelectedRole:           /*E24*/
  case prtDesigNotFwdNotAgNotPropNotEdge:      /*E27*/
  case prtDesigAllSynced:                           /*E29*/
  case prtDesigNotLrnNotFwdNotSynced:       /*E30*/
  case prtDesigAgreedNotSynced:                             /*E31*/
  case prtDesigEdgeNotSynced:                               /*E32*/
  case prtDesigSyncSynced:                                  /*E33*/
  case prtDesigReRootRrWhileZero:                           /*E37*/
  case prtDesigListen:/*need more explicit events*/         /*E38*/
  case prtDesigLearn: /*need more explicit events*/         /*E39*/
  case prtDesigForward:/*need more explicit events*/        /*E40*/
  case prtRootPortRoleNotEqualSelectedRole:            /*E23*/
  case prtRootProposedNotAgree:                            /*E26*/
  case prtRootProposedAgree:                               /*E26*/
  case prtRootAllSyncedNotAgree:                           /*E29*/
  case prtRootAgreedNotSynced:                             /*E31*/
  case prtRootSyncSynced:                                  /*E33*/
  case prtRootNotFwdNotReRoot:                             /*E34*/
  case prtRootRrWhileNotEqualFwd:                          /*E35*/
  case prtRootReRootFwd:                                   /*E36*/
  case prtRootLearn: /*need more explicit events*/         /*E39*/
  case prtRootForward:/*need more explicit events*/        /*E40*/
  case prtBackupPortRoleNotEqualSelectedRole:          /*E16*/
  case prtAltbkProposedNotAgree:                            /*E26*/
  case prtAltbkAllSyncedNotAgree:                           /*E29*/
  case prtAltbkProposedAgree:                            /*E26*/
  case prtAltbkNotLrngNotFwdg:                            /*E26*/
  case prtAltbkRoleBkupRbWhile:                            /*E26*/
  case prtAltbkSync:
  case prtAltbkReroot:
  case prtAltbkNotSynced:
  case prtAltbkFdWhileNotFwdDelay:
  case prxBegin:                                  		/*E1.prx*/
  case prxRcvdBpduPortDisabled:                        /*E41*/
  case prxRcvdBpduPortEnabledNotRcvdAnyMsg:            /*E42*/
  case pstBegin:                                  		/*E1.pst*/
  case pstLearn:                                       /*E43*/
  case pstNotLearn:                                    /*E44*/
  case pstForward:                                     /*E45*/
  case pstNotForward:                                  /*E46*/
  case tcmBegin:                                  	   /*E1.tcm*/
  case tcmRcvdTc:                                      /*E47*/
  case tcmRcvdTcn:                                     /*E48*/
  case tcmRcvdTcAck:                                   /*E49*/
  case tcmTcProp:                                      /*E50*/
  case tcmRoleRootFwdNotEdge:                          /*E51*/
  case tcmRoleDesigFwdNotEdge:                         /*E52*/
  case tcmRoleMasterFwdNotEdge:                        /*E53*/
  /*case *//*tcmEdge:*//*this event is removed as per D14*//*E54*/
  case tcmRoleNotRootNotDesigNotMaster:                /*E55*/ 
  case ppmBegin:                                  		/*E1.ppm*/
  case ppmNotPortEnabledmDelayWhileNotMigrateTime:
  case ppmMDelayWhileZero:
  case ppmPortDisabled:
  case ppmMCheck:
  case ppmSendRSTPRcvdStp:
  case ppmRstpVersionNotSendRstpRcvdSTP:
  case ptxBegin:                                  	   /*E1.ptx*/
  case ptxHelloWhenZero:                               /*E65*/
  case ptxSendRstp:                                    /*E66*/
  case ptxSendTcn:                                     /*E67*/
  case ptxSendConfig:                                  /*E68*/
  case bdmBeginAdminEdge:                                  	   /*E1.bdm*/
  case bdmBeginNotAdminEdge:                                  	   /*E1.bdm*/
  case bdmNotPortEnabledNotAdminEdge:                  /*E69*/	/*link up/dn*/
  case bdmNotOperEdge:
  case bdmNotPortEnabledAdminEdge:                  /*E69*/	/*link up/dn*/
  case bdmEdgeDelayWhileZero:
  case instanceQueueSynchronize:
    break; /* NULL data, proceed */

  case dot1sIntfChange:
	if (data != L7_NULL)
	{
		memcpy(&msg->data.status, data, (L7_uint32)sizeof(NIM_EVENT_COMPLETE_INFO_t));
	}
	break;

  default:
    /* unmatched event */
    return L7_FAILURE;

  }/*end switch (msg->event)*/

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Route the event to a handling function and grab the parms
*
* @param    msg  @b{(input)} dot1s message
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t dot1sDispatchCmd(DOT1S_MSG_t msg)
{
  L7_RC_t rc = L7_SUCCESS;
  DOT1S_PORT_COMMON_t *p;
  L7_uint32 instIndex;
  L7_uint32 currTime=0;
  L7_uint32 startTime=0;
  L7_uint32 endTime=0;

  switch (msg.event)
  {
  /* 4 byte priorities/keys/ports */
  case switchModeEnable:
    rc = dot1sSwitchModeEnableSet(msg.data.p);
    break;

  case switchModeDisable:
    rc = dot1sSwitchModeDisableSet(msg.data.p);
    break;

  case switchForceVersionSet:
    rc = dot1sSwitchForceVerSet(msg.data.p);
    break;

  case switchConfigFormatSelectorSet:
    rc = dot1sSwitchCfgForSelSet(msg.data.p);
    break;

  case switchConfigRevisionSet:
    rc = dot1sSwitchRevSet(msg.data.p);
    break;

  case switchPortMigrationCheck:
    rc = dot1sSwitchPortMigrationCheck(msg.intf);
    break;

  case switchMacAddrChange:
    rc = dot1sSwitchMacAddrChange();
    break;

  case switchPortAdminModeEnable:
    rc = dot1sSwitchPortAdminModeEnableSet(msg.intf);
    break;

  case switchPortAdminModeDisable:
    rc = dot1sSwitchPortAdminModeDisableSet(msg.intf);
    break;

  case switchPortStateSet:
    rc = dot1sSwitchPortStateSet(msg.intf);
    break;
  
  case commonPrioritySet:
    rc = dot1sCommonPriSet(msg.instance, msg.data.p);
    break;

  case commonBridgeMaxAgeSet:
    rc = dot1sCommonBridgeMaxAgeSet(msg.data.p);
    break;

  case commonBridgeMaxHopSet:
    rc = dot1sCommonBridgeMaxHopSet(msg.data.p);
    break;
  
  case commonBridgeTxHoldCountSet:
	rc = dot1sCommonBridgeMaxHopSet(msg.data.p);
	break;

  case commonBridgeHelloTimeSet:
    rc = dot1sCommonBridgeHelloTimeSet(msg.data.p);
    break;

  case commonBridgeFwdDelaySet:
    rc = dot1sCommonBridgeFwdDelaySet(msg.data.p);
    break;

  case commonPortPathCostSet:
    rc = dot1sCommonPortPathCostSet(msg.intf, msg.instance, msg.data.p);
    break;
  
  case commonPortPrioritySet:
    rc = dot1sCommonPortPriSet(msg.intf, msg.instance, msg.data.p);
    break;
   
  case commonPortEdgePortEnable:
    rc = dot1sCommonPortEdgePortEnable(msg.intf, msg.instance, msg.data.p);
    break;
    
  case commonPortEdgePortDisable:
    rc = dot1sCommonEdgePortDisable(msg.intf, msg.instance, msg.data.p);
    break;
 
  case commonPortRestrictedRoleSet:
	rc = dot1sCommonPortRestrictedRoleSet(msg.intf, msg.instance, msg.data.mode);
	break;

  case commonPortLoopGuardSet:
  rc = dot1sCommonPortLoopGuardSet(msg.intf, msg.instance, msg.data.mode);
  break;

  case commonPortRestrictedTcnSet:
	rc = dot1sCommonPortRestrictedTcnSet(msg.intf, msg.instance, msg.data.mode);
	break;
 
  case commonPortAutoEdgeSet:
	rc = dot1sCommonPortAutoEdgeSet(msg.intf, msg.instance, msg.data.mode);
	break;
 
  case instanceCreate:
    rc = dot1sUserInstanceCreate(msg.instance);
    break;

  case instanceDelete:
    rc = dot1sInstanceDelete(msg.instance);
    break;

  case instancePortPrioritySet:
    rc = dot1sInstancePortPriSet(msg.intf, msg.instance, msg.data.p);
    break;

  case switchConfigNameSet:
    rc = dot1sSwitchCfgNameSet(msg.data.name);
    break;

  case instanceVlanAdd:
    rc = dot1sUserInstanceVlanAdd(msg.instance, msg.data.p);
    break;

  case instanceVlanRemove:
    rc = dot1sInstanceVlanRemove(msg.instance, msg.data.p);
    break;
  
  case instancePortPathCostSet:
    rc = dot1sInstancePortPathCostSet(msg.intf, msg.instance, msg.data.p);
    break;
	
  case instancePrioritySet:
    rc = dot1sInstancePriSet(msg.instance, msg.data.p);
    break;
	
  case dot1sBpduReceived:
    
    if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_MSG_PROC_TIME))
    {
      /* process time diff in queue first */
      currTime=osapiTimeMillisecondsGet();
      if (Qcnt == 10)
      {
        Qcnt=0;
      }
      if (currTime < msg.timeStamp )
      {
        printf("\n Error! current time cannot be less than time when msg entered the queue.Curr Time:%u Timestamp:%u",
               currTime,msg.timeStamp);
      }
      else
      {
        //dot1sQueueTime[Qcnt] = currTime - msg.timeStamp;
        dot1sQueueTime[Qcnt] = osapiTimeMillisecondsDiff(currTime,msg.timeStamp);
        Qcnt++;
      }

      /* process msg processing time*/
      /*get timestamp here*/
      startTime = osapiTimeMillisecondsGet();
    }
    rc = dot1sStateMachineRxBpdu(msg.intf, (DOT1S_MSTP_ENCAPS_t 	*)msg.data.bpdu);

    if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_MSG_PROC_TIME))
    {
      /* get another here store the diff */
      endTime = osapiTimeMillisecondsGet();
      if (MsgCnt == 10 )
      {
        MsgCnt =0;
      }
      //dot1sBPDUProcessTime[MsgCnt] = endTime-startTime;    
      dot1sBPDUProcessTime[MsgCnt] = osapiTimeMillisecondsDiff(endTime,startTime);  
      MsgCnt++;
    }
	break;

  case dot1sIntfChange:
	rc = dot1sIhProcessIntfChange(msg.intf, msg.data.status);
	break;

  case dot1sIntfStateSet:
    dot1sIhPortStateAllSet(msg.intf, msg.data.p);
    break;

  case commonPortAdminHelloTimeSet:
    dot1sPortAdminHelloTimeSet(msg.instance, msg.intf, msg.data.p);
    break;

  case commonPortExtPathCostSet:
    dot1sPortExtPathCostSet(msg.instance, msg.intf, msg.data.p);
    break;

  case instanceQueueSynchronize:
    dot1sQueueSyncSemFree();
    break;

  case pimRcvdMsg:                                      /*E2*/
  case pimSupDsgInfo:                                   /*E4*/
  case pimRepDsgInfo:                                   /*E5*/
  case pimInferiorDesignatedInfo:                       /*E5A*/
  case pimRootAlternateInfo:                            /*E6*/
  case pimOtherInfo:                                    /*E7*/
  case pimRcvdXstMsgNotUpdtXstInfo:                     /*E8*/

	p = dot1sIntfFind(msg.intf);
    if (p != L7_NULLPTR && p->portNum != 0)
    {

      rc = dot1sInstIndexFind(msg.instance, &instIndex);
	  if (rc == L7_FAILURE)
	  {
		break;
	  }

      if (msg.instance == DOT1S_CIST_INDEX)
      {
        rc = dot1sStateMachineClassifier(msg.event, p, instIndex, L7_NULL, (void *)&msg.data.cistMsg);
      }
      else
      {
        rc = dot1sStateMachineClassifier(msg.event, p, instIndex, L7_NULL, (void *)&msg.data.mstiMsg);
      }
    }
    break;
    
  case pimBegin:                                  		/*E1.pim*/
  case pimUpdtInfo:                                     /*E3*/
  case pimSelectedUpdtInfo:                             /*E9*/
  case pimRcvdRcvdInfoWhileZeroNotUpdtInfoNotRcvdXstInfo:  /*E10*/
  case pimPortEnabled:                         /*E11*/
  case pimPortDisabledInfoIsNotEqualDisabled:          /*E12*/
  case prsBegin:                                  		/*E1.prs*/
  case prsReselect:                                    /*E13*/

  case prtBegin:                                  		/*E1.prt*/

  case prtAlternatePortRoleNotEqualSelectedRole:       /*E15*/
  case prtDisabledPortRoleNotEqualSelectedRole:
  case prtDisabledNotLearningNotForwarding:
  case prtDisabledSync:
  case prtDisabledReroot:
  case prtDisabledNotSynced:
  case prtDisabledFdWhileNotMaxAge:


  case prtMasterPortRoleNotEqualSelectedRole:          /*E25*/
  case prtMasterProposedNotAgree:                            /*E26*/
  case prtMasterProposedAgree:                            /*E26*/
  case prtMasterAllSyncedNotAgree:                           /*E29*/
  case prtMasterNotLrnNotFwdNotSynced:       /*E30*/
  case prtMasterAgreedNotSynced:                             /*E31*/
  case prtMasterEdgeNotSynced:                               /*E32*/
  case prtMasterSyncSynced:                                  /*E33*/
  case prtMasterReRootRrWhileZero:                           /*E37*/
  case prtMasterListen:/*need more explicit events*/         /*E38*/
  case prtMasterLearn: /*need more explicit events*/         /*E39*/
  case prtMasterForward:/*need more explicit events*/        /*E40*/


  case prtDesigPortRoleNotEqualSelectedRole:           /*E24*/
  case prtDesigNotFwdNotAgNotPropNotEdge:      /*E27*/
  case prtDesigAllSynced:                           /*E29*/
  case prtDesigNotLrnNotFwdNotSynced:       /*E30*/
  case prtDesigAgreedNotSynced:                             /*E31*/
  case prtDesigEdgeNotSynced:                               /*E32*/
  case prtDesigSyncSynced:                                  /*E33*/
  case prtDesigReRootRrWhileZero:                           /*E37*/
  case prtDesigListen:/*need more explicit events*/         /*E38*/
  case prtDesigLearn: /*need more explicit events*/         /*E39*/
  case prtDesigForward:/*need more explicit events*/        /*E40*/


  case prtRootPortRoleNotEqualSelectedRole:            /*E23*/
  case prtRootProposedNotAgree:                            /*E26*/
  case prtRootProposedAgree:                               /*E26*/
  case prtRootAllSyncedNotAgree:                           /*E29*/
  case prtRootAgreedNotSynced:                             /*E31*/
  case prtRootSyncSynced:                                  /*E33*/
  case prtRootNotFwdNotReRoot:                             /*E34*/
  case prtRootRrWhileNotEqualFwd:                          /*E35*/
  case prtRootReRootFwd:                                   /*E36*/
  case prtRootLearn: /*need more explicit events*/         /*E39*/
  case prtRootForward:/*need more explicit events*/        /*E40*/


  case prtBackupPortRoleNotEqualSelectedRole:          /*E16*/
  case prtAltbkProposedNotAgree:                            /*E26*/
  case prtAltbkAllSyncedNotAgree:                           /*E29*/
  case prtAltbkProposedAgree:                            /*E26*/
  case prtAltbkNotLrngNotFwdg:                            /*E26*/
  case prtAltbkRoleBkupRbWhile:                            /*E26*/
  case prtAltbkSync:
  case prtAltbkReroot:
  case prtAltbkNotSynced:
  case prtAltbkFdWhileNotFwdDelay:




  case prxBegin:                                  		/*E1.prx*/
  case prxRcvdBpduPortDisabled:                        /*E41*/
  case prxRcvdBpduPortEnabledNotRcvdAnyMsg:            /*E42*/
  case pstBegin:                                  		/*E1.pst*/
  case pstLearn:                                       /*E43*/
  case pstNotLearn:                                    /*E44*/
  case pstForward:                                     /*E45*/
  case pstNotForward:                                  /*E46*/
  case tcmBegin:                                  		/*E1.tcm*/
  case tcmRcvdTc:                                      /*E47*/
  case tcmRcvdTcn:                                     /*E48*/
  case tcmRcvdTcAck:                                   /*E49*/
  case tcmTcProp:                                      /*E50*/
  case tcmRoleRootFwdNotEdge:                          /*E51*/
  case tcmRoleDesigFwdNotEdge:                         /*E52*/
  case tcmRoleMasterFwdNotEdge:                        /*E53*/
  /*case *//*tcmEdge:*//*this event is removed as per D14*//*E54*/
  case tcmRoleNotRootNotDesigNotMaster:                /*E55*/ 
  case ppmBegin:                                  		/*E1.ppm*/
  case ppmNotPortEnabledmDelayWhileNotMigrateTime:
  case ppmMDelayWhileZero:
  case ppmPortDisabled:
  case ppmMCheck:
  case ppmSendRSTPRcvdStp:
  case ppmRstpVersionNotSendRstpRcvdSTP:
  case ptxBegin:                                  		/*E1.ptx*/
  case ptxHelloWhenZero:                               /*E65*/
  case ptxSendRstp:                                    /*E66*/
  case ptxSendTcn:                                     /*E67*/
  case ptxSendConfig:                                  /*E68*/
  case bdmBeginAdminEdge:                                  	   /*E1.bdm*/
  case bdmBeginNotAdminEdge:                                  	   /*E1.bdm*/
  case bdmNotPortEnabledNotAdminEdge:                  /*E69*/	/*link up/dn*/
  case bdmNotOperEdge:
  case bdmNotPortEnabledAdminEdge:                  /*E69*/	/*link up/dn*/
  case bdmEdgeDelayWhileZero:
    p = dot1sIntfFind(msg.intf);
    if (p == L7_NULLPTR /*|| p->portNum == 0*/)/*allow port zero for prs begin as it is per instance*/
    {
      return L7_FAILURE;
    }
	rc = dot1sInstIndexFind(msg.instance, &instIndex);
    if (rc == L7_SUCCESS)
	{
      rc = dot1sStateMachineClassifier(msg.event,p,instIndex,L7_NULL,L7_NULLPTR);
	}
	break;


  case dot1sCnfgr:
	dot1sCnfgrParse(&msg.data.CmdData);
	break;

  case switchBpduGuardModeSet:
    dot1sSwitchBpduGuardModeSet(msg.data.mode);
    break;

  case switchBpduFilterModeSet:
    dot1sSwitchBpduFilterModeSet(msg.data.mode);
    break;

  case commonPortBpduFilterModeSet:
    dot1sCommonPortBpduFilterModeSet(msg.intf, msg.instance, msg.data.mode);
    break;

  case commonPortBpduFloodModeSet:
    dot1sCommonPortBpduFloodModeSet(msg.intf, msg.instance, msg.data.mode);
    break;

  default:
    rc = L7_FAILURE;

  }/*end switch (msg.event)*/
 return rc;

}
/*********************************************************************
* @purpose  Send a command to LAC
*
* @param    event       @b{(input)} Event type
* @param    intIfNum    @b{(input)} interface number
* @param    instanceId  @b{(input)} instance ID
* @param    *data       @b{(input)} pointer to data
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments Command will be queued for service
*       
* @end
*********************************************************************/
L7_RC_t dot1sIssueCmd(L7_uint32 event, 
					  L7_uint32 intIfNum, 
					  L7_uint32 instanceId, 
					  void* data)
{
  L7_RC_t rc;

  /* find storage for new msg, possibly add to another queue */
  DOT1S_MSG_t msg;
  
  /* copy event, intIfNum, instanceId and data ptr to msg struct */
  msg.event = event;
  msg.intf = intIfNum;
  msg.instance = instanceId;

  rc = dot1sFillMsg(data,&msg);

  /* send msg */     
  rc = osapiMessageSend(dot1s_queue, &msg, (L7_uint32)sizeof(DOT1S_MSG_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    if (event != dot1sBpduReceived)
    {
      /* Missing any event other than a BPDU will put a system in an unknown state,
      ** so make sure that it doesn't happen.
      */
      LOG_ERROR (event);
    }

    missedMsgCount++;

    if (missedMsgCount == 50)
    {
      missedMsgCount = 0;
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
          "Dot1s Msg Queue is full!!!!Event: %u, on interface: %u, for instance: %u."
          " The message Queue is full.", event, intIfNum, instanceId);
    }
  }

  return rc;
}
/*********************************************************************
* @purpose  This routine decrements all the timer counters for all ports in all instances
*
* @param    
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments Also the events generated are directly sent to state machiune classifier.
*       
* @end
*********************************************************************/
L7_RC_t dot1sTimerAction()
{
  /*this routine is called every second. decrement all the timer counters for all 
   *ports in all instances of spanning tree
   */
  DOT1S_PORT_COMMON_t *p;
  L7_uint32 instIndex, j;
  L7_RC_t rc = L7_SUCCESS;
  static L7_uint32 wholeSecond = DOT1S_TICKS_PER_SECOND;
  L7_BOOL timersExpired;

  /* Ignore timer events until dot1s is initialized.
  */
  if (!(DOT1S_IS_READY))
  {
    wholeSecond = DOT1S_TICKS_PER_SECOND;
    return L7_SUCCESS;
  }

  /*timer values are already multiplied by 256 so accomodate for it*/
  /*if MSTP is disabled do not process the timer tick event*/
  if(dot1sBridge->Mode == L7_DISABLE)
  {
	wholeSecond = DOT1S_TICKS_PER_SECOND;
    return L7_SUCCESS;
  }
  
  for (j = 1; j <= (L7_uint32)L7_MAX_INTERFACE_COUNT; j++)
  {
	timersExpired = L7_FALSE;
  	p = dot1sIntfFind(j);
	if (p != L7_NULLPTR )
	{
	  if (p->portUpEnabledNum != 0 )
	  {
	    /*decrement per port timers*/
		if (p->helloWhen/DOT1S_TIMER_UNIT_PER_TICK != 0)
		{
		  p->helloWhen -= DOT1S_TIMER_UNIT_PER_TICK;
		  if (p->helloWhen == 0)
		  {
				  timersExpired = L7_TRUE;
		  }
		}
		
		if (p->mdelayWhile/DOT1S_TIMER_UNIT_PER_TICK != 0)
		{
		  p->mdelayWhile -= DOT1S_TIMER_UNIT_PER_TICK;
		  if (p->mdelayWhile == 0)
		  {
				  timersExpired = L7_TRUE;
		  }
		}
        
		if (wholeSecond == 0 && p->txCount != 0)
		  p->txCount--;
      
		for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
		{
		  if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
		  {
		    /*decrement per port per instance timers*/

			if (p->portInstInfo[instIndex].tcWhile/DOT1S_TIMER_UNIT_PER_TICK == 1)
              rc = dot1sTcWhileZero(p, instIndex);

            if (p->portInstInfo[instIndex].tcWhile/DOT1S_TIMER_UNIT_PER_TICK != 0)
			{
			  p->portInstInfo[instIndex].tcWhile -= DOT1S_TIMER_UNIT_PER_TICK;
			  if (p->portInstInfo[instIndex].tcWhile == 0)
			  {
				  timersExpired = L7_TRUE;
			  }

			}

			if (p->portInstInfo[instIndex].fdWhile/DOT1S_TIMER_UNIT_PER_TICK != 0)
			{
                /* If this port is in loop inconsistent state do not decrement the timer
                   so the port will remian stuck in discarding state*/
				if ((p->portInstInfo[instIndex].prtState != PRT_ALTERNATE_PORT) &&
					(p->portInstInfo[instIndex].prtState != PRT_DISABLED_PORT) &&
                    (p->loopInconsistent == L7_FALSE))
				{
			        p->portInstInfo[instIndex].fdWhile -= DOT1S_TIMER_UNIT_PER_TICK;
					if (p->portInstInfo[instIndex].fdWhile == 0)
					{
						timersExpired = L7_TRUE;
					}
				}
				
			}
            
			if (p->edgeDelayWhile/DOT1S_TIMER_UNIT_PER_TICK != 0)
			{
                /* If this port is in loop inconsistent state do not decrement the timer
                   so the port will remian stuck in discarding state*/
				if ((p->portInstInfo[instIndex].prtState != PRT_ALTERNATE_PORT) &&
					(p->portInstInfo[instIndex].prtState != PRT_DISABLED_PORT)&&
                    (p->loopInconsistent == L7_FALSE))
				{
					p->edgeDelayWhile -= DOT1S_TIMER_UNIT_PER_TICK;
					if (p->edgeDelayWhile == 0)
					{
						timersExpired = L7_TRUE;
					}
				}

			}

			if (p->portInstInfo[instIndex].rcvdInfoWhile/DOT1S_TIMER_UNIT_PER_TICK != 0)
			{
			  p->portInstInfo[instIndex].rcvdInfoWhile -= DOT1S_TIMER_UNIT_PER_TICK;
			  if (p->portInstInfo[instIndex].rcvdInfoWhile == 0)
			  {
				  timersExpired = L7_TRUE;
			  }
			}
            
			if (p->portInstInfo[instIndex].rrWhile/DOT1S_TIMER_UNIT_PER_TICK != 0)
			{
				if (p->portInstInfo[instIndex].role != ROLE_ROOT)
				{
			        p->portInstInfo[instIndex].rrWhile -= DOT1S_TIMER_UNIT_PER_TICK;
					if (p->portInstInfo[instIndex].rrWhile == 0)
					{
						timersExpired = L7_TRUE;
					}
				}
				
			}
			  
			if (p->portInstInfo[instIndex].rbWhile/DOT1S_TIMER_UNIT_PER_TICK != 0)
			{
				if (p->portInstInfo[instIndex].role != ROLE_BACKUP)
				{
			        p->portInstInfo[instIndex].rbWhile -= DOT1S_TIMER_UNIT_PER_TICK;
					if (p->portInstInfo[instIndex].rbWhile == 0)
					{
						timersExpired = L7_TRUE;
					}
				}
			  
			}
		  }

		/*call the timer generate event routine which send the events to the
		  dot1sStateMachineClassifier*/
		  if (timersExpired == L7_TRUE)
		  {
		      rc = dot1sTimerGenerateEvents(p, instIndex);
			  timersExpired = L7_FALSE;
		  }
		}/*end for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)*/
	  }/*end if (p->portEnabled == L7_TRUE && p->portLinkState == L7_UP)*/
	}/*end if (p != L7_NULLPTR && p->portNum != 0)*/
  }/*end for (j = 1; j <= (L7_uint32)L7_MAX_INTERFACE_COUNT; j++)*/

  if (wholeSecond == 0)
    wholeSecond = DOT1S_TICKS_PER_SECOND;
  else
    wholeSecond--;

  return rc;
}
/*********************************************************************
* @purpose  Enables MSTP on the bridge
*
* @param    mode  @b{(input)} switch mode 
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchModeEnableSet(L7_uint32 mode)
{
  DOT1S_BRIDGE_t *bridge;
  L7_uint32 i, j, adminState,  nextIntf;
  L7_INTF_TYPES_t itype;
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
    if (bridge->Mode == L7_FALSE)
	{
	  bridge->Mode = L7_TRUE;
	  dot1sCfg->cfg.dot1sBridge.Mode = bridge->Mode;
	  /* The processing of this message involves setting the port state 
	   * in every instance of spanning tree. When there are lots of ports and
	   * lots of instances (say 400 ports and 32 instances) We send over 12000
	   * state set commands to the driver. With the advent of a new async driver
	   * Q and task to handle these sets we hold on to the dot1sTaskSyncSema 
	   * for a long period of time (until all the sets are sent to this driver Q)
	   * On the callback for a set from the driver, to protect the data, we attempt
	   * to take the dot1sTaskSyncSema again. Obviously on the first callback of set 
	   * we block ,as dot1s task is still sending state sets to the driver Q.
	   * This leads to the number of messages getting built up in the driver Q,
	   * which leads to sets not getting complete in a timely manner, which leads to 
	   * interfaces being diagnostically disabled.
	   */

	  /* The proposed solution is to split the sema takes and gives on a per 
	   * interface basis. Thus with every give we allow the driver task do 
	   * its callback. This is ofcource dependant on the OS in use such that OS
	   * lets other pending tasks (on this sema) a chance to execute.
	   * VxWorks follows such a policy and we are making sure Linux too implements
	   * such a scheme.
	   */

	  /* All the sema gives and takes in this routine are done to accomplish the above.
	   * We are gaurenteed that this routine is only called from dot1sDispatch, or
	   * dot1sCnfgrInstApply which is called from dot1sDispatch or dot1sApplyConfigData 
	   * which is unused (dot1sRestore is also obsolete), so 
	   * we can assume that dot1staskSyncSema has already been taken.
	   */
	  (void)osapiSemaGive(dot1sTaskSyncSema);



      /* Loop to Enable Spanning Tree on all enabled valid interfaces
       * that are participating and have already received an L7_UP.
       * Disable all others.
       */
      i = 0;
      while (nimNextValidIntfNumber(i, &nextIntf) == L7_SUCCESS)
      {
        i = nextIntf;
        rc = nimGetIntfType(i, &itype);
        if ((rc == L7_SUCCESS) && 
            ((itype == L7_LAG_INTF) || (itype == L7_PHYSICAL_INTF)))
        {
          rc = nimGetIntfAdminState(i, &adminState);

		  /* Take the task sync sems for this interface alone */
		  (void)osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);

		  p = dot1sIntfFind(i);


          if ((adminState == L7_ENABLE) && (p != L7_NULLPTR) &&
              (p->portLinkState == L7_UP))
          {
            if ((p->portAdminMode == L7_ENABLE) && (p->notParticipating == L7_FALSE))
            {      
              /* Interfaces that are admin enabled, up and configured to 
               * participate in STP.
               */
              rc = dot1sSwitchPortEnableSet(i);
            }
            else
            {
              /* Interfaces that are admin enabled, up and BUT not configured
               * to participate in STP.
               */
              rc = dot1sSwitchPortDisableSet(i);
            }

          }
          else /* Interfaces that are not admin enabled or not up */
          {
            rc = dot1sSwitchPortDisableSet(i);
          }
		  /* Give the task sync sema for this interface allowing the driver to do
		   * it's state set callback.
		   */
		  (void)osapiSemaGive(dot1sTaskSyncSema);

        }/*end if rc*/
      } /* end while nimNextValidIntfNumber */

	  /* Take the task sync sema so that it can be given in the dot1s_task()*/
	  (void)osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);
		
	  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
	  if (p == L7_NULLPTR)
	  {
		return L7_FAILURE;
	  }
	  /*generate begin events for the cist and mstis in use - prs*/
	  for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
	  {
		if (dot1sInstanceMap[j].inUse == L7_TRUE)
		{
		  rc = dot1sStateMachineClassifier(prsBegin, p, j, L7_NULL, L7_NULLPTR); 
		}
      }

	  /* No need to give the task sync sema as it will be given in the dot1s_task() */

	}/*end if (bridge->Mode == L7_FALSE)*/
  }/*end if (bridge != L7_NULLPTR)*/
  else 
  {
	/* No need to give the task sync sema as it will be given in the dot1s_task() */
	return L7_FAILURE;
  }
  /* No need to give the task sync sema as it will be given in the dot1s_task() */
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Disables MSTP on the bridge
*
* @param    mode	@b{(input)} set to L7_DISABLE
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchModeDisableSet(L7_uint32 mode)
{
  DOT1S_BRIDGE_t *bridge;
  L7_uint32 i, j, nextIntf;
  L7_INTF_TYPES_t itype;
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
	if (bridge->Mode == L7_TRUE)
	{
	  bridge->Mode = L7_FALSE;
	  dot1sCfg->cfg.dot1sBridge.Mode = bridge->Mode;

	  /* The processing of this message involves setting the port state 
	   * in every instance of spanning tree. When there are lots of ports and
	   * lots of instances (say 400 ports and 32 instances) We send over 12000
	   * state set commands to the driver. With the advent of a new async driver
	   * Q and task, to handle these sets, we hold on to the dot1sTaskSyncSema 
	   * for a long period of time (until all the sets are sent to this driver Q)
	   * On the callback for a set from the driver, to protect the data, we attempt
	   * to take the dot1sTaskSyncSema again. Obviously on the first callback of set 
	   * we block ,as dot1s task is still sending state sets to the driver Q.
	   * This leads to the number of messages getting built up in the driver Q,
	   * which leads to sets not getting complete in a timely manner, which leads to 
	   * interfaces being diagnostically disabled.
	   */

	  /* The proposed solution is to split the sema takes and gives on a per 
	   * interface basis. Thus with every give we allow the driver task do 
	   * its callback. This is ofcource dependant on the OS in use such that OS
	   * lets other pending tasks (on this sema) a chance to execute.
	   * VxWorks follows such a policy and we are making sure Linux too implements
	   * such a scheme.
	   */

	  /* All the sema gives and takes in this routine are done to accomplish the above.
	   * We are gaurenteed that this routine is only called from dot1sDispatch, or
	   * dot1sCnfgrInstApply which is called from dot1sDispatch or dot1sApplyConfigData 
	   * which is unused (dot1sRestore is also obsolete), so 
	   * we can assume that dot1staskSyncSema has already been taken.
	   */
	  (void)osapiSemaGive(dot1sTaskSyncSema);

      /* Loop to disable Spanning Tree on all valid interfaces.
       */
      i = 0;
      while (nimNextValidIntfNumber(i, &nextIntf) == L7_SUCCESS)
      {
        i = nextIntf;
        rc = nimGetIntfType(i, &itype);
        if ((rc == L7_SUCCESS) && 
            ((itype == L7_LAG_INTF) || (itype == L7_PHYSICAL_INTF)))
        {
		  /* Take the task sync sems for this interface alone */
		  (void)osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);

          p = dot1sIntfFind(i);
          if (p != L7_NULLPTR)
          {
            if (p->portEnabled == L7_TRUE && p->portNum != 0)
            {
              rc = dot1sSwitchPortDisableSet(i);
            }

            /* On behalf of NIM, queue up the setting of the state 
             * for the port as if MSTP was never enabled.
             */
            rc = dot1sIssueCmd(switchPortStateSet, p->portNum, 0, L7_NULLPTR);
          }
		  /* Give the task sync sema for this interface allowing the driver to do
		   * it's state set callback.
		   */
		  (void)osapiSemaGive(dot1sTaskSyncSema);
        }
      } /* end while nimNextValidIntfNumber */

	  /* Take the task sync sema so that it can be given in the dot1s_task()*/
	  (void)osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);

	  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
	  if (p == L7_NULLPTR)
	  {
		return L7_FAILURE;
	  }
	  /*generate begin events for the cist and mstis in use - prs*/
	  for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
	  {
		if (dot1sInstanceMap[j].inUse == L7_TRUE)
		{
		  rc = dot1sStateMachineClassifier(prsBegin, p, j, L7_NULL, L7_NULLPTR);
		}
      }

	  /* No need to give the task sync sema as it will be given in the dot1s_task() */

	}/*end if (bridge->Mode == L7_TRUE)*/
  }/*end if (bridge != L7_NULLPTR)*/
  else 
  {
	/* No need to give the task sync sema as it will be given in the dot1s_task() */
	return L7_FAILURE;
  }

  /* No need to give the task sync sema as it will be given in the dot1s_task() */
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the force version for the bridge
*
* @param    forceVer	@b{(input)} force version for the bridge
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchForceVerSet(L7_uint32 forceVer)
{
  DOT1S_BRIDGE_t *bridge;
  L7_uint32 i;
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
	 if (bridge->ForceVersion == forceVer)
	 {
		 return L7_SUCCESS; /* Do not do any thing */
	 }
	bridge->ForceVersion = forceVer;
	
  }
  else 
	return L7_FAILURE;
  
  /*Generate events due to this change in force version*/
  if (bridge->Mode == L7_TRUE)
  {
	for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
	{
	  p = dot1sIntfFind(i);
	  if (p != L7_NULLPTR && p->portNum != L7_NULL)
	  {
	     rc = dot1sStateMachineClassifier(ppmBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
	    
	  }/*end if (p != L7_NULLPTR && p->portNum != L7_NULL)*/
    }/*end for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)*/
  }/*end if (bridge->Mode == L7_TRUE)*/
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the format selector for this bridge
*
* @param    formatSelector	@b{(input)} format selector 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments should always be set to 0
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchCfgForSelSet(L7_uint32 formatSelector)
{
  DOT1S_BRIDGE_t *bridge;
  L7_RC_t rc = L7_SUCCESS;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
	bridge->MstConfigId.formatSelector = (L7_uchar8)formatSelector;
	dot1sCfg->cfg.dot1sBridge.MstConfigId.formatSelector = bridge->MstConfigId.formatSelector;
  }
  else 
	return L7_FAILURE;
  
  return rc;
}
/*********************************************************************
* @purpose  Force the Port to send RSTP or MSTP BPDU's
*
* @param    intIfNum	@b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchPortMigrationCheck(L7_uint32 intIfNum)
{
  DOT1S_BRIDGE_t *pBridge;
  DOT1S_PORT_COMMON_t *pPort;
  L7_RC_t rc = L7_FAILURE;

  pBridge = dot1sBridgeParmsFind();

  if (pBridge != L7_NULLPTR)
  {
    pPort = dot1sIntfFind(intIfNum);

    if (pPort != L7_NULLPTR)
    {
      if ((pPort->portNum != L7_NULL) && 
          (pBridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W))
      {
        pPort->mcheck = L7_TRUE;
        /*Send Event E62*/
        rc = dot1sStateMachineClassifier(ppmMCheck, pPort, L7_NULL, L7_NULL, L7_NULLPTR);
      }
      else
      {
        pPort->mcheck = L7_FALSE;
        return L7_FAILURE;
      }
    }
  }
  
  return rc;
}
/*********************************************************************
* @purpose  Sets the bridge revision number
*
* @param    switchRevision	@b{(input)} revision number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchRevSet(L7_uint32 switchRevision)
{
  DOT1S_BRIDGE_t *bridge;
  L7_RC_t rc;
  L7_uint32 instId, instIndex, intIfNum;
  DOT1S_PORT_COMMON_t *p;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
	bridge->MstConfigId.revLevel = (L7_ushort16)switchRevision;
	dot1sCfg->cfg.dot1sBridge.MstConfigId.revLevel = bridge->MstConfigId.revLevel;
    for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
    {
      if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
      {
        instId = dot1sInstanceMap[instIndex].instNumber;

        /* generate begin events for all the ports that are enabled for each msti 
         * and cist that is in use - pim, prt, pst, tcm
         */
        for (intIfNum = 1; intIfNum <= (L7_uint32)L7_MAX_INTERFACE_COUNT; intIfNum++)
        {
          p = dot1sIntfFind(intIfNum);
          if (p != L7_NULLPTR && p->portEnabled == L7_TRUE && p->portNum != 0)
          {
			rc = dot1sStateMachineClassifier(pstBegin, p, instIndex, L7_NULL, L7_NULLPTR);
            rc = dot1sStateMachineClassifier(pimBegin, p, instIndex, L7_NULL, L7_NULLPTR);
            rc = dot1sStateMachineClassifier(prtBegin, p, instIndex, L7_NULL, L7_NULLPTR);
            rc = dot1sStateMachineClassifier(tcmBegin, p, instIndex, L7_NULL, L7_NULLPTR);
			rc = dot1sStateMachineClassifier(pimPortEnabled, p, instIndex, L7_NULL, L7_NULLPTR);
			rc = dot1sPortStateMachineUpTimeReset(p);
          }
        }
        p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
	    if (p != L7_NULLPTR)
	    {
		  rc = dot1sStateMachineClassifier(prsBegin, p, instIndex, L7_NULL, L7_NULLPTR);
	    }
	  }
    }

  }/*end if (bridge != L7_NULLPTR)*/
  else 
  {
	return L7_FAILURE;
  }
  

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets this MSTP Port Administrative Mode to Enable
*
* @param    intIfNum @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchPortAdminModeEnableSet(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t             rc;
  L7_uint32           adminState;
  L7_BOOL             acquired = L7_FALSE;
  
  /* check if previously acquired */
  DOT1S_ACQ_NONZEROMASK(dot1sPort[intIfNum].acquiredList, acquired);
  
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	if (p->portAdminMode == L7_DISABLE)
	{
	  p->portAdminMode = L7_ENABLE;

      if (p->ignoreLinkStateChanges != L7_TRUE &&
		  acquired == L7_FALSE)
      {
        p->notParticipating = L7_FALSE;
      }

      /* See if this port can be enabled now that it has been
       * configured for MSTP.
       */
      rc = nimGetIntfAdminState(intIfNum, &adminState);
      if ((dot1sBridge->Mode == L7_ENABLE) &&
          (adminState == L7_ENABLE) &&
          (p->portLinkState == L7_UP) &&
          (p->ignoreLinkStateChanges == L7_FALSE) &&
		  (acquired == L7_FALSE))
      {
		rc = dot1sSwitchPortEnableSet(intIfNum);
      }

	}
	
	return L7_SUCCESS;
  }
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets this MSTP Port Administrative Mode to Disable
*
* @param    intIfNum	@b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchPortAdminModeDisableSet(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  DOT1S_PORT_COMMON_t *p;

  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	if (p->portAdminMode == L7_ENABLE)
	{
	  p->portAdminMode = L7_DISABLE;
	  p->notParticipating = L7_TRUE;

      /* See if this port should be disabled now that it has been
       * un-configured for MSTP.
       */
      if (p->portEnabled == L7_TRUE)
        rc = dot1sSwitchPortDisableSet(intIfNum);

	}
	
	return L7_SUCCESS;
  }
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the MSTP Port Operational Mode to Enabled
*
* @param    intIfNum	@b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchPortEnableSet(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_t *p;
  DOT1S_BRIDGE_t *b;
  L7_uint32 j;
  L7_RC_t rc;

  p = dot1sIntfFind(intIfNum);
  b = dot1sBridgeParmsFind();

  if (p != L7_NULLPTR && p->portNum != 0)
  {
	if ((p->portEnabled == L7_FALSE) && (b->enabledPortCount < DOT1S_MAX_PORT_COUNT))
	{
	  p->portEnabled = L7_TRUE;
    if (p->portLinkState == L7_UP)
    {
      p->portUpEnabledNum = p->portNum;
    }
      b->enabledPortCount++;
      p->bpduGuardEffect = L7_FALSE;

      /* Calculate the cost for all instances */
      rc = dot1sCalcPortPathCost(intIfNum);

      /*generate begin events for all the ports that are enabled prx, ppm, ptx, bdm*/
	  rc = dot1sStateMachineClassifier(prxBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
	  rc = dot1sStateMachineClassifier(ppmBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
	  rc = dot1sStateMachineClassifier(ptxBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
	  if (p->adminEdge == L7_TRUE)
		  rc = dot1sStateMachineClassifier(bdmBeginAdminEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);
	  else
		  rc = dot1sStateMachineClassifier(bdmBeginNotAdminEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);


	  for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
	  {
        if (dot1sInstanceMap[j].inUse == L7_TRUE)
		{
		  /*generate begin events for all the ports that are enabled for each msti and cist that is in use - pim, prt, pst, tcm*/
		  rc = dot1sStateMachineClassifier(pstBegin, p, j, L7_NULL, L7_NULLPTR);
		  rc = dot1sStateMachineClassifier(pimBegin, p, j, L7_NULL, L7_NULLPTR);
		  rc = dot1sStateMachineClassifier(prtBegin, p, j, L7_NULL, L7_NULLPTR);
		  rc = dot1sStateMachineClassifier(tcmBegin, p, j, L7_NULL, L7_NULLPTR);
		  rc = dot1sStateMachineClassifier(pimPortEnabled, p, j, L7_NULL, L7_NULLPTR);
        }

      }/*end for (j = DOT1S_CIST_INDEX...*/
	  
	  rc = dot1sPortStateMachineUpTimeReset(p);
	}/*end if ((p->portEnabled == L7_FALSE)...*/
	
	return L7_SUCCESS;
  }/*end if (p != L7_NULLPTR && p->portNum != 0)*/
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the MSTP Port Operational Mode to Disable
*
* @param    intIfNum	@b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchPortDisableSet(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_t *p;
  DOT1S_BRIDGE_t *b;
  L7_uint32 j, state, adminState, instIndex;
  L7_RC_t rc;

  p = dot1sIntfFind(intIfNum);
  b = dot1sBridgeParmsFind();

  if (p != L7_NULLPTR && p->portNum != 0)
  {
	if (p->portEnabled == L7_TRUE)
	{
	  p->portEnabled = L7_FALSE;
    p->portUpEnabledNum = 0;
      b->enabledPortCount--;
      p->bpduGuardEffect = L7_FALSE;
      
      /* Revert to defaults for all fields except the configured ones */
      rc = dot1sPortDefaultPopulate(p, intIfNum);

      /* Zero out when autocalculate is set */
      rc = dot1sCalcPortPathCost(intIfNum);

	  /*Send event E69*/
	  /*generate begin events for all the ports that are enabled prx, ppm, ptx, bdm*/
	  rc = dot1sStateMachineClassifier(prxBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
	  rc = dot1sStateMachineClassifier(ppmBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
	  rc = dot1sStateMachineClassifier(ptxBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
	  if (p->adminEdge == L7_TRUE)
		  rc = dot1sStateMachineClassifier(bdmBeginAdminEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);
	  else
		  rc = dot1sStateMachineClassifier(bdmBeginNotAdminEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);

      for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
	  {
		if (dot1sInstanceMap[j].inUse == L7_TRUE)
		{
		  /*generate begin events for all the ports that are enabled for each msti and cist that is in use - pim, prt, pst, tcm*/
          rc = dot1sStateMachineClassifier(pstBegin, p, j, L7_NULL, L7_NULLPTR);
		  rc = dot1sStateMachineClassifier(pimBegin, p, j, L7_NULL, L7_NULLPTR);
		  rc = dot1sStateMachineClassifier(prtBegin, p, j, L7_NULL, L7_NULLPTR);
		  rc = dot1sStateMachineClassifier(tcmBegin, p, j, L7_NULL, L7_NULLPTR);
		}
      }
	  rc = dot1sPortStateMachineUpTimeReset(p);
	}/*end if (p->portEnabled == L7_TRUE)*/

    rc = nimGetIntfAdminState(intIfNum, &adminState);

    if (p->bpduGuardEffect == L7_TRUE)
    {
      state = L7_DOT1S_BPDU_GUARD;
    }
    else if ((p->portLinkState == L7_UP) && (adminState == L7_ENABLE))
	{
	  state = L7_DOT1S_MANUAL_FWD;
	}
    else
    {
	  state = L7_DOT1S_DISABLED;
    }

    /*rc = dot1sIssueCmd(dot1sIntfStateSet, p->portNum, L7_NULL, &state);*/
    if (COMPONENT_ACQ_ISMASKBITSET(dot1sPort[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID)) {
        /* This interface has been acquired by lag we need to set state on this port for display 
		 * purposes. NIM will take care of setting the lag state to the physical port
         */
		for (instIndex=0; instIndex<= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
		{
		  dot1sPort[intIfNum].portInstInfo[instIndex].portState = state;
		}

    }
    else
        dot1sIhPortStateAllSet(p->portNum, state);

	return L7_SUCCESS;
  }/*end if (p != L7_NULLPTR && p->portNum != 0)*/
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  On behalf of NIM, set the Port State to Manual Forwarding
* @purpose  or Disabled.
*
* @param    intIfNum	@b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchPortStateSet(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFind(intIfNum);

  if ((pPort != L7_NULLPTR) && (pPort->portNum != 0) && 
      (pPort->ignoreLinkStateChanges != L7_TRUE))
  {
    if (pPort->portLinkState == L7_UP)
    {
      dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, intIfNum, L7_DOT1S_MANUAL_FWD);                      
    }
    else
    {
      dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, intIfNum, L7_DOT1S_DISABLED);                      
    }

	return L7_SUCCESS;
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Update the MSTP structures with the new Mac Address.
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchMacAddrChange()
{
  DOT1S_INSTANCE_INFO_t *instance;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 instIndex, intIfNum;
  DOT1S_PORT_COMMON_t *pPort;
  
  instance = dot1sInstFind();

  /* update Mac Address values for the cist and msti dot1sInstance structures */
  for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    rc = dot1sInstanceMacAddrPopulate(instance, instIndex);

    /* For all ports of the Bridge, the reselect for the CIST parameter 
     * is set TRUE, and the selected paramter for the CIST is set FALSE.
     */
    rc = dot1sReselectTreeSet(instIndex);
    rc = dot1sSelectedTreeClear(instIndex);

    for (intIfNum = 1; intIfNum <= (L7_uint32)L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      pPort = dot1sIntfFind(intIfNum);
      if (pPort != L7_NULLPTR)
      {
      /* update Mac Address values for the dot1sport structures */
      rc = dot1sPortInstInfoMacAddrPopulate(pPort, instIndex);

      }
    }
	/* generate reselect event since the tree has been set */
	/*use a valid port number for the event to be processed*/
    pPort = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
	if (pPort != L7_NULLPTR)
	{
	  rc = dot1sStateMachineClassifier(prsReselect, pPort, instIndex, L7_NULL, L7_NULLPTR);
	}
  }
  
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the bridge priority for the CIST
*
* @param    instId		@b{(input)} instance id for the CIST
* @param    priority	@b{(input)} priority for the CIST
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments only the 4 msbits are taken from this 
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPriSet(L7_uint32 instId, L7_uint32 priority)
{
  L7_ushort16 shortPri;
  L7_uint32 instIndex;
  L7_RC_t rc;
  DOT1S_PORT_COMMON_t *p;

  shortPri = (L7_ushort16)priority;

  /*the instId part of this priInstId is all zeros for the CIST*/
  dot1sInstance->cist.BridgeIdentifier.priInstId = shortPri; 
  dot1sInstance->cist.CistBridgePriority.rootId.priInstId = shortPri;
  dot1sInstance->cist.CistBridgePriority.regRootId.priInstId = shortPri;
  dot1sInstance->cist.CistBridgePriority.dsgBridgeId.priInstId = shortPri;

  rc = dot1sInstIndexFind(instId, &instIndex);

  /* For all ports of the Bridge, the reselect for the CIST parameter 
   * is set TRUE, and the selected paramter for the CIST is set FALSE.
   */
  rc = dot1sReselectTreeSet(instIndex);
  rc = dot1sSelectedTreeClear(instIndex);

  /* generate reselect event since the tree has been set */
  /*use a valid port number for event to be processed*/
  /*rc = dot1sIssueCmd(prsReselect, DOT1S_VALID_PORT_NUM, instId, L7_NULLPTR);*/
  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
  if (p != L7_NULLPTR)
  {
  rc = dot1sStateMachineClassifier(prsReselect, p, instIndex, L7_NULL, L7_NULLPTR);
  }
  
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the bridge max age
*
* @param    maxAge	@b{(input)} bridge max age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonBridgeMaxAgeSet(L7_uint32 maxAge)
{
  L7_uint32 i, j;
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;

  dot1sInstance->cist.CistBridgeTimes.bridgeMaxAge = maxAge;  
  /*set this value in all the porttimes variables as well*/
  /*also update the remHop count with this value divided by 256*/
  for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
  {
    p = dot1sIntfFind(i);
	if (p != L7_NULLPTR && p->portNum != 0)
	{
	  p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.maxAge = maxAge;
	  if (p->portAdminMode == L7_ENABLE)
	  {
		p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
		p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;
		for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
		{
		  p->portInstInfo[j].reselect = L7_TRUE;
		  p->portInstInfo[j].selected = L7_FALSE;
		}
	  }
	}
  }
  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
  for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
	if (dot1sInstanceMap[j].inUse == L7_TRUE)
	{
	  if (p != L7_NULLPTR)
	  {
		rc = dot1sStateMachineClassifier(prsReselect, p, j, L7_NULL, L7_NULLPTR);
	  }
	}
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the bridge max age
*
* @param    maxAge	@b{(input)} bridge max age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonBridgeMaxHopSet(L7_uint32 maxHop)
{
  L7_uint32 i, j;
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;
  DOT1S_BRIDGE_t *pBridge;
  
  pBridge = dot1sBridgeParmsFind();

  if (pBridge != L7_NULLPTR)
  {
	pBridge->MaxHops = maxHop;

  }
  dot1sInstance->cist.CistBridgeTimes.maxHops = maxHop;
  for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
	dot1sInstance->msti[j].MstiBridgeTimes.bridgeMaxHops = maxHop;
  }
  /* set this value in all the porttimes variables as well*/
  /* also update the remHop count with this value */
  for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
  {
    p = dot1sIntfFind(i);
	if (p != L7_NULLPTR && p->portNum != 0)
	{
	  p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.remHops = maxHop;
	  for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
	  {
		p->portInstInfo[j].inst.msti.mstiPortTimes.remHops = maxHop;
      }
	  if (p->portAdminMode == L7_ENABLE)
	  {
		p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
		p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;
		for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
		{
		  p->portInstInfo[j].reselect = L7_TRUE;
		  p->portInstInfo[j].selected = L7_FALSE;
		}
	  }
	}
  }
  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
  for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
	if (dot1sInstanceMap[j].inUse == L7_TRUE)
	{
	  if (p != L7_NULLPTR)
	  {
		rc = dot1sStateMachineClassifier(prsReselect, p, j, L7_NULL, L7_NULLPTR);
	  }
	}
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the bridge hold count
*
* @param    maxAge	@b{(input)} bridge max age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonBridgeTxHoldCountSet(L7_uint32 holdCount)
{
  DOT1S_BRIDGE_t *pBridge;
  
  pBridge = dot1sBridgeParmsFind();

  /* Validate */
  if ((holdCount > L7_DOT1S_BRIDGE_HOLDCOUNT_MAX) ||
	  (holdCount < L7_DOT1S_BRIDGE_HOLDCOUNT_MIN))
  {
	  
	  return L7_FAILURE;
  }

  if (pBridge != L7_NULLPTR)
  {
	pBridge->TxHoldCount  = holdCount;

  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the hello time for the bridge
*
* @param    helloTime	@b{(input)} hello time for the bridge
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonBridgeHelloTimeSet(L7_uint32 helloTime)
{

 LOG_MSG("Acc. IEEE 802.1Q-REV 2005 updating hello time is disallowed \n");
#if 0
  L7_uint32 i, j;
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;
  dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime = helloTime;
  /*set this value in all the porttimes variables as well*/
  for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
  {
    p = dot1sIntfFind(i);
	if (p != L7_NULLPTR && p->portNum != 0)
	{
	  p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.helloTime = helloTime;
	  
	  if (p->portAdminMode == L7_ENABLE)
	  {
		p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
		p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;
		for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
		{
		  p->portInstInfo[j].reselect = L7_TRUE;
		  p->portInstInfo[j].selected = L7_FALSE;
		}
	  }
	}
  }
  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
  for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
	if (dot1sInstanceMap[j].inUse == L7_TRUE)
	{
	  if (p != L7_NULLPTR)
	  {
		rc = dot1sStateMachineClassifier(prsReselect, p, j, L7_NULL, L7_NULLPTR);
	  }
	}
  }
#endif
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the bridge forward delay timer value
*
* @param   	fwdDelay	@b{(input)} forward delay timer value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonBridgeFwdDelaySet(L7_uint32 fwdDelay)
{
  L7_uint32 i, j;
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;
  DOT1S_BRIDGE_t 	*bridge;

  dot1sInstance->cist.CistBridgeTimes.bridgeFwdDelay = fwdDelay;
  bridge = dot1sBridgeParmsFind();
  if(bridge == L7_NULLPTR)
	return L7_FAILURE;
  bridge->FwdDelay = fwdDelay;
  dot1sCfg->cfg.dot1sBridge.FwdDelay = fwdDelay;
  /*set this value in all the porttimes variables as well*/
  for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
  {
    p = dot1sIntfFind(i);
	if (p != L7_NULLPTR && p->portNum != 0)
	{
	  p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.fwdDelay = fwdDelay;
	  
	  if (p->portAdminMode == L7_ENABLE)
	  {
		p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
		p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;
		for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
		{
		  p->portInstInfo[j].reselect = L7_TRUE;
		  p->portInstInfo[j].selected = L7_FALSE;
		}
	  }
	}
  }
  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
  for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
	if (dot1sInstanceMap[j].inUse == L7_TRUE)
	{
	  if (p != L7_NULLPTR)
	  {
		rc = dot1sStateMachineClassifier(prsReselect, p, j, L7_NULL, L7_NULLPTR);
	  }
	}
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the path cost for this interface for the CIST
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	pathCost	@b{(input)} path cost for this interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortPathCostSet(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 pathCost)
{
  L7_RC_t rc;
  DOT1S_PORT_COMMON_t *p;
  
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	p->portInstInfo[DOT1S_CIST_INDEX].InternalPortPathCost = pathCost;

    /* Update the Auto Port Path Cost setting */
    if (pathCost == L7_DOT1S_AUTO_PORT_PATHCOST)
    {
      p->portInstInfo[DOT1S_CIST_INDEX].autoInternalPortPathCost = L7_TRUE;
      

      /* Enabled ports need the Auto calculation completed now */
      if (dot1sPort[intIfNum].portEnabled == L7_TRUE)
      {
        /* Calculate the cost for all instances.
         * It is not really necessary to re-calculate the Path Cost 
         * for all instances at this time, only the current instance,
         * but doing so will not hurt anything.
         */
        rc = dot1sCalcPortPathCost(intIfNum);
      }
    }
    else
    {
      p->portInstInfo[DOT1S_CIST_INDEX].autoInternalPortPathCost = L7_FALSE;
      
    }
     
    /* Procedure 12.8.2.5.4 */
    /* The reselect parameter value for the CIST for the port
     * is set TRUE, and the selected paramter for the CIST for
     * the port is set FALSE.
     */
    p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
    p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;

    /* generate reselect event */
    rc = dot1sStateMachineClassifier(prsReselect, p, DOT1S_CIST_INDEX, L7_NULL, L7_NULLPTR);


	return L7_SUCCESS;
  }/*end if (p != L7_NULLPTR && p->portNum != 0)*/
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Stes the port priority for the CIST
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	portPri		@b{(input)} port priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortPriSet(L7_uint32 intIfNum, L7_uint32 instId, L7_uint32 portPri)
{
  L7_RC_t rc;
  L7_ushort16	shortPri;
  L7_ushort16   currentPortId;
  DOT1S_PORT_COMMON_t *p;

  /* shift to proper position in Port ID struct */
  shortPri = (L7_ushort16)(portPri << 8);

  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	currentPortId = p->portInstInfo[DOT1S_CIST_INDEX].portId;

	/* Clear the current priority but preserve the port number */
	currentPortId = (currentPortId & DOT1S_PORTNUM_MASK);

	currentPortId = currentPortId | shortPri;

	p->portInstInfo[DOT1S_CIST_INDEX].portId = currentPortId;
	p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.rxPortId = currentPortId;
	p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority.rxPortId = currentPortId;
	p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.rxPortId = currentPortId;
    
    /* Procedure 12.8.2.5.4 */
    /* The reselect parameter value for the CIST for the port
     * is set TRUE, and the selected paramter for the CIST for
     * the port is set FALSE.
     */
    p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
    p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;

    /* generate reselect event */
    rc = dot1sStateMachineClassifier(prsReselect, p, DOT1S_CIST_INDEX, L7_NULL, L7_NULLPTR);


	return L7_SUCCESS;
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the port edge property to enabled
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	edgeMode	@b{(input)} Edge mode for this port L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortEdgePortEnable(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 edgeMode)
{
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;

  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	p->adminEdge = L7_TRUE;

   /* call the bpdu filter api to set the filter mode */
    if(dot1sCfg->cfg.dot1sBridge.bpduFilterMode == L7_TRUE &&
       p->bpduFilter != L7_TRUE)
    {
      if (dtlDot1sBpduFilterSet(intIfNum,L7_TRUE) != L7_SUCCESS)
      {
        LOG_MSG("Failed to set BPDU Filter value for %d\n",intIfNum);
        return L7_FAILURE;
      }
    }

    rc = dot1sStateMachineClassifier(bdmBeginAdminEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);
	
	return L7_SUCCESS;
  }
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the port edge property to disabled
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	edgeMode	@b{(input)} Edge mode for this port L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonEdgePortDisable(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 edgeMode)
{
DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;
  
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	p->adminEdge = L7_FALSE;

    /* call the bpdu filter api to set the filter mode */
    if(dot1sCfg->cfg.dot1sBridge.bpduFilterMode == L7_TRUE &&
       p->bpduFilter != L7_TRUE)
    {
      if (dtlDot1sBpduFilterSet(intIfNum,L7_FALSE) != L7_SUCCESS)
      {
        LOG_MSG("Failed to reset BPDU Filter value for %d\n",intIfNum);
        return L7_FAILURE;
      }
    }

    if (p->bpduGuardEffect == L7_TRUE)
    {
      dot1sResetBpduGuardEffect(p);
    }

    rc = dot1sStateMachineClassifier(bdmBeginNotAdminEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);

	return L7_SUCCESS;
  }
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the port edge property to disabled
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	edgeMode	@b{(input)} Edge mode for this port L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortRestrictedRoleSet(L7_uint32 intIfNum, L7_uint32 instId,L7_BOOL mode)
{
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i;
  
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
    p->restrictedRole = mode;


    /* generate reselect event */
    for (i = DOT1S_CIST_INDEX; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
      if (dot1sInstanceMap[i].inUse == L7_TRUE)
      {
        p->portInstInfo[i].reselect = L7_TRUE;
        p->portInstInfo[i].selected = L7_FALSE;
        rc = dot1sStateMachineClassifier(prsReselect, p, i, L7_NULL, L7_NULLPTR);
      }
    }
    return rc;
  }
  else
    return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the loopguard to the port
*
* @param  intIfNum  @b{(input)} internal interface number
* @param  instId    @b{(input)} instance id
* @param  mode      @b{(input)} mode for this port L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortLoopGuardSet(L7_uint32 intIfNum, L7_uint32 instId, L7_BOOL mode)
{
  DOT1S_PORT_COMMON_t *p;
    
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
    p->loopGuard = mode;

    if ( mode == L7_FALSE )
    {
        /* If loopguard is being clear and port is inconsistent state .. 
           restart all dot1s state machines*/
      if ( p->loopInconsistent == L7_TRUE )
      {
        dot1sLoopInconsistentSet(p,DOT1S_CIST_INDEX,L7_FALSE); 
        dot1sSwitchPortEnableSet(intIfNum);
      }      
    }  
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the port edge property to disabled
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	edgeMode	@b{(input)} Edge mode for this port L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortRestrictedTcnSet(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 mode)
{
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;
  
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	p->restrictedTcn = mode;

	p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
    p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;

    /* generate reselect event */
    rc = dot1sStateMachineClassifier(prsReselect, p, DOT1S_CIST_INDEX, L7_NULL, L7_NULLPTR);
	return rc;
  }
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the port edge property to disabled
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	edgeMode	@b{(input)} Edge mode for this port L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortAutoEdgeSet(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 mode)
{
  DOT1S_PORT_COMMON_t *p;
  
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	p->autoEdge = mode;

	generateEventsForBDM(p);

	return L7_SUCCESS;
  }
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Find an available Index into the Instance List
*
* @param    index @b{(output)} will contain an available instance index if L7_SUCCESS
*                              or L7_NULL if L7_FAILURE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstanceAvailIndexFind(L7_uint32 *index)
{
  DOT1S_BRIDGE_t *bridge;
  L7_uint32 instIndex;
  
  bridge = dot1sBridgeParmsFind();

  if ((bridge != L7_NULLPTR) && (bridge->instanceCount < L7_MAX_MULTIPLE_STP_INSTANCES))
  {
    for (instIndex = 1; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
    {
      if (dot1sInstanceMap[instIndex].inUse == L7_FALSE)
      {
        *index = instIndex;
        return L7_SUCCESS;
      }
    }
  }
  *index = L7_NULL;

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Process an Apply Config request to create an instance from
*           a saved configuration file.
*
* @param    instId	@b{(input)} Instance id for this instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sApplyCfgInstanceCreate(L7_uint32 instId)
{
  L7_uint32 instIndex;

  if (dot1sInstanceAvailIndexFind(&instIndex) == L7_SUCCESS)
  {
    return(dot1sInstanceCreate(instIndex, instId));
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Process a User request to create an instance
*
* @param    instId	@b{(input)} Instance id for this instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sUserInstanceCreate(L7_uint32 instId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 instIndex;

  
  if (dot1sInstCheckInUse(instId) == L7_SUCCESS)
  {
	LOG_MSG("dot1sUserInstanceCreate: Instance (%d) already present\n",instId);
	return rc;
  }

  if (dot1sInstanceAvailIndexFind(&instIndex) == L7_SUCCESS)
  {
    dot1sInstanceDefaultPopulate(dot1sInstFind(), instIndex, instId);
	dot1sCfgInstanceDefaultPopulate(&dot1sCfg->cfg.dot1sInstance, instIndex, instId);
    rc = dot1sInstanceCreate(instIndex, instId) ;

        
  }

  if (rc != L7_SUCCESS)
  {
     /*Reset the datachanged flag since the configuration failed */
     dot1sCfg->hdr.dataChanged = L7_FALSE;
  }

  return rc;
}
/*********************************************************************
* @purpose  Create an instance of spanning tree using the supplied index.
*
* @param    instId	  @b{(input)} Instance id for this instance
* @param    instIndex @b{(input)} Index for this instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments the index must be valid and available
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstanceCreate(L7_uint32 instIndex, L7_uint32 instId)
{
  DOT1S_PORT_COMMON_t *p;
  L7_RC_t rc;
  L7_uint32 intIfNum;
  DOT1S_BRIDGE_t *bridge;
  
  /* sanity check even though this function should only be called
   * with a valid and available index to Create the instance.
   */
  if (dot1sInstanceMap[instIndex].inUse == L7_FALSE)
  {
    dot1sInstanceMap[instIndex].inUse = L7_TRUE;
    dot1sInstanceMap[instIndex].instNumber = instId;
	dot1sCfg->cfg.dot1sInstanceMap[instIndex].inUse = L7_TRUE;
	dot1sCfg->cfg.dot1sInstanceMap[instIndex].instNumber = instId;

    if ((bridge = dot1sBridgeParmsFind()) != L7_NULLPTR)
    {
      bridge->instanceCount++;
    }
    else
    {
      return L7_FAILURE;
    }

    /* Propagate this instance creation to the rest of the system */
    rc = dot1sIhDoNotifyInstanceCreate(instId);

    /* Load defaults for all the ports in the system */
    for (intIfNum = 1; intIfNum <= (L7_uint32)L7_MAX_INTERFACE_COUNT; intIfNum++)
    {

      if (dot1sIsValidIntf(intIfNum) != L7_TRUE)
          continue;

      p = dot1sIntfFind(intIfNum);
      if (p != L7_NULLPTR)
      {
        (void)dot1sPortInstInfoDefaultPopulate(p, instIndex, intIfNum);
	  }
    }
    /* Generate begin events for all the ports that are enabled for each 
     * msti and cist that is in use - pim, prt, pst, tcm
     */
    for (intIfNum = 1; intIfNum <= (L7_uint32)L7_MAX_INTERFACE_COUNT; intIfNum++)
    {

      if (dot1sIsValidIntf(intIfNum) != L7_TRUE)
          continue;

      p = dot1sIntfFind(intIfNum);
      if (p != L7_NULLPTR && p->portEnabled == L7_TRUE && p->portNum != 0)
      {
		rc = dot1sStateMachineClassifier(pstBegin, p, instIndex, L7_NULL, L7_NULLPTR);
        rc = dot1sStateMachineClassifier(pimBegin, p, instIndex, L7_NULL, L7_NULLPTR);
        rc = dot1sStateMachineClassifier(prtBegin, p, instIndex, L7_NULL, L7_NULLPTR);
        rc = dot1sStateMachineClassifier(tcmBegin, p, instIndex, L7_NULL, L7_NULLPTR);
		rc = dot1sStateMachineClassifier(pimPortEnabled, p, instIndex, L7_NULL, L7_NULLPTR);
	  }
	  if(p != L7_NULLPTR) 
        rc = dot1sPortStateMachineUpTimeReset(p);
    }
    p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
	if (p != L7_NULLPTR)
	{
	  rc = dot1sStateMachineClassifier(prsBegin, p, instIndex, L7_NULL, L7_NULLPTR);
	}

    return L7_SUCCESS;
  }/*end if (dot1sInstanceMap[instIndex].inUse == L7_FALSE)*/

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Delete the instance of spanning tree
*
* @param    instId	@b{(input)} instance id  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstanceDelete(L7_uint32 instId)
{
  DOT1S_BRIDGE_t *bridge;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i, intIfNum;
  DOT1S_PORT_COMMON_t *p;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
	  for (i = 1; i <= (L7_uint32)L7_MAX_MULTIPLE_STP_INSTANCES; i++)
	  {
		if (dot1sInstanceMap[i].instNumber == instId)
		{
		  /*generate begin events for all the ports that are enabled for each 
           *msti and cist that is in use - pim, prt, pst, tcm*/
		  for (intIfNum = 1; intIfNum <= (L7_uint32)L7_MAX_INTERFACE_COUNT; intIfNum++)
		  {
			p = dot1sIntfFind(intIfNum);
			if (p != L7_NULLPTR && p->portEnabled == L7_TRUE && p->portNum != 0)
			{
			  rc = dot1sStateMachineClassifier(pstBegin, p, i, L7_NULL, L7_NULLPTR);
			  rc = dot1sStateMachineClassifier(pimBegin, p, i, L7_NULL, L7_NULLPTR);
			  rc = dot1sStateMachineClassifier(prtBegin, p, i, L7_NULL, L7_NULLPTR);
			  rc = dot1sStateMachineClassifier(tcmBegin, p, i, L7_NULL, L7_NULLPTR);
			}
			if (p != L7_NULLPTR) {
			  rc = dot1sPortStateMachineUpTimeReset(p);
			}
		  }
		  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
		  if (p != L7_NULLPTR)
		  {
		  rc = dot1sStateMachineClassifier(prsBegin, p, i, L7_NULL, L7_NULLPTR);
		  }
		  
		  dot1sInstanceMap[i].inUse = L7_FALSE;
		  dot1sInstanceMap[i].instNumber = 0;
		  dot1sCfg->cfg.dot1sInstanceMap[i].inUse = L7_FALSE;
		  dot1sCfg->cfg.dot1sInstanceMap[i].instNumber = 0;
		  bridge->instanceCount--;
		  rc = dot1sInstanceDefaultPopulate(dot1sInstFind(), i, instId);
		  break;
		}
      }

      /* Remove all references to this instance from the Vlan Map struct */
      for (i = 1; i <= L7_DOT1Q_MAX_VLAN_ID; i++)
      {
        if (dot1sInstVlanMap[i].instNumber == (L7_ushort16)instId)
        {
          /* accociate this vlan back to the CIST */
          dot1sInstVlanMap[i].instNumber = DOT1S_CIST_ID;
        }
      }
      rc = dot1sMstConfigIdCompute();

      /* Propagate this instance deletion to the rest of the system */
      rc = dot1sIhDoNotifyInstanceDelete(instId);

      
  }/*end if (bridge != L7_NULLPTR)*/

  return rc;
}
/*********************************************************************
* @purpose  Sets the port priotiy for this interface in this instance of spanning tree
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	portPri		@b{(input)} priority for this interface
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstancePortPriSet(L7_uint32 intIfNum, L7_uint32 instId, L7_uint32 portPri)
{
  L7_ushort16	shortPri;
  L7_ushort16   currentPortId;
  DOT1S_PORT_COMMON_t *p;
  L7_uint32		instIndex;
  L7_RC_t rc;

  rc = dot1sInstIndexFind(instId, &instIndex);
  if (rc == L7_FAILURE)
  {
	return L7_FAILURE;
  }

  /* shift to proper position in Port ID struct */
  shortPri = (L7_ushort16)(portPri << 8);

  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	currentPortId = p->portInstInfo[instIndex].portId;

	/* Clear the current priority but preserve the port number */
	currentPortId = (currentPortId & DOT1S_PORTNUM_MASK);

	currentPortId = currentPortId | shortPri;

	p->portInstInfo[instIndex].portId = currentPortId;
	p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.rxPortId = currentPortId;
	p->portInstInfo[instIndex].inst.msti.mstiMsgPriority.rxPortId = currentPortId;
	p->portInstInfo[instIndex].inst.msti.mstiPortPriority.rxPortId = currentPortId;

    /* Procedure 12.8.2.6.4 */
    /* The reselect parameter value for the MSTI for the port
     * is set TRUE, and the selected paramter for the MSTI for
     * the port is set FALSE.
     */
    p->portInstInfo[instIndex].reselect = L7_TRUE;
    p->portInstInfo[instIndex].selected = L7_FALSE;
	
    /* generate reselect event */
    rc = dot1sStateMachineClassifier(prsReselect, p, instIndex, L7_NULL, L7_NULLPTR);


	return L7_SUCCESS;
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the name for this switch used in the calculation of MST ID
*
* @param    name	@b{(input)} pointer to the name max size of 32 bytes
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchCfgNameSet(L7_uchar8	*name)
{
  DOT1S_BRIDGE_t *bridge;
  L7_RC_t rc;
  L7_uint32 instId, instIndex, intIfNum;
  DOT1S_PORT_COMMON_t *p;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
    memcpy((L7_char8 *)bridge->MstConfigId.configName, (L7_char8 *)name, DOT1S_MAX_CONFIG_NAME_SIZE);

    for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
    {
      if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
      {
        instId = dot1sInstanceMap[instIndex].instNumber;

        /* generate begin events for all the ports that are enabled for each msti 
         * and cist that is in use - pim, prt, pst, tcm
         */
        for (intIfNum = 1; intIfNum <= (L7_uint32)L7_MAX_INTERFACE_COUNT; intIfNum++)
        {
          p = dot1sIntfFind(intIfNum);
          if (p != L7_NULLPTR && p->portEnabled == L7_TRUE && p->portNum != 0)
          {
			rc = dot1sStateMachineClassifier(pstBegin, p, instIndex, L7_NULL, L7_NULLPTR);
            rc = dot1sStateMachineClassifier(pimBegin, p, instIndex, L7_NULL, L7_NULLPTR);
            rc = dot1sStateMachineClassifier(prtBegin, p, instIndex, L7_NULL, L7_NULLPTR);
            rc = dot1sStateMachineClassifier(tcmBegin, p, instIndex, L7_NULL, L7_NULLPTR);
			rc = dot1sStateMachineClassifier(pimPortEnabled, p, instIndex, L7_NULL, L7_NULLPTR);
          }
		  if(p != L7_NULLPTR) 
            rc = dot1sPortStateMachineUpTimeReset(p);
        }
        
		p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
	    if (p != L7_NULLPTR)
	    {
		  rc = dot1sStateMachineClassifier(prsBegin, p, instIndex, L7_NULL, L7_NULLPTR);
		}
      }
    }
  }/*end if (bridge != L7_NULLPTR)*/
  else 
  {
	return L7_FAILURE;
  }


  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Process an Apply Config request to add a vlan to an instance 
*           as configured in a saved configuration file.
*
* @param    instId		@b{(input)} instance id
* @param    vlanId		@b{(input)} vlaid to be associated
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sApplyCfgInstanceVlanAdd(L7_uint32 instId, L7_uint32 vlanId)
{
  if ((vlanId >= L7_DOT1S_MIN_VLAN_ID) && (vlanId <= L7_DOT1Q_MAX_VLAN_ID))
  {
    if (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask,vlanId))
    return(dot1sInstanceVlanAdd(instId, vlanId));
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Process a User request to add a vlan to a specified instance
*
* @param    instId		@b{(input)} instance id
* @param    vlanId		@b{(input)} vlan id to be associated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sUserInstanceVlanAdd(L7_uint32 instId, L7_uint32 vlanId)
{
  L7_RC_t rc;

  if ((vlanId >= L7_DOT1S_MIN_VLAN_ID) && (vlanId <= L7_DOT1Q_MAX_VLAN_ID))
  {
    rc = dot1sInstanceVlanAdd(instId, vlanId);

	/*request to regenerate the mstid config*/ 
	rc = dot1sMstConfigIdCompute();

    /* Keeping datachanged here since vlan callback also calls this function */
    dot1sCfg->hdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose 	Add this vlan to this instance of spanning tree 
*
* @param    instId		@b{(input)} instance id
* @param    vlanId		@b{(input)} vlanid to be associated
*
* @returns  L7_SUCCESS
*
* @comments    this overwrites any previous vlan to instance mapping
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstanceVlanAdd(L7_uint32 instId, L7_uint32 vlanId)
{
  L7_RC_t rc;
  L7_uint32 oldInstId;
  L7_uint32 i, stpMode;
  DOT1S_PORT_COMMON_t *p;

  oldInstId = dot1sInstVlanMap[vlanId].instNumber;
    
  dot1sInstVlanMap[vlanId].instNumber = (L7_ushort16)instId;
  dot1sCfg->cfg.dot1sInstVlanMap[vlanId].instNumber = (L7_ushort16)instId;
  /* check for a move from one instance to another */
  if (oldInstId != instId)
  {
    /* Remove VLAN from the original ST instance from the device through DTL */
    rc = dtlDot1sInstVlanIdRemove(oldInstId, vlanId);
  }

  /* if the oldInstId is the cist and the new instance id is also cist, 
   * i.e. this is a new vlan being created, then there is no need for the 
   * dtl call as during the vlan creation the driver automatically assigns the cist
   * as the default instance for this vlan
   */
  if (oldInstId != DOT1S_CIST_ID || instId != DOT1S_CIST_ID)
  {
    /* Add VLAN to the new ST instance onto the device through DTL */
    rc = dtlDot1sInstVlanIdAdd(instId, vlanId);
  }

  /* check if STP for the device is disabled if yes then 
   * ensure that all the ports that are linked up for this instance and vlan 
   * are set to manual forwarding
   */
  {
    stpMode = dot1sModeGet();
    if (stpMode == L7_DISABLE)
    {
      /* MSTP disabled, set the Port State to manual Fowarding
       * for this instance
	   */
	  for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
	  {
	    p = dot1sIntfFind(i);
	    if (p != L7_NULLPTR && p->portNum != 0 && p->portLinkState == L7_UP)
	    {
            if (dot1sMstiPortStateGet(instId,i) != L7_DOT1S_MANUAL_FWD)
                dot1sIhSetPortState(instId, p->portNum, L7_DOT1S_MANUAL_FWD);
	    }
	  }
    }
    /* if STP is enabled for this box then only set the ports that 
     * are stpdisabled (port disabled) to manual forwarding
     */
    else
    {
  	  for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
	  {
        p = dot1sIntfFind(i);
	    if (p != L7_NULLPTR && p->portNum != 0 && p->portLinkState == L7_UP
		    && p->portAdminMode == L7_DISABLE)
	    {
            if (dot1sMstiPortStateGet(instId,p->portNum) != L7_DOT1S_MANUAL_FWD)
                dot1sIhSetPortState(instId, p->portNum, L7_DOT1S_MANUAL_FWD);
	    }
	  }
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Removes the vlan from this instance of spanning tree
*
* @param    instId		@b{(input)} instance id
* @param    vlanId		@b{(input)} vlan id to be associated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments sets the vlan mstid mapping to zero
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstanceVlanRemove(L7_uint32 instId, L7_uint32 vlanId)
{
  L7_RC_t rc;
  L7_uint32 oldInstId;

  /*this overwrites any previous instance mapping*/
  if (vlanId != 0 && vlanId <= L7_MAX_VLAN_ID)
  {
    oldInstId = dot1sInstVlanMap[vlanId].instNumber;

    /* Check if the correct instance is removing the VLAN */
    if (oldInstId == instId)
    {
      dot1sInstVlanMap[vlanId].instNumber = DOT1S_CIST_ID;
      dot1sCfg->cfg.dot1sInstVlanMap[vlanId].instNumber = DOT1S_CIST_ID;
	  /*request to regenerate the mstid config*/ 
	  rc = dot1sMstConfigIdCompute();

      /* Remove VLAN from the specified ST instance at the DTL */
      rc = dtlDot1sInstVlanIdRemove(instId, vlanId);

      /* Check if the VLAN itself has been deleted */
      if (dot1qVlanCheckValid(vlanId) == L7_SUCCESS &&
		  instId != DOT1S_CIST_ID)
      {
        /* If VLAN still exists add it to the CIST */
        rc = dtlDot1sInstVlanIdAdd(DOT1S_CIST_ID, vlanId);
      }

      /* Keeping datachanged here since vlan callback also calls this function */
      dot1sCfg->hdr.dataChanged = L7_TRUE;
      return rc;
    }
  }/*end if (vlanId != 0 && vlanId <= L7_MAX_VLAN_ID)*/

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Deletes the vlan from this instance of spanning tree
*
* @param    instId		@b{(input)} instance id
* @param    vlanId		@b{(input)} vlan id to be associated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments sets the vlan mstid mapping to zero
*           this routine is different from the remove as the remove is
*           initiated from the user who is looking to remove the vlan mapping
*			from a particular instance to the CIST. In this case the vlan 
*			is being deleted so we just update the list of vlans associated 
*			with this particular instance
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstanceVlanDelete(L7_uint32 instId, L7_uint32 vlanId)
{
  L7_RC_t rc;
  L7_uint32 oldInstId;

  /*this overwrites any previous instance mapping*/
  if (vlanId != 0 && vlanId <= L7_MAX_VLAN_ID)
  {
    oldInstId = dot1sInstVlanMap[vlanId].instNumber;

    /* Check if the correct instance is removing the VLAN */
    if (oldInstId == instId)
    {
      dot1sInstVlanMap[vlanId].instNumber = DOT1S_CIST_ID;
      dot1sCfg->cfg.dot1sInstVlanMap[vlanId].instNumber = DOT1S_CIST_ID;
	  /*request to regenerate the mstid config*/ 
	  rc = dot1sMstConfigIdCompute();

      /* At the DTL, remove VLAN from any instance it is 
      associated with */
      rc = dtlDot1sInstVlanIdRemove(instId, vlanId);

      return rc;
    }/*end if (oldInstId == instId)*/
  }/*end if (vlanId != 0 && vlanId <= L7_MAX_VLAN_ID)*/

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Stes the path cost for this interface in this instance
*
* @param    intIfNum	@b{(input)} internal interface number
* @param    instId		@b{(input)} instance id
* @param	pathCost	@b{(input)} cost for this interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstancePortPathCostSet(L7_uint32 intIfNum, L7_uint32 instId, L7_uint32 pathCost)
{
  DOT1S_PORT_COMMON_t *p;
  L7_uint32		instIndex;
  L7_RC_t rc;

  rc = dot1sInstIndexFind(instId, &instIndex);
  if (rc == L7_FAILURE)
  {
	return L7_FAILURE;
  }
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	p->portInstInfo[instIndex].InternalPortPathCost = pathCost;

    /* Update the Auto Port Path Cost setting */
    if (pathCost == L7_DOT1S_AUTO_PORT_PATHCOST)
    {
      p->portInstInfo[instIndex].autoInternalPortPathCost = L7_TRUE;

      /* Enabled ports need the Auto calculation completed now */
      if (dot1sPort[intIfNum].portEnabled == L7_TRUE)
      {
        /* Calculate the cost for all instances.
         * It is not really necessary to re-calculate the Path Cost 
         * for all instances at this time, only the current instance,
         * but doing so will not hurt anything.
         */
        rc = dot1sCalcPortPathCost(intIfNum);
      }
    }
    else
    {
      p->portInstInfo[instIndex].autoInternalPortPathCost = L7_FALSE;
    }
     
    /* Procedure 12.8.2.6.4 */
    /* The reselect parameter value for the MSTI for the port
     * is set TRUE, and the selected paramter for the MSTI for
     * the port is set FALSE.
     */
    p->portInstInfo[instIndex].reselect = L7_TRUE;
    p->portInstInfo[instIndex].selected = L7_FALSE;

    /* generate reselect event */
    rc = dot1sStateMachineClassifier(prsReselect, p, instIndex, L7_NULL, L7_NULLPTR);

	return L7_SUCCESS;
  }/*end if (p != L7_NULLPTR && p->portNum != 0)*/
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the Priority for this instance  
*
* @param    instId	@b{(input)} Instance ID 
* @param    pri		@b{(input)} priority for this instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments just the four MS bits the rest 12 bits are ignored 
*       
* @end
*********************************************************************/
L7_RC_t dot1sInstancePriSet(L7_uint32 instId, L7_uint32 pri)
{
  L7_uint32 instIndex;
  L7_ushort16 shortPri;
  L7_ushort16 shortInstId;
  L7_RC_t rc;
  DOT1S_PORT_COMMON_t *p;

  rc = dot1sInstIndexFind(instId, &instIndex);
  if (rc == L7_FAILURE)
  {
	return L7_FAILURE;
  }
  
  shortPri = (L7_ushort16)pri;

  shortInstId = (L7_ushort16)instId;
  shortPri = shortInstId | shortPri;

  dot1sInstance->msti[instIndex].BridgeIdentifier.priInstId = shortPri; 
  dot1sInstance->msti[instIndex].MstiBridgePriority.regRootId.priInstId = shortPri;
  dot1sInstance->msti[instIndex].MstiBridgePriority.dsgBridgeId.priInstId = shortPri;

  /* For all ports of the Bridge, the reselect for the CIST parameter 
   * is set TRUE, and the selected paramter for the CIST is set FALSE.
   */
  rc = dot1sReselectTreeSet(instIndex);
  rc = dot1sSelectedTreeClear(instIndex);

  /* generate reselect event since the tree has been set
   * use a valid port number for this event to be processed
   */
  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
  if (p != L7_NULLPTR)
  {
    rc = dot1sStateMachineClassifier(prsReselect, p, instIndex, L7_NULL, L7_NULLPTR);
  }
  

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This is the routine which receives the BPDU and passes it to the classifier
*			with the relevant event for the prx state machine. 
*
* @param   	intIfNum 	@b{(input)} Interface that this BPDU was received on
* @param    bufHandle	@b{(input)} buffer handle to the BPDU
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments It also queues up a event for the bdm state machine for bdmbpduRcvd
*           This routine is responsible for freing up the buffer pool before 
*			any possible return path.
*       
* @end
*********************************************************************/
L7_RC_t dot1sStateMachineRxBpdu(L7_uint32 intIfNum, DOT1S_MSTP_ENCAPS_t 	*bpdu)
{
  DOT1S_PORT_COMMON_t *p;
  L7_BOOL rcvdAnyMsg = L7_FALSE;
  L7_uint32 i;
  L7_RC_t rc;
  
  do
  {
  p = dot1sIntfFind(intIfNum);
    if (p == L7_NULLPTR || p->portNum == 0)
    {
      dot1sPortStats[intIfNum].rxDiscards++;
      LOG_MSG("dot1sStateMachineRxBpdu(): Rcvd BPDU Discarded on unknown interface\n");
      rc = L7_FAILURE;
      break;
    }

    if (p->bpduGuardEffect == L7_TRUE)
    {
      rc = L7_SUCCESS;
      break;
    }

	if (p->portInstInfo[DOT1S_CIST_INDEX].rcvdMsg == L7_TRUE)
	{
	  rcvdAnyMsg = L7_TRUE;
	}
	else
	{
	  /*check in the other mstis*/
	  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
	  {
		if (dot1sInstanceMap[i].inUse == L7_TRUE)
		{
		  if (p->portInstInfo[i].rcvdMsg == L7_TRUE)
		  {
			rcvdAnyMsg = L7_TRUE;
			break;
		  }
		}
	  }
	}
	/*Set rcvdBpdu to L7_TRUE*/
	p->rcvdBpdu = L7_TRUE;

	if (p->portEnabled == L7_FALSE)
	{
	  /*Send Event E41*/
	  rc = dot1sStateMachineClassifier(prxRcvdBpduPortDisabled, p, L7_NULL, bpdu, L7_NULLPTR);
	}
	else if (rcvdAnyMsg == L7_FALSE)/*port is enabled*/
	{
	  /*Send Event E42*/
	  rc = dot1sStateMachineClassifier(prxRcvdBpduPortEnabledNotRcvdAnyMsg, p, L7_NULL, bpdu, L7_NULLPTR);
	}
	else
	{
	  dot1sPortStats[intIfNum].rxDiscards++;
	  L7_LOG(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
          "dot1sStateMachineRxBpdu(): Rcvd BPDU Discarded."
          " The current conditions, like port is not enabled"
          " or we are currently not finished processing another"
          " BPDU on the same interface, does not allow us to process this BPDU.");
	  rc = L7_SUCCESS;

	}
	
  
  }while (0);

  bufferPoolFree(dot1sBPDUBufferPoolId, (L7_uchar8 *)bpdu);
  return rc;
}
/*********************************************************************
* @purpose  Callback function to process VLAN changes.
*
* @param    vlanId      @b{(input)} VLAN ID
* @param    intIfnum    @b{(input)} internal interface whose state has changed
* @param    event       @b{(input)} VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event)
{
  L7_uint32 instId;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i = 0,vlanId = 0, numVlans = 0;
  
  if (dot1sDeregister.dot1sVlanChangeCallback == L7_TRUE)
  {
	LOG_MSG("dot1sVlanChangeCallback is deregistered\n");
	return L7_FAILURE;
  }
  if (!(DOT1S_IS_READY))
  {
	LOG_MSG("Rcvd a vlan callback in incorrect state %d vlan %d event %d int %d\n",
			dot1sCnfgrState, vlanId, event, intIfNum);
    return L7_FAILURE;
  }

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
      case VLAN_ADD_NOTIFY:
        /* A VLAN has been added to the system, it now needs to be added 
         * to the "Instance Vlan Map" for the instance that it has 
         * previously been configured for or for the CIST by default. It may 
         * result in this bridge becoming part of a different region.
         */
        DOT1S_VLAN_SETMASKBIT(dot1sVlanMask, vlanId);
        instId = dot1sCfg->cfg.dot1sInstVlanMap[vlanId].instNumber;
        if (dot1sInstCheckInUse(instId) == L7_SUCCESS)
        {
          (void) osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);
          rc = dot1sUserInstanceVlanAdd(instId, vlanId);
          (void) osapiSemaGive(dot1sTaskSyncSema);
        }
        else
        {
          rc = L7_SUCCESS;
        }
        break;
    
      case VLAN_DELETE_PENDING_NOTIFY:
        /* A VLAN has been deleted, it needs to be removed from the MSTP
         * configuration and associated with the CIST. This may result 
         * in this bridge becoming part of a different region.
         */
        instId = dot1sVlanToMstiGet(vlanId);
        if ((instId != DOT1S_CIST_INDEX) && (dot1sInstCheckInUse(instId) == L7_SUCCESS))
        {
          /* Cannot put this remove vlan request on the dot1s queue
           * as the driver needs to have a valid vlan to remove
           * thats the reason we respond to the notification VLAN_DELETE_PENDING_NOTIFY
           * and not VLAN_DELETE_NOTIFY.
           * Therefore we must take the same semaphore that the dot1s queues
           * use to provide mutual exclusion
           */
          rc = osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);
          rc = dot1sInstanceVlanDelete(instId, vlanId);
          rc = osapiSemaGive(dot1sTaskSyncSema);
        }
        else
        {
            rc = L7_SUCCESS;
        }
        break;
      case VLAN_DELETE_NOTIFY:
        DOT1S_VLAN_CLRMASKBIT(dot1sVlanMask, vlanId);
        rc = L7_SUCCESS;
        break;
      case VLAN_ADD_PORT_NOTIFY:
      case VLAN_DELETE_PORT_NOTIFY:
      case VLAN_INITIALIZED_NOTIFY:
      case VLAN_START_TAGGING_PORT_NOTIFY:
      case VLAN_STOP_TAGGING_PORT_NOTIFY:
      case VLAN_RESTORE_NOTIFY:
        rc = L7_SUCCESS;
        break;
    
      default:
        break;
      }
    numVlans++;
  }
  return rc;
}
/*********************************************************************
* @purpose  Makes the inputed DOT1S_BRIDGEID_t into a endian safe
*           string of chars
*          
* @param    *bridgeID @b{(input)} Pointer to the identifier 
* @param    *buf      @b{(output)} Pointer to the buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Caller is responsible for allocation of the buffer. 
*           This routine does NOT check for boundaries.
*
* @end
*********************************************************************/
void dot1sBridgeIdEndianSafe(DOT1S_BRIDGEID_t *bridgeID, L7_uchar8 *buf)
{
  L7_ushort16 priInstId = 0;

  priInstId = osapiHtons(bridgeID->priInstId);

  memcpy((char *)buf, (char *)&priInstId, (L7_int32)sizeof(L7_ushort16));
  memcpy((char *)&buf[2], (char *)bridgeID->macAddr, L7_MAC_ADDR_LEN);

  return;
}

/*********************************************************************
* @purpose  Sets the ports admin hello time
*          
* @param    instId   @b{(input)} InstanceID
* @param    intIfNum @b{(output)} Internal Interface Number
* @param	helloTime @b{(input)} New hello time value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sPortAdminHelloTimeSet(L7_uint32 instId, 
								   L7_uint32 intIfNum, 
								   L7_uint32 helloTime)
{
  DOT1S_PORT_COMMON_t *p;
  L7_uint32 j;
  L7_RC_t rc;

  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	p->HelloTime = helloTime;
	/*Generate neccessary events for this port in all instances*/
	if (p->portAdminMode == L7_ENABLE)
	{
	  p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
	  p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;
	  for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
	  {
	    p->portInstInfo[j].reselect = L7_TRUE;
	    p->portInstInfo[j].selected = L7_FALSE;
	  }
	}
  }
  p = dot1sIntfFind(DOT1S_VALID_PORT_NUM);
  for (j = DOT1S_CIST_INDEX; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
	if (dot1sInstanceMap[j].inUse == L7_TRUE)
	{
	  if (p != L7_NULLPTR)
	  {
		rc = dot1sStateMachineClassifier(prsReselect, p, j, L7_NULL, L7_NULLPTR);
	  }
	}
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the ports cist external port path cost
*          
* @param    instId   @b{(input)} InstanceID
* @param    intIfNum @b{(output)} Internal Interface Number
* @param	helloTime @b{(input)} New external port path cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sPortExtPathCostSet(L7_uint32 instId, 
								L7_uint32 intIfNum, 
								L7_uint32 pathCost)
{
  L7_RC_t rc;
  DOT1S_PORT_COMMON_t *p;
  
  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR && p->portNum != 0)
  {
	p->portInstInfo[DOT1S_CIST_INDEX].ExternalPortPathCost = pathCost;
	
	/* Update the Auto Port Path Cost setting */
    if (pathCost == L7_DOT1S_AUTO_PORT_PATHCOST)
    {
      p->portInstInfo[DOT1S_CIST_INDEX].autoExternalPortPathCost = L7_TRUE;

      /* Enabled ports need the Auto calculation completed now */
      if (dot1sPort[intIfNum].portEnabled == L7_TRUE)
      {
        /* Calculate the cost for all instances.
         * It is not really necessary to re-calculate the Path Cost 
         * for all instances at this time, only the current instance,
         * but doing so will not hurt anything.
         */
        rc = dot1sCalcPortPathCost(intIfNum);
      }
    }
    else
    {
      p->portInstInfo[DOT1S_CIST_INDEX].autoExternalPortPathCost = L7_FALSE;
    }
     
    /* Procedure 12.8.2.5.4 */
    /* The reselect parameter value for the CIST for the port
     * is set TRUE, and the selected paramter for the CIST for
     * the port is set FALSE.
     */
    p->portInstInfo[DOT1S_CIST_INDEX].reselect = L7_TRUE;
    p->portInstInfo[DOT1S_CIST_INDEX].selected = L7_FALSE;

    /* generate reselect event */
    rc = dot1sStateMachineClassifier(prsReselect, p, DOT1S_CIST_INDEX, L7_NULL, L7_NULLPTR);


	return L7_SUCCESS;
  }/*end if (p != L7_NULLPTR && p->portNum != 0)*/
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the ports begin reset state machine time
*          
* @param    p @b{(output)} pointer to port
*
* @returns  L7_SUCCESS
*
* @comments	Begin is common to all instances so can
*
* @end
*********************************************************************/
L7_RC_t dot1sPortStateMachineUpTimeReset(DOT1S_PORT_COMMON_t *p)
{
  p->tick = osapiUpTimeRaw();
  return L7_SUCCESS;
}   

/**************************************************************************
* @purpose  Create the queue sync semaphore.
*
* @comments The dot1s Queue sync semaphore allows other queues to synchronize
*           with the dot1sQueue by putting a message on the dot1s queue and 
*           blocking on the dot1sQueueSync semaphore 
*
* @end
*************************************************************************/
void dot1sQueueSyncSemCreate (void)
{

   dot1sQueueSyncSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
   if (dot1sQueueSyncSem == L7_NULL) 
   {
	   LOG_ERROR (0);
   }
}

/**************************************************************************
* @purpose  Get the queue sync semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dot1sQueueSyncSemGet (void)
{
  L7_RC_t rc;

   rc = osapiSemaTake (dot1sQueueSyncSem, L7_WAIT_FOREVER);
   if (rc != L7_SUCCESS) 
   {
	   LOG_ERROR (rc);
   }
}

/**************************************************************************
* @purpose  Get the queue sync semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dot1sQueueSyncSemFree (void)
{
	L7_RC_t rc;

	rc = osapiSemaGive (dot1sQueueSyncSem);
	if (rc != L7_SUCCESS) 
	{
		LOG_ERROR (rc);
	}
}

/**************************************************************************
* @purpose  Get the queue sync semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dot1sQueueSyncSemDelete (void)
{
	L7_RC_t rc;

	rc = osapiSemaDelete (dot1sQueueSyncSem);
	if (rc != L7_SUCCESS) 
	{
		LOG_ERROR (rc);
	}
}

/*********************************************************************
* @purpose  Sets the BPDU Guard mode for the bridge
*
* @param    val      @b{(input)} the new value of the STP BPDU Guard
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchBpduGuardModeSet(L7_BOOL val)
{
  DOT1S_BRIDGE_t *bridge;
  DOT1S_PORT_COMMON_t *p;
  L7_uint32 i;
  L7_RC_t   rc = L7_FAILURE;

  bridge = dot1sBridgeParmsFind();
  if (bridge != L7_NULLPTR)
  {
    rc = L7_SUCCESS;
    if (bridge->bpduGuard != val)
    {
      bridge->bpduGuard = val;
      if (val == L7_FALSE)
      {
        /* If the BPDU Guard is being disabled, check for the guard effect for
           each port. If the guard effect is true, enable the port and set the 
           guard effect to false. We don't need to do anything if the guard
           effect is already false.*/
        for (i=0; i <= L7_MAX_INTERFACE_COUNT; i++)
        {
          p = dot1sIntfFind(i);

          if ((p != L7_NULLPTR) && 
             (p->bpduGuardEffect == L7_TRUE))
          {
            dot1sResetBpduGuardEffect(p);
          }
        } /* For */
      }
    } /* val is different*/
  }
  return rc;
}

/*********************************************************************
* @purpose  Sets the BPDU Filter mode for the bridge
*
* @param    val      @b{(input)} the new value of the STP BPDU Filter
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sSwitchBpduFilterModeSet(L7_BOOL val)
{
  L7_RC_t   rc = L7_FAILURE;
  DOT1S_BRIDGE_t          *bridge;
  DOT1S_PORT_COMMON_t     *pPort = L7_NULLPTR;
  L7_uint32 i;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
    if (bridge->bpduFilter != val)
    {
      bridge->bpduFilter = val;
      for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
      {
        pPort = dot1sIntfFind(i);
        if (pPort != L7_NULLPTR && pPort->adminEdge == L7_TRUE)
        {
          if (pPort->bpduFilter != L7_TRUE)
          {
            /* set the filter mode in the driver */
            if (dtlDot1sBpduFilterSet(i,val) != L7_SUCCESS)
            {
              LOG_MSG("Failed to set BPDU Filter value %d for %d\n", val,i);
              return L7_FAILURE;
            }
          }
        }
      }
      rc = L7_SUCCESS;
    }
  }
  return rc;
}


/*********************************************************************
* @purpose  Sets the BPDU Filter mode for the port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    instId	  @b{(input)} instance id
* @param    val       @b{(input)} the new value of the STP BPDU Filter
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortBpduFilterModeSet(L7_uint32 intIfNum, L7_uint32 instId,L7_BOOL val)
{
  DOT1S_PORT_COMMON_t     *pPort = L7_NULLPTR;
  DOT1S_BRIDGE_t          *bridge;

  pPort = dot1sIntfFind(intIfNum); 
  bridge = dot1sBridgeParmsFind();

  if (pPort != L7_NULLPTR)
  {
    /* Set the value when it is different from the old one. */
    if (pPort->bpduFilter != val)
    {
      if (!(pPort->adminEdge == L7_TRUE && bridge->bpduFilter == L7_TRUE))
      {
        /* set the filter mode in the driver */
        if (dtlDot1sBpduFilterSet(intIfNum,val) != L7_SUCCESS)
        {
          LOG_MSG("Failed to set BPDU Filter value %d for %d\n", val,intIfNum);
          return L7_FAILURE;
        }
      }
      pPort->bpduFilter = val;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the BPDU Flood mode for the port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    instId	  @b{(input)} instance id
* @param    val       @b{(input)} the new value of the STP BPDU Flood
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t dot1sCommonPortBpduFloodModeSet(L7_uint32 intIfNum, L7_uint32 instId,L7_BOOL val)
{
  DOT1S_PORT_COMMON_t     *pPort = L7_NULLPTR;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 curr = 0,next;
  L7_BOOL done = L7_FALSE;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    if (nimFirstValidIntfNumber(&curr) != L7_SUCCESS)
    {
        LOG_MSG("Cannot get first valid NIM Interface number \n");
        return L7_FAILURE;
    }
  }
  else
  {
    curr = intIfNum;
    done = L7_TRUE;
  }

  do
  {
    pPort = dot1sIntfFind(curr); 
    if (pPort != L7_NULLPTR)
    {
      L7_BOOL             acquired = L7_FALSE;

      /* check if previously acquired */
      DOT1S_ACQ_NONZEROMASK(dot1sPort[curr].acquiredList, acquired);
      if (acquired == L7_TRUE && val == L7_TRUE)
      {
        rc = L7_SUCCESS;   
      }
      else
      {
        /* Set the value when it is different from the old one. */
        if (pPort->bpduFlood != val)
        {
          pPort->bpduFlood = val;
          /* set the flood mode in the driver */
          if (dtlDot1sBpduFloodSet(curr,val) != L7_SUCCESS)
          {
            LOG_MSG("Failed to set BPDU Flood value %d for %d\n", val,curr);
            rc = L7_FAILURE;
          }
        }
      } /* else acquired*/
    }/* pPort != NULL*/

	/* Get the next valid interface if we need to */
    if (done == L7_FALSE)
    {
      rc = nimNextValidIntfNumber(curr,&next);
      if ( rc != L7_SUCCESS)
      {
          done = L7_TRUE;
  
      }
      else
          curr = next;
    }
  } while(done == L7_FALSE);
  
  return rc;
}


