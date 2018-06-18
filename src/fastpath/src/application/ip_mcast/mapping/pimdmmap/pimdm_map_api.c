/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  pimdm_map_api.c
*
* @purpose   PIMDM API functions
*
* @component PIMDM Mapping Layer
*
* @comments  none
*
* @create    03/06/2006
*
* @author    gkiran
*
* @end
*
**********************************************************************/

#include "l3_mcast_commdefs.h"
#include "l3_addrdefs.h"
#include "l7_mcast_api.h"
#include "l7_pimdm_api.h"
#include "trap_layer3_mcast_api.h"
#include "l3_defaultconfig.h"
#include "mcast_wrap.h"
#include "pimdm_map.h"
#include "pimdm_map_vend_exten.h"
#include "pimdm_map_debug.h"
#include "pimdm_map_util.h"

/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Get the PIMDM administrative mode
*
* @param    familyType  @b{(input)}  Address Family type
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimdmAdminModeGet(L7_uchar8 familyType,
                                  L7_uint32 *mode)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    /*Check whether PIMDM component is initialized*/
    if (pimdmMapCbPtr->pPimdmMapCfgData == L7_NULL)
    {
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIMDM component not initialized");
        return L7_FAILURE;
    }
  
    /*Copy the admin mode */
    *mode = pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the PIMDM administrative mode
*
* @param    familyType  @b{(input)}  Address Family type
* @param    mode        @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimdmAdminModeSet(L7_uchar8 familyType,
                                  L7_uint32 mode)
{
    L7_uint32 currentMcastProtocol = L7_NULL;
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    /* this fails if PIMDM component not started by configurator */
    if (pimdmMapCbPtr->pPimdmMapCfgData == L7_NULL)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIMDM component not initialized");
      return L7_FAILURE;
    }
    
    if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    {
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Invalid mode (%d)",mode);
        return L7_FAILURE;
    }

    /* assumes current admin mode already established, 
     * so nothing more to do here 
     */
    if (mode == pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode)
    {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, 
                        "\n Mode Already Enabled\n");
        return L7_SUCCESS;
    }

    if (mode == L7_DISABLE)
    {
        pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode = mode;
        pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
       if ((mcastMapIpCurrentMcastProtocolGet(pimdmMapCbPtr->familyType, &currentMcastProtocol) == L7_SUCCESS) && 
           (currentMcastProtocol == L7_MCAST_IANA_MROUTE_PIM_DM))
       {
         if (mcastMapIpCurrentMcastProtocolSet(pimdmMapCbPtr->familyType, L7_MCAST_IANA_MROUTE_UNASSIGNED)
                                               != L7_SUCCESS)
         {
           PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                           "Failed to reset current Mcast Protocol",
                           currentMcastProtocol);
         }
       }   
      if (pimdmMapUIEventSend (pimdmMapCbPtr, PIMDMMAP_ADMIN_MODE_SET, mode, 0, 0)
                            != L7_SUCCESS)
      {
        PIMDM_MAP_DEBUG  (PIMDM_MAP_DEBUG_FAILURES,
                          "Map Event Post Failed for eventType:%d for \
                           familyType %d \n",PIMDMMAP_ADMIN_MODE_SET,familyType);
      }
       
    }
    else if (mode == L7_ENABLE )
    {
        if ((mcastMapIpCurrentMcastProtocolGet (pimdmMapCbPtr->familyType, &currentMcastProtocol)
                                             == L7_SUCCESS) &&
                      currentMcastProtocol != L7_MCAST_IANA_MROUTE_PIM_DM &&
                      currentMcastProtocol != L7_MCAST_IANA_MROUTE_UNASSIGNED)

        {
           /* Here L7_ERROR  return status is used by CLI to recognize the failure
              as "other MRP is configured" and other return status for representing 
              other failure states. The return status can be enhanced to capture
              various return states in multicast. */
            PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Another MCAST protocol(%d)is \
                               already enabled",currentMcastProtocol);
            return L7_ERROR;
        }
       if(pimdmMapMemoryInit(pimdmMapCbPtr) != L7_SUCCESS)
       {
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Another MCAST protocol might be enabled");
        return L7_FAILURE;                     
       }
       pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode = mode;
       pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

       mcastMapIpCurrentMcastProtocolSet (pimdmMapCbPtr->familyType, 
                                          L7_MCAST_IANA_MROUTE_PIM_DM);        
       if (pimdmMapUIEventSend (pimdmMapCbPtr, PIMDMMAP_ADMIN_MODE_SET, mode, 0, 0)
                             != L7_SUCCESS)
       {
         PIMDM_MAP_DEBUG  (PIMDM_MAP_DEBUG_FAILURES,
                           "Map Event Post Failed for eventType:%d for \
                            familyType %d \n",PIMDMMAP_ADMIN_MODE_SET,familyType);
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIMDM Map Event Post Failed for eventType:%d.\n",
                 PIMDMMAP_ADMIN_MODE_SET);
        /*Reset everything back to previous values*/
        pimdmMapMemoryDeInit(pimdmMapCbPtr);
        pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode = L7_DISABLE;
        pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
        mcastMapIpCurrentMcastProtocolSet (pimdmMapCbPtr->familyType, 
                                                 L7_MCAST_IANA_MROUTE_UNASSIGNED);                              
       }
    }

    return L7_SUCCESS;
}

