/*
 * $Id: triumph_multicast.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:    multicast.c
 * Purpose: Manages multicast functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/triumph.h>

#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/trx.h>

#include <bcm_int/esw_dispatch.h>

/*
 * Function:
 *      _bcm_trx_multicast_ipmc_write
 * Purpose:
 *      Write L3 multicast distribuition ports.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   IPMC index.
 *      l2_pbmp    - (IN)   L2 distribution ports.
 *      l3_pbmp    - (IN)   L3 distribuition port. 
 *      valid      - (IN)   Distribuition is valid bit.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_trx_multicast_ipmc_write(int unit, int ipmc_id, bcm_pbmp_t l2_pbmp,
                              bcm_pbmp_t l3_pbmp, int valid)
{
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];  /* hw entry bufffer. */
    int rv;

    sal_memset(&entry, 0, sizeof(ipmc_entry_t));

    /* Currently there is no need to invalidate distribution. */
    if (0 == valid) {
        BCM_PBMP_CLEAR(l2_pbmp);
        BCM_PBMP_CLEAR(l3_pbmp);
    }

    /* Table index sanity check. */
    if ((ipmc_id < soc_mem_index_min(unit, L3_IPMCm)) ||
        (ipmc_id > soc_mem_index_max(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    /* Read / Modify / Write section. */
    soc_mem_lock(unit, L3_IPMCm);
    rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, ipmc_id, entry);
    if (BCM_SUCCESS(rv)) {
        if (0  == soc_mem_field32_get(unit, L3_IPMCm, entry, VALIDf)) {
            /* Invalid entry, flush */
            sal_memset(&entry, 0, sizeof(ipmc_entry_t));
        }
        soc_mem_field32_set(unit, L3_IPMCm, entry, VALIDf, valid);
        soc_mem_pbmp_field_set(unit, L3_IPMCm, entry, L2_BITMAPf, &l2_pbmp);
        soc_mem_pbmp_field_set(unit, L3_IPMCm, entry, L3_BITMAPf, &l3_pbmp);
        rv = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, ipmc_id, &entry);
    }
    soc_mem_unlock(unit, L3_IPMCm);

    return rv;
}

