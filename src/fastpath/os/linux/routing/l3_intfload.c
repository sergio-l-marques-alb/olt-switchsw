/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename l3_intfload.c
*
* @purpose Setup of Linux network adaptation for the router interfaces.
*
* @component Linux IPM
*
* @comments This serves as the interface between IP MAP and the IP stack.
*           This code is considered internal to IP MAP and should not call
*           any IP MAP APIs. IP MAP APIs take an IP MAP semaphore, which 
*           is assumed to already be taken when IP MAP invokes one of these 
*           functions. May call internal IP MAP functions.
*
* @create 02/08/2002
*
* @author JWL
* @end
*
**********************************************************************/

/*********************************************************************
 *                
 *********************************************************************/
#include <string.h>

#include "l3_end.h"
#include "l3end_api.h" /* need to change name of this file -- JWL */
#include "l3_intf.h"
#include "log.h"
#include "l7_ipmap_arp_api.h"
#include "nimapi.h"
#include "rto_api.h"
#include "default_cnfgr.h"
#include "l3_defaultconfig.h"
#include "l7_ip_api.h"
#include "sysapi.h"
#include "simapi.h"
#include "osapi_support.h"
#include "ipstk_api.h"
#include "osapi_priv.h"
#include "ip_util.h"
#if defined(L3INTF_USE_TAP)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include "dtlapi.h"
#include "dtl_tap_monitor.h"
#endif

extern void *VrrpConfigSema;
extern L7_RC_t vrrpGetVMac(L7_uchar8 *ipAddress, L7_uchar8 *tVMAC);
extern L7_RC_t _ipMapIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32 *rtrIfNum);
extern void arpd_entry_fn(void);

#if defined(L3INTF_USE_TAP)
void l3intf_tap_send(int fd, L7_uint32 intIfNum,L7_netBufHandle handle, tapDtlInfo *info);
#endif

/* Maximum number of router interfaces */
L7_uint32 MaxRouterIf;

/* Router interface info table. Indexed on router interface number. */
L3INTF_INFO *l3intfInfo;

/* Flag indicating that router interface component has been initialized */
static L7_BOOL RouterIntfInitDone = L7_FALSE;

/* Task ID for task that processes ARP requests */
static L7_uint32 arpd_task_id = 0;


/**************************************************************************
* @purpose  Add a route
*
* @param    network      32bit destination ip address
* @param    gateway      32bit gateway ip address
* @param    netmask      32 bit net mask
*
* @notes    none
*
* @end
*************************************************************************/
void l3intfAddRoute(L7_uint32 network, L7_uint32 gateway, L7_uint32 netmask) 
{

   int    sock;
   struct rtentry  rtEntry;
 
   if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "Failed to open socket to add route to IP stack.");
      return;
   }
 
   memset(&rtEntry, 0x00, sizeof (rtEntry));
   ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_addr.s_addr = osapiHtonl(network);
   ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_family = AF_INET;
   ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_addr.s_addr = osapiHtonl(gateway);
   ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_family = AF_INET;
   ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_addr.s_addr = osapiHtonl(netmask);
   ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_family = AF_INET;

   rtEntry.rt_dev = L7_NULLPTR;
   rtEntry.rt_flags |=RTF_UP;
   
   if((network & netmask) != (gateway & netmask))
      rtEntry.rt_flags |= RTF_GATEWAY;

   if((network == 0) && (netmask ==0))
   {
       if (gateway != 0)
           rtEntry.rt_flags |= RTF_GATEWAY;
   }

   if ((ioctl(sock, SIOCADDRT, &rtEntry) < 0) && (errno != EEXIST)) 
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
             "Error adding a route in the IP stack routing table.");
      close(sock);
      return;
   }

   close(sock);
}

/**************************************************************************
* @purpose  Delete a route
*
* @param    network      32bit destination ip address
* @param    gateway      32bit gateway ip address
* @param    netmask      32 bit net mask
*
* @notes    none
*
* @end
*************************************************************************/
void l3intfDeleteRoute(L7_uint32 network, L7_uint32 gateway,
                       L7_uint32 netmask) 
{

   int    sock;
   struct rtentry  rtEntry;
 
   if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "Failed to open a socket to delete a route from the IP stack routing table.");
      return;
   }
 
   memset(&rtEntry, 0x00, sizeof (rtEntry));
   ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_addr.s_addr = osapiHtonl(network);
   ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_family = AF_INET;
   ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_addr.s_addr = osapiHtonl(gateway);
   ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_family = AF_INET;
   ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_addr.s_addr = osapiHtonl(netmask);
   ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_family = AF_INET;

   rtEntry.rt_dev = L7_NULLPTR;

   if ((ioctl(sock, SIOCDELRT, &rtEntry) < 0) && (errno != ESRCH)) 
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
             "Failed to delete a route from the IP stack routing table.");
      close(sock);
      return;
   }

   close(sock);
}

