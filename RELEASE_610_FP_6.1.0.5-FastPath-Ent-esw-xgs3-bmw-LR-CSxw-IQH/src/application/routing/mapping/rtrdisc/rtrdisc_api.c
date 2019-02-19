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
  L7_uint32 rtrIfNum, mode;
  L7_IP_ADDR_t ipAddr;
  L7_IP_MASK_t mask;
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
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if ((ipMapRtrIntfModeGet(intIfNum, &mode) == L7_SUCCESS) && 
          (mode == L7_ENABLE))
      {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS &&
            ipAddr != L7_NULL_IP_ADDR)
        {
          /*Remove the router info from the list*/
          rtrDiscRtrIntfDisable(intIfNum);
        }
      }
    }
  }
  else if (pCfg->advertise == L7_TRUE)
  {
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if ((ipMapRtrIntfModeGet(intIfNum, &mode) == L7_SUCCESS) && 
          (mode == L7_ENABLE))
      {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS &&
            ipAddr != L7_NULL_IP_ADDR)
        {
          /*Add the router info to the list*/
          rtrDiscRtrIntfEnable(intIfNum);
        }
      }
    }
  }
  else
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets the advertisement address that is used as the destination
*           ip address in the advertisement packet
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
*           ip address in the advertisement packet
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
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMaxAdvIntervalGet(L7_uint32 intIfNum, L7_uint32 *time)
{
  rtrDiscIntfCfgData_t *pCfg;

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  if (time != L7_NULLPTR)
    *time = pCfg->maxAdvInt;

  osapiReadLockGive(rtrDiscIntfRWLock);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the maximum time allowed between sending router  
*           advertisements from the interface.
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMaxAdvIntervalSet(L7_uint32 intIfNum, L7_uint32 time)
{
  rtrDiscIntfCfgData_t *pCfg;
  L7_uint32 minInt, maxInt;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if (pCfg->maxAdvInt == time)
  {
    return L7_SUCCESS;
  }

  if (rtrDiscMinAdvIntervalGet(intIfNum,&minInt) != L7_SUCCESS)
    return L7_FAILURE;
  if (minInt < L7_RTR_DISC_MAXINTERVAL_MIN)
    minInt = L7_RTR_DISC_MAXINTERVAL_MIN;

  if (rtrDiscAdvLifetimeGet(intIfNum,&maxInt) != L7_SUCCESS)
    return L7_FAILURE;
  if (maxInt > L7_RTR_DISC_MAXINTERVAL_MAX)
    maxInt = L7_RTR_DISC_MAXINTERVAL_MAX;

  if (time < minInt || time > maxInt)
    return L7_FAILURE;

  pCfg->maxAdvInt = time;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

  rtrDiscMaxAdvIntervalApply(intIfNum, time);

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
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMinAdvIntervalGet(L7_uint32 intIfNum, L7_uint32 *time)
{
  rtrDiscIntfCfgData_t *pCfg;

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  if (time != L7_NULLPTR)
    *time = pCfg->minAdvInt;

  osapiReadLockGive(rtrDiscIntfRWLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the minimum time allowed between sending router
*           advertisements from the interface.
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscMinAdvIntervalSet(L7_uint32 intIfNum, L7_uint32 time)
{
  rtrDiscIntfCfgData_t *pCfg;
  L7_uint32 maxInt;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if (pCfg->minAdvInt == time)
  {
    return L7_SUCCESS;
  }

  if (rtrDiscMaxAdvIntervalGet(intIfNum,&maxInt) != L7_SUCCESS)
    return L7_FAILURE;
  if (time < L7_RTR_DISC_MININTERVAL_MIN || time > maxInt)
    return L7_FAILURE;

  pCfg->minAdvInt = time;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

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

  osapiReadLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
	  osapiReadLockGive(rtrDiscIntfRWLock);
    return L7_FAILURE;
  }

  if (time != L7_NULLPTR)
    *time = pCfg->advLifetime;

  osapiReadLockGive(rtrDiscIntfRWLock);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvLifetimeSet(L7_uint32 intIfNum, L7_uint32 time)
{
  rtrDiscIntfCfgData_t *pCfg;
  L7_uint32 maxInt;

  if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if (pCfg->advLifetime == time)
  {
    return L7_SUCCESS;
  }

  if (rtrDiscMaxAdvIntervalGet(intIfNum,&maxInt) != L7_SUCCESS)
    return L7_FAILURE;
  if (time < maxInt || time > L7_RTR_DISC_LIFETIME_MAX)
    return L7_FAILURE;

  pCfg->advLifetime = time;
  rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

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

