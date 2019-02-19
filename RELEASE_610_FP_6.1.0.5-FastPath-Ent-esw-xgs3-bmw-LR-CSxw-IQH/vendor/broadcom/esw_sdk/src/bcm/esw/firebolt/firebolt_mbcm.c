/* 
 * $Id: firebolt_mbcm.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:        mbcm.c
 */

#include <soc/defs.h>
#if defined(BCM_FIREBOLT_SUPPORT)
#include <soc/debug.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/draco.h>
#if defined(BCM_EASYRIDER_SUPPORT)
#include <bcm_int/esw/easyrider.h>
#endif /* BCM_EASYRIDER_SUPPORT */

#include <bcm_int/esw/firebolt.h>
#include <bcm/error.h>

/* Dummy functions not yet implemented */

static int
unimpl(char *desc)
{
    soc_cm_debug(DK_VERBOSE,
		 "WARNING: function %s not implemented for Firebolt\n", desc);
    return SOC_E_NONE;
}

#ifdef BCM_METER_SUPPORT

int bcm_fb_meter_init(int unit)
{
    return unimpl("bcm_fb_meter_init");
}

int bcm_fb_meter_create(int unit, int port, int *mid)
{
    return unimpl("bcm_fb_meter_create");
}

int bcm_fb_meter_delete(int unit, int port, int mid)
{
    return unimpl("bcm_fb_meter_delete");
}

int bcm_fb_meter_delete_all(int unit)
{
    return unimpl("bcm_fb_meter_delete_all");
}

int bcm_fb_meter_get(int unit, int port, int mid,
		     uint32 *kbits_sec, uint32 *kbits_burst)
{
    return unimpl("bcm_fb_meter_get");
}

int bcm_fb_meter_set(int unit, int port, int mid,
		     uint32 kbits_sec, uint32 kbits_burst)
{
    return unimpl("bcm_fb_meter_set");
}

int bcm_fb_ffppacketcounter_set(int unit, int port, int mid,
				uint64 val)
{
    return unimpl("bcm_fb_ffppacketcounter_set");
}

int bcm_fb_ffppacketcounter_get(int unit, int port, int mid,
				uint64 *val)
{
    return unimpl("bcm_fb_ffppacketcounter_get");
}

int bcm_fb_ffpcounter_init(int unit)
{
    return unimpl("bcm_fb_ffpcounter_init");
}

int bcm_fb_ffpcounter_create(int unit, int port, int *ffpcounterid)
{
    return unimpl("bcm_fb_ffpcounter_create");
}

int bcm_fb_ffpcounter_delete(int unit, int port, int ffpcounterid)
{
    return unimpl("bcm_fb_ffpcounter_delete");
}

int bcm_fb_ffpcounter_delete_all(int unit)
{
    return unimpl("bcm_fb_ffpcounter_delete_all");
}

int bcm_fb_ffpcounter_set(int unit, int port, int ffpcounterid,
			  uint64 val)
{
    return unimpl("bcm_fb_ffpcounter_set");
}

int bcm_fb_ffpcounter_get(int unit, int port, int ffpcounterid,
			  uint64 *val)
{
    return unimpl("bcm_fb_ffpcounter_get");
}

#endif /* BCM_METER_SUPPORT */

