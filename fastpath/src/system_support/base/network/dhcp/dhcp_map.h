/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  dhcp_map.h
*
* @purpose   DHCP Mapping system infrastructure
*
* @component DHCP Client
*
* @comments  none
*
* @create    11-Jan-2010
*
* @author    Krishna Mohan CS
*
* @end
*
**********************************************************************/

#ifndef _DHCP_MAP_H_
#define _DHCP_MAP_H_

#include "l7_common.h"
#include "l3_commdefs.h"
#include "commdefs.h"
#include "async_event_api.h"
#include "l7_dhcp.h"

L7_RC_t
dhcpClientRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                      L7_uint32 event, 
                                      void *pData, 
                                      ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

L7_RC_t
dhcpClientMessageSend (dhcpClientQueueMsg_t *msg);

void
dhcpClientSocketFdsSet (L7_uint32 *maxFds,
                        fd_set *readFds);
#endif /*_DHCP_MAP_H_ */

