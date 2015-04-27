/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvmrp_cache_table.h
*
* @purpose   This file contains prototypes for operations on the cache table 
*
* @component DVMRP CACHE TABLE API's (AVL Trees)
*
* @comments  Access to elements in the list is protected by a semaphore.
*            Functions that return values from the list always return a copy
*            of the data in the list.
*
* @create    10/02/2006
*
* @author    shashidhar Dasoju
*
* @end
*
**********************************************************************/

#ifndef _INCLUDE_DVMRP_CACHE_TABLE_H_
#define _INCLUDE_DVMRP_CACHE_TABLE_H_
#include "dvmrp_common.h"


#define DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES  L7_DVMRP_MAX_MRT_IPV4_TABLE_SIZE

/* Function Prototypes. */

/*****************************************************************
* @purpose  Compare IP Network Address (source and group) keys and
*           indicate relationship
*
* @param    a     -  @b{(input)}      Pointer to the first key
* @param    b     -  @b{(input)}      Pointer to the second key
* @param    len   -  @b{(input)}      Length of the key (unused)
*  
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes   At this time, these are compared the same way they would
*          be if they were compared with memcmp() on a big-endian platform.
*          It might be more useful to compare them by some other criterion
*          (shorter masks first, for example).
*
* @end
*****************************************************************/
L7_int32 dvmrpAVLCacheCompare(const void *a, const void *b, L7_uint32 len);


/*********************************************************************
* @purpose  Initializes the cache table
*
* @param    dvmrpcb  -  @b{(input)}  Pointer to the DVMRP control block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableInit(dvmrp_t *dvmrpcb);

/*********************************************************************
* @purpose  De-Initializes the cache table
*
* @param    dvmrpcb  -  @b{(input)}  Pointer to the DVMRP control block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableDestroy(dvmrp_t *dvmrpcb);

/*********************************************************************
* @purpose  Add a cache to the cache table
*
* @param    dvmrpcb    -  @b{(input)} Pointer to the DVMRP control block.
* @param    cacheData   -  @b{(input)} Pointer to cache information to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableCacheAdd(dvmrp_t *dvmrpcb, dvmrp_cache_entry_t *cacheData);

/*********************************************************************
* @purpose  Delete a cache from the cache table
*
* @param    dvmrpcb  -  @b{(input)}  Pointer to the DVMRP control block.
* @param    pSource   -  @b{(input)} pointer to Source of the multicast data
* @param    pGroup    -  @b{(input)} pointer to Multicast group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes None
*
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableCacheDelete(dvmrp_t *dvmrpcb, L7_inet_addr_t *pSource, 
                                   L7_inet_addr_t *pGroup);


/*********************************************************************
* @purpose  Check whether the cache exists in the cache table
*
* @param    dvmrpcb    -  @b{(input)}  Pointer to the DVMRP control block.
* @param    srcRtData   -  @b{(input)}   Route info to look for
* @param    routeData  -  @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableCacheGet(dvmrp_t *dvmrpcb,dvmrp_cache_entry_t *srcRtData,
                                dvmrp_cache_entry_t **cacheData);

/*********************************************************************
* @purpose  Get the next cache after the specified cache in the cache table
*
* @param    dvmrpcb    -  @b{(input)}  Pointer to the DVMRP control block.
* @param    srcRtData   -  @b{(input)} Pointer to the Cache info to look for
* @param    cacheData  -  @b{(output)} Pointer to the next Cache info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*
* @end
*********************************************************************/
L7_RC_t dvmrpCacheTableCacheNextGet(dvmrp_t *dvmrpcb,dvmrp_cache_entry_t *srcRtData,
                                    dvmrp_cache_entry_t **cacheData);


/*********************************************************************
* @purpose  This function is used to flush the dvmrp cache table.
* 
* @param    dvmrpcb -@b{(input)}  Pointer to the DVMRP control block.
* 
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_cache_table_flush(dvmrp_t *dvmrpcb);


#endif /* _INCLUDE_DVMRP_CACHE_TABLE_H_ */

