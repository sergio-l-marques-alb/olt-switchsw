/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  l7_socket.h
*
* @purpose   Common defines, enums and data structures for sockets
*
* @component System Common
*
* @comments  none
*
* @create    11/01/2002
*
* @author    mfiorito
*            jpickering
*
* @end
*             
**********************************************************************/

/*************************************************************
all modules that access sockets other than via osapi_supprt, or
require sokcet related structs or #defines need to include this file.
The selected files included by this file will determine whether
socket calls go through the OS (switching builds) or through
the interpeak stack (routing builds).
                    
*************************************************************/

#ifndef _INCLUDE_L7_SOCKET_
#define _INCLUDE_L7_SOCKET_

#include "l7_common.h"

#ifdef _L7_OS_VXWORKS_

#include "l7_ipcom_config.h"

#if (VX_VERSION==65)
#include <sockLib.h>
#include <inetLib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <ipnet/ipioctl.h>
#include <wrapper/wrapperHostLib.h>
#include <net/uio.h>
/* these are intentionally not limited to v6 flex or v6 mgmt builds. Some code 
 * refers to structures defined here, even in base builds. For example, see
 * osapiPktInfoRecv(). */
#include <netinet6/in6.h>
#include <netinet6/in6_var.h>

/* Would like to include this and use ip_tunnel_param in osapiIpeakTunnelIfCreate(), 
 * but it looks like ipioctl_var.h has a compile error (missing ";" at end of 
 * sioc_route_table definition. Err. */
/* #include <ipnet/ipioctl_var.h> */
#endif

#if (VX_VERSION==55)
#include "ipcom_bsdsock.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_sock6.h"
#endif


#if (VX_VERSION==65)
struct protoent	*getprotobyname (const char *);
#define IP_IFF_IPV6_ENA     0x0020  /* ipv6 admin enable */
#define IFF_UNNUMB          0x0400  /* Unnumbered interface */
#endif



/* extra stuff interpeak doesnt define */
#define IN_CLASSA_NET     0xff000000
#define IN_CLASSB_NET     0xffff0000
#define IN_CLASSC_NET     0xffffff00
#define IN_CLASSA_HOST    0x00ffffff
#define IN_CLASSB_HOST    0x0000ffff
#define IN_CLASSC_HOST    0x000000ff
#define IN_CLASSA_NSHIFT  24
#define IN_CLASSB_NSHIFT  16
#define IN_CLASSC_NSHIFT  8
#define IN_LOOPBACKNET    127
#define IP_MAX_MEMBERSHIPS 20
#define INET_ADDR_LEN     18

#if (VX_VERSION==55)
#define INADDR_MAX_LOCAL_GROUP 0xe00000ff
#define IN_MULTICAST(xxadd)  IN_CLASSD(xxadd)
#define IN_BADCLASS(xxadd)   IN_EXPERIMENTAL(xxadd)
#define MSG_DONTWAIT      IP_MSG_DONTWAIT
#define MSG_DONTROUTE     IP_MSG_DONTROUTE
#define SO_KEEPALIVE      IP_SO_KEEPALIVE
#define SO_DONTROUTE      IP_SO_DONTROUTE
#endif

#endif

#ifdef _L7_OS_LINUX_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>
#endif

#ifdef _L7_OS_ECOS_
#include <sys/bsdtypes.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <time.h>
#include <cyg/hal/drv_api.h>
#define _KERNEL
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_arp.h>
#undef _KERNEL
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/ip_mroute.h>
#endif

#endif /* _INCLUDE_L7_SOCKET_ */

