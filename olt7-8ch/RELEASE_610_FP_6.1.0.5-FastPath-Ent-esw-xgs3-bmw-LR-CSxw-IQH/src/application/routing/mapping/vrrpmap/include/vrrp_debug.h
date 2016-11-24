/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 * @filename  vrrp_debug.h
 *
 * @purpose   vrrp Debug functions
 *
 * @component vrrp
 *
 * @comments  none
 *
 * @create
 *
 * @author    P.Sowjanya
 *
 * @end
 *
 **********************************************************************/
/*************************************************************

 *************************************************************/
#ifndef VRRP_DEBUG_H
#define VRRP_DEBUG_H

#include "l7_common.h"
#include "log.h"
#include "vrrp_config.h"

extern L7_BOOL vrrpPacketDebugFlag;

#define VRRP_USER_TRACE(__fmt__, __args__... )                              \
  if (vrrpDebugPacketTraceFlag == L7_TRUE)                          \
{                                                                 \
  LOG_USER_TRACE(L7_VRRP_MAP_COMPONENT_ID, __fmt__,##__args__);        \
}


/*********************************************************************
 *
 * @purpose Trace vrrp packets received
 *
 * @param   intIfNum     @b{(input)} Internal Interface Number
 * @param   *pdu        @b{(input)} pointer to packet
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void vrrpDebugPacketRxTrace(L7_uint32 intIfNum, L7_uchar8* pdu);
/*********************************************************************
 *
 * @purpose Trace vrrp packets transmitted
 *
 * @param   intIfNum     @b{(input)} Internal Interface Number
 * @param   *pdu         @b{(input)} pointer to packet
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void vrrpDebugPacketTxTrace(L7_uint32 intIfNum,L7_uchar8 *pdu);

#endif /*VRRP_DEBUG_H */
