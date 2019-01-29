/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_map_api.c
*
* @purpose   MGMD API functions
*
* @component MGMD Mapping Layer
*
* @comments  none
*
* @create    02/08/2002
*
* @author    ramakrishna
* @end
*
**********************************************************************/

#include "l7_mgmdmap_include.h"
#include "l7_mcast_api.h"
#include "l7_mgmd_api.h"
#include "sdm_api.h"

/*---------------------------------------------------------------------
*                    API FUNCTIONS  -  GLOBAL CONFIG
*---------------------------------------------------------------------
*/

/********************************************************************
* @purpose  Get the MGMD administrative mode
*
* @param    familyType  @b{(input)} Address Family type
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapAdminModeGet(L7_uchar8 familyType, L7_uint32 *mode)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapCbPtr->pMgmdMapCfgData == L7_NULL)
    {
        *mode = L7_NULL;                /* neither enabled nor diabled */
        MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"MGMD_MAP:No configuration data\n");
        return L7_FAILURE;
    }
    *mode = mgmdMapCbPtr->pMgmdMapCfgData->adminMode;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the MGMD administrative mode
*
* @param    familyType  @b{(input)} Address Family type
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapAdminModeSet(L7_uchar8 familyType,L7_uint32 mode)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    /* fails if MGMD component not started by configurator */
    if (mgmdMapCbPtr->pMgmdMapCfgData == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD_MAP:No configuration data\n");
        return L7_FAILURE;
    }

    /* if current admin mode already established, return */
    if (mode == mgmdMapCbPtr->pMgmdMapCfgData->adminMode)
    {
        MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS,"Mode Already Set \n");
        return L7_SUCCESS;
    }

    if (mode == L7_ENABLE)
    {
      if(mgmdMapMemoryInit(mgmdMapCbPtr) != L7_SUCCESS)
      {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Memory Initialization Failed for addrFamily - %d", familyType);
        return L7_FAILURE;
      }
    }

    mgmdMapCbPtr->pMgmdMapCfgData->adminMode = mode;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_ADMIN_MODE_SET, mode, 0, MGMD_INTERFACE_MODE_NULL)
                          != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n", MGMD_ADMIN_MODE_SET);

      if (mode == L7_ENABLE)
      {
        mgmdMapMemoryDeInit (mgmdMapCbPtr);
        mgmdMapCbPtr->pMgmdMapCfgData->adminMode = L7_DISABLE;
        mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;
      }
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the administrative mode of an MGMD interface
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
L7_RC_t mgmdMapInterfaceModeGet(L7_uchar8 familyType,
                                L7_uint32 intIfNum, L7_uint32* mode)
{
    mgmdIntfCfgData_t *pCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
    L7_RC_t rc = L7_FAILURE;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &pCfg) != L7_TRUE)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
               is valid for MGMD configuration\n");
      return L7_ERROR;
    }
    if (pCfg->interfaceMode == MGMD_ROUTER_INTERFACE)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS,"Code Flow: SUCCESS\n");
      *mode = pCfg->adminMode;
      rc = L7_SUCCESS;
    }
    else
    {
      *mode = L7_DISABLE;
      rc = L7_SUCCESS;
    }

    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "The interface is not a router interface\n");
    return rc;
}

