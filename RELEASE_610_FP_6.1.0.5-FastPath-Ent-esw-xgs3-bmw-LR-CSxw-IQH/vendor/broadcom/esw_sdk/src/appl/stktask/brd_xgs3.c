/* 
 * $Id: brd_xgs3.c,v 1.1 2011/04/18 17:11:00 mruas Exp $
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
 * File:        brd_xgs3.c
 * Purpose:     XGS3 Based SDK and Reference Design board programming
 */

#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/trunk.h>
#include <bcm/topo.h>
#include <bcm/init.h>               /* bcm_info_get */

#include <appl/stktask/topo_brd.h>
#include <sal/appl/config.h>

#include "topo_int.h"

/* Set up a Firebolt device. */
STATIC int
_bcm_board_xgs3_fb(int unit, topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                 i, m;
    topo_stk_port_t     *tsp;
    int                 port;
    cpudb_entry_t       *l_entry;
    bcm_module_t        mod;
    bcm_port_config_t   config;
    
    /*
     * Generate the bitmap of egress ports for each mod id;
     * Do not exclude the higig port from the bitmap b/c the
     * FB ucbitmap set applies to the chip, not the ingress port.
     */

    l_entry = &tp_cpu->local_entry;
    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        if (unit != l_entry->base.stk_ports[i].unit) {
            /* Only looking at this device for now */
            continue;
        }

        /* For each mod-id reached by this port, set in device */
        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;

        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, mod, port));
        }
    }
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref,
                                                &config));
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    return BCM_E_NONE;
}

STATIC int
_bcm_board_banner(int unit, char *prefix)
{
    bcm_info_t info;
    uint32 dev;

    BCM_IF_ERROR_RETURN(bcm_info_get(unit, &info));

    /* Make the device ID correspond to the actual part number */

    dev = info.device;
    
    if (dev > 0x5699) {
        dev += 0x4b000;
    }
    
    TOPO_VERB(("TOPO: %sBCM%x SDK\n", prefix, dev));

    return BCM_E_NONE;
}

/* Single Fireolt or Firebolt-like device on a board, unit number is 0 */

#define FB_UNIT 0
int
bcm_board_topo_sdk_xgs3_24g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int         modid;

    _bcm_board_banner(FB_UNIT, "");

    /* Use the default stackable mod ID mapping since only one unit on brd */
    bcm_topo_map_set(bcm_board_topomap_stk);

    modid = tp_cpu->local_entry.mod_ids[FB_UNIT];
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(FB_UNIT, modid));

    return _bcm_board_xgs3_fb(FB_UNIT, tp_cpu, db_ref);
}
#undef FB_UNIT

/* Support functions for dual XGS3 SDK and Reference designs */

/* Internal trunk setup */

#define NUMXGS3DEV 2

STATIC int
_bcm_board_setup_trunk_dual_xgs3(xgs3devinfo_t dev[NUMXGS3DEV])
{
    int i;
    bcm_trunk_add_info_t     trunk;
    bcm_trunk_chip_info_t    ti;
    
    sal_memset(&trunk, 0, sizeof(trunk));
    trunk.psc        = -1;
    trunk.dlf_index  = -1;
    trunk.mc_index   = -1;
    trunk.ipmc_index = -1;
    trunk.num_ports  = 2;

    for ( i = 0; i < NUMXGS3DEV; i++ ) {
        
        trunk.tp[0]      = dev[i].intport[0];
        trunk.tp[1]      = dev[i].intport[1];

        BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(dev[i].unit, &ti));
        bcm_trunk_destroy(dev[i].unit, ti.trunk_fabric_id_max);
        BCM_IF_ERROR_RETURN(bcm_trunk_create_id(dev[i].unit,
                                                ti.trunk_fabric_id_max));
        BCM_IF_ERROR_RETURN(bcm_trunk_set(dev[i].unit,
                                          ti.trunk_fabric_id_max, &trunk));
    }
    
    return BCM_E_NONE;
}


/* Generic board programming for dual XGS3 designs */

