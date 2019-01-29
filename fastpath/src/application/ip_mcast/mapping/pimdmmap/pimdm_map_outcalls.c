/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pimdm_outcalls.c
*
* @purpose Functions providing policy services to the PIMDM component.
*
* @component 
*
* @comments 
*
* @create 06/25/2003
*
* @author jeffr
*         gkiran
* @end
*
**********************************************************************/

#include "log.h"
#include "commdefs.h"
#include "nimapi.h"
#include "pimdm_map_debug.h"
/*********************************************************************
* @purpose  Determine if the interface type is valid in PIMDM
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
L7_BOOL pimdmIntfIsValidType(L7_uint32 sysIntfType)
{
  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"Invalid interface type %d in \
                    PIMDM",sysIntfType);
    return L7_FALSE;
  }
  return L7_TRUE;
}
/*********************************************************************
* @purpose  Determine if the interface is valid in PIMDM
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
L7_BOOL pimdmIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType = L7_NULL;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to get Interface Type from NIM for \
                      intIfNum (%d)",intIfNum);
    return L7_FALSE;
  }

  return pimdmIntfIsValidType(sysIntfType);
}

