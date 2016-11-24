/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_main.c
*
* @purpose Contains definitions for initialization, send and receive routines to
*          support the DVMRP protocol.
*
* @component
*
* @comments
*
* @create 03/03/2006
*
* @author Prakash/Shashidhar
* @end
*
********************************************************************************/
#include "dvmrp_main.h"
#include "dvmrp_mfc.h"
#include "dvmrp_nbr.h"
#include "dvmrp_igmp.h"
#include "dvmrp_timer.h"
#include "dvmrp_util.h"
#include "dvmrp_protocol.h"
#include "dvmrp_interface.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_igmp_group_table.h"
#include "dvmrp_vend_ctrl.h"
#include "dvmrp_map_debug.h"


/*********************************************************************
* @purpose   This function initializes all the control block parameters
*
* @param    dvmrpCB    @b{ (input) }Pointer to the  DVMRP Control Block.
*     
* @returns  L7_SUCCESS - Protocol has been initialized properly. *
* @returns  L7_ERROR   - Protocol has not been initialized properly.
*
* @notes    none.
*        
* @end
*********************************************************************/
L7_int32 dvmrpAdminModeSet(dvmrp_t *dvmrpcb)
{
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_ERROR;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
              "%s:%d\n\nStarted dvmrp_timer_update every (60sec)\n\n",
              __FUNCTION__,__LINE__);
  dvmrpcb->updateTimerHandle =
  handleListNodeStore(dvmrpcb->handle_list, (void*)dvmrpcb);

  if (L7_NULLPTR == (dvmrpcb->timer = appTimerAdd(dvmrpcb->timerHandle,
                                                  dvmrp_timer_update,
                                                  (void *)dvmrpcb->updateTimerHandle,
                                                  DVMRP_UPDATE_INTERVAL,
                                                  "DV-UPD")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d timer instantiation failed \n", 
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:%d timer instantiation failed \n",
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:%d timer instantiation failed \n", 
                __FUNCTION__,__LINE__);
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->timer);
    if (dvmrpcb->updateTimerHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &dvmrpcb->updateTimerHandle);
      dvmrpcb->updateTimerHandle = L7_NULL;
    }
    return L7_FAILURE;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,
              "%s:%d\n\nStarted dvmrp_routes_timeout every (140sec)\n\n",
              __FUNCTION__,__LINE__);
  dvmrpcb->routesTimeoutHandle =
  handleListNodeStore(dvmrpcb->handle_list, (void*)dvmrpcb);

  if (L7_NULLPTR == (dvmrpcb->age = appTimerAdd(dvmrpcb->timerHandle,
                                                dvmrp_routes_timeout,
                                                (void *)dvmrpcb->routesTimeoutHandle,
                                                DVMRP_TIMEOUT_INTERVAL,
                                                "DV-RT")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d timer instantiation failed \n", 
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:%d timer instantiation failed \n", 
                __FUNCTION__,__LINE__);
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->age);
    if (dvmrpcb->routesTimeoutHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &dvmrpcb->routesTimeoutHandle);
      dvmrpcb->routesTimeoutHandle = L7_NULL;
    }
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->timer);
    if (dvmrpcb->updateTimerHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &dvmrpcb->updateTimerHandle);
      dvmrpcb->updateTimerHandle = L7_NULL;
    }
    dvmrpcb->timer=L7_NULLPTR;
    return L7_FAILURE;
  }

  dvmrpcb->pruneExpireHandle =
  handleListNodeStore(dvmrpcb->handle_list, (void*)dvmrpcb);

  if (L7_NULLPTR == (dvmrpcb->expire = appTimerAdd(dvmrpcb->timerHandle,
                                                   dvmrp_prune_expire,
                                                   (void *)dvmrpcb->pruneExpireHandle,
                                                   DVMRP_PRUNE_TIMEOUT_INTERVAL,
                                                   "DV-PT")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d timer instantiation failed \n", 
                __FUNCTION__,__LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:%d prune timer instantiation failed \n",
                __FUNCTION__,__LINE__);
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->age);
    if (dvmrpcb->routesTimeoutHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &dvmrpcb->routesTimeoutHandle);
      dvmrpcb->routesTimeoutHandle = L7_NULL;
    }
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->expire);
    if (dvmrpcb->pruneExpireHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &dvmrpcb->pruneExpireHandle);
      dvmrpcb->pruneExpireHandle = L7_NULL;
    }
    dvmrpcb->expire=L7_NULLPTR;
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->timer);
    if (dvmrpcb->updateTimerHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &dvmrpcb->updateTimerHandle);
      dvmrpcb->updateTimerHandle = L7_NULL;
    }
    dvmrpcb->timer=L7_NULLPTR;
    return L7_FAILURE;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,"%s:%d prune timer instantiation successful\n",
              __FUNCTION__,__LINE__);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  This function is called to deinit DVMRP
