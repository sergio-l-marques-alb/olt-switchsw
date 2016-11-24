/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     peer_txrx_api.h
*
* @purpose      Wireless Peer communication API header
*
* @component    PEER_TXRX
*
* @comments     none
*
* @create       02/22/2006
*
* @author       muralikp
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_WIRELESS_PEER_TXRX_API_H
#define INCLUDE_WIRELESS_PEER_TXRX_API_H

#include "datatypes.h"
#include "sysnet_api.h"

/****************************************
*
*  Peer type enumeration
*
*****************************************/
typedef enum
{
  WS_PEER_TYPE_SWITCH =1,
  WS_PEER_TYPE_AP,
  WS_PEER_TYPE_PROVISION
}wsPeerType_t;

#define L7_WIRELESS_PEER_TXRX_TASK_TIMEOUT  1 /* 1 second */

/* Function Prototypes */

/*********************************************************************
* @purpose  Get IP Address of peer given MAC address
*
* @param    macAddr  @b{(input)} Peer MAC address
* @param    ipAddr  @b{(output)} Peer IP address
*
* @returns  L7_SUCCESS if cross reference found
* @returns  L7_FAILURE if cross reference notfound
*
* @notes    This function returns peer IP address for given MAC address.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessPeerTxRxIPAddrGet( L7_enetMacAddr_t macAddr,
                           L7_IP_ADDR_t *ipAddr );


/*********************************************************************
* @purpose  Get MAC Address of peer given IP address
*
* @param    ipAddr  @b{(input)} Peer IP address
* @param    macAddr  @b{(output)} Peer MAC address
*
* @returns  L7_SUCCESS if cross reference found
* @returns  L7_FAILURE if cross reference not found
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t wirelessPeerTxRxMACAddrGet( L7_IP_ADDR_t ipAddr,
                                    L7_enetMacAddr_t *macAddr );

/*********************************************************************
* @purpose  Open TCP connection to peer
*
* @param    peerIpAddr  @b{(input)} Peer IP address to open connection
*
* @returns  L7_SUCCESS if TCP connection open successful/already exists
* @returns  L7_FAILURE if TCP connection fails or errors
*
* @notes    This function provided to open new TCP connection with peer at
*           given Peer IP address and WIRELESS_TCP_PORT.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessTcpConnectionOpen ( L7_IP_ADDR_t peerIpAddr );

/*********************************************************************
* @purpose  Open TCP connection to peer with peer type PROVISION
*
* @param    peerIpAddr  @b{(input)} Peer IP address to open connection
*
* @returns  L7_SUCCESS if TCP connection open successful/already exists
* @returns  L7_FAILURE if TCP connection fails or errors
*
* @notes    This function provided to open new TCP connection with peer at
*           given Peer IP address and WIRELESS_TCP_PORT.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessProvisionTcpConnectionOpen ( L7_IP_ADDR_t peerIpAddr );

/*********************************************************************
* @purpose  Close TCP connection to peer
*
* @param    peerIpAddr  @b{(input)} Peer IP address to close connection
*
* @returns  L7_SUCCESS if TCP connection close is successful
* @returns  L7_FAILURE if TCP connection to peer does not exist
*
* @notes    This function is provided to close existing TCP connection with peer.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessTcpConnectionClose( L7_IP_ADDR_t peerIpAddr );


/*********************************************************************
* @purpose  Close TCP connection to peer
*
* @param    macAddr  @b{(input)} Peer MAC address to close connection
*
* @returns  L7_SUCCESS if TCP connection close is successful
* @returns  L7_FAILURE if TCP connection to peer does not exist
*
* @notes    This function is provided to close existing TCP connection with peer.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t wirelessTcpMacAddrConnectionClose( L7_enetMacAddr_t macAddr );


/*********************************************************************
* @purpose  Send Data on UDP socket to peer referencing MAC address
*
* @param    macAddr  @b{(input)} MAC address of the peer to send Data
* @param    buf      @b{(input/output)} ptr to buffer with data to transmit
* @param    len      @b{(input)} length of the data to be transmitted
*
* @returns  L7_SUCCESS if packet send is Successful
* @returns  L7_FAILURE if packet send fails
*
* @notes    This function is invoked with MAC address of the peer to send data
*           on UDP socket. MAC Address - IP Address cross reference is
*           maintained in socket database, if found the data is sent to peer at
*           referenced IP address and WIRELESS_UDP_PORT. Freeing the buffer is
*           the responsibility of the module invoking this API.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessUdpMacAddrSend( L7_enetMacAddr_t macAddr,
                                L7_uchar8 *buf,
                                L7_uint32 len );


/*********************************************************************
* @purpose  Send Data on UDP socket
*
* @param    peerIpAddr  @b{(input)} IP address of the peer to send Data
* @param    buf         @b{(input/output)} ptr to buffer with data to transmit
* @param    len         @b{(input)} length of the data to be transmitted
*
* @returns  L7_SUCCESS if packet send is Successful
* @returns  L7_FAILURE if packet send fails
*
* @notes    This function is invoked to send data on UDP socket.The data is sent
*           to peer at given IP address and WIRELESS_UDP_PORT. Freeing the buffer
*           is the responsibility of the module invoking this API.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessUdpSend( L7_IP_ADDR_t peerIpAddr,
                         L7_uchar8 *buf,
                         L7_uint32 len );



/*********************************************************************
* @purpose  Send Data on TUNNEL UDP socket
*
* @param    peerIpAddr  @b{(input)} IP address of the peer to send Data
* @param    buf         @b{(input/output)} ptr to buffer with data to transmit
* @param    len         @b{(input)} length of the data to be transmitted
*
* @returns  L7_SUCCESS if packet send is Successful
* @returns  L7_FAILURE if packet send fails
*
* @notes    This function is invoked to send data on TUNNEL UDP socket.Data is 
*           sent to peer at given IP address and WIRELESS_TNL_UDP_PORT. Freeing 
*           buffer is responsibility of module invoking this API.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessTnlUdpSend( L7_IP_ADDR_t peerIpAddr,
                            L7_uchar8 *buf,
                            L7_uint32 len );


/*********************************************************************
* @purpose  Send Data on TCP socket to peer referencing MAC Address
*
* @param    macAddr  @b{(input)} MAC address of the peer
* @param    buf      @b{(input/output)} ptr to buffer with data to transmit
* @param    len      @b{(input)} length of  data to be transmitted
*
* @returns  L7_SUCCESS if packet send Successful
* @returns  L7_FAILURE if packet send fails/connection to peer does not exist
*
* @notes    This function is invoked to send data on TCP socket.MAC Address -
*           IP Address cross reference is maintained in socket database, if
*           found the data is sent to peer on existing TCP connection. Freeing
*           the buffer is the responsibility of the module invoking this API.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessTcpMacAddrSend( L7_enetMacAddr_t macAddr,
                        L7_uchar8 *buf,
                L7_uint32 len );


/*********************************************************************
* @purpose  Send Data on TCP socket to peer referencing IP Address
*
* @param    peerIpAddr  @b{(input)} IP address of the peer
* @param    buf         @b{(input/output)} ptr to buffer with data to transmit
* @param    len         @b{(input)} length of  data to be transmitted
*
* @returns  L7_SUCCESS if packet send Successful
* @returns  L7_FAILURE if packet send fails/connection to peer does not exist
*
* @notes    This function is invoked to send data on TCP socket.The data is
*           sent to peer on existing TCP connection. Freeing the buffer is the
*           responsibility of the module invoking this API.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessTcpSend( L7_IP_ADDR_t peerIpAddr,
                         L7_uchar8 *buf,
                         L7_uint32 len );


/*********************************************************************
* @purpose  To send a message either to one of this switch's
*           managed APs, or to the peer managing the AP.
*           
* @param    L7_enetMacAddr_t @b{(input)} AP 
* @param    L7_uchar8      * @b{(input)} buf
* @param    L7_uint32        @b{(input)} len
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    It would be nice to store the IP address of the 
*           recipient in our tree, but then we wouldn't be able
*           to handle the case where an AP could be moved, from
*           being managed on one peer to another.  So we're stuck
*           with determining this address on the fly.
*
* @end
*********************************************************************/