/*********************************************************************
* @purpose  Tell Linux networking stack about the new best routes.
*
* @param    routeEntry  Route information about the network that was added/
*           deleted/ modified
* @param    route_status information regarding the reason the callback was
*           issued.  Route_status events are enumerated in RTO_ROUTE_EVENT_t.
* @param    passthrough context specifier passed to callback registration
*
* @end
*********************************************************************/
void l3BestRouteReport(L7_routeEntry_t *routeEntry,
                                 L7_uint32 route_status,
                                 void *passthrough) 
{
  L7_uint32 nh;
  L7_uint32 rc;
  L7_uint32 netmask,ipAddr,gw;
  L7_uint32 routeCmd;
  L7_uint32 intIfNum;   /* outgoing interface to next hop router */

  netmask = routeEntry->subnetMask;
  ipAddr = routeEntry->ipAddr;

  /* If we are adding or deleting a default route, let ipstk do the work. If stack
   * has a default route via mgmt interface, adding one here will replace it. If
   * we are deleting, ipstk will restore default route on mgmt interface. */
  if (((route_status == RTO_ADD_ROUTE) || (route_status == RTO_DELETE_ROUTE)) &&
      (ipAddr == 0) &&
      (netmask == 0))
  {
    ipstkDefGwUpdate();
    return;
  }

  /* The route may contain multiple next hops. Each may or may not have already
   * been resolved to a MAC address. stack will only take one next hop. In 
   * selecting one, prefer a resolved next hop. */

  gw = routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.ipAddr;
  intIfNum = routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
  for (nh = 0; nh < routeEntry->ecmpRoutes.numOfRoutes; nh++)
  {
      if (routeEntry->ecmpRoutes.equalCostPath[nh].arpEntry.flags & L7_ARP_RESOLVED)
      {
          gw = routeEntry->ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr;
          intIfNum = routeEntry->ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum;
          break;
      }
  }

  /* For p2p links, OSPF adds a /32 route to the p2p neighbor. The stack needs 
   * this route on unnumbered links. The route is used to resolve the next 
   * hop of other routes via the unnumbered link. However, if the gw address
   * of the /32 route is non-zero, Interpeak treats it as a non-local route.
   * Linux rejects it.
   * Other routes may use the neighbor's address as a next hop address. Interpeak
   * drops routes whose next hop matches a non-local route.  */
  if (gw == ipAddr)
  {
    /* if gw on unnumbered link, set gw to 0 and install route. */
    /* exception to the rule about not calling IP MAP APIs. */
    if (_ipMapIntfIsUnnumbered(intIfNum))
      gw = 0;
    else
      /* gw on numbered link. No need for /32 to neighbor. */
    return;
  }

  /* Tell stack about the new route.
  */
  routeCmd = SIM_ROUTE_CHANGE_LAST;
  switch (route_status)
  {
  case RTO_ADD_ROUTE:
    routeCmd = SIM_ROUTE_ADD;
	break;
  case RTO_DELETE_ROUTE:
    routeCmd = SIM_ROUTE_DELETE;
    break;
  case RTO_CHANGE_ROUTE:
    routeCmd = SIM_ROUTE_MODIFY;
    break;
  default:
    break;
  }

  /* Call sim to check if this route can be modified in OS */
  if (routeCmd != SIM_ROUTE_CHANGE_LAST)
  {
	rc = simRouteModifyCheck(routeEntry->ipAddr, netmask,
							 routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.ipAddr,
							 routeCmd);
	if (rc != L7_SUCCESS)
	  return;
  }

  /* if its a direct connect, ignore. stack will install route. other
     option require ability to specify stack's internal ifid which we
     dont have access to without changing stack
  */
  if(routeEntry->protocol == RTO_LOCAL)
       return;

  switch (route_status)
  {
  case RTO_DELETE_ROUTE:
    /* We never give the IP stack more than one next hop to a given destination;
     * so no need to specify the outgoing interface or next hop address. */
    rc = osapiDeleteMRoute(ipAddr, netmask, 0, 0);
    break;

    /* If we are adding, make sure there is not already a route to 
     * this destination. Maintain our invariant that the IP stack
     * routing table always contains a single next hop for each
     * destination prefix. In delete, don't give next hop address
     * or outgoing interface, since these are likely to be different on the
     * existing route than on the new route. */
  case RTO_ADD_ROUTE:
  case RTO_CHANGE_ROUTE:
    rc = osapiDeleteMRoute(ipAddr, netmask, 0, 0);
    rc = osapiAddMRoute(ipAddr, netmask, gw, intIfNum, routeEntry->flags);
    break;
  default:
    break;
  }
}


#if !defined(L3INTF_USE_TAP)
/*********************************************************************
* @purpose  Add the VRRP Ip address
*
* @param    intIfnum    NIM interface number for the port changing state.
*
* @notes    none
*
* @end
*********************************************************************/
void addVrrpIpAddrs(int sock, struct ifreq ifr, L7_IP_ADDR_t ipAddr,
                    L7_uint32 ipAddrCount, L3INTF_INFO *ifInfo,
                    L7_uchar8 *pVmac)
{
	L7_short16 flags;

	((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = osapiHtonl(ipAddr);

	if (ioctl(sock, SIOCSIFADDR, &ifr) == -1)
	{
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
                "Failed to Set Interface IP Address."
                " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");
	}

	((struct sockaddr_in *)&ifr.ifr_netmask)->sin_family = AF_INET;
	((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr =
		osapiHtonl(ifInfo->ip_netmask[ipAddrCount]);

	if (ioctl(sock, SIOCSIFNETMASK, &ifr) == -1)
	{
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
                "Failed to Set Interface IP NetMask."
                " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");
	}

	((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_family = AF_INET;
	((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr.s_addr =
		osapiHtonl(ipAddr | ~(ifInfo->ip_netmask[ipAddrCount]));

	if (ioctl(sock, SIOCSIFBRDADDR, &ifr) == -1)
	{
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
            "Failed to Set Interface Broadcast Address."
            " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");
	}

	/* Get the current flags */

	if (ioctl(sock, SIOCGIFFLAGS, &ifr) == -1)
	{
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
                "Failed to Retrieve Interface Flags\n");
	}

	flags = ifr.ifr_flags;

	/* If the interface is not already down, then bring it down */

	if ((ifr.ifr_flags & IFF_UP) != 0)
	{
		ifr.ifr_flags &= ~IFF_UP;

		if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1)
		{
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
                "Failed to Set Interface Flags."
                " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");
		}
	}

	/* Set the MAC address */

	ifr.ifr_hwaddr.sa_family = L7_ARPHRD_ETHER;
	memcpy(&ifr.ifr_hwaddr.sa_data, pVmac, L7_ENET_MAC_ADDR_LEN);

	if (ioctl(sock, SIOCSIFHWADDR, &ifr) == -1)
	{
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
            "Failed to Set Interface Hardware Address."
            " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");
	}

	/* Set the old flags + IFF_UP back */

	ifr.ifr_flags = (flags | IFF_UP);

	if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1)
	{
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
            "Failed to Set Interface Flags."
            " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");
	}
}

