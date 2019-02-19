/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  osapi_support.h
*
* @purpose   OS independent prototype declarations
*
* @component osapi
*
* @comments  none
*
* @create    10/29/2001
*
* @author
*
* @end
*
*********************************************************************/

#ifndef OSAPISUPPORT_HEADER
#define OSAPISUPPORT_HEADER

#include <stddef.h>
#include "commdefs.h"
#include "datatypes.h"
#include "osapi_sockdefs.h"

#ifdef _L7_OS_VXWORKS_
#include <selectLib.h>
#elif defined(_L7_OS_LINUX_) || defined(_L7_OS_ECOS_)
#include <sys/select.h>
#endif

#if defined (PC_LINUX_HOST) && !defined (NDA_RTA)
#define NDA_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))
#endif

/**************************************************************************
*
* @purpose  Create a socket
*
* @param    domain      @b{(input)} address family (for example, AF_INET)
* @param    type        @b{(input)} SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW
* @param    protocol    @b{(input)} protocol to be used
* @param    descriptor  @b{(output)}ptr to socket descriptor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
extern L7_uint32 osapiSocketCreate(L7_uint32 domain,
                                   L7_uint32 type,
                                   L7_uint32 protocol,
                                   L7_uint32 *descriptor);

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
extern L7_RC_t osapiSocketBind(L7_uint32 socketDescriptor,
                               L7_sockaddr_t *saddr,L7_uint32 s_len);

/**************************************************************************
*
* @purpose      Initiate a connection on a socket
*
* @param        sockFd          @b{(input)} socket descriptor
* @param        servAddress     @b{(input)} pointer to connector's address info.
* @param        addrLength      @b{(input)} length of connector's address info.
*
* @returns      L7_SUCCESS      if the connection or binding succeeds
*               L7_FAILURE      on error
*
* @end
*
*************************************************************************/

extern L7_RC_t osapiConnect (L7_uint32 sockFd, L7_sockaddr_t *servAddress, L7_uint32 addrLength);

/**************************************************************************
*
* @purpose      Wait on a set of file descriptors
*
* @param    width             @b{(input)} number of bits to examine from 0
* @param    rFds              @b{(input)} read fds
* @param    wFds              @b{(input)} write fds
* @param    eFds              @b{(input)} exception fds
* @param    timeOutSec    @b{(input)} max time in sec to wait,
*                                             NULL = forever
* @param    timeOutMicroSec   @b{(input)} time in microsec
*
* @returns      The number of file descriptors with activity, 0 if timed out
*               or ERROR
*
* @end
*
*************************************************************************/
#ifndef  _L7_OS_ECOS_
 extern L7_int32 osapiSelect( L7_uint32 width,
                              fd_set*   rFds,
                              fd_set*   wFds,
                              fd_set*   eFds,
                              L7_long32 timeOutSec,
                              L7_long32 timeOutMicroSec);
#endif


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
extern L7_RC_t osapiSocketRecvfrom(L7_uint32 socketDescriptor,
                                     L7_uchar8 *buf,
                                     L7_uint32 length,
                                     L7_uint32 flag,
                                     L7_sockaddr_t *from,
                                     L7_uint32   *from_len,
                                     L7_uint32 *bytesRcvd);

/**************************************************************************
*
* @purpose  Receive data from a socket
*
* @param      fd                @b{(input)} socket to receive from
* @param      ptr               @b{(input)} pointer to data buffer
* @param      nbytes            @b{(input/output)} length of buffer
* @param      flag              @b{(input)} flags to underlying protocols
* @param      from              @b{(output)} source address
* @param      from_len          @b{(output)} source address length
* @param      to                @b{(output)} source address
* @param      to_len            @b{(output)} source address length
* @param      ifIndex           @b{(output)} intIfNum
* @param      hops              @b{(output)} hop count
* @param      rtrAlert          @b{(output)} did packet have Router-Alert
*                               option set?
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*
*************************************************************************/
L7_RC_t osapiPktInfoRecv(int fd , void *ptr, int *nbytes,int *flagsp,L7_sockaddr_t *from,
                   int *from_len, L7_sockaddr_t *to, int *to_len, L7_uint32 *ifIndex,L7_uint32 *hops,L7_BOOL *rtrAlert);

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
                         int *from_len, L7_sockaddr_t *to, int *to_len, L7_uint32 *stackIfIndex);

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

