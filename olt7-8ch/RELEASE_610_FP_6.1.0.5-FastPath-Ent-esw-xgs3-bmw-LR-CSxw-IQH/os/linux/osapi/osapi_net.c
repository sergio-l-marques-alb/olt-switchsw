/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_net.c
*
* @purpose   OS independent API's
*
* @component osapi
*
* @comments
*
* @create    11/20/2001
*
* @author    John Linville
*
* @end
*
*********************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <net/route.h>
#include <sys/param.h>

#include "l7_common.h"
#include "defaultconfig.h"
#include "dtlapi.h"
#include "osapi.h"
#include "osapi_priv.h"
#include "osapi_support.h"
#include "log.h"
#include "bspapi.h"
#include "l7_socket.h"
#include "l7_icmp.h"
#include "ping6_map_api.h"
#include "l3end_api.h"
#include <asm/types.h>
#include <linux/rtnetlink.h>
/* for tunnels */
#include <linux/ip.h>
#include <linux/if_tunnel.h>
#define OSAPI_DHCP_MAX_TRIES  10

/*this is taken from <linux/sockios.h> to create a private ioctl command*/
  #define SIOCGPHYSTATUS SIOCDEVPRIVATE
  #define PHY_LINK_UP 0x0001
  #define PHY_LINK_DOWN 0x0000

/* Set to L7_FALSE by IP Map if routing is enabled on the switch */
L7_uint32 servPortDefaultRouteEnable = L7_TRUE;

static L7_uint32 L7IcmpOutEchos = 0;

static L7_uint32 osapiNetlinkSequence = 1;

/* if this flag is cleared in the arp cache
 *  that indicates that an entry is incomplete, and should
 *  no be displayed
 */
#define ARP_ENTRY_CMPLT_FLAG 0x02
#define ARP_INCOMPLETE 0x00

extern void osapiDebugSysInfoDumpRegister(void);

/* callback struct for netlink address dump functions */
typedef struct osapiNetlinkAddrDumpInfo_s
{
  L7_uint32          sd;         /* socket */
  L7_uint32   ifi;        /* stack interface index */
  struct sockaddr_nl nladdr;    /* netlink sockaddr */
  L7_in6_prefix_t    *addrs;
  L7_in6_addr_t      *gateways;
  L7_uint32          maxcount;
  L7_uint32          acount;
}osapiNetlinkAddrDumpInfo_t;


#include "ipstk_api.h"


/**************************************************************************
*
* @purpose  Add route to the routing table 
*
* @param	ifName	 @b{(input)} Interface to add the route to
* @param	destIp	 @b{(input)} destination address, network order 
* @param	gateIp 	 @b{(input)} gateway address, network order
* @param	mask	   @b{(input)} mask for destination, network order
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiRawMRouteEntryAdd(L7_uchar8 *ifName, L7_long32 destIp, L7_long32 gateIp,
  L7_long32 mask, L7_uint32 tos, L7_uint32 flags, L7_uint32 proto)
{
  if (proto != 1) /* Currently only implemented for IP */
  {
    return L7_FAILURE;
  }
  else
  {
    struct rtentry  Entry1;
    struct ifreq ifr;
    int       s, rc, lerrno;
    char      device[20];


    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (s < 0)
    {
      LOG_MSG ("osapimRouteEntryAdd - failed to open socket.\n");
      return L7_FAILURE;
    }

    memset(&Entry1, 0x00, sizeof (Entry1));
    ((struct sockaddr_in *) &Entry1.rt_dst)->sin_addr.s_addr = osapiHtonl(destIp);
    ((struct sockaddr_in *) &Entry1.rt_dst)->sin_family = AF_INET;
    ((struct sockaddr_in *) &Entry1.rt_gateway)->sin_addr.s_addr = osapiHtonl(gateIp);
    ((struct sockaddr_in *) &Entry1.rt_gateway)->sin_family = AF_INET;
    ((struct sockaddr_in *) &Entry1.rt_genmask)->sin_addr.s_addr = osapiHtonl(mask);
    ((struct sockaddr_in *) &Entry1.rt_genmask)->sin_family = AF_INET;

    /* If network mask is 255.255.255.255 then this is a host route.
    */
    if (mask == 0xffffffff)
    {
      flags |= RTF_HOST;
    }

    Entry1.rt_flags = (unsigned short) flags;
    Entry1.rt_tos = (unsigned char) tos;

    sprintf(device, "%s", ifName);

    /*
    LOG_MSG("Attempting to add route IP 0x%x Mask 0x%x Gate 0x%x IF %s\n",
      destIp, mask, gateIp, device);
    */
    strcpy(ifr.ifr_name, device);

    if (strchr(device, ':') != NULL)
    {
        if (ioctl(s, SIOCGIFADDR, &ifr) == -1)
        {
            /*
            If sub-interface device does not exist, attempt to add the route
            to the main interface
            */

            /*
            LOG_MSG("osapimRouteEntryAdd: device %s does not exist ... \n",
              device);
            */

            *(strchr(device, ':')) = '\0';

            /*
            LOG_MSG("osapimRouteEntryAdd: adding 0x%x to %s instead\n",
              destIp, device);
            */
        }
    }

    Entry1.rt_dev = device;

    rc = ioctl(s, SIOCADDRT, &Entry1);
    lerrno = errno;

    if (rc < 0 && errno != EEXIST && errno != ENODEV)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
              "osapimRouteEntryAdd, errno %d adding 0x%lx to %s."
              " Error adding or deleting an IPv4 route (listed in"
              " hex as 0x%lx), on the interface with Linux name %s Error"
              " code can be looked up in errno.h.",
              errno, destIp, ifName, destIp, ifName);
      perror ("osapimRouteEntryAdd - SIOCADDRT ");
      close(s);
      return L7_FAILURE;
    }

    close(s);
    return L7_SUCCESS;
  }
}
/**************************************************************************
*
* @purpose  Delete route from the routing table 
*
* @param	ifName	 @b{(input)} Interface to delete the route from
* @param	destIp	 @b{(input)} destination address, network order 
* @param	gateIp 	 @b{(input)} gateway address, network order
* @param	mask	   @b{(input)} mask for destination, network order
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t
osapiRawMRouteEntryDelete(L7_uchar8 *ifName, L7_long32 destIp, L7_long32 gateIp,
  L7_long32 mask, L7_uint32 tos, L7_uint32 flags, L7_uint32 proto)
{
  if (proto != 1) /* Currently only implemented for IP */
  {
    LOG_MSG("osapimRouteEntryDelete - Unsupported protocol %d\n", proto);
    return L7_FAILURE;
  }
  else
  {
    struct rtentry  Entry1;
    struct ifreq ifr;
    int       s, rc, lerrno;
    char      device[20];

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (s < 0)
    {
      LOG_MSG ("osapimRouteEntryDelete - failed to open socket.\n");
      return L7_FAILURE;
    }

    memset(&Entry1, 0x00, sizeof (Entry1));
    ((struct sockaddr_in *) &Entry1.rt_dst)->sin_addr.s_addr = osapiHtonl(destIp);
    ((struct sockaddr_in *) &Entry1.rt_dst)->sin_family = AF_INET;
    ((struct sockaddr_in *) &Entry1.rt_gateway)->sin_addr.s_addr = osapiHtonl(gateIp);
    ((struct sockaddr_in *) &Entry1.rt_gateway)->sin_family = AF_INET;
    ((struct sockaddr_in *) &Entry1.rt_genmask)->sin_addr.s_addr = osapiNtohl(mask);
    ((struct sockaddr_in *) &Entry1.rt_genmask)->sin_family = AF_INET;
    Entry1.rt_flags = (unsigned short) flags;
    Entry1.rt_tos = (unsigned char) tos;

    sprintf(device, "%s", ifName);

    if (strchr(device, ':') != NULL)
    {
        if (ioctl(s, SIOCGIFADDR, &ifr) == -1)
        {
            /*
            If sub-interface device does not exist, attempt to delete the route
            from the main interface
            */

            /*
            LOG_MSG("osapimRouteEntryDelete: device %s does not exist ... \n",
              device);
            */

            *(strchr(device, ':')) = '\0';

            /*
            LOG_MSG("osapimRouteEntryDelete: deleting 0x%x from %s instead\n",
              destIp, device);
            */
        }
    }

    Entry1.rt_dev = device;

    rc = ioctl(s, SIOCDELRT, &Entry1);
    lerrno = errno;

    if (rc < 0 && errno != ENODEV && errno != ESRCH)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
              "osapimRouteEntryDelete, errno %d deleting 0x%lx from %s."
              " Error adding or deleting an IPv4 route (listed in"
              " hex as 0x%lx), on the interface with Linux name %s Error"
              " code can be looked up in errno.h.",
        errno, destIp, ifName, destIp, ifName);
      perror ("osapimRouteEntryDelete SIOCDELRT ");
      close(s);
      return L7_FAILURE;
    }
    close(s);
  }
  return L7_SUCCESS;
}


