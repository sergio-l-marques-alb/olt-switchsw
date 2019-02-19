/*
 * $Id: bfd.c,v 1.28 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * FE2000 OAM API
 */

#if defined(INCLUDE_SBX_BFD)

#include <shared/bsl.h>

#include <bcm_int/sbx/fe2000/oam/oam.h>
#include <bcm_int/sbx/fe2000/oam/bfd.h>
#include <bcm_int/sbx/fe2000/oam/mpls.h>
#include <bcm_int/sbx/fe2000/oam/recovery.h>
#include <bcm_int/sbx/fe2000/mpls.h>

#include "sal/core/dpc.h"

extern oam_state_t* _state[SOC_MAX_NUM_DEVICES];

extern int global_oam_sdk;     /* number of times the ucode table writes timed out. */
extern int global_oam_dpc;     /* Flag for DPC feature, to delay fast-rate Watchdog timeout. */
extern int global_oam_timeout; /* If DPC==1, the watchdog timeout immediately after transition to up. */


void _dpc_delay_callback(void* owner, void* p0, void* p1, void* p2, void* p3){
    int rv = BCM_E_NONE;
    int unit = (int) p0;
    bcm_bfd_endpoint_t id = (bcm_bfd_endpoint_t) owner;
    bcm_bfd_endpoint_info_t ep_info;

    bcm_bfd_endpoint_info_t_init(&ep_info);

    /* Retrieve the remote endpoint indicated as "owner" in the callback. */
    rv = bcm_bfd_endpoint_get(unit, id, &ep_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Error: couldnt find endpoint: 0x%08x\n"),
                   id));
    } else {
        ep_info.flags |= (BCM_BFD_ENDPOINT_UPDATE | BCM_BFD_ENDPOINT_WITH_ID);
        ep_info.id = id;
        rv = bcm_bfd_endpoint_create(unit, &ep_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Error: couldnt update endpoint: 0x%08x\n"),
                       id));
        }
    }

    return;
}


extern int is_oam_ep_id_app_reserved(int unit, int id);
extern int oam_group_ep_list_add(int unit, int group, dq_p_t ep);
extern int _oam_remove_ep_from_trunk_list(int unit, bcm_trunk_t tid, dq_p_t ep_elem);
extern int _oam_delete_trunk_info(int unit, bcm_trunk_t tid);
extern int oam_group_ep_list_remove(int unit, int group, dq_p_t ep);


void
_init_bfd_ep_hash_key(int unit, oam_sw_hash_key_t key, 
                  bcm_bfd_endpoint_info_t *ep_info)
{
    uint8* loc = key;
    uint32 direction = 0;
    
    sal_memset(key, 0, sizeof(oam_sw_hash_key_t));

    if(ep_info) {
        direction = !!(ep_info->flags & BCM_OAM_ENDPOINT_UP_FACING);
        OAM_KEY_PACK(loc,OAM_GROUP_INFO(unit,0)->name, MAID_MAX_LEN);
        OAM_KEY_PACK(loc, ep_info->mep_id, sizeof(ep_info->mep_id));
        OAM_KEY_PACK(loc, &ep_info->gport, sizeof(ep_info->gport));
        OAM_KEY_PACK(loc, &ep_info->label, sizeof(ep_info->label));
    }
    assert ((int)(loc - key) <= sizeof(oam_sw_hash_key_t));    
}


/*
 * Update the egress path based on the endpoint, and commit to hardware
 */
int _bfd_egr_path_update(int unit,
                     egr_path_desc_t *egrPath, 
                     bcm_bfd_endpoint_info_t *ep_info,
                     bcm_trunk_add_info_t *trunk_info)
{
    int          rv;
    bcm_module_t mod;
    bcm_port_t   port;
    int fabUnit, fabNode, fabPort;
    int dir;
    bcm_gport_t gport;

    if (!egrPath || !ep_info || !trunk_info) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "NULL pointer\n")));
        return BCM_E_PARAM;
    }

    dir = !!(ep_info->flags & BCM_OAM_ENDPOINT_UP_FACING);

    egrPath->eteL2.usevid = TRUE;

    /* when p2e.customer = 1, (customer port) this vlan_pri field will
     * be copied into the vlan tag.pricfi.  cfi is always zero.
     */
    egrPath->eteL2.defpricfi = (ep_info->vlan_pri << 1) + 0;

    /* when p2e.customer = 0, (provider port) the remark table, rcos, and rdp
     * are all specified in the int_pri field.
     * 7 bits table, 3 bits rcos, 2 bits rdp
     * set the table number in the encap ETE, and the rcos/rdp into the oamEp entry.
     */
    if(ep_info->flags & BCM_OAM_ENDPOINT_USE_QOS_MAP) {
        egrPath->eteEncap.remark = (ep_info->egr_map) & 0x7f;
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Set remark index (QOS map) from egr_map %d (0x%04x)\n")), 
                  egrPath->eteEncap.remark, egrPath->eteEncap.remark));
    } else {
        egrPath->eteEncap.remark = ((int)ep_info->int_pri >> 5) & 0x7f;
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Set remark index (QOS map) from int_pri %d (0x%04x)\n")), 
                  egrPath->eteEncap.remark, egrPath->eteEncap.remark));
    }

    gport = ep_info->gport;

    /* for BFD, use physical gport under mpls gport
     * In case of LSP, use the modport of the tunnel egress interface
    */
    if (BCM_GPORT_IS_MPLS_PORT(ep_info->gport) || 
        OAM_IS_LSP_BFD(ep_info->type)) {
        gport = egrPath->port;
    }


    /* get dest node/port from the gport - which could be a trunk */
    if (BCM_GPORT_IS_LOCAL(gport)) {
        rv = bcm_stk_my_modid_get(unit, &mod);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to get modid: %d %s\n"), 
                       rv, bcm_errmsg(rv)));
            return rv;
        }
        port = BCM_GPORT_LOCAL_GET(gport);

    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        mod = BCM_GPORT_MODPORT_MODID_GET(gport);
        port = BCM_GPORT_MODPORT_PORT_GET(gport);

    } else {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Unsupported GPORT 0x%08x\n"),
                   gport));
        return BCM_E_PARAM;
    }

    if ((port < 0 || port >= SBX_MAX_PORTS) || !SOC_MODID_ADDRESSABLE(unit, mod)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "invalid gport passed: 0x%08x %s %s\n"),
                   gport, SOC_PORT_VALID(unit, port) ? " " : "invalid port",
                   SOC_MODID_ADDRESSABLE(unit, mod) ? " " : "invalid module id"));
        return BCM_E_PARAM;
    }

    /* get the fab node/port from the dest node/port */
    rv = soc_sbx_node_port_get(unit, mod, port,
                               &fabUnit, &fabNode, &fabPort);
    
    if (BCM_FAILURE(rv)) {
        /* failed to get target information */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "soc_sbx_node_port_get(%d,%d,%d,&(%d),&(%d),&(%d)) "
                               "returned %d (%s)\n"),
                   unit, mod, port, fabUnit, fabNode, fabPort,
                   rv, bcm_errmsg(rv)));
        return rv;
    } else {
        /* check destination node */
        if (!SOC_SBX_NODE_ADDRESSABLE(unit, fabNode)) {
            /* inaccessible destination node */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "target node %d inaccessible\n"),
                       fabNode));
            return rv;;
        }
    }

    /* get the qid from the fab node/port */
    egrPath->fte.qid = SOC_SBX_NODE_PORT_TO_QID(unit,fabNode, fabPort, 
                                                NUM_COS(unit));

    return _oam_egr_path_commit(unit, egrPath);
}


int
bcm_fe2000_bfd_init(int unit)
{
    /* Currently, the OAM init function handles both oam & bfd. */
    return BCM_E_NONE;
}

int
bcm_fe2000_bfd_detach(int unit)
{
    /* Currently, the OAM detach function handles both oam & bfd. */
    return BCM_E_NONE;
}


/*
 *   Function
 *      bcm_fe2000_bfd_endpoint_get
 *   Purpose
 *      Retrieve an oam endpoint with the given endpoint id
 *   Parameters
 *       unit           = BCM device number
 *       endpoint       = endpoint ID to retrieve
 *       endpoint_info  = storage location for found endpoint
 *   Returns
 *       BCM_E_*
 *  Notes:
 */
int
bcm_fe2000_bfd_endpoint_get(int unit, bcm_bfd_endpoint_t endpoint, 
                            bcm_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    /* soc_sbx_g2p3_oamep_t oamep; */
    /* CRA uint32 clear_persistent_faults; */

    switch(SOC_SBX_CONTROL(unit)->ucodetype) {
#ifdef BCM_FE2000_P3_SUPPORT
    case SOC_SBX_UCODE_TYPE_G2P3:
        if (!BCM_OAM_IS_INIT(unit)) {
            return BCM_E_INIT;
        }
        break;
#endif
    default:
        SBX_UNKNOWN_UCODE_WARN(unit);
        return BCM_E_CONFIG;
        break;
    }

    if (endpoint == 0 || endpoint > _state[unit]->max_endpoints) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Invalid endpoint.\n")));
        return BCM_E_PARAM;
    }

    /* save the flags for use later */
    /* CRA clear_persistent_faults = endpoint_info->clear_persistent_faults; */

    bcm_bfd_endpoint_info_t_init(endpoint_info);
    endpoint_info->flags |= BCM_BFD_ENDPOINT_WITH_ID;
    endpoint_info->id = endpoint;

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "Entered oam_endpoint_get with id=0x%x.\n"),
                 endpoint));

    OAM_LOCK(unit);

    if (oamBfd == _state[unit]->hash_data_store[endpoint].oam_type) {
        rv = _oam_bfd_endpoint_get(unit, endpoint_info, endpoint);

#if 0
        /* CRA, not supported by new API */
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "Persistent-Flags = 0x%8x, Flags = 0x%8x.\n"),
                     clear_persistent_faults, endpoint_info->flags));

        /* now check the RDI bit.  Only valid for Peer (remote) endpoint entries. */
        if ((clear_persistent_faults & BCM_OAM_ENDPOINT_FAULT_REMOTE) &&
            (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE)) {

            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Clear persistent faults set, checking Ack RDI... \n")));

            /* get the ucode endpoint entry. */
            rv = soc_sbx_g2p3_oamep_get(unit, endpoint, &oamep);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to get oamep 0x%x: %d %s\n"), 
                           endpoint, rv, bcm_errmsg(rv)));
                rv = BCM_E_INTERNAL;
            } else {

               /* clear the ack bit, acknowledging the host application received
                * the change of state in the received RDI bit from the peer.
                */
               oamep.ackrdi = 0;
               oamep.ackrdi_rdi = oamep.peerrdi;

               rv = soc_sbx_g2p3_oamep_set(unit, endpoint, &oamep);
               if (BCM_FAILURE(rv)) {
                   LOG_ERROR(BSL_LS_BCM_OAM,
                             (BSL_META_U(unit,
                                         "Failed to set oamep 0x%x: %d %s\n"), 
                              endpoint, rv, bcm_errmsg(rv)));
                   rv = BCM_E_INTERNAL;
               } else {
                   endpoint_info->clear_persistent_faults =
                       ~BCM_BFD_ENDPOINT_FAULT_REMOTE & endpoint_info->clear_persistent_faults;
                   LOG_VERBOSE(BSL_LS_BCM_OAM,
                               (BSL_META_U(unit,
                                           "Cleared Ack-Flag in oamEp %d. \n"),
                                endpoint));
               }
            }
        }
