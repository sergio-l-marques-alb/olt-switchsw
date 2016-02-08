/*
 * $Id: triumph_l2.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        l2.c
 * Purpose:     Triumph L2 function implementations
 */

#include <soc/defs.h>

#if defined(BCM_TRX_SUPPORT)

#include <assert.h>

#include <sal/core/libc.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/triumph.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>

#define DEFAULT_L2DELETE_CHUNKS		64	/* 16k entries / 64 = 256 */

typedef struct _bcm_mac_block_info_s {
    bcm_pbmp_t mb_pbmp;
    int ref_count;
} _bcm_mac_block_info_t;

static _bcm_mac_block_info_t *_mbi_entries[BCM_MAX_NUM_UNITS];
static int _mbi_num[BCM_MAX_NUM_UNITS];

#define L2_DEBUG(flags, stuff)  BCM_DEBUG(flags | BCM_DBG_L2, stuff)
#define L2_ERR(stuff)           L2_DEBUG(BCM_DBG_ERR, stuff)
#define L2_WARN(stuff)          L2_DEBUG(BCM_DBG_WARN, stuff)
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
extern int16 * _sc_subport_group_index[BCM_MAX_NUM_UNITS];
#define _SC_SUBPORT_NUM_PORT  (4096)
#define _SC_SUBPORT_NUM_GROUP (4096/8)
#define _SC_SUBPORT_VPG_FIND(unit, vp, grp) \
    do { \
         int ix; \
         grp = -1; \
         for (ix = 0; ix < _SC_SUBPORT_NUM_GROUP; ix++) { \
              if (_sc_subport_group_index[unit][ix] == vp) { \
                  grp = ix * 8; \
                  break;  \
              } \
         } \
       } while ((0))
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */

/*
 * Function:
 *      _bcm_tr_l2_from_l2x 
 * Purpose:
 *      Convert a Triumph L2X entry to a hardware-independent L2 entry
 * Parameters:
 *      unit - Unit number
 *      l2addr - (OUT) Hardware-independent L2 entry
 *      l2x_entry - Firebolt L2X entry
 */

 
int     
_bcm_tr_l2_from_l2x(int unit, bcm_l2_addr_t *l2addr,
        l2x_entry_t *l2x_entry)
{
    int l2mc_index, mb_index, vfi;
    
    sal_memset(l2addr, 0, sizeof (*l2addr));
 
    /* Valid bit is ignored here; entry is assumed valid */
 
    soc_L2Xm_mac_addr_get(unit, l2x_entry, MAC_ADDRf, l2addr->mac);
 
    l2addr->cos_dst = soc_L2Xm_field32_get(unit, l2x_entry, PRIf);
    l2addr->cos_src = soc_L2Xm_field32_get(unit, l2x_entry, PRIf);

    if (soc_L2Xm_field32_get(unit, l2x_entry, CPUf)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, DST_DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, SRC_DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, SCPf)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, PENDINGf)) {
        l2addr->flags |= BCM_L2_PENDING;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                       TR_L2_HASH_KEY_TYPE_VFI) {
        vfi = soc_L2Xm_field32_get(unit, l2x_entry, VFIf);
        /* VPLS or MIM VPN */
#if defined(INCLUDE_L3)
        if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
            _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
        } else {
            _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
        }
#endif
    } else {
        l2addr->vid = soc_L2Xm_field32_get(unit, l2x_entry, VLAN_IDf);
    }

    if (BCM_MAC_IS_MCAST(l2addr->mac)) {
        l2addr->flags |= BCM_L2_MCAST;
        l2mc_index = soc_L2Xm_field32_get(unit, l2x_entry, DESTINATIONf);
        if (soc_L2Xm_field32_get(unit, l2x_entry, VPG_TYPEf)) {
            if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                               TR_L2_HASH_KEY_TYPE_VFI) {
                vfi = soc_L2Xm_field32_get(unit, l2x_entry, VFIf);
                /* VPLS or MIM multicast */
#if defined(INCLUDE_L3)
                if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
                    _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_index,
                            _BCM_MULTICAST_TYPE_VPLS, l2mc_index);
                } else {
                    _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_index,
                            _BCM_MULTICAST_TYPE_MIM, l2mc_index);
                }
#endif
            } else {
                /* Subport multicast */
                _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_index,
                        _BCM_MULTICAST_TYPE_SUBPORT, l2mc_index);
            }
        } else {
            l2addr->l2mc_index = l2mc_index;
        }
    } else {
        _bcm_gport_dest_t       dest;
        int                     isGport = 0;

        _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
        if (soc_L2Xm_field32_get(unit, l2x_entry, DEST_TYPEf) == 2) {
            int vp;
            vp = soc_L2Xm_field32_get(unit, l2x_entry, DESTINATIONf);
            if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                     TR_L2_HASH_KEY_TYPE_VFI) {
                /* MPLS/MiM virtual port unicast */
                if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                    dest.mpls_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                    isGport=1;
                } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                    dest.mim_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                    isGport=1;
                } else {
                    return BCM_E_INTERNAL; /* Cannot reach here */
                }
            } else {
                /* Subport/WLAN unicast */
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
                if (SOC_IS_SC_CQ(unit)){
                   /* Scorpion uses index to L3_NEXT_HOP as VPG */
                   int grp;

                   _SC_SUBPORT_VPG_FIND(unit, vp, grp);
                   if ((vp = grp) == -1) {
                        L2_ERR(("Unit: %d can not find entry for VPG\n", unit));
                   }
                   dest.subport_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                   isGport=1;
              } else
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
              {
                if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
                    dest.subport_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                    isGport=1;
                } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                    dest.wlan_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                    isGport=1;
                } else {
                    return BCM_E_INTERNAL; /* Cannot reach here */
                }
              }
            }
        } else 
