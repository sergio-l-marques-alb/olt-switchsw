/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   dtl_l3.c
*
* @purpose    Dtl layer3 functions
*
* @component  Device Transformation Layer
*
* @comments   none
*
* @create     03/18/2000
*
* @author     wjacobs
* @author     asuthan
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#include "l3_comm_structs.h"
#include "dtlinclude.h"
#include "dtl_l3_api.h"
#include "l7_ip_api.h"
#include "dtl_ip.h"
#include "l7_ipmap_arp_api.h"

/* internal function prototypes */
static L7_RC_t dtlIpv4RouteEntryExtract(L7_routeEntry_t *pRoute,
                                        DAPI_ROUTING_ROUTE_ENTRY_t *pRouteEntry);
static DAPI_ROUTING_ROUTE_FLAG_t dtlIpv4RouteToDapiFlags(L7_routeEntry_t *pRoute);
static DAPI_ROUTING_ARP_FLAG_t dtlIpv4ArpToDapiFlags(L7_uint32 arpFlags);


extern dtlInfo_t         dtlInfo;

/* Begin Function Declarations:  dtl_l3_ipv4_api.h */

/*********************************************************************
* @purpose  Enables or disables the routing function on a whole, i.e.
* @purpose  whether this entity has the ability to route or not
*
* @param    mode     @b{(input)} L7_ENABLE, to enable forwarding of PDUs in this router
*                                L7_DISABLE, to disable forwarding in this router
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes    Forwarding - The indication of whether this entity is acting as an
* @notes    IP gateway in respect to the forwarding of PDUs received by, but not
* @notes    addressed to, this entity.  IP gateways forward PDUs.  IP hosts do
* @notes    not (except those source-routed via the host).
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ForwardingModeSet(L7_uint32 mode)
{
  L7_RC_t rc;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  L7_BOOL     enable;
  DAPI_USP_t ddusp;
  L7_RC_t dr;


  DTL_IP_TRACE("%s : mode =  %s\n",
                 __FUNCTION__, mode?"enable":"disable");

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.forwardingConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.forwardingConfig.enable = enable;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG, &dapiCmd);
  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;


}


/*********************************************************************
* @purpose  Enables or disables the ability to forward based on TOS
* @purpose  (Type Of Service) for the router
*
* @param    mode  @b{(input)} L7_ENABLE, to enable TOS based Forwarding in this router.
*                             L7_DISABLE, to disable TOS based forwarding in this router.
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4TOSForwardingModeSet(L7_uint32 mode)
{


  DTL_IP_TRACE("%s : mode =  %s\n",
                 __FUNCTION__,
                 mode?"enable":"disable");

  /* Function is not needed for current LVL7 implementations - L7_NOT_SUPPORTED */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the ability of the routing function to fragment PDUs
*
* @param    mode     @b{(input)} as defined by L7_RT_FRAGMENTATION_MODE_t
*                                {L7_RT_FRAGMENT_DISCARD,
*                                L7_RT_FRAGMENT_SEND_TO_CPU,
*                                L7_RT_FRAGMENT_SEND_TO_2ND_RTR}
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4FragmentationModeSet(L7_uint32 mode)
{
#if 0
  L7_RC_t rc;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;

  DAPI_USP_t ddusp;
  L7_RC_t dr;

  DTL_IP_TRACE("%s : mode =  %s, \n",
                 __FUNCTION__,
                 mode?"enable":"disable");


  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;



  dapiCmd.cmdData.fragmentConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.fragmentConfig.cfg = mode;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_FRAGMENT_CONFIG , &dapiCmd);
  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
#else
  /* colinw: The above code has been disabled since the driver doesn't support
             the operation */
  return L7_SUCCESS;
#endif
}

/*********************************************************************
* @purpose  Configures the router whether to forward BOOTP/DHCP requests
* @purpose  through router or not.
*
* @param     @b{(input)} L7_ENABLE, to enabling forwarding of BOOTP/DHCP requests.
*                        L7_DISABLE, to disable forwarding of BOOTP/DHCP requests
* @param     @b{(input)} The IP address of the BOOTP/DHCP server
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes    Forwarding BOOTP and DHCP frames is an exception to the specification
* @notes    that Layer2 broadcasts and multicasts packets should not be forwarded
* @notes    through the router.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4BootpDhcpRelaySet(L7_uint32 mode, L7_IP_ADDR_t ipAddr)
{
  L7_RC_t rc;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  L7_BOOL     enable;
  DAPI_USP_t ddusp;
  L7_RC_t dr;


  DTL_IP_TRACE("%s : mode =  %s, server %x\n",
                 __FUNCTION__,
                 mode?"enable":"disable", ipAddr);

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.bootpDhcpConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.bootpDhcpConfig.enable = enable;
  dapiCmd.cmdData.bootpDhcpConfig.bootpDhcpRelayServer = (L7_ulong32)ipAddr;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG , &dapiCmd);
  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}



/*********************************************************************
* @purpose  Enables or disables the ability of the router to perform
* @purpose  address checking of the source address
*
* @param    mode     @b{(input)} L7_ENABLE, to enable checking of source address in this router.
*                                L7_DISABLE, to disable checking of source address in this router
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes    Typically checking is done to isolate address beginning with
* @notes    127.x.x.x, 224.x.x.x, all ones and all zeros
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4SourceAddressCheckingModeSet(L7_uint32 mode)
{
#if 0
  L7_RC_t rc;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  L7_BOOL     enable;
  DAPI_USP_t ddusp;
  L7_RC_t dr;

  static const char *routine_name = "dtlIpv4SourceAddressCheckingModeSet()";

  DTL_IP_TRACE("%s : mode =  %s\n",
                 __FUNCTION__,
                 mode?"enable":"disable");


  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.sourceIPCheck.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.sourceIPCheck.enable = enable;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_SOURCE_IP_CHECK , &dapiCmd);
  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
#else
  /* colinw: The above code has been disabled since the driver doesn't support
             the operation */
  return L7_SUCCESS;
#endif
}

