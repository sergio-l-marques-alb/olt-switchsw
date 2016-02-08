/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   dtl_l3_ipv4_mcast.c
*
* @purpose    Dtl layer3 functions for multicast forwarding
*
* @component  Device Transformation Layer
*
* @comments   none
*
* @create     05/31/2002
*
* @author
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "l3_comm_structs.h"
#include "comm_mask.h"
#include "dtlinclude.h"
#include "dtl_l3_api.h"
#include "dtl_l3_mcast_api.h"
#include "l7_ip_api.h"
#include "dtl_ip.h"
#include "nimapi.h"
#include "usmdb_ip_api.h"
#include "usmdb_sim_api.h"

/*********************************************************************
* @purpose  Enables or disables the ability to forward multicast
* @purpose  traffic for this particular router interface.
*
* @param    mode     @b{(input)}
*           L7_ENABLE, to enable forwarding of multicast traffic.
*           L7_DISABLE, to disable forwarding of multicast traffic.
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlRouterMulticastForwardModeSet(L7_uint32 mode,L7_uint32 family)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;
  L7_BOOL     enable;

  static const char *routine_name = "dtlRouterMulticastForwardModeSet()";

  DTL_IP_TRACE("%s %d: %s : mode =  %s\n",
                 __FILE__, __LINE__, routine_name,
                 mode?"enable":"disable");

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.mcastforwardConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.mcastforwardConfig.enable = enable;
  dapiCmd.cmdData.mcastforwardConfig.family = family;

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG, &dapiCmd);

  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
* @purpose  Enables or disables IGMP admin mode
*
* @param    mode     @b{(input)}
*           L7_ENABLE, to enable IGMP admin mode.
*           L7_DISABLE, to disable IGMP admin mode.
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlRouterMulticastIgmpModeSet(L7_uint32 mode, L7_uint32 family)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;
  L7_BOOL     enable;

  static const char *routine_name = "dtlRouterMulticastIgmpModeSet()";

  DTL_IP_TRACE("%s %d: %s : mode =  %s\n",
                 __FILE__, __LINE__, routine_name,
                 mode?"enable":"disable");

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.mcastIgmpConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.mcastIgmpConfig.enable = enable;
  dapiCmd.cmdData.mcastIgmpConfig.family = family;

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG, &dapiCmd);

  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}


/*********************************************************************
* @purpose  Adds or updates a Multicast Forwarding entry in the
* @purpose  Multicast Forwarding List in the network processor.
* @purpose  If the entry is new, the driver will add the forwarding
* @purpose  entry, if it was previously added, the driver updates
* @purpose  the entry (e.g. change to outgoing interfaces).
*
* @param    *pMcastForwEntry  @b{(input)}
*           A pointer to the Multicast Entry that contains all the
*           necessary information needed to add the entry to the
*           Multicast Forwarding list .
*
* @returns  L7_SUCCESS  on successful addition, or if driver has
*                       not added entry due to non-support of options
*                       requested
* @returns  L7_FAILURE  if the addition failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlMulticastAddrAdd(L7_multicastForwList_t *pMcastForwEntry)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t   uspIn;
  nimUSP_t   uspOut;
  DAPI_USP_t ddusp, ddPhysUsp;
  DAPI_USP_t ddoutusp;
  L7_uint32  intIfNum;
  L7_uint32  vid;
  L7_uint32  highestIntfNumber;
  L7_uint32  intIfType;
  L7_uint32  tagMode;
  L7_uint32  mode;
  nimUSP_t   portNimUsp, physNimUsp;
  
  L7_uint32   i;
  L7_uint32   oifList[L7_MAX_INTERFACE_COUNT+1];
  L7_uint32   oifCount;
  DAPI_USP_t  oifUsp[L7_MAX_INTERFACE_COUNT+1];
  L7_uint32   nonZeroOutIntfMask;

  static const char *routine_name = "dtlMulticastAddrAdd()";

  DTL_IP_TRACE("%s %d: %s : Dest IP Addr = %x  Source IP addr = %x OutIntIfMask = ??,inIntIfNum = %d\n",
                 __FILE__, __LINE__, routine_name,
                 pMcastForwEntry->mcastGroupDestAddress.addr.ipv4.s_addr,
                 pMcastForwEntry->sourceIpAddress.addr.ipv4.s_addr,
/*                 pMcastForwEntry->outIntIfMask,    */
                 pMcastForwEntry->inIntIfNum);
  
  if (nimGetUnitSlotPort(pMcastForwEntry->inIntIfNum, &uspIn) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddusp.unit = uspIn.unit;
  ddusp.slot = uspIn.slot;
  ddusp.port = uspIn.port - 1;

  if (nimGetIntfType(pMcastForwEntry->inIntIfNum, &intIfType) != L7_SUCCESS) {
      return L7_FAILURE;
  }

  if (intIfType == L7_LOGICAL_VLAN_INTF) 
  {
      if (nimGetUnitSlotPort(pMcastForwEntry->inPhysicalIntIfNum, &physNimUsp) != L7_SUCCESS) 
          {
          return L7_FAILURE;
          }
      ddPhysUsp.unit = physNimUsp.unit;
      ddPhysUsp.slot = physNimUsp.slot;
      ddPhysUsp.port = physNimUsp.port - 1;
  } 
  else 
  {
      ddPhysUsp = ddusp;
  }
  dapiCmd.cmdData.mcastAdd.ingressPhysicalPort = ddPhysUsp;  
  dapiCmd.cmdData.mcastAdd.getOrSet          = DAPI_CMD_SET;
  inetCopy(&dapiCmd.cmdData.mcastAdd.mcastGroupAddr,
                                &pMcastForwEntry->mcastGroupDestAddress);
  dapiCmd.cmdData.mcastAdd.rpfCheckEnable    = pMcastForwEntry->reversePathCheck;
  /* specify type of reverse path check applied to this entry */
  switch (pMcastForwEntry->typeOfReversePathCheck)
  {
  case L7_MCAST_RPF_CHECK_METHOD_IIF_MATCH:
      dapiCmd.cmdData.mcastAdd.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_IIF_MATCH;
      break;
  case L7_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE:
     dapiCmd.cmdData.mcastAdd.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE;
     break;
  default:
     return L7_ERROR;
  }

  /* specify what we want done if reverse path check fails for traffic on this entry */
  switch (pMcastForwEntry->reversePathCheckFailAction)
  {
  case L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP:
      dapiCmd.cmdData.mcastAdd.rpfCheckFailAction = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_DROP;
      break;
  case L7_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU:
     dapiCmd.cmdData.mcastAdd.rpfCheckFailAction  = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU;
     break;
  default:
     return L7_ERROR;
  }
  if (inetIsInAddressAny(&pMcastForwEntry->sourceIpAddress) == L7_FALSE)
  {
     inetCopy(&dapiCmd.cmdData.mcastAdd.srcIpAddr,
                                &pMcastForwEntry->sourceIpAddress);
     dapiCmd.cmdData.mcastAdd.matchSrcAddr   = L7_TRUE ;
  }
  else
  {
     dapiCmd.cmdData.mcastAdd.matchSrcAddr   = L7_FALSE ;
  }


  L7_INTF_NONZEROMASK(pMcastForwEntry->outIntIfMask, nonZeroOutIntfMask);
  if (nonZeroOutIntfMask)
  {   
     /* if there are any outgoing interfaces, create array of USPs, one for eact out router interface */
     nimMaskToList( (NIM_INTF_MASK_t *) &(pMcastForwEntry->outIntIfMask), oifList, &oifCount);
     for(i=0;i<oifCount;i++)
     {
      if(nimGetUnitSlotPort(oifList[i], &uspOut) != L7_SUCCESS)
         return L7_FAILURE;
      oifUsp[i].unit = uspOut.unit;
      oifUsp[i].slot = uspOut.slot;
      oifUsp[i].port = uspOut.port - 1;
     }

     dapiCmd.cmdData.mcastAdd.outUspList = oifUsp;
     dapiCmd.cmdData.mcastAdd.outUspCount = oifCount;
     dapiCmd.cmdData.mcastAdd.outGoingIntfPresent = L7_TRUE;
  }
  else
  {   
     dapiCmd.cmdData.mcastAdd.outGoingIntfPresent = L7_FALSE;
  }
  
  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_ADD, &dapiCmd);

  if (dr == L7_FAILURE)
    rc = L7_FAILURE;
  else if (dr == L7_NOT_SUPPORTED) /*dtl returns this if entry options not supported on platform, skip port add */
  {
     rc = L7_SUCCESS;
  }
   else if (dr == L7_SUCCESS)
   {
      /* only need to set up egress port masks if there are one or more outgoing interfaces */
      if (dapiCmd.cmdData.mcastAdd.outGoingIntfPresent == L7_TRUE)
      {
         for (i=0;i<oifCount;i++)
         {
            /* setup outport masks for egress interface */

           /* note: unit and port number adjustments between NIM and dapi already taken care of when oifUsp[]
                    was populated above
          */
            ddoutusp.unit = oifUsp[i].unit;    
            ddoutusp.slot = oifUsp[i].slot;    
            ddoutusp.port = oifUsp[i].port;

            dapiCmd.cmdData.mcastPortAdd.getOrSet          = DAPI_CMD_SET;
            inetCopy(&dapiCmd.cmdData.mcastAdd.mcastGroupAddr,
                                &pMcastForwEntry->mcastGroupDestAddress);
            if (inetIsInAddressAny(&pMcastForwEntry->sourceIpAddress) == L7_FALSE)
            {
               dapiCmd.cmdData.mcastPortAdd.matchSrcAddr   = L7_TRUE ;
               inetCopy(&dapiCmd.cmdData.mcastAdd.srcIpAddr,
                                &pMcastForwEntry->sourceIpAddress);
            } else
            {
               dapiCmd.cmdData.mcastPortAdd.matchSrcAddr   = L7_FALSE ;
            }


            if (nimGetIntfType(oifList[i], &intIfType)!=L7_SUCCESS)
               return L7_FAILURE;
            /* make DAPI calls based on type of router interface */
            switch (intIfType)
            {
            case L7_PHYSICAL_INTF:
               /* if outgoing router port is a physical port, add that port to the mcast route */
               dapiCmd.cmdData.mcastPortAdd.outPortUsp.unit = oifUsp[i].unit;
               dapiCmd.cmdData.mcastPortAdd.outPortUsp.slot = oifUsp[i].slot;
               dapiCmd.cmdData.mcastPortAdd.outPortUsp.port = oifUsp[i].port;
               /* invoke DAPI command to add interface to outgoing port bitmask for this ip mcast route */
               dr = dapiCtl(&ddoutusp, DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD, &dapiCmd);
               break;
            case L7_LOGICAL_VLAN_INTF:
               /* if outgoing router port is a logical interface to a VLAN, add each port in the VLAN to the mcast route */
               /* determine which ports to add based on VLAN membership */
               if ((ipMapVlanRtrIntIfNumToVlanId(oifList[i], &vid) != L7_SUCCESS))
                  return L7_FAILURE;

               nimGetHighestIntfNumber(&highestIntfNumber);

               /* for each internal interface number */
               for (intIfNum=1;intIfNum<=highestIntfNumber;intIfNum++)
               {
                  /* check if it is a member of the VLAN */
                  if (dot1qOperVlanMemberGet(vid, intIfNum, &mode)==L7_SUCCESS && (mode==L7_DOT1Q_FIXED))
                  {
                     /* check that the member interface is a physical port */
                     if ((nimGetIntfType(intIfNum, &intIfType)==L7_SUCCESS) &&
                         (intIfType == L7_PHYSICAL_INTF))
                     {
                        /* handle whether port is wants tagged data or not */
                        if (dot1qVlanTaggedMemberGet(vid, intIfNum, &tagMode)==L7_SUCCESS && tagMode==L7_DOT1Q_TAGGED)
                           dapiCmd.cmdData.mcastPortAdd.vlanTagging       = L7_TRUE;
                        else
                           dapiCmd.cmdData.mcastPortAdd.vlanTagging       = L7_FALSE;
                        /* convert internal interface number to unit/slot/port */
                        if ((nimGetUnitSlotPort(intIfNum, &portNimUsp)==L7_SUCCESS))
                        {
                           dapiCmd.cmdData.mcastPortAdd.outPortUsp.unit = portNimUsp.unit;
                           dapiCmd.cmdData.mcastPortAdd.outPortUsp.slot = portNimUsp.slot;
                           dapiCmd.cmdData.mcastPortAdd.outPortUsp.port = portNimUsp.port - 1;
                           /* invoke DAPI command to add interface to outgoing port bitmask for this ip mcast route */
                           dr = dapiCtl(&ddoutusp, DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD, &dapiCmd);
                        }
                     }
                  }
               }
               break;
            default:
               return L7_FAILURE;
               break;
            }
         }
      }
      rc = L7_SUCCESS;
   }
   else
	 rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
