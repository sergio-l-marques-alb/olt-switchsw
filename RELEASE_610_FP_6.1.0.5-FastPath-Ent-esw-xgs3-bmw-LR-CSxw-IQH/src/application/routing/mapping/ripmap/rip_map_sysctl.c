/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rip_map_sysctl.c
*
* @purpose   RIP sysctl emulation functions
*
* @component RIP Mapping Layer
*
* @comments  none
*
* @create    05/21/2001
*
* @author    gpaussa
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_ripinclude.h"
#ifdef _L7_OS_VXWORKS_
#include <net/if_types.h>
#if VX_VERSION == 65
#include <net/if.h>
#define IFT_ISO88023 1
#endif
#endif
#include <errno.h>
#include "usmdb_common.h"
#include "usmdb_sim_api.h"

#ifdef _L7_OS_LINUX_
#undef AF_LINK
#define AF_LINK 1
#define IFT_ISO88023 1
#define IFT_ETHER 2
#endif

/* common message processing argument structure (from rtsock.c) */
struct walkarg {
  int           w_op;         /* NET_RT_xxx */
  int           w_arg;        /* RTF_xxx for FLAGS, if_index for IFLIST */
  int           w_given;      /* size of process' buffer */
  int           w_needed;     /* #bytes actually needed (at end) */
  int           w_tmemsize;   /* size of buffer pointed to by w_tmem */
  caddr_t       w_where;      /* ptr to process' buffer (maybe null) */
  caddr_t       w_tmem;       /* ptr to our malloc'ed buffer */
};

/* Sleazy use of local variables throughout file, warning!!!! (from rtsock.c) */
#define dst info.rti_info[RTAX_DST]
#define gate    info.rti_info[RTAX_GATEWAY]
#define netmask info.rti_info[RTAX_NETMASK]
#define genmask info.rti_info[RTAX_GENMASK]
#define ifpaddr info.rti_info[RTAX_IFP]
#define ifaaddr info.rti_info[RTAX_IFA]
#define brdaddr info.rti_info[RTAX_BRD]

#ifdef sgi
#define ROUNDUP(a) ((a) > 0 ? (1 + (((a) - 1) | (sizeof(__uint64_t) - 1))) \
            : sizeof(__uint64_t))
#else
#define ROUNDUP(a) ((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) \
            : sizeof(long))
#endif

/* local function prototypes */
static int ripMapSysctlTwo(int *name, u_int namelen, caddr_t where, 
                           size_t *given, void *new, size_t newlen);
static int ripMapSysctl_iflist(int af, struct walkarg *w);
static int ripMapSysctl_rt_msg2(int type, struct rt_addrinfo *rtinfo, 
                                caddr_t cp, struct walkarg *w);


/*********************************************************************
* @purpose  Acts as a wrapper for the un*x sysctl function
*
* @param    *name       pointer to command parameter list
* @param    namelen     number of parameters contained in name list  
* @param    *old        pointer to caller's buffer (can be null)
* @param    *oldlenp    size of caller's buffer
*                       @b{(output) number of bytes contained in response} 
* @param    *new        pointer to a variable to be set (not used here)
* @param    newlen      variable set value (not used here)
*
* @returns  0           Successful
* @returns  -1          Unsuccessful (errno contains error code)
*
* @notes    This wrapper is used primarily for converting an error code
*           into the response normally expected from the system call.
*
* @end
*********************************************************************/
int ripMapSysctl(int *name, u_int namelen, void *old, size_t *oldlenp,
                 void *new, size_t newlen)
{
  int error = EINVAL;

  error = ripMapSysctlTwo(name, namelen, old, oldlenp, new, newlen);

  /* convert an error code into a standard errno with a -1 return value */
  if (error != 0)
  {
    errno = error;
    error = -1;
  }
  return error;
}

