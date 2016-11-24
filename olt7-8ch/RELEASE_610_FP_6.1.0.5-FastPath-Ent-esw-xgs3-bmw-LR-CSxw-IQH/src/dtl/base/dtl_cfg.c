/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_cfg.c
*
* Purpose: This file contains the functions to configure the DTL.
*
* Component: Device Transformation Layer (DTL)
*
* Commnets:
*
* Created by: Shekhar Kalyanam 3/14/2001
*
*********************************************************************/

/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_CFG_GLOBALS               /* Enable global space   */
#include "dtlinclude.h"
#include "dtl_pkg.h"
#include "dtl_sid.h"
#include "dtl_cnfgr.h"
#include "dot1s_api.h"

#if DTLCTRL_COMPONENT_CFG

dtlInfo_t         dtlInfo;

void *dtlLockSem;
void *dtlL2McastSema;

/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES
**********************************************************************
*/

/*
**********************************************************************
*                           API FUNCTIONS
**********************************************************************
*/
/*********************************************************************
* @purpose  create the message queue and wait for messages
*
* @param    taskId    {(output)}  The task id for the dtl task
*
* @returns  L7_FAILURE - if unable to create the OS message queue
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlTask()
{
  L7_RC_t     mrc;
  L7_ushort16   Message;
  L7_uint32 dtl_msgq_size;

  dtl_msgq_size = DTL_MSG_COUNT * sizeof (DTL_MSG_t);
  dtlMsg = osapiMalloc (L7_DTL_COMPONENT_ID, dtl_msgq_size);
  if (dtlMsg == 0)
  {
    LOG_ERROR (0);
  }
  memset (dtlMsg, 0, dtl_msgq_size);

  dtlMsgQueue = (void *)osapiMsgQueueCreate(DTL_QUEUE,DTL_MSG_COUNT,DTL_MSG_SIZE);
  if (dtlMsgQueue == L7_NULLPTR)
  {
    LOG_MSG("DTL: Unable to create msg queue for dtlTask\n");
    return(L7_FAILURE);
  }

  /* dtlMsgTail and dtlMsgHead initialized to zero on module load */

  do
  {
    bzero((L7_char8 *)&Message, sizeof(L7_ushort16));
    if ((mrc = osapiMessageReceive(dtlMsgQueue, (void *)&Message,
                                   DTL_MSG_SIZE, L7_WAIT_FOREVER)) != L7_SUCCESS)
    {
      LOG_MSG("DTL: dtlTask msg receive error, status = %x\n",mrc);
      continue;
    }
    /*enter critical section*/
    dtlEnterCS();

    if (dtlMsgTail != dtlMsgHead)
    {
      dtlMsgHead = (dtlMsgHead+1)%DTL_MSG_COUNT;

      /*exit critical section*/
      dtlExitCS();

      switch (dtlMsg[Message].cmdFamilyType)
      {
        case DAPI_FAMILY_INTF_MGMT:
          dtlLinkChange(&(dtlMsg[Message].ddusp),
                        dtlMsg[Message].cmdFamilyType,
                        dtlMsg[Message].cmdType,
                        dtlMsg[Message].eventType,
                        &(dtlMsg[Message].cmdData.dapiIntfEventInfo));
          break;
        case DAPI_FAMILY_FRAME:
          dtlPduReceive(&(dtlMsg[Message].ddusp),
                        dtlMsg[Message].cmdFamilyType,
                        dtlMsg[Message].cmdType,
                        dtlMsg[Message].eventType,
                        &(dtlMsg[Message].cmdData.dapiFrameEventInfo));
          break;
        default:
          continue;
      }
    }
    else
    {
      /*exit critical section*/
      dtlExitCS();
    }
  }while (1);
}

