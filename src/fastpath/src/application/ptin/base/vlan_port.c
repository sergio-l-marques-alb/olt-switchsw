/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename vlan_port.c
*
* @purpose VLAN PORT Interface Manager NIM interactions
*
* @component VLAN PORT Interface Manager
*
* @comments
*
* @create 01/10/2012
*
* @author mruas
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "nimapi.h"
#include "vlan_port.h"
#include "simapi.h"

#include "logger.h"

/* Callback for phases termination */
void vlanportNimEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal);


/**
 * Create a new vlan port (virtual port) as na new intIfNum 
 * value (does not affect hardware) 
 * 
 * @param intfId : some value
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t vlan_port_new(L7_uint32 intfId)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0;

  rc = vlan_port_intIfNum_create(intfId, &intIfNum);

  printf("Creation operation result: intIfNum=%u, rc=%d\r\n",intIfNum,rc);

  return rc;
}

/**
 * Remove a vlan port (virtual port) as an existent intIfNum 
 * value (does not affect hardware) 
 * 
 * @param intIfNum: intIfNum value
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t vlan_port_free(L7_uint32 intIfNum)
{
  L7_RC_t rc;

  rc = vlan_port_intIfNum_delete(intIfNum);

  printf("Deletion operation result: rc=%d\r\n",rc);

  return rc;
}


/*********************************************************************
* @purpose  Create a NIM instance for a VLAN_PORT interface
*
* @param    intfType    Interface type
* @param    intfId      Interface ID
* @param    pIntIfNum       Pointer to Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vlan_port_intIfNum_create(L7_uint32 intfId, L7_uint32 *pIntIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  nimIntfCreateRequest_t nimRequest;
  nimConfigID_t intfIdInfo;
  nimIntfDescr_t intfDescr;
  nimIntfConfig_t defaultCfg;
  NIM_INTF_CREATE_INFO_t createInfo;
  NIM_EVENT_NOTIFY_INFO_t notifyEventInfo;
  nimIntfCreateOutput_t nimOutput;
  NIM_HANDLE_t handle;

  /* Initialize data structures */

  memset((void *) &intfIdInfo, 0, sizeof(nimConfigID_t));
  memset((void *) &createInfo, 0, sizeof(NIM_INTF_CREATE_INFO_t));
  memset((void *) &defaultCfg, 0, sizeof(nimIntfConfig_t));
  memset((void *) &intfDescr, 0, sizeof(nimIntfDescr_t));
  memset((void *) &nimRequest, 0, sizeof(nimIntfCreateRequest_t));

  /* set up creation completion info */
  createInfo.component = L7_PTIN_COMPONENT_ID;
  createInfo.pCbFunc = vlanportNimEventCompletionCallback;

  /* setup the default config for the interface */
  memset(defaultCfg.LAAMacAddr.addr, 0, L7_MAC_ADDR_LEN);
  defaultCfg.addrType = L7_SYSMAC_BIA;
  memset(defaultCfg.ifAlias, 0, sizeof(defaultCfg.ifAlias));
  defaultCfg.nameType = L7_SYSNAME;
  defaultCfg.ifSpeed = L7_PORTCTRL_PORTSPEED_UNKNOWN;
  defaultCfg.negoCapabilities = 0; /* Autonegotiation disabled */
  defaultCfg.adminState = L7_ENABLE;
  defaultCfg.trapState = L7_ENABLE;
  defaultCfg.encapsType = L7_ENCAP_NONE;

  /* setup the intf characteristics for this interface */
  intfDescr.configurable = L7_TRUE;
  intfDescr.internal = L7_FALSE;
  memset(&intfDescr.bcastMacAddr, 0, sizeof(intfDescr.bcastMacAddr));
  intfDescr.defaultSpeed = L7_PORTCTRL_PORTSPEED_UNKNOWN;
  intfDescr.phyCapability = 0;
  intfDescr.connectorType = L7_CONNECTOR_NONE;
  memset((void *) &intfDescr.macroPort, 0, sizeof(nimMacroPort_t));

  /* Set the mac-address of the tunnel/loopback interface to the
   * system MAC Address irrespective of if the feature L7_FEAT_UNIQUE_L3_ROUTER_MAC
   * is enabled in the system */
  simMacAddrGet((L7_uchar8 *)&intfDescr.macAddr.addr);
  simMacAddrGet((L7_uchar8 *)&intfDescr.l3MacAddr.addr);

  /* VLAN PORT definition*/
  intfIdInfo.configSpecifier.vlanportId = intfId;
  intfIdInfo.type = L7_VLAN_PORT_INTF;

  defaultCfg.ipMtu = 1518;
  defaultCfg.cfgMaxFrameSize = 1518;

  intfDescr.settableParms = L7_NULL;
  intfDescr.ianaType = L7_IANA_VLAN_PORT;
  intfDescr.frameSize.largestFrameSize = 1518;
  strcpy((L7_char8 *) & (intfDescr.ifDescr), IANA_VLAN_PORT_DESC);
  sprintf((L7_char8 *) & (intfDescr.ifName), "%s%u", "vlanport", intfId);


  /* setup the request block pointers */
  nimRequest.pDefaultCfg = &defaultCfg;
  nimRequest.pIntfDescr = &intfDescr;
  nimRequest.pIntfIdInfo = &intfIdInfo;
  nimRequest.pCreateInfo = &createInfo;

  /* setup the output block  */
  nimOutput.handle = &handle;
  nimOutput.intIfNum = pIntIfNum;


  /* Tell the system about the interface */
  if ((rc = nimIntfCreate(&nimRequest, &nimOutput)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed to create interface %u", intfId);
    return L7_FAILURE;
  }

  notifyEventInfo.component = L7_PTIN_COMPONENT_ID;
  notifyEventInfo.pCbFunc = vlanportNimEventCompletionCallback;
  notifyEventInfo.event = L7_CREATE;
  notifyEventInfo.intIfNum = *pIntIfNum;
  rc = nimEventIntfNotify(notifyEventInfo, &handle);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR(rc);
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete an VLAN_PORT intf in NIM
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t vlan_port_intIfNum_delete(L7_uint32 intIfNum)
{
  L7_BOOL wait_for_transition;
  L7_BOOL detach;
  NIM_HANDLE_t handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  L7_NIM_QUERY_DATA_t nimQueryData;

  /* Prep the NIM event info structure */
  memset((void *) &eventInfo, 0, sizeof(eventInfo));
  eventInfo.component = L7_PTIN_COMPONENT_ID;
  eventInfo.intIfNum = intIfNum;

  /* Send a detach notification only if interface is attached */
  memset((void *) &nimQueryData, 0, sizeof(nimQueryData));
  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;
  wait_for_transition = L7_FALSE;
  detach = L7_FALSE;

  while (1)
  {
    if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
    {
      /* should never get here */
      PT_LOG_ERR(LOG_CTX_INTF, "nimIntfQuery failed for intIfNum %u", intIfNum);
      return L7_FAILURE;
    }

    switch (nimQueryData.data.state)
    {
      case L7_INTF_CREATED:
        /* Interface is not attached. No need to detach the interface */
        /* Go straight to delete */
        wait_for_transition = L7_FALSE;
        detach = L7_FALSE;
        break;

      case L7_INTF_ATTACHED:
        /* detach the interface before deleting */
        wait_for_transition = L7_FALSE;
        detach = L7_TRUE;
        break;

      case L7_INTF_ATTACHING:
        wait_for_transition = L7_TRUE;
        break;

      default:
        wait_for_transition = L7_FALSE;
        detach = L7_FALSE;
        break;
    }                           /* switch */

    if (wait_for_transition != L7_TRUE)
      break;

    osapiSleepMSec(10);
  }                             /* while (1) */

  if (detach == L7_TRUE)
  {
    /*
     * Now send a detach notification.
     * Delete will be completed in the completion callback
     */
    eventInfo.event = L7_DETACH;
    eventInfo.pCbFunc = vlanportNimEventCompletionCallback;

    if (nimEventIntfNotify(eventInfo, &handle) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "detach notification failed for intIfNum %u", intIfNum);
      return L7_FAILURE;
    }
  }                             /* detach == L7_TRUE */
  else
  {
    wait_for_transition = L7_FALSE;

    while (1)
    {
      if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
      {
        /* should never get here */
        PT_LOG_ERR(LOG_CTX_INTF, "nimIntfQuery failed for intIfNum %u in delete attempt", intIfNum);
        return L7_FAILURE;
      }

      switch (nimQueryData.data.state)
      {
        case L7_INTF_CREATED:
          /* Go straight to delete */
          wait_for_transition = L7_FALSE;
          break;

        case L7_INTF_ATTACHED:
        case L7_INTF_ATTACHING:
          /* Handle the case of the VLAN interface becoming attached
             while an end user is deleting the interface. This is a valid
             scenario for a dynamically created VLAN or a VLAN which was not
             previously existing on the box being configured just prior to
             a detach notification */

          /*
             WPJ NOTE:  We need to do some sort of semaphore control in
             this case to prevent an infinite loop here  - If so, this
             case moves to the
             default area */

          wait_for_transition = L7_TRUE;
          break;

        case L7_INTF_DETACHING:
          wait_for_transition = L7_TRUE;
          break;

        default:
          /* detach the interface before deleting */
          PT_LOG_ERR(LOG_CTX_INTF, "Interface should not be in state %d", nimQueryData.data.state);
          wait_for_transition = L7_FALSE;
          return L7_FAILURE;
          break;
      }                         /* switch */

      if (wait_for_transition != L7_TRUE)
        break;

      osapiSleepMSec(10);
    }                           /* while (1) */

    /*
     * Now send a delete notification.
     */
    eventInfo.event = L7_DELETE;
    eventInfo.pCbFunc = vlanportNimEventCompletionCallback;

    if (nimEventIntfNotify(eventInfo, &handle) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "delete notification failed for intIfNum %u", intIfNum);
      return L7_FAILURE;
    }
  }                             /* detach != L7_TRUE */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Receive the completion notification from NIM event notifications