/*********************************************************************
* @purpose  Enables or disables IP Spoofing check
*
* @mode    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4SpoofingModeSet(L7_uint32 mode)
{
#if 0
  L7_RC_t rc;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  L7_BOOL     enable;
  DAPI_USP_t ddusp;
  L7_RC_t dr;


  DTL_IP_TRACE("%s : mode =  %s, \n",
                 __FUNCTION__,
                 mode?"enable":"disable");

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.spoofingCheck.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.spoofingCheck.enable = enable;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_SPOOFING_CHECK , &dapiCmd);
  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
#else
  /* colinw: The above code has been disabled since the driver doesn't support
             the operation */
  return L7_SUCCESS;
#endif
}

/*********************************************************************
* @purpose  Configure handling of IP Spoofing Errors
*
* @mode    mode     @b{(input)} defined by L7_RT_NP_SPOOFING_HANDLER_MODE_t
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes   This configuration is valid only if spoofing is enabled.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4SpoofingHandlingSet(L7_uint32 mode)
{

  static const char *routine_name = "dtlIpv4SpoofingHandlingSet()";

  DTL_IP_TRACE("%s : mode =  %s, \n",
                 routine_name,
                 mode?"enable":"disable");

  /* Function is not needed for current LVL7 implementations */
  return(L7_NOT_SUPPORTED);
}

/*********************************************************************
* @purpose  Enables or disables the ability of the router to send ICMP Redirect
* @purpose  error when it receives a PDU that should have been to a different router.
*
* @param    mode     @b{(input)} L7_ENABLE, to enable ICMP Redirect in this router.
*                                L7_DISABLE, to disable ICMP Redirect in this router
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ICMPRedirectModeSet(L7_uint32 mode)
{
  L7_RC_t rc;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  L7_BOOL     enable;
  DAPI_USP_t ddusp;
  L7_RC_t dr;

  static const char *routine_name = "dtlIpv4ICMPRedirectModeSet()";

  DTL_IP_TRACE("%s : mode =  %s, \n", routine_name, mode?"enable":"disable");

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.icmpRedirectsConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.icmpRedirectsConfig.enable = enable;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG , &dapiCmd);
  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
* @purpose  Get IPv4 circuit configuration data
*
* @param    ipCircDesc @b{(input)} interface parameters
* @param    pDapiCmd   @b{(input)} Pointer to a DAPI_ROUTING_INTF_MGMT_CMD_t structure
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4RoutingIntfModifyCmdBuild(dtlRtrIntfDesc_t *ipCircDesc,
                                  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd)
{
  DAPI_USP_t ddusp;
  nimUSP_t nimusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(ipCircDesc->ipCircuit.intIfNum, L7_SYSNAME, ifName);

  if (nimGetUnitSlotPort(ipCircDesc->ipCircuit.intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  /*  Build the command structure */
  memset(dapiCmd,0,sizeof(DAPI_ROUTING_INTF_MGMT_CMD_t));
  dapiCmd->cmdData.rtrIntfModify.getOrSet = DAPI_CMD_SET;

  if (ipCircDesc->flags & DTL_IP_CIRC_BROADCAST_CAPABLE)
      dapiCmd->cmdData.rtrIntfModify.broadcastCapable = L7_TRUE;

  if (ipCircDesc->llEncapsType == L7_ENCAP_ETHERNET)
    dapiCmd->cmdData.rtrIntfModify.encapsType = DAPI_ROUTING_INTF_ENCAPS_TYPE_ETHERNET;
  else
    dapiCmd->cmdData.rtrIntfModify.encapsType = DAPI_ROUTING_INTF_ENCAPS_TYPE_SNAP;

  dapiCmd->cmdData.rtrIntfModify.ipAddr = ipCircDesc->ipAddress;
  dapiCmd->cmdData.rtrIntfModify.ipMask = ipCircDesc->subnetMask;

  /* Get the Mac address associated with the internal interface number */
  nimGetIntfL3MacAddress(ipCircDesc->ipCircuit.intIfNum, 0,
                      (L7_uchar8 *)&(dapiCmd->cmdData.rtrIntfModify.macAddr));
  dapiCmd->cmdData.rtrIntfModify.mtu = ipCircDesc->mtu;
  dapiCmd->cmdData.rtrIntfModify.vlanID = ipCircDesc->ipCircuit.vlanId;

  DTL_IP_TRACE("%s : intf = %u, %s, vlanID = %d \n",
                 __FUNCTION__,
                 ipCircDesc->ipCircuit.intIfNum, ifName, ipCircDesc->ipCircuit.vlanId);

  DTL_IP_TRACE("%s : IP Address= %x, Mask = %x, MTU = %d, encaps = %s, "
                 "flags =  %x\n",
                 __FUNCTION__,
                 ipCircDesc->ipAddress, ipCircDesc->subnetMask, ipCircDesc->mtu,
                 (ipCircDesc->llEncapsType == L7_ENCAP_ETHERNET) ? "enet" : "snap",
                 ipCircDesc->flags);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get IPv4 circuit configuration data
