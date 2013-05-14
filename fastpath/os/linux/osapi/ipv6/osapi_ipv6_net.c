#ifdef _L7_OS_LINUX_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_ipv6_net.c
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

/*************************************************************

*************************************************************/

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

  #include <l7_common.h>
  #include "defaultconfig.h"
  #include <dtlapi.h>
  #include <osapi.h>
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

#include "ipstk_api.h"

/**************************************************************************
* @purpose  Add a v6 route
*
* @param    network      128bit destination ip address
* @param    prefixlen    length of network prefix
* @param    gateway      128bit gateway ip address
* @param    intIfNum     32bit interface of gateway (if linklocal)
* @param    flags        32bit route flags
* @param    intIfName    interface name (used only when intIfNum is 0)
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t osapiAddMRoute6(L7_in6_addr_t *network, L7_uint32 prefixLen,
                        L7_in6_addr_t *gateway, L7_uint32 intIfNum,
                        L7_uint32 flags, L7_uchar8 *intIfName)
{
   int    sock;
   int rc;
#ifdef L7_ROUTING_PACKAGE
   L7_uchar8 ifName[IFNAMSIZ];
#endif
   L7_uint32 stackIfIndex = 0;
   struct in6_rtmsg rtm;


   if(!network || (prefixLen > 128)){
      return L7_FAILURE;
   }

   if((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {

      return L7_FAILURE;

   }

   memset((L7_uchar8 *)&rtm,0,sizeof(rtm));
   rtm.rtmsg_flags = RTF_UP ;
   rtm.rtmsg_dst = *(struct in6_addr *)network;
   rtm.rtmsg_dst_len = prefixLen;

   /* if direct attach, spoof local route */
   if(flags & L7_RTF_DIRECT_ATTACH)
   {
     if (intIfNum != 0)
     {
#ifdef L7_ROUTING_PACKAGE
         rc = osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ);
         if(rc == L7_SUCCESS)
            ipstkStackIfIndexGet(ifName,&stackIfIndex);
         if(rc != L7_SUCCESS)
         {
            close(sock);
            return L7_FAILURE;
         }
#endif
     }
     else if (intIfName != L7_NULLPTR)
     {
       ipstkStackIfIndexGet(intIfName,&stackIfIndex);
     }
     if(stackIfIndex == 0)
     {
        close(sock);
        return L7_FAILURE;
     }
     gateway = L7_NULL;

   }

   /* if gateway is link local, ifname specified router interface name */
   if(gateway){
      rtm.rtmsg_gateway = *(struct in6_addr *)gateway;
      rtm.rtmsg_flags |=  RTF_GATEWAY;

      if(L7_IP6_IS_ADDR_LINK_LOCAL(gateway))
      {
         if (intIfNum != 0)
         {
#ifdef L7_ROUTING_PACKAGE
             if( osapiIfNameStringGet(intIfNum, ifName,IFNAMSIZ) != L7_SUCCESS){
                close(sock);
                return L7_FAILURE;
             }
             ipstkStackIfIndexGet(ifName,&stackIfIndex);
#endif
          }
          else if (intIfName != L7_NULLPTR)
          {
            ipstkStackIfIndexGet(intIfName,&stackIfIndex);
          }

         if(stackIfIndex == 0){
            close(sock);
            return L7_FAILURE;
         }
         rtm.rtmsg_ifindex = stackIfIndex;
      }
   }
   else{
      rtm.rtmsg_ifindex = 0;
   }

   if(flags & L7_RTF_DISCARD)
      rtm.rtmsg_flags |= RTF_REJECT;
   if(flags & L7_RTF_REJECT)
      rtm.rtmsg_flags |= RTF_REJECT;
   rtm.rtmsg_metric = 16;    /* learned default routes get 1024 */

   rc = ioctl(sock, SIOCADDRT, &rtm);

   close(sock);

   return (rc == 0)?L7_SUCCESS:L7_FAILURE;

}