*
* @param    dvmrpCB    @b{ (input) }Pointer to the  DVMRP Control Block.
* 
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 dvmrpAdminModeReset(dvmrp_t *dvmrpcb)
{
  L7_uint32 ret = L7_SUCCESS;
  L7_uint32 rtrIfNum;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  for (rtrIfNum =1;rtrIfNum < MAX_INTERFACES;rtrIfNum++)
  {
    if (MCAST_BITX_TEST(dvmrpcb->interface_mask, rtrIfNum))
    {
      dvmrp_interface_activate(dvmrpcb,rtrIfNum,L7_DISABLE,L7_NULLPTR,L7_NULLPTR);  
    }
  }

  if(dvmrpcb->expire != L7_NULLPTR)
  {
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->expire);
    dvmrpcb->expire = L7_NULLPTR;
  }

  if (dvmrpcb->pruneExpireHandle != L7_NULL)
  {
    handleListNodeDelete(dvmrpcb->handle_list,
                         &dvmrpcb->pruneExpireHandle);
  }
  
  if(dvmrpcb->age != L7_NULLPTR)
  {
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->age);
    dvmrpcb->age = L7_NULLPTR;  
  }
  if (dvmrpcb->routesTimeoutHandle != L7_NULL)
  {
    handleListNodeDelete(dvmrpcb->handle_list,
                         &dvmrpcb->routesTimeoutHandle);
  }
  
  if(dvmrpcb->timer != L7_NULLPTR)
  {
    appTimerDelete(dvmrpcb->timerHandle, dvmrpcb->timer);
    dvmrpcb->timer = L7_NULLPTR;  
  }

  if (dvmrpcb->updateTimerHandle != L7_NULL)
  {
    handleListNodeDelete(dvmrpcb->handle_list,
                         &dvmrpcb->updateTimerHandle);
  }
  
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(ret);
}


