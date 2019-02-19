/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mfc_v6_stub.c
*
* @purpose    Implements the Packet Reception/Transmission mechanisms
*             of the Multicast Forwarding Cache (MFC) module for IPv6
*
* @component  Multicast Forwarding Cache (MFC)
*
* @comments   This is a wrapper in case of IPv4-only builds.
*
* @create     10-Jan-06
*
* @author     ddevi.
* @end
*
**********************************************************************/
#include "defaultconfig.h"
#include "l3_comm_structs.h"
#include "sysnet_api.h"
#include "rtiprecv.h" 
#include "mfc_debug.h"
#include "mfc_v6.h"

/*********************************************************************
* @purpose  Stub Routine to return the Data Socket fd 
*
* @param    family @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
*
* @returns  L7_SUCCESS, if operation successful
* @returns  L7_FAILURE, if operation failed
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SockFdGet(L7_uchar8 family,L7_uint32 *pMfcIp6SockId)
{
   MFC_DEBUG(MFC_DEBUG_FAILURES," Error trying to get V6 socket in V4 build \n");
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Stub Routine to initializes the Ipv6 Data Rx/Tx sockets
*
* @param    none
*
* @returns  L7_SUCCESS  Initialization successful.
* @returns  L7_FAILURE  Initialization failed.
*
* @notes    As of now, we only have IPv6 socket support. IPv4 packets
*           do not use the socket mechanism but rather the sysnet hooks.
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SockInit()
{
   MFC_DEBUG(MFC_DEBUG_FAILURES," Error trying to initialize V6 socket in V4 build \n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Stub Routine to deinitializes the Ipv6 Data Rx/Tx sockets 
*
* @param    none
*
* @returns  L7_SUCCESS   DeInitialization successful.
* @returns  L7_FAILURE   DeInitialization failed.
*
* @notes    As of now, we only have IPv6 socket support. IPv4 packets
*           do not use the socket mechanism but rather the sysnet hooks.
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SockDeInit()
{
   MFC_DEBUG(MFC_DEBUG_FAILURES," Error trying to deinitialize V6 socket in V4 build \n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Stub Routine to transmit the given IPv6 packet on a specific outgoing interface
*
* @param    rtrIfNum  @b{(input)}router interface number of the outgoing interface
* @param     pBuf     @b{(input)} pointer to the buffer containing the packet to be sent
*
* @returns  L7_SUCCESS, Packet forwarded successfully.
* @returns  L7_FAILURE, Packet forwarding failed.
*
* @notes    CAUTION : 1. Both the given frame-buffer and the rtm-buffer would
*                        be freed-up by this function irrespective of the return value.
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6PktTransmit(L7_uint32 rtrIfNum, struct rtmbuf *pBuf)
{
   MFC_DEBUG(MFC_DEBUG_FAILURES," Error trying to tranmit V6 data pkt in V4 build \n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Stub Routine to process all the IPv6 Multicast Data Packets received.
*
* @param    sockFd    @b{(input)}Socket Fd to receive the packets from
*
* @returns  L7_TRUE   Packet was received and processed
* @returns  L7_FALSE  No Packet was received.
*
* @notes    Be it success or failure, the given buffer is preserved
*           intact. It is freed-up irrespective of return value
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6PktRecv(void  *pktMsg)
{
   MFC_DEBUG(MFC_DEBUG_FAILURES," Error trying to receive V6 data pkt in V4 build \n");
  return L7_FAILURE;
}  
/*********************************************************************
* @purpose  Stub Routine to updates interface status in IP stack
*
* @param    rtrIfNum  @b{(input)} interface whose statuc has changed       
* @param    mode      @b{(input)} L7_ENABLE/ L7_DISABLE
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    As of now, we only have IPv6 socket support. 
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6HwInterfaceStatusChange(L7_uint32 rtrIfNum, L7_BOOL mode)
{
   MFC_DEBUG(MFC_DEBUG_FAILURES," Error updating interface V6 multicast mode in IP stack"
           " in V4 build \n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Stub Routine to register with sysnet to intercept Ipv6 packets before 
*           copied onto stack
*
* @param    none
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE 
*
* @notes    
*       
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SysnetPduInterceptRegister(void)
{
   MFC_DEBUG(MFC_DEBUG_FAILURES," Error registering to sysnet to receive V6 data pkt"
           "in V4 build \n");
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Stub Routine to DeRegister with sysnet to intercept Ipv6 packets  
*
* @param    none
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE 
*
* @notes    
*       
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SysnetPduInterceptDeregister(void)
{
   MFC_DEBUG(MFC_DEBUG_FAILURES," Error deregistering to sysnet to receive V6 data pkt"
           "in V4 build \n");
  return L7_FAILURE;
}