/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  INTERFACE ENTRY GET/NEXT
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Check whether the routing internal interface number is valid
*
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfEntryGet(L7_uchar8 familyType,
                             L7_uint32 intIfNum)
{
    L7_uint32 mode = L7_DISABLE;
    
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /*Check whether Interface is configurable*/
    if ((pimdmMapIntfAdminModeGet(familyType, intIfNum,  &mode) == L7_SUCCESS)
         && (mode == L7_ENABLE))
    {
        return L7_SUCCESS;
    }
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,"PIMDM not enabled on \
                     intIfNum(%d) for familyTpe %d",intIfNum,familyType);
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next sequential routing internal interface number
*
* @param    familyType   @b{(input)}  Address Family type
* @param    pIntIfNum    @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfEntryNextGet(L7_uchar8 familyType,
                                 L7_uint32 *pIntIfNum)
{

  L7_uint32 rtrIfNum = L7_NULL;
  L7_uint32 tmpIntIfNum = L7_NULL;
  L7_uint32 mode = L7_DISABLE;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  for (tmpIntIfNum = *pIntIfNum+1; tmpIntIfNum <= L7_MAX_INTERFACE_COUNT; 
       tmpIntIfNum++)
  {
    if (mcastIpMapIntIfNumToRtrIntf(familyType, 
                                   tmpIntIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if ((pimdmMapIntfAdminModeGet(familyType, tmpIntIfNum, 
          &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
      {
        *pIntIfNum = tmpIntIfNum;
        return L7_SUCCESS;
      }
    }
  }
  PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,"Failed to get the next PIMDM \
                    enabled interface after intfIfNum(%d) for familyType %d\n",
                    *pIntIfNum,familyType);
  return L7_FAILURE; 
}


/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE STATS
 *---------------------------------------------------------------------
 */
/* write code to get  the two new statistical parameters for PIMDM.
 * 1.Number of neighbors for a specific interface
 * 2.Niegbor uptime for a given nieghbor on a given interfaces
 */
/*********************************************************************
* @purpose  Get the Neighbor count specified interface
*
* @param    familyType @b{(input)}  Address Family type
* @param    intIfNum   @b{(input)}  Interface Number
* @param    nbrCount   @b{(output)} neighbour count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR 
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrCountGet(L7_uchar8 familyType,L7_uint32 intIfNum,
                            L7_uint32 *nbrCount)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    if(pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum)!= L7_TRUE)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"PIM-DM not operational \
                      on intIfNum(%d) for familyType %d\n",
                      intIfNum,familyType);
      return L7_ERROR;
    }
    return pimdmMapExtenNbrCountGet(pimdmMapCbPtr, intIfNum, nbrCount);
}
/*********************************************************************
* @purpose  Get the PIMDM Neighbor Uptime for the specified neighbour
*           of the specified interface
*
* @param    familyType  @b{(input)} Address Family type.
* @param    ngbrIpAddr   @b{(input)} Neighbhour IP Address.
* @param    ngbrUpTime  @b{(output)} The time since the neighbour is up
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrUpTimeGet(L7_uchar8 familyType,L7_inet_addr_t *ngbrIpAddr,
                             L7_uint32 *ngbrUpTime)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    /*Get the neighbor count from vendor stack*/
    return pimdmMapExtenNbrUpTimeGet(pimdmMapCbPtr, 
                                          ngbrIpAddr, ngbrUpTime);
}
/*********************************************************************
* @purpose  Get the PIMDM Neighbor Uptime for the specified neighbour
*           of the specified interface
*
* @param    familyType  @b{(input)} Address Family type.
* @param    ngbrIpAddr   @b{(input)} Neighbhour IP Address.
* @param    ngbrIpAddr   @b{(input)} internal interface number
* @param    ngbrUpTime  @b{(output)} The time since the neighbour is up
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrUpTimeByIfIndexGet(L7_uchar8 familyType,L7_inet_addr_t *ngbrIpAddr,
                             L7_uint32 intIfNum,
                             L7_uint32 *ngbrUpTime)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    /*Get the neighbor count from vendor stack*/
    return pimdmMapExtenNbrUpTimeByIfIndexGet(pimdmMapCbPtr, 
                                          ngbrIpAddr,intIfNum, ngbrUpTime);
}

/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Get the administrative mode of a PIMDM routing interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR 
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfAdminModeGet(L7_uchar8 familyType, L7_uint32 intIfNum,
                                 L7_uint32 *mode)
{
    pimdmCfgCkt_t   *pCfg = L7_NULLPTR;
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }
  
    if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
    {
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                           "intIfNum(%d) not valid for PIMDM configuration\n",
                           intIfNum);
        return L7_ERROR;
    }
  
    /* Get the  mode*/
    *mode = pCfg->mode;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the administrative mode of a PIMDM routing interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR 
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfAdminModeSet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                 L7_uint32 mode)
{
    pimdmCfgCkt_t   *pCfg = L7_NULLPTR;
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* These pre-conditions has to be removed when the support is provided 
       for unnumbered interfaces for PIMDM */
  if (mcastIpMapIsIntfUnnumbered(familyType,intIfNum) == L7_TRUE)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "\n PIMDM cannot be set on unnumbered interface intIfNum(%d)", intIfNum);
    return L7_FAILURE;
  }


    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    /*return error if mode value is otherthan Disable/enable */
    if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    {
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Invalid mode (%d) on intIfNum(%d)",
                           mode,intIfNum);
        return L7_ERROR;
    }

    if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, 
                      "\nIntfIsConfigurable Failed\n");
      return L7_ERROR;
    }

    /*check whether the mode is already set to the same value*/
    if(pCfg->mode == mode)
    {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, 
                        "\nAlready Enabled\n");
        return L7_SUCCESS;
    }

    pCfg->mode = mode;
    pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (pimdmMapUIEventSend (pimdmMapCbPtr, PIMDMMAP_INTF_MODE_SET, mode,
                             intIfNum, 0)
                          != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG  (PIMDM_MAP_DEBUG_FAILURES,"Event Post Failed for \
                        eventType:%d for familyType %d",
                        PIMDMMAP_INTF_MODE_SET,familyType);
    }

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Exit\n");

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the operational state of the specified interface.
*
* @param    familyType      @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfOperationalStateGet(L7_uchar8 familyType,
                                        L7_uint32 intIfNum)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }
    if(pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) == L7_TRUE)
    {
       return L7_SUCCESS;
    }
    else 
    {
       PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,"PIMDM not opertaional on \
                        interface %d for familyType %d",
                         intIfNum,familyType);
       return L7_FAILURE;
    }
}

