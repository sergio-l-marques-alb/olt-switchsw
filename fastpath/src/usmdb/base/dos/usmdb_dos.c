/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  usmdb_dos.c
*
* @purpose   Provide interface to API's for Denial of Service UI components
*
* @component DOSCONTROL
*
* @comments  This file provides usmdb API for Denial of Service, which will
*            be called by User Interface.
*
* @create    03/28/2005
*
* @author    esmiley
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "dos_api.h"
#include "usmdb_dos_api.h"

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
L7_RC_t usmdbDoSSIPDIPSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSSIPDIPSet(mode);
}

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
L7_RC_t usmdbDoSSMACDMACSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSSMACDMACSet(mode);
}

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
L7_RC_t usmdbDoSFirstFragModeSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSFirstFragModeSet(mode);
}

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
L7_RC_t usmdbDoSFirstFragSizeSet(L7_uint32 unit, L7_uint32 minTCPHdrSize)
{
    return doSFirstFragSizeSet(minTCPHdrSize);
}

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
L7_RC_t usmdbDoSTCPFragSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSTCPFragSet(mode);
}

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
L7_RC_t usmdbDoSTCPOffsetSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSTCPOffsetSet(mode);
}

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
L7_RC_t usmdbDoSTCPFlagSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSTCPFlagSet(mode);
}

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
L7_RC_t usmdbDoSTCPFlagSeqSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSTCPFlagSeqSet(mode);
}

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
L7_RC_t usmdbDoSTCPPortSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSTCPPortSet(mode);
}

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
L7_RC_t usmdbDoSUDPPortSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSUDPPortSet(mode);
}

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
L7_RC_t usmdbDoSTCPSynSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSTCPSynSet(mode);
}

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
L7_RC_t usmdbDoSTCPSynFinSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSTCPSynFinSet(mode);
}

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
L7_RC_t usmdbDoSTCPFinUrgPshSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSTCPFinUrgPshSet(mode);
}

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
L7_RC_t usmdbDoSL4PortSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSL4PortSet(mode);
}

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
L7_RC_t usmdbDoSICMPModeSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSICMPModeSet(mode);
}

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
L7_RC_t usmdbDoSICMPSizeSet(L7_uint32 unit, L7_uint32 maxICMPSize)
{
    return doSICMPSizeSet(maxICMPSize);
}

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
L7_RC_t usmdbDoSICMPv6SizeSet(L7_uint32 unit, L7_uint32 maxICMPSize)
{
    return doSICMPv6SizeSet(maxICMPSize);
}

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
L7_RC_t usmdbDoSICMPFragModeSet(L7_uint32 unit, L7_uint32 mode)
{
    return doSICMPFragModeSet(mode);
}

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
L7_RC_t usmdbDoSSIPDIPModeGet(L7_uint32 *mode)
{
    doSSIPDIPModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSSMACDMACModeGet(L7_uint32 *mode)
{
    doSSMACDMACModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSFirstFragModeGet(L7_uint32 *mode)
{
    doSFirstFragModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSTCPPartialModeGet(L7_uint32 *mode)
{
    doSFirstFragModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSFirstFragSizeGet(L7_int32 *minTCPHdrSize)
{
    doSFirstFragSizeGet(minTCPHdrSize);
    return L7_SUCCESS; 
}

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
L7_RC_t usmdbDoSTCPOffsetModeGet(L7_uint32 *mode)
{
    doSTCPOffsetModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSTCPPortModeGet(L7_uint32 *mode)
{
    doSTCPPortModeGet(mode);
    return L7_SUCCESS;
}
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
L7_RC_t usmdbDoSUDPPortModeGet(L7_uint32 *mode)
{
    doSUDPPortModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSTCPFragModeGet(L7_uint32 *mode)
{
    doSTCPFragModeGet(mode);
    return L7_SUCCESS; 
}

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
L7_RC_t usmdbDoSTCPFlagModeGet(L7_uint32 *mode)
{
    doSTCPFlagModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSTCPFlagSeqModeGet(L7_uint32 *mode)
{
    doSTCPFlagSeqModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSTCPSynModeGet(L7_uint32 *mode)
{
    doSTCPSynModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSTCPSynFinModeGet(L7_uint32 *mode)
{
    doSTCPSynFinModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSTCPFinUrgPshModeGet(L7_uint32 *mode)
{
    doSTCPFinUrgPshModeGet(mode);
    return L7_SUCCESS;
}

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
L7_RC_t usmdbDoSL4PortModeGet(L7_uint32 *mode)
{
    doSL4PortModeGet(mode);
    return L7_SUCCESS; 
}

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
L7_RC_t usmdbDoSICMPModeGet(L7_uint32 *mode)
{
    doSICMPModeGet(mode);
    return L7_SUCCESS; 
}

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
L7_RC_t usmdbDoSICMPSizeGet(L7_int32 *maxICMPSize)
{
    doSICMPSizeGet(maxICMPSize);
    return L7_SUCCESS; 
}

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
L7_RC_t usmdbDosIntfPingFloodingSet(L7_uint32 interface, L7_uint32 mode, L7_uint32 param)
{
  return dosIntfPingFloodingSet(interface, mode, param);
}

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
L7_RC_t usmdbDosIntfPingFloodingGet(L7_uint32 interface, L7_uint32 *mode , L7_uint32 *param)
{
  return dosIntfPingFloodingGet(interface, mode, param);
}

/*********************************************************************
* @purpose Enables or disables the mode for preventing Smurf DOS Attack
*
* @param   L7_uint32 interface (input) Interface on which mode needs to
*                                      be enabled or disabled
* @param   L7_uint32 mode      (input) Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDosIntfSmurfAttackSet(L7_uint32 interface, L7_uint32 mode)
{
  return dosIntfSmurfAttackSet(interface, mode);
} 

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
L7_RC_t usmdbDosIntfSmurfAttackGet(L7_uint32 interface, L7_uint32 *mode)
{
  return dosIntfSmurfAttackGet(interface, mode);
}

#if 0
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
L7_RC_t usmdbDosIntfSynAckFloodingSet(L7_uint32 interface, L7_uint32 mode , L7_uint32 param)
{
  return doIntfSynAckFloodingSet(interface, mode, param);
}
#endif

/*********************************************************************
* @purpose  Gets the SynAck Flooding mode for an interface
*
* @param   L7_uint32 interface (input)  Interface on which mode needs to
*                                       be fetched
* @param   L7_uint32 *mode     (output) Mode value
* @param   L7_uint32 *param    (output) param value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDosIntfSynAckFloodingGet(L7_uint32 interface, L7_uint32 *mode ,L7_uint32 *param)
{
  return dosIntfSynAckFloodingGet(interface, mode, param);
}


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
L7_RC_t usmdbDosGlobalPingFloodingSet(L7_uint32 mode, L7_uint32 param)
{
  return dosGlobalPingFloodingSet(mode, param);
}

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
L7_RC_t usmdbDosGlobalPingFloodingGet(L7_uint32 *mode , L7_uint32 *param)
{
  return dosGlobalPingFloodingGet(mode, param);
}

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
L7_RC_t usmdbDosGlobalSmurfAttackSet(L7_uint32 mode)
{
  return dosGlobalSmurfAttackSet(mode);
} 

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
L7_RC_t usmdbDosGlobalSmurfAttackGet(L7_uint32 *mode)
{
  return dosGlobalSmurfAttackGet(mode);
}
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
L7_RC_t usmdbDosGlobalSynAckFloodingSet(L7_uint32 mode , L7_uint32 param)
{
  return dosGlobalSynAckFloodingSet(mode, param);
}
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
L7_RC_t usmdbDosGlobalSynAckFloodingGet(L7_uint32 *mode ,L7_uint32 *param)
{
  return dosGlobalSynAckFloodingGet(mode, param);
}

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
L7_RC_t usmdbDoSICMPv4SizeGet(L7_int32 *maxICMPSize)
{
    doSICMPv4SizeGet(maxICMPSize);
    return L7_SUCCESS; 
}

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
L7_RC_t usmdbDoSICMPv6SizeGet(L7_int32 *maxICMPSize)
{
    doSICMPv6SizeGet(maxICMPSize);
    return L7_SUCCESS; 
}

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
L7_RC_t usmdbDoSICMPFragModeGet(L7_uint32 *mode)
{
    doSICMPFragModeGet(mode);
    return L7_SUCCESS; 
}



