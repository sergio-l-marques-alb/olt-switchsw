/*
 * $Id: mpls.c,v 1.43 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * FE2000 MPLS OAM module
 */

#if defined(INCLUDE_L3)

#include <shared/bsl.h>

#include <bcm_int/sbx/fe2000/oam/oam.h>
#include <bcm_int/sbx/fe2000/oam/mpls.h>
#include <bcm_int/sbx/fe2000/oam/bfd.h>
#include <bcm_int/sbx/fe2000/oam/pm.h>
#include <bcm_int/sbx/fe2000/oam/psc.h>
#include <bcm/stack.h>

extern oam_state_t* _state[SOC_MAX_NUM_DEVICES];

#define SET_LAST_ERRNO(var,errno)                       \
             do {                                       \
               var = ((var == BCM_E_NONE) ? errno:var); \
             } while(0)


int
_bcm_fe2000_get_tunnel_interface_info(int unit, 
                                      bcm_if_t l3_egr_ifid,
                                      bcm_fe2000_mpls_tunnel_direction_t direction)
{
    int status = BCM_E_NONE;

    if(direction < _FE2K_OUTGOING_LSP || direction  >= _FE2K_MAX_DIRECTION) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Bad Input Parameter\n")));        
    } else {
        if (_FE2K_OUTGOING_LSP == direction) {
            /* Find Associated L3 interface to get SMAC information */
            status = bcm_l3_egress_get(unit, l3_egr_ifid,
                                       &OAM_MPLS_LSP_OUT_EGR_IF(unit));
            if(BCM_FAILURE(status)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to Get Out LSP Egress Interface [0x%x] Info: %d %s\n"),
                           l3_egr_ifid, status, bcm_errmsg(status)));

            } else {
                bcm_l3_intf_t_init(&OAM_MPLS_LSP_OUT_IF(unit));
                OAM_MPLS_LSP_OUT_IF(unit).l3a_flags   = BCM_L3_WITH_ID;
                OAM_MPLS_LSP_OUT_IF(unit).l3a_intf_id = OAM_MPLS_LSP_OUT_EGR_IF(unit).intf;

                status = bcm_l3_intf_get(unit, &OAM_MPLS_LSP_OUT_IF(unit));
                if(BCM_FAILURE(status)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to Get Out LSP Tunnel Interface [0x%x] Info: %d %s\n"),
                               OAM_MPLS_LSP_OUT_EGR_IF(unit).intf, status, bcm_errmsg(status)));
                    
                } 
            }
        } else {
            status = bcm_l3_egress_get(unit, l3_egr_ifid,
                                       &OAM_MPLS_LSP_IN_EGR_IF(unit));
            if(BCM_FAILURE(status)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to Get Incoming LSP Egress Interface [0x%x] Info: %d %s\n"),
                           l3_egr_ifid, status, bcm_errmsg(status)));
                
            } else {
                bcm_l3_intf_t_init(&OAM_MPLS_LSP_IN_IF(unit));
                OAM_MPLS_LSP_IN_IF(unit).l3a_flags   = BCM_L3_WITH_ID;
                OAM_MPLS_LSP_IN_IF(unit).l3a_intf_id = OAM_MPLS_LSP_IN_EGR_IF(unit).intf;
                
                status = bcm_l3_intf_get(unit, &OAM_MPLS_LSP_IN_IF(unit));
                if(BCM_FAILURE(status)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to Get Out LSP Tunnel Interface [0x%x] Info: %d %s\n"),
                               OAM_MPLS_LSP_IN_EGR_IF(unit).intf, status, bcm_errmsg(status)));
                    
                } 
            }
        }
    }
    return status;
}

int
_bcm_fe2000_lsp_intf_ep_list_add(int unit, 
                                 bcm_if_t l3_egr_ifid,
                                 oam_sw_hash_data_t *hash_data)
{
    bcm_l3_egress_t l3_egif;
    int status = BCM_E_NONE;

    if (!hash_data) {
        status = BCM_E_PARAM;
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Bad input parameter")));
    } else {
        status = bcm_l3_egress_get(unit, l3_egr_ifid, &l3_egif);
        if(BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to Get  LSP Egress Interface [0x%x] Info: %d %s\n"),
                       l3_egr_ifid, status, bcm_errmsg(status)));
            
        } else {
            status = _bcm_fe2000_l3_oam_endpoint_associate(unit, l3_egif.intf, 
                                                           &hash_data->lsp_list_node,
                                                           1);        
        }
    }

    return status;
}                                          

int
_bcm_fe2000_lsp_intf_ep_list_remove(int unit, 
                                    bcm_if_t l3_egr_ifid,
                                    oam_sw_hash_data_t *hash_data)
{
    bcm_l3_egress_t l3_egif;
    int status = BCM_E_NONE;

    if (!hash_data) {
        status = BCM_E_PARAM;
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Bad input parameter")));
    } else {
        status = bcm_l3_egress_get(unit, l3_egr_ifid, &l3_egif);
        if(BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to Get  LSP Egress Interface [0x%x] Info: %d %s\n"),
                       l3_egr_ifid, status, bcm_errmsg(status)));
            
        } else {
            status = _bcm_fe2000_l3_oam_endpoint_associate(unit, l3_egif.intf, 
                                                           &hash_data->lsp_list_node, 
                                                           0);        
        }
    }

    return status;
}                                          

int _bcm_fe2000_validate_oam_mpls_endpoint(int unit, 
                                           bcm_oam_endpoint_info_t *endpoint_info)
{
    int status = BCM_E_PARAM;

    if(!endpoint_info) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Null endpoint info\n")));

    } else if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Unsupported Endpoing Flags: INTERMEDIATE\n")));

    } else if (endpoint_info->type & ~_BCM_FE2000_MPLS_SUPPORTED_EPTYPE) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Unsupported Endpoint Type: %d\n"),
                   endpoint_info->type));

    } else {
        status = BCM_E_NONE;
    }

    /* Currently OAM is supported on MPLS LSP. Application has to specify
     * the Tunnel Interface on which the end point has to be associated to.
     * Validate Interface information to make sure tunnel is created right and
     * obtain all Interface Information for setting up OAM encapsulation */
    if(BCM_SUCCESS(status)) {
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            /* verify if the label2e is provisioned for peer match label to POP it */
            soc_sbx_g2p3_label2e_t  label2e;
            
            status = _bcm_fe2000_get_tunnel_interface_info(unit, endpoint_info->intf_id, _FE2K_INCOMING_LSP);
            if(BCM_SUCCESS(status)) {

                status = soc_sbx_g2p3_label2e_get(unit, 
                                                  OAM_MPLS_LSP_IN_EGR_IF(unit).mpls_label,
                                                  &label2e);
                if (BCM_FAILURE(status)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Match Label [0x%x] not Provisioned %d %s\n"),
                               OAM_MPLS_LSP_IN_EGR_IF(unit).mpls_label,
                               status, bcm_errmsg(status)));         
                    
                } else {
                    /* verify if this local endpoint exist on the group */
                    if (OAM_GROUP_EP_LIST_EMPTY(unit, endpoint_info->group)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Group has no Endpoints associated. "
                                               "Local Endpoint must exist before creating"
                                               " remote endpointID\n")));
                        return BCM_E_PARAM;
                    } else {
                        dq_p_t ep_elem;
                        
                        DQ_TRAVERSE(&OAM_GROUP_EP_LIST(unit, endpoint_info->group), ep_elem) {
                            oam_sw_hash_data_t *hash_data = NULL;
                            _BCM_FE2000_OAM_GET_EP_DATA_FROM_LIST(ep_elem, hash_data);
                            
                            if(hash_data->rec_idx != endpoint_info->local_id) {
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Local Endpoint does not match with one on group\n")));
                                status = BCM_E_PARAM;
                                break;
                            }
                        } DQ_TRAVERSE_END(&trunk_data->endpoint_list, ep_elem);
                    }
                }
            }
        } else {
            status = _bcm_fe2000_get_tunnel_interface_info(unit, endpoint_info->intf_id, _FE2K_OUTGOING_LSP);
        }
    }
        
    return status;
}

