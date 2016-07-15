/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename  cpu_init.c
*
* @purpose   Provide cpu specific initialization functions for the
*            BCM98548XMC (GTO) CPU.
*
* @component BSP
*
* @create    03/21/2008
*
* @author    cmutter
*
* @end
*
*********************************************************************/
#include <stdio.h>
#include "bspapi.h"
#include "bspcpu_api.h"

/* Global variables used by other code */
L7_int32 cfgValid = L7_FALSE;
L7_int32 cpu_card_id;

/**************************************************************************
* @purpose  Perform all hardware specific initialization for this CPU
*           complex.
*
* @param    none
*
* @returns  L7_SUCCESS - CPU initialization completed.
* @returns  L7_FAILURE - Problem detected in CPU initialization.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuInit(void)
{
  L7_RC_t retVal = L7_SUCCESS;

  cpu_card_id = CARD_CPU_GTO_REV_1_ID;

  /* No VPD to read */
  cfgValid = L7_TRUE;

  return(retVal);
}
