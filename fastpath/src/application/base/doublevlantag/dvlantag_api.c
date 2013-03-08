/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvlantag_api.c
*
* @purpose   Double Vlan Tagging API file
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
extern osapiRWLock_t dvlantagCfgRWLock;
extern L7_uint32 dvlantagDeviceEthertype; 
extern L7_BOOL dvlantagDeviceEthertypePresent;

/*********************************************************************
* @purpose  Set the double vlan tag mode on the interface
*          
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    mode       @b{(input)} Mode for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments mode: L7_ENABLE or L7_DISABLE
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  DVLANTAG_INTF_CFG_t *pCfg;
  L7_uchar8  tmp;

  /*check for valid parameters*/
  if ((mode != L7_ENABLE && mode != L7_DISABLE) ||
	  dvlantagIntfValidCheck(intIfNum) == L7_FAILURE)
  {
	  return L7_FAILURE;
  }

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (mode == pCfg->mode)
  {
	  return L7_SUCCESS;
  }
  
  osapiWriteLockTake(dvlantagCfgRWLock, L7_WAIT_FOREVER);

  /*Set it in the cfg structure*/
  pCfg->mode = mode;
  if (mode == L7_ENABLE)
  {
    pCfg->etherValMask = 1; /* Only Primary TPID is allowed */  
  }
  else if (mode == L7_DISABLE)
  {
    pCfg->etherValMask = 0;
  }

  /* Apply the config change */
  (void)dvlantagIntfModeApply(intIfNum, mode);

   if (mode == L7_DISABLE) /* Above call to HAPI, takes care of removing
                                                    configured TPIDs from the HW. We need to 
                                                    adjust the same in SW tables also. */
   {
     if(L7_DVLANTAG_MAX_TPIDS > 1) /* Do this only if there are Multiple TPIDS */
     {  
       for (tmp=1; tmp<L7_DVLANTAG_MAX_TPIDS;tmp++)
       {
         pCfg->etherType[tmp]=0;
       }
     }
   }

  dvlantagCfg->cfgHdr.dataChanged = L7_TRUE;

  osapiWriteLockGive(dvlantagCfgRWLock);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the double vlan tag mode on the interface
*          
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    *mode      @b{(input)} Mode for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments mode: L7_ENABLE or L7_DISABLE
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  DVLANTAG_INTF_CFG_t *pCfg;

  /*check for valid parameters*/
  if (dvlantagIntfValidCheck(intIfNum) == L7_FAILURE)
  {
	return L7_FAILURE;
  }

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /*return the cfg mode*/
  (void)osapiReadLockTake(dvlantagCfgRWLock, L7_WAIT_FOREVER);
  *mode = pCfg->mode;
  (void)osapiReadLockGive(dvlantagCfgRWLock);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the ethertype for the double vlan tagg for this interface
