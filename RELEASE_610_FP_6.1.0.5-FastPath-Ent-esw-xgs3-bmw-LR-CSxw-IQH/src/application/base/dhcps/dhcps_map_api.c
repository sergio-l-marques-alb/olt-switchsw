/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 * @filename  dhcps_map_api.c
 *
 * @purpose   DHCP Server API functions
 *
 * @component DHCP Server Mapping Layer
 *
 * @comments      
 *
 * @create    09/22/2002
 *
 * @author    athakur
 *
 * @end
 *             
 **********************************************************************/

#include "l7_dhcpsinclude.h"

extern L7_dhcpsMapCfg_t     *pDhcpsMapCfgData;
extern L7_dhcpsMapLeaseCfg_t *pDhcpsMapLeaseCfgData;
extern dhcpsInfo_t          *pDhcpsInfo;
extern dhcpsMapCtrl_t        dhcpsMapCtrl_g;

/*:ignore*/
/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
 * @purpose  Get the DHCP Server administrative mode 
 *
 * @param    *pMode   @b{(output)} admin mode L7_ENABLE or L7_DISABLE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapAdminModeGet(L7_uint32 *pMode)
{
  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    *pMode = 0;                /* neither enabled nor diabled */
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pMode = pDhcpsMapCfgData->dhcps.dhcpsAdminMode;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Set the DHCP Server administrative mode
 *
 * @param    mode   @b{(input)} admin mode L7_ENABLE or L7_DISABLE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *       
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapAdminModeSet(L7_uint32 mode)
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if DHCP Server component not started by configurator */
  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  /* assumes current admin mode already established, so nothing more to do here */
  if (mode == pDhcpsMapCfgData->dhcps.dhcpsAdminMode)
    return L7_SUCCESS;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  pDhcpsMapCfgData->dhcps.dhcpsAdminMode = mode;
  DHCPS_SET_CONFIG_DATA_DIRTY;
  rc = L7_SUCCESS;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the DHCP Server Number of ping packets to echo 
 *
 * @param    *pNoOfPingPkt   @b{(output)} Number of packets
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNoOfPingPktGet(L7_uint32 *pNoOfPingPkt)
{
  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pNoOfPingPkt = pDhcpsMapCfgData->dhcps.dhcpsPingPktNo;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Set the DHCP Server Number of ping packets to echo 
 *
 * @param    noOfPingPkt   @b{(input)} Number of ping  packets
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNoOfPingPktSet(L7_uint32 noOfPingPkt)
{

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (noOfPingPkt < L7_DHCPS_PINGPKTNO_MIN || 
      noOfPingPkt > L7_DHCPS_PINGPKTNO_MAX  )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  pDhcpsMapCfgData->dhcps.dhcpsPingPktNo = noOfPingPkt;
  DHCPS_SET_CONFIG_DATA_DIRTY;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*:ignore*/
/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  DHCPS INFO GET/SET
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
 * @purpose  Get the number of active leases statistics for a DHCP Server          
 *
 * @param    pNoActiveLeases @b{(output)} number of active leases         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfActiveLeasesGet( L7_uint32  *pNoActiveLeases)
{
  L7_uint32 i             = 0;
  L7_uint32 activeLeases  = 0;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
  {
    /* Looking for all  active leases */
    if (pDhcpsMapLeaseCfgData->lease[i].state == ACTIVE_LEASE )
      activeLeases++;
  }

  *pNoActiveLeases = activeLeases;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of active dynamic leases statistics 
 *
 * @param    pNoActiveDynamicLeases @b{(output)} number of active dynamic leases 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfActiveDynamicLeasesGet( L7_uint32  *pNoActiveDynamicLeases)
{
  L7_uint32 i             = 0;
  L7_uint32 activeDynamicLeases  = 0;
  L7_uint32 poolIndex ;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
  {
    /* Looking for all  active leases from dynamic pools*/
    if ((pDhcpsMapLeaseCfgData->lease[i].state == ACTIVE_LEASE ) &&
	(pDhcpsMapLeaseCfgData->lease[i].IsBootp == L7_FALSE ) &&
	( dhcpsGetPoolIndex(pDhcpsMapLeaseCfgData->lease[i].poolName, &poolIndex) 
	  == L7_SUCCESS))
    {
      if (pDhcpsMapCfgData->pool[poolIndex].poolType == DYNAMIC_POOL)
	activeDynamicLeases++;
    }
  }

  *pNoActiveDynamicLeases = activeDynamicLeases;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of active manual leases statistics 
 *
 * @param    pNoActiveManualLeases @b{(output)} number of active manual leases 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfActiveManualLeasesGet( L7_uint32  *pNoActiveManualLeases)
{
  L7_uint32 i             = 0;
  L7_uint32 activeManualLeases  = 0;
  L7_uint32 poolIndex ;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
  {
    /* Looking for all  active leases from manual pools*/
    if ((pDhcpsMapLeaseCfgData->lease[i].state == ACTIVE_LEASE ) &&
	(pDhcpsMapLeaseCfgData->lease[i].IsBootp == L7_FALSE ) &&
	( dhcpsGetPoolIndex(pDhcpsMapLeaseCfgData->lease[i].poolName, &poolIndex) 
	  == L7_SUCCESS))
    {
      if (pDhcpsMapCfgData->pool[poolIndex].poolType == MANUAL_POOL)
	activeManualLeases++;
    }
  }

  *pNoActiveManualLeases = activeManualLeases;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId);

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of active bootp leases statistics 
 *
 * @param    pNoActiveBootpLeases @b{(output)} number of active bootp leases 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfActiveBootpLeasesGet( L7_uint32  *pNoActiveBootpLeases)
{
  L7_uint32 i             = 0;
  L7_uint32 activeBootpLeases  = 0;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
  {
    /* Looking for all  active bootp leases */
    if ((pDhcpsMapLeaseCfgData->lease[i].state == ACTIVE_LEASE ) &&
	(pDhcpsMapLeaseCfgData->lease[i].IsBootp == L7_TRUE ) )
    {
      activeBootpLeases++;
    }
  }

  *pNoActiveBootpLeases = activeBootpLeases;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of free leases statistics for a DHCP Server          
 *
 * @param    pNoFreeLeases @b{(output)} number of free leases         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfFreeLeasesGet( L7_uint32  *pNoFreeLeases)
{

  L7_uint32  i          = 0;
  L7_uint32  freeLeases = 0;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
  {
    /* Looking for all  active leases */
    if (pDhcpsMapLeaseCfgData->lease[i].state == FREE_LEASE )
      freeLeases++;
  }

  *pNoFreeLeases = freeLeases;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of expired leases statistics for a DHCP Server          
 *
 * @param    pNoExpiredLeases @b{(output)} number of expired leases         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfExpiredLeasesGet( L7_uint32  *pNoExpiredLeases)
{

  L7_uint32 i             =0;
  L7_uint32 expiredLeases =0;


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);


  for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
  {
    /* Looking for all  active leases */
    if (pDhcpsMapLeaseCfgData->lease[i].state == EXPIRED_LEASE )
      expiredLeases++;
  }

  *pNoExpiredLeases = expiredLeases;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of abandoned leases statistics for a DHCP Server          
 *
 * @param    pNoAbandonedLeases @b{(output)} number of abandoned leases         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfAbandonedLeasesGet( L7_uint32  *pNoAbandonedLeases)
{

  L7_uint32 i               = 0;
  L7_uint32 abandonedLeases = 0;


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
  {
    /* Looking for all  active leases */
    if (pDhcpsMapLeaseCfgData->lease[i].state == ABANDONED_LEASE )
      abandonedLeases++;
  }

  *pNoAbandonedLeases = abandonedLeases;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Get the number of offered leases statistics for a DHCP Server          
 *
 * @param    pNoOfferedLeases @b{(output)} number of offered leases         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfpNoOfferedLeasesGet( L7_uint32  *pNoOfferedLeases)
{

  L7_uint32 i             = 0;
  L7_uint32 offeredLeases = 0;


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
  {
    /* Looking for all  active leases */
    if (pDhcpsMapLeaseCfgData->lease[i].state == OFFERED_LEASE )
      offeredLeases++;
  }

  *pNoOfferedLeases = offeredLeases;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of Discover Received statistics 
 *
 * @param    pDiscoverReceived @b{(output)} number of discover received         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsDiscoverReceivedGet( L7_uint32  *pDiscoverReceived)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pDiscoverReceived=pDhcpsInfo->dhcpsStats.numOfDhcpsDiscoverReceived;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Get the number of Request Received statistics 
 *
 * @param    pRequestReceived @b{(output)} number of requests  received        
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsRequestReceivedGet( L7_uint32  *pRequestReceived)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pRequestReceived=pDhcpsInfo->dhcpsStats.numOfDhcpsRequestReceived;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/***************************************************************************
 * @purpose  Get the number of Decline Received statistics 
 *
 * @param    pDeclineReceived @b{(output)} number of decline messages received         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 ****************************************************************************/
L7_RC_t dhcpsDeclineReceivedGet( L7_uint32  *pDeclineReceived)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pDeclineReceived=pDhcpsInfo->dhcpsStats.numOfDhcpsDeclineReceived;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*****************************************************************************
 * @purpose  Get the number of Release Received statistics 
 *
 * @param    pReleaseReceived @b{(output)} number of release messages received         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 ******************************************************************************/
L7_RC_t dhcpsReleaseReceivedGet( L7_uint32  *pReleaseReceived)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pReleaseReceived=pDhcpsInfo->dhcpsStats.numOfDhcpsReleaseReceived;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/****************************************************************************
 * @purpose  Get the number of Inform Received statistics 
 *
 * @param    pInformReceived @b{(output)} number of inform messages received
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 ****************************************************************************/
L7_RC_t dhcpsInformReceivedGet( L7_uint32  *pInformReceived)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pInformReceived=pDhcpsInfo->dhcpsStats.numOfDhcpsInformReceived;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of Offer Sent statistics 
 *
 * @param    pOfferSent @b{(output)} number of offers sent         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsOfferSentGet( L7_uint32  *pOfferSent)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pOfferSent=pDhcpsInfo->dhcpsStats.numOfDhcpsOfferSent;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of Ack Sent statistics 
 *
 * @param    pAckSent @b{(output)} number of Acknowledges sent         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsAckSentGet( L7_uint32  *pAckSent)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pAckSent=pDhcpsInfo->dhcpsStats.numOfDhcpsAckSent;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of Nack Sent statistics 
 *
 * @param    pNackSent @b{(output)} number of Nack Messages sent         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNackSentGet( L7_uint32  *pNackSent)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pNackSent=pDhcpsInfo->dhcpsStats.numOfDhcpsNackSent;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the number of malformed statistics 
 *
 * @param    pMalformed @b{(output)} number of malformed messages         
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMalformedGet( L7_uint32  *pMalformed)
{


  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pMalformed=pDhcpsInfo->dhcpsStats.numOfMalformedMessages;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Clear the  statistics for a DHCP Server          
 *
 * @param    
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Resets packet counters only     
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsStatisticsClear()
{

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  pDhcpsInfo->dhcpsStats.numOfDhcpsDiscoverReceived=0;
  pDhcpsInfo->dhcpsStats.numOfDhcpsRequestReceived=0;
  pDhcpsInfo->dhcpsStats.numOfDhcpsDeclineReceived=0;
  pDhcpsInfo->dhcpsStats.numOfDhcpsReleaseReceived=0;
  pDhcpsInfo->dhcpsStats.numOfDhcpsInformReceived=0;
  pDhcpsInfo->dhcpsStats.numOfBootpOnlyReceived=0;

  pDhcpsInfo->dhcpsStats.numOfDhcpsOfferSent=0;
  pDhcpsInfo->dhcpsStats.numOfDhcpsAckSent=0;
  pDhcpsInfo->dhcpsStats.numOfDhcpsNackSent=0;
  pDhcpsInfo->dhcpsStats.numOfBootpOnlySent=0;

  pDhcpsInfo->dhcpsStats.numOfMalformedMessages=0;
  pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded=0;
  pDhcpsInfo->dhcpsStats.opt82CopyFailed = 0;


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}


/*:ignore*/
/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  POOL CONFIG ENTRY GET/NEXT
 *---------------------------------------------------------------------
 */
/*:end ignore*/


/*********************************************************************
 * @purpose  Set the DHCP Server lease time for a pool          
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    leaseTime    @b{(input)} lease time for a pool in minutes 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapLeaseTimeSet(L7_char8 *pPoolName, L7_uint32 leaseTime)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (leaseTime < L7_DHCPS_LEASETIME_MIN  )
  {
    return L7_FAILURE;
  }

  /* In case of infinite reset lease to max allowed lease time */ 
  if (leaseTime > L7_DHCPS_LEASETIME_MAX  )
    leaseTime = L7_DHCPS_LEASETIME_MAX;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {

    pDhcpsMapCfgData->pool[poolIndex].leaseTime = leaseTime * 60; /* save as seconds */
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }
  else
  {
    /* No such pool found */
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the DHCP Server lease time for a pool          
 *
 * @param    *pPoolName    @b{(input)} pool name
 * @param    *pLeaseTime   @b{(output)} lease time for a pool 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapLeaseTimeGet(L7_char8 *pPoolName, L7_uint32 *pLeaseTime)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    *pLeaseTime = pDhcpsMapCfgData->pool[poolIndex].leaseTime / 60; /* covert to mins */
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Create the DHCP Server pool 
 *
 * @param    *pPoolName   @b{(input)} pool name
 *
 * @returns  L7_SUCCESS          New pool created
 * @returns  L7_TABLE_IS_FULL    No space left for a new pool     
 * @returns  L7_FAILURE          Function failed
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolCreate(L7_char8 *pPoolName)
{
  L7_uint32 poolIndex=-1;
  L7_uint32 i;
  L7_RC_t rc;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    /***      pool name already exists ****/
    rc = L7_FAILURE;
  }
  else
  {
    /**   Initialize assuming worst case and hope to get a free slot  **/
    rc = L7_TABLE_IS_FULL;

    /****   Try to find out free slot for a new pool ***/
    for(i=0; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
    {
      if( *pDhcpsMapCfgData->pool[i].poolName == L7_NULL)
      {
	/*** Set name and default values for new pool ***/
	strncpy(pDhcpsMapCfgData->pool[i].poolName,pPoolName,
	    L7_DHCPS_POOL_NAME_MAXLEN);
	DHCPS_SET_CONFIG_DATA_DIRTY;

	rc = L7_SUCCESS;
	break;
      }
    }

  }  

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Delete the DHCP Server pool 
 *
 * @param    *pPoolName   @b{(input)} pool name
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolDelete(L7_char8 *pPoolName)
{
  L7_uint32 poolIndex=-1;
  dhcpsPoolNode_t *pPool=L7_NULL;
  L7_RC_t rc;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) != L7_SUCCESS)
  {
    /* pool name does not exists */
    rc = L7_FAILURE;
  }
  else
  {
    rc = dhcpsFindPoolByIp(pDhcpsMapCfgData->pool[poolIndex].ipAddr, &pPool );
    if ( rc == L7_SUCCESS)
    {
      /* Now delete pool entries from dynamic data struct */
      rc = dhcpsDestroyPool(&pPool);
    }

    /* reset pool struct and set default values */
    memset(&(pDhcpsMapCfgData->pool[poolIndex]), 0 , sizeof(poolCfgData_t));

    pDhcpsMapCfgData->pool[poolIndex].leaseTime= 
      FD_DHCPS_DEFAULT_POOLCFG_LEASETIME * 60; /* conv to seconds */
    pDhcpsMapCfgData->pool[poolIndex].poolType= 
      FD_DHCPS_DEFAULT_POOLCFG_TYPEOFBINDING;
    pDhcpsMapCfgData->pool[poolIndex].ipAddr = L7_NULL_IP_ADDR;
    pDhcpsMapCfgData->pool[poolIndex].ipMask = L7_NULL_IP_MASK;

    DHCPS_SET_CONFIG_DATA_DIRTY;
    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Map the Lease data structures to existing pools
 *
 *
 * @returns  L7_TRUE       If Success
 * @returns  L7_FALSE      Otherwise
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapInitMappingLeaseConfigData()
{
  return dhcpsInitMappingLeaseConfigData();
}

/*********************************************************************
 * @purpose  Get first DHCP Server pool Entry
 *
 * @param    *pPoolName   @b{(output)} pool name
 * @param    *pPoolIndex  @b{(output)} pool index to fetch next entry
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolEntryFirst(L7_char8 *pPoolName, L7_uint32 *pPoolIndex )
{
  L7_RC_t rc;
  L7_RC_t i;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  *pPoolIndex = -1;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
  {
    if( *pDhcpsMapCfgData->pool[i].poolName != L7_NULL)
    {
      *pPoolIndex = i;
      break;
    }
  }


  if( *pPoolIndex  > L7_DHCPS_MAX_POOL_NUMBER || *pPoolIndex < 0 )
  {
    /* pool name does not exists */
    rc = L7_FAILURE;
  }
  else
  {

    /* copy pool name */
    strncpy(pPoolName, pDhcpsMapCfgData->pool[*pPoolIndex].poolName,
	L7_DHCPS_POOL_NAME_MAXLEN);
    rc = L7_SUCCESS;
  }  

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get next DHCP Server pool Entry
 *
 * @param    *pPoolName   @b{(output)} pool name
 * @param    *pPoolIndex  @b{(input)}  pool index to get next entry
 * @param                 @b{(output)} pool index to fetch next entry
 *
 * @returns  L7_SUCCESS   Next pool found
 * @returns  L7_ERROR     No more pool available
 * @returns  L7_FAILURE   Function failed
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolEntryNext(L7_char8 *pPoolName, L7_uint32 *pPoolIndex )
{
  L7_uint32 i;
  L7_RC_t rc;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  /* Check valid index range */ 
  if(*pPoolIndex < 0 || *pPoolIndex >= L7_DHCPS_MAX_POOL_NUMBER )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /*Initialize rc, in case no more valid pool available  */
  rc = L7_ERROR;

  /* Try to find out next valid  pool from given index */
  for(i= *pPoolIndex +1 ; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
  {
    if( *pDhcpsMapCfgData->pool[i].poolName != L7_NULL)
    {
      /* copy name and index values for found valid pool */
      strncpy(pPoolName,pDhcpsMapCfgData->pool[i].poolName,
	  L7_DHCPS_POOL_NAME_MAXLEN);
      *pPoolIndex = i;
      rc = L7_SUCCESS;
      break;
    }
  }


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Delete the DHCP Server network ip and mask for a pool          
 *
 * @param    *pPoolName   @b{(input)} pool name
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNetworkDelete(L7_char8 *pPoolName )
{
  L7_uint32 poolIndex;
  dhcpsPoolNode_t  * pPool;
  poolCfgData_t    tempPoolCfg;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  if (  rc == L7_SUCCESS)
  {
    if (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL) 
    {
      pDhcpsMapCfgData->pool[poolIndex].ipAddr   = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_ADDR;
      DHCPS_SET_CONFIG_DATA_DIRTY;
    } 
    else if (pDhcpsMapCfgData->pool[poolIndex].poolType == DYNAMIC_POOL) 
    {
      rc = dhcpsFindPoolByIp(pDhcpsMapCfgData->pool[poolIndex].ipAddr, &pPool );
      if ( rc == L7_SUCCESS)
      {
	/* save pool config data (dns etc) */
	memcpy(&tempPoolCfg, &pDhcpsMapCfgData->pool[poolIndex], sizeof(poolCfgData_t));

	/* Now delete pool entries from dynamic data struct */
	dhcpsDestroyPool(&pPool);

	/* restore */
	memcpy(&pDhcpsMapCfgData->pool[poolIndex], &tempPoolCfg, sizeof(poolCfgData_t));
      }

      pDhcpsMapCfgData->pool[poolIndex].ipAddr   = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->pool[poolIndex].poolType = INACTIVE_POOL;
      DHCPS_SET_CONFIG_DATA_DIRTY;
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
    } 
  }
  else
  {
    /* no such pool exists*/
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}
/*********************************************************************
 * @purpose  Set the DHCP Server network ipAddress and mask for a pool          
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipAddr       @b{(input)} network address for a pool 
 * @param    ipMask       @b{(input)} network mask for a pool 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNetworkSet(L7_char8 *pPoolName, L7_uint32 ipAddr, L7_uint32 ipMask)
{
  L7_uint32 poolIndex;
  dhcpsPoolNode_t  * pool;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  if (  rc == L7_SUCCESS &&
      (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL) &&
      (dhcpsValidateSubnet( ipAddr,ipMask) == L7_SUCCESS ))
  {
    pDhcpsMapCfgData->pool[poolIndex].poolType = DYNAMIC_POOL;
    pDhcpsMapCfgData->pool[poolIndex].ipAddr   = ipAddr;
    pDhcpsMapCfgData->pool[poolIndex].ipMask   = ipMask;

    /* Now add this pool in dynamic data struct */
    if(dhcpsCreatePool(&(pDhcpsMapCfgData->pool[poolIndex]),&pool)
	!= L7_SUCCESS) 
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsCreatePool failed.\n");     
      pDhcpsMapCfgData->pool[poolIndex].poolType = INACTIVE_POOL;
      pDhcpsMapCfgData->pool[poolIndex].ipAddr   = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_MASK;

      rc = L7_FAILURE;
    }
    else
    {
      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }

  } /* End of if for dynamic pool validity */
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the DHCP Server network ipAddress and mask for a pool          
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    *pIpAddr      @b{(output)} network address for a pool 
 * @param    *pIpMask      @b{(output)} network mask for a pool 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNetworkGet(L7_char8 *pPoolName, L7_uint32 *pIpAddr, L7_uint32 *pIpMask)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc =  dhcpsGetPoolIndex(pPoolName, &poolIndex) ;
  if( rc  == L7_SUCCESS)
  {
    *pIpAddr = pDhcpsMapCfgData->pool[poolIndex].ipAddr;
    *pIpMask = pDhcpsMapCfgData->pool[poolIndex].ipMask;

  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Set the DHCP DNS Servers for a pool          
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    DnsServers[] @b{(input)} array of DNS server Ip addresses
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapDnsServersSet(L7_char8 *pPoolName, L7_uint32  DnsServers[L7_DHCPS_DNS_SERVER_MAX])
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;
  L7_uint32 counter = 0;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  for (counter = 0; counter < L7_DHCPS_DNS_SERVER_MAX; counter++)
  {
    if(DnsServers[counter] > 0)
    {
      /* Check that the address is not restricted */
      if ((DnsServers[counter] <= 0x01000000UL) ||               /* 0x10000000 ==   1.0.0.0 */
	  (DnsServers[counter] >= 0xE0000000UL) ||                   /* 0xE0000000 == 224.0.0.0 */
	  ((DnsServers[counter] & 0xFF000000UL) == 0x7F000000UL))   /* 0x7F000000 == 127.0.0.0 */
	return L7_FAILURE;
    }
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {

    memcpy(pDhcpsMapCfgData->pool[poolIndex].DNSServerAddrs,DnsServers,
	sizeof(L7_uint32)*L7_DHCPS_DNS_SERVER_MAX) ;

    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the DHCP DNS Servers for a pool          
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    DnsServers[] @b{(output)} array of DNS server Ip addresses
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapDnsServersGet(L7_char8 *pPoolName, L7_uint32  DnsServers[L7_DHCPS_DNS_SERVER_MAX])
{
  L7_uint32 poolIndex;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {

    memcpy(DnsServers,pDhcpsMapCfgData->pool[poolIndex].DNSServerAddrs,
	sizeof(L7_uint32)*L7_DHCPS_DNS_SERVER_MAX) ;

    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Set the DHCP default routers Ip addresses for a pool          
 *
 * @param    *pPoolName       @b{(input)} pool name
 * @param    defaultRouters[] @b{(input)} array of default router Ip addresses
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapDefaultRoutersIpSet(L7_char8 *pPoolName, 
    L7_uint32  defaultRouters[L7_DHCPS_DEFAULT_ROUTER_MAX])
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;
  L7_uint32 counter = 0;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  for (counter = 0; counter < L7_DHCPS_DEFAULT_ROUTER_MAX; counter++)
  {
    if(defaultRouters[counter] > 0)
    {
      /* Check that the address is not restricted */
      if ((defaultRouters[counter] <= 0x01000000UL) ||                  /* 0x10000000 ==   1.0.0.0 */
	  (defaultRouters[counter] >= 0xE0000000UL) ||                  /* 0xE0000000 == 224.0.0.0 */
	  ((defaultRouters[counter] & 0xFF000000UL) == 0x7F000000UL))   /* 0x7F000000 == 127.0.0.0 */
	return L7_FAILURE; 

    }
  } 
  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {

    memcpy(pDhcpsMapCfgData->pool[poolIndex].defaultrouterAddrs,defaultRouters,
	sizeof(L7_uint32)*L7_DHCPS_DEFAULT_ROUTER_MAX) ;

    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the Default routers Ip addresses  for a pool          
 *
 * @param    *pPoolName       @b{(input)} pool name
 * @param    defaultRouters[] @b{(output)} array of default router Ip addresses
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapDefaultRoutersIpGet(L7_char8 *pPoolName, 
    L7_uint32  defaultRouters[L7_DHCPS_DEFAULT_ROUTER_MAX])
{
  L7_uint32 poolIndex;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {

    memcpy(defaultRouters,pDhcpsMapCfgData->pool[poolIndex].defaultrouterAddrs,
	sizeof(L7_uint32)*L7_DHCPS_DEFAULT_ROUTER_MAX) ;

    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get first lease Entry for a pool
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    *pIpAddr     @b{(output)} IP address assigned to lease
 * @param    *pState      @b{(output)} state of the lease          
 * @param    *pLeaseIndex @b{(output)} lease index to fetch next entry
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapLeaseEntryFirst(L7_char8 *pPoolName, L7_uint32 *pLeaseIndex, 
    L7_uint32 *pIpAddr, L7_uint32 *pState )
{
  L7_uint32 poolIndex;
  L7_uint32 i;
  L7_RC_t rc;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) != L7_SUCCESS)
  {
    /* pool name does not exists */
    rc = L7_FAILURE;
  }
  else
  {

    /* Initialize assuming worst case and hope to get a valid lease */ 
    rc = L7_FAILURE;

    /* Try to find out first valid lease for a given pool. */
    for(i=0; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
    {
      /* Looking for all leases except free ones */ 
      if( (pDhcpsMapLeaseCfgData->lease[i].state != FREE_LEASE ) && 
	  (*pDhcpsMapLeaseCfgData->lease[i].poolName != L7_NULL ) && 
	  (strncmp(pDhcpsMapLeaseCfgData->lease[i].poolName,pPoolName,
		   L7_DHCPS_POOL_NAME_MAXLEN) == 0) )
      {
	/* Got one. Copy ipAddr and index of this lease */
	*pIpAddr = pDhcpsMapLeaseCfgData->lease[i].ipAddr; 
	*pState = pDhcpsMapLeaseCfgData->lease[i].state; 
	*pLeaseIndex = i;

	rc = L7_SUCCESS;
	break;
      }
    }

  }  

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Get next lease Entry for a pool
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    *pLeaseIndex @b{(input)} lease index to fetch next entry
 *                        @b{(output)} lease index to fetch further next entry
 * @param    *pIpAddr     @b{(output)} IP address assigned to lease
 * @param    *pState      @b{(output)} state of the lease          
 *
 * @returns  L7_SUCCESS   
 * @returns  L7_FAILURE   function failed
 * @returns  L7_ERROR     No more lease entry available
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapLeaseEntryNext(L7_char8 *pPoolName, L7_uint32 *pLeaseIndex, 
    L7_uint32 *pIpAddr, L7_uint32 *pState )
{
  L7_uint32 poolIndex;
  L7_uint32 i;
  L7_RC_t rc;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }


  /* Check valid index range */
  if(*pLeaseIndex < 0 || *pLeaseIndex >= L7_DHCPS_MAX_LEASE_NUMBER )
  {
    return L7_FAILURE;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) != L7_SUCCESS)
  {
    /* pool name does not exists */
    rc = L7_FAILURE;
  }
  else
  {

    /* Initialize assuming worst case and hope to get a next valid lease */ 
    rc = L7_ERROR;

    /* Try to find out first valid lease for a given pool. */
    for(i= *pLeaseIndex +1; i < L7_DHCPS_MAX_LEASE_NUMBER ; i++)
    {
      /* Looking for all leases except free ones */ 
      if( (pDhcpsMapLeaseCfgData->lease[i].state != FREE_LEASE ) && 
	  (*pDhcpsMapLeaseCfgData->lease[i].poolName != L7_NULL ) && 
	  (strncmp(pDhcpsMapLeaseCfgData->lease[i].poolName,pPoolName,
		   L7_DHCPS_POOL_NAME_MAXLEN) == 0) )
      {
	/* Got one. Copy ipAddr and index of this lease */
	*pIpAddr = pDhcpsMapLeaseCfgData->lease[i].ipAddr; 
	*pState = pDhcpsMapLeaseCfgData->lease[i].state; 
	*pLeaseIndex = i;

	rc = L7_SUCCESS;
	break;
      }
    }

  }  

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Get the DHCP Server pool type
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    *pPoolType   @b{(output)} pool type
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolTypeGet(L7_char8 *pPoolName, L7_uint32 *pPoolType)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc =  dhcpsGetPoolIndex(pPoolName, &poolIndex) ;
  if( rc  == L7_SUCCESS)
  {
    *pPoolType = pDhcpsMapCfgData->pool[poolIndex].poolType;

  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the DHCP Server pool name by index
 *
 * @param    poolIndex    @b{(input)} pool Index
 * @param    *pPoolName   @b{(output)} pool name
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolNameByIndexGet(L7_uint32 poolIndex, L7_char8 *pPoolName )
{
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if( poolIndex  > L7_DHCPS_MAX_POOL_NUMBER || poolIndex < 0 )
  {
    /* pool does not exists */
    rc = L7_FAILURE;

  }
  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( *pDhcpsMapCfgData->pool[poolIndex].poolName  != L7_NULL)
  {
    strncpy(pPoolName, pDhcpsMapCfgData->pool[poolIndex].poolName,
	L7_DHCPS_POOL_NAME_MAXLEN);
    rc = L7_SUCCESS;

  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Clear an active or offered automatic binding  
 *
 * @param    ipAddr     @b{(input)} IP address assigned to lease
 * @param    state      @b{(input)} state of lease 
 *
 * @returns  L7_SUCCESS   
 * @returns  L7_FAILURE   
 *
 * @comments valid state is ACTIVE_LEASE or OFFERED_LEASE     
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapBindingClear( L7_uint32 ipAddr, L7_uint32 state )
{
  L7_RC_t rc;
  dhcpsLeaseNode_t *pLease;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  /* Check valid state */
  if( !(state == ACTIVE_LEASE || state == OFFERED_LEASE ) )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /* Search lease by ipAddress */ 
  if(dhcpsFindLeaseNodeByIpAddr(ipAddr, &pLease ) == L7_SUCCESS )
  {
    /*Check if lease belong to automatic pool and matches required state */
    if( (pLease->parentPool->poolData->poolType == DYNAMIC_POOL ) &&
	(pLease->leaseData->state == state ) ) 
    {
      if(dhcpsDeallocateLease(&pLease) == L7_SUCCESS )
      {
	DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
	rc = L7_SUCCESS;
      }
      else
      {
	DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsMapBindingClear():lease deallocation failed for ip 0x%x .\n",ipAddr);     
	rc = L7_FAILURE;
      }
    }
    else
    {
      rc = L7_FAILURE;
    }
  }  
  else 
  {  
    rc = L7_FAILURE;
  }  /* else of lease search */

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Clear all active or all offered automatic bindings 
 *
 * @param    state      @b{(input)} state of lease 
 *
 * @returns  L7_SUCCESS   
 * @returns  L7_FAILURE   
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapBindingClearAll(L7_uint32 state)
{
  dhcpsLink_t       *pLink;
  dhcpsLeaseNode_t *pTempLease;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  /* Check valid state */
  if( !(state == ACTIVE_LEASE || state == OFFERED_LEASE ) )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  pLink = pDhcpsInfo->dhcpsScheduledLeaseHead.next;

  /* Go through all active or offered leases */ 
  while(L7_NULLPTR != pLink)
  {
    pTempLease = (dhcpsLeaseNode_t*)pLink->object;

    pLink = pLink->next;

    /* state matches ? */
    if(( pTempLease->leaseData->state == state) &&
	(pTempLease->parentPool->poolData->poolType == DYNAMIC_POOL))
    {

      if(dhcpsDeallocateLease(&pTempLease) != L7_SUCCESS )
      {
	DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsMapBindingClearAll():lease deallocation failed for ip 0x%x .\n", pTempLease->leaseData->ipAddr);
      }
    }
  }  

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Get lease data for an given ipAddress
 *
 * @param    ipAddr           @b{(input)} IP address assigned to lease
 * @param    piPMask          @b{(output)} IP Mask
 * @param    hwaddress        @b{(output)} hardware address
 * @param    clientIdentifier @b{(output)} client Identifier
 * @param    pPoolName        @b{(output)} pool name
 * @param    pRemainingTime   @b{(output)} time remaining
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Hardware address is passed out as a formatted string xx:xx:xx...
 *           The max length is therefore (L7_DHCPS_HARDWARE_ADDR_MAXLEN*3)
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapLeaseDataGet( L7_uint32 ipAddr, L7_uint32 *pIpMask,
    L7_uchar8 * hwAddr,
    L7_uchar8 * clientIdentifier,
    L7_uchar8 * poolName,
    L7_uint32 * pRemainingTime)
{
  L7_RC_t rc;
  dhcpsLeaseNode_t *pLease;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /* Search lease by ipAddress */ 
  rc = L7_FAILURE;
  if(dhcpsFindLeaseNodeByIpAddr(ipAddr, &pLease ) == L7_SUCCESS )
  {
    /* is lease Active ? */
    if( pLease->leaseData->state == ACTIVE_LEASE  ) 
    {
      *pIpMask = pLease->leaseData->ipMask;
      if( pLease->leaseEndTime > osapiUpTimeRaw())
	*pRemainingTime = (pLease->leaseEndTime - osapiUpTimeRaw())/60; /* return in mins */
      else
	*pRemainingTime = 0;

      /* If Bootp, set the Lease time to L7_DHCPS_LEASETIME_MAX (DFCT 13382) */
      if( pLease->leaseData->IsBootp == L7_TRUE)
	*pRemainingTime = L7_DHCPS_LEASETIME_MAX + 1;

      sprintf(hwAddr, "%02x:%02x:%02x:%02x:%02x:%02x",
	  pLease->leaseData->hwAddr[0],
	  pLease->leaseData->hwAddr[1],
	  pLease->leaseData->hwAddr[2],
	  pLease->leaseData->hwAddr[3],
	  pLease->leaseData->hwAddr[4],
	  pLease->leaseData->hwAddr[5]);

      memcpy(poolName,pLease->leaseData->poolName,L7_DHCPS_POOL_NAME_MAXLEN);
      memcpy(clientIdentifier,pLease->leaseData->clientIdentifier,
	  L7_DHCPS_CLIENT_ID_MAXLEN);
      rc = L7_SUCCESS;
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get lease hardware-address for an given ipAddress            
 *
 * @param    UnitIndex   @b{(input)} Unit for this operation
 * @param    ipAddr      @b{(input)} IP address assigned to lease
 * @param    hwAddr      @b{(output)) Hardware address of assigned lease
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapLeaseHWAddrGet(L7_uint32 ipAddr, L7_uchar8 *hwAddr)
{
  L7_RC_t rc;
  dhcpsLeaseNode_t *pLease;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /* Search lease by ipAddress */ 
  rc = L7_FAILURE;
  if(dhcpsFindLeaseNodeByIpAddr(ipAddr, &pLease) == L7_SUCCESS)
  {
    /* is lease Active ? */
    if (pLease->leaseData->state == ACTIVE_LEASE) 
    {
      memcpy(hwAddr,pLease->leaseData->hwAddr,L7_DHCPS_HARDWARE_ADDR_MAXLEN);
      rc = L7_SUCCESS;
    }
  }  

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*:ignore*/
/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  EXTRA(FOR SNMP) CONFIG ENTRY GET/NEXT
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
 * @purpose  Check if a DHCP pool corresponding to number is valid or not
 *
 * @param    poolNumber   @b{(input)} pool number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP WALK operation  
 * @notes    
 * @notes    
 *                                                   
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolValidate( L7_uint32 poolNumber)
{
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  /* decr to zero base */
  poolNumber--;

  if( poolNumber  > L7_DHCPS_MAX_POOL_NUMBER || poolNumber < 0 )
  {
    /* pool does not exists */
    rc = L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if(*pDhcpsMapCfgData->pool[poolNumber].poolName != L7_NULL)
    rc= L7_SUCCESS;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Get the next valid DHCP pool number 
 *
 * @param    poolNumber       @b{(input)} pool number 
 * @param    pNextPoolNumber  @b{(output)} next pool number 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP WALK operation  
 * @notes    
 * @notes    
 *                                                   
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapPoolGetNext( L7_uint32 poolNumber, L7_uint32 *pNextPoolNumber)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  /* decr to zero base */
  poolNumber--;

  if( poolNumber  > L7_DHCPS_MAX_POOL_NUMBER || poolNumber < 0 )
  {
    /* pool does not exists */
    rc = L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i = poolNumber + 1 ; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
  {
    if( *pDhcpsMapCfgData->pool[i].poolName != L7_NULL)
    {
      *pNextPoolNumber = i + 1; /* zero base to 1 base */
      rc= L7_SUCCESS;
      break;
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the pool name corresponding to a valid pool number
 *
 * @param    poolNumber       @b{(input)} pool Number 
 * @param    pPoolName        @b{(output)} pool Name 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP GET / SET operation, where it is required
 * @notes  to convert pool number to pool name for calling the USMDB apis 
 * @notes    
 * @notes    
 *                                                   
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNumberToPoolNameGet(L7_uint32 poolNumber, L7_uchar8* pPoolName)
{
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  /* decr (to zero base) */
  poolNumber--;

  if( poolNumber  > L7_DHCPS_MAX_POOL_NUMBER || poolNumber < 0 )
  {
    /* pool does not exists */
    rc = L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( *pDhcpsMapCfgData->pool[poolNumber].poolName != L7_NULL)
  {
    strncpy(pPoolName, pDhcpsMapCfgData->pool[poolNumber].poolName,
	L7_DHCPS_POOL_NAME_MAXLEN);
    rc= L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Set the DHCP Server network ipAddress 
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipAddr       @b{(input)} network address for a pool 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNetworkIpAddrSet(L7_char8 *pPoolName, L7_uint32 ipAddr )
{
  L7_uint32 poolIndex;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  if ( (rc == L7_SUCCESS) &&
      (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL))
  {
    pDhcpsMapCfgData->pool[poolIndex].ipAddr   = ipAddr;
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  } 
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Set the DHCP Server network mask for a pool          
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipMask       @b{(input)} network mask for a pool 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapNetworkMaskSet(L7_char8 *pPoolName, L7_uint32 ipMask)
{
  L7_uint32 poolIndex;
  L7_uint32 ipAddr;
  dhcpsPoolNode_t  * pool;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  /* Check if ipAddr is set or not */
  ipAddr = pDhcpsMapCfgData->pool[poolIndex].ipAddr;
  if( ipAddr == L7_NULL_IP_ADDR) 
    rc= L7_FAILURE;


  if (  rc == L7_SUCCESS &&
      (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL) &&
      (dhcpsValidateSubnet( ipAddr,ipMask) == L7_SUCCESS ))
  {
    pDhcpsMapCfgData->pool[poolIndex].poolType = DYNAMIC_POOL;
    pDhcpsMapCfgData->pool[poolIndex].ipAddr   = ipAddr;
    pDhcpsMapCfgData->pool[poolIndex].ipMask   = ipMask;

    /* Now add this pool in dynamic data struct */
    if(dhcpsCreatePool(&(pDhcpsMapCfgData->pool[poolIndex]),&pool)
	!= L7_SUCCESS) 
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsCreatePool failed.\n");     
      pDhcpsMapCfgData->pool[poolIndex].poolType = INACTIVE_POOL;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_MASK;

      rc = L7_FAILURE;
    }
    else
    {
      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }

  } /* End of if for dynamic pool validity */
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}



/*********************************************************************
 * @purpose  Get the number of pool for a DHCP Server          
 *
 * @param    pNoOfPool @b{(output)} number of pool 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments      
 *      
 * @end
 *********************************************************************/
L7_RC_t dhcpsNoOfPool( L7_uint32  *pNoOfPool)
{

  L7_uint32 i     = 0;
  L7_uint32 pools = 0;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
  {
    /* Looking for all  active leases */
    if (*pDhcpsMapCfgData->pool[i].poolName != L7_NULL)
      pools++;
  }

  *pNoOfPool = pools;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Create the DHCP Server pool from SNMP Manager
 *
 * @param    *pPoolName   @b{(input)} pool name
 *
 * @returns  L7_SUCCESS          New pool created
 * @returns  L7_TABLE_IS_FULL    No space left for a new pool
 * @returns  L7_FAILURE          Function failed
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSNMPMapPoolCreate(L7_char8 *pPoolName)
{
  L7_uint32 poolIndex=-1;
  L7_uint32 i;
  L7_RC_t rc;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    /***      pool name already exists ****/
    rc = L7_FAILURE;
  }
  else
  {
    /**   Initialize assuming worst case and hope to get a free slot  **/
    rc = L7_TABLE_IS_FULL;
    /****   Try to find out free slot for a new pool ***/
    for(i=0; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
    {
      if( *pDhcpsMapCfgData->pool[i].poolName == L7_NULL)
      {
	/*** Set name and default values for new pool ***/
	strncpy(pDhcpsMapCfgData->pool[i].poolName,pPoolName,
	    L7_DHCPS_POOL_NAME_MAXLEN);
	/* Set the state to 'notReady' */
	pDhcpsMapCfgData->pool[i].poolType = NOT_READY_POOL;
	DHCPS_SET_CONFIG_DATA_DIRTY;

	rc = L7_SUCCESS;
	break;
      }
    }

  }


  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Sets the  DHCP Server pool name
 *
 * @param    poolNumber @b{(input)}  poolNumber
 *           poolName @b{(input)}  poolName
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSNMPPoolNameSet( L7_uint32 poolNumber, L7_uchar8* poolName)
{

  L7_uint32 validPoolCount=0;
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }


  if( poolNumber  > L7_DHCPS_MAX_POOL_NUMBER || poolNumber < 0 )
  {
    /* pool does not exists */
    rc = L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0 ; i < L7_DHCPS_MAX_POOL_NUMBER ; i++)
  {
    if( *pDhcpsMapCfgData->pool[i].poolName != L7_NULL)
      validPoolCount++;
    if ( poolNumber == validPoolCount )
    {
      if(pDhcpsMapCfgData->pool[i].poolType == NOT_READY_POOL)
      {
	strncpy(pDhcpsMapCfgData->pool[i].poolName,poolName,
	    L7_DHCPS_POOL_NAME_MAXLEN);
	pDhcpsMapCfgData->pool[i].poolType = INACTIVE_POOL;
	rc= L7_SUCCESS;
	break;
      }
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}
/*********************************************************************
 * @purpose  Set DHCP host for a manual pool
 *
 * @param    poolName    @b{(input)} Name of address pool
 * @param    ipAddr     @b{(input)}  IP Address
 * @param    ipMask     @b{(input)}  IP Mask
 *
 * @returns  L7_SUCCESS  Set host completed successfully
 * @returns  L7_FAILURE  Failed to set host for a pool
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostSet(L7_char8 * pPoolName, L7_uint32  ipAddr, L7_uint32 ipMask)
{
  L7_uint32 poolIndex;
  dhcpsPoolNode_t  * pool;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  if (ipAddr == L7_NULL_IP_ADDR || ipMask == L7_NULL_IP_MASK) 
  {
    rc= L7_FAILURE;
    return rc;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  if (  rc == L7_SUCCESS &&
      (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL) )
  {
    pDhcpsMapCfgData->pool[poolIndex].poolType = MANUAL_POOL;
    pDhcpsMapCfgData->pool[poolIndex].ipAddr   = ipAddr;
    pDhcpsMapCfgData->pool[poolIndex].ipMask   = ipMask;

    /* Now add this pool in dynamic data struct */
    if(dhcpsCreateManualPool(&(pDhcpsMapCfgData->pool[poolIndex]),&pool)
	!= L7_SUCCESS) 
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsCreatePool failed.\n");     
      pDhcpsMapCfgData->pool[poolIndex].poolType = INACTIVE_POOL;
      pDhcpsMapCfgData->pool[poolIndex].ipAddr   = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_MASK;

      rc = L7_FAILURE;
    }
    else
    {
      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }

  } /* End of if for manual pool validity */
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Get DHCP host from a manual  pool
 *
 * @param    poolName    @b{(input)} Name of address pool
 * @param    pIpAddr     @b{(output)} Pointer to IP Address
 * @param    pIpMask     @b{(output)} Pointer to IP Mask
 *
 * @returns  L7_SUCCESS  Dhcp host deleted from a pool
 * @returns  L7_FAILURE  Failed to delete Dhcp host pool
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostGet(L7_char8 * pPoolName, L7_uint32  *pIpAddr, L7_uint32 *pIpMask)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc =  dhcpsGetPoolIndex(pPoolName, &poolIndex) ;
  if( rc  == L7_SUCCESS)
  {
    *pIpAddr = pDhcpsMapCfgData->pool[poolIndex].ipAddr;
    *pIpMask = pDhcpsMapCfgData->pool[poolIndex].ipMask;

  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Delete DHCP host ipaddress and mask from a manual pool
 *
 * @param    poolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  ipAddress and netMask deleted
 * @returns  L7_FAILURE  Failed to deleted ipAddress and netmask
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostDelete(L7_char8 * pPoolName)
{
  L7_uint32 poolIndex;
  dhcpsPoolNode_t  * pPool;
  poolCfgData_t    tempPoolCfg;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  if (  rc == L7_SUCCESS)
  {
    if (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL) 
    {
      pDhcpsMapCfgData->pool[poolIndex].ipAddr   = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_ADDR;
      DHCPS_SET_CONFIG_DATA_DIRTY;
    } 
    else if (pDhcpsMapCfgData->pool[poolIndex].poolType == MANUAL_POOL) 
    {
      rc = dhcpsFindPoolByIp(pDhcpsMapCfgData->pool[poolIndex].ipAddr, &pPool );
      if ( rc == L7_SUCCESS)
      {
	/* save pool config data (dns etc) */
	memcpy(&tempPoolCfg, &pDhcpsMapCfgData->pool[poolIndex], sizeof(poolCfgData_t));

	/* Now delete pool entries from dynamic data struct */
	dhcpsDestroyPool(&pPool);

	/* restore */
	memcpy(&pDhcpsMapCfgData->pool[poolIndex], &tempPoolCfg, sizeof(poolCfgData_t));
      }

      pDhcpsMapCfgData->pool[poolIndex].ipAddr   = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->pool[poolIndex].poolType = INACTIVE_POOL;
      DHCPS_SET_CONFIG_DATA_DIRTY;
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
    } 
  }
  else
  {
    /* no such pool exists*/
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Set the DHCP Server host ipAddress
 *
 * @param    poolName     @b{(input)} pool name
 * @param    ipAddr       @b{(input)} IP Address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostIpAddrSet(L7_char8 *pPoolName, L7_uint32 ipAddr)
{
  L7_uint32 poolIndex;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  if ( (rc == L7_SUCCESS) &&
      (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL))
  {
    pDhcpsMapCfgData->pool[poolIndex].ipAddr   = ipAddr;
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  } 
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Set the DHCP Server host Mask
 *
 * @param    poolName     @b{(input)} pool name
 * @param    ipMask       @b{(input)} IP Mask
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHostMaskSet(L7_char8 *pPoolName, L7_uint32 ipMask)
{
  L7_uint32 poolIndex;
  L7_uint32 ipAddr;
  dhcpsPoolNode_t  * pool;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }


  if (ipMask == L7_NULL_IP_MASK) 
  {
    rc= L7_FAILURE;
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  /* Check if ipAddr is set or not */
  ipAddr = pDhcpsMapCfgData->pool[poolIndex].ipAddr;
  if( ipAddr == L7_NULL_IP_ADDR) 
    rc= L7_FAILURE;


  if (  rc == L7_SUCCESS &&
      (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL)) 
  {
    pDhcpsMapCfgData->pool[poolIndex].poolType = MANUAL_POOL;
    pDhcpsMapCfgData->pool[poolIndex].ipAddr   = ipAddr;
    pDhcpsMapCfgData->pool[poolIndex].ipMask   = ipMask;

    /* Now add this pool in dynamic data struct */
    if(dhcpsCreatePool(&(pDhcpsMapCfgData->pool[poolIndex]),&pool)
	!= L7_SUCCESS) 
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsCreatePool failed.\n");     
      pDhcpsMapCfgData->pool[poolIndex].poolType = INACTIVE_POOL;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_MASK;

      rc = L7_FAILURE;
    }
    else
    {
      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }

  } /* End of if for dynamic pool validity */
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Set DHCP host hardware address for a manual pool
 *
 * @param    poolName    @b{(input)} Name of address pool
 * @param    hwAddr      @b{(input)} Hardware address
 * @param    hwType      @b{(input)} Hardware address type
 *
 * @returns  L7_SUCCESS  Hardware address set
 * @returns  L7_FAILURE  Failed to set Hardware address
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHwAddressSet(L7_char8 * pPoolName, L7_char8  * hwAddr, L7_uint32 hwType)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if( hwType != DHCPS_HW_TYPE_IEEE802 && hwType != DHCPS_HW_TYPE_10B) 
  {
    return L7_FAILURE;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    if (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL)
    {
      memset(pDhcpsMapCfgData->pool[poolIndex].hostAddr,0,
	  L7_DHCPS_HARDWARE_ADDR_MAXLEN);
      pDhcpsMapCfgData->pool[poolIndex].hostAddrtype = hwType;

      /*
       * As of now supported H/W Types are 1 and 6 having HwAddr len 6(L7_MAC_ADDR_LEN).
       * Protocol can support HwAddr len upto 16(L7_DHCPS_HARDWARE_ADDR_MAXLEN).
       */
      if(hwType == DHCPS_HW_TYPE_IEEE802 || hwType == DHCPS_HW_TYPE_10B) 
	pDhcpsMapCfgData->pool[poolIndex].hostAddrLength = L7_MAC_ADDR_LEN;

      memcpy(pDhcpsMapCfgData->pool[poolIndex].hostAddr,hwAddr,L7_DHCPS_HARDWARE_ADDR_MAXLEN);

      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  else
  {
    /* No such pool exists */
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Get DHCP host hardware address for a manual pool
 *
 * @param    poolName    @b{(input)}  Name of address pool
 * @param    pHwAddr     @b{(output)} Pointer to Hardware address
 * @param    pHwType     @b{(output)} Pointer to Hardware address type
 *
 * @returns  L7_SUCCESS  Hardware address set
 * @returns  L7_FAILURE  Failed to set Hardware address
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHwAddressGet(L7_char8 * pPoolName, L7_char8  *pHwAddr, L7_uint32 *pHwType)
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc =  dhcpsGetPoolIndex(pPoolName, &poolIndex) ;
  if( rc  == L7_SUCCESS)
  {
    *pHwType = pDhcpsMapCfgData->pool[poolIndex].hostAddrtype;
    memcpy(pHwAddr,pDhcpsMapCfgData->pool[poolIndex].hostAddr,
	L7_DHCPS_HARDWARE_ADDR_MAXLEN);

  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Delete DHCP host hardware address for a manual pool
 *
 * @param    poolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Hardware address deleted
 * @returns  L7_FAILURE  Failed to delete Hardware address
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsHwAddressDelete( L7_char8 * pPoolName)
{
  L7_uint32 poolIndex = -1;
  L7_RC_t   rc        = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    /* must be inactive */
    if(INACTIVE_POOL == pDhcpsMapCfgData->pool[poolIndex].poolType)
    {
      memset(pDhcpsMapCfgData->pool[poolIndex].hostAddr,0,
	  L7_DHCPS_HARDWARE_ADDR_MAXLEN);
      pDhcpsMapCfgData->pool[poolIndex].hostAddrtype = 0;
      pDhcpsMapCfgData->pool[poolIndex].hostAddrLength = 0;

      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Set DHCP client identifier for a manual pool
 *
 * @param    poolName    @b{(input)} Name of address pool
 * @param    clientId    @b{(input)} Host identifier
 * @param    clientIdLen @b{(input)} Host identifier Length
 *
 * @returns  L7_SUCCESS  Client identifier set
 * @returns  L7_FAILURE  Failed to set client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientIdSet(L7_char8 * pPoolName, L7_char8 *  clientId, L7_uint32 clientIdLen)
{
  L7_uint32 poolIndex=-1;
  L7_uint32 clientIdLenTemp = clientIdLen;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if( (0 == clientIdLenTemp) || (clientIdLenTemp > L7_DHCPS_CLIENT_ID_MAXLEN) )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    if ((pDhcpsMapCfgData->pool[poolIndex].poolType == MANUAL_POOL) || (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL))
    {
      memcpy(pDhcpsMapCfgData->pool[poolIndex].hostIdentifier,clientId,clientIdLen);
      pDhcpsMapCfgData->pool[poolIndex].hostIdLength = (L7_uchar8) clientIdLen;

      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  else
  {
    /* No such pool exists */
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Get DHCP client identifier for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    clientId     @b{(output)} Host identifier
 * @param    clientIdLen  @b{(output)} Host identifier Length
 *
 * @returns  L7_SUCCESS  Client identifier set
 * @returns  L7_FAILURE  Failed to set client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientIdGet(L7_char8 * pPoolName, L7_char8 *  clientId, L7_uint32 * clientIdLen)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc =  dhcpsGetPoolIndex(pPoolName, &poolIndex) ;
  if( rc  == L7_SUCCESS)
  {
    memcpy(clientId,pDhcpsMapCfgData->pool[poolIndex].hostIdentifier,
	L7_DHCPS_CLIENT_ID_MAXLEN);
    *clientIdLen = pDhcpsMapCfgData->pool[poolIndex].hostIdLength;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Delete DHCP client identifier for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Client identifier deleted
 * @returns  L7_FAILURE  Failed to delete client identifier
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientIdDelete(L7_char8 * pPoolName )
{
  L7_uint32 poolIndex = -1;
  L7_RC_t   rc        = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if(dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    /* must be inactive */
    if(INACTIVE_POOL == pDhcpsMapCfgData->pool[poolIndex].poolType)
    {
      memset(pDhcpsMapCfgData->pool[poolIndex].hostIdentifier,0,
	  L7_DHCPS_CLIENT_ID_MAXLEN);
      pDhcpsMapCfgData->pool[poolIndex].hostIdLength = 0;

      DHCPS_SET_CONFIG_DATA_DIRTY;

      rc = L7_SUCCESS;
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Set DHCP client name for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    clientName  @b{(input)} Name of client
 *
 * @returns  L7_SUCCESS  Client name set
 * @returns  L7_FAILURE  Failed to set client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientNameSet(L7_char8 * pPoolName, L7_char8 *  clientName)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {

    memcpy(pDhcpsMapCfgData->pool[poolIndex].hostName,clientName,L7_DHCPS_HOST_NAME_MAXLEN);

    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }
  else
  {
    /* No such pool exists */
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}
/*********************************************************************
 * @purpose  Get DHCP client name for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    clientName  @b{(output)} Name of client
 *
 * @returns  L7_SUCCESS  Client name set
 * @returns  L7_FAILURE  Failed to set client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientNameGet(L7_char8 * pPoolName, L7_char8 *  clientName)
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc =  dhcpsGetPoolIndex(pPoolName, &poolIndex) ;
  if( rc  == L7_SUCCESS)
  {
    memcpy(clientName,pDhcpsMapCfgData->pool[poolIndex].hostName,
	L7_DHCPS_HOST_NAME_MAXLEN);

  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}
/*********************************************************************
 * @purpose  Delete DHCP client name for a manual pool
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  Client name delete
 * @returns  L7_FAILURE  Failed to delete client name
 *
 * @comments  Check return code to determine result.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsClientNameDelete(L7_char8 * pPoolName )
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc =  dhcpsGetPoolIndex(pPoolName, &poolIndex) ;
  if( rc  == L7_SUCCESS)
  {
    memset(pDhcpsMapCfgData->pool[poolIndex].hostName,0,
	L7_DHCPS_HOST_NAME_MAXLEN);
    DHCPS_SET_CONFIG_DATA_DIRTY;

  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Add exluded IP address range
 *
 * @param    fromIpAddr  @b{(input)} from IP address of range
 * @param    toIpAddr    @b{(input)} to IP address of range
 *
 * @returns  L7_SUCCESS  IP address range added for exclusion
 * @returns  L7_FAILURE  Failed to add IP address range
 *
 * @comments  
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsExcludedAddressRangeAdd(L7_uint32 fromIpAddr, L7_uint32 toIpAddr)
{
  L7_uint32 i;
  L7_int32 freeSlot=-1;
  L7_uint32 rc=L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (fromIpAddr == 0 || toIpAddr == 0 || toIpAddr < fromIpAddr)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc= L7_SUCCESS;

  /* Check if there is any overlap with exsting range */
  for(i=0; i < L7_DHCPS_MAX_EXCLUDE_NUMBER ; i++)
  {
    if(pDhcpsMapCfgData->excludedAddrs[i].fromAddr >0)
    {
      if( ((fromIpAddr >= pDhcpsMapCfgData->excludedAddrs[i].fromAddr ) &&
	    (fromIpAddr <= pDhcpsMapCfgData->excludedAddrs[i].toAddr   )   )  ||

	  ((toIpAddr >= pDhcpsMapCfgData->excludedAddrs[i].fromAddr ) &&
	   (toIpAddr <= pDhcpsMapCfgData->excludedAddrs[i].toAddr )     )    )
      {
	/* New range is overlaping with an existing range */
	rc=L7_FAILURE;
	break;
      }
    }
    else
    {
      if( freeSlot == -1 )
	freeSlot=i;
    }
  }


  if( rc == L7_SUCCESS && 
      freeSlot >=0 && freeSlot < L7_DHCPS_MAX_EXCLUDE_NUMBER )
  {
    pDhcpsMapCfgData->excludedAddrs[freeSlot].fromAddr = fromIpAddr;
    pDhcpsMapCfgData->excludedAddrs[freeSlot].toAddr  =  toIpAddr;
    DHCPS_SET_CONFIG_DATA_DIRTY;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Delete exluded IP address range
 *
 * @param    fromIpAddr  @b{(input)} from IP address of range
 * @param    toIpAddr    @b{(input)} to IP address of range
 *
 * @returns  L7_SUCCESS  IP address range deleted for exclusion
 * @returns  L7_FAILURE  Failed to delete IP address range
 *
 * @comments  
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsExcludedAddressRangeDelete(L7_uint32 fromIpAddr, 
    L7_uint32 toIpAddr)
{
  L7_uint32 i=0;
  L7_uint32 rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (toIpAddr == 0 || toIpAddr < fromIpAddr)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=0; i < L7_DHCPS_MAX_EXCLUDE_NUMBER ; i++)
  {
    if( pDhcpsMapCfgData->excludedAddrs[i].fromAddr >0  &&
	fromIpAddr == pDhcpsMapCfgData->excludedAddrs[i].fromAddr  &&
	toIpAddr == pDhcpsMapCfgData->excludedAddrs[i].toAddr     ) 
    { 
      pDhcpsMapCfgData->excludedAddrs[i].fromAddr = L7_NULL_IP_ADDR;
      pDhcpsMapCfgData->excludedAddrs[i].toAddr  =  L7_NULL_IP_ADDR;
      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc= L7_SUCCESS;
    } 
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}


/*********************************************************************
 * @purpose  To get exluded IP address range first entry
 *
 * @param    pFromIpAddr  @b{(output)} from IP address of range
 * @param    pToIpAddr    @b{(output)} to IP address of range
 * @param    pRangeIndex @b{(output)} index to get next entry 
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 * @comments  
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsExcludedAddressRangeEntryFirst(L7_uint32 * pFromIpAddr, 
    L7_uint32  * pToIpAddr, 
    L7_uint32  * pRangeIndex)
{
  L7_uint32 i=0;
  L7_uint32 rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = L7_FAILURE;

  for(i=0; i < L7_DHCPS_MAX_EXCLUDE_NUMBER ; i++)
  {
    if(pDhcpsMapCfgData->excludedAddrs[i].fromAddr >0)
    {
      *pFromIpAddr = pDhcpsMapCfgData->excludedAddrs[i].fromAddr;
      *pToIpAddr = pDhcpsMapCfgData->excludedAddrs[i].toAddr;    
      *pRangeIndex = i;
      rc = L7_SUCCESS;
      break;
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  To get exluded IP address range Next entry
 *
 * @param    pFromIpAddr  @b{(output)} from IP address of range
 * @param    pToIpAddr    @b{(output)} to IP address of range
 * @param    pRangeIndex  @b{(input)} index to get this entry 
 * @param    pRangeIndex  @b{(output)} index to get next entry 
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 * @comments  
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsExcludedAddressRangeEntryNext(L7_uint32 * pFromIpAddr, 
    L7_uint32  * pToIpAddr, 
    L7_uint32  * pRangeIndex)
{
  L7_uint32 i=0;
  L7_uint32 rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (*pRangeIndex < 0 || *pRangeIndex >= L7_DHCPS_MAX_EXCLUDE_NUMBER)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = L7_FAILURE;

  for(i= *pRangeIndex +1  ; i < L7_DHCPS_MAX_EXCLUDE_NUMBER ; i++)
  {
    if(pDhcpsMapCfgData->excludedAddrs[i].fromAddr >0)
    {
      *pFromIpAddr = pDhcpsMapCfgData->excludedAddrs[i].fromAddr;
      *pToIpAddr = pDhcpsMapCfgData->excludedAddrs[i].toAddr;    
      *pRangeIndex = i;
      rc = L7_SUCCESS;
      break;
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}
/*********************************************************************
 * @purpose  Set the DHCP Server host ip mask for a pool for manual binding
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipMask       @b{(input)} ip mask for a pool
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsMapSNMPHostMaskSet(L7_char8 *pPoolName, L7_uint32 ipMask)
{
  L7_uint32 poolIndex;
  L7_uint32 ipAddr;
  dhcpsPoolNode_t  * pool;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  if( ipMask == L7_NULL_IP_MASK) 
    rc= L7_FAILURE;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  /* Check if ipAddr is set or not */
  ipAddr = pDhcpsMapCfgData->pool[poolIndex].ipAddr;
  if( ipAddr == L7_NULL_IP_ADDR) 
    rc= L7_FAILURE;


  if (  rc == L7_SUCCESS &&
      (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL)) 
  {
    pDhcpsMapCfgData->pool[poolIndex].poolType = MANUAL_POOL;
    pDhcpsMapCfgData->pool[poolIndex].ipAddr   = ipAddr;
    pDhcpsMapCfgData->pool[poolIndex].ipMask   = ipMask;

    /* Now add this pool in dynamic data struct */
    if(dhcpsCreatePool(&(pDhcpsMapCfgData->pool[poolIndex]),&pool)
	!= L7_SUCCESS) 
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsCreatePool failed.\n");     
      pDhcpsMapCfgData->pool[poolIndex].poolType = INACTIVE_POOL;
      pDhcpsMapCfgData->pool[poolIndex].ipMask   = L7_NULL_IP_MASK;

      rc = L7_FAILURE;
    }
    else
    {
      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }

  } /* End of if for dynamic pool validity */
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Set the DHCP Server host ipAddress for manual binding
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    ipAddr       @b{(input)} ip address for a pool
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsMapSNMPHostIpAddrSet(L7_char8 *pPoolName, L7_uint32 ipAddr )
{
  L7_uint32 poolIndex;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    rc= L7_FAILURE;
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  rc = dhcpsGetPoolIndex(pPoolName, &poolIndex);

  if ( (rc == L7_SUCCESS) &&
      (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL))
  {
    pDhcpsMapCfgData->pool[poolIndex].ipAddr   = ipAddr;
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  } 
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the next valid exclude address range
 *
 * @param    rangeNumber     @b{(input)} index of the range
 * @param    pStartIpAddress @b{(output)} from Ip  Address of the range
 * @param    pEndIpAddress   @b{(output)} to Ip Address of the range 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP WALK operation
 * @notes
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSNMPExcludeAddressRangeGet(L7_uint32 rangeNumber,
    L7_uint32 *pStartIpAddress,
    L7_uint32 *pEndIpAddress)
{
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if((rangeNumber-1) <  L7_DHCPS_MAX_EXCLUDE_NUMBER)
  {
    if( pDhcpsMapCfgData->excludedAddrs[rangeNumber-1].fromAddr !=  L7_NULL_IP_ADDR)
    {
      *pStartIpAddress = pDhcpsMapCfgData->excludedAddrs[rangeNumber-1].fromAddr;
      *pEndIpAddress = pDhcpsMapCfgData->excludedAddrs[rangeNumber-1].toAddr;    
      rc = L7_SUCCESS;
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Get the next valid exclude address index
 *
 * @param    rangeNumber     @b{(input)} index of the range
 * @param    pIndex          @b{(output)} next index of the range
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  intended for the use of SNMP WALK operation
 * @notes
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t dhcpsSNMPExcludeAddressRangeGetNext(L7_uint32 rangeNumber,
    L7_uint32 *pIndex)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  for(i=rangeNumber ; i < L7_DHCPS_MAX_EXCLUDE_NUMBER ; i++)
  {
    if( pDhcpsMapCfgData->excludedAddrs[i].fromAddr !=  L7_NULL_IP_ADDR)
    {
      *pIndex = i + 1;
      rc = L7_SUCCESS;
      break;
    }
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Set DHCP host hardware type for a manual pool
 *
 * @param    poolName    @b{(input)} Name of address pool
 * @param    hwType      @b{(input)} Hardware address type
 *
 * @returns  L7_SUCCESS  Hardware type set
 * @returns  L7_FAILURE  Failed to set Hardware type
 *
 * @comments  for setting HW type from SNMP manager  
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSNMPHwTypeSet(L7_char8 * pPoolName,  L7_uint32 hwType)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if( hwType != DHCPS_HW_TYPE_IEEE802 && hwType != DHCPS_HW_TYPE_10B) 
  {
    return L7_FAILURE;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    if (pDhcpsMapCfgData->pool[poolIndex].poolType == INACTIVE_POOL)
    {
      pDhcpsMapCfgData->pool[poolIndex].hostAddrtype = hwType;

      DHCPS_SET_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  else
  {
    /* No such pool exists */
    rc = L7_FAILURE;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  Get Next Lease IP address
 *
 * @param    currIpAddr  @b{(input)} Lease IP address
 * @param    pNextAddr   @b{(output)} Next Lease IP address
 *
 * @returns  L7_SUCCESS  Got the next IP
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsLeaseGetNext(L7_uint32 currIpAddr, L7_uint32 * pNextAddr)
{
  L7_uint32 maxIpAddr=0;
  dhcpsPoolNode_t  *tempPool;
  dhcpsLeaseNode_t  *tempLease;
  dhcpsLink_t     *pPoolLink;
  dhcpsLink_t     *pLeaseLink;
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /* iterate server pools list, find lease's ip */
  pPoolLink = (pDhcpsInfo->dhcpsPoolsHead).next;
  while(L7_NULLPTR != pPoolLink && L7_FAILURE == rc)
  {
    tempPool = (dhcpsPoolNode_t *)pPoolLink->object;

    if(MANUAL_POOL == tempPool->poolData->poolType)
      maxIpAddr = tempPool->poolData->ipAddr + 1;
    else
      maxIpAddr = tempPool->poolData->ipAddr + ~(tempPool->poolData->ipMask);

    /* skip pool if curr ip is larger than the max for the pool */
    if(currIpAddr < maxIpAddr)
    {
      /* get first lease link */
      pLeaseLink = (tempPool->poolLeasesHead).next;

      /* iterate lease of the pool */
      while ( L7_NULLPTR != pLeaseLink )
      {
	tempLease = (dhcpsLeaseNode_t *)pLeaseLink->object;
	if(ACTIVE_LEASE == tempLease->leaseData->state )
	{
	  if ( currIpAddr < tempLease->leaseData->ipAddr )
	  {
	    *pNextAddr = tempLease->leaseData->ipAddr;
	    rc= L7_SUCCESS;
	    break;
	  }
	}
	pLeaseLink = pLeaseLink->next;
      }
    }

    /* else try next pool */
    pPoolLink = pPoolLink->next;
  } 

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId);

  return rc;
}


/* DHCP Phase 2 USMDB Functions */
/*********************************************************************
 * @purpose  Create the option row
 *
 * @param    poolName    @b{(input)} Name of address pool
 * @param    code @b{(input)} TLV Field code
 *
 * @returns  L7_SUCCESS  Row for storing the TLV created successfully
 * @returns  L7_FAILURE 
 * @returns  L7_ALREADY_CONFIGURED  Row already exist.
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionCreate(L7_char8* pPoolName, L7_uchar8 code )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 codeTemp = code;
  L7_uint32 poolIndex;

  if ( codeTemp > L7_DHCPS_OPTION_CONFIGURE_MAX )
    return L7_FAILURE;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsValidateOptionCode(code) == L7_SUCCESS &&
      dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    rc = dhcpsTLVOptionCreate(pDhcpsMapCfgData->pool[poolIndex].options,code);
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}


/*********************************************************************
 * @purpose  Set the option field value
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 * @param    pData        @b{(input)} TLV code data
 * @param    length       @b{(input)} TLV code field length
 * @param    dataFormat   @b{(input)} TLV data format
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionSet(L7_char8* pPoolName, L7_uchar8 code, 
    L7_uchar8* pData, L7_uint32 length, L7_uchar8 dataFormat)
{

  L7_RC_t rc = L7_FAILURE;
  L7_uint32 codeTemp = code;
  L7_uint32 poolIndex;
  L7_uint32 optionFlagIndex;
  dhcpsPoolNode_t  *pPool;

  if ( codeTemp > L7_DHCPS_OPTION_CONFIGURE_MAX )
    return L7_FAILURE;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( L7_SUCCESS == dhcpsValidateOption(code, pData, length) &&
      dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    rc = dhcpsSetTLVOption(pDhcpsMapCfgData->pool[poolIndex].options,
	code, L7_DHCPS_ACTIVE, pData, length, dataFormat);

    if(rc == L7_SUCCESS) 
    {
      DHCPS_SET_CONFIG_DATA_DIRTY;
      if (( pDhcpsMapCfgData->pool[poolIndex].poolType == DYNAMIC_POOL ||
	    pDhcpsMapCfgData->pool[poolIndex].poolType == MANUAL_POOL)  &&
	  (dhcpsFindPoolByIp(pDhcpsMapCfgData->pool[poolIndex].ipAddr, 
			     &pPool ) == L7_SUCCESS ) )
      {
	/* Pool Node found. Set bit for the optionFlag  */
	optionFlagIndex = code ;
	pPool->optionFlag[optionFlagIndex] = L7_TRUE;
      }
    } 
    /* 
     * Else, poolNode is not created yet. optionFlag bit will be set during 
     * pool node creation 
     */  

  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Get the first option code
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pCode        @b{(output)} TLV Field code
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionGetFirst( L7_char8 * pPoolName, L7_uchar8* pCode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 poolIndex;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    rc = dhcpsTLVOptionGetFirst(pDhcpsMapCfgData->pool[poolIndex].options,pCode);
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Set the next option code
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    prevCode     @b{(input)} TLV Field code
 * @param    pCode        @b{(output)} TLV Field code
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionGetNext( L7_char8* pPoolName, L7_uchar8 prevCode, L7_uchar8* pCode)
{

  L7_RC_t rc = L7_FAILURE;
  L7_uint32 poolIndex;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    rc = dhcpsTLVOptionGetNext(pDhcpsMapCfgData->pool[poolIndex].options,prevCode, pCode);
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Get the option field value
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 * @param    pData        @b{(output)} TLV code data
 * @param    pLength      @b{(output)} TLV code field length
 * @param    pDataFormat  @b{(output)} TLV data format
 * @param    pStatus      @b{(output)} Row status
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionGet(L7_char8* pPoolName, L7_uchar8 code, 
    L7_uchar8* pData, L7_uint32* pLength, L7_uchar8* pDataFormat, L7_char8* pStatus )
{

  L7_RC_t rc = L7_FAILURE;
  L7_uint32 poolIndex;
  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    rc = dhcpsGetTLVOption(pDhcpsMapCfgData->pool[poolIndex].options,code, pData, pLength, pDataFormat, pStatus);
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}
/*********************************************************************
 * @purpose  delete the option field value
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    code         @b{(input)} TLV Field code
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsOptionDelete(L7_char8* pPoolName, L7_uchar8 code)
{

  L7_RC_t rc = L7_FAILURE;
  L7_uint32 poolIndex;
  L7_uint32 optionFlagIndex;
  dhcpsPoolNode_t  *pPool;

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    rc = dhcpsTLVOptionDelete(pDhcpsMapCfgData->pool[poolIndex].options,code);
    if ( rc ==  L7_SUCCESS   ) 
    {
      DHCPS_SET_CONFIG_DATA_DIRTY;
      if (( pDhcpsMapCfgData->pool[poolIndex].poolType == DYNAMIC_POOL ||
	    pDhcpsMapCfgData->pool[poolIndex].poolType == MANUAL_POOL)  &&
	  (dhcpsFindPoolByIp(pDhcpsMapCfgData->pool[poolIndex].ipAddr, 
			     &pPool ) == L7_SUCCESS ) )
      {
	/* Pool Node found. reset bit for the optionFlag  */
	optionFlagIndex = code ;
	pPool->optionFlag[optionFlagIndex] = L7_FALSE;
      }
    } 
    /* 
     * Else, poolNode is not created yet. Nothing to reset.
     */  

  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Set the ip dhcp conflict mode
 *
 * @param    conflict @b{(input)} Ip dhcp conflict logging value
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictLoggingSet(L7_BOOL  conflict)
{
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  pDhcpsMapCfgData->dhcps.dhcpConflictLogging = conflict;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;

}

/*********************************************************************
 * @purpose  Get the ip dhcp conflict logging mode
 *
 * @param    pConflict @b{(output)} Ip dhcp conflict logging value
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictLoggingGet(L7_BOOL*  pConflict)
{
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pConflict = pDhcpsMapCfgData->dhcps.dhcpConflictLogging;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return L7_SUCCESS;

}

/*********************************************************************
 * @purpose  Clear the dhcp conflict address
 *
 * @param    poolAddress @b{(input)} DHCP conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsClearConflict(L7_uint32 poolAddress)
{
  L7_uint32 maxIpAddr=0;
  dhcpsPoolNode_t  *tempPool;
  dhcpsLeaseNode_t  *tempLease;
  dhcpsLink_t     *pPoolLink;
  dhcpsLink_t     *pLeaseLink;
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /* iterate server pools list, find lease's ip */
  pPoolLink = (pDhcpsInfo->dhcpsPoolsHead).next;
  while(L7_NULLPTR != pPoolLink && L7_FAILURE == rc)
  {
    tempPool = (dhcpsPoolNode_t *)pPoolLink->object;

    if(MANUAL_POOL == tempPool->poolData->poolType)
      maxIpAddr = tempPool->poolData->ipAddr + 1;
    else
      maxIpAddr = tempPool->poolData->ipAddr + ~(tempPool->poolData->ipMask);

    /* skip pool if curr ip is larger than the max for the pool */
    if(poolAddress < maxIpAddr)
    {
      /* get first lease link */
      pLeaseLink = (tempPool->poolLeasesHead).next;

      /* iterate lease of the pool */
      while ( L7_NULLPTR != pLeaseLink )
      {
	tempLease = (dhcpsLeaseNode_t *)pLeaseLink->object;
	if ((ABANDONED_LEASE == tempLease->leaseData->state ) && (poolAddress == tempLease->leaseData->ipAddr))
	{
	  if(dhcpsDeallocateLease(&tempLease) == L7_SUCCESS )
	  {
	    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
	    rc = L7_SUCCESS;
	    break;
	  }
	  else
	  {
	    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsClearConflict():lease deallocation failed for ip 0x%x .\n",tempLease->leaseData->ipAddr);     
	    rc = L7_FAILURE;
	  }
	}
	pLeaseLink = pLeaseLink->next;
      }
    }

    /* else try next pool */
    pPoolLink = pPoolLink->next;
  } 

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId);

  return rc;
}

/*********************************************************************
 * @purpose  Clear all the dhcp conflict address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsClearAllConflict()
{
  dhcpsPoolNode_t   *tempPool;
  dhcpsLeaseNode_t  *tempLease;
  dhcpsLink_t       *pPoolLink;
  dhcpsLink_t       *pLeaseLink;
  L7_RC_t           rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /* iterate server pools list, find lease's ip */
  pPoolLink = (pDhcpsInfo->dhcpsPoolsHead).next;
  while(L7_NULLPTR != pPoolLink)
  {
    tempPool = (dhcpsPoolNode_t *)pPoolLink->object;

    /* get first lease link */
    pLeaseLink = (tempPool->poolLeasesHead).next;

    /* iterate lease of the pool */
    while ( L7_NULLPTR != pLeaseLink )
    {
      tempLease = (dhcpsLeaseNode_t *)pLeaseLink->object;
      pLeaseLink = pLeaseLink->next;

      if(ABANDONED_LEASE == tempLease->leaseData->state )
      {
	if(dhcpsDeallocateLease(&tempLease) == L7_SUCCESS )
	{
	  DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
	  rc = L7_SUCCESS;
	}
	else
	{
	  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,"dhcpsClearAllConflict():lease deallocation failed for ip 0x%x .\n",tempLease->leaseData->ipAddr);     
	  rc = L7_FAILURE;
	}
      }
    }
    /* else try next pool */
    pPoolLink = pPoolLink->next;
  } 

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId);

  return rc;
}


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    pConflictAddress    @b{(output)} Conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictGetFirst( L7_uint32* pConflictAddress)
{
  return (dhcpsConflictGetNext(0, pConflictAddress));
}


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    prevConflictAddress    @b{(input)} Conflict ip address
 * @param    pConflictAddress    @b{(output)} Conflict ip address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictGetNext( L7_uint32 prevConflictAddress, L7_uint32* pConflictAddress)
{

  L7_uint32 maxIpAddr=0;
  dhcpsPoolNode_t  *tempPool;
  dhcpsLeaseNode_t  *tempLease;
  dhcpsLink_t     *pPoolLink;
  dhcpsLink_t     *pLeaseLink;
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /* iterate server pools list, find lease's ip */
  pPoolLink = (pDhcpsInfo->dhcpsPoolsHead).next;
  while(L7_NULLPTR != pPoolLink && L7_FAILURE == rc)
  {
    tempPool = (dhcpsPoolNode_t *)pPoolLink->object;

    if(MANUAL_POOL == tempPool->poolData->poolType)
      maxIpAddr = tempPool->poolData->ipAddr + 1;
    else
      maxIpAddr = tempPool->poolData->ipAddr + ~(tempPool->poolData->ipMask);

    /* skip pool if curr ip is larger than the max for the pool */
    if(prevConflictAddress < maxIpAddr)
    {
      /* get first lease link */
      pLeaseLink = (tempPool->poolLeasesHead).next;

      /* iterate lease of the pool */
      while ( L7_NULLPTR != pLeaseLink )
      {
	tempLease = (dhcpsLeaseNode_t *)pLeaseLink->object;
	if(ABANDONED_LEASE == tempLease->leaseData->state )
	{
	  if ( prevConflictAddress < tempLease->leaseData->ipAddr )
	  {
	    *pConflictAddress = tempLease->leaseData->ipAddr;
	    rc= L7_SUCCESS;
	    break;
	  }
	}
	pLeaseLink = pLeaseLink->next;
      }
    }

    /* else try next pool */
    pPoolLink = pPoolLink->next;
  } 

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId);

  return rc;
}


/*********************************************************************
 * @purpose  Get the first dhcp conflict address
 *
 * @param    conflictAddress  @b{(input)} Conflict ip address
 * @param    pDetectionTime   @b{(input)} Conflict detection time
 * @param    pDetectionMethod @b{(input)} Conflict detection Method
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsConflictGet( L7_uint32 conflictAddress , L7_uint32* pDetectionTime,
    L7_uint32* pDetectionMethod)
{
  L7_uint32 maxIpAddr=0;
  dhcpsPoolNode_t  *tempPool;
  dhcpsLeaseNode_t  *tempLease;
  dhcpsLink_t     *pPoolLink;
  dhcpsLink_t     *pLeaseLink;
  L7_RC_t rc = L7_FAILURE;

  if(pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL )
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  /* iterate server pools list, find lease's ip */
  pPoolLink = (pDhcpsInfo->dhcpsPoolsHead).next;
  while(L7_NULLPTR != pPoolLink && L7_FAILURE == rc)
  {
    tempPool = (dhcpsPoolNode_t *)pPoolLink->object;

    if(MANUAL_POOL == tempPool->poolData->poolType)
      maxIpAddr = tempPool->poolData->ipAddr + 1;
    else
      maxIpAddr = tempPool->poolData->ipAddr + ~(tempPool->poolData->ipMask);

    /* skip pool if curr ip is larger than the max for the pool */
    if(conflictAddress < maxIpAddr)
    {
      /* get first lease link */
      pLeaseLink = (tempPool->poolLeasesHead).next;

      /* iterate lease of the pool */
      while ( L7_NULLPTR != pLeaseLink )
      {
	tempLease = (dhcpsLeaseNode_t *)pLeaseLink->object;
	if ((ABANDONED_LEASE == tempLease->leaseData->state ) && (conflictAddress == tempLease->leaseData->ipAddr))
	{
	  *pDetectionTime = tempLease->leaseStartTime; 
	  *pDetectionMethod = tempLease->leaseData->conflictDetectionMethod; 
	  rc = L7_SUCCESS;
	  break;
	}

	/* get next lease */ 
	pLeaseLink = pLeaseLink->next;
      }
    }

    /* else try next pool */
    pPoolLink = pPoolLink->next;
  } 

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId);

  return rc;
}


/*********************************************************************
 * @purpose  Set the netbios name server addresses
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    addresses  @b{(input)} List of addresses
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNameServerAddressSet( L7_char8* pPoolName , 
    L7_uint32 addresses[L7_DHCPS_NETBIOS_NAME_SERVER_MAX])
{
  L7_uint32 poolIndex;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memcpy( pDhcpsMapCfgData->pool[poolIndex].netBiosNameServer, addresses ,
	sizeof(L7_uint32) * L7_DHCPS_NETBIOS_NAME_SERVER_MAX);
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }
  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}


/*********************************************************************
 * @purpose  Get the netbios name server addresses
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    addresses  @b{(output)} List of addresses
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNameServerAddressGet( L7_char8* pPoolName , 
    L7_uint32 addresses[L7_DHCPS_NETBIOS_NAME_SERVER_MAX])
{

  L7_uint32 poolIndex;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memcpy( addresses, pDhcpsMapCfgData->pool[poolIndex].netBiosNameServer, 
	sizeof(L7_uint32) * L7_DHCPS_NETBIOS_NAME_SERVER_MAX); 
    rc = L7_SUCCESS;
  }
  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Delete the netbios name server addresses
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNameServerAddressDelete( L7_char8* pPoolName)
{
  L7_uint32 poolIndex;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memset( pDhcpsMapCfgData->pool[poolIndex].netBiosNameServer, 0 , 
	sizeof(L7_uint32) * L7_DHCPS_NETBIOS_NAME_SERVER_MAX); 

    /* copy factory default */
    pDhcpsMapCfgData->pool[poolIndex].netBiosNameServer[0] = FD_DHCPS_DEFAULT_NBNAMESERVERIP_ADDR;

    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }
  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;
  return rc;

}

/*********************************************************************
 * @purpose  To set the netbios node type
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    nodeType   @b{(input)} Netbios node type
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNodeTypeSet( L7_char8* pPoolName, L7_char8 nodeType)
{
  L7_uint32 poolIndex;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    pDhcpsMapCfgData->pool[poolIndex].netBiosNodeType = nodeType;
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }
  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  To get the netbios node type
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pNodeType   @b{(output)} Netbios node type
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNodeTypeGet( L7_char8* pPoolName, L7_char8* pNodeType)
{

  L7_uint32 poolIndex;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    *pNodeType = pDhcpsMapCfgData->pool[poolIndex].netBiosNodeType;
    rc = L7_SUCCESS;
  }
  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  To delete the netbios node type
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNetbiosNodeTypeDelete( L7_char8* pPoolName)
{
  L7_uint32 poolIndex;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    pDhcpsMapCfgData->pool[poolIndex].netBiosNodeType = FD_DHCPS_DEFAULT_NBNODETYPE;
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }
  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  To set the next server address
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    nextAddress  @b{(input)} Next-server address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNextServerSet( L7_char8* pPoolName,
    L7_uint32 nextAddress)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {

    pDhcpsMapCfgData->pool[poolIndex].nextServer =  nextAddress;

    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  To get the next server address
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pNextAddress  @b{(input)} Next-server address
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNextServerGet( L7_char8* pPoolName, 
    L7_uint32* pNextAddress)
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {

    *pNextAddress =  pDhcpsMapCfgData->pool[poolIndex].nextServer;

    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  To delete the next server address
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsNextServerDelete( L7_char8* pPoolName)
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    pDhcpsMapCfgData->pool[poolIndex].nextServer =  FD_DHCPS_DEFAULT_NEXTSERVERIP_ADDR;
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc= L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  To set the domain name of a dhcp client
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pDomainName  @b{(input)} Domain name of dhcp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsDomainNameSet(L7_char8* pPoolName, L7_char8* pDomainName)
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memcpy(pDhcpsMapCfgData->pool[poolIndex].domainName,
	pDomainName, L7_DHCPS_DOMAIN_NAME_MAXLEN);
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  To get the domain name of a dhcp client
 *
 * @param    pPoolName    @b{(input)} Name of address pool
 * @param    pDomainName  @b{(output)} Domain name of dhcp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsDomainNameGet(L7_char8* pPoolName, L7_char8* pDomainName)
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memcpy(pDomainName, pDhcpsMapCfgData->pool[poolIndex].domainName,
	L7_DHCPS_DOMAIN_NAME_MAXLEN);
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  To delete the domain name of a dhcp client
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsDomainNameDelete(L7_char8* pPoolName)
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memset(pDhcpsMapCfgData->pool[poolIndex].domainName,
	0, L7_DHCPS_DOMAIN_NAME_MAXLEN);

    /* copy factory default */
    strcpy(pDhcpsMapCfgData->pool[poolIndex].domainName,
	FD_DHCPS_DEFAULT_DOMAIN_NAME);

    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  To set the bootfile name
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pBootFile  @b{(input)} boot file name for bootp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootfileSet(L7_char8* pPoolName, L7_char8* pBootFile)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }


  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memcpy(pDhcpsMapCfgData->pool[poolIndex].clientBootFile,
	pBootFile, L7_DHCPS_BOOT_FILE_NAME_MAXLEN);
    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  To get the bootfile name
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 * @param    pBootFile  @b{(output)} boot file name for bootp client
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootfileGet(L7_char8* pPoolName, L7_char8* pBootFile)
{
  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memcpy( pBootFile, pDhcpsMapCfgData->pool[poolIndex].clientBootFile,
	L7_DHCPS_BOOT_FILE_NAME_MAXLEN);
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  To delete the bootfile name
 *
 * @param    pPoolName  @b{(input)} Name of address pool
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootfileDelete(L7_char8* pPoolName)
{

  L7_uint32 poolIndex=-1;
  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  if( dhcpsGetPoolIndex(pPoolName, &poolIndex) == L7_SUCCESS)
  {
    memset(pDhcpsMapCfgData->pool[poolIndex].clientBootFile, 0 , 
	L7_DHCPS_BOOT_FILE_NAME_MAXLEN);

    /* copy factory default */
    strcpy(pDhcpsMapCfgData->pool[poolIndex].clientBootFile,
	FD_DHCPS_DEFAULT_BOOTFILE_NAME);

    DHCPS_SET_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;
  }

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  To set the bootp client allocation mode
 *
 * @param    mode  @b{(input)} Automatic value to be set
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootpAutomaticSet(L7_BOOL mode)
{

  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  pDhcpsMapCfgData->dhcps.dhcpBootpAuto = mode;
  DHCPS_SET_CONFIG_DATA_DIRTY;
  rc = L7_SUCCESS;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;

}

/*********************************************************************
 * @purpose  To get the bootp client allocation mode
 *
 * @param    pMode  @b{(output)} Automatic value to be set
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dhcpsBootpAutomaticGet(L7_BOOL* pMode)
{

  L7_RC_t rc = L7_FAILURE;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return rc;
  }

  osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

  *pMode = pDhcpsMapCfgData->dhcps.dhcpBootpAuto;
  rc = L7_SUCCESS;

  osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  return rc;
}

/*********************************************************************
 * @purpose  Inject a DHCP IP Packet into the DHCP Server
 *
 * @param    ipData    @b{(output)} Pointer to an DHCP IP packet
 * @param    ipLen     @b{(output)} Length of ipData
 * @param    intIfNum  @b{(output)} Interface on which ipData was received
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE 
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsPacketInject(L7_uchar8 *ipData, L7_int32 ipLen, L7_uint32 intIfNum)
{
  L7_uint32  serverIpAddr;
  L7_uint32  serverIpMask;
  L7_ipHeader_t *ipHeader = (L7_ipHeader_t *)ipData;

  if (pDhcpsMapLeaseCfgData == L7_NULL || pDhcpsMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (dhcpsGetServerID(intIfNum, &serverIpAddr, &serverIpMask) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (dhcpsIsValidDest(intIfNum, serverIpAddr, ipHeader) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  return processDhcpsFrames(ipHeader, ipLen, intIfNum);
}

