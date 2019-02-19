/*
 * $Id: esw_mcast.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * Multicast Table Management
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/feature.h>
#include <soc/mem.h>
#include <soc/debug.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm/error.h>
#include <bcm/mcast.h>
#include <bcm/debug.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/mcast.h>
#include <bcm_int/esw/firebolt.h>

#include <bcm_int/esw_dispatch.h>

#define MCAST_INIT_RETURN(unit) \
        if (!_bcm_mcast_init[unit]) {return BCM_E_INIT;}

#define MCAST_DEBUG(flags, stuff) BCM_DEBUG(flags | BCM_DBG_MCAST, stuff)
#define MCAST_OUT(stuff) BCM_DEBUG(BCM_DBG_MCAST, stuff)
#define MCAST_WARN(stuff) MCAST_DEBUG(BCM_DBG_WARN, stuff)
#define MCAST_ERR(stuff) MCAST_DEBUG(BCM_DBG_ERR, stuff)
#define MCAST_VERB(stuff) MCAST_DEBUG(BCM_DBG_VERBOSE, stuff)

static int _bcm_mcast_init[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_esw_mcast_detach
 * Purpose:
 *      De-initialize multicast bcm routines.
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_mcast_detach(int unit)
{
    int rv;

    rv = mbcm_driver[unit]->mbcm_mcast_detach(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    _bcm_mcast_init[unit] = FALSE;

    return rv;
}

/*
 * Function:
 *      bcm_esw_mcast_init
 * Purpose:
 *      Initialize multicast bcm routines.
 * Parameters:
 *      unit - StrataSwitch unit #
 * Returns:
 *      Mcast table size for XGS, or BCM_E_NONE for Strata
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_init(int unit)
{
    int rv;

    MCAST_OUT(("MCAST %d: Init\n", unit));

    rv = mbcm_driver[unit]->mbcm_mcast_init(unit);
    if (rv < 0) {
        return rv;
    }

    _bcm_mcast_init[unit] = TRUE;

    return rv;
}

/*
 * Function:
 *      bcm_esw_mcast_port_add
 * Purpose:
 *      Add ports to an existing entry in the multicast table
 * Parameters:
 *      unit - unit number.
 *	mcaddr - Pointer to fully filled-in bcm_mcast_addr_t
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);

    MCAST_OUT(("MCAST %d: port add %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n",
               unit, mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2],
               mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5],
               mcaddr->vid));
    MCAST_VERB(("        : l2 idx %u, cos dest %d, ports 0x%x, ut 0x%x\n",
                mcaddr->l2mc_index, mcaddr->cos_dst,
                SOC_PBMP_WORD_GET(mcaddr->pbmp, 0),
                SOC_PBMP_WORD_GET(mcaddr->ubmp, 0)));

    /* Remove inactive stack ports; all from untagged */
    BCM_PBMP_REMOVE(mcaddr->pbmp, SOC_PBMP_STACK_INACTIVE(unit));
    BCM_PBMP_REMOVE(mcaddr->ubmp, SOC_PBMP_STACK_CURRENT(unit));
    return mbcm_driver[unit]->mbcm_mcast_port_add(unit, mcaddr);
}

/*
 * Function:
 *      bcm_esw_mcast_port_remove
 * Purpose:
 *      Remove ports from an existing entry in the multicast table
 * Parameters:
 *      unit - unit number.
 *	mcaddr - Pointer to fully filled-in bcm_mcast_addr_t
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);

    MCAST_OUT(("MCAST %d: port remove %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n",
               unit, mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2],
               mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5],
               mcaddr->vid));
    MCAST_VERB(("        : l2 idx %u, cos dest %d, ports 0x%x, ut 0x%x\n",
                mcaddr->l2mc_index, mcaddr->cos_dst,
                SOC_PBMP_WORD_GET(mcaddr->pbmp, 0),
                SOC_PBMP_WORD_GET(mcaddr->ubmp, 0)));

    /* Remove stack ports */
    BCM_PBMP_REMOVE(mcaddr->pbmp, SOC_PBMP_STACK_CURRENT(unit));
    BCM_PBMP_REMOVE(mcaddr->ubmp, SOC_PBMP_STACK_CURRENT(unit));
    return mbcm_driver[unit]->mbcm_mcast_port_remove(unit, mcaddr);
}

