/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  mcast_map.c
*
* @purpose   MCAST Mapping system infrastructure
*
* @component MCAST Mapping Layer
*
* @comments  none
*
* @create    05/17/2006
*
* @author    gkiran
* @end
*
**********************************************************************/

#include "l3_addrdefs.h"
#include "nvstoreapi.h"
#include "l3_defaultconfig.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "dot1q_api.h"
#include "dtl_l3_mcast_api.h"
#include "support_api.h"
#include "mfc_api.h"
#include "mcast_wrap.h"
#include "mcast_util.h"
#include "mcast_map.h"
#include "mcast_debug.h"
#include "mfc_map.h"

/***************************************************
* Global Multicast Routing Information Structures *
***************************************************/
mcastGblVar_t mcastGblVariables_g;

mcastDebugCfg_t mcastDebugCfg;

static
L7_RC_t mcastMapResetCleanUp(void);

static
L7_RC_t mcastMapIntfBuildDefaultConfigData(L7_mcastMapIfCfgData_t *pCfg);

static
L7_RC_t mcastMapAdminScopeCfgApply(void);


static void mcastMapVlanChangeProcess(dot1qNotifyData_t *vlanData, 
                                      L7_uint32 intIfNum, L7_uint32 event);
static void mcastMapRoutingEventProcess (L7_uint32 intIfNum, L7_uint32 event, 
                                         void *pData, L7_BOOL asyncRespReq,
                                         ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
static void mcastMapRouting6EventProcess (L7_uint32 intIfNum, L7_uint32 event, 
                                         void *pData, L7_BOOL asyncRespReq,
                                         ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

static void mcastMapIntfChangeProcess(L7_uint32 intIfNum, 
                                      L7_uint32 event,
                                      NIM_CORRELATOR_t correlator);

static void mcastMapProtoEventChangeProcess(L7_uint32 event, void *pData);

static void mcastMapConfigEventChangeProcess(mcastMapConfigSetEventParms_t configMsg);

static void mcastMapMRPEventProcess(mcastMapMRPEventParms_t mrpEventParms);

/*********************************************************************
*
* @purpose task to handle all Mcast Mapping messages
*
* @param void
*
* @returns void
*
* @comments none
*
* @end
*
*********************************************************************/
void mcastMapTask()
{
  mcastMapMsg_t msg;
  mcastMapAppTmrMsg_t appTmrMsg;
  mcastMapCtrlPktMsg_t pktMsg;

  L7_RC_t       status;
  L7_uint32 QIndex=0;

  mcastGblVariables_g.mcastMapQueue[MCASTMAP_APP_TIMER_Q].QRecvBuffer =  &appTmrMsg;
  mcastGblVariables_g.mcastMapQueue[MCASTMAP_EVENT_Q].QRecvBuffer =  &msg;
  mcastGblVariables_g.mcastMapQueue[MCASTMAP_CTRL_PKT_Q].QRecvBuffer =  &pktMsg;



  osapiTaskInitDone(L7_MCAST_MAP_TASK_SYNC);

  do
  {
    /* Since we are reading from multiple queues, we cannot wait forever 
     * on the message receive from each queue. Rather than sleep between
     * queue reads, use a semaphore to indicate whether any queue has
     * data. Wait until data is available. */  
    if (osapiSemaTake(mcastGblVariables_g.mcastMapMsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\n MCASTMAP : Failed to take message queue semaphore \n");
      continue;
    }

    for(QIndex =0;QIndex < MCASTMAP_MAX_Q; QIndex++)
    {
      status = osapiMessageReceive(mcastGblVariables_g.mcastMapQueue[QIndex].QPointer, 
                                   mcastGblVariables_g.mcastMapQueue[QIndex].QRecvBuffer,
                                   mcastGblVariables_g.mcastMapQueue[QIndex].QSize, 
                                   L7_NO_WAIT);

      if(status == L7_SUCCESS)
      {
        break;
      }
    }

    if(status == L7_SUCCESS)
    {
      mcastGblVariables_g.mcastMapQueue[QIndex].QCurrentMsgCnt--;
      mcastGblVariables_g.mcastMapQueue[QIndex].QRxSuccess ++;

      switch(QIndex)
      {
        case MCASTMAP_APP_TIMER_Q:
          mcastMapProtoEventChangeProcess(appTmrMsg.mcastMapAppTmrParms.event,L7_NULLPTR);
        break;

        case MCASTMAP_EVENT_Q:
        {
          switch (msg.msgId)
          {
            case MCASTMAP_CNFGR_MSG:
            mcastCnfgrCommandProcess(&msg.u.cnfgrCmdData);
            break;

            case MCASTMAP_INTF_CHANGE_MSG:
            mcastMapIntfChangeProcess(msg.u.mcastMapIntfChangeParms.intIfNum, 
            msg.u.mcastMapIntfChangeParms.event, 
            msg.u.mcastMapIntfChangeParms.correlator);
            break;

            case MCASTMAP_ROUTING_EVENT_MSG:
            mcastMapRoutingEventProcess(msg.u.mcastMapRoutingEventParms.intIfNum, 
            msg.u.mcastMapRoutingEventParms.event, 
            msg.u.mcastMapRoutingEventParms.pData, 
            msg.u.mcastMapRoutingEventParms.asyncResponseRequired, 
            &(msg.u.mcastMapRoutingEventParms.eventInfo));
            break;
            case MCASTMAP_ROUTING6_EVENT_MSG:
            mcastMapRouting6EventProcess(msg.u.mcastMapRoutingEventParms.intIfNum, 
            msg.u.mcastMapRoutingEventParms.event, 
            msg.u.mcastMapRoutingEventParms.pData, 
            msg.u.mcastMapRoutingEventParms.asyncResponseRequired, 
            &(msg.u.mcastMapRoutingEventParms.eventInfo));
            break;

            case MCASTMAP_VLAN_EVENT_MSG:
            mcastMapVlanChangeProcess(&msg.u.mcastMapVlanEventParms.vlanData, 
            msg.u.mcastMapVlanEventParms.intIfNum, 
            msg.u.mcastMapVlanEventParms.event);
            break;

            case MCASTMAP_CONFIG_SET_MSG:
            mcastMapConfigEventChangeProcess(msg.u.mcastMapConfigSetEventParms);
            break;

            case MCASTMAP_MRP_EVENT_MSG:
            mcastMapMRPEventProcess(msg.u.mcastMapMRPEventParms);
            break;

            default:
            MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "Invalid message type:%d.\n",msg.msgId);
            MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Invalid Message = %d\n", msg.msgId);
            break;
          }
        }
        break;

        case MCASTMAP_CTRL_PKT_Q:
          mcastMapProtoEventChangeProcess(pktMsg.mcastMapProtocolEventParms.event,
                                        (void *) (&pktMsg.mcastMapProtocolEventParms.msgData));
        break;

        default:
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"invalid Queue type.\n");
        break;
      }
    }
        
  } while (1);
}

/*********************************************************************
*
* @purpose task to handle all IPv6 Packets
*
* @param    none 
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void pktRcvrTask()
{
  osapiTaskInitDone(L7_PKT_RCVR_TASK_SYNC);

  do
  {
    pktRcvrReceive(mcastGblVariables_g.pktRcvr);
  }while(1);
}

/*********************************************************************
*
* @purpose  Addition of a Packet FD to the Packet Receiver 
*
* @param    sockFd      @b{(input)} Socket FD on which the application wants to 
*                                   poll for control packets   
* @param    eventType   @b{(input)} Event Type the application is interested in                 
* @param    buffPoolID  @b{(input)} Buffer pool to be used for pkt receive
* @param    msgQID      @b{(input)} Msg Q ID to send the message to
* @param    pktRcvr     @b{(input)} Data structure for the packer receiver
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to add socket Fds in the packet receiver's structure.
            The fdlist is a sorted one.FD with highest value is the 1st element.  
*
* @end
*
*********************************************************************/
L7_RC_t mcastMapPktRcvrSocketFdRegister (L7_int32 sockFd, L7_uint32 evTypeId, L7_uint32 bufPoolId, 
                                         L7_IPV6_PKT_RCVR_ID_TYPE_t id)
{
  return pktRcvrSocketFdRegister (sockFd, evTypeId, bufPoolId,
                                  id, mcastGblVariables_g.pktRcvr);
}

/*********************************************************************
*
* @purpose  Deletion of a Packet FD from the Packet Receiver
*
* @param    sockFd  @b{(input)}Socket FD to be removed
* @param    pktRcvr @b{(input)}Data structure for the packet Receiver
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to remove a socket Fd from the packet 
*           receiver structure.
*
* @end
*
*********************************************************************/
L7_RC_t mcastMapPktRcvrSocketFdDeRegister (L7_IPV6_PKT_RCVR_ID_TYPE_t id)
{
  return pktRcvrSocketFdDeRegister (id, mcastGblVariables_g.pktRcvr);
}
/********************************************************************
*
* @purpose    Save multicast forwarding config file to NVStore
*
* @param      None
*
* @returns    L7_SUCCESS
*
* @comments   None
*
* @end
*******************************************************************/
L7_RC_t mcastMapSave(void)
{
  if (mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged = L7_FALSE;
    mcastGblVariables_g.mcastMapCfgData.checkSum = 
    nvStoreCrc32((L7_char8 *)&mcastGblVariables_g.mcastMapCfgData,
            sizeof(L7_mcastMapCfg_t) - 
                 sizeof(mcastGblVariables_g.mcastMapCfgData.checkSum));

    if (sysapiCfgFileWrite(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                           L7_MCAST_CFG_FILENAME,
                           (L7_char8 *)&mcastGblVariables_g.mcastMapCfgData,
                           sizeof(L7_mcastMapCfg_t)) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nERROR:"
                      "Error on call to sysapiCfgFileWrite routine for "
                      "filename  %s.\n",  
                      L7_MCAST_CFG_FILENAME);
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Error on call to sysapiCfgFileWrite routine.\n");
    }
  }

  return L7_SUCCESS;
}

