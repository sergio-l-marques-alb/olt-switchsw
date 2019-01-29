/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
********************************************************************************
*
* @filename  broad_l3_xgs3.c
*
* @purpose   This file contains functions to write L3 entries to XGS3 hardware.
*            To improve L3 performance on large stacks, caching of L3 entries 
*            is supported where multiple L3 entries are cached and programmed 
*            in hardware with a single bulk operation. When large number of 
*            routes are added or deleted, the overall system performance gets 
*            affected due to RPC transactions (one route operation per transaction).
*            Caching L3 entries enables us to carry more L3 entries in a single
*            RPC transaction.
*
* @component HAPI
*
* @comments  XGS2 devices are not supported.
*
* @create    10/1/2007
*
* @author    sdoke
*
* @end
*
*******************************************************************************/
#include "l7_common.h"
#include "broad_l3_int.h"
#include "hpc_hw_api.h"

#include "l7_usl_bcmx_l3.h"
#include "l7_usl_bcm.h"

/* L3 command for the cached entries */
typedef enum {
  BROAD_L3_HW_CMD_NO_OP = 0,
  BROAD_L3_HW_CMD_ADD,       /* Add entries to hardware */
  BROAD_L3_HW_CMD_DEL        /* Delete entries from hardware */
} BROAD_L3_HW_CMD_t;

/* Cache for Next hop entries */
typedef struct BROAD_L3_NHOP_CACHE_s
{
  BROAD_L3_HW_CMD_t        cmd;               /* HW command for the cached nexthops */
  L7_uint32                numEntries;        /* Number of nexthops cached */
  L7_uint32                maxEntries;        /* Max entries that can be cached */
  L7_uint32               *pFlags;           /* Egress object flags */
  BROAD_L3_NH_ENTRY_t    **pNhopList;     /* List of entries in cache */
  usl_bcm_l3_egress_t     *pBcmInfo;      /* BCM data assoicated with next hops */
  bcm_if_t                *pEgressId;      /* Egrees Interface id */
} BROAD_L3_NHOP_CACHE_t;

/* Cache for Host entries */
typedef struct BROAD_L3_HOST_CACHE_s
{
  BROAD_L3_HW_CMD_t cmd;               /* HW command for cached host entries */
  L7_uint32         numEntries;        /* Number of host entries cached */
  L7_uint32         maxEntries;        /* Max entries that can be cached */
  BROAD_L3_HOST_ENTRY_t **pHostList;   /* List of host entries in cache */
  usl_bcm_l3_host_t    *pBcmInfo;        /* BCM data associated with hosts */
} BROAD_L3_HOST_CACHE_t;

/* Cache for Route entries */
typedef struct BROAD_L3_ROUTE_CACHE_s
{
  BROAD_L3_HW_CMD_t cmd;               /* HW command for the cached routes */
  L7_uint32         numEntries;        /* Number of routes cached */
  L7_uint32         maxEntries;        /* Max entries that can be cached */
  BROAD_L3_ROUTE_ENTRY_t **pRouteList; /* List of route entries in cache */
  usl_bcm_l3_route_t      *pBcmInfo;   /* BCMX data associated with routes */
} BROAD_L3_ROUTE_CACHE_t;

/* Cache stats */
static L7_uint32 maxNhopAddsInCache = 0;
static L7_uint32 maxNhopDelsInCache = 0;
static L7_uint32 maxHostAddsInCache = 0;
static L7_uint32 maxHostDelsInCache = 0;
static L7_uint32 maxRouteAddsInCache = 0;
static L7_uint32 maxRouteDelsInCache = 0;

/* Nhop cache */
static BROAD_L3_NHOP_CACHE_t  hapiBroadL3NhopCache;

/* Host cache */
static BROAD_L3_HOST_CACHE_t  hapiBroadL3HostCache;

/* Route cache */
static BROAD_L3_ROUTE_CACHE_t hapiBroadL3RouteCache;

/* Buffer for return codes */
static L7_int32 rv_array[512];

/* Funtion prototypes */
static void hapiBroadL3NhopCacheClear (void);
static void hapiBroadL3HostCacheClear (void);
static void hapiBroadL3RouteCacheClear (void);
static void hapiBroadL3NhopCacheCmdSet (BROAD_L3_HW_CMD_t cmd);
static void hapiBroadL3HostCacheCmdSet (BROAD_L3_HW_CMD_t cmd);
static void hapiBroadL3RouteCacheCmdSet (BROAD_L3_HW_CMD_t cmd);

