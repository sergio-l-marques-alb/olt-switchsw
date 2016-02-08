/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_support.c
*
* @purpose   OS independent API's
*
* @component osapi
*
* @comments
*
* @create    11/09/2001
*
* @author    hkumar
*
* @end
*
*********************************************************************/

#include "l7_common.h"
#include <stdlib.h>
#include "osapi.h"
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "osapi_support.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include "l7_socket.h"
#include "ipstk_api.h"
#include "osapi_defs.h"
#include "osapi_mroute.h"
#include "osapi_mroute6.h"
#include "l3end_api.h"

#ifdef L7_RLIM_PACKAGE
#include "rlim_api.h"
#endif

#ifdef PC_LINUX_HOST
#include <linux/types.h>
#endif
#include <linux/icmpv6.h>  /* after l7_socket.h */

#define ERROR (-1)

#ifdef PC_LINUX_HOST
#define L7_SETSOCKOPT_ERROR L7_SUCCESS
#else
#define L7_SETSOCKOPT_ERROR L7_FAILURE
#endif

extern L7_RC_t osapiRawMRouteEntryAdd(L7_uchar8 *ifName, L7_long32 destIp,
             L7_long32 gateIp, L7_long32 mask, L7_uint32 tos,L7_uint32 flags, L7_uint32 proto);
extern L7_RC_t osapiRawMRouteEntryDelete(L7_uchar8 *ifName, L7_long32 destIp,
             L7_long32 gateIp, L7_long32 mask, L7_uint32 tos,L7_uint32 flags, L7_uint32 proto);

extern int osapi_proc_set(const char *path, const char *value);


/**************************************************************************
*
* @purpose  map L7_AF to AF
*
* @param    l7af         L7_AF type
*
* @returns  stack AF
*
* @comments static use only. here since osapi callers may not have access
* to AF_ values of stack. Values are NOT the same.
*
* @end
*
*************************************************************************/
static L7_uint32 osapiFromL7AF(L7_uint32 l7af)
{
  switch(l7af)
  {
  case L7_AF_INET:
      return AF_INET;
  case L7_AF_INET6:
      return AF_INET6;
  default:
      break;
  }
  return 0;
}

/**************************************************************************
*
* @purpose  map AF to L7AF
*
* @param    af         AF type
*
* @returns  L7_AF
*
* @comments static use only
*
* @end
*
*************************************************************************/
static L7_uint32 osapiToL7AF(L7_uint32 af)
{
  switch(af)
  {
  case AF_INET:
      return L7_AF_INET;
  case AF_INET6:
      return L7_AF_INET6;
  default:
      break;
  }
  return 0;
}

/**************************************************************************
*
* @purpose  map  L7 socket type to stack socket type
*
* @param    l7_stype     L7 socket type
*
* @returns  stack socket type
*
* @comments static use only
*
* @end
*************************************************************************/
static L7_uint32 osapiFromL7SockType(L7_uint32 l7_stype)
{
  switch(l7_stype)
  {
  case L7_SOCK_RAW:
      return SOCK_RAW;
  case L7_SOCK_DGRAM:
      return SOCK_DGRAM;
  case L7_SOCK_STREAM:
      return SOCK_STREAM;
  default:
      break;
  }
  return 0;
}

/**************************************************************************
*
* @purpose  Create a socket
*
* @param    domain   @b{(input)}   address family (for example, AF_INET)
* @param    type     @b{(input)}   SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW
* @param    protocol @b{(input)}   protocol to be used
* @param    descriptor      @b{(input)}   ptr to socket descriptor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_uint32 osapiSocketCreate(L7_uint32 domain,L7_uint32 type,L7_uint32 protocol,L7_uint32 *descriptor)
{
  L7_int32 fd;

  domain = osapiFromL7AF(domain);
  type = osapiFromL7SockType(type);

  fd = socket(domain,type,protocol);
  if (fd == ERROR)
  {
    return L7_ERROR;
  }
  *descriptor = fd;
  return L7_SUCCESS;
}


/**************************************************************************
*
* @purpose     Set Broadcast Address on the interface
*
* @param       fd         @b{(input)}  socket descriptor
* @param       intf       @b{(input)}  Ptr to interface name
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiSetBroadAddr(L7_int32 fd,const L7_uchar8 *intf)
{
  return L7_SUCCESS;
}



/**************************************************************************
*
* @purpose  Bind a name to a socket
*
* @param    socketDescriptor  @b{(input)} socket descriptor
* @param    family            @b{(input)} Address Family (for example, AF_INET)
* @param    bindIP            @b{(input)} IP Address to bind with socket
* @param    port              @b{(input)} Port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/

L7_RC_t osapiSocketBind(L7_uint32 socketDescriptor, L7_sockaddr_t *saddr, L7_uint32 s_len)
{
  /* os sockaddr has (ushort sa_family), l7_sockaddr has (u8)sa_len/(u8)sa_family */
  L7_sockaddr_union_t c_saddr;
  struct sockaddr *os_saddr = (struct sockaddr *)&c_saddr;


  if(s_len > sizeof(c_saddr)) return L7_FAILURE;
  memcpy(&c_saddr, saddr, s_len);
  os_saddr->sa_family = (L7_ushort16)osapiFromL7AF(saddr->sa_family);

  if ( bind(socketDescriptor,os_saddr, s_len) == ERROR)
    return L7_FAILURE;
  else
    return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Receive data from a socket
