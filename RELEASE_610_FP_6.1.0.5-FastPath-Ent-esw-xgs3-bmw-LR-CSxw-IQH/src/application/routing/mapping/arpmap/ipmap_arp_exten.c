/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ipmap_arp_exten.c
* @purpose     IP Mapping layer ARP vendor extensions
* @component   IP Mapping Layer -- ARP
* @comments    Provides additional functions for vendor ARP code
*
*
* @comments    All functions in this file are internal to the ARP
*              component and must not be called without first taking
*              the ARP lock (via the API).
*              
*              None of the functions in this file take the lock.
*
* @create      03/29/2001
* @author      gpaussa
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#include <stdio.h>

#include "l7_common.h"
#include "ipmap_arp.h"
#include "arp.h"
#include "hl.ext"
#include "timer.ext"

/* Global data */
extern L7_BOOL        ipMapRoutingStarted;
extern ipMapArpCtx_t    ipMapArpCtx_g;
extern ipMapArpIntf_t   *ipMapArpIntfInUse_g[];
extern char *ipMapArpEntryTypeStr[];

/* internal function protoypes */
static L7_RC_t ipMapArpExtenEntryCopy(ipMapArpIpNode_t *pIp, L7_arpEntry_t *pArp);

/* Make sure both systems' maximums are compatible */
#if (MAX_INTERFACES < IPM_ARP_INTF_MAX)
#error The MAX_INTERFACES value defined in arp.h is too low!
#endif

/*********************************************************************
* @purpose  Returns contents of the ARP entry for the specified IP address
*
* @param    ipAddr      IP address of the ARP entry
* @param    intIfNum    Internal Interface number for the entry
* @param    *pArp       pointer to output location to store ARP entry info
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    could not find requested ARP entry
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenEntryGet(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum,
    L7_arpEntry_t *pArp)
{
  static const char *routine_name = "ipMapArpExtenEntryGet";
  ipMapArpIpNode_t  *pIp;
  ipMapArpIntf_t *pIntf;
  L7_uint32 localAddr, localMask;
  L7_RC_t rc = L7_FAILURE;

  /* make sure routing subsystem is initialized before trying to access anything */
  if (ipMapRoutingStarted != L7_TRUE)
    return L7_ERROR;

  /* if interface not specified, infer from address */
  if (intIfNum == L7_INVALID_INTF)
  {
    if (ipMapArpSubnetFind(ipAddr, &pIntf, &localAddr, &localMask) != L7_SUCCESS)
      return L7_FAILURE;
    intIfNum = pIntf->intIfNum;
  }

  /* search for the entry node in the ARP IP lookup table */
  if (ipMapArpIpTableSearch(ipAddr, intIfNum, (L7_uint32)L7_MATCH_EXACT, &pIp) != L7_SUCCESS)
    return L7_ERROR;

  if (pIp->key != ipAddr)               /* these should match */
  {
    L7_uchar8   ipStr1[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8   ipStr2[OSAPI_INET_NTOA_BUF_SIZE];

    osapiInetNtoa((L7_uint32)ipAddr, ipStr1);
    osapiInetNtoa(pIp->key, ipStr2);
    LOG_MSG("%s: found entry, but lookup of %s got %s instead\n", 
            routine_name, (char *)ipStr1, (char *)ipStr2);
    L7_assert(pIp->key != ipAddr);
    return L7_FAILURE;
  }

  rc = ipMapArpExtenEntryCopy(pIp, pArp);

  return rc;
}


/*********************************************************************
* @purpose  Returns contents of the ARP entry for the next sequential
*           IP address following the one specified
*
* @param    ipAddr      IP address of the ARP entry to start the search
* @param    intIfNum    Internal Interface number for the entry
* @param    *pArp       pointer to output location to store ARP entry info
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no more ARP entries exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenEntryNext(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum,
                               L7_arpEntry_t *pArp)
{
  ipMapArpIpNode_t  *pIp;
  L7_IP_ADDR_t key;

  /* make sure routing subsystem is initialized before trying to access anything */
  if (ipMapRoutingStarted != L7_TRUE)
    return L7_ERROR;

  /* determine the next sequential IP address in the ARP table */
  if (ipMapArpIpTableSearch(ipAddr, intIfNum, (L7_uint32)L7_MATCH_GETNEXT, &pIp) != L7_SUCCESS)
    return L7_ERROR;

  /* copy the key and release the sema */
  key = (L7_IP_ADDR_t)pIp->key;

  return ipMapArpExtenEntryGet(key, pIp->intIfNum, pArp);
}