int _oam_mpls_egr_path_update (int unit,
                               egr_path_desc_t *egrPath,  
                               bcm_oam_endpoint_info_t *endpoint_info)
{
    int status = BCM_E_NONE;
    bcm_module_t mod = -1;
    bcm_port_t   port = -1;
    int index = 0;
    int tid = 0;
    bcm_trunk_add_info_t trunk_info;

    if ((!egrPath) || (!endpoint_info) || 
           ((!OAM_IS_MPLS_Y1731(endpoint_info->type)) &&
#if defined(INCLUDE_SBX_BFD)
            (!OAM_IS_LSP_BFD(endpoint_info->type)) &&
#endif
            (!OAM_IS_PSC(endpoint_info->type)))) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Invalid Param:  egrPath %p, ep_info %p\n"),
                   (void *)egrPath, (void *)endpoint_info));
        status = BCM_E_PARAM;
    } else {

        status = _oam_egr_path_dmac_set(unit, 
                                        egrPath, 
                                        OAM_MPLS_LSP_OUT_EGR_IF(unit).mac_addr,
                                        TRUE);
         if (BCM_FAILURE(status)) {
             LOG_ERROR(BSL_LS_BCM_OAM,
                       (BSL_META_U(unit,
                                   "Failed to set dmac on egress path for EP %d\n"),
                        endpoint_info->id));

         } else {
             status = _oam_egr_path_smac_set(unit, 
                                             egrPath,
                                             OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr);
             if (BCM_FAILURE(status)) {
                 LOG_ERROR(BSL_LS_BCM_OAM,
                           (BSL_META_U(unit,
                                       "Failed to set smac on egress path for EP %d\n"),
                            endpoint_info->id));

             } else {
                 if (OAM_IS_MPLS_Y1731(endpoint_info->type) || 
#if defined(INCLUDE_SBX_BFD)
                     OAM_IS_LSP_BFD(endpoint_info->type) ||
#endif
                     OAM_IS_PSC(endpoint_info->type)) {

                   /* Clear overlay (or union) fields in the ETE. */
                    egrPath->eteEncap.isid        = 0;
                    egrPath->eteEncap.mimtype     = 0;

                    egrPath->eteEncap.s2          = 0;
                    egrPath->eteEncap.exp2        = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_exp & 0x7);
                    egrPath->eteEncap.label2      = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_label & 0xfffff);
                    egrPath->eteEncap.ttl2        = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_ttl & 0xff);
                    egrPath->eteEncap.remark      = (endpoint_info->egr_map & 0x7f);
                    egrPath->eteEncap.exp2remark  = 1;
                    egrPath->eteEncap.ttl2dec     = 0; /* use ttl from ETE */

                    egrPath->eteEncap.s1          = 1;
                    egrPath->eteEncap.exp1        = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_exp & 0x7);
                    egrPath->eteEncap.label1      = (OAM_MPLS_LSP_GAL_LABEL & 0xfffff); /* GAL label */
                    egrPath->eteEncap.ttl1        = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_ttl & 0xff);
                    egrPath->eteEncap.exp1remark  = 0;
                    egrPath->eteEncap.ttl1dec     = 0; /* use ttl from ETE */

                    egrPath->eteEncap.tunnelenter = 1;
                    egrPath->eteEncap.encaplen    = 10;
                    egrPath->eteEncap.etype       = 0x8847;
                    egrPath->eteEncap.ttlcheck    = 0;
                    egrPath->eteEncap.mplsttldec  = 0;

                    /* modport or local gport */
                    if (OAM_MPLS_LSP_OUT_EGR_IF(unit).flags & BCM_L3_TGID) {
                        tid = OAM_MPLS_LSP_OUT_EGR_IF(unit).trunk;
                        status = bcm_fe2000_trunk_get_old(unit, tid, &trunk_info);
                        if (status != BCM_E_NONE) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Could not retreive trunk info for trunkId %d\n"),
                                       tid));
                        } else {
                            index = endpoint_info->trunk_index;
                            if ((index > trunk_info.num_ports) || (index < 0)) {
                                index = 0;
                            }
                            mod = trunk_info.tm[index];
                            port = trunk_info.tp[index];
                            /* We are on a trunk, OAM CB takes care of saving
                             * the trunk context, no need to worry
                             */
                            egrPath->is_trunk = 1;
                            egrPath->trunk_id = tid;
                        }
                    } else {
                        mod = OAM_MPLS_LSP_OUT_EGR_IF(unit).module,
                        port = OAM_MPLS_LSP_OUT_EGR_IF(unit).port;
                        egrPath->is_trunk = 0;
                   }

                   BCM_GPORT_MODPORT_SET(egrPath->port, mod, port);

                   /* copy egress interface vlan to epinfo */
                   endpoint_info->vlan           =  OAM_MPLS_LSP_OUT_EGR_IF(unit).vlan;
                 }
             }
         }
    }

    return status;
}

#if defined(INCLUDE_SBX_BFD)
int _oam_mpls_bfd_egr_path_update (int unit,
                               egr_path_desc_t *egrPath,  
                               bcm_bfd_endpoint_info_t *endpoint_info)
{
    int status = BCM_E_NONE;
    bcm_module_t mod = -1;
    bcm_port_t   port = -1;
    int index = 0;
    int tid = 0;
    bcm_trunk_add_info_t trunk_info;

    if ((!egrPath) || (!endpoint_info) || 
           (!OAM_IS_LSP_BFD(endpoint_info->type))) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Invalid Param:  egrPath %x, ep_info %x\n"),
                   egrPath, endpoint_info));
        status = BCM_E_PARAM;
    } else {

        status = _oam_egr_path_dmac_set(unit, 
                                        egrPath, 
                                        OAM_MPLS_LSP_OUT_EGR_IF(unit).mac_addr,
                                        TRUE);
         if (BCM_FAILURE(status)) {
             LOG_ERROR(BSL_LS_BCM_OAM,
                       (BSL_META_U(unit,
                                   "Failed to set dmac on egress path for EP %d\n"),
                        endpoint_info->id));

         } else {
             status = _oam_egr_path_smac_set(unit, 
                                             egrPath,
                                             OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr);
             if (BCM_FAILURE(status)) {
                 LOG_ERROR(BSL_LS_BCM_OAM,
                           (BSL_META_U(unit,
                                       "Failed to set smac on egress path for EP %d\n"),
                            endpoint_info->id));

             } else {

                   /* Clear overlay (or union) fields in the ETE. */
                    egrPath->eteEncap.isid        = 0;
                    egrPath->eteEncap.mimtype     = 0;

                    egrPath->eteEncap.s2          = 0;
                    egrPath->eteEncap.exp2        = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_exp & 0x7);
                    egrPath->eteEncap.label2      = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_label & 0xfffff);
                    egrPath->eteEncap.ttl2        = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_ttl & 0xff);
                    egrPath->eteEncap.remark      = (endpoint_info->egr_map & 0x7f);
                    egrPath->eteEncap.exp2remark  = 1;
                    egrPath->eteEncap.ttl2dec     = 0; /* use ttl from ETE */

                    egrPath->eteEncap.s1          = 1;
                    egrPath->eteEncap.exp1        = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_exp & 0x7);
                    egrPath->eteEncap.label1      = (OAM_MPLS_LSP_GAL_LABEL & 0xfffff); /* GAL label */
                    egrPath->eteEncap.ttl1        = (OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_ttl & 0xff);
                    egrPath->eteEncap.exp1remark  = 0;
                    egrPath->eteEncap.ttl1dec     = 0; /* use ttl from ETE */

                    egrPath->eteEncap.tunnelenter = 1;
                    egrPath->eteEncap.encaplen    = 10;
                    egrPath->eteEncap.etype       = 0x8847;
                    egrPath->eteEncap.ttlcheck    = 0;
                    egrPath->eteEncap.mplsttldec  = 0;

                    /* modport or local gport */
                    if (OAM_MPLS_LSP_OUT_EGR_IF(unit).flags & BCM_L3_TGID) {
                        tid = OAM_MPLS_LSP_OUT_EGR_IF(unit).trunk;
                        status = bcm_fe2000_trunk_get_old(unit, tid, &trunk_info);
                        if (status != BCM_E_NONE) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Could not retreive trunk info for trunkId %d\n"),
                                       tid));
                        } else {
                            index = 0; /* CRA API doesnt yet support:  endpoint_info->trunk_index; */
                            if ((index > trunk_info.num_ports) || (index < 0)) {
                                index = 0;
                            }
                            mod = trunk_info.tm[index];
                            port = trunk_info.tp[index];
                            /* We are on a trunk, OAM CB takes care of saving
                             * the trunk context, no need to worry
                             */
                            egrPath->is_trunk = 1;
                            egrPath->trunk_id = tid;
                        }
                    } else {
                        mod = OAM_MPLS_LSP_OUT_EGR_IF(unit).module,
                        port = OAM_MPLS_LSP_OUT_EGR_IF(unit).port;
                        egrPath->is_trunk = 0;
                    }

                   BCM_GPORT_MODPORT_SET(egrPath->port, mod, port);

                   /* copy egress interface vlan to epinfo */
                   /* CRA endpoint_info->vlan           =  OAM_MPLS_LSP_OUT_EGR_IF(unit).vlan; */
             }
         }
    }

    return status;
}
#endif /* defined(INCLUDE_BFD) */