/*********************************************************************
* @purpose  Get the Hello interval of a PIMDM routing interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)}  Interface Number
* @param    helloIntvl  @b{(output)} Hello interval value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR 
*
*
* @comments The frequency at which PIM Hello messages are transmitted
*           on this interface
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfHelloIntervalGet(L7_uchar8 familyType,
         L7_uint32 intIfNum, L7_uint32 *helloIntvl)
{
    pimdmCfgCkt_t   *pCfg = L7_NULLPTR;
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                         "intIfNum(%d) not valid for PIMDM configuration \
                         for familyType %d\n",intIfNum,familyType);
      return L7_ERROR;
    }
  
    *helloIntvl= pCfg->pimInterfaceHelloInterval;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Hello interval of a PIMDM routing interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    helloIntvl  @b{(input)} Hello interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments The frequency at which PIM Hello messages are transmitted 
*           on this interface
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfHelloIntervalSet(L7_uchar8 familyType,
           L7_uint32 intIfNum, L7_uint32 helloIntvl)
{
    pimdmCfgCkt_t   *pCfg = L7_NULLPTR;
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "intIfNum(%d) not valid for PIMDM configuration \
                        for familyType %d\n",intIfNum,familyType);
      return L7_FAILURE;
    }

    pCfg->pimInterfaceHelloInterval= helloIntvl;
    pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (pimdmMapUIEventSend (pimdmMapCbPtr, PIMDMMAP_HELLO_INTVL_SET, 0,
                             intIfNum, helloIntvl)
                          != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG  (PIMDM_MAP_DEBUG_FAILURES,"Event Post Failed for \
                        eventType(%d) for familyType %d",
                        PIMDMMAP_HELLO_INTVL_SET,familyType);
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DR IP address for the specified specified interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    dsgRtrIpAddr  @b{(output)}IP Address of the Designated Router
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfDRGet(L7_uchar8 familyType,
     L7_uint32 intIfNum, L7_inet_addr_t *dsgRtrIpAddr)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
    return L7_FAILURE;
  }

  if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) != L7_TRUE)
  {
    inetAddressSet(pimdmMapCbPtr->familyType, 0, dsgRtrIpAddr);
    return L7_SUCCESS;
  }

  /*Get the DR value*/
  return pimdmMapExtenIntfDRGet(pimdmMapCbPtr, intIfNum, dsgRtrIpAddr);
}

