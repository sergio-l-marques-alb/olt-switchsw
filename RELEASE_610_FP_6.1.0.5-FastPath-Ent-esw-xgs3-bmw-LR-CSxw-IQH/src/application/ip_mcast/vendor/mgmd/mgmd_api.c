
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  mgmd_vend_api.c
*
* @purpose   MGMD vendor-specific API functions
*
* @component Mgmd Vendor APIs
*
* @comments  none
*
* @create    April 05, 2006
*
* @author    akamlesh
*
* @end
*
**********************************************************************/
#include "mgmd.h"
#include "mgmd_proxy.h"
#include "mgmd_api.h"
#include "mgmd_mrp.h"

static L7_RC_t  mgmdVerify(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum);

/*********************************************************************
* @purpose  Gets the Querier for the specified interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    *querierIP  @b{(output)}  Querier  IP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfQuerierGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_inet_addr_t *querierIP)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE; 
  }

  if (inetCopy(querierIP, &((mgmdCB->mgmd_info[rtrIfNum]).mgmd_querierIP)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " inetCopy Failed. \n");  
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Querier Up Time for the specified interface
*
* @param    mgmdCBHndl      @b{(input)}   MGMD CB Handle
* @param    rtrIfNum        @b{(input)}   Router Interface Number
* @param    querierUpTime   @b{(output)}  Querier  Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfQuerierUpTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32* querierUpTime)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  *querierUpTime = (osapiUpTimeRaw()) - (mgmdCB->mgmd_info[rtrIfNum].querierUpTime);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Querier Expiry Time for the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    querierExpiryTime   @b{(output)}  Querier  Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfQuerierExpiryTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32* querierExpiryTime)
{
  L7_int32 expiryTime = 0;
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  L7_int32 status = L7_FAILURE,querier=L7_FALSE;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if((mgmdIntfQuerierStatusGet(mgmdCBHndl,rtrIfNum,&querier)==L7_SUCCESS) &&
     (querier==L7_TRUE))
  {
     *querierExpiryTime = 0;
     return L7_SUCCESS;
  }


  status = appTimerTimeLeftGet(mgmdCB->timerHandle, mgmdCB->mgmd_info[rtrIfNum].mgmd_querier_timer, &expiryTime);

  if (status != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " COuld not get the left out time\n");  
    return L7_FAILURE; 
  }

  if (expiryTime < 0)
    *querierExpiryTime = 0;
  else
    *querierExpiryTime = expiryTime;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Wrong Version Queries for the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    wrongVerQueries     @b{(output)}  Wrong Version Queries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfWrongVersionQueriesGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32* wrongVerQueries)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  *wrongVerQueries = mgmdCB->mgmd_info[rtrIfNum].Wrongverqueries;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Number of Joins on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    intfJoins           @b{(output)}  Number of Joins on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfJoinsGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32* intfJoins)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  *intfJoins  =  mgmdCB -> mgmd_info[rtrIfNum].numOfJoins;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    numOfGroups         @b{(output)}  Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfGroupsGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32* numOfGroups)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  *numOfGroups = (mgmdCB->mgmd_info[rtrIfNum]).numOfGroups;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Gets the Querier Status for the specified interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    status      @b{(output)}  Querier  Status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfQuerierStatusGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32 *status)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  *status = mgmd_is_querier(mgmdCB, rtrIfNum);

  return L7_SUCCESS;
}

/**************************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    *ipAddr           @b{(output)} last reporter ip
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheLastReporterGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_inet_addr_t *ipAddr)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }


  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " AVL search  Failed. \n"); 
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    if (inetCopy(ipAddr, &(mgmd_group->reporter)) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_FAILURE;
    }
  }

  osapiSemaGive(mgmdCB->membership.semId);
  return L7_SUCCESS;
}

/**************************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache Table for
*           the specified group address & the specified interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *upTime           @b{(output)} Up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheUpTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_uint32 *upTime)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " AVL search  Failed. \n"); 
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    *upTime  = osapiUpTimeRaw() - mgmd_group->ctime;
  }

  osapiSemaGive(mgmdCB->membership.semId);
  return L7_SUCCESS;
}

/**************************************************************************************
* @purpose  Gets the  Expiry time before the specified entry in Cache Table will be aged out
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *expTime          @b{(output)} Expiry Time
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheExpiryTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_uint32 *expTime)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    L7_int32 expiryTime = 0;

    if (appTimerTimeLeftGet(mgmdCB->timerHandle,mgmd_group->groupTimer, &expiryTime) != L7_SUCCESS)
    {
      MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE, " failed to get left over time for grp=",
                          &mgmd_group->group);
    }
    *expTime = expiryTime;
  }

  osapiSemaGive(mgmdCB->membership.semId);
  return L7_SUCCESS;
}

/**************************************************************************************
* @purpose  Gets the  time remaining until the router assumes there are no longer
* @purpose  any MGMD version 1 Hosts on the specified interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *version1Time     @b{(output)} version1 Host Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheVersion1HostTimerGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_uint32 *version1Time)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;
  L7_uint32      robustnessvar;
  L7_uint32      queryInterval;
  L7_uint32      responseInterval;
  L7_RC_t        rc;
  L7_uint32      intIfNumber;
  L7_int32 status = L7_FAILURE;  

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNumber) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not Map Rtr interfaceto interface Number\n"); 
    return L7_FAILURE;  
  }
  rc = mgmdMapInterfaceRobustnessGet(mgmdCB->proto, intIfNumber, &robustnessvar);
  if (rc != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not get interface robustness \n");  
    return L7_FAILURE;  
  }
  rc = mgmdMapInterfaceQueryIntervalGet(mgmdCB->proto, intIfNumber, &queryInterval);
  if (rc != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not get interface query interval \n");  
    return L7_FAILURE;  
  }
  rc = mgmdMapInterfaceQueryMaxResponseTimeGet(mgmdCB->proto, intIfNumber, &responseInterval);
  if (rc != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not get query Max Response time \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    if (mgmd_group->v1HostPresentFlag == 1)
    {
      /* If the v1HostTimer is NULL, it hasn't been allocated yet, so, use the interval */
      if (mgmd_group->v1HostTimer != L7_NULL)
      {
        status = appTimerTimeLeftGet(mgmdCB->timerHandle, mgmd_group->v1HostTimer, version1Time);
        if (status != L7_SUCCESS)
        {
          osapiSemaGive(mgmdCB->membership.semId);
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not get the left out time\n");  
          return L7_FAILURE;      
        }
      }
      else
        *version1Time = robustnessvar * queryInterval + responseInterval/10;
    }
    else
    {
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_FAILURE;
    }

    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }
}