#endif /* INCLUDE_L3 */
        if (soc_L2Xm_field32_get(unit, l2x_entry, Tf)) {
            /* Trunk group */
            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            l2addr->tgid = soc_L2Xm_field32_get(unit, l2x_entry, TGIDf);
            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &(l2addr->rtag));
            if (soc_L2Xm_field32_get(unit, l2x_entry, REMOTE_TRUNKf)) {
                l2addr->flags |= BCM_L2_REMOTE_TRUNK;
            }
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        } else {
            bcm_module_t    mod_in, mod_out;
            bcm_port_t      port_in, port_out;

            port_in = soc_L2Xm_field32_get(unit, l2x_entry, PORT_NUMf);
            mod_in = soc_L2Xm_field32_get(unit, l2x_entry, MODULE_IDf);
            BCM_IF_ERROR_RETURN
                (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out));
            l2addr->modid = mod_out;
            l2addr->port = port_out;
            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        }

        if (isGport) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_construct(unit, &dest, &(l2addr->port)));
        }
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, L3f)) {
        l2addr->flags |= BCM_L2_L3LOOKUP;
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        l2addr->group = soc_L2Xm_field32_get(unit, l2x_entry, CLASS_IDf);
    } else {
        mb_index = soc_L2Xm_field32_get(unit, l2x_entry,
                                                  MAC_BLOCK_INDEXf);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _mbi_entries[unit][mb_index].mb_pbmp);
        }
        l2addr->group = 0;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, RPEf)) {
        l2addr->flags |= BCM_L2_SETPRI;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, STATIC_BITf)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, MIRRORf)) {
        l2addr->flags |= BCM_L2_MIRROR;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, HITSAf)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
    }

    if (soc_L2Xm_field32_get(unit, l2x_entry, HITDAf)) {
        l2addr->flags |= BCM_L2_DES_HIT;
    }

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, LIMIT_COUNTEDf)) {
        if (!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST |
                               BCM_L2_STATIC))) {
            if (!soc_L2Xm_field32_get(unit, l2x_entry, LIMIT_COUNTEDf)) {
                l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_to_l2x
 * Purpose:
 *      Convert a hardware-independent L2 entry to a Firebolt L2X entry
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - (OUT) Firebolt L2X entry
 *      l2addr - Hardware-independent L2 entry
 */

int
_bcm_tr_l2_to_l2x(int unit, l2x_entry_t *l2x_entry, bcm_l2_addr_t *l2addr)
{
    
    VLAN_CHK_PRIO(unit, l2addr->cos_dst);

    sal_memset(l2x_entry, 0, sizeof (*l2x_entry));

    soc_L2Xm_field32_set(unit, l2x_entry, VALIDf, 1);
    soc_L2Xm_mac_addr_set(unit, l2x_entry, MAC_ADDRf, l2addr->mac);
    soc_L2Xm_field32_set(unit, l2x_entry, PRIf, l2addr->cos_dst);
    if (_BCM_MPLS_VPN_IS_VPLS(l2addr->vid)) {
        soc_L2Xm_field32_set(unit, l2x_entry, VFIf, 
                             _BCM_MPLS_VPN_ID_GET(l2addr->vid));
        soc_L2Xm_field32_set(unit, l2x_entry, KEY_TYPEf, 
                             TR_L2_HASH_KEY_TYPE_VFI);
    } else if (_BCM_MIM_VPN_IS_SET(l2addr->vid)) {
        soc_L2Xm_field32_set(unit, l2x_entry, VFIf, 
                             _BCM_MIM_VPN_ID_GET(l2addr->vid));
        soc_L2Xm_field32_set(unit, l2x_entry, KEY_TYPEf, 
                             TR_L2_HASH_KEY_TYPE_VFI);
    } else {
        VLAN_CHK_ID(unit, l2addr->vid);
        soc_L2Xm_field32_set(unit, l2x_entry, VLAN_IDf, l2addr->vid);
        soc_L2Xm_field32_set(unit, l2x_entry, KEY_TYPEf, 
                             TR_L2_HASH_KEY_TYPE_BRIDGE);
    }

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        soc_L2Xm_field32_set(unit, l2x_entry, CPUf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        soc_L2Xm_field32_set(unit, l2x_entry, DST_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        soc_L2Xm_field32_set(unit, l2x_entry, SCPf, 1);
    }

    if (l2addr->flags & BCM_L2_REMOTE_TRUNK) {
        soc_L2Xm_field32_set(unit, l2x_entry, REMOTE_TRUNKf, 1);
    }

    if (l2addr->flags & BCM_L2_PENDING) {
        soc_L2Xm_field32_set(unit, l2x_entry, PENDINGf, 1);
    }

    if (BCM_MAC_IS_MCAST(l2addr->mac)) {
        if (_BCM_MULTICAST_IS_VPLS(l2addr->l2mc_index) ||
            _BCM_MULTICAST_IS_MIM(l2addr->l2mc_index) ||
            _BCM_MULTICAST_IS_WLAN(l2addr->l2mc_index) ||
            _BCM_MULTICAST_IS_SUBPORT(l2addr->l2mc_index)) {
            soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x3);
        }

        if (_BCM_MULTICAST_IS_SET(l2addr->l2mc_index)) {
            soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf,
                                 _BCM_MULTICAST_ID_GET(l2addr->l2mc_index));
        } else {
            soc_L2Xm_field32_set(unit, l2x_entry, L2MC_PTRf, l2addr->l2mc_index);
        }
    } else {
        bcm_port_t      port = -1;
        bcm_trunk_t     tgid = BCM_TRUNK_INVALID;
        bcm_module_t    modid = -1;
        int             gport_id = -1;
        int             vpg_type = 0;

        if (BCM_GPORT_IS_SET(l2addr->port)) {
            _bcm_l2_gport_params_t  g_params;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_l2_gport_parse(unit, l2addr, &g_params));

            switch (g_params.type) {
                case _SHR_GPORT_TYPE_TRUNK: 
                    tgid = g_params.param0;
                    break;
                case  _SHR_GPORT_TYPE_MODPORT:
                    port = g_params.param0;
                    modid = g_params.param1;
                    break;
                case _SHR_GPORT_TYPE_LOCAL_CPU:
                    port = g_params.param0;
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_stk_my_modid_get(unit, &modid));
                    break;
                case _SHR_GPORT_TYPE_SUBPORT_GROUP:
                    gport_id = g_params.param0;
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
                    if (SOC_IS_SC_CQ(unit)) {
                        /* Map the sub_port to index to L3_NEXT_HOP */
                        gport_id = (int) _sc_subport_group_index[unit][gport_id/8];
                    }
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
                    vpg_type = 1;
                    break;
                case _SHR_GPORT_TYPE_MPLS_PORT:
                    gport_id = g_params.param0;
                    vpg_type = 1;
                    break;
                case _SHR_GPORT_TYPE_MIM_PORT:
                    gport_id = g_params.param0;
                    vpg_type = 1;
                    break;
                case _SHR_GPORT_TYPE_WLAN_PORT:
                    gport_id = g_params.param0;
                    vpg_type = 1;
                    break;
                default:
                    return BCM_E_PORT;
            }
        } else if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
            tgid = l2addr->tgid;

        } else {
            PORT_DUALMODID_VALID(unit, l2addr->port);
            BCM_IF_ERROR_RETURN(
                bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                       l2addr->modid, l2addr->port, 
                                       &modid, &port));
            /* Check parameters */
            if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port)) {
                return BCM_E_PORT;
            }
        }

        /* Setting l2x_entry fields according to parameters */
        if ( BCM_TRUNK_INVALID != tgid) {
            soc_L2Xm_field32_set(unit, l2x_entry, Tf, 1);
            soc_L2Xm_field32_set(unit, l2x_entry, TGIDf, tgid);
            /*
             * Note:  RTAG is ignored here.  Use bcm_trunk_psc_set to
             * to set for a given trunk.
             */
        } else if (-1 != port) {
            soc_L2Xm_field32_set(unit, l2x_entry, MODULE_IDf, modid);
            soc_L2Xm_field32_set(unit, l2x_entry, PORT_NUMf, port);
        } else if (-1 != gport_id) {
            soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x2); 
            if (vpg_type) {
                soc_L2Xm_field32_set(unit, l2x_entry, VPGf, gport_id);
                soc_L2Xm_field32_set(unit, l2x_entry, VPG_TYPEf, vpg_type); 
            } else {
                soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf, gport_id);
            }
        }
    }

    if (l2addr->flags & BCM_L2_L3LOOKUP) {
        soc_L2Xm_field32_set(unit, l2x_entry, L3f, 1);
    }

    if (l2addr->flags & BCM_L2_MIRROR) {
        soc_L2Xm_field32_set(unit, l2x_entry, MIRRORf, 1);
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        soc_L2Xm_field32_set(unit, l2x_entry, CLASS_IDf, l2addr->group);
    } /* else MAC_BLOCK_INDEXf is handled in the add/remove functions below */

    if (l2addr->flags & BCM_L2_SETPRI) {
        soc_L2Xm_field32_set(unit, l2x_entry, RPEf, 1);
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        soc_L2Xm_field32_set(unit, l2x_entry, STATIC_BITf, 1);
    }

    if (l2addr->flags & BCM_L2_SRC_HIT) {
        soc_L2Xm_field32_set(unit, l2x_entry, HITSAf, 1);
    }

    if (l2addr->flags & BCM_L2_DES_HIT) {
        soc_L2Xm_field32_set(unit, l2x_entry, HITDAf, 1);
    }

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, LIMIT_COUNTEDf)) {
        if (!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST | BCM_L2_STATIC |
                               BCM_L2_LEARN_LIMIT_EXEMPT))) {
            soc_L2Xm_field32_set(unit, l2x_entry, LIMIT_COUNTEDf, 1);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_from_ext_l2
 * Purpose:
 *      Convert an ESM L2 entry to a hardware-independent L2 entry
 * Parameters:
 *      unit         Device number
 *      l2addr       (OUT) Hardware-independent L2 entry
 *      ext_l2_entry ESM L2 hardware entry
 */
int
_bcm_tr_l2_from_ext_l2(int unit, bcm_l2_addr_t *l2addr,
                       ext_l2_entry_entry_t *ext_l2_entry)
{
    _bcm_gport_dest_t       dest;
    int                     mb_index, vfi;
    bcm_module_t            mod;
    bcm_port_t              port;
    int  isGport = 0;

    sal_memset(l2addr, 0, sizeof(*l2addr));
    _bcm_gport_dest_t_init(&dest);

    soc_mem_mac_addr_get(unit, EXT_L2_ENTRYm, ext_l2_entry, MAC_ADDRf,
                         l2addr->mac);

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                            KEY_TYPE_VFIf)) {
        _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS,
                          soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                              ext_l2_entry, VFIf));
    } else {
        l2addr->vid = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                          VLAN_IDf);
    }

     if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf) == 0x1) {
         vfi = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, VFIf);
         /* VPLS or MIM VPN */
#if defined(INCLUDE_L3)
         if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
              _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
         } else {
              _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
         }
#endif
    } else {
         l2addr->vid = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, VLAN_IDf);
    }

    _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, DEST_TYPEf) == 0x2) {
         int vp;

         vp = soc_mem_field32_get(unit, EXT_L2_ENTRYm,ext_l2_entry,  DESTINATIONf);
         if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf) == 0x1) {
              /* MPLS/MiM virtual port unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                   dest.mpls_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                   dest.mim_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                   isGport=1;
              } else {
                   return BCM_E_INTERNAL; /* Cannot reach here */
              }
         } else {
              /* Subport/WLAN unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
                   dest.subport_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                   dest.wlan_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                   isGport=1;
              } else {
                   return BCM_E_INTERNAL; /* Cannot reach here */
              }
         }
    } else {
#endif /* INCLUDE_L3 */
        if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, Tf)) {
            l2addr->tgid = soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                               ext_l2_entry, TGIDf);
            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &l2addr->rtag);
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
        } else {
            mod = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                      MODULE_IDf);
            port = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                       PORT_NUMf);
            BCM_IF_ERROR_RETURN(bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                       mod, port, &mod, &port));
            l2addr->modid = mod;
            l2addr->port = port;
            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit, bcmSwitchUseGport,
                                                      &isGport));
#if defined(INCLUDE_L3)
    }
