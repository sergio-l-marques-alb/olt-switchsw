/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename rlim_nim.c
*
* @purpose Router Logical Interface Manager NIM interactions
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/23/2005
*
* @author eberge
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l7_ip6_api.h"
#include "rlim.h"


/*********************************************************************
* @purpose  Create a NIM instance for a RLIM interface
*
* @param    intfType    Interface type
* @param    intfId      Interface ID
* @param    pIntIfNum		Pointer to Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimIntfCreateInNim(L7_uint32 intfType, L7_uint32 intfId,
                            L7_uint32 *pIntIfNum)
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
  L7_uchar8 namebuf[RLIM_INTF_MAX_NAME_LENGTH];

  /* Initialize data structures */

  memset((void *) &intfIdInfo, 0, sizeof(nimConfigID_t));
  memset((void *) &createInfo, 0, sizeof(NIM_INTF_CREATE_INFO_t));
  memset((void *) &defaultCfg, 0, sizeof(nimIntfConfig_t));
  memset((void *) &intfDescr, 0, sizeof(nimIntfDescr_t));
  memset((void *) &nimRequest, 0, sizeof(nimIntfCreateRequest_t));

  /* set up creation completion info */
  createInfo.component = L7_RLIM_COMPONENT_ID;
  createInfo.pCbFunc = rlimNimEventCompletionCallback;

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

  rc = sysapiRegistryGet(SYS_MAC, MAC_ENTRY, (void *)&intfDescr.macAddr.addr);
  if (rc != L7_SUCCESS)
  {
    rlimIntfNameFromTypeIdGet(intfType, intfId, namebuf);
    LOG_MSG("RLIM: failed to get mac address for %s\n", namebuf);
    return L7_FAILURE;
  }

  switch (intfType)
  {
    case L7_LOOPBACK_INTF:
      intfIdInfo.configSpecifier.loopbackId = intfId;
      intfIdInfo.type = L7_LOOPBACK_INTF;

      defaultCfg.ipMtu = RLIM_INTF_LOOPBACK_MTU;
      defaultCfg.cfgMaxFrameSize = RLIM_INTF_LOOPBACK_MTU;

      intfDescr.settableParms = L7_INTF_PARM_ADMINSTATE | L7_INTF_PARM_LINKTRAP;
      intfDescr.ianaType = L7_IANA_SOFTWARE_LOOPBACK;
      intfDescr.frameSize.largestFrameSize = RLIM_INTF_LOOPBACK_MTU;
      strcpy((L7_char8 *) & (intfDescr.ifDescr), IANA_SOFTWARE_LOOPBACK_DESC);
      sprintf((L7_char8 *) & (intfDescr.ifName), "%s%u",
              RLIM_INTF_LOOPBACK_NAME_PREFIX, intfId);
      break;

    case L7_TUNNEL_INTF:
      intfIdInfo.configSpecifier.tunnelId = intfId;
      intfIdInfo.type = L7_TUNNEL_INTF;

      defaultCfg.ipMtu = RLIM_INTF_TUNNEL_MTU;
      defaultCfg.cfgMaxFrameSize = RLIM_INTF_TUNNEL_MTU;

      intfDescr.settableParms = L7_INTF_PARM_ADMINSTATE | L7_INTF_PARM_LINKTRAP;
      intfDescr.ianaType = L7_IANA_TUNNEL;
      intfDescr.frameSize.largestFrameSize = RLIM_INTF_TUNNEL_MTU;
      strcpy((L7_char8 *) & (intfDescr.ifDescr), IANA_TUNNEL_DESC);
      sprintf((L7_char8 *) & (intfDescr.ifName), "%s%u",
              RLIM_INTF_TUNNEL_NAME_PREFIX, intfId);
      break;

    default:
      return L7_FAILURE;
  }

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
    rlimIntfNameFromTypeIdGet(intfType, intfId, namebuf);
    LOG_MSG("RLIM: failed to create interface %s\n", namebuf);
    return L7_FAILURE;
  }

  notifyEventInfo.component = L7_RLIM_COMPONENT_ID;
  notifyEventInfo.pCbFunc = rlimNimEventCompletionCallback;
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
* @purpose  Delete an RLIM intf in NIM
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
L7_RC_t rlimIntfDeleteInNim(L7_uint32 intIfNum)
{

  L7_BOOL wait_for_transition;
  L7_BOOL detach;
  NIM_HANDLE_t handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  L7_NIM_QUERY_DATA_t nimQueryData;
  L7_INTF_TYPES_t itype;
  L7_INTF_TYPES_t iftype;
  L7_uint32 linkState;

  /* Prep the NIM event info structure */
  memset((void *) &eventInfo, 0, sizeof(eventInfo));
  eventInfo.component = L7_RLIM_COMPONENT_ID;
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
      LOG_MSG("nimIntfQuery failed for intIfNum %d\n", intIfNum);
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
    /* If loopback intf is up, send an L7_DOWN event */
    if ((nimGetIntfType(intIfNum, &iftype) == L7_SUCCESS) && 
        (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS) &&
        (linkState == L7_UP) && (iftype == L7_LOOPBACK_INTF))
    {
      memset((void *)&eventInfo,0,sizeof(eventInfo));
      eventInfo.component   = L7_RLIM_COMPONENT_ID;
      eventInfo.intIfNum    = intIfNum;
      eventInfo.event   = L7_DOWN;
      eventInfo.pCbFunc = L7_NULLPTR;
      if (nimEventIntfNotify(eventInfo,&handle) != L7_SUCCESS)
      {
        LOG_MSG("down notification failed for intIfNum %d\n", intIfNum);
      }
    }

    /*
     * Now send a detach notification.
     * Delete will be completed in the completion callback
     */
    eventInfo.event = L7_DETACH;
    eventInfo.pCbFunc = rlimNimEventCompletionCallback;

    if (nimEventIntfNotify(eventInfo, &handle) != L7_SUCCESS)
    {
      LOG_MSG("detach notification failed for intIfNum %d\n", intIfNum);
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
        LOG_MSG("nimIntfQuery failed for intIfNum %d in delete attempt\n",
                intIfNum);
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
          LOG_MSG("Interface %d should not be in state %d\n",
                  intIfNum, nimQueryData.data.state);
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
    eventInfo.pCbFunc = rlimNimEventCompletionCallback;

    /*
     * Set up for the invocation of per-type deletion-complete handlers
     */
    if (nimGetIntfType(intIfNum, &itype) == L7_SUCCESS)
    {
      switch (itype)
      {
        case L7_LOOPBACK_INTF:
          eventInfo.pCbFunc = rlimLoopbackNimDeleteDone;
          break;
        case L7_TUNNEL_INTF:
          eventInfo.pCbFunc = rlimTunnelNimDeleteDone;
          break;
        default:
          break;
      }
    }

    if (nimEventIntfNotify(eventInfo, &handle) != L7_SUCCESS)
    {
      LOG_MSG("delete notification failed for intIfNum %d\n", intIfNum);
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
void rlimNimEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t handle;
  L7_INTF_TYPES_t itype;
  L7_RC_t rc;

  if (retVal.response.rc != L7_SUCCESS)
  {
    /* Failed to complete the request */
    LOG_MSG("RLIM: notification failed for event(%d), intf(%d), reason(%d)\n",
            retVal.event, retVal.intIfNum, retVal.response.reason);
    return;
  }
  else
  {
    /* successful event notification */
    memset((void *) &eventInfo, 0, sizeof(eventInfo));
    eventInfo.component = L7_RLIM_COMPONENT_ID;
    eventInfo.intIfNum = retVal.intIfNum;
    eventInfo.pCbFunc = rlimNimEventCompletionCallback;

    switch (retVal.event)
    {
      case L7_CREATE:
        eventInfo.event = L7_ATTACH;
        (void) nimEventIntfNotify(eventInfo, &handle);
        break;

      case L7_ATTACH:
        if (nimGetIntfType(retVal.intIfNum, &itype) == L7_SUCCESS &&
            itype == L7_LOOPBACK_INTF)
        {
          rc = ipMapRtrIntfModeSet(retVal.intIfNum, L7_ENABLE);
          if (rc != L7_SUCCESS)
          {
            LOG_MSG("failed to enable routing on intf %u", retVal.intIfNum);
          }
#ifdef L7_IPV6_PACKAGE
          rc = ip6MapRtrIntfModeSet(retVal.intIfNum, L7_ENABLE);
          if (rc != L7_SUCCESS)
          {
            LOG_MSG("failed to enable v6 routing on intf %u\n",
                    retVal.intIfNum);
          }
#endif
        }
        break;
        

      case L7_DETACH:
        eventInfo.event = L7_DELETE;
        if (nimGetIntfType(retVal.intIfNum, &itype) == L7_SUCCESS)
        {
          switch (itype)
          {
            case L7_LOOPBACK_INTF:
              eventInfo.pCbFunc = rlimLoopbackNimDeleteDone;
              break;
            case L7_TUNNEL_INTF:
              eventInfo.pCbFunc = rlimTunnelNimDeleteDone;
              break;
            default:
              break;
          }
        }
        (void) nimEventIntfNotify(eventInfo, &handle);
        break;

      case L7_DELETE:
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
void rlimNimEventSend(L7_uint32 intIfNum, L7_uint32 event)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t handle;

  memset((void *) &eventInfo, 0, sizeof(eventInfo));
  eventInfo.component = L7_RLIM_COMPONENT_ID;
  eventInfo.intIfNum = intIfNum;
  eventInfo.event = event;
  eventInfo.pCbFunc = L7_NULLPTR;
  (void) nimEventIntfNotify(eventInfo, &handle);
}