/********************************************************************
*
* @purpose    Restore multicast forwarding config file to factory
*             defaults.
*
* @param      none
*
* @returns    L7_SUCCESS
* @retunrs    L7_FAILURE
*
* @comments   None
*
* @end
*
*******************************************************************/
L7_RC_t mcastMapRestore(void)
{
  L7_RC_t retCode = L7_FAILURE;

  if (mcastMapResetCleanUp() == L7_SUCCESS)
  {
    mcastMapBuildDefaultConfigData(L7_MCAST_CFG_VER_CURRENT);
    mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged = L7_TRUE;
    retCode = mcastMapApplyConfigData();
  }
  return retCode;
}

/**********************************************************************
* @purpose  Clean up lower level data structures as part of restoring 
*           to default configuration.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments This routine performs memory clean up of lower level 
*           admin scope data structures
*
* @end
*********************************************************************/
L7_RC_t mcastMapResetCleanUp(void)
{
  if (mcastMapAdminScopeBoundaryFlush() != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to cleanup admin scope Boundaries.\n");
    }
  mcastMapMcastAdminModeDisableAndNotify();
    return L7_SUCCESS;
}

/********************************************************************
* @purpose Restore multicast forwarding config file to factory
*          defaults.
*
* @param   None
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments None
*
* @end
*
****************************************************************/
L7_BOOL mcastMapHasDataChanged(void)
{
  return mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged;
}
void mcastMapResetDataChanged(void)
{ 
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
} 
/***************************************************************
*
* @purpose    Build default multicast forwarding config data
*
* @param      ver       @b{(input)}  version
*
* @returns    void
*
* @comments   None
*
* @end
*
*******************************************************************/
void mcastMapBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 itr;
  L7_uint32 nullIpAddress;
  L7_inet_addr_t nullInetAddr;

  nullIpAddress = 0;
  inetAddressSet(L7_AF_INET, &nullIpAddress, &nullInetAddr);

  /************************
   * Building file header *
   ************************/
  memset((void *)(&mcastGblVariables_g.mcastMapCfgData.cfgHdr),  
                                                    0, sizeof(L7_fileHdr_t));

  strcpy(mcastGblVariables_g.mcastMapCfgData.cfgHdr.filename,
                                                 L7_MCAST_CFG_FILENAME);

  mcastGblVariables_g.mcastMapCfgData.cfgHdr.version          = ver;
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.componentID      
                                            = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.type             
                                           = L7_CFG_DATA;
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.length           
                                           = sizeof(L7_mcastMapCfg_t); 
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged      
                                           = L7_FALSE;

  /********************************
   * Building default config data *
   ********************************/

  /* Static MRoute - ipv4 */
  memset((void *)(&mcastGblVariables_g.mcastMapCfgData.rtr), 0,
                                                 sizeof(L7_mcastMapCfgData_t));

  mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminMode        
                                          = FD_MCAST_DEFAULT_ADMIN_MODE;
  mcastGblVariables_g.mcastMapCfgData.rtr.currentMcastProtocol  
                                          = FD_MCAST_DEFAULT_ROUTING_PROTOCOL;
  mcastGblVariables_g.mcastMapCfgData.rtr.currentMcastV6Protocol  
                                          = FD_MCAST_DEFAULT_ROUTING_PROTOCOL;

  /* Static MRoute - ipv6 */
  if (mcastGblVariables_g.mcastMapV6CfgData != L7_NULLPTR)
  {
    memset((void *)(&mcastGblVariables_g.mcastMapV6CfgData->rtr), 0,
                                                   sizeof(L7_mcastMapCfgData_t));
    mcastGblVariables_g.mcastMapV6CfgData->rtr.mcastAdminMode        
                                            = FD_MCAST_DEFAULT_ADMIN_MODE;
    mcastGblVariables_g.mcastMapV6CfgData->rtr.currentMcastProtocol  
                                            = FD_MCAST_DEFAULT_ROUTING_PROTOCOL;
    mcastGblVariables_g.mcastMapV6CfgData->rtr.currentMcastV6Protocol  
                                            = FD_MCAST_DEFAULT_ROUTING_PROTOCOL;
  }

  for (itr = 0; itr < L7_RTR_MAX_STATIC_MROUTES; itr++)
  {

    /* Static MRoute - ipv4 */
  
    memset((void *)&mcastGblVariables_g.mcastMapCfgData.rtr.
           mcastStaticRtsCfgData[itr].ifConfigId, 0, sizeof(nimConfigID_t));
    inetCopy(&(mcastGblVariables_g.mcastMapCfgData.rtr.
           mcastStaticRtsCfgData[itr].source), &nullInetAddr); 
                                    /* FD_MCAST_DEFAULT_STATIC_SRC; */
    inetCopy(&(mcastGblVariables_g.mcastMapCfgData.rtr.
           mcastStaticRtsCfgData[itr].mask), &nullInetAddr);   
                                    /* FD_MCAST_DEFAULT_STATIC_SRC_MASK; */
    inetCopy(&(mcastGblVariables_g.mcastMapCfgData.rtr.
           mcastStaticRtsCfgData[itr].rpfAddr), &nullInetAddr);
                                    /* FD_MCAST_DEFAULT_STATIC_RPFADDR; */
    mcastGblVariables_g.mcastMapCfgData.rtr.
           mcastStaticRtsCfgData[itr].preference  = FD_MCAST_DEFAULT_STATIC_METRIC;    

    /* Static MRoute - ipv6 */

    if (mcastGblVariables_g.mcastMapV6CfgData != L7_NULLPTR)
    {
      memset((void *)&mcastGblVariables_g.mcastMapV6CfgData->rtr.
             mcastStaticRtsCfgData[itr].ifConfigId, 0, sizeof(nimConfigID_t));
      inetCopy(&(mcastGblVariables_g.mcastMapV6CfgData->rtr.
             mcastStaticRtsCfgData[itr].source), &nullInetAddr); 
                                      /* FD_MCAST_DEFAULT_STATIC_SRC; */
      inetCopy(&(mcastGblVariables_g.mcastMapV6CfgData->rtr.
             mcastStaticRtsCfgData[itr].mask), &nullInetAddr);   
                                      /* FD_MCAST_DEFAULT_STATIC_SRC_MASK; */
      inetCopy(&(mcastGblVariables_g.mcastMapV6CfgData->rtr.
             mcastStaticRtsCfgData[itr].rpfAddr), &nullInetAddr);
                                      /* FD_MCAST_DEFAULT_STATIC_RPFADDR; */
      mcastGblVariables_g.mcastMapV6CfgData->rtr.
             mcastStaticRtsCfgData[itr].preference  = FD_MCAST_DEFAULT_STATIC_METRIC;    
    }
  }

  for (itr = 0; itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES; itr++)
  {
    /* Static MRoute - ipv4 */
    memset((void *)&mcastGblVariables_g.mcastMapCfgData.rtr.
           mcastStaticRtsCfgData[itr].ifConfigId, 0, sizeof(nimConfigID_t));
    mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].inUse
                                         = FD_MCAST_DEFAULT_ADMINSCOPE_INUSE;
    inetCopy(&(mcastGblVariables_g.mcastMapCfgData.rtr.
             mcastAdminScopeCfgData[itr].groupIpAddr), &nullInetAddr);
                                  /* FD_MCAST_DEFAULT_ADMINSCOPE_IPADDR; */
    inetCopy(&(mcastGblVariables_g.mcastMapCfgData.rtr.
             mcastAdminScopeCfgData[itr].groupIpMask), &nullInetAddr);  
                                /* FD_MCAST_DEFAULT_ADMINSCOPE_MASK; */
  }
  /* Static MRoute - ipv4 */
  mcastGblVariables_g.mcastMapCfgData.rtr.numStaticMRouteEntries 
                                 = FD_MCAST_DEFAULT_NUM_STATIC_ENTRIES;
  mcastGblVariables_g.mcastMapCfgData.rtr.numAdminScopeEntries  
                                 = FD_MCAST_DEFAULT_NUM_ADMIN_SCOPE_ENTRIES;

  /* Static MRoute - ipv6 */
  if (mcastGblVariables_g.mcastMapV6CfgData != L7_NULLPTR)
  {
    mcastGblVariables_g.mcastMapV6CfgData->rtr.numStaticMRouteEntries 
                                   = FD_MCAST_DEFAULT_NUM_STATIC_ENTRIES;
  }

  /**********************************
   * Building interface config data *
   **********************************/
  for (itr = 1; itr < L7_IPMAP_INTF_MAX_COUNT; itr++)
  {
    (void)mcastMapIntfBuildDefaultConfigData(&mcastGblVariables_g.
                                             mcastMapCfgData.intf[itr]);
  }

  /* Static MRoute - ipv4 */
  mcastGblVariables_g.mcastMapCfgData.checkSum = 0;

  /* Static MRoute - ipv6 */
  if (mcastGblVariables_g.mcastMapV6CfgData != L7_NULLPTR)
  {
    mcastGblVariables_g.mcastMapV6CfgData->checkSum = 0;
  }
}

