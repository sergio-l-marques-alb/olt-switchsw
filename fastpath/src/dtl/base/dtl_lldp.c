/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename dtl_lldp.c
*
* @purpose DTL interface
*
* @component DTL
*
* @comments none
*
* @create 02/21/2005
*
* @author dfowler
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "log.h"
#include "dtlinclude.h"

/*********************************************************************
* @purpose  Enable/Disable LLDP for an interface
*
* @param    mode  @b{(input)} lldp mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlLldpModeSet(L7_uint32 intIfNum,
                       L7_uint32 mode)
{
  DAPI_INTF_MGMT_CMD_t cmd;
  nimUSP_t             usp;
  DAPI_USP_t           ddUsp;
  L7_RC_t              rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.lldpConfig.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.lldpConfig.enable = (mode == L7_ENABLE) ? L7_TRUE : L7_FALSE;

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_LLDP_CONFIG, &cmd);
  }

  return rc;
}


