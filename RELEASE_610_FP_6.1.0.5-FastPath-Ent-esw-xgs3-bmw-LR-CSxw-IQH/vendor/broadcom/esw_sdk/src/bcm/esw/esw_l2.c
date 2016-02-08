/*
 * $Id: esw_l2.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * L2 - Broadcom StrataSwitch Layer-2 switch API.
 */

#include <sal/core/libc.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/l2x.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#if defined(BCM_EASYRIDER_SUPPORT)
#include <bcm_int/esw/easyrider.h>
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
#include <bcm_int/esw/firebolt.h>	  
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#include <soc/triumph.h>	  
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_XGS12_SWITCH_SUPPORT)
#include <bcm_int/esw/draco.h>	  
#endif /* BCM_XGS12_SWITCH_SUPPORT */
#include <bcm_int/esw/mpls.h>

#include <bcm_int/esw_dispatch.h>

/****************************************************************
 *
 * L2 software tables, per unit.
 *
 ****************************************************************/

static int _l2_init[BCM_MAX_NUM_UNITS];


/****************************************************************
 *
 * L2 learn limit software representation, per unit.
 *
 ****************************************************************/

#define BCM_MAC_LIMIT_MAX   (soc_mem_index_count(unit, L2Xm) - 1)

/*
 * Define:
 *	L2_INIT
 * Purpose:
 *	Causes a routine to return BCM_E_INIT (or some other
 *	error) if L2 software module is not yet initialized.
 */

#define L2_INIT(unit) do { \
	if (_l2_init[unit] < 0) return _l2_init[unit]; \
	if (_l2_init[unit] == 0) return BCM_E_INIT; \
	} while (0);

/* forward declaration */
static int _bcm_l2_bpdu_init(int unit);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
STATIC int _bcm_l2_learn_limit_init(int unit);
STATIC int _bcm_l2_learn_limit_porttrunk_set(int unit, int port_tgid, 
                                             uint32 flags, int limit);
STATIC int _bcm_l2_learn_limit_port_set(int unit, bcm_port_t port,
                                        uint32 flags, int limit);
STATIC int _bcm_l2_learn_limit_trunk_set(int unit, bcm_trunk_t trunk,
                                         uint32 flags, int limit);
#endif /* BCM_TRX_SUPPORT || BCM_RAPTOR_SUPPORT */

/*
 * Function:
 *	_bcm_esw_l2_gport_parse
 * Description:
 *	Parse gport in the l2 table
 * Parameters:
 *	unit -      [IN] StrataSwitch PCI device unit number (driver internal).
 *  l2addr -    [IN/OUT] l2 addr structure to parse and fill
 *  sub_port -  [OUT] Virtual port group (-1 if not supported)
 *  mpls_port - [OUT] MPLS port (-1 if not supported)
 * Returns:
 *	BCM_E_XXX
 */
int 
_bcm_esw_l2_gport_parse(int unit, bcm_l2_addr_t *l2addr, 
                        _bcm_l2_gport_params_t *params)
{
    int             id;
    bcm_port_t      _port;
    bcm_trunk_t     _trunk;
    bcm_module_t    _modid;

    if ((NULL == l2addr) || (NULL == params)){
        return BCM_E_PARAM;
    }

    params->param0 = -1;
    params->param1 = -1;
    params->type = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, l2addr->port, &_modid, &_port, 
                               &_trunk, &id));

    if (BCM_TRUNK_INVALID != _trunk && (-1 == id)) {
        params->param0 = _trunk;
        params->type = _SHR_GPORT_TYPE_TRUNK;
        return BCM_E_NONE;
    }

    if ((-1 != _port) && (-1 == id)) {
        params->param0 = _port;
        if (_port == CMIC_PORT(unit)) {
            params->type = _SHR_GPORT_TYPE_LOCAL_CPU;
            return BCM_E_NONE;
        }
        params->param1 = _modid;
        params->type = _SHR_GPORT_TYPE_MODPORT;
        return BCM_E_NONE;
    }

    if (-1 != id) {
        params->param0 = id;
        if (BCM_GPORT_IS_SUBPORT_GROUP(l2addr->port)) {
            params->type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
           
        } else if (BCM_GPORT_IS_MPLS_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_MPLS_PORT;
        } else if (BCM_GPORT_IS_MIM_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_MIM_PORT;
        } else if (BCM_GPORT_IS_WLAN_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_WLAN_PORT;
        }

        return BCM_E_NONE;
    }

    return BCM_E_PORT;
}

/*
 * Function:
 *	bcm_esw_l2_init
 * Description:
 *	Perform required initializations to L2 table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_l2_init(int unit)
{
    int rv;

    _l2_init[unit] = 0;

    if (SOC_IS_XGS_FABRIC(unit)) {
        _l2_init[unit] = BCM_E_UNAVAIL;
        return BCM_E_NONE;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Turn off arl aging */
        BCM_IF_ERROR_RETURN
            (SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, 0, 0));

        bcm_esw_l2_detach(unit);
    }

    /*
     * Call chip-dependent initialization
     */
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_l2_init(unit));

    if (!SOC_WARM_BOOT(unit)) {
        /*
         * Init BPDU station address registers.
         */
        rv = _bcm_l2_bpdu_init(unit);
        if (rv < 0 && rv != BCM_E_UNAVAIL) {
            return rv;
        }

        /*
         * Init L2 cache
         */
        rv = bcm_esw_l2_cache_init(unit);
        if (rv < 0 && rv != BCM_E_UNAVAIL) {
            return rv;
        }
    }

    /*
     * Init L2 learn limit
     */
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    rv = _bcm_l2_learn_limit_init(unit);
    if (rv < 0 && rv != BCM_E_UNAVAIL) {
        return rv;
    }
#endif /* BCM_TRIUMPH_SUPPORT  || BCM_RAPTOR_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    /*
     * Init class based learning
     */
    if (SOC_MEM_IS_VALID(unit, PORT_CBL_TABLEm)) { 
        BCM_IF_ERROR_RETURN(soc_mem_clear(unit, PORT_CBL_TABLEm, MEM_BLOCK_ALL, 0));
    }
#endif /* BCM_TRX_SUPPORT */

    _l2_init[unit] = 1;		/* some positive value */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_l2_detach
 * Purpose:
 *      Clean up l2 bcm layer when unit is detached
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_l2_detach(int unit)
{
    /*
     * Call chip-dependent finalization.
     */
    SOC_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_l2_term(unit));

    _l2_init[unit] = 0;
    return BCM_E_NONE;
}


/*
 * Function:
 *	_bcm_esw_delete_replace_flags_convert
 * Description:
 *	A helper function to all delete_by APIs to use bcm_l2_replace. 
 *  Converts L2 flags to L2 replace flags compatable to use with bcm_l2_replace
 * Parameters:
 *	unit        [IN]- device unit
 *	flags       [IN] - BCM_L2_DELETE_XXX
 *	repl_flags  [OUT]- Vflags BCM_L2_REPLACE_XXX
 * Returns:
 *	BCM_E_XXX	
 */

int 
_bcm_esw_delete_replace_flags_convert(int unit, uint32 flags, uint32 *repl_flags)
{
    uint32 tmp_flags = BCM_L2_REPLACE_DELETE;

    if (flags & BCM_L2_DELETE_PENDING) {
        tmp_flags |= BCM_L2_REPLACE_PENDING;
    }
    if (flags & BCM_L2_DELETE_STATIC) {
        tmp_flags |= BCM_L2_REPLACE_MATCH_STATIC; 
    }
    if (flags & BCM_L2_DELETE_NO_CALLBACKS) {
        tmp_flags |= BCM_L2_REPLACE_NO_CALLBACKS;
    }
    *repl_flags = tmp_flags;
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_addr_add
 * Description:
 *	Add a MAC address to the Switch Address Resolution Logic (ARL)
 *	port with the given VLAN ID and parameters.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Use CMIC_PORT(unit) to associate the entry with the CPU.
 *	Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 */

int
bcm_esw_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    L2_INIT(unit);

    /* Input parameters check. */
    if (NULL == l2addr) {
        return (BCM_E_PARAM);
    }

    if (l2addr->flags & BCM_L2_LOCAL_CPU) {
        l2addr->port = CMIC_PORT(unit);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_modid_get(unit, &l2addr->modid));
    }

    if (SOC_L2X_GROUP_ENABLE_GET(unit)) {
        if ((l2addr->group > SOC_ADDR_CLASS_MAX(unit)) || 
            (l2addr->group < 0)) {
            return (BCM_E_PARAM);
        }
        if (!BCM_PBMP_IS_NULL(l2addr->block_bitmap)) {
            return (BCM_E_PARAM);
        }
    } else {
        if (l2addr->group)  {
            return (BCM_E_PARAM);
        }
    }
    
    SOC_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_l2_addr_add(unit, l2addr));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete
 * Description:
 *	Delete an L2 address (MAC+VLAN) from the device
 * Parameters:
 *	unit - device unit
 *	mac  - MAC address to delete
 *	vid  - VLAN id
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    L2_INIT(unit);

   return mbcm_driver[unit]->mbcm_l2_addr_delete(unit, mac, vid);
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_port
 * Description:
 *	Delete L2 addresses associated with a destination module/port.
 * Parameters:
 *	unit  - device unit
 *	mod   - module id (or -1 for local unit)
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */

int
bcm_esw_l2_addr_delete_by_port(int unit,
			   bcm_module_t mod, bcm_port_t port,
			   uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    L2_INIT(unit);
    
    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.port = port;
    match_addr.modid = mod;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    return bcm_esw_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0); 
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_mac
 * Description:
 *	Delete L2 entries associated with a MAC address.
 * Parameters:
 *	unit  - device unit
 *	mac   - MAC address
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_mac(int unit, bcm_mac_t mac, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));

    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));
    
    BCM_IF_ERROR_RETURN(
        _bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_MAC;
    return bcm_esw_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0); 
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_vlan
 * Description:
 *	Delete L2 entries associated with a VLAN.
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_vlan(int unit, bcm_vlan_t vid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN;
    return bcm_esw_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0); 
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_trunk
 * Description:
 *	Delete L2 entries associated with a trunk.
 * Parameters:
 *	unit  - device unit
 *	tid   - trunk id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_trunk(int unit, bcm_trunk_t tid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.tgid = tid;
    match_addr.flags |= BCM_L2_TRUNK_MEMBER;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    return bcm_esw_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0); 
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_mac_port
 * Description:
 *	Delete L2 entries associated with a MAC address and
 *	a destination module/port
 * Parameters:
 *	unit  - device unit
 *	mac   - MAC address
 *	mod   - module id
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_mac_port(int unit, bcm_mac_t mac,
			       bcm_module_t mod, bcm_port_t port,
			       uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.port = port;
    match_addr.modid = mod;
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));

    BCM_IF_ERROR_RETURN(
        _bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    repl_flags |= BCM_L2_REPLACE_MATCH_MAC;
    return bcm_esw_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0); 
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_vlan_port
 * Description:
 *	Delete L2 entries associated with a VLAN and
 *	a destination module/port
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	mod   - module id
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_vlan_port(int unit, bcm_vlan_t vid,
				bcm_module_t mod, bcm_port_t port,
				uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    match_addr.port = port;
    match_addr.modid = mod;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN;
    return bcm_esw_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0); 
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_vlan_trunk
 * Description:
 *	Delete L2 entries associated with a VLAN and a
 *      destination trunk.
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	tid   - trunk group id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_vlan_trunk(int unit, bcm_vlan_t vid,
                                 bcm_trunk_t tid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    match_addr.tgid = tid;
    match_addr.flags |= BCM_L2_TRUNK_MEMBER;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN;
    return bcm_esw_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0); 
}

