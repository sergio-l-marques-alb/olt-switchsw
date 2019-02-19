/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvlantag_ih.c
*
* @purpose   Double Vlan Tagging Interface file
*
* @component DvlanTag
*
* @comments
*
* @create    08/20/2003
*
* @author    skalyanam
*
* @end
*
**********************************************************************/
#include "dvlantag_include.h"

extern DVLANTAG_CFG_t *dvlantagCfg;
extern DVLANTAG_PORT_t *dvlantagPort;
extern L7_uint32 *dvlantagMapTbl;

extern dvlantagCnfgrState_t dvlantagCnfgrState;
extern L7_uint32 dvlantagDeviceEthertype; 
extern L7_BOOL dvlantagDeviceEthertypePresent;

/*********************************************************************
* @purpose  Applies the interface mode for Double vlan tagging
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode     @b{(input)} Mode to be applied
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t        rc = L7_SUCCESS;
  DVLANTAG_INTF_CFG_t *pCfg;
  DVLANTAG_DTL_t dTag;

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_SUCCESS;

  if (dvlantagIntfIsAcquired(intIfNum) == L7_TRUE)
    return L7_SUCCESS;

  dTag.mode = mode;
  dTag.etherMask = pCfg->etherValMask;
  dTag.custId = pCfg->custId;
  /* Mode should be enabled alsways with the default TPID */
  {
  dTag.etherType = dvlantagCfg->etherType[0];
  }

  /*Make the DTL call*/
  rc = dtlDvlantagIntfApply(intIfNum, &dTag);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("Dvlantag: Unable to set mode on interface %u", intIfNum);
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Applies the GLOBAL ethertype
*
* @param    etherType @b{(input)} etherType for Double Vlan Tagging
* @param    tpidIdx  @b{(input)} TPID index for setting the EtherType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This will only make the DTL call
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagEthertypeApply(L7_uint32 etherType, L7_uint32 tpidIdx)
{
  L7_RC_t        rc = L7_SUCCESS;
  DVLANTAG_DTL_GLOBAL_t dTag;

  dTag.tpidIdx = tpidIdx;
  dTag.etherType = etherType;

  /* Make the call irrespective whether dvlantag is enabled
   */
  rc = dtlDvlantagApply(&dTag);
  
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("Dvlantag: Unable to set etherType %d", etherType);
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Applies the ethertype for this interface's double vlan tagging
*
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    etherType @b{(input)} etherType for Double Vlan Tagging
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This will only make the DTL call if dvlantagging is
*           enabled for this interface
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfEthertypeApply(L7_uint32 intIfNum, L7_uint32 etherType,
                                   L7_BOOL enable)
{
  L7_RC_t        rc = L7_SUCCESS;
  DVLANTAG_INTF_CFG_t *pCfg;
  DVLANTAG_DTL_t dTag;

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_SUCCESS;

  /* Reverting back the etherType from Mask as we don't have
     Needed SDK support for this
   */
  dTag.etherType = etherType;/* pCfg->etherValMask;*/
  dTag.mode = pCfg->mode;
  dTag.enable = enable;
  dTag.custId = pCfg->custId;

  /* Make the call irrespective whether dvlantag is enabled
   */
  rc = dtlDvlantagIntfMultiTpidApply(intIfNum, &dTag);
  
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("Dvlantag: Unable to set etherType on interface %u", intIfNum);
    rc = L7_FAILURE;
  }

  return rc;
}
/*********************************************************************
* @purpose  Applies the CustomerID for this interface's double vlan tagging
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    custId   @b{(input)} Customer ID
* @param    @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This will only make the DTL call if dvlantagging is
*           enabled for this interface
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfCustIdApply(L7_uint32 intIfNum, L7_uint32 custId)
{
  L7_RC_t        rc = L7_SUCCESS;
  DVLANTAG_INTF_CFG_t *pCfg;
  DVLANTAG_DTL_t dTag;
  
  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_SUCCESS;

  if (dvlantagIntfIsAcquired(intIfNum) == L7_TRUE)
    return L7_SUCCESS;

  dTag.custId = custId;
  dTag.etherMask = pCfg->etherValMask;
  dTag.mode = pCfg->mode;

  /* Make the call irrespective whether dvlantag is enabled
   */
  rc = dtlDvlantagIntfApply(intIfNum, &dTag);
  
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("Dvlantag: Unable to set custId on interface %u", intIfNum);
    rc = L7_FAILURE;
  }

  return rc;
}
/*********************************************************************
* @purpose
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL dvlantagIntfIsAcquired(L7_uint32 intIfNum)
{
  /* this routine checks to see if we can apply the config
     to this port. the reasons being configured for routing,
   probe and lag member
  */
  L7_uint32 routingEnabled = L7_DISABLE;

  if (dot3adIsLagActiveMember(intIfNum) == L7_TRUE)
    return L7_TRUE;
  /* If this is the mirror dest port, it can be configured but not enabled */
  if (mirrorIsActiveProbePort(intIfNum) == L7_TRUE)
    return L7_TRUE;
  /* If this port is configured for routing, it can be configured but not enabled */
  if ((ipMapRtrIntfModeGet(intIfNum, &routingEnabled) == L7_SUCCESS) &&
      (routingEnabled == L7_ENABLE))
    return L7_TRUE;

  return L7_FALSE;
}
/*********************************************************************
* @purpose  NIM interface change callback processing
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    event    @b{(input)} Event to be processes
* @param    @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfChangeCallBack(L7_uint32 intIfNum, L7_uint32 intfEvent, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_FAILURE;
  NIM_EVENT_COMPLETE_INFO_t status;
  DVLANTAG_INTF_CFG_t *pCfg;
  L7_uint32    prevAcquired;
  L7_uint32    currAcquired;
  L7_uint32    lagIntf;
  L7_uint32    tmp = 0;

  status.intIfNum     = intIfNum;
  status.component    = L7_DVLANTAG_COMPONENT_ID;
  status.event        = intfEvent;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (!(DVLANTAG_IS_READY))
  {
    LOG_MSG("dvlantagIntfChangeCallBack: Received an interface change callback while not ready to receive it");
    rc = L7_FAILURE;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if ((intfEvent != L7_CREATE) && (intfEvent != L7_DELETE))

  {
    /*check to see if dvlantag is config enabled for this interface*/
    if ( (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)) /*  ||
         ((pCfg->mode != L7_ENABLE))
       ) &&
		  (pCfg->etherType[0] == L7_DVLANTAG_DEFAULT_ETHERTYPE)))*/
    {
      rc = L7_SUCCESS;
      status.response.rc = rc;
      nimEventStatusCallback(status);
      return rc;
    }
  }

  if (dvlantagIntfValidCheck(intIfNum) != L7_SUCCESS)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }
  switch (intfEvent)
  {
      case L7_LAG_ACQUIRE:
    /* When a port is enabled for a LAG, remove the individual port from the
       double tag and let the policy of the aggregator apply. Specifically,
       this means that the double tag policy of the aggregator applies to all
       ports that are aggregated together on any particular aggregator. It is
       expected that the driver will enforce this policy. */

    COMPONENT_ACQ_NONZEROMASK(dvlantagPort[intIfNum].acquiredList, prevAcquired);
    /* update acquired list */
    COMPONENT_ACQ_SETMASKBIT(dvlantagPort[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);

    /* If the interface was not previously acquired, disable dvlan tagging. */
    if (prevAcquired == L7_FALSE)
    {
      if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
      {
        if (pCfg->mode == L7_ENABLE)
		{
		  (void) dvlantagConfigPortApply(intIfNum,
                                         L7_DISABLE,
                                         pCfg->etherValMask,
                                         pCfg->custId);

		}
      }
    }

    if ((dot3adWhoisOwnerLag(intIfNum, &lagIntf) == L7_SUCCESS) &&
		(intIfNum != lagIntf))
	{
	  if (dvlantagIntfIsConfigurable(lagIntf, &pCfg) == L7_TRUE)
	  {
	    (void) dvlantagConfigPortApply(lagIntf,
                                       pCfg->mode,
                                       pCfg->etherValMask,
                                       pCfg->custId);

         /* Apply the Multiple TPIDs on this port */
         /* The default TPID is configured as part of the above call */
         for (tmp = 1; tmp < L7_DVLANTAG_MAX_TPIDS; tmp++)
         {
            if (pCfg->etherType[tmp])
            {
              (void) dvlantagIntfEthertypeApply (lagIntf,
                                                 pCfg->etherType[tmp],
                                                 L7_ENABLE);
            }
         }
	  }
	}

    rc = L7_SUCCESS;
    break;

  case L7_PROBE_SETUP:
    /* When a port is enabled with port mirroring, remove the individual port from the
       double tag and let the policy of the port mirroring virtual interface apply, i.e.
       the mirrored port. */
    COMPONENT_ACQ_NONZEROMASK(dvlantagPort[intIfNum].acquiredList, prevAcquired);
    /* update acquired list */
    COMPONENT_ACQ_SETMASKBIT(dvlantagPort[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

    /* If the interface was not previously acquired, disable dvlan tagging. */
    if (prevAcquired == L7_FALSE)
    {
      if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
      {
        if (pCfg->mode == L7_ENABLE)
		{
		  (void) dvlantagConfigPortApply(intIfNum,
                                         L7_DISABLE,
                                         pCfg->etherValMask,
                                         pCfg->custId);
	}
		}
    }
    rc = L7_SUCCESS;
    break;

  case L7_PORT_ROUTING_ENABLED:
  case L7_PORT_ROUTING_DISABLED:

    /* When a port is enabled/disabled for routing, leave the original policy
       selected for double tagging on the individual port. This means that
       for layer 3 ports (really, for the layer 3 virtual interface),
       double tagging can apply to some of the ports on the routing virtual interface
       and not others. */

    rc = L7_SUCCESS;
    break;

  case L7_LAG_RELEASE:
    /* When a port is disabled for a LAG and if no other component has acquired
       the port, e.g. port mirroring, and the port originally had double tagging
       enabled, then enable double tagging again. */
    COMPONENT_ACQ_CLRMASKBIT(dvlantagPort[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
    COMPONENT_ACQ_NONZEROMASK(dvlantagPort[intIfNum].acquiredList, currAcquired);
    if (currAcquired == L7_FALSE)
    {
      if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
      {
        (void) dvlantagConfigPortApply(intIfNum,
                                       pCfg->mode,
                                       pCfg->etherValMask,
                                       pCfg->custId);

         /* Apply the Multiple TPIDs on this port */
         /* The default TPID is configured as part of the above call */
         for (tmp = 1; tmp < L7_DVLANTAG_MAX_TPIDS; tmp++)
         {
            if (pCfg->etherType[tmp])
            {
              (void) dvlantagIntfEthertypeApply (intIfNum,
                                                 pCfg->etherType[tmp],
                                                 L7_ENABLE);
            }
         }

     }
        
    }
    rc = L7_SUCCESS;
    break;

  case L7_PROBE_TEARDOWN:
    /* When a port is disabled for port mirroring and if no other component has acquired
       the port, e.g. LAG, and the port originally had double tagging
       enabled, then enable double tagging on the port again. */
    COMPONENT_ACQ_CLRMASKBIT(dvlantagPort[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
    COMPONENT_ACQ_NONZEROMASK(dvlantagPort[intIfNum].acquiredList, currAcquired);
    if (currAcquired == L7_FALSE)
    {
      if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
        (void) dvlantagConfigPortApply(intIfNum,
                                       pCfg->mode,
                                       pCfg->etherValMask,
                                       pCfg->custId);
    }
    rc = L7_SUCCESS;
    break;

  case L7_CREATE:
    rc = dvlantagIntfCreate(intIfNum);
    break;

  case L7_ATTACH:
    rc = dvlantagApplyIntfConfigData(intIfNum);
    break;

  case L7_DETACH:
    rc = dvlantagIntfDetach(intIfNum);
    break;

  case L7_DELETE:
    rc = dvlantagIntfDelete(intIfNum);
    break;

  default:
    rc = L7_SUCCESS;
    break;

  }

  status.response.rc = rc;
  nimEventStatusCallback(status);

  return rc;
}
/*********************************************************************
* @purpose  Routine uses the config values for setting up dvlantag
*
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    mode      @b{(input)} Mode
* @param    etherType @b{(input)} Ethertype
* @param    custId    @b{(input)} Customer ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvlantagConfigPortApply(L7_uint32 intIfNum,
                L7_uint32 mode,
                L7_uint32 etherValMask,
                L7_uint32 custId)
{
  L7_RC_t        rc = L7_FAILURE;
  DVLANTAG_DTL_t dTag;
  
  dTag.mode = mode;
  dTag.custId = custId;
  dTag.etherMask = etherValMask;
  dTag.etherType = 0x8100;
  dTag.etherType =  dvlantagCfg->etherType[0];
  
  /*Make the DTL call*/
  rc = dtlDvlantagIntfApply(intIfNum, &dTag);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("Dvlantag: Unable to set mode on interface %u", intIfNum);
    rc = L7_FAILURE;
  }

  return rc;


}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dvlantag port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL dvlantagIntfIsConfigurable(L7_uint32 intIfNum, DVLANTAG_INTF_CFG_t **pCfg)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId;

  if (!(DVLANTAG_IS_READY))
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  cfgIndex = dvlantagMapTbl[intIfNum];

  if (cfgIndex == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(dvlantagCfg->intfCfg[cfgIndex].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between dvlantagCfg and dvlantagMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DVLANTAG_COMPONENT_ID,
          "dvlantagIntfIsConfigurable: Error accessing dvlantag config data for interface %d."
          " A default configuration does not exist for this interface. Typically a case when a"
          " new interface is created and has no pre-configuration.", intIfNum);
      return L7_FALSE;
    }
  }

  *pCfg = &dvlantagCfg->intfCfg[cfgIndex];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dvlantag port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @end
*********************************************************************/
L7_BOOL dvlantagIntfConfigEntryGet(L7_uint32 intIfNum, DVLANTAG_INTF_CFG_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_RC_t rc;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (!(DVLANTAG_IS_READY))
    return L7_FALSE;

  if ((rc = nimConfigIdGet(intIfNum, &configId)) == L7_SUCCESS)
  {
    for (i = 1; i < L7_DVLANTAG_MAX_INTERFACE_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&dvlantagCfg->intfCfg[i].configId, &configIdNull))
      {
        dvlantagMapTbl[intIfNum] = i;
        *pCfg = &dvlantagCfg->intfCfg[i];
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dvlantagIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  DVLANTAG_INTF_CFG_t *pCfg;
  L7_uint32 i;

  if (dvlantagIntfValidCheck(intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_DVLANTAG_MAX_INTERFACE_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&dvlantagCfg->intfCfg[i].configId, &configId))
    {
      dvlantagMapTbl[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface, assign one */
  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if (dvlantagIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id */
    if (pCfg != L7_NULL)
    {
      dvlantagBuildDefaultIntfConfigData(&configId, pCfg);
  	  if (dvlantagDeviceEthertypePresent == L7_TRUE)
  	  {
  		/* Update the ethertype to the global value which may be different from 
  		 * the default.
  		 */
  		  pCfg->etherType[0] = dvlantagDeviceEthertype;
  	  }
	  }
  }
  else
  {
	/* We found a saved configuration for this interface use that 
	 * ethertype value to update the global value if and only if 
	 * global ethertype is supported.
	 */
	  if (dvlantagDeviceEthertypePresent == L7_TRUE)
	  {
      if (pCfg->etherType[0] != 0)
      {
        dvlantagDeviceEthertype = pCfg->etherType[0];
      }
      else 
      {
        dvlantagDeviceEthertype = L7_DVLANTAG_VMAN_ETHERTYPE;
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dvlantagIntfDetach(L7_uint32 intIfNum)
{
  DVLANTAG_INTF_CFG_t *pCfg;

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    (void)dvlantagConfigPortApply(intIfNum, L7_DISABLE, 
                                  L7_DVLANTAG_DEFAULT_PORT_MODE, 
                                  L7_DVLANTAG_DEFAULT_CUSTID);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dvlantagIntfDelete(L7_uint32 intIfNum)
{
  DVLANTAG_INTF_CFG_t *pCfg;

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset((void *)&dvlantagMapTbl[intIfNum], 0, sizeof(L7_uint32));
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies the ethertype for this interface's double vlan tagging
*
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    etherType @b{(input)} etherType for Double Vlan Tagging
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This will only make the DTL call if dvlantagging is
*           enabled for this interface
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagDefaultTpidApply(L7_uint32 etherType)

{

  L7_RC_t        rc = L7_SUCCESS;
  DVLANTAG_DTL_t dTag;


  dTag.etherType = etherType;/* pCfg->etherValMask;*/

  rc = dtlDvlantagDefaultTpidApply(&dTag);

  if (rc != L7_SUCCESS)
  {
    LOG_MSG("Dvlantag: Unable to set Default TPID %u", etherType);
    rc = L7_FAILURE;
  }

  return rc;
}

