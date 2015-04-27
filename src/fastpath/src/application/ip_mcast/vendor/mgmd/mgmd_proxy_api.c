/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  mgmd_proxy_vend_api.c
*
* @purpose   MGMD Proxy vendor-specific API functions
*
* @component Mgmd Mapping Layer
*
* @comments  none
*
* @create    April 07, 2006
*
* @author    akamlesh
*
* @end
*
**********************************************************************/

#include "mgmd.h"
#include "mgmd_proxy.h"

/*********************************************************************
* @purpose  To validate mgmd CB and router inteface number
*
* @param    mgmdCB      @b{(input)}   MGMD Control Block
* @param    rtrIfNum    @b{(input)}   Router Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t mgmdProxyVerifyParameters(mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum)
{
  if (mgmdCB == L7_NULL ||
       mgmdCB->mgmd_host_info == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (mgmdCB->mgmd_proxy_ifIndex != rtrIfNum)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/****************************************************************************
* @pupose  Gets the Proxy Version1 Querier Time for the specified interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    Timer       @b{(output)}  Querier Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t mgmdProxyIntfVer1QuerierTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32 *timer)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  if (appTimerTimeLeftGet(mgmdCB->timerHandle, mgmdCB->mgmd_host_info->ver1_querier_timer, timer) != L7_SUCCESS)
    return L7_FAILURE;

  if (*timer < 0)
    return L7_FAILURE;
  else
    return L7_SUCCESS;
}

/****************************************************************************
* @pupose  Gets the Proxy Version1 Querier Time for the specified interface
*
* @param    rtrIfNum              @b{(input)}   Router Interface Number
* @param    Timer  @b{(output)}   Querier  Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t mgmdProxyIntfVer2QuerierTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32* timer)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  if (appTimerTimeLeftGet(mgmdCB->timerHandle, mgmdCB->mgmd_host_info->ver2_querier_timer, timer) != L7_SUCCESS)
    return L7_FAILURE;

  if (timer < 0)
    return L7_FAILURE;
  else
    return L7_SUCCESS;
}

/************************************************************************
* @purpose  Gets the Proxy Restart Count for the specified interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum   @b{(input)}   Router Interface Number
* @param    count      @b{(output)}  count
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* 
* @notes    none
* 
* @end
*************************************************************************/
L7_RC_t mgmdProxyIntfRestartCountGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* count)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *count =  mgmdCB->mgmd_proxy_restartCount ;
  return L7_SUCCESS;
}


/***********************************************************************
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
**********************************************************************/
L7_RC_t mgmdProxyCacheLastReporterGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                      L7_inet_addr_t *ipAddr)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t  *mgmd_host_info;
  mgmd_host_group_t *mgmd_host_group, searchKey;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    if (inetCopy(ipAddr, &(mgmd_host_group->last_reporter)) != L7_SUCCESS)
    {
      osapiSemaGive(mgmd_host_info->hostMembership.semId);
      return L7_FAILURE;  
    }
  }

  osapiSemaGive(mgmd_host_info->hostMembership.semId);
  return L7_SUCCESS;
}


