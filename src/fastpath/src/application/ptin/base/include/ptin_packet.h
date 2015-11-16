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

extern L7_BOOL ptin_packet_debug_enable;

/**
 * Initialize ptin_packet module
 * 
 * @author mruas (5/23/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_packet_init(void);

/**
 * DEInitialize ptin_packet module
 * 
 * @author mruas (5/23/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_packet_deinit(void);

/**
 * Callback to be called for bradcast packets. Packets will be 
 * flood for a specific list of vlans.
 * 
 * @param bufHandle 
 * @param pduInfo 
 * 
 * @return L7_RC_t : L7_FAILURE (always)
 */
extern L7_RC_t ptinMacBcastRecv(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

/**
 * Routine to transmit frames
 * 
 * @param intIfNum 
 * @param vlanId 
 * @param innerVID 
 * @param payload 
 * @param payloadLen 
 */
extern void ptin_packet_send(L7_uint32 intIfNum,
                      L7_uint32 vlanId,
                      L7_uint32 innerVID,
                      L7_uchar8 *payload,
                      L7_uint32 payloadLen);


/*********************************************************************
* @purpose  Forward a packet (L2 switching).
*
* @param    intIfNum    @b{(input)} receive interface
* @param    vlanId      @b{(input)} VLAN ID
* @param    innerVlanId @b{(input)} Inner VLAN ID
* @param    frame       @b{(input)} ethernet frame
* @param    frameLen    @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t ptin_packet_frame_l2forward(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                                           L7_uchar8 *frame, L7_ushort16 frameLen);

extern L7_RC_t ptin_packet_frame_l2forward_nonblocking(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                                                       L7_uchar8 *frame, L7_ushort16 frameLen);

#endif  /* _PTIN_PACKET_H */
