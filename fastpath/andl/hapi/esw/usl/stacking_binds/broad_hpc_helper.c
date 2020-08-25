/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
*
* @filename  broad_hpc_helper.c
*
* @purpose   Technology specific functions and data for the hardware
*            platform control component
*
* @component
*
* @create    02/04/2009
*
* @author    colinw
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "broad_hpc_helper.h"

#include "bcm/error.h"

/*********************************************************************
* @purpose  Routine for application helpers to send packets on the local
*           unit.
*          
* @param    frame              @b{(input)} 
* @param    frameSize          @b{(input)} 
* @param    priority           @b{(input)} 
* @param    slot               @b{(input)} 
* @param    port               @b{(input)} 
* @param    ignoreEgressRules  @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareHelperSend(L7_uchar8   *frame,
			                  L7_uint32    frameSize,
			                  L7_uint8     priority,
			                  L7_uint8     slot,
			                  L7_ushort16  port,
			                  L7_BOOL      ignoreEgressRules)
{
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Allocate memory for the CPU send stats for HPC helper.
*          Zero out the stat values.
*
* @returns 
*
* @notes   none
*
* @end
*
*********************************************************************/
void hpcHardwareHelperStatsInit()
{
  return;
}

/*********************************************************************
*
* @purpose Get a stat from the HPC helper
*
* @param   
*
* @returns L7_RC_t result
*
* @notes   This function is entered only by one task at a time.
*
*          The cache is optimized for use by RMON, which asks for
*          multiple counters for the same interface before proceeding
*          to the next interface.
*
* @end
*
*********************************************************************/
int hpcHardwareHelperStatDeltaGet(HPC_HELPER_STATS_t  helperStatId,
                                  L7_ulong64         *helperStatValue)
{
  return BCM_E_UNAVAIL;
}