*          
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    etherType  @b{(input)} ethertype for this interface
* @param    etherTypeSet  @b{(input)} ethertype Enable/Disable flag.
* @param    ownLock       @b{(input)} Flag to indicate to take the LOCK.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* @returns  L7_NOT_EXIST  if the specified etherType is not yet configured 
*                         globally.
*
* @comments etherType: 0x0001 - 0xFFFF
*
* @Note This function is called from UI routines and internal rountines.
*       For internal routines we should not take the semaphore.
*       
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfEthertypeSet(L7_uint32 intIfNum, L7_uint32 etherType, 
                                 L7_BOOL etherTypeSet,
                                 L7_BOOL ownLock)
{
  DVLANTAG_INTF_CFG_t *pCfg;
  L7_uint32 i=0;
  L7_uint32 tpidIdx=0;
  L7_RC_t   rc = L7_FAILURE;

  /*check for valid parameters*/
  if ( (etherType < L7_DVLANTAG_MIN_ETHERTYPE) || 
       (etherType > L7_DVLANTAG_MAX_ETHERTYPE) ||
       (dvlantagIntfValidCheck(intIfNum) == L7_FAILURE)
     )
  {
  	return L7_FAILURE;
  }

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }


  if(L7_DVLANTAG_MAX_TPIDS <= 1) /* Proceed ahead only if there is support for Multiple TPIDS */
  {  
    return L7_NOT_SUPPORTED;
  }

  if(etherTypeSet == L7_TRUE)
  {
    if (dvlantagCfg->etherType[0] == etherType)
    {
      /* This is the primary Ethertype of the system which is already configured on all
       * interfaces. Just return without doing anything 
       */
      return L7_ALREADY_CONFIGURED;
    }
    for (i=1; i<L7_DVLANTAG_MAX_TPIDS; i++)
    {
      if (dvlantagCfg->etherType[i] == etherType)
      {
        tpidIdx = i;
        break;
      }
    }
  }
  else
  {
    for (i=1; i<L7_DVLANTAG_MAX_TPIDS; i++)
    {
      if (pCfg->etherType[i] == etherType)
      {
        tpidIdx = i;
        break;
      }
    }
    /*This interface does not have specified etherType
      configured. So no need to uncongigure.
      So simply return success.*/ 
    if(i == L7_DVLANTAG_MAX_TPIDS)
    {
      return L7_SUCCESS;
    }
  }

  if (i== L7_DVLANTAG_MAX_TPIDS)
  {
    /* No more free entries, so return back */
    return L7_NOT_EXIST;
  }

  if (ownLock == L7_TRUE)
  {
    osapiWriteLockTake(dvlantagCfgRWLock, L7_WAIT_FOREVER);
  }

  if(pCfg->mode == L7_DISABLE)
  {
    if (ownLock == L7_TRUE)
    {
      osapiWriteLockGive(dvlantagCfgRWLock);
    }
    return L7_NOT_SUPPORTED;
  }
  
  /*Set it in the cfg structure*/
  if (etherTypeSet == L7_TRUE)
  {
    pCfg->etherType[tpidIdx] = etherType;
    pCfg->etherValMask |= (1 << tpidIdx);    
  }
  else
  {
    pCfg->etherType[tpidIdx] = 0x00;
    pCfg->etherValMask ^= (1 << tpidIdx);        
  }

  /* Apply the config change */
  
  if (dvlantagIntfIsAcquired(intIfNum) == L7_TRUE)
  {
    dvlantagCfg->cfgHdr.dataChanged = L7_TRUE;
    if (ownLock == L7_TRUE)
    {
      osapiWriteLockGive(dvlantagCfgRWLock);
    }
    return L7_SUCCESS;
  }

  rc = dvlantagIntfEthertypeApply(intIfNum, etherType, etherTypeSet);
  if (rc != L7_SUCCESS)
  {
     /* Revert back the settings */
     if (etherTypeSet == L7_TRUE)
     {
       pCfg->etherType[tpidIdx] = 0;
       pCfg->etherValMask ^= (1 << tpidIdx);
     }
     else
     {
       pCfg->etherType[tpidIdx] = etherType;
       pCfg->etherValMask ^= (1 << tpidIdx);
     }
  }

  dvlantagCfg->cfgHdr.dataChanged = L7_TRUE;

  if (ownLock == L7_TRUE)
  {
    osapiWriteLockGive(dvlantagCfgRWLock);
  }
  
  return L7_SUCCESS;
}

