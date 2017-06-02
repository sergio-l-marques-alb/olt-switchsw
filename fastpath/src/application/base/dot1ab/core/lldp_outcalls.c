/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename lldp_outcalls.c
*
* @purpose   calls to external FASTPATH components
*
* @component 802.1AB
*
* @comments
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/

#include "nimapi.h"
#include "poe_api.h"

/*********************************************************************
* @purpose  Determine if the interface is valid for 802.1AB
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
L7_BOOL lldpIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    if (sysIntfType == L7_PHYSICAL_INTF)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}
/*********************************************************************
* @purpose  Determine if the interface is valid and capable of supporting poe
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
* *********************************************************************/
L7_RC_t lldpPoeIsValidIntf(L7_uint32 intIfNum)
{
#ifdef L7_POE_PACKAGE
  L7_INTF_TYPES_t sysIntfType;

  if ( (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS) ||
       (sysIntfType != L7_PHYSICAL_INTF) )
  {
    return L7_FAILURE;
  }
  if (poeIsValidIntf(intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
#else
  return L7_FAILURE;
#endif
}
/*********************************************************************
* @purpose  Determine if the interface is valid and capable of supporting poe PSE
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t lldpPoeIsValidPSEIntf(L7_uint32 intIfNum)
{
#ifdef L7_POE_PACKAGE
  L7_INTF_TYPES_t sysIntfType;

  if ( (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS) ||
       (sysIntfType != L7_PHYSICAL_INTF) )
  {
    return L7_FAILURE;
  }
  if (poeIsValidPSEIntf(intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
#else
  return L7_FAILURE;
#endif
}  
/*********************************************************************
* @purpose  Determine if the interface is capable of supporting poe PD.
*
* @param    intIfNum    Internal    Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpPoeIsValidPDIntf(L7_uint32 intIfNum)
{
#ifdef L7_POE_PACKAGE
  L7_INTF_TYPES_t sysIntfType;

  if ( (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS) ||
       (sysIntfType != L7_PHYSICAL_INTF) )
  {
    return L7_FAILURE;
  }
  if (poeIsValidPDIntf(intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
#else
  return L7_FAILURE;
#endif
}