/**************************************************************************
* @purpose  Delete a v6 route
*
* @param    network      128bit destination ip address
* @param    gateway      128bit gateway ip address
* @param    netmask      128 bit net mask
* @param    intIfNum     32bit interface of gateway (if linklocal)
* @param    flags        32bit route flags
* @param    intIfName    interface name (used only when intIfNum is 0)
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t osapiDelMRoute6(L7_in6_addr_t *network, L7_uint32 prefixLen,
                        L7_in6_addr_t *gateway, L7_uint32 intIfNum,
                        L7_uint32 flags, L7_uchar8 *intIfName)
{
   int    sock;
   int rc;
#ifdef L7_ROUTING_PACKAGE
   L7_uchar8 ifName[IFNAMSIZ];
#endif
   L7_uint32 stackIfIndex = 0;
   struct in6_rtmsg rtm;


   if(!network || (prefixLen > 128)){
      return L7_FAILURE;
   }

   if((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {

      return L7_FAILURE;

   }

   memset((L7_uchar8 *)&rtm,0,sizeof(rtm));
   rtm.rtmsg_flags = RTF_UP ;
   rtm.rtmsg_dst = *(struct in6_addr *)network;
   rtm.rtmsg_dst_len = prefixLen;

   /* if direct attach, spoof local route */
   if(flags & L7_RTF_DIRECT_ATTACH)
   {
     if (intIfNum != 0)
     {
#ifdef L7_ROUTING_PACKAGE
         rc = osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ);
         if(rc == L7_SUCCESS)
            ipstkStackIfIndexGet(ifName,&stackIfIndex);
         if(rc != L7_SUCCESS)
         {
            close(sock);
            return L7_FAILURE;
         }
#endif
     }
     else if (intIfName != L7_NULLPTR)
     {
        ipstkStackIfIndexGet(intIfName,&stackIfIndex);
     }
     if(stackIfIndex == 0)
     {
        close(sock);
        return L7_FAILURE;
     }
     gateway = L7_NULL;

   }

   /* if gateway is link local, ifname specified router interface name */
   if(gateway){
      rtm.rtmsg_gateway = *(struct in6_addr *)gateway;
      rtm.rtmsg_flags |=  RTF_GATEWAY;

      if(L7_IP6_IS_ADDR_LINK_LOCAL(gateway))
      {
         if (intIfNum != 0)
         {
#ifdef L7_ROUTING_PACKAGE
            if( osapiIfNameStringGet(intIfNum, ifName,IFNAMSIZ) != L7_SUCCESS){
               close(sock);
               return L7_FAILURE;
            }
            ipstkStackIfIndexGet(ifName,&stackIfIndex);
#endif
         }
         else if (intIfName != L7_NULLPTR)
         {
            ipstkStackIfIndexGet(intIfName,&stackIfIndex);
         }

         if(stackIfIndex == 0){
            close(sock);
            return L7_FAILURE;
         }
         rtm.rtmsg_ifindex = stackIfIndex;
      }
   }
   else{
      rtm.rtmsg_ifindex = 0;
   }


   if(flags & L7_RTF_DISCARD)
      rtm.rtmsg_flags |= RTF_REJECT;
   if(flags & L7_RTF_REJECT)
      rtm.rtmsg_flags |= RTF_REJECT;
   rtm.rtmsg_metric = 16;    /* learned default routes get 1024 */

   rc = ioctl(sock, SIOCDELRT, &rtm);

   close(sock);

   return (rc == 0)?L7_SUCCESS:L7_FAILURE;

}


/* cant include linux/ipv6.h due to numerous redefs */
struct in6_ifreq {
    struct in6_addr ifr6_addr;
    unsigned long   ifr6_prefixlen;
    int     ifr6_ifindex;
};