* @purpose  Deletes the  Multicasting Forwarding Entry from
* @purpose  Multicast Forwarding List
*
* @param    *pMcastForwEntry  @b{(input)}
*           A pointer to the Multicast Entry that contains all the
*           necessary information needed to identify the Multicast
*           Forwarding entry  and delete it.
*
* @returns  L7_SUCCESS  on successful addition, or if driver has
*                       not added entry due to non-support of options
*                       requested
* @returns  L7_FAILURE  if the addition failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlMulticastAddrDelete(L7_multicastForwList_t *pMcastForwEntry)
{
   L7_RC_t rc;
   L7_RC_t dr;
   DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
   nimUSP_t nimusp;
   DAPI_USP_t ddusp;

   static const char *routine_name = "dtlMulticastAddrDelete()";

   DTL_IP_TRACE("%s %d: %s : Dest IP Addr = %x  Source IP addr = %x ,inIntIfNum = %d\n",
                  __FILE__, __LINE__, routine_name,
                  pMcastForwEntry->mcastGroupDestAddress.addr.ipv4.s_addr,
                  pMcastForwEntry->sourceIpAddress.addr.ipv4.s_addr,
                  pMcastForwEntry->inIntIfNum);

   if (nimGetUnitSlotPort(pMcastForwEntry->inIntIfNum, &nimusp) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }

   ddusp.unit = nimusp.unit;
   ddusp.slot = nimusp.slot;
   ddusp.port = nimusp.port - 1;

   dapiCmd.cmdData.mcastDelete.getOrSet       = DAPI_CMD_SET;
   inetCopy(&dapiCmd.cmdData.mcastDelete.mcastGroupAddr,
                                            &pMcastForwEntry->mcastGroupDestAddress);
   dapiCmd.cmdData.mcastDelete.rpfCheckEnable    = pMcastForwEntry->reversePathCheck;

   /* specify type of reverse path check applied to this entry */
   switch (pMcastForwEntry->typeOfReversePathCheck)
   {
   case L7_MCAST_RPF_CHECK_METHOD_IIF_MATCH:
      dapiCmd.cmdData.mcastDelete.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_IIF_MATCH;
      break;
   case L7_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE:
      dapiCmd.cmdData.mcastDelete.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE;
      break;
   default:
      return L7_ERROR;
   }

   /* specify what we want done if reverse path check fails for traffic on this entry */
   switch (pMcastForwEntry->reversePathCheckFailAction)
   {
   case L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP:
      dapiCmd.cmdData.mcastDelete.rpfCheckFailAction = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_DROP;
      break;
   case L7_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU:
      dapiCmd.cmdData.mcastDelete.rpfCheckFailAction  = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU;
      break;
   default:
      return L7_ERROR;
   }
   if (inetIsInAddressAny(&pMcastForwEntry->sourceIpAddress) == L7_FALSE)
   {
      dapiCmd.cmdData.mcastDelete.matchSrcAddr   = L7_TRUE;
      inetCopy(&dapiCmd.cmdData.mcastDelete.srcIpAddr,&pMcastForwEntry->sourceIpAddress);
   } else
   {
      dapiCmd.cmdData.mcastDelete.matchSrcAddr   = L7_FALSE ;
   }

   dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_DELETE, &dapiCmd);

   if (dr == L7_SUCCESS)
   {
      rc = L7_SUCCESS;
   } else if (dr == L7_NOT_SUPPORTED)
   {
      rc = L7_SUCCESS;
   } else
   {
      rc = L7_FAILURE;
   }

   return rc;
}

