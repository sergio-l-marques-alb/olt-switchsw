/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  pimdm_vend_exten.c
*
* @purpose   PIMDM vendor-specific functions
*
* @component PIMDM Mapping Layer
*
* @comments  none
*
* @create    02/07/2001
*
* @author    gkiran
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "commdefs.h"
#include "mcast_wrap.h"
#include "pimdm_api.h"
#include "pimdm_map.h"
#include "pimdm_map_debug.h"
#include "pimdm_map_util.h"
#include "pimdm_map_vend_ctrl.h"
#include "pimdm_intf.h"
#include "l7_mcast_api.h" 

static L7_uint32
pimAsyncSetHelloInterval(pimdmMapCB_t *pimdmMapCbPtr,
                L7_uint32 intfNum, L7_uint32 hellointervl);
static L7_RC_t
pimAsyncActivateInterface(pimdmMapCB_t *pimdmMapCbPtr, 
                  L7_uint32 rtrIfNum, L7_uint32 mode);
/*
----------------------------------------------------------------------
                     SET FUNCTIONS  -  GLOBAL CONFIG
----------------------------------------------------------------------
*/

/*********************************************************************
* @purpose  Sets the PIMDM Admin mode
*
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    mode            @b{(input)} L7_ENABLE or L7_DISABLE
* @param    pimdmDoInit     @b{(input)} Flag to check whether PIM-DM
*                                       Memory can be Initialized/
                                        De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Controls the vendor PIMDM task through the use of a semaphore,
*           taking and holding it while disabled and releasing it while
*           enabled.
*
* @comments A flag is used to allow repeated calls for the same admin
*           state without hanging the process due to an already-taken
*           semaphore.
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenPimdmAdminModeSet(pimdmMapCB_t *pimdmMapCbPtr,
                                       L7_uint32 mode, L7_BOOL pimdmDoInit)
{
  pimdmMapEvent_t pimdmMapEvent;
  L7_uint32 mcastMode=L7_DISABLE;  
  L7_uint32 rtrIfNum;    

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_EVENTS, "GlobalAdminMode - %d.\n", pimdmMapCbPtr->pPimdmInfo->pimdmInitialized);

  if (mode == L7_ENABLE)
  {
    if (pimdmMapCbPtr->pPimdmInfo->pimdmInitialized != L7_TRUE)
    {
      if(pimdmDoInit == L7_TRUE)
      {
        /* Initialize the protocol's Memory & Control Block */
        if (pimdmCnfgrInitPhase1DynamicProcess (pimdmMapCbPtr) != L7_SUCCESS)
        {
          PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Vendor Layer Dynamic Init Failed");
          PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Error initializing PIM-DM protocol component for "
                          "familyType %d",pimdmMapCbPtr->familyType); 
          return L7_FAILURE;
        }
      }

      if((mcastIpMapRtrAdminModeGet(pimdmMapCbPtr->familyType) == L7_ENABLE) &&
        (mcastMapMcastAdminModeGet(&mcastMode) ==L7_SUCCESS)&&(mcastMode == L7_ENABLE))
      {
        /* Process Admin Mode Set Event */
        pimdmMapEvent.msg.adminMode.mode = L7_ENABLE;
        if (pimdmGlobalAdminModeSet(pimdmMapCbPtr->cbHandle,
                                    &pimdmMapEvent) != L7_SUCCESS)
        {
          PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "PIMDM Global Admin Set Failed for mode (%d) for "
                          "familyType %d", mode, pimdmMapCbPtr->familyType);
          if (pimdmDoInit == L7_TRUE)
          {
            /* De-Initialize the protocol's Memory & Control Block */
            pimdmCnfgrFiniPhase1DynamicProcess (pimdmMapCbPtr);
          }
          return L7_FAILURE;
        }
        pimdmMapCbPtr->pPimdmInfo->pimdmInitialized = L7_TRUE;
      }
    }
  }
  else
  {
    if (pimdmMapCbPtr->pPimdmInfo->pimdmInitialized == L7_TRUE)
    {
      /* Process Admin Mode Set Event */
      pimdmMapEvent.msg.adminMode.mode = L7_DISABLE;
      if (pimdmGlobalAdminModeSet(pimdmMapCbPtr->cbHandle,
                                  &pimdmMapEvent) != L7_SUCCESS) 
      {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                        "PIMDM Global Admin Set Failed for mode (%d) for "
                        "familyType %d", mode, pimdmMapCbPtr->familyType);
        return L7_FAILURE;
      }
      pimdmMapCbPtr->pPimdmInfo->pimdmInitialized = L7_FALSE;
      /* pimdmGlobalAdminModeSetfuntion disables all the interfaces in the vendor code.
         So in the mapping layer,all the interfaces have to be made operationally disabled.
       */
      for (rtrIfNum= 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
      {
        pimdmMapCbPtr->pimdmIntfInfo[rtrIfNum].pimdmIsOperational= L7_FALSE;
      }
      
    } 

    if(pimdmDoInit == L7_TRUE)
      {
        /* De-Initialize the protocol's Memory & Control Block */
        if (pimdmCnfgrFiniPhase1DynamicProcess (pimdmMapCbPtr) != L7_SUCCESS)
        {
          PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                           "Vendor Layer Dynamic De-Init Failed for Family - %d",
                           pimdmMapCbPtr->familyType); 
        }
      }
  }
  return L7_SUCCESS;
}