*
* @param      socketDescriptor  @b{(input)} socket to receive from
* @param      buf               @b{(input)} pointer to data buffer
* @param      length            @b{(input)} length of buffer
* @param      flag              @b{(input)} flags to underlying protocols
* @param      family            @b{(output)} Ptr to Address Family for received data
* @param    port              @b{(output)} Ptr to Port
* @param    ipAddr            @b{(output)} Ptr to IP Address
* @param    bytesRcvd         @b{(output)} Ptr to number of bytes received
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*
*************************************************************************/
L7_RC_t osapiSocketRecvfrom(L7_uint32 socketDescriptor, L7_uchar8 *buf, L7_uint32 length,
                              L7_uint32 flag, L7_sockaddr_t *from, L7_uint32 *from_len,
                              L7_uint32 *bytesRcvd)
{
  L7_int32 count;
  /* sa_len/sa_family vs (u16)sa_family */
  struct sockaddr *os_saddr = (struct sockaddr *)from;

  count= recvfrom(socketDescriptor, buf, length,flag, (struct sockaddr *)from,from_len);
  if (count < 0)
  {
    return L7_ERROR;
  }
  *bytesRcvd=count;
  from->sa_family = (L7_uchar8)osapiToL7AF(os_saddr->sa_family);
  from->sa_len = *from_len;

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  receive with packet info
*
* @param    fd                @b{(input)} socket descriptor 
* @param    ptr               @b{(input)} data destination
* @param    nbytes            @b{(in/out)} rx data length
* @param    from              @b{(in/out)} sender address
* @param    from_len          @b{(in/out)} sender address legnth
* @param    to                @b{(in/out)} destination address
* @param    to_len            @b{(in/out)} destination address legnth
* @param    ifIndex           @b{(in/out)} receiving intIfNum
* @param    hops              @b{(in/out)} received hop count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes         Assumes L7_IPPKTINFO sock option set
*                ifIndex is intIfNum, N/A for non routing interfaces
*                To detect Router-Alert, L7_IPRECVOPTS or L7_IPV6_HOPOPTS
*                must be set on the socket
*
*************************************************************************/
L7_RC_t osapiPktInfoRecv(int fd , void *ptr, int *nbytes,int *flagsp,L7_sockaddr_t *from,
                   int *from_len, L7_sockaddr_t *to, int *to_len, L7_uint32 *ifIndex,
           L7_uint32 *hops, L7_BOOL *rtrAlert)
{
    struct msghdr msg;
    /*note: some conflict if leave as iovec */
    struct iovec iov[1];
    int n =0;
    L7_sockaddr_in_t *to_in;
    L7_sockaddr_in6_t *to_in6;
    char *cur_hdr_byte;
    int i;
    struct in_pktinfo *pktinfo_in;
    struct in6_pktinfo *pktinfo_in6;
    struct cmsghdr *cmptr;
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(struct in_addr)) + CMSG_SPACE(sizeof(struct in_pktinfo)) + 
                     CMSG_SPACE(sizeof(struct in_addr)) + CMSG_SPACE(sizeof(int /* hoplim */)) +
                     64 /* misc options req on sock */];

    }   control_un;
    /* sa_len/sa_family vs (u16)sa_family */
    struct sockaddr *os_saddr = (struct sockaddr *)from;

    if (rtrAlert != L7_NULL) {
      *rtrAlert = L7_FALSE; /* Cover most common case by far */
    }
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_name= from;
    msg.msg_namelen= (socklen_t)*from_len;
    msg.msg_flags=0;
    iov[0].iov_base = ptr;
    iov[0].iov_len = *nbytes;

    msg.msg_iov= iov;
    msg.msg_iovlen=1;

    if((n=recvmsg(fd,&msg,*flagsp))<0)
        return (L7_FAILURE);

    *from_len = msg.msg_namelen;

    *flagsp = msg.msg_flags;
    if(msg.msg_controllen < sizeof(struct cmsghdr) ||
            (msg.msg_flags & (MSG_CTRUNC || MSG_TRUNC)) )
        return(L7_FAILURE);
    for(cmptr = CMSG_FIRSTHDR(&msg); cmptr != L7_NULL;
        cmptr = CMSG_NXTHDR(&msg,cmptr)){
        if(cmptr->cmsg_level == IPPROTO_IP &&
           cmptr->cmsg_type == IP_PKTINFO){
            if(*to_len < sizeof(L7_sockaddr_in_t))
                return(L7_FAILURE);
            *to_len = sizeof(L7_sockaddr_in_t);
            memset(to, 0, *to_len);
            to_in = (L7_sockaddr_in_t *)to;
            pktinfo_in = (struct in_pktinfo *)CMSG_DATA(cmptr);
            to_in->sin_family = L7_AF_INET;
            to_in->sin_addr.s_addr = pktinfo_in->ipi_addr.s_addr;
            ipstkStackIfIndexToIntIfNum(pktinfo_in->ipi_ifindex,ifIndex);
        }
        else if(cmptr->cmsg_level == IPPROTO_IPV6 &&
           cmptr->cmsg_type == IPV6_PKTINFO){
            if(*to_len < sizeof(L7_sockaddr_in6_t))
                return(L7_FAILURE);
            *to_len = sizeof(L7_sockaddr_in6_t);
            memset(to, 0, *to_len);
            to_in6 = (L7_sockaddr_in6_t *)to;
            pktinfo_in6 = (struct in6_pktinfo *)CMSG_DATA(cmptr);
            to_in6->sin6_family = L7_AF_INET;
            to_in6->sin6_addr = *(L7_in6_addr_t *)&pktinfo_in6->ipi6_addr;
            ipstkStackIfIndexToIntIfNum(pktinfo_in6->ipi6_ifindex,ifIndex);
        }
        else if(cmptr->cmsg_level == IPPROTO_IPV6 &&
           cmptr->cmsg_type == IPV6_HOPLIMIT){
            *hops = *(int *)CMSG_DATA(cmptr);
        }
	else if(cmptr->cmsg_level == IPPROTO_IPV6 &&
		cmptr->cmsg_type == IPV6_HOPOPTS){
	  if (rtrAlert != L7_NULL) /* If caller doesn't care, neither do we */
	  {
	    cur_hdr_byte = (char *)CMSG_DATA(cmptr);
	    i = 1; /* Skip Next Header byte */
	    while(i < cmptr->cmsg_len)
	    {
	      while (cur_hdr_byte[i] == 0)
	      {
		i++; /* Skip padding */
	      }
	      if (cur_hdr_byte[i] == 5)
	      { /* Found router-alert option */
		*rtrAlert = L7_TRUE;
		break;
	      }
	      i++; /* Skip to length */
	      i += cur_hdr_byte[i]; /* Skip this option */	      
	      i++; /* Go to first byte of next option */
	    }	    
	  }
	} /* end if IPV6_HOPOPTS */
	else if (cmptr->cmsg_level == IPPROTO_IP && 
		 cmptr->cmsg_type == IP_OPTIONS) {
	  if (rtrAlert != L7_NULL) /* If caller doesn't care, neither do we */
	  { 
	    cur_hdr_byte = (char *)CMSG_DATA(cmptr);
	    i = 0;
	    while (i < cmptr->cmsg_len)
	    {
	      if ((cur_hdr_byte[i] & 0x1F) == 0x14)
	      { /* Found Router-Alert option */
		*rtrAlert = L7_TRUE;
		break;
	      }
	      i += cur_hdr_byte[i + 1]; /* Skip to next option */
	    }
	  }
	} /* end if IP_OPTIONS */
    }
    from->sa_family = (L7_uchar8)osapiToL7AF(os_saddr->sa_family);
    from->sa_len = *from_len;
    *nbytes = n;
    return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  receive with packet info with incoming stack if index
*
* @param    fd                @b{(input)} socket descriptor 
* @param    ptr               @b{(input)} data destination
* @param    nbytes            @b{(in/out)} rx data length
* @param    from              @b{(in/out)} sender address
* @param    from_len          @b{(in/out)} sender address legnth
* @param    to                @b{(in/out)} destination address
* @param    to_len            @b{(in/out)} destination address legnth
* @param    stackIfIndex      @b{(in/out)} incoming stack interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiPktIntfRecv(int fd , void *ptr, int *nbytes,int *flagsp,L7_sockaddr_t *from,
                         int *from_len, L7_sockaddr_t *to, int *to_len, L7_uint32 *stackIfIndex)
{
    struct msghdr msg;
    /*note: some conflict if leave as iovec */
    struct iovec iov[1];
    int n =0;
    L7_sockaddr_in_t *to_in;
    L7_sockaddr_in6_t *to_in6;
    struct in_pktinfo *pktinfo_in;
    struct in6_pktinfo *pktinfo_in6;
    struct cmsghdr *cmptr;
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(struct in_addr)) + CMSG_SPACE(sizeof(struct in_pktinfo)) + 
                     CMSG_SPACE(sizeof(struct in_addr)) + CMSG_SPACE(sizeof(int /* hoplim */)) +
                     64 /* misc options req on sock */];

    }   control_un;
    /* sa_len/sa_family vs (u16)sa_family */
    struct sockaddr *os_saddr = (struct sockaddr *)from;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_name= from;
    msg.msg_namelen= (socklen_t)*from_len;
    msg.msg_flags=0;
    iov[0].iov_base = ptr;
    iov[0].iov_len = *nbytes;

    msg.msg_iov= iov;
    msg.msg_iovlen=1;

    if((n=recvmsg(fd,&msg,*flagsp))<0)
        return (L7_FAILURE);

    *from_len = msg.msg_namelen;

    *flagsp = msg.msg_flags;
    if(msg.msg_controllen < sizeof(struct cmsghdr) ||
            (msg.msg_flags & (MSG_CTRUNC || MSG_TRUNC)) )
        return(L7_FAILURE);
    for(cmptr = CMSG_FIRSTHDR(&msg); cmptr != L7_NULL;
        cmptr = CMSG_NXTHDR(&msg,cmptr)){
        if(cmptr->cmsg_level == IPPROTO_IP &&
           cmptr->cmsg_type == IP_PKTINFO){
            if(*to_len < sizeof(L7_sockaddr_in_t))
                return(L7_FAILURE);
            *to_len = sizeof(L7_sockaddr_in_t);
            memset(to, 0, *to_len);
            to_in = (L7_sockaddr_in_t *)to;
            pktinfo_in = (struct in_pktinfo *)CMSG_DATA(cmptr);
            to_in->sin_family = L7_AF_INET;
            to_in->sin_addr.s_addr = pktinfo_in->ipi_addr.s_addr;
            *stackIfIndex = pktinfo_in->ipi_ifindex;
        }
        else if(cmptr->cmsg_level == IPPROTO_IPV6 &&
           cmptr->cmsg_type == IPV6_PKTINFO){
            if(*to_len < sizeof(L7_sockaddr_in6_t))
                return(L7_FAILURE);
            *to_len = sizeof(L7_sockaddr_in6_t);
            memset(to, 0, *to_len);
            to_in6 = (L7_sockaddr_in6_t *)to;
            pktinfo_in6 = (struct in6_pktinfo *)CMSG_DATA(cmptr);
            to_in6->sin6_family = L7_AF_INET;
            to_in6->sin6_addr = *(L7_in6_addr_t *)&pktinfo_in6->ipi6_addr;
            *stackIfIndex = pktinfo_in6->ipi6_ifindex;
        }
    }
    from->sa_family = (L7_uchar8)osapiToL7AF(os_saddr->sa_family);
    from->sa_len = *from_len;
    *nbytes = n;
    return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  send with packet info
*
* @param    fd                @b{(input)} socket descriptor
* @param    buf               @b{(input)} data destination
* @param    bufLen            @b{(input)} tx data length
* @param    flags             @b{(input)} tx flags
* @param    from              @b{(in/out)} sender address
* @param    from_len          @b{(in/out)} sender address legnth
* @param    to                @b{(in/out)} destination address
* @param    to_len            @b{(in/out)} destination address legnth
* @param    intIfNum          @b{(in/out)} transmitting interface number
* @param    hops              @b{(in/out)} received hop count
* @param    bytesSent         @b{(in/out)} tx bytes transmitted
* @param    trafficClass      @b{(input)} Traffic Class in the tx'ed header
* @param    hopLimit          @b{(input)} Hop Limit in the tx'ed header
* @param    nextHdr           @b{(input)} Next Header value
* @param    tcFlag            @b{(input)} If the traffic-class is supplied
* @param    hlFlag            @b{(input)} If the hop-limit is supplied
* @param    nhFlag            @b{(input)} If the next-header is supplied
* @param    rtrAlert          @b{(input)} Send with Router Alert hdr option
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/

