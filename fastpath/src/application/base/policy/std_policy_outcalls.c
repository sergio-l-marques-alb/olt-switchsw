/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename std_policy_outcalls.c
*
* @purpose Contains definitions to support LVL7 standard policy offerings
*
* @component 
*
* @comments 
*
* @create 06/13/2003
*
* @author rjindal
* @end
*
**********************************************************************/

#include "nimapi.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid in policy
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
L7_BOOL policyIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
      return L7_TRUE;
      break;

    default:
      return L7_FALSE;
      break;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid in policy
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
L7_BOOL policyIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return policyIsValidIntfType(sysIntfType);
  }
  return L7_FALSE;
}


