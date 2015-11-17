/*
 * $Id: trill_pb.c,v 1.89 Broadcom SDK $
 *
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * Soc_petra-B Layer 2 Management
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TRILL
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/gport_mgmt.h> 
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/switch.h>

#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>

int
_bcm_dpp_mc_trill_get_src_list(
    int                                     unit,
    bcm_trill_name_t                        root_name, 
    _bcm_petra_trill_mc_trill_src_list_t    **src_list)
{
    int  rv = BCM_E_NONE;

   
    BCMDNX_INIT_FUNC_DEFS;
    rv =  _bcm_dpp_sw_db_hash_trill_root_src_find(unit,
                                                 &(root_name),
                                                 (shr_htb_data_t*)(void *)src_list,
                                                 FALSE); 
    if ((rv == BCM_E_NONE) ||  (rv == BCM_E_NOT_FOUND) ){
        BCM_RETURN_VAL_EXIT(rv);
    }       
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("bcm_dpp_mc_trill_get_src_list")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_mc_trill_get_route_list(
    int                                     unit,
    bcm_trill_name_t                        root_name, 
    _bcm_petra_trill_mc_trill_route_list_t  **route_list)
{
    int  rv = BCM_E_NONE;

   
    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_trill_route_info_find(unit,
                                                   &(root_name),
                                                   (shr_htb_data_t*)(void *)route_list,
                                                   FALSE);  
    if ((rv == BCM_E_NONE) ||  (rv == BCM_E_NOT_FOUND) ){
        BCM_RETURN_VAL_EXIT(rv);
    }
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("_bcm_dpp_mc_trill_get_route_list")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_mc_trill_get_route_info(
    int                                     unit,
    uint32                                  flags, 
    bcm_trill_name_t                        root_name, 
    bcm_vlan_t                             vlan, 
    bcm_mac_t                               c_dmac, 
    bcm_multicast_t                         group,
    _bcm_petra_trill_mc_trill_route_list_t  **route_list,
    int                                     *route_ndx)
{
    int                                     rv = BCM_E_NONE;
    _bcm_petra_trill_mc_trill_route_list_t  *rlist;
    int                                     i;

    BCMDNX_INIT_FUNC_DEFS;
    *route_list = NULL;
    *route_ndx = -1;

    rv = _bcm_dpp_mc_trill_get_route_list(unit, root_name, &rlist);
    BCMDNX_IF_ERR_EXIT(rv);
    for (i = 0; i < rlist->nof_routes; i++) {
        if ((rlist->route[i].flags == flags)
            &&
            (rlist->route[i].vlan == vlan)
            &&
            (rlist->route[i].group == group))
         {
            *route_list =  rlist;
            *route_ndx = i;
            BCM_EXIT;
         }
    }
    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("route not found")));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_pb_trill_multicast_source_get(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t *port) 
{
    int                                     rv = BCM_E_NONE;
    _bcm_petra_trill_mc_trill_src_list_t    *src_list;
    int                                     i;

    BCMDNX_INIT_FUNC_DEFS;
    
    rv = _bcm_dpp_mc_trill_get_src_list(unit, root_name, &src_list);
    if (rv == BCM_E_NONE ){  
        for (i = 0; i < src_list->nof_src_rbridge; i++) {
            if (src_list->src_rbridge[i].src_rbridge == source_rbridge_name) {
                *port = src_list->src_rbridge[i].port;
                BCM_EXIT;
            }
        }
    }
    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Port haven\'t been found")));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_pb_trill_multicast_source_delete(
    int unit,
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_name, 
    bcm_gport_t gport,
    bcm_if_t    intf)
{

    int                                     rv = BCM_E_NONE;
    _bcm_petra_trill_mc_trill_src_list_t    *src_list;
    _bcm_petra_trill_mc_route_info_t        *route_info;
    _bcm_petra_trill_mc_trill_route_list_t  *route_list;
    int                                     i;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* For Soc_petra remove all the routes added for this source.
       */

    rv = _bcm_dpp_mc_trill_get_src_list(unit, root_name, &src_list);
    if (rv == BCM_E_NONE ){  
        for (i = 0; i < src_list->nof_src_rbridge; i++) {
            if (src_list->src_rbridge[i].src_rbridge == source_name) {
                sal_memcpy(&(src_list->src_rbridge[i]), &(src_list->src_rbridge[i+1]),
                         (src_list->nof_src_rbridge - i-1)*sizeof(struct _bcm_petra_trill_mc_trill_src_list_s));
            }
        }
    }

    rv = _bcm_dpp_mc_trill_get_route_list(unit, root_name, &route_list);    
    if (rv == BCM_E_NOT_FOUND ) {
        BCM_EXIT;
    }
    if (rv == BCM_E_NONE) {
        for (i=0; i < route_list->nof_routes;i++ ) {
            route_info = &(route_list->route[i]);

            rv = _ppd_trill_multicast_route_remove(unit, 0, root_name, route_info->vlan, 0, route_info->group, source_name);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    BCM_EXIT;

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_pb_trill_multicast_source_add(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t port,
    bcm_if_t intf) 
{
    int                                     rv = BCM_E_NONE;
    _bcm_petra_trill_mc_trill_src_list_t    *src_list = NULL;
    _bcm_petra_trill_mc_route_info_t        *route_info;
    _bcm_petra_trill_mc_trill_route_list_t  *route_list;
    int                                     i;

    BCMDNX_INIT_FUNC_DEFS;
   
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* For Soc_petra the route is added when the sources are defined.
       The routes are stored in route list, and the sources in the src_list,
       when a new route or source is added  the find the route list for this root name */
    rv = _bcm_dpp_mc_trill_get_src_list(unit, root_name, &src_list);
    if (rv == BCM_E_NOT_FOUND ) {
        BCMDNX_ALLOC(src_list, sizeof(_bcm_petra_trill_mc_trill_src_list_t), "trill_mc_src_list");
        if (src_list == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        src_list->nof_src_rbridge = 0;

        rv = _bcm_dpp_sw_db_hash_trill_root_src_insert(unit,
                                                       &(root_name),
                                                       src_list);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG( "error(%s) adding route info (0x%x)\n"),
                  bcm_errmsg(rv), root_name));
        }
    }
    src_list->src_rbridge[src_list->nof_src_rbridge].src_rbridge = source_rbridge_name;
    src_list->src_rbridge[src_list->nof_src_rbridge].port = port;
    src_list->nof_src_rbridge++;

    rv = _bcm_dpp_mc_trill_get_route_list(unit, root_name, &route_list);    
    if (rv == BCM_E_NOT_FOUND ) {
        BCM_EXIT;
    }
    if (rv == BCM_E_NONE) {
        for (i=0; i < route_list->nof_routes;i++ ) {
            route_info = &(route_list->route[i]);
            rv = _ppd_trill_multicast_route_add(unit, route_info->flags, root_name, route_info->vlan,BCM_VLAN_INVALID, route_info->group, source_rbridge_name, port);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int _bcm_pb_trill_multicast_entry_get(int unit, bcm_trill_multicast_entry_t *trill_mc)
{
    _bcm_petra_trill_mc_trill_src_list_t   *src_list;
    int                                    rv = BCM_E_NONE;
    int                                    i;
    uint8                                  found = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* For Soc_petra the route is added when the sources are defined.
       The routes are stored in route list, and the sources in the src_list,
       when a new route or source is added  the find the route list for this root name */
    rv = _bcm_dpp_mc_trill_get_src_list(unit, trill_mc->root_name, &src_list);

    if (rv == BCM_E_NOT_FOUND ) {
        rv = _ppd_trill_multicast_route_get(unit, 
                       trill_mc->flags,
                       trill_mc->root_name,
                       trill_mc->c_vlan,
                       trill_mc->c_vlan_inner,
                       0,
                       0,
                       &(trill_mc->group),
                       &found);
        BCMDNX_IF_ERR_EXIT(rv);
       
    } else {
        for (i = 0; i < src_list->nof_src_rbridge; i++) {
            rv = _ppd_trill_multicast_route_get(unit, 
                           trill_mc->flags,
                           trill_mc->root_name, 
                           trill_mc->c_vlan,   
                           trill_mc->c_vlan_inner,
                           src_list->src_rbridge[i].src_rbridge,
                           src_list->src_rbridge[i].port,
                           &(trill_mc->group),
                           &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (found) {
                BCM_EXIT;
            }
        }
    }
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}
int _bcm_pb_trill_multicast_entry_add(int unit, bcm_trill_multicast_entry_t *trill_mc) 
{

    SOC_PPD_TRILL_MC_ROUTE_KEY                 trill_mc_key;
    _bcm_petra_trill_mc_trill_src_list_t   *src_list;
    _bcm_petra_trill_mc_trill_route_list_t *route_list = NULL;
    int                                    rv = BCM_E_NONE;
    int                                    ndx = -1, i;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    SOC_PPD_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);

    /* For Soc_petra the route is added when the sources are defined.
       The routes are stored in route list, and the sources in the src_list,
       when a new route or source is added  the find the route list for this root name */

    rv = _bcm_dpp_mc_trill_get_route_list(unit, trill_mc->root_name, &route_list);
    if (rv == BCM_E_NOT_FOUND ) {
        /* No routes for this root name*/
        BCMDNX_ALLOC(route_list, sizeof(_bcm_petra_trill_mc_trill_route_list_t), "trill_mc_route_list");
        if (route_list == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        route_list->nof_routes = 0;

        
        rv = _bcm_dpp_sw_db_hash_trill_route_info_insert(unit,
                                                         &(trill_mc->root_name),
                                                         route_list);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) adding route info (0x%x)\n"),
                  bcm_errmsg(rv), trill_mc->root_name));
        }
        
        ndx = route_list->nof_routes;

    } else if (rv == BCM_E_NONE ){
        ndx = route_list->nof_routes;
        for (i = 0; i < route_list->nof_routes; i++) {
            if ((route_list->route[i].flags == trill_mc->flags)
                &&
                (route_list->route[i].vlan == trill_mc->c_vlan)
               )
            {
                /* route already exists - Do nothing*/
                if ((route_list->route[i].group == trill_mc->group)) {
                       BCM_EXIT;
                }
                ndx = i;
                /* Remove old routes */
                rv = _bcm_dpp_mc_trill_get_src_list(unit, trill_mc->root_name, &src_list);
                if (rv == BCM_E_NONE ){  
                    for (i = 0; i < src_list->nof_src_rbridge; i++) {
                        rv = _ppd_trill_multicast_route_remove(unit,
                                       0,
                                       trill_mc->root_name, 
                                       trill_mc->c_vlan, 
                                       0,
                                       trill_mc->group, 
                                       src_list->src_rbridge[i].src_rbridge);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
                break;
            }
        }
    } else {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    route_list->route[ndx].flags = trill_mc->flags;
    route_list->route[ndx].root_name = trill_mc->root_name;
    route_list->route[ndx].vlan = trill_mc->c_vlan;
    route_list->route[ndx].group = trill_mc->group;
    route_list->nof_routes++;
           
    rv = _bcm_dpp_mc_trill_get_src_list(unit, trill_mc->root_name, &src_list);
    if (rv == BCM_E_NOT_FOUND ) {

        /* Add  without source*/
        rv =  _ppd_trill_multicast_route_add(unit, trill_mc->flags, 
                                             trill_mc->root_name, 
                                             trill_mc->c_vlan, 
                                             BCM_VLAN_INVALID,
                                             trill_mc->group, 0,0);
       
    } else {
        for (i = 0; i < src_list->nof_src_rbridge; i++) {
            rv = _ppd_trill_multicast_route_add(unit, trill_mc->flags, 
                                        trill_mc->root_name, 
                                        trill_mc->c_vlan, BCM_VLAN_INVALID, trill_mc->group,
                                        src_list->src_rbridge[i].src_rbridge, 
                                        src_list->src_rbridge[i].port);
            BCMDNX_IF_ERR_EXIT(rv);

          
        }
    }
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int _bcm_pb_trill_multicast_entry_delete(int unit, bcm_trill_multicast_entry_t *trill_mc)
{
     _bcm_petra_trill_mc_trill_src_list_t   *src_list;
     _bcm_petra_trill_mc_trill_route_list_t *route_list;
     int                                    route_ndx;
     int                                    rv = BCM_E_NONE;
     int                                    i;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* For Soc_petra remove the route for each defined source. */
    rv = _bcm_dpp_mc_trill_get_route_info(unit, trill_mc->flags, 
                                          trill_mc->root_name, trill_mc->c_vlan, 
                                          trill_mc->c_dmac, trill_mc->group, 
                                          &route_list, &route_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_mc_trill_get_src_list(unit, trill_mc->root_name, &src_list);
    if (rv == BCM_E_NONE ){  
        for (i = 0; i < src_list->nof_src_rbridge; i++) {

            rv = _ppd_trill_multicast_route_remove(unit, 0, trill_mc->root_name, 
                                       trill_mc->c_vlan, 
                                       0,
                                       trill_mc->group, 
                                       src_list->src_rbridge[i].src_rbridge);
            BCMDNX_IF_ERR_EXIT(rv);
           
        }
    }
    sal_memcpy(&(route_list->route[route_ndx]), 
               &(route_list->route[route_ndx+1]),
               ((route_list->nof_routes - route_ndx-1) * 
                sizeof(struct _bcm_petra_trill_mc_route_info_s)));

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}