/*
 * Function:
 *      _bcm_trx_multicast_ipmc_read
 * Purpose:
 *      Read L3 multicast distribuition ports.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   IPMC index.
 *      l2_pbmp    - (IN)   L2 distribution ports.
 *      l3_pbmp    - (IN)   L3 distribuition port. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_multicast_ipmc_read(int unit, int ipmc_id, bcm_pbmp_t *l2_pbmp,
                             bcm_pbmp_t *l3_pbmp)
{
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];  /* hw entry bufffer. */

    /* Input parameters check. */
    if ((NULL == l2_pbmp) || (NULL == l3_pbmp)) {
        return (BCM_E_PARAM);
    }

    /* Table index sanity check. */
    if ((ipmc_id < soc_mem_index_min(unit, L3_IPMCm)) ||
        (ipmc_id > soc_mem_index_max(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    /* Read L3 ipmc table. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY,
                                      ipmc_id, entry));

    /* If entry is invalid - clear L2 & L3 bitmaps. */
    if (0  == soc_mem_field32_get(unit, L3_IPMCm, entry, VALIDf)) {
        BCM_PBMP_CLEAR(*l2_pbmp);
        BCM_PBMP_CLEAR(*l3_pbmp);
        return (BCM_E_NONE);
    }

    /* Extract L2 & L3 bitmaps. */
    soc_mem_pbmp_field_get(unit, L3_IPMCm, entry, L2_BITMAPf, l2_pbmp);
    soc_mem_pbmp_field_get(unit, L3_IPMCm, entry, L3_BITMAPf, l3_pbmp);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_multicast_create
 * Purpose:
 *      Allocate a multicast group index
 * Parameters:
 *      unit       - (IN)   Device Number
 *      flags      - (IN)   BCM_MULTICAST_*
 *      group      - (OUT)  Group ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_multicast_create(int unit, uint32 flags, bcm_multicast_t *group)
{
    int type, mc_index, rv = BCM_E_NONE;
    egr_ipmc_entry_t egr_ipmc;
    bcm_pbmp_t active, l2_pbmp, l3_pbmp;
    l2mc_entry_t l2mc_entry;

    type = flags & BCM_MULTICAST_TYPE_MASK;
    if ((type != BCM_MULTICAST_TYPE_L2) &&
        (type != BCM_MULTICAST_TYPE_L3) &&
        (type != BCM_MULTICAST_TYPE_VPLS) &&
        (type != BCM_MULTICAST_TYPE_MIM) &&
        (type != BCM_MULTICAST_TYPE_SUBPORT)) {
        return BCM_E_PARAM;
    }

    if (SOC_IS_SC_CQ(unit) && ((type == BCM_MULTICAST_TYPE_MIM) || 
               (type == BCM_MULTICAST_TYPE_VPLS))) {
        /* Scorpion does not support VPLS type */
        return BCM_E_UNAVAIL;
    }

    if (type == BCM_MULTICAST_TYPE_L2) {
        if (flags & BCM_MULTICAST_WITH_ID) {
            mc_index = _BCM_MULTICAST_ID_GET(*group);
            if ((mc_index < 0) ||
                (mc_index >= soc_mem_index_count(unit, L2MCm))) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_id_alloc(unit, mc_index));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_free_index(unit, &mc_index));
            _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, mc_index);
        }
        sal_memset(&l2mc_entry, 0, sizeof(l2mc_entry));
        soc_mem_field32_set(unit, L2MCm, &l2mc_entry, VALIDf, 1);
        rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, mc_index, &l2mc_entry);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        if (flags & BCM_MULTICAST_WITH_ID) {
            mc_index = _BCM_MULTICAST_ID_GET(*group);
            if ((mc_index < 0) ||
                (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
                return BCM_E_PARAM;
            }
            if (SOC_IS_SC_CQ(unit)) {
                rv = bcm_xgs3_ipmc_id_alloc(unit, mc_index);
            } else if (SOC_IS_TR_VL(unit)){
                rv = _bcm_tr_ipmc_id_alloc(unit, mc_index);
            } else {
                return (BCM_E_INTERNAL);
            }
            if (rv == BCM_E_FULL) {
                return BCM_E_EXISTS;
            }
        } else {
            /* Allocate an IPMC index */
            if (SOC_IS_SC_CQ(unit)) {
                rv = bcm_xgs3_ipmc_create(unit, &mc_index);
            } else if (SOC_IS_TR_VL(unit)) { 
                rv = _bcm_tr_ipmc_create(unit, &mc_index);
            } else {
                return (BCM_E_INTERNAL);
            }
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Commit the values to HW */
        sal_memset(&egr_ipmc, 0, sizeof(egr_ipmc));
        if (soc_mem_field_valid(unit, EGR_IPMCm, L3_PAYLOADf)) {
            soc_EGR_IPMCm_field32_set(unit, &egr_ipmc, L3_PAYLOADf, 0);
        }
        if ((type == BCM_MULTICAST_TYPE_VPLS) || 
            (type == BCM_MULTICAST_TYPE_MIM) || 
            soc_feature(unit, soc_feature_subport_enhanced)) {
            if (soc_mem_field_valid(unit, EGR_IPMCm, REPLICATION_TYPEf)) {
                soc_EGR_IPMCm_field32_set(unit, &egr_ipmc, REPLICATION_TYPEf,
                                          0x1); /* Next-hop indexes from MMU */
            }
            soc_EGR_IPMCm_field32_set(unit, &egr_ipmc, DONT_PRUNE_VLANf, 1);
        } else if (type == BCM_MULTICAST_TYPE_SUBPORT) {
            soc_EGR_IPMCm_field32_set(unit, &egr_ipmc, DONT_PRUNE_VLANf, 1);
        }
        rv = WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL, mc_index, &egr_ipmc);
        if (rv != BCM_E_NONE) {
            if (SOC_IS_SC_CQ(unit)) {
                bcm_xgs3_ipmc_id_free(unit, mc_index);
            } else if (SOC_IS_TR_VL(unit)) { 
                (void) _bcm_tr_ipmc_id_free(unit, mc_index);
            }
            return rv;
        }
    
        if ((type == BCM_MULTICAST_TYPE_VPLS) || (type == BCM_MULTICAST_TYPE_MIM) ||
            (type == BCM_MULTICAST_TYPE_SUBPORT)) {
            /* Add stack ports to L2 PBMP */
            BCM_PBMP_CLEAR(l2_pbmp);
            BCM_PBMP_CLEAR(l3_pbmp);
            SOC_PBMP_STACK_ACTIVE_GET(unit, active);
            BCM_PBMP_OR(l2_pbmp, active);
            BCM_PBMP_REMOVE(l2_pbmp, SOC_PBMP_STACK_INACTIVE(unit));
            
             rv = _bcm_trx_multicast_ipmc_write(unit, mc_index, l2_pbmp,
                                                l3_pbmp, TRUE);
            if (rv != BCM_E_NONE) {
                sal_memset(&egr_ipmc, 0, sizeof(egr_ipmc));
                WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL, mc_index, &egr_ipmc);
                if (SOC_IS_TR_VL(unit)) { 
                    (void) _bcm_tr_ipmc_id_free(unit, mc_index);
                } else if (SOC_IS_SC_CQ(unit)) {
                    (void) bcm_xgs3_ipmc_id_free(unit, mc_index);
                }
                return rv;
            }
        }
        if (type == BCM_MULTICAST_TYPE_L3) {
            _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3, mc_index);
        } else if (type == BCM_MULTICAST_TYPE_VPLS) {
            _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VPLS, mc_index);
        } else if (type == BCM_MULTICAST_TYPE_MIM) {
            _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_MIM, mc_index);
        } else if (type == BCM_MULTICAST_TYPE_SUBPORT) {
            _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_SUBPORT, mc_index);
        }
    } 
    return rv;
}