/*********************************************************************
* @purpose  Process VRRP Transition to Master
*
* @param    intIfnum    NIM interface number for the port changing state.
*
* @notes    none
*
* @end
*********************************************************************/
void l3intfVrrpToMaster(L7_uint32 intIfNum)
{

   int sock;
   struct ifreq ifr;
   L7_uint32 index;
   L3INTF_INFO *ifInfo;
   L7_uint32 infNum;
   L7_uchar8 vrID;
   vrrpRouterInfo_t *routerInfo;
   nimUSP_t usp;
   L7_uchar8 ipAddrCount, physAddrCount;
   L7_BOOL isPrimDeleted = L7_FALSE;
   L7_uchar8 ifname[IFNAMSIZ];
   L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];


   if (_ipMapIntIfNumToRtrIntf(intIfNum, &index) != L7_SUCCESS) return;
   /* ROBRICE - appears not to be called, but should not call this API. */
   if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS) return;


   if ((index < 1) || (index > MaxRouterIf)) return;

   ifInfo = &(l3intfInfo[index]);

   if (ifInfo->port_state == L3INTF_INVALID) return;

   if(L7_vrrpFirstGet(&vrID,&infNum) == L7_FAILURE) return;

   do {

        osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

        if (infNum == intIfNum) {

        routerInfo
            = (vrrpRouterInfo_t*)L7_vrrpVirtualRouterFetch(vrID, infNum);

        if ((routerInfo != L7_NULL)
         && (routerInfo->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)) {

        sock = socket(PF_INET, SOCK_DGRAM, 0);

        for ( ipAddrCount = 0; ipAddrCount < L7_L3_NUM_IP_ADDRS &&
                (isPrimDeleted == L7_FALSE); ipAddrCount++)
        {
          for (physAddrCount =0; physAddrCount < L7_L3_NUM_IP_ADDRS &&
                 (isPrimDeleted == L7_FALSE); physAddrCount++)
          {
            if (routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount] == 0)
            {
              break;
            }

            if (ipAddrList[physAddrCount].ipAddr == 0)
            {
              continue;
            }

            /* if virtual interface IP address matches physical interface,
               disable physical */

            if (ipAddrList[physAddrCount].ipAddr == routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount]) {

               osapiIfNameStringGet(intIfNum,ifname,IFNAMSIZ);
               /* Check if primary address matches*/
               if (physAddrCount != 0)
               {
                 sprintf(ifr.ifr_name, "%s:%d", ifname, physAddrCount);
               }
               else
               {
                 isPrimDeleted = L7_TRUE;
               }

               ((struct sockaddr_in *)&ifr.ifr_addr)->sin_family = AF_INET;
               ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = 0;

               /* since the IP addr we are trying to set is 0.0.0.0 we are most
                  likely trying to just delete the interface, so don't worry
                  if it's already deleted */
               if ((ioctl(sock, SIOCSIFADDR, &ifr) == -1) && (errno != ENODEV)) {

                  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
                      "Failed to Set Interface IP Address."
                      " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");

               }
            }
          }
        }

        if(nimGetUnitSlotPort(intIfNum,&usp) != L7_SUCCESS)
        {
           L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
                   "Failed to retrieve parent interface usp\n");
        }

        for ( ipAddrCount = 0; ipAddrCount < L7_L3_NUM_IP_ADDRS; ipAddrCount++)
        {
          if (routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount] == 0)
          {
            continue;
          }

          /* set IP address for VRRP net instance */
          if ( ipAddrCount == 0)
          {
            /* Create primary interface */
            sprintf (ifr.ifr_name, "%s%d_%d_%d", L3INTF_VRRP_DEVICE_NAME,
                     usp.slot,usp.port - 1,
                     routerInfo->vrrpCfgInfo.vrid);
          }
          else
          {
            sprintf (ifr.ifr_name, "%s%d_%d_%d:%d", L3INTF_VRRP_DEVICE_NAME,
                     usp.slot,usp.port - 1,
                     routerInfo->vrrpCfgInfo.vrid, ipAddrCount);
          }

          addVrrpIpAddrs(sock, ifr, routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount],
						 ipAddrCount, ifInfo, routerInfo->vrrpVirtRouterOperInfo.vmac);

        }

        if ( isPrimDeleted == L7_TRUE)
        {
          /* Add secondary IP addresses */
          for ( physAddrCount = 1; physAddrCount < L7_L3_NUM_IP_ADDRS; physAddrCount++)
          {
            if (ipAddrList[physAddrCount].ipAddr == 0)
            {
              continue;
            }

            /* set IP address for VRRP net instance */
            sprintf (ifr.ifr_name, "%s%d_%d_%d:%d", L3INTF_VRRP_DEVICE_NAME,
                     usp.slot,usp.port - 1,
                     routerInfo->vrrpCfgInfo.vrid, physAddrCount + L7_L3_NUM_IP_ADDRS);

            addVrrpIpAddrs(sock, ifr, ipAddrList[physAddrCount].ipAddr, ipAddrCount,
						   ifInfo, routerInfo->vrrpVirtRouterOperInfo.vmac);
          }
        }
        close(sock);
      }
    }
    osapiSemaGive(VrrpConfigSema);

  } while (L7_vrrpNextGet(vrID,infNum,&vrID,&infNum)
         == L7_SUCCESS);

}

/*********************************************************************
* @purpose  Process VRRP Transition from Master
*
* @param    intIfnum    NIM interface number for the port changing state.
*
* @notes    none
*
* @end
*********************************************************************/
void l3intfVrrpFromMaster(L7_uint32 intIfNum)
{

   int sock;
   struct ifreq ifr;
   L7_uint32 index;
   L3INTF_INFO *ifInfo;
   L7_uint32 infNum;
   L7_uchar8 vrID;
   vrrpRouterInfo_t *routerInfo;
   nimUSP_t usp;
   int ipAddrCount;
   L7_uchar8 physAddrCount;
   L7_uchar8 ifname[20];
   L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
   L7_uchar8 mac[L7_MAC_ADDR_LEN];

   if (_ipMapIntIfNumToRtrIntf(intIfNum, &index) != L7_SUCCESS) return;

   if ((index < 1) || (index > MaxRouterIf)) return;

   ifInfo = &(l3intfInfo[index]);

   if (ifInfo->port_state == L3INTF_INVALID) return;

   if(L7_vrrpFirstGet(&vrID,&infNum) == L7_FAILURE) return;
   /* ROBRICE - ditto above */
   if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS) return;

   do {

      osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

      if (infNum == intIfNum) {

         routerInfo
            = (vrrpRouterInfo_t*)L7_vrrpVirtualRouterFetch(vrID, infNum);

         if ((routerInfo != L7_NULL)
          && ((routerInfo->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_BACKUP)
           || (routerInfo->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_INIT))) {

            sock = socket(PF_INET, SOCK_DGRAM, 0);

            /* if virtual interface IP address matches physical interface,
               enable physical */

            if (ifInfo->ip_addr[0] == routerInfo->vrrpCfgInfo.ipaddress[0]) {

               osapiIfNameStringGet(intIfNum,ifr.ifr_name,IFNAMSIZ);

               ((struct sockaddr_in *)&ifr.ifr_addr)->sin_family = AF_INET;
               ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr
                  = osapiHtonl(ifInfo->ip_addr[0]);

               if (ioctl(sock, SIOCSIFADDR, &ifr) == -1) {

                  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
                      "Failed to Set Interface IP Address."
                      " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");

               }

            }

            /* only do this if we are moving to backup, if we are deleting the
               interface then this call will fail because its not needed. */
            if (routerInfo->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_BACKUP)
            {
              osapiIfNameStringGet(intIfNum,ifname,IFNAMSIZ);
              /* set IP address for VRRP net instance to zero */
              if(nimGetUnitSlotPort(intIfNum,&usp) != L7_SUCCESS)
              {
                 L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
                         "Failed to retrieve parent interface usp\n");
              }

              sprintf (ifr.ifr_name, "%s%d_%d_%d", L3INTF_VRRP_DEVICE_NAME,
                       usp.slot, usp.port - 1,
                       routerInfo->vrrpCfgInfo.vrid);

              ((struct sockaddr_in *)&ifr.ifr_addr)->sin_family = AF_INET;
              ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = 0;

              /* since the IP addr we are trying to set is 0.0.0.0 we are most
                  likely trying to just delete the interface, so don't worry
                  if it's already deleted */
              if ((ioctl(sock, SIOCSIFADDR, &ifr) == -1) && (errno != ENODEV))
              {

                 L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
                     "Failed to Set Interface IP Address."
                     " Trouble adding VRRP IP or MAC address(es) to a Linux network interface.");
              }


              /* Get the mac address */
              nimGetIntfL3MacAddress(intIfNum, 0, mac);
              /* Add secondary IP interfaces */


              for ( ipAddrCount = 1; ipAddrCount < L7_L3_NUM_IP_ADDRS; ipAddrCount++)
              {
                for (physAddrCount = 1; physAddrCount < L7_L3_NUM_IP_ADDRS; physAddrCount++)
                {
                  if (routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount] == 0)
                  {
                    break;
                  }

                  if (ipAddrList[physAddrCount].ipAddr == 0)
                  {
                    continue;
                  }


                  if (ipAddrList[physAddrCount].ipAddr == routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount])
                  {
                    /* Add the IP */
                    sprintf (ifr.ifr_name, "%s:%d", ifname, ipAddrCount);
                    addVrrpIpAddrs(sock, ifr, ipAddrList[physAddrCount].ipAddr,
								   ipAddrCount, ifInfo, mac);

                  }
                }
              }
            }
            close(sock);

         }

      }

      osapiSemaGive(VrrpConfigSema);

   } while (L7_vrrpNextGet(vrID,infNum,&vrID,&infNum)
         == L7_SUCCESS);
}
#endif

