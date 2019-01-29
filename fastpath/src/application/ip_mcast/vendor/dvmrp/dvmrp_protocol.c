/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_protocol.c
*
* @purpose Contains routines to process the various DVMRP protocol
*          packet types.
*
* @component
*
* @comments
*
* @create 06/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#include "buff_api.h"
#include "dvmrp_protocol.h"
#include "dvmrp_nbr.h"
#include "dvmrp_mfc.h"
#include "dvmrp_util.h"
#include "dvmrp_main.h"
#include "dvmrp_snmp.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_routing.h"
#include "dvmrp_interface.h"
#include "dvmrp_timer.h"
#include "dvmrp_igmp.h"
#include "mcast_wrap.h"
#include "dvmrp_vend_ctrl.h"

/*********************************************************************
* @purpose  This function processes the DVMRP probe packet
*
* @param    interface       -  @b{(input)}Pointer to the Interface through which the probe 
*                                        packet has been recieved.
* @param    versionSupport  -  @b{(input)} Verison supported by the 
*                                               neighbouring router
* @param    source          -  @b{(input)}Pointer to the Source address of the packet
* @param    data            -  @b{(input)} pointer to the packet buffer
* @param    datalen         -  @b{(input)}  length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    This function parses the probe packet and sees whether the 
*           packet is correct or not, if correct and if the packet is 
*           coming for the first time from this neighbour then a neighbor 
*           node is created and added  to the neighbour list.
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_probe_recv (dvmrp_interface_t *interface, L7_ulong32 versionSupport,
                          L7_dvmrp_inet_addr_t *source, L7_uchar8 *data, L7_int32 datalen)
{
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  L7_ulong32 genid = 0;
  L7_uchar8 *endp = data + datalen;
  L7_int32 i_am_on = 0;
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR;
  dvmrp_prune_t *prune,*pruneNext=L7_NULLPTR;
  L7_ulong32 capabilities = (versionSupport >> 16) & 0x000F;
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  L7_uint32 timeLeft = L7_NULL;
  L7_char8 src[IPV6_DISP_ADDR_LEN];
  desg_fwd_t *desg_fwd=L7_NULLPTR;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL \n", 
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }


  nbr = dvmrp_neighbor_lookup (interface, versionSupport, source);
  if (inetAddrHtop(&source->addr, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n", 
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_PROBE, "\n%s: PROBE PACKET recvd from NBR=%s\n", __FUNCTION__,src);

  if (nbr != L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_PROBE, "\n%s: NBR exists already for NBR=%s\n", __FUNCTION__,src);
    nbr->utime = osapiUpTimeRaw();
    if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpcb->timerHandle, nbr->timeout, 
                                          &timeLeft))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to get the time left\n", 
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d Failed to get the time left\n", 
                  __FUNCTION__, __LINE__);
    }
    if (timeLeft >= L7_NULL)
    {
      appTimerDelete(dvmrpcb->timerHandle, nbr->timeout);
      if (nbr->nbrTimeoutHandle != L7_NULL)
      {
        handleListNodeDelete(dvmrpcb->handle_list,
                             &nbr->nbrTimeoutHandle);
      }
      nbr->timeout=L7_NULLPTR;
    }
    nbr->nbrTimeoutHandle =
    handleListNodeStore(dvmrpcb->handle_list, (void*)nbr);

    if (L7_NULLPTR == (nbr->timeout = appTimerAdd(dvmrpcb->timerHandle,
                                                  dvmrp_neighbor_timeout,
                                                  (void *)nbr->nbrTimeoutHandle,
                                                  DVMRP_NEIGHBOR_EXPIRE_TIME,
                                                  "DV-NLT3")))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d timer instantiation failed \n", 
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d timer instantiation failed \n", 
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_PROBE, "%s:%d timer instantiation failed \n", 
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
  }

  if (endp - data < 4)
  {
    /*  increment for bad Pkts */
    nbr = &dvmrpcb->index2neighbor[interface->index];
    dvmrp_badpkts_inc(interface,nbr);
    DVMRP_DEBUG(DVMRP_DEBUG_PROBE,"\n%s: BAD PACKET recvd from NBR=%s\n",
                __FUNCTION__,src);
    return L7_FAILURE;
  }
  MCAST_GET_LONG (genid, data);
  genid = osapiNtohl(genid);    
  while (endp - data > 0)
  {
    L7_inet_addr_t inetAddr;
    L7_ulong32 addr;

    if (endp - data < 4)
    {
      /*  increment for bad Pkts */
      nbr = &dvmrpcb->index2neighbor[interface->index];
      dvmrp_badpkts_inc(interface,nbr);
      DVMRP_DEBUG(DVMRP_DEBUG_PROBE,"\n%s: BAD PACKET recvd from NBR=%s\n",
                  __FUNCTION__,src);
      return L7_FAILURE;
    }
    MCAST_GET_NETLONG (addr, data);
    /*addr = osapiNtohl(addr);Shashi */

    if (L7_SUCCESS != inetAddressSet(dvmrpcb->family, &addr, &inetAddr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address.\n", 
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }

    if (L7_TRUE == inetIsDirectlyConnected(&inetAddr, interface->index))
    {
      i_am_on++;
    }
  }

  if (nbr != L7_NULLPTR)
  {
    if (i_am_on)
    {
      nbr->state = L7_DVMRP_NEIGHBOR_STATE_ACTIVE;
    }
    else if (!BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))
    {
      /* Send a trap if transitioning from active state */
      if (nbr->state == L7_DVMRP_NEIGHBOR_STATE_ACTIVE)
      {
        if (L7_SUCCESS != dvmrp_send_neighbor_loss_trap(interface, nbr, 
                                                        L7_DVMRP_NEIGHBOR_STATE_IGNORING))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to send Neighbor loss trap recvd\n",
                      __FUNCTION__, __LINE__);
        }
      }
      nbr->state = L7_DVMRP_NEIGHBOR_STATE_IGNORING;
    }
  }
  if (nbr == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_PROBE,
                "\n%s: NBR doesn't exist so it is being created for NBR=%s\n",
                __FUNCTION__,src);
    nbr = dvmrp_neighbor_register (interface, versionSupport, genid, source);
    if (nbr == L7_NULLPTR)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:There is no NBR entry in the table:%d\n",
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_PROBE,
                  "%s:Failed to create NBR entry:%d\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    if (i_am_on)
    {
      nbr->state = L7_DVMRP_NEIGHBOR_STATE_ACTIVE;
    }
    else
    {
      nbr->state = L7_DVMRP_NEIGHBOR_STATE_ONEWAY;
    }

    if (dvmrp_probe_send (interface, source) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d COuld not Send the Probe Message \n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }

    /* If Prune bit is not set in capability flag, send a trap */
    if ((capabilities & (0x1 << L7_DVMRP_CAPABILITIES_PRUNE)) == 0)
    {
      if (L7_SUCCESS != dvmrp_send_neighbor_not_pruning_trap(interface, nbr,
                                                             capabilities))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to send Neighbor loss trap recvd\n",
                    __FUNCTION__, __LINE__);
      }
    }
  }
  else if (BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))
  {
    /* Safe to make it active instead of ONEWAY */
    nbr->state = L7_DVMRP_NEIGHBOR_STATE_ACTIVE; 
    nbr->genid = genid;
    dvmrp_neighbor_recover(interface, nbr);

    if (dvmrpcb->flash_update_waiting)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:dvmrp_route_advertise called :%d\n", 
                  __FUNCTION__, __LINE__);
      dvmrpcb->flash_update_waiting = 0;
    }
    dvmrp_route_advertise (L7_TRUE, dvmrpcb);

    /* If Prune bit is not set in capability flag, send a trap */
    if ((capabilities & (0x1 << L7_DVMRP_CAPABILITIES_PRUNE)) == 0)
    {
      if (L7_SUCCESS != dvmrp_send_neighbor_not_pruning_trap(interface, nbr,
                                                             capabilities))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to send Neighbor loss trap recvd\n",
                    __FUNCTION__, __LINE__);
      }
    }
  }
  else
  {
    if (nbr->genid != genid)
    {
      memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
      if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s: no cache entry in the table:%d\n",
                    __FUNCTION__,__LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s: no cache entry in the table:%d\n",
                    __FUNCTION__,__LINE__);
      }
      while (L7_NULLPTR != entry)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s: Prune list found in the DUT:%d\n",
                    __FUNCTION__,__LINE__);
        for (prune = (dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes));
            prune != L7_NULLPTR;
            prune = pruneNext)
        {

          pruneNext= (dvmrp_prune_t*)SLLNextGet(&(entry->ll_prunes),(void*)prune);
          if (prune->neighbor == nbr)
          {
            dvmrp_route_t *route = (dvmrp_route_t *)entry->data;
            L7_char8 printNBR[IPV6_DISP_ADDR_LEN];

            if (inetAddrHtop(&nbr->nbrAddr.addr, printNBR) != L7_SUCCESS)
            {
              DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                          __FUNCTION__, __LINE__);
              return L7_FAILURE;


            }
            DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Found prune from nbr(%s):\n",
                        __FUNCTION__,__LINE__,printNBR);
            if (route == L7_NULLPTR)
            {
              DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Route PTR is NULL: so breaking\n",
                          __FUNCTION__,__LINE__);
              break;
            }
            if (BIT_TEST(entry->flags, DVMRP_CACHE_NEGATIVE) && 
                (route->neighbor != L7_NULLPTR))
            {
              if (dvmrp_graft_send(entry, route->neighbor, 0) != L7_SUCCESS)
              {
                DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Graft Not Sent \n",
                            __FUNCTION__, __LINE__);
                return L7_FAILURE;
              }
            }
            if (((desg_fwd = dvmrp_get_desg_fwd(route,prune->neighbor->interface)) 
                 != L7_NULLPTR)&&(desg_fwd->status == DVMRP_ENTRY_NOT_USED))
            {
              MCAST_BITX_SET(entry->children, 
                             prune->neighbor->interface->index);
            }
            dvmrp_cache_mfc_update(entry);
            if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
            {
              DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                          __FUNCTION__, __LINE__);
              return L7_FAILURE;
            }

            if (L7_SUCCESS != SLLDelete(&(entry->ll_prunes), 
                                        (L7_sll_member_t *)prune))
            {
              DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                          "%s:Deletion failed from SLL:%d\n",
                          __FUNCTION__, __LINE__);
              osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
              return L7_FAILURE;
            }
            osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
          }
        }
        memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                      "%s:There is no cache entry in the table:%d\n",
                      __FUNCTION__, __LINE__);
          break;
        }
      }

      nbr->genid = genid;
      nbr->ctime = osapiUpTimeRaw();
      if (dvmrpcb->flash_update_waiting)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:dvmrp_route_advertise called  :%d\n",
                    __FUNCTION__, __LINE__);
        dvmrpcb->flash_update_waiting = 0;
      }
      dvmrp_route_advertise (L7_TRUE, dvmrpcb); /*  -- genid changed*/      

      /* If Prune bit is not set in capability flag, send a trap */
      if ((capabilities & (0x1 << L7_DVMRP_CAPABILITIES_PRUNE)) == 0)
      {
        if (L7_SUCCESS != dvmrp_send_neighbor_not_pruning_trap(interface, nbr,
                                                               capabilities))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to send Neighbor loss trap recvd\n",
                      __FUNCTION__, __LINE__);
        }
      }
    }
  }
  if (nbr != L7_NULLPTR)
  {
    nbr->utime = osapiUpTimeRaw();
    timeLeft = L7_NULL;
    if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpcb->timerHandle, nbr->timeout, 
                                          &timeLeft))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to get the time left\n", 
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d Failed to get the time left\n", 
                  __FUNCTION__, __LINE__);
    }
    if (timeLeft >= L7_NULL)
    {
      appTimerDelete(dvmrpcb->timerHandle, nbr->timeout);
      if (nbr->nbrTimeoutHandle != L7_NULL)
      {
        handleListNodeDelete(dvmrpcb->handle_list,
                             &nbr->nbrTimeoutHandle);
      }
      nbr->timeout = L7_NULLPTR;
    }
    nbr->nbrTimeoutHandle =
    handleListNodeStore(dvmrpcb->handle_list, (void*)nbr);

    if (L7_NULLPTR == (nbr->timeout = appTimerAdd(dvmrpcb->timerHandle,
                                                  dvmrp_neighbor_timeout,
                                                  (void *)nbr->nbrTimeoutHandle,
                                                  DVMRP_NEIGHBOR_EXPIRE_TIME,
                                                  "DV-NLT4")))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d timer instantiation failed \n", 
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d timer instantiation failed \n", 
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function processes the DVMRP report packet
*
* @param    interface       -  @b{(input)}Pointer to the Interface through which the report 
*                                       packet has been recieved.
* @param    versionSupport  -  @b{(input)} Verison supported by the 
*                                       neighbouring router
* @param    source          -  @b{(input)} Pointer to the Source address of the packet
* @param    data            -  @b{(input)}  pointer to the packet buffer
* @param    datalen         -  @b{(input)} - length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    This function parses the report packet and sees whether 
*           the packet is correct or not, if correct ,all the routes 
*           present iin it are added to the route table.
* @end
*********************************************************************/
L7_RC_t dvmrp_report_recv(dvmrp_interface_t *interface, L7_ulong32 versionSupport,
                          L7_dvmrp_inet_addr_t *source, L7_uchar8 *data, L7_int32 datalen)
{
  L7_uchar8 *endp = data + datalen;
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  L7_int32 major_version = versionSupport & 0xff;
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  dvmrp_report_t *aReport;
  L7_uint32 counter = 0,no_of_reports = 0;
  L7_char8 src[IPV6_DISP_ADDR_LEN];

  if (inetAddrHtop(&source->addr, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d nInetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;


  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: Report Packet received frm NBR=%s\n", __FUNCTION__,src);
  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d n\nINVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_FAILURE;
  }


  nbr = dvmrp_neighbor_lookup (interface, versionSupport, source);
  if (nbr == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s: Report Pkt frm unknown NBR=%s\n", __FUNCTION__,src);
    if (major_version == DVMRP_MAJOR_VERSION)
    {
      /* return if report received from newer versionSupport neighbor and
         return if neighbor is not present*/
      dvmrp_badpkts_inc(interface,L7_NULLPTR);
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:Retuning[Pkt frm newer ver NBR=%s]\n", __FUNCTION__,src);
      return L7_FAILURE;
    }
    nbr = dvmrp_neighbor_register (interface, versionSupport, interface->genid,
                                   source);
    if (nbr != L7_NULLPTR)
    {
      nbr->state = L7_DVMRP_NEIGHBOR_STATE_ACTIVE;
    }
    /* this is new to me */
    if (dvmrp_probe_send (interface, source) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d COuld not Send the Probe Message \n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;

    }
  }
  else
  {
    if ((major_version == DVMRP_MAJOR_VERSION) && 
        (nbr->state == L7_DVMRP_NEIGHBOR_STATE_DOWN))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:Retuning[NBR=%s state is DOWN]\n", __FUNCTION__,src);
      dvmrp_badpkts_inc(interface,nbr);
      return L7_FAILURE;
    }
    else
    {
      if (nbr->state == L7_DVMRP_NEIGHBOR_STATE_DOWN)
      {
        nbr->state = L7_DVMRP_NEIGHBOR_STATE_ACTIVE;
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:[Recovering NBR=%s]\n", __FUNCTION__,src);
        dvmrp_neighbor_recover(interface,nbr);
      }
    }
  }
  while (endp - data > 0)
  {
    L7_uchar8 mask[4];
    L7_uchar8 prefixlen, bytes;
    L7_int32 metric;
    L7_inet_addr_t addrmask;

    if (endp - data < 3)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:Breaking[Truncated RouteReport NBR=%s]\n",__FUNCTION__,src);
      break;
    }
    mask[0] = 0xff;
    MCAST_GET_BYTE (mask[1], data);
    MCAST_GET_BYTE (mask[2], data);
    MCAST_GET_BYTE (mask[3], data);

    if (!inet_mask_valid (mask))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "Non Continuoous Mask\n");
    }
    memset (&addrmask, 0, sizeof (addrmask));
    if (L7_SUCCESS != inetAddressSet(L7_AF_INET, &mask, &addrmask))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    if (L7_FAILURE== inetMaskToMaskLen(&addrmask, &prefixlen))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "Couldn't compute mask length\n");
      return L7_FAILURE;
    }
    bytes = (prefixlen + 7)/8;
    do 
    {
      L7_inet_addr_t inetAddr;
      L7_uchar8 origin[4];
      L7_int32 i;
      memset (origin, 0, sizeof (origin));
      for (i = 0; i < bytes; i++)
      {
        MCAST_GET_BYTE (origin[i], data);
      }
      MCAST_GET_BYTE (metric, data);
      if (* (L7_ulong32 *) origin == 0L && prefixlen == 8)
      {
        /* special case for default route */
        prefixlen = 0;
      }
      *(L7_ulong32 *)origin = osapiNtohl (*(L7_ulong32 *)origin);
      if (L7_SUCCESS != inetAddressSet(L7_AF_INET, &origin, &inetAddr))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }

      aReport = dvmrpcb->report_entries;

      aReport[counter].srcNetwork.maskLength = prefixlen;
      inetCopy(&aReport[counter].srcNetwork.addr,&inetAddr);
      aReport[counter].metric = metric & 0x7f;
      aReport[counter].neighbor = nbr;
      counter++;
    }while ((!(metric & 0x80))&&((endp-data)>0));
  }

  no_of_reports = counter;
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE,"%s:[Total (%d)Reports recvd NBR=%s]\n",__FUNCTION__,no_of_reports,src);
  if (no_of_reports > L7_NULL)
  {
    if (dvmrp_report_process (dvmrpcb, no_of_reports) == L7_FAILURE)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed in Processing the report \n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  This function sends the report packets out on an interface. 
*
* @param     no_of_rts_advertise     -  @b{(input)} No of routes to advertise.
* @param     interface                     -  @b{(input)}Pointer the interface
* 
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/

L7_int32 dvmrp_routes_send (L7_uint32 no_of_rts_advertise,dvmrp_interface_t *interface)
{
  L7_uchar8 sendbuf[DVMRP_MAX_DVMRP_DATA_LEN];
  L7_uchar8 *cp = sendbuf;
  L7_int32 count = 0;
  L7_char8 bitlen = -1;
  L7_uint32 s_addr = L7_NULL;
  dvmrp_t *dvmrpcb = L7_NULLPTR;  


  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s: ENTERED \n", __FUNCTION__);

  if (interface == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : Interface pointer is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_FAILURE;
  }

  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_FAILURE;
  }



  for (count =0; count < no_of_rts_advertise;count++)
  {
    L7_uchar8 *addr = L7_NULLPTR ;
    L7_int32 bytes;
    L7_int32 i;
    L7_uint32 net_addr;    
    dvmrp_report_t   *ann_rt=L7_NULLPTR;

    ann_rt = &dvmrpcb->report_entries[count];

    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &ann_rt->srcNetwork.addr, 
                                     &net_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert InetAddress to IpAddress\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    addr = (L7_uchar8 *)&net_addr;
    bytes = (ann_rt->srcNetwork.maskLength + 7) / 8;


    if (cp - sendbuf + ((bitlen == ann_rt->srcNetwork.maskLength)?
                        (bytes + 1): (3 + bytes + 1))
        >= DVMRP_MAX_DVMRP_DATA_LEN)
    {
      *(cp - 1) |= 0x80;
      dvmrp_send (DVMRP_REPORT, L7_NULLPTR,
                  sendbuf, cp - sendbuf, interface);
      cp = sendbuf;
      bitlen = -1;
    }
    if (bitlen != ann_rt->srcNetwork.maskLength)
    {
      L7_uchar8 mask[4];
      L7_inet_addr_t net_mask;
      bitlen = ann_rt->srcNetwork.maskLength;
      if (cp != sendbuf)
      {
        *(cp - 1) |= 0x80;
      }
      assert (ann_rt->srcNetwork.maskLength <= 32);
      memset(&net_mask, 0, sizeof(L7_inet_addr_t));
      if (L7_FAILURE == inetMaskLenToMask(dvmrpcb->family, bitlen, &net_mask))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert masklen to mask\n",
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }

      if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &net_mask, &s_addr))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert InetAddress to IpAddress\n",
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
      memcpy(mask,(L7_uchar8*)&(s_addr), 4*sizeof(L7_uchar8));
      assert (ann_rt->srcNetwork.maskLength == 0 || mask[0] == 0xff);
      MCAST_PUT_BYTE (mask[1], cp);
      MCAST_PUT_BYTE (mask[2], cp);
      MCAST_PUT_BYTE (mask[3], cp);
    }
    for (i = 0; i < bytes; i++)
    {
      MCAST_PUT_BYTE (addr[i], cp);
    }

    if (ann_rt->metric == 0)
    {
      MCAST_PUT_BYTE (1, cp); /* direct */
    }
    else
    {
      MCAST_PUT_BYTE (ann_rt->metric, cp);
    }
  }
  if (cp - sendbuf > 0)
  {
    *(cp - 1) |= 0x80;

    interface->reportPktsSentCount++;
    dvmrp_send (DVMRP_REPORT, L7_NULLPTR, sendbuf, cp - sendbuf, interface);
  }
  interface->sentRts = count;
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s: LEAVING \n", __FUNCTION__);
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  This function processes the DVMRP prune packet
*
* @param    interface        -  @b{(input)}Pointer to the Interface through which the prune 
*                                       packet has been recieved.
* @param    versionSupport   -  @b{(input)}  Verison supported by the 
*                                       neighbouring router
* @param    source           -  @b{(input)}Pointer to the source address of the packet
* @param    data             -  @b{(input)} Pointer to the packet buffer
* @param    datalen          -  @b{(input)}  length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
* 
* @notes    Handles the prune packet and updates the cache table accordingly.
* @end
*********************************************************************/
L7_RC_t dvmrp_prune_recv (dvmrp_interface_t *interface, L7_ulong32 versionSupport,
                          L7_dvmrp_inet_addr_t *source, L7_uchar8 *data, L7_int32 datalen)
{
  dvmrp_neighbor_t *nbr;
  L7_ulong32 prune_src, prune_grp,msk=0;
  L7_inet_addr_t prune_source, prune_group,tmp,prune_mask;
  L7_int32 lifetime, min_lifetime = DVMRP_AVERAGE_PRUNE_LIFETIME;
  L7_uchar8 *endp = data + datalen;
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR;
  dvmrp_route_t *route;
  L7_uint32 now;
  neighbor_bitset_t bitset;
  dvmrp_prune_t *prune = L7_NULLPTR;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  L7_inet_addr_t  network;
  L7_char8 src[IPV6_DISP_ADDR_LEN], grp[IPV6_DISP_ADDR_LEN];
  L7_uchar8 maskLen=0;    

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_FAILURE;
  }

  if ((datalen < DVMRP_PRUNE_MIN_LENGTH) ||(datalen >DVMRP_PRUNE_MAX_LENGTH))         /* Draft sectio 3.5.3.2 */
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d Failed, too short datalen\n",
                __FUNCTION__, __LINE__);
    /* too short datalen increment for bad Pkts */
    nbr = &dvmrpcb->index2neighbor[interface->index];
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }
  nbr = dvmrp_neighbor_lookup (interface, versionSupport, source);
  if (nbr == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d Failed, Neighbor is not found\n", 
                __FUNCTION__, __LINE__);
    /*  increment for bad Rts */
    dvmrp_badpkts_inc(interface,L7_NULLPTR);
    return L7_FAILURE;
  }
  if (BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d Failed, Neighbor delete flag is set\n",
                __FUNCTION__, __LINE__);
    dvmrp_badpkts_inc(interface,L7_NULLPTR);
    return L7_FAILURE;
  }

  /* get the data */  /* Draft section 3.5.3.3 */
  MCAST_GET_LONG (prune_src, data);
  MCAST_GET_LONG (prune_grp, data);
  MCAST_GET_LONG (lifetime, data);
  if(datalen  == DVMRP_PRUNE_MAX_LENGTH)
  {
    MCAST_GET_LONG (msk, data);  
  }

  if (data > endp)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Prune Msg Length %d is not valid",
                __FUNCTION__, __LINE__, datalen);
    return L7_FAILURE;
  }

  /* Error if Prune group is zero or lifetime is zero */
  if (prune_grp == 0 || lifetime == 0)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d Failed, Prune group is zero or lifetime is zero\n",
                __FUNCTION__,__LINE__);
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != inetAddressSet(dvmrpcb ->family, &prune_src, &prune_source))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  if (L7_SUCCESS != inetAddressSet(dvmrpcb ->family, &prune_grp, &prune_group))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  if (L7_SUCCESS != inetAddressSet(dvmrpcb ->family, &msk, &prune_mask))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  
  if (inetAddrHtop(&prune_source, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;


  }
  if (inetAddrHtop(&prune_group, grp) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;


  }
  DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d: Received Prune from src = %s,grp = %s \n",
              __FUNCTION__,__LINE__,(src),(grp));

  inetCopy(&network,&prune_source);
  inetCopy(&tmp,&network);
  if (inetAddrHtop(&network, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;


  }
  inetAddrHton(&tmp, &network);
  /* Draft section 3.5.3.4 */
  dvmrpKeySet(&key, network);
  /* find the longest match */
  pData = radixMatchNode(&dvmrpcb->dvmrpRouteTreeData, &key, skipentries);
  if (pData == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:%d Failed to get the best match for addr=%s\n",
                __FUNCTION__, __LINE__, src);
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d No Best match for the addr=%s\n",
                __FUNCTION__, __LINE__, src);
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }

  route = &pData->dvmrpRouteInfo;
  /* Draft section 3.5.3.5 */
  if (!MCAST_BITX_TEST (route->dependents.bits, nbr->index))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:%d Nbr in route Entry not a dependent Nbr for "
                "addr=%s\n", __FUNCTION__, __LINE__,
                src);
    DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                "%s:%d Nbr in route Entry not a dependent Nbr for "
                "addr=%s\n", __FUNCTION__, __LINE__, 
                src);
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }

  /* If mask is present or     */
  if((msk != 0) && (msk != DVMRP_FULL_HOST_MASK))
  {
     if(inetMaskToMaskLen(&prune_mask,&maskLen) != L7_SUCCESS)
     {
       DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Couldn't convert mask to masklen \n");
       return L7_FAILURE;
     }
     if(maskLen != pData->dvmrpRouteInfo.networkAddr.maskLength)
     {
       DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Network mask does not match.So Prune message is ignored. \n");
      return L7_FAILURE;
     }
  }
  

  /* Iterate through all the cache entries and apply prune to all the entries
     that match the source and mask of the dvmrp route */
  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                "%s:%d No Cache Entry in the Cache Table for addr=%s\n",
                __FUNCTION__, __LINE__, src);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:There is no cache entry in the table:%d\n", 
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  while (L7_NULLPTR != entry)
  {
    L7_inet_addr_t  cache_src;
    L7_dvmrp_inet_addr_t  route_src;

    /* If the Source address matches then prune needs to be applied */
    inetCopy(&cache_src,&entry->source);
    memcpy(&route_src,&route->networkAddr,sizeof(L7_dvmrp_inet_addr_t));

    if (inetAddressAnd(&cache_src,&pData->netmask.addr, &tmp) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Inet And operation Failed!!!!!!!!\n",
                  __FUNCTION__, __LINE__); 
      return L7_FAILURE;
    }
    inetAddrHton(&tmp, &cache_src);
    inetCopy(&tmp,&route_src.addr);
    inetAddrHton(&tmp, &route_src.addr); 

    /* Draft section 3.5.3.4 */

    if (inetAddrHtop(&cache_src, src) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d nInetAddress to ASCII conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;


    }
    if (inetAddrHtop(&route_src.addr, grp) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;


    }
    DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d Inet Compare between cache_src=%s,route_src=%s\n",
                __FUNCTION__,__LINE__,(src),(grp));
    if ((L7_INET_ADDR_COMPARE(&cache_src, &route_src.addr) == L7_NULL)&&
        (L7_INET_ADDR_COMPARE(&entry->group, &prune_group) == L7_NULL))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d Compare is success=%s,route_src=%s\n",
                  __FUNCTION__,__LINE__,(src),(grp));

      if(msk == DVMRP_FULL_HOST_MASK)
      {
        if( L7_INET_ADDR_COMPARE(&entry->source,&prune_source) != 0)
        {
          memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
          if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_INFO, "%s:No more cache entries in the table:%d\n",
                        __FUNCTION__, __LINE__);
            break;
          }
          continue;  
        }
      }
      
      now = osapiUpTimeRaw();
      /* Draft section 3.5.3.6 */
      bitset = route->dependents;
      for (prune = (dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes));
          prune != L7_NULLPTR;
          prune = (dvmrp_prune_t*)SLLNextGet(&(entry->ll_prunes),(void*)prune))
      {
        if (prune->neighbor == nbr)      /* Draft section 3.5.3.7 if part */
        {
          prune->received = now;
          prune->lifetime = lifetime;
          prune->expire = now + lifetime;
          break;
        }
      }
      if (prune == L7_NULLPTR) /* Draft section 3.5.3.7 else part */
      {
        if((prune = DVMRP_ALLOC (L7_AF_INET, sizeof (dvmrp_prune_t))) == L7_NULLPTR)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n %s, %d : Memory allocation failed",
                      __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }
        memset(prune, L7_NULL, sizeof(dvmrp_prune_t));

        DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d New Prune is added for cache_src=%s,route_src=%s\n",
                    __FUNCTION__,__LINE__,(src),(grp));
        prune->neighbor = nbr;
        prune->received = now;
        prune->lifetime = lifetime;
        prune->expire = now + lifetime;

        if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                      __FUNCTION__, __LINE__);
          return L7_FAILURE;
        }

        if (L7_SUCCESS != SLLAdd(&(entry->ll_prunes), (L7_sll_member_t *)prune))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d Linked list addition failed cache_src=%s,route_src=%s\n",
                      __FUNCTION__,__LINE__,(src),(grp));
          DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d Linked list addition failed cache_src=%s,route_src=%s\n",
                      __FUNCTION__,__LINE__,(src),(grp));
          osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
          return L7_FAILURE;
        }
        osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      }
      MCAST_BITX_SET(entry->pruneRcvd, nbr->interface->index);

      min_lifetime = lifetime; /* current lifetime */
      /* get the minimum of all */
      for (prune = (dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes));
          prune != L7_NULLPTR;
          prune = (dvmrp_prune_t*)SLLNextGet(&(entry->ll_prunes),(void*)prune))
      {
        if (min_lifetime > prune->expire - now)
        {
          min_lifetime = prune->expire - now;
        }
        MCAST_BITX_RESET (bitset.bits, prune->neighbor->index);
      }
      /* Draft section 3.5.3.8 */
      if (dvmrp_is_only_neighbor_dependent(&bitset,nbr) == L7_SUCCESS)
      {
        /* 
         * Determine group memebers if any on the interface  
         * Draft section 3.5.3.9 
         */
        if (dvmrpMemberShipTest(dvmrpcb,&entry->group, &entry->source, 
                                nbr->interface->index) 
            != L7_SUCCESS)
        {
          /* section 3.5.3.10 */
          if (BIT_TEST (entry->flags, DVMRP_CACHE_DELETE))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d This Cache entry flag is set to delete cache_src=%s,route_src=%s\n",
                        __FUNCTION__,__LINE__,(src),(grp));
            memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
            if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
            {
              DVMRP_DEBUG(DVMRP_DEBUG_INFO, "%s:No more cache entries in the table:%d\n",
                          __FUNCTION__, __LINE__);
              break;
            }
            continue;
          }
          DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d Checking if the RtrIfNum(%d) is set on the router bitmask cache_src=%s,route_src=%s\n",
                      __FUNCTION__,__LINE__,nbr->interface->index, (src),(grp));
          if (MCAST_BITX_TEST(entry->routers, nbr->interface->index))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d Cache entry removes the ifindex=%d from o/f cache_src=%s,route_src=%s\n",
                        __FUNCTION__,__LINE__,nbr->interface->index,(src),(grp));
            MCAST_BITX_RESET (entry->routers, nbr->interface->index);
            dvmrp_cache_mfc_update (entry);
            /* Draft section  3.5.3.11 */
            if (BIT_TEST(entry->flags,DVMRP_CACHE_NEGATIVE) && route->neighbor)
            {
              MCAST_BITX_SET(entry->pruneSent, 
                             route->neighbor->interface->index);
              if (dvmrp_prune_send (route->neighbor->interface, &entry->source, 
                                    &entry->group, min_lifetime /* XXX */, 
                                    route->neighbor) != L7_SUCCESS)
              {
                DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Prune Not Sent \n",
                            __FUNCTION__, __LINE__);
                return L7_FAILURE;

              }
            }
          }
          else
          {
            DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d Nbr index %d is not set on the cache entry route bitmask cache_src=%s,route_src=%s\n",
                        __FUNCTION__,__LINE__,nbr->interface->index,(src),(grp));
          }
        }
      }
    }
    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                  "%s:No more cache entries in the table:%d\n",
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:No more cache entries in the table:%d\n",
                  __FUNCTION__, __LINE__);
      break;
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  This function processes the DVMRP graft packet
*
* @param    interface       - @b{(input)}Pointer to the Interface throufh which the garft 
*                                                       packet  has been recieved.
* @param    versionSupport  -  @b{(input)} Verison supported by the 
*                                         neighbouring router
* @param    source - -  @b{(input)}Pointer to the Source address of the packet
* @param    data --  @b{(input)} pointer to the packet buffer
* @param    datalen --  @b{(input)} length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    Handles the graft packet and updated the cache table accordingly.
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_graft_recv (dvmrp_interface_t *interface, 
                          L7_ulong32 versionSupport, L7_dvmrp_inet_addr_t *source, 
                          L7_uchar8 *data, L7_int32 datalen)
{
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  L7_ulong32 graft_src, graft_grp, nbrAddr,msk=0;
  L7_inet_addr_t graft_source, graft_group,tmp,graft_mask;
  L7_uchar8 *endp = data + datalen;
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR;
  dvmrp_prune_t *prune = L7_NULLPTR;
  L7_int32 need_graft = 0;
  dvmrp_route_t *route;
  dvmrp_t *dvmrpcb = L7_NULLPTR;  
  L7_inet_addr_t  network;  
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  L7_char8 src[IPV6_DISP_ADDR_LEN], grp[IPV6_DISP_ADDR_LEN];  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);


  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_FAILURE;
  }

  if ((datalen < DVMRP_GRAFT_MIN_LENGTH) ||(datalen >DVMRP_GRAFT_MAX_LENGTH))         /* Draft sectio 3.5.3.2 */
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d Failed, too short datalen\n",
                __FUNCTION__, __LINE__);
    /* too short datalen increment for bad Pkts */
    nbr = &dvmrpcb->index2neighbor[interface->index];
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }


  nbr = dvmrp_neighbor_lookup (interface, versionSupport, source);
  if (nbr == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d: Failed to find the Neighbor\n",
                __FUNCTION__, __LINE__);
    /*  increment for bad Pkts */
    dvmrp_badpkts_inc(interface,L7_NULLPTR);
    return L7_FAILURE;
  }
  if (BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d: Neighbor Flag set to deletion\n",
                __FUNCTION__, __LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d: Neighbor Flag set to deletion\n",
                __FUNCTION__, __LINE__);
    /*  increment for bad Pkts */
    dvmrp_badpkts_inc(interface,L7_NULLPTR);
    return L7_FAILURE;
  }

  /* 
   * LVL7 - our implementation does not advertise netmask capability,
   * accept graft packets that do not include a net mask 
   */
  if (endp - data < 8)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d: Incomplete data received\n", 
                __FUNCTION__, __LINE__);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d: Incomplete data received\n",
                __FUNCTION__, __LINE__);
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }
  MCAST_GET_LONG (graft_src, data);
  MCAST_GET_LONG (graft_grp, data);

  if(datalen == DVMRP_GRAFT_MAX_LENGTH)
  {
    MCAST_GET_LONG (msk, data);    
  }

  if (data > endp)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Graft Msg Length %d is not valid", 
                __FUNCTION__, __LINE__,datalen);
    return L7_FAILURE;
  }


  DVMRP_DEBUG(DVMRP_DEBUG_RX, 
              "%s:%d: Received graft from src = %s,grp = %s \n",
              __FUNCTION__, __LINE__, osapiInet_ntoa(graft_src), 
              osapiInet_ntoa(graft_grp));
  if (L7_SUCCESS != inetAddressSet(dvmrpcb->family, &graft_src, &graft_source))
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Failed to set the address\n");
    return L7_FAILURE;
  }

  if (L7_SUCCESS != inetAddressSet(dvmrpcb->family, &graft_grp, &graft_group))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != inetAddressSet(dvmrpcb ->family, &msk, &graft_mask))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (inetAddrHtop(&graft_source, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;


  }
  if (inetAddrHtop(&graft_group, grp) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;


  }
  DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d: Received Graft from src = %s,grp = %s \n",
              __FUNCTION__,__LINE__,(src),(grp));
  

  inetCopy(&network,&graft_source);
  inetCopy(&tmp,&network);
  if (inetAddrHtop(&network, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;


  }
  inetAddrHton(&tmp, &network);
  /* Draft section 3.5.3.4 */
  dvmrpKeySet(&key, network);
  /* find the longest match */
  pData = radixMatchNode(&dvmrpcb->dvmrpRouteTreeData, &key, skipentries);
  if (pData == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:%d Failed to get the best match for addr=%s\n",
                __FUNCTION__, __LINE__, src);
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d No Best match for the addr=%s\n",
                __FUNCTION__, __LINE__, src);
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }

  route = &pData->dvmrpRouteInfo;
  /* Draft section 3.5.3.5 */
  if (!MCAST_BITX_TEST (route->dependents.bits, nbr->index))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:%d Nbr in route Entry not a dependent Nbr for "
                "addr=%s\n", __FUNCTION__, __LINE__,
                src);
    DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                "%s:%d Nbr in route Entry not a dependent Nbr for "
                "addr=%s\n", __FUNCTION__, __LINE__, 
                src);
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }

  /*Send graft ack */
  if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &nbr->nbrAddr.addr, &nbrAddr))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert InetAddress to IpAddress\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_RX, 
              "%s:%d: Sending the GRAFT ACK for src = %s,grp = %s to "
              "Nbr=%s \n", __FUNCTION__, __LINE__, 
              osapiInet_ntoa(graft_src), osapiInet_ntoa(graft_grp), 
              osapiInet_ntoa(nbrAddr));
  if (dvmrp_graft_ack_send(interface,&graft_source,&graft_group, nbr) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Graft Ack Not Sent \n",
                __FUNCTION__, __LINE__);

  }

  
  /* Iterate through all the cache entries and apply graft to all the entries
     that match the source and mask of the dvmrp route */
  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                "%s:%d No Cache Entry in the Cache Table for addr=%s\n",
                __FUNCTION__, __LINE__, src);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:There is no cache entry in the table:%d\n", 
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  while (L7_NULLPTR != entry)
  {
    L7_inet_addr_t  cache_src;
    L7_dvmrp_inet_addr_t  route_src;

    /* If the Source address matches then graft needs to be applied */
    inetCopy(&cache_src,&entry->source);
    memcpy(&route_src,&route->networkAddr,sizeof(L7_dvmrp_inet_addr_t));

    if (inetAddressAnd(&cache_src,&pData->netmask.addr, &tmp) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Inet And operation Failed!!!!!!!!\n",
                  __FUNCTION__, __LINE__); 
      return L7_FAILURE;
    }
    inetAddrHton(&tmp, &cache_src);
    inetCopy(&tmp,&route_src.addr);
    inetAddrHton(&tmp, &route_src.addr); 

    /* Draft section 3.5.3.4 */

    if (inetAddrHtop(&cache_src, src) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d nInetAddress to ASCII conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;


    }
    if (inetAddrHtop(&route_src.addr, grp) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;


    }
    DVMRP_DEBUG(DVMRP_DEBUG_RX,"%s:%d Inet Compare between cache_src=%s,route_src=%s\n",
                __FUNCTION__,__LINE__,(src),(grp));
    if ((L7_INET_ADDR_COMPARE(&cache_src, &route_src.addr) == L7_NULL)&&
        (L7_INET_ADDR_COMPARE(&entry->group, &graft_group) == L7_NULL))
    {

      if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
  if (L7_NULLPTR == (prune=(dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes))))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:There is no cache entry in the table:%d\n", 
                __FUNCTION__, __LINE__);            
    DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                "%s:%d: No Prunes found  from src = %s,grp = %s \n", 
                __FUNCTION__, __LINE__, osapiInet_ntoa(graft_src), 
                osapiInet_ntoa(graft_grp));
  }
  while (L7_NULLPTR != prune)
  {
    dvmrp_prune_t *pTmpPrune = L7_NULLPTR;

    if (L7_NULLPTR == (pTmpPrune=(dvmrp_prune_t*)SLLNextGet(&(entry->ll_prunes), 
                                                            (void*)prune)))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                  "%s:There is no cache entry in the table:%d\n", 
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                  "%s:%d: No more Prunes found  from src = %s,"
                  "grp = %s \n", __FUNCTION__, __LINE__, 
                  osapiInet_ntoa(graft_src), osapiInet_ntoa(graft_grp));
    }

    if (prune->neighbor == nbr)
    {
      if (L7_SUCCESS != SLLDelete(&(entry->ll_prunes), (L7_sll_member_t *)prune))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Deletion failure\n", 
                    __FUNCTION__, __LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                    "%s:%d: Prune Deletion failed from src = %s,"
                    "grp = %s \n", __FUNCTION__, __LINE__, 
                    osapiInet_ntoa(graft_src), osapiInet_ntoa(graft_grp));
      }
    }
    prune = pTmpPrune;
  }
  osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
  /* Check whether the prune had been sent upstream */
  need_graft = BIT_TEST (entry->flags, DVMRP_CACHE_NEGATIVE);
  if (!MCAST_BITX_TEST ((entry->routers), interface->index))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                "%s:%d: Setting the ifindex=%d found  from src = %s,"
                "grp = %s \n", __FUNCTION__, __LINE__, 
                osapiInet_ntoa(graft_src), osapiInet_ntoa(graft_grp), 
                interface->index);
    MCAST_BITX_SET((entry->routers), interface->index);      
    MCAST_BITX_RESET(entry->pruneRcvd, interface->index);
    dvmrp_cache_mfc_update (entry);
  }
  if (need_graft)
  {
    if (route->neighbor != L7_NULLPTR)
    {
      if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, 
                                       &route->neighbor->nbrAddr.addr, &nbrAddr))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert InetAddress to IpAddress\n",
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
      DVMRP_DEBUG(DVMRP_DEBUG_RX, 
                  "%s:%d: Sending the GRAFT for src = %s,grp = %s \n "
                  "to Nbr=%s", __FUNCTION__, __LINE__, 
                  osapiInet_ntoa(graft_src), osapiInet_ntoa(graft_grp), 
                  osapiInet_ntoa(nbrAddr));
      if (dvmrp_graft_send(entry, route->neighbor, 0) != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Graft Not Sent \n",
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;

      }
    }
  }


    }
    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                  "%s:No more cache entries in the table:%d\n",
                  __FUNCTION__, __LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:No more cache entries in the table:%d\n",
                  __FUNCTION__, __LINE__);
      break;
    }
  }
  
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  This function processes the DVMRP graft ack packet
*
* @param    interface - -  @b{(input)}Pointer to the Interface through which the graft 
*                                      ack packet has been recieved.
* @param    versionSupport - -  @b{(input)} Verison supported by the 
*                                      neighbouring router
* @param    source - -  @b{(input)}Pointer to the Source address of the packet
* @param    data -  -  @b{(input)} pointer to the packet buffer
* @param    datalen - -  @b{(input)} length of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    Handles the graft ack packet and updates the cache table 
*           accordingly.
* @end
*********************************************************************/
L7_RC_t dvmrp_graft_ack_recv(dvmrp_interface_t *interface, 
                             L7_ulong32 versionSupport, L7_dvmrp_inet_addr_t *source, 
                             L7_uchar8 *data, L7_int32 datalen)
{
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  L7_ulong32 graft_src, graft_grp;
  L7_inet_addr_t graft_source, graft_group;
  L7_uchar8 *endp = data + datalen;
  dvmrp_cache_entry_t e, *entry = L7_NULLPTR;
  dvmrp_route_t* route;
  dvmrp_t *dvmrpcb = L7_NULLPTR;    

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_FAILURE;
  }


  if (endp - data < 12)
  {
    /*  increment for bad Pkts */
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }

  nbr = dvmrp_neighbor_lookup (interface, versionSupport, source);
  if (nbr == L7_NULLPTR)
  {
    dvmrp_badpkts_inc(interface,L7_NULLPTR);
    return L7_FAILURE;
  }
  if (BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))
  {
    dvmrp_badpkts_inc(interface,L7_NULLPTR);
    return L7_FAILURE;
  }
  MCAST_GET_LONG (graft_src, data);
  MCAST_GET_LONG (graft_grp, data);

  if (data > endp)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Graft Ack Msg Length is not valid", datalen,
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != inetAddressSet(dvmrpcb->family, &graft_src, &graft_source))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (L7_SUCCESS != inetAddressSet(dvmrpcb->family, &graft_grp, &graft_group))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Failed to set the address\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  memcpy(&e.source, &graft_source,sizeof(L7_inet_addr_t));
  memcpy(&e.group, &graft_group,sizeof(L7_inet_addr_t));
  if (L7_SUCCESS != dvmrpCacheTableCacheGet(dvmrpcb,&e, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "(%s):(%d)Couldn't find the requested cache entry \n", 
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  if (entry == L7_NULLPTR || !BIT_TEST (entry->flags, DVMRP_CACHE_DVMRP_GRAFT))
  {
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }
  route = (dvmrp_route_t*) entry->data;
  if (route == L7_NULLPTR || route->neighbor != nbr)
  {
    dvmrp_badpkts_inc(interface,nbr);
    return L7_FAILURE;
  }
  BIT_RESET (entry->flags, DVMRP_CACHE_DVMRP_GRAFT);
  /*
   * if we have sent a prune for this entry, and are now receiving 
   * acknowledgement of a graft, clear out the prune context
   */
  if ((entry->count != 0) || (entry->firstPrune !=0))
  {
    entry->count = 0;
    entry->firstPrune = 0;
  if(entry->cacheRemove_timer != L7_NULLPTR)
  {
  if (L7_SUCCESS != appTimerDelete(dvmrpcb ->timerHandle, entry->cacheRemove_timer))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s: Failed to delete timer(%d) \n",
                __FUNCTION__,__LINE__);
  }
    entry->cacheRemove_timer = L7_NULLPTR;
  }

  if (entry->cacheRemoveHandle != L7_NULL)
  {
    handleListNodeDelete(dvmrpcb ->handle_list,
                         &entry->cacheRemoveHandle);
  }

  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);

  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  This function sends the DVMRP graft ack packet