/*********************************************************************
* @purpose  Add an ipv6 address/prefixlen to stack
*
* @param    ifname       interface name
* @param    addr         pointer to address
* @param    prefixLen    address prefix length
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    no special status of primary address
*
* @end
*********************************************************************/
L7_RC_t osapiNetAddrV6Add(L7_uchar8 *ifname,
                L7_in6_addr_t  *addr, L7_uint32 prefixlen)
{
    struct in6_ifreq ifreq;
    int fd,ifIndex;

    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    ifreq.ifr6_addr = *(struct in6_addr *)addr;
    ifreq.ifr6_prefixlen = prefixlen;
    if(ipstkStackIfIndexGet(ifname,&ifIndex) != L7_SUCCESS)
    {
        close(fd);
        return L7_FAILURE;
    }
    ifreq.ifr6_ifindex = ifIndex;


    /* in this version, SIFADDR means add addr */
    if (ioctl(fd, SIOCSIFADDR, &ifreq) < 0)
    {
        close(fd);
        return L7_FAILURE;
    }
    close(fd);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete an ipv6 address from stack
*
* @param    ifname       interface name
* @param    addr         pointer to address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    prefixLen not required
*
* @end
*********************************************************************/
L7_RC_t osapiNetAddrV6Del( L7_uchar8 *ifname, L7_in6_addr_t *addr,L7_uint32 prefixlen)
{
    struct in6_ifreq ifreq;
    int fd,ifIndex;

    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return L7_FAILURE;
    }

    ifreq.ifr6_addr = *(struct in6_addr *)addr;
    ifreq.ifr6_prefixlen = prefixlen;
    if(ipstkStackIfIndexGet(ifname,&ifIndex) != L7_SUCCESS)
    {
        close(fd);
        return L7_FAILURE;
    }
    ifreq.ifr6_ifindex = ifIndex;


    if (ioctl(fd, SIOCDIFADDR, &ifreq) < 0)
    {
        close(fd);
        return L7_FAILURE;
    }
    close(fd);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  parse a netlink ipv6 nbr discovery report
*
* @parma    cmsg      @b{(input)}  pointer to netlink message
* @parma    type      @b{(output)}  pointer to message type
* @parma    cmsg      @b{(output)}  pointer to ipv6 address
* @parma    cmsg      @b{(output)}  pointer to internal IfNum
* @parma    cmsg      @b{(output)}  pointer to mac address
*
* @returns  pointer to next netlink message in chain of L7_NULL
*
* @notes    also parses interface address change messages
*
* @end
*********************************************************************/
#include <linux/rtnetlink.h>
L7_uchar8 *osapiNetLinkNDParse(L7_uchar8 *cmsg, L7_uint32 *type, L7_BOOL *is_router,
                               L7_in6_addr_t *naddr,
                               L7_uint32 *intIfNum, L7_uchar8 *mac, L7_uint32 *mac_len)
{
    struct nlmsghdr *nlm = (struct nlmsghdr *)cmsg;
    struct ndmsg *ndm = NLMSG_DATA(nlm);
    struct ifaddrmsg *ifa = NLMSG_DATA(nlm);
    struct rtattr *lla,*dst;
    L7_uint32 dummy = 0;
    L7_uchar8  *dst_v6_addr,*lladdr;
    L7_uint32 paylen;

    switch(nlm->nlmsg_type)
    {
    case RTM_NEWNEIGH:
        if((ndm->ndm_state == NUD_INCOMPLETE) ||
       (ndm->ndm_state == NUD_FAILED))
        {
            *type = L7_NLMSG_NDDEL;
        }
    else{
            *type = L7_NLMSG_NDADD;
        }
        break;
    case RTM_NEWADDR:
        *type = L7_NLMSG_IFADDR_ADD;
        break;
    default:
        *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
    }

    if(*type == L7_NLMSG_IFADDR_ADD)
    {
        if(ifa->ifa_family != AF_INET6){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
        if(ipstkStackIfIndexToIntIfNum(ifa->ifa_index,intIfNum) != L7_SUCCESS){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }

        dst = IFA_RTA(ifa);
        if(dst->rta_type != IFA_ADDRESS){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
        dst_v6_addr = RTA_DATA(dst);
        /* ok */
        memcpy(naddr, dst_v6_addr, sizeof(L7_in6_addr_t));
    }
    else{
        if(ndm->ndm_family != AF_INET6){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
        if(ipstkStackIfIndexToIntIfNum(ndm->ndm_ifindex,intIfNum) != L7_SUCCESS){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
        *is_router = ((ndm->ndm_flags & NTF_ROUTER) !=0)? L7_TRUE:L7_FALSE;

        dst = ((struct rtattr*)(((char*)(ndm)) + NLMSG_ALIGN(sizeof(struct ndmsg))));;
        if(dst->rta_type != NDA_DST){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
        dst_v6_addr = RTA_DATA(dst);

    if(L7_IP6_IS_ADDR_UNSPECIFIED(dst_v6_addr)){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
    if(L7_IP6_IS_ADDR_MULTICAST(dst_v6_addr)){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
        memcpy(naddr, dst_v6_addr, sizeof(L7_in6_addr_t));

    /* ignore lla on delete, may not have one */
    if(*type == L7_NLMSG_NDDEL)
            goto ndp_exit;

        lla = RTA_NEXT(dst,dummy);
        if(lla->rta_type != NDA_LLADDR){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
        lladdr = RTA_DATA(lla);

        paylen = RTA_PAYLOAD(lla);
        if(paylen > *mac_len){
            *type = L7_NLMSG_UNKNOWN;
        goto ndp_exit;
        }
        *mac_len = paylen;


        /* ok */
        memcpy(mac,lladdr,paylen);
    }

ndp_exit:
    /* next msg */
    if(nlm->nlmsg_flags & NLM_F_MULTI){
        nlm = NLMSG_NEXT(nlm,dummy);
        if(nlm->nlmsg_type == NLMSG_DONE)
        nlm = 0;
    }
    else{
    nlm = 0;
    }
    return (L7_uchar8 *)nlm;
}

#ifdef L7_ROUTING_PACKAGE

/*********************************************************************
* @purpose  Turns an NDP netlink report into a request to re-resolve
*
* @parma    daddr       adddress to resolve
* @parma    intIfnum    interface of address if link local
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiNDResolve(L7_in6_addr_t *daddr, L7_uint32 intIfNum)
{

    L7_sockaddr_in6_t to;
    L7_uint32 rc,sock;

    if(osapiSocketCreate(L7_AF_INET6, L7_SOCK_RAW, IPPROTO_ICMPV6,&sock) != L7_SUCCESS)
    {
       return L7_FAILURE;
    }

    memset(&to, 0, sizeof(to));
    to.sin6_family = L7_AF_INET6;
    to.sin6_port = 0;
    to.sin6_scope_id = 0;
    to.sin6_addr = *daddr;

    /* ignore ifIndex for all except link local addres, in which case its used for
       address scope
    */
    if(L7_IP6_IS_ADDR_LINK_LOCAL(&to.sin6_addr))
    {
    if(ipstkIntIfNumToStackIfIndex(intIfNum, &to.sin6_scope_id) != L7_SUCCESS)
    {
             close(sock);
         return L7_FAILURE;
        }
    }

    rc =  ping6SendRequest(sock, &to, 1, 64);

    close(sock);

    return rc;
}
#endif /* L7_ROUTING_PACKAGE */

/*********************************************************************
* @purpose  enables reception of netlink groups
*
* @parma    cmsg  @b{(input)}  pointer to netink message
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiNetLinkSocketIpv6(L7_uint32 *sd)
{
    struct sockaddr_nl nladdr;
    int rc;
    int family;
    L7_uchar8 procfs_message[80];

    /* linux use 548 bytes of internal buffer size to deliver each
       80 byte ndp notification. (kernel does *2 to assign internal rcvbuf_max)
    */
    int bsize = 300*L7_IPV6_NDP_CACHE_SIZE;

    /* also set rmem_max, which controls max socket rx buffer size since notify
      of nbr delete uses 2 pkts per nbr (1 sent to ip6map the other to
      arp listener (netlink cant differentiate). each pkt is received by the app
      as 80 bytes, but uses ~360 bytes of alloc in the kernel. (weird skbuf stuff).
      kernel also doubles the passed in value to account for the skbuf weirdness.
    */
    sprintf(procfs_message, "%d", bsize);
    osapi_proc_set("/proc/sys/net/core/rmem_max", procfs_message);

    /* linux uses a global proto of 0 for all route related stuff */
    family = 0;
    if((*sd = socket(AF_NETLINK, SOCK_RAW,family)) <= 0){
        return L7_FAILURE;
    }
    if(setsockopt(*sd, SOL_SOCKET, SO_RCVBUF, &bsize, sizeof(bsize)) < 0){
        return L7_FAILURE;
    }

    memset(&nladdr,0,sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = osapiGetpid();
    nladdr.nl_groups = RTMGRP_NEIGH | RTMGRP_IPV6_IFADDR;
    rc = bind(*sd,(struct sockaddr *)&nladdr, sizeof(nladdr));

    return (rc <0)?L7_FAILURE:L7_SUCCESS;

}

#ifdef L7_ROUTING_PACKAGE

/*********************************************************************
* @purpose  Set router advertise nd ns interval (ie retransmit)
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6NdNsIntervalSet( L7_uint32 intIfNum, L7_uint32 val)
{
   L7_uchar8 procfs_message[80];
   L7_uchar8 ifName[IFNAMSIZ];
   L7_uchar8 string_val[11];
   if (osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ) != L7_SUCCESS)
   {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
               "intIfNum %d: unable to get interface name\n", intIfNum);
       return L7_ERROR;
   }

#ifdef L7_LINUX_26
   sprintf(procfs_message,
           "/proc/sys/net/ipv6/neigh/%s/retrans_time_ms", &ifName[0]);
#else
   /* For 2.4, time is in userspace-jiffies (100 / second) */
   val /= 10;
   sprintf(procfs_message,
           "/proc/sys/net/ipv6/neigh/%s/retrans_time", &ifName[0]);
#endif
   sprintf(string_val, "%d", val);
   osapi_proc_set(procfs_message, string_val);

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set router advertise nd reachable time
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6NdReachableTimeSet( L7_uint32 intIfNum, L7_uint32 val)
{
   L7_uchar8 procfs_message[80];
   L7_uchar8 ifName[IFNAMSIZ];
   L7_uchar8 string_val[11];

   if (osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ) != L7_SUCCESS)
   {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
               "intIfNum %d: unable to get interface name\n", intIfNum);
       return L7_ERROR;
   }

#ifdef L7_LINUX_26
   sprintf(procfs_message,
           "/proc/sys/net/ipv6/neigh/%s/base_reachable_time_ms", &ifName[0]);
#else
   /* For 2.4, time is in seconds */
   val /= 1000;
   sprintf(procfs_message,
           "/proc/sys/net/ipv6/neigh/%s/base_reachable_time", &ifName[0]);
#endif
   sprintf(string_val, "%d", val);
   osapi_proc_set(procfs_message, string_val);

   return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set dad_transmits
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6DadTransmitsSet( L7_uint32 intIfNum, L7_uint32 val)
{
   L7_uchar8 procfs_message[80];
   L7_uchar8 ifName[IFNAMSIZ];
   L7_uchar8 string_val[11];
   FILE *fd;

   if (osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ) != L7_SUCCESS)
   {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
               "intIfNum %d: unable to get interface name\n", intIfNum);
       return L7_ERROR;
   }

   /* called prior to IFF_UP and procfs may not be there */
   /* if /proc/sys/net/conf/<dev>/dad_transmits does not exist, use the default
      file which will get inherited when the device opens its procfs
   */
   sprintf(procfs_message, "/proc/sys/net/ipv6/conf/%s/dad_transmits", &ifName[0]);
   sprintf(string_val, "%d", val);
   if ((fd = fopen(procfs_message,"r")) == NULL)
   {
       strcpy(procfs_message, "/proc/sys/net/ipv6/conf/default/dad_transmits");
   }
   else{
      fclose(fd);
   }
   osapi_proc_set(procfs_message, string_val);

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set mode of sending ICMPv6 destination unreachables
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
L7_RC_t osapiIfIpv6IcmpSendDestinationUnreachableSet( L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_uchar8 procfs_message[80],ifName[IFNAMSIZ];
  char *val;

   if(osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ) != L7_SUCCESS)
   {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
              "intIfNum %d: unable to get interface name\n", intIfNum);
      return L7_ERROR;

   }

   val = (mode == L7_ENABLE)?"1":"0";
  (void)osapiSnprintf(procfs_message, sizeof(procfs_message),
                      "/proc/sys/net/ipv6/conf/%s/ipv6_unreachables", ifName);

   if (osapi_proc_set(procfs_message, val) < 0)
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
         "Error in Opening file %s."
         " When trying to set the ipv6_unreachables where this variable is not "
         "defined in the proc file sysyem. A kernel change specific to FASTPATH "
         "is needed.", procfs_message);
     return L7_FAILURE;
   }


   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the ICMPv6 Rate limit
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
L7_RC_t osapiIpv6IcmpRatelimitSet ( L7_uint32 buckectSize, L7_uint32 interval)
{
   L7_uchar8 procfs_message[80];
   char val[20];
   L7_ulong32  rate;

   rate = (L7_ulong32) (interval/buckectSize);
   (void)osapiSnprintf(val, sizeof(val), "%lu", rate);

   (void)osapiSnprintf(procfs_message, sizeof(procfs_message),"/proc/sys/net/ipv6/icmp/ratelimit");

   if (osapi_proc_set(procfs_message, val)< 0)
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
            "Error in opening file %s."
            " When trying to set the ratelimit where "
            "this variable is not defined in the proc file system", procfs_message);
     return L7_FAILURE;
   }

   return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the amount of time that a neighbor cache entry remains
*           stale before being garbage collected.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is analogous to the ARP max age time. The draft update to
*           RFC 2461 says "ten minutes or more" should be adequate. Linux 2.6
*           uses 1 minute by default. Linux 2.4 used 1 day by default. This
*           function is introduced to allow us to set a common value (one that
*           approximates our VxWorks behavior).
*
*           Since we forward in hardware, neighbor entries usually sit in
*           stale state.
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6StaleTimeSet(L7_uint32 intIfNum, L7_uint32 val)
{
   L7_uchar8 procfs_message[80];
   L7_uchar8 ifName[IFNAMSIZ];
   L7_uchar8 string_val[11];

   if (osapiIfNameStringGet(intIfNum, ifName, IFNAMSIZ) != L7_SUCCESS)
   {
     L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
     nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_IPV6_MAP_COMPONENT_ID,
             "Failed to set IPv6 neighbor stale time. "
             "Unable to get interface name for interface %s.", ifName);
     return L7_ERROR;
   }

   sprintf(procfs_message, "/proc/sys/net/ipv6/neigh/%s/gc_stale_time", ifName);
   sprintf(string_val, "%d", val);
   if (osapi_proc_set(procfs_message, string_val) != 0)
   {
     L7_uchar8 ifName2[L7_NIM_IFNAME_SIZE + 1];
     nimGetIntfName(intIfNum, L7_SYSNAME, ifName2);
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_IPV6_MAP_COMPONENT_ID,
             "Failed to set IPv6 neighbor stale time on interface %s (%s).",
             ifName2, ifName);
     return L7_FAILURE;
   }

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set ipv6 interface hop-limit
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    linux does this per interface, rest of universe treats as
*           global, so ip6map calls once with 0 for intIfNu and once per
*           enabled interface. Also note that hop_limit from dev is applied to
*           route and changing hop-limit does not affect existing routes.
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6HopLimitSet( L7_uint32 intIfNum, L7_uint32 val)
{
   L7_uchar8 procfs_message[80];
   L7_uchar8 ifName[IFNAMSIZ];
   L7_uchar8 hopLimit[10];

   if(intIfNum == 0){
       return L7_SUCCESS;
   }
   if (osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ) != L7_SUCCESS)
   {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
               "intIfNum %d: unable to get interface name\n", intIfNum);
       return L7_ERROR;
   }

   sprintf(procfs_message, "/proc/sys/net/ipv6/conf/%s/hop_limit", ifName);
   sprintf(hopLimit, "%d", val);
   osapi_proc_set(procfs_message, hopLimit);

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set ipv6 interface mtu
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6MtuSet( L7_uint32 intIfNum, L7_uint32 val)
{
   L7_uchar8 procfs_message[80];
   L7_uchar8 ifName[IFNAMSIZ];
   L7_uchar8 string_val[11];

   if (osapiIfNameStringGet(intIfNum,ifName,IFNAMSIZ) != L7_SUCCESS)
   {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
               "intIfNum %d: unable to get interface name\n", intIfNum);
       return L7_ERROR;
   }

   sprintf(procfs_message,
           "/proc/sys/net/ipv6/conf/%s/mtu", &ifName[0]);
   sprintf(string_val, "%d", val);
   osapi_proc_set(procfs_message, string_val);

   return L7_SUCCESS;
}
#endif /* L7_ROUTING_PACKAGE */