/*********************************************************************
* @purpose  Get the administrative mode of an MGMD-PROXY interface
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
L7_RC_t mgmdMapProxyInterfaceModeGet(L7_uchar8 familyType,
                                     L7_uint32 intIfNum, L7_uint32* mode)
{
  mgmdIntfCfgData_t *pCfg = L7_NULLPTR;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }

  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                &pCfg) != L7_TRUE)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
            is valid for MGMD configuration\n");
    return L7_ERROR;
  }
  if (pCfg->interfaceMode == MGMD_PROXY_INTERFACE)
  {
    *mode = pCfg->adminMode;
    rc = L7_SUCCESS;
  }
  else
  {
    *mode = L7_DISABLE;
    rc = L7_SUCCESS;
  }

  MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map: The interface is not a proxy interface\n");
  return rc;
}

/*********************************************************************
* @purpose  Set the administrative mode of an MGMD interface
*
* @param    familyType      @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    mode            @b{(input)} L7_ENABLE or L7_DISABLE
* @param    proxy_or_router @b{(input)} proxy or router
* @param    errCode         @b{(input)} error code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceModeSet(L7_uchar8 familyType,
                                L7_uint32 intIfNum,
                                L7_uint32 mode,
                                L7_MGMD_INTF_MODE_t proxy_or_router,
                                L7_MGMD_ERROR_CODE_t *errCode)
{
  mgmdIntfCfgData_t *intfCfg = L7_NULLPTR;
  L7_uint32          currentMcastProtocol;
  mgmdMapCB_t       *mgmdMapCbPtr = L7_NULLPTR;
  L7_uint32          mcastMode = L7_NULL;
  L7_RC_t            retVal = L7_SUCCESS;


  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* These pre-conditions has to be present as the support for
     unnumbered interfaces is not defined for MGMD */
  if (mcastIpMapIsIntfUnnumbered(familyType,intIfNum) == L7_TRUE)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "\n MGMD cannot be set on unnumbered interface intIfNum(%d)", intIfNum);
    return L7_FAILURE;
  }

  *errCode = L7_MGMD_ERR_NONE;
  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid Mode\n");
    *errCode = L7_MGMD_ERR_INVALID_INPUT;
    return L7_FAILURE;
  }
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                &intfCfg) != L7_TRUE)
  {
    *errCode = L7_MGMD_ERR_REQUEST_FAILED;  /* ERROR: Operation failed. */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface %d is valid for MGMD configuration\n", intIfNum);
    return L7_ERROR;
  }

  if (intfCfg == L7_NULL)
  {
    *errCode = L7_MGMD_ERR_REQUEST_FAILED;  /* ERROR: Operation failed. */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get MGMD Interface config structure\n");
    return L7_FAILURE;
  }


  if (mgmdMapCbPtr->pMgmdMapCfgData == L7_NULL)
  {
    *errCode = L7_MGMD_ERR_REQUEST_FAILED;  /* ERROR: Operation failed. */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IGMAP_MAP:No configuration data\n");
    return L7_FAILURE;
  }

  switch(proxy_or_router)
  {
    case MGMD_PROXY_INTERFACE:
    {
      /*
       * Proxy interface
       */
      if (mode == L7_ENABLE)
      {
         if (intfCfg->interfaceMode == MGMD_ROUTER_INTERFACE)
         {
           *errCode = L7_MGMD_ERR_MGMD_INTF;
                             /*FAILED: MGMD configured on this interface.*/
           MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "This is a mgmd router interface\n");
           return L7_FAILURE;
         }
         /* if the mode is same as already set then just return */
         if (mode == intfCfg->adminMode)
         {
           /*  multicast forwarding is enabled */
           mcastMapMcastAdminModeGet(&mcastMode);
  
           if(mcastMode == L7_DISABLE)
           {
             *errCode = L7_MGMD_ERR_MCAST_FWD_DISABLED;
                              /* FAILED: Multicast Forwarding not enabled. */
             MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "Multicast forwarding not enabled\n");
             return L7_FAILURE;
           }
  
           /* check whether routing is enabled */
           if (mcastIpMapRtrAdminModeGet(mgmdMapCbPtr->familyType) == L7_DISABLE)
           {
             *errCode = L7_MGMD_ERR_ROUTING_NOT_ENABLED;
                              /*FAILED: Routing not enabled on this interface.*/
             MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Routing not enabled on this surface\n");
             return L7_FAILURE;
           }
  
           /* Check if the global MGMD status is enabled.*/
           if (mgmdMapCbPtr->pMgmdMapCfgData->adminMode == L7_DISABLE)
           {
             *errCode = L7_MGMD_ERR_MGMD_DISABLED_ON_SYSTEM;
                              /*FAILED: Routing not enabled on this interface.*/
             MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Global MGMD mode is not enabled.\n");
             return L7_FAILURE;
           }
           *errCode = L7_MGMD_PROXY_ALREADY_CONFIGURED;
           MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Proxy is already running\n");
           return L7_SUCCESS;
         }

         mcastMapIpCurrentMcastProtocolGet(familyType, &currentMcastProtocol);
         /* check whether any other multicast protocol is running */
         if (currentMcastProtocol != L7_MCAST_IANA_MROUTE_UNASSIGNED)
         {
           if (currentMcastProtocol == L7_MCAST_IANA_MROUTE_IGMP_PROXY)
           {
             *errCode = L7_MGMD_PROXY_ALREADY_CONFIGURED;
                   /* FAILED: MGMD Proxy already configured on **
                   ** some other interface.                    */
           }
           else
           {
             *errCode = L7_MGMD_ERR_OTHER_MCAST_ROUTING_PROTOCOL_CONFIGURED;
                   /* FAILED: Some another Multicast Routing **
                   ** Protocol already configured.           */
           }
           MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Proxy either configured or some other multicast \
                   protocol already running\n");
           return L7_FAILURE;
         }

         /* Initialize the Memory for MGMD */
         if (mgmdProxyMemoryInit (familyType) != L7_SUCCESS)
         {
           MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Proxy: Vendor Layer Dynamic Memory Init Failed for family = %d",
                             familyType);
           return L7_FAILURE;
         }

         /* set the config value */
         intfCfg->interfaceMode = MGMD_PROXY_INTERFACE;
         intfCfg->adminMode = L7_ENABLE;
         mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;
         /* set the current multicast protocol is MGMD PROXY */
         mcastMapIpCurrentMcastProtocolSet(familyType,
               L7_MCAST_IANA_MROUTE_IGMP_PROXY);

         if(sdmTemplateSupportsIpv6())
         {
           if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_INTF_MODE_SET, mode,
                                   intIfNum, MGMD_PROXY_INTERFACE) != L7_SUCCESS)
           {
             MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n",
                       MGMD_INTF_MODE_SET);
             *errCode = L7_MGMD_ERR_REQUEST_FAILED;/*ERROR: Operation failed.*/
             mgmdProxyMemoryDeInit (familyType);
            /* set the default config value */
            intfCfg->interfaceMode = MGMD_INTERFACE_MODE_NULL;
            intfCfg->adminMode = L7_DISABLE;
            mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;
            mcastMapIpCurrentMcastProtocolSet (familyType,
                                                   L7_MCAST_IANA_MROUTE_UNASSIGNED);
            return L7_FAILURE;
           }
         }
         else
         {
           return L7_SUCCESS;
         }
          /*  multicast forwarding is enabled */
         mcastMapMcastAdminModeGet(&mcastMode);

         if(mcastMode == L7_DISABLE)
         {
           *errCode = L7_MGMD_ERR_MCAST_FWD_DISABLED;
                      /* FAILED: Multicast Forwarding not enabled. */
           MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "Multicast forwarding not enabled\n");
           return L7_FAILURE;
         }

         /* check whether routing is enabled */
         if (mcastIpMapRtrAdminModeGet(mgmdMapCbPtr->familyType) == L7_DISABLE)
         {
           *errCode = L7_MGMD_ERR_ROUTING_NOT_ENABLED;
                            /*FAILED: Routing not enabled on this interface.*/
           MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Routing not enabled on this surface\n");
           return L7_FAILURE;
         }

         /* Check if the global MGMD status is enabled.*/
         if (mgmdMapCbPtr->pMgmdMapCfgData->adminMode == L7_DISABLE)
         {
           *errCode = L7_MGMD_ERR_MGMD_DISABLED_ON_SYSTEM;
                            /*FAILED: Routing not enabled on this interface.*/
           MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Global MGMD mode is not enabled.\n");
           return L7_FAILURE;
         }
      }
      else if (mode == L7_DISABLE)
      {
         if(intfCfg->interfaceMode == MGMD_PROXY_INTERFACE)
         {
           /* if the mode is same as already set just return */
           if (mode == intfCfg->adminMode)
           {
             return L7_SUCCESS;
           }
            /* set the config value */
           intfCfg->interfaceMode = MGMD_INTERFACE_MODE_NULL;
           intfCfg->adminMode = L7_DISABLE;
           mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;


           /* Reset the current multicast protocol to NONE. */
           mcastMapIpCurrentMcastProtocolSet(familyType, L7_MCAST_IANA_MROUTE_UNASSIGNED);

           if(sdmTemplateSupportsIpv6())
           {
             if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_INTF_MODE_SET, mode,
                                     intIfNum, MGMD_PROXY_INTERFACE) != L7_SUCCESS)
             {
               MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n",
                         MGMD_INTF_MODE_SET);
               *errCode = L7_MGMD_ERR_REQUEST_FAILED;/*ERROR: Operation failed.*/
             }
           }
           else
           {
             return L7_SUCCESS;
           }
         }
         else
         {
           MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "interfaceMode is not MGMD proxy interface\n");
           *errCode = L7_MGMD_ERR_NOT_PROXY_INTF;
                     /*Given interface is not an MGMD-Proxy interface.*/
         }
      }
    }
    break;

    case MGMD_ROUTER_INTERFACE:
    {
      /*
       * Router interface
       */
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nSwitch Case:Mgmd Router I/f\n");
      if (mode == L7_ENABLE)
      {
        if (intfCfg->interfaceMode == MGMD_PROXY_INTERFACE)
        {
          *errCode = L7_MGMD_ERR_PROXY_INTF;
          /* FAILED: MGMD Proxy configured on this interface.*/
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Proxy configured on this surface\n");
          return L7_FAILURE;
        }
        /* if the mode is same as already set just return */
        if (mode == intfCfg->adminMode)
        {
          MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nmode already configured\n");
          return L7_SUCCESS;
        }

        /* set the config value */
        intfCfg->interfaceMode = MGMD_ROUTER_INTERFACE;
        intfCfg->adminMode = L7_ENABLE;
        mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;
        MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nmode stored\n");

        if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_INTF_MODE_SET, mode,
                                intIfNum, MGMD_ROUTER_INTERFACE) != L7_SUCCESS)
        {
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n",
                    MGMD_INTF_MODE_SET);
          *errCode = L7_MGMD_ERR_REQUEST_FAILED;/*ERROR: Operation failed.*/
        }
      }
      else if (mode == L7_DISABLE)
      {
        if (intfCfg->interfaceMode == MGMD_ROUTER_INTERFACE)
        {
          /* if the mode is same as already set just return */
          if (mode == intfCfg->adminMode)
          {
            MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nAlready Disabled\n");
            return L7_SUCCESS;
          }
          /* set the config value */
          intfCfg->interfaceMode = MGMD_INTERFACE_MODE_NULL;
          intfCfg->adminMode = L7_DISABLE;
          mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

          if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_INTF_MODE_SET, mode,
                                  intIfNum, MGMD_ROUTER_INTERFACE) != L7_SUCCESS)
          {
            MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n",
                      MGMD_INTF_MODE_SET);
            *errCode = L7_MGMD_ERR_REQUEST_FAILED;/*ERROR: Operation failed.*/
          }

        }
        else
        {
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "interfaceMode is not MGMD router interface\n");
          *errCode = L7_MGMD_ERR_NOT_MGMD_INTF;
                                 /*Given interface is not an MGMD interface.*/
        }
      }
    }
    break;

    default:
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nERROR\n");
      *errCode = L7_MGMD_ERR_INVALID_INPUT;
      break;
  }/*End-of-Switch*/

  return retVal;
}/*End-of-Function*/

/*********************************************************************
* @purpose  Sets whether the optional Router Alert field is required.
*
* @param    checkRtrAlert       @b{(input)}   True if Router Alert required
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapRouterAlertMandatorySet(L7_uchar8 familyType, L7_BOOL checkRtrAlert)
{
  mgmdMapCB_t       *mgmdMapCbPtr = L7_NULLPTR;

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  mgmdMapCbPtr->pMgmdMapCfgData->checkRtrAlert = checkRtrAlert;
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_ROUTER_ALERT_CHECK_SET,
                          checkRtrAlert, 0, MGMD_INTERFACE_MODE_NULL)
                          != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
              MGMD_ROUTER_ALERT_CHECK_SET);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Indicates whether the optional Router Alert field is required.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapRouterAlertMandatoryGet(L7_uchar8 familyType, L7_BOOL *checkRtrAlert)
{
  mgmdMapCB_t       *mgmdMapCbPtr = L7_NULLPTR;

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }

  *checkRtrAlert = mgmdMapCbPtr->pMgmdMapCfgData->checkRtrAlert;
  return( L7_SUCCESS );
}

/*********************************************************************
* @purpose  Get the interface query interval for the specific Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(output)} Interface Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQueryIntervalGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *interval)
{
    mgmdIntfCfgData_t *intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (interval == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure \
        for this interface\n");
        return L7_FAILURE;
    }


    *interval = intfCfg->queryInterval;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the query interval value for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(input)} Query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQueryIntervalSet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_uint32 interval)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (interval < L7_IGMP_QUERY_INTERVAL_MIN ||
        interval > L7_IGMP_QUERY_INTERVAL_MAX)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "The query is beyond range\n");
        return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }

    /* Per RFC2236, the Query Response Interval must be less than **
    ** the Query Interval.                                        */
    /* For IPv4 , The Query Resp Interval is in tenths of seconds,**
    ** while the Query Interval is in seconds                     */
    /* For IPv6 , The Query Resp Interval is in milli - seconds,  **
    ** while the Query Interval is in seconds                     */
    if (((familyType == L7_AF_INET) && ((intfCfg->responseInterval / 10) >= interval)) ||
        ((familyType == L7_AF_INET6) && ((intfCfg->responseInterval / 1000) >= interval)))
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Query Response Interval is not less than the Query interval\n");
      return L7_FAILURE;
    }

    intfCfg->queryInterval = interval;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_QUERY_INTERVAL_SET,
                          interval, intIfNum, MGMD_INTERFACE_MODE_NULL)
      != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
                   MGMD_QUERY_INTERVAL_SET);
  }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the MGMD Version configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    version     @b{(output)} MGMD Version on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceVersionGet(L7_uchar8 familyType,
                                   L7_uint32 intIfNum,
                                   L7_uint32* version)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (version == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration \
                     structure for interface(%d)\n", intIfNum);
    return L7_FAILURE;
    }
    *version = intfCfg->version;

    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Set the MGMD Version for this interface