#if !defined(L3INTF_USE_TAP)
/*********************************************************************
* @purpose  Filter out packets destined to the system IP address
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_DISCARD  if frame has been discarded; should not be processed further
* @returns  SYSNET_PDU_RC_IGNORED  if frame has been ignored; continue processing it
*
* @notes    This function is registered with sysnet during initialization of the IP
* @notes    mapping layer on Linux.  A packet is discarded here if it is destined
* @notes    to the system IP address.  The packet was sent to the local IP stack by
* @notes    the kernel and will still be processed.
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t l3intfIpIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
  L7_char8 *data;
  L7_ipHeader_t *ipHdr;

  /* If the packet is destined to the system IP address, discard it since the kernel
  ** has already sent it to the local IP stack.
  */
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  (L7_uchar8 *)ipHdr = data + sysNetDataOffsetGet(data);
  if ( osapiNtohl(ipHdr->iph_dst) == simGetSystemIPAddr() )
    return SYSNET_PDU_RC_DISCARD;

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Filter out ARP requests to local interfaces
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if frame has been "consumed"; should not be processed further
* @returns  SYSNET_PDU_RC_IGNORED   if frame has been ignored; continue processing it
*
* @notes    In Linux, all ARP requests targetted to IP addresses of local interfaces
* @notes    and Virtual Router interfaces are handled by the kernel.  We need to
* @notes    pretend these frames are consumed here so they will not be processed any
* @notes    further.  The buf handle will be freed as we return back up the call stack.
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t l3intfArpIntercept(L7_uint32 hookId,
                                   L7_netBufHandle bufHandle,
                                   sysnet_pdu_info_t *pduInfo,
                                   L7_FUNCPTR_t continueFunc)
{
  L7_char8 *data;
  L7_ether_arp_t *arpHdr;
  L7_arpEntry_t arpEntry;
  L7_uint32 targetIpAddr;
  L7_uchar8 vMac[L7_MAC_ADDR_LEN];

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  (L7_uchar8 *)arpHdr = data + sysNetDataOffsetGet(data);
	if (arpHdr->arp_op  == osapiHtonl(L7_ARPOP_REQUEST))
  {
    /* Catch all ARP requests targetted to local interfaces */
    targetIpAddr = (L7_uint32)osapiNtohl(*(L7_ulong32 *)arpHdr->arp_tpa);
    if ( (ipMapIpArpEntryGet(targetIpAddr, &arpEntry) == L7_SUCCESS) &&
         ( (arpEntry.flags & L7_ARP_LOCAL) == L7_ARP_LOCAL) )
      return SYSNET_PDU_RC_CONSUMED;

    /* Catch all ARP requests targetted to Virtual Router IP addresses */
    if (vrrpGetVMac(arpHdr->arp_tpa, vMac) == L7_SUCCESS)
      return SYSNET_PDU_RC_CONSUMED;
  }

  return SYSNET_PDU_RC_IGNORED;
}
#endif

/*********************************************************************
* @purpose  Do phase 1 initialization actions for the LAYER3 component.
*
* @param    max_interfaces    Maximum number of router interfaces.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_ERROR  If not successful
*
* @notes    For simplicity, the instance for each end driver MUST
* @notes    correspond to the associated instance for each internal
* @notes    router interface number (rtrIfNum) inside the IP Mapping
* @notes    layer. However, this number (end driver instance, rtrIfNum)
* @notes    has meaning only within the associated component.  The 
* @notes    system-wide internal interface number is used in all 
* @notes    external APIs. Thus the IP Mapping APIs ipMapRtrIntfToIntIfNum() 
* @notes    and ipMapIntIfNumToRtrIntf() can be used to toggle between the 
* @notes    driver instance and the system-wide intIfNum instance. 
*       
* @end
*********************************************************************/
L7_RC_t 
ipmRouterIfInitPhase1Process(L7_uint32 max_interfaces)
{
    L7_uint32 i;
    L3INTF_INFO *ifInfo;

    if (RouterIntfInitDone == L7_TRUE) 
    {
        return L7_ERROR;
    }

    /* Allocate memory for router control blocks. */
    l3intfInfo = malloc(sizeof(L3INTF_INFO) * (max_interfaces + 1));
    if (l3intfInfo == L7_NULLPTR) 
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
               "Failed to Create Interface."
               " Out of memory at system initialization time.");
    }   

    MaxRouterIf = max_interfaces;

    /* Initialize interface info. */
    for (i = 0; i <= max_interfaces; i++) 
    {
        ifInfo = &(l3intfInfo[i]);
        memset(ifInfo, 0x00, sizeof(L3INTF_INFO));
        ifInfo->port_state = L3INTF_INVALID;
    }

    RouterIntfInitDone = L7_TRUE;

    return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  De-initialize router drivers. Phase 1