/**************************************************************************
*
* @purpose  Set ndp cache size in stack
*
* @param       val           cache size
*
* @returns     none
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t osapiIpv6NdpCacheSizeSet(L7_uint32 val)
{
   L7_uchar8 procfs_message[80];
   L7_uchar8 thresh[10];

   /* thresh3 is absolute max. thresh2 is level at which garbage collection
      starts. unfortunately, gc deletes all nbs not in incomplete state
      (eg not only stale/probe, etc). So we set thresh2=thresh3.
   */
   strcpy(procfs_message, "/proc/sys/net/ipv6/neigh/default/gc_thresh2");
   sprintf(thresh, "%d", val);
   osapi_proc_set(procfs_message, thresh);
   strcpy(procfs_message, "/proc/sys/net/ipv6/neigh/default/gc_thresh3");
   osapi_proc_set(procfs_message, thresh);
   return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Enables IPV6 on an interface
 *
 * @param    ifname         interface name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t osapiIfIpv6Enable( L7_uchar8 *ifname )
{
  return osapiIfIpv6EnableSet(ifname, L7_TRUE);
}

/*********************************************************************
 * @purpose  Disables IPV6 on an interface
 *
 * @param    ifname         interface name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t osapiIfIpv6Disable( L7_uchar8 *ifname )
{
  return osapiIfIpv6EnableSet(ifname, L7_FALSE);
}

#endif /* _L7_OS_LINUX_ */