#endif /* INCLUDE_L3 */
	
    if (isGport) {
         BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, 
                                                      &(l2addr->port)));
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        l2addr->group = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                            CLASS_IDf);
    } else {
        mb_index = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                       MAC_BLOCK_INDEXf);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _mbi_entries[unit][mb_index].mb_pbmp);
        }
        l2addr->group = 0;
    }

    l2addr->cos_dst = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                          PRIf);
    l2addr->cos_src = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                          PRIf);

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, SCPf)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, CPUf)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, STATIC_BITf)) {
        l2addr->flags |= BCM_L2_STATIC;
    }
    if (dest.gport_type == _SHR_GPORT_TYPE_TRUNK) {
        l2addr->flags |= BCM_L2_TRUNK_MEMBER;
    }
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_HITf)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
    }
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_HITf)) {
        l2addr->flags |= BCM_L2_DES_HIT;
    }
    if (dest.gport_type == _SHR_GPORT_TYPE_TRUNK &&
        soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                            REMOTE_TRUNKf)) {
        l2addr->flags |= BCM_L2_REMOTE_TRUNK;
    }
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, MIRRORf)) {
        l2addr->flags |= BCM_L2_MIRROR;
    }
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, RPEf)) {
        l2addr->flags |= BCM_L2_SETPRI;
    }

    if (SOC_MEM_FIELD_VALID(unit, EXT_L2_ENTRYm, LIMIT_COUNTEDf)) {
        if (!(l2addr->flags & BCM_L2_STATIC)) {
            if (!soc_L2Xm_field32_get(unit, ext_l2_entry, LIMIT_COUNTEDf)) {
                l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_to_ext_l2
 * Purpose:
 *      Convert a hardware-independent L2 entry to an ESM L2 entry
 * Parameters:
 *      unit         Device number
 *      ext_l2_entry (OUT) ESM L2 hardware entry
 *      l2addr       Hardware-independent L2 entry
 */
int
_bcm_tr_l2_to_ext_l2(int unit, ext_l2_entry_entry_t *ext_l2_entry,
                     bcm_l2_addr_t *l2addr)
{
    _bcm_l2_gport_params_t  g_params;
    bcm_module_t            mod;
    bcm_port_t              port;
    uint32                  fval;

    sal_memset(ext_l2_entry, 0, sizeof(*ext_l2_entry));

    soc_mem_mac_addr_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MAC_ADDRf,
                         l2addr->mac);
    if (_BCM_MPLS_VPN_IS_VPLS(l2addr->vid)) {
        fval = _BCM_MPLS_VPN_ID_GET(l2addr->vid);
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VFIf, fval)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VFIf, fval);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf,
                            1);
    } else if (_BCM_MIM_VPN_IS_SET(l2addr->vid)) {
        fval = _BCM_MIM_VPN_ID_GET(l2addr->vid);
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VFIf, fval)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VFIf, fval);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf,
                            1);
    } else {
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VLAN_IDf,
                                       l2addr->vid)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VLAN_IDf,
                            l2addr->vid);
    }

    if (!BCM_GPORT_IS_SET(l2addr->port)) {
        if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
            g_params.param0 = l2addr->tgid;
            g_params.type = _SHR_GPORT_TYPE_TRUNK;
        } else {
            PORT_DUALMODID_VALID(unit, l2addr->port);
            BCM_IF_ERROR_RETURN(bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                                       l2addr->modid, l2addr->port,
                                                       &mod, &port));
            g_params.param0 = port;
            g_params.param1 = mod;
            g_params.type = _SHR_GPORT_TYPE_MODPORT;
        }
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_l2_gport_parse(unit, l2addr, &g_params));
    }
    switch (g_params.type) {
    case _SHR_GPORT_TYPE_TRUNK:
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, TGIDf,
                                       g_params.param0)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, Tf, 1);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, TGIDf,
                            g_params.param0);
        break;
    case _SHR_GPORT_TYPE_MODPORT:
        if (!SOC_MODID_ADDRESSABLE(unit, g_params.param1)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, g_params.param0)) {
            return BCM_E_PORT;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MODULE_IDf,
                            g_params.param1);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, PORT_NUMf,
                            g_params.param0);
        break;
    case _SHR_GPORT_TYPE_LOCAL_CPU:
        if (!SOC_PORT_ADDRESSABLE(unit, g_params.param0)) {
            return BCM_E_PORT;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, PORT_NUMf,
                            g_params.param0);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod));
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MODULE_IDf,
                            mod);
        break;
    case _SHR_GPORT_TYPE_MPLS_PORT:
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VPGf,
                                       g_params.param0)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VPGf,
                            g_params.param0);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VPG_TYPEf, 1);
        break;
    case _SHR_GPORT_TYPE_MIM_PORT:
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VPGf,
                                       g_params.param0)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VPGf,
                            g_params.param0);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VPG_TYPEf, 1);
        break;
    default:
        return BCM_E_PORT;
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, CLASS_IDf,
                            l2addr->group);
    } /* else MAC_BLOCK_INDEXf is handled in the add/remove functions */

    if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, PRIf,
                                   l2addr->cos_dst)) {
        return BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, PRIf,
                        l2addr->cos_dst);

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SCPf, 1);
    }
    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_DISCARDf,
                            1);
    }
    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_DISCARDf,
                            1);
    }
    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, CPUf, 1);
    }
    if (l2addr->flags & BCM_L2_STATIC) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, STATIC_BITf, 1);
    }
    if (l2addr->flags & BCM_L2_SRC_HIT) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_HITf, 1);
    }
    if (l2addr->flags & BCM_L2_DES_HIT) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_HITf, 1);
    }
    if (l2addr->flags & BCM_L2_REMOTE_TRUNK) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, REMOTE_TRUNKf,
                            1);
    }
    if (l2addr->flags & BCM_L2_MIRROR) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MIRRORf, 1);
    }
    if (l2addr->flags & BCM_L2_SETPRI) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, RPEf, 1);
    }

    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VALIDf, 1);

    if (SOC_MEM_FIELD_VALID(unit, EXT_L2_ENTRYm, LIMIT_COUNTEDf)) {
        if (!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST | BCM_L2_STATIC |
                               BCM_L2_LEARN_LIMIT_EXEMPT))) {
            soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                LIMIT_COUNTEDf, 1);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_tr_compose_ext_l2_entry
 * Description:
 *      Compose ext_l2_entry from its tcam portion, data portion, and hit bit
 *      Hardware does not support read and write to ext_l2_entry view.
 * Parameters:
 *      unit         Device number
 *      tcam_entry   TCAM portion of ESM L2 entry (ext_l2_entry_tcam_entry_t)
 *      data_entry   DATA portion of ESM L2 entry (ext_l2_entry_data_entry_t)
 *      src_hit      SRC_HIT field value
 *      dst_hit      DST_HIT field value
 *      ext_l2_entry (OUT) Buffer to store the composed ext_l2_entry_entry_t
 *                   result
 * Return:
 *      BCM_E_XXX.
 */
int
_bcm_tr_compose_ext_l2_entry(int unit,
                             ext_l2_entry_tcam_entry_t *tcam_entry,
                             ext_l2_entry_data_entry_t *data_entry,
                             int src_hit,
                             int dst_hit,
                             ext_l2_entry_entry_t *ext_l2_entry)
{
    sal_mac_addr_t      mac;
    uint32              fval;
    uint32              fbuf[2];

    if (tcam_entry == NULL || data_entry == NULL || ext_l2_entry == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(ext_l2_entry, 0, sizeof(ext_l2_entry_entry_t));

    /******************** Values from TCAM *******************************/
    fval = soc_mem_field32_get(unit, EXT_L2_ENTRY_TCAMm, tcam_entry, VLAN_IDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VLAN_IDf, fval);

    soc_mem_mac_addr_get(unit, EXT_L2_ENTRY_TCAMm, tcam_entry, MAC_ADDRf, mac);
    soc_mem_mac_addr_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MAC_ADDRf, mac);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRY_TCAMm, tcam_entry,
                               KEY_TYPE_VFIf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf,
                        fval);

    /******************** Values from DATA *******************************/
    soc_mem_field_get(unit, EXT_L2_ENTRY_DATAm, (uint32 *)data_entry,
                      AD_EXT_L2f, fbuf);
    soc_mem_field_set(unit, EXT_L2_ENTRYm, (uint32 *)ext_l2_entry, AD_EXT_L2f,
                      fbuf);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRY_DATAm, data_entry, VALIDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VALIDf, fval);

    /******************** Hit Bits *******************************/
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_HITf, dst_hit);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_HITf, src_hit);

    return BCM_E_NONE;
}

/*
 * function:
 *     _bcm_tr_l2_traverse_mem
 * Description:
 *      Helper function to _bcm_esw_l2_traverse to itterate over given memory
 *      and actually read the table and parse entries for Triumph external
 *      memory
 * Parameters:
 *     unit         device number
 *      mem         External L2 memory to read
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
#ifdef BCM_TRIUMPH_SUPPORT
int
_bcm_tr_l2_traverse_mem(int unit, soc_mem_t mem, _bcm_l2_traverse_t *trav_st)
{
    ext_l2_entry_entry_t      *ext_l2_entry;
    int                       rv, idx, idx_max;

    if (mem != EXT_L2_ENTRYm) {
        return BCM_E_UNAVAIL;
    }

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    idx_max = soc_mem_index_max(unit, mem);
    for (idx = soc_mem_index_min(unit, mem); idx <= idx_max; idx++ ) {
        rv = soc_shadow_ext_l2_entry_read(unit, idx, &ext_l2_entry);
        if (rv == SOC_E_NOT_FOUND || rv == SOC_E_INIT) {
            continue;
        } else if (SOC_FAILURE(rv)) {
            return rv;
        }
        trav_st->data = (uint32 *)ext_l2_entry;
        trav_st->mem = mem;

        BCM_IF_ERROR_RETURN(trav_st->int_cb(unit, trav_st));
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      _bcm_mac_block_insert
 * Purpose:
 *      Find or create a MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_pbmp - egress port bitmap for source MAC blocking
 *      mb_index - (OUT) Index of MAC_BLOCK table with bitmap.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No more MAC_BLOCK entries available
 *      BCM_E_PARAM             Bad bitmap supplied
 */
