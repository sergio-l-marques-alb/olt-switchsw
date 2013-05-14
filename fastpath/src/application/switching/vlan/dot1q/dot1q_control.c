/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename dot1q_control.c
*
* @purpose   Vlan Control file
*
* @component dot1q
*
* @comments
*
* @create    06/22/2005
*
* @author    akulkarni
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "nimapi.h"
#include "dtlapi.h"
#include "dot1q_api.h"
#include "log.h"
#include "datatypes.h"
#include "trapapi.h"
#include "dot1q_debug.h"
#include "dot1q.h"
#include "dot1q_data.h"
#include "dot1q_cnfgr.h"
#include "dot1q_qports.h"
#include "dot1q_cfg.h"
#include "dot1q_intf.h"
#include "dot1q_control.h"
#include "dot1q_outcalls.h"
#include "usmdb_util_api.h"
#ifdef L7_ROUTING_PACKAGE
#include "l7_ip_api.h"
#endif /* L7_ROUTING_PACKAGE */

#if defined(FEAT_METRO_CPE_V1_0)
#include "dot1ad_api.h"
#include "dot1ad_l2tp_api.h"
#include "simapi.h"
#endif

extern  dot1qInfo_t         *dot1qInfo;
extern void *dot1qMsgQueue;
extern  dot1qIntfData_t      *dot1qQports;
extern  osapiRWLock_t       dot1qCfgRWLock;
extern  dot1qCfgData_t      *dot1qCfg;
extern  L7_VLAN_MASK_t      dot1qIntfVidList[L7_MAX_INTERFACE_COUNT +1];

vlanTree_t          *pVlanTree    = L7_NULLPTR;
void * dot1qSem = L7_NULLPTR;
static L7_ushort16 port_list[L7_MAX_INTERFACE_COUNT+1]; /* list of intfs to become members */
static L7_ushort16 tag_list[L7_MAX_INTERFACE_COUNT+1]; /* list of intfs to be tagged */
static L7_uint32 numPorts = 0;
static L7_uint32 numTagPorts = 0;
/* required only for mask processing */
static L7_ushort16 del_port_list[L7_MAX_INTERFACE_COUNT+1]; /* list of intfs to become members */
static L7_uint32 numDelPorts = 0;

L7_uint32 dot1qEventsList[DOT1Q_MAX_EVENTS];
dot1qVlanPendingCreate_t   dot1qVlanPendingCreate[DOT1Q_MAX_PENDING_CREATES+1];
dot1qInternalVlanInfo_t  *dot1qInternalVlanInfo;

/* dot1q queue sync semaphore*/
static void * dot1qQueueSyncSem = 0;
L7_VLAN_MASK_t dot1qVlanCreateMaskFailure;
L7_uint32 dot1qVlanCreateMaskFailureCount;


static L7_BOOL dot1qIsRequestorDynamic(L7_uint32 requestor)
{
  return ((requestor == DOT1Q_GVRP) ||
          (requestor == DOT1Q_WS_L2TUNNEL) ||
          (requestor == DOT1Q_IP_INTERNAL_VLAN) ||
          (requestor == DOT1Q_VOICE_VLAN));
}

static L7_BOOL dot1qIsVlanDynamic(dot1q_vlan_dataEntry_t *pDataEntry)
{
  return (pDataEntry->current.status == L7_DOT1Q_TYPE_GVRP_REGISTERED ||
        pDataEntry->current.status == L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED ||
        pDataEntry->current.status == L7_DOT1Q_TYPE_DOT1X_REGISTERED||
        pDataEntry->current.status == L7_DOT1Q_TYPE_IP_INTERNAL_VLAN);

}

/*********************************************************************
*
* @purpose  Complete the vlan Participation process.
*
* @param
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes
*       For a list of ports call the driver:
*       1. If the dtl call is succesful notify vlan interface
*       2. notify the registered users
*
*
* @end
*********************************************************************/
static L7_RC_t dot1qMemberSetPostProcess(L7_uint32 vlanId,L7_ushort16 *port_list,
                                         L7_uint32 numPorts,
                                         L7_uint32 mode, L7_ushort16 *tag_list, L7_uint32 numTagIntf)
{
    L7_uint32 i;
    L7_uint32 notifyMode = 0;
    L7_RC_t rc =L7_SUCCESS;
    L7_uchar8   traceBuf[L7_INTF_MASK_STRING_LENGTH];
    dot1qNotifyData_t vlanData;

    vlanData.numVlans = 1;
    vlanData.data.vlanId = vlanId;

    if (numPorts > 0)
    {
        if (mode == L7_DOT1Q_FIXED)
        {
            /* Set the tag in the driver only for add port.
             * for delete port driver will take care of the tagged set
             */

            /* fdbId parm in dtl call is unused */
            rc = dtlDot1qAddPort(0, (L7_short16)vlanId, port_list, numPorts,tag_list,
                                 numTagIntf,L7_NULLPTR,0);

            if (rc != L7_SUCCESS)
            {
                dot1qDebugListToMaskString(port_list,numPorts, traceBuf);

                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                        "dtl failure when adding ports to vlan id %d - portMask = %s",
                         vlanId, traceBuf);
            }

            notifyMode =  VLAN_ADD_PORT_NOTIFY ;
        }
        else if ((mode == L7_DOT1Q_NORMAL_REGISTRATION) ||
                 (mode == L7_DOT1Q_FORBIDDEN))
        {
            /* fdbId parm in dtl call is unused */
            rc = dtlDot1qDeletePort(0, (L7_short16)vlanId,port_list, numPorts,
                                    L7_NULLPTR,0,L7_NULLPTR,0);

            if (rc != L7_SUCCESS)
            {
                dot1qDebugListToMaskString(port_list,numPorts, traceBuf);

                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                        "dtl failure when deleting ports from vlan id %d - portMask = %s",
                         vlanId, traceBuf);
            }

            notifyMode = VLAN_DELETE_PORT_NOTIFY;

        }

        if (rc != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Cannot update driver for interface  vlan id %d", vlanId);
            return L7_FAILURE;
        }
        else
        {
            for (i=0; i<numPorts; i++)
            {
                vlanNotifyRegisteredUsers(&vlanData, port_list[i], notifyMode);
            }
        }
    }/* if numPort > 0 */

    return rc;
}



/*********************************************************************
*
* @purpose  Complete the tagging process.
*
* @param    void
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes
*       For a list of ports call the driver:
*       1. If the dtl call is succesful notify the registered users
*
*
* @end
*********************************************************************/
static L7_RC_t dot1qTaggingPostProcess(L7_uint32 vlanId,L7_ushort16 *port_list,
                                       L7_uint32 numPorts,L7_uint32 action)
{
    L7_uint32 i;
    L7_uint32 notifyMode = 0;
    L7_RC_t rc =L7_SUCCESS;
    L7_uchar8   traceBuf[L7_INTF_MASK_STRING_LENGTH];
    dot1qNotifyData_t vlanData;

    vlanData.numVlans = 1;
    vlanData.data.vlanId = vlanId;

    if (numPorts > 0)
    {

        if (action == L7_DOT1Q_TAGGED)
        {
            /* fdbId parm in dtl call is unused */
            rc = dtlDot1qAddPort(0, (L7_short16)vlanId, L7_NULLPTR, 0,
                                 port_list,numPorts,L7_NULLPTR,0);

            if (rc != L7_SUCCESS)
            {
                dot1qDebugListToMaskString(port_list,numPorts, traceBuf);

                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                        "dtl failure when adding ports to tagged list for vlan id %d - portMask = %s", vlanId, traceBuf);
            }

            notifyMode = VLAN_START_TAGGING_PORT_NOTIFY;

        }
        else if (action == L7_DOT1Q_UNTAGGED)
        {
            /*Just to be safe checking for the condition explicitly */
            /* fdbId parm in dtl call is unused */
            rc = dtlDot1qDeletePort(0, (L7_short16)vlanId,L7_NULLPTR, 0,port_list,
                                    numPorts,L7_NULLPTR,0);

            if (rc != L7_SUCCESS)
            {
                dot1qDebugListToMaskString(port_list,numPorts, traceBuf);

                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                        "dtl failure when deleting ports from tagged list for vlan id %d - portMask = %s", vlanId, traceBuf);
            }

            notifyMode = VLAN_STOP_TAGGING_PORT_NOTIFY;
        }


        if (rc != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Cannot update driver for interface  vlan id %d", vlanId);
            return L7_FAILURE;
        }
        else
        {
            for (i=0; i<numPorts; i++)
            {
                vlanNotifyRegisteredUsers(&vlanData, port_list[i], notifyMode);
            }
        }

    } /* if numPort > 0 */
    return rc;

}

/*********************************************************************
*
* @purpose  task to handle dot1q processing
*
* @param    void
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes
*
*
* @end
*********************************************************************/
void dot1qTask(void)
{
    DOT1Q_MSG_t msg;
    L7_RC_t rc;

    (void) osapiTaskInitDone(L7_DOT1Q_TASK_SYNC);

    /* Note: Trace is done in dot1qDispatch */
    for (;;)
    {
        rc = osapiMessageReceive(dot1qMsgQueue,(void *)&msg,sizeof(DOT1Q_MSG_t),L7_WAIT_FOREVER);

        if (rc == L7_SUCCESS)
        {
            (void) dot1qDispatch(&msg);
        }
        else
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                    "dot1qTask: unsuccessful return code on receive from dot1qMsgQueue: %08x", rc);
        }
    }
}



/*********************************************************************
*
* @purpose  Place a command on the dot1q message queue
*
* @param    *msg    pointer to DOT1Q_MSG_t
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qIssueCmd(DOT1Q_MSG_t *msg )
{
    L7_RC_t rc;
    L7_uint32 num = 0;

    dot1qTraceQueueMsgSend(msg);

    dot1qEventsList[msg->event]++;

    /* Messages placed on the dot1q queue by the GARP task have the potential of
       overruning the message queue. This is because of the small timeouts which is
       an inherent part of the GARP protocol can cause a lot of churn.  As such,
       do not use the L7_NO_WAIT option when submitting messages to the dot1q queue
       to avoid dropping messages.

       This solution has the potential of causing the management of dot1q to be slowed
       during periods of heavy GARP activity and/or low GARP timer settings.  If this
       occurs, an alternative may be implemented to throttle messages from GARP task.

     */

    rc = osapiMessageSend(dot1qMsgQueue, msg,sizeof(DOT1Q_MSG_t),L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);

    if (rc != L7_SUCCESS)
    {
        (void)osapiMsgQueueGetNumMsgs(dot1qMsgQueue, &num);
        sysapiPrintf("Unable to send message %d to dot1qMsgQueue for vlan %d - %d msgs in queue\n",
               msg->event, msg->vlanId, num );
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                 "Unable to send message %d to dot1qMsgQueue for vlan %d - %d msgs in queue",
                  msg->event, msg->vlanId, num);
        dot1qDebugMsgQueue(1);
        osapiSleep(1);
        LOG_ERROR(msg->event);

    }

    return rc;

}


/*********************************************************************
*
* @purpose  Process a command received on the dot1q message queue
*
* @param    *msg    pointer to DOT1Q_MSG_t
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qDispatch(DOT1Q_MSG_t *msg)
{
    dot1qTraceQueueMsgRecv(msg);

    numPorts = 0;
    memset(port_list,0x0,sizeof(port_list));
    switch (msg->event)
    {
    case dot1q_vlan_create_static:
        dot1qVlanCreateProcess(DOT1Q_DEFAULT_FDB_ID, msg->vlanId, DOT1Q_ADMIN);
        break;
    case dot1q_vlan_delete_static:
        dot1qVlanDeleteProcess(msg->vlanId,DOT1Q_ADMIN);
        break;
    case dot1q_vlan_create_dynamic:
        dot1qVlanCreateProcess(DOT1Q_DEFAULT_FDB_ID, msg->vlanId, msg->requestor);
        break;
    case dot1q_vlan_delete_dynamic:
        dot1qVlanDeleteProcess(msg->vlanId, msg->requestor);
        break;
    case dot1q_vlan_tagged_set:
      dot1qVlanTaggedMemberSetModify(msg->vlanId, msg->data.intIfNum, msg->mode, msg->requestor);
        break;
    case dot1q_vlan_tagged_set_delta_mask:
        dot1qVlanTSMaskModify(msg->vlanId, &(msg->data.intfMask) , msg->mode);
        break;
    case dot1q_vlan_member_set:
        dot1qVlanMemberSetModify(msg->vlanId, msg->data.intIfNum , msg->mode, msg->requestor, msg->swport_mode);
        break;
    case dot1q_vlan_member_set_delta_mask:
        dot1qVlanMSMaskModify(msg->vlanId, &(msg->data.intfMask) , msg->mode, msg->swport_mode);
        break;
#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
    case dot1q_vlan_group_filter_static:
        dot1qVlanGroupFilterModify(msg->vlanId, msg->data.intIfNum , msg->mode, DOT1Q_ADMIN);
        break;
#endif
    case dot1q_vlan_tagged_set_static_mask:
        dot1qVlanTaggedSetMaskModify(msg->vlanId, msg->data.intfMask );
        break;
    case dot1q_vlan_member_set_static_mask:
        if (msg->mode == L7_DOT1Q_FORBIDDEN)
        {
           dot1qVlanMemberSetMaskForbidden(msg->vlanId, msg->data.intfMask);
        }
        else
        {
           dot1qVlanMemberSetMaskModify(msg->vlanId, msg->data.intfMask);
        }
        break;
    case dot1q_vlan_tagged_set_dynamic:
        /*dot1qVlanTaggedSetModify(msg->vlanId, msg->data.intIfNum , msg->mode, DOT1Q_GVRP);*/
        break;