*
* @param    ipCircDesc @b{(input)} interface parameters
* @param    pDapiCmd   @b{(input)} Pointer to a DAPI_ROUTING_INTF_MGMT_CMD_t structure
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4AddrCmdBuild(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                            L7_IP_MASK_t ipMask,
                            DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd)
{
  DAPI_USP_t ddusp;
  nimUSP_t nimusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (nimGetUnitSlotPort(intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  /*  Build the command structure */
  memset(dapiCmd,0,sizeof(DAPI_ROUTING_INTF_MGMT_CMD_t));
  dapiCmd->cmdData.rtrIntfModify.getOrSet = DAPI_CMD_SET;
  dapiCmd->cmdData.rtrIntfModify.ipAddr = ipAddr;
  dapiCmd->cmdData.rtrIntfModify.ipMask = ipMask;

  DTL_IP_TRACE("%s : intf = %u, %s IP Address= %x, Mask = %x\n",
                 __FUNCTION__, intIfNum, ifName, ipAddr, ipMask);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Modify parameters of a routing interface
*
* @param    ipCircDesc @b{(input)} new parameters for the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Some of these parameters are not IPv4 specific (i.e., they also
*           apply to IPv6). We assign them here as a group because the
*           driver requires we pass all these values in the command.
*
*           Sets these parameters:
*             -- L2 encapsulation type
*             -- interface MTU
*             -- whether IPv4 routing is enabled
*             -- whether forwarding of net directed broadcasts is enabled
*             -- whether multicast forwarding is enabled
*             -- whether the interface is a broadcast interface
*
*           Does not set IPv4 interface addresses.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4RtrIntfModify(dtlRtrIntfDesc_t *ipCircDesc)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;
  nimUSP_t nimusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(ipCircDesc->ipCircuit.intIfNum, L7_SYSNAME, ifName);

  DTL_IP_TRACE("%s. intf %u, %s, vlan %u, encaps %s, mtu %u, flags %#x\n",
                 __FUNCTION__,
                 ipCircDesc->ipCircuit.intIfNum, ifName, ipCircDesc->ipCircuit.vlanId,
                 (ipCircDesc->llEncapsType == L7_ENCAP_ETHERNET) ? "enet" : "snap",
                 ipCircDesc->mtu, ipCircDesc->flags);

  if (nimGetUnitSlotPort(ipCircDesc->ipCircuit.intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  if (dtlIpv4RoutingIntfModifyCmdBuild(ipCircDesc, &dapiCmd) == L7_SUCCESS)
  {
    dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MODIFY, &dapiCmd);
    if (dr == L7_SUCCESS)
      rc = L7_SUCCESS;
    else
      rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Sets an IP address for a particular router interface
*
* @param    intIfNum @b{(input)} The internal interface number for which the IP address is assigned.
* @param    ipAddr   @b{(input)} The IP address to be assigned
* @param    ipMask   @b{(input)} The subnet mask associated with the IP address
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
*
* @notes    This function sets the IP address for all external interfaces
*           and for the  system
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4IntfIPAddrSet(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                             L7_IP_MASK_t ipMask)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;
  nimUSP_t nimusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (nimGetUnitSlotPort(intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  if (dtlIpv4AddrCmdBuild(intIfNum, ipAddr, ipMask, &dapiCmd) == L7_SUCCESS)
  {
    dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET, &dapiCmd);

    DTL_IP_TRACE("%s : intf = %u, %s, IP Addr =  %x, IP Mask = %x  \n",
                   __FUNCTION__,
                   intIfNum, ifName,  ipAddr,  ipMask);

    if (dr == L7_SUCCESS)
      rc = L7_SUCCESS;
    else
      rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Add an IPv4 address to a particular router interface
*
* @param    intIfNum @b{(input)} The internal interface number for
* @param                         which the IP address is assigned.
* @param    ipAddr   @b{(input)} The IP address to be assigned
* @param    ipMask   @b{(input)} The subnet mask associated with the
* @param                         IP address
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
*
* @notes    This function adds an IP address on the interface.  It is
*           up to the hardware how to interpret this, it may be able to
*           add multiple interfaces into tables on the chip or it may
*           need to create routes or filters for the chip to use to
*           receive packets on this address.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4IntfAddrAdd(L7_uint32 intIfNum,
                           L7_IP_ADDR_t ipAddr,
                           L7_IP_MASK_t ipMask)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Delete an IPv4 address from a particular router interface
*
* @param    intIfNum @b{(input)} The internal interface number for
* @param                         which the IP address is assigned.
* @param    ipAddr   @b{(input)} The IP address to be assigned
* @param    ipMask   @b{(input)} The subnet mask associated with the
* @param                         IP address
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
*
* @notes    This function deletes an IP address from the interface.
*           It is up to the hardware how to interpret this.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4IntfAddrDel(L7_uint32 intIfNum,
                           L7_IP_ADDR_t ipAddr,
                           L7_IP_MASK_t ipMask)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Enable or disable network directed broadcasts on the router interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
*
* @returns  L7_NOT_SUPPORTED
*
* @notes    This is not needed at the for LVL7 NP implementations.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4NetDirectedBcastModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  return L7_NOT_SUPPORTED;
}


/*********************************************************************
* @purpose  Enables or disables the ability to forward multicast traffic for
* @purpose  this particular router interface
*
* @param    intIfNum @b{(input)} The internal interface number for this router interface
* @param    mode     @b{(input)} L7_ENABLE, to enable forwarding of multicast traffic in this router interface.
*                                L7_DISABLE, to disable forwarding of multicast traffic in this router interface.
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4MulticastIntfForwardModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  L7_BOOL     enable;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  DTL_IP_TRACE("%s : mode =  %s, intf %u, %s\n",
                 __FUNCTION__, mode?"enable":"disable", intIfNum, ifName);



  if (nimGetUnitSlotPort(intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.mcastIntfFwdConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.mcastIntfFwdConfig.enable = enable;

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG, &dapiCmd);

  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}


/*********************************************************************
* @purpose  Adds an IP address to the Local Multicast List associated
* @purpose  with this particular router interface
*
* @param    intIfNum @b{(input)} The internal interface number for this router interface
* @param    ipAddress@b{(input)} The IP address to be added to this list
*
*
* @returns  L7_SUCCESS  on successful addition
* @returns  L7_FAILURE  if the addition failed
*
* @notes    The Local Multicast List contains the list Multicast IP addresses
*           that this interface will interact with, respond to, etc.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4LocalMulticastAddrAdd(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);


  static const char *routine_name = "dtlIpv4LocalMulticastAddrAdd()";

  DTL_IP_TRACE("%s : intf = %u, %s, IP Addr = %x  \n",
                 routine_name,
                 intIfNum, ifName, ipAddress);

  if (nimGetUnitSlotPort(intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  dapiCmd.cmdData.localMcastAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.localMcastAdd.ipAddr = (L7_ulong32) ipAddress;

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD, &dapiCmd);

  if (dr != L7_SUCCESS)
    rc = L7_FAILURE;
  else
  {
    rc = L7_SUCCESS;
    dtlInfo.localMcastAddrEntryCount++;
  }

  return rc;
}



/*********************************************************************
* @purpose  Deletes an IP address from the Local Multicast List
* @purpose  associated with this particular router interface
*
* @param    intIfNum @b{(input)} The internal interface number for this router interface
* @param    ipAddress@b{(input)} The IP address to be deleted from this list
*
*
* @returns  L7_SUCCESS  on successful deletion
* @returns  L7_FAILURE  if the deletion failed or the entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4LocalMulticastAddrDelete(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  static const char *routine_name = "dtlIpv4LocalMulticastAddrDelete()";

  DTL_IP_TRACE("%s : intf = %u, %s, IP Addr = %x  \n",
                 routine_name,
                 intIfNum, ifName, ipAddress);

  if (nimGetUnitSlotPort(intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  dapiCmd.cmdData.localMcastDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.localMcastDelete.ipAddr = (L7_ulong32)ipAddress;

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE, &dapiCmd);

  if (dr != L7_SUCCESS)
    rc = L7_FAILURE;
  else
  {
    rc = L7_SUCCESS;
    if (dtlInfo.localMcastAddrEntryCount > 0)
      dtlInfo.localMcastAddrEntryCount--;
    else
    {
      DTL_IPV4_ERROR("%s %d: %s : DTL Local Mcast Addr entry count out of sync\n",
                     __FILE__, __LINE__, routine_name);
    }
  }

  return rc;
}


/*********************************************************************
* @purpose  Adds an ARP entry in the ARP table maintained in the
* @purpose  Forwarding Information Base of the network-processing device.
*
* @param    *pArp    @b{(input)} A reference to the ARP entry object
*                    containing information needed to add an entry to the ARP table.
*
*
* @returns  L7_SUCCESS  on a successful addition of the ARP entry
* @returns  L7_FAILURE  if the addition failed
*
* @notes    ARP table entries correlate an IP address to a MAC address and
*           Router Interface. The table is built as a result of static entries
*           and information obtained from ARP requests and replies. Dynamic
*           entries are periodically aged.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ArpEntryAdd(L7_arpEntry_t *pArp)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(pArp->intIfNum, L7_SYSNAME, ifName);


  static const char *routine_name = "dtlIpv4ArpEntryAdd()";

  DTL_IP_TRACE("%s : IP Addr = %x, intf = %u, %s, flags = %x, vlan = %d" \
                 " mac addr = %d:%d:%d:%d:%d:%d \n",
                 routine_name,
                 pArp->ipAddr, pArp->intIfNum, ifName, pArp->flags,pArp->vlanId,
                 pArp->macAddr.addr.enetAddr.addr[0],
                 pArp->macAddr.addr.enetAddr.addr[1],
                 pArp->macAddr.addr.enetAddr.addr[2],
                 pArp->macAddr.addr.enetAddr.addr[3],
                 pArp->macAddr.addr.enetAddr.addr[4],
                 pArp->macAddr.addr.enetAddr.addr[5]);

  /* an interface number of 0 is used for "system" ARP entries, such
   * as the routing protocol multicast IP addresses (these are typically
   * static ARP entries), so don't call NIP in this case
   */
  if (pArp->intIfNum == 0)
  {
    ddusp.unit = -1;
    ddusp.slot = -1;
    ddusp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(pArp->intIfNum, &nimusp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    ddusp.unit = nimusp.unit;
    ddusp.slot = nimusp.slot;
    ddusp.port = nimusp.port - 1;
  }

  dapiCmd.cmdData.arpAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.arpAdd.ipAddr = pArp->ipAddr;
  dapiCmd.cmdData.arpAdd.vlanID = pArp->vlanId;
  dapiCmd.cmdData.arpAdd.flags = dtlIpv4ArpToDapiFlags(pArp->flags);

  memcpy(&(dapiCmd.cmdData.arpAdd.macAddr),&(pArp->macAddr.addr.enetAddr),L7_MAC_ADDR_LEN);

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ARP_ENTRY_ADD,&dapiCmd);

  if (dr != L7_SUCCESS)
    rc = L7_FAILURE;
  else
  {
    rc = L7_SUCCESS;
  }

  return rc;
}



/*********************************************************************
* @purpose  Modifies an existing ARP entry in the ARP table maintained
* @purpose  in the Forwarding Information Base of the network-processing device.
*
* @param    *pArp    @b{(input)} A reference to the ARP entry object,
*                                which contains modified parameters.
*
*
* @returns  L7_SUCCESS  on a successful modification of the ARP entry
* @returns  L7_FAILURE  if the modification failed or the entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ArpEntryModify(L7_arpEntry_t *pArp)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(pArp->intIfNum, L7_SYSNAME, ifName);

  static const char *routine_name = "dtlIpv4ArpEntryModify()";

  DTL_IP_TRACE("%s : IP Addr = %x, intf = %u, %s, flags = %x, vlan = %d" \
                 " mac addr = %d:%d:%d:%d:%d:%d \n",
                 routine_name,
                 pArp->ipAddr, pArp->intIfNum, ifName, pArp->flags,pArp->vlanId,
                 pArp->macAddr.addr.enetAddr.addr[0],
                 pArp->macAddr.addr.enetAddr.addr[1],
                 pArp->macAddr.addr.enetAddr.addr[2],
                 pArp->macAddr.addr.enetAddr.addr[3],
                 pArp->macAddr.addr.enetAddr.addr[4],
                 pArp->macAddr.addr.enetAddr.addr[5]);

  /* an interface number of 0 is used for "system" ARP entries, such
   * as the routing protocol multicast IP addresses (these are typically
   * static ARP entries), so don't call NIP in this case
   */
  if (pArp->intIfNum == 0)
  {
    ddusp.unit = -1;
    ddusp.slot = -1;
    ddusp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(pArp->intIfNum, &nimusp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    ddusp.unit = nimusp.unit;
    ddusp.slot = nimusp.slot;
    ddusp.port = nimusp.port - 1;
  }

  dapiCmd.cmdData.arpModify.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.arpModify.ipAddr = pArp->ipAddr;
  dapiCmd.cmdData.arpModify.vlanID = pArp->vlanId;
  dapiCmd.cmdData.arpModify.flags = dtlIpv4ArpToDapiFlags(pArp->flags);

  memcpy(&(dapiCmd.cmdData.arpModify.macAddr),&(pArp->macAddr.addr.enetAddr),L7_MAC_ADDR_LEN);

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY,&dapiCmd);

  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}



/*********************************************************************
* @purpose  Deletes an ARP entry in the ARP table maintained in the
* @purpose  Forwarding Information Base of the network-processing device.
*
* @param    ipAddress @b{(input)} The IP address associated with this ARP entry
*
*
* @returns  L7_SUCCESS  on a successful deletion of the ARP entry
* @returns  L7_FAILURE  if the deletion failed or the entry does not exist
*
* @notes    An ARP entry is uniquely identified by the IP address associated with it.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ArpEntryDelete(L7_arpEntry_t *pArp)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;

  static const char *routine_name = "dtlIpv4ArpEntryDelete()";

  DTL_IP_TRACE("%s : IP Addr =  %x,  \n",
                 routine_name,
                 pArp->ipAddr);

  /* an interface number of 0 is used for "system" ARP entries, such
   * as the routing protocol multicast IP addresses (these are typically
   * static ARP entries), so don't call NIP in this case
   */
  if (pArp->intIfNum == 0)
  {
    ddusp.unit = -1;
    ddusp.slot = -1;
    ddusp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(pArp->intIfNum, &nimusp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    ddusp.unit = nimusp.unit;
    ddusp.slot = nimusp.slot;
    ddusp.port = nimusp.port - 1;
  }


  dapiCmd.cmdData.arpDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.arpDelete.ipAddr = pArp->ipAddr;
  dapiCmd.cmdData.arpDelete.vlanID = pArp->vlanId;
  dapiCmd.cmdData.arpDelete.flags = dtlIpv4ArpToDapiFlags(pArp->flags);

  memcpy(&(dapiCmd.cmdData.arpDelete.macAddr),&(pArp->macAddr.addr.enetAddr),L7_MAC_ADDR_LEN);

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ARP_ENTRY_DELETE,&dapiCmd);

  if (dr != L7_SUCCESS)
    rc = L7_FAILURE;
  else
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Deletes all ARP entries in the ARP table maintained in the
*           Forwarding Information Base of the network-processing device.
*
* @param    none
*
*
* @returns  L7_SUCCESS  on a successful deletion of the ARP entries
* @returns  L7_FAILURE  if the deletion failed or the entries do not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dtlIpv4ArpEntryDeleteAll (void)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  DAPI_USP_t ddusp;

  static const char *routine_name = "dtlIpv4ArpEntryDeleteAll()";

  DTL_IP_TRACE("%s \n", routine_name);

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  dapiCmd.cmdData.arpDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.arpDelete.ipAddr = 0;
  dapiCmd.cmdData.arpDelete.vlanID = 0;
  dapiCmd.cmdData.arpDelete.flags = 0;

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL, &dapiCmd);

  if (dr != L7_SUCCESS)
    rc = L7_FAILURE;
  else
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the status of a ARP entry in the hardware
*
* @param    *pArp    @b{(input)} A reference to the ARP entry object
*                    containing information about hit-bit and last-hit-time
*
*
* @returns  L7_SUCCESS  on a successful addition of the ARP entry
* @returns  L7_FAILURE  if the addition failed
*
* @notes    number of entries queried for in this API is 1.
*
*
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ArpEntryQuery(L7_arpQuery_t *pArp)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  L7_uint32 lhs,lhd;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(pArp->intIfNum, L7_SYSNAME, ifName);


  static const char *routine_name = "dtlIpv4ArpEntryQuery()";

  DTL_IP_TRACE("%s %d: %s : IP Addr = %x, vlan = %d, intf = %u, %s\n",
               __FILE__, __LINE__, routine_name,
               pArp->ipAddr, pArp->vlanId, pArp->intIfNum, ifName);

  if (nimGetUnitSlotPort(pArp->intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  dapiCmd.cmdData.arpQuery.getOrSet        = DAPI_CMD_GET;
  dapiCmd.cmdData.arpQuery.numOfEntries    = 1;
  dapiCmd.cmdData.arpQuery.ipAddr[0]       = pArp->ipAddr;
  dapiCmd.cmdData.arpQuery.vlanID[0]       = pArp->vlanId;
  dapiCmd.cmdData.arpQuery.arpQueryFlags[0]= 0;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ARP_ENTRY_QUERY,&dapiCmd);

  if (dr != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    rc = L7_SUCCESS;

    pArp->arpQueryFlags &= ~(L7_ARP_HIT_ANY);

    if(dapiCmd.cmdData.arpQuery.arpQueryFlags[0] & DAPI_ROUTING_ARP_HIT_SOURCE)
    {
      pArp->arpQueryFlags |= L7_ARP_HIT_SOURCE;
      lhs=dapiCmd.cmdData.arpQuery.lastHitSrc[0];
    }
    else
    {
      lhs=0;
    }

    if(dapiCmd.cmdData.arpQuery.arpQueryFlags[0] & DAPI_ROUTING_ARP_HIT_DEST)
    {
      pArp->arpQueryFlags |= L7_ARP_HIT_DEST;
      lhd=dapiCmd.cmdData.arpQuery.lastHitDst[0];
    }
    else
    {
      lhd=0;
    }

    if(pArp->arpQueryFlags & L7_ARP_HIT_ANY)
    {
      pArp->lastHitTime = (lhs>lhd?lhs:lhd);
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Callback function from the driver for ARP work.
*
* @param    *ddusp         @b{(input)} Device driver reference to unit slot and port
* @param      family         @b{(input)} Device Driver family type
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    event          @b{(input)} Event indication of type DAPI_EVENT_t
* @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ArpEntryCallback(DAPI_USP_t *ddusp,
                                DAPI_FAMILY_t family,
                                DAPI_CMD_t cmd,
                                DAPI_EVENT_t event,
                                void *dapiEventInfo)
{
  DAPI_ROUTING_ARP_CMD_t *dapiRsp = (DAPI_ROUTING_ARP_CMD_t *)dapiEventInfo;
  L7_arpEntry_t arpEntry;
  nimUSP_t      nimUsp;

  static const char *routine_name = "dtlIpv4ArpEntryQueryCallback()";


  /* this function only for ARP Mgmt family */
  if (family != DAPI_FAMILY_ROUTING_ARP_MGMT)
    return L7_FAILURE;

  /* Check for driver-initiated request to resolve an ARP entry.  This
   * entry may have been issued previously, but the driver was unable
   * to store it in the hardware at that time.
   */
  if ((event == DAPI_EVENT_ROUTING_ARP_ENTRY_QUERY) &&
      (cmd   == DAPI_CMD_ROUTING_ARP_UNSOLICITED_EVENT))
  {
    DTL_IP_TRACE("%s : ddusp = %d.%d.%d, ip=0x%8.8lx\n",
                   routine_name,
                   ddusp->unit, ddusp->slot, ddusp->port,
                   dapiRsp->cmdData.unsolArpResolve.ipAddr);

    if (dapiRsp->cmdData.unsolArpResolve.ipAddr == (L7_ulong32)L7_NULL_IP_ADDR)
      return L7_FAILURE;

    memset(&arpEntry, 0, sizeof(L7_arpEntry_t));
    arpEntry.ipAddr = (L7_IP_ADDR_t)dapiRsp->cmdData.unsolArpResolve.ipAddr;
    nimUsp.unit = ddusp->unit;
    nimUsp.slot = ddusp->slot;
    nimUsp.port = ddusp->port;
    if ( nimGetIntIfNumFromUSP(&nimUsp, &arpEntry.intIfNum) != L7_SUCCESS)
    {
      DTL_IP_TRACE("%s : Invalid interface ddusp = %d.%d.%d \n",
                   routine_name,
                   ddusp->unit, ddusp->slot, ddusp->port );
      return L7_FAILURE;
    }


    if (ipMapArpEntryGet(&arpEntry) == L7_SUCCESS) /* entry was found */
    {
      memcpy(dapiRsp->cmdData.unsolArpResolve.macAddr.addr, arpEntry.macAddr.addr.enetAddr.addr,
             (size_t)L7_ENET_MAC_ADDR_LEN);
      dapiRsp->cmdData.unsolArpResolve.vlanID = arpEntry.vlanId;
      dapiRsp->cmdData.unsolArpResolve.flags = DAPI_ROUTING_ARP_DEST_KNOWN;
    }
    else
    {
      dapiRsp->cmdData.unsolArpResolve.flags = DAPI_ROUTING_ARP_UNKNOWN;
    }

    return L7_SUCCESS;

  } /* endif ARP unsolicited event */
  else if ((event == DAPI_EVENT_ROUTING_ARP_ENTRY_COLLISION) &&
           (cmd   == DAPI_CMD_ROUTING_ARP_UNSOLICITED_EVENT))
  {
    if (ipMapArpCollisionProcess() == L7_SUCCESS)
      return L7_SUCCESS;
  }

  /* any other usage of this function is not supported */
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Gets ARP entry in the ARP table maintained in the
* @purpose  Application ARP table.
*
* @param    pArp->ipAddr @b{(input)} The ip address of the requested
*                        arp entry.
* @param    *pArp        @b{(output)} A reference to the ARP entry object
*                        containing information needed to add an entry to
*                        the ARP table.
*
*
* @returns  L7_SUCCESS  on a successful addition of the ARP entry
* @returns  L7_FAILURE  if the addition failed
* @returns  L7_ERROR    invalid parameter received.
*
* @notes    ARP table entries correlate an IP address to a MAC address and
*           Router Interface. The table is built as a result of static entries
*           and information obtained from ARP requests and replies. Dynamic
*           entries are periodically aged.
*
* @notes    The ARP information is obtained from the APPLICATION ARP TABLE
*           and NOT the DRIVER & BELOW as is normally the case with dtl
*           API.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ArpEntryGets(L7_arpEntry_t *pArp)
{
  /* If pArp is  null ptr or a null ip address was provided return an error */
  if((pArp == L7_NULLPTR) || (pArp->ipAddr == L7_NULL_IP_ADDR) || pArp->intIfNum == L7_INVALID_INTF)
  {
    DTL_IP_TRACE("dtlIpv4ArpEntryGets : Invalid request \n" );
    return L7_ERROR;
  }

  return (ipMapArpEntryGet(pArp));
}


/*********************************************************************
* @purpose  Adds a route to the Routing Table maintained in the
* @purpose  Forwarding Information Base in the network-processing device.
*
* @param    *pRoute  @b{(input)} A reference to the route entry object
*                                that contains all the necessary information
*                                needed for a route entry.
*
*
* @returns  L7_SUCCESS  on successful addition
* @returns  L7_FAILURE  if the operation failed
*
* @notes    Route table entries correlate a route to a router interface or
* @notes    network interface. The routes kept in the table originate from static
* @notes    configuration and routing protocols, such as RIP, OSPF, and BGP.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4RouteEntryAdd(L7_routeEntry_t *pRoute)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_ROUTING_ROUTE_ENTRY_t *pEcmpRoutes;
  DAPI_USP_t ddusp;

  DTL_IP_TRACE("dtlIpv4RouteEntryAdd - IP Addr = %#x, Mask = %#x\n",
                 pRoute->ipAddr, pRoute->subnetMask);

  osapiSemaTake(dtlInfo.routeEntrySem, L7_WAIT_FOREVER);

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;


  memset(&dapiCmd,0,sizeof(dapiCmd));

  pEcmpRoutes = &dapiCmd.cmdData.routeAdd.route;

  if (dtlIpv4RouteEntryExtract(pRoute,pEcmpRoutes) != L7_SUCCESS)
  {
    osapiSemaGive(dtlInfo.routeEntrySem);
    return L7_FAILURE;
  }

  dapiCmd.cmdData.routeAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.routeAdd.ipAddr = pRoute->ipAddr;
  dapiCmd.cmdData.routeAdd.ipMask = pRoute->subnetMask;

  dapiCmd.cmdData.routeAdd.flags = dtlIpv4RouteToDapiFlags(pRoute);
  dapiCmd.cmdData.routeAdd.count = 0;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD,&dapiCmd);


  if (dr == L7_SUCCESS)
  {
      dtlInfo.routeEntryCount++;
      rc = L7_SUCCESS;
  }
  else if (dr == L7_ALREADY_CONFIGURED)
  {
    dtlInfo.routeEntryCount++;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dtlInfo.routeEntrySem);
  return rc;
}



/*********************************************************************
* @purpose  Modifies a route in the routing table maintained in the
* @purpose  Forwarding Information Base in the network-processing device.
*
* @param    *pRoute  @b{(input)} A reference to the route entry object
*                                that contains all the necessary information
*                                needed to identify a route and modify it.
*
*
* @returns  L7_SUCCESS  on successful modification
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4RouteEntryModify(L7_routeEntry_t *pRoute)
{
    L7_RC_t rc;
    L7_RC_t dr;
    DAPI_ROUTING_MGMT_CMD_t dapiCmd;
    DAPI_ROUTING_ROUTE_ENTRY_t *pEcmpRoutes;
    DAPI_USP_t ddusp;

    DTL_IP_TRACE("dtlIpv4RouteEntryModify - IP Addr = %#x, Mask = %#x\n",
                 pRoute->ipAddr, pRoute->subnetMask);

    osapiSemaTake(dtlInfo.routeEntrySem, L7_WAIT_FOREVER);

    ddusp.unit = -1;
    ddusp.slot = -1;
    ddusp.port = -1;

    memset(&dapiCmd,0,sizeof(dapiCmd));

    pEcmpRoutes = &dapiCmd.cmdData.routeModify.route;
    if (dtlIpv4RouteEntryExtract(pRoute,pEcmpRoutes) != L7_SUCCESS)
    {
        osapiSemaGive(dtlInfo.routeEntrySem);
        return L7_FAILURE;
    }

    dapiCmd.cmdData.routeModify.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.routeModify.ipAddr = pRoute->ipAddr;
    dapiCmd.cmdData.routeModify.ipMask = pRoute->subnetMask;

    dapiCmd.cmdData.routeModify.flags = dtlIpv4RouteToDapiFlags(pRoute);
    dapiCmd.cmdData.routeModify.count = 0;

    dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY,&dapiCmd);
    if (dr == L7_SUCCESS)
        rc = L7_SUCCESS;
    else
        rc = L7_FAILURE;

    osapiSemaGive(dtlInfo.routeEntrySem);
    return rc;
}



/*********************************************************************
* @purpose  Deletes a route in the routing table maintained in the
* @purpose  Forwarding Information Base in the network-processing device.
*
* @param    *pRoute  @b{(input)} A reference to the route entry object
*                                that contains all the necessary information
*                                needed to identify a route and delete it.
*
*
* @returns  L7_SUCCESS  on successful deletion
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4RouteEntryDelete(L7_routeEntry_t *pRoute)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;

  DTL_IP_TRACE("dtlIpv4RouteEntryDelete - IP Addr = %#x, Mask = %#x\n",
                 pRoute->ipAddr, pRoute->subnetMask);

  osapiSemaTake(dtlInfo.routeEntrySem, L7_WAIT_FOREVER);

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  dapiCmd.cmdData.routeDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.routeDelete.ipAddr = pRoute->ipAddr;
  dapiCmd.cmdData.routeDelete.ipMask = pRoute->subnetMask;
  dapiCmd.cmdData.routeDelete.count = 0;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE,&dapiCmd);

  if (dr != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    dtlInfo.routeEntryCount--;
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dtlInfo.routeEntrySem);
  return rc;
}


/*********************************************************************
* @purpose  Deletes all the routes in the routing table maintained in the
*           Forwarding Information Base in the network-processing device.
*
* @param    none
*
*
* @returns  L7_SUCCESS  on successful deletion
* @returns  L7_FAILURE  if the operation failed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
dtlIpv4RouteEntryDeleteAll (void)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;

  DTL_IP_TRACE("dtlIpv4RouteEntryDeleteAll\n");

  osapiSemaTake(dtlInfo.routeEntrySem, L7_WAIT_FOREVER);

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  dapiCmd.cmdData.routeDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.routeDelete.ipAddr = 0;
  dapiCmd.cmdData.routeDelete.ipMask = 0;
  dapiCmd.cmdData.routeDelete.count = 0;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL,&dapiCmd);

  if (dr != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    dtlInfo.routeEntryCount = 0;
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dtlInfo.routeEntrySem);
  return rc;
}


/*********************************************************************
* @purpose  Gets count of subsystem route entries
*
* @param    void
*
*
* @returns  count of route entries in subsystem
*
* @notes    This routine is used for internal component synchronization.
*           It is not intended to be used for external user management
*           (i.e. SNMP, Web) purposes.
*
*
* @end
*********************************************************************/
L7_uint32 dtlIpv4RouteEntryCountGet()
{
  return(dtlInfo.routeEntryCount);
}

/* End Function Declarations */


/* Begin Internal Function Declarations */


/*********************************************************************
* @purpose  Extracts equal cost route information from a route entry.
*
* @param    *pRoute  @b{(input)} Pointer to a route entry
* @param    *pRouteEntry    @b{(output)}  TOS route information array
* @param    *pECR    @b{(output)}  Equal cost route array
* @param    *pNumValidTos  @b{(output)}  Count of valid TOS metrics
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t dtlIpv4RouteEntryExtract(L7_routeEntry_t *pRoute,
                                        DAPI_ROUTING_ROUTE_ENTRY_t *pRouteEntry)
{
  L7_uint32 i;
  nimUSP_t  nimUsp;
#if FUTURE_FUNC
  DAPI_ROUTING_ROUTE_DESTINATION_t *pEcrRow, *pEcr;
#endif
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  static const char *routine_name = "dtlIpv4RouteEntryExtract()";

  for (i=0; i < pRoute->ecmpRoutes.numOfRoutes; i++)
  {
    nimGetIntfName(pRoute->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum, L7_SYSNAME, ifName);
    DTL_IP_TRACE("%s : IP Addr = %x, Mask = %x. intf = %u, %s, vlan = %d, flags =  %x  \n",
                   routine_name,
                   pRoute->ipAddr, pRoute->subnetMask, pRoute->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum, ifName,
                   pRoute->ecmpRoutes.equalCostPath[i].arpEntry.vlanId, pRoute->flags);

    if (pRoute->ecmpRoutes.equalCostPath[i].flags & L7_RT_EQ_PATH_VALID)
    {
      pRouteEntry->equalCostRoute[i].valid = L7_TRUE;
      pRouteEntry->equalCostRoute[i].ipAddr  = pRoute->ecmpRoutes.equalCostPath[i].arpEntry.ipAddr;
      if (nimGetUnitSlotPort(pRoute->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum, &nimUsp) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pRouteEntry->equalCostRoute[i].usp.unit = (L7_ushort16)nimUsp.unit;
      pRouteEntry->equalCostRoute[i].usp.slot = (L7_ushort16)nimUsp.slot;
      pRouteEntry->equalCostRoute[i].usp.port = (L7_ushort16)nimUsp.port - 1;
    }
    else
    {
      pRouteEntry->equalCostRoute[i].valid = L7_FALSE;
      pRouteEntry->equalCostRoute[i].ipAddr  = L7_NULL_IP_ADDR;
      pRouteEntry->equalCostRoute[i].usp.unit = -1;
      pRouteEntry->equalCostRoute[i].usp.slot = -1;
      pRouteEntry->equalCostRoute[i].usp.port = -1;
    }

  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts route entry flags to their corresponding DAPI flags
*
* @param    routeFlags @b{(input)} route entry flags
*
*
* @returns  DAPI flags of type DAPI_ROUTING_ROUTE_FLAG_t
*
* @notes    This function is used internally by DTL and is not an API.
*
* @end
*********************************************************************/
static DAPI_ROUTING_ROUTE_FLAG_t dtlIpv4RouteToDapiFlags(L7_routeEntry_t *pRoute)
{
  L7_uint32 routeFlags;
  DAPI_ROUTING_ROUTE_FLAG_t dapiFlags = 0;

  routeFlags = pRoute->flags;

  if (routeFlags & L7_RTF_DEFAULT_ROUTE)
    dapiFlags |= DAPI_ROUTING_ROUTE_DEFAULT;

  if (routeFlags & L7_RTF_DISCARD)
    dapiFlags |= DAPI_ROUTING_ROUTE_DISCARD;

  /* NOTE: L7_RTF_REJECT routes get installed as
     cpu routes (due to no next hops) in order to
     generate icmp unreachable
  */

  return dapiFlags;
}



/*********************************************************************
* @purpose  Converts ARP entry flags to their corresponding DAPI flags
*
* @param    arpFlags @b{(input)} ARP entry flags
*
*
* @returns  DAPI flags of type DAPI_ROUTING_ARP_FLAG_t
*
* @notes    This function is used internally by DTL and is not an API.
*
* @end
*********************************************************************/
static DAPI_ROUTING_ARP_FLAG_t dtlIpv4ArpToDapiFlags(L7_uint32 arpFlags)
{
  DAPI_ROUTING_ARP_FLAG_t dapiFlags = 0;

  if (arpFlags & L7_ARP_LOCAL)
    dapiFlags |= DAPI_ROUTING_ARP_LOCAL;

  if (arpFlags & L7_ARP_RESOLVED)
    dapiFlags |= DAPI_ROUTING_ARP_DEST_KNOWN;

  if (arpFlags & L7_ARP_NET_DIR_BCAST)
    dapiFlags |= DAPI_ROUTING_ARP_NET_DIR_BCAST;

  if (arpFlags & L7_ARP_UNNUMBERED)
    dapiFlags |= DAPI_ROUTING_ARP_UNNUMBERED;

  return dapiFlags;
}


/*********************************************************************
* @purpose  Add a VRRP Virtural Router ID to an interface
*
* @param    intIfNum @b{(input)} an internal interface number
* @param    vrID     @b{(input)} VRRP Virtual Router ID
* @param    ipAddr   @b((input)) VRRP IPv4 address
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
*
* @notes    This function is valid only for routing interfaces.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4VrrpVridAdd(L7_uint32 intIfNum, L7_uint32 vrID, L7_uint32 ipAddr)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  dtlRtrIntf_t ipCircuit;
  DAPI_USP_t ddusp;
  nimUSP_t nimusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  static const char *routine_name = "dtlIpv4VrrpVridAdd()";

  ipCircuit.intIfNum    = intIfNum;
  ipCircuit.vlanId      = 0;        /* FUTURE_FUNC */

  if (nimGetUnitSlotPort(ipCircuit.intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  rc = L7_FAILURE;

  memset(&dapiCmd,0,sizeof(DAPI_ROUTING_INTF_MGMT_CMD_t));
  dapiCmd.cmdData.rtrIntfVRIDAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.rtrIntfVRIDAdd.vrID   = vrID;
  dapiCmd.cmdData.rtrIntfVRIDAdd.ipAddr = ipAddr;

  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_INTF_VRID_ADD, &dapiCmd);

  DTL_IP_TRACE("%s : intf = %u, %s, vrID =  %d",
                 routine_name,
                 intIfNum, ifName,  vrID);

  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;


}


/*********************************************************************
* @purpose  Remove a VRRP Virtural Router ID from an interface
*
* @param    intIfNum @b{(input)} an internal interface number
* @param    vrID     @b{(input)} VRRP Virtual Router ID
*
* @returns  L7_SUCCESS  on successful operation
* @returns  L7_FAILURE  if the operation failed
*
*
* @notes    This function is valid only for routing interfaces.
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4VrrpVridDelete(L7_uint32 intIfNum, L7_uint32 vrID)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  dtlRtrIntf_t ipCircuit;
  DAPI_USP_t ddusp;
  nimUSP_t nimusp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  static const char *routine_name = "dtlIpv4VrrpVridDelete()";

  ipCircuit.intIfNum    = intIfNum;
  ipCircuit.vlanId      = 0;        /* FUTURE_FUNC */

  if (nimGetUnitSlotPort(ipCircuit.intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  rc = L7_FAILURE;

  memset(&dapiCmd,0,sizeof(DAPI_ROUTING_INTF_MGMT_CMD_t));
  dapiCmd.cmdData.rtrIntfVRIDDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.rtrIntfVRIDDelete.vrID   = vrID;


  dr = dapiCtl(&ddusp,DAPI_CMD_ROUTING_INTF_VRID_DELETE, &dapiCmd);

  DTL_IP_TRACE("%s : intf = %u, %s, vrID =  %d",
                 routine_name,
                 intIfNum, ifName,  vrID);

  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;


}


/* End Internal Function Declarations */
