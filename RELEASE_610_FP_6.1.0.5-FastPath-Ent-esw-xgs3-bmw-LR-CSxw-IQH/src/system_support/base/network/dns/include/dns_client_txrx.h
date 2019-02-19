/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_txrx.h
*
* @purpose DNS client packet transmit/receive function prototypes
*
* @component DNS client
*
* @comments none
*
* @create 03/11/05
*
* @author dfowler
* @end
*
**********************************************************************/
#ifndef DNS_CLIENT_TXRX_H
#define DNS_CLIENT_TXRX_H


#define DNS_SERVER_SOCKET_TIMEOUT   1 /* one second */
                   

L7_RC_t dnsNameServerPacketSend(L7_inet_addr_t  *serverAddr,
                                L7_uchar8       *packet,
                                L7_ushort16     length);
L7_RC_t dnsNameServerPacketReceive(void);

void dnsNameServerActiveEntryRemove(L7_inet_addr_t *serverAddr);

void dnsNameServerRequestRemove(L7_inet_addr_t *serverAddr);

void dnsNameServerClose(dnsNameServerEntry_t *entry);

#endif /* DNS_CLIENT_TXRX_H */