#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
    case dot1q_vlan_group_filter_dynamic:
        dot1qVlanGroupFilterModify(msg->vlanId, msg->data.intIfNum , msg->mode, DOT1Q_GVRP);
        break;
#endif

    case dot1q_vlan_dynamic_to_static_set:
        dot1qDynamicToStaticVlanConvertProcess(msg->vlanId);
        break;
    case dot1q_qports_vid_set:
        dot1qQportsVIDProcess(msg->data.intIfNum, msg->vlanId,msg->mode,msg->requestor);
        break;
    case dot1q_qports_accept_frame_type_set:
        dot1qQportsAcceptFrameTypeProcess(msg->data.intIfNum,msg->mode);
        break;
    case dot1q_qports_default_user_prio_set:
        dot1dPortDefaultUserPriorityProcess(msg->data.intIfNum, msg->mode);
        break;
    case dot1q_qports_default_user_prio_global_set:
        dot1dPortDefaultUserPriorityGlobalProcess(msg->mode);
        break;
    case dot1q_qports_ingress_filter_set:
        dot1qQportsIngressFilterProcess(msg->data.intIfNum,msg->mode);
        break;
    case dot1q_qports_traffic_classes_enabled_set:
        dot1dTrafficClassesEnabledProcess(msg->mode);
        break;
    case dot1q_qports_traffic_class_set:
        dot1dPortTrafficClassProcess(msg->data.prio.intIfNum,msg->data.prio.priority,msg->data.prio.trafficClass);
        break;
    case dot1q_qports_num_traffic_classes_set:
        dot1dPortNumTrafficClassesProcess(msg->data.intIfNum,msg->mode);
        break;
    case dot1q_qports_num_traffic_classes_global_set:
        dot1dPortNumTrafficClassesGlobalProcess(msg->mode);
        break;
    case dot1q_qports_traffic_class_global_set:
        dot1dPortTrafficClassGlobalProcess(msg->data.prio.priority,msg->data.prio.trafficClass);
        break;
    case dot1q_clear:
        dot1qVlanCfgClearProcess();
        break;
    case dot1q_intfChange:
        dot1qIntfChangeProcess(msg->data.status.intIfNum,msg->data.status);
        break;
    case dot1q_nimStartup:
        if (msg->data.nimStartup.startupPhase == NIM_INTERFACE_CREATE_STARTUP)
        {
          dot1qNimCreateStartup();
        }
        else if (msg->data.nimStartup.startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
        {
          if (dot1qNimActivateStartup() == L7_SUCCESS)
          {
#ifdef L7_ROUTING_PACKAGE
            if (usmDbFeaturePresentCheck (1, L7_SIM_COMPONENT_ID,
                                          L7_SIM_NETWORK_PORT_FEATURE_ID)
                                       != L7_TRUE)
            {
              ipMapDefaultRoutingVlanCreate();
            }
#endif /* L7_ROUTING_PACKAGE */
          }
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "Unknown NIM startup event %d", msg->data.nimStartup.startupPhase);
        }
      break;        break;
    case dot1q_cnfgr:
        dot1qParseCnfgr(&(msg->data.CmdData));
        break;
    case dot1q_queueSynchronize:
         dot1qQueueSyncSemFree();
         break;
    case dot1q_qports_vlan_tagging_delta_mask_set:
        dot1qQportsVlanTagMaskProcess(msg->data.intfVlanMask.intIfNum, &msg->data.intfVlanMask.vlanMask, msg->mode);
        break;
    case dot1q_qports_vlan_membership_delta_mask_set:
        dot1qQportsVlanMembershipMaskProcess(msg->data.intfVlanMask.intIfNum,
                                             &msg->data.intfVlanMask.vlanMask,
                                             msg->mode, msg->swport_mode, DOT1Q_ADMIN);
        break;
    case dot1q_switchport_mode_set:
        dot1qSwitchPortModeProcess(&(msg->data.intfMask),msg->swport_mode, L7_TRUE);
        break;
    case dot1q_switchport_access_vlan_set:
        dot1qSwitchPortAccessModeSet(msg->data.intIfNum,msg->vlanId, msg->requestor);
        break;
    case dot1q_vlan_create_static_mask:
        dot1qVlanCreateMaskProcess(&msg->data.intfVlanMask.vlanMask);
        break;
    case dot1q_vlan_delete_static_mask:
        dot1qVlanDeleteMaskProcess(&msg->data.intfVlanMask.vlanMask);
        break;
    default:
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Unknown Event %d received ",msg->event);
        break;

    }

    return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose  Signify that there is a pending VLAN create request,
*           so that others do not attempt to create it.
*
* @param    vid         VLAN ID
*           requestor   vlanType, as associated by requestor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanCreatePendingSet(L7_uint32 vlanId, L7_uint32 requestor)
{
    L7_uint32 i;
    L7_uint32 firstAvailable;

    firstAvailable = 0;
    for (i = 1; i <= DOT1Q_MAX_PENDING_CREATES; i++)
    {
       if (dot1qVlanPendingCreate[i].vlanId == vlanId)
       {
           /* A dynamic VLAN is pending */
           return L7_FAILURE;
       }

       if (dot1qVlanPendingCreate[i].vlanId == 0)
       {
           /* Find first empty entry */
           if (firstAvailable == 0)
           {
               firstAvailable = i;
           }
       }
    }

    if (firstAvailable != 0)
    {
        memset(&dot1qVlanPendingCreate[firstAvailable],0x00,sizeof(dot1qVlanPendingCreate_t));
        dot1qVlanPendingCreate[firstAvailable].vlanId = vlanId;
        dot1qVlanPendingCreate[firstAvailable].vlanType = requestor;
        return L7_SUCCESS;
    }

    return L7_FAILURE;

}


/*********************************************************************
*
* @purpose  Clear pending VLAN request, so that others do not
*           attempt to create it.
*
* @param    vid         VLAN ID
*           requestor   vlanType, as associated by requestor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanCreatePendingReset(L7_uint32 vlanId, L7_uint32 requestor)
{
    L7_uint32 i;

    for (i = 1; i <= DOT1Q_MAX_PENDING_CREATES; i++)
    {
       if ( (dot1qVlanPendingCreate[i].vlanId == vlanId) &&
            (dot1qVlanPendingCreate[i].vlanType == requestor) )
       {
           memset(&dot1qVlanPendingCreate[i],0x00,sizeof(dot1qVlanPendingCreate_t));
           return L7_SUCCESS;
       }

    }
    return L7_FAILURE;

}



/*********************************************************************
*
* @purpose  Check if VLAN create is pending so others do not
*           attempt to create it.
*
* @param    vid         VLAN ID
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
*
* @end
*********************************************************************/
L7_BOOL dot1qVlanCreateIsPending(L7_uint32 vlanId)
{
    L7_uint32 i;

    L7_BOOL  createPending;

    createPending = L7_FALSE;

    for (i = 1; i <= DOT1Q_MAX_PENDING_CREATES; i++)
    {
       if  (dot1qVlanPendingCreate[i].vlanId == vlanId)
       {
           createPending = L7_TRUE;
           break;
       }
    }
    return createPending;

}

/*********************************************************************
*
* @purpose  Create a new VLAN
*
* @param    vid         VLAN ID
*           requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanCreateProcess(L7_uint32 fdbId, L7_uint32 vlanId, L7_uint32 requestor)
{

    dot1q_vlan_dataEntry_t *pDataEntry;
    dot1q_vlan_dataEntry_t createDataEntry;
    dot1qVlanCfgData_t *vCfg;
    L7_RC_t rc = L7_SUCCESS;
    L7_uint32 currentVlanCount ;
    dot1qNotifyData_t vlanData;

    vlanData.numVlans = 1;
    vlanData.data.vlanId = vlanId;

    if (!DOT1Q_IS_PROBE_MASK_SET(DOT1Q_PROBE_MASK_VLAN_CREATE))
    {
        DOT1Q_DEBUG_MSG("trace not set for %x \n", dot1qProbeMask);
    }
    dot1qDebugProbe(vlanId,1,DOT1Q_PROBE_MASK_VLAN_CREATE);

    DOT1Q_DEBUG_MSG("dot1qVlanCreateVlan : vlan %d fdbId %d requestor %d \n",
                    vlanId, fdbId, requestor);


    /* range check vlan ID */
    if ( (vlanId <= L7_DOT1Q_NULL_VLAN_ID) || (vlanId > L7_DOT1Q_MAX_VLAN_ID) )
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Attempt to create a vlan with an invalid vlan id %d."
                " This accommodates for reserved vlan ids. i.e. 4094 - x.",
                (int)vlanId);
        rc = L7_FAILURE;
    }

    /* Validate the entry */

    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    if (vlanDataCount(pVlanTree) >= L7_MAX_VLANS)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Unable to apply VLAN creation request for VLAN ID %d, Database reached MAX VLAN count!", vlanId);
        rc = L7_FAILURE;


        /* Decrement current count */
        dot1qCurrentVlanCountUpdate(L7_FALSE);
        /* Check to see if VLAN create is outstanding */
        if (dot1qVlanCreateIsPending(vlanId) != L7_TRUE)
        {
            /* Do not prevent a reset of the VLAN create pending flag later on */
            if (dot1qVlanCheckValid(vlanId) == L7_SUCCESS)
            {
              /* This vlan already exists, just return failure without resetting the mapTbl
               * This will allow the original vlan to be valid
               */
              (void)osapiWriteLockGive(dot1qCfgRWLock);
              return rc;
            }
        }


        /* If we got this far, and the requestor was DOT1Q_ADMIN, there were requests for
           VLAN creates in the dot1qMsgQueue ahead of this submission.  Clean up the
           allocated vlan cfg entry. */

        if (requestor == DOT1Q_ADMIN)
        {
            if (dot1qVlanConfigEntryReset(vlanId) == L7_SUCCESS)
            {
                /* Set dataChanged flag */
                /* Setting the dataChanged flag outside
                   of the API Set routine and the
                   Build/Restore config paths is an
                   anomaly.  This change is purposeful
                   herre to account for the fact that
                   the operational database could not be
                   configured.

                   This is the only path in the routine
                    where the config file is purposefully
                    rolled back.
                 */
                dot1qCfg->hdr.dataChanged = L7_TRUE;
            }
        } /* requestor == DOT1Q_ADMIN */

    }

    (void)osapiWriteLockGive(dot1qCfgRWLock);



    if (rc != L7_SUCCESS)
    {
        return rc;
    }

    /*===========================================================*/
     /*
         Start updating the database

      */
     /*===========================================================*/


    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry != L7_NULLPTR)
    {
        /*Entry is already present*/
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Attempt to create a vlan (%d) that already exists",vlanId);

        /* Decrement current count */
        dot1qCurrentVlanCountUpdate(L7_FALSE);

        /* Clean up the allocated vlan cfg entry. */
        if (requestor == DOT1Q_ADMIN)
        {
          (void)dot1qVlanConfigEntryReset(vlanId);
        }

        osapiSemaGive(dot1qSem);
        /*dot1qDebugProbeFinish(); */
        return L7_FAILURE;
    }

    memset( (char *)&createDataEntry, 0, sizeof(dot1q_vlan_dataEntry_t));

    createDataEntry.vlanId = vlanId;
    osapiUpTime(&(createDataEntry.current.creationTime));
    if (vlanId == L7_DOT1Q_DEFAULT_VLAN )
    {
        createDataEntry.current.status = L7_DOT1Q_TYPE_DEFAULT;
    }
    else if (requestor == DOT1Q_GVRP)
    {
        createDataEntry.current.status = L7_DOT1Q_TYPE_GVRP_REGISTERED;
    }
    else if (requestor == DOT1Q_WS_L2TUNNEL)
    {
        createDataEntry.current.status = L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED;
    }
    else if (requestor == DOT1Q_DOT1X)
    {
        createDataEntry.current.status = L7_DOT1Q_TYPE_DOT1X_REGISTERED;
    }
    else if (requestor == DOT1Q_IP_INTERNAL_VLAN)
    {
        createDataEntry.current.status = L7_DOT1Q_TYPE_IP_INTERNAL_VLAN;
    }
    else
        createDataEntry.current.status = L7_DOT1Q_TYPE_ADMIN;

    /* Point to configuration structure */
    if (requestor == DOT1Q_ADMIN)
    {
        if (dot1qVlanIsConfigurable(vlanId, &vCfg) == L7_TRUE)
        {
            createDataEntry.admin = vCfg;
        }
        else
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Error pointing to DOT1Q config data for VLAN %d", vlanId);
        }
    }

    /* add the entry to the main tree */
    if ( (rc = vlanEntryAdd(pVlanTree, &createDataEntry) ) != L7_SUCCESS )
    {
        /* The effects of this rc should be propagated as there is a severe error*/
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Error in Updating VLAN %d to VLAN database!", (L7_int32)vlanId);
        rc = L7_FAILURE;
        /* Decrement current count */
        dot1qCurrentVlanCountUpdate(L7_FALSE);

        /* Clean up the allocated vlan cfg entry. */
        if (requestor == DOT1Q_ADMIN)
        {
          (void)dot1qVlanConfigEntryReset(vlanId);
        }

        osapiSemaGive(dot1qSem);
        dot1qDebugProbe(vlanId,2,DOT1Q_PROBE_MASK_VLAN_CREATE);

    }
    else
    {
        osapiSemaGive(dot1qSem);
        if ( dot1qVlanTransientUpdate(vlanId,L7_NULLPTR,L7_NULLPTR) != L7_SUCCESS )
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Problem updating transient data for VLAN %d", (int)vlanId);
        }

        dot1qDebugProbe(vlanId,2,DOT1Q_PROBE_MASK_VLAN_CREATE);


        /* Handle dynamic VLANs for which an instantaneous VLAN ID is needed
           e.g. do not wait around for processing */
        if (requestor == DOT1Q_IP_INTERNAL_VLAN)
        {
            /* This VLAN type will never have a port member.  This is purely a patch for hardware
               to associated a VLAN for internal use by the driver.
               The internal VLAN is used only by the routing code, and is passed along with
               the port-based router internal interface in the routing path.
               This path is simply to allow the VLAN database to consistently report all VLAN IDs
               in use. */
            (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
            if (dot1qVlanCreatePendingReset(vlanId, requestor) != L7_SUCCESS)
            {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                        "Problem resetting pending VLAN create request for VLAN %d", (int)vlanId);
            }
            (void)osapiWriteLockGive(dot1qCfgRWLock);
        }
        else
        {
            /* Other types of VLANs are created and used in hardware*/
            /* fdbId parm in dtl call is unused */
            rc = dtlDot1qCreate(0, vlanId);
            if (rc != L7_SUCCESS)
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                        "DTL call to create VLAN %d failed with rc %d", vlanId, (L7_int32)rc);

                /* Delete from the operational structure */
                if (vlanEntryDelete(pVlanTree,vlanId) != L7_SUCCESS)
                {
                    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                            "Problem unrolling data for VLAN %d", (L7_int32)vlanId);
                }

                /* Decrement current count */
                dot1qCurrentVlanCountUpdate(L7_FALSE);

                /* Clean up the allocated vlan cfg entry. */
                if (requestor == DOT1Q_ADMIN)
                {
                  (void)dot1qVlanConfigEntryReset(vlanId);
                }
            }
        }


    }

    dot1qDebugProbe(vlanId,3,DOT1Q_PROBE_MASK_VLAN_CREATE);

    /* Update statistics */
    if (rc == L7_SUCCESS)
    {
        currentVlanCount = vlanDataCount(pVlanTree);
        if (currentVlanCount > dot1qInfo->MostVLANCount)
        {
            dot1qInfo->MostVLANCount   = currentVlanCount;
        }

        if (dot1qIsRequestorDynamic(requestor) || (requestor == DOT1Q_DOT1X))
        {
            dot1qInfo->DynamicVLANCount++;
        }
        /* Do all notifications here, _after_ adding to DTL to avoid races */
        /* Do the notifications regardless of the DTL calls for the reasons stated above */
        if (requestor != DOT1Q_IP_INTERNAL_VLAN)
        {
            vlanNotifyRegisteredUsers(&vlanData, 0, VLAN_ADD_NOTIFY);
        }
    }

    else /* failed create */
    {
        dot1qInfo->FailedVLANCreations++;
    }

    /*dot1qDebugProbeFinish();*/
    dot1qDebugProbe(vlanId,4,DOT1Q_PROBE_MASK_VLAN_CREATE);

    return rc;


}