/*********************************************************************
* @purpose  This function process DVMRP Events.
*
* @param    familyType     @b{ (input) } address Family.
* @param    eventType      @b{ (input) } Event.
* @param    pMsg           @b{ (input) } Message.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  This function recieves all the messages and events and 
*            call the appropriate handler
* @end
*********************************************************************/
L7_RC_t dvmrpEventProcess(L7_uchar8 familyType, 
                          mcastEventTypes_t eventType, 
                          void *pMsg)
{
  dvmrp_t           *dvmrpcb = L7_NULLPTR;  

  /* DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__); */

  /* DVMRP supports IPv4 only in current implementation.*/
  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (familyType)) == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  switch (eventType)
  {
    case MCAST_MFC_NOCACHE_EVENT:
      {
        dvmrp_src_grp_entry_t srcGrpEntry;
        mfcEntry_t  *mfc_node = L7_NULLPTR;
        L7_char8 src[DVMRP_STRING_SIZE], grp[DVMRP_STRING_SIZE];

        DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s :MCAST_MFC_NOCACHE_EVENT rcvd\n\n",
                    __FUNCTION__);

        if (dvmrpcb->dvmrpOperFlag != L7_ENABLE)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nDVMRP NOT ENABLED.\n");
          return L7_FAILURE;
        }

        mfc_node = (mfcEntry_t *) pMsg;

        if (inetAddrHtop(&mfc_node->source, src) != L7_SUCCESS)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
          return L7_FAILURE;
        }
        if (inetAddrHtop(&mfc_node->group, grp) != L7_SUCCESS)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
          return L7_FAILURE;
        }
        DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s :src=%s, grp=%s on iif=%d\n\n",
                    __FUNCTION__, src, grp,mfc_node->iif);
        memset(&srcGrpEntry, L7_NULL, sizeof(dvmrp_src_grp_entry_t));
        inetCopy(&srcGrpEntry.source, &mfc_node->source);
        inetCopy(&srcGrpEntry.group, &mfc_node->group);
        srcGrpEntry.iif = mfc_node->iif;
        if (inetAddrHtop(&mfc_node->source, src) != L7_SUCCESS)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
          return L7_FAILURE;
        }
        if (inetAddrHtop(&mfc_node->group, grp) != L7_SUCCESS)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
          return L7_FAILURE;
        }

        DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                    "%s:%d NOCACHE EVENT for src=%s,grp=%s\n",
                    __FUNCTION__,__LINE__, src, grp);
        dvmrpMfcNoCache(dvmrpcb, &srcGrpEntry);
      }
      break;

    case MCAST_MFC_ENTRY_EXPIRE_EVENT:
      {
        dvmrp_cache_entry_t *entry = L7_NULLPTR;
        mfcEntry_t  *mfc_node = L7_NULLPTR;
        L7_uint32 expiryTime;
        L7_uint32 now;


        DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,
                    "%s :MCAST_MFC_ENTRY_EXPIRE_EVENT rcvd\n\n",__FUNCTION__);
        if (dvmrpcb->dvmrpOperFlag != L7_ENABLE)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nDVMRP NOT ENABLED.\n");
          return L7_FAILURE;
        }

        now = osapiUpTimeRaw();

        mfc_node = (mfcEntry_t *) pMsg;                

        entry = dvmrp_cache_lookup(dvmrpcb, 
                                   &mfc_node->source, &mfc_node->group);

        if (entry == L7_NULLPTR)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d MCAST_MFC_ENTRY_EXPIRE_EVENT"
                      "recieved for unknown cache entry. \n", 
                      __FUNCTION__,__LINE__);
          return L7_FAILURE;
        }

        BIT_SET (entry->flags, DVMRP_CACHE_DELETE);
        if (entry->expire < now)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, "%s:%d\n\n(entry->expire < now)"
                      "so will be removed immediately \n\n",
                      __FUNCTION__,__LINE__);
          dvmrpCacheRemove(dvmrpcb,entry);          
        }
        else
        {
          expiryTime = entry->expire - now;
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                      "%s:%d\n\n(entry->expire > now)so will be removed"
                      "after (%d sec) \n\n",
                      __FUNCTION__,__LINE__,expiryTime);
        

        DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,
                    "%s:%d\n\nStarted dvmrp_Cache_Remove (%dsec)\n\n",
                    __FUNCTION__,__LINE__, expiryTime);
        entry->cacheRemoveHandle =
        handleListNodeStore(dvmrpcb->handle_list, (void*)entry);

        if (L7_NULLPTR == (entry->cacheRemove_timer = 
                           appTimerAdd(dvmrpcb->timerHandle,
                                       dvmrp_Cache_Remove,
                                       (void *)entry->cacheRemoveHandle,
                                       expiryTime,
                                       "DV-CT")))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:%d timer instantiation failed \n",
                      __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,
                      "%s:%d timer instantiation failed \n",
                      __FUNCTION__,__LINE__);
        }
      }
      }
      break;        

    case MCAST_EVENT_MGMD_GROUP_UPDATE:
      {
        mgmdMrpEventInfo_t *grp_info = L7_NULLPTR; 
        L7_char8 grp[DVMRP_STRING_SIZE];

        grp_info = (mgmdMrpEventInfo_t *) pMsg;

        if (dvmrpcb->dvmrpOperFlag != L7_ENABLE)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d DVMRP NOT ENABLED.\n",
                      __FUNCTION__,__LINE__);
          return L7_FAILURE;
        }

        if (inetAddrHtop(&grp_info->group, grp) != L7_SUCCESS)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
          return L7_FAILURE;
        }
        DVMRP_DEBUG (DVMRP_DEBUG_EVENTS, "\n[%s-%d]: MCAST_EVENT_MGMD_GROUP_UPDATE Received.\n", __FUNCTION__, __LINE__);
        DVMRP_DEBUG (DVMRP_DEBUG_GROUP, "[%s-%d]: Group-%s, rtrIfNum-%d, Mode-%d, numSrcs-%d.\n",
                     __FUNCTION__, __LINE__, grp, grp_info->rtrIfNum, grp_info->mode, grp_info->numSrcs);
        dvmrpGroupSourcesUpdate(dvmrpcb, grp_info, MCAST_EVENT_MGMD_GROUP_UPDATE);
        dvmrpCacheTableUpdateForGrpUpdateEvent(dvmrpcb,grp_info);

        /* Free the MCAST MGMD Events Source List Buffers */
        /* If the return status is taken into consideration for above processing,
           then the following BufferPoolFree needs to be done for successful returns 
           only, as the failure case frees are handled by caller of this routine.*/
        mcastMgmdEventsBufferPoolFree (dvmrpcb->family, grp_info);
      }
      break;        

    case MCAST_EVENT_MGMD_QUERIER:
      {
        L7_int32 intf_index;

        DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,
                    "%s :MCAST_EVENT_MGMD_GROUP_DELETE rcvd\n\n",__FUNCTION__);

        if (dvmrpcb->dvmrpOperFlag != L7_ENABLE)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nDVMRP NOT ENABLED.\n");
          return L7_FAILURE;
        }

        intf_index = *((L7_int32 *)(pMsg));
        if (MCAST_BITX_TEST(dvmrpcb->interface_mask, intf_index))
        {
          dvmrpcb->dvmrp_interfaces[intf_index].igmp_querier = eventType;
        }
      }
      break;

    case MCAST_EVENT_MGMD_NO_QUERIER:
      {
        L7_int32 intf_index;

        DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,
                    "%s :MCAST_EVENT_MGMD_NO_QUERIER rcvd\n\n",__FUNCTION__);

        if (dvmrpcb->dvmrpOperFlag != L7_ENABLE)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nDVMRP NOT ENABLED.\n");
          return L7_FAILURE;
        }

        intf_index = *((L7_int32 *)(pMsg));
        if (MCAST_BITX_TEST(dvmrpcb->interface_mask, intf_index))
        {
          dvmrpcb->dvmrp_interfaces[intf_index].igmp_querier = eventType;
        }
      }
      break;

    case MCAST_EVENT_DVMRP_CONTROL_PKT_RECV:
    {
      mcastControlPkt_t *dvmrpMsg = L7_NULLPTR;
      dvmrpMsg = (mcastControlPkt_t *) pMsg;
      dvmrpCtrlPktHandler(dvmrpcb,dvmrpMsg);
      mcastCtrlPktBufferPoolFree(L7_AF_INET, dvmrpMsg->payLoad);
    }
    break;
    case MCAST_DVMRP_TIMER_EXPIRY_EVENT:
      {
        /* DVMRP_DEBUG(DVMRP_DEBUG_TIMERS,
                    "%s :DVMRP_TIMER_EXPIRY_EVENT rcvd\n\n",__FUNCTION__); */
        appTimerProcess(pMsg);
      }
      break;
    case MCAST_EVENT_ADMINSCOPE_BOUNDARY:
      {
        mcastAdminMsgInfo_t *adminScopeMsg =L7_NULLPTR;
        DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,
                    "%s :MCAST_EVENT_ADMINSCOPE_BOUNDARY rcvd\n\n",
                    __FUNCTION__);
        if (dvmrpcb->dvmrpOperFlag != L7_ENABLE)
        {
          DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nDVMRP NOT ENABLED.\n");
          return L7_FAILURE;
        }

        adminScopeMsg = (mcastAdminMsgInfo_t *) pMsg;

        if (dvmrpAdminScopeBoundaryEventHandler(dvmrpcb,
                                                adminScopeMsg) != L7_SUCCESS)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "Failed to process the admin scope message!!!\n");

        }
      }
      break;
    default:
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Invalid Event - %d received", eventType);          
      return L7_FAILURE;
      /* passthru */
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function is used to handle the dvmrp ctrl pkt.
*
* @param    dvmrpcb   -  @b{(input)} DVMRP control block Handle.
* @param    dvmrpMsg -  @b{(input)} dvmrp  Msg.
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCtrlPktHandler(dvmrp_t *dvmrpcb, mcastControlPkt_t * dvmrpMsg)
{
  dvmrp_interface_t *interface = L7_NULLPTR;
  struct igmp *igmp2 = L7_NULLPTR;
  L7_int32  delay, code;    
  L7_dvmrp_inet_addr_t group;
  L7_ulong32 versionSupport=0;
  L7_dvmrp_inet_addr_t source;
  L7_int32 igmp_code=1;
  L7_uchar8 *data = L7_NULLPTR;
  L7_int32 datalen=0;
  L7_int32 index=0;
  L7_uchar8 printString[MAX_STRING_LENGTH];

  dvmrpDebugPacketRxTxTrace(L7_TRUE, dvmrpMsg->rtrIfNum, 
                            &dvmrpMsg->srcAddr, &dvmrpMsg->destAddr,
                            dvmrpMsg->payLoad,  dvmrpMsg->length);

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,
              "%s :MCAST_EVENT_DVMRP_CONTROL_PKT_RECV rcvd\n\n",
              __FUNCTION__);

  if((dvmrpcb == L7_NULLPTR)||(dvmrpMsg == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Input parameters are not proper.!\n", 
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (dvmrpcb->dvmrpOperFlag != L7_ENABLE)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d DVMRP NOT ENABLED !\n", 
                __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }


  if (dvmrpMsg->family == L7_AF_INET)
  {
    igmp2 = (struct igmp *) dvmrpMsg->payLoad;

    if (L7_NULL != inetChecksum(igmp2, dvmrpMsg->length))
    {
      if (inetAddrHtop(&source.addr,printString) != L7_SUCCESS)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
        return L7_FAILURE;
      }
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "packet from %s len %d bad igmp checksum\n",
                  printString, dvmrpMsg->length);
      /* added this code to support Dvmrp mib for bad pkts count */
      if (igmp2->igmp_type == L7_IGMP_DVMRP)
      {
        igmp2->igmp_code = L7_IGMP_BAD_CHKSUM;
      }
      else
      {
        return L7_FAILURE;
      }
    }

    code = delay = igmp2->igmp_code;
    /* Convert the igmp-Header fields from Network to Host Byte Order */
    igmp2->igmp_cksum = osapiNtohs (igmp2->igmp_cksum);
    igmp2->igmp_group.s_addr = osapiNtohl (igmp2->igmp_group.s_addr);
    memset(&group, 0, sizeof(L7_dvmrp_inet_addr_t));
    group.maskLength = L7_NULL;
    if (L7_SUCCESS != inetAddressSet(dvmrpcb->family, 
                                     &(igmp2->igmp_group.s_addr), &(group.addr)))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address !\n", 
                  __FUNCTION__, __LINE__);

      return L7_FAILURE;
    }
    index = dvmrpMsg->rtrIfNum;
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &group.addr, 
                                     &versionSupport))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't get the IP ADDRESS !\n", 
                  __FUNCTION__, __LINE__);

      return L7_FAILURE;
    }
    inetCopy(&source.addr,&dvmrpMsg->srcAddr);
    igmp_code = code;
    data  = (L7_uchar8 *)(igmp2+1);
    datalen = dvmrpMsg->length - sizeof(*igmp2);

    if ((datalen <= 0) ||
        (datalen> DVMRP_MAX_PDU))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Ctrl Pkt Payload Length is out of bounds !\n", 
                  __FUNCTION__, __LINE__);


      return L7_FAILURE;
    }

    interface = &dvmrpcb->dvmrp_interfaces [index];
    if (interface->index == index)
    {
      dvmrp_recv(interface, versionSupport,&source, igmp_code, data, 
                 datalen);
    }
  }
  else if (dvmrpMsg->family == L7_AF_INET6)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d IPV6 NOT SUPPORTED !\n", 
                __FUNCTION__, __LINE__);


    return L7_FAILURE;
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Unknown family Type !\n", 
                __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the DVMRP control packets