L7_RC_t osapiPktInfoSend(L7_uint32 fd,L7_uchar8 *buf,L7_uint32 bufLen,L7_uint32 flags,
                         L7_sockaddr_t *from, L7_uint32 from_len,
                         L7_sockaddr_t *to, L7_uint32 to_len, L7_uint32 intIfNum,
                         L7_uint32 *bytesSent, L7_uint32 trafficClass,
                         L7_uint32 hopLimit, L7_uint32 nextHdr,
                         L7_BOOL tcFlag, L7_BOOL hlFlag, L7_BOOL nhFlag, L7_BOOL rtrAlert);

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
extern L7_RC_t osapiSetsockopt(L7_uint32 targetSocket,
                               L7_uint32 level,
                               L7_uint32 optname,
                               L7_uchar8 *optval,
                               L7_uint32 optlen);

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
extern L7_RC_t osapiIpIpv6MRTSet(L7_uint32 targetSocket,L7_uint32 level,
                                 L7_BOOL initFlag, L7_uchar8 *optval,
                                 L7_uint32 optlen);


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
extern L7_RC_t osapiIpIpv6MRTMifSet(L7_uint32 targetSocket, L7_uint32 level,
                                    L7_BOOL addFlag, L7_uint32 intIfNum);

/**************************************************************************
*
* @purpose  Get socket options
*
* @param    targetSocket @b{(input)} socket to receive from
* @param    level        @b{(input)} protocol level of option
* @param    optname      @b{(input)} option name
* @param    optval       @b{(input)} pointer to option value
* @param    optlen       @b{(input/output)} option length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
extern L7_RC_t osapiGetsockopt(L7_uint32 targetSocket,
                               L7_uint32 level,
                               L7_uint32 optname,
                               L7_uchar8 *optval,
                               L7_uint32 *optlen);

/**************************************************************************
*
* @purpose  Set Broadcast Address on the interface
*
* @param    fd   @b{(input)} socket descriptor
* @param    intf @b{(input)} Ptr to interface name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
extern L7_RC_t osapiSetBroadAddr(L7_int32 fd,
                                 const L7_uchar8 *intf);

/**************************************************************************
*
* @purpose  Shutdown read and/or write on a socket
*
* @param    fd   @b{(input)} socket descriptor
* @param    how  @b{(input)} read/write flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
extern L7_RC_t osapiShutdown(L7_uint32 fd, L7_uint32 how);

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
extern L7_RC_t osapiSocketSendto(L7_uint32 s,
                                   L7_uchar8 *buf,
                                   L7_uint32 bufLen,
                                   L7_uint32 flags,
                                   L7_sockaddr_t *to,
                                   L7_uint32   to_len,
                                   L7_uint32 *bytesSent);

/**************************************************************************
*
* @purpose  Send a message to a socket on a specific interface
*
* @param	s         @b{(input)} socket to send  
* @param	buf       @b{(input)} pointer to data buffer
* @param	bufLen    @b{(input)} length of buffer
* @param	flags     @b{(input)} flags to underlying protocols
* @param	family    @b{(input)} Address Family  
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
                          L7_uint32 *bytesSent);

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
extern void osapiSocketClose(L7_uint32 fd);


/**************************************************************************
*
* @purpose  Add route to the routing table
*
* @param    ifName   @b{(input)} Interface to add the route to
* @param    destIp   @b{(input)} destination address, host order
* @param    gateIp   @b{(input)} gateway address, host order
* @param    mask       @b{(input)} mask for destination, host order
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
extern L7_RC_t osapimRouteEntryAdd(L7_uchar8 *ifName,
                                   L7_long32 destIp,
                                   L7_long32 gateIp,
                                   L7_long32 mask,
                                   L7_uint32 tos,
                                   L7_uint32 flags,
                                   L7_uint32 proto);

/**************************************************************************
*
* @purpose  Delete route from the routing table
*
* @param    destIp   @b{(input)} destination address, host order
* @param    gateIp   @b{(input)} gateway address, host order
* @param    mask       @b{(input)} mask for destination, host order
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
extern L7_RC_t osapimRouteEntryDelete(L7_uchar8 *ifName,
                                      L7_long32 destIp,
                                      L7_long32 gateIp,
                                      L7_long32 mask,
                                      L7_uint32 tos,
                                      L7_uint32 flags,
                                      L7_uint32 proto);

/**************************************************************************
*
* @purpose  Delete route from the routing table
*
* @param    destIp @b{(input)} destination address, host order
* @param    mask   @b{(input)} mask for destination, host order
* @param    tos    @b{(input)} type of service
* @param    flags  @b{(input)} route flags
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
                                 L7_uint32 flags);


/**************************************************************************
*
* @purpose  Convert a dot notation Internet address to a long integer
*
* @param    address @b{(input)} IP address in dotted notation
*
* @returns  IP address in long integer
*
* @end
*
*************************************************************************/
extern L7_ulong32 osapiInet_addr(L7_uchar8 *address);