static int
_bcm_mac_block_insert(int unit, bcm_pbmp_t mb_pbmp, int *mb_index)
{
    int cur_index = 0;
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    mac_block_entry_t mbe;
    bcm_pbmp_t temp_pbmp;

    /* Check for reasonable pbmp */
    BCM_PBMP_ASSIGN(temp_pbmp, mb_pbmp);
    BCM_PBMP_AND(temp_pbmp, PBMP_ALL(unit));
    if (BCM_PBMP_NEQ(mb_pbmp, temp_pbmp)) {
        return BCM_E_PARAM;
    }

    for (cur_index = 0; cur_index < _mbi_num[unit]; cur_index++) {
        if (BCM_PBMP_EQ(mbi[cur_index].mb_pbmp, mb_pbmp)) {
            mbi[cur_index].ref_count++;
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Not in table already, see if any space free */
    for (cur_index = 1; cur_index < _mbi_num[unit]; cur_index++) {
        if (mbi[cur_index].ref_count == 0) {
            /* Attempt insert */
            sal_memset(&mbe, 0, sizeof(mac_block_entry_t));

            soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_LOf,
                                       SOC_PBMP_WORD_GET(mb_pbmp, 0));
            soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_HIf,
                                       SOC_PBMP_WORD_GET(mb_pbmp, 1));
            SOC_IF_ERROR_RETURN(WRITE_MAC_BLOCKm(unit, MEM_BLOCK_ALL,
                                                 cur_index, &mbe));
            mbi[cur_index].ref_count++;
            BCM_PBMP_ASSIGN(mbi[cur_index].mb_pbmp, mb_pbmp);
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Didn't find a free slot, out of table space */
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_mac_block_delete
 * Purpose:
 *      Remove reference to MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_index - Index of MAC_BLOCK table with bitmap.
 */
static void
_bcm_mac_block_delete(int unit, int mb_index)
{
    if (_mbi_entries[unit][mb_index].ref_count > 0) {
        _mbi_entries[unit][mb_index].ref_count--;
    } else if (mb_index) {
        
        /* Someone reran init without flushing the L2 table */
    } /* else mb_index = 0, as expected for learning */
}

/*
 * Function:
 *      bcm_tr_l2_addr_add
 * Description:
 *      Add a MAC address to the Switch Address Resolution Logic (ARL)
 *      port with the given VLAN ID and parameters.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No MAC_BLOCK entries available
 * Notes:
 *      Use CMIC_PORT(unit) to associate the entry with the CPU.
 *      Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 *      In case the L2X table is full (e.g. bucket full), an attempt
 *      will be made to store the entry in the L2_USER_ENTRY table.
 */

int
bcm_tr_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    l2x_entry_t  l2x_entry, l2x_lookup, l2ent;
#ifdef BCM_TRIUMPH_SUPPORT
    ext_l2_entry_entry_t ext_l2_entry, ext_l2_lookup, *ext_l2_read;
    int          same_dest, limit_counted;
#endif /* BCM_TRIUMPH_SUPPORT */
    int          rv, hash_select, bucket, slot, num_bits;
    int          hash_select2, bucket2 = -1;
    int          l2_index, mb_index = 0;
    uint8        key[XGS_HASH_KEY_SIZE];
    uint32       hash_control;
    int          cf_hit, cf_unhit;
    bcm_mac_t    mac;
    int          exist_in_ext_l2, update_limit;

    exist_in_ext_l2 = FALSE;
    update_limit =
        l2addr->flags & (BCM_L2_STATIC | BCM_L2_LEARN_LIMIT_EXEMPT) ?
        FALSE : TRUE;

    BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_l2x(unit, &l2x_entry, l2addr));

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm) > 0) {
        same_dest = FALSE;
        limit_counted = FALSE;
        BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_ext_l2(unit, &ext_l2_entry, l2addr));
        rv = soc_mem_generic_lookup(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                    &ext_l2_entry, &ext_l2_lookup, &l2_index);
        if (BCM_SUCCESS(rv)) {
            exist_in_ext_l2 = TRUE;
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }

        if (!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST |
                               BCM_L2_PENDING | BCM_L2_STATIC))) {
            if (exist_in_ext_l2) {
                if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                        DEST_TYPEf) ==
                    soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_lookup,
                                        DEST_TYPEf) &&
                    soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                        DESTINATIONf) ==
                    soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_lookup,
                                        DESTINATIONf)) {
                    same_dest = TRUE;
                }
                rv = soc_shadow_ext_l2_entry_read(unit, l2_index,
                                                  &ext_l2_read);
                if (BCM_SUCCESS(rv)) {
                    limit_counted =
                        soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_read,
                                            LIMIT_COUNTEDf);
                }
            }
            if (update_limit) {
                if (!limit_counted) {
                    BCM_IF_ERROR_RETURN
                        (soc_triumph_learn_count_update(unit, &ext_l2_entry,
                                                        TRUE, 1));
                } else if (!same_dest) {
                    BCM_IF_ERROR_RETURN
                        (soc_triumph_learn_count_update(unit, &ext_l2_entry,
                                                        FALSE, 1));
                }
            }
            if (!SOC_CONTROL(unit)->l2x_group_enable) {
                /* Mac blocking, attempt to associate with bitmap entry */
                BCM_IF_ERROR_RETURN
                    (_bcm_mac_block_insert(unit, l2addr->block_bitmap,
                                           &mb_index));
                soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                    MAC_BLOCK_INDEXf, mb_index);
            }
            rv = soc_mem_generic_insert(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                        &ext_l2_entry, &ext_l2_lookup, NULL);
            if (rv == BCM_E_EXISTS) {
                /* entry exists, clear setting for old entry */
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index = soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                                   &ext_l2_lookup,
                                                   MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (limit_counted) {
                    if (!update_limit) {
                        (void)soc_triumph_learn_count_update(unit,
                                                             &ext_l2_lookup,
                                                             TRUE, -1);
                    } else if (!same_dest) {
                        (void)soc_triumph_learn_count_update(unit,
                                                             &ext_l2_lookup,
                                                             FALSE, -1);
                    }
                }
                rv = BCM_E_NONE;
            } else if (BCM_FAILURE(rv)) {
                /* insert fail, undo setting for new entry */
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index =
                        soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                            MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (update_limit) {
                    if (!limit_counted) {
                        (void)soc_triumph_learn_count_update(unit,
                                                             &ext_l2_entry,
                                                             TRUE, -1);
                    } else if (!same_dest) {
                        (void)soc_triumph_learn_count_update(unit,
                                                             &ext_l2_entry,
                                                             FALSE, -1);
                    }
                }
            }
            if (BCM_SUCCESS(rv)) {
                /* insert to ext_l2_entry OK, delete from l2x if present */
                soc_mem_lock(unit, L2Xm);
                if (SOC_L2_DEL_SYNC_LOCK(SOC_CONTROL(unit)) >= 0) {
                    rv = soc_mem_generic_delete(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                                &l2x_entry, &l2x_lookup,
                                                &l2_index);
                    if (BCM_SUCCESS(rv)) {
                        if (!SOC_CONTROL(unit)->l2x_group_enable) {
                            mb_index =
                                soc_mem_field32_get(unit, L2Xm, &l2x_lookup,
                                                    MAC_BLOCK_INDEXf);
                            _bcm_mac_block_delete(unit, mb_index);
                        }
                        rv = soc_l2x_sync_delete(unit, (uint32 *)&l2x_lookup,
                                                 l2_index);
                    } else if (rv == BCM_E_NOT_FOUND) {
                        rv = BCM_E_NONE;
                    }
                    SOC_L2_DEL_SYNC_UNLOCK(SOC_CONTROL(unit));
                } else {
                    rv = BCM_E_INTERNAL;
                }
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            if (rv != BCM_E_FULL) {
                goto done;
            }
        }
    }
#endif

    rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                &l2x_lookup, &l2_index);
    if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        /* Mac blocking, attempt to associate with bitmap entry */
        BCM_IF_ERROR_RETURN
            (_bcm_mac_block_insert(unit, l2addr->block_bitmap, &mb_index));
        soc_mem_field32_set(unit, L2Xm, &l2x_entry, MAC_BLOCK_INDEXf,
                            mb_index);
    }

    rv = soc_mem_insert_return_old(unit, L2Xm, MEM_BLOCK_ANY, 
                                   (void *)&l2x_entry, (void *)&l2x_entry);
    if ((rv == BCM_E_FULL) && (l2addr->flags & BCM_L2_REPLACE_DYNAMIC)) {
        rv = READ_HASH_CONTROLr(unit, &hash_control); 
        if (rv < 0 ) {
            goto done;
        }
        hash_select = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                        L2_AND_VLAN_MAC_HASH_SELECTf);

        num_bits = soc_tr_l2x_param_to_key(unit, l2addr->mac, l2addr->vid, key);
        bucket = soc_tr_l2x_hash(unit, hash_select, num_bits, key);

        rv = READ_L2_AUX_HASH_CONTROLr(unit, &hash_control);
        if (rv < 0 ) {
            goto done;
        }
        if (soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                              hash_control, ENABLEf)) {
            hash_select2 = soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                             hash_control, HASH_SELECTf);
        } else {
            hash_select2 = hash_select;
        }
        bucket2 = soc_fb_l2_hash(unit, hash_select2, key);

        rv = soc_l2x_freeze(unit);
        if (rv < 0) {
            goto done;
        }

        cf_hit = cf_unhit = -1;
        for (slot = 0; slot < SOC_L2X_BUCKET_SIZE; slot++) {
            if ((bucket2 >=0) && (slot == (SOC_L2X_BUCKET_SIZE/2))) {
                bucket = bucket2;
            }
            l2_index = bucket * SOC_L2X_BUCKET_SIZE + slot;
            rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, l2_index, &l2ent);
            if ((rv < 0)) {
                (void) soc_l2x_thaw(unit);
                goto done;
            }

            if (!soc_L2Xm_field32_get(unit, &l2ent, VALIDf)) {
                /* Found invalid entry - stop the search victim found */
                cf_unhit = l2_index; 
                break;
            } else {
                if ((soc_L2Xm_field32_get(unit, &l2ent, KEY_TYPEf) != 
                                          TR_L2_HASH_KEY_TYPE_BRIDGE) &&
                    (soc_L2Xm_field32_get(unit, &l2ent, KEY_TYPEf) != 
                                          TR_L2_HASH_KEY_TYPE_VFI)) {
                    continue;
                }

                soc_L2Xm_mac_addr_get(unit, &l2ent, MAC_ADDRf, mac);
                /* Skip static entries */
                if ((soc_L2Xm_field32_get(unit, &l2ent, STATIC_BITf)) ||
                    (BCM_MAC_IS_MCAST(mac)) ||
                    (soc_L2Xm_field32_get(unit, &l2ent, L3f))) {
                    continue;
                }
                if (soc_L2Xm_field32_get(unit, &l2ent, HITDAf) || 
                    soc_L2Xm_field32_get(unit, &l2ent, HITSAf) ) {
                    cf_hit =  l2_index;
                } else {
                    /* Found unhit entry - stop search victim found */
                    cf_unhit = l2_index;
                    break;
                }
            }
        }

        if (cf_unhit >= 0) {
            l2_index = cf_unhit;   /* take last unhit dynamic */
        } else if (cf_hit >= 0) {
            l2_index = cf_hit;     /* or last hit dynamic */
        } else {
          rv = BCM_E_FULL;     /* no dynamics to delete */
          (void) soc_l2x_thaw(unit);
          goto done;
        }

        soc_mem_write(unit, L2Xm, MEM_BLOCK_ALL, l2_index, &l2x_entry);
        rv = soc_l2x_thaw(unit);
    } else if (rv == BCM_E_EXISTS) {
        if (!SOC_CONTROL(unit)->l2x_group_enable) {
            mb_index = soc_mem_field32_get(unit, L2Xm, &l2x_lookup,
                                           MAC_BLOCK_INDEXf);
            _bcm_mac_block_delete(unit, mb_index);
        }
        rv = BCM_E_NONE;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (rv == BCM_E_NONE) {
        if (exist_in_ext_l2) {
            rv = soc_mem_generic_delete(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                        &ext_l2_entry, &ext_l2_lookup,
                                        &l2_index);
            if (BCM_SUCCESS(rv)) {
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index =
                        soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                            &ext_l2_lookup, MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                BCM_IF_ERROR_RETURN
                    (soc_shadow_ext_l2_entry_read(unit, l2_index,
                                                  &ext_l2_read));
                if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_read,
                                        LIMIT_COUNTEDf)) {
                    (void)soc_triumph_learn_count_update(unit, &ext_l2_lookup,
                                                         TRUE, -1);
                }
            }
        }
    }