int _oam_mpls_endpoint_set(int unit,
                           bcm_oam_endpoint_info_t *ep_info, 
                           uint32 ep_rec_index, 
                           egr_path_desc_t *egrPath,
                           tcal_id_t *tcal_id) 
{
    int                               status = BCM_E_NONE , rv;
    oam_sw_hash_data_t               *hash_data = NULL;
    uint32                            lsmIdx;
    soc_sbx_g2p3_lsmac_t              lsm;
    soc_sbx_g2p3_oamep_t              oamep;

    /* MPLS OAM Endpoints are programmed currently.
     * MPLS endpoints can support only loss/delay measurement.
     * MPLS endpoint will only allocate Encapsulation Information
     * required to encapsulate LM/DM packets for now.
     * BFD over MPLS LSP will be created through BFD endpoint */

    /* sanity check pointers */
    if (!ep_info || !egrPath || !tcal_id) {
        status = BCM_E_PARAM;
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Bad input parameter\n")));

    } else {
        /* group */
        hash_data = &_state[unit]->hash_data_store[ep_rec_index];
        
        soc_sbx_g2p3_oamep_t_init(&oamep);

        if (ep_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            /* nothing to do */
        } else {

            if (ep_info->flags & BCM_OAM_ENDPOINT_REPLACE) {

                status = soc_sbx_g2p3_oamep_get(unit, ep_rec_index, &oamep);
                if (BCM_FAILURE(status)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to get oamep 0x%x: %d %s\n"),
                               ep_info->local_id, status, bcm_errmsg(status)));
                } else {
                    _oam_oamep_prepare(unit, &oamep);

                    lsmIdx = ~0;
                    status = _sbx_gu2_ismac_idx_free(unit, 
                                                     OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr,
                                                     _SBX_GU2_RES_UNUSED_PORT,
                                                     &lsmIdx);
                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "freed mac " L2_6B_MAC_FMT " at %d on replace: "
                                             "%s\n"),
                                 L2_6B_MAC_PFMT(OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr), lsmIdx, 
                                 bcm_errmsg(status)));

                    if (status == BCM_E_EMPTY) {
                        status = BCM_E_NONE;
                    }
                }
            }
    
            if (BCM_SUCCESS(status)) {
                /* provision local station match */
                soc_sbx_g2p3_lsmac_t_init (&lsm);
                status = _sbx_gu2_ismac_idx_alloc(unit, 0, 
                                                  OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr,
                                                  _SBX_GU2_RES_UNUSED_PORT,
                                                  &lsmIdx);
                if (BCM_FAILURE(status)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to allocate local station MAC idx\n")));
                    
                } else {
                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "Stored mac " L2_6B_MAC_FMT " at %d\n"),
                                 L2_6B_MAC_PFMT(OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr), lsmIdx));
                    
                    sal_memcpy(lsm.mac, OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr, sizeof(bcm_mac_t));
                    lsm.useport = 0;
                    status = soc_sbx_g2p3_lsmac_set (unit, lsmIdx, &lsm);                
                    
                    if (BCM_FAILURE(status)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Failed to write local station MAC\n")));
                        
                    } else {                
                        hash_data->lsm_idx = lsmIdx;

                        oamep.ftidx = egrPath->ftIdx;
                        status = soc_sbx_g2p3_oamep_set(unit, ep_rec_index, &oamep);

                        if (BCM_FAILURE(status)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Failed to write oamep 0x%x: %d %s\n"),
                                       ep_rec_index, status, bcm_errmsg(status)));
                            
                        }                        
                    }
                }
            }
        }
        
        if (BCM_FAILURE(status) && (!(ep_info->flags & BCM_OAM_ENDPOINT_REPLACE))) {
            if (!(ep_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {   
                if(hash_data->lsm_idx == lsmIdx) {
                    COMPILER_REFERENCE(rv);
                    rv = _sbx_gu2_ismac_idx_free(unit, 
                                            OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr,
                                            _SBX_GU2_RES_UNUSED_PORT,
                                            &lsmIdx);
                }
            }
        }
    }  

    if (BCM_SUCCESS(status)) {
        *tcal_id = _BCM_TCAL_INVALID_ID;
    }

    return status;
}

int _oam_mpls_endpoint_get(int unit,
                          bcm_oam_endpoint_info_t *ep_info, 
                          uint32 ep_rec_index)
{
    int status = BCM_E_NONE;
    oam_sw_hash_data_t *hash_data = NULL;
    uint32 oamep_idx = ep_rec_index;
    egr_path_desc_t egrPath;
    /*soc_sbx_g2p3_oamep_t oamep;*/

    if (ep_info && ENDPOINT_ID_VALID(unit,ep_rec_index)) {
        hash_data = &_state[unit]->hash_data_store[ep_rec_index];

        ep_info->name       = hash_data->ep_name;
        ep_info->flags      = hash_data->flags;
        ep_info->type       = hash_data->type;
        ep_info->group      = hash_data->group;
        ep_info->gport      = hash_data->gport; 
        ep_info->name       = hash_data->ep_name;
        ep_info->local_id   = hash_data->local_rec_idx;
        ep_info->intf_id    = hash_data->intf_id;
        ep_info->trunk_index  = hash_data->trunk_index;
        
        if (hash_data->flags & BCM_OAM_ENDPOINT_REMOTE) {
            oamep_idx = hash_data->local_rec_idx;
            
        } else {
            status = _oam_egr_path_get(unit, &egrPath, oamep_idx);
        }

        /* MPLS-TP OAM Exp and TTL is taken from the encapsulating PW or LSP.
         * they do not use either the pkt_pri or int_pri fields from the endpoint_info.
         * only the egr_map is used to specify the Remark Table Index.
         */
        if(BCM_SUCCESS(status)) {
            ep_info->egr_map = egrPath.eteEncap.remark;
        }
    } else {
        status = BCM_E_PARAM;
    }

    return status;
}


int _oam_mpls_endpoint_delete(int unit, 
                             bcm_oam_endpoint_info_t *ep_info)
{
    int status = BCM_E_PARAM;
    oam_sw_hash_data_t *hash_data = NULL;
    soc_sbx_g2p3_oamep_t oamep;
    int last_errno = BCM_E_NONE;


    /* sanity check pointers */
    if (ep_info && ENDPOINT_ID_VALID(unit, ep_info->id)) {

        hash_data = &_state[unit]->hash_data_store[ep_info->id];

        if(ENDPOINT_ID_VALID(unit, hash_data->loss_idx) ||
           ENDPOINT_ID_VALID(unit, hash_data->delay_idx)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Endpoint has Assoicated Loss[0x%x] or Delay[0x%x]"
                                   "Delete them before destroying endpoint\n"),
                       hash_data->loss_idx, hash_data->delay_idx));            

        } else if (!(hash_data->flags & BCM_OAM_ENDPOINT_REMOTE)) {

            uint32 unknownIdx = ~0;

            status = _bcm_fe2000_get_tunnel_interface_info(unit,
                                                           ep_info->intf_id,
                                                           _FE2K_OUTGOING_LSP);
            if(BCM_SUCCESS(status)) {

                status  = soc_sbx_g2p3_oamep_get(unit, ep_info->id, &oamep);
                if (BCM_FAILURE(status)) {
                /* non-fatal error, keep trying */
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to find EP record idx=%d err=%d %s\n"),
                               ep_info->id, status, bcm_errmsg(status)));
                    SET_LAST_ERRNO(last_errno, status);
                }

                /* remove the LSM entry */
                status = _sbx_gu2_ismac_idx_free(unit, 
                                                 OAM_MPLS_LSP_OUT_IF(unit).l3a_mac_addr,
                                                 _SBX_GU2_RES_UNUSED_PORT,
                                                 &unknownIdx);
                if (status == BCM_E_EMPTY) {
                    soc_sbx_g2p3_lsmac_t lsm;
                    soc_sbx_g2p3_lsmac_t_init (&lsm);
                    
                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "Found no more users of idx %d,"
                                             " clearing hw table\n"),
                                 unknownIdx));

                    status = soc_sbx_g2p3_lsmac_set (unit, unknownIdx, &lsm);
                    if (BCM_FAILURE(status)) {
                        LOG_WARN(BSL_LS_BCM_OAM,
                                 (BSL_META_U(unit,
                                             "Failed to delete local station MAC: %d %s\n"),
                                  status, bcm_errmsg(status)));
                        SET_LAST_ERRNO(last_errno, status);
                    }
                }

                /* 'clear' the entry */
                soc_sbx_g2p3_oamep_t_init(&oamep);  
                status = soc_sbx_g2p3_oamep_set(unit, ep_info->id, &oamep);
                if (BCM_FAILURE(status)) {
                    /* non-fatal error, keep trying */
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to set BFD oamEp idx=%d err=%d %s\n"),
                               ep_info->id, status, bcm_errmsg(status)));
                    SET_LAST_ERRNO(last_errno, status);
                }          
                
                if (last_errno != 0) {
                    status = last_errno;
                }
            }
        }  else {
            status = BCM_E_NONE;
        }
    } 

    return status;
}