/**************************************************************************
* @purpose  Add a route
*
* @param    destination  @b{(input)} 32bit destination ip address
* @param    gateway      @b{(input)} 32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @comments    
*
* @end
*************************************************************************/
static L7_RC_t osapiRawRouteAdd( L7_uint32 destination, L7_uint32 gateway )
{
  struct rtentry rtreq;
  int fd;

  if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {

    return (L7_FAILURE);

  }

  memset(&rtreq, 0, sizeof(rtreq));

  ((struct sockaddr_in *)&(rtreq.rt_dst))->sin_family = AF_INET;
  ((struct sockaddr_in *)&(rtreq.rt_dst))->sin_addr.s_addr = osapiHtonl(destination);
  ((struct sockaddr_in *)&(rtreq.rt_gateway))->sin_family = AF_INET;
  ((struct sockaddr_in *)&(rtreq.rt_gateway))->sin_addr.s_addr = osapiHtonl(gateway);

  rtreq.rt_flags = (RTF_UP | RTF_GATEWAY);

  if (ioctl(fd, SIOCADDRT, &rtreq) == -1) {

    return (L7_FAILURE);

  }

  close(fd);

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Delete a route
*
* @param    destination  @b{(input)} 32bit destination ip address
* @param    gateway      @b{(input)} 32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @comments    
*
* @end
*************************************************************************/
static L7_RC_t osapiRawRouteDelete( L7_uint32 destination, L7_uint32 gateway )
{
  struct rtentry rtreq;
  int fd;

  if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {

    return (L7_FAILURE);

  }

  memset(&rtreq, 0, sizeof(rtreq));

  ((struct sockaddr_in *)&(rtreq.rt_dst))->sin_family = AF_INET;
  ((struct sockaddr_in *)&(rtreq.rt_dst))->sin_addr.s_addr = osapiHtonl(destination);
  ((struct sockaddr_in *)&(rtreq.rt_gateway))->sin_family = AF_INET;
  ((struct sockaddr_in *)&(rtreq.rt_gateway))->sin_addr.s_addr = osapiHtonl(gateway);

  if (ioctl(fd, SIOCDELRT, &rtreq) == -1) {

    return (L7_FAILURE);

  }

  close(fd);

  return L7_SUCCESS;
}


/**************************************************************************
* @purpose  Add a route
*
* @param    ipAddr    @b{(input)} 32bit destination ip address
* @param    netmask   @b{(input)} 32bit network mask
* @param    gateway   @b{(input)} 32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @comments    
*
* @end
*************************************************************************/
static L7_RC_t osapiRawMRouteAdd( L7_uint32 ipAddr, L7_uint32 netmask, L7_uint32 gateway )
{
  int    sock;
  struct rtentry  rtEntry;

  /* Probably this should be changed to use Netlink sockets... */

  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {

     LOG_MSG("Failed to Open Socket\n");
     return L7_FAILURE;

  }

  memset(&rtEntry, 0x00, sizeof (rtEntry));
  ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_addr.s_addr = osapiHtonl(ipAddr);
  ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_family = AF_INET;
  ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_addr.s_addr = osapiHtonl(gateway);
  ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_family = AF_INET;
  ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_addr.s_addr = osapiHtonl(netmask);
  ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_family = AF_INET;

  rtEntry.rt_dev = L7_NULLPTR;
  rtEntry.rt_flags |=RTF_UP;

  if((ipAddr & netmask) != (gateway & netmask))
     rtEntry.rt_flags |= RTF_GATEWAY;

  if((ipAddr == 0) && (netmask ==0))
  {
      if (gateway != 0)
          rtEntry.rt_flags |= RTF_GATEWAY;
  }

  if ((ioctl(sock, SIOCADDRT, &rtEntry) < 0) && (errno != EEXIST)) {

     L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
             "l3intfAddRoute: Failed to Add Route."
             " Error adding a default gateway"
             " in the kernel’s routing table (the function"
             " is really osapiRawMRouteAdd())");
     close(sock);
     return L7_FAILURE;

  }

  close(sock);

  return L7_SUCCESS;
}
/**************************************************************************
* @purpose  Delete a route
*
* @param    ipAddr    @b{(input)} 32bit destination ip address
* @param    netmask   @b{(input)} 32bit network mask
* @param    gateway   @b{(input)} 32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @comments    
*
* @end
*************************************************************************/
static L7_RC_t osapiRawMRouteDelete( L7_uint32 ipAddr, L7_uint32 netmask, L7_uint32 gateway )
{
  int    sock;
  struct rtentry  rtEntry;

  /* Probably this should be changed to use Netlink sockets... */

  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {

     LOG_MSG("Failed to Open Socket\n");
     return L7_FAILURE;

  }

  memset(&rtEntry, 0x00, sizeof (rtEntry));
  ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_addr.s_addr = osapiHtonl(ipAddr);
  ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_family = AF_INET;
  ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_addr.s_addr = osapiHtonl(gateway);
  ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_family = AF_INET;
  ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_addr.s_addr = osapiHtonl(netmask);
  ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_family = AF_INET;

  rtEntry.rt_dev = L7_NULLPTR;

  if ((ioctl(sock, SIOCDELRT, &rtEntry) < 0) && (errno != ESRCH)) {

     L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
             "l3intfDeleteRoute: Failed to Delete Route."
             " Error deleting a default gateway"
             " in the kernel’s routing table (the function"
             " is really osapiRawMRouteDelete())");
     close(sock);
     return L7_FAILURE;

  }

  close(sock);

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Gets interface address/
*
* @param    ifName    @b{(input)} pointer to the interface name
* @param    ifAddr    @b{(input)} 32bit address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*
* @end
*************************************************************************/
L7_RC_t osapiRawIfAddrGet( L7_uchar8 *ifName, L7_uint32 *ipAddr )
{
  L7_int32 sock;
  struct ifreq ifr;
  L7_RC_t rc = 0;

  sock = socket(PF_INET, SOCK_DGRAM, 0);

  if (sock < 0)
  {
    LOG_MSG("osapiNetIPGet: unable to open a socket! sock = %d\n", sock);
    return L7_FAILURE;
  }

  strcpy(ifr.ifr_name, ifName);

  /*
  Try to get address for tmpIfName
  */

  rc = ioctl(sock, SIOCGIFADDR, (int)&ifr);

  if(rc == 0){
    *ipAddr = osapiNtohl(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
    return L7_SUCCESS;
  }
  return L7_FAILURE;

}


/**************************************************************************
* @purpose  Manage vxworks mbufs
*
* @param    bufHandle   vxWorks mbuf
* @param    bufData     start of data for vxWorks mbuf
* @param    bufSize     size of data for vxWorks mbuf
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void osapiNetMbufGetNextBuffer(void *bufHandle, L7_uchar8 **bufData,
                               L7_uint32 *bufSize )
{

  LOG_MSG("osapiNetMbufGetNextBuffer not yet implemented!\n");

}

/**************************************************************************
* @purpose  Get the next mbuf in chain
*
* @param    mbuf    vxWorks mbuf
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void *osapiNetMbufGetNextMbuf(void *mbuf)
{

  LOG_MSG("osapiNetMbufGetNextMbuf not yet implemented!\n");

  return(L7_NULLPTR);

}

/**************************************************************************
* @purpose  Get the total length of data in a mbuf chain
*
* @param    mbuf    vxWorks mbuf
*
* @returns  frameLength
*
* @comments    none.
*
* @end
*************************************************************************/
L7_uint32 osapiNetMbufGetFrameLength(void *mbuf)
{

  LOG_MSG("osapiNetMbufGetFrameLength not yet implemented!\n");

  return(L7_SUCCESS);

}

/**************************************************************************
* @purpose  Free vxworks mbufs
*
* @param    bufHandle   vxWorks mbuf
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void osapiNetMbufFree(void *bufHandle)
{

  LOG_MSG("osapiNetMbufFree not yet implemented!\n");

}

/**************************************************************************
*
* @purpose  Initializes the IP Network
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiNetworkInit(void)
{
#ifdef L7_ROUTING_PACKAGE
  L7_uchar8 rtrIntfMaxStr[20];
#endif

   if (ipstkInit() != L7_SUCCESS)
   {
     return L7_FAILURE;
   }  

#ifdef L7_ROUTING_PACKAGE
  if(ipstkRoutingSharedLibInit() != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Linux enforces a limit on the number of groups that can be joined on 
   * a socket. The default is 20. OSPF uses a single socket for all interfaces
   * and joins two groups (224.0.0.5 and 224.0.0.6) on each interface where 
   * OSPF is enabled. So clearly 20 group memberships isn't enough. RIP only
   * joins one group per interface. So set igmp_max_memberships to twice 
   * the max number of routing interfaces. */
  osapiSnprintf(rtrIntfMaxStr, 20, "%d", 2 * L7_MAX_NUM_ROUTER_INTF);
  osapi_proc_set("/proc/sys/net/ipv4/igmp_max_memberships", rtrIntfMaxStr);
#endif

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  {
    L7_uchar8 procfs_path[80];

    /* IPv6 is disabled in the kernel for all interfaces by default. 
       Enable on the service port and loopback interfaces. */
    osapi_proc_set("/proc/sys/net/ipv6/conf/lo/ipv6_enable", "1");

    if (SERVICE_PORT_PRESENT) {
        osapiSnprintf(procfs_path, 79, 
                      "/proc/sys/net/ipv6/conf/%s0/ipv6_enable", 
                      bspapiServicePortNameGet());
        osapi_proc_set(procfs_path, "1");
        memset(procfs_path, 0, 80);
        osapiSnprintf(procfs_path, 79, 
                      "/proc/sys/net/ipv6/conf/%s0/autoconf",
                      bspapiServicePortNameGet());
        osapi_proc_set(procfs_path, "0");
    }

    /* to keep anvl happy guarantee pmtu timeout < 600 by allowing for
       gc_interval slop */
    osapi_proc_set("/proc/sys/net/ipv6/route/mtu_expires", "560");
  }
#endif

  osapiDebugSysInfoDumpRegister();

  return L7_SUCCESS;
}


