/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename ds_outcalls.c
*
* @purpose  Figure out if an interface is valid for DHCP snooping
*
* @component
*
* @comments DHCP snooping can only be enabled on a physical port
*
* @create 3/15/2007
*
* @author Rob Rice (rrice)
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "nimapi.h"


/*********************************************************************
* @purpose  Checks to see if the interface type is valid for  
*           DHCP snooping
*                    
* @param    sysIntfType   @b{(input)} interface type
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments For now, DHCP snooping is only allowed on physical ports.
*        
* @end
*********************************************************************/
L7_BOOL dsIntfTypeIsValid(L7_uint32 intIfNum, L7_uint32 sysIntfType)
{
  return ( (sysIntfType == L7_PHYSICAL_INTF) ||
           (sysIntfType == L7_LAG_INTF)
          );
}

/*********************************************************************
* @purpose  Checks to see if the interface is valid for DHCP snooping
*          
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL dsIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FALSE;

  return dsIntfTypeIsValid(intIfNum, sysIntfType);
}