/*********************************************************************
* @purpose  Clean up the interface properties when a VLAN is deleted
*
* @param    vid         VLAN ID
*
* @returns  None
*
* @notes This function assumes that the data structures accessed are
*        protected
*
* @end
*********************************************************************/
static void dot1qVlanDeleteIntfCleanup(L7_uint32 vlanId)
{
  L7_RC_t intfRc;
  L7_uint32 intIfNum;
  dot1qIntfCfgData_t *pCfg;
  L7_uint32 *pvid;

  intfRc = nimFirstValidIntfNumber(&intIfNum);
  while (intfRc == L7_SUCCESS)
  {
    L7_VLAN_CLRMASKBIT(dot1qIntfVidList[intIfNum],vlanId);
    if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      pvid = &pCfg->portCfg.VID[L7_DOT1Q_DEFAULT_VID_INDEX];
      if (*pvid == vlanId)
      {
        *pvid = L7_DOT1Q_DEFAULT_VLAN;
        (void)dot1qQportsVIDProcess(intIfNum, L7_DOT1Q_DEFAULT_VLAN,
                                 L7_DOT1Q_DEFAULT_VID_INDEX,DOT1Q_ADMIN);
      }
    }
    intfRc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }
}
/*********************************************************************
* @purpose  Delete a VLAN
*
* @param    vid         VLAN ID
* @param    requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanDeleteProcess(L7_uint32 vlanId,L7_uint32 requestor)
{
    L7_uint32 rc = L7_FAILURE, cid, vlanType;
    dot1q_vlan_dataEntry_t *pDataEntry;
    dot1qNotifyData_t vlanData;
    L7_BOOL result;


    vlanData.numVlans = 1;
    vlanData.data.vlanId = vlanId;

    dot1qDebugProbe(vlanId,1,DOT1Q_PROBE_MASK_VLAN_DELETE);
    DOT1Q_DEBUG_MSG("dot1qVlanDeleteProcess : vlan %d requestor %d \n", vlanId, requestor);
    
    /*Search the vlan DB */

    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "VLan %d does not exist", vlanId);
        osapiSemaGive(dot1qSem);
        dot1qCurrentVlanCountUpdate(L7_TRUE);
        return L7_FAILURE;
    }

    /* Parmameter checking */
    if (vlanDataCount(pVlanTree)  <= 1)
    {
        (void) trapMgrLastVlanDeleteLastLogTrap(vlanId);
        /* Logs a message while deleting the last vlan
        and does not return a failure and performs the delete operation*/
    }

    if (pDataEntry->current.status == L7_DOT1Q_TYPE_DEFAULT)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1Q_COMPONENT_ID,
                "Deleting the default VLAN."
                " Typically encountered during clear Vlan and clear config");
    }
   
    if((dot1qIsVlanDynamic(pDataEntry) == L7_TRUE))
    {
       /* First clear the requested componet bit in the creator list */  
       DOT1Q_CLRMASKBIT(pDataEntry->current.dynamicVlanAcquiredList, requestor);

       /* Check for any component having the ownership on that vlan */ 
       DOT1Q_DYNAMICVLAN_ACQ_NONZEROMASK(pDataEntry->current.dynamicVlanAcquiredList, result);
    
       if(result == L7_TRUE)     
       {  
          for(cid=1; cid < DOT1Q_LAST_DYNAMIC_VLAN_COMPONENT; cid++)
          {
            if (COMPONENT_ACQ_ISMASKBITSET(pDataEntry->current.dynamicVlanAcquiredList, cid)!=0)
            {
               if(dot1qVlanTypeGetByRequestor(cid, &vlanType) == L7_SUCCESS)
               {
                 pDataEntry->current.status = vlanType;
                 break;   
               }
               else 
               {
                 L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                         "VLan %d requestor type %d does not exist", vlanId, cid);
                 dot1qCurrentVlanCountUpdate(L7_TRUE);
                 osapiSemaGive(dot1qSem);
                 return L7_FAILURE;
               }   
            } 
          } 
          /* Somebody has taken the ownership and we dont need to do actual deletion */         
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                 "Can not delete the VLAN, Some unknown component has taken the ownership!",vlanId); 
          dot1qCurrentVlanCountUpdate(L7_TRUE);
          osapiSemaGive(dot1qSem);
          return L7_SUCCESS;  
       }
    }


    if (((pDataEntry->current.status == L7_DOT1Q_TYPE_GVRP_REGISTERED) && (requestor != DOT1Q_GVRP)) ||
        ((pDataEntry->current.status == L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED) && (requestor != DOT1Q_WS_L2TUNNEL)) ||
        ((pDataEntry->current.status == L7_DOT1Q_TYPE_IP_INTERNAL_VLAN) && (requestor != DOT1Q_IP_INTERNAL_VLAN)) ||
        ((pDataEntry->current.status == L7_DOT1Q_TYPE_DOT1X_REGISTERED) && (requestor != DOT1Q_DOT1X)) ||
        ((pDataEntry->current.status == L7_DOT1Q_TYPE_ADMIN) && (requestor != DOT1Q_ADMIN))
       )
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Not valid permision to delete the VLAN %d requestor %d",
                vlanId,requestor );

        dot1qCurrentVlanCountUpdate(L7_TRUE);
        osapiSemaGive(dot1qSem);

    }
    else
    {

        (void) dot1qVlanDeleteOutcalls(vlanId, requestor);

        if (requestor != DOT1Q_IP_INTERNAL_VLAN)
        {
            vlanNotifyRegisteredUsers(&vlanData, 0, VLAN_DELETE_PENDING_NOTIFY);
            rc = dtlDot1qPurge(0, (L7_ushort16)vlanId);
            if (rc != L7_SUCCESS)
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                       "VLAN Delete Call failed in driver for vlan %d", (L7_int32)vlanId);
            }
        }


        /* Delete from the operational structure */
        if (vlanEntryDelete(pVlanTree,vlanId) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                    "Problem deleting data for VLAN %d", (L7_int32)vlanId);
        }

        (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

        dot1qVlanDeleteIntfCleanup(vlanId);

        (void)osapiWriteLockGive(dot1qCfgRWLock);
        osapiSemaGive(dot1qSem);

        if (rc == L7_SUCCESS)
        {
            dot1qInfo->VLANDeletionCount++;
        }

        /* This notification will take place regardless of the DTL failure as the DTL call is
           only one of several processes that take place when deleting a VLAN */
        if (requestor != DOT1Q_IP_INTERNAL_VLAN)
        {
            vlanNotifyRegisteredUsers(&vlanData, 0, VLAN_DELETE_NOTIFY);
        }

        rc = L7_SUCCESS;


    }
    dot1qDebugProbe(vlanId,4,DOT1Q_PROBE_MASK_VLAN_DELETE);

    return rc;
}