/*********************************************************************
* @purpose  Get the PIMDM mode on the specified interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)}  Interface Number
* @param    mode          @b{(output)}  PIM protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfModeGet(L7_uchar8 familyType,
             L7_uint32 intIfNum, L7_uint32 *mode)
{
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /*Return that PIM Dense mode is set*/
    /**mode =  L7_PIM_DENSE;*/
    return   L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the PIMDM mode on the specified interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    mode          @b{(input)} PIM protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfModeSet(L7_uchar8 familyType,L7_uint32 intIfNum, 
                            L7_uint32 mode)
{
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Not supported*/
    return L7_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE INFO (NON-MIB)
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Get the IP address of the specified PIMDM interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    ipAddr        @b{(output)} Interface IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfIPAddrGet(L7_uchar8 familyType, L7_uint32 intIfNum, 
                              L7_inet_addr_t *ipAddr)
{
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mcastIpMapRtrIntfIpAddressGet(familyType, 
                                      intIfNum, ipAddr) != L7_SUCCESS)
    {
       PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                        "Failed to get IP address on intIfNum(%d) for \
                         familyType %d",intIfNum,familyType);
       return L7_FAILURE;
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the subnet mask of the specified PIMDM interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    netMask     @b{(output)} Interface subnet mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfSubnetMaskGet(L7_uchar8 familyType,
                         L7_uint32 intIfNum, L7_inet_addr_t *netMask)
{
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mcastIpMapRtrIntfNetMaskGet(familyType, 
                                    intIfNum, netMask) != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                         "Failed to get subnet mask on intIfNum(%d) \
                          for familyType %d",intIfNum,familyType);
      return L7_FAILURE;
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the PIMDM neighbor entry on the specified interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    ngbrIpAddr   @b{(output)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrEntryGet(L7_uchar8 familyType,
                            L7_inet_addr_t *nbrIpAddr)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenNbrEntryGet(pimdmMapCbPtr, nbrIpAddr);
    /* Get the neighbor entry*/
}

/*********************************************************************
* @purpose  Get the PIMDM Next neighbor entry on the specified interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    ngbrIpAddr    @b{(output)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrEntryNextGet(L7_uchar8 familyType,
                                L7_inet_addr_t* nbrIpAddr)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenNbrEntryNextGet(pimdmMapCbPtr, nbrIpAddr);
}

/*********************************************************************
* @purpose  Check whether neighbor entry exists for the specified IP address
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    nbrIpAddr   @b{(input)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrEntryByIfIndexGet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                     L7_inet_addr_t* nbrIpAddr)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenNbrEntryByIfIndexGet(pimdmMapCbPtr, intIfNum, nbrIpAddr);
}

/*********************************************************************
* @purpose  Get the PIMDM Next neighbor entry By interface index 
*           on the specified interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    ngbrIpAddr    @b{(in/out)} Internal Interface Number
* @param    ngbrIpAddr    @b{(output)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrEntryByIfIndexNextGet(L7_uchar8 familyType,L7_uint32 *outIntIfNum,
                                L7_inet_addr_t* nbrIpAddr)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenNbrEntryByIfIndexNextGet(pimdmMapCbPtr, outIntIfNum,nbrIpAddr);
}

/*********************************************************************
* @purpose  Get the Pimdm Interface index of the specified interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    nbrIpAddr   @b{(output)} Neighbor Ip address
* @param    pIntIfNum   @b{(output)} L7_uint32 interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrIfIndexGet(L7_uchar8 familyType, 
                              L7_inet_addr_t *nbrIpAddr,
                              L7_uint32 *pIntIfNum)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenNbrIntfIndexGet(pimdmMapCbPtr, 
                                        nbrIpAddr, pIntIfNum);
}

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Expire time for the specified neighbour
*           of the specified interface
*
* @param    familyType     @b{(input)} Address Family type
* @param    ngbrIpAddr     @b{(input)}Neighbor Ip address
* @param    nbrExpireTime  @b{(output)} Get the neighbor expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Time before this neighbor entry status expires
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrExpiryTimeGet(L7_uchar8 familyType,
             L7_inet_addr_t *nbrIpAddr, L7_uint32 *nbrExpireTime)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenNbrExpireTimeGet(pimdmMapCbPtr, 
                                         nbrIpAddr, nbrExpireTime);
}

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Expire time for the specified neighbour
*           of the specified interface
*
* @param    familyType     @b{(input)} Address Family type
* @param    ngbrIpAddr     @b{(input)}Neighbor Ip address
* @param    ngbrIpAddr     @b{(input)}internal interface number
* @param    nbrExpireTime  @b{(output)} Get the neighbor expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Time before this neighbor entry status expires
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrExpiryTimeByIfIndexGet(L7_uchar8 familyType,
             L7_inet_addr_t *nbrIpAddr, L7_uint32 intIfNum,L7_uint32 *nbrExpireTime)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenNbrExpireTimeByIfIndexGet(pimdmMapCbPtr, 
                                         nbrIpAddr,intIfNum, nbrExpireTime);
}

/*********************************************************************
* @purpose  Get the PIMDM  neighbor mode on the specified interface
*
* @param    familyType     @b{(input)} Address Family type
* @param    ngbrIpAddr     @b{(input)}Neighbor Ip address
* @param    mode           @b{(output)} PIM protocol mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapNbrModeGet(L7_uchar8 familyType,
                           L7_inet_addr_t *nbrIpAddr,
                           L7_uint32 *mode)
{
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /**mode = L7_PIM_DENSE;*/
    *mode = 1; /* To replace with L7_PIMDM_DENSE */
    return 	L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the assert metric
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    assertMetric         @b{(output)} Assert metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteAssertMetricGet(L7_uchar8 familyType,
           L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
           L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetric)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteAssertMetricGet(pimdmMapCbPtr, 
            ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, assertMetric);
}