/*
 *   Function
 *      _oam_mpls_coco_configure
 *   Purpose
 *      Set the g2p3 coherent counters in the LP, evp2e and oi2e 
 *      of the given endpoint
 *   Parameters
 *       unit        = BCM device number
 *       ep_data     = internal data representing port to count frames
 *       coco_idx    = coherent counter index
 *   Returns
 *       BCM_E_*
 */
static int 
_bcm_fe2000_oam_mpls_coco_configure(int unit,
                                    oam_sw_hash_data_t *ep_data, 
                                    oam_sw_hash_data_t *remote_ep_data,
                                    int loss_idx,
                                    int coco_idx,
                                    uint8 pkt_pri_bitmap,
                                    int update)
{
    
    int status = BCM_E_NONE, last_err_no = BCM_E_NONE;
    /* egr_path_desc_t egrPath; */
    soc_sbx_g2p3_label2e_t label2e;
    soc_sbx_g2p3_lp_t lp;
    soc_sbx_g2p3_oi2e_t oi2e;

    if (!ep_data || !remote_ep_data) {
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "Bad Input Parameter\n")));
        status = BCM_E_NONE;

    } else if (ep_data->delay_idx) {
        /* For 5.7.0 a MPLS endpoint can only be associated with Either LM or DM.
         * Verify if Delay index on MPLS endpoint is zero */
        status = BCM_E_PARAM;

    } else {
        /* Coherent counters has to be provisioned on 
         * Set Local Ep Data Encap OI2E to point to TX Coco Counter */

        /* Y.1731 specifies to count various flavours of OAM-PDU's generated
         * by this MEP. The way to achieve it is set coherent counters to the 
         * MEP Encap and use RCE to conditionalize which OAM-PDU to account.
         * For phase I we only counter Data packets */

        /* Set coherent counters for Data traffic */
        status = _bcm_fe2000_get_tunnel_interface_info(unit, ep_data->intf_id,
                                                       _FE2K_OUTGOING_LSP);
        if (BCM_SUCCESS(status)) {

            status = _bcm_fe2000_get_tunnel_interface_info(unit, remote_ep_data->intf_id,
                                                           _FE2K_INCOMING_LSP);
            if (BCM_SUCCESS(status)) {
                uint32 oidx = OAM_MPLS_LSP_OUT_EGR_IF(unit).encap_id - SBX_RAW_OHI_BASE;
 
                status = soc_sbx_g2p3_oi2e_get(unit, oidx, &oi2e);
                if (BCM_FAILURE(status)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to get Encap oi2e[0x%x]: %d %s\n"),
                               oidx, status, bcm_errmsg(status)));
                    
                } else {
                    if (pkt_pri_bitmap == 0)
                      pkt_pri_bitmap = 0xFF;

                    oi2e.priclass  = pkt_pri_bitmap;
                    if (ep_data->direction == OAM_DIR_DOWN) {
                        oi2e.cocounter = OAM_COCO_TX(coco_idx);
                    } else {
                        oi2e.cocounter = OAM_COCO_RX(coco_idx);
                    }
                    status = soc_sbx_g2p3_oi2e_set(unit, oidx, &oi2e);
                    if (BCM_FAILURE(status)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Failed to set Encap oi2e[0x%x]: %d %s\n"),
                                   oidx, status, bcm_errmsg(status)));
                        
                    } else {
                        LOG_VERBOSE(BSL_LS_BCM_OAM,
                                    (BSL_META_U(unit,
                                                "updated LSP OI2E[0x%x] TX cocounter=0x%x\n"),
                                     oidx, coco_idx));

                        status = soc_sbx_g2p3_label2e_get(unit, 
                                                          OAM_MPLS_LSP_IN_EGR_IF(unit).mpls_label,
                                                          &label2e);
                        if (BCM_FAILURE(status)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Failed to get EP[0x%x] Label2e[0x%x]: %d %s\n"),
                                       remote_ep_data->rec_idx, 
                                       OAM_MPLS_LSP_IN_EGR_IF(unit).mpls_label,
                                       status, bcm_errmsg(status)));
                            
                        } else {
                            /* Tunnel switch add forces logical port allocation, so logical port MUST
                             * exist at this point */
                            if (!label2e.lpidx) {
                                status = BCM_E_INTERNAL;
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "no logical port associated with label: %d\n"),
                                           status));

                            } else {
                                status = soc_sbx_g2p3_lp_get(unit, label2e.lpidx, &lp);
                                if (BCM_FAILURE(status)) {
                                    LOG_ERROR(BSL_LS_BCM_OAM,
                                              (BSL_META_U(unit,
                                                          "Failed to read lp[0x%03x]: %d %s\n"),
                                               label2e.lpidx, status, bcm_errmsg(status)));
                                    
                                } else {
                                    lp.oamepi    = loss_idx;
                                    lp.priclass  = pkt_pri_bitmap;

                                    if (ep_data->direction == OAM_DIR_DOWN) {
                                        lp.cocounter = OAM_COCO_RX(coco_idx);
                                    } else {
                                        lp.cocounter = OAM_COCO_TX(coco_idx);
                                    }
                                    status = soc_sbx_g2p3_lp_set(unit, label2e.lpidx, &lp);

                                    if (BCM_FAILURE(status)) {
                                        LOG_ERROR(BSL_LS_BCM_OAM,
                                                  (BSL_META_U(unit,
                                                              "Failed to write lp[0x%03x]: %d %s\n"),
                                                   label2e.lpidx, status, bcm_errmsg(status)));

                                    } else {
                                        LOG_VERBOSE(BSL_LS_BCM_OAM,
                                                    (BSL_META_U(unit,
                                                                "updated EP[0x%x] Label[0x%x]"
                                                                 " LP[0x%x] RX cocounter=0x%x\n"),
                                                     remote_ep_data->rec_idx, 
                                                     OAM_MPLS_LSP_OUT_EGR_IF(unit).mpls_label,
                                                     label2e.lpidx, coco_idx));
                                    }
                                }
                            }
                            
                            /* undo */
                            if (BCM_FAILURE(status)) {
                                oi2e.priclass  = 0;
                                oi2e.cocounter = 0;
                                status = soc_sbx_g2p3_oi2e_set(unit, oidx, &oi2e);
                                if (BCM_FAILURE(status)) {
                                    SET_LAST_ERRNO(last_err_no, status);
                                    LOG_ERROR(BSL_LS_BCM_OAM,
                                              (BSL_META_U(unit,
                                                          "Failed to write oi2e[0x%03x]: %d %s\n"),
                                               oidx, status, bcm_errmsg(status)));
                                } else {            
                                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                                (BSL_META_U(unit,
                                                            "cleared OI2E[0x%x] TX cocounter\n"),
                                                 oidx));
                                }
                            }
                        } /* label2e get */
                    } /* oi2e set */
                } /* oi2e get */
            }        
        }
    }

    if (BCM_FAILURE(last_err_no)) {
        status = last_err_no;
    }

    return status;
}