/*********************************************************************
*
* @purpose  Set VLAN port membership
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be added
*                       or L7_ALL_INTERFACES change all interfaces
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
* @param    requestor   DOT1Q_ADMIN or DOT1Q_GVRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberSetModify(L7_uint32 vlanId, L7_uint32 intIfNum, L7_uint32 mode,
                                 L7_uint32 requestor, DOT1Q_SWPORT_MODE_t swport_mode)
{
    dot1q_vlan_dataEntry_t *pDataEntry;
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 curr;
    /*L7_INTF_TYPES_t sysIntfType;*/
    L7_uint32  acquired;
    L7_BOOL done = L7_FALSE;


    dot1qDebugProbe(vlanId,1,DOT1Q_PROBE_MASK_VLAN_PARTICIPATE);

    DOT1Q_DEBUG_MSG("dot1qVlanMemberSetModify : vlan %d  intifNum %d mode %d requestor %d \n",
                    vlanId, intIfNum, mode, requestor);

    if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
    {
        return rc;
    }



    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        /*Entry is already present*/

        osapiSemaGive(dot1qSem);
        return L7_NOT_EXIST;
    }

    if (dot1qIsVlanDynamic(pDataEntry) && (requestor == DOT1Q_ADMIN))
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
            "Dynamic entry %d can only be modified after it is converted to static."
            " If this vlan is a learnt via GVRP then we cannot modify it’s member "
            "set via management.",pDataEntry->vlanId);
        osapiSemaGive(dot1qSem);
        return L7_FAILURE;
    }

    if (intIfNum == L7_ALL_INTERFACES)
    {
        if (nimFirstValidIntfNumber(&curr) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Cannot get first valid NIM Interface number");
            osapiSemaGive(dot1qSem);
            return L7_FAILURE;
        }

    }
    else
    {

        curr = intIfNum;
        done = L7_TRUE;

    }


    /*Make sure the slate is clean */
    memset(port_list,0x0,sizeof(port_list));
    memset(tag_list,0x0,sizeof(tag_list));
    numPorts = 0 ;
    numTagPorts = 0 ;

    do
    {
        COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
        if ( (dot1qIntfIsValid(curr)== L7_TRUE)  && (dot1qIntfIsSettable(curr) == L7_TRUE))
        {
            if (dot1qOperSwitchPortVlanConfigAllowed(curr, vlanId, mode) != L7_TRUE)
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                       "Cannot configure VLAN membership for an access port %d",
                       curr);
            }
            else
            {
                switch (mode)
                {
                  case L7_DOT1Q_FIXED:
                  {
                    L7_uint32 result=0;
                    COMPONENT_MASKNUMBITSETGET(dot1qQports[curr].acquiredList,result);
                    if (acquired != L7_TRUE ||
                        (requestor == DOT1Q_DOT1X && COMPONENT_ISMASKBITSET(dot1qQports[curr].acquiredList,L7_DOT1X_COMPONENT_ID)!=0 &&
                         result == 1))
                    {
                      /*  Compile the list of ports in the mask to tell the driver */
                      port_list[numPorts] = curr;
                      numPorts++;
                      NIM_INTF_SETMASKBIT(pDataEntry->current.currentEgressPorts,curr);

                      L7_VLAN_SETMASKBIT(dot1qIntfVidList[curr],vlanId);

                      /*Set the tagging properties also if the vlan is being modified by gvrp */
                      if(dot1qIsRequestorDynamic(requestor) ||
                         (pDataEntry->admin && NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.taggedSet, curr))
                        )
                      {
                          NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, curr);
                          tag_list[numTagPorts] = curr;
                          numTagPorts++;
                      }

                      	/* if the given vlan is dynamic and requestor also dynamic we need to set the vlan acquired 
			   component flag. Since if one component try to delete the vlan after its use and at the same 
			   time other component also using the same vlan then other component acquires that vlan*/ 	

                      if((dot1qIsVlanDynamic(pDataEntry) == L7_TRUE))
                      {
                          DOT1Q_ACQ_SETMASKBIT(pDataEntry->current.dynamicVlanAcquiredList, requestor);
                      }
	
                    }

                  }
                  break;

                  case L7_DOT1Q_NORMAL_REGISTRATION:
                  {
                    if (NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,curr) &&
                        (acquired != L7_TRUE ))
                    {
                        /*  Compile the list of ports in the mask to tell the driver */
                        port_list[numPorts] = curr;
                        numPorts++;
                        NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts,curr);
                        L7_VLAN_CLRMASKBIT(dot1qIntfVidList[curr],vlanId);
                        if (dot1qIsRequestorDynamic(requestor))
                        {
                           NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet ,curr);
                        }
                    }

                  }
                  break;

                  case L7_DOT1Q_FORBIDDEN:
                  {
                    L7_uint32 result=0;
                    COMPONENT_MASKNUMBITSETGET(dot1qQports[curr].acquiredList,result);

                    if (NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,curr) &&
                        (acquired != L7_TRUE || (requestor == DOT1Q_DOT1X &&
                         COMPONENT_ISMASKBITSET(dot1qQports[curr].acquiredList,L7_DOT1X_COMPONENT_ID)!=0 &&
                         result == 1)))
                    {
                        /*  Compile the list of ports in the mask to tell the driver */
                        port_list[numPorts] = curr;
                        numPorts++;
                        NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts,curr);
                        if(requestor == DOT1Q_ADMIN)
                        {
                          NIM_INTF_CLRMASKBIT(pDataEntry->admin->vlanCfg.staticEgressPorts,curr);
                        }
                        L7_VLAN_CLRMASKBIT(dot1qIntfVidList[curr],vlanId);
                        if (dot1qIsRequestorDynamic(requestor))
                        {
                           NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet ,curr);
                        }
                    }

                  }
                  break;

                 default:
                    break;
                } /* switch*/
            } /*else swportmode == access */

        } /* if intf valid*/

        /* Get the next valid interface if we need to */
        if ( nimNextValidIntfNumber(curr,&curr) != L7_SUCCESS)
        {
            done = L7_TRUE;
        }
    }/* do*/
    while ( done == L7_FALSE );


    /* We are done with the vlan tree return the sema. */
    osapiSemaGive(dot1qSem);

    if ( dot1qVlanTransientUpdate(vlanId,L7_NULLPTR,L7_NULLPTR) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Problem updating transient data for VLAN %d", (int)vlanId);
    }

    dot1qMemberSetPostProcess(vlanId,port_list,numPorts,mode,tag_list, numTagPorts);

    dot1qDebugProbe(vlanId,4,DOT1Q_PROBE_MASK_VLAN_PARTICIPATE);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the tagging configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanTaggedMemberSetModify(L7_uint32 vlanId,L7_uint32 intIfNum, L7_uint32 tagMode, vlanRequestor_t requestor)
{
    dot1q_vlan_dataEntry_t *pDataEntry = L7_NULLPTR;
    L7_uint32 curr;
    L7_uint32  acquired;
    L7_BOOL done = L7_FALSE;
    DOT1Q_SWPORT_MODE_t currSwPortMode;


    if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
    {
        return L7_NOT_EXISTS;
    }


    dot1qDebugProbe(vlanId,1,DOT1Q_PROBE_MASK_VLAN_TAGGEDSET);

    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        /*Entry is not present*/

        osapiSemaGive(dot1qSem);
        return L7_NOT_EXIST;
    }

    if ((L7_TRUE == dot1qIsVlanDynamic(pDataEntry)) &&
        (L7_TRUE != dot1qIsRequestorDynamic(requestor) && (requestor != DOT1Q_DOT1X)))
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
            "Dynamic entry %d can only be modified after it is converted to static."
            " If this vlan is a learnt via GVRP then we cannot modify it’s member "
            "set via management.",pDataEntry->vlanId);
        osapiSemaGive(dot1qSem);
        return L7_FAILURE;
    }

    if (intIfNum == L7_ALL_INTERFACES)
    {
        if (nimFirstValidIntfNumber(&curr) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Cannot get first valid NIM Interface number");
            osapiSemaGive(dot1qSem);
            return L7_FAILURE;
        }

    }
    else
    {

        curr = intIfNum;
        done = L7_TRUE;

    }


    /*Make sure the slate is clean */
    memset(port_list,0x0,sizeof(port_list));
    numPorts = 0 ;

    do
    {
        if ((dot1qIntfIsValid(curr)== L7_TRUE) && (dot1qIntfIsSettable(curr) == L7_TRUE))
        {
            currSwPortMode = dot1qOperSwitchPortModeGet(curr);

            if ((currSwPortMode == DOT1Q_SWPORT_MODE_ACCESS) ||
                 (currSwPortMode == DOT1Q_SWPORT_MODE_TRUNK))
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                       "Cannot configure VLAN tagging for an access port %d",
                       curr);
            }
            else
            {

                COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
                if (acquired != L7_TRUE )
                {
                  /*  Compile the list of ports in the mask to tell the driver */
                  port_list[numPorts] = curr;
                  numPorts++;
                }

                /* Although this information is stored in the config structure, it is
                   replicated in the operational structure so that the data is
                   accessible for dynamic VLANs */
                if (tagMode == L7_DOT1Q_TAGGED)
                    NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, curr);
                else
                    NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet, curr);
            }
        }


        /* Get the next valid interface if we need to */
        if ( nimNextValidIntfNumber(curr,&curr) != L7_SUCCESS)
        {
            done = L7_TRUE;
        }
    }
    while ( done == L7_FALSE );


    /* We are done with the vlan tree return the sema. */
    osapiSemaGive(dot1qSem);

    if ( dot1qVlanTransientUpdate(vlanId,L7_NULLPTR,L7_NULLPTR) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Problem updating transient data for VLAN %d", (int)vlanId);
    }

    dot1qTaggingPostProcess(vlanId,port_list,numPorts,tagMode);

    dot1qDebugProbe(vlanId,4,DOT1Q_PROBE_MASK_VLAN_TAGGEDSET);

    return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Set VLAN port membership for a range of interfaces specified in the Mask
*
* @param    vid         vlan ID
* @param    mask        intf Mask
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  There are four api's for changing vlan membership. This function processes membership
*         for a range of interfaces. The mask it expects is only for change in value
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMSMaskModify(L7_uint32 vlanId, NIM_INTF_MASK_t *mask, L7_uint32 mode, DOT1Q_SWPORT_MODE_t swport_mode)
{
    dot1q_vlan_dataEntry_t *pDataEntry;
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 curr;
    /*L7_INTF_TYPES_t sysIntfType;*/
    L7_uint32  acquired;
    NIM_INTF_MASK_t currMask;


    dot1qDebugProbe(vlanId,1,DOT1Q_PROBE_MASK_VLAN_PARTICIPATE);



    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        /*Entry is already present*/

        osapiSemaGive(dot1qSem);
        return L7_NOT_EXIST;
    }

    if (dot1qIsVlanDynamic(pDataEntry))
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Dynamic entry %d can only be modified after it is converted to static",pDataEntry->vlanId);
        osapiSemaGive(dot1qSem);
        return L7_FAILURE;
    }

    memcpy(&currMask, mask, sizeof(currMask));
    /*Make sure the slate is clean */
    memset(port_list,0x0,sizeof(port_list));
    memset(tag_list,0x0,sizeof(tag_list));
    numPorts = 0 ;
    numTagPorts = 0 ;

    NIM_INTF_FHMASKBIT(currMask,curr);
    while ( curr != 0 )
    {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(curr, L7_SYSNAME, ifName);

        COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
        if ( (dot1qIntfIsValid(curr)== L7_TRUE)  && (dot1qIntfIsSettable(curr) == L7_TRUE))
        {
            if (dot1qOperSwitchPortVlanConfigAllowed(curr, vlanId, mode) != L7_TRUE)
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                       "Cannot configure VLAN membership for an access port %s",
                       ifName);
            }
            else
            {
                switch (mode)
                {
                  case L7_DOT1Q_FIXED:
                  {
#if defined(FEAT_METRO_CPE_V1_0)
  do
  {
    DOT1AD_INTFERFACE_TYPE_t intfType;

    if (vlanId == simMgmtVlanIdGet() &&
        dot1adInterfaceTypeGet(curr, &intfType) == L7_SUCCESS &&
        (
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI)||
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)||
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
        )
       )

    {
      /* Re-using existing flag */
      acquired = L7_TRUE;
    }
  } while(0);
#endif
                    if (acquired != L7_TRUE )
                    {
                      /*  Compile the list of ports in the mask to tell the driver */
                      port_list[numPorts] = curr;
                      numPorts++;
                      NIM_INTF_SETMASKBIT(pDataEntry->current.currentEgressPorts,curr);

                      L7_VLAN_SETMASKBIT(dot1qIntfVidList[curr],vlanId);

                      if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.taggedSet, curr))
                      {
                          NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, curr);
                          tag_list[numTagPorts] = curr;
                          numTagPorts++;
                      }
                    }

                  }
                  break;

                  case L7_DOT1Q_NORMAL_REGISTRATION:
                  {
#if defined(FEAT_METRO_CPE_V1_0)
  do
  {
    DOT1AD_INTFERFACE_TYPE_t intfType;

    if (vlanId == simMgmtVlanIdGet() &&
        dot1adInterfaceTypeGet(curr, &intfType) == L7_SUCCESS &&
        (
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI) ||
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)||
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
        )
      )
    {
      /* Re-using existing flag */
      acquired = L7_TRUE;
    }
  } while(0);
#endif
                    if (NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,curr) &&
                        (acquired != L7_TRUE ))
                    {
                        /*  Compile the list of ports in the mask to tell the driver */
                        port_list[numPorts] = curr;
                        numPorts++;
                        NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts,curr);
                        L7_VLAN_CLRMASKBIT(dot1qIntfVidList[curr],vlanId);
                        NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet ,curr);

                    }

                  }
                  break;

                  case L7_DOT1Q_FORBIDDEN:
                  {
                    if (NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,curr) &&
                        (acquired != L7_TRUE ))
                    {
                        /*  Compile the list of ports in the mask to tell the driver */
                        port_list[numPorts] = curr;
                        numPorts++;
                        NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts,curr);
                        L7_VLAN_CLRMASKBIT(dot1qIntfVidList[curr],vlanId);
                        NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet ,curr);

                    }

                  }
                  break;

                 default:
                    break;
                }/*switch mode*/
            }/* else swictchport == access*/

        }/* if intf valid*/

        NIM_INTF_CLRMASKBIT(currMask,curr);
        /* Get the next valid interface */
        NIM_INTF_FHMASKBIT(currMask,curr);
    }/* while*/



    /* We are done with the vlan tree return the sema. */
    osapiSemaGive(dot1qSem);

    if ( dot1qVlanTransientUpdate(vlanId,L7_NULLPTR,L7_NULLPTR) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Problem updating transient data for VLAN %d", (int)vlanId);
    }

    dot1qMemberSetPostProcess(vlanId,port_list,numPorts,mode,tag_list, numTagPorts);

    dot1qDebugProbe(vlanId,4,DOT1Q_PROBE_MASK_VLAN_PARTICIPATE);

    return rc;


}

/*********************************************************************
*
* @purpose  Set the tagging configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanTSMaskModify(L7_uint32 vlanId,NIM_INTF_MASK_t *mask, L7_uint32 tagMode )
{
    dot1q_vlan_dataEntry_t *pDataEntry = L7_NULLPTR;
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 curr;
    L7_uint32  acquired;
    NIM_INTF_MASK_t currMask;
    DOT1Q_SWPORT_MODE_t currSwPortMode;


    dot1qDebugProbe(vlanId,1,DOT1Q_PROBE_MASK_VLAN_TAGGEDSET);

    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        /*Entry is already present*/

        osapiSemaGive(dot1qSem);
        return L7_NOT_EXIST;
    }

    if (dot1qIsVlanDynamic(pDataEntry))
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Dynamic entry %d can only be modified after it is converted to static",pDataEntry->vlanId);
        osapiSemaGive(dot1qSem);
        return L7_FAILURE;
    }


    memcpy(&currMask, mask, sizeof(currMask));

    /*Make sure the slate is clean */
    memset(port_list,0x0,sizeof(port_list));
    numPorts = 0 ;
    NIM_INTF_FHMASKBIT(currMask,curr);
    while ( curr != 0 )
    {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(curr, L7_SYSNAME, ifName);

        if ( (dot1qIntfIsValid(curr)== L7_TRUE)  && (dot1qIntfIsSettable(curr) == L7_TRUE))
        {
            currSwPortMode = dot1qOperSwitchPortModeGet(curr);

            if ((currSwPortMode == DOT1Q_SWPORT_MODE_ACCESS) ||
                 (currSwPortMode == DOT1Q_SWPORT_MODE_TRUNK))
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                       "Cannot configure VLAN tagging for an access port %s",
                       ifName);
            }
            else
            {
                COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
                if (acquired != L7_TRUE )
                {
                  /*  Compile the list of ports in the mask to tell the driver */
                  port_list[numPorts] = curr;
                  numPorts++;
                }

                /* Although this information is stored in the config structure, it is
                   replicated in the operational structure so that the data is
                   accessible for dynamic VLANs */
                if (tagMode == L7_DOT1Q_TAGGED)
                    NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, curr);
                else
                    NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet, curr);
            }
        }

        NIM_INTF_CLRMASKBIT(currMask,curr);
        /* Get the next valid interface */
        NIM_INTF_FHMASKBIT(currMask,curr);
    }



    /* We are done with the vlan tree return the sema. */
    osapiSemaGive(dot1qSem);

    if ( dot1qVlanTransientUpdate(vlanId,L7_NULLPTR,L7_NULLPTR) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Problem updating transient data for VLAN %d", (int)vlanId);
    }


    dot1qTaggingPostProcess(vlanId,port_list,numPorts,tagMode);
    dot1qDebugProbe(vlanId,4,DOT1Q_PROBE_MASK_VLAN_TAGGEDSET);


    return rc;


}


