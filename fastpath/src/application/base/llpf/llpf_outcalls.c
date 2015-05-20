
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename  llpf_outcalls.c
*
* @purpose   Figure out if an interface is valid for LLPF
*
* @component Link Local Protocol Filtering
*
* @comments  LLPF can be enabled on a physical port or a LAG port
*
* @create 10/14/2009
*
* @author Vijayanand K(kvijayan)
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "nimapi.h"
#include "dot3ad_api.h"


/*********************************************************************
* @purpose  Checks to see if the interface is valid for LLPF
*          
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL llpfIntfIsValid(L7_uint32 intIfNum)
{
  
  L7_INTF_TYPES_t sysIntfType;
  L7_BOOL  rc = L7_FALSE;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    return rc;
  }

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    switch (sysIntfType)
    {
      case L7_PHYSICAL_INTF:
        rc = L7_TRUE;
        break;

      case L7_LAG_INTF:
        if (dot3adIsLagConfigured(intIfNum) == L7_FALSE)
        {
          rc = L7_FALSE;
        }
        else
        {
          rc = L7_TRUE;
        }
        break;

      default:
        rc = L7_FALSE;
        break;
    }
  }
  return rc;
}