*
* @param    retVal  pointer to a NIM_NOTIFY_CB_INFO_t structure
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
*
* @end
*********************************************************************/
void vlanportNimEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t handle;

  if (retVal.response.rc != L7_SUCCESS)
  {
    /* Failed to complete the request */
    PT_LOG_ERR(LOG_CTX_INTF, "notification failed for event(%d), intIfNum(%u), reason(%d)",
            retVal.event, retVal.intIfNum, retVal.response.reason);
    return;
  }
  else
  {
    /* successful event notification */
    memset((void *) &eventInfo, 0, sizeof(eventInfo));
    eventInfo.component = L7_PTIN_COMPONENT_ID;
    eventInfo.intIfNum = retVal.intIfNum;
    eventInfo.pCbFunc = vlanportNimEventCompletionCallback;

    switch (retVal.event)
    {
      case L7_CREATE:
        eventInfo.event = L7_ATTACH;
        (void) nimEventIntfNotify(eventInfo, &handle);
        break;

      case L7_ATTACH:
        PT_LOG_TRACE(LOG_CTX_INTF, "intIfNum %u attached!", retVal.intIfNum);
        break;

    case L7_DETACH:
        eventInfo.event = L7_DELETE;
        (void) nimEventIntfNotify(eventInfo, &handle);
        break;

      case L7_DELETE:
        PT_LOG_TRACE(LOG_CTX_INTF, "intIfNum %u deleted!", retVal.intIfNum);
        break;

      default:
        break;
    }
  }
}

/*********************************************************************
* @purpose  Send the specified NIM interface event
*
* @param    intIfNum   internal interface identifier
* @param    event      event to send
*
* @returns  void
*
* @notes    none
*
*
* @end
*********************************************************************/
void vlanport_NimEventSend(L7_uint32 intIfNum, L7_uint32 event)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t handle;

  memset((void *) &eventInfo, 0, sizeof(eventInfo));
  eventInfo.component = L7_PTIN_COMPONENT_ID;
  eventInfo.intIfNum = intIfNum;
  eventInfo.event = event;
  eventInfo.pCbFunc = L7_NULLPTR;
  (void) nimEventIntfNotify(eventInfo, &handle);
}