*
* @param    familyType @b{(input)} Address Family type
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    version    @b{(input)} MGMD Version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceVersionSet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                   L7_uint32 version)
{
  mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (version < L7_IGMP_VERSION_MIN || version > L7_IGMP_VERSION_MAX)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD version number out of range\n");
    return L7_FAILURE;
  }

  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                &intfCfg) != L7_TRUE)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
    return L7_ERROR;
  }

  if (intfCfg == L7_NULL)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
    return L7_FAILURE;
  }
  if (intfCfg->version == version)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Same version is already configured on this interface\n");
    return L7_SUCCESS;
  }
  intfCfg->version = version;
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_VERSION_SET, version,
                          intIfNum, intfCfg->interfaceMode) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n",
              MGMD_INTF_MODE_SET);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Query Max Response Time configured for the Interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    maxRespTime  @b{(output)} Max Response Time on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQueryMaxResponseTimeGet(L7_uchar8 familyType,
               L7_uint32 intIfNum, L7_uint32* maxRespTime)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (maxRespTime == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }
    *maxRespTime = intfCfg->responseInterval;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Query Max Response Time for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    maxRespTime @b{(input)} Query Max Response Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQueryMaxResponseTimeSet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum, L7_uint32 maxRespTime)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface  \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }

    /* Per RFC2236, the Query Response Interval must be less than **
    ** the Query Interval.                                        */
    /* For IPv4 , The Query Resp Interval is in tenths of seconds,**
    ** while the Query Interval is in seconds                     */
    /* For IPv6 , The Query Resp Interval is in milli - seconds,  **
    ** while the Query Interval is in seconds                     */
    if (((familyType == L7_AF_INET) && ((intfCfg->queryInterval * 10) <= maxRespTime)) ||
        ((familyType == L7_AF_INET6) && ((intfCfg->queryInterval * 1000) <= maxRespTime)))
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Query Response Interval is not less than the Query interval\n");
      return L7_FAILURE;
    }

    intfCfg->responseInterval = maxRespTime;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_QUERY_MAX_RESPONSE_TIME_SET,
                            maxRespTime, intIfNum, MGMD_INTERFACE_MODE_NULL)
                            != L7_SUCCESS)
    {
       MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
                      MGMD_QUERY_MAX_RESPONSE_TIME_SET);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Robustness variable configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    robustness  @b{(output)} Robustness variable on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceRobustnessGet(L7_uchar8 familyType, L7_uint32 intIfNum,
                                      L7_uint32* robustness)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (robustness == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_ERROR;
    }
    *robustness = intfCfg->robustness;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Robustness variable for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    robustness  @b{(input)} Robustness variable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceRobustnessSet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                      L7_uint32 robustness)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }
    intfCfg->robustness = robustness;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_ROBUSTNESS_SET,
                            robustness, intIfNum, MGMD_INTERFACE_MODE_NULL)
                            != L7_SUCCESS)
    {
       MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
                      MGMD_ROBUSTNESS_SET);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Last Member Query Interval configured for the Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    interval    @b{(output)} Last Member Query Interval on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceLastMembQueryIntervalGet(L7_uchar8 familyType,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32* interval)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (interval == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }
    *interval = intfCfg->lastMemQueryInterval;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Last Member Query Interval for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(input)} Last Member Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceLastMembQueryIntervalSet(L7_uchar8 familyType,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32 interval)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }

    intfCfg->lastMemQueryInterval = interval;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_LASTMEMBER_QUERY_INTERVAL_SET,
                            interval, intIfNum, MGMD_INTERFACE_MODE_NULL)
                            != L7_SUCCESS)
    {
       MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
                      MGMD_LASTMEMBER_QUERY_INTERVAL_SET);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Last Member Query Count configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    queryCount  @b{(output)} Last Member Query Count on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceLastMembQueryCountGet(L7_uchar8 familyType,
                                     L7_uint32 intIfNum,
                                     L7_uint32* queryCount)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (queryCount == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }
    *queryCount = intfCfg->lastMemQueryCount;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Last Member Query Count for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    queryCount  @b{(input)} Last Member Query Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceLastMembQueryCountSet(L7_uchar8 familyType,
                                     L7_uint32 intIfNum,
                                     L7_uint32 queryCount)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }

    intfCfg->lastMemQueryCount = queryCount;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_LASTMEMBER_QUERY_COUNT_SET,
                            queryCount, intIfNum, MGMD_INTERFACE_MODE_NULL)
                            != L7_SUCCESS)
    {
       MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
                      MGMD_LASTMEMBER_QUERY_COUNT_SET);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Startup Query Interval configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(output)} Startup Query Interval on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceStartupQueryIntervalGet(L7_uchar8 familyType,
                                                L7_uint32 intIfNum,
                                                L7_uint32* interval)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (interval == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }

    *interval = intfCfg->startupQueryInterval;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Startup Query Interval for this interface
*
* @param    familyType@b{(input)} Address Family type
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    interval  @b{(input)} Startup  Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceStartupQueryIntervalSet(L7_uchar8 familyType,
                                                L7_uint32 intIfNum,
                                                L7_uint32 interval)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }

    intfCfg->startupQueryInterval = interval;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_STARTUP_QUERY_INTERVAL_SET,
                            interval, intIfNum, MGMD_INTERFACE_MODE_NULL)
                            != L7_SUCCESS)
    {
       MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
                      MGMD_STARTUP_QUERY_INTERVAL_SET);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Startup Query Count configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    queryCount  @b{(output)} Startup Query Count on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceStartupQueryCountGet(L7_uchar8 familyType,
                                             L7_uint32 intIfNum,
                                             L7_uint32* queryCount)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (queryCount == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }
    *queryCount = intfCfg->startupQueryCount;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Startup Query Count for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    queryCount  @b{(input)} Startup Query Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceStartupQueryCountSet(L7_uchar8 familyType,
                                             L7_uint32 intIfNum,
                                             L7_uint32 queryCount)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }

    intfCfg->startupQueryCount = queryCount;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_STARTUP_QUERY_COUNT_SET,
                            queryCount, intIfNum, MGMD_INTERFACE_MODE_NULL)
                            != L7_SUCCESS)
    {
       MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
                      MGMD_STARTUP_QUERY_COUNT_SET);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Interface Querier on the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    querierIP   @b{(output)} Querier Address on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQuerierGet(L7_uchar8 familyType,
                                   L7_uint32 intIfNum,
                                   L7_inet_addr_t* querierIP)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenIntfQuerierGet(mgmdMapCbPtr, intIfNum,
                                      querierIP);
}

/*********************************************************************
* @purpose  Get the Interface Querier UpTime on the Interface
*
* @param    familyType      @b{(input)} Address Family type
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    querierUpTime   @b{(output)} Querier UpTime on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQuerierUpTimeGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_uint32* querierUpTime)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenIntfQuerierUpTimeGet(mgmdMapCbPtr, intIfNum,
                                            querierUpTime);
}

/*********************************************************************
* @purpose  Get the Interface Querier Expiry Time on the Interface
*
* @param    familyType          @b{(input)} Address Family type
* @param    intIfNum            @b{(input)}  Internal Interface Number
* @param    querierExpiryTime   @b{(output)} Querier ExpiryTime on
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQuerierExpiryTimeGet(L7_uchar8 familyType,
                                             L7_uint32 intIfNum,
                                             L7_uint32* querierExpiryTime)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenIntfQuerierExpiryTimeGet(mgmdMapCbPtr, intIfNum,
                                                querierExpiryTime);
}

/*********************************************************************
* @purpose  Get the Interface Wrong Version Queries  on the Interface
*
* @param    familyType        @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  Internal Interface Number
* @param    wrongVerQueries   @b{(output)} Wrong Version Queries on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceWrongVersionQueriesGet(L7_uchar8 familyType,
                                               L7_uint32 intIfNum,
                                               L7_uint32* wrongVerQueries)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenIntfWrongVersionQueriesGet(mgmdMapCbPtr, intIfNum,
                                                  wrongVerQueries);
}

/*********************************************************************
* @purpose  Get the Number Of Joins  on the Interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    intfJoins     @b{(output)} Num of Joins on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceJoinsGet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum,
                                 L7_uint32* intfJoins)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenIntfJoinsGet(mgmdMapCbPtr, intIfNum, intfJoins);
}

/*********************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    familyType          @b{(input)} Address Family type
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    numOfGroups         @b{(output)}   Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceGroupsGet(L7_uchar8 familyType,
                                  L7_uint32 intIfNum,
                                  L7_uint32* numOfGroups)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }


    return mgmdMapExtenIntfGroupsGet(mgmdMapCbPtr, intIfNum,
                                     numOfGroups);
}

/*********************************************************************
* @purpose  Gets the Querier Status for the specified interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    status      @b{(output)}  Querier  Status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQuerierStatusGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *status)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenIntfQuerierStatusGet(mgmdMapCbPtr, intIfNum,
                                            status);
}

/*********************************************************************
* @purpose  Gets the Next Valid Interface for MGMD
*
* @param    familyType  @b{(input)} Address Family type
* @param    pIntIfNum   @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceEntryNextGet(L7_uchar8 familyType,
                                     L7_uint32 *pIntIfNum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenIntfEntryNextGet(mgmdMapCbPtr, pIntIfNum);
}

/*******************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*       received for the specified group address on the specified interface
*
* @param    familyType        @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    ipAddr            @b{(output)} last reporter ip
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
****************************************************************************/
L7_RC_t mgmdMapCacheLastReporterGet(L7_uchar8 familyType,
                                    L7_inet_addr_t *multipaddr,
                                    L7_uint32 intIfNum,
                                    L7_inet_addr_t *ipAddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheLastReporterGet(mgmdMapCbPtr, multipaddr,
                                            intIfNum, ipAddr);
}

