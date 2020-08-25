/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename dos.h
*
* @purpose     Header file to be included by dos.c
*
* @component  DOSCONTROL
*
* @comments   This file contains all the defines, function prototypes
*             and include files required by dos.c
*
* @create     03/28/2005
*
* @author     esmiley
* @end
*
**********************************************************************/

#ifndef DOS_API_H
#define DOS_API_H

#include "l7_common.h"



/*********************************************************************
* @purpose  Sets DoS SIP=DIP mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSSIPDIPSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS SMAC=DMAC mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSSMACDMACSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the DoS First Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSFirstFragModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the DoS Minimum TCP Header Size
*
* @param    minTCPHdrSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSFirstFragSizeSet(L7_uint32 minTCPHdrSize);

/*********************************************************************
* @purpose  Sets the DoS Minimum TCP Header Size
*
* @param    minTCPHdrSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPPartialSizeSet(L7_uint32 minTCPHdrSize);

/*********************************************************************
* @purpose  Sets the DoS TCP Partial mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPPartialModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPFragSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Offset mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPOffsetSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Flag mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPFlagSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Flag & Sequence mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPFlagSeqSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPPortSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS UDP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSUDPPortSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP SYN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPSynSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP SYN&FIN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPSynFinSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP FIN&URG&PSH mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPFinUrgPshSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS L4 Port Mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSL4PortSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS ICMP Mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets Maximum ICMP Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPSizeSet(L7_uint32 maxICMPSize);

/*********************************************************************
* @purpose  Sets Maximum ICMPv6 Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPv6SizeSet(L7_uint32 maxICMPSize);

/*********************************************************************
* @purpose  Sets DoS ICMP Fragment Mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPFragModeSet(L7_uint32 mode);


/*********************************************************************
* @purpose  Gets DoS SIP=DIP mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSSIPDIPModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS SMAC=DMAC mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSSMACDMACModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS First Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSFirstFragModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS Minimum TCP Header Size
*
* @param    minTCPHdrSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSFirstFragSizeGet(L7_uint32 *minTCPHdrSize);

/*********************************************************************
* @purpose  Gets DoS TCP Offset mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPOffsetModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS TCP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPPortModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS USP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSUDPPortModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS TCP Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPFragModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS TCP Flag mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPFlagModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS TCP Flag & Sequence mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPFlagSeqModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS TCP SYN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPSynModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS TCP SYN&FIN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPSynFinModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS TCP FIN&URG&PSH mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSTCPFinUrgPshModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS L4 Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSL4PortModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS ICMP mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS Maximum ICMP Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPSizeGet(L7_uint32 *maxICMPSize);

/*********************************************************************
* @purpose  Gets DoS Maximum ICMPv4 Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPv4SizeGet(L7_uint32 *maxICMPSize);

/*********************************************************************
* @purpose  Gets DoS Maximum ICMPv6 Size
*
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPv6SizeGet(L7_uint32 *maxICMPSize);

/*********************************************************************
* @purpose  Gets DoS ICMP Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t doSICMPFragModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Enables or disables the mode for preventing PING
*           Flooding DOS Attack
*
* @param   L7_uint32 interface (input)   Interface on which mode needs to
*                                        be enabled or disabled
* @param   L7_uint32 mode      (input)   Mode value
* @param   L7_uint32 param     (input)   Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfPingFloodingSet(L7_uint32 interface, L7_uint32 mode,L7_uint32 param);

/*********************************************************************
* @purpose  Gets the PING Flooding mode for an interface
*
* @param   L7_uint32 interface (input)   Interface on which mode needs to
*                                        be fetched
* @param   L7_uint32 *mode     (output)  Mode value
* @param   L7_uint32 *param    (output)  param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfPingFloodingGet(L7_uint32 interface, L7_uint32 *mode ,L7_uint32 *param);

/*********************************************************************
* @purpose Enables or disables the mode for preventing Smurf DOS Attack
*
* @param   L7_uint32 interface (input) Interface on which mode needs to
*                                      be enabled or disabled
* @param   L7_uint32 mode      (input) Mode value*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfSmurfAttackSet(L7_uint32 interface, L7_uint32 mode);

/*********************************************************************
* @purpose  Gets the Smurf Attack mode for an interface
*
* @param   L7_uint32 interface (input)  Interface on which mode needs to
*                                       be fetched
* @param   L7_uint32 *mode     (output) Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfSmurfAttackGet(L7_uint32 interface, L7_uint32 *mode);

/*********************************************************************
* @purpose  Enables or disables the mode for preventing SynAck
*           Flooding DOS Attack
*
* @param   L7_uint32 interface (input) Interface on which mode needs to
*                                      enabled or disabled
* @param   L7_uint32 mode      (input) Mode value
* @param   L7_uint32 param     (input) Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfSynAckFloodingSet(L7_uint32 interface, L7_uint32 mode , L7_uint32 param);

/*********************************************************************
* @purpose  Gets the SynAck Flooding mode for an interface
*
* @param   L7_uint32 interface (input)  Interface on which mode needs to
*                                       be fetched
* @param   L7_uint32 *mode     (output) Mode value
* @param   L7_uint32 *param    (output)  param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosIntfSynAckFloodingGet(L7_uint32 interface, L7_uint32 *mode, L7_uint32 *param);

/*********************************************************************
* @purpose  Determine if the interface type is valid for configuration of DOS ATTACK
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dosIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid for configuration of DOS ATTACK
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dosIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Enables or disables the mode for preventing PING
*           Flooding DOS Attack
*
* @param   L7_uint32 mode      (input)   Mode value
* @param   L7_uint32 param     (input)   Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalPingFloodingSet(L7_uint32 mode,L7_uint32 param);

/*********************************************************************
* @purpose  Gets the PING Flooding mode for an interface
*
* @param   L7_uint32 *mode     (output)  Mode value
* @param   L7_uint32 *param    (output)  param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalPingFloodingGet(L7_uint32 *mode ,L7_uint32 *param);

/*********************************************************************
* @purpose Enables or disables the mode for preventing Smurf DOS Attack
*
* @param   L7_uint32 mode      (input) Mode value*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalSmurfAttackSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Gets the Smurf Attack mode for an interface
*
* @param   L7_uint32 *mode     (output) Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalSmurfAttackGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Enables or disables the mode for preventing SynAck
*           Flooding DOS Attack
*
* @param   L7_uint32 mode      (input) Mode value
* @param   L7_uint32 param     (input) Param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalSynAckFloodingSet(L7_uint32 mode , L7_uint32 param);

/*********************************************************************
* @purpose  Gets the SynAck Flooding mode for an interface
*
* @param   L7_uint32 *mode     (output) Mode value
* @param   L7_uint32 *param    (output)  param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dosGlobalSynAckFloodingGet(L7_uint32 *mode, L7_uint32 *param);

/* DEBUG */

void dosDumpIntfCfg(int unit,int slot, int port);
void dosDebugIntfModeSet(int unit,int slot, int port,int type, int mode,int param);

#endif