*
* @param    interface       -  @b{(input)}Pointer to the Interface throufh which the graft 
*                                         ack packet has to be sent.
* @param    graft_src            -  @b{(input)}Pointer to the Source node
* @param    graft_grp            -  @b{(input)}Pointer to the group node
* @para       nbr                -  @b{(input)}Pointer to the neighbor to whom the packet
*                                                     needs to be sent
*
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    Send the graft ack pack to the neighbour.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_graft_ack_send(dvmrp_interface_t *interface,L7_inet_addr_t *graft_src, 
                               L7_inet_addr_t *graft_grp,dvmrp_neighbor_t *nbr)
{
  L7_uchar8 sendbuf[DVMRP_MAX_DVMRP_DATA_LEN];
  L7_uchar8 *cp = sendbuf;
  L7_uint32 s_addr, g_addr;
  dvmrp_t *dvmrpcb = L7_NULLPTR;      

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);

  dvmrpcb = interface->global;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_FAILURE;
  }

  assert (nbr);

  if (dvmrpcb->family == L7_AF_INET)
  {
    /* --  -- Change src addr */
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, graft_src, &s_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert InetAddress to IpAddress\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, graft_grp, &g_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert InetAddress to IpAddress\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    MCAST_PUT_NETLONG (osapiHtonl(g_addr), cp);
    {
      unsigned long mask = 0xffffffff;
      dvmrpRouteData_t *pData = L7_NULLPTR;
      L7_uint32 skipentries = 0;
      dvmrpTreeKey_t key;
      dvmrpKeySet(&key, *graft_src);
      /* find the longest match */
      pData = radixMatchNode(&dvmrpcb->dvmrpRouteTreeData, &key, skipentries);
      if (pData == L7_NULLPTR)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                    "%s:%d Failed to get the best match for addr=%s\n",
                    __FUNCTION__, __LINE__, osapiInet_ntoa(s_addr));
        DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d No Best match for the addr=%s\n",
                    __FUNCTION__, __LINE__, osapiInet_ntoa(s_addr));
        return L7_FAILURE;
      }
      if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &pData->netmask.addr, &mask))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't convert InetAddress to IpAddress\n",
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }

      MCAST_PUT_LONG (mask, cp);
    }

    DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);  
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d Sent the GRAFT-ACK-PACKET src=%s\n",
                __FUNCTION__, __LINE__, osapiInet_ntoa(s_addr));

    interface->graftAckPktsSentCount++;    
    dvmrp_send (DVMRP_GRAFT_ACK, &nbr->nbrAddr, sendbuf, cp - sendbuf, 
                       interface);
    return L7_SUCCESS;
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Family type %d not supported\n\n",dvmrpcb->family,
                __FUNCTION__, __LINE__);            
    DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);      
    return L7_FAILURE;
  }

}