/*********************************************************************
*
* @purpose  Set the member list as well as the tagging list of a port.
*           Copy the config information to the vlan data structure
*
* @param    intIfNum    physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    memberlist  Mask of vlans with bits set for vlans to be members for this port
* @param    memberlist  Mask of vlans with bits set for vlans to be tagged for this port
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if intf not present
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfVlanListPopulate(L7_uint32 intIfNum, L7_VLAN_MASK_t *memberVlans,
                                  L7_VLAN_MASK_t *taggedVlans)
{

    /*

     - find the vlan entry in avl tree and set the memberSet and tagged set bits for this port
     - call the driver with all the vlans for this port
     - Notify the users of port add/ tagging start for each vlan

    */

    dot1q_vlan_dataEntry_t *pDataEntry = L7_NULLPTR;
    L7_uint32 vlanId;
    dot1qNotifyData_t vlanData;
    dot1qNotifyData_t vlanTagData;
    L7_uint32 singleVlan = 0;
    L7_uint32 singleTagVlan = 0;
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    memset(&vlanData, 0x00, sizeof(dot1qNotifyData_t));
    memset(&vlanTagData, 0x00, sizeof(dot1qNotifyData_t));

    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = (dot1q_vlan_dataEntry_t*)(vlanFirstDataEntry(pVlanTree));
    while (pDataEntry != NULL)
    {
        vlanId = pDataEntry->vlanId;
        /* update membership */
        if (L7_VLAN_ISMASKBITSET(*memberVlans, vlanId))
        {
          /*dot1qVlanMemberSet(vCfg->vlanId, intIfNum, L7_DOT1Q_FIXED, DOT1Q_ADMIN);*/
          /*dot1qVlanMemberSetModify(vCfg->vlanId,intIfNum, L7_DOT1Q_FIXED, DOT1Q_ADMIN);*/
          NIM_INTF_SETMASKBIT(pDataEntry->current.currentEgressPorts,intIfNum);
          L7_VLAN_SETMASKBIT(dot1qIntfVidList[intIfNum],vlanId);
          vlanData.numVlans++;
          singleVlan = vlanId;
          if (L7_VLAN_ISMASKBITSET(*taggedVlans, vlanId))
          {
              NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, intIfNum);
              vlanTagData.numVlans++;
              singleTagVlan = vlanId;
            /* dot1qVlanTaggedMemberSetModify(vCfg->vlanId, intIfNum, L7_DOT1Q_TAGGED);i */
        }
        }
        pDataEntry = vlanNextDataEntry(pVlanTree, pDataEntry ->vlanId);
    } /* end for vlanId = NULL  */
    osapiSemaGive(dot1qSem);


    if (dtlDot1qConfigInterface(intIfNum,(L7_uchar8 *)memberVlans, (L7_uchar8 *)taggedVlans)
        != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "dtl request failure when deleting intf %s",
                ifName);

        return L7_FAILURE;

    }


    if (vlanData.numVlans == 1)
    {
        vlanData.data.vlanId = singleVlan;
    }
    else
    {
        memcpy(&vlanData.data.vlanMask, memberVlans, sizeof(L7_VLAN_MASK_t));
    }
    if (vlanTagData.numVlans == 1)
    {
        vlanTagData.data.vlanId = singleTagVlan;
    }
    else
    {
        memcpy(&vlanTagData.data.vlanMask, taggedVlans, sizeof(L7_VLAN_MASK_t));
    }

    vlanNotifyRegisteredUsers(&vlanData,intIfNum,VLAN_ADD_PORT_NOTIFY);
    vlanNotifyRegisteredUsers(&vlanTagData,intIfNum,VLAN_START_TAGGING_PORT_NOTIFY);

    return L7_SUCCESS;
}


#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER

/*********************************************************************
*
* @purpose  Set the group filter configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    filterMode  group filtering configuration
*                       (@b{  L7_DOT1Q_FORWARD_ALL,
*                             L7_DOT1Q_FORWARD_UNREGISTERED, or
*                             L7_DOT1Q_FILTER_UNREGISTERED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanGroupFilterModify(L7_uint32 vlanId,L7_uint32 intIfNum,
                                   L7_uint32 filterType, L7_uint32 requestor)
{

    dot1q_vlan_dataEntry_t *pDataEntry = L7_NULLPTR;
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 curr, next;
    /*L7_INTF_TYPES_t sysIntfType;*/
    L7_uint32  acquired;
    L7_BOOL done = L7_FALSE;


    if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
    {
        return L7_NOT_EXISTS;
    }

    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        /*Entry is already present*/

        osapiSemaGive(dot1qSem);
        return L7_NOT_EXIST;
    }

    if (dot1qIsVlanDynamic(pDataEntry))
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Dynamic entry %d can only be modified after it is converted to static",pDataEntry->vlanId);
        osapiSemaGive(dot1qSem);
        return L7_FAILURE;
    }

    if (intIfNum == L7_ALL_INTERFACES)
    {
        if (nimFirstValidIntfNumber(&curr) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Cannot get first valid NIM Interface number");
            osapiSemaGive(dot1qSem);
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
        COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
        if ((dot1qIntfIsValid(curr)== L7_TRUE) &&
            (dot1qIntfIsSettable(curr)== L7_TRUE) &&
            (acquired != L7_TRUE )
           )
        {


            if (filterType == L7_DOT1Q_FORWARD_ALL)
            {
                if (requestor == DOT1Q_ADMIN)
                {
                    NIM_INTF_SETMASKBIT(pDataEntry->admin->groupFilter.forwardAllMask, curr);
                    NIM_INTF_CLRMASKBIT(pDataEntry->admin->groupFilter.forwardUnregMask, curr);
                }
            }
            else if (filterType == L7_DOT1Q_FORWARD_UNREGISTERED)
            {
                if (requestor == DOT1Q_ADMIN)
                {
                    NIM_INTF_SETMASKBIT(pDataEntry->admin->groupFilter.forwardUnregMask, curr);
                    NIM_INTF_CLRMASKBIT(pDataEntry->admin->groupFilter.forwardAllMask, curr);
                }
            }
            else
            {
                if (requestor == DOT1Q_ADMIN)
                {
                    NIM_INTF_CLRMASKBIT(pDataEntry->admin->groupFilter.forwardUnregMask, curr);
                    NIM_INTF_CLRMASKBIT(pDataEntry->admin->groupFilter.forwardAllMask, curr);
                }
            }

        }


        /* Get the next valid interface if we need to */
        rc = nimNextValidIntfNumber(curr,&next);
        if ( rc != L7_SUCCESS)
        {
            done = L7_TRUE;

        }
        else
            curr = next;
    }
    while ( done == L7_FALSE );


    /* We are done with the vlan tree return the sema. */
    osapiSemaGive(dot1qSem);

    if ( dot1qVlanTransientUpdate(vlanId,L7_NULLPTR,L7_NULLPTR) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Problem updating transient data for VLAN %d", (int)vlanId);
    }


    return L7_SUCCESS;


}

#endif


/*********************************************************************
*
* @purpose  Set the tagging configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanTaggedSetMaskModify(L7_uint32 vlanId, NIM_INTF_MASK_t intfMask)
{
    dot1q_vlan_dataEntry_t *pDataEntry ;
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 curr= 0;
    L7_uint32  acquired;
    DOT1Q_SWPORT_MODE_t currSwPortMode;

    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        /*Entry is already present*/
        osapiSemaGive(dot1qSem);
        return L7_NOT_EXIST;
    }

    if (dot1qIsVlanDynamic(pDataEntry))
    {
        /* Only static configurations are allowed to change via the mask */
        osapiSemaGive(dot1qSem);
        return L7_ERROR;
    }

    if (nimPhaseStatusCheck() == L7_TRUE)
    {
      rc = nimFirstValidIntfNumber(&curr);
    }

    numPorts = 0;
    numDelPorts = 0;

    memset(port_list,0x0,sizeof(port_list));
    memset(del_port_list,0x0,sizeof(del_port_list));

    while (rc == L7_SUCCESS)
    {
      currSwPortMode = dot1qOperSwitchPortModeGet(curr);

      if ((currSwPortMode == DOT1Q_SWPORT_MODE_ACCESS) ||
           (currSwPortMode == DOT1Q_SWPORT_MODE_TRUNK))
      {
         if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.taggedSet,curr) !=
             NIM_INTF_ISMASKBITSET(intfMask, curr))
         {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(curr, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                 "Cannot configure VLAN tagging for an access or trunk port %s",
                 ifName);
      }
      }
      else
      {

      COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
      if ( !NIM_INTF_ISMASKBITSET(pDataEntry->current.taggedSet,curr) &&
           NIM_INTF_ISMASKBITSET(intfMask,curr))
      {

        NIM_INTF_SETMASKBIT(pDataEntry->admin->vlanCfg.taggedSet, curr);
        if (acquired != L7_TRUE)
        {

            NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, curr);
          port_list[numPorts] = curr;
          numPorts++;
        }
      }

      if (NIM_INTF_ISMASKBITSET(pDataEntry->current.taggedSet,curr) &&
           !NIM_INTF_ISMASKBITSET(intfMask,curr))
      {
        NIM_INTF_CLRMASKBIT(pDataEntry->admin->vlanCfg.taggedSet,curr);
        if (acquired != L7_TRUE)
        {
          NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet,curr);
          del_port_list[numDelPorts] = curr;
          numDelPorts++;
        }

      }
        }

      rc = nimNextValidIntfNumber(curr, &curr);
    }

    osapiSemaGive(dot1qSem);

    if (numPorts > 0 )
    {
      dot1qTaggingPostProcess(vlanId,port_list,numPorts,L7_DOT1Q_TAGGED);
    }

    if (numDelPorts > 0 )
    {
      dot1qTaggingPostProcess(vlanId,del_port_list,numDelPorts,L7_DOT1Q_UNTAGGED);
    }


    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set a port mask identifying VLAN membership
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set
*                       correspond to ports which are members of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberSetMaskModify(L7_uint32 vlanId, NIM_INTF_MASK_t intfMask)
{
    dot1q_vlan_dataEntry_t *pDataEntry ;
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 curr= 0;
    L7_uint32  acquired;

    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        /*Entry is already present*/
        osapiSemaGive(dot1qSem);
        return L7_NOT_EXIST;
    }

    if (dot1qIsVlanDynamic(pDataEntry))
    {
        /* Only static configurations are allowed to change via the mask */
        osapiSemaGive(dot1qSem);
        return rc;
    }


    if (nimPhaseStatusCheck() == L7_TRUE)
    {
      rc = nimFirstValidIntfNumber(&curr);
    }

    numPorts = 0;
    numDelPorts = 0;
    numTagPorts = 0 ;
    memset(port_list,0x0,sizeof(port_list));
    memset(del_port_list,0x0,sizeof(del_port_list));
    memset(tag_list,0x0,sizeof(tag_list));


    while (rc == L7_SUCCESS)
    {

      COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
      if (!NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,curr) &&
           NIM_INTF_ISMASKBITSET(intfMask,curr))
      {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(curr, L7_SYSNAME, ifName);

          if (dot1qOperSwitchPortVlanConfigAllowed(curr, vlanId, L7_DOT1Q_FIXED) != L7_TRUE)
          {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                     "Cannot configure VLAN membership for an access port %s",
                     ifName);
          }
          else
          {

              NIM_INTF_SETMASKBIT(pDataEntry->admin->vlanCfg.staticEgressPorts, curr);
              NIM_INTF_CLRMASKBIT(pDataEntry->admin->vlanCfg.forbiddenEgressPorts, curr);
#if defined(FEAT_METRO_CPE_V1_0)
  do
  {
    DOT1AD_INTFERFACE_TYPE_t intfType;

    if (vlanId == simMgmtVlanIdGet() &&
        dot1adInterfaceTypeGet(curr, &intfType) == L7_SUCCESS &&
        (
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI)||
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)||
          (intfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
        )
      )
    {
      /* Re-using existing flag */
      acquired = L7_TRUE;
    }
  } while(0);
#endif
              if (acquired != L7_TRUE)
              {
                NIM_INTF_SETMASKBIT(pDataEntry->current.currentEgressPorts,curr);
                L7_VLAN_SETMASKBIT(dot1qIntfVidList[curr],vlanId);

                port_list[numPorts] = curr;
                numPorts++;
                if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.taggedSet, curr))
                {
                   tag_list[numTagPorts] = curr;
                   numTagPorts++;
                }
                else if(dot1qOperSwitchPortModeGet(curr) == DOT1Q_SWPORT_MODE_TRUNK)
                {
                  NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, curr);
                  tag_list[numTagPorts] = curr;
                  numTagPorts++;
                }

              }
          }/* else switchportmode == access*/
      }/* if is mskt bit set */


      if (NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,curr) &&
           !NIM_INTF_ISMASKBITSET(intfMask,curr))
      {
          NIM_INTF_CLRMASKBIT(pDataEntry->admin->vlanCfg.staticEgressPorts,curr);
          if (acquired != L7_TRUE)
          {
            NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts,curr);
            L7_VLAN_CLRMASKBIT(dot1qIntfVidList[curr],vlanId);
            del_port_list[numDelPorts] = curr;
            numDelPorts++;
          }
      }

      rc = nimNextValidIntfNumber(curr, &curr);
    }


    osapiSemaGive(dot1qSem);


    dot1qMemberSetPostProcess(vlanId,port_list,numPorts,L7_DOT1Q_FIXED,tag_list, numTagPorts);

    dot1qMemberSetPostProcess(vlanId,del_port_list,numDelPorts,L7_DOT1Q_NORMAL_REGISTRATION,L7_NULLPTR,0);






    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Set a port mask identifying VLAN membership