/*
 * Function:
 *      bcm_esw_l2_addr_delete_by_mac_vpn
 * Description:
 *      Delete L2 entry with matching MAC address and VPN.
 * Parameters:
 *      unit  - device unit
 *      mac   - MAC address
 *      vpn   - L2 VPN identifier
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_addr_delete_by_mac_vpn(int unit, bcm_mac_t mac, bcm_vpn_t vpn)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_addr_delete_by_vpn
 * Description:
 *      Delete L2 entries associated with an L2 VPN.
 * Parameters:
 *      unit  - device unit
 *      vpn   - L2 VPN identifier
 *      flags - BCM_L2_DELETE_XXX
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *      L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_vpn(int unit, bcm_vpn_t vpn, uint32 flags)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_l2_addr_get
 * Description:
 *	Given a MAC address and VLAN ID, check if the entry is present
 *	in the L2 table, and if so, return all associated information.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	mac - input MAC address to search
 *	vid - input VLAN ID to search
 *	l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *	BCM_E_NONE		Success (l2addr filled in)
 *	BCM_E_PARAM		Illegal parameter (NULL pointer)
 *	BCM_E_INTERNAL		Chip access failure
 *	BCM_E_NOT_FOUND	        Address not found (l2addr not filled in)
 */

int
bcm_esw_l2_addr_get(int unit, mac_addr_t mac,
		bcm_vlan_t vid, bcm_l2_addr_t *l2addr)
{
    L2_INIT(unit);
    return mbcm_driver[unit]->mbcm_l2_addr_get(unit, mac, vid, l2addr);
}

/****************************************************************************
 *
 * L2X Message Registration
 *
 ****************************************************************************/

#define L2_CB_MAX		3

typedef struct l2x_cb_entry_s {
    bcm_l2_addr_callback_t	fn;
    void			*fn_data;
} l2_cb_entry_t;

#define _BCM_L2X_THREAD_STOP      (1 << 0)
typedef struct l2_data_s {
    l2_cb_entry_t		cb[L2_CB_MAX];
    int				cb_count;
    int                 flags;
    sal_mutex_t                 l2_mutex;
} l2_data_t;

static l2_data_t l2_data[SOC_MAX_NUM_DEVICES];


#define L2_LOCK(unit) \
    sal_mutex_take(l2_data[unit].l2_mutex, sal_mutex_FOREVER)

#define L2_UNLOCK(unit) sal_mutex_give(l2_data[unit].l2_mutex)

#define L2_MUTEX(unit) if (l2_data[unit].l2_mutex == NULL &&                 \
        (l2_data[unit].l2_mutex = sal_mutex_create("bcm_l2_lock")) == NULL)  \
            return BCM_E_MEMORY

/*
 * Function:
 *      bcm_esw_l2_port_native
 * Purpose:
 *      Determine if the given port is "native" from the point
 *      of view of L2.
 * Parameters:
 *      unit       - The unit
 *      modid      - Module ID of L2 entry being queried
 *      port       - Module port of L2 entry being queried
 * Returns:
 *      TRUE (> 0) if (modid, port) is front panel/CPU port for unit.
 *      FALSE (0) otherwise.
 *      < 0 on error.
 *
 *      Native means front panel, but also includes the CPU port.
 *      HG ports are always excluded as are ports marked as internal or
 *      external stacking
 */

int
bcm_esw_l2_port_native(int unit, int modid, int port)
{
    int unit_modid;
    int unit_port;  /* In case unit supports multiple module IDs */
    bcm_trunk_t     tgid;
    int             id, isLocal;

    L2_INIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &modid,
            &port, &tgid, &id));

        if (-1 != id || BCM_TRUNK_INVALID != tgid) {
            return FALSE;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, modid, &isLocal));
    if (isLocal != TRUE) {
        return FALSE;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &unit_modid));
    if (unit_modid != modid) {
        unit_port = port + 32; 
    } else {
        unit_port = port;
    }

    if (IS_ST_PORT(unit, unit_port)) {
        return FALSE;
    } 

    return TRUE;
}


/****************************************************************************
 *
 * L2 Callbacks Registration
 *
 ****************************************************************************/
static bcm_l2_addr_callback_t _bcm_l2_cbs[SOC_MAX_NUM_DEVICES];
static void *_bcm_l2_cb_data[SOC_MAX_NUM_DEVICES];


/*
 * Function:
 *      _bcm_l2_register_callback
 * Description:
 *      Call back to handle bcm_l2_addr_register clients.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      entry_del - Entry to be deleted or updated, NULL if none.
 *      entry_add - Entry to be inserted or updated, NULL if none.
 *      fn_data - unused
 * Notes:
 *      Only one callback per unit to the bcm layer is supported here.
 *      Multiple bcm client callbacks per unit are supported in the bcm layer.
 */
void
_bcm_l2_register_callback(int unit,
                          l2x_entry_t *entry_del,
                          l2x_entry_t *entry_add,
                          void *fn_data)
{
    if (_bcm_l2_cbs[unit] != NULL) {
        bcm_l2_addr_t l2addr_del, l2addr_add;
        uint32 flags = 0; /* Common flags: Move, From/to native */

        /* First, set up the entries:  decode HW entries and set flags */
        if (entry_del != NULL) {
            (void)_bcm_esw_l2_from_l2x(unit, 0, &l2addr_del, (uint32 *)entry_del);
        }
        if (entry_add != NULL) {
            (void)_bcm_esw_l2_from_l2x(unit, 0, &l2addr_add, (uint32 *)entry_add);
        }

        if ((entry_del != NULL) && (entry_add != NULL)) { /* It's a move */
            flags |= BCM_L2_MOVE;
            if (SOC_USE_GPORT(unit)) {
                if (l2addr_del.port != l2addr_add.port) {
                    flags |= BCM_L2_MOVE_PORT;
                }
            } else {
                if (l2addr_del.modid != l2addr_add.modid ||
                    l2addr_del.tgid != l2addr_add.tgid ||
                    l2addr_del.port != l2addr_add.port) {
                    flags |= BCM_L2_MOVE_PORT;
                }
            }
            if (bcm_esw_l2_port_native(unit, l2addr_del.modid,
                                       l2addr_del.port) > 0) {
                flags |= BCM_L2_FROM_NATIVE;
                l2addr_del.flags |= BCM_L2_NATIVE;
            }
            if (bcm_esw_l2_port_native(unit, l2addr_add.modid,
                                       l2addr_add.port) > 0) {
                flags |= BCM_L2_TO_NATIVE;
                l2addr_add.flags |= BCM_L2_NATIVE;
            }
            l2addr_del.flags |= flags;
            l2addr_add.flags |= flags;
        } else if (entry_del != NULL) { /* Age out or simple delete */
            if (bcm_esw_l2_port_native(unit, l2addr_del.modid,
                                       l2addr_del.port) > 0) {
                l2addr_del.flags |= BCM_L2_NATIVE;
            }
        } else if (entry_add != NULL) { /* Insert or learn */
            if (bcm_esw_l2_port_native(unit, l2addr_add.modid,
                                       l2addr_add.port) > 0) {
                l2addr_add.flags |= BCM_L2_NATIVE;
            }
        }

        /* The entries are now set up.  Make the callbacks */
        if (entry_del != NULL) {
            _bcm_l2_cbs[unit](unit, &l2addr_del, 0, _bcm_l2_cb_data[unit]);
        }
        if (entry_add != NULL) {
            _bcm_l2_cbs[unit](unit, &l2addr_add, 1, _bcm_l2_cb_data[unit]);
        }
    }
}



/*
 * Function:
 * 	_bcm_l2_addr_callback
 * Description:
 *	Callback used with chip addr registration functions.
 *	This callback calls all the top level client callbacks.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	l2addr
 *	insert
 *	userdata
 * Returns:
 *
 */
static void
_bcm_l2_addr_callback(int unit,
		      bcm_l2_addr_t *l2addr,
		      int insert,
		      void *userdata)
{
    l2_data_t		*ad = &l2_data[unit];
    int i;

    if (ad->l2_mutex == NULL) {
        return;
    }

    L2_LOCK(unit);
    for(i = 0; i < L2_CB_MAX; i++) {
	if(ad->cb[i].fn) {
	    ad->cb[i].fn(unit, l2addr, insert, ad->cb[i].fn_data);
	}
    }
    L2_UNLOCK(unit);
}