L7_RC_t osapiPktInfoSend(L7_uint32 fd,L7_uchar8 *buf,L7_uint32 bufLen,
			 L7_uint32 flags,
                         L7_sockaddr_t *from, L7_uint32 from_len,
                         L7_sockaddr_t *to, L7_uint32 to_len, 
			 L7_uint32 intIfNum,
                         L7_uint32 *bytesSent, L7_uint32 trafficClass,
                         L7_uint32 hopLimit, L7_uint32 nextHdr,
                         L7_BOOL tcFlag, L7_BOOL hlFlag, L7_BOOL nhFlag, 
			 L7_BOOL rtrAlert)
{
    int count;
    int *tC, *hL;
    char *rH;
    struct msghdr msg;
    /*note: some conflict if leave as iovec */
    struct iovec iov[1];
    struct cmsghdr *cmptr;
    struct in_pktinfo *pktinfo_in;
    struct in6_pktinfo *pktinfo_in6;
    struct sockaddr_in6 to6;
    struct sockaddr_in  to4;

    union {
        char control_in4[sizeof(struct cmsghdr) + sizeof(struct in_pktinfo) + 
/* source address / ifindex */
			 sizeof(struct cmsghdr) + 4];
/* router-alert option */ 
        char control_in6[sizeof(struct cmsghdr) + sizeof(struct in6_pktinfo) + /* source address / ifindex */
                         sizeof(struct cmsghdr) + sizeof(int) +                /* traffic class */
                         sizeof(struct cmsghdr) + 8 +
/* router-alert header */
                         sizeof(struct cmsghdr) + sizeof(int)];                /* hop limit */
    } control_un;

    memset (&msg, 0, sizeof (msg));
    msg.msg_namelen = to_len;
    memset (&iov, 0, sizeof (iov));
    iov[0].iov_base = buf;
    iov[0].iov_len = bufLen;
    msg.msg_iov= iov;
    msg.msg_iovlen=1;

    if(to->sa_family == L7_AF_INET)
    {
        L7_sockaddr_in_t *tsa = (L7_sockaddr_in_t *)to;
        
        memcpy(&(to4.sin_addr), &(tsa->sin_addr), sizeof(tsa->sin_addr));
        to4.sin_family = AF_INET;
	to4.sin_port = tsa->sin_port;
        msg.msg_name= &to4;

        memset (control_un.control_in4, 0, sizeof (control_un.control_in4));
        cmptr = (struct cmsghdr *) control_un.control_in4;
        cmptr->cmsg_len = sizeof(struct cmsghdr) + sizeof(struct in_pktinfo);
        cmptr->cmsg_level = IPPROTO_IP;
        cmptr->cmsg_type = IP_PKTINFO;

        msg.msg_control = (void *)control_un.control_in4;
        msg.msg_controllen = sizeof(struct cmsghdr) + sizeof(struct in_pktinfo);
        pktinfo_in = (struct in_pktinfo *)((char *)cmptr + sizeof(struct cmsghdr));
        ipstkIntIfNumToStackIfIndex(intIfNum, &(pktinfo_in->ipi_ifindex));
        memcpy(&(pktinfo_in->ipi_addr), &((L7_sockaddr_in_t *)from)->sin_addr.s_addr, L7_IP_ADDR_LEN);

        if(rtrAlert == L7_TRUE)
        {
            msg.msg_controllen += sizeof(struct cmsghdr) + 4;

            cmptr = CMSG_NXTHDR(&msg, cmptr);
            cmptr->cmsg_len = sizeof(struct cmsghdr) + 4;
            cmptr->cmsg_level = IPPROTO_IP;
            cmptr->cmsg_type = IP_RETOPTS;
            rH = ((char *)cmptr + sizeof(struct cmsghdr));
            rH[0] = 0x94; /* Type: Copied=1, Class=0, Number=0x14 */
	    rH[1] = 4; /* Length */
	    rH[2] = 0; /* 2-byte value: 0=routers, examine this packet */
	    rH[3] = 0;	    
        }
    }
    else if(to->sa_family == L7_AF_INET6)
    {
        L7_sockaddr_in6_t *tsa = (L7_sockaddr_in6_t *)to;

        memcpy(&to6.sin6_addr, &(tsa->sin6_addr), sizeof(tsa->sin6_addr));
        to6.sin6_family = AF_INET6;
	to6.sin6_port = tsa->sin6_port;
	to6.sin6_scope_id = tsa->sin6_scope_id;
	to6.sin6_flowinfo = tsa->sin6_flowinfo;
        msg.msg_name= &to6;

        memset (control_un.control_in6, 0, sizeof (control_un.control_in6));
        cmptr = (struct cmsghdr *) control_un.control_in6;
        cmptr->cmsg_len = sizeof(struct cmsghdr) + sizeof(struct in6_pktinfo);
        cmptr->cmsg_level = IPPROTO_IPV6;
        cmptr->cmsg_type = IPV6_PKTINFO;

        msg.msg_control = (void *)control_un.control_in6;
        msg.msg_controllen = sizeof(struct cmsghdr) + sizeof(struct in6_pktinfo);

        pktinfo_in6 = (struct in6_pktinfo *)((char *)cmptr + sizeof(struct cmsghdr));
        ipstkIntIfNumToStackIfIndex(intIfNum, &(pktinfo_in6->ipi6_ifindex));
        memcpy (&(pktinfo_in6->ipi6_addr), ((L7_sockaddr_in6_t *)from)->sin6_addr.in6.addr8, L7_IP6_ADDR_LEN);

        if(tcFlag == L7_TRUE)
        {
            msg.msg_controllen += sizeof(struct cmsghdr) + sizeof(int);

            cmptr = CMSG_NXTHDR(&msg, cmptr);
            cmptr->cmsg_len = sizeof(struct cmsghdr) + sizeof(int);
            tC = (int *)((char *)cmptr + sizeof(struct cmsghdr));
            *tC = trafficClass;
            cmptr->cmsg_level = IPPROTO_IPV6;
            cmptr->cmsg_type = IPV6_TCLASS;
        }
        if(hlFlag == L7_TRUE)
        {
            msg.msg_controllen += sizeof(struct cmsghdr) + sizeof(int);

            cmptr = CMSG_NXTHDR(&msg, cmptr);
            cmptr->cmsg_len = sizeof(struct cmsghdr) + sizeof(int);
            cmptr->cmsg_level = IPPROTO_IPV6;
            cmptr->cmsg_type = IPV6_HOPLIMIT;
            hL = (int *)((char *)cmptr + sizeof(struct cmsghdr));
            *hL = hopLimit;
        }
        /* Not handling Next Header value field in Linux */
        if(rtrAlert == L7_TRUE)        
	{            
	    msg.msg_controllen += sizeof(struct cmsghdr) + 8;
            cmptr = CMSG_NXTHDR(&msg, cmptr);
            cmptr->cmsg_len = sizeof(struct cmsghdr) + 8;
            cmptr->cmsg_level = IPPROTO_IPV6;
            cmptr->cmsg_type = IPV6_HOPOPTS;
            rH = ((char *)cmptr + sizeof(struct cmsghdr));
	    rH[0] = IPPROTO_ICMPV6; /* next header */
	    rH[1] = 0;
            rH[2] = 5; /* Type: 5 */
	    rH[3] = 2; /* Length */
	    rH[4] = 0; /* 2-byte value: 0=MLD packet */
	    rH[5] = 0;
	    rH[6] = 1; /* Pad option */
	    rH[7] = 0; /* Length */
        }    
    }
    else
    {
      return L7_ERROR;
    }

    if ((count = sendmsg (fd, &msg, flags)) < 0)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
                "OSAPI: sendmsg() failed for fd=%d, intIfNum=%d."
                " A component has attempted to send a UDP packet with" 
                " special options (hop count, etc.) set and failed.", fd, intIfNum);
        return L7_ERROR;
    }
    *bytesSent = count;
    return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Set socket options