/*********************************************************************
* @purpose  Copies contents of an ARP table entry to an output location
*
* @param    *pIp        pointer to ARP IP node
* @param    *pArp       pointer to output location to store local ARP entry info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Though this function sits in ArpExten it needs to take the
*           interface lock as it is called directly from an API function
*           (that also strangely sits in the ArpExten).
*       
* @end
*********************************************************************/
static L7_RC_t ipMapArpExtenEntryCopy(ipMapArpIpNode_t *pIp, L7_arpEntry_t *pArp)
{
  ipMapArpIntf_t  *pIntf;
  L7_ulong32      tmp;
  pIntf = ipMapArpIntfInUse_g[pIp->arpEntry.intfNum];
  if (pIntf == L7_NULL)
    pIntf = ipMapArpIntfInUse_g[0];

  memcpy(&tmp,pIp->arpEntry.ipAddr,sizeof(tmp));
  pArp->ipAddr = (L7_IP_ADDR_t)osapiNtohl(tmp);
  pArp->macAddr.type = L7_LL_ETHERNET;
  pArp->macAddr.len = L7_MAC_ADDR_LEN;
  memcpy(pArp->macAddr.addr.enetAddr.addr, pIp->arpEntry.macAddr, L7_MAC_ADDR_LEN);
  pArp->vlanId = 0;
  pArp->intIfNum = pIntf->intIfNum;
  pArp->hits = 0;                       /* no longer meaningful */
  ipMapArpEntryFlagsBuild(pIp, &pArp->flags);
  /* do not calculate an age value for local or static entries */
  if ((pArp->flags & (L7_ARP_LOCAL | L7_ARP_STATIC)) != 0)
    pArp->ageSecs = 0;                  /* these entries are never aged */
  else
    ipMapArpExtenEntryAgeCalc(pIp->arpEntry.timeStamp, (ulng *)&pArp->ageSecs);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the ARP entry ageout time to a new value
*
* @param    secs        new ARP entry ageout time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenAgeTimeSet(L7_uint32 secs)
{
  t_ARP         *p_A = (t_ARP *)ipMapArpCtx_g.arpHandle;
  e_Err         e;

  /* adjust the timeout value of each ARP entry based on the new arpAgeTime */
  e = ARP_AdjustTimeout(p_A, (ulng)secs);

  if (e == E_OK)
    p_A->arpAgeTime = (word)secs;

  return (e == E_OK) ? L7_SUCCESS : L7_FAILURE;
}


/*********************************************************************
* @purpose  Sets the ARP resolution response timeout value
*
* @param    secs        new ARP response timeout (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenRespTimeSet(L7_uint32 secs)
{
  t_ARP         *p_A = (t_ARP *)ipMapArpCtx_g.arpHandle;

  p_A->arpRespTime = (word)secs;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the number of retries for failed ARP resolution requests
*
* @param    numRetry    number of retry requests
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenRetriesSet(L7_uint32 numRetry)
{
  t_ARP         *p_A = (t_ARP *)ipMapArpCtx_g.arpHandle;

  p_A->retrNmb = (word)numRetry;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the ARP cache size
*
* @param    maxEnt      maximum number of entries allowed in ARP cache
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If request ARP cache size is outside of range then return 
*           an error.
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenCacheSizeSet(L7_uint32 maxEnt)
{
  t_ARP         *p_A = (t_ARP *)ipMapArpCtx_g.arpHandle;
  e_Err         e;

  if ((maxEnt < (L7_uint32)L7_IP_ARP_CACHE_SIZE_MIN) || 
      (maxEnt > (L7_uint32)L7_IP_ARP_CACHE_SIZE_MAX))
    return L7_FAILURE;

  /* remove oldest entries in excess of new cache size and adjust size
   *
   * NOTE: The ARP cache size min and max values have been defined such that
   *       changes in the cache size will not force out any local or static
   *       ARP entries currently residing in the cache and still leave room
   *       for some dynamic entries.
   */
  e = ARP_TrimCache(p_A, (word)maxEnt);

  if (e == E_OK)
    p_A->arpCacheSize = (word)maxEnt;

  return (e == E_OK) ? L7_SUCCESS : L7_FAILURE;
}


