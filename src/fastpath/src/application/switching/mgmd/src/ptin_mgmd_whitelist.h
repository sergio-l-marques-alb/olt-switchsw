/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    14/01/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/
#ifndef _PTIN_MGMD_WHITELIST_H
#define _PTIN_MGMD_WHITELIST_H

#include "ptin_mgmd_defs.h"
#include "ptin_mgmd_inet_defs.h"


typedef struct
{
  uint32                serviceId;
  ptin_mgmd_inet_addr_t groupAddr;
  ptin_mgmd_inet_addr_t sourceAddr;
} mgmdPtinWhitelistDataKey_t;

typedef struct mgmdPTinWhitelistData_s
{
  mgmdPtinWhitelistDataKey_t  key;

  void                        *next;
} mgmdPTinWhitelistData_t;


/**
 * Create a new AVL tree to hold the white-list entries.
 * 
 * @return RC_t 
 *  
 * @note: Note that the max number of entries is given by PTIN_MGMD_MAX_WHITELIST 
 */
RC_t ptinMgmdWhitelistInit(void);

/**
 * Add a new channel to the white-list. 
 *  
 * @param serviceId  : Service ID
 * @param groupAddr  : Group Address
 * @param sourceAddr : Source Address
 * 
 * @return Pointer to inserted item. 
 */
mgmdPTinWhitelistData_t* ptinMgmdWhitelistAdd(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, ptin_mgmd_inet_addr_t *sourceAddr);

/**
 * Remove an existing channel from the white-list.
 *  
 * @param serviceId  : Service ID
 * @param groupAddr  : Group Address
 * @param sourceAddr : Source Address
 *  
 * @return RC_t [NOT_EXIST if not found]
 */
RC_t ptinMgmdWhitelistRemove(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, ptin_mgmd_inet_addr_t *sourceAddr);

/**
 * Search for the given channel in the white-list.
 *  
 * @param serviceId  : Service ID
 * @param groupAddr  : Group Address
 * @param sourceAddr : Source Address
 * @param flag       : Search flag [AVL_NEXT or AVL_EXACT]
 *  
 * @return Pointer to searched item or PTIN_NULLPTR if not found.
 */
mgmdPTinWhitelistData_t* ptinMgmdWhitelistSearch(uint32 serviceId, ptin_mgmd_inet_addr_t *groupAddr, ptin_mgmd_inet_addr_t *sourceAddr, uint32 flag);

/**
 * Dump the current white-list.
 *  
 * @return RC_t
 */
RC_t ptinMgmdWhitelistDump(void);

#endif //_PTIN_MGMD_WHITELIST_H