/****************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the
*           Cache Table for the specified group address & the specified interface
* @param    familyType          @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    upTime            @b{(output)} Up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheUpTimeGet(L7_uchar8 familyType,
                              L7_inet_addr_t *multipaddr,
                              L7_uint32 intIfNum,
                              L7_uint32 *upTime)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheUpTimeGet(mgmdMapCbPtr, multipaddr,
                                      intIfNum, upTime);
}

/**************************************************************************************
* @purpose  Gets the  Expiry time before the specified entry in Cache
*           Table will be aged out
*
* @param    familyType        @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    expTime           @b{(output)} Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheExpiryTimeGet(L7_uchar8 familyType,
                                  L7_inet_addr_t *multipaddr,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *expTime)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheExpiryTimeGet(mgmdMapCbPtr, multipaddr,
                                          intIfNum, expTime);
}

/**************************************************************************************
* @purpose  Gets the  time remaining until the router assumes there are no longer
*           any MGMD version 1 Hosts on the specified interface
*
* @param    familyType        @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    version1Time      @b{(output)} version1 Host Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheVersion1HostTimerGet(L7_uchar8 familyType,
                                         L7_inet_addr_t *multipaddr,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *version1Time)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheVersion1HostTimerGet(mgmdMapCbPtr, multipaddr,
                                                 intIfNum, version1Time);
}

/**************************************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    familyType        @b{(input)}  Address Family type
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheEntryGet (L7_uchar8 familyType,
                              L7_inet_addr_t *multipaddr,
                              L7_uint32 intIfNum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheEntryGet (mgmdMapCbPtr, multipaddr,intIfNum);
}

/**************************************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given multi ipaddress & an interface number.
* @
*
* @param    familyType        @b{(input)}  Address Family type
* @param    pIntIfNum         @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum = 0 & multipaddr = 0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheEntryNextGet (L7_uchar8 familyType,
                                  L7_inet_addr_t *multipaddr,
                                  L7_uint32 *pIntIfNum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheEntryNextGet(mgmdMapCbPtr, multipaddr,pIntIfNum);
}


/***************************************************************************
* @purpose  Gets the time remaining until the router assumes there are no
*           longer any MGMD version 2 Hosts for the specified group on the
*           specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    version2Time      @b{(output)} version1 Host Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheVersion2HostTimerGet(L7_uchar8 familyType,
                                         L7_inet_addr_t *groupAddr,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *version2Time)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheVersion2HostTimerGet(mgmdMapCbPtr, groupAddr,
                                                 intIfNum, version2Time);
}

/***************************************************************************
* @purpose  Gets the compatibility mode (v1, v2 or v3) for the specified
*           group on the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupCompatMode   @b{(output)} group compatibility mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheGroupCompatModeGet(L7_uchar8 familyType,
                                       L7_inet_addr_t *groupAddr,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *groupCompatMode)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheGroupCompatModeGet(mgmdMapCbPtr, groupAddr,
                                               intIfNum, groupCompatMode);
}

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
*           group on the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    sourceFilterMode  @b{(output)} source filter mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheGroupSourceFilterModeGet(L7_uchar8 familyType,
                                             L7_inet_addr_t *groupAddr,
                                             L7_uint32 intIfNum,
                                             L7_uint32 *sourceFilterMode)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheGroupSourceFilterModeGet(mgmdMapCbPtr, groupAddr,
                                                   intIfNum, sourceFilterMode);
}

/*****************************************************************************
* @purpose  To validate if the specified interface contains a cache entry for
*           the specified group or not
*
* @param    familyType        @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheIntfEntryGet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                 L7_inet_addr_t *groupAddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheIntfEntryGet(mgmdMapCbPtr, intIfNum,
                                         groupAddr);
}

/*****************************************************************************
* @purpose  Get the Next Entry in Cache Table for the specified group and
*           interface
*
* @param    familyType         @b{(input)} Address Family type
* @param    pIntIfNum           @b{(input)}  internal Interface number
* @param    ipaddr             @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum=0 & groupAddr=0, then return the first valid
*           entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheIntfEntryNextGet(L7_uchar8 familyType,
         L7_uint32 *pIntIfNum, L7_inet_addr_t *ipaddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenCacheIntfEntryNextGet(mgmdMapCbPtr, pIntIfNum,
                                             ipaddr);
}

/*****************************************************************************
* @purpose  To validate if the specified source address exists for the specified
*           group address and interface or not
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapSrcListEntryGet(L7_uchar8 familyType,
                               L7_inet_addr_t *groupAddr,
                               L7_uint32 intIfNum,
                               L7_inet_addr_t *hostAddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenSrcListEntryGet(mgmdMapCbPtr, groupAddr,
                                       intIfNum, hostAddr);
}

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
*           interface and source address
*
* @param    familyType         @b{(input)}  Address Family type
* @param    groupAddr          @b{(input)}  multicast group ip address
* @param    pIntIfNum          @b{(input)}  internal Interface number
* @param    hostAddr           @b{(input)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum=0, groupAddr=0 & hostAddr=0,
*           then return the first valid source list entry
*
* @end
*********************************************************************/
L7_RC_t mgmdMapSrcListEntryNextGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *groupAddr,
                                   L7_uint32 *pIntIfNum,
                                   L7_inet_addr_t *hostAddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenSrcListEntryNextGet(mgmdMapCbPtr, groupAddr,
                                              pIntIfNum, hostAddr);
}

/*****************************************************************************
* @purpose  Get the Next  group address and interface
*
* @param    familyType         @b{(input)}  Address Family type
* @param    groupAddr          @b{(input)}  multicast group ip address
* @param    pIntIfNum          @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum=0  and groupAddr=0
*           then return the first valid entry
*
* @end
*********************************************************************/
L7_RC_t mgmdMapGrpIntfEntryNextGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *groupAddr,
                                   L7_uint32 *pIntIfNum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenGrpIntfEntryNextGet(mgmdMapCbPtr, groupAddr,
                                              pIntIfNum);
}

/*************************************************************************
* @purpose  Get the amount of time until the specified Source Entry is aged out
*
* @param    familyType          @b{(input)}  Address Family type
* @param    groupAddr           @b{(input)}  multicast group ip address
* @param    intIfNum            @b{(input)}  internal Interface number
* @param    hostAddr            @b{(input)}  source address
* @param    srcExpiryTime       @b{(output)} source expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapSrcExpiryTimeGet(L7_uchar8 familyType,L7_inet_addr_t *groupAddr,
                                L7_uint32 intIfNum, L7_inet_addr_t *hostAddr,
                                L7_uint32 *srcExpiryTime)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenSrcExpiryTimeGet(mgmdMapCbPtr, groupAddr, intIfNum,
                                        hostAddr, srcExpiryTime);
}

/**************************************************************************
* @purpose  Get the number of source records for the specified group
*           and interface
*
* @param    familyType         @b{(input)}  Address Family type
* @param    groupAddr          @b{(input)}  multicast group ip address
* @param    intIfNum           @b{(input)}  internal Interface number
* @param    srcCount           @b{(output)} number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapSrcCountGet(L7_uchar8 familyType,
                           L7_inet_addr_t *groupAddr,
                           L7_uint32 intIfNum,
                           L7_uint32 *srcCount)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenSrcCountGet(mgmdMapCbPtr, groupAddr,
                                   intIfNum, srcCount);
}

/*********************************************************************
* @purpose  Determine if the MGMD  has been initialized
*
* @param    familyType    @b{(input)} Address Family type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdMapMgmdIsOperational(L7_uchar8 familyType)
{
  mgmdMapCB_t       *mgmdMapCbPtr = L7_NULLPTR;

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Failed to get control block\n");
    return L7_FALSE;
  }
  return mgmdMapMgmdInitialized(mgmdMapCbPtr);
}
/*********************************************************************
* @purpose  Determine if the MGMD interface has been initialized
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdIntfInitialized(L7_uchar8 familyType, L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum = L7_NULL;
  mgmdIntfCfgData_t* intfCfg;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }


  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                &intfCfg) != L7_TRUE)
  {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
               is valid for MGMD configuration\n");
      return L7_FAILURE;
  }
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
                 "Code Flow.\n", __FUNCTION__, __LINE__);
  if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Failed to convert internal interface number \
               to router interface number\n");
      return L7_FAILURE;
  }
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
                 "Code Flow.\n", __FUNCTION__, __LINE__);
  if(mgmdMapCbPtr->pMgmdInfo == L7_NULLPTR)
  {
      /* MGMD component was not initialized during system initialization */
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD component is not initialised\n");
      return L7_FAILURE;
  }
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
                 "Code Flow.\n", __FUNCTION__, __LINE__);
  if(mgmdMapCbPtr->pMgmdInfo->mgmdIntfInfo[rtrIfNum].mgmdInitialized == L7_TRUE)
  {
     MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
                        "MGMD is intialized\n", __FUNCTION__, __LINE__);
     return L7_SUCCESS;
  }
  else
  {
     MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
                    "MGMD is not initialized for this interface - %d.\n",
                    __FUNCTION__, __LINE__, rtrIfNum);
     MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
                 "Code Flow.\n", __FUNCTION__, __LINE__);
     return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Get the proxy Interface Querier on the Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    querierIP   @b{(output)} Querier Address on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceQuerierGet(L7_uchar8 familyType,
                                        L7_uint32 intIfNum,
                                        L7_inet_addr_t* querierIP)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapProxyExtenIntfQuerierGet(mgmdMapCbPtr, intIfNum,
                                           querierIP);
}

