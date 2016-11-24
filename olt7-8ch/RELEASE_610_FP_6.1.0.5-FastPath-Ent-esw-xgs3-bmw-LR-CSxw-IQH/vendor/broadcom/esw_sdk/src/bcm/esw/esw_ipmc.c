/*
 * $Id: esw_ipmc.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * IP Multicast API
 */

#ifdef INCLUDE_L3

#include <soc/drv.h>
#include <soc/mem.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm/error.h>
#include <bcm/ipmc.h>

#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/proxy.h>
#include <bcm_int/esw/draco.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_EASYRIDER_SUPPORT)
#include <bcm_int/esw/easyrider.h>
#endif /* BCM_EASYRIDER_SUPPORT */
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <bcm_int/esw/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>

#define IPMC_INIT(unit) \
	if (!soc_feature(unit, soc_feature_ip_mcast)) { \
	    return BCM_E_UNAVAIL; \
	} else if (!_bcm_ipmc_init[unit]) { return BCM_E_INIT; }

#define	IPMC_REPL_UNIT(unit) \
	if (!soc_feature(unit, soc_feature_ip_mcast_repl)) \
        { return BCM_E_UNAVAIL; }

#define IPMC_DEBUG(flags, stuff) BCM_DEBUG(flags | BCM_DBG_IPMC, stuff)
#define IPMC_OUT(stuff) BCM_DEBUG(BCM_DBG_IPMC, stuff)
#define IPMC_WARN(stuff) IPMC_DEBUG(BCM_DBG_WARN, stuff)
#define IPMC_ERR(stuff) IPMC_DEBUG(BCM_DBG_ERR, stuff)
#define IPMC_VERB(stuff) IPMC_DEBUG(BCM_DBG_VERBOSE, stuff)

static int _bcm_ipmc_init[BCM_MAX_NUM_UNITS];

/* Forward declaration */
#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_esw_ipmc_reinit(int unit)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if ((SOC_IS_FB_FX_HX(unit) && !SOC_IS_RAPTOR(unit)) || 
        SOC_IS_EASYRIDER(unit) || SOC_IS_TR_VL(unit)) {
        if ((rv = _bcm_xgs3_ipmc_reinit(unit)) >= 0) {
            _bcm_ipmc_init[unit] = TRUE;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return rv;
}
#else
#define    _bcm_esw_ipmc_reinit(unit)     (BCM_E_UNAVAIL)
#endif /* BCM_WARM_BOOT_SUPPORT */



/*
 * Function:
 *	    _bcm_esw_ipmc_gport_construct
 * Description:
 *	    Constructs gport from port, modid or trunk given in the data structure
 *	    if bcmSwitchUseGport is set. 
 * Parameters:
 *	    unit - StrataSwitch PCI device unit number (driver internal).
 *      data - (IN/OUT) data structure to use in order to construct a gport
 * Returns:
 *	    BCM_E_XXX
 */

STATIC int 
_bcm_esw_ipmc_gport_construct(int unit, bcm_ipmc_addr_t *data)
{
    int                 isGport, rv;
    bcm_gport_t         gport;
    _bcm_gport_dest_t   dest;
    bcm_module_t        mymodid;

    if (NULL == data) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));

    /* if output should not be gport then do nothing*/
    if (!isGport) {
        return BCM_E_NONE;
    }

    _bcm_gport_dest_t_init(&dest);

    if (data->ts) {
        dest.tgid = data->port_tgid;
        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
    } else {
        /* Stacking ports should be encoded as devport */
        if (IS_ST_PORT(unit, data->port_tgid)) {
            rv = bcm_esw_stk_my_modid_get(unit, &mymodid);
            if (BCM_E_UNAVAIL == rv) {
                dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
            } else {
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                dest.modid = data->mod_id;
            }
        } else {
            dest.modid = data->mod_id;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        }
        dest.port = data->port_tgid;
    }
    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_construct(unit, &dest, &gport));

    data->port_tgid = gport;
    return BCM_E_NONE;
}


/*
 * Function:
 *	    _bcm_esw_ipmc_gport_resolve
 * Description:
 *	    Resolve port, modid or trunk from given gport. 
 * Parameters:
 *	    unit - StrataSwitch PCI device unit number (driver internal).
 *      gport - given gport
 *      port  - (Out) physical port encoded in gport
 *      modid - (Out) module id encoded in gport 
 *      tgid  - (Out) Trunk group ID encoded in gport
 *      tbit  - (Out) Trunk indicator 1 if trunk encoded in gport 0 otherwise
 *      isPortLocal - Indicator if port encoded in gport must be only local physical port
 * Returns:
 *	    BCM_E_XXX
 */

STATIC int 
_bcm_esw_ipmc_gport_resolve(int unit, bcm_gport_t gport, bcm_port_t *port, 
                            bcm_module_t *modid, bcm_trunk_t *tgid, int *tbit,
                            int isPortLocal)
{
    int id;
    bcm_trunk_t     local_tgid;
    bcm_module_t    local_modid;
    bcm_port_t      local_port;

    if ((NULL == port) || (NULL == modid) || (NULL == tgid) ||
        (NULL == tbit) ) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, gport, &local_modid, &local_port, &local_tgid, &id));
    

    if (-1 != id) {
        return BCM_E_PARAM;
    }
    if (BCM_TRUNK_INVALID != local_tgid) {
        *tgid = local_tgid;
        *tbit = 1;
    } else {
        *port = local_port;
        *modid = local_modid;
        *tbit = 0;
        if (isPortLocal) {
            int    ismymodid;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, local_modid, &ismymodid));
            if (ismymodid != TRUE) {
                return BCM_E_PARAM;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_init
 * Description:
 *	Initialize the IPMC module and enable IPMC support. This
 *	function will enable the source port checking and the
 *	search rule to include source IP checking.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	This function has to be called before any other IPMC functions.
 */

int
bcm_esw_ipmc_init(int unit)
{
    int         rv;

    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        return BCM_E_UNAVAIL;
    }

    IPMC_OUT(("IPMC %d: Init\n", unit));

    if (SOC_WARM_BOOT(unit)) {
        return (_bcm_esw_ipmc_reinit(unit));
    }

    rv = mbcm_driver[unit]->mbcm_ipmc_init(unit);

    if (rv >= 0 && soc_feature(unit, soc_feature_ip_mcast_repl)) {
        rv = mbcm_driver[unit]->mbcm_ipmc_repl_init(unit);
    }

    /*
     * Note: for Lynx, egress_port_init requires ipmc_repl_init to have
     * been done already.
     */

    if (rv >= 0) {
        rv = bcm_esw_ipmc_egress_port_init(unit);
    }

    if (rv >= 0) {
        _bcm_ipmc_init[unit] = TRUE;
    }

    return rv;
}

