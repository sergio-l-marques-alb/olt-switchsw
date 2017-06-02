
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  dai_outcalls.c
*
* @purpose   Figure out if an interface is valid for DAI
*
* @component Dynamic ARP Inspection
*
* @comments  DAI can be enabled on a physical port or a LAG port
*
* @create 09/10/2007
*
* @author Kiran Kumar Kella
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "nimapi.h"


/*********************************************************************
* @purpose  Checks to see if the interface type is valid for  
*           DAI
*                    
* @param    sysIntfType   @b{(input)} interface type
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL daiIntfTypeIsValid(L7_uint32 intIfNum, L7_uint32 sysIntfType)
{
  return ((sysIntfType == L7_PHYSICAL_INTF) ||
          (sysIntfType == L7_LAG_INTF));
}

/*********************************************************************
* @purpose  Checks to see if the interface is valid for DAI
*          
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL daiIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FALSE;

  return daiIntfTypeIsValid(intIfNum, sysIntfType);
}