/**************************************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    ipaddr            @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheEntryGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }
}

/**************************************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given multi ipaddress & an interface number.
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}   multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @notes    if rtrIfNum = 0 & group = 0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 *rtrIfNum)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, *rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = *rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_NEXT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  *rtrIfNum = mgmd_group->rtrIfNum;
  inetCopy(group, &mgmd_group->group);

  osapiSemaGive(mgmdCB->membership.semId);
  return L7_SUCCESS;
}


/***************************************************************************
* @purpose  Gets the time remaining until the router assumes there are no
* @purpose  longer any MGMD version 2 Hosts for the specified group on the
* @purpose  specified interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *version2Time     @b{(output)} version1 Host Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheVersion2HostTimerGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_uint32 *version2Time)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;
  L7_uint32      robustnessvar;
  L7_uint32      queryInterval;
  L7_uint32      responseInterval;
  L7_RC_t        rc, status;
  L7_uint32      intIfNumber;

  *version2Time = L7_NULL;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNumber) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not Map Rtr interfaceto interface Number\n"); 
    return L7_FAILURE;  
  }

  rc = mgmdMapInterfaceRobustnessGet(mgmdCB->proto, intIfNumber, &robustnessvar);
  if (rc != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not get interface robustness \n");  
    return L7_FAILURE;  
  }

  rc = mgmdMapInterfaceQueryIntervalGet(mgmdCB->proto, intIfNumber, &queryInterval);
  if (rc != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not get interface query interval \n");  
    return L7_FAILURE;  
  }

  rc = mgmdMapInterfaceQueryMaxResponseTimeGet(mgmdCB->proto, intIfNumber, &responseInterval);
  if (rc != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not get query Max Response time \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    if (mgmd_group->v2HostPresentFlag == 1)
    {
      /* If the v2HostTimer is NULL, it hasn't been allocated yet, so, use the interval */
      if (mgmd_group->v2HostTimer != L7_NULL)
      {
        status = appTimerTimeLeftGet(mgmdCB->timerHandle, mgmd_group->v2HostTimer, version2Time);
        if (status != L7_SUCCESS)
        {
          osapiSemaGive(mgmdCB->membership.semId);
          MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not get the left out time\n");  
          return L7_FAILURE;      
        }
      }
      else
        *version2Time = robustnessvar * queryInterval + responseInterval/10;
    }
    else
    {
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_FAILURE;
    }

    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }
}