/*
 * Function:
 *	bcm_esw_ipmc_detach
 * Description:
 *	Delete all entries of the IPMC table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_detach(int unit)
{
    int         rv = BCM_E_NONE;

    if (!_bcm_ipmc_init[unit]) {
        return (rv);
    }

    IPMC_OUT(("IPMC %d: Detach\n", unit));
    if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
        rv = mbcm_driver[unit]->mbcm_ipmc_repl_detach(unit);
    }

    if (rv >= 0) {
        rv = mbcm_driver[unit]->mbcm_ipmc_detach(unit);
    }

    if (rv >= 0) {
	_bcm_ipmc_init[unit] = FALSE;
    }

    return rv;
}

/*
 * Function:
 *	bcm_esw_ipmc_enable
 * Description:
 *	Enable/disable IPMC support.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	enable - 1: enable IPMC support.
 *		0: disable IPMC support.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_enable(int unit, int enable)
{
    IPMC_INIT(unit);

    IPMC_OUT(("IPMC %d: %sable\n", unit, enable ? "En" : "Dis"));
    return mbcm_driver[unit]->mbcm_ipmc_enable(unit, enable);
}

/*
 * Function:
 *      bcm_esw_ipmc_source_port_check
 * Description:
 *      Enable/disable validation of the source port setting of the
 *      matching IP multicast entry against the ingress port from which an
 *      IPMC packet arrives.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      enable - 1: enable source port checking.
 *               0: disable source port checking.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_ipmc_source_port_check(int unit, int enable)
{
    IPMC_INIT(unit);

    return mbcm_driver[unit]->mbcm_ipmc_src_port_check(unit, enable);
}

/*
 * Function:
 *	bcm_esw_ipmc_source_ip_search
 * Description:
 *	Define the IPMC search rule.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	enable - 1: IPMC search key = Source IP + destination IP + VID.
 *		 0: IPMC search key = 0 + destination IP + VID.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_source_ip_search(int unit, int enable)
{
    IPMC_INIT(unit);

    return mbcm_driver[unit]->mbcm_ipmc_src_ip_search(unit, enable);
}


/*
 * Function:
 *      _bcm_esw_ipmc_key_validate
 * Description:
 *      Validate ipmc lookup key 
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *	    data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_ipmc_key_validate(int unit, bcm_ipmc_addr_t *data)
{
    /* Make sure ipmc was initialized on the device. */
    IPMC_INIT(unit);

    /* Input parameters check. */
    if (NULL == data) {
        return (BCM_E_PARAM);
    }

    /* Validate vrf range. */
    if ((data->vrf > SOC_VRF_MAX(unit)) ||
        (data->vrf < BCM_L3_VRF_DEFAULT)) {
        return (BCM_E_PARAM);
    }

    /* IPv6 support check. */ 
    if ((data->flags & BCM_IPMC_IP6) && !SOC_IS_XGS3_SWITCH(unit)) {
        if (!_bcm_proxy_client_enabled(unit)) {
            return (BCM_E_UNAVAIL);
        } else {
            data->flags |= BCM_IPMC_PROXY_IP6;
        }
    }

    /* Vlan id range check. */ 
    if (data->vid > BCM_VLAN_INVALID) {
        return (BCM_E_PARAM);
    } 

    /* Destination address must be multicast */
    /* Source address must be unicast        */
    if (data->flags & BCM_IPMC_IP6) {
        if (!BCM_IP6_MULTICAST(data->mc_ip6_addr)) {
            return (BCM_E_PARAM);
        } 
        if (BCM_IP6_MULTICAST(data->s_ip6_addr)) {
            return (BCM_E_PARAM);
        } 
    } else {
        if (!BCM_IP4_MULTICAST(data->mc_ip_addr)) {
            return (BCM_E_PARAM);
        } 
        if (BCM_IP4_MULTICAST(data->s_ip_addr)) {
            return (BCM_E_PARAM);
        } 
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *	bcm_esw_ipmc_add
 * Description:
 *	Adds a new entry to the IPMC table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	data - IPMC entry information.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_add(int unit, bcm_ipmc_addr_t *data)
{
    bcm_pbmp_t      active;
    bcm_gport_t     gport;
    bcm_port_t      local_port;
    int             is_local_mod;

    /* Input parameters validation. */
    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, data));

    if (!(data->flags & BCM_IPMC_SOURCE_PORT_NOCHECK)) {
        /* Validate source port/trunk. */ 
        if (BCM_GPORT_IS_SET(data->port_tgid)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_ipmc_gport_resolve(unit, data->port_tgid,
                                             &(data->port_tgid),
                                             &(data->mod_id),
                                             &(data->port_tgid),
                                             &(data->ts), 0)); 
        } 

        if(data->ts) { 
            if (BCM_FAILURE(_bcm_trunk_id_validate(unit, data->port_tgid))) {
                return (BCM_E_PARAM);
            }
        } else  {
            /* Check port validity only on local module. */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_modid_is_local(unit, data->mod_id, &is_local_mod));
            if (is_local_mod) {
                BCM_GPORT_MODPORT_SET(gport, data->mod_id, data->port_tgid);
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, gport, &local_port));
            }
        }
    }

    /* Validate lookup class range. */
    if ((data->lookup_class > SOC_ADDR_CLASS_MAX(unit)) || 
        (data->lookup_class < 0)) {
        return (BCM_E_PARAM);
    }


    /* Validate cos range. */ 
    if (data->flags & BCM_IPMC_SETPRI) {
        if(!BCM_PRIORITY_VALID(data->cos)) {
            return (BCM_E_PARAM);
        }
    }

    /* Add stack ports to L2 tagged; remove from L3 and untagged bitmaps */
    SOC_PBMP_STACK_ACTIVE_GET(unit, active);
    BCM_PBMP_OR(data->l2_pbmp, active);
    BCM_PBMP_REMOVE(data->l2_pbmp, SOC_PBMP_STACK_INACTIVE(unit));
    BCM_PBMP_REMOVE(data->l3_pbmp, SOC_PBMP_STACK_CURRENT(unit));
    BCM_PBMP_REMOVE(data->l2_ubmp, SOC_PBMP_STACK_CURRENT(unit));

    /*
     * Copy L3 bitmap to L2 bitmap for proxy replicated egress ports
     */
    if (data->flags & BCM_IPMC_PROXY_IP6) {
        BCM_PBMP_OR(data->l2_pbmp, data->l3_pbmp);
        BCM_PBMP_CLEAR(data->l3_pbmp);
    }

    IPMC_OUT(("IPMC %d: Add mc 0x%x, sip 0x%x, vid %d. cos %d, %s %d\n",
              unit,
              data->mc_ip_addr,
              data->s_ip_addr,
              data->vid,
              data->cos,
              data->ts ? "trunk" : "port",
              data->port_tgid));
    IPMC_VERB(("      : Ports 0x%x, UT 0x%x, L3 0x%x\n",
               SOC_PBMP_WORD_GET(data->l2_pbmp, 0),
               SOC_PBMP_WORD_GET(data->l2_ubmp, 0),
               SOC_PBMP_WORD_GET(data->l3_pbmp, 0)));

    return mbcm_driver[unit]->mbcm_ipmc_add(unit, data);
}