/**************************************************************************
* @purpose  Configures an ip interface.
*
* @param    ifName   pointer to the interface name
* @param    ipAddr   32bit ip address
* @param    ifName   32bit netmask
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*************************************************************************/
L7_int32 osapiNetIfConfig(L7_uchar8 *ifName, L7_uint32 ipAddr,
                          L7_uint32 ipNetMask )
{
  int sock;
  struct ifreq ifr;
  char tmpName[20];
  L7_int32 rc;

  sock = socket(PF_INET, SOCK_DGRAM, 0);

  if (strchr(ifName, ':') != NULL)
  {
    /*
    Trying to configure a sub-interface, so be sure that address
    is not the same as the main interface
    */

    strcpy(tmpName, ifName);
    *(strchr(tmpName, ':')) = '\0';
    strcpy(ifr.ifr_name, tmpName);
    rc = ioctl(sock, SIOCGIFADDR, &ifr);
    if (rc != 0)
    {
      /* Don't print a message to the screen if the interface doesn't exist
       because it might not be created yet.  This is a special case that can
       happen during bootup when the dtl0 interface if configured for dhcp.
       ADG */
      if (errno != ENODEV)
      {
        osapi_printf(
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d\n",
                    ifr.ifr_name, ipAddr, errno);
        perror("osapiNetIfConfig - SIOCGIFADDR ");
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d."
                    " Failed trying to set the IP address (in hex as 0x%x) of the "
                    "interface with Linux name %s, and the interface does not exist."
                    " Sometimes this is a harmless race condition (e.g. we try to set"
                    " address 0 when DHCPing on the network port (dtl0) at bootup, "
                    "before it’s created using TAP). ",
                    ifr.ifr_name, ipAddr, errno, ipAddr, ifr.ifr_name);
      }
      close (sock);
      return(L7_ERROR);
    }

    if (osapiNtohl(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr) == ipAddr)
    {
      osapi_printf(
                  "osapiNetIfConfig: base intf %s has same address 0x%x, ignoring %s\n",
                  tmpName, ipAddr, ifr.ifr_name);

      close (sock);
      return(L7_ERROR);
    }
  }

  strncpy(ifr.ifr_name, ifName, (IFNAMSIZ - 1));
  ifr.ifr_name[strlen(ifName)] = '\0';

  /*
  LOG_MSG("osapiNetIfConfig: setting addr 0x%x, intf %s\n", ipAddr,
    ifr.ifr_name);
  */

  ((struct sockaddr_in *)&ifr.ifr_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = osapiHtonl(ipAddr);

  if (ioctl(sock, SIOCSIFADDR, &ifr) == -1)
  {
    /* Don't print a message to the screen if the interface doesn't exist
       because it might not be created yet.  This is a special case that can
       happen during bootup when the dtl0 interface if configured for dhcp.
       ADG */
    if (errno != ENODEV)
    {
      osapi_printf(
                  "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d\n",
                  ifName, ipAddr, errno);
      perror("osapiNetIfConfig - SIOCSIFADDR ");
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d."
                    " Failed trying to set the IP address (in hex as 0x%x) of the "
                    "interface with Linux name %s, and the interface does not exist."
                    " Sometimes this is a harmless race condition (e.g. we try to set"
                    " address 0 when DHCPing on the network port (dtl0) at bootup, "
                    "before it’s created using TAP). ",
                    ifName, ipAddr, errno, ipAddr, ifName);
    }
    close (sock);
    return(L7_ERROR);
  }

  if (ipAddr != 0)
  {
    /*
    ipAddr == 0 means that the interface has been removed (in Linux)
    */

    ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_family = AF_INET;
    ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr = osapiHtonl(ipNetMask);

    if (ioctl(sock, SIOCSIFNETMASK, &ifr) == -1)
    {
      /* Don't print a message to the screen if the interface doesn't exist
       because it might not be created yet.  This is a special case that can
       happen during bootup when the dtl0 interface if configured for dhcp.
       ADG */
      if (errno != ENODEV)
      {
        osapi_printf(
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d\n",
                    ifName, ipAddr, errno);
        perror("osapiNetIfConfig - SIOCSIFNETMASK ");
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d."
                    " Failed trying to set the IP address (in hex as 0x%x) of the "
                    "interface with Linux name %s, and the interface does not exist."
                    " Sometimes this is a harmless race condition (e.g. we try to set"
                    " address 0 when DHCPing on the network port (dtl0) at bootup, "
                    "before it’s created using TAP). ",
                    ifName, ipAddr, errno, ipAddr, ifName);
      }
      close (sock);
      return(L7_ERROR);
    }

    ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_family = AF_INET;
    ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr.s_addr =
    osapiHtonl((ipAddr | ~ipNetMask));

    if (ioctl(sock, SIOCSIFBRDADDR, &ifr) == -1)
    {
      /* Don't print a message to the screen if the interface doesn't exist
       because it might not be created yet.  This is a special case that can
       happen during bootup when the dtl0 interface if configured for dhcp.
       ADG */
      if (errno != ENODEV)
      {
        osapi_printf(
                    "osapiNetIPSet: ioctl on %s failed: addr: 0x%x, err: %d\n",
                    ifName, ipAddr, errno);
        perror("osapiNetIfConfig - SIOCSIFBRDADDR ");
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                    "osapiNetIPSet: ioctl on %s failed: addr: 0x%x, err: %d."
                    " Failed trying to set the IP address (in hex as 0x%x) of the "
                    "interface with Linux name %s, and the interface does not exist."
                    " Sometimes this is a harmless race condition (e.g. we try to set"
                    " address 0 when DHCPing on the network port (dtl0) at bootup, "
                    "before it’s created using TAP). ",
                    ifName, ipAddr, errno, ipAddr, ifName);
      }
      close (sock);
      return(L7_ERROR);
    }
  }

  if (strchr(ifName, ':') == NULL)
  {
    /*
    The SIFFLAGS operation is not valid on sub-interfaces, only
    main interfaces
    */

    if (ioctl(sock, SIOCGIFFLAGS, &ifr) == -1)
    {
      /* Don't print a message to the screen if the interface doesn't exist
       because it might not be created yet.  This is a special case that can
       happen during bootup when the dtl0 interface if configured for dhcp.
       ADG */
      if (errno != ENODEV)
      {
        osapi_printf(
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d\n",
                    ifName, ipAddr, errno);
        perror("osapiNetIfConfig - SIOCGIFFLAGS ");
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d."
                    " Failed trying to set the IP address (in hex as 0x%x) of the "
                    "interface with Linux name %s, and the interface does not exist."
                    " Sometimes this is a harmless race condition (e.g. we try to set"
                    " address 0 when DHCPing on the network port (dtl0) at bootup, "
                    "before it’s created using TAP). ",
                    ifName, ipAddr, errno, ipAddr, ifName);
      }
      close (sock);
      return(L7_ERROR);
    }

    ifr.ifr_flags |= IFF_UP;

    if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1)
    {
      /* Don't print a message to the screen if the interface doesn't exist
       because it might not be created yet.  This is a special case that can
       happen during bootup when the dtl0 interface if configured for dhcp.
       ADG */
      if (errno != ENODEV)
      {
        osapi_printf(
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d\n",
                    ifName, ipAddr, errno);
        perror("osapiNetIfConfig - SIOCSIFFLAGS ");
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                    "osapiNetIfConfig: ioctl on %s failed: addr: 0x%x, err: %d."
                    " Failed trying to set the IP address (in hex as 0x%x) of the "
                    "interface with Linux name %s, and the interface does not exist."
                    " Sometimes this is a harmless race condition (e.g. we try to set"
                    " address 0 when DHCPing on the network port (dtl0) at bootup, "
                    "before it’s created using TAP). ",
                    ifName, ipAddr, errno, ipAddr, ifName);
      }
      close (sock);
      return(L7_ERROR);
    }
  }

  close (sock);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Set the IPv4 MTU on an interface in the IP stack.
*
* @param    intIfNum              internal interface number
* @param    ipv4Mtu               IPv4 MTU
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
#ifdef L7_ROUTING_PACKAGE
L7_RC_t osapiIfIpv4MtuSet(L7_uint32 intIfNum, L7_uint32 ipv4Mtu)
{
    L7_uchar8  ifName[IFNAMSIZ];
    struct ifreq       ifreq;
    int fd;

    if (osapiIfNameStringGet(intIfNum, ifName, IFNAMSIZ) != L7_SUCCESS)
        return L7_FAILURE;

    fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name, ifName);
    ifreq.ifr_ifru.ifru_mtu = ipv4Mtu;

    if (ioctl(fd, SIOCSIFMTU, &ifreq) < 0)
    {
        (void)close(fd);
        return L7_FAILURE;
    }
    (void)close(fd);
    return L7_SUCCESS;
}
#endif
/**************************************************************************
* @purpose  Attaches an ip interface.
*
* @param    ifUnit    @b{(input)} unit number 
* @param    ifName    @b{(input)} pointer to the interface type name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*
* @end
*************************************************************************/
L7_RC_t osapiIfAttach( L7_uint32 ifUnit, L7_uchar8 *ifName)
{
    return L7_FAILURE;

}



