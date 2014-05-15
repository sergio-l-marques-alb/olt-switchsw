/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename ipstkIf.c
*
* @purpose  stack interface manipulation
*
* @componenta ipstack
*
* @comments none
*
* @create 10/08/2004
*
* @author jpickering
* @end
*
**********************************************************************/


#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "l7_socket.h"
#include "ipstk_api.h"
#include "l7_common.h"
#include "l3_comm_structs.h"
#include "osapi_support.h"
#include "osapi_sockdefs.h"
#include "l7_ip6_api.h"
#include "bspapi.h"
#include "dtlapi.h"
#include "simapi.h"

#ifdef L7_ROUTING_PACKAGE
#include "rto_api.h"
#endif


#ifdef _L7_OS_LINUX_
/* cant include ipv6.h due to numerous redefs */
struct in6_ifreq {
    struct in6_addr ifr6_addr;
    unsigned long   ifr6_prefixlen;
    int     ifr6_ifindex;
};
#endif
#ifdef _L7_OS_ECOS_
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#endif


/* Keep track of default gateway in IP stack. */
static L7_uint32 ipstkDefaultGateway = 0;

/* semaphore to synchronize default gateway changes */
static void *ipstkDefGwSema = NULL;


/*********************************************************************
* @purpose  initialize the IP stack mapping layer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkInit(void)
{
  ipstkDefGwSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ipstkDefGwSema == NULL)
    return L7_FAILURE;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Record the default gateway in the stack
*
* @param    defGw   new default gateway
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void ipstkDefaultGatewaySet(L7_uint32 defGw)
{
  ipstkDefaultGateway = defGw;
}

/*********************************************************************
* @purpose  Get the default gateway we believe the stack is currently using.
*
* @param    void
*
* @returns  the IPv4 address of the default gateway
*
* @notes    none
*
* @end
*********************************************************************/
static L7_uint32 ipstkDefaultGatewayGet(void)
{
  return ipstkDefaultGateway;
}


/*********************************************************************
* @purpose  Change the state of a network interface
*
* @param    ifname       name of interface
* @param    flag         name of flag to change
* @param    val          set flag if L7_TRUE, else clear
*
* @returns  L7_SUCCESS,L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ipstkIfFlagChange(L7_uchar8 *ifname, L7_uint32 flag, L7_BOOL val)
{
    struct ifreq ifreq;
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name,ifname);
    if (ioctl(fd, SIOCGIFFLAGS, &ifreq) < 0)
    {
        (void)close(fd);
        return L7_FAILURE;
    }

    if (val)
        ifreq.ifr_flags |= flag;
    else
        ifreq.ifr_flags &= ~flag;
    if (ioctl(fd, SIOCSIFFLAGS, &ifreq) < 0)
    {
        (void)close(fd);
        return L7_FAILURE;
    }
    (void)close(fd);
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the state of a network interface
*
* @param    ifname       name of interface
* @param    flag         name of flag to change
* @param    val          set flag if L7_TRUE, else clear
*
* @returns  L7_SUCCES,L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ipstkIfFlagGet(L7_uchar8 *ifname, L7_uint32 flag, L7_BOOL *val)
{
    struct ifreq ifreq;
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name,ifname);
    if (ioctl(fd, SIOCGIFFLAGS, &ifreq) < 0)
    {
        (void)close(fd);
        return L7_FAILURE;
    }

    if(ifreq.ifr_flags & flag)
        *val = L7_TRUE;
    else
        *val = L7_FALSE;

    (void)close(fd);
    return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Gets an ipv4 interface address from the stack
*
* @param    ifname      interface name
* @param    addr        pointer to address
*
* @returns  L7_SUCCESS, L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfAddrGet( L7_uchar8 *ifname, L7_uint32 *addr)
{
    struct ifreq ifreq;
    struct sockaddr_in *in_addr;
    int fd;

    if (addr == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name,ifname);

    if (ioctl(fd, SIOCGIFADDR, &ifreq) < 0)
    {
        /* SIOCGIFADDR fails if address not set. Just set address to 0 and
         * return success. */
        if (errno == EADDRNOTAVAIL)
        {
            *addr = 0;
            (void) close(fd);
            return L7_SUCCESS;
        }
        else
        {
            (void)close(fd);
            return L7_FAILURE;
        }
    }

    /* Adding address and mask. */
    in_addr = (struct sockaddr_in *) &ifreq.ifr_ifru.ifru_addr;
    *addr = ntohl(in_addr->sin_addr.s_addr);

    (void)close(fd);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the primary ipv4 address in stack
