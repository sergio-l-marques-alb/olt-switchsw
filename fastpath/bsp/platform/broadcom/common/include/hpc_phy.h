/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename  hpc_phy.h
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

#ifndef INCLUDE_HPC_PHY
#define INCLUDE_HPC_PHY

void hpcShadowSemaphoreTake(L7_uint32 unit);
void hpcShadowSemaphoreGive(L7_uint32 unit);

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
#define HPC_PHY_SHADOW_REG_LOCK(unit) \
        hpcShadowSemaphoreTake(unit)

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
#define HPC_PHY_SHADOW_REG_UNLOCK(unit) \
        hpcShadowSemaphoreGive(unit)

/*******************************************************************************
* @purpose Performs an operation. If the operation fails, then the macro will release
*          the semaphore and return.
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
#define HPC_PHY_SHADOW_IF_ERROR_RETURN(un,op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) {HPC_PHY_SHADOW_REG_UNLOCK(un); return(__rv__);} } while(0)

#endif /* INCLUDE_HPC_PHY */
