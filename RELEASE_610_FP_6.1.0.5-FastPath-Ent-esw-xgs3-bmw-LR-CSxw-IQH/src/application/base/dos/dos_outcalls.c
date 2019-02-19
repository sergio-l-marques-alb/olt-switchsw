/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dos_outcalls.c
*
* @purpose
*
* @component DENIAL OF SERVICE
*
* @comments
*
* @create 04/13/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#include "nimapi.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid for 
*           configuration of DOS ATTACK
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dosIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
      return L7_TRUE;
      break;

    default:
      return L7_FALSE;
  }
  return L7_FALSE;

}
/*********************************************************************
* @purpose  Determine if the interface is valid for configuration
*           of DOS ATTACK
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dosIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_FALSE;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return dosIsValidIntfType(sysIntfType);
  }
  
  return L7_FALSE;
  
}