#endif
    }

    OAM_UNLOCK(unit);

    endpoint_info->flags &= ~BCM_BFD_ENDPOINT_WITH_ID;

    return rv;
}


/*
 *   Function
 *      bcm_fe2000_bfd_endpoint_create
 *   Purpose
 *      Create a bfd endpoint and commit to hardware
 *   Parameters
 *       unit           = BCM device number
 *       endpoint_info  = description of endpoint to create
 *   Returns
 *       BCM_E_*
 *  Notes:
 *
 *  BFD over LSP over trunk notes:
 *    1/ There is no gport associated with a LSP. We get to know if there is 
 *       a trunk underneath the LSP when we fetch the tunnel egress info
 *       None of the code under BCM_GPORT_IS_TRUNK is applicable in this case.
 *
 *    2/ Application can optionally specify a port within the trunk group 
 *       for BFD traffic using the trunk_index variable of the endpoint_info struct. 
 *       If incorrectly specified this defaults to index 0
 *
 *    3/ The selected port determines the QID to be used for BFD, this is 
 *       setup in _oam_egr_path_update. The endpoint->gport is internally 
 *       setup to reflect the MODPORT of the port over which the bfd traffic 
 *       has to be sent
 *
 *    4/ We also enqueue the lsp endpoint for callback processing, and rely 
 *       on the oam callback for trunks. 
 */