/*********************************************************************
* @purpose  Request counters for the  Multicasting Forwarding Entry from
* @purpose  the driver
*
* @param    *pMcastForwEntryCntQuery  @b{(input)}
*           A pointer to a structure that contains all the
*           necessary information needed to identify the Multicast
*           Forwarding entry.  This structure also has member fields
*           inwhich to return count data.
*
* @returns  L7_SUCCESS  on successful addition, or if driver has
*                       not added entry due to non-support of options
*                       requested
* @returns  L7_FAILURE  if the addition failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlMulticastSGCountersGet(L7_multicastForwEntryCounterQuery_t *pMcastForwEntryCntQuery)
{
   L7_RC_t rc;
   L7_RC_t dr;
   DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
   nimUSP_t nimusp;
   DAPI_USP_t ddusp;

   static const char *routine_name = "dtlMulticastSGCountersGet()";

   DTL_IP_TRACE("%s %d: %s : Dest IP Addr = %x  Source IP addr = %x ,inIntIfNum = %d\n",
                  __FILE__, __LINE__, routine_name,
                  pMcastForwEntryCntQuery->mcastGroupDestAddress.addr.ipv4.s_addr,
                  pMcastForwEntryCntQuery->sourceIpAddress.addr.ipv4.s_addr,
                  pMcastForwEntryCntQuery->inIntIfNum);

   if (nimGetUnitSlotPort(pMcastForwEntryCntQuery->inIntIfNum, &nimusp) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }

   ddusp.unit = nimusp.unit;
   ddusp.slot = nimusp.slot;
   ddusp.port = nimusp.port - 1;

   dapiCmd.cmdData.mcastCountQuery.getOrSet       = DAPI_CMD_GET;
   inetCopy(&dapiCmd.cmdData.mcastCountQuery.mcastGroupAddr,
                                        &pMcastForwEntryCntQuery->mcastGroupDestAddress);

   dapiCmd.cmdData.mcastCountQuery.rpfCheckEnable    = pMcastForwEntryCntQuery->reversePathCheck;

   /* specify type of reverse path check applied to this entry */
   switch (pMcastForwEntryCntQuery->typeOfReversePathCheck)
   {
   case L7_MCAST_RPF_CHECK_METHOD_IIF_MATCH:
      dapiCmd.cmdData.mcastCountQuery.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_IIF_MATCH;
      break;
   case L7_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE:
      dapiCmd.cmdData.mcastCountQuery.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE;
      break;
   default:
      return L7_ERROR;
   }

   /* specify what we want done if reverse path check fails for traffic on this entry */
   switch (pMcastForwEntryCntQuery->reversePathCheckFailAction)
   {
   case L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP:
      dapiCmd.cmdData.mcastCountQuery.rpfCheckFailAction = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_DROP;
      break;
   case L7_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU:
      dapiCmd.cmdData.mcastCountQuery.rpfCheckFailAction  = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU;
      break;
   default:
      return L7_ERROR;
   }
   if (inetIsInAddressAny(&pMcastForwEntryCntQuery->sourceIpAddress) == L7_FALSE)
   {
      dapiCmd.cmdData.mcastCountQuery.matchSrcAddr   = L7_TRUE;
      inetCopy(&dapiCmd.cmdData.mcastCountQuery.srcIpAddr,
                                            &pMcastForwEntryCntQuery->sourceIpAddress);
   
   } else
   {
      dapiCmd.cmdData.mcastCountQuery.matchSrcAddr   = L7_FALSE ;
   }

   dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET, &dapiCmd);

   if (dr == L7_SUCCESS)
   {
      pMcastForwEntryCntQuery->byte_count               = dapiCmd.cmdData.mcastCountQuery.byteCount;
      pMcastForwEntryCntQuery->byte_count_supported     = dapiCmd.cmdData.mcastCountQuery.byteCountSupported;
      pMcastForwEntryCntQuery->packet_count             = dapiCmd.cmdData.mcastCountQuery.packetCount;
      pMcastForwEntryCntQuery->packet_count_supported   = dapiCmd.cmdData.mcastCountQuery.packetCountSupported;
      pMcastForwEntryCntQuery->wrong_if_count           = dapiCmd.cmdData.mcastCountQuery.wrongIntfCount;
      pMcastForwEntryCntQuery->wrong_if_count_supported = dapiCmd.cmdData.mcastCountQuery.wrongIntfCountSupported;
      rc = L7_SUCCESS;
   } 
   else
   {
      rc = L7_FAILURE;
   }

   return rc;
}