*
* @param    interface      -  @b{(input)} Interface through which the
*                                         packet has been recieved.
* @param    versionSupport -  @b{(input)} Verison supported by the
*                                         neighbouring router
* @param    source         -  @b{(input)} Source address of the packet
* @param    dvmrp_pkt_type -  @b{(input)} type of the packet
* @param    data           -  @b{(input)} pointer to the packet buffer
* @param    datalen        -  @b{(input)} length of the packet
*
* @returns  L7_SUCCESS - Packet has successfully been processed.
* @returns  L7_ERROR   - Packet processing encountered errors
*
* @notes    Packet type is identified and the corresponding 
*           handler is called.
*
* @end
*********************************************************************/
L7_int32 dvmrp_recv (dvmrp_interface_t *interface, 
                     L7_ulong32 versionSupport, 
                     L7_dvmrp_inet_addr_t *source,
                     L7_int32 dvmrp_pkt_type, 
                     L7_uchar8 *data, L7_int32 datalen)
{
  L7_uchar8 *data_end = data + datalen;
  L7_uchar8 sendbuf[DVMRP_MAX_PDU];
  L7_uchar8 *cp = sendbuf;
  L7_int32 major = versionSupport & 0xff;
  L7_int32 minor = (versionSupport >> 8) & 0xff;
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n", 
                __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }


  if (!MCAST_BITX_TEST(dvmrpcb->interface_mask, interface->index))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrp is not enabled on interface %d \n", 
                __FUNCTION__, __LINE__, interface->index);
    return(L7_FAILURE);
  }
  nbr = dvmrp_neighbor_lookup (interface, versionSupport, source);

  if (dvmrp_pkt_type == L7_IGMP_BAD_CHKSUM)
  {
    dvmrp_badpkts_inc(interface,nbr);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d BAD PKT Recieved!\n", 
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if ((dvmrp_pkt_type <= 0) || (dvmrp_pkt_type > DVMRP_INFO_REPLY))
  {
    return(L7_FAILURE);
  }

  if (major == 10 || major == 11 || major < 3 || (major == 3 &&
                                                  minor < 5))
  {
    /* for backward compatibility*/
    if (dvmrp_pkt_type != DVMRP_REPORT)
    {
      dvmrp_badpkts_inc(interface,nbr);
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d BAD PKT Recieved!\n", 
                  __FUNCTION__, __LINE__);
      return(L7_FAILURE);
    }
  }

  /* Identify the corresponding dvmrp interface and pass it as a pointer   */
  switch (dvmrp_pkt_type)
  {
    case DVMRP_PROBE:
      if (dvmrp_probe_recv (interface, versionSupport, source, data, datalen) != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d BAD PROBE PKT Recieved!\n", 
                    __FUNCTION__, __LINE__);
        interface->badProbePkts++;
        return(L7_FAILURE);
      }
      interface->probePktsRecievedCount++;
      break;
    case DVMRP_REPORT:
      if (dvmrp_report_recv (interface, versionSupport, source, data, datalen) != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d BAD REPORT PKT Recieved!\n", 
                    __FUNCTION__, __LINE__);
        interface->badReportPkts++;
        return(L7_FAILURE);
      }
      interface->reportPktsRecievedCount++;;
      break;
    case DVMRP_ASK_NEIGHBORS:
    case DVMRP_NEIGHBORS:
      break;
    case DVMRP_ASK_NEIGHBORS2:
      if (dvmrp_neighbors2 (interface, source) != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d BAD NBR2 PKT Recieved!\n", 
                    __FUNCTION__, __LINE__);
        interface->badNbr2Pkts++;        
        return(L7_FAILURE);
      }
      interface->nbr2PktsRecievedCount++;;      
      break;
    case DVMRP_NEIGHBORS2:
      break;
    case DVMRP_PRUNE:
      if (dvmrp_prune_recv (interface, versionSupport, source, data, datalen) != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d BAD PRUNE PKT Recieved!\n", 
                    __FUNCTION__, __LINE__);
        interface->badPrunePkts++;        
        return(L7_FAILURE);
      }
      interface->prunePktsRecievedCount++;;            
      break;
    case DVMRP_GRAFT:       /* -- */
      if (dvmrp_graft_recv (interface, versionSupport, source, data, datalen) != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d BAD GRAFT PKT Recieved!\n", 
                    __FUNCTION__, __LINE__);
        interface->badGraftPkts++;        
        return(L7_FAILURE);
      }
      interface->graftPktsRecievedCount++;;                  
      break;
    case DVMRP_GRAFT_ACK:   /* -- */
      if (dvmrp_graft_ack_recv (interface, versionSupport, source, data, datalen) != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d BAD GRAFT ACK PKT Recieved!\n", 
                    __FUNCTION__, __LINE__);
        interface->badGraftAckPkts++;        
        return(L7_FAILURE);
      }
      interface->graftAckPktsRecievedCount++;;                        
      break;
    case DVMRP_INFO_REQUEST:
      while (data + 4 < data_end)
      {
        if (*data == DVMRP_INFO_VERSION)
        {
          L7_int32 len = strlen (DVMRP_VERSION);
          len = ((len + 3) / 4) * 4;
          memset (cp, 0, len);
          *cp++ = DVMRP_INFO_VERSION;
          *cp++ = len;
          *cp++ = 0;
          *cp++ = 0;
          strcpy ((L7_char8 *)cp, DVMRP_VERSION);
          cp += len;
        }
        else
        {
          /*   trace (TR_PACKET, dvmrpcb->trace,
               "ignoring unknown info type %d", *data);*/
        }
        data += (4 + data[1] * 4);
      }
      if (cp > sendbuf)
      {
        dvmrp_send (DVMRP_INFO_REPLY, source,
                    sendbuf, cp - sendbuf, interface);
      }
      break;
    case DVMRP_INFO_REPLY:
    default:
      break;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  This function sends the DVMRP packet to IGMP. 