/*
 * Function:
 *      bcm_esw_ipmc_remove
 * Description:
 *      Delete an entry from the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *	data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_ipmc_remove(int unit, bcm_ipmc_addr_t *data)
{
    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, data));

    if ((data->flags & BCM_IPMC_IP6) && !SOC_IS_XGS3_SWITCH(unit)) {
        BCM_PBMP_OR(data->l2_pbmp, data->l3_pbmp);
        BCM_PBMP_CLEAR(data->l3_pbmp);
    }

    return mbcm_driver[unit]->mbcm_ipmc_delete(unit, data);
}

/*
 * Function:
 *	bcm_esw_ipmc_delete
 * Description:
 *	Delete an entry from the IPMC table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	s_ip_addr - Source IP address.
 *	mc_ip_addr - Destination IP address.
 *	vid - VLAN tag.
 *	keep - 1: Keep the entry in IPMC table with V bit clear.
 *             0: Remove the entry from the IPMC table.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_delete(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
		    bcm_vlan_t vid, int keep)
{
    bcm_ipmc_addr_t ipmc;

    IPMC_OUT(("IPMC %d: Del mc 0x%x, sip 0x%x, vid %d, keep %d\n",
              unit, mc_ip_addr, s_ip_addr, vid, keep));

    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    ipmc.flags = (keep) ? BCM_IPMC_KEEP_ENTRY : 0;

    return mbcm_driver[unit]->mbcm_ipmc_delete(unit, &ipmc);
}

/*
 * Function:
 *      bcm_esw_ipmc_remove_all
 * Description:
 *      Remove all entries of the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_ipmc_remove_all(int unit)
{
    IPMC_INIT(unit);


    IPMC_OUT(("IPMC %d: Del all\n", unit));

    return mbcm_driver[unit]->mbcm_ipmc_delete_all(unit);
}

/*
 * Function:
 *	bcm_esw_ipmc_delete_all
 * Description:
 *	Delete all entries of the IPMC table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_delete_all(int unit)
{
    IPMC_INIT(unit);


    IPMC_OUT(("IPMC %d: Del all\n", unit));
    return mbcm_driver[unit]->mbcm_ipmc_delete_all(unit);
}

/*
 * Function:
 *	bcm_esw_ipmc_get
 * Description:
 *	Get an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	s_ip_addr - Source IP address.
 *	mc_ip_addr - Destination IP address.
 *	vid - vlan tag.
 *	data - (IN/OUT) IPMC entry information.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_get(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
		 bcm_vlan_t vid, bcm_ipmc_addr_t *data)
{
    int         index, rv;
    int         flags;
    bcm_vrf_t     vrf;

    if(NULL == data) {
        return (BCM_E_PARAM);
    } 

    flags = data->flags;
    vrf = data->vrf;

    bcm_ipmc_addr_t_init(data);
    data->s_ip_addr = s_ip_addr;
    data->mc_ip_addr = mc_ip_addr;
    data->vid = vid;
    data->flags = flags;
    data->vrf = vrf;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, data));

    rv = mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, data);
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_ipmc_gport_construct(unit, data);
    }

    return rv;
}

/*
 * Function:
 *	bcm_esw_ipmc_get_by_index
 * Description:
 *	Get an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	index - The index number.
 *	data - (OUTPUT) IPMC entry information.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_get_by_index(int unit, int index, bcm_ipmc_addr_t *data)
{
    int rv;
    int flags;

    IPMC_INIT(unit);

    if(NULL == data) {
        return (BCM_E_PARAM);
    } 

    flags = data->flags;
    sal_memset(data, 0, sizeof(bcm_ipmc_addr_t));
    data->flags = flags;

    rv = mbcm_driver[unit]->mbcm_ipmc_get(unit, index, data);
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_ipmc_gport_construct(unit, data);
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_ipmc_find
 * Description:
 *      Find an IPMC entry.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_ipmc_find(int unit, bcm_ipmc_addr_t *data)
{
    int index, rv;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, data));

    rv = mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, data);
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_ipmc_gport_construct(unit, data); 
    }

    return rv;
}

/*
 * Function:
 *	bcm_esw_ipmc_entry_enable_set
 * Description:
 *	Set the value of an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	s_ip_addr - Source IP address.
 *	mc_ip_addr - Destination IP address.
 *	vid - vlan tag.
 *	enable - 1: The entry is enabled;
 *               0: The entry is disabled;
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_entry_enable_set(int unit,
			      bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
			      bcm_vlan_t vid, int enable)
{
    bcm_ipmc_addr_t	ipmc;
    int			index;

    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    BCM_IF_ERROR_RETURN
	    (mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, &ipmc));

    ipmc.v = enable ? 1 : 0;

    BCM_IF_ERROR_RETURN
	    (mbcm_driver[unit]->mbcm_ipmc_put(unit, index, &ipmc));

    return (BCM_E_NONE);
}

/*
 * Function:
 *	bcm_esw_ipmc_cos_set
 * Description:
 *	Set the cos of an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	s_ip_addr - Source Ip address.
 *	mc_ip_addr - Destination Ip address.
 *	vid - vlan tag.
 *	cos - Class of service.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_cos_set(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
		     bcm_vlan_t vid, int cos)
{
    bcm_ipmc_addr_t	ipmc;
    int			index;


    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    BCM_IF_ERROR_RETURN
  	    (mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, &ipmc));

    if (!ipmc.v) {
        return BCM_E_NOT_FOUND;
    }

    /* Validate cos range. */ 
    if(BCM_PRIORITY_VALID(cos)) {
        ipmc.flags |= BCM_IPMC_SETPRI;
        ipmc.cos = cos;
    } else if (cos < 0) {
        ipmc.flags &= ~BCM_IPMC_SETPRI;
        ipmc.cos = -1;
    } else {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_ipmc_put(unit, index, &ipmc));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_port_tgid_set
 * Description:
 *	Set the source port/trunk group ID of an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	s_ip_addr - Source Ip address.
 *	mc_ip_addr - Destination Ip address.
 *	vid - vlan tag.
 *	ts - 1: port_tgid is the trunk ID of the trunk port that the IP
 *		multicast traffic should arrive on.
 *	     0: port_tgid is the port the IP multicast traffic should
 *		arrive on.
 *	port_tgid - Source port/trunk group ID
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_port_tgid_set(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
			   bcm_vlan_t vid, int ts, int port_tgid)
{
    bcm_ipmc_addr_t	ipmc;
    int			index;

    /* Validate source port/trunk. */ 
    /* Port is not validated, because module id might be not local unit.*/
    if(ts) { 
        if (BCM_FAILURE(_bcm_trunk_id_validate(unit, port_tgid))) {
            return (BCM_E_PARAM);
        }
    } 

    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, &ipmc));

    if (!ipmc.v) {
        return BCM_E_NOT_FOUND;
    }

    ipmc.ts = ts;
    ipmc.port_tgid = port_tgid;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_put(unit, index, &ipmc));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_port_modid_set
 * Description:
 *	Set the input module ID of an IPMC entry.
 * Parameters:
 *	unit - PCI device unit number (driver internal).
 *	s_ip_addr - Source IP address.
 *	mc_ip_addr - Destination IP address.
 *	vid - VLAN tag.
 *	mod_id - Source module ID
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_port_modid_set(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
			    bcm_vlan_t vid, int mod_id)
{
    bcm_ipmc_addr_t	ipmc;
    int			index;

    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, &ipmc));

    if (!ipmc.v) {
        return BCM_E_NOT_FOUND;
    }

    ipmc.mod_id = mod_id;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_put(unit, index, &ipmc));

    return BCM_E_NONE;
}

/*
 * Function:
 *        bcm_esw_ipmc_add_l2_ports
 * Description:
 *        Add l2 ports to an IPMC entry.
 * Parameters:
 *        unit - StrataSwitch PCI device unit number (driver internal).
 *        s_ip_addr - Source Ip address.
 *        mc_ip_addr - Destination Ip address.
 *        vid - vlan tag.
 *        pbmp - port bit mask of ports to add.
 *        ut_pbmp - untagged port bit mask of ports to add.
 * Returns:
 *        BCM_E_XXX
 */

