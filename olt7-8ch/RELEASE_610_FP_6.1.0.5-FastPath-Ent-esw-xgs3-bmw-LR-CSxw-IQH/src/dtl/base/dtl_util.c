/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename  dtl_util.c
*
* @purpose   This file contains DTL utilities
*
* @component Generic 
*
* @comments
*
* @create   06/03/2005
*
* @author   ryadagiri
*
* @end
*
**********************************************************************/


#include "dtl_util.h"


/*************************************************************************
*
* @purpose Populate an array of DAPI USP structures from an interface mask
*
* @param   intfMask    @b{(input)}  interface mask
* @param   *portUsp    @b{(output)} pointr to array of DAPI_USP_t structures 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
************************************************************************/
L7_RC_t dtlIntfMaskToDapiUSPList(L7_INTF_MASK_t intfMask, DAPI_USP_t *portUsp)
{
  L7_uint32 i, j;
    nimUSP_t usp;
    L7_INTF_TYPES_t sysIntfType;

  memset( portUsp, -1, sizeof(DAPI_USP_t) * (L7_MAX_INTERFACE_COUNT + 1) );

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    return(L7_FAILURE);
  }

  for (i = 1, j = 0; i < L7_MAX_INTERFACE_COUNT; i++)
  {
        if (nimCheckIfNumber(i) != L7_SUCCESS)
    {
            continue;
    }

        if (L7_INTF_ISMASKBITSET(intfMask, i) != 0)
        {
            if(nimGetUnitSlotPort(i, &usp) != L7_SUCCESS)
            {
                /* Skip the port that could not be found, but process the rest */
                LOG_MSG("Could not get USP for intIfNum %d\n", i);
                continue;
            }
            if( (nimGetIntfTypeFromUSP(&usp, &sysIntfType) != L7_SUCCESS) &&
                              (sysIntfType != L7_PHYSICAL_INTF))
      {
                continue;
      }
      else                      /* Make a dense list */
      {
        portUsp[j].unit = usp.unit;
        portUsp[j].slot = usp.slot;
        portUsp[j].port = usp.port-1;
        j++;
      }
    }
  }

    return(L7_SUCCESS);
}
