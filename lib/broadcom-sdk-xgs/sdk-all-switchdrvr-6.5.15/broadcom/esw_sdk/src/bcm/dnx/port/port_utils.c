/**
 * \file port_utils.c $Id$ PORT Utility procedures for DNX. 
 *       this file is meant for internal functions, not
 *       dispatched from BCM APIs.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include "port_utils.h"

/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <soc/drv.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */
#define DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS   (BCM_PORT_ATTR_PFM_MASK           |  \
                                                 BCM_PORT_ATTR_PHY_MASTER_MASK    |  \
                                                 BCM_PORT_ATTR_RATE_MCAST_MASK    |  \
                                                 BCM_PORT_ATTR_RATE_BCAST_MASK    |  \
                                                 BCM_PORT_ATTR_RATE_DLFBC_MASK    |  \
                                                 BCM_PORT_ATTR_INTERFACE_MASK     |  \
                                                 BCM_PORT_ATTR_SPEED_MAX_MASK     |  \
                                                 BCM_PORT_ATTR_ABILITY_MASK       |  \
                                                 BCM_PORT_ATTR_MDIX_STATUS_MASK   |  \
                                                 BCM_PORT_ATTR_MEDIUM_MASK        |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK  |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK |  \
                                                 BCM_PORT_ATTR_DISCARD_MASK       |  \
                                                 BCM_PORT_ATTR_UNTAG_PRI_MASK     |  \
                                                 BCM_PORT_ATTR_ENCAP_MASK         |  \
                                                 BCM_PORT_ATTR_PAUSE_MAC_MASK     |  \
                                                 BCM_PORT_ATTR_MDIX_MASK)

#define DNX_PORT_NIF_ILKN_UNSUPPORTED_ATTRS     (DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS |  \
                                                 BCM_PORT_ATTR_FRAME_MAX_MASK          |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK       |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK      |  \
                                                 BCM_PORT_ATTR_FAULT_MASK)

#define DNX_PORT_NIF_ILKN_ELK_UNSUPPORTED_ATTRS (DNX_PORT_NIF_ILKN_UNSUPPORTED_ATTRS   |  \
                                                 BCM_PORT_ATTR_LEARN_MASK              |  \
                                                 BCM_PORT_ATTR_VLANFILTER_MASK         |  \
                                                 BCM_PORT_ATTR_UNTAG_VLAN_MASK         |  \
                                                 BCM_PORT_ATTR_PAUSE_MASK              |  \
                                                 BCM_PORT_ATTR_STP_STATE_MASK)

#define DNX_PORT_STIF_UNSUPPORTED_ATTRS         (DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS |  \
                                                 BCM_PORT_ATTR_LEARN_MASK              |  \
                                                 BCM_PORT_ATTR_VLANFILTER_MASK         |  \
                                                 BCM_PORT_ATTR_UNTAG_VLAN_MASK         |  \
                                                 BCM_PORT_ATTR_PAUSE_TX_MASK           |  \
                                                 BCM_PORT_ATTR_PAUSE_RX_MASK           |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK       |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK      |  \
                                                 BCM_PORT_ATTR_FAULT_MASK              |  \
                                                 BCM_PORT_ATTR_LINKSCAN_MASK)

#define DNX_PORT_FABRIC_UNSUPPORTED_ATTRS       (BCM_PORT_ATTR_PAUSE_TX_MASK           |  \
                                                 BCM_PORT_ATTR_PAUSE_RX_MASK           |  \
                                                 BCM_PORT_ATTR_LEARN_MASK              |  \
                                                 BCM_PORT_ATTR_DISCARD_MASK            |  \
                                                 BCM_PORT_ATTR_VLANFILTER_MASK         |  \
                                                 BCM_PORT_ATTR_UNTAG_PRI_MASK          |  \
                                                 BCM_PORT_ATTR_UNTAG_VLAN_MASK         |  \
                                                 BCM_PORT_ATTR_STP_STATE_MASK          |  \
                                                 BCM_PORT_ATTR_INTERFACE_MASK          |  \
                                                 BCM_PORT_ATTR_FRAME_MAX_MASK          |  \
                                                 BCM_PORT_ATTR_AUTONEG_MASK            |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK       |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK      |  \
                                                 BCM_PORT_ATTR_ENCAP_MASK              |  \
                                                 BCM_PORT_ATTR_PFM_MASK                |  \
                                                 BCM_PORT_ATTR_PHY_MASTER_MASK         |  \
                                                 BCM_PORT_ATTR_RATE_MCAST_MASK         |  \
                                                 BCM_PORT_ATTR_RATE_BCAST_MASK         |  \
                                                 BCM_PORT_ATTR_ABILITY_MASK            |  \
                                                 BCM_PORT_ATTR_MDIX_STATUS_MASK        |  \
                                                 BCM_PORT_ATTR_PAUSE_MAC_MASK          |  \
                                                 BCM_PORT_ATTR_RATE_DLFBC_MASK         |  \
                                                 BCM_PORT_ATTR_MEDIUM_MASK)
#define DNX_PORT_UNSUPPORTED_COMMON_ATTRS2       BCM_PORT_ATTR2_PORT_ABILITY

#define DNX_PORT_NON_ETHER_UNSUPPORTED_ATTRS     BCM_PORT_ATTR_STP_STATE_MASK
/*
 * }
 */

/**
 * \brief - internal function to the bcm_port_add API. 
 * this function is used to retrieve the added port and call the IMB layer 
 * port_add API to open the port path in the IMB.
 *  
 * see .h file 
 */
int
dnx_port_imb_path_port_add_handle(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get added port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_type))
    {
        /*
         * only call IMB open path for NIF and Fabric
         */
        SHR_IF_ERR_EXIT(imb_port_add(unit, port, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - internal function to the bcm_port_remove API. 
 * this function is used to retrieve the removed port and call 
 * the IMB layer port_remove API to close the port path in the 
 * IMB. 
 * 
 * see .h file
 */
int
dnx_port_imb_path_port_remove_handle(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get removed port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_type))
    {
        /*
         * only call IMB close path for NIF and Fabric
         */
        SHR_IF_ERR_EXIT(imb_port_remove(unit, port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get support action mask for port info (bcm_port_info_t)
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] mask - support mask
 * \param [out] mask2 - support mask2
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_info_support_action_mask_get(
    int unit,
    bcm_port_t port,
    uint32 *mask,
    uint32 *mask2)
{
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    *mask = BCM_PORT_ATTR_ALL_MASK;

    if (!IS_E_PORT(unit, port))
    {
        *mask &= ~DNX_PORT_NON_ETHER_UNSUPPORTED_ATTRS;
    }
    /*
     * Currently BCM_PORT_ATTR2_PORT_ABILITY is the only attr2 flag 
     */
    *mask2 = BCM_PORT_ATTR2_PORT_ABILITY;
    /*
     * Get rid of the unspported attr for mask1 and mask2
     */
    *mask2 &= ~DNX_PORT_UNSUPPORTED_COMMON_ATTRS2;
    switch (port_type)
    {
        case DNX_ALGO_PORT_TYPE_FABRIC:
            *mask &= ~DNX_PORT_FABRIC_UNSUPPORTED_ATTRS;
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK:
            *mask &= ~DNX_PORT_NIF_ILKN_ELK_UNSUPPORTED_ATTRS;
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
            *mask &= ~DNX_PORT_NIF_ILKN_UNSUPPORTED_ATTRS;
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
            *mask &= ~DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS;
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ETH_STIF:
            *mask &= ~DNX_PORT_STIF_UNSUPPORTED_ATTRS;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Port type %d is not supported.\r\n", port_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
