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

// PTin added: first changes
#include "immap_cpm2.h"
#include "addrmap.h"
#include "pci.h"
#include "debug.h"

volatile cpm2_map_t *PPC8280RegPtr= NULL;//ponteiro para os registos do processador
TAddrMap      PPC8280RegAddrMap;
unsigned long IMMR_ADDR = 0xFF000000;
// PTin end

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

  // PTin added: first changes
  if (bspapiIplModelGet()==0x00508270UL)
  {
    /* Inicializar chipselects */    
    if ( (PPC8280RegPtr = AddrAlloc(&PPC8280RegAddrMap, IMMR_ADDR , 0x00100000)) == MAP_FAILED ) {
      printf("\rSTART_UP: Aloc MPC8280 [ERROR]\n\r");
      return(L7_FAILURE);
    }
    sleep(1);
    PCI_Init();
    sleep(1);
    //PCI_Teste();
    if ( AddrFree(&PPC8280RegAddrMap) ) {
      printf("\rSTART_UP: Free MPC8280 [ERROR]\n\r");
      return(L7_FAILURE);
    }
  }
  // PTin end

  return(retVal);
}
