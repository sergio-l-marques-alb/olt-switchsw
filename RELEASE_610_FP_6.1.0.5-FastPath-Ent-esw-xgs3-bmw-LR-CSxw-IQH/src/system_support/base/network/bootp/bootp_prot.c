/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename bootp_prot.c
*
* @purpose bootp suport file
*
* @component bootp
*
* @comments none
*
* @create 
*
* @author     Deepesh Aggarwal
*
* @end
*             
**********************************************************************/

#include <stdio.h>
#include "l7_bootp.h"
#include "l7netapi.h"
#include "l7_socket.h"
#include "bspapi.h"
#include "dns_client_api.h"

/* cant include dtlapi.h */
#define L7_DTL_PORT_IF "dtl"

static L7_char8 *_bootp_op[] = {"", "REQUEST", "REPLY"};
static L7_char8 *_bootp_hw_type[] = {"", "Ethernet", "Exp Ethernet", "AX25",
  "Pronet", "Chaos", "IEEE802", "Arcnet"};

#ifdef FEAT_METRO_CPE_V1_0
extern L7_ulong32 bootpDhcpServerAddress;
#endif

/**************************************************************************
* @purpose  To get the actual packet size of an initialized buffer
*
* @param    L7_bootp_t   BOOTP structure
* 
* @returns  L7_int32 Size of the packet
*
* @comments None.
*
* @end
*************************************************************************/
L7_int32 bootp_size( L7_bootp_t *ppkt )
{
  L7_bp_value_t *bpTagPtr;
  L7_int32  size;

  bpTagPtr = (L7_bp_value_t *)(&ppkt->bp_vend[4]);   /* First check for the cookie!*/
  
  if ( ppkt->bp_vend[0] !=  99 ||
       ppkt->bp_vend[1] != 130 ||
       ppkt->bp_vend[2] !=  83 ||
       ppkt->bp_vend[3] !=  99 )
  {
    LOG_MSG( "bootp_size():Bad BOOTP cookie" );
    return 0;
  }
  
  while ( bpTagPtr->tag != L7_TAG_END )
  {
    bpTagPtr = (L7_bp_value_t *)( &(bpTagPtr->data) + bpTagPtr->dataLen);  /* jump to next existing TAG */

    if ( (L7_uchar8*)bpTagPtr > &ppkt->bp_vend[L7_BP_VEND_LEN - 1] )
    {
      LOG_MSG( "bootp_size(): Oversize BOOTP packet in bootp_size" );
      return 0;
    }
  }
  size = (L7_uchar8*)bpTagPtr - (L7_uchar8 *)ppkt + 1;
  return size;
}