int
bcm_esw_ipmc_add_l2_ports(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
			  bcm_vlan_t vid, bcm_pbmp_t pbmp, bcm_pbmp_t ut_pbmp)
{
    bcm_ipmc_addr_t	ipmc;
    int			index;

    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, &ipmc));

    if (!ipmc.v) {
        return BCM_E_NOT_FOUND;
    }

    /* Remove inactive stack ports; all from untagged */
    BCM_PBMP_REMOVE(pbmp, SOC_PBMP_STACK_INACTIVE(unit));
    BCM_PBMP_REMOVE(ut_pbmp, SOC_PBMP_STACK_CURRENT(unit));

    SOC_PBMP_OR(ipmc.l2_pbmp, pbmp);
    SOC_PBMP_REMOVE(ipmc.l2_ubmp, pbmp);
    SOC_PBMP_OR(ipmc.l2_ubmp, ut_pbmp);

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_put(unit, index, &ipmc));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_delete_l2_ports
 * Description:
 *	Delete l2 port from an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	s_ip_addr - Source Ip address.
 *	mc_ip_addr - Destination Ip address.
 *	vid - vlan tag.
 *	port - port to delete.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_delete_l2_ports(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
			     bcm_vlan_t vid, bcm_port_t port)
{
    bcm_ipmc_addr_t	ipmc;
    int			index;

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port)); 
    } 
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (BCM_PBMP_MEMBER(SOC_PBMP_STACK_CURRENT(unit), port)) {
        return BCM_E_PORT;
    }

    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, &ipmc));

    if (!ipmc.v) {
        return BCM_E_NOT_FOUND;
    }

    SOC_PBMP_PORT_REMOVE(ipmc.l2_pbmp, port);
    SOC_PBMP_PORT_REMOVE(ipmc.l2_ubmp, port);

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_put(unit, index, &ipmc));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_add_l3_ports
 * Description:
 *	Add l3 ports to an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	s_ip_addr - Source Ip address.
 *	mc_ip_addr - Destination Ip address.
 *	vid - vlan tag.
 *	pbmp - port bit mask of ports to add.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_add_l3_ports(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
			  bcm_vlan_t vid, bcm_pbmp_t pbmp)
{
    bcm_ipmc_addr_t	ipmc;
    int			index;

    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, &ipmc));

    if (!ipmc.v) {
        return BCM_E_NOT_FOUND;
    }

    /* Remove all stack ports */
    BCM_PBMP_REMOVE(pbmp, SOC_PBMP_STACK_CURRENT(unit));

    SOC_PBMP_OR(ipmc.l3_pbmp, pbmp);

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_put(unit, index, &ipmc));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_delete_l3_port
 * Description:
 *	Delete l3 port from an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	s_ip_addr - Source Ip address.
 *	mc_ip_addr - Destination Ip address.
 *	vid - vlan tag.
 *	port - port to delete.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_delete_l3_port(int unit, bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr,
			    bcm_vlan_t vid, bcm_port_t port)
{
    bcm_ipmc_addr_t	ipmc;
    int			index;

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port)); 
    } 
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (BCM_PBMP_MEMBER(SOC_PBMP_STACK_CURRENT(unit), port)) {
        return BCM_E_PORT;
    }

    bcm_ipmc_addr_t_init(&ipmc);
    ipmc.s_ip_addr = s_ip_addr;
    ipmc.mc_ip_addr = mc_ip_addr;
    ipmc.vid = vid;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, &ipmc));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, &ipmc));

    if (!ipmc.v) {
        return BCM_E_NOT_FOUND;
    }

    SOC_PBMP_PORT_REMOVE(ipmc.l3_pbmp, port);

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_put(unit, index, &ipmc));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_port_init
 * Description:
 *	Reinitialize/clear the egress IP multicast configuration
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_port_init(int unit)
{

    if (soc_feature(unit, soc_feature_ip_mcast)) {
	int		port;

	PBMP_E_ITER(unit, port) {
	    BCM_IF_ERROR_RETURN
		(bcm_esw_ipmc_egress_port_set(unit, port,
                                              _soc_mac_all_zeroes,
                                              0, 0, 0));
	}
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_port_set
 * Description:
 *	Config the egress IP multicast Configuration Register.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - port to config.
 *	mac - MAC address.
 *	untag - 1: The IP multicast packet is transmitted as untagged packet.
 *		0: The IP multicast packet is transmitted as tagged packet
 *		with VLAN tag vid.
 *	vid  - VLAN ID.
 *	ttl_thresh - Drop IPMC packets if TTL of IP packet <= ttl_thresh.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_port_set(int unit, bcm_port_t port, const bcm_mac_t mac,
			     int untag, bcm_vlan_t vid, int ttl_thresh)
{
    /*
     * Can't check IPMC_INIT here because this function is used as part
     * of the bcm_esw_ipmc_init function (via bcm_ipmc_egress_port_init).
     * Just check that IPMC feature is enabled.
     */
    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check.*/
    if (vid > BCM_VLAN_INVALID) {
        return (BCM_E_PARAM);
    }
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port)); 
    } 
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (!BCM_TTL_VALID(ttl_thresh)) {
        return (BCM_E_PARAM);
    }

    return mbcm_driver[unit]->mbcm_ipmc_egress_port_set(unit, port, mac,
                                                        untag, vid, ttl_thresh);
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_port_get
 * Description:
 *	Get the egress IP multicast Configuration Register.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - port to config.
 *	mac - (OUT) MAC address.
 *	untag - (OUT) 1: The IP multicast packet is transmitted as
 *			 untagged packet.
 *		      0: The IP multicast packet is transmitted as tagged
 *			 packet with VLAN tag vid.
 *	vid - (OUT) VLAN ID.
 *	ttl_thresh - (OUT) Drop IPMC packets if TTL <= ttl_thresh.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_port_get(int unit, bcm_port_t port, mac_addr_t mac,
			     int *untag, bcm_vlan_t *vid, int *ttl_thresh)
{
    /* Make sure ipmc was initialized on the device. */
    IPMC_INIT(unit);

    if ((NULL == untag) || (NULL == vid) || (NULL == ttl_thresh)) {
        return (BCM_E_PARAM);
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port)); 
    } 
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    return mbcm_driver[unit]->mbcm_ipmc_egress_port_get(unit, port, mac,
							untag, vid, ttl_thresh);
}

/*
 * Function:
 *	bcm_esw_ipmc_counters_get
 * Description:
 *	Get IPMC counters of hardware.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - port to get information.
 *	counters - (OUT) IPMC entry information.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_counters_get(int unit, bcm_port_t port,
			  bcm_ipmc_counters_t *counters)
{
    IPMC_INIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port)); 
    } 

    if (!SOC_PBMP_PORT_VALID(port) || !IS_E_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if (NULL == counters) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
	(bcm_esw_stat_get(unit, port, snmpIfHCInMulticastPkts,
                          &counters->rmca));
    BCM_IF_ERROR_RETURN
	(bcm_esw_stat_get(unit, port, snmpIfHCOutMulticastPkts,
                          &counters->tmca));
    BCM_IF_ERROR_RETURN
	(bcm_esw_stat_get(unit, port, snmpBcmIPMCBridgedPckts,
                          &counters->imbp));
    BCM_IF_ERROR_RETURN
	(bcm_esw_stat_get(unit, port, snmpBcmIPMCRoutedPckts,
                          &counters->imrp));
    BCM_IF_ERROR_RETURN
	(bcm_esw_stat_get(unit, port, snmpBcmIPMCInDroppedPckts,
                          &counters->rimdr));
    BCM_IF_ERROR_RETURN
	(bcm_esw_stat_get(unit, port, snmpBcmIPMCOutDroppedPckts,
                          &counters->timdr));

    return BCM_E_NONE;
}

#if defined(BCM_XGS12_FABRIC_SUPPORT)
STATIC int
_bcm5670_ipmc_bitmap_set(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    mem_mc_entry_t  entry;
    int             blk;
    int             rv;

    /* Make sure packet never goes back */
    BCM_PBMP_PORT_REMOVE(pbmp, in_port);

    blk = SOC_PORT_BLOCK(unit, in_port);

    sal_memset(&entry, 0, sizeof(entry));

    soc_mem_lock(unit, MEM_MCm);

    if ((rv = READ_MEM_MCm(unit, blk, ipmc_idx, &entry)) >= 0)
    {
        soc_MEM_MCm_field32_set(unit, &entry, IPMCBITMAPf,
                                  SOC_PBMP_WORD_GET(pbmp, 0));
        rv = WRITE_MEM_MCm(unit, blk, ipmc_idx, &entry);
    }

    soc_mem_unlock(unit, MEM_MCm);

    return rv;
}

