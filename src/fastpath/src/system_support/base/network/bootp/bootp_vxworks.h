/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename bootp_vxworks.h
*
* @purpose bootp and dhcp specific for vxworks os header file
*
* @component bootp and dhcp
*
* @comments none
*
* @create 
*
* @author   bmutz
*
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef INCLUDE_BOOTP_VXWORKS
#define INCLUDE_BOOTP_VXWORKS


#ifdef _L7_OS_VXWORKS_
  #include <netBufLib.h>
#endif /* _L7_OS_VXWORKS_ */

#define      SVRPORT_QUEUE       "SVRPORT-Q "
#define      SYSPORT_QUEUE       "SYSPORT-Q "

#define      IP_PROTO_UDP        0x11
   
struct psuedohdr_s
{
  L7_uint32   source_address;
  L7_uint32   dest_address;
  L7_uchar8   place_holder;
  L7_uchar8   protocol;
  L7_ushort16 length;
};
   
typedef struct l7DhcpSend
{
  struct bootp  *xmit;
  char          *dstMac;
  char          *srcMac;
  L7_ushort16    type;
  L7_uint32      srcIp;
  L7_uint32      dstIp;
  void          *pCookie;
} l7DhcpSend_t;
 
typedef struct l7DhcpMacHdr_s
{
  L7_enetHeader_t  macHdr;
  L7_ushort16      type;
}l7DhcpMacHdr_t;

extern M_BLK_ID dtlGetVxBuffer(L7_netBufHandle bufHandle);

/**************************************************************************
* @purpose  Send bootp msgs out an end device
*
* @param    l7DhcpSend_t* send structure
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None
*
* @end
*************************************************************************/
extern L7_uint32 l7DhcpSendPacket(l7DhcpSend_t *ptr);

/**************************************************************************
* @purpose  Initialize service port connection to end device
*
* @param    const L7_uchar8* interface name to connect
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None
*
* @end
*************************************************************************/
extern L7_RC_t l7SvrPortConnect(const L7_uchar8 *intf);


/**************************************************************************
* @purpose  Initialize dtl connection to end device
*
* @param    const L7_uchar8* interface name to connect
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None
*
* @end
*************************************************************************/
extern L7_RC_t l7SysPortConnect(const L7_uchar8 *intf);
 
#endif /* INCLUDE_BOOTP_VXWORKS */