/*
 * Function:
 *	bcm_esw_l2_addr_register
 * Description:
 *	Register a callback routine that will be called whenever
 *	an entry is inserted into or deleted from the L2 address table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	fn - Callback function of type bcm_l2_addr_callback_t.
 *	fn_data - Arbitrary value passed to callback along with messages
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_addr_register(int unit,
		     bcm_l2_addr_callback_t fn,
		     void *fn_data)
{
    l2_data_t		*ad = &l2_data[unit];
    int i;
#if defined(BCM_XGS_SWITCH_SUPPORT)
    int rv;
    int usec;
#endif /* BCM_XGS_SWITCH_SUPPORT */

    if (!SOC_IS_XGS_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

    L2_INIT(unit);

    _bcm_l2_cbs[unit] = _bcm_l2_addr_callback;
    _bcm_l2_cb_data[unit] = NULL;

    L2_MUTEX(unit);
    L2_LOCK(unit);
#if defined(BCM_XGS_SWITCH_SUPPORT)
    /* Start L2x thread if it isn't running already. */
    if (!soc_l2x_running(unit, NULL, NULL)) {
        usec = (SAL_BOOT_BCMSIM)? BCMSIM_L2XMSG_INTERVAL : 3000000;
        usec = soc_property_get(unit, spn_L2XMSG_THREAD_USEC, usec);
        rv = soc_l2x_start(unit, 0, usec);
        if ((BCM_FAILURE(rv)) && (rv != BCM_E_UNAVAIL)) {
            _bcm_l2_cbs[unit] = NULL;
            _bcm_l2_cb_data[unit] = NULL;
            L2_UNLOCK(unit);
            return(rv);
        }
        ad->flags |= _BCM_L2X_THREAD_STOP;
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

    /* See if the function is already registered (with same data) */
    for (i = 0; i < L2_CB_MAX; i++) {
	if (ad->cb[i].fn == fn && ad->cb[i].fn_data == fn_data) {
            L2_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }

    /* Not found; add to list. */
    for (i = 0; i < L2_CB_MAX; i++) {
	if (ad->cb[i].fn == NULL) {
	    ad->cb[i].fn = fn;
	    ad->cb[i].fn_data = fn_data;
	    ad->cb_count++;
	    break;
	}
    }

    L2_UNLOCK(unit);
    return i >= L2_CB_MAX ? BCM_E_RESOURCE : BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_addr_unregister
 * Description:
 *	Unregister a previously registered callback routine.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	fn - Same callback function used to register callback
 *	fn_data - Same arbitrary value used to register callback
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Both callback and userdata must match from original register call.
 */

int
bcm_esw_l2_addr_unregister(int unit,
		       bcm_l2_addr_callback_t fn,
		       void *fn_data)
{
    l2_data_t		*ad = &l2_data[unit];
    int             rv = BCM_E_NOT_FOUND;
    int			i;

    if (!SOC_IS_XGS_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

    L2_INIT(unit);
    L2_MUTEX(unit);
    L2_LOCK(unit);

    for (i = 0; i < L2_CB_MAX; i++) {
        if((ad->cb[i].fn == fn) && (ad->cb[i].fn_data == fn_data)) {
            rv = BCM_E_NONE;
            ad->cb[i].fn = NULL;
            ad->cb[i].fn_data = NULL;
            ad->cb_count--;
            if (ad->cb_count == 0) {
                _bcm_l2_cbs[unit] = NULL;
                _bcm_l2_cb_data[unit] = NULL;

#if defined(BCM_XGS_SWITCH_SUPPORT)
                /* Stop l2x thread if callback registration started it. */
                if (ad->flags & _BCM_L2X_THREAD_STOP) {
                    rv = soc_l2x_stop(unit);
                    ad->flags &= ~_BCM_L2X_THREAD_STOP;
                }
#endif /* BCM_XGS_SWITCH_SUPPORT */
            }
        }
    }

    L2_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *	bcm_esw_l2_age_timer_set
 * Description:
 *	Set the age timer for all blocks.
 *	Setting the value to 0 disables the age timer.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	age_seconds - Age timer value in seconds
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_age_timer_set(int unit, int age_seconds)
{
    int			max_value;
    int			enabled;
    int			rv;

    L2_INIT(unit);

    BCM_IF_ERROR_RETURN
	(SOC_FUNCTIONS(unit)->soc_age_timer_max_get(unit, &max_value));

    if (age_seconds < 0 || age_seconds > max_value) {
	return BCM_E_PARAM;
    }

    enabled = age_seconds ? 1 : 0;

    /* Lock ARLm b/c soc_arl_freeze() does read/modify/write of AGE_TIMER */
    SOC_L2X_MEM_LOCK(unit);
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, age_seconds, enabled);
    SOC_L2X_MEM_UNLOCK(unit);


    return rv;
}

/*
 * Function: *	bcm_esw_l2_age_timer_get
 * Description:
 *	Returns the current age timer value.
 *	The value is 0 if aging is not enabled.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	age_seconds - Place to store returned age timer value in seconds
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_age_timer_get(int unit, int *age_seconds)
{
    int			seconds, enabled;
    int			rv;

    L2_INIT(unit);

    if (age_seconds == NULL) {
	return BCM_E_PARAM;
    }

    /*
     * Lock ARLm because soc_arl_freeze() does read/modify/write of
     * AGE_TIMER
     */

    SOC_L2X_MEM_LOCK(unit);
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &seconds, &enabled);
    SOC_L2X_MEM_UNLOCK(unit);

    BCM_IF_ERROR_RETURN(rv);

    if (enabled) {
	*age_seconds = seconds;
    } else {
	*age_seconds = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_addr_freeze
 * Description:
 *	Temporarily quiesce ARL from all activity (learning, aging)
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_addr_freeze(int unit)
{
    L2_INIT(unit);

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
	return soc_l2x_freeze(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_l2_addr_thaw
 * Description:
 *	Restore normal ARL activity.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_addr_thaw(int unit)
{
    L2_INIT(unit);

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
	return soc_l2x_thaw(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *	_bcm_l2_bpdu_init
 * Description:
 *	Initialize all BPDU addresses to recognize the 802.1D
 *      Spanning Tree address on all chips.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 * Notes:
 */

static int
_bcm_l2_bpdu_init(int unit)
{
    int		i, nbpdu;
    mac_addr_t	mac;

    nbpdu = bcm_esw_l2_bpdu_count(unit);

    /* Spanning Tree addr (01:80:c2:00:00:00) used as default entries */
    mac[0] = 0x01;
    mac[1] = 0x80;
    mac[2] = 0xc2;
    mac[3] = mac[4] = mac[5] = 0x00;

    for (i = 0; i < nbpdu; i++) {
	BCM_IF_ERROR_RETURN(soc_bpdu_addr_set(unit, i, mac));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_bpdu_count
 * Description:
 *	Retrieve the number of BPDU addresses available on the
 *      specified unit.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	number of BPDU address entries
 */

int
bcm_esw_l2_bpdu_count(int unit)
{
    if (SOC_IS_XGS_SWITCH(unit)) {
	return 6;
    }

    return 0;
}

/*
 * Function:
 *	bcm_esw_l2_bpdu_set
 * Description:
 *	Set BPDU address
 * Parameters:
 *	unit - switch unit
 *	index - BPDU index
 *	addr - 6 byte mac address
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_*			Failure
 */

int
bcm_esw_l2_bpdu_set(int unit, int index, mac_addr_t addr)
{
    int	nbpdu;

    L2_INIT(unit);

    nbpdu = bcm_esw_l2_bpdu_count(unit);

    if (nbpdu == 0) {
	return BCM_E_UNAVAIL;
    }
    if (index < 0 || index >= nbpdu) {
	return BCM_E_PARAM;
    }
    return soc_bpdu_addr_set(unit, index, addr);
}

/*
 * Function:
 *	bcm_esw_l2_bpdu_get
 * Description:
 *	Return BPDU address
 * Parameters:
 *	unit - switch unit
 *	index - BPDU index
 *	addr - (OUT) 6 byte mac address
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_*			Failure
 */

int
bcm_esw_l2_bpdu_get(int unit, int index, mac_addr_t *addr)
{
    int nbpdu;

    L2_INIT(unit);

    nbpdu = bcm_esw_l2_bpdu_count(unit);

    if (nbpdu == 0) {
	return BCM_E_UNAVAIL;
    }
    if (index < 0 || index >= nbpdu) {
	return BCM_E_PARAM;
    }
    return soc_bpdu_addr_get(unit, index, addr);
}


/*
 * Function:
 *	bcm_esw_l2_key_dump
 * Purpose:
 *	Dump the key (VLAN+MAC) portion of a hardware-independent
 *	L2 address for debugging
 * Parameters:
 *	unit - Unit number
 *	pfx - String to print before output
 *	entry - Hardware-independent L2 entry to dump
 *	sfx - String to print after output
 */

int
bcm_esw_l2_key_dump(int unit, char *pfx, bcm_l2_addr_t *entry, char *sfx)
{
    /*
     * In VxWorks, in interrupt context, soc_cm_print uses logMsg, which only
     * allows up to 6 args.  That's why the MAC address is formatted as
     * two hex numbers here.
     */
    soc_cm_print("l2: %sVLAN=0x%03x MAC=0x%02x%02x%02x"
                 "%02x%02x%02x%s", pfx, entry->vid,
		 entry->mac[0], entry->mac[1], entry->mac[2],
		 entry->mac[3], entry->mac[4], entry->mac[5], sfx);

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_conflict_get
 * Purpose:
 *	Given an L2 address, return existing addresses which could conflict.
 * Parameters:
 *	unit		- switch device
 *	addr		- l2 address to search for conflicts
 *	cf_array	- (OUT) list of l2 addresses conflicting with addr
 *	cf_max		- number of entries allocated to cf_array
 *	cf_count	- (OUT) actual number of cf_array entries filled
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *	Given an L2 or L2 multicast address, return any existing L2 or
 *	L2 multicast addresses which might prevent it from being
 *	inserted because a chip resource (like a hash bucket) is full.
 *	This routine could be used if bcm_l2_addr_add or bcm_l2_mcast_add
 *	returns BCM_E_FULL.
 */

int
bcm_esw_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
		    bcm_l2_addr_t *cf_array, int cf_max,
		    int *cf_count)
{
    L2_INIT(unit);
    /*
     * Call chip-dependent handler.
     */
    SOC_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_l2_conflict_get(unit, addr,
                                       cf_array, cf_max, cf_count));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_l2_tunnel_add
 * Purpose:
 *    Add a (MAC, VLAN) for tunnel/MPLS processing, frames
 *      destined to (MAC, VLAN) is subjected to TUNNEL/MPLS processing.
 * Parameters:
 *      unit - StrataXGS unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */

int
bcm_esw_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        my_station_entry_t mse;
	sal_memset(&mse, 0, sizeof(mse));
	soc_MY_STATIONm_mac_addr_set(unit, &mse, MAC_ADDRf, mac);
	soc_MY_STATIONm_field32_set(unit, &mse, VLAN_IDf, vlan);
	soc_MY_STATIONm_field32_set(unit, &mse, VALIDf, 1);
	return soc_mem_insert(unit, MY_STATIONm, COPYNO_ALL, &mse);
    }
#endif

#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (SOC_IS_TR_VL(unit)) {
        return bcm_trx_metro_l2_tunnel_add(unit, mac, vlan);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_tunnel_delete
 * Purpose:
 *      Remove a tunnel processing indicator for an L2 address
 * Parameters:
 *      unit - StrataXGS unit number
 *      mac  - MAC address
      vlan - VLAN ID
 */

int
bcm_esw_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        my_station_entry_t mse;
	sal_memset(&mse, 0, sizeof(mse));
	soc_MY_STATIONm_mac_addr_set(unit, &mse, MAC_ADDRf, mac);
	soc_MY_STATIONm_field32_set(unit, &mse, VLAN_IDf, vlan);
	return soc_mem_delete(unit, MY_STATIONm, COPYNO_ALL, &mse);
    }
#endif

#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (SOC_IS_TR_VL(unit)) {
        return bcm_trx_metro_l2_tunnel_delete(unit, mac, vlan);
    }
#endif
    return BCM_E_UNAVAIL;

}

/*
 * Function:
 *      bcm_l2_tunnel_delete_all
 * Purpose:
 *      Remove all tunnel processing indicating L2 addresses
 * Parameters:
 *      unit - StrataXGS unit number
 */

int
bcm_esw_l2_tunnel_delete_all(int unit)
{
#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
	return soc_mem_clear(unit, MY_STATIONm, COPYNO_ALL, 0);
    }
#endif

#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (SOC_IS_TR_VL(unit)) {
        return bcm_trx_metro_l2_tunnel_delete_all(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_clear
 * Purpose:
 *      Clear the L2 layer
 * Parameters:
 *      unit  - BCM unit number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_l2_clear(int unit)
{
    if (_l2_init[unit] < 0) {
        /* This sets the init bit, so if init failed we shouldn't
         * override that here. */
        return _l2_init[unit];
    }

    bcm_esw_l2_detach(unit);

    /*
     * Call chip-dependent initialization
     */
    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l2_init(unit));

#if defined(BCM_XGS_SWITCH_SUPPORT)
    /* Stop l2x thread if callback registration started it. */
    if (l2_data[unit].flags & _BCM_L2X_THREAD_STOP) {
        BCM_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

    /* Clear l2_data structure */
    l2_data[unit].cb_count = 0;
    l2_data[unit].flags = 0;
    sal_memset(&l2_data[unit].cb, 0, sizeof(l2_data[unit].cb));

    _l2_init[unit] = 1;		/* some positive value */

    return BCM_E_NONE;
}

/*
 * MAC learn limit
 */
#ifdef BCM_TRIUMPH_SUPPORT
/*
 * Function:
 *      _bcm_tr_l2_learn_limit_system_set
 * Purpose:
 *      Set MAC learn limit for a system 
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask 
 *      limit - System limit of MAC addresses to learn
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_learn_limit_system_set(int unit, uint32 flags, int limit)
{
    uint32 rval, orval;
    int tocpu, drop, enable;
    
    if (limit < 0) {
        tocpu = 0;
        drop = 0;
        limit = soc_mem_index_max(unit, L2Xm);
        enable = 0;
    } else {
        tocpu = flags & BCM_L2_LEARN_LIMIT_ACTION_CPU ? 1 : 0;
        drop = flags & BCM_L2_LEARN_LIMIT_ACTION_DROP ? 1 : 0;
        enable = 1;
    }

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    orval = rval;
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, 
                      SYS_OVER_LIMIT_TOCPUf, tocpu);
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval,
                      SYS_OVER_LIMIT_DROPf, drop);
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, SYS_LIMITf, limit);
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, ENABLEf, enable);
    if (rval != orval) {
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMIT_CONTROLr(unit, rval));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_learn_limit_system_get
 * Purpose:
 *      Get MAC learn limit for a system
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask
 *      limit - System limit of MAC addresses to learn
 *              Negative if limit disabled
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_learn_limit_system_get(int unit, uint32 *flags, int *limit)
{
    uint32  rval;

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    *flags &= ~(BCM_L2_LEARN_LIMIT_ACTION_DROP |
                BCM_L2_LEARN_LIMIT_ACTION_CPU);
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                          SYS_OVER_LIMIT_TOCPUf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
    }
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                          SYS_OVER_LIMIT_DROPf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
    }
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval, ENABLEf)) {
        *limit = soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                                   SYS_LIMITf);
    } else {
        *limit = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_learn_limit_set
 * Purpose:
 *      Set MAC learn limit for a port/trunk/vlan
 * Parameters:
 *      unit  - BCM unit number
 *      mem   - PORT_OR_TRUNK_MAC_LIMITm or VLAN_OR_VFI_MAC_LIMITm
 *      index - for PORT_OR_TRUNK_MAC_LIMITm
 *                  0 - 127: trunk identifier
 *                  128 - 181: port identifier + 128
 *              for VLAN_OR_VFI_MAC_LIMITm
 *                  0 - 4095: VLAN identifier
 *                  4096 - 5119: virtual forwarding instance + 4096
 *      flags - Action bitmask
 *      limit - Max number of MAC addresses can be learned for the specified
 *              identifier
 *              Negative if limit disabled (i.e. unlimit)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_learn_limit_set(int unit, soc_mem_t mem, int index, uint32 flags,
                           int limit)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    int rv;

    if (limit < 0) {
        soc_mem_lock(unit, mem);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                           soc_mem_entry_null(unit, mem));
        soc_mem_unlock(unit, mem);
        return rv;
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry);
    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, mem, &entry, OVER_LIMIT_TOCPUf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_CPU ? 1 : 0);
        soc_mem_field32_set(unit, mem, &entry, OVER_LIMIT_DROPf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_DROP ? 1 : 0);
        soc_mem_field32_set(unit, mem, &entry, LIMITf, limit);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry);
    }
    soc_mem_unlock(unit, mem);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, ENABLEf, 1);
    return (WRITE_SYS_MAC_LIMIT_CONTROLr(unit, rval));
}

/*
 * Function:
 *      _bcm_tr_l2_learn_limit_get
 * Purpose:
 *      Get MAC learn limit for a port/trunk/vlan
 * Parameters:
 *      unit  - BCM unit number
 *      mem   - PORT_OR_TRUNK_MAC_LIMITm or VLAN_OR_VFI_MAC_LIMITm
 *      index - for PORT_OR_TRUNK_MAC_LIMITm
 *                  0 - 127: trunk identifier
 *                  128 - 181: port identifier + 128
 *              for VLAN_OR_VFI_MAC_LIMITm
 *                  0 - 4095: VLAN identifier
 *                  4096 - 5119: virtual forwarding instance + 4096
 *      flags - Action bitmask
 *      limit - Max number of MAC addresses can be learned for the specified
 *              identifier
 *              Negative if limit disabled (i.e. unlimit)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_learn_limit_get(int unit, soc_mem_t mem, int index, uint32* flags,
                           int* limit)
{
    uint32 entry[SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));
    *flags &= ~(BCM_L2_LEARN_LIMIT_ACTION_DROP |
                BCM_L2_LEARN_LIMIT_ACTION_CPU);
    if (soc_mem_field32_get(unit, mem, &entry, OVER_LIMIT_DROPf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
    }
    if (soc_mem_field32_get(unit, mem, &entry, OVER_LIMIT_TOCPUf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
    }
    *limit = soc_mem_field32_get(unit, mem, &entry, LIMITf);

    return BCM_E_NONE;
}
#endif

#define PORT_COUNT(unit) (SOC_INFO(unit).port_num)

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
/*
 * Function:
 *      _bcm_l2_learn_limit_porttrunk_set
 * Purpose:
 *      helper function to configure registers
 * Parameters:
 *      unit  - BCM unit number
 *      port_tgid - Port or Trunk number
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative if limit disabled  
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_l2_learn_limit_porttrunk_set(int unit, int port_tgid, uint32 flags,
                                  int limit)
{
    int     rv = BCM_E_NONE;
    port_or_trunk_mac_limit_entry_t lim_entry;
    port_or_trunk_mac_action_entry_t act_entry;


    if (limit > BCM_MAC_LIMIT_MAX){
        return BCM_E_PARAM;
    }
    /* Negative limit disables the check */
    if (limit < 0) {
        soc_mem_lock(unit, PORT_OR_TRUNK_MAC_LIMITm);

        rv = READ_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ANY, port_tgid, &lim_entry);
        if ( BCM_E_NONE == rv) {
            soc_PORT_OR_TRUNK_MAC_LIMITm_field32_set(unit, &lim_entry, 
                                                     PORT_TRUNK_MAC_LIMITf,
                                                     BCM_MAC_LIMIT_MAX);
            rv = WRITE_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ALL, 
                                                port_tgid, &lim_entry);
        }
        soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_LIMITm);

        return rv;
    }


    /* If everything is OK - configure the registers */
    soc_mem_lock(unit, PORT_OR_TRUNK_MAC_LIMITm);
    
    rv = READ_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ANY, port_tgid, &lim_entry);
    if (BCM_E_NONE == rv) {
        soc_PORT_OR_TRUNK_MAC_LIMITm_field32_set(unit, &lim_entry, 
                                                 PORT_TRUNK_MAC_LIMITf,
                                                 limit);
        rv = WRITE_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ALL, 
                                            port_tgid, &lim_entry);
    }
    soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_LIMITm);

    BCM_IF_ERROR_RETURN(rv);

    soc_mem_lock(unit, PORT_OR_TRUNK_MAC_ACTIONm);
    
    rv = READ_PORT_OR_TRUNK_MAC_ACTIONm(unit, MEM_BLOCK_ANY, port_tgid, &act_entry);
    if (BCM_E_NONE == rv) {
        soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_ACTIONm, &act_entry,
                            OVER_LIMIT_DROPf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_DROP ? 1 : 0);
        soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_ACTIONm, &act_entry,
                            OVER_LIMIT_TOCPUf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_CPU ? 1 : 0);
        rv = WRITE_PORT_OR_TRUNK_MAC_ACTIONm(unit, MEM_BLOCK_ALL, port_tgid, &act_entry);
    }
    soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_ACTIONm);

    return rv;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_porttrunk_get
 * Purpose:
 *      helper function to configure registers
 * Parameters:
 *      unit  - BCM unit number
 *      port_tgid - Port or trunk number 
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_l2_learn_limit_porttrunk_get(int unit, int port_tgid, uint32 *flags,
                                  int *limit)
{
    uint32  limit_enable;
    int     org_limit, org_action;
    port_or_trunk_mac_limit_entry_t lim_entry;
    port_or_trunk_mac_action_entry_t act_entry;

    if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(READ_MAC_LIMIT_ENABLEr(unit, &limit_enable));
    org_limit = soc_reg_field_get(unit, MAC_LIMIT_ENABLEr, limit_enable, 
                                  ENABLEf);

    *flags = 0;
    if (!org_limit) {
        *limit = -1;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (READ_PORT_OR_TRUNK_MAC_LIMITm(unit,
                                       MEM_BLOCK_ANY,
                                       port_tgid,
                                       &lim_entry));
    *limit = soc_PORT_OR_TRUNK_MAC_LIMITm_field32_get(unit, &lim_entry, 
                                                 PORT_TRUNK_MAC_LIMITf);
    BCM_IF_ERROR_RETURN
        (READ_PORT_OR_TRUNK_MAC_ACTIONm(unit,
                                        MEM_BLOCK_ANY,
                                        port_tgid,
                                        &act_entry));
    org_action = soc_PORT_OR_TRUNK_MAC_ACTIONm_field32_get(unit, &act_entry,
                                                  OVER_LIMIT_DROPf);
    if (org_action) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
    }
    org_action = soc_PORT_OR_TRUNK_MAC_ACTIONm_field32_get(unit, &act_entry,
                                                  OVER_LIMIT_TOCPUf);
    if (org_action) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_init
 * Purpose:
 *      Init the system to support MAC learn limit
 * Parameters:
 *      unit  - BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_init(int unit)
{

    bcm_port_t                          port;
    bcm_trunk_t                         trunk;
    mac_limit_port_map_table_entry_t    entry;
    int                                 rv = BCM_E_NONE, idx, idx_max;
    uint32                              maxInitVal, zeroInitVal;

    if (soc_feature(unit, soc_feature_mac_learn_limit)) {
        idx = idx_max = 0;
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            _bcm_tr_l2_learn_limit_system_set(unit, 0, -1);
        BCM_IF_ERROR_RETURN
            (soc_mem_clear(unit, PORT_OR_TRUNK_MAC_LIMITm, COPYNO_ALL, FALSE));
        BCM_IF_ERROR_RETURN
            (soc_mem_clear(unit, VLAN_OR_VFI_MAC_LIMITm, COPYNO_ALL, FALSE));
            return BCM_E_NONE;
        }
#endif
        maxInitVal = BCM_MAC_LIMIT_MAX; 
        zeroInitVal = 0;

        /* Initialize all learning limits to max and disable feature */
        BCM_IF_ERROR_RETURN(WRITE_MAC_LIMIT_ENABLEr(unit, zeroInitVal));
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMITr(unit, maxInitVal));
        /* Map initial port/trunk to counters */
        soc_mem_lock(unit, MAC_LIMIT_PORT_MAP_TABLEm);
        soc_mem_lock(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
        soc_mem_lock(unit, PORT_OR_TRUNK_MAC_LIMITm);
        soc_mem_lock(unit, PORT_OR_TRUNK_MAC_ACTIONm);
        PBMP_ALL_ITER(unit, port) {
            /* Initialize the HW tables for port map table*/
            rv = WRITE_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ALL, 
                                                port, &maxInitVal);
            if (rv < 0) {
                goto done;
            }

            rv = WRITE_PORT_OR_TRUNK_MAC_ACTIONm(unit, MEM_BLOCK_ALL, 
                                                 port, &zeroInitVal );
            if (rv < 0) {
                goto done;
            }

            rv = READ_MAC_LIMIT_PORT_MAP_TABLEm(unit, MEM_BLOCK_ANY, 
                                                port, &entry);
            if (rv >= 0) {
                soc_MAC_LIMIT_PORT_MAP_TABLEm_field32_set(unit, &entry,
                                                          INDEXf, port);
                rv = WRITE_MAC_LIMIT_PORT_MAP_TABLEm(unit, MEM_BLOCK_ALL, 
                                                     port, &entry);
            }
            if (rv < 0) {
                goto done;
            }
        }
        /* Initialize the HW tables for trunk map table */
        for (trunk = 0;
             trunk < soc_mem_index_count(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
             trunk ++ ) {
            rv = READ_MAC_LIMIT_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ALL,
                                                 trunk, &entry);
            if (rv) {
                soc_MAC_LIMIT_TRUNK_MAP_TABLEm_field32_set(unit, &entry,
                                                           INDEXf, trunk);
                rv = WRITE_MAC_LIMIT_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ALL, 
                                                      trunk, &entry);
            }
            if (rv < 0) {
                goto done;
            }
        }