/*
----------------------------------------------------------------------
                     SET FUNCTIONS  -  INTERFACE CONFIG
----------------------------------------------------------------------
*/

/* ######################################################################
 * NOTE:  All intf config 'set' functions return with a successful return
 *        code in the event the desired PIMDM interface structure is not
 *        found.  This allows for pre-configuration of PIMDM routing
 *        interfaces (the config gets picked up in pimdm_ifinit() when the
 *        interface structure is created).
 *
 * NOTE:  All intf config functions accept a pointer to a PIMDM vendor
 *        interface structure, if known.  Otherwise, this p parm must be
 *        set to L7_NULL.  The intIfNum parm is required in either case.
 *        (This mechanism allows these same functions to be used both when
 *        handling config requests from the user interface as well as when
 *        the vendor PIMDM code asks for its interface config information.)
 * ######################################################################
 */

/*********************************************************************
* @purpose  Set the PIMDM administrative mode for the specified interface
*
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    mode           @b{(input)} Admin mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIntfAdminModeSet(pimdmMapCB_t *pimdmMapCbPtr, 
                              L7_uint32 intIfNum, L7_uint32 mode)
{
    L7_uint32     rtrIfNum = L7_NULL;
    L7_uint32     state = L7_NULL;

    if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType, 
                                    intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert intIfNum (%d) to rtrIfNum \
                           for familyType %d",
                           intIfNum,pimdmMapCbPtr->familyType);
        return L7_FAILURE;
    }
    if (mode == L7_ENABLE)
    {
        if ((nimGetIntfActiveState(intIfNum, &state) == L7_SUCCESS) && 
             (state != L7_ACTIVE))
        {
          return L7_SUCCESS;
        }
    }
 
    if (pimAsyncActivateInterface(pimdmMapCbPtr, rtrIfNum, 
                                  mode) !=  L7_SUCCESS)
    {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to activate PIMDM on rtrIfNum %d for \
                          familyType %d",
                          rtrIfNum,pimdmMapCbPtr->familyType);
       if (mode != L7_ENABLE)
       {
         pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].pimdmIsOperational = L7_FALSE;
       }
       return L7_FAILURE;
    }
    if(mode == L7_ENABLE)
    {
      pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].pimdmIsOperational = L7_TRUE;
    }
    else
    {
      pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].pimdmIsOperational = L7_FALSE;
    }

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, 
                    "\n pimdmIntialized:%d. IntIfNum:%d RtrIfNum:%d\n", 
                    pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].pimdmIsOperational, 
                    intIfNum, rtrIfNum);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To send interface admin Mode Set event to PIMDM task.
*
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    rtrIfNum       @b{(input)} Interface Num
* @param    mode           @b{(input)} admin Mode (ENABLE / DISBALE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t
pimAsyncActivateInterface(pimdmMapCB_t *pimdmMapCbPtr, L7_uint32 rtrIfNum, 
                          L7_uint32 mode)
{
    pimdmMapEvent_t pimdmMapEvent;
    pimdmCfgCkt_t   *pCfg = L7_NULLPTR;
    L7_uint32       intIfNum = 0;
    L7_inet_addr_t  ipAddr;
    L7_inet_addr_t  ipAddrMask;

    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered\n", __FUNCTION__, __LINE__);

    /* Get the Internal Internal Interface Number */
    if (mcastIpMapRtrIntfToIntIfNum (pimdmMapCbPtr->familyType, rtrIfNum,
                                     &intIfNum) != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
            "RtrIfNum(%d) to IntIfNum Conversion Failed for familyType %d",
             rtrIfNum,pimdmMapCbPtr->familyType);
      return L7_FAILURE;
    }

    /* Get the PIM-DM Circuit Data Pointer */
    if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                       "intIfNum(%d) not valid for PIMDM configuration for \
                       familyType %d",intIfNum,pimdmMapCbPtr->familyType);
      return L7_FAILURE;
    }

    if(mode == L7_ENABLE)
    {
    
      /* Get the IP Address and Netmask for the Interface */
      inetAddressZeroSet (pimdmMapCbPtr->familyType, &ipAddr);
      inetAddressZeroSet (pimdmMapCbPtr->familyType, &ipAddrMask);
      if (mcastIpMapRtrIntfIpAddressGet (pimdmMapCbPtr->familyType, intIfNum,
                                       &ipAddr) != L7_SUCCESS)
      {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                        "Failed to get IP address on intIfNum(%d) for \
                        familyType %d",intIfNum,pimdmMapCbPtr->familyType);
        return L7_FAILURE;
      }
      if (mcastIpMapRtrIntfNetMaskGet (pimdmMapCbPtr->familyType, intIfNum,
                                     &ipAddrMask) != L7_SUCCESS)
      {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                        "Failed to get Subnet Mask on intIfNum(%d) for \
                        family Type %d",intIfNum,pimdmMapCbPtr->familyType);
        return L7_FAILURE;
      }
      inetCopy (&(pimdmMapEvent.msg.intfMode.intfAddr), &ipAddr);
      inetCopy (&(pimdmMapEvent.msg.intfMode.intfMask), &ipAddrMask);
    }

    /* Update the Event Message and post event to the protocol thread */
    pimdmMapEvent.msg.intfMode.rtrIfNum = rtrIfNum;
    pimdmMapEvent.msg.intfMode.intIfNum = intIfNum;
    pimdmMapEvent.msg.intfMode.mode = mode;
    pimdmMapEvent.msg.intfMode.helloInterval = pCfg->pimInterfaceHelloInterval;

    pimdmMapEvent.msg.intfMode.helloHoldTime = 
                            (35 * pCfg->pimInterfaceHelloInterval)/10;
    pimdmMapEvent.msg.intfMode.triggeredHelloDelay = 
                            FD_PIMDM_DEFAULT_TRIGGERED_DELAY;
    pimdmMapEvent.msg.intfMode.propagationDelay = 
                            FD_PIMDM_DEFAULT_LANPRUNEDELAY;
    pimdmMapEvent.msg.intfMode.overrideIntvl = 
                            FD_PIMDM_DEFAULT_OVERRIDEINTERVAL;
        /* Process Interface Admin Mode Set Event */
    if (pimdmIntfAdminModeSet(pimdmMapCbPtr->cbHandle,
                              &pimdmMapEvent) != L7_SUCCESS) 
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                        "Failed to set PIMDM Admin Mode for mode (%d) on \
                        intIfNum(%d) for familyType %d",mode,intIfNum,
                        pimdmMapCbPtr->familyType);
      return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Hello interval of a PIMDM routing interface
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    helloIntvl       @b{(output)} Hello interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The frequency at which PIM Hello messages are transmitted on this interface
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIntfHelloIntervalSet(pimdmMapCB_t *pimdmMapCbPtr, 
                            L7_uint32 intIfNum, L7_uint32 helloIntvl)
{
    L7_uint32     rtrIfNum;

    if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType, 
                                         intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert intIfNum (%d) to rtrIfNum \
                           for familyType %d",
                           intIfNum,pimdmMapCbPtr->familyType);
        return L7_FAILURE;
    }

    if (pimAsyncSetHelloInterval(pimdmMapCbPtr,
             rtrIfNum, helloIntvl) == L7_FAILURE)
    {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to set Hello interval on rtrIfNum(%d) \
                           for familyType %d",
                           rtrIfNum,pimdmMapCbPtr->familyType);
        return L7_FAILURE;
    }

    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  To send Hello Interval Set event to PIMDM task.
