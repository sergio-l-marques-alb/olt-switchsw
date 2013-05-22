/**
 * ptin_packet.h 
 *  
 * Implements the Packet capture and process routines
 *
 * Created on: 2013/05/22 
 * Author: Milton Ruas (milton-r-silva@ext.ptinovacao.pt)
 * Notes: 
 */

#ifndef _PTIN_PACKET_H
#define _PTIN_PACKET_H

#include "ptin_include.h"
#include "sysnet_api.h"

/*********************************************************************
* @purpose  Intercept incoming ARP packets.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if packet to be forwarded
*           SYSNET_PDU_RC_IGNORED   if packet should continue normal processing
*           SYSNET_PDU_RC_DISCARD   if packet falls through the cracks
*           SYSNET_PDU_RC_COPIED    if broadcast destination MAC
*
* @notes    This function is executed on the DTL thread.
*
* @end
*********************************************************************/
extern SYSNET_PDU_RC_t ptinArpRecv(L7_uint32 hookId,
                                   L7_netBufHandle bufHandle,
                                   sysnet_pdu_info_t *pduInfo,
                                   L7_FUNCPTR_t continueFunc);

#endif  /* _PTIN_PACKET_H */
