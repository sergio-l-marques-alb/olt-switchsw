/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    dot1dgarpapi.c
* @purpose     GARP API functions
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "string.h"
#include "stdlib.h"
#include "log.h"
#include "nimapi.h"
#include "nvstoreapi.h"
#include "sysnet_api.h"
#include "garpapi.h"
#include "garpctlblk.h"
#include "garpcfg.h"
#include "default_cnfgr.h"
#include "dot3ad_api.h"

#include "dot1s_api.h"
#include "dot1dgarp.h"
#include "dot1q_api.h"
#include "garp_dot1q_api.h"
#include "garp_dot1q_util.h"
#include "garp_debug.h"
 
            
extern GARPStatus GARP_CREATE_ENTRY(void *);
extern L7_RC_t    GARP_SEND_PDU(L7_netBufHandle bufHandle, L7_uint32 intIfNum, L7_uint32 vlan_id);
extern void *garpQueue;
extern L7_uint32  *garpMapTbl;
extern GarpInfo_t garpInfo;


extern void garpTraceQueueMsgSend(GarpPacket *msg);

/*--------------------------------------------------------------------------*/
/* Local Data Instatiation                                                  */
/*--------------------------------------------------------------------------*/

L7_uchar8 Garp_gvrp_mac_address[6] = 
{
  0x01, 0x80, 0xc2, 0x00, 0x00, 0x21
};

L7_uchar8 Garp_gmrp_mac_address[6] = 
{
  0x01, 0x80, 0xc2, 0x00, 0x00, 0x20
};

garpCfg_t  *garpCfg  = L7_NULLPTR;   /* configuration file */
gvrpInfo_t *gvrpInfo = L7_NULLPTR;   /* statistics info for GVRP */
gmrpInfo_t *gmrpInfo = L7_NULLPTR;   /* statistics info for GMRP */
extern GARPCBptr GARPCB;

/*********************************************************************
* @purpose Check if the Garp Application is supported.
*
* @param   app    Garp application
*
* @returns L7_FALSE
* @returns L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpApplicationIsSupported(GARPApplication app)
{
  /*  If the application is GVRP and it is supported     */
  /*  or If the application is GMRP and it is supported  */
  /*  Return True OTHERWISE, return FALSE                */

  return((L7_BOOL)((app == GARP_GVRP_APP && GARPCB->GarpCB_gvrpSupported) ||
                   (app == GARP_GMRP_APP && GARPCB->GarpCB_gmrpSupported)));

}

/*********************************************************************
* @purpose  Init the commmand buffer 
*
* @param    pCmd           pointer to command
* @param    app            garp application
* @param    cmd            garp command 
* @param    attrType       attribute type
* @param    pAttrValue     attribute value
* @param    port           port number
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
GARPStatus GarpBuildCommand(GARPCommand *pCmd,
                            GARPApplication app,
                            GARPCommandNameTable cmd,
                            GARPAttrType attrType,
                            GARPAttrValue pAttrValue, 
                            GARPPort port, L7_uint32 gmrpVID)
{
  GARPStatus returnVal = GARP_SUCCESS;

  /* initialize common fields */
  pCmd->port    = port;
  pCmd->command = cmd;
  pCmd->app     = app;
  pCmd->vlan_id = gmrpVID;            /* GMRP use only */

  /* initialize command specific fields */
  switch (cmd)
  {
    case GARP_JOIN:
    case GARP_LEAVE:
    case GARP_DELETE_ATTRIBUTE:
    case GARP_NORMAL_PARTICIPANT:
    case GARP_NON_PARTICIPANT:
    case GARP_NORMAL_REGISTRATION:
    case GARP_REGISTRATION_FIXED:
    case GARP_REGISTRATION_FORBIDDEN:
      switch (app) /* application based fields */
      {
        case GARP_GMRP_APP:
          
          switch (attrType) /* attribute based fields */
          {
            case GARP_GMRP_LEGACY_ATTRIBUTE:
              switch (*pAttrValue)
              {
                case GARP_GMRP_FORWARD_ALL:
                  pCmd->flags = GARP_GMRP_FWDALL;
                  break;

                case GARP_GMRP_FORWARD_UNREGISTERED:
                  pCmd->flags = GARP_GMRP_FWDUN;
                  break;
                default:
                returnVal = GARP_FAILURE;
                break;
              } /* pAttrValue */
              break;
            
            case GARP_GMRP_MULTICAST_ATTRIBUTE:
              memcpy(pCmd->data, pAttrValue, 6);
              break;

            default:
              returnVal = GARP_FAILURE;
              break;
          } /* attrType */
        break;
        
        case GARP_GVRP_APP:
          *(L7_ushort16 *)&pCmd->data = (L7_ushort16)(*(L7_uint32 *)pAttrValue);
          break;
        
        default:
          returnVal = GARP_FAILURE;
          break;
      } /* app */
      break;
    
    case GARP_ENABLE_PORT:
    case GARP_DISABLE_PORT:
    case GARP_ENABLE:
    case GARP_DISABLE:
      /* already initialized in common fields */
      break;

    case GARP_JOIN_TIME:
    case GARP_LEAVE_TIME:
    case GARP_LEAVEALL_TIME:
      *(GARPTime *)&pCmd->data = (GARPTime)(*(GARPTime *)pAttrValue);
      break;

    case GVRP_VLAN_CREATION_FORBID:
    case GVRP_VLAN_REGISTRATION_FORBID:
      *(L7_BOOL *)&pCmd->data = (L7_BOOL)(*(L7_BOOL *)pAttrValue);
      break;
    default:
      returnVal = GARP_FAILURE;
      break;
  }

  return returnVal;
}

