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

#endif  /* _PTIN_PACKET_H */