/*********************************************************************
* @purpose  Build default MCAST config data for an intf 
*
* @param    pCfg       @b{(output)} pointer to the MCAST 
*                                   config data for the interface
*
* @returns  L7_SUCCESS   default configuration applied
* @returns  L7_FAILURE   could not build default config data for interface
*
* @comments    none 
*
* @end
*********************************************************************/
L7_RC_t mcastMapIntfBuildDefaultConfigData(L7_mcastMapIfCfgData_t *pCfg)
{ 
  L7_RC_t rc = L7_FAILURE;

  if (pCfg != L7_NULLPTR)
  {
    pCfg->ipMRouteIfTtlThresh = FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD;
    rc = L7_SUCCESS;
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Input Parameters not initialized.\n");
  return rc; 
} 

/***************************************************************
*
* @purpose  Apply multicast routing config data
*
* @param    None
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*
***************************************************************/
L7_RC_t mcastMapApplyConfigData(void)
{
  L7_uint32      cfgIndex, intIfNum;

  /* This is commented as both of these checks are anyways hit 
     while enabling/disabling  */
  /*  
  if ((mcastGblVariables_g.mcastMapInfo.mcastInitialized != L7_TRUE) && 
      (ipMapRtrAdminModeGet() == L7_DISABLE))
    return L7_SUCCESS; */

  if (mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminMode == L7_ENABLE)
  {
    mcastMapMcastAdminModeEnableAndNotify();
  } 
  else if (mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminMode == L7_DISABLE)
  {
    mcastMapMcastAdminModeDisableAndNotify();
  } 
  else
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Invalid Admin Mode = %d\n", 
            mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminMode);
    return L7_FAILURE;
  }

  /* create operational data structures for configured
   * boundaries.
   */
  if (mcastGblVariables_g.mcastMapCfgData.rtr.numAdminScopeEntries != 0)
    mcastMapAdminScopeCfgApply();

  /* apply any interface configuration data for interfaces that exist 
   * at this time.
   */
  if (MCAST_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
    {
      if (nimIntIfFromConfigIDGet(&(mcastGblVariables_g.
                                    mcastMapCfgData.intf[cfgIndex].configId), &intIfNum) == L7_SUCCESS)
      {
        mcastMapIntfConfigApply(intIfNum, L7_ENABLE);
      }
    }
  }

  return L7_SUCCESS;
}