L7_RC_t
wirelessManagedAPTcpSend(L7_enetMacAddr_t ap, 
                         L7_uchar8 * buf, L7_uint32 len);



/*********************************************************************
* @purpose  Check if TCP connection to peer exists referenced using MAC Address
*
* @param    peerMacAddr  @b{(input)} Peer MAC address to check for
*                                    existing connection
*
* @returns  L7_SUCCESS if TCP connection exists
* @returns  L7_FAILURE if TCP connection does not exist
*
* @notes    This function is provided to check if TCP connection with peer
*           exists referenced using MAC address.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessTcpMacAddrConnectionExists(L7_enetMacAddr_t peerMacAddr);


/*********************************************************************
* @purpose  Check if TCP connection to peer exists
*
* @param    peerIpAddr  @b{(input)} Peer IP address to check for existing connection
*
* @returns  L7_SUCCESS if TCP connection exists
* @returns  L7_FAILURE if TCP connection does not exist
*
* @notes    This function is provided to check if TCP connection with peer at
*           given Peer IP address exists.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessTcpConnectionExists ( L7_IP_ADDR_t peerIpAddr );



/*********************************************************************
* @purpose  TCP connection params udpate
*
* @param    peerIpAddr     @b{(input)} Peer IP address
* @param    peerSwitchIP   @b{(input)} Peer switch IP address received in intial ID message
* @param    peerMacAddr    @b{(input)} Peer MAC address to update
* @param    peerType       @b{(input)} Peer Type AP/Peer Switch
*
* @returns  L7_SUCCESS if Update Successful
* @returns  L7_FAILURE if TCP connection does not exist
*
* @notes    This function is provided to update peer MAC address and peer type
*           of TCP connection  in socket database.
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t wirelessTcpPeerParamsUpdate( L7_IP_ADDR_t peerIpAddr,
                             L7_IP_ADDR_t peerSwitchIP,
                     L7_enetMacAddr_t peerMacAddr,
                     wsPeerType_t peerType );



/*********************************************************************
* @purpose Receives L2 messages
*
* @param   bufHandle    @b{(input)} handle to the network buffer the L2 PDU is
*                                   stored in
* @param   bpduInfo     @b{(input)} pointer to sysnet structure which contains
*                                   the internal interface number that the PDU
*                                   was received on
*
* @returns L7_SUCCESS on receiving L2 Msg successful
* @returns L7_FAILURE for any error
*
* @comments This routine is registered with sysnet ethernet type 0x7777 at
*           wireless init time to receive wireless ether type L2 multicast
*           messages.
*
* @end
*********************************************************************/
L7_RC_t wirelessL2MsgReceive(L7_netBufHandle bufHandle,
                     sysnet_pdu_info_t *pduInfo);