int
bcm_fe2000_bfd_endpoint_create(int unit, 
                               bcm_bfd_endpoint_info_t *endpoint_info)
{
    int rv=BCM_E_NONE, rv2=BCM_E_NONE;
    uint32 ep_rec_index = 0, ep_rec_index2=0;
    oam_sw_hash_key_t hash_key;
    oam_sw_hash_key_t tmp_hash_key;
    oam_sw_hash_data_t *hash_data = NULL;
    tcal_id_t tcal_id = _BCM_TCAL_INVALID_ID;
    egr_path_desc_t egrPath;
    _oam_trunk_data_t *trunk_data = NULL;
    bcm_trunk_t tid = 0;
    bcm_trunk_add_info_t trunk_info;
    _fe2k_vpn_sap_t *vpn_sap;
    bcm_bfd_endpoint_info_t ep_info;
    int new_hash = 0;

    if (!BCM_OAM_IS_INIT(unit)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to initialize OAM Module\n")));
        return BCM_E_INIT;
    }

    OAM_LOCK (unit);

    egrPath.allocated = FALSE;

    if (OAM_IS_LSP_BFD(endpoint_info->type)) {
        rv =  _bcm_fe2000_validate_oam_lsp_bfd_endpoint(unit, endpoint_info);
    } else if (OAM_IS_BFD(endpoint_info->type)) {
        rv =  _bcm_fe2000_validate_oam_bfd_endpoint(unit, endpoint_info);
        if (BCM_SUCCESS(rv)) {
            /* obtain encapsulation information from the Gport */
            rv = bcm_fe2000_mpls_gport_get(unit,
                                           endpoint_info->gport,
                                           &vpn_sap);
            if(BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to set mpls gport[0x%x] information %d\n"),
                           endpoint_info->gport, rv));
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to validate specific endpoint type\n")));
        goto exit;
    }

    sal_memset(&egrPath, 0, sizeof(egrPath));
    sal_memset(&trunk_info, 0, sizeof(bcm_trunk_add_info_t));
    _init_bfd_ep_hash_key(unit, hash_key, endpoint_info);

    rv = shr_htb_find(_state[unit]->mamep_htbl, hash_key,
                      (shr_htb_data_t *)&hash_data,
                      0 /* don't remove */);

    if (BCM_SUCCESS(rv) && 
        !(endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE)) 
    {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Attempted to add same entry without replace "
                               "flag set\n")));
        rv = BCM_E_EXISTS;
        goto exit;

    } else if (BCM_FAILURE(rv) && 
              (endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE)) {
        /* If we are replacing an endpoint, we need to get a hash hit,
         * otherwise there is no matching entry to replace.
         *   
         * If we are replacing an endpoint with_id, then we ignore the
         * hash hit, and replace the hash entry itself.
         */
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_WITH_ID) {
            bcm_bfd_endpoint_info_t_init(&ep_info);
            rv = bcm_fe2000_bfd_endpoint_get(unit, endpoint_info->id, &ep_info);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to get endpoint info for ep 0x%x: %d %s\n"),
                           endpoint_info->id, rv, bcm_errmsg(rv)));
                goto exit;

            } else {
                _init_bfd_ep_hash_key(unit, tmp_hash_key, &ep_info);
                rv = shr_htb_find(_state[unit]->mamep_htbl, tmp_hash_key,
                                 (shr_htb_data_t *)&hash_data,
                                  1 /* remove old hash */);
                new_hash = 1;
            }   
        } else {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Replace flag set, but entry not found\n")));
            rv = BCM_E_NOT_FOUND;
            goto exit;
        }
    }

    /* Allocate an endpoint record, configure the record for local or peer,
     * allocate the necessary policer resource, and write the record out to
     * the ilib for ucode access.
     */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE) {
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_WITH_ID) {
            ep_rec_index = endpoint_info->id;
        } else {
            /* At this point, hash_data is valid */
            ep_rec_index = hash_data->rec_idx;
        }

        /* verify found record index is allocated */
        rv = shr_idxres_list_elem_state(_state[unit]->ep_pool, ep_rec_index);

        /* caller must have added a record at this given MAID & MEP prior to
         * calling with the REPLACE flag
         */
        if (rv != BCM_E_EXISTS) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Didn't find endpoint %d\n"),
                       endpoint_info->id));
            goto exit;
        }

    } else {
        /* new entry, allocate a record from the database pool.
         * check if a particular endpoint-id has been requested. */
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_WITH_ID) {
            /* set the requested ID */
            ep_rec_index = endpoint_info->id;

            if ((OAM_IS_BFD(endpoint_info->type)) || \
                (OAM_IS_LSP_BFD(endpoint_info->type))) {
                /* check if the requested ID has already been alocated. */
                rv = shr_idxres_list_elem_state(_state[unit]->ep_pool, ep_rec_index);
                if (rv == BCM_E_NOT_FOUND) {
                    /* endpoint not in app reserved range, reserve the requested endpoint-id */
                    rv = shr_idxres_list_reserve(_state[unit]->ep_pool, ep_rec_index, ep_rec_index);

                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Endpoint %d couldnt be reserved:"
                                               "%d %s\n"), ep_rec_index, rv, bcm_errmsg(rv)));
                        goto exit;
                    }
                } else if (rv == BCM_E_EXISTS) {
                        if (!(is_oam_ep_id_app_reserved(unit, ep_rec_index)) || (hash_data != NULL)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Endpoint %d already in use:"
                                                   "%d %s\n"), ep_rec_index, rv, bcm_errmsg(rv)));
                            goto exit;
                        }
                }
            } else {
                if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE)) {
                    ep_rec_index2 = ep_rec_index + 1;
                    rv2 = shr_idxres_list_elem_state(_state[unit]->ep_pool, ep_rec_index2);
                } else {
                    ep_rec_index2 = ep_rec_index;
                    rv2 = BCM_E_NOT_FOUND;
                }

                /* check if the requested ID has already been alocated. */
                rv = shr_idxres_list_elem_state(_state[unit]->ep_pool, ep_rec_index);

                /* Element state of App managed resources will return BCM_E_EXISTS, this is not an error case.
                 * For such resources we do an additional check to see if the endpoint is already allocated.
                 * For locally managed resources, BCM_E_EXISTS indicates the endpoint is already allocated, and is an error.
                 */
                if (rv == BCM_E_EXISTS) {
                    if (is_oam_ep_id_app_reserved(unit, ep_rec_index)) {
                        /* Make sure endpoint is not already created */
                        if (hash_data) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Endpoint %d already in use:"
                                                   "%d %s\n"), ep_rec_index, rv, bcm_errmsg(rv)));
                            goto exit;
                        } 

                        if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE) && (rv2 == BCM_E_EXISTS)) {
                           
                            /* Make sure associated endpoint is not already created */
                            if (!is_oam_ep_id_app_reserved(unit, ep_rec_index2)) {
                                /* local ep is reserved, but associated endpoint is not app reserved */
                                LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Cannot mix externally managed resource (Endpoint %d)"
                                                       "with internally managed resource (Endpoint %d)"),
                                           ep_rec_index, ep_rec_index2));
                                rv = BCM_E_PARAM;
                                goto exit;
                            }
                        }
                    } else if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE) && 
                                (is_oam_ep_id_app_reserved(unit, ep_rec_index2))) {
                        /* local ep is not reserved, but associated endpoint is app reserved */
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Cannot mix externally managed resource (Endpoint %d)"
                                               "with internally managed resource (Endpoint %d)"),
                                   ep_rec_index2, ep_rec_index));
                        rv = BCM_E_PARAM;
                        goto exit;
                    } else {
                        /* Locally managed resource already used */
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Endpoint %d already in use:"
                                               "%d %s\n"), ep_rec_index, rv, bcm_errmsg(rv)));
                        goto exit;
                   }
                } else {
                    if ((rv != BCM_E_NOT_FOUND) || (rv2 != BCM_E_NOT_FOUND)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Endpoint %d cannot be allocated:"
                                               "%d %s\n"), ep_rec_index, rv, bcm_errmsg(rv)));
                        goto exit;
                    } else {
                        /* reserve the requested endpoint-id */
                        rv = shr_idxres_list_reserve(_state[unit]->ep_pool, ep_rec_index, ep_rec_index);

                        if (rv != BCM_E_NONE) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Endpoint %d couldnt be reserved:"
                                                   "%d %s\n"), ep_rec_index, rv, bcm_errmsg(rv)));
                            goto exit;
                        }
                    }
                }
            }
        } else {
            rv = shr_idxres_list_alloc(_state[unit]->ep_pool, &ep_rec_index);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to allocate an endpoint record: %d %s\n"),
                           rv, bcm_errmsg(rv)));
                goto exit;
            }
        }
    }

    hash_data = &_state[unit]->hash_data_store[ep_rec_index];

    /* this rv could be BCM_E_EXISTS or other expected "error" codes
     * depending on the path(s) taken above.  rv is checked again below,
     * and it is expected that rv be set to BCM_E_NONE.  so clear any
     * error codes now.
     */
    rv = BCM_E_NONE;

    /* local endpoints need an egress path, allocate, or find on replace */
    if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE)) {

        if (endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE) { 
            rv = _oam_egr_path_get(unit, &egrPath, ep_rec_index);

        } else {
            /* allocate a new egress path, for down meps only */
            rv = _oam_egr_path_alloc(unit, 
                                     endpoint_info->src_mac_address, &egrPath);
        }

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to allocate/retrieve egress path for "
                                   "endpoint %d: %d %s\n"), endpoint_info->id, rv, 
                       bcm_errmsg(rv)));
            goto exit;
        }

        /* BFD egress path */
        if (OAM_IS_BFD(endpoint_info->type)) {
            rv = _oam_bfd_egr_path_update(unit, &egrPath, endpoint_info, vpn_sap);

        } else if (OAM_IS_LSP_BFD(endpoint_info->type)) {
            rv = _oam_mpls_bfd_egr_path_update(unit, &egrPath, endpoint_info);
        }

        if(BCM_SUCCESS(rv)) {
            rv = _bfd_egr_path_update(unit, &egrPath, endpoint_info, &trunk_info);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to update egress path for EP %d\n"),
                           endpoint_info->id));
            }
        }

    } else {  /* REMOTE endpoint */
        egrPath.ftIdx = 0;
    }

    if(BCM_SUCCESS(rv)) {   
        if (OAM_IS_BFD(endpoint_info->type)) {
            rv = _oam_bfd_endpoint_set(unit, endpoint_info, ep_rec_index,
                                       &egrPath, &tcal_id, 
                                       vpn_sap->vc_mpls_port.match_label);
        } else if (OAM_IS_LSP_BFD(endpoint_info->type)) {
            rv = _oam_bfd_endpoint_set(unit, endpoint_info, ep_rec_index,
                                       &egrPath, &tcal_id, 
                                       endpoint_info->label);

        } else {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Unsupported endpoint type:%d\n"),
                       endpoint_info->type));
            rv = BCM_E_PARAM;
        }
    }
    

    if (BCM_FAILURE(rv)) {

        /* free record on failure only if this is a new entry, that is, not
         * a REPLACE
         */
        if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE)) {
            if (ep_rec_index) {
                shr_idxres_list_free(_state[unit]->ep_pool, ep_rec_index);
            }
            if(_BCM_TCAL_ID_VALID(tcal_id)) {
                _bcm_tcal_free(unit, &tcal_id);
            }
        }

    } else {

        /* CRA hash_data->vid_label = endpoint_info->vlan; */
        
        /* add the endpoint entry to the trunk's list of endpoints
         * if the gport is a trunk or if the LSP is over a trunk */
        if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE)) {
            if (egrPath.is_trunk) {
                tid = egrPath.trunk_id;
                trunk_data = &(_state[unit]->trunk_data[tid]);
            }
            if (BCM_GPORT_IS_TRUNK(endpoint_info->gport) || egrPath.is_trunk) {
                /* insert this entry at start of list. */
                DQ_INSERT_HEAD(&trunk_data->endpoint_list, &hash_data->trunk_list_node);
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "Adding hash_data 0x%8x, ep_idx %d to trunk"
                                         " list\n"),
                             hash_data, ep_rec_index));
            }
        }

        /* 
         * BFD_FINAL and COPYFIRSTTOCPU are cleard by ucode
         * SDK will reflect this and autoclear thse flags
         */
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_FIRST_RX_COPY_TO_CPU) {
            endpoint_info->flags &= ~BCM_BFD_ENDPOINT_FIRST_RX_COPY_TO_CPU;
        }
        /* CRA new API doesnt support
        if (endpoint_info->flags & BCM_OAM_BFD_FINAL) {
            endpoint_info->flags &= ~BCM_OAM_BFD_FINAL;
        }
        */

        hash_data->rec_idx     = ep_rec_index;
        /* CRA, MEPID hash_data->ep_name     = endpoint_info->name; */
        hash_data->flags       = endpoint_info->flags;
        hash_data->type        = endpoint_info->type;
        hash_data->group       = 0; /* CRA, endpoint_info->group; */
        hash_data->gport       = endpoint_info->gport;
        hash_data->tcal_id     = tcal_id;
        /* hash_data->ing_map     = endpoint_info->ing_map; */
        hash_data->egr_map     = endpoint_info->egr_map;
        hash_data->intf_id     = endpoint_info->egress_if;
        /* By caching the trunk id, we save on invoking set of API calls 
         * to get trunk id during endpoint_delete
         */
        hash_data->lsp_trunk_id = (egrPath.is_trunk ?  egrPath.trunk_id : SBX_MAX_TRUNKS);
        /* CRA hash_data->trunk_index = endpoint_info->trunk_index; */
        
        if (OAM_IS_BFD(endpoint_info->type)) {
            hash_data->oam_type  = oamBfd;
            hash_data->vid_label = vpn_sap->vc_mpls_port.match_label;
            hash_data->flags = endpoint_info->flags;

            if(!(endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE)) {
                hash_data->local_rec_idx = ep_rec_index;
                /* Local endpoint_info->local_min_tx sets TCAL. */
                hash_data->interval = endpoint_info->local_min_tx;
            } else {
                hash_data->local_rec_idx = endpoint_info->local_id;
                /* Remote endpoint_info->local_min_rx sets WD. */
                hash_data->period = endpoint_info->local_min_rx;
            }

        } else if (OAM_IS_LSP_BFD(endpoint_info->type)) {
            hash_data->oam_type  = oamBfd;
            hash_data->vid_label = endpoint_info->label;
            hash_data->flags = endpoint_info->flags;

            if(!(endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE)) {
                hash_data->local_rec_idx = ep_rec_index;
                /* Local endpoint_info->local_min_tx sets TCAL. */
                hash_data->interval = endpoint_info->local_min_tx;
            } else {
                hash_data->local_rec_idx = endpoint_info->local_id;
                /* Remote endpoint_info->local_min_rx sets WD. */
                hash_data->period = endpoint_info->local_min_rx;
            }

        }
        sal_memcpy( hash_data->mac_address, endpoint_info->src_mac_address,
                    sizeof(bcm_mac_t));

        /* Add to hash table if the entry is newly added/ replaced */
        if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE) || (new_hash)) {
            rv = shr_htb_insert(_state[unit]->mamep_htbl, hash_key, hash_data);
        }

        endpoint_info->id = ep_rec_index;

        /* back out the endpoint add only if its not a replace */
        if (BCM_FAILURE(rv) && 
            !(endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE)) {

            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Duplicate MAID/MEP\n")));
            /* don't re-fetch the data, use the record id currently allocated.
             * The record found by hash may be a duplicate, thus getting the
             * duplicate data will delete the valid entry, not _this_
             * invalid duplicate
             */
            bcm_bfd_endpoint_destroy(unit, endpoint_info->id);
        }
    }

    if (BCM_SUCCESS(rv) && 
        !(endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE)) {
        rv = oam_group_ep_list_add(unit, 
                                   0, /* CRA endpoint_info->group, */
                                   &hash_data->list_node);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to add endpoint id 0x%x to group "
                                   "tracking state: %d %s\n"), 
                       endpoint_info->id, rv, bcm_errmsg(rv)));
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (BCM_SUCCESS(rv)) {
        
        /* rv = oam_bfd_ep_store(unit, endpoint_info); */
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
exit:
    if(BCM_FAILURE(rv)) {
        if(egrPath.allocated) {
            _oam_egr_path_free(unit, &egrPath);
        }
    }

    OAM_UNLOCK(unit);
    
    return rv;
}


/*
 *   Function
 *      bcm_fe2000_bfd_endpoint_destroy
 *   Purpose
 *      Destroy a bfd endpoint and all allocated resources  with the given 
 *      endpoint id
 *   Parameters
 *       unit           = BCM device number
 *       endpoint       = endpoint ID to destroy
 *   Returns
 *       BCM_E_*
 *  Notes:
 *  BFD over LSP over trunk notes:
 *    1/ There is no gport associated with a LSP. We get to know if there is 
 *       a trunk underneath the LSP when we fetch the tunnel egress info
 *       None of the code under BCM_GPORT_IS_TRUNK is applicable in this case.
 *
 *    2/ Application can optionally specify a port within the trunk group 
 *       for BFD traffic using the trunk_index variable of the endpoint_info
 *       If incorrectly specified this defaults to index 0
 *
 *    3/ The selected port determines the QID to be used for BFD, this is 
 *       setup in _oam_egr_path_update. The endpoint->gport is internally 
 *       setup to reflect the MODPORT of the port over which the bfd traffic 
 *       has to be sent
 *
 *    4/ We also make sure that the endpoint is removed from the trunk_list
 *
 */
