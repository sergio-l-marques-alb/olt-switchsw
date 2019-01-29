/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
* @filename  dtl_llpf.c
*
* @purpose   DTL interface
*
* @component DTL
*
* @comments  None 
*
* @create   10/27/2009 
*
* @author   vijayanand K(kvijayan) 
*
* @end
*             
**********************************************************************/

#include "dtlinclude.h"
#include "dtlapi.h"

/*********************************************************************
* @purpose  Set LLPF block mode for a pirticular protocol
*
* @param    interface        @b{(input)} interface type
* @param    blockType        @b{(input)} LLPF block type
* @param    mode             @b{(input)} mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlLlpfIntfBlockConfig(L7_uint32 interface,L7_LLPF_BLOCK_TYPE_t blockType,
                               L7_BOOL mode)
{
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
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

    dapiCmd.cmdData.llpfConfig.getOrSet  = DAPI_CMD_SET;
    dapiCmd.cmdData.llpfConfig.blockType = blockType;
    dapiCmd.cmdData.llpfConfig.enable    = mode;
    return dapiCtl(&ddUsp, DAPI_CMD_INTF_LLPF_CONFIG, &dapiCmd);
  }
}