/********************************************************
* @purpose  This routine configures internal interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mode       @b{(input)} L7_ENABLE/L7_DISABLE
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*********************************************************/
L7_RC_t mcastMapIntfConfigApply(L7_uint32 intIfNum, L7_uint32 mode)
{

  L7_uint32 rtrMode;
  L7_RC_t   rc;

  rc = ipMapRtrIntfModeGet(intIfNum,&rtrMode);

  if((rc == L7_SUCCESS) && (rtrMode == L7_ENABLE))
  {
    L7_mcastMapIfCfgData_t *pIfCfg = L7_NULLPTR;

    if (mcastMapIntfIsConfigurable(intIfNum, &pIfCfg) == L7_TRUE)
    {
      if (mode == L7_ENABLE)
      {
        mcastMapIntfTtlThreshApply(intIfNum, pIfCfg->ipMRouteIfTtlThresh);
        mcastMapAdminscopeBoundaryInterfaceModeApply(intIfNum,L7_ENABLE);
        return L7_SUCCESS;
      }
      else if (mode == L7_DISABLE)
      {
        mcastMapIntfTtlThreshApply(intIfNum,FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD);
        mcastMapAdminscopeBoundaryInterfaceModeApply(intIfNum,L7_DISABLE);
        return L7_SUCCESS;
      }
      else
      {
        return L7_FAILURE;
      }
    }
    else
    {
      /* As we do not support some Interface (like loopback),
       * just return SUCCESS.
       */
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t mcastMapIntfCreate(L7_uint32 intIfNum) 
{
  nimConfigID_t configId;
  L7_mcastMapIfCfgData_t *pCfg;
  L7_RC_t mcastRC = L7_FAILURE;
  L7_uint32 i;

  if (mcastIntfIsValid(intIfNum) != L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Interface = %d is invalid \n", intIfNum);
    return mcastRC;
  }

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to get nim config Id for intIfNum = %d\n", intIfNum);
    return mcastRC;
  }

  pCfg = L7_NULLPTR;

  /* scan for any preexisting config records that match this configId; 
   * if found, update the config mapping table.
   */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++ )
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&mcastGblVariables_g.
                               mcastMapCfgData.intf[i].configId, &configId))
    {
      mcastGblVariables_g.pMcastMapCfgMapTbl[intIfNum] = i;
      break;
    }
  }

  /* If an interface configuration entry is not already assigned to the 
   * interface, assign one 
   */
  /* 
   * Note: mcastMapIntfIsConfigurable() can return L7_FALSE for 
   * two reasons: no matching config table entry was found,
   * or the intIfNum is not valid for MCAST... the above call to 
   * mcastIntfIsValid() assures that if we get 
   * L7_FALSE it is due to the first reason and we should try to 
   * assign a config table entry for this intf.
   */
  if (mcastMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    if (mcastMapIntfConfigEntryGet(intIfNum, &pCfg) == L7_TRUE)
    {
      /* successfully assigned a new config table entry for this intf, 
       * so initialize the entry to defaults.
       */
      /* Update the configuration structure with the config id */
      NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
      /* since this is a newly assigned configuration entry, initialize 
       * it to default settings 
       */
      mcastRC = mcastMapIntfBuildDefaultConfigData(pCfg);
    }
    else
    {
      /* unsuccessful in assigning config table entry for this intf... 
       * report failure.
       */
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to get config table entry for intIfNum =%d\n",
               intIfNum);
      mcastRC = L7_FAILURE;
    }
  }
  else
  {
    /* we found an existing config table entry for this intf */
    mcastRC = L7_SUCCESS;
  }
  return mcastRC;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t mcastMapIntfDelete(L7_uint32 intIfNum)
{
  L7_mcastMapIfCfgData_t *pCfg;

  /* remove the configuration data for the interface */
  if (mcastMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    mcastGblVariables_g.pMcastMapCfgMapTbl[intIfNum] = 0;
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Interface notifications to MCAST Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    NIM_CORRELATOR_t  @b{(input)} correlator  Correlator for event
*
* @returns  L7_SUCCESS  
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t mcastMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                                   NIM_CORRELATOR_t correlator)
{                          
  mcastMapMsg_t msg;
  NIM_EVENT_COMPLETE_INFO_t status; 

  /* update event status record with results of processing and inform event issuer */
  status.intIfNum     = intIfNum;
  status.component    = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;  

  if (mcastIntfIsValid(intIfNum) != L7_TRUE)
  {
    /* if mcastMap is not interested in this interface, 
     * inform event issuer that we have completed processing 
     */
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES," MCASTMAP : Interface = %d is invalid.\n", intIfNum);     
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES," Interface = %d is invalid.\n", intIfNum);
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  if ((event != L7_CREATE) && (event != L7_DELETE) &&
      (event != L7_ACTIVE) && (event != L7_INACTIVE))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"Event is ignored\n");
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  msg.msgId = MCASTMAP_INTF_CHANGE_MSG;

  msg.u.mcastMapIntfChangeParms.intIfNum = intIfNum;
  msg.u.mcastMapIntfChangeParms.event = event;
  msg.u.mcastMapIntfChangeParms.correlator = correlator;

  if (mcastMapMessageSend(MCASTMAP_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to send interface change event= %d to"
            "mcastMap task\n", event);
    nimEventStatusCallback(status);    
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process interface-related events
*
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    event       @b{(input)} Event On the Interface
* @param    correlator  @b{(input)} Correlator
*
* @returns  none
*
* @end
*********************************************************************/
void mcastMapIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event,
                               NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32  mode;
  NIM_EVENT_COMPLETE_INFO_t status;    

  /* update event status record with results of processing and inform event issuer */
  status.intIfNum     = intIfNum;
  status.component    = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  switch (event)
  {
    case L7_UP:
    case L7_DOWN:
      break;

    case L7_CREATE:
      rc = mcastMapIntfCreate(intIfNum);
      break;

    case L7_ATTACH:
      break;

    case L7_DETACH:
      break;

    case L7_DELETE:
      rc = mcastMapIntfDelete(intIfNum);
      break;

    case L7_ACTIVE:
      if ((mcastMapMcastAdminModeGet(&mode) == L7_SUCCESS) && (mode == L7_ENABLE))
      {
        rc = mcastMapIntfConfigApply(intIfNum, L7_ENABLE);
      }
      break;

    case L7_INACTIVE:
      rc =  mcastMapIntfConfigApply(intIfNum, L7_DISABLE);
      break;

    case L7_LAG_ACQUIRE:
    case L7_PROBE_SETUP:
      break;

    case L7_LAG_RELEASE:
    case L7_PROBE_TEARDOWN:
      break;            

    default:
      break;
  }

  /* update event status record with results of processing and 
   * inform event issuer.
   */
  if (rc != L7_SUCCESS)
  {
    /* log error */
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n MCASTMAP : failed to handle interface status change \n");
  }

  nimEventStatusCallback(status);

  return;
}