/*********************************************************************
* @purpose  This function sends the DVMRP graft packet
* 
* @param    entry               -  @b{(input)}Pointer to the cahce entry
* @param    nbr                  -  @b{(input)}Pointer to the neighbor node 
* @param    fromTimeout    -  @b{(input)} Graft node needs to added or not
* 
* @returns  L7_SUCCESS/L7_FAILURE.
* 
* @notes    Send the graft packet to the neighbour.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_graft_send (dvmrp_cache_entry_t *entry, dvmrp_neighbor_t *nbr, 
                           L7_int32 fromTimeout)
{
  L7_uchar8 sendbuf[DVMRP_MAX_DVMRP_DATA_LEN];
  L7_uchar8 *cp = sendbuf;
  dvmrp_interface_t *interface;
  dvmrp_graft_t *graft;
  L7_uint32 now;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  L7_inet_addr_t   source,route_network;
  L7_uint32 s_addr = L7_NULL;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  L7_char8 nbrAddr[IPV6_DISP_ADDR_LEN],src[IPV6_DISP_ADDR_LEN],grp[IPV6_DISP_ADDR_LEN];
  L7_ulong32 capabilities;
  L7_ulong32 timerRunning =L7_FALSE;
  



  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  if (nbr == L7_NULLPTR || nbr->interface == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Nbr PTR or INTR PTR NULL, returning error\n",
                __FUNCTION__,__LINE__);
    return(L7_ERROR);
  }
  if (BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Nbr delete flag set, returning error\n",
                __FUNCTION__,__LINE__);
    return(L7_ERROR);     
  }
  interface =nbr->interface;
  dvmrpcb = interface->global;
  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return L7_FAILURE;
  }


  now = osapiUpTimeRaw();
  BIT_SET (entry->flags, DVMRP_CACHE_DVMRP_GRAFT);
  BIT_RESET (entry->flags, DVMRP_CACHE_DELETE);

  if (entry->cacheRemove_timer != L7_NULLPTR)
  {
    appTimerDelete(dvmrpcb->timerHandle, entry->cacheRemove_timer);
    if (entry->cacheRemoveHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &entry->cacheRemoveHandle);
    }
    entry->cacheRemove_timer=L7_NULLPTR;    
  }
  
  if (entry->prune_retry_timer != L7_NULLPTR)
  {
    appTimerDelete(dvmrpcb->timerHandle, entry->prune_retry_timer);
    entry->prune_retry_timer=L7_NULLPTR;
  }

    if (entry->pruneRetransmitimerHandle != L7_NULL)
    {
      handleListNodeDelete(dvmrpcb->handle_list,
                           &entry->pruneRetransmitimerHandle);
    }

 
  entry->holdtime = DVMRP_CACHE_EXPIRE_TIME;
  entry->expire = now + DVMRP_CACHE_EXPIRE_TIME;
  if (inetAddrHtop(&nbr->nbrAddr.addr, nbrAddr) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return  L7_FAILURE;


  }
  if (inetAddrHtop(&entry->source, src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return  L7_FAILURE;


  }
  if (inetAddrHtop(&entry->group, grp) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return  L7_FAILURE;


  }

  DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Graft to be sent to src=%s,grp=%s,nbr=%s on index=%d\n",
              __FUNCTION__,__LINE__,src,grp,nbrAddr,nbr->interface->index);

  /* Make sure neighbor is capable of receiving prunes otherwise don't send grafts too*/
  capabilities = (nbr->versionSupport >> 16) & 0x000F;
  if ( (capabilities & (0x1 << L7_DVMRP_CAPABILITIES_PRUNE)) == 0 )
  {

    BIT_RESET (entry->flags, DVMRP_CACHE_DVMRP_GRAFT);
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Neighbor is not prune capable.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if (!fromTimeout)
  {
    graft = entry->graft_sent; 
      
    if(graft == L7_NULLPTR)
    {
    if((graft = DVMRP_ALLOC (L7_AF_INET, sizeof (dvmrp_graft_t)))== L7_NULLPTR)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n %s, %d : Memory allocation failed",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    }
    else
    {
      timerRunning = L7_TRUE;

    }
    memset(graft, L7_NULL, sizeof(dvmrp_graft_t));
    graft->neighbor = nbr;
    graft->holdtime = DVMRP_GRAFT_TIMEOUT_VAL;
    graft->entry = entry;
    graft->received = now;
    graft->expire = now + graft->holdtime;
    entry->graft_sent = graft;  

    if(timerRunning == L7_TRUE)
    {
      if (appTimerUpdate(dvmrpcb->timerHandle, entry->graft_timer, dvmrp_graft_timeout, 
                         (void *)entry->graftTimeoutHandle, graft->holdtime,
                         "DVMRP Graft Timer4") != L7_SUCCESS)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s : %d Timer Updation failed\n",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }
    }
    else
    {
      entry->graftTimeoutHandle =
      handleListNodeStore(dvmrpcb->handle_list, (void*)entry);
      if (L7_NULLPTR == (entry->graft_timer = appTimerAdd(dvmrpcb->timerHandle,
                                                          dvmrp_graft_timeout,
                                                          (void *)entry->graftTimeoutHandle,
                                                          graft->holdtime,
                                                          "DV-GT")))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s : %d Timer creation failed\n",__FUNCTION__,__LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s : %d Timer creation failed\n",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }
    }

    if (nbr && nbr->interface)
    {
      MCAST_BITX_RESET(entry->pruneSent, nbr->interface->index);
    }
    /* CHANGE */
    if (BIT_TEST (nbr->flags, DVMRP_NEIGHBOR_DELETE))/*Merge from REL_K*/
    {
      DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Nbr Delete flag set so returning\n",
                  __FUNCTION__,__LINE__);
      return(L7_SUCCESS);
    }
  }

  inetCopy(&source,&entry->source);

  if (inetAddrHtop(&entry->source,src) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;


  }
  inetAddrHton(&entry->source, &route_network);
  /* Draft section 3.5.3.4 */
  dvmrpKeySet(&key, route_network);

  /* find the longest match */
  pData = radixMatchNode(&dvmrpcb->dvmrpRouteTreeData, &key, skipentries);
  if (pData == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                "%s:%d Failed to get the best match for addr=%s\n",
                __FUNCTION__, __LINE__, src);
    DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d No Best match for the addr=%s\n",
                __FUNCTION__, __LINE__, src);
    DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d No Best match for the addr=%s\n",
                __FUNCTION__,__LINE__,src);
    return L7_FAILURE;
  }

  if (dvmrpcb->family == L7_AF_INET)
  {
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &entry->source, &s_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't get the IP ADDRESS\n\n",
                  __FUNCTION__, __LINE__);     
      return L7_FAILURE;
    }
    MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &entry->group, &s_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't get the IP ADDRESS\n\n",
                  __FUNCTION__, __LINE__);     
      return L7_FAILURE;
    }
    MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);

    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &pData->netmask.addr, &s_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't get the IP ADDRESS\n\n",
                  __FUNCTION__, __LINE__);     
      return L7_FAILURE;
    }

    MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);

    DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);  
    DVMRP_DEBUG(DVMRP_DEBUG_GRAFT,"%s:%d Sending the GRAFT PKT addr=%s\n",
                __FUNCTION__,__LINE__,nbrAddr);

    nbr->interface->graftPktsSentCount++;    
    dvmrp_send (DVMRP_GRAFT, &nbr->nbrAddr, sendbuf, cp - sendbuf,
                       nbr->interface);
    return L7_SUCCESS;
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Family type %d not supported\n\n",dvmrpcb->family,
                __FUNCTION__, __LINE__);            
    DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);      
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  This function sends the DVMRP probe packet
*
* @param    interface    -  @b{(input)}Pointer to the interface through which probe 
*                                          packet has to be sent.
* @param    addr         -  @b{(input)} Pointer to the address of the probe packet
* 
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Send the probe packet on to the given interface.
* @end
*********************************************************************/
L7_int32 dvmrp_probe_send(dvmrp_interface_t *interface, 
                          L7_dvmrp_inet_addr_t *addr)
{
  dvmrp_neighbor_t *nbr;
  L7_uchar8 sendbuf[DVMRP_MAX_DVMRP_DATA_LEN];
  L7_uchar8 *cp = sendbuf;
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  L7_uint32 ss_addr = L7_NULL;
  L7_uchar8 nbrNode[IPV6_DISP_ADDR_LEN];
  L7_uchar8 src[IPV6_DISP_ADDR_LEN] ={'\0'};

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  if (interface == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : interface is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return(L7_FAILURE);
  }

  if (addr != L7_NULLPTR)
  {
    if (inetAddrHtop(&addr->addr, src) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
  }
  dvmrpcb = interface->global;
  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpCB is NULL!\n\n",
                __FUNCTION__, __LINE__);            
    return(L7_FAILURE);
  }
  if (!MCAST_BITX_TEST (dvmrpcb->interface_mask, interface->index))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_PROBE,"\n%s:IfIndex is not set on the intf bitmask NBR=%s\n",
                __FUNCTION__,src);
    return(L7_FAILURE);
  }
  if (dvmrpcb->family == L7_AF_INET)
  {
    MCAST_PUT_LONG (osapiHtonl(interface->genid), cp);
    for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(interface->ll_neighbors));
        nbr != L7_NULLPTR;
        nbr = (dvmrp_neighbor_t*)SLLNextGet(&(interface->ll_neighbors),(void*)nbr))
    {
      if (nbr->state != L7_DVMRP_NEIGHBOR_STATE_DOWN)
      {
        if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &nbr->nbrAddr.addr, &ss_addr))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't get the IP ADDRESS\n\n",
                      __FUNCTION__, __LINE__);     
          return L7_FAILURE;
        }
        if (inetAddrHtop(&nbr->nbrAddr.addr, nbrNode) != L7_SUCCESS)
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                      __FUNCTION__, __LINE__);
          return L7_FAILURE;


        }
        MCAST_PUT_NETLONG (osapiHtonl(ss_addr), cp);
        DVMRP_DEBUG(DVMRP_DEBUG_PROBE,
                    "\n%s:Added NBR=%s to the PROBE PKT being sent to NBR=%s\n",
                    __FUNCTION__, nbrNode, src);
      }
    }
    DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
    /* I don't know but mrouted does so */
    MCAST_PUT_LONG (0L, cp);
    DVMRP_DEBUG(DVMRP_DEBUG_PROBE,"\n%s:PKT sent SUCCESSFULLY to NBR=%s\n",
                __FUNCTION__,src);
    interface->probePktsSentCount++;    
    dvmrp_send (DVMRP_PROBE, L7_NULLPTR, sendbuf, cp - sendbuf,
                       interface);
    return L7_SUCCESS;
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Family type %d not supported\n\n",dvmrpcb->family,
                __FUNCTION__, __LINE__);            
    DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);    
    return L7_FAILURE;
  }

}