/*********************************************************************
* @purpose  Sets the dynamic ARP entry renew mode
*
* @param    mode        dynamic renew mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenDynamicRenewSet(L7_uint32 mode)
{
  t_ARP         *p_A = (t_ARP *)ipMapArpCtx_g.arpHandle;

  p_A->dynamicRenew = (word)((mode == L7_ENABLE) ? TRUE : FALSE);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param    *pStats     pointer to output location for storing ARP cache stats
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenCacheStatsGet(L7_arpCacheStats_t *pStats)
{
  return ipMapArpIpTableStatsGet(pStats);
}


/*********************************************************************
* @purpose  Refresh the age of an existing entry in the ARP cache
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  ARP entry info ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenEntryAgeRefresh(t_Handle arpCacheId, t_ADR *pArpInfo)
{
  L7_RC_t           rc = L7_FAILURE;
  L7_uint32         ipAddr;
  ipMapArpIntf_t    *pIntf;
  L7_ulong32        tmp;
  L7_uchar8         *tmpip;

  if (pArpInfo == L7_NULLPTR)
    return E_FAILED;
  tmpip = pArpInfo->ipAddr;
  tmp = (tmpip[0]<<24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3] ) ;
  ipAddr = (L7_uint32)osapiNtohl(tmp);

  if ((pIntf = ipMapArpIntfInUse_g[(L7_uint32)pArpInfo->intfNum]) == L7_NULL)
  {
    return E_FAILED;
  }

  rc = ipMapArpIpAgeListRefresh(ipAddr, pIntf->intIfNum);

  return (rc == L7_SUCCESS) ? E_OK : E_FAILED;
}

/*********************************************************************
* @purpose  Calculate the current age time for a given ARP entry timestamp
*
* @param    timeStamp     {@b(Input)}   ARP entry timestamp (in ticks)
* @param    *pAgeSecs     {@b(Output)}  age time (in seconds) output ptr
*
* @returns  void
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
void ipMapArpExtenEntryAgeCalc(ulng timeStamp, ulng *pAgeSecs)
{
  ulng          sysTime, age;

  sysTime = TIMER_SysTime();

  /* handle simple timer wrap case */
  if (sysTime >= timeStamp)
    age = (sysTime - timeStamp);
  else
    age = timeStamp - sysTime;
  age /= (ulng)TM_TICKS_PER_SECOND;

  *pAgeSecs = age;
}

/*********************************************************************
* @purpose  Handle conversion of an existing entry to static type
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  ARP entry info ptr
*
* @returns  E_OK
* @returns  E_BADPARM   already a permanent entry
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    Performs whatever processing is necessary during the conversion
*           of a dynamic/gateway entry to a static.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenEntryStaticConvert(t_Handle arpCacheId, t_ADR *pArpInfo)
{
  L7_RC_t           rc = L7_FAILURE;
  ipMapArpIpNode_t  *pIp;
  L7_uint32         ipAddr;
  ipMapArpIntf_t    *pIntf;
  L7_ulong32        tmp;
  L7_uchar8         *tmpip;

  if (pArpInfo == L7_NULLPTR)
    return E_FAILED;

  tmpip = pArpInfo->ipAddr;
  tmp = (tmpip[0]<< 24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3]) ;
  ipAddr = (L7_uint32)osapiNtohl(tmp);

  if ((pIntf = ipMapArpIntfInUse_g[(L7_uint32)pArpInfo->intfNum]) == L7_NULL)
  {
    return E_FAILED;
  }

  if (ipMapArpIpTableSearch(ipAddr, pIntf->intIfNum, (L7_uint32)L7_MATCH_EXACT, &pIp) != L7_SUCCESS)
    return E_FAILED;

  /* increment the static ARP current count */
  rc = ipMapArpIpTableEntryStaticConvert(pIp);

  if (rc != L7_SUCCESS)
    return (rc == L7_ERROR) ? E_BADPARM : E_FAILED;

  return E_OK;
}

/*********************************************************************
* @purpose  Insert an entry into the ARP cache
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  pointer to ARP information
* @param    **ppArpEntry  {@b(Output)} pointer to ARP entry ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    If the ARP cache is full, an attempt is made to replace the 
*           oldest (least-recently updated) dynamic ARP entry.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheInsert(t_Handle arpCacheId, t_ADR *pArpInfo, 
                               t_ADR **ppArpEntry)
{
  ipMapArpIpNode_t  ip, *pIp, *pNode;
  ipMapArpIntf_t    *pIntf;
  L7_ulong32        tmp;
  L7_uchar8         *tmpip;

  if ((pArpInfo == L7_NULLPTR) || (ppArpEntry == L7_NULLPTR))
    return E_FAILED;

  *ppArpEntry = L7_NULLPTR;

  pIp = &ip;
  memset(pIp, 0, sizeof(*pIp));
  tmpip = pArpInfo->ipAddr;
  tmp = (tmpip[0]<<24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3] ) ;
  pIp->key = (L7_uint32)osapiNtohl(tmp);
  
  if ((pIntf = ipMapArpIntfInUse_g[(L7_uint32)pArpInfo->intfNum]) == L7_NULL)
  {
    return E_FAILED;
  }
  pIp->intIfNum = pIntf->intIfNum;

  memcpy(&pIp->arpEntry, pArpInfo, sizeof(pIp->arpEntry)); 

  /* storing the internal interface number and initial lasthittime
   * values in the ARP entry structure */
  pIp->arpEntry.interfaceNumber = pIntf->intIfNum;
  pIp->arpEntry.lastHitTime     = osapiUpTimeRaw();

  if (ipMapArpIpTableInsert(pIp, &pNode) != L7_SUCCESS)
  {
	  if (pNode != L7_NULLPTR)
	  {
		  *ppArpEntry = &pNode->arpEntry;
	  }
	  return E_FAILED;
  }

  *ppArpEntry = &pNode->arpEntry;
  return E_OK;
}