int
bcm_fe2000_bfd_endpoint_destroy(int unit, bcm_bfd_endpoint_t endpoint)
{
    int rv;
    bcm_bfd_endpoint_info_t ep_info;
    egr_path_desc_t egrPath;
    oam_sw_hash_key_t hash_key;
    oam_sw_hash_data_t *hash_data = NULL;
    bcm_trunk_t tid = 0;
    _oam_trunk_data_t *trunk_data = NULL;
    bcm_trunk_add_info_t trunk_info;
    int is_trunk = 0;

    switch(SOC_SBX_CONTROL(unit)->ucodetype) {
#ifdef BCM_FE2000_P3_SUPPORT
    case SOC_SBX_UCODE_TYPE_G2P3:
        if (!BCM_OAM_IS_INIT(unit)) {
            return BCM_E_NONE;
        }
        break;
#endif
    default:
        SBX_UNKNOWN_UCODE_WARN(unit);
        return BCM_E_CONFIG;
    }

    /* bfd_endpoint_get will clear the ackrdi bit in the ucode oamep table entry,
     * if ep_info.clear_persistent_faults & BCM_BFD_ENDPOINT_FAULT_REMOTE is true.
     * so, ep_info can not be used uninitialized with the bfd_endpoint_get function.
     * always clear out ep_info before sending it into bfd_endpoint_get.
     */
    bcm_bfd_endpoint_info_t_init(&ep_info);
    rv = bcm_fe2000_bfd_endpoint_get(unit, endpoint, &ep_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to get endpoint info for ep 0x%x: %d %s\n"),
                   endpoint, rv, bcm_errmsg(rv)));
        return rv;
    }

    /* For BFD, verify remote is destroyed before local endpoint */
    if (OAM_IS_BFD(ep_info.type)) {
        if (OAM_GROUP_EP_LIST_EMPTY(unit, 0 /* CRA ep_info.group */)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Group has no Endpoints associated. \n")));
            return BCM_E_INTERNAL;

        } else {
            dq_p_t ep_elem;
            uint8 local_ep_count = 0, peer_ep_count = 0;

            /* CRA removed ep_info.group) */
            DQ_TRAVERSE(&OAM_GROUP_EP_LIST(unit, 0), ep_elem) {
                oam_sw_hash_data_t *hash_data = NULL;
                _BCM_FE2000_OAM_GET_EP_DATA_FROM_LIST(ep_elem, hash_data);

                if (hash_data->flags & BCM_BFD_ENDPOINT_REMOTE) {   
                    peer_ep_count++;
                } else {
                    local_ep_count++;
                }
            } DQ_TRAVERSE_END(&trunk_data->endpoint_list, ep_elem);

            if (ep_info.flags & BCM_BFD_ENDPOINT_REMOTE) {
                if(!local_ep_count) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Local Endpoing must Exist."
                                           " Peer must be destroyed before local endpoint\n")));
                    return BCM_E_INTERNAL;

                } else if (!peer_ep_count) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Missing Remote Endpoint!!!\n")));
                    return BCM_E_PARAM;      
    
                }      
            } else {
                /* local endpoint */
                if (!local_ep_count) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Missing Local Endpoint!!!\n")));
                    return BCM_E_PARAM;

                } else if (peer_ep_count) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Peer BFD endpoint must be destroyed" 
                                           " before local endpoint destroy!!!\n")));
                    return BCM_E_PARAM;
                }
            }
        }
    }

    if (!(ep_info.flags & BCM_BFD_ENDPOINT_REMOTE)) {
        rv = _oam_egr_path_get(unit, &egrPath, ep_info.id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to get egress path for epi=0x%x\n"),
                       ep_info.id));
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "ep_info.flags=0x%x\n"),
                       ep_info.flags));
            return rv;
        }
    }

    _init_bfd_ep_hash_key(unit, hash_key, &ep_info);

    shr_htb_find(_state[unit]->mamep_htbl, hash_key,
                 (shr_htb_data_t *)&hash_data,
                 0 /* do not remove */);

    if ((hash_data) && OAM_TID_VALID(hash_data->lsp_trunk_id)) {
        /* This is an oam ep, over an lsp over a trunk */
        tid = hash_data->lsp_trunk_id;
        is_trunk = 1;
    }

    if (is_trunk) {
        trunk_data = &(_state[unit]->trunk_data[tid]);

        /* need to save a copy of trunk_info (trunk_add_info)
         * because we may remove the trunk_data structure.
         */
        sal_memcpy(&trunk_info, &(trunk_data->add_info), sizeof(bcm_trunk_add_info_t));

        /* if the endpoint is created on a trunk, then remove the
         * endpoint from the trunk's list.  if trunk's list is empty, 
         * then remove the trunk entry. */
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "Remove endpoint from trunk list\n")));
        rv = _oam_remove_ep_from_trunk_list(unit, tid, &hash_data->trunk_list_node);
        if (DQ_EMPTY(&trunk_data->endpoint_list)) {
            _oam_delete_trunk_info(unit, tid);
        }
    }

    if (OAM_IS_BFD(ep_info.type) || OAM_IS_LSP_BFD(ep_info.type)) {
        rv = _oam_bfd_endpoint_delete(unit, &ep_info);

    } else {
        rv = BCM_E_BADID;
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to destroy endpoint: %d %s\n"), 
                   rv, bcm_errmsg(rv)));
        /* non-fatal, keep trying */
    }

    if (!(ep_info.flags & BCM_BFD_ENDPOINT_REMOTE)) {
        _oam_egr_path_free(unit, &egrPath);
    }

    if (!is_oam_ep_id_app_reserved(unit, ep_info.id)) {
        shr_idxres_list_free(_state[unit]->ep_pool, ep_info.id);
    }

    shr_htb_find(_state[unit]->mamep_htbl, hash_key,
                 (shr_htb_data_t *)&hash_data,
                 1 /* remove */);

    if (hash_data) {
        oam_group_ep_list_remove(unit, 0 /* CRA ep_info.group */, &hash_data->list_node);
        _bcm_tcal_free(unit, &hash_data->tcal_id);
        _oam_hash_data_clear(hash_data);
    }

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "Endpoint 0x%x destroyed\n"),
                 endpoint));

    return rv;
}

/*
 *   Function
 *      bcm_fe2000_bfd_endpoint_destroy_all
 *   Purpose
 *      Destroy all bfd endpoints associated with the given unit
 *   Parameters
 *       unit         = BCM device number
 *   Returns
 *       BCM_E_*
 *  Notes:
 */
int
bcm_fe2000_bfd_endpoint_destroy_all(int unit)
{
    int rv;
    dq_p_t ep_elem;
    /* CRA, going to need a default BFD group for awhile */
    bcm_oam_group_t group = 0;

    switch(SOC_SBX_CONTROL(unit)->ucodetype) {
#ifdef BCM_FE2000_P3_SUPPORT
    case SOC_SBX_UCODE_TYPE_G2P3:
        if (!BCM_OAM_IS_INIT(unit)) {
            return BCM_E_NONE;
        }
        break;
#endif
    default:
        SBX_UNKNOWN_UCODE_WARN(unit);
        return BCM_E_CONFIG;
    }

    rv = shr_idxres_list_elem_state(_state[unit]->group_pool, group);

    if (rv != BCM_E_EXISTS) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "oam group id %d does not exist!\n"),
                   group));
        return BCM_E_NOT_FOUND;
    }

    /* iterate through all the endpoints in the group and destroy */
    rv = BCM_E_NONE;
    OAM_LOCK(unit);
    if(!OAM_GROUP_EP_LIST_EMPTY(unit, group)) {
        DQ_TRAVERSE(&OAM_GROUP_EP_LIST(unit,group), ep_elem) {
            int tmp_rv;
            oam_sw_hash_data_t *hash_data = NULL;
            _BCM_FE2000_OAM_GET_EP_DATA_FROM_LIST(ep_elem, hash_data);
            
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Destroying endpoint 0x%x\n"),
                         hash_data->rec_idx));
            
            tmp_rv = bcm_fe2000_bfd_endpoint_destroy(unit, hash_data->rec_idx);
            if (BCM_FAILURE(tmp_rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Error destrying endpoint 0x%x: %d %s\n"), 
                           hash_data->rec_idx, rv, bcm_errmsg(rv)));
                
                /* save the first error, and keep trying */
                if (BCM_SUCCESS(rv)) { 
                    rv  = tmp_rv; 
                }            
            }
        } DQ_TRAVERSE_END(&OAM_GROUP_EP_LIST(unit,group), ep_elem);
    }

    OAM_UNLOCK(unit);

    return rv;
}


/* the following functions not currently supported */

/* Register a callback for handling BFD events */
int bcm_fe2000_bfd_event_register(
    int unit, 
    bcm_bfd_event_types_t event_types, 
    bcm_bfd_event_cb cb, 
    void *user_data) {
    return BCM_E_NONE;
}

/* Unregister a callback for handling BFD events */
int bcm_fe2000_bfd_event_unregister(
    int unit, 
    bcm_bfd_event_types_t event_types, 
    bcm_bfd_event_cb cb) {
    return BCM_E_NONE;
}

/* 
 * Poll an BFD endpoint object.  Valid only for BFD sessions in Demand
 * Mode
 */
int bcm_fe2000_bfd_endpoint_poll(
    int unit, 
    bcm_bfd_endpoint_t endpoint) {
    return BCM_E_NONE;
}

/* Set SHA1 authentication entry */
int bcm_fe2000_bfd_auth_sha1_set(
    int unit, 
    int index, 
    bcm_bfd_auth_sha1_t *sha1) {
    return BCM_E_NONE;
}

/* Get SHA1 authentication entry */
int bcm_fe2000_bfd_auth_sha1_get(
    int unit, 
    int index, 
    bcm_bfd_auth_sha1_t *sha1) {
    return BCM_E_NONE;
}

/* Set Simple Password authentication entry */
int bcm_fe2000_bfd_auth_simple_password_set(
    int unit, 
    int index, 
    bcm_bfd_auth_simple_password_t *sp) {
    return BCM_E_NONE;
}

/* Get Simple Password authentication entry */
int bcm_fe2000_bfd_auth_simple_password_get(
    int unit, 
    int index, 
    bcm_bfd_auth_simple_password_t *sp) {
    return BCM_E_NONE;
}