/*******************************************************************************
*
* @purpose Allocate and Initialize L3 cache
*
* @param   none
*
* @returns L7_RC_t result
*
* @notes   Called only once during L3 Init.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3CacheInit(void)
{
  L7_uint32 size;

  /* Init the next hop cache */
  memset(&hapiBroadL3NhopCache, 0, sizeof(hapiBroadL3NhopCache));
  hapiBroadL3NhopCache.maxEntries = 1; /* No caching by default */

  /* Init the next hop cache */
  memset(&hapiBroadL3HostCache, 0, sizeof(hapiBroadL3HostCache));
  hapiBroadL3HostCache.maxEntries = 1; /* No caching by default */

  /* Init the route cache */
  memset(&hapiBroadL3RouteCache, 0, sizeof(hapiBroadL3RouteCache));
  hapiBroadL3RouteCache.maxEntries = 1; /* No caching by default */

  /* Set the cache size based on how much underlying layer can handle in a 
   * single transaction. This way we require minimal memory for cache with
   * max. performance.
   */
  hapiBroadL3NhopCache.maxEntries = usl_l3_egress_nhop_max_entries_get(); 
  if (hapiBroadL3NhopCache.maxEntries == 0)
    return L7_FAILURE;

  hapiBroadL3HostCache.maxEntries = usl_l3_host_max_entries_get();
  if (hapiBroadL3HostCache.maxEntries == 0)
    return L7_FAILURE;

  hapiBroadL3RouteCache.maxEntries = usl_l3_route_max_entries_get();
  if (hapiBroadL3RouteCache.maxEntries == 0)
    return L7_FAILURE;
  
  /* Alloc the next hop cache */
  size = sizeof(usl_bcm_l3_egress_t) * hapiBroadL3NhopCache.maxEntries;
  hapiBroadL3NhopCache.pBcmInfo = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);

  if (hapiBroadL3NhopCache.pBcmInfo == L7_NULLPTR)
    return L7_FAILURE;

  size = sizeof (L7_uint32) * hapiBroadL3NhopCache.maxEntries;
  hapiBroadL3NhopCache.pFlags = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);

  if (hapiBroadL3NhopCache.pFlags == L7_NULLPTR)
    return L7_FAILURE;

  size = sizeof (BROAD_L3_NH_ENTRY_t *) * hapiBroadL3NhopCache.maxEntries;
  hapiBroadL3NhopCache.pNhopList = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);

  if (hapiBroadL3NhopCache.pNhopList == L7_NULLPTR)
    return L7_FAILURE;

  size = sizeof (bcm_if_t) * hapiBroadL3NhopCache.maxEntries;
  hapiBroadL3NhopCache.pEgressId = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);

  hapiBroadL3NhopCacheClear();

  /* Alloc the host cache */
  size = sizeof(usl_bcm_l3_host_t) * hapiBroadL3HostCache.maxEntries;
  hapiBroadL3HostCache.pBcmInfo = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);

  if (hapiBroadL3HostCache.pBcmInfo == L7_NULLPTR)
    return L7_FAILURE;

  size = sizeof (BROAD_L3_HOST_ENTRY_t *) * hapiBroadL3HostCache.maxEntries;
  hapiBroadL3HostCache.pHostList = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);
  
  if (hapiBroadL3HostCache.pHostList == L7_NULLPTR)
    return L7_FAILURE;

  /* All is ok. Clear it */
  hapiBroadL3HostCacheClear();
  
  /* Alloc the route cache */
  size = sizeof(usl_bcm_l3_route_t) * hapiBroadL3RouteCache.maxEntries;
  hapiBroadL3RouteCache.pBcmInfo = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);

  if (hapiBroadL3RouteCache.pBcmInfo == L7_NULLPTR)
    return L7_FAILURE;

  size = sizeof (BROAD_L3_ROUTE_ENTRY_t *) * hapiBroadL3RouteCache.maxEntries;
  hapiBroadL3RouteCache.pRouteList = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);
  
  if (hapiBroadL3RouteCache.pRouteList == L7_NULLPTR)
    return L7_FAILURE;

  /* All is ok. Clear it */
  hapiBroadL3RouteCacheClear();

  usl_db_sync_failure_notify_callback_register(USL_L3_HOST_DB_ID, hapiBroadL3AsyncHostFailureCallback);

  /* A semaphore lock for cache is not required as it is expected that the hw
   * L3 operations are called from L3 ASYNC TASK only.
   */
  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Set the HW command for the Nexthop cache