/*********************************************************************
* @purpose  Update the contents of an existing ARP cache entry
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  pointer to ARP information
* @param    macHasChanged {@b(Input)}  indicates ARP entry MAC addr changed
*
* @returns  E_OK
* @returns  E_BADPARM   entry does not exist in ARP cache
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheUpdate(t_Handle arpCacheId, t_ADR *pArpInfo, 
                               Bool macHasChanged) 
{
  ipMapArpIpNode_t  ip, *pIp;
  e_Err             erc;
  L7_RC_t           rc;
  ipMapArpIntf_t    *pIntf;
  L7_ulong32        tmp;
  L7_uchar8         *tmpip=0;

  if (pArpInfo == L7_NULLPTR)
    return E_FAILED;

  /* refresh the entry age time (effectively a no-op for permanent entries) */
  erc = ipMapArpExtenEntryAgeRefresh(arpCacheId, pArpInfo);
  if (erc != E_OK)
    return erc;

  /* update ARP table only if the MAC addr has actually changed */
  if (macHasChanged == TRUE)
  {
    pIp = &ip;
    memset(pIp, 0, sizeof(*pIp));
    tmpip = pArpInfo->ipAddr;
    tmp = (tmpip[0] << 24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3] ) ;
    pIp->key = (L7_uint32)osapiNtohl(tmp);

    if ((pIntf = ipMapArpIntfInUse_g[(L7_uint32)pArpInfo->intfNum])==L7_NULLPTR)
    {
      return E_FAILED;
    }
    pIp->intIfNum = pIntf->intIfNum;

    memcpy(&pIp->arpEntry, pArpInfo, sizeof(pIp->arpEntry)); 

    rc = ipMapArpIpTableUpdate(pIp, IPM_ARP_TABLE_UPDATE);
    if (rc != L7_SUCCESS)
      return (rc == L7_ERROR) ? E_BADPARM : E_FAILED;
  }

  return E_OK;
}

/*********************************************************************
* @purpose  Remove an entry from the ARP cache
*
* @param    arpCacheId    {@b(Input)}   ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}   pointer to ARP information
*
* @returns  E_OK
* @returns  E_BADPARM   entry does not exist in ARP cache
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    The original ARP entry node is no longer valid upon successful
*           return from this call.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheRemove(t_Handle arpCacheId, t_ADR *pArpInfo)
{
  ipMapArpIpNode_t  ip, *pIp;
  L7_RC_t           rc;
  ipMapArpIntf_t    *pIntf;
  L7_ulong32        tmp;
  L7_uchar8         *tmpip;

  if (pArpInfo == L7_NULLPTR)
    return E_FAILED;

  pIp = &ip;
  memset(pIp, 0, sizeof(*pIp));
  tmpip = pArpInfo->ipAddr;
  tmp = (tmpip[0] << 24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3] ) ;
  pIp->key = (L7_uint32)osapiNtohl(tmp);

  if ((pIntf = ipMapArpIntfInUse_g[(L7_uint32)pArpInfo->intfNum]) == L7_NULL)
  {
    return E_FAILED;
  }
  pIp->intIfNum = pIntf->intIfNum;
  memcpy(&pIp->arpEntry, pArpInfo, sizeof(pIp->arpEntry)); 

  rc = ipMapArpIpTableRemove(pIp);
  if (rc != L7_SUCCESS)
    return (rc == L7_ERROR) ? E_BADPARM : E_FAILED;

  return E_OK;
}

/*********************************************************************
* @purpose  Remove the older entries from the ARP cache
*
* @param    none
*
* @returns  E_OK
*
* @notes    This function is called whenever there is a hardware failure
*           in adding a new ARP entry (or) whenever the ARP cache size
*           has hit the high threshold.
*           This creates space and the probability for the new entries
*           to be added into the cache.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenPurgeOldEntries(void)
{
  return ARP_PurgeOldEntries(&(ipMapArpCtx_g.arpHandle));
}

/*********************************************************************
* @purpose  Remove the oldest dynamic entry from the ARP cache
*
* @param    arpCacheId    {@b(Input)}   ARP cache instance identifier
* @param    allowGw       {@b(Input)}   indicates if gateway entry can be removed
*
* @returns  E_OK
* @returns  E_BADPARM   no removable entry exists in ARP cache
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    The original ARP entry node is no longer valid upon successful
*           return from this call, so its arpEntry contents are copied to
*           the location specified by the caller.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheOldestRemove(t_Handle arpCacheId, Bool allowGw)
{
  L7_RC_t           rc;

  rc = ipMapArpIpTableOldestRemove((allowGw == TRUE) ? L7_TRUE : L7_FALSE);
  if (rc != L7_SUCCESS)
    return (rc == L7_ERROR) ? E_BADPARM : E_FAILED;

  return E_OK;
}

/*********************************************************************
* @purpose  Get the first entry in the ARP cache
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    **ppArpEntry  {@b(Output)} pointer to ARP entry info ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheGetFirst(t_Handle arpCacheId, t_ADR **ppArpEntry)
{
  ipMapArpIpNode_t  *pIp;
  L7_uint32         ipAddr;

  if (ppArpEntry == L7_NULLPTR)
    return E_FAILED;

  ipAddr = 0;                           /* use to find first entry in table */

  if (ipMapArpIpTableSearch(ipAddr, L7_INVALID_INTF, (L7_uint32)L7_MATCH_GETNEXT, &pIp) != L7_SUCCESS)
  {
    *ppArpEntry = L7_NULLPTR;
    return E_FAILED;
  }

  *ppArpEntry = &pIp->arpEntry;

  return E_OK;
}

