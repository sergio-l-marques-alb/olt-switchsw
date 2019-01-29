/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_txrx.h
*
* @purpose RADIUS Client transmit and receive header file
*
* @component radius
*
* @comments none
*
* @create 03/28/2003
*
* @author spetriccione
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_RADIUS_TXRX_H
#define INCLUDE_RADIUS_TXRX_H

/*
** Internal function prototypes
*/

typedef enum
{
  L7_RADIUS_RX_STS_PKT_MALFORMED = 1,
  L7_RADIUS_RX_STS_SENDER_UNKNOWN,
  L7_RADIUS_RX_STS_INVALID_MSG_CODE,
  L7_RADIUS_RX_STS_ERROR_CODE_MAX
} socketRxStatus_t;

L7_RC_t radiusServerOpen(radiusServerEntry_t *newEntry);

/*********************************************************************
*
* @purpose Handler for receiving the messages from RADIUS Auth & Acct servers.
*
* @param void.
*
* @returns None.
*
* @comments
*
* @end
*
*********************************************************************/
void radiusPacketReceive(void);

L7_RC_t radiusPacketSend(radiusServerEntry_t *serverEntry, 
                         radiusRequest_t *request);

void radiusServerClose(radiusServerEntry_t *serverEntry, L7_uint32 index);

L7_RC_t radiusResolveServerIPAddress(radiusServerEntry_t *serverEntry);

radiusServerEntry_t *radiusResolveGetNextServerIPAddress(radiusServerEntry_t *serverEntry);

void radiusResolveAllServerIPAddresses(radiusServerType_t type);
void radiusResolveAllServerIPAddresses(radiusServerType_t type);

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
/*********************************************************************
*
* @purpose Open the UDP socket on specified local ipAddress.
*
* @param L7_IP_ADDRESS_TYPE_t ipAddr @b((input)) the Server ipAddress.
* @param L7_uint32 *interfaceSocket @b((input)) Pointer to the socket.
*
* @returns L7_SUCCESS on success.
* @returns L7_FAILURE on failure.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketGet(/*L7_IP_ADDRESS_TYPE_t*/L7_uint32 ipAddr,
                     L7_uint32 *interfaceSocket);

/*********************************************************************
*
* @purpose Open the UDP socket connection on specified local ipAddress.
*
* @param L7_IP_ADDRESS_TYPE_t ipAddr @b((input)) the Server ipAddress.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketOpen(/*L7_IP_ADDRESS_TYPE_t*/L7_uint32 ipAddr, 
                     L7_uint32 *interfaceSocket);

/*********************************************************************
*
* @purpose Close the UDP socket connection on specified local ipAddress.
*
* @param L7_IP_ADDRESS_TYPE_t ipAddr @b((input)) the Server ipAddress.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketClose(/*L7_IP_ADDRESS_TYPE_t*/L7_uint32 ipAddr);

/*********************************************************************
*
* @purpose Open the UDP socket on specified local ipAddress.
*
* @param L7_IP_ADDRESS_TYPE_t ipAddr @b((input)) the Server ipAddress.
* @param L7_uint32 *interfaceSocket @b((input)) Pointer to the socket.
*
* @returns L7_SUCCESS on success.
* @returns L7_FAILURE on failure.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketAddrGet(L7_uint32 interfaceSocket, 
                     /*L7_IP_ADDRESS_TYPE_t*/L7_uint32 *ipAddr);

/*********************************************************************
*
* @purpose Put the time stamp on the specified socket.
*
* @param L7_uint32 interfaceSocket @b((input)) socket bound to local IPAddr.
*
* @returns L7_SUCCESS on success.
* @returns L7_FAILURE on failure.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketTimeStampPut(L7_uint32 interfaceSocket);


#endif

/*********************************************************************
*
* @purpose Receive the data on specified socket.
*
* @param radiusServerEntry_t *server @b((input)) Pointer to radius server structure.
* @param nwInterface_t nwSocket@b((input)) structure contains socket info.
* @param L7_uint32 interfaceSocket @b((input)) socket bound to local IPAddr.
* @param radiusPacket_t *packet @b((input)) buffer to store received data.
*
* @returns L7_SUCCESS on success.
* @returns L7_FAILURE on failure.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusSocketReceive(radiusServerEntry_t *server, 
                              nwInterface_t nwSocket,
                              socketRxStatus_t *status,
                              radiusPacket_t *packet);

#endif /* INCLUDE_RADIUS_TXRX_H */