done:
    soc_mem_unlock(unit, MAC_LIMIT_PORT_MAP_TABLEm);
    soc_mem_unlock(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
    soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_LIMITm);
    soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_ACTIONm);

    return rv;
    } 

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      _bcm_l2_learn_limit_system_set
 * Purpose:
 *      Set MAC learn limit for a system 
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask 
 *      limit - System limit of MAC addresses to learn
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_system_set(int unit, uint32 flags, int limit)
{
    uint32  limit_enable, limit_action;
    int org_limit = 0;
    
    if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
        return BCM_E_UNAVAIL;
    }

    if (limit > BCM_MAC_LIMIT_MAX){
        return BCM_E_PARAM;
    }

    /* Negative limit disables the check */
    if (limit < 0) {
        BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMITr(unit, &limit_enable));
        soc_reg_field_set(unit, SYS_MAC_LIMITr, &limit_enable, 
                          SYS_MAC_LIMITf, BCM_MAC_LIMIT_MAX);
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMITr(unit, limit_enable));

        return BCM_E_NONE;
    }

    /* If everything is OK - configure the registers */
    /* First configure the Limit and then enable */
    

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMITr(unit, &limit_enable));
    org_limit = soc_reg_field_get(unit, SYS_MAC_LIMITr, limit_enable, 
                                  SYS_MAC_LIMITf);
    if (org_limit != limit) {
        soc_reg_field_set(unit, SYS_MAC_LIMITr, &limit_enable, 
                          SYS_MAC_LIMITf, limit);
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMITr(unit, limit_enable));
    }

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_ACTIONr(unit, &limit_action));
    /*
     * Need to reset the action bit if the flasg is not set.
     */
    soc_reg_field_set(unit,
                      SYS_MAC_ACTIONr,
                      &limit_action,
                      OVER_LIMIT_DROPf, 
                      (flags & BCM_L2_LEARN_LIMIT_ACTION_DROP) ? 1 : 0);
    soc_reg_field_set(unit,
                      SYS_MAC_ACTIONr,
                      &limit_action, 
                      OVER_LIMIT_TOCPUf,
                      (flags & BCM_L2_LEARN_LIMIT_ACTION_CPU) ? 1 : 0);
    soc_reg_field_set(unit,
                      SYS_MAC_ACTIONr,
                      &limit_action, 
                      MAC_LIMIT_USE_SYS_ACTIONf,
                      (flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) ? 1 : 0);

    BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_ACTIONr(unit, limit_action));


    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_system_get
 * Purpose:
 *      Get MAC learn limit for a system 
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask 
 *      limit - System limit of MAC addresses to learn
 *              Negative if limit disabled  
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_system_get(int unit, uint32 *flags, int *limit)
{
       uint32  limit_enable, limit_action;
       int     org_limit, org_action;

       if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
           return BCM_E_UNAVAIL;
       }

       BCM_IF_ERROR_RETURN(READ_MAC_LIMIT_ENABLEr(unit, &limit_enable));
       org_limit = soc_reg_field_get(unit, MAC_LIMIT_ENABLEr, limit_enable, 
                                     ENABLEf);

       *flags = 0;
       if (!org_limit) {
           *limit = -1;
           return BCM_E_NONE;
       }

       BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMITr(unit, &limit_enable));
       *limit = soc_reg_field_get(unit, SYS_MAC_LIMITr, limit_enable, 
                                  SYS_MAC_LIMITf);

       /*
        *  In case of MAC learn limit was disabled but somehow 
        *  enable bit was set 
        */
       if (*limit > BCM_MAC_LIMIT_MAX) {
           *limit = -1 ;
           return BCM_E_NONE;
       }

       BCM_IF_ERROR_RETURN(READ_SYS_MAC_ACTIONr(unit, &limit_action));
       org_action = soc_reg_field_get(unit, SYS_MAC_ACTIONr, limit_action, 
                                      OVER_LIMIT_DROPf);
       if (org_action) {
           *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
       }
       org_action = soc_reg_field_get(unit, SYS_MAC_ACTIONr, limit_action, 
                                      OVER_LIMIT_TOCPUf);
       if (org_action) {
           *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
       }
       org_action = soc_reg_field_get(unit, SYS_MAC_ACTIONr, limit_action, 
                                      MAC_LIMIT_USE_SYS_ACTIONf);
       if (org_action) {
           *flags |= BCM_L2_LEARN_LIMIT_ACTION_PREFER;
       }

       return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_l2_learn_limit_port_set
 * Purpose:
 *      Set MAC learn limit for a specific port 
 * Parameters:
 *      unit  - BCM unit number
 *      port - Port number
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative if limit disabled  
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_port_set(int unit, bcm_port_t port, uint32 flags,
                             int limit)
{
    int                                 rv = BCM_E_NONE;
    mac_limit_port_map_table_entry_t    entry;
    source_trunk_map_table_entry_t      trunk_entry;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (0 == soc_feature(unit, soc_feature_mac_learn_limit)) {
        return (BCM_E_UNAVAIL);
    }

    SOC_IF_ERROR_RETURN(READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                                     port, &trunk_entry));
    /* Normal port will be encoded as 0 */
    if (soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &trunk_entry,
                                                PORT_TYPEf)) {
        return BCM_E_CONFIG;
    }
    rv = _bcm_l2_learn_limit_porttrunk_set(unit, port, flags, limit);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    soc_mem_lock(unit, MAC_LIMIT_PORT_MAP_TABLEm);
    /* Map port to it's counter */
    rv = READ_MAC_LIMIT_PORT_MAP_TABLEm(unit, MEM_BLOCK_ANY, 
                                        port, &entry);
    if (BCM_E_NONE == rv) {
        soc_MAC_LIMIT_PORT_MAP_TABLEm_field32_set(unit, &entry,
                                                  INDEXf, port);
        rv = WRITE_MAC_LIMIT_PORT_MAP_TABLEm(unit, MEM_BLOCK_ALL, 
                                             port, &entry);
    }
    soc_mem_unlock(unit, MAC_LIMIT_PORT_MAP_TABLEm);

    return rv;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_port_get
 * Purpose:
 *      Get MAC learn limit for a specific port 
 * Parameters:
 *      unit  - BCM unit number
 *      port - Port number
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_port_get(int unit, bcm_port_t port, uint32 *flags,
                             int *limit)
{
    int                                 rv = BCM_E_NONE;
    int                                 hw_index;
    source_trunk_map_table_entry_t      trunk_entry;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (0 == soc_feature(unit, soc_feature_mac_learn_limit)) {
        return (BCM_E_UNAVAIL);
    }
    /* If port is part of a trunk  - Error */
    SOC_IF_ERROR_RETURN(READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                                     port, &trunk_entry));
    /* Normal port will be encoded as 0 */
    if (soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &trunk_entry,
                                                PORT_TYPEf)) {
        return BCM_E_CONFIG;
    }
    hw_index = port;
    rv = _bcm_l2_learn_limit_porttrunk_get(unit, hw_index, flags, limit);
    return rv;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_trunk_set
 * Purpose:
 *      Set MAC learn limit for a specific port 
 * Parameters:
 *      unit  - BCM unit number
 *      trunk - Trunk identifier
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_trunk_set(int unit, bcm_trunk_t trunk, uint32 flags,
                              int limit)
{
    int                                  rv = BCM_E_NONE;
    int                                  hw_index = -1;
    mac_limit_trunk_map_table_entry_t    entry;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (trunk >= soc_mem_index_count(unit, TRUNK_GROUPm) || trunk < 0) {
        return BCM_E_PARAM;
    }

    if (0 == soc_feature(unit, soc_feature_mac_learn_limit)) {
        return (BCM_E_UNAVAIL);
    }
    hw_index = trunk + PORT_COUNT(unit);
    rv = _bcm_l2_learn_limit_porttrunk_set(unit, hw_index, flags, limit);
    soc_mem_lock(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
    /* Map port to it's counter */
    rv = READ_MAC_LIMIT_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, 
                                         trunk, &entry);
    if (BCM_E_NONE == rv) {
        soc_MAC_LIMIT_TRUNK_MAP_TABLEm_field32_set(unit, &entry,
                                                   INDEXf, hw_index);
        rv = WRITE_MAC_LIMIT_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ALL, 
                                              trunk, &entry);
    }
    soc_mem_unlock(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
    return rv;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_trunk_get
 * Purpose:
 *      Get MAC learn limit for a specific port 
 * Parameters:
 *      unit  - BCM unit number
 *      trunk - Trunk identifier
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative if limit disabled 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_trunk_get(int unit, bcm_trunk_t tid, uint32 *flags,
                              int *limit)
{
    int rv = BCM_E_NONE;
    int hw_index = -1;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (tid >= soc_mem_index_count(unit, TRUNK_GROUPm) || tid < 0) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_mac_learn_limit)) {
        hw_index = tid + PORT_COUNT(unit);
        rv = _bcm_l2_learn_limit_porttrunk_get(unit, hw_index, flags, limit);
        return rv;
    } 

    return BCM_E_UNAVAIL;
}
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRIUMPH_SUPPORT */
/*
 * Function:
 *     bcm_esw_l2_learn_limit_set
 * Description:
 *     Set the L2 MAC learning limit
 * Parameters:
 *     unit        device number
 *     limit       learn limit control info
 *                 limit->flags - qualifiers bits and action bits
 *                 limit->vlan - vlan identifier
 *                 limit->port - port number
 *                 limit->trunk - trunk identifier
 *                 limit->limit - max number of learned entry, -1 for unlimit
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_learn_limit_set(int unit, bcm_l2_learn_limit_t *limit)
{
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    uint32 type, action, limit_enable, org_limit=0;

    if ((!soc_feature(unit, soc_feature_mac_learn_limit))) {
        return BCM_E_UNAVAIL;
    }

    if (!limit) {
        return BCM_E_PARAM;
    }

    if (limit->limit > BCM_MAC_LIMIT_MAX){
        return BCM_E_PARAM;
    }

    type = limit->flags &
           (BCM_L2_LEARN_LIMIT_SYSTEM | BCM_L2_LEARN_LIMIT_VLAN | 
            BCM_L2_LEARN_LIMIT_PORT | BCM_L2_LEARN_LIMIT_TRUNK);

    action = limit->flags & 
             (BCM_L2_LEARN_LIMIT_ACTION_DROP | BCM_L2_LEARN_LIMIT_ACTION_CPU |
              BCM_L2_LEARN_LIMIT_ACTION_PREFER);

    if (!type) {
        return BCM_E_PARAM;
    }

    if (type != BCM_L2_LEARN_LIMIT_SYSTEM &&
        (action & BCM_L2_LEARN_LIMIT_ACTION_PREFER)) {
        return BCM_E_PARAM;
    }

    if ((action & BCM_L2_LEARN_LIMIT_ACTION_DROP) && 
        (action & BCM_L2_LEARN_LIMIT_ACTION_CPU)) {
        return BCM_E_PARAM;
    }

    if (type & BCM_L2_LEARN_LIMIT_SYSTEM) {
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_learn_limit_system_set(unit, action,
                                                   limit->limit));
        } else
#endif
        if (soc_feature(unit, soc_feature_system_mac_learn_limit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_l2_learn_limit_system_set(unit, action, limit->limit));
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    if (type & BCM_L2_LEARN_LIMIT_PORT) {
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index;

            if (!SOC_PORT_VALID(unit, limit->port)) {
                return BCM_E_PORT;
            }
            index = limit->port + soc_mem_index_count(unit, TRUNK_GROUPm);
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_learn_limit_set(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                            index, action, limit->limit));
        } else
#endif
        {
            BCM_IF_ERROR_RETURN
                (_bcm_l2_learn_limit_port_set(unit, limit->port, action,
                                              limit->limit));
        }
    }

    if (type & BCM_L2_LEARN_LIMIT_TRUNK) {
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index;

            if (limit->trunk < 0 ||
                limit->trunk >= soc_mem_index_count(unit, TRUNK_GROUPm)) {
                return BCM_E_PARAM;
            }
            index = limit->trunk;
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_learn_limit_set(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                            index, action, limit->limit));
        } else
#endif
        {
            BCM_IF_ERROR_RETURN
                (_bcm_l2_learn_limit_trunk_set(unit, limit->trunk, action,
                                               limit->limit));
        }
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (type & BCM_L2_LEARN_LIMIT_VLAN) {
        if (SOC_IS_TR_VL(unit)) {
            int index, vfi;

            if (_BCM_MPLS_VPN_IS_VPLS(limit->vlan)) {
                vfi = _BCM_MPLS_VPN_ID_GET(limit->vlan);
                if (vfi >= soc_mem_index_count(unit, VFIm)) {
                    return BCM_E_PARAM;
                }
                index = vfi + soc_mem_index_count(unit, VLAN_TABm);
            } else {
                if (limit->vlan >= soc_mem_index_count(unit, VLAN_TABm)) {
                    return BCM_E_PARAM;
                }
                index = limit->vlan;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_learn_limit_set(unit, VLAN_OR_VFI_MAC_LIMITm,
                                            index, action, limit->limit));
        }
    }

    if (SOC_IS_TR_VL(unit)) {
        return BCM_E_NONE;
    }
#endif

    /* Enable the MAC learn limit feature after configuring the indexes */
    /* Negative limit disables the check */
    if (limit->limit < 0) {
        BCM_IF_ERROR_RETURN(READ_MAC_LIMIT_ENABLEr(unit, &limit_enable));
        soc_reg_field_set(unit, MAC_LIMIT_ENABLEr, &limit_enable, 
                          ENABLEf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MAC_LIMIT_ENABLEr(unit, limit_enable));
    } else {
        BCM_IF_ERROR_RETURN(READ_MAC_LIMIT_ENABLEr(unit, &limit_enable));
        org_limit = soc_reg_field_get(unit, MAC_LIMIT_ENABLEr, limit_enable,
                                      ENABLEf);
        if (!org_limit) {
            soc_reg_field_set(unit, MAC_LIMIT_ENABLEr, &limit_enable, 
                              ENABLEf, 1);
            BCM_IF_ERROR_RETURN(WRITE_MAC_LIMIT_ENABLEr(unit, limit_enable));
        }
    }
    return BCM_E_NONE;
