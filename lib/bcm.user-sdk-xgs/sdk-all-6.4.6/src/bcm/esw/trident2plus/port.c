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
 * Purpose:     Port driver.
 *
 */

#include <soc/defs.h>

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/td2_td2p.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/portctrl.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm/error.h>
#include <bcm/port.h>


/*
 * FlexPort Operations Changes
 */
#define BCM_TD2P_PORT_RESOURCE_OP_NONE     0
#define BCM_TD2P_PORT_RESOURCE_OP_REMAP   (1 << 0)
#define BCM_TD2P_PORT_RESOURCE_OP_LANES   (1 << 1)
#define BCM_TD2P_PORT_RESOURCE_OP_SPEED   (1 << 2)
#define BCM_TD2P_PORT_RESOURCE_OP_ENCAP   (1 << 3)
#define BCM_TD2P_PORT_RESOURCE_OP_ALL           \
    (BCM_TD2P_PORT_RESOURCE_OP_REMAP |          \
     BCM_TD2P_PORT_RESOURCE_OP_LANES |          \
     BCM_TD2P_PORT_RESOURCE_OP_SPEED |          \
     BCM_TD2P_PORT_RESOURCE_OP_ENCAP) 


/*
 * Forward static function declaration
 */
STATIC int
_bcm_td2p_port_resource_multi_set(int unit, 
                                  int nport, bcm_port_resource_t *resource);

/*
 * Function Vector
 */
static bcm_esw_port_drv_t bcm_td2p_port_drv = {
    /* fn_drv_init                */ bcmi_td2p_port_fn_drv_init,
    /* resource_set               */ bcmi_xgs5_port_resource_set,
    /* resource_get               */ bcmi_xgs5_port_resource_get,
    /* resource_multi_set         */ _bcm_td2p_port_resource_multi_set,
    /* resource_traverse          */ bcmi_xgs5_port_resource_traverse,
};


/*
 * Function:
 *      bcmi_td2p_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_td2p_port_fn_drv_init(int unit)
{
    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit, &bcm_td2p_port_drv));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_resolve
 * Purpose:
 *      Convert logical port number GPORT to BCM local port format.
 *      In addition, it validates the following:
 *      - The order of elements in array, 'delete' operations must
 *        be placed first in array.
 *      - Logical and physical port numbers are within the valid range.
 *      - Ports must not be CPU or Loopback ports.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 */
STATIC int
_bcm_td2p_port_resource_resolve(int unit, 
                                int nport,
                                bcm_port_resource_t *resource)
{
    int i;
    bcm_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Check that logical port number is addressable and convert */
        BCM_IF_ERROR_RETURN
            (bcmi_xgs5_port_addressable_local_get(unit,
                                                  pr->port,
                                                  &pr->port));

        /* Check that physical port is within the valid range */
        if (pr->physical_port != -1) {
            SOC_IF_ERROR_RETURN
                (soc_td2p_phy_port_addressable(unit, pr->physical_port));
        }

        /* Check that ports, logical and physical, are not a management port */
        if ((si->port_l2p_mapping[pr->port] == TD2P_PHY_PORT_CPU) ||
            (si->port_l2p_mapping[pr->port] == TD2P_PHY_PORT_LB) ||
            (pr->physical_port == TD2P_PHY_PORT_CPU) ||
            (pr->physical_port == TD2P_PHY_PORT_LB)) {
            return BCM_E_PORT;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_input_validate
 * Purpose:
 *      Validate function input requirements.
 *
 *      This routine checks for function semantics and guidelines
 *      from the API perspective:
 *      - Check that logical port number is valid and convert GPORT port
 *        format to BCM local port format.
 *      - Logical and physical port number must be within valid range.
 *      - Check the order of elements in array, 'delete' operations must
 *        be placed first in array.
 *      - Ports must not be CPU or Loopback ports.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 */
STATIC int
_bcm_td2p_port_resource_input_validate(int unit, 
                                       int nport,
                                       bcm_port_resource_t *resource)
{
    /* Check for order of elements */
    int i;
    bcm_port_resource_t *pr;
    int delete = 1;
    int enable;

    /* Convert port format and validate logical and physical port numbers */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_resolve(unit, 
                                         nport, resource));

    /* Check array order and port state disable */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Check that delete operations are first */
        if (pr->physical_port != -1) {  /* First non-delete found */
            delete = 0;
        } else if ((pr->physical_port == -1) && !delete) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Delete operations must be "
                                  "first in array\n")));
            return BCM_E_CONFIG;
        }

        /* Check that ports are disabled */
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_enable_get(unit, pr->port, &enable));
        if (enable) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Port %d needs to be disabled\n"),
                       pr->port));
            return BCM_E_BUSY;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_op_get
 * Purpose:
 *      Get the type of FlexPort operations/changes:
 *          none, remap, lanes, speed, encap.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      op       - (OUT) FlexPort operation BCM_TD2P_PORT_RESOURCE_OP_...
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Resource is not NULL.
 *      Logical port in 'resource' is in BCM port format (non GPORT).
 */
