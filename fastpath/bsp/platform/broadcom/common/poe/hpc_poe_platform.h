/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2001-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename  hpc_poe.h
*
* @purpose
*
* @component hpc
*
* @comments  The functions declared in this file are expected to be
*            implemented by platform specific code.
*
* @create    10/2/2003
*
* @author    arvinds
*
* @end
*
**********************************************************************/

#ifndef HPC_POE_PLATFORM_H
#define HPC_POE_PLATFORM_H

/*********************************************************************
* @purpose   Helper Function to determine if a port is Combo port
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
L7_uint32  hpcPoeIsComboPort(L7_uint32 port);

/*********************************************************************
* @purpose   Platform specific PoE Hardware Initialization
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hpcPlatformHwPoeInit(void);

/*********************************************************************
* @purpose   Platform specific function to initialize PoE LED Functionality
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
int hpcPlatformHwPoeLedInit(L7_uint32 cardTypeId);

/*********************************************************************
* @purpose   Platform specific function to set PoE LED for Delivering
*            Power state
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
int hpcPlatformHwPoeLedDeliveringPower(L7_uint32 channel);

/*********************************************************************
* @purpose   Platform specific function to set PoE LED for Disable state
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
int hpcPlatformHwPoeLedDisable(L7_uint32 channel);

/*********************************************************************
* @purpose   Platform specific function to set PoE LED for Detecting State
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
int hpcPlatformHwPoeLedDetecting(L7_uint32 channel);

/*********************************************************************
* @purpose   Platform specific function to set PoE LED for Overload State
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
int hpcPlatformHwPoeLedOverload(L7_uint32 channel);

/*********************************************************************
* @purpose   Platform specific function to set PoE LED for Exceeding Power Budget
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
int hpcPlatformHwPoeLedExceedPower(L7_uint32 channel);

/*********************************************************************
* @purpose   Platform specific function to determine if External Power
*            Supply is present
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hpcPlatformHwExternalSupplyPresent(void);

/*********************************************************************
* @purpose   Platform specific function to Reset PoE Device
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void hpcPlatformHwPoeReset(void);

#endif  /* HPC_POE_PLATFORM_H */
