/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_api.c
*
* @purpose   Multiple Spanning tree APIs
*
* @component dot1s
*
* @comments 
*
* @create    9/19/2002
*
* @author    spetriccione
*
* @end
**********************************************************************/

#include "dot1s_include.h"

dot1sNotifyList_t   *dot1sNotifyList;
extern DOT1S_PORT_STATS_t			*dot1sPortStats;
extern dot1sDeregister_t dot1sDeregister;
extern dot1sCnfgrState_t dot1sCnfgrState;
extern dot1sCfg_t *dot1sCfg;
extern void *dot1sTaskSyncSema;

extern  DOT1S_VLAN_MASK_t           dot1sVlanMask;
extern  DOT1S_INSTANCE_MAP_t	    *dot1sInstanceMap;
/*********************************************************************
* @purpose  Set the MSTP Force Protocol Version parameter	
*          
* @param    val @b{(input)} the new value of the MSTP parameter  
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sForceVersionSet(DOT1S_FORCE_VERSION_t val)
{
  if ((val < DOT1S_FORCE_VERSION_LAST) && (val != DOT1S_FORCE_VERSION_UNUSED))
  {
    dot1sCfg->cfg.dot1sBridge.ForceVersion = val;
	dot1sCfg->hdr.dataChanged = L7_TRUE;
    /* issue the command */
    return(dot1sIssueCmd(switchForceVersionSet, L7_NULL, L7_NULL, &val));
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the value of the MSTP Force Protocol Version parameter	
*          
* @param    
*
* @returns  the current value of the MSTP parameter or L7_NULL in the
* @returns  event of an error
*
* @comments
*
* @end
*********************************************************************/
DOT1S_FORCE_VERSION_t dot1sForceVersionGet()
{
  return dot1sCfg->cfg.dot1sBridge.ForceVersion;
  }

/*********************************************************************
* @purpose  Set the MSTP Operational Mode 
*          
* @param    val @b{(input)} the new value of the MSTP mode  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sModeSet(L7_BOOL val)
{
  L7_uint32 event;

  if (val != L7_TRUE)
  {
    event = switchModeDisable;
  }
  else 
  { 
    event = switchModeEnable;
  }

  dot1sCfg->cfg.dot1sBridge.Mode = val;
  dot1sCfg->hdr.dataChanged = L7_TRUE;
  
  /* issue the command */
  return(dot1sIssueCmd(event, L7_NULL, L7_NULL, &val));
}

/*********************************************************************
* @purpose  Get the MSTP Operational Mode 
*          
* @param    
*
* @returns the current value of the MSTP mode  
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL dot1sModeGet()
{
  return dot1sCfg->cfg.dot1sBridge.Mode;
  }

/*********************************************************************
* @purpose  Set the MSTP Configuration Format Selector 
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sConfigFormatSelectorSet(L7_uint32 val)
{
  if (val <= L7_DOT1S_MAX_FORMAT_SELECTOR)
  {
    dot1sCfg->cfg.dot1sBridge.MstConfigId.formatSelector = val;
	dot1sCfg->hdr.dataChanged = L7_TRUE;
	
    /* issue the command */
    return(dot1sIssueCmd(switchConfigFormatSelectorSet, L7_NULL, L7_NULL, &val));
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Configuration Format Selector 
*          
* @param    
*
* @returns the current value of the MSTP parameter
*
* @comments
*
*
* @end
*********************************************************************/
L7_uchar8 dot1sConfigFormatSelectorGet()
{
  return dot1sCfg->cfg.dot1sBridge.MstConfigId.formatSelector;
  }

/*********************************************************************
* @purpose  Set the MSTP Configuration Name
*          
* @param    buf @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sConfigNameSet(L7_uchar8 *buf)
{
  if ((strlen((L7_char8 *)buf) <= L7_DOT1S_MAX_CONFIG_NAME) && (strlen((char *)buf) != 0))
  {
    memset(dot1sCfg->cfg.dot1sBridge.MstConfigId.configName, L7_EOS, DOT1S_MAX_CONFIG_NAME_SIZE);
    memcpy((L7_char8 *)dot1sCfg->cfg.dot1sBridge.MstConfigId.configName, (L7_char8 *)buf, DOT1S_MAX_CONFIG_NAME_SIZE);
    /* issue the command */
    return(dot1sIssueCmd(switchConfigNameSet, L7_NULL, L7_NULL, buf));
  }
  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Configuration Name
*          
* @param    buf @b{(output)} the buffer where the Cfg Name is stored
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sConfigNameGet(L7_uchar8 buf[])
{
    bzero((L7_char8 *)buf, DOT1S_MAX_CONFIG_NAME_SIZE);

    memcpy((L7_char8 *)buf, 
           (L7_char8 *)&dot1sCfg->cfg.dot1sBridge.MstConfigId.configName, 
           DOT1S_MAX_CONFIG_NAME_SIZE);
    buf[DOT1S_MAX_CONFIG_NAME_SIZE] = L7_EOS;
    return(L7_SUCCESS);
  
  }

/*********************************************************************
* @purpose  Set the MSTP Configuration Revision
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sConfigRevisionSet(L7_uint32 val)
{
  if (val <= L7_DOT1S_MAX_REVISION_VALUE)
  {
    dot1sCfg->cfg.dot1sBridge.MstConfigId.revLevel = val;
	dot1sCfg->hdr.dataChanged = L7_TRUE;

    /* issue the command */
    return(dot1sIssueCmd(switchConfigRevisionSet, L7_NULL, L7_NULL, &val));
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Configuration Revision
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sConfigRevisionGet()
{
  return dot1sCfg->cfg.dot1sBridge.MstConfigId.revLevel;
  }

/*********************************************************************
* @purpose  Get the MSTP Configuration Digest Key
*          
* @param    
*
* @returns  a pointer to the current value of the MSTP parameter
* @returns  L7_NULLPTR bridge params not found
*
* @comments
*
*
* @end
*********************************************************************/
L7_uchar8 *dot1sConfigDigestKeyGet()
{
  DOT1S_BRIDGE_t *pBridge;

  pBridge = dot1sBridgeParmsFind();
  if (pBridge != L7_NULLPTR)
  {
    return((L7_uchar8 *)&pBridge->MstConfigId.configDigest);
  }

  return((L7_uchar8 *)L7_NULLPTR);
}