*
* @param   cmd - Add/Delete
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3NhopCacheCmdSet (BROAD_L3_HW_CMD_t cmd)
{
  /* Check if any entries are already in cache */
  if (hapiBroadL3NhopCache.numEntries > 0)
  {
    /* Check the current cmd on cached entries */
    if (hapiBroadL3NhopCache.cmd == BROAD_L3_HW_CMD_NO_OP)
    {
      /* If there are entries in cache, this shouldn't be the case */
      HAPI_BROAD_L3_L7_LOG_ERROR(hapiBroadL3NhopCache.cmd);
    }
    else if (hapiBroadL3NhopCache.cmd == cmd)
    {
      /* Nothing to do here */
      return;
    }
    else if (hapiBroadL3NhopCache.cmd != cmd)
    {
      /* Cache command has changed. Commit the current entries in cache */
      hapiBroadL3NhopCacheCommit();
      hapiBroadL3NhopCache.cmd = cmd;
    }
  }
  else
  {
    hapiBroadL3NhopCache.cmd = cmd;
  }
}


/*******************************************************************************
*
* @purpose Create/cache L3 Nhop egress object in hardware
*
* @param   egrFlags      - Egress object flags
* @param   pNhop         - pointer to internal L3 nexthop struct
* @param   pBcmxInfo     - BCMX data assoicated with the L3 nexthop egress object.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 Nexthop cache is commited to hardware,
*            - when Nexthop cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwNhopAdd (L7_uint32 egrFlags, 
                           BROAD_L3_NH_ENTRY_t *pNhopEntry,
                           usl_bcm_l3_egress_t *pBcmInfo,
                           L7_BOOL cacheOrCommit)
{
  L7_uint32 cacheIdx = 0;

  hapiBroadL3NhopCacheCmdSet(BROAD_L3_HW_CMD_ADD);

  cacheIdx = hapiBroadL3NhopCache.numEntries;

  /* Cache the entry */
  hapiBroadL3NhopCache.pFlags[cacheIdx] = egrFlags;
  hapiBroadL3NhopCache.pEgressId[cacheIdx] = pNhopEntry->egressId;
  hapiBroadL3NhopCache.pNhopList[cacheIdx] = pNhopEntry;
  memcpy (&hapiBroadL3NhopCache.pBcmInfo[cacheIdx],
          pBcmInfo, sizeof (usl_bcm_l3_egress_t));

  hapiBroadL3NhopCache.numEntries++;

  if (maxNhopAddsInCache < hapiBroadL3NhopCache.numEntries)
    maxNhopAddsInCache = hapiBroadL3NhopCache.numEntries;

  if ((hapiBroadL3NhopCache.numEntries == hapiBroadL3NhopCache.maxEntries) ||
      (cacheOrCommit == BROAD_L3_ENTRY_COMMIT))
  {
    hapiBroadL3NhopCacheCommit();
  }
}


/*******************************************************************************
*
* @purpose Delete/cache L3 next hop egress object (s) from hardware
*
* @param   pbcmInfo      - BCMX data assoicated with the L3 egress object.
* @param   egressId      - L3 Egress object id
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 Nhop cache is commited to hardware,
*            - when Nhop cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwNhopDelete (usl_bcm_l3_egress_t *pBcmInfo,
                              bcm_if_t egressId, 
                              L7_BOOL  cacheOrCommit)
{
  L7_uint32 cacheIdx = 0;

  hapiBroadL3NhopCacheCmdSet(BROAD_L3_HW_CMD_DEL);

  cacheIdx = hapiBroadL3NhopCache.numEntries;

  /* Note: For delete, the internal nhop struct is not available */

  /* Cache the entry */
  hapiBroadL3NhopCache.pEgressId[cacheIdx] = egressId;
  memcpy (&hapiBroadL3NhopCache.pBcmInfo[cacheIdx],
          pBcmInfo, sizeof (usl_bcm_l3_egress_t));

  hapiBroadL3NhopCache.numEntries++;

  if (maxNhopDelsInCache < hapiBroadL3NhopCache.numEntries)
    maxNhopDelsInCache = hapiBroadL3NhopCache.numEntries;

  if ((hapiBroadL3NhopCache.numEntries == hapiBroadL3NhopCache.maxEntries) ||
      (cacheOrCommit == BROAD_L3_ENTRY_COMMIT))
  {
    /* If we have max entries or caller has explicitly flagged for commit,
     * commit the cache entries to hardware
     */
    hapiBroadL3NhopCacheCommit();
  }
}


