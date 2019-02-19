/*
 * $Id: compat_6512.c,v 2.0 2017/10/15
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * RPC Compatibility with <=sdk-6.5.8 routines
 */

#ifdef	BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6512.h>

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6512in_vxlan_vpn_config_t
 * Purpose:
 *      Convert the bcm_vxlan_vpn_config_t datatype from <=6.5.12 to
 *      SDK 6.5.12+
 * Parameters:
 *      from        - (IN) The <=6.5.12 version of the datatype
 *      to          - (OUT) The SDK 6.5.12+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6512in_vxlan_vpn_config_t(bcm_compat6512_vxlan_vpn_config_t *from,
                           bcm_vxlan_vpn_config_t *to)
{
    bcm_vxlan_vpn_config_t_init(to);
    to->flags= from->flags;
    to->vpn = from->vpn;
    to->vnid = from->vnid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->broadcast_group = from->broadcast_group;
    to->unknown_unicast_group = from->unknown_multicast_group;
    to->protocol_pkt = from->protocol_pkt;
    to->vlan = from->vlan;
    to->match_port_class = from->match_port_class;
 }

/*
 * Function:
 *      _bcm_compat6512out_ecn_map_t
 * Purpose:
 *      Convert the bcm_vxlan_vpn_config_t datatype from 6.5.12+ to
 *      <=6.5.12
 * Parameters:
 *      from        - (IN) The SDK 6.5.8+ version of the datatype
 *      to          - (OUT) The <=6.5.8 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6512out_vxlan_vpn_config_t(bcm_vxlan_vpn_config_t *from,
                            bcm_compat6512_vxlan_vpn_config_t *to)
{
    to->flags= from->flags;
    to->vpn = from->vpn;
    to->vnid = from->vnid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->broadcast_group = from->broadcast_group;
    to->unknown_unicast_group = from->unknown_multicast_group;
    to->protocol_pkt = from->protocol_pkt;
    to->vlan = from->vlan;
    to->match_port_class = from->match_port_class;
}

/*
 * Function: bcm_compat6512_vxlan_vpn_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_vpn_get.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      l2vpn   - (IN)VXLAN VPN
 *      info     - (IN/OUT)VXLAN VPN Config
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6512_vxlan_vpn_get(int unit, bcm_vpn_t l2vpn,
                                     bcm_compat6512_vxlan_vpn_config_t *info)
{
    int rv;
    bcm_vxlan_vpn_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_vxlan_vpn_config_t *)
            sal_alloc(sizeof(bcm_vxlan_vpn_config_t), "New vxlan vpn config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6512in_vxlan_vpn_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_vpn_get(unit, l2vpn, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6512out_vxlan_vpn_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat6512_vxlan_vpn_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_vxlan_vpn_create.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      info     - (IN/OUT)VXLAN VPN Config
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6512_vxlan_vpn_create(int unit,
                                         bcm_compat6512_vxlan_vpn_config_t *info)
{
    int rv;
    bcm_vxlan_vpn_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_vxlan_vpn_config_t *)
            sal_alloc(sizeof(bcm_vxlan_vpn_config_t), "New vxlan vpn config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6512in_vxlan_vpn_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_vpn_create(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6512out_vxlan_vpn_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}
#endif
#endif  /* BCM_RPC_SUPPORT */