/*********************************************************************
* @purpose  Get the next sequential entry in the ARP cache
*
* @param    arpCacheId        {@b(Input)}  ARP cache instance identifier
* @param    *pArpEntry        {@b(Input)}  current ARP entry info ptr
* @param    **ppArpEntryNext  {@b(Output)} pointer to next ARP entry info ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheGetNext(t_Handle arpCacheId, t_ADR *pArpEntry, 
                                t_ADR **ppArpEntryNext)
{
  ipMapArpIpNode_t  *pIp;
  L7_uint32         ipAddr;
  ipMapArpIntf_t    *pIntf;
  L7_ulong32        tmp;
  L7_uchar8         *tmpip;

  if ((pArpEntry == L7_NULLPTR) || (ppArpEntryNext == L7_NULLPTR))
    return E_FAILED;

  tmpip = pArpEntry->ipAddr;
  tmp = (tmpip[0] << 24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3] ) ;
  ipAddr = (L7_uint32)osapiNtohl(tmp);

  if ((pIntf = ipMapArpIntfInUse_g[(L7_uint32)pArpEntry->intfNum]) == L7_NULL)
  {
    return E_FAILED;
  }

  if (ipMapArpIpTableSearch(ipAddr, pIntf->intIfNum, (L7_uint32)L7_MATCH_GETNEXT, &pIp) != L7_SUCCESS)
  {
    *ppArpEntryNext = L7_NULLPTR;
    return E_FAILED;
  }

  *ppArpEntryNext = &pIp->arpEntry;

  return E_OK;
}

/*********************************************************************
* @purpose  Find the entry in the ARP cache for the specified IP addr
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    pIpAddr       {@b(Input)}  pointer to IP address (octet array)    
* @param    index     {@b(Input)}   ARP interface index number
* @param    **ppArpEntry  {@b(Output)} pointer to ARP entry info ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheFind(t_Handle arpCacheId, byte *pIpAddr, 
    word index, t_ADR **ppArpEntry)
{
  ipMapArpIpNode_t  *pIp;
  L7_uint32         ipAddr;
  ipMapArpIntf_t    *pIntf;

  if ((pIpAddr == L7_NULLPTR) || (ppArpEntry == L7_NULLPTR))
    return E_FAILED;

  *ppArpEntry = L7_NULLPTR;

  memcpy(&ipAddr, pIpAddr, sizeof(ipAddr));
  ipAddr = osapiNtohl(ipAddr);

  if ((pIntf = ipMapArpIntfInUse_g[(L7_uint32)index]) == L7_NULLPTR)
  {
    return E_FAILED;
  }

  if (ipMapArpIpTableSearch(ipAddr, pIntf->intIfNum, (L7_uint32)L7_MATCH_EXACT, &pIp) != L7_SUCCESS)
    return E_FAILED;

  *ppArpEntry = &pIp->arpEntry;

  return E_OK;
}

/*********************************************************************
* @purpose  Find the oldest dynamic entry currently in the ARP cache
*
* @param    arpCacheId    {@b(Input)}   ARP cache instance identifier
* @param    allowGw       {@b(Input)}   indicates if gateway entry can be removed
* @param    **ppArpEntry  {@b(Output)}  pointer to ARP entry info ptr         
*
* @returns  E_OK
* @returns  E_BADPARM   no removable entry exists in ARP cache
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    There is no guarantee that the entry output by this function  
*           will remain the oldest entry at the time it is removed from the
*           cache, unless external measures are taken to protect the
*           current thread from being disrupted.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheOldestFind(t_Handle arpCacheId, Bool allowGw, 
                                   t_ADR **ppArpEntry)
{
  ipMapArpIpNode_t  *pIp;
  L7_RC_t           rc;

  if (ppArpEntry == L7_NULLPTR)
    return E_FAILED;

  *ppArpEntry = L7_NULLPTR;

  rc = ipMapArpIpTableOldestFind((allowGw == TRUE) ? L7_TRUE : L7_FALSE, 
                                 &pIp);
  if (rc != L7_SUCCESS)
    return (rc == L7_ERROR) ? E_BADPARM : E_FAILED;

  *ppArpEntry = &pIp->arpEntry;
  return E_OK;
}

/*********************************************************************
* @purpose  Change the size of the ARP cache
*
* @param    maxEnt      {@b(Input)}  maximum number of ARP entries allowed
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheResize(word maxEnt)
{
  return (ipMapArpIpTableResize((L7_uint32)maxEnt) == L7_SUCCESS) ? E_OK : E_FAILED;
}

/*********************************************************************
* @purpose  Check if the ARP cache is currently full
*
* @param    arpCacheId    {@b(Input)}   ARP cache instance identifier
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
Bool ipMapArpExtenCacheFullCheck(t_Handle arpCacheId)
{
  return (ipMapArpIpTableFullCheck() == L7_TRUE) ? TRUE : FALSE;
}

/*********************************************************************
* @purpose  Check if the specified ip address is the primary address
*           on the given interface
*
* @param    index     {@b(Input)}   ARP interface index number
* @param    ipAddr    {@b(Input)}   Ip Address to be tested
*
* @returns  TRUE      This is the primary address on the interface
* @returns  FALSE     This is not the primary address on the interface  
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    Do NOT call this function from outside the vendor code as
*           that will result in unprotected access to the ARP interface
*           structure.
*
* @end
*********************************************************************/
Bool ipMapArpExtenIpAddrIsPrimary(word index, byte* pIpAddr)
{
	ipMapArpIntf_t  *pIntf = L7_NULLPTR;
    L7_IP_ADDR_t    ipAddr;
    L7_uint32 localIP;

	if ((pIpAddr == L7_NULLPTR) || (index >= IPM_ARP_INTF_MAX))
	{
		return FALSE;
	}

    memcpy(&localIP,pIpAddr,4);
    ipAddr = (L7_IP_ADDR_t)osapiNtohl(localIP);

	pIntf  = ipMapArpIntfInUse_g[index];
    if (pIntf->ipAddr == ipAddr)
		return TRUE;

	return FALSE;
}