*
* @param    code       - @b{(input)}  DVMRP packet type
* @param    dst         - @b{(input)}   Pointer to the Destination group being pruned
* @param    data       - @b{(input)}  Pointer to the packet buffer
* @param    len         - @b{(input)}   Length of the packet buffer
* @param    interface-  @b{(input)}Pointer to the Interface throufh which DVMRP packet 
*                                       has to be sent.
* 
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    Sends the DVMRP packet to the IGMP.
*       
* @end
*********************************************************************/
L7_int32
dvmrp_send (L7_int32 code, 
            L7_dvmrp_inet_addr_t *dst, 
            L7_uchar8 *data, L7_int32 len,
            dvmrp_interface_t *interface)
{
  dvmrp_t *dvmrpcb= L7_NULLPTR; 
  L7_ulong32 versionSupport;  
  L7_dvmrp_inet_addr_t group; 
  L7_int32 ret = L7_SUCCESS;
  struct igmp *igmphdr = L7_NULLPTR;
  L7_in_addr_t  ipaddr;
  L7_uchar8 buffer[L7_MULTICAST_MAX_IP_MTU];
  L7_dvmrp_inet_addr_t tmp_addr;
  mcastSendPkt_t dvmrpCtrlPkt;
  L7_char8 dstAddr[DVMRP_STRING_SIZE],src[DVMRP_STRING_SIZE];

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }


  /* the capability is set only for the probe messages and not set for 
   * other messages.
   */
  versionSupport = (code == DVMRP_PROBE || code == DVMRP_NEIGHBORS2)?
                   (dvmrpcb->versionSupport): ((dvmrpcb->versionSupport& 0x0000ffff));

  if (L7_FAILURE == inetAddressSet(dvmrpcb->family,&versionSupport,&group.addr))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Failed to set the address\n");
    return L7_FAILURE;
  }

  group.maskLength = DVMRP_MASK_LEN;

  if (dst == L7_NULLPTR)
  {
    dst = &dvmrpcb->all_routers;
    inetAddrNtoh(&dst->addr,&tmp_addr.addr); 

    if (inetAddrHtop(&tmp_addr.addr, dstAddr) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;    

    }
    DVMRP_DEBUG(DVMRP_DEBUG_PROBE,"\n%s:%d Destination is NULL PTR, dstAddr=%s\n",
                __FUNCTION__,__LINE__,dstAddr);
    DVMRP_DEBUG(DVMRP_DEBUG_INFO,"\n%s:%d Destination is NULL PTR, dstAddr=%s\n",
                __FUNCTION__,__LINE__,dstAddr);

  }
  else
  {
    memcpy(&tmp_addr,dst,sizeof(L7_dvmrp_inet_addr_t));
    if (inetAddrHtop(&tmp_addr.addr, dstAddr) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;    

    }
    DVMRP_DEBUG(DVMRP_DEBUG_PROBE,"\n%s:%d Destination is NON-NULL PTR, dstAddr=%s\n",
                __FUNCTION__,__LINE__,dstAddr);
    DVMRP_DEBUG(DVMRP_DEBUG_INFO,"\n%s:%d Destination is NON-NULL PTR, dstAddr=%s\n",
                __FUNCTION__,__LINE__,dstAddr);
  }

  memcpy(&dvmrpCtrlPkt.destAddr, &tmp_addr.addr, sizeof(L7_inet_addr_t));
  memcpy(&dvmrpCtrlPkt.srcAddr, &interface->primary.inetAddr.addr, 
         sizeof(L7_inet_addr_t));
  dvmrpCtrlPkt.family = dvmrpcb->family;
  dvmrpCtrlPkt.rtrIfNum = interface->index;
  dvmrpCtrlPkt.protoType = L7_MCAST_IANA_MROUTE_DVMRP;
  dvmrpCtrlPkt.ttl = DVMRP_TTL;
  dvmrpCtrlPkt.flags = L7_NULL;
  dvmrpCtrlPkt.sockFd = dvmrpcb->sockFd;
  dvmrpCtrlPkt.rtrAlert = L7_FALSE;

  igmphdr = (struct igmp *)buffer;
  memset(igmphdr, 0, sizeof (struct igmp));
  igmphdr->igmp_type = L7_IGMP_DVMRP;
  igmphdr->igmp_code = code;

  inetAddrNtoh(&group.addr, &tmp_addr.addr);   
  inetAddressGet(dvmrpcb->family, &tmp_addr.addr, &ipaddr);
  igmphdr->igmp_group = ipaddr;
  igmphdr->igmp_cksum = 0;

  if (len == L7_NULL || data == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Data pointer is NULL\n");
    DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Data pointer is NULL, dstAddr=%s\n",
                __FUNCTION__,__LINE__,dstAddr);
    return L7_FAILURE;
  }
  if (len > 0)
  {
    assert ((L7_uchar8*)(igmphdr + 1) + len - buffer <= sizeof (buffer));
    if ((L7_uchar8*)(igmphdr + 1) + len - buffer > sizeof (buffer))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Data pointer is NULL\n");
      DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Data pointer is NULL, dstAddr=%s\n",
                  __FUNCTION__,__LINE__,dstAddr);
      return L7_FAILURE;
    }
    memcpy (igmphdr + 1, data, len);
  }
  dvmrpCtrlPkt.payLoad = (L7_uchar8 *)igmphdr;
  dvmrpCtrlPkt.length = sizeof(*igmphdr)+len;
  igmphdr->igmp_cksum = inetChecksum (igmphdr, 
                                      sizeof (*igmphdr) + len);

  assert (inetChecksum (igmphdr, sizeof (*igmphdr) + len) == 0);

  if (inetAddrHtop(&dvmrpCtrlPkt.srcAddr, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;    

  }
  DVMRP_DEBUG(DVMRP_DEBUG_INFO,"\n%s:sockFd=%d, srcAddr=%s\n",
              __FUNCTION__,dvmrpCtrlPkt.sockFd,src);

  if (L7_SUCCESS != mcastMapPacketSend(&dvmrpCtrlPkt))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Couldn't send the DVMRP packet\n");
    DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Failed to send the PKT to dstAddr=%s\n",
                __FUNCTION__,__LINE__,dstAddr);
    return L7_FAILURE;
  }
  dvmrpDebugPacketRxTxTrace(L7_FALSE, dvmrpCtrlPkt.rtrIfNum, 
                            &dvmrpCtrlPkt.srcAddr, &dvmrpCtrlPkt.destAddr,
                            dvmrpCtrlPkt.payLoad,  dvmrpCtrlPkt.length);

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return(ret);
}