/*
 * Function:
 *      bcm_multicast_destroy
 * Purpose:
 *      Free a multicast group index
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Group ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_multicast_destroy(int unit, bcm_multicast_t group)
{
    int mc_index, is_l3 = 0, rv = BCM_E_NONE;
    bcm_pbmp_t l2_pbmp, l3_pbmp;
    egr_ipmc_entry_t egr_ipmc;
    bcm_port_t port_iter;

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if (_BCM_MULTICAST_IS_L2(group)) {
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L2MCm))) {
            return BCM_E_PARAM;
        }
        /* Clear the HW entry */
        rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, mc_index,
                           soc_mem_entry_null(unit, L2MCm));
        BCM_IF_ERROR_RETURN(rv);

        /* Free the L2MC index */
        rv = _bcm_xgs3_l2mc_id_free(unit, mc_index);
    } else {
        if (_BCM_MULTICAST_IS_L3(group)) {
            is_l3 = 1;
        } else if (!_BCM_MULTICAST_IS_VPLS(group) && 
                   !_BCM_MULTICAST_IS_MIM(group) &&
                   !_BCM_MULTICAST_IS_SUBPORT(group)) {
            return BCM_E_PARAM;
        }
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
            return BCM_E_PARAM;
        }

        /* Clear the replication set */
        PBMP_E_ITER(unit, port_iter) {
            rv = bcm_fb_ipmc_egress_intf_set(unit, mc_index, port_iter,
                                             0, NULL, is_l3);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Clear the IPMC related tables */
        BCM_PBMP_CLEAR(l2_pbmp);
        BCM_PBMP_CLEAR(l3_pbmp);
        rv = _bcm_trx_multicast_ipmc_write(unit, mc_index, l2_pbmp,
                                                l3_pbmp, FALSE);
        BCM_IF_ERROR_RETURN(rv);

        sal_memset(&egr_ipmc, 0, sizeof(egr_ipmc));
        if (soc_mem_field_valid(unit, EGR_IPMCm, L3_PAYLOADf)) {
            soc_EGR_IPMCm_field32_set(unit, &egr_ipmc, L3_PAYLOADf, 0);
        }
        BCM_IF_ERROR_RETURN (WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL,
                                             mc_index, &egr_ipmc));

        /* Free the IPMC index */
        if (SOC_IS_TR_VL(unit)) {
            rv = _bcm_tr_ipmc_id_free(unit, mc_index);
        } else if (SOC_IS_SC_CQ(unit)) {
            rv = bcm_xgs3_ipmc_id_free(unit, mc_index);
        }
    }
    return rv;
}
#ifdef BCM_TRIUMPH_SUPPORT
/*
 * Function:
 *      bcm_multicast_vpls_encap_get
 * Purpose:
 *      Get the Encap ID for a MPLS port.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      group        - (IN) Multicast group ID.
 *      port         - (IN) Physical port.
 *      mpls_port_id - (IN) MPLS port ID.
 *      encap_id     - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr_multicast_vpls_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t mpls_port_id, bcm_if_t *encap_id)
{
    int vp;
    ing_dvp_table_entry_t dvp;

    if (!BCM_GPORT_IS_MPLS_PORT(mpls_port_id)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id); 
    if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    /* Next-hop index is used for multicast replication */
    *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      bcm_multicast_subport_encap_get
 * Purpose:
 *      Get the Encap ID for a subport.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      port      - (IN) Physical port.
 *      subport   - (IN) Subport ID.
 *      encap_id  - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr_multicast_subport_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                   bcm_gport_t subport, bcm_if_t *encap_id)
{

    if (!BCM_GPORT_IS_SUBPORT_PORT(subport)) {
        return BCM_E_PARAM;
    }

    /* Egress L3 interface is used for multicast replication */
    *encap_id = (bcm_if_t) BCM_GPORT_SUBPORT_PORT_GET(subport);
    if (*encap_id >= BCM_XGS3_L3_IF_TBL_SIZE(unit)) {
        return (BCM_E_PARAM);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_multicast_egress_set
 * Purpose:
 *      Assign the complete set of egress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count   - (IN) Number of ports in replication list
 *      port_array   - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_multicast_egress_set(int unit, bcm_multicast_t group, int port_count,
                            bcm_gport_t *port_array, bcm_if_t *encap_id_array)
{
    int mc_index, i, list_count, is_l3 = 0, is_vpls = 0, rv = BCM_E_NONE;
    int gport_id, *temp_encap_list = NULL;
    bcm_port_t *local_port_array = NULL;
    bcm_module_t mod_out;
    bcm_port_t port_out, port_iter;
    bcm_pbmp_t l2_pbmp, l3_pbmp;
    l2mc_entry_t l2mc_entry; 
    bcm_trunk_t trunk_id;
    soc_mem_t   mem;
    int idx;
    int modid_local=0;
    bcm_port_t  local_trunk_port_array[SOC_MAX_NUM_PORTS][BCM_SWITCH_TRUNK_MAX_PORTCNT];
    int   trunk_local_ports[SOC_MAX_NUM_PORTS];
    bcm_trunk_add_info_t tinfo;

    /* Input parameters check. */
    mc_index = _BCM_MULTICAST_ID_GET(group);
    mem = (_BCM_MULTICAST_IS_L2(group)) ? L2MCm : L3_IPMCm; 
    if ((mc_index < 0) || (mc_index >= soc_mem_index_count(unit, mem))) {
        return (BCM_E_PARAM);
    }

    if (port_count > 0) {
        if (NULL == port_array) {
            return (BCM_E_PARAM);
        }
        if ((0 == _BCM_MULTICAST_IS_L2(group)) && 
            (NULL == encap_id_array)) {
            return (BCM_E_PARAM);
        }
    } else if (port_count < 0) {
        return (BCM_E_PARAM);
    }   

    if ((0 == _BCM_MULTICAST_IS_L2(group)) && 
        (0 == _BCM_MULTICAST_IS_L3(group)) &&
        (0 == _BCM_MULTICAST_IS_VPLS(group)) &&
        (0 == _BCM_MULTICAST_IS_MIM(group)) &&
        (0 == _BCM_MULTICAST_IS_SUBPORT(group))) {
        return (BCM_E_PARAM);
    }

    /* Convert GPORT array into local port numbers */
    if (port_count > 0) {
        local_port_array = sal_alloc(sizeof(bcm_port_t) * port_count, "local_port array");
        if (!local_port_array) {
            return BCM_E_MEMORY;
        }

	  temp_encap_list = sal_alloc(sizeof(int) * port_count, "temp_encap_list");
	  if (NULL == temp_encap_list) {
              sal_free(local_port_array);
		return (BCM_E_MEMORY);
	  }
		
	  for (i=0; i<SOC_MAX_NUM_PORTS; i++) {
		   trunk_local_ports[i] = 0;
		  for (idx=0; idx< BCM_SWITCH_TRUNK_MAX_PORTCNT; idx++) {
			  local_trunk_port_array[i][idx] = 0;
		  }
         }	
    }

    for (i = 0; i < port_count ; i++) {
        if (!SOC_GPORT_IS_LOCAL(port_array[i]) && 
            !SOC_GPORT_IS_MODPORT(port_array[i]) && 
            !SOC_GPORT_IS_TRUNK(port_array[i])) {
            sal_free(local_port_array);
            sal_free(temp_encap_list);
            temp_encap_list    = NULL;			
            return (BCM_E_PORT);
        }

        if (BCM_GPORT_IS_TRUNK(port_array[i])) {
            rv = _bcm_trunk_id_validate(unit, BCM_GPORT_TRUNK_GET(port_array[i]));
            if (BCM_FAILURE(rv)) {
                sal_free(local_port_array);
                sal_free(temp_encap_list);
                temp_encap_list	 = NULL;
                return (BCM_E_PORT);
            }
            rv = bcm_esw_trunk_get(unit, BCM_GPORT_TRUNK_GET(port_array[i]), &tinfo);
            if (BCM_FAILURE(rv)) {
                sal_free(local_port_array);
                sal_free(temp_encap_list);
                temp_encap_list	= NULL;
                return (BCM_E_PORT);
            }

            for (idx=0; idx<tinfo.num_ports; idx++) {
                rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                            tinfo.tm[idx], tinfo.tp[idx], &mod_out, &port_out);
                if (BCM_FAILURE(rv)) {
                    sal_free(local_port_array);
                    sal_free(temp_encap_list);
                    temp_encap_list  = NULL;
                    return (BCM_E_PORT);
                }
                rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
                if (modid_local) {
                    local_trunk_port_array[i][trunk_local_ports[i]] = port_out;
                    (trunk_local_ports[i])++;
                }
            }
        } else {
            rv = _bcm_esw_gport_resolve(unit, port_array[i], &mod_out, 
                                        &port_out, &trunk_id, &gport_id); 
            if (BCM_FAILURE(rv)) {
                sal_free(local_port_array);
                sal_free(temp_encap_list);
                temp_encap_list	= NULL;
                return (rv);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
            if (BCM_FAILURE(rv)) {
                sal_free(local_port_array);
                sal_free(temp_encap_list);
                temp_encap_list	= NULL;
                return (rv);
            }

            if ((0 == IS_E_PORT(unit, port_out)) || 
                (TRUE != modid_local)) {
                sal_free(local_port_array);
                sal_free(temp_encap_list);
                temp_encap_list  = NULL;
                return (BCM_E_PARAM);
            }
            local_port_array[i] = port_out;
        }
    }

    BCM_PBMP_CLEAR(l2_pbmp);
    BCM_PBMP_CLEAR(l3_pbmp);
    if (_BCM_MULTICAST_IS_L2(group)) {
        for (i = 0; i < port_count; i++) {
            BCM_PBMP_PORT_ADD(l2_pbmp, local_port_array[i]);
        }

        /* Update the L2MC port bitmap */
        soc_mem_lock(unit, L2MCm);
        rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, L2MCm);
            if (NULL != local_port_array) sal_free(local_port_array);
            sal_free(temp_encap_list);
            temp_encap_list    = NULL;
			
            return (rv);
        }
        soc_mem_pbmp_field_set(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                               &l2_pbmp);
        rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, mc_index, &l2mc_entry);
        soc_mem_unlock(unit, L2MCm);
    } else {
        if (_BCM_MULTICAST_IS_L3(group)) {
            is_l3 = 1;
        }
        if (0 == port_count) {
            /* Clearing the replication set */
            list_count = 0;		
            PBMP_E_ITER(unit, port_iter) {
                rv = bcm_fb_ipmc_egress_intf_set(unit, mc_index, port_iter,
                                                 list_count, NULL, is_l3);
                BCM_IF_ERROR_RETURN(rv);
            }		
        } else {
            if (_BCM_MULTICAST_IS_VPLS(group)) {
                is_vpls = 1;
            }
            /* 
             * For each front-panel port, walk through the list of GPORTs
             * and collect the ones that match the port.
             */
            PBMP_E_ITER(unit, port_iter) {
                list_count = 0;
                for (i = 0; i < port_count ; i++) {
                   if (BCM_GPORT_IS_TRUNK(port_array[i])) {
                      for (idx=0; idx<trunk_local_ports[i]; idx++) {					   
                          if (local_trunk_port_array[i][idx] != port_iter) {
                               continue;
                          } else {
                               break;
                          }
                      }
                   } else {
                        if (local_port_array[i] != port_iter) {
                            continue;
                        }
                   }
                   if (encap_id_array[i] == BCM_IF_INVALID) {
                       if (!is_vpls) {
                           /* Add port to L2 pbmp */
                           BCM_PBMP_PORT_ADD(l2_pbmp, port_iter);
                       } else {
                           return BCM_E_PARAM;
                       }
                   } else {
                       temp_encap_list[list_count] = encap_id_array[i];
                       list_count++;
                   }
                }
                rv = bcm_fb_ipmc_egress_intf_set(unit, mc_index, port_iter,
                                             list_count, temp_encap_list, is_l3);
                if (BCM_FAILURE(rv)) {
                     sal_free(local_port_array);
                     sal_free(temp_encap_list);
                     temp_encap_list	= NULL;
                     return (rv);
                } 
                if (list_count) {
                    BCM_PBMP_PORT_ADD(l3_pbmp, port_iter);
                }
            }
        }
        rv = _bcm_trx_multicast_ipmc_write(unit, mc_index, l2_pbmp,
                                                l3_pbmp, TRUE);
    }
    if (local_port_array != NULL) {
       sal_free(local_port_array);
    }

    if (temp_encap_list != NULL) {	
	sal_free(temp_encap_list);
    }
    return rv;
}