#endif

done:
    if (rv < 0) {
        _bcm_mac_block_delete(unit, mb_index);
    }

    return rv;
}

STATIC int
_bcm_tr_l2_addr_delete(int unit, bcm_l2_addr_t *l2addr)
{
    l2x_entry_t    l2x_entry, l2x_lookup;
#ifdef BCM_TRIUMPH_SUPPORT
    ext_l2_entry_entry_t ext_l2_entry, ext_l2_lookup, *ext_l2_read;
    int limit_counted;
#endif /* BCM_TRIUMPH_SUPPORT */
    int            l2_index, mb_index;
    int            rv;
    soc_control_t  *soc = SOC_CONTROL(unit);

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm)) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_ext_l2(unit, &ext_l2_entry, l2addr));
        rv = soc_mem_generic_lookup(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                    &ext_l2_entry, NULL, &l2_index);
        if (BCM_SUCCESS(rv)) {
            BCM_IF_ERROR_RETURN
                (soc_shadow_ext_l2_entry_read(unit, l2_index, &ext_l2_read));
            limit_counted = soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                                ext_l2_read, LIMIT_COUNTEDf);
            rv = soc_mem_generic_delete(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                        &ext_l2_entry, &ext_l2_lookup, NULL);
            if (BCM_SUCCESS(rv)) {
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index =
                        soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                            &ext_l2_lookup, MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (limit_counted) {
                    rv = soc_triumph_learn_count_update(unit, ext_l2_read,
                                                        TRUE, -1);
                }
            }
        }
        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_tr_l2_to_l2x(unit, &l2x_entry, l2addr));

    soc_mem_lock(unit, L2Xm);
   
    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index,
                       (void *)&l2x_entry, (void *)&l2x_lookup, 0);
    if (BCM_E_NONE != rv) {
        soc_mem_unlock(unit, L2Xm);
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        mb_index = soc_L2Xm_field32_get(unit, &l2x_lookup, MAC_BLOCK_INDEXf);
        _bcm_mac_block_delete(unit, mb_index);
    }

    if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
        soc_mem_unlock(unit, L2Xm);
        return BCM_E_RESOURCE;
    }
    rv = soc_mem_delete_return_old(unit, L2Xm, MEM_BLOCK_ANY,
                                   (void *)&l2x_entry, (void *)&l2x_entry);
    if (rv >= 0) {
        rv = soc_l2x_sync_delete(unit, (uint32 *) &l2x_lookup, l2_index);
    }
    SOC_L2_DEL_SYNC_UNLOCK(soc);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *      bcm_tr_l2_addr_delete
 * Description:
 *      Delete an L2 address (MAC+VLAN) from the device
 * Parameters: 
 *      unit - device unit
 *      mac  - MAC address to delete
 *      vid  - VLAN id
 * Returns:  
 *      BCM_E_XXX
 */     
int
bcm_tr_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_addr_t  l2addr;
    
    bcm_l2_addr_t_init(&l2addr, mac, vid);

    return _bcm_tr_l2_addr_delete(unit, &l2addr);
}

#ifdef BCM_TRIUMPH_SUPPORT
void
_bcm_tr_ext_l2_entry_callback(int unit, int index, ext_l2_entry_entry_t *entry)
{
    ext_l2_entry_entry_t ext_l2_lookup;
    int rv, mb_index;

    rv = soc_mem_generic_delete(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0, entry,
                                &ext_l2_lookup, NULL);
    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        mb_index = soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_lookup,
                                       MAC_BLOCK_INDEXf);
        _bcm_mac_block_delete(unit, mb_index);
    }
    if (!soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_lookup,
                             STATIC_BITf)) {
        (void)soc_triumph_learn_count_update(unit,  &ext_l2_lookup, TRUE, -1);
    }
}
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      bcm_tr_l2_addr_get
 * Description:
 *      Given a MAC address and VLAN ID, check if the entry is present
 *      in the L2 table, and if so, return all associated information.
 * Parameters:
 *      unit - Device unit number
 *      mac - input MAC address to search
 *      vid - input VLAN ID to search
 *      l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *      BCM_E_NONE              Success (l2addr filled in)
 *      BCM_E_PARAM             Illegal parameter (NULL pointer)
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_NOT_FOUND Address not found (l2addr not filled in)
 */

int
bcm_tr_l2_addr_get(int unit, mac_addr_t mac, bcm_vlan_t vid,
                   bcm_l2_addr_t *l2addr)
{
    l2x_entry_t l2x_entry, l2x_lookup;
    ext_l2_entry_entry_t ext_l2_entry, ext_l2_lookup;
    int         index, rv;
    uint32      fval;

    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm) > 0 &&
        !(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST))) {
        sal_memset(&ext_l2_entry, 0, sizeof(ext_l2_entry_entry_t));
        soc_mem_mac_addr_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, MAC_ADDRf,
                             mac);
        if (_BCM_MPLS_VPN_IS_VPLS(vid)) {
            fval = _BCM_MPLS_VPN_ID_GET(vid);
            if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VFIf, fval)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, VFIf,
                                fval);
            soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                KEY_TYPE_VFIf, 1);
        } else if (_BCM_MIM_VPN_IS_SET(vid)) {
            fval = _BCM_MIM_VPN_ID_GET(vid);
            if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VFIf, fval)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, VFIf,
                                fval);
            soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                KEY_TYPE_VFIf, 1);
        } else {
            if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VLAN_IDf,
                                           vid)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, VLAN_IDf,
                                vid);
        }

        rv = soc_mem_generic_lookup(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                    &ext_l2_entry, &ext_l2_lookup, NULL);
        if (BCM_SUCCESS(rv)) {
            return _bcm_tr_l2_from_ext_l2(unit, l2addr, &ext_l2_lookup);
        } else if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }

    /* Set up entry for query */
    sal_memset(&l2x_entry, 0, sizeof(l2x_entry_t));
    soc_L2Xm_mac_addr_set(unit, &l2x_entry, MAC_ADDRf, mac);
    soc_L2Xm_mac_addr_set(unit, &l2x_entry, MAC_ADDRf, mac);
    if (_BCM_MPLS_VPN_IS_VPLS(vid)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, VFIf, 
                             _BCM_MPLS_VPN_ID_GET(vid));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf, 
                             TR_L2_HASH_KEY_TYPE_VFI);
    } else if (_BCM_MIM_VPN_IS_SET(vid)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, VFIf, 
                             _BCM_MIM_VPN_ID_GET(vid));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf, 
                             TR_L2_HASH_KEY_TYPE_VFI);
    } else {
        VLAN_CHK_ID(unit, vid);
        soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, vid);
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf, 
                             TR_L2_HASH_KEY_TYPE_BRIDGE);
    }

    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &index, 
                        (void *)&l2x_entry, (void *)&l2x_lookup, 0);

    if (rv == SOC_E_NONE) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l2_from_l2x(unit, l2addr, &l2x_lookup));
        return BCM_E_NONE;
    }

    return rv;
}


#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_tr_l2_reload_mbi
 * Description:
 *      Load MAC block info from hardware into software data structures.
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

STATIC int
_bcm_tr_l2_reload_mbi(int unit)
{
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    l2x_entry_t         *l2x_entry, *l2x_table;
    mac_block_entry_t   mbe;
    int                 index, mb_index, l2x_size;
    pbmp_t              mb_pbmp;

    /*
     * Refresh MAC Block information from the hardware tables.
     */

    for (mb_index = 0; mb_index < _mbi_num[unit]; mb_index++) {
        SOC_IF_ERROR_RETURN
            (READ_MAC_BLOCKm(unit, MEM_BLOCK_ANY, mb_index, &mbe));

        SOC_PBMP_CLEAR(mb_pbmp);

        SOC_PBMP_WORD_SET(mb_pbmp, 0,
                          soc_MAC_BLOCKm_field32_get(unit, &mbe, 
                                                     MAC_BLOCK_MASK_LOf));
        SOC_PBMP_WORD_SET(mb_pbmp, 1,
                          soc_MAC_BLOCKm_field32_get(unit, &mbe, 
                                                     MAC_BLOCK_MASK_HIf));
        BCM_PBMP_ASSIGN(mbi[mb_index].mb_pbmp, mb_pbmp);
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        l2x_size = sizeof(l2x_entry_t) * soc_mem_index_count(unit, L2Xm);
        l2x_table = soc_cm_salloc(unit, l2x_size, "l2 reload");
        if (l2x_table == NULL) {
            return BCM_E_MEMORY;
        }

        memset((void *)l2x_table, 0, l2x_size);
        if (soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                               soc_mem_index_min(unit, L2Xm),
                               soc_mem_index_max(unit, L2Xm),
                               l2x_table) < 0) {
            soc_cm_sfree(unit, l2x_table);
            return SOC_E_INTERNAL;
        }

        for (index = soc_mem_index_min(unit, L2Xm);
             index <= soc_mem_index_max(unit, L2Xm); index++) {

             l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm,
                                                      l2x_entry_t *,
                                                      l2x_table, index);
             if (!soc_L2Xm_field32_get(unit, l2x_entry, VALIDf)) {
                 continue;
             }
  
             mb_index = soc_L2Xm_field32_get(unit, l2x_entry, MAC_BLOCK_INDEXf);
             mbi[mb_index].ref_count++;
        }
        soc_cm_sfree(unit, l2x_table);
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *      _tr_l2x_delete_all
 * Purpose:
 *      Clear the L2 table by invalidating entries.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      static_too - if TRUE, delete static and non-static entries;
 *                   if FALSE, delete only non-static entries
 * Returns:
 *      SOC_E_XXX
 */