/* Add stack ports */

STATIC void
_esw_mcast_addr_update(int unit, bcm_mcast_addr_t *mcaddr)
{
    bcm_pbmp_t active;

    SOC_PBMP_STACK_ACTIVE_GET(unit, active);
    BCM_PBMP_OR(mcaddr->pbmp, active);
    BCM_PBMP_REMOVE(mcaddr->ubmp, SOC_PBMP_STACK_CURRENT(unit));
}

/*
 * Function:
 *      bcm_esw_mcast_addr_add_w_l2mcindex
 * Purpose:
 *      Add an entry in the multicast table with l2mc index provided
 * Parameters:
 *      unit - unit number.
 *	mcaddr - Pointer to fully filled-in bcm_mcast_addr_t
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_addr_add_w_l2mcindex(int unit, bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);


    MCAST_OUT(("MCAST %d: addr add w/ idx. %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n",
               unit, mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2],
               mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5],
               mcaddr->vid));
    MCAST_VERB(("        : l2 idx %u, cos dest %d, ports 0x%x, ut 0x%x\n",
                mcaddr->l2mc_index, mcaddr->cos_dst,
                SOC_PBMP_WORD_GET(mcaddr->pbmp, 0),
                SOC_PBMP_WORD_GET(mcaddr->ubmp, 0)));
    _esw_mcast_addr_update(unit, mcaddr);
    return mbcm_driver[unit]->mbcm_mcast_addr_add_w_l2mcindex(unit, mcaddr);
}


/*
 * Function:
 *      bcm_esw_mcast_addr_add
 * Purpose:
 *      Add an entry in the multicast table
 * Parameters:
 *      unit - StrataSwitch unit number.
 *	mcaddr - Pointer to fully filled-in bcm_mcast_addr_t
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL if table write failed.
 */

int
bcm_esw_mcast_addr_add(int unit, bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);

    _esw_mcast_addr_update(unit, mcaddr);
    MCAST_OUT(("MCAST %d: addr add %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n",
               unit, mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2],
               mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5],
               mcaddr->vid));
    MCAST_VERB(("        : l2 idx %u, cos dest %d, ports 0x%x, ut 0x%x\n",
                mcaddr->l2mc_index, mcaddr->cos_dst,
                SOC_PBMP_WORD_GET(mcaddr->pbmp, 0),
                SOC_PBMP_WORD_GET(mcaddr->ubmp, 0)));

    return mbcm_driver[unit]->mbcm_mcast_addr_add(unit, mcaddr);
}

/*
 * Function:
 *      bcm_esw_mcast_addr_remove_w_l2mcindex
 * Purpose:
 *      Delete an entry from the multicast table and provide l2mcindex in
 *      bcm_mcast_addr_t of the deleted entry
 * Parameters:
 *      unit - unit number.
 *      mcaddr - Pointer to  filled-in bcm_mcast_addr_t with mac addr and vid
 *               l2mc_index is provided as output
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXXX
 */

int
bcm_esw_mcast_addr_remove_w_l2mcindex(int unit, bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);

    MCAST_OUT(("MCAST %d: addr rmv w/ idx %d\n", unit, mcaddr->l2mc_index));

    return mbcm_driver[unit]->mbcm_mcast_addr_remove_w_l2mcindex(unit,
								 mcaddr);
}

/*
 * Function:
 *      bcm_esw_mcast_addr_remove
 * Purpose:
 *      Delete an entry from the multicast table
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mac  - mac address
 *      vid  - vlan id
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXXX
 */

int
bcm_esw_mcast_addr_remove(int unit, mac_addr_t mac, bcm_vlan_t vid)
{
    MCAST_INIT_RETURN(unit);

    MCAST_OUT(("MCAST %d: addr remove %2x:%2x:%2x:%2x:%2x:%2x, vid %d\n",
               unit, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vid));

    return mbcm_driver[unit]->mbcm_mcast_addr_remove(unit, mac, vid);
}