/* 
 * Function:
 *      _bcm_tr_multicast_egress_get
 * Purpose:
 *      Retrieve a set of egress multicast GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      mc_index   - (IN) Multicast index
 *      port_max   - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of multicast GPORTs 
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of multicasts returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returned list of GPORTS is in GPORT_LOCAL format
 * 
 */
STATIC int     
_bcm_tr_multicast_egress_get(int unit, bcm_multicast_t group, int port_max, 
                             bcm_gport_t *port_array,
                             bcm_if_t *encap_id_array, int *port_count)
{
    int mc_index, i = 0, count, rv = BCM_E_NONE;
    bcm_pbmp_t l2_pbmp, l3_pbmp;
    l2mc_entry_t l2mc_entry; 
    bcm_port_t port_iter;

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if (_BCM_MULTICAST_IS_L2(group)) {
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L2MCm))) {
            return BCM_E_PARAM;
        }

        /* Get the L2MC port bitmap */
        soc_mem_lock(unit, L2MCm);
        rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry);
        soc_mem_unlock(unit, L2MCm);
        BCM_IF_ERROR_RETURN(rv);

        soc_mem_pbmp_field_get(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                               &l2_pbmp);
        PBMP_ITER(l2_pbmp, port_iter) {
            if (i >= port_max) {
                break;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_gport_get(unit, port_iter, (port_array + i)));
            encap_id_array[i] = BCM_IF_INVALID;
            i++;
        }
        *port_count = i;
    } else {
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
            return BCM_E_PARAM;
        }
    
        if (!_BCM_MULTICAST_IS_VPLS(group) &&
            !_BCM_MULTICAST_IS_MIM(group) &&
            !_BCM_MULTICAST_IS_L3(group) &&
            !_BCM_MULTICAST_IS_SUBPORT(group)) {
            return BCM_E_PARAM;
        }
    
        /* Collect list of GPORTs for each front-panel port. */
        *port_count = 0;
        PBMP_E_ITER(unit, port_iter) {
            rv = bcm_fb_ipmc_egress_intf_get(unit, mc_index, port_iter,
                                             port_max - *port_count, 
                                           (int *) &encap_id_array[*port_count], 
                                             &count);
            BCM_IF_ERROR_RETURN(rv);
            for (i = 0; i < count; i++) {
                /* Convert to GPORT values */
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_gport_get(unit, port_iter, 
                                           &(port_array[(*port_count + i)])));
            }
            *port_count += count;
            if (*port_count == port_max) {
                break;
            }
        }
        /* Check for L2 ports */
        BCM_IF_ERROR_RETURN
            (_bcm_trx_multicast_ipmc_read(unit, mc_index, &l2_pbmp,
                                          &l3_pbmp));
        PBMP_ITER(l2_pbmp, port_iter) {
            if (*port_count == port_max) {
                break;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_gport_get(unit, port_iter, 
                                        &(port_array[*port_count])));
            encap_id_array[*port_count] = BCM_IF_INVALID;
            *port_count += 1;
        }        
    }
    return rv;
}