*
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intfNum         @b{(input)} Interface Number.
* @param    hellointrvl     @b{(input)} Hello Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_uint32
pimAsyncSetHelloInterval(pimdmMapCB_t *pimdmMapCbPtr, L7_uint32 intfNum, 
                         L7_uint32 hellointervl)
{
  pimdmMapEvent_t pimdmMapEvent;

  pimdmMapEvent.msg.helloIntrvl.rtrIfNum = intfNum;
  pimdmMapEvent.msg.helloIntrvl.intrvl = hellointervl;

  /* Process Hello Interval Set Event */
  if (pimdmIntfHelloIntervalSet(pimdmMapCbPtr->cbHandle,
                                &pimdmMapEvent) != L7_SUCCESS) 
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
            "Failed to set Hello interval on intIfNum(%d) for familyType %d",
            intfNum,pimdmMapCbPtr->familyType);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the DR IP address for the specified specified interface
*
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    dsgRtrIpAddr    @b{(output)}IP Address of the Designated Router
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIntfDRGet(pimdmMapCB_t *pimdmMapCbPtr, 
          L7_uint32 intIfNum, L7_inet_addr_t *dsgRtrIpAddr)
{
  L7_uint32 rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType, 
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
     PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert intIfNum (%d) to rtrIfNum \
                           for familyType %d",
                           intIfNum,pimdmMapCbPtr->familyType);
      return L7_FAILURE;
  }
