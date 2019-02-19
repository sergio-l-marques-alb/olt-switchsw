/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename  hpc_phy.c
*
* @purpose   PHY support functions on this platform
*
* @component hpc
*
* @create    07/10/2008
*
* @author    colinw
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "hpc_phy.h"
#include "bcm/types.h"


void *hpcShadowSemaphore[BCM_MAX_NUM_UNITS] = {L7_NULL};

/*******************************************************************************
* @purpose Used to ensure mutual exclusion for accessing various indirect 'shadow'
*          registers in the platforms PHYs.
*
* @param   unit         @b{(input)}  BCM unit
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hpcShadowSemaphoreTake(L7_uint32 unit)
{
  if (hpcShadowSemaphore[unit] == L7_NULL)
  {
    hpcShadowSemaphore[unit] = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  }
  if (hpcShadowSemaphore[unit] != L7_NULL)
  {
    osapiSemaTake(hpcShadowSemaphore[unit], L7_WAIT_FOREVER);
  }
}

/*******************************************************************************
* @purpose Used to ensure mutual exclusion for accessing various indirect 'shadow'
*          registers in the platforms PHYs.
*
* @param   unit         @b{(input)}  BCM unit
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hpcShadowSemaphoreGive(L7_uint32 unit)
{
  if (hpcShadowSemaphore[unit] != L7_NULL)
  {
    osapiSemaGive(hpcShadowSemaphore[unit]);
  }
}