/*
 * Function:
 *      bcm_esw_mcast_port_get
 * Purpose:
 *      Get membership and untagged port bit maps for a MC group
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      mac - mac address used for lookup.
 *      vid - vlan id used for lookup.
 *	mcaddr - pointer to bcm_mcast_addr_t that is fully filled in
 * Returns:
 *      BCM_E_NONE		Success (mcaddr filled in)
 *      BCM_E_INTERNAL		Chip access failure
 *	BCM_E_NOT_FOUND	Address not found (mcaddr not filled in)
 */

int
bcm_esw_mcast_port_get(int unit,
		   mac_addr_t mac, bcm_vlan_t vid,
		   bcm_mcast_addr_t *mcaddr)
{
    MCAST_INIT_RETURN(unit);

    return mbcm_driver[unit]->mbcm_mcast_port_get(unit, mac, vid, mcaddr);
}

/*
 * Function:
 *	bcm_esw_mcast_leave
 * Purpose:
 *	This function removes the given port from the group membership
 *	of the given mcast mac address.
 * Parameters:
 *	unit    - device number
 *	mac	- Multicast MAC Address
 *	vlan    - Vlan ID
 *	port    - port to be removed from the given multicast address
 * Returns:
 *	BCM_MCAST_LEAVE_DELETED - If the entry with the given mcast
 *                                        addr is deleted from H/W table
 *	BCM_MCAST_LEAVE_UPDATED - If the mcast address is found in
 *                                        the table and bitmap is updated.
 *	BCM_MCAST_LEAVE_NOTFOUND - If the mcast address is not found in
 *                                         the table
 *	BCM_E_XXX		- an error occurred
 * Notes:
 *	BCM_MCAST_LEAVE_XXX  values are non-negative success codes
 */

int
bcm_esw_mcast_leave(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_port_t port)
{
    int                 rv;
    bcm_mcast_addr_t	mcaddr;
    bcm_mcast_addr_t	rtaddr;

    MCAST_INIT_RETURN(unit);

    bcm_mcast_addr_t_init(&mcaddr, mac, vlan);
    BCM_PBMP_PORT_SET(mcaddr.pbmp, port);
    BCM_IF_ERROR_RETURN(bcm_esw_mcast_port_remove(unit, &mcaddr));
    BCM_IF_ERROR_RETURN(bcm_esw_mcast_port_get(unit, mac, vlan, &mcaddr));

    if (SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit)) {
        rv = bcm_esw_mcast_port_get(unit, (uint8 *)_soc_mac_all_routers, vlan,
                                    &rtaddr);
        if (rv < 0) {
            bcm_mcast_addr_t_init(&rtaddr, (uint8 *)_soc_mac_all_routers, vlan);
        }
        BCM_PBMP_REMOVE(mcaddr.pbmp, rtaddr.pbmp);
    }

    
    if (BCM_PBMP_IS_NULL(mcaddr.pbmp)) {
	BCM_IF_ERROR_RETURN(bcm_esw_mcast_addr_remove(unit, mac, vlan));
        return BCM_MCAST_LEAVE_DELETED;
    }
    return BCM_MCAST_LEAVE_UPDATED;
}

/*
 * Function:
 *	bcm_esw_mcast_join
 * Purpose:
 *	This function adds the given port to the membership of the
 *	given mcast mac address.
 * Parameters:
 *	unit	- device number
 *	mac	- Multicast MAC Address
 *	vlan    - Vlan ID
 *	port    - port to be added to the mulicast address
 *	mcaddr	- (OUT) If not NULL, returns the updated mcast_addr_t for
 *		  the given multicast address
 *	all_router_pbmp - (OUT) If not NULL, returns the ports associated with
 *			the all routers multicast address
 *
 * Returns:
 *	BCM_MCAST_JOIN_ADDED - If a new entry is created for the
 *                                     given mcast address.
 *	BCM_MCAST_JOIN_UPDATED - If the mcast address is found in
 *                                       the table and bitmap is updated.
 *	BCM_E_XXX		- an error occurred
 * Notes:
 *	BCM_MCAST_JOIN_XXX values are non-negative success codes
 */