/**************************************************************************
*
* @purpose  Convert a network address to dotted decimal notation
*
* @param    inetAddress @b{(input)} network address
*
* @returns  char pointer to the string version of an Internet address
*
* @end
*
*************************************************************************/
extern L7_char8* osapiInet_ntoa(L7_ulong32 inetAddress);

/**************************************************************************
*
* @purpose  Convert host byte order to network byte order in 64-bit integer
*
* @param    hostlong @b{(input)} host byte order
*
* @returns  network byte order
*
* @end
*
*************************************************************************/
extern L7_uint64  osapiHtonll(L7_uint64 hostlong);

/**************************************************************************
*
* @purpose  Convert network byte order to host byte order in 64-bit integer
*
* @param    netlong @b{(input)} network byte order
*
* @returns  host byte order
*
* @end
*
*************************************************************************/
extern L7_uint64  osapiNtohll(L7_uint64 netlong);

/**************************************************************************
*
* @purpose  Convert host byte order to network byte order in long integer
*
* @param    hostlong @b{(input)} host byte order
*
* @returns  network byte order
*
* @end
*
*************************************************************************/
extern L7_ulong32  osapiHtonl(L7_ulong32 hostlong);


/**************************************************************************
*
* @purpose  Convert host byte order to network byte order in short integer
*
* @param    hostshort @b{(input)} host byte order
*
* @returns  network byte order
*
* @end
*
*************************************************************************/
extern L7_ushort16  osapiHtons(L7_ushort16 hostshort);


/**************************************************************************
*
* @purpose  Convert network byte order to host byte order in long integer
*
* @param    netlong @b{(input)} network byte order
*
* @returns  host byte order
*
* @end
*
*************************************************************************/
extern L7_ulong32  osapiNtohl(L7_ulong32 netlong);


/**************************************************************************
*
* @purpose  Convert network byte order to host byte order in short integer
*
* @param    netshort @b{(input)} network byte order
*
* @returns  host byte order
*
* @end
*
*************************************************************************/
extern L7_ushort16  osapiNtohs(L7_ushort16 netshort);

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
extern L7_ulong32  osapiHtole32(L7_ulong32 hostlong);

/**************************************************************************
* @purpose  Read bytes from a file
*
* @param    fd       @b{(input)} file descriptor
* @param    buffer   @b{(input)} ptr to buffer to read data
* @param    maxbytes @b{(input)} number of bytes to read
*
* @returns  The number of bytes read from file
*
* @end
*
*************************************************************************/
extern L7_uint32 osapiRead(L7_uint32 fd,
                           L7_char8* buffer,
                           size_t maxbytes);


/**************************************************************************
* @purpose  Write bytes to a file
*
* @param    fd        @b{(input)} file descriptor
* @param    buffer    @b{(input)} ptr to buffer to write data
* @param    maxbytes  @b{(input)} number of bytes to write to file
*
* @returns  The number of bytes written to file
*
* @end
*
*************************************************************************/
extern L7_uint32 osapiWrite(L7_uint32 fd,
                            L7_char8* buffer,
                            size_t maxbytes);

/**************************************************************************
* @purpose  Get Operating System Error Number
*
* @returns  ERRNO set by the system.
*
* @end
*
*************************************************************************/
extern L7_uint32 osapiErrnoGet(void);



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

extern L7_double64 osapiFabs(L7_double64 x);

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

extern L7_double64 osapiFmax(L7_double64 x, L7_double64 y);

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

extern L7_double64 osapiSqrt(L7_double64 x);


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

L7_RC_t osapiSocketNonBlockingModeSet(L7_uint32 fd, L7_BOOL nbio);


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
L7_uchar8 *osapiInetNtop(L7_uint32 family, L7_uchar8 *addr, L7_uchar8 *str, L7_uint32 len);

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
L7_RC_t osapiInetPton(L7_uint32 family, L7_uchar8 *str, L7_uchar8 *addr);

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
* @param    s   @b{(input)}ipv6 string format.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @end
*
*************************************************************************/
L7_BOOL  osapiIpv6Support(L7_char8 *s);

/**************************************************************************
*
* @purpose  It finds wheather all the string part contains 4 characters or more
*           in between colons in IPV6 address.Also handles each char is valid hex didgit or not.
*           Since This condition is not handled
*           in interpeak stack.(NOTE: This function checks only four characters
*           condition only.Not any other conditions.)
*
*           Return true if string is contains not more than 4 characters
*           xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
*
*           Return false if string contains more than 4 characters in between colons
*           xxxx:xxxxxx:xxxxxxx:xxxx:xxxx:xxxx:xxxx:xxxx
*
* @param    s   @b{(input)}ipv6 string format.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @end
*
*************************************************************************/
L7_BOOL  osapiIpv6AddressFormatCheck(L7_char8 *s);