/*********************************************************************
* @purpose  Request use status for the  Multicasting Forwarding Entry from
* @purpose  the driver.  This function is to be called ONLY by the application
* @purpose  function that is aging Multicast Forwarding Entries.  The usage
* @purpose  state for the entry is reset as a side effect of calling this
* @purpose  function.  Calling this function can cause use data to be missed
* @purpose  by the aging function and cause a entry to be aged out incorrectly.
*
* @param    *pMcastForwEntryUseQuery  @b{(input)}
*           A pointer to a structure that contains all the
*           necessary information needed to identify the Multicast
*           Forwarding entry.  This structure also has member fields
*           inwhich to return count data.
*
* @returns  L7_SUCCESS  on successful addition, or if driver has
*                       not added entry due to non-support of options
*                       requested
* @returns  L7_FAILURE  if the addition failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlMulticastSGUseGet(L7_multicastForwEntryUseQuery_t *pMcastForwEntryUseQuery)
{
   L7_RC_t rc;
   L7_RC_t dr;
   DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
   nimUSP_t nimusp;
   DAPI_USP_t ddusp;

   static const char *routine_name = "dtlMulticastSGCountersGet()";

   DTL_IP_TRACE("%s %d: %s : Dest IP Addr = %x  Source IP addr = %x ,inIntIfNum = %d\n",
                  __FILE__, __LINE__, routine_name,
                  pMcastForwEntryUseQuery->mcastGroupDestAddress.addr.ipv4.s_addr,
                  pMcastForwEntryUseQuery->sourceIpAddress.addr.ipv4.s_addr,
                  pMcastForwEntryUseQuery->inIntIfNum);

   if (nimGetUnitSlotPort(pMcastForwEntryUseQuery->inIntIfNum, &nimusp) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }

   ddusp.unit = nimusp.unit;
   ddusp.slot = nimusp.slot;
   ddusp.port = nimusp.port - 1;

   dapiCmd.cmdData.mcastUseQuery.getOrSet = DAPI_CMD_GET;
   inetCopy(&dapiCmd.cmdData.mcastUseQuery.mcastGroupAddr,
                             &pMcastForwEntryUseQuery->mcastGroupDestAddress);
   dapiCmd.cmdData.mcastUseQuery.rpfCheckEnable = pMcastForwEntryUseQuery->reversePathCheck;

   /* specify type of reverse path check applied to this entry */
   switch (pMcastForwEntryUseQuery->typeOfReversePathCheck)
   {
   case L7_MCAST_RPF_CHECK_METHOD_IIF_MATCH:
      dapiCmd.cmdData.mcastUseQuery.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_IIF_MATCH;
      break;
   case L7_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE:
      dapiCmd.cmdData.mcastUseQuery.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_UNICAST_TABLE;
      break;
   default:
      return L7_ERROR;
   }

   /* specify what we want done if reverse path check fails for traffic on this entry */
   switch (pMcastForwEntryUseQuery->reversePathCheckFailAction)
   {
   case L7_MCAST_RPF_CHECK_FAIL_ACTION_DROP:
      dapiCmd.cmdData.mcastUseQuery.rpfCheckFailAction = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_DROP;
      break;
   case L7_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU:
      dapiCmd.cmdData.mcastUseQuery.rpfCheckFailAction  = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU;
      break;
   default:
      return L7_ERROR;
   }


   if (inetIsInAddressAny(&pMcastForwEntryUseQuery->sourceIpAddress) == L7_FALSE)
   {
      dapiCmd.cmdData.mcastUseQuery.matchSrcAddr   = L7_TRUE;
      inetCopy(&dapiCmd.cmdData.mcastUseQuery.srcIpAddr,
                                    &pMcastForwEntryUseQuery->sourceIpAddress);
   } else
   {
      dapiCmd.cmdData.mcastUseQuery.matchSrcAddr   = L7_FALSE ;
   }

   dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_USE_GET, &dapiCmd);

   if (dr == L7_SUCCESS)
   {
      pMcastForwEntryUseQuery->entry_used = dapiCmd.cmdData.mcastUseQuery.entryUsed;
      rc = L7_SUCCESS;
   } 
   else
   {
      rc = L7_FAILURE;
   }

   return rc;
}



