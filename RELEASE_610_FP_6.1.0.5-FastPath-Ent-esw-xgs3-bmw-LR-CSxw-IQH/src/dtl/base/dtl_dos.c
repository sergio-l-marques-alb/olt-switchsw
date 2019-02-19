/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dtl_dos.c
*
* @purpose   DTL interface
*
* @component DTL
*
* @comments  none
*
* @create    03/30/2005
*
* @author    esmiley
*
* @end
*             
**********************************************************************/

#include "dtlinclude.h"


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
L7_RC_t dtlDoSSIPDIPSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_SIPDIP;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }

    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSSMACDMACSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_SMACDMAC;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }

    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

}

/*********************************************************************
* @purpose  Sets the DoS First Fragment mode and minTCPHdrSize
*
* @param    mode
* @param    minTCPHdrSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t dtlDoSFirstFragSet(L7_uint32 mode, L7_int32 minTCPHdrSize)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0; 

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_FIRSTFRAG;
    dapiCmd.cmdData.dosControlConfig.arg = minTCPHdrSize;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }


    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSTCPFragSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_TCPFRAG;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSTCPOffsetSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_TCPOFFSET;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSTCPFlagSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_TCPFLAG;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSTCPFlagSeqSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_TCPFLAGSEQ;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSTCPPortSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_TCPPORT;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSUDPPortSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_UDPPORT;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSTCPSynSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_TCPSYN;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSTCPSynFinSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_TCPSYNFIN;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSTCPFinUrgPshSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_TCPFINURGPSH;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }



    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSL4PortSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_L4PORT;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }

    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

}

/*********************************************************************
* @purpose  Sets DoS ICMP Mode and maxICMPSize
*
* @param    mode
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t dtlDoSICMPSet(L7_uint32 mode, L7_int32 maxICMPSize)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_ICMP;
    dapiCmd.cmdData.dosControlConfig.arg = maxICMPSize;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }

    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

}

/*********************************************************************
* @purpose  Sets DoS ICMPv6 Mode and maxICMPSize
*
* @param    mode
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t dtlDoSICMPv6Set(L7_uint32 mode, L7_int32 maxICMPSize)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_ICMPV6;
    dapiCmd.cmdData.dosControlConfig.arg = maxICMPSize;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }

    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

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
L7_RC_t dtlDoSICMPFragSet(L7_uint32 mode)
{
    DAPI_USP_t ddUsp;
    DAPI_SYSTEM_CMD_t dapiCmd;


    ddUsp.unit = 0;
    ddUsp.slot = 0;
    ddUsp.port = 0;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType = DOSCONTROL_ICMPFRAG;
    dapiCmd.cmdData.dosControlConfig.arg = 0;
    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
        dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }

    return dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG, &dapiCmd);

}
/*********************************************************************
* @purpose  Set mode for Ping flooding Dos Attack
*
* @param    interface
* @param    mode
* @param    param
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDosIntfPingFlooding(L7_uint32 interface,L7_uint32 mode, L7_uint32 param)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(interface, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType  = DOSCONTROL_PINGFLOODING;
    dapiCmd.cmdData.dosControlConfig.arg = param;

    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
      dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }
    return dapiCtl(&ddUsp, DAPI_CMD_INTF_DOSCONTROL_CONFIG, &dapiCmd);
  }
}

/*********************************************************************
* @purpose  Set mode for Smurf Dos Attack
*
* @param    interface
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDosIntfSmurfAttack(L7_uint32 interface,L7_uint32 mode)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(interface, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType  = DOSCONTROL_SMURFATTACK;
    dapiCmd.cmdData.dosControlConfig.arg = 0;

    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
      dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }
    return dapiCtl(&ddUsp, DAPI_CMD_INTF_DOSCONTROL_CONFIG, &dapiCmd);
  }

}

/*********************************************************************
* @purpose  Set mode for SYN ACK Flooding Dos Attack
*
* @param    interface
* @param    mode
* @param    param
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDosIntfSynAckFlooding(L7_uint32 interface,L7_uint32 mode, L7_uint32 param)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(interface, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.dosControlConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.dosControlConfig.dosType  = DOSCONTROL_SYNACKFLOODING;
    dapiCmd.cmdData.dosControlConfig.arg = param;

    if (mode == L7_ENABLE) {
        dapiCmd.cmdData.dosControlConfig.enable = L7_TRUE;
    }
    else
    {
      dapiCmd.cmdData.dosControlConfig.enable = L7_FALSE;
    }
    return dapiCtl(&ddUsp, DAPI_CMD_INTF_DOSCONTROL_CONFIG, &dapiCmd);
  }
}