/*********************************************************************
* @purpose  make sure GARP initilization is done.
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void GarpInitDone(void)
{
  void *syncSem = (void *)GARPCB->GarpCB_initSyncSem;

  /*  if GARP is waiting */
  if (syncSem != L7_NULL)
  {
    /* tell GARP that we can continue */
    osapiSemaGive(syncSem);
  }

  return;
}

/*********************************************************************
* @purpose  Wait till GARP initilization is done.
*
* @param    timeout  timeout period in milliseconds
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
*
* @notes
*       
* @end
*********************************************************************/
GARPStatus GarpWaitInitDone(L7_int32 timeout)
{
  void *     syncSem;
  GARPStatus returnVal = GARP_FAILURE;

  /* Get a Binary Semaphore for synchronization with Garp  */
  syncSem = GARPCB->GarpCB_initSyncSem;

  /*  Take the semaphore and wait for Garp to give it back */
  if (osapiSemaTake(syncSem, timeout) == L7_SUCCESS)
  {
    returnVal = GARP_SUCCESS;
  }

  osapiSemaDelete(syncSem);
  GARPCB->GarpCB_initSyncSem = L7_NULL;

  return returnVal;
}

/*********************************************************************
* @purpose  Print the current GARP config values to 
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t GarpConfigDump(void)
{
  L7_RC_t rc=L7_SUCCESS;
  L7_char8 buf[32];
  L7_uint32 i=0;
  L7_uint32 idx;
  nimUSP_t usp;
  garpIntfCfgData_t *pCfg;

  for (i=1;i<=L7_MAX_INTERFACE_COUNT;i++)
  {
    if ((dot3adIsLagMember(i) == L7_FALSE) ||
        (dot3adIsLagConfigured(i) == L7_TRUE))
      {
        if (nimGetUnitSlotPort(i, &usp)==L7_SUCCESS)
        {
          if (garpMapIntfIsConfigurable(i, &pCfg) == L7_TRUE)
          {
            idx = garpMapTbl[i];
            sysapiPrintf("Interface %d.%d:\n",usp.slot,usp.port);
            sysapiPrintf("Join Time (ms) - %d\n",garpCfg->intf[idx].join_time);
            sysapiPrintf("Leave Time (ms) - %d\n",garpCfg->intf[idx].leave_time);
            sysapiPrintf("Leave All Time (ms) - %d\n",garpCfg->intf[idx].leaveall_time);
            sysapiPrintf("vlan-creation-forbid flag - %d\n",garpCfg->intf[idx].vlanCreationForbid);
            sysapiPrintf("registration-forbid flag - %d\n",garpCfg->intf[idx].registrationForbid);
          }
        }
      }
  }
  
  if (garpCfg->gvrp_enabled == L7_TRUE)
    sprintf(buf,"%s","L7_TRUE");
  else
    sprintf(buf,"%s","L7_FALSE");
  sysapiPrintf("\nGVRP Enabled - %s\n",buf);
  if (garpCfg->gmrp_enabled == L7_TRUE)
    sprintf(buf,"%s","L7_TRUE");
  else
    sprintf(buf,"%s","L7_FALSE");
  sysapiPrintf("GMRP Enabled - %s\n",buf);

  return rc;

}

/****************************************************************************/
/*                       GARP Command Interface                             */
/****************************************************************************/

