/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 *
 * @filename    arp_debug_api.h
 * @purpose     arp debug functions
 * @component   ARP
 * @comments    none
 * @create      08/02/2006
 * @author      sowjanya
 * @end
 *
 **********************************************************************/
#ifndef ARP_DEBUG_API_H
#define ARP_DEBUG_API_H

#include "l7_common.h"

/*********************************************************************
 * @purpose  Get the current status of displaying arp packet debug info
 *
 * @param    none
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_BOOL arpDebugPacketTraceFlagGet();


/*********************************************************************
 * @purpose  Turns on/off the displaying of arp packet debug info
 *
 * @param    flag         new value of the Packet Debug flag
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t arpDebugPacketTraceFlagSet(L7_BOOL flag);

#endif /*ARP_DEBUG_API_H*/