/********************************************************************
*
* purpose   Create the lower level admin scope data structures for
*           saved configuration data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This routine is invoked on system start up with saved
*           configuration data
*
* @end
*
********************************************************************/
L7_RC_t mcastMapAdminScopeCfgApply(void)
{
  L7_uint32 itr, entriesChecked = 0;
  L7_uint32 intIfNum;

  for (itr = 0; 
      (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
       numAdminScopeEntries) && (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); 
      itr++)
  {
    if (mcastGblVariables_g.mcastMapCfgData.rtr.
                            mcastAdminScopeCfgData[itr].inUse == L7_TRUE)
    {
      entriesChecked++;
      if (nimIntIfFromConfigIDGet(&mcastGblVariables_g.
                                  mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].ifConfigId, 
          &intIfNum) == L7_SUCCESS)
      {
        if (mcastMapAdminScopeBoundaryAddApply(intIfNum, 
                                               &(mcastGblVariables_g.mcastMapCfgData.rtr.
              mcastAdminScopeCfgData[itr].groupIpAddr),
                                               &(mcastGblVariables_g.mcastMapCfgData.rtr.
             mcastAdminScopeCfgData[itr].groupIpMask)) == L7_FAILURE)
        {
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to apply admin scope changes\n");
          return L7_FAILURE;
        }
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Routing Event notifications to MCAST Map task
*
* @param    intIfNum     @b{(input)} Interface number
* @param    event        @b{(input)} Event type
* @param    pData        @b{(input)} Data
* @param    pEventInfo   @b{(input)} Event Info
*
* @returns  L7_SUCCESS
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t mcastMapRoutingChangeCallback(L7_uint32 intIfNum, 
                                    L7_uint32 event, 
                                    void *pData, 
                                    ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{                          
  mcastMapMsg_t msg;
  L7_uint32     gblAdminMode = L7_DISABLE;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;  

  memset(&msg, 0, sizeof(mcastMapMsg_t));
    
  msg.msgId = MCASTMAP_ROUTING_EVENT_MSG;

  msg.u.mcastMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.mcastMapRoutingEventParms.event = event;
  msg.u.mcastMapRoutingEventParms.pData = pData;

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.mcastMapRoutingEventParms.eventInfo, 
           pEventInfo, sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
    msg.u.mcastMapRoutingEventParms.asyncResponseRequired = L7_TRUE;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId; 
  }
  else
  {
    msg.u.mcastMapRoutingEventParms.asyncResponseRequired = L7_FALSE;
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Routing change callback input "
            "Parameters not initialized.\n");
  }
   
  if ((event != L7_RTR_ENABLE) && 
      (event != L7_RTR_DISABLE_PENDING) &&
      (event != L7_RTR_INTF_ENABLE) && 
      (event != L7_RTR_INTF_DISABLE_PENDING))
  {

    completionData.componentId = L7_IPRT_MCAST;

    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;

    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send event  = %d to mcastMap task\n", event); 
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"Event is ignored\n");    
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  /* Post the event if Multicast Admin mode is enabled.
   */
  if ((mcastMapMcastAdminModeGet (&gblAdminMode) != L7_SUCCESS) || (gblAdminMode != L7_ENABLE))
  {
    /* Return event completion status  */
    completionData.componentId = L7_IPRT_MCAST;

    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }


  if (mcastMapMessageSend(MCASTMAP_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_MCAST;
    completionData.async_rc.rc = L7_FAILURE;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;

    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send  routing change event = %d to "
            "mcastMap task\n", event); 
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This routine is called if router is enabled/disabled for
*           for routing.
*
* @param    intIfNum     @b{(input)} internal interface number 
* @param    event        @b{(input)} event ID
* @param    pData        @b{(input)} Data
* @param    asyncRespReq @b{(input)} async response request
* @param    pEventInfo   @b{(input)} Event Info
*
* @returns  void
*
* @comments none
*
* @end
*
***************************************************************/
void mcastMapRoutingEventProcess (L7_uint32 intIfNum, 
                                  L7_uint32 event,
                                  void *pData, 
                                  L7_BOOL asyncRespReq,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mode;

  if (mcastMapMcastAdminModeGet(&mode) == L7_SUCCESS && mode == L7_ENABLE)
  {
    switch (event)
    {
      case L7_RTR_ENABLE:
        rc = mcastMapMcastAdminModeEnable(L7_AF_INET);
        break;

      case L7_RTR_DISABLE_PENDING:
        rc = mcastMapMcastAdminModeDisable(L7_AF_INET);
        break;

      case L7_RTR_DISABLE:
        break;

      case L7_RTR_INTF_ENABLE:
        if ((mcastMapMcastAdminModeGet(&mode) == L7_SUCCESS) && (mode == L7_ENABLE))
        {
          rc = mcastMapIntfConfigApply(intIfNum, L7_ENABLE);
        }
      break;

      case L7_RTR_INTF_DISABLE_PENDING:
        rc = mcastMapIntfConfigApply(intIfNum, L7_DISABLE);
        break;


      default:
        rc = L7_SUCCESS;
        break;
    } /*end of switch*/
  }

  if (asyncRespReq == L7_TRUE)
  {
    ASYNC_EVENT_COMPLETE_INFO_t completionData;

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_MCAST;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
    completionData.async_rc.rc = rc;
    if (rc == L7_SUCCESS)
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    }
    else
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Routing Event Process Failed for event = %d\n", event);
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
    }
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }

  return;
}


/*********************************************************************
* @purpose  Propogate Routing v6 Event notifications to MCAST Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t mcastMapRouting6ChangeCallback(L7_uint32 intIfNum, 
                                         L7_uint32 event, 
                                         void *pData, 
                                         ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{                          
  L7_uint32 gblAdminMode = L7_DISABLE;
  mcastMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId = L7_IPRT_MCAST;
  if (pEventInfo != L7_NULLPTR)
  {
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
  }
  completionData.async_rc.rc = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;

  /* Filter out the events that PIM-DM Map is not interested in.
   */

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"event is %d",event);

  if ((event != RTR6_EVENT_ENABLE) &&
      (event != RTR6_EVENT_DISABLE_PENDING) &&
      (event != RTR6_EVENT_INTF_ADDR_ADD) &&
      (event != RTR6_EVENT_INTF_ADDR_DEL) &&
      (event != RTR6_EVENT_INTF_ENABLE) &&
      (event != RTR6_EVENT_INTF_DISABLE_PENDING))
  {
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  /* Post the event if Multicast Admin mode is enabled.
   */
  if ((mcastMapMcastAdminModeGet (&gblAdminMode) != L7_SUCCESS) || (gblAdminMode != L7_ENABLE))
  {
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(mcastMapMsg_t));
  msg.msgId = MCASTMAP_ROUTING6_EVENT_MSG;

  msg.u.mcastMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.mcastMapRoutingEventParms.event = event;
  msg.u.mcastMapRoutingEventParms.pData = pData;

  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.mcastMapRoutingEventParms.eventInfo, pEventInfo, 
           sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
    msg.u.mcastMapRoutingEventParms.asyncResponseRequired = L7_TRUE;
  }
  else
  {
    msg.u.mcastMapRoutingEventParms.asyncResponseRequired = L7_FALSE;
  }
   
  if (mcastMapMessageSend(MCASTMAP_EVENT_Q,(L7_VOIDPTR) &msg) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"Failed to post event(%d) to \
                       MCAST Map Queue",event);

    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This routine is called if router is enabled/disabled for
*           for routing v6.
*
* @param    intIfNum     @b{(input)} internal interface number 
* @param    event        @b{(input)} event ID
* @param    pData        @b{(input)} Data
* @param    asyncRespReq @b{(input)} async response request
* @param    pEventInfo   @b{(input)} Event Info
*
* @returns  void
*
* @comments none
*
* @end
*
***************************************************************/
void mcastMapRouting6EventProcess (L7_uint32 intIfNum, 
                                  L7_uint32 event,
                                  void *pData, 
                                  L7_BOOL asyncRespReq,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (event)
  {
    case RTR6_EVENT_ENABLE:
      rc = mcastMapMcastAdminModeEnable(L7_AF_INET6);
      break;

    case RTR6_EVENT_DISABLE_PENDING:
      rc = mcastMapMcastAdminModeDisable(L7_AF_INET6);
      break;

    case RTR6_EVENT_DISABLE:
      break;

    case RTR6_EVENT_INTF_ENABLE:
      /* TBD what to be done */
      break;

    case RTR6_EVENT_INTF_DISABLE_PENDING:
      /* TBD what to be done */
      break;


    default:
      rc = L7_SUCCESS;
      break;
  } /*end of switch*/

  if (asyncRespReq == L7_TRUE)
  {
    ASYNC_EVENT_COMPLETE_INFO_t completionData;

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_MCAST;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
    completionData.async_rc.rc = rc;
    if (rc == L7_SUCCESS)
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    }
    else
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Routing Event Process Failed for event = %d\n", event);
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
    }
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }

  return;
}