/***************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache
*           Table for the specified group address & the specified interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    *upTime           @b{(output)} Up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t mgmdProxyCacheUpTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                L7_uint32 *upTime)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t  *mgmd_host_info;
  mgmd_host_group_t *mgmd_host_group, searchKey;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    *upTime  = (osapiUpTimeRaw()) - mgmd_host_group->uptime ;
  }

  osapiSemaGive(mgmd_host_info->hostMembership.semId);
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
* @purpose  group on the specified interface
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *groupFilterMode  @b{(output)} source filter mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Proxy Cache Table
*
* @end
******************************************************************************/
L7_RC_t mgmdProxyCacheGroupFilterModeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                         L7_uint32 *groupFilterMode)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t  *mgmd_host_info;
  mgmd_host_group_t *mgmd_host_group, searchKey;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    *groupFilterMode = mgmd_host_group->filterMode;
  }

  osapiSemaGive(mgmd_host_info->hostMembership.semId);
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    ipaddr            @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCacheEntryGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group ,L7_uint32 rtrIfNum)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t  *mgmd_host_info;
  mgmd_host_group_t *mgmd_host_group, searchKey;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  osapiSemaGive(mgmd_host_info->hostMembership.semId);
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given 
*           multi ipaddress & an interface number.
*
* @param    mgmdCBHndl    @b{(input)}   MGMD CB Handle
* @param    group         @b{(input)}  multicast group ip address
* @param    rtrIfNum      @b{(input)}  Router Interface Number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @notes    if rtrIfNum = 0 & group = 0, 
*           then return the first valid entry in the cache table
*
* @end
**********************************************************************/
L7_RC_t mgmdProxyCacheEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 *rtrIfNum)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t  *mgmd_host_info;
  mgmd_host_group_t *mgmd_host_group, searchKey;

  if (mgmdProxyVerifyParameters(mgmdCB, *rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;


  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_NEXT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  inetCopy(group, &mgmd_host_group->group);
  osapiSemaGive(mgmd_host_info->hostMembership.semId);
  return L7_SUCCESS;
}


/***********************************************************************
* @purpose  Gets the Proxy v3 queries received on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v3QueriesRcvd       @b{(output)} v3QueriesRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV3QueriesRecvdGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32 *v3QueriesRcvd)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v3QueriesRcvd =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_QUERY_V3][MGMD_STATS_RX];
  return L7_SUCCESS;
}

/**************************************************************************** 
* @purpose  Gets the Proxy v3 reports received  for the specified interface
*
* @param    mgmdCBHndl           @b{(input)}   MGMD CB Handle
* @param    rtrIfNum             @b{(input)}   Router Interface Number
* @param    v3ReportsRcvd        @b{(output)}  v3ReportsRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t mgmdProxyV3ReportsRecvdGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* v3ReportsRcvd)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v3ReportsRcvd =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_REPORT_V3][MGMD_STATS_RX];
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Proxy v3 reports Sent for the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v3ReportsSent       @b{(output)}  v3ReportsSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV3ReportsSentGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* v3ReportsSent)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v3ReportsSent =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_REPORT_V3][MGMD_STATS_TX];
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Gets the Proxy v2 queries received on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v2QueriesRcvd       @b{(output)}  v2QueriesRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t mgmdProxyV2QueriesRecvdGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* v2QueriesRcvd)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v2QueriesRcvd =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_QUERY_V2][MGMD_STATS_RX];
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Gets the Proxy v2 reports received on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v2ReportsRcvd       @b{(output)}  v2ReportsRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV2ReportsRecvdGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_uint32* v2ReportsRcvd)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v2ReportsRcvd =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_REPORT_V2][MGMD_STATS_RX];
  return L7_SUCCESS;
}

/***********************************************************************
* @purpose  Gets the Proxy v2 reports sent on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v2ReportsSent       @b{(output)}  v2ReportsSent 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV2ReportsSentGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* v2ReportsSent)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v2ReportsSent =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_REPORT_V2][MGMD_STATS_TX];
  return L7_SUCCESS;
}


/*************************************************************************
* @purpose  Gets the Proxy v2 leaves received on the specified interface
*
* @param    mgmdCBHndl      @b{(input)}   MGMD CB Handle
* @param    rtrIfNum        @b{(input)}   Router Interface Number
* @param    v2LeavesRcvd    @b{(output)}  v2LeavesRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV2LeavesRecvdGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32 *v2LeavesRcvd)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v2LeavesRcvd =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_LEAVE_V2][MGMD_STATS_RX];
;
  return L7_SUCCESS;
}

/************************************************************************
* @purpose  Gets the Proxy v2 leaves sent on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v2LeavesSent        @b{(output)}  v2LeavesSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t mgmdProxyV2LeavesSentGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* v2LeavesSent)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v2LeavesSent =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_LEAVE_V2][MGMD_STATS_TX];
  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Gets the Proxy v1 queries received on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v1QueriesRcvd       @b{(output)}  v1QueriesRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV1QueriesRecvdGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* v1QueriesRcvd)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v1QueriesRcvd =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_QUERY_V1][MGMD_STATS_RX];
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Gets the Proxy v1 reports received on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v1ReportsRcvd       @b{(output)}  v1ReportsRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t mgmdProxyV1ReportsRecvdGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* v1ReportsRcvd)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v1ReportsRcvd =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_REPORT_V1][MGMD_STATS_RX];
  return L7_SUCCESS;
}

/************************************************************************
* @purpose  Gets the Proxy v1 reports sent on the specified interface
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v1ReportsSent       @b{(output)}  v1ReportsSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t mgmdProxyV1ReportsSentGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* v1ReportsSent)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *v1ReportsSent =  mgmdCB->mgmd_host_info->proxyIntfStats[MGMD_REPORT_V1][MGMD_STATS_TX];
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Proxy interface number configured for
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    ifindex     @b(output)    index of the proxy interface
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyInterfaceGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_uint32* ifIndex)
{
  mgmd_cb_t         *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  return(ipMapRtrIntfToIntIfNum(rtrIfNum, ifIndex));
}


/*********************************************************************
* @purpose  To validate if the specified source address exists for 
*           the specified group address and interface or not
*
* @param    mgmdCBHndl        @b{(input)}  MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxySrcListEntryGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                 L7_inet_addr_t *host)
{
  mgmd_host_group_t         *mgmd_host_group, searchKey;
  mgmd_host_info_t          *mgmd_host_info;
  mgmd_host_source_record_t *srcRec, srcSearchKey;
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;


  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    memset(&srcSearchKey, 0, sizeof(srcSearchKey));
    inetCopy(&srcSearchKey.sourceAddress, host);
    srcRec = (mgmd_host_source_record_t *)(SLLFind(&(mgmd_host_group->sourceRecords),
                                                   (L7_sll_member_t *)&srcSearchKey));
    if (srcRec)
    {
      osapiSemaGive(mgmd_host_info->hostMembership.semId);
      return L7_SUCCESS;
    }
    else
    {
      osapiSemaGive(mgmd_host_info->hostMembership.semId);
      return L7_FAILURE;
    }
  }
}

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
* @purpose  interface and source address
*
* @param    mgmdCBHndl        @b{(input)}  MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes    if currentHostIp = 0 return the first source address in that 
*           group address 
*
* @end
******************************************************************************/
L7_RC_t mgmdProxySrcListEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 *rtrIfNum,
                                     L7_inet_addr_t *host)
{
  mgmd_host_group_t         *mgmd_host_group, searchKey;
  mgmd_host_info_t          *mgmd_host_info;
  mgmd_host_source_record_t *srcRec, srcSearchKey;
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, *rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    if (inetCopy(&srcSearchKey.sourceAddress, host) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
      osapiSemaGive(mgmd_host_info->hostMembership.semId);
      return L7_FAILURE;
    }

    srcRec = (mgmd_host_source_record_t *)(SLLFindNext(&(mgmd_host_group->sourceRecords),
                                                       (L7_sll_member_t *)&srcSearchKey));

    if (srcRec)
    {
      if (inetCopy(host, &(srcRec->sourceAddress)) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
        osapiSemaGive(mgmd_host_info->hostMembership.semId);
        return L7_FAILURE;  
      }
      osapiSemaGive(mgmd_host_info->hostMembership.semId);
      return L7_SUCCESS;
    }
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  To get the source expiry time
*
* @param    mgmdCBHndl        @b{(input)}  MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    hostAddr          @b{(input)}  source address
* @param    expiryTime        @b{(output)} source Expiry Time
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxySrcListExpiryTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                      L7_inet_addr_t *host, L7_uint32 *srcExpiryTime)
{
  mgmd_host_group_t         *mgmd_host_group, searchKey;
  mgmd_host_info_t          *mgmd_host_info;
  mgmd_host_source_record_t *srcRec, srcSearchKey;
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;


  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    memset(&srcSearchKey, 0, sizeof(srcSearchKey));
    inetCopy(&srcSearchKey.sourceAddress, host);
    srcRec = (mgmd_host_source_record_t *)(SLLFind(&(mgmd_host_group->sourceRecords),
                                                   (L7_sll_member_t *)&srcSearchKey));
    if (srcRec)
    {
      L7_uint32 upTime;
      
      upTime  = (osapiUpTimeRaw()) - srcRec->sourceCtime;

      if (mgmd_host_group->filterMode == MGMD_FILTER_MODE_EXCLUDE)
      {
        if (srcRec->sourceCtime == 0)
          *srcExpiryTime = 0;
        else
          *srcExpiryTime  = srcRec->sourceInterval - upTime;
      }
      else
      {
        *srcExpiryTime  = srcRec->sourceInterval - upTime;
      }

      if ((L7_int32)(*srcExpiryTime) < 0)
        *srcExpiryTime = 0;

      osapiSemaGive(mgmd_host_info->hostMembership.semId);
      return L7_SUCCESS;
    }
    else
    {
      *srcExpiryTime = 0;
      osapiSemaGive(mgmd_host_info->hostMembership.semId);
      return L7_FAILURE;
    }
  }
}