/*********************************************************************
* @purpose  Get the version 1 Querier time on the mgmd proxy Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    timer       @b{(output)} Querier Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceVer1QuerierTimerGet (L7_uchar8 familyType,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32* timer)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapProxyExtenIntfVer1QuerierTimeGet(mgmdMapCbPtr,
                                                intIfNum, timer);
}

/*********************************************************************
* @purpose  Get the version 2 Querier time on the mgmd proxy Interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    timer         @b{(output)} Querier Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceVer2QuerierTimerGet (L7_uchar8 familyType,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32* timer)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapProxyExtenIntfVer2QuerierTimeGet(mgmdMapCbPtr,
                                                   intIfNum, timer);
}


/*********************************************************************
* @purpose  Gets the Next Valid Interface for MGMD-PROXY
*
* @param    familyType  @b{(input)}   Address Family type
* @param    pIntIfNum   @b{(inout)}   Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceEntryNextGet(L7_uchar8 familyType,
                                          L7_uint32 *pIntIfNum)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

  return mgmdProxyInterfaceEntryNextGet(mgmdMapCbPtr->proxyCbHandle,
                                        pIntIfNum);
}

/*********************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    familyType          @b{(input)} Address Family type
* @param    intIfNum            @b{(input)} Internal Interface Number
* @param    numOfGroups         @b{(output)}Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceNumMcastGroupsGet(L7_uchar8 familyType,
                                               L7_uint32 intIfNum,
                                               L7_uint32* numOfGroups)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }


    return mgmdMapProxyExtenIntfGroupsGet(mgmdMapCbPtr, intIfNum,
                                          numOfGroups);
}


/*********************************************************************
* @purpose  Set the unsolicited Interval for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(input)} Startup  Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceUnsolicitedIntervalSet(L7_uchar8 familyType,
                                                    L7_uint32 intIfNum,
                                                    L7_uint32 interval)
{
    mgmdIntfCfgData_t* intfCfg;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
               is valid for MGMD configuration\n");
       return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
       MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
       return L7_FAILURE;
    }

    intfCfg->unsolicitedReportInterval = interval;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_UNSOLICITED_REPORT_INTERVAL_SET,
                            interval, intIfNum, MGMD_INTERFACE_MODE_NULL)
                            != L7_SUCCESS)
    {
       MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS, "MGMD Map Event Post Failed for eventType:%d.\n",
                      MGMD_UNSOLICITED_REPORT_INTERVAL_SET);
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Unsolicited Report Interval configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(output)} Startup Query Interval on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceUnsolicitedIntervalGet(L7_uchar8 familyType,
                                                    L7_uint32 intIfNum,
                                                    L7_uint32* interval)
{
    mgmdIntfCfgData_t* intfCfg;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (interval == L7_NULLPTR)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
      return L7_FAILURE;
    }

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
       MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
       return L7_FAILURE;
    }

    *interval = intfCfg->unsolicitedReportInterval;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Restart count for the Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    count       @b{(output)} Restart count on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyInterfaceRestartCountGet(L7_uchar8 familyType,
                                              L7_uint32 intIfNum,
                                              L7_uint32* count)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenIntfRestartCountGet(mgmdMapCbPtr,
                                             intIfNum, count);
}


/****************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache
*           Table for the specified group address & the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    upTime            @b{(output)} Up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyCacheUpTimeGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *multipaddr,
                                   L7_uint32 intIfNum,
                                   L7_uint32 *upTime)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenCacheUpTimeGet(mgmdMapCbPtr,
                                     multipaddr, intIfNum, upTime);
}


/***************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    ipAddr            @b{(output)} last reporter ip
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
****************************************************************************/
L7_RC_t mgmdMapProxyCacheLastReporterGet(L7_uchar8 familyType,
                                         L7_inet_addr_t *multipaddr,
                                         L7_uint32 intIfNum,
                                         L7_inet_addr_t *ipAddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenCacheLastReporterGet(mgmdMapCbPtr, multipaddr,
                                                 intIfNum, ipAddr);
}

/***************************************************************************
* @purpose  Gets MGMD Proxy Cache Status
*
* @param    familyType        @b{(input)}  Address Family type
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    status            @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
****************************************************************************/
L7_RC_t mgmdMapProxyCacheStatusGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *multipaddr,
                                   L7_uint32 intIfNum,
                                   L7_uint32 *status)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenCacheStatusGet(mgmdMapCbPtr, multipaddr,
                                           intIfNum, status);
}

/*****************************************************************************
* @purpose  Gets the group filter mode (Include or Exclude) for the specified
*           group on the specified interface
*
* @param    familyType       @b{(input)}  Address Family type
* @param    groupAddr        @b{(input)}  multicast group ip address
* @param    intIfNum         @b{(input)}  internal Interface number
* @param    groupFilterMode  @b{(output)} group filter mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyCacheGroupFilterModeGet(L7_uchar8 familyType,
                                            L7_inet_addr_t *groupAddr,
                                            L7_uint32 intIfNum,
                                            L7_uint32 *groupFilterMode)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenCacheGroupFilterModeGet(mgmdMapCbPtr, groupAddr,
                                                    intIfNum, groupFilterMode);
}


/************************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipaddr              @b{(input)}  multicast group ip address
* @param    intIfNum            @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyCacheEntryGet (L7_uchar8 familyType,
                                   L7_inet_addr_t *ipaddr,
                                   L7_uint32  intIfNum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenCacheEntryGet(mgmdMapCbPtr, ipaddr,
                                          intIfNum);
}


/***********************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given
*           multi ipaddress & an interface number.
*
* @param    familyType         @b{(input)}  Address Family type
* @param    multipaddr         @b{(inout)}  multicast group ip address
* @param    pIntIfNum           @b{(inout)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments if intIfNum = 0 & multipaddr = 0, then return
*           the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyCacheEntryNextGet(L7_uchar8 familyType,
                                      L7_inet_addr_t *multipaddr,
                                      L7_uint32 *pIntIfNum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapProxyExtenCacheEntryNextGet(mgmdMapCbPtr, multipaddr,
                                              pIntIfNum);
}

/**********************************************************************
* @purpose  Get the Proxy interface number configured for
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapMgmdProxyIntfInitialized(L7_uchar8 familyType,
                                         L7_uint32 intIfNum)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
    L7_RC_t rc = L7_FAILURE;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
               is valid for MGMD configuration\n");
      return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
      return L7_ERROR;
    }
    if ((intfCfg->interfaceMode == MGMD_PROXY_INTERFACE) &&
        (mgmdMapCbPtr->pMgmdInfo->mgmdProxyEnabled == L7_ENABLE))
    {
      rc = L7_SUCCESS;
    }
    else
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,
                     "%s %d: MGMD Proxy not enabled for this interface\n",
                     __FUNCTION__, __LINE__);
      rc = L7_FAILURE;
    }

    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "The interface is not an MGMD Proxy interface\n");
    return rc;
}

/************************************************************************
* @purpose  Get the Proxy v3 queries received on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v3QueriesRcvd  @b{(output)} queries received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV3QueriesRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v3QueriesRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV3QueriesRecvdGet(mgmdMapCbPtr, intIfNum,
                                              v3QueriesRcvd);
}

/***********************************************************************
* @purpose  Get the Proxy v3 reports received on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v3ReportsRcvd  @b{(output)} Reports received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV3ReportsRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v3ReportsRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV3ReportsRecvdGet(mgmdMapCbPtr, intIfNum,
                                              v3ReportsRcvd);
}


/***********************************************************************
* @purpose  Get the Proxy v3 reports sent on that interface
*
* @param    familyType     @b{(input)}   Address Family type
* @param    intIfNum       @b{(input)}   internal interface number
* @param    v3ReportsSent  @b{(output)}  Reports sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV3ReportsSentGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *v3ReportsSent)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return  mgmdMapProxyExtenV3ReportsSentGet(mgmdMapCbPtr, intIfNum,
                                              v3ReportsSent);
}


/************************************************************************
* @purpose  Get the Proxy v2 queries received on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v2QueriesRcvd  @b{(output)} Queries received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*******************************************************************/
L7_RC_t  mgmdMapProxyV2QueriesRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v2QueriesRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV2QueriesRecvdGet(mgmdMapCbPtr, intIfNum,
                                              v2QueriesRcvd);
}

