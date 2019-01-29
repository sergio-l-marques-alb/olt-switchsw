/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_mac_db.h
*
* @purpose   dot1x Mac Address Database header file
*
* @component dot1x
*
* @comments  none
*
* @create    11/12/2007
*
* @author    SNK
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1X_MAC_DB_H
#define INCLUDE_DOT1X_MAC_DB_H

#include "l7sll_api.h"
#include "buff_api.h"

extern L7_RC_t dot1xMacAddrDataDestroy (L7_sll_member_t *ll_member);
extern L7_int32 dot1xMacAddrDataCmp(void *p, void *q, L7_uint32 key);
extern L7_RC_t dot1xMacAddrInfoDBInit(L7_uint32 nodeCount);
extern L7_RC_t dot1xMacAddrInfoDBDeInit(void);
extern L7_RC_t dot1xMacAddrInfoAdd(L7_enetMacAddr_t *mac_addr,L7_uint32 lIntIfNum);
extern L7_RC_t dot1xMacAddrInfoRemove(L7_enetMacAddr_t *mac_addr);
extern L7_RC_t dot1xMacAddrInfoFind(L7_enetMacAddr_t *mac_addr,L7_uint32 *lIntIfNum);
extern L7_RC_t dot1xMacAddrInfoFindNext(L7_enetMacAddr_t *mac_addr,L7_uint32 *lIntIfNum);

#endif /* INCLUDE_DOT1X_MAC_DB_H */