/*********************************************************************
* @purpose Send L2 messages
*
* @param   buf    @b{(input/output)} buffer containing data to be transmitted as
*                                    L2 message
* @param   len    @b{(input)} len of the data stored in buffer
*
* @param   vlanId @b{(input)} Vlan ID to send the L2 message
*
* @returns L7_SUCCESS on a successful operation
* @returns L7_FAILURE for any error
*
* @comments This routine is provided to send wireless ether type L2 multicast
*           messages.
*
* @end
*********************************************************************/
L7_RC_t wirelessL2MsgSend(L7_uchar8 *buf, L7_uint32 len , L7_uint32 vlanId);


/*********************************************************************
* @purpose  Send Keep Alive message to peer
*
* @param    macAddr  @b{(input)} Peer MAC address
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if any errors  
*
* @notes    This function sends keep alive message to Peer.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessPeerKeepAliveMsgSend( L7_enetMacAddr_t macAddr );

/*********************************************************************
* @purpose  peer TxRx task TCP socket reinit for mutual authentication
*
* @param    mode      @b{(input)} network mutual authentication mode 
*
* @returns  none
*
* @notes    This function is invoked to reconfigure SSL context for communication
*           with AP/Peer Switch.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessPeerTxRxSSLSockReinit(L7_uint32 mode);

/*********************************************************************
* @purpose  TCP connection peer type set
*
* @param    peerIpAddr  @b{(input)} Peer IP address to update
* @param    peerType    @b{(input)} Peer Type
*
* @returns  L7_SUCCESS if Successful
* @returns  L7_FAILURE if Peer Type is not updated or any errors
*
* @notes    This function is provided to get peer type of TCP connection  in
*           socket database.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessTcpPeerTypeSet( L7_IP_ADDR_t peerIpAddr,
                                wsPeerType_t peerType );
#endif /* INCLUDE_WIRELESS_PEER_TXRX_API_H */