/* It looks like this Callback registration is removed with the fast failover 
 * changes.
 * Enable this if we again get back to the VLAN change registration.
 */
#if 0
/*********************************************************************
* @purpose  Propogate VLAN Event notifications to Mcast Map task
*
* @param    vlanId     @b{(input)} Vlan Id
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event
*
* @returns  L7_SUCCESS  
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t mcastMapVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event)
{                          
  mcastMapMsg_t msg;
  L7_uint32 temp;
  dot1qNotifyData_t vlanTemp;
  L7_uint32 vid, vidCount;

  memset(&msg, 0, sizeof(mcastMapMsg_t));
  memcpy(&vlanTemp, vlanData, sizeof(dot1qNotifyData_t));
    
  if (mcastIntfIsValid(intIfNum) != L7_TRUE)
  {
    /* if mcastMap is not interested in this interface, 
     * inform event issuer that we have completed processing 
     */
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Interface  = %d is invalid.\n", intIfNum);
    return L7_SUCCESS;
  }


  msg.msgId = MCASTMAP_VLAN_EVENT_MSG;

  msg.u.mcastMapVlanEventParms.intIfNum = intIfNum;
  msg.u.mcastMapVlanEventParms.event    = event;
  vidCount = 0;
  /* We are only interested about vlans which have a vlan routing interface.
     For VLANS which have none , return success */ 
  if (vlanTemp.numVlans == 1)
  {
    if (dot1qVlanIntfVlanIdToIntIfNum(vlanTemp.data.vlanId, &temp) != L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  else
  {
    /* Loop through all the bits in the vlanData->data.vlanmask and see which are
     * are enabled for routing, prune the list so that the mask contains only those
     * vlans which are enabled for routing. Decrement the numVlans component as well
     */
    for (vid = 1; vid <= L7_VLAN_MAX_MASK_BIT; vid++)
    {
      if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,vid))
      {
        /* This vid is set, is this vid a routing interface? */
        vidCount ++;
        if (dot1qVlanIntfVlanIdToIntIfNum(vid, &temp) != L7_SUCCESS)
        {
          /* No it is not a routing interface, clear this bit in vlanTemp and decrement numVlans */
          L7_VLAN_CLRMASKBIT(vlanTemp.data.vlanMask,vid);
          vlanTemp.numVlans--;

          if (vlanTemp.numVlans == 0)
          {
            /* No vlans which are also routing vlans */
            break;
          }
        }
        /* else we have a vlan bit set that is also a routing interface */
        if (vidCount == vlanData->numVlans)
        {
          /* accounted for all vlans in the mask */
          break;
        }
      }
    }
  }
  if (vlanTemp.numVlans == 0)
  {
    return L7_SUCCESS;
  }
  memcpy(&msg.u.mcastMapVlanEventParms.vlanData, vlanData, sizeof(dot1qNotifyData_t));

  if (mcastMapMessageSend(MCASTMAP_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send Vlan change event = %d "
             "to mcastMap task\n", event);
  }
  return L7_SUCCESS;
}
#endif

/*********************************************************************
*
* @purpose  To process vlan changes
*
* @param    vlanid      @b{(input)} id of the vlan
* @param    intIfNum    @b{(input)} internal interface number 
*                                   for the port affected by event.
* @param    event       @b{(input)} event ID
*
* @returns  none
*
* @comments    
*                                                   
* @end
*********************************************************************/
void mcastMapVlanChangeProcess(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event)
{
  L7_uint32 i = 0, vlanId = 0, numVlans = 0;

   switch (event)
   {
     case VLAN_ADD_NOTIFY:
     case VLAN_DELETE_NOTIFY:
       /* No action, vlan create handled if/when router 
        * interface event happens.
        */
       break;

     case VLAN_ADD_PORT_NOTIFY:
     case VLAN_DELETE_PORT_NOTIFY:
     case VLAN_START_TAGGING_PORT_NOTIFY:
     case VLAN_STOP_TAGGING_PORT_NOTIFY:
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
          dtlMcastVlanPortMemberUpdate(vlanId, intIfNum);
          numVlans++;
       }
       break;

     default:
       break;
   }

   return;
}

