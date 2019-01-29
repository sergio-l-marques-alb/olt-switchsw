/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename mcast_outcalls.c
*
* @purpose Functions providing policy services to the MCAST component.
*
* @component 
*
* @comments 
*
* @create 06/04/2006
*
* @author gkiran
* @end
*
**********************************************************************/
#include "l3_addrdefs.h"
#include "log.h"
#include "nimapi.h"
#include "mcast_debug.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid in MCAST
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
L7_BOOL mcastIntfIsValidType(L7_uint32 sysIntfType)
{
  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Interface Type = %d is not valid.\n", sysIntfType);
    return L7_FALSE;
  }
  else
  {
    return L7_TRUE;
  }
}

/*********************************************************************
* @purpose  Determine if the interface is valid in MCAST
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
L7_BOOL mcastIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    return L7_FALSE;
  }
  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Interface = %d is not valid.\n", intIfNum);
    return L7_FALSE;
  }

  return L7_TRUE;
}

