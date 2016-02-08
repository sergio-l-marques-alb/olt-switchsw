#ifndef SPM_API_H
#define SPM_API_H
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   cda_api.h
*
* @purpose    Component APIs for the Stack Port Manager
*
* @component  spm
*
* @comments   
*
* @create     7/26/2004
*
* @author     Andrey Tsigler
* @end
*
**********************************************************************/
#include "l7_common.h"
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
                                   L7_uint32 port);

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
                               L7_uint32 * index);

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
L7_RC_t spmFpsConfigQosModeGet (L7_uint32 * mode);

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
L7_RC_t spmFpsConfigQosModeSet (L7_uint32 mode);


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
                                    L7_uint32 mode);

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
                                    SPM_STACK_PORT_ENTRY_t *port_entry);

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
                                    SPM_STACK_PORT_ENTRY_t *port_entry);


/*********************************************************************
* @purpose  Report link UP event for a stacking port.
*
* @param    hpc_index - Stack port index.
*
* @returns  none
*
* @notes    This routine is called for local stack port
*           on the local unit.
*
* @end
*********************************************************************/
void spmStackPortLinkUp (L7_uint32 hpc_index);

 /*********************************************************************
* @purpose  Report link DOWN event for a stacking port.
*
* @param    hpc_index - Stack port index.
*
* @returns  none
*
* @notes    This routine is called for local stack port
*           on the local unit.
*
* @end
*********************************************************************/
void spmStackPortLinkDown (L7_uint32 hpc_index);

/*********************************************************************
* @purpose  The Stack Port Manager Initialization Function.
*
* @param    none
*
* @returns  none
*
* @notes   The Stack Port Manager should be initialized
*          as part of the sysapi initialization 
*
* @end
*********************************************************************/
void spmInit (void);

/*********************************************************************
* @purpose  Retrieve the default port mode configuration.
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 spmSidStackPortModeGet(void);

/*********************************************************************
* @purpose  Send unsolicited status report to the management unit.
*
* @param    none
*
* @returns  none
*
* @notes    Report for all the local stack ports is sent.
*
* @end
*********************************************************************/
void spmUnsolicitedStatusSend (void);

/*********************************************************************
* @purpose  Request status of remote unit stack ports.
*
* @param    unit: Unit number of the remote unit
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
void spmStackPortReqStatus(L7_uint32 unit);

#endif                          