*
* @param    vid         vlan ID
* @param    *mask       pointer to mask of ports. Bits which are set
*                       correspond to ports which are to be excluded
*                       from member list of the vlan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanMemberSetMaskForbidden(L7_uint32 vlanId, NIM_INTF_MASK_t intfMask )
{
    dot1q_vlan_dataEntry_t *pDataEntry ;
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 curr= 0;
    L7_uint32  acquired;

    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == L7_NULLPTR)
    {
        /*Entry is already present*/
        osapiSemaGive(dot1qSem);
        return L7_NOT_EXIST;
    }

    if (dot1qIsVlanDynamic(pDataEntry))
    {
        /* Only static configurations are allowed to change via the mask */
        osapiSemaGive(dot1qSem);
        return L7_ERROR;
    }


    if (nimPhaseStatusCheck() == L7_TRUE)
    {
      rc = nimFirstValidIntfNumber(&curr);
    }


    numDelPorts = 0;
    memset(del_port_list,0x0,sizeof(del_port_list));

    while (rc == L7_SUCCESS)
    {

      COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
      if (NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,curr) &&
           NIM_INTF_ISMASKBITSET(intfMask,curr))
      {


          NIM_INTF_CLRMASKBIT(pDataEntry->admin->vlanCfg.staticEgressPorts,curr);
          if (acquired != L7_TRUE)
          {
            NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts,curr);
            L7_VLAN_CLRMASKBIT(dot1qIntfVidList[curr],vlanId);
            del_port_list[numDelPorts] = curr;
            numDelPorts++;
          }
      }

      if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.forbiddenEgressPorts ,curr) &&
           ! NIM_INTF_ISMASKBITSET(intfMask,curr))
      {
          NIM_INTF_CLRMASKBIT(pDataEntry->admin->vlanCfg.forbiddenEgressPorts,curr);

      }




      rc = nimNextValidIntfNumber(curr, &curr);
    }


    osapiSemaGive(dot1qSem);


    dot1qMemberSetPostProcess(vlanId,del_port_list,numDelPorts,L7_DOT1Q_FORBIDDEN,L7_NULLPTR,0);




    return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose  Convert a dynamically learned VLAN into an
*           administratively configured one.
*
* @param    vid         vlan ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_FAILURE, if other failure
*
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dot1qDynamicToStaticVlanConvertProcess(L7_uint32 vlanId)
{

    dot1q_vlan_dataEntry_t   *pDataEntry;
    dot1qVlanCfgData_t *vCfg;
    L7_uint32 vlanType;

    vCfg = L7_NULL;

    /* Find current entry */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry    = vlanDataSearch(pVlanTree, vlanId);
    if (pDataEntry == NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot find vlan %d to convert it to static", (int)vlanId);
        osapiSemaGive(dot1qSem);
        return(L7_FAILURE);

    }

    if (dot1qIsVlanDynamic(pDataEntry) != L7_TRUE)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Only Dynamically created vlans can be converted");
        osapiSemaGive(dot1qSem);
        return L7_FAILURE;
    }


    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    (void) dot1qVlanIsConfigurable(vlanId, &vCfg);
    if (vCfg == L7_NULL)
    {
      (void)osapiWriteLockGive(dot1qCfgRWLock);
      osapiSemaGive(dot1qSem);
      return L7_FAILURE;
    }

    for(vlanType=1; vlanType < DOT1Q_LAST_DYNAMIC_VLAN_COMPONENT; vlanType++)
    {
      if (COMPONENT_ACQ_ISMASKBITSET(pDataEntry->current.dynamicVlanAcquiredList, vlanType)!=0)
      {
        DOT1Q_CLRMASKBIT(pDataEntry->current.dynamicVlanAcquiredList, vlanType);
      }
    }

    /* Update operational data */
    pDataEntry->admin = vCfg;
    pDataEntry->current.status = L7_DOT1Q_TYPE_ADMIN;

    /* Update configuration */
    memcpy((L7_uchar8 *)&(vCfg->vlanCfg.staticEgressPorts),
           (L7_uchar8 *)&(pDataEntry->current.currentEgressPorts),
           sizeof(NIM_INTF_MASK_t));

    memcpy((L7_uchar8 *)&(vCfg->vlanCfg.taggedSet),
           (L7_uchar8 *)&(pDataEntry->current.taggedSet),
           sizeof(NIM_INTF_MASK_t));

    (void)osapiWriteLockGive(dot1qCfgRWLock);


    osapiSemaGive(dot1qSem);




    return L7_SUCCESS;




}



