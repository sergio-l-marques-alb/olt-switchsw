/*
 * $Id: init.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * BCM Library Initialization
 *
 *   This module calls the initialization routine of each BCM module.
 *
 * Initial System Configuration
 *
 *   Each module should initialize itself without reference to other BCM
 *   library modules to avoid a chicken-and-the-egg problem.  To do
 *   this, each module should initialize its respective internal state
 *   and hardware tables to match the Initial System Configuration.  The
 *   Initial System Configuration is:
 *
 *   STG 1 containing VLAN 1
 *   STG 1 all ports in the DISABLED state
 *   VLAN 1 with
 *  PBMP = all switching Ethernet ports (non-fabric) and the CPU.
 *  UBMP = all switching Ethernet ports (non-fabric).
 *   No trunks configured
 *   No mirroring configured
 *   All L2 and L3 tables empty
 *   Ingress VLAN filtering disabled
 *   BPDU reception enabled
 */

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/boot.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/robo.h>
#include <soc/phyctrl.h>

#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/l2.h>
#include <bcm/link.h>
#include <bcm/vlan.h>
#include <bcm/cosq.h>
#include <bcm/trunk.h>
#include <bcm/filter.h>
#include <bcm/field.h>
#include <bcm/diffserv.h>
#include <bcm/stat.h>
#include <bcm/mcast.h>
#include <bcm/mirror.h>
#include <bcm/tx.h>
#include <bcm/rx.h>
#include <bcm/stack.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#include <bcm/stg.h>
#include <bcm/dmux.h>
#include <bcm/auth.h>
#include <bcm/igmp.h>

#include <bcm_int/common/lock.h>
#include <bcm_int/common/family.h>

#ifdef INCLUDE_MACSEC
#include <bcm_int/common/macsec_cmn.h>
#endif /* INCLUDE_MACSEC */

/* See BCM_SEL_INIT flags in init.h */
STATIC uint32 _bcm_robo_initialized_flag[BCM_MAX_NUM_UNITS];

/* ASSUMES unit PARAMETER which is not in macro's list. */
#define SEL_CALL(rtn, name, dispname, flags) {                          \
    sal_usecs_t stime = sal_time_usecs();                   \
        if ((BCM_INIT_SEL_ ## name) & (flags)) {                        \
            BCM_IF_ERROR_RETURN(rtn(unit));                             \
            _bcm_robo_initialized_flag[unit] |= (BCM_INIT_SEL_ ## name);     \
            soc_cm_debug(DK_VERBOSE, "bcm_init: %s took %d usec\n", \
                        dispname, SAL_USECS_SUB(sal_time_usecs(), stime));  \
        }                                                               \
}

/* ASSUMES unit PARAMETER which is not in macro's list. */
#define CLEAR_CALL(_rtn, _name) {                                       \
        int rv;                                                         \
        rv = (_rtn)(unit);                                              \
        if (rv < 0 && rv != BCM_E_UNAVAIL) {                            \
            soc_cm_debug(DK_ERR, "bcm_clear %d: %s failed %d. %s\n",    \
                         unit, _name, rv, bcm_errmsg(rv));              \
            return rv;                                                  \
        }                                                               \
}

/*
 * Function:
 *  _bcm_robo_lock_init
 * Purpose:
 *  Allocate BCM_LOCK.
 */