/***************************************************************************
* @purpose  Gets the compatibility mode (v1, v2 or v3) for the specified
* @purpose  group on the specified interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *groupCompatMode  @b{(output)} group compatibility mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheGroupCompatModeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_uint32 *groupCompatMode)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    if(mgmdCB->proto == L7_AF_INET)
    {
      *groupCompatMode = mgmd_group->groupCompatMode;
    }
    else if(mgmdCB->proto == L7_AF_INET6)
    {
      /* Compatibility mode should be decrimented by 1 for MLD as 
         while storing it plus one is inherently being added since
         the common peice of code is used for both MLD and IGMP
         in many places.
       */

      *groupCompatMode = (mgmd_group->groupCompatMode) - 1;
    }
    
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }
}

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
* @purpose  group on the specified interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *sourceFilterMode @b{(output)} source filter mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheGroupSourceFilterModeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, 
                                          L7_uint32 *sourceFilterMode)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    *sourceFilterMode = mgmd_group->filterMode;
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }
}

/*****************************************************************************
* @purpose  To validate if the specified interface contains a cache entry for
* @purpose  the specified group or not
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    group             @b{(output)}  multicast group ip address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheIntfEntryGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_inet_addr_t *group)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }
}

/*****************************************************************************
* @purpose  Get the Next Entry in Cache Table for the specified group and
* @purpose  interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    ipaddr            @b{(output)}  multicast group ip address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes    if rtrIfNum=0 & group=0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheIntfEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 *rtrIfNum, L7_inet_addr_t *group)
{
  mgmd_cb_t     *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t  *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, *rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = *rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_NEXT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    if (inetCopy(group, &(mgmd_group->group)) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_FAILURE;
    }
    *rtrIfNum = mgmd_group->rtrIfNum;

    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }
}

/*****************************************************************************
* @purpose  To validate if the specified source address exists for the specified
* @purpose  group address and interface or not
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}   multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    host              @b{(output)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdSrcListEntryGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_inet_addr_t *host)
{
  mgmd_cb_t               *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t            *mgmd_group, searchKey;
  mgmd_source_record_t    srcSearchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    if (inetCopy(&srcSearchKey.sourceAddress, host) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_FAILURE;
    }

    if (SLLFind(&(mgmd_group->sourceRecords), (L7_sll_member_t *)&srcSearchKey))
    {
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_SUCCESS;
    }
    else
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not find the entry \n");  
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_FAILURE;
    }
  }
}

/*****************************************************************************
* @purpose  Get the Next valid Group and Interface entry in the order of (Grp, Intf)
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(inout)}   multicast group ip address
* @param    rtrIfNum          @b{(inout)}   Router Interface Number
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
*
* @end
*********************************************************************/