/*********************************************************************
* @purpose  Resets the proxy statistics parameter 
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyResetHostStatus(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum)
{
  mgmd_host_info_t          *mgmd_host_info;
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed verification of params for rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
   

  mgmd_host_info = mgmdCB->mgmd_host_info;

  memset(&mgmd_host_info->proxyIntfStats, 0, sizeof(mgmd_host_info->proxyIntfStats));
  return L7_SUCCESS;
}

/*****************************************************************************
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
******************************************************************************/
L7_RC_t mgmdProxyIntfGroupsGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_int32 *numOfGroups)
{
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  *numOfGroups = mgmdCB->mgmd_host_info->numOfGroups;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To get the state of the member 
*
* @param    mgmdCBHndl        @b{(input)}  MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    hostState         @b((output)) state of the group address 
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyGroupStatusGet (MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                 L7_uint32 *hostState)
{
  mgmd_host_group_t         *mgmd_host_group, searchKey;
  mgmd_host_info_t          *mgmd_host_info;
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    *hostState = mgmd_host_group->hostState;
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_SUCCESS;
  }
}

/**********************************************************************
* @purpose  Get the number of source records for the specified group 
*           and interface
*
* @param    mgmdCBHndl        @b{(input)}  MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    srcCount          @b{(output)} number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t mgmdProxySrcCountGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group,L7_uint32 rtrIfNum,L7_uint32 *srcCount)
{
  mgmd_host_group_t         *mgmd_host_group, searchKey;
  mgmd_host_info_t          *mgmd_host_info;
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    *srcCount = mgmd_host_group->sourceRecords.sllNumElements;
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_SUCCESS;
  }
}

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
**********************************************************************/
L7_RC_t mgmdProxyIntfQuerierGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum,L7_inet_addr_t *querierIP)
{
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  inetCopy(querierIP, &(mgmdCB->mgmd_host_info->querierIP));
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the status of the row in the host cache table
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    group       @b{(input)}   Internal Group Address
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    *status     @b{(output)}  Enabled or Disabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCacheStatusGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group, L7_uint32 rtrIfNum,
                                L7_uint32 *status)
{
  mgmd_host_group_t         *mgmd_host_group, searchKey;
  mgmd_host_info_t          *mgmd_host_info;
  mgmd_cb_t                 *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  *status = L7_DISABLE;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if (osapiSemaTake(mgmd_host_info->hostMembership.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (inetCopy(&searchKey.group, group) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " InetCopy Failed. \n");  
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }

  if ((mgmd_host_group=avlSearchLVL7(&(mgmd_host_info->hostMembership), &searchKey, AVL_EXACT)) == L7_NULLPTR)
  {
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_FAILURE;
  }
  else
  {
    *status = L7_ENABLE;
    osapiSemaGive(mgmd_host_info->hostMembership.semId);
    return L7_SUCCESS;
  }
}


/*********************************************************************
* @purpose  Gets the next entry for Proxy Interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyInterfaceEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 *rtrIfNum)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, *rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the next entry for Proxy Interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    groupAddr   @b{(output)}  Internal Group Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxymgmdProxyInterfaceGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_inet_addr_t *groupAddr)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the next entry for Proxy Interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    ipaddr      @b{(output)}  Internal Group Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyInverseCacheEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 *rtrIfNum, L7_inet_addr_t *ipaddr)
{
  mgmd_cb_t *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;

  if (mgmdProxyVerifyParameters(mgmdCB, *rtrIfNum) == L7_FAILURE)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the MGMD  IP Multicast route table Next entry
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
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
L7_RC_t mgmdProxyIpMRouteEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                      L7_inet_addr_t* ipMRouteGroup, 
                                      L7_inet_addr_t* ipMRouteSource,
                                      L7_inet_addr_t* ipMRouteSourceMask)
{
  mgmd_cb_t             *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t      *mgmd_host_info;
  mgmdProxyCacheEntry_t *mrtEntry;

  if (mgmdCB == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if(mgmd_host_info == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to Acquire MGMD MRT Semaphore.\n");
    return L7_FAILURE;
  }

  if (osapiSemaTake(mgmd_host_info->mgmdProxyCache.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB, ipMRouteSource, ipMRouteGroup))
      == L7_NULLPTR)
  {
    osapiSemaGive (mgmd_host_info->mgmdProxyCache.semId);
    return L7_FAILURE;
  }

  if (inetCopy (ipMRouteGroup, &(mrtEntry->grpAddr)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to copy grpAddr");
    osapiSemaGive (mgmd_host_info->mgmdProxyCache.semId);
    return L7_FAILURE;
  }
  if (inetCopy (ipMRouteSource, &(mrtEntry->srcAddr)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to copy srcAddr");
    osapiSemaGive (mgmd_host_info->mgmdProxyCache.semId);
    return L7_FAILURE;
  }

  osapiSemaGive(mgmd_host_info->mgmdProxyCache.semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the minimum amount of time remaining before  this
*           entry will be aged out.
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
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
L7_RC_t mgmdProxyIpMRouteExpiryTimeGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                               L7_inet_addr_t *ipMRtGrp,
                                               L7_inet_addr_t *ipMRtSrc,
                                               L7_inet_addr_t *ipMRtSrcMask,
                                               L7_uint32       *expire)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the time since the multicast routing information
*           represented by this entry was learned by the router.
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
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
L7_RC_t mgmdProxyIpMRouteUpTimeGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                   L7_inet_addr_t *ipMRtGrp,
                                   L7_inet_addr_t *ipMRtSrc,
                                   L7_inet_addr_t *ipMRtSrcMask,
                                   L7_uint32       *upTime)
{
  mgmd_cb_t             *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t      *mgmd_host_info;
  mgmdProxyCacheEntry_t *mrtEntry;

  if (mgmdCB == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if(mgmd_host_info == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to Acquire MGMD MRT Semaphore.\n");
    return L7_FAILURE;
  }

  if (osapiSemaTake(mgmd_host_info->mgmdProxyCache.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((mrtEntry = mgmdProxyMRTCacheEntryGet(mgmdCB, ipMRtSrc, ipMRtGrp))
      == L7_NULLPTR)
  {
    osapiSemaGive (mgmd_host_info->mgmdProxyCache.semId);
    return L7_FAILURE;
  }

  *upTime = osapiUpTimeRaw() - mrtEntry->entryUpTime;

  osapiSemaGive(mgmd_host_info->mgmdProxyCache.semId);
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Gets the Rpf address for the given index
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
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
L7_RC_t mgmdProxyIpMRouteRpfAddrGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRtSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_inet_addr_t *rpfAddress)
{
  return L7_NOT_SUPPORTED;
}
/**********************************************************************
* @purpose  Gets the number of MRT entries in Proxy table.
*
* @param    mgmdCBHndl   @b{(input)} MGMD CB Handle
* @param    count        @b{(input)} Number of MRT entries in Proxy table
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyIpMrouteEntryCountGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                       L7_uint32 *count)
{
  mgmd_cb_t             *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t      *mgmd_host_info;
  
  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to get MGMD control block\n");
    return L7_FAILURE;
  }

  if (count == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid input parameters\n");
    return L7_FAILURE;
  }
  mgmd_host_info = mgmdCB->mgmd_host_info;
  if(mgmd_host_info == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to get MGMD host info\n");
    return L7_FAILURE;
  }
  *count = avlTreeCount(&mgmd_host_info->mgmdProxyCache);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gets the interface index on which IP datagrams sent by
*           these sources to this multicast address are received.
*           corresponding to the index received.
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    ipMRtGrp       @b{(input)}  Multicast Group address
* @param    ipMRouteSrc    @b{(input)}  Source address
* @param    ipMRtSrcMask   @b{(input)}  Mask Address
* @param    rtrIfNum       @b{(output)} router interface number
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
L7_RC_t mgmdProxyIpMRouteIfIndexGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRtSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_uint32      *rtrIfNum)
{
  mgmd_cb_t             *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t      *mgmd_host_info;
  mgmdProxyCacheEntry_t *mrtEntry;

  if (mgmdCB == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if(mgmd_host_info == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to Acquire MGMD MRT Semaphore.\n");
    return L7_FAILURE;
  }

  if (osapiSemaTake(mgmd_host_info->mgmdProxyCache.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((mrtEntry = mgmdProxyMRTCacheEntryGet(mgmdCB, ipMRtSrc, ipMRtGrp))
      == L7_NULLPTR)
  {
    osapiSemaGive (mgmd_host_info->mgmdProxyCache.semId);
    return L7_FAILURE;
  }

  *rtrIfNum = mrtEntry->upstrmRtrIfNum;

  osapiSemaGive(mgmd_host_info->mgmdProxyCache.semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    ipMRtGrp           @b{(input)} mcast group
* @param    ipMRtSrc           @b{(input)} mcast source
* @param    ipMRtSrcMask       @b{(input)} source mask
* @param    rtrIfNum           @b{(inout)} router interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*****************************************************************/
L7_RC_t mgmdProxyRouteOutIntfEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                          L7_inet_addr_t *ipMRtGrp,
                                          L7_inet_addr_t *ipMRtSrc,
                                          L7_inet_addr_t *ipMRtSrcMask,
                                          L7_uint32      *outRtrIfNum)
{
  mgmd_cb_t             *mgmdCB = (mgmd_cb_t *)mgmdCBHndl;
  mgmd_host_info_t      *mgmd_host_info;
  mgmdProxyCacheEntry_t *mrtEntry;
  L7_uint32              rtrIfNum;
  L7_RC_t                retVal = L7_FAILURE;

  if (mgmdCB == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  mgmd_host_info = mgmdCB->mgmd_host_info;
  if(mgmd_host_info == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to Acquire MGMD MRT Semaphore.\n");
    return L7_FAILURE;
  }

  if (osapiSemaTake(mgmd_host_info->mgmdProxyCache.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((mrtEntry = mgmdProxyMRTCacheEntryGet(mgmdCB, ipMRtSrc, ipMRtGrp))
      == L7_NULLPTR)
  {
    osapiSemaGive (mgmd_host_info->mgmdProxyCache.semId);
    return L7_FAILURE;
  }

  if (*outRtrIfNum == 0)
  {
    for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      if (MCAST_BITX_TEST (mrtEntry->oifList, rtrIfNum))
      {
        *outRtrIfNum = rtrIfNum;
        retVal = L7_SUCCESS;
        break;
      }
    }
  }
  else
  {
    for (rtrIfNum = *outRtrIfNum+1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      if (MCAST_BITX_TEST (mrtEntry->oifList, rtrIfNum))
      {
        *outRtrIfNum = rtrIfNum;
        retVal = L7_SUCCESS;
        break;
      }
    }
  }

  osapiSemaGive(mgmd_host_info->mgmdProxyCache.semId);
  return retVal;
}