/*******************************************************************************
*
* @purpose Commit the Nexthop cache to hardware
*
* @param   none
*
* @returns none
*
* @notes   If cache command is set to ADD, then cached nexthop entries are added
*          to hardware. If cmd is DELETE, then entries are deleted from hardware.
*
* @end
*
*******************************************************************************/
void hapiBroadL3NhopCacheCommit (void)
{
  L7_uint32 i;
  L7_uint32 count;
  L7_int32  rv;
  L7_RC_t   result;
  BROAD_L3_NH_ENTRY_t *pNhop;
  usl_bcm_l3_egress_t *pBcmInfo;
  bcm_if_t            *pEgrIntf;
  L7_uint32           *pFlags;

  count = hapiBroadL3NhopCache.numEntries;

  if ((count == 0) && (hapiBroadL3NhopCache.cmd == BROAD_L3_HW_CMD_NO_OP))
  {
    /* No entries in cache. Return. Caller might flush the cache to make
     * sure that entries are not sitting in cache.
     */
    return;
  }

  pBcmInfo = hapiBroadL3NhopCache.pBcmInfo;
  pEgrIntf  = hapiBroadL3NhopCache.pEgressId;
  pFlags    = hapiBroadL3NhopCache.pFlags;

  /* Set default to Internal Error */
  memset(rv_array, BCM_E_INTERNAL, sizeof(rv_array));

  if (hapiBroadL3NhopCache.cmd == BROAD_L3_HW_CMD_ADD)
  {
    result = usl_bcmx_l3_egress_create(pFlags, pBcmInfo,
                                       count, pEgrIntf, rv_array);

    if (result != L7_SUCCESS)
    {
      /* Egress create operation didn't complete */
      HAPI_BROAD_L3_BCMX_DBG(-1, "usl_bcmx_l3_egress_create: failed\n");
    }

    /* Handle return code(s) */
    for (i=0; i < count; i++)
    {
      pNhop = hapiBroadL3NhopCache.pNhopList[i];
      rv = rv_array[i];

      HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_egress_create returned (%s)"
                             " egress Id %d: flags %d\n", bcm_errmsg(rv),
                             pEgrIntf[i], pFlags[i]);

      if (pFlags[i] & BCM_L3_REPLACE)
      {
        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwNhopStats, 1, rv);
      }
      else
      {
        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwNhopStats, 0, rv);
      }

      pNhop->rv = rv; 

      if (rv == BCM_E_NONE) /* Do not use L7_BCMX_OK() here */
      {
        pNhop->egressId = pEgrIntf[i]; /* If successful, egress id is returned */
      }
      else
      {
        
        if ((pFlags[i] & BCM_L3_REPLACE) == L7_FALSE)
        {
          /* If create failed, destroy the object so that all units are in sync */
          if (pEgrIntf[i] != HAPI_BROAD_L3_INVALID_EGR_ID)
          {
            /* Clean up the failed entry */
            usl_bcmx_l3_egress_destroy(pBcmInfo, &pEgrIntf[i], 1, &rv);
          }
        }
      }
    }
  }
  else if (hapiBroadL3NhopCache.cmd == BROAD_L3_HW_CMD_DEL)
  {
    if (usl_bcmx_l3_egress_destroy(pBcmInfo, pEgrIntf, count, rv_array) == L7_SUCCESS)
    {
      for (i=0; i < count; i++)
      {
        rv = rv_array[i];
        HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_egress_destroy returned (%s)"
                  "egress Id %d", bcm_errmsg(rv), pEgrIntf[i]);

        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwNhopStats, 2, rv);
        /* Error handling for Delete - TBD */
      }
    }
    else
    {
      HAPI_BROAD_L3_BCMX_DBG(-1, "usl_bcmx_l3_egress_destroy: failed\n");
    }
  }
  else
  {
    /* This shouldn't be the case if numEntries > 0 */
    HAPI_BROAD_L3_L7_LOG_ERROR(hapiBroadL3NhopCache.cmd);
  }

  /* Clear up cache */
  hapiBroadL3NhopCacheClear();
}


/*******************************************************************************
*
* @purpose Clear the Next hop cache
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
static void hapiBroadL3NhopCacheClear (void)
{
  L7_uint32 size;

  hapiBroadL3NhopCache.numEntries = 0;
  hapiBroadL3NhopCache.cmd = BROAD_L3_HW_CMD_NO_OP;

  size = sizeof(usl_bcm_l3_egress_t) * hapiBroadL3NhopCache.maxEntries;
  memset(hapiBroadL3NhopCache.pBcmInfo, 0, size);

  size = sizeof(L7_uint32) * hapiBroadL3NhopCache.maxEntries;
  memset(hapiBroadL3NhopCache.pFlags, 0, size);

  size = sizeof (BROAD_L3_NH_ENTRY_t *) * hapiBroadL3NhopCache.maxEntries;
  memset(hapiBroadL3NhopCache.pNhopList, 0, size);

  size = sizeof (bcm_if_t) * hapiBroadL3NhopCache.maxEntries;
  memset(hapiBroadL3NhopCache.pEgressId, HAPI_BROAD_L3_INVALID_EGR_ID, size);
}


/*******************************************************************************
*
* @purpose Add/cache L3 host entry to hardware
*
* @param   pHostEntry    - pointer to internal L3 host struct
* @param   pBcmxInfo     - BCMX data assoicated with the L3 host.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 host cache is commited to hardware,
*            - when host cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwHostAdd (BROAD_L3_HOST_ENTRY_t *pHostEntry,
                           usl_bcm_l3_host_t *pBcmInfo,
                           L7_BOOL          cacheOrCommit)
{
  L7_uint32 cacheIdx = 0;

  hapiBroadL3HostCacheCmdSet(BROAD_L3_HW_CMD_ADD);

  cacheIdx = hapiBroadL3HostCache.numEntries;

  /* Cache the entry */
  hapiBroadL3HostCache.pHostList[cacheIdx] = pHostEntry;
  memcpy (&hapiBroadL3HostCache.pBcmInfo[cacheIdx],
          pBcmInfo, sizeof (usl_bcm_l3_host_t));

  hapiBroadL3HostCache.numEntries++;

  if (maxHostAddsInCache < hapiBroadL3HostCache.numEntries)
    maxHostAddsInCache = hapiBroadL3HostCache.numEntries;

  if ((hapiBroadL3HostCache.numEntries == hapiBroadL3HostCache.maxEntries) ||
      (cacheOrCommit == BROAD_L3_ENTRY_COMMIT))
  {
    hapiBroadL3HostCacheCommit();
  }
}