/*********************************************************************
* @purpose  This function sends the DVMRP probe packet
*
* @param    pParam -void pointer to get the required info.
* @returns  None.
*
* @notes    Send the probe packet on to the given interface.
* 
* @end
*********************************************************************/
void dvmrp_probe_sendHandler(void *pParam)
{
  dvmrp_timer_event_t *timerBlock = L7_NULLPTR;
  dvmrp_interface_t *interface = L7_NULLPTR;
  L7_dvmrp_inet_addr_t *addr = L7_NULLPTR;
  L7_int32  handle = (L7_int32)pParam;
  L7_int32 rtrIfNum;
  dvmrp_t *dvmrpcb; 

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);

  timerBlock = (dvmrp_timer_event_t *)handleListNodeRetrieve(handle);
  if (L7_NULLPTR == timerBlock)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"Invalid input parameter");
    return ;
  }

  dvmrpcb = timerBlock->data4;

  if (dvmrpcb == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpCB is NULL!\n\n");            
    return; 
  }

  rtrIfNum = timerBlock->data1; 

  if (!MCAST_BITX_TEST(dvmrpcb->interface_mask, rtrIfNum))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d interface %d Is not Enables.So Returning\n", 
                __FUNCTION__, __LINE__,rtrIfNum);
    return;
  }

  interface = &dvmrpcb->dvmrp_interfaces[rtrIfNum];
  if (interface->index != rtrIfNum)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d interface pointer is Invalid\n", 
                __FUNCTION__, __LINE__);
    return; 
  }

  addr = timerBlock->data3;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  if (dvmrp_probe_send(interface, addr) != L7_SUCCESS)
  {
    L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
             "DVMRP Probe Control message Send Failed on rtrIfNum - %d"
             " DVMRP Probe control message send failed."
             " This could mostly be because of a Failure"
             " return status of the socket call sendto()."
             " As a result of this, the DVMRP neighborship"
             " could be lost in the neighboring DVMRP routers.",
             interface->index);
  }

  if (L7_NULLPTR == (interface->probe = appTimerAdd(dvmrpcb->timerHandle,
                                                    dvmrp_probe_sendHandler, (void *)interface->probePeriodicTimerHandle, 
                                                    DVMRP_NEIGHBOR_PROBE_INTERVAL,
                                                    "DV-PRB2")))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d timer instantiation failed \n", 
                __FUNCTION__, __LINE__);
    return;
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
}