/**************************************************************************
*
* @purpose  open a netlink socket for dump
*
* @param    sd      pointer to opened socket
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiNetlinkDumpOpen(L7_uint32 *sd, struct sockaddr_nl *nladdr)
{
  int rc;

  /*
   *get an open netlink socket
   */
  *sd = socket(AF_NETLINK,SOCK_RAW,NETLINK_ROUTE);
  if (*sd < 0)
  {
    return L7_FAILURE;
  }

  memset(nladdr,0,sizeof(struct sockaddr_nl));
  nladdr->nl_family = AF_NETLINK;

  rc = bind(*sd,(struct sockaddr *)nladdr,sizeof(struct sockaddr_nl));
  if (rc != 0)
  {
    LOG_MSG("osapiNetlinkDumpOpen: unable to bind socket! errno = %d\n",errno);
    close(*sd);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  send a netlink dump request
*
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
static
L7_RC_t osapiNetlinkDumpRequest( L7_uint32 sd, L7_uint32 seq, L7_uint32 rtype, void *request, L7_uint32 rlen)
{
  struct sockaddr_nl nladdr;
  struct nlmsghdr nlh;
  struct iovec iov[2] = {{&nlh, sizeof(nlh)}, {request, rlen}};
  struct msghdr msg = {(void *)&nladdr, sizeof(nladdr),iov,2,NULL,0,0};
  int rc;


  memset(&nladdr,0,sizeof(nladdr));
  nladdr.nl_family= AF_NETLINK;

  nlh.nlmsg_len = NLMSG_LENGTH(sizeof(rlen));
  nlh.nlmsg_flags = NLM_F_REQUEST|NLM_F_ROOT|NLM_F_MATCH;
  nlh.nlmsg_type = rtype;
  nlh.nlmsg_seq = seq;
  nlh.nlmsg_pid = osapiGetpid();

  rc = sendmsg(sd,&msg,0);
  if (rc < 0)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/**************************************************************************
*
* @purpose  process each netlink dumped element
*
* @param    sd          netlink socket
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
static 
L7_RC_t osapiNetlinkDumpProcess(L7_uint32 sd, L7_uint32 seq, L7_uint32 pid, L7_uint32 rtype,
                             void *arg, 
                             L7_RC_t (*callback)(struct nlmsghdr *nm, void *arg))
{
  int rc; /*return code*/
  int buffer_length;
  struct sockaddr_nl nladdr;
  int messages_pending;

  struct nlmsghdr *rmsg;

  char recv_buf[8192];
  struct iovec iova;
  struct msghdr msga = {(void *)&nladdr, sizeof(nladdr),&iova,1,NULL,0,0};

  messages_pending = 1;


  /*
   *now we receive the response
   */

  while (messages_pending)
  {
    memset(recv_buf,0,sizeof(recv_buf));
    iova.iov_base = (void *)recv_buf;
    iova.iov_len = sizeof(recv_buf);

    while (1)
    {
      rc = recvmsg(sd,&msga,0);
      if (rc <= 0)
      {
        LOG_MSG("osapiNetlinkDumpProcess: error receiving arp table contents!  errno = %d\n",errno);
        return L7_FAILURE;
      }

      rmsg = (struct nlmsghdr *)recv_buf;


      /* this should match, but never does. could be tid, but gettid() doesnt work 
      if (rmsg->nlmsg_pid != pid)
      {
        continue;
      }
      */

      if (rmsg->nlmsg_type == NLMSG_DONE)
      {
        return L7_SUCCESS;
      }

      /*
       *make sure its a correct type */
      if ((rmsg->nlmsg_type != rtype) && (rmsg->nlmsg_type != NLMSG_DONE))
      {
        continue;
      }
      if (rmsg->nlmsg_seq != seq)
      {
        LOG_MSG("osapiNetlinkDumpProcess: wrong sequence number, got %d, expected %d!\n",
               rmsg->nlmsg_seq, seq);
        return L7_FAILURE;
      }

      break;
    }/*while 1*/


    buffer_length = rc;

    while (NLMSG_OK(rmsg,buffer_length))
    {
      if(rmsg->nlmsg_type == (L7_ushort16)NLMSG_DONE){
         messages_pending = 0;
         break;
      }
      if (rmsg->nlmsg_type == NLMSG_DONE)
      {
        return L7_SUCCESS;
      }

      if((*callback)(rmsg, arg) != L7_SUCCESS)
        return L7_FAILURE;

      rmsg = NLMSG_NEXT(rmsg,buffer_length);
    }/*while...NLMSG_OK*/
  }/*while messages_pending*/
  return L7_SUCCESS;
}


/**************************************************************************
*
* @purpose  Delete one arp entry discovered during dump
*
* @param    rmgs       pointer to netlink msg header
* @param    arg        pointer to callback info
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
static 
L7_RC_t osapiNbrFlushCallback(struct nlmsghdr *rmsg, void *arg)
{
  int remaining_length, msg_len = 0;
  osapiNetlinkAddrDumpInfo_t *dInfo = (osapiNetlinkAddrDumpInfo_t *)arg;

  struct del_msg
  {
    struct nlmsghdr n;
    char buf[256];
  }dmsg;

  struct ndmsg *neighbor_table;
  struct ndmsg *ndm_flush;
  struct rtattr *neigh_attr, *flush_attr;
  struct iovec iovb;
  struct msghdr msgb = {(void *)&dInfo->nladdr, sizeof(dInfo->nladdr),&iovb,1,NULL,0,0};
  L7_uchar8 addrStr[40];



  /*
   *extract the neighbor table is data
   */
  neighbor_table = NLMSG_DATA(rmsg);
  remaining_length = rmsg->nlmsg_len - NLMSG_LENGTH(sizeof(*neighbor_table));

  if (remaining_length < 0)
  {
     LOG_MSG("osapiNbrFlushCallback(): error! corrupt datagram length!\n");
     return L7_FAILURE;
  }

  neigh_attr = ((struct rtattr*)(((char*)(neighbor_table)) + NLMSG_ALIGN(sizeof(struct ndmsg))));

  /* restrict flush to interface ifName */
  if(dInfo->ifi != 0 && neighbor_table->ndm_ifindex != dInfo->ifi)
  {
     return L7_SUCCESS;
  }

  while (RTA_OK(neigh_attr,remaining_length))
  {
     switch (neigh_attr->rta_type)
     {
     case RTA_SRC:
     case RTA_OIF:
        /*
         *skip it
         */
        break;
     case RTA_DST:
        /* Netlink socket is returning both our neighbor's address and some
         * multicast addresses. Ignore the multicast addresses. */
       {
         L7_uchar8  *dst_v6_addr;
         dst_v6_addr = RTA_DATA(neigh_attr);

         if ((L7_IP6_IS_ADDR_UNSPECIFIED(dst_v6_addr)) ||
             (L7_IP6_IS_ADDR_MULTICAST(dst_v6_addr))) {
           return L7_SUCCESS;
         }

         if(dInfo->addrs && !L7_IP6_IS_ADDR_EQUAL(&dInfo->addrs->in6Addr,(L7_in6_addr_t *)dst_v6_addr)){
           return L7_SUCCESS;
         }
         if(dInfo->addrs)
         {
           dmsg.n.nlmsg_type = RTM_DELNEIGH;
           dmsg.n.nlmsg_flags = NLM_F_REQUEST;
           dmsg.n.nlmsg_seq = osapiNetlinkSequence++;
           ndm_flush = NLMSG_DATA(&dmsg);
           ndm_flush->ndm_family = AF_INET6;
           ndm_flush->ndm_flags = 0;
           ndm_flush->ndm_type = RTM_DELNEIGH;
           ndm_flush->ndm_state = 0;
           ndm_flush->ndm_ifindex = neighbor_table->ndm_ifindex;
           msg_len = NLMSG_ALIGN(sizeof(struct ndmsg));
  
           flush_attr = ((struct rtattr*)(((char*)(ndm_flush)) + NLMSG_ALIGN(sizeof(struct ndmsg))));
           flush_attr->rta_type = RTA_DST;
           flush_attr->rta_len = RTA_LENGTH(sizeof(L7_in6_addr_t));
           memcpy(RTA_DATA(flush_attr), dst_v6_addr, sizeof(L7_in6_addr_t));
           msg_len += RTA_LENGTH(RTA_ALIGN(sizeof(L7_in6_addr_t)));
           dmsg.n.nlmsg_len = NLMSG_LENGTH(msg_len);
           osapiInetNtop(L7_AF_INET6, dst_v6_addr, addrStr, 40);
           iovb.iov_base = (void *)&dmsg.n;
           iovb.iov_len = dmsg.n.nlmsg_len;
           if(sendmsg(dInfo->sd,&msgb,0) < 0)
           {
             LOG_MSG("osapiNbrFlushCallback(): sending delete failed for %s\n",addrStr);
             return L7_SUCCESS;
           }
          }
        }
        break;
     case RTA_IIF:
        /*
         *this is our interface name.  We can now send the delete message
         */
        if(! dInfo->addrs)
        {
          memcpy(&(dmsg.n),rmsg,rmsg->nlmsg_len);
          dmsg.n.nlmsg_type = RTM_DELNEIGH;
          dmsg.n.nlmsg_flags = NLM_F_REQUEST;
          dmsg.n.nlmsg_seq = osapiNetlinkSequence++;
          iovb.iov_base = (void *)&dmsg.n;
          iovb.iov_len = dmsg.n.nlmsg_len;
          if(sendmsg(dInfo->sd,&msgb,0) < 0)
          {
            LOG_MSG("osapiNbrFlushCallback(): sending delete failed\n");
            return L7_SUCCESS;
          }
        }
        break;
     default:
        LOG_MSG("osapiNbrFlushCallback(): panic: unknown RTA message in netlink response\n");
        return L7_FAILURE;
     }/*end switch*/
     neigh_attr = RTA_NEXT(neigh_attr,remaining_length);

  }/*while RTA_OK*/
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Delete ARP entries from the operating system ARP table.
*
* @param    ifName  interface name (null for all interfaces)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
void osapiArpFlush(L7_uchar8 *ifName)
{
  struct ifreq ifr;
  int sd; /*socket descriptior*/
  L7_RC_t rc; /*return code*/
  int sequence;
  struct rtmsg dump_req;
  osapiNetlinkAddrDumpInfo_t dInfo;

  memset(&dInfo, 0, sizeof(osapiNetlinkAddrDumpInfo_t));
  /* get interface index */
  memset(&ifr, 0x00, sizeof (ifr));
  if(0 != ifName)
  {
    strcpy(ifr.ifr_name, ifName);

    sd = socket(AF_INET,SOCK_DGRAM,0);
    if (sd < 0)
    {
      LOG_MSG("osapiArpFlush: could not open socket for ifreq! errno = %d\n",errno);
      close(sd);
      return;
    }

    if(ioctl(sd, SIOCGIFINDEX, (int)&ifr) != 0)
    {
      LOG_MSG("osapiArpFlush: could not get interface %s! errno = %d\n", 
        ifr.ifr_name, errno);
      return;
    }

    close(sd);
  }

  if(osapiNetlinkDumpOpen(&sd, &dInfo.nladdr) != L7_SUCCESS)
  {
    LOG_MSG("osapiArpFlush: failed to open dump socket\n");
    return;
  }

  sequence = osapiNetlinkSequence++;
  dump_req.rtm_family = AF_INET;
  if(osapiNetlinkDumpRequest(sd, sequence, RTM_GETNEIGH, &dump_req, sizeof(dump_req)) != L7_SUCCESS)
  {
    LOG_MSG("osapiArpFlush: failed to send dump request\n");
    close(sd);
    return;
  }


  /*
   *now we process the response
   */
  dInfo.sd = sd;
  dInfo.ifi = ifr.ifr_ifindex;
  rc = osapiNetlinkDumpProcess(sd, sequence, osapiGetpid(), RTM_NEWNEIGH, &dInfo, osapiNbrFlushCallback);


  close(sd);
  return;
}

/*********************************************************************
 * @purpose  Enables an interface
 *
 * @param    ifname         interface name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t osapiIfEnable( L7_uchar8 *ifname )
{
  return ipstkMgmtIfUp( ifname );
}
                                                                                                                                        
/*********************************************************************
 * @purpose  Disables an interface
 *
 * @param    ifname         interface name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t osapiIfDisable( L7_uchar8 *ifname )
{
  return ipstkMgmtIfDown( ifname );
}


/**************************************************************************
* @purpose  Removes the interface from the route table
*
* @param    intf   pointer to the interface name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/
void osapiCleanupIf(const L7_char8 *intf)
{
  /*
   *we don't need to do anything here as
   *modifying the address or state of the interface
   *should make appropriate modifications to the route
   *table
   */

  return;
}

/**************************************************************************
* @purpose  Adds a gateway route
*
* @param    gateway  32bit gateway ip address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    For VxWorks, it seems only the first added gateway is used, no matter how
* @comments    many are in the route table. For this design, there will be only one gateway
* @comments    in the table. If the Service Port is up and the Service Port has a non-zero
* @comments    gateway, the Network gateway will be removed from the table and the Service
* @comments    Port's added. If the Service Port is up but its gateway is zero, the Network's
* @comments    gateway will not be removed.  The bulk of the logic is in simSetServPortIPGateway().
*
* @end
*************************************************************************/
L7_RC_t osapiAddGateway( L7_uint32 gateway, L7_uint32 intIfNum )
{
  return osapiRawMRouteAdd(0, 0, gateway);
}



/**************************************************************************
* @purpose  Deletes a gateway route
*
* @param    gateway  @b{(input)} 32bit gateway ip address
* @param    intIfNum  @b{(input)} interface to gateway
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*
* @end
*************************************************************************/
L7_RC_t osapiDelGateway( L7_uint32 gateway, L7_uint32 intIfNum )
{
#ifndef PC_LINUX_HOST
  return osapiRawMRouteDelete(0, 0, gateway);
#else
  return L7_SUCCESS;
#endif
}


/**************************************************************************
* @purpose  Add a route to a destination that is a network
*
* @param    destination  32bit destination ip address
* @param    gateway  32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*************************************************************************/
L7_RC_t osapiAddNetRoute( L7_uint32 destination, L7_uint32 gateway )
{
  return osapiRawRouteAdd(destination, gateway);
}
/**************************************************************************
* @purpose  Add a route
*
* @param    destination  32bit destination ip address
* @param    gateway  32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*************************************************************************/
L7_RC_t osapiAddRoute( L7_uint32 destination, L7_uint32 gateway )
{
  return osapiRawRouteAdd(destination, gateway);
}
/**************************************************************************
* @purpose  Delete a route
*
* @param    destination  32bit destination ip address
* @param    gateway  32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*************************************************************************/
L7_RC_t osapiDeleteRoute( L7_uint32 destination, L7_uint32 gateway )
{
  return osapiRawRouteDelete(destination, gateway );
}