#ifdef PIMDM_MAP_TBD
  return (pimdmExtenApiIntfDRGet (pimdmMapCbPtr->cbHandle, 
                                                 rtrIfNum, dsgRtrIpAddr));
#endif
  return L7_SUCCESS;
}


/*
----------------------------------------------------------------------
                     GET FUNCTIONS  -  INTERFACE -Statistics
----------------------------------------------------------------------
*/


/*********************************************************************
* @purpose  Get the PIMDM Neighbor Uptime for the specified neighbour
*           of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)}  Mapping Control Block.
* @param    nbrIpAddr        @b{(input)}  Neighbhour IP Address.
* @param    nbrUpTime        @b{(output)}  The time since the neighbour is up
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrUpTimeGet(pimdmMapCB_t *pimdmMapCbPtr, 
                            L7_inet_addr_t *nbrIpAddr, L7_uint32 *nbrUpTime)
{
  return (pimdmExtenApiNbrUpTimeGet (pimdmMapCbPtr->cbHandle, 
                                                   nbrIpAddr, nbrUpTime));
}

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Uptime for the specified neighbour
*           of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)}  Mapping Control Block.
* @param    nbrIpAddr        @b{(input)}  Neighbhour IP Address.
* @param    nbrIpAddr        @b{(input)}  internal interface number
* @param    nbrUpTime        @b{(output)}  The time since the neighbour is up
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrUpTimeByIfIndexGet(pimdmMapCB_t *pimdmMapCbPtr, 
                            L7_inet_addr_t *nbrIpAddr,L7_uint32 intIfNum, L7_uint32 *nbrUpTime)
{
  return (pimdmExtenApiNbrUpTimeByIfIndexGet (pimdmMapCbPtr->cbHandle, 
                                                   nbrIpAddr,intIfNum, nbrUpTime));
}
/*********************************************************************
* @purpose  Get the Neighbor count specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum         @b{(input)}  Interface Number
* @param    nbrCount         @b{(output)} neighbour count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrCountGet(pimdmMapCB_t *pimdmMapCbPtr, 
                      L7_uint32 intIfNum, L7_uint32 *nbrCount)
{
  L7_uint32 rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType, 
                                       intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
     PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert intIfNum (%d) to rtrIfNum \
                           for familyType %d",
                           intIfNum,pimdmMapCbPtr->familyType);
     return L7_FAILURE;
  }
  return (pimdmExtenApiNbrCountGet (pimdmMapCbPtr->cbHandle, 
                                                   rtrIfNum, nbrCount));
}

/*********************************************************************
* @purpose  Get the Pimdm Interface index of the specified interface
*
* @param    pimdmMapCbPtr     @b{(input)} Mapping Control Block.
* @param    nbrIpAddr         @b{(input)} Neighbor Ip address
* @param    pIntIfNum         @b{(output)} interface Num.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrIntfIndexGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                L7_inet_addr_t *nbrIpAddr, L7_uint32 *pIntIfNum)
{
  L7_uint32 rtrIfNum = L7_NULL;
  L7_RC_t   retCode = L7_FAILURE;

  retCode = pimdmExtenApiNbrIntfIndexGet (pimdmMapCbPtr->cbHandle,
                                                      nbrIpAddr, &rtrIfNum);
  if (mcastIpMapRtrIntfToIntIfNum(pimdmMapCbPtr->familyType,
                                       rtrIfNum, pIntIfNum) != L7_SUCCESS)
  {
     PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert rtrIfNum (%d) to intIfNum \
                           for familyType %d",
                           rtrIfNum,pimdmMapCbPtr->familyType);
      return L7_FAILURE;
  }
  return retCode;
}

/*********************************************************************
* @purpose  Get the PIMDM neighbor entry validity
*           on the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    nbrIpAddr        @b{(output)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrEntryGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                                L7_inet_addr_t *nbrIpAddr)
{
  return (pimdmExtenApiNbrEntryGet (pimdmMapCbPtr->cbHandle, nbrIpAddr));
}

/*********************************************************************
* @purpose  Get the PIMDM Next neighbor entry on the specified interface
*
* @param    pimdmMapCbPtr    @b{(inout)} Mapping Control Block.
* @param    ngbrIpAddr       @b{(inout)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrEntryNextGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                                     L7_inet_addr_t* nbrIpAddr)
{
  return (pimdmExtenApiNbrEntryNextGet (pimdmMapCbPtr->cbHandle, nbrIpAddr));
}

/*********************************************************************
* @purpose  Get the PIMDM neighbor entry validity on the specified interface
*
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    nbrIpAddr      @b{(input)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrEntryByIfIndexGet(pimdmMapCB_t *pimdmMapCbPtr,
                                          L7_uint32 intIfNum,
                                          L7_inet_addr_t* nbrIpAddr)
{
  return pimdmExtenApiNbrEntryByIfIndexGet (pimdmMapCbPtr->cbHandle,intIfNum, nbrIpAddr);
}

/*********************************************************************
* @purpose  Get the PIMDM Next neighbor entry by interface index on 
*           the specified interface
*
* @param    pimdmMapCbPtr    @b{(inout)} Mapping Control Block.
* @param    pimdmMapCbPtr    @b{(inout)} interface number.
* @param    ngbrIpAddr       @b{(inout)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrEntryByIfIndexNextGet(pimdmMapCB_t *pimdmMapCbPtr,
                                     L7_uint32 *outIntIfNum,
                                     L7_inet_addr_t* nbrIpAddr)
{
  return (pimdmExtenApiNbrEntryByIfIndexNextGet (pimdmMapCbPtr->cbHandle,outIntIfNum, nbrIpAddr));
}

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Expire time for the specified neighbour
*           of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    ngbrIpAddr       @b{(input)} Neighbor Ip address
* @param    nbrExpireTime    @b{(output)} Get the neighbor expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Time before this neighbor entry status expires
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrExpireTimeGet(pimdmMapCB_t *pimdmMapCbPtr, 
                           L7_inet_addr_t *nbrIpAddr, L7_uint32 *nbrExpireTime)
{
  return (pimdmExtenApiNbrExpireTimeGet (pimdmMapCbPtr->cbHandle, 
                                         nbrIpAddr, nbrExpireTime));
}

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Expire time for the specified neighbour
*           of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    ngbrIpAddr       @b{(input)} Neighbor Ip address
* @param    intIfNum         @b{(input)} internal interface number
* @param    nbrExpireTime    @b{(output)} Get the neighbor expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Time before this neighbor entry status expires
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrExpireTimeByIfIndexGet(pimdmMapCB_t *pimdmMapCbPtr, 
                           L7_inet_addr_t *nbrIpAddr,L7_uint32 intIfNum, L7_uint32 *nbrExpireTime)
{
  return (pimdmExtenApiNbrExpireTimeByIfIndexGet (pimdmMapCbPtr->cbHandle, 
                                         nbrIpAddr, intIfNum,nbrExpireTime));
}

/*********************************************************************
* @purpose  Get the assert metric
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
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
L7_RC_t pimdmMapExtenIpMRouteAssertMetricGet(pimdmMapCB_t *pimdmMapCbPtr, 
            L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
            L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetric)
{
  return (pimdmExtenApiIpMRouteAssertMetricGet (pimdmMapCbPtr->cbHandle, 
                                                ipMRouteGroup,
                                                ipMRouteSource,
                                                ipMRouteSourceMask,
                                                assertMetric));
}

/*********************************************************************
* @purpose  Get the assert metric preference
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
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
L7_RC_t pimdmMapExtenIpMRouteAssertMetricPrefGet(pimdmMapCB_t *pimdmMapCbPtr, 
     L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
     L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetricPref)
{
  return (pimdmExtenApiIpMRouteAssertMetricPrefGet (pimdmMapCbPtr->cbHandle, 
                                                    ipMRouteGroup,
                                                    ipMRouteSource,
                                                    ipMRouteSourceMask,
                                                    assertMetricPref));
}

/*********************************************************************
* @purpose  Get the RPT bit status
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
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
L7_RC_t pimdmMapExtenIpMRouteAssertRPTBitGet(pimdmMapCB_t *pimdmMapCbPtr, 
     L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
     L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *RPTBitStatus)
{
  return (pimdmExtenApiIpMRouteAssertRPTBitGet (pimdmMapCbPtr->cbHandle, 
                                                ipMRouteGroup,
                                                ipMRouteSource,
                                                ipMRouteSourceMask,
                                                RPTBitStatus));
}

/*********************************************************************
* @purpose  Get the Upstream assert timer status
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    upstreamAssertTimer  @b{(output)} Upstream assert timer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteUpstreamAssertTimerGet(pimdmMapCB_t *pimdmMapCbPtr,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *upstreamAssertTimer)
{
  return (pimdmExtenApiIpMRouteUpstreamAssertTimerGet 
          (pimdmMapCbPtr->cbHandle, ipMRouteGroup, ipMRouteSource,
          ipMRouteSourceMask, upstreamAssertTimer));
}

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast route table  entry
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
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
L7_RC_t pimdmMapExtenIpMRouteEntryGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask)
{
  return (pimdmExtenApiIpMRouteEntryGet (pimdmMapCbPtr->cbHandle, 
                                         ipMRouteGroup,
                                         ipMRouteSource,
                                         ipMRouteSourceMask));
}

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast route table Next entry
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup        @b{(inout)}  Multicast Group address
* @param    ipMRouteSource       @b{(inout)}  Source address
* @param    ipMRouteSourceMask   @b{(inout)}  Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteEntryNextGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource, 
    L7_inet_addr_t* ipMRouteSourceMask)
{
  return (pimdmExtenApiIpMRouteEntryNextGet (pimdmMapCbPtr->cbHandle, 
                                             ipMRouteGroup,
                                             ipMRouteSource,
                                             ipMRouteSourceMask));
}

/*********************************************************************
* @purpose  Get the PIMDM  Next Hop table entry
*
* @param    pimdmMapCbPtr              @b{(input)} Mapping Control Block.
* @param    ipMRouteNextHopGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteNextHopSource      @b{(input)}  Source address
* @param    ipMRouteNextHopSourceMask  @b{(input)}  Mask Address
* @param    ipMRouteNextHopRtrIfNum     @b{(input)} Next Hop RtrIfIndex
* @param    ipMRouteNextHopAddress     @b{(input)}  Next Hop Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteNextHopEntryGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *ipMRouteNextHopGroup, L7_inet_addr_t *ipMRouteNextHopSource,
  L7_inet_addr_t  *ipMRouteNextHopSourceMask, L7_uint32 ipMRouteNextHopRtrIfNum,
  L7_inet_addr_t *ipMRouteNextHopAddress)
{
  L7_uint32 rtrIfNum;

  if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType, 
                      ipMRouteNextHopRtrIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert intIfNum (%d) to rtrIfNum \
                           for familyType %d",
                           ipMRouteNextHopRtrIfNum,pimdmMapCbPtr->familyType);
    return L7_FAILURE;
  }
  return (pimdmExtenApiIpMRouteNextHopEntryGet (pimdmMapCbPtr->cbHandle, 
                                                ipMRouteNextHopGroup,
                                                ipMRouteNextHopSource,
                                                ipMRouteNextHopSourceMask,
                                                rtrIfNum,
                                                ipMRouteNextHopAddress));
}
/*********************************************************************
* @purpose  Get the PIMDM  Next Hop table Next entry
*
* @param  pimdmMapCbPtr              @b{(input)} Mapping Control Block.
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
L7_RC_t pimdmMapExtenIpMRouteNextHopEntryNextGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t* ipMRouteNextHopGroup, L7_inet_addr_t* ipMRouteNextHopSource, 
  L7_inet_addr_t* ipMRouteNextHopSourceMask, L7_uint32* ipMRouteNextHopRtrIfNum,
  L7_inet_addr_t* ipMRouteNextHopAddress)
{
  return (pimdmExtenApiIpMRouteNextHopEntryNextGet (pimdmMapCbPtr->cbHandle,
                                                    ipMRouteNextHopGroup,
                                                    ipMRouteNextHopSource,
                                                    ipMRouteNextHopSourceMask,
                                                    ipMRouteNextHopRtrIfNum,
                                                    ipMRouteNextHopAddress));
}

 /*********************************************************************
* @purpose  Get the Next Hop Prune reason on the specified interface
*
* @param    pimdmMapCbPtr              @b{(input)} Mapping Control Block.
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
* @comments Prune is generated in router durig assert/flood-prune and IGMP no member cases
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteNextHopPruneReasonGet(pimdmMapCB_t *pimdmMapCbPtr,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask,L7_uint32 ipMRouteNextHopRtrIfNum,
    L7_inet_addr_t *ipMRouteNextHopAddress,L7_uint32 *pruneReason)
{
  return (pimdmExtenApiIpMRouteNextHopPruneReasonGet 
          (pimdmMapCbPtr->cbHandle, ipMRouteGroup,
          ipMRouteSource, ipMRouteSourceMask, ipMRouteNextHopRtrIfNum,
          ipMRouteNextHopAddress, pruneReason));
}

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast Flags
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
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
L7_RC_t pimdmMapExtenIpMRouteFlagsGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
    L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32*  flags)
{
  return (pimdmExtenApiIpMRouteFlagsGet (pimdmMapCbPtr->cbHandle, 
               ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask,
                                         flags));
}

/*********************************************************************
* @purpose  Get the PIMDM  IP multicast table  Out going interface list
*
* @param    pimdmMapCbPtr       @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtrIfNum            @b{(input)}  Input RtrIfIndex
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
L7_RC_t pimdmMapExtenIpMRouteOIFNextGet
  (pimdmMapCB_t *pimdmMapCbPtr, L7_inet_addr_t *ipMRouteGroup,
   L7_inet_addr_t *ipMRouteSource, L7_inet_addr_t  *ipMRouteSourceMask, 
   L7_uint32* rtrIfNum, L7_uint32* state, L7_uint32* mode, L7_uint32*  upTime,
   L7_uint32* expiryTime)
{
  L7_uint32 tempRtrIfNum;

  if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType, 
                      *rtrIfNum, &tempRtrIfNum) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert intIfNum (%d) to rtrIfNum \
                           for familyType %d",
                           *rtrIfNum,pimdmMapCbPtr->familyType);
    return L7_FAILURE;
  }

  return (pimdmExtenApiIpMRouteOIFNextGet 
                              (pimdmMapCbPtr->cbHandle, ipMRouteGroup,
                               ipMRouteSource, ipMRouteSourceMask,
                               &tempRtrIfNum, state, mode, upTime, expiryTime));
}


/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    grpIpAddr       @b{(input)} Group IP Address
* @param    srcIpAddr       @b{(input)} source IP Address
* @param    srcMask         @b{(input)} source Mask
* @param    intIfNum        @b{(output)}Interface Index which is passed back 
*                                       to the calling function
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteIfIndexGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr,
  L7_inet_addr_t *srcMask, L7_uint32 *intIfNum)
{
  L7_RC_t rc =L7_FAILURE;
  L7_uint32 rtrIfNum =0;

  rc = pimdmExtenApiRouteIfIndexGet (pimdmMapCbPtr->cbHandle, grpIpAddr,
                                     srcIpAddr, srcMask, &rtrIfNum);
  if (rc == L7_SUCCESS)
  {
    if (rtrIfNum == 0)
    {
      *intIfNum = 0;
      return L7_SUCCESS;
    }

    if(mcastIpMapRtrIntfToIntIfNum(pimdmMapCbPtr->familyType,rtrIfNum,intIfNum) != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to convert rtrIfNum(%d) to intIfNum",rtrIfNum);
      return L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    grpIpAddr        @b{(input)} Group IP Address
* @param    srcIpAddr        @b{(input)} source IP Address
* @param    srcMask          @b{(input)} source Mask
* @param    rtUptime         @b{(output)}route up time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteUptimeGet(pimdmMapCB_t *pimdmMapCbPtr, 
 L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask,
 L7_uint32 *rtUpTime)
{
  return pimdmExtenApiRouteUptimeGet(pimdmMapCbPtr->cbHandle, grpIpAddr, 
                                            srcIpAddr, srcMask, rtUpTime);
}

/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    grpIpAddr      @b{(input)} Group IP Address
* @param    srcIpAddr      @b{(input)} source IP Address
* @param    srcMask        @b{(input)} source Mask
* @param    rtExpTime      @b{(output)}route expiry time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteExpiryTimeGet(pimdmMapCB_t *pimdmMapCbPtr, 
 L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask,
 L7_uint32 *rtExpTime)
{
  return pimdmExtenApiRouteExpiryTimeGet(pimdmMapCbPtr->cbHandle, grpIpAddr,
                                         srcIpAddr, srcMask, rtExpTime);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtAddr              @b{(output)} rt Address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRtAddrGet(pimdmMapCB_t *pimdmMapCbPtr, 
 L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
 L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rpfAddr)
{
  return pimdmExtenApiIpMRouteRtAddrGet(pimdmMapCbPtr->cbHandle, 
         ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, rpfAddr);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtMask              @b{(output)} rt mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRtMaskGet(pimdmMapCB_t *pimdmMapCbPtr, 
   L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
   L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtMask)
{
  return pimdmExtenApiIpMRouteRtMaskGet(pimdmMapCbPtr->cbHandle, 
         ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, rtMask);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtType              @b{(output)} rt Type.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRtTypeGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtType)
{
  return pimdmExtenApiIpMRouteRtTypeGet(pimdmMapCbPtr->cbHandle, 
      ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, rtType);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Proto.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtProto             @b{(output)} Route Protocol
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRtProtoGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtProto)
{
  return pimdmExtenApiIpMRouteRtProtoGet(pimdmMapCbPtr->cbHandle, 
      ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, rtProto);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Addr.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rpfAddr             @b{(output)} rpfAddr.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRpfAddrGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t *rpfAddr)
{
  return pimdmExtenApiIpMRouteRpfAddrGet(pimdmMapCbPtr->cbHandle, 
                 ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, rpfAddr);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    entryCount          @b{(output)}  entryCount
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteEntryCountGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                                         L7_uint32 *entryCount)
{
  return pimdmExtenApiIpMRouteEntryCountGet(pimdmMapCbPtr->cbHandle, 
                                                               entryCount);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count.
*
* @param    pimdmMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    heCount            @b{(output)}  entryCount
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteHighestEntryCountGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                                            L7_uint32 *heCount)
{
  return pimdmExtenApiIpMRouteHighestEntryCountGet(pimdmMapCbPtr->cbHandle, 
                                                   heCount);
}

/*********************************************************************
* @purpose  Delete Routes
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteDelete(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource)
{
  return pimdmExtenApiRouteDelete(pimdmMapCbPtr->cbHandle, 
                                  ipMRouteGroup, ipMRouteSource);
}

/*********************************************************************
* @purpose  Delete Routes
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteDeleteAll()
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  /* TODO ... To update the CB Ptr */

  return pimdmExtenApiRouteDeleteAll(pimdmMapCbPtr);
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Addr.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Source Mask
* @param    outIntf             @b{(input)}  Out interface.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpRouteOutIntfEntryNextGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *outIfNum)
{
  L7_uint32 rtrIfNum =0,intIfNum=0;
  L7_RC_t rc = L7_FAILURE;

  if(*outIfNum != 0)
  {
    if(mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType,*outIfNum,&rtrIfNum) != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert intIfNum (%d) to rtrIfNum \
                           for familyType %d",
                           *outIfNum,pimdmMapCbPtr->familyType);
      return L7_FAILURE;
    }

  }

  rc = pimdmExtenApiIpRouteOutIntfEntryNextGet(pimdmMapCbPtr->cbHandle, 
          ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, &rtrIfNum);

  if(rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if(mcastIpMapRtrIntfToIntIfNum(pimdmMapCbPtr->familyType,rtrIfNum,&intIfNum) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                          "Failed to convert rtrIfNum (%d) to intIfNum \
                           for familyType %d",
                           rtrIfNum,pimdmMapCbPtr->familyType);
    return L7_FAILURE;
  }

  *outIfNum = intIfNum;

  return rc;
}