/*********************************************************************
* @purpose  This function sends the DVMRP prune packet
*
* @param    interface    -        @b{(input)}Pointer to the interface through which probe
*                                               packet has to be sent.
* @param    src          -        @b{(input)}Pointer to the  Source network being pruned
* @param    dst          -        @b{(input)}Pointer to the  Destination group being pruned
* @param    lifetime     -        @b{(input)}  lifetime of the prune
* @param    neighbor     -        @b{(input)}Pointer to the Nighbor to whom the prune 
*                                                 should be sent
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends the prune packet on the given interface.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_prune_send(dvmrp_interface_t *interface, L7_inet_addr_t *src, 
                          L7_inet_addr_t *dst, L7_int32 lifetime, 
                          dvmrp_neighbor_t *neighbor)
{
  L7_uchar8 sendbuf[DVMRP_MAX_DVMRP_DATA_LEN];
  L7_uchar8 *cp = sendbuf;
  L7_ulong32 capabilities;
  dvmrp_cache_entry_t* entry = L7_NULLPTR;
  L7_int32 prune_remaining_lifetime = 0;
  dvmrp_t *dvmrpcb = L7_NULLPTR;
  L7_uint32 s_addr;
  dvmrpRouteData_t *pData = L7_NULLPTR;
  L7_uint32 skipentries = 0;
  dvmrpTreeKey_t key;
  L7_char8 src_str[IPV6_DISP_ADDR_LEN];  
  L7_inet_addr_t  route_network;
  L7_uint32  bitLen=0;  
  L7_uint32  bitLen1=0;  

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);
  if ((interface == L7_NULLPTR) || (neighbor == L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : Either interface = L7_NULLPTR Or neighbor = L7_NULLPTRn\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  dvmrpcb = interface->global;

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }


  /* lookup cache entry for this (S,G) */
  entry = dvmrp_cache_lookup(dvmrpcb, src,dst);
  /* 
   * if found, we can store information about the prune in the entry 
   * structure 
   * */
  if (entry != L7_NULLPTR)
  {
    L7_uint32 now;

    now = osapiUpTimeRaw();
    /* store time when this prune (initial or retry) message is sent */
    entry->lastPrune = now;
    /*
     * if this is the first prune sent for this entry or if the firstPrune 
     * time field is uninitialized for some reason, set the firstPrune record 
     * in this entry to current time
     */
    if ((entry->count == 0) || (entry->firstPrune == 0))
    {
      entry->firstPrune = now;
      entry->lifetime = lifetime;
    }
    /* 
     * calculate retry interval based on number of times prunes have been sent 
     * for this entry (binary exponential backoff) 
     */
    entry->prune_retry_interval = (DVMRP_PRUNE_REXMIT_VAL << entry->count);
    /* 
     * lifetime field in the prune message is set to passed value if first 
     * time, set to remaining lifetime if a retry 
     */
    prune_remaining_lifetime = entry->lifetime - (now - entry->firstPrune);
    /*
     * if the backoff time interval for this prune retry is longer than 
     * remaining life of this prune, set backoff time equal to remaining 
     * prune lifetime
     */
    if (entry->prune_retry_interval > prune_remaining_lifetime)
    {
      entry->prune_retry_interval = prune_remaining_lifetime;
    }
    /*
     * set the entry's holdtime to last as long as the prune's lifetime... 
     * this is to retain prune state information for as long as a sent prune 
     * is valid
     */
    entry->holdtime = prune_remaining_lifetime;
    entry->expire = now + entry->holdtime;
    /* 
     * To implement prune retransmit, we will add a negative entry to lower 
     * forwarding tables.  A timer is set to expire the prune retransmit time.
     * After timer fires, negative entry is removed from lower tables.  If 
     * data is received after negative entry removed, prune retransmission 
     * will be triggered.
     */
    if (!entry->prune_retry_timer)
    {

      /* 
       * if no prune timer yet created for entry, create one and 
       * initialize it 
       */
      entry->pruneRetransmitimerHandle =
      handleListNodeStore(dvmrpcb->handle_list, (void*)entry);

      entry->prune_retry_timer = appTimerAdd(dvmrpcb->timerHandle,
                                             dvmrp_prune_retransmit_timer_expire,
                                             (void *)entry->pruneRetransmitimerHandle,
                                             entry->prune_retry_interval,
                                             "DV-PRxT");
    }
    else
    {
      L7_uint32 timeLeft = 0;

      /* else, just update interval */
      if (L7_SUCCESS != appTimerTimeLeftGet(dvmrpcb->timerHandle, 
                                            entry->prune_retry_timer, &timeLeft))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                    "%s:%d Failed to get the time left\n", 
                    __FUNCTION__, __LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d Failed to get the time left\n", 
                    __FUNCTION__, __LINE__);
      }
      if (timeLeft >= L7_NULL)
      {
        appTimerDelete(dvmrpcb->timerHandle, entry->prune_retry_timer);
        if (entry->pruneRetransmitimerHandle != L7_NULL)
        {
          handleListNodeDelete(dvmrpcb->handle_list,
                               &entry->pruneRetransmitimerHandle);
        }
        entry->prune_retry_timer=0;
      }
      entry->pruneRetransmitimerHandle =
      handleListNodeStore(dvmrpcb->handle_list, (void*)entry);

      if (L7_NULLPTR == (entry->prune_retry_timer = 
                         appTimerAdd(dvmrpcb->timerHandle, 
                                     dvmrp_prune_retransmit_timer_expire,
                                     (void *)entry->pruneRetransmitimerHandle,
                                     entry->prune_retry_interval,
                                      "DV-PRxT2")))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                    "%s:%d timer instantiation failed \n", 
                    __FUNCTION__, __LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_ROUTE, "%s:%d timer instantiation failed \n", 
                    __FUNCTION__, __LINE__);
        return L7_FAILURE;   
      }
    }

    if(entry->graft_sent != L7_NULLPTR)
    {
      appTimerDelete(dvmrpcb->timerHandle, entry->graft_timer);
      DVMRP_FREE (L7_AF_INET, (void*) entry->graft_sent);        
      entry->graft_sent=L7_NULLPTR;   
      entry->graft_timer=L7_NULLPTR;
    }
      if (entry->graftTimeoutHandle != L7_NULL)
      {
        handleListNodeDelete(dvmrpcb->handle_list,
                             &entry->graftTimeoutHandle);
      }
    
  }
  assert (neighbor);
  if (BIT_TEST (neighbor->flags, DVMRP_NEIGHBOR_DELETE))
  {
    return L7_FAILURE;   
  }

  if (dvmrpcb->family == L7_AF_INET)
  {
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, src, &s_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't get the IP ADDRESS\n\n",
                  __FUNCTION__, __LINE__);     
      return L7_FAILURE;
    }
    MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);
    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, dst, &s_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't get the IP ADDRESS\n\n",
                  __FUNCTION__, __LINE__);     
      return L7_FAILURE;
    }
    MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);
    MCAST_PUT_LONG (prune_remaining_lifetime, cp);

    if (inetAddrHtop(src,src_str) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d InetAddress to ASCII conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    if (inetAddrHton(src, &route_network) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Host to Network conversion Failed.\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }
    /* Draft section 3.5.3.4 */
    dvmrpKeySet(&key, route_network);

    /* find the longest match */
    pData = radixMatchNode(&dvmrpcb->dvmrpRouteTreeData, &key, skipentries);
    if (pData == L7_NULLPTR)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                  "%s:%d Failed to get the best match for addr=%s\n",
                  __FUNCTION__, __LINE__, src_str);
      DVMRP_DEBUG(DVMRP_DEBUG_RX, "%s:%d No Best match for the addr=%s\n",
                  __FUNCTION__, __LINE__, src_str);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != inetAddressGet(dvmrpcb->family, &pData->netmask.addr, &s_addr))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Couldn't get the IP ADDRESS\n\n",
                  __FUNCTION__, __LINE__);     
      return L7_FAILURE;
    }
    MCAST_PUT_NETLONG (osapiHtonl(s_addr), cp);

    /* Make sure neighbor is capable of receiving prunes */
    capabilities = (neighbor->versionSupport >> 16) & 0x000F;
    if ((capabilities & (0x1 << L7_DVMRP_CAPABILITIES_PRUNE)) == 0)
    {
      return L7_FAILURE;
    }

    if (L7_SUCCESS != inetMaskToMaskLen(&pData->netmask.addr, (L7_uchar8 *)&bitLen1))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : Mask to length conversion Failed!\n\n",
                  __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }

    bitLen =bitLen1;

    DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
    /* 
     * Before sending the prune, the check needs to be made that we have  send
     * a prune to the upstream router for another source that is present in the
     * same network for the same group address. If so, the prune need not be sent.
     */
    if (dvmrp_send_prune_check(dvmrpcb,src,dst,bitLen) == L7_FAILURE)
    {
      neighbor->interface->prunePktsSentCount++;
      dvmrp_send (DVMRP_PRUNE, &neighbor->nbrAddr, sendbuf, cp - sendbuf,
                         neighbor->interface);
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }
  else
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Family type %d not supported\n\n",dvmrpcb->family,
                __FUNCTION__, __LINE__);            
    DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);      
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  This function is used to elect the DF 
*
* @param   route    -  @b{(input)}Pointer to the route entry 
*                nbr      -   @b{(input)}Pointer to the neighbor node
*                metric  -   @b{(input)}Pointer to the metric
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_df_elect (dvmrp_route_t *route, dvmrp_neighbor_t* nbr, 
                     L7_int32 metric)
{
  /*
   * check if route came on downstream interface check if peer is advertising 
   * better metric than ours if metric is same, but his ip address is less 
   * than ours then the peer becomes df on the downstream outgoing interface
   */
  desg_fwd_t *desg_fwd = L7_NULLPTR;
  dvmrp_cache_entry_t tmpEntry, *entry = L7_NULLPTR;
  L7_int32 comp_metric = 0;
  L7_inet_addr_t  compPrefix, *comp_prefix = L7_NULLPTR;
  dvmrp_t *dvmrpcb = L7_NULLPTR;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: ENTERED \n", __FUNCTION__);

  if ((nbr->interface == L7_NULLPTR) ||(nbr->interface->global ==L7_NULLPTR))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Either nbr->interface = NULL Or  nbr->interface->global =NULL\n\n",
                __FUNCTION__, __LINE__);     
    return;
  }

  dvmrpcb = nbr->interface->global;

  memset(&compPrefix, L7_NULL, sizeof(L7_inet_addr_t));
  comp_prefix = &compPrefix;

  desg_fwd = &route->desg_fwd[nbr->interface->index];
  if (desg_fwd->status == DVMRP_ENTRY_NOT_USED)
  {
    comp_metric = route->metric ? route->metric: 1;
    comp_prefix = &nbr->interface->primary.inetAddr.addr;
  }
  else
  {
    comp_metric = desg_fwd->metric;
    if (desg_fwd->fwd_nbr != L7_NULLPTR)
    {
      comp_prefix = &desg_fwd->fwd_nbr->nbrAddr.addr;
    }
  }

  if ((metric < comp_metric) ||
      (( metric == comp_metric ) &&
       L7_INET_ADDR_COMPARE(&nbr->nbrAddr.addr, comp_prefix) < L7_NULL))
  {
    /* Select the new designated forwader */
    if (desg_fwd->status == DVMRP_ENTRY_NOT_USED)
    {
      memset(desg_fwd, L7_NULL, sizeof(desg_fwd_t));
      desg_fwd->status =  DVMRP_ENTRY_USED;      

      memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
      if (L7_SUCCESS != 
          dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                    "%s:There is no cache entry in the table:%d\n", 
                    __FUNCTION__, __LINE__);
      }
      while (L7_NULLPTR != entry)
      {
        /* we get the cache entry...*/
        if (route == (dvmrp_route_t *) entry->data)
        {
          if ((dvmrp_any_dependents_on_this_interface(&route->dependents, 
                                                      nbr->interface) 
               == L7_SUCCESS))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                        "%s:Clearing the entry->router bitmask:%d\n", 
                        __FUNCTION__, __LINE__);
            MCAST_BITX_RESET(entry->routers, nbr->interface->index);
          }

          if ((dvmrpMemberShipTest (dvmrpcb,&entry->group, &entry->source, 
                                    nbr->interface->index) == L7_SUCCESS))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                        "%s:Clearing the entry->children bitmask:%d\n", 
                        __FUNCTION__, __LINE__);
            MCAST_BITX_RESET(entry->children, nbr->interface->index);
          }
          dvmrp_cache_mfc_update(entry);
        }
        memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                      "%s:There is no cache entry in the table:%d\n", 
                      __FUNCTION__, __LINE__);
          break;
        }
      }
    }
    desg_fwd->fwd_nbr = nbr;
    desg_fwd->metric = metric;
  }
  else if ((((L7_INET_ADDR_COMPARE(&nbr->nbrAddr.addr, comp_prefix) == L7_NULL) && 
             (metric > comp_metric)) ||
            ((L7_INET_ADDR_COMPARE(&nbr->nbrAddr.addr, &nbr->interface->primary.inetAddr.addr) > L7_NULL) && 
             (metric == comp_metric) &&
             (L7_INET_ADDR_COMPARE(&nbr->nbrAddr.addr, comp_prefix) != L7_NULL))) &&
           (route->desg_fwd[nbr->interface->index].status== DVMRP_ENTRY_USED))
  {
    /* Assume the control as DF for this downstream interface */
    if (desg_fwd->status == DVMRP_ENTRY_USED)
    {
      memset(desg_fwd, L7_NULL, sizeof(desg_fwd_t));
      desg_fwd->status =  DVMRP_ENTRY_NOT_USED;      
    }

    memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                  "%s:There is no cache entry in the table:%d\n", 
                  __FUNCTION__, __LINE__);
    }
    while (L7_NULLPTR != entry)
    {
      L7_int32 need_graft = 0;
      dvmrp_prune_t *prune = L7_NULLPTR;
      dvmrp_cache_entry_t *entryNext = L7_NULLPTR;      

      memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
      if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entryNext ))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                    "%s:There is no cache entry in the table:%d\n", 
                    __FUNCTION__, __LINE__);
        break;
      }

      if (entry->data != route)
      {
        entry = entryNext;
        continue;
      }

      if (osapiWriteLockTake(dvmrpcb->dvmrpRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Could Not Access the semaphore.\n\n",
                    __FUNCTION__, __LINE__);
        return ;
      }

      for (prune = (dvmrp_prune_t*)SLLFirstGet(&(entry->ll_prunes));
          prune != L7_NULLPTR;
          prune = (dvmrp_prune_t*)SLLNextGet(&(entry->ll_prunes),(void*)prune))
      {
        if (L7_INET_ADDR_COMPARE(&prune->neighbor->nbrAddr.addr, 
                                 &nbr->nbrAddr.addr) == L7_NULL)
        {
          if (L7_SUCCESS != SLLDelete(&(entry->ll_prunes), 
                                      (L7_sll_member_t *)prune))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                        "%s:Failed to delete the prune entry:%d\n", 
                        __FUNCTION__, __LINE__);
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, 
                        "%s:Failed to delete the prune entry:%d\n", 
                        __FUNCTION__, __LINE__);     
          }
          break;
        }
      }
      osapiWriteLockGive(dvmrpcb->dvmrpRwLock);
      need_graft = BIT_TEST(entry->flags,DVMRP_CACHE_NEGATIVE);
      if (need_graft && route->neighbor)
      {
        L7_inet_addr_t source, group;

        inetCopy(&source,&entry->source);
        inetCopy(&group,&entry->group);

        if (dvmrp_any_dependents_on_this_interface(&route->dependents,
                                                   nbr->interface) || /*Merge from the REL_K*/
            (dvmrpMemberShipTest(dvmrpcb,&group, &source, 
                                nbr->interface->index)== L7_SUCCESS))
        {
          if (dvmrp_graft_send(entry, route->neighbor, 0) != L7_SUCCESS)
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d Graft Not Sent \n",
                        __FUNCTION__, __LINE__);
            return;
          }
        }
      }
      if (entry->parent != nbr->interface)
      {
        if ((dvmrp_any_dependents_on_this_interface(&route->dependents,
                                                    nbr->interface) == L7_SUCCESS))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                      "%s:Set index=%d the entry->routers bitmask:%d\n", 
                      __FUNCTION__, nbr->interface->index, __LINE__);
          MCAST_BITX_SET(entry->routers, nbr->interface->index);
        }
        if ((dvmrpMemberShipTest (dvmrpcb,&entry->group, &entry->source, 
                                  nbr->interface->index) == L7_SUCCESS))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE, 
                      "%s:Set index=%d the entry->children bitmask:%d\n",
                      __FUNCTION__, nbr->interface->index, __LINE__);
          MCAST_BITX_SET(entry->children, nbr->interface->index);
        }
        dvmrp_cache_mfc_update (entry);
      }
      entry = entryNext;      
    }
  }
  DVMRP_DEBUG(DVMRP_DEBUG_APIS, "%s: LEAVING \n", __FUNCTION__);
}
/*********************************************************************
*
* @purpose  To De-initialize the AdminScope Boundary Database
*
* @param    dvmrpcb  @b{ (input) } Pointer to the DVMRP Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundaryDeInit (  dvmrp_t *dvmrpcb)
{
  L7_RC_t retVal = L7_SUCCESS;

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);         

  /* Destory the Admin Scope Boundary Buffer Pool */

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  /* Destory the Admin Scope SLL */
  if (SLLDestroy (L7_FLEX_PIMDM_MAP_COMPONENT_ID, &(dvmrpcb->dvmrpasbList))
      != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:Admin Scope List Destroy Failure \n",__FUNCTION__);
    return L7_FAILURE;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: AdminScope Database Destroy Successful\n",
              __FUNCTION__);

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);          
  return retVal;
}

