/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_map_rto.c
*
* @purpose    PIM-DM Unicast Routing Table Access Related Function
*             Definitions
*
* @component  PIM-DM
*
* @comments   none
*
* @create     25-Apr-06
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#include "l7_pimdm_api.h"
#include "pimdm_map_util.h"
#include "pimdm_main.h"
#include "pimdm_rto.h"
#include "pimdm_map_v6_wrappers.h"
#include "pimdm_map_debug.h"

/*********************************************************************
*
* @purpose  callback function for rto best route.
*
* @param    none
*                            
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapRto4BestRouteClientCallback (void)
{
  pimdmMapCB_t* pimdmMapCbPtr = L7_NULLPTR;
  L7_uint32 gblAdminMode = L7_DISABLE;

  if(pimdmMapCtrlBlockGet(L7_AF_INET, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        L7_AF_INET);
    return;
  }

  /* Post the event if PIM-DM Map Global Admin mode is enabled.
   */
  if (pimdmMapPimdmAdminModeGet (pimdmMapCbPtr->familyType, &gblAdminMode)
                              != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "PIMDM Global Admin Mode is not enabled for \
                       familyType %d\n",pimdmMapCbPtr->familyType);
    return;
  }
  else
  {
    if (gblAdminMode != L7_ENABLE)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "PIMDM Global Admin Mode is not enabled for \
                       familyType %d\n",pimdmMapCbPtr->familyType);
      return;
    }
  }

  /* Set the PIM-DM RTO Timer */
  pimdmRTOTimerStart (pimdmMapCbPtr->cbHandle);

  return;
}

/*********************************************************************
*
* @purpose  callback function for rto6 best route.
*
* @param    none
*                            
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapRto6BestRouteClientCallback (void)
{
  pimdmMapCB_t* pimdm6MapCbPtr = L7_NULLPTR;
  L7_uint32 gblAdminMode = L7_DISABLE;

  if(pimdmMapCtrlBlockGet(L7_AF_INET6, &pimdm6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        L7_AF_INET6);
    return;
  }

  /* Post the event if PIM-DM Map Global Admin mode is enabled.
   */
  if (pimdmMapPimdmAdminModeGet (pimdm6MapCbPtr->familyType, &gblAdminMode)
                              != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "PIMDM Global Admin Mode is not enabled for \
                       familyType %d\n",pimdm6MapCbPtr->familyType);
    return;
  }
  else
  {
    if (gblAdminMode != L7_ENABLE)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "PIMDM Global Admin Mode is not enabled for \
                       familyType %d\n",pimdm6MapCbPtr->familyType);
      return;
    }
  }

  /* Set the PIM-DM RTO Timer */
  pimdmRTOTimerStart (pimdm6MapCbPtr->cbHandle);

  return;
}

/*********************************************************************
*
* @purpose  Registration with RTO for Best Route Changes.
*
* @param    addrFamily    @b{(input)} Address Family Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t
pimdmMapRTOBestRouteClientRegister (L7_uint32 addrFamily)
{
  if (addrFamily == L7_AF_INET)
  {
    if (rtoBestRouteClientRegister ("PIM-DM-Map_v4",
                                    pimdmMapRto4BestRouteClientCallback)
                                 != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else if (addrFamily == L7_AF_INET6)
  {
    if (pimdmMapRto6BestRouteRegister ("PIM-DM-Map_v6",
                                       pimdmMapRto6BestRouteClientCallback)
                                    != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  DeRegistration with RTO for Best Route Changes.
*
* @param    addrFamily    @b{(input)} Address Family Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t
pimdmMapRTOBestRouteClientDeRegister (L7_uint32 addrFamily)
{
  if (addrFamily == L7_AF_INET)
  {
    if (rtoBestRouteClientDeregister ("PIM-DM-Map_v4",
                                    pimdmMapRto4BestRouteClientCallback)
                                 != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else if (addrFamily == L7_AF_INET6)
  {
    if (pimdmMapRto6BestRouteDeRegister ("PIM-DM-Map_v6",
                                       pimdmMapRto6BestRouteClientCallback)
                                    != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
