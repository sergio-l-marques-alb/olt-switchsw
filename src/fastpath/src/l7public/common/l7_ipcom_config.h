

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7_ipcom_config.h
*
* @purpose    interpeak configuration file
*
* @component  interpeak stack
*
* @comments   none
*
* @create     
*
* @author     jpp
* @end
*
**********************************************************************/

#ifndef INCLUDE_L7_IPCOM_CONFIG
#define INCLUDE_L7_IPCOM_CONFIG

#include "flex.h"
#ifdef _L7_OS_VXWORKS_
#include <vxWorks.h>
#endif

/* configuration file required to include before
   interpeak api includes. this is here because interpeak user land include
   files require config information. The file is also included when building
   the stack itself.
*/
#if _BYTE_ORDER == _BIG_ENDIAN
#ifndef IP_BIG_ENDIAN
#define IP_BIG_ENDIAN
#endif
#else
#define IP_LITTLE_ENDIAN
#endif

#if !defined(IPNET)
#define IPNET
#endif

#if !defined(IPTCP)
#define IPTCP
#endif

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#define IPCOM_USE_INET6
#endif

/* Defining this here makes IP stack stats available even if IP_DEBUG is 
 * not defined. */
#define IPNET_STATISTICS

/* for socket tracing */
#define TRSKT_OPEN    1
#define TRSKT_ACCEPT  2
#define TRSKT_CLOSE   3
#define TRSKT_FREE    4

/* outcall from stack to learn task parms from mapping */
int ipstkTaskParmsGet(unsigned int *pStackSize, unsigned int *pPriority, unsigned int *pSlice);


#endif /* INCLUDE_L7_IPCOM_CONFIG */