/*********************************************************************
* @purpose  Check if two addresses on a given interface are in the same
*           IP subnet
*
* @param    index     {@b(Input)}   ARP interface index number
* @param    targetIP  {@b(Input)}   target IP address in an ARP request we're sending
* @param    srcIP     {@b(Input)}   a candidate for the source IP address 
*
* @returns  TRUE      targetIP and srcIP are in the same subnet
* @returns  FALSE     
*
* @end
*********************************************************************/
Bool ipMapArpExtenSameSubnet(word index, byte *targetIP, byte *srcIP)
{
  ipMapArpIntf_t *pIntf = L7_NULLPTR;
  L7_uint32 tgt;
  L7_uint32 src;
  L7_uint32 i;
  L7_ulong32 tmp;

  if (!targetIP || !srcIP || (index >= IPM_ARP_INTF_MAX))
  {
    return FALSE;
  }

  tmp = (targetIP[0] << 24) | (targetIP[1] << 16) | (targetIP[2] << 8) | (targetIP[3]) ;
  tgt = osapiNtohl(tmp);
  tmp = (srcIP[0] << 24) | (srcIP[1] << 16) | (srcIP[2] << 8) | (srcIP[3]) ;    
  src = osapiNtohl(tmp);
  
  pIntf  = ipMapArpIntfInUse_g[index];

  /* Find srcIP and its corresponding network mask on this interface */
  /* Check primary */
  if ((pIntf->ipAddr == src) &&
      ((src & pIntf->netMask) == (tgt & pIntf->netMask)))
  {
    return TRUE;
  }

  /* check secondaries */
  for (i = 0; i < L7_L3_NUM_SECONDARIES; i++)
  {
    if (pIntf->secondaries[i].ipAddr &&
        (pIntf->secondaries[i].ipAddr == src) &&
        ((src & pIntf->secondaries[i].ipMask) == (tgt & pIntf->secondaries[i].ipMask)))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*********************************************************************
* @purpose  Returns internal intf number for intf registerd with ARP.
*
* @param    index     {@b(Input)}   ARP interface index number
* @param    intIfNum  {@b(Output)}  Internal interface number of ARP interface
*
* @returns  E_OK      Success
* @returns  E_FAILED  Could not find the internal interface number
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenGetIntIfNum(word index, L7_uint32 *intIfNum)
{
    if (ipMapArpIntfInUse_g[(L7_uint32)index] == L7_NULLPTR)
    {
      return E_FAILED;
    }
    *intIfNum = ipMapArpIntfInUse_g[index]->intIfNum;
    return E_OK;
}


/*********************************************************************
* @purpose  Checks if the given interface is of type unnumbered.
*
* @param    index     {@b(Input)}   ARP interface index number
*
* @returns  TRUE if interface is unnumbered
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
Bool ipMapArpExtenIsUnnumbIntf(word index)
{
  return ipMapArpIntfInUse_g[index]->unnumbered;
}

/*********************************************************************
* @purpose  Checks if proxy ARP is enabled on an interface.
*
* @param    index     {@b(Input)}   ARP interface index number
*
* @returns  TRUE if proxy ARP is enabled on the interface
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
Bool ipMapArpExtenProxyArpEnabled(word index)
{
  return ipMapArpIntfInUse_g[index]->proxyArp;
}

/*********************************************************************
* @purpose  Checks if the given interface is of type un-numbered.
*
* @param    index     {@b(Input)}   ARP interface index number
*
* @returns  TRUE if local proxy ARP is enabled on the interface
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
Bool ipMapArpExtenLocalProxyArpEnabled(word index)
{
  return ipMapArpIntfInUse_g[index]->localProxyArp;
}

/*********************************************************************
* @purpose  Displays the contents of the ARP table, either in its 
*           entirety, or for a specific interface index
*
* @param    intIfNum    internal interface number
*
* @returns  void
*
* @notes    This function intended for debugging use only.
*       
* @end
*********************************************************************/
void ipMapArpShow(L7_uint32 intIfNum)
{
  t_ARP         *pArpObj = (t_ARP *)ipMapArpCtx_g.arpHandle;
  t_ADR         *pAdr;
  ipMapArpIntf_t *pIntf;
  L7_uint32     i, ipAddr, count;
  e_Err         e;
  L7_uchar8     ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8     *pStatus;
  L7_ulong32    tmp;
  L7_uchar8     *tmpip;

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\nARP TABLE METRICS:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  config values:  agetime=%u resptime=%u retries=%u "
              "cachesize=%u dynamicrenew=%u\n",
              pArpObj->arpAgeTime, pArpObj->arpRespTime, pArpObj->retrNmb,
              pArpObj->arpCacheSize, pArpObj->dynamicRenew);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  overall entry count:  current=%u peak=%u max=%u\n",
              ipMapArpCtx_g.cacheCurrCt, ipMapArpCtx_g.cachePeakCt, 
              ipMapArpCtx_g.cacheMaxCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  static entry count:  current=%u max=%u\n",
              ipMapArpCtx_g.staticCurrCt, ipMapArpCtx_g.staticMaxCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  bad source discards:  %u\n",
              ipMapArpCtx_g.inSrcDiscard);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  bad target discards:  %u\n",
              ipMapArpCtx_g.inTgtDiscard);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n");

  if (intIfNum == 0)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
                "\nDisplaying entire ARP Table contents\n\n");
  }
  else if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
                "\n\nInvalid interface %d (must be for a router intf or 0)\n\n",
                intIfNum);
    return;
  }
  else
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
                "\n\nDisplaying ARP Table entries for interface %d\n\n", 
                intIfNum);
  }

  /* display output header */
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  ENTRY#        IP ADDR           MAC ADDR      INTF  RTRY  INDEV   TYPE   LAST REFRESHED\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  ------    ---------------  -----------------  ----  ----  ----- -------- --------------\n");

  /* NOTE:  Display each entry if intIfNum is specified as 0.  Otherwise,
   *        only show entries associated with the interface (in this case,
   *        the ARP entries belonging to interface instance 0 are not shown).
   *        This applies for both local and non-local entries.
   */

  /* display entries */
  count = 0;
  e = ipMapArpExtenCacheGetFirst(pArpObj->arpCacheId, &pAdr);
  while (e == E_OK)
  {
    if (pAdr == (t_ADR *)NULLP)
      break;

    pIntf = ipMapArpIntfInUse_g[pAdr->intfNum];
    if (pIntf == L7_NULL)
      pIntf = ipMapArpIntfInUse_g[0];

    if ((intIfNum == 0) || (intIfNum == pIntf->intIfNum))
    {
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  %6u    ", ++count);
      tmpip = pAdr->ipAddr;
      tmp = (tmpip[0] << 24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3] ) ;
      ipAddr = (L7_uint32)osapiNtohl(tmp);
      osapiInetNtoa(ipAddr, ipStr);
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%-15.15s  ", ipStr);
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%2.2x:", pAdr->macAddr[i]);
      }
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%2.2x  ", pAdr->macAddr[i]);
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, " %2.1d   ", pIntf->intIfNum);
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, " %2u   ", (L7_uint32)pAdr->retrNmb);
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  %1s   ", 
                  (((ipMapArpIpNode_t *)(pAdr->pNode))->inDevice == L7_TRUE) ? "Y" : "N");

      pStatus = (L7_uchar8 *)ipMapArpEntryTypeStr[pAdr->entryType];
      if (pAdr->permanent != TRUE)
        if (ipMapArpIsMacZero(pAdr->macAddr) == L7_TRUE)
          pStatus = (L7_uchar8 *)"Pending";

      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%-8.8s ", pStatus);
      if (pAdr->permanent != TRUE)
      {
        ulng   secs, mins, hrs;
        ipMapArpExtenEntryAgeCalc(pAdr->timeStamp, (ulng *)&secs);
        hrs = secs / SECONDS_PER_HOUR;
        secs -= (hrs * SECONDS_PER_HOUR);
        mins = secs / SECONDS_PER_MINUTE;
        secs -= (mins * SECONDS_PER_MINUTE);
        IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%uh %um %us", hrs, mins, secs);
      }
      else
      {
        IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "n/a");
      }
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n");
    }

    e = ipMapArpExtenCacheGetNext(pArpObj->arpCacheId, pAdr, &pAdr);

  } /* endwhile */

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n");
}


