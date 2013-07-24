/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pimsm_outcalls.c
*
* @purpose Functions providing policy services to the PIMSM component.
*
* @component 
*
* @comments 
*
* @create 06/25/2003
*
* @author gkiran/dsatyanarayana
* @end
*
**********************************************************************/

#include "l7_pimsminclude.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid in PIMSM
*
* @param    sysIntfType  @b{(input)}  interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL pimsmIntfIsValidType(L7_uint32 sysIntfType)
{
  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid interface type %d",sysIntfType);
    return L7_FALSE;
  }
  return L7_TRUE;
}
/*********************************************************************
* @purpose  Determine if the interface is valid in PIMSM
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
L7_BOOL pimsmIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the internal interface type for \
                      intIfNum(%d) ",intIfNum); 
    return L7_FALSE;
  }
  return pimsmIntfIsValidType(sysIntfType);
}