/*********************************************************************
* @purpose  Reset Vlan configuration to default parameters
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanCfgClearProcess(void)
{
    if (dot1qRestore() == L7_SUCCESS)
        return dot1qApplyConfigData();
    else
        return L7_FAILURE;
}


/**************************************************************************
* @purpose  Create the queue sync semaphore.
*
* @comments The dot1q Queue sync semaphore allows other queues to synchronize
*           with the dot1qQueue by putting a message on the dot1q queue and
*           blocking on the dot1qQueueSync semaphore
*
* @end
*************************************************************************/
void dot1qQueueSyncSemCreate (void)
{

   dot1qQueueSyncSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
   if (dot1qQueueSyncSem == L7_NULL)
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
void dot1qQueueSyncSemGet (void)
{
  L7_RC_t rc;

   rc = osapiSemaTake (dot1qQueueSyncSem, L7_WAIT_FOREVER);
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
void dot1qQueueSyncSemFree (void)
{
    L7_RC_t rc;

    rc = osapiSemaGive (dot1qQueueSyncSem);
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
void dot1qQueueSyncSemDelete (void)
{
    L7_RC_t rc;

    rc = osapiSemaDelete (dot1qQueueSyncSem);
    if (rc != L7_SUCCESS)
    {
        LOG_ERROR (rc);
    }
}


/*********************************************************************
* @purpose  For a given interface sets the tagging property for all the vlans in the vlan mask
*
* @param    intIfNum  Internal Interface Number
* @param    vlanMask  Mask of all the vlans affected
* @param    tagMode   Whether this is to tag or untagg this interface on all these vlans
*                       L7_DOT1Q_UNTAGGED
*                       L7_DOT1Q_TAGGED
*
* @returns  None
*
* @notes    The vlan Mask supplied represents the delta of tagging or untagging property,
*           i.e. this is not the final mask, so if vlan 10 and 20 are already tagged and the
*           mask contains vlan 24 to be tagged, then this port will be tagged for vlan 10, 20 and 24
*           Same for the untagged case.
*           Routine assumes that the requestor is DOT1Q_ADMIN
*           Routine assumes that the driver takes this delta and processes only those vlans
*           identified in this mask.
*
* @end
*********************************************************************/
void dot1qQportsVlanTagMaskProcess(L7_uint32 intIfNum, L7_VLAN_MASK_t *vlanMask, L7_uint32 tagMode)
{
  dot1q_vlan_dataEntry_t *pDataEntry;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32  acquired;
  L7_uint32 vlanId;
  L7_VLAN_MASK_t taggedSet;
  L7_VLAN_MASK_t tempVlanMask;
  vlanNotifyEvent_t event;
  L7_BOOL atleastOneVlan = L7_FALSE;
  DOT1Q_SWPORT_MODE_t currSwPortMode;
  dot1qNotifyData_t vlanTagData;
  L7_uint32 singleTagVlan = 0;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if ( (dot1qIntfIsValid(intIfNum) != L7_TRUE)  || (dot1qIntfIsSettable(intIfNum) != L7_TRUE))
  {
    return;
  }
  COMPONENT_ACQ_NONZEROMASK(dot1qQports[intIfNum].acquiredList, acquired);
  if (acquired)
  {
    return;
  }

  currSwPortMode = dot1qOperSwitchPortModeGet(intIfNum);

  if ((currSwPortMode == DOT1Q_SWPORT_MODE_ACCESS) ||
       (currSwPortMode == DOT1Q_SWPORT_MODE_TRUNK))
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
             "Cannot configure VLAN tagging for an access port %s",
             ifName);
      return;
  }

  memcpy(&tempVlanMask, vlanMask, sizeof(tempVlanMask));

  memset(&taggedSet, 0, sizeof(L7_VLAN_MASK_t));
  memset(&vlanTagData,0,sizeof(dot1qNotifyData_t));

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);

  for (vlanId=1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
  {
    if (L7_VLAN_ISMASKBITSET(tempVlanMask, vlanId))
    {
      pDataEntry = vlanDataSearch(pVlanTree, vlanId);
      if (pDataEntry == L7_NULLPTR)
      {
        /*Entry is NOT present*/
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot modify tagging of interface %s to non existance vlan %d", ifName,  vlanId);
        /*Clear the bit as we need to send notification based on the presence of the bit */
        L7_VLAN_CLRMASKBIT(tempVlanMask, vlanId);
        /*Process the next vlan id*/
        continue;
      }
      if (dot1qIsVlanDynamic(pDataEntry))
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Dynamic VLAN entry %d can only be modified after it is converted to static",pDataEntry->vlanId);
        /*Clear the bit as we need to send notification based on the presence of the bit */
        L7_VLAN_CLRMASKBIT(tempVlanMask, vlanId);
        /*Process the next vlan id*/
        continue;
      }
      if (!NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,intIfNum))
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot modify tagging for port %s for vlan %d as this port is not a member of the vlan",
                ifName,  vlanId);
        /*Clear the bit as we need to send notification based on the presence of the bit */
        L7_VLAN_CLRMASKBIT(tempVlanMask, vlanId);
        /*Process the next vlan id*/
        continue;
      }

      atleastOneVlan = L7_TRUE;

      if (tagMode == L7_DOT1Q_TAGGED)
      {
        NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, intIfNum);
      }
      else
      {
        NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet, intIfNum);
      }
    }/* if is mask bit set*/
  }/* for*/
    /* If we get here then all early continues are complete and the vlan mask has the current vlans*/
    vlanTagData.numVlans++;
    singleTagVlan = vlanId;

  osapiSemaGive(dot1qSem);


  if (atleastOneVlan == L7_FALSE)
  {
    /* No vlans to modify, so no need to call driver hence return*/
    return;
  }

  if (tagMode == L7_DOT1Q_TAGGED)
  {
    memcpy((void *)&taggedSet, (void *)&tempVlanMask, sizeof(L7_VLAN_MASK_t));
  }

  /* We have the exact vlans that need to be modified with tagging properties
   * Lets send it to the lower level
   */
  rc = dtlDot1qConfigInterface(intIfNum, (L7_uchar8 *)&tempVlanMask, (L7_uchar8 *)&taggedSet);

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
            "dtl request failure when modifying tagging for intf %s", ifName);
    return;
  }

  switch(tagMode)
  {
    case L7_DOT1Q_TAGGED:
      event = VLAN_START_TAGGING_PORT_NOTIFY;
      break;
    case L7_DOT1Q_UNTAGGED:
      event = VLAN_STOP_TAGGING_PORT_NOTIFY;
      break;
    default:
      event = 0;
      break;
  }
  if (event != 0)
  {
      if (vlanTagData.numVlans == 1)
      {
          vlanTagData.data.vlanId = singleTagVlan;
      }
      else
      {
          memcpy(&vlanTagData.data.vlanMask, &tempVlanMask, sizeof(L7_VLAN_MASK_t));
      }
    vlanNotifyRegisteredUsers(&vlanTagData,intIfNum,event);
  }
  return;
}
/*********************************************************************
* @purpose  For a given interface sets the membership property for all the vlans in the vlan mask
*
* @param    intIfNum  Internal Interface Number
* @param    vlanMask  Mask of all the vlans affected
* @param    mode      Whether this is to include or exclude or normal this interface on all these vlans
*                       L7_DOT1Q_FIXED
*                       L7_DOT1Q_NORMAL_REGISTRATION
*                       L7_DOT1Q_FORBIDDEN
*
* @returns  None
*
* @notes    1. The vlan Mask supplied represents the delta of the membership,
*           i.e. this is not the final mask, so if this port is member of vlan 10 and 20 and
*           mask contains vlan 24 to be fixed, then this port will be member for vlan 10, 20 and 24
*           Same for the forbidden and normal registration case.
*           Routine assumes that the requestor is DOT1Q_ADMIN
*           Routine assumes that the driver takes this delta and processes only those vlans
*           identified in this mask.
*           2. This function is not called directly by the requestors
*              but the accessVlanSet could be and that calls this function.
*
* @end
*********************************************************************/
void dot1qQportsVlanMembershipMaskProcess(L7_uint32 intIfNum,
                                          L7_VLAN_MASK_t *vlanMask,
                                          L7_uint32 mode,
                                          DOT1Q_SWPORT_MODE_t swport_mode,
                                          L7_uint32 requestor)
{
  dot1q_vlan_dataEntry_t *pDataEntry;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 vlanId;
  L7_VLAN_MASK_t taggedSet;
  L7_VLAN_MASK_t localVlanMask;
  vlanNotifyEvent_t event;
  L7_BOOL atleastOneVlan = L7_FALSE;
  dot1qNotifyData_t vlanData;
  L7_uint32 singleVlan = 0;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if ( (dot1qIntfIsValid(intIfNum) != L7_TRUE)  || (dot1qIntfIsSettable(intIfNum) != L7_TRUE))
  {
    return;
  }

  if (dot1qCanRequestorModifyIntf(intIfNum, requestor) != L7_TRUE)
  {
    return;
  }

  if (dot1qOperSwitchPortModeGet(intIfNum) == DOT1Q_SWPORT_MODE_ACCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
             "Cannot configure VLAN membership for an access port %s",
             ifName);
      return;
  }

  memcpy(&localVlanMask,vlanMask, sizeof(localVlanMask));

  memset(&taggedSet, 0, sizeof(L7_VLAN_MASK_t));
  memset(&vlanData, 0, sizeof(dot1qNotifyData_t));

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);

  for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
  {
    if (L7_VLAN_ISMASKBITSET(localVlanMask, vlanId))
    {

      pDataEntry = vlanDataSearch(pVlanTree, vlanId);
      if (pDataEntry == L7_NULLPTR || pDataEntry->admin == L7_NULLPTR)
      {
        /*Entry is NOT present*/
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Cannot modify membership of interface %s to non existance vlan %d", ifName,  vlanId);
        /*Clear the bit as we need to send notification based on the presence of the bit */
        L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
        /*Process the next vlan id*/
        continue;
      }
      if (dot1qIsVlanDynamic(pDataEntry))
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Dynamic entry %d can only be modified after it is converted to static",pDataEntry->vlanId);
        /*Clear the bit as we need to send notification based on the presence of the bit */
        L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
        /*Process the next vlan id*/
        continue;
      }


      switch (mode)
      {
        case L7_DOT1Q_FIXED:
          if (!NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,intIfNum))
          {
            atleastOneVlan = L7_TRUE;
            vlanData.numVlans++;
            singleVlan = vlanId;
            NIM_INTF_SETMASKBIT(pDataEntry->current.currentEgressPorts,intIfNum);
            L7_VLAN_SETMASKBIT(dot1qIntfVidList[intIfNum],vlanId);
            if ((FD_DOT1Q_DEFAULT_TAGGING == L7_DOT1Q_TAGGED) ||
                (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.taggedSet, intIfNum)))
            {
              L7_VLAN_SETMASKBIT(taggedSet, vlanId);
              NIM_INTF_SETMASKBIT(pDataEntry->current.taggedSet, intIfNum);
            }
          }
          else
          {
            L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
          }
          break;
        case L7_DOT1Q_NORMAL_REGISTRATION:
        case L7_DOT1Q_FORBIDDEN:
          if (NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts,intIfNum))
          {
            atleastOneVlan = L7_TRUE;
            vlanData.numVlans++;
            singleVlan = vlanId;
            NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts,intIfNum);
            L7_VLAN_CLRMASKBIT(dot1qIntfVidList[intIfNum],vlanId);
          }
          else
          {
            L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
          }
          break;
        default:
          break;
      }
    }
  }

  osapiSemaGive(dot1qSem);

  if (atleastOneVlan == L7_FALSE)
  {
    /* No vlans to modify, so no need to call driver hence return*/
    return;
  }

  if (mode == L7_DOT1Q_FIXED)
  {
    rc = dtlDot1qConfigInterface(intIfNum, (L7_uchar8 *)&localVlanMask, (L7_uchar8 *) &taggedSet);
  }
  else
  {
    rc = dtlDot1qUnConfigInterface(intIfNum, (L7_uchar8 *)&localVlanMask);
  }


  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
            "dtl request failure when modifying vlan membership of intIfNum %s", ifName);
    return;
  }

  /* Let every one know of the vlan port modifications */
  switch (mode)
  {
    case L7_DOT1Q_FIXED:
      event = VLAN_ADD_PORT_NOTIFY;
      break;
    case L7_DOT1Q_NORMAL_REGISTRATION:
    case L7_DOT1Q_FORBIDDEN:
      event = VLAN_DELETE_PORT_NOTIFY;
      break;
    default:
      event = 0;
      break;
  }
  if (event != 0)
  {
    if (vlanData.numVlans == 1)
    {
        vlanData.data.vlanId = singleVlan;
    }
    else
    {
        memcpy(&vlanData.data.vlanMask, &localVlanMask, sizeof(L7_VLAN_MASK_t));
    }
    vlanNotifyRegisteredUsers(&vlanData,intIfNum,event);
  }

  return;
}
/*********************************************************************
* @purpose  Create a range of vlans
*
* @param    vlanMask     Vlan ID Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    The vlan mask need not be contiguous
*
*
* @end
*********************************************************************/
void dot1qVlanCreateMaskProcess(L7_VLAN_MASK_t *vlanMask)
{
    dot1q_vlan_dataEntry_t *pDataEntry;
    dot1q_vlan_dataEntry_t createDataEntry;
    L7_VLAN_MASK_t localVlanMask;
    dot1qVlanCfgData_t *vCfg;
    L7_RC_t rc = L7_SUCCESS;
    L7_uint32 currentVlanCount;
    L7_uint32 vlanId = 0;
    L7_uint32 numVlans = 0;
    dot1qNotifyData_t vlanData;
    L7_uint32 singleVlan = 0, vlanCount;

    memset(&vlanData,0,sizeof(dot1qNotifyData_t));
    /* There is no need to protect the following two failure variables as this routine is not re entrant
     * and will always ONLY be called within dot1q task context. There can only be one outstanding call to this routine
     * at any given time
     */
    memset(&dot1qVlanCreateMaskFailure,0,sizeof(L7_VLAN_MASK_t));
    dot1qVlanCreateMaskFailureCount = 0;
    memcpy(&localVlanMask, vlanMask, sizeof(localVlanMask));

    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);

    for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
    {
      if (L7_VLAN_ISMASKBITSET(localVlanMask, vlanId))
      {
        numVlans++;
        if (!DOT1Q_IS_PROBE_MASK_SET(DOT1Q_PROBE_MASK_VLAN_CREATE))
        {
          DOT1Q_DEBUG_MSG("trace not set for %x \n", dot1qProbeMask);
        }
        dot1qDebugProbe(vlanId,1,DOT1Q_PROBE_MASK_VLAN_CREATE);

        DOT1Q_DEBUG_MSG("dot1qVlanCreateMaskProcess : vlan %d \n", vlanId);
        /* Validate the entry */
        if (vlanDataCount(pVlanTree) >= L7_MAX_VLANS)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "Unable to apply VLAN creation request for VLAN ID %d, Database reached MAX VLAN Count!", vlanId);
          rc = L7_FAILURE;
          /* Decrement current count */
          dot1qInfo->CurrentVLANCount--;

          /* If we got this far, and the requestor was DOT1Q_ADMIN, there were requests for
             VLAN creates in the dot1qMsgQueue ahead of this submission.  Clean up the
             allocated vlan cfg entry. */
          if (dot1qVlanConfigEntryReset(vlanId) == L7_SUCCESS)
          {
            /* Set dataChanged flag */
            /* Setting the dataChanged flag outside of the API Set routine and the
               Build/Restore config paths is an anomaly.  This change is purposeful
               herre to account for the fact that the operational database could not be
               configured. This is the only path in the routine where the config file is purposefully
                rolled back.
             */
            dot1qCfg->hdr.dataChanged = L7_TRUE;
           }
           L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
           numVlans--;
           continue;
        }
        /*Search the vlan DB */
        osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
        pDataEntry = vlanDataSearch(pVlanTree, vlanId);
        if (pDataEntry != L7_NULLPTR)
        {
          /*Entry is already present*/
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "Attempt to create a vlan (%d) that already exists",vlanId);
          
          /* Decrement current count */
          dot1qInfo->CurrentVLANCount--;

          /* Reset the allocated vlan cfg entry */
          (void)dot1qVlanConfigEntryReset(vlanId);

          /*dot1qDebugProbeFinish(); */
          L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
          numVlans--;
          osapiSemaGive(dot1qSem);
          continue;
        }

        memset( (char *)&createDataEntry, 0, sizeof(dot1q_vlan_dataEntry_t));

        createDataEntry.vlanId = vlanId;
        osapiUpTime(&(createDataEntry.current.creationTime));
        if (vlanId == DOT1Q_DEFAULT_VLAN )
        {
          createDataEntry.current.status = L7_DOT1Q_TYPE_DEFAULT;
        }
        else
          createDataEntry.current.status = L7_DOT1Q_TYPE_ADMIN;

        /* Point to configuration structure */
        if (dot1qVlanIsConfigurable(vlanId, &vCfg) == L7_TRUE)
        {
          createDataEntry.admin = vCfg;
        }
        else
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Error pointing to DOT1Q config data for VLAN %d", vlanId);
        }
        /* add the entry to the main tree */
        if ( (rc = vlanEntryAdd(pVlanTree, &createDataEntry) ) != L7_SUCCESS )
        {
          /* The effects of this rc should be propagated as there is a severe error*/
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "Error in updating data for VLAN %d in VLAN database", (L7_int32)vlanId);
          rc = L7_FAILURE;

          /* Decrement current count */
          dot1qInfo->CurrentVLANCount--;
          
          /* Reset the allocated vlan cfg entry */
          (void)dot1qVlanConfigEntryReset(vlanId);

          L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
          numVlans--;
          dot1qDebugProbe(vlanId,2,DOT1Q_PROBE_MASK_VLAN_CREATE);
          osapiSemaGive(dot1qSem);
          continue;
        }
        else
        {
          osapiSemaGive(dot1qSem);
          if ( dot1qVlanTransientUpdate(vlanId,L7_NULLPTR,L7_NULLPTR) != L7_SUCCESS )
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                    "Problem updating transient data for VLAN %d", (int)vlanId);
          }

          dot1qDebugProbe(vlanId,2,DOT1Q_PROBE_MASK_VLAN_CREATE);
        }
        singleVlan = vlanId;
      }
    }
    (void)osapiWriteLockGive(dot1qCfgRWLock);

    vlanData.numVlans = numVlans;
    if (vlanData.numVlans == 1)
    {
      vlanData.data.vlanId = singleVlan;
    }
    else
    {
      memcpy(&vlanData.data.vlanMask, &localVlanMask, sizeof(L7_VLAN_MASK_t));
    }

    /* Make the DTL Call with the mask*/
    rc = dtlDot1qCreateMask(&localVlanMask, numVlans, &dot1qVlanCreateMaskFailure, &dot1qVlanCreateMaskFailureCount);

    /* Update statistics */
    if (rc == L7_SUCCESS)
    {
      currentVlanCount = vlanDataCount(pVlanTree);
      if (currentVlanCount > dot1qInfo->MostVLANCount)
      {
        dot1qInfo->MostVLANCount   = currentVlanCount;
      }
    }
    else /* failed create */
    {
      vlanCount = 0;
      osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
      for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
      {
        if (L7_VLAN_ISMASKBITSET(dot1qVlanCreateMaskFailure, vlanId))
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "DTL call to create VLAN %d failed with rc %d", vlanId, (L7_int32)rc);

          /* Delete from the operational structure */
          if (vlanEntryDelete(pVlanTree,vlanId) != L7_SUCCESS)
          {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                      "Problem unrolling data for VLAN %d", (L7_int32)vlanId);
          }

          (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
          /*If this vlan is admin attempted delete this vlan from cfg*/
          /*Any staic vlan create will be reset while any dynamic vlan would return a failure
           *as there is no cfg to begin with. Hence ignore return code
           */
          (void)dot1qVlanConfigEntryReset(vlanId);
          dot1qCfg->hdr.dataChanged = L7_TRUE;
          (void)osapiWriteLockGive(dot1qCfgRWLock);

          /* Decrement current count */
          dot1qInfo->CurrentVLANCount--;
          dot1qInfo->FailedVLANCreations++;
          vlanCount++;
        }
        if (vlanCount == dot1qVlanCreateMaskFailureCount)
        {
          break;
        }
      }
      osapiSemaGive(dot1qSem);
    }

    if (numVlans == dot1qVlanCreateMaskFailureCount)
    {
      /* none of the vlans were created*/
      return;
    }

    if (dot1qVlanCreateMaskFailureCount > 0 && numVlans > dot1qVlanCreateMaskFailureCount)
    {
      L7_VLAN_MASKANDEQINV(localVlanMask, dot1qVlanCreateMaskFailure);
      if ((numVlans - dot1qVlanCreateMaskFailureCount) == 1)
      {
        /*The was only one successfully create vlan*/
        for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
        {
          if (L7_VLAN_ISMASKBITSET(localVlanMask,vlanId))
          {
            vlanData.numVlans = 1;
            vlanData.data.vlanId = vlanId;
            break;
          }
        }
      }
      else
      {
        vlanData.numVlans = numVlans - dot1qVlanCreateMaskFailureCount;
        memcpy(&vlanData.data.vlanMask, &localVlanMask, sizeof(L7_VLAN_MASK_t));
      }
    }

    vlanNotifyRegisteredUsers(&vlanData,0,VLAN_ADD_NOTIFY);

    return;
}
/*********************************************************************
* @purpose  Delete a range of vlans
*
* @param    vlanMask     Vlan ID Mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    The vlan mask need not be contiguous
*
*
* @end
*********************************************************************/
void dot1qVlanDeleteMaskProcess(L7_VLAN_MASK_t *vlanMask)
{
    L7_uint32 rc = L7_FAILURE;
    dot1q_vlan_dataEntry_t *pDataEntry;
    L7_VLAN_MASK_t localVlanMask;
    dot1qNotifyData_t vlanData;
    L7_uint32 singleVlanId = 0;
    L7_uint32 vlanId;

    memset(&vlanData,0,sizeof(dot1qNotifyData_t));

    DOT1Q_DEBUG_MSG("dot1qVlanDeleteMask");
    memcpy(&localVlanMask,vlanMask,sizeof(localVlanMask));
    /*Search the vlan DB */
    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);

    for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
    {
      if (L7_VLAN_ISMASKBITSET(localVlanMask,vlanId))
      {
        pDataEntry = vlanDataSearch(pVlanTree, vlanId);
        if (pDataEntry == L7_NULLPTR)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "Vlan %d does not exist", vlanId);
          L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
          /* Increment the current count */
          dot1qInfo->CurrentVLANCount++;
          continue;
        }
        /* Parmameter checking */
        if (vlanDataCount(pVlanTree)  <= 1)
        {
          (void) trapMgrLastVlanDeleteLastLogTrap(vlanId);
          /* Logs a message while deleting the last vlan
          and does not return a failure and performs the delete operation*/
        }
        if (pDataEntry->current.status == L7_DOT1Q_TYPE_DEFAULT)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                  "Deleting the default VLAN");
        }
        if (pDataEntry->current.status != L7_DOT1Q_TYPE_ADMIN)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "Not valid permision to delete the VLAN %d", vlanId);
          L7_VLAN_CLRMASKBIT(localVlanMask, vlanId);
          /* Increment the current count */
          dot1qInfo->CurrentVLANCount++;
          continue;
        }
        else
        {
          (void) dot1qVlanDeleteOutcalls(vlanId, DOT1Q_ADMIN);
          vlanData.numVlans++;
          singleVlanId = vlanId;
        }
      }
    }
    osapiSemaGive(dot1qSem);
    /* Now the list of vlans is pruned so send out thedelete pending notification */
    if (vlanData.numVlans == 1)
    {
      vlanData.data.vlanId = singleVlanId;
    }
    else
    {
      memcpy(&vlanData.data.vlanMask, vlanMask, sizeof(L7_VLAN_MASK_t));
    }
    if (vlanData.numVlans > 0)
    {
      vlanNotifyRegisteredUsers(&vlanData, 0, VLAN_DELETE_PENDING_NOTIFY);
    }

    /* Now we perform the actual delete */
    /* Make the DTL Call with the mask*/
    rc = dtlDot1qPurgeMask(vlanMask, vlanData.numVlans);

    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
    for (vlanId = 1; vlanId <= L7_VLAN_MAX_MASK_BIT; vlanId++)
    {
      if (L7_VLAN_ISMASKBITSET(localVlanMask,vlanId))
      {
        /* Delete from the operational structure */
        if (vlanEntryDelete(pVlanTree,vlanId) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                  "Problem deleting data for VLAN %d", (L7_int32)vlanId);
        }

        dot1qVlanDeleteIntfCleanup(vlanId);

        if (rc == L7_SUCCESS)
        {
          dot1qInfo->VLANDeletionCount++;
        }
      }
    }
    (void)osapiWriteLockGive(dot1qCfgRWLock);
    osapiSemaGive(dot1qSem);
    if (vlanData.numVlans > 0)
    {
      vlanNotifyRegisteredUsers(&vlanData, 0, VLAN_DELETE_NOTIFY);
    }

    dot1qDebugProbe(singleVlanId,4,DOT1Q_PROBE_MASK_VLAN_DELETE);
    return;
}