/**************************************************************************
* @purpose  Add a route
*
* @param    ipAddr    32bit destination ip address
* @param    netmask   32bit network mask
* @param    gateway  32bit gateway ip address
* @param    flags    32bit route flags
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*************************************************************************/
L7_RC_t osapiAddMRoute( L7_uint32 ipAddr, L7_uint32 netmask, L7_uint32 gateway, 
                        L7_uint32 intIfNum, L7_uint32 flags )
{
  int    sock;
  struct rtentry  rtEntry;

  if(flags & L7_RTF_REJECT)
  {
    /* software forwarding code in fastpath handles the ipv4 reject routes
     * so, no need to add the ipv4 reject route in the stack */
    return L7_SUCCESS;
  }

  /* Probably this should be changed to use Netlink sockets... */

  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {

     LOG_MSG("Failed to Open Socket\n");
     return L7_FAILURE;

  }

  memset(&rtEntry, 0x00, sizeof (rtEntry));
  ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_addr.s_addr = osapiHtonl(ipAddr);
  ((struct sockaddr_in *) &rtEntry.rt_dst)->sin_family = AF_INET;
  ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_addr.s_addr = osapiHtonl(gateway);
  ((struct sockaddr_in *) &rtEntry.rt_gateway)->sin_family = AF_INET;
  ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_addr.s_addr = osapiHtonl(netmask);
  ((struct sockaddr_in *) &rtEntry.rt_genmask)->sin_family = AF_INET;

  rtEntry.rt_dev = L7_NULLPTR;
  rtEntry.rt_flags |=RTF_UP;

  if((ipAddr & netmask) != (gateway & netmask))
     rtEntry.rt_flags |= RTF_GATEWAY;

  if((ipAddr == 0) && (netmask ==0))
  {
      if (gateway != 0)
          rtEntry.rt_flags |= RTF_GATEWAY;
  }
  if(flags & L7_RTF_DISCARD)
     rtEntry.rt_flags |= RTF_REJECT;
  if(flags & L7_RTF_REJECT)
     rtEntry.rt_flags |= RTF_REJECT;

  if ((ioctl(sock, SIOCADDRT, &rtEntry) < 0) && (errno != EEXIST)) {

     L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
             "l3intfAddRoute: Failed to Add Route."
             " Error adding a default gateway"
             " in the kernel’s routing table (the function"
             " is really osapiRawMRouteAdd())");
     close(sock);
     return L7_FAILURE;

  }

  close(sock);

  return L7_SUCCESS;
}
/**************************************************************************
* @purpose  Delete a route
*
* @param    ipAddr    32bit destination ip address
* @param    netmask   32bit network mask
* @param    gateway  32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*************************************************************************/
L7_RC_t osapiDeleteMRoute( L7_uint32 ipAddr, L7_uint32 netmask, L7_uint32 gateway, 
                           L7_uint32 intIfNum )
{
  return osapiRawMRouteDelete(ipAddr,netmask,gateway);
}

#ifdef L7_ROUTING_PACKAGE

/*********************************************************************
* @purpose  Set mode of sending ICMPv4 destination unreachables
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv4IcmpSendDestinationUnreachableSet (L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_uchar8 procfs_message[80], ifName[IFNAMSIZ];
  char *val;

   if(osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ) != L7_SUCCESS)
   {
      LOG_MSG("intIfNum %d: unable to get interface name\n", intIfNum);
      return L7_ERROR;
   }
   
   val = (mode == L7_ENABLE)?"1":"0";
   
   (void)osapiSnprintf(procfs_message, sizeof(procfs_message),
           "/proc/sys/net/ipv4/conf/%s/ip_unreachables", ifName);

   if (osapi_proc_set(procfs_message, val)< 0)
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
         "Error in opening file %s."
         " When trying to set the ip_unreachables where this is "
         "not defined in the proc file sysyem. A kernel change "
         "specific to FASTPATH is needed.  ", procfs_message);
     return L7_FAILURE;
   }

   return L7_SUCCESS; 
}
/*********************************************************************
* @purpose  Set mode of sending ICMPv4 Redirects 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv4IcmpRedirectSendSet (L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_uchar8 procfs_message[80], ifName[IFNAMSIZ];
  char *val;

  if(osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ) != L7_SUCCESS)
  {
     LOG_MSG("intIfNum %d: unable to get interface name\n", intIfNum);
     return L7_ERROR;
  }

   val = (mode == L7_ENABLE)?"1":"0";

   (void)osapiSnprintf(procfs_message, sizeof(procfs_message),
           "/proc/sys/net/ipv4/conf/%s/send_redirects", ifName);

   if (osapi_proc_set(procfs_message, val) < 0)
   {
     LOG_MSG("Error in opening file %s \n", procfs_message);
     return L7_FAILURE;
   }

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set mode to ignore ICMPv4 Echo Requests
*
* @param    val      @b{(input)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv4IcmpIgnoreEchoRequestSet ( L7_uint32 mode)
{
  L7_uchar8 procfs_message[80];
   char *val;

   val = (mode == L7_ENABLE)?"1":"0";

   (void)osapiSnprintf(procfs_message, sizeof(procfs_message),
           "/proc/sys/net/ipv4/icmp_echo_ignore_all");

   if (osapi_proc_set(procfs_message, val) < 0 )
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
             "Error in opening file %s"
             " When trying to set the echo-ignore-all"
             " where this variable is not defined in "
             "the proc file sysyem.  ", procfs_message);
     return L7_FAILURE;
   }

   return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the ICMPv4 Rate limit
*
* @param    buckectSize  @b{(input)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv4IcmpRatelimitSet ( L7_uint32 buckectSize, L7_uint32 interval)
{
   L7_uchar8 procfs_message[80];
   char val[20];
   L7_ulong32  rate;
   
   rate = (L7_ulong32) ((HZ * (interval/1000))/buckectSize);
   (void)osapiSnprintf(val, sizeof(val), "%lu", rate); 

   (void)osapiSnprintf(procfs_message, sizeof(procfs_message),
                       "/proc/sys/net/ipv4/icmp_ratelimit");

   if (osapi_proc_set(procfs_message, val) < 0 )
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
             "Error in opening file %s"
             " When trying to set the icmp_ratelimit"
             " where this variable is not defined in "
             "the proc file sysyem.", procfs_message);
     return L7_FAILURE;
   }

   return L7_SUCCESS;
}
#endif

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
/*********************************************************************
* @purpose  Set interface ipv6_enable
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} boolean
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6EnableSet( L7_uchar8 *ifName, L7_BOOL enable)
{
   L7_uchar8 procfs_message[80];
   char *val;

   val = (enable == L7_TRUE)?"1":"0";

   sprintf(procfs_message, 
           "/proc/sys/net/ipv6/conf/%s/ipv6_enable", ifName);
   osapi_proc_set(procfs_message, val);

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set interface ipv6 address autoconfiguration mode
*
* @param    ifName @b{(input)} Interface Name
* @param    enable @b{(input)} if enabled or not
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6AddrAutoConfigSet(L7_uchar8 *ifName, L7_uint32 enable)
{
    L7_uchar8 procfs_path[80];

    memset(procfs_path, 0, 80);
    osapiSnprintf(procfs_path, 79, 
                  "/proc/sys/net/ipv6/conf/%s/autoconf", ifName);
    osapi_proc_set(procfs_path, ((enable == L7_ENABLE) ? "1" : "0"));

    return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Delete all NDP entries from the operating system NDP table.
*
* @param       intIfNum   internal interface number
*
* @returns     none
*
* @comments    
*
* @end
*
*************************************************************************/
void osapiIpv6NdpFlush( L7_uint32 intIfNum, L7_in6_addr_t *ip6Addr)
{
  
  int sd; /*socket descriptior*/
  L7_RC_t rc; /*return code*/
  int sequence;
  struct rtmsg dump_req;
  osapiNetlinkAddrDumpInfo_t dInfo;
  L7_in6_prefix_t pfx;

  memset(&dInfo, 0, sizeof(osapiNetlinkAddrDumpInfo_t));
  /* get interface index */
  if(0 != intIfNum)
  {
    if(ipstkIntIfNumToStackIfIndex(intIfNum, &dInfo.ifi) != L7_SUCCESS)
        return;
  }
  else
  {
     dInfo.ifi = 0;
  }

  if(osapiNetlinkDumpOpen(&sd,&dInfo.nladdr) != L7_SUCCESS)
  {
    LOG_MSG("osapiArpFlush: failed to open dump socket\n");
    return;
  }

  sequence = osapiNetlinkSequence++;
  dump_req.rtm_family = AF_INET6;
  if(osapiNetlinkDumpRequest(sd, sequence, RTM_GETNEIGH, &dump_req, sizeof(dump_req)) != L7_SUCCESS)
  {
    LOG_MSG("osapiArpFlush: failed to send dump request\n");
    close(sd);
    return;
  }


  /*
   *now we process the response
   */
  if(ip6Addr)
  {
    pfx.in6Addr = *ip6Addr;
    dInfo.addrs = &pfx;
  }
  else
  {
    dInfo.addrs = 0;
  }
  dInfo.sd = sd;
  dInfo.maxcount = 0;
  dInfo.acount = 0;
  rc = osapiNetlinkDumpProcess(sd, sequence, osapiGetpid(), RTM_NEWNEIGH, &dInfo, osapiNbrFlushCallback);


  close(sd);
  return;
}


