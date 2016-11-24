/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   spm.c
*
* @purpose    Component code for Stack Port Manager
*
* @component  spm
*
* @comments
*
* @create     07/26/2004
*
* @author     Andrey Tsigler
* @end
*
**********************************************************************/
#include "spm_api.h"
#include "usmdb_spm_api.h"

/*********************************************************************
* @purpose  Determine whether specified USP is used for stacking.
*
* @param    unit - unit of the front-panel stack port.
* @param    slot - slot of the front-panel stack port.
* @param    port - port of the front-panel stack port.
*
* @returns  L7_TRUE - Stacking port.
*           L7_FALSE - Not Stacking Port.
*
* @notes   
*
* @end
*********************************************************************/
L7_BOOL spmFpsPortStackingModeCheck (L7_uint32 unit,
                                   L7_uint32 slot,
                                   L7_uint32 port)
{
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Find front-panel port that matches the specified USP.
*
* @param    unit - unit of the front-panel stack port.
* @param    slot - slot of the front-panel stack port.
* @param    port - port of the front-panel stack port.
* @param    index - Output: Index of the matching port.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t spmFpsIndexFromUspGet (L7_uint32 unit,
                               L7_uint32 slot,
                               L7_uint32 port,
                               L7_uint32 * index)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get QOS configuration for all front-panel stacking ports.
*           stack port.
*
* @param    enable - 1 - Enable QOS Mode
*                    0 - Disable QOS mode.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t spmFpsConfigQosModeGet (L7_uint32 * mode)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set QOS configuration for all front-panel stacking ports.
*           stack port.
*
* @param    enable - 1 - Enable QOS Mode
*                    0 - Disable QOS mode.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t spmFpsConfigQosModeSet (L7_uint32 mode)
{
  return L7_FAILURE;
}
 
/*********************************************************************
* @purpose  Set stacking mode configuration for a front-panel 
*           stack port.
*
* @param    index - SNMP index for the stack port.
* @param    enable - 1 - Enable Stacking Mode
*                    0 - Disable Stacking mode.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t spmFpsConfigStackingModeSet (L7_uint32 index,
                                    L7_uint32 mode)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get information about a stacking port given an index.
*
* @param    index - SNMP index for the stacking port.
* @param    port_entry - Output parameter representing port info.
*                       Caller must allocate space for this data.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t spmStackingCapablePortGet (L7_uint32 index,
                                    SPM_STACK_PORT_ENTRY_t *port_entry)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get information about a stacking port given an index.
*
* @param    port_entry - Input - Current port info. Should be set to 
*                               all zeroes in order to get the first port.
*                       Output - Next port info.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - No more entries in the table.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t spmStackingCapablePortNextGet (
                                    SPM_STACK_PORT_ENTRY_t *port_entry)
{
 return L7_FAILURE;
}