/*
 *   Function
 *      _bcm_fe2000_mpls_oam_lm_create
 *   Purpose
 *      Set a g2p3 oamep structure for loss measurements
 *   Parameters
 *       unit        = BCM device number
 *       flags       = BCM flags describing additional features
 *       lmIdx       = loss measurement hw index
 *       epIdx       = endpoint hw index associated with this DM
 *       cocoIdx     = coherent counter index
 *       multId      = loss thrshold mupliplier table id
 *   Returns
 *       BCM_E_*
 */
static int
 _bcm_fe2000_mpls_oam_lm_create(int unit,
                                int flags, uint16 lmIdx, uint16 epIdx, 
                                int cocoIdx, int multId, int int_pri)
{
    int rv;
    soc_sbx_g2p3_oamep_t lmEp, localEp;

    soc_sbx_g2p3_oamep_t_init(&lmEp);

    rv = soc_sbx_g2p3_oamep_get(unit, epIdx, &localEp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read local endpoint at idx 0x%04x: "
                               "%d %s\n"), epIdx, rv, bcm_errmsg(rv)));
        return rv;
    }

    lmEp.mode         = !(flags & BCM_OAM_LOSS_SINGLE_ENDED);
    lmEp.function     = _state[unit]->ep_type_ids.lm;
    lmEp.type         = _state[unit]->ep_transport.lsp;
    lmEp.dir          = OAM_DIR_DOWN;
    lmEp.conttohost   = 0; 
    lmEp.singletohost = (flags & BCM_OAM_LOSS_FIRST_RX_COPY_TO_CPU)?1:0;
    lmEp.mdlevel      = localEp.mdlevel;
    lmEp.counteridx   = cocoIdx;
    lmEp.multiplieridx = multId;
    /* for mpls, this looks OK. MPLS endpoint is only useful when 
     * LM/DM are attached to it. So the encap information can be used by
     * LM/DM attached this will save forwarding resources */
    lmEp.ftidx_store   = localEp.ftidx;  
    lmEp.intpripm      = int_pri;
    /* CRA, this is probably an incorrect setting of intpripm.
     *      for MPLS-TP OAM, the TTL & Exp marking is taken
     *      from the encapsulating PW or LSP.  MPLS-TP OAM
     *      is currently configured on Provider mode ports.
     *      Exp = rcos = intpripm = egrPath.ete.exp.  The
     *      remark table index is in endpoint_info->egr_map.
     */

    rv = soc_sbx_g2p3_oamep_set(unit, lmIdx, &lmEp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to write loss data at idx 0x%04x"
                               ": %d %s\n"), lmIdx, rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "Wrote LossIdx=0x%04x, EpIdx=0x%04x\n"),
                 lmIdx, epIdx));

    return rv;
}

