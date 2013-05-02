/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dot3ad_outcalls.c
*
* @purpose    The functions that handle dot3ad outcall requests
*
* @component  dot3ad
*
* @comments   none
*
* @create     05/07/2003
*
* @author     
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "nimapi.h"

/*********************************************************************
* @purpose  Validate whether the interface type can participate in dot3ad
*
* @param    sysIntfType  @b({input}) interface type being checked
*
* @returns  L7_BOOL
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot3adIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
    case L7_LAG_INTF:
      return L7_TRUE;
      break;
    default:
      return L7_FALSE;
      break;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Validate whether the interface can participate in dot3ad
*
* @param    intIfNum  @b({input}) The internal interface number being checked
*
* @returns  L7_BOOL
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot3adIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t type;
  L7_BOOL rc;

  if (nimGetIntfType(intIfNum,&type) != L7_SUCCESS)
  {
    rc = L7_FALSE;
  }
  else
  {
    rc = dot3adIsValidIntfType(type);
  }
  return rc;
}