STATIC int
_bcm_robo_lock_init(int unit)
{
    if (_bcm_lock[unit] == NULL) {
    _bcm_lock[unit] = sal_mutex_create("bcm_config_lock");
    }

    if (_bcm_lock[unit] == NULL) {
    return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_init_phy_id_map
 * Purpose:
 *  Initialize the phy id map for the unit
 * Parameters:
 *  unit - RoboSwitch unit #.
 *      phy_id_map - an array indexed by (0-based) port giving
 *          MII address
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_init_phy_id_map(int unit, int map[])
{
    /* Re-initialize the phy port map for the unit */
    return (soc_phyctrl_init_phy_id_map(unit, map));
}

/*
 * Function:
 *  bcm_robo_init
 * Purpose:
 *  Initialize the BCM API layer only, without resetting the switch chip.
 * Parameters:
 *  unit - RoboSwitch unit #.
 * Returns:
 *  BCM_E_XXX
 */

int 
bcm_robo_init(int unit)     /* Initialize chip and BCM layer */
{
    uint32 init_flags = BCM_INIT_SEL_ALL;
    
    bcm_port_t      port;
    pbmp_t          pbmp_linkscan_fe, pbmp_linkscan_ge, pbmp;
    char            pfmt[SOC_PBMP_FMT_LEN];
    int             rv;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_robo_lock_init(unit));

    if (!soc_feature(unit, soc_feature_filter)) {
        init_flags &= ~BCM_INIT_SEL_FILTER;
        init_flags &= ~BCM_INIT_SEL_DIFFSERV;
    }
    if (!soc_feature(unit, soc_feature_l3)) {
        init_flags &= ~BCM_INIT_SEL_L3;
    }
    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        init_flags &= ~BCM_INIT_SEL_IPMC;
    }
    if (!soc_feature(unit, soc_feature_dmux)) {
        init_flags &= ~BCM_INIT_SEL_DMUX;
    }
    if (!soc_feature(unit, soc_feature_auth)) {
        init_flags &= ~BCM_INIT_SEL_AUTH;
    }

    if (soc_feature(unit, soc_feature_no_stat_mib)) {
        init_flags &= ~BCM_INIT_SEL_STAT;
    }

    if (SAL_BOOT_SIMULATION) {
        if (init_flags & BCM_INIT_SEL_FILTER) {
            soc_cm_print("SIMULATION: skipped init filter\n");
            init_flags &= ~BCM_INIT_SEL_FILTER;
        }
        if (init_flags & BCM_INIT_SEL_DIFFSERV) {
            soc_cm_print("SIMULATION: skipped init diffserv\n");
            init_flags &= ~BCM_INIT_SEL_DIFFSERV;
        }
            if (init_flags & BCM_INIT_SEL_AUTH) {
                soc_cm_print("SIMULATION: skipped init auth\n");
                init_flags &= ~BCM_INIT_SEL_AUTH;
            }
        if (init_flags & BCM_INIT_SEL_L3) {
            soc_cm_print("SIMULATION: skipped init l3\n");
            init_flags &= ~BCM_INIT_SEL_L3;
        }
        if (init_flags & BCM_INIT_SEL_STAT) {
            soc_cm_print("SIMULATION: skipped init stat\n");
            init_flags &= ~BCM_INIT_SEL_STAT;
        }
    }

    /* If linkscan is running, disable it. */
    bcm_linkscan_enable_set(unit, 0);

#ifdef INCLUDE_MACSEC
    BCM_IF_ERROR_RETURN(_bcm_common_macsec_init(unit)); 
#endif /* INCLUDE_MACSEC */

    BCM_IF_ERROR_RETURN(bcm_init_selective(unit, init_flags));

    if(soc_feature(unit, soc_feature_igmp_ip)) {
        bcm_igmp_snooping_init(unit);
    }

    /* Enable LinkScan task at all port */
    bcm_linkscan_enable_set(unit, 25000);
    BCM_PBMP_ASSIGN(pbmp_linkscan_fe, PBMP_FE_ALL(unit));
    BCM_PBMP_ASSIGN(pbmp_linkscan_ge, PBMP_GE_ALL(unit));
    BCM_PBMP_OR(pbmp_linkscan_fe, pbmp_linkscan_ge);
    pbmp = pbmp_linkscan_fe ;   /* fe + ge */

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbmp,
                    BCM_LINKSCAN_MODE_SW)) < 0) {
        soc_cm_debug(DK_WARN, 
                "Init Failed to set SW link scanning: PBM=%s: %s\n",
                SOC_PBMP_FMT(pbmp, pfmt), bcm_errmsg(rv));
    }
    
     
    /* set all port been forwarded status (specific for Robo family only) */ 
    BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));

    PBMP_ITER(pbmp, port) {
        BCM_IF_ERROR_RETURN(bcm_stg_stp_set(
                                unit, BCM_STG_DEFAULT, 
                                port, BCM_STG_STP_FORWARD));    
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_init_selective
 * Purpose:
 *  Initialize specific bcm modules as desired.
 * Parameters:
 *  unit - RoboSwitch unit #.
 *      flags - Combination of bit selectors (see init.h)
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_init_selective(int unit, uint32 flags)
{
    /*
     * Initialize each bcm module that requires it.
     */

    if (!SOC_UNIT_VALID(unit)) {
    return BCM_E_UNIT;
    }

    BCM_IF_ERROR_RETURN(_bcm_robo_lock_init(unit));

    bcm_chip_family_set(unit, BCM_FAMILY_ROBO); 


    SEL_CALL(bcm_port_init,     PORT,     "PORT",     flags);
    SEL_CALL(bcm_l2_init,       L2,       "L2",       flags);
    SEL_CALL(bcm_stg_init,      STG,      "STG",      flags);
    SEL_CALL(bcm_vlan_init,     VLAN,     "VLAN",     flags);
    SEL_CALL(bcm_trunk_init,    TRUNK,    "TRUNK",    flags);
    SEL_CALL(bcm_cosq_init,     COSQ,     "COSQ",     flags);
    SEL_CALL(bcm_mcast_init,    MCAST,    "MCAST",    flags);
    SEL_CALL(bcm_linkscan_init, LINKSCAN, "LINKSCAN", flags);
    SEL_CALL(bcm_stat_init,     STAT,     "STAT",     flags);
    /* Robo5324 is not suitable for stacking solution currently :
     *
    SEL_CALL(bcm_stk_init,      STACK,    "STACK",    flags);
     */
#ifdef BCM_FIELD_SUPPORT
    if (soc_feature(unit, soc_feature_field)) {
        SEL_CALL(bcm_field_init,    FIELD,    "FIELD",    flags);
    }
#endif     
    SEL_CALL(bcm_ds_init,       DIFFSERV, "DIFFSERV", flags);
    SEL_CALL(bcm_mirror_init,   MIRROR,   "MIRROR",   flags);
    SEL_CALL(bcm_tx_init,       TX,       "TX",       flags);
    SEL_CALL(bcm_rx_init,       RX,       "RX",       flags);
    SEL_CALL(bcm_auth_init,     AUTH,     "AUTH",     flags);

    return BCM_E_NONE;
}   

/*
 * Function:
 *  bcm_robo_init_check
 * Purpose:
 *  Return TRUE if bcm_init_bcm has already been called and succeeded
 * Parameters:
 *  unit- RoboSwitch unit #.
 * Returns:
 *  TRUE or FALSE
 */
int 
bcm_robo_init_check(int unit)   /* Return true BCM layer init done */
{
   return _bcm_robo_initialized_flag[unit];
}   


int 
bcm_robo_attach(int unit, char *type, char *subtype, int remunit)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_detach
 * Purpose:
 *      Clean up bcm layer when unit is detached
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *  BCM_E_XXX
 * Note :
 *  This API currently is dispatchable. But for control.c in bcm layer
 *  exist an API with the same name. We renamed this API as "bcm_robo_detach"
 */
int 
bcm_robo_detach(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_l2_detach(unit));
    BCM_IF_ERROR_RETURN(bcm_linkscan_detach(unit));

    return(BCM_E_NONE);
}   