static int
_tr_l2x_delete_all(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int index_min, index_max, index, mem_max;
    l2_entry_only_entry_t *l2x_entry;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    int mem_size, idx;

    index_min = soc_mem_index_min(unit, L2_ENTRY_ONLYm);
    mem_max = soc_mem_index_max(unit, L2_ENTRY_ONLYm);
    mem_size =  DEFAULT_L2DELETE_CHUNKS * sizeof(l2_entry_only_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "L2_ENTRY_ONLY_delete");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }

    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += DEFAULT_L2DELETE_CHUNKS) {
        index_max = idx + DEFAULT_L2DELETE_CHUNKS - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        for (index = 0; index < DEFAULT_L2DELETE_CHUNKS; index++) {
            l2x_entry =
                soc_mem_table_idx_to_pointer(unit, L2_ENTRY_ONLYm,
                                             l2_entry_only_entry_t *, buffer, index);
            if (!soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VALIDf)) {
                continue;
            }
            if ((soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                                TR_L2_HASH_KEY_TYPE_BRIDGE) ||
                (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                                TR_L2_HASH_KEY_TYPE_VFI)) {
                sal_memcpy(l2x_entry, soc_mem_entry_null(unit, L2_ENTRY_ONLYm),
                           sizeof(l2_entry_only_entry_t));
            }
        }
        if ((rv = soc_mem_write_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL, 
                                     idx, index_max, buffer)) < 0) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    }

    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }
    soc_cm_sfree(unit, buffer);

    /* Clear external L2 table if it exists */
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm)) {
        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, EXT_L2_ENTRY_TCAMm,
                                          MEM_BLOCK_ALL, TRUE));
        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, EXT_L2_ENTRY_DATAm,
                                          MEM_BLOCK_ALL, TRUE));
    }
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *      bcm_tr_l2_init
 * Description:
 *      Initialize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_tr_l2_init(int unit)
{
    int         was_running = FALSE;
    uint32      flags;
    sal_usecs_t interval;

    if (soc_l2x_running(unit, &flags, &interval)) { 	 
        was_running = TRUE; 	 
        BCM_IF_ERROR_RETURN(soc_l2x_stop(unit)); 	 
    }

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        if (!(SAL_BOOT_QUICKTURN || SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM)) {
            _tr_l2x_delete_all(unit);
        }
    }

    if (_mbi_entries[unit] != NULL) {
        sal_free(_mbi_entries[unit]);
        _mbi_entries[unit] = NULL;
    }

    _mbi_num[unit] = (SOC_MEM_INFO(unit, MAC_BLOCKm).index_max -
                      SOC_MEM_INFO(unit, MAC_BLOCKm).index_min + 1);
    _mbi_entries[unit] = sal_alloc(_mbi_num[unit] *
                                   sizeof(_bcm_mac_block_info_t),
                                   "BCM L2X MAC blocking info");
    if (!_mbi_entries[unit]) {
        return BCM_E_MEMORY;
    }

    sal_memset(_mbi_entries[unit], 0,
               _mbi_num[unit] * sizeof(_bcm_mac_block_info_t));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l2_reload_mbi(unit));
    }
#endif

    /* bcm_l2_register clients */
    
    soc_l2x_register(unit,
            _bcm_l2_register_callback,
            NULL);

    if (was_running || SAL_BOOT_BCMSIM) {
        interval = (SAL_BOOT_BCMSIM)? BCMSIM_L2XMSG_INTERVAL : interval;
        soc_l2x_start(unit, flags, interval);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_l2_term
 * Description:
 *      Finalize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_tr_l2_term(int unit)
{
    if (_mbi_entries[unit] != NULL) {
        sal_free(_mbi_entries[unit]);
        _mbi_entries[unit] = NULL;
    }

    return BCM_E_NONE;
}

static int
_bcm_tr_dual_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                             bcm_l2_addr_t *cf_array, int cf_max,
                             int *cf_count)
{
    l2x_entry_t         l2ent;
    uint8               key[XGS_HASH_KEY_SIZE];
    uint32              tmp_hs;
    int                 hash_sel = 0, bucket, bucket_chunk, slot, bank, num_bits;

    *cf_count = 0;

    for (bank = 0; bank < 2; bank++) {
        /* Get L2 hash select */
        if (bank > 0) {
            SOC_IF_ERROR_RETURN(READ_L2_AUX_HASH_CONTROLr(unit, &tmp_hs));
            if (soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                  tmp_hs, ENABLEf)) {
                hash_sel = soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                             tmp_hs, HASH_SELECTf);
            }
        } else {
            SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
            hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                         tmp_hs, L2_AND_VLAN_MAC_HASH_SELECTf);
        }

        num_bits = soc_tr_l2x_param_to_key(unit, addr->mac, addr->vid, key);
        bucket = soc_tr_l2x_hash(unit, hash_sel, num_bits, key);

        bucket_chunk = SOC_L2X_BUCKET_SIZE / 2;

        for (slot = bucket_chunk * bank;
             (slot < (bucket_chunk * (bank + 1))) && (*cf_count < cf_max);
             slot++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY,
                              bucket * SOC_L2X_BUCKET_SIZE + slot,
                              &l2ent));
            if (!soc_L2Xm_field32_get(unit, &l2ent, VALIDf)) {
                continue;
            }
            if ((soc_L2Xm_field32_get(unit, &l2ent, KEY_TYPEf) ==
                                      TR_L2_HASH_KEY_TYPE_BRIDGE) ||
                (soc_L2Xm_field32_get(unit, &l2ent, KEY_TYPEf) ==
                                      TR_L2_HASH_KEY_TYPE_VFI)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_l2_from_l2x(unit, &cf_array[*cf_count], &l2ent));
                *cf_count += 1;
            }
        }
    }

    return BCM_E_NONE;
}

