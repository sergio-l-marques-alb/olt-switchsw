/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pml_outcalls.c
*
* @purpose 
*
* @component Port MAC Locking
*
* @comments 
*
* @create 05/21/2004
*
* @author colinw
* @end
*
**********************************************************************/

#include "nimapi.h"
#include "pml_api.h"
#include "dot3ad_api.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid in Port MAC Locking
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
L7_BOOL pmlIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
      return L7_TRUE;
      break;

    case L7_LAG_INTF:
      return L7_TRUE;
      break;

    default:
      return L7_FALSE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid in Port MAC Locking
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
L7_BOOL pmlIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_FALSE;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return pmlIsValidIntfType(sysIntfType);
  }

  return L7_FALSE;
}