STATIC int
_bcm_td2p_port_resource_op_get(int unit, 
                               int nport, bcm_port_resource_t *resource,
                               int *op)
{
    int i;
    bcm_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);
    int speed;
    int encap;

    *op = BCM_TD2P_PORT_RESOURCE_OP_NONE;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Check port mapping */
        if (!(*op & BCM_TD2P_PORT_RESOURCE_OP_REMAP)) {
            if ((pr->physical_port == -1) ||
                (pr->physical_port != si->port_l2p_mapping[pr->port])) {
                *op |= BCM_TD2P_PORT_RESOURCE_OP_REMAP;
            }
        }

        /* Check port speed */
        if (!(*op & BCM_TD2P_PORT_RESOURCE_OP_SPEED)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, pr->port, &speed));
            if (pr->speed != speed) {
                *op |= BCM_TD2P_PORT_RESOURCE_OP_SPEED;
            }
        }

        /* Check port lanes */
        if (!(*op & BCM_TD2P_PORT_RESOURCE_OP_LANES)) {
            if (pr->lanes != si->port_num_lanes[pr->port]) {
                *op |= BCM_TD2P_PORT_RESOURCE_OP_LANES;
            }
        }

        /* Check port encap */
        if (!(*op & BCM_TD2P_PORT_RESOURCE_OP_ENCAP)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, pr->port, &encap));
            if (pr->encap != encap) {
                *op |= BCM_TD2P_PORT_RESOURCE_OP_ENCAP;
            }
        }

        /* If all are set, no more checking is necessary */
        if (*op == BCM_TD2P_PORT_RESOURCE_OP_ALL) {
            break;
        }
    }
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_multi_set
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
STATIC int
_bcm_td2p_port_resource_multi_set(int unit, 
                                  int nport, bcm_port_resource_t *resource)
{
    int rv;
    int op;
    int i;
    soc_port_resource_t *soc_resource;

    if (resource == NULL) {
        return BCM_E_PARAM;
    }

    /*
     * Validate function input requirements
     *
     * This steps checks for function semantics and guidelines
     * from the API perspective.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_input_validate(unit, nport, resource));

    /* Check type of FlexPort operations */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_op_get(unit, nport, resource, &op));
    if (op == BCM_TD2P_PORT_RESOURCE_OP_NONE) {
        return BCM_E_NONE;     /* No changes, just return */
    }


    /*
     * FlexPort Operation
     *
     * More validation takes place in the SOC layer.
     * These check for abilities and configurations based on
     * the physical aspects of the device.
     *
     * In addition, if the validation is successful,
     * the SOC Port Resource data structure is built.
     */

    /*
     * SOC Port Structure initialization
     */
    /* Allocate memory for SOC Port Resource array data structure */
    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "port_resource");
    if (soc_resource == NULL) {
        return BCM_E_MEMORY;
    }

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = resource[i].port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
    }

    /*
     * SOC FlexPort Operation
     *
     * The rest of the steps takes place at the SOC layer.
     * This include rest of the validation and execute of the
     * FlexPort operation.
     */
    rv = soc_td2p_port_resource_configure(unit, nport, soc_resource);

    /*
     * Bring new ports down to ensure port state is proper
     */
    for (i = 0; i < nport; i++) {
        if (resource[i].physical_port == -1) {
            continue;
        }

        rv = bcmi_esw_portctrl_enable_set(unit, resource[i].port, 0);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    sal_free(soc_resource);

    return rv;
}


/*
 * Function:
 *      bcmi_td2p_port_lanes_get
 * Purpose:
 *      Get the number of PHY lanes for the given port.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (OUT) Returns number of lanes for port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_td2p_port_lanes_get(int unit, bcm_port_t port, int *lanes)
{
    bcm_port_resource_t resource;

    BCM_IF_ERROR_RETURN(bcm_esw_port_resource_get(unit, port, &resource));

    *lanes = resource.lanes;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_td2p_port_lanes_set
 * Purpose:
 *      Set the number of PHY lanes for the given port.
 *
 *      This function should only be used to support the legacy
 *      FlexPort API bcm_port_control_set(... , bcmPortControlLanes).
 *
 *      The legacy API does not delete the port mappings when flexing
 *      to fewer lanes.  Ports which lanes are used by the base
 *      physical port becomes inactive.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (IN) Number of lanes to set on given port.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Only to be used by legacy API to support TD2 behavior
 *        bcm_port_control_set(... , bcmPortControlLanes).
 *      - Supports only up to 4 lanes (TSC-4).
 *      - 'port' must be in BCM port format (non-GPORT) and mapped to
 *        the base physical port.
 *      - Several calls to legacy API may be required to
 *        achieve desired configuration.
 */