/*******************************************************************************
*
* @purpose Set the HW command for the host cache
*
* @param   cmd - Add/Delete
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3HostCacheCmdSet (BROAD_L3_HW_CMD_t cmd)
{
  /* Check if any entries are already in cache */
  if (hapiBroadL3HostCache.numEntries > 0)
  {
    /* Check the current cmd on cached entries */
    if (hapiBroadL3HostCache.cmd == BROAD_L3_HW_CMD_NO_OP)
    {
      /* If there are entries in cache, this shouldn't be the case */
      HAPI_BROAD_L3_L7_LOG_ERROR(hapiBroadL3HostCache.cmd);
    }
    else if (hapiBroadL3HostCache.cmd == cmd)
    {
      /* Nothing to do here */
      return;
    }
    else if (hapiBroadL3HostCache.cmd != cmd)
    {
      /* Cache command has changed. Commit the current entries in cache */
      hapiBroadL3HostCacheCommit();
      hapiBroadL3HostCache.cmd = cmd;
    }
  }
  else
  {
    /* No entries. No command is pending. Set it */
    hapiBroadL3HostCache.cmd = cmd;
  }
}


/*******************************************************************************
*
* @purpose Delete/cache L3 host entry from hardware
*
* @param   pBcmxInfo     - BCMX data assoicated with the L3 host.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 host cache is commited to hardware,
*            - when host cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwHostDelete (usl_bcm_l3_host_t *pBcmInfo,
                              L7_BOOL        cacheOrCommit)
{
  L7_uint32 cacheIdx = 0;

  hapiBroadL3HostCacheCmdSet(BROAD_L3_HW_CMD_DEL);

  cacheIdx = hapiBroadL3HostCache.numEntries;

  /* Note: For delete, the internal host struct is not available */
  hapiBroadL3HostCache.pHostList[cacheIdx] = L7_NULL;

  /* Cache the entry */
  memcpy (&hapiBroadL3HostCache.pBcmInfo[cacheIdx],
          pBcmInfo, sizeof (usl_bcm_l3_host_t));

  hapiBroadL3HostCache.numEntries++;

  if (maxHostDelsInCache < hapiBroadL3HostCache.numEntries)
    maxHostDelsInCache = hapiBroadL3HostCache.numEntries;

  if ((hapiBroadL3HostCache.numEntries == hapiBroadL3HostCache.maxEntries) ||
      (cacheOrCommit == BROAD_L3_ENTRY_COMMIT))
  {
    /* If we have max entries or caller has explicitly flagged for commit,
     * commit the cache entries to hardware
     */
    hapiBroadL3HostCacheCommit();
  }
}

