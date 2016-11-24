/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename mgmd_outcalls.c
*
* @purpose Functions providing policy services to the MGMD component.
*
* @component 
*
* @comments 
*
* @create 06/25/2003
*
* @author jeffr
* @end
*
**********************************************************************/

#include "l7_mgmdmap_include.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid in MGMD
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
L7_BOOL mgmdIntfIsValidType(L7_uint32 sysIntfType)
{
  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid interface\n");
    return L7_FALSE;
  }
  
  return L7_TRUE;
}
/*********************************************************************
* @purpose  Determine if the interface is valid in MGMD
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
L7_BOOL mgmdIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType = L7_NULL;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid interface in MGMD\n");
    return L7_FALSE;
  }

  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Invalid interface\n");
    return L7_FALSE;
  }

  return L7_TRUE;
}