#else /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_l2_learn_limit_get
 * Description:
 *     Get the L2 MAC learning limit
 * Parameters:
 *     unit        device number
 *     limit       learn limit control info
 *                 limit->flags - qualifiers bits and action bits
 *                 limit->vlan - vlan identifier
 *                 limit->port - port number
 *                 limit->trunk - trunk identifier
 *                 limit->limit - max number of learned entry, -1 for unlimit
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_learn_limit_get(int unit, bcm_l2_learn_limit_t *limit)
{
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    int rv;
    uint32 type, action;
    int max;

    if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
        return BCM_E_UNAVAIL;
    }

    if (!limit) {
        return BCM_E_PARAM;
    }

    type = limit->flags &
           (BCM_L2_LEARN_LIMIT_SYSTEM | BCM_L2_LEARN_LIMIT_VLAN | 
            BCM_L2_LEARN_LIMIT_PORT | BCM_L2_LEARN_LIMIT_TRUNK);
    action = 0;
    max = 0;

    rv = BCM_E_UNAVAIL;
    switch (type) {
    case BCM_L2_LEARN_LIMIT_SYSTEM:
#ifdef BCM_TRIUMPH_SUPPORT
      if (SOC_IS_TR_VL(unit)) {
            return _bcm_tr_l2_learn_limit_system_get(unit, &limit->flags,
                                                     &limit->limit);
        }
#endif
      if (soc_feature(unit, soc_feature_system_mac_learn_limit)) {
        rv = _bcm_l2_learn_limit_system_get(unit, &action, &max);
      } 
      break;

    case BCM_L2_LEARN_LIMIT_PORT:
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index;

            if (limit->port < 0 ||
                limit->port >= soc_mem_index_count(unit, PORT_TABm)) {
                return BCM_E_PORT;
            }
            index = limit->port + soc_mem_index_count(unit, TRUNK_GROUPm);
            return _bcm_tr_l2_learn_limit_get(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                              index, &limit->flags,
                                              &limit->limit);
        }
