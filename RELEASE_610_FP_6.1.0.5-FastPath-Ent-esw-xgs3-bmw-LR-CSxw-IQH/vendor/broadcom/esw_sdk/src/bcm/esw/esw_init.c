/*
 * $Id: esw_init.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 *	PBMP = all switching Ethernet ports (non-fabric) and the CPU.
 *	UBMP = all switching Ethernet ports (non-fabric).
 *   No trunks configured
 *   No mirroring configured
 *   All L2 and L3 tables empty
 *   Ingress VLAN filtering disabled
 *   BPDU reception enabled
 */

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/boot.h>

#include <soc/cmext.h>
#include <soc/l2x.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/phyctrl.h>

#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/rx.h>
#include <bcm/pkt.h>
#include <bcm/ipfix.h>

#include <bcm_int/api_xlate_port.h>
#include <bcm_int/control.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/esw/rcpu.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/filter.h>
#include <bcm_int/esw/stat.h>
#include <bcm_int/esw/mcast.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/ipfix.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/diffserv.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stg.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/rx.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/mcast.h>
#include <bcm_int/esw/port.h>
#endif

#ifdef INCLUDE_MACSEC
#include <bcm_int/common/macsec_cmn.h>
#endif /* INCLUDE_MACSEC */

#include <bcm_int/esw_dispatch.h>

