/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_mfc.h
*
* @purpose Contains structures,constants,macros, for dvmrp protocol
*
* @component
*
* @comments
*
* @create 07/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#ifndef _DVMRP_MFC_H_
#define _DVMRP_MFC_H_
#include <dvmrp_common.h>

#define DVMRP_CACHE_DEFAULT_LIFETIME 30

/*********************************************************************
* @purpose  This function is used to update the MFC about cache outgoing list
*
* @param     source  -  @b{(input)}Pointer to the source address of the cache
* @param     group   -  @b{(input)}Pointer to the group address of the cache
* @param     iif         -  @b{(input)}   Pointer to the incoming interface of the cache
* @param     oif        -  @b{(input)}  Pointer to the outgoing interface list
* 
* @returns  L7_SUCCESS or L7_FAILURE
*
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpCacheMfcUpdate (L7_inet_addr_t *source, 
                              L7_inet_addr_t *group,
                              dvmrp_interface_t *iif, L7_uchar8 *oif);


/*********************************************************************
* @purpose  This function is used to update the MFC that interface has 
*                        gone down
*
* @param       dvmrpInterface     - @b{(input)}Pointer to the interface that is going down
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpUpdateMfcInterfaceDown (dvmrp_interface_t *dvmrpInterface);

/*********************************************************************
* @purpose  This function is used to update the MFC about cache outgoing list
*
* @param     source -  @b{(input)}Pointer to the source address of the cache
* @param     group  -  @b{(input)}Pointer to the group address of the cache
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpCacheMfcDelete (L7_inet_addr_t *source, 
                              L7_inet_addr_t *group);

/*********************************************************************
* @purpose  This function is used to remove the cache entry
*
* @param     dvmrpcb    -  @b{(input)}Pointer to the  DVMRP control block
* @param     entry     -  @b{(input)} Pointer to the cache entry to be removed
* 
* @returns    L7_SUCCESS
* @returns    L7_FAILURE
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t dvmrpCacheRemove(dvmrp_t *gDvmrp, dvmrp_cache_entry_t *entry);

/*********************************************************************
* @purpose  This function is used to handle the MFC NO CACHE event 
*
* @param     dvmrpcb     -  @b{(input)}Pointer to the  DVMRP control block
* @param     cache_info -  @b{(input)} Pointer to the Info required to create the cache 
*                                      entry
* @returns    None
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpMfcNoCache(dvmrp_t *gDvmrp, dvmrp_src_grp_entry_t *cache_info);

/*********************************************************************
* @purpose    This function is to update the MFC
*
* @param      entry - @b{(input)}Pointer to the  cache entry
* 
* @returns    None
*
* @notes      None
* @end
*********************************************************************/
void dvmrp_cache_mfc_update (dvmrp_cache_entry_t *entry);

/*********************************************************************
* @purpose  This function is to lookup the cache
*
* @param     dvmrpcb    -  @b{(input)}Pointer to the  DVMRP control block
* @param     source  -  @b{(input)} Pointer to the source of the cache
* @param     group -    @b{(input)} Pointer to the group of the cache
* 
* @returns    cache entry
* 
* @notes   None
* @end
*********************************************************************/
dvmrp_cache_entry_t *dvmrp_cache_lookup (dvmrp_t *dvmrpcb, L7_inet_addr_t *source, 
                                         L7_inet_addr_t *group);

/*********************************************************************
* @purpose  This function is to update the cache table when the new 
*           interface is up
*           
* @param     interface - @b{(input)} Pointer to the interface which has become up
* 
* @returns    None
*
* @notes   None
* @end
*********************************************************************/
void dvmrp_cache_update_interface_up(dvmrp_interface_t *interface);

/*********************************************************************
* @purpose  This function is to update the cache table when the 
*           interface goes down
*           
* @param     interface - @b{(input)} Pointer to the Interface which has become down
* 
* @returns    None
*
* @notes   None
* @end
*********************************************************************/
void dvmrp_cache_update_interface_down(dvmrp_interface_t *interface);

#endif