*
* @param    max_interfaces    Maximum number of router interfaces.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_ERROR  If not successful
*
* @notes   
*       
* @end
*********************************************************************/
L7_RC_t 
ipmRouterIfFiniPhase1Process(L7_uint32 max_interfaces)
{
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Do phase 2 initialization actions for the LAYER3 component.
*
* @param    max_interfaces    Maximum number of router interfaces.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_ERROR  If not successful
*
* @end
*********************************************************************/
L7_RC_t 
ipmRouterIfInitPhase2Process(L7_uint32 max_interfaces)
{
#if !defined(L3INTF_USE_TAP)
    sysnetPduIntercept_t sysnetPduIntercept;
#endif
    
#if !defined(L3INTF_USE_TAP)
    /* Register with sysnet */
    sysnetPduIntercept.addressFamily = AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_IN;
    sysnetPduIntercept.hookPrecedence = L7_SYSNET_HOOK_PRECEDENCE_0;
    sysnetPduIntercept.interceptFunc = l3intfIpIntercept;
    strcpy(sysnetPduIntercept.interceptFuncName, "l3intfIpIntercept");
    rc = sysNetPduInterceptRegister(&sysnetPduIntercept);
    if (rc != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
                "Failed to register with Sysnet PDU Intercept (AF_INET/SYSNET_INET_IN)\n");
    }
    sysnetPduIntercept.addressFamily = AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_ARP_IN;
    sysnetPduIntercept.hookPrecedence = L7_SYSNET_HOOK_PRECEDENCE_1;
    sysnetPduIntercept.interceptFunc = l3intfArpIntercept;
    strcpy(sysnetPduIntercept.interceptFuncName, "l3intfArpIntercept");
    rc = sysNetPduInterceptRegister(&sysnetPduIntercept);
    if (rc != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
                "Failed to register with Sysnet PDU Intercept (AF_INET/SYSNET_INET_ARP_IN)\n");
    }
#else
    /* +1 is for management dtl0 interface */
    tap_monitor_init(MaxRouterIf+1);
#endif

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  De-initialize router drivers. Phase 2
*
* @param    void
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_ERROR  If not successful
*
*       
* @end
*********************************************************************/
L7_RC_t ipmRouterIfFiniPhase2Process(void)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Associate a router interface with an IP address and MAC address.
*
* @param    index       Router interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
* @param    ip          IP address used for the interface
* @param    netmask     Network mask.
* @param    unnumbered  L7_TRUE if interface is unnumbered
* @param    mac         Interface MAC address
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    If the interface is unnumbered, ip is the IP address on 
*           the corresponding numbered interface and netmask is also
*           for the numbered interface. We may choose to use a netmask
*           of 0xFFFFFFFF in the IP stack for unnumbered interfaces. If
*           so, that adjustment is made here. 
*
* @end
*********************************************************************/
L7_RC_t ipmRouterIfCreate(L7_uint32 index, L7_uint32 intIfNum,
                          L7_uint32 ip, L7_uint32 netmask, L7_BOOL unnumbered,
                          L7_uchar8 * mac )
{
   int sock = 0;
   struct ifreq ifr;
   L7_RC_t rc;
   L7_uint32 if_up = 0;
   L7_uint32 adminState, linkState;
   L3INTF_INFO *ifInfo;
   L7_uint32 rtoGateway;
   char solicit_message[80];


   if (RouterIntfInitDone == L7_FALSE)
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "ipmRouterIfCreate: RouterIntfInitDone == L7_FALSE\n");
      return L7_ERROR;
   }
 
   if ((index < 1) || (index > MaxRouterIf))
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "rtrIntfNum %d: invalid\n", index);
      return L7_ERROR;
   }
 
   /* Make sure that IP address is not multicast.  */
   if (ip >= 0xe0000000)
   {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
               "ip 0x%08x is an invalid interface address\n", ip);
	   return L7_ERROR;
   }

   /* Initialize parms */
   ifInfo = &(l3intfInfo[index]);

   /* If this interface is already in use then return an error. */

#if 0
   if (ifInfo->port_state != L3INTF_INVALID) return L7_FAILURE;
#endif

   if ((ip != 0) && (netmask != 0))
   {
   /* If there is already a route to the new local network, remove it 
    * from the IP stack's routing table. */
   rtoGateway = 0;
   if (rtoBestRouteLookupExact(ip, netmask, &rtoGateway) == L7_SUCCESS)
   {
     /* Delete any existing route in IP stack, regardless of outgoing
      * interface or next hop IP address. */
     osapiDeleteMRoute(ip & netmask, netmask, 0, L7_INVALID_INTF);
   }
   }

   rc = L7_SUCCESS;
  do
  {
    if (osapiIfNameStringGet(intIfNum,ifr.ifr_name,IFNAMSIZ) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
              "Unable to get name IP stack uses for interface %u.", intIfNum);
      rc = L7_ERROR; 
      break;
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);


    if (ipstkIfAddrSet(ifr.ifr_name,ip) != L7_SUCCESS)
    {
      L7_uchar8 addrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      osapiInetNtoa(ip, addrStr);
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
              "Failed to set IP address %s on interface %s (%s) in the IP stack.",
              addrStr, ifr.ifr_name, ifName);
    }
    if (netmask != 0)
    {
      if (ipstkIfMaskSet(ifr.ifr_name,netmask) != L7_SUCCESS)
      {
        L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        osapiInetNtoa(netmask, maskStr);
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
                "Failed to set network mask %s on interface %s (%s) in the IP stack.",
                maskStr, ifr.ifr_name, ifName);
      }
    }

    ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_family = AF_INET;
    ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr.s_addr =
    osapiHtonl(ip | ~netmask);

    if ((ip != 0) && (netmask != 0))
    {
      if (ioctl(sock, SIOCSIFBRDADDR, &ifr) == -1)
      {
        L7_uchar8 addrStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        osapiInetNtoa(ip | ~netmask, addrStr);
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
                "Failed to set network directed broadcast address %s on interface %s (%s) in the IP stack.",
                addrStr, ifr.ifr_name, ifName);
      }
    }
  } while (0);

   /* Record interface information. */
   if (mac != L7_NULLPTR) 
   {
      memcpy(&(ifInfo->enetAddr[0]), mac, L7_ENET_MAC_ADDR_LEN);
   }
   ifInfo->ip_addr[0] = ip;
   ifInfo->ip_netmask[0] = netmask;
   ifInfo->port_state = L3INTF_VALID;

   rc = nimGetIntfAdminState (intIfNum, &adminState);
   if ((rc == L7_SUCCESS) && (adminState == L7_ENABLE)) {

      rc = nimGetIntfActiveState (intIfNum, &linkState);
      if ((rc == L7_SUCCESS) && (linkState == L7_ACTIVE)) 
      {
         if_up = 1;
      }
   }
 
   if (if_up != 0) 
   {
      ipstkRtrIfUp( index, L7_IP_MAP_COMPONENT_ID );
   } 
   else 
   {
      ipstkRtrIfDown( index, L7_IP_MAP_COMPONENT_ID );
   }

   close(sock);

   if (ipMapIntfSupports(intIfNum, IPMAP_INTFCAP_ARP) == L7_TRUE)
   {
     /*after we do the ioctls we need to set app_solict for the interface*/
     /*we also need to turn on arp filtering in the kernel on this interface*/ 
     sprintf(solicit_message,
             "/proc/sys/net/ipv4/neigh/%s/app_solicit", &ifr.ifr_name[0]);
     osapi_proc_set(solicit_message, "3");
     sprintf(solicit_message,
             "/proc/sys/net/ipv4/neigh/%s/ucast_solicit", &ifr.ifr_name[0]);
     osapi_proc_set(solicit_message, "0");
     sprintf(solicit_message, 
             "/proc/sys/net/ipv4/neigh/%s/mcast_solicit", &ifr.ifr_name[0]);
     osapi_proc_set(solicit_message, "0");
   }

   return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose  Remove router IP interface.
