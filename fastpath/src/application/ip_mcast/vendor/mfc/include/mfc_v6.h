/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mfc_v6.h
*
* @purpose    Implements the Key/Main features of the Multicast
*             Forwarding Cache (MFC) module
*
* @component  Multicast Forwarding Cache (MFC)
*
* @comments   none
*
* @create     10-Jan-06
*
* @author     ddevi.
* @end
*
**********************************************************************/

extern L7_RC_t mfcIpv6SockInit();

extern L7_RC_t mfcIpv6SockDeInit();

extern L7_RC_t mfcIpv6PktTransmit(L7_uint32 rtrIfNum, struct rtmbuf *pBuf);

extern L7_RC_t mfcIpv6PktRecv(void *pktMsg);

extern L7_RC_t mfcIpv6HwInterfaceStatusChange(L7_uint32 rtrIfNum, L7_BOOL mode);

extern L7_RC_t mfcIpv6SysnetPduInterceptRegister(void);

extern L7_RC_t mfcIpv6SysnetPduInterceptDeregister(void);