int 
bcm_robo_find(char *type, char *subtype, int remunit)
{
    return BCM_E_UNAVAIL;
}   

int 
bcm_robo_attach_check(int unit)
{
    return BCM_E_UNAVAIL;
}   

int 
bcm_robo_attach_max(int *max_units)
{
    return BCM_E_UNAVAIL;
}   

/*
 * Function:
 *  bcm_robo_info_get
 * Purpose:
 *  Provide unit information to caller
 * Parameters:
 *  unit    - switch device
 *  info    - (OUT) bcm unit info structure
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_info_get(int unit, bcm_info_t *info)
{
    uint16  dev_id = 0;
    uint8   rev_id = 0;
    
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    
    if (info == NULL) {
        return BCM_E_PARAM;
    }
    
    soc_cm_get_id(unit, &dev_id, &rev_id);
    info->vendor = SOC_PCI_VENDOR(unit);
    info->device = dev_id;
    info->revision = rev_id;
    info->capability = 0;
    /* All ROBO chips are of switch capability now */
    info->capability |= BCM_INFO_SWITCH;
    if (soc_feature(unit, soc_feature_l3)) {
        info->capability |= BCM_INFO_L3;
    }
    if (soc_feature(unit, soc_feature_ip_mcast)) {
        info->capability |= BCM_INFO_IPMC;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_module_name
 * Purpose:
 *  Return the name of a module given its number
 * Parameters:
 *  unit - placeholder
 *  module_num - One of BCM_MODULE_xxx
 * Returns:
 *  Pointer to static char string
 * Notes:
 *  This function to be moved to its own module when it makes sense.
 */
static char *_bcm_robo_module_names[] = BCM_MODULE_NAMES_INITIALIZER;

char *
bcm_robo_module_name(int unit, int module_num)
{
    if (module_num < 0 || module_num >= BCM_MODULE__COUNT) {
    return "???";
    }

    return _bcm_robo_module_names[module_num];
}

/*
 * Function:
 *      bcm_robo_clear
 * Purpose:
 *      Initialize a device without a full reset
 * Parameters:
 *      unit        - The unit number of the device to clear
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      For each module, call the underlieing init/clear operation
 */

int 
bcm_robo_clear(int unit)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    CLEAR_CALL(bcm_port_clear, "port");
    CLEAR_CALL(bcm_l2_clear, "L2");
    CLEAR_CALL(bcm_stg_clear, "STG");
    CLEAR_CALL(bcm_vlan_init, "VLAN");
    CLEAR_CALL(bcm_trunk_init, "trunk");
    CLEAR_CALL(bcm_cosq_init, "COSQ");
    CLEAR_CALL(bcm_mcast_init, "MCast");

    /* Linkscan init is not called; assumed running as configured */
    /* Stats init is not called; assumed running as configured */

#ifdef BCM_FIELD_SUPPORT
    CLEAR_CALL(bcm_field_init, "field");
#endif
    CLEAR_CALL(bcm_mirror_init, "mirror");

    /* TX should not need clearing */

    /* Stacking calls will go away */

    CLEAR_CALL(bcm_auth_init, "AUTH");

    return BCM_E_NONE;
}

    
