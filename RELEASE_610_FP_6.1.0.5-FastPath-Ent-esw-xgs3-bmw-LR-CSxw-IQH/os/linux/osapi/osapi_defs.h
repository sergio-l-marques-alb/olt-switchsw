#ifdef _L7_OS_LINUX_
#ifndef OSAPI_DEFS_H
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_defs.h
*
* @purpose   IPv6 Socket defines
*
* @component osapi
*
* @comments 
*
* @create    12/12/2006
*
* @author    Kiran Kumar Kella
*
* @end
*
*********************************************************************/

/* None needed for Wind River Linux 2.0 */
/* For 1.4... */
#ifndef IPV6_TCLASS
#define IPV6_TCLASS 67
#endif
#ifndef IPV6_RECVPKTINFO
#define IPV6_RECVPKTINFO IPV6_PKTINFO
#endif
#ifndef IPV6_RECVHOPLIMIT
#define IPV6_RECVHOPLIMIT IPV6_HOPLIMIT
#endif
#ifndef IPV6_RECVHOPOPTS
#define IPV6_RECVHOPOPTS IPV6_HOPOPTS
#endif
#endif /* OSAPI_DEFS_H */
#endif /* _L7_OS_LINUX_ */