/**************************************************************************
* @purpose   In case factory default static configuration exists resolve
             the rest of bootp parameters
*
* @param     L7_uchar8	 @b{(input)}  Ptr to Interface name
* @param     L7_bootp_t  @b{(output)} Ptr to L7_bootp_t structure Received
* @param     L7_int32    @b{(input)}  Timeout in seconds
*
* @returns   L7_TRUE  if receives the reply packet successfully 
* @returns   L7_FALSE if any error occurs
*
* @comments None.
*
* @end
*************************************************************************/
L7_BOOL bootpVendorSpecificValuesRequest(const L7_uchar8 *intf, L7_bootp_t *bootpReplyPacket, L7_int32 timeout)
{
  L7_int32  clientSocket = L7_NULL;                /* socket descriptor */
  L7_int32  maxFdLength  = L7_NULL;                /* number of file descriptors in set */
  L7_int32  optionSetOn  = 1;                      /* used to set socket option On */
  L7_int32  start_clock  = L7_NULL;                /* used for timeout checking */
  L7_int32  check_clock  = L7_NULL;                /* used for timeout checking */
  L7_BOOL   timeout_flag = L7_FALSE;               /* used for timeout checking */
  L7_BOOL   seen_correct_bootp_reply = L7_FALSE;   /* indicates correct BOOTP reply received */
  L7_uint32 xid = L7_NULL;                         /* BOOTP transaction ID */
  L7_uint32 currentIpAddr = L7_NULL;               /* default static IP address on transmitting interface */
  L7_uint32 dnsSvrIpAddr  = L7_NULL;               /* DNS server IP address */
  L7_uint32 bytesSent = L7_NULL;                   /* bytes sent */
  L7_uint32 bytesRcvd = L7_NULL;                   /* bytes received */
  L7_uint32 from_len  = sizeof(L7_sockaddr_in_t);  
  L7_uchar8 mincookie[] = L7_VENDOR_MAGIC_COOKIE;  /* BOOTP vendor section prologue */
  L7_uchar8 macAddress[L7_BP_CHADDR_LEN];          /* mac address of transmitting interface to be set in request packets */
  L7_uchar8 ttlValue = L7_BP_IP_TTL_NUMBER;        /* IP TTL number to be set in request packets */
  L7_uchar8 intfName[IFNAMSIZ];                    /* interface name */
  fd_set    readFds;                               /* file desciptors set */
 
  L7_sockaddr_in_t baddr, saddr, raddr;            /* socket endpoints addresses */
  L7_bootp_t rx_local;                             /* local BOOTP packet structure to save received packet */
  L7_bootp_t *received = &rx_local;                /* pointer to received BOOTP packet */
  L7_bootp_t *xmit     = bootpReplyPacket;         /* pointer to BOOTP packet to return */
  
  L7_char8   domainNameList[L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX];  /* list of DNS servers received */   
  L7_inet_addr_t dnsSvrInetAddr;

  memset(macAddress, 0, sizeof(macAddress));
  memset(intfName, 0, sizeof(intfName));
  memset(&baddr, 0, sizeof(baddr));  
  memset(&saddr, 0, sizeof(saddr));  
  memset(&raddr, 0, sizeof(raddr));  
  memset(domainNameList, 0, sizeof(domainNameList));    

  FD_ZERO(&readFds);

  bootpConfigSemGet();  
  if ( strcmp(bspapiServicePortNameGet(), intf) == L7_NULL )
  {
    /* Get the Mac Address of the Corresponding netwotk Interface*/  
    simGetServicePortBurnedInMac((L7_uchar8 *) macAddress);
    currentIpAddr = simGetServPortIPAddr();
    osapiSnprintf(intfName, sizeof(intfName), "%s0", bspapiServicePortNameGet());	
  }
  else if ( strcmp(L7_DTL_PORT_IF, intf) == L7_NULL )
  {
    /* Get the Mac Address of the Corresponding netwotk Interface*/  
    if ( simGetSystemIPMacType() == L7_SYSMAC_BIA ) 
    {
      simGetSystemIPBurnedInMac((L7_uchar8 *) macAddress);
    }
    else 
    {
      simGetSystemIPLocalAdminMac((L7_uchar8 *) macAddress);
    }    
	
    currentIpAddr = simGetSystemIPAddr();
    osapiSnprintf(intfName, sizeof(intfName), "%s0", L7_DTL_PORT_IF);	
  }  
  bootpConfigSemFree();
  
  /*Generate unique Transaction Id */
  xid = (L7_uint32)bootpReplyPacket + (L7_uint32) (0xffffffff & osapiUpTimeRaw());

  /*Create a socket*/
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &clientSocket) == L7_FAILURE)
  {
    LOG_MSG("bootpVendorSpecificValuesRequest(): Failed to create socket");
    return L7_FALSE;
  }

  /* Set options to reuse the Address and Port*/
  if ( osapiSetsockopt(clientSocket, L7_SOL_SOCKET, L7_SO_REUSEADDR,(L7_char8 *) &optionSetOn, sizeof(optionSetOn)) == L7_FAILURE )
  {
    LOG_MSG("bootpVendorSpecificValuesRequest(): osapiSetsockopt SO_REUSEADDR failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }

#ifdef _L7_OS_VXWORKS_
  if ( osapiSetsockopt(clientSocket, L7_SOL_SOCKET, L7_SO_BINDTODEVICE, intfName, IFNAMSIZ) )
  {
    LOG_MSG("bootpVendorSpecificValuesRequest(): osapiSetsockopt SO_BINDTODEVICE failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
   }
#endif

  if ( osapiSetBroadAddr(clientSocket, intf) != L7_SUCCESS)
  {
    LOG_MSG("bootpVendorSpecificValuesRequest(): osapiSetBroadAddr failed ");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }

  if (osapiSetsockopt(clientSocket, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &optionSetOn, sizeof(optionSetOn))== L7_FAILURE)
  {
    LOG_MSG("bootpVendorSpecificValuesRequest(): osapiSetsockopt SO_BROADCAST failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }
  
  if ( osapiSetsockopt(clientSocket, IPPROTO_IP, L7_IP_MULTICAST_TTL, (L7_char8 *) &ttlValue, sizeof(ttlValue)) == L7_FAILURE )
  {
    LOG_MSG("bootpVendorSpecificValuesRequest(): osapiSetsockopt IP_MULTICAST_TTL failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }

  baddr.sin_family = L7_AF_INET;
  baddr.sin_port = osapiHtons(L7_IPPORT_BOOTPC);
  baddr.sin_addr.s_addr = currentIpAddr;
  
  if (osapiSocketBind(clientSocket, (L7_sockaddr_t *)&baddr, sizeof(baddr)) == L7_FAILURE )
  {
    LOG_MSG("bootpVendorSpecificValuesRequest(): osapiSocketBind error");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }

  /* Fill in the BOOTP request packet*/
  bzero((char *)xmit, sizeof(*xmit));

  xmit->bp_op     = L7_BOOT_REQUEST;
  xmit->bp_htype  = L7_HTYPE_ETHERNET;
  xmit->bp_hlen   = L7_MAC_ADDR_LEN;
  xmit->bp_xid    = xid;
  xmit->bp_secs   = 0;
  xmit->bp_flags  = osapiHtons(0x0000); /* BROADCAST FLAG*/
  xmit->bp_ciaddr = currentIpAddr;

  bcopy((L7_uchar8 *) macAddress, (L7_uchar8 *)&xmit->bp_chaddr, xmit->bp_hlen);
  bcopy(mincookie, xmit->bp_vend, sizeof(mincookie));

#ifdef BOOTP_DEBUG
  LOG_MSG( "\n---------Bootp Packet sending-----------" );
  L7_bootp_show( intf, xmit );
#endif

  saddr.sin_family = L7_AF_INET;
  saddr.sin_port = osapiHtons(L7_IPPORT_BOOTPS);
  saddr.sin_addr.s_addr = osapiHtonl(L7_INADDR_BROADCAST);
  
  if (osapiSocketSendto(clientSocket, (L7_char8 *)xmit, sizeof(L7_bootp_t), 0, (L7_sockaddr_t *)&saddr, sizeof(saddr),
                        &bytesSent) != L7_SUCCESS)
  {
    LOG_MSG("bootpVendorSpecificValuesRequest(): osapiSocketSendto error");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }
  else
  {
#ifdef BOOTP_DEBUG
    LOG_MSG(" ---- BOOTP Request Packet sent successfully ---- \n");
#endif
  }

  seen_correct_bootp_reply = L7_FALSE;
  timeout_flag = L7_FALSE;

  /*point to the Bootp Reply packet*/
  maxFdLength = clientSocket+1;

  while ((seen_correct_bootp_reply != L7_TRUE) && (timeout_flag != L7_TRUE))
  {
    bzero((L7_char8 *)received, sizeof(*received));

    start_clock = osapiUpTimeRaw();
#ifdef BOOTP_DEBUG
    LOG_MSG(" ---- Waiting for the BOOTP Reply ---- ");
#endif

    FD_ZERO(&readFds);
    FD_SET(clientSocket, &readFds);

    if (osapiSelect(maxFdLength, &readFds, NULL, NULL, timeout, 0) <= 0)
    {
      LOG_MSG("bootpVendorSpecificValuesRequest():Timeout ");
      osapiSocketClose(clientSocket);
      timeout_flag = L7_TRUE;
    }

    /* Receive the Packet*/
    if (osapiSocketRecvfrom(clientSocket, (L7_char8*)received, sizeof(L7_bootp_t), 0,
                            (L7_sockaddr_t *)&raddr, &from_len, &bytesRcvd ) == L7_ERROR)
    {
      LOG_MSG("bootpVendorSpecificValuesRequest(): Error in Receiving the packet");
      osapiSocketClose(clientSocket);
      return L7_FALSE;
    }
    else
    {
      /* check for the Transaction Id to identify the Bootp Reply of the Bootp Request sent*/
      if ( received->bp_xid != xid )
      {
#ifdef BOOTP_DEBUG
        LOG_MSG(" --Transaction ID does not match with the Received packet-- ");
#endif
        check_clock = osapiUpTimeRaw();
        if ( (check_clock - start_clock) < timeout )
        {
          timeout -= (check_clock - start_clock);
        }
      }
      else
      {
#ifdef BOOTP_DEBUG
        LOG_MSG( "\n ---- BOOTP Reply received ---- " );
        L7_bootp_show(intf, received);
#endif 
        /* Save the good packet in *xmit */
        bcopy( (L7_char8*)received, (L7_char8*) xmit, bootp_size(received) );      
        seen_correct_bootp_reply = L7_TRUE;
		
        if (L7_bootp_option_get(received, L7_TAG_DOMAIN_SERVER, &dnsSvrIpAddr))
        {
          /* if domain name sever option is found in response,
             add it dns name server list */
          inetAddressZeroSet(L7_AF_INET, &dnsSvrInetAddr);
          inetAddressSet(L7_AF_INET, &dnsSvrIpAddr, &dnsSvrInetAddr);
          if (dnsClientNameServerEntryAdd(&dnsSvrInetAddr) != L7_SUCCESS)
          {
            LOG_MSG("bootpVendorSpecificValuesRequest(): DNS name server entry add failed");
          }
        }
        
		if (L7_bootp_option_get(received, L7_TAG_DOMAIN_NAME, domainNameList))
        {
          /* if domain name List option is found in response,
             add it to dns domain name List */
          if (dnsClientDomainNameListAdd(domainNameList) != L7_SUCCESS)
          {
            LOG_MSG("bootpVendorSpecificValuesRequest(): DNS domain name list entry add failed");
          }
        }
      }
    }/*End of else*/
  }/*End of while*/

  if (timeout_flag == L7_TRUE)
  {
    return L7_FALSE;
  }

  /*Close the Socket*/
  osapiSocketClose(clientSocket);
  return L7_TRUE;

}

/**************************************************************************
* @purpose   This Function implements the basic bootp functionality means
*            sends a BOOTP Request and then Wait for the BOOTP Reply 
*            packet till the time specified by timeout parameter
*
* @param     intf	  @b{(input)} Ptr to Interface name
* @param     res     @b{(output)} Ptr to L7_bootp_t structure Received
* @param     timeout @b{(input)} Timeout in Sec
*
* @returns  L7_TRUE  if receives the reply packet successfully 
* @returns  L7_FALSE if any error occurs
*
* @comments None.
*
* @end
*************************************************************************/

L7_BOOL
L7_bootp_do(const L7_uchar8 *intf, L7_bootp_t *res, L7_int32 timeout)
{
  L7_int32 s,maxFdLength;
  L7_int32 start_clock,check_clock,one = 1;
  L7_int32 seen_correct_bootp_reply;
  L7_int32 timeout_flag; 
  L7_uint32 xid;
  L7_uint32 dnsSvrIpAddr  = L7_NULL;               /* DNS server IP address */
  L7_uchar8 mincookie[] = L7_VENDOR_MAGIC_COOKIE;
  L7_uchar8 * macAddress[L7_BP_CHADDR_LEN];
  L7_uchar8 ttlValue = L7_BP_IP_TTL_NUMBER;
  L7_bootp_t *received;
  L7_bootp_t *xmit;
  L7_uchar8 buffer[IFNAMSIZ];
  L7_uint32 bytesSent,bytesRcvd;
  fd_set readFds;
  L7_ushort16 port;
  L7_uint32 ipadd,from_len;
  L7_sockaddr_in_t baddr,saddr,raddr;
  L7_inet_addr_t dnsSvrInetAddr;

  L7_char8 domainNameList[L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX];  /* list of DNS servers received */    

  memset(domainNameList, 0, sizeof(domainNameList));  
  
  /* point to the Bootp Request Packet */
  xmit=res;

  bootpConfigSemGet ();
  /* Find the Inteface and set its address to 0.0.0.0 */
  if (strcmp(bspapiServicePortNameGet(), intf)==0)
  {
    sprintf(buffer, "%s0", bspapiServicePortNameGet());
    if (osapiNetIfConfig(buffer, 0x0 ,0xffffff00 )!= L7_SUCCESS)
    {
      LOG_MSG("do_bootp(): osapiNetIfConfig Failed for Service Interface");
    }
  }
  else if (strcmp(L7_DTL_PORT_IF, intf)==0)
  {
      simSetSystemIPAddr(0,L7_FALSE);
      sprintf(buffer, "%s0", L7_DTL_PORT_IF);
      osapiNetIPSet(buffer, 0);
  }
  bootpConfigSemFree ();

#ifdef _L7_OS_LINUX_
  /* This is required on linux, but harmful on VxWorks. VxWorks installs a hidden
   * route to 255.255.255.255 at startup. Adding the same route here would either fail
   * or just bump the ref count in the stack's routing table. Then if DHCP fails
   * for example if the mgmt interface is down, removing this route will remove
   * the route installed by VxWorks itself. Then sending to 255.255.255.255 will
   * never work again. One might wonder why you would even do a routing table lookup
   * for a broadcast packet, but there it is. */
  osapimRouteEntryAdd (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif

  /*Generate unique Transaction Id */
  xid = (L7_uint32)res + (L7_uint32) (0xffffffff & osapiUpTimeRaw());

  /*Create a socket*/
  

  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &s) == L7_FAILURE)
  {
    LOG_MSG("do_bootp(): Failed to create socket");

#ifdef _L7_OS_LINUX_
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    return L7_FALSE;
  }

  /* Set options to reuse the Address and Port*/
  if ( osapiSetsockopt(s, L7_SOL_SOCKET, L7_SO_REUSEADDR,(L7_char8 *) &one, sizeof(one)) == L7_FAILURE )
  {
    LOG_MSG("do_bootp(): osapiSetsockopt SO_REUSEADDR failed");
#ifdef _L7_OS_LINUX_
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    osapiSocketClose (s);
    return L7_FALSE;
  }

#ifdef _L7_OS_VXWORKS_
  if ( osapiSetsockopt(s, L7_SOL_SOCKET, L7_SO_BINDTODEVICE, buffer, IFNAMSIZ) )
  {
    LOG_MSG("do_bootp(): osapiSetsockopt SO_BINDTODEVICE failed");
#ifdef _L7_OS_LINUX_
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    osapiSocketClose (s);
    return L7_FALSE;
   }
#endif

  if ( osapiSetBroadAddr(s,intf) != L7_SUCCESS)
  {
    LOG_MSG("do_bootp(): osapiSetBroadAddr failed ");
#ifdef _L7_OS_LINUX_
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    osapiSocketClose (s);
    return L7_FALSE;
  }

  if (osapiSetsockopt(s, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &one, sizeof(one))== L7_FAILURE)
  {
    LOG_MSG("do_bootp(): osapiSetsockopt SO_BROADCAST failed");
#ifdef _L7_OS_LINUX_
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    osapiSocketClose (s);
    return L7_FALSE;
  }
  
  if ( osapiSetsockopt(s, IPPROTO_IP, L7_IP_MULTICAST_TTL, (L7_char8 *) &ttlValue, sizeof(ttlValue)) == L7_FAILURE )
  {
    LOG_MSG("do_bootp(): osapiSetsockopt IP_MULTICAST_TTL failed");
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
    osapiSocketClose (s);
    return L7_FALSE;
  }

  baddr.sin_family = L7_AF_INET;
  baddr.sin_port = osapiHtons(L7_IPPORT_BOOTPC);
  baddr.sin_addr.s_addr = L7_INADDR_ANY;
  if (osapiSocketBind(s,(L7_sockaddr_t *)&baddr,sizeof(baddr)) == L7_FAILURE )
  {
    LOG_MSG("do_bootp(): osapiSocketBind error");
    osapiSocketClose (s);
#ifdef _L7_OS_LINUX_
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    return L7_FALSE;
  }

  /* Get the Mac Address of the Corresponding netwotk Interface*/
  bootpConfigSemGet ();
  if (strcmp(bspapiServicePortNameGet(), intf)==0)
  {
    simGetServicePortBurnedInMac((L7_uchar8 *) macAddress);
  }
  else if (strcmp(L7_LOCAL_NETWORK_PORT_IF, intf)==0)
  {
    if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
      simGetSystemIPBurnedInMac((L7_uchar8 *) macAddress);
    else
      simGetSystemIPLocalAdminMac((L7_uchar8 *) macAddress);
  }
  bootpConfigSemFree ();

  /* Fill in the BOOTP request packet*/
  bzero((char *)xmit, sizeof(*xmit));

  xmit->bp_op = L7_BOOT_REQUEST;
  xmit->bp_htype = L7_HTYPE_ETHERNET;
  xmit->bp_hlen = L7_MAC_ADDR_LEN;
  xmit->bp_xid = xid;
  xmit->bp_secs = 0;
  xmit->bp_flags = osapiHtons(0x8000); /* BROADCAST FLAG*/

  bcopy((L7_uchar8 *) macAddress, (L7_uchar8 *)&xmit->bp_chaddr, xmit->bp_hlen);
  bcopy(mincookie, xmit->bp_vend, sizeof(mincookie));

#ifdef BOOTP_DEBUG
  LOG_MSG( "\n---------Bootp Packet sending-----------" );
  L7_bootp_show( intf, xmit );
#endif

  saddr.sin_family = L7_AF_INET;
  saddr.sin_port = osapiHtons(L7_IPPORT_BOOTPS);
  saddr.sin_addr.s_addr = osapiHtonl(L7_INADDR_BROADCAST);
  if (osapiSocketSendto(s,(L7_char8 *)xmit,sizeof(L7_bootp_t), 0,(L7_sockaddr_t *)&saddr,sizeof(saddr),
                        &bytesSent) != L7_SUCCESS)
  {
    LOG_MSG("do_bootp(): osapiSocketSendto error");
#ifdef _L7_OS_LINUX_
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    osapiSocketClose (s);
    return L7_FALSE;
  }
  else
  {
#ifdef BOOTP_DEBUG
    LOG_MSG(" ---- BOOTP Request Packet sent successfully ---- \n");
#endif
  }

  seen_correct_bootp_reply = L7_FALSE;
  timeout_flag = L7_FALSE;

  /*point to the Bootp Reply packet*/
  received = res;
  maxFdLength= s+1;

  while ((seen_correct_bootp_reply != L7_TRUE) && (timeout_flag != L7_TRUE))
  {
    bzero((L7_char8 *)received, sizeof(*received));

    start_clock=osapiUpTimeRaw();
#ifdef BOOTP_DEBUG
    LOG_MSG(" ---- Waiting for the BOOTP Reply ---- ");
#endif

    FD_ZERO(&readFds);
    FD_SET(s, &readFds);

    if (osapiSelect(maxFdLength, &readFds, NULL, NULL, timeout, 0) <= 0)
    {
      LOG_MSG("do_bootp():Timeout ");
      osapiSocketClose(s);
      timeout_flag = L7_TRUE;
    }

    /* Receive the Packet*/
    from_len = sizeof(raddr);
    if (osapiSocketRecvfrom(s,(L7_char8*)received, sizeof(L7_bootp_t), 0,
                            (L7_sockaddr_t *)&raddr,&from_len,&bytesRcvd ) == L7_ERROR)
    {
      LOG_MSG("do_bootp(): Error in Receiving the packet");
#ifdef _L7_OS_LINUX_
      /*delete route entry for 0.0.0.0 IP address */
      osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
      osapiSocketClose(s);
      return L7_FALSE;
    }
    else
    {
      port = osapiNtohs(raddr.sin_port);
      ipadd = osapiNtohl(raddr.sin_addr.s_addr);
      /* check for the Transaction Id to identify the Bootp Reply of the Bootp Request sent*/
      if ( received->bp_xid != xid )
      {
#ifdef BOOTP_DEBUG
        LOG_MSG(" --Transaction ID does not match with the Received packet-- ");
#endif
        check_clock = osapiUpTimeRaw();
        if ( (check_clock - start_clock) < timeout )
        {
          timeout -= (check_clock - start_clock);
        }
      }
      else
      {
#ifdef BOOTP_DEBUG
        LOG_MSG( "\n ---- BOOTP Reply received ---- " );
        L7_bootp_show(intf,received);
#endif

#ifdef FEAT_METRO_CPE_V1_0
        bootpDhcpServerAddress = received->bp_siaddr;
#endif

        seen_correct_bootp_reply = L7_TRUE;
        
		if (L7_bootp_option_get(received, L7_TAG_DOMAIN_SERVER, &dnsSvrIpAddr))
        {
          /* if domain name sever option is found in response,
             add it dns name server list */
          inetAddressZeroSet(L7_AF_INET, &dnsSvrInetAddr);
          inetAddressSet(L7_AF_INET, &dnsSvrIpAddr, &dnsSvrInetAddr);
          if (dnsClientNameServerEntryAdd(&dnsSvrInetAddr) != L7_SUCCESS)
          {
            LOG_MSG("L7_bootp_do(): DNS name server entry add failed");
          }
        }
        
		if (L7_bootp_option_get(received, L7_TAG_DOMAIN_NAME, domainNameList))
        {
          /* if domain name List option is found in response,
             add it to dns domain name List */
          if (dnsClientDomainNameListAdd(domainNameList) != L7_SUCCESS)
          {
            LOG_MSG("L7_bootp_do(): DNS domain name list entry add failed");
          }
        }		
      }
    }/*End of else*/
  }/*End of while*/

  if (timeout_flag == L7_TRUE)
  {
#ifdef _L7_OS_LINUX_
    /*delete route entry for 0.0.0.0 IP address */
    osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    return L7_FALSE;
  }

#ifdef _L7_OS_LINUX_
  /*delete route entry for 0.0.0.0 IP address */
  osapimRouteEntryDelete (buffer, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif

  /*Close the Socket*/
  osapiSocketClose(s);
  return L7_TRUE;
}




/**************************************************************************
* @purpose  Interrogate a bootp record for a particular option 
*
* @param    bp      @b{(input)} Ptr to L7_bootp_t structure
* @param    tag		@b{(input)} Tag Identity
* @param    opt     @b{(output)} Ptr to option Value
*
* @returns  L7_TRUE  if the specified option found in the packet
* @returns  L7_FALSE if couldn't find the specified option 
*
* @comments None.
*
* @end
*************************************************************************/

L7_BOOL
L7_bootp_option_get(L7_bootp_t *bp, L7_uchar8 tag, void *opt)
{
  L7_uchar8 *val; 
  L7_uint32 i;
  L7_uchar8 *op; 
  L7_uchar8 *bufEnd;

  val= (L7_uchar8 *)opt;
  op = &(bp->bp_vend[4]);
  bufEnd = &bp->bp_vend[L7_BP_VEND_LEN - 1];

  while (*op != L7_TAG_END && op <= bufEnd)
  {
    if (*op == tag)
    {
      for (i = 0;  i < *(op+1);  i++)
      {
        *val++ = *(op+i+2);             
      }
      return L7_TRUE;                        
    }
    op += (*(op+1) + 2);                        
  }    
  LOG_MSG("Option not specified in the Vendor field");
  LOG_MSG("Possible Reason:This Tag value not specified on BOOTP server");
  return L7_FALSE;
}

/*************************************************************************
* @purpose  To show bootp parameters
*
* @param    intf     @b{(input)} Ptr to Interface Name
* @param    bp       @b{(input)} Ptr to L7_bootp_t structure
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/

void
L7_bootp_show(const L7_char8 *intf, L7_bootp_t *bp)
{
  L7_int32 i, len;
  L7_uchar8 *op, *ap = 0;
  L7_uchar8 name[128];
  L7_ulong32 addr[32];

  LOG_MSG("BOOTP[%s] op: %s\n", intf, _bootp_op[bp->bp_op]);
  LOG_MSG("       htype: %s\n", _bootp_hw_type[bp->bp_htype]);
  LOG_MSG("        hlen: %d\n", bp->bp_hlen );
  LOG_MSG("        hops: %d\n", bp->bp_hops );
  LOG_MSG("         xid: 0x%x\n", bp->bp_xid );
  LOG_MSG("        secs: %d\n", bp->bp_secs );
  LOG_MSG("       flags: 0x%x\n", bp->bp_flags );
  LOG_MSG("       hw_addr: ");

  for (i = 0;  i < bp->bp_hlen;  i++)
  {
    LOG_MSG("%02x", bp->bp_chaddr[i]);
    if (i != (bp->bp_hlen-1)) LOG_MSG(":");
  }
  LOG_MSG("     client IP: %s\n", osapiInet_ntoa(bp->bp_ciaddr));
  LOG_MSG("         my IP: %s\n", osapiInet_ntoa(bp->bp_yiaddr));
  LOG_MSG("     server IP: %s\n", osapiInet_ntoa(bp->bp_siaddr));
  LOG_MSG("    gateway IP: %s\n", osapiInet_ntoa(bp->bp_giaddr));
  LOG_MSG("        server: %s\n", bp->bp_sname);
  LOG_MSG("          file: %s\n", bp->bp_file);

  if (bp->bp_vend[0])
  {
    LOG_MSG("  options:\n");
    op = &bp->bp_vend[4];

    while (*op != L7_TAG_END)
    {
      switch (*op)
      {
      case L7_TAG_SUBNET_MASK:
      case L7_TAG_GATEWAY:
      case L7_TAG_DOMAIN_SERVER:
        ap = (L7_uchar8 *)&addr[0];
        len = *(op+1);
        for (i = 0;  i < len;  i++)
        {
          *ap++ = *(op+i+2);
        }
        if (*op == L7_TAG_SUBNET_MASK)   ap =  "  subnet mask";
        if (*op == L7_TAG_GATEWAY)       ap =  "      gateway";
        if (*op == L7_TAG_DOMAIN_SERVER) ap =  "domain server";
        LOG_MSG("      %s: ", ap);
        ap = (L7_uchar8 *)&addr[0];

        while (len > 0)
        {
          LOG_MSG("%s", osapiInet_ntoa(*(L7_ulong32 *)ap));
          len -= sizeof(L7_ulong32);
          ap += sizeof(L7_ulong32);
          if (len) LOG_MSG(", ");
        }
        break;
      case L7_TAG_HOST_NAME:
        for (i = 0;  i < *(op+1);  i++)
        {
          name[i] = *(op+i+2);
        }
        name[*(op+1)] = '\0';

        if (*op == L7_TAG_HOST_NAME)   ap =  "   host name";
        LOG_MSG("       %s: %s\n", ap, name);
        break;

      default:
        LOG_MSG("Unknown option: %x/%d.%d:", *op, *op, *(op+1));
        for ( i = 2; i < 2 + op[1]; i++ )
          LOG_MSG(" %d",op[i]);
        break;
      }                
      op += *(op+1)+2; 
    }/* End of while loop */
  }/*End of if loop */
}


/* End of file */