*
* @param    ifname       interface name
* @param    addr         interface address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfAddrSet( L7_uchar8 *ifname, L7_uint32 addr)
{
    struct ifreq ifreq;
    struct sockaddr_in *in_addr;
    int fd;

#if 0
    if (addr == 0)
    {
        return L7_FAILURE;
    }
#endif
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name,ifname);

    /* Adding address and mask. */
    in_addr = (struct sockaddr_in *) &ifreq.ifr_ifru.ifru_addr;
    in_addr->sin_family = AF_INET;
#ifdef _L7_OS_VXWORKS_
    in_addr->sin_len    = sizeof(struct sockaddr_in);
#endif
    in_addr->sin_addr.s_addr   = osapiHtonl(addr);


    if (ioctl(fd, SIOCSIFADDR, &ifreq) < 0)
    {
        (void)close(fd);
        return L7_FAILURE;
    }

    (void)close(fd);
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete the primary ipv4 address in stack
*
* @param    ifname       interface name
* @param    addr         interface address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfAddrDel( L7_uchar8 *ifname, L7_uint32 addr)
{
    struct ifreq ifreq;
    struct sockaddr_in *in_addr;
    int fd;

    if (addr == 0)
    {
        return L7_FAILURE;
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name,ifname);

    /* Adding address and mask. */
    in_addr = (struct sockaddr_in *) &ifreq.ifr_ifru.ifru_addr;
    in_addr->sin_family = AF_INET;
#ifdef _L7_OS_LINUX_
    in_addr->sin_addr.s_addr   = 0;

    /* since the IP addr we are trying to set is 0.0.0.0 we are most
       likely trying to just delete the interface, so don't worry
       if it's already deleted */
    if ((ioctl(fd, SIOCSIFADDR, &ifreq) < 0) && (errno != ENODEV))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
              "%s: Failed to Set Interface IP Address to 0, error %s\n",
              ifname, strerror(errno));
    }
#endif

#ifdef _L7_OS_VXWORKS_
    in_addr->sin_len    = sizeof(struct sockaddr_in);
    in_addr->sin_addr.s_addr   = htonl(addr);


    if (ioctl(fd, SIOCDIFADDR, &ifreq) < 0)
    {
        (void)close(fd);
        return L7_FAILURE;
    }