/*********************************************************************
* @purpose  Get the assert metric preference
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    assertMetricPref     @b{(output)} Assert metric preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteAssertMetricPrefGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetricPref)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteAssertMetricPrefGet(pimdmMapCbPtr, 
          ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask,assertMetricPref);
}

/*********************************************************************
* @purpose  Get the RPT bit status
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    RPTBitStatus         @b{(output)} RPT bit status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteAssertRPTBitGet(L7_uchar8 familyType,
   L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
   L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *RPTBitStatus)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteAssertRPTBitGet(pimdmMapCbPtr, 
           ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, RPTBitStatus);
}
/*********************************************************************
* @purpose  Get the Upstream assert timer status
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    upstreamAssertTimer  @b{(output)}  Upstream assert timer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteUpstreamAssertTimerGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *upstreamAssertTimer)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteUpstreamAssertTimerGet(pimdmMapCbPtr, 
          ipMRouteGroup,ipMRouteSource, ipMRouteSourceMask, 
          upstreamAssertTimer);
}

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast route table entry
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteEntryGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
    L7_inet_addr_t  *ipMRouteSourceMask)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return  pimdmMapExtenIpMRouteEntryGet(pimdmMapCbPtr, ipMRouteGroup, 
           ipMRouteSource,ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast route table Next entry
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(inout)}  Multicast Group address
* @param    ipMRouteSource       @b{(inout)}  Source address
* @param    ipMRouteSourceMask   @b{(inout)}  Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteEntryNextGet(L7_uchar8 familyType,
   L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource,
   L7_inet_addr_t* ipMRouteSourceMask)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteEntryNextGet(pimdmMapCbPtr, ipMRouteGroup,
                                           ipMRouteSource, ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast Flags
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    flags                @b{(output)} flags.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteFlagsGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32*  flags)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteFlagsGet(pimdmMapCbPtr, ipMRouteGroup, 
                ipMRouteSource, ipMRouteSourceMask, flags);
}

/*********************************************************************
* @purpose  Get the PIMDM  Next Hop table entry
*
* @param    familyType                 @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteNextHopSource      @b{(input)}  Source address
* @param    ipMRouteNextHopSourceMask  @b{(input)}  Mask Address
* @param    ipMRouteNextHopRtrIfNum    @b{(input)}  Next Hop RtrIfIndex
* @param    ipMRouteNextHopAddress     @b{(input)}  Next Hop Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteNextHopEntryGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *ipMRouteNextHopGroup, 
                                   L7_inet_addr_t *ipMRouteNextHopSource, 
                                   L7_inet_addr_t  *ipMRouteNextHopSourceMask, 
                                   L7_uint32 ipMRouteNextHopRtrIfNum,
                                   L7_inet_addr_t *ipMRouteNextHopAddress)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteNextHopEntryGet(pimdmMapCbPtr, 
          ipMRouteNextHopGroup, ipMRouteNextHopSource,  
          ipMRouteNextHopSourceMask,  ipMRouteNextHopRtrIfNum, 
          ipMRouteNextHopAddress);
}

/*********************************************************************
* @purpose  Get the PIMDM  Next Hop table Next entry
*
* @param  familyType                 @b{(input)}  Address Family type
* @param  ipMRouteNextHopGroup       @b{(inout)} Multicast Group address
* @param  ipMRouteNextHopSource      @b{(inout)}  Source address
* @param  ipMRouteNextHopSourceMask  @b{(inout)}  Mask Address
* @param  ipMRouteNextHopRtrIfNum    @b{(inout)}  Next Hop RtrIfIndex
* @param  ipMRouteNextHopAddress     @b{(inout)}  Next Hop Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteNextHopEntryNextGet
 (L7_uchar8 familyType, L7_inet_addr_t* ipMRouteNextHopGroup, 
  L7_inet_addr_t* ipMRouteNextHopSource, L7_inet_addr_t* ipMRouteNextHopSourceMask, 
  L7_uint32* ipMRouteNextHopRtrIfNum, L7_inet_addr_t* ipMRouteNextHopAddress)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteNextHopEntryNextGet(pimdmMapCbPtr, 
        ipMRouteNextHopGroup, ipMRouteNextHopSource, ipMRouteNextHopSourceMask,
        ipMRouteNextHopRtrIfNum,  ipMRouteNextHopAddress);        
}

/*********************************************************************
* @purpose  Get the Next Hop Prune reason on the specified interface
*
* @param    familyType                 @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteNextHopSource      @b{(input)}  Source address
* @param    ipMRouteNextHopSourceMask  @b{(input)}  Mask Address
* @param    ipMRouteNextHopRtrIfNum    @b{(input)}  Next Hop RtrIfIndex
* @param    ipMRouteNextHopAddress     @b{(input)}  Next Hop Address
* @param    PruneReason                @b{(output)} Prune reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Prune is generated in router durig assert/flood-prune 
*           and IGMP no member cases
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteNextHopPruneReasonGet
 (L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteNextHopGroup, L7_inet_addr_t *ipMRouteNextHopSource, 
  L7_inet_addr_t *ipMRouteNextHopSourceMask, L7_uint32 ipMRouteNextHopRtrIfNum,
  L7_inet_addr_t *ipMRouteNextHopAddress, L7_uint32 *pruneReason)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteNextHopPruneReasonGet(pimdmMapCbPtr, 
       ipMRouteNextHopGroup, ipMRouteNextHopSource, ipMRouteNextHopSourceMask,
          ipMRouteNextHopRtrIfNum, ipMRouteNextHopAddress, pruneReason);
}

/*********************************************************************
* @purpose  Get the PIMDM  IP multicast table  Out going interface list
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtrIfIndex          @b{(input)}  Input Router Interface Index
* @param    state               @b{(input)}  interface status
* @param    mode                @b{(output)} PIM mode
* @param    upTime              @b{(output)} entry uptime
* @param    expiryTime          @b{(output)} entry expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteOIFNextGet (L7_uchar8 familyType,
                                    L7_inet_addr_t *ipMRouteGroup,
                                    L7_inet_addr_t *ipMRouteSource,
                                    L7_inet_addr_t *ipMRouteSourceMask,
                                    L7_uint32* rtrIfIndex,
                                    L7_uint32* state,
                                    L7_uint32* mode,
                                    L7_uint32* upTime, 
                                    L7_uint32* expiryTime)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteOIFNextGet(pimdmMapCbPtr, 
    ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask,  rtrIfIndex, state,  
    mode, upTime, expiryTime);
}

/*********************************************************************
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    familyType @b{(input)} Address Family type
* @param    rtrIfNum   @b{(input)} Router Interface on which neighbor is lost.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimTrapNeighborLoss (L7_uchar8 familyType,
                                     L7_uint32 rtrIfNum)
{
  L7_uint32 intIfNum;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mcastIpMapRtrIntfToIntIfNum (familyType, rtrIfNum, &intIfNum)
                                != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to convert rtrIfNum(%d) "
                     "intIfNum(%d)", intIfNum, rtrIfNum);
    return L7_FAILURE;
  }

  return trapMgrPimTrapNeighborLoss(intIfNum, 0, L7_FALSE);
}

/*********************************************************************
* @purpose To check PIMDM is operational.
*
* @param    familyType   @b{(input)} Address Family type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimdmMapPimDmIsOperational (L7_uchar8 familyType)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
    return L7_FALSE;
  }

  if (pimdmMapCbPtr->pPimdmInfo == L7_NULLPTR)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"PIMDM component not \
                    initialized for familyType %d",familyType);
    return L7_FALSE;
  }

  if(pimdmMapCbPtr->pPimdmInfo->pimdmInitialized == L7_TRUE)
  {
    return L7_TRUE;
  }
  else 
  {
    return L7_FALSE;
  }
}

/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
*                                    
* @param    familyType   @b{(input)} Address Family type
* @param    grpIpAddr    @b{(input)} Group IP Address
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtrIfNum     @b{(output)}RtrIfIndex which is passed back 
*                                    to the calling function
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  pimdmMapRouteIfIndexGet(L7_uchar8 familyType,
    L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask, 
    L7_uint32 *rtrIfNum)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenRouteIfIndexGet(pimdmMapCbPtr, grpIpAddr,
                                             srcIpAddr, srcMask, rtrIfNum);
}

/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    familyType   @b{(input)} Address Family type
* @param    grpIpAddr    @b{(input)} Group IP Address
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtUptime     @b{(output)}route up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapRouteUptimeGet(L7_uchar8 familyType,
         L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, 
         L7_inet_addr_t *srcMask, L7_uint32 *rtUptime)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenRouteUptimeGet(pimdmMapCbPtr, grpIpAddr, 
                                       srcIpAddr, srcMask, rtUptime);
}

/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    familyType   @b{(input)} Address Family type
* @param    grpIpAddr    @b{(input)} Group IP Address
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtExpTime    @b{(output)} route expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapRouteExpiryTimeGet(L7_uchar8 familyType,
     L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, 
     L7_inet_addr_t *srcMask, L7_uint32 *rtExpTime)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenRouteExpiryTimeGet(pimdmMapCbPtr, grpIpAddr, 
                                srcIpAddr, srcMask, rtExpTime);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtAddr              @b{(output)}  rt Address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRtAddressGet(L7_uchar8 familyType,
   L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
   L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtAddr)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteRtAddrGet(pimdmMapCbPtr, ipMRouteGroup, 
                            ipMRouteSource, ipMRouteSourceMask, rtAddr);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtMask              @b{(output)}  rt mask.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRtMaskGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, 
  L7_inet_addr_t*  rtMask)
{
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      return L7_FAILURE;
    }

    return pimdmMapExtenIpMRouteRtMaskGet(pimdmMapCbPtr, ipMRouteGroup, 
                   ipMRouteSource, ipMRouteSourceMask, rtMask);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtType              @b{(output)}  rt Type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRtTypeGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtType)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to get control block");
    return L7_FAILURE;
  }

  return pimdmMapExtenIpMRouteRtTypeGet(pimdmMapCbPtr, ipMRouteGroup, 
                                ipMRouteSource, ipMRouteSourceMask, rtType);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Proto.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtproto             @b{(output)} Route Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRtProtoGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtProto)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
    return L7_FAILURE;
  }

  return pimdmMapExtenIpMRouteRtProtoGet(pimdmMapCbPtr, ipMRouteGroup, 
                                ipMRouteSource, ipMRouteSourceMask, rtProto);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Address.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rpfAddr             @b{(output)} rpfAddr.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteRpfAddrGet(L7_uchar8 familyType,
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, 
  L7_inet_addr_t *rpfAddr)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
    return L7_FAILURE;
  }

  return pimdmMapExtenIpMRouteRpfAddrGet(pimdmMapCbPtr, ipMRouteGroup, 
                                 ipMRouteSource, ipMRouteSourceMask, rpfAddr);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count Get.
*
* @param    familyType         @b{(input)}   Address Family type
* @param    entryCount         @b{(output)}  entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteEntryCountGet(L7_uchar8 familyType,
                                      L7_uint32 *entryCount)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
    return L7_FAILURE;
  }

  return pimdmMapExtenIpMRouteEntryCountGet(pimdmMapCbPtr, entryCount);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count Get.
*
* @param    familyType         @b{(input)}   Address Family type
* @param    heCount            @b{(output)}  entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIpMRouteHighestEntryCountGet(L7_uchar8 familyType,
                                             L7_uint32 *heCount)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
    return L7_FAILURE;
  }

  return pimdmMapExtenIpMRouteHighestEntryCountGet(pimdmMapCbPtr, heCount);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Source Mask
* @param    outIntf             @b{(input)} Out interface. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapRouteOutIntfEntryNextGet(L7_uchar8 familyType,
 L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
 L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *outIntf)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
    return L7_FAILURE;
  }

  return pimdmMapExtenIpRouteOutIntfEntryNextGet(pimdmMapCbPtr, 
                                             ipMRouteGroup,
                                             ipMRouteSource,
                                             ipMRouteSourceMask, outIntf);
}

/*********************************************************************
* @purpose  Returns whether PIM-DM is operational on interface or not
*
*
* @param    familyType      Address Family type
* @param    intIfNum        Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimdmMapInterfaceIsOperational(L7_uchar8 familyType,
                                  L7_uint32 intIfNum)
{
    pimdmMapCB_t    *pimdmMapCbPtr = L7_NULLPTR;
 
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* Get the interface CB based on family & store family type in CB*/
    if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        familyType);
      /* Failed to get control block */
      return L7_FALSE;
    }
    if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) == L7_TRUE)
    {
      return L7_TRUE;
    }
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"PIM-DM not operational on \
                    intIfNum(%d) for familyType %d",intIfNum,familyType);
    return L7_FALSE;
}