/*********************************************************************
* @purpose  Emulates the unix sysctl function and assembles a set of
*           current information for all the routing interfaces configured
*           for RIP operation
*
* @param    *name       pointer to command parameter list
* @param    namelen     number of parameters contained in name list  
* @param    *where      pointer to caller's buffer (can be null)
* @param    *given      size of caller's buffer
*                       @b{(output) number of bytes contained in response} 
* @param    *new        
* @param    newlen
*
* @returns  0           Successful
* @returns  ENOPROTOOPT Invalid protocol usage (only IP supported)
* @returns  EINVAL      Improper parameter value
* @returns  EPERM
* @returns  ENOMEM      Required memory allocation failed
*
* @notes    Designed to only service the NET_RT_IFLIST request issued
*           by the RIP code's rip_ifinit() function.  Builds just the 
*           necessary information needed by RIP.
*
* @notes    For reference, the logic used in this function is derived
*           from sysctl_rtable().  The sysctl() and net_sysctl() functions
*           are replaced with some rudimentary parameter checking.
*
* @notes    Typical usage is to call once with a 'where' parm set to 0 to
*           obtain an estimate of the storage requirements, then call this a 
*           second time after allocating a buffer pointed to by 'where' and
*           with a buffer length specified by the 'given' parm in order to
*           get the data.
*       
* @end
*********************************************************************/
static int ripMapSysctlTwo(int *name, u_int namelen, caddr_t where, 
                           size_t *given, void *new, size_t newlen)
{
    int error = EINVAL;
    u_char  af;
    struct  walkarg w;

  do    /* one-pass error control loop */
  {

  } while (0);
  /* check for expected parm values (like sysctl, net_sysctl) */
  if ((name == L7_NULL) || (namelen != 6) ||
      (name[0] != CTL_NET) || (name[1] != PF_ROUTE) || (name[2] != 0))
    return (EINVAL);

  /* perform sysctl_rtable work */
  name += 3;
  namelen -= 3;

  if (new)
    return (EPERM);
    if (namelen != 3)
        return (EINVAL);
    af = name[0];
  if (af != AF_INET)
    return (ENOPROTOOPT);
    bzero((caddr_t)&w, sizeof(w));
  w.w_where = where;
  w.w_given = *given;
  w.w_needed = 0 - w.w_given;
    w.w_op = name[1];
    w.w_arg = name[2];

    /* s = splnet(); */                   /* LVL7_MOD */
    switch (w.w_op) {

    case NET_RT_IFLIST:
        error = ripMapSysctl_iflist(af, &w);
    break;

  default:
    break;
    }
    /* splx(s); */                        /* LVL7_MOD */
    if (w.w_tmem)
    osapiFree(L7_RIP_MAP_COMPONENT_ID, w.w_tmem);                /* LVL7_MOD */
  w.w_needed += w.w_given;
  if (where) {
    *given = w.w_where - where;
    if (*given < w.w_needed)
      return (ENOMEM);
  } else {
    *given = (w.w_needed * 11) / 10;
  }
    return (error);
}