/*********************************************************************
* @purpose  Sets the port (intIfNum) as access port for Vlan (vlanId)
*
* @param    intIfNum  Interface number
* @param    vlanId    Access vlan to be set if the switchportmode
*
* @returns  None
*
* @notes  Set vlan 'vlanId' member of interface intf
*         -Set forbidden mask for all other vlans on interface intf -not needed.
*         -set tagging mode to untagged
*         -set pvid for port intf vlan vlanId
*         - set ingress filtering to enable
*         - set acceptable frame type as admit untagged only (new type)
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortAccessModeSet(L7_uint32 intIfNum, L7_uint32 vlanId,
                                     L7_uint32 requestor)
{
   L7_VLAN_MASK_t vlanMask;
   L7_RC_t rc = L7_SUCCESS;
   dot1q_vlan_dataEntry_t *pDataEntry;


   /* range check vlan ID */
   if ( (vlanId <= L7_DOT1Q_NULL_VLAN_ID) || (vlanId > L7_DOT1Q_MAX_VLAN_ID) )
   {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
               "Attempt to set access vlan with an invalid vlan id %d", (int)vlanId);
       return L7_FAILURE;
   }


   osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);

   pDataEntry = vlanDataSearch(pVlanTree, vlanId);
   if (pDataEntry == L7_NULLPTR)
   {
       /*Entry is already present*/
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
               "Attempt to set access vlan with (%d) that does not exist",vlanId);
       osapiSemaGive(dot1qSem);

       /*dot1qDebugProbeFinish(); */
       return L7_FAILURE;
   }

   osapiSemaGive(dot1qSem);

   /* Set the switchport mode to none so that we can use the vlan membership functions */
   dot1qQports[intIfNum].switchport_mode = DOT1Q_SWPORT_MODE_NONE;

   memset(&vlanMask,0, sizeof(vlanMask));
   L7_VLAN_SETMASKBIT(vlanMask, vlanId);

   /* delete all other vlans from this port*/
   /* Since this is within the protocol the switchport mode does not matter. call with none*/
   dot1qQportsVlanMembershipMaskProcess(intIfNum,&dot1qIntfVidList[intIfNum],
                                        L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE, requestor);
   /* Clear VlanForbiddenEgressPorts for the access VLAN */
   if(pDataEntry->admin != L7_NULL)
   {
      NIM_INTF_CLRMASKBIT(pDataEntry->admin->vlanCfg.forbiddenEgressPorts, intIfNum);
   }

   /*Add this vlan as the member for this port*/
   dot1qQportsVlanMembershipMaskProcess(intIfNum,&vlanMask,L7_DOT1Q_FIXED,
                                        DOT1Q_SWPORT_MODE_NONE, requestor);


   /*No need for range command since the vlan membership has been removed for all other ports*/
   rc = dot1qVlanTaggedMemberSetModify(vlanId,intIfNum, L7_DOT1Q_UNTAGGED, requestor);
   if (rc != L7_SUCCESS)
   {
       /* TBD rollback*/
       return rc;
   }
   rc = dot1qQportsVlanParamSet(intIfNum,vlanId,L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED,L7_ENABLE);

   dot1qQports[intIfNum].switchport_mode = DOT1Q_SWPORT_MODE_ACCESS;
   dot1qQports[intIfNum].access_vlan = vlanId;

   return rc;

}


/*********************************************************************
* @purpose  Sets the port (intIfNum) as general port.
*
* @param    intIfNum  Interface number
*
* @returns  None
*
* @notes  Sets the intf in default configuration
*         1. Removes all Vlans except default vlan from the port
*         2. Makes 1 untagging ,
*         3. PVID default vlan, ingress filtering disabled, accept all frames
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortGeneralModeSet(L7_uint32 intIfNum, L7_BOOL setupDefaults)
{
    L7_VLAN_MASK_t vlanMask;
    L7_RC_t rc = L7_SUCCESS;
    dot1qIntfCfgData_t *pCfg = L7_NULL;
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    DOT1Q_TRACE("%s(%d) intf %d, %s \n", __FUNCTION__, __LINE__, intIfNum, ifName);
    if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    {
      return L7_ERROR;
    }

   /* Set the switchport mode to none so that we can use the vlan membership functions */
    dot1qQports[intIfNum].switchport_mode = DOT1Q_SWPORT_MODE_NONE;

    if (setupDefaults == L7_TRUE)
    {

      memset(&vlanMask, 0, sizeof(vlanMask));
      L7_VLAN_SETMASKBIT(vlanMask, FD_DOT1Q_DEFAULT_VLAN);

      /* delete all other vlans from this port*/
      dot1qQportsVlanMembershipMaskProcess(intIfNum, &dot1qIntfVidList[intIfNum],
                                           L7_DOT1Q_FORBIDDEN,
                                           DOT1Q_SWPORT_MODE_NONE, DOT1Q_ADMIN);
         /*Add this vlan as the member for this port*/
      dot1qQportsVlanMembershipMaskProcess(intIfNum, &vlanMask, L7_DOT1Q_FIXED,
                                           DOT1Q_SWPORT_MODE_GENERAL, DOT1Q_ADMIN);
    }

    rc = dot1qQportsVlanParamSet(intIfNum,pCfg->portCfg.VID[DOT1Q_DEFAULT_PVID],
                                 pCfg->portCfg.acceptFrameType,
                                 pCfg->portCfg.enableIngressFiltering);

    dot1qQports[intIfNum].ingressFilter = pCfg->portCfg.enableIngressFiltering;
    dot1qQports[intIfNum].pvid[DOT1Q_DEFAULT_PVID] =
        pCfg->portCfg.VID[DOT1Q_DEFAULT_PVID];

    dot1qQports[intIfNum].switchport_mode = DOT1Q_SWPORT_MODE_GENERAL;
    dot1qQports[intIfNum].access_vlan = L7_DOT1Q_NULL_VLAN_ID;

    return rc;
}



/*********************************************************************
* @purpose  Sets a port to Trunk Port Mode
*
* @param
* @param    intIfNum  intIfNumber
*
* @returns  L7_SUCCESS on success L7_FAILURE otherwise
*
* @notes
*           -Set acceptable frame type as tagged only
*           -Remove this interface from all the vlans
*           -Enable ingress filtering
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortTrunkModeSet(L7_uint32 intIfNum)
{
   L7_VLAN_MASK_t vlanMask;
   L7_RC_t rc;
   L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
   nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

   DOT1Q_TRACE("%s(%d) intf %d, %s \n", __FUNCTION__, __LINE__, intIfNum, ifName);
   /* Set the switchport mode to none so that we can use the vlan membership functions */
   dot1qQports[intIfNum].switchport_mode = DOT1Q_SWPORT_MODE_NONE;

   /* Build the vlan list for ports to set */
   memset(&vlanMask, 0, sizeof(vlanMask));

   /* Delete the vlans participating in this port  */
   dot1qQportsVlanMembershipMaskProcess(intIfNum,&dot1qIntfVidList[intIfNum],
                                        L7_DOT1Q_NORMAL_REGISTRATION,
                                        DOT1Q_SWPORT_MODE_NONE, DOT1Q_ADMIN);


   /* Even though pvid is not valid for trunk mode
      initialize it to the default vlan so we receive control frames on trunk port
   */
   rc = dot1qQportsVlanParamSet(intIfNum, L7_DOT1Q_DEFAULT_VLAN,
                                L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED,
                                L7_ENABLE);
   /*dot1qQportsVlanTagMaskProcess(intIfNum, vlanMask, L7_DOT1Q_TAGGED);*/

   dot1qQports[intIfNum].switchport_mode = DOT1Q_SWPORT_MODE_TRUNK;
   dot1qQports[intIfNum].access_vlan = L7_DOT1Q_NULL_VLAN_ID;
   dot1qQports[intIfNum].pvid[DOT1Q_DEFAULT_PVID] = L7_DOT1Q_DEFAULT_VLAN;

   return rc;
}
/*********************************************************************
* @purpose  For a given vlan sets the swport mode property for the range of intfs
*           supplied in intf mask
*
* @param    The Switchport mode to be set for the interfaces
                                                          DOT1Q_SWPORT_MODE_GENERAL,
                                                          DOT1Q_SWPORT_MODE_ACCESS,
                                                          DOT1Q_SWPORT_MODE_TRUNK
* @param    intfMask      Mask of all the vlans affected
*           setupDefaults A bool variable indicating if the defaults for the mode are
*                         configured. (Currently used only by general mode)
*
* @returns  None
*
* @notes    If the config is being applied then, all the port properties would be seperately
*           configured, hence there is no need to set up the defaults for the mode.
*
* @end
*********************************************************************/
L7_RC_t dot1qSwitchPortModeProcess(NIM_INTF_MASK_t *mask, DOT1Q_SWPORT_MODE_t mode, L7_BOOL setupDefaults)
{
    NIM_INTF_MASK_t currMask;
    L7_uint32 curr;
    L7_uint32  acquired;
    L7_RC_t rc;

    memcpy(&currMask, mask, sizeof(currMask));
    NIM_INTF_FHMASKBIT(currMask,curr);

    while ( curr != 0 )
    {
        COMPONENT_ACQ_NONZEROMASK(dot1qQports[curr].acquiredList, acquired);
        if ( (dot1qIntfIsValid(curr)== L7_TRUE)  && (dot1qIntfIsSettable(curr) == L7_TRUE) && (acquired != L7_TRUE))
        {
            if (mode != dot1qQports[curr].switchport_mode)
            {
                switch (mode)
                {
                  case DOT1Q_SWPORT_MODE_ACCESS:
                      rc = dot1qSwitchPortAccessModeSet(curr,FD_DOT1Q_DEFAULT_VLAN, DOT1Q_ADMIN);
                      break;
                  case DOT1Q_SWPORT_MODE_TRUNK:
                      rc = dot1qSwitchPortTrunkModeSet(curr);
                      break;
                  case DOT1Q_SWPORT_MODE_GENERAL:
                      rc = dot1qSwitchPortGeneralModeSet(curr, setupDefaults);
                      dot1qQports[curr].switchport_mode = mode;
                      /*No special setting for general mode*/
                      break;
                  default:
                    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                            "Incorrect mode entered");
                    break;

                }
            }

        }

        NIM_INTF_CLRMASKBIT(currMask,curr);
        /* Get the next valid interface */
        NIM_INTF_FHMASKBIT(currMask,curr);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if the requestor can act on this interface
*
* @param    intIfNum
*           requestor
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    If the interface is acquired by DOT1X only DOT1Q_DOT1X requestor
*           can act on the interface.
*
* @end
*********************************************************************/
L7_BOOL dot1qCanRequestorModifyIntf(L7_uint32 intIfNum, vlanRequestor_t requestor)
{
  L7_uint32  acquired;

  /* No one has acquired this interface. Anybody can access it*/
  COMPONENT_ACQ_NONZEROMASK(dot1qQports[intIfNum].acquiredList, acquired);
  if (acquired == L7_FALSE)
  {
    return L7_TRUE;
  }

  switch (requestor)
  {
    case DOT1Q_DOT1X:
      /* DOt1X is the requestor. It can only modify interface properties
         if it has acquired the port.
       */
      return (COMPONENT_ISMASKBITSET(dot1qQports[intIfNum].acquiredList,
                             L7_DOT1X_COMPONENT_ID)!=0);

    default:
      return L7_FALSE;

  }

}