/*******************************************************************************
*
* @purpose Commit the Host cache to hardware
*
* @param   none
*
* @returns none
*
* @notes   If cache command is set to ADD, then cached host entries are added 
*          to hardware. If cmd is DELETE, then entries are deleted from hardware.
*
* @end
*
*******************************************************************************/
void hapiBroadL3HostCacheCommit (void)
{
  L7_uint32 i;
  L7_uint32 count;
  L7_int32  rv;
  BROAD_L3_HOST_ENTRY_t *pHost;
  usl_bcm_l3_host_t      *pBcmInfo;  

  count = hapiBroadL3HostCache.numEntries;

  if ((count == 0) && (hapiBroadL3HostCache.cmd == BROAD_L3_HW_CMD_NO_OP)) 
  {
    /* No entries in cache. Return. Caller might flush the cache to make 
     * sure that entries are not sitting in cache.
     */
    return;
  }

  pBcmInfo = hapiBroadL3HostCache.pBcmInfo;

  /* Set default to Internal Error */
  memset(rv_array, BCM_E_INTERNAL, sizeof(rv_array)); 

  if (hapiBroadL3HostCache.cmd == BROAD_L3_HW_CMD_ADD)
  {
    if (usl_bcmx_l3_host_add(pBcmInfo, count, rv_array) != L7_SUCCESS)
    {
      HAPI_BROAD_L3_BCMX_DBG(-1, "usl_bcmx_l3_host_add: failed\n");
    }

    /* Handling return code(s) */
    for (i=0; i < count; i++)
    {
      pHost = hapiBroadL3HostCache.pHostList[i];

      HAPI_BROAD_L3_BCMX_DBG(rv_array[i], "usl_bcmx_l3_host_add returned (%s) "
                             "egress Id %d, flags %d\n", bcm_errmsg(rv_array[i]), 
                             pBcmInfo[i].l3a_intf, pBcmInfo[i].l3a_flags);     

      if (pBcmInfo[i].l3a_flags & BCM_L3_REPLACE)
      {
        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwHostStats, 1, rv_array[i]);
      }
      else
      {
        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwHostStats, 0, rv_array[i]);
      }

      if (rv_array[i] != BCM_E_NONE) /* Do not use L7_BCMX_OK() here */
      {
        /* Since a box may have different device types, some devices may
         * actually have inserted the host, so we need to delete it now
         * so that everybody has a consistent picture of the host table.
         * This is best effort only. Can't do much about failures here
         */
        if (pHost->rv == 0)
        {
          broadL3HwHostStats.current_add_failures++;
          pHost->rv = rv_array[i]; /* Set the error code for retry */
        }
        usl_bcmx_l3_host_delete(&pBcmInfo[i], 1, &rv);
      }
      else
      {
        if (pHost->rv != 0)
        {
          /* Previously failed host got in! */
          pHost->rv = 0;
          broadL3HwHostStats.current_add_failures--;
        }
      }
    } 
  }
  else if (hapiBroadL3HostCache.cmd == BROAD_L3_HW_CMD_DEL)
  {
    if (usl_bcmx_l3_host_delete(pBcmInfo, count, rv_array) == L7_SUCCESS)
    {
      for (i=0; i < count; i++)
      {
        rv = rv_array[i];
        HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_host_delete returned (%s)"
                  "egress Id %d", bcm_errmsg(rv), pBcmInfo[i].l3a_intf);
 
        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwHostStats, 2, rv_array[i]);
        /* Error handling for Delete - TBD */
      }
    }
    else
    {
      HAPI_BROAD_L3_BCMX_DBG(-1, "usl_bcmx_l3_host_delete: failed\n");
    }
  }
  else
  {
    /* This shouldn't be the case if numEntries > 0 */
    HAPI_BROAD_L3_L7_LOG_ERROR(hapiBroadL3HostCache.cmd);
  }

  /* Clear up cache */
  hapiBroadL3HostCacheClear();
}


/*******************************************************************************
*
* @purpose Clear the Host cache
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
static void hapiBroadL3HostCacheClear (void)
{
  L7_uint32 size;

  hapiBroadL3HostCache.numEntries = 0;
  hapiBroadL3HostCache.cmd = BROAD_L3_HW_CMD_NO_OP;

  size = sizeof(usl_bcm_l3_host_t) * hapiBroadL3HostCache.maxEntries;
  memset(hapiBroadL3HostCache.pBcmInfo, 0, size);

  size = sizeof (BROAD_L3_HOST_ENTRY_t *) * hapiBroadL3HostCache.maxEntries;
  memset(hapiBroadL3HostCache.pHostList, 0, size);
}



/*******************************************************************************
*
* @purpose Add/cache L3 route entry to hardware
*
* @param   pRouteEntry   - pointer to internal L3 route struct
* @param   pBcmxInfo     - BCMX data assoicated with the L3 route.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 route cache is commited to hardware,
*            - when route cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwRouteAdd (BROAD_L3_ROUTE_ENTRY_t *pRouteEntry,
                            usl_bcm_l3_route_t      *pBcmInfo,
                            L7_BOOL          cacheOrCommit)
{
  L7_uint32 cacheIdx = 0;

  hapiBroadL3RouteCacheCmdSet(BROAD_L3_HW_CMD_ADD);

  cacheIdx = hapiBroadL3RouteCache.numEntries;

  /* Cache the entry */
  hapiBroadL3RouteCache.pRouteList[cacheIdx] = pRouteEntry;
  memcpy (&hapiBroadL3RouteCache.pBcmInfo[cacheIdx], 
          pBcmInfo, sizeof (usl_bcm_l3_route_t));

  hapiBroadL3RouteCache.numEntries++;

  if (maxRouteAddsInCache < hapiBroadL3RouteCache.numEntries)
    maxRouteAddsInCache = hapiBroadL3RouteCache.numEntries;

  if ((hapiBroadL3RouteCache.numEntries == hapiBroadL3RouteCache.maxEntries) ||
      (cacheOrCommit == BROAD_L3_ENTRY_COMMIT))
  {
    hapiBroadL3RouteCacheCommit();
  }
}