STATIC int
_bcm5670_ipmc_bitmap_get(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t *pbmp)
{
    mem_mc_entry_t  entry;
    int             blk;

    sal_memset(&entry, 0, sizeof(entry));
    blk = SOC_PORT_BLOCK(unit, in_port);

    SOC_IF_ERROR_RETURN
        (READ_MEM_MCm(unit, blk, ipmc_idx, &entry));

    BCM_PBMP_CLEAR(*pbmp);
    SOC_PBMP_WORD_SET(
        *pbmp, 0, soc_MEM_MCm_field32_get(unit, &entry, IPMCBITMAPf));

    return BCM_E_NONE;
}

STATIC int
_bcm5670_ipmc_bitmap_del(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    mem_mc_entry_t  entry;
    uint32          fval;
    int             blk;
    int             rv;

    blk = SOC_PORT_BLOCK(unit, in_port);

    sal_memset(&entry, 0, sizeof(entry));

    soc_mem_lock(unit, MEM_MCm);

    if ((rv = READ_MEM_MCm(unit, blk, ipmc_idx, &entry)) >= 0)
    {
        soc_MEM_MCm_field_get(unit, &entry, IPMCBITMAPf, &fval);
        fval &= ~SOC_PBMP_WORD_GET(pbmp, 0);
        soc_MEM_MCm_field32_set(unit, &entry, IPMCBITMAPf, fval);
        rv = WRITE_MEM_MCm(unit, blk, ipmc_idx, &entry);

    }
    soc_mem_unlock(unit, MEM_MCm);

    return rv;
}

#if defined(BCM_XGS2_FABRIC_SUPPORT)
STATIC int
_bcm5675_ipmc_bitmap_set(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    mem_ipmc_entry_t  entry;
    int             blk;
    int             rv;

    /* Make sure packet never goes back */
    BCM_PBMP_PORT_REMOVE(pbmp, in_port);

    blk = SOC_PORT_BLOCK(unit, in_port);

    sal_memset(&entry, 0, sizeof(entry));

    soc_mem_lock(unit, MEM_IPMCm);

    if ((rv = READ_MEM_IPMCm(unit, blk, ipmc_idx, &entry)) >= 0)
    {
        soc_MEM_IPMCm_field32_set(unit, &entry, IPMCBITMAPf,
                                  SOC_PBMP_WORD_GET(pbmp, 0));
        rv = WRITE_MEM_IPMCm(unit, blk, ipmc_idx, &entry);
    }

    soc_mem_unlock(unit, MEM_IPMCm);

    return rv;
}

STATIC int
_bcm5675_ipmc_bitmap_get(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t *pbmp)
{
    mem_ipmc_entry_t  entry;
    int             blk;

    sal_memset(&entry, 0, sizeof(entry));
    blk = SOC_PORT_BLOCK(unit, in_port);

    SOC_IF_ERROR_RETURN
        (READ_MEM_IPMCm(unit, blk, ipmc_idx, &entry));

    BCM_PBMP_CLEAR(*pbmp);
    SOC_PBMP_WORD_SET(
        *pbmp, 0, soc_MEM_IPMCm_field32_get(unit, &entry, IPMCBITMAPf));

    return BCM_E_NONE;
}

STATIC int
_bcm5675_ipmc_bitmap_del(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    mem_ipmc_entry_t  entry;
    uint32          fval;
    int             blk;
    int             rv;

    blk = SOC_PORT_BLOCK(unit, in_port);

    sal_memset(&entry, 0, sizeof(entry));

    soc_mem_lock(unit, MEM_IPMCm);

    if ((rv = READ_MEM_IPMCm(unit, blk, ipmc_idx, &entry)) >= 0)
    {
        soc_MEM_IPMCm_field_get(unit, &entry, IPMCBITMAPf, &fval);
        fval &= ~SOC_PBMP_WORD_GET(pbmp, 0);
        soc_MEM_IPMCm_field32_set(unit, &entry, IPMCBITMAPf, fval);
        rv = WRITE_MEM_IPMCm(unit, blk, ipmc_idx, &entry);

    }
    soc_mem_unlock(unit, MEM_IPMCm);

    return rv;
}
#endif /* BCM_XGS2_FABRIC_SUPPORT */
#endif /* BCM_XGS12_FABRIC_SUPPORT */

#ifdef BCM_BRADLEY_SUPPORT
int
_bcm_xgs3_ipmc_bitmap_set(int unit, int ipmc_idx, bcm_pbmp_t pbmp)
{
    int		rv, mc_base, mc_size, mc_index;
    l2mc_entry_t	l2mc;

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
    if (ipmc_idx < 0 || ipmc_idx > mc_size) {
        return BCM_E_PARAM;
    }
    mc_index = ipmc_idx + mc_base;

    soc_mem_lock(unit, L2MCm);
    rv = READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &l2mc);
    if (rv >= 0) {
        soc_mem_field32_set(unit, L2MCm, &l2mc, PORT_BITMAPf,
                            SOC_PBMP_WORD_GET(pbmp, 0));
        soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
        rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index, &l2mc);
    }
    soc_mem_unlock(unit, L2MCm);

    /* set the l3_ipmc table entry valid but empty */
    if (rv >= 0 && ipmc_idx <= soc_mem_index_max(unit, L3_IPMCm)) {
        ipmc_entry_t	ipmc_e;

        sal_memset(&ipmc_e, 0, sizeof(ipmc_e));
        soc_mem_field32_set(unit, L3_IPMCm, &ipmc_e, VALIDf, 1);
        rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, ipmc_idx, &ipmc_e);
    }
    return rv;
}

int
_bcm_xgs3_ipmc_bitmap_get(int unit, int ipmc_idx, bcm_pbmp_t *pbmp)
{
    int		mc_base, mc_size, mc_index;
    uint32		pword;
    l2mc_entry_t	l2mc;

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
    if (ipmc_idx < 0 || ipmc_idx > mc_size) {
        return BCM_E_PARAM;
    }
    mc_index = ipmc_idx + mc_base;

    SOC_IF_ERROR_RETURN(READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &l2mc));
    pword = soc_mem_field32_get(unit, L2MCm, &l2mc, PORT_BITMAPf);
    SOC_PBMP_CLEAR(*pbmp);
    SOC_PBMP_WORD_SET(*pbmp, 0, pword);
    return BCM_E_NONE;
}

int
_bcm_xgs3_ipmc_bitmap_del(int unit, int ipmc_idx, bcm_pbmp_t pbmp)
{
    int		rv, mc_base, mc_size, mc_index;
    uint32          pword;
    l2mc_entry_t	l2mc;

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
    if (ipmc_idx < 0 || ipmc_idx > mc_size) {
        return BCM_E_PARAM;
    }
    mc_index = ipmc_idx + mc_base;

    soc_mem_lock(unit, L2MCm);
    rv = READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &l2mc);
    if (rv >= 0) {
        pword = soc_mem_field32_get(unit, L2MCm, &l2mc, PORT_BITMAPf);
        pword &= ~SOC_PBMP_WORD_GET(pbmp, 0);
        soc_mem_field32_set(unit, L2MCm, &l2mc, PORT_BITMAPf, pword);
        soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
        rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index, &l2mc);
    }
    soc_mem_unlock(unit, L2MCm);

    /* set the l3_ipmc table entry valid but empty */
    if (rv >= 0 && ipmc_idx <= soc_mem_index_max(unit, L3_IPMCm)) {
        ipmc_entry_t	ipmc_e;

        sal_memset(&ipmc_e, 0, sizeof(ipmc_e));
        soc_mem_field32_set(unit, L3_IPMCm, &ipmc_e, VALIDf, 1);
        rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, ipmc_idx, &ipmc_e);
    }
    return rv;
}
#endif /* BCM_BRADLEY_SUPPORT */