int
bcmi_td2p_port_lanes_set(int unit, bcm_port_t port, int lanes)
{
    soc_info_t *si = &SOC_INFO(unit);
    bcm_port_resource_t resource[8];  /* Max size to support up to 4 lanes */
    int max_array_cnt = 8;
    int i;
    int cur_lanes;
    int phy_port;
    int speed;
    int encap;
    int num_ports_clear;
    int num_ports_new;

    /* Support legacy API up to 4 lanes */
    if ((lanes != 1) && (lanes != 2) && (lanes != 4)) {
        return BCM_E_PARAM;
    }

    /* Check current number of lanes */
    BCM_IF_ERROR_RETURN(bcmi_td2p_port_lanes_get(unit, port, &cur_lanes));
    if (cur_lanes == lanes) {
        return BCM_E_NONE;
    }

    /* Check that port can support number of lanes specified */
    SOC_IF_ERROR_RETURN(soc_td2p_port_lanes_valid(unit, port, lanes));

    for (i = 0; i < max_array_cnt; i++) {
        bcm_port_resource_t_init(&resource[i]);
    }

    /*
     * Build port resource array
     * Certain information is derived from 'best' guess,
     * similar to the legacy behaviour.
     */

    /* Select speed */
    switch(lanes) {
    case 1:
        speed = 10000;
        break;
    case 2:
        speed = 20000;
        break;
    case 4:
        speed = 40000;
        break;
    default:
        return BCM_E_PARAM;
        break;
    }

    /* Keep encap the same */
    BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, port, &encap));

    switch (cur_lanes) {
    case 1:
        /*
         * 2x10 flex to: 2 lanes -> 1x20
         * 4x10 flex to: 4 lanes -> 1x40
         */
        if (lanes == 2) {
            num_ports_clear = 2;
            num_ports_new = 1;
        } else {
            num_ports_clear = 4;
            num_ports_new = 1;
        }
        break;
    case 2:
        /*
         * 1x20 flex to: 1 lane  -> 2x10
         * 2x20 flex to: 4 lanes -> 1x40
         */
        if (lanes == 1) {
            num_ports_clear = 2;
            num_ports_new = 2;
        } else {
            num_ports_clear = 4;
            num_ports_new = 1;
        }
        break;
    case 4:
        /*
         * 1x40 flex to: 1 lane  -> 4x10
         *               2 lanes -> 2x20
         */
        if (lanes == 1) {
            num_ports_clear = 4;
            num_ports_new = 4;
        } else {
            num_ports_clear = 4;
            num_ports_new = 2;
        }
        break;
    default:
        return BCM_E_CONFIG;
        break;
    }

    if ((num_ports_clear + num_ports_new) > max_array_cnt) {
        return BCM_E_INTERNAL;
    }

    /*
     * Clear mapping for physical ports involved in FlexPort operation.
     * Assume physical ports are numbered consecutively in device.
     */
    phy_port = si->port_l2p_mapping[port];
    for (i = 0; i < num_ports_clear; i++) {
        resource[i].flags = SOC_PORT_RESOURCE_I_MAP;
        resource[i].port = si->port_p2l_mapping[phy_port++];
        resource[i].physical_port = -1;
    }

    /*
     * Map new ports
     *
     * Legacy API requires that logical-physical port mapping
     * is valid in the SOC_INFO SW data.
     */
    phy_port = si->port_l2p_mapping[port];
    for (; i < (num_ports_clear + num_ports_new); i++) {
        /* Check valid physical port */
        if (phy_port == -1) {
            return BCM_E_INTERNAL;
        }

        resource[i].flags = SOC_PORT_RESOURCE_I_MAP;
        resource[i].port = si->port_p2l_mapping[phy_port];
        resource[i].physical_port = phy_port;
        resource[i].lanes = lanes;
        resource[i].speed = speed;
        resource[i].encap = encap;
        phy_port += lanes;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_port_resource_multi_set(unit,
                                         (num_ports_clear + num_ports_new),
                                         resource));

    return BCM_E_NONE;
}

#else /* BCM_TRIDENT2PLUS_SUPPORT */
int bcm_esw_td2p_port_not_empty;
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