/*********************************************************************
* @purpose  Prepares the message buffer containing information about all
*           currently configured RIP interfaces
*
* @param    af          address family
* @param    *w          pointer to argument structure
*
* @returns  0           Successful
* @returns  EINVAL      Improper parameter value
* @returns  ENOMEM      Required memory allocation failed
*
* @notes    Similar to sysctl_iflist, this function builds the interface
*           information in the same format as the kernel, with the help
*           of a ported rt_msg2() function.  Only the necessary fields are
*           supplied, as used by the RIP ifinit() function.
*       
* @end
*********************************************************************/
static int ripMapSysctl_iflist(int af, struct walkarg *w)
{
  struct rt_addrinfo info;
    int len;
  L7_BOOL       is_up;

  L7_uint32     intf_type,i, max, count;
  L7_uint32     intIfNum, encapType, flags, metric;
  L7_uchar8     nameStr[NET_IF_DL_DATA_MAX * 2];  /* oversized buffer */
  L7_uint32     nameStrLen;
  struct sockaddr_dl  ifname;
  struct if_data      ifdata;
  L7_RC_t rc;
  L7_uint32 unitIndex;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  bzero((caddr_t)&info, sizeof(info));
  unitIndex = usmDbThisUnitGet();

  /* process each routing interface, only using ones enabled for RIP */
  max = ipmRouterIfMaxCountGet();
  for (i = RIP_MAP_RTR_INTF_FIRST; i <= max; i++)
  {
    /* make sure this is an operable RIP interface */
    if (ripMapRtrIntfIsOperable(i, &intIfNum) != L7_TRUE)
      continue;

    /* only include interfaces considered to be UP for RIP, so that
     * route entries for any DOWN interfaces can be removed by the
     * vendor code (i.e., it no longer sees an existing interface 
     * in the list)
     */
    if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
      continue;

    /* check if request is for a specific routing interface */
    if (w->w_arg && (w->w_arg != i))
      continue;

    /* collect interface information from various parts of the system */
    if (nimEncapsulationTypeGet(intIfNum, &encapType) != L7_SUCCESS)
      continue;

    /* collect interface flags */
    flags = 0;
    if(osapiRouterIfStatusGet(intIfNum, &is_up) != L7_SUCCESS)
      continue;
    if(is_up == L7_TRUE)
      flags |= IFF_UP;
    if(osapiRouterIfTypeGet(intIfNum, &intf_type) != L7_SUCCESS)
      continue;
    if(intf_type & L7_IFF_BROADCAST)
      flags |= IFF_BROADCAST;
    if(intf_type & L7_IFF_LOOPBACK)
      flags |= IFF_LOOPBACK;
    if(intf_type & L7_IFF_POINTOPOINT)
      flags |= IFF_POINTOPOINT;
    if(intf_type & L7_IFF_MULTICAST)
      flags |= IFF_MULTICAST;
    

    if (ipmRouterIfMetricGet(intIfNum, &metric) != L7_SUCCESS)
      continue;
    /* set up the interface data structure (RIP only needs certain counters) */
    /* If the return code is not checked, the variable can contain junk value
       particularly for interfaces for which stats are not kept (i.e. some logical
       interfaces). This was blowing up RIP. So, I am zeroing the values if the return
       code is failure.
    */
    bzero((caddr_t)&ifdata, sizeof(ifdata));
    rc = usmDbStatGet(unitIndex, L7_CTR_RX_TOTAL_FRAMES, intIfNum, (L7_uint32 *)&ifdata.ifi_ipackets);
    if (rc == L7_FAILURE)
    {
      ifdata.ifi_ipackets = 0;
    }
    rc = usmDbStatGet(unitIndex, L7_CTR_RX_TOTAL_ERROR_FRAMES, intIfNum, (L7_uint32 *)&ifdata.ifi_ierrors);
    if (rc == L7_FAILURE)
    {
      ifdata.ifi_ierrors = 0;
    }
    rc = usmDbStatGet(unitIndex, L7_CTR_TX_TOTAL_FRAMES, intIfNum, (L7_uint32 *)&ifdata.ifi_opackets);
    if (rc == L7_FAILURE)
    {
      ifdata.ifi_opackets = 0;
    }
    rc = usmDbStatGet(unitIndex, L7_CTR_TX_TOTAL_ERROR_FRAMES, intIfNum, (L7_uint32 *)&ifdata.ifi_oerrors);
    if (rc == L7_FAILURE)
    {
      ifdata.ifi_oerrors = 0;
    }
    #ifdef sgi
      ifdata.ifi_odiscards = 0;     /* we don't keep this ctr per intf */
    #endif

    /* set up a sockaddr_dl struct with interface name and MAC address */
    bzero((caddr_t)&ifname, sizeof(ifname));
    if ((osapiIfNameStringGet(intIfNum, nameStr,NET_IF_DL_DATA_MAX*2) != L7_SUCCESS) ||
        ((nameStrLen = strlen(nameStr)) > NET_IF_DL_NAME_MAX))
      continue;
    bcopy(nameStr, &ifname.sdl_data[0], nameStrLen);
    if ((L7_ENET_MAC_ADDR_LEN > NET_IF_DL_LLADDR_MAX) ||
        (ipMapRouterIfMacAddressGet(intIfNum, &ifname.sdl_data[nameStrLen]) != L7_SUCCESS))
      continue;
    ifname.sdl_len = sizeof(ifname);
    ifname.sdl_family = AF_LINK;
    ifname.sdl_index = i;
    ifname.sdl_type = (encapType == L7_LL_ENCAP_802) ? IFT_ISO88023 : IFT_ETHER;
    ifname.sdl_nlen = nameStrLen;
    ifname.sdl_alen = L7_ENET_MAC_ADDR_LEN;
    ifname.sdl_slen = 0;

    ifpaddr = (struct sockaddr *)&ifname;
        len = ripMapSysctl_rt_msg2(RTM_IFINFO, &info, (caddr_t)0, w);
        ifpaddr = 0;
        if (w->w_where && w->w_tmem && (w->w_needed <= 0))
    {
            register struct if_msghdr *ifm;

            ifm = (struct if_msghdr *)w->w_tmem;
            ifm->ifm_index = i;
            ifm->ifm_flags = flags;
            ifm->ifm_data = ifdata;
            ifm->ifm_addrs = info.rti_addrs;
      bcopy((caddr_t)ifm, w->w_where, len);
      w->w_where += len;
    }

    /* Add each ip address in the message buffer */
    if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) == L7_SUCCESS)
    {
      struct sockaddr_in  ifaddr, ifmask, ifbcst;

      for (count = 0; count < L7_L3_NUM_IP_ADDRS; count++)
      {
        if (ipAddrList[count].ipAddr == L7_NULL_IP_ADDR)
        {
          continue;
        }

        /* set up three sockaddr_if structs with addressing info for this intf */
        bzero((caddr_t)&ifaddr, sizeof(ifaddr));
        #ifdef _L7_OS_VXWORKS_
          ifaddr.sin_len = sizeof(ifaddr);
        #endif
        ifaddr.sin_family = AF_INET;
        ifaddr.sin_port = osapiHtons(0);   /* not sure what to set here */
        /* copy common fields to other structs */
        bcopy((caddr_t)&ifaddr, (caddr_t)&ifmask, sizeof(ifmask));
        bcopy((caddr_t)&ifaddr, (caddr_t)&ifbcst, sizeof(ifbcst));
        /* set individual address fields (in network byte order) */
        ifaddr.sin_addr.s_addr = osapiHtonl(ipAddrList[count].ipAddr);
        ifmask.sin_addr.s_addr = osapiHtonl(ipAddrList[count].ipMask);
        ifbcst.sin_addr.s_addr = osapiHtonl((ipAddrList[count].ipAddr & 
                                             ipAddrList[count].ipMask) | 
                                            ~(ipAddrList[count].ipMask));

        ifaaddr = (struct sockaddr *)&ifaddr;
        netmask = (struct sockaddr *)&ifmask;
        brdaddr = (struct sockaddr *)&ifbcst;
        len = ripMapSysctl_rt_msg2(RTM_NEWADDR, &info, 0, w);
        if (w->w_where && w->w_tmem && (w->w_needed <= 0))
        {
          register struct ifa_msghdr *ifam;

          ifam = (struct ifa_msghdr *)w->w_tmem;
          ifam->ifam_index = i;
          ifam->ifam_flags = 0;           /* not same as ifm_flags */
          ifam->ifam_metric = metric;
          ifam->ifam_addrs = info.rti_addrs;
          bcopy(w->w_tmem, w->w_where, len);
          w->w_where += len;
        }
      }
    }
    ifaaddr = netmask = brdaddr = 0;
  }

  return (0);
}