/* See BCM_SEL_INIT flags in init.h */
STATIC uint32 _bcm_initialized_flag[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *	_bcm_lock_init
 * Purpose:
 *	Allocate BCM_LOCK.
 */

STATIC int
_bcm_lock_init(int unit)
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
 *	_bcm_lock_deinit
 * Purpose:
 *	De-allocate BCM_LOCK.
 */

STATIC int
_bcm_lock_deinit(int unit)
{
    if (_bcm_lock[unit] != NULL) {
        sal_mutex_destroy(_bcm_lock[unit]);
        _bcm_lock[unit] = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_init_phy_id_map
 * Purpose:
 * 	Initialize the phy id map for the unit
 * Parameters:
 *	unit - StrataSwitch unit #.
 *      map - an array indexed by (0-based) port giving
 *            MII address
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_esw_init_phy_id_map(int unit, int map[])
{
    bcm_port_t  port;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32      oregv = 0;
    uint32      regv = 0;
    soc_reg_t   reg = CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r;
    soc_reg_t   map_r[14] = {   CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_7_4r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_11_8r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_15_12r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_19_16r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_23_20r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_27_24r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_31_28r
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
                                ,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_35_32r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_39_36r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_43_40r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_47_44r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_51_48r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_55_52r
#endif
                                };
    soc_field_t   map_f[4] = {  PHY_ID_0f, PHY_ID_1f, PHY_ID_2f, PHY_ID_3f};
#endif

    /* Re-initialize the phy port map for the unit */
    PBMP_PORT_ITER(unit, port) {
        /* Update software copy of PHY ID mapping so that phy probing uses
         * the new addresses. */ 
        BCM_IF_ERROR_RETURN
            (soc_phyctrl_init_phy_id_map(unit, map));
 
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (soc_feature(unit, soc_feature_mdio_enhanced)) {
            /* Use MDIO address re-mapping for hardware linkscan */ 
            assert((port / 4) < (bcm_port_t)(sizeof(map_r)/sizeof(map_r[0])));
            reg = map_r[port / 4];
            regv = soc_pci_read(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0));
            oregv = regv;
            soc_reg_field_set(unit, CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r,
                              &regv, map_f[port % 4], (map[port] & 0x1f));
            if (oregv != regv) {
                soc_pci_write(unit,
                              soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                              regv);
            }
        }
#endif
    }
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_mdio_enhanced)) {
        SOC_IF_ERROR_RETURN(READ_CMIC_CONFIGr(unit, &regv));
        oregv = regv;
        soc_reg_field_set(unit, CMIC_CONFIGr, &regv, MIIM_ADDR_MAP_ENABLEf, 1);
        if (oregv != regv) {
            WRITE_CMIC_CONFIGr(unit, reg);
        }
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *	   _bcm_esw_modules_deinit
 * Purpose:
 *	   De-initialize bcm modules
 * Parameters:
 *     unit - (IN) BCM device number.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_esw_modules_deinit(int unit)
{
    int rv;    /* Operation return status. */

    if (soc_feature(unit, soc_feature_oam))
    {
        soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing OAM...\n");
        rv = bcm_esw_oam_detach(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (soc_feature(unit, soc_feature_time_support))
    {
        soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing time...\n");
        rv = bcm_esw_time_deinit(unit);
        BCM_IF_ERROR_RETURN(rv);
    }
#if 0
#ifdef INCLUDE_RCPU
    _bcm_esw_rcpu_deinit,     RCPU,     "RCPU",     flags);
#endif /* INCLUDE_RCPU */
#endif 

#if defined(BCM_FILTER_SUPPORT) || defined(BCM_FIELD_SUPPORT)
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing auth...\n");
    rv = bcm_esw_auth_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif

#ifdef BCM_DMUX_SUPPORT
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing dmux...\n");
    rv = bcm_esw_dmux_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif

#ifdef INCLUDE_L3
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing proxy...\n");
    rv = bcm_esw_proxy_cleanup(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing subport...\n");
    rv = bcm_esw_subport_cleanup(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

#ifdef BCM_MPLS_SUPPORT
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing MPLS...\n");
    rv = bcm_esw_mpls_cleanup(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif /* BCM_MPLS_SUPPORT */

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing IPMC...\n");
    rv = bcm_esw_ipmc_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing L3...\n");
    rv = bcm_esw_l3_cleanup(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif /* INCLUDE_L3 */

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing RX...\n");
    rv = bcm_esw_rx_deinit(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

#if 0
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing TX...\n");
    rv = bcm_esw_tx_deinit(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif 

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing mirror...\n");
    rv = bcm_esw_mirror_deinit(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

#if defined(BCM_FILTER_SUPPORT) || defined(BCM_FIELD_SUPPORT)
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing diffserv...\n");
    rv = _bcm_esw_ds_deinit(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif /* BCM_FIELD_SUPPORT || BCM_FILTER_SUPPORT */

#ifdef BCM_FIELD_SUPPORT
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing FP...\n");
    rv = bcm_esw_field_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif /* BCM_FIELD_SUPPORT */

#ifdef BCM_FILTER_SUPPORT
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing filter...\n");
    rv = _bcm_esw_filter_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif /* BCM_FILTER_SUPPORT*/

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing stacking...\n");
    rv = _bcm_esw_stk_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing stats...\n");
    rv = _bcm_esw_stat_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing linkscan...\n");
    rv = bcm_esw_linkscan_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing mcast...\n");
    rv = _bcm_esw_mcast_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing cosq...\n");
    rv = bcm_esw_cosq_deinit(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing trunk...\n");
    rv = bcm_esw_trunk_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing vlan...\n");
    rv = bcm_esw_vlan_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing STG...\n");
    rv = bcm_esw_stg_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing L2...\n");
    rv = bcm_esw_l2_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing port...\n");
    rv = _bcm_esw_port_deinit(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing ipfix...\n");
    rv = _bcm_esw_ipfix_deinit(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing mbcm...\n");
    rv = mbcm_deinit(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

#ifdef INCLUDE_L3
    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing wLAN...\n");
    rv = bcm_esw_wlan_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing MiM...\n");
    rv = bcm_esw_mim_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif

    soc_cm_debug(DK_VERBOSE, "bcm_detach: Deinitializing QOS...\n");
    rv = bcm_esw_qos_detach(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    soc_cm_debug(DK_VERBOSE, "bcm_detach: All modules deinitialized.\n");
    BCM_UNLOCK(unit);
    _bcm_lock_deinit(unit);
    _bcm_initialized_flag[unit] = 0;

    return rv;
}

/*
 * Function:
 *      _bcm_esw_threads_shutdown
 * Purpose:
 *      Terminate all the spawned threads for specific unit. 
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_esw_threads_shutdown(int unit)
{
    int rv;     /* Operation return status. */

    rv = bcm_esw_linkscan_enable_set(unit, 0);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

#ifdef BCM_XGS_SWITCH_SUPPORT
    rv = soc_l2x_stop(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    rv = soc_counter_detach(unit);

    return (rv);
}


/*
 * Function:
 *	_bcm_esw_init
 * Purpose:
 * 	Initialize the BCM API layer only, without resetting the switch chip.
 * Parameters:
 *	unit - StrataSwitch unit #.
 * Returns:
 *	BCM_E_XXX
 */

STATIC int
_bcm_esw_init(int unit)
{
    uint32 init_flags = BCM_INIT_SEL_ALL;

    BCM_IF_ERROR_RETURN(_bcm_lock_init(unit));

    if (!soc_feature(unit, soc_feature_filter)) {
        init_flags &= ~BCM_INIT_SEL_FILTER;
    } else {
        if (!soc_feature(unit, soc_feature_filter_metering)) {
            init_flags &= ~BCM_INIT_SEL_DIFFSERV;
        }
    }

    if (!soc_feature(unit, soc_feature_field)) {
        init_flags &= ~BCM_INIT_SEL_FIELD;
    }
    if (!(init_flags & (BCM_INIT_SEL_FILTER | BCM_INIT_SEL_FIELD))) {
        /* No fields or filters */
        init_flags &= ~BCM_INIT_SEL_DIFFSERV;
        init_flags &= ~BCM_INIT_SEL_AUTH;
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
    if (!soc_feature(unit, soc_feature_mpls)) {
        init_flags &= ~BCM_INIT_SEL_MPLS;
    }
    if (!soc_feature(unit, soc_feature_mim)) {
        init_flags &= ~BCM_INIT_SEL_MIM;
    }
    if (!soc_feature(unit, soc_feature_subport)) {
        init_flags &= ~BCM_INIT_SEL_SUBPORT;
    }
    if (!soc_feature(unit, soc_feature_wlan)) {
        init_flags &= ~BCM_INIT_SEL_WLAN;
    }
    if (!soc_feature(unit, soc_feature_time_support)) {
        init_flags &= ~BCM_INIT_SEL_TIME;
    }
    if (!soc_feature(unit, soc_feature_oam)) {
        init_flags &= ~BCM_INIT_SEL_OAM;
    }
    if (!soc_feature(unit, soc_feature_qos_profile)) {
        init_flags &= ~BCM_INIT_SEL_QOS;
    }
    if ((SAL_BOOT_SIMULATION) && (!SAL_BOOT_BCMSIM)) {
        if (soc_property_get(unit, spn_SKIP_L2_VLAN_INIT, 0)) {
            init_flags &= ~BCM_INIT_SEL_L2;
            init_flags &= ~BCM_INIT_SEL_VLAN;
            soc_cm_print("SIMULATION: forced to skip init l2 and vlan\n");
        }
	if (init_flags & BCM_INIT_SEL_FILTER) {
	    soc_cm_print("SIMULATION: skipped init filter\n");
	    init_flags &= ~BCM_INIT_SEL_FILTER;
	}
	if (init_flags & BCM_INIT_SEL_FIELD) {
	    soc_cm_print("SIMULATION: skipped init field\n");
	    init_flags &= ~BCM_INIT_SEL_FIELD;
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
	if (init_flags & BCM_INIT_SEL_IPMC) {
	    soc_cm_print("SIMULATION: skipped init ipmc\n");
	    init_flags &= ~BCM_INIT_SEL_IPMC;
	}
	if (init_flags & BCM_INIT_SEL_TRUNK) {
	    soc_cm_print("SIMULATION: skipped init trunk\n");
	    init_flags &= ~BCM_INIT_SEL_TRUNK;
	}
	if (init_flags & BCM_INIT_SEL_MCAST) {
	    soc_cm_print("SIMULATION: skipped init mcast\n");
	    init_flags &= ~BCM_INIT_SEL_MCAST;
	}
	if (init_flags & BCM_INIT_SEL_MPLS) {
	    soc_cm_print("SIMULATION: skipped init mpls\n");
	    init_flags &= ~BCM_INIT_SEL_MPLS;
	}
	if (init_flags & BCM_INIT_SEL_MIM) {
	    soc_cm_print("SIMULATION: skipped init mim\n");
	    init_flags &= ~BCM_INIT_SEL_MIM;
	}
	if (init_flags & BCM_INIT_SEL_SUBPORT) {
	    soc_cm_print("SIMULATION: skipped init subport\n");
	    init_flags &= ~BCM_INIT_SEL_SUBPORT;
	}
	if (init_flags & BCM_INIT_SEL_WLAN) {
	    soc_cm_print("SIMULATION: skipped init wlan\n");
	    init_flags &= ~BCM_INIT_SEL_WLAN;
	}
	if (init_flags & BCM_INIT_SEL_OAM) {
	    soc_cm_print("SIMULATION: skipped init oam\n");
	    init_flags &= ~BCM_INIT_SEL_OAM;
	}
	if (init_flags & BCM_INIT_SEL_QOS) {
	    soc_cm_print("SIMULATION: skipped init qos\n");
	    init_flags &= ~BCM_INIT_SEL_QOS;
	}
    }

    /* If linkscan is running, disable it. */
    bcm_esw_linkscan_enable_set(unit, 0);

#ifdef INCLUDE_MACSEC
    BCM_IF_ERROR_RETURN(_bcm_common_macsec_init(unit)); 
#endif /* INCLUDE_MACSEC */

    BCM_IF_ERROR_RETURN(bcm_esw_init_selective(unit, init_flags));

#ifdef INCLUDE_RCPU
    BCM_IF_ERROR_RETURN(_bcm_esw_rcpu_init(unit)); 
#endif /* INCLUDE_RCPU */

#if defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT)
    BCM_IF_ERROR_RETURN(_bcm_esw_switch_init(unit));
#endif

    return BCM_E_NONE;
}    

/*
 * Function:
 *	bcm_esw_init
 * Purpose:
 * 	Initialize the BCM API layer only, without resetting the switch chip.
 * Parameters:
 *	unit - StrataSwitch unit #.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_init(int unit)
{
    if (0 == SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    return _bcm_esw_init(unit);
}    

/*      bcm_esw_attach
 * Purpose:
 *      Attach and initialize bcm device
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_esw_attach(int unit, char *subtype)
{
    int  dunit;
    int  rv;

    COMPILER_REFERENCE(subtype);

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;
    dunit = BCM_CONTROL(unit)->unit;

    /* Initialize soc layer */
    if ((NULL == SOC_CONTROL(dunit)) || 
        (0 == (SOC_CONTROL(dunit)->soc_flags & SOC_F_ATTACHED))) {
        return (BCM_E_INIT);
    }
    
    if (SAL_THREAD_ERROR == SOC_CONTROL(dunit)->counter_pid) {
        rv = soc_counter_attach(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Initialize bcm layer */
    BCM_CONTROL(unit)->chip_vendor = SOC_PCI_VENDOR(dunit);
    BCM_CONTROL(unit)->chip_device = SOC_PCI_DEVICE(dunit);
    BCM_CONTROL(unit)->chip_revision = SOC_PCI_REVISION(dunit);
    if (SOC_IS_XGS_SWITCH(dunit)) {
        BCM_CONTROL(unit)->capability |= BCM_CAPA_SWITCH;
    }
    if (SOC_IS_XGS_FABRIC(dunit)) {
        BCM_CONTROL(unit)->capability |= BCM_CAPA_FABRIC;
    }
    if (soc_feature(dunit, soc_feature_l3)) {
        BCM_CONTROL(unit)->capability |= BCM_CAPA_L3;
    }
    if (soc_feature(dunit, soc_feature_ip_mcast)) {
        BCM_CONTROL(unit)->capability |=
            BCM_CAPA_IPMC;
    }

    /* Initialize port mappings */
    _bcm_api_xlate_port_init(unit);

    rv = _bcm_esw_init(unit);
    return (rv);
}

/* ASSUMES unit PARAMETER which is not in macro's list. */
#define SEL_CALL(rtn, name, dispname, flags) {                          \
	sal_usecs_t stime = sal_time_usecs();			        \
        if ((BCM_INIT_SEL_ ## name) & (flags)) {                        \
            BCM_IF_ERROR_RETURN(rtn(unit));                             \
            _bcm_initialized_flag[unit] |= (BCM_INIT_SEL_ ## name);     \
	    soc_cm_debug(DK_VERBOSE, "bcm_init: %s took %d usec\n",	\
		dispname, SAL_USECS_SUB(sal_time_usecs(), stime));	\
        }                                                               \
}

/*
 * Function:
 *	bcm_esw_init_selective
 * Purpose:
 * 	Initialize specific bcm modules as desired.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *      flags - Combination of bit selectors (see init.h)
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_init_selective(int unit, uint32 flags)
{
    /*
     * Initialize each bcm module that requires it.
     */

    if (!SOC_UNIT_VALID(unit)) {
	return BCM_E_UNIT;
    }

    /* Must call mbcm init first to ensure driver properly installed */
    BCM_IF_ERROR_RETURN(mbcm_init(unit));

    SEL_CALL(bcm_esw_port_init,     PORT,     "PORT",     flags);
    SEL_CALL(bcm_esw_l2_init,       L2,       "L2",       flags);
    SEL_CALL(bcm_esw_stg_init,      STG,      "STG",      flags);
    SEL_CALL(bcm_esw_vlan_init,     VLAN,     "VLAN",     flags);
    SEL_CALL(bcm_esw_trunk_init,    TRUNK,    "TRUNK",    flags);
    SEL_CALL(bcm_esw_cosq_init,     COSQ,     "COSQ",     flags);
    SEL_CALL(bcm_esw_mcast_init,    MCAST,    "MCAST",    flags);
    SEL_CALL(bcm_esw_linkscan_init, LINKSCAN, "LINKSCAN", flags);
    SEL_CALL(bcm_esw_stat_init,     STAT,     "STAT",     flags);
    SEL_CALL(bcm_esw_stk_init,      STACK,    "STACK",    flags);
#ifdef BCM_FILTER_SUPPORT
    SEL_CALL(bcm_esw_filter_init,   FILTER,   "FILTER",   flags);
#endif
#ifdef BCM_FIELD_SUPPORT
    SEL_CALL(bcm_esw_field_init,    FIELD,    "FIELD",    flags);
#endif
#if defined(BCM_FILTER_SUPPORT) || defined(BCM_FIELD_SUPPORT)
    SEL_CALL(bcm_esw_ds_init,       DIFFSERV, "DIFFSERV", flags);
#endif
    SEL_CALL(bcm_esw_mirror_init,   MIRROR,   "MIRROR",   flags);
    SEL_CALL(bcm_esw_tx_init,       TX,       "TX",       flags);
    SEL_CALL(bcm_esw_rx_init,       RX,       "RX",       flags);

#ifdef INCLUDE_L3
    SEL_CALL(bcm_esw_l3_init,       L3,       "L3",       flags);
    SEL_CALL(bcm_esw_ipmc_init,     IPMC,     "IPMC",     flags);
#ifdef BCM_MPLS_SUPPORT
    SEL_CALL(bcm_esw_mpls_init,     MPLS,     "MPLS",     flags);
#endif
    SEL_CALL(bcm_esw_mim_init,      MIM,      "MIM",      flags);
    SEL_CALL(bcm_esw_subport_init,  SUBPORT,  "SUBPORT",  flags);
    SEL_CALL(bcm_esw_wlan_init,     WLAN,     "WLAN",     flags);
    SEL_CALL(bcm_esw_proxy_init,    L3,       "PROXY",    flags);
    SEL_CALL(bcm_esw_qos_init,      QOS,      "QOS",      flags);
#endif /* INCLUDE_L3 */

#ifdef BCM_DMUX_SUPPORT
    SEL_CALL(bcm_esw_dmux_init,     DMUX,     "DMUX",     flags);
#endif
#if defined(BCM_FILTER_SUPPORT) || defined(BCM_FIELD_SUPPORT)
    SEL_CALL(bcm_esw_auth_init,     AUTH,     "AUTH",     flags);
#endif

    SEL_CALL(bcm_esw_time_init,    TIME,   "TIME",     flags);
    SEL_CALL(bcm_esw_oam_init,      OAM,    "OAM",      flags);

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_init_check
 * Purpose:
 *	Return TRUE if bcm_esw_init_bcm has already been called and succeeded
 * Parameters:
 *	unit- StrataSwitch unit #.
 * Returns:
 *	TRUE or FALSE
 */
int
bcm_esw_init_check(int unit)
{
    return _bcm_initialized_flag[unit];
}


/*
 * Function:
 *      _bcm_esw_detach
 * Purpose:
 *      Clean up bcm layer when unit is detached
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *	BCM_E_XXX
 */
int
_bcm_esw_detach(int unit)
{
    int rv;                    /* Operation return status. */

    /* BCM module initialized check. */
    if (0 == _bcm_initialized_flag[unit]) {
        return (BCM_E_NONE);
    }
    /* Shut down all the spawned threads. */
    rv = _bcm_esw_threads_shutdown(unit);
    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    /* 
     *  Don't move up, holding lock or disabling hw operations 
     *  might prevent theads clean exit.
     */
    BCM_LOCK(unit);

    rv = _bcm_esw_modules_deinit(unit);

    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_shutdown
 * Purpose:
 *      Free up resources without touching hardware
 * Parameters:
 *      unit    - switch device
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_shutdown(int unit)
{
    int warm_boot;
    int rv;

    /* Since this API is used for warm-boot, we need to enable
       warm boot here even if it hasn't been already, and restore
       it afterward if necessary. */

    warm_boot = SOC_WARM_BOOT(unit);

    if (!warm_boot)
    {
        SOC_WARM_BOOT_START(unit);
    }

    rv = bcm_detach(unit);

    if (!warm_boot)
    {
        SOC_WARM_BOOT_DONE(unit);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *	bcm_esw_info_get
 * Purpose:
 *	Provide unit information to caller
 * Parameters:
 *	unit	- switch device
 *	info	- (OUT) bcm unit info structure
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_esw_info_get(int unit, bcm_info_t *info)
{
    uint16 dev_id = 0;
    uint8 rev_id = 0;

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
    if (SOC_IS_XGS_FABRIC(unit)) {
	info->capability |= BCM_INFO_FABRIC;
    } else {
	info->capability |= BCM_INFO_SWITCH;
    }
    if (soc_feature(unit, soc_feature_l3)) {
	info->capability |= BCM_INFO_L3;
    }
    if (soc_feature(unit, soc_feature_ip_mcast)) {
	info->capability |= BCM_INFO_IPMC;
    }
    return BCM_E_NONE;
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
 *      bcm_esw_clear
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
bcm_esw_clear(int unit)
{
/* SDK 5.6.1 bcm_clear calls bcm_esw_init which clears everything. This is
** not the expected behavior of bcm_clear. SDK team agrees that this is a bug
** and will fix it in next SDK release. Meanwhile restoring 
** bcm_clear implementation to SDK 5.6.0
*/
#ifndef LVL7_FIXUP
    return _bcm_esw_init(unit);
#else 
    if (!SOC_UNIT_VALID(unit)) {
	return BCM_E_UNIT;
    }

    CLEAR_CALL(bcm_esw_port_clear, "port");
    CLEAR_CALL(bcm_esw_l2_clear, "L2");
    CLEAR_CALL(bcm_esw_stg_clear, "STG");
    CLEAR_CALL(bcm_esw_vlan_init, "VLAN");
    CLEAR_CALL(bcm_esw_trunk_init, "trunk");
#ifdef LVL7_FIXUP
     /* WLAN init is not getting called, so call it here */
#ifdef INCLUDE_L3
    CLEAR_CALL(bcm_esw_wlan_init, "wlan");
#endif
#endif
#ifndef LVL7_FIXUP
    /* Removed to avoid re-programming of MMU registers during bcm_clear
    ** as stack links may forward traffic while clear is being done.
    */
    CLEAR_CALL(bcm_esw_cosq_init, "COSQ");
#endif
    CLEAR_CALL(bcm_esw_mcast_init, "MCast");

    /* Linkscan init is not called; assumed running as configured */
    /* Stats init is not called; assumed running as configured */

#ifdef BCM_FILTER_SUPPORT
    CLEAR_CALL(bcm_esw_filter_init, "filter");
#endif

#ifdef BCM_FIELD_SUPPORT
    CLEAR_CALL(bcm_esw_field_init, "field");
#endif

#if defined(BCM_FILTER_SUPPORT) || defined(BCM_FIELD_SUPPORT)
    CLEAR_CALL(bcm_esw_ds_init, "diffserv");
#endif

    CLEAR_CALL(bcm_esw_mirror_init, "mirror");

    /* TX should not need clearing */

#ifdef INCLUDE_L3
    CLEAR_CALL(bcm_esw_l3_init, "L3");
#ifdef BCM_MPLS_SUPPORT
    CLEAR_CALL(bcm_esw_mpls_init, "MPLS");
#endif
    CLEAR_CALL(bcm_esw_subport_init, "SUBPORT");
#endif


    /* Stacking calls will go away */

#ifdef INCLUDE_L3
    CLEAR_CALL(bcm_esw_ipmc_init, "IPMC");
#endif

#ifdef BCM_DMUX_SUPPORT
    CLEAR_CALL(bcm_esw_dmux_init, "DMux");
#endif

    

#if defined(BCM_FILTER_SUPPORT) || defined(BCM_FIELD_SUPPORT)
    CLEAR_CALL(bcm_esw_auth_init, "AUTH");
#endif
#ifdef INCLUDE_RCPU
    CLEAR_CALL(_bcm_esw_rcpu_init, "RCPU");
#endif /* INCLUDE_RCPU */

    return BCM_E_NONE;

#endif /* LVL7 FIXUP */
}