int
bcm_tr_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                          bcm_l2_addr_t *cf_array, int cf_max,
                          int *cf_count)
{
    l2x_entry_t         l2ent;
    uint8               key[XGS_HASH_KEY_SIZE];
    int                 hash_sel, bucket, slot, num_bits;
    uint32              hash_control;

    if (soc_feature(unit, soc_feature_dual_hash)) {
        return _bcm_tr_dual_l2_conflict_get(unit, addr, cf_array,
                                            cf_max, cf_count);
    }

    *cf_count = 0;

    /* Get L2 hash select */
    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    hash_sel = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                    L2_AND_VLAN_MAC_HASH_SELECTf);
    num_bits = soc_tr_l2x_param_to_key(unit, addr->mac, addr->vid, key);
    bucket = soc_tr_l2x_hash(unit, hash_sel, num_bits, key);

    for (slot = 0;
         slot < SOC_L2X_BUCKET_SIZE && *cf_count < cf_max;
         slot++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY,
                          bucket * SOC_L2X_BUCKET_SIZE + slot,
                          &l2ent));
        if (!soc_L2Xm_field32_get(unit, &l2ent, VALIDf)) {
            continue;
        }
        if ((soc_L2Xm_field32_get(unit, &l2ent, KEY_TYPEf) == 
                                  TR_L2_HASH_KEY_TYPE_BRIDGE) ||
            (soc_L2Xm_field32_get(unit, &l2ent, KEY_TYPEf) ==
                                  TR_L2_HASH_KEY_TYPE_VFI)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_from_l2x(unit, &cf_array[*cf_count], &l2ent));
            *cf_count += 1;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_add
 * Purpose:
 *      Add a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 *      port_1     - First port in the cross-connect
 *      port_2     - Second port in the cross-connect
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr_l2_cross_connect_add(int unit, bcm_vlan_t outer_vlan, 
                            bcm_vlan_t inner_vlan, bcm_gport_t port_1, 
                            bcm_gport_t port_2)
{
    l2x_entry_t  l2x_entry, l2x_lookup;
    int rv, gport_id, l2_index;
    bcm_port_t port_out;
    bcm_module_t mod_out;
    bcm_trunk_t trunk_id;

    sal_memset(&l2x_entry, 0, sizeof (l2x_entry));
    if ((outer_vlan < 1) || (outer_vlan > 4095)) {
        return BCM_E_PARAM;
    } else if (inner_vlan == BCM_VLAN_INVALID) {
        /* Single cross-connect (use only outer_vid) */
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT);
    } else {
        if ((inner_vlan < 1) || (inner_vlan > 4095)) {
            return BCM_E_PARAM;
        }
        /* Double cross-connect (use both outer_vid and inner_vid) */
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT);
        soc_L2Xm_field32_set(unit, &l2x_entry, IVIDf, inner_vlan);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_BITf, 1);
    soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, outer_vlan);

    /* See if the entry already exists */
    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index, 
                        (void *)&l2x_entry, (void *)&l2x_lookup, 0);
                 
    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
         return rv;
    } 

    /* Resolve first port */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port_1, &mod_out, &port_out, &trunk_id,
                                &gport_id));
    if (BCM_GPORT_IS_TRUNK(port_1)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, Tf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, TGIDf, trunk_id);
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(port_1)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, VPG_TYPEf, 1);
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
           if (SOC_IS_SC_CQ(unit)) {
               /* Map the gport_id to index to L3_NEXT_HOP */
               gport_id = (int) _sc_subport_group_index[unit][gport_id/8];
           }
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
        soc_L2Xm_field32_set(unit, &l2x_entry, VPGf, gport_id);
    } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, MODULE_IDf, mod_out);
        soc_L2Xm_field32_set(unit, &l2x_entry, PORT_NUMf, port_out);
    }

    /* Resolve second port */
    BCM_IF_ERROR_RETURN 
        (_bcm_esw_gport_resolve(unit, port_2, &mod_out, &port_out, &trunk_id,
                                &gport_id));
    if (BCM_GPORT_IS_TRUNK(port_2)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, T_1f, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, TGID_1f, trunk_id);
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(port_2)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, VPG_TYPE_1f, 1);
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
           if (SOC_IS_SC_CQ(unit)) {
               /* Map the gport_id to index to L3_NEXT_HOP */
               gport_id = (int) _sc_subport_group_index[unit][gport_id/8];
           }
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
        soc_L2Xm_field32_set(unit, &l2x_entry, VPG_1f, gport_id);
    } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, MODULE_ID_1f, mod_out);
        soc_L2Xm_field32_set(unit, &l2x_entry, PORT_NUM_1f, port_out);
    }

    rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ANY, (void *)&l2x_entry);
    return rv;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete
 * Purpose:
 *      Delete a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr_l2_cross_connect_delete(int unit, bcm_vlan_t outer_vlan, 
                               bcm_vlan_t inner_vlan)
{
    l2x_entry_t  l2x_entry, l2x_lookup;
    int rv, l2_index;

    sal_memset(&l2x_entry, 0, sizeof (l2x_entry));
    if ((outer_vlan < 1) || (outer_vlan > 4095)) {
        return BCM_E_PARAM;
    } else if (inner_vlan == BCM_VLAN_INVALID) {
        /* Single cross-connect (use only outer_vid) */
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT);
    } else {
        if ((inner_vlan < 1) || (inner_vlan > 4095)) {
            return BCM_E_PARAM;
        }
        /* Double cross-connect (use both outer_vid and inner_vid) */
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT);
        soc_L2Xm_field32_set(unit, &l2x_entry, IVIDf, inner_vlan);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, outer_vlan);

    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index, 
                        (void *)&l2x_entry, (void *)&l2x_lookup, 0);
                 
    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
         return rv;
    } 

    rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ANY, (void *)&l2x_entry);
    return rv;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete_all
 * Purpose:
 *      Delete all VLAN cross connect entries
 * Parameters:
 *      unit       - Device unit number
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr_l2_cross_connect_delete_all(int unit)
{
    soc_control_t  *soc = SOC_CONTROL(unit);
    int index_min, index_max, index, mem_max;
    l2_entry_only_entry_t *l2x_entry;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    int mem_size, idx;

    index_min = soc_mem_index_min(unit, L2_ENTRY_ONLYm);
    mem_max = soc_mem_index_max(unit, L2_ENTRY_ONLYm);
    mem_size =  DEFAULT_L2DELETE_CHUNKS * sizeof(l2_entry_only_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "L2_ENTRY_ONLY_delete");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }

    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += DEFAULT_L2DELETE_CHUNKS) {
        index_max = idx + DEFAULT_L2DELETE_CHUNKS - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        for (index = 0; index < DEFAULT_L2DELETE_CHUNKS; index++) {
            l2x_entry =
                soc_mem_table_idx_to_pointer(unit, L2_ENTRY_ONLYm,
                                             l2_entry_only_entry_t *, buffer, index);
            if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VALIDf) &&
                ((soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                      TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT) ||
                 (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                      TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT))) {
                sal_memcpy(l2x_entry, soc_mem_entry_null(unit, L2_ENTRY_ONLYm),
                           sizeof(l2_entry_only_entry_t));
            }
        }
        if ((rv = soc_mem_write_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    }

    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }
    soc_cm_sfree(unit, buffer);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_traverse
 * Purpose:
 *      Walks through the valid cross connect entries and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr_l2_cross_connect_traverse(int unit,
                                 bcm_vlan_cross_connect_traverse_cb cb,
                                 void *user_data)
{
    int index_min, index_max, index, mem_max;
    l2_entry_only_entry_t *l2x_entry;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    int mem_size, idx;
    bcm_gport_t port_1, port_2;
    bcm_vlan_t outer_vlan, inner_vlan;
    bcm_port_t port_in, port_out;
    bcm_module_t mod_in, mod_out;
    
    index_min = soc_mem_index_min(unit, L2_ENTRY_ONLYm);
    mem_max = soc_mem_index_max(unit, L2_ENTRY_ONLYm);
    mem_size =  DEFAULT_L2DELETE_CHUNKS * sizeof(l2_entry_only_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "cross connect traverse");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }
    
    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += DEFAULT_L2DELETE_CHUNKS) {
        index_max = idx + DEFAULT_L2DELETE_CHUNKS - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        for (index = 0; index < DEFAULT_L2DELETE_CHUNKS; index++) {
            l2x_entry = 
                soc_mem_table_idx_to_pointer(unit, L2_ENTRY_ONLYm,
                                             l2_entry_only_entry_t *, buffer, index);
            if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VALIDf)) {
                if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                     TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT) {
                    /* Double cross-connect entry */
                    inner_vlan = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VLAN_IDf);
                } else if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                     TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT) {
                    /* Single cross-connect entry */
                    inner_vlan = BCM_VLAN_INVALID;
                } else {
                    /* Not a cross-connect entry, ignore */
                    continue;
                }
                outer_vlan = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VLAN_IDf);

                /* Get first port params */
                if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VPG_TYPEf)) {
                    int vpg;
                    vpg = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VPGf);
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
                /* Scorpion uses index to L3_NEXT_HOP as VPG */
                    if (SOC_IS_SC_CQ(unit)) {
                        int grp;
                        _SC_SUBPORT_VPG_FIND(unit, vpg, grp);
                        if ((vpg = grp) == -1) {
                            L2_ERR(("Unit: %d can not find entry for VPG\n", unit));
                        }
                    }             
#endif  /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */     
                    BCM_GPORT_SUBPORT_GROUP_SET(port_1, vpg);
                } else if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, Tf)) {
                    BCM_GPORT_TRUNK_SET(port_1, 
                        soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, TGIDf));
                } else {
                    port_in = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, PORT_NUMf);
                    mod_in = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, MODULE_IDf);
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                mod_in, port_in, &mod_out, &port_out));
                    BCM_GPORT_MODPORT_SET(port_1, mod_out, port_out);
                }

                /* Get second port params */
                if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VPG_TYPE_1f)) {
                    int vpg;
                    vpg = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VPG_1f);
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
                    if (SOC_IS_SC_CQ(unit)) {
                        int grp;
                        _SC_SUBPORT_VPG_FIND(unit, vpg, grp);
                        if ((vpg = grp) == -1) {
                            L2_ERR(("Unit: %d can not find entry for VPG\n", unit));
                        }
                    }             
#endif  /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */     
                    BCM_GPORT_SUBPORT_GROUP_SET(port_2, vpg);
                } else if (soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, T_1f)) {
                    BCM_GPORT_TRUNK_SET(port_2, 
                        soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, TGID_1f));
                } else {
                    port_in = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, PORT_NUM_1f);
                    mod_in = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, MODULE_ID_1f);
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                mod_in, port_in, &mod_out, &port_out));
                    BCM_GPORT_MODPORT_SET(port_2, mod_out, port_out);
                }

                /* Call application call-back */
                cb(unit, outer_vlan, inner_vlan, port_1, port_2, user_data);
            }
        }
    }
    soc_cm_sfree(unit, buffer);
    soc_mem_unlock(unit, L2Xm);

    return BCM_E_NONE;
}



/*
 * Function:
 *     _bcm_tr_age_reg_config
 * Description:
 *     Helper function to _bcm_tr_l2_addr_replace_by_XXX functions to 
 *     configure PER_PORT_AGE_CONTROL register for Triumph
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace 
 * Return:
 *     BCM_E_XXX
 */
STATIC int 
_bcm_tr_age_reg_config(int unit, uint32 flags, _bcm_l2_replace_t *rep_st)
{
    soc_reg_t       age_reg_array[2] = {PER_PORT_AGE_CONTROLr, INVALIDr};
    uint32          age_val_array[2] = {0,0}, op = 0;
    uint32          sync_op, soc_flags;
    int             rv, i;
    soc_control_t   *soc = SOC_CONTROL(unit);
    
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm) > 0) {
        age_reg_array[1] = ESM_PER_PORT_AGE_CONTROLr;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_get_op_from_flags(flags, &op, &sync_op));

    for (i = 0; i < COUNTOF(age_reg_array); i++) {
        if (INVALIDr == age_reg_array[i]) {
            continue;
        }
        soc_reg_field_set(unit, age_reg_array[i], &(age_val_array[i]), 
                          PPA_MODEf, op);
        soc_reg_field_set(unit, age_reg_array[i], &(age_val_array[i]), 
                          VLAN_IDf, rep_st->match_vid);
        
        if (rep_st->isTrunk) {
            soc_reg_field_set(unit, age_reg_array[i], &(age_val_array[i]),
                              Tf, 1);
            soc_reg_field_set(unit, age_reg_array[i], &(age_val_array[i]),
                              TGIDf, rep_st->match_trunk);
            if (sync_op == SOC_L2X_PORTMOD_DEL) {
                sync_op = SOC_L2X_TRUNK_DEL;
            }
        } else {
            soc_reg_field_set(unit, age_reg_array[i], &(age_val_array[i]),
                              MODULE_IDf, rep_st->match_module);
            soc_reg_field_set(unit, age_reg_array[i], &(age_val_array[i]),
                              PORT_NUMf, rep_st->match_port);
        }
    }

    rv = soc_l2x_port_age(unit, age_val_array[0], age_reg_array[1], 
                          age_val_array[1]);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
        return BCM_E_RESOURCE;
    }
    if ((flags & BCM_L2_REPLACE_DELETE) &&
        !(flags & BCM_L2_REPLACE_NO_CALLBACKS)) {
        soc_flags = (flags & BCM_L2_REPLACE_MATCH_STATIC) ? SOC_L2X_INC_STATIC : 0;
        rv = _soc_l2x_sync_delete_by(unit, rep_st->match_module, rep_st->match_port,
                                     rep_st->match_vid, rep_st->match_trunk, 0,
                                     soc_flags, sync_op);
    }
    SOC_L2_DEL_SYNC_UNLOCK(soc);
    return rv;
}