/**************************************************************************
*
* @purpose  Delete one arp entry discovered during dump
*
* @param    rmgs       pointer to netlink msg header
* @param    arg        pointer to callback info
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
static 
L7_RC_t osapiIfIpv6AddrsGetCallback(struct nlmsghdr *rmsg, void *arg)
{
  int remaining_length;
  osapiNetlinkAddrDumpInfo_t *dInfo = (osapiNetlinkAddrDumpInfo_t *)arg;
  struct ifaddrmsg *ifa;
  struct rtattr *addr_attr;
  L7_uchar8 *rta_data;
  L7_in6_prefix_t *addr;



  /*
   *extract the neighbor table is data
   */
  ifa = NLMSG_DATA(rmsg);
  remaining_length = rmsg->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa));

  if (remaining_length < 0)
  {
     LOG_MSG("osapiIfIpv6AddrsGet: error! corrupt datagram length!\n");
     return L7_FAILURE;
  }

  addr_attr = IFA_RTA(ifa);

  /* restrict flush to interface ifName */
  if(dInfo->ifi != 0 && ifa->ifa_index != dInfo->ifi)
  {
     return L7_SUCCESS;
  }
  if(ifa->ifa_family != AF_INET6)
  {
     return L7_SUCCESS;
  }

  while (RTA_OK(addr_attr,remaining_length))
  {
     switch (addr_attr->rta_type)
     {
     case IFA_LOCAL:
     case IFA_ADDRESS:
        if(dInfo->acount < dInfo->maxcount)
        {
           addr = &dInfo->addrs[dInfo->acount];
           addr->in6PrefixLen = ifa->ifa_prefixlen;
           rta_data = RTA_DATA(addr_attr);
           memcpy(&addr->in6Addr,rta_data,16);
           dInfo->acount++;
        }
        break;
     default:
        break;
     }/*end switch*/
     addr_attr = RTA_NEXT(addr_attr,remaining_length);

  }/*while RTA_OK*/
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on interface
*
* @param    ifName  interface name 
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiIfIpv6AddrsGet(L7_uchar8 *ifName, L7_in6_prefix_t *addrs, L7_uint32 *acount)
{
  struct ifreq ifr;
  int sd; /*socket descriptior*/
  L7_RC_t rc; /*return code*/
  int sequence;
  struct rtmsg dump_req;
  osapiNetlinkAddrDumpInfo_t dInfo;

  /* get interface index */
  memset(&ifr, 0x00, sizeof (ifr));
  if(0 != ifName)
  {
    sprintf(ifr.ifr_name,"%s%d", ifName,0);

    sd = socket(AF_INET,SOCK_DGRAM,0);
    if (sd < 0)
    {
      LOG_MSG("osapiIfIpv6AddrsGet: could not open socket for ifreq! errno = %d\n",errno);
      close(sd);
      return L7_FAILURE;
    }

    if(ioctl(sd, SIOCGIFINDEX, (int)&ifr) != 0)
    {
      LOG_MSG("osapiIfIpv6AddrsGet: could not get interface %s! errno = %d\n", 
        ifr.ifr_name, errno);
	  close (sd);
      return L7_FAILURE;
    }

    close(sd);
  }
  else
  {
    return L7_FAILURE;
  }

  if(osapiNetlinkDumpOpen(&sd, &dInfo.nladdr) != L7_SUCCESS)
  {
    LOG_MSG("osapiIfIpv6AddrsGet: failed to open dump socket\n");
    return L7_FAILURE;
  }

  sequence = osapiNetlinkSequence++;
  dump_req.rtm_family = AF_INET6;
  if(osapiNetlinkDumpRequest(sd, sequence, RTM_GETADDR, &dump_req, sizeof(dump_req)) != L7_SUCCESS)
  {
    LOG_MSG("osapiIfIpv6AddrsGet: failed to send dump request\n");
    close(sd);
    return L7_FAILURE;
  }


  /*
   *now we process the response
   */
  dInfo.sd = sd;
  dInfo.ifi = ifr.ifr_ifindex;
  dInfo.addrs = addrs;
  dInfo.maxcount = *acount;
  dInfo.acount = 0;
  rc = osapiNetlinkDumpProcess(sd, sequence, osapiGetpid(), RTM_NEWADDR, &dInfo, osapiIfIpv6AddrsGetCallback);
  *acount = dInfo.acount;


  close(sd);
  return rc;
}


/**************************************************************************
*
* @purpose  Process 1 route, ignore non-matching ifindex
*
* @param    rmgs       pointer to netlink msg header
* @param    arg        pointer to callback info
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
static 
L7_RC_t osapiDefaultRoutersGetCallback(struct nlmsghdr *rmsg, void *arg)
{
  int remaining_length;
  osapiNetlinkAddrDumpInfo_t *dInfo = (osapiNetlinkAddrDumpInfo_t *)arg;
  struct rtmsg *rtm;
  struct rtattr *attr;
  L7_uchar8 *gw = L7_NULL;
  L7_uchar8 *rta_data;
  L7_in6_addr_t *addr;
  L7_uint32 ifIndex = 0;



  /*
   *extract the neighbor table is data
   */
  rtm = NLMSG_DATA(rmsg);
  remaining_length = rmsg->nlmsg_len - NLMSG_LENGTH(sizeof(*rtm));

  if (remaining_length < 0)
  {
     LOG_MSG("osapiIfIpv6AddrsGet: error! corrupt datagram length!\n");
     return L7_FAILURE;
  }

  attr = RTM_RTA(rtm);

  /* restrict to interface ifName */
  if(rtm->rtm_family != AF_INET6)
  {
     return L7_SUCCESS;
  }
  /* default routes only */
  if(rtm->rtm_dst_len != 0)
  {
     return L7_SUCCESS;
  }
  if(dInfo->acount >= dInfo->maxcount){
     return L7_SUCCESS;
  }
    

  while (RTA_OK(attr,remaining_length))
  {
     switch (attr->rta_type)
     {
     case RTA_GATEWAY:
        gw = RTA_DATA(attr);
        break;
     case RTA_OIF:
        rta_data = RTA_DATA(attr);
        ifIndex = *(L7_uint32 *)rta_data;
        break;
     default:
        break;
     }/*end switch*/
     attr = RTA_NEXT(attr,remaining_length);

  }/*while RTA_OK*/

  if((ifIndex == dInfo->ifi) && (gw != L7_NULL)){
     addr = &dInfo->gateways[dInfo->acount];
     memcpy(addr,gw,16);
     dInfo->acount++;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  get list of default routers from stack
*
* @param    ifName       interface name
* @param    addrs        in/out- pointer to where addresses are returned
* @param    acount       in/out- pointer to max addresses (in), returned addresses (out)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    also included on non-v6 build for management ports
*           walks entire routing table. extremely heavy-handed, but unavoidable.
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6DefaultRoutersGet( L7_uchar8 *ifName, L7_in6_addr_t *addrs, L7_uint32 *acount)
{
  struct ifreq ifr;
  int sd; /*socket descriptior*/
  L7_RC_t rc; /*return code*/
  int sequence;
  struct rtmsg dump_req;
  osapiNetlinkAddrDumpInfo_t dInfo;

  /* get interface index */
  memset(&ifr, 0x00, sizeof (ifr));
  sprintf(ifr.ifr_name,"%s%d", ifName,0);

  sd = socket(AF_INET,SOCK_DGRAM,0);
  if (sd < 0)
  {
      LOG_MSG("osapiDefaultRoutersGet: could not open socket for ifreq! errno = %d\n",errno);
      close(sd);
      return L7_FAILURE;
  }

  if(ioctl(sd, SIOCGIFINDEX, (int)&ifr) != 0)
  {
      LOG_MSG("osapiDefaultRoutersGet: could not get interface %s! errno = %d\n", 
        ifr.ifr_name, errno);
      close(sd);
      return L7_FAILURE;
  }

  close(sd);
  if(osapiNetlinkDumpOpen(&sd, &dInfo.nladdr) != L7_SUCCESS)
  {
    LOG_MSG("osapiDefaultRoutersGet: failed to open dump socket\n");
    return L7_FAILURE;
  }

  sequence = osapiNetlinkSequence++;
  dump_req.rtm_family = AF_INET6;
  if(osapiNetlinkDumpRequest(sd, sequence, RTM_GETROUTE, &dump_req, sizeof(dump_req)) != L7_SUCCESS)
  {
    LOG_MSG("osapiDefaultRoutersGet: failed to send dump request\n");
    close(sd);
    return L7_FAILURE;
  }


  /*
   *now we process the response
   */
  dInfo.sd = sd;
  dInfo.ifi = ifr.ifr_ifindex;
  dInfo.gateways = addrs;
  dInfo.maxcount = *acount;
  dInfo.acount = 0;
  rc = osapiNetlinkDumpProcess(sd, sequence, osapiGetpid(), RTM_NEWROUTE, &dInfo, osapiDefaultRoutersGetCallback);
  *acount = dInfo.acount;


  close(sd);
  return rc;
}
#endif /* L7_IPV6_PACKAGE  || L7_IPV6_MGMT_PACKAGE*/

/**************************************************************************
* @purpose  Switch the system network mac address between Burned-In or
*           Locally Administered Mac Address
*
* @param    oldMac  L7_uchar8 pointer to a mac address
* @param    newMac  L7_uchar8 pointer to a mac address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*************************************************************************/
L7_int32 osapiUpdateNetworkMac(L7_uchar8 *newMac)
{

  dtlFdbMacAddrChange(newMac);

  return(L7_SUCCESS);

}



/**************************************************************************
*
* @purpose  get the next IP MIB-II arp entry
*
* @param    *index       starting index to find next index
* @param    *ipaAddr     entry's ip address
* @param    *mac         entry's mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if the next entry does not exist
*
* @comments index of zero is used to start search
*
* @end
*
*************************************************************************/
L7_RC_t osapiM2IpArpTblEntryGet(L7_long32 *index, L7_ulong32 *ipAddr,
                                L7_uchar8 *macAddr)
{

  int rc = 0, cur_index;
  FILE *fp;
  L7_uchar8 cur_ip[4], cur_mac[6], cur_if[IFNAMSIZ];
  L7_uint32 flags;

  if ((fp = fopen("/proc/net/arp","r")) == NULL)
  {
    return(L7_FAILURE);
  }

  /* first skip the header line */
  rc = fscanf(fp,"%*s %*s %*s %*s %*s %*s %*s %*s %*s\n");

  cur_index = 0;
  while (rc != EOF)
  {
    rc = fscanf(fp, "%hhd.%hhd.%hhd.%hhd %*x %x %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %*s %s\n",
		&cur_ip[0], &cur_ip[1], &cur_ip[2], &cur_ip[3], &flags, 
		&cur_mac[0], &cur_mac[1], &cur_mac[2],
		&cur_mac[3], &cur_mac[4], &cur_mac[5], cur_if);
    if (rc != 12)
    {
      continue;
    }
    if (!(flags & ARP_ENTRY_CMPLT_FLAG))
    {
      /* Incomplete entry */
      continue;
    }
    if (!(cur_mac[5] || cur_mac[4] || cur_mac[3] || cur_mac[2] || 
	  cur_mac[1] || cur_mac[0]))
    {
      /* MAC address is 0; entry is incomplete */
      continue;
    }
    if (strncmp(cur_if, L3INTF_DEVICE_NAME, strlen(L3INTF_DEVICE_NAME)) == 0)
    {
      /* Don't return entries for routing interfaces */
      continue;
    }
    /* We have a valid entry at this point. Is this the one we want? */
    if (*index != cur_index)
    {
      cur_index++;
      continue;
    }
    /* It is */
    /* Get IP address in host byte order */
    *ipAddr = (cur_ip[0] << 24) + (cur_ip[1] << 16) + 
      (cur_ip[2] << 8) + cur_ip[3];
    memcpy(macAddr, cur_mac, 6);
    (*index)++;
    fclose(fp);
    return(L7_SUCCESS);
  } /* End loop over file contents */
  fclose(fp);
  /* Did not find a complete entry with index we sought */
  return(L7_FAILURE);
}

/**************************************************************************
*
* @purpose  Enable a socket library function specific to this OS
*
* @param    socketType enum of which library to init
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiSocketLibInit ( OSAPI_SOCKET_LIB_t socketType )
{
  /* This was a special function needed only to ensure that RIP would run
     properly on VxWorks.  It will now silently return L7_SUCCESS. - andyg */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the current link up/down status of the service port
*
* @param    none
*
* @returns  L7_UP
* @returns  L7_DOWN
*
* @end
*********************************************************************/
L7_uint32 osapiServicePortLinkStatusGet ( void )
{
  L7_uchar8 ifName[20];
  L7_BOOL ifUp;

  /* just return L7_DOWN if there is no service port */
  if (cnfgrIsComponentPresent(L7_SERVICE_PORT_PRESENT_ID) == L7_FALSE)
  {
    return L7_DOWN;
  }

  sprintf (ifName, "%s0", bspapiServicePortNameGet());

  /*
  If L7_SERVICE_PORT_IF is an aliased interface, then change name
  to base interface, since aliased interfaces have no physical
  presence and are not counted separately in Linux
  */

  if (strchr(ifName, ':') != NULL)
  {
    *(strchr(ifName, ':')) = '\0';
  }

  if((osapiInterfaceIsUp(ifName,&ifUp) == L7_SUCCESS) && (ifUp == L7_TRUE))
  {
    return L7_UP;
  }
  else
  {
    return L7_DOWN;
  }
}


/*********************************************************************
* @purpose  Retrieves the current link up/down status of the serial port
*
* @param    none
*
* @returns  L7_UP
* @returns  L7_DOWN
*
* @end
*********************************************************************/
L7_uint32 osapiSerialStatusGet ( void )
{
  return( L7_UP ) ; /* VERY HARDWARE DEPENDENT */
}


/**************************************************************************
* @purpose  Sets the IP address for an interface.
*
* @param    ifName   pointer to the interface name
* @param    ipAddr   pointer to a dotted decimal ip address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiNetIPSet( L7_uchar8 *ifName, L7_uint32 ip_addr )

{
  L7_int32 sock;
  struct ifreq ifr;
  L7_RC_t rc = L7_SUCCESS;


  sock = socket(PF_INET, SOCK_DGRAM, 0);

  strncpy(ifr.ifr_name, ifName, (IFNAMSIZ - 1));
  ifr.ifr_name[strlen(ifName)] = '\0';

  ((struct sockaddr_in *)&ifr.ifr_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = osapiHtonl(ip_addr);

  if (ioctl(sock, SIOCSIFADDR, &ifr) == -1)
  {
    /* Don't print a message to the screen if the interface doesn't exist
       because it might not be created yet.  This is a special case that can
       happen during bootup when the dtl0 interface if configured for dhcp.
       ADG */
    if (errno != ENODEV)
    {
      osapi_printf(
                  "osapiNetIPSet: ioctl on %s failed: addr: 0x%x, err: %d\n",
                  ifName, osapiHtonl(ip_addr), errno);
      perror("osapiNetIPSet - SIOCSIFADDR ");
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                    "osapiNetIPSet: ioctl on %s failed: addr: 0x%lx."
                    " Failed trying to set the IP address (in hex as 0x%lx) of the "
                    "interface with Linux name %s, and the interface does not exist."
                    " Sometimes this is a harmless race condition (e.g. we try to set"
                    " address 0 when DHCPing on the network port (dtl0) at bootup, "
                    "before it’s created using TAP). ",
                    ifName, osapiHtonl(ip_addr), osapiHtonl(ip_addr), ifName);
    }
    close (sock);
    return(L7_ERROR);
  }

  if (ip_addr != 0)
  {
    if (ioctl(sock, SIOCGIFNETMASK, &ifr) == -1)
    {
      osapi_printf(
                  "osapiNetIPSet: ioctl on %s failed: addr: 0x%x, err: %d\n",
                  ifName, osapiHtonl(ip_addr), errno);
      perror("osapiNetIPSet - SIOCGIFNETMASK ");

      rc = L7_ERROR;
    }
    else
    {
      ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr.s_addr =
      osapiHtonl(ip_addr & osapiNtohl(((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr));

      ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_family = AF_INET;

      if (ioctl(sock, SIOCSIFBRDADDR, &ifr) == -1)
      {
        osapi_printf("osapiNetIPSet: ioctl on %s failed: %d\n",
                     ifName, errno);
        perror("osapiNetIPSet - SIOCSIFBRDADDR ");

        rc = L7_ERROR;
      }
    }
  }

  if (strchr(ifName, ':') == NULL)
  {
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) == -1)
    {
      osapi_printf("osapiNetIPSet: ioctl on %s failed: %d\n",
                   ifName, errno);
      perror("osapiNetIPSet - SIOCGIFFLAGS ");

      rc = L7_ERROR;
    }

    ifr.ifr_flags |= IFF_UP;

    if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1)
    {
      osapi_printf("osapiNetIPSet: ioctl on %s failed: %d\n",
                   ifName, errno);
      perror("osapiNetIPSet - SIOCSIFFLAGS 2 ");

      rc = L7_ERROR;
    }
  }

  close (sock);

  return(rc);
}