#endif
        rv = _bcm_l2_learn_limit_port_get(unit, limit->port, &action, &max);
        break;

    case BCM_L2_LEARN_LIMIT_TRUNK:
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index;

            if (limit->trunk < 0 ||
                limit->trunk >= soc_mem_index_count(unit, TRUNK_GROUPm)) {
                return BCM_E_PARAM;
            }
            index = limit->trunk;
            return _bcm_tr_l2_learn_limit_get(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                              index, &limit->flags,
                                              &limit->limit);
        }
#endif
        rv = _bcm_l2_learn_limit_trunk_get(unit, limit->trunk, &action, &max);
        break;

    case BCM_L2_LEARN_LIMIT_VLAN:
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index, vfi;

            if (_BCM_MPLS_VPN_IS_VPLS(limit->vlan)) {
                vfi = _BCM_MPLS_VPN_ID_GET(limit->vlan);
                if (vfi >= soc_mem_index_count(unit, VFIm)) {
                    return BCM_E_PARAM;
                }
                index = vfi + soc_mem_index_count(unit, VLAN_TABm);
            } else {
                if (limit->vlan >= soc_mem_index_count(unit, VLAN_TABm)) {
                    return BCM_E_PARAM;
                }
                index = limit->vlan;
            }
            return _bcm_tr_l2_learn_limit_get(unit, VLAN_OR_VFI_MAC_LIMITm,
                                              index, &limit->flags,
                                              &limit->limit);
        }
#endif
        break;

    default:
        return BCM_E_PARAM;
    }

    if (rv == BCM_E_NONE) {
        limit->flags |= action;
        limit->limit = max;
    }
    return rv;
#else /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_l2_learn_class_set
 * Description:
 *     To set the attributes of L2 learning class
 * Parameters:
 *     unit        device number
 *     lclass      learning class id (0 - 3)
 *     lclass_prio learning class priority (0 - 3, 3 is the highest)
 *     flags       flags (BCM_L2_LEARN_CLASS_*)
 * Return:
 *     BCM_E_XXX
 * Note:
 *     This feature is disabled by default (see bcmPortControlLearnClassEnable).
 *     By default, each learning class is assigned priority 0 (lowest)
 *     and the attribute, BCM_L2_LEARN_CLASS_MOVE, is not set. By default,
 *     each port is assigned to learning class 0.
 */
int
bcm_esw_l2_learn_class_set(int unit, int lclass, int lclass_prio, uint32 flags)
{
#if defined(BCM_TRX_SUPPORT)
    uint32 data;

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return (BCM_E_UNAVAIL);
    }

    if (lclass < 0 || lclass >= SOC_REG_NUMELS(unit, CBL_ATTRIBUTEr) ||
        lclass_prio < 0 ||
        lclass_prio >= (1 << soc_reg_field_length(unit, CBL_ATTRIBUTEr,
                                                  PORT_LEARNING_PRIORITYf))) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_CBL_ATTRIBUTEr(unit, lclass, &data));
    soc_reg_field_set(unit, CBL_ATTRIBUTEr, &data, PORT_LEARNING_PRIORITYf,
                      lclass_prio);
    soc_reg_field_set(unit, CBL_ATTRIBUTEr, &data, ALLOW_MOVE_IN_CLASSf,
                      flags & BCM_L2_LEARN_CLASS_MOVE ? 1 : 0);
    BCM_IF_ERROR_RETURN(WRITE_CBL_ATTRIBUTEr(unit, lclass, data));

    return SOC_E_NONE;
#else /* BCM_TRIUMPH_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRIUMPH_SUPPORT */
}
/*
 * Function:
 *     bcm_esw_l2_learn_class_get
 * Description:
 *     To get the attributes of L2 learning class
 * Parameters:
 *     unit        device number
 *     lclass      learning class id (0 - 3)
 *     lclass_prio learning class priority (0 - 3, 3 is the highest)
 *     flags       flags (BCM_L2_LEARN_CLASS_*)
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_learn_class_get(int unit, int lclass, int *lclass_prio, uint32 *flags)
{
#if defined(BCM_TRX_SUPPORT)
    uint32 data;

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return (BCM_E_UNAVAIL);
    }

    if (lclass < 0 || lclass >= SOC_REG_NUMELS(unit, CBL_ATTRIBUTEr) ||
        lclass_prio == NULL || flags == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_CBL_ATTRIBUTEr(unit, lclass, &data));
    *lclass_prio = soc_reg_field_get(unit, CBL_ATTRIBUTEr, data,
                                     PORT_LEARNING_PRIORITYf);
    *flags = 0;
    if (soc_reg_field_get(unit, CBL_ATTRIBUTEr, data, ALLOW_MOVE_IN_CLASSf)) {
        *flags |= BCM_L2_LEARN_CLASS_MOVE;
    }

    return (BCM_E_NONE);
#else /* BCM_TRX_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_l2_learn_port_class_set
 * Description:
 *     To set the L2 learning class of a port
 * Parameters:
 *     unit        device number
 *     port        Ingress generic port
 *     lclass      learning class id (0 - 3)
 * Return:
 *     BCM_E_XXX
 * Note:
 *     This feature is disabled by default (see bcmPortControlLearnClassEnable).
 *     By default, each port is assigned to learning class 0.
 */
int
bcm_esw_l2_learn_port_class_set(int unit, bcm_gport_t port, int lclass)
{
#if defined(BCM_TRX_SUPPORT)
    int gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    BCM_L2_LEARN_CLASSID_VALID(lclass);

    if (BCM_GPORT_IS_TRUNK(port)) {
        trunk_cbl_table_entry_t tr_entry;
        sal_memset(&tr_entry, 0, sizeof(tr_entry));
        soc_TRUNK_CBL_TABLEm_field32_set(unit, &tr_entry,
                                         PORT_LEARNING_CLASSf, lclass);
        BCM_IF_ERROR_RETURN(WRITE_TRUNK_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                                  trunk_id, &tr_entry));
    } else if (BCM_GPORT_IS_MODPORT(port)) {
        port_cbl_table_entry_t entry;

        sal_memset(&entry, 0, sizeof(entry));
        soc_PORT_CBL_TABLEm_field32_set(unit, &entry, PORT_LEARNING_CLASSf,
                                        lclass);
        BCM_IF_ERROR_RETURN(WRITE_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                                  (mod_out << 6) | port_out,
                                                  &entry));
    } else if (BCM_GPORT_IS_MPLS_PORT(port)) {
        port_cbl_table_entry_t entry;

        sal_memset(&entry, 0, sizeof(entry));
        soc_PORT_CBL_TABLEm_field32_set(unit, &entry, 
                                        VIRTUAL_PORT_LEARNING_CLASSf, lclass);
        BCM_IF_ERROR_RETURN
            (WRITE_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                   BCM_GPORT_MPLS_PORT_ID_GET(port), &entry));
    } else if (BCM_GPORT_IS_MIM_PORT(port)) {
        port_cbl_table_entry_t entry;

        sal_memset(&entry, 0, sizeof(entry));
        soc_PORT_CBL_TABLEm_field32_set(unit, &entry, 
                                        VIRTUAL_PORT_LEARNING_CLASSf, lclass);
        BCM_IF_ERROR_RETURN
            (WRITE_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                   BCM_GPORT_MIM_PORT_ID_GET(port), &entry));
    }  else if (BCM_GPORT_IS_WLAN_PORT(port)) {
        port_cbl_table_entry_t entry;

        sal_memset(&entry, 0, sizeof(entry));
        soc_PORT_CBL_TABLEm_field32_set(unit, &entry, 
                                        VIRTUAL_PORT_LEARNING_CLASSf, lclass);
        BCM_IF_ERROR_RETURN
            (WRITE_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                   BCM_GPORT_WLAN_PORT_ID_GET(port), &entry));
    }

    return (BCM_E_NONE);
#else /* BCM_TRX_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_l2_learn_port_class_get
 * Description:
 *     To get the L2 learning class of a port
 * Parameters:
 *     unit        device number
 *     port        Ingress generic port
 *     lclass      learning class id (0 - 3)
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_learn_port_class_get(int unit, bcm_gport_t port, int *lclass)
{
#if defined(BCM_TRX_SUPPORT)
    int gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (lclass == NULL) {
        return (BCM_E_PARAM);
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        trunk_cbl_table_entry_t tr_entry;
        BCM_IF_ERROR_RETURN(READ_TRUNK_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                                  trunk_id, &tr_entry));
        *lclass = soc_TRUNK_CBL_TABLEm_field32_get(unit, &tr_entry,
                                                   PORT_LEARNING_CLASSf);
    } else if (BCM_GPORT_IS_MODPORT(port)) {
        port_cbl_table_entry_t entry;

        BCM_IF_ERROR_RETURN
            (READ_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                  (mod_out << 6) | port_out, &entry));
        *lclass = soc_PORT_CBL_TABLEm_field32_get(unit, &entry,
                                                  PORT_LEARNING_CLASSf);
    } else if (BCM_GPORT_IS_MPLS_PORT(port)) {
        port_cbl_table_entry_t entry;

        BCM_IF_ERROR_RETURN
            (READ_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                  BCM_GPORT_MPLS_PORT_ID_GET(port), &entry));
        *lclass = soc_PORT_CBL_TABLEm_field32_get(unit, &entry,
                                                  VIRTUAL_PORT_LEARNING_CLASSf);
    } else if (BCM_GPORT_IS_MIM_PORT(port)) {
        port_cbl_table_entry_t entry;

        BCM_IF_ERROR_RETURN
            (READ_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                  BCM_GPORT_MIM_PORT_ID_GET(port), &entry));
        *lclass = soc_PORT_CBL_TABLEm_field32_get(unit, &entry,
                                                  VIRTUAL_PORT_LEARNING_CLASSf);
    } else if (BCM_GPORT_IS_WLAN_PORT(port)) {
        port_cbl_table_entry_t entry;

        BCM_IF_ERROR_RETURN
            (READ_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                  BCM_GPORT_WLAN_PORT_ID_GET(port), &entry));
        *lclass = soc_PORT_CBL_TABLEm_field32_get(unit, &entry,
                                                  VIRTUAL_PORT_LEARNING_CLASSf);
    }

    return (BCM_E_NONE);