/*********************************************************************
*
* @purpose  To Initialize the AdminScope Boundary database
*
* @param    dvmrpcb  @b{ (input) } Pointer to the DVMRP Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundaryInit (dvmrp_t *dvmrpcb)
{

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);          

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  /* Initialize the Admin Scope SLL */
  if (SLLCreate (L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_SLL_NO_ORDER, L7_NULL , dvmrpAdminScopeEntryCompare ,
                 dvmrpAdminScopeBoundaryNodeDelete, &(dvmrpcb->dvmrpasbList))
      != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:Admin Scope List Creation Failure \n",__FUNCTION__);

    return L7_FAILURE;
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: AdminScope Database Creation Successful \n",
              __FUNCTION__);


  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);          

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose   Compare function for the Admin Scope List
*
* @param     pData1 @b{ (input) } Pointer to Admin Scope Entry
*            pData2 @b{ (input) } Pointer to Admin Scope Entry
*            size   @b{ (input) } Size for the comparision 
*                            
* @returns   > 0  if pData1 > pData2
*            = 0 if pData1 == pData2
*            < 0 if pData1 < pData2
*
* @comments  None
*
* @end
*********************************************************************/
L7_int32
dvmrpAdminScopeEntryCompare (void* pData1,
                             void* pData2,
                             L7_uint32 size)
{
  L7_sll_member_t *pKey1 = (L7_sll_member_t *) pData1;
  L7_sll_member_t *pKey2 = (L7_sll_member_t *) pData2;
  register L7_int32 retVal = 0;

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);          

  if ((pKey1 == L7_NULLPTR) || (pKey2 == L7_NULLPTR))
  {
    return 1;
  }

  if ((retVal = L7_INET_ADDR_COMPARE (&(((dvmrpASBNode_t*)pKey1)->grpAddr),
                                      &(((dvmrpASBNode_t*)pKey2)->grpAddr))) != 0)
  {
    return retVal;
  }

  retVal = L7_INET_ADDR_COMPARE (&(((dvmrpASBNode_t*)pKey1)->grpMask), 
                                 &(((dvmrpASBNode_t*)pKey2)->grpMask));

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);          

  return retVal;
}