/*********************************************************************
* @purpose  Set the MSTP Bridge parameters(Forward Delay,Hello time 
            and Max age) for the CIST
*          
* @param    fwdDelay,maxAge,helloTime @b{(input)} the new parameters of the MSTP 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments  
*
*
* @end
*********************************************************************/
L7_RC_t dot1sCistBridgeParametersSet(L7_uint32 fwdDelay, L7_uint32 maxAge, L7_uint32 helloTime)
{
  L7_BOOL test1 = L7_FALSE;
  L7_BOOL test2 = L7_FALSE;
  L7_BOOL test3 = L7_FALSE;
  L7_uint32 tempMaxAge, tempFwdDelay, tempHelloTime;

  tempMaxAge = dot1sCistBridgeMaxAgeGet() * DOT1S_TIMER_UNIT;
  tempFwdDelay = dot1sCistBridgeFwdDelayGet() * DOT1S_TIMER_UNIT;
  tempHelloTime = dot1sCistBridgeHelloTimeGet() * DOT1S_TIMER_UNIT;

  if( ( 2*(fwdDelay - 1) >= maxAge ) && ( maxAge >= 2*( helloTime + 1) ) )
   {
     /* Now multiply by 256 */
     fwdDelay = fwdDelay * DOT1S_TIMER_UNIT;
     maxAge   = maxAge   * DOT1S_TIMER_UNIT;
     helloTime= helloTime* DOT1S_TIMER_UNIT;
 
     /* If setting of any one of the timer value fails,
          revert the values back to previous values */
     /* issue the command */
     dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeFwdDelay  = fwdDelay;
     test1 = dot1sIssueCmd(commonBridgeFwdDelaySet, L7_NULL, L7_NULL, &fwdDelay);
     if (test1 != L7_SUCCESS)
     {
       dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeFwdDelay  = tempFwdDelay;  
       return L7_FAILURE;
     }

     dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeMaxAge    = maxAge;
     test2 = dot1sIssueCmd(commonBridgeMaxAgeSet, L7_NULL, L7_NULL, &maxAge);
     if (test2 != L7_SUCCESS)
     {
       dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeFwdDelay  = tempFwdDelay;
       dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeMaxAge    = tempMaxAge;
       dot1sIssueCmd(commonBridgeFwdDelaySet, L7_NULL, L7_NULL, &tempFwdDelay);
       return L7_FAILURE;
     }

     dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeHelloTime = helloTime;
     test3 = dot1sIssueCmd(commonBridgeHelloTimeSet, L7_NULL, L7_NULL, &helloTime);
     if (test3 != L7_SUCCESS)
     {
       dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeFwdDelay  = tempFwdDelay;
       dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeMaxAge    = tempMaxAge;
       dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeHelloTime = tempHelloTime;

       dot1sIssueCmd(commonBridgeMaxAgeSet, L7_NULL, L7_NULL, &tempFwdDelay);
       dot1sIssueCmd(commonBridgeMaxAgeSet, L7_NULL, L7_NULL, &tempMaxAge);
       return L7_FAILURE;
     }

     dot1sCfg->hdr.dataChanged = L7_TRUE;
     return(L7_SUCCESS);
   }
   return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Set the MSTP Bridge Max Age parameter for the CIST
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments the Bridge Max Age is stored as a multiple of 256
*
*
* @end
*********************************************************************/
L7_RC_t dot1sCistBridgeMaxAgeSet(L7_uint32 val)
{
  DOT1S_INSTANCE_INFO_t *instance;
  L7_BOOL proceed = L7_FALSE;

  instance = dot1sInstFind();

  if ((val >= L7_DOT1S_BRIDGE_MAXAGE_MIN) && (val <= L7_DOT1S_BRIDGE_MAXAGE_MAX))
  {
    if ((val <= (2 * ((instance->cist.CistBridgeTimes.bridgeFwdDelay / DOT1S_TIMER_UNIT) - 1))) &&
		(val >= (2 * ((instance->cist.CistBridgeTimes.bridgeHelloTime / DOT1S_TIMER_UNIT) + 1))))
    {
        proceed = L7_TRUE;
    }
    else if (dot1sApiQueueSynchronize() == L7_SUCCESS)
    {
        proceed = ((val <= (2 * ((instance->cist.CistBridgeTimes.bridgeFwdDelay / DOT1S_TIMER_UNIT) - 1))) &&
		             (val >= (2 * ((instance->cist.CistBridgeTimes.bridgeHelloTime / DOT1S_TIMER_UNIT) + 1))));
    }
    if (proceed == L7_TRUE)
    {
      /* Now multiply by 256 */
      val = val * DOT1S_TIMER_UNIT;

	  dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeMaxAge = val;
	  dot1sCfg->hdr.dataChanged = L7_TRUE;

      /* issue the command */
      return(dot1sIssueCmd(commonBridgeMaxAgeSet, L7_NULL, L7_NULL, &val));
    }
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Bridge Max Age parameter for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Bridge Max Age is stored as a multiple of 256
*
*
* @end
*********************************************************************/
L7_uint32 dot1sCistBridgeMaxAgeGet()
{
  return (dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeMaxAge /DOT1S_TIMER_UNIT);
}

/*********************************************************************
* @purpose  Get the MSTP Root Port Max Age parameter for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Root Port Max Age is stored as a multiple of 256
*
*
* @end
*********************************************************************/
L7_uint32 dot1sCistRootMaxAgeGet()
{
  DOT1S_INSTANCE_INFO_t *instance;

  instance = dot1sInstFind();

  return(instance->cist.cistRootTimes.maxAge / DOT1S_TIMER_UNIT);
}

/*********************************************************************
* @purpose  Set the MSTP Bridge HelloTime parameter for the CIST
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments the Bridge HelloTime is stored as a multiple of 256
*
*
* @end
*********************************************************************/
L7_RC_t dot1sCistBridgeHelloTimeSet(L7_uint32 val)
{


  L7_LOG(L7_LOG_SEVERITY_WARNING, L7_DOT1S_COMPONENT_ID,
                "Attempt to update hello time which is disallowed per IEEE 802.1Q-REV 2005");

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the MSTP Bridge HelloTime paramter for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Bridge HelloTime is stored as a multiple of 256
*
*
* @end
*********************************************************************/
L7_uint32 dot1sCistBridgeHelloTimeGet()
{
  return (dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeHelloTime /DOT1S_TIMER_UNIT);
}

/*********************************************************************
* @purpose  Get the MSTP Root Port HelloTime paramter for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Root Port HelloTime is stored as a multiple of 256
*
*
* @end
*********************************************************************/
L7_uint32 dot1sCistRootHelloTimeGet()
{
  DOT1S_INSTANCE_INFO_t *instance;

  instance = dot1sInstFind();

  return(instance->cist.cistRootTimes.helloTime / DOT1S_TIMER_UNIT);
}

/*********************************************************************
* @purpose  Set the MSTP Bridge Forward Delay for the CIST
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments the Bridge Forward Delay is stored as a multiple of 256
*
*
* @end
*********************************************************************/
L7_RC_t dot1sCistBridgeFwdDelaySet(L7_uint32 val)
{
  L7_uint32 maxAge;
  DOT1S_INSTANCE_INFO_t *instance;
  L7_BOOL proceed = L7_FALSE;

  instance = dot1sInstFind();
  maxAge = instance->cist.CistBridgeTimes.bridgeMaxAge / DOT1S_TIMER_UNIT;

  /* for calculation of max age and forward delay relationships, round up */
  if ((maxAge % 2) != 0)
  {
    maxAge++;
  }

  if ((val >= L7_DOT1S_BRIDGE_FORWARDDELAY_MIN) && 
      (val <= L7_DOT1S_BRIDGE_FORWARDDELAY_MAX))
  {
    if (val >= ((maxAge / 2) + 1))
    {
      proceed = L7_TRUE;
    }
    else if (dot1sApiQueueSynchronize() == L7_SUCCESS)
    {
       maxAge = instance->cist.CistBridgeTimes.bridgeMaxAge / DOT1S_TIMER_UNIT;
       if ((maxAge % 2) != 0)
       {
         maxAge++;
       }
        proceed = (val >= ((maxAge / 2) + 1));
    }


    if (proceed == L7_TRUE)
    {
      /* Now Multiple by 256 */
      val = val * DOT1S_TIMER_UNIT;

	  dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeFwdDelay = val;
	  dot1sCfg->hdr.dataChanged = L7_TRUE;

      /* issue the command */
      return(dot1sIssueCmd(commonBridgeFwdDelaySet, L7_NULL, L7_NULL, &val));
    }
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Bridge Forward Delay for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Bridge Forward Delay is stored as a multiple of 256
*
*
* @end
*********************************************************************/
L7_uint32 dot1sCistBridgeFwdDelayGet()
{
  return (dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeFwdDelay /DOT1S_TIMER_UNIT);
}

/*********************************************************************
* @purpose  Get the MSTP Root Port Forward Delay for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sCistRootFwdDelayGet()
{
  DOT1S_INSTANCE_INFO_t *instance;

  instance = dot1sInstFind();

  return(instance->cist.cistRootTimes.fwdDelay / DOT1S_TIMER_UNIT);
}

/*********************************************************************
* @purpose  Get the MSTP Bridge Hold Time for the CIST
*          
* @param    
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sCistBridgeHoldTimeGet()
{
  DOT1S_BRIDGE_t *pBridge;

  pBridge = dot1sBridgeParmsFind();
  if (pBridge != L7_NULLPTR)
  {
    return(pBridge->TxHoldCount);
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Set the MSTP Bridge Priority parameter in the specific
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
* @param    val   @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE if val is out of range for a MSTP Bridge 
*                      Priority or the MSTID is not valid.
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiBridgePrioritySet(L7_uint32 mstID, L7_uint32 val)
{
  L7_uint32 event;
  L7_ushort16 shortPri;
  L7_ushort16 shortInstId;
  L7_uint32 i;
  L7_BOOL instInUse;

  shortPri = (L7_ushort16)val;
  shortInstId = (L7_ushort16)mstID;
  shortPri = shortInstId | shortPri;

  if (dot1sInstCheckInUse(mstID) == L7_SUCCESS)
  {
    instInUse = L7_TRUE;
  }
  else
  {
    dot1sApiQueueSynchronize();
    instInUse = (dot1sInstCheckInUse(mstID) == L7_SUCCESS);
  }

    if ((val >= L7_DOT1S_PRIORITY_MIN) && (val <= L7_DOT1S_PRIORITY_MAX))
    {
      if (instInUse == L7_TRUE)
      {
        if (mstID != DOT1S_CIST_ID)
        {
            event = instancePrioritySet;
			for (i = 1; i<= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
			{
			  if (dot1sCfg->cfg.dot1sInstance.msti[i].mstid == mstID)
			  {
			    dot1sCfg->cfg.dot1sInstance.msti[i].BridgeIdentifier.priInstId = shortPri;
				break;
			  }
			}

        }
        else
        {
            event = commonPrioritySet;
			dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId = shortPri;
        }
		dot1sCfg->hdr.dataChanged = L7_TRUE;
        /* issue the command */
        return(dot1sIssueCmd(event, L7_NULL, mstID, &val));
      }
    }
    return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Bridge Priority parameter in the specific
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  the current value of the MSTP parameter or L7_NULL if
*           not found.
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_ushort16 dot1sMstiBridgePriorityGet(L7_uint32 mstID)
{
  L7_uint32 index;
  L7_uint32 i;
  
  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {

    if (mstID != DOT1S_CIST_ID)
    {
      for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
	  {
		if (dot1sCfg->cfg.dot1sInstance.msti[i].mstid == mstID)
		{
		  return (dot1sCfg->cfg.dot1sInstance.msti[i].BridgeIdentifier.priInstId &
             DOT1S_BRIDGE_PRIORITY_MASK);
    }
      }
	}
    else
    {
      return(dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId &
             DOT1S_BRIDGE_PRIORITY_MASK);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Set the MSTP Administrative Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortEdgeSet(L7_uint32 intIfNum, L7_BOOL val)
{
  L7_uint32 event;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  if (val != L7_TRUE)
  {
    event = commonPortEdgePortDisable;
  }
  else
  { 
    event = commonPortEdgePortEnable;
  }

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
	pPortCfg->adminEdge = val;
	dot1sCfg->hdr.dataChanged = L7_TRUE;
  }
  else
	return L7_FAILURE;

  /* issue the command */
  return(dot1sIssueCmd(event, intIfNum, L7_NULL, &val));
}

/*********************************************************************
* @purpose  Get the MSTP Administrative Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortEdgeGet(L7_uint32 intIfNum, L7_BOOL *val)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;
  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);

  if (pPortCfg != L7_NULLPTR)
  {
    *val = pPortCfg->adminEdge;
		return(L7_SUCCESS);
	}

	return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Operational Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortOperEdgeGet(L7_uint32 intIfNum, L7_BOOL *val)
{
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFind(intIfNum);
  if (pPort != L7_NULLPTR)
  {
    *val = pPort->operEdge;
		return(L7_SUCCESS);
	}

	return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Operational Point to Point Mac parameter 
*           of a specific port for the CIST
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL dot1sCistPortOperPointToPointGet(L7_uint32 intIfNum)
{
  return(dot1sIhIsIntfSpeedFullDuplex(intIfNum));
}

/*********************************************************************
* @purpose  Force the specified port to transmit RSTP or MSTP BPDU's
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sPortForceMigrationCheck(L7_uint32 intIfNum)
{
  DOT1S_BRIDGE_t *pBridge;
  L7_RC_t rc = L7_FAILURE;

  pBridge = dot1sBridgeParmsFind();

  if ((pBridge != L7_NULLPTR) && 
      (pBridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W))
  {
    /* issue the command */
    rc = dot1sIssueCmd(switchPortMigrationCheck, intIfNum, L7_NULL, L7_NULLPTR);
  }

  return(rc);
}

/*********************************************************************
* @purpose  Set the MSTP Port Administrative Mode variable
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments returns L7_FAILURE if the number of ports enabled will 
*           exceed the number that can be supported by MSTP.
*
*
* @end
*********************************************************************/
L7_RC_t dot1sPortAdminModeSet(L7_uint32 intIfNum, L7_BOOL val)
{
  L7_uint32 event;
  DOT1S_BRIDGE_t *pBridge;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  if (val != L7_TRUE)
  {
    event = switchPortAdminModeDisable;
  }
  else 
  { 
    pBridge = dot1sBridgeParmsFind();
    if ((pBridge == L7_NULLPTR) || 
        (pBridge->enabledPortCount >= DOT1S_MAX_PORT_COUNT))
    {
      return(L7_FAILURE);
    }

    event = switchPortAdminModeEnable;
  }
  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
	pPortCfg->portAdminMode = val;
	dot1sCfg->hdr.dataChanged = L7_TRUE;
  }
  else
	return L7_FAILURE;

  /* issue the command */
  return(dot1sIssueCmd(event, intIfNum, L7_NULL, L7_NULLPTR));
}

/*********************************************************************
* @purpose  Get the MSTP Port Administrative Mode variable
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter. L7_TRUE for an
*           enabled port and L7_FALSE for a disabled port.
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL dot1sPortAdminModeGet(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
    if (pPortCfg->portAdminMode == L7_ENABLE)
    {
      return L7_TRUE;
    }
  }

  return(L7_FALSE);
}

/*********************************************************************
* @purpose  Check if the port in a specific instance is valid for 
*           MSTP use.
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  L7_SUCCESS valid instance/port combination specified
* @returns  L7_FAILURE not a valid combination 
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiPortCheck(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the MSTP Forwarding State of the port in a specific
*           instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current forwarding state
*
* @comments an MSTID of zero will represent the CIST
*           Use dot1sPortForwardingCheck if it is required to know if this 
*           port is forwarding in any MSTP instance
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiPortStateGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      return(pPort->portInstInfo[index].portState);
    }
  }

  return(L7_NULL);
}

// PTin added
/*********************************************************************
* @purpose  Set MAC learning state in a port.
*          
* @param    intIfNum @b{(input)} interface number
*           learnEnable: Learn state (L7_TRUE or L7_FALSE)
*
* @returns  L7_SUCCESS change is sucessfull
* @returns  L7_FAILURE if error
*
* @end
*********************************************************************/
L7_RC_t dot1sPortMacLearningSet(L7_uint32 intIfNum, L7_BOOL learnEnable)
{
  return dtlDot1sLearnModeSet(intIfNum,learnEnable);
}

/*********************************************************************
* @purpose  Get MAC learning state in a port.
*          
* @param    intIfNum @b{(input)} interface number
*           learnEnable: Learn state (L7_TRUE or L7_FALSE)
*
* @returns  L7_SUCCESS change is sucessfull
* @returns  L7_FAILURE if error
*
* @end
*********************************************************************/
L7_RC_t dot1sPortMacLearningGet(L7_uint32 intIfNum, L7_BOOL *learnEnable)
{
  return dtlDot1sLearnModeGet(intIfNum, learnEnable);
}
// PTin end


/*********************************************************************
* @purpose  Check if this port is Forwarding in any MSTP instance or
*           Manual Forwarding by NIM when MSTP is disabled.
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  L7_SUCCESS if forwarding in any instance or Nim Manual Forwarding
* @returns  L7_FAILURE if not forwarding in all instances
*
* @comments Use dot1sMstiPortStateGet if the state of a port in a specific 
*           instance is required. 
*
* @end
*********************************************************************/
L7_RC_t dot1sPortForwardingCheck(L7_uint32 intIfNum)
{
  L7_uint32 index, state;
  DOT1S_PORT_COMMON_t *pPort;

  /* Only return port forwarding state information if MSTP is enabled.
   */
  if (dot1sModeGet() == L7_ENABLE)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      for (index = 0; index <= L7_MAX_MULTIPLE_STP_INSTANCES; index++)
      {
        if (dot1sInstanceMap[index].inUse == L7_TRUE)
	{
          if(pPort->portInstInfo[index].portState == L7_DOT1S_FORWARDING ||
             pPort->portInstInfo[index].portState == L7_DOT1S_MANUAL_FWD)
          {
            return(L7_SUCCESS);
          }
        }
      }
    }
  }
  else /* MSTP is disabled, return NIM Manual Forwarding status */
  {
    if (nimGetIntfActiveState(intIfNum, &state) == L7_SUCCESS)
    {
      if (state == L7_ACTIVE)
      {
        return(L7_SUCCESS);
      }
    }
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Port Role of the port in a specific
*           instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current forwarding state
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiPortRoleGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index, role;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      switch (pPort->portInstInfo[index].role)
      {
      case ROLE_DISABLED: 
        /* DisabledPort */
        role = L7_DOT1S_ROLE_DISABLED;
        break;

      case ROLE_ROOT:
        /* RootPort */
        role = L7_DOT1S_ROLE_ROOT;
        break;

      case ROLE_DESIGNATED:
        /* DesignatedPort */
        role = L7_DOT1S_ROLE_DESIGNATED;
        break;

      case ROLE_ALTERNATE:
        /* AlternatePort */
        role = L7_DOT1S_ROLE_ALTERNATE;
        break;

      case ROLE_BACKUP:
        /* BakupPort */
        role = L7_DOT1S_ROLE_BACKUP;
        break;

      case ROLE_MASTER:
        /* MasterPort */
        role = L7_DOT1S_ROLE_MASTER;
        break;

      default:
        /* error */
        role = L7_DOT1S_ROLE_DISABLED;
        break;
      } /* end switch(p->portInstInfo[index].role) */

      return(role);
    } /* end if (p != L7_NULLPTR) */
  } /* end if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS) */

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MTSI associated with the specified VID
*          
* @param    VID @b{(input)} the desired VLAN ID
*
* @returns  the MSTID of the associated instance
*
* @comments All VLAN IDs by default will be associated with the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sVlanToMstiGet(L7_uint32 VID)
{
  return(dot1sInstVlanMap[VID].instNumber);
}

/*********************************************************************
* @purpose  Set the MSTP Port Path Cost in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*           Setting the Port Path Cost to '0' or 'auto' will also cause
*           the Port Path Cost mode to be set to L7_TRUE. Setting it to 
*           any other value will set the Port Path Cost mode to L7_FALSE.
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiPortPathCostSet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_uint32 event;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;
  L7_uint32 index;

  if ((val >= L7_DOT1S_MIN_PORT_PATHCOST) && (val <= L7_DOT1S_MAX_PORT_PATHCOST))
  {
    if (dot1sInstCheckInUse(mstID) == L7_SUCCESS)
    {
      if (mstID != DOT1S_CIST_ID)
      {
          event = instancePortPathCostSet;
      }
      else 
      { 
          event = commonPortPathCostSet;
      }
	  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
      if (pPortCfg != L7_NULLPTR)
      {
		if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
		{
		  pPortCfg->portInstInfo[index].InternalPortPathCost = val;
	  if(mstID == DOT1S_CIST_ID)
	  {
	    pPortCfg->portInstInfo[index].ExternalPortPathCost= val;
	    if (val == L7_DOT1S_AUTO_PORT_PATHCOST)
	      pPortCfg->portInstInfo[index].autoExternalPortPathCost = L7_TRUE;
	    else
	      pPortCfg->portInstInfo[index].autoExternalPortPathCost = L7_FALSE;
	  }
		  if (val == L7_DOT1S_AUTO_PORT_PATHCOST)
		  {
			pPortCfg->portInstInfo[index].autoInternalPortPathCost = L7_TRUE;
		  }
		  else
		  {
			pPortCfg->portInstInfo[index].autoInternalPortPathCost = L7_FALSE;
		  }
		}
		else
		  return L7_FAILURE;
      }
      /* issue the command */
      return(dot1sIssueCmd(event, intIfNum, mstID, &val));
    }
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Port Path Cost in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiPortPathCostGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
   	  return pPortCfg->portInstInfo[index].InternalPortPathCost;
   	}
   	else
	  return L7_NULL;
  }
  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MSTP Port Path Cost Mode in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_BOOL dot1sMstiPortPathCostModeGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
   	  return pPortCfg->portInstInfo[index].autoInternalPortPathCost;
   	}
   	else
	  return L7_FALSE;
  }
  
  return(L7_FALSE);
}