#else /* BCM_TRX_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRX_SUPPORT */
}

/*
 * Function:
 *     _bcm_esw_l2_entry_valid
 * Description:
 *      Check if given L2 entry is Valid
 * Parameters:
 *      unit         device number
 *      mem         L2 memory to operate on
 *      l2_entry    L2X entry read from HW
 *      valid       (OUT) Entry valid indicator
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_esw_l2_entry_valid(int unit, soc_mem_t mem, uint32 *l2_entry, int *valid)
{
    uint32  fval = 0;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    if ((NULL == l2_entry) || (NULL == valid)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        fval = soc_mem_field32_get(unit, mem, l2_entry, VALID_BITf);
    } 
#endif
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)){
        fval = soc_mem_field32_get(unit, mem, l2_entry, VALIDf);
    }
#endif
#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
        fval = soc_mem_field32_get(unit, mem, l2_entry, VALID_BITf);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit) && fval) {
        int key_type;
        key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPEf);
        /* Skip cross connect entries */
        if ((key_type == TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT) ||
            (key_type == TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT)) {
            fval = 0;
        }
    }
#endif
    *valid = fval ? TRUE : FALSE;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_esw_l2_from_l2x
 * Description:
 *      Parser function to parse L2X entry into L2_Addr structure
 *      will call per chip parser
 * Parameters:
 *      unit         device number
 *      mem         L2 memory to operate on
 *      l2addr      L2 address structure to fill
 *      l2_entry    L2X entry read from HW
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_esw_l2_from_l2x(int unit, soc_mem_t mem, bcm_l2_addr_t *l2addr, uint32 *l2_entry)
{
#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        bcm_pbmp_t  pbm;

        return _bcm_er_l2_from_l2x(unit, l2addr,
                                (l2_entry_internal_entry_t *)l2_entry, &pbm);
    } else
#endif
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        if (EXT_L2_ENTRYm == mem) { 
            return _bcm_tr_l2_from_ext_l2(unit, l2addr, 
                                        (ext_l2_entry_entry_t *)l2_entry);
        } else { 
            return _bcm_tr_l2_from_l2x(unit, l2addr, 
                                       (l2x_entry_t *)l2_entry);
        }
    } else
#endif 
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_l2_from_l2x(unit, l2addr, 
                                   (l2x_entry_t *)l2_entry);
    } else
#endif
#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
        return _bcm_l2_from_l2x(unit, l2addr, 
                                (l2x_entry_t *)l2_entry);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _bcm_esw_l2_traverse_mem
 * Description:
 *      Helper function to _bcm_esw_l2_traverse to itterate over given memory 
 *      and actually read the table and parse entries.
 * Parameters:
 *     unit         device number
 *      mem         L2 memory to read
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_esw_l2_traverse_mem(int unit, soc_mem_t mem, _bcm_l2_traverse_t *trav_st)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, valid, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32          *l2_tbl_chnk;
    uint32          *l2_entry;
    int             rv = BCM_E_NONE;
    
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_tbl_chnk = soc_cm_salloc(unit, buf_size, "l2 traverse");
    if (NULL == l2_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            chnk_idx + chunksize : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx < chnk_end; ent_idx ++) {
            l2_entry = 
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                                             l2_tbl_chnk, ent_idx);
            rv = _bcm_esw_l2_entry_valid(unit, mem, l2_entry, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }

            if (FALSE == valid) {
                continue;
            }
            trav_st->data = l2_entry;
            trav_st->mem = mem;
            trav_st->mem_idx = chnk_idx + ent_idx;
            rv = trav_st->int_cb(unit, (void *)trav_st);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_tbl_chnk);
    return rv;        
}

/*
 * Function:
 *     _bcm_esw_l2_traverse
 * Description:
 *      Helper function to bcm_esw_l2_traverse to itterate over table 
 *      and actually read the momery
 * Parameters:
 *     unit         device number
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_esw_l2_traverse(int unit, _bcm_l2_traverse_t *trav_st)
{
    int rv = BCM_E_UNAVAIL; 

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        rv = _bcm_esw_l2_traverse_mem(unit, L2_ENTRY_EXTERNALm, trav_st);
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_esw_l2_traverse_mem(unit, L2_ENTRY_INTERNALm, trav_st);
        }
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_esw_l2_traverse_mem(unit, L2_ENTRY_OVERFLOWm, trav_st);
        }
    }
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        rv = _bcm_esw_l2_traverse_mem(unit, L2Xm, trav_st);
#if defined(BCM_TRIUMPH_SUPPORT)
        if (soc_feature(unit, soc_feature_esm_support) && BCM_SUCCESS(rv)) {
            rv = _bcm_tr_l2_traverse_mem(unit, EXT_L2_ENTRYm, trav_st);
        }
#endif  /* BCM_TRIUMPH_SUPPORT */
    }
#endif  /* BCM_FBX_SUPPORT */

#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
        if (soc_mem_is_valid(unit, L2Xm)) {
            rv = _bcm_esw_l2_traverse_mem(unit, L2Xm, trav_st);
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *     _bcm_esw_l2_traverse_int_cb
 * Description:
 *      Simple internal callback function for bcm_l2_traverse API just to call
 *      a user provided callback on a given entry.
 * Parameters:
 *      unit         device number
 *      trav_st      traverse structure
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_esw_l2_traverse_int_cb(int unit, void *trav_st)
{
    _bcm_l2_traverse_t *trv;
    bcm_l2_addr_t      l2_addr; 

    trv = (_bcm_l2_traverse_t *)trav_st;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_l2_from_l2x(unit, trv->mem, &l2_addr, trv->data));

    return trv->user_cb(unit, &l2_addr, trv->user_data);
}

/*
 * Function:
 *     bcm_esw_l2_traverse
 * Description:
 *     To traverse the L2 table and call provided callback function with matched entry
 * Parameters:
 *     unit         device number
 *     trav_fn      User specified callback function 
 *     user_data    User specified cookie
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_esw_l2_traverse(int unit, bcm_l2_traverse_cb trav_fn, void *user_data)
{
    _bcm_l2_traverse_t  trav_st;
    
    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trav_st, 0, sizeof(_bcm_l2_traverse_t));
    
    trav_st.user_cb = trav_fn;
    trav_st.user_data = user_data;
    trav_st.int_cb = _bcm_esw_l2_traverse_int_cb;
    return (_bcm_esw_l2_traverse(unit, &trav_st));
    
}

#ifdef BCM_XGS_SWITCH_SUPPORT
/*
 * Function:
 *     _bcm_get_op_from_flags
 * Description:
 *     Helper function to _bcm_fb_age_reg_config to 
 *     decide on a PPA command according to flags
 * Parameters:
 *     flags        flags BCM_L2_REPLACE_* 
 *     op           PPA command to be programmed
 *     sync_op      Search Key for _soc_l2x_sync_delete_by
 * Return:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Failure
 */
int 
_bcm_get_op_from_flags(uint32 flags, uint32 *op, uint32 *sync_op)
{
    uint32 cmp_flags = 0, int_op = 0, int_sync_op = 0;
    int    rv = BCM_E_NONE;

    if (NULL == op) {
        return BCM_E_PARAM;
    }
    cmp_flags = flags & _BCM_L2_REPLACE_MATCH_VLAN_DEST;
    switch (cmp_flags) {
        case BCM_L2_REPLACE_MATCH_VLAN: 
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_VLAN : 
                   XGS_PPAMODE_VLAN_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_VLAN_DEL : 
                           SOC_L2X_NO_DEL);
         break;
        }
        case BCM_L2_REPLACE_MATCH_DEST :
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_PORTMOD :
                   XGS_PPAMODE_PORTMOD_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_PORTMOD_DEL : 
                           SOC_L2X_NO_DEL);
            break;
        }
        case _BCM_L2_REPLACE_MATCH_VLAN_DEST:
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_PORTMOD_VLAN :
                   XGS_PPAMODE_PORTMOD_VLAN_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_PORTMOD_VLAN_DEL : 
                           SOC_L2X_NO_DEL);

            break;
        }
        default:
            rv = BCM_E_PARAM;
            break;
    }

    *op = int_op;
    *sync_op = int_sync_op;
    return rv;
}

#endif /* BCM_XGS_SWITCH_SUPPORT */


int 
_bcm_l2_entry_replace_by_mac(int unit, soc_mem_t mem, uint32 *entry, int idx, uint32 flags,
                                _bcm_l2_replace_t *rep_st)
{
    uint32      static_bit = 0;
    uint32      pending_bit = 0;
    uint32      valid_bit_f = INVALIDf;
    uint32      port_f = INVALIDf;
    bcm_mac_t   l2mac;
    uint32      mod_val, port_val;
    
    if (NULL == rep_st || NULL == entry || INVALIDm == mem) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_L2_REPLACE_MATCH_STATIC) {
        static_bit = 1;
    }
    if (flags & BCM_L2_REPLACE_PENDING) {
        pending_bit = 1;
    }

    soc_mem_mac_addr_get(unit, mem, entry, MAC_ADDRf, l2mac);
    /* If MAC is not equal do nothing */
    if (ENET_CMP_MACADDR(rep_st->match_mac, l2mac)) {
        return (BCM_E_NONE);
    }
    /* if match_port is valid then we should match port modid as well */
    if (-1 != rep_st->match_port) {
        /* Draco Style device */
        if (soc_mem_field_valid(unit,mem, TGID_PORTf)) {
            port_val = soc_mem_field32_get(unit, mem, entry, TGID_PORTf);
        } else {    /* Triumph style device */
            port_val = soc_mem_field32_get(unit, mem, entry, PORT_NUMf);
        }
        if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            if (soc_mem_field32_get(unit, mem, entry, Tf)) {
                /* If trunk member - do nothing */
                return (BCM_E_NONE);
            }
        } else {
            if (port_val & BCM_TGID_TRUNK_INDICATOR(unit)) {
                /* If trunk member - do nothing */ 
                return (BCM_E_NONE);
            }
        }
        mod_val = soc_mem_field32_get(unit, mem, entry, MODULE_IDf);
        if ((rep_st->match_port != port_val) 
            || (rep_st->match_module != mod_val)) {
            /* if port or module not equal do nothng */
            return (BCM_E_NONE);
        }
    }

    /* BCM_L2_REPLACE_MATCH_STATIC means replace only static entries */
    if (static_bit != soc_mem_field32_get(unit, mem, entry, STATIC_BITf)) {
        return (BCM_E_NONE);
    }
    /* BCM_L2_REPLACE_PENDING means replace only pending entries */
    if (soc_mem_field_valid(unit, mem, PENDINGf) ) {
        if (pending_bit != soc_mem_field32_get(unit, mem, entry, PENDINGf)) {
            return (BCM_E_NONE);
        }
    }
    if (flags & BCM_L2_REPLACE_DELETE) {
        valid_bit_f = soc_mem_field_valid(unit, mem, VALIDf) ? VALIDf : VALID_BITf;
        soc_mem_field32_set(unit, mem, entry, valid_bit_f, 0);
    } else {
        /* New destination is trunk */
        if (flags & BCM_L2_REPLACE_NEW_TRUNK) {
            /* Draco Style device */
            if (soc_mem_field_valid(unit,mem, PORT_TGIDf)) {
                mod_val = BCM_TRUNK_TO_MODIDf(unit, rep_st->new_trunk);
                port_val = BCM_TRUNK_TO_TGIDf(unit, rep_st->new_trunk);
                soc_mem_field32_set(unit, mem, entry, MODULE_IDf, mod_val);
                soc_mem_field32_set(unit, mem, entry, PORT_TGIDf, port_val);
            } else {    /* Triumph style device */
                soc_mem_field32_set(unit, mem, entry, Tf, 1);
                soc_mem_field32_set(unit, mem, entry, TGIDf, rep_st->new_trunk);
            }
        } else { /* New destination is port + modid pair */
            mod_val = rep_st->new_module;
            port_val = rep_st->new_port;
            port_f = soc_mem_field_valid(unit, mem, PORT_NUMf) ? PORT_NUMf : PORT_TGIDf;
            soc_mem_field32_set(unit, mem, entry, MODULE_IDf, mod_val);
            soc_mem_field32_set(unit, mem, entry, port_f, port_val);
            if (soc_mem_field_valid(unit,mem, Tf)) {
                soc_mem_field32_set(unit, mem, entry, Tf, 0);
            } 
        }
    }
