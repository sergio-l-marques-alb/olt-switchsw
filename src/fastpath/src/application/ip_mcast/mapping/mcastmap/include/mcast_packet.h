/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mcast_packet.h
*
* @purpose    MCAST Mapping layer internal function prototypes
*
* @component  MCAST Mapping Layer
*
* @comments   none
*
* @create     01/25/2006
*
* @author     gkiran
*
* @end
**********************************************************************/

#ifndef _MCAST_PACKET_H_
#define _MCAST_PACKET_H_

#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "sysnet_api.h"
#include "mcast_inet.h"

/********************************************************************
*                     Function Prototypes
*********************************************************************/

extern SYSNET_PDU_RC_t mcastMapIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc);

extern L7_RC_t mcastMapV6PacketSend(mcastSendPkt_t *pSendPkt);

L7_RC_t mcastMapV6Register(L7_uint32 routerProtocol_ID, L7_char8 *name, 
                           L7_RC_t (*notify)(L7_uint32 intIfNum, 
                           L7_uint32 event, void *pData, 
                           ASYNC_EVENT_NOTIFY_INFO_t *response));
L7_RC_t mcastMapV6Deregister(L7_uint32 routerProtocol_ID);
#endif /* _MCAST_PACKET_H_ */