/*
 *   Function
 *     _bcm_fe2000_mpls_oam_delay_set
 *   Purpose
 *      Provision Delay measurement on MPLS LSP OAM endpoint
 *   Parameters
 *       unit        = BCM device number
 *       delay_ptr   = delay parameter
 *       local_ep    = mpls oam local endpoint datum
 *       peer_dp     = mpls peer oam endpoint datum
 *       add_dm_endpoint = TRUE(add delay measurement)
 *                         FALSE(delete delay measurement)
 *   Returns
 *       BCM_E_*
 */
 int _bcm_fe2000_mpls_oam_loss_set(int unit, 
                                   bcm_oam_loss_t *loss_ptr,
                                   oam_sw_hash_data_t *local_ep,
                                   uint8 add_lm_endpoint) 
{
    int status = BCM_E_NONE;
    uint32            loss_idx, coco_idx = OAM_COCO_INVAID_ID;
    tcal_id_t           tcal_id = 0, tmp_tcal = 0;
    int                 update=0;
    int                 txenable, coco_configured = 0;
    int                 multId = _OAM_LTM_INVALID_ID;
    soc_sbx_g2p3_oamep_t oamep, loss_ep;
    oam_sw_hash_data_t *lm_data=NULL;
    oam_sw_hash_data_t *remote_ep_data = NULL;

    OAM_LOCK(unit);

    /* if this is a LM added to MPLS performance endpoint, verify if remote 
     * mpls endpoint exists. Remote endpoint is required to associate coherent
     * rx counters to Gport logical port */
    if (!ENDPOINT_ID_VALID(unit, loss_ptr->remote_id)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Invalid remote endpoint id: 0x%08x\n"), 
                   loss_ptr->remote_id));
        status =  BCM_E_PARAM;

    } else {
        remote_ep_data = &_state[unit]->hash_data_store[loss_ptr->remote_id];
        
        if (!ENDPOINT_ID_VALID(unit, remote_ep_data->rec_idx) ||
            (!(remote_ep_data->flags & BCM_OAM_ENDPOINT_REMOTE))) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Remote endpoint not found at endpoint id: 0x%08x\n"), 
                       loss_ptr->remote_id));
                
            status = BCM_E_PARAM;
            
        } else if (!(loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED)) {
            /* only single ended lm supported */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Only Singled Ended LM supported \n")));
            status = BCM_E_PARAM;                
        }
    }
    
    if(BCM_SUCCESS(status)) {
        status = soc_sbx_g2p3_oamep_get(unit, local_ep->rec_idx, &oamep);
        if (BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to get oamep 0x%04x\n"),
                       local_ep->rec_idx));

        } else {
            /* Add or Update Endpoint */
            if (add_lm_endpoint) {

                update = ENDPOINT_ID_VALID(unit, local_ep->loss_idx);
                if (update && (loss_ptr->flags & BCM_OAM_LOSS_WITH_ID) &&
                     (local_ep->loss_idx != loss_ptr->loss_id)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Existing loss id (0x%04x) doesn't "
                                           "match passed loss id (0x%04x)\n"), 
                               local_ep->loss_idx, loss_ptr->loss_id));

                    status = BCM_E_CONFIG;
                } else if (update) {
                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "loss record 0x%04x found to exist; re-using\n"),
                                 local_ep->loss_idx));
                    
                    loss_idx = local_ep->loss_idx;
                    lm_data = &_state[unit]->hash_data_store[loss_idx];
                    
                    if ((loss_ptr->flags &  BCM_OAM_LOSS_SINGLE_ENDED) &&
                        !_BCM_TCAL_ID_VALID(lm_data->tcal_id)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Invalid timer calendar found in loss state; "
                                               "idx=0x%04x\n"), loss_idx));
                        status = BCM_E_INTERNAL;

                    } else {
                        soc_sbx_g2p3_oamep_t loss_ep;
                        status = soc_sbx_g2p3_oamep_get(unit, loss_idx, &loss_ep);
                        if (BCM_FAILURE(status)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Failed to read LossEP 0x%x: %d %s\n"),
                                       loss_idx, status, bcm_errmsg(status)));

                        } else {
                            _bcm_ltm_threshold_free(unit, loss_ep.multiplieridx);
                            tcal_id   = lm_data->tcal_id;
                            coco_idx  = lm_data->cocounter;
                        }
                    }
                } else {
                    if (loss_ptr->flags & BCM_OAM_LOSS_WITH_ID) {
                        status = _bcm_fe2000_oam_ep_check_and_reserve(unit, 
                                                      loss_ptr->loss_id);
                        loss_idx = loss_ptr->loss_id;
                    } else {

                        status = shr_idxres_list_alloc(_state[unit]->ep_pool,
                                                       &loss_idx);
                        if (BCM_FAILURE(status)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Failed to allocate an "
                                                   "loss record: %d %s\n"),
                                       status, bcm_errmsg(status)));

                        } else {
                            loss_ptr->loss_id = loss_idx;
                            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "Allocated loss record "
                                                     "0x%04x\n"), loss_idx));
                        }
                    }
                    if (BCM_SUCCESS(status)) {

                        lm_data = &_state[unit]->hash_data_store[loss_idx];

                        status = shr_idxres_list_alloc(_state[unit]->coco_pool, &coco_idx);
                        if (BCM_FAILURE(status)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Out of coherent counter resources\n")));
                        } else {
                            coco_idx = OAM_COCO_BASE(coco_idx);
                        }
                    }
                }

                /* allocate a loss threshold entry */
                if (BCM_SUCCESS(status)) {

                    status = _bcm_ltm_threshold_alloc(unit, loss_ptr->loss_threshold, &multId);
                    if (BCM_FAILURE(status)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Failed to allocate a loss threshold table entry"
                                               ": %d %s\n"), status, bcm_errmsg(status)));

                    } else {
                        /* set the basic loss hw data  */
                        status = _bcm_fe2000_mpls_oam_lm_create(unit, loss_ptr->flags, 
                                                   loss_idx, local_ep->rec_idx, 
                                                   coco_idx, multId, loss_ptr->int_pri);
                        if (BCM_FAILURE(status)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Failed to configure lm oam endpoint"
                                                   ": %d %s\n"), status, bcm_errmsg(status)));
                        } else {
                            /* configure the allocated coherent counters in the lp, oi & evp2e */
                            status = _bcm_fe2000_oam_mpls_coco_configure(unit, local_ep, 
                                                                         remote_ep_data,
                                                                         loss_idx, coco_idx,
                                                                         loss_ptr->pkt_pri_bitmap,
                                                                         update);
                            if (BCM_FAILURE(status)) {
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Failed to configure coherent counters"
                                                       ": %d %s\n"), status, bcm_errmsg(status)));
                            } else {
                                coco_configured = 1;
                                txenable = !!(loss_ptr->flags & BCM_OAM_LOSS_TX_ENABLE);
                                tmp_tcal = tcal_id;
                                
                                
                                if (_BCM_TCAL_ID_VALID(tcal_id)) {
                                    _bcm_tcal_free(unit, &tmp_tcal);
                                }

                                if (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
                                    status = _bcm_tcal_alloc(unit, loss_ptr->period,
                                                             txenable, loss_idx,
                                                             &tmp_tcal);
                                    if (BCM_SUCCESS(status)) {
                                        LOG_VERBOSE(BSL_LS_BCM_OAM,
                                                    (BSL_META_U(unit,
                                                                "%s timer calender entry 0x%05x\n"),
                                                     _BCM_TCAL_ID_VALID(tcal_id)?"Updated":"Allocated",
                                                     tmp_tcal));
                                    } else {
                                        LOG_ERROR(BSL_LS_BCM_OAM,
                                                  (BSL_META_U(unit,
                                                              "Failed to %s tcal id\n"),
                                                   _BCM_TCAL_ID_VALID(tcal_id)?"update":"allocate"));
                                    }
                                    tcal_id = tmp_tcal;
                                    oamep.nextentry = 0;
                                } else {
                                    status = BCM_E_PARAM; /*sanity*/
                                }
                            }
                        }

                        if (BCM_SUCCESS(status)) {
                            status = soc_sbx_g2p3_oamep_set(unit, local_ep->rec_idx, &oamep);
                            if (BCM_FAILURE(status)) {
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Failed to set oamep 0x%04x\n"),
                                           local_ep->rec_idx));

                            } else {
                                LOG_VERBOSE(BSL_LS_BCM_OAM,
                                            (BSL_META_U(unit,
                                                        "Updated EP 0x%04x lmEpIdx=0x%04x\n"),
                                             local_ep->rec_idx, oamep.nextentry));

                                 /* link endpoint to loss data to recognize 'replace' on next add */
                                local_ep->loss_idx  = loss_idx;
                                remote_ep_data->loss_idx  = loss_idx;
                                if (lm_data) {
                                    lm_data->tcal_id   = tcal_id;
                                    lm_data->rec_idx   = loss_idx;
                                    lm_data->peer_idx  = loss_ptr->remote_id;
                                    lm_data->cocounter = coco_idx;
                                    lm_data->flags     = loss_ptr->flags;
                                    lm_data->oam_type  = oamLoss;
                                    lm_data->pkt_pri_bitmap = loss_ptr->pkt_pri_bitmap;
                                }
                                /* If LM over mpls lsp, reference interface so the interface
                                 * cannot be deleted before LM is deleted */
                                if(!update) {
                                    status = _bcm_fe2000_l3_reference_interface(unit, local_ep->intf_id, 1);   
                                    if (BCM_SUCCESS(status)) {
                                        status = _bcm_fe2000_l3_reference_interface(unit, remote_ep_data->intf_id, 1);               
                                        if (BCM_FAILURE(status)) {
                                            _bcm_fe2000_l3_reference_interface(unit, local_ep->intf_id, 0); 
                                        }
                                    }
                                } /* !update */
                            } /* oamep_set */
                        }
                    } /*  _bcm_ltm_threshold_alloc */
                }
                if (BCM_FAILURE(status)) {
                    /* If the loss idx was allocated for the first time,
                     * free it on error
                     */
                    if (!update) {
                        if (ENDPOINT_ID_VALID(unit, loss_idx)) {
                            _bcm_fe2000_oam_ep_check_and_free(unit, loss_idx);
                            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "(error %d) Freeing loss ep 0x%x\n"),
                                         status, loss_idx));
                        }

                        if (_BCM_TCAL_ID_VALID(tcal_id)) {
                            _bcm_tcal_free(unit, &tcal_id);
                            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "(error %d) Freeing tcalId 0x%x\n"),
                                         status, tcal_id));
                        }

                        if (OAM_COCO_VALID(coco_idx)) {
                            shr_idxres_list_free(_state[unit]->coco_pool, 
                                                 OAM_COCO_RES(coco_idx));
                            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "(error %d) Freeing coherent counter 0x%x\n"),
                                         status, coco_idx));
                        }
                        
                        if (multId != _OAM_LTM_INVALID_ID) {
                            _bcm_ltm_threshold_free(unit, multId);
                            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "(error %d) Freeing loss threshold multiplier"
                                                     " table 0x%x\n"),
                                         status, multId));
                        }

                        if(coco_configured) {
                            status =  _bcm_fe2000_oam_mpls_coco_configure(unit, local_ep, 
                                                                      remote_ep_data, 
                                                                      0, 0, 0, update);                        
                        }
                    }
                }
            }  /* if (add_lm_endpoint) */ 
            else {
                /* Delete lm endpoint */
                /* validate loss measurement 'endpoint' */
                lm_data = &_state[unit]->hash_data_store[local_ep->loss_idx];
                
                if (!ENDPOINT_ID_VALID(unit, local_ep->loss_idx) ||
                    (lm_data->oam_type != oamLoss)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Loss measurement not enabled on endpoint id:"
                                           " 0x%08x\n"), loss_ptr->id));
                    status = BCM_E_PARAM;

                } else {
                    status = soc_sbx_g2p3_oamep_get(unit, local_ep->loss_idx, &loss_ep);
                    if (BCM_FAILURE(status)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Failed to read oamep[0x%x]: %d %s\n"),
                                   local_ep->loss_idx, status, bcm_errmsg(status)));
                        status = BCM_E_PARAM;
                    } else {
                        status = _bcm_ltm_threshold_free(unit, loss_ep.multiplieridx);
                    }

                    if (lm_data->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
                        if (_BCM_TCAL_ID_VALID(lm_data->tcal_id)) {
                            _bcm_tcal_free(unit, &lm_data->tcal_id);
                            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "(error %d) Freeing tcalId 0x%x\n"),
                                         status, lm_data->tcal_id));
                        } else {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Invalid timer calendar entry foudn on LM in"
                                                   "single-ended mode\n")));
                            status = BCM_E_CONFIG;
                        }
                    }

                    if (BCM_SUCCESS(status)) {
                        status = shr_idxres_list_free(_state[unit]->coco_pool,
                                                      OAM_COCO_RES(lm_data->cocounter));
                        if (BCM_FAILURE(status)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Failed to free coherent counter 0x%x: %d %s\n"),
                                       lm_data->cocounter, status, bcm_errmsg(status)));
                        } else {
                            status =  _bcm_fe2000_oam_mpls_coco_configure(unit, local_ep, 
                                                                          remote_ep_data, 
                                                                          0, 0, 0, 0);
                            soc_sbx_g2p3_oamep_t_init(&loss_ep);
                            status = soc_sbx_g2p3_oamep_set(unit, local_ep->loss_idx, &loss_ep);
                            if (BCM_FAILURE(status)) {
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Failed to clear LM[0x%x]:%d %s\n"),
                                           local_ep->loss_idx, status, bcm_errmsg(status)));
                            } else {
                                /* If LM over mpls lsp, dereference interface */
                                status = _bcm_fe2000_l3_reference_interface(unit, local_ep->intf_id, 0);   
                                if (BCM_FAILURE(status)) {
                                    LOG_ERROR(BSL_LS_BCM_OAM,
                                              (BSL_META_U(unit,
                                                          "Failed to Dereference Interface [0x%x]:%d %s\n"),
                                               local_ep->intf_id, status, bcm_errmsg(status)));
                                }
                                
                                status = _bcm_fe2000_l3_reference_interface(unit, remote_ep_data->intf_id, 0);               
                                if (BCM_FAILURE(status)) {
                                    LOG_ERROR(BSL_LS_BCM_OAM,
                                              (BSL_META_U(unit,
                                                          "Failed to Dereference Interface [0x%x]:%d %s\n"),
                                               remote_ep_data->intf_id, status, bcm_errmsg(status)));
                                }
                                
                                _oam_hash_data_clear(lm_data);
                                _bcm_fe2000_oam_ep_check_and_free(unit, 
                                                       local_ep->loss_idx);
                                local_ep->loss_idx = INVALID_RECORD_INDEX;
                                remote_ep_data->loss_idx = INVALID_RECORD_INDEX;

                                if (BCM_SUCCESS(status)) {
                                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                                (BSL_META_U(unit,
                                                            "Removed loss measurement from EP 0x%x\n"),
                                                 loss_ptr->id));
                                }
                            }
                        }
                    }
                }
            } /* else of add endpoint - delete endpoint */
        } /* else oamep get */
    }

    OAM_UNLOCK(unit);
    return status;
}