/*
 * Function:
 *     _bcm_tr_repl_reg_config
 * Description:
 *     Helper function to _bcm_tr_l2_addr_replace_by_XXX functions to 
 *     configure PER_PORT_REPL_CONTROL register for Firebolt
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace 
 *     vpn          indicates VPLS VPN suport
 * Return:
 *     BCM_E_XXX
 */
STATIC int 
_bcm_tr_repl_reg_config(int unit, uint32 flags, _bcm_l2_replace_t *rep_st, int vpn)
{
    soc_reg_t   repl_reg_array[2] = {PER_PORT_REPL_CONTROLr, INVALIDr};
    uint32      repl_val_array[2];
    uint32      pending = 0;
    int         i = 0;

    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm) > 0) {
        repl_reg_array[1] = ESM_PER_PORT_REPL_CONTROLr;
    }
    if (flags & BCM_L2_REPLACE_PENDING) {
        pending = 1;
    }

    for (i = 0; i < COUNTOF(repl_reg_array); i++) {
        if (INVALIDr == repl_reg_array[i]) {
            continue;
        }
        SOC_IF_ERROR_RETURN(
            soc_reg32_read(unit, soc_reg_addr(unit, repl_reg_array[i], 
                           REG_PORT_ANY, 0), &repl_val_array[i]));
        soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]), 
                         EXCL_STATICf,
                         (flags & BCM_L2_REPLACE_MATCH_STATIC) ? 0 : 1);
        if (flags & BCM_L2_REPLACE_NEW_TRUNK) {
            soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]),
                              Tf, 1);
            soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]),
                              TGIDf, rep_st->new_trunk);
        } else {
            soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]),
                              MODULE_IDf, rep_st->new_module);
            soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]),
                              PORT_NUMf, rep_st->new_port);
        }
        if (soc_reg_field_valid(unit, repl_reg_array[i], EXCL_NON_PENDINGf)) {  
            soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]),
                              EXCL_NON_PENDINGf, pending);
            soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]),
                              EXCL_PENDINGf, (!pending));            
        }
        if (-1 != vpn) {
            if (soc_reg_field_valid(unit, repl_reg_array[i], KEY_TYPEf)) {
                soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]),
                                  KEY_TYPEf, 0x3); /* VFI for internal */
            }
            if (soc_reg_field_valid(unit, repl_reg_array[i], KEY_TYPE_VFIf)) {
                soc_reg_field_set(unit, repl_reg_array[i], &(repl_val_array[i]),
                                  KEY_TYPE_VFIf, 0x1); /* VFI external */
            }
        }
        SOC_IF_ERROR_RETURN(
            soc_reg32_write(unit, soc_reg_addr(unit, repl_reg_array[i], 
                           REG_PORT_ANY, 0), repl_val_array[i]));

        /*
         * Unlike L2_MOD_FIFO, EXT_L2_MOD_FIFO does not report both new and
         * replaced L2 destination for PPA replace command. To workaround
         * the problem, we add an special entry to EXT_L2_MOD_FIFO before
         * issuing the PPA replace command. The special entry has the new
         * destination and a special "type" value. L2 mod fifo processing
         * thread knows all entries after this special entry are associated
         * with this new destination.
         */
        if (repl_reg_array[i] == ESM_PER_PORT_REPL_CONTROLr) {
            ext_l2_mod_fifo_entry_t ext_l2_mod_entry;
            ext_l2_entry_entry_t ext_l2_entry;

            sal_memset(&ext_l2_mod_entry, 0, sizeof(ext_l2_mod_entry));
            sal_memset(&ext_l2_entry, 0, sizeof(ext_l2_entry));

            if (flags & BCM_L2_REPLACE_NEW_TRUNK) {
                soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, Tf, 1);
                soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, TGIDf,
                                    rep_st->new_trunk);
            } else {
                soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                    MODULE_IDf, rep_st->new_module);
                soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                    PORT_NUMf, rep_st->new_port);
            }
            soc_mem_field_set(unit, EXT_L2_MOD_FIFOm,
                              (uint32 *)&ext_l2_mod_entry, WR_DATAf,
                              (uint32 *)&ext_l2_entry);
            soc_mem_field32_set(unit, EXT_L2_MOD_FIFOm, &ext_l2_mod_entry,
                                TYPf, 3); /* use INSERTED as special type */
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, EXT_L2_MOD_FIFOm,
                                              MEM_BLOCK_ANY, 0,
                                              &ext_l2_mod_entry));
        }
    }
    return BCM_E_NONE;
}

/*
 * Functions:
 *      _bcm_tr_l2_delete_dest
 * Description:
 *      Helper function to delete the required non-static entries via DMA 
 * Parameters:
 *     unit         device number
 *     rep_st       structure with info of what to match 
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_delete_mpls_dest (int unit, uint32 flags, _bcm_l2_replace_t *rep_st)
{
    int           ix, rv;
    int           chunksize, nchunks, chunk;
    l2x_entry_t   *l2xe, *l2xep;
    soc_control_t *soc = SOC_CONTROL(unit);
    int           dyn_only = (flags & BCM_L2_REPLACE_MATCH_STATIC) ? 0 : 1;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 DEFAULT_L2DELETE_CHUNKS);

    l2xe = soc_cm_salloc(unit, chunksize * sizeof(*l2xe), "l2entrydel_chunk");
    if (l2xe == NULL) {
        return BCM_E_MEMORY;
    }

    nchunks = soc_mem_index_count(unit, L2Xm) / chunksize;
    rv = BCM_E_NONE;

    soc_mem_lock(unit, L2Xm);
    for (chunk = 0; chunk < nchunks; chunk++) {
        rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                                chunk * chunksize,
                                (chunk + 1) * chunksize - 1,
                                l2xe);
        if (rv < 0) {
            break;
        }
        for (ix = 0; ix < chunksize; ix++) {
            l2xep = soc_mem_table_idx_to_pointer(unit, L2Xm,
                                                 l2x_entry_t *, l2xe, ix);
            if (!soc_L2Xm_field32_get(unit, l2xep, VALIDf)) {
                continue;
            }
            if (dyn_only &&  soc_L2Xm_field32_get(unit, l2xep, STATIC_BITf)) {
                continue;
            }
            if (rep_st->isMpls) {
                 /* Match the Key Type to VPLS VFI */
                if ( soc_L2Xm_field32_get(unit, l2xep, KEY_TYPEf) != 0x3) {
                    continue;
                }
                if ( soc_L2Xm_field32_get(unit, l2xep, DESTINATIONf) != rep_st->mpls_vp) {
                    continue;
                }
            }
            if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                rv = BCM_E_RESOURCE;
                break;
            }
            if (((rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, l2xep)) < 0) ||
               ((rv = soc_l2x_sync_delete(unit, (uint32 *) l2xep,
                                          ((chunk * chunksize) + ix))) < 0)) {
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                break;
            }
            SOC_L2_DEL_SYNC_UNLOCK(soc);
        }
        if (rv < 0) {
            break;
        }
    }
    soc_mem_unlock(unit, L2Xm);
    soc_cm_sfree(unit, l2xe);

    return rv;
}


/*
 * Function:
 *     _bcm_tr_l2_addr_replace_by_vlan
 * Description:
 *     Helper function to _bcm_l2_addr_replace_by_vlan API to replace l2 entries 
 *      by mac for Triumph 
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       Replace structure   
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_tr_l2_addr_replace_by_vlan_dest(int unit, uint32 flags, _bcm_l2_replace_t *rep_st)
{
    int vfi = -1;

    if (NULL == rep_st) {
        return BCM_E_PARAM;
    }
    if (flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (_BCM_MPLS_VPN_IS_VPLS(rep_st->match_vid)) {
            vfi = _BCM_MPLS_VPN_ID_GET(rep_st->match_vid);
            if (vfi >= soc_mem_index_count(unit, VFIm)) {
                return BCM_E_PARAM;
            }
            rep_st->match_vid = vfi;
        } else if (_BCM_MIM_VPN_IS_SET(rep_st->match_vid)) {
            vfi = _BCM_MIM_VPN_ID_GET(rep_st->match_vid);
            if (vfi >= soc_mem_index_count(unit, VFIm)) {
                return BCM_E_PARAM;
            }
            rep_st->match_vid = vfi;
        }
        VLAN_CHK_ID(unit, rep_st->match_vid);
    }

    if ((rep_st->isMpls) && (flags & BCM_L2_REPLACE_DELETE)) {
         BCM_IF_ERROR_RETURN(_bcm_tr_l2_delete_mpls_dest (unit, flags, rep_st));
    } else {
         BCM_IF_ERROR_RETURN(
              _bcm_tr_repl_reg_config(unit, flags, rep_st, vfi));
         BCM_IF_ERROR_RETURN(
              _bcm_tr_age_reg_config(unit, flags, rep_st));
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_tr_l2_sw_dump
 * Purpose:
 *     Displays L2 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_tr_l2_sw_dump(int unit)
{
    _bcm_mac_block_info_t *mbi;
     char                 pfmt[SOC_PBMP_FMT_LEN];
     int                  i;

    soc_cm_print("\n");
    soc_cm_print("  TR L2 MAC Blocking Info -\n");
    soc_cm_print("      Number : %d\n", _mbi_num[unit]);

    mbi = _mbi_entries[unit];
    soc_cm_print("      Entries (index: pbmp-count) :\n");
    if (mbi != NULL) {
        for (i = 0; i < _mbi_num[unit]; i++) {
            SOC_PBMP_FMT(mbi[i].mb_pbmp, pfmt);
            soc_cm_print("          %5d: %s-%d\n", i, pfmt, mbi[i].ref_count);
        }
    }

    soc_cm_print("\n");

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#else /* BCM_TRX_SUPPORT */
int bcm_esw_triumph_l2_not_empty;
#endif  /* BCM_TRX_SUPPORT */