int bcm_fe2000_bfd_endpoint_stat_get(
    int unit, 
    bcm_bfd_endpoint_t endpoint, 
    bcm_bfd_endpoint_stat_t *ctr_info, 
    uint8 clear) {
    return BCM_E_NONE;
}




int _bcm_fe2000_validate_oam_lsp_bfd_endpoint(int unit, 
                                          bcm_bfd_endpoint_info_t *endpoint_info)
{
    /* int index = 0;
    int tid = 0;
    bcm_trunk_add_info_t trunk_info; */
    int status = BCM_E_PARAM;

    if(!endpoint_info) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Null endpoint info\n")));

    } else if (endpoint_info->flags & ~_BCM_FE2000_BFD_SUPPORTED_FLAGS) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Unsupported Endpoint Flag: 0x%x\n"),
                   endpoint_info->flags));

    } else if (endpoint_info->type & ~_BCM_FE2000_BFD_SUPPORTED_EPTYPE) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Unsupported Endpoint Type: %d\n"),
                   endpoint_info->type));

    } else {
        status = BCM_E_NONE;
    }

    if(BCM_SUCCESS(status)) {
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE) {
            soc_sbx_g2p3_label2e_t  label2e;
    
            /* verify that if remote endpoint, associated mpls label is pop */
            if (!endpoint_info->label) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Remote Endpoints should have an associated POP label.\n")));
                return BCM_E_PARAM;
            }
                
            status = soc_sbx_g2p3_label2e_get(unit, endpoint_info->label,
                                              &label2e);
            if ((BCM_FAILURE(status)) ||\
                (!_BCM_FE2K_IS_LABEL_LER(unit,label2e.opcode))) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Remote Endpoints should have an associated POP label.\n")));
                return status;
            }
         
            /* verify if this local endpoint exist on the group */
            if (OAM_GROUP_EP_LIST_EMPTY(unit, 0 /* CRA endpoint_info->group */)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Group has no Endpoints associated. "
                                       "Local Endpoint must exist before creating"
                                       " remote endpointID\n")));
                return BCM_E_PARAM;
            } else {
                dq_p_t ep_elem;
    
                /* CRA removed ep_info.group) */
                DQ_TRAVERSE(&OAM_GROUP_EP_LIST(unit, 0), ep_elem) {
                    oam_sw_hash_data_t *hash_data = NULL;
                    _BCM_FE2000_OAM_GET_EP_DATA_FROM_LIST(ep_elem, hash_data);
                    if (!(hash_data->flags & BCM_BFD_ENDPOINT_REMOTE)) {
                       if(hash_data->rec_idx != endpoint_info->local_id) {
                          LOG_ERROR(BSL_LS_BCM_OAM,
                                    (BSL_META_U(unit,
                                                "Local Endpoint does not match with one on group\n")));
                          status = BCM_E_PARAM;
                          break;
                       }
                    }
                } DQ_TRAVERSE_END(&OAM_GROUP_EP_LIST(unit, 0), ep_elem);
                /* CRA removed endpoint_info->group */
            }
        } else {
           if (endpoint_info->egress_if) {
               status = _bcm_fe2000_get_tunnel_interface_info(unit, 
                                                   endpoint_info->egress_if,
                                                   _FE2K_OUTGOING_LSP);
           } else {
               LOG_ERROR(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Invalid Egress Interface specified\n")));
               status = BCM_E_PARAM;
           }
           /* Validate trunk_index
           CRA, not sure how trunk is handled on MPLS-TP or the new BFD API
           if (OAM_MPLS_LSP_OUT_EGR_IF(unit).flags & BCM_L3_TGID) {
                tid = OAM_MPLS_LSP_OUT_EGR_IF(unit).trunk;
                status = bcm_fe2000_trunk_get_old(unit, tid, &trunk_info);
                if (status != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Could not retreive trunk info for trunkId %d\n"),
                               tid));
                    status = BCM_E_PARAM;
                } else {
                    index = endpoint_info->trunk_index;
                    if ((index > trunk_info.num_ports) || (index < 0)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Invalid trunk_index %d for trunk %d\n"),
                                   index, tid));
                        status = BCM_E_PARAM;
                    }
                }
            }
           */
        }
    }
    
    return status;
}

int _bcm_fe2000_validate_oam_bfd_endpoint(int unit, 
                                          bcm_bfd_endpoint_info_t *endpoint_info)
{
    int status = BCM_E_PARAM;

    

    if(!endpoint_info) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Null endpoint info\n")));

    } else if (endpoint_info->flags & ~_BCM_FE2000_BFD_SUPPORTED_FLAGS) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Unsupported Endpoint Flag: 0x%x\n"),
                   endpoint_info->flags));

    } else if (endpoint_info->type & ~_BCM_FE2000_BFD_SUPPORTED_EPTYPE) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Unsupported Endpoint Type: %d\n"),
                   endpoint_info->type));

    } else if (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "BFD Supported only on MPLS Gport\n")));

    } else {
        status = BCM_E_NONE;
    }

    if(BCM_SUCCESS(status) && (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE)) {
        /* verify if this local endpoint exist on the group */
      if (OAM_GROUP_EP_LIST_EMPTY(unit, 0 /* CRA endpoint_info->group */)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Group has no Endpoints associated. "
                                   "Local Endpoint must exist before creating"
                                   " remote endpointID\n")));
            return BCM_E_PARAM;
        } else {
            dq_p_t ep_elem;

            /* CRA removed endpoint_info->group */
            DQ_TRAVERSE(&OAM_GROUP_EP_LIST(unit, 0), ep_elem) {
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
    return status;
}

int _oam_bfd_oamep_validate_test(int unit, int remote, int endpoint_id, soc_sbx_g2p3_oamep_t *oamep)
{
    int   rv = BCM_E_NONE;
    int   oamep_status, timeout, error;
    soc_sbx_g2p3_oamep_t oamep_copy;

    oamep_status = 1;
    timeout = 5;
    error = 0;

    /* loop through timeout times or until values match */
    while (oamep_status && (timeout > 0) && (error == 0)) {
        timeout--;
        rv = soc_sbx_g2p3_oamep_set(unit, endpoint_id, oamep);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to write oamep 0x%x: %d %s\n"),
                       endpoint_id, rv, bcm_errmsg(rv)));
            error = 1;
        } else {
            rv = soc_sbx_g2p3_oamep_get(unit, endpoint_id, &oamep_copy);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to get oamep 0x%x: %d %s\n"),
                           endpoint_id, rv, bcm_errmsg(rv)));
                error = 1;
            }
        }
        if (remote) {
            if (oamep->policerid == oamep_copy.policerid) {
                oamep_status = 0;
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "_oam_bfd_oamep_validate_test successful %x for %x\n"),
                             oamep->policerid, oamep_copy.policerid));
            } else {
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Retry to write oamep 0x%x: timeout %d. %x for %x\n"),
                          endpoint_id, timeout, oamep->policerid, oamep_copy.policerid));
            }
        } else {
            if (oamep->localstate == oamep_copy.localstate) {
                oamep_status = 0;
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "_oam_bfd_oamep_validate_test successful (localstate = %x). \n"),
                             oamep->localstate));
            } else {
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Retry to write oamep 0x%x: timeout %d. %x for %x\n"),
                          endpoint_id, timeout, oamep->localstate, oamep_copy.localstate));
            }
        }
    }
    if (timeout == 0) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to write oamep TIMEOUT 0x%x: %d %s\n"),
                   endpoint_id, rv, bcm_errmsg(rv)));
        global_oam_sdk = global_oam_sdk + 1;
    }
    return rv;
}

