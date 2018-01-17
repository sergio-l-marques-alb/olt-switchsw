/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  usmdb_dos_api.h
*
* @purpose   Provide interface to API's for Denial of Service components
*
* @component DOSCONTROL
*
* @comments  Header file to be included by UI layer for using the 
*            Denial of Service usmdb function.
*
* @create    03/28/2005
*
* @author    esmiley
*
* @end
*
**********************************************************************/
 
#ifndef USMDB_DENIALOFSERVICE_API_H
#define USMDB_DENIALOFSERVICE_API_H

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
extern L7_RC_t usmdbDoSSIPDIPSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSSMACDMACSet(L7_uint32 unit, L7_uint32 mode);  

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
extern L7_RC_t usmdbDoSFirstFragModeSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSFirstFragSizeSet(L7_uint32 unit, L7_uint32 minTCPHdrSize);
                       
/*********************************************************************
* @purpose  Sets DoS TCP Partial mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmdbDoSTCPPartialModeSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSTCPFragSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSTCPOffsetSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSTCPFlagSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSTCPFlagSeqSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSTCPPortSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSUDPPortSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSTCPSynSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSTCPSynFinSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSTCPFinUrgPshSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSL4PortSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSICMPModeSet(L7_uint32 unit, L7_uint32 mode);

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
extern L7_RC_t usmdbDoSICMPFragModeSet(L7_uint32 unit, L7_uint32 mode);
                               
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
extern L7_RC_t usmdbDoSICMPSizeSet(L7_uint32 unit, L7_uint32 maxICMPSize);

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
extern L7_RC_t usmdbDoSICMPv6SizeSet(L7_uint32 unit, L7_uint32 maxICMPSize);


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
extern L7_RC_t usmdbDoSSIPDIPModeGet(L7_uint32 *mode); 

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
extern L7_RC_t usmdbDoSSMACDMACModeGet(L7_uint32 *mode);  

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
extern L7_RC_t usmdbDoSFirstFragModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS TCP Partial mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmdbDoSTCPPartialModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSFirstFragSizeGet(L7_int32 *minTCPHdrSize);

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
extern L7_RC_t usmdbDoSTCPFragModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSTCPOffsetModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSTCPPortModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS UDP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmdbDoSUDPPortModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSTCPFlagModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSTCPFlagSeqModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSTCPSynModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSTCPSynFinModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSTCPFinUrgPshModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSL4PortModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSICMPModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets DoS ICMPv4 mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmdbDoSICMPModeGet(L7_uint32 *mode);

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
extern L7_RC_t usmdbDoSICMPSizeGet(L7_int32 *maxICMPSize);

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
extern L7_RC_t usmdbDoSICMPv4SizeGet(L7_int32 *maxICMPSize);

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
extern L7_RC_t usmdbDoSICMPv6SizeGet(L7_int32 *maxICMPSize);

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
extern L7_RC_t usmdbDoSICMPFragModeGet(L7_uint32 *mode);
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
extern L7_RC_t usmdbDosGlobalPingFloodingSet(L7_uint32 mode, L7_uint32 param);

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
extern L7_RC_t usmdbDosGlobalPingFloodingGet(L7_uint32 *mode , L7_uint32 *param);
/*********************************************************************
* @purpose Enables or disables the mode for preventing Smurf DOS Attack
*
* @param   L7_uint32 mode      (input) Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmdbDosGlobalSmurfAttackSet(L7_uint32 mode);

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
extern L7_RC_t usmdbDosGlobalSmurfAttackGet(L7_uint32 *mode);
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
extern L7_RC_t usmdbDosGlobalSynAckFloodingSet(L7_uint32 mode , L7_uint32 param);
/*********************************************************************
* @purpose  Gets the SynAck Flooding mode for an interface
*
* @param   L7_uint32 *mode     (output) Mode value
* @param   L7_uint32 *param    (output) param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmdbDosGlobalSynAckFloodingGet(L7_uint32 *mode ,L7_uint32 *param);


#endif