static L7_RC_t dvlanTagPrimaryEtherTypeSet(L7_ushort16 etherType)
{
   L7_RC_t rc;
   L7_uint32              intf, j;
   DVLANTAG_INTF_CFG_t    *pCfg;
  
  if(dvlantagCfg->etherType[0] == etherType)
  {
    /* Primary TPID is already configured. Nothing to be done here */
    return L7_SUCCESS;
  }

  /* Apply this on to zeroth index of global Array */
  dvlantagCfg->etherType[0] = etherType;

  /* Let the HW know about the new Primary TPID  [HAPI change ]*/
  rc = dvlantagDefaultTpidApply(etherType);
  if(rc != L7_SUCCESS)
  {
    return rc;
  }
  /* Hapi call is successfull - Update the application structure */

  if(L7_DVLANTAG_MAX_TPIDS == 1)
  {
    /* Support for Multi-TPID is not present in this hardware */
    for (intf = 1; intf < L7_DVLANTAG_MAX_INTERFACE_COUNT; intf++)
    {
      if (dvlantagIntfIsConfigurable(intf, &pCfg) == L7_TRUE)
      {
        pCfg->etherType[0] = etherType;
        pCfg->etherValMask |= (1 << 0);
      }
    }
  }
  else /* Multi-TPID is supported */
  {
    /* Apply this on to each interface */
    for (intf = 1; intf < L7_DVLANTAG_MAX_INTERFACE_COUNT; intf++)
    {
      if (dvlantagIntfIsConfigurable(intf, &pCfg) == L7_TRUE)
      {
        pCfg->etherType[0] = etherType;
        pCfg->etherValMask |= (1 << 0);

        /* Setting the primary TPID deletes the secondary TPIDs from interfaces */
        /* Restore the secondary TPIDs on each each interface */
        for(j=1; j < L7_DVLANTAG_MAX_TPIDS; j++)
        {
          if (pCfg->etherType[j] != 0x00)
          {
            rc = dvlantagIntfEthertypeApply(intf, pCfg->etherType[j], L7_TRUE);      
          }
        }
      }
    }
  }
  dvlantagCfg->cfgHdr.dataChanged = L7_TRUE;
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DVLANTAG_COMPONENT_ID,
          "Primary Ethertype set to 0x%x", etherType); 

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set/Clear the GLOBAL ethertype
*          
* @param    etherType     @b{(input)} ethertype for this interface
* @param    primaryTpid   @b{(input)} Is this the primary TPID
* @param    etherTypeSet  @b{(input)} Set /Clear the TPID value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments etherType: 0x0001 - 0xFFFF
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagEthertypeSet(L7_uint32 etherType, L7_BOOL primaryTpid, L7_BOOL etherTypeSet)
{
  L7_uint32              i;
  L7_uint32              tpidIdx=L7_DVLANTAG_MAX_TPIDS;
  L7_BOOL                tpidIdxFree = L7_FALSE;
  L7_BOOL                isAlreadyConfigured = L7_FALSE;
  L7_RC_t                 rc = L7_SUCCESS;

  /*check for valid parameters*/
  if ( (etherType < L7_DVLANTAG_MIN_ETHERTYPE) ||
       (etherType > L7_DVLANTAG_MAX_ETHERTYPE))
  {
  	 return L7_FAILURE;
  }

  if (primaryTpid == L7_TRUE)
  {
    /* Set/Clear the Primary TPID (TPID at index 0) */
    tpidIdx = 0;
    
    if (etherTypeSet == L7_FALSE)                                     /* We need to clear the set TPID (reset it back to FD TPID) */
    {
      if (etherType == L7_DVLANTAG_DEFAULT_ETHERTYPE) /* We can't remove the FD TPID */
      {
        return L7_NOT_SUPPORTED;
      }
      if(etherType == dvlantagCfg->etherType[tpidIdx])     /* Check if the set TPID is the same as we need to clear */
      {
        etherType = L7_DVLANTAG_DEFAULT_ETHERTYPE;
      }
      else
      {
        return L7_NOT_EXIST;
      }
    }

    return dvlanTagPrimaryEtherTypeSet(etherType); 
  }

  if(L7_DVLANTAG_MAX_TPIDS <= 1) /* Proceed ahead only if there is support for Multiple TPIDS */
  {  
    return L7_NOT_SUPPORTED;
  }

  /* Check if the EtherType is already configured */
  for (i=1; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    if (dvlantagCfg->etherType[i] == etherType)
    {
      isAlreadyConfigured = L7_TRUE;
      tpidIdx = i;
      break;
    }
  }

    /* Find the first available free TPID index and set the TPID OR 
        Find the TPID index that needs to be cleared */
  for (i=1; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    if(dvlantagCfg->etherType[i] == etherType)
    {
      if(etherTypeSet == L7_FALSE)
      {
        /* This is the value we need to clear - set the tpidIdx */
        tpidIdx = i;
        break;
      }
      else if (isAlreadyConfigured == L7_TRUE)
      {
        /* Current Ethertype is already set */
        tpidIdx = i;
        break;
      }
    }
    else if ((isAlreadyConfigured == L7_FALSE) && 
                (dvlantagCfg->etherType[i] == 0 && etherTypeSet == L7_TRUE))
    {
      /* Found a free Index - set the tpidIdx */
      tpidIdx = i;
      tpidIdxFree = L7_TRUE;
      break;
    }
  }

  if(tpidIdx == L7_DVLANTAG_MAX_TPIDS)
  {
    if(etherTypeSet == L7_TRUE)
    {
      return L7_TABLE_IS_FULL;
    }
    else
    {
      return L7_NOT_EXIST;
    }
  }

  osapiWriteLockTake(dvlantagCfgRWLock, L7_WAIT_FOREVER);

  /* Apply the config change */
  rc = dvlantagEthertypeApply(etherType, tpidIdx);

  /* Update the pCfg structure */
  if (etherTypeSet == L7_TRUE)
  {
    dvlantagCfg->etherType[tpidIdx] = etherType;
  }
  else if (etherTypeSet == L7_FALSE)
  {
    dvlantagCfg->etherType[tpidIdx] = 0;
  }
  dvlantagCfg->cfgHdr.dataChanged = L7_TRUE;
  
  osapiWriteLockGive(dvlantagCfgRWLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the GLOBAL ethertype based on TPID Index
*          
* @param    etherType     @b{(input)} ethertype for this interface
* @param    tpidIdx       @b{(input)} TPID Index
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments etherType: 0x0000 - 0xFFFF
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagEthertypeGet(L7_uint32 *etherType, L7_uint32 tpidIdx)
{
  if(tpidIdx >= L7_DVLANTAG_MAX_TPIDS)
  {
    return L7_FAILURE;
  }
  
  (void)osapiReadLockTake(dvlantagCfgRWLock, L7_WAIT_FOREVER);
  *etherType = dvlantagCfg->etherType[tpidIdx];
  (void)osapiReadLockGive(dvlantagCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the ethertype for the double vlan tagg for this interface
*          
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    *etherType @b{(input)} ethertype for this interface
* @param    tpidIdx       @b{(input)} TPID Index
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments etherType: 0x0000 - 0xFFFF
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfEthertypeGet(L7_uint32 intIfNum, L7_uint32 *etherType, L7_uint32 tpidIdx)
{
  DVLANTAG_INTF_CFG_t *pCfg;

  /*check for valid parameters*/
  if (dvlantagIntfValidCheck(intIfNum) == L7_FAILURE)
  {
	return L7_FAILURE;
  }

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /*return the cfg mode*/
  (void)osapiReadLockTake(dvlantagCfgRWLock, L7_WAIT_FOREVER);
  *etherType = pCfg->etherType[tpidIdx];
  (void)osapiReadLockGive(dvlantagCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the customer ID for double vlan tagging for this interface
*          
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    custId     @b{(input)} Customer ID for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments custId: 0 - 4095
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfCustIdSet(L7_uint32 intIfNum, L7_uint32 custId)
{
  DVLANTAG_INTF_CFG_t *pCfg;

  /*check for valid parameters*/
  if ( (custId < 1) ||
       (custId > L7_DVLANTAG_MAX_CUSTID) ||
       (dvlantagIntfValidCheck(intIfNum) == L7_FAILURE)
     )
  {
	return L7_FAILURE;
  }

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;
  
  if (custId == pCfg->custId)
  {
	return L7_SUCCESS;
  }

  osapiWriteLockTake(dvlantagCfgRWLock, L7_WAIT_FOREVER);

  /* Apply the config change */
  (void)dvlantagIntfCustIdApply(intIfNum, custId);

  /*Set it in the cfg structure*/
  pCfg->custId = custId;
  dvlantagCfg->cfgHdr.dataChanged = L7_TRUE;

  osapiWriteLockGive(dvlantagCfgRWLock);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the customer ID for double vlan tagging for this interface
*          
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    *custId    @b{(input)} Customer ID for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments custId: 0 - 4095
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfCustIdGet(L7_uint32 intIfNum, L7_uint32 *custId)
{
  DVLANTAG_INTF_CFG_t *pCfg;

  /*check for valid parameters*/
  if (dvlantagIntfValidCheck(intIfNum) == L7_FAILURE)
  {
	return L7_FAILURE;
  }

  if (dvlantagIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /*return the cfg mode*/
  (void)osapiReadLockTake(dvlantagCfgRWLock, L7_WAIT_FOREVER);
  *custId = pCfg->custId;
  (void)osapiReadLockGive(dvlantagCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks is dvlan configuration can be configued to a given interface type
*          
* @param    intfType  @b{(input)} interface type
*
* @returns  L7_SUCCESS  On a valid interface
* @returns  L7_FAILURE  On an invalid interface
*
* @comments Does not mean that this configuration will be applied.
*           For configuration to be applied the interface must not
*           active lag member, probe intf, router port etc.
*
* @end
*********************************************************************/
L7_RC_t dvlantagIsValidIntfType(L7_uint32 intfType)
{
  if ((intfType != L7_PHYSICAL_INTF) && (intfType != L7_LAG_INTF))
      return L7_FAILURE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks is dvlan configuration can be configued to a given interface 
*          
* @param    intIfNum  @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  On a valid interface
* @returns  L7_FAILURE  On an invalid interface
*
* @comments Does not mean that this configuration will be applied.
*           For configuration to be applied the interface must not
*           active lag member, probe intf, router port etc.
*
*
* @end
*********************************************************************/
L7_RC_t dvlantagIntfValidCheck(L7_uint32 intIfNum)
{
  L7_uint32 intfType;
  L7_RC_t   rc;
  
  rc = nimGetIntfType(intIfNum,&intfType);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return dvlantagIsValidIntfType(intfType);
}