int
bcm_esw_mcast_join(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_port_t port,
	       bcm_mcast_addr_t *mcaddr,
	       bcm_pbmp_t *all_router_pbmp)
{
    int                 rv;
    bcm_mcast_addr_t	newmc;

    MCAST_INIT_RETURN(unit);

    /*
     * Fill in all_routers port bitmap
     */
    if (all_router_pbmp != NULL) {
	rv = bcm_esw_mcast_port_get(unit, (uint8 *)_soc_mac_all_routers, vlan,
                                    &newmc);
        if (rv == BCM_E_NOT_FOUND) {
	    BCM_PBMP_CLEAR(*all_router_pbmp);
        } else if (rv < 0) {
            return rv;
        } else {
	    BCM_PBMP_ASSIGN(*all_router_pbmp, newmc.pbmp);
        }
    }

    /*
     * Now, join the actual multicast group
     */
    bcm_mcast_addr_t_init(&newmc, mac, vlan);
    BCM_PBMP_PORT_SET(newmc.pbmp, port);
    rv = bcm_esw_mcast_port_add(unit, &newmc);

    if (rv == BCM_E_NOT_FOUND) {
	BCM_IF_ERROR_RETURN(bcm_esw_mcast_addr_add(unit, &newmc));
        rv = BCM_MCAST_JOIN_ADDED;
    } else if (rv < 0) {
	return rv;
    } else {
	rv = BCM_MCAST_JOIN_UPDATED;
    }
    if (mcaddr != NULL) {
	BCM_IF_ERROR_RETURN(bcm_esw_mcast_port_get(unit, mac, vlan, mcaddr));
    }
    return rv;
}

/*
 * Function:	bcm_mcast_bitmap_max_get
 * Purpose:	provides maximum mcast index that this fabric can handle
 * Parameters:	unit		- device
 *		max_index	- (OUT) returned number of entries
 * Returns:	BCM_E_*
 * Note:
 * 	Fabric only
 */