int _oam_bfd_endpoint_set(int unit, bcm_bfd_endpoint_info_t *ep_info, 
                          uint32 ep_rec_index, egr_path_desc_t *egrPath, 
                          tcal_id_t *tcal_id, uint32 label)
{
    int                               rv = BCM_E_NONE, rv2 = BCM_E_NONE;
    uint32                          watchdog_id = INVALID_POLICER_ID;
    uint32                            bfd_type=0, ep_type=0;
    soc_sbx_g2p3_oamep_t              oamep;
    bcm_oam_group_info_t             *group_info = NULL;
    oam_sw_hash_data_t               *hash_data = NULL;
    uint32                            lsmIdx = 0;
    soc_sbx_g2p3_lsmac_t              lsm;
    soc_sbx_g2p3_label2e_t            label2e;
    soc_sbx_g2p3_lp_t                 label_lp;
    int                               started = 0;

    /* sanity check pointers */
    if ((ep_info==NULL) || (egrPath==NULL) || (tcal_id==NULL)) {
        goto exit;
    }

    /* group */
    group_info = OAM_GROUP_INFO(unit, 0 /* CRA ep_info->group */);

    hash_data = &_state[unit]->hash_data_store[ep_rec_index];

    soc_sbx_g2p3_oam_function_bfd_get(unit, &bfd_type);
    if (OAM_IS_BFD(ep_info->type)) {
        soc_sbx_g2p3_oam_type_mpls_pwe_get(unit, &ep_type);
    } else {
        soc_sbx_g2p3_oam_type_mpls_lsp_get(unit, &ep_type);
    }

    soc_sbx_g2p3_oamep_t_init(&oamep);
    soc_sbx_g2p3_label2e_t_init(&label2e);

    if (ep_info->flags & BCM_BFD_ENDPOINT_UPDATE) {
        rv = soc_sbx_g2p3_oamep_get(unit, ep_rec_index, &oamep);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to get oamep 0x%x: %d %s\n"),
                       ep_rec_index, rv, bcm_errmsg(rv)));
            goto exit;
        }

        /* the soc layer doesn't handle overlays very well.
         * Must clear all fields not related to this type to ensure
         * bits are set as expected
         */
        _oam_oamep_prepare(unit, &oamep);
    }

    /************* PEER entry ****************/
    if (ep_info->flags & BCM_BFD_ENDPOINT_REMOTE) {

        rv = soc_sbx_g2p3_oamep_get(unit, ep_info->local_id, &oamep);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to get oamep 0x%x: %d %s\n"),
                       ep_info->local_id, rv, bcm_errmsg(rv)));
            return rv;
        }

        _oam_oamep_prepare(unit, &oamep);

        if (ep_info->flags & BCM_BFD_ENDPOINT_UPDATE) {

            /* free the timer and reallocate to simulate a reset */
            if (BCM_SUCCESS(rv) && oamep.policerid != INVALID_POLICER_ID) {
                rv = _oam_timer_free(unit, &oamep.policerid);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to free watchdog timer 0x%x: %d %s\n"),
                               oamep.policerid, rv, bcm_errmsg(rv)));
                    /* not a fatal error.  */
                    rv = BCM_E_NONE;
                    oamep.policerid = INVALID_POLICER_ID;
                }
            }
        } 

        /* only peer/remote endpoints require watchdog timers */
        if (ep_info->local_min_rx && ep_info->remote_detect_mult) {

            /* Always create a new WD timer.  But dont set the started
             * flag unless the host app sets CCM_RX.
             */
            started = !!(ep_info->flags & BCM_BFD_ENDPOINT_RX_ENABLE);
            rv = _oam_timer_allocate(unit, ep_rec_index,
                                     ep_info->local_min_rx,
                                     ep_info->remote_detect_mult,
                                     started,
                                     &watchdog_id);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to allocated watchdog timer of period %d"
                                       " for epId 0x%x: %d %s\n"),
                           ep_info->local_min_rx * ep_info->remote_detect_mult,
                           ep_rec_index, 
                           rv, bcm_errmsg(rv)));
                goto exit;
            }
        } else {
            LOG_WARN(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Receive watchdog timer not allocated epId 0x%x, rx_interval %d detect_mult %d \n"),
                      ep_rec_index, ep_info->local_min_rx,
                      ep_info->remote_detect_mult));
        }

        /* If remote endpoint, configure local ep idx to label2e Logical port */
        if (!(ep_info->flags & BCM_BFD_ENDPOINT_UPDATE)) {

           rv = soc_sbx_g2p3_label2e_get(unit, label, &label2e);
           if  (BCM_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Failed to get label2e 0x%x: %d %s\n"),
                          label, rv, bcm_errmsg(rv)));
               goto exit;
           }

           if (!label2e.lpidx) {
               rv = BCM_E_INTERNAL;
               LOG_ERROR(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Label2e for label 0x%x has not associate logical port\n"),
                          label));
               goto exit;

           } else {
               rv = soc_sbx_g2p3_lp_get(unit, label2e.lpidx, &label_lp);
               if  (BCM_FAILURE(rv)) {
                   LOG_ERROR(BSL_LS_BCM_OAM,
                             (BSL_META_U(unit,
                                         "Failed to get lpi 0x%x: %d %s\n"),
                              label2e.lpidx, rv, bcm_errmsg(rv)));
                   goto exit;

               } else {
                   /* if psc endpoint exists on the LSP, associate it to bfd endpoint */
                   if (label_lp.oamepi) {
                       soc_sbx_g2p3_oamep_t pscep;
                       soc_sbx_g2p3_oamep_t_init(&pscep); 
                       rv = soc_sbx_g2p3_oamep_get(unit, label_lp.oamepi, &pscep);
                       if (BCM_FAILURE(rv)) {
                           LOG_ERROR(BSL_LS_BCM_OAM,
                                     (BSL_META_U(unit,
                                                 "Failed to get oamep 0x%x: %d %s\n"),
                                      label_lp.oamepi, rv, bcm_errmsg(rv)));
                           goto exit;
                       } else {
                           _oam_oamep_prepare(unit, &pscep);
                           if (pscep.function != _state[unit]->ep_type_ids.psc) {
                               LOG_ERROR(BSL_LS_BCM_OAM,
                                         (BSL_META_U(unit,
                                                     "Unsupported chaining endpoint 0x%d\n"),
                                          pscep.function));
                               goto exit;
                           } else {
                               /* chain bfd to psc endpoint */
                               oamep.pscentry  = label_lp.oamepi;
                               label_lp.oamepi = ep_info->local_id;
                           }
                       }
                   }

                   label_lp.oamepi = ep_info->local_id;
                   rv = soc_sbx_g2p3_lp_set(unit, label2e.lpidx, &label_lp);
                   if  (BCM_FAILURE(rv)) {
                       LOG_ERROR(BSL_LS_BCM_OAM,
                                 (BSL_META_U(unit,
                                             "Failed to set lpi 0x%x: %d %s\n"),
                                  label2e.lpidx, rv, bcm_errmsg(rv)));
                       goto exit;

                   }    
               }
           }       
       }

        oamep.peerdetectmulti = ep_info->remote_detect_mult;
        oamep.yourdiscrim     = ep_info->remote_discr;
        oamep.peerstate       = ep_info->remote_state;
        oamep.policerid       = watchdog_id; /* watchdog_id is initialized to INVALID for local EP */
        /* Note: Currently TX/RX time must be same so ignore setting time interval on EP */

    } else {  /************* LOCAL entry ****************/

       if (ep_info->flags & BCM_BFD_ENDPOINT_UPDATE) {
           lsmIdx = ~0;
           rv = _sbx_gu2_ismac_idx_free(unit, hash_data->mac_address,
                                        _SBX_GU2_RES_UNUSED_PORT, &lsmIdx);
           LOG_VERBOSE(BSL_LS_BCM_OAM,
                       (BSL_META_U(unit,
                                   "freed mac " L2_6B_MAC_FMT " at %d on replace: "
                                    "%s\n"),
                        L2_6B_MAC_PFMT(hash_data->mac_address), lsmIdx, 
                        bcm_errmsg(rv)));
           if (rv == BCM_E_EMPTY) {
               rv = BCM_E_NONE;
           }
       } 

       /* Add an LSM entry (for both MEPs and MIPs) */
       soc_sbx_g2p3_lsmac_t_init (&lsm);
       rv = _sbx_gu2_ismac_idx_alloc(unit, 0, ep_info->src_mac_address, 
                                     _SBX_GU2_RES_UNUSED_PORT, &lsmIdx);
       
       if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Failed to allocate local station MAC idx\n")));
           goto exit;
       } else {
           LOG_VERBOSE(BSL_LS_BCM_OAM,
                       (BSL_META_U(unit,
                                   "Stored mac " L2_6B_MAC_FMT " at %d\n"),
                        L2_6B_MAC_PFMT(ep_info->src_mac_address), lsmIdx));

           hash_data->lsm_idx = lsmIdx;
       }

       sal_memcpy (lsm.mac, ep_info->src_mac_address, sizeof(bcm_mac_t));
       lsm.useport = 0;
       rv = soc_sbx_g2p3_lsmac_set (unit, lsmIdx, &lsm);
       
       if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Failed to write local station MAC\n")));
           goto exit;
       }
       
       oamep.function      = bfd_type;
       oamep.type          = ep_type;
       /* these "to-host" flags are for both CCM and BFD endpoints for as
        * long as the BFD and CCM share the same endpoint_info structure.
        * when/if BFD and CCm are seperated, there will be new BFD specific
        * flags.
        */
       oamep.conttohost    = (ep_info->flags & BCM_BFD_ENDPOINT_FIRST_RX_COPY_TO_CPU)?1:0;
       oamep.singletohost  = (ep_info->flags & BCM_BFD_ENDPOINT_ALL_RX_COPY_TO_CPU)?1:0;
       /* CRA, oamep.mepid         = ep_info->name; */
       oamep.mode          = (ep_info->flags & BCM_BFD_ENDPOINT_PWE_ACH)?0:1;
       oamep.localstate    = ep_info->local_state;
       oamep.diag          = ep_info->local_diag;
       oamep.poll          = (ep_info->flags & BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE)?1:0;
       /* CRA new API doesnt support
       if (ep_info->flags & BCM_OAM_BFD_FINAL) {
          oamep.final = 1;
       }
       */
       /* CRA, CSP 339509 & SDK-31001 discuss the handling of the Final bit.
        *      It was agreed that when the RX ucode receives a Poll Message,
        *      the ucode will NOT automatically send a Final Message.  Rather,
        *      the Poll message must be sent to the host app for processing.  The
        *      host app must then update the endpoint_info with the new info in
        *      the Poll message, plus the host app will also set the Final Flag.
        *      The SDK will set the "final" ucode flag when the API FINAL flag is
        *      set.  The ucode's "final" flag will tell the TX ucode to transmit
        *      one and only one Final messsage, then auto-clear the final flag.
        *      The oamep.final flag is used to transmit Final to peer.
        */

       /* CRA, the new BFD API does not have flags for these bits.
       oamep.c             = (ep_info->flags & BCM_OAM_BFD_CONTROL_INDEPENDENT)?1:0;
       oamep.a             = (ep_info->flags & BCM_OAM_BFD_OAM_BFD_AUTHENTICATE)?1:0;
       oamep.d             = (ep_info->flags & BCM_OAM_BFD_DEMAND_MODE)?1:0;
       oamep.m             = (ep_info->flags & BCM_OAM_BFD_MULTIPOINT)?1:0;
       oamep.unibi         = (ep_info->flags & BCM_OAM_BFD_UNIDIRECTIONAL)?0:1;
       */
       /* 0=uni, 1=bi-directional. */

       /* The API fields min_rx_interval & min_tx_interval are in usec. */
       oamep.rxtxinterval  = ep_info->local_min_echo; /* advertised rate (in usec) */
       oamep.udpsourceport = ep_info->udp_src_port;
       oamep.ipda          = ep_info->dst_ip_addr;
       /* IPSA - push down global to ucode */
       /* UDP dest port - push down global to ucode */
       /* Control independent - hardwired to 0 in ucode */
       oamep.localdetectmulti = ep_info->local_detect_mult;
       oamep.mydiscrim  = ep_info->local_discr;

       /* MPLS-TP OAM Exp and TTL is taken from the encapsulating PW or LSP.
        * they do not use either the pkt_pri or int_pri fields from the endpoint_info.
        * only the egr_map is used to specify the Remark Table Index.
        */
       /* CRA, new API doesnt support RDI flags
       oamep.rdi      = !!(group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX);
       */
       oamep.ftidx    = egrPath->ftIdx;

       /* MPLS-TP OAM is configured on Provider mode ports.  Egress remarking
        * is used to set Exp.  The remark table is a 1-to-1 mapping of rcos
        * to exp.  the remark table index is taken from endpoint_info->egr_map.
        * the rcos is set into the ucode's intpripm field from the encapsulating
        * PW or LSP Exp (which has been stashed in this egrPath ETE field).
        * egrPath->eteEncap.exp2 is only used to copy the rcos into the ucode
        * ep entry, as MPLS-TP OAM is not currently configured on Customer mode
        * ports.
        */
       /* CSP 404126, config RCOS per endpoint through OAM API's int_pri. */
       oamep.intpripm = (ep_info->int_pri & 0x7);

    } /* end LOCAL entry */

    /* the remaining calls are common to local and peer/remote endpoint
     * configuration
     */
#if 0
    rv = soc_sbx_g2p3_oamep_set(unit, 
                                (ep_info->flags & BCM_BFD_ENDPOINT_REMOTE)?\
                                ep_info->local_id:ep_rec_index,
                                &oamep);
#else
    rv = _oam_bfd_oamep_validate_test(unit, !!(ep_info->flags & BCM_BFD_ENDPOINT_REMOTE),
                                      (ep_info->flags & BCM_BFD_ENDPOINT_REMOTE)?\
                                      ep_info->local_id:ep_rec_index,
                                      &oamep);
#endif
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to write oamep 0x%x: %d %s\n"),
                   ep_rec_index, rv, bcm_errmsg(rv)));

    } else {
        if (!(ep_info->flags & BCM_BFD_ENDPOINT_REMOTE)) {

            /* Set up the timer calendar to trigger the transmission of
             * OAM packets.
             * ! the endpoint entry MUST be added before these tcal entries        !
             * ! because they (tcal) will trigger the start of packet transmission !
             * ! which reads the endpoint entries.  order matters.                 !
             */

            int tx_enable = !!(ep_info->flags & BCM_BFD_ENDPOINT_TX_ENABLE);

            if (ep_info->flags & BCM_BFD_ENDPOINT_UPDATE) {
                *tcal_id = _state[unit]->hash_data_store[ep_rec_index].tcal_id;
                rv = _bcm_tcal_update(unit, ep_info->local_min_tx, tx_enable,
                                      ep_rec_index, tcal_id);
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "Timer Calendar: 0x%x Updated for EP[0x%x] txEnable[%d]\n"),
                             *tcal_id, ep_rec_index, tx_enable));
            } else {
                rv = _bcm_tcal_alloc(unit, ep_info->local_min_tx, tx_enable,
                                     ep_rec_index, tcal_id);
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "Timer Calendar: 0x%x Allocated for EP[0x%x] txEnable[%d]\n"),
                             *tcal_id, ep_rec_index, tx_enable));
            }

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to write/update timer calendar 0x%x: %d %s\n"),
                           *tcal_id, rv, bcm_errmsg(rv)));
            } else {
                /* Insert the local endpoint into L3 interface OAM endpoint list */
                if (!(ep_info->flags & BCM_BFD_ENDPOINT_UPDATE)) {
                    /* If MPLS LSP related endpoints, hang it to interface endpoint list */
                    if (bcmBFDTunnelTypeMplsTpLspCc == ep_info->type || \
                        bcmBFDTunnelTypeMplsTpLspCcCv == ep_info->type) {
                        rv = _bcm_fe2000_lsp_intf_ep_list_add(unit, ep_info->egress_if, hash_data); 
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_BCM_OAM,
                                      (BSL_META_U(unit,
                                                  "Failed to insert oam into interface endpoint list: %d %s\n"),
                                       rv, bcm_errmsg(rv)));
                        }
                    }
                }                
            }
        }
    }

 exit:
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "failed: err=%d %s\n"),
                   rv, bcm_errmsg(rv)));

        if (ep_info->flags & BCM_BFD_ENDPOINT_REMOTE) {         

            if (oamep.policerid != INVALID_POLICER_ID) {
                rv2 = _oam_timer_free(unit, &oamep.policerid);
                if (BCM_FAILURE(rv2)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to free watchdog timer 0x%x: %d %s\n"),
                               oamep.policerid, rv2, bcm_errmsg(rv2)));
                } 

                /* remove policer from db record on error after it's been commmited to
                 * avoid resource leaks and pointers to free'd policers
                 */
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "clearing timers in oamep\n")));
                oamep.policerid = INVALID_POLICER_ID;
                rv2 = soc_sbx_g2p3_oamep_set(unit, ep_info->local_id, &oamep);  
                if (BCM_FAILURE(rv2)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to write oamep 0x%x: %d %s\n"),
                               ep_info->local_id, rv2, bcm_errmsg(rv2)));
                }
            }

            if (!(ep_info->flags & BCM_BFD_ENDPOINT_UPDATE)) {
                if (label2e.lpidx) {
                    label_lp.oamepi = 0;
                    rv2 = soc_sbx_g2p3_lp_set(unit, label2e.lpidx, &label_lp);
                    if  (BCM_FAILURE(rv2)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Failed to set lpi 0x%x: %d %s\n"),
                                   label2e.lpidx, rv2, bcm_errmsg(rv2)));
                    } 
                }
            }

        } else {
            if(hash_data->lsm_idx == lsmIdx) {
                _sbx_gu2_ismac_idx_free(unit, hash_data->mac_address, _SBX_GU2_RES_UNUSED_PORT, &lsmIdx);
            }
        }
    }

    return rv;
}

int _oam_bfd_endpoint_get(int unit,
                          bcm_bfd_endpoint_info_t *ep_info, 
                          uint32 ep_rec_index)
{
    int status = BCM_E_NONE;
    oam_sw_hash_data_t *hash_data = NULL;
    uint32 oamep_idx = ep_rec_index;
    egr_path_desc_t egrPath;
    soc_sbx_g2p3_oamep_t oamep;

    if (ep_info && ENDPOINT_ID_VALID(unit,ep_rec_index)) {

        hash_data = &_state[unit]->hash_data_store[ep_rec_index];
        /* CRA ep_info->name       = hash_data->ep_name; */
        ep_info->flags      = hash_data->flags;
        ep_info->flags      = hash_data->flags;
        ep_info->type       = hash_data->type;
        /* CRA ep_info->group      = hash_data->group; */
        ep_info->gport      = hash_data->gport;
        /* CRA ep_info->vlan       = hash_data->vid_label; */
        /* CRA ep_info->name       = hash_data->ep_name; */
        ep_info->local_id   = hash_data->local_rec_idx;
        ep_info->label      = hash_data->vid_label;
        ep_info->egress_if  = hash_data->intf_id;
        /* CRA ep_info->trunk_index  = hash_data->trunk_index; */
        
        if (hash_data->flags & BCM_BFD_ENDPOINT_REMOTE) {
            oamep_idx = hash_data->local_rec_idx;
            ep_info->local_min_rx = hash_data->period;
            
        } else {
            status = _oam_egr_path_get(unit, &egrPath, oamep_idx);
            ep_info->local_min_tx = hash_data->interval;
        }
    
        if(BCM_SUCCESS(status)) {
            status = soc_sbx_g2p3_oamep_get(unit, oamep_idx, &oamep);
            if(BCM_SUCCESS(status)) {

                if (hash_data->flags & BCM_BFD_ENDPOINT_REMOTE) {
                  /* CRA not used ep_info->min_rx_interval     = oamep.rxtxinterval; */
                    ep_info->remote_detect_mult  = oamep.peerdetectmulti;
                    ep_info->remote_state        = oamep.peerstate;
                    ep_info->remote_discr        = oamep.yourdiscrim;

                } else {
                    ep_info->local_discr         = oamep.mydiscrim;
                    ep_info->local_min_echo      = oamep.rxtxinterval;
                    ep_info->local_detect_mult   = oamep.localdetectmulti;
                    ep_info->local_state         = oamep.localstate;
                    ep_info->local_diag          = oamep.diag;
                    if (OAM_IS_BFD(ep_info->type)) {
                        ep_info->udp_src_port        = oamep.udpsourceport;
                        ep_info->dst_ip_addr         = oamep.ipda;
                        soc_sbx_g2p3_oam_pwe_udp_dest_port_get(unit, 
                                           (uint32*)&ep_info->udp_src_port);
                        soc_sbx_g2p3_oam_pwe_ip_sa_get(unit, 
                                           &ep_info->src_ip_addr);
                    }
                    if(hash_data->lsm_idx) {
                        soc_sbx_g2p3_lsmac_t      lsm;
                        soc_sbx_g2p3_lsmac_get(unit, hash_data->lsm_idx, &lsm);
                        sal_memcpy(&ep_info->src_mac_address, lsm.mac, sizeof(bcm_mac_t));
                    }

                    sal_memcpy(ep_info->dst_mac_address, hash_data->mac_address,
                               sizeof(bcm_mac_t));

                    /* int_pri and pkt_pri are not used.  TTL & Exp marking
                     * is based on the encapsulating PW or LSP.  The Remark
                     * Table Index, however, is taken from endpoint_info->egr_map.
                     * MPLS-TP port mode is always Provider, so Exp is set in
                     * the label(s) from the egress remarking table.
                     */
                    /* CSP 404126, config RCOS per endpoint through OAM API's int_pri. */
                    ep_info->int_pri = oamep.intpripm;
                    ep_info->egr_map = egrPath.eteEncap.remark;
                }
            }
        }
    } else {
        status = BCM_E_PARAM;
    }

    return status;
}