/***********************************************************************
* @purpose  Get the Proxy v2 reports received on that interface
*
* @param    familyType     @b{(input)}   Address Family type
* @param    intIfNum       @b{(input)}   internal interface number
* @param    v2ReportsRcvd  @b{(output)}  Reports received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV2ReportsRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v2ReportsRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV2ReportsRecvdGet(mgmdMapCbPtr, intIfNum,
                                              v2ReportsRcvd);
}

/*************************************************************************
* @purpose  Get the Proxy v2 reports sent on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v2ReportsSent  @b{(output)} Reports sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV2ReportsSentGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *v2ReportsSent)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV2ReportsSentGet(mgmdMapCbPtr, intIfNum,
                                             v2ReportsSent);
}

/*************************************************************************
* @purpose  Get the Proxy v2 leaves received on that interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    intIfNum      @b{(input)}  internal interface number
* @param    v2LeavesRcvd  @b{(output)} Leaves Received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV2LeavesRecvdGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *v2LeavesRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV2LeavesRecvdGet(mgmdMapCbPtr, intIfNum,
                                             v2LeavesRcvd);
}

/**********************************************************************
* @purpose  Get the Proxy v2 leaves sent on that interface
*
* @param    familyType    @b{(input)}   Address Family type
* @param    intIfNum      @b{(input)}   internal interface number
* @param    v2LeavesSent  @b{(output)}  Leaves Sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV2LeavesSentGet(L7_uchar8 familyType,
                                     L7_uint32 intIfNum,
                                     L7_uint32 *v2LeavesSent)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV2LeavesSentGet(mgmdMapCbPtr, intIfNum,
                                            v2LeavesSent);
}

/***********************************************************************
* @purpose  Get the Proxy v1 queries received on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v1QueriesRcvd  @b{(output)} Queries received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV1QueriesRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v1QueriesRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV1QueriesRecvdGet(mgmdMapCbPtr, intIfNum,
                                              v1QueriesRcvd);
}

/***********************************************************************
* @purpose  Get the Proxy v1 reports received on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v1ReportsRcvd  @b{(output)} Reports received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV1ReportsRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v1ReportsRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV1ReportsRecvdGet(mgmdMapCbPtr, intIfNum,
                                              v1ReportsRcvd);
}

/************************************************************************
* @purpose  Get the Proxy v1 reports sent on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v1ReportsSent  @b{(output)} Reports sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV1ReportsSentGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *v1ReportsSent)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenV1ReportsSentGet(mgmdMapCbPtr, intIfNum,
                                             v1ReportsSent);
}


/*****************************************************************************
* @purpose  To validate if the specified interface contains a inverse cache
*           entry for the specified group or not
*
* @param    familyType          @b{(input)} Address Family type
* @param    intIfNum            @b{(input)} internal Interface number
* @param    groupAddr           @b{(input)} multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Inverse Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInverseCacheEntryGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_inet_addr_t *groupAddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenInverseCacheEntryGet(mgmdMapCbPtr, intIfNum,
                                                 groupAddr);
}

/*****************************************************************************
* @purpose  Get the Next Entry in inverse Cache Table for the specified group
*           and interface
*
* @param    familyType         @b{(input)}  Address Family type
* @param    pIntIfNum          @b{(input)}  internal Interface number
* @param    ipaddr             @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments if intIfNum=0 & groupAddr=0, then return the first
*           valid entry in the inverse cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInverseCacheEntryNextGet(L7_uchar8 familyType,
                                             L7_uint32 *pIntIfNum,
                                             L7_inet_addr_t *ipaddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenInverseCacheEntryNextGet(mgmdMapCbPtr,
                                               pIntIfNum, ipaddr);
}


/*****************************************************************************
* @purpose  To validate if the specified source address exists for the specified
*           group address and interface or not
*
* @param    familyType         @b{(input)}  Address Family type
* @param    groupAddr          @b{(input)}  multicast group ip address
* @param    intIfNum           @b{(input)}  internal Interface number
* @param    hostAddr           @b{(input)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxySrcListEntryGet(L7_uchar8 familyType,
                                    L7_inet_addr_t *groupAddr,
                                    L7_uint32 intIfNum,
                                    L7_inet_addr_t *hostAddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenSrcListEntryGet(mgmdMapCbPtr, groupAddr,
                               intIfNum, hostAddr);
}

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
*           interface and source address
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(inout)}  multicast group ip address
* @param    pIntIfNum         @b{(inout)}  internal Interface number
* @param    hostAddr          @b{(inout)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments if intIfNum=0, groupAddr=0 & hostAddr=0,
*           then return the first valid source list entry
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxySrcListEntryNextGet(L7_uchar8 familyType,
                                        L7_inet_addr_t *groupAddr,
                                        L7_uint32 *pIntIfNum,
                                        L7_inet_addr_t *hostAddr)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenSrcListEntryNextGet(mgmdMapCbPtr, groupAddr,
                                                pIntIfNum, hostAddr);
}
/****************************************************************************
* @purpose  Get the amount of time until the specified Source
*           Entry is aged out
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    hostAddr          @b{(input)}  source address
* @param    srcExpiryTime     @b{(output)} source expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxySrcListExpiryTimeGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_inet_addr_t *groupAddr,
                                         L7_inet_addr_t *hostAddr,
                                         L7_uint32 *srcExpiryTime)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered\n", __FUNCTION__, __LINE__);
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapProxyExtenSrcListExpiryTimeGet(mgmdMapCbPtr, intIfNum,
                                      groupAddr, hostAddr, srcExpiryTime);
}

/*********************************************************************
* @purpose  Get the MGMD Version configured for the Interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    version      @b{(output)} MGMD Version on the interface
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
L7_RC_t mgmdMapProxyInterfaceVersionGet(L7_uchar8 familyType,
                                        L7_uint32 intIfNum,
                                        L7_uint32* version)
{
   mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
   mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
   L7_RC_t rc = L7_FAILURE;

   MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

   if (version == L7_NULLPTR)
   {
     MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," NULL output parameter");
     return L7_FAILURE;
   }

   if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
   {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
     /* Failed to get control block */
     return L7_FAILURE;
   }


    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
      rc = L7_FAILURE;
    }
    else
    {
      *version = intfCfg->version;
      rc = L7_SUCCESS;
    }
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get the MGMD Proxy Version\n");
    return rc;

}