/*********************************************************************
* @purpose  Formats interface information in a prescribed message format
*
* @param    type        message type   
* @param    *rtinfo     pointer to socket address list structure
* @param    cp          start of caller's buffer
* @param    *w          pointer to argument structure
*
* @returns  message length
*
* @notes    If cp is null, nothing is stored by this function, but the 
*           the total number of bytes that would have been stored is 
*           returned.  The caller can use this to assess how large of
*           a buffer is needed.  Note, however, that there is no guarantee
*           of the same results for successive calls to this function, since
*           interface conditions can change at any time.
*       
* @end
*********************************************************************/
static int ripMapSysctl_rt_msg2(int type, struct rt_addrinfo *rtinfo, 
                                caddr_t cp, struct walkarg *w)
{
    register int i;
    int len, dlen, second_time = 0;
    caddr_t cp0;

    rtinfo->rti_addrs = 0;
again:
    switch (type) {

    case RTM_DELADDR:
    case RTM_NEWADDR:
        len = sizeof(struct ifa_msghdr);
        break;

    case RTM_IFINFO:
        len = sizeof(struct if_msghdr);
        break;

    default:
        len = sizeof(struct rt_msghdr);
    }
    cp0 = cp;
    if (cp0)
        cp += len;
    for (i = 0; i < RTAX_MAX; i++) {
        register struct sockaddr *sa;

        if ((sa = rtinfo->rti_info[i]) == 0)
            continue;
        rtinfo->rti_addrs |= (1 << i);
        #ifdef _L7_OS_VXWORKS_
        dlen = ROUNDUP(sa->sa_len);
        #endif
        #ifdef _L7_OS_LINUX_
        dlen = sizeof(struct sockaddr);
        #endif
        if (cp) {
            bcopy((caddr_t)sa, cp, (unsigned)dlen);
            cp += dlen;
        }
        len += dlen;
    }
    if (cp == 0 && w != NULL && !second_time) {
        register struct walkarg *rw = w;

    rw->w_needed += len;
        if (rw->w_needed <= 0 && rw->w_where) {
            if (rw->w_tmemsize < len) {
                if (rw->w_tmem)
          osapiFree(L7_RIP_MAP_COMPONENT_ID, rw->w_tmem);        /* LVL7_MOD */
                rw->w_tmem = (caddr_t)
          osapiMalloc(L7_RIP_MAP_COMPONENT_ID, len);             /* LVL7_MOD */
                if (rw->w_tmem)
                    rw->w_tmemsize = len;
            }
            if (rw->w_tmem) {
                cp = rw->w_tmem;
                second_time = 1;
                goto again;
            } else
        rw->w_where = 0;
        }
    }
    if (cp0) {
        register struct rt_msghdr *rtm = (struct rt_msghdr *)cp0;

        rtm->rtm_version = RTM_VERSION;
        rtm->rtm_type = type;
        rtm->rtm_msglen = len;
    }
    return (len);
}