STATIC int
_bcm_board_dual_xgs3(xgs3devinfo_t dev[NUMXGS3DEV], topo_cpu_t *tp_cpu,
                     cpudb_ref_t db_ref)
{
    int                 i, m;
    topo_stk_port_t     *tsp;
    int                 port;
    cpudb_entry_t       *l_entry;
    bcm_module_t        mod;
    bcm_port_config_t   config;
    int                 sp, np;
    int                 auto_trunk;

    /* Set devinfo device modids */
    for ( i = 0; i < NUMXGS3DEV; i++ ) {
        dev[i].modid = tp_cpu->local_entry.mod_ids[i];
    }
    
    l_entry = &tp_cpu->local_entry;

    /* Set module mapping based on modids reachable by the stack ports */
    
    for ( i = 0; i < l_entry->base.num_stk_ports; i++) {
        if (!(l_entry->sp_info[i].flags & CPUDB_SPF_TX_RESOLVED)) {
            /* Skip unresolved ports */
            continue;
        }

        /* Set sp (stack-port-dev) and np (non-stack-port-dev) */
        if (dev[0].unit == l_entry->base.stk_ports[i].unit) {
            /* sp is the dev in devinfo that this stack port is connected to */
            /* np is the other (non stack port) dev */
            sp = 0;
            np = 1;
        } else {
            sp = 1;
            np = 0;
        }

        /* For each modid reached by this port, set both devices (s
           and np). For sp (the device this stack port is on), set the
           modmap to the stack port. For np (the device this stack
           port is *not* on), set the modmap to the internal port. */

        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;

        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(dev[sp].unit, mod, port));
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(dev[np].unit, mod,
                                                    dev[np].intport[0]));
        }
    }

    /* Set the module mapping, internal stack ports and module
       filtering for the two devices themselves. */

    BCM_IF_ERROR_RETURN(bcm_board_auto_trunk_get(&auto_trunk));
    for ( i = 0; i < NUMXGS3DEV; i++ ) {
        int j = i^1; /* The other device */
        
        BCM_IF_ERROR_RETURN(bcm_stk_modport_set(dev[i].unit,
                                                dev[j].modid,
                                                dev[i].intport[0]));
        BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(dev[i].unit,
                                                        dev[i].intport[0]));
        if (dev[i].intport[1] >= 0 && auto_trunk) {
            BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(dev[i].unit,
                                                       dev[i].intport[1]));
        }
        BCM_IF_ERROR_RETURN(bcm_port_config_get(dev[i].unit, &config));
        BCM_IF_ERROR_RETURN(bcm_board_module_filter(dev[i].unit,
                                                    tp_cpu, db_ref, &config));

        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(dev[i].unit,
                                                 dev[i].modid));
    }

    /* Set up internal trunking */

    if (dev[0].intport[1] >= 0 && dev[1].intport[1] >= 0) {
        _bcm_board_setup_trunk_dual_xgs3(dev);
    }
    
    /* Set up external trunking */
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

#ifdef BCM_BOARD_AUTO_E2E
    /* intra-board end to end flow control, if available */
    BCM_IF_ERROR_RETURN(_bcm_board_dual_xgs3_e2e_set(dev[0].unit, dev[0].modid,
                                                     dev[1].unit,
                                                     dev[1].modid));
#endif  /* BCM_BOARD_AUTO_E2E */

    return BCM_E_NONE;
}

/* sdk_xgs3_48f */

xgs3devinfo_t xgs3_48f_devinfo[NUMXGS3DEV] = {
    {
        0,              /* unit */
        -1,             /* modid (dynamic) */
        { 26, -1 }      /* internal stack port (not trunked) */
    },
    {
        1,              /* unit */
        -1,             /* modid (dynamic) */
        { 26, -1 }      /* internal stack port (not trunked) */
    }
};


int
bcm_board_topo_xgs3_48f(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    TOPO_VERB(("TOPO: XGS3 (2)56102 48FE+2GE+2XHG\n"));

    BCM_IF_ERROR_RETURN
        (_bcm_board_dual_xgs3(xgs3_48f_devinfo, tp_cpu, db_ref));

    return BCM_E_NONE;
}

/* sdk_xgs3_48g */

xgs3devinfo_t xgs3_48g_devinfo[NUMXGS3DEV] = {
    {
        0,              /* unit */
        -1,             /* modid (dynamic) */
        { 26, 27 }      /* internal stack ports (trunked) */
    },
    {
        1,              /* unit */
        -1,             /* modid (dynamic) */
        { 26, 27 }      /* internal stack ports (trunked) */
    }
};