#ifdef  MGMD_PROXY_FP_NOT_SUPPORTED
/*********************************************************************
* @purpose  Set the MGMD Proxy Version for this interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    version      @b{(input)} MGMD Version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceVersionSet(L7_uchar8 familyType,
                                        L7_uint32 intIfNum,
                                        L7_uint32 version)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if ((version < L7_IGMP_VERSION_MIN) || (version > L7_IGMP_VERSION_MAX))
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD proxy Version our of range\n");
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
               is valid for MGMD configuration\n");
      return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
      return L7_FAILURE;
    }
    if (intfCfg->version == version)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "Already the version %d is set", version);
      return L7_SUCCESS;
    }
    intfCfg->version = version;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (mgmdMapIntfVersionApply(mgmdMapCbPtr, intIfNum,
                                     version) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD component is not activated yet\n");
    }
    return L7_SUCCESS;
}
#endif
/*********************************************************************
* @purpose  Resets the proxy statistics parameter
*
* @param    familyType   @b{(input)}   Address Family type
* @param    intIfNum     @b{(input)}   Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/

L7_RC_t mgmdMapProxyResetHostStatus(L7_uchar8 familyType,
                                    L7_uint32 intIfNum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenResetHostStatus(mgmdMapCbPtr, intIfNum);
}
#ifdef  MGMD_PROXY_FP_NOT_SUPPORTED
/*********************************************************************
* @purpose  Sets the specified interface as MGMD QUERIER
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    mode         @b{(input)}  mode
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
L7_RC_t  mgmdMapInterfaceQuerierStatusSet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum,
                                          L7_uint32 mode)
{
    mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                  &intfCfg) != L7_TRUE)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return L7_ERROR;
    }

    if (intfCfg == L7_NULL)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get configuration structure for this interface\n");
        return L7_FAILURE;
    }

    intfCfg->qurierStatus = mode;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disables the down stream traffic to flow into up stream interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    mode         @b{(input)}  mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyUpstreamFwdDisable(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 mode)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapExtenProxyUpstreamFwdDisable(mgmdMapCbPtr,
                                            intIfNum, mode);
}

/*********************************************************************
* @purpose  Disables the down stream traffic to flow into up stream interface
*
* @param    familyType   @b{(input)}   Address Family type
* @param    intIfNum     @b{(input)}   Internal Interface Number
* @param    mode         @b{(input)}   mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyUpstreamFwdDisableGet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum,
                                          L7_BOOL *mode)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapExtenProxyUpstreamFwdDisableGet(mgmdMapCbPtr,
                                               intIfNum, mode);
}
#endif

/*********************************************************************
* @purpose  Gets the Group status on the given interface
*
* @param    familyType     @b{(input)}   Address Family type
* @param    multipaddr     @b{(input)}   multicast IP Address
* @param    intIfNum       @b{(input)}   Internal Interface Number
* @param    hostState      @b{(output)}  host state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/

L7_RC_t mgmdMapProxyGroupStatusGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *multipaddr,
                                   L7_uint32 intIfNum,
                                   L7_uint32 *hostState)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenProxyGroupStatusGet(mgmdMapCbPtr, multipaddr,
                              intIfNum, hostState);
}



/*******************************************************************************
* @purpose  Get the number of source records for the specified group and
*           interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    srcCount          @b{(output)} number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxySrcCountGet(L7_uchar8 familyType,
                                L7_inet_addr_t *groupAddr,
                                L7_uint32 intIfNum,
                                L7_uint32 *srcCount)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapProxyExtenSrcCountGet(mgmdMapCbPtr, groupAddr,
                                        intIfNum, srcCount);
}

/*********************************************************************
* @purpose  Checks if MGMD-Proxy is Enabled.
*
* @param    familyType          @b{(input)} Address Family type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/

L7_BOOL mgmdMapProxyChkIsEnabled(L7_uchar8 familyType)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FALSE;
  }

  if (mgmdMapCbPtr->pMgmdInfo->mgmdProxyEnabled == L7_ENABLE)
  {
     return L7_TRUE;
  }
  else
  {
     MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS,
                    "MGMD Proxy not enabled for the interface\n");
     return L7_FALSE;
  }
}

/*********************************************************************
* @purpose  To register Callback routine for MGMD to call MRP's
*
* @param    familyType         @b{(input)}  Address Family type
* @param    routerProtocol_ID  @b{(input)}  protocol ID.
* @param    notifyFn           @b{(input)}  Function Pointer to call
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapRegisterMembershipChangeEvent(L7_uchar8 familyType,
                       L7_MRP_TYPE_t  routerProtocol_ID,
                       L7_RC_t (*notifyFn)(L7_uchar8 family,
                       mcastEventTypes_t event,
                       L7_uint32 msgLen, void *pMsg))
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }

  if (routerProtocol_ID >= L7_MRP_MAXIMUM)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, " Invalid protocol ID = %d", routerProtocol_ID);
    return L7_FAILURE;
  }

  mgmdMapCbPtr->mgmdInfo.mgmdGrpInfoNotifyList[routerProtocol_ID].notifyMembershipChange = notifyFn;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Deregister Callback routine for MGMD to call MRP's
*
* @param    familyType         @b{(input)}  Address Family type
* @param    routerProtocol_ID  @b{(input)}  protocol ID.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDeRegisterMembershipChangeEvent(L7_uchar8 familyType,
                                               L7_MRP_TYPE_t  routerProtocol_ID)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    if (routerProtocol_ID >= L7_MRP_MAXIMUM)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, " Invalid protocol ID = %d", routerProtocol_ID);
      return L7_FAILURE;
    }

    mgmdMapCbPtr->mgmdInfo.mgmdGrpInfoNotifyList[routerProtocol_ID].notifyMembershipChange = L7_NULLPTR;

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    addrFamily  @b{(input)} address Family
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    msg         @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMessageQueueSend(L7_uchar8 addrFamily, L7_uint32 eventType,
                                L7_uint32 msgLen, L7_VOIDPTR pMsg)
{
  mgmdMapPktMsg_t pktMsg;
  mgmdAppTimerMsg_t appTimerMsg;
  mgmdMapMsg_t      eventMsg;
  L7_uint32         rtrIfNum, intIfNum;

  if ((pMsg == L7_NULLPTR) ||
      ((eventType != MCAST_EVENT_IGMP_CONTROL_PKT_RECV) &&
       (eventType != MCAST_EVENT_MLD_CTRL_PKT_RECV) &&
       (eventType != MCAST_MFC_NOCACHE_EVENT) &&
       (eventType != MCAST_MFC_ENTRY_EXPIRE_EVENT) &&
       (eventType != MCAST_EVENT_MGMD_TIMER_EXPIRY_EVENT) &&
       (eventType != MCAST_EVENT_MGMD_GROUP_INFO_GET) &&
       (eventType != MCAST_EVENT_ADMINSCOPE_BOUNDARY)))
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                   "valid for eventType:%d.\n", eventType);
    return L7_FAILURE;
  }

  if (eventType == MCAST_EVENT_ADMINSCOPE_BOUNDARY)
  {
    intIfNum = ((mcastAdminMsgInfo_t *)pMsg)->intIfNum;
  }
  if ((eventType == MCAST_MFC_NOCACHE_EVENT) ||
      (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
  {
    if (mgmdMapProxyChkIsEnabled(addrFamily) != L7_TRUE)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,
                     "MGMD-Proxy not enabled for event =%d ", eventType);
      return L7_FAILURE;
    }
  }


  if(eventType == MCAST_EVENT_MGMD_TIMER_EXPIRY_EVENT)
  {
    memset(&appTimerMsg, 0, sizeof(mgmdAppTimerMsg_t));
    appTimerMsg.msgId = MGMDMAP_PROTOCOL_EVENT_MSG;
    appTimerMsg.event = eventType;
    appTimerMsg.timerCtrlBlk = pMsg;

    if(mgmdMessageSend(MGMD_APP_TIMER_Q,&appTimerMsg) != L7_SUCCESS)
    {
     MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n", eventType);
     return L7_FAILURE;
    }
  }
  else if((eventType == MCAST_EVENT_IGMP_CONTROL_PKT_RECV)||
            (eventType == MCAST_EVENT_MLD_CTRL_PKT_RECV))
  {

    memset(&pktMsg, 0, sizeof(mgmdMapPktMsg_t));
    pktMsg.msgId = MGMDMAP_PROTOCOL_EVENT_MSG;

    pktMsg.familyType = addrFamily;
    pktMsg.event = eventType;
    memcpy(&(pktMsg.ctrlPkt), pMsg, msgLen);
    rtrIfNum =  ((mcastControlPkt_t *)pMsg)->rtrIfNum;

    if (mcastIpMapRtrIntfToIntIfNum (addrFamily, rtrIfNum, &intIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,
                      "Failed to convert rtrIfNum (%d) to intIfNum for "
                      "familyType %d", rtrIfNum, addrFamily);
      return L7_FAILURE;
    }

    if ((mgmdMapMgmdIntfInitialized (addrFamily, intIfNum) != L7_SUCCESS) &&
         (mgmdMapMgmdProxyIntfInitialized(addrFamily, intIfNum) != L7_SUCCESS))
    {
      MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,
                      "MGMD/Proxy is not Operational on rtrIfNum - %d for Family - %d"
                      " for eventType - %d", rtrIfNum, addrFamily, eventType);
      return L7_FAILURE;
    }

    if(mgmdMessageSend(MGMD_CTRL_PKT_Q,&pktMsg) != L7_SUCCESS)
    {
     MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n", eventType);
     return L7_FAILURE;
    }

  }
  else
  {
    memset(&eventMsg, 0, sizeof(mgmdMapMsg_t));
    eventMsg.msgId = MGMDMAP_PROTOCOL_EVENT_MSG;

    eventMsg.u.mgmdMapProtocolEvenetParms.familyType = addrFamily;
    eventMsg.u.mgmdMapProtocolEvenetParms.event = eventType;
    memcpy(&(eventMsg.u.mgmdMapProtocolEvenetParms.u), pMsg, msgLen);

    if(mgmdMessageSend(MGMD_EVENT_Q,&eventMsg) != L7_SUCCESS)
    {
     MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n", eventType);
     return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Received for the router.
*
* @param    familyType  @b{(input)}   Address Family type
* @param    validPkts   @b{(output)}  Valid Packets Received.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapValidPacketsRcvdGet(L7_uchar8 familyType,
                                   L7_uint32 *validPkts)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenValidPacketsRcvdGet(mgmdMapCbPtr, validPkts);
}

/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Sent for the router.
*
* @param    familyType @b{(input)}    Address Family type
* @param    pktsSent   @b{(output)}   Valid Packets Sent.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapValidPacketsSentGet(L7_uchar8 familyType,
                                   L7_uint32 *pktsSent)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenValidPacketsSentGet(mgmdMapCbPtr, pktsSent);
}

/*********************************************************************
* @purpose  Get the Queries Received for the router.
*
* @param    familyType  @b{(input)}  Address Family type
* @param    quriesRcvd  @b{(output)} Queries Rcvd.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE

* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapValidQueriesRcvdGet(L7_uchar8 familyType,
                                   L7_uint32 *quriesRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenQueriesRcvdGet(mgmdMapCbPtr, quriesRcvd);
}

/*********************************************************************
* @purpose  Get the Queries Sent for the router.
*
* @param    familyType  @b{(input)}  Address Family type
* @param    quriesSent  @b{(output)} Queries Sent.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapValidQueriesSentGet(L7_uchar8 familyType,
                                   L7_uint32 *quriesSent)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenQueriesSentGet(mgmdMapCbPtr, quriesSent);
}

/*********************************************************************
* @purpose  Get the Reports Received for the router.
*
* @param    familyType   @b{(input)}  Address Family type
* @param    reportsRcvd  @b{(output)} Reports Received.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapReportsReceivedGet(L7_uchar8 familyType,
                                  L7_uint32 *reportsRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenReportsRcvdGet(mgmdMapCbPtr, reportsRcvd);
}

/*********************************************************************
* @purpose  Get the Reports Sent for the router.
*
* @param    familyType   @b{(input)}  Address Family type
* @param    reportsSent  @b{(output)} Reports Sent.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapReportsSentGet(L7_uchar8 familyType,
                              L7_uint32 *reportsSent)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenReportsSentGet(mgmdMapCbPtr, reportsSent);
}

/*********************************************************************
* @purpose  Get the Leaves Received for the router.
*
* @param    familyType  @b{(input)}  Address Family type
* @param    leavesRcvd  @b{(output)} Leaves Received.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapLeavesReceivedGet(L7_uchar8 familyType,
                                 L7_uint32 *leavesRcvd)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenLeavesRcvdGet(mgmdMapCbPtr, leavesRcvd);
}

/*********************************************************************
* @purpose  Get the Leaves Sent for the router.
*
* @param    familyType   @b{(input)}  Address Family type
* @param    leavesSent   @b{(output)} Leaves Sent.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapLeavesSentGet(L7_uchar8 familyType,
                             L7_uint32 *leavesSent)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }

    return mgmdMapExtenLeavesSentGet(mgmdMapCbPtr, leavesSent);
}

/*********************************************************************
* @purpose  Get the Bad Checksum packets for the router.
*
* @param    familyType   @b{(input)}  Address Family type.
* @param    badChkSum    @b{(output)} Bad Checksum packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapBadCheckSumPktsGet(L7_uchar8 familyType,
                                  L7_uint32 *badChkSum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapExtenBadCheckSumPktsGet(mgmdMapCbPtr, badChkSum);
}

/*********************************************************************
* @purpose  Get the Malformed packets for the router.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    malfrmedPkts  @b{(output)} Malformed packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMalformedPktsGet(L7_uchar8 familyType,
                                L7_uint32 *malfrmedPkts)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapExtenMalformedPktsGet(mgmdMapCbPtr, malfrmedPkts);
}

/*********************************************************************
* @purpose  Clear the  Counters for MLD.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    intIfNum    @b{(input)} interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCountersClear(L7_uchar8 familyType, L7_uint32 intIfNum)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapExtenCountersClear(mgmdMapCbPtr, intIfNum);
}

/*********************************************************************
* @purpose  Clear the  Traffic Counters for MLD.
*
* @param    familyType    @b{(input)}  Address Family type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapTrafficCountersClear(L7_uchar8 familyType)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
      /* Failed to get control block */
      return L7_FAILURE;
    }
    return mgmdMapExtenTrafficCountersClear(mgmdMapCbPtr);
}