/*********************************************************************
* @purpose  Sets the Multicast TTL scoping value for a router interface.
*
* @param    inIntIfNum    @b{(input)} The  internal interface for which the
*                                     TTL scooping has to be applied .
* @param    mcastTtlValue @b{(input)} The TTL value to be applied to this
*                                     router interface
*
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlMulticastTTLScopingSet(L7_uint32 inIntIfNum, L7_uint32 mcastTtlValue)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;

  static const char *routine_name = "dtlMulticastTTLScopingSet()";

  DTL_IP_TRACE("%s %d: %s : ttlValue =  %x \n",
                 __FILE__, __LINE__, routine_name,
                 mcastTtlValue);


  if (nimGetUnitSlotPort(inIntIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  dapiCmd.cmdData.ttlMcastVal.getOrSet    = DAPI_CMD_SET;
  dapiCmd.cmdData.ttlMcastVal.ttlVal      = mcastTtlValue;


  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET, &dapiCmd);

  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
* @purpose  Updates port information in the IP Multicast Forwarding tables
* @purpose  regarding port vlan membership, tagging (and in the future group membership).
* @purpose  It is expected that this will be invoked in response to notification
* @purpose  events from the dot1q and mfdb components.
*
* @param    vid             @b{(input)} VLAN ID for which port data is to be updated
* @param    portIntIfNum    @b{(input)} internal interface number of the physical port
*                           whose VLAN parameters are to be updated (VLAN membership,
*                           tagging, group membership).
*
*
*
* @returns  L7_SUCCESS  on successful update
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlMcastVlanPortMemberUpdate(L7_uint32 vid, L7_uint32 portIntIfNum)
{
  L7_RC_t              rc;
  L7_RC_t        dr;
  DAPI_USP_t           ddRtrUsp;
  nimUSP_t             rtrNimUsp;
  nimUSP_t             portNimUsp;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;

  L7_uint32            intIfNum;
  L7_uint32            portIntIfType;
  L7_uint32            mode;
  L7_uint32            tagMode;

  static const char   *routine_name = "dtlMcastVlanPortMemberUpdate()";

  DTL_IP_TRACE("%s %d: %s : vid = %d, portIntIfNum = %d \n",
                 __FILE__, __LINE__, routine_name,
                 vid, portIntIfNum);

  /* convert vid to a DAPI usp for vlan's router interface  */
  if ((rc = usmDbIpVlanRtrVlanIdToIntIfNum(usmDbThisUnitGet(),vid, 
                                           &intIfNum)) != L7_SUCCESS)
     return rc;

  if ((rc = nimGetUnitSlotPort(intIfNum, &rtrNimUsp)) != L7_SUCCESS)
     return rc;

  ddRtrUsp.unit = rtrNimUsp.unit;
  ddRtrUsp.slot = rtrNimUsp.slot;
  ddRtrUsp.port = rtrNimUsp.port - 1;


  /* check that the interface number specified is a physical port */
  if ((nimGetIntfType(portIntIfNum, &portIntIfType)==L7_SUCCESS) &&
      (portIntIfType == L7_PHYSICAL_INTF))
  {
     /* set parameter to add or delete port */
     if (dot1qOperVlanMemberGet(vid, portIntIfNum, &mode)==L7_SUCCESS && (mode==L7_DOT1Q_FIXED))
        dapiCmd.cmdData.mcastVlanPortMemberUpdate.bIsMember = L7_TRUE;
     else
        dapiCmd.cmdData.mcastVlanPortMemberUpdate.bIsMember = L7_FALSE;

     /* handle whether port is wants tagged data or not */
     if (dot1qVlanTaggedMemberGet(vid, portIntIfNum, &tagMode)==L7_SUCCESS && tagMode==L7_DOT1Q_TAGGED)
         dapiCmd.cmdData.mcastVlanPortMemberUpdate.vlanTagging       = L7_TRUE;
     else
         dapiCmd.cmdData.mcastVlanPortMemberUpdate.vlanTagging       = L7_FALSE;

     /* convert port's internal interface number to a DAPI usp  */
     if ((rc = nimGetUnitSlotPort(portIntIfNum, &portNimUsp)) != L7_SUCCESS)
     {
        return rc;
     }
     dapiCmd.cmdData.mcastVlanPortMemberUpdate.outPortUsp.unit = portNimUsp.unit;
     dapiCmd.cmdData.mcastVlanPortMemberUpdate.outPortUsp.slot = portNimUsp.slot;
     dapiCmd.cmdData.mcastVlanPortMemberUpdate.outPortUsp.port = portNimUsp.port - 1;

     dr = dapiCtl(&ddRtrUsp, DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE, &dapiCmd);

     if (dr == L7_SUCCESS)
      rc = L7_SUCCESS;
     else
      rc = L7_FAILURE;
  }
  return rc;
}

