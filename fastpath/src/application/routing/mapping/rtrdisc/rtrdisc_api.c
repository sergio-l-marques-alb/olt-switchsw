/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rtrdisc_api.c
*
* @purpose   Router Discovery API functions
*
* @component 
*
* @comments  
*
* @create    11/19/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
        
*************************************************************/

#include "l7_common.h"
#include "l7_common_l3.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l3_default_cnfgr.h"
#include "rtrdisc.h"
#include "rtrdisc_config.h"
#include "rtrdisc_api.h"
#include "rtrdisc_util.h"


extern rtrDiscCfgData_t *rtrDiscCfgData;
extern L7_uint32 *rtrDiscMapTbl;
extern osapiRWLock_t    rtrDiscIntfRWLock;

/*********************************************************************
* @purpose  Gets the router discovery mode on the interface
*          
* @param    intIfNum   @b{(input)}  internal interface number
* @param    *flag      @b{(output)} advertisement flag L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvertiseGet(L7_uint32 intIfNum, L7_BOOL *flag)
{
  rtrDiscIntfCfgData_t *pCfg;

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  if (flag != L7_NULLPTR)
    *flag = pCfg->advertise;

  osapiReadLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the router discovery mode on the interface
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    flag       @b{(input)} advertisement flag L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvertiseSet(L7_uint32 intIfNum, L7_BOOL flag)
{
  rtrDiscIntfCfgData_t *pCfg;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if ((flag != L7_TRUE) && (flag != L7_FALSE))
  {
    return L7_FAILURE;
  }

  if (pCfg->advertise == flag)
  {
    return L7_SUCCESS;
  }

  pCfg->advertise = flag;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (pCfg->advertise == L7_FALSE)
  {
    /* Remove interface from the active list */
    rtrDiscRtrIntfDisable(intIfNum);
  }
  else if (pCfg->advertise == L7_TRUE)
  {
    if (rtrDiscMayEnableInterface(intIfNum))
    {
      /* Add the interface to the active list */
      rtrDiscRtrIntfEnable(intIfNum);
    }
  }
  else
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets the advertisement address that is used as the destination
*           IP address in the advertisement packet
*                
*          
* @param    intIfNum     @b{(input)}  internal interface number
* @param    *ipAddr      @b{(output)} advertisement address
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvAddrGet(L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr)
{
  rtrDiscIntfCfgData_t *pCfg;

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  if (ipAddr != L7_NULLPTR)
    *ipAddr = pCfg->advAddress;

  osapiReadLockGive(rtrDiscIntfRWLock);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the advertisement address that is used as the destination
*           IP address in the advertisement packet
*          
* @param    intfNum     @b{(input)} internal interface number
* @param    ipAddr      @b{(input)} advertisement address
*
* @returns  L7_SUCCESS 
*           L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvAddrSet(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr)
{
  rtrDiscIntfCfgData_t *pCfg;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if ((ipAddr != 0xE0000001) && (ipAddr != 0xFFFFFFFF))
  {
    return L7_FAILURE;
  }

  if (pCfg->advAddress == ipAddr)
  {
    return L7_SUCCESS;
  }

  pCfg->advAddress = ipAddr;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets the maximum time allowed between sending router  
*           advertisements from the interface.
*          
* @param    intIfNum    @b{(input)} internal interface number
* @param    *time       @b{(output)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface does not exists.
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMaxAdvIntervalGet(L7_uint32 intIfNum, L7_uint32 *time)
{
  rtrDiscIntfCfgData_t *pCfg;

  if (!time)
    return L7_FAILURE;

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  *time = pCfg->maxAdvInt;

  osapiReadLockGive(rtrDiscIntfRWLock);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the maximum time allowed between sending router  
*           advertisements from the interface.
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    maxAdvInt  @b{(input)} maximum advertisement interval (seconds)
*
* @returns  L7_SUCCESS 
*           L7_ERROR - if maxAdvInt is out of range
*           L7_REQUEST_DENIED - if maxAdvInt < configured minAdvInt
*           L7_FAILURE - any other failure
*
* @comments If the max adv int is set to a value < min adv int, min adv int
*           is automatically adjusted to 0.75 * max, whether or not the user
*           has configured a min value.
*
*           Whenever max adv int is set, the lifetime is automatically set
*           to 3 * max, even if the user has configured a lifetime. 
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMaxAdvIntervalSet(L7_uint32 intIfNum, L7_uint32 maxAdvInt)
{
  rtrDiscIntfCfgData_t *pCfg;
  L7_BOOL minIsDefault = L7_FALSE;
  L7_BOOL lifetimeIsDefault = L7_FALSE;

  /* Enforce range */
  if ((maxAdvInt < L7_RTR_DISC_MAXINTERVAL_MIN) ||
      (maxAdvInt > L7_RTR_DISC_MAXINTERVAL_MAX))
  {
    return L7_ERROR;
  }

  osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  if (pCfg->maxAdvInt == maxAdvInt)
  {
    /* No change */
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_SUCCESS;
  }

  if (pCfg->minAdvInt == rdMinAdvIntervalDefGet(intIfNum))
  {
    minIsDefault = L7_TRUE;
  }

  if (pCfg->advLifetime == rdAdvLifetimeDefGet(intIfNum))
  {
    lifetimeIsDefault = L7_TRUE;
  }

  /* If max < min and min is not the default, reject change. */
  if ((maxAdvInt < pCfg->minAdvInt) && !minIsDefault)
  {
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_REQUEST_DENIED;
  }

  /* If max > lifetime and lifetime not the default, reject change */
  if ((maxAdvInt > pCfg->advLifetime) && !lifetimeIsDefault)
  {
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_REQUEST_DENIED;
  }

  /* Set new max adv int */
  pCfg->maxAdvInt = maxAdvInt;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

  /* If min adv int is the default, update it according to the new max. */
  if (minIsDefault)
  {
    pCfg->minAdvInt = rdMinAdvIntervalDefGet(intIfNum);
  }

  /* If advertisement lifetime if the default, update it according to 
   * the new max. */
  if (lifetimeIsDefault)
  {
    pCfg->advLifetime = rdAdvLifetimeDefGet(intIfNum);
  }

  osapiWriteLockGive(rtrDiscIntfRWLock);

  rtrDiscMaxAdvIntervalApply(intIfNum, maxAdvInt);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets the minimum time allowed between sending router
*           advertisements from the interface.
*          
* @param    intIfNum   @b{(input)}  internal interface number
* @param    *time      @b{(output)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMinAdvIntervalGet(L7_uint32 intIfNum, L7_uint32 *time)
{
  rtrDiscIntfCfgData_t *pCfg;

  if (!time)
    return L7_FAILURE;

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  *time = pCfg->minAdvInt;

  osapiReadLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the minimum time allowed between sending router
*           advertisements from the interface.
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    minAdvInt  @b{(input)} minimum advertisement interval in seconds
*
* @returns  L7_SUCCESS 
*           L7_ERROR - min adv interval is out of range
*           L7_REQUEST_DENIED - If minAdvInt > maxAdvInt
*           L7_FAILURE - any other error
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMinAdvIntervalSet(L7_uint32 intIfNum, L7_uint32 minAdvInt)
{
  rtrDiscIntfCfgData_t *pCfg;

  /* Enforce range */
  if ((minAdvInt < L7_RTR_DISC_MININTERVAL_MIN) ||
      (minAdvInt > L7_RTR_DISC_MININTERVAL_MAX))
  {
    return L7_ERROR;
  }

  osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  if (pCfg->minAdvInt == minAdvInt)
  {
    /* No change */
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_SUCCESS;
  }

  /* Validate against max adv interval */
  if (minAdvInt > pCfg->maxAdvInt)
  {
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_REQUEST_DENIED;
  }

  /* Set value */
  pCfg->minAdvInt = minAdvInt;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

  osapiWriteLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the default MinAdvertisementInterval.
*          
* @param    intIfNum     @b{(input)}  internal interface number
* @param    defMinAdvInt @b{(output)} default MinAdvertisementInterval in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  
*
* @comments The default value is dynamic and depends on the current value
*           of MaxAdvertisementInterval.
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMinAdvIntervalDefaultGet(L7_uint32 intIfNum, L7_uint32 *defMinAdvInt)
{
  if (!defMinAdvInt)
    return L7_FAILURE;

  if (osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  *defMinAdvInt = rdMinAdvIntervalDefGet(intIfNum);

  osapiReadLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Revert MinAdvertisementInterval to its default value
*          
* @param    intIfNum     @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  
*
* @comments The default value is dynamic and depends on the current value
*           of MaxAdvertisementInterval.
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMinAdvIntervalRevert(L7_uint32 intIfNum)
{
  L7_uint32 defVal;
  rtrDiscIntfCfgData_t *pCfg;

  if (osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  defVal = rdMinAdvIntervalDefGet(intIfNum);

  if (pCfg->minAdvInt != defVal)
  {
    pCfg->minAdvInt = defVal;
    rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  osapiWriteLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    *time      @b{(output)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvLifetimeGet(L7_uint32 intIfNum, L7_uint32 *time)
{
  rtrDiscIntfCfgData_t *pCfg;

  if (!time)
    return L7_FAILURE;

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  *time = pCfg->advLifetime;

  osapiReadLockGive(rtrDiscIntfRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    lifetime   @b{(input)} lifetime in seconds
*
* @returns  L7_SUCCESS 
*           L7_ERROR - if lifetime is > max value
*           L7_REQUEST_DENIED - if lifetime is < max adv interval 
*           L7_FAILURE - any other failure
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvLifetimeSet(L7_uint32 intIfNum, L7_uint32 lifetime)
{
  rtrDiscIntfCfgData_t *pCfg;

  /* Enforce range */
  if (lifetime > L7_RTR_DISC_LIFETIME_MAX)
  {
    return L7_ERROR;
  }

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if (pCfg->advLifetime == lifetime)
  {
    /* No change */
    return L7_SUCCESS;
  }

  /* Lifetime must be >= max adv interval */
  if (lifetime < pCfg->maxAdvInt)
  {
    return L7_REQUEST_DENIED;
  }

  pCfg->advLifetime = lifetime;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    intIfNum     @b{(input)} internal interface number
* @param    defLifetime  @b{(input)} lifetime in seconds
*
* @returns  L7_SUCCESS 
*           L7_ERROR - if lifetime is > max value
*           L7_REQUEST_DENIED - if lifetime is < max adv interval 
*           L7_FAILURE - any other failure
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvLifetimeDefaultGet(L7_uint32 intIfNum, L7_uint32 *defLifetime)
{
  if (!defLifetime)
    return L7_FAILURE;

  if (osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  *defLifetime = rdAdvLifetimeDefGet(intIfNum);

  osapiReadLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Revert AdvertisementLifetime to its default value
*          
* @param    intIfNum     @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  
*
* @comments The default value is dynamic and depends on the current value
*           of MaxAdvertisementInterval.
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvLifetimeRevert(L7_uint32 intIfNum)
{
  L7_uint32 defVal;
  rtrDiscIntfCfgData_t *pCfg;

  if (osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  defVal = rdAdvLifetimeDefGet(intIfNum);

  if (pCfg->advLifetime != defVal)
  {
    pCfg->advLifetime = defVal;
    rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  osapiWriteLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the preferability of the address as a default router 
*           address, relative to other router addresses on the same subnet.  
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    *integer   @b{(output)} preference level
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscPreferenceLevelGet(L7_uint32 intIfNum, L7_int32 *integer)
{
  rtrDiscIntfCfgData_t *pCfg;

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  if (integer != L7_NULLPTR)
    *integer = pCfg->preferenceLevel;

  osapiReadLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the preferability of the address as a default router 
*           address, relative to other router addresses on the same subnet.  
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    integer    @b{(input)} preference level
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscPreferenceLevelSet(L7_uint32 intIfNum, L7_int32 integer)
{
  rtrDiscIntfCfgData_t *pCfg;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if (pCfg->preferenceLevel == integer)
  {
    return L7_SUCCESS;
  }

  pCfg->preferenceLevel = integer;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

