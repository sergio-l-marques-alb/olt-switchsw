/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsm_mrt.c
*
* @purpose  (*,G) application specific implementation.
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
#include "buff_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimsmdefs.h"
#include "pimsmcache.h"

/******************************************************************************
* @purpose  compare kernel cache entries
*
* @param cacheEntry1   @b{(input)} pointer to cache entry struct
* @param cacheEntry2   @b{(input)} pointer to cache entry struct
* @param keySize       @b{(input)}  not used.     
*
* @returns   an integer less than, equal to, or greater than zero
*
* @comments      
*       
* @end
******************************************************************************/
static L7_int32 pimsmCacheCompare(L7_VOIDPTR cacheEntry1, 
                                        L7_VOIDPTR cacheEntry2, L7_uint32 keySize)
{
  pimsmCache_t * cachentPtr1, *cachentPtr2;
  L7_uint32   result;

  cachentPtr1 = (pimsmCache_t *)cacheEntry1;
  cachentPtr2 = (pimsmCache_t *)cacheEntry2;

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,   PIMSM_TRACE_INFO, "Source Addr1 :",
   &cachentPtr1->pimsmSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,   PIMSM_TRACE_INFO, "Group Addr1 :", 
   &cachentPtr1->pimsmGrpAddr);

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,   PIMSM_TRACE_INFO, "Source Addr2 :",
   &cachentPtr2->pimsmSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,   PIMSM_TRACE_INFO, "Group Addr2 :",
   &cachentPtr2->pimsmGrpAddr);

  result = L7_INET_ADDR_COMPARE(&cachentPtr1->pimsmGrpAddr,
                                &cachentPtr2->pimsmGrpAddr);
  if(result == 0)
  {
    return L7_INET_ADDR_COMPARE(&cachentPtr1->pimsmSrcAddr, 
                                &cachentPtr2->pimsmSrcAddr);
  }
  return result;
}
/******************************************************************************
* @purpose  Destroy the kernel cache entry 
*
* @param    cacheEntry  @b{(input)} pointer to cache entry struct
*
* @returns  L7_SUCCESS
*
* @comments      
*       
* @end
******************************************************************************/
static L7_RC_t pimsmCacheDestroy(L7_sll_member_t * cacheEntry)
{
  L7_uchar8 addrFamily = 0;

  addrFamily = L7_INET_GET_FAMILY (&(((pimsmCache_t*)cacheEntry)->pimsmSrcAddr));

  PIMSM_FREE (addrFamily, (void*) cacheEntry);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO, 
              "Freed Kernel Cache Node ");

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Initializes the Kernel Cache List
*
* @param    pimsmCb       @b{(input)}  control block  
* @param    cacheList     @b{(input)}  SLL pointer 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmCacheInit(pimsmCB_t * pimsmCb,  
                        L7_sll_t    * cacheList)
{

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO, 
              "Creating Kernel Cache Linked List ");
  
  if(pimsmCb == L7_NULLPTR 
     || cacheList == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }
  /* Create the Kernel Cache list */
  if(SLLCreate (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                L7_SLL_ASCEND_ORDER, 0,
                pimsmCacheCompare,
                pimsmCacheDestroy,
                cacheList) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR, 
                "Error: unable to create Kernel Cache Linked List ");
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO, "Exit ");
  return L7_SUCCESS;
}



