/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi_outcalls.c
*
* @purpose
*
* @component ISCSI
*
* @comments
*
* @create 04/18/2008
* @end
*
**********************************************************************/
#include "nimapi.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid for ISCSI 
*           configuration
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
L7_BOOL iscsiIsValidIntfType(L7_uint32 sysIntfType)
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
* @purpose  Determine if the interface is valid for ISCSI configuration
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
L7_BOOL iscsiIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_FALSE;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return iscsiIsValidIntfType(sysIntfType);
  }
  return L7_FALSE;
}