/**************************************************************************
* @purpose  Gets the IP address for an interface.
*
* @param    ifName   @b{(input)} pointer to the interface name
* @param    ipAddr   @b{(input)} pointer to a dotted decimal ip address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*
* @end
*************************************************************************/
L7_RC_t osapiNetIPGetAddr( L7_uchar8 *ifName, L7_uint32 *ipAddr )
{
  return osapiRawIfAddrGet ( ifName, ipAddr );
}
/**************************************************************************
* @purpose  Gets the IP address validity for an interface
*
* @param    ifName   pointer to the interface name
* @param    ipAddr   network order IP address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiNetIPGet( L7_uchar8 *ifName, L7_uint32 ipAddr )
{
  L7_uint32 tmp_addr;
  L7_RC_t rc = 0;
  L7_BOOL ifUp;
  char  tmpIfName[20] = "";

  strcpy(tmpIfName, ifName);


  /*
  Try to get address for tmpIfName
  */
  if( ((rc = osapiRawIfAddrGet( tmpIfName, &tmp_addr )) != L7_SUCCESS ) &&
	(ipAddr == 0))

  {
      /*
      Try base interface if sub-interface doesn't work
      */

      if (strchr(tmpIfName, ':') != NULL){
      
        *(strchr(tmpIfName, ':')) = '\0';
      }

      rc = osapiRawIfAddrGet( tmpIfName, &tmp_addr );

  }

  if (rc == L7_SUCCESS)
  {
    /*
    tmpIfName exists, check if address either matches ipAddr if ipAddr
    is not 0, or is not 0 if ipAddr is 0
    */
    if (((ipAddr != 0) && (tmp_addr == ipAddr)) || 
		((ipAddr == 0) && (tmp_addr != 0)))
    {
      /*
      tmpIfName address same as passed-in or non-zero, so check if it's up
      */

      /*
      LOG_MSG("osapiNetIPGet: same %s address\n", tmpIfName);
      */

      
      if (strchr(tmpIfName, ':') != NULL)
      {
          *(strchr(tmpIfName, ':')) = '\0';
      }

      if((osapiInterfaceIsUp(tmpIfName,&ifUp) == L7_SUCCESS) &&
	 (ifUp == L7_TRUE)){
	      return L7_SUCCESS;
      }
    }
  }


  return L7_FAILURE;
}


/**************************************************************************
*
* @purpose     Set the status of service port default gateway
*
* @param       status        @b{(input)} status
*
* @returns     void
*
* @notes
*
* @end
*
*************************************************************************/
void osapiServPortDefaultRouteEnableSet(L7_uint32 status)
{

  servPortDefaultRouteEnable = status;

}


/**************************************************************************
*
* @purpose     Get the status of serv port default gateway
*
* @param       void
*
* @returns     L7_TRUE      If serv port should not use IP Map def route
* @returns     L7_FALSE     If serv port should use IP Map def route
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 osapiServPortDefaultRouteEnableGet(void)
{

  return servPortDefaultRouteEnable;

}

/**************************************************************************
*
* @purpose     Convert an IP address from a hex value to an ASCII string
*
* @param       ipAddr     @b{(input)}  IP address to be converted (host byte
*                                        order)
* @param       buf        @b{(output)} location to store IP address string
*
* @returns     void
*
* @comments    Caller must provide an output buffer of at least
* @comments    OSAPI_INET_NTOA_BUF_SIZE bytes (as defined in
* @comments    osapi.h).
* @comments    'ipAddr' is supplied in HOST BYTE ORDER
*
* @end
*
*************************************************************************/
void osapiInetNtoa (L7_uint32 ipAddr, L7_uchar8 *buf)
{
  sprintf( buf, "%d.%d.%d.%d",
           (ipAddr & 0xff000000) >> 24,
           (ipAddr & 0x00ff0000) >> 16,
           (ipAddr & 0x0000ff00) >> 8,
           (ipAddr & 0x000000ff) );
}


/* should find a better way to specify these buffer sizes */
unsigned char osapi_ping_tx_buf[84];
unsigned char osapi_ping_rx_buf[104];

unsigned short osapi_inet_cksum(char *buf, int size)
{

  unsigned short *data = (unsigned short *)buf;
  unsigned long accum = 0;
  int i;

  for (i = 0; i < (size / 2); i++)
  {

    accum += *data;

    data++;

  }

  if ((size % 2) == 1)
  {

    buf = (char *)data;

    accum += ((*buf << 8) & 0x0000ff00);

  }

  /* account for overflow */
  accum = (((accum & 0xffff0000) >> 16) + (accum & 0x0000ffff));

  /* do it twice, in case of second overflow */
  accum = (((accum & 0xffff0000) >> 16) + (accum & 0x0000ffff));

  return(~((unsigned short)(accum & 0x0000ffff)));

}

int osapi_ping_rx(int argc, void *argv[])
{

  int rc, sock, fromlen, ip_hdrlen;
  unsigned short id;
  struct sockaddr_in from, *dest;
  L7_ipHeader_t *ip_hdr;
  struct icmp *icmp_hdr;
  L7_uint32 *count;

  if (argc != 4)
  {

    return(-1);

  }

  sock = (int)argv[0];

  id = (unsigned short)(unsigned int)argv[1];

  count = (L7_uint32 *)argv[2];

  dest = (struct sockaddr_in *)argv[3];

  for (;;)
  {

    fromlen = sizeof(from);

    if ((rc = recvfrom(sock, osapi_ping_rx_buf, 104, 0,
                       (struct sockaddr *)&from, &fromlen)) < 0)
    {

      if (errno == EINTR) continue; /* normal */

      osapi_printf("recvfrom() error\n");
      continue;

    }

    ip_hdr = (L7_ipHeader_t *)osapi_ping_rx_buf;

    ip_hdrlen = (ip_hdr->iph_versLen & 0x0f) << 2;

    icmp_hdr = (struct icmp *)(&osapi_ping_rx_buf[ip_hdrlen]);

    if (from.sin_addr.s_addr != dest->sin_addr.s_addr) continue;
    if (osapi_inet_cksum((char *)icmp_hdr, 84) != 0) continue;
    if (icmp_hdr->icmp_type != ICMP_ECHOREPLY) continue;
    if ((L7_ushort16)icmp_hdr->icmp_id != osapiHtons(id)) continue;

    (*count)++;

  }

  return(0); /* should not reach here */

}