/* convenient "show arp table" shortcut command for debug use */
void sharp(L7_uint32 intIfNum)
{
  ipMapArpShow(intIfNum);
}


/* a different "show arp table" debug command that uses the USMDB function */

#include "usmdb_ip_api.h"

#ifndef U_IDX
#define U_IDX   1
#endif

void sharp2Display(L7_arpEntry_t *pArp);

void sharp2(L7_int32 mode)
{
  L7_arpEntry_t arp, *pArp;
  L7_IP_ADDR_t  ip;
  L7_arpCacheStats_t  arpStats;
  L7_uint32     ageTime, respTime, retries, cacheSize, dynamicRenew;
  L7_uint32     count;
  L7_uint32     intIfNum;

  memset(&arpStats, 0, sizeof(arpStats));
  (void)usmDbIpArpCacheStatsGet(U_IDX, &arpStats);

  (void)usmDbIpArpAgeTimeGet(U_IDX, &ageTime);
  (void)usmDbIpArpRespTimeGet(U_IDX, &respTime);
  (void)usmDbIpArpRetriesGet(U_IDX, &retries);
  (void)usmDbIpArpCacheSizeGet(U_IDX, &cacheSize);
  (void)usmDbIpArpDynamicRenewGet(U_IDX, &dynamicRenew);

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\nARP TABLE METRICS:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  config values:  agetime=%u resptime=%u retries=%u "
              "cachesize=%u dynamicrenew=%u\n",
              ageTime, respTime, retries, cacheSize, dynamicRenew);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  overall entry count:  current=%u peak=%u max=%u\n",
              arpStats.cacheCurrent, arpStats.cachePeak, arpStats.cacheMax);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  static entry count:  current=%u max=%u\n",
              arpStats.staticCurrent, arpStats.staticMax);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  bad source discards:  %u\n",
              ipMapArpCtx_g.inSrcDiscard);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
              "  bad target discards:  %u\n",
              ipMapArpCtx_g.inTgtDiscard);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n");

  if (mode >= 0)
  {
    /* display output header */
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
                "  Entry#        IP ADDR           MAC ADDR      INTF  FLAGS   VID   HITS\n");
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, 
                "  ------    ---------------  -----------------  ----  ------  ----  ----\n");

    pArp = &arp;
    memset(pArp, 0, sizeof(L7_arpEntry_t));

    /* display each ARP entry (regardless of whether local or not) */
    count = 0;
    ip = 0;                               /* start with first entry */
    intIfNum = 0;
    while (usmDbIpArpEntryNext(U_IDX, ip, intIfNum, pArp) == L7_SUCCESS)
    {
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  %6u    ", ++count);
      sharp2Display(pArp);
      ip = pArp->ipAddr;
      intIfNum = pArp->intIfNum;
    }

    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n");
  }
}

void sharp2Display(L7_arpEntry_t *pArp)
{
  L7_uchar8     ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32     i, fl;

  osapiInetNtoa(pArp->ipAddr, ipStr);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%-15.15s  ", ipStr);
  for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%2.2x:", pArp->macAddr.addr.enetAddr.addr[i]);
  }
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%2.2x  ", pArp->macAddr.addr.enetAddr.addr[i]);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, " %2.1d   ", pArp->intIfNum);
  fl = pArp->flags;
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, (fl & L7_ARP_LOCAL) ? "L" : "-");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, (fl & L7_ARP_GATEWAY) ? "G" : "-");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, (fl & L7_ARP_RESOLVED) ? "R" : "-");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, (fl & L7_ARP_NET_DIR_BCAST) ? "B" : "-");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, (fl & L7_ARP_DISCARD_WHEN_SOURCE) ? "D" : "-");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, (fl & L7_ARP_STATIC) ? "S" : "-");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  %4.1d ", pArp->vlanId);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, " %4.1d ", pArp->hits);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n");
}