/*
 *   Function
 *      _bcm_fe2000_oam_mpls_dm_set
 *   Purpose
 *      Set a mpls oamep structure for delay measurements
 *   Parameters
 *       unit        = BCM device number
 *       flags       = BCM flags describing additional features
 *       dmIdx       = delay measurement hw index
 *       epIdx       = endpoint hw index associated with this DM
 *       clear       = clear the dm endpoint
 *   Returns
 *       BCM_E_*
 */
STATIC int _bcm_fe2000_oam_mpls_dm_set(int unit, 
                                       int flags, 
                                       uint16 dmIdx, 
                                       uint16 epIdx,
                                       uint8 clear,
                                       uint8 int_pri)
{
    int status = BCM_E_NONE;
    soc_sbx_g2p3_oamep_t dmEp, localEp;

    /* There is no persistent data in the dm endpoint; regenerate it
     * each time 
     */
    if(!clear) {
        soc_sbx_g2p3_oamep_t_init(&dmEp);

        status = soc_sbx_g2p3_oamep_get(unit, epIdx, &localEp);
        if (BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to read local endpoint at idx 0x%04x: "
                                   "%d %s\n"), epIdx, status, bcm_errmsg(status)));

        } else {
            dmEp.mode         = !(flags & BCM_OAM_DELAY_ONE_WAY);
            dmEp.function     = _state[unit]->ep_type_ids.dm;
            dmEp.type         = _state[unit]->ep_transport.lsp;
            dmEp.conttohost   = 0; 
            dmEp.singletohost = (flags & BCM_OAM_LOSS_FIRST_RX_COPY_TO_CPU)?1:0;
            dmEp.dir          = OAM_DIR_DOWN;
            dmEp.mdlevel      = localEp.mdlevel;
            dmEp.ftidx        = localEp.ftidx;
            dmEp.intpripm     = int_pri; 
    /* CRA, this is probably an incorrect setting of intpripm.
     *      for MPLS-TP OAM, the TTL & Exp marking is taken
     *      from the encapsulating PW or LSP.  MPLS-TP OAM
     *      is currently configured on Provider mode ports.
     *      Exp = rcos = intpripm = egrPath.ete.exp.  The
     *      remark table index is in endpoint_info->egr_map.
     */

            status = soc_sbx_g2p3_oamep_set(unit, dmIdx, &dmEp);
            if (BCM_FAILURE(status)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to write delay data at idx 0x%04x"
                                       ": %d %s\n"), dmIdx, status, bcm_errmsg(status)));

            } else {
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "Wrote DelayIdx=0x%04x, EpIdx=0x%04x\n"),
                             dmIdx, epIdx));
            }
        }
    } else {
        soc_sbx_g2p3_oamep_t_init(&dmEp);    
        status = soc_sbx_g2p3_oamep_set(unit, dmIdx, &dmEp);
        if (BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to clear delay data at idx 0x%04x"
                                   ": %d %s\n"), dmIdx, status, bcm_errmsg(status)));

        } else {
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Cleared DelayIdx=0x%04x\n"),
                         dmIdx));
        }    
    }

    return status;
}

/*
 *   Function
 *     _bcm_fe2000_mpls_oam_delay_set
 *   Purpose
 *      Provision Delay measurement on MPLS LSP OAM endpoint
 *   Parameters
 *       unit        = BCM device number
 *       delay_ptr   = delay parameter
 *       local_ep    = mpls oam local endpoint datum
 *       peer_dp     = mpls peer oam endpoint datum
 *       add_dm_endpoint = TRUE(add delay measurement)
 *                         FALSE(delete delay measurement)
 *   Returns
 *       BCM_E_*
 */