/*********************************************************************
*
* @purpose  To Initialize the AdminScope Boundary database
*
* @param    asbNode  @b{ (input) } Admin Scope Node
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/


L7_RC_t
dvmrpAdminScopeBoundaryNodeDelete (L7_sll_member_t *asbNode)
{
  dvmrp_t* dvmrpcb = L7_NULLPTR;
  L7_uchar8 addrFamily = 0;

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);          

  if (asbNode == L7_NULLPTR)
  {
    DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\n\nINVALID : asbNode is NULL!\n\n");
    return L7_FAILURE;
  }

  addrFamily = L7_INET_GET_FAMILY (&(((dvmrpASBNode_t*)asbNode)->grpAddr));

  if ((dvmrpcb = dvmrpMapProtocolCtrlBlockGet (addrFamily)) == L7_NULLPTR)
    return L7_FAILURE;

  DVMRP_FREE (L7_AF_INET, (void*) asbNode);        

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);          

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the Adminscope Boundary Node for a Group
*
* @param    dvmrpcb   -     @b{ (input) } Pointer to the DVMRP Ctrl Block
*                grpAddr     -   @b{ (input) } Address of the Multicast Group
*                grpMask    -    @b{ (input) } Mask of the Multicast Group
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
dvmrpASBNode_t*
dvmrpAdminScopeNodeGet (dvmrp_t *dvmrpcb,
                        L7_inet_addr_t *grpAddr,
                        L7_inet_addr_t *grpMask)
{

  dvmrpASBNode_t  asbCurrNode;

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);          

  memset(&asbCurrNode,0,sizeof(dvmrpASBNode_t ));
  inetCopy (&(asbCurrNode.grpAddr), grpAddr);
  inetCopy (&(asbCurrNode.grpMask), grpMask);

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);          

  return(dvmrpASBNode_t  *)SLLFind(&dvmrpcb->dvmrpasbList,
                                   (L7_sll_member_t *)&asbCurrNode);
}

/*********************************************************************
*
* @purpose  Set the Adminscope Boundary for a Group on an Interface
*
* @param    dvmrpcb     -    @b{ (input) } Pointer to the DVMRP Ctrl Block
*                grpAddr       - @b{ (input) } Address of the Multicast Group
*                grpMask       -  @b{ (input) } Mask of the Multicast Group
*                rtrIfIndex    - @b{ (input) } Index of the Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundarySet (dvmrp_t *dvmrpcb,
                            L7_inet_addr_t *grpAddr,
                            L7_inet_addr_t *grpMask,
                            L7_uint32 rtrIfNum)
{
  dvmrpASBNode_t *asbNode = L7_NULLPTR;
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR;     
  dvmrp_neighbor_t* dvmrpNbr = L7_NULLPTR;  
  L7_uchar8 grp[DVMRP_MAX_DBG_ADDR_SIZE];
  L7_uchar8              maskLen;  
  L7_uchar8 oif[L7_INTF_INDICES];

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);          

  /* Get the ASB Node for the Group.
   */

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if ((asbNode = dvmrpAdminScopeNodeGet (dvmrpcb, grpAddr, grpMask))
      == L7_NULLPTR)
  {
    /* Node doesn't exist.  Create one.
     */
    if ((asbNode = DVMRP_ALLOC (L7_AF_INET, sizeof (dvmrpASBNode_t)))
                                == L7_NULLPTR)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:Admin Scope Node Alloc Failed  \n",__FUNCTION__);

      return L7_FAILURE;
    }

    /* Update the Node members and Add to the List.
     */
    inetCopy (&(asbNode->grpAddr), grpAddr);
    inetCopy (&(asbNode->grpMask), grpMask);    
    MCAST_BITX_SET (asbNode->intfMask, rtrIfNum);
    if (SLLAdd (&(dvmrpcb->dvmrpasbList), (L7_sll_member_t*) asbNode) != L7_SUCCESS)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                  "%s:Admin Scope Node Addition Failed  \n",__FUNCTION__);
      return L7_FAILURE;
    }
  }

  /* Verify if the Interface is already SET for this Group.
   */
  else
  {
    if (MCAST_BITX_TEST (asbNode->intfMask, rtrIfNum))
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Admin Scope Intf already SET for "
               "Grp - %s Intf - %d", inetAddrPrint(grpAddr,grp), rtrIfNum);

      return L7_FAILURE;
    }
    /* Set the Interface as Scoped for this Group.
     */
    MCAST_BITX_SET (asbNode->intfMask, rtrIfNum);
  }

  inetMaskToMaskLen(grpMask,&maskLen);

  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != 
      dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d No Entry found \n",
                __FUNCTION__,__LINE__);
    return L7_SUCCESS;
  }
  while (L7_NULLPTR != entry)
  {
    if (inetAddrCompareAddrWithMask(&entry->group,maskLen,grpAddr,maskLen) ==L7_SUCCESS)
    {
      if (rtrIfNum ==entry->parent->index)
      {
        dvmrpNbr = entry->data->neighbor;

        if (dvmrpNbr != L7_NULLPTR)
        {
          if ((dvmrpNbr->interface) && (!MCAST_BITX_TEST (entry->pruneSent,
                                                   dvmrpNbr->interface->index)))
          {
            MCAST_BITX_SET(entry->pruneSent, 
                           rtrIfNum);
            if (dvmrp_prune_send (dvmrpNbr->interface, &entry->source, 
                                &entry->group, DVMRP_AVERAGE_PRUNE_LIFETIME, 
                                dvmrpNbr) != L7_SUCCESS)
            {
              L7_uchar8 nbr[IPV6_DISP_ADDR_LEN];
              L7_uchar8 src[IPV6_DISP_ADDR_LEN];
              L7_uchar8 grp1[IPV6_DISP_ADDR_LEN];

              L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                       "DVMRP Prune Control message Send Failed; rtrIfNum - %d,"
                       " Neighbor - %s, SrcAddr - %s, GrpAddr - %s"
                       " DVMRP Prune control message send failed."
                       " This could mostly be because of a Failure"
                       " return status of the socket call sendto()."
                       " As a result of this, the unwanted multicast"
                       " traffic is still received and forwarded.",
                       dvmrpNbr->interface->index,
                       inetAddrPrint(&dvmrpNbr->nbrAddr.addr, nbr),
                       inetAddrPrint(&entry->source, src),
                       inetAddrPrint(&entry->group, grp1));
              return L7_FAILURE;
            }
          }
        }
        memset(&oif, 0 ,sizeof(oif));
        if (L7_SUCCESS != dvmrpCacheMfcUpdate(&entry->source, &entry->group, entry->parent, oif))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_NOCACHE,"%s:%d MFC failed to update HARDWARE with negative entry \n",
                      __FUNCTION__,__LINE__);
        }
        entry->flags |= DVMRP_CACHE_NEGATIVE;
      }
      else
      {
        dvmrp_update_fn_call(DVMRP_EVENT_ADMIN_SCOPE_ADD, entry, 
                             rtrIfNum); 
      }
    }
    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    entry = L7_NULLPTR;
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d No more Entry found \n",
                  __FUNCTION__,__LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d No more Entry found \n",
                  __FUNCTION__,__LINE__);
      break;
    }
  }

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);           

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Reset the Adminscope Boundary for a Group on an Interface
*
* @param    pimdmCB        - @b{ (input) } Pointer to the PIM-DM Ctrl Block
*                 grpAddr          - @b{ (input) } Address of the Multicast Group
*                 grpMask          - @b{ (input) } Mask of the Multicast Group
*                 rtrIfIndex        - @b{ (input) } Index of the Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundaryReset (dvmrp_t *dvmrpcb,
                              L7_inet_addr_t *grpAddr,
                              L7_inet_addr_t *grpMask,
                              L7_uint32 rtrIfNum)
{
  dvmrpASBNode_t *asbNode = L7_NULLPTR;
  L7_BOOL isEmpty = L7_FALSE;
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR;     
  dvmrp_neighbor_t* dvmrpNbr = L7_NULLPTR;  
  dvmrp_interface_t *interface = L7_NULLPTR;
  L7_uchar8 grp[DVMRP_MAX_DBG_ADDR_SIZE];  
  L7_uchar8              maskLen;    


  /* Get the ASB Node for the Group.
   */
  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);          

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }


  if ((asbNode = dvmrpAdminScopeNodeGet (dvmrpcb, grpAddr, grpMask))
      == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:Admin Scope Node doesn't Exist  \n",__FUNCTION__);

    return L7_FAILURE;
  }

  /* Verify if the Interface is SET for this Group.
   */
  if (!MCAST_BITX_TEST (asbNode->intfMask, rtrIfNum))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:Admin Scope Intf Not SET\n",__FUNCTION__);

    return L7_FAILURE;
  }

  /* Reset the Interface as Scoped for this Group.
   */
  MCAST_BITX_RESET (asbNode->intfMask, rtrIfNum);

  /* If there are no other Interfaces set for this Group,
   * Delete it.
   */
  MCAST_BITX_IS_EMPTY (asbNode->intfMask, isEmpty);
  if (isEmpty == L7_TRUE)
  {
    if (SLLDelete (&(dvmrpcb->dvmrpasbList), (L7_sll_member_t*) asbNode) != L7_SUCCESS)
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "Deleting Admin Scope Node for Grp - %s Intf - %d ",
               inetAddrPrint(grpAddr,grp), rtrIfNum);

      return L7_FAILURE;

    }
  }

  interface = &dvmrpcb->dvmrp_interfaces[rtrIfNum];

  if (interface->index != rtrIfNum)
  {
    return L7_SUCCESS;
  }

  inetMaskToMaskLen(grpMask,&maskLen);
  memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
  if (L7_SUCCESS != 
      dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d No Entry found \n",
                __FUNCTION__,__LINE__);
    return L7_SUCCESS;
  }
  while (L7_NULLPTR != entry)
  {
    dvmrpASBNode_t *asbNodeCurr = L7_NULLPTR;
    L7_uchar8 maskLen = 0;
    L7_BOOL adminScoped = L7_FALSE;

    /* Find whether the group in (S,G) entry falls in any other existing 
       admin-scope range */
    for (asbNodeCurr = (dvmrpASBNode_t *)SLLFirstGet(&(dvmrpcb->dvmrpasbList));
         asbNodeCurr != L7_NULLPTR ;
         asbNodeCurr = (dvmrpASBNode_t *)SLLNextGet(&(dvmrpcb->dvmrpasbList), (L7_sll_member_t*) asbNodeCurr))
    {
      inetMaskToMaskLen(&asbNodeCurr->grpMask,&maskLen);
      if ((inetAddrCompareAddrWithMask(&entry->group,maskLen,
                                      &asbNodeCurr->grpAddr,maskLen) == L7_SUCCESS)) 
      {
        /* Check when resetting a admin-scope boundary on an interface whether
           the interface falls  in any other admin-scope range (then reset the 
           children and dependent list) or upstream interface is in admin-scope   
           (then set that particular interface in router or children list)   */

        /* P.S : The interface in router and children list is set or reset 
                 only when admin-scope is added or completely removed  from
                 that interface, irrespective of parent interface            */

        if ((adminScoped == L7_FALSE) &&
            (MCAST_BITX_TEST(asbNodeCurr->intfMask, entry->parent->index)))
        {
          if (dvmrpMemberShipTest (dvmrpcb,&entry->group, &entry->source, 
                                   rtrIfNum) == L7_SUCCESS)
          {
            MCAST_BITX_SET(entry->children, rtrIfNum);
          }
          if (dvmrp_any_dependents_on_this_interface(&entry->data->dependents, 
                                                     interface) == L7_SUCCESS)
          {
            MCAST_BITX_SET(entry->routers, rtrIfNum);
          }
          adminScoped = L7_TRUE;
        }
        if (MCAST_BITX_TEST(asbNodeCurr->intfMask, rtrIfNum))
        {    
          MCAST_BITX_RESET(entry->children, rtrIfNum);
          MCAST_BITX_RESET(entry->routers, rtrIfNum);
          adminScoped = L7_TRUE;
          break;
        }
      }
    }
    if (adminScoped == L7_FALSE)
    {
      if (rtrIfNum ==entry->parent->index)
      {
        L7_uchar8 pOif[L7_INTF_INDICES];
        L7_BOOL isEmpty = L7_FALSE;

        MCAST_BITX_OR(entry->children, entry->routers, pOif);
        MCAST_BITX_IS_EMPTY(pOif, isEmpty);
        if (isEmpty == L7_FALSE)
        {

        if (MCAST_BITX_TEST (entry->pruneSent,
                             rtrIfNum))
        {
          dvmrpNbr = entry->data->neighbor;      
          if (dvmrp_graft_send (entry, dvmrpNbr, 0) != L7_SUCCESS)
          {
            DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nGraft Not Sent \n");
            return L7_FAILURE;
          }
        }
        }
        /* Update all */

        dvmrp_cache_mfc_update(entry);
      }
      else
      {

        if ((dvmrp_any_dependents_on_this_interface(&entry->data->dependents, 
                                                    interface) == L7_SUCCESS)||((dvmrpMemberShipTest (dvmrpcb,&entry->group, &entry->source, 
                                                                                                      rtrIfNum) == L7_SUCCESS)))
        {
          dvmrp_update_fn_call(DVMRP_EVENT_ADMIN_SCOPE_DELETE, entry, 
                               rtrIfNum);
        }

      }
    }
    memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
    entry = L7_NULLPTR;
    if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d No more Entry found \n",
                  __FUNCTION__,__LINE__);
      DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d No more Entry found \n",
                  __FUNCTION__,__LINE__);
      break;
    }
  }


  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);          

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Handler for Admin Scope Boundary Events
*
* @param    dvmrpcb           -  @b{ (input) } Pointer to the DVMRP Ctrl Block
*                 asbEventType   -@b{ (input) } Admin Scope Event ID
*                 asbInfo            - @b{ (input) } Admin Scope Boundary Event Info 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dvmrpAdminScopeBoundaryEventHandler (dvmrp_t *dvmrpcb,
                                     mcastAdminMsgInfo_t *asbInfo)
{
  L7_RC_t retVal = L7_FAILURE;
  L7_uint32 rtrIfNum;  

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);          

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return L7_FAILURE;
  }

  if (asbInfo == L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : asbInfo  is NULL!\n\n");
    return L7_FAILURE;

  }

  if (mcastIpMapIntIfNumToRtrIntf(dvmrpcb->family,asbInfo->intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nIfNum to RtrIntf conversion failed \n\n");
    return L7_FAILURE;
  }


  switch (asbInfo->mode)
  {
    case L7_ENABLE:
      {
        DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:MCAST_EVENT_ADMINSCOPE_BOUNDARY ADD\n",__FUNCTION__);                

        retVal = dvmrpAdminScopeBoundarySet (dvmrpcb,
                                             &(asbInfo->groupAddress),
                                             &(asbInfo->groupMask),
                                             rtrIfNum);
        break;
      }

    case L7_DISABLE:
      {
        DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:MCAST_EVENT_ADMINSCOPE_BOUNDARY DELETE \n",__FUNCTION__);                      

        retVal = dvmrpAdminScopeBoundaryReset (dvmrpcb,
                                               &(asbInfo->groupAddress),
                                               &(asbInfo->groupMask),
                                               rtrIfNum);
        break;
      }

    default:
      DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:Invalid Admin Scope Mode - %d received \n",__FUNCTION__,asbInfo->mode);                            
  }

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:LEAVING  \n",__FUNCTION__);          
  return retVal;
}
/*********************************************************************
*
* @purpose  Apply the boundary scope information
*
* @param    dvmrpcb   -   @b{ (input) } Pointer to the DVMRP Ctrl Block
*                 entry        -   @b{ (input) } Pointer to the cache entry  
*
*
* @notes
*
* @end
*********************************************************************/
void
dvmrpApplyAdminScope(dvmrp_t *dvmrpcb,dvmrp_cache_entry_t *entry)