/*
 * Function:
 *	bcm_esw_ipmc_bitmap_max_get
 * Purpose:
 *	Provides maximum IPMC index that this fabric can handle
 * Parameters:
 *	unit - device
 *	max_index - (OUT) returned number of entries
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Fabric only
 */

int
bcm_esw_ipmc_bitmap_max_get(int unit, int *max_index)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_HERCULES1(unit)) {
	*max_index = soc_mem_index_count(unit, MEM_MCm);
	return BCM_E_NONE;
    }
#ifdef	BCM_XGS2_FABRIC_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
	*max_index = soc_mem_index_count(unit, MEM_IPMCm);
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS2_FABRIC_SUPPORT */
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int	mc_base, mc_size;

	SOC_IF_ERROR_RETURN
            (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
	*max_index = mc_size;
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    *max_index = 0;
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_bitmap_set
 * Purpose:
 *	Set the IPMC forwarding port bitmap
 * Parameters:
 *	unit     - SOC unit #
 *	index    - the IPMC index carried in HiGig header
 *	in_port  - ingress port number
 *	pbmp     - IPMC forwarding port bitmap
 * Returns:
 *	BCM_E_XXX
 * Note:
 *	Fabric only
 */

int
bcm_esw_ipmc_bitmap_set(int unit, int index,
			bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, in_port, &in_port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, in_port)) {
        return BCM_E_PORT;
    }
#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_HERCULES1(unit)) {
        return _bcm5670_ipmc_bitmap_set(unit, index, in_port, pbmp);
    }
#ifdef	BCM_XGS2_FABRIC_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        return _bcm5675_ipmc_bitmap_set(unit, index, in_port, pbmp);
    }
#endif	/* BCM_XGS2_FABRIC_SUPPORT */
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        COMPILER_REFERENCE(in_port);
        return _bcm_xgs3_ipmc_bitmap_set(unit, index, pbmp);
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_bitmap_get
 * Purpose:
 *	Get the IPMC forwarding port bitmap
 * Parameters:
 *	unit     - SOC unit #
 *	index    - the IPMC index carried in HiGig header
 *	in_port  - ingress port number
 *	pbmp     - IPMC forwarding port bitmap
 * Returns:
 *	BCM_E_XXX
 * Note:
 *	Fabric only
 */

int
bcm_esw_ipmc_bitmap_get(int unit, int index,
			bcm_port_t in_port, bcm_pbmp_t *pbmp)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, in_port, &in_port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, in_port)) {
        return BCM_E_PORT;
    }

#ifdef	BCM_XGS12_FABRIC_SUPPORT
#ifdef	BCM_XGS2_FABRIC_SUPPORT
    if (SOC_IS_HERCULES1(unit)) {
	return _bcm5670_ipmc_bitmap_get(unit, index, in_port, pbmp);
    }
    if (SOC_IS_HERCULES15(unit)) {
        return _bcm5675_ipmc_bitmap_get(unit, index, in_port, pbmp);
    }
#endif	/* BCM_XGS2_FABRIC_SUPPORT */
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        COMPILER_REFERENCE(in_port);
        return _bcm_xgs3_ipmc_bitmap_get(unit, index, pbmp);
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_bitmap_del
 * Purpose:
 *	Remove IPMC forwarding port bitmap
 * Parameters:
 *	unit     - SOC unit #
 *	index - the IPMC index carried in HiGig header
 *	in_port  - ingress port number
 *	pbmp     - IPMC forwarding port bitmap
 * Returns:
 *	BCM_E_XXX
 * Note:
 * 	Fabric only
 */

int
bcm_esw_ipmc_bitmap_del(int unit, int index,
			bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, in_port, &in_port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, in_port)) {
        return BCM_E_PORT;
    }

#ifdef	BCM_XGS12_FABRIC_SUPPORT
#ifdef	BCM_XGS2_FABRIC_SUPPORT
    if (SOC_IS_HERCULES1(unit)) {
	return _bcm5670_ipmc_bitmap_del(unit, index, in_port, pbmp);
    }
    if (SOC_IS_HERCULES15(unit)) {
        return _bcm5675_ipmc_bitmap_del(unit, index, in_port, pbmp);
    }
#endif	/* BCM_XGS2_FABRIC_SUPPORT */
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        COMPILER_REFERENCE(in_port);
        return _bcm_xgs3_ipmc_bitmap_del(unit, index, pbmp);
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_repl_get
 * Purpose:
 *	Return set of VLANs selected for port's replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	index    - The index number.
 *	port     - port for which to retrieve info.
 *	vlan_vec - (OUT) vector of replicated VLANs common to selected ports.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_repl_get(int unit, int index, bcm_port_t port,
		      bcm_vlan_vector_t vlan_vec)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, port, &port, &modid, &tgid, &t, 1)); 
    } 
    /* Convert system ports to physical ports */
    if (soc_feature(unit, soc_feature_sysport_remap)) {
        BCM_XLATE_SYSPORT_S2P(unit, &port);
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    return mbcm_driver[unit]->mbcm_ipmc_repl_get(unit, index, port, vlan_vec);
}