/*********************************************************************
* @purpose  This function is used to enable/disable Global admin mode.
*
* @param    dvmrpCbHandle   -  @b{(input)} DVMRP control block Handle.
* @param    globalAdminMode -  @b{(input)} dvmrp  Mode.
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_global_adminmode_set(MCAST_CB_HNDL_t dvmrpCbHandle,
                                   L7_uint32       globalAdminMode)
{
  dvmrp_t            *dvmrpCb = (dvmrp_t *) dvmrpCbHandle;
  L7_RC_t            rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,
              "%s :MCAST_EVENT_DVMRP_GLOBAL_ADMIN_MODE_SET rcvd\n\n",
              __FUNCTION__);

  if(dvmrpCb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "DvmrpCB memory still not allocated.\n");
    return L7_FAILURE;    
  }


  if (globalAdminMode ==  L7_ENABLE)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_INFO,"Mode value is L7_ENABLE.\n");
    dvmrpCb->dvmrpOperFlag = L7_ENABLE;
    DVMRP_DEBUG(DVMRP_DEBUG_INFO,
                "initializing the DVMRP application.\n");
    if ((rc = dvmrpAdminModeSet(dvmrpCb)) != L7_SUCCESS)
    {
      dvmrpCb->dvmrpOperFlag = L7_DISABLE;
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "Cannot initialize the DVMRP application.\n");
      DVMRP_DEBUG(DVMRP_DEBUG_INFO,
                  "Failed to initialize the DVMRP application.\n");
      return L7_FAILURE;
    }
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_INFO,
                "de-initializing the DVMRP application.\n");
    dvmrpCb->dvmrpOperFlag = L7_DISABLE;
    rc = dvmrpAdminModeReset(dvmrpCb);
  }
  return rc;
}