/******************************************************************************
* @purpose  DeInitializes the Kernel Cache List
*
* @param    pimsmCb       @b{(input)}  control block 
* @param    cacheList     @b{(input)}  SLL pointer 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmCacheDeInit(pimsmCB_t * pimsmCb, 
                        L7_sll_t    * cacheList)
{
  L7_RC_t rc;
  L7_sll_t * pCacheList;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Enter");
  
  if(pimsmCb == L7_NULLPTR  
     || cacheList == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }
  pCacheList = cacheList;
  
  rc = SLLDestroy(L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCacheList);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_FATAL, 
                "Error: unable to Destroy Kernel Cache Linked List ");
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO, "Exit ");
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Adds a kernelel cache entry to the Kernel Cache List
*
* @param    pimsmCb       @b{(input)}  control block  
* @param    cacheList     @b{(input)}  SLL pointer 
* @param    pSrcAddr      @b{(input)}  source address
* @param    pGrpAddr      @b{(input)}  group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmCacheAdd(pimsmCB_t * pimsmCb, 
                            L7_sll_t    * cacheList,
                            L7_inet_addr_t      * pSrcAddr,
                            L7_inet_addr_t      * pGrpAddr)
{
  pimsmCache_t  * cache;
  L7_RC_t rc;

  if(pimsmCb == L7_NULLPTR  
     || cacheList == L7_NULLPTR || pSrcAddr == L7_NULLPTR
     || pGrpAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }
  

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Enter");
  rc = pimsmCacheFind(pimsmCb,  cacheList,pSrcAddr, pGrpAddr,
                                &cache);

  if(rc == L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_NORMAL, 
                "Node Already exists");
    return L7_SUCCESS;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_NORMAL, 
              "Node is not found, add the new node");

  if ((cache = PIMSM_ALLOC (pimsmCb->family, sizeof (pimsmCache_t)))
                         == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                "Unable to allocate buffer from cache");
    return L7_FAILURE;
  }
  
  if(cache == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,
                "PIMSM: Memory allocation failed for kernel cache entry\n");
    return L7_FAILURE;
  }
  memset(cache, 0, sizeof(pimsmCache_t));
  inetCopy(&cache->pimsmGrpAddr, pGrpAddr);
  inetCopy(&cache->pimsmSrcAddr, pSrcAddr);
  cache->pimsmCacheBufferId = pimsmCb->pimsmCacheBufferId;

  rc = SLLAdd (cacheList, 
                   (void *)cache);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_FATAL, 
                "Error: unable to add to Kernel Cache Linked List ");
    PIMSM_FREE (pimsmCb->family, (void*) cache);
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO, "Exit ");
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Deletes a kernelel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    cacheList         @b{(input)}  SLL pointer 
* @param    cacheEntry  @b{(input)} cache entry struct
* @param    pFlags            @b{(input)}  (*,G) or (*,*,RP) flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmCacheDelete(pimsmCB_t * pimsmCb,  
                            L7_sll_t    * cacheList,
                            pimsmCache_t  * cacheEntry,
                            L7_ushort16    *pFlags  )
{
  L7_sll_t * pCacheList;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Enter");
  if(pimsmCb == L7_NULLPTR 
     || cacheList == L7_NULLPTR || pFlags == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }
  
  pCacheList = cacheList;

  rc = SLLDelete(pCacheList, (L7_sll_member_t *)cacheEntry);

  if(SLLNumMembersGet(pCacheList) == 0)
  {
    *pFlags &= ~PIMSM_ADDED_TO_MFC;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO, "Exit ");     
  return rc;
}


/******************************************************************************
* @purpose  Return the first kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block
* @param    cacheList         @b{(input)}  SLL pointer 
* @param    cacheEntry  @b{(input)}  pointer to cache entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmCacheGetFirst(pimsmCB_t * pimsmCb, 
                            L7_sll_t    * cacheList,
                                 pimsmCache_t  ** cacheEntry)
{
  L7_sll_t * pCacheList;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG,"Enter");

  
  if(pimsmCb == L7_NULLPTR 
     || cacheList == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }
  pCacheList = cacheList;

  *cacheEntry = (pimsmCache_t *)SLLFirstGet(pCacheList);

  if(*cacheEntry == (pimsmCache_t *)L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Exit ");         
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Returns the next kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb                @b{(input)}  control block 
* @param    cacheList              @b{(input)}  SLL pointer 
* @param    currentCacheEntry  @b{(input)} current cache entry
* @param    nextCacheEntry     @b{(input)} pointer current cache entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmCacheNextGet(pimsmCB_t * pimsmCb,
                            L7_sll_t    * cacheList, 
                                pimsmCache_t  * currentCacheEntry,
                                pimsmCache_t  ** nextCacheEntry)
{
  L7_sll_t * pCacheList;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG,"Enter"); 

  if(pimsmCb == L7_NULLPTR 
     || cacheList == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }
  
  pCacheList = cacheList;
  

  *nextCacheEntry = (pimsmCache_t *)SLLNextGet(pCacheList,
                         (L7_sll_member_t *)currentCacheEntry);

  if(*nextCacheEntry == (pimsmCache_t *)L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Exit ");      
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Find and return the kernel cache entry from the Kernel Cache List
*
* @param    pimsmCb           @b{(input)}  control block  
* @param    cacheList         @b{(input)}  SLL pointer 
* @param    pSrcAddr          @b{(input)}  source address
* @param    pGrpAddr          @b{(input)}  group address
* @param    cacheEntry  @b{(input)}  pointer to cache entry struct
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmCacheFind(pimsmCB_t * pimsmCb, 
                            L7_sll_t    * cacheList,
                             L7_inet_addr_t    *pSrcAddr,
                             L7_inet_addr_t      *pGrpAddr,
                             pimsmCache_t  ** cacheEntry)
{
  L7_sll_t * pCacheList;

  pimsmCache_t cacheEntryTemp;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Enter");  

  if(pimsmCb == L7_NULLPTR  
     || cacheList == L7_NULLPTR || pSrcAddr == L7_NULLPTR
     || pGrpAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO,"Invalid input parameters");
    return L7_FAILURE;
  }
  
  pCacheList = cacheList;
  
  inetCopy(&cacheEntryTemp.pimsmSrcAddr,pSrcAddr);
  inetCopy(&cacheEntryTemp.pimsmGrpAddr,pGrpAddr); 


  *cacheEntry = (pimsmCache_t *)SLLFind(pCacheList,
                           (L7_sll_member_t *)&cacheEntryTemp);

  if(*cacheEntry == (pimsmCache_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO, "Entry Not found ");          
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_INFO, "Found - Exit ");       
  return L7_SUCCESS;
}