/*********************************************************************
* @purpose  MAC address add/delete and port mac locking violations.
*
* @param    none
*
* @returns  none - if unable to create the OS message queue
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlTaskAddr()
{
  L7_RC_t     mrc;
  DTL_MSG_t   Message;

  dtlAddrMsgQueue = (void *)osapiMsgQueueCreate(DTL_ADDR_QUEUE,
                                                DTL_ADDR_MSG_COUNT,
                                                sizeof (DTL_MSG_t));
  if (dtlAddrMsgQueue == L7_NULLPTR)
  {
    LOG_ERROR (0);
  }

  do
  {
    if ((mrc = osapiMessageReceive(dtlAddrMsgQueue,
                                   (void *)&Message,
                                   sizeof (Message), L7_WAIT_FOREVER)) != L7_SUCCESS)
    {
      LOG_ERROR (mrc);
    }

   #if defined(FEAT_METRO_CPE_V1_0)
     if (!dtlIsReady())
     {
       continue;
     }
   #endif


    switch (Message.cmdFamilyType)
    {
      case DAPI_FAMILY_ADDR_MGMT:
        dtlFdbReceive(&Message.ddusp,
                      Message.cmdFamilyType,
                      Message.cmdType,
                      Message.eventType,
                      &(Message.cmdData.dapiAddrEventInfo));
        break;
      default:
        continue;
    }
  }while (1);

  return L7_ERROR;
}

/*********************************************************************
* @purpose   Create the dtl task
*
* @param        @b{(void)}
*
* @returns      L7_SUCCESS  on successful Task creation
* @returns      L7_ERROR    on unsuccessful Task creation
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlStartTask(L7_uint32 *taskId)
{
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32   dtlAddrTask;

  if ((dtlLockSem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == NULL)
  {
    LOG_MSG("DTL: Could not create dtlLockSem\n");
    return(L7_ERROR);
  }

  *taskId = osapiTaskCreate("dtlTask",dtlTask,0,L7_NULLPTR,
                            dtlSidTaskStackSizeGet(),
                            dtlSidTaskPriorityGet(),
                            dtlSidTaskSliceGet());

  if (*taskId == L7_ERROR)
  {
    LOG_MSG("DTL: Could not create dtlTask\n");
    rc = L7_ERROR;
  }

  bzero((L7_char8 *)&dtlInfo, sizeof(dtlInfo_t));

  if ((dtlInfo.routeEntrySem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == NULL)
  {
    LOG_MSG("DTL: Could not create dtlInfo.routeEntrySem\n");
    return(L7_ERROR);
  }

  dtlL2McastSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dtlL2McastSema == L7_NULL)
  {
    LOG_MSG("Unable to create dtlL2McastSema\n");
    return L7_ERROR;
  }

  dtlAddrTask = osapiTaskCreate("dtlAddrTask",dtlTaskAddr,0,L7_NULLPTR,
                                dtlSidTaskStackSizeGet(),
                                dtlSidTaskPriorityGet(),
                                dtlSidTaskSliceGet());

  if (dtlAddrTask == L7_ERROR)
  {
    LOG_ERROR (0);
  }

  return(rc);
}

/*********************************************************************
* @purpose  Generic dtl callback registration
*
* @param    family       @b{(input)} DTL family type
*                           DTL_FAMILY_CONTROL,
*                           DTL_FAMILY_SYSTEM,
*                           DTL_FAMILY_INTF_MGMT,
*                           DTL_FAMILY_LAG_MGMT,
*                           DTL_FAMILY_ADDR_MGMT,
*                           DTL_FAMILY_QVLAN_MGMT,
*                           DTL_FAMILY_GARP_MGMT,
*                           DTL_FAMILY_ROUTING_MGMT,
*                           DTL_FAMILY_ROUTING_ARP_MGMT,
*                           DTL_FAMILY_ROUTING_INTF_MGMT,
*                           DTL_FAMILY_FRAME
* @param    (*funcPtr)   @b{(input)} Pointer to the function to be registered
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    passes this information to the device driver for actual registration
* @notes    function must be of type (*DAPI_CALLBACK_t)
*
* @end
*********************************************************************/
L7_RC_t dtlCallbackRegistration(DTL_FAMILY_t family,
                                L7_RC_t (*funcPtr)(DAPI_USP_t *usp, DAPI_FAMILY_t family, DAPI_CMD_t cmd, DAPI_EVENT_t event, void *eventInfoPtr))
{
  DAPI_CONTROL_CALLBACK_CMD_t cmdInfo;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;
  L7_RC_t rc = L7_FAILURE;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  switch (family)
  {
    case DTL_FAMILY_CONTROL:
      {
        cmdInfo.family = DAPI_FAMILY_CONTROL;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_SYSTEM:
      {
        cmdInfo.family = DAPI_FAMILY_SYSTEM;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_INTF_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_INTF_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_LAG_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_LAG_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_ADDR_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_ADDR_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_QVLAN_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_QVLAN_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_GARP_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_GARP_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_ROUTING_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_ROUTING_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_ROUTING_ARP_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_ROUTING_ARP_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_ROUTING_INTF_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_ROUTING_INTF_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_FRAME:
      {
        cmdInfo.family = DAPI_FAMILY_FRAME;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_QOS_ACL_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_QOS_ACL_MGMT;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    case DTL_FAMILY_SERVICES:
      {
        cmdInfo.family = DAPI_FAMILY_SERVICES;
        cmdInfo.funcPtr = funcPtr;
        break;
      }
    default:
      return L7_FAILURE;
  }

  dr = dapiCtl(&ddUsp, DAPI_CMD_CONTROL_CALLBACK_REGISTER, &cmdInfo);

  if (dr == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Generic dtl callback de-registration
*
* @param    family       @b{(input)} DTL family type
*                           DTL_FAMILY_CONTROL,
*                           DTL_FAMILY_SYSTEM,
*                           DTL_FAMILY_INTF_MGMT,
*                           DTL_FAMILY_LAG_MGMT,
*                           DTL_FAMILY_ADDR_MGMT,
*                           DTL_FAMILY_QVLAN_MGMT,
*                           DTL_FAMILY_GARP_MGMT,
*                           DTL_FAMILY_ROUTING_MGMT,
*                           DTL_FAMILY_ROUTING_ARP_MGMT,
*                           DTL_FAMILY_ROUTING_INTF_MGMT,
*                           DTL_FAMILY_FRAME
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    passes this information to the device driver for actual registration
* @notes    function must be of type (*DAPI_CALLBACK_t)
*
* @end
*********************************************************************/
L7_RC_t dtlCallbackDeRegistration(DTL_FAMILY_t family)
{
  DAPI_CONTROL_CALLBACK_CMD_t cmdInfo;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;
  L7_RC_t rc = L7_FAILURE;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  cmdInfo.funcPtr = L7_NULLPTR;

  switch (family)
  {
    case DTL_FAMILY_CONTROL:
      {
        cmdInfo.family = DAPI_FAMILY_CONTROL;
        break;
      }
    case DTL_FAMILY_SYSTEM:
      {
        cmdInfo.family = DAPI_FAMILY_SYSTEM;
        break;
      }
    case DTL_FAMILY_INTF_MGMT:
      {

        cmdInfo.family = DAPI_FAMILY_INTF_MGMT;
        break;
      }
    case DTL_FAMILY_LAG_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_LAG_MGMT;
        break;
      }
    case DTL_FAMILY_ADDR_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_ADDR_MGMT;
        break;
      }
    case DTL_FAMILY_QVLAN_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_QVLAN_MGMT;
        break;
      }
    case DTL_FAMILY_GARP_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_GARP_MGMT;
        break;
      }
    case DTL_FAMILY_ROUTING_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_ROUTING_MGMT;
        break;
      }
    case DTL_FAMILY_ROUTING_ARP_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_ROUTING_ARP_MGMT;
        break;
      }
    case DTL_FAMILY_ROUTING_INTF_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_ROUTING_INTF_MGMT;
        break;
      }
    case DTL_FAMILY_FRAME:
      {
        cmdInfo.family = DAPI_FAMILY_FRAME;
        break;
      }
    case DTL_FAMILY_QOS_ACL_MGMT:
      {
        cmdInfo.family = DAPI_FAMILY_QOS_ACL_MGMT;
        break;
      }
    case DTL_FAMILY_SERVICES:
      {
        cmdInfo.family = DAPI_FAMILY_SERVICES;
        break;
      }
    default:
      return L7_FAILURE;
  }

  dr = dapiCtl(&ddUsp, DAPI_CMD_CONTROL_CALLBACK_REGISTER, &cmdInfo);

  if (dr != L7_FAILURE)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Registers functions with DTL which are called depending on