/*********************************************************************
* @purpose  Get the MGMD  IP Multicast route table Next entry
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
L7_RC_t mgmdMapProxyIpMRouteEntryNextGet(L7_uchar8 familyType,
                                         L7_inet_addr_t* ipMRouteGroup,
                                         L7_inet_addr_t* ipMRouteSource,
                                         L7_inet_addr_t* ipMRouteSourceMask)
{
    mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block");
      return L7_FAILURE;
    }

    return mgmdMapProxyExtenIpMRouteEntryNextGet(mgmdMapCbPtr, ipMRouteGroup,
                                                 ipMRouteSource, ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Gets the minimum amount of time remaining before  this
*           entry will be aged out.
*
* @param    familyType     @b{(input)}  Address Family type
* @param    ipMRtGrp       @b{(input)} Multicast Group address
* @param    ipMRouteSrc    @b{(input)} Source address
* @param    ipMRtSrcMask   @b{(input)} Mask Address
* @param    expire         @b{(output)} expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The value of 0 indicates that the entry is not
*           subject to aging.
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyIpMRouteExpiryTimeGet(L7_uchar8 familyType,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask,
                                      L7_uint32       *expire)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block");
    return L7_FAILURE;
  }

  return mgmdMapProxyExtenIpMRouteExpiryTimeGet(mgmdMapCbPtr, ipMRtGrp,
                                               ipMRtSrc, ipMRtSrcMask, expire);
}

/*********************************************************************
* @purpose  Gets the time since the multicast routing information
*           represented by this entry was learned by the router.
*
* @param    familyType    @b{(input)}  Address Family type
* @param    ipMRtGrp      @b{(input)} Multicast Group address
* @param    ipMRouteSrc   @b{(input)} Source address
* @param    ipMRtSrcMask  @b{(input)} Mask Address
* @param    upTime        @b{(output)} uptime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyIpMRouteUpTimeGet(L7_uchar8 familyType,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask,
                                      L7_uint32       *upTime)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block");
    return L7_FAILURE;
  }

  return mgmdMapProxyExtenIpMRouteUpTimeGet(mgmdMapCbPtr, ipMRtGrp,
                                            ipMRtSrc, ipMRtSrcMask, upTime);
}


/**********************************************************************
* @purpose  Gets the Rpf address for the given index
*
* @param    familyType      @b{(input)}  Address Family type
* @param    ipMRtGrp        @b{(input)} mcast group address
* @param    ipMRtSrc        @b{(input)} mcast source
* @param    ipMRtSrcMask    @b{(input)} source mask
* @param    rpfAddr         @b{(output)} RPF address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyIpMRouteRpfAddrGet(L7_uchar8 familyType,
                                       L7_inet_addr_t *ipMRtGrp,
                                       L7_inet_addr_t *ipMRtSrc,
                                       L7_inet_addr_t *ipMRtSrcMask,
                                       L7_inet_addr_t *rpfAddress)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block");
    return L7_FAILURE;
  }

  return mgmdMapProxyExtenIpMRouteRpfAddrGet(mgmdMapCbPtr, ipMRtGrp,
                                             ipMRtSrc, ipMRtSrcMask, rpfAddress);
}
/**********************************************************************
* @purpose  Gets the number of MRT entries in Proxy table.
*
* @param    familyType      @b{(input)}  Address Family type
* @param    count           @b{(input)} Number of MRT entries
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyIpMRouteEntryCountGet(L7_uchar8 familyType, L7_uint32 *count)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block");
    return L7_FAILURE;
  }
  return  mgmdMapProxyExtenIpMRouteEntryCountGet(mgmdMapCbPtr, count);
}
/*********************************************************************
* @purpose  Gets the interface index on which IP datagrams sent by
*           these sources to this multicast address are received.
*           corresponding to the index received.
*
* @param    familyType     @b{(input)}  Address Family type
* @param    ipMRtGrp       @b{(input)}  Multicast Group address
* @param    ipMRouteSrc    @b{(input)}  Source address
* @param    ipMRtSrcMask   @b{(input)}  Mask Address
* @param    intIfNum       @b{(output)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A value of 0 indicates that datagrams are not subject
*           to an incoming interface check, but may be accepted on
*           multiple interfaces.
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyIpMRouteIfIndexGet(L7_uchar8 familyType,
                                       L7_inet_addr_t *ipMRtGrp,
                                       L7_inet_addr_t *ipMRtSrc,
                                       L7_inet_addr_t *ipMRtSrcMask,
                                       L7_uint32 *inIfIndex)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block");
    return L7_FAILURE;
  }

  return mgmdMapProxyExtenIpMRouteIfIndexGet(mgmdMapCbPtr, ipMRtGrp,
                                             ipMRtSrc, ipMRtSrcMask, inIfIndex);
}


/*********************************************************************
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType         @b{(input)}  Address Family type
* @param    ipMRtGrp           @b{(input)} mcast group
* @param    ipMRtSrc           @b{(input)} mcast source
* @param    ipMRtSrcMask       @b{(input)} source mask
* @param    intIfNum           @b{(inout)} interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*****************************************************************/
L7_RC_t mgmdMapProxyRouteOutIntfEntryNextGet(L7_uchar8 familyType,
                                             L7_inet_addr_t *ipMRtGrp,
                                             L7_inet_addr_t *ipMRtSrc,
                                             L7_inet_addr_t *ipMRtSrcMask,
                                             L7_uint32 *intIfNum)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block");
    return L7_FAILURE;
  }

  return mgmdMapProxyExtenRouteOutIntfEntryNextGet(mgmdMapCbPtr, ipMRtGrp,
                                                   ipMRtSrc, ipMRtSrcMask, intIfNum);
}

/*********************************************************************
* @purpose  Retrieve group information learned by MGMD
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} Router Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is used by MRPs to get the group information learned
*           by MGMD whenever MRP is disabled and enabled at run-time
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMRPGroupInfoGet(L7_uchar8 familyType, L7_uint32 rtrIfNum)
{
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if ((familyType != L7_AF_INET) && (familyType != L7_AF_INET6))
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid FamilyType - %d", familyType);
    return L7_FAILURE;
  }
  if ((rtrIfNum <= 0) || (rtrIfNum > MCAST_MAX_INTERFACES))
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid rtrIfNum - %d", rtrIfNum);
    return L7_FAILURE;
  }

  if (mgmdMapComponentCallback (familyType, MCAST_EVENT_MGMD_GROUP_INFO_GET,
                                sizeof (L7_uint32), (void*) &rtrIfNum)
                             != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for "
                    "eventType - %d.", MCAST_EVENT_MGMD_GROUP_INFO_GET);
    return L7_FAILURE;
  }

  MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_APIS, "Function Exit.");
  return L7_SUCCESS;
}