/*
 * Function:
 *	bcm_esw_ipmc_repl_add
 * Purpose:
 *	Add a VLAN to selected ports' replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	index    - The index number.
 *	pbmp     - port bitmap for ports to add.
 *	vlan     - replicated VLAN for these ports.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_repl_add(int unit, int index, bcm_pbmp_t pbmp, bcm_vlan_t vlan)
{
    bcm_port_t		port;
    bcm_pbmp_t		work_pbm;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (!BCM_VLAN_VALID(vlan)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_port_get(unit, vlan, &work_pbm, NULL));

    BCM_PBMP_AND(work_pbm, pbmp);

    if (BCM_PBMP_NEQ(work_pbm, pbmp)) {
        return BCM_E_PARAM;
    }

    /*
     * Now we've verified that the requested VLAN exists for
     * the specified port set.
     */

    BCM_PBMP_ITER(pbmp, port) {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_ipmc_repl_add(unit, index, port, vlan));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_repl_delete
 * Purpose:
 *	Remove VLAN from selected ports' replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	index    - The index number.
 *	pbmp     - port bitmap for ports to remove.
 *	vlan     - VLAN to remove from replication.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_repl_delete(int unit, int index, bcm_pbmp_t pbmp, bcm_vlan_t vlan)
{
    bcm_port_t		port;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (!BCM_VLAN_VALID(vlan)) {
        return (BCM_E_PARAM);
    }

    BCM_PBMP_ITER(pbmp, port) {
	BCM_IF_ERROR_RETURN
	    (mbcm_driver[unit]->mbcm_ipmc_repl_delete(unit, index,
						      port, vlan));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_ipmc_age
 * Purpose:
 *      Age out the ipmc entry by clearing the HIT bit when appropriate,
 *      the ipmc entry itself is removed if HIT bit is not set.
 * Parameters:
 *      unit       -  (IN) BCM device number.
 *      flags      -  (IN) The criteria used to age out ipmc table.
 *      age_cb     -  (IN) Call back routine.
 *      user_data  -  (IN) User provided cookie for callback.
 * Returns:
 *      BCM_E_XXX 
 */
int
bcm_esw_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_cb, 
                 void *user_data)
{
    IPMC_INIT(unit);
    
    return mbcm_driver[unit]->mbcm_ipmc_age(unit, flags, age_cb, user_data);
}

/*
 * Function:
 *      bcm_esw_ipmc_traverse
 * Purpose:
 *      Go through all valid ipmc entries, and call the callback function
 *      at each entry
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      flags     - (IN) The criteria used to age out ipmc table.
 *      cb        - (IN) User supplied callback function.
 *      user_data - (IN) User supplied cookie used in parameter 
 *                       in callback function.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_ipmc_traverse(int unit, uint32 flags,
                      bcm_ipmc_traverse_cb cb, void *user_data)
{
    IPMC_INIT(unit);

    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    return  mbcm_driver[unit]->mbcm_ipmc_traverse(unit, flags, cb, user_data);
}


/*
 * Function:
 *	bcm_esw_ipmc_repl_delete_all
 * Purpose:
 *	Remove all VLANs from selected ports' replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	index    - The index number.
 *	pbmp     - port bitmap for ports to remove.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_repl_delete_all(int unit, int index, bcm_pbmp_t pbmp)
{
    bcm_port_t		port;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    BCM_PBMP_ITER(pbmp, port) {
	BCM_IF_ERROR_RETURN
	    (mbcm_driver[unit]->mbcm_ipmc_repl_delete_all(unit, index, port));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_repl_set
 * Purpose:
 *	Assign set of VLANs provided to port's replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	mc_index  - The index number.
 *	port     - port to list.
 *	vlan_vec - (IN) vector of replicated VLANs common to selected ports.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_repl_set(int unit, int mc_index, bcm_port_t port,
                      bcm_vlan_vector_t vlan_vec)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, port, &port, &modid, &tgid, &t, 1)); 
    } 
    /* Convert system ports to physical ports */
    if (soc_feature(unit, soc_feature_sysport_remap)) {
        BCM_XLATE_SYSPORT_S2P(unit, &port);
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#ifdef	BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
       return bcm_draco15_ipmc_repl_set(unit, mc_index, port, vlan_vec);
    }
#endif
#ifdef	BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
       return bcm_fb_ipmc_repl_set(unit, mc_index, port, vlan_vec);
    }
#endif
#ifdef	BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
       return bcm_er_ipmc_repl_set(unit, mc_index, port, vlan_vec);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_intf_set
 * Purpose:
 *	Assign set of interfaces provided to port's replication list
 *      for chosen IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	mc_index - The index number.
 *	port     - port to list.
 *      if_count - number of interfaces in list.
 *	if_array - (IN) array of bcm_if_t interfaces, size if_count.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_intf_set(int unit, int mc_index, bcm_port_t port,
                             int if_count, bcm_if_t *if_array)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             id, t, ismymodid;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, modid, &ismymodid));
            if (ismymodid != TRUE) {
                return BCM_E_PORT;
            }
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_ipmc_gport_resolve(unit, port, &port, &modid, 
                                            &tgid, &t, 1)); 
        }
    } 
    /* Convert system ports to physical ports */
    if (soc_feature(unit, soc_feature_sysport_remap)) {
        BCM_XLATE_SYSPORT_S2P(unit, &port);
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#ifdef	BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_ipmc_egress_intf_set(unit, mc_index, port,
                                            if_count, if_array);
    }
#endif
#ifdef	BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return bcm_draco15_ipmc_egress_intf_set(unit, mc_index, port,
                                                if_count, if_array);
    }
#endif
#ifdef	BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return bcm_fb_ipmc_egress_intf_set(unit, mc_index, port,
                                           if_count, if_array, 1);
    }
#endif
#ifdef	BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_ipmc_egress_intf_set(unit, mc_index, port,
                                           if_count, if_array);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_intf_get
 * Purpose:
 *	Retrieve list of interfaces in port's replication list
 *      for chosen IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	mc_index - The index number.
 *	port     - port to list.
 *      if_max   - maximum size of returned interface array.
 *      if_count - (OUT) number of interfaces in list.
 *	if_array - (OUT) array of bcm_if_t interfaces, size if_count.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_intf_get(int unit, int mc_index, bcm_port_t port,
                             int if_max, bcm_if_t *if_array, int *if_count)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             id, t, ismymodid;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, modid, &ismymodid));
            if (ismymodid != TRUE) {
                return BCM_E_PORT;
            }
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_ipmc_gport_resolve(unit, port, &port, &modid, 
                                            &tgid, &t, 1)); 
        }
    }
    /* Convert system ports to physical ports */
    if (soc_feature(unit, soc_feature_sysport_remap)) {
        BCM_XLATE_SYSPORT_S2P(unit, &port);
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#ifdef	BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_ipmc_egress_intf_get(unit, mc_index, port,
                                            if_max, if_array, if_count);
    }
#endif
#ifdef	BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return bcm_draco15_ipmc_egress_intf_get(unit, mc_index, port,
                                                if_max, if_array, if_count);
    }
#endif
#ifdef	BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return bcm_fb_ipmc_egress_intf_get(unit, mc_index, port,
                                           if_max, if_array, if_count);
    }
#endif
#ifdef	BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_ipmc_egress_intf_get(unit, mc_index, port,
                                           if_max, if_array, if_count);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_intf_add
 * Purpose:
 *	Add an egress L3 interface for the given IPMC Group Index
 *      and the set of egress ports for replication
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	index      - The IPMC Group index number.
 *	port       - egress port to add.
 *	l3_intf    - L3 interface for replication on egress ports.
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_esw_ipmc_egress_intf_add(int unit, int index, bcm_port_t port, 
                             bcm_l3_intf_t *l3_intf)
{
    bcm_pbmp_t		work_pbm;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, port, &port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    /* Input parameters check. */
    if (NULL == l3_intf) {
        return (BCM_E_PARAM);
    } 

    if (!BCM_VLAN_VALID(l3_intf->l3a_vid)) {
        return (BCM_E_PARAM);
    }

    /* Verify that port is vlan member. */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, l3_intf->l3a_vid, &work_pbm, NULL));

    if (!BCM_PBMP_MEMBER(work_pbm, port)) {
        return (BCM_E_PARAM);
    }

    /*
     * Now we've verified that the requested VLAN exists for
     * the specified port.
     */
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_egress_intf_add(unit, index, port, 
                                                      l3_intf));

    return (BCM_E_NONE);
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_intf_delete
 * Purpose:
 *	Delete an egress L3 interface for the given IPMC Group Index
 *      and the set of egress ports
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	index      - The IPMC Group index number.
 *	port       - egress port to delete.
 *	l3_intf    - L3 interface for replication on egress ports.
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_esw_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port, 
                                bcm_l3_intf_t *l3_intf)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, port, &port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (NULL == l3_intf) {
        return (BCM_E_PARAM);
    } 

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_ipmc_egress_intf_delete(unit, index, port,
					                 l3_intf));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_intf_delete_all
 * Purpose:
 *	Remove all L3 Interfaces from selected ports' replication list for 
 *      chosen IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	index    - The index number.
 *	port     - egress ports to remove.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_intf_delete_all(int unit, int index, bcm_port_t port)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, port, &port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
      (mbcm_driver[unit]->mbcm_ipmc_repl_delete_all(unit, index, port));

    return BCM_E_NONE;
}

/****************************************************************
 *
 * Stack port updating of IPMC tables
 *
 ****************************************************************/