*
* @param    intIfNum    Internal Interface Number associated with this
*                       router port.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    invalid parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ipmRouterIfRemove(L7_uint32 intIfNum)
{
   struct ifreq ifr;
   L7_uint32 index,ipaddr;
   L3INTF_INFO *ifInfo;
   L7_uint32 ipa;        /*ipaddr in host byte order */

   if (RouterIntfInitDone == L7_FALSE)
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "ipmRouterIfRemove: RouterIntfInitDone == L7_FALSE");
      return L7_ERROR;
   }
 
   if (_ipMapIntIfNumToRtrIntf(intIfNum, &index) != L7_SUCCESS)
   {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "Could not retrieve router interface number for interface %s.", ifName);
      return L7_ERROR;
   }

   if ((index < 1) || (index > MaxRouterIf))
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "router interface number %d is invalid", index);
      return L7_ERROR;
   }

   if (osapiIfNameStringGet(intIfNum, ifr.ifr_name,IFNAMSIZ) != L7_SUCCESS) {

     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
              "Unable to get name IP stack uses for interface %u.", intIfNum);
     return (0);

   }

   ifInfo = &(l3intfInfo[index]);

   /* Return without error or logging if not ready for use */
   if (ifInfo->port_state == L3INTF_INVALID) 
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "Failed to disable router interface %d in the IP stack.", 
              index);
      return L7_FAILURE;
   }

   if (osapiIfNameStringGet(intIfNum, ifr.ifr_name, IFNAMSIZ) != L7_SUCCESS)
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
             "Unable to get name IP stack uses for interface %u.", intIfNum);
     return L7_ERROR; 
   }

   /* take the interface down */
   ipstkRtrIfDown(index,  L7_IP_MAP_COMPONENT_ID);

   /* get the specific address that was used*/
   if(ipstkIfAddrGet((L7_uchar8 *)ifr.ifr_name,&ipaddr) != L7_SUCCESS)
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
             "Failed to get IP address assigned to interface %s in IP stack. Error is %s", 
             ifr.ifr_name, strerror(errno));
   }
   else
   {
     /* note: stack itself deletes net route on if down */
     /* Call assumes ipaddr in host byte order */
     ipa = osapiNtohl(ipaddr);
     if (ipa != 0)
     {
       if (ipstkIfAddrDel((L7_uchar8 *)ifr.ifr_name, ipa) != L7_SUCCESS)
       { 
         L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
                 "Failed to delete IP address on interface %s in IP stack. Error is %s", 
                 ifr.ifr_name, strerror(errno));
       }
     }
   }
   ifInfo->port_state = L3INTF_INVALID;
   ifInfo->ip_addr[0] = 0;
   ifInfo->ip_netmask[0] = 0;
   ifInfo->ip_gateway[0] = 0;

   return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose  associate router intf with intIfNum in driver.
*
* @param    index       Router interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfMapCreate ( L7_uint32 index, L7_uint32 intIfNum )
{
   L3INTF_INFO *ifInfo;
   L7_RC_t rc = L7_SUCCESS;
   struct ifreq ifr;

   if (RouterIntfInitDone == L7_FALSE)
     return L7_ERROR;

   if (( index < 1) || ( index > MaxRouterIf) )
     return L7_ERROR;


   ifInfo = &(l3intfInfo[index]);

   /* If this interface is already in use then return an error.
   */
   if (ifInfo->port_mapped != L7_FALSE)
     return L7_FAILURE;


#if defined(L3INTF_USE_TAP)
   do
   {

    osapiIfNameStringGet(intIfNum,ifr.ifr_name,IFNAMSIZ);

    ifInfo->tap_fd = open("/dev/tap",O_RDWR);
    if(ifInfo->tap_fd < 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
             "TAP Unable to open /dev/tap."
             " The /dev/tap file is missing, or "
             "the kernel is missing “Universal TUN/TAP device driver support” (CONFIG_TUN).");
      rc = L7_ERROR; 
      break;
    }

    /*
     *use the socket created above in conjunction with 
     *the ifr structure (with the name already filled out)
     *to create the corresponding net interface
     */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if(ioctl(ifInfo->tap_fd,TUNSETIFF,&ifr) < 0)
    {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
             "Unable to create interface %s in IP stack.",
             ifr.ifr_name);
     close(ifInfo->tap_fd);
     ifInfo->tap_fd = -1;
     rc = L7_ERROR;
     break;
    }

    /*
     *now add the file descriptor to the list of those
     *managed by the tap_monitor
     */
    tap_monitor_register(ifInfo->tap_fd,intIfNum,l3intf_tap_send);

   }while(0);
#endif

   ifInfo->intIfNum = intIfNum;
   ifInfo->port_mapped = L7_TRUE;

   return(rc);
}

/*********************************************************************
*
* @purpose  de-associate router intf with intIfNum in driver.
*
* @param    index       Router interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfMapDelete ( L7_uint32 index, L7_uint32 intIfNum )
{
   L3INTF_INFO *ifInfo;

   if (RouterIntfInitDone == L7_FALSE) {

      return L7_ERROR;

   }
 
   if ((index < 1) || (index > MaxRouterIf)) {

      return L7_ERROR;

   }
 
   ifInfo = &(l3intfInfo[index]);

   if(ifInfo->port_mapped != L7_TRUE){
      return L7_ERROR;

   }

#if defined(L3INTF_USE_TAP)
   /*
    * Need to deregister the interface and close the file
    *
    * Check for a valid tap fd, as some devices (e.g., loopbacks)
    * do not have tap devices associated with them.
    */
   if (ifInfo->tap_fd >= 0)
   {
      tap_monitor_unregister(ifInfo->tap_fd);
      close(ifInfo->tap_fd);
      ifInfo->tap_fd = -1;
   }