int
bcm_esw_mcast_bitmap_max_get(int unit, int *max_index)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
	*max_index = soc_mem_index_count(unit, MEM_MCm);
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int	mc_base, mc_size;

	SOC_IF_ERROR_RETURN
	    (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
	*max_index = mc_size;
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    *max_index = 0;
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_mcast_bitmap_set
 * Purpose:
 *     Set the L2 MC forwarding port bitmap
 * Parameters:
 *     unit     - SOC unit #
 *     index    - the MC index carried in HiGig header
 *     in_port  - ingress port number
 *     pbmp     - IPMC forwarding port bitmap
 * Returns:
 *     BCM_E_XXX
 * Note:
 *	Fabric only
 */
int
bcm_esw_mcast_bitmap_set(int unit, int index, bcm_port_t in_port, bcm_pbmp_t pbmp)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
	mem_mc_entry_t      entry;
	int                 blk, rv;

	if (!SOC_PORT_VALID(unit, in_port)) {
	    return BCM_E_PORT;
	}
	if (index < 0 || index > soc_mem_index_max(unit, MEM_MCm)) {
	    return BCM_E_PARAM;
	}

	/* Make sure packet never goes back */
	BCM_PBMP_PORT_REMOVE(pbmp, in_port);

	blk = SOC_PORT_BLOCK(unit, in_port);

	sal_memset(&entry, 0, sizeof(entry));

	soc_mem_lock(unit, MEM_MCm);
	rv = READ_MEM_MCm(unit, blk, index, &entry);
	if (rv >= 0) {
	    soc_MEM_MCm_field32_set(unit, &entry, MCBITMAPf,
				    SOC_PBMP_WORD_GET(pbmp, 0));
	    rv = WRITE_MEM_MCm(unit, blk, index, &entry);
	}
	soc_mem_unlock(unit, MEM_MCm);

	return rv;
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int		rv, mc_base, mc_size;
	l2mc_entry_t	l2mc;

	COMPILER_REFERENCE(in_port);

	SOC_IF_ERROR_RETURN
	    (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
	if (index < 0 || index > mc_size) {
	    return BCM_E_PARAM;
	}
	index += mc_base;

	soc_mem_lock(unit, L2MCm);
        rv = READ_L2MCm(unit, MEM_BLOCK_ANY, index, &l2mc);
	if (rv >= 0) {
	    soc_mem_field32_set(unit, L2MCm, &l2mc, PORT_BITMAPf,
				SOC_PBMP_WORD_GET(pbmp, 0));
	    soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
	    rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, index, &l2mc);
	}
	soc_mem_unlock(unit, L2MCm);
	return rv;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_mcast_bitmap_get
 * Purpose:
 *     Get the L2 MC forwarding port bitmap
 * Parameters:
 *     unit    - SOC unit #
 *     index   - the L2 MC index carried in HiGig header
 *     in_port - ingress port number
 *     pbmp    - IPMC forwarding port bitmap
 * Returns:
 *     BCM_E_xxxx
 * Note:
 * 	Fabric only
 */
int
bcm_esw_mcast_bitmap_get(int unit, int index, bcm_port_t in_port,
		     bcm_pbmp_t *pbmp)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT

    if (SOC_IS_XGS12_FABRIC(unit)) {
	mem_mc_entry_t	entry;
	int		blk;
	uint32		pword;

	if (!SOC_PORT_VALID(unit, in_port)) {
	    return BCM_E_PORT;
	}
	if (index < 0 || index > soc_mem_index_max(unit, MEM_MCm)) {
	    return BCM_E_PARAM;
	}

	blk = SOC_PORT_BLOCK(unit, in_port);
	SOC_IF_ERROR_RETURN(READ_MEM_MCm(unit, blk, index, &entry));
	pword = soc_MEM_MCm_field32_get(unit, &entry, MCBITMAPf);
	SOC_PBMP_CLEAR(*pbmp);
	SOC_PBMP_WORD_SET(*pbmp, 0, pword);
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int		mc_base, mc_size;
	uint32		pword;
	l2mc_entry_t	l2mc;

	COMPILER_REFERENCE(in_port);

	SOC_IF_ERROR_RETURN
	    (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
	if (index < 0 || index > mc_size) {
	    return BCM_E_PARAM;
	}
	index += mc_base;

        SOC_IF_ERROR_RETURN(READ_L2MCm(unit, MEM_BLOCK_ANY, index, &l2mc));
	pword = soc_mem_field32_get(unit, L2MCm, &l2mc, PORT_BITMAPf);
	SOC_PBMP_CLEAR(*pbmp);
	SOC_PBMP_WORD_SET(*pbmp, 0, pword);
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_mcast_bitmap_del
 * Purpose:
 *     Remove L2 MC forwarding port bitmap
 * Parameters:
 *     unit     - SOC unit #
 *     index    - the L2 MC index carried in HiGig header
 *     in_port  - ingress port number
 *     pbmp     - IPMC forwarding port bitmap
 * Returns:
 *     BCM_E_xxxx
 * Note:
 * 	Fabric only
 */
int
bcm_esw_mcast_bitmap_del(int unit, int index, bcm_port_t in_port, bcm_pbmp_t pbmp)
{
#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
	mem_mc_entry_t      entry;
	uint32              pword;
	int                 rv, blk;

	if (!SOC_PORT_VALID(unit, in_port)) {
	    return BCM_E_PORT;
	}
	if (index < 0 || index > soc_mem_index_max(unit, MEM_MCm)) {
	    return BCM_E_PARAM;
	}

	blk = SOC_PORT_BLOCK(unit, in_port);

	soc_mem_lock(unit, MEM_MCm);

	rv = READ_MEM_MCm(unit, blk, index, &entry);
	if (rv >= 0) {
	    pword = soc_MEM_MCm_field32_get(unit, &entry, MCBITMAPf);
	    pword &= ~SOC_PBMP_WORD_GET(pbmp, 0);
	    soc_MEM_MCm_field32_set(unit, &entry, MCBITMAPf, pword);
	    rv = WRITE_MEM_MCm(unit, blk, index, &entry);
	}

	soc_mem_unlock(unit, MEM_MCm);

	return rv;
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int		rv, mc_base, mc_size;
	uint32          pword;
	l2mc_entry_t	l2mc;

	COMPILER_REFERENCE(in_port);

	SOC_IF_ERROR_RETURN
	    (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
	if (index < 0 || index > mc_size) {
	    return BCM_E_PARAM;
	}
	index += mc_base;

	soc_mem_lock(unit, L2MCm);
        rv = READ_L2MCm(unit, MEM_BLOCK_ANY, index, &l2mc);
	if (rv >= 0) {
	    pword = soc_mem_field32_get(unit, L2MCm, &l2mc, PORT_BITMAPf);
	    pword &= ~SOC_PBMP_WORD_GET(pbmp, 0);
	    soc_mem_field32_set(unit, L2MCm, &l2mc, PORT_BITMAPf, pword);
	    soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
	    rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, index, &l2mc);
	}
	soc_mem_unlock(unit, L2MCm);
	return rv;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

STATIC int
_xgs12_mcast_stk_update(int unit, bcm_pbmp_t add_ports,
                      bcm_pbmp_t remove_ports)
{
    int rv = BCM_E_NONE;  /* Ignore if unavailable */

#if defined(BCM_XGS12_SWITCH_SUPPORT)
    int i;
    bcm_pbmp_t new_ports, old_ports;
    l2x_mc_entry_t l2mc;
    int changed = 0;

    soc_mem_lock(unit, L2X_MCm);
    for (i = soc_mem_index_min(unit, L2X_MCm);
             i <= soc_mem_index_max(unit, L2X_MCm);
             i++) {

        rv = soc_mem_read(unit, L2X_MCm, MEM_BLOCK_ANY, i, &l2mc);
        if (rv < 0) {
            break;
        }
        if (soc_L2X_MCm_field32_get(unit, &l2mc, VALIDf) == 0) {
            continue;
        }
        _bcm_l2mcentry_pbmp_get(unit, &l2mc, &old_ports);
        BCM_PBMP_ASSIGN(new_ports, old_ports);
        BCM_PBMP_OR(new_ports, add_ports);
        BCM_PBMP_REMOVE(new_ports, remove_ports);
        if (BCM_PBMP_NEQ(new_ports, old_ports)) {
            ++changed;
            _bcm_l2mcentry_pbmp_set(unit, &l2mc, new_ports);
            rv = soc_mem_write(unit, L2X_MCm, MEM_BLOCK_ALL, i, &l2mc);
            if (rv < 0) {
                break;
            }
        }
    }
    soc_mem_unlock(unit, L2X_MCm);

    if (changed) {
        MCAST_VERB(("L2MC %d: xgs stk update changed %d entries\n",
                    unit, changed));
    }
#endif

    return rv;
}

STATIC int
_xgs3_fb_mcast_stk_update(int unit, bcm_pbmp_t add_ports,
                       bcm_pbmp_t remove_ports)
{
    int rv = BCM_E_NONE;  /* Ignore if unavailable */

#if defined(BCM_FIREBOLT_SUPPORT)
    int i;
    bcm_pbmp_t new_ports, old_ports;
    l2mc_entry_t l2mc;
    int changed = 0;

    soc_mem_lock(unit, L2MCm);
    for (i = soc_mem_index_min(unit, L2MCm);
             i <= soc_mem_index_max(unit, L2MCm);
             i++) {

        rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, i, &l2mc);
        if (rv < 0) {
            break;
        }
        if (soc_L2MCm_field32_get(unit, &l2mc, VALIDf) == 0) {
            continue;
        }
        soc_mem_pbmp_field_get(unit, L2MCm, &l2mc, PORT_BITMAPf, &old_ports);
        BCM_PBMP_ASSIGN(new_ports, old_ports);
        BCM_PBMP_OR(new_ports, add_ports);
        BCM_PBMP_REMOVE(new_ports, remove_ports);
        if (BCM_PBMP_NEQ(new_ports, old_ports)) {
            ++changed;
            soc_mem_pbmp_field_set(unit, L2MCm, &l2mc, PORT_BITMAPf,
                                   &new_ports);
            rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, i, &l2mc);
            if (rv < 0) {
                break;
            }
        }
    }
    soc_mem_unlock(unit, L2MCm);

    if (changed) {
        MCAST_VERB(("L2MC %d: xgs stk update changed %d entries\n",
                    unit, changed));
    }
#endif

    return rv;
}

STATIC int
_xgs3_er_mcast_stk_update(int unit, bcm_pbmp_t add_ports,
                       bcm_pbmp_t remove_ports)
{
    int rv = BCM_E_NONE;  /* Ignore if unavailable */

#if defined(BCM_EASYRIDER_SUPPORT)
    int i;
    bcm_pbmp_t new_ports, old_ports;
    l2mc_table_entry_t l2mc;
    int changed = 0;
    uint32 pbm_field;

    soc_mem_lock(unit, L2MC_TABLEm);
    for (i = soc_mem_index_min(unit, L2MC_TABLEm);
             i <= soc_mem_index_max(unit, L2MC_TABLEm);
             i++) {

        rv = soc_mem_read(unit, L2MC_TABLEm, MEM_BLOCK_ANY, i, &l2mc);
        if (rv < 0) {
            break;
        }
        if (soc_L2MC_TABLEm_field32_get(unit, &l2mc, VALIDf) == 0) {
            continue;
        }
        BCM_PBMP_CLEAR(old_ports);
        SOC_PBMP_WORD_SET(old_ports, 0,
                          soc_L2MC_TABLEm_field32_get(unit, &l2mc,
                                                      PORT_BITMAPf));
        BCM_PBMP_ASSIGN(new_ports, old_ports);
        BCM_PBMP_OR(new_ports, add_ports);
        BCM_PBMP_REMOVE(new_ports, remove_ports);
        if (BCM_PBMP_NEQ(new_ports, old_ports)) {
            ++changed;
            pbm_field = SOC_PBMP_WORD_GET(new_ports, 0);
            soc_L2MC_TABLEm_field32_set(unit, &l2mc, PORT_BITMAPf, pbm_field);
            rv = soc_mem_write(unit, L2MC_TABLEm, MEM_BLOCK_ALL, i, &l2mc);
            if (rv < 0) {
                break;
            }
        }
    }
    soc_mem_unlock(unit, L2MC_TABLEm);

    if (changed) {
        MCAST_VERB(("L2MC %d: xgs stk update changed %d entries\n",
                    unit, changed));
    }
#endif

    return rv;
}


/*
 * Function:
 *      _bcm_esw_mcast_stk_update
 * Purpose:
 *      Update port bitmaps after stack change
 * Parameters:
 *      
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */


int
_bcm_esw_mcast_stk_update(int unit, bcm_port_t port, uint32 flags)
{
    bcm_pbmp_t add_ports, remove_ports;
    int rv = BCM_E_NONE;

    MCAST_DEBUG(BCM_DBG_STK,
                ("MCast %d: stk_update port %d flags 0x%x\n",
                 unit, port, flags));

    /* Calculate ports that should and should not be in bitmap */
    SOC_PBMP_ASSIGN(add_ports, SOC_PBMP_STACK_CURRENT(unit));
    SOC_PBMP_REMOVE(add_ports, SOC_PBMP_STACK_INACTIVE(unit));

    /* Remove ports no longer stacking, or explicitly inactive */
    SOC_PBMP_ASSIGN(remove_ports, SOC_PBMP_STACK_PREVIOUS(unit));
    SOC_PBMP_REMOVE(remove_ports, SOC_PBMP_STACK_CURRENT(unit));
    SOC_PBMP_OR(remove_ports, SOC_PBMP_STACK_INACTIVE(unit));

    if (SOC_IS_XGS12_SWITCH(unit)) {
        rv = _xgs12_mcast_stk_update(unit, add_ports, remove_ports);
    } else if (SOC_IS_FB_FX_HX(unit)) {
        rv = _xgs3_fb_mcast_stk_update(unit, add_ports, remove_ports);
    } else if (SOC_IS_EASYRIDER(unit)) {
        rv = _xgs3_er_mcast_stk_update(unit, add_ports, remove_ports);
    } else { /* May update fabric in the future, but not needed yet */
        rv = BCM_E_NONE; /* Ignore if unavailable */
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_mcast_cleanup(int unit)
{
    int rv = BCM_E_NONE;
  	 
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_EASYRIDER(unit)) {
        rv = _bcm_xgs3_mcast_detach(unit);
    }
#endif
    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_mcast_sw_dump
 * Purpose:
 *     Displays mcast information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_mcast_sw_dump(int unit)
{

    soc_cm_print("\nSW Information MCAST - Unit %d\n", unit);
    soc_cm_print("    Init : %d\n", _bcm_mcast_init[unit]);

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_EASYRIDER(unit)) {
        _bcm_xgs3_mcast_sw_dump(unit);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    soc_cm_print("\n");

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