STATIC void
_xgs_ipmc_bitmaps_get(int unit, ipmc_entry_t *ipmc_entry,
                      bcm_pbmp_t *l2_ports, bcm_pbmp_t *l3_ports)
{
    uint32 bm32;

    BCM_PBMP_CLEAR(*l2_ports);
    BCM_PBMP_CLEAR(*l3_ports);

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L3_BITMAP_LOf);
        SOC_PBMP_WORD_SET(*l3_ports, 0, bm32);
        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L3_BITMAP_HIf);
        SOC_PBMP_WORD_SET(*l3_ports, 1, bm32);

        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L2_BITMAP_LOf);
        SOC_PBMP_WORD_SET(*l2_ports, 0, bm32);
        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L2_BITMAP_HIf);
        SOC_PBMP_WORD_SET(*l2_ports, 1, bm32);
    } else
#endif
    if (SOC_IS_TUCANA(unit)) {
#ifdef BCM_TUCANA_SUPPORT
        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L3_BITMAP_M0f);
        SOC_PBMP_WORD_SET(*l3_ports, 0, bm32);
        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L3_BITMAP_M1f);
        SOC_PBMP_WORD_SET(*l3_ports, 1, bm32);

        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L2_BITMAP_M0f);
        SOC_PBMP_WORD_SET(*l2_ports, 0, bm32);
        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L2_BITMAP_M1f);
        SOC_PBMP_WORD_SET(*l2_ports, 1, bm32);
#endif
    } else {
        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L3_BITMAPf);
        SOC_PBMP_WORD_SET(*l3_ports, 0, bm32);

        bm32 = soc_L3_IPMCm_field32_get(unit, ipmc_entry, L2_BITMAPf);
        SOC_PBMP_WORD_SET(*l2_ports, 0, bm32);
    }
}

STATIC void
_xgs_ipmc_bitmaps_set(int unit, ipmc_entry_t *ipmc_entry,
                      bcm_pbmp_t l2_ports, bcm_pbmp_t l3_ports)
{
    uint32 bm32;

    /* Set new bitmaps and write back entry */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        bm32 = SOC_PBMP_WORD_GET(l3_ports, 0);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L3_BITMAP_LOf, bm32);
        bm32 = SOC_PBMP_WORD_GET(l3_ports, 1);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L3_BITMAP_HIf, bm32);

        bm32 = SOC_PBMP_WORD_GET(l2_ports, 0);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L2_BITMAP_LOf, bm32);
        bm32 = SOC_PBMP_WORD_GET(l2_ports, 1);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L2_BITMAP_HIf, bm32);
    } else
#endif
    if (SOC_IS_TUCANA(unit)) {
#ifdef BCM_TUCANA_SUPPORT
        bm32 = SOC_PBMP_WORD_GET(l3_ports, 0);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L3_BITMAP_M0f, bm32);
        bm32 = SOC_PBMP_WORD_GET(l3_ports, 1);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L3_BITMAP_M1f, bm32);

        bm32 = SOC_PBMP_WORD_GET(l2_ports, 0);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L2_BITMAP_M0f, bm32);
        bm32 = SOC_PBMP_WORD_GET(l2_ports, 1);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L2_BITMAP_M1f, bm32);
#endif
    } else {
        bm32 = SOC_PBMP_WORD_GET(l3_ports, 0);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L3_BITMAPf, bm32);

        bm32 = SOC_PBMP_WORD_GET(l2_ports, 0);
        soc_L3_IPMCm_field32_set(unit, ipmc_entry, L2_BITMAPf, bm32);
    }

}


STATIC int
_xgs_ipmc_stk_update(int unit, bcm_pbmp_t add_ports,
                     bcm_pbmp_t remove_ports)
{
    int rv = BCM_E_NONE;  /* Ignore if unavailable */

#if defined(BCM_XGS_SUPPORT)
    int i;
    ipmc_entry_t	ipmc_entry;
    bcm_pbmp_t new_l2_ports, old_l2_ports;
    bcm_pbmp_t new_l3_ports, old_l3_ports;
    int changed = 0;

    soc_mem_lock(unit, L3_IPMCm);
    for (i = soc_mem_index_min(unit, L3_IPMCm);
             i <= soc_mem_index_max(unit, L3_IPMCm);
             i++) {
        rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, i, &ipmc_entry);
        if (rv < 0) {
            break;
        }
        if (soc_L3_IPMCm_field32_get(unit, &ipmc_entry, VALIDf) == 0) {
            continue;
        }

        /* Get bitmaps, update, write back if changed */
        _xgs_ipmc_bitmaps_get(unit, &ipmc_entry,
                              &old_l2_ports, &old_l3_ports);
        SOC_PBMP_ASSIGN(new_l3_ports, old_l3_ports);
        SOC_PBMP_ASSIGN(new_l2_ports, old_l2_ports);

        BCM_PBMP_OR(new_l2_ports, add_ports);
        BCM_PBMP_REMOVE(new_l2_ports, remove_ports);
        BCM_PBMP_REMOVE(new_l3_ports, SOC_PBMP_STACK_CURRENT(unit));

        if (BCM_PBMP_NEQ(new_l2_ports, old_l2_ports) ||
                BCM_PBMP_NEQ(new_l3_ports, old_l3_ports)) {
            ++changed;
            _xgs_ipmc_bitmaps_set(unit, &ipmc_entry,
                                  new_l2_ports, new_l3_ports);
            rv = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, i, &ipmc_entry);
            if (rv < 0) {
                break;
            }
        }
    }
    soc_mem_unlock(unit, L3_IPMCm);

    if (changed) {
        IPMC_VERB(("IPMC %d: xgs stk update changed %d entries\n",
                   unit, changed));
    }
#endif

    return rv;
}


/*
 * Function:
 *      _bcm_esw_ipmc_stk_update
 * Purpose:
 *      Update port bitmaps after stack change
 * Parameters:
 *
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */


int
_bcm_esw_ipmc_stk_update(int unit, bcm_port_t port, uint32 flags)
{
    bcm_pbmp_t add_ports, remove_ports;
    int rv = BCM_E_NONE;

    IPMC_DEBUG(BCM_DBG_STK,
               ("IPMC %d: stk_update port %d flags 0x%x\n",
                unit, port, flags));

    if (soc_feature(unit, soc_feature_ip_mcast)) {
        /* Calculate ports that should and should not be in bitmap */
        SOC_PBMP_ASSIGN(add_ports, SOC_PBMP_STACK_CURRENT(unit));
        SOC_PBMP_REMOVE(add_ports, SOC_PBMP_STACK_INACTIVE(unit));

        /* Remove ports no longer stacking, or explicitly inactive */
        SOC_PBMP_ASSIGN(remove_ports, SOC_PBMP_STACK_PREVIOUS(unit));
        SOC_PBMP_REMOVE(remove_ports, SOC_PBMP_STACK_CURRENT(unit));
        SOC_PBMP_OR(remove_ports, SOC_PBMP_STACK_INACTIVE(unit));

        if (SOC_IS_XGS_SWITCH(unit)) {
            rv = _xgs_ipmc_stk_update(unit, add_ports, remove_ports);
        } else { 
            rv = BCM_E_NONE;  /* Ignore if unavailable */
        }
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_ipmc_sw_dump
 * Purpose:
 *     Displays IPMC information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_ipmc_sw_dump(int unit)
{
    soc_cm_print("\nSW Information IPMC - Unit %d\n", unit);
    soc_cm_print("  Init : %d\n", _bcm_ipmc_init[unit]);

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_EASYRIDER(unit)) {
        _bcm_xgs3_ipmc_sw_dump(unit);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#else /* INCLUDE_L3 */
int _bcm_esw_ipmc_not_empty;
#endif	/* INCLUDE_L3 */