mbcm_functions_t mbcm_firebolt_driver = {
    /*  L2 functions */
    bcm_fb_l2_init,
    bcm_fb_l2_term,
    bcm_fb_l2_addr_get,
    bcm_fb_l2_addr_add,
    bcm_fb_l2_addr_delete,
    
    bcm_fb_l2_conflict_get,

    /*  Port table related functions */
    bcm_xgs3_port_cfg_init,
    bcm_xgs3_port_cfg_get,
    bcm_xgs3_port_cfg_set,

    /*  VLAN functions */
    bcm_xgs3_vlan_init,
    bcm_xgs3_vlan_reload,
    bcm_xgs3_vlan_create,
    bcm_xgs3_vlan_destroy,
    bcm_xgs3_vlan_port_add,
    bcm_xgs3_vlan_port_remove,
    bcm_xgs3_vlan_port_get,
    bcm_xgs3_vlan_stg_get,
    bcm_xgs3_vlan_stg_set,

    /*  Firebolt trunking functions */
    bcm_xgs3_trunk_set,
    bcm_xgs3_trunk_get,
    bcm_xgs3_trunk_destroy,
    bcm_xgs3_trunk_mcast_join,

    /*  Spanning Tree Group functions */
    bcm_xgs3_stg_stp_init,
    bcm_xgs3_stg_stp_get,
    bcm_xgs3_stg_stp_set,

    /*  Multicasting functions */
    bcm_xgs3_mcast_addr_add,
    bcm_xgs3_mcast_addr_remove,
    bcm_xgs3_mcast_port_get,
    bcm_xgs3_mcast_init,
    _bcm_xgs3_mcast_detach,
    bcm_xgs3_mcast_addr_add_w_l2mcindex,
    bcm_xgs3_mcast_addr_remove_w_l2mcindex,
    bcm_xgs3_mcast_port_add,
    bcm_xgs3_mcast_port_remove,

    /*  COSQ functions */
    bcm_fb_cosq_init,
    bcm_fb_cosq_detach,
    bcm_fb_cosq_config_set,
    bcm_fb_cosq_config_get,
    bcm_fb_er_cosq_mapping_set,
    bcm_fb_er_cosq_mapping_get,
    bcm_fb_cosq_port_sched_set,
    bcm_fb_cosq_port_sched_get,
    bcm_fb_cosq_sched_weight_max_get,
    bcm_fb_cosq_port_bandwidth_set,
    bcm_fb_cosq_port_bandwidth_get,
    bcm_fb_cosq_discard_set,
    bcm_fb_cosq_discard_get,
    bcm_fb_cosq_discard_port_set,
    bcm_fb_cosq_discard_port_get,

#ifdef BCM_METER_SUPPORT
    /*  Meter functions */
    bcm_fb_meter_init,
    bcm_fb_meter_create,
    bcm_fb_meter_delete,
    bcm_fb_meter_delete_all,
    bcm_fb_meter_get,
    bcm_fb_meter_set,
    bcm_fb_ffppacketcounter_set,
    bcm_fb_ffppacketcounter_get,
    bcm_fb_ffpcounter_init,
    bcm_fb_ffpcounter_create,
    bcm_fb_ffpcounter_delete,
    bcm_fb_ffpcounter_delete_all,
    bcm_fb_ffpcounter_set,
    bcm_fb_ffpcounter_get,
    bcm_fb_port_rate_egress_set,
    bcm_fb_port_rate_egress_get,
#endif /* BCM_METER_SUPPORT */

#ifdef INCLUDE_L3
    /* L3 functions */
    bcm_xgs3_l3_tables_init,
    bcm_xgs3_l3_tables_cleanup,
    bcm_xgs3_l3_enable,
    bcm_xgs3_l3_intf_get,
    bcm_xgs3_l3_intf_get_by_vid,
    bcm_xgs3_l3_intf_create,
    bcm_xgs3_l3_intf_id_create,
    bcm_xgs3_l3_intf_lookup,
    bcm_xgs3_l3_intf_del,
    bcm_xgs3_l3_intf_del_all,

    bcm_xgs3_l3_get,
    bcm_xgs3_l3_add,
    bcm_xgs3_l3_del,
    bcm_xgs3_l3_del_prefix,
    bcm_xgs3_l3_del_intf,
    bcm_xgs3_l3_del_all,
    bcm_xgs3_l3_replace,
    bcm_xgs3_l3_age,
    bcm_xgs3_l3_ip4_traverse,

    bcm_xgs3_l3_get,
    bcm_xgs3_l3_add,
    bcm_xgs3_l3_del,
    bcm_xgs3_l3_del_prefix,
    bcm_xgs3_l3_replace,
    bcm_xgs3_l3_ip6_traverse,

    bcm_xgs3_defip_get,
    bcm_xgs3_defip_ecmp_get_all,
    bcm_xgs3_defip_add,
    bcm_xgs3_defip_del,
    bcm_xgs3_defip_del_intf,
    bcm_xgs3_defip_del_all,
    bcm_xgs3_defip_age,
    bcm_xgs3_defip_ip4_traverse,

    bcm_xgs3_defip_get,
    bcm_xgs3_defip_ecmp_get_all,
    bcm_xgs3_defip_add,
    bcm_xgs3_defip_del,
    bcm_xgs3_defip_ip6_traverse,

    bcm_xgs3_l3_conflict_get,
    bcm_xgs3_l3_info,

    bcm_xgs3_lpm_defip_sw_tbl_empty,  /* not implemented */
    bcm_xgs3_lpm_defip_hw_tbl_empty,  /* not implemented */
    bcm_xgs3_lpm_check_table,         /* not implemented */
    bcm_xgs3_lpm_delete,              /* not implemented */
    bcm_xgs3_lpm_insert,              /* not implemented */
    bcm_xgs3_lpm_lookup,              /* not implemented */

    bcm_fb_er_ipmc_init,
    bcm_fb_er_ipmc_detach,
    bcm_fb_er_ipmc_enable,
    bcm_fb_er_ipmc_src_port_check,
    bcm_fb_er_ipmc_src_ip_search,
    bcm_fb_er_ipmc_add,
    bcm_fb_er_ipmc_delete,
    bcm_fb_er_ipmc_delete_all,
    bcm_fb_er_ipmc_lookup,
    bcm_fb_er_ipmc_get,
    bcm_fb_er_ipmc_put,
    bcm_fb_er_ipmc_egress_port_get,
    bcm_fb_er_ipmc_egress_port_set, 
    bcm_fb_ipmc_repl_init,
    bcm_fb_ipmc_repl_detach,
    bcm_fb_ipmc_repl_get,
    bcm_fb_ipmc_repl_add,
    bcm_fb_ipmc_repl_delete,
    bcm_fb_ipmc_repl_delete_all,
    bcm_fb_ipmc_egress_intf_add,
    bcm_fb_ipmc_egress_intf_delete,
    bcm_xgs3_ipmc_age, 
    bcm_xgs3_ipmc_traverse, 
#endif /* INCLUDE_L3 */
};
#else /* BCM_FIREBOLT_SUPPORT */
int _firebolt_mbcm_not_empty; 
#endif /* BCM_FIREBOLT_SUPPORT */