#ifdef BCM_TRIUMPH_SUPPORT
    if (EXT_L2_ENTRYm == mem) {
        int rv;
        if (flags & BCM_L2_REPLACE_DELETE) {
            BCM_IF_ERROR_RETURN(
                soc_mem_generic_delete(unit, mem, MEM_BLOCK_ANY, 0,
                                       entry, entry, NULL));
            /* learn limit counters should not be updated for static entries */
            if (static_bit) {
                rv = BCM_E_NONE;
            } else {
                rv = soc_triumph_learn_count_update(unit, (void *)entry,
                                                    TRUE, -1);
            }
        } else {
            rv = soc_mem_generic_insert(unit, mem, MEM_BLOCK_ALL, 0,
                                        entry, entry, 0);
            if (rv == BCM_E_EXISTS) {
                rv = BCM_E_NONE;
            }
        }
        return rv;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit) && (L2Xm == mem)) {
        if (flags & BCM_L2_REPLACE_DELETE) {
            return soc_l2x_delete(unit, (l2x_entry_t *)entry);
        } else {
            return soc_l2x_insert(unit, (l2x_entry_t *)entry);
        } 
    } 
#endif

    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, entry);
}

/*
 * Function:
 *     _bcm_l2_addr_replace_by_mac
 * Description:
 *     Helper function to bcm_l2_replace API to be used as a call back for 
 *     bcm_l2_traverse API and replace given entry if MAC matches
 * Parameters:
 *     unit         device number
 *     trav_st      structure with information from traverse function 
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_l2_addr_replace_by_mac(int unit, void *trav_st)
{
    _bcm_l2_traverse_t  *trv;
    _bcm_l2_replace_t   *rep_st;
    
    trv = (_bcm_l2_traverse_t *)trav_st;
    rep_st = (_bcm_l2_replace_t *)trv->user_data;

    /* Old devices before Draco1.5 should not support replace */
    if (SOC_IS_XGS12_SWITCH(unit) && !SOC_IS_DRACO15(unit) && 
        (0 == (rep_st->flags & BCM_L2_REPLACE_DELETE))) {
        return (BCM_E_UNAVAIL);
    }
    return _bcm_l2_entry_replace_by_mac(unit, trv->mem, trv->data, 
                                        trv->mem_idx, rep_st->flags, rep_st);
}

/*
 * Function:
 *     _bcm_l2_addr_replace_by_vlan_dest
 * Description:
 *     Helper function to bcm_l2_replace API to replace l2 entries by vlan 
 *      or modid and port or trunk
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace 
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_l2_addr_replace_by_vlan_dest(int unit, uint32 flags, _bcm_l2_replace_t *rep_st)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return _bcm_tr_l2_addr_replace_by_vlan_dest(unit, flags, rep_st);
    }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_l2_addr_replace_by_vlan_dest(unit, flags, rep_st);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_l2_addr_replace_by_vlan_dest(unit, flags, rep_st);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        return _bcm_draco_l2_addr_replace_by_vlan_dest(unit, flags, rep_st);
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */
    
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _bcm_l2_replace_flags_validate
 * Description:
 *     THelper function to bcm_l2_replace API to validate given flags
 * Parameters:
 *     flags        flags BCM_L2_REPLACE_* 
 * Return:
 *     BCM_E_NONE - OK 
 *     BCM_E_PARAM - Failure
 */
int 
_bcm_l2_replace_flags_validate(uint32 flags)
{
    if (!flags) {
        return BCM_E_PARAM;
    }
    if ((flags & BCM_L2_REPLACE_DELETE) && (flags & BCM_L2_REPLACE_NEW_TRUNK)) {
        return BCM_E_PARAM;
    }
    if ((flags > (_BCM_L2_REPLACE_MATCH_MAC_VLAN_DEST | BCM_L2_REPLACE_MATCH_STATIC)) && 
        (flags < BCM_L2_REPLACE_DELETE)) {
        return BCM_E_PARAM;
    }
    if (flags > _BCM_L2_REPLACE_ALL_LEGAL) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_l2_replace_dest_setup
 * Description:
 *     Helper function to bcm_l2_replace API to setup a destination to replace
 * Parameters:
 *      unit        - device number
 *      match_addr  - L2 Address to match on 
 *      rep_st      - Replace structure to fill         
 * Return:
 *     BCM_E_NONE - OK 
 *     BCM_E_PARAM - Failure
 */
int 
_bcm_l2_replace_dest_setup(int unit, bcm_l2_addr_t *match_addr, 
                           _bcm_l2_replace_t *rep_st)
{
    if (BCM_GPORT_IS_SET(match_addr->port)) {
        if (BCM_GPORT_IS_MPLS_PORT(match_addr->port)){
              rep_st->isMpls = 1;
              rep_st->mpls_vp = BCM_GPORT_MPLS_PORT_ID_GET(match_addr->port);
        } else {
              BCM_IF_ERROR_RETURN(
                   _bcm_esw_gport_resolve(unit, match_addr->port, 
                                   &(rep_st->match_module),
                                   &(rep_st->match_port),
                                   &(rep_st->match_trunk),
                                   &(rep_st->mpls_vp)));
              if (-1 != rep_st->match_trunk) {
                   rep_st->isTrunk = 1;
              } else if (-1 != rep_st->mpls_vp) { /* Not supported for Non-mpls Gports */
                   return BCM_E_PORT;
              }
        }
    } else {
        if (match_addr->flags & BCM_L2_TRUNK_MEMBER) {
            BCM_IF_ERROR_RETURN(
                _bcm_trunk_id_validate(unit, match_addr->tgid));
            rep_st->isTrunk = 1;
            rep_st->match_trunk = match_addr->tgid;
        } else {
            if (match_addr->modid < 0) { /* use local modid with no mapping */
                if (!SOC_PORT_VALID(unit, match_addr->port)) {
                    return BCM_E_PORT;
                }
                rep_st->match_port = match_addr->port;
                BCM_IF_ERROR_RETURN(
                    bcm_esw_stk_my_modid_get(unit, &(rep_st->match_module)));
            } else { /* map module/port */
                PORT_DUALMODID_VALID(unit, match_addr->port);
                BCM_IF_ERROR_RETURN(
                    bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                           match_addr->modid, match_addr->port,
                                           &(rep_st->match_module), 
                                           &(rep_st->match_port)));
                if (!SOC_MODID_ADDRESSABLE(unit, rep_st->match_module)) {
                    return BCM_E_BADID;
                }
                if (!SOC_PORT_ADDRESSABLE(unit, rep_st->match_port)) {
                    return BCM_E_PORT;
                }
            }
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_esw_l2_replace
 * Description:
 *     To replace destination (or delete) multiple L2 entries
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     match_addr   L2 parameters to match on delete/replace
 *     new_module   new module ID for a replace 
 *     new_port     new port for a replace
 *     new_trunk    new trunk for a replace  
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_esw_l2_replace(int unit, uint32 flags, bcm_l2_addr_t *match_addr,
                   bcm_module_t new_module, bcm_port_t new_port, 
                   bcm_trunk_t new_trunk)
{
    uint32              cmp_flags;
    _bcm_l2_replace_t   rep_st;
    _bcm_l2_traverse_t  trav_st;
    int                 rv = BCM_E_UNAVAIL;

    L2_INIT(unit);

    if (NULL == match_addr) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_l2_replace_flags_validate(flags));

    sal_memset(&rep_st, 0, sizeof(_bcm_l2_replace_t));
    sal_memset(&trav_st, 0, sizeof(_bcm_l2_traverse_t));

    if (0 == (flags & BCM_L2_REPLACE_DELETE)) {
        if (BCM_GPORT_IS_SET(new_port)) {
            bcm_port_t  tmp_port;
            int         tmp_id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, new_port, &new_module, &tmp_port,
                                       &new_trunk, &tmp_id));
            new_port = tmp_port;
        } 
        if (flags & BCM_L2_REPLACE_NEW_TRUNK) { 
            BCM_IF_ERROR_RETURN(
                _bcm_trunk_id_validate(unit, new_trunk));
        } else {
            if (!SOC_MODID_ADDRESSABLE(unit, new_module)) {
                return BCM_E_PARAM;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, new_port)) {
                return BCM_E_PORT;
            }
        }
        rep_st.new_module = new_module;
        rep_st.new_port = new_port;
        rep_st.new_trunk = new_trunk;
    }

    cmp_flags = flags & _BCM_L2_REPLACE_MATCH_MAC_VLAN_DEST;

    BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_freeze(unit));

    switch (cmp_flags) {
        case BCM_L2_REPLACE_MATCH_MAC: 
        {
            sal_memcpy(&(rep_st.match_mac), match_addr->mac, sizeof(bcm_mac_t));
            rep_st.flags = flags;
            rep_st.match_port = -1; /* Match only MAC don't match port */
            rep_st.match_vid = -1; /* Match only MAC don't match vid */
            
            trav_st.user_data = (void *)&rep_st;
            trav_st.int_cb = _bcm_l2_addr_replace_by_mac;
            rv = _bcm_esw_l2_traverse(unit, &trav_st);
            break;
        }
        case _BCM_L2_REPLACE_MATCH_MAC_DEST: 
        {
            rv = _bcm_l2_replace_dest_setup(unit, match_addr, &rep_st);
            if (BCM_FAILURE(rv)) {
                break;
            }
            sal_memcpy(&(rep_st.match_mac), match_addr->mac, sizeof(bcm_mac_t));
            rep_st.flags = flags;

            trav_st.user_data = (void *)&rep_st;
            trav_st.int_cb = _bcm_l2_addr_replace_by_mac;
            rv = _bcm_esw_l2_traverse(unit, &trav_st);
            break;
        }
        case BCM_L2_REPLACE_MATCH_VLAN: 
        {
            rep_st.match_vid = match_addr->vid;
            rv = _bcm_l2_addr_replace_by_vlan_dest(unit, flags, &rep_st);
            break;
        }
        case BCM_L2_REPLACE_MATCH_DEST: 
        {
            rv = _bcm_l2_replace_dest_setup(unit, match_addr, &rep_st);
            if (BCM_FAILURE(rv)) {
                break;
            }
            rv = _bcm_l2_addr_replace_by_vlan_dest(unit, flags, &rep_st);
            break;
        }
        case _BCM_L2_REPLACE_MATCH_VLAN_DEST:
        {
            rv = _bcm_l2_replace_dest_setup(unit, match_addr, &rep_st);
            if (BCM_FAILURE(rv)) {
                break;
            }
            rep_st.match_vid = match_addr->vid;
            rv = _bcm_l2_addr_replace_by_vlan_dest(unit, flags, &rep_st);
            break;
        }
        default:
            break;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_thaw(unit));

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP

/*
 * Function:
 *     _bcm_l2_sw_dump
 * Purpose:
 *     Displays L2 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 * Note:
 *    The l2_data structure maintains registration of callbacks routines.
 *    Dump of l2_data is skipped.
 */
void
_bcm_l2_sw_dump(int unit)
{

    soc_cm_print("\nSW Information L2 - Unit %d\n", unit);
    soc_cm_print("  Initialized : %d\n", _l2_init[unit]);

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        _bcm_tr_l2_sw_dump(unit); 
    }
#endif

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FB_FX_HX(unit)) {
        _bcm_fb_l2_sw_dump(unit);
    }
#endif

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
	_bcm_er_l2_sw_dump(unit);
    }
#endif 
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */


