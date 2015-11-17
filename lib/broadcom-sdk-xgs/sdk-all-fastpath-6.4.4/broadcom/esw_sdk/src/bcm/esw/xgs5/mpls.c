/*
 * $Id: mpls.c $
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
 * File:    mpls.c
 * Purpose: Manages Tomahawk, TD2+ MPLS functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#ifdef INCLUDE_L3
#ifdef BCM_MPLS_SUPPORT
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))

#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/triumph.h>

#include <bcm/error.h>
#include <bcm/mpls.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/port.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/failover.h>

#define MPLS_INFO(_unit_)   (&_bcm_tr_mpls_bk_info[_unit_])
#define L3_INFO(_unit_)   (&_bcm_l3_bk_info[_unit_])

STATIC int
_bcmi_xgs5_mpls_entry_get_key(int unit, mpls_entry_entry_t *ment,
                           bcm_mpls_tunnel_switch_t *info)
{
    bcm_port_t port_in, port_out;
    bcm_module_t mod_in, mod_out;
    bcm_trunk_t trunk_id;

    port_in = soc_MPLS_ENTRYm_field32_get(unit, ment, PORT_NUMf);
    mod_in = soc_MPLS_ENTRYm_field32_get(unit, ment, MODULE_IDf);
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, Tf)) {
        trunk_id = soc_MPLS_ENTRYm_field32_get(unit, ment, TGIDf);
        BCM_GPORT_TRUNK_SET(info->port, trunk_id);
    } else if ((port_in == 0) && (mod_in == 0)) {
        /* Global label, mod/port not part of lookup key */
        info->port = BCM_GPORT_INVALID;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(info->port, mod_out, port_out);
    } 
    info->label = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_LABELf);

    return BCM_E_NONE;
}

/* Convert data part of HW entry to application format. */
STATIC int
_bcmi_xgs5_mpls_entry_get_data(int unit, mpls_entry_entry_t *ment,
                            bcm_mpls_tunnel_switch_t *info)
{
    int rv, action, nh_index, vrf;
    bcm_if_t egress_if=0;
    int mode = 0;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_ingress_mode_get(unit, &mode));
    action = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_ACTION_IF_BOSf);
    switch(action) {
    case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_IIF:
        info->action = BCM_MPLS_SWITCH_ACTION_POP;
        if (!mode) {
            vrf = soc_MPLS_ENTRYm_field32_get(unit, ment, L3_IIFf);
            vrf -= _BCM_TR_MPLS_L3_IIF_BASE;
           _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
        } else {
            info->ingress_if = soc_MPLS_ENTRYm_field32_get(unit, ment, L3_IIFf);
        } 
        break;
    case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI:
        info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
        nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               NEXT_HOP_INDEXf);
        rv = bcm_tr_mpls_get_vp_nh (unit, nh_index,&egress_if);
        if (rv == BCM_E_NONE) {
            rv = bcm_tr_mpls_l3_nh_info_get(unit, info, nh_index);
        } else {
            info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN;
            info->egress_label.label = BCM_MPLS_LABEL_INVALID;
            rv = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN(rv);
        break;
    case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI:
        info->action = BCM_MPLS_SWITCH_ACTION_PHP;
        nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               NEXT_HOP_INDEXf);
        info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN;
        break;
    case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP:
        info->action = BCM_MPLS_SWITCH_ACTION_PHP;
        nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               ECMP_PTRf);
        info->egress_if = nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN;
        break;
    case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP:
        info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
        nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               ECMP_PTRf);
        info->egress_if = nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN;
        break;
    default:
        return BCM_E_INTERNAL;
        break;
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUM_VALIDf)) {
        info->flags |= BCM_MPLS_SWITCH_COUNTED;
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
     else if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, CLASS_IDf)) {
        info->class_id = soc_MPLS_ENTRYm_field32_get(unit, ment, CLASS_IDf);
    }
#endif

    if (!soc_MPLS_ENTRYm_field32_get(unit, ment, DECAP_USE_TTLf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_TTL;
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, DECAP_USE_EXP_FOR_INNERf)) {
        if (info->action != BCM_MPLS_SWITCH_ACTION_SWAP) {
            info->flags |= BCM_MPLS_SWITCH_INNER_EXP;
        }
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                    DECAP_USE_EXP_FOR_PRIf) == 0x1) {

        /* Use specified EXP-map to determine internal prio/color */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_MAP;
        info->exp_map = 
            soc_MPLS_ENTRYm_field32_get(unit, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    } else if (soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                           DECAP_USE_EXP_FOR_PRIf) == 0x2) {

        /* Use the specified internal priority value */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_SET;
        info->int_pri =
            soc_MPLS_ENTRYm_field32_get(unit, ment, NEW_PRIf);

        /* Use specified EXP-map to determine internal color */
        info->flags |= BCM_MPLS_SWITCH_COLOR_MAP;
        info->exp_map = 
            soc_MPLS_ENTRYm_field32_get(unit, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    }
    if (SOC_MEM_FIELD_VALID(unit,MPLS_ENTRYm,
                            DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
        if (soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                        DO_NOT_CHANGE_PAYLOAD_DSCPf) == 0) {
            if ((info->action == BCM_MPLS_SWITCH_ACTION_PHP) ||
                (info->action == BCM_MPLS_SWITCH_ACTION_POP)) {
                     info->flags |=
                         (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL);
            }
        }
    }
    return BCM_E_NONE;
}

/* Convert key part of application format to HW entry. */
STATIC int
_bcmi_xgs5_mpls_entry_set_key(int unit, bcm_mpls_tunnel_switch_t *info,
                           mpls_entry_entry_t *ment)
{
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int rv, gport_id;

    sal_memset(ment, 0, sizeof(mpls_entry_entry_t));

    if (info->port == BCM_GPORT_INVALID) {
        /* Global label, mod/port not part of lookup key */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MODULE_IDf, 0);
        soc_MPLS_ENTRYm_field32_set(unit, ment, PORT_NUMf, 0);
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_LABELf, info->label);
        } else {
            return BCM_E_PARAM;
        }
        soc_MPLS_ENTRYm_field32_set(unit, ment, VALIDf, 1);
        return BCM_E_NONE;
    }

    rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(info->port)) {
        soc_MPLS_ENTRYm_field32_set(unit, ment, Tf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, ment, TGIDf, trunk_id);
    } else {
        soc_MPLS_ENTRYm_field32_set(unit, ment, MODULE_IDf, mod_out);
        soc_MPLS_ENTRYm_field32_set(unit, ment, PORT_NUMf, port_out);
    }
    if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_LABELf, info->label);
    } else {
        return BCM_E_PARAM;
    }
    soc_MPLS_ENTRYm_field32_set(unit, ment, VALIDf, 1);
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_add
 * Purpose:
 *      Add an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info)
{
    mpls_entry_entry_t ment; 
    int mode=0, nh_index = -1, vrf=0, rv, num_pw_term, old_pw_cnt = -1, pw_cnt = -1;
    int index, action = -1, old_nh_index = -1, old_ecmp_index = -1;
    int  tunnel_switch_update=0;
    uint32 mpath_flag=0;
    int  ref_count=0;
    bcm_if_t  egress_if=0;

    rv = bcm_xgs3_l3_egress_mode_get(unit, &mode);
    BCM_IF_ERROR_RETURN(rv);
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    /* Check for Port_independent Label mapping */
    if (!BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
        return BCM_E_PARAM;
    }

    rv = bcm_tr_mpls_port_independent_range (unit, info->label, info->port);
    if (rv < 0) {
        return rv;
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(SOC_IS_TRIDENT2PLUS(unit)) {
        /*Ensure that class_id doesnt overcross CLASS_IDf*/
        int bit_num = 0;
        if((info->flags & BCM_MPLS_SWITCH_COUNTED) && (info->class_id)) {
            return BCM_E_PARAM;
        }
        if(info->class_id) {
            bit_num = soc_mem_field_length(unit, MPLS_ENTRYm, CLASS_IDf);
            if(info->class_id > (( 1 << bit_num ) - 1)) {
                return BCM_E_PARAM;
            }
        }
    }
#endif

    BCM_IF_ERROR_RETURN(_bcmi_xgs5_mpls_entry_set_key(unit, info, &ment));

    /* See if entry already exists */
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);

    /* default not to use DSCP from ING_MPLS_EXP_MAPPING table */
    if (SOC_MEM_FIELD_VALID(unit,MPLS_ENTRYm,DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
        soc_MPLS_ENTRYm_field32_set(unit, &ment,
                                          DO_NOT_CHANGE_PAYLOAD_DSCPf, 1);
    }

    if (rv == SOC_E_NONE) {
        /* Entry exists, save old info */
        tunnel_switch_update = 1;
        action = soc_MPLS_ENTRYm_field32_get(unit, &ment, MPLS_ACTION_IF_BOSf);
        if ((action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI) ||
                (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI)) {
            old_nh_index = soc_MPLS_ENTRYm_field32_get(unit, &ment, NEXT_HOP_INDEXf);
        } else if (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP) {
            old_ecmp_index = soc_MPLS_ENTRYm_field32_get(unit, &ment, ECMP_PTRf);
        } else if ((action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP) &&
                soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
            old_ecmp_index = soc_MPLS_ENTRYm_field32_get(unit, &ment, ECMP_PTRf);
        }

        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUM_VALIDf)) {
            old_pw_cnt = soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUMf);
        } 
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }

    switch(info->action) {
    case BCM_MPLS_SWITCH_ACTION_SWAP:
        if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
            if ((!soc_feature(unit, soc_feature_mpls_lsr_ecmp)) || 
                 (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if))) {
                return BCM_E_PARAM;
            }
        }
        break;
    case BCM_MPLS_SWITCH_ACTION_POP:
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_POP);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_IIF);
        /* uniform qos model if either of these two flags set.
         * Only apply to L3 MPLS and BOS
         */
        if (info->flags & (BCM_MPLS_SWITCH_OUTER_EXP |
                           BCM_MPLS_SWITCH_OUTER_TTL) ) {
            if (SOC_MEM_FIELD_VALID(unit,MPLS_ENTRYm,
                             DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment,
                                  DO_NOT_CHANGE_PAYLOAD_DSCPf, 0);
            }
        }

        break;
    case BCM_MPLS_SWITCH_ACTION_PHP:
        if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if)) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                    _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP);
            if (soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                        _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP);
            }
        } else if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                    _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                    _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI);
        } else {
            return BCM_E_PARAM;
        }
        /* uniform qos model if either of these two flags set.
         * Only apply to L3 MPLS and BOS
         */
        if (info->flags & (BCM_MPLS_SWITCH_OUTER_EXP |
                           BCM_MPLS_SWITCH_OUTER_TTL) ) {
            if (SOC_MEM_FIELD_VALID(unit,MPLS_ENTRYm,
                             DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment,
                                  DO_NOT_CHANGE_PAYLOAD_DSCPf, 0);
            }
        }
        break;
    default:
        return BCM_E_PARAM;
        break;
    }
    if (info->action == BCM_MPLS_SWITCH_ACTION_POP) {
        mode = -1;
        if (_BCM_MPLS_VPN_IS_L3(info->vpn)) {
            _BCM_MPLS_VPN_GET(vrf, _BCM_MPLS_VPN_TYPE_L3, info->vpn);
            if (!_BCM_MPLS_VRF_USED_GET(unit, vrf)) {
                return BCM_E_PARAM;
            }

            /* Check L3 Ingress Interface mode. */ 
            mode = 0;
            rv = bcm_xgs3_l3_ingress_mode_get(unit, &mode);
            BCM_IF_ERROR_RETURN(rv);
        }
        if (!mode) {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, L3_IIFf, 
                                        _BCM_TR_MPLS_L3_IIF_BASE + vrf);
         } else {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, L3_IIFf, info->ingress_if);
         }
    } else if (info->action == BCM_MPLS_SWITCH_ACTION_PHP) {

        /*
         * Get egress next-hop index from egress object and
         * increment egress object reference count.
         */

        BCM_IF_ERROR_RETURN(bcm_xgs3_get_nh_from_egress_object(unit,
                    info->egress_if, &mpath_flag, 1, &nh_index));
       
        /* Fix: Entry_Type = 1, for PHP Packets with more than 1 Label */
        /* Read the egress next_hop entry pointed by Egress-Object */   
        rv = bcm_tr_mpls_egress_entry_modify(unit, nh_index, mpath_flag, 1);
        if (rv < 0 ) {
            return rv;
        }

        if (mpath_flag == BCM_L3_MULTIPATH) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, ECMP_PTRf, nh_index);
            
            /* Swap to ECMP Type  PHP_ECMP or L3_ECMP flag is set up earlier */
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, NEXT_HOP_INDEXf, nh_index);
        }
    } else if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label)) {         
            rv = bcm_tr_mpls_l3_nh_info_add(unit, info, &nh_index);
            if (rv < 0) {
                goto cleanup;
            }
            soc_MPLS_ENTRYm_field32_set(unit, &ment, NEXT_HOP_INDEXf, nh_index);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                    _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                    _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI);

        } else {
            /*
             * Get egress next-hop index from egress object and
             * increment egress object reference count.
             */
            BCM_IF_ERROR_RETURN(bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                                                   &mpath_flag, 1, &nh_index));
       
            if (soc_feature(unit, soc_feature_mpls_lsr_ecmp) && (mpath_flag == BCM_L3_MULTIPATH)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, ECMP_PTRf, nh_index);
                /* Swap to ECMP Type */
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                        _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_BOSf,
                        _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP);
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, NEXT_HOP_INDEXf, nh_index);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                        _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                        _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI);

            }
        }
    }/* BCM_MPLS_SWITCH_ACTION_SWAP */

    soc_MPLS_ENTRYm_field32_set(unit, &ment, V4_ENABLEf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, &ment, V6_ENABLEf, 1);
    if (info->flags & BCM_MPLS_SWITCH_INNER_TTL) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_TTLf, 0);
    } else {
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_TTLf, 1);
    }
    if (info->flags & BCM_MPLS_SWITCH_INNER_EXP) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 0);
    } else {
        /* For SWAP, Do-not PUSH EXP */
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 0);
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 1);
        }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        if (info->flags & BCM_MPLS_SWITCH_DROP) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment,
                    MPLS__DROP_DATA_ENABLEf, 1);
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment,
                    MPLS__DROP_DATA_ENABLEf, 0);
        }
        if ((info->failover_id >= 0) &&
                (info->failover_id <=
                 (soc_mem_index_max(unit, RX_PROT_GROUP_TABLEm) *
                  BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY))) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__RX_PROT_GROUPf,
                    info->failover_id);
        }
    }
#endif

    (void) bcm_tr_mpls_entry_internal_qos_set(unit, NULL, info, &ment);

    if ((info->flags & BCM_MPLS_SWITCH_COUNTED)) {
       if (SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
        if (old_pw_cnt == -1) {
            num_pw_term = soc_mem_index_count(unit, ING_PW_TERM_COUNTERSm);
            for (pw_cnt = 0; pw_cnt < num_pw_term; pw_cnt++) {
                if (!_BCM_MPLS_PW_TERM_USED_GET(unit, pw_cnt)) {
                    break;
                }
            }
            if (pw_cnt == num_pw_term) {
                rv = BCM_E_RESOURCE;
                goto cleanup;
            }
            _BCM_MPLS_PW_TERM_USED_SET(unit, pw_cnt);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PW_TERM_NUMf, pw_cnt);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PW_TERM_NUM_VALIDf, 1);
        }
      }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    else if (SOC_IS_TRIDENT2PLUS(unit) &&
                SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, CLASS_IDf)) {
        soc_MPLS_ENTRYm_field32_set(unit, &ment, CLASS_IDf, info->class_id);
    }
#endif

    if (!tunnel_switch_update) {
        rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);
    } else {
        rv = soc_mem_write(unit, MPLS_ENTRYm,
                           MEM_BLOCK_ALL, index,
                           &ment);
    }

    if (rv < 0) {
        goto cleanup;
    }

    if (tunnel_switch_update) {
        /* Clean up old next-hop and counter info if entry was replaced */
        if ((old_pw_cnt != -1) && !(info->flags & BCM_MPLS_SWITCH_COUNTED)) {
            _BCM_MPLS_PW_TERM_USED_CLR(unit, old_pw_cnt);
        }
        if (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI) {
            /* Check if tunnel_switch.egress_label mode is being used */
            rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) old_nh_index, &egress_if);
            if (rv == BCM_E_NONE) {
                rv = bcm_tr_mpls_l3_nh_info_delete(unit, old_nh_index);
            } else {
                /* Decrement next-hop Reference count */
                rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, old_nh_index);
            }
        } else if (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI) {
            rv = bcm_xgs3_nh_del(unit, 0, old_nh_index);
        } else if (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP) {
            rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index);
        } else if (soc_feature(unit, soc_feature_mpls_lsr_ecmp) &&
                (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP)) {
            rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index);
        }
    }
    if (rv < 0) {
        goto cleanup;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;

  cleanup:
    if (pw_cnt != -1) {
        _BCM_MPLS_PW_TERM_USED_CLR(unit, pw_cnt);
    }
    if (nh_index != -1) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label) ||
                (info->action == BCM_MPLS_SWITCH_ACTION_PHP)) {
                (void) bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
            }
        } else if (info->action == BCM_MPLS_SWITCH_ACTION_PHP) {
            (void) bcm_xgs3_nh_del(unit, 0, nh_index);
        }
    }
    return rv;
}

STATIC int
_bcmi_xgs5_mpls_entry_delete(int unit, mpls_entry_entry_t *ment)
{   
    ing_pw_term_counters_entry_t pw_cnt_entry;
    int rv, action, ecmp_index = -1, nh_index = -1, pw_cnt = -1;
    bcm_if_t  egress_if=0;
    int  ref_count=0;

    if (soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUM_VALIDf)) {
        pw_cnt = soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUMf);
    }

    action = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_ACTION_IF_BOSf);
    if ((action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI) ||
            (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI)) {
        nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment, NEXT_HOP_INDEXf);
    } else if (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP) {
        ecmp_index = soc_MPLS_ENTRYm_field32_get(unit, ment, ECMP_PTRf);
    } else if (soc_feature(unit, soc_feature_mpls_lsr_ecmp) &&
            (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP)) {
        ecmp_index = soc_MPLS_ENTRYm_field32_get(unit, ment, ECMP_PTRf);
    } 
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, ment);
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
         return rv;
    }

    if (pw_cnt != -1) {
        sal_memset(&pw_cnt_entry, 0, sizeof(ing_pw_term_counters_entry_t));
        (void) WRITE_ING_PW_TERM_COUNTERSm(unit, MEM_BLOCK_ALL, pw_cnt,
                                           &pw_cnt_entry);
        _BCM_MPLS_PW_TERM_USED_CLR(unit, pw_cnt);
    }

    if (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI) {
        /* Check if tunnel_switch.egress_label mode is being used */
        rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) nh_index, &egress_if);
        if (rv == BCM_E_NONE) {
            rv = bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
        } else {
            /* Decrement next-hop Reference count */
            rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, nh_index);
        }
    } else if (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI) {
        rv = bcm_xgs3_nh_del(unit, 0, nh_index);
    } else if ((action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP) || 
            (soc_feature(unit, soc_feature_mpls_lsr_ecmp) &&
             (action == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP))) {
        rv = bcm_xgs3_ecmp_group_del(unit, ecmp_index);
    }
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_delete
 * Purpose:
 *      Delete an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv, index;
    mpls_entry_entry_t ment;


    rv = _bcmi_xgs5_mpls_entry_set_key(unit, info, &ment);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);
    if (rv < 0) {
        return rv;
    }
    rv = _bcmi_xgs5_mpls_entry_delete(unit, &ment);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_delete_all
 * Purpose:
 *      Delete all MPLS label entries.
 * Parameters:
 *      unit   - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_mpls_tunnel_switch_delete_all(int unit)
{
    int rv, i, num_entries;
    mpls_entry_entry_t ment;


    

    num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
    for (i = 0; i < num_entries; i++) {
        rv = READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, &ment);
        if (rv < 0) {
            return rv;
        }
        if (!soc_MPLS_ENTRYm_field32_get(unit, &ment, VALIDf)) {
            continue;
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, MPLS_ACTION_IF_BOSf) ==
                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L2_SVP) {
            /* L2_SVP */
            continue;
        }
        rv = _bcmi_xgs5_mpls_entry_delete(unit, &ment);
        if (rv < 0) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_get
 * Purpose:
 *      Get an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv, index;
    mpls_entry_entry_t ment;

    rv = _bcmi_xgs5_mpls_entry_set_key(unit, info, &ment);


    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);

    if (rv < 0) {
        return rv;
    }
    rv = _bcmi_xgs5_mpls_entry_get_data(unit, &ment, info);

    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_traverse
 * Purpose:
 *      Traverse all valid MPLS label entries an call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per MPLS entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_mpls_tunnel_switch_traverse(int unit, 
                                   bcm_mpls_tunnel_switch_traverse_cb cb,
                                   void *user_data)
{
    int rv, i;
    mpls_entry_entry_t *ment = NULL;
    bcm_mpls_tunnel_switch_t info;
    int index_min, index_max;
    uint8 *mpls_entry_buf = NULL;

    mpls_entry_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, MPLS_ENTRYm), "MPLS_ENTRY buffer");
    if (NULL == mpls_entry_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    index_min = soc_mem_index_min(unit, MPLS_ENTRYm);
    index_max = soc_mem_index_max(unit, MPLS_ENTRYm);
    rv = soc_mem_read_range(unit, MPLS_ENTRYm, MEM_BLOCK_ANY,
            index_min, index_max, mpls_entry_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    for (i = index_min; i <= index_max; i++) {
        ment = soc_mem_table_idx_to_pointer
            (unit, MPLS_ENTRYm, mpls_entry_entry_t *, mpls_entry_buf, i);

        /* Check for valid entry */
        if (!soc_MPLS_ENTRYm_field32_get(unit, ment, VALIDf)) {
            continue;
        }
        /* Check MPLS Key_type */
        if (soc_feature(unit, soc_feature_mpls_enhanced)) {
            if (0x0 != soc_MPLS_ENTRYm_field32_get(unit, ment, KEY_TYPEf)) {
                continue;
            }
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_ACTION_IF_BOSf) ==
                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L2_SVP) {
            continue;
        }
        sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
        rv = _bcmi_xgs5_mpls_entry_get_key(unit, ment, &info);
        if (rv < 0) {
            goto cleanup;
        }
        rv = _bcmi_xgs5_mpls_entry_get_data(unit, ment, &info);
        if (rv < 0) {
            goto cleanup;
        }
        rv = cb(unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            goto cleanup;
        }
#endif
    }

cleanup:
    if (mpls_entry_buf) {
        soc_cm_sfree(unit, mpls_entry_buf);
    }

    return rv;
}
#endif /* (BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) */
#endif /* BCM_MPLS_SUPPORT */
#endif /* INCLUDE_L3 */