/*********************************************************************
* @purpose  To send the protocol(MFC)-related events to mcastMap thread.
*
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    pMsg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Note the IPv6 packets being received are queued in separate 
*            message queue (mcastMapPktMsgQueue) to handover higher priority 
*            to control msg queue.
*
* @end
*********************************************************************/
L7_RC_t mcastMapProtocolMsgQueue(L7_uint32 eventType,L7_VOIDPTR pMsg,
                                 L7_uint32 msgLen)
{
  mcastMapAppTmrMsg_t appTmrMsg;
  mcastMapCtrlPktMsg_t pktMsg;
  L7_RC_t       rc;
  L7_uint32 QIndex=0;
  L7_VOIDPTR sendBuffer = L7_NULLPTR;

   if (eventType == MFC_IPV6_DATA_PKT_RECV_EVENT)
  {
     memset(&pktMsg,0,sizeof(mcastMapCtrlPktMsg_t));
     pktMsg.msgId = MCASTMAP_PROTOCOL_EVENT_MSG;

     pktMsg.mcastMapProtocolEventParms.event = eventType;
     memcpy(&(pktMsg.mcastMapProtocolEventParms.msgData), pMsg, msgLen);

     QIndex = MCASTMAP_CTRL_PKT_Q;
     sendBuffer = (L7_VOIDPTR)&pktMsg;
  }
  else
  {
    memset(&appTmrMsg,0,sizeof(mcastMapAppTmrMsg_t));
    appTmrMsg.msgId = MCASTMAP_PROTOCOL_EVENT_MSG;
    appTmrMsg.mcastMapAppTmrParms.event = eventType;

    QIndex = MCASTMAP_APP_TIMER_Q;
    sendBuffer = (L7_VOIDPTR)&appTmrMsg;


  }
  rc = mcastMapMessageSend(QIndex,sendBuffer);
  if (rc != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nMessage Send Failed"
                    "for eventType:%d.\n", eventType);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send  protocol event to mcastMap task\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process protocol-related events
*
* @param    event       @b{(input)} Event.
* @param    pData       @b{(input)} Data.
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mcastMapProtoEventChangeProcess(L7_uint32 event, void *pData)
{
  if (mfcEventProcess(event, pData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n MCASTMAP : MFC Event = %d Process Failed.\n", event);
    return;
  }
  return;
}

/*********************************************************************
* @purpose  To send configuration events to mcastMap thread 
*
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    pMsg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mcastMapConfigMsgQueue(mcastMapConfigSetEvents_t eventType,
                               L7_VOIDPTR pMsg,
                               L7_uint32 msgLen)
{
  mcastMapMsg_t msg;

  memset(&msg, 0, sizeof(mcastMapMsg_t));
  if ((eventType != MCASTMAP_ADMINMODE_SET_EVENT) &&
      (eventType != MCASTMAP_THRESHOLD_SET_EVENT) &&
      (eventType != MCASTMAP_STATIC_ROUTE_SET_EVENT) &&
      (eventType != MCASTMAP_ADMIN_SCOPE_BOUNDARY_SET_EVENT))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", eventType);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"MCASTMAP : config Event = %d is not handled.\n",eventType);
    return L7_FAILURE;
  }
  msg.msgId = MCASTMAP_CONFIG_SET_MSG;

  msg.u.mcastMapConfigSetEventParms.configEvent = eventType;
  memcpy(&(msg.u.mcastMapConfigSetEventParms.u), pMsg, msgLen);

  if (mcastMapMessageSend(MCASTMAP_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nMessae Send Failed"
                    "for eventType:%d.\n", eventType);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send Config event to mcastMap task\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To send MRP related events to mcastMap thread 
*
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    pMsg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mcastMapMRPEventMsgSend(mcastMapConfigSetEvents_t eventType,
                               L7_VOIDPTR pMsg,
                               L7_uint32 msgLen)
{
  mcastMapMsg_t msg;

  memset(&msg, 0, sizeof(mcastMapMsg_t));
  if ((eventType != MCAST_EVENT_ADMIN_SCOPE_INFO_GET))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", eventType);
    return L7_FAILURE;
  }
  msg.msgId = MCASTMAP_MRP_EVENT_MSG;

  msg.u.mcastMapMRPEventParms.event = eventType;
  memcpy(&(msg.u.mcastMapMRPEventParms.rtrIfNum), pMsg, msgLen);

  if (mcastMapMessageSend(MCASTMAP_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nMessage Send Failed"
                    "for eventType:%d.\n", eventType);
    
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process configuration change related events
*
* @param    configMsg   @b{(input)} configured information
*
* @returns  none
*
* @comments Extracts the configured parameter and sets the configuration
*           data structure accordingly.
*
* @end
*********************************************************************/
void mcastMapMRPEventProcess(mcastMapMRPEventParms_t mrpEventParms)
{ 
  L7_uint32 intIfNum = 0;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "\nEvent received = %d", 
                  mrpEventParms.event);
  if (mrpEventParms.event == MCAST_EVENT_ADMIN_SCOPE_INFO_GET)
  {
    if (mcastIpMapRtrIntfToIntIfNum(L7_AF_INET, mrpEventParms.rtrIfNum, &intIfNum) 
                                    != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS,
                      "Router to internal interface converison failed for rtrIfNum = %d",
                      mrpEventParms.rtrIfNum);
    }
    mcastMapAdminscopeBoundaryInterfaceModeApply(intIfNum,  L7_ENABLE);
  }
}

/*********************************************************************
* @purpose  Process configuration change related events
*
* @param    configMsg   @b{(input)} configured information
*
* @returns  none
*
* @comments Extracts the configured parameter and sets the configuration
*           data structure accordingly.
*
* @end
*********************************************************************/
void mcastMapConfigEventChangeProcess(mcastMapConfigSetEventParms_t configMsg)
{ 
  switch (configMsg.configEvent)
  {
    case MCASTMAP_ADMINMODE_SET_EVENT:
      if (configMsg.u.adminMode == L7_ENABLE)
      {
        mcastMapMcastAdminModeEnableAndNotify();
      }
      else if (configMsg.u.adminMode == L7_DISABLE)
      {
        mcastMapMcastAdminModeDisableAndNotify();
      }
      else
      {
         MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
            "\n MCASTMAP : invalid configMsg.u.adminMode= %d ",configMsg.u.adminMode );
      }
      break;
    case MCASTMAP_THRESHOLD_SET_EVENT:
      {
        mcastMapIntfTtlThreshApply(configMsg.u.ttlEventParms.intIfNum, 
                                   configMsg.u.ttlEventParms.ttl);
      }
      break;
    case MCASTMAP_ADMIN_SCOPE_BOUNDARY_SET_EVENT:
      if (configMsg.u.adminScopeEventParms.mode == L7_TRUE)
      {
        mcastMapAdminScopeBoundaryAddApply(
                                          configMsg.u.adminScopeEventParms.intIfNum,
                                          &configMsg.u.adminScopeEventParms.grpAddr,
                                          &configMsg.u.adminScopeEventParms.grpMask);
      }
      else
      {
        mcastMapAdminScopeBoundaryDeleteApply(
                                             configMsg.u.adminScopeEventParms.intIfNum,
                                             &configMsg.u.adminScopeEventParms.grpAddr,
                                             &configMsg.u.adminScopeEventParms.grpMask);
      }
      break;
    default:
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n MCASTMAP : invalid config event = %d ",configMsg.configEvent );
      break;
  }
  return;
}