/*********************************************************************
* @purpose  The number of L7 ICMP Echo (request) messages sent.
*
* @param    None.
*
* @returns Number of packets sent
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 osapiL7IcmpOutEchosGet()
{
  return L7IcmpOutEchos;
}

/*********************************************************************
* @purpose  To Increment the number of L7 ICMP Echo (request) messages sent.
*
* @param    None.
*
* @returns None.
*
* @comments
*
* @end
*********************************************************************/
void osapiL7IcmpOutEchosInc()
{
  L7IcmpOutEchos++;

  return;
}

/**************************************************************************
* @purpose  Wrapper to Linux Ping
*
* @param    hostName       @b{(input)}  host name (as a string) or ip address (as a string) to ping
* @param    numPackets     @b{(input)}  number of packets to send
* @param    msWait         @b{(input)}  sleep time between two ECHO requests in milliseconds
*
* @returns  Number of packets echoed
* @returns  L7_SUCCESS
*
* @comments    none
*
* @end
*************************************************************************/
L7_uint32 osapiPingTimed(L7_char8 *hostName, L7_int32 numPackets, L7_uint32 msWait)
{

  int rc, i, sock, xmited = 0;
  char *udata;
  struct sockaddr_in dest;
  struct icmp *icmp_hdr;
  L7_uint32 packets_rx = 0;
  L7_int32 rx_task_id;
  void *rx_args[4];

  dest.sin_family = AF_INET;
  if (inet_aton(hostName, &(dest.sin_addr)) == 0)
  {

    LOG_MSG("osapiPingTimed - inet_aton error, host = %s\n", hostName);
    return(L7_ERROR);

  }

  /* should make threadsafe version of this */

  sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

  /* This does not seem to be a requirement and we are having problems with it
     so it is being taken out for the time being.  ADG

  osapiArpFlush ();

  */

  rx_args[0] = (void *)sock;
  rx_args[1] = (void *)(osapiGetpid() & 0xffff); /* change after test */
  rx_args[2] = (void *)&packets_rx;
  rx_args[3] = (void *)&dest;

  /* start receive task */
  if ((rx_task_id = osapiTaskCreate("osapi_ping_rx", osapi_ping_rx,
                                    4, &rx_args,
                                    L7_DEFAULT_STACK_SIZE,
                                    L7_DEFAULT_TASK_PRIORITY,
                                    L7_DEFAULT_TASK_SLICE)) == L7_ERROR)
  {

    close (sock);
    LOG_MSG("osapiPingTimed - osapiTaskCreate error %d\n", errno);
    return(L7_ERROR);

  }

  icmp_hdr = (struct icmp *)osapi_ping_tx_buf;

  icmp_hdr->icmp_type = ICMP_ECHO;
  icmp_hdr->icmp_code = 0;
  icmp_hdr->icmp_id = osapiHtons((unsigned short)(unsigned int)rx_args[1]);

  udata = osapi_ping_tx_buf + sizeof(struct icmp);

  for (i = sizeof(struct icmp); i < 84; i++)
  {

    *udata = i;

    udata++;

  }

  for (i = 0; i < numPackets; i++)
  {

    /* send ICMP Echo Request */
    icmp_hdr->icmp_cksum = 0;
    icmp_hdr->icmp_seq = osapiHtons(xmited++);

    icmp_hdr->icmp_cksum = osapi_inet_cksum((char *)icmp_hdr, 84);

    rc = sendto(sock, osapi_ping_tx_buf, 84, 0,
                (struct sockaddr *)&dest, sizeof(dest));

    if ((rc < 0) || (rc != 84))
    {

      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
               "ping: sendto error."
               " Trouble sending an ICMP echo request packet for "
               "the UI ‘ping’ command. Maybe there was no route to that network.");

    }
    
    L7IcmpOutEchos++;

    osapiSleepMSec(msWait);

  }

  for (i = 0; (i < 10) && (packets_rx < numPackets); i++)
  {

    osapiSleepMSec(msWait);

  }

  /* kill receive task */
  osapiTaskDelete(rx_task_id);

  close (sock);

  return(packets_rx);

}

/**************************************************************************
* @purpose  Wrapper to Linux Ping
*
* @param    hostName       @b{(input)}  host name (as a string) or ip address (as a string) to ping
* @param    numPackets     @b{(input)}  number of packets to send
*
* @returns  Number of packets echoed
* @returns  L7_SUCCESS
*
* @comments    none
*
* @end
*************************************************************************/
L7_uint32 osapiPing(L7_char8 *hostName, L7_int32 numPackets)
{
  return osapiPingTimed(hostName, numPackets, FD_MILLISECONDS_BETWEEN_PINGS);
}


#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  create a tunnel interface in stack.
*
* @param    intIfNum     internal interface number
* @param    srcAddr      source address of tunnel endpoint
* @param    dstAddr      destination address of tunnel endpoint
* @param    ttl          ttl of the outer IP packet
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    this call is independent of creation of routing interface
*           using the tunnel or routing interface for tunnel endpoint
*
* @end
*********************************************************************/
L7_RC_t osapiTunnelIfCreate(L7_uint32 intIfNum, L7_uint32 mode,
                   L7_sockaddr_union_t *srcAddr, L7_sockaddr_union_t *dstAddr,
                       L7_uint32 ttl)
{
    L7_uchar8   ifName[IFNAMSIZ];
    struct ip_tunnel_parm tp;
    struct ifreq ifReq;
    int fd;
    L7_sockaddr_union_t src,dst;
    L7_uchar8 procfs_message[80];

    if(osapiIfNameStringGet(intIfNum, ifName,IFNAMSIZ) != L7_SUCCESS)
        return L7_FAILURE;

    src = *srcAddr;
    dst = *dstAddr;
    /* only v4 tunnels for now */
    if(src.u.sa.sa_family != L7_AF_INET)
        return L7_FAILURE;

    /* must always match */
    if(src.u.sa.sa_family != dst.u.sa.sa_family)
        return L7_FAILURE;

    memset(&ifReq, 0, sizeof(struct ifreq));
    strcpy(ifReq.ifr_name,"sit0");           /* sit0: strange but true */
    ifReq.ifr_ifru.ifru_data = (void *)&tp;
      
    memset(&tp, 0, sizeof(tp));
    strcpy(tp.name, ifName);
    tp.iph.protocol = IPPROTO_IPV6;
    tp.iph.saddr = src.u.sa4.sin_addr.s_addr;
    tp.iph.daddr = dst.u.sa4.sin_addr.s_addr;
    /* from usagi user-land iptunnel.c */
    tp.iph.version = 4;
    tp.iph.ihl = 5;
    tp.iph.ttl = ttl;
    tp.iph.frag_off = htons(0x4000);


    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }
    if (ioctl(fd, SIOCADDTUNNEL, &ifReq) < 0)
    {
        close(fd);
        return L7_FAILURE;
    }
    close(fd);

    osapiSnprintf(procfs_message, 79, "/proc/sys/net/ipv6/conf/%s/ipv6_enable", 
                  ifName);
    osapi_proc_set(procfs_message, "1");
    osapiSnprintf(procfs_message, 79, "/proc/sys/net/ipv6/conf/%s/accept_ra", 
                  ifName);
    osapi_proc_set(procfs_message, "0");
    osapiSnprintf(procfs_message, 79, "/proc/sys/net/ipv6/conf/%s/forwarding", 
                  ifName);
    osapi_proc_set(procfs_message, "1");

    return L7_SUCCESS;
    
}


/*********************************************************************
* @purpose  get the attributes of a tunnel interface in stack.
*
* @param    intIfNum     internal interface number
* @param    pMode        return value for tunnel mode
* @param    pSrcAddr     return value for tunnel source address
* @param    pDstAddr     return value for tunnel destination address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiTunnelIfGet(L7_uint32 intIfNum, L7_uint32 *pMode,
                         L7_sockaddr_union_t *pSrcAddr,
                         L7_sockaddr_union_t *pDstAddr)
{
    L7_uchar8   ifName[IFNAMSIZ];
    struct ip_tunnel_parm tp;
    struct ifreq ifReq;
    int fd;

    if(osapiIfNameStringGet(intIfNum, ifName,IFNAMSIZ) != L7_SUCCESS)
        return L7_FAILURE;

    memset(&ifReq, 0, sizeof(struct ifreq));
    strcpy(ifReq.ifr_name, ifName);
    ifReq.ifr_ifru.ifru_data = (void *)&tp;
      
    memset(&tp, 0, sizeof(tp));
    strcpy(tp.name, ifName);


    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }
    if (ioctl(fd, SIOCGETTUNNEL, &ifReq) < 0)
    {
        close(fd);
        return L7_FAILURE;
    }
    close(fd);

    *pMode = L7_TUNNEL_MODE_6OVER4;
    pSrcAddr->u.sa4.sin_family = L7_AF_INET;
    pSrcAddr->u.sa4.sin_addr.s_addr = tp.iph.saddr;
    pDstAddr->u.sa4.sin_family = L7_AF_INET;
    pDstAddr->u.sa4.sin_addr.s_addr = tp.iph.daddr;

    return L7_SUCCESS;
    
}


/*********************************************************************
* @purpose  delete a tunnel interface in stack.
*
* @param    intIfNum     internal interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    this call is independent of creation of routing interface
*           using the tunnel or routing interface for tunnel endpoint
*
* @end
*********************************************************************/
L7_RC_t osapiTunnelIfDelete(L7_uint32 intIfNum)
{
    L7_uchar8   ifName[IFNAMSIZ];
    struct ifreq  ifReq;
    int fd;

    if(osapiIfNameStringGet(intIfNum, ifName,IFNAMSIZ) != L7_SUCCESS)
        return L7_FAILURE;

    memset(&ifReq, 0, sizeof(struct ifreq));
    strcpy(ifReq.ifr_name,ifName);

    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }
    if (ioctl(fd, SIOCDELTUNNEL, &ifReq) < 0)
    {
        close(fd);
        return L7_FAILURE;
    }
    close(fd);
    return L7_SUCCESS;
    
}
#endif /* L7_ROUTING_PACKAGE */