int _oam_bfd_endpoint_delete(int unit, 
                             bcm_bfd_endpoint_info_t *ep_info)
{
    int status = BCM_E_PARAM;
    oam_sw_hash_data_t *hash_data = NULL;
    uint32 oamep_idx =0;
    soc_sbx_g2p3_oamep_t oamep;
    uint32 watchdog_id = INVALID_POLICER_ID;

    /* sanity check pointers */
    if (ep_info && ENDPOINT_ID_VALID(unit, ep_info->id)) {
        
        hash_data = &_state[unit]->hash_data_store[ep_info->id];

        oamep_idx = (hash_data->flags & BCM_BFD_ENDPOINT_REMOTE)? \
                     ep_info->local_id:ep_info->id;

        status  = soc_sbx_g2p3_oamep_get(unit, oamep_idx, &oamep);
        if (BCM_FAILURE(status)) {
            /* non-fatal error, keep trying */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to find EP record idx=%d err=%d %s\n"),
                       oamep_idx, status, bcm_errmsg(status)));
        }

        if (hash_data->flags & BCM_BFD_ENDPOINT_REMOTE) {
            soc_sbx_g2p3_label2e_t  label2e;
            soc_sbx_g2p3_lp_t       label_lp;

            watchdog_id = oamep.policerid;
            oamep.peerdetectmulti = 0;
            oamep.yourdiscrim     = 0;
            oamep.peerstate       = 0;
            oamep.policerid       = 0;

            status = soc_sbx_g2p3_label2e_get(unit, hash_data->vid_label, &label2e);
            if  (BCM_FAILURE(status)) {
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Failed to get label2e 0x%x: %d %s\n"),
                          hash_data->vid_label, status, bcm_errmsg(status)));

            } else {
                status = soc_sbx_g2p3_lp_get(unit, label2e.lpidx, &label_lp);
                if  (BCM_FAILURE(status)) {
                    LOG_WARN(BSL_LS_BCM_OAM,
                             (BSL_META_U(unit,
                                         "Failed to get lpi 0x%x: %d %s\n"),
                              label2e.lpidx, status, bcm_errmsg(status)));

                } else {
                    /* if PSC endpoint is chained, set lp to point to psc endpoint */
                    label_lp.oamepi = (oamep.pscentry)?oamep.pscentry:0;
                    status = soc_sbx_g2p3_lp_set(unit, label2e.lpidx, &label_lp);
                    if  (BCM_FAILURE(status)) {
                        LOG_WARN(BSL_LS_BCM_OAM,
                                 (BSL_META_U(unit,
                                             "Failed to set lpi 0x%x: %d %s\n"),
                                  label2e.lpidx, status, bcm_errmsg(status)));

                    }  
                }                
            }
        } else {
            uint32 unknownIdx = ~0;

            /* remove the LSM entry */
            status = _sbx_gu2_ismac_idx_free(unit, ep_info->src_mac_address, 
                                             _SBX_GU2_RES_UNUSED_PORT, &unknownIdx);
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
                }
            }

            if (BCM_FAILURE(status)) {
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Failed to delete local station MAC: %d %s\n"),
                          status, bcm_errmsg(status)));
            }
            /* 'clear' the entry */
            soc_sbx_g2p3_oamep_t_init(&oamep);        

            /* Remove the local endpoint into L3 interface OAM endpoint list */
            status = _bcm_fe2000_lsp_intf_ep_list_remove(unit, ep_info->egress_if, hash_data); 
            if (BCM_FAILURE(status)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to delete oam into interface endpoint list: %d %s\n"),
                           status, bcm_errmsg(status)));
            }
        }

        status = soc_sbx_g2p3_oamep_set(unit, oamep_idx, &oamep);
        if (BCM_FAILURE(status)) {
            /* non-fatal error, keep trying */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to set BFD oamEp idx=%d err=%d %s\n"),
                       oamep_idx, status, bcm_errmsg(status)));
        }
        
        if (watchdog_id != INVALID_POLICER_ID ) {
            _oam_timer_free(unit, &watchdog_id);
        }        


  
        status = BCM_E_NONE;
    }

    return status;
}

int _oam_bfd_egr_path_update (int unit,
                              egr_path_desc_t *egrPath,  
                              bcm_bfd_endpoint_info_t *endpoint_info,
                              _fe2k_vpn_sap_t *vpn_sap)
{
    int status = BCM_E_NONE;

    if (!egrPath || !endpoint_info || !vpn_sap) {
        status = BCM_E_PARAM;
    } else {
        status = _oam_egr_path_dmac_set(unit, egrPath, endpoint_info->dst_mac_address, TRUE);
         if (BCM_FAILURE(status)) {
             LOG_ERROR(BSL_LS_BCM_OAM,
                       (BSL_META_U(unit,
                                   "Failed to set dmac on egress path for EP %d\n"),
                        endpoint_info->id));

         } else {
             status = _oam_egr_path_smac_set(unit, egrPath, endpoint_info->src_mac_address);
             if (BCM_FAILURE(status)) {
                 LOG_ERROR(BSL_LS_BCM_OAM,
                           (BSL_META_U(unit,
                                       "Failed to set smac on egress path for EP %d\n"),
                            endpoint_info->id));

             } else {
                 egrPath->eteEncap.isid        = 0;
                 egrPath->eteEncap.mimtype     = 0;
                 egrPath->eteEncap.s2          = 0;
                 egrPath->eteEncap.exp2        = (vpn_sap->mpls_psn_label_exp & 0x7);
                 egrPath->eteEncap.label2      = (vpn_sap->mpls_psn_label & 0xfffff);
                 egrPath->eteEncap.remark      = (endpoint_info->egr_map & 0x7f);
                 egrPath->eteEncap.exp2remark  = 1;
                 egrPath->eteEncap.ttl2        = (vpn_sap->mpls_psn_label_ttl & 0xff);
                 egrPath->eteEncap.ttl2dec     = 0; /* use ttl from ETE */

                 /* CRA, For PW & LSP OAM, both the TTL and EXP will be taken
                  * from the encapsulating PW or LSP.
                  * Here is where we would modify the SDK to use the "int_pri" from
                  * the endpoint_info, if we wanted Exp from the endpoint rather
                  * than from the PW.
                  * MPLS-TP OAM sessions are created on Provider-mode ports.  So,
                  * the Exp marking will always be from the Remark Table.  Above
                  * we specify the exp2remark for the remark table index.  We
                  * also configure the egrPath->eteEncap.exp2 which would be used
                  * for Customer-mode if MPLS-TP OAM were configured on customer
                  * ports.  We must still configure the egrPath->eteEncap.exp2
                  * because it is used by _oam_bfd_endpoint_set to configure the
                  * rcos value in the ucode's intpri field.  The Remark Table Index
                  * must specify a 1-to-1 mapping from  rcos to exp.
                  */

                 egrPath->eteEncap.s1          = 1;
                 egrPath->eteEncap.exp1        = (vpn_sap->vc_mpls_port.egress_label.exp & 0x7);
                 egrPath->eteEncap.label1      = (vpn_sap->vc_mpls_port.match_label & 0xfffff);
                 egrPath->eteEncap.exp1remark  = 1;
                 egrPath->eteEncap.ttl1        = (vpn_sap->vc_mpls_port.egress_label.ttl & 0xff);
                 egrPath->eteEncap.ttl2dec     = 0; /* use ttl from ETE */

                 egrPath->eteEncap.tunnelenter = 1;
                 egrPath->eteEncap.encaplen    = 10;

                 egrPath->eteEncap.etype       = 0x8847;
                 egrPath->eteEncap.ttlcheck    = 0;
                 egrPath->eteEncap.mplsttldec  = 0;

                 /* modport or local gport */
                 egrPath->port                 = vpn_sap->vc_mpls_port.port; 
             }
         }
    }

    return status;
}

int _oam_fe2000_bfd_chain_psc (int      unit, 
                               uint32 bfd_ep_index,
                               uint32 psc_ep_index,
                               uint8  chain /* 0 -unchain, 1- chain*/)
{
    int status = BCM_E_NONE;
    soc_sbx_g2p3_oamep_t oamep;

    if(bfd_ep_index > _state[unit]->max_endpoints ||\
       psc_ep_index > _state[unit]->max_endpoints)  {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Invalid endpoint id.  Must supply endpoint ID \n")));
        status = BCM_E_PARAM;
    } else {
        soc_sbx_g2p3_oamep_t_init(&oamep);   
        status = soc_sbx_g2p3_oamep_get(unit, bfd_ep_index, &oamep);
        if (BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to get oamep 0x%x: %d %s\n"),
                       bfd_ep_index, status, bcm_errmsg(status)));
        } else {
            _oam_oamep_prepare(unit, &oamep);
            /* verify if this is a BFD endpoint */
            if (oamep.function != _state[unit]->ep_type_ids.bfd) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Invalid BFD endpoint id 0x%x \n"),
                           bfd_ep_index));
            } else {
                /* assumes psc ep index is valid */
                oamep.pscentry = (chain) ? psc_ep_index : 0;
                status = soc_sbx_g2p3_oamep_set(unit, bfd_ep_index, &oamep);
                if (BCM_FAILURE(status)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to write oamep 0x%x: %d %s\n"),
                               bfd_ep_index, status, bcm_errmsg(status)));
                }
            }
        }
    }   

    return status;
}


#else   /* INCLUDE_SBX_BFD */
int bcm_fe2000_oam_bfd_not_empty;

#if defined(INCLUDE_BFD)

#include <bcm_int/control.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm/error.h>
#include <bcm/bfd.h>
#include <bcm_int/sbx_dispatch.h>

int
bcm_fe2000_bfd_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_endpoint_create(
    int unit,
    bcm_bfd_endpoint_info_t *endpoint_info)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_endpoint_destroy(
    int unit,
    bcm_bfd_endpoint_t endpoint)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_endpoint_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_info_t *endpoint_info)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_endpoint_poll(
    int unit,
    bcm_bfd_endpoint_t endpoint)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_endpoint_stat_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_stat_t *ctr_info,
    uint8 clear)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_event_register(
    int unit,
    bcm_bfd_event_types_t event_types,
    bcm_bfd_event_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_event_unregister(
    int unit,
    bcm_bfd_event_types_t event_types,
    bcm_bfd_event_cb cb)
{
    return BCM_E_UNAVAIL;
}
int
bcm_fe2000_bfd_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
int 
bcm_fe2000_bfd_auth_sha1_set(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t *sha1) {
    return BCM_E_NONE;
}
int 
bcm_fe2000_bfd_auth_sha1_get(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t *sha1) {
    return BCM_E_NONE;
}
int 
bcm_fe2000_bfd_auth_simple_password_set(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t *sp) {
    return BCM_E_NONE;
}
int 
bcm_fe2000_bfd_auth_simple_password_get(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t *sp) {
    return BCM_E_NONE;
}

#endif

#endif  /* INCLUDE_SBX_BFD */