#endif

   ifInfo->port_mapped = L7_FALSE;


   ifInfo->intIfNum = 0;

   return(L7_SUCCESS);
}


/*********************************************************************
*
* @purpose  Configure a router interface with a secondary IP address.
*
* @param    intIfNum    Internal Interface Number associated with this router port.
* @param    ip          IP Address of the interface.
* @param    netmask     Network mask.
* @param    index       Index to addrs[] array on interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ipmRouterIfSecondaryAddrAdd(L7_uint32 intIfNum, L7_uint32 ip,
                                    L7_uint32 netmask, L7_uint32 index)
{
	int sock;
	struct ifreq ifr;
	L7_RC_t rc;
	L3INTF_INFO *ifInfo;
	L7_uint32 rtoGateway;
	L7_uint32 rtrIfNum;
	L7_uchar8 ifname[IFNAMSIZ];

	memset(&ifr, 0x00, sizeof(struct ifreq));
	memset(ifname, 0x00, sizeof(ifname));

	if (RouterIntfInitDone == L7_FALSE)
	{
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
                "End driver initialization not yet done.");
		return L7_ERROR;
	}
 
	/* Check if this is a routing interface */
	if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
	{
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
            "Failed to get router interface number for internal interface %u.", 
            intIfNum);
		return L7_ERROR;
	}

	if ((rtrIfNum < 1) || (rtrIfNum > MaxRouterIf)) 
	{
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
            "Router interface number %d invalid.", rtrIfNum);
		return L7_ERROR;
	}
 
	/* Make sure that IP address is not zero and not multicast.  */
	if ((ip == 0) || (ip >= 0xe0000000))
	{
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
            "IP address 0x%08x is invalid.", ip);
		return L7_ERROR;
	}

	/* Initialize parms */
	ifInfo = &(l3intfInfo[rtrIfNum]);

	rtoGateway = 0;
	if (rtoBestRouteLookupExact(ip, netmask, &rtoGateway) == L7_SUCCESS)
	{
    /* Delete any existing route in IP stack, regardless of outgoing
     * interface or next hop IP address. */
    osapiDeleteMRoute(ip & netmask, netmask, 0, L7_INVALID_INTF);
	}

	rc = L7_SUCCESS;

	do
	{
		sock = socket(PF_INET, SOCK_DGRAM, 0);

		/* Build interface name */

		if (osapiIfNameStringGet(intIfNum, ifname,IFNAMSIZ) != L7_SUCCESS)
		{
			L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
              "Unable to get name IP stack uses for interface %u.", intIfNum);
			return L7_FAILURE;
		}

		sprintf(ifr.ifr_name, "%s:%d", ifname, index);

		if(ipstkIfAddrSet(ifr.ifr_name,ip) != L7_SUCCESS)
    {
      L7_uchar8 addrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      osapiInetNtoa(ip, addrStr);
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
              "Failed to set secondary IPv4 address %s on interface %s (%s) in IP stack.",
              addrStr, ifr.ifr_name, ifName);
    }
    if(ipstkIfMaskSet(ifr.ifr_name,netmask) != L7_SUCCESS)
    {
      L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      osapiInetNtoa(netmask, maskStr);
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
              "Failed to set secondary IPv4 address network mask %s on interface %s (%s) in IP stack.",
              maskStr, ifr.ifr_name, ifName);
    }

    ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_family = AF_INET;
    ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr.s_addr =
    osapiHtonl(ip | ~netmask);

    if (ioctl(sock, SIOCSIFBRDADDR, &ifr) == -1)
    {
      L7_uchar8 addrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      osapiInetNtoa(ip | ~netmask, addrStr);
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
              "Failed to set network directed broadcast address %s on interface %s (%s) in IP stack.",
              addrStr, ifr.ifr_name, ifName);
    }

	} while (0); 
   

	ifInfo->ip_addr[index] = ip;
	ifInfo->ip_netmask[index] = netmask;
	/*
	ifInfo->intIfNum = intIfNum;
	ifInfo->port_state = L3INTF_VALID;
	*/

	close(sock);

	return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose  Remove router secondary IP interface.
*
* @param    intIfNum    Internal Interface Number associated with this
*                       router port.
* @param    ip          IP Address of the interface.
* @param    netmask     Network mask.
* @param    index       Index to addrs[] array on interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    invalid parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ipmRouterIfSecondaryAddrDelete(L7_uint32 intIfNum, L7_uint32 ip, 
                                       L7_uint32 netmask, L7_uint32 index) 
{
	int sock;
	struct ifreq ifr;
	L7_uint32 rtrIfNum;
	L3INTF_INFO *ifInfo;
	L7_uchar8 ifname[IFNAMSIZ];

	/* Cleanup data structures */

	memset(ifname, 0x00, sizeof(ifname));
	memset(&ifr, 0x00, sizeof(struct ifreq));

	if (RouterIntfInitDone == L7_FALSE) 
	{
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
            "End driver initialization not complete.");
		return L7_ERROR;
	}
 
	if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS) 
	{
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
            "Failed to get router interface number for interface %s.", 
            ifName);
		return L7_ERROR;
	}

	if ((rtrIfNum < 1) || (rtrIfNum > MaxRouterIf)) 
	{
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
            "Router interface number %d is invalid.", rtrIfNum);
		return L7_ERROR;
	}
 
	/* Make sure that IP address is not zero and not a multicast */

	if ((ip == 0) || (ip >= 0xe0000000))
	{
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
            "IP address 0x%08x is invalid.", ip);
		return L7_ERROR;
	}

	ifInfo = &(l3intfInfo[rtrIfNum]);

	/* Return without error or logging if not ready for use */

	if (ifInfo->port_state == L3INTF_INVALID) 
	{
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
                "rtrIfNum %d: Interface state invalid\n", rtrIfNum);
		return L7_FAILURE;
	}

	/* Build interface name */

	if (osapiIfNameStringGet(intIfNum, ifname,IFNAMSIZ) != L7_SUCCESS)
	{
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "Unable to get name IP stack uses for interface %u.", intIfNum);
		return L7_FAILURE;
	}
	sprintf(ifr.ifr_name, "%s:%d", ifname, index);

	sock = socket(PF_INET, SOCK_DGRAM, 0);

	if(ipstkIfAddrDel(ifr.ifr_name, ip) != L7_SUCCESS)
	{
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
            "Failed to delete secondary IP address from IP stack interface %s.", 
            ifr.ifr_name);
	}

	ifInfo->ip_addr[index] = 0;
	ifInfo->ip_netmask[index] = 0;

	close(sock);

	return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose  Get maximum number of routing interfaces