/*********************************************************************
* @purpose  Obtain APP Timer Buffer Pool ID       
* 
* @param    bufPoolId   @b{(inout)} Buffer Pool Id
*
* @returns  App Timer Buffer Pool Id
*
* @comments This buffer is for internal use only by the APP Timer.
*
* @end
*********************************************************************/
L7_RC_t mcastMapGetAppTimerBufPoolId (L7_uint32 *bufPoolId)
{
  if(mcastGblVariables_g.mcastAppTimerBufPoolId)
  {
    *bufPoolId = mcastGblVariables_g.mcastAppTimerBufPoolId;
    return L7_SUCCESS;
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n MCASTMAP : Failed to get  apptimer buffer poolId\n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain MCAST Handle List Memory Handle
* 
* @param    family @b{(input)} IP Address family
* @param    mcastHandleListHandle @b{(output)} Handle to the Handle List
*
* @returns  L7_SUCCESS, on success
* @returns  L7_FAILURE, on failure
*
* @comments This buffer is for internal use only by the Handle List of
*           the MCAST Component.
*
* @end
*********************************************************************/
L7_RC_t mcastMapGetHandleListHandle (L7_uint32 family, void** mcastHandleListMemHandle)
{
  if(family == L7_AF_INET)
  {
    if(mcastGblVariables_g.mcastHandleListMemHndl != L7_NULLPTR)
    {
      *mcastHandleListMemHandle = mcastGblVariables_g.mcastHandleListMemHndl;
      return L7_SUCCESS;
    }
  }
  else if(family == L7_AF_INET6)
  {
    if(mcastGblVariables_g.mcastV6HandleListMemHndl != L7_NULLPTR)
    {
      *mcastHandleListMemHandle = mcastGblVariables_g.mcastV6HandleListMemHndl;
      return L7_SUCCESS;
    }
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,
                   "MCASTMAP: Failed to get Handle List Handle.\n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain Multicast HeapID
* 
* @param    family @b{(input)} IP Address family
*
* @returns  heapId, on success
* @returns  L7_NULL, on failure
*
* @comments This heapID is to be used for all the memory allocations for the
*           given IP address family within the Multicast module
*
* @end
*********************************************************************/
L7_uint32 mcastMapHeapIdGet (L7_uint32 family)
{
  if(family == L7_AF_INET)
  {
    return mcastGblVariables_g.mcastV4HeapId;
  }
  else if(family == L7_AF_INET6)
  {
    return mcastGblVariables_g.mcastV6HeapId;
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                  "\n MCASTMAP : Failed to get heap ID for family %d\n", family);
  return L7_NULL;
}


/*********************************************************************
* @purpose  Wrapper function to send the the message on the appropriate Q
*
* @param    QIndex    @b{(input)} Index to the Queue on which message is to be sent.
* @param    msg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/

L7_RC_t mcastMapMessageSend(MCASTMAP_QUEUE_ID_t QIndex,void *msg)
{

  if (osapiMessageSend (mcastGblVariables_g.mcastMapQueue[QIndex].QPointer, (L7_VOIDPTR) msg,
                        mcastGblVariables_g.mcastMapQueue[QIndex].QSize, L7_NO_WAIT, L7_MSG_PRIORITY_NORM)
                     != L7_SUCCESS)
  {
    mcastGblVariables_g.mcastMapQueue[QIndex].QSendFailedCnt++;

    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "MCAST Map Queue of queueIndex:%d  Failed while \
                      sending the message :.\n", QIndex);
    return L7_FAILURE;
  }
  else
  {
    
     mcastGblVariables_g.mcastMapQueue[QIndex].QCurrentMsgCnt++;

     if(mcastGblVariables_g.mcastMapQueue[QIndex].QCurrentMsgCnt >
               mcastGblVariables_g.mcastMapQueue[QIndex].QMaxRx)
     {
       mcastGblVariables_g.mcastMapQueue[QIndex].QMaxRx =
                 mcastGblVariables_g.mcastMapQueue[QIndex].QCurrentMsgCnt;
     }
     
     osapiSemaGive(mcastGblVariables_g.mcastMapMsgQSema);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Wrapper function to send the the message on the appropriate Q
*
* @param    QIndex    @b{(input)} Index to the Queue on which message is to be sent.
* @param    msg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/

L7_RC_t mcastMapQDelete(void)
{
  L7_uint32 count;

  for(count =0; count <MCASTMAP_MAX_Q;count++)
  {
    if(mcastGblVariables_g.mcastMapQueue[count].QPointer != L7_NULLPTR)
    {
      osapiMsgQueueDelete(mcastGblVariables_g.mcastMapQueue[count].QPointer);
    }
  }
  return L7_SUCCESS;
}


/********************** MCAST DEBUG TRACE ROUTINES *******************/

/*********************************************************************
 * @purpose  Saves mcast debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    mcastDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t mcastDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  mcastDebugCfgUpdate();

  if (mcastDebugHasDataChanged() == L7_TRUE)
  {
    mcastDebugCfg.hdr.dataChanged = L7_FALSE;
    mcastDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&mcastDebugCfg,
        (L7_uint32)(sizeof(mcastDebugCfg) - sizeof(mcastDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_FLEX_MCAST_MAP_COMPONENT_ID, MCAST_DEBUG_CFG_FILENAME ,
            (L7_char8 *)&mcastDebugCfg, (L7_uint32)sizeof(mcastDebugCfg_t))) == L7_ERROR)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Error on call to osapiFsWrite routine on config file %s\n",MCAST_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Restores mcast debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    mcastDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t mcastDebugRestore(void)
{
  L7_RC_t rc;

  mcastDebugBuildDefaultConfigData(MCAST_DEBUG_CFG_VER_CURRENT);

  mcastDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = mcastApplyDebugConfigData();

  return rc;
}
/*********************************************************************
 * @purpose  Checks if mcast debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL mcastDebugHasDataChanged(void)
{
  return mcastDebugCfg.hdr.dataChanged;
}

/*********************************************************************
 * @purpose  Build default mcast config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void mcastDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  mcastDebugCfg.hdr.version = ver;
  mcastDebugCfg.hdr.componentID = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  mcastDebugCfg.hdr.type = L7_CFG_DATA;
  mcastDebugCfg.hdr.length = (L7_uint32)sizeof(mcastDebugCfg_t);
  strcpy((L7_char8 *)mcastDebugCfg.hdr.filename, MCAST_DEBUG_CFG_FILENAME);
  mcastDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&mcastDebugCfg.cfg, 0, sizeof(mcastDebugCfg.cfg));
}

/*********************************************************************
 * @purpose  Apply mcast debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t mcastApplyDebugConfigData(void)
{
  L7_uchar8 familyIndex = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 flagIndex;

  for (familyIndex = 0; familyIndex < MCAST_MAP_CB_MAX; 
       familyIndex++)
  {
    mcastDebugTraceFlags_t *mcastDebugTraceFlags = 
       &(mcastDebugCfg.cfg.mcastDebugTraceFlag[familyIndex]);
    for(flagIndex = 0;  flagIndex < MCAST_DEBUG_LAST_TRACE ; flagIndex ++)
    { 
       if(((*(mcastDebugTraceFlags[flagIndex/MCAST_DEBUG_TRACE_FLAG_BITS_MAX])) & 
          (MCAST_DEBUG_TRACE_FLAG_VALUE << (flagIndex % MCAST_DEBUG_TRACE_FLAG_BITS_MAX))) != 0)
       {
         mcastDebugTraceFlagSet(familyIndex+1, flagIndex, L7_TRUE);
       }
       else
       {
         mcastDebugTraceFlagSet(familyIndex+1, flagIndex, L7_FALSE);
       }
    }
  }
  return rc;
}