int _bcm_fe2000_mpls_oam_delay_set(int unit, 
                                   bcm_oam_delay_t *delay_ptr,
                                   oam_sw_hash_data_t *local_ep,
                                   oam_sw_hash_data_t *peer_ep,
                                   uint8 add_dm_endpoint) 
{
    int status = BCM_E_NONE, last_err_no = BCM_E_NONE;
    oam_sw_hash_data_t *dm_data = NULL;
    uint32 delay_idx = INVALID_RECORD_INDEX;
    soc_sbx_g2p3_label2e_t label2e;
    soc_sbx_g2p3_lp_t lp;
    uint8 tx_enable = 0;
    uint8 update=0;
    int old_period=0, old_tx_enable=0;

    /* Assumption: validations are performed on parent function */

    if (!peer_ep || !local_ep || !delay_ptr) {
        status = BCM_E_PARAM;

    } else if (local_ep->loss_idx) {
        /* For 5.7.0 a MPLS endpoint can only be associated with Either LM or DM.
         * Verify if Delay index on MPLS endpoint is zero */
        status = BCM_E_PARAM;

    } else {
        /* get incoming tunnel label information so we can associate  
         * delay endpoint with label's logical port */
        status = _bcm_fe2000_get_tunnel_interface_info(unit, peer_ep->intf_id, _FE2K_INCOMING_LSP);
        if(BCM_SUCCESS(status)) {

            status = soc_sbx_g2p3_label2e_get(unit, 
                                              OAM_MPLS_LSP_IN_EGR_IF(unit).mpls_label,
                                              &label2e);
            if (BCM_FAILURE(status)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to get EP[0x%x] Label2e[0x%x]: %d %s\n"),
                           peer_ep->rec_idx, 
                           OAM_MPLS_LSP_IN_EGR_IF(unit).mpls_label,
                           status, bcm_errmsg(status)));
                
            } else {
                tx_enable =  !!(delay_ptr->flags & BCM_OAM_DELAY_TX_ENABLE);

                /* An update/replace is when the delay_idx already exists for the endpoint
                 */
                if(add_dm_endpoint) {
                    update = ENDPOINT_ID_VALID(unit, local_ep->delay_idx);
                    if (update && (delay_ptr->flags & BCM_OAM_DELAY_WITH_ID) &&
                         (local_ep->delay_idx != delay_ptr->delay_id)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Existing delay id (0x%04x) "
                                               "doesn't match passed delay id (0x%04x)\n"), 
                                   local_ep->delay_idx, delay_ptr->delay_id));

                        status = BCM_E_CONFIG;
                    } else if (update) {
                        /* if update, logical port is expected to be linked to label2e */
                        if(label2e.lpidx) {

                             LOG_VERBOSE(BSL_LS_BCM_OAM,
                                         (BSL_META_U(unit,
                                                     "delay record 0x%04x found to exist; re-using\n"),
                                          local_ep->delay_idx));
                             
                             delay_idx = local_ep->delay_idx;
                             dm_data = &_state[unit]->hash_data_store[delay_idx];
                             
                             if (!_BCM_TCAL_ID_VALID(dm_data->tcal_id)) {
                                 LOG_ERROR(BSL_LS_BCM_OAM,
                                           (BSL_META_U(unit,
                                                       "Invalid timer calendar found in delay state; "
                                                        "idx=0x%04x\n"), delay_idx));
                                 status = BCM_E_INTERNAL;

                             } else {
                                 status = _bcm_tcal_period_get(unit, dm_data->tcal_id, &old_period, &old_tx_enable);
                                 if (BCM_SUCCESS(status)) {
                                     status = _bcm_tcal_update(unit, 
                                                               delay_ptr->period, 
                                                               tx_enable, delay_idx,
                                                               &dm_data->tcal_id);
                                 } else {
                                     old_period = -1;
                                 }
                             }
                        } else {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "No Logical port associated with label[0x%x]\n"),
                                       OAM_MPLS_LSP_IN_EGR_IF(unit).mpls_label));
                            status = BCM_E_PARAM;
                        }
                        
                    } else { /* add */
                        if (!label2e.lpidx) {
                            status = BCM_E_INTERNAL;
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "no logical port associated with label: %d\n"),
                                       status));

                        }  else {
                            status = soc_sbx_g2p3_lp_get(unit, label2e.lpidx, &lp);
                            if (BCM_FAILURE(status)) {
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Failed to read lp[0x%03x]: %d %s\n"),
                                           label2e.lpidx, status, bcm_errmsg(status)));
                                
                            } else {
                                if (delay_ptr->flags & BCM_OAM_LOSS_WITH_ID) {
                                    status = _bcm_fe2000_oam_ep_check_and_reserve(
                                                     unit, delay_ptr->delay_id);
                                    if (BCM_FAILURE(status)) {
                                        return status;
                                    }
                                    delay_idx = delay_ptr->delay_id;
                                } else {

                                    status = shr_idxres_list_alloc(
                                                         _state[unit]->ep_pool,
                                                         &delay_idx);
                                    if (BCM_FAILURE(status)) {
                                        LOG_ERROR(BSL_LS_BCM_OAM,
                                                  (BSL_META_U(unit,
                                                              "Failed to "
                                                               "allocate an delay record: %d %s"
                                                               "\n"), status, bcm_errmsg(status)));
                                    
                                    } else {
                                        delay_ptr->delay_id = delay_idx;
                                        LOG_VERBOSE(BSL_LS_BCM_OAM,
                                                    (BSL_META_U(unit,
                                                                "Allocated "
                                                                 "delay record 0x%04x\n"), 
                                                     delay_idx));
                                    }
                                }
                                if (BCM_SUCCESS(status)) {
                                    dm_data = &_state[unit]->hash_data_store[delay_idx];
                                    
                                    status = _bcm_tcal_alloc(unit, 
                                                             delay_ptr->period, 
                                                             tx_enable, delay_idx,
                                                             &dm_data->tcal_id);
                                    if(BCM_FAILURE(status)) {
                                        LOG_ERROR(BSL_LS_BCM_OAM,
                                                  (BSL_META_U(unit,
                                                              "unable to allocate Timer Calendar: %d %s\n"),
                                                   status, bcm_errmsg(status)));
                                    }
                                }
                            }
                        }
                    } /*add*/

                    if (BCM_SUCCESS(status)) {
                        status = _bcm_fe2000_oam_mpls_dm_set(unit, delay_ptr->flags, 
                                                             delay_idx, local_ep->rec_idx,
                                                             FALSE, delay_ptr->int_pri);
                        
                        if (BCM_SUCCESS(status) && !update) {
                            lp.oamepi = delay_idx;
                            status = soc_sbx_g2p3_lp_set(unit, label2e.lpidx, &lp);
                            if (BCM_FAILURE(status)) {
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Failed to set lp[0x%03x]: %d %s\n"),
                                           label2e.lpidx, status, bcm_errmsg(status)));
                                
                            } 
                            /* Reference the L3 interface */
                            status = _bcm_fe2000_l3_reference_interface(unit, local_ep->intf_id, 1);   
                            if (BCM_SUCCESS(status)) {
                                
                                status = _bcm_fe2000_l3_reference_interface(unit, peer_ep->intf_id, 1);               
                                if (BCM_FAILURE(status)) {
                                    _bcm_fe2000_l3_reference_interface(unit, local_ep->intf_id, 0); 
                                    }
                            }
                        }
                    }

                    if (BCM_SUCCESS(status)) {
                        if (!update) {
                            dm_data = &_state[unit]->hash_data_store[delay_idx];
                        }

                        local_ep->delay_idx = delay_idx;
                        peer_ep->delay_idx  = delay_idx;
                        dm_data->rec_idx    = delay_idx;
                        dm_data->peer_idx   = delay_ptr->remote_id;
                        dm_data->flags      = delay_ptr->flags;
                        dm_data->oam_type   = oamDelay;

                    } else {
                        /* Last failure for add is referencing L3 interace. So ignore dereferencing
                         * l3 interface */
                        if(!update) {
                            if(dm_data) {
                                status = _bcm_tcal_free(unit, &dm_data->tcal_id);
                                if(BCM_FAILURE(status)) {
                                    LOG_ERROR(BSL_LS_BCM_OAM,
                                              (BSL_META_U(unit,
                                                          "(error %d) Failed Freeing tcalId 0x%x\n"),
                                               status, dm_data->tcal_id));
                                    SET_LAST_ERRNO(last_err_no, status);
                                }            
                            }    

                            status = _bcm_fe2000_oam_mpls_dm_set(unit, 0, delay_idx, 0, TRUE, 0);
                            if (BCM_FAILURE(status)) {
                                SET_LAST_ERRNO(last_err_no, status);
                            }            

                            lp.oamepi = 0;
                            status = soc_sbx_g2p3_lp_set(unit, label2e.lpidx, &lp);
                            if (BCM_FAILURE(status)) {
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Failed to set lp[0x%03x]: %d %s\n"),
                                           label2e.lpidx, status, bcm_errmsg(status)));
                                SET_LAST_ERRNO(last_err_no, status);
                                
                            } 
                        } else {
                            /* leave old config untouched */
                            if(old_period && dm_data) {
                                status = _bcm_tcal_update(unit, old_period, 
                                                          old_tx_enable, delay_idx,
                                                          &dm_data->tcal_id);
                                if (BCM_FAILURE(status)) {
                                    SET_LAST_ERRNO(last_err_no, status);
                                    LOG_ERROR(BSL_LS_BCM_OAM,
                                              (BSL_META_U(unit,
                                                          "(error %d) Failed tcal update tcalId 0x%x\n"),
                                               status, dm_data->tcal_id));
                                }
                            }
                        }
                    }
                } /*if(add_dm_endpoint)*/ else {
                    int last_err_no = BCM_E_NONE;

                    delay_idx = local_ep->delay_idx;
                    dm_data = &_state[unit]->hash_data_store[delay_idx];

                    /* delete delay measurement */
                    if (label2e.lpidx) {
                        lp.oamepi = 0;
                        status = soc_sbx_g2p3_lp_set(unit, label2e.lpidx, &lp);
                        if (BCM_FAILURE(status)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Failed to set lp[0x%03x]: %d %s\n"),
                                       label2e.lpidx, status, bcm_errmsg(status)));
                            SET_LAST_ERRNO(last_err_no, status);
                            
                        }
                    } else {
                        status = BCM_E_INTERNAL;
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "no logical port associated with label: %d\n"),
                                   status));
                        SET_LAST_ERRNO(last_err_no, status);
                    }

                    status = _bcm_tcal_free(unit, &dm_data->tcal_id);
                    if(BCM_FAILURE(status)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "(error %d) Failed Freeing tcalId 0x%x\n"),
                                   status, dm_data->tcal_id));
                        SET_LAST_ERRNO(last_err_no, status);
                    }
                    
                    status = _bcm_fe2000_oam_mpls_dm_set(unit, 0, delay_idx, 0, TRUE, 0);
                    if (BCM_FAILURE(status)) {
                        SET_LAST_ERRNO(last_err_no, status);
                    }

                    /* dereference l3 interface */
                    status = _bcm_fe2000_l3_reference_interface(unit, local_ep->intf_id, 0);   
                    if (BCM_FAILURE(status)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Failed to De-reference Outgoing L3 interface [0x%x]\n"),
                                   local_ep->intf_id));
                        SET_LAST_ERRNO(last_err_no, status);
                    }
                        
                    status = _bcm_fe2000_l3_reference_interface(unit, peer_ep->intf_id, 0);               
                    if (BCM_FAILURE(status)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Failed to De-reference Incoming L3 interface [0x%x]\n"),
                                   peer_ep->intf_id));
                        SET_LAST_ERRNO(last_err_no, status);
                    }

                    _bcm_fe2000_oam_ep_check_and_free(unit, 
                                                      local_ep->delay_idx);
                    _oam_hash_data_clear(dm_data);
                    local_ep->delay_idx = INVALID_RECORD_INDEX;
                    peer_ep->delay_idx  = INVALID_RECORD_INDEX;
                } /* delete */
            }
        }
    }

    if (last_err_no != 0) {
        status = last_err_no;
    }
    return status;
}

#endif  /* INCLUDE_L3 */