/*********************************************************************
* @purpose  Set the MSTP Port Priority in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiPortPrioritySet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_uint32 event;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;
  L7_uint32 index;

  if ((val >= L7_DOT1S_PORT_PRIORITY_MIN) && (val <= L7_DOT1S_PORT_PRIORITY_MAX))
  {
    if (dot1sInstCheckInUse(mstID) == L7_SUCCESS)
    {
      if (mstID != DOT1S_CIST_ID)
      {
        event = instancePortPrioritySet;
      }
      else 
      {
        event = commonPortPrioritySet;
      }

      pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
	  if (pPortCfg != L7_NULLPTR)
	  {
		if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
		{
		  pPortCfg->portInstInfo[index].portPriority = val;
		}
		else
		  return L7_FAILURE;
	  }
	  else
		return L7_FAILURE;

      /* issue the command */
      return(dot1sIssueCmd(event, intIfNum, mstID, &val));
    }
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Port Priority in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiPortPriorityGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
   	  return pPortCfg->portInstInfo[index].portPriority;
   	}
   	else
	  return L7_NULL;
  }
  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MSTP Port Identifier of a specific port in a
*           specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the port ID
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiPortIDGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      return(pPort->portInstInfo[index].portId);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Create a MSTP instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments total created should not exceed (L7_MAX_MULTIPLE_STP_INSTANCES - 1)
*           0 is not allowed as an mstID
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiCreate(L7_uint32 mstID)
{
  DOT1S_BRIDGE_t *pBridge;

  pBridge = dot1sBridgeParmsFind();
  if (pBridge != L7_NULLPTR)
  {
    /* Ensure that the ID is within the allowable range and that adding
     * this instance will not exceeded the maximum number supported.
     */
    if ((mstID >= L7_DOT1S_MSTID_MIN) && (mstID <= L7_DOT1S_MSTID_MAX) && 
        (pBridge->instanceCount < L7_MAX_MULTIPLE_STP_INSTANCES))
    {
      if (dot1sInstCheckInUse(mstID) != L7_SUCCESS)
      {
        
        /* mstID is not already in use */
        dot1sCfg->hdr.dataChanged = L7_TRUE;
        /* issue the command */
        return(dot1sIssueCmd(instanceCreate, L7_NULL, mstID, L7_NULLPTR));
      }
    }
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Delete a MSTP instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 0 is not allowed as an mstID
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiDelete(L7_uint32 mstID)
{
  /* Ensure that the ID is within the allowable range
   */
  if ((mstID >= L7_DOT1S_MSTID_MIN) && (mstID <= L7_DOT1S_MSTID_MAX))
  {
    if (dot1sInstCheckInUse(mstID) == L7_SUCCESS)
    {
      /* found the mstid in the list */
      /* issue the command */
      dot1sCfg->hdr.dataChanged = L7_TRUE;

      return(dot1sIssueCmd(instanceDelete, L7_NULL, mstID, L7_NULLPTR));
    }
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Add an association between a VLAN and an MSTP instance
*          
* @param    mstID  @b{(input)} the MSTID for the desired MTSI
* @param    vlanID @b{(input)} the VLAN ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments The VLAN ID will no longer be associated with the CIST.
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiVlanAdd(L7_uint32 mstID, L7_uint32 vlanID)
{
  L7_BOOL instInUse = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;

  if (dot1sInstCheckInUse(mstID) == L7_SUCCESS)
  {
    instInUse = L7_TRUE;
  }
  else
  {
    dot1sApiQueueSynchronize();
    instInUse = (dot1sInstCheckInUse(mstID) == L7_SUCCESS);
  }

  if (instInUse == L7_TRUE)
  {
    if ((vlanID > L7_DOT1Q_NULL_VLAN_ID) && (vlanID <= L7_DOT1Q_MAX_VLAN_ID))
    {
      /* Only allow the vlan to be associated if it already
       * exists. 
       */
      (void) osapiSemaTake(dot1sTaskSyncSema, L7_WAIT_FOREVER);
      if (dot1qVlanCheckValid(vlanID) == L7_SUCCESS)
      {
        rc = dot1sUserInstanceVlanAdd(mstID, vlanID);

      }
	  (void) osapiSemaGive(dot1sTaskSyncSema);
	  
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Remove an association between a VLAN and an MSTP instance
*          
* @param    mstID  @b{(input)} the MSTID for the desired MSTI
* @param    vlanID @b{(input)} the VLAN ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments The VLAN ID will again be associated with the CIST.
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiVlanRemove(L7_uint32 mstID, L7_uint32 vlanID)
{
  if (dot1sInstCheckInUse(mstID) == L7_SUCCESS)
  {
    if ((vlanID > L7_DOT1Q_NULL_VLAN_ID) && (vlanID <= L7_DOT1Q_MAX_VLAN_ID))
    {
        /* issue the command */
        return(dot1sIssueCmd(instanceVlanRemove, L7_NULL, mstID, &vlanID));
    }
  }

  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Check if the specified MSTP instance exists
*          
* @param    mstID  @b{(input)} MSTP instance number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sInstanceGet(L7_uint32 mstID)
{
  L7_uint32 index;

  /* check validity of the mstID */
  if ((mstID < L7_DOT1S_MSTID_MIN) || (mstID > L7_DOT1S_MSTID_MAX))
  {
    return L7_FAILURE;
  }

  return dot1sInstIndexFind(mstID, &index);
}

/*********************************************************************
* @purpose  Get the first MSTP instances number
*          
* @param    
*
* @returns  the first MSTP instance number
*
* @comments The first MSTP instance will always be the CIST
*
* @end
*********************************************************************/
L7_uint32 dot1sInstanceFirstGet()
{
  L7_uint32 i;

  /* loop through the instance map looking for first one in use  */
  for (i = 0; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if (dot1sInstanceMap[i].inUse == L7_TRUE)
    {
      return(dot1sInstanceMap[i].instNumber);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the next MSTP instances ID in sorted order
*          
* @param    prevInstNumber @b{(input)} previous instance number
*
* @returns  the next MSTP instance number or L7_NULL if none
*
* @comments the first MSTP instance ID will be returned if L7_NULL is
*           specified as the prevInstNumber
*           The CIST ID will not be returned from this function.
*
* @end
*********************************************************************/
L7_uint32 dot1sInstanceNextGet(L7_uint32 prevInstNumber)
{
  L7_uint32 instIndex, bestNextInstNum = L7_NULL;

  /* check validity of the prevInstNumber */
  if (prevInstNumber > L7_DOT1S_MSTID_MAX)
  {
    return(L7_NULL);
  }

  /* loop through the instance map looking for one in use that
   * has an instance ID that is after the previous one specified.
   * ie; sorted order from smallest to largest
   */
  for (instIndex = 1; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
    {
      if (dot1sInstanceMap[instIndex].instNumber > prevInstNumber)
      {
        if ((bestNextInstNum == L7_NULL) || 
            (dot1sInstanceMap[instIndex].instNumber < bestNextInstNum))
        {
          bestNextInstNum = dot1sInstanceMap[instIndex].instNumber;
        }
      }
    }
  }

  return(bestNextInstNum);
}

/*********************************************************************
* @purpose  List all of the created MSTP instances
*          
* @param    buf @b{(output)} pointer to a buffer for a list of FIDs
* @param    val @b{(output)} count of the actual number of FIDs in the list
*
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sInstanceList(L7_uint32 buf[], L7_uint32 *val)
{
  L7_uint32 i, j = 0;

  /* loop through the instance map looking for ones in use  */
  for (i = 0; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if (dot1sInstanceMap[i].inUse == L7_TRUE)
    {
      buf[j] = dot1sInstanceMap[i].instNumber;
      j++;
    }
  }
  *val = j;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Get the first of the Forwarding Database ID's associated 
*           with the specified MSTP instance ID
*          
* @param    mstID @b{(input)} the MSTP instance in question
*
* @returns  the first FID associated with the MSTP 
*           instance ID or L7_NULL if none found.
*
* @comments an MSTID of zero will represent the CIST
*           VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiFIDFirstGet(L7_uint32 mstID)
{
  L7_uint32 id;

  /* loop through the instance VLAN map looking for the first FID
   * (actually VID) that matches the specified instance.
   */
  for (id = 1; id <= L7_DOT1Q_MAX_VLAN_ID; id++)
  {
    /* For the CIST, Only return the FID if dot1q has it a 
     * corresponding VID created.
     * For the instances, return the FID regardless, though it
     * should correspond to what dot1q has.
     */ 
    if (((L7_uint32)dot1sInstVlanMap[id].instNumber == mstID) &&
        ((mstID != DOT1S_CIST_ID) || ((mstID == DOT1S_CIST_ID) && 
         (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask, id)))))
    {
      return(id);
    }
  }

  return(L7_NULL);                               
}

/*********************************************************************
* @purpose  Get the next of the Forwarding Database ID's associated 
*           with the specified MSTP instance ID
*          
* @param    prevFID @b{(input)} previous FID
* @param    mstID   @b{(input)} the MSTP instance in question
*
* @returns  the next FID or L7_NULL if no more found
*
* @comments an MSTID of zero will represent the CIST
            VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiFIDNextGet(L7_uint32 mstID, L7_uint32 prevFID)
{
  L7_uint32 id;

  /* No real previous specificed so return the first */
  if (prevFID == L7_NULL)
  {
    return(dot1sMstiFIDFirstGet(mstID));
  }

  /* loop through the instance VLAN map looking for FID's 
   * (actually VIDs) that match specified instance.
   */
  for (id = prevFID + 1; id <= L7_DOT1Q_MAX_VLAN_ID; id++)
  {
    /* For the CIST, Only return the FID if dot1q has it a 
     * corresponding VID created.
     * For the instances, return the FID regardless, though it
     * should correspond to what dot1q has.
     */ 
    if (((L7_uint32)dot1sInstVlanMap[id].instNumber == mstID) &&
        ((mstID != DOT1S_CIST_ID) || ((mstID == DOT1S_CIST_ID) && 
         (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask, id)))))
    {
      return(id);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  List all of the Forwarding Database ID's associated with 
*           an MSTP instance
*          
* @param    mstID @b{(input)}  the MSTID for the desired MSTI
* @param    buf   @b{(output)} pointer to a buffer for a list of FIDs
* @param    val   @b{(output)} count of the actual number of FIDs in the list
*
* @returns  L7_SUCCESS  
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiFIDList(L7_uint32 mstID, L7_uint32 buf[], L7_uint32 *val)
{
  L7_uint32 id, j = 0;

  /* loop through all FIDs (actually VIDs) looking for a matching mstID */
  for (id = 1; id <= L7_DOT1Q_MAX_VLAN_ID; id++)
  {
    /* For the CIST, Only return the FID if dot1q has it a 
     * corresponding VID created.
     * For the instances, return the FID regardless, though it
     * should correspond to what dot1q has.
     */ 
    if (((L7_uint32)dot1sInstVlanMap[id].instNumber == mstID) &&
        ((mstID != DOT1S_CIST_ID) || ((mstID == DOT1S_CIST_ID) && 
         (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask, id)))))
    {
      buf[j] = id;
      j++;
    }
  }

  *val = j;
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Get the first of the VLAN ID's associated with the 
*           specified MSTP instance ID
*          
* @param    mstID @b{(input)} the MSTP instance in question
*
* @returns  the first VID associated with the MSTP 
*           instance ID or L7_NULL if none found.
*
* @comments an MSTID of zero will represent the CIST
*           VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiVIDFirstGet(L7_uint32 mstID)
{
  L7_uint32 id;

  /* loop through the instance VLAN map looking for the first VID
   * that matches the specified instance.
   */
  for (id = 1; id <= L7_DOT1Q_MAX_VLAN_ID; id++)
  {
    /* For the CIST, Only return the FID if dot1q has it a 
     * corresponding VID created.
     * For the instances, return the FID regardless, though it
     * should correspond to what dot1q has.
     */ 
    if (((L7_uint32)dot1sInstVlanMap[id].instNumber == mstID) &&
        ((mstID != DOT1S_CIST_ID) || ((mstID == DOT1S_CIST_ID) && 
         (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask, id)))))
    {
      return(id);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the next of the VLAN ID's associated with the 
*           specified MSTP instance ID
*          
* @param    prevVID @b{(input)} previous VID
* @param    mstID   @b{(input)} the MSTP instance in question
*
* @returns  the next VID, or L7_NULL if no more found
*
* @comments an MSTID of zero will represent the CIST
*           VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiVIDNextGet(L7_uint32 mstID, L7_uint32 prevVID)
{
  L7_uint32 id;

  /* No real previous specified so return the first */
  if (prevVID == L7_NULL)
  {
    return(dot1sMstiVIDFirstGet(mstID));
  }

  /* loop through the instance VLAN map looking for VID's 
   * that match specified instance.
   */
  for (id = prevVID + 1; id <= L7_DOT1Q_MAX_VLAN_ID; id++)
  {
    /* For the CIST, Only return the VID if dot1q has it created.
     * For the instances, return the VID regardless, though it
     * should correspond to what dot1q has.
     */ 
    if (((L7_uint32)dot1sInstVlanMap[id].instNumber == mstID) &&
        ((mstID != DOT1S_CIST_ID) || ((mstID == DOT1S_CIST_ID) && 
         (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask, id)))))
    {
      return(id);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  List all of the VLAN ID's associated with an MSTP instance
*          
* @param    mstID @b{(input)}  the MSTID for the desired MTSI
* @param    buf   @b{(output)} pointer to a buffer for a list of VIDs
* @param    val   @b{(output)} count of the actual number of VIDs in the list
*
* @returns  L7_SUCCESS  
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiVIDList(L7_uint32 mstID, L7_ushort16 *buf, L7_uint32 *val)
{
  L7_uint32 id, j = 0;

  /* loop through all VIDs looking for a matching mstID */
  for (id = 1; id <= L7_DOT1Q_MAX_VLAN_ID; id++)
  {
    /* For the CIST, Only return the VID if dot1q has it created.
     * For the instances, return the VID regardless, though it
     * should correspond to what dot1q has.
     */ 
    if (((L7_uint32)dot1sInstVlanMap[id].instNumber == mstID) && 
        ((mstID != DOT1S_CIST_ID) || ((mstID == DOT1S_CIST_ID) && 
         (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask, id)))))
    {
      buf[j] = (L7_ushort16)id;
      j++;
    }
  }

  *val = j;
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Get the Base Mac Address
*          
* @param    buf @b{(output)} Base Mac Address
*
* @returns  
*
* @comments
*
*
* @end
*********************************************************************/
void dot1sBaseMacAddrGet(L7_uchar8 *buf)
{
    if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    {
        simGetSystemIPBurnedInMac(buf);
    }
    else
    {
        simGetSystemIPLocalAdminMac(buf);
    }
}

/*********************************************************************
* @purpose  Get the MSTP Bridge Identifier in a specific instance
*          
* @param    mstID @b{(input)}  the MSTID for the desired MTSI
* @param    buf   @b{(output)} Bridge Identifier
* @param    len   @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiBridgeIdentifierGet(L7_uint32 mstID, L7_uchar8 *buf, L7_uint32 *len)
{
  L7_uint32 index;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    if (mstID != DOT1S_CIST_ID)
    {
      dot1sBridgeIdEndianSafe(&dot1sInstance->msti[index].BridgeIdentifier, buf);

    }
    else
    {
      dot1sBridgeIdEndianSafe(&dot1sInstance->cist.BridgeIdentifier, buf);
    }

    *len = (L7_uint32)sizeof(DOT1S_BRIDGEID_t);

    return(L7_SUCCESS);
  }

  *len = L7_NULL;
  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP CIST Root Bridge Identifier
*          
* @param    buf @b{(output)} Bridge Identifier
* @param    len @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sCistRootIDGet(L7_uchar8 *buf, L7_uint32 *len)
{
  dot1sBridgeIdEndianSafe(&dot1sInstance->cist.cistRootPriority.rootId, buf);

  *len = (L7_uint32)sizeof(DOT1S_BRIDGEID_t);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Get the MSTP Regional Root Bridge Identifier in a specific 
*           instance
*          
* @param    mstID @b{(input)}  the MSTID for the desired MTSI
* @param    buf   @b{(output)} Bridge Identifier
* @param    len   @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiRegionalRootIDGet(L7_uint32 mstID, L7_uchar8 *buf, L7_uint32 *len)
{
  L7_uint32 index;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    if (mstID != DOT1S_CIST_ID)
    {
      dot1sBridgeIdEndianSafe(&dot1sInstance->msti[index].mstiRootPriority.regRootId, buf);
    }
    else
    {
      dot1sBridgeIdEndianSafe(&dot1sInstance->cist.cistRootPriority.regRootId, buf);
    }
	  
    *len = (L7_uint32)sizeof(DOT1S_BRIDGEID_t);

    return(L7_SUCCESS);
  }

  *len = L7_NULL;
  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP CIST Root Path Cost
*          
* @param    
*
* @returns current value of the Path Cost
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 dot1sCistRootPathCostGet()
{
    return((L7_uint32)dot1sInstance->cist.cistRootPriority.extRootPathCost);
}

/*********************************************************************
* @purpose  Get the MSTP Regional Root Path Cost in a specific 
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  current value of the Path Cost or L7_NULL if the specified
*           instance is not found.
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiRegionalRootPathCostGet(L7_uint32 mstID)
{
  L7_uint32 index;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    if (mstID != DOT1S_CIST_ID)
    {
      return((L7_uint32)dot1sInstance->msti[index].mstiRootPriority.intRootPathCost);
    }
    else
    {
      return((L7_uint32)dot1sInstance->cist.cistRootPriority.intRootPathCost);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MSTP Time since the last Topology Change in a
*           specific instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  time in seconds since last TCN
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiTimeSinceTopologyChangeGet(L7_uint32 mstID)
{
  L7_uint32 index, tcTime;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    if (mstID != DOT1S_CIST_INDEX)
    {
      tcTime = dot1sInstance->msti[index].tcWhileChangeTime;
    }
    else
    {
      tcTime = dot1sInstance->cist.tcWhileChangeTime;
    }

    if (tcTime == (L7_uint32)0xFFFFFFFF)
    {
      /* Not a valid time so zero it out */
      tcTime = L7_NULL;
    }
      /* valid time so calculate the difference */
      tcTime = osapiUpTimeRaw() - tcTime;
    

    return(tcTime);
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MSTP count of Topology Changes in a specific 
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  number of TCNs received
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiTopologyChangeCountGet(L7_uint32 mstID)
{
  L7_uint32 index;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    if (mstID != DOT1S_CIST_ID)
    {
      return(dot1sInstance->msti[index].tcWhileChange);
    }
    else
    {
      return(dot1sInstance->cist.tcWhileChange);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MSTP Topology Change parameter in a specific
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  the current value of the TCN parm
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_BOOL dot1sMstiTopologyChangeParmGet(L7_uint32 mstID)
{
  L7_uint32 intIfNum, index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    for (intIfNum = 1; intIfNum <= (L7_uint32)L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      pPort = dot1sIntfFind(intIfNum);
      if ((pPort != L7_NULLPTR) && (pPort->portInstInfo[index].tcWhile != 0))
      {
        return(L7_TRUE);
      }
    }
  }

  return(L7_FALSE);
}

/*********************************************************************
* @purpose  Get the MSTP Topology Change Acknowledge parameter for a
*           specific port in the CIST
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parm
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL dot1sCistPortTopologyChangeAckGet(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFind(intIfNum);
  if (pPort != L7_NULLPTR)
  {
    return(pPort->tcAck);
  }

  return(L7_FALSE);
}

/*********************************************************************
* @purpose  Get the MSTP Root Port ID in a specific instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  Port ID of the Root Port
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiRootPortIDGet(L7_uint32 mstID)
{
  L7_uint32 index;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    if (mstID != DOT1S_CIST_ID)
    {
        return(dot1sInstance->msti[index].mstiRootPortId);
    }
    else
    {
        return(dot1sInstance->cist.cistRootPortId);
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MSTP Designated Cost of a specific port in a 
*           specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns current value of the Path Cost
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_uint32 dot1sMstiDesignatedCostGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      if (mstID != DOT1S_CIST_ID)
      {
        return(pPort->portInstInfo[index].inst.msti.mstiPortPriority.intRootPathCost);
      }
      else 
      {
        return(pPort->portInstInfo[index].inst.cist.cistPortPriority.extRootPathCost);
      }
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MSTP Designated Bridge ID of a specific port in a 
*           specific instance
*          
* @param    mstID    @b{(input)}  the MSTID for the desired MTSI
* @param    intIfNum @b{(input)}  interface number
* @param    buf      @b{(output)} Bridge ID
* @param    len      @b{(output)} length of Bridge ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiDesignatedBridgeIDGet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uchar8 *buf, L7_uint32 *len)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      if (mstID != DOT1S_CIST_ID)
      {
        dot1sBridgeIdEndianSafe(&pPort->portInstInfo[index].inst.msti.mstiPortPriority.dsgBridgeId, buf);
      }
      else 
      {
        dot1sBridgeIdEndianSafe(&pPort->portInstInfo[index].inst.cist.cistPortPriority.dsgBridgeId, buf);
      }

      *len = (L7_uint32)sizeof(DOT1S_BRIDGEID_t);

      return(L7_SUCCESS);
    }
  }

  *len = L7_NULL;
  return(L7_FAILURE);
}

/*********************************************************************
* @purpose  Get the MSTP Designated Port ID of a specific port in a 
*           specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the value of the port ID
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
L7_ushort16 dot1sMstiDesignatedPortIDGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      if (mstID != DOT1S_CIST_ID)
      {
		return(pPort->portInstInfo[index].inst.msti.mstiPortPriority.dsgPortId);
	  }
	  else/*use CIST*/
	  {
		return(pPort->portInstInfo[index].inst.cist.cistPortPriority.dsgPortId);
	  }
    }
  }

  return(L7_NULL);
}

/*********************************************************************
* @purpose  Get the MSTP Statistics for STP BPDU's Received on a 
*           specific port
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP stat  
*
* @returns  the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sPortStatsSTPBPDUsReceivedGet(L7_uint32 intIfNum)
{
  return(dot1sPortStats[intIfNum].stpRx);
}

/*********************************************************************
* @purpose  Get the MSTP Statistics for RSTP BPDU's Received on a 
*           specific port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sPortStatsRSTPBPDUsReceivedGet(L7_uint32 intIfNum)
{
  return(dot1sPortStats[intIfNum].rstpRx);
}

/*********************************************************************
* @purpose  Get the MSTP Statistics for MSTP BPDU's Received on a 
*           specific port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sPortStatsMSTPBPDUsReceivedGet(L7_uint32 intIfNum)
{
  return(dot1sPortStats[intIfNum].mstpRx);
}

/*********************************************************************
* @purpose  Get the MSTP Statistics for STP BPDU's Sent on a specific
*           port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sPortStatsSTPBPDUsSentGet(L7_uint32 intIfNum)
{
  return(dot1sPortStats[intIfNum].stpTx);
}

/*********************************************************************
* @purpose  Get the MSTP Statistics for RSTP BPDU's Sent on a specific
*           port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sPortStatsRSTPBPDUsSentGet(L7_uint32 intIfNum)
{
  return(dot1sPortStats[intIfNum].rstpTx);
}

/*********************************************************************
* @purpose  Get the MSTP Statistics for MSTP BPDU's Sent on a specific
*           port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sPortStatsMSTPBPDUsSentGet(L7_uint32 intIfNum)
{
  return(dot1sPortStats[intIfNum].mstpTx);
}

/*********************************************************************
* @purpose  Get the MSTP Statistics for the number of times this interface 
*           has transitioned into loop inconsistent state
* 
* @param    mstID    @b((input)) MST Instance ID          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the Loop Inconsistent State Start stat  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sPortStatsTransitionsIntoLoopInconsistentStateGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    return(dot1sPortStats[intIfNum].transitionsIntoLoopInconsistentState);
  }
  else
  {
    return(L7_NULL);
  }
}

/*********************************************************************
* @purpose  Get the MSTP Statistics for the number of times this interface 
*           has transitioned out of loop inconsistent state
* 
* @param    mstID    @b((input)) MST Instance ID          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the Loop Inconsistent State End stat  
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 dot1sPortStatsTransitionsOutOfLoopInconsistentStateGet(L7_uint32 mstID, L7_uint32 intIfNum)
{
  L7_uint32 index;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    return(dot1sPortStats[intIfNum].transitionsOutOfLoopInconsistentState);
  }
  else
  {
    return(L7_NULL);
  }
}

/*********************************************************************
* @purpose  Register a routine to be called when a link state changes.
*
* @param    registrar_ID   @b{(input)} routine registrar id  (See L7_COMPONENT_ID_t)      
* @param    *notify        @b{(input)} pointer to a routine to be invoked for link state     
*                                      changes.  Each routine has the following parameters:
*                                      (internal interface number, event(L7_UP, L7_DOWN,
*                                      etc.)). 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*                                 
* @end
*********************************************************************/
L7_RC_t dot1sRegisterIntfChange( L7_COMPONENT_IDS_t registrar_ID, 
                                L7_RC_t (*notify)(L7_uint32 mstID, 
                                                    L7_uint32 intIfNum, 
                                                    L7_uint32 event))
{
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("Dot1s registrar ID %u greater than Last Component ID\n", registrar_ID);
    return(L7_FAILURE);
  }

  if ((L7_uint32)dot1sNotifyList[registrar_ID].notify_intf_change != L7_NULL)
  {
    LOG_MSG("Dot1s registrar ID %u already registered\n", registrar_ID);
    return(L7_FAILURE);
  }

  dot1sNotifyList[registrar_ID].registrar_ID = registrar_ID;
  dot1sNotifyList[registrar_ID].notify_intf_change = notify;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Set the MSTP Path Cost per port speed setting or continue 
*           to use the current configured path cost
*
* @param    intIfNum @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function will also zero out the path cost if auto
*           calculate is enabled and the port is disabled.
*
*       
* @end
*********************************************************************/
L7_RC_t dot1sCalcPortPathCost(L7_uint32 intIfNum)
{
  L7_uint32 speed, cost = L7_NULL, instIndex;
  L7_INTF_TYPES_t type;
  DOT1S_PORT_COMMON_t *pPort;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  pPort = dot1sIntfFind(intIfNum);
  if (pPort == L7_NULLPTR)
  {
    return(L7_FAILURE);
  }

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if ((nimCheckIfNumber(intIfNum) == L7_SUCCESS) && 
      (pPort->portEnabled == L7_TRUE) &&
      (nimGetIntfSpeedStatus(intIfNum, &speed) == L7_SUCCESS))
  {
    switch (speed)
    {
      case L7_PORTCTRL_PORTSPEED_FULL_10T:
      case L7_PORTCTRL_PORTSPEED_HALF_10T:
        cost = 2000000;
        break;
     
	  case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
        cost = 200000;
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        cost = 20000;
        break;
      
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
        cost = 8000;
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
        cost = 2000;
         break;
             
	case L7_PORTCTRL_PORTSPEED_LAG:
        cost = 200;
        break;
      
	  default:
        cost = 200000;
        break;

    } /* end switch speed */
  
    /* Some hardware platforms do not return the correct speed for
     * lag interfaces so if the intIfNum is of type lag set the cost 
     * to 10000
     */
    if ((nimGetIntfType(intIfNum, &type) == L7_SUCCESS) &&
        (type == L7_LAG_INTF))
    {
      /*Adaptive Path Cost calculation for lag */
      if (dot1sLagAutoPathCalculation(intIfNum, &cost) != L7_SUCCESS)
      cost = 200;
    }
  } /*end if nimCheckIfNumber */
  for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
	if (pPort->portInstInfo[instIndex].autoInternalPortPathCost == L7_TRUE)
    {
      pPort->portInstInfo[instIndex].InternalPortPathCost = cost;
	  pPortCfg->portInstInfo[instIndex].InternalPortPathCost  = cost;

    } /* end if L7_DOT1S_AUTO_CALC_PATH_COST */

  } /* end for instIndex */
  if (pPort->portInstInfo[DOT1S_CIST_INDEX].autoExternalPortPathCost == L7_TRUE)
  {
	pPort->portInstInfo[DOT1S_CIST_INDEX].ExternalPortPathCost = cost;
	pPortCfg->portInstInfo[DOT1S_CIST_INDEX].ExternalPortPathCost = cost;
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose 	Check if a particular MSTP instance ID is in use
*
* @param    instNumber	@b{(input)} Instance ID  
*
* @returns  L7_SUCCESS 	if the instance number is found and inUse is L7_TRUE  
* @returns  L7_FAILURE 	if the instance number is not found
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t	dot1sInstCheckInUse(L7_uint32 mstID)
{
  L7_uint32 index;

  return(dot1sInstIndexFind(mstID, &index));
}

/*********************************************************************
* @purpose 	Check if this int if number is forwarding or learning in any instance
*			other than the one supplied.
*
* @param    instId	 @b{(input)} Instance ID  
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS 	if the instance number is found and inUse is L7_TRUE  
* @returns  L7_FAILURE 	if the instance number is not found
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL dot1sIsFwdInAnyOtherInst(L7_uint32 instId, L7_uint32 intIfNum)
{
  /*this routine finds out whether this port is fwd or lrn in any other inst */
  L7_uint32 instIndex,i;
  L7_RC_t rc;
  DOT1S_PORT_COMMON_t *pPort;

  rc = dot1sInstIndexFind(instId, &instIndex);
  pPort = dot1sIntfFind(intIfNum);
  if ((pPort == L7_NULLPTR) || (pPort->portNum == 0))
  {
	return L7_FALSE;
  }

  for (i = 0; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
	if ( i != instIndex && 
		 dot1sInstanceMap[i].inUse == L7_TRUE &&
		 (pPort->portInstInfo[i].portState == L7_DOT1S_FORWARDING ||
		  pPort->portInstInfo[instIndex].portState == L7_DOT1S_LEARNING))
	{
	  return L7_TRUE;
	}
  }

  return L7_FALSE;

}

/********************************************************************
* @purpose 	Check if this intIf number is a member of a vlan associated
*			with the specified instance.
*
* @param    instId	 @b{(input)} Instance ID  
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL dot1sIsIntIfMemberOfInstanceVlans(L7_uint32 instId, 
                                          L7_uint32 intIfNum)
{
  L7_uint32 instIndex; 
  L7_RC_t rc;
  L7_uint32 mode, vid, prevVID = L7_NULL;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(instId, &instIndex) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if ((pPort != L7_NULLPTR) && (pPort->portNum != L7_NULL))
    {
      while ((vid = dot1sMstiVIDNextGet(instId, prevVID)) != L7_NULL)
      {
        prevVID = vid;
        rc = dot1qOperVlanMemberGet(vid, intIfNum, &mode);
        if (mode == L7_DOT1Q_FIXED)
        {
          return L7_TRUE;
        }
      }
	  }
  }

  return L7_FALSE;

}
/*********************************************************************
* @purpose  callback function for interface state query 
*          
* @param    intfNum    @b{(input)} Internal Interface Number
* @param    dtlEvent   @b{(input)} DTL_EVENT_t DTL_EVENT_ADDR_INTF_MAC_QUERY
* @param    data       @b{(input)} pointer to MSTP instance ID 
* @param    stateQurey @b{(input)} pointer to bool to set on exit 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t dot1sInstStateQueryCallback(L7_uint32 intIfNum, 
									DTL_EVENT_t dtlEvent, 
									void *data, 
									L7_BOOL *stateQuery)
{
  L7_uint32 instId;
  
  if (dot1sDeregister.dot1sInstStateQueryCallback == L7_TRUE)
  {
	LOG_MSG("dot1sInstStateQueryCallback is deregistered\n");
	return L7_FAILURE;
  }
  if (!(DOT1S_IS_READY))
  {
	LOG_MSG("Received a vlan callback while outside the EXECUTE state");
    return L7_FAILURE;
  }

  instId = *(L7_uint32 *)data;
  *stateQuery = dot1sIsIntIfMemberOfInstanceVlans(instId, intIfNum);
  
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Notify MSTP of a change to the Mac Address. 
*          
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If the Base Mac Address has been locally admistrated or 
*           switched back to the burned in value, dot1s needs to be 
*           notified.
*
* @end
*********************************************************************/
L7_RC_t dot1sMacAddrChangeNotify()
{
  /* issue the command */
  return(dot1sIssueCmd(switchMacAddrChange, L7_NULL, L7_NULL, L7_NULLPTR));
}
/*********************************************************************
* @purpose  Clears the port statistics. 
*          
* @param    intIfNum @b{(input)} Internal Interface Number of the port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Clears all the BPDU stats associated with this port. (STP RSTP & MSTP)
*
* @end
*********************************************************************/
L7_RC_t dot1sPortStatsClear(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;

  dot1sPortStats[intIfNum].stpRx = L7_NULL;
  dot1sPortStats[intIfNum].rstpRx = L7_NULL;
  dot1sPortStats[intIfNum].mstpRx = L7_NULL;
  dot1sPortStats[intIfNum].stpTx = L7_NULL;
  dot1sPortStats[intIfNum].rstpTx = L7_NULL;
  dot1sPortStats[intIfNum].mstpTx = L7_NULL;
  dot1sPortStats[intIfNum].rxDiscards = L7_NULL;
  dot1sPortStats[intIfNum].txDiscards = L7_NULL;

  dot1sPortStats[intIfNum].transitionsIntoLoopInconsistentState = L7_NULL;
  dot1sPortStats[intIfNum].transitionsOutOfLoopInconsistentState = L7_NULL;   

  return rc;
}
/*********************************************************************
* @purpose  Get the MSTP Designated Root ID of a specific port in a 
*           specific instance
*          
* @param    mstID    @b((input))  the MSTID for the desired MTSI
* @param    intIfNum @b((input))  interface number
* @param    buf      @b((output)) Root Bridge ID
* @param    len      @b((output)) length of Bridge ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiPortDesignatedRootIDGet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uchar8 *buf, L7_uint32 *len)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      if (mstID != DOT1S_CIST_ID)
      {
        dot1sBridgeIdEndianSafe(&pPort->portInstInfo[index].inst.msti.mstiPortPriority.regRootId, buf);
      }
      else 
      {
        dot1sBridgeIdEndianSafe(&pPort->portInstInfo[index].inst.cist.cistPortPriority.rootId, buf);
      }

      *len = (L7_uint32)sizeof(DOT1S_BRIDGEID_t);

      return(L7_SUCCESS);
    }
  }

  *len = L7_NULL;
  return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Get the CIST Regional Root ID of a specific port
*          
* @param    intIfNum @b((input))  interface number
* @param    buf      @b((output)) CIST Regional Root ID
* @param    len      @b((output)) length of Bridge ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortRegionalRootIDGet(L7_uint32 intIfNum, L7_uchar8 *buf, L7_uint32 *len)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(DOT1S_CIST_ID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      dot1sBridgeIdEndianSafe(&pPort->portInstInfo[index].inst.cist.cistPortPriority.regRootId, buf);
      
      *len = (L7_uint32)sizeof(DOT1S_BRIDGEID_t);

      return(L7_SUCCESS);
    }
  }

  *len = L7_NULL;
  return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Get the MSTP Port Hello Time parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)}  interface number
* @param    val      @b{(output)} pointer to hello time value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Hello time is stored in multiples of 256
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortHelloTimeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFind(intIfNum);
  if (pPort == L7_NULLPTR)
  {
	return L7_FAILURE;
  }
  *val = pPort->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.helloTime / DOT1S_TIMER_UNIT;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the MSTP Port Path Cost parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)}  interface number
* @param    val      @b{(output)} pointer to CIST Path Cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortPathCostGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(DOT1S_CIST_ID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      *val = pPort->portInstInfo[index].inst.cist.cistDesignatedPriority.intRootPathCost;
      
	  return(L7_SUCCESS);
    }
  }
  return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Get the MSTP Max Hop count parameter for the device
*          
* @param    val      @b{(output)} pointer to Max Hop Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sBridgeMaxHopCountGet(L7_uint32 *val)
{
  *val = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.maxHops;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the MSTP Max Hop count parameter for the device
*          
* @param    val      @b{(output)} Max Hop Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sBridgeMaxHopCountSet(L7_uint32 val)
{
  if (val > 0)
  {
    dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.maxHops = val;
	dot1sCfg->hdr.dataChanged = L7_TRUE;
	/* issue the command */
    return(dot1sIssueCmd(commonBridgeMaxHopSet, L7_NULL, L7_NULL, (void *)&val));
  }
  return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Get the MSTP Tx Hold count parameter for the device
*          
* @param    val      @b{(output)} pointer to Tx Hold Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sBridgeTxHoldCountGet(L7_uint32 *val)
{
  *val = dot1sCfg->cfg.dot1sBridge.TxHoldCount ;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the MSTP Tx Hold count parameter for the device
*          
* @param    val      @b{(input)} Tx Hold Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sBridgeTxHoldCountSet(L7_uint32 val)
{

  /* Validate */
  if ((val > L7_DOT1S_BRIDGE_HOLDCOUNT_MAX) ||
	  (val < L7_DOT1S_BRIDGE_HOLDCOUNT_MIN))
  {
	  
	  return L7_FAILURE;
  }
  dot1sCfg->cfg.dot1sBridge.TxHoldCount = val;
  dot1sCfg->hdr.dataChanged = L7_TRUE;
	/* issue the command */
  return(dot1sIssueCmd(commonBridgeTxHoldCountSet, L7_NULL, L7_NULL, (void *)&val));
  
}
/*********************************************************************
* @purpose  Get the Admin Hello Time for this port
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} pointer to the adminHelloTime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortAdminHelloTimeGet(L7_uint32 intIfNum, 
									   L7_uint32 *val)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;
  
  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
	*val = pPortCfg->HelloTime/DOT1S_TIMER_UNIT;
	return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Set the Admin Hello Time for this port
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} hello time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortAdminHelloTimeSet(L7_uint32 intIfNum, 
									   L7_uint32 val)
{

  L7_LOG(L7_LOG_SEVERITY_WARNING, L7_DOT1S_COMPONENT_ID,
                "Attempt to update hello time which is disallowed per IEEE 802.1Q-REV 2005");
 
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get the CIST External Path Cost for this port
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} pointer to path cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Use dot1sMstiPortPathCostGet with mstid 0 to get the 
*           internal port path cost for the CIST
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortExternalPathCostGet(L7_uint32 intIfNum,
										 L7_uint32 *val)
{
  DOT1S_PORT_COMMON_t *pPort;
  pPort = dot1sIntfFind(intIfNum);
  if (pPort != L7_NULLPTR)
  {
    *val = pPort->portInstInfo[DOT1S_CIST_INDEX].ExternalPortPathCost;
    
    return(L7_SUCCESS);
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Set the CIST External Path Cost for this port
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} path cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Use dot1sMstiPortPathCostSet with mstid 0 to set the 
*           internal port path cost for the CIST
*
* @end
*********************************************************************/
L7_RC_t dot1sCistPortExternalPathCostSet(L7_uint32 intIfNum,
										 L7_uint32 val)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;
  
  if ((val >= L7_DOT1S_MIN_PORT_PATHCOST) && (val <= L7_DOT1S_MAX_PORT_PATHCOST))
  {
    pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
    if (pPortCfg != L7_NULLPTR)
    {
	  pPortCfg->portInstInfo[DOT1S_CIST_INDEX].ExternalPortPathCost = val;
	  if (val == L7_DOT1S_AUTO_PORT_PATHCOST)
	  {
	  	pPortCfg->portInstInfo[DOT1S_CIST_INDEX].autoExternalPortPathCost = L7_TRUE;
	  }
      else
	  {
	  	pPortCfg->portInstInfo[DOT1S_CIST_INDEX].autoExternalPortPathCost = L7_FALSE;
	  }
	}
	else
	  return L7_FAILURE;
    return dot1sIssueCmd(commonPortExtPathCostSet,intIfNum,DOT1S_CIST_ID, (void *)&val);
  }
  
  return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Gets the ports begin reset state machine time
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} pointer to timer value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sPortStateMachineUpTimeGet(L7_uint32 intIfNum, L7_timespec *val)
{
  DOT1S_PORT_COMMON_t *p;
  L7_uint32 currentTime = 0;
  L7_uint32 uptime = 0;

  currentTime = osapiUpTimeRaw();
  p = dot1sIntfFind(intIfNum);
  
  if (p != L7_NULLPTR && p->tick <= currentTime)
  {
	uptime = currentTime - p->tick;
	osapiConvertRawUpTime(uptime,  val);
	return L7_SUCCESS;
  }
  else
	return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get CIST External Port Path Cost Mode in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments Use the dot1sMstiPortPathCostModeGet for the cist internal 
*           path cost mode.
*
*
* @end
*********************************************************************/
L7_BOOL dot1sCistExtPortPathCostModeGet(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
    return pPortCfg->portInstInfo[DOT1S_CIST_INDEX].autoExternalPortPathCost;
  }
  return(L7_FALSE);
}

/*********************************************************************
* @purpose 	Synchronize with the dot1s Queue.
*
* @param    @b{(input)} None 
*
* @returns  L7_SUCCESS 	if the instance number is found and inUse is L7_TRUE  
* @returns  L7_FAILURE 	if the instance number is not found
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t	dot1sApiQueueSynchronize()
{  
    L7_RC_t rc = dot1sIssueCmd(instanceQueueSynchronize, L7_NULL, L7_NULL, L7_NULLPTR);
    
    
    if (rc == L7_SUCCESS)
        dot1sQueueSyncSemGet();

    return rc;


}

/*********************************************************************
* @purpose  Determine if the interface type is valid in dot1s
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
L7_BOOL dot1sIsValidIntfType(L7_uint32 sysIntfType)
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
* @purpose  Determine if the interface is valid in dot1s
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
L7_BOOL dot1sIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_FALSE;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return dot1sIsValidIntfType(sysIntfType);
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get the MSTP Operational Auto Edge parameter of a 
*           specific port
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sAutoEdgeGet(L7_uint32 intIfNum, L7_BOOL *val)
{
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFind(intIfNum);
  if (pPort != L7_NULLPTR)
  {
    *val = pPort->autoEdge;
		return(L7_SUCCESS);
	}

	return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Set the MSTP Auto Edge Port parameter of a 
*           specific port.
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sAutoEdgeSet(L7_uint32 intIfNum, L7_BOOL val)
{
  L7_uint32 event;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  event = commonPortAutoEdgeSet;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
	pPortCfg->autoEdge = val;
	dot1sCfg->hdr.dataChanged = L7_TRUE;
  }
  else
	return L7_FAILURE;

  /* issue the command */
  return(dot1sIssueCmd(event, intIfNum, L7_NULL, &val));
}

/*********************************************************************
* @purpose  Get the MSTP Administrative value or the restricted role parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sRestrictedRoleGet(L7_uint32 intIfNum, L7_BOOL *val)
{
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFind(intIfNum);
  if (pPort != L7_NULLPTR)
  {
    *val = pPort->restrictedRole;
		return(L7_SUCCESS);
	}

	return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Set the MSTP Administrative value or the restricted role parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* @returns  L7_ERROR  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sRestrictedRoleSet(L7_uint32 intIfNum, L7_BOOL val)
{
  L7_uint32 event;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  event = commonPortRestrictedRoleSet;
  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  
  if (pPortCfg != L7_NULLPTR)
  {
    if ( (val == L7_TRUE) && (pPortCfg->loopGuard == L7_TRUE) )
    {
      LOG_MSG("Root Guard cannot be enabled on interface %d with enabled Loop Guard\n", intIfNum);    
      return L7_ERROR;
    }   
  	pPortCfg->restrictedRole = val;
  	dot1sCfg->hdr.dataChanged = L7_TRUE;
  }
  else
  {
  	return L7_FAILURE;
  }
  
  /* issue the command */
  return(dot1sIssueCmd(event, intIfNum, L7_NULL, &val));
}

/*********************************************************************
* @purpose  Get the MSTP Administrative value of the loopguard parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sLoopGuardGet(L7_uint32 intIfNum, L7_BOOL *val)
{
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFind(intIfNum);
  if (pPort != L7_NULLPTR)
  {
		*val = pPort->loopGuard;
    return(L7_SUCCESS);
  }

  return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Set the MSTP Administrative value of the loopguard parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* @returns  L7_ERROR  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sLoopGuardSet(L7_uint32 intIfNum, L7_BOOL val)
{
  L7_uint32 event;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  event = commonPortLoopGuardSet;
  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);

  if (pPortCfg != L7_NULLPTR)
  {
    if ( (val == L7_TRUE) && (pPortCfg->restrictedRole == L7_TRUE) )
    {
      LOG_MSG("Loop Guard  cannot be enabled on interface %d with enabled Root Guard\n", intIfNum);
      return L7_ERROR;
    }   
    pPortCfg->loopGuard = val;
    dot1sCfg->hdr.dataChanged = L7_TRUE;
  }
  else
  {
    return L7_FAILURE;
  }
  
  /* issue the command */
  return(dot1sIssueCmd(event, intIfNum, L7_NULL, &val));
}
/*********************************************************************
* @purpose  Get the MST Loop Inconsistent state of the port in a specific instance
*
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiPortLoopInconsistentStateGet(L7_uint32 mstID, L7_uint32 intIfNum, L7_BOOL *val)
{
  L7_uint32 index;
  DOT1S_PORT_COMMON_t *pPort;

  if (dot1sInstIndexFind(mstID, &index) == L7_SUCCESS)
  {
    pPort = dot1sIntfFind(intIfNum);
    if (pPort != L7_NULLPTR)
    {
      *val = pPort->loopInconsistent;
			return(L7_SUCCESS);
		}
  }

	return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Get the MSTP Administrative value or the restricted TCN parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sRestrictedTcnGet(L7_uint32 intIfNum, L7_BOOL *val)
{
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFind(intIfNum);
  if (pPort != L7_NULLPTR)
  {
    *val = pPort->restrictedTcn;
		return(L7_SUCCESS);
	}

  return(L7_FAILURE);
}
/*********************************************************************
* @purpose  Set the MSTP Administrative value or the restricted TCN parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sRestrictedTcnSet(L7_uint32 intIfNum, L7_BOOL val)
{
  L7_uint32 event;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;

  event = commonPortRestrictedTcnSet;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
	pPortCfg->restrictedTcn = val;
	dot1sCfg->hdr.dataChanged = L7_TRUE;
  }
  else
	return L7_FAILURE;

  /* issue the command */
  return(dot1sIssueCmd(event, intIfNum, L7_NULL, &val));
}
/*********************************************************************
* @purpose  Set BPDU Guard for the bridge
*
* @param    val      @b{(input)} the new value of the STP BPDU Guard
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduGuardSet(L7_BOOL val)
{
  /* Set the value when it is different from the old one. */
  if (val != dot1sCfg->cfg.dot1sBridge.bpduGuardMode)
  {
    dot1sCfg->cfg.dot1sBridge.bpduGuardMode = val;
    /* Config data has changed */
    dot1sCfg->hdr.dataChanged = L7_TRUE;
    dot1sIssueCmd(switchBpduGuardModeSet, L7_NULL, L7_NULL, &val);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the BPDU Guard for the brdige
*
* @param    val      @b{(output)} the new value of the STP BPDU Guard
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduGuardGet(L7_BOOL *val)
{
  if(dot1sCfg != L7_NULL)
  {
    *val = dot1sCfg->cfg.dot1sBridge.bpduGuardMode;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the BPDU Guard effect for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(output)} the new value of the STP BPDU Guard
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduGuardEffectGet(L7_uint32 intIfNum,L7_BOOL *val)
{
  DOT1S_PORT_COMMON_t *port;

  *val = L7_FALSE;
  port = dot1sIntfFind(intIfNum);
  if (port != L7_NULLPTR)
  {
    *val = port->bpduGuardEffect;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the BPDU filter for all the edge ports
*
* @param    val      @b{(input)} the new value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduFilterSet(L7_BOOL val)
{
  /* Set the value when it is different from the old one. */
  if (val != dot1sCfg->cfg.dot1sBridge.bpduFilterMode)
  {
    dot1sCfg->cfg.dot1sBridge.bpduFilterMode = val;
    /* Config data has changed */
    dot1sCfg->hdr.dataChanged = L7_TRUE;
    dot1sIssueCmd(switchBpduFilterModeSet, L7_NULL, L7_NULL, &val);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the BPDU filter for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(input)} the new value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfBpduFilterSet(L7_uint32 intIfNum, L7_BOOL val)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg = L7_NULLPTR;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  
  if (pPortCfg != L7_NULLPTR)
  {
    if (pPortCfg->bpduFilterMode != val)
    {
      pPortCfg->bpduFilterMode = val;
      /* Config data has changed */
      dot1sCfg->hdr.dataChanged = L7_TRUE;
      dot1sIssueCmd(commonPortBpduFilterModeSet, intIfNum, L7_NULL, &val);
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the BPDU filter for the switch
*
* @param    val      @b{(input)} the value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduFilterGet(L7_BOOL *val)
{
  if(dot1sCfg != L7_NULL)
  {
    *val = dot1sCfg->cfg.dot1sBridge.bpduFilterMode;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the BPDU filter for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(input)} the value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfBpduFilterGet(L7_uint32 intIfNum, L7_BOOL *val)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg = L7_NULLPTR;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
    *val = pPortCfg->bpduFilterMode;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the BPDU flood for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(input)} the new value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfBpduFloodSet(L7_uint32 intIfNum, L7_BOOL val)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL done = L7_FALSE, sendMsg = L7_FALSE;
  L7_uint32 curr= 0, next;

  if (intIfNum == L7_ALL_INTERFACES)
  {
      if (nimFirstValidIntfNumber(&curr) != L7_SUCCESS)
      {
          LOG_MSG("Cannot get first valid NIM Interface number \n");
          return L7_FAILURE;
      }
  }
  else
  {
      curr = intIfNum;
      done = L7_TRUE;
  }

  do
  {

    pPortCfg = dot1sIntfCfgEntryGet(curr);
  
    if (pPortCfg != L7_NULLPTR)
    {
      if (pPortCfg->bpduFloodMode != val)
      {
        /* set the flood mode*/
        pPortCfg->bpduFloodMode = val;
        /* Config data has changed */
        dot1sCfg->hdr.dataChanged = L7_TRUE;
        rc = L7_SUCCESS;
  
      }
      sendMsg = L7_TRUE;
    }

    rc = nimNextValidIntfNumber(curr,&next);
	if ( rc != L7_SUCCESS)
	{
	  done = L7_TRUE;
	}
	else
	  curr = next;
  } while (done == L7_FALSE);

     
  if (sendMsg == L7_TRUE)
  {
    return dot1sIssueCmd(commonPortBpduFloodModeSet, intIfNum, L7_NULL, &val);
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the BPDU flood for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(input)} the value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfBpduFloodGet(L7_uint32 intIfNum, L7_BOOL *val)
{
  DOT1S_PORT_COMMON_CFG_t *pPortCfg = L7_NULLPTR;

  pPortCfg = dot1sIntfCfgEntryGet(intIfNum);
  if (pPortCfg != L7_NULLPTR)
  {
    *val = pPortCfg->bpduFloodMode;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Adaptive Path Cost Calculation for Link Aggregation Interfaces
*          
* @param    intIfNum   @b{(input:}   interface number
* @param    *pathCost  @b{Output:}   pointer to adaptive pathCost
*                      
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
L7_RC_t dot1sLagAutoPathCalculation(L7_uint32 intIfNum, L7_uint32 *pathCost)
{
  L7_uint32 speed;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 lagDataRate = 0;                     
                       
  rc = dot3adLagActiveDataRateGet(intIfNum, &lagDataRate);
    if (rc != L7_SUCCESS)
    {
      return rc;
    }
  /* accumulated speed is normalized to IEEE 802.1s denominator.
   * See Section 13.6.1 in IEEE 802.1s.
   */                                             
  speed = lagDataRate * 10;

                       
  if (speed > DOT1S_DEFAULT_PATH_COST_WORSE)
  {
    *pathCost = 0;    
  }
  else
  {
    *pathCost = ( DOT1S_DEFAULT_PATH_COST_WORSE / speed);              
  } 
  return rc;
}

/*********************************************************************
* @purpose Checks if the device is the root bridge.
*
* @param   mstID The MSTP instance ID
*
* @returns L7_TRUE  If the device is the root in the given instance
* @returns L7_FALSE  For all other cases
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dot1sIsBridgeRoot(L7_uint32 mstID)
{
  DOT1S_BRIDGEID_t bridgeId, rootID;
  L7_uint32 bridgeLen, rootLen;
  L7_RC_t rc;
  
  /* check if STP for the device is disabled if yes then retrun L7_TRUE */
  if(dot1sModeGet() == L7_DISABLE)
  {
    return L7_TRUE;
  }
  
  if (dot1sMstiBridgeIdentifierGet(mstID,(char *)&bridgeId, &bridgeLen) == L7_SUCCESS) 
  {
    if (dot1sInstCheckInUse(mstID) == L7_SUCCESS)
    {
      if (mstID != DOT1S_CIST_ID)
      {
        /* Regional Root for the MSTP Instances */
        rc = dot1sMstiRegionalRootIDGet(mstID, (char *)&rootID, &rootLen);
      }
      else
      {
         /* CIST Root */
         rc = dot1sCistRootIDGet((L7_uchar8 *)&rootID,&rootLen);
      }
    }
    else
	return L7_FALSE;  /* Device is not a Root Bridge */
    if (memcmp(&bridgeId, &rootID, sizeof(DOT1S_BRIDGEID_t)) == 0)
      return L7_TRUE;  /* Device is a Root Bridge */
    else
      return L7_FALSE;  /* Device is not a Root Bridge */
  }
  return L7_FALSE;    /* Device is not a Root Bridge */
}