L7_RC_t mgmdGrpIntfEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *searchKeyGroup, L7_uint32 *searchKeyRtrIfNum)
{
  mgmd_cb_t               *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  L7_inet_addr_t minGroup;
  L7_uint32 minRtrIfNum;
  mgmd_group_t *current_group, current_searchKey;
  L7_BOOL  intfFound = L7_FALSE;
  L7_BOOL  grpFound = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;
  mgmd_group_t *mgmd_group = L7_NULLPTR;

  if (mgmdVerify(mgmdCB, *searchKeyRtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  inetAddressZeroSet(mgmdCB->proto, &(current_searchKey.group));
  inetAddressZeroSet(mgmdCB->proto, &(minGroup));
  current_searchKey.rtrIfNum = 1;
  minRtrIfNum = (L7_uint32)(-1);

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n");
    return L7_FAILURE;
  }

  while((current_group=avlSearchLVL7(&(mgmdCB->membership), &current_searchKey, AVL_NEXT)))
  {
     if (inetCopy(&current_searchKey.group, &(current_group->group)) != L7_SUCCESS)
     {
        rc = L7_FAILURE;
        break;
      }
      current_searchKey.rtrIfNum = current_group->rtrIfNum;

      if (L7_INET_ADDR_COMPARE((&current_group->group), searchKeyGroup) < 0)
      {
           continue;
      }
      if (L7_INET_ADDR_COMPARE((&current_group->group), searchKeyGroup) == 0)
      {
         if (current_group->rtrIfNum <= (*searchKeyRtrIfNum))
         {
            continue;
         }
         if ((grpFound == L7_TRUE) && (current_group->rtrIfNum > (*searchKeyRtrIfNum)))
         {
           minRtrIfNum = (L7_uint32)(-1);
         }
         if (current_group->rtrIfNum < minRtrIfNum)
         {
           if (inetCopy(&minGroup, &(current_group->group)) != L7_SUCCESS)
           {
              rc = L7_FAILURE;
              break;
           }
           mgmd_group = current_group;
           intfFound = L7_TRUE;
           grpFound  = L7_FALSE; /* Make Next Group as invalid if we have next valid
                                       interface entry for the given group */
         }
         else
         {
           continue;
         }
      }/* end if grp equal */
      else
      {
        if ((L7_INET_ADDR_COMPARE((&current_group->group), &(minGroup)) <= 0 ) ||
                inetIsAddressZero(&(minGroup)))
           {
              if (intfFound != L7_TRUE)
              {
                if (L7_INET_ADDR_COMPARE((&current_group->group), &(minGroup)) == 0)
                {
                  if (minRtrIfNum > current_group->rtrIfNum)
                  {
                    minRtrIfNum =current_group->rtrIfNum;
                    mgmd_group = current_group;
                    continue;
                  }
                }
                else
                {
                  if (inetCopy(&minGroup, &(current_group->group)) != L7_SUCCESS)
                  {
                    rc = L7_FAILURE;
                    break;
                  }
                  mgmd_group = current_group;
                  minRtrIfNum =current_group->rtrIfNum;
                  grpFound = L7_TRUE;
                }
             }
          } /* end if grp greater */

         }
      } /* end of while next */
      if (((intfFound == L7_FALSE) && (grpFound == L7_FALSE))
          || (mgmd_group == L7_NULLPTR))
      {
        rc = L7_FAILURE;
      }
      else if (mgmd_group != L7_NULLPTR )
      {
        *searchKeyRtrIfNum = mgmd_group->rtrIfNum;
        if (inetCopy(searchKeyGroup,  &(mgmd_group->group)) != L7_SUCCESS)
        {
          rc = L7_FAILURE;
        }
      }
      osapiSemaGive(mgmdCB->membership.semId);
      return rc;
} 
/*****************************************************************************
* @purpose  Get the Next valid Source List Entry 
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(inout)}   multicast group ip address
* @param    rtrIfNum          @b{(inout)}   Router Interface Number
* @param    host              @b{(inout)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
*
* @end
*********************************************************************/
L7_RC_t mgmdSrcListEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 *rtrIfNum, L7_inet_addr_t *host)
{
  mgmd_cb_t               *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t            *mgmd_group, searchKey;
  mgmd_source_record_t    *srcRec, srcSearchKey;

  if (mgmdVerify(mgmdCB, *rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = *rtrIfNum;
  inetCopy(&searchKey.group, group);

  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_NEXT)) == L7_NULLPTR)
    {
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_FAILURE;
    }
  }

  if (mgmd_group != L7_NULLPTR)
  {
    memset(&srcSearchKey, 0, sizeof(srcSearchKey));
    inetCopy(&srcSearchKey.sourceAddress, host);

    srcRec = (mgmd_source_record_t *)(SLLFindNext(&(mgmd_group->sourceRecords),
                                               (L7_sll_member_t *)&srcSearchKey));

    if (srcRec == L7_NULLPTR)
    {
      inetCopy(&searchKey.group, &mgmd_group->group);
      searchKey.rtrIfNum = mgmd_group->rtrIfNum;
      mgmd_group = L7_NULLPTR;
      if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_NEXT)) == L7_NULLPTR)
      {
        osapiSemaGive(mgmdCB->membership.semId);
        return L7_FAILURE;
      }
    }
    if (srcRec != L7_NULLPTR)
    {
      inetCopy(host, &(srcRec->sourceAddress));
      inetCopy(group,&mgmd_group->group);
      *rtrIfNum = mgmd_group->rtrIfNum;
      osapiSemaGive(mgmdCB->membership.semId);
      return L7_SUCCESS;
    } /* end if srcRec */
  }
  osapiSemaGive(mgmdCB->membership.semId);
  return L7_FAILURE;
}
/*******************************************************************************
* @purpose  Get the amount of time until the specified Source Entry is aged out
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}   multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    host              @b{(input)}   source address
* @param    srcExpiryTime     @b{(output)}  source expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdSrcExpiryTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_inet_addr_t *host, 
                             L7_uint32 *srcExpiryTime)
{
  mgmd_cb_t               *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t            *mgmd_group, searchKey;
  mgmd_source_record_t    *srcRec, srcSearchKey;
  L7_uint32                timeLeft = 0;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }

  if (inetCopy(&srcSearchKey.sourceAddress, host) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }

  srcRec = (mgmd_source_record_t *) SLLFind(&(mgmd_group->sourceRecords), 
                                            (L7_sll_member_t *)&srcSearchKey);
  if (srcRec != L7_NULLPTR)
  {
    if (srcRec->sourceTimer != L7_NULLPTR)
    {
      if (appTimerTimeLeftGet(mgmdCB->timerHandle,srcRec->sourceTimer, &timeLeft) != L7_SUCCESS)
      {
        MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE, " failed to get left over time for src=",
                            &srcRec->sourceAddress);
      }
      *srcExpiryTime  = timeLeft;
    }
    else 
    {
      *srcExpiryTime = 0;
    }

    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }

  *srcExpiryTime = 0;
  osapiSemaGive(mgmdCB->membership.semId);
  return L7_FAILURE;
}

/*******************************************************************************
* @purpose  Get the number of source records for the specified group and interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    srcCount          @b{(output)} number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdSrcCountGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum, L7_uint32 *srcCount)
{
  mgmd_cb_t               *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_group_t            *mgmd_group, searchKey;

  if (mgmdVerify(mgmdCB, rtrIfNum ) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Parameters Verification Failed  \n"); 
    return L7_FAILURE;  
  }

  if (osapiSemaTake(mgmdCB->membership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Could not take the samaphore\n"); 
    return L7_FAILURE;
  }

  searchKey.rtrIfNum = rtrIfNum;
  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  if ((mgmd_group=avlSearchLVL7(&(mgmdCB->membership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_FAILURE;
  }
  else
  {
    *srcCount = SLLNumMembersGet(&(mgmd_group->sourceRecords));
    osapiSemaGive(mgmdCB->membership.semId);
    return L7_SUCCESS;
  }
}

/******************************************************************************
* @purpose  Multicast routing protocols De-register their callbacks
*           with MGMD using this API for getting the membership changes.
*
* @param    mgmdCB    @b{(input)}    MGMD CB 
* @param    rtrIfNum    @b{(input)}   router Interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
L7_RC_t  mgmdVerify(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum)
{
  mgmd_cb_t               *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  L7_uint32 intIfNum;

  if (mgmdCB == L7_NULL || 
      ((rtrIfNum !=0) && (ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) != L7_SUCCESS)))
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Parameters are not proper \n");  
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets thecorresponding counter value
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    counterType           @b{(output)}  Type of the counter
*                 value                         value of the counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdCounterGet(MCAST_CB_HNDL_t mgmdCBHndl,L7_uint32 counterType,L7_uint32* value)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdCB == L7_NULL)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Control block pointer is null. \n"); 
    return L7_FAILURE;
  }

  switch (counterType)
  {
    case L7_MGMD_TOTAL_PKTS_RECV:
      {
        *value = mgmdCB->counters.TotalPacketsReceived;
      }
      break;
    case L7_MGMD_TOTAL_PKTS_SENT:
      {
        *value = mgmdCB->counters.TotalPacketsSent;
      }
      break;
    case L7_MGMD_QUERIES_RECV:
      {
        *value = mgmdCB->counters.QueriesReceived;
      }
      break;
    case L7_MGMD_QUERIES_SENT:
      {
        *value = mgmdCB->counters.QueriesSent;
      }
      break;
    case L7_MGMD_REPORTS_RECV:
      {
        *value = mgmdCB->counters.ReportsReceived;
      }
      break;
    case L7_MGMD_REPORTS_SENT:
      {
        *value = mgmdCB->counters.ReportsSent;
      }
      break;
    case L7_MGMD_LEAVES_RECV:
      {
        *value = mgmdCB->counters.LeavesReceived;
      }
      break;
    case L7_MGMD_LEAVES_SENT:
      {
        *value = mgmdCB->counters.LeavesSent;
      }
      break;
    case L7_MGMD_BAD_CKSM_PKTS:
      {
        *value = mgmdCB->counters.BadChecksumPackets;
      }
      break;
    case L7_MGMD_MALFORM_PKTS :
      {
        *value = mgmdCB->counters.MalformedPackets;
      }
      break;
    default:
      break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clears all the global counters
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdTrafficCountersClear(MCAST_CB_HNDL_t mgmdCBHndl) 
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdCB == L7_NULL)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " Control block pointer is null. \n");  
    return L7_FAILURE;
  }

  memset(&mgmdCB->counters,0,sizeof(mgmd_counters_t));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clears all mld interface counters
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    rtrIntf           @b{(input)}   router Interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdCountersClear(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIntf) 
{
  mgmd_cb_t   *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_info_t *mgmd_info = L7_NULLPTR;
  L7_uchar8   rtrIndex = L7_NULL;

  if (mgmdCB == L7_NULL)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Control block pointer is null. \n"); 
    return L7_FAILURE;
  }
  if (rtrIntf != L7_NULL)
  {
    mgmd_info = &(mgmdCB->mgmd_info[rtrIntf]);
    if (mgmd_info == L7_NULLPTR)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD Info pointer is null. \n"); 
      return L7_FAILURE;
    }
    mgmd_info->Wrongverqueries = L7_NULL;
    mgmd_info->numOfJoins = L7_NULL;
    mgmd_info->numOfGroups = L7_NULL;
  }
  else
  {
    /* clearing mgmd counter for all interfaces. */
    for (rtrIndex = MGMD_ONE; rtrIndex < MAX_INTERFACES; rtrIndex++)
    {
      mgmd_info = &(mgmdCB->mgmd_info[rtrIndex]);
      if (mgmd_info != L7_NULLPTR)
      {
        mgmd_info->Wrongverqueries = L7_NULL;
        mgmd_info->numOfJoins = L7_NULL;
        mgmd_info->numOfGroups = L7_NULL;
      }
    }
  }
  return L7_SUCCESS;
}