/*******************************************************************************
*
* @purpose Set the HW command for the route cache
*
* @param   cmd - Add/Delete
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3RouteCacheCmdSet (BROAD_L3_HW_CMD_t cmd)
{
  /* Check if any entries are already in cache */
  if (hapiBroadL3RouteCache.numEntries > 0)
  {
    /* Check the current cmd on cached entries */
    if (hapiBroadL3RouteCache.cmd == BROAD_L3_HW_CMD_NO_OP)
    {
      /* If there are entries in cache, this shouldn't be the case */
      HAPI_BROAD_L3_L7_LOG_ERROR(hapiBroadL3RouteCache.cmd);
    }
    else if (hapiBroadL3RouteCache.cmd == cmd)
    {
      /* Nothing to do here */
      return;
    }
    else if (hapiBroadL3RouteCache.cmd != cmd)
    {
      /* Cache command has changed. Commit the current entries in cache */ 
      hapiBroadL3RouteCacheCommit();
      hapiBroadL3RouteCache.cmd = cmd;
    }
  }
  else
  {
    /* No entries. No command is pending. Set it */
    hapiBroadL3RouteCache.cmd = cmd;
  } 
} 


/*******************************************************************************
*
* @purpose Commit the route cache to hardware
*
* @param   none
*
* @returns none
*
* @notes   If cache command is set to ADD, then cached route entries are added 
*          to hardware. If cmd is DELETE, then entries are deleted from hardware.
*
* @end
*
*******************************************************************************/
void hapiBroadL3RouteCacheCommit (void)
{
  L7_uint32 i;
  L7_uint32 count;
  L7_int32  rv;
  BROAD_L3_ROUTE_ENTRY_t *pRoute;
  usl_bcm_l3_route_t      *pBcmInfo;  

  count = hapiBroadL3RouteCache.numEntries;

  if ((count == 0) && (hapiBroadL3RouteCache.cmd == BROAD_L3_HW_CMD_NO_OP)) 
  {
    /* No entries in cache. Return. Caller might flush the cache to make 
     * sure that entries are not sitting in cache.
     */
    return;
  }

  pBcmInfo = hapiBroadL3RouteCache.pBcmInfo;

  /* Set default to Internal Error */
  memset(rv_array, BCM_E_INTERNAL, sizeof(rv_array)); 

  if (hapiBroadL3RouteCache.cmd == BROAD_L3_HW_CMD_ADD)
  {
    if (usl_bcmx_l3_route_add(pBcmInfo, count, rv_array) != L7_SUCCESS)
    {
      HAPI_BROAD_L3_BCMX_DBG(-1, "usl_bcmx_l3_route_add: failed\n");
    }

    /* Handling return code(s) */
    for (i=0; i < count; i++)
    {
      pRoute = hapiBroadL3RouteCache.pRouteList[i];

      HAPI_BROAD_L3_BCMX_DBG(rv_array[i], "usl_bcmx_l3_route_add returned (%s) "
                             "egress Id %d: flags %d\n", bcm_errmsg(rv_array[i]), 
                             pBcmInfo[i].l3a_intf, pBcmInfo[i].l3a_flags);     

      if (pBcmInfo[i].l3a_flags & BCM_L3_REPLACE)
      {
        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwRouteStats, 1, rv_array[i]);
      }
      else
      {
        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwRouteStats, 0, rv_array[i]);
      }

      if (rv_array[i] != BCM_E_NONE) /* Do not use L7_BCMX_OK() here */
      {
        /* Since a box may have different device types, some devices may
         * actually have inserted the route, so we need to delete it now
         * so that everybody has a consistent picture of the route table.
         * This is best effort only. Can't do much about failures here
         * The route is already marked. 
         */
        if (pRoute->rv == 0)
        {
          broadL3HwRouteStats.current_add_failures++;
          pRoute->rv = rv_array[i]; /* Set the error code for retry */
        }
        usl_bcmx_l3_route_delete(&pBcmInfo[i], 1, &rv);
      }
      else
      {
        if (pRoute->rv != 0)
        {
          /* Previously failed route got in! */
          pRoute->rv = 0;
          broadL3HwRouteStats.current_add_failures--;
        }
      }
    } 
  }
  else if (hapiBroadL3RouteCache.cmd == BROAD_L3_HW_CMD_DEL)
  {
    if (usl_bcmx_l3_route_delete(pBcmInfo, count, rv_array) == L7_SUCCESS)
    {
      for (i=0; i < count; i++)
      {
        rv = rv_array[i];
        HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_route_delete returned (%s)"
                  "egress Id %d", bcm_errmsg(rv), pBcmInfo[i].l3a_intf);
 
        HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwRouteStats, 2, rv_array[i]);
        /* Error handling for Delete - TBD */
      }
    }
    else
    {
      HAPI_BROAD_L3_BCMX_DBG(-1, "usl_bcmx_l3_route_delete: failed\n");
    }
  }
  else
  {
    /* This shouldn't be the case if numEntries > 0 */
    HAPI_BROAD_L3_L7_LOG_ERROR(hapiBroadL3RouteCache.cmd);
  }

  /* Clear up cache */
  hapiBroadL3RouteCacheClear();
}