{
  dvmrpASBNode_t *asbNode = L7_NULLPTR;
  L7_uint32 rtrIfIndex; 
  dvmrp_interface_t *interface;
  dvmrp_neighbor_t *dvmrpNbr=L7_NULLPTR;
  L7_uchar8              maskLen;      

  DVMRP_DEBUG(DVMRP_DEBUG_EVENTS,"%s:ENTERED  \n",__FUNCTION__);          

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb is NULL!\n\n");
    return;
  }

  if ((asbNode = (dvmrpASBNode_t*) SLLFirstGet (&(dvmrpcb->dvmrpasbList))) == L7_NULLPTR)
  {

    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,
                "%s:Admin Scope Node doesn't Exist  \n",__FUNCTION__);

    return;

  }

  while (asbNode != L7_NULLPTR)
  {

    inetMaskToMaskLen(&asbNode->grpMask,&maskLen);
    if (inetAddrCompareAddrWithMask(&entry->group,maskLen,&asbNode->grpAddr,maskLen) ==L7_SUCCESS)
    {
      for (rtrIfIndex=0;rtrIfIndex<MAX_INTERFACES;rtrIfIndex++)
      {
        if ((MCAST_BITX_TEST(dvmrpCB->interface_mask, rtrIfIndex))&&
            (MCAST_BITX_TEST (asbNode->intfMask,rtrIfIndex)))
        {
          interface =&dvmrpcb->dvmrp_interfaces[rtrIfIndex];
          if (interface->index == rtrIfIndex)
          {
            if (rtrIfIndex ==entry->parent->index)
            {
              if (!MCAST_BITX_TEST (entry->pruneSent,
                                    rtrIfIndex))
              {
                dvmrpNbr = entry->data->neighbor;

                MCAST_BITX_SET(entry->pruneSent, 
                               entry->parent->index);
                if (dvmrp_prune_send (dvmrpNbr->interface, &entry->source, 
                                      &entry->group, DVMRP_AVERAGE_PRUNE_LIFETIME, 
                                      dvmrpNbr) != L7_SUCCESS)
                {
                  DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nPrune Not Sent \n");
                  return;
                }
              }
            }
            else
            {
              if (MCAST_BITX_TEST (entry->children,
                                   rtrIfIndex) ||MCAST_BITX_TEST (entry->routers,
                                                                  rtrIfIndex))
              {

                dvmrp_update_fn_call(DVMRP_EVENT_MFC_DEL_MEMBER, entry, 
                                     rtrIfIndex);
              }
            }
          }
        }
      }
    }
    asbNode  = (dvmrpASBNode_t*) SLLNextGet (&(dvmrpcb->dvmrpasbList),(L7_sll_member_t*) asbNode );  
  }
}