int
bcm_board_topo_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    _bcm_board_banner(0, "(2)");

    BCM_IF_ERROR_RETURN
        (_bcm_board_dual_xgs3(xgs3_48g_devinfo, tp_cpu, db_ref));

    return BCM_E_NONE;
}

int
bcm_board_topo_xgs3_20x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int                  unit;
    bcm_module_t         mod;
    bcm_port_t           port;
    bcm_port_config_t    config;

    unit = 0;            /* board fabric unit */

    _bcm_board_banner(unit, "");
    
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.cpu, port) {
        break;   /* Get CPU port */
    }
    mod = db_ref->local_entry->mod_ids[0];
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, mod, port));
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(0, mod));
    BCM_IF_ERROR_RETURN(bcm_board_fab_mod_map(unit,
                                              tp_cpu,
                                              db_ref->local_entry));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref,
                                                &config));
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(&tp_cpu->local_entry,
                                               db_ref,
                                               BCM_BOARD_TRUNK_NORMAL));

    return BCM_E_NONE;
}

int
bcm_board_topo_xgs3_48g5g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    int modid;
    bcm_port_config_t    config;
    int unit = 0;

    _bcm_board_banner(unit, "");

    BCM_IF_ERROR_RETURN(bcm_board_num_modid_set(16));

    /* Use the default stackable mod ID mapping since only one unit on brd */
    bcm_topo_map_set(bcm_board_topomap_stk);

    modid = tp_cpu->local_entry.mod_ids[0];
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, modid));
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_IF_ERROR_RETURN(bcm_board_fab_mod_map(unit,
                                              tp_cpu,
                                              db_ref->local_entry));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref,
                                                &config));

    /* If there are only two CPUs, deal with any possible untrunkable
       stack ports */
    if (db_ref->num_cpus == 2) {
        BCM_IF_ERROR_RETURN(bcm_board_untrunkable_stack_ports(unit, tp_cpu));
    }

    return BCM_E_NONE;

}

/* sdk_xgs3_48g2 */

xgs3devinfo_t xgs3_48g2_devinfo[NUMXGS3DEV] = {
    {
        0,              /* unit */
        -1,             /* modid (dynamic) */
        { 1, -1 }       /* internal stack port (not trunked) */
    },
    {
        1,              /* unit */
        -1,             /* modid (dynamic) */
        { 1, -1 }       /* internal stack port (not trunked) */
    }
};


#if defined(INCLUDE_RCPU) && defined(BCM_ESW_SUPPORT)
int
bcm_board_topo_xgs3_48g2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    extern int _bcm_esw_rcpu_init(int);
    char modstr[16];
    int rv;

    _bcm_board_banner(0, "(2)");

    BCM_IF_ERROR_RETURN
        (_bcm_board_dual_xgs3(xgs3_48g2_devinfo, tp_cpu, db_ref));

    /* Disable ge2 */
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(0, 3, 0));
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(1, 3, 0));

    /* reinit RCPU subsystem with new modids */
    sal_sprintf(modstr, "%d", db_ref->local_entry->mod_ids[0]);
    if (sal_config_set("rcpu_master_modid", modstr) != 0) {
        TOPO_VERB(("TOPO: rcpu_master_modid set error\n"));
    }
    rv = _bcm_esw_rcpu_init(0);
    if (BCM_FAILURE(rv)) {
        TOPO_VERB(("TOPO: rcpu master init error %d (%s)\n",
                   rv, bcm_errmsg(rv)));
    }

    sal_sprintf(modstr, "%d", db_ref->local_entry->mod_ids[1]);
    if (sal_config_set("rcpu_slave_modid", modstr) != 0) {
        TOPO_VERB(("TOPO: rcpu_master_modid set error\n"));
    }
    rv=_bcm_esw_rcpu_init(1);
    if (BCM_FAILURE(rv)) {
        TOPO_VERB(("TOPO: rcpu slave init error %d (%s)\n",
                   rv, bcm_errmsg(rv)));
    }

    return BCM_E_NONE;
}

#else

int
bcm_board_topo_xgs3_48g2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    COMPILER_REFERENCE(tp_cpu);
    COMPILER_REFERENCE(db_ref);

    return BCM_E_UNAVAIL;
}

#endif /* defined(INCLUDE_RCPU) && defined(BCM_ESW_SUPPORT) */