/*******************************************************************************
*
* @purpose Delete/cache L3 route entry from hardware
*
* @param   pBcmxInfo     - BCMX data assoicated with the L3 route.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 route cache is commited to hardware,
*            - when route cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwRouteDelete (usl_bcm_l3_route_t *pBcmInfo,
                               L7_BOOL            cacheOrCommit)
{
  L7_uint32 cacheIdx = 0;

  hapiBroadL3RouteCacheCmdSet(BROAD_L3_HW_CMD_DEL);

  cacheIdx = hapiBroadL3RouteCache.numEntries;

  /* Note: For delete, the internal route struct is not available */
  hapiBroadL3RouteCache.pRouteList[cacheIdx] = L7_NULL;

  /* Cache the entry */
  memcpy (&hapiBroadL3RouteCache.pBcmInfo[cacheIdx],
          pBcmInfo, sizeof (usl_bcm_l3_route_t));

  hapiBroadL3RouteCache.numEntries++;

  if (maxRouteDelsInCache < hapiBroadL3RouteCache.numEntries)
    maxRouteDelsInCache = hapiBroadL3RouteCache.numEntries;

  if ((hapiBroadL3RouteCache.numEntries == hapiBroadL3RouteCache.maxEntries) ||
      (cacheOrCommit == BROAD_L3_ENTRY_COMMIT))
  {
    /* If we have max entries or caller has explicitly flagged for commit, 
     * commit the cache entries to hardware
     */
    hapiBroadL3RouteCacheCommit();
  }
}


/*******************************************************************************
*
* @purpose Clear the route cache
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*******************************************************************************/
static void hapiBroadL3RouteCacheClear (void)
{
  L7_uint32 size;

  hapiBroadL3RouteCache.numEntries = 0;
  hapiBroadL3RouteCache.cmd = BROAD_L3_HW_CMD_NO_OP;

  size = sizeof(usl_bcm_l3_route_t) * hapiBroadL3RouteCache.maxEntries;
  memset(hapiBroadL3RouteCache.pBcmInfo, 0, size);

  size = sizeof (BROAD_L3_ROUTE_ENTRY_t *) * hapiBroadL3RouteCache.maxEntries;
  memset(hapiBroadL3RouteCache.pRouteList, 0, size);
}


void hapiBroadL3CacheStats(void)
{
  sysapiPrintf ("\nMax Nhop objs adds ever cached %d\n", maxNhopAddsInCache);
  sysapiPrintf ("Max Nhop dels ever cached %d\n", maxNhopDelsInCache);
  sysapiPrintf ("Current Nhops cache cmd %d\n", hapiBroadL3NhopCache.cmd);
  sysapiPrintf ("Number of Nhops in cache %d\n", hapiBroadL3NhopCache.numEntries);
  sysapiPrintf ("Max Nhops that can be cached %d\n", hapiBroadL3NhopCache.maxEntries);

  sysapiPrintf ("\nMax host adds ever cached %d\n", maxHostAddsInCache);
  sysapiPrintf ("Max host dels ever cached %d\n", maxHostDelsInCache);
  sysapiPrintf ("Current host cache cmd %d\n", hapiBroadL3HostCache.cmd);
  sysapiPrintf ("Number of hosts in cache %d\n", hapiBroadL3HostCache.numEntries);
  sysapiPrintf ("Max hosts that can be cached %d\n", hapiBroadL3HostCache.maxEntries);

  sysapiPrintf ("\nMax route adds ever cached %d\n", maxRouteAddsInCache);
  sysapiPrintf ("Max route dels ever cached %d\n", maxRouteDelsInCache);
  sysapiPrintf ("Current route cache cmd %d\n", hapiBroadL3RouteCache.cmd);
  sysapiPrintf ("Number of routes in cache %d\n", hapiBroadL3RouteCache.numEntries);
  sysapiPrintf ("Max routes that can be cached %d\n", hapiBroadL3RouteCache.maxEntries);
}