*           the type of event they register for.
*
* @param    dtlFamily @b{(input)} DTL family type
*                                 {DTL_FAMILY_CONTROL = 1,
*                                  DTL_FAMILY_SYSTEM,
*                                  DTL_FAMILY_INTF_MGMT,
*                                  DTL_FAMILY_LAG_MGMT,
*                                  DTL_FAMILY_ADDR_MGMT,
*                                  DTL_FAMILY_QVLAN_MGMT,
*                                  DTL_FAMILY_GARP_MGMT,
*                                  DTL_FAMILY_ROUTING_MGMT,
*                                  DTL_FAMILY_ROUTING_ARP_MGMT,
*                                  DTL_FAMILY_ROUTING_INTF_MGMT,
*                                  DTL_FAMILY_FRAME
*                                  }
* @param    dtlEvent  @b{(input)} Type of event for which the function is registered
*                                 {DTL_EVENT_FRAME_RX_ARP,
*                                  DTL_EVENT_FRAME_RX_IP,
*                                  DTL_EVENT_FRAME_RX_DOT1D,
*                                  DTL_EVENT_FRAME_RX_GARP,
*                                  DTL_EVENT_INTF,
*                                  DTL_EVENT_ADDR,
*                                  DTL_EVENT_QVLAN_MEMBER_QUERY,
*                                  DTL_EVENT_ROUTING_ARP_ENTRY_QUERY,
*                                  }
* @param    funcPtr   @b{(input)} Pointer to the function that is registered
*
* @returns  L7_SUCCESS on success
* @returns  L7_FAILURE on failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlGenericRegistration(DTL_FAMILY_t dtlFamily,
                               DTL_EVENT_t dtlEvent,
                               L7_FUNCPTR_t funcPtr)
{
  L7_uint32 rc;

  if (dtlCnfgrStarted() == L7_FALSE)
  {
    /* Boxes that are not capable of supporting management function
    ** hit this code.
    */
    return L7_SUCCESS;
  }

  rc = dtlRegistryNodeAdd(dtlEventHandle[dtlEvent],funcPtr);
  if (rc == L7_FAILURE)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deregisters functions to receive notifications
*
* @param    dtlFamily @b{(input)} DTL family type
*                                 {DTL_FAMILY_CONTROL = 1,
*                                  DTL_FAMILY_SYSTEM,
*                                  DTL_FAMILY_INTF_MGMT,
*                                  DTL_FAMILY_LAG_MGMT,
*                                  DTL_FAMILY_ADDR_MGMT,
*                                  DTL_FAMILY_QVLAN_MGMT,
*                                  DTL_FAMILY_GARP_MGMT,
*                                  DTL_FAMILY_ROUTING_MGMT,
*                                  DTL_FAMILY_ROUTING_ARP_MGMT,
*                                  DTL_FAMILY_ROUTING_INTF_MGMT,
*                                  DTL_FAMILY_FRAME
*                                  }
* @param    dtlEvent  @b{(input)} Type of event for which the function is deregistered
*                                 {DTL_EVENT_FRAME_RX_ARP,
*                                  DTL_EVENT_FRAME_RX_IP,
*                                  DTL_EVENT_FRAME_RX_DOT1D,
*                                  DTL_EVENT_FRAME_RX_GARP,
*                                  DTL_EVENT_INTF,
*                                  DTL_EVENT_ADDR,
*                                  DTL_EVENT_QVLAN_MEMBER_QUERY,
*                                  }
* @param    funcPtr   @b{(input)} Pointer to the function that is deregistered
*
* @returns  L7_SUCCESS on success
* @returns  L7_FAILURE on failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlGenericDeregistration(DTL_FAMILY_t dtlFamily,
                                 DTL_EVENT_t dtlEvent,
                                 L7_FUNCPTR_t funcPtr)
{
  DTL_FUNC_REG_t  *regNodePtr;
  L7_RC_t rc;
  DTL_REG_CTRL_t *handle;

  handle = dtlEventHandle[dtlEvent];

  regNodePtr = dtlRegistryNodeFind(handle, funcPtr);
  if (regNodePtr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  rc = dtlRegistryNodeDelete(handle, regNodePtr);
  if (rc==L7_SUCCESS)
  {
    handle->count--;
  }

  return rc;
}

/*********************************************************************
* @purpose  Receive callback from the driver and based on registrations with DTL
* @purpose  call the function/s registered for the event
*
* @param    *ddusp         @b{(input)} Device driver reference to unit slot and port
* @param    family         @b{(input)} Device Driver family type
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    event          @b{(input)} Event of type DAPI_EVENT_t
* @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlGenericCallback(DAPI_USP_t *ddusp,
                           DAPI_FAMILY_t family,
                           DAPI_CMD_t cmd,
                           DAPI_EVENT_t event,
                           void *dapiEventInfo)
{
  L7_uint32 intfNum;
  nimUSP_t nimUsp;
  void *data;
  L7_uint32 numOfElements;
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 numOfFunc;
  DTL_FUNC_REG_t *currentReg;
  L7_uint32 dot1sState;
  dot1s_stateCB_t status;

  nimUsp.unit = ddusp->unit;
  nimUsp.slot = ddusp->slot;
  nimUsp.port = ddusp->port +1;

  if (nimGetIntIfNumFromUSP(&nimUsp, &intfNum) != L7_SUCCESS)
  {
    LOG_MSG("invalid DDusp\n");
    return L7_FAILURE;
  }

  switch (event)
  {
    case DAPI_EVENT_ADDR_INTF_MAC_QUERY:
      {
        numOfFunc =  dtlEventHandle[DTL_EVENT_ADDR_INTF_MAC_QUERY]->count;
        if (numOfFunc > 0)
        {
          currentReg =  dtlEventHandle[DTL_EVENT_ADDR_INTF_MAC_QUERY]->first;
          do
          {
            rc = (*((DTL_ATTR_FUNC_t)currentReg->funcPtr))(intfNum,
                                                           DTL_EVENT_ADDR_INTF_MAC_QUERY,
                                                           &(((DAPI_ADDR_MGMT_CMD_t *)dapiEventInfo)->cmdData.unsolIntfMacQuery.macAddr),
                                                           &numOfElements);
            currentReg = currentReg->next;
          } while (currentReg != L7_NULLPTR);
        }

        if (rc != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        else
          return L7_SUCCESS;

        break;
      }

    case DAPI_EVENT_QVLAN_MEMBER_QUERY:
      {
        data = (void *)osapiMalloc(L7_DTL_COMPONENT_ID, L7_MAX_VLANS * sizeof(dot1qVidList_t));
        if ((dot1qVidList_t *)data == L7_NULLPTR)
        {
          return L7_FAILURE;
        }

        rc = L7_FAILURE;

        numOfFunc =  dtlEventHandle[DTL_EVENT_QVLAN_MEMBER_QUERY]->count;

        if (numOfFunc > 0)
        {
          currentReg =  dtlEventHandle[DTL_EVENT_QVLAN_MEMBER_QUERY]->first;
          do
          {
            rc = (*((DTL_ATTR_FUNC_t)currentReg->funcPtr))(intfNum,
                                                           DTL_EVENT_QVLAN_MEMBER_QUERY,
                                                           data,
                                                           &numOfElements);
            currentReg = currentReg->next;
          } while (currentReg != L7_NULLPTR);
        }

        if (rc != L7_SUCCESS)
        {
          osapiFree(L7_DTL_COMPONENT_ID, data);
          return L7_FAILURE;
        }
        for (i=0 ; i < numOfElements ; i++)
        {
          ((DAPI_QVLAN_MGMT_CMD_t *)dapiEventInfo)->cmdData.unsolMemberQuery.vlanID[i] = (L7_ushort16)(((dot1qVidList_t *)data)[i].vid);
          ((DAPI_QVLAN_MGMT_CMD_t *)dapiEventInfo)->cmdData.unsolMemberQuery.lagTagged[i] = (L7_BOOL    )(((dot1qVidList_t *)data)[i].tag);
        }
        (((DAPI_QVLAN_MGMT_CMD_t *)dapiEventInfo)->cmdData.unsolMemberQuery.numOfEntries) = numOfElements;

        osapiFree(L7_DTL_COMPONENT_ID, data);
        return L7_SUCCESS;

        break;
      }

    case DAPI_EVENT_INTF_DOT1S_STATE_QUERY:
      {
        numOfFunc =  dtlEventHandle[DTL_EVENT_PORT_DOT1S_STATE_QUERY]->count;
        if (numOfFunc > 0)
        {
          currentReg =  dtlEventHandle[DTL_EVENT_PORT_DOT1S_STATE_QUERY]->first;
          do
          {
            rc = (*((DTL_ATTR_FUNC_t)currentReg->funcPtr))(intfNum,
                                                           DTL_EVENT_PORT_DOT1S_STATE_QUERY,
                                                           &(((DAPI_INTF_MGMT_CMD_t *)dapiEventInfo)->cmdData.dot1sQuery.instNumber),
                                                           (void *)&(((DAPI_INTF_MGMT_CMD_t *)dapiEventInfo)->cmdData.dot1sQuery.canApplyStateChange));
            currentReg = currentReg->next;
          } while (currentReg != L7_NULLPTR);
        }

        if (rc != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        else
        {
          return L7_SUCCESS;
        }

        break;
      }
    case DAPI_EVENT_SERVICES_VLAN_TAG_QUERY:
      {
        numOfFunc =  dtlEventHandle[DTL_EVENT_SERVICES_VLAN_TAG_QUERY]->count;
        if (numOfFunc > 0)
        {
          currentReg =  dtlEventHandle[DTL_EVENT_SERVICES_VLAN_TAG_QUERY]->first;
          do
          {
            rc = (*((DTL_ATTR_FUNC_t)currentReg->funcPtr))(intfNum, DTL_EVENT_SERVICES_VLAN_TAG_QUERY,
                                                           dapiEventInfo, 0);
            currentReg = currentReg->next;
          } while (currentReg != L7_NULLPTR);
        }

        if (rc != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        else
        {
          return L7_SUCCESS;
        }
        break;
      }

    case DAPI_EVENT_INTF_DOT1S_STATE_SET_RESPONSE:
      {
        numOfFunc =  dtlEventHandle[DTL_EVENT_PORT_DOT1S_STATE_SET_RESPONSE]->count;
        switch (((DAPI_INTF_MGMT_CMD_t *)dapiEventInfo)->cmdData.dot1sState.state)
        {
          case DAPI_PORT_DOT1S_DISCARDING:
            dot1sState = L7_DOT1S_DISCARDING;
            break;
          case DAPI_PORT_DOT1S_LEARNING:
            dot1sState = L7_DOT1S_LEARNING;
            break;
          case DAPI_PORT_DOT1S_FORWARDING:
            dot1sState = L7_DOT1S_FORWARDING;
            break;
          case DAPI_PORT_DOT1S_ADMIN_DISABLED:
            dot1sState = L7_DOT1S_DISABLED;
            break;
          case DAPI_PORT_DOT1S_NOT_PARTICIPATING:
            dot1sState = L7_DOT1S_MANUAL_FWD;
            break;
          default:
            dot1sState = 0;
            break;
        }
        status.mpc = (((DAPI_INTF_MGMT_CMD_t *)dapiEventInfo)->cmdData.dot1sState.applicationReference);
        status.rc = (((DAPI_INTF_MGMT_CMD_t *)dapiEventInfo)->cmdData.dot1sState.rc);
        if (numOfFunc > 0)
        {
          currentReg =  dtlEventHandle[DTL_EVENT_PORT_DOT1S_STATE_SET_RESPONSE]->first;
          do
          {
            rc = (*(currentReg->funcPtr))(intfNum,
                                          (((DAPI_INTF_MGMT_CMD_t *)dapiEventInfo)->cmdData.dot1sState.instNumber),
                                          dot1sState,
                                          status);
            currentReg = currentReg->next;
          } while (currentReg != L7_NULLPTR);
        }

        if (rc != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        else
          return L7_SUCCESS;
        break;
      }
    default:
      return L7_SUCCESS;
  }

  return L7_SUCCESS;
}

/*
**********************************************************************
*                           PRIVATE FUNCTIONS
**********************************************************************
*/

/*********************************************************************
* @purpose  Disables interrupts and locks tasks
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
void dtlEnterCS(void)
{
  L7_RC_t rc;

  if ((rc = osapiSemaTake(dtlLockSem, L7_WAIT_FOREVER)) != L7_SUCCESS)
  {
    LOG_ERROR(rc);
  }
}

/*********************************************************************
* @purpose  Unlocks tasks and enables interrupts
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
void dtlExitCS(void)
{
  L7_RC_t rc;

  if ((rc = osapiSemaGive(dtlLockSem)) != L7_SUCCESS)
  {
    LOG_ERROR(rc);
  }
}

/*********************************************************************
* @purpose  creates the first node in the registry
*
* @param    none
*
* @returns  pointer to the first node on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
DTL_FUNC_REG_t *dtlRegistryNodeFirstGet()
{
  DTL_FUNC_REG_t  *address;

  address = (DTL_FUNC_REG_t *) osapiMalloc(L7_DTL_COMPONENT_ID, sizeof(DTL_FUNC_REG_t));
  if (address == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  else
  {
    address->next = L7_NULLPTR;
    address->prev = L7_NULLPTR;
  }
  return address;
}

/*********************************************************************
* @purpose  allocates a new node entry in the registry
*
* @param    *lastRegNodePtr   @b{(input)} pointer tot he last node in the registry
*
* @returns  pointer to new node on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
DTL_FUNC_REG_t *dtlRegistryNodeNextGet(DTL_FUNC_REG_t *lastRegNodePtr)
{
  DTL_FUNC_REG_t  *address;

  address = (DTL_FUNC_REG_t *) osapiMalloc(L7_DTL_COMPONENT_ID, sizeof(DTL_FUNC_REG_t));
  if (address == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  else
  {
    address->next = L7_NULLPTR;
    address->prev = lastRegNodePtr;
    lastRegNodePtr->next = address;
  }
  return address;
}

/*********************************************************************
* @purpose  Removes a node entry from the registry
*
* @param    *handle       @b{(input)} Handle to registry control block
* @param    *regNodePtr   @b{(input)} node pointer to remove
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlRegistryNodeDelete(DTL_REG_CTRL_t *handle, DTL_FUNC_REG_t *regNodePtr)
{
  if (regNodePtr == handle->first)
  {
    handle->first = regNodePtr->next;
    regNodePtr->next->prev = L7_NULLPTR;
  }
  else if (regNodePtr == handle->last)
  {
    handle->last = regNodePtr->prev;
    regNodePtr->prev->next = L7_NULLPTR;
  }
  else
  {
    regNodePtr->prev->next = regNodePtr->next;
    regNodePtr->next->prev = regNodePtr->prev;

  }
  osapiFree(L7_DTL_COMPONENT_ID, regNodePtr);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Creates a registry control block in the registry
*
* @param    none
*
* @returns  pointer to the control block on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
DTL_REG_CTRL_t *dtlRegistryCtrlCreate()
{
  DTL_REG_CTRL_t  *handle;

  handle = (DTL_REG_CTRL_t *)osapiMalloc(L7_DTL_COMPONENT_ID, sizeof(DTL_REG_CTRL_t));
  if (handle == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  handle->count = 0;
  handle->first = L7_NULLPTR;
  handle->last = L7_NULLPTR;
  return handle;
}

/*********************************************************************
* @purpose  Removes control block from the registry
*
* @param    *handle   @b{(input)} Handle to registry control block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlRegistryCtrlDelete(DTL_REG_CTRL_t *handle)
{
  DTL_FUNC_REG_t *nextRegPtr;
  DTL_FUNC_REG_t *tempRegPtr;

  if (handle->first != L7_NULLPTR)
  {

    nextRegPtr = handle->first->next;
    osapiFree(L7_DTL_COMPONENT_ID, handle->first);

    while (nextRegPtr != L7_NULLPTR)
    {
      tempRegPtr = nextRegPtr;
      nextRegPtr = nextRegPtr->next;
      osapiFree(L7_DTL_COMPONENT_ID, tempRegPtr);
    }
  }
  osapiFree(L7_DTL_COMPONENT_ID, handle);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  adds a node entry from the registry
*
* @param    *handle   @b{(input)} Handle to registry control block
* @param    *funcPtr  @b{(input)} Function pointer to add
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlRegistryNodeAdd(DTL_REG_CTRL_t *handle, L7_FUNCPTR_t funcPtr)
{
  DTL_FUNC_REG_t  *regNodePtr;

  if (handle->first == L7_NULLPTR)
  {
    handle->first = dtlRegistryNodeFirstGet();
    handle->last = handle->first;
    if (handle->first == L7_NULLPTR)
    {
      return L7_FAILURE;
    }
    regNodePtr = handle->first;

  }
  else
  {
    regNodePtr = dtlRegistryNodeNextGet(handle->last);
  }
  if (regNodePtr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  regNodePtr->funcPtr = funcPtr;
  handle->last = regNodePtr;
  handle->count++;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Finds a node entry from the registry
*
* @param    *handle   @b{(input)} Handle to registry control block
* @param    *funcPtr  @b{(input)} Function pointer to locate
*
* @returns  registry entry pointer on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
DTL_FUNC_REG_t *dtlRegistryNodeFind(DTL_REG_CTRL_t *handle, L7_FUNCPTR_t funcPtr)
{
  DTL_FUNC_REG_t  *regNodePtr;

  if (handle->count == 0)
  {
    return L7_NULLPTR;
  }
  regNodePtr = handle->first;

  while (regNodePtr != L7_NULLPTR)
  {
    if (regNodePtr->funcPtr == funcPtr)
      return regNodePtr;
    else
      regNodePtr = regNodePtr->next;
  }
  return regNodePtr;
}

#endif /*DTLCTRL_COMPONENT_CFG*/
