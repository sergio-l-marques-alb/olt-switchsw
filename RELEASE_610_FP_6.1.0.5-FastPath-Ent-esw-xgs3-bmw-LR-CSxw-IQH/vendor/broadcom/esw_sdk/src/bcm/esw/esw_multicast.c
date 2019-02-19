/*
 * $Id: esw_multicast.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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

#include <sal/core/libc.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/l2.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/multicast.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/xgs3.h>

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
bcm_esw_multicast_create(int unit, uint32 flags, bcm_multicast_t *group)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_create(unit, flags, group);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_multicast_create(unit, flags, group);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_create(unit, flags, group);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
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
bcm_esw_multicast_destroy(int unit, bcm_multicast_t group)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_destroy(unit, group);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_multicast_destroy(unit, group);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_destroy(unit, group);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_l3_encap_get
 * Purpose:
 *      Get the Encap ID for L3.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      group - (IN) Multicast group ID.
 *      port  - (IN) Physical port.
 *      intf  - (IN) L3 interface ID.
 *      encap_id - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_multicast_l3_encap_get(int unit, bcm_multicast_t group,
                               bcm_gport_t port, bcm_if_t intf,
                               bcm_if_t *encap_id)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        /* For L3 IPMC, encap_id is simply the L3 interface ID */
        *encap_id = intf;
        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_multicast_l2_encap_get
 * Purpose:
 *      Get the Encap ID for L2.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      group    - (IN) Multicast group ID.
 *      port     - (IN) Physical port.
 *      vlan     - (IN) Vlan.
 *      encap_id - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_multicast_l2_encap_get(int unit, bcm_multicast_t group,
                               bcm_gport_t port, bcm_vlan_t vlan,
                               bcm_if_t *encap_id)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        /* Encap ID is not used for L2 in XGS3 */
        *encap_id = BCM_IF_INVALID;
        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL; 
}

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
bcm_esw_multicast_vpls_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                 bcm_gport_t mpls_port_id, bcm_if_t *encap_id)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_vpls_encap_get(unit, group, port, 
                                               mpls_port_id, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_multicast_vpls_encap_get(unit, group, port, 
                                               mpls_port_id, encap_id);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

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
bcm_esw_multicast_subport_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                    bcm_gport_t subport, bcm_if_t *encap_id)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        return bcm_tr2_multicast_subport_encap_get(unit, group, port, 
                                                  subport, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_multicast_subport_encap_get(unit, group, port, 
                                                  subport, encap_id);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_mim_encap_get
 * Purpose:
 *      Get the Encap ID for MiM.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      port      - (IN) Physical port.
 *      mim_port  - (IN) MiM port ID.
 *      encap_id  - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_mim_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t mim_port, bcm_if_t *encap_id)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        return bcm_tr2_multicast_mim_encap_get(unit, group, port, 
                                              mim_port, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_wlan_encap_get
 * Purpose:
 *      Get the Encap ID for WLAN.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      port      - (IN) Physical port.
 *      wlan_port - (IN) WLAN port ID.
 *      encap_id  - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_wlan_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                 bcm_gport_t wlan_port, bcm_if_t *encap_id)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_wlan_encap_get(unit, group, port, 
                                               wlan_port, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
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
bcm_esw_multicast_egress_add(int unit, bcm_multicast_t group, 
                             bcm_gport_t port, bcm_if_t encap_id)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_egress_add(unit, group, port, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_multicast_egress_add(unit, group, port, encap_id);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_egress_add(unit, group, port, encap_id);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
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
bcm_esw_multicast_egress_delete(int unit, bcm_multicast_t group, 
                                bcm_gport_t port, bcm_if_t encap_id)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_egress_delete(unit, group, port, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_multicast_egress_delete(unit, group, port, encap_id);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_egress_delete(unit, group, port, encap_id);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_egress_delete_all
 * Purpose:
 *      Delete all replications for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_egress_delete_all(int unit, bcm_multicast_t group)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_egress_delete_all(unit, group);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_multicast_egress_delete_all(unit, group);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_egress_delete_all(unit, group);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
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
 *      port_count - (IN) Number of ports in replication list
 *      port_array - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 * Returns:
 *      BCM_E_XXX
 */     
int     
bcm_esw_multicast_egress_set(int unit, bcm_multicast_t group, int port_count,
                             bcm_gport_t *port_array, bcm_if_t *encap_id_array)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_egress_set(unit, group, port_count, 
                                            port_array, encap_id_array);
    }
#endif  /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_multicast_egress_set(unit, group, port_count, 
                                           port_array, encap_id_array);
    }
#endif  /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_egress_set(unit, group, port_count, 
                                             port_array, encap_id_array);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}   

/*
 * Function:
 *      bcm_multicast_egress_get
 * Purpose:
 *      Retrieve a set of egress multicast GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters: 
 *      unit           - (IN) Device Number
 *      mc_index       - (IN) Multicast index
 *      port_max       - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of ports
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of ports returned
 * Returns:
 *      BCM_E_XXX
 */
int     
bcm_esw_multicast_egress_get(int unit, bcm_multicast_t group, int port_max,
                             bcm_gport_t *port_array, bcm_if_t *encap_id_array, 
                             int *port_count)
{
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_multicast_egress_get(unit, group, port_max,
                                            port_array, encap_id_array,
                                            port_count);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_multicast_egress_get(unit, group, port_max,
                                           port_array, encap_id_array,
                                           port_count);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_egress_get(unit, group, port_max,
                                             port_array, encap_id_array,
                                             port_count);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}   
