/* 
 * $Id:$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * Purpose:     XGS5 Port common driver.
 *
 * Notes:
 *      More than likely only new Port APIs are found in
 *      this function driver due to a huge amount of legacy code
 *      that were implemented before this model.
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm/error.h>

/* Checks for 'null' argument */
#define PARAM_NULL_CHECK(_arg)  \
    if ((_arg) == NULL) {       \
        return BCM_E_PARAM;     \
    }


/*
 * Function:
 *      bcmi_xgs5_port_addressable_local_get
 * Description:
 *      Validate that given port/gport parameter is an addressable
 *      local logical port and return local logical port in
 *      BCM port format (non-Gport).
 *      Logical port does not need to be defined (does not
 *      need to be a VALID port, only addressable).
 * Parameters:
 *      unit       - (IN) BCM device number
 *      port       - (IN) Port / Gport to validate
 *      local_port - (OUT) Port number if valid.
 * Return Value:
 *      BCM_E_NONE - Port OK
 *      BCM_E_INIT - Not initialized
 *      BCM_E_PORT - Port Invalid
 *
 * NOTE:
 *      This function only accepts those GPORT types that
 *      deal with logical BCM ports (i.e. MODPORT, LOCAL, etc.).
 *
 *      Also, unlike bcm_esw_port_local_get(), this routine only checks
 *      that the port number is within the valid port range.
 *      It does NOT check whether the port
 *      is a BCM valid port (i.e. a defined/configured port).
 *      This allows functions that requires port validation with
 *      a new logical port that has not been declared/configured
 *      in the system.
 */
int
bcmi_xgs5_port_addressable_local_get(int unit,
                                     bcm_port_t port, bcm_port_t *local_port)
{
    bcm_module_t mod;
    bcm_trunk_t trunk_id;
    int id;
    int is_local;
    
    /*
     * A logical port can be:
     *   - Defined and active.
     *   - Defined and inactive (SOC property portmap with ':i' option).
     *   - Not defined.
     *
     * The following port definitions/words (i.e. functions, macro names)
     * are used in the SDK as follows:
     *   - VALID       :  The logical port is defined, it may be active or
     *                    inactive.
     *                    There IS a logical-to-physical port mapping.
     *
     *   - ADDRESSABLE :  The logical port number is within the valid
     *                    range.
     *                    The port may or may have not been defined
     *                    (i.e. there is NO logical-to-physical port mapping.)
     */
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &mod, local_port,
                                    &trunk_id, &id));
        if ((trunk_id != -1) || (id != -1)) {
            return BCM_E_PORT;
        }

        /* Check that port is local */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod, &is_local));

        if (!is_local) {
            return BCM_E_PORT;
        }
    } else {
        *local_port = port;
    }

    /* Check that port is within valid logical port range */
    if (!SOC_PORT_ADDRESSABLE(unit, *local_port)) {
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs5_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 *      drv  - (IN) Pointer to the Port function driver.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      This routine only intializes the function driver.
 *      The main port routine is still in the esw/port.c file.
 */
int
bcmi_xgs5_port_fn_drv_init(int unit, bcm_esw_port_drv_t *drv)
{
    BCM_ESW_PORT_DRV(unit) = drv;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_resource_set
 * Purpose:
 *      Modify the following port resources: 
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      resource - (IN) Port resource configuration.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_set(int unit, 
                            bcm_gport_t port, bcm_port_resource_t *resource)
{
    bcm_port_t  lport1, lport2;

    PARAM_NULL_CHECK(resource);

    /* Check that given port matches logical port field in structure */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              port, &lport1));
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              resource->port, &lport2));
    if (lport1 != lport2) {
        return BCM_E_PARAM;
    }

    return BCM_ESW_PORT_DRV(unit)->resource_multi_set(unit, 1, resource);
}


/*
 * Function:
 *      bcmi_xgs5_port_resource_get
 * Purpose:
 *      Get the port resource configuration for the specified logical port.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical port.
 *      resource  - (OUT) Returns port resource information.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_get(int unit, 
                            bcm_gport_t port, bcm_port_resource_t *resource)
{
    bcm_port_t lport;
    soc_info_t *si = &SOC_INFO(unit);
    int encap = 0;

    PARAM_NULL_CHECK(resource);

    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              port, &lport));
    bcm_port_resource_t_init(resource);
    resource->flags = 0x0;
    resource->port = lport;
    resource->physical_port = si->port_l2p_mapping[lport];
    resource->lanes = si->port_num_lanes[lport];
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_speed_get(unit, lport, &resource->speed));
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_encap_get(unit, lport, &encap));
    resource->encap = encap;
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs5_port_resource_multi_set
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_multi_set(int unit, 
                                  int nport, bcm_port_resource_t *resource)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmi_xgs5_port_resource_traverse
 * Purpose:
 *      Iterates over the port resource configurations on a given
 *      unit and calls user-provided callback for every entry.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      trav_fn    - (IN) Callback function to execute.
 *      user_data  - (IN) Pointer to any user data.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_port_resource_traverse(int unit, 
                                 bcm_port_resource_traverse_cb trav_fn, 
                                 void *user_data)
{
    bcm_port_t port;
    bcm_port_resource_t resource;
    soc_info_t *si = &SOC_INFO(unit);

    PARAM_NULL_CHECK(trav_fn);

    for (port = 0; port < SOC_MAX_NUM_PORTS ; port++) {
        /* Skip logical ports with no assignment and management ports */
        if ((si->port_l2p_mapping[port] == -1) ||
            IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                continue;
        }

        /* Get port information */
        if (BCM_FAILURE(bcmi_xgs5_port_resource_get(unit, port, &resource))) {
            continue;
        }

        /* Call user-provided callback routine */
        BCM_IF_ERROR_RETURN(trav_fn(unit, &resource, user_data));
    }
    
    return BCM_E_NONE;
}