*
* @returns  max_if  Maximum number of routing interfaces.
*
* @notes    probably should be an IP MAP API instead, but not worth
*           the effort to move it. 
*
* @end
*********************************************************************/
L7_uint32 ipmRouterIfMaxCountGet(void) 
{
   return MaxRouterIf;
}

/*********************************************************************
*
* @purpose  Verify that the IP interface is valid and configured.
*
* @param    ifIndex    IP Interface index.
*
* @returns  0    Interface is not valid or configured.
* @returns  1    Interface is valid.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipmIfCheck(L7_uint32 ifIndex) {

   if ((ifIndex < 1) || (ifIndex > MaxRouterIf)) {

      return 0;

   }
 
   if (l3intfInfo[ifIndex].port_state == L3INTF_INVALID) {

      return 0;

   }
 
   return 1;

}

/*********************************************************************
*
* @purpose  Get default gateway for the specified interface.
*
* @param    intIfNum    Internal Interface Number
*
* @returns  0    No valid default gateway defined for this interface.
* @returns  gateway_ip Default gateway of primary IP address.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipmRouterIfDefaultGatewayGet (L7_uint32 intIfNum) {
 
	L7_uint32 ifIndex;
 
	if (_ipMapIntIfNumToRtrIntf(intIfNum, &ifIndex) != L7_SUCCESS)
	{
		return 0;
	}
 
	if (!ipmIfCheck(ifIndex))
	{
		return 0;
	}
 
	/*
	For the primary gateway to be valid, it must be in the same subnet
	as the primary IP address of the interface.
	*/

	if ((l3intfInfo[ifIndex].ip_addr[0] & l3intfInfo[ifIndex].ip_netmask[0]) ==
		(l3intfInfo[ifIndex].ip_gateway[0] & l3intfInfo[ifIndex].ip_netmask[0]))
	{
		return l3intfInfo[ifIndex].ip_gateway[0];
	}
 
	return 0;
}

/*********************************************************************
*
* @purpose  Get name string for the specified interface.
*
* @param    buf        (output) Buffer to store name string.
*
* @returns  L7_SUCCESS Interface is valid.
* @returns  L7_FAILURE   Interface is not valid.
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ipmRouterIfDrvNamePrefixGet( L7_uchar8 *buf)
{

	strcpy(buf, L3INTF_DEVICE_NAME);

	return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose  Get cost metric for the specified interface.
*
* @param    intIfNum   Internal Interface Number
* @param    metric     (output) Interface metric.
*
* @returns  L7_SUCCESS Interface is valid.
* @returns  L7_FAILURE Interface is not valid.
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ipmRouterIfMetricGet(L7_uint32 intIfNum, L7_uint32 *metric)
{
   struct ifreq ifr;
   L7_uint32 adminState, activeState;

   if (osapiIfNameStringGet(intIfNum, ifr.ifr_name,IFNAMSIZ) != L7_SUCCESS) {

	  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
            "Unable to get name IP stack uses for interface %u.", intIfNum);
      return (L7_FAILURE);

   }

   if ( (nimGetIntfAdminState(intIfNum, &adminState) != L7_SUCCESS) ||
        (adminState != L7_ENABLE) ||
        (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS) ||
        (activeState != L7_ACTIVE) )
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
             "Failed to get interface %s routing metric from the IP stack. "
             "Interface is not active.",
             ifr.ifr_name);
     return(L7_ERROR);
   }


   if(ipstkIfMetricGet(ifr.ifr_name,metric) != L7_SUCCESS) {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
              "Failed to get interface %s routing metric from IP stack.",
              ifr.ifr_name);
      return (L7_FAILURE);

   }

   return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose  Create task to service ARP requests.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ipmArpTaskCreate(void)
{
    L7_RC_t rc = osapiTaskCreate("linux_arpd_task", arpd_entry_fn, 0, 0,
                                 L7_DEFAULT_STACK_SIZE,
                                 L7_MEDIUM_TASK_PRIORITY,
                                 L7_DEFAULT_TASK_SLICE);

    if (rc != L7_ERROR)
    {
        /* task create succeeded and returned the new task ID */
        arpd_task_id = rc;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Delete task servicing ARP requests.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ipmArpTaskDelete(void)
{
    osapiTaskDelete(arpd_task_id);
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  driver up
*
* @param    index       internal interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfDrvUp ( L7_uint32 intIfNum)
{
   int sock;
   struct ifreq ifr;
   L7_RC_t rc;
   L7_uchar8 mac[L7_MAC_ADDR_LEN];
   L7_INTF_TYPES_t itype;

   rc = L7_SUCCESS;

   if (osapiIfNameStringGet(intIfNum,ifr.ifr_name,IFNAMSIZ) != L7_SUCCESS)
   {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
              "Unable to get name IP stack uses for interface %u.", intIfNum);
       return L7_ERROR; 
   }

   if(nimGetIntfType(intIfNum, &itype) != L7_SUCCESS){
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
               "intf %s: unable to get interface type\n", ifName);
       return L7_ERROR; 
   }
   switch(itype){

   case L7_PHYSICAL_INTF:
   case L7_LOGICAL_VLAN_INTF:
   case L7_LOOPBACK_INTF:
     if (nimGetIntfL3MacAddress(intIfNum, 0, mac) != L7_SUCCESS)
       return L7_ERROR;

     sock = socket(PF_INET, SOCK_DGRAM, 0);
 
     /* Set the MAC address */
     ifr.ifr_hwaddr.sa_family = L7_ARPHRD_ETHER;
     memcpy(&ifr.ifr_hwaddr.sa_data, mac, L7_ENET_MAC_ADDR_LEN);
 
     if (ioctl(sock, SIOCSIFHWADDR, &ifr) == -1) 
     {
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
               "Failed to set MAC address on IP stack interface %s (%s).",
               ifr.ifr_name, ifName);
       rc = L7_ERROR;
     }
     close(sock);
     break;
   default:
     break;
   }

   ipstkIfFlagChange(ifr.ifr_name, IFF_UP,L7_TRUE);

   return rc;
}


/*********************************************************************
*
* @purpose  driver down
*
* @param    intIfNum    Internal Interface Number associated with this router port.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfDrvDown ( L7_uint32 intIfNum)
{
  struct ifreq ifr;

  if (osapiIfNameStringGet(intIfNum,ifr.ifr_name,IFNAMSIZ) != L7_SUCCESS)
  {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LAYER3_COMPONENT_ID,
               "Failed to get IP stack name for interface %u.", intIfNum);
       return L7_ERROR; 
  }

  ipstkIfFlagChange(ifr.ifr_name, IFF_UP,L7_FALSE);
  return L7_SUCCESS;
}

