/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_outcalls.c
*
* @purpose   Protected Port outcaslls file
*
* @component protectedPort
*
* @comments 
*
* @create    6/6/2005
*
* @author    ryadagiri 
*
* @end
*             
**********************************************************************/

#include "protected_port_include.h"

/* Begin Function Declarations: protected_port_outcalls.h */

/*********************************************************************
* @purpose  Determine if the interface type is valid in private edge vlans
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
L7_BOOL protectedPortIntfTypeIsValid(L7_uint32 sysIntfType)
{
    L7_BOOL rc;

    switch (sysIntfType)
    {
    case L7_PHYSICAL_INTF:
        rc = L7_TRUE;
        break;

    default:
        rc = L7_FALSE;
        break;
    }
    return rc;
}

/*********************************************************************
* @purpose  Determine if the interface is valid in private edge vlans
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t protectedPortIntfIsValid(L7_uint32 intIfNum)
{
    L7_INTF_TYPES_t sysIntfType;
    L7_RC_t rc = L7_FAILURE;

    if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    {
        return (rc);
    }

    if (protectedPortIntfTypeIsValid(sysIntfType) == L7_FALSE)
    {
        return (rc);
    }
    else
    {
        rc = L7_SUCCESS;
        return (rc);
    }
        
}

/* End Function Declarations */