/* 
 * Function:
 *      bcm_multicast_egress_get
 * Purpose:
 *      Retrieve a set of egress multicast GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      mc_index   - (IN) Multicast index
 *      port_max   - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of multicast GPORTs 
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of multicasts returned
 * Returns:
 *      BCM_E_XXX
 */
int     
bcm_tr_multicast_egress_get(int unit, bcm_multicast_t group, int port_max, 
                            bcm_gport_t *port_array, bcm_if_t *encap_id_array,
                            int *port_count)
{
    int i;
    bcm_module_t mod_out, my_modid;
    bcm_port_t port_in, port_out;

    BCM_IF_ERROR_RETURN
        (_bcm_tr_multicast_egress_get(unit, group, port_max, port_array,
                                      encap_id_array, port_count));

    /* Convert GPORT_LOCAL format to GPORT_MODPORT format */
    if (bcm_esw_stk_my_modid_get(unit, &my_modid) < 0) {
        return BCM_E_INTERNAL;
    }
    for (i = 0; i < *port_count; i++) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port_array[i], &port_in));
        
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    my_modid, port_in, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(port_array[i], mod_out, port_out);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_multicast_egress_add
 * Purpose:
 *      Add a GPORT to the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_multicast_egress_add(int unit, bcm_multicast_t group,
                            bcm_gport_t port, bcm_if_t encap_id)
{
    int rv = BCM_E_NONE, is_l3 = 0,idx,trunk_local_ports=0;
    int mc_index, gport_id, port_max = 4096;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    bcm_trunk_add_info_t tinfo;
    bcm_port_t    trunk_member_port[BCM_SWITCH_TRUNK_MAX_PORTCNT];	
    bcm_pbmp_t l2_pbmp, l3_pbmp, old_pbmp, *new_pbmp;
    l2mc_entry_t l2mc_entry;
    int modid_local=0;

    if (!SOC_GPORT_IS_LOCAL(port) && !SOC_GPORT_IS_MODPORT(port) && 
        !SOC_GPORT_IS_TRUNK(port)) {
        return BCM_E_PORT;
    } 

    if (BCM_GPORT_IS_TRUNK(port)) {
        rv = _bcm_trunk_id_validate(unit, BCM_GPORT_TRUNK_GET(port));
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }
        rv = bcm_esw_trunk_get(unit, BCM_GPORT_TRUNK_GET(port), &tinfo);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }

        for (idx=0; idx<tinfo.num_ports; idx++) {
            rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                        tinfo.tm[idx], tinfo.tp[idx],
                                        &mod_out, &port_out);
            if (BCM_FAILURE(rv)) {
                return (BCM_E_PORT);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
            if (modid_local) {
                trunk_member_port[trunk_local_ports++] = port_out;
            }
        }
    } else {
        rv = _bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                                    &trunk_id, &gport_id); 
        BCM_IF_ERROR_RETURN(rv);
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &modid_local));
        if (TRUE != modid_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if (_BCM_MULTICAST_IS_L2(group)) {
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L2MCm))) {
            return BCM_E_PARAM;
        }

        /* Add the port to the L2MC port bitmap */
        rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry);
        if (rv < 0) {
            return rv;
        }
        soc_mem_pbmp_field_get(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                               &l2_pbmp);
        BCM_PBMP_PORT_ADD(l2_pbmp, port_out);
        soc_mem_pbmp_field_set(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                               &l2_pbmp);
    
        rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, mc_index, &l2mc_entry);
    } else {
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
            return BCM_E_PARAM;
        }

        if (_BCM_MULTICAST_IS_L3(group)) {
            is_l3 = 1;
        } else if (!_BCM_MULTICAST_IS_VPLS(group) &&
                   !_BCM_MULTICAST_IS_MIM(group) &&
                   !_BCM_MULTICAST_IS_SUBPORT(group)) {
            return BCM_E_PARAM;
        }

        if (encap_id != BCM_IF_INVALID) {
            if (BCM_GPORT_IS_TRUNK(port)) {
                for (idx=0; idx<trunk_local_ports; idx++) {			
                    rv =  _bcm_fb_ipmc_egress_intf_add(unit, mc_index,
                                  trunk_member_port[idx], encap_id, is_l3);
                    if (rv < 0) {
                        while (idx--) {
                            (void) _bcm_fb_ipmc_egress_intf_delete(unit,
                                           mc_index, trunk_member_port[idx],
                                           port_max, encap_id, is_l3);
                        }
                        return rv;
                    }
             	}
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb_ipmc_egress_intf_add(unit, mc_index, port_out, 
                                                  encap_id, is_l3));
            }
            new_pbmp = &l3_pbmp;
        } else {
            if (!_BCM_MULTICAST_IS_VPLS(group)) {
                /* Updating the L2 bitmap */
                new_pbmp = &l2_pbmp;
            } else {
                return BCM_E_PARAM;
            }
        }     

        /* Add port to the IPMC L3_BITMAP or L2_BITMAP as decided above */
        rv = _bcm_trx_multicast_ipmc_read(unit, mc_index, &l2_pbmp,
                                          &l3_pbmp);
        if (BCM_SUCCESS(rv)) {
            BCM_PBMP_ASSIGN(old_pbmp, *new_pbmp);
            if (BCM_GPORT_IS_TRUNK(port)) {
                 for (idx=0; idx<trunk_local_ports; idx++) { 		   
                     BCM_PBMP_PORT_ADD(*new_pbmp, trunk_member_port[idx]);
                 }
            } else {
                BCM_PBMP_PORT_ADD(*new_pbmp, port_out);
            }
            if (BCM_PBMP_NEQ(old_pbmp, *new_pbmp)) {
                rv = _bcm_trx_multicast_ipmc_write(unit, mc_index, l2_pbmp,
                                                   l3_pbmp, TRUE);
            }
        } 
        if (BCM_FAILURE(rv) && (encap_id != BCM_IF_INVALID)) {
            if (BCM_GPORT_IS_TRUNK(port)) {
                 for (idx=0; idx<trunk_local_ports; idx++) { 		   
                     (void) _bcm_fb_ipmc_egress_intf_delete(unit, mc_index, trunk_member_port[idx],
									   port_max, encap_id, is_l3);
                 }
            } else {
                     (void) _bcm_fb_ipmc_egress_intf_delete(unit, mc_index, port_out,
									   port_max, encap_id, is_l3);
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_multicast_egress_delete
 * Purpose:
 *      Delete GPORT from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_multicast_egress_delete(int unit, bcm_multicast_t group,
                               bcm_gport_t port, bcm_if_t encap_id)
{
    int rv = BCM_E_NONE, gport_id, mc_index; 
    int port_count, port_max = 4096;
    int i, local_count = 0, is_l3 = 0;
    bcm_pbmp_t l2_pbmp, l3_pbmp, old_l2_pbmp;
    bcm_module_t mod_out;
    bcm_port_t port_out, temp_port;
    bcm_trunk_t trunk_id;
    bcm_gport_t *port_array;
    bcm_if_t *encap_array;
    l2mc_entry_t l2mc_entry;
    bcm_trunk_add_info_t tinfo;
    bcm_port_t    trunk_member_port[BCM_SWITCH_TRUNK_MAX_PORTCNT];	
    int idx, modid_local=0, trunk_local_ports=0;
	
    if (!SOC_GPORT_IS_LOCAL(port) && !SOC_GPORT_IS_MODPORT(port) 
        && !SOC_GPORT_IS_TRUNK(port)) {
        return BCM_E_PORT;
    } 

    if (BCM_GPORT_IS_TRUNK(port)) {
           rv = _bcm_trunk_id_validate(unit, BCM_GPORT_TRUNK_GET(port));
           if (BCM_FAILURE(rv)) {
                 return (BCM_E_PORT);
           }
           rv = bcm_esw_trunk_get(unit, BCM_GPORT_TRUNK_GET(port), &tinfo);
           if (BCM_FAILURE(rv)) {
                 return (BCM_E_PORT);
           }

           for (idx=0; idx<tinfo.num_ports; idx++) {
                rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                           tinfo.tm[idx], tinfo.tp[idx], &mod_out, &port_out);
               if (BCM_FAILURE(rv)) {
                   return (BCM_E_PORT);
               }

               rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
               if (modid_local) {
                   trunk_member_port[trunk_local_ports++] = port_out;
                }
           }
     } else {
           rv = _bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
							&trunk_id, &gport_id); 
           BCM_IF_ERROR_RETURN(rv);

           BCM_IF_ERROR_RETURN(
               _bcm_esw_modid_is_local(unit, mod_out, &modid_local));

           if (TRUE != modid_local) {
               /* Only add this to replication set if destination is local */
              return BCM_E_PORT;
           }
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if (_BCM_MULTICAST_IS_L2(group)) {
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L2MCm))) {
            return BCM_E_PARAM;
        }

        /* Remove the port from the L2MC port bitmap */
        soc_mem_lock(unit, L2MCm);
        rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry);
        if (rv < 0) {
            soc_mem_unlock(unit, L2MCm);
            return rv;
        }
        soc_mem_pbmp_field_get(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                               &l2_pbmp);
        BCM_PBMP_PORT_REMOVE(l2_pbmp, port_out);
        soc_mem_pbmp_field_set(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                               &l2_pbmp);

        rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, mc_index, &l2mc_entry);
        soc_mem_unlock(unit, L2MCm);
    } else {
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
            return BCM_E_PARAM;
        }

        if (_BCM_MULTICAST_IS_L3(group)) {
            is_l3 = 1;
        } else if (!_BCM_MULTICAST_IS_VPLS(group) &&
                   !_BCM_MULTICAST_IS_MIM(group) &&
                   !_BCM_MULTICAST_IS_SUBPORT(group)) {
            return BCM_E_PARAM;
        }

        if (encap_id == BCM_IF_INVALID) {
            /* Remove L2 port */
            if (_BCM_MULTICAST_IS_VPLS(group)) {
                return BCM_E_PARAM;
            }

            soc_mem_lock(unit, L3_IPMCm);
            /* Delete the port from the IPMC L2_BITMAP */
            rv = _bcm_trx_multicast_ipmc_read(unit, mc_index, 
                                              &l2_pbmp, &l3_pbmp);
            if (BCM_SUCCESS(rv)) {
                BCM_PBMP_ASSIGN(old_l2_pbmp, l2_pbmp);
                if (BCM_GPORT_IS_TRUNK(port)) {
                    for (idx=0; idx<trunk_local_ports; idx++) {
                        BCM_PBMP_PORT_REMOVE(l2_pbmp, trunk_member_port[idx]);
                    }
                } else {
                    BCM_PBMP_PORT_REMOVE(l2_pbmp, port_out);
                }
                if (BCM_PBMP_NEQ(old_l2_pbmp, l2_pbmp)) {
                    rv = _bcm_trx_multicast_ipmc_write(unit, mc_index,
                                            l2_pbmp, l3_pbmp, TRUE);
                } else {
                    return BCM_E_NOT_FOUND;
                }
            }
            soc_mem_unlock(unit, L3_IPMCm);
            return rv;
        }
		
        /*
         * Walk through the list of egress interfaces for this group.
         * Check if the interface getting deleted is the ONLY
         * instance of the local port in the replication list.
         * If so, we can delete the port from the IPMC L3_BITMAP.
         */
        port_array = sal_alloc(port_max * sizeof(bcm_gport_t), "mcast port array");
        if (port_array == NULL) {
            return BCM_E_MEMORY;
        }
        encap_array = sal_alloc(port_max * sizeof(bcm_if_t), "mcast encap array");
        if (encap_array == NULL) {
            sal_free (port_array);
            return BCM_E_MEMORY;
        }

        rv = _bcm_tr_multicast_egress_get(unit, group, port_max, port_array, 
                                          encap_array, &port_count);
        if (BCM_FAILURE(rv)) {
            sal_free (port_array);
            sal_free (encap_array);
            return (rv);
        } 

        for (i = 0; i < port_count ; i++) {
            if (encap_id == encap_array[i]) {
                /* Skip the one we're about to delete */
                continue;
            }

            (void) bcm_esw_port_local_get(unit, port_array[i], &temp_port);

            if (temp_port == port_out) {
                local_count = 1;
                break;
            } else {
                for (idx=0; idx<trunk_local_ports; idx++) {
                    if (temp_port == trunk_member_port[idx]) {
                           local_count = 1;
                           break;
                    	}
                }
                if (local_count) {
                    break;
                }
            }
        }
        sal_free(port_array);
        sal_free(encap_array);

        if (!local_count) {
            soc_mem_lock(unit, L3_IPMCm);
            /* Delete the port from the IPMC L3_BITMAP */
            rv = _bcm_trx_multicast_ipmc_read(unit, mc_index, 
                                              &l2_pbmp, &l3_pbmp);
            if (BCM_SUCCESS(rv)) {
                 if (BCM_GPORT_IS_TRUNK(port)) {
                        for (idx=0; idx<trunk_local_ports; idx++) {
                                BCM_PBMP_PORT_REMOVE(l3_pbmp, trunk_member_port[idx]);
                        }
                 } else {
                        BCM_PBMP_PORT_REMOVE(l3_pbmp, port_out);
                 }
                rv = _bcm_trx_multicast_ipmc_write(unit, mc_index, l2_pbmp,
                                                  l3_pbmp, TRUE);
            }
            soc_mem_unlock(unit, L3_IPMCm);
            BCM_IF_ERROR_RETURN(rv);
        }
		
        if (BCM_GPORT_IS_TRUNK(port)) {
              for (idx=0; idx<trunk_local_ports; idx++) {		   
                     rv =  _bcm_fb_ipmc_egress_intf_delete(unit, mc_index, trunk_member_port[idx],
                                         port_max, encap_id, is_l3);
                     if (rv < 0) {
                         while (idx--) {
                                   (void) _bcm_fb_ipmc_egress_intf_add(unit, mc_index, trunk_member_port[idx],
                                                            encap_id, is_l3);
                         }
                         return rv;
                     }
             }
        } else {
                     rv =  _bcm_fb_ipmc_egress_intf_delete(unit, mc_index, port_out,
                                         port_max, encap_id, is_l3);
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_multicast_egress_delete_all
 * Purpose:
 *      Delete all GPORTs from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_multicast_egress_delete_all(int unit, bcm_multicast_t group)
{
    return bcm_tr_multicast_egress_set(unit, group, 0, NULL, NULL);
}
#else  /* INCLUDE_L3 && BCM_TRX_SUPPORT */
int bcm_esw_triumph_multicast_not_empty;
#endif /* INCLUDE_L3 && BCM_TRX_SUPPORT */