/**************************************************************************
*
* @purpose  Get socket name
*
* @param    socketDescriptor  @b{(input)} socket descriptor
* @param    saddr             @b{(output)} local address
* @param    s_len             @b{(output)} local address length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiGetsockname(L7_uint32 socketDescriptor,
                               L7_sockaddr_t *saddr,L7_uint32 *s_len);


/**************************************************************************
*
* @purpose  Get peer name
*
* @param    socketDescriptor  @b{(input)} socket descriptor
* @param    saddr             @b{(output)} remote address
* @param    s_len             @b{(output)} remote address length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiGetpeername(L7_uint32 socketDescriptor,
                               L7_sockaddr_t *saddr,L7_uint32 *s_len);


/*********************************************************************
*
* @purpose  Get name string for router interface
*
* @param    intIfNum   Internal Interface Number
* @param    buf        (output) Buffer to store name string.
* @param    buflen     Len of Buffer to store name string.
*
* @returns  L7_SUCCESS Interface is valid.
* @returns  L7_FAILURE   Interface is not valid.
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
osapiIfNameStringGet ( L7_uint32 intIfNum, L7_uchar8 *buf, L7_uint32 buflen);


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
L7_RC_t osapiIfEnable( L7_uchar8 *ifname );


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
L7_RC_t osapiIfDisable( L7_uchar8 *ifname );


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
L7_RC_t osapiInterfaceIsUp(L7_uchar8 *ifName, L7_BOOL *isUP);



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
L7_RC_t osapiRouterIfStatusGet(L7_uint32 intIfNum, L7_BOOL *isUp);

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
L7_RC_t osapiRouterIfTypeGet(L7_uint32 intIfNum, L7_uint32 *t_flags);

/*********************************************************************
* @purpose  Sets the NOARP flag on an interface
*
* @param    ifname         interface name
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfNoArpSet( L7_uchar8 *ifname );



/*********************************************************************
* @purpose  Add a primary or secondary ip4 interface address/mask
*
* @param    ifname       interface name
* @param    addr         interface address
* @param    mask         interface mask
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfNetAddrAdd( L7_uchar8 *ifname, L7_uint32 addr, L7_uint32 mask);



/*********************************************************************
* @purpose  Set the link layer address of an interface
*
* @param    ifname                interface name
* @param    lladdr                pointer to link layer address
* @param    lladdr_len            lenght of link layer address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfLLAddrSet(L7_uchar8 *ifname,L7_uchar8 *lladdr, L7_uint32 lladdr_len);


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
L7_RC_t osapiIfIpv4IcmpSendDestinationUnreachableSet (L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t osapiIfIpv4IcmpRedirectSendSet(L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t osapiIpv4IcmpIgnoreEchoRequestSet ( L7_uint32 val);

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
L7_RC_t osapiIpv4IcmpRatelimitSet ( L7_uint32 buckectSize, L7_uint32 interval);

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
L7_RC_t osapiIfIpv4MtuSet(L7_uint32 intIfNum, L7_uint32 ipv4Mtu);

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
                      L7_uint32 ttl);

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
                         L7_sockaddr_union_t *pDstAddr);

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
L7_RC_t osapiTunnelIfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  enables reception of netlink groups
*
* @parma    sd  @b{(input)}  pointer to returned descriptor
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiNetLinkSocketIpv6(L7_uint32 *sd);


/*********************************************************************
* @purpose  parse a netlink ipv6 nbr discovery report
*
* @parma    cmsg      @b{(input)}  pointer to netlink message
* @parma    type      @b{(output)}  pointer to message type
* @parma    is_router @b{(output)}  pointer to nbr is_router
* @parma    naddr     @b{(output)}  pointer to ipv6 address
* @parma    intIfNum  @b{(output)}  pointer to internal interface
* @parma    mac       @b{(output)}  pointer to mac address
* @parma    mac_len   @b{(output)}  pointer to mac address len
*
* @returns  pointer to next netlink message in chain of L7_NULL
*
* @notes    also parses interface address change messages
*
* @end
*********************************************************************/
L7_uchar8 *osapiNetLinkNDParse(L7_uchar8 *cmsg, L7_uint32 *type, L7_BOOL *is_router,
                               L7_in6_addr_t *naddr, 
                               L7_uint32 *intIfNum, L7_uchar8 *mac, L7_uint32 *mac_len);


/*********************************************************************
* @purpose  Request resolution of scoped address
*
* @parma    ip6Addr        pointer to address
* @parma    intIfNum       intIfnum of addr if link local
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiNDResolve(L7_in6_addr_t *ip6Addr, L7_uint32 intIfNum);


#endif/* OSAPISUPPORT_HEADER*/