*
* @param    targetSocket @b{(input)} socket to receive from
* @param    level        @b{(input)} protocol level of option
* @param    optname      @b{(input)} option name
* @param    optval       @b{(input)} pointer to option value
* @param    optlen       @b{(input)} option length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiSetsockopt(L7_uint32 targetSocket,L7_uint32 level,L7_uint32 optname,
                        L7_uchar8 *optval,L7_uint32 optlen)
{
  /* converts to SOL_SOCKET, IPPROT_xxx vals are IETF defined and dont need
     conversion
  */
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uint32 i;
  struct L7_ip_mreq6_s *l7mreq6 = (struct L7_ip_mreq6_s *)optval;
  struct ipv6_mreq mreq6;
  int realval;
#endif

  if(level == L7_SOL_SOCKET)
  {
    level = SOL_SOCKET;

     /* convert optname */
     switch(optname)
     {
     case L7_SO_REUSEADDR:
        optname = SO_REUSEADDR;
        break;
     case L7_SO_BROADCAST:
        optname = SO_BROADCAST;
        break;
/* tbd jpp
     case L7_SO_REUSEPORT:
        optname = SO_REUSEPORT;
        break;
*/
     case L7_SO_DONTROUTE:
        optname = SO_DONTROUTE;
        break;
     case L7_SO_SNDBUF:
       {
	FILE *max_wmem_fp;
	char buf[11];
	int cur_max_wmem = 0, requested_wmem;

	max_wmem_fp = fopen("/proc/sys/net/core/wmem_max", "r");
	if (max_wmem_fp)
	{
	  requested_wmem = *(int *)optval;
	  fscanf(max_wmem_fp, "%d", &cur_max_wmem);
	  fclose(max_wmem_fp);
	  if (requested_wmem > cur_max_wmem) 
	  {
	    snprintf(buf, 10, "%d", requested_wmem);
	    osapi_proc_set("/proc/sys/net/core/wmem_max", buf);
	  }
	}
        optname = SO_SNDBUF;
	
        break;
       }
     case L7_SO_RCVBUF:
       {
	FILE *max_rmem_fp;
	char buf[11];
	int cur_max_rmem = 0, requested_rmem;

	max_rmem_fp = fopen("/proc/sys/net/core/rmem_max", "r");
	if (max_rmem_fp)
	{
	  requested_rmem = *(int *)optval;
	  fscanf(max_rmem_fp, "%d", &cur_max_rmem);
	  fclose(max_rmem_fp);
	  if (requested_rmem > cur_max_rmem) {
	    snprintf(buf, 10, "%d", requested_rmem);
	    osapi_proc_set("/proc/sys/net/core/rmem_max", buf);
	  }
	}
        optname = SO_RCVBUF;
        break;
       }
     case L7_SO_RCVTIMEO:
        optname = SO_RCVTIMEO;
        break;
     case L7_SO_ERROR:
        optname = SO_ERROR;
        break;
     case L7_SO_TYPE:
        optname = SO_TYPE;
        break;
     case L7_SO_KEEPALIVE:
        optname = SO_KEEPALIVE;
        break;
     case L7_SO_BINDTODEVICE:
        optname = SO_BINDTODEVICE;
        break;
     default:
        return L7_FAILURE;
     }
  }
  else if(level == IPPROTO_IP)
  {
     /* convert optname */
     switch(optname)
     {
     case L7_IP_HDRINCL:
       optname = IP_HDRINCL;
       break;
     case L7_IP_TOS:
        optname = IP_TOS;
        break;
     case L7_IP_TTL:
        optname = IP_TTL;
        break;
     case L7_IP_PKTINFO:
        optname = IP_PKTINFO;
        break;
     case L7_IP_MULTICAST_IF:
        optname = IP_MULTICAST_IF;
        break;
     case L7_IP_MULTICAST_TTL:
        optname = IP_MULTICAST_TTL;
        break;
     case L7_IP_MULTICAST_LOOP:
        optname = IP_MULTICAST_LOOP;
        break;
     case L7_IP_ADD_MEMBERSHIP:
        optname = IP_ADD_MEMBERSHIP;
        break;
     case L7_IP_DROP_MEMBERSHIP:
        optname = IP_DROP_MEMBERSHIP;
        break;
     case L7_IP_RECVTTL:
        optname = IP_RECVTTL;
        break;
     case L7_IP_ROUTER_ALERT:
       /* This socket will receive all forwarded packets that have the 
	  Router Alert header option set */
        optname = IP_ROUTER_ALERT;
        break;
     case L7_IP_OPTIONS:
       /* Datagrams received on this socket by recvmsg() will include a 
	  control message with the IP Options from the header */
       optname = IP_OPTIONS;
       break;
/* tbd jpp
     case L7_IP_RECVIF:
        optname = IP_RECVIF;
        break;
     case L7_IP_DONTFRAG:
        optname = IP_DONTFRAG;
        break;
*/
     default:
        return L7_FAILURE;
     }
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if(level == IPPROTO_IPV6)
  {
     /* convert optname */
     switch(optname)
     {
     case L7_IPV6_UNICAST_HOPS:
          optname = IPV6_UNICAST_HOPS;
          break;
     case L7_IPV6_MULTICAST_IF:
          optname = IPV6_MULTICAST_IF;
          break;
     case L7_IPV6_MULTICAST_HOPS:
          optname = IPV6_MULTICAST_HOPS;
          break;
     case L7_IPV6_MULTICAST_LOOP:
          optname = IPV6_MULTICAST_LOOP;
          break;
     case L7_IPV6_ADD_MEMBERSHIP:
          /* convert mreq to use stack ifindex */
          if(ipstkIntIfNumToStackIfIndex(l7mreq6->imr6_intIfNum,
                                    &mreq6.ipv6mr_interface) != L7_SUCCESS)
               return L7_FAILURE;

          memcpy(&mreq6.ipv6mr_multiaddr, &l7mreq6->imr6_multiaddr, 16);
          optval = ( L7_uchar8 *) &mreq6;
          optlen = sizeof(mreq6);
          optname = IPV6_ADD_MEMBERSHIP;
          break;
     case L7_IPV6_DROP_MEMBERSHIP:

          /* convert mreq to use stack ifindex */
          if(ipstkIntIfNumToStackIfIndex(l7mreq6->imr6_intIfNum,
                                    &mreq6.ipv6mr_interface) != L7_SUCCESS)
               return L7_FAILURE;

          memcpy(&mreq6.ipv6mr_multiaddr, &l7mreq6->imr6_multiaddr, 16);
          optval = ( L7_uchar8 *) &mreq6;
          optlen = sizeof(mreq6);
          optname = IPV6_DROP_MEMBERSHIP;
          break;

     case L7_IPV6_PKTINFO:
          optname = IPV6_PKTINFO;
          break;
     case L7_IPV6_TCLASS:
          optname = IPV6_TCLASS;
          break;
     case L7_IPV6_NEXTHOP:
          optname = IPV6_NEXTHOP;
          break;
     case L7_IPV6_RTHDR:
          optname = IPV6_RTHDR;
          break;
     case L7_IPV6_HOPOPTS:
          optname = IPV6_HOPOPTS;
          break;
     case L7_IPV6_DSTOPTS:
          optname = IPV6_DSTOPTS;
          break;
     case L7_IPV6_ROUTER_ALERT:
       /* This socket will receive all forwarded packets that have the 
	  Router Alert header option set */
          optname = IPV6_ROUTER_ALERT;
       /* Higher layers expect to call with true and false, OS 
	  expects 0 for enable and -1 for disable */
	  if (*(int *)optval) {
	    realval = 0;
	  } else {
	    realval = -1;
	  }
	  optval = (L7_uchar8 *)&realval;
	  optlen = sizeof(realval);
          break;
#if 0
     case L7_IPV6_RTHDRDSTOPTS:
          optname = IPV6_RTHDRDSTOPTS;
          break;
#endif
     /* tbd rfc3542 */
     case L7_IPV6_RECVPKTINFO:
          optname = IPV6_RECVPKTINFO;
          break;
     case L7_IPV6_RECVHOPLIMIT:
          optname = IPV6_RECVHOPLIMIT;
          break;
     case L7_IPV6_CHECKSUM:
          optname = IPV6_CHECKSUM;
          break;
     case L7_IPV6_RECVHOPOPTS:
          optname = IPV6_RECVHOPOPTS;
          break;
#if 0
jpp tbd
     case L7_IPV6_RECVTCLASS:
          optname = IPV6_RECVTCLASS;
          break;
     case L7_IPV6_RECVRTHDR:
          optname = IPV6_RECVRTHDR;
          break;
     case L7_IPV6_RECVDSTOPTS:
          optname = IPV6_RECVDSTOPTS;
          break;
     case L7_IPV6_RECVIF:
          optname = IPV6_RECVIF;
          break;
#endif


     default:
        return L7_FAILURE;
     }
  }
  else if(level == IPPROTO_ICMPV6)
  {
     /* convert optname */
     switch(optname)
     {
     case L7_ICMP6_FILTER:
        optname = ICMPV6_FILTER;
        /* linux does not provide macros for setting filters, but it appears
           that block/pass are opposite of rfc. */
        for(i = 0; i < sizeof(struct icmp6_filter); i++){
            *(optval +i) = ~(*(optval+i));
        }
        break;
     default:
        return L7_FAILURE;
     }
  }
#endif /* L7_IPV6_PACKAGE || L7_IPV6_MGMT_PACKAGE */
  else if(level == IPPROTO_TCP)
  {
     /* convert optname */
     switch(optname)
     {
/* tbd jpp
     case L7_TCP_NODELAY:
        optname = TCP_NODELAY;
        break;
     case L7_TCP_X_MD5_SIGNATURE_KEY:
        optname = IP_TCP_X_MD5_SIGNATURE_KEY;
        break;
*/
     default:
        return L7_FAILURE;
     }
  }
  else
  {
     return L7_FAILURE;
  }


  if (setsockopt(targetSocket,level,optname,optval,optlen) == ERROR)
    return L7_SETSOCKOPT_ERROR;

  return L7_SUCCESS;
}
#ifdef L7_MCAST_PACKAGE
/**************************************************************************
*
* @purpose  Enable/Disable send and receive of all multicast addresses 
*           on a particular socket
*
* @param	targetSocket @b{(input)} socket to receive from 
* @param	level        @b{(input)} protocol level of option 
* @param	initFlag     @b{(input)} Boolean flag for INIT
* @param	optval       @b{(input)} pointer to option value 
* @param	optlen       @b{(input)} option length 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiIpIpv6MRTSet(L7_uint32 targetSocket,L7_uint32 level,
                           L7_BOOL initFlag, L7_uchar8 *optval,
                           L7_uint32 optlen)
{
  L7_uint32 optname;

#ifdef L7_IPV6_PACKAGE
  if(level == IPPROTO_IPV6)
  {
     optname = (initFlag) ? MRT6_INIT : MRT6_DONE;
  }
  else
#endif /* L7_IPV6_PACKAGE */
  {
     /* Not implemented for ipv4 */
     return L7_FAILURE;
  }

  if (setsockopt(targetSocket,level,optname,optval,optlen) == ERROR)
     return L7_SETSOCKOPT_ERROR;

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Enable reception / transmit of all multicast frames on 
*           an interface
*
* @param	targetSocket @b{(input)} socket to receive from 
* @param	level        @b{(input)} protocol level of option 
* @param	addFlag      @b{(input)} Boolean flag for Addition/Deletion
* @param        intIfNum     @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiIpIpv6MRTMifSet(L7_uint32 targetSocket, L7_uint32 level,
                             L7_BOOL addFlag, L7_uint32 intIfNum)
{
#ifdef L7_IPV6_PACKAGE
  L7_uint32 optname;

  L7_uint32 stkIfNum, rtrIfNum;
  struct mif6ctl mifctl;
  mifi_t mifi;

  if (ipstkIntIfNumToStackIfIndex(intIfNum, &stkIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipstkIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
     return L7_FAILURE;

  if(level == IPPROTO_IPV6)
  {
     optname = (addFlag) ? MRT6_ADD_MIF : MRT6_DEL_MIF;

     if(optname == MRT6_ADD_MIF)
     {
        memset(&mifctl, 0, sizeof(struct mif6ctl));
        mifctl.mif6c_mifi = rtrIfNum - 1;
        mifctl.mif6c_pifi = stkIfNum;

        if (setsockopt(targetSocket, level, optname, 
		       (L7_uchar8 *) &mifctl,sizeof(mifctl)) == ERROR)
           return L7_SETSOCKOPT_ERROR;
     }
     else
     {
        mifi = (mifi_t)(rtrIfNum - 1);

        if (setsockopt(targetSocket,level,optname,
                 (L7_uchar8 *) &mifi,sizeof(mifi_t)) == ERROR)
           return L7_SETSOCKOPT_ERROR;
     }
  }
  else
#endif /* L7_IPV6_PACKAGE */
  {
     /* Not implemented for ipv4 */
     return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif
/**************************************************************************
*
* @purpose  Send a message to a socket
*
* @param    s         @b{(input)} socket to send
* @param    buf       @b{(input)} pointer to data buffer
* @param    bufLen    @b{(input)} length of buffer
* @param    flags     @b{(input)} flags to underlying protocols
* @param    family    @b{(input)} Address Family
* @param  port      @b{(input)} Port
* @param  ipAddr    @b{(input)} IP Address
* @param  bytesSent @b{(output)} Ptr to number of bytes sent
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*
*************************************************************************/

L7_RC_t osapiSocketSendto(L7_uint32 s,L7_uchar8 *buf,L7_uint32 bufLen,L7_uint32 flags,
                          L7_sockaddr_t *to, L7_uint32 to_len,
                          L7_uint32 *bytesSent)
{
  L7_int32 count;
  L7_sockaddr_union_t c_saddr;
  struct sockaddr *os_saddr = (struct sockaddr *)&c_saddr;

  if(to_len > sizeof(c_saddr)) return L7_FAILURE;
  memcpy(&c_saddr, to, to_len);
  /* AF values abstracted from caller, so dont need to include world */
  os_saddr->sa_family = (L7_ushort16)osapiFromL7AF(to->sa_family);

  count=sendto(s,buf,bufLen,flags,os_saddr,to_len);
  if (count < 0)
  {
    return L7_ERROR;
  }
  *bytesSent = count;
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Close a socket
*
* @param    fd @b{(input)} socket descriptor to close
*
* @returns  none
*
* @end
*
*************************************************************************/
void osapiSocketClose(L7_uint32 fd)
{
  close(fd) ;
}
/**************************************************************************
*
* @purpose  Shutdown a socket
*
* @param    fd  @b{(input)} socket descriptor to close
* @param    how @b{(input)} how to shutdown
*
* @returns  none
*
* @end
*
*************************************************************************/
L7_RC_t osapiShutdown(L7_uint32 fd, L7_uint32 how)
{
  int rc;

  switch(how)
  {
  case L7_SHUT_RD:
     how = SHUT_RD;
     break;
  case L7_SHUT_WR:
     how = SHUT_WR;
     break;
  case L7_SHUT_RDWR:
     how = SHUT_RDWR;
     break;
  default:
     return L7_FAILURE;
  }


  rc = shutdown(fd,(int)how) ;
  return (rc < 0)?L7_FAILURE:L7_SUCCESS;
}
/**************************************************************************
*
* @purpose  Add route to the routing table
*
* @param    ifName   @b{(input)} Interface to add the route to
* @param    destIp   @b{(input)} destination address, network order
* @param    gateIp   @b{(input)} gateway address, network order
* @param    mask       @b{(input)} mask for destination, network order
* @param    tos      @b{(input)} type of service
* @param    flags      @b{(input)} route flags
* @param    proto      @b{(input)} routing protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapimRouteEntryAdd(L7_uchar8 *ifName, L7_long32 destIp,
  L7_long32 gateIp, L7_long32 mask, L7_uint32 tos, L7_uint32 flags,
  L7_uint32 proto)
{

  return osapiRawMRouteEntryAdd(ifName, destIp, gateIp,mask,tos,flags,proto);

}

/**************************************************************************
*
* @purpose  Delete route from the routing table
*
* @param    ifName   @b{(input)} Interface to delete the route from
* @param    destIp   @b{(input)} destination address, network order
* @param    gateIp   @b{(input)} gateway address, network order
* @param    mask       @b{(input)} mask for destination, network order
* @param    tos      @b{(input)} type of service
* @param    flags      @b{(input)} route flags
* @param    proto      @b{(input)} routing protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/

L7_RC_t osapimRouteEntryDelete(L7_uchar8 *ifName, L7_long32 destIp,
  L7_long32 gateIp, L7_long32 mask, L7_uint32 tos, L7_uint32 flags,
  L7_uint32 proto)
{
  return osapiRawMRouteEntryDelete(ifName, destIp, gateIp,mask,tos,flags,proto);

}


/**************************************************************************
*
* @purpose  delete route from the routing table
*
* @param    destIp  @b{(input)} destination address, network order
* @param    mask    @b{(input)} mask for destination, network order
* @param    tos     @b{(input)} type of service
* @param    flags   @b{(input)} route flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
extern L7_RC_t osapimRouteDelete(L7_uchar8 *destIp,
                                 L7_long32 mask,
                                 L7_uint32 tos,
                                 L7_uint32 flags)
{
  printf ("mRrouteDelete - Not Implemented.\n");
  return L7_FAILURE;
}




/**************************************************************************
*
* @purpose  convert a dot notation Internet address to a long integer
*
* @param    address @b{(input)} IP address in dotted notation
*
* @returns  IP address in long integer
*
* @end
*
*************************************************************************/

L7_ulong32 osapiInet_addr(L7_uchar8 *address)
{
  L7_ulong32 longAddress;
  longAddress=osapiNtohl(inet_addr(address));
  return longAddress;
}

/**************************************************************************
*
* @purpose  convert a network address to dotted decimal notation
*
* @param    inetAddress   @b{(input)} network address
*
* @returns  char pointer to the string version of an Internet address
*
* @end
*
*************************************************************************/

L7_char8* osapiInet_ntoa(L7_ulong32 inetAddress)
{
  L7_char8 * temp;
  struct in_addr inaddr;

  inaddr.s_addr=osapiHtonl(inetAddress);
  temp=inet_ntoa(inaddr);
  return(temp);
}

/**************************************************************************
*
* @purpose  Convert host byte order to network byte order in long integer
*
* @param    hostlong   @b{(input)} host byte order
*
* @returns  network byte order
*
* @end
*
*************************************************************************/

L7_uint64  osapiHtonll(L7_uint64 hostlong)
{
  L7_uint64 netlong;

#if __BYTE_ORDER==__BIG_ENDIAN
  netlong = hostlong;
#else
  netlong = (((unsigned long long)(ntohl((int)((hostlong << 32) >> 32))) << 32) |
                     (unsigned int)ntohl(((int)(hostlong >> 32))));
#endif

  return netlong;
}

/**************************************************************************
*
* @purpose  to convert host byte order to network byte order in long integer
*
* @param    hostlong  @b{(input)} host byte order
*
* @returns  network byte order
*
* @end
*
*************************************************************************/

L7_ulong32  osapiHtonl(L7_ulong32 hostlong)
{
  L7_ulong32 netAddress;
  netAddress=htonl(hostlong);
  return netAddress;
}

/**************************************************************************
*
* @purpose  to convert host byte order to network byte order in short integer
*
* @param    hostshort   @b{(input)} host byte order
*
* @returns  network byte order
*
* @end
*
*************************************************************************/

L7_ushort16  osapiHtons(L7_ushort16 hostshort)
{
  L7_ushort16 netAddress;
  netAddress=htons(hostshort);
  return netAddress;
}

/**************************************************************************
*
* @purpose  Convert network byte order to host byte order in long integer
*
* @param    netlong network byte order
*
* @returns  host byte order
*
* @end
*
*************************************************************************/
L7_uint64  osapiNtohll(L7_uint64 netlong)
{
 L7_uint64 hostlong;

#if    __BYTE_ORDER==__BIG_ENDIAN
  hostlong = netlong;
#else
  hostlong = (((unsigned long long)(ntohl((int)((netlong << 32) >> 32))) << 32) |
                     (unsigned int)ntohl(((int)(netlong >> 32))));
#endif

  return hostlong;
}

/**************************************************************************
*
* @purpose  to convert network byte order to host byte order in long integer
*
* @param    netlong  @b{(input)} network byte order
*
* @returns  host byte order
*
* @end
*
*************************************************************************/

L7_ulong32  osapiNtohl(L7_ulong32 netlong)
{
  L7_ulong32 hostAddress;
  hostAddress=ntohl(netlong);
  return hostAddress;
}

/**************************************************************************
*
* @purpose  to convert network byte order to host byte order in short integer
*
* @param    netshort  @b{(input)} network byte order
*
* @returns  host byte order
*
* @end
*
*************************************************************************/

L7_ushort16  osapiNtohs(L7_ushort16 netshort)
{
  L7_ushort16 hostAddress;
  hostAddress=ntohs(netshort);
  return hostAddress;
}

/**************************************************************************
*
* @purpose  to convert host byte order to little endian byte order in long integer
*
* @param    hostlong  @b{(input)} host byte order
*
* @returns  little endian byte order
*
* @end
*
*************************************************************************/

L7_ulong32  osapiHtole32(L7_ulong32 hostlong)
{
#if    __BYTE_ORDER==__BIG_ENDIAN
  return ntohl(hostlong);
#else
  return hostlong;
#endif
}

/**************************************************************************
*
* @purpose      pend on a set of file descriptors
*
* @param        width           @b{(input)} number of bits to examine from 0
* @param        rFds            @b{(input)} read fds
* @param        wFds            @b{(input)} write fds
* @param        eFds            @b{(input)} exception fds
* @param        timeOutSec      @b{(input)} max time in sec to wait,
*                                           NULL = forever
* @param        timeOutMicroSec @b{(input)} time in microsec
*
* @returns      The number of file descriptors with activity, 0 if timed out
*               or ERROR
*
* @end
*
*************************************************************************/

L7_int32 osapiSelect(L7_uint32 width, fd_set* rFds, fd_set* wFds,
                      fd_set* eFds, L7_long32 timeOutSec,
                      L7_long32 timeOutMicroSec)
{
  L7_int32 noFD;
  struct timeval timeout;

  if ((timeOutSec == L7_NULL) && (timeOutMicroSec == L7_NULL))
  {
    /* must pass null pointer to wait forever */
    noFD = select(width, rFds, wFds, eFds, L7_NULLPTR);
  }
  else
  {
    timeout.tv_sec=timeOutSec;
    timeout.tv_usec=timeOutMicroSec;

    noFD = select(width, rFds, wFds, eFds, &timeout);
  }
  return noFD;
}

/**************************************************************************
* @purpose  Read bytes from the file
*
* @param    fd       @b{(input)} file descriptor
* @param    buffer   @b{(output)} ptr to buffer to read data
* @param    maxbytes @b{(input)} no. of bytes
*
* @returns  The number of bytes read from file
*
* @end
*
*************************************************************************/

L7_uint32 osapiRead(L7_uint32 fd,L7_char8* buffer,size_t maxbytes)
{
  L7_uint32 num;
  num = read(fd,buffer,maxbytes);
  return num;
}

/**************************************************************************
* @purpose  write bytes to the file
*
* @param    fd        @b{(input)} file descriptor
* @param    buffer    @b{(input)} ptr to buffer to write data
* @param    maxbytes  @b{(input)} no. of bytes
*
* @returns  The number of bytes written to file
*
* @end
*
*************************************************************************/

L7_uint32 osapiWrite(L7_uint32 fd,L7_char8* buffer,size_t maxbytes)
{
  L7_uint32 num;
  num = write(fd,buffer,maxbytes);
  return num;
}



/**************************************************************************
* @purpose  Get Operating System Error Number
*
*
* @returns  ERRNO set by the system.
*
* @end
*
*************************************************************************/
extern L7_uint32 osapiErrnoGet(void)
{
  return(errno);
}


/**************************************************************************
*
* @purpose      Initiate a connection on a socket
*
* @param        sockFd          @b{(input)} socket descriptor
* @param        family          @b{(input)} Address Family (for example, AF_INET)
* @param        ipAddr          @b{(input)} IP Address to connect with socket
* @param        port            @b{(input)} Port
*
* @returns      L7_SUCCESS      if the connection or binding succeeds
*               L7_FAILURE      on error
*
* @end
*
*************************************************************************/

L7_RC_t osapiConnect(L7_uint32 sockFd,L7_sockaddr_t *saddr, L7_uint32 s_len)
{
  L7_sockaddr_union_t c_saddr;
  struct sockaddr *os_saddr = (struct sockaddr *)&c_saddr;

  if(s_len > sizeof(c_saddr)) return L7_FAILURE;
  memcpy(&c_saddr, saddr, s_len);
  os_saddr->sa_family = (L7_ushort16)osapiFromL7AF(saddr->sa_family);

  if (connect (sockFd, os_saddr, s_len) == ERROR)
    return L7_FAILURE;
  else
    return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose      Find the absolute value of a number (double precision)
*
* @param        x               @b{(input)} the number
*
* @returns      L7_double64     absolute value of x
*
* @comments     Math replacement for fabs() to avoid usage of math.h or libm.
*
* @end
*
*************************************************************************/

L7_double64 osapiFabs(L7_double64 x)
{
  return (x > 0.0) ? x : -x;
}

/**************************************************************************
*
* @purpose      Find the maximum of two numbers (double precision)
*
* @param        x               @b{(input)} first number
* @param        y               @b{(input)} second number
*
* @returns      L7_double64     maximum value of x or y
*
* @comments     Math replacement for fmax() to avoid usage of math.h or libm.
*
* @end
*
*************************************************************************/

L7_double64 osapiFmax(L7_double64 x, L7_double64 y)
{
  return (x > y) ? x : y;
}

/**************************************************************************
*
* @purpose      Find the non-negative square root (double precision)
*
* @param        x               @b{(input)} the number
*
* @returns      L7_double64     square root of x, or 0.0 if x <= 0.0
*
* @comments     Math replacement for sqrt() to avoid usage of math.h or libm.
*
* @comments     Uses Newton's method to compute the square root.
*
* @end
*
*************************************************************************/

L7_double64 osapiSqrt(L7_double64 x)
{
#define EPS   1.0E-8
  L7_double64 delta = 1.0;
  L7_double64 r = x;
  if (x <= 0.0)
     return 0.0;         /* the square root of 0.0 is a special case */
  while ((delta > EPS) ||
         (delta < -EPS))
  {
    delta = (r * r - x) / (2.0 * r);
    r -= delta;
  }
  return r;
}

/**************************************************************************
*
* @purpose      Set the non-blocking mode on a file (socket).
*               The FIONBIO method says only for sockets in tornado include,
*               but we apply it to other file types as well.
*
* @param        fd              file descriptor
*               nbio            if L7_TRUE, non-blocking, else blocking
*
* @returns      L7_SUCCESS or L7_FAILURE
*
*
* @end
*
*************************************************************************/

L7_RC_t osapiSocketNonBlockingModeSet(L7_uint32 fd, L7_BOOL nbio)
{
    int mode = (nbio == L7_TRUE)?1:0;
    int rc;

    rc = ioctl(fd, FIONBIO, (void *)&mode);

    return (rc< 0)? L7_FAILURE:L7_SUCCESS;
}


/**************************************************************************
*
* @purpose      create ascii displayable string from IP (4/6) address
*
* @param        family          address faminly
*               addr            pointer to address
*               str             pointer to ouput string
*               len             length of output string
*
* @returns      output string or L7_NULL
*
*
* @end
*
*************************************************************************/
L7_uchar8 *osapiInetNtop(L7_uint32 family, L7_uchar8 *addr, L7_uchar8 *str, L7_uint32 len)
{
   const L7_uchar8 *rp;

   family = osapiFromL7AF(family);
   rp =  inet_ntop(family,addr,str,len);
   return (L7_uchar8 *)rp;
}

/**************************************************************************
*
* @purpose      create IP (4/6) address from ascii string
*
* @param        family          address faminly
*               str             pointer to ouput string
*               addr            pointer to address
*
* @returns      L7_SUCCESS or L7_FAILURE
*
*
* @end
*
*************************************************************************/
L7_RC_t osapiInetPton(L7_uint32 family, L7_uchar8 *str, L7_uchar8 *addr)
{
   int rc;

   family = osapiFromL7AF(family);

      if(family == AF_INET6)
   {
     /* This condition checks the ipv6 adress having less than or eqult to 4
        characters  in each colon part or not */

     if(osapiIpv6Support(str) == L7_FALSE)
     return L7_FAILURE;
   }

   rc = inet_pton(family,str,addr);
   return (rc <= 0)? L7_FAILURE:L7_SUCCESS;
}



/**************************************************************************
*
* @purpose  Get Interface status
*
* @param    ifName            @b{(input)} interface name
* @param    isUp              @b{(output)} ptr to boolean
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiInterfaceIsUp(L7_uchar8 *ifName, L7_BOOL *isUp)
{
   return ipstkIfIsUp(ifName,isUp);
}


#ifdef L7_ROUTING_PACKAGE
/**************************************************************************
*
* @purpose  Get Router Interface status
*
* @param    intfIfNum         @b{(input)} interface number
* @param    isUp              @b{(output)} ptr to boolean
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiRouterIfStatusGet(L7_uint32 intIfNum, L7_BOOL *isUp)
{

   *isUp = (ipstkRtrIfIsUp(intIfNum) == L7_SUCCESS)? L7_TRUE: L7_FALSE;

   return L7_SUCCESS;
}


/**************************************************************************
*
* @purpose  Get Router Interface type
*
* @param    intfIfNum         @b{(input)} interface number
* @param    t_flags           @b{(output)} ptr to interface flags describing
*                               interface capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiRouterIfTypeGet(L7_uint32 intIfNum, L7_uint32 *t_flags)
{
   L7_uchar8 ifName[IFNAMSIZ];
   L7_BOOL is_set;

   *t_flags = 0;
   if( osapiIfNameStringGet(intIfNum, ifName,IFNAMSIZ) != L7_SUCCESS)
       return L7_FAILURE;


   if(ipstkIfFlagGet(ifName, IFF_BROADCAST, &is_set) != L7_SUCCESS)
       return L7_FAILURE;
   if(is_set == L7_TRUE)
       *t_flags |= L7_IFF_BROADCAST;

   if(ipstkIfFlagGet(ifName, IFF_LOOPBACK, &is_set) != L7_SUCCESS)
       return L7_FAILURE;
   if(is_set == L7_TRUE)
       *t_flags |= L7_IFF_LOOPBACK;

   if(ipstkIfFlagGet(ifName, IFF_POINTOPOINT, &is_set) != L7_SUCCESS)
       return L7_FAILURE;
   if(is_set == L7_TRUE)
       *t_flags |= L7_IFF_POINTOPOINT;

   if(ipstkIfFlagGet(ifName, IFF_MULTICAST, &is_set) != L7_SUCCESS)
       return L7_FAILURE;
   if(is_set == L7_TRUE)
       *t_flags |= L7_IFF_MULTICAST;


   return L7_SUCCESS;
}

#ifdef L7_RLIM_PACKAGE
/* keep this def local to here */
#define IP_TUNNEL_6OVER4_PREFIX "sit"
#define IP_TUNNEL_6TO4_PREFIX   "6to4_"

L7_RC_t
osapiRouterIfTunnelNamePrefixGet(L7_uint32 intIfNum, L7_uchar8 *buf)
{
   L7_TUNNEL_MODE_t tunnelMode;

   if(rlimTunnelActiveModeGet(intIfNum, &tunnelMode) != L7_SUCCESS)
       return L7_FAILURE;

   switch(tunnelMode)
   {
   case L7_TUNNEL_MODE_6OVER4:
       strcpy(buf,IP_TUNNEL_6OVER4_PREFIX);
       break;
   case L7_TUNNEL_MODE_6TO4:
       strcpy(buf,IP_TUNNEL_6TO4_PREFIX);
       break;
   default:
       return L7_FAILURE;
   }
   return L7_SUCCESS;
}
#endif

/*********************************************************************
*
* @purpose  Get name string for the specified interface.
*
* @param    intIfNum   Internal Interface Number
* @param    buf        (output) Buffer to store name string.
* @param    buflen     Length of buffer to store name string.
*
* @returns  L7_SUCCESS Interface is valid.
* @returns  L7_FAILURE   Interface is not valid.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfNameStringGet(L7_uint32 intIfNum, L7_uchar8 *buf, L7_uint32 buflen)
{

  L7_RC_t rc = 0;
  nimUSP_t usp;
  char  tmpBuf[2*IFNAMSIZ];
  L7_INTF_TYPES_t itype;
  nimConfigID_t configId;
  L7_uchar8 dev[IFNAMSIZ];

  memset(tmpBuf, 0, 2*IFNAMSIZ);

  if (nimGetIntfType(intIfNum, &itype) != L7_SUCCESS)
  {
  LOG_MSG("intIfNum %d: Could not get type, rc = %d\n", intIfNum, rc);
  return L7_FAILURE;
  }

  if (itype == L7_LOOPBACK_INTF)
  {
    if ((rc = nimConfigIdGet(intIfNum, &configId)) != L7_SUCCESS)
    {
      LOG_MSG("intIfNum %d: Could not get configId, rc = %d\n", intIfNum, rc);
      return L7_FAILURE;
    }

    /* dont use lo: alias because there is no procfs support */
    /* instead use normal tap driver with output squelched as is done
       on vxworks */
    sprintf(tmpBuf, "loopback_%u", configId.configSpecifier.loopbackId);
  }
#ifdef L7_RLIM_PACKAGE
  else if (itype == L7_TUNNEL_INTF)
  {
    if ((rc = nimConfigIdGet(intIfNum, &configId)) != L7_SUCCESS)
    {
      LOG_MSG("intIfNum %d: Could not get configId, rc = %d\n", intIfNum, rc);
      return L7_FAILURE;
    }

    if(osapiRouterIfTunnelNamePrefixGet(intIfNum, dev) != L7_SUCCESS)
      return L7_FAILURE;

    /* tunnels use intIfNum as index, not rtrIf because we need to
       create and manage the tunnel device independent of creating router
       interfaces that use the tunnel.
    */
    sprintf(tmpBuf, "%s%d", dev, intIfNum);
  }
#endif
  else
  {
    if ((rc = nimGetUnitSlotPort(intIfNum, &usp)) != L7_SUCCESS)
    {
      LOG_MSG("intIfNum %d: Could not get USP, rc = %d\n", intIfNum, rc);
      return L7_FAILURE;
    }

    ipmRouterIfDrvNamePrefixGet(dev);
    sprintf (tmpBuf, "%s%d_%d_%d", L3INTF_DEVICE_NAME,
             usp.unit, usp.slot, usp.port-1);
  }

  if (strlen(tmpBuf) >= buflen)
  {
    LOG_MSG("intIfNum %d: Interface name %s invalid\n", intIfNum, tmpBuf);
    return L7_FAILURE;
  }

  strcpy(buf, tmpBuf);

  return (L7_SUCCESS);
}


/**************************************************************************
*
* @purpose  Send a message to a socket on a specific interface
*
* @param    s         @b{(input)} socket to send
* @param    buf       @b{(input)} pointer to data buffer
* @param    bufLen    @b{(input)} length of buffer
* @param    flags     @b{(input)} flags to underlying protocols
* @param    family    @b{(input)} Address Family
* @param  port      @b{(input)} Port
* @param  ipAddr    @b{(input)} IP Address
* @param  intIfNum  @b{(input)} internal interface
* @param  bytesSent @b{(output)} Ptr to number of bytes sent
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    Needed due to link local addresses where unicast addr does
*           not identify interface.
* @end
*
*************************************************************************/

L7_RC_t osapiSocketIntfSendto(L7_uint32 s,L7_uchar8 *buf,L7_uint32 bufLen,L7_uint32 flags,
                          L7_sockaddr_t *to, L7_uint32 to_len, L7_uint32 intIfNum,
                          L7_uint32 *bytesSent)
{
  L7_sockaddr_in6_t from;

  /* osapiPktInfoSend() honors the intIfNum argument in all cases, not just 
     link-local addresses, so just use that. */
  /* Using IN6ADDR_ANY for the from address tells the kernel to just use 
     whatever it would ordinarily use */
  memset(&from, 0, sizeof(from));
  return osapiPktInfoSend(s, buf, bufLen, flags, (L7_sockaddr_t *)(&from), 
			  sizeof(from), to, to_len, intIfNum, bytesSent, 
			  0, 0, 0, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE);
}

#endif /* L7_ROUTING_PACKAGE */

/**************************************************************************
*
* @purpose  It finds wheather all the string part contains 4 characters or more
*           in between colons in IPV6 address.Since This condition is not handled
*           in interpeak stack.(NOTE: This function checks only four characters
*           condition only.Not any other conditions.)
*
*           Return true if string is contains not more than 4 characters
*           xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
*
*           Return false if string contains more than 4 characters in between colons
*           xxxx:xxxxxx:xxxxxxx:xxxx:xxxx:xxxx:xxxx:xxxx
*
*           Return false if the number of colons in complete string exceeds a count of 7
*
*           Return false if the string ends with a colon (ABCD::)
*
* @param    s   @b{(input)}ipv6 string format.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @end
*
*************************************************************************/
L7_BOOL  osapiIpv6Support(L7_char8 *s)
{

   L7_uint32 index =0, count=0, searchLength = 0, final = 0, start, end, colonCount = 0;
   L7_char8  *ref1, *ref2;
   L7_BOOL   isdotForm = 0;

   ref1 = ref2 = s;

   /* This loop finds upto what position search shold be done.
      to handle the dotted decimal and colon (ipv4 on ipv6) notation */

   while(*ref1 != '\0')
   {
      if(*ref1 == ':')
      {
         final = index;
         colonCount++;
      }

      if(*ref1 == '.')
      {
         isdotForm = L7_TRUE;
         break;
      }

      if(colonCount > 7)
      {
        return L7_FALSE;
      }


      ref1++;
      index++;
   }

   if(isdotForm == L7_TRUE)
   {
      searchLength = final;
   }
   else
   {
      searchLength = index;
   }

   end = 0 ;
   start= 0;

   while( (count <= searchLength) && (*ref2 != '\0') )
   {
      /* It will handle the single colon case
         if it is single quotation it will move one position forward */
      if(*ref2 == ':')
      {
         end = count;

         if((end-start) > 4)
         return L7_FALSE;

         start = count + 1;

         /* It will handle the double colon case
            if it is double colon then it should move two positions forward */


         if( *(ref2 + 1) == ':')
         {
              ref2++;
              count++;
              start = count + 1;
         }
      }

      ref2++;
      count++;

      /* it will handle the case like colon and dotted quotation mark
         as weill as final 4 characters.since it wont end with colon */

      if(count == searchLength)
      {
        if((count-start) > 4 )
            return L7_FALSE;
      }
   }
   return L7_TRUE;
}

#if defined(L7_IPV6_PACKAGE) && defined(L7_IP_MCAST_PACKAGE)
int osapiDebugV6Mcast(int send, int intIfNum, int do_mrt, int do_mif) {
  int sockfd;
  L7_sockaddr_in6_t sa, dst_sa;
  L7_BOOL option;
  int ifindex;
  L7_uchar8 buf[64];
  fd_set rfds;
  int nbytes, flags, hops, salen, dstsalen;

  if (osapiSocketCreate(L7_AF_INET6, L7_SOCK_RAW, IPPROTO_UDP, &sockfd)
      != L7_SUCCESS) {
    perror("Error creating socket");
    return(1);
  }
  memset(&sa, 0, sizeof(sa));
  sa.sin6_family = L7_AF_INET6;
  sa.sin6_len = sizeof(sa);
  if (osapiSocketBind(sockfd, (L7_sockaddr_t *)&sa, sizeof(sa)) 
      != L7_SUCCESS) {
    perror("Error binding socket to v6 INADDR_ANY");
    osapiSocketClose(sockfd);
    return(1);
  }
  option = L7_TRUE;
  if (osapiSetsockopt(sockfd, IPPROTO_IPV6, L7_IPV6_RECVPKTINFO, 
		      (L7_uchar8 *)&option, sizeof(option)) != L7_SUCCESS) {
    perror("Error setting RECVPKTINFO");
    osapiSocketClose(sockfd);
    return(1);
  }
  option = L7_FALSE;
  if (osapiSetsockopt(sockfd, IPPROTO_IPV6, L7_IPV6_MULTICAST_LOOP, 
		      (L7_uchar8 *)&option, sizeof(option)) != L7_SUCCESS) {
    perror("Error setting MULTICAST_LOOP");
    osapiSocketClose(sockfd);
    return(1);
  }
  if (intIfNum && send) {    
    if (ipstkIntIfNumToStackIfIndex(intIfNum, &ifindex) != L7_SUCCESS) {
      printf("Could not resolve intifnum %d to ifindex\n", intIfNum);
      osapiSocketClose(sockfd);
      return(1);
    }
    printf("Setting multicast send ifindex to %d\n", ifindex);
    if (osapiSetsockopt(sockfd, IPPROTO_IPV6, L7_IPV6_MULTICAST_IF, 
			(L7_uchar8 *)&ifindex, sizeof(ifindex)) != L7_SUCCESS) {
      perror("Error setting multicast send ifindex");
      osapiSocketClose(sockfd);
      return(1);
    }
  }
  if (do_mrt) {
    printf("Enabling multicast send/receive globally\n");
    if (osapiIpIpv6MRTSet(sockfd, IPPROTO_IPV6, L7_TRUE, 
			  (L7_uchar8 *)&option, sizeof(option)) != L7_SUCCESS) {
      perror("Error enabling multicast");
      osapiSocketClose(sockfd);
      return(1);
    }
  }
  if (intIfNum && do_mif) {
    printf("Enabling multicast send/receive on interface\n");
    if (osapiIpIpv6MRTMifSet(sockfd, IPPROTO_IPV6, L7_TRUE, 
			     intIfNum) != L7_SUCCESS) {
      perror("Error enabling multicast on interface");
      osapiSocketClose(sockfd);
      return(1);
    }
  }
  if (send) {
    bzero(&sa, sizeof(sa));
    sa.sin6_len = sizeof(sa);
    sa.sin6_family = L7_AF_INET6;
    sa.sin6_addr.in6.addr8[0] = 0xff;
    sa.sin6_addr.in6.addr8[1] = 0x0e;
    sa.sin6_addr.in6.addr8[15] = 0x33;
    printf("About to send packet to ff0e::33\n");
    memset(buf, 0x55, 64);
    if (osapiSocketSendto(sockfd, buf, 64, 0, (L7_sockaddr_t *)&sa, 
			  sizeof(sa), &nbytes) != L7_SUCCESS) {
      perror("Send error");
      osapiSocketClose(sockfd);
      return(1);      
    }
    printf("Sent %d bytes\n", nbytes);
  } else {
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    printf("Waiting up to 60 seconds for receive\n");
    osapiSelect(sockfd+1, &rfds, NULL, NULL, 60, 0);
    if (!FD_ISSET(sockfd, &rfds)) {
      printf("No packet received.\n");
      osapiSocketClose(sockfd);
      return(2);
    }
    bzero(buf, 64);
    bzero(&sa, sizeof(sa));
    bzero(&dst_sa, sizeof(dst_sa));
    sa.sin6_family = L7_AF_INET6;
    sa.sin6_len = sizeof(sa);
    dst_sa.sin6_len = sizeof(dst_sa);
    dst_sa.sin6_family = L7_AF_INET6;
    nbytes = 64;
    ifindex = hops = flags = 0;
    salen = sizeof(sa);
    dstsalen = sizeof(dst_sa);
    if (osapiPktInfoRecv(sockfd, buf, &nbytes, &flags, (L7_sockaddr_t *)&sa, 
			 &salen, (L7_sockaddr_t *)&dst_sa, 
			 &dstsalen, &ifindex, &hops, NULL)
	!= L7_SUCCESS) {
      perror("Read error");
      osapiSocketClose(sockfd);
      return(1);      
    }
    printf("Received %d bytes on ifindex %d with hop count %d\n", nbytes, 
	   ifindex, hops);
    printf("Packet was addressed to %x:%x:%x:%x\n", 
	   dst_sa.sin6_addr.in6.addr32[0], dst_sa.sin6_addr.in6.addr32[1],
 	   dst_sa.sin6_addr.in6.addr32[2], dst_sa.sin6_addr.in6.addr32[3]); 
  }
  osapiSocketClose(sockfd);
  return(0);
}
#endif
