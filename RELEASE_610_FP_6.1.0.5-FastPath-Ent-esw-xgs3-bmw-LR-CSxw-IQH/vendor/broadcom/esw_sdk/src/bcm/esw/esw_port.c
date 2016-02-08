/*
 * $Id: esw_port.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * File:        port.c
 * Purpose:     Tracks and manages ports.
 *              P-VLAN table is managed directly.
 *              MAC/PHY interfaces are managed through respective drivers.
 */

#include <assert.h>

#include <sal/core/libc.h>
#include <sal/core/boot.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/phy.h>
#include <soc/ll.h>
#include <soc/ptable.h>
#include <soc/tucana.h>
#include <soc/firebolt.h>
#include <soc/xaui.h>
#include <soc/phyctrl.h>
#include <soc/phyreg.h>

#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/mirror.h>

#include <bcm_int/api_xlate_port.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/diffserv.h>
#include <bcm_int/esw/link.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/strata.h>
#include <bcm_int/esw/lynx.h>
#include <bcm_int/esw/draco.h>
#include <bcm_int/esw/tucana.h>
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_EASYRIDER_SUPPORT)
#include <bcm_int/esw/easyrider.h>
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
#include <bcm_int/esw/scorpion.h>
#include <bcm_int/esw/triumph.h>
#endif /* BCM_SCORPION_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#include <soc/triumph2.h>
#include <bcm_int/esw/virtual.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT)
#include <bcm_int/esw/enduro.h>
#endif /* BCM_ENDURO_SUPPORT */
#include <bcm_int/esw/xgs3.h>
#if defined(BCM_METER_SUPPORT)
#include <bcm/diffserv.h>
#endif

#include <bcm_int/esw_dispatch.h>

/*
 * Variable:
 *      bcm_port_info
 * Purpose:
 *      One entry for each SOC device containing port information
 *      for that device.  Use the PORT macro to access it.
 */
static _bcm_port_info_t     *bcm_port_info[BCM_MAX_NUM_UNITS];

#define PORT(unit, port)        bcm_port_info[unit][port]

/* Accessor to bcm_port_info used by other modules */
void _bcm_port_info_access(int unit, bcm_port_t port, _bcm_port_info_t **info)
{
    *info = &PORT(unit, port);
    return;
}

/*
 * Define:
 *      PORT_LOCK/PORT_UNLOCK
 * Purpose:
 *      Serialization Macros.
 *      Here the cmic memory lock also serves to protect the
 *      bcm_port_info structure and EPC_PFM registers.
 */

#define PORT_LOCK(unit) \
        if (soc_mem_is_valid(unit, PORT_TABm)) \
        { soc_mem_lock(unit, PORT_TABm); }


#define PORT_UNLOCK(unit) \
        if (soc_mem_is_valid(unit, PORT_TABm)) \
        { soc_mem_unlock(unit, PORT_TABm); }

/*
 * Define:
 *      PORT_INIT
 * Purpose:
 *      Causes a routine to return BCM_E_INIT if port is not yet initialized.
 */

#define PORT_INIT(unit) \
        if (bcm_port_info[unit] == NULL) { return BCM_E_INIT; }

/*
 * Define:
 *      PORT_PARAM_CHECK
 * Purpose:
 *      Check unit and port parameters for most bcm_port api calls
 */
#define PORT_PARAM_CHECK(unit, port) do { \
        PORT_INIT(unit); \
        if (!SOC_PORT_VALID(unit, port)) { return BCM_E_PORT; } \
        } while (0);

/*
 * Define:
 *      PORT_SWITCHED_CHECK
 * Purpose:
 *      Check unit and port for switching feature support
 */
#define PORT_SWITCHED_CHECK(unit, port) do { \
        if (IS_ST_PORT(unit, port)) { \
            return BCM_E_PORT; \
        } else { \
            if (IS_CPU_PORT(unit, port)) { \
                if (!soc_feature(unit, soc_feature_cpuport_switched)) { \
                    return BCM_E_PORT; \
                } \
            } \
        } \
        } while (0);

#ifdef BCM_TRIUMPH2_SUPPORT
typedef int _src_mod_egr_prof_ref_t;
STATIC _src_mod_egr_prof_ref_t src_mod_egr_prof_ref[BCM_MAX_NUM_UNITS][8];
#define SRC_MOD_EGR_REF_COUNT(_u, _r) src_mod_egr_prof_ref[_u][_r]

typedef int _port_src_mod_egr_prof_ptr_t;
STATIC _port_src_mod_egr_prof_ptr_t 
    port_src_mod_egr_prof_ptr[BCM_MAX_NUM_UNITS][SOC_MAX_NUM_PORTS];
#define PORT_SRC_MOD_EGR_PROF_PTR(_u, _p) port_src_mod_egr_prof_ptr[_u][_p]
#endif


/*
 * Function:
 *      _bcm_esw_port_gport_validate
 * Description:
 *      Helper funtion to validate port/gport parameter 
 * Parameters:
 *      unit  - (IN) BCM device number
 *      port_in  - (IN) Port / Gport to validate
 *      port_out - (OUT) Port number if valid. 
 * Return Value:
 *      BCM_E_NONE - Port OK 
 *      BCM_E_INIT - Not initialized
 *      BCM_E_PORT - Port Invalid
 */
int
_bcm_esw_port_gport_validate(int unit, bcm_port_t port_in, bcm_port_t *port_out)
{
    if (bcm_port_info[unit] == NULL) { 
        return BCM_E_INIT; 
    }

    if (BCM_GPORT_IS_SET(port_in)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port_in, port_out));
    } else if (SOC_PORT_VALID(unit, port_in)) { 
        *port_out = port_in;
    } else {
        return BCM_E_PORT; 
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_info_get
 * Description:
 *      Helper funtion  
 * Parameters:
 *      unit  - (IN) BCM device number
 *      port  - (IN) Port number
 *      pinfo - (OUT) Port info. 
 * Return Value:
 *      BCM_E_XXX 
 */
int
_bcm_port_info_get(int unit, bcm_port_t port, _bcm_port_info_t **pinfo)
{
    /* Input parameters check. */
    if (NULL == pinfo) {
        return (BCM_E_PARAM);
    }

    PORT_INIT(unit);

    *pinfo = &PORT(unit, port);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_iport_tab_set
 * Description:
 *      Helper funtion for bcm_esw_port_control_set.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      field - Field name within PORT_TAB table entry
 *      value - new field value
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_XXX - an error occurred accessing PORT_TAB table.
 */
STATIC int
_bcm_esw_iport_tab_set(int unit, bcm_port_t port, 
                       soc_field_t field, int value)

{
    port_tab_entry_t pent;
    soc_mem_t mem;
    int rv, cur_val;

    mem = IPORT_TABLEm;
    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_UNAVAIL);
    }

    PORT_LOCK(unit);

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                      SOC_PORT_MOD_OFFSET(unit, port), &pent);

    if (BCM_SUCCESS(rv)) {
        cur_val = soc_mem_field32_get(unit, mem, &pent, field);
        if (value != cur_val) {
            soc_mem_field32_set(unit, mem, &pent, field, value);
            rv = soc_mem_write(unit, mem, 
                               MEM_BLOCK_ALL, port, &pent);
        }
    }
    PORT_UNLOCK(unit);
    return rv;
}

#if defined(BCM_TRIUMPH2_SUPPORT) ||  defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
/*
 * Function:
 *      _bcm_esw_iport_tab_get
 * Description:
 *      Helper funtion for bcm_esw_port_control_get.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      field - Field name within PORT_TAB table entry
 *      value - pointer to store field value
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_XXX - an error occurred accessing PORT_TAB table.
 */
STATIC int
_bcm_esw_iport_tab_get(int unit, bcm_port_t port, 
                       soc_field_t field, int *value)

{
    port_tab_entry_t pent;
    soc_mem_t mem;
    int rv;

    mem = IPORT_TABLEm;

    if (!SOC_MEM_FIELD_VALID(unit, mem, field))
    {
        return (BCM_E_UNAVAIL);
    }

    PORT_LOCK(unit);

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
        SOC_PORT_MOD_OFFSET(unit, port), &pent);

    if (BCM_SUCCESS(rv))
    {
        *value = soc_mem_field32_get(unit, mem, &pent, field);
    }

    PORT_UNLOCK(unit);

    return rv;
}
#endif

#define _BCM_CPU_TABS_NONE        0
#define _BCM_CPU_TABS_ETHER     0x1
#define _BCM_CPU_TABS_HIGIG     0x2
#define _BCM_CPU_TABS_BOTH      0x3


/*
 * Function:
 *      _bcm_port_autoneg_advert_remote_get
 * Purpose:
 *      Main part of bcm_port_advert_get_remote
 */

STATIC int
_bcm_port_autoneg_advert_remote_get(int unit, bcm_port_t port,
                            bcm_port_ability_t *ability_mask)
{
    int                 an, an_done;

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_auto_negotiate_get(unit,  port,
                                &an, &an_done));

    if (!an) {
        return BCM_E_DISABLED;
    }

    if (!an_done) {
        return BCM_E_BUSY;
    }

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_ability_remote_get(unit, port, ability_mask));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_ability_local_get
 * Purpose:
 *      Main part of bcm_port_ability_local_get
 */

STATIC int
_bcm_port_ability_local_get(int unit, bcm_port_t port,
                           bcm_port_ability_t *ability_mask)
{
    soc_port_ability_t             mac_ability, phy_ability;

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_ability_local_get(unit, port, &phy_ability));

    SOC_IF_ERROR_RETURN
        (MAC_ABILITY_LOCAL_GET(PORT(unit, port).p_mac, unit, 
                              port, &mac_ability));

    /* Combine MAC and PHY abilities */
    ability_mask->speed_half_duplex  = mac_ability.speed_half_duplex & phy_ability.speed_half_duplex;
    ability_mask->speed_full_duplex  = mac_ability.speed_full_duplex & phy_ability.speed_full_duplex;
    ability_mask->pause     = mac_ability.pause & phy_ability.pause;
    if (phy_ability.interface == 0) {
        ability_mask->interface = mac_ability.interface;
    } else {
        ability_mask->interface = phy_ability.interface;
    }
    ability_mask->medium    = phy_ability.medium;
    ability_mask->loopback  = mac_ability.loopback | phy_ability.loopback |
                               BCM_PORT_ABILITY_LB_NONE;
    ability_mask->flags     = mac_ability.flags | phy_ability.flags;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_lport_tab_set
 * Description:
 *      Helper funtion for bcm_esw_port_control_set.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      field - Field name within PORT_TAB table entry
 *      value - new field value
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_XXX - an error occurred accessing PORT_TAB table.
 */
STATIC int
_bcm_esw_lport_tab_set(int unit, bcm_port_t port, 
                      soc_field_t field, int value)

{
    bcm_port_config_t port_config;
    lport_tab_entry_t pent;
    int rv, cur_val;
    int idx;
    soc_mem_t mem = LPORT_TABm;

    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_UNAVAIL);
    }

    if (0 == PBMP_MEMBER(port_config.hg, port)) { 
        return (BCM_E_NONE);
    }

    if (soc_feature(unit, soc_feature_lport_tab_profile)) {
        idx = soc_mem_index_min(unit, LPORT_TABm);
    } else {
        idx = SOC_PORT_MOD_OFFSET(unit, port);
    }

    sal_memset(&pent, 0, sizeof(lport_tab_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, &pent);
    BCM_IF_ERROR_RETURN(rv);

    cur_val = soc_LPORT_TABm_field32_get(unit, &pent, field);
    if (value != cur_val) {
        soc_LPORT_TABm_field32_set(unit, &pent, field, value);
        if (0 == soc_feature(unit, soc_feature_lport_tab_profile)) {
            idx = port;
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, &pent);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_port_tab_set
 * Description:
 *      Helper funtion for bcm_esw_port_control_set.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      cpu_tabs - If CPU port, which ingress tables should be written?
 *      field - Field name within PORT_TAB table entry
 *      value - new field value
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_XXX - an error occurred accessing PORT_TAB table.
 */
STATIC int
_bcm_esw_port_tab_set(int unit, bcm_port_t port, int cpu_tabs,
                      soc_field_t field, int value)

{
    port_tab_entry_t pent;
    soc_mem_t mem;
    int rv, cur_val;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        rv = _bcm_tr2_wlan_port_set(unit, port, field, value);
        return rv;
    }
#endif

    mem = SOC_PORT_MEM_TAB(unit, port);
    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_UNAVAIL);
    }

    PORT_LOCK(unit);

    sal_memset(&pent, 0, sizeof(port_tab_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                      SOC_PORT_MOD_OFFSET(unit, port), &pent);

    if (BCM_SUCCESS(rv)) {
        cur_val = soc_PORT_TABm_field32_get(unit, &pent, field);
        if (value != cur_val) {
            soc_PORT_TABm_field32_set(unit, &pent, field, value);
            if (!IS_CPU_PORT(unit, port) || (cpu_tabs != 0)) {
                rv = soc_mem_write(unit, mem, 
                                   MEM_BLOCK_ALL, port, &pent);
            }
            if (BCM_SUCCESS(rv) && IS_CPU_PORT(unit, port) &&
                SOC_MEM_IS_VALID(unit, IPORT_TABLEm) &&
                (cpu_tabs & _BCM_CPU_TABS_HIGIG)) {
                rv = _bcm_esw_iport_tab_set(unit, port,field, value);
            }
        }
    }
 
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_port_tab_get
 * Description:
 *      Helper funtion for bcm_esw_port_control_get.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      field - Field name within PORT_TAB table entry
 *      value - (OUT) field value
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_XXX - an error occurred accessing PORT_TAB table.
 */
STATIC int
_bcm_esw_port_tab_get(int unit, bcm_port_t port, 
                      soc_field_t field, int *value)

{
    port_tab_entry_t pent;
    soc_mem_t mem;
    int rv;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        rv = _bcm_tr2_lport_field_get(unit, port, field, value);
        return rv;
    }
#endif

    mem = SOC_PORT_MEM_TAB(unit, port);
    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_UNAVAIL);
    }

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                      SOC_PORT_MOD_OFFSET(unit, port), &pent);

    if (BCM_SUCCESS(rv)) {
        *value = soc_PORT_TABm_field32_get(unit, &pent, field);
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_untagged_vlan_get
 * Purpose:
 *      Retrieve the default VLAN ID for the port.
 *      This is the VLAN ID assigned to received untagged packets.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number of port to get info for
 *      vid_ptr - (OUT) Pointer to VLAN ID for return
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table read failed.
 */

int
bcm_esw_port_untagged_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vid_ptr)
{
    bcm_port_cfg_t              pcfg;
    int                         rv;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        return bcm_tr2_wlan_port_untagged_vlan_get(unit, port, vid_ptr);
    }
#endif    
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, port, &port));

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        *vid_ptr = pcfg.pc_vlan;
    } else {
        *vid_ptr = BCM_VLAN_INVALID;
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_untagged_vlan_get: u=%d p=%d vid=%d rv=%d\n",
                     unit, port, *vid_ptr, rv));

    return rv;
}

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
/*
 * Function:
 *      _bcm_port_mmu_update
 * Purpose:
 *      Adjust MMU settings depending on port status.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      link -  True if link is active, false if link is inactive.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

STATIC int
_bcm_port_mmu_update(int unit, bcm_port_t port, int link)
{
    int pause_tx, pause_rx;
    uint32 psl_rval, opc_rval;


#ifdef LVL7_FIXUP
    if (SOC_IS_SCORPION(unit)) return BCM_E_NONE;
#endif

    if (!SOC_IS_HBX(unit)) {
        return (BCM_E_UNAVAIL);
    }

    if (link < 0) {
        SOC_IF_ERROR_RETURN
            (bcm_esw_port_link_status_get(unit, port, &link));
    }

    SOC_IF_ERROR_RETURN
        (MAC_PAUSE_GET(PORT(unit, port).p_mac, unit, port,
                       &pause_tx, &pause_rx));

    SOC_IF_ERROR_RETURN(READ_PORT_SHARED_LIMITr(unit, port, &psl_rval));
    SOC_IF_ERROR_RETURN(READ_OP_PORT_CONFIGr(unit, port, &opc_rval));
    if (link && pause_tx) {
        soc_reg_field_set(unit, PORT_SHARED_LIMITr, &psl_rval, 
                          PORT_SHARED_LIMITf,
                          SOC_IS_SC_CQ(unit) ? 0x2 : 0x3);
        soc_reg_field_set(unit, PORT_SHARED_LIMITr, &psl_rval, 
                          PORT_SHARED_DYNAMICf, 0x1);
        soc_reg_field_set(unit, OP_PORT_CONFIGr, &opc_rval, 
                          PORT_LIMIT_ENABLEf, 1);
    } else {
        /* Turn off */
        soc_reg_field_set(unit, PORT_SHARED_LIMITr, &psl_rval, 
                          PORT_SHARED_LIMITf,
                          SOC_IS_SC_CQ(unit) ? 0x3fff : 0x3000);
        soc_reg_field_set(unit, PORT_SHARED_LIMITr, &psl_rval, 
                          PORT_SHARED_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_PORT_CONFIGr, &opc_rval, 
                          PORT_LIMIT_ENABLEf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMITr(unit, port, psl_rval));
    SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIGr(unit, port, opc_rval));

    return (BCM_E_NONE);
}
#endif /* BCM_BRADLEY_SUPPORT || BCM_SCORPION_SUPPORT */

/*
 * Function:
 *      _bcm_port_untagged_vlan_set
 * Purpose:
 *      Main part of bcm_port_untagged_vlan_set.
 * Notes:
 *      Port does not have to be a member of the VLAN.
 */

STATIC int
_bcm_port_untagged_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vid)
{
    bcm_port_cfg_t      pcfg;
    int                 ut_prio;
    bcm_vlan_t          pdvid;

#ifdef BCM_TRX_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_action)) {
        bcm_vlan_action_set_t action;

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_default_action_get(unit, port, &action));

        #ifdef LVL7_FIXUP
        action.it_inner_prio = bcmVlanActionNone;
        #endif
        action.new_outer_vlan = vid;
        action.priority = PORT(unit, port).p_ut_prio;

        return _bcm_trx_vlan_port_default_action_set(unit, port, &action);
    }
#endif

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));

    pdvid = pcfg.pc_vlan;
    pcfg.pc_vlan = vid;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));

#if defined(BCM_DRACO15_SUPPORT)
    if (SOC_IS_DRACO15(unit)) {
        /*
         * Update default VLAN ID in VLAN_DATA
         */
        int rv = _bcm_d15_port_untagged_vlan_data_update(unit, port,
                                                         pdvid, vid);
        if (rv != BCM_E_NONE) {
            soc_cm_debug(DK_ERR,
                         "failed updating default VLAN ID "
                         "in VLAN_DATA: %s\n", bcm_errmsg(rv));
        }
    }
#endif /* BCM_DRACO15_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        /*
         * Update default VLAN ID in VLAN_PROTOCOL_DATA
         */
        int rv = _bcm_fb_port_untagged_vlan_data_update(unit, port,
                                                        pdvid, vid);
        if (rv != BCM_E_NONE) {
            soc_cm_debug(DK_ERR,
                         "failed updating default VLAN ID "
                         "in VLAN_PROTOCOL_DATA: %s\n", bcm_errmsg(rv));
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    if (!soc_feature(unit, soc_feature_remap_ut_prio)) {
        /* Reset the untagged port priority filter entry, if any */

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_untagged_priority_get(unit, port, &ut_prio));

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_untagged_priority_set(unit, port, ut_prio));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_untagged_vlan_set
 * Purpose:
 *      Set the default VLAN ID for the port.
 *      This is the VLAN ID assigned to received untagged packets.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *      vid -  VLAN ID used for packets that ingress the port untagged
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_NOT_FOUND if vid not in VTABLE
 *      BCM_E_INTERNAL if table read failed.
 *      BCM_E_CONFIG - port does not belong to the VLAN
 * Notes:
 *      BCM_LOCK is used because bcm_vlan_port_get is called internally
 *      which also takes it.  BCM_LOCK must be taken before PORT_LOCK.
 */

int
bcm_esw_port_untagged_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vid)
{
    int rv;

    VLAN_CHK_ID(unit, vid);
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        return bcm_tr2_wlan_port_untagged_vlan_set(unit, port, vid);
    }
#endif
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, port, &port));

    BCM_LOCK(unit);
    PORT_LOCK(unit);
    rv = _bcm_port_untagged_vlan_set(unit, port, vid);
    PORT_UNLOCK(unit);
    BCM_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_untagged_vlan_set: u=%d p=%d vid=%d rv=%d\n",
                     unit, port, vid, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_enable_set
 * Purpose:
 *      Physically enable/disable the MAC/PHY on this port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If linkscan is running, it also controls the MAC enable state.
 */

int
bcm_esw_port_enable_set(int unit, bcm_port_t port, int enable)
{
    int         rv;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    if (enable) {
        if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port)) {
            PORT_UNLOCK(unit);
            return BCM_E_NONE;
        }
        rv = MAC_ENABLE_SET(PORT(unit, port).p_mac, unit, port, TRUE);

        if (SOC_SUCCESS(rv)) {
            rv = soc_phyctrl_enable_set(unit, port, TRUE);
        }
    } else {
        rv = soc_phyctrl_enable_set(unit, port, FALSE);

        if (SOC_SUCCESS(rv)) {
            rv = MAC_ENABLE_SET(PORT(unit, port).p_mac, unit, port, FALSE);
        }
    }

    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_enable_set: u=%d p=%d enable=%d rv=%d\n",
                     unit, port, enable, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_enable_get
 * Purpose:
 *      Gets the enable state as defined by bcm_port_enable_set()
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The PHY enable holds the port enable state set by the user.
 *      The MAC enable transitions up and down automatically via linkscan
 *      even if user port enable is always up.
 */

int
bcm_esw_port_enable_get(int unit, bcm_port_t port, int *enable)
{
    int                 rv;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, port, &port));

    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port)) {
        *enable = 0;
        return BCM_E_NONE;
    }

    rv = soc_phyctrl_enable_get(unit, port, enable);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_enable_get: u=%d p=%d rv=%d enable=%d\n",
                     unit, port, rv, *enable));

    return rv;
}

/*
 * Function:
 *      _bcm_port_mode_setup
 * Purpose:
 *      Set initial operating mode for a port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - Whether to enable or disable
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_port_mode_setup(int unit, bcm_port_t port, int enable)
{
    soc_port_if_t       pif;
    bcm_port_ability_t  local_pa, advert_pa;

    SOC_DEBUG_PRINT((DK_PORT,
        "u=%d p=%d port_mode_setup\n",unit, port));

    SOC_IF_ERROR_RETURN(bcm_esw_port_ability_local_get(unit, port, &local_pa));

    /* If MII supported, enable it, otherwise use TBI */
    if (local_pa.interface & (SOC_PA_INTF_MII | SOC_PA_INTF_GMII |
                              SOC_PA_INTF_SGMII | SOC_PA_INTF_XGMII)) {
        if (IS_GE_PORT(unit, port)) {
            pif = SOC_PORT_IF_GMII;
        } else if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
            pif = SOC_PORT_IF_XGMII;
        } else {
            pif = SOC_PORT_IF_MII;
        }
    } else {
        pif = SOC_PORT_IF_TBI;
    }

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_interface_set(unit, port, pif));
    SOC_IF_ERROR_RETURN
        (MAC_INTERFACE_SET(PORT(unit, port).p_mac, unit, port, pif));

    if (IS_ST_PORT(unit, port)) {
        
        /* Since stacking port doesn't support flow control,
         * make sure that PHY is not advertising flow control capabilities.
         */
        SOC_IF_ERROR_RETURN(
            soc_phyctrl_ability_advert_get(unit, port, &advert_pa));
        advert_pa.pause &= ~(SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM);
        SOC_IF_ERROR_RETURN(
            soc_phyctrl_ability_advert_set(unit, port, &advert_pa));
    }

    if (!SOC_WARM_BOOT(unit) && 
        !(SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port)) ) {
        SOC_IF_ERROR_RETURN
            (MAC_ENABLE_SET(PORT(unit, port).p_mac, unit, port, enable));
    }

    return BCM_E_NONE;
}
#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_bcm_port_vd_pbvl_reinit(int unit)
{

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        bcm_port_t port;
        _bcm_port_info_t *pinfo; 
        int        idxmin, idx, i, index;

        idxmin = soc_mem_index_min(unit, VLAN_PROTOCOL_DATAm);
        idx = soc_mem_index_min(unit, VLAN_PROTOCOLm);
        PBMP_ALL_ITER(unit, port) {
            int start, end;

            start = idxmin + port*16;
            end   = start + 16;
            for (i = start; i < end; i++) {
                 vlan_data_entry_t     vde;
                 vlan_protocol_entry_t vpe;
                 bcm_vlan_t            cvid;
                 uint32                ft;

                 SOC_IF_ERROR_RETURN
                    (READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                                              i, &vde));
                 cvid = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                                                            VLAN_IDf);
                 if (cvid == 0) {
                     continue;
                 }

                 /*
                  * Check against the VLAN_PROTOCOL table to see if a
                  * valid entry exists for matching the packet protocol
                  */
                 index = i - start;
                 SOC_IF_ERROR_RETURN
                    (READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, idx+index, &vpe));
                 ft = 0;
                 if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, ETHERIIf)) {
                     ft |= BCM_PORT_FRAMETYPE_ETHER2;
                 }
                 if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, SNAPf)) {
                     ft |= BCM_PORT_FRAMETYPE_8023;
                 }
                 if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, LLCf)) {
                     ft |= BCM_PORT_FRAMETYPE_LLC;
                 }
                 if (ft == 0) {
                     /*
                      * Something wrong here.. bail out
                      */
                     continue;
                 }
                 BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
                 if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, idx+index)) {
                     /* Set as explicit VID */
                     _BCM_PORT_VD_PBVL_SET(pinfo, idx+index);
                 }
            }
        }
        return BCM_E_NONE;
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        bcm_port_t port;
        int        idxmin, idx, i, index;
        int        idxmax;
        _bcm_port_info_t *pinfo;
        int        vlan_prot_entries, vlan_data_prot_start;

        idxmin = soc_mem_index_min(unit, VLAN_DATAm);
        idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);
        vlan_prot_entries = idxmax + 1;
        vlan_data_prot_start = soc_mem_index_max(unit, VLAN_SUBNETm) + 1;
        idx = soc_mem_index_min(unit, VLAN_PROTOCOLm);

        /*
         * Set VLAN ID for target port. For all other GE ports,
         * set default VLAN ID in entries indexed by the matched entry in
         * VLAN_PROTOCOL.
         */
        PBMP_E_ITER(unit, port) {
            int start, end;

            start = idxmin + (port * vlan_prot_entries);
            end = start + vlan_prot_entries;
            for (i = start; i < end; i++) {
                 vlan_protocol_entry_t   vpe;
                 vlan_data_entry_t       vde;
                 bcm_vlan_t              cvid;
                 uint32                  ft;


                 SOC_IF_ERROR_RETURN
                    (READ_VLAN_DATAm(unit, MEM_BLOCK_ANY, i, &vde));
                  cvid = soc_VLAN_DATAm_field32_get(unit, &vde, VLAN_IDf);
                  if (cvid == 0) {
                      continue;
                  }

                  /*
                   * Check against the VLAN_PROTOCOL table to see if a
                   * valid entry exists for matching the packet protocol
                   */
                  index = i - start;
                  SOC_IF_ERROR_RETURN
                      (READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, idx+index, &vpe));
                  ft = 0;
                  if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, ETHERIIf)) {
                      ft |= BCM_PORT_FRAMETYPE_ETHER2;
                  }
                  if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, SNAPf)) {
                      ft |= BCM_PORT_FRAMETYPE_8023;
                  }
                  if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, LLCf)) {
                      ft |= BCM_PORT_FRAMETYPE_LLC;
                  }
                  if (ft == 0) {
                      /*
                       * Something wrong here.. bail out
                       */
                      continue;
                  }
                  BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));

                  if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, idx+index)) {
                      /* Set as explicit VID */
                      _BCM_PORT_VD_PBVL_SET(pinfo, idx+index);
                  }
            }
        }
        return BCM_E_NONE;
    }
#endif /* BCM_DRACO15_SUPPORT */

    return BCM_E_NONE;
}
#else
#define _bcm_port_vd_pbvl_reinit(u)  (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */


#if defined(BCM_DRACO15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
/*
 * Function:
 *      _bcm_port_vd_pbvl_init
 * Purpose:
 *      Initialization of vd_pbvl bitmap in port
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_MEMORY - failed to allocate required memory.
 */

STATIC int
_bcm_port_vd_pbvl_init(int unit)
{
    bcm_port_t port;
    int idxmax = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    /* Round to the next entry */
    int inds_bytes = (idxmax + (_BCM_PORT_VD_PBVL_ESIZE -  1)) / \
                     _BCM_PORT_VD_PBVL_ESIZE;

    PBMP_ALL_ITER(unit, port) {
        PORT(unit, port).p_vd_pbvl = sal_alloc(inds_bytes, "vdv_info");
        if (NULL == PORT(unit, port).p_vd_pbvl) {
            return (BCM_E_MEMORY);
        }

        if (!SOC_WARM_BOOT(unit)) {
            sal_memset(PORT(unit, port).p_vd_pbvl, 0, inds_bytes);
        }
    }

    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_port_vd_pbvl_reinit(unit));
    }

    return BCM_E_NONE;
}
#endif /* BCM_DRACO15_SUPPORT || BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *      _bcm_esw_port_software_detach
 * Purpose:
 *      De-initialization of software for port subsystem.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write PTABLE entries
 *      BCM_E_MEMORY - failed to allocate required memory.
 */

int
_bcm_esw_port_software_detach(int unit)
{
    bcm_port_t          port;

    if (bcm_port_info[unit] == NULL) {
         return (BCM_E_NONE);
    }

    PBMP_ALL_ITER(unit, port) {
        if (NULL != PORT(unit, port).p_vd_pbvl) {
        sal_free(PORT(unit, port).p_vd_pbvl);
        PORT(unit, port).p_vd_pbvl = NULL;
    }
    }

    SOC_IF_ERROR_RETURN(soc_phy_common_detach(unit));

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_detach(unit));
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT \
          || BCM_RAPTOR_SUPPORT */


    sal_free (bcm_port_info[unit]);
    bcm_port_info[unit] = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_software_init
 * Purpose:
 *      Initialization of software for port subsystem.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write PTABLE entries
 *      BCM_E_MEMORY - failed to allocate required memory.
 */

STATIC int
_bcm_port_software_init(int unit)
{
    bcm_port_t          port;

    if (bcm_port_info[unit] != NULL) {
        for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
#ifdef BCM_FILTER_SUPPORT
            if (PORT(unit, port).p_ut_filter) {
                (void) bcm_esw_filter_destroy(unit, PORT(unit, port).p_ut_filter);
                PORT(unit, port).p_ut_filter = 0;
            }
#endif
#ifdef BCM_METER_SUPPORT
            if (PORT(unit, port).meter_dpid >= 0) {
                assert(PORT(unit, port).meter_cfid != -1);
                (void) bcm_esw_ds_datapath_delete(unit,
                                                  PORT(unit, port).meter_dpid);
            }
#endif
            if (PORT(unit, port).p_vd_pbvl != NULL) {
                sal_free(PORT(unit, port).p_vd_pbvl);
                PORT(unit, port).p_vd_pbvl = NULL;
            }
        }
    }

    if (bcm_port_info[unit] == NULL) {
        bcm_port_info[unit] = sal_alloc(sizeof(_bcm_port_info_t) * SOC_MAX_NUM_PORTS,
                                        "bcm_port_info");
        if (bcm_port_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(bcm_port_info[unit], 0, sizeof(_bcm_port_info_t) * SOC_MAX_NUM_PORTS);

    SOC_IF_ERROR_RETURN(soc_phy_common_init(unit));

#ifdef BCM_METER_SUPPORT
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        PORT(unit, port).meter_dpid = -1;
        PORT(unit, port).meter_cfid = -1;
    }
#endif

#if defined(BCM_DRACO15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_DRACO15(unit) || SOC_IS_FBX(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_port_vd_pbvl_init(unit));
    }
#endif /* BCM_DRACO15_SUPPORT || BCM_FIREBOLT_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_init(unit));
    }
    if (soc_feature(unit, soc_feature_color_prio_map)) {
        BCM_IF_ERROR_RETURN(_bcm_fb2_priority_map_init(unit));
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT \
          || BCM_RAPTOR_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
            PORT(unit, port).dtag_mode = BCM_PORT_DTAG_MODE_NONE;
        }
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_settings_init
 * Purpose:
 *      Initialize port settings if they are to be different from the
 *      default ones
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - port number
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write PTABLE entries
 * Notes:
 *      This function initializes port settings based on the folowing config
 *      variables:
 *           port_init_speed
 *           port_init_duplex
 *           port_init_adv
 *           port_init_autoneg
 *      If a variable is not set, then no additional initialization of the
 *      corresponding parameter is done (and the defaults will normally be
 *      advertize everything you can do and use autonegotiation).
 *
 *      A typical use would be to set:
 *          port_init_adv=0
 *          port_init_autoneg=1
 *      to force link down in the beginning.
 *
 *      Another setup that makes sense is something like:
 *          port_init_speed=10
 *          port_init_duplex=0
 *          port_init_autoneg=0
 *      in order to force link into a certain mode. (It is very important to
 *      disable autonegotiation in this case).
 *
 *      PLEASE NOTE:
 *          The standard rc.soc forces autoneg=on on all the ethernet ports
 *          (FE and GE). Thus, to use the second example one has to edit rc.soc
 *          as well.
 *
 *     This function has been declared as global, but not exported. This will
 *     make port initialization easier when using VxWorks shell.
 */
int
bcm_port_settings_init(int unit, bcm_port_t port)
{
    int             val;
    bcm_port_info_t info;

    bcm_port_info_t_init(&info);
    
    val = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, -1);
    if (val != -1) {
        info.speed = val;
        info.action_mask |= BCM_PORT_ATTR_SPEED_MASK;
    }

    val = soc_property_port_get(unit, port, spn_PORT_INIT_DUPLEX, -1);
    if (val != -1) {
        info.duplex = val;
        info.action_mask |= BCM_PORT_ATTR_DUPLEX_MASK;
    }

    val = soc_property_port_get(unit, port, spn_PORT_INIT_ADV, -1);
    if (val != -1) {
        info.local_advert = val;
        info.action_mask |= BCM_PORT_ATTR_LOCAL_ADVERT_MASK;
    }

    val = soc_property_port_get(unit, port, spn_PORT_INIT_AUTONEG, -1);
    if (val != -1) {
        info.autoneg = val;
        info.action_mask |= BCM_PORT_ATTR_AUTONEG_MASK;
    }

    return bcm_esw_port_selective_set(unit, port, &info);
}

#ifdef BCM_TRIUMPH2_SUPPORT
/* Helper function to program logical to physical (and reverse) port mapping */
STATIC int
_bcm_port_remap_set(int unit, bcm_port_t phys, bcm_port_t logical)
{
    port_tab_entry_t ptab;
    sys_portmap_entry_t sys_portmap;

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                     MEM_BLOCK_ANY, phys, &ptab));
    soc_PORT_TABm_field32_set(unit, &ptab, SRC_SYS_PORT_IDf, logical);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm,
                                      MEM_BLOCK_ALL, phys, &ptab));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, SYS_PORTMAPm,
                                     MEM_BLOCK_ANY, logical, &sys_portmap));
    soc_SYS_PORTMAPm_field32_set(unit, &sys_portmap, PHYS_PORT_IDf, phys);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, SYS_PORTMAPm,
                                      MEM_BLOCK_ALL, logical, &sys_portmap));

   return BCM_E_NONE; 
}

STATIC int
_bcm_tr2_system_tpid_init(int unit)
{
    int tpid_index, sys_index, rv = BCM_E_NONE;
    bcm_module_t module;
    bcm_port_t port;
    system_config_table_entry_t systab; 
    uint32 tpid_enable;
    uint16 tpid;

    tpid = _bcm_fb2_outer_tpid_default_get(unit);
    BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_lkup(unit, tpid, &tpid_index));

    for (module = 0; module < 128; module++) {
        for (port = 0; port < 64; port++) {
            sys_index = module * 64 + port; 
            rv = READ_SYSTEM_CONFIG_TABLEm(unit, MEM_BLOCK_ANY, 
                                           sys_index, &systab);
            BCM_IF_ERROR_RETURN(rv);
            tpid_enable = (1 << tpid_index);
            soc_SYSTEM_CONFIG_TABLEm_field32_set(unit, &systab, 
                                                 OUTER_TPID_ENABLEf, 
                                                 tpid_enable);
            rv = WRITE_SYSTEM_CONFIG_TABLEm(unit, MEM_BLOCK_ALL, 
                                            sys_index, &systab);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return rv;
}
#endif

/*
 * Function:
 *      _bcm_esw_port_deinit
 * Purpose:
 *      De-initialize the PORT interface layer for the specified SOC device.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_port_deinit(int unit)
{
    int  rv;

    if (NULL == bcm_port_info[unit]) {
        return (BCM_E_NONE);
    }

    rv = _bcm_esw_port_software_detach(unit);
    BCM_IF_ERROR_RETURN(rv);

    if(SOC_IS_RCPU_ONLY(unit)) {
        /* No other port de-initialization necessary */
        return (BCM_E_NONE); 
    }   


#ifdef BCM_TRX_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_action)) {
#ifdef BCM_TRIUMPH2_SUPPORT
        if ((SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
             SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit))) {
            _bcm_tr2_port_vpd_bitmap_free(unit);
        }
#endif /* BCM_TRIUMPH2_SUPPORT */

        /* Initialize the vlan action profile table */
        rv = _bcm_trx_vlan_action_profile_detach(unit);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        rv = _bcm_esw_flex_stat_detach(unit, _bcmFlexStatTypeGport);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_init
 * Purpose:
 *      Initialize the PORT interface layer for the specified SOC device.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write PTABLE entries
 *      BCM_E_MEMORY - failed to allocate required memory.
 * Notes:
 *      By default ports come up enabled. They can be made to come up disabled
 *      at startup by a compile-time application policy flag in your Make.local
 *      PTABLE initialized.
 */

int
bcm_esw_port_init(int unit)
{
    int                 rv, port_enable;
    bcm_port_t          p;
    pbmp_t              okay_ports;
    bcm_vlan_data_t     vd;
    char                pfmtok[SOC_PBMP_FMT_LEN],
                        pfmtall[SOC_PBMP_FMT_LEN];

    soc_cm_debug(DK_VERBOSE, "bcm_port_init: unit %d\n", unit);
    
    assert(unit < BCM_MAX_NUM_UNITS);

    if ((rv = _bcm_port_software_init(unit)) != BCM_E_NONE) {
        soc_cm_debug(DK_ERR,
                     "Error unit %d:  Failed software port init: %s\n",
                     unit, bcm_errmsg(rv));
        return rv;
    }

    if(SOC_IS_RCPU_ONLY(unit)) {
        /* No other port initialization necessary */
        return 0; 
    }   


#ifdef BCM_TRX_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_action)) {
        bcm_vlan_action_set_t action;

#ifdef BCM_TRIUMPH2_SUPPORT
        if ((SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
             SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit))) {
            BCM_IF_ERROR_RETURN(_bcm_tr2_port_vpd_bitmap_alloc(unit)); 
        }
#endif

        PBMP_ALL_ITER(unit, p) {
            if (!IS_LB_PORT(unit, p)) {
#ifdef BCM_TRIUMPH2_SUPPORT
                if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                    SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_port_vlan_prot_index_alloc
                            (unit, &(PORT(unit, p).vlan_prot_ptr)));
                } else
#endif
                {
                    PORT(unit, p).vlan_prot_ptr = p * soc_mem_index_count
                                                  (unit, VLAN_PROTOCOLm);
                }
                PORT(unit, p).vp_count = 0;
            }
        }

        /* Initialize the vlan action profile table */
        BCM_IF_ERROR_RETURN (_bcm_trx_vlan_action_profile_init(unit));

        PBMP_HG_ITER(unit, p) {
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_port_egress_default_action_get(unit,
                                                             p, &action));
            /* Backward compatible defaults */
            action.ot_outer = bcmVlanActionDelete;
            action.dt_outer = bcmVlanActionDelete;
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_port_egress_default_action_set(unit,
                                                             p, &action));
        }
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_flex_stat_init(unit, _bcmFlexStatTypeGport));
        if (SOC_WARM_BOOT(unit)) {
            port_tab_entry_t ptab;
            int fs_idx;
            
            PBMP_ALL_ITER(unit, p) {
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                                 MEM_BLOCK_ANY, p, &ptab));
                fs_idx =
                    soc_mem_field32_get(unit, PORT_TABm, &ptab,
                                        VINTF_CTR_IDXf);
                if (fs_idx) {
                    _bcm_esw_flex_stat_reinit_add(unit,
                             _bcmFlexStatTypeGport, fs_idx, p);
                }
            }
        }
    }
#endif

    /*
     * Write port configuration tables to contain the Initial System
     * Configuration (see init.c).
     */

    vd.vlan_tag = BCM_VLAN_DEFAULT;
    BCM_PBMP_ASSIGN(vd.port_bitmap, PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(vd.ut_port_bitmap, PBMP_ALL(unit));
    BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));

    PBMP_ALL_ITER(unit, p) {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_init(unit, p, &vd));
    }
    
    if (!SOC_WARM_BOOT(unit)) {
        /*
         * Clear egress port blocking table
         */
        if (SOC_IS_EASYRIDER(unit)) {
#ifdef BCM_EASYRIDER_SUPPORT
            int i;

            for (i = 0; i < SOC_REG_NUMELS(unit, MAC_BLOCK_TABLEr); i++) {
                SOC_IF_ERROR_RETURN(WRITE_MAC_BLOCK_TABLEr(unit, i, 0));
            }
#endif /* BCM_EASYRIDER_SUPPORT */
        } else if (SOC_IS_XGS_SWITCH(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MAC_BLOCKm, COPYNO_ALL, TRUE));
        }

#if defined(BCM_TUCANA_SUPPORT)
        /* Initialize DSCP-based priority mapping... */
        if (SOC_IS_TUCANA(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, DSCP_PRIORITY_TABLEm, COPYNO_ALL, TRUE));
        }
#endif /* BCM_TUCANA_SUPPORT */
    }

    /* Probe for ports */
    SOC_PBMP_CLEAR(okay_ports);
    if ((rv = bcm_esw_port_probe(unit, PBMP_PORT_ALL(unit), &okay_ports)) !=
        BCM_E_NONE) {
        soc_cm_debug(DK_ERR,
                     "Error unit %d:  Failed port probe: %s\n",
                     unit, bcm_errmsg(rv));
        return rv;
    }
    
    soc_cm_debug(DK_VERBOSE, "Probed ports okay: %s of %s\n",
                 SOC_PBMP_FMT(okay_ports, pfmtok),
                 SOC_PBMP_FMT(PBMP_PORT_ALL(unit), pfmtall));

    /* Probe and initialize MAC and PHY drivers for ports that were OK */
    PBMP_ITER(okay_ports, p) {
        soc_cm_debug(DK_PORT | DK_VERBOSE, "bcm_port_init: unit %d port %s\n",
                     unit, SOC_PORT_NAME(unit, p));

        if ((rv = _bcm_port_mode_setup(unit, p, TRUE)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to set initial mode: %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        }

        if (SOC_WARM_BOOT(unit)) {
            continue;
        }

        if ((rv = bcm_port_settings_init(unit, p)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to configure initial settings: %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        }

        /*
         * A compile-time application policy may prefer to disable ports 
         * when switch boots up
         */

#ifdef BCM_PORT_DEFAULT_DISABLE
            port_enable = FALSE;
#else
            port_enable = TRUE;
#endif  /* BCM_PORT_DEFAULT_DISABLE */
        if ((rv = bcm_esw_port_enable_set(unit, p, port_enable)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to %s port: %s\n",
                         unit, SOC_PORT_NAME(unit, p),(port_enable) ? "enable" : "disable" ,bcm_errmsg(rv));
        }

        /*
         *  JAM should be enabled by default
         */
        if (IS_E_PORT(unit, p)) {     
            if (soc_reg_field_valid(unit, CONFIGr, JAM_ENf) || 
                soc_mem_field_valid(unit, PORT_TABm, JAM_ENf) ||
                soc_reg_field_valid(unit, GE_PORT_CONFIGr, JAM_ENf) ||
                soc_feature(unit, soc_feature_unimac)) {

                rv = bcm_esw_port_jam_set(unit, p, 1);
                if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
                    soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                                 "Failed to enable JAM %s\n", unit, 
                                 SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                }
            }
        }
#ifdef INCLUDE_RCPU
        if ((uint32)p == soc_property_get(unit, spn_RCPU_PORT, -1)) {
            bcm_esw_port_frame_max_set(unit, p, BCM_PORT_JUMBO_MAXSZ);
        }
#endif /* INCLUDE_RCPU */
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    if (!SOC_WARM_BOOT(unit)) {
        /* Enable the default outer TPID. */
        if (soc_feature(unit, soc_feature_vlan_ctrl)) {
             PBMP_ALL_ITER(unit, p) {
                 rv = bcm_esw_port_tpid_set(unit, p,
                                    _bcm_fb2_outer_tpid_default_get(unit));
                 if (BCM_FAILURE(rv)) {
                     soc_cm_debug(DK_WARN, "Warning: Unit %d : "
                              "Failed to set %s port default TPID: %s\n",
                              unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                 }
             }
         }
    }
#endif

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        PBMP_ALL_ITER(unit, p) {
            bcm_esw_port_learn_set(unit, p, BCM_PORT_LEARN_FWD);
        }
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        port_tab_entry_t ptab;

        PBMP_ALL_ITER(unit, p) {
            /* Initialize the logical to physical port mapping */
            if (!SOC_IS_ENDURO(unit)) {
                bcm_port_t sp = p;
                if (soc_feature(unit, soc_feature_sysport_remap)) {
                    BCM_XLATE_SYSPORT_P2S(unit, &sp);
                }
                _bcm_port_remap_set(unit, p, sp);
            }

            /* Initialize egress block profile pointer to invalid value */
            PORT_SRC_MOD_EGR_PROF_PTR(unit, p) = -1;

            /* Program the VLAN_PROTOCOL_DATA / FP_PORT_FIELD_SEL_INDEX pointers */
            if (!IS_LB_PORT(unit, p)) {
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                    MEM_BLOCK_ANY, p, &ptab));
                soc_PORT_TABm_field32_set(unit, &ptab, 
                    VLAN_PROTOCOL_DATA_INDEXf, PORT(unit, p).vlan_prot_ptr 
                        / soc_mem_index_count(unit, VLAN_PROTOCOLm));

                soc_PORT_TABm_field32_set(unit, &ptab, 
                                          FP_PORT_FIELD_SEL_INDEXf, p);

                BCM_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm,
                                    MEM_BLOCK_ALL, p, &ptab));

                if (soc_feature(unit, soc_feature_embedded_higig) 
                    && IS_E_PORT(unit, p)) {
                    uint32 buffer[_BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ];

                    sal_memset(buffer, 0, 
                           WORDS2BYTES(_BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ));
                    BCM_IF_ERROR_RETURN(
                        _bcm_port_ehg_header_write(unit, p, buffer, buffer,
                                        _BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ));
                }
            }
        }
        BCM_IF_ERROR_RETURN(_bcm_tr2_system_tpid_init(unit));
    }
#endif

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_port_clear
 * Purpose:
 *      Initialize the PORT interface layer for the specified SOC device
 *      without resetting stacking ports.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write PTABLE entries
 *      BCM_E_MEMORY - failed to allocate required memory.
 * Notes:
 *      By default ports come up enabled. They can be made to come up disabled
 *      at startup by a compile-time application policy flag in your Make.local
 *      A call to bcm_port_clear should exhibit similar behavior for 
 *      non-stacking ethernet ports
 *      PTABLE initialized.
 */

int
bcm_esw_port_clear(int unit)
{
    bcm_port_config_t port_config;
    bcm_pbmp_t reset_ports;
    bcm_port_t port;
    int rv, port_enable;

    PORT_INIT(unit);

    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

    /* Clear all non-stacking ethernet ports */
    BCM_PBMP_ASSIGN(reset_ports, port_config.e);
    BCM_PBMP_REMOVE(reset_ports, SOC_PBMP_STACK_CURRENT(unit));

    PBMP_ITER(reset_ports, port) {
        soc_cm_debug(DK_PORT | DK_VERBOSE,
                     "bcm_port_clear: unit %d port %s\n",
                     unit, SOC_PORT_NAME(unit, port));

        if ((rv = _bcm_port_mode_setup(unit, port, TRUE)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to set initial mode: %s\n",
                         unit, SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
        }



        /*
         * A compile-time application policy may prefer to disable 
         * ports at startup. The same behavior should be observed 
         * when bcm_port_clear gets called.
         */

#ifdef BCM_PORT_DEFAULT_DISABLE
            port_enable = FALSE;
#else
            port_enable = TRUE;
#endif  /* BCM_PORT_DEFAULT_DISABLE */


        if ((rv = bcm_esw_port_enable_set(unit, port, port_enable)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to %s port: %s\n",
                         unit, SOC_PORT_NAME(unit, port),(port_enable) ? "enable" : "disable" ,bcm_errmsg(rv));
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_probe
 * Purpose:
 *      Probe the phy and set up the phy and mac of the indicated port
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - Port to probe
 *      okay - Output parameter indicates port can be enabled.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL - internal error.
 * Notes:
 *      If error is returned, the port should not be enabled.
 */

int
_bcm_port_probe(int unit, bcm_port_t p, int *okay)
{
    int                 rv;
    mac_driver_t        *macd;

    *okay = FALSE;

    soc_cm_debug(DK_PORT | DK_VERBOSE, "Init port %d PHY...\n", p);
    
    if ((rv = soc_phyctrl_probe(unit, p)) < 0) {
        soc_cm_debug(DK_WARN,
                     "Unit %d Port %s: Failed to probe PHY: %s\n",
                     unit, SOC_PORT_NAME(unit, p), soc_errmsg(rv));
        return rv;
    }
    
    if ((rv = soc_phyctrl_init(unit, p)) < 0) {
        soc_cm_debug(DK_WARN,
                     "Unit %d Port %s: Failed to initialize PHY: %s\n",
                     unit, SOC_PORT_NAME(unit, p), soc_errmsg(rv));
        return rv;
    }

    /* Probe function should leave port disabled */
    if ((rv = soc_phyctrl_enable_set(unit, p, 0)) < 0) {
        return rv;
    }

    /*
     * Currently initializing MAC after PHY is required because of
     * phy_5690_notify_init().
     */

    soc_cm_debug(DK_PORT | DK_VERBOSE, "Init port %d MAC...\n", p);

    if ((rv = soc_mac_probe(unit, p, &macd)) < 0) {
        soc_cm_debug(DK_WARN,
                     "Unit %d Port %s: Failed to probe MAC: %s\n",
                     unit, SOC_PORT_NAME(unit, p), soc_errmsg(rv));
        return rv;
    }
    
    PORT(unit, p).p_mac = macd;

    if (!SOC_WARM_BOOT(unit) &&
        (rv = MAC_INIT(PORT(unit, p).p_mac, unit, p)) < 0) {
        soc_cm_debug(DK_WARN,
                     "Unit %d Port %s: Failed to initialize MAC: %s\n",
                     unit, SOC_PORT_NAME(unit, p), soc_errmsg(rv));
        return rv;
    }

    /* Probe function should leave port disabled */
    if ((rv = MAC_ENABLE_SET(PORT(unit, p).p_mac, unit, p, 0)) < 0) {
        return rv;
    }

    *okay = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_probe
 * Purpose:
 *      Probe the PHY and set up the PHY and MAC for the specified ports.
 *      This is purely a discovery routine and does no configuration.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbmp - Bitmap of ports to probe.
 *      okay_pbmp (OUT) - Ports which were successfully probed.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL - internal error.
 * Notes:
 *      If error is returned, the port should not be enabled.
 *      Assumes port_init done.
 *      Note that if a PHY is not present, the port will still probe
 *      successfully.  The default driver will be installed.
 */

int
bcm_esw_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp)
{
    int rv = BCM_E_NONE;
    bcm_port_t port;
    int okay;

    SOC_PBMP_CLEAR(*okay_pbmp);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    PBMP_ITER(pbmp, port) {
        rv = _bcm_port_probe(unit, port, &okay);
        if (okay) {
            SOC_PBMP_PORT_ADD(*okay_pbmp, port);
        }
        if (rv < 0) {
            soc_cm_debug(DK_WARN,
                         "Port probe failed on port %s\n",
                         SOC_PORT_NAME(unit, port));
            break;
        }
    }

    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_port_detach
 * Purpose:
 *      Main part of bcm_port_detach
 */

int
_bcm_port_detach(int unit, pbmp_t pbmp, pbmp_t *detached)
{
    bcm_port_t          port;

    SOC_PBMP_CLEAR(*detached);

    PBMP_ITER(pbmp, port) {
        SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port));
        BCM_IF_ERROR_RETURN(bcm_esw_port_stp_set(unit, port, BCM_STG_STP_DISABLE));
        BCM_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, port, FALSE));
        SOC_PBMP_PORT_ADD(*detached, port);
    }
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        _bcm_tr2_port_vpd_bitmap_free(unit);
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_detach
 * Purpose:
 *      Detach a port.  Set phy driver to no connection.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbmp - Bitmap of ports to detach.
 *      detached (OUT) - Bitmap of ports successfully detached.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL - internal error.
 * Notes:
 *      If a port to be detached does not appear in detached, its
 *      state is not defined.
 */

int
bcm_esw_port_detach(int unit, pbmp_t pbmp, pbmp_t *detached)
{
    int         rv;
#ifdef  BROADCOM_DEBUG
    char        pfmtp[SOC_PBMP_FMT_LEN],
                pfmtd[SOC_PBMP_FMT_LEN];
#endif  /* BROADCOM_DEBUG */

    PORT_INIT(unit);

    PORT_LOCK(unit);
    rv = _bcm_port_detach(unit, pbmp, detached);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT, "bcm_port_detach: u=%d pbmp=%s det=%s rv=%d\n",
                     unit,
                     SOC_PBMP_FMT(pbmp, pfmtp),
                     SOC_PBMP_FMT(*detached, pfmtd),
                     rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_config_get
 * Purpose:
 *      Get port configuration of a device
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      config - (OUT) Structure returning configuration
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_config_get(int unit, bcm_port_config_t *config)
{
    PORT_INIT(unit);

    config->fe          = PBMP_FE_ALL(unit);
    config->ge          = PBMP_GE_ALL(unit);
    config->xe          = PBMP_XE_ALL(unit);
    config->e           = PBMP_E_ALL(unit);
    config->hg          = PBMP_HG_ALL(unit);
    config->port        = PBMP_PORT_ALL(unit);
    config->cpu         = PBMP_CMIC(unit);
    config->all         = PBMP_ALL(unit);
    /* Remove LB port from the PBMP_ALL bitmap for backward compatibility */
    SOC_PBMP_REMOVE(config->all, PBMP_LB(unit));
    config->stack_ext   = PBMP_ST_ALL(unit);

    BCM_PBMP_CLEAR(config->stack_int);
    BCM_PBMP_CLEAR(config->sci);
    BCM_PBMP_CLEAR(config->sfi);
    BCM_PBMP_CLEAR(config->spi);
    BCM_PBMP_CLEAR(config->spi_subport);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_update
 * Purpose:
 *      Get port characteristics from PHY and program MAC to match.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      link -  True if link is active, false if link is inactive.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

STATIC int
_bcm_port_update(int unit, bcm_port_t port, int link)
{
    int                 rv;
    int                 duplex, speed, an, an_done;
    soc_port_if_t       pif;

    if (!link) {
        /* PHY is down.  Disable the MAC. */

        rv = (MAC_ENABLE_SET(PORT(unit, port).p_mac, unit, port, FALSE));
        if (BCM_FAILURE(rv)) {
            SOC_DEBUG_PRINT((DK_WARN,
                    "u=%d p=%d MAC_ENABLE_SET FALSE rv=%d\n",
                    unit, port, rv));
            return rv;
        }

        /* PHY link down event */
        rv = (soc_phyctrl_linkdn_evt(unit, port));
        if (BCM_FAILURE(rv) && (BCM_E_UNAVAIL != rv)) {
            SOC_DEBUG_PRINT((DK_WARN,
                    "u=%d p=%d soc_phyctrl_linkdn_evt rv=%d\n",unit, port, rv));
            return rv;
        }

        return BCM_E_NONE;
    }

    /* PHY link up event may not be support by all PHY driver. 
     * Just ignore it if not supported */
    rv = (soc_phyctrl_linkup_evt(unit, port));
    if (BCM_FAILURE(rv) && (BCM_E_UNAVAIL != rv)) {
        SOC_DEBUG_PRINT((DK_WARN,
                "u=%d p=%d soc_phyctrl_linkup_evt rv=%d\n",unit, port, rv));
        return rv;
    }

    /*
     * Set MAC speed first, since for GTH ports, this will switch
     * between the 1000Mb/s or 10/100Mb/s MACs.
     */

    if (!IS_HG_PORT(unit, port) || IS_GX_PORT(unit, port)) {
        rv = (soc_phyctrl_speed_get(unit, port, &speed));
        if (BCM_FAILURE(rv) && (BCM_E_UNAVAIL != rv)) {
            SOC_DEBUG_PRINT((DK_WARN,
                "u=%d p=%d phyctrl_speed_get rv=%d\n",unit, port, rv));
            return rv;
        }
        if (IS_HG_PORT(unit, port) && speed < 10000) {
            speed = 0;
        }
        SOC_DEBUG_PRINT((DK_PORT,
            "u=%d p=%d phyctrl_speed_get speed=%d\n",unit, port, speed));

        if (BCM_E_UNAVAIL == rv ) {
            /* If PHY driver doesn't support speed_get, don't change 
             * MAC speed. E.g, Null PHY driver 
             */
            rv = BCM_E_NONE;
        } else {
            rv =  (MAC_SPEED_SET(PORT(unit, port).p_mac, unit, port, speed));
        }
        if (BCM_FAILURE(rv)) {
            SOC_DEBUG_PRINT((DK_WARN, 
                            "u=%d p=%d MAC_SPEED_SET speed=%d rv=%d\n",
                            unit, port, speed, rv));
            return rv;
        }

        rv =   (soc_phyctrl_duplex_get(unit, port, &duplex));
        if (BCM_FAILURE(rv)) {
            SOC_DEBUG_PRINT((DK_WARN, "u=%d p=%d phyctrl_duplex_get rv=%d\n",
                                unit, port, rv));
            return rv;
        }

        rv = (MAC_DUPLEX_SET(PORT(unit, port).p_mac, unit, port, duplex));
        if (BCM_FAILURE(rv)) {
            SOC_DEBUG_PRINT((DK_WARN, "u=%d p=%d MAC_DUPLEX_SET %s sp=%d rv=%d\n", 
                             unit, port, 
                             duplex ? "FULL" : "HALF", speed, rv));
            return rv;
        }
    } else {

        duplex = 1;
    }

    rv = (soc_phyctrl_interface_get(unit, port, &pif));
    if (BCM_FAILURE(rv)) {
        SOC_DEBUG_PRINT((DK_WARN,
                        "u=%d p=%d phyctrl_interface_get rv=%d\n",
                        unit, port, rv));
        return rv;
    }
    rv = (MAC_INTERFACE_SET(PORT(unit, port).p_mac, unit, port, pif));
    if (BCM_FAILURE(rv)) {
        SOC_DEBUG_PRINT((DK_WARN,
                        "u=%d p=%d MAC_INTERFACE_GET rv=%d\n",
                        unit, port,rv));
        return rv;
    }

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_auto_negotiate_get(unit, port, &an, &an_done));

    /*
     * If autonegotiating, check the negotiated PAUSE values, and program
     * MACs accordingly.
     */

    if (an) {
        bcm_port_ability_t      remote_advert, local_advert;
        int                     tx_pause, rx_pause;

        rv = soc_phyctrl_ability_advert_get(unit, port, &local_advert); 
        if (BCM_FAILURE(rv)) {
            SOC_DEBUG_PRINT((DK_WARN,
                            "u=%d p=%d soc_phyctrl_adv_local_get rv=%d\n",
                             unit, port, rv));
            return rv;
        }
        rv = soc_phyctrl_ability_remote_get(unit, port, &remote_advert); 
        if (BCM_FAILURE(rv)) {
            SOC_DEBUG_PRINT((DK_WARN,
                            "u=%d p=%d soc_phyctrl_adv_remote_get rv=%d\n",
                             unit, port, rv));
            return rv;
        }

        /*
         * IEEE 802.3 Flow Control Resolution.
         * Please see $SDK/doc/pause-resolution.txt for more information.
         */

        if (duplex) {
             tx_pause = 	             
	                 ((remote_advert.pause & SOC_PA_PAUSE_RX) && 	 
	                  (local_advert.pause & SOC_PA_PAUSE_RX)) || 	 
	                 ((remote_advert.pause & SOC_PA_PAUSE_RX) && 	 
	                  !(remote_advert.pause & SOC_PA_PAUSE_TX) && 	 
	                  (local_advert.pause & SOC_PA_PAUSE_TX)); 	 
	  	 
	             rx_pause = 	 
	                 ((remote_advert.pause & SOC_PA_PAUSE_RX) && 	 
	                  (local_advert.pause & SOC_PA_PAUSE_RX)) || 	 
	                 ((local_advert.pause & SOC_PA_PAUSE_RX) && 	 
	                  (remote_advert.pause & SOC_PA_PAUSE_TX) && 	 
	                  !(local_advert.pause & SOC_PA_PAUSE_TX));
        } else {
            rx_pause = tx_pause = 0;
        }

        rv = (MAC_PAUSE_SET(PORT(unit, port).p_mac,
                           unit, port, tx_pause, rx_pause));
        if (BCM_FAILURE(rv)) {
            SOC_DEBUG_PRINT((DK_WARN,
                            "u=%d p=%d MAC_PAUSE_SET rv=%d\n",
                            unit, port, rv));
            return rv;
        }
    }

#ifdef INCLUDE_MACSEC
    {
    /* 
     * In MACSEC based PHYs the switch side could be operating with 
     * different speed,duplex and pause compared to line speed, duplex and
     * pause settings. The above setting will be overridden if the switch side
     * of the PHY is set to operate in fixed mode
     */
    uint32 value;
    rv = (soc_phyctrl_control_get(unit, port,
                                  BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED, 
                                  &value));

    if (BCM_E_UNAVAIL != rv) { /* Switch fixed speed is supported */

        if (value == 1) {
            /* Get and Set Speed */
            rv = (soc_phyctrl_control_get(unit, port,
                              BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED, 
                              &value));
            if (BCM_FAILURE(rv)) {
                SOC_DEBUG_PRINT((DK_WARN,
                    "u=%d p=%d phyctrl_control_get(speed) rv=%d\n",
                    unit, port, rv));
                return rv;
            }

            rv =  (MAC_SPEED_SET(PORT(unit, port).p_mac, unit, port, value));
            if (BCM_FAILURE(rv)) {
                SOC_DEBUG_PRINT((DK_WARN, 
                            "u=%d p=%d MAC_SPEED_SET speed=%d rv=%d\n",
                            unit, port, value, rv));
                return rv;
            }

            /* Get and Set Duplex */
            rv = (soc_phyctrl_control_get(unit, port,
                              BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX, 
                              &value));
            if (BCM_FAILURE(rv)) {
                SOC_DEBUG_PRINT((DK_WARN,
                    "u=%d p=%d phyctrl_control_get(duplex) rv=%d\n",
                    unit, port, rv));
                return rv;
            }
            rv = (MAC_DUPLEX_SET(PORT(unit, port).p_mac, unit, port, value));
            if (BCM_FAILURE(rv)) {
                SOC_DEBUG_PRINT((DK_WARN, "u=%d p=%d MAC_DUPLEX_SET %s sp=%d rv=%d\n", 
                                 unit, port, 
                                 value ? "FULL" : "HALF", value, rv));
                return rv;
            }

            /* Get and Set PAUSE */
            rv = (soc_phyctrl_control_get(unit, port,
                              BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE, 
                              &value));
            if (BCM_FAILURE(rv)) {
                SOC_DEBUG_PRINT((DK_WARN,
                    "u=%d p=%d phyctrl_control_get(Pause) rv=%d\n",
                    unit, port, rv));
                return rv;
            }
            if (value == 1) { 
                /* 
                 * Flow control domain is between switch MAC and PHY MAC.
                 * If the flow control domain is not between switch and PHY's
                 * switch MAC, then the flow control domain is extended to that 
                 * of GPHY.
                 */
                rv = (MAC_PAUSE_SET(PORT(unit, port).p_mac,
                               unit, port, value, value));
                if (BCM_FAILURE(rv)) {
                    SOC_DEBUG_PRINT((DK_WARN,
                                "u=%d p=%d MAC_PAUSE_SET rv=%d\n",
                                unit, port, rv));
                    return rv;
                }
            }

        }
    }
    }
#endif

#ifdef BCM_GXPORT_SUPPORT
    if (link && soc_feature(unit, soc_feature_port_lag_failover) && 
        IS_GX_PORT(unit, port)) {
        uint32 val;
        /* Toggle link bit to notify IPIPE on link up */
        BCM_IF_ERROR_RETURN
            (READ_GXPORT_LAG_FAILOVER_CONFIGr(unit, port, &val));
        soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr, &val,
                          LINK_STATUS_UPf, 1);
        BCM_IF_ERROR_RETURN
            (WRITE_GXPORT_LAG_FAILOVER_CONFIGr(unit, port, val));
        soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr, &val,
                          LINK_STATUS_UPf, 0);
        BCM_IF_ERROR_RETURN
            (WRITE_GXPORT_LAG_FAILOVER_CONFIGr(unit, port, val));
    }
#endif /* BCM_GXPORT_SUPPORT */

    /* Enable the MAC. */
    rv =  (MAC_ENABLE_SET(PORT(unit, port).p_mac, unit, port, TRUE));
    if (BCM_FAILURE(rv)) {
        SOC_DEBUG_PRINT((DK_WARN,
                        "u=%d p=%d MAC_ENABLE_SET TRUE rv=%d\n",
                        unit, port, rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_update
 * Purpose:
 *      Get port characteristics from PHY and program MAC to match.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      link - TRUE - process as link up.
 *             FALSE - process as link down.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_update(int unit, bcm_port_t port, int link)
{
    int         rv;
    
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = _bcm_port_update(unit, port, link);
    if (BCM_FAILURE(rv)) {
        PORT_UNLOCK(unit);
        return (rv);
    }

#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HBX(unit)) {
        rv = _bcm_port_mmu_update(unit, port, link);
    }
#endif /* BCM_BRADLEY_SUPPORT */

    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_update: u=%d p=%d link=%d rv=%d\n",
                     unit, port, link, rv));

    return(rv);
}

/*
 * Function:
 *      bcm_port_stp_set
 * Purpose:
 *      Set the spanning tree state for a port.
 *      All STGs containing all VLANs containing the port are updated.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *      stp_state - State to place port in, one of BCM_PORT_STP_xxx.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 * Notes:
 *      BCM_LOCK is taken so that the current list of VLANs
 *      can't change during the operation.
 */

int
bcm_esw_port_stp_set(int unit, bcm_port_t port, int stp_state)
{
    bcm_stg_t           *list = NULL;
    int                 count = 0, i;
    int                 rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_LOCK(unit);

    rv = bcm_esw_stg_list(unit, &list, &count);

    if (rv == BCM_E_UNAVAIL) {
        if (stp_state == BCM_STG_STP_FORWARD) {
            rv = BCM_E_NONE;
        } else {
            rv = BCM_E_PARAM;
        }
    } else if (BCM_SUCCESS(rv)) {
        for (i = 0; i < count; i++) {
            if ((rv = bcm_esw_stg_stp_set(unit, list[i], 
                                          port, stp_state)) < 0) {
                break;
            }
        }
        
        bcm_esw_stg_list_destroy(unit, list, count);
    }

    BCM_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_stp_set: u=%d p=%d state=%d rv=%d\n",
                     unit, port, stp_state, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_stp_get
 * Purpose:
 *      Get the spanning tree state for a port in the default STG.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *      stp_state - Pointer where state stored.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
bcm_esw_port_stp_get(int unit, bcm_port_t port, int *stp_state)
{
    int                 stg_defl, rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = bcm_esw_stg_default_get(unit, &stg_defl);
    if (rv >= 0) {
        rv = bcm_esw_stg_stp_get(unit, stg_defl, port, stp_state);
    } else if (rv == BCM_E_UNAVAIL) {   /* FABRIC switches, etc */
        *stp_state = BCM_STG_STP_FORWARD;
        rv = BCM_E_NONE;
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_stp_get: u=%d p=%d state=%d rv=%d\n",
                     unit, port, *stp_state, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_bpdu_enable_set
 * Purpose:
 *      Enable/Disable BPDU reception on the specified port.
 * Parameters:
 *      unit - SOC unit #
 *      port - Port number (0 based)
 *      enable - TRUE to enable, FALSE to disable (reject bpdu).
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
bcm_esw_port_bpdu_enable_set(int unit, bcm_port_t port, int enable)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        pcfg.pc_bpdu_disable = !enable;
        rv = mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg);
    }

    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_bpdu_enable_get
 * Purpose:
 *      Return whether BPDU reception is enabled on the specified port.
 * Parameters:
 *      unit - SOC unit #
 *      port - Port number (0 based)
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
bcm_esw_port_bpdu_enable_get(int unit, bcm_port_t port, int *enable)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        *enable = !pcfg.pc_bpdu_disable;
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_l3_enable_set
 * Purpose:
 *      Enable/Disable L3 switching on the specified port.
 * Parameters:
 *      unit -          device number
 *      port -          port number
 *      enable -        TRUE to enable, FALSE to disable.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_l3_enable_set(int unit, bcm_port_t port, int enable)
{
    int  rv = BCM_E_UNAVAIL;
#if defined(INCLUDE_L3)
    bcm_port_cfg_t      pcfg;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        if (enable) {
            pcfg.pc_l3_flags |= (BCM_PORT_L3_V4_ENABLE | BCM_PORT_L3_V6_ENABLE);
        } else {
            pcfg.pc_l3_flags &= 
                ~(BCM_PORT_L3_V4_ENABLE | BCM_PORT_L3_V6_ENABLE);
        }
        rv = mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg);
    }

    PORT_UNLOCK(unit);
#endif /* INCLUDE_L3 */
    return rv;
}

/*
 * Function:
 *      bcm_port_l3_enable_get
 * Purpose:
 *      Return whether L3 switching is enabled on the specified port.
 * Parameters:
 *      unit -          device number
 *      port -          port number
 *      enable -        (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_l3_enable_get(int unit, bcm_port_t port, int *enable)
{
    int  rv = BCM_E_UNAVAIL;
#if defined(INCLUDE_L3)
    bcm_port_cfg_t      pcfg;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        *enable = ((pcfg.pc_l3_flags & BCM_PORT_L3_V4_ENABLE) || 
                   (pcfg.pc_l3_flags & BCM_PORT_L3_V6_ENABLE));
    }
#endif /* INCLUDE_L3 */

    return rv;
}

/*
 * Function:
 *      bcm_port_tgid_get
 * Purpose:
 *      Get the trunk group for a given port.
 * Parameters:
 *      unit - SOC unit #
 *      port - Port number (0 based)
 *      tid - trunk ID
 *      psc - trunk selection criterion
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
bcm_esw_port_tgid_get(int unit, bcm_port_t port, int *tid, int *psc)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        *tid = pcfg.pc_tgid;
        *psc = 0;
    }

    return rv;

}

/*
 * Function:
 *      bcm_port_tgid_set
 * Purpose:
 *      Set the trunk group for a given port.
 * Parameters:
 *      unit - SOC unit #
 *      port - Port number (0 based)
 *      tid - trunk ID
 *      psc - trunk selection criterion
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
bcm_esw_port_tgid_set(int unit, bcm_port_t port, int tid, int psc)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        if (tid != BCM_TRUNK_INVALID) {
            pcfg.pc_tgid = tid;
            pcfg.pc_trunk = 1;
        } else {
            pcfg.pc_tgid = 0;
            pcfg.pc_trunk = 0;
        }
        rv = mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg);
    }

    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_port_mirror_enable_get (internal)
 * Purpose:
 *       Return whether mirroring is enabled on the specified port.
 * Parameters:
 *      unit   - (IN) BCM device number. 
 *      port   - (IN) Port number (0 based).
 *      enable - (OUT) Bitmap of ingress mirror enabled. 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
_bcm_port_mirror_enable_get(int unit, bcm_port_t port, int *enable)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

    /* Input parameters check. */
    if (NULL == enable) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        *enable = pcfg.pc_mirror_ing;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_port_mirror_enable_set (internal)
 * Purpose:
 *       Enable/Disable mirroring for a given port.
 * Parameters:
 *      unit -   (IN) BCM unit #
 *      port -   (IN) Port number (0 based)
 *      enable - (IN) Bitmap of ingress mirror enabled. 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
_bcm_port_mirror_enable_set(int unit, bcm_port_t port, int enable)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        pcfg.pc_mirror_ing = enable;
        rv = mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg);
    }

    PORT_UNLOCK(unit);

    return rv;
}

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *      _bcm_port_mirror_egress_true_enable_get (internal)
 * Purpose:
 *       Return whether true egress mirroring is enabled on the
 *       specified port.
 * Parameters:
 *      unit   - (IN) BCM device number. 
 *      port   - (IN) Port number (0 based).
 *      enable - (OUT) Bitmap of true egress mirrors enabled. 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
_bcm_port_mirror_egress_true_enable_get(int unit, bcm_port_t port,
                                        int *enable)
{
    uint64 egr_val64;
    int                 rv;

    /* Input parameters check. */
    if (NULL == enable) {
        return (BCM_E_PARAM);
    }

    rv = READ_EGR_PORT_64r(unit, port, &egr_val64);
    if (BCM_SUCCESS(rv)) {
        *enable = soc_reg64_field32_get(unit, EGR_PORT_64r, egr_val64, MIRRORf);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_port_mirror_egress_true_enable_set (internal)
 * Purpose:
 *       Enable/Disable true egress mirroring for a given port.
 * Parameters:
 *      unit -   (IN) BCM unit #
 *      port -   (IN) Port number (0 based)
 *      enable - (IN) Bitmap of true egress mirrors enabled. 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */

int
_bcm_port_mirror_egress_true_enable_set(int unit, bcm_port_t port,
                                        int enable)
{
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    rv = soc_reg_field32_modify(unit, EGR_PORT_64r, port, MIRRORf, enable);
    if (BCM_SUCCESS(rv) && IS_HG_PORT(unit, port)) {
        rv = soc_reg_field32_modify(unit, IEGR_PORT_64r, port,
                                    MIRRORf, enable);
    }

    PORT_UNLOCK(unit);
    return rv;
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      bcm_port_linkscan_get
 * Purpose:
 *      Get the link scan state of the port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      linkscan - (OUT) Linkscan value (None, S/W, H/W)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_linkscan_get(int unit, bcm_port_t port, int *linkscan)
{
    return bcm_esw_linkscan_mode_get(unit, port, linkscan);
}

/*
 * Function:
 *      bcm_port_linkscan_set
 * Purpose:
 *      Set the linkscan state for a given port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      linkscan - Linkscan value (None, S/W, H/W)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_linkscan_set(int unit, bcm_port_t port, int linkscan)
{
    return bcm_esw_linkscan_mode_set(unit, port, linkscan);
}

/*
 * Function:
 *      bcm_port_autoneg_get
 * Purpose:
 *      Get the autonegotiation state of the port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      autoneg - (OUT) Boolean value
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_autoneg_get(int unit, bcm_port_t port, int *autoneg)
{
    int done, rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = soc_phyctrl_auto_negotiate_get(unit, port, autoneg, &done);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_autoneg_get: u=%d p=%d an=%d done=%d rv=%d\n",
                     unit, port, *autoneg, done, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_autoneg_set
 * Purpose:
 *      Set the autonegotiation state for a given port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      autoneg - Boolean value
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_autoneg_set(int unit, bcm_port_t port, int autoneg)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = soc_phyctrl_auto_negotiate_set(unit, port, autoneg);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_autoneg_set: u=%d p=%d an=%d rv=%d\n",
                     unit, port, autoneg, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_speed_get
 * Purpose:
 *      Getting the speed of the port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      speed - (OUT) Value in megabits/sec (10, 100, etc)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      If port is in MAC loopback, the speed of the loopback is returned.
 */

int
bcm_esw_port_speed_get(int unit, bcm_port_t port, int *speed)
{
    int         rv = SOC_E_NONE;
    int         mac_lb;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    rv = MAC_LOOPBACK_GET(PORT(unit, port).p_mac, unit, port, &mac_lb);

    if (BCM_SUCCESS(rv)) {
        if (mac_lb || (IS_HG_PORT(unit, port) && !IS_GX_PORT(unit, port))) {
            rv = MAC_SPEED_GET(PORT(unit, port).p_mac, unit, port, speed);
        } else {
            rv = soc_phyctrl_speed_get(unit, port, speed);
            if (BCM_E_UNAVAIL == rv) {
                /* PHY driver doesn't support speed_get. Get the speed from
                 * MAC.
                 */
                rv = MAC_SPEED_GET(PORT(unit, port).p_mac, unit, port, speed);
            }
            if (IS_HG_PORT(unit, port) && *speed < 10000) {
                *speed = 0;
            }
        }
    }

    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_speed_get: u=%d p=%d speed=%d rv=%d\n",
                     unit, port, BCM_SUCCESS(rv) ? *speed : 0, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_speed_max
 * Purpose:
 *      Getting the maximum speed of the port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      speed - (OUT) Value in megabits/sec (10, 100, etc)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_speed_max(int unit, bcm_port_t port, int *speed)
{
    bcm_port_ability_t  ability;
    int                 rv;

    if (NULL == speed) {
        return (BCM_E_PARAM);
    }

    rv = bcm_esw_port_ability_local_get(unit, port, &ability);

    if (BCM_SUCCESS(rv)) {
        
        *speed = BCM_PORT_ABILITY_SPEED_MAX(ability.speed_full_duplex | ability.speed_half_duplex);
        if (10000 == *speed) {
            if (IS_HG_PORT(unit, port) && SOC_INFO(unit).port_speed_max[port]) {
                *speed = SOC_INFO(unit).port_speed_max[port];
            }
        }
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_speed_max: u=%d p=%d speed=%d rv=%d\n",
                     unit, port, *speed, rv));

    return rv;
}

/*
 * Function:
 *      _bcm_port_speed_set
 * Purpose:
 *      Main part of bcm_port_speed_set.
 */

STATIC int
_bcm_port_speed_set(int unit, bcm_port_t port, int speed)
{
    int         lb_phy, force_phy, rv, enable;
    int             mac_lb;

    /*
     * If port is in MAC loopback mode, do not try setting the PHY
     * speed.  This allows MAC loopback at 10/100 even if the PHY is
     * 1000 only.  Loopback diagnostic tests should enable loopback
     * before setting the speed, and vice versa when cleaning up.
     */

    SOC_IF_ERROR_RETURN
        (MAC_LOOPBACK_GET(PORT(unit, port).p_mac, unit, port, &mac_lb));

    force_phy = !mac_lb;

    if (speed == 0) {
        /* if speed is 0, set the port speed to max */
        SOC_IF_ERROR_RETURN
            (bcm_esw_port_speed_max(unit, port, &speed));    
    }

    if (force_phy) {
        bcm_port_ability_t      mac_ability, phy_ability;
        bcm_port_ability_t      requested_ability;

        /* Make sure MAC can handle the requested speed. */
        SOC_IF_ERROR_RETURN
            (MAC_ABILITY_LOCAL_GET(PORT(unit, port).p_mac, unit, port, &mac_ability));
        requested_ability.speed_full_duplex = SOC_PA_SPEED(speed);
        requested_ability.speed_half_duplex = SOC_PA_SPEED(speed);
        SOC_DEBUG_PRINT((DK_PHY,
         "_bcm_port_speed_set: u=%u p=%d MAC FD speed %08X MAC HD speed %08X \
          Requested FD Speed %08X Requested HD Speed %08X\n",
                     unit,
                     port,
                     mac_ability.speed_full_duplex,
                     mac_ability.speed_half_duplex,       
                     requested_ability.speed_full_duplex,
                     requested_ability.speed_half_duplex));

        if (((mac_ability.speed_full_duplex &\
               requested_ability.speed_full_duplex) == 0) &&
            ((mac_ability.speed_half_duplex &\
               requested_ability.speed_half_duplex) == 0) ) {
            SOC_DEBUG_PRINT((DK_VERBOSE,
                             "u=%d p=%d MAC doesn't support %d Mbps speed.\n",
                             unit, port, speed));
            return SOC_E_CONFIG;
        }

        SOC_IF_ERROR_RETURN
            (soc_phyctrl_ability_local_get(unit, port, &phy_ability));

        if (((phy_ability.speed_full_duplex &\
               requested_ability.speed_full_duplex) == 0) &&
            ((phy_ability.speed_half_duplex &\
               requested_ability.speed_half_duplex) == 0) ) {

            SOC_DEBUG_PRINT((DK_VERBOSE,
                             "u=%d p=%d PHY doesn't support %d Mbps speed.\n",
                             unit, port, speed));
            return SOC_E_CONFIG;
        }

        SOC_IF_ERROR_RETURN
            (soc_phyctrl_auto_negotiate_set(unit, port, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_speed_set(unit, port, speed));
    }
    
    /* Prevent PHY register access while resetting BigMAC and Fusion core */
    if (IS_HG_PORT(unit, port)) {
        soc_phyctrl_enable_get(unit, port, &enable);
        soc_phyctrl_enable_set(unit, port, 0);
        soc_phyctrl_loopback_get(unit, port, &lb_phy);
    }

    rv = MAC_SPEED_SET(PORT(unit, port).p_mac, unit, port, speed);
    SOC_IF_ERROR_DEBUG_PRINT
        (rv, (DK_VERBOSE, "MAC_SPEED_SET failed: %s\n", bcm_errmsg(rv)));

    /* Restore PHY register access */
    if (IS_HG_PORT(unit, port)) {
        soc_phyctrl_enable_set(unit, port, enable);
        soc_phyctrl_loopback_set(unit, port, lb_phy);
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_speed_set
 * Purpose:
 *      Setting the speed for a given port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      speed - Value in megabits/sec (10, 100, etc)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Turns off autonegotiation.  Caller must make sure other forced
 *      parameters (such as duplex) are set.
 */

int
bcm_esw_port_speed_set(int unit, bcm_port_t port, int speed)
{
    int         rv, max_speed;
    pbmp_t      pbm;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_speed_max(unit,port, &max_speed));

    if (speed < 0 || speed > max_speed) {
        return BCM_E_CONFIG;
    }

    PORT_LOCK(unit);                    /* multiple operations operation */
    rv = _bcm_port_speed_set(unit, port, speed);
    PORT_UNLOCK(unit);                  /* Unlock before link call */

    if (BCM_SUCCESS(rv) && !SAL_BOOT_SIMULATION) {
        SOC_PBMP_CLEAR(pbm);
        SOC_PBMP_PORT_ADD(pbm, port);
        (void)bcm_esw_link_change(unit, pbm);
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_speed_set: u=%d p=%d speed=%d rv=%d\n",
                     unit, port, speed, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_master_get
 * Purpose:
 *      Getting the master status of the port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ms - (OUT) BCM_PORT_MS_*
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      WARNING: assumes BCM_PORT_MS_* matches SOC_PORT_MS_*
 */

int
bcm_esw_port_master_get(int unit, bcm_port_t port, int *ms)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = soc_phyctrl_master_get(unit, port, ms);
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_master_set
 * Purpose:
 *      Setting the master status for a given port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ms - BCM_PORT_MS_*
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Ignored if not supported on port.
 *      WARNING: assumes BCM_PORT_MS_* matches SOC_PORT_MS_*
 */

int
bcm_esw_port_master_set(int unit, bcm_port_t port, int ms)
{
    int         rv;
    pbmp_t      pbm;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);                    /* multiple operations operation */

    rv = soc_phyctrl_master_set(unit, port, ms);
    SOC_IF_ERROR_DEBUG_PRINT
        (rv, (DK_VERBOSE, "PHY_MASTER_SET failed: %s\n", bcm_errmsg(rv)));

    PORT_UNLOCK(unit);                  /* Unlock before link call */

    if (BCM_SUCCESS(rv)) {
        SOC_PBMP_CLEAR(pbm);
        SOC_PBMP_PORT_ADD(pbm, port);
        (void)bcm_esw_link_change(unit, pbm);
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_interface_get
 * Purpose:
 *      Getting the interface type of a port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      intf - (OUT) BCM_PORT_IF_*
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      WARNING: assumes BCM_PORT_IF_* matches SOC_PORT_IF_*
 */

int
bcm_esw_port_interface_get(int unit, bcm_port_t port, bcm_port_if_t *intf)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = soc_phyctrl_interface_get(unit, port, intf);
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_interface_set
 * Purpose:
 *      Setting the interface type for a given port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      if - BCM_PORT_IF_*
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      WARNING: assumes BCM_PORT_IF_* matches SOC_PORT_IF_*
 */

int
bcm_esw_port_interface_set(int unit, bcm_port_t port, bcm_port_if_t intf)
{
    int         rv;
    pbmp_t      pbm;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);                    /* multiple operations operation */

    rv = soc_phyctrl_interface_set(unit, port, intf);
    SOC_IF_ERROR_DEBUG_PRINT
        (rv, (DK_VERBOSE, "PHY_INTERFACE_SET failed: %s\n", bcm_errmsg(rv)));

    PORT_UNLOCK(unit);                  /* Unlock before link call */

    if (BCM_SUCCESS(rv)) {
        SOC_PBMP_CLEAR(pbm);
        SOC_PBMP_PORT_ADD(pbm, port);
        (void)bcm_esw_link_change(unit, pbm);
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_loopback_set
 * Purpose:
 *      Setting the speed for a given port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      loopback - one of:
 *              BCM_PORT_LOOPBACK_NONE
 *              BCM_PORT_LOOPBACK_MAC
 *              BCM_PORT_LOOPBACK_PHY
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_loopback_set(int unit, bcm_port_t port, int loopback)
{
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = BCM_E_NONE;
    /*
     * Always force link before changing hardware to avoid
     * race with the linkscan thread.
     */
    if (!(loopback == BCM_PORT_LOOPBACK_NONE)) {
        rv = _bcm_esw_link_force(unit, port, TRUE, FALSE);
    }

    PORT_LOCK(unit);

    if (BCM_SUCCESS(rv)) {
        rv = MAC_LOOPBACK_SET(PORT(unit, port).p_mac, unit, port,
                              (loopback == BCM_PORT_LOOPBACK_MAC));
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_phyctrl_loopback_set(unit, port,
                              (loopback == BCM_PORT_LOOPBACK_PHY));
    }

    /* some mac loopback implementations require the phy to also be in loopback */
    if (soc_feature(unit, soc_feature_phy_lb_needed_in_mac_lb) &&
        (loopback == BCM_PORT_LOOPBACK_MAC)) {
              rv = soc_phyctrl_loopback_set(unit, port, 1);
     }

    PORT_UNLOCK(unit);                  /* unlock before link call */

    if ((loopback == BCM_PORT_LOOPBACK_NONE) || !BCM_SUCCESS(rv)) {
        _bcm_esw_link_force(unit, port, FALSE, DONT_CARE);
    } else {
        /* Enable only MAC instead of calling bcm_port_enable_set so
         * that this API doesn't silently enable the port if the 
         * port is disabled by application.
         */ 
        rv = MAC_ENABLE_SET(PORT(unit, port).p_mac, unit, port, TRUE);

        if (BCM_SUCCESS(rv)) {
            /* Make sure that the link status is updated only after the
             * MAC is enabled so that link_mask2 is set before the
             * calling thread synchronizes with linkscan thread in
             * _bcm_link_force call.
             * If the link is forced before MAC is enabled, there could
             * be a race condition in _soc_link_update where linkscan 
             * may use an old view of link_mask2 and override the
             * EPC_LINK_BMAP after the mac_enable_set updates 
             * link_mask2 and EPC_LINK_BMAP. 
             */
            rv = _bcm_esw_link_force(unit, port, TRUE, TRUE);
        }
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

#if defined(BCM_BRADLEY_SUPPORT)
        /*
         * Call _bcm_port_mmu_update explicitly because linkscan
         * will not call bcm_port_update when the link is forced.
         */
        if (SOC_IS_HBX(unit)) {
            rv = _bcm_port_mmu_update(unit, port, 1);
        }
#endif /* BCM_BRADLEY_SUPPORT */
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_loopback_set: u=%d p=%d lb=%d rv=%d\n",
                     unit, port, loopback, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_loopback_get
 * Purpose:
 *      Recover the current loopback operation for the specified port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      loopback - (OUT) one of:
 *              BCM_PORT_LOOPBACK_NONE
 *              BCM_PORT_LOOPBACK_MAC
 *              BCM_PORT_LOOPBACK_PHY
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_loopback_get(int unit, bcm_port_t port, int *loopback)
{
    int         rv = BCM_E_NONE;
    int         phy_lb = 0;
    int         mac_lb = 0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);                    /* multiple operations operation */

    rv = soc_phyctrl_loopback_get(unit, port, &phy_lb);
    if (rv >= 0) {
        rv = MAC_LOOPBACK_GET(PORT(unit, port).p_mac, unit, port, &mac_lb);
    }

    PORT_UNLOCK(unit);

    if (rv >= 0) {
        if (mac_lb) {
            *loopback = BCM_PORT_LOOPBACK_MAC;
        } else if (phy_lb) {
            *loopback = BCM_PORT_LOOPBACK_PHY;
        } else {
            *loopback = BCM_PORT_LOOPBACK_NONE;
        }
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_loopback_get: u=%d p=%d lb=%d rv=%d\n",
                     unit, port, *loopback, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_duplex_get
 * Purpose:
 *      Get the port duplex settings
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Duplex setting, one of SOC_PORT_DUPLEX_xxx
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_duplex_get(int unit, bcm_port_t port, int *duplex)
{
    int         phy_duplex;
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = soc_phyctrl_duplex_get(unit, port, &phy_duplex);
    PORT_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        *duplex = phy_duplex ? SOC_PORT_DUPLEX_FULL : SOC_PORT_DUPLEX_HALF;
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_duplex_get: u=%d p=%d dup=%d rv=%d\n",
                     unit, port, *duplex, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_duplex_set
 * Purpose:
 *      Set the port duplex settings.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      duplex - Duplex setting, one of SOC_PORT_DUPLEX_xxx
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Turns off autonegotiation.  Caller must make sure other forced
 *      parameters (such as speed) are set.
 */

int
bcm_esw_port_duplex_set(int unit, bcm_port_t port, int duplex)
{
    int         rv;
    pbmp_t      pbm;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    rv = soc_phyctrl_auto_negotiate_set(unit, port, FALSE);
    SOC_IF_ERROR_DEBUG_PRINT
        (rv, (DK_VERBOSE, "PHY_AUTONEG_SET failed: %s\n", bcm_errmsg(rv)));

    if (BCM_SUCCESS(rv)) {
        rv = soc_phyctrl_duplex_set(unit, port, duplex);
        SOC_IF_ERROR_DEBUG_PRINT
            (rv, (DK_VERBOSE, "PHY_DUPLEX_SET failed: %s\n", bcm_errmsg(rv)));
    }

    if (BCM_SUCCESS(rv)) {
        rv = MAC_DUPLEX_SET(PORT(unit, port).p_mac, unit, port, duplex);
        SOC_IF_ERROR_DEBUG_PRINT
            (rv, (DK_VERBOSE, "MAC_DUPLEX_SET failed: %s\n", bcm_errmsg(rv)));
    }

    PORT_UNLOCK(unit);                  /* Unlock before link call */

    if (BCM_SUCCESS(rv) && !SAL_BOOT_SIMULATION) {
        SOC_PBMP_CLEAR(pbm);
        SOC_PBMP_PORT_ADD(pbm, port);
        (void)bcm_esw_link_change(unit, pbm);
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_duplex_set: u=%d p=%d dup=%d rv=%d\n",
                     unit, port, duplex, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_pause_get
 * Purpose:
 *      Get the source address for transmitted PAUSE frames.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      pause_tx - (OUT) Boolean value
 *      pause_rx - (OUT) Boolean value
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_pause_get(int unit, bcm_port_t port, int *pause_tx, int *pause_rx)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = MAC_PAUSE_GET(PORT(unit, port).p_mac, unit, port, pause_tx, pause_rx);
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_pause_set
 * Purpose:
 *      Set the pause state for a given port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      pause_tx - Boolean value, or -1 (don't change)
 *      pause_rx - Boolean value, or -1 (don't change)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Symmetric pause requires the two "pause" values to be the same.
 */

int
bcm_esw_port_pause_set(int unit, bcm_port_t port, int pause_tx, int pause_rx)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = MAC_PAUSE_SET(PORT(unit, port).p_mac, unit, port, pause_tx, pause_rx);
    if (BCM_FAILURE(rv)) {
        PORT_UNLOCK(unit);
        return (rv);
    }

#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HBX(unit)) {
        rv = _bcm_port_mmu_update(unit, port, -1);
    }
#endif /* BCM_BRADLEY_SUPPORT */ 

    PORT_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_port_pause_sym_get
 * Purpose:
 *      Get the current pause setting for pause
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      pause - (OUT) returns a bcm_port_pause_e enum value
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_pause_sym_get(int unit, bcm_port_t port, int *pause)
{
    int         rv;
    int         pause_rx, pause_tx;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = MAC_PAUSE_GET(PORT(unit, port).p_mac, unit, port,
                       &pause_tx, &pause_rx);
    PORT_UNLOCK(unit);

    BCM_IF_ERROR_RETURN(rv);
    if (pause_tx) {
        if (pause_rx) {
            *pause = BCM_PORT_PAUSE_SYM;
        } else {
            *pause = BCM_PORT_PAUSE_ASYM_TX;
        }
    } else if (pause_rx) {
        *pause = BCM_PORT_PAUSE_ASYM_RX;
    } else {
        *pause = BCM_PORT_PAUSE_NONE;
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_pause_sym_set
 * Purpose:
 *      Set the pause values for the port using single integer
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      pause - a bcm_port_pause_e enum value
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_pause_sym_set(int unit, bcm_port_t port, int pause)
{
    int         pause_rx, pause_tx;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    pause_tx = pause_rx = 0;

    switch (pause) {
    case BCM_PORT_PAUSE_SYM:
        pause_tx = pause_rx = 1;
        break;
    case BCM_PORT_PAUSE_ASYM_RX:
        pause_rx = 1;
        break;
    case BCM_PORT_PAUSE_ASYM_TX:
        pause_tx = 1;
        break;
    case BCM_PORT_PAUSE_NONE:
        break;
    default:
        return BCM_E_PARAM;
    }

    return bcm_esw_port_pause_set(unit, port, pause_tx, pause_rx);
}

/*
 * Function:
 *      bcm_port_pause_addr_get
 * Purpose:
 *      Get the source address for transmitted PAUSE frames.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      mac - (OUT) MAC address sent with pause frames.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_pause_addr_get(int unit, bcm_port_t port, bcm_mac_t mac)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = MAC_PAUSE_ADDR_GET(PORT(unit, port).p_mac, unit, port, mac);
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_pause_addr_set
 * Purpose:
 *      Set the source address for transmitted PAUSE frames.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      mac - station address used for pause frames.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Symmetric pause requires the two "pause" values to be the same.
 */

int
bcm_esw_port_pause_addr_set(int unit, bcm_port_t port, bcm_mac_t mac)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = MAC_PAUSE_ADDR_SET(PORT(unit, port).p_mac, unit, port, mac);
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_advert_get
 * Purpose:
 *      Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - (OUT) Local advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_advert_get(int unit, bcm_port_t port, bcm_port_abil_t *ability_mask)
{
    int                 rv;
    bcm_port_ability_t  ability;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = soc_phyctrl_ability_advert_get(unit, port, &ability);
    if (BCM_SUCCESS(rv)) {
        rv = soc_port_ability_to_mode(&ability, ability_mask);
    }
    PORT_UNLOCK(unit);



    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_advert_get: u=%d p=%d abil=0x%x rv=%d\n",
                     unit, port, *ability_mask, rv));

    return rv;
}

/*
 * Function:
 *      bcm_esw_port_ability_advert_get
 * Purpose:
 *      Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - (OUT) Local advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_ability_advert_get(int unit, bcm_port_t port, 
                                bcm_port_ability_t *ability_mask)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = soc_phyctrl_ability_advert_get(unit, port, ability_mask);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_esw_port_ability_advert_get: u=%d p=%d rv=%d\n",
                     unit, port, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_advert_set
 * Purpose:
 *      Set the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - Local advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This call MAY NOT restart autonegotiation (depending on the phy).
 *      To do that, follow this call with bcm_port_autoneg_set(TRUE).
 */

int
bcm_esw_port_advert_set(int unit, bcm_port_t port, bcm_port_abil_t ability_mask)
{
    int                 rv;
    bcm_port_ability_t  given_ability, port_ability;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_ability_local_get(unit, port, &port_ability));

    BCM_IF_ERROR_RETURN(
        soc_port_mode_to_ability(ability_mask, &given_ability));

    /* make sure that the ability advertising in PHY is supported by MAC */ 
    given_ability.flags &= port_ability.flags;
    given_ability.loopback &= port_ability.loopback;
    given_ability.medium &= port_ability.medium;
    given_ability.pause &= port_ability.pause;
    given_ability.speed_full_duplex &= port_ability.speed_full_duplex;
    given_ability.speed_half_duplex &= port_ability.speed_half_duplex;

    if (IS_HG_PORT(unit, port) && SOC_INFO(unit).port_speed_max[port]) {
        if (SOC_INFO(unit).port_speed_max[port] < 16000) {
            given_ability.speed_full_duplex &= ~(BCM_PORT_ABILITY_16GB);
            given_ability.speed_half_duplex &= ~(BCM_PORT_ABILITY_16GB);
        }
        
        if (SOC_INFO(unit).port_speed_max[port] < 13000) {
            given_ability.speed_full_duplex &= ~(BCM_PORT_ABILITY_13GB);
            given_ability.speed_half_duplex &= ~(BCM_PORT_ABILITY_13GB);
        }

        if (SOC_INFO(unit).port_speed_max[port] < 12000) {
            given_ability.speed_full_duplex &= ~(BCM_PORT_ABILITY_12GB);
            given_ability.speed_half_duplex &= ~(BCM_PORT_ABILITY_12GB);
        }  
    }
 
    PORT_LOCK(unit);
    rv = soc_phyctrl_ability_advert_set(unit, port, &given_ability);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_advert_set: u=%d p=%d abil=0x%x rv=%d\n",
                     unit, port, ability_mask, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_ability_advert_set
 * Purpose:
 *      Set the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - Local advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This call MAY NOT restart autonegotiation (depending on the phy).
 *      To do that, follow this call with bcm_port_autoneg_set(TRUE).
 */

int
bcm_esw_port_ability_advert_set(int unit, bcm_port_t port,
                                bcm_port_ability_t *ability_mask)
{
    int             rv;
    bcm_port_ability_t port_ability;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_ability_local_get(unit, port, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.speed_half_duplex   &= ability_mask->speed_half_duplex;
    port_ability.speed_full_duplex   &= ability_mask->speed_full_duplex;
    port_ability.pause      &= ability_mask->pause;
    port_ability.interface  &= ability_mask->interface;
    port_ability.medium     &= ability_mask->medium;
    port_ability.loopback   &= ability_mask->loopback;
    port_ability.flags      &= ability_mask->flags;

    PORT_LOCK(unit);
    rv = soc_phyctrl_ability_advert_set(unit, port, &port_ability);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_advert_set: u=%d p=%d rv=%d\n",
                     unit, port, rv));
    SOC_DEBUG_PRINT((DK_PORT | DK_VERBOSE,
                     "Speed(HD=0x%08x, FD=0x%08x) Pause=0x%08x\n"
                     "Interface=0x%08x Medium=0x%08x Loopback=0x%08x Flags=0x%08x\n",
                     port_ability.speed_half_duplex,
                     port_ability.speed_full_duplex,
                     port_ability.pause, port_ability.interface,
                     port_ability.medium, port_ability.loopback,
                     port_ability.flags));

    return rv;
}

/*
 * Function:
 *      bcm_port_advert_remote_get
 * Purpose:
 *      Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - (OUT) Remote advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_advert_remote_get(int unit, bcm_port_t port, bcm_port_abil_t *ability_mask)
{
    int                 rv;
    bcm_port_ability_t  port_ability;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = _bcm_port_autoneg_advert_remote_get(unit, port, &port_ability);
    if (BCM_SUCCESS(rv)) {
        rv = soc_port_ability_to_mode(&port_ability, ability_mask);
    }
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_advert_remote_get: u=%d p=%d abil=0x%x rv=%d\n",
                     unit, port, *ability_mask, rv));

    return rv;
}


/*
 * Function:
 *      bcm_port_ability_remote_get
 * Purpose:
 *      Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - (OUT) Remote advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_port_ability_remote_get(int unit, bcm_port_t port,
                           bcm_port_ability_t *ability_mask)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = _bcm_port_autoneg_advert_remote_get(unit, port, ability_mask);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_remote_get: u=%d p=%d rv=%d\n",
                     unit, port, rv));
    SOC_DEBUG_PRINT((DK_PORT | DK_VERBOSE,
                     "Speed(HD=0x%08x, FD=0x%08x) Pause=0x%08x\n"
                     "Interface=0x%08x Medium=0x%08x Loopback=0x%08x Flags=0x%08x\n",
                     ability_mask->speed_half_duplex,
                     ability_mask->speed_full_duplex,
                     ability_mask->pause, ability_mask->interface,
                     ability_mask->medium, ability_mask->loopback,
                     ability_mask->flags));

    return rv;
}

/*
 * Function:
 *      bcm_port_ability_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - (OUT) Mask of BCM_PORT_ABIL_ values indicating the
 *              ability of the MAC/PHY.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_ability_get(int unit, bcm_port_t port, bcm_port_abil_t *ability_mask)
{
    int                 rv;
    bcm_port_ability_t  port_ability;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = _bcm_port_ability_local_get(unit, port, &port_ability);
    if (BCM_SUCCESS(rv)) {
        rv = soc_port_ability_to_mode(&port_ability, ability_mask);
    }
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_get: u=%d p=%d abil=0x%x rv=%d\n",
                     unit, port, *ability_mask, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_ability_local_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - (OUT) Mask of BCM_PORT_ABIL_ values indicating the
 *              ability of the MAC/PHY.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_ability_local_get(int unit, bcm_port_t port,
                               bcm_port_ability_t *ability_mask)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = _bcm_port_ability_local_get(unit, port, ability_mask);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_local_get: u=%d p=%d rv=%d\n",
                     unit, port, rv));
    SOC_DEBUG_PRINT((DK_PORT | DK_VERBOSE,
                     "Speed(HD=0x%08x, FD=0x%08x) Pause=0x%08x\n"
                     "Interface=0x%08x Medium=0x%08x Loopback=0x%08x Flags=0x%08x\n",
                     ability_mask->speed_half_duplex,
                     ability_mask->speed_full_duplex,
                     ability_mask->pause, ability_mask->interface,
                     ability_mask->medium, ability_mask->loopback,
                     ability_mask->flags));

    return rv;
}
/*
 * Function:
 *      bcm_port_discard_get
 * Purpose:
 *      Get port discard attributes for the specified port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port discard mode, one of BCM_PORT_DISCARD_xxx
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_discard_get(int unit, bcm_port_t port, int *mode)
{
    bcm_port_cfg_t pcfg;
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if ((rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg)) ==
        BCM_E_NONE) {
        *mode = pcfg.pc_disc;
    }

    return(rv);
}

/*
 * Function:
 *      bcm_port_discard_set
 * Purpose:
 *      Set port discard attributes for the specified port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      mode - Port discard mode, one of BCM_PORT_DISCARD_xxx
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_discard_set(int unit, bcm_port_t port, int mode)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    switch (mode) {
    case BCM_PORT_DISCARD_TAG:
#ifdef BCM_XGS12_SWITCH_SUPPORT
        if (SOC_IS_DRACO1(unit) || SOC_IS_LYNX(unit) ||
            SOC_IS_TUCANA(unit)) {
            /* This mode not supported on 5690/73/74/65/50 */
            return BCM_E_UNAVAIL;
        }
#endif
        /* Fall through */
    case BCM_PORT_DISCARD_NONE:
    case BCM_PORT_DISCARD_ALL:
    case BCM_PORT_DISCARD_UNTAG:
        break;
    default:
        return BCM_E_PARAM;
    }

    PORT_LOCK(unit);

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    if (BCM_SUCCESS(rv)) {
        pcfg.pc_disc = mode;
        rv = mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg);
    }

    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_port_learn_modify
 * Purpose:
 *      Main part of bcm_port_learn_modify.
 */

STATIC int
_bcm_port_learn_modify(int unit, bcm_port_t port, uint32 add, uint32 remove)
{
    uint32      flags;

    SOC_IF_ERROR_RETURN(bcm_esw_port_learn_get(unit, port, &flags));

    flags |= add;
    flags &= ~remove;

    SOC_IF_ERROR_RETURN(bcm_esw_port_learn_set(unit, port, flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_learn_modify
 * Purpose:
 *      Modify the port learn flags, adding add and removing remove flags.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      add  - Flags to set.
 *      remove - Flags to clear.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_learn_modify(int unit, bcm_port_t port, uint32 add, uint32 remove)
{
    int         rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = _bcm_port_learn_modify(unit, port, add, remove);
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_learn_get
 * Purpose:
 *      Get the ARL hardware learning options for this port.
 *      This defines what the hardware will do when a packet
 *      is seen with an unknown address.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      flags - (OUT) Logical OR of BCM_PORT_LEARN_xxx flags
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_learn_get(int unit, bcm_port_t port, uint32 *flags)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) 
    if (BCM_GPORT_IS_SUBPORT_GROUP(port)) {
        return bcm_tr_subport_learn_get(unit, (bcm_gport_t) port, flags);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        return bcm_tr_mpls_port_learn_get(unit, (bcm_gport_t) port, flags);
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3) 
    if (BCM_GPORT_IS_MIM_PORT(port)) {
        return bcm_tr2_mim_port_learn_get(unit, (bcm_gport_t) port, flags);
    }
    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        return bcm_tr2_wlan_port_learn_get(unit, (bcm_gport_t) port, flags);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    
    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    PORT_UNLOCK(unit);

    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        *flags = 0;
        if (!(pcfg.pc_cml & (1 << 0))) {
           *flags |= BCM_PORT_LEARN_FWD;
        }
        if (pcfg.pc_cml & (1 << 1)) {
           *flags |= BCM_PORT_LEARN_CPU;
        }
        if (pcfg.pc_cml & (1 << 2)) {
           *flags |= BCM_PORT_LEARN_PENDING;
        }
        if (pcfg.pc_cml & (1 << 3)) {
           *flags |= BCM_PORT_LEARN_ARL;
        }
    } else 
#endif
    {
        switch (pcfg.pc_cml) {
        case PVP_CML_SWITCH:
            *flags = (BCM_PORT_LEARN_ARL |
                      BCM_PORT_LEARN_FWD |
                      (pcfg.pc_cpu ? BCM_PORT_LEARN_CPU : 0));
            break;
        case PVP_CML_CPU:
            *flags = BCM_PORT_LEARN_CPU;
            break;
        case PVP_CML_FORWARD:
            *flags = BCM_PORT_LEARN_FWD;
            break;
        case PVP_CML_DROP:
            *flags = 0;
            break;
        case PVP_CML_CPU_SWITCH:            /* Possible on Draco only */
            *flags = (BCM_PORT_LEARN_ARL |
                      BCM_PORT_LEARN_CPU |
                      BCM_PORT_LEARN_FWD);
            break;
        case PVP_CML_CPU_FORWARD:           /* Possible on Draco only */
            *flags = BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD;
            break;
        default:
            return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_learn_set
 * Purpose:
 *      Set the ARL hardware learning options for this port.
 *      This defines what the hardware will do when a packet
 *      is seen with an unknown address.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      flags - Logical OR of BCM_PORT_LEARN_xxx flags
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

#define Arl     BCM_PORT_LEARN_ARL
#define Cpu     BCM_PORT_LEARN_CPU
#define Fwd     BCM_PORT_LEARN_FWD

int
bcm_esw_port_learn_set(int unit, bcm_port_t port, uint32 flags)
{
    bcm_port_cfg_t      pcfg;
    int                 rv;

#if defined(BCM_TRX_SUPPORT)  && defined(INCLUDE_L3)
    if (BCM_GPORT_IS_SUBPORT_GROUP(port)) {
        return bcm_tr_subport_learn_set(unit, (bcm_gport_t) port, flags);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        return bcm_tr_mpls_port_learn_set(unit, (bcm_gport_t) port, flags);
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3) 
    if (BCM_GPORT_IS_MIM_PORT(port)) {
        return bcm_tr2_mim_port_learn_set(unit, (bcm_gport_t) port, flags);
    }
    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        return bcm_tr2_wlan_port_learn_set(unit, (bcm_gport_t) port, flags);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (flags & BCM_PORT_LEARN_PENDING) {
        if (!soc_feature(unit, soc_feature_l2_pending)) {
            return BCM_E_UNAVAIL;
        } else if (!(flags & BCM_PORT_LEARN_ARL)) {
            /* When the PENDING flag is set, the ARL must also be set */
            return BCM_E_PARAM;
        }
    }

    PORT_LOCK(unit);

    rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);

    pcfg.pc_cpu = ((flags & BCM_PORT_LEARN_CPU) != 0);

    /* Use shortened names to handle each flag combination individually */

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        pcfg.pc_cml = 0;
        if (!(flags & BCM_PORT_LEARN_FWD)) {
           pcfg.pc_cml |= (1 << 0);
        }
        if (flags & BCM_PORT_LEARN_CPU) {
           pcfg.pc_cml |= (1 << 1);
        }
        if (flags & BCM_PORT_LEARN_PENDING) {
           pcfg.pc_cml |= (1 << 2);
        }
        if (flags & BCM_PORT_LEARN_ARL) {
           pcfg.pc_cml |= (1 << 3);
        }
    } else
#endif
    {
        switch (flags) {
        case ((!Arl) | (!Cpu) | (!Fwd)):
            pcfg.pc_cml = PVP_CML_DROP;
            break;
        case ((!Arl) | (!Cpu) | ( Fwd)):
            pcfg.pc_cml = PVP_CML_FORWARD;
            break;
        case ((!Arl) | ( Cpu) | (!Fwd)):
            pcfg.pc_cml = PVP_CML_CPU;
            break;
        case ((!Arl) | ( Cpu) | ( Fwd)):
            if (SOC_IS_XGS_SWITCH(unit)) {
                pcfg.pc_cml = PVP_CML_CPU_FORWARD;
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        case (( Arl) | (!Cpu) | (!Fwd)):
            rv = BCM_E_UNAVAIL;
            break;
        case (( Arl) | (!Cpu) | ( Fwd)):
            pcfg.pc_cml = PVP_CML_SWITCH;
            break;
        case (( Arl) | ( Cpu) | (!Fwd)):
            rv = BCM_E_UNAVAIL;
            break;
        case (( Arl) | ( Cpu) | ( Fwd)):
            if (SOC_IS_XGS_SWITCH(unit)) {
                pcfg.pc_cml = PVP_CML_CPU_SWITCH;
            } else {
                pcfg.pc_cml = PVP_CML_SWITCH;       /* pc_cpu also being set */
            }
            break;
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg);
    }

    PORT_UNLOCK(unit);

    return rv;
}

#undef Arl
#undef Cpu
#undef Fwd

/*
 * Function:
 *      bcm_port_ifilter_get
 * Description:
 *      Return input filter mode for a port.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number to operate on
 *      mode - (OUT) Filter mode, 1 is enable; 0 is disable. 
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 * Notes:
 *      This API is superseded by bcm_port_vlan_member_get.
 */

int
bcm_esw_port_ifilter_get(int unit, bcm_port_t port, int *mode)
{
    int rv;
    uint32 flags;

    rv = bcm_esw_port_vlan_member_get(unit, port, &flags);

    if (BCM_SUCCESS(rv)) {
        *mode = (flags & BCM_PORT_VLAN_MEMBER_INGRESS) ? 1 : 0;
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_ifilter_set
 * Description:
 *      Set input filter mode for a port.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number to operate on
 *      mode - 1 to enable, 0 to disable.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 * Notes:
 *      This API is superseded by bcm_port_vlan_member_set.
 *
 *      When input filtering is turned on for a port, packets received
 *      on the port that do not match the port's VLAN classifications
 *      are discarded.
 */

int
bcm_esw_port_ifilter_set(int unit, bcm_port_t port, int mode)
{
    uint32 flags;

    BCM_IF_ERROR_RETURN(bcm_esw_port_vlan_member_get(unit, port, &flags));

    if (mode) {
        flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
    }  else {
        flags &= (~BCM_PORT_VLAN_MEMBER_INGRESS);
    }

    return bcm_esw_port_vlan_member_set(unit, port, flags);
}

/*
 * Function:
 *      bcm_esw_port_vlan_member_get
 * Description:
 *      Return filter mode for a port.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number to operate on
 *      flags - (OUT) Filter mode, one of BCM_PORT_VLAN_MEMBER_xxx. 
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 */

int
bcm_esw_port_vlan_member_get(int unit, bcm_port_t port, uint32 *flags)
{
    int  rv = BCM_E_UNAVAIL;

    *flags = 0;

    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        rv = BCM_E_NONE;
    } else {
        rv = _bcm_esw_port_gport_validate(unit, port, &port);
    }
    BCM_IF_ERROR_RETURN(rv);
    rv = BCM_E_UNAVAIL;

    switch (BCM_CHIP_FAMILY(unit)) {

    case BCM_FAMILY_HERCULES:
        rv = BCM_E_NONE;
        break;

    case BCM_FAMILY_TUCANA:
    case BCM_FAMILY_DRACO15:
    case BCM_FAMILY_DRACO:
    case BCM_FAMILY_LYNX:

    case BCM_FAMILY_FIREBOLT:
    case BCM_FAMILY_EASYRIDER:
    case BCM_FAMILY_BRADLEY:
    case BCM_FAMILY_HUMV:
    case BCM_FAMILY_TRIUMPH:
    case BCM_FAMILY_SCORPION:
    case BCM_FAMILY_CONQUEROR:
    case BCM_FAMILY_TRIUMPH2:
        if (IS_HG_PORT(unit, port) && SOC_IS_XGS12_SWITCH(unit)) {
            rv = BCM_E_NONE;
        } else {
            port_tab_entry_t    pent;
            soc_mem_t mem;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_WLAN_PORT(port)) {
                int value;
                rv = _bcm_tr2_lport_field_get(unit, port, EN_IFILTERf, &value);
                if (value) {
                    *flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
                }
                return rv;
            }
#endif
            mem = SOC_PORT_MEM_TAB(unit, port);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                              SOC_PORT_MOD_OFFSET(unit, port), &pent);

            if (BCM_SUCCESS(rv) &&
                soc_PORT_TABm_field32_get(unit, &pent, EN_IFILTERf)) {
                    *flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
            }

            if (SOC_IS_XGS3_SWITCH(unit)) {
                uint32 egr_val;
                uint64 egr_val64;
                if (SOC_REG_IS_VALID(unit, EGR_PORTr)) {
                    rv = READ_EGR_PORTr(unit, port, &egr_val);
                } else {
                    rv = READ_EGR_PORT_64r(unit, port, &egr_val64);
                }
                if (BCM_SUCCESS(rv)) {
                    if (SOC_REG_IS_VALID(unit, EGR_PORTr)) {
                        if (soc_reg_field_get(unit, EGR_PORTr, egr_val, EN_EFILTERf)) {
                            *flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
                        }
                    } else if (soc_reg64_field32_get(unit, 
                                   EGR_PORT_64r, egr_val64, EN_EFILTERf)) {
                            *flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
                    }
                }
            }
        }
        break;

    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_port_vlan_member_set
 * Description:
 *      Set ingress and egress filter mode for a port.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number to operate on
 *      flags - BCM_PORT_VLAN_MEMBER_xxx.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 */

int
bcm_esw_port_vlan_member_set(int unit, bcm_port_t port, uint32 flags)
{
    int       rv = BCM_E_UNAVAIL;

    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        rv = BCM_E_NONE;
    } else {
        rv = _bcm_esw_port_gport_validate(unit, port, &port);
    }
    BCM_IF_ERROR_RETURN(rv);
    rv = BCM_E_UNAVAIL;

    PORT_LOCK(unit);

    switch (BCM_CHIP_FAMILY(unit)) {

    case BCM_FAMILY_HERCULES:
        rv = (flags & BCM_PORT_VLAN_MEMBER_INGRESS) ? BCM_E_UNAVAIL :
              ((flags & BCM_PORT_VLAN_MEMBER_EGRESS) ? BCM_E_UNAVAIL : BCM_E_NONE);
        break;

    case BCM_FAMILY_TUCANA:
    case BCM_FAMILY_DRACO15:
    case BCM_FAMILY_DRACO:
    case BCM_FAMILY_LYNX:

    case BCM_FAMILY_FIREBOLT:
    case BCM_FAMILY_EASYRIDER:
    case BCM_FAMILY_BRADLEY:
    case BCM_FAMILY_HUMV:
    case BCM_FAMILY_TRIUMPH:
    case BCM_FAMILY_SCORPION:
    case BCM_FAMILY_CONQUEROR:
    case BCM_FAMILY_TRIUMPH2:
        if (IS_HG_PORT(unit, port) && SOC_IS_XGS12_SWITCH(unit)) {
            rv = (flags & BCM_PORT_VLAN_MEMBER_INGRESS) ? BCM_E_UNAVAIL :
                  ((flags & BCM_PORT_VLAN_MEMBER_EGRESS) ? BCM_E_UNAVAIL : 
                  BCM_E_NONE);
        } else {
            port_tab_entry_t pent;
            soc_mem_t mem;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_WLAN_PORT(port)) {
                if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) {
                    rv = _bcm_tr2_wlan_port_set(unit, port, EN_IFILTERf, 1);
                } else {
                    rv = _bcm_tr2_wlan_port_set(unit, port, EN_IFILTERf, 0);
                }
                PORT_UNLOCK(unit);
                return rv;
            }
#endif
            mem = SOC_PORT_MEM_TAB(unit, port);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                              SOC_PORT_MOD_OFFSET(unit, port), &pent);

            if (BCM_SUCCESS(rv)) {
                soc_PORT_TABm_field32_set(unit, &pent, EN_IFILTERf,
                              (flags & BCM_PORT_VLAN_MEMBER_INGRESS) ? 1 : 0);
                rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &pent);
            }

            if (SOC_IS_XGS3_SWITCH(unit)) {
                soc_reg_t egr_port_reg;
                egr_port_reg = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                                SOC_IS_VALKYRIE2(unit)) ? EGR_PORT_64r : EGR_PORTr;
                rv = soc_reg_field32_modify(unit, egr_port_reg, port, EN_EFILTERf, 
                                      (flags & BCM_PORT_VLAN_MEMBER_EGRESS) ? 1 : 0);
                if (BCM_SUCCESS(rv)) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
        defined(BCM_SCORPION_SUPPORT)
                    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                        SOC_IS_SC_CQ(unit) || SOC_IS_HAWKEYE(unit)) {
                        uint32 egr_val;
                        pbmp_t pbmp;
                        uint32 bitmap, pbmp32;

                        rv = READ_ING_EN_EFILTER_BITMAPr(unit, &egr_val);
                        if (BCM_SUCCESS(rv)) {
                            SOC_PBMP_PORT_SET(pbmp, port);
                            pbmp32 = SOC_PBMP_WORD_GET(pbmp, 0);
                            bitmap =
                                soc_reg_field_get(unit,
                                                  ING_EN_EFILTER_BITMAPr,
                                                  egr_val, BITMAPf);
                            if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
                                bitmap |= pbmp32;
                            } else {
                                bitmap &= ~pbmp32;
                            }
                            soc_reg_field_set(unit, ING_EN_EFILTER_BITMAPr,
                                              &egr_val, BITMAPf, bitmap);
                            rv = WRITE_ING_EN_EFILTER_BITMAPr(unit, egr_val);
                        }
                    }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
                    /* Do this only if there is no VP on this port */
                    if (SOC_IS_TR_VL(unit)) {
                        uint64 egr_val_64;
                        pbmp_t pbmp;
                        uint32 bitmap, pbmp32;

                        rv = READ_ING_EN_EFILTER_BITMAP_64r(unit, &egr_val_64);
                        if (BCM_SUCCESS(rv)) {
                            SOC_PBMP_PORT_SET(pbmp, port);
                            /* Low bitmap */
                            bitmap =
                                soc_reg64_field32_get(unit,
                                                      ING_EN_EFILTER_BITMAP_64r,
                                                      egr_val_64, BITMAP_LOf);
                            pbmp32 = SOC_PBMP_WORD_GET(pbmp, 0);
                            if ((flags & BCM_PORT_VLAN_MEMBER_EGRESS)  && 
                                (PORT(unit, port).vp_count == 0)) {
                                bitmap |= pbmp32;
                            } else {
                                bitmap &= ~pbmp32;
                            }
                            soc_reg64_field32_set(unit, ING_EN_EFILTER_BITMAP_64r,
                                                  &egr_val_64, BITMAP_LOf, bitmap);

                            if(!SOC_IS_ENDURO(unit)) {
                                    /* High bitmap */
                                    bitmap =
                                        soc_reg64_field32_get(unit,
                                                              ING_EN_EFILTER_BITMAP_64r,
                                                              egr_val_64, BITMAP_HIf);
                                    pbmp32 = SOC_PBMP_WORD_GET(pbmp, 1);
                                    if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
                                        bitmap |= pbmp32;
                                    } else {
                                        bitmap &= ~pbmp32;
                                    }
                                    soc_reg64_field32_set(unit, ING_EN_EFILTER_BITMAP_64r,
                                                          &egr_val_64, BITMAP_HIf, bitmap);
                            }

                            rv = WRITE_ING_EN_EFILTER_BITMAP_64r(unit, egr_val_64);
                        }
                    }
#endif
                }
            }
        }
        break;

    default:
        break;
    }

    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_port_link_get
 * Purpose:
 *      Return current PHY up/down status
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      hw - If TRUE, assume hardware linkscan is active and use it
 *              to reduce PHY reads.
 *           If FALSE, do not use information from hardware linkscan.
 *      up - (OUT) TRUE for link up, FALSE for link down.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
_bcm_port_link_get(int unit, bcm_port_t port, int hw, int *up)
{
    int     rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    if (hw) {
        pbmp_t hw_linkstat;

        rv = soc_linkscan_hw_link_get(unit, &hw_linkstat);

        *up = PBMP_MEMBER(hw_linkstat, port);

        /*
         * We need to confirm link down because we may receive false link
         * change interrupts when hardware and software linkscan are mixed.
         * Processing a false link down event is known to cause packet
         * loss, which is obviously unacceptable.
         */
        if(!(*up)) {
            rv = soc_phyctrl_link_get(unit, port, up);
        }
    } else {
        rv = soc_phyctrl_link_get(unit, port, up);
    }

    PORT_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_MEDIUM_CHANGE)) {
            soc_port_medium_t  medium;
            soc_phyctrl_medium_get(unit, port, &medium);
            soc_phy_medium_status_notify(unit, port, medium);
        }
    }
    SOC_DEBUG_PRINT((DK_PORT | DK_VERBOSE | DK_LINK,
                     "_bcm_port_link_get: u=%d p=%d hw=%d up=%d rv=%d\n",
                     unit, port, hw, *up, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_link_status_get
 * Purpose:
 *      Return current Link up/down status, queries linkscan, if unable to
 *      retrieve status queries the PHY.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      up - (OUT) Boolean value, FALSE for link down and TRUE for link up
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_link_status_get(int unit, bcm_port_t port, int *up)
{
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = _bcm_esw_link_get(unit, port, up);

    if (rv == BCM_E_DISABLED) {
        int mode;
        
        BCM_IF_ERROR_RETURN(bcm_esw_linkscan_mode_get(unit, port, &mode));

        if (mode == BCM_LINKSCAN_MODE_HW) {
            rv = _bcm_port_link_get(unit, port, 1, up);
        } else {
            rv = _bcm_port_link_get(unit, port, 0, up);
        }
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_linkstatus_get: u=%d p=%d up=%d rv=%d\n",
                     unit, port, *up, rv));

    return rv;
}

/*
 * Function:
 *      bcm_esw_port_link_failed_clear
 * Purpose:
 *      Clear failed link status from a port which has undergone
 *      LAG failover.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 * Notes:
 *      The port is moved to down status.  The application is responsible
 *      for removing the port from all trunk memberships before calling this
 *      function.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_link_failed_clear(int unit, bcm_port_t port)
{
    return _bcm_esw_link_failed_clear(unit, port);
}

/*
 * Function:
 *      _bcm_port_pfm_set
 * Purpose:
 *      Main part of function bcm_port_pfm_set
 */

STATIC int
_bcm_port_pfm_set(int unit, bcm_port_t port, int mode)
{
    int                 rv = BCM_E_NONE;

    /*
     * The filter mode for the first 16 ports is in one register
     * and for the rest in another
     */
    switch (BCM_CHIP_FAMILY(unit)) {

#ifdef BCM_XGS12_FABRIC_SUPPORT
    case BCM_FAMILY_HERCULES:
        /* See remarks under bcm_port_pfm_get below */
        break;
#endif

#ifdef BCM_XGS3_SWITCH_SUPPORT
    case BCM_FAMILY_FIREBOLT:
    case BCM_FAMILY_EASYRIDER:
    case BCM_FAMILY_BRADLEY:
    case BCM_FAMILY_HUMV:
    case BCM_FAMILY_TRIUMPH:
    case BCM_FAMILY_SCORPION:
    case BCM_FAMILY_CONQUEROR:
    case BCM_FAMILY_TRIUMPH2:
        /*
         * PFM is a per VLAN attribute. It is not a per port attribute
         * on XGS3
         */
        return BCM_E_UNAVAIL;
#endif

#ifdef  BCM_XGS12_SWITCH_SUPPORT
    case BCM_FAMILY_TUCANA:
    case BCM_FAMILY_DRACO15:
    case BCM_FAMILY_DRACO:
    case BCM_FAMILY_LYNX:
        if (IS_E_PORT(unit,port)) {
            soc_mem_t mem = SOC_PORT_MEM_TAB(unit, port);
            port_tab_entry_t    pent;

            sal_memset(&pent, 0, sizeof(pent));
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                             SOC_PORT_MOD_OFFSET(unit, port), &pent));
            soc_mem_field32_force(unit, mem, &pent, PFMf, (uint32)mode);
            soc_PORT_TABm_field32_set(unit, &pent, PFMf, mode);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                              SOC_PORT_MOD_OFFSET(unit, port), &pent));
        } else if (IS_HG_PORT(unit, port)) {
            /* For now, ignore on IPIC.  Pending final decision */
        } else {
            rv = BCM_E_PORT;
        }
        break;
#endif /* BCM_XGS12_SWITCH_SUPPORT */

    default:
        return BCM_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_pfm_set
 * Purpose:
 *      Set current port filtering mode (see port.h)
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      mode - mode for PFM bits (see port.h)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_pfm_set(int unit, bcm_port_t port, int mode)
{
    int                 rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (!IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    PORT_LOCK(unit);
    rv = _bcm_port_pfm_set(unit, port, mode);
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_pfm_get
 * Purpose:
 *      Return current port filtering mode (see port.h)
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) mode read from StrataSwitch for PFM bits (see port.h)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This function not supported on 5670.
 */

int
bcm_esw_port_pfm_get(int unit, bcm_port_t port, int *mode)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (!IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    switch (BCM_CHIP_FAMILY(unit)) {

#ifdef BCM_HERCULES_SUPPORT
    case BCM_FAMILY_HERCULES:
        /*
         * Packets marked as multicast prompt a lookup in the
         * MC table in 567x.  There is no notion of missing a multicast
         * address.
         *
         * Alternatively, we can mess with the MC/IPMC table
         * entries to support the different modes.  This is
         * probably not worth the trouble when we look at how
         * 567x is meant to fit into the system.
         */
        *mode = BCM_PORT_PFM_MODEC;
        break;
#endif

#ifdef BCM_XGS3_SWITCH_SUPPORT
    case BCM_FAMILY_FIREBOLT:
    case BCM_FAMILY_EASYRIDER:
    case BCM_FAMILY_BRADLEY:
    case BCM_FAMILY_HUMV:
    case BCM_FAMILY_TRIUMPH:
    case BCM_FAMILY_SCORPION:
    case BCM_FAMILY_CONQUEROR:
    case BCM_FAMILY_TRIUMPH2:
        /*
         * PFM is a per VLAN attribute. It is not a per port attribute
         * on XGS3
         */
        *mode = BCM_PORT_PFM_MODEC;
        return BCM_E_UNAVAIL;
#endif

#ifdef BCM_XGS_SWITCH_SUPPORT
    case BCM_FAMILY_TUCANA:
    case BCM_FAMILY_DRACO15:
    case BCM_FAMILY_DRACO:
    case BCM_FAMILY_LYNX:
        if (IS_E_PORT(unit, port)) {
            port_tab_entry_t    pent;
            soc_mem_t mem = SOC_PORT_MEM_TAB(unit, port);

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                    SOC_PORT_MOD_OFFSET(unit, port),  &pent));
            *mode = soc_PORT_TABm_field32_get(unit, &pent, PFMf);
        } else if (IS_HG_PORT(unit, port)) {
            *mode = BCM_PORT_PFM_MODEC;  /* Pending final decision */
        } else {
            return BCM_E_PORT;
        }
        break;
#endif

    default:
        return BCM_E_UNAVAIL;
        break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_untagged_priority_set
 * Purpose:
 *      Main part of bcm_port_untagged_priority_set.
 */

STATIC int
_bcm_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
#ifdef BCM_FILTER_SUPPORT
    bcm_filterid_t      f;
    int                 rv;
    bcm_vlan_t          vid;
    pbmp_t              pbmp;
#endif

    if (priority > 7) {
        return BCM_E_PARAM;
    }

#if defined(BCM_XGS_SWITCH_SUPPORT)
    if (soc_feature(unit, soc_feature_remap_ut_prio) && (priority >= 0)) {
        bcm_port_cfg_t pcfg;
        SOC_IF_ERROR_RETURN(
            mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
            pcfg.pc_new_pri = priority;
        SOC_IF_ERROR_RETURN(
            mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));
        PORT(unit, port).p_ut_prio = pcfg.pc_new_pri;
        return SOC_E_NONE;
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

    if (priority < 0) {
        return BCM_E_PARAM;
    }

#ifdef BCM_FILTER_SUPPORT
    /*
     * Remap priority by using a filter
     */

    BCM_IF_ERROR_RETURN(bcm_esw_port_untagged_vlan_get(unit, port, &vid));

    /*
     * If a filter currently exists, remove it.
     */

    if (PORT(unit, port).p_ut_filter != 0) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_filter_remove(unit, PORT(unit, port).p_ut_filter));

        BCM_IF_ERROR_RETURN
            (bcm_esw_filter_destroy(unit, PORT(unit, port).p_ut_filter));

        PORT(unit, port).p_ut_filter = 0;
    }

    PORT(unit, port).p_ut_prio = priority;

    if (priority == 0) {
        return BCM_E_NONE;      /* No filter needed because default is 0 */
    }

    /*
     * Create filter for Insert 802.1p Priority matching untagged
     * packets with the correct VLAN ID.
     */

    BCM_IF_ERROR_RETURN(bcm_esw_filter_create(unit, &f));

    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_PORT_ADD(pbmp, port);
    (void) bcm_esw_filter_qualify_ingress(unit, f, pbmp);
    (void) bcm_esw_filter_qualify_data16(unit, f, 12, 0x8100, 0xffff);
    (void) bcm_esw_filter_qualify_data16(unit, f, 14, vid, 0x0fff);
    (void) bcm_esw_filter_action_match(unit, f, bcmActionInsPrio, priority);

    rv = bcm_esw_filter_install(unit, f);

    if (rv < 0) {
        (void) bcm_esw_filter_destroy(unit, f);
    } else {
        PORT(unit, port).p_ut_filter = f;
    }

    return rv;
#else /* !BCM_FILTER_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* !BCM_FILTER_SUPPORT */
}

/*
 * Function:
 *      bcm_port_untagged_priority_set
 * Purpose:
 *      Set the 802.1P priority for untagged packets coming in on a
 *      port.  This value will be written into the priority field of the
 *      tag that is added at the ingress.
 * Parameters:
 *      unit      - StrataSwitch Unit #.
 *      port      - StrataSwitch port #.
 *      priority  - Priority to be set in 802.1p priority tag, from 0 to 7.
 *                  A negative priority leaves the ingress port priority as
 *                  is, but disables it from overriding ARL-based priorities.
 *                  (on those devices that support ARL-based priority).
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
    int         rv;

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        return bcm_tr2_wlan_port_untagged_prio_set(unit, port, priority);
    }
#endif
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = _bcm_port_untagged_priority_set(unit, port, priority);
    PORT_UNLOCK(unit);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ut_priority_set: u=%d p=%d pri=%d rv=%d\n",
                     unit, port, priority, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_untagged_priority_get
 * Purpose:
 *      Returns priority being assigned to untagged receive packets
 * Parameters:
 *      unit      - StrataSwitch Unit #.
 *      port      - StrataSwitch port #.
 *      priority  - Pointer to an int in which priority value is returned.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_port_untagged_priority_get(int unit, bcm_port_t port, int *priority)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        return bcm_tr2_wlan_port_untagged_prio_get(unit, port, priority);
    }
#endif
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (priority != NULL) {
        *priority = PORT(unit, port).p_ut_prio;
	SOC_DEBUG_PRINT((DK_PORT,
        	"bcm_port_ut_priority_get: u=%d p=%d pri=%d\n",
                unit, port, *priority));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_selective_get
 * Purpose:
 *      Get requested port parameters
 * Parameters:
 *      unit - switch Unit
 *      port - switch port
 *      info - (OUT) port information structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The action_mask field of the info argument is used as an input
 */

int
bcm_esw_port_selective_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int                 r;
    uint32              mask;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_ENCAP_MASK) {
        r = bcm_esw_port_encap_get(unit, port, &info->encap_mode);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_encap_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        r = bcm_esw_port_enable_get(unit, port, &info->enable);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_enable_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LINKSTAT_MASK) {
        r = bcm_esw_port_link_status_get(unit, port, &info->linkstatus);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_link_status_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK) {
        r = bcm_esw_port_autoneg_get(unit, port, &info->autoneg);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_autoneg_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LOCAL_ADVERT_MASK) {
        r = bcm_esw_port_ability_advert_get(unit, port,
                                            &info->local_ability);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_ability_advert_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
        r = soc_port_ability_to_mode(&info->local_ability,
                                     &info->local_advert);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_REMOTE_ADVERT_MASK) {

        if ((r = bcm_esw_port_ability_remote_get(unit, port,
                                                 &info->remote_ability)) < 0) {
            info->remote_advert = 0;
            info->remote_advert_valid = FALSE;
        } else {
            r = soc_port_ability_to_mode(&info->remote_ability,
                                         &info->remote_advert);
            BCM_IF_ERROR_RETURN(r);
            info->remote_advert_valid = TRUE;
        }
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        if ((r = bcm_esw_port_speed_get(unit, port, &info->speed)) < 0) {
            if (r != BCM_E_BUSY) {
                SOC_DEBUG_PRINT((DK_VERBOSE,
                                 "bcm_port_speed_get failed: %s\n", bcm_errmsg(r)));
                return(r);
            } else {
                info->speed = 0;
            }
        }
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK) {
        if ((r = bcm_esw_port_duplex_get(unit, port, &info->duplex)) < 0) {
            if (r != BCM_E_BUSY) {
                SOC_DEBUG_PRINT((DK_VERBOSE,
                                 "bcm_port_duplex_get failed: %s\n", bcm_errmsg(r)));
                return r;
            } else {
                info->duplex = 0;
            }
        }
    }

    /* get both if either mask bit set */
    if (mask & (BCM_PORT_ATTR_PAUSE_TX_MASK |
                BCM_PORT_ATTR_PAUSE_RX_MASK)) {
        r = bcm_esw_port_pause_get(unit, port,
                                   &info->pause_tx, &info->pause_rx);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_pause_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
        r = bcm_esw_port_pause_addr_get(unit, port, info->pause_mac);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_pause_addr_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        r = bcm_esw_port_linkscan_get(unit, port, &info->linkscan);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_linkscan_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LEARN_MASK) {
        r = bcm_esw_port_learn_get(unit, port, &info->learn);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_learn_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_DISCARD_MASK) {
        r = bcm_esw_port_discard_get(unit, port, &info->discard);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_discard_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_VLANFILTER_MASK) {
        r = bcm_esw_port_vlan_member_get(unit, port, &info->vlanfilter);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_esw_port_vlan_member_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
        r = bcm_esw_port_untagged_priority_get(unit, port,
                                               &info->untagged_priority);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_untagged_priority_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK) {
        r = bcm_esw_port_untagged_vlan_get(unit, port,
                                           &info->untagged_vlan);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_untagged_vlan_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_STP_STATE_MASK) {
        r = bcm_esw_port_stp_get(unit, port, &info->stp_state);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_stp_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PFM_MASK) {
        r = bcm_esw_port_pfm_get(unit, port, &info->pfm);
        if (r != BCM_E_UNAVAIL) {
            SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                         "bcm_port_pfm_get failed: %s\n", bcm_errmsg(r)));
        }
        BCM_IF_ERROR_NOT_UNAVAIL_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        r = bcm_esw_port_loopback_get(unit, port, &info->loopback);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_loopback_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PHY_MASTER_MASK) {
        r = bcm_esw_port_master_get(unit, port, &info->phy_master);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_master_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK) {
        r = bcm_esw_port_interface_get(unit, port, &info->interface);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_interface_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_RATE_MCAST_MASK) {
        r = bcm_esw_rate_mcast_get(unit, &info->mcast_limit,
                                   &info->mcast_limit_enable, port);
        if (r == BCM_E_UNAVAIL) {
            r = BCM_E_NONE;     /* Ignore if not supported on chip */
        }
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_rate_mcast_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_RATE_BCAST_MASK) {
        r = bcm_esw_rate_bcast_get(unit, &info->bcast_limit,
                                   &info->bcast_limit_enable, port);
        if (r == BCM_E_UNAVAIL) {
            r = BCM_E_NONE;     /* Ignore if not supported on chip */
        }
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_rate_bcast_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_RATE_DLFBC_MASK) {
        r = bcm_esw_rate_dlfbc_get(unit, &info->dlfbc_limit,
                                   &info->dlfbc_limit_enable, port);
        if (r == BCM_E_UNAVAIL) {
            r = BCM_E_NONE;     /* Ignore if not supported on chip */
        }
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_rate_dlfbc_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_SPEED_MAX_MASK) {
        r = bcm_esw_port_speed_max(unit, port, &info->speed_max);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_speed_max failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_ABILITY_MASK) {
        r = bcm_esw_port_ability_local_get(unit, port, &info->port_ability);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_ability_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
        r = soc_port_ability_to_mode(&info->port_ability,
                                     &info->ability);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        r = bcm_esw_port_frame_max_get(unit, port, &info->frame_max);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_frame_max_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_MDIX_MASK) {
        r = bcm_esw_port_mdix_get(unit, port, &info->mdix);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_mdix_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_MDIX_STATUS_MASK) {
        r = bcm_esw_port_mdix_status_get(unit, port, &info->mdix_status);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_mdix_status_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_MEDIUM_MASK) {
        r = bcm_esw_port_medium_get(unit, port, &info->medium);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_medium_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_FAULT_MASK) {
        r = bcm_esw_port_fault_get(unit, port, &info->fault);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_fault_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_selective_set
 * Purpose:
 *      Set requested port parameters
 * Parameters:
 *      unit - switch unit
 *      port - switch port
 *      info - port information structure
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Does not set spanning tree state.
 */

int
bcm_esw_port_selective_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int                 r;
    uint32              mask;
    int                 flags = 0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_ENCAP_MASK) {
        r = bcm_esw_port_encap_set(unit, port, info->encap_mode);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_encap_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        r = bcm_esw_port_enable_set(unit, port, info->enable);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_enable_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
        r = bcm_esw_port_pause_addr_set(unit, port, info->pause_mac);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_pause_addr_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK) {
        r = bcm_esw_port_interface_set(unit, port, info->interface);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_interface_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PHY_MASTER_MASK) {
        r = bcm_esw_port_master_set(unit, port, info->phy_master);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_master_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        r = bcm_esw_port_linkscan_set(unit, port, info->linkscan);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_linkscan_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LEARN_MASK) {
        r = bcm_esw_port_learn_set(unit, port, info->learn);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_learn_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_DISCARD_MASK) {
        r = bcm_esw_port_discard_set(unit, port, info->discard);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_discard_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_VLANFILTER_MASK) {
        r = bcm_esw_port_vlan_member_set(unit, port, info->vlanfilter);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_esw_port_vlan_member_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
        r = bcm_esw_port_untagged_priority_set(unit, port,
                                               info->untagged_priority);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_untagged_priority_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK) {
        r = bcm_esw_port_untagged_vlan_set(unit, port, info->untagged_vlan);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_untagged_vlan_set (%d) failed: %s\n",
                                     info->untagged_vlan, bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PFM_MASK) {
        r = bcm_esw_port_pfm_set(unit, port, info->pfm);
        if (r != BCM_E_UNAVAIL) {
            SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                         "bcm_port_pfm_set failed: %s\n", bcm_errmsg(r)));
        }
        BCM_IF_ERROR_NOT_UNAVAIL_RETURN(r);
    }

    /*
     * Set loopback mode before setting the speed/duplex, since it may
     * affect the allowable values for speed/duplex.
     */

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        r = bcm_esw_port_loopback_set(unit, port, info->loopback);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_loopback_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LOCAL_ADVERT_MASK) {
        if (info->action_mask2 & BCM_PORT_ATTR2_PORT_ABILITY) {
            r = bcm_esw_port_ability_advert_set(unit, port,
                                                &(info->local_ability));
            SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                         "bcm_port_ability_advert_set failed: %s\n",
                                         bcm_errmsg(r)));
            BCM_IF_ERROR_RETURN(r);
        } else {
            r = bcm_esw_port_advert_set(unit, port, info->local_advert);
            SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                         "bcm_port_advert_set failed: (0x%x): %s\n",
                                         info->local_advert, bcm_errmsg(r)));
            BCM_IF_ERROR_RETURN(r);
        }
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK) {
        r = bcm_esw_port_autoneg_set(unit, port, info->autoneg);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_autoneg_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        r = bcm_esw_port_speed_set(unit, port, info->speed);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_speed_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK) {
        r = bcm_esw_port_duplex_set(unit, port, info->duplex);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_duplex_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & (BCM_PORT_ATTR_PAUSE_TX_MASK |
                BCM_PORT_ATTR_PAUSE_RX_MASK)) {
        int     tpause, rpause;

        tpause = rpause = -1;
        if (mask & BCM_PORT_ATTR_PAUSE_TX_MASK) {
            tpause = info->pause_tx;
        }
        if (mask & BCM_PORT_ATTR_PAUSE_RX_MASK) {
            rpause = info->pause_rx;
        }
        r = bcm_esw_port_pause_set(unit, port, tpause, rpause);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_pause_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_RATE_MCAST_MASK) {
        flags = (info->mcast_limit_enable) ? BCM_RATE_MCAST : 0;
        r = bcm_esw_rate_mcast_set(unit, info->mcast_limit, flags, port);
        if (r == BCM_E_UNAVAIL) {
            r = BCM_E_NONE;     /* Ignore if not supported on chip */
        }
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_rate_mcast_port_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_RATE_BCAST_MASK) {
        flags = (info->bcast_limit_enable) ? BCM_RATE_BCAST : 0;
        r = bcm_esw_rate_bcast_set(unit, info->bcast_limit, flags, port);
        if (r == BCM_E_UNAVAIL) {
            r = BCM_E_NONE;     /* Ignore if not supported on chip */
        }
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_rate_bcast_port_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_RATE_DLFBC_MASK) {
        flags = (info->dlfbc_limit_enable) ? BCM_RATE_DLF : 0;
        r = bcm_esw_rate_dlfbc_set(unit, info->dlfbc_limit, flags, port);
        if (r == BCM_E_UNAVAIL) {
            r = BCM_E_NONE;     /* Ignore if not supported on chip */
        }
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_rate_dlfbcast_port_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_STP_STATE_MASK) {
        r = bcm_esw_port_stp_set(unit, port, info->stp_state);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_stp_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        r = bcm_esw_port_frame_max_set(unit, port, info->frame_max);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_frame_max_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_MDIX_MASK) {
        r = bcm_esw_port_mdix_set(unit, port, info->mdix);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                                     "bcm_port_mdix_set failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_info_get
 * Purpose:
 *      Get all information on the port
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_info_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    bcm_port_info_t_init(info);

    if (IS_ST_PORT(unit, port)) {
        info->action_mask = BCM_PORT_HERC_ATTRS;
        if (SOC_IS_XGS_SWITCH(unit)) {
            info->action_mask |= BCM_PORT_ATTR_STP_STATE_MASK |
                                 BCM_PORT_ATTR_DISCARD_MASK |
                                 BCM_PORT_ATTR_LEARN_MASK;
        }
    } else {
        info->action_mask = BCM_PORT_ATTR_ALL_MASK;
    }

    return bcm_esw_port_selective_get(unit, port, info);
}

/*
 * Function:
 *      bcm_port_info_set
 * Purpose:
 *      Set all information on the port
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Checks if AN is on, and if so, clears the
 *      proper bits in the action mask.
 */

int
bcm_esw_port_info_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (IS_ST_PORT(unit, port)) {
        info->action_mask = BCM_PORT_HERC_ATTRS;
        if (SOC_IS_XGS_SWITCH(unit)) {
            info->action_mask |= BCM_PORT_ATTR_STP_STATE_MASK |
                                 BCM_PORT_ATTR_DISCARD_MASK |
                                 BCM_PORT_ATTR_LEARN_MASK;
        }
    } else {
        info->action_mask = BCM_PORT_ATTR_ALL_MASK;
    }

    /* If autoneg is set, remove those attributes controlled by it */
    if (info->autoneg) {
        info->action_mask &= ~BCM_PORT_AN_ATTRS;
    }

    return bcm_esw_port_selective_set(unit, port, info);
}

/*
 * Function:
 *      bcm_port_info_save
 * Purpose:
 *      Save the current settings of a port
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The action_mask will be adjusted so that the
 *      proper values will be set when a restore is made.
 *      This mask should not be altered between these calls.
 */

int
bcm_esw_port_info_save(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (IS_ST_PORT(unit, port)) {
        info->action_mask = BCM_PORT_HERC_ATTRS;
        if (SOC_IS_XGS_SWITCH(unit)) {
            info->action_mask |= BCM_PORT_ATTR_STP_STATE_MASK;
        }
#ifdef BCM_GXPORT_SUPPORT
        if (IS_GX_PORT(unit, port)) {
            info->action_mask |= BCM_PORT_ATTR_AUTONEG_MASK;
        }
#endif
    } else {
        info->action_mask = BCM_PORT_ATTR_ALL_MASK;
    }
    info->action_mask2 = BCM_PORT_ATTR2_PORT_ABILITY;

    BCM_IF_ERROR_RETURN(bcm_esw_port_selective_get(unit, port, info));

    if (info->autoneg) {
        info->action_mask &= ~BCM_PORT_AN_ATTRS;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_info_restore
 * Purpose:
 *      Restore port settings saved by info_save
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      info - Pointer to structure with info from port_info_save
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      bcm_port_info_save has done all the work.
 *      We just call port_selective_set.
 */

int
bcm_esw_port_info_restore(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    return bcm_esw_port_selective_set(unit, port, info);
}

/*
 * Function:
 *      bcm_port_phy_drv_name_get
 * Purpose:
 *      Return the name of the PHY driver being used on a port.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 * Returns:
 *      Pointer to static string
 */
int
bcm_esw_port_phy_drv_name_get(int unit, bcm_port_t port, char *name, int len)
{
    int str_len;

    if (bcm_port_info[unit] == NULL) {
        str_len = sal_strlen("driver not initialized");
        if (str_len <= len) {
            sal_strcpy(name, "driver not initialized");
        }
        return BCM_E_INIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        str_len = sal_strlen("invalid port");
        if (str_len <= len) {
            sal_strcpy(name, "invalid port");
        }
        return BCM_E_PORT;
    }
    return (soc_phyctrl_drv_name_get(unit, port, name, len)); 
}

/*
 * Function:
 *      _bcm_port_encap_xport_set
 * Purpose:
 *      Convert 10G Ether port to Higig port, or reverse
 */

STATIC int
_bcm_port_encap_xport_set(int unit, bcm_port_t port, int mode)
{
#ifdef BCM_FIREBOLT_SUPPORT
    port_tab_entry_t ptab;
    bcm_stg_t        stg;
    int              to_higig;
    soc_field_t      port_type;
    soc_reg_t        egr_port_reg;
    soc_port_ability_t ability;
    soc_port_mode_t  non_ieee_speed;
    int              an, an_done, force_speed;

    to_higig = (mode != BCM_PORT_ENCAP_IEEE);
    port_type = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                 SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) ? 
                 PORT_TYPEf : HIGIG_PACKETf;
    egr_port_reg = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                    SOC_IS_VALKYRIE2(unit)) ? EGR_PORT_64r : EGR_PORTr;

    SOC_IF_ERROR_RETURN
        (MAC_ENCAP_SET(PORT(unit, port).p_mac, unit, port, mode));

    if (!to_higig) {
        SOC_IF_ERROR_RETURN
            (MAC_INTERFACE_SET(PORT(unit, port).p_mac,
                               unit, port, SOC_PORT_IF_XGMII));
    }

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_ability_local_get(unit, port, &ability));
    non_ieee_speed = ability.speed_full_duplex &
        ~(SOC_PA_SPEED_10GB | SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB |
          SOC_PA_SPEED_10MB);

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_ability_advert_get(unit, port, &ability));
    if (to_higig) {
        ability.speed_full_duplex |= non_ieee_speed;
        force_speed = SOC_CONTROL(unit)->info.port_speed_max[port];
    } else {
        ability.speed_full_duplex &= ~non_ieee_speed;
        force_speed = 10000;
    }
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_ability_advert_set(unit, port, &ability));

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_auto_negotiate_get(unit, port, &an, &an_done));
    /* Some mac driver re-init phy while executing MAC_ENCAP_SET, in that case
     * autoneg is probably always true here */
    if (an) {
        SOC_IF_ERROR_RETURN(bcm_esw_port_autoneg_set(unit, port, TRUE));
    } else {
        SOC_IF_ERROR_RETURN(soc_phyctrl_speed_set(unit, port, force_speed));
    }

    /* Now we propagate the changes */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                     MEM_BLOCK_ANY, port, &ptab));
    if (to_higig) {
        soc_PORT_TABm_field32_set(unit, &ptab, port_type, 1);
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, egr_port_reg, port, port_type, 1));
    } else {
        soc_PORT_TABm_field32_set(unit, &ptab, port_type, 0);
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, egr_port_reg, port, port_type, 0));
    }

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm,
                                      MEM_BLOCK_ALL, port, &ptab));

    /*
     * FIXME: All of the following items are candidates for
     * incorporation into the stack module.  Presently, they
     * adjust the results so rc.soc matches whether the port is
     * marked in config, or dynamically changed.
     */
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        uint64 cpu_pbm64;
        uint64 rval64;      /* Current 64 bit register data.  */
        COMPILER_64_ZERO(cpu_pbm64);
        COMPILER_64_SET(cpu_pbm64, 0, to_higig ? 1 : 0);
        SOC_IF_ERROR_RETURN(READ_ICONTROL_OPCODE_BITMAP_64r(unit, port,
                                                            &rval64));
        soc_reg64_field_set(unit, ICONTROL_OPCODE_BITMAP_64r, &rval64,
                            BITMAPf, cpu_pbm64);
        SOC_IF_ERROR_RETURN(WRITE_ICONTROL_OPCODE_BITMAP_64r(unit, port,
                                                             rval64));
    } else {

        /* Set HG ingress CPU Opcode map to the CPU */
        int pbm_len;
        uint32 cpu_pbm = 0;

        if (to_higig) {
            if (SOC_IS_TR_VL(unit)) {
                soc_xgs3_port_to_higig_bitmap(unit, CMIC_PORT(unit),
                                              &cpu_pbm);
            } else if (CMIC_PORT(unit)) {
                pbm_len = soc_reg_field_length(unit, ICONTROL_OPCODE_BITMAPr,
                                               BITMAPf);
                cpu_pbm = 1 << (pbm_len - 1);
            } else {
                cpu_pbm = 1;
            }
        } /* else, cpu_pbm = 0 */

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ICONTROL_OPCODE_BITMAPr,
                                    port, BITMAPf, cpu_pbm));
    }

    if (to_higig) {
        /* HG ports to forwarding */
        BCM_IF_ERROR_RETURN(bcm_esw_port_stp_set(unit, port,
                                                 BCM_STG_STP_FORWARD));
    }  

    /* Clear mirror enable settings */
    BCM_IF_ERROR_RETURN(bcm_esw_mirror_port_set(unit, port, -1, -1, 0));

    /* Set untagged state in default VLAN properly */
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_untag_update(unit, port, to_higig));

    /* Resolve STG 0 */
    BCM_IF_ERROR_RETURN(bcm_esw_stg_default_get(unit, &stg));
    BCM_IF_ERROR_RETURN
        (bcm_esw_stg_stp_set(unit, 0, port,
                             to_higig ? BCM_STG_STP_FORWARD : BCM_STG_STP_DISABLE));

#ifdef BCM_TRX_SUPPORT
    /* Reset the vlan default action */
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        bcm_vlan_action_set_t action;

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_egress_default_action_get(unit,
                                                         port, &action));
        /* Backward compatible defaults */
        if (to_higig) {
            action.ot_outer = bcmVlanActionDelete;
            action.dt_outer = bcmVlanActionDelete;
        } else {
            action.ot_outer = bcmVlanActionNone;
            action.dt_outer = bcmVlanActionNone;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_egress_default_action_set(unit,
                                                         port, &action));
    }
#endif

#ifdef INCLUDE_L3
    BCM_IF_ERROR_RETURN
        (bcm_esw_ipmc_egress_port_set(unit, port, to_higig ? _soc_mac_all_ones :
                                      _soc_mac_all_zeroes, 0, 0, 0));
#endif /* INCLUDE_L3 */

    return BCM_E_NONE;
#else
    return BCM_E_PARAM;
#endif
}

/*
 * Function:
 *      _bcm_port_encap_stport_set
 * Purpose:
 *      Convert 2.5G Higig 2 port to Ether port, or reverse
 */
STATIC int
_bcm_port_encap_stport_set(int unit, bcm_port_t port, int mode)
{
#if defined(BCM_RAVEN_SUPPORT) || defined (BCM_TRIUMPH2_SUPPORT)
    port_tab_entry_t ptab;
    uint32           rval;
    bcm_stg_t        stg;
    int              to_higig = 0;
    soc_info_t       *si = &SOC_INFO(unit);
    soc_field_t      hg_en[6] = {-1, HGIG2_EN_S0f, HGIG2_EN_S1f, -1, 
                                 HGIG2_EN_S3f, HGIG2_EN_S4f};
    soc_field_t      port_type;
    soc_reg_t        egr_port;   
    
    /* This check is not correct for devices supporting embedded higig */
    if (!soc_feature(unit, soc_feature_embedded_higig)) {
        if ((port != 1) && (port != 2) && (port != 4) && (port != 5)) {
            return BCM_E_PORT;
        }
    }

    to_higig = (mode == BCM_PORT_ENCAP_HIGIG2);
    sal_memset(&ptab, 0, sizeof(ptab));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                     MEM_BLOCK_ANY, port, &ptab));
    port_type = (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH2(unit) 
                 || SOC_IS_APOLLO(unit)) ? PORT_TYPEf : HIGIG_PACKETf;

    soc_PORT_TABm_field32_set(unit, &ptab, port_type, to_higig);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm,
                                      MEM_BLOCK_ALL, port, &ptab));
    if (SOC_REG_IS_VALID(unit, EGR_PORT_64r)) {
        egr_port = EGR_PORT_64r;
    } else {
        egr_port = EGR_PORTr;
    }
    SOC_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, egr_port, port, port_type, to_higig));


    if (SOC_IS_RAVEN(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, GPORT_CONFIGr, port, hg_en[port], to_higig));
    }

    if (to_higig) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_pause_set(unit, port, 0, 0));

        SOC_PBMP_PORT_ADD(si->s_pbm, port);
        SOC_PBMP_PORT_ADD(si->st.bitmap, port);
        SOC_PBMP_PORT_ADD(si->hl.bitmap, port);  
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);

        BCM_IF_ERROR_RETURN(bcm_esw_port_frame_max_set(unit, port, 0x3fe8));
    } else {
        SOC_PBMP_PORT_REMOVE(si->s_pbm, port);
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hl.bitmap, port);
        SOC_PBMP_PORT_ADD(si->ether.bitmap, port);

        /* Set pause and frame - other abilities depend on the board and
         * the application needs to call other APIs appropriately */
        BCM_IF_ERROR_RETURN(bcm_esw_port_pause_set(unit, port, 1, 1));
        BCM_IF_ERROR_RETURN(bcm_esw_port_frame_max_set(unit, port, 1518));
    }

    /* Set HG ingress CPU Opcode map to the CPU */
    if (SOC_REG_IS_VALID(unit, ICONTROL_OPCODE_BITMAP_64r)) {
        uint64 cpu_pbm64;
        uint64 rval64;      /* Current 64 bit register data.  */
        COMPILER_64_ZERO(cpu_pbm64);
        COMPILER_64_SET(cpu_pbm64, 0, to_higig ? 1 : 0);
        SOC_IF_ERROR_RETURN(
            READ_ICONTROL_OPCODE_BITMAP_64r(unit, port,&rval64));

        soc_reg64_field_set(unit, ICONTROL_OPCODE_BITMAP_64r, &rval64,
                            BITMAPf, cpu_pbm64);

        SOC_IF_ERROR_RETURN(
            WRITE_ICONTROL_OPCODE_BITMAP_64r(unit, port, rval64));
    } else {
        uint32 cpu_pbm;

        cpu_pbm = to_higig ? 1 : 0;
        SOC_IF_ERROR_RETURN(
            READ_ICONTROL_OPCODE_BITMAPr(unit, port,&rval));

        soc_reg_field_set(unit, ICONTROL_OPCODE_BITMAPr, 
                          &rval, BITMAPf, cpu_pbm);

        SOC_IF_ERROR_RETURN(
            WRITE_ICONTROL_OPCODE_BITMAPr(unit, port, rval));
    }

    /* HG ports to forwarding */
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_stp_set(unit, port,
                             to_higig ? BCM_STG_STP_FORWARD : BCM_STG_STP_DISABLE));

    /* Set untagged state in default VLAN properly */
    BCM_IF_ERROR_RETURN(_bcm_esw_vlan_untag_update(unit, port, to_higig));

    /* Resolve STG 0 */
    BCM_IF_ERROR_RETURN(bcm_esw_stg_default_get(unit, &stg));
    BCM_IF_ERROR_RETURN(
        bcm_esw_stg_stp_set(unit, stg, port,
                            to_higig ? BCM_STG_STP_FORWARD : BCM_STG_STP_DISABLE));
    return BCM_E_NONE;
#else
    return BCM_E_PARAM;
#endif
}

/*
 * Function:
 *      _bcm_port_ehg_header_mem_get
 * Purpose:
 *      Calculates the correct memories for a given port
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      tx_mem          - (OUT) tx memory for EHG heade
 *      rx_mem          - (OUT) rx memory for EHG heade
 *      rx_mask_mem     - (OUT) rx mask memory for EHG heade
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_port_ehg_header_mem_get(int unit, bcm_port_t port, soc_mem_t *tx_mem, 
                             soc_mem_t *rx_mem, soc_mem_t *rx_mask_mem)
{
    if (NULL == tx_mem || NULL == rx_mem || NULL == rx_mask_mem) {
        return BCM_E_PARAM;
    }

    switch (SOC_PORT_BLOCK_TYPE(unit, port)) {
        case SOC_BLK_GXPORT: 
            *tx_mem = XPORT_EHG_TX_TUNNEL_DATAm;
            *rx_mem = XPORT_EHG_RX_TUNNEL_DATAm;
            *rx_mask_mem = XPORT_EHG_RX_TUNNEL_MASKm;
            break;
        case SOC_BLK_GPORT:
            *tx_mem = GPORT_EHG_TX_TUNNEL_DATAm;
            *rx_mem = GPORT_EHG_RX_TUNNEL_DATAm;
            *rx_mask_mem = GPORT_EHG_RX_TUNNEL_MASKm;
            break; 
        case SOC_BLK_SPORT:
            *tx_mem = SPORT_EHG_TX_TUNNEL_DATAm;
            *rx_mem = SPORT_EHG_RX_TUNNEL_DATAm;
            *rx_mask_mem = SPORT_EHG_RX_TUNNEL_MASKm;
            break; 
        case SOC_BLK_XQPORT:
            *tx_mem = XQPORT_EHG_TX_TUNNEL_DATAm;
            *rx_mem = XQPORT_EHG_RX_TUNNEL_DATAm;
            *rx_mask_mem = XQPORT_EHG_RX_TUNNEL_MASKm;
            break; 
        default: 
            *tx_mem = *rx_mem = *rx_mask_mem = INVALIDm;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_ehg_header_read
 * Purpose:
 *      Reads EHG header from the HW tables
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      buffer          - (OUT) Pointer to the buffer to contain header
 * Returns:
 *      BCM_E_XXX
 */

STATIC int 
_bcm_port_ehg_header_read(int unit, bcm_port_t port, uint32 *buffer)
{
    soc_mem_t   tx_mem, rx_mem, rx_mask_mem;
    int         tbl_idx_start, tbl_idx_end, idx;
    xqport_ehg_rx_tunnel_data_entry_t      entry;
    
    BCM_IF_ERROR_RETURN(
        _bcm_port_ehg_header_mem_get(unit, port, &tx_mem, &rx_mem, 
                                     &rx_mask_mem));

    if (INVALIDm == tx_mem) {
        return (BCM_E_PORT);
    }
    
    tbl_idx_start = SOC_PORT_BINDEX(unit, port) * 4; 
    tbl_idx_end = tbl_idx_start + 3;

    for (idx = 0 ; idx <= (tbl_idx_end - tbl_idx_start); idx ++) {
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, tx_mem, SOC_PORT_BLOCK(unit, port), 
                     (idx + tbl_idx_start), (void *)&entry));
        soc_mem_field_get(unit, tx_mem, (void *)&entry, TUNNEL_DATAf, 
                          (buffer + (idx * 4)));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_port_ehg_header_write
 * Purpose:
 *      Writes EHG header into the HW tables
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      tx_buff         - (IN) Pointer to the tx header
 *      rx_buff         - (IN) Pointer to the rx header
 *      len             - (IN) Header length
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_port_ehg_header_write(int unit, bcm_port_t port, uint32 *tx_buf, 
                           uint32 *rx_buf, int len)
{
    soc_mem_t   tx_mem, rx_mem, rx_mask_mem;
    int         tbl_idx_start, tbl_idx_end, idx;
    xqport_ehg_rx_tunnel_data_entry_t data_entry;
    xqport_ehg_rx_tunnel_mask_entry_t mask_entry;

    BCM_IF_ERROR_RETURN(
        _bcm_port_ehg_header_mem_get(unit, port, &tx_mem, &rx_mem, 
                                     &rx_mask_mem));

    if (INVALIDm == tx_mem) {
        return (BCM_E_PORT);
    }

    tbl_idx_end = (SOC_PORT_BINDEX(unit, port) + 1) * 4 - 1;
    tbl_idx_start = tbl_idx_end - (len / 4) + 1;    

    for (idx = 0; idx <= (tbl_idx_end - tbl_idx_start); idx ++) {
        /* Reset hw buffer. */
        sal_memset(&data_entry, 0, sizeof(xqport_ehg_rx_tunnel_data_entry_t));
        soc_mem_field_set(unit, tx_mem, (uint32 *)&data_entry, TUNNEL_DATAf, 
                          (tx_buf + (idx * 4)));
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, tx_mem, SOC_PORT_BLOCK(unit, port), 
                           (idx + tbl_idx_start), &data_entry));

        /* Reset hw buffer. */
        sal_memset(&data_entry, 0, sizeof(xqport_ehg_rx_tunnel_data_entry_t));
        soc_mem_field_set(unit, rx_mem, (uint32 *)&data_entry, TUNNEL_DATAf, 
                          (rx_buf + (idx * 4)));
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, rx_mem, SOC_PORT_BLOCK(unit, port), 
                           (idx + tbl_idx_start), &data_entry));

        /* Reset hw buffer. */
       sal_memset(&mask_entry, 0xFF, sizeof(xqport_ehg_rx_tunnel_mask_entry_t));
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, rx_mask_mem, SOC_PORT_BLOCK(unit, port), 
                           (idx + tbl_idx_start), &mask_entry));
        
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *      _bcm_port_ehg_setup_txrx_ethernet_hdr
 * Purpose:
 *      Prepares SRC and DEST MAC addresses into rx and tx buffers on a given index
 * Parameters:
 *      tx_buffer       -(IN/OUT) - tx buffer to fill
 *      rx_buffer       -(IN/OUT) - rx buffer to fill
 *      index           -(IN/OUT) - index to the right location in buffer
 *      encap_config    - (IN) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */

STATIC void 
_bcm_port_ehg_setup_txrx_ethernet_hdr(uint32 *tx_buffer, uint32 *rx_buffer,
                                      int *index, 
                                      bcm_port_encap_config_t *encap_config)
{
    int idx = *index;

    /* Destination mac address. */
    tx_buffer[idx] = (((uint32)(encap_config->dst_mac)[0]) << 16 | \
                      ((uint32)(encap_config->dst_mac)[1]) << 8  | \
                      ((uint32)(encap_config->dst_mac)[2]));
    /* TX dest MAC is RX src MAC */
    rx_buffer[idx] = (((uint32)(encap_config->src_mac)[0]) << 16 | \
                      ((uint32)(encap_config->src_mac)[1]) << 8  | \
                      ((uint32)(encap_config->src_mac)[2]));
    idx--;

    tx_buffer[idx] = (((uint32)(encap_config->dst_mac)[3]) << 24 | \
                      ((uint32)(encap_config->dst_mac)[4]) << 16 | \
                      ((uint32)(encap_config->dst_mac)[5]) << 8  | \
                      ((uint32)(encap_config->src_mac)[0])); 
    /* TX dest MAC is RX src MAC */
    rx_buffer[idx] = (((uint32)(encap_config->src_mac)[3]) << 24 | \
                      ((uint32)(encap_config->src_mac)[4]) << 16 | \
                      ((uint32)(encap_config->src_mac)[5]) << 8  | \
                      ((uint32)(encap_config->dst_mac)[0])); 
    idx--;
    /* Source mac address. */
    tx_buffer[idx] = (((uint32)(encap_config->src_mac)[1]) << 24 | \
                      ((uint32)(encap_config->src_mac)[2]) << 16 | \
                      ((uint32)(encap_config->src_mac)[3]) << 8  | \
                      ((uint32)(encap_config->src_mac)[4])); 
    /* TX src MAC is RX dest MAC */
    rx_buffer[idx] = (((uint32)(encap_config->dst_mac)[1]) << 24 | \
                      ((uint32)(encap_config->dst_mac)[2]) << 16 | \
                      ((uint32)(encap_config->dst_mac)[3]) << 8  | \
                      ((uint32)(encap_config->dst_mac)[4])); 
    idx--;
    tx_buffer[idx] = (((uint32)(encap_config->src_mac)[5]) << 24);
    rx_buffer[idx] = (((uint32)(encap_config->dst_mac)[5]) << 24);

    /* Set tpid & vlan id. */
    if (BCM_VLAN_VALID(encap_config->vlan)) {
        /* Tpid. */
        tx_buffer[idx] |= (((uint32)(encap_config->tpid >> 8)) << 16 | \
                           ((uint32)(encap_config->tpid & 0xff)) << 8);
        rx_buffer[idx] |= (((uint32)(encap_config->tpid >> 8)) << 16 | \
                           ((uint32)(encap_config->tpid & 0xff)) << 8);

        /* Priority,  Cfi, Vlan id. */
        tx_buffer[idx] |= (((uint32)(encap_config->vlan >> 8)));
        rx_buffer[idx] |= (((uint32)(encap_config->vlan >> 8)));
        idx--;
        tx_buffer[idx] = ((uint32)(encap_config->vlan & 0xff) << 24);
        rx_buffer[idx] = ((uint32)(encap_config->vlan & 0xff) << 24);
    }

    *index = idx;
    return;
}


/*
 * Function:
 *      _bcm_port_ehg_ethernet_header_parse
 * Purpose:
 *      Parses Ethernet header from EHG header
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      header          - (IN) pointer to the EHG header buffer
 *      encap_config    - (IN/OUT) structure describes port encapsulation configuration
 *      index           - (IN/OUT) index to the right location in buffer
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_port_ehg_ethernet_header_parse(int unit, bcm_port_t port, uint32 *header,
                                    bcm_port_encap_config_t *encap_config, int *index)
{
    uint32  val;
    int     idx = *index;
    
    BCM_IF_ERROR_RETURN(
        READ_EHG_TX_CONTROLr(unit, port, &val));
    encap_config->dst_mac[0] = (uint8)(header[idx] >> 16);
    encap_config->dst_mac[1] = (uint8)(header[idx] >> 8);
    encap_config->dst_mac[2] = (uint8)(header[idx]);
    idx--;
    encap_config->dst_mac[3] = (uint8)(header[idx] >> 24);
    encap_config->dst_mac[4] = (uint8)(header[idx] >> 16);
    encap_config->dst_mac[5] = (uint8)(header[idx] >> 8);
    encap_config->src_mac[0] = (uint8)(header[idx]);
    idx--;
    encap_config->src_mac[1] = (uint8)(header[idx] >> 24);
    encap_config->src_mac[2] = (uint8)(header[idx] >> 16);
    encap_config->src_mac[3] = (uint8)(header[idx] >> 8);
    encap_config->src_mac[4] = (uint8)(header[idx]);
    idx--;
    encap_config->src_mac[5] = (uint8)(header[idx] >> 24);

    if (soc_reg_field_get(unit, EHG_TX_CONTROLr, val, VLAN_TAG_CONTROLf)) {
        encap_config->tpid = (uint16)(header[idx] >> 8);
        encap_config->vlan = ((uint8)(header[idx])) << 8;
        idx--;
        encap_config->vlan |= (uint8)((header[idx] >> 24));
    } else {
        encap_config->vlan = BCM_VLAN_NONE;
        encap_config->tpid = BCM_VLAN_NONE;
    }

    *index = idx;
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_port_ip4_tunnel_header_set
 * Purpose:
 *      Prepares and sets IP GRE tunnel header for Embedded higig
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      encap_config    - (IN) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_port_ip4_tunnel_header_set(int unit, bcm_port_t port, 
                               bcm_port_encap_config_t *encap_config)
{
    /*SW tunnel tx encap buffer.*/
    uint32 tx_buffer[_BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ]; 
    /*SW tunnel rx encap buffer.*/
    uint32 rx_buffer[_BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ]; 
    int    idx;  /* Headers offset iterator.*/
    
    sal_memset(tx_buffer, 0, WORDS2BYTES(_BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ));
    sal_memset(rx_buffer, 0, WORDS2BYTES(_BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ));

    idx = _BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ - 1; 
    
    _bcm_port_ehg_setup_txrx_ethernet_hdr(tx_buffer, rx_buffer, &idx, 
                                          encap_config);

    /* Set ether type to ip. 0x800  */
    tx_buffer[idx] |= (uint32)(0x08 << 16);
    rx_buffer[idx] |= (uint32)(0x08 << 16);
    /*
     *   IPv4 header. 
     */
    /* Version + 5 word no options length.  + Tos */
    /* Length, Id, Flags, Fragmentation offset. */
    tx_buffer[idx] |= ((uint32)(0x45)); 
    rx_buffer[idx] |= ((uint32)(0x45)); 
    idx--;

    tx_buffer[idx] = ((uint32)(encap_config->tos) << 24);
    rx_buffer[idx] = tx_buffer[idx];
    idx--;

    /* Ttl */
    tx_buffer[idx] = ((uint32)encap_config->ttl); 
    rx_buffer[idx] = ((uint32)encap_config->ttl); 
    idx--;

    /* Protocol (GRE 0x2f) */
    tx_buffer[idx] = ((uint32)(0x2f << 24));
    rx_buffer[idx] = ((uint32)(0x2f << 24));

    /* TX Src Ip. */
    tx_buffer[idx] |= ((uint32)(encap_config->src_addr >> 24));

    /* RX Dst Ip. */
    rx_buffer[idx] |= ((uint32)(encap_config->dst_addr >> 24));

    idx--;
    tx_buffer[idx] = (((uint32)((encap_config->src_addr << 8))));
    rx_buffer[idx] = (((uint32)((encap_config->dst_addr << 8))));

    /* TX Dst Ip. */
    tx_buffer[idx] |= ((uint32)(encap_config->dst_addr >> 24));
    /* RX Src Ip */
    rx_buffer[idx] |= ((uint32)(encap_config->src_addr >> 24));

    idx--;
    tx_buffer[idx] = (((uint32)((encap_config->dst_addr << 8))));
    rx_buffer[idx] = (((uint32)((encap_config->src_addr << 8))));

    /*
     *   Gre header. 
     */

    /* Protocol. 0x88be */
    tx_buffer[idx] |= ((uint32)(0x88));
    rx_buffer[idx] |= ((uint32)(0x88));
    idx--;

    tx_buffer[idx] = ((uint32)(0xbe) << 24);

    /* Set ether type to OUI  */
    tx_buffer[idx] |= (((uint32)(encap_config->oui_ethertype >> 8)) << 16 | \
                      ((uint32)(encap_config->oui_ethertype & 0xff)) << 8);

    /* Set OUI and HG ether type */
    tx_buffer[idx] |=  (((uint32)(encap_config->oui)[0]));
    /* From this point no difference between RX and TX */
    rx_buffer[idx] = tx_buffer[idx];    
    idx--;

    tx_buffer[idx] = (((uint32)(encap_config->oui)[1]) << 24 | \
                      ((uint32)(encap_config->oui)[2]) << 16);

    tx_buffer[idx] |= (((uint32)(encap_config->higig_ethertype >> 8)) << 8 | \
                       ((uint32)(encap_config->higig_ethertype)));

    rx_buffer[idx] = tx_buffer[idx];

    return _bcm_port_ehg_header_write(unit, port, tx_buffer, rx_buffer, 
                                      _BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ);
}

/*
 * Function:
 *      _bcm_port_ipv4_tunnel_header_parse
 * Purpose:
 *      Parse IPv4 GRE tunnel header for Embedded higig and updates encap_config 
 *      structure accordingly 
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      header          - (IN) Buffer contains EHG header to parse          
 *      encap_config    - (OUT) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_port_ipv4_tunnel_header_parse(int unit, bcm_port_t port, uint32 *header,
                                   bcm_port_encap_config_t *encap_config)
{
    /* Headers offset iterator, begins at end of header */
    int  idx = 4 * BYTES2WORDS(sizeof(xport_ehg_tx_tunnel_data_entry_t)) - 1; 

    BCM_IF_ERROR_RETURN(
    _bcm_port_ehg_ethernet_header_parse(unit, port, header, encap_config,
                                            &idx));

    idx--;
    encap_config->tos = (uint8)(header[idx] >> 24);
    idx--;
    encap_config->ttl = (uint8)(header[idx]);
    idx--;
    encap_config->src_addr = ((uint8)(header[idx])) << 24;
    idx--;
    encap_config->src_addr |=(uint32)(header[idx] >> 8);
    encap_config->dst_addr = ((uint8)(header[idx])) << 24;
    idx--;
    encap_config->dst_addr |=(uint32)(header[idx] >> 8);
    idx--;
    encap_config->oui_ethertype = (uint16)(header[idx] >> 8);
    encap_config->oui[0] = (uint8)(header[idx]);
    idx--;
    encap_config->oui[1] = (uint8)(header[idx] >> 24);
    encap_config->oui[2] = (uint8)(header[idx] >> 16);
    encap_config->higig_ethertype = (uint16)(header[idx]);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_l2_tunnel_header_set
 * Purpose:
 *      Prepares and sets L2 tunnel header for Embedded higig
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      encap_config    - (IN) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_port_l2_tunnel_header_set(int unit, bcm_port_t port, 
                               bcm_port_encap_config_t *encap_config)
{
    /*SW tunnel tx encap buffer.*/
    uint32 tx_buffer[_BCM_PORT_EHG_L2_HEADER_BUFFER_SZ]; 
    /*SW tunnel rx encap buffer.*/
    uint32 rx_buffer[_BCM_PORT_EHG_L2_HEADER_BUFFER_SZ]; 
    int     idx;  /* Headers offset iterator.*/
    
    sal_memset(tx_buffer, 0, WORDS2BYTES(_BCM_PORT_EHG_L2_HEADER_BUFFER_SZ));
    sal_memset(rx_buffer, 0, WORDS2BYTES(_BCM_PORT_EHG_L2_HEADER_BUFFER_SZ));

    idx = _BCM_PORT_EHG_L2_HEADER_BUFFER_SZ - 1;
    
    _bcm_port_ehg_setup_txrx_ethernet_hdr(tx_buffer, rx_buffer, &idx, 
                                          encap_config);

    /* Set ether type to OUI  */
    tx_buffer[idx] |= (((uint32)(encap_config->oui_ethertype >> 8)) << 16 | \
                       ((uint32)(encap_config->oui_ethertype & 0xff)) << 8);
    rx_buffer[idx] |= (((uint32)(encap_config->oui_ethertype >> 8)) << 16 | \
                       ((uint32)(encap_config->oui_ethertype & 0xff)) << 8);

    /* Set TX OUI and HG ether type */
    tx_buffer[idx] |=  (((uint32)(encap_config->oui)[0]));
    rx_buffer[idx] |=  (((uint32)(encap_config->oui)[0]));
    idx--;
    tx_buffer[idx] = (((uint32)(encap_config->oui)[1]) << 24 | \
                      ((uint32)(encap_config->oui)[2]) << 16);

    tx_buffer[idx] |= (((uint32)(encap_config->higig_ethertype >> 8)) << 8 | \
                       ((uint32)(encap_config->higig_ethertype)));
    rx_buffer[idx] = tx_buffer[idx];

    return _bcm_port_ehg_header_write(unit, port, tx_buffer, rx_buffer,
                                       _BCM_PORT_EHG_L2_HEADER_BUFFER_SZ);
}


/*
 * Function:
 *      _bcm_port_l2_tunnel_header_parse
 * Purpose:
 *      Parse L2 tunnel header for Embedded higig and updates encap_config 
 *      structure accordingly 
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      header          - (IN) Buffer contains EHG header to parse          
 *      encap_config    - (OUT) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_port_l2_tunnel_header_parse(int unit, bcm_port_t port, uint32 *header,
                                 bcm_port_encap_config_t *encap_config)
{
    /* Headers offset iterator, begins at end of header */
    int  idx = 4 * BYTES2WORDS(sizeof(xport_ehg_tx_tunnel_data_entry_t)) - 1; 
    
    BCM_IF_ERROR_RETURN(
        _bcm_port_ehg_ethernet_header_parse(unit, port, header, encap_config, 
                                            &idx));

    encap_config->oui_ethertype = (uint16)(header[idx] >> 8);
    encap_config->oui[0] = (uint8)(header[idx]);
    idx--;
    encap_config->oui[1] = (uint8)(header[idx] >> 24);
    encap_config->oui[2] = (uint8)(header[idx] >> 16);
    encap_config->higig_ethertype = (uint16)(header[idx]);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_encap_ehg_xport_update
 * Purpose:
 *      Updates XPORT higig config for EHG
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 * Returns:
 *      
 */
STATIC int 
_bcm_port_encap_ehg_xport_update(int unit, bcm_port_t port)
{
#ifdef BCM_GXPORT_SUPPORT
    soc_reg_t   egr_port_reg;
    uint32      val;

    if (IS_GX_PORT(unit, port) || IS_XQ_PORT(unit,port)) {
        
        egr_port_reg = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                        SOC_IS_VALKYRIE2(unit)) ? EGR_PORT_64r : EGR_PORTr;
        
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                  HIGIG2f, 1));
        
        BCM_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, egr_port_reg, port, HIGIG2f, 
                                   BCM_EGR_PORT_HIGIG2));
        BCM_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, egr_port_reg, port, PORT_TYPEf, 
                                   BCM_EGR_PORT_TYPE_EHG));
        
        if(SOC_REG_IS_VALID(unit, XPORT_CONFIGr)) {
            READ_XPORT_CONFIGr(unit, port, &val);
            soc_reg_field_set(unit, XPORT_CONFIGr, &val, HIGIG2_MODEf, 1);
            WRITE_XPORT_CONFIGr(unit, port, val);
        }

    }
#endif /* BCM_GXPORT_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_port_ehg_transport_mode_set
 * Purpose:
 *      Set the port into embedded higig transport mode
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      encap_config    - (IN) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_port_ehg_transport_mode_set(int unit, bcm_port_t port, 
                                 bcm_port_encap_config_t *encap_config)
{
    int         vlan_valid;
    uint32      val;

    if (IS_HG_PORT(unit, port)) {
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_port_encap_ehg_xport_update(unit, port));

    vlan_valid = BCM_VLAN_VALID(encap_config->vlan);

    /* Configure TX control */
    SOC_IF_ERROR_RETURN(
        READ_EHG_TX_CONTROLr(unit, port, &val));
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, ENABLEf, 1);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, TUNNEL_TYPEf, 0);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, VLAN_TAG_CONTROLf, 
                      vlan_valid);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, UPDATE_VLAN_PRI_CFIf, 
                      vlan_valid);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, PAYLOAD_LENGTH_ADJUSTMENTf,
                      0);    
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, UPDATE_DSCPf, 0);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, UPDATE_IPf, 0);
    SOC_IF_ERROR_RETURN
      (WRITE_EHG_TX_CONTROLr(unit, port, val));

    /* Configure RX control */
    SOC_IF_ERROR_RETURN(
        READ_EHG_RX_CONTROLr(unit, port, &val));
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, TUNNEL_TYPEf, 0);
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, VLAN_TAG_CONTROLf, 
                      vlan_valid);
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, COMPARE_VLANf, 
                      vlan_valid);
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, VLANf, 
                      (vlan_valid) ? encap_config->vlan : 0);
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, IPV4_CHKSUM_ENABLEf, 0);
    SOC_IF_ERROR_RETURN
      (WRITE_EHG_RX_CONTROLr(unit, port, val));

    return _bcm_port_l2_tunnel_header_set(unit, port, encap_config);

}

/*
 * Function:
 *      _bcm_port_ehg_tunnel_mode_set
 * Purpose:
 *      Set the port into embedded higig tunnel mode
 * Parameters:
 *      unit            - (IN) device id
 *      port            - (IN) Physical port
 *      encap_config    - (IN) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_port_ehg_tunnel_mode_set(int unit, bcm_port_t port, 
                                 bcm_port_encap_config_t *encap_config)
{
    int         vlan_valid;
    uint32      val;

    if (IS_HG_PORT(unit, port)) {
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_port_encap_ehg_xport_update(unit, port));

    vlan_valid = BCM_VLAN_VALID(encap_config->vlan);

    /* Configure TX control */
    SOC_IF_ERROR_RETURN(
        READ_EHG_TX_CONTROLr(unit, port, &val));
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, ENABLEf, 1);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, TUNNEL_TYPEf, 1);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, VLAN_TAG_CONTROLf, 
                      vlan_valid);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, UPDATE_VLAN_PRI_CFIf, 
                      vlan_valid);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, PAYLOAD_LENGTH_ADJUSTMENTf, 
         vlan_valid ? (WORDS2BYTES(_BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ) - 1):
                 (WORDS2BYTES(_BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ - 1) - 1));
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, UPDATE_DSCPf, 1);
    soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, UPDATE_IPf, 1);
    SOC_IF_ERROR_RETURN
      (WRITE_EHG_TX_CONTROLr(unit, port, val));

    /* Configure RX control */
    SOC_IF_ERROR_RETURN(
        READ_EHG_RX_CONTROLr(unit, port, &val));
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, TUNNEL_TYPEf, 1);
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, VLAN_TAG_CONTROLf, 
                      vlan_valid);
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, COMPARE_VLANf, 
                      vlan_valid);
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, VLANf, 
                      (vlan_valid) ? encap_config->vlan : 0);
    soc_reg_field_set(unit, EHG_RX_CONTROLr, &val, IPV4_CHKSUM_ENABLEf, 1);
    SOC_IF_ERROR_RETURN
      (WRITE_EHG_RX_CONTROLr(unit, port, val));

    return _bcm_port_ip4_tunnel_header_set(unit, port, encap_config);
}

/*
 * Function:
 *      bcm_esw_port_encap_config_set
 * Purpose:
 *      Set the port encapsulation 
 * Parameters:
 *      unit            - (IN) device id
 *      gport           - (IN) Generic port
 *      encap_config    - (IN) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_port_encap_config_set(int unit, bcm_gport_t gport, 
                                  bcm_port_encap_config_t *encap_config)
{
    bcm_port_t  port;
    int         rv = BCM_E_UNAVAIL;
    soc_info_t  *si = &SOC_INFO(unit);

    if (NULL == encap_config) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &port));

    if (encap_config->encap != BCM_PORT_ENCAP_HIGIG2_L2 && 
        encap_config->encap != BCM_PORT_ENCAP_HIGIG2_IP_GRE ) {
        return bcm_esw_port_encap_set(unit, port, encap_config->encap);
    }

    if (!soc_feature(unit, soc_feature_embedded_higig)) {
        return (BCM_E_UNAVAIL);
    }

    PORT_LOCK(unit);

    if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
        rv = _bcm_port_encap_xport_set(unit, port, BCM_PORT_ENCAP_IEEE);
    } else if (IS_ST_PORT(unit, port) || IS_E_PORT(unit, port)){
        rv = _bcm_port_encap_stport_set(unit, port, BCM_PORT_ENCAP_IEEE);
    }

    if (BCM_SUCCESS(rv)) {
        if (BCM_PORT_ENCAP_HIGIG2_L2 == encap_config->encap) {
            rv = _bcm_port_ehg_transport_mode_set(unit, port, encap_config);
        } else if (BCM_PORT_ENCAP_HIGIG2_IP_GRE == encap_config->encap){
            rv = _bcm_port_ehg_tunnel_mode_set(unit, port, encap_config);
        } 
    }
    /* Embedded Higig ports should be marked as ST ports */
    if (BCM_SUCCESS(rv)) {
        SOC_PBMP_PORT_ADD(si->st.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hg2_pbm, port);
    }

    PORT_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *      bcm_esw_port_encap_config_get
 * Purpose:
 *      Get the port encapsulation 
 * Parameters:
 *      unit            - (IN) device id
 *      gport           - (IN) Generic port
 *      encap_config    - (IN/OUT) structure describes port encapsulation configuration
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_port_encap_config_get(int unit, bcm_gport_t gport, 
                                  bcm_port_encap_config_t *encap_config)
{
    bcm_port_t      port;
    uint32          val, buffer[16];   /* 64 bytes max EHG header size */
    int             rv, mode;

    if (NULL == encap_config) {
        return (BCM_E_PARAM);
    }

    sal_memset(encap_config, 0, sizeof(bcm_port_encap_config_t));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &port));
    
    if (!soc_feature(unit, soc_feature_embedded_higig)) {
        rv = bcm_esw_port_encap_get(unit, port, &mode);
        if (BCM_SUCCESS(rv)) {
            encap_config->encap = (bcm_port_encap_t)mode; 
        }
        return rv;
    }

    SOC_IF_ERROR_RETURN(
        READ_EHG_TX_CONTROLr(unit, port, &val));
    /* If no EHG was programmed the just read port encapsulation */
    if (!soc_reg_field_get(unit, EHG_TX_CONTROLr, val, ENABLEf)) {
        rv = bcm_esw_port_encap_get(unit, port, &mode);
        if (BCM_SUCCESS(rv)) {
            encap_config->encap = (bcm_port_encap_t)mode; 
        }
        return rv;
    }

    /* EHG is enabled. */
    if (!IS_ST_PORT(unit, port)) {
        return (BCM_E_CONFIG);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_port_ehg_header_read(unit, port, buffer));

    if (!soc_reg_field_get(unit, EHG_TX_CONTROLr, val, TUNNEL_TYPEf)) {
        encap_config->encap = BCM_PORT_ENCAP_HIGIG2_L2;
        rv = _bcm_port_l2_tunnel_header_parse(unit, port, buffer, 
                                              encap_config);
    } else {
        encap_config->encap = BCM_PORT_ENCAP_HIGIG2_IP_GRE;
        rv = _bcm_port_ipv4_tunnel_header_parse(unit, port, buffer, 
                                                encap_config);
    }

    return (rv);
}

/*
 * Function:
 *      bcm_port_encap_set
 * Purpose:
 *      Set the port encapsulation mode
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      mode - One of BCM_PORT_ENCAP_xxx (see port.h)
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_encap_set(int unit, bcm_port_t port, int mode)
{
    int         rv, xport_swap = FALSE;
    int         stport_swap = FALSE;
    uint32      val;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_esw_port_encap_set: u=%d p=%d mode=%d\n",
                     unit, port, mode));

    PORT_LOCK(unit);

    if ((IS_HG_PORT(unit,port) && (mode == BCM_PORT_ENCAP_IEEE)) ||
        (IS_XE_PORT(unit,port) && (mode != BCM_PORT_ENCAP_IEEE))) {
        if (soc_feature(unit, soc_feature_xport_convertible)) {
            xport_swap =  TRUE;
        } else {
            /* Ether <=> Higig not allowed on all systems */
            PORT_UNLOCK(unit);
            return BCM_E_UNAVAIL;
        }
    }

    if (SOC_IS_RAVEN(unit)) {
        if ((IS_ST_PORT(unit,port) && (mode == BCM_PORT_ENCAP_IEEE)) ||
            (IS_E_PORT(unit,port) && (mode == BCM_PORT_ENCAP_HIGIG2))) {
                stport_swap =  TRUE;
        }
    }

    if (xport_swap) {
        rv = _bcm_port_encap_xport_set(unit, port, mode);
    } else if (stport_swap) {
        rv = _bcm_port_encap_stport_set(unit, port, mode);
    } else if (IS_HG_PORT(unit, port)) {
        rv = MAC_ENCAP_SET(PORT(unit, port).p_mac, unit, port, mode);
    } else if (IS_GE_PORT(unit, port) && IS_ST_PORT(unit, port)) {
        rv = (mode == BCM_PORT_ENCAP_HIGIG2) ? BCM_E_NONE : BCM_E_UNAVAIL;
    } else if (mode == BCM_PORT_ENCAP_IEEE) {
        rv = BCM_E_NONE;
    } else {
        rv = BCM_E_UNAVAIL;
    }

#ifdef BCM_GXPORT_SUPPORT
    if (IS_GX_PORT(unit, port) || IS_XG_PORT(unit,port)) {
        int hg2;
        soc_reg_t egr_port_reg;
        
        egr_port_reg = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                        SOC_IS_VALKYRIE2(unit)) ? EGR_PORT_64r : EGR_PORTr;
        hg2 = (mode == BCM_PORT_ENCAP_HIGIG2) ? 1 : 0;

        if (BCM_SUCCESS(rv)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                       HIGIG2f, hg2);
        }
        if (BCM_SUCCESS(rv)) {
            rv = soc_reg_field32_modify(unit, egr_port_reg, port, HIGIG2f, 
                                        (uint32)hg2);
        }
        if (BCM_SUCCESS(rv)) {
            rv = READ_XPORT_CONFIGr(unit, port, &val);
        }
        if (BCM_SUCCESS(rv)) {
            soc_reg_field_set(unit, XPORT_CONFIGr, &val, HIGIG2_MODEf, hg2);
            rv = WRITE_XPORT_CONFIGr(unit, port, val);
        }
    }
#endif /* BCM_GXPORT_SUPPORT */

    if (BCM_SUCCESS(rv)) {
        if (SOC_REG_IS_VALID(unit, EHG_TX_CONTROLr)) {
            rv = READ_EHG_TX_CONTROLr(unit, port, &val);
            soc_reg_field_set(unit, EHG_TX_CONTROLr, &val, ENABLEf, 0);
            if (BCM_SUCCESS(rv)) {            
                rv = WRITE_EHG_TX_CONTROLr(unit, port, val);
            }
        }
    }

#ifdef BCM_HIGIG2_SUPPORT
    /* Update cached version of HiGig2 encapsulation */
    if (BCM_SUCCESS(rv)) {
        if (mode == BCM_PORT_ENCAP_HIGIG2) {
            SOC_HG2_ENABLED_PORT_ADD(unit, port);
        } else {
            SOC_HG2_ENABLED_PORT_REMOVE(unit, port);
        }
    }
#endif /* BCM_HIGIG2_SUPPORT */

    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_encap_get
 * Purpose:
 *      Get the port encapsulation mode
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      mode (OUT) - One of BCM_PORT_ENCAP_xxx (see port.h)
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_encap_get(int unit, bcm_port_t port, int *mode)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (IS_GE_PORT(unit, port) && IS_ST_PORT(unit, port) 
         && !SOC_IS_ENDURO(unit)) {
        uint32 rval;
        soc_field_t hg_en[6] = {-1, HGIG2_EN_S0f, HGIG2_EN_S1f, -1, 
                                HGIG2_EN_S3f, HGIG2_EN_S4f};
        BCM_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &rval));
        if (soc_feature(unit, soc_feature_embedded_higig)) {
            *mode = BCM_PORT_ENCAP_IEEE;
        } else {
            if (SOC_REG_FIELD_VALID(unit, GPORT_CONFIGr, hg_en[port])) {
                *mode = soc_reg_field_get(unit, GPORT_CONFIGr, rval, 
                    hg_en[port]) ? BCM_PORT_ENCAP_HIGIG2 : BCM_PORT_ENCAP_IEEE;
            } else {
                return (BCM_E_CONFIG);
            }
        }
    } else {
        SOC_IF_ERROR_RETURN
            (MAC_ENCAP_GET(PORT(unit, port).p_mac, unit, port, mode));
    }
    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_esw_port_encap_get: u=%d p=%d mode=%d\n",
                     unit, port, *mode));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_queued_count_get
 * Purpose:
 *      Returns the count of packets (or cells) currently buffered
 *      for a port.  Useful to know when a port has drained all
 *      data and can then be re-configured.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      count (OUT) - count of packets currently buffered
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      "packets" may actually be cells on most chips,
 *      If no packets are buffered, the cell count is 0,
 *      If some packets are buffered the cell count will be
 *      greater than or equal to the packet count.
 */

int
bcm_esw_port_queued_count_get(int unit, bcm_port_t port, uint32 *count)
{
    uint32      regval;
    int         cos;

    *count = 0;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (SOC_IS_DRACO(unit) || SOC_IS_LYNX(unit) || SOC_IS_FB_FX_HX(unit)) {
        for (cos = 0; cos < NUM_COS(unit); cos++) {
            regval = 0;
            SOC_IF_ERROR_RETURN(READ_COSLCCOUNTr(unit, port, cos, &regval));
            *count += soc_reg_field_get(unit, COSLCCOUNTr, regval, LCCOUNTf);
        }
        return BCM_E_NONE;
    }

#if defined(BCM_TUCANA_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    /*
     * Tucana and Easyrider actually keep the packet count!
     */
    if (SOC_IS_TUCANA(unit) || SOC_IS_EASYRIDER(unit)) {
        soc_field_t count_f = SOC_IS_EASYRIDER(unit) ? COUNTf : PKTCOUNTf;
        for (cos = 0; cos < NUM_COS(unit); cos++) {
            regval = 0;
            SOC_IF_ERROR_RETURN(READ_MTPCOSr(unit, port, cos, &regval));
            *count += soc_reg_field_get(unit, MTPCOSr, regval,
                                        count_f);
        }
        return BCM_E_NONE;
    }
#endif  /* BCM_TUCANA_SUPPORT || BCM_EASYRIDER_SUPPORT */

#if defined(BCM_XGS12_FABRIC_SUPPORT)
    if (SOC_IS_XGS12_FABRIC(unit)) {
        regval = 0;
        SOC_IF_ERROR_RETURN(READ_MMU_CELLCNTTOTALr(unit, port, &regval));
        *count += soc_reg_field_get(unit, MMU_CELLCNTTOTALr, regval, COUNTf);
        return BCM_E_NONE;
    }
#endif /* BCM_XGS12_FABRIC_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HBX(unit)) {
        regval = 0;
        SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNTr(unit, port, &regval));
        *count += soc_reg_field_get(unit, OP_PORT_TOTAL_COUNTr,
                                    regval, OP_PORT_TOTAL_COUNTf);
        return BCM_E_NONE;
    }
#endif /* BCM_BRADLEY_SUPPORT  */

    return BCM_E_UNAVAIL;
}

    /*
 * Function:
 *      bcm_port_protocol_vlan_add
 * Purpose:
 *      Adds a protocol based vlan to a port.  The protocol
 *      is matched by frame type and ether type.  Returns an
 *      error if hardware does not support protocol vlans
 *      (Strata and Hercules).
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      frame - one of BCM_PORT_FRAMETYPE_{ETHER2,8023,LLC}
 *      ether - 16 bit Ethernet type field
 *      vid - VLAN ID
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_protocol_vlan_add(int unit,
                           bcm_port_t port,
                           bcm_port_frametype_t frame,
                           bcm_port_ethertype_t ether,
                           bcm_vlan_t vid)
{
#ifdef  BCM_XGS_SWITCH_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    PORT_SWITCHED_CHECK(unit, port);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }

        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = vid;
        action.new_inner_vlan = 0;
        action.priority = PORT(unit, port).p_ut_prio;
        action.ut_outer = bcmVlanActionAdd;
        action.it_outer = bcmVlanActionAdd;
        action.it_inner = bcmVlanActionDelete;
        action.it_inner_prio = bcmVlanActionDelete;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;

        return _bcm_trx_vlan_port_protocol_action_add(unit, port, frame,
                                                     ether, &action);
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_DRACO1_SUPPORT) || defined(BCM_TUCANA_SUPPORT)
    if (SOC_IS_DRACO1(unit) || SOC_IS_TUCANA(unit)) {
        return _bcm_draco_port_protocol_vlan_add(unit, port, frame,
                                                 ether, vid);
    }
#endif /* BCM_DRACO1_SUPPORT || BCM_TUCANA_SUPPORT */

#if defined(BCM_DRACO15_SUPPORT)
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_draco15_port_protocol_vlan_add(unit, port, frame, 
                                                   ether, vid);
    }
#endif /* BCM_DRACO15_SUPPORT */

#if defined(BCM_LYNX_SUPPORT)
    if (SOC_IS_LYNX(unit)) {
        return _bcm_lynx_port_protocol_vlan_add(unit, port, frame, ether, vid);
    }
#endif /* BCM_LYNX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_port_protocol_vlan_add(unit, port, frame, ether, vid);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_port_protocol_vlan_add(unit, port, frame, ether, vid);
    }
#endif /* BCM_EASYRIDER_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */
    /* not supported on STRATA and HERCULES */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_protocol_vlan_delete
 * Purpose:
 *      Remove an already created proto protocol based vlan
 *      on a port.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      frame - one of BCM_PORT_FRAMETYPE_{ETHER2,8023,LLC}
 *      ether - 16 bit Ethernet type field
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_protocol_vlan_delete(int unit,
                              bcm_port_t port,
                              bcm_port_frametype_t frame,
                              bcm_port_ethertype_t ether)
{
#ifdef  BCM_XGS_SWITCH_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    PORT_SWITCHED_CHECK(unit, port);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_port_protocol_delete(unit, port, frame, ether);
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_DRACO1_SUPPORT) || defined(BCM_TUCANA_SUPPORT)
    if (SOC_IS_DRACO1(unit) || SOC_IS_TUCANA(unit)) {
        return _bcm_draco_port_protocol_vlan_delete(unit, port, frame, ether);
    }
#endif /* BCM_DRACO1_SUPPORT || BCM_TUCANA_SUPPORT */

#if defined(BCM_DRACO15_SUPPORT)
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_draco15_port_protocol_vlan_delete(unit, port, frame, ether);
    }
#endif /* BCM_DRACO15_SUPPORT */

#if defined(BCM_LYNX_SUPPORT)
    if (SOC_IS_LYNX(unit)) {
        return _bcm_lynx_port_protocol_vlan_delete(unit, port, frame, ether);
    }
#endif /* BCM_LYNX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_port_protocol_vlan_delete(unit, port, frame, ether);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_port_protocol_vlan_delete(unit, port, frame, ether);
    }
#endif /* BCM_EASYRIDER_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */
    /* not supported on STRATA and HERCULES */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_protocol_vlan_delete_all
 * Purpose:
 *      Remove all protocol based vlans on a port.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_protocol_vlan_delete_all(int unit, bcm_port_t port)
{
#ifdef  BCM_XGS_SWITCH_SUPPORT
    int         i;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    PORT_SWITCHED_CHECK(unit, port);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_port_protocol_delete_all(unit, port);
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_DRACO1_SUPPORT) || defined(BCM_TUCANA_SUPPORT)
    if (SOC_IS_DRACO1(unit) || SOC_IS_TUCANA(unit)) {
        uint64  regval;

        COMPILER_64_ZERO(regval);

        for (i = 0; i < SOC_REG_NUMELS(unit, PRTABLE_ENTRYr); i++) {
            SOC_IF_ERROR_RETURN(WRITE_PRTABLE_ENTRYr(unit, port, i, regval));
        }
        return BCM_E_NONE;
    }
#endif /* BCM_DRACO1_SUPPORT || BCM_TUCANA_SUPPORT */

#if defined(BCM_DRACO15_SUPPORT)
    if (SOC_IS_DRACO15(unit)) {
        vlan_protocol_entry_t   vpe;
        vlan_data_entry_t       vde;
        bcm_vlan_t              cvid, defvid;
        _bcm_port_info_t        *pinfo;
        int                     idxmin, idxmax;
        int                     vlan_prot_entries, vlan_data_prot_start;
        int                     vdentry, p, valid;

        idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
        idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);
        vlan_prot_entries = soc_mem_index_count(unit, VLAN_PROTOCOLm);
        vlan_data_prot_start = soc_mem_index_max(unit, VLAN_SUBNETm) + 1;

        for (i = idxmin; i <= idxmax; i++) {
            vdentry = vlan_data_prot_start + (port * vlan_prot_entries) + i;
            SOC_IF_ERROR_RETURN
                (READ_VLAN_DATAm(unit, MEM_BLOCK_ANY, vdentry, &vde));
            cvid = soc_VLAN_DATAm_field32_get(unit, &vde, VLAN_IDf);
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_untagged_vlan_get(unit, port, &defvid));
            BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
            if (cvid == 0 ||
                (cvid == defvid && (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)))) {
                continue;
            }
            sal_memset(&vde, 0, sizeof(vde));
            soc_VLAN_DATAm_field32_set(unit, &vde, VLAN_IDf, defvid);
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_DATAm(unit, MEM_BLOCK_ALL, vdentry, &vde));
            _BCM_PORT_VD_PBVL_CLEAR(pinfo, i);
            /* see if any vlan_data entries are still valid */
            valid = 0;
            PBMP_E_ITER(unit, p) {
                BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
                if (p == port) {        /* skip the entry we just wrote */
                    continue;
                }
                /* entry valid if programmed VLAN ID for port */
                if (_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) {
                    valid = 1;
                    break;
                }
            }
            if (!valid) {
                /* clear all VLAN_DATA entries associated with this protocol */
                PBMP_E_ITER(unit, p) {
                    vdentry = vlan_data_prot_start + (p * vlan_prot_entries) + i;
                    sal_memset(&vde, 0, sizeof(vde));
                    SOC_IF_ERROR_RETURN
                        (WRITE_VLAN_DATAm(unit, MEM_BLOCK_ALL, vdentry, &vde));
                    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
                    _BCM_PORT_VD_PBVL_CLEAR(pinfo, i);
                }
                /* clear VLAN_PROTOCOL entry */
                sal_memset(&vpe, 0, sizeof(vpe));
                SOC_IF_ERROR_RETURN
                    (WRITE_VLAN_PROTOCOLm(unit, MEM_BLOCK_ALL, i, &vpe));
            }
        }
        return BCM_E_NONE;
    }
#endif /* BCM_DRACO15_SUPPORT */

#if defined(BCM_LYNX_SUPPORT)
    if (SOC_IS_LYNX(unit)) {
        int     idxmin, idxmax, blk;
        pr_tab_entry_t  pte;

        blk = SOC_PORT_BLOCK(unit, port);
        sal_memset(&pte, 0, sizeof(pte));

        idxmin = soc_mem_index_min(unit, PR_TABm);
        idxmax = soc_mem_index_max(unit, PR_TABm);
        for (i = idxmin; i <= idxmax; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PR_TABm(unit, blk, i, &pte));
        }
        return BCM_E_NONE;
    }
#endif /* BCM_LYNX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        vlan_protocol_entry_t        vpe;
        vlan_protocol_data_entry_t   vde;
        bcm_vlan_t                   cvid, defvid;
        int                          idxmin, idxmax;
        int                          vlan_prot_entries;
        int                          vdentry, p, valid;
        _bcm_port_info_t             *pinfo;
        bcm_pbmp_t                   switched_pbm;

        idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
        idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);
        vlan_prot_entries = idxmax + 1;

        for (i = idxmin; i <= idxmax; i++) {
            vdentry = (port * vlan_prot_entries) + i;
            SOC_IF_ERROR_RETURN
                (READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                                          vdentry, &vde));
            cvid = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, VLAN_IDf);
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_untagged_vlan_get(unit, port, &defvid));
            BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
            if (cvid == 0 ||
                (cvid == defvid && (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)))) {
                continue;
            }
            sal_memset(&vde, 0, sizeof(vde));
            soc_VLAN_PROTOCOL_DATAm_field32_set(unit, &vde, VLAN_IDf, defvid);
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ALL, vdentry, &vde));
            _BCM_PORT_VD_PBVL_CLEAR(pinfo, i);
            /*
             * see if any vlan_protocol_data entries are still valid
             * for the current protocol.
             */
            valid = 0;
            switched_pbm = PBMP_E_ALL(unit);
            if (soc_feature(unit, soc_feature_cpuport_switched)) {
                BCM_PBMP_OR(switched_pbm, PBMP_CMIC(unit));
            }
       
            BCM_PBMP_ITER(switched_pbm, p) {
                if (p == port) {    /* skip the entry we just "defaulted" */
                    continue;
                }
                BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
                /* entry valid if programmed VLAN ID for port */
                if (_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) {
                    valid = 1;
                    break;
                }
            }
            if (!valid) {
                /*
                 * Clear all VLAN_PROTOCOL_DATA entries associated
                 * with this protocol.
                 */
                BCM_PBMP_ITER(switched_pbm, p) {
                    vdentry = (p * vlan_prot_entries) + i;
                    sal_memset(&vde, 0, sizeof(vde));
                    SOC_IF_ERROR_RETURN
                        (WRITE_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ALL,
                                                   vdentry, &vde));
                    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
                    _BCM_PORT_VD_PBVL_CLEAR(pinfo, i);
                }
                /* clear VLAN_PROTOCOL entry */
                sal_memset(&vpe, 0, sizeof(vpe));
                SOC_IF_ERROR_RETURN
                    (WRITE_VLAN_PROTOCOLm(unit, MEM_BLOCK_ALL, i, &vpe));
            }
        }
        return BCM_E_NONE;
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        /*
         * Disable all protocol mapping & map data for target port.
         */
        for (i = 0; i < SOC_REG_NUMELS(unit, VLAN_PROTOCOLr); i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_PROTOCOLr(unit, port, i, 0));
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_PROTOCOL_DATAr(unit, port, i, 0));
        }
        return BCM_E_NONE;
    }
#endif /* BCM_EASYRIDER_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */

    /* not supported on STRATA and HERCULES */

    return BCM_E_UNAVAIL;
}

/*
 * Differentiated Services Code Point Mapping
 */

/*
 * Function:
 *      bcm_esw_port_dscp_map_mode_set
 * Purpose:
 *      Set DSCP mapping for the port.
 * Parameters:
 *      unit - switch device
 *      port - switch port      or -1 to apply on all the ports.
 *      mode - BCM_PORT_DSCP_MAP_NONE
 *           - BCM_PORT_DSCP_MAP_ZERO Map if incomming DSCP = 0
 *           - BCM_PORT_DSCP_MAP_ALL DSCP -> DSCP mapping. 
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_esw_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode)
{
    bcm_port_cfg_t      pcfg;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    if (pcfg.pc_dse_mode == -1) {       /* STRATA1 for example */
        return BCM_E_UNAVAIL;
    }
    /*
     * Strata/Draco/Lynx/Tucana supports an additional mode. mode = 2
     */
    switch(mode) {
        case BCM_PORT_DSCP_MAP_NONE:
            pcfg.pc_dse_mode = 0;
#if defined(BCM_XGS_SWITCH_SUPPORT)
            pcfg.pc_dscp_prio = 0;
#endif /* BCM_XGS_SWITCH_SUPPORT */
            break;
        case BCM_PORT_DSCP_MAP_ZERO:
            if (soc_feature(unit, soc_feature_dscp_map_mode_all)) {
                return BCM_E_UNAVAIL;
            }
            pcfg.pc_dse_mode = 1;
#if defined(BCM_XGS_SWITCH_SUPPORT)
            pcfg.pc_dscp_prio = 1;
#endif /* BCM_XGS_SWITCH_SUPPORT */
            break;
        case BCM_PORT_DSCP_MAP_ALL:
            if (soc_feature(unit, soc_feature_dscp_map_mode_all)) {
                pcfg.pc_dse_mode = 1;
            } else {
                pcfg.pc_dse_mode = 2;
            }
#if defined(BCM_XGS_SWITCH_SUPPORT)
            pcfg.pc_dscp_prio = 1;
#endif /* BCM_XGS_SWITCH_SUPPORT */
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));

    return BCM_E_NONE;
}

int
bcm_esw_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode)
{
    int rv;
    bcm_port_config_t port_conf;
    bcm_pbmp_t pbmp;

    if (!soc_feature(unit, soc_feature_dscp)) {
        return BCM_E_UNAVAIL;
    }

    if (port != -1) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    }

    PORT_LOCK(unit);

    /* Get device port configuration. */
    rv = bcm_esw_port_config_get(unit, &port_conf);
    if (BCM_FAILURE(rv)) {
        PORT_UNLOCK(unit);
        return rv;
    }
    BCM_PBMP_ASSIGN(pbmp, port_conf.e);
    if (SOC_IS_XGS3_SWITCH(unit) || SOC_IS_XGS3_FABRIC(unit)) {
        BCM_PBMP_OR(pbmp, port_conf.cpu);
    }

    if (port == -1) {
        PBMP_ITER(pbmp, port) {
            rv = _bcm_esw_port_dscp_map_mode_set(unit, port, mode);
            if (BCM_FAILURE(rv)) {
                PORT_UNLOCK(unit);
                return rv;
            }
        }
    } else {
        if (BCM_PBMP_MEMBER(pbmp, port)) {
            rv = _bcm_esw_port_dscp_map_mode_set(unit, port, mode);
        } else {
            rv = BCM_E_PORT;
        }
    }
    PORT_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_port_dscp_map_mode_get
 * Purpose:
 *      DSCP mapping status for the port.
 * Parameters:
 *      unit - switch device
 *      port - switch port or -1 to get mode from first available port
 *      mode - (OUT)
 *           - BCM_PORT_DSCP_MAP_NONE
 *           - BCM_PORT_DSCP_MAP_ZERO Map if incomming DSCP = 0
 *           - BCM_PORT_DSCP_MAP_ALL DSCP -> DSCP mapping. 
 * Returns:
 *      BCM_E_XXX
 */

static int
_bcm_esw_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    bcm_port_cfg_t      pcfg;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    if (pcfg.pc_dse_mode == -1) {       /* STRATA1 for example */
        return BCM_E_UNAVAIL;
    }
    /*
     * Strata/Draco/Lynx/Tucana supports an additional mode. mode = 2
     */
    switch(pcfg.pc_dse_mode) {
        case 1:
            *mode = soc_feature(unit, soc_feature_dscp_map_mode_all) ?
                        BCM_PORT_DSCP_MAP_ALL : BCM_PORT_DSCP_MAP_ZERO;
            break;
        case 2:
            *mode = BCM_PORT_DSCP_MAP_ALL;
            break;
        case 0:
        default:
            *mode = BCM_PORT_DSCP_MAP_NONE;
            break;
    }

    return BCM_E_NONE;
}

int
bcm_esw_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    int rv;
    bcm_port_config_t port_conf;
    bcm_pbmp_t pbmp;

    if (!soc_feature(unit, soc_feature_dscp)) {
        return BCM_E_UNAVAIL;
    }

    if (port != -1) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    }

    PORT_LOCK(unit);

    /* Get device port configuration. */
    rv = bcm_esw_port_config_get(unit, &port_conf);
    if (BCM_FAILURE(rv)) {
        PORT_UNLOCK(unit);
        return rv;
    }
    BCM_PBMP_ASSIGN(pbmp, port_conf.e);
    if (SOC_IS_XGS3_SWITCH(unit) || SOC_IS_XGS3_FABRIC(unit)) {
        BCM_PBMP_OR(pbmp, port_conf.cpu);
    }

    if (port == -1) {
        PBMP_ITER(pbmp, port) {
            break;
        }
    }

    if (BCM_PBMP_MEMBER(pbmp, port)) {
        rv = _bcm_esw_port_dscp_map_mode_get(unit, port, mode);
    } else {
        rv = BCM_E_PORT;
    }
    PORT_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_port_dscp_map_set
 * Purpose:
 *      Internal implementation for bcm_port_dscp_map_set
 * Parameters:
 *      unit - switch device
 *      port - switch port or -1 for global table
 *      srccp - src code point or -1
 *      mapcp - mapped code point or -1
 *      prio - priority value for mapped code point
 *              -1 to use port default untagged priority
 *              BCM_PRIO_RED    can be or'ed into the priority
 *              BCM_PRIO_YELLOW can be or'ed into the priority
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_port_dscp_map_set(int unit, bcm_port_t port,
                       int srccp, int mapcp, int prio)
{
#define DSCP_CODE_POINT_CNT 64
#define DSCP_CODE_POINT_MAX (DSCP_CODE_POINT_CNT - 1)
    bcm_port_cfg_t      pcfg;

    if (mapcp < -1 || mapcp > DSCP_CODE_POINT_MAX) {
        return BCM_E_PARAM;
    }

#if defined(BCM_DRACO15_SUPPORT)
    if (SOC_IS_DRACO15(unit)) {
        int             base, i, cng;
        dscp_entry_t    de;
        if (srccp < -1 || srccp > DSCP_CODE_POINT_MAX) {
            return BCM_E_PARAM;
        }
        /* Extract cng bit and check for valid priority. */
        cng = 0;
        if (prio < 0) {
            prio = PORT(unit, port).p_ut_prio;
        }
        if (prio & BCM_PRIO_DROP_FIRST) {
            cng = 1;
            prio &= ~BCM_PRIO_DROP_FIRST;
        }
        if ((prio & ~BCM_PRIO_MASK) != 0) {
            return BCM_E_PARAM;
        }
        if (srccp < 0 && mapcp < 0) {
            /* No mapping */
            return BCM_E_NONE;
        } else if (srccp < 0) {
            /* Map all DSCPs to a new DSCP */
            /* fill all DSCP_CODE_POINT_CNT entries in DSCPm with mapcp */
            sal_memset(&de, 0, sizeof(de));
            soc_DSCPm_field32_set(unit, &de, DSCPf, mapcp);
            soc_DSCPm_field32_set(unit, &de, PRIf, prio);
            soc_DSCPm_field32_set(unit, &de, CNGf, cng); /* congestion */
            base = port * DSCP_CODE_POINT_CNT;
            for (i = 0; i < DSCP_CODE_POINT_CNT; i++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_DSCPm(unit, MEM_BLOCK_ALL, base + i, &de));
            }
        } else {
            /* Map a specific DSCP to a new DSCP */
            sal_memset(&de, 0, sizeof(de));
            base = port * DSCP_CODE_POINT_CNT;

            /* fill specific srccp entry in DSCPm with mapcp */
            soc_DSCPm_field32_set(unit, &de, DSCPf, mapcp);
            soc_DSCPm_field32_set(unit, &de, PRIf, prio);
            soc_DSCPm_field32_set(unit, &de, CNGf, cng);

            SOC_IF_ERROR_RETURN
                (WRITE_DSCPm(unit, MEM_BLOCK_ALL, base + srccp, &de));
        }
        return BCM_E_NONE;
    }
#endif /* BCM_DRACO15_SUPPORT */

#if defined(BCM_TUCANA_SUPPORT)
    if (SOC_IS_TUCANA(unit)) {
        /*
         * DSCP -> DSCP mapping
         *      Map src Codepoint = 0 to a new value 
         *      Identity mapping
         */
        int cng = 0;
        if (srccp != mapcp) {
            if (srccp < -1 || srccp > 0) {
                return BCM_E_PARAM;
            }
        } else if (prio >= 0) {
            /* Extract cng bit and check for valid priority. */
            if (prio & BCM_PRIO_DROP_FIRST) {
                cng = 1;
                prio &= ~BCM_PRIO_DROP_FIRST;
            }
            if ((prio & ~BCM_PRIO_MASK) != 0) {
                return BCM_E_PARAM;
            }
        } else if (srccp > 0) {
            /* srccp == mapcd > 0 && prio < 0 */
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
          (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));

        if ((mapcp >= 0) && (srccp == mapcp)) {
            if (prio >= 0) {
                /*
                 * Tucana can map DSCP to 802.1p priority. This mapping is
                 * derived from a chip-wide lookup table, although each port
                 * can be individually configured to use it.
                 *
                 * Set only the designated DSCP-to-priority lookup
                 * table entry (device global, irrespective of port).
                 * Leave the other table entries as they currently are.
                 *
                 * We'll also enable DSCP-to-priority mapping for this port.
                 */

                dscp_priority_table_entry_t dpe;

                SOC_IF_ERROR_RETURN
                  (READ_DSCP_PRIORITY_TABLEm(unit, MEM_BLOCK_ANY,
                                             srccp, &dpe));
                soc_DSCP_PRIORITY_TABLEm_field32_set(unit, &dpe,
                                                     PRIORITYf, prio);
                soc_DSCP_PRIORITY_TABLEm_field32_set(unit, &dpe,
                                                     DROP_PRECEDENCEf, cng);
                SOC_IF_ERROR_RETURN
                  (WRITE_DSCP_PRIORITY_TABLEm(unit, MEM_BLOCK_ALL,
                                              srccp, &dpe));
            } else {
                /* srccp = mapcp = 0 with invalid prio same as "no mapping" */
                srccp = mapcp = -1;
            }
        }

        /*
         * DSCP-to-DSCP mapping configuration if map zero (src dscp = 0)
         * or map all  ((src dscp = -1)
         * if srccp == mapcp adjust only the priority mapping
         * as arbitrary DSCP -> DSCP mapping not supported.
         */
        if ((mapcp >= 0) && (srccp <= 0)) {
            pcfg.pc_dscp = mapcp;
        }

        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));

        return BCM_E_NONE;
    }
#endif /* BCM_TUCANA_SUPPORT */

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        int                  i, cng;
        dscp_table_entry_t   de;
        int                  max_index;   
        int                  min_index;

        if (srccp < -1 || srccp > DSCP_CODE_POINT_MAX) {
            return BCM_E_PARAM;
        }
        /* Extract cng bits and check for valid priority. */
        
        cng = 0; /* Green */
        if (prio < 0) {
            return BCM_E_PARAM;
        }
        if (prio & BCM_PRIO_RED) {
            cng = 0x01;  /* Red */
            prio &= ~BCM_PRIO_RED;
        } else if (prio & BCM_PRIO_YELLOW) {
            cng = 0x03;  /* Yellow  */
            prio &= ~BCM_PRIO_YELLOW;
        }
        if ((prio & ~BCM_PRIO_MASK) != 0) {
            return BCM_E_PARAM;
        }
        if (srccp < 0 && mapcp < 0) {
            /* No mapping */
            return BCM_E_NONE;
        } else if (srccp < 0) {
            /* Map all DSCPs to a new DSCP */
            /* fill all entries in DSCP_TABLEm with mapcp */
            sal_memset(&de, 0, sizeof(de));
            soc_DSCP_TABLEm_field32_set(unit, &de, DSCPf, mapcp);
            soc_DSCP_TABLEm_field32_set(unit, &de, PRIf, prio);
            soc_DSCP_TABLEm_field32_set(unit, &de, CNGf, cng);
            if (soc_feature(unit, soc_feature_dscp_map_per_port)) {
                min_index = port * DSCP_CODE_POINT_CNT;
                max_index = min_index + DSCP_CODE_POINT_MAX;
            } else {
                max_index = soc_mem_index_max(unit, DSCP_TABLEm);
                min_index = 0;
            }

            for (i = min_index; i <= max_index; i++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_DSCP_TABLEm(unit, MEM_BLOCK_ALL, i, &de));
            }
        } else {
            int num_cosq;
            if (SOC_IS_EASYRIDER(unit)) {
                num_cosq = 8; /* Per-cosq */
            } else {
                num_cosq = 1; /* For all cosqs */
            }
            /* Map a specific DSCP to a new DSCP */
            sal_memset(&de, 0, sizeof(de));
            /* fill specific srccp entry/entries in DSCP_TABLEm with mapcp */
            soc_DSCP_TABLEm_field32_set(unit, &de, DSCPf, mapcp);
            soc_DSCP_TABLEm_field32_set(unit, &de, PRIf, prio);
            soc_DSCP_TABLEm_field32_set(unit, &de, CNGf, cng);

            if (soc_feature(unit, soc_feature_dscp_map_per_port)) {
                SOC_IF_ERROR_RETURN
                  (WRITE_DSCP_TABLEm(unit, MEM_BLOCK_ALL, 
                                     (port * DSCP_CODE_POINT_CNT) + srccp, &de));
            } else {
                for (i = 0; i < num_cosq ; i++) {
                    SOC_IF_ERROR_RETURN
                      (WRITE_DSCP_TABLEm(unit, MEM_BLOCK_ALL, 
                                         (i * DSCP_CODE_POINT_CNT) + srccp, &de));
                }
            }
        }
        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    /* all other switches */
    if (srccp < -1 || srccp > 0) {
        return BCM_E_PARAM;
    }

    pcfg.pc_dse_mode = -1;
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    if (pcfg.pc_dse_mode == -1) {       /* STRATA1 for example */
        return BCM_E_UNAVAIL;
    }

    if (mapcp >= 0) {
        pcfg.pc_dscp = mapcp;

        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_dscp_map_set
 * Purpose:
 *      Define a mapping for diffserv code points
 *      The mapping enable/disable is controlled by a seperate API
 *      bcm_port_dscp_map_enable_set/get
 *      Also Enable/Disable control for DSCP mapping for tunnels
 *      are available in the respective tunnel create APIs.
 * Parameters:
 *      unit - switch device
 *      port - switch port      or -1 to setup global mapping table.
 *      srccp - src code point or -1
 *      mapcp - mapped code point or -1
 *      prio - priority value for mapped code point
 *              -1 to use port default untagged priority
 *              BCM_PRIO_DROP_FIRST can be or'ed into the priority
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      On all strata2 and most xgs switches, there are a limited set
 *      of mappings that are possible:
 *              srccp -1, mapcp -1:     no mapping
 *              srccp -1, mapcp 0..63:  map all packets
 *              srccp 0, mapcp 0..63:   map packets with cp 0
 *      On Draco1.5 switches, there is a full per port mapping table
 *      for all 64 possible code points.
 *      On Firebolt/Helix/Felix, there is no per port mapping table.
 *      Only a global mapping table is available.
 */

int
bcm_esw_port_dscp_map_set(int unit, bcm_port_t port, int srccp, int mapcp,
                      int prio)
{
    int rv;
    bcm_port_config_t port_conf;
    bcm_pbmp_t pbmp;

    if (!soc_feature(unit, soc_feature_dscp)) {
        return BCM_E_UNAVAIL;
    }

    if (port != -1) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
        if (!soc_feature(unit, soc_feature_dscp_map_per_port)) {
            return BCM_E_PORT;
        }
    }

    PORT_LOCK(unit);

    /* Get device port configuration. */
    rv = bcm_esw_port_config_get(unit, &port_conf);
    if (BCM_FAILURE(rv)) {
        PORT_UNLOCK(unit);
        return rv;
    }
    BCM_PBMP_ASSIGN(pbmp, port_conf.e);
    if (SOC_IS_XGS3_SWITCH(unit) || SOC_IS_XGS3_FABRIC(unit)) {
        BCM_PBMP_OR(pbmp, port_conf.cpu);
    }

    if ((port == -1) && (soc_feature(unit, soc_feature_dscp_map_per_port))) {
        PBMP_ITER(pbmp, port) {
            rv = _bcm_port_dscp_map_set(unit, port, srccp, mapcp, prio);
            if (BCM_FAILURE(rv)) {
                PORT_UNLOCK(unit);
                return rv;
            }
        }
    } else {
        if (BCM_PBMP_MEMBER(pbmp, port) || (port == -1)) {
            rv = _bcm_port_dscp_map_set(unit, port, srccp, mapcp, prio);
        } else {
            rv = BCM_E_PORT;
        }
    }
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_port_dscp_map_get
 * Purpose:
 *      Get a mapping for diffserv code points
 * Parameters:
 *      unit - switch device
 *      port - switch port
 *      srccp - src code point or -1
 *      mapcp - (OUT) pointer to returned mapped code point
 *      prio - (OUT) Priority value for mapped code point or -1
 *                      May have BCM_PRIO_DROP_FIRST or'ed into it
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_port_dscp_map_get(int unit, bcm_port_t port, int srccp, int *mapcp,
                      int *prio)
{
    bcm_port_cfg_t      pcfg;

    if (srccp < -1 || srccp > DSCP_CODE_POINT_MAX || 
        mapcp == NULL || prio == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_DRACO15_SUPPORT)
    if (SOC_IS_DRACO15(unit)) {
        int             base;
        dscp_entry_t    de;
        /* look up in DSCPm */
        if (srccp < 0) {
            srccp = 0;
        }
        base = port * DSCP_CODE_POINT_CNT;
        SOC_IF_ERROR_RETURN
            (READ_DSCPm(unit, MEM_BLOCK_ANY, base + srccp, &de));
        *mapcp = soc_DSCPm_field32_get(unit, &de, DSCPf);
        *prio = soc_DSCPm_field32_get(unit, &de, PRIf);
        if (soc_DSCPm_field32_get(unit, &de, CNGf)) {
            *prio |= BCM_PRIO_DROP_FIRST;
        }
        return BCM_E_NONE;
    }
#endif /* BCM_DRACO15_SUPPORT */

#if defined(BCM_TUCANA_SUPPORT)
    if (SOC_IS_TUCANA(unit)) {
        dscp_priority_table_entry_t dpe;

        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
        *mapcp = pcfg.pc_dscp;

        /* Mode 2: look up mapped priority in DSCP_PRIORITY_TABLEm */
        if (srccp < 0) {
            srccp = 0;
        }
        SOC_IF_ERROR_RETURN
            (READ_DSCP_PRIORITY_TABLEm(unit, MEM_BLOCK_ANY,
                                       srccp, &dpe));
        *prio = soc_DSCP_PRIORITY_TABLEm_field32_get(unit, &dpe,
                                                     PRIORITYf);
        if (soc_DSCP_PRIORITY_TABLEm_field32_get(unit, &dpe,
                                                 DROP_PRECEDENCEf)) {
            *prio |= BCM_PRIO_DROP_FIRST;
        }
        return BCM_E_NONE;
    }
#endif /* BCM_TUCANA_SUPPORT */

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        int                     base;
        dscp_table_entry_t      de;

        /* look up in DSCP_TABLEm */
        if (srccp < 0) {
            srccp = 0;
        }
        if (soc_feature(unit, soc_feature_dscp_map_per_port)) {
            base = (port * DSCP_CODE_POINT_CNT);
        } else {
            base = 0;
        }
        SOC_IF_ERROR_RETURN
            (READ_DSCP_TABLEm(unit, MEM_BLOCK_ANY, base + srccp, &de));
        *mapcp = soc_DSCP_TABLEm_field32_get(unit, &de, DSCPf);
        *prio = soc_DSCP_TABLEm_field32_get(unit, &de, PRIf);
        if (soc_DSCP_TABLEm_field32_get(unit, &de, CNGf)) {
            
            *prio |= BCM_PRIO_DROP_FIRST;
        }

        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    /* all other switches */

    pcfg.pc_dse_mode = -1;
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    if (pcfg.pc_dse_mode == -1) {       /* STRATA1 for example */
        return BCM_E_UNAVAIL;
    }

    /*
     * The mapping table is set independently of the mapping mode.
     * So the _get API will return the mapping table setup and not
     * necessarily the actual mapping that will be applied to the packet.
     * The mapping applied to the packet is determined by mapping mode
     * set using bcm_port_dscp_map_mode_set
     */
    *mapcp = pcfg.pc_dscp;

    *prio = -1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_dscp_map_get
 * Purpose:
 *      Get a mapping for diffserv code points
 * Parameters:
 *      unit - switch device
 *      port - switch port
 *      srccp - src code point or -1
 *      mapcp - (OUT) pointer to returned mapped code point
 *      prio - (OUT) Priority value for mapped code point or -1
 *                      May have BCM_PRIO_DROP_FIRST or'ed into it
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_dscp_map_get(int unit, bcm_port_t port, int srccp, int *mapcp,
                      int *prio)
{
    int rv;
    bcm_port_config_t port_conf;
    bcm_pbmp_t pbmp;

    if (!soc_feature(unit, soc_feature_dscp)) {
        return BCM_E_UNAVAIL;
    }

    if (port != -1) {
        if (0 == soc_feature(unit, soc_feature_dscp_map_per_port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    }

    PORT_LOCK(unit);

    /* Get device port configuration. */
    rv = bcm_esw_port_config_get(unit, &port_conf);
    if (BCM_FAILURE(rv)) {
        PORT_UNLOCK(unit);
        return rv;
    }
    BCM_PBMP_ASSIGN(pbmp, port_conf.e);
    if (SOC_IS_XGS3_SWITCH(unit) || SOC_IS_XGS3_FABRIC(unit)) {
        BCM_PBMP_OR(pbmp, port_conf.cpu);
    }

    if (port == -1) {
        PBMP_ITER(pbmp, port) {
            break;
        }
    }

    if (SOC_PORT_VALID(unit, port) && BCM_PBMP_MEMBER(pbmp, port)) {
        rv = _bcm_esw_port_dscp_map_get(unit, port, srccp, mapcp, prio);
    } else {
        rv = BCM_E_PORT;
    }

    PORT_UNLOCK(unit);
    return rv;
}

#undef DSCP_CODE_POINT_MAX
#undef DSCP_CODE_POINT_CNT

STATIC int 
_bcm_esw_port_modid_egress_resolve(int unit, _bcm_port_egr_dest_t *egr_dst)
{
    int             port_adjst = 0;
    bcm_module_t    my_mod;

    if (BCM_GPORT_IS_SET(egr_dst->in_port)) {
        bcm_trunk_t     tid;
        int             id;
        bcm_port_t      port;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, egr_dst->in_port, &my_mod, &port, 
                                   &tid, &id));
        if ((-1 != id) || (-1 != tid)) {
            return BCM_E_PORT;
        }
        egr_dst->out_min_port = egr_dst->out_max_port = port;
        egr_dst->out_min_modid = egr_dst->out_max_modid = my_mod;
    } else {
        if (egr_dst->in_modid < 0) {
            egr_dst->out_min_modid = 0;
            egr_dst->out_max_modid = SOC_MODID_MAX(unit);
        } else if (!SOC_MODID_ADDRESSABLE(unit, egr_dst->in_modid)) {
            return BCM_E_PARAM;
        } else {
            if ((NUM_MODID(unit) == 2) && (SOC_IS_TR_VL(unit)) ) {
                int isLocal;
                BCM_IF_ERROR_RETURN(
                    bcm_esw_stk_my_modid_get(unit, &my_mod));
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_modid_is_local(unit, egr_dst->in_modid, &isLocal));
                if ((TRUE == isLocal) && (my_mod != egr_dst->in_modid)) {
                    egr_dst->in_modid = my_mod;
                    port_adjst = 32;
                }
            }
            egr_dst->out_min_modid = egr_dst->out_max_modid = egr_dst->in_modid;
        }

        if (egr_dst->in_port < 0) {
            egr_dst->out_min_port = 0;
            egr_dst->out_max_port = SOC_PORT_MAX(unit, all);
        } else if (!SOC_PORT_ADDRESSABLE(unit, egr_dst->in_port)) {
            return BCM_E_PORT;
        } else {
            egr_dst->out_min_port = egr_dst->out_max_port = 
                egr_dst->in_port + port_adjst; 
        }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_port_egress_set
 * Description:
 *      Set switching only to indicated ports from given (modid, port).
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port  - ingress port number.
 *      modid - source module ID index (in HiGig mod header).
 *      pbmp - bitmap of ports to allow egress.
 * Returns:
 *      BCM_E_xxxx
 * Note:
 *      if port < 0, means all/any port
 *      if modid < 0, means all/any modid
 */
int
bcm_esw_port_egress_set(int unit, bcm_port_t port, int modid, bcm_pbmp_t pbmp)
{
    egr_mask_entry_t        em_entry;
    int                     em_index;
    bcm_module_t            cur_mod; 
    bcm_pbmp_t              em_pbmp;
    bcm_port_t              cur_port;
    int                     rv = BCM_E_NONE;
    _bcm_port_egr_dest_t    egr_dst;

    if (!SOC_IS_XGS_SWITCH(unit)) {
        if (BCM_PBMP_EQ(pbmp, PBMP_ALL(unit))) {
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    sal_memset(&em_entry, 0, sizeof(egr_mask_entry_t));

    BCM_PBMP_NEGATE(em_pbmp, pbmp);
    BCM_PBMP_AND(em_pbmp, PBMP_ALL(unit));
    BCM_PBMP_REMOVE(em_pbmp, PBMP_LB(unit));
    if (SOC_IS_TUCANA(unit)) {
        soc_EGR_MASKm_field32_set(unit, &em_entry, EGRESS_MASK_M0f,
                                  SOC_PBMP_WORD_GET(em_pbmp, 0));
        soc_EGR_MASKm_field32_set(unit, &em_entry, EGRESS_MASK_M1f,
                                  SOC_PBMP_WORD_GET(em_pbmp, 1));
    } else {
        soc_mem_pbmp_field_set(unit, EGR_MASKm, &em_entry, EGRESS_MASKf,
                               &em_pbmp);
    }
    egr_dst.in_modid = modid;
    egr_dst.in_port = port;

        BCM_IF_ERROR_RETURN(
        _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));
    
    soc_mem_lock(unit, EGR_MASKm);
    for (cur_mod = egr_dst.out_min_modid; cur_mod <= egr_dst.out_max_modid; cur_mod++) {
        for (cur_port = egr_dst.out_min_port; cur_port <= egr_dst.out_max_port; cur_port++) {
                em_index = (cur_mod & SOC_MODID_MAX(unit)) *
                           (SOC_PORT_ADDR_MAX(unit) + 1) |
                           (cur_port & SOC_PORT_ADDR_MAX(unit));

#if defined(BCM_DRACO15_SUPPORT)
            if (SOC_IS_DRACO15(unit)) {
                /* Preserve EGR_MASK T/TGID fields. */
                rv = soc_mem_read(unit, EGR_MASKm, MEM_BLOCK_ANY,
                                  em_index, &em_entry);
                if (!BCM_SUCCESS(rv)) {
                    break;
                }
                soc_EGR_MASKm_field32_set(unit, &em_entry, EGRESS_MASKf,
                                          SOC_PBMP_WORD_GET(em_pbmp, 0));
            }
#endif /* BCM_DRACO15_SUPPORT */
            rv = soc_mem_write(unit, EGR_MASKm, MEM_BLOCK_ALL,
                               em_index, &em_entry);
            if (!BCM_SUCCESS(rv)) {
                break;
            }
        }
        if (!BCM_SUCCESS(rv)) {
            break;
        }
    }
    soc_mem_unlock(unit, EGR_MASKm);

    return rv;
}

/*
 * Function:
 *      bcm_port_egress_get
 * Description:
 *      Retrieve bitmap of ports for which switching is enabled
 *      for (modid, port).
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port  - ingress port number.
 *      modid - source module ID index (in HiGig mod header).
 *      pbmp - (OUT) bitmap of ports where egress allowed.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_esw_port_egress_get(int unit, bcm_port_t port, int modid, bcm_pbmp_t *pbmp)
{
    egr_mask_entry_t    em_entry;
    int                 em_index;
    bcm_pbmp_t          em_pbmp, temp_pbmp;
    _bcm_port_egr_dest_t    egr_dst;

    if (!SOC_IS_XGS_SWITCH(unit)) {
        BCM_PBMP_ASSIGN(*pbmp, PBMP_ALL(unit));
        return BCM_E_NONE;
    }

    if ((modid < 0) || (port < 0) ){
            return BCM_E_PARAM;
        }

    egr_dst.in_modid = modid;
    egr_dst.in_port = port;

            BCM_IF_ERROR_RETURN(
        _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        em_index =
        (egr_dst.out_min_modid & SOC_MODID_MAX(unit)) * 
        (SOC_PORT_ADDR_MAX(unit) + 1) | (egr_dst.out_min_port 
                                         & SOC_PORT_ADDR_MAX(unit));

    BCM_IF_ERROR_RETURN
        (READ_EGR_MASKm(unit, MEM_BLOCK_ALL, em_index, &em_entry));

    BCM_PBMP_CLEAR(em_pbmp);
    if (SOC_IS_TUCANA(unit)) {
        SOC_PBMP_WORD_SET(em_pbmp, 0,
                          soc_EGR_MASKm_field32_get(unit, &em_entry,
                                                    EGRESS_MASK_M0f));
        SOC_PBMP_WORD_SET(em_pbmp, 1,
                          soc_EGR_MASKm_field32_get(unit, &em_entry,
                                                    EGRESS_MASK_M1f));
    } else {
        soc_mem_pbmp_field_get(unit, EGR_MASKm, &em_entry, EGRESS_MASKf,
                               &em_pbmp);
    }
    BCM_PBMP_NEGATE(temp_pbmp, em_pbmp);
    BCM_PBMP_AND(temp_pbmp, PBMP_ALL(unit));
    BCM_PBMP_REMOVE(temp_pbmp, PBMP_LB(unit));
    BCM_PBMP_ASSIGN(*pbmp, temp_pbmp);

    return BCM_E_NONE;
}

#ifdef BCM_TRIUMPH2_SUPPORT
STATIC int
_bcm_port_src_mod_egress_profile_remove(int unit, bcm_port_t port, 
                                        int min_mod, int max_mod)
{
    int idx, i, region, modid_cnt;
    uint32 val;
    src_modid_egress_entry_t srcmodegr;

    BCM_IF_ERROR_RETURN(READ_SRC_MODID_EGRESS_SELr(unit, port, &val));
    soc_reg_field_set(unit, SRC_MODID_EGRESS_SELr, &val, ENABLEf, 0); 
    soc_reg_field_set(unit, SRC_MODID_EGRESS_SELr, &val, SRCMOD_INDEXf, 0);
    BCM_IF_ERROR_RETURN(WRITE_SRC_MODID_EGRESS_SELr(unit, port, val));

    /* Null out the egress pbmp from the profile region */
    region = PORT_SRC_MOD_EGR_PROF_PTR(unit, port);
    SRC_MOD_EGR_REF_COUNT(unit, region)--;
    modid_cnt = SOC_MODID_MAX(unit) + 1;
    PORT_SRC_MOD_EGR_PROF_PTR(unit, port) = -1;

    if (SRC_MOD_EGR_REF_COUNT(unit, region) == 0) {
        for (i = min_mod; i <= max_mod; i++) {
            idx = region * modid_cnt + i;
            sal_memset(&srcmodegr, 0, sizeof(src_modid_egress_entry_t));
            BCM_IF_ERROR_RETURN(
                WRITE_SRC_MODID_EGRESSm(unit, MEM_BLOCK_ALL, idx, &srcmodegr));
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_port_src_mod_egress_profile_add(int unit, bcm_port_t port, 
                                     int min_mod, int max_mod, pbmp_t pbmp)
{
    int idx, i, j, num_regions, region, modid_cnt, match, rv = BCM_E_NONE;
    uint32 val, pword0, pword1;
    src_modid_egress_entry_t srcmodegr;
    modid_cnt = SOC_MODID_MAX(unit) + 1;
    region = PORT_SRC_MOD_EGR_PROF_PTR(unit, port);
    if (region >= 0) {
        /* Profile for this port already exists - update it */
        for (i = min_mod; i < max_mod; i++) {
            idx = region * modid_cnt + i;
            sal_memset(&srcmodegr, 0, sizeof(src_modid_egress_entry_t));
            soc_SRC_MODID_EGRESSm_field32_set(unit, &srcmodegr, 
                PORT_BLOCK_MASK_BITMAP_LOf, SOC_PBMP_WORD_GET(pbmp, 0));
            soc_SRC_MODID_EGRESSm_field32_set(unit, &srcmodegr, 
                PORT_BLOCK_MASK_BITMAP_HIf, SOC_PBMP_WORD_GET(pbmp, 1));
            BCM_IF_ERROR_RETURN(
                WRITE_SRC_MODID_EGRESSm(unit, MEM_BLOCK_ALL, idx, &srcmodegr));
        }

    } else {
        /* Allocate a new / find existing profile */
        num_regions = soc_mem_index_count(unit, SRC_MODID_EGRESSm) / 
                      SOC_MODID_MAX(unit);
        for (j = 0; j < num_regions; j++) {
            match = 1;
            for (i = min_mod; i <= max_mod; i++) {
                BCM_IF_ERROR_RETURN(READ_SRC_MODID_EGRESSm(unit, MEM_BLOCK_ANY, 
                                                           j * modid_cnt + i, 
                                                           &srcmodegr));
                pword0 = soc_SRC_MODID_EGRESSm_field32_get
                             (unit, &srcmodegr, PORT_BLOCK_MASK_BITMAP_LOf);
                pword1 = soc_SRC_MODID_EGRESSm_field32_get
                             (unit, &srcmodegr, PORT_BLOCK_MASK_BITMAP_HIf);
                if (pword0 != SOC_PBMP_WORD_GET(pbmp, 0) ||
                    pword1 != SOC_PBMP_WORD_GET(pbmp, 1)) {
                    match = 0;
                    break;
                }                            
            }
            if (match) {
                BCM_IF_ERROR_RETURN(READ_SRC_MODID_EGRESS_SELr(unit, port, &val));
                soc_reg_field_set(unit, SRC_MODID_EGRESS_SELr, &val, ENABLEf, 1); 
                soc_reg_field_set(unit, SRC_MODID_EGRESS_SELr, &val, SRCMOD_INDEXf, j);
                BCM_IF_ERROR_RETURN(WRITE_SRC_MODID_EGRESS_SELr(unit, port, val));

                PORT_SRC_MOD_EGR_PROF_PTR(unit, port) = j;
                SRC_MOD_EGR_REF_COUNT(unit, j)++;
                break;
            }
            if (SRC_MOD_EGR_REF_COUNT(unit, j) == 0) {
                for (i = min_mod; i <= max_mod; i++) {
                    sal_memset(&srcmodegr, 0, sizeof(src_modid_egress_entry_t));
                    soc_SRC_MODID_EGRESSm_field32_set(unit, &srcmodegr, 
                        PORT_BLOCK_MASK_BITMAP_LOf, SOC_PBMP_WORD_GET(pbmp, 0));
                    soc_SRC_MODID_EGRESSm_field32_set(unit, &srcmodegr, 
                        PORT_BLOCK_MASK_BITMAP_HIf, SOC_PBMP_WORD_GET(pbmp, 1));
                    BCM_IF_ERROR_RETURN
                        (WRITE_SRC_MODID_EGRESSm(unit, MEM_BLOCK_ALL, j *   
                                                 modid_cnt + i, &srcmodegr));
                }
                BCM_IF_ERROR_RETURN(READ_SRC_MODID_EGRESS_SELr(unit, port, &val));
                soc_reg_field_set(unit, SRC_MODID_EGRESS_SELr, &val, ENABLEf, 1); 
                soc_reg_field_set(unit, SRC_MODID_EGRESS_SELr, &val, SRCMOD_INDEXf, j);
                BCM_IF_ERROR_RETURN(WRITE_SRC_MODID_EGRESS_SELr(unit, port, val));

                PORT_SRC_MOD_EGR_PROF_PTR(unit, port) = j;
                SRC_MOD_EGR_REF_COUNT(unit, j)++;
                break;
            }
        }
        if (j == num_regions) {
            rv = BCM_E_RESOURCE;
        }
    }
    return rv;
}
#endif

/*
 * Function:
 *      bcm_port_modid_egress_set
 * Description:
 *      Set port bitmap of egress ports on which the incoming packets
 *      will be forwarded.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port  - ingress port number.
 *      modid - source module ID index (in HiGig mod header).
 *      pbmp - bitmap of ports to allow egress.
 * Returns:
 *      BCM_E_xxxx
 * Note:
 *      if port < 0, means all/any port
 *      if modid < 0, means all/any modid
 */
int
bcm_esw_port_modid_egress_set(int unit, bcm_port_t port,
			  bcm_module_t modid, bcm_pbmp_t pbmp)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_UNAVAIL;

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_ing_srcmodblk_entry_t srcmod;
        uint32 smf;
        bcm_module_t mod_idx, min_mod, max_mod;
        bcm_pbmp_t mod_pbmp;
        int bk=-1, blk;

        if (port >= 0) {
            if (!SOC_PORT_VALID(unit, port) || !IS_HG_PORT(unit, port)) {
                return BCM_E_PORT;
            }
            bk = SOC_PORT_BLOCK(unit, port);
        }

        if (modid < 0) {
            min_mod = soc_mem_index_min(unit, MEM_ING_SRCMODBLKm);
            max_mod = soc_mem_index_max(unit, MEM_ING_SRCMODBLKm);
        } else if (modid > soc_mem_index_max(unit, MEM_ING_SRCMODBLKm)) {
            return BCM_E_PARAM;
        } else {
            min_mod = max_mod = modid;
        }

        BCM_PBMP_ASSIGN(mod_pbmp, pbmp);
        BCM_PBMP_AND(mod_pbmp, PBMP_ALL(unit));
        if (BCM_PBMP_NEQ(pbmp, mod_pbmp)) {
            return BCM_E_PARAM;
        }

        soc_mem_lock(unit, MEM_ING_SRCMODBLKm);
        SOC_MEM_BLOCK_ITER(unit, MEM_ING_SRCMODBLKm, blk) {
            if ((port >= 0) && (bk !=blk)) {
                continue;
            }
            for (mod_idx = min_mod; mod_idx <= max_mod; mod_idx++) {
                 sal_memset(&srcmod, 0, sizeof(srcmod));
                 rv = READ_MEM_ING_SRCMODBLKm(unit, blk, mod_idx, &srcmod);
                 if (rv >= 0) {
                     SOC_PBMP_WORD_SET(mod_pbmp, 0,
                         soc_MEM_ING_SRCMODBLKm_field32_get(unit, &srcmod, BITMAPf));
                     if (BCM_PBMP_NEQ(pbmp, mod_pbmp)) {
                         smf = SOC_PBMP_WORD_GET(pbmp, 0);
                         soc_MEM_ING_SRCMODBLKm_field_set(unit, &srcmod, BITMAPf, &smf);
                         rv = WRITE_MEM_ING_SRCMODBLKm(unit, blk, mod_idx, &srcmod);
                     }
                 }
            }
        }
        soc_mem_unlock(unit, MEM_ING_SRCMODBLKm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        src_modid_ingress_block_entry_t srcmoding;
        _bcm_port_egr_dest_t egr_dst;
        int all_st_ports;
        int modid_cnt, i;
        bcm_port_t p;
        pbmp_t mod_pbmp;
        uint32 pword0, pword1;

        /* Number of supported MODIDs for this unit */
        modid_cnt = SOC_MODID_MAX(unit) + 1;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_min_port == egr_dst.out_max_port) {
            all_st_ports = 0;
        } else {
            all_st_ports = 1;
        }

        /* First program the ingress blocking */
        soc_mem_lock(unit, SRC_MODID_INGRESS_BLOCKm);
        for (i = egr_dst.out_min_modid; i <= egr_dst.out_max_modid; i++) { 
            rv = READ_SRC_MODID_INGRESS_BLOCKm(unit, MEM_BLOCK_ANY, i, 
                                               &srcmoding);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, SRC_MODID_INGRESS_BLOCKm);
                return rv;
            }
            if (!all_st_ports) {
                if (egr_dst.out_min_port < 32) {
                    soc_SRC_MODID_INGRESS_BLOCKm_field32_set
                        (unit, &srcmoding, PORT_BITMAP_LOf, (1 << egr_dst.out_min_port));
                } else {
                    soc_SRC_MODID_INGRESS_BLOCKm_field32_set
                        (unit, &srcmoding, PORT_BITMAP_HIf, (1 << (egr_dst.out_min_port - 32)));
                } 
            } else {
                pword0 = pword1 = 0;
                PBMP_ST_ITER(unit, p) {
                    if (p < 32) {
                        pword0 |= (1 << p);
                    } else {
                        pword1 |= (1 << (p - 32));
                    } 
                }
                soc_SRC_MODID_INGRESS_BLOCKm_field32_set
                    (unit, &srcmoding, PORT_BITMAP_LOf, pword0);
                soc_SRC_MODID_INGRESS_BLOCKm_field32_set
                    (unit, &srcmoding, PORT_BITMAP_HIf, pword1);
            }
            rv = WRITE_SRC_MODID_INGRESS_BLOCKm(unit, MEM_BLOCK_ALL, i, 
                                                &srcmoding);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, SRC_MODID_INGRESS_BLOCKm);
                return rv;
            }
        } 
        soc_mem_unlock(unit, SRC_MODID_INGRESS_BLOCKm);

        /* Deal with the egress blocking - use one of 8 profiles */
        BCM_PBMP_ASSIGN(mod_pbmp, pbmp);
        BCM_PBMP_AND(mod_pbmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(mod_pbmp, PBMP_LB(unit));
        if (BCM_PBMP_NEQ(pbmp, mod_pbmp)) {
            return BCM_E_PARAM;
        }

        BCM_PBMP_NEGATE(mod_pbmp, pbmp);
        BCM_PBMP_AND(mod_pbmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(mod_pbmp, PBMP_LB(unit));

        soc_mem_lock(unit, SRC_MODID_EGRESSm);
        if (!all_st_ports) {
            /* Deal with a single stack port and its profile */
            if (BCM_PBMP_IS_NULL(mod_pbmp)) {
                rv = _bcm_port_src_mod_egress_profile_remove(unit, 
                                                      egr_dst.out_min_port, 
                                                      egr_dst.out_min_modid, 
                                                      egr_dst.out_max_modid);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, SRC_MODID_EGRESSm);
                    return rv;
                }
            } else {
                rv = _bcm_port_src_mod_egress_profile_add(unit,
                                                       egr_dst.out_min_port,
                                                       egr_dst.out_min_modid, 
                                                       egr_dst.out_max_modid,
                                                       mod_pbmp);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, SRC_MODID_EGRESSm);
                    return rv;
                }
            }
        } else {
            /* Deal with all stack ports and their profiles */
            PBMP_ST_ITER(unit, p) {
                if (BCM_PBMP_IS_NULL(mod_pbmp)) {
                    rv = _bcm_port_src_mod_egress_profile_remove
                             (unit, p, egr_dst.out_min_modid, egr_dst.out_max_modid);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, SRC_MODID_EGRESSm);
                        return rv;
                    }
                } else {
                    rv = _bcm_port_src_mod_egress_profile_add
                             (unit, p, egr_dst.out_min_modid, egr_dst.out_max_modid, mod_pbmp);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, SRC_MODID_EGRESSm);
                        return rv;
                    }
                }
            }
        }
        soc_mem_unlock(unit, SRC_MODID_EGRESSm);

        return rv;
    }
#endif

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        src_modid_block_entry_t srcmod;
        int modid_cnt, idx, i, j;
        bcm_pbmp_t mod_pbmp;
        _bcm_port_egr_dest_t    egr_dst;

        /* Number of supported MODIDs for this unit */
        modid_cnt = SOC_MODID_MAX(unit) + 1;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_min_port == egr_dst.out_max_port) {
            SOC_IF_ERROR_RETURN(
                soc_xgs3_port_num_to_higig_port_num(unit, egr_dst.out_max_port,
                                                    &egr_dst.out_min_port));
            egr_dst.out_max_port = egr_dst.out_min_port;
        } else {
            egr_dst.out_min_port = soc_mem_index_min(unit, SRC_MODID_BLOCKm) / modid_cnt;
            egr_dst.out_max_port = soc_mem_index_max(unit, SRC_MODID_BLOCKm) / modid_cnt;
        }

        BCM_PBMP_ASSIGN(mod_pbmp, pbmp);
        BCM_PBMP_AND(mod_pbmp, PBMP_ALL(unit));
        if (BCM_PBMP_NEQ(pbmp, mod_pbmp)) {
            return BCM_E_PARAM;
        }

        BCM_PBMP_NEGATE(mod_pbmp, pbmp);
        BCM_PBMP_AND(mod_pbmp, PBMP_ALL(unit));

        soc_mem_lock(unit, SRC_MODID_BLOCKm);
        for (i = egr_dst.out_min_port; i <= egr_dst.out_max_port; i++) {
            for (j = egr_dst.out_min_modid; j <= egr_dst.out_max_modid; j++) {
                idx = i * modid_cnt + j;
                rv = READ_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ANY, idx, &srcmod);
                if (rv >= 0) {
                    soc_SRC_MODID_BLOCKm_field_set(unit, &srcmod,
                                                   PORT_BLOCK_MASK_BITMAPf,
                                                   (uint32 *)&mod_pbmp);
                    rv = WRITE_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ALL, idx,
                                                &srcmod);
                }
                if (rv < 0) {
                    soc_mem_unlock(unit, SRC_MODID_BLOCKm);
                    return rv;
                }
            }
        }
        soc_mem_unlock(unit, SRC_MODID_BLOCKm);
        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (soc_feature(unit, soc_feature_src_modid_blk)) {
        src_modid_block_entry_t srcmod;
        bcm_module_t mod_idx;
        int p_idx, min_p, max_p;
        int tbl_size, modid_cnt, idx;
        bcm_pbmp_t mod_pbmp, tst_pbmp, hg_pbmp;
        int cpu=0, ether=0;
        uint32 xge_val;
        _bcm_port_egr_dest_t    egr_dst; 
#if defined(BCM_RAPTOR_SUPPORT)
        uint32  rp_val;
#endif /* BCM_RAPTOR_SUPPORT */
        /* Number of supported MODIDs for this unit */
        modid_cnt = SOC_MODID_MAX(unit) + 1;


        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_min_port == egr_dst.out_min_port) {
            min_p = max_p = egr_dst.out_min_port - SOC_HG_OFFSET(unit);
        } else {
            min_p = 0;
            tbl_size = soc_mem_index_count(unit, SRC_MODID_BLOCKm);
            max_p = (tbl_size / modid_cnt) - 1;
            assert(max_p >= 0);
        }

        BCM_PBMP_ASSIGN(mod_pbmp, pbmp);
        BCM_PBMP_AND(mod_pbmp, PBMP_ALL(unit));
        if (BCM_PBMP_NEQ(pbmp, mod_pbmp)) {
            return BCM_E_PARAM;
        }
    
        BCM_PBMP_NEGATE(mod_pbmp, pbmp);
        BCM_PBMP_AND(mod_pbmp, PBMP_ALL(unit));

        BCM_PBMP_ASSIGN(tst_pbmp, mod_pbmp);
        BCM_PBMP_AND(tst_pbmp, PBMP_CMIC(unit));
        if (BCM_PBMP_NOT_NULL(tst_pbmp)) {
            cpu = 1;
        }

        BCM_PBMP_ASSIGN(tst_pbmp, mod_pbmp);
        BCM_PBMP_AND(tst_pbmp, PBMP_E_ALL(unit));
        if (BCM_PBMP_NOT_NULL(tst_pbmp)) {
            ether = 1;
            BCM_PBMP_ASSIGN(tst_pbmp, PBMP_XE_ALL(unit));
        }

        BCM_PBMP_ASSIGN(hg_pbmp, mod_pbmp);
        BCM_PBMP_AND(hg_pbmp, PBMP_ST_ALL(unit));

        if (ether && BCM_PBMP_NOT_NULL(tst_pbmp)) {
            BCM_PBMP_OR(hg_pbmp, tst_pbmp);
        }
        xge_val = SOC_PBMP_WORD_GET(hg_pbmp, 0) >> SOC_HG_OFFSET(unit);

        /* In Raven, port #3 is the GMII port and it introduces a hole in the ST pbmp */ 
        soc_mem_lock(unit, SRC_MODID_BLOCKm);
        for (p_idx = min_p; p_idx <= max_p; p_idx++) {
            if (SOC_IS_RAVEN(unit) && p_idx == 3) {
                continue;
            }
            for (mod_idx = egr_dst.out_min_modid; mod_idx <= egr_dst.out_max_modid; mod_idx++) {
                 sal_memset(&srcmod, 0, sizeof(srcmod));
                 /* Table is 2-dimensional array */
                 if (SOC_IS_RAVEN(unit) && p_idx >= 4) {
                     idx = (p_idx - 1) * modid_cnt + mod_idx;
                 } else {
                     idx = (p_idx * modid_cnt) + mod_idx;
                 }
                 rv = READ_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ANY, idx, &srcmod);
                 if (rv >= 0) {
                     soc_SRC_MODID_BLOCKm_field32_set(unit, &srcmod, 
                                                      CPU_PORT_BLOCK_MASKf, 
                                                      (cpu) ? 1 : 0);
                     if (SOC_IS_FB_FX_HX(unit)) {
                         soc_SRC_MODID_BLOCKm_field32_set(unit, &srcmod, 
                                                          GE_PORT_BLOCK_MASKf,
                                                          (ether) ? 1 : 0);
                     }
                     soc_SRC_MODID_BLOCKm_field32_set(unit, &srcmod, 
                                                      HIGIG_XGE_PORT_BLOCK_MASKf, 
                                                      xge_val);
#if defined(BCM_RAPTOR_SUPPORT)
                     if (soc_mem_field_valid(unit, SRC_MODID_BLOCKm, 
                                             PORT_0_5_BLOCK_MASKf)) {
                          rp_val = (SOC_PBMP_WORD_GET(mod_pbmp, 0)) & 
                              ((1 << soc_reg_field_length(unit, SRC_MODID_BLOCKm,
                                                   PORT_0_5_BLOCK_MASKf )) - 1);
                          soc_SRC_MODID_BLOCKm_field32_set(unit, &srcmod, 
                                                           PORT_0_5_BLOCK_MASKf, 
                                                           rp_val);
                     }
#endif /* BCM_RAPTOR_SUPPORT */
                     rv = WRITE_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ALL, 
                                                 idx, &srcmod);
                 }
            }
        }
        soc_mem_unlock(unit, SRC_MODID_BLOCKm);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_port_modid_egress_get
 * Description:
 *      Retrieve port bitmap of egress ports on which the incoming packets
 *      will be forwarded.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port  - ingress port number.
 *      modid - source module ID index (in HiGig mod header).
 *      pbmp - (OUT) bitmap of ports where egress allowed.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_esw_port_modid_egress_get(int unit, bcm_port_t port,
			  bcm_module_t modid, bcm_pbmp_t *pbmp)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_UNAVAIL;

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_ing_srcmodblk_entry_t srcmod;
        bcm_module_t min_mod, max_mod;
        int blk, rv=BCM_E_NONE;

        if (!SOC_PORT_VALID(unit, port) || !IS_HG_PORT(unit, port)) {
            return BCM_E_PORT;
        }

        min_mod = soc_mem_index_min(unit, MEM_ING_SRCMODBLKm);
        max_mod = soc_mem_index_max(unit, MEM_ING_SRCMODBLKm);
        if ((modid < min_mod) || (modid > max_mod))  {
            return BCM_E_PARAM;
        }

        blk = SOC_PORT_BLOCK(unit, port);
        SOC_PBMP_CLEAR(*pbmp);
        rv = READ_MEM_ING_SRCMODBLKm(unit, blk, modid, &srcmod);
        if (rv >= 0) {
            SOC_PBMP_WORD_SET(*pbmp, 0,
                soc_MEM_ING_SRCMODBLKm_field32_get(unit, &srcmod, BITMAPf));
        }
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        src_modid_egress_entry_t srcmodegr;
        int idx;
        uint32 pword0, pword1;
        _bcm_port_egr_dest_t    egr_dst;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_min_modid != egr_dst.out_max_modid) {
            return BCM_E_PARAM;
        }
        if (egr_dst.out_max_port != egr_dst.out_min_port) {
            return BCM_E_PORT;
        }

        idx = PORT_SRC_MOD_EGR_PROF_PTR(unit, egr_dst.out_min_port) *
              (SOC_MODID_MAX(unit) + 1) + egr_dst.out_min_modid;
        SOC_IF_ERROR_RETURN(READ_SRC_MODID_EGRESSm(unit, MEM_BLOCK_ANY, idx,
                                                   &srcmodegr));
        pword0 = soc_SRC_MODID_EGRESSm_field32_get
                     (unit, &srcmodegr, PORT_BLOCK_MASK_BITMAP_LOf);
        pword1 = soc_SRC_MODID_EGRESSm_field32_get
                     (unit, &srcmodegr, PORT_BLOCK_MASK_BITMAP_HIf); 
        SOC_PBMP_WORD_SET(*pbmp, 0, pword0); 
        SOC_PBMP_WORD_SET(*pbmp, 1, pword1);    
        BCM_PBMP_NEGATE(*pbmp, *pbmp);
        BCM_PBMP_AND(*pbmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(*pbmp, PBMP_LB(unit));
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        src_modid_block_entry_t srcmod;
        int hg_port, idx;
        _bcm_port_egr_dest_t    egr_dst;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_min_modid != egr_dst.out_max_modid) {
            return BCM_E_PARAM;
        }
        if (egr_dst.out_max_port != egr_dst.out_min_port) {
            return BCM_E_PORT;
        }

        SOC_IF_ERROR_RETURN(
            soc_xgs3_port_num_to_higig_port_num(unit, egr_dst.out_min_port, 
                                                &hg_port));
        idx = hg_port * (SOC_MODID_MAX(unit) + 1) + egr_dst.out_min_modid;
        SOC_IF_ERROR_RETURN(READ_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ANY, idx,
                                                  &srcmod));
        soc_SRC_MODID_BLOCKm_field_get(unit, &srcmod, PORT_BLOCK_MASK_BITMAPf,
                                       (uint32 *)pbmp);
        BCM_PBMP_NEGATE(*pbmp, *pbmp);
        BCM_PBMP_AND(*pbmp, PBMP_ALL(unit));

        return SOC_E_NONE;
    }
#endif

#ifdef BCM_FIREBOLT_SUPPORT
    if (soc_feature(unit, soc_feature_src_modid_blk)) {
        src_modid_block_entry_t srcmod;
        int modid_cnt, p_idx, idx;
        _bcm_port_egr_dest_t    egr_dst;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_min_modid != egr_dst.out_max_modid) {
            return BCM_E_PARAM;
        }
        if (egr_dst.out_max_port != egr_dst.out_min_port) {
            return BCM_E_PORT;
        }

        modid_cnt = SOC_MODID_MAX(unit) + 1;
        p_idx = egr_dst.out_min_port - SOC_HG_OFFSET(unit);
        if (SOC_IS_RAVEN(unit) && p_idx >= 4) {
            idx = (p_idx - 1) * modid_cnt + egr_dst.out_min_modid;
        } else {
            idx = (p_idx * modid_cnt) + egr_dst.out_min_modid;
        }

        rv = READ_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ANY, idx, &srcmod);
        BCM_PBMP_CLEAR(*pbmp);
        if (rv >= 0) {
            int cpu, ether;
            pbmp_t hg_pbmp;
            uint32 xge_val;
#if defined(BCM_RAPTOR_SUPPORT)
            uint32 rp_val;
#endif /* BCM_RAPTOR_SUPPORT */
            cpu = soc_SRC_MODID_BLOCKm_field32_get(unit, &srcmod, 
                                                   CPU_PORT_BLOCK_MASKf);
            if (SOC_IS_FB_FX_HX(unit)) {
                ether = soc_SRC_MODID_BLOCKm_field32_get(unit, &srcmod, 
                                                         GE_PORT_BLOCK_MASKf);
            } else {
                ether = 0;
            }
            xge_val = soc_SRC_MODID_BLOCKm_field32_get(unit, &srcmod, 
                                                       HIGIG_XGE_PORT_BLOCK_MASKf);
            SOC_PBMP_CLEAR(hg_pbmp);
            SOC_PBMP_WORD_SET(hg_pbmp, 0, xge_val << SOC_HG_OFFSET(unit));

#if defined(BCM_RAPTOR_SUPPORT)
            if (soc_mem_field_valid(unit, SRC_MODID_BLOCKm, 
                                    PORT_0_5_BLOCK_MASKf)) {
                rp_val = soc_SRC_MODID_BLOCKm_field32_get(unit, &srcmod, 
                                                         PORT_0_5_BLOCK_MASKf);
                SOC_PBMP_WORD_SET(hg_pbmp, 0, rp_val);
            }
#endif /* BCM_RAPTOR_SUPPORT */
            BCM_PBMP_ASSIGN(*pbmp, PBMP_ALL(unit));
            if (cpu) {
                BCM_PBMP_REMOVE(*pbmp, PBMP_CMIC(unit));
            }
            if (ether) {
                BCM_PBMP_REMOVE(*pbmp, PBMP_E_ALL(unit)); 
            }
            if (BCM_PBMP_NOT_NULL(hg_pbmp)) {
                BCM_PBMP_REMOVE(*pbmp, hg_pbmp);
            }
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *    bcm_port_modid_enable_set
 * Purpose:
 *    Enable/block packets from a specific module on a port.
 * Parameters:
 *    unit - StrataSwitch PCI device unit number (driver internal).
 *    port - StrataSwitch port number.
 *    modid - Which source module id to enable/disable
 *    enable - TRUE/FALSE Enable/disable forwarding packets from
 *             source module arriving on port.
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_port_modid_enable_set(int unit, bcm_port_t port, int modid, int enable)
{
    int max_mod;
#ifdef BCM_HERCULES_SUPPORT
    uint32 smf, osmf;
#endif /* BCM_HERCULES_SUPPORT */
#ifdef BCM_HERCULES1_SUPPORT
    uint32 mask;
#endif /* BCM_HERCULES1_SUPPORT */
#ifdef BCM_HERCULES15_SUPPORT
    mem_ing_srcmodblk_entry_t srcmod;
    int blk;
#endif /* BCM_HERCULES15_SUPPORT */
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;
#endif /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        src_modid_block_entry_t srcmod;
        int modid_cnt, hg_port, idx, i;
        bcm_pbmp_t pbmp;
        _bcm_port_egr_dest_t    egr_dst;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_max_port != egr_dst.out_min_port) {
            return BCM_E_PORT;
        }

        if (enable) {
            BCM_PBMP_CLEAR(pbmp);
        } else {
            BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
            BCM_PBMP_REMOVE(pbmp, PBMP_LB(unit));
        }

        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            soc_mem_lock(unit, SRC_MODID_INGRESS_BLOCKm);
            for (i = egr_dst.out_min_modid; i <= egr_dst.out_max_modid; i++) {
                rv = READ_SRC_MODID_INGRESS_BLOCKm(unit, MEM_BLOCK_ANY, i, &srcmod);
                if (rv >= 0) {
#ifdef LVL7_FIXUP
                  SOC_PBMP_WORD_SET(pbmp, 0, soc_SRC_MODID_INGRESS_BLOCKm_field32_get(unit, &srcmod, PORT_BITMAP_LOf));
                  SOC_PBMP_WORD_SET(pbmp, 1, soc_SRC_MODID_INGRESS_BLOCKm_field32_get(unit, &srcmod, PORT_BITMAP_HIf));
                  if (enable)
                  {
                    BCM_PBMP_PORT_REMOVE(pbmp, port);
                  }
                  else
                  {
                    BCM_PBMP_PORT_ADD(pbmp, port);
                  }
#endif
                    soc_SRC_MODID_INGRESS_BLOCKm_field32_set(unit, &srcmod,
                                                   PORT_BITMAP_LOf,
                                                   SOC_PBMP_WORD_GET(pbmp, 0));
                    soc_SRC_MODID_INGRESS_BLOCKm_field32_set(unit, &srcmod,
                                                   PORT_BITMAP_HIf,
                                                   SOC_PBMP_WORD_GET(pbmp, 1));
                    rv = WRITE_SRC_MODID_INGRESS_BLOCKm(unit, MEM_BLOCK_ALL, i, &srcmod);
                }
                if (rv < 0) {
                    soc_mem_unlock(unit, SRC_MODID_INGRESS_BLOCKm);
                    return rv;
                }
            }
            soc_mem_unlock(unit, SRC_MODID_INGRESS_BLOCKm);
        } else {
            modid_cnt = SOC_MODID_MAX(unit) + 1;
            SOC_IF_ERROR_RETURN(
                soc_xgs3_port_num_to_higig_port_num(unit, port, &hg_port));

            soc_mem_lock(unit, SRC_MODID_BLOCKm);
            for (i = egr_dst.out_min_modid; i <= egr_dst.out_max_modid; i++) {
                idx = hg_port * modid_cnt + i;
                rv = READ_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ANY, idx, &srcmod);
                if (rv >= 0) {
                    soc_SRC_MODID_BLOCKm_field_set(unit, &srcmod,
                                                   PORT_BLOCK_MASK_BITMAPf,
                                                   (uint32 *)&pbmp);
                    rv = WRITE_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ALL, idx, &srcmod);
                }
                if (rv < 0) {
                    soc_mem_unlock(unit, SRC_MODID_BLOCKm);
                    return rv;
                }
            }
            soc_mem_unlock(unit, SRC_MODID_BLOCKm);
        }
        return rv;
    }
#endif  /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (soc_feature(unit, soc_feature_src_modid_blk)) {
        src_modid_block_entry_t src_mod;
        int modid_cnt, mod_idx, p_idx, idx, val;
        _bcm_port_egr_dest_t    egr_dst;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_max_port != egr_dst.out_min_port) {
            return BCM_E_PORT;
        }

        modid_cnt = SOC_MODID_MAX(unit) + 1;
        p_idx = egr_dst.out_min_port - SOC_HG_OFFSET(unit);
        soc_mem_lock(unit, SRC_MODID_BLOCKm);
        for (mod_idx = egr_dst.out_min_modid; mod_idx <= egr_dst.out_max_modid; mod_idx++) {
             sal_memset(&src_mod, 0, sizeof(src_mod));
             if (SOC_IS_RAVEN(unit) && p_idx >= 4) {
                 idx = (p_idx - 1) * modid_cnt + mod_idx;
             } else {
                 idx = (p_idx * modid_cnt) + mod_idx;
             }
             rv = READ_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ANY, idx, &src_mod);
             if (rv >= 0) {
                 soc_SRC_MODID_BLOCKm_field32_set(unit, &src_mod,
                                                  CPU_PORT_BLOCK_MASKf,
                                                  (enable) ? 0 : 1);
                 if (SOC_IS_FB_FX_HX(unit)) {
                     soc_SRC_MODID_BLOCKm_field32_set(unit, &src_mod,
                                                      GE_PORT_BLOCK_MASKf,
                                                      (enable) ? 0 : 1);
                 }

                 val = (1 << soc_mem_field_length(unit, SRC_MODID_BLOCKm,
                                             HIGIG_XGE_PORT_BLOCK_MASKf)) - 1;
                 
                 soc_SRC_MODID_BLOCKm_field32_set(unit, &src_mod,
                                                  HIGIG_XGE_PORT_BLOCK_MASKf,
                                                  (enable) ? 0 : val);
#if defined(BCM_RAPTOR_SUPPORT)
                 if (soc_mem_field_valid(unit, SRC_MODID_BLOCKm, 
                                         PORT_0_5_BLOCK_MASKf)) {

                     val = (1 << soc_mem_field_length(unit, SRC_MODID_BLOCKm,
                                                 PORT_0_5_BLOCK_MASKf)) - 1;

                     soc_SRC_MODID_BLOCKm_field32_set(unit, &src_mod,
                                                      PORT_0_5_BLOCK_MASKf,
                                                      (enable) ? 0 : val);

                 }
#endif /* BCM_RAPTOR_SUPPORT */
                 rv = WRITE_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ALL,
                                             idx, &src_mod);
             }
        }
        soc_mem_unlock(unit, SRC_MODID_BLOCKm);

        return rv;
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    if (!IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if (!soc_feature(unit, soc_feature_srcmod_filter)) {
        if (enable) {
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    max_mod = SOC_IS_HERCULES1(unit) ? 32 : 128;
    if (modid >= max_mod) {
        return BCM_E_PARAM;
    }

#ifdef BCM_HERCULES1_SUPPORT
    if (SOC_IS_HERCULES1(unit)) {
        SOC_IF_ERROR_RETURN(READ_ING_SRCMODFILTERr(unit, port, &smf));
        osmf = smf;
        if (modid < 0) {
            mask = 0xffffffff;
        } else {
            mask = 1 << modid;
        }
        if (enable) {
            smf |= mask;
        } else {
            smf &= ~mask;
        }
        if (smf != osmf) {
            SOC_IF_ERROR_RETURN(WRITE_ING_SRCMODFILTERr(unit, port, smf));
        }
    }
#endif /* BCM_HERCULES1_SUPPORT */

#ifdef BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        blk = SOC_PORT_BLOCK(unit, port);
        soc_mem_lock(unit, MEM_ING_SRCMODBLKm);
        if (modid < 0) {
            for (modid = 0; modid < max_mod; modid++) {
                rv = READ_MEM_ING_SRCMODBLKm(unit, blk, modid, &srcmod);
                if (rv < 0) {
                    break;
                }
                soc_MEM_ING_SRCMODBLKm_field_get(unit, &srcmod, BITMAPf,
                                                 &osmf);
                smf = (enable) ? SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0) : 0;
                if (osmf != smf) {
                    soc_MEM_ING_SRCMODBLKm_field_set(unit, &srcmod, BITMAPf,
                                                     &smf);
                    rv = WRITE_MEM_ING_SRCMODBLKm(unit, blk, modid, &srcmod);
                    if (rv < 0) {
                        break;
                    }
                }
            }
        } else {
            rv = READ_MEM_ING_SRCMODBLKm(unit, blk, modid, &srcmod);
            if (rv >= 0) {
                soc_MEM_ING_SRCMODBLKm_field_get(unit, &srcmod, BITMAPf,
                                                 &osmf);
                smf = (enable) ? SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0) : 0;
                if (osmf != smf) {
                    soc_MEM_ING_SRCMODBLKm_field_set(unit, &srcmod, BITMAPf,
                                                     &smf);
                    rv = WRITE_MEM_ING_SRCMODBLKm(unit, blk, modid, &srcmod);
                }
            }
        }
        soc_mem_unlock(unit, MEM_ING_SRCMODBLKm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcm_port_modid_enable_get
 * Purpose:
 *    Return enable/block state for a specific module on a port.
 * Parameters:
 *    unit - StrataSwitch PCI device unit number (driver internal).
 *    port - StrataSwitch port number.
 *    modid - Which source module id to enable/disable
 *    enable - (OUT) TRUE/FALSE Enable/disable forwarding packets from
 *             source module arriving on port.
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_port_modid_enable_get(int unit, bcm_port_t port, int modid, int *enable)
{
    int max_mod;
#ifdef BCM_HERCULES_SUPPORT
    uint32 smf;
#endif /* BCM_HERCULES_SUPPORT */
#ifdef BCM_HERCULES1_SUPPORT
    uint32 mask;
#endif /* BCM_HERCULES1_SUPPORT */
#ifdef BCM_HERCULES15_SUPPORT
    mem_ing_srcmodblk_entry_t srcmod;
    int blk, rv;
#endif /* BCM_HERCULES15_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        src_modid_block_entry_t srcmod;
        int hg_port, idx;
        bcm_pbmp_t pbmp;
        _bcm_port_egr_dest_t    egr_dst;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_max_port != egr_dst.out_min_port) {
            return BCM_E_PORT;
        }
        if (egr_dst.out_max_modid != egr_dst.out_min_modid) {
            return BCM_E_PARAM;
        }

        SOC_PBMP_CLEAR(pbmp);
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            uint32  hw_val = 0;

            SOC_IF_ERROR_RETURN(
                READ_SRC_MODID_INGRESS_BLOCKm(unit, MEM_BLOCK_ANY, 
                                              egr_dst.out_min_modid, &srcmod));
            soc_SRC_MODID_INGRESS_BLOCKm_field_get(unit, &srcmod, 
                                                   PORT_BITMAP_LOf, &hw_val);
            SOC_PBMP_WORD_SET(pbmp, 0, hw_val);
            soc_SRC_MODID_INGRESS_BLOCKm_field_get(unit, &srcmod, 
                                                   PORT_BITMAP_HIf, &hw_val);
            SOC_PBMP_WORD_SET(pbmp, 1, hw_val);
        } else {
            SOC_IF_ERROR_RETURN(
                soc_xgs3_port_num_to_higig_port_num(unit, egr_dst.out_min_port, 
                                                    &hg_port));

            idx = hg_port * (SOC_MODID_MAX(unit) + 1) + egr_dst.out_min_modid;
            SOC_IF_ERROR_RETURN(READ_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ANY, idx,
                                                      &srcmod));
            soc_SRC_MODID_BLOCKm_field_get(unit, &srcmod, PORT_BLOCK_MASK_BITMAPf,
                                           (uint32 *)&pbmp);
        }

        *enable = SOC_PBMP_IS_NULL(pbmp);

        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (soc_feature(unit, soc_feature_src_modid_blk)) {
        uint32 config;
        src_modid_block_entry_t src_mod;
        int gable, modid_cnt, p_idx, idx;
        _bcm_port_egr_dest_t    egr_dst;

        egr_dst.in_port = port;
        egr_dst.in_modid = modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_modid_egress_resolve(unit, &egr_dst));

        if (egr_dst.out_max_port != egr_dst.out_min_port) {
            return BCM_E_PORT;
        }
        if (egr_dst.out_max_modid != egr_dst.out_min_modid) {
            return BCM_E_PARAM;
        }
  
        SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &config));
        gable = !soc_reg_field_get(unit, ING_CONFIGr, config,
                                   FB_A0_COMPATIBLEf);
        if (gable) {
            modid_cnt = SOC_MODID_MAX(unit) + 1;
            p_idx = egr_dst.out_min_port - SOC_HG_OFFSET(unit);
            if (SOC_IS_RAVEN(unit) && p_idx >= 4) {
                idx = (p_idx - 1) * modid_cnt + egr_dst.out_min_modid;
            } else {
                idx = (p_idx * modid_cnt) + egr_dst.out_min_modid;
            } 
            SOC_IF_ERROR_RETURN
                (READ_SRC_MODID_BLOCKm(unit, MEM_BLOCK_ANY, 
                                       idx, &src_mod));
            *enable = (soc_SRC_MODID_BLOCKm_field32_get(unit, &src_mod, 
                                                        CPU_PORT_BLOCK_MASKf) |
                       soc_SRC_MODID_BLOCKm_field32_get(unit, &src_mod, 
                                                        HIGIG_XGE_PORT_BLOCK_MASKf)
                                                        ) ? 0 : 1;
            if (SOC_IS_FB_FX_HX(unit)) {
                *enable = (soc_SRC_MODID_BLOCKm_field32_get(unit, &src_mod, 
                                                            GE_PORT_BLOCK_MASKf)
                                                            ) ? 0 : 1;
            }

#if defined(BCM_RAPTOR_SUPPORT)
            if (soc_mem_field_valid(unit, SRC_MODID_BLOCKm, 
                                    PORT_0_5_BLOCK_MASKf)) {
                *enable = (soc_SRC_MODID_BLOCKm_field32_get(unit, &src_mod,
                                                 PORT_0_5_BLOCK_MASKf)
                                                 ) ? 0 : 1;
            }
#endif /* BCM_RAPTOR_SUPPORT */

        } else {
            *enable = gable;
        }

        return BCM_E_NONE;
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    if (!IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if (!soc_feature(unit, soc_feature_srcmod_filter)) {
        *enable = TRUE;
        return BCM_E_NONE;
    }

    max_mod = SOC_IS_HERCULES1(unit) ? 32 : 128;
    if (!((modid >= 0) && (modid < max_mod))) {
        return BCM_E_PARAM;
    }

#ifdef BCM_HERCULES1_SUPPORT
    if (SOC_IS_HERCULES1(unit)) {
        SOC_IF_ERROR_RETURN(READ_ING_SRCMODFILTERr(unit, port, &smf));
        mask = 1 << modid;
        *enable = ((smf & mask) != 0);
    }
#endif /* BCM_HERCULES1_SUPPORT */

#ifdef BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_ING_SRCMODBLKm(unit, blk, modid, &srcmod);
        if (rv >= 0) {
            soc_MEM_ING_SRCMODBLKm_field_get(unit, &srcmod, BITMAPf, &smf);
            *enable = (smf & SOC_PBMP_WORD_GET(PBMP_ALL(unit),0)) ? 1 : 0;
        }
    }
#endif /* BCM_HERCULES15_SUPPORT */

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_port_flood_block_set
 * Purpose:
 *      Enable/disable port membership in one of the egress block registers
 * Parameters:
 *      unit
 *      ingress_port    - ingress port on which the egress blocking
 *                        will be enabled
 *      egress_port     - port for which egress will be blocked
 *      reg             - egress block register.
 *      enable          - Whether blocking should be enabled or disabled
 * Returns:
 *      BCM_E_UNAVAIL   - This chip does not support this feature
 *      BCM_E_NONE
 * Notes:
 *      'reg' must be one of BCAST_BLOCK_MASKr,
 *      UNKNOWN_UCAST_BLOCK_MASKr, UNKNOWN_MCAST_BLOCK_MASKr, or
 *      their IPIC variants.
 *
 *      This function is a helper for bcm_port_flood_block_set()
 */

STATIC int
_bcm_port_flood_block_set(int unit,
                          bcm_port_t ingress_port, bcm_port_t egress_port,
                          soc_reg_t reg, int enable)
{
    uint64 r;
    uint32 addr;
    pbmp_t pbmp;
    uint32 pbmp32;
    uint32 bitmap32;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return BCM_E_UNAVAIL;
    }

    addr = soc_reg_addr(unit, reg, ingress_port, 0);
    BCM_IF_ERROR_RETURN(soc_reg_read(unit, reg, addr, &r));

    SOC_PBMP_PORT_SET(pbmp, egress_port);

    if (SOC_REG_IS_64(unit, reg)) {
        /* Low bitmap */
        bitmap32 = soc_reg64_field32_get(unit, reg, r, BLK_BITMAP_0f);
        pbmp32 = SOC_PBMP_WORD_GET(pbmp, 0);
        (enable) ? (bitmap32 |= pbmp32) : (bitmap32 &= ~pbmp32);
        soc_reg64_field32_set(unit, reg, &r, BLK_BITMAP_0f, bitmap32);

        /* High bitmap */
        bitmap32 = soc_reg64_field32_get(unit, reg, r, BLK_BITMAP_1f);
        pbmp32 = SOC_PBMP_WORD_GET(pbmp, 1);
        (enable) ? (bitmap32 |= pbmp32) : (bitmap32 &= ~pbmp32);
        soc_reg64_field32_set(unit, reg, &r, BLK_BITMAP_1f, bitmap32);
    } else {
        bitmap32 = soc_reg64_field32_get(unit, reg, r, BLK_BITMAPf);
        pbmp32 = SOC_PBMP_WORD_GET(pbmp, 0);
        (enable) ? (bitmap32 |= pbmp32) : (bitmap32 &= ~pbmp32);
        soc_reg64_field32_set(unit, reg, &r, BLK_BITMAPf, bitmap32);
    }

    return soc_reg_write(unit, reg, addr, r);
}

/*
 * Function:
 *      _bcm_port_flood_block_get
 * Purpose:
 *      Retrieve the current state egress block state on a port
 * Parameters:
 *      unit
 *      ingress_port    - ingress_port for which the state will be retreived
 *      egress_port     - port for which the egress blocking state
 *                        on ingress_port is requested
 *      reg             - block mask register
 *      enable          - (OUT) receives the block state
 * Returns:
 *      BCM_E_UNAVAIL   - Feature unavailable on this chip
 *      BCM_E_NONE
 * Notes:
 *      See _bcm_port_flood_block_set()
 *      This is a helper function for bcm_port_flood_block_get()
 */

STATIC int
_bcm_port_flood_block_get(int unit,
                          bcm_port_t ingress_port, bcm_port_t egress_port,
                          soc_reg_t reg, int* enable)
{
    uint64 r;
    uint32 addr;
    pbmp_t pbmp;
    uint32 pbmp32;
    uint32 bitmap32;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return BCM_E_UNAVAIL;
    }

    addr = soc_reg_addr(unit, reg, ingress_port, 0);
    BCM_IF_ERROR_RETURN(soc_reg_read(unit, reg, addr, &r));

    SOC_PBMP_PORT_SET(pbmp, egress_port);

    if (SOC_REG_IS_64(unit, reg)) {
        /* Check low bitmap */
        bitmap32 = soc_reg64_field32_get(unit, reg, r, BLK_BITMAP_0f);
        pbmp32 = SOC_PBMP_WORD_GET(pbmp, 0);
        if (pbmp32) {
            *enable = (pbmp32 & bitmap32) ? 1 : 0;
        } else {
            /* Check high bitmap */
            bitmap32 = soc_reg64_field32_get(unit, reg, r, BLK_BITMAP_1f);
            pbmp32 = SOC_PBMP_WORD_GET(pbmp, 1);
            assert(pbmp32);
            *enable = (pbmp32 & bitmap32) ? 1 : 0;
        }
    } else {
        bitmap32 = soc_reg64_field32_get(unit, reg, r, BLK_BITMAPf);
        pbmp32 = SOC_PBMP_WORD_GET(pbmp, 0);
        *enable = (pbmp32 & bitmap32) ? 1 : 0;
    }

    return BCM_E_NONE;
}
#if defined(BCM_RAPTOR_SUPPORT)
/*
 * Function:
 *      _bcm_port_flood_block_hi_set
 * Purpose:
 *      Enable/disable port membership in one of the egress block registers
 * Parameters:
 *      unit
 *      ingress_port    - ingress port on which the egress blocking
 *                        will be enabled
 *      egress_port     - port for which egress will be blocked
 *      reg             - egress block register.
 *      enable          - Whether blocking should be enabled or disabled
 * Returns:
 *      BCM_E_UNAVAIL   - This chip does not support this feature
 *      BCM_E_NONE
 * Notes:
 *      'reg' must be one of BCAST_BLOCK_MASKr,
 *      UNKNOWN_UCAST_BLOCK_MASKr, UNKNOWN_MCAST_BLOCK_MASKr, or
 *      their Stack port variants.
 *
 *      This function is a helper for bcm_port_flood_block_set()
 */

STATIC int
_bcm_port_flood_block_hi_set(int unit,
                              bcm_port_t ingress_port, bcm_port_t egress_port,
                              soc_reg_t reg, int enable)
{
    uint32 rval;
    uint32 addr;
    pbmp_t pbmp;
    uint32 pbmp32;
    uint32 bitmap32, obitmap32;

    if (!soc_feature(unit, soc_feature_register_hi)) {
        return BCM_E_UNAVAIL;
    }

    SOC_PBMP_PORT_SET(pbmp, egress_port);
    pbmp32 = SOC_PBMP_WORD_GET(pbmp, 1);

    if (!pbmp32) {
        return BCM_E_NONE;
    }

    addr = soc_reg_addr(unit, reg, ingress_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));

    obitmap32 = bitmap32 = soc_reg_field_get(unit, reg, rval, BLK_BITMAPf);
    if (enable) {
        bitmap32 |= pbmp32;
    } else {
        bitmap32 &= ~pbmp32;
    }

    if (obitmap32 != bitmap32) {
        soc_reg_field_set(unit, reg, &rval, BLK_BITMAPf, bitmap32);
        return soc_reg32_write(unit, addr, rval);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_port_flood_block_hi_get
 * Purpose:
 *      Retrieve the current state egress block state on a port
 * Parameters:
 *      unit
 *      ingress_port    - ingress_port for which the state will be retreived
 *      egress_port     - port for which the egress blocking state
 *                        on ingress_port is requested
 *      reg             - block mask register
 *      enable          - (OUT) receives the block state
 * Returns:
 *      BCM_E_UNAVAIL   - Feature unavailable on this chip
 *      BCM_E_NONE
 * Notes:
 *      See _bcm_port_flood_block_hi_set()
 *      This is a helper function for bcm_port_flood_block_get()
 */

STATIC int
_bcm_port_flood_block_hi_get(int unit,
                          bcm_port_t ingress_port, bcm_port_t egress_port,
                          soc_reg_t reg, int* enable)
{
    uint32 rval;
    uint32 addr;
    pbmp_t pbmp;
    uint32 pbmp32;
    uint32 bitmap32;

    if (!soc_feature(unit, soc_feature_register_hi)) {
        return BCM_E_UNAVAIL;
    }

    SOC_PBMP_PORT_SET(pbmp, egress_port);
    pbmp32 = SOC_PBMP_WORD_GET(pbmp, 1);
    if (!pbmp32) {
        *enable = 0;
        return BCM_E_NONE;
    }

    addr = soc_reg_addr(unit, reg, ingress_port, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));

    bitmap32 = soc_reg_field_get(unit, reg, rval, BLK_BITMAPf);
    *enable = (pbmp32 & bitmap32) ? 1 : 0;

    return BCM_E_NONE;
}
#endif /* BCM_RAPTOR_SUPPORT */

/*
 * Function:
 *      bcm_port_flood_block_set
 * Purpose:
 *      Set selective per-port blocking of flooded VLAN traffic
 * Parameters:
 *      unit            - unit number
 *      ingress_port    - Port traffic is ingressing
 *      egress_port     - Port for which the traffic should be blocked.
 *      flags           - Specifies the type of traffic to block
 * Returns:
 *      BCM_E_UNAVAIL   - Functionality not available
 *      BCM_E_NONE
 *
 * NOTE:  For fabrics, this changes egrmskbmap; as a result, all traffic
 * will be blocked (or enabled) to the given egress port, including
 * unicast traffic.
 */

int
bcm_esw_port_flood_block_set(int unit,
                         bcm_port_t ingress_port, bcm_port_t egress_port,
                         uint32 flags)
{
    soc_reg_t reg;
    int enable;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, ingress_port, 
                                                     &ingress_port));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, egress_port, 
                                                     &egress_port));

#if defined(BCM_XGS12_FABRIC_SUPPORT)
    /* See note above */
    if (SOC_IS_XGS12_FABRIC(unit)) {
        uint32  ports, oports;
        pbmp_t  pbmp;

        if (!IS_HG_PORT(unit, ingress_port)) {
            return BCM_E_PORT;
        }

        SOC_IF_ERROR_RETURN
            (READ_ING_EGRMSKBMAPr(unit, ingress_port, &ports));
        oports = ports;
        SOC_PBMP_CLEAR(pbmp);
        SOC_PBMP_WORD_SET(pbmp, 0, ports);
        if (flags) {
            SOC_PBMP_PORT_REMOVE(pbmp, egress_port);
        } else {
            SOC_PBMP_PORT_ADD(pbmp, egress_port);
        }
        ports = SOC_PBMP_WORD_GET(pbmp, 0);
        if (ports != oports) {
            SOC_IF_ERROR_RETURN
                (WRITE_ING_EGRMSKBMAPr(unit, ingress_port, ports));
        }
        return BCM_E_NONE;
    }
#endif /* BCM_XGS12_FABRIC_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
        || defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit)||
        SOC_IS_HBX(unit) || SOC_IS_HAWKEYE(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IING_EGRMSKBMAPr : ING_EGRMSKBMAPr;
        enable = flags & BCM_PORT_FLOOD_BLOCK_ALL;
        BCM_IF_ERROR_RETURN
            (_bcm_port_flood_block_set(unit, ingress_port, egress_port,
                                       reg, enable));
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || \
          BCM_BRADLEY_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IING_EGRMSKBMAP_64r : ING_EGRMSKBMAP_64r;
        enable = flags & BCM_PORT_FLOOD_BLOCK_ALL;
        BCM_IF_ERROR_RETURN
            (_bcm_port_flood_block_set(unit, ingress_port, egress_port,
                                       reg, enable));
    }
#endif

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IBCAST_BLOCK_MASK_64r : BCAST_BLOCK_MASK_64r;
    } else 
#endif
    {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IBCAST_BLOCK_MASKr : BCAST_BLOCK_MASKr;
    }
    enable = flags & BCM_PORT_FLOOD_BLOCK_BCAST;
    BCM_IF_ERROR_RETURN
        (_bcm_port_flood_block_set(unit, ingress_port, egress_port,
                                   reg, enable));

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_MCAST_BLOCK_MASK_64r : UNKNOWN_MCAST_BLOCK_MASK_64r;
    } else 
#endif
    {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_MCAST_BLOCK_MASKr : UNKNOWN_MCAST_BLOCK_MASKr;
    }
    enable = flags & BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST;
    BCM_IF_ERROR_RETURN
        (_bcm_port_flood_block_set(unit, ingress_port, egress_port,
                                   reg, enable));

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_UCAST_BLOCK_MASK_64r : UNKNOWN_UCAST_BLOCK_MASK_64r;
    } else 
#endif
    {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_UCAST_BLOCK_MASKr : UNKNOWN_UCAST_BLOCK_MASKr;
    }
    enable = flags & BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST;
    BCM_IF_ERROR_RETURN
        (_bcm_port_flood_block_set(unit, ingress_port, egress_port,
                                   reg, enable));

    if (IS_CPU_PORT(unit, ingress_port) && SOC_IS_FBX(unit)) {
        soc_reg_t rreg, ireg;
        {
          if (SOC_IS_TR_VL(unit)) {
              rreg = BCAST_BLOCK_MASK_64r;
              ireg = IBCAST_BLOCK_MASK_64r;
          } else {
              rreg = BCAST_BLOCK_MASKr;
              ireg = IBCAST_BLOCK_MASKr;
          }
          reg = !IS_ST_PORT(unit, ingress_port) ? ireg : rreg;
        }
        enable = flags & BCM_PORT_FLOOD_BLOCK_BCAST;
        BCM_IF_ERROR_RETURN
          (_bcm_port_flood_block_set(unit, ingress_port, egress_port,
                                     reg, enable));

        {
          if (SOC_IS_TR_VL(unit)) {
              rreg = UNKNOWN_MCAST_BLOCK_MASK_64r;
              ireg = IUNKNOWN_MCAST_BLOCK_MASK_64r;
          } else {
              rreg = UNKNOWN_MCAST_BLOCK_MASKr;
              ireg = IUNKNOWN_MCAST_BLOCK_MASKr;
          }
          reg = !IS_ST_PORT(unit, ingress_port) ? ireg : rreg;
        }
        enable = flags & BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST;
        BCM_IF_ERROR_RETURN
          (_bcm_port_flood_block_set(unit, ingress_port, egress_port,
                                     reg, enable));

        {
          if (SOC_IS_TR_VL(unit)) {
              rreg = UNKNOWN_UCAST_BLOCK_MASK_64r;
              ireg = IUNKNOWN_UCAST_BLOCK_MASK_64r;
          } else {
              rreg = UNKNOWN_UCAST_BLOCK_MASKr;
              ireg = IUNKNOWN_UCAST_BLOCK_MASKr;
          }
          reg = !IS_ST_PORT(unit, ingress_port) ? ireg : rreg;
        }
        enable = flags & BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST;
        BCM_IF_ERROR_RETURN
          (_bcm_port_flood_block_set(unit, ingress_port, egress_port,
                                     reg, enable));
    }

    /*
     * Handle the devices with > 32 ports that use _HI convention
     */
#if defined(BCM_RAPTOR_SUPPORT)
       if (soc_feature(unit, soc_feature_register_hi)){
            reg = IS_ST_PORT(unit, ingress_port) ?
                IUNKNOWN_UCAST_BLOCK_MASK_HIr : UNKNOWN_UCAST_BLOCK_MASK_HIr;
            enable = flags & BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST;
            BCM_IF_ERROR_RETURN
                (_bcm_port_flood_block_hi_set(unit, ingress_port, egress_port,
                                              reg, enable));

            reg = IS_ST_PORT(unit, ingress_port) ?
                IUNKNOWN_MCAST_BLOCK_MASK_HIr : UNKNOWN_MCAST_BLOCK_MASK_HIr;
            enable = flags & BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST;
            BCM_IF_ERROR_RETURN
                (_bcm_port_flood_block_hi_set(unit, ingress_port, egress_port,
                                              reg, enable));

            reg = IS_ST_PORT(unit, ingress_port) ?
                IBCAST_BLOCK_MASK_HIr : BCAST_BLOCK_MASK_HIr;
            enable = flags & BCM_PORT_FLOOD_BLOCK_BCAST;
            BCM_IF_ERROR_RETURN
                (_bcm_port_flood_block_hi_set(unit, ingress_port, egress_port,
                                              reg, enable));
       }
#endif /* BCM_RAPTOR_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_flood_block_get
 * Purpose:
 *      Get the current per-port flood block settings
 * Parameters:
 *      unit            - unit number
 *      ingress_port    - Port traffic is ingressing
 *      egress_port     - Port for which the traffic would be blocked
 *      flags           - (OUT) Returns the current settings
 * Returns:
 *      BCM_E_UNAVAIL   - Functionality not available
 *      BCM_E_NONE
 */

int
bcm_esw_port_flood_block_get(int unit,
                         bcm_port_t ingress_port, bcm_port_t egress_port,
                         uint32 *flags)
{
    soc_reg_t reg;
    int enable;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, ingress_port, 
                                                     &ingress_port));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, egress_port, 
                                                     &egress_port));

    *flags = 0;

#if defined(BCM_XGS12_FABRIC_SUPPORT)
    if (SOC_IS_XGS12_FABRIC(unit)) {
        uint32  ports;
        pbmp_t  pbmp;

        if (!IS_HG_PORT(unit, ingress_port)) {
            return BCM_E_PORT;
        }

        SOC_IF_ERROR_RETURN
            (READ_ING_EGRMSKBMAPr(unit, ingress_port, &ports));
        SOC_PBMP_CLEAR(pbmp);
        SOC_PBMP_WORD_SET(pbmp, 0, ports);
        if (!SOC_PBMP_MEMBER(pbmp, egress_port)) {
            *flags = -1;        
        }
        return BCM_E_NONE;
    }
#endif

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
        || defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit)||
        SOC_IS_HBX(unit) || SOC_IS_HAWKEYE(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IING_EGRMSKBMAPr : ING_EGRMSKBMAPr;
        BCM_IF_ERROR_RETURN
            (_bcm_port_flood_block_get(unit, ingress_port, egress_port,
                                       reg, &enable));
        *flags |= (enable) ? BCM_PORT_FLOOD_BLOCK_ALL : 0;
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IING_EGRMSKBMAP_64r : ING_EGRMSKBMAP_64r;
        BCM_IF_ERROR_RETURN
            (_bcm_port_flood_block_get(unit, ingress_port, egress_port,
                                       reg, &enable));
        *flags |= (enable) ? BCM_PORT_FLOOD_BLOCK_ALL : 0;
    }
#endif

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IBCAST_BLOCK_MASK_64r : BCAST_BLOCK_MASK_64r;
    } else 
#endif
    {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IBCAST_BLOCK_MASKr : BCAST_BLOCK_MASKr;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_port_flood_block_get(unit, ingress_port, egress_port,
                                   reg, &enable));
    *flags |= (enable) ? BCM_PORT_FLOOD_BLOCK_BCAST : 0;

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_MCAST_BLOCK_MASK_64r : UNKNOWN_MCAST_BLOCK_MASK_64r;
    } else 
#endif
    {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_MCAST_BLOCK_MASKr : UNKNOWN_MCAST_BLOCK_MASKr;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_port_flood_block_get(unit, ingress_port, egress_port,
                                   reg, &enable));
    *flags |= (enable) ? BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST : 0;

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_UCAST_BLOCK_MASK_64r : UNKNOWN_UCAST_BLOCK_MASK_64r;
    } else 
#endif
    {
        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_UCAST_BLOCK_MASKr : UNKNOWN_UCAST_BLOCK_MASKr;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_port_flood_block_get(unit, ingress_port, egress_port,
                                   reg, &enable));
    *flags |= (enable) ? BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST : 0;

#if defined(BCM_RAPTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_register_hi)){
        reg = IS_ST_PORT(unit, ingress_port) ?
            IBCAST_BLOCK_MASK_HIr : BCAST_BLOCK_MASK_HIr;
        BCM_IF_ERROR_RETURN
            (_bcm_port_flood_block_hi_get(unit, ingress_port, egress_port,
                                       reg, &enable));
        *flags |= (enable) ? BCM_PORT_FLOOD_BLOCK_BCAST : 0;

        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_MCAST_BLOCK_MASK_HIr : UNKNOWN_MCAST_BLOCK_MASK_HIr;
        BCM_IF_ERROR_RETURN
            (_bcm_port_flood_block_hi_get(unit, ingress_port, egress_port,
                                       reg, &enable));
        *flags |= (enable) ? BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST : 0;

        reg = IS_ST_PORT(unit, ingress_port) ?
            IUNKNOWN_UCAST_BLOCK_MASK_HIr : UNKNOWN_UCAST_BLOCK_MASK_HIr;
        BCM_IF_ERROR_RETURN
            (_bcm_port_flood_block_hi_get(unit, ingress_port, egress_port,
                                       reg, &enable));
        *flags |= (enable) ? BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST : 0;
    }
#endif /* BCM_RAPTOR_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Per-port leaky bucket ingress and egress rate limiting
 *
 * Granularity is in kbits/sec and a rate of 0 disables rate limiting.
 * The max burst size is set in kbits.
 */

/*
 * Function:
 *      bcm_port_rate_egress_set
 * Purpose:
 *      Set egress rate limiting parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      kbits_sec - Rate in kilobits (1000 bits) per second.
 *                      Rate of 0 disables rate limiting.
 *      kbits_burst - Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_rate_egress_set(int unit,
			 bcm_port_t port,
			 uint32 kbits_sec,
			 uint32 kbits_burst)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (!soc_feature(unit, soc_feature_egress_metering)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_METER_SUPPORT
    else {
        return mbcm_driver[unit]->mbcm_port_rate_egress_set(unit, port,
                                                            kbits_sec,
                                                            kbits_burst);
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_rate_egress_get
 * Purpose:
 *      Get egress rate limiting parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      kbits_sec - (OUT) Rate in kilobits (1000 bits) per second, or
 *                        zero if rate limiting is disabled.
 *      kbits_burst - (OUT) Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_rate_egress_get(int unit,
			 bcm_port_t port,
			 uint32 *kbits_sec,
			 uint32 *kbits_burst)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (!soc_feature(unit, soc_feature_egress_metering)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_METER_SUPPORT
    else {
        return mbcm_driver[unit]->mbcm_port_rate_egress_get(unit, port,
                                                            kbits_sec,
                                                            kbits_burst);
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_rate_ingress_set
 * Purpose:
 *      Set ingress rate limiting parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      kbits_sec - Rate in kilobits (1000 bits) per second.
 *                  Rate of 0 disables rate limiting.
 *      kbits_burst - Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_rate_ingress_set(int unit,
			  bcm_port_t port,
			  uint32 kbits_sec,
			  uint32 kbits_burst)
{
#ifdef BCM_METER_SUPPORT
    int                 rv = BCM_E_NONE;
#endif

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        /* Use port-based ingress metering function */
        return bcm_tucana_port_rate_ingress_set(unit, port, kbits_sec,
                                                kbits_burst);
    }
#endif

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        /*
         * Use port-based ingress metering function.
         * Enabling rate-based ingress metering on the Firebolt
         * will also enable rate-based pause frames on this port.
         */
        return bcm_xgs3_port_rate_ingress_set(unit, port, kbits_sec,
                                              kbits_burst);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_METER_SUPPORT
    /* Use FFP and metering to implement port-based ingress metering */
    /* If it exists, kill existing filter/meter */
    if (PORT(unit, port).meter_dpid != -1) {
        assert(PORT(unit, port).meter_cfid != -1);
        rv = bcm_esw_ds_datapath_delete(unit,
                                        PORT(unit, port).meter_dpid);
        PORT(unit, port).meter_dpid = -1;
        PORT(unit, port).meter_cfid = -1;
        if (rv < 0) {
            return rv;
        }
    }
    if (!kbits_sec) {
        return BCM_E_NONE;
    } else {
        bcm_ds_clfr_t clfr;
        bcm_ds_outprofile_actn_t outp;
        pbmp_t pbmp;

        /* Setup the profile */
        bcm_ds_clfr_t_init(&clfr);
        outp.opa_flags = BCM_DS_OUT_ACTN_DO_NOT_SWITCH;
        outp.opa_kbits_burst = kbits_burst;
        outp.opa_kbits_sec = kbits_sec;
        clfr.cf_flags = 0; /* no options */

        BCM_PBMP_PORT_SET(pbmp, port);
        rv = bcm_esw_ds_datapath_create(unit, 0, pbmp,
                                        &PORT(unit, port).meter_dpid);
        if (rv < 0) {
            return rv;
        }

        rv = bcm_esw_ds_classifier_create(unit, PORT(unit, port).meter_dpid,
                                          &clfr,
                                          NULL,
                                          &outp,
                                          NULL,
                                          &PORT(unit, port).meter_cfid);
        if (rv < 0) {
            (void) bcm_esw_ds_datapath_delete(unit,
                                              PORT(unit, port).meter_dpid);
            return rv;
        }

        rv = bcm_esw_ds_datapath_install(unit, PORT(unit, port).meter_dpid);
        if (rv < 0) {
            (void) bcm_esw_ds_classifier_delete(unit,
                                                PORT(unit, port).meter_dpid,
                                                PORT(unit, port).meter_cfid);
            (void) bcm_esw_ds_datapath_delete(unit,
                                              PORT(unit, port).meter_dpid);
            return rv;
        }
    } /* Set an FFP meter */

    return BCM_E_NONE;

#else /* !BCM_METER_SUPPORT */

    return BCM_E_UNAVAIL;

#endif /* !BCM_METER_SUPPORT */

}

/*
 * Function:
 *      bcm_port_rate_ingress_get
 * Purpose:
 *      Get ingress rate limiting parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      kbits_sec - (OUT) Rate in kilobits (1000 bits) per second, or
 *                        zero if rate limiting is disabled.
 *      kbits_burst - (OUT) Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_rate_ingress_get(int unit,
			  bcm_port_t port,
			  uint32 *kbits_sec,
			  uint32 *kbits_burst)
{

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (!kbits_sec || !kbits_burst) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        /* Use port-based ingress metering function */
        return bcm_tucana_port_rate_ingress_get(unit, port, kbits_sec,
                                                kbits_burst);
    }
#endif

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
       /* Use port-based ingress metering function */
        return bcm_xgs3_port_rate_ingress_get(unit, port, kbits_sec,
                                            kbits_burst);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_METER_SUPPORT
    if (PORT(unit, port).meter_dpid == -1) {
        assert(PORT(unit, port).meter_cfid == -1);
        *kbits_sec = 0;
        *kbits_burst = 0;
    } else {
        return bcm_esw_ds_rate_get(unit,
                                   PORT(unit, port).meter_dpid,
                                   PORT(unit, port).meter_cfid,
                                   kbits_sec,
                                   kbits_burst);
    }

    return BCM_E_NONE;

#else /* !BCM_METER_SUPPORT */

    *kbits_sec = 0;
    *kbits_burst = 0;
    return BCM_E_UNAVAIL;

#endif /* !BCM_METER_SUPPORT */

}

/*
 * Function:
 *      bcm_port_rate_pause_set
 * Purpose:
 *      Set ingress rate limiting pause frame control parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      kbits_pause - Pause threshold in kbits (1000 bits).
 *              A value of zero disables the pause/resume mechanism.
 *      kbits_resume - Resume threshold in kbits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This interface applies only when port ingress rate limiting
 *      is active.  Currently, only the 566x supports this feature.
 *
 *      If the maximum of bits that could be received before dropping a
 *      frame falls below the kbits_pause, a pause frame is sent.
 *      A resume frame will be sent once it becomes possible to receive
 *      kbits_resume bits of data without dropping.
 */

int
bcm_esw_port_rate_pause_set(int unit,
			bcm_port_t port,
			uint32 kbits_pause,
			uint32 kbits_resume)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        return bcm_tucana_port_rate_pause_set(unit, port, kbits_pause,
                                              kbits_resume);
    }
#endif /* BCM_TUCANA_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    /*
     * For Firebolt, rate-based pause frames are generated as
     * a result of rate-based metering. As such, their behavior
     * is not directly controllable, but may be read.
     * see bcm_port_rate_pause_get().
     */
#endif /* BCM_FIREBOLT_SUPPORT */
    /* Otherwise, not supported  */
    return (kbits_pause == 0) ? BCM_E_NONE : BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_rate_pause_get
 * Purpose:
 *      Get ingress rate limiting pause frame control parameters
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      kbits_pause - (OUT) Pause threshold in kbits (1000 bits).
 *              Zero indicates the pause/resume mechanism is disabled.
 *      kbits_resume - (OUT) Resume threshold in kbits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_port_rate_pause_get(int unit,
			bcm_port_t port,
			uint32 *kbits_pause,
			uint32 *kbits_resume)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit))  {
        return bcm_tucana_port_rate_pause_get(unit, port, kbits_pause,
                                              kbits_resume);
    }
#endif /* BCM_TUCANA_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_port_rate_pause_get(unit, port, kbits_pause,
                                          kbits_resume);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return bcm_fb_port_rate_pause_get(unit, port, kbits_pause,
                                          kbits_resume);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    /* Otherwise, not supported */
    *kbits_pause = 0;
    *kbits_resume = 0;
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_sample_rate_set
 * Purpose:
 *      Control the sampling of packets ingressing or egressing a port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      ingress_rate - Every 1/ingress_rate packets will be sampled
 *              0 indicates no sampling
 *              1 indicates sampling all packets
 *      egress_rate - Every 1/egress_rate packets will be sampled
 *              0 indicates no sampling
 *              1 indicates sampling all packets
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This API is a building block for supporting sFlow (RFC 3176)
 */

int
bcm_esw_port_sample_rate_set(int unit, bcm_port_t port,
                         int ingress_rate, int egress_rate)
{
    /*
     * IMPLEMENTATION NOTES
     *
     * ingress_rate maps into the per port SFLOW_ING_THRESHOLD reg:
     *  if (ingress_rate <= 0) {
     *          SFLOW_ING_THRESHOLD.ENABLE=0
     *          SFLOW_ING_THRESHOLD.THRESHOLD=0
     *  } else {
     *          SFLOW_ING_THRESHOLD.ENABLE=1
     *          SFLOW_ING_THRESHOLD.THRESHOLD=0xffff/ingress_rate
     *  }
     * egress_rate maps into the per port SFLOW_EGR_THRESHOLD reg:
     *  if (egress_rate <= 0) {
     *          SFLOW_EGR_THRESHOLD.ENABLE=0
     *          SFLOW_EGR_THRESHOLD.THRESHOLD=0
     *  } else {
     *          SFLOW_EGR_THRESHOLD.ENABLE=1
     *          SFLOW_EGR_THRESHOLD.THRESHOLD=0xffff/egress_rate
     *  }
     *
     * bcmSwitchCpuSamplePrio maps to CPU_CONTROL_2.CPU_SFLOW_PRIORITY
     * bcmSwitchSampleIngressRandomSeed maps to SFLOW_ING_RAND_SEED.SEED
     * bcmSwitchSampleEgressRandomSeed maps to SFLOW_EGR_RAND_SEED.SEED
     *
     * There are two related RX reason codes defined in <bcm/rx.h>
     *  BCM_RX_PR_SAMPLE_SRC
     *  BCM_RX_PR_SAMPLE_DST
     */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (ingress_rate < 0 || egress_rate < 0) {
        return BCM_E_PARAM;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_port_sample_rate_set(unit, port,
                                             ingress_rate, egress_rate);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_sample_rate_get
 * Purpose:
 *      Get the sampling of packets ingressing or egressing a port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      ingress_rate - Every 1/ingress_rate packets will be sampled
 *              0 indicates no sampling
 *              1 indicates sampling all packets
 *      egress_rate - Every 1/egress_rate packets will be sampled
 *              0 indicates no sampling
 *              1 indicates sampling all packets
 * Returns:
 *      BCM_E_UNIT
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_XXX
 * Notes:
 *      This API is a building block for supporting sFlow (RFC 3176)
 */

int
bcm_esw_port_sample_rate_get(int unit, bcm_port_t port,
                         int *ingress_rate, int *egress_rate)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (ingress_rate == NULL  || egress_rate == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_port_sample_rate_get(unit, port,
                                           ingress_rate, egress_rate);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */


    return BCM_E_UNAVAIL;
}

/*
 * Double Tagging
 */

/*
 * Function:
 *      bcm_port_dtag_mode_set
 * Description:
 *      Set the double-tagging mode of a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - Double-tagging mode, one of:
 *              BCM_PORT_DTAG_MODE_NONE            No double tagging
 *              BCM_PORT_DTAG_MODE_INTERNAL        Service Provider port
 *              BCM_PORT_DTAG_MODE_EXTERNAL        Customer port
 *              BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG  Remove customer tag
 *              BCM_PORT_DTAG_ADD_EXTERNAL_TAG     Add customer tag
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      BCM_PORT_DTAG_MODE_INTERNAL is for service provider ports.
 *              A tag will be added if the packet does not already
 *              have the internal TPID (see bcm_port_tpid_set below).
 *              Internally this sets DT_MODE and clears IGNORE_TAG.
 *      BCM_PORT_DTAG_MODE_EXTERNAL is for customer ports.
 *              The service provider TPID will always be added
 *              (see bcm_port_tpid_set below).
 *              Internally this sets DT_MODE and sets IGNORE_TAG.
 *      On some chips, such as BCM5665, double-tag enable is a system-wide
 *              setting rather than a port setting, so enabling double-
 *              tagging on one port may enable it on all ports.
 */

int
bcm_esw_port_dtag_mode_set(int unit, bcm_port_t port, int mode)
{
    int         dt_mode, ignore_tag;
    int         dt_mode_mask;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    /* Skip PORT_SWITCHED_CHECK for Higig lookup (for q-in-q proxy) */
    if (!(IS_ST_PORT(unit, port) &&
          soc_feature(unit, soc_feature_higig_lookup))) {
        PORT_SWITCHED_CHECK(unit, port);
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || \
        SOC_IS_HAWKEYE(unit) || SOC_IS_TRX(unit)) {
        if (mode & ~(BCM_PORT_DTAG_MODE_INTERNAL |
                     BCM_PORT_DTAG_MODE_EXTERNAL |
                     BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG |
                     BCM_PORT_DTAG_ADD_EXTERNAL_TAG)) {
            return BCM_E_UNAVAIL;
        }

        if (IS_ST_PORT(unit, port)) {
            /* Addition or removal of two tags is not supported on 
             * Higig / Higig Lite ports.
             */
            if (mode & BCM_PORT_DTAG_ADD_EXTERNAL_TAG ||
                mode & BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG) {
                return BCM_E_PARAM;
            }
        }

        /* Removal of two tags should be enable only for UNI egress ports. */ 
        if (mode == (BCM_PORT_DTAG_MODE_INTERNAL | 
                     BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG)) {
            return BCM_E_PARAM;
        } 
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return  _bcm_trx_port_dtag_mode_set(unit, port, mode);
    }
#endif

    dt_mode_mask = BCM_PORT_DTAG_MODE_INTERNAL | 
        BCM_PORT_DTAG_MODE_EXTERNAL;


    switch (mode & dt_mode_mask) {
      case BCM_PORT_DTAG_MODE_NONE:
          dt_mode = 0;
          ignore_tag = 0;
          break;
      case BCM_PORT_DTAG_MODE_INTERNAL:
          dt_mode = 1;
          ignore_tag = 0;
          break;
      case BCM_PORT_DTAG_MODE_EXTERNAL:
          dt_mode = 1;
          ignore_tag = 1;
          break;
      default:
          return BCM_E_PARAM;
    }

#ifdef BCM_LYNX_SUPPORT
    if (SOC_IS_LYNX(unit)) {
        return _bcm_lynx_port_dtag_mode_set(unit, port, mode, 
                                            dt_mode, ignore_tag);
    }
#endif

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_draco15_port_dtag_mode_set(unit, port, mode, 
                                               dt_mode, ignore_tag);
    }
#endif

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        return _bcm_tucana_port_dtag_mode_set(unit, port, mode, 
                                              dt_mode, ignore_tag);
    }
#endif

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_port_dtag_mode_set(unit, port, mode, 
                                          dt_mode, ignore_tag);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_port_dtag_mode_set(unit, port, mode, 
                                          dt_mode, ignore_tag);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_dtag_mode_get
 * Description:
 *      Return the current double-tagging mode of a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - (OUT) Double-tagging mode
 * Return Value:
 *      BCM_E_XXX
 */

int
bcm_esw_port_dtag_mode_get(int unit, bcm_port_t port, int *mode)
{

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    /* Skip PORT_SWITCHED_CHECK for Higig lookup (for q-in-q proxy) */
    if (!(IS_ST_PORT(unit, port) &&
          soc_feature(unit, soc_feature_higig_lookup))) {
        PORT_SWITCHED_CHECK(unit, port);
    }

    if (mode == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        *mode = PORT(unit, port).dtag_mode;
        return BCM_E_NONE;
    }
#endif

#ifdef BCM_LYNX_SUPPORT
    if (SOC_IS_LYNX(unit)) {
        return _bcm_lynx_port_dtag_mode_get(unit, port, mode);
    }
#endif

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_draco15_port_dtag_mode_get(unit, port, mode);
    }
#endif

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        return _bcm_tucana_port_dtag_mode_get(unit, port, mode);
    }
#endif

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_port_dtag_mode_get(unit, port, mode);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_port_dtag_mode_get(unit, port, mode);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_tpid_set
 * Description:
 *      Set the default Tag Protocol ID for a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      This API is not specifically double-tagging-related, but
 *      the port TPID becomes the service provider TPID when double-tagging
 *      is enabled on a port.  The default TPID is 0x8100.
 *      On BCM5665, only 0x8100 is allowed for the inner (customer) tag.
 */

int
bcm_esw_port_tpid_set(int unit, bcm_port_t port, uint16 tpid)
{
#ifdef BCM_FIREBOLT_SUPPORT
    int rv; 
#endif /* BCM_FIREBOLT_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#if defined (BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) &&
        (PORT(unit, port).dtag_mode == BCM_PORT_DTAG_MODE_EXTERNAL)) {
        /* For DT mode external ports, don't enable any outer TPIDs */
        return BCM_E_NONE;
    }
#endif

#if (defined(BCM_DRACO15_SUPPORT) || defined(BCM_LYNX_SUPPORT) || \
     defined(BCM_TUCANA_SUPPORT))
    if (SOC_IS_DRACO15(unit) || SOC_IS_LYNX(unit) || SOC_IS_TUCANA(unit)) {
        return _bcm_draco_port_tpid_set(unit, port, tpid);
    }
#endif

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        PORT_LOCK(unit);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) || \
        defined(BCM_RAVEN_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_ctrl)) {
            rv = _bcm_fb2_port_tpid_set(unit, port, tpid);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
        {
            rv = _bcm_fb_port_tpid_set(unit, port, tpid);
        }
        PORT_UNLOCK(unit);
        return rv;
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_port_tpid_set(unit, port, tpid);
    }
#endif /* BCM_EASYRIDER_SUPPORT */

    if (tpid == 0x8100) {
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_tpid_get
 * Description:
 *      Retrieve the default Tag Protocol ID for a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - (OUT) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */

int
bcm_esw_port_tpid_get(int unit, bcm_port_t port, uint16 *tpid)
{
#ifdef BCM_FIREBOLT_SUPPORT
    int rv; 
#endif /* BCM_FIREBOLT_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (tpid == NULL) {
        return BCM_E_PARAM;
    }

#if (defined(BCM_DRACO15_SUPPORT) || defined(BCM_LYNX_SUPPORT) || \
     defined(BCM_TUCANA_SUPPORT))
    if (SOC_IS_DRACO15(unit) || SOC_IS_LYNX(unit) || SOC_IS_TUCANA(unit)) {
        return _bcm_draco_port_tpid_get(unit, port, tpid);
    }
#endif /* BCM_DRACO15_SUPPORT || BCM_LYNX_SUPPORT || BCM_TUCANA_SUPPORT */


#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
         PORT_LOCK(unit);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
        || defined(BCM_RAVEN_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_ctrl)) {
            rv = _bcm_fb2_port_tpid_get(unit, port, tpid);
        } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
        {
            rv = _bcm_fb_port_tpid_get(unit, port, tpid);
        }
        PORT_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return  _bcm_er_port_tpid_get(unit, port, tpid);
    }
#endif /* BCM_EASYRIDER_SUPPORT */
    *tpid = 0x8100;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_port_tpid_add
 * Description:
 *      Add allowed TPID for a port.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      tpid         - (IN) Tag Protocol ID
 *      color_select - (IN) Color mode for TPID
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_tpid_add(int unit, bcm_port_t port, 
                      uint16 tpid, int color_select)
{
    int rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (color_select != BCM_COLOR_PRIORITY && 
        color_select != BCM_COLOR_OUTER_CFI &&
        color_select != BCM_COLOR_INNER_CFI) {
        return BCM_E_PARAM;
    }

#if defined (BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) &&
        (PORT(unit, port).dtag_mode == BCM_PORT_DTAG_MODE_EXTERNAL)) {
        /* For DT mode external ports, don't enable any outer TPIDs */
        return BCM_E_NONE;
    }
#endif

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        PORT_LOCK(unit);
        rv = _bcm_fb2_port_tpid_add(unit, port, tpid, color_select);
        PORT_UNLOCK(unit);
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_port_tpid_delete
 * Description:
 *      Delete allowed TPID for a port.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      tpid - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_tpid_delete(int unit, bcm_port_t port, uint16 tpid)
{
    int rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        PORT_LOCK(unit);
        rv = _bcm_fb2_port_tpid_delete(unit, port, tpid);
        PORT_UNLOCK(unit);
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_port_tpid_delete_all
 * Description:
 *      Delete all allowed TPID for a port.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 * Return Value:
 *      BCM_E_XXX
 */
int 
bcm_esw_port_tpid_delete_all(int unit, bcm_port_t port)
{
    int rv;
    rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        PORT_LOCK(unit);
        rv = _bcm_fb2_port_tpid_delete_all(unit, port);
        PORT_UNLOCK(unit);
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_port_inner_tpid_set
 * Purpose:
 *      Set the expected TPID for the inner tag in double-tagging mode.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - Tag Protocol ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_port_inner_tpid_set(int unit, bcm_port_t port, uint16 tpid)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_SWITCHED_CHECK(unit, port);

#ifdef BCM_LYNX_SUPPORT
    if (SOC_IS_LYNX(unit)) {
        uint32          rval, orval;

        BCM_IF_ERROR_RETURN(READ_DT_CONFIG1r(unit, port, &rval));
        orval = rval;
        soc_reg_field_set(unit, DT_CONFIG1r, &rval, INNER_TPIDf, tpid);
        if (rval != orval) {
            BCM_IF_ERROR_RETURN(WRITE_DT_CONFIG1r(unit, port, rval));
        }

        return BCM_E_NONE;
    }
#endif

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        uint32 rval;
        BCM_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &rval));
        soc_reg_field_set(unit, VLAN_CTRLr, &rval, INNER_TPIDf, tpid);
        BCM_IF_ERROR_RETURN(WRITE_VLAN_CTRLr(unit, rval));
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit, EGR_CONFIGr,
                                                   REG_PORT_ANY, INNER_TPIDf,
                                                   tpid));
        return BCM_E_NONE;
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_port_inner_tpid_set(unit, port, tpid);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_inner_tpid_get
 * Purpose:
 *      Get the expected TPID for the inner tag in double-tagging mode.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      tpid - (OUT) Tag Protocol ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_port_inner_tpid_get(int unit, bcm_port_t port, uint16 *tpid)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    PORT_SWITCHED_CHECK(unit, port);

    if (tpid == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_LYNX_SUPPORT
    if (SOC_IS_LYNX(unit)) {
        uint32          rval;

        BCM_IF_ERROR_RETURN(READ_DT_CONFIG1r(unit, port, &rval));
        *tpid = soc_reg_field_get(unit, DT_CONFIG1r, rval, INNER_TPIDf);
        return BCM_E_NONE;
    }
#endif

#if defined(BCM_DRACO15_SUPPORT) || defined(BCM_TUCANA_SUPPORT)
    if (SOC_IS_DRACO15(unit) || SOC_IS_TUCANA(unit)) {
        *tpid = 0x8100;
        return BCM_E_NONE;
    }
#endif

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        uint32 rval;
        BCM_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &rval));
        *tpid = soc_reg_field_get(unit, VLAN_CTRLr, rval, INNER_TPIDf);
        return BCM_E_NONE;
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_port_inner_tpid_get(unit, port, tpid);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Depending on chip or port type the actual maximum receive frame size
 *      might be slightly higher.
 *
 *      It looks like this operation is performed the same way on all the chips
 *      and the only depends on the port type.
 */
int
bcm_esw_port_frame_max_set(int unit, bcm_port_t port, int size)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    uint32 reg;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    SOC_IF_ERROR_RETURN
        (MAC_FRAME_MAX_SET(PORT(unit, port).p_mac, unit, port, size));

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        SOC_IF_ERROR_RETURN(READ_EGR_MTU_SIZEr(unit, port, &reg));
        soc_reg_field_set(unit, EGR_MTU_SIZEr, &reg,
                          MTU_SIZEf, size + 4);
        SOC_IF_ERROR_RETURN(WRITE_EGR_MTU_SIZEr(unit, port, reg));
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        SOC_IF_ERROR_RETURN(READ_EGR_PORT_MTUr(unit, port, &reg));
        soc_reg_field_set(unit, EGR_PORT_MTUr, &reg, MTUf, size + 4);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_MTUr(unit, port, reg));
#ifdef INCLUDE_L3
        SOC_IF_ERROR_RETURN(bcm_er_ipmc_port_mtu_update(unit,port));
#endif /* INCLUDE_L3 */
    }
#endif /* BCM_EASYRIDER_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_frame_max_get
 * Description:
 *      Get the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Depending on chip or port type the actual maximum receive frame size
 *      might be slightly higher.
 *
 *      For GE ports that use 2 separate MACs (one for GE and another one for
 *      10/100 modes) the function returns the maximum rx frame size set for
 *      the current mode.
 */
int
bcm_esw_port_frame_max_get(int unit, bcm_port_t port, int *size)
{
    int            rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = MAC_FRAME_MAX_GET(PORT(unit, port).p_mac, unit, port, size);

    return rv;
}

/*
 * Function:
 *      bcm_esw_port_l3_mtu_set
 * Description:
 *      Set the maximum L3 frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_l3_mtu_set(int unit, bcm_port_t port, int size)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_egr_l3_mtu)) {
        uint32 reg, oreg;

        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

        SOC_IF_ERROR_RETURN(READ_EGR_MTU_SIZEr(unit, port, &reg));
        oreg = reg;
        soc_reg_field_set(unit, EGR_MTU_SIZEr, &reg,
                          L3_MTU_SIZEf, size + 4);
        if (oreg != reg) {
            SOC_IF_ERROR_RETURN(WRITE_EGR_MTU_SIZEr(unit, port, reg));
        }

        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_port_l3_mtu_get
 * Description:
 *      Get the maximum L3 frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_l3_mtu_get(int unit, bcm_port_t port, int *size)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_egr_l3_mtu)) {
        uint32 reg;

        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
        SOC_IF_ERROR_RETURN(READ_EGR_MTU_SIZEr(unit, port, &reg));
        *size = soc_reg_field_get(unit, EGR_MTU_SIZEr, reg, L3_MTU_SIZEf) - 4;
        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_ifg_set
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - Inter-frame gap in bit-times
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
int
bcm_esw_port_ifg_set(int unit, bcm_port_t port,
                 int speed, bcm_port_duplex_t duplex,
                 int ifg)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return MAC_IFG_SET(PORT(unit, port).p_mac, unit, port,
                       speed, duplex, ifg);
}

/*
 * Function:
 *      bcm_port_ifg_get
 * Description:
 *      Gets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - Inter-frame gap in bit-times
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_port_ifg_get(int unit, bcm_port_t port,
                 int speed, bcm_port_duplex_t duplex,
                 int *ifg)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return MAC_IFG_GET(PORT(unit, port).p_mac, unit, port,
                       speed, duplex, ifg);
}

/*
 * Function:
 *      bcm_port_phy_get
 * Description:
 *      General PHY register read
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - (OUT) Data that was read
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_port_phy_get(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    uint8  phy_id;
    uint8  phy_devad;
    uint16 phy_reg;
    uint16 phy_rd_data;
    uint32 reg_flag;
    int    rv;

    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    }

    rv       = BCM_E_UNAVAIL;
    reg_flag = BCM_PORT_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT) {
        if (flags & BCM_PORT_PHY_NOMAP) {
            /* Indirect register access is performed through PHY driver.
             * Therefore, indirect register access is not supported if
             * BCM_PORT_PHY_NOMAP flag is set.
             */
            return BCM_E_PARAM;
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        PORT_LOCK(unit);
        rv = soc_phyctrl_reg_read(unit, port, flags, phy_reg_addr, phy_data);
        PORT_UNLOCK(unit);
    }

    if (rv == BCM_E_UNAVAIL) {
        if (flags & BCM_PORT_PHY_NOMAP) {
            phy_id = port;
        } else if (flags & BCM_PORT_PHY_INTERNAL) {
            phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
        } else {
            phy_id = PORT_TO_PHY_ADDR(unit, port);
        }

        PORT_LOCK(unit);
        if (flags & BCM_PORT_PHY_CLAUSE45) {
            phy_devad = BCM_PORT_PHY_CLAUSE45_DEVAD(phy_reg_addr);
            phy_reg   = BCM_PORT_PHY_CLAUSE45_REGAD(phy_reg_addr);
            rv = soc_miimc45_read(unit, phy_id, phy_devad, 
                                  phy_reg, &phy_rd_data);

        } else {
            phy_reg = phy_reg_addr;
            rv = soc_miim_read(unit, phy_id, phy_reg, &phy_rd_data);
        }
        PORT_UNLOCK(unit);

        if (BCM_SUCCESS(rv)) {
           *phy_data = phy_rd_data;
        }
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_esw_port_phy_get: u=%d p=%d flags=0x%08x "
                     "phy_reg=0x%08x, phy_data=0x%08x, rv=%d\n",
                     unit, port, flags, phy_reg_addr, *phy_data, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_phy_set
 * Description:
 *      General PHY register write
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_port_phy_set(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{
    uint8  phy_id;
    uint8  phy_devad;
    uint16 phy_reg;
    uint16 phy_wr_data;
    uint32 reg_flag;
    int    rv;

    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_esw_port_phy_set: u=%d p=%d flags=0x%08x "
                     "phy_reg=0x%08x phy_data=0x%08x\n",
                     unit, port, flags, phy_reg_addr, phy_data));

    rv       = BCM_E_UNAVAIL;
    reg_flag = BCM_PORT_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT) {
        if (flags & BCM_PORT_PHY_NOMAP) {
            /* Indirect register access is performed through PHY driver.
             * Therefore, indirect register access is not supported if
             * BCM_PORT_PHY_NOMAP flag is set.
             */
            return BCM_E_PARAM;
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        PORT_LOCK(unit);
        rv = soc_phyctrl_reg_write(unit, port, flags, phy_reg_addr, phy_data);
        PORT_UNLOCK(unit);
    }

    if (rv == BCM_E_UNAVAIL) {
        if (flags & BCM_PORT_PHY_NOMAP) {
            phy_id = port;
        } else if (flags & BCM_PORT_PHY_INTERNAL) {
            phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
        } else {
            phy_id = PORT_TO_PHY_ADDR(unit, port);
        }

        phy_wr_data = (uint16) (phy_data & 0xffff);
        PORT_LOCK(unit);
        if (flags & BCM_PORT_PHY_CLAUSE45) {
            phy_devad = BCM_PORT_PHY_CLAUSE45_DEVAD(phy_reg_addr);
            phy_reg   = BCM_PORT_PHY_CLAUSE45_REGAD(phy_reg_addr);
            rv = soc_miimc45_write(unit, phy_id, phy_devad, 
                                   phy_reg, phy_wr_data);
        } else {
            phy_reg = phy_reg_addr;
            rv = soc_miim_write(unit, phy_id, phy_reg, phy_wr_data);
        }
        PORT_UNLOCK(unit);
    }
    return rv;
}

/*
 * Function:
 *      bcm_port_mdix_set
 * Description:
 *      Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - One of:
 *              BCM_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              BCM_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              BCM_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              BCM_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      BCM_E_UNAVAIL - feature unsupported by hardware
 *      BCM_E_XXX - other error
 */
int
bcm_esw_port_mdix_set(int unit, bcm_port_t port, bcm_port_mdix_t mode)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return soc_phyctrl_mdix_set(unit, port, mode);
}

/*
 * Function:
 *      bcm_port_mdix_get
 * Description:
 *      Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - (Out) One of:
 *              BCM_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              BCM_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              BCM_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              BCM_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      BCM_E_UNAVAIL - feature unsupported by hardware
 *      BCM_E_XXX - other error
 */
int
bcm_esw_port_mdix_get(int unit, bcm_port_t port, bcm_port_mdix_t *mode)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return soc_phyctrl_mdix_get(unit, port, mode);
}

/*
 * Function:
 *      bcm_port_mdix_status_get
 * Description:
 *      Get the current MDIX status on a port/PHY
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 *      status  - (OUT) One of:
 *              BCM_PORT_MDIX_STATUS_NORMAL
 *                      Straight connection
 *              BCM_PORT_MDIX_STATUS_XOVER
 *                      Crossover has been performed
 * Return Value:
 *      BCM_E_UNAVAIL - feature unsupported by hardware
 *      BCM_E_XXX - other error
 */
int
bcm_esw_port_mdix_status_get(int unit, bcm_port_t port,
                         bcm_port_mdix_status_t *status)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return soc_phyctrl_mdix_status_get(unit, port, status);
}

/*
 * Function:
 *      bcm_port_medium_config_set
 * Description:
 *      Set the medium-specific configuration for a combo port
 * Parameters:
 *      unit     - Device number
 *      port     - Port number
 *      medium   - The medium (BCM_PORT_MEDIUM_COPPER or BCM_PORT_MEDIUM_FIBER)
 *                 to apply the configuration to
 *      config   - per-medium configuration
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_medium_config_set(int unit, bcm_port_t port,
                           bcm_port_medium_t medium,
                           bcm_phy_config_t  *config)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    return soc_phyctrl_medium_config_set(unit, port, medium, config);
}

/*
 * Function:
 *      bcm_port_medium_config_get
 * Description:
 *      Get the medium-specific configuration for a combo port
 * Parameters:
 *      unit     - Device number
 *      port     - Port number
 *      medium   - The medium (BCM_PORT_MEDIUM_COPPER or BCM_PORT_MEDIUM_FIBER)
 *                 to get the config for
 *      config   - per-medium configuration
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_medium_config_get(int unit, bcm_port_t port,
                           bcm_port_medium_t  medium,
                           bcm_phy_config_t  *config)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return soc_phyctrl_medium_config_get(unit, port, medium, config);
}

/*
 * Function:
 *      bcm_port_medium_get
 * Description:
 *      Get the current medium used by a combo port
 * Parameters:
 *      unit     - Device number
 *      port     - Port number
 *      medium   - The medium (BCM_PORT_MEDIUM_COPPER or BCM_PORT_MEDIUM_FIBER)
 *                 which is currently selected
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_medium_get(int unit, bcm_port_t port,
                    bcm_port_medium_t *medium)
{
    int rv;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = soc_phyctrl_medium_get(unit, port, medium);

    return rv;
}

/*
 * Function:
 *      bcm_port_medium_status_register
 * Description:
 *      Register a callback function to be called on medium change event
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM  -- NULL function pointer or bad {unit, port} combination
 *      BCM_E_FULL   -- Cannot register more than 1 callback per {unit, port}
 */
int
bcm_esw_port_medium_status_register(int                          unit,
                                bcm_port_t                   port,
                                bcm_port_medium_status_cb_t  callback,
                                void                        *user_data)
{
    return soc_phy_medium_status_register(unit, port, callback, user_data);
}

/*
 * Function:
 *      bcm_port_medium_status_unregister
 * Description:
 *      Unregister a callback function to be called on medium change event
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM       -- Bad {unit, port} combination
 *      BCM_E_NOT_FOUND   -- The specified {unit, port, callback, user_data}
 *                           combination have not been registered before
 */
int
bcm_esw_port_medium_status_unregister(int                          unit,
                                  bcm_port_t                   port,
                                  bcm_port_medium_status_cb_t  callback,
                                  void                        *user_data)
{
    return soc_phy_medium_status_unregister(unit, port, callback, user_data);
}

/*
 * Function:
 *      bcm_port_phy_reset
 * Description:
 *      This function performs the low-level PHY reset and is intended to be
 *      called ONLY from callback function registered with
 *      bcm_port_phy_reset_register. Attempting to call it from any other
 *      place will break lots of things.
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_port_phy_reset(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return soc_phyctrl_reset(unit, port, NULL);
}

/*
 * Function:
 *      bcm_port_phy_reset_register
 * Description:
 *      Register a callback function to be called whenever a PHY driver
 *      needs to perform a PHY reset
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM       -- Bad {unit, port} combination
 *      BCM_E_NOT_FOUND   -- The specified {unit, port, callback, user_data}
 *                           combination have not been registered before
 */
int
bcm_esw_port_phy_reset_register(int unit, bcm_port_t port,
                            bcm_port_phy_reset_cb_t callback,
                            void  *user_data)
{
    return soc_phy_reset_register(unit, port, callback, user_data, FALSE);
}

/*
 * Function:
 *      bcm_port_phy_reset_unregister
 * Description:
 *      Unregister a callback function to be called whenever a PHY driver
 *      needs to perform a PHY reset
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM       -- Bad {unit, port} combination
 *      BCM_E_NOT_FOUND   -- The specified {unit, port, callback, user_data}
 *                           combination have not been registered before
 */
int
bcm_esw_port_phy_reset_unregister(int unit, bcm_port_t port,
                              bcm_port_phy_reset_cb_t callback,
                              void  *user_data)
{
    return soc_phy_reset_unregister(unit, port, callback, user_data);
}

/*
 * Function:
 *      bcm_port_jam_set
 * Description:
 *      Enable or disable half duplex jamming on a port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      enable - non-zero to enable jamming
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_jam_set(int unit, bcm_port_t port, int enable)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (!IS_E_PORT(unit, port)) {       /* HG, CPU ports */
        return enable ? BCM_E_CONFIG : BCM_E_NONE;
    }

#if defined(BCM_DRACO_SUPPORT)
    if (SOC_IS_DRACO(unit)) {
        uint32  config, oconfig;

        SOC_IF_ERROR_RETURN(READ_CONFIGr(unit, port, &config));
        oconfig = config;
        soc_reg_field_set(unit, CONFIGr, &config, JAM_ENf, enable ? 1 : 0);
        if (config != oconfig) {
            SOC_IF_ERROR_RETURN(WRITE_CONFIGr(unit, port, config));
        }
        return BCM_E_NONE;
    }
#endif

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        port_tab_entry_t        pent;
        soc_mem_t               mem;
        int                     oenable;

        mem = SOC_PORT_MEM_TAB(unit, port);
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                         SOC_PORT_MOD_OFFSET(unit, port),
                                         &pent));
        oenable = soc_PORT_TABm_field32_get(unit, &pent, JAM_ENf);
        enable = enable ? 1 : 0;
        if (oenable != enable) {
            soc_PORT_TABm_field32_set(unit, &pent, JAM_ENf, enable);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                              SOC_PORT_MOD_OFFSET(unit, port),
                                              &pent));
        }
        return BCM_E_NONE;
    }
#endif

#ifdef  BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32                  rval;
        int                     oenable;

        if (!IS_GE_PORT(unit, port) && !IS_FE_PORT(unit, port)) {
            return BCM_E_UNAVAIL;
        } else if (!soc_reg_field_valid(unit, GE_PORT_CONFIGr, JAM_ENf) &&
                   !soc_feature(unit, soc_feature_unimac)) {
            return BCM_E_UNAVAIL;
        } 

        if (soc_feature(unit, soc_feature_unimac)) { 
            SOC_IF_ERROR_RETURN(READ_IPG_HD_BKP_CNTLr(unit, port, &rval));
            oenable = soc_reg_field_get(unit, IPG_HD_BKP_CNTLr, rval, 
                                        HD_FC_ENAf);
            enable = enable ? 1 : 0;
            if (oenable != enable) {
                soc_reg_field_set(unit, IPG_HD_BKP_CNTLr, &rval, HD_FC_ENAf, 
                                  enable);
                SOC_IF_ERROR_RETURN(WRITE_IPG_HD_BKP_CNTLr(unit, port, rval));
            }
        } else {
            SOC_IF_ERROR_RETURN(READ_GE_PORT_CONFIGr(unit, port, &rval));
            oenable = soc_reg_field_get(unit, GE_PORT_CONFIGr, rval, JAM_ENf);
            enable = enable ? 1 : 0;
            if (oenable != enable) {
                soc_reg_field_set(unit, GE_PORT_CONFIGr, &rval, JAM_ENf, enable);
                SOC_IF_ERROR_RETURN(WRITE_GE_PORT_CONFIGr(unit, port, rval));
            }
        }
        return BCM_E_NONE;
    }
#endif

    /* LYNX XE ports */
    return enable ? BCM_E_CONFIG : BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_jam_get
 * Description:
 *      Return half duplex jamming state
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      enable - (OUT) non-zero if jamming enabled
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_esw_port_jam_get(int unit, bcm_port_t port, int *enable)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (!IS_E_PORT(unit, port)) {       /* HG, CPU ports */
        *enable = 0;
        return BCM_E_NONE;
    }

#if defined(BCM_DRACO_SUPPORT)
    if (SOC_IS_DRACO(unit)) {
        uint32  config;

        SOC_IF_ERROR_RETURN(READ_CONFIGr(unit, port, &config));
        *enable = soc_reg_field_get(unit, CONFIGr, config, JAM_ENf);
        return BCM_E_NONE;
    }
#endif

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        port_tab_entry_t        pent;
        soc_mem_t               mem;

        mem = SOC_PORT_MEM_TAB(unit, port);
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                         SOC_PORT_MOD_OFFSET(unit, port),
                                         &pent));
        *enable = soc_PORT_TABm_field32_get(unit, &pent, JAM_ENf);
        return BCM_E_NONE;
    }
#endif

#ifdef  BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32                  rval;

        if (!IS_GE_PORT(unit, port) && !IS_FE_PORT(unit, port)) {
            return BCM_E_UNAVAIL;
        } else if (!soc_reg_field_valid(unit, GE_PORT_CONFIGr, JAM_ENf) &&
                   !soc_feature(unit, soc_feature_unimac)) {
            return BCM_E_UNAVAIL;
        } 

        if (soc_feature(unit, soc_feature_unimac)) {
            SOC_IF_ERROR_RETURN(READ_IPG_HD_BKP_CNTLr(unit, port, &rval));
            *enable = soc_reg_field_get(unit, IPG_HD_BKP_CNTLr, rval, HD_FC_ENAf);
        } else {
            SOC_IF_ERROR_RETURN(READ_GE_PORT_CONFIGr(unit, port, &rval));
            *enable = soc_reg_field_get(unit, GE_PORT_CONFIGr, rval, JAM_ENf);
        }
        return BCM_E_NONE;
    }
#endif

    /* LYNX XE ports */
    *enable = 0;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_cable_diag
 * Description:
 *      Run Cable Diagnostics on port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      status - (OUT) cable diag status structure
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Cable diagnostics are only supported by some phy types
 *      (currently 5248 10/100 phy and 546x 10/100/1000 phys)
 */
int
bcm_esw_port_cable_diag(int unit, bcm_port_t port,
                    bcm_port_cable_diag_t *status)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return soc_phyctrl_cable_diag(unit, port, status);
}

/*
 * Function:
 *      bcm_esw_port_fault_get
 * Description:
 *      Get link fault type 
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - (OUT) flags to indicate fault type 
 * Return Value:
 *      BCM_E_XXX
 */

int
bcm_esw_port_fault_get(int unit, bcm_port_t port, uint32 *flags)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    *flags = 0; 
    if (soc_feature(unit, soc_feature_bigmac_fault_stat) &&
        (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port))) {
        uint64 lss;
        int lcl_fault, rmt_fault;
        soc_reg_t reg;

        if (SOC_IS_LYNX(unit)) {
            /*
             * This will "work" on Lynx A0, which does not have a register
             * by this name. Rather, the resultant register address points
             * to an unused register on Lynx A0, which always returns zero.
             * On Lynx B0 and Lynx 1.5, a real MAC_RXLSSCTRL register is
             * implemented.
             */
            reg = MAC_RXLSSCTRLr;
        } else {
            reg = MAC_RXLSSSTATr;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_read(unit, reg, soc_reg_addr(unit, reg, port, 0), &lss));
        rmt_fault = soc_reg64_field32_get(unit, reg, lss, REMOTEFAULTSTATf);
        lcl_fault = soc_reg64_field32_get(unit, reg, lss, LOCALFAULTSTATf);

        if (rmt_fault) { 
            *flags |= BCM_PORT_FAULT_REMOTE; 
        }
        if (lcl_fault) {
            *flags |= BCM_PORT_FAULT_LOCAL; 
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_trunk_index_set
 * Description:
 *      Set port index of a trunk for ingress port that is used to select 
 *      the egress port in the trunk.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number (driver internal).
 *      port       - Ingress port.
 *      port_index - Port index of a trunk.
 * Returns:
 *      BCM_E_xxxx
 * Note:
 *      The psc (aka rtag) vlaue needs to be 7 (set in bcm_trunk_set()) for 
 *      a trunk in order to utilize this programmable hashing mechanism.   
 */

int
bcm_esw_port_trunk_index_set(int unit, bcm_port_t port, int port_index)
{
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    uint32 val, mask = 0x7;

    if (soc_feature(unit, soc_feature_port_trunk_index)) {
        if (IS_ST_PORT(unit, port)) {
            val = 0;
            if (!SOC_IS_RAVEN(unit)) {
                mask = 0x3;
            }
            soc_reg_field_set(unit, IUSER_TRUNK_HASH_SELECTr, &val,
                              TRUNK_CFG_VALf, port_index & mask);
            SOC_IF_ERROR_RETURN
                (WRITE_IUSER_TRUNK_HASH_SELECTr(unit, port, val));
        } else if (IS_E_PORT(unit, port)) {
            val = 0;
            soc_reg_field_set(unit, USER_TRUNK_HASH_SELECTr, &val,
                              TRUNK_CFG_VALf, port_index & mask);
            SOC_IF_ERROR_RETURN
                (WRITE_USER_TRUNK_HASH_SELECTr(unit, port, val));
        }

        return BCM_E_NONE;
    }
#endif  /* BCM_FIREBOLT_SUPPORT || BCM_EASYRIDER_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_trunk_index_get 
 * Description:
 *      Get port index of a trunk for ingress port that is used to select 
 *      the egress port in the trunk.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number (driver internal).
 *      port       - Ingress port.
 *      port_index - (OUT) Port index of a trunk.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_port_trunk_index_get(int unit, bcm_port_t port, int *port_index)
{
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    uint32 val;

    if (soc_feature(unit, soc_feature_port_trunk_index)) {
        if (IS_ST_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
               (READ_IUSER_TRUNK_HASH_SELECTr(unit, port, &val));
            *port_index = soc_reg_field_get(unit, IUSER_TRUNK_HASH_SELECTr,
                                            val, TRUNK_CFG_VALf);
        } else if (IS_E_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
               (READ_USER_TRUNK_HASH_SELECTr(unit, port, &val));
            *port_index = soc_reg_field_get(unit, USER_TRUNK_HASH_SELECTr,
                                            val, TRUNK_CFG_VALf);
        }

        return BCM_E_NONE;
    }
#endif  /* BCM_FIREBOLT_SUPPORT || BCM_EASYRIDER_SUPPORT */

    return BCM_E_UNAVAIL;
}

#ifdef BCM_XGS3_SWITCH_SUPPORT

static soc_field_t _bcm_xgs3_priority_fields[] = {
    PRIORITY0_CNGf,
    PRIORITY1_CNGf,
    PRIORITY2_CNGf,
    PRIORITY3_CNGf,
    PRIORITY4_CNGf,
    PRIORITY5_CNGf,
    PRIORITY6_CNGf,
    PRIORITY7_CNGf,
};

#endif /* BCM_XGS3_SWITCH_SUPPORT */

/*
 * Function:
 *      bcm_port_priority_color_set
 * Purpose:
 *      Specify the color selection for the given priority.
 * Parameters:
 *      unit -  StrataSwitch PCI device unit number (driver internal).
 *      port -  Port to configure
 *      prio -  priority (aka 802.1p CoS)
 *      color - color assigned to packets with indicated priority.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int
bcm_esw_port_priority_color_set(int unit, bcm_port_t port,
                                int prio, bcm_color_t color)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_color)) {
        uint32 val, oval;
        int ptr = port;

        VLAN_CHK_PRIO(unit, prio);
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
            /* Get the pointer from the ING_PRI_CNG_MAP table */
            port_tab_entry_t pent;
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                                SOC_PORT_MOD_OFFSET(unit, port), &pent));
            ptr = soc_PORT_TABm_field32_get(unit, &pent, TRUST_DOT1P_PTRf);
        }
#endif
        SOC_IF_ERROR_RETURN(READ_CNG_MAPr(unit, ptr, &val));
        oval = val;
        soc_reg_field_set(unit, CNG_MAPr, &val,
                          _bcm_xgs3_priority_fields[prio],
                          _BCM_COLOR_ENCODING(unit, color));
        if (oval != val) {
            SOC_IF_ERROR_RETURN(WRITE_CNG_MAPr(unit, ptr, val));
        }
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_priority_color_get
 * Purpose:
 *      Get the color selection for the given priority.
 * Parameters:
 *      unit -  StrataSwitch PCI device unit number (driver internal).
 *      port -  Port to configure
 *      prio -  priority (aka 802.1p CoS)
 *      color - (OUT) color assigned to packets with indicated priority.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int
bcm_esw_port_priority_color_get(int unit, bcm_port_t port,
                                int prio, bcm_color_t *color)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_color)) {
        uint32 val, hw_color;
        int ptr = port;

        VLAN_CHK_PRIO(unit, prio);
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
            /* Get the pointer from the ING_PRI_CNG_MAP table */
            port_tab_entry_t pent;
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                                SOC_PORT_MOD_OFFSET(unit, port), &pent));
            ptr = soc_PORT_TABm_field32_get(unit, &pent, TRUST_DOT1P_PTRf);
        }
#endif
        SOC_IF_ERROR_RETURN(READ_CNG_MAPr(unit, ptr, &val));
        hw_color = soc_reg_field_get(unit, CNG_MAPr, val,
                                     _bcm_xgs3_priority_fields[prio]);
        *color = _BCM_COLOR_DECODING(unit, hw_color);
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_cfi_color_set
 * Purpose:
 *      Specify the color selection for the given CFI.
 * Parameters:
 *      unit -  StrataSwitch PCI device unit number (driver internal).
 *      port -  Port to configure
 *      cfi -   Canonical format indicator (TRUE/FALSE) 
 *      color - color assigned to packets with indicated CFI.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int
bcm_esw_port_cfi_color_set(int unit, bcm_port_t port,
                           int cfi, bcm_color_t color)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_color)) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
        || defined (BCM_TRX_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit)) {
            int                     index, pkt_pri;
            ing_pri_cng_map_entry_t pri_map;
     
            if (cfi > 1) {
                return BCM_E_PARAM;
            }
 
            sal_memset(&pri_map, 0, sizeof(pri_map));
            for (pkt_pri = 0; pkt_pri <= 7; pkt_pri++) {
                /* ING_PRI_CNG_MAP table is indexed with 
                 * port[0:4] incoming priority[2:0] incoming CFI[0]
                 */
                index = (port << 4) | (pkt_pri << 1) | cfi;
                soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, PRIf, 
                                    pkt_pri);
                soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, CNGf, 
                                    _BCM_COLOR_ENCODING(unit, color));
                SOC_IF_ERROR_RETURN(WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL,
                                                           index, &pri_map));
            }
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
        {
            uint32 val, oval;
            SOC_IF_ERROR_RETURN(READ_CNG_MAPr(unit, port, &val));
            oval = val;
            soc_reg_field_set(unit, CNG_MAPr, &val,
                              cfi ? CFI1_CNGf : CFI0_CNGf,
                             _BCM_COLOR_ENCODING(unit, color));
            if (oval != val) {
                SOC_IF_ERROR_RETURN(WRITE_CNG_MAPr(unit, port, val));
            }
        }
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_cfi_color_get
 * Purpose:
 *      Get the color selection for the given CFI.
 * Parameters:
 *      unit -  StrataSwitch PCI device unit number (driver internal).
 *      port -  Port to configure
 *      cfi -   Canonical format indicator (TRUE/FALSE) 
 *      color - (OUT) color assigned to packets with indicated CFI.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int
bcm_esw_port_cfi_color_get(int unit, bcm_port_t port,
                           int cfi, bcm_color_t *color)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_color)) {
        uint32 hw_color;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
        || defined(BCM_TRX_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit)
            || SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit)) {
            int index;
            ing_pri_cng_map_entry_t pri_map;
            /* ING_PRI_CNG_MAP table is indexed with 
             * port[0:4] incoming priority[2:0] incoming CFI[0]
             */
            index = (port << 4) | cfi; 
            SOC_IF_ERROR_RETURN(READ_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY, 
                                                      index, &pri_map));
            hw_color = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, 
                                           &pri_map, CNGf);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */
        {
            uint32 val;

            SOC_IF_ERROR_RETURN(READ_CNG_MAPr(unit, port, &val));
            hw_color = soc_reg_field_get(unit, CNG_MAPr, val,
                                         cfi ? CFI1_CNGf : CFI0_CNGf);
        }
        *color = _BCM_COLOR_DECODING(unit, hw_color);
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_port_vlan_priority_map_set
 * Description:
 *      Define the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_esw_port_vlan_priority_map_set(int unit, bcm_port_t port, int pkt_pri,
                                   int cfi, int internal_pri, bcm_color_t color)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if ((pkt_pri > 7) || (cfi > 1) || (pkt_pri < 0) || (cfi < 0)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
        || defined(BCM_TRX_SUPPORT)
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {

         return _bcm_tr2_port_vlan_priority_map_set(unit, port, pkt_pri, cfi,
                                                    internal_pri, color);
    } else
#endif
    if (soc_feature(unit, soc_feature_color_prio_map)) {
        int                      index;
        ing_pri_cng_map_entry_t  pri_map;

        /* ING_PRI_CNG_MAP table is indexed with
         * port[0:4] incoming priority[2:0] incoming CFI[0]
         */
        index = (port << 4) | (pkt_pri << 1) | cfi;

        memset(&pri_map, 0, sizeof(pri_map));
        soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, PRIf, 
                            internal_pri);
        soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, CNGf, 
                            _BCM_COLOR_ENCODING(unit, color));
        SOC_IF_ERROR_RETURN
            (WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL, index, &pri_map));
        return BCM_E_NONE;
   }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_port_vlan_priority_map_get
 * Description:
 *      Get the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (OUT) Internal priority
 *      color        - (OUT) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_esw_port_vlan_priority_map_get(int unit, bcm_port_t port, int pkt_pri,
                                   int cfi, int *internal_pri, 
                                   bcm_color_t *color)
{
    /* Input parameters check. */
    if ((NULL == internal_pri) || (NULL == color)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if ((pkt_pri > 7) || (cfi > 1)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
        || defined(BCM_TRX_SUPPORT)
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        return _bcm_tr2_port_vlan_priority_map_get(unit, port, pkt_pri,
                                                   cfi, internal_pri, color);
    } else
#endif
    if (soc_feature(unit, soc_feature_color_prio_map)) {
        int index, hw_color;
        ing_pri_cng_map_entry_t pri_map;

        /* ING_PRI_CNG_MAP table is indexed with
         * port[4:0] incoming priority[2:0] incoming CFI[0]
         */
        index = (port << 4) | (pkt_pri << 1) | cfi;

        SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_esw_port_vlan_priority_map_get: u=%d p=%d index=%d\n",
                         unit, port, index));
 
        SOC_IF_ERROR_RETURN
            (READ_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY, index, &pri_map));
        *internal_pri = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, &pri_map, 
                                           PRIf);
        hw_color = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, &pri_map, CNGf);
        *color = _BCM_COLOR_DECODING(unit, hw_color);
        return BCM_E_NONE;
   }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;

}

/*
 * Function:
 *      bcm_esw_port_vlan_priority_unmap_set
 * Description:
 *      Define the mapping of outgoing port, internal priority, and color to
 *  outgoing packet priority and cfi bit.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) Color
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_esw_port_vlan_priority_unmap_set(int unit, bcm_port_t port, 
                                     int internal_pri, bcm_color_t color,
                                     int pkt_pri, int cfi)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
        || defined(BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_color_prio_map)) {
        int index, port_shift = 5;
        egr_pri_cng_map_entry_t  pri_unmap;

        if ((internal_pri > 7) || 
            ((color != bcmColorGreen) && (color != bcmColorYellow) &&
             (color != bcmColorRed))) {
            return BCM_E_PARAM;
        }

        /* EGR_PRI_CNG_MAP table is indexed with
         * port[4:0] priority[2:0] CNG [1:0]
         */
        if (SOC_IS_TRX(unit)) {
            /* port[5:0] priority[3:0] CNG [1:0] */
            port_shift = 6;
        }
        index = (port << port_shift) | (internal_pri << 2) | 
                _BCM_COLOR_ENCODING(unit, color);

        memset(&pri_unmap, 0, sizeof(pri_unmap));
        soc_mem_field32_set(unit, EGR_PRI_CNG_MAPm, &pri_unmap, PRIf, pkt_pri);
        soc_mem_field32_set(unit, EGR_PRI_CNG_MAPm, &pri_unmap, CFIf, cfi);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL, index, &pri_unmap));
        return BCM_E_NONE;
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_port_vlan_priority_unmap_get
 * Description:
 *      Get the mapping of outgoing port, internal priority, and color to
 *  outgoing packet priority and cfi bit.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) Color
 *      pkt_pri      - (OUT) Packet priority
 *      cfi          - (OUT) Packet CFI
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_esw_port_vlan_priority_unmap_get(int unit, bcm_port_t port, 
                                     int internal_pri, bcm_color_t color,
                                     int *pkt_pri, int *cfi)
{
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
        || defined(BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_color_prio_map)) {
        int index, port_shift = 5;
        egr_pri_cng_map_entry_t  pri_unmap;

        if ((internal_pri > 7) || 
            ((color != bcmColorGreen) && (color != bcmColorYellow) &&
             (color != bcmColorRed))) {
            return BCM_E_PARAM;
        }

        /* EGR_PRI_CNG_MAP table is indexed with
         * port[4:0] priority[2:0] CNG [1:0]
         */
        if (SOC_IS_TRX(unit)) {
            /* port[5:0] priority[3:0] CNG [1:0] */
            port_shift = 6;
        }
        index = (port << port_shift) | (internal_pri << 2) | 
                _BCM_COLOR_ENCODING(unit, color);

        SOC_IF_ERROR_RETURN
            (READ_EGR_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY, index, &pri_unmap));
        *pkt_pri = soc_mem_field32_get(unit, EGR_PRI_CNG_MAPm, 
                                       &pri_unmap, PRIf);
        *cfi = soc_mem_field32_get(unit, EGR_PRI_CNG_MAPm, &pri_unmap, CFIf);
        return BCM_E_NONE;
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_port_l3_modify_set
 * Description:
 *      Enable/Disable ingress port based L3 unicast packet operations.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - bitmap of the packet operations to be enabled or disabled.
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int 
bcm_esw_port_l3_modify_set(int unit, bcm_port_t port, uint32 flags)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    uint64 r;
    uint32 addr;
    soc_reg_t reg;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
#if defined (BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        if (IS_HG_PORT(unit, port)) {
            return BCM_E_UNAVAIL;
        }
        reg = EGR_PORT_L3UC_MODS_TABLEr;
    } else 
#endif /* BCM_EASYRIDER_SUPPORT */
    if (SOC_IS_XGS3_SWITCH(unit)) {
        reg = IS_ST_PORT(unit, port) ?
            IEGR_PORT_L3UC_MODSr : EGR_PORT_L3UC_MODSr;
    } else {
        return BCM_E_UNAVAIL;
    }

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return BCM_E_UNAVAIL;
    }

    addr = soc_reg_addr(unit, reg, port, 0);
    BCM_IF_ERROR_RETURN(soc_reg_read(unit, reg, addr, &r));

    soc_reg64_field32_set(unit, reg, &r, L3_UC_SA_DISABLEf, 
                          (flags & BCM_PORT_L3_MODIFY_NO_SRCMAC) ? 1 : 0);

    soc_reg64_field32_set(unit, reg, &r, L3_UC_DA_DISABLEf, 
                          (flags & BCM_PORT_L3_MODIFY_NO_DSTMAC) ? 1 : 0);

    soc_reg64_field32_set(unit, reg, &r, L3_UC_TTL_DISABLEf, 
                          (flags & BCM_PORT_L3_MODIFY_NO_TTL) ? 1 : 0);

    soc_reg64_field32_set(unit, reg, &r, L3_UC_VLAN_DISABLEf, 
                          (flags & BCM_PORT_L3_MODIFY_NO_VLAN) ? 1 : 0);

    return soc_reg_write(unit, reg, addr, r);
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_port_l3_modify_get
 * Description:
 *      Get ingress port based L3 unicast packet operations status.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - (OUT) pointer to uint32 where bitmap of the current L3 packet 
 *              operations is returned.
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 * Notes:
 *      Available on XGS3 only.
 */
int 
bcm_esw_port_l3_modify_get(int unit, bcm_port_t port, uint32 *flags)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    uint64 r;
    uint32 addr;
    soc_reg_t reg;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        if (IS_HG_PORT(unit, port)) {
            return BCM_E_UNAVAIL;
        }
        reg = EGR_PORT_L3UC_MODS_TABLEr;
    } else 
#endif /* BCM_EASYRIDER_SUPPORT */
    if (SOC_IS_XGS3_SWITCH(unit)) {
        reg = IS_ST_PORT(unit, port) ?
            IEGR_PORT_L3UC_MODSr : EGR_PORT_L3UC_MODSr;
    } else {
        return BCM_E_UNAVAIL;
    }

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return BCM_E_UNAVAIL;
    }

    addr = soc_reg_addr(unit, reg, port, 0);
    BCM_IF_ERROR_RETURN(soc_reg_read(unit, reg, addr, &r));

    *flags = 0;

    if (soc_reg64_field32_get(unit, reg, r, L3_UC_SA_DISABLEf)) {
        *flags |= BCM_PORT_L3_MODIFY_NO_SRCMAC;
    }

    if (soc_reg64_field32_get(unit, reg, r, L3_UC_DA_DISABLEf)) {
        *flags |= BCM_PORT_L3_MODIFY_NO_DSTMAC;
    }

    if (soc_reg64_field32_get(unit, reg, r, L3_UC_TTL_DISABLEf)) {
        *flags |= BCM_PORT_L3_MODIFY_NO_TTL;
    }

    if (soc_reg64_field32_get(unit, reg, r, L3_UC_VLAN_DISABLEf)) {
        *flags |= BCM_PORT_L3_MODIFY_NO_VLAN;
    }

    return BCM_E_NONE;
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_port_ipmc_modify_set
 * Description:
 *      Enable/Disable ingress port based L3 multicast packet operations.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - bitmap of the packet operations to be enabled or disabled.
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int 
bcm_esw_port_ipmc_modify_set(int unit, bcm_port_t port, uint32 flags)
{
#if defined(INCLUDE_L3)
    int          idx;
    soc_field_t  fields[]={DISABLE_SA_REPLACEf, DISABLE_TTL_DECREMENTf};
    uint32       values[]={0, 0};

    if (!soc_feature(unit, soc_feature_ip_mcast)) { 
        return BCM_E_UNAVAIL; 
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#if defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_RAPTOR(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_RAPTOR_SUPPORT */


    for (idx = 0; idx < COUNTOF(fields); idx++) {
        if (!SOC_REG_FIELD_VALID(unit, EGR_IPMC_CFG2r, fields[idx])) {
            return (BCM_E_UNAVAIL);
        }
    }

    values[0] = (flags & BCM_PORT_IPMC_MODIFY_NO_SRCMAC) ? 1 : 0;
    values[1] = (flags & BCM_PORT_IPMC_MODIFY_NO_TTL) ? 1 : 0;

    return soc_reg_fields32_modify(unit, EGR_IPMC_CFG2r, port, 
                                   COUNTOF(fields), fields, values);
#else /* INCLUDE_L3 */
    return (BCM_E_UNAVAIL);
#endif /* INCLUDE_L3 */
}

/*
 * Function:
 *      bcm_esw_port_ipmc_modify_get
 * Description:
 *      Enable/Disable ingress port based L3 multicast packet operations.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - (OUT) pointer to uint32 where bitmap of the current L3 packet
 *              operations is returned.
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 * Notes:
 *      Available on XGS3 only.
 */
int 
bcm_esw_port_ipmc_modify_get(int unit, bcm_port_t port, uint32 *flags)
{
#if defined(INCLUDE_L3)
    uint32 reg_val;

    if (!soc_feature(unit, soc_feature_ip_mcast)) { 
        return BCM_E_UNAVAIL; 
    }

#if defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_RAPTOR(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_RAPTOR_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (NULL == flags) {
        return (BCM_E_PARAM);
    }

    if (!SOC_REG_FIELD_VALID(unit, EGR_IPMC_CFG2r, DISABLE_SA_REPLACEf)) {
        return (BCM_E_UNAVAIL);
    }

    if (!SOC_REG_FIELD_VALID(unit, EGR_IPMC_CFG2r, DISABLE_TTL_DECREMENTf)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN(READ_EGR_IPMC_CFG2r(unit, port, &reg_val));

    *flags = 0;
    if (soc_reg_field_get(unit, EGR_IPMC_CFG2r, reg_val, DISABLE_SA_REPLACEf)) {
        *flags |=  BCM_PORT_IPMC_MODIFY_NO_SRCMAC;
    }
    if (soc_reg_field_get(unit, EGR_IPMC_CFG2r, reg_val, DISABLE_TTL_DECREMENTf)) {
        *flags |=  BCM_PORT_IPMC_MODIFY_NO_TTL;
    }

    return (BCM_E_NONE);
#else /* INCLUDE_L3 */
    return (BCM_E_UNAVAIL);
#endif /* INCLUDE_L3 */
}

/*
 * Function:
 *      bcm_esw_port_force_forward_set
 * Purpose:
 *      This function allows packets to bypass the normal forwarding
 *      logic and be sent out on a specific port instead.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      egr_port - Egress port number
 *      enable - Bypass switching logic and forward to egr_port
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 * Notes:
 *      All normal ingress processing (table lookups etc.) is still
 *      performed and used for contructing the Higig header if the
 *      forced egress port is a Higig port.
 */
int 
bcm_esw_port_force_forward_set(int unit, bcm_port_t port, 
                               bcm_port_t egr_port, int enable)
{
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) \
    || defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_force_forward)) {

        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
        if (enable) {
            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, egr_port, 
                                                             &egr_port));
        }

#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TR_VL(unit)) {
            pbmp_t pbmp;
            uint64 val;

            COMPILER_64_SET(val, 0, 0);
            SOC_PBMP_CLEAR(pbmp);
            if (enable) {
                SOC_PBMP_PORT_SET(pbmp, egr_port);
            }
            soc_reg64_field32_set(unit, LOCAL_SW_DISABLE_DEFAULT_PBM_64r, 
                                  &val, PORT_BITMAP_LOf, 
                                  SOC_PBMP_WORD_GET(pbmp, 0));
            if(!SOC_IS_ENDURO(unit)){
                soc_reg64_field32_set(unit, LOCAL_SW_DISABLE_DEFAULT_PBM_64r, 
                                      &val, PORT_BITMAP_HIf, 
                                      SOC_PBMP_WORD_GET(pbmp, 1));
            }
            
            if (IS_ST_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (WRITE_ILOCAL_SW_DISABLE_DEFAULT_PBM_64r(unit, port, val));
            } else {
                BCM_IF_ERROR_RETURN
                    (WRITE_LOCAL_SW_DISABLE_DEFAULT_PBM_64r(unit, port, val));
            }
        }  else 
#endif /* BCM_TRIUMPH_SUPPORT */  
        {      
#if defined(BCM_FIREBOLT2_SUPPORT) ||  defined(BCM_RAVEN_SUPPORT)  || \
            defined(BCM_SCORPION_SUPPORT)
            if ((SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                 SOC_IS_SC_CQ(unit)) && IS_ST_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (WRITE_ILOCAL_SW_DISABLE_DEFAULT_PBMr(unit, port,
                                                          (enable) ? 1 << egr_port : 0));
            } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */
            {
                BCM_IF_ERROR_RETURN
                    (WRITE_LOCAL_SW_DISABLE_DEFAULT_PBMr(unit, port,
                                    (enable) ? 1 << egr_port : 0));
            }
        }

#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(unit)) {
            uint32 val_32;
            BCM_IF_ERROR_RETURN
                (READ_LOCAL_SW_DISABLE_CTRLr(unit, port, &val_32));
            soc_reg_field_set(unit, LOCAL_SW_DISABLE_CTRLr,
                              &val_32, SW_MODEf, (enable) ? 1 : 0);
           
            return (WRITE_LOCAL_SW_DISABLE_CTRLr(unit, port, val_32));
        } else 
#endif
        {
            return _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                         LOCAL_SW_DISABLEf, 
                                         (enable) ? 1 : 0);
        }
    }
#endif /* BCM_BRADLEY_SUPPORT || BCM_FIREBOLT2_SUPPORT \
          || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_port_force_forward_get
 * Purpose:
 *      Determine forced forwarding setting for a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      egr_port - (OUT) Egress port number
 *      enabled - (OUT) Forced forwarding enabled
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 * Notes:
 *      If the value of enabled is zero, the value of egr_port should 
 *      be ignored.
 */
int 
bcm_esw_port_force_forward_get(int unit, bcm_port_t port, 
                               bcm_port_t *egr_port, int *enabled)
{
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) \
        || defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_force_forward)) {
        uint32 egr_pbm;
#if defined(BCM_TRIUMPH_SUPPORT)
        uint64 val;
        uint32 val_32;
#endif /* BCM_TRIUMPH_SUPPORT */
        int ep = 0;

        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TR_VL(unit)) {
            BCM_IF_ERROR_RETURN
                (READ_LOCAL_SW_DISABLE_CTRLr(unit, port, &val_32));
            *enabled = soc_reg_field_get(unit, LOCAL_SW_DISABLE_CTRLr, 
                                         val_32, SW_MODEf);
        } else
#endif
        {
            _bcm_esw_port_tab_get(unit, port, LOCAL_SW_DISABLEf, enabled);
        }

        if (*enabled) {
            _bcm_gport_dest_t   dest;
            bcm_module_t        modid;
            int                 isGport = 0;

            _bcm_gport_dest_t_init(&dest);
#if defined(BCM_TRIUMPH_SUPPORT)
            if (SOC_IS_TR_VL(unit)) {
                BCM_IF_ERROR_RETURN
                    (READ_LOCAL_SW_DISABLE_DEFAULT_PBM_64r(unit, port, &val));
                egr_pbm = 
                   soc_reg64_field32_get(unit, LOCAL_SW_DISABLE_DEFAULT_PBM_64r,
                                         val, PORT_BITMAP_LOf);
                if ((egr_pbm == 0)&&(!SOC_IS_ENDURO(unit))) {
                    ep = 32;
                    egr_pbm = soc_reg64_field32_get(unit, 
                                  LOCAL_SW_DISABLE_DEFAULT_PBM_64r,
                                  val, PORT_BITMAP_HIf);
                }
            } else 
#endif /* BCM_TRIUMPH_SUPPORT */
            if ((SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                 SOC_IS_SC_CQ(unit)) && IS_ST_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (READ_ILOCAL_SW_DISABLE_DEFAULT_PBMr(unit, port, &egr_pbm));
            } else {
                BCM_IF_ERROR_RETURN
                    (READ_LOCAL_SW_DISABLE_DEFAULT_PBMr(unit, port, &egr_pbm));
            }
            if (egr_pbm == 0) {
                return BCM_E_INTERNAL;
            }
            /* Convert port bitmap into port number */
            while ((egr_pbm >>= 1) != 0) {
                ep++;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
            if (isGport) {
                if (IS_ST_PORT(unit, ep)) {
                    dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
                } else {
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_stk_my_modid_get(unit, &modid));
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    dest.modid = modid;
                }
                dest.port = ep;

                BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_construct(unit, &dest, &ep));
            }
            *egr_port = ep;
        }
        return BCM_E_NONE;
    }
#endif /* BCM_BRADLEY_SUPPORT || BCM_FIREBOLT2_SUPPORT \
          || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_port_err_sym_detect_set
 * Description:
 *      Enable/Disable XAUI error symbol monitoring feature.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      enable - TRUE, enable |E| monitoring feature on the port.
 *               FALSE, disable |E| monitoring feature on the port.
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 */
#ifdef BCM_XGS3_SWITCH_SUPPORT
STATIC int
_bcm_esw_port_err_sym_detect_set(int unit, bcm_port_t port, int enable) 
{
    int rv;

    SOC_DEBUG_PRINT((DK_PORT,
                     "_bcm_esw_port_err_sym_detect_set: u=%d p=%d enable=%d\n",
                     unit, port, enable));

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) 
#ifdef BCM_GXPORT_SUPPORT
        || IS_GX_PORT(unit, port)
#endif /* BCM_GXPORT_SUPPORT */
                                 ) {
        PORT_LOCK(unit);
        rv = soc_xaui_err_sym_detect_set(unit, port, enable);
        PORT_UNLOCK(unit);
    } else {
        rv = BCM_E_UNAVAIL;
    }


    return rv;
}
#endif

/*
 * Function:
 *      _bcm_esw_port_err_sym_detect_get
 * Description:
 *      Get the status of XAUI error symbol monitoring feature.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      status - (OUT) TRUE, port is enabled, FALSE port is disabled.
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 */
#ifdef BCM_XGS3_SWITCH_SUPPORT
STATIC int
_bcm_esw_port_err_sym_detect_get(int unit, bcm_port_t port, int *status) 
{
    int rv;

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) 
#ifdef BCM_GXPORT_SUPPORT
        || IS_GX_PORT(unit, port)
#endif /* BCM_GXPORT_SUPPORT */
                                 ) {
        PORT_LOCK(unit);
        rv = soc_xaui_err_sym_detect_get(unit, port, status);
        PORT_UNLOCK(unit);
    } else {
        rv = BCM_E_UNAVAIL;
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "_bcm_esw_port_err_sym_detect_get: u=%d p=%d status=%d\n",
                     unit, port, *status));

    return rv;
}
#endif

/*
 * Function:
 *      _bcm_esw_port_err_sym_count_get
 * Description:
 *      Get the number of |E| symbol in XAUI lanes since last read.
 *      The |E| symbol count is cleared on read.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      count - (OUT) Number of |E| error since last read.
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 *      BCM_E_INIT    - Error symbol detect feature is not enabled
 */
STATIC int 
_bcm_esw_port_err_sym_count_get(int unit, bcm_port_t port, int *count) 
{
    int rv;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
 
    rv = BCM_E_UNAVAIL;

#ifdef BCM_XGS3_SWITCH_SUPPORT

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) 
#ifdef BCM_GXPORT_SUPPORT
        || IS_GX_PORT(unit, port)
#endif /* BCM_GXPORT_SUPPORT */
                                 ) {
        PORT_LOCK(unit);
        rv = soc_xaui_err_sym_count(unit, port, count); 
        PORT_UNLOCK(unit);
    }
#endif

    SOC_DEBUG_PRINT((DK_PORT,
                     "_bcm_esw_port_err_sym_count_get: u=%d p=%d count=%d\n",
                     unit, port, *count));

    return rv;
}

STATIC int 
_bcm_esw_port_control_bridge_set(int unit, bcm_port_t port, int value)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        if (IS_E_PORT(unit, port) || IS_CPU_PORT(unit, port)) {
            if (SOC_REG_IS_VALID(unit, PORT_BRIDGE_BMAP_64r)) {
                bcm_pbmp_t  pbmp;            /* Port bitmap to update.*/
                uint64      buf64;           /* Buffer for reg64 value. */
                SOC_PBMP_CLEAR(pbmp);

                BCM_IF_ERROR_RETURN
                    (READ_PORT_BRIDGE_BMAP_64r(unit, &buf64));

                /* Set l3 port bitmap. */
                SOC_PBMP_WORD_SET(pbmp, 0,  
                    soc_reg64_field32_get(unit, PORT_BRIDGE_BMAP_64r, 
                                          buf64, BITMAP_LOf));
                if(!SOC_IS_ENDURO(unit)) {
                    SOC_PBMP_WORD_SET(pbmp, 1,  
                        soc_reg64_field32_get(unit, PORT_BRIDGE_BMAP_64r, 
                                              buf64, BITMAP_HIf));
                }

                if (SOC_PBMP_MEMBER(pbmp, port) && value) {
                    return BCM_E_NONE;
                }
                if (!SOC_PBMP_MEMBER(pbmp, port) && !value) {
                    return BCM_E_NONE;
                }
                if (!SOC_PBMP_MEMBER(pbmp, port) && value) {
                    SOC_PBMP_PORT_ADD(pbmp, port);
                } else if (SOC_PBMP_MEMBER(pbmp, port) && !value) {
                    SOC_PBMP_PORT_REMOVE(pbmp, port);
                }

                soc_reg64_field32_set(unit, PORT_BRIDGE_BMAP_64r,
                                      &buf64, BITMAP_LOf,
                                      SOC_PBMP_WORD_GET(pbmp, 0));
                if(!SOC_IS_ENDURO(unit)) {
                    soc_reg64_field32_set(unit, PORT_BRIDGE_BMAP_64r,
                                          &buf64, BITMAP_HIf,
                                          SOC_PBMP_WORD_GET(pbmp, 1));
                }

                BCM_IF_ERROR_RETURN
                    (WRITE_PORT_BRIDGE_BMAP_64r(unit, buf64));
            } 

            return _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       PORT_BRIDGEf, (value) ? 1 : 0);
        }
        return (BCM_E_UNAVAIL);
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
        if (SOC_IS_XGS3_SWITCH(unit) &&
            (IS_E_PORT(unit, port) || IS_CPU_PORT(unit, port))) {
            rv =_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       PORT_BRIDGEf, (value) ? 1 : 0);
            BCM_IF_ERROR_RETURN(rv);
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_SCORPION_SUPPORT)
            if (soc_feature(unit, soc_feature_src_trunk_port_bridge)) {
                bcm_pbmp_t  pbmp;            /* Port bitmap to update.*/
                uint32      port_bridge_map;
#if defined (BCM_RAPTOR_SUPPORT)
                uint32 port_bridge_map_hi;
#endif

                SOC_PBMP_CLEAR(pbmp);
                BCM_IF_ERROR_RETURN
                    (READ_PORT_BRIDGE_BMAPr(unit, &port_bridge_map));

                SOC_PBMP_WORD_SET(pbmp, 0,  
                    soc_reg_field_get(unit, PORT_BRIDGE_BMAPr, 
                                      port_bridge_map, BITMAPf));
#if defined (BCM_RAPTOR_SUPPORT)
                if (soc_feature(unit, soc_feature_register_hi)) {
                    BCM_IF_ERROR_RETURN
                        (READ_PORT_BRIDGE_BMAP_HIr(unit,
                                                   &port_bridge_map_hi));
                    SOC_PBMP_WORD_SET(pbmp, 1,  
                        soc_reg_field_get(unit, PORT_BRIDGE_BMAP_HIr, 
                                          port_bridge_map_hi, BITMAPf));
                }
#endif
                if (!SOC_PBMP_MEMBER(pbmp, port) && value) {
                    SOC_PBMP_PORT_ADD(pbmp, port);
                    soc_reg_field_set(unit, PORT_BRIDGE_BMAPr,
                                      &port_bridge_map, BITMAPf,
                                      SOC_PBMP_WORD_GET(pbmp, 0));
                    BCM_IF_ERROR_RETURN
                        (WRITE_PORT_BRIDGE_BMAPr(unit, port_bridge_map));
#if defined (BCM_RAPTOR_SUPPORT)

                    if (soc_feature(unit, soc_feature_register_hi)) {
                        soc_reg_field_set(unit, PORT_BRIDGE_BMAP_HIr,
                                          &port_bridge_map_hi, BITMAPf,
                                          SOC_PBMP_WORD_GET(pbmp, 1));
                        BCM_IF_ERROR_RETURN
                            (WRITE_PORT_BRIDGE_BMAP_HIr(unit,
                                                        port_bridge_map_hi));
                    }
#endif
                } else if (SOC_PBMP_MEMBER(pbmp, port) && !value) {
                    SOC_PBMP_PORT_REMOVE(pbmp, port);
                    soc_reg_field_set(unit, PORT_BRIDGE_BMAPr,
                                      &port_bridge_map, BITMAPf,
                                      SOC_PBMP_WORD_GET(pbmp, 0));
                    BCM_IF_ERROR_RETURN
                        (WRITE_PORT_BRIDGE_BMAPr(unit, port_bridge_map));
#if defined (BCM_RAPTOR_SUPPORT)
                    if (soc_feature(unit, soc_feature_register_hi)) {
                        soc_reg_field_set(unit, PORT_BRIDGE_BMAP_HIr,
                                          &port_bridge_map_hi, BITMAPf,
                                          SOC_PBMP_WORD_GET(pbmp, 1));
                        BCM_IF_ERROR_RETURN
                            (WRITE_PORT_BRIDGE_BMAP_HIr(unit,
                                                        port_bridge_map_hi));
                    }
#endif
                }
            }
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        }
#ifdef BCM_EASYRIDER_SUPPORT
        else if (SOC_IS_EASYRIDER(unit) && IS_HG_PORT(unit, port)) {
            bcm_port_cfg_t      pcfg;
            BCM_IF_ERROR_RETURN
                (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
            pcfg.pc_bridge_port = (value) ? 1 : 0;
            rv = mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg);
        }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_port_control_set
 * Description:
 *      Enable/Disable specified port feature.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      type - Enum value of the feature
 *      value - value to be set
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 */
int 
bcm_esw_port_control_set(int unit, bcm_port_t port, 
                         bcm_port_control_t type, int value)
{
    int rv = BCM_E_UNAVAIL;
    soc_reg_t egr_port_reg;
    egr_port_reg = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                    SOC_IS_VALKYRIE2(unit)) ? EGR_PORT_64r : EGR_PORTr;

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_control_set: u=%d p=%d rv=%d : %d\n",
                     unit, port, value, (int)type));

    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        rv = BCM_E_NONE;
    } else {
        rv = _bcm_esw_port_gport_validate(unit, port, &port);
    }
    BCM_IF_ERROR_RETURN(rv);
    rv = BCM_E_UNAVAIL;

    switch (type) {
    case bcmPortControlBridge:
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            rv = _bcm_tr2_wlan_port_set(unit, port, PORT_BRIDGEf, value);
        } else
#endif 
        {
            PORT_LOCK(unit);
            rv = _bcm_esw_port_control_bridge_set(unit, port, value);
            PORT_UNLOCK(unit);
        }
        break;
    case bcmPortControlTrunkHashSet:
#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       RTAG7_HASH_CFG_SEL_TRUNKf, 
                                       (value) ? 1 : 0);
        }
#endif /* BCM_BRADLEY_SUPPORT */
        break; 
    case bcmPortControlFabricTrunkHashSet:
#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       RTAG7_HASH_CFG_SEL_HIGIG_TRUNKf, 
                                       (value) ? 1 : 0);
        }
#endif /* BCM_BRADLEY_SUPPORT */
        break; 
    case bcmPortControlECMPHashSet:
#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       RTAG7_HASH_CFG_SEL_ECMPf, 
                                       (value) ? 1 : 0);
        }
#endif /* BCM_BRADLEY_SUPPORT */
        break; 
    case bcmPortControlLoadBalanceHashSet:
#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       RTAG7_HASH_CFG_SEL_LBIDf, 
                                       (value) ? 1 : 0);
        }
#endif /* BCM_BRADLEY_SUPPORT */
        break; 
    case bcmPortControlLoadBalancingNumber:
#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       RTAG7_PORT_LBNf, 
                                       (value) & 0xf);
        }
#endif /* BCM_BRADLEY_SUPPORT */
        break; 
    case bcmPortControlErrorSymbolDetect:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit)) {
             rv = _bcm_esw_port_err_sym_detect_set(unit, port, value);
        }
#endif
        break;
    case bcmPortControlErrorSymbolCount:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit)) {
            int temp;
            /* Error symbol count is read only and cleared on read. */
            rv = _bcm_esw_port_err_sym_count_get(unit, port, &temp);
        }
#endif
        break;
#if defined(INCLUDE_L3)
    case bcmPortControlIP4:
        if (SOC_IS_XGS3_SWITCH(unit) && 
            (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port))) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       V4L3_ENABLEf, (value) ? 1 : 0);
#if defined(BCM_TRIUMPH_SUPPORT)
            if (BCM_SUCCESS(rv) && 
                soc_feature(unit, soc_feature_esm_support)) {
                rv = _bcm_tr_l3_enable(unit, port, 0, (value) ? 1 : 0);
            }
#endif /* BCM_TRIUMPH_SUPPORT */
        }
        break;
    case bcmPortControlIP6:
        if (SOC_IS_XGS3_SWITCH(unit) && 
            (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port))) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       V6L3_ENABLEf, (value) ? 1 : 0);
#if defined(BCM_TRIUMPH_SUPPORT)
            if (BCM_SUCCESS(rv) && 
                soc_feature(unit, soc_feature_esm_support)) {
                rv = _bcm_tr_l3_enable(unit, port, BCM_L3_IP6, 
                                       (value) ? 1 : 0);
            }
#endif /* BCM_TRIUMPH_SUPPORT */
        }
        break;
    case bcmPortControlIP4Mcast:
        if (SOC_IS_XGS3_SWITCH(unit) && 
            (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port))) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       V4IPMC_ENABLEf, (value) ? 1 : 0);
        }
        break;
    case bcmPortControlIP6Mcast:
        if (SOC_IS_XGS3_SWITCH(unit) && 
            (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port))) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       V6IPMC_ENABLEf, (value) ? 1 : 0);
        }
        break;
    case bcmPortControlMpls:
#if defined(BCM_EASYRIDER_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
        if ((BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port)) &&
            SOC_MEM_FIELD_VALID(unit, PORT_TABm, MPLS_ENABLEf)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       MPLS_ENABLEf, (value) ? 1 : 0);
        }
#endif
        break;
    case bcmPortControlMacInMac:
#if defined(BCM_TRIUMPH2_SUPPORT)
        if ((BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port)) &&
            SOC_MEM_FIELD_VALID(unit, PORT_TABm, MIM_TERM_ENABLEf)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       MIM_TERM_ENABLEf, (value) ? 1 : 0);
        }
#endif
        break;
    case bcmPortControlMplsIngressPortCheck:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_EASYRIDER_SUPPORT
        if (!IS_ST_PORT(unit, port) &&
            SOC_MEM_FIELD_VALID(unit, PORT_TABm, MPLS_PORT_CHECKf)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       MPLS_PORT_CHECKf, (value) ? 1 : 0);
        }
#endif
        break;
    case bcmPortControlMplsMultiLabelSwitching:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_EASYRIDER_SUPPORT
        if (!IS_ST_PORT(unit, port) &&
            SOC_MEM_FIELD_VALID(unit, PORT_TABm, THREE_MPLS_LABELf)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       THREE_MPLS_LABELf, (value) ? 1 : 0);
        }
#endif
        break;
#endif /* INCLUDE_L3 */
    case bcmPortControlIP4McastL2:
#ifdef BCM_TRX_SUPPORT
        if ((BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port)) &&
            SOC_MEM_FIELD_VALID(unit, PORT_TABm, V4IPMC_L2_ENABLEf)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       V4IPMC_L2_ENABLEf, (value) ? 1 : 0);
        }
#endif
        break;
    case bcmPortControlIP6McastL2:
#ifdef BCM_TRX_SUPPORT
        if ((BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port)) &&
            SOC_MEM_FIELD_VALID(unit, PORT_TABm, V6IPMC_L2_ENABLEf)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                                       V6IPMC_L2_ENABLEf, (value) ? 1 : 0);
        }
#endif
        break;
    case bcmPortControlPassControlFrames:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit) && IS_E_PORT(unit, port)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                       PASS_CONTROL_FRAMESf, (value) ? 1 : 0);
        }
#endif
#ifdef BCM_XGS12_SWITCH_SUPPORT
        if ((SOC_IS_DRACO(unit) || SOC_IS_TUCANA(unit)) && 
            IS_E_PORT(unit, port)) {
            uint64 rsv_mask;
            /* Set/clear the Control Frame Purge bit */
            PORT_LOCK(unit);
            rv = READ_RSV_MASKr(unit, port, &rsv_mask);
            if (BCM_SUCCESS(rv)) {
                if (value) {
                    COMPILER_64_SET(rsv_mask, 0,
                                    COMPILER_64_LO(rsv_mask) & ~0x08000000);
                } else {
                    COMPILER_64_SET(rsv_mask, 0,
                                    COMPILER_64_LO(rsv_mask) | 0x08000000);
                }
                rv = WRITE_RSV_MASKr(unit, port, rsv_mask);
            }
            PORT_UNLOCK(unit);
        }
#endif
#ifdef BCM_XGS_SWITCH_SUPPORT
        if (IS_XE_PORT(unit, port)) {
            uint64 rx_ctrl;
            /* Enable Control Frames in BigMAC */
            PORT_LOCK(unit);
            rv = READ_MAC_RXCTRLr(unit, port, &rx_ctrl);
            if (BCM_SUCCESS(rv)) {
                soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, RXPASSCTRLf,
                                      (value) ? 1 : 0);
                rv = WRITE_MAC_RXCTRLr(unit, port, rx_ctrl);
            }
            PORT_UNLOCK(unit);
        }
#endif
        break;
#ifdef BCM_XGS3_SWITCH_SUPPORT
      case bcmPortControlFilterLookup:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
          value = value ? 1 : 0; 
          if (soc_mem_field_valid(unit, PORT_TABm, VFP_ENABLEf)) {
              rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                         VFP_ENABLEf, value);
          } 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
          break;
      case bcmPortControlFilterIngress:
          value = value ? 1 : 0;
              /* Enable/disable ingress filtering. */
          rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                     FILTER_ENABLEf, value);
          break;
      case bcmPortControlFilterEgress:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        } else
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        {
             value = value ? 1 : 0;
             /* Enable/disable egress filtering. */
             if (SOC_REG_FIELD_VALID(unit, egr_port_reg, EFP_FILTER_ENABLEf)) {
                 rv = soc_reg_field32_modify(unit, egr_port_reg, port,
                                             EFP_FILTER_ENABLEf, value);
             }
         }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
          break;
      case bcmPortControlFrameSpacingStretch:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
          rv = (MAC_CONTROL_SET(PORT(unit, port).p_mac, unit, port,
                                  SOC_MAC_CONTROL_FRAME_SPACING_STRETCH,
                                  value));
          break;
      case bcmPortControlPreservePacketPriority:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
          rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                     USE_INCOMING_DOT1Pf, (value) ? 1 : 0);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || defined(BCM_RAVEN_SUPPORT) */
	  break;	
      case bcmPortControlLearnClassEnable:
#if defined(BCM_TRX_SUPPORT) 
          if (soc_feature(unit, soc_feature_class_based_learning)) {
              rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                         CLASS_BASED_SM_ENABLEf, value ? 1 : 0);
          }
#endif /* BCM_TRX_SUPPORT */
          break;
      case bcmPortControlTrustIncomingVlan:
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_TRX_SUPPORT)
          rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                     TRUST_INCOMING_VIDf, (value) ? 1 : 0);
#endif /* BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
          break;
      case bcmPortControlDoNotCheckVlan:
#if defined(BCM_TRX_SUPPORT)
          rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                     DISABLE_VLAN_CHECKSf, (value) ? 1 : 0);
#endif /* BCM_TRX_SUPPORT */
          break;
       case bcmPortControlIEEE8021ASEnableIngress:
          if (soc_feature(unit, soc_feature_rx_timestamp)) {
              rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                     IEEE_802_1AS_ENABLEf, (value) ? 1 : 0);
          }
          break;
      case bcmPortControlIEEE8021ASEnableEgress:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_HAWKEYE_SUPPORT)
          if (SOC_IS_HAWKEYE(unit)) {
              rv = soc_reg_field32_modify(unit, EGR_PORTr, port,
                                         IEEE_802_1AS_ENABLEf, (value) ? 1 : 0);
          }
#endif /* BCM_HAWKEYE_SUPPORT */
          break;
      case bcmPortControlEgressVlanPriUsesPktPri:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRX_SUPPORT) 
          if (SOC_IS_TRX(unit)) {
              rv = soc_reg_field32_modify(unit, EGR_VLAN_CONTROL_1r, port,
                                         REMARK_OUTER_DOT1Pf,
                                         value ? 0 : 1);
          }
#endif /* BCM_TRX_SUPPORT */
          break;
      case bcmPortControlEgressModifyDscp:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRX_SUPPORT)
          if (SOC_IS_TRX(unit)) {
              rv = soc_reg_field32_modify(unit, EGR_VLAN_CONTROL_1r, port,
                                          REMARK_OUTER_DSCPf,
                                          value ? 1 : 0);
          }
#endif /* BCM_TRX_SUPPORT */
          break;
    case bcmPortControlIpfixRate:
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_ipfix_rate)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       IPFIX_FLOW_METER_IDf, value);
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        break;
    case bcmPortControlCustomerQueuing:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_vlan_queue_map)) {
            uint32 rval, fval;
            if (value) {
                fval = 3;
            } else {
                fval = 0;
            }
            BCM_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &rval));
            soc_reg_field_set(unit, ING_COS_MODEr, &rval, SELECTf, fval);
            BCM_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, port, rval));
            BCM_IF_ERROR_RETURN(READ_COS_MODEr(unit, port, &rval));
            soc_reg_field_set(unit, COS_MODEr, &rval, SELECTf, fval);
            rv = WRITE_COS_MODEr(unit, port, rval);
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        break;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT)
      case bcmPortControlOAMEnable:
          if (BCM_GPORT_IS_WLAN_PORT(port)) {
              return BCM_E_UNAVAIL;
          }
          rv = _bcm_esw_iport_tab_set(unit, port, OAM_ENABLEf, value ? 1 : 0);
          break;
#endif /* defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) */

#ifdef BCM_TRX_SUPPORT
      case bcmPortControlOamLoopback:
          if (BCM_GPORT_IS_WLAN_PORT(port)) {
              return BCM_E_UNAVAIL;
          }
          if (SOC_IS_TRX(unit))
          {
              rv = soc_reg_field32_modify(unit, ING_MISC_PORT_CONFIGr, port,
                  OAM_DO_NOT_MODIFYf, value ? 1 : 0);
          }

          break;
#endif /* BCM_TRX_SUPPORT */

    case bcmPortControlLanes:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_port)) {
            PORT_LOCK(unit);
            rv = _bcm_tr2_port_lanes_set(unit, port, value);
            PORT_UNLOCK(unit);
        }
#endif
        break;

      case bcmPortControlPFCEthertype:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              BCM_IF_ERROR_RETURN
                  (soc_reg_field32_modify(unit, MAC_PFC_TYPEr, port,
                                          PFC_ETH_TYPEf,
                                          value & 0xffff));
              rv = soc_reg_field32_modify(unit, BMAC_PFC_TYPEr, port,
                                          PFC_ETH_TYPEf,
                                          value & 0xffff);
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;

      case bcmPortControlPFCOpcode:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              BCM_IF_ERROR_RETURN
                  (soc_reg_field32_modify(unit, MAC_PFC_OPCODEr, port,
                                          PFC_OPCODEf,
                                          value & 0xffff));
              rv = soc_reg_field32_modify(unit, BMAC_PFC_OPCODEr, port,
                                          PFC_OPCODEf,
                                          value & 0xffff);
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;

      case bcmPortControlPFCReceive:
      case bcmPortControlPFCTransmit:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 rval;
              int pfc_enable = FALSE;
              
              /* We don't just use reg_modify because we need the other
               * field below. */
              BCM_IF_ERROR_RETURN(READ_UNIMAC_PFC_CTRLr(unit, port, &rval));
              soc_reg_field_set(unit, UNIMAC_PFC_CTRLr, &rval, 
                                (type == bcmPortControlPFCReceive) ?
                                PFC_RX_ENBLf : PFC_TX_ENBLf,
                                value ? 1 : 0);
              BCM_IF_ERROR_RETURN(WRITE_UNIMAC_PFC_CTRLr(unit, port, rval));

              BCM_IF_ERROR_RETURN
                  (soc_reg_field32_modify(unit, MAC_CTRLr, port,
                                   (type == bcmPortControlPFCReceive) ?
                                          PP_RX_ENABLEf : PP_TX_ENABLEf,
                                          value ? 1 : 0));

              if (type == bcmPortControlPFCReceive) {
                  BCM_IF_ERROR_RETURN
                      (soc_reg_field32_modify(unit, MMU_LLFC_RX_CONFIGr,
                                              port, RX_ENABLEf,
                                              value ? 1 : 0));
                  if (value == 0) {
                      /* Disabling RX, flush MMU XOFF state */
                      BCM_IF_ERROR_RETURN
                          (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                                  FORCE_PFC_XONf, 1));
                      BCM_IF_ERROR_RETURN
                          (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                                  FORCE_PFC_XONf, 0));
                      BCM_IF_ERROR_RETURN
                          (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr,
                                                  port, FORCE_PFC_XONf, 1));
                      BCM_IF_ERROR_RETURN
                          (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr,
                                                  port, FORCE_PFC_XONf, 0));
                  }
              } else {
                  rval = 0;
                  if (value) {
                      soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                                        PORT_PRI_XON_ENABLEf, 0xff);   
                      soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                                        PORT_PG7PAUSE_DISABLEf, 1);   
                  }
                  SOC_IF_ERROR_RETURN
                      (WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));
              }

              /* Update feature/stats enable as OR of TX/RX enables */
              if (soc_reg_field_get(unit, UNIMAC_PFC_CTRLr,
                                    rval, PFC_RX_ENBLf) ||
                  soc_reg_field_get(unit, UNIMAC_PFC_CTRLr,
                                    rval, PFC_TX_ENBLf)) {
                  pfc_enable = TRUE;
              }
              
              /* Feature enable */
              BCM_IF_ERROR_RETURN
                  (soc_reg_field32_modify(unit, PPFC_ENr, port,
                                          PPFC_FEATURE_ENf,
                                          pfc_enable ? 1 : 0));
              /* Stats enable */
              BCM_IF_ERROR_RETURN
                  (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                          PFC_STATS_ENf,
                                          pfc_enable ? 1 : 0));
              rv = soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                          PFC_STATS_ENf,
                                          pfc_enable ? 1 : 0);
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;

      case bcmPortControlPFCClasses:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              int eight_class = TRUE;

              if (value == 16) {
                  eight_class = FALSE;
              } else if (value != 8) {
                  return BCM_E_PARAM;
              }
              BCM_IF_ERROR_RETURN
                  (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                          PFC_EIGHT_CLASSf,
                                          eight_class ? 1 : 0));
              rv = soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                          PFC_EIGHT_CLASSf,
                                          eight_class ? 1 : 0);
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;

      case bcmPortControlPFCPassFrames:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              BCM_IF_ERROR_RETURN
                  (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                          RX_PASS_PFC_FRMf,
                                          value ? 1 : 0));
              rv = soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                          RX_PASS_PFC_FRMf,
                                          value ? 1 : 0);
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;

      case bcmPortControlPFCDestMacOui:
      case bcmPortControlPFCDestMacNonOui:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 rval;
              
              BCM_IF_ERROR_RETURN
                  (READ_MAC_PFC_DA_0r(unit, port, &rval));
              if (type == bcmPortControlPFCDestMacNonOui) {
                  rval = (rval & 0xff000000) | (value & 0xffffff);
              } else {
                  rval = (rval & 0x00ffffff) | ((value & 0xff) << 24);
                  value = (value >> 8) & 0xffff;
                  BCM_IF_ERROR_RETURN
                      (soc_reg_field32_modify(unit, MAC_PFC_DA_1r, port,
                                              PFC_MACDA_1f, value));
                  BCM_IF_ERROR_RETURN
                      (soc_reg_field32_modify(unit, BMAC_PFC_DA_HIr, port,
                                              PFC_MACDA_HIf, value));
                  
              }
              BCM_IF_ERROR_RETURN(WRITE_MAC_PFC_DA_0r(unit, port, rval));
              rv = soc_reg_field32_modify(unit, BMAC_PFC_DA_LOr, port,
                                          PFC_MACDA_LOf, rval);
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;

      case bcmPortControlVrf:		
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIUMPH_SUPPORT)
      {
          if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, VRF_IDf)) {
              if  ((value > 0 ) && (value < SOC_VRF_MAX(unit) )) {
                   rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                             port, VRF_IDf, value);
                   BCM_IF_ERROR_RETURN(rv);
                   if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                         rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                   PORT_OPERATIONf, 0x3);
                   } 
              } else {
                   if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                       rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                             PORT_OPERATIONf, 0x0);
                   }
              }
          }
      }
#endif  /* BCM_TRIUMPH_SUPPORT */
        break;

      case bcmPortControlL3Ingress:		
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIUMPH_SUPPORT)
      {      
          if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, L3_IIFf)) {
              if ((value >= 1) && (value < 4096)) {
                   rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                             port, L3_IIFf, value);
                   BCM_IF_ERROR_RETURN(rv);
                   if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                        PORT_OPERATIONf, 0x2);
                   }
              } else {
                   if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                    PORT_OPERATIONf, 0x0);
                   }
              }
          } 
      }
#endif  /* BCM_TRIUMPH_SUPPORT */
        break;

      case bcmPortControlL2Learn:
#if defined(BCM_TRX_SUPPORT)
          /* BCM_PORT_LEARN_xxx maps directly to HW values */
          if (value & ~0xf) {
              return BCM_E_PARAM;
          }
          rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH, 
                                     CML_FLAGS_NEWf, value);
#endif /* BCM_TRX_SUPPORT */
          break;

      case bcmPortControlL2Move:
#if defined(BCM_TRX_SUPPORT)
          /* BCM_PORT_LEARN_xxx maps directly to HW values */
          if (value & ~0xf) {
              return BCM_E_PARAM;
          }
          rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH, 
                                     CML_FLAGS_MOVEf, value);
#endif /* BCM_TRX_SUPPORT */
          break;

      case bcmPortControlForwardStaticL2MovePkt:
#if defined(BCM_TRX_SUPPORT)
         rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                    DISABLE_STATIC_MOVE_DROPf, value ? 1 : 0);
#endif /* BCM_TRX_SUPPORT */
        break;

    default:
        break;
    }
    return rv;
}

/*
 * Function:
 *      bcm_esw_port_control_get
 * Description:
 *      Get the status of specified port feature.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      type - Enum  value of the feature
 *      value - (OUT) Current value of the port feature
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 */
int 
bcm_esw_port_control_get(int unit, bcm_port_t port, 
                         bcm_port_control_t type, int *value)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_GPORT_IS_WLAN_PORT(port)) {
        rv = BCM_E_NONE;
    } else {
        rv = _bcm_esw_port_gport_validate(unit, port, &port);
    }
    BCM_IF_ERROR_RETURN(rv);
    rv = BCM_E_UNAVAIL;

    switch (type) {
      case bcmPortControlBridge:
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            rv = _bcm_tr2_lport_field_get(unit, port, PORT_BRIDGEf, value);
        } else
#endif
        {
            if (SOC_IS_XGS3_SWITCH(unit) && !IS_ST_PORT(unit, port)) {
                rv = _bcm_esw_port_tab_get(unit, port, PORT_BRIDGEf, value);
            }
#ifdef BCM_EASYRIDER_SUPPORT
            else if (SOC_IS_EASYRIDER(unit) && IS_HG_PORT(unit, port)) {
                bcm_port_cfg_t      pcfg;

                PORT_LOCK(unit);
                rv = mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg);
                if (BCM_SUCCESS(rv)) {
                    *value = pcfg.pc_bridge_port;
                }
                PORT_UNLOCK(unit);
            }
#endif
        }
          break;
      case bcmPortControlTrunkHashSet:
#if defined(BCM_BRADLEY_SUPPORT)
          if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
              rv = _bcm_esw_port_tab_get(unit, port, 
                                         RTAG7_HASH_CFG_SEL_TRUNKf, value);
          }
#endif /* BCM_BRADLEY_SUPPORT */
          break;
      case bcmPortControlFabricTrunkHashSet:
#if defined(BCM_BRADLEY_SUPPORT)
          if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
              rv = _bcm_esw_port_tab_get(unit, port, 
                                         RTAG7_HASH_CFG_SEL_HIGIG_TRUNKf, value);
          }
#endif /* BCM_BRADLEY_SUPPORT */
          break;
      case bcmPortControlECMPHashSet:
#if defined(BCM_BRADLEY_SUPPORT)
          if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
              rv = _bcm_esw_port_tab_get(unit, port, 
                                         RTAG7_HASH_CFG_SEL_ECMPf, value);
          }
#endif /* BCM_BRADLEY_SUPPORT */
          break;
      case bcmPortControlLoadBalanceHashSet:
#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            rv = _bcm_esw_port_tab_get(unit, port, 
                                       RTAG7_HASH_CFG_SEL_LBIDf, value);
        }
#endif /* BCM_BRADLEY_SUPPORT */
        break;
    case bcmPortControlLoadBalancingNumber:
#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            rv = _bcm_esw_port_tab_get(unit, port, 
                                       RTAG7_PORT_LBNf, value);
        }
#endif /* BCM_BRADLEY_SUPPORT */
        break;
      case bcmPortControlErrorSymbolDetect:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit)) {
             rv = _bcm_esw_port_err_sym_detect_get(unit, port, value);
        }
#endif
        break;
      case bcmPortControlErrorSymbolCount:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit)) {
            rv = _bcm_esw_port_err_sym_count_get(unit, port, value);
        }
#endif
        break;
      case bcmPortControlIP4:
          if (SOC_IS_XGS3_SWITCH(unit) && 
              (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port))) {
              rv = _bcm_esw_port_tab_get(unit, port, V4L3_ENABLEf, value);
          }
          break;
      case bcmPortControlIP6:
          if (SOC_IS_XGS3_SWITCH(unit) && 
              (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port))) {
              rv = _bcm_esw_port_tab_get(unit, port, V6L3_ENABLEf, value);
          }
          break;
      case bcmPortControlIP4Mcast:
          if (SOC_IS_XGS3_SWITCH(unit) && 
              (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port))) {
              rv = _bcm_esw_port_tab_get(unit, port, V4IPMC_ENABLEf, value);
          }
          break;
      case bcmPortControlIP6Mcast:
          if (SOC_IS_XGS3_SWITCH(unit) && 
              (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port))) {
              rv = _bcm_esw_port_tab_get(unit, port, V6IPMC_ENABLEf, value);
          }
          break;
      case bcmPortControlIP4McastL2:
#ifdef BCM_TRX_SUPPORT
          if (SOC_IS_XGS3_SWITCH(unit) && 
              (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port)) && 
              SOC_MEM_FIELD_VALID(unit, PORT_TABm, V4IPMC_L2_ENABLEf)) {
              rv = _bcm_esw_port_tab_get(unit, port, V4IPMC_L2_ENABLEf, value);
          }
#endif
          break;
      case bcmPortControlIP6McastL2:
#ifdef BCM_TRX_SUPPORT
          if (SOC_IS_XGS3_SWITCH(unit) && 
              (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port)) && 
              SOC_MEM_FIELD_VALID(unit, PORT_TABm, V6IPMC_L2_ENABLEf)) {
              rv = _bcm_esw_port_tab_get(unit, port, V6IPMC_L2_ENABLEf, value);
          }
#endif
          break;

      case bcmPortControlMpls:
#if defined(BCM_EASYRIDER_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
          if (SOC_IS_XGS3_SWITCH(unit) && 
              (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port)) &&
              SOC_MEM_FIELD_VALID(unit, PORT_TABm, MPLS_ENABLEf)) {
              rv = _bcm_esw_port_tab_get(unit, port, MPLS_ENABLEf, value);
          }
#endif
          break;

      case bcmPortControlMacInMac:
#if defined(BCM_TRIUMPH2_SUPPORT)
          if (SOC_IS_XGS3_SWITCH(unit) && 
              (BCM_GPORT_IS_WLAN_PORT(port) || !IS_ST_PORT(unit, port)) &&
              SOC_MEM_FIELD_VALID(unit, PORT_TABm, MIM_TERM_ENABLEf)) {
              rv = _bcm_esw_port_tab_get(unit, port, MIM_TERM_ENABLEf, value);
          }
#endif
          break;

    case bcmPortControlMplsIngressPortCheck:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_EASYRIDER_SUPPORT
        if (!IS_ST_PORT(unit, port) &&
            SOC_MEM_FIELD_VALID(unit, PORT_TABm, MPLS_PORT_CHECKf)) {
            rv = _bcm_esw_port_tab_get(unit, port, MPLS_PORT_CHECKf, value);
        }
#endif /* BCM_EASYRIDER_SUPPORT */
        break;

    case bcmPortControlMplsMultiLabelSwitching:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_EASYRIDER_SUPPORT
        if (!IS_ST_PORT(unit, port) &&
            SOC_MEM_FIELD_VALID(unit, PORT_TABm, THREE_MPLS_LABELf)) {
            rv = _bcm_esw_port_tab_get(unit, port, THREE_MPLS_LABELf, value);
        }
#endif /* BCM_EASYRIDER_SUPPORT */
        break;

    case bcmPortControlPassControlFrames:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_XGS_SWITCH_SUPPORT
        if (IS_XE_PORT(unit, port)) {
            uint64 rx_ctrl;
            rv = READ_MAC_RXCTRLr(unit, port, &rx_ctrl);
            if (BCM_SUCCESS(rv)) {
                *value = soc_reg64_field32_get(unit, MAC_RXCTRLr, rx_ctrl, 
                                               RXPASSCTRLf);
            }
        }
#endif
#ifdef BCM_XGS12_SWITCH_SUPPORT
        if ((SOC_IS_DRACO(unit) || SOC_IS_TUCANA(unit)) && 
            IS_E_PORT(unit, port)) {
            uint64 rsv_mask;
            rv = READ_RSV_MASKr(unit, port, &rsv_mask);
            if (BCM_SUCCESS(rv)) {
                *value = (COMPILER_64_LO(rsv_mask) & 0x08000000) ? 0 : 1;
            }
        }
#endif
        break;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    case bcmPortControlFilterLookup:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        if (soc_mem_field_valid(unit, PORT_TABm, VFP_ENABLEf)) {
            rv = _bcm_esw_port_tab_get(unit, port, VFP_ENABLEf, value);
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case bcmPortControlFilterIngress:
        rv = _bcm_esw_port_tab_get(unit, port, FILTER_ENABLEf, value);
        break;
    case bcmPortControlFilterEgress:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        } else
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        {
            if (SOC_REG_FIELD_VALID(unit, EGR_PORTr, EFP_FILTER_ENABLEf)) {
                uint32 egr_val;
                uint64 egr_val64;
            
                if (SOC_REG_IS_VALID(unit, EGR_PORT_64r)) {
                    rv = READ_EGR_PORT_64r(unit, port, &egr_val64);
                    if (BCM_SUCCESS(rv)) {
                        *value = soc_reg64_field32_get(unit, EGR_PORT_64r, 
                                                       egr_val64,
                                                       EFP_FILTER_ENABLEf);
                    }
                } else {
                    rv = READ_EGR_PORTr(unit, port, &egr_val);
                    if (BCM_SUCCESS(rv)) {
                        *value = soc_reg_field_get(unit, EGR_PORTr, egr_val,
                                                   EFP_FILTER_ENABLEf);
                    }
                }
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
      case bcmPortControlFrameSpacingStretch:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
          rv = MAC_CONTROL_GET(PORT(unit, port).p_mac, unit, port,
                               SOC_MAC_CONTROL_FRAME_SPACING_STRETCH, value);
          break;
      case bcmPortControlPreservePacketPriority:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
          rv = _bcm_esw_port_tab_get(unit, port, USE_INCOMING_DOT1Pf, value);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
	  break;	
      case bcmPortControlLearnClassEnable:
#if defined(BCM_TRX_SUPPORT) 
           if (soc_feature(unit, soc_feature_class_based_learning)) {
               rv = _bcm_esw_port_tab_get(unit, port, CLASS_BASED_SM_ENABLEf,
                                          value);
           }
#endif /* BCM_TRX_SUPPORT */
          break;
      case bcmPortControlTrustIncomingVlan:
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_TRX_SUPPORT)
           rv = _bcm_esw_port_tab_get(unit, port, TRUST_INCOMING_VIDf, value);
#endif /* BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
          break;
      case bcmPortControlDoNotCheckVlan:
#if defined(BCM_TRX_SUPPORT)
          rv = _bcm_esw_port_tab_get(unit, port, DISABLE_VLAN_CHECKSf, value);
#endif /* BCM_TRX_SUPPORT */
          break;
       case bcmPortControlIEEE8021ASEnableIngress:
#if defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_ENDURO_SUPPORT)
          if (soc_feature(unit, soc_feature_rx_timestamp)) {
              rv = _bcm_esw_port_tab_get(unit, port, IEEE_802_1AS_ENABLEf, value);
          }
#endif /* BCM_HAWKEYE_SUPPORT */
          break;
       case bcmPortControlIEEE8021ASEnableEgress:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_HAWKEYE_SUPPORT)
          if (SOC_IS_HAWKEYE(unit)) {
              uint32 egr_val;

              rv = READ_EGR_PORTr(unit, port, &egr_val);
              if (BCM_SUCCESS(rv)) {
                  *value = soc_reg_field_get(unit, EGR_PORTr, egr_val,
                                           IEEE_802_1AS_ENABLEf);
              }
          }
#endif /* BCM_HAWKEYE_SUPPORT */
          break;
      case bcmPortControlEgressVlanPriUsesPktPri:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRX_SUPPORT) 
          if (SOC_IS_TRX(unit)) {
              uint32 val;
              rv = READ_EGR_VLAN_CONTROL_1r(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = (soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r, val,
                                              REMARK_OUTER_DOT1Pf)) ? 0 : 1;
              }
          }
#endif /* BCM_TRX_SUPPORT */
          break;
      case bcmPortControlEgressModifyDscp:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRX_SUPPORT) 
          if (SOC_IS_TRX(unit)) {
              uint32 val;
              rv = READ_EGR_VLAN_CONTROL_1r(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = (soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r, val,
                                              REMARK_OUTER_DSCPf)) ? 1 : 0;
              }
          }
#endif /* BCM_TRX_SUPPORT */
          break;
    case bcmPortControlIpfixRate:
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_ipfix_rate)) {
            rv = _bcm_esw_port_tab_get(unit, port, IPFIX_FLOW_METER_IDf,
                                       value);
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        break;
    case bcmPortControlCustomerQueuing:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_vlan_queue_map)) {
            uint32 rval;
            rv = READ_ING_COS_MODEr(unit, port, &rval);
            if (soc_reg_field_get(unit, ING_COS_MODEr, rval, SELECTf) == 3) {  
                *value = 1;
            } else {
                *value = 0;
            }
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        break;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT)
      case bcmPortControlOAMEnable:
          if (BCM_GPORT_IS_WLAN_PORT(port)) {
              return BCM_E_UNAVAIL;
          }
          rv = _bcm_esw_iport_tab_get(unit, port, OAM_ENABLEf, value);
          break;
#endif /* defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) */

#ifdef BCM_TRX_SUPPORT
      case bcmPortControlOamLoopback:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
          if (SOC_IS_TRX(unit))
          {
              uint32 reg_value;

              rv = READ_ING_MISC_PORT_CONFIGr(unit, port, &reg_value);

              if (SOC_SUCCESS(rv))
              {
                  *value = soc_reg_field_get(unit, ING_MISC_PORT_CONFIGr,
                      reg_value, OAM_DO_NOT_MODIFYf);
              }
          }

          break;
#endif /* BCM_TRX_SUPPORT */

    case bcmPortControlLanes:
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
        if (soc_feature(unit, soc_feature_flex_port)) {
            rv = _bcm_tr2_port_lanes_get(unit, port, value);
        }
#endif
        break;
      case bcmPortControlPFCEthertype:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 val;
              rv = READ_MAC_PFC_TYPEr(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = soc_reg_field_get(unit, MAC_PFC_TYPEr, val,
                                             PFC_ETH_TYPEf);
              }
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;
      case bcmPortControlPFCOpcode:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 val;
              rv = READ_MAC_PFC_OPCODEr(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = soc_reg_field_get(unit, MAC_PFC_OPCODEr, val,
                                             PFC_OPCODEf);
              }
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;
      case bcmPortControlPFCReceive:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 val;
              rv = READ_UNIMAC_PFC_CTRLr(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, val,
                                             PFC_RX_ENBLf);
              }
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;
      case bcmPortControlPFCTransmit:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 val;
              rv = READ_UNIMAC_PFC_CTRLr(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, val,
                                             PFC_TX_ENBLf);
              }
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;
      case bcmPortControlPFCClasses:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 val;
              rv = READ_UNIMAC_PFC_CTRLr(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = (soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, val,
                                              PFC_EIGHT_CLASSf)) ? 8 : 16;
              }
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;
      case bcmPortControlPFCPassFrames:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 val;
              rv = READ_UNIMAC_PFC_CTRLr(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, val,
                                             RX_PASS_PFC_FRMf);
              }
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;
      case bcmPortControlPFCDestMacNonOui:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 val;
              rv = READ_MAC_PFC_DA_0r(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = soc_reg_field_get(unit, MAC_PFC_DA_0r, val,
                                             PFC_MACDA_0f) & 0x00ffffff;
              }
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;
      case bcmPortControlPFCDestMacOui:
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_SCORPION_SUPPORT) 
          if (soc_feature(unit, soc_feature_priority_flow_control) &&
              IS_GX_PORT(unit, port)) {
              uint32 val;
              rv = READ_MAC_PFC_DA_0r(unit, port, &val);
              if (BCM_SUCCESS(rv)) {
                  *value = (soc_reg_field_get(unit, MAC_PFC_DA_0r, val,
                                             PFC_MACDA_0f) >> 24) & 0xff;
                  rv = READ_MAC_PFC_DA_1r(unit, port, &val);
                  if (BCM_SUCCESS(rv)) {
                      *value |= soc_reg_field_get(unit, MAC_PFC_DA_1r, val,
                                                  PFC_MACDA_1f)<< 8;
                  }
              }
          }
#endif /* BCM_SCORPION_SUPPORT */
          break;

      case bcmPortControlVrf: 		
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIUMPH_SUPPORT)
          {
              source_trunk_map_table_entry_t stm_entry;

              if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                  rv = _bcm_esw_port_tab_get(unit, port, PORT_OPERATIONf, value);
              }	  

              if ((*value == 0x3) && 
                  (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, VRF_IDf)) ) {
                  SOC_IF_ERROR_RETURN
                      (READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, port, &stm_entry));
                  *value = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                               &stm_entry, VRF_IDf);
              } else {
                  *value = 0;
              }
          }     
#endif  /* BCM_TRIUMPH_SUPPORT */
          break;

      case bcmPortControlL3Ingress: 	
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIUMPH_SUPPORT)
       {
	     source_trunk_map_table_entry_t stm_entry;

            if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                 rv = _bcm_esw_port_tab_get(unit, port, PORT_OPERATIONf, value);
            }	

            if ((*value == 0x2) && 
                 (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, L3_IIFf)) ) {
                     SOC_IF_ERROR_RETURN
                          (READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, port, &stm_entry));
                      *value = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                                       &stm_entry, L3_IIFf);
            } else {
                      *value = 0;
            }
       }	  
#endif  /* BCM_TRIUMPH_SUPPORT */
        break;

      case bcmPortControlL2Learn:
#if defined(BCM_TRX_SUPPORT)
          rv = _bcm_esw_port_tab_get(unit, port, CML_FLAGS_NEWf, value);
#endif /* BCM_TRX_SUPPORT */
          break;

      case bcmPortControlL2Move:
#if defined(BCM_TRX_SUPPORT)
          rv = _bcm_esw_port_tab_get(unit, port, CML_FLAGS_MOVEf, value);
#endif /* BCM_TRX_SUPPORT */
          break;

    case bcmPortControlForwardStaticL2MovePkt:
#if defined(BCM_TRX_SUPPORT)
         rv = _bcm_esw_port_tab_get(unit, port, DISABLE_STATIC_MOVE_DROPf, 
                                    value);
#endif /* BCM_TRX_SUPPORT */

      default:
          break;
    }

    SOC_DEBUG_PRINT((DK_PORT,
                   "bcm_port_control_get: u=%d p=%d v=%d\n",
                    unit, port, *value));
    return rv;
}

/* 
 * Function:
 *      bcm_port_vlan_inner_tag_set
 * Purpose:
 *      Set the inner tag to be added to the packet.
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      inner_tag  - (IN) Inner tag. 
 *                    Priority[15:13] CFI[12] VLAN ID [11:0]
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_port_vlan_inner_tag_set(int unit, bcm_port_t port, uint16 inner_tag)
{
    int rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = BCM_E_UNAVAIL;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }

        PORT_LOCK(unit);
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_default_action_get(unit, port, &action));
        action.new_inner_vlan = BCM_VLAN_CTRL_ID(inner_tag);
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_default_action_set(unit, port, &action));
        PORT_UNLOCK(unit); 
        return BCM_E_NONE;
    }
#endif

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        PORT_LOCK(unit);

        rv = soc_reg_field32_modify(unit, EGR_SRC_PORTr, port, 
                                    INNER_TAGf, inner_tag);
        PORT_UNLOCK(unit); 
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */

    return rv;
}

/* 
 * Function:
 *      bcm_port_vlan_inner_tag_get
 * Purpose:
 *      Get the inner tag to be added to the packet.
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      inner_tag  - (OUT) Inner tag. 
 *                    Priority[15:13] CFI[12] VLAN ID [11:0]
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_port_vlan_inner_tag_get(int unit, bcm_port_t port, uint16 *inner_tag)
{
    int rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    rv = BCM_E_UNAVAIL;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }

        PORT_LOCK(unit);
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_default_action_get(unit, port, &action));
        *inner_tag = action.new_inner_vlan;
        PORT_UNLOCK(unit);
        return BCM_E_NONE;
    }
#endif

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        uint32 value;

        rv = READ_EGR_SRC_PORTr(unit, port, &value);
        *inner_tag = soc_reg_field_get(unit, EGR_SRC_PORTr, value,
                                       INNER_TAGf);
    } 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */
    return rv;
}

/* 
 * Function    : 
 *     bcm_port_class_set
 * Description : 
 *     Set the ports class ID. Ports with the
 *     same class ID can be treated as a group in
 *     field processing and VLAN translation.
 * Parameters  : 
 *     (IN) unit      - BCM device number
 *     (IN) port      - Device or logical port number
 *     (IN) pclass    - Classification type 
 *     (IN) pclass_id - New class ID of the port.
 * Returns     : 
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX      - Failed
 */
int 
bcm_esw_port_class_set(int unit, bcm_port_t port, 
                       bcm_port_class_t pclass, uint32 pclass_id)
{
    int        rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        int        field_width;
        uint32     max_value;
        int        port_class_id;
        soc_reg_t egr_port_reg;
        egr_port_reg = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                        SOC_IS_VALKYRIE2(unit)) ? EGR_PORT_64r : EGR_PORTr;
#endif /* BCM_FIREBOLT_SUPPORT || BCM_TRX_SUPPORT */

        switch (pclass) {
        case bcmPortClassFieldLookup:
        case bcmPortClassFieldIngress:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
            if (soc_mem_field_valid(unit, PORT_TABm, VFP_PORT_GROUP_IDf)) {
                rv = soc_mem_field_pbmp_fit(unit, PORT_TABm, VFP_PORT_GROUP_IDf,
                                           &pclass_id);
                if (rv == BCM_E_NONE) {
                    port_class_id = (int) pclass_id;
                    rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                               VFP_PORT_GROUP_IDf, 
                                               port_class_id);
                }
            }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
            if ((SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, CLASS_IDf)) &&
                (BCM_SUCCESS(rv))) {
                rv = _bcm_trx_source_trunk_map_set(unit, port, CLASS_IDf, pclass_id);
            }
            if ((SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                     VFP_PORT_GROUP_IDf)) &&
                (BCM_SUCCESS(rv))) {
                rv = _bcm_trx_source_trunk_map_set(unit, port,
                                                   VFP_PORT_GROUP_IDf, 
                                                   pclass_id);
            }
#endif /* BCM_TRX_SUPPORT */
            break;
        case bcmPortClassFieldEgress:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
            if (soc_reg_field_valid(unit, egr_port_reg, EGR_PORT_GROUP_IDf)) {
                field_width = soc_reg_field_length(unit, egr_port_reg,
                                                   EGR_PORT_GROUP_IDf);
                max_value = (1 << (field_width + 1)) - 1;
                if (pclass_id > max_value) {
                    rv = BCM_E_PARAM;
                }
                rv = soc_reg_field32_modify(unit, egr_port_reg, port,
                                            EGR_PORT_GROUP_IDf, pclass_id);
            }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_TRX_SUPPORT */
            break;
        case bcmPortClassVlanTranslateEgress:
#if defined(BCM_TRX_SUPPORT)
            if (soc_reg_field_valid(unit, egr_port_reg, VT_PORT_GROUP_IDf)) {
                field_width = soc_reg_field_length(unit, egr_port_reg,
                                                   VT_PORT_GROUP_IDf);
                max_value = (1 << (field_width + 1)) - 1;
                if (pclass_id > max_value) {
                    rv = BCM_E_PARAM;
                }
                rv = soc_reg_field32_modify(unit, egr_port_reg, port,
                                            VT_PORT_GROUP_IDf, pclass_id);
            }
#endif /* BCM_TRX_SUPPORT */
            break;
        default:
            rv = BCM_E_PARAM;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return rv;
}
  
/* 
 * Function    : 
 *     bcm_port_class_get
 * Description : 
 *     Get the ports class ID. Ports with the
 *     same class ID can be treated as a group in
 *     field processing and VLAN translation.
 * Parameters  : 
 *     (IN) unit       - BCM device number
 *     (IN) port       - Device or logical port number
 *     (IN) pclass     - Classification type 
 *     (OUT) pclass_id - New class ID of the port.
 * Returns     : 
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX       - Failed
 */
int 
bcm_esw_port_class_get(int unit, bcm_port_t port, 
                       bcm_port_class_t pclass, uint32 *pclass_id)
{
    int        rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (NULL == pclass_id) {
        return BCM_E_PARAM;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        uint32 reg_val;
        uint64 reg_val64;
        int    port_class_id;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        switch (pclass) {
        case bcmPortClassFieldLookup:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
            if (soc_mem_field_valid(unit, PORT_TABm, VFP_PORT_GROUP_IDf)) {
                rv = _bcm_esw_port_tab_get(unit, port, VFP_PORT_GROUP_IDf, 
                                           &port_class_id);
                *pclass_id = (uint32) port_class_id; 
            }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_TRX_SUPPORT */
            break;
        case bcmPortClassFieldIngress:
#if defined(BCM_TRX_SUPPORT)
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, CLASS_IDf)) {
                rv = _bcm_trx_source_trunk_map_get(unit, port, CLASS_IDf, pclass_id);
            }
#endif /* BCM_TRX_SUPPORT */
            break;
        case bcmPortClassFieldEgress:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
            if (soc_reg_field_valid(unit, EGR_PORTr, EGR_PORT_GROUP_IDf)) {
                if (SOC_REG_IS_VALID(unit, EGR_PORT_64r)) {
                    rv = READ_EGR_PORT_64r(unit, port, &reg_val64); 
                    if (BCM_SUCCESS(rv)) {
                        *pclass_id = soc_reg64_field32_get(unit, EGR_PORT_64r, 
                                                       reg_val64, EGR_PORT_GROUP_IDf);
                    }
                } else {
                    rv = READ_EGR_PORTr(unit, port, &reg_val); 
                    if (BCM_SUCCESS(rv)) {
                        *pclass_id = soc_reg_field_get(unit, EGR_PORTr, 
                                                       reg_val, EGR_PORT_GROUP_IDf);
                    }
                }
            }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_TRX_SUPPORT */
            break;
        case bcmPortClassVlanTranslateEgress:
#if defined(BCM_TRX_SUPPORT)
            if (soc_reg_field_valid(unit, EGR_PORTr, VT_PORT_GROUP_IDf)) {
                if (SOC_REG_IS_VALID(unit, EGR_PORT_64r)) {
                    rv = READ_EGR_PORT_64r(unit, port, &reg_val64);
                    if (BCM_SUCCESS(rv)) {
                        *pclass_id = soc_reg64_field32_get(unit, EGR_PORT_64r,
                                                       reg_val64, VT_PORT_GROUP_IDf);
                    }
                } else {
                    rv = READ_EGR_PORTr(unit, port, &reg_val);
                    if (BCM_SUCCESS(rv)) {
                        *pclass_id = soc_reg_field_get(unit, EGR_PORTr,
                                                       reg_val, VT_PORT_GROUP_IDf);
                    }
                }
            }
#endif /* BCM_TRX_SUPPORT */
            break;
        default:
            rv = BCM_E_PARAM;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return rv;
}


/*
 * Function    : _bcm_esw_port_config_set
 * Description : Internal function to set port configuration.
 * Parameters  : (IN)unit  - BCM device number.
 *               (IN)port  - Port number.
 *               (IN)type  - Port property.   
 *               (IN)value - New property value.
 * Returns     : BCM_E_XXX
 */
int 
_bcm_esw_port_config_set(int unit, bcm_port_t port, 
                         _bcm_port_config_t type, int value)
{
    int rv = BCM_E_UNAVAIL;    /* Operation return status. */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    switch (type) { 
    case _bcmPortL3UrpfMode:       
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   URPF_MODEf, value);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortL3UrpfDefaultRoute:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   URPF_DEFAULTROUTECHECKf, 
                                   (value) ? 0 : 1);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortVlanTranslate:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   VT_ENABLEf, value);
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit) && (rv == SOC_E_NONE)) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       VT_KEY_TYPE_USE_GLPf, value);
            if (rv == SOC_E_NONE) {
                rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                           VT_KEY_TYPE_2_USE_GLPf, value);
            }
        }
#endif
        break; 
    case _bcmPortVlanPrecedence:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   VLAN_PRECEDENCEf, value);
        break;
    case _bcmPortVTMissDrop:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   VT_MISS_DROPf, value);
        break;
    case _bcmPortLookupMACEnable:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   MAC_BASED_VID_ENABLEf, value);
        break;
    case _bcmPortLookupIPEnable:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   SUBNET_BASED_VID_ENABLEf,
                                   value);
        break;
    case _bcmPortUseInnerPri:
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   USE_INNER_PRIf, value);
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortUseOuterPri:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   TRUST_OUTER_DOT1Pf, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortVerifyOuterTpid:
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   OUTER_TPID_VERIFYf, value);
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break; 
    case _bcmPortVTKeyTypeFirst:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   VT_KEY_TYPEf, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortVTKeyPortFirst:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   VT_KEY_TYPE_USE_GLPf, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortVTKeyTypeSecond:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   VT_KEY_TYPE_2f, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortVTKeyPortSecond:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   VT_KEY_TYPE_2_USE_GLPf, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortIpmcEnable:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   IPMC_ENABLEf, value);
        break;
    case _bcmPortIpmcV4Enable:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   V4IPMC_ENABLEf, value);
        break;
    case _bcmPortIpmcV6Enable:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   V6IPMC_ENABLEf, value);
        break;
    case _bcmPortIpmcVlanKey:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   IPMC_DO_VLANf, value);
        break;
    case _bcmPortCfiAsCng:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   CFI_AS_CNGf, value);
        break;
    case _bcmPortNni:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   NNI_PORTf, value);
        break;
    case _bcmPortHigigTrunkId:
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
        if (value < 0) {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       HIGIG_TRUNKf, 0);
            if (BCM_SUCCESS(rv)) {
                rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                           HIGIG_TRUNK_IDf, 0);
            }
        } else {
            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                       HIGIG_TRUNK_IDf, value);
            if (BCM_SUCCESS(rv)) {
                rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                           HIGIG_TRUNKf, 1);
            }
        }
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortModuleLoopback:
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   ALLOW_SRC_MODf, value);
        if (BCM_SUCCESS(rv) && SOC_MEM_IS_VALID(unit, LPORT_TABm)) {
            rv = _bcm_esw_lport_tab_set(unit, port, ALLOW_SRC_MODf, value);
        }
        break;
    case _bcmPortOuterTpidEnables:
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                   OUTER_TPID_ENABLEf, value);
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    default:
        rv = BCM_E_INTERNAL;
    }

    PORT_UNLOCK(unit);
    return (rv);
}

/*
 * Function    : _bcm_esw_port_config_get
 * Description : Internal function to get port configuration.
 * Parameters  : (IN)unit  - BCM device number.
 *               (IN)port  - Port number.
 *               (IN)type  - Port property.   
 *               (OUT)value -Port property value.
 * Returns     : BCM_E_XXX
 */
int 
_bcm_esw_port_config_get(int unit, bcm_port_t port, 
                     _bcm_port_config_t type, int *value)
{
    int rv = BCM_E_UNAVAIL;    /* Operation return status. */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    int tmp_value; /* Temporary value.     */
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);

    switch (type) { 
    case _bcmPortL3UrpfMode:       
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, URPF_MODEf, value);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortL3UrpfDefaultRoute:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, URPF_DEFAULTROUTECHECKf,
                                   &tmp_value);
        *value = (tmp_value) ? FALSE : TRUE; 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortVlanTranslate:
        rv = _bcm_esw_port_tab_get(unit, port, VT_ENABLEf, value);
        break;
    case _bcmPortVlanPrecedence:
        rv = _bcm_esw_port_tab_get(unit, port, VLAN_PRECEDENCEf, value);
        break; 
    case _bcmPortVTMissDrop:
        rv = _bcm_esw_port_tab_get(unit, port, VT_MISS_DROPf, value);
        break;
    case _bcmPortLookupMACEnable:
        rv = _bcm_esw_port_tab_get(unit, port, MAC_BASED_VID_ENABLEf, value);
        break;
    case _bcmPortLookupIPEnable:
        rv = _bcm_esw_port_tab_get(unit, port, SUBNET_BASED_VID_ENABLEf,
                                   value);
        break;
    case _bcmPortUseInnerPri:
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, USE_INNER_PRIf, value);
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortUseOuterPri:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, TRUST_OUTER_DOT1Pf, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortVerifyOuterTpid:
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, OUTER_TPID_VERIFYf, value);
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortVTKeyTypeFirst:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, VT_KEY_TYPEf, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortVTKeyPortFirst:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, VT_KEY_TYPE_USE_GLPf, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortVTKeyTypeSecond:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, VT_KEY_TYPE_2f, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortVTKeyPortSecond:
#if defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, VT_KEY_TYPE_2_USE_GLPf, value);
#endif /* BCM_TRX_SUPPORT */
        break;
    case _bcmPortIpmcEnable:
        rv = _bcm_esw_port_tab_get(unit, port, IPMC_ENABLEf, value);
        break;
    case _bcmPortIpmcV4Enable:
        rv = _bcm_esw_port_tab_get(unit, port, V4IPMC_ENABLEf, value);
        break;
    case _bcmPortIpmcV6Enable:
        rv = _bcm_esw_port_tab_get(unit, port, V6IPMC_ENABLEf, value);
        break;
    case _bcmPortIpmcVlanKey:
        rv = _bcm_esw_port_tab_get(unit, port, IPMC_DO_VLANf, value);
        break;
    case _bcmPortCfiAsCng:
        rv = _bcm_esw_port_tab_get(unit, port, CFI_AS_CNGf, value);
        break;
    case _bcmPortNni:
        rv = _bcm_esw_port_tab_get(unit, port, NNI_PORTf, value);
        break;
    case _bcmPortHigigTrunkId:
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, HIGIG_TRUNKf, value);
        if (BCM_SUCCESS(rv)) {
            if (*value == 0) {
                *value = -1;
            } else {
                rv = _bcm_esw_port_tab_get(unit, port,
                                           HIGIG_TRUNK_IDf, value);
            }
        }
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */
        break;
    case _bcmPortModuleLoopback:
        rv = _bcm_esw_port_tab_get(unit, port, ALLOW_SRC_MODf, value);
        break;
    case _bcmPortOuterTpidEnables:
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        rv = _bcm_esw_port_tab_get(unit, port, OUTER_TPID_ENABLEf, value);
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        break;
    default:
        rv = BCM_E_INTERNAL;
    }
    PORT_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *     bcm_port_force_vlan_set
 * Description:
 *     To set the force vlan attribute of a port
 * Parameters:
 *     unit        device number
 *     port        port number
 *     vlan        vlan identifier
 *                 (0 - 4095) - use this VLAN id if egress packet is tagged
 *     pkt_prio    egress packet priority (-1, 0..7)
 *                 any negative priority value disable the priority
 *                 override if the egress packet is tagged
 *     flags       bit fields
 *                 BCM_PORT_FORCE_VLAN_ENABLE - enable force vlan on this
 *                                              port
 *                 BCM_PORT_FORCE_VLAN_UNTAG - egress untagged when force
 *                                             vlan is enabled on this port
 *
 * Return:
 *     BCM_E_NONE
 *     BCM_E_UNIT
 *     BCM_E_PORT
 *     BCM_E_PARAM
 *     BCM_E_UNAVAIL
 *     BCM_E_XXX
 */
int
bcm_esw_port_force_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vlan,
                            int pkt_prio, uint32 flags)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRX_SUPPORT)
    if (SOC_REG_FIELD_VALID(unit, EGR_PVLAN_EPORT_CONTROLr, PVLAN_UNTAGf)) {
        rv = _bcm_trx_port_force_vlan_set(unit, port, vlan, pkt_prio, flags);
    }
#endif /* defined(BCM_TRX_SUPPORT) */

    return rv;
}

/*
 * Function:
 *     bcm_port_force_vlan_get
 * Description:
 *     To get the force vlan attribute of a port
 * Parameters:
 *     unit        device number
 N_UNTAGf*     port        port number
 *     vlan        pointer to vlan identifier
 *                 (0 - 4095) - use this VLAN id if egress packet is tagged
 *                 valid only when BCM_PORT_FORCE_VLAN_ENABLE is set and
 *                 BCM_PORT_FORCE_VLAN_UNTAG is clear
 *     pkt_prio    egress packet priority (-1, 0 - 7)
 *                 valid only when BCM_PORT_FORCE_VLAN_ENABLE is set and
 *                 BCM_PORT_FORCE_VLAN_UNTAG is clear
 *     flags       bit fields
 *                 BCM_PORT_FORCE_VLAN_ENABLE - enable force vlan on this
 *                                              port
 *                 BCM_PORT_FORCE_VLAN_UNTAG - egress untagged when force
 *                                             vlan is enabled on this port
 *
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_port_force_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vlan,
                            int *pkt_prio, uint32 *flags)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRX_SUPPORT)
    if (SOC_REG_FIELD_VALID(unit, EGR_PVLAN_EPORT_CONTROLr, PVLAN_UNTAGf)) {
        rv = _bcm_trx_port_force_vlan_get(unit, port, vlan, pkt_prio, flags);
    }
#endif /* defined(BCM_TRX_SUPPORT) */

    return rv;
}

/*
 * Function:
 *     bcm_port_phy_control_set
 * Description:
 *     Set PHY specific properties 
 * Parameters:
 *     unit        device number
 *     port        port number
 *     type        configuration type
 *     value       new value for the configuration
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_port_phy_control_set(int unit, bcm_port_t port, 
                             bcm_port_phy_control_t type, uint32 value)
{
    int rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    PORT_LOCK(unit);
    rv = soc_phyctrl_control_set(unit, port, type, value);
    PORT_UNLOCK(unit);

    return rv; 
}

/*
 * Function:
 *     bcm_port_phy_control_get
 * Description:
 *     Set PHY specific properties 
 * Parameters:
 *     unit        device number
 *     port        port number
 *     type        configuration type
 *     value       value for the configuration
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_port_phy_control_get(int unit, bcm_port_t port,
                             bcm_port_phy_control_t type, uint32 *value)
{
    int rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (NULL == value) {
        return BCM_E_PARAM;
    }

    PORT_LOCK(unit);
    rv = soc_phyctrl_control_get(unit, port, type, value); 
    PORT_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_port_phy_firmware_set
 * Purpose:
 *      Write the firmware to the PHY device's non-volatile storage.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port number
 *      flags - (IN) PHY spcific flags, such as BCM_PORT_PHY_INTERNAL 
 *      offset - (IN) Offset to the firmware data array 
 *      array - (IN)  The firmware data array 
 *      length - (IN) The length of the firmware data array 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_port_phy_firmware_set(int unit, bcm_port_t port, uint32 flags, 
                              int offset, uint8 *array, int length)
{
    int rv;
                                                                                
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
                                                                                
    PORT_LOCK(unit);
    rv = soc_phyctrl_firmware_set(unit, port, flags,offset,array,length);
    PORT_UNLOCK(unit);
                                                                                
    return rv;
}

/*
 * Function    : bcm_esw_port_local_get
 * Description : Get the local port from the given GPORT ID.
 *
 * Parameters  : (IN)  unit         - BCM device number
 *               (IN)  gport        - global port identifier
 *               (OUT) local_port   - local port encoded in gport
 * Returns     : BCM_E_XXX
 */
int
bcm_esw_port_local_get(int unit, bcm_gport_t gport, bcm_port_t *local_port)
{
    bcm_module_t my_mod, mod_out;
    bcm_port_t port_out;
    int num_modid;

    BCM_IF_ERROR_RETURN(bcm_esw_stk_modid_count(unit, &num_modid));
    if (!num_modid) {
        /* No modid for this device */
        return BCM_E_UNAVAIL;
    }

    if (SOC_GPORT_IS_LOCAL(gport)) {
         *local_port = SOC_GPORT_LOCAL_GET(gport);
    } else if (SOC_GPORT_IS_LOCAL_CPU(gport)) {
        if (CMIC_PORT(unit) != SOC_GPORT_LOCAL_CPU_GET(gport)) {
            return BCM_E_PORT;
        }
        *local_port = CMIC_PORT(unit);
    } else if (SOC_GPORT_IS_DEVPORT(gport)) {
        *local_port = SOC_GPORT_DEVPORT_PORT_GET(gport);
        if (unit != SOC_GPORT_DEVPORT_DEVID_GET(gport)) {
            return BCM_E_PORT;
        }
    } else if (SOC_GPORT_IS_MODPORT(gport)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &my_mod));
        mod_out = SOC_GPORT_MODPORT_MODID_GET(gport);
        port_out = SOC_GPORT_MODPORT_PORT_GET(gport);
        if (mod_out == my_mod){ 
            *local_port = port_out;
        } else if (num_modid == 2) {
            if (mod_out == my_mod + 1) {
                *local_port = port_out + 32;
            } else {
                return BCM_E_PORT;
            }
        } else {
            return BCM_E_PORT;
        }
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, local_port);
        }
        if (!SOC_PORT_VALID(unit, *local_port)) {
            return BCM_E_PORT;
        }
    } else {
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*
 * Function    : _bcm_esw_modid_is_local
 * Description : Identifies if given modid is local on a given unit
 *
 * Parameters  : (IN)   unit      - BCM device number
 *               (IN)   modnd     - Module ID 
 *               (OUT)  result    - TRUE if modid is local, FALSE otherwise
 * Returns     : BCM_E_XXX
 */
int 
_bcm_esw_modid_is_local(int unit, bcm_module_t modid, int *result)
{
    bcm_module_t    mymodid;    
    int             rv;

    /* Input parameters check. */
    if (NULL == result) {
        return (BCM_E_PARAM);
    }

    /* Get local module id. */
    rv = bcm_esw_stk_my_modid_get(unit, &mymodid);
    if (BCM_E_UNAVAIL == rv) {
        if (0 == modid) {
            *result = TRUE;
        } else {
            *result = FALSE;
        }
        return (BCM_E_NONE);
    }

    if (mymodid == modid) {
        *result = TRUE;
    } else if (NUM_MODID(unit) == 2) {
        if (modid == (mymodid + 1)) {
            *result = TRUE;
        } else {
            *result = FALSE;
        }
    } else {
        *result = FALSE;
    }

    return (BCM_E_NONE);
}


/*
 * Function    : _bcm_gport_modport_hw2api_map
 * Description : Remaps module and port from HW space to API space 
 *
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  mod_in    - Module ID to map   
 *               (IN)  port_in   - Physical port to map   
 *               (OUT)  mod_out  - Module ID after mapping
 *               (OUT)  port_out - Port number after mapping 
 * Returns     : BCM_E_XXX
 * Notes       : If mod_out == NULL then port_out will be local physical port.
 */
int 
_bcm_gport_modport_hw2api_map(int unit, bcm_module_t mod_in, bcm_port_t port_in,
                              bcm_module_t *mod_out, bcm_port_t *port_out)
{
    if (port_out == NULL) {
        return (BCM_E_PARAM);
    }

    if (NUM_MODID(unit) == 1) { /* HW device has single modid */
        if (mod_out != NULL) {
            *mod_out = mod_in;
        }
        *port_out = port_in;

        return (BCM_E_NONE);
    }
    /* Here only for devices with dual modid NUM_MODID(unit) == 2 */
    if (mod_out == NULL) {  /* physical port requested */
        int             isLocal;
        bcm_module_t    mymodid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_in, &isLocal));
        if (isLocal != TRUE) {
            return (BCM_E_PARAM);
        }
        BCM_IF_ERROR_RETURN (bcm_esw_stk_my_modid_get(unit, &mymodid));
        if (mod_in == mymodid) {
            *port_out = port_in;
        } else {
            *port_out = port_in + 32;
        }
    } else {    /* NUM_MODID(unit) == 2 and not local physical port */
        if (port_in > 31) {
            mod_in +=1;
            port_in -=32;
        }

        *port_out = port_in;
        *mod_out = mod_in;
    }

    return (BCM_E_NONE);
}

/*
 * Function    : _bcm_gport_modport_api2hw_map
 * Description : Remaps module and port gport from API space to HW space.
 *
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  mod_in    - Module ID to map   
 *               (IN)  port_in   - Physical port to map   
 *               (OUT)  mod_out  - Module ID after mapping
 *               (OUT)  port_out - Port number after mapping 
 * Returns     : BCM_E_XXX
 */
int 
_bcm_gport_modport_api2hw_map(int unit, bcm_module_t mod_in, bcm_port_t port_in,
                       bcm_module_t *mod_out, bcm_port_t *port_out)
{
    if (port_out == NULL || mod_out == NULL) {
        return (BCM_E_PARAM);
    }

    if (NUM_MODID(unit) == 2 && port_in > 31) {
        mod_in += 1;
        port_in -= 32;
    }

    *mod_out = mod_in;
    *port_out = port_in;

    return (BCM_E_NONE);
}

/*
 * Function    : bcm_port_gport_get
 * Description : Get the GPORT ID for the specified physical port.
 *
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  port      - Port number
 *               (OUT) gport     - GPORT ID
 * Returns     : BCM_E_XXX
 * Notes:
 *     Always returns a MODPORT gport or an error.
 */
int
bcm_esw_port_gport_get(int unit, bcm_port_t port, bcm_gport_t *gport)
{
    int                 rv;
    _bcm_gport_dest_t   dest;

    _bcm_gport_dest_t_init(&dest);

    PORT_PARAM_CHECK(unit, port);

    rv = bcm_esw_stk_my_modid_get(unit, &dest.modid);
    
    if (BCM_FAILURE(rv)) {
        return BCM_E_UNAVAIL;
    }

    if (soc_feature(unit, soc_feature_sysport_remap)) {
        BCM_XLATE_SYSPORT_P2S(unit, &port);
    }

    dest.port = port;
    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;

    BCM_IF_ERROR_RETURN
        (_bcm_gport_modport_api2hw_map(unit, dest.modid, dest.port, 
                                       &(dest.modid), &(dest.port)));

    return _bcm_esw_gport_construct(unit, &dest, gport); 
}

#if defined(BCM_TRIUMPH2_SUPPORT)
int
_bcm_esw_port_flex_stat_index_set(int unit, bcm_gport_t port, int fs_idx)
{
    bcm_port_t loc_port;
    int rv;

    rv = bcm_esw_port_local_get(unit, port, &loc_port);
    if (BCM_FAILURE(rv)) {
        return BCM_E_NOT_FOUND;  /* Local port disabled */
    }

    PORT_LOCK(unit); /* Keep port tables in sync */
    rv = soc_mem_field32_modify(unit, PORT_TABm, loc_port, VINTF_CTR_IDXf, 
                                fs_idx);
    if (BCM_SUCCESS(rv)) {
        rv = soc_reg_field32_modify(unit, EGR_PORT_64r, loc_port,
                                    VINTF_CTR_IDXf, fs_idx);
    }
    PORT_UNLOCK(unit);
    return rv;
}

STATIC int
_bcm_esw_port_stat_param_valid(int unit, bcm_gport_t port)
{
    int vp;
    PORT_INIT(unit);

    if (!soc_feature(unit, soc_feature_gport_service_counters)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_MPLS_PORT(port)) {
#if defined(BCM_MPLS_SUPPORT) && defined(INCLUDE_L3)
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            return BCM_E_PARAM;
        }
#endif
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(port) ||
               BCM_GPORT_IS_SUBPORT_PORT(port)) {
#if defined(INCLUDE_L3)
        if (_bcm_tr_subport_gport_used(unit, port) == BCM_E_NOT_FOUND) {
            return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_MIM_PORT(port)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
            return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_WLAN_PORT(port)) {
        vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_PARAM;
        }
#endif
    } else {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &vp));
    }

    return BCM_E_NONE;
}

STATIC _bcm_flex_stat_t
_bcm_esw_port_stat_to_flex_stat(bcm_port_stat_t stat)
{
    _bcm_flex_stat_t flex_stat;

    switch (stat) {
    case bcmPortStatIngressPackets:
        flex_stat = _bcmFlexStatIngressPackets;
        break;
    case bcmPortStatIngressBytes:
        flex_stat = _bcmFlexStatIngressBytes;
        break;
    case bcmPortStatEgressPackets:
        flex_stat = _bcmFlexStatEgressPackets;
        break;
    case bcmPortStatEgressBytes:
        flex_stat = _bcmFlexStatEgressBytes;
        break;
    default:
        flex_stat = _bcmFlexStatNum;
    }

    return flex_stat;
}

/* Requires "idx" variable */
#define BCM_PORT_VALUE_ARRAY_VALID(unit, nstat, value_arr) \
    for (idx = 0; idx < nstat; idx++) { \
        if (NULL == value_arr + idx) { \
            return (BCM_E_PARAM); \
        } \
    }

STATIC int
_bcm_port_stat_array_convert(int unit, int nstat, bcm_port_stat_t *stat_arr, 
                             _bcm_flex_stat_t *fs_arr)
{
    int idx;

    if ((nstat <= 0) || (nstat > _bcmFlexStatNum)) {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < nstat; idx++) {
        if (NULL == stat_arr + idx) {
            return (BCM_E_PARAM);
        }
        fs_arr[idx] = _bcm_esw_port_stat_to_flex_stat(stat_arr[idx]);
    }
    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      bcm_esw_port_stat_enable_set
 * Purpose:
 *      Enable/disable packet and byte counters for the selected
 *      gport.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_enable_set(int unit, bcm_gport_t port, int enable)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    return _bcm_esw_flex_stat_enable_set(unit, _bcmFlexStatTypeGport,
                             _bcm_esw_port_flex_stat_hw_index_set,
                                         NULL, port, enable);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_port_stat_get
 * Purpose:
 *      Get 64-bit counter value for specified port statistic type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_get(int unit, bcm_gport_t port, bcm_port_stat_t stat, 
                      uint64 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    return _bcm_esw_flex_stat_get(unit, _bcmFlexStatTypeGport, port,
                           _bcm_esw_port_stat_to_flex_stat(stat), val);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_port_stat_get32
 * Purpose:
 *      Get lower 32-bit counter value for specified port statistic
 *      type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_get32(int unit, bcm_gport_t port, 
                        bcm_port_stat_t stat, uint32 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    return _bcm_esw_flex_stat_get32(unit, _bcmFlexStatTypeGport, port,
                           _bcm_esw_port_stat_to_flex_stat(stat), val);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_port_stat_set
 * Purpose:
 *      Set 64-bit counter value for specified port statistic type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_set(int unit, bcm_gport_t port, bcm_port_stat_t stat, 
                      uint64 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    return _bcm_esw_flex_stat_set(unit, _bcmFlexStatTypeGport, port,
                           _bcm_esw_port_stat_to_flex_stat(stat), val);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_port_stat_set32
 * Purpose:
 *      Set lower 32-bit counter value for specified port statistic
 *      type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_set32(int unit, bcm_gport_t port, 
                        bcm_port_stat_t stat, uint32 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    return _bcm_esw_flex_stat_set32(unit, _bcmFlexStatTypeGport, port,
                           _bcm_esw_port_stat_to_flex_stat(stat), val);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_port_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple port statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_multi_get(int unit, bcm_gport_t port, int nstat, 
                            bcm_port_stat_t *stat_arr, 
                            uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    BCM_IF_ERROR_RETURN
        (_bcm_port_stat_array_convert(unit, nstat, stat_arr, fs_arr));
    BCM_PORT_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_get(unit, _bcmFlexStatTypeGport, port,
                                        nstat, fs_arr, value_arr);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_port_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple port statistic
 *      types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_multi_get32(int unit, bcm_gport_t port, int nstat, 
                              bcm_port_stat_t *stat_arr, 
                              uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    BCM_IF_ERROR_RETURN
        (_bcm_port_stat_array_convert(unit, nstat, stat_arr, fs_arr));
    BCM_PORT_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_get32(unit, _bcmFlexStatTypeGport, port,
                                          nstat, fs_arr, value_arr);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_port_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple port statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_multi_set(int unit, bcm_gport_t port, int nstat, 
                            bcm_port_stat_t *stat_arr, 
                            uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    BCM_IF_ERROR_RETURN
        (_bcm_port_stat_array_convert(unit, nstat, stat_arr, fs_arr));
    BCM_PORT_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_set(unit, _bcmFlexStatTypeGport, port,
                                          nstat, fs_arr, value_arr);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_port_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple port statistic
 *      types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_stat_multi_set32(int unit, bcm_gport_t port, int nstat, 
                              bcm_port_stat_t *stat_arr, 
                              uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_stat_param_valid(unit, port));
    BCM_IF_ERROR_RETURN
        (_bcm_port_stat_array_convert(unit, nstat, stat_arr, fs_arr));
    BCM_PORT_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_set32(unit, _bcmFlexStatTypeGport, port,
                                          nstat, fs_arr, value_arr);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function    : _bcm_gport_dest_t_init
 * Description : Initialize gport_dest structure
 * Parameters  : (IN/OUT)  gport_dest - Structure to initialize
 * Returns     : None
 */

void
_bcm_gport_dest_t_init(_bcm_gport_dest_t *gport_dest)
{
    sal_memset(gport_dest, 0, sizeof (_bcm_gport_dest_t));
}


/*
 * Function    : _bcm_esw_gport_construct
 * Description : Internal function to construct a gport from 
 *                given parameters 
 * Parameters  : (IN)  unit       - BCM device number
 *               (IN)  gport_dest - Structure that contains destination
 *                                   to encode into a gport
 *               (OUT) gport      - Global port identifier
 * Returns     : BCM_E_XXX
 * Notes       : The modid and port are translated from the
 *               local modid/port space to application space
 */
int 
_bcm_esw_gport_construct(int unit, _bcm_gport_dest_t *gport_dest, bcm_gport_t *gport)
{

    bcm_gport_t     l_gport = 0;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    
    if ((NULL == gport_dest) || (NULL == gport) ){
        return BCM_E_PARAM;
    }

    switch (gport_dest->gport_type) {
        case _SHR_GPORT_TYPE_TRUNK:
            SOC_GPORT_TRUNK_SET(l_gport, gport_dest->tgid);
            break;
        case _SHR_GPORT_TYPE_LOCAL_CPU:
            SOC_GPORT_LOCAL_CPU_SET(l_gport, CMIC_PORT(unit));
            break;
        case _SHR_GPORT_TYPE_LOCAL:
            SOC_GPORT_LOCAL_SET(l_gport, gport_dest->port);
            break;
        case _SHR_GPORT_TYPE_SUBPORT_GROUP:
            SOC_GPORT_SUBPORT_GROUP_SET(l_gport, gport_dest->subport_id);
            break;
        case _SHR_GPORT_TYPE_MPLS_PORT:
            BCM_GPORT_MPLS_PORT_ID_SET(l_gport, gport_dest->mpls_id);
            break;
        case _SHR_GPORT_TYPE_MIM_PORT:
            BCM_GPORT_MIM_PORT_ID_SET(l_gport, gport_dest->mim_id);
            break;
        case _SHR_GPORT_TYPE_WLAN_PORT:
            BCM_GPORT_WLAN_PORT_ID_SET(l_gport, gport_dest->wlan_id);
            break;
        case _SHR_GPORT_TYPE_DEVPORT: 
            BCM_GPORT_DEVPORT_SET(l_gport, unit, gport_dest->port);
            break;
        case _SHR_GPORT_TYPE_MODPORT:
            BCM_IF_ERROR_RETURN (
                _bcm_gport_modport_hw2api_map(unit, gport_dest->modid, 
                                              gport_dest->port, &mod_out,
                                              &port_out));
            SOC_GPORT_MODPORT_SET(l_gport, mod_out, port_out);
            break;
        default:    
            return BCM_E_PARAM;
    }

    *gport = l_gport;
    return BCM_E_NONE;
}

/*
 * Function    : _bcm_esw_gport_resolve
 * Description : Internal function to get modid, port, and trunk_id
 *               from a bcm_gport_t (global port)
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  gport     - Global port identifier
 *               (OUT) modid     - Module ID
 *               (OUT) port      - Port number
 *               (OUT) trunk_id  - Trunk ID
 *               (OUT) id        - HW ID
 * Returns     : BCM_E_XXX
 * Notes       : The modid and port are translated from the
 *               application space to local modid/port space if applicable, 
 *               on units without modid (Fabric) modid will be -1 and port 
 *               will be a local physical port.
 */
int 
_bcm_esw_gport_resolve(int unit, bcm_gport_t gport,
                       bcm_module_t *modid, bcm_port_t *port, 
                       bcm_trunk_t *trunk_id, int *id)
{
    int             local_id, rv = BCM_E_NONE;
    bcm_module_t    mod_in, local_modid;
    bcm_port_t      port_in, local_port;
    bcm_trunk_t     local_tgid;
    
    local_modid = -1;
    local_port = -1;
    local_id = -1;
    local_tgid = BCM_TRUNK_INVALID;

    if (SOC_GPORT_IS_TRUNK(gport)) {
        local_tgid = SOC_GPORT_TRUNK_GET(gport);
    } else if (SOC_GPORT_IS_LOCAL_CPU(gport)) {
        rv = bcm_esw_stk_my_modid_get(unit, &local_modid);
        if (BCM_FAILURE(rv) && !SOC_IS_XGS_FABRIC(unit)) {
            return rv;
        }
        local_port = CMIC_PORT(unit);
    } else if (SOC_GPORT_IS_LOCAL(gport)) {
        BCM_IF_ERROR_RETURN 
            (bcm_esw_stk_my_modid_get(unit, &local_modid));
        local_port = SOC_GPORT_LOCAL_GET(gport);
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_P2S(unit, &local_port);
        }
        if (!SOC_PORT_ADDRESSABLE(unit, local_port)) {
            return BCM_E_PORT;
        }
    } else if (SOC_GPORT_IS_DEVPORT(gport)) {
        rv = bcm_esw_stk_my_modid_get(unit, &local_modid);
        if (BCM_FAILURE(rv)) {
            if (!SOC_IS_XGS_FABRIC(unit)) {
                return rv;
            }
            if (unit != SOC_GPORT_DEVPORT_DEVID_GET(gport)) {
                return BCM_E_PORT;
            }
        }
        local_port = SOC_GPORT_DEVPORT_PORT_GET(gport);

        if (!SOC_PORT_ADDRESSABLE(unit, local_port)) {
            return BCM_E_PORT;
        }
    } else if (SOC_GPORT_IS_MODPORT(gport)) {
        mod_in = SOC_GPORT_MODPORT_MODID_GET(gport);
        port_in = SOC_GPORT_MODPORT_PORT_GET(gport);
        PORT_DUALMODID_VALID(unit, port_in);
        rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    mod_in, port_in, &local_modid, &local_port);

        if (!SOC_MODID_ADDRESSABLE(unit, local_modid)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, local_port)) {
            return BCM_E_PORT;
        }
    }
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    else if (SOC_GPORT_IS_MPLS_PORT(gport)) {
        if (SOC_IS_TR_VL(unit)) {
            rv = _bcm_tr_mpls_port_resolve(unit, gport, &local_modid,
                                           &local_port, &local_tgid, 
                                           &local_id);
        }
    }
#endif
#if defined (BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    else if (SOC_GPORT_IS_MIM_PORT(gport)) {
        if (soc_feature(unit, soc_feature_mim)) {
            rv = _bcm_tr2_mim_port_resolve(unit, gport, &local_modid,
                                          &local_port, &local_tgid, 
                                          &local_id);
        }
    } else if (SOC_GPORT_IS_WLAN_PORT(gport)) {
        if (soc_feature(unit, soc_feature_wlan)) {
            rv = _bcm_tr2_wlan_port_resolve(unit, gport, &local_modid,
                                           &local_port, &local_tgid, 
                                           &local_id);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined (BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    else if (SOC_GPORT_IS_SUBPORT_GROUP(gport)) {
        if (SOC_IS_TRX(unit)) {
            rv = _bcm_tr_subport_group_resolve(unit, gport, &local_modid,
                                               &local_port, &local_tgid, 
                                               &local_id);
        }
    } else if (SOC_GPORT_IS_SUBPORT_PORT(gport)) {
        if (SOC_IS_TRX(unit)) {
            rv = _bcm_tr_subport_port_resolve(unit, gport, &local_modid,
                                              &local_port, &local_tgid, 
                                              &local_id);
        }
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#ifdef BCM_TRIUMPH_SUPPORT
    else if (SOC_GPORT_IS_SCHEDULER(gport)) {
#if defined (BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            rv = _bcm_tr2_cosq_port_resolve(unit, gport, &local_modid,
                                            &local_port, &local_tgid, 
                                            &local_id);
        } else 
#endif /* BCM_TRIUMPH2_SUPPORT */
        if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_ENDURO_SUPPORT)
                if (SOC_IS_ENDURO(unit)) {
                    rv = _bcm_en_cosq_port_resolve(unit, gport, &local_modid,
                                                   &local_port, &local_tgid, 
                                                   &local_id);
                } else 
#endif /* BCM_ENDURO_SUPPORT */
                {
                    rv = _bcm_tr_cosq_port_resolve(unit, gport, &local_modid,
                                                   &local_port, &local_tgid, 
                                                   &local_id);
                }
        }
    }
#endif
    else if ((gport != BCM_GPORT_INVALID) && 
             (gport != BCM_GPORT_TYPE_BLACK_HOLE)) {
        return BCM_E_BADID;
    }
    *modid = local_modid;
    *port = local_port;
    *trunk_id = local_tgid;
    *id = local_id; 
    return (rv);
}

/*
 * Function:
 *      bcm_port_subsidiary_ports_get
 * Purpose:
 *      Given a controlling port, this API returns the set of ancillary ports
 *      belonging to the group (port block) that can be configured to operate
 *      either as a single high-speed port or multiple GE ports. If the input
 *      port is not a controlling port, an empty bitmap is returned.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port
 *      pbmp - (OUT) Ports associated with the hot-swap group
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_port_subsidiary_ports_get(int unit, bcm_port_t port, bcm_pbmp_t *pbmp)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_port)) {
        int i;
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        if ((port != 30) && (port != 38) && (port != 42) && (port != 50)) {
            return BCM_E_PORT;
        }
        BCM_PBMP_CLEAR(*pbmp);
        for (i = 0; i < 4; i++) {
            if (SOC_PORT_VALID(unit, port + i)) {
                BCM_PBMP_PORT_ADD(*pbmp, port + i);
            }
        }
        rv = BCM_E_NONE;
    }
#endif
    return rv;
}

int	  
bcm_esw_port_congestion_config_get(int unit, bcm_gport_t port, 	  
                                   bcm_port_congestion_config_t *config)	  
{	  
    int rv = BCM_E_UNAVAIL;	  
    return(rv);	  
}	  
 	  
int	  
bcm_esw_port_congestion_config_set(int unit, bcm_gport_t port, 	  
                                   bcm_port_congestion_config_t *config)	  
{	  
    int rv = BCM_E_UNAVAIL;	  
    return(rv);
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_port_cleanup(int unit)
{
#if defined(BCM_DRACO15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_DRACO15(unit) || SOC_IS_FB_FX_HX(unit)) {
        bcm_port_t port;

        for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
             if (PORT(unit, port).p_vd_pbvl != NULL) {
                 sal_free(PORT(unit, port).p_vd_pbvl);
                 PORT(unit, port).p_vd_pbvl = NULL;
             }
        }

        if (bcm_port_info[unit] != NULL) {
            sal_free(bcm_port_info[unit]);
            bcm_port_info[unit] = NULL;
        }
    }
#endif /* BCM_DRACO15_SUPPORT || BCM_FIREBOLT_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_port_sw_dump
 * Purpose:
 *     Displays port information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 * Note:
 *    Skip following structure fields: p_phy, p_mac
 */
void
_bcm_port_sw_dump(int unit)
{
    _bcm_port_info_t *info;
    int              i, j;
    int              idxmax, entries;

    info = bcm_port_info[unit];
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm) + 1;
    entries = (idxmax + (_BCM_PORT_VD_PBVL_ESIZE -  1)) / _BCM_PORT_VD_PBVL_ESIZE;

    
    if (info != NULL) {
        soc_cm_print("     Protocol based VLAN - \n");
        soc_cm_print("       Total Entries : %d\n", entries);

        for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
            soc_cm_print("       Port %2d : ", i);
            if (info[i].p_vd_pbvl == NULL) {
                soc_cm_print("\n");
                continue;
            }

            for (j = 0; j < entries; j++) {
                if ((j > 0) && !(j % 10)) {
                    soc_cm_print("\n                 ");
                }
                soc_cm_print(" 0x%2.2x", info[i].p_vd_pbvl[j]);
            }
            soc_cm_print("\n");
        }
    }

    soc_cm_print("\n");

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