/*********************************************************************
* @purpose  Send command to the GARP queue
*
* @param    app          Application
* @param    cmd          command
* @param    port         port number
* @param    attrType     Attribute Type
* @param    pAttrValue   Attribute Value
* @param    gmrpVID      shall be used for GMRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t GarpIssueCmd(GARPApplication app, GARPCommandNameTable cmdname,
                     GARPPort port, GARPAttrType attrType,
                     GARPAttrValue pAttrValue, L7_uint32 gmrpVID)
{
  GARPCommand cmd;
  GarpPacket  msg;
  L7_RC_t     rc = L7_FAILURE;
  L7_uchar8   garpCommandName[30];
  L7_uchar8   garpAppName[15];

  
  /* if GARP exists and (application is supported OR a set time command) */
  if (GARPCB != L7_NULL && (GarpApplicationIsSupported(app) ||
                            cmdname == GVRP_VLAN_CREATION_FORBID     ||
                            cmdname == GVRP_VLAN_REGISTRATION_FORBID ||
                            cmdname == GARP_JOIN_TIME          ||
                            cmdname == GARP_LEAVE_TIME         ||
                            cmdname == GARP_LEAVEALL_TIME))
  {
    /* if the command can be issued */
    if (GarpIsReady() == L7_TRUE)
    {
      /* build and issue the command */
      if (GarpBuildCommand(&cmd, app, cmdname, attrType, pAttrValue, port, gmrpVID) == GARP_SUCCESS)
      {
        
        msg.msgId = G_COMMAND;
        memcpy( (void*)&msg.msgData.command, (void*)&cmd, sizeof(GARPCommand) );

        garpTraceQueueMsgSend(&msg);

        rc = osapiMessageSend(garpQueue, &msg, 
                              sizeof(GarpPacket), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
        
        if (rc != L7_SUCCESS) 
        {
            garpTraceProcessCommandNameFormat(&cmd, garpCommandName);
            garpTraceProcessCommandAppNameFormat(&cmd, garpAppName);

            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_GARP_COMPONENT_ID,
                    "GarpIssueCmd: QUEUE SEND FAILURE: %s: %s: port = %d, attrType = %d, pAttrValue = %d, gmrpVID = %d."
                    " The garpQueue is full, logs specifics of the message content like"
                    " internal interface number, type of message etc.", 
                    garpAppName, garpCommandName, port, attrType, pAttrValue ? ( *pAttrValue ) : (0), 
                    gmrpVID ); 

        }
        else
        {
            osapiSemaGive(garpInfo.msgQSema);
        }
      }
      else
      {
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Check if the PKT is a GVRP Packet.
*
* @param    daMacAddr    Mac address
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpPktIsGvrpPDU(const L7_uchar8 daMacAddr[])
{
  L7_BOOL returnVal = L7_FALSE;

  if (memcmp(Garp_gvrp_mac_address, daMacAddr, 6) == 0 )
  {
    returnVal = L7_TRUE;
  }

  return returnVal;
}

/*********************************************************************
* @purpose  Check if the PKT is a GMRP Packet.
*
* @param    daMacAddr    Mac address
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpPktIsGmrpPDU(const L7_uchar8 daMacAddr[])
{
  L7_BOOL returnVal = L7_FALSE;

  if (memcmp(Garp_gmrp_mac_address, daMacAddr, 6) == 0 )
  {
    returnVal = L7_TRUE;
  }

  return returnVal;
}

/*********************************************************************
* @purpose  Check if the PKT is a GARP Packet.
*
* @param    daMacAddr    Mac address
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpPktIsGPDU(const L7_uchar8 daMacAddr[])
{
  L7_BOOL returnVal = L7_FALSE;

  if (GarpPktIsGvrpPDU(daMacAddr) || GarpPktIsGmrpPDU(daMacAddr))
  {
    returnVal = L7_TRUE;
  }

  return returnVal;
}

/*********************************************************************
* @purpose  Check if the PKT is VLAN tagged
*
* @param    daMacAddr    Mac address
*
* @returns  offset 0 if not tagged
* @returns  offset 4 if tagged
*
* @notes
*       
* @end
*********************************************************************/
L7_uint32 GarpPktIsTagged(L7_uchar8 *pPkt)
{
  if ((pPkt[18] == 0x42) && (pPkt[19] == 0x42))
  {
    return 4;
  }
  else
    return 0;
}

/*********************************************************************
* @purpose  Tell Garp the link status has changed 
*
* @param    intIfNum        interface number
* @param    event           event
* @param    correlator      correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t garpIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;    
  L7_INTF_TYPES_t sysIntfType;
  GARPStatus    returnVal;
  L7_BOOL       notify_nim = L7_TRUE;

  /*  If Garp exist  */
  if (GARPCB != L7_NULL)
  {
    /*   Can the Command be issued?  */
    if ((GarpIsReady() == L7_TRUE) &&
        (GARPCB->GarpCB_linkStateChange != L7_NULL) &&
        (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
        (garpIsValidIntfType(sysIntfType) == L7_TRUE))
    {
      switch (event)
      {
      case L7_INACTIVE:                    
      case L7_ACTIVE:
      case L7_CREATE:
      case L7_ATTACH:
      case L7_DELETE:
      case L7_DETACH:
      case L7_LAG_ACQUIRE:                    
      case L7_LAG_RELEASE:
      case L7_PROBE_SETUP:
      case L7_PROBE_TEARDOWN:
      case L7_PORT_ROUTING_ENABLED:
      case L7_PORT_ROUTING_DISABLED:
        returnVal = (*GARPCB->GarpCB_intfStateChange)(intIfNum, event, correlator);
        if (returnVal == GARP_SUCCESS)
        {
          /* The GARP task will tell NIM when it is done with the interface.
          */
          notify_nim = L7_FALSE;
        }
        break;
      
      default:
        break;
      }
    }
  }
  
  if (notify_nim == L7_TRUE)
  {
    status.intIfNum    = intIfNum;
    status.component   = L7_GARP_COMPONENT_ID;
    status.event       = event;
    status.correlator  = correlator;
    status.response.rc = rc;
    nimEventStatusCallback(status);
  }

  return rc;
}

/*********************************************************************
* @purpose  Processes GARP-related event initiated by Dot1Q  
*
* @param (in)    vlanId    Virtual LAN Id  
* @param (in)    intIfNum  Interface Number  
* @param (in)    event     
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t garpVlanChangeCallback(dot1qNotifyData_t *vlanDataCopy, L7_uint32 intIfNum, L7_uint32 event)
{ 

  GarpPacket  msg;
  L7_RC_t     rc1 = L7_FAILURE;
  L7_uint32   numVlans = 0;
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32 i, vlanType;
  L7_VLAN_MASK_t  adminTypeVlanMask;
  L7_BOOL sendGvrpMsg = L7_FALSE;
  dot1qNotifyData_t vlanData;

  memcpy(&vlanData, vlanDataCopy, sizeof(dot1qNotifyData_t));
  memset(&msg,0,sizeof(GarpPacket));
  memset(&adminTypeVlanMask,0,sizeof(L7_VLAN_MASK_t));
  /* add check to see if vlan is static . if dynamic , then vlan deletion would
             have already been processed */
  if (event == VLAN_DELETE_PENDING_NOTIFY || event == VLAN_ADD_NOTIFY) 
  {
      if (vlanData.numVlans == 1) 
      {
        rc1 = dot1qOperVlanTypeGet(vlanData.data.vlanId,&vlanType);
        if (rc1 == L7_SUCCESS && vlanType == L7_DOT1Q_ADMIN)
        {
          L7_VLAN_SETMASKBIT(adminTypeVlanMask,vlanData.data.vlanId);
        }
        else if(event == VLAN_ADD_NOTIFY)
        {
          /*Do not process dynamic vlan add notification */
          return L7_SUCCESS;
        }
      }
      else
      {
          /* Taking advantage of the fact the dynamic vlans are created one by one and will
           * NOT use the mask hence it is OK to perform the following loop for
           * delete pendings alone*/
          if (event == VLAN_DELETE_PENDING_NOTIFY)
          {
            for (i=1;i<=L7_VLAN_MAX_MASK_BIT;i++) 
            {
              if (L7_VLAN_ISMASKBITSET(vlanData.data.vlanMask,i))
              {
                rc1 = dot1qOperVlanTypeGet(i,&vlanType);
                if (rc1 == L7_SUCCESS && vlanType == L7_DOT1Q_ADMIN)
                {
                  L7_VLAN_SETMASKBIT(adminTypeVlanMask,i);
                }
                else if(rc1 == L7_SUCCESS && event == VLAN_ADD_NOTIFY)
                {
                  /*Do not process dynamic vlan add notification */
                  L7_VLAN_CLRMASKBIT(vlanData.data.vlanMask,i);
                }
                numVlans++;
                if (numVlans == vlanData.numVlans)
                {
                  /*Break out of for loop as we have taken care of all the vlan bits that are set*/
                  break;
                }
              }
            }
          }
      }
  }

  switch (event)
  {
      case VLAN_DELETE_NOTIFY:
         break; /* Ignore VLAN_DELETE as we do all the action in DELETE_PENDING */
      case VLAN_ADD_NOTIFY:
      case VLAN_ADD_PORT_NOTIFY:
      case VLAN_DELETE_PENDING_NOTIFY:
      case VLAN_DELETE_PORT_NOTIFY:
      case VLAN_RESTORE_NOTIFY:
          msg.msgId   = G_VLAN_NOTIFY;
          memcpy(&msg.msgData.dot1q.vlanData,&vlanData, sizeof(dot1qNotifyData_t));
          msg.port = intIfNum;
          msg.msgData.dot1q.event = event;
          if (event == VLAN_DELETE_PENDING_NOTIFY)
          {
            msg.msgData.dot1q.gvrp_event = 0;
          }

          garpTraceQueueMsgSend(&msg);

          if (osapiMessageSend(garpQueue, (void*)&msg, sizeof(GarpPacket), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
          {
            LOG_MSG("GARP VLAN callback: Failed to send message for intIfNum %d, event %d\n", 
                    intIfNum, event);
            return L7_FAILURE;
          }
          else
          {
              osapiSemaGive(garpInfo.msgQSema);

              if (event == VLAN_DELETE_PENDING_NOTIFY)
              {
                msg.msgData.dot1q.gvrp_event = 1;
                if (vlanData.numVlans == 1)
                {
                 if(L7_VLAN_ISMASKBITSET(adminTypeVlanMask,vlanData.data.vlanId))
                  {
                    sendGvrpMsg = L7_TRUE;
                  }
                }
                else
                {
                  memcpy(&vlanData.data.vlanMask, &adminTypeVlanMask, sizeof(L7_VLAN_MASK_t));
                  sendGvrpMsg = L7_TRUE;
                }
                
                /*Send the delete pending message again this time for gvrp and with only admin created vlans
                */
                if (sendGvrpMsg == L7_TRUE)
                {
                   memcpy(&msg.msgData.dot1q.vlanData,&vlanData, sizeof(dot1qNotifyData_t));
                  if (osapiMessageSend(garpQueue, (void*)&msg, sizeof(GarpPacket), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
                  {
                    LOG_MSG("GARP VLAN callback: Failed to send message for intIfNum %d, event %d\n", 
                            intIfNum, event);
                    return L7_FAILURE;
                  }
                  else
                  {
                    osapiSemaGive(garpInfo.msgQSema);
                  }
                }
              }

          }

          break;

      default:
          break;
  }

  return rc;
}



/*********************************************************************
* @purpose  GARP Registered Callback function for Dot1s
*
* @param    mstId       spanning tree ID
* @param    intIfNum    internal interface number
* @param    event       event
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   
*
*
* @end
*********************************************************************/
L7_uint32 garpDot1sChangeCallBack(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 event)
{
  L7_RC_t rc = L7_SUCCESS;

  GarpPacket Message;

  memset((void *)&Message, 0, sizeof(Message));

  Message.msgId = G_DOT1S_NOTIFY;
  Message.msgData.dot1s.mstID = mstID;
  Message.msgData.dot1s.event = event;
  Message.port  = intIfNum;


  switch (event)
  {
    /* The follwing two case statements need to be L7_DOT1S_EVENT_FORWARDING and 
	 * L7_DOT1S_EVENT_NOT_FORWARDING. This routine is registered with dot1s and not NIM.
	 * dot1s does not generate the L7_ACTIVE and L7_INACTIVE events, NIM 
	 * does that. The active status of the port is checked before a GARP PDU
	 * is transmitted and a GARP PDU is never received on an INACTIVE interface,
	 * thus the active status of the port is honored.
	 */
	  
	case L7_DOT1S_EVENT_FORWARDING:
    case L7_DOT1S_EVENT_NOT_FORWARDING:
    case L7_DOT1S_EVENT_INSTANCE_CREATE:
    case L7_DOT1S_EVENT_INSTANCE_DELETE:
        garpTraceQueueMsgSend(&Message);

        if (osapiMessageSend(garpQueue, (void*)&Message, 
                             sizeof(GarpPacket), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) == L7_ERROR)
        { 
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_GARP_COMPONENT_ID,
                    "garpDot1sChangeCallBack: QUEUE SEND FAILURE: %s: mstID = %d, intIfNum = %d, event = %d."
                    " The garpQueue is full, logs specifics of the message content like"
                    " internal interface number, type of message etc.", 
                    "G_DOT1S_NOTIFY", mstID, intIfNum, event); 
        }
        else
        {
            osapiSemaGive(garpInfo.msgQSema);
        }
        break;


    default:
        break;
  }
  
  return rc;
}



/*********************************************************************
* @purpose  Check if the port is active.
*
* @param    port    port number
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpLinkIsActive(GARPPort port)
{
  L7_uint32 activeState;
  L7_BOOL returnVal = L7_FALSE;

  /* get the port state */
  if (nimGetIntfActiveState((L7_int32)port,&activeState)==L7_SUCCESS &&
      activeState == L7_ACTIVE)
  {
    returnVal = L7_TRUE;
  }

  return returnVal;
}

/*********************************************************************
* @purpose  install a link state change hook 
*
* @param    GARPStatus *  pointer to untion that takes the following param
*                        @param GARPSpanState   port state
*                        &param GARPPort        port
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
* @returns  GARP_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
GARPStatus GarpSpanInstallHook( GARPStatus (*span_hook_fcn)( GARPSpanState, GARPPort) )
{
  GARPStatus returnVal = GARP_FAILURE;

  /* if GARP exists */
  if (GARPCB != L7_NULL)
  {
    /* setup GARP to receive spanning tree states */
    GARPCB->GarpCB_linkStateChange = span_hook_fcn;
    returnVal = GARP_SUCCESS;
  }

  return returnVal;
}

/*********************************************************************
* @purpose  Uninstall a link state change hook 
*
* @param    void
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void GarpSpanUnInstallHook()
{
  /*  If Garp exist */
  if (GARPCB != L7_NULL)
  {
    GARPCB->GarpCB_linkStateChange = L7_NULL;
  }
}

/*********************************************************************
* @purpose  check if garp is active 
*
* @param    void
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL GarpIsReady(void)
{
  GARPStatus returnVal = L7_FALSE;

  /*  If Garp is ready, Post True Return Value */
  if (GARPCB->GarpCB_garpReady == L7_TRUE)
  {
    returnVal = L7_TRUE;
  }

  return returnVal;
}

/*********************************************************************
* @purpose  Send a message to the Garp_Task to process the incomming packet.
*
* @param    bufHandle   buffer handle to the bpdu received
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t GarpRecvPkt(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  return GARP_SEND_PDU(bufHandle,pduInfo->intIfNum,pduInfo->vlanId);
}

/*********************************************************************
* @purpose  Allow stats manager to get a garp related statistic
*
* @param    c           Pointer to storage allocated by stats
                        manager to hold the results
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Only called by stats manager
*       
* @end
*********************************************************************/
void garpStatGet(pCounterValue_entry_t c)
{
  c->status     = L7_SUCCESS;
  c->cSize      = C32_BITS;

  switch (c->cId)
  {
  case L7_PLATFORM_CTR_GVRP_PDU_RCV_COUNT:
    c->cValue.low = gvrpInfo->received[c->cKey];
    break;
  case L7_PLATFORM_CTR_GVRP_PDU_SND_COUNT:
    c->cValue.low = gvrpInfo->send[c->cKey];
    break;
  case L7_PLATFORM_CTR_GVRP_FAILED_REGISTRATIONS:
    c->cValue.low = gvrpInfo->failedReg[c->cKey];
    break;
  case L7_PLATFORM_CTR_GMRP_PDU_RCV_COUNT:
    c->cValue.low = gmrpInfo->received[c->cKey];
    break;
  case L7_PLATFORM_CTR_GMRP_PDU_SND_COUNT:
    c->cValue.low = gmrpInfo->send[c->cKey];
    break;
  case L7_PLATFORM_CTR_GMRP_FAILED_REGISTRATIONS:
    c->cValue.low = gmrpInfo->failedReg[c->cKey];
    break; 
  }
}

/*********************************************************************
* @purpose  Get the GARP protocol related statistic
*
* @param    interface   interface number
* @param    counterType the type of statistic counter 
* @param    val         the statistic 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarity
*           complex way.
*       
* @end
*********************************************************************/
L7_RC_t garpStatisticGet(L7_uint32 intIfNum, GARP_STAT_COUNTER_TYPE_t counterType, L7_uint32 *val)
{
  switch (counterType)
  {
    case GARP_JOIN_EMPTY_RECEIVED:
      *val = gvrpInfo->garpStats[intIfNum].rJE;
      break;
    case GARP_JOIN_IN_RECEIVED:
      *val = gvrpInfo->garpStats[intIfNum].rJIn;
      break;
    case GARP_EMPTY_RECEIVED:
      *val = gvrpInfo->garpStats[intIfNum].rEmp;
      break;
    case GARP_LEAVE_EMPTY_RECEIVED:
      *val = gvrpInfo->garpStats[intIfNum].rLE;
      break;
    case GARP_LEAVE_IN_RECEIVED:
      *val = gvrpInfo->garpStats[intIfNum].rLIn;
      break;
    case GARP_LEAVEALL_RECEIVED:
      *val = gvrpInfo->garpStats[intIfNum].rLA;
      break;
    case GARP_JOIN_EMPTY_SENT:
      *val = gvrpInfo->garpStats[intIfNum].sJE;
      break;
    case GARP_JOIN_IN_SENT:
      *val = gvrpInfo->garpStats[intIfNum].sJIn;
      break;
    case GARP_EMPTY_SENT:
      *val = gvrpInfo->garpStats[intIfNum].sEmp;
      break;
    case GARP_LEAVE_EMPTY_SENT:
      *val = gvrpInfo->garpStats[intIfNum].sLE;
      break;
    case GARP_LEAVE_IN_SENT:
      *val = gvrpInfo->garpStats[intIfNum].sLIn;
      break;
    case GARP_LEAVEALL_SENT:
      *val = gvrpInfo->garpStats[intIfNum].sLA;
      break;
    default:
      *val = L7_NULL;
      return L7_FAILURE;
      break;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the GARP protocol related error statistic
*
* @param    interface   interface number
* @param    counterType the type of error statistic counter 
* @param    val         the statistic 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarity
*           complex way.
*       
* @end
*********************************************************************/
L7_RC_t garpErrorStatisticGet(L7_uint32 intIfNum, GARP_ERROR_STAT_COUNTER_TYPE_t counterType, L7_uint32 *val)
{
  switch (counterType)
  {
    case GARP_INVALID_PROTOCOL_ID:
      *val = gvrpInfo->garpErrorStats[intIfNum].invalidProtocolId;
      break;
    case GARP_INVALID_ATTR_TYPE:
      *val = gvrpInfo->garpErrorStats[intIfNum].invalidAttrType;
      break;
    case GARP_INVALID_ATTR_LENGTH:
      *val = gvrpInfo->garpErrorStats[intIfNum].invalidAttrLen;
      break;
    case GARP_INVALID_ATTR_EVENT:
      *val = gvrpInfo->garpErrorStats[intIfNum].invalidAttrEvent;
      break;
    case GARP_INVALID_ATTR_VALUE:
      *val = gvrpInfo->garpErrorStats[intIfNum].invalidAttrValue;
      break;
    default:
      *val = L7_NULL;
      return L7_FAILURE;
      break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear the GARP protocol related statistics
*
* @param    interface   interface number
*
* @returns  L7_SUCCESS
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarity
*           complex way.
*       
* @end
*********************************************************************/
L7_RC_t garpStatisticsClear(L7_uint32 intIfNum)
{
  if (osapiSemaTake(garpTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    /* Set all the stats counters to NULL */
    gvrpInfo->garpStats[intIfNum].rJE = L7_NULL;
    gvrpInfo->garpStats[intIfNum].rJIn = L7_NULL;
    gvrpInfo->garpStats[intIfNum].rEmp = L7_NULL;
    gvrpInfo->garpStats[intIfNum].rLE = L7_NULL;
    gvrpInfo->garpStats[intIfNum].rLIn = L7_NULL;
    gvrpInfo->garpStats[intIfNum].rLA = L7_NULL;
    gvrpInfo->garpStats[intIfNum].sJE = L7_NULL;
    gvrpInfo->garpStats[intIfNum].sJIn = L7_NULL;
    gvrpInfo->garpStats[intIfNum].sEmp = L7_NULL;
    gvrpInfo->garpStats[intIfNum].sLE = L7_NULL;
    gvrpInfo->garpStats[intIfNum].sLIn = L7_NULL;
    gvrpInfo->garpStats[intIfNum].sLA = L7_NULL;
    (void)osapiSemaGive(garpTaskSyncSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Clear the GARP protocol related error statistics
*
* @param    interface   interface number
*
* @returns  L7_SUCCESS
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarity
*           complex way.
*       
* @end
*********************************************************************/
L7_RC_t garpErrorStatisticsClear(L7_uint32 intIfNum)
{
  if (osapiSemaTake(garpTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    /* Set all the error stats counters to NULL */
    gvrpInfo->garpErrorStats[intIfNum].invalidProtocolId = L7_NULL;
    gvrpInfo->garpErrorStats[intIfNum].invalidAttrType = L7_NULL;
    gvrpInfo->garpErrorStats[intIfNum].invalidAttrLen = L7_NULL;
    gvrpInfo->garpErrorStats[intIfNum].invalidAttrEvent = L7_NULL;
    gvrpInfo->garpErrorStats[intIfNum].invalidAttrValue = L7_NULL;
    (void)osapiSemaGive(garpTaskSyncSema);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the value of vlan-creation-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (output) vlan-creation-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t gvrpVlanCreationForbidGet(L7_uint32 interface, L7_BOOL *forbid)
{
  garpIntfCfgData_t *pCfg;

  if (garpMapIntfIsConfigurable(interface, &pCfg) == L7_TRUE)
  {
    *forbid = pCfg->vlanCreationForbid;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the value of vlan-creation-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) vlan-creation-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t gvrpPortVlanCreationForbidSet(L7_uint32 interface, L7_BOOL forbid)
{
  garpCfg->hdr.dataChanged = L7_TRUE;
  return GarpIssueCmd(GARP_GVRP_APP, GVRP_VLAN_CREATION_FORBID, interface, GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)&forbid, 0);
}

/*********************************************************************
* @purpose  Get the value of registration-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (output) VLAN registration-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t gvrpPortRegistrationForbidGet(L7_uint32 interface, L7_BOOL *forbid)
{
  garpIntfCfgData_t *pCfg;

  if (garpMapIntfIsConfigurable(interface, &pCfg) == L7_TRUE)
  {
    *forbid = pCfg->registrationForbid;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the value of registration-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) VLAN registration-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t gvrpPortRegistrationForbidSet(L7_uint32 interface, L7_BOOL forbid)
{
  garpCfg->hdr.dataChanged = L7_TRUE;
  return GarpIssueCmd(GARP_GVRP_APP, GVRP_VLAN_REGISTRATION_FORBID, interface, GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)&forbid, 0);
}