#endif

    (void)close(fd);
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gets the ipv4 interface mask from the stack
*
* @param    ifname      interface name
* @param    addr        pointer to mask
*
* @returns  L7_SUCCESS, L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfMaskGet( L7_uchar8 *ifname, L7_uint32 *addr)
{
    struct ifreq ifreq;
    struct sockaddr_in *in_addr;
    int fd;

    if (addr == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name,ifname);

    if (ioctl(fd, SIOCGIFNETMASK, &ifreq) < 0)
    {
        (void)close(fd);
        return L7_FAILURE;
    }

    /* Adding address and mask. */
    in_addr = (struct sockaddr_in *) &ifreq.ifr_ifru.ifru_addr;
    *addr = ntohl(in_addr->sin_addr.s_addr);

    (void)close(fd);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the primary ipv4 address mask in stack
*
* @param    ifname       interface name
* @param    addr         interface mask
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfMaskSet( L7_uchar8 *ifname, L7_uint32 addr)
{
    struct ifreq ifreq;
    struct sockaddr_in *in_addr;
    int fd;

#if 0
    if (addr == 0)
    {
        return L7_FAILURE;
    }
#endif
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name,ifname);

    in_addr = (struct sockaddr_in *) &ifreq.ifr_ifru.ifru_addr;
    in_addr->sin_family = AF_INET;
#ifdef _L7_OS_VXWORKS_
    in_addr->sin_len    = sizeof(struct sockaddr_in);
#endif
    in_addr->sin_addr.s_addr   = osapiHtonl(addr);


    if (ioctl(fd, SIOCSIFNETMASK, &ifreq) < 0)
    {
        (void)close(fd);
        return L7_FAILURE;
    }

    (void)close(fd);
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets an interface metric
*
* @param    ifname     interface name
* @param    metric     pointer to metric
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    currently always 0
*
* @end
*********************************************************************/
L7_RC_t ipstkIfMetricGet(L7_uchar8 *ifname, L7_uint32 *metric)
{
    *metric = 0;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Manage the default gateway in the IP stack's routing table.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    FASTPATH can learn a default gateway for the service port, the
*           network port, or a routing interface. The IP stack can only
*           have a single default gateway. (The stack may accept multiple
*           default routes, but if we let that happen we may end up with load
*           balancing across the network and service port or some other
*           combination we don't want.) RTO may report an ECMP default route.
*           We only give the IP stack a single next hop in this case, since
*           it's not likely we need to additional capacity provided by load
*           sharing for packets originating on the box.
*
*           This function is called whenever an event could cause a change
*           in the default route that should be installed in the stack's
*           routing table. Events include user configuration of service
*           port or network port, learning a default gateway through DHCP,
*           management interface up or down, or a default route change in
*           RTO. This function determines what if any default route
*           should be installed given the current system state and gives
*           that gateway to the stack.
*
*           The precedence of default gateways is as follows:
*                 - via routing interface
*                 - via service port
*                 - via network port
*
* @end
*********************************************************************/
L7_RC_t ipstkDefGwUpdate(void)
{
  L7_uint32 defaultGateway = 0;
  L7_uchar8 spName[L7_SERVICE_PORT_MAX_LENGTH + 1];
  L7_uchar8 npName[L7_DTL_PORT_IF_LEN + 1];
  L7_BOOL spUp = L7_FALSE;
  L7_BOOL npUp = L7_FALSE;
  L7_uint32 rtoGateway = 0;  /* default gateway on routing interface */
  L7_uint32 spGateway = 0;   /* default gateway on service port */
  L7_uint32 npGateway = 0;   /* default gateway on network port */
  L7_uint32 intIfNum = 0;    /* outgoing interface for default route */
#ifdef L7_ROUTING_PACKAGE
  L7_routeEntry_t defaultRoute;
#endif

  if (!ipstkDefGwSema ||
      (osapiSemaTake(ipstkDefGwSema, L7_WAIT_FOREVER) != L7_SUCCESS))
    return L7_FAILURE;

  /* Delete existing default gateway */
  defaultGateway = ipstkDefaultGatewayGet();
  if (defaultGateway)
  {
    /* Don't need to specify interface, since we only allow one default route in
     * the stack's routing table. Ignore return code. If gateway was via service
     * port and service port went down, IP stack may delete default gateway
     * before we do. */
    osapiDelGateway(defaultGateway, 0);
  }

  defaultGateway = 0;

#ifdef L7_ROUTING_PACKAGE
  /* Prefer gateway via routing interface. */
  if (rtoPrefixFind(0, 0, &defaultRoute) == L7_SUCCESS)
  {
    rtoGateway = defaultRoute.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr;
    intIfNum = defaultRoute.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
  }
  else
  {
#endif

     /* Get default gateway on service port */
    osapiSnprintf(spName, L7_SERVICE_PORT_MAX_LENGTH, "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
    if ((ipstkIfIsUp(spName, &spUp) == L7_SUCCESS) && spUp)
    {
      spGateway = simGetServPortIPGateway();
    }

    /* Get default gateway on network port. */
    osapiSnprintf(npName, L7_DTL_PORT_IF_LEN + 1, "%s%d", L7_DTL_PORT_IF, 0);
    if ((ipstkIfIsUp(npName, &npUp) == L7_SUCCESS) && npUp)
    {
      npGateway = simGetSystemIPGateway();
    }
#ifdef L7_ROUTING_PACKAGE
  }
#endif

  /* Prefer RTO over service port over network port */
  if (rtoGateway)
  {
    defaultGateway = rtoGateway;
  }
  else if (spGateway)
  {
    defaultGateway = spGateway;
    intIfNum = 0;
  }
  else if (npGateway)
  {
    defaultGateway = npGateway;
    /* Need internal interface number for network interface */
    if (!nimPhaseStatusCheck() ||
        nimFirstValidIntfNumberByType(L7_CPU_INTF, &intIfNum) != L7_SUCCESS)
    {
      osapiSemaGive(ipstkDefGwSema);
      return L7_FAILURE;
    }
  }

  if (defaultGateway)
  {
    if (osapiAddGateway(defaultGateway, intIfNum) != L7_SUCCESS)
    {
      L7_char8 gwStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

      if (defaultGateway == spGateway)
        osapiStrncpySafe(ifName, spName, L7_NIM_IFNAME_SIZE);
      else if (defaultGateway == npGateway)
        osapiStrncpySafe(ifName, npName, L7_NIM_IFNAME_SIZE);
      else
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      osapiInetNtoa(defaultGateway, gwStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to add default gateway %s via interface %s to IP stack",
              gwStr, ifName);
      ipstkDefaultGatewaySet(0);
      osapiSemaGive(ipstkDefGwSema);
      return L7_FAILURE;
    }
  }

  ipstkDefaultGatewaySet(defaultGateway);

  osapiSemaGive(ipstkDefGwSema);
  return L7_SUCCESS;
}


#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
static L7_BOOL ipstkIpv6GwInit = L7_FALSE;
static L7_in6_addr_t ipstkIpv6DGAddr;
static L7_uchar8 ipstkIpv6DGIfName[IFNAMSIZ];
/*********************************************************************
* @purpose  Manage the Ipv6 default gateway in the IP stack's routing table.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    FASTPATH can learn a default gateway for the service port, the
*           network port, or a routing interface. The IP stack can only
*           have a single default gateway. (The stack may accept multiple
*           default routes, but if we let that happen we may end up with load
*           balancing across the network and service port or some other
*           combination we don't want.) RTO6 may report an ECMP default route.
*           We only give the IP stack a single next hop in this case, since
*           it's not likely we need to additional capacity provided by load
*           sharing for packets originating on the box.
*
*           This function is called whenever an event could cause a change
*           in the default route that should be installed in the stack's
*           routing table. Events include user configuration of service
*           port or network port,
*           management interface up or down, or a default route change in
*           RTO6. This function determines what if any default route
*           should be installed given the current system state and gives
*           that gateway to the stack.
*
*           The precedence of default gateways is as follows:
*                 - via routing interface
*                 - via service port
*                 - via network port
*
*           Note that the stack itself may also learn default routes by listening
*           to router advertisements on host interfaces. Such routes are given a higher cost
*           and will not be used if this function provides a route.
*
* @end
*********************************************************************/
L7_RC_t ipstkIpv6DefGwUpdate(void)
{
  L7_in6_addr_t defaultDst;
  L7_uchar8 spName[IFNAMSIZ];
  L7_uchar8 npName[IFNAMSIZ];
  L7_BOOL spUp = L7_FALSE;
  L7_BOOL npUp = L7_FALSE;
  L7_in6_addr_t rto6Gateway;
  L7_in6_addr_t spGateway;
  L7_in6_addr_t npGateway;
  L7_uint32 intIfNum;

  memset(&defaultDst,0, sizeof(defaultDst));

  if (!ipstkDefGwSema ||
      (osapiSemaTake(ipstkDefGwSema, L7_WAIT_FOREVER) != L7_SUCCESS))
    return L7_FAILURE;

  if(ipstkIpv6GwInit == L7_FALSE){
    memset(&ipstkIpv6DGAddr,0, sizeof(defaultDst));
    ipstkIpv6GwInit = L7_TRUE;
  }

  /* Delete existing default gateway */
  if(!L7_IP6_IS_ADDR_UNSPECIFIED(&ipstkIpv6DGAddr)){
    osapiDelMRoute6(&defaultDst,0,&ipstkIpv6DGAddr,0,0,ipstkIpv6DGIfName);
    memset(&ipstkIpv6DGAddr,0, sizeof(defaultDst));

  }

#ifdef L7_IPV6_PACKAGE
  /* Prefer gateway via routing interface. */
  if (rto6DefaultGatewayGet(&rto6Gateway, &intIfNum) == L7_SUCCESS)
  {
    if(osapiIfNameStringGet(intIfNum, ipstkIpv6DGIfName, IFNAMSIZ) == L7_SUCCESS){
          ipstkIpv6DGAddr = rto6Gateway;
    }
  }
#endif

     /* Get default gateway on service port */
  if(L7_IP6_IS_ADDR_UNSPECIFIED(&ipstkIpv6DGAddr)){
    osapiSnprintf(spName, IFNAMSIZ, "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
    if ((ipstkIfIsUp(spName, &spUp) == L7_SUCCESS) && spUp && (simGetServPortIPV6AdminMode() == L7_ENABLE))
    {
      if(simGetServPortIPV6Gateway(&spGateway) == L7_SUCCESS) {
          ipstkIpv6DGAddr = spGateway;
          osapiStrncpySafe(ipstkIpv6DGIfName, spName, IFNAMSIZ);
      }
    }
  }

    /* Get default gateway on network port. */
  if(L7_IP6_IS_ADDR_UNSPECIFIED(&ipstkIpv6DGAddr)){
    osapiSnprintf(npName, IFNAMSIZ , "%s%d", L7_DTL_PORT_IF, 0);
    if ((ipstkIfIsUp(npName, &npUp) == L7_SUCCESS) && npUp && (simGetSystemIPV6AdminMode() == L7_ENABLE))
    {
      if(simGetSystemIPV6Gateway(&npGateway) == L7_SUCCESS){
          ipstkIpv6DGAddr = npGateway;
          osapiStrncpySafe(ipstkIpv6DGIfName, npName, IFNAMSIZ);
      }
    }
  }

  /* now add it if there is one */
  if(!L7_IP6_IS_ADDR_UNSPECIFIED(&ipstkIpv6DGAddr))
  {
    if(osapiAddMRoute6(&defaultDst,0,&ipstkIpv6DGAddr,0,0,ipstkIpv6DGIfName) != L7_SUCCESS)
    {
      L7_char8 gwStr[IPV6_DISP_ADDR_LEN];

      osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ipstkIpv6DGAddr, gwStr, IPV6_DISP_ADDR_LEN);

      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Failed to add IPV6 default gateway %s via interface %s to IP stack",
              gwStr, ipstkIpv6DGIfName);
      osapiSemaGive(ipstkDefGwSema);
      return L7_FAILURE;
    }
  }

  osapiSemaGive(ipstkDefGwSema);
  return L7_SUCCESS;
}
#endif /* defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE) */

/*********************************************************************
* @purpose  tell stack non-router interface is up.
* located here so caller doesnt need to include vendor files
*
* @param    ifname          ascii interface name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkMgmtIfUp(L7_uchar8 *ifname)
{
  L7_RC_t rc;
#ifdef _L7_OS_VXWORKS_
  struct ifreq ifreq;
  int fd;

  /* bump metric for ipv6 rts */
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd < 0)
  {
        return L7_FAILURE;
  }

  memset(&ifreq, 0, sizeof(struct ifreq));
  osapiStrncpySafe(ifreq.ifr_name, ifname, sizeof(ifreq.ifr_name));
  if (ioctl(fd, SIOCGIFMETRIC, (int )&ifreq) == 0){
    ifreq.ifr_ifru.ifru_metric++;
        ioctl(fd, SIOCSIFMETRIC, (int )&ifreq);
  }
  close(fd);
  ipstkIfFlagChange(ifname,IP_IFF_IPV6_ENA, L7_TRUE);
#endif
  rc = ipstkIfFlagChange(ifname,IFF_UP, L7_TRUE);

  if (memcmp(ifname,"lo0",4) != 0)
  {
    ipstkDefGwUpdate();

  }
  return rc;
}

/*********************************************************************
* @purpose  tell stack non-router interface is down.
*
* @param    ifname          ascii interface name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkMgmtIfDown(L7_uchar8 *ifname)
{
  L7_RC_t rc;

  rc = ipstkIfFlagChange(ifname,IFF_UP, L7_FALSE);
  ipstkDefGwUpdate();

  return rc;
}
/*********************************************************************
* @purpose  get stack's interface index
*
* @param    ifname          ascii interface name
* @param    ifIndex         interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkStackIfIndexGet(L7_uchar8 *ifname, L7_uint32 *ifIndex)
{
#ifndef _L7_OS_ECOS_
    struct ifreq ifreq;
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID, "JVM: ipstkStackIfIndexGet (%d): Failed to get interface index from IP stack for interface %s.", __LINE__, ifname);
        return L7_FAILURE;
    }

    memset(&ifreq, 0, sizeof(struct ifreq));
    strcpy(ifreq.ifr_name,ifname);
    if (ioctl(fd, SIOCGIFINDEX, &ifreq) < 0)
    {
        (void)close(fd);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID, "JVM: ipstkStackIfIndexGet (%d): Failed to get interface index from IP stack for interface %s.", __LINE__, ifname);
        return L7_FAILURE;
    }
    *ifIndex = ifreq.ifr_ifindex;

    (void)close(fd);
#endif
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the state of a network interface
*
* @param    ifname       name of interface
* @param    val          ptr to boolean, L7_TRUE if interface UP
*
* @returns  L7_SUCCES,L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ipstkIfIsUp(L7_uchar8 *ifname, L7_BOOL *val)
{
   return ipstkIfFlagGet(ifname, IFF_UP, val);
}


