/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsm_mrt.h
*
* @purpose  (*,G) implementation specifices.
*
* @component 
*
* @comments 
*
* @create 01/01/2006
*
* @author vmurali/ dsatyanarayana
* @end
*
******************************************************************************/
#ifndef _PIMSM_CACHE_H_
#define _PIMSM_CACHE_H_
#include "l7sll_api.h"
#include "l3_addrdefs.h"

typedef struct pimsmCache_s 
{
  struct pimsmCache_s  *next;
  L7_inet_addr_t   pimsmSrcAddr;
  L7_inet_addr_t   pimsmGrpAddr;
  L7_uint32       pimsmSGRealtimeByteCount; /* Will be used to keep track of byte count for
                                      register packets. */
  L7_uint32       pimsmSGRealtimePktcnt;  /* Will be used to keep track of pkt count for
                                      register packets. */
  L7_uint32   pimsmSGByteCount; /* The (s,g) h/w data related counters (see above) */
  L7_uint32 pimsmCacheBufferId;
} pimsmCache_t;


extern L7_RC_t pimsmCacheInit(pimsmCB_t * pimsmCb,
                        L7_sll_t    * cacheList);
extern L7_RC_t pimsmCacheDeInit(pimsmCB_t * pimsmCb, 
                        L7_sll_t    * cacheList);
extern L7_RC_t pimsmCacheFind(pimsmCB_t * pimsmCb,
                            L7_sll_t    * cacheList,
                             L7_inet_addr_t    *pSrcAddr,
                             L7_inet_addr_t      *pGrpAddr,
                             pimsmCache_t  ** cacheEntry);
extern L7_RC_t pimsmCacheAdd(pimsmCB_t * pimsmCb,
                            L7_sll_t    * cacheList,
                            L7_inet_addr_t      * pSrcAddr,
                            L7_inet_addr_t      * pGrpAddr);
extern L7_RC_t pimsmCacheDelete(pimsmCB_t * pimsmCb, 
                            L7_sll_t    * cacheList,
                            pimsmCache_t  * cacheEntry,
                            L7_ushort16    *pFlags  );
extern L7_RC_t pimsmCacheGetFirst(pimsmCB_t * pimsmCb, 
                            L7_sll_t    * cacheList,
                                 pimsmCache_t  ** cacheEntry);
extern L7_RC_t pimsmCacheNextGet(pimsmCB_t * pimsmCb, 
                            L7_sll_t    * cacheList, 
                                pimsmCache_t  * currentCacheEntry,
                                pimsmCache_t  ** nextCacheEntry);
#endif /* _PIMSM_CACHE_H_ */


