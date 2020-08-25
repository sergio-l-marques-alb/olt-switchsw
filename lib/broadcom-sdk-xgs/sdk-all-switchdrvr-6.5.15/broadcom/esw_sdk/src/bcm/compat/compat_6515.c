/*
 * $Id: compat_6515.h,v 2.0 2018/09/20
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * RPC Compatibility with sdk-6.5.15 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6515.h>

#ifdef INCLUDE_L3
/*
 * Function:
 *      _bcm_compat6515in_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from <=6.5.15 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.15 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6515in_mpls_tunnel_switch_t(
    bcm_compat6515_mpls_tunnel_switch_t *from,
    bcm_mpls_tunnel_switch_t *to)
{
    bcm_mpls_tunnel_switch_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->label = from->label;
    to->second_label = from->second_label;
    to->port = from->port;
    to->action = from->action;
    to->action_if_bos = from->action_if_bos;
    to->action_if_not_bos = from->action_if_not_bos;
    to->mc_group = from->mc_group;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->policer_id = from->policer_id;
    to->vpn = from->vpn;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(to->egress_label));
    to->egress_if = from->egress_if;
    to->ingress_if = from->ingress_if;
    to->mtu = from->mtu;
    to->qos_map_id = from->qos_map_id;
    to->failover_id = from->failover_id;
    to->tunnel_id = from->tunnel_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->egress_port = from->egress_port;
    to->oam_global_context_id = from->oam_global_context_id;
    to->class_id = from->class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->ecn_map_id = from->ecn_map_id;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
}

/*
 * Function:
 *      _bcm_compat6515out_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from 6.5.15+ to
 *      <=6.5.15
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.15 version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6515out_mpls_tunnel_switch_t(
    bcm_mpls_tunnel_switch_t *from,
    bcm_compat6515_mpls_tunnel_switch_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->label = from->label;
    to->second_label = from->second_label;
    to->port = from->port;
    to->action = from->action;
    to->action_if_bos = from->action_if_bos;
    to->action_if_not_bos = from->action_if_not_bos;
    to->mc_group = from->mc_group;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->policer_id = from->policer_id;
    to->vpn = from->vpn;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(to->egress_label));
    to->egress_if = from->egress_if;
    to->ingress_if = from->ingress_if;
    to->mtu = from->mtu;
    to->qos_map_id = from->qos_map_id;
    to->failover_id = from->failover_id;
    to->tunnel_id = from->tunnel_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->egress_port = from->egress_port;
    to->oam_global_context_id = from->oam_global_context_id;
    to->class_id = from->class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->ecn_map_id = from->ecn_map_id;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
}

/*
 * Function:
 *      bcm_compat6515_mpls_tunnel_switch_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_add.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) MPLS tunnel switch info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_mpls_tunnel_switch_add(
    int unit,
    bcm_compat6515_mpls_tunnel_switch_t *info)
{
    int rv = 0;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t*)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t), "New tunnel switch info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_mpls_tunnel_switch_t(info, new_info);
    }

    rv = bcm_mpls_tunnel_switch_add(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_mpls_tunnel_switch_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_mpls_tunnel_switch_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_delete.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) MPLS tunnel switch info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_mpls_tunnel_switch_delete(
    int unit,
    bcm_compat6515_mpls_tunnel_switch_t *info)
{
    int rv = 0;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t*)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t), "New tunnel switch info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_mpls_tunnel_switch_t(info, new_info);
    }

    rv = bcm_mpls_tunnel_switch_delete(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_mpls_tunnel_switch_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_mpls_tunnel_switch_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) MPLS tunnel switch info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_mpls_tunnel_switch_get(
    int unit,
    bcm_compat6515_mpls_tunnel_switch_t *info)
{
    int rv = 0;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t*)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t), "New tunnel switch info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_mpls_tunnel_switch_t(info, new_info);
    }

    rv = bcm_mpls_tunnel_switch_get(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_mpls_tunnel_switch_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}
/*
 * Function:
 *      _bcm_compat6515in_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from <=6.5.15 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.15 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6515in_ipmc_addr_t(
    bcm_compat6515_ipmc_addr_t *from,
    bcm_ipmc_addr_t *to)
{
    bcm_ipmc_addr_t_init(to);
    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(&to->s_ip6_addr, &from->s_ip6_addr, sizeof(to->s_ip6_addr));
    sal_memcpy(&to->mc_ip6_addr, &from->mc_ip6_addr, sizeof(to->mc_ip6_addr));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(&to->mc_ip6_mask, &from->mc_ip6_mask, sizeof(to->mc_ip6_mask));
    to->group_l2 = from->group_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    sal_memcpy(&to->s_ip6_mask, &from->s_ip6_mask, sizeof(to->s_ip6_mask));
    to->priority = from->priority;
}

/*
 * Function:
 *      _bcm_compat6515out_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from 6.5.15+ to
 *      <=6.5.15
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.15 version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6515out_ipmc_addr_t(
    bcm_ipmc_addr_t *from,
    bcm_compat6515_ipmc_addr_t *to)
{
    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(&to->s_ip6_addr, &from->s_ip6_addr, sizeof(to->s_ip6_addr));
    sal_memcpy(&to->mc_ip6_addr, &from->mc_ip6_addr, sizeof(to->mc_ip6_addr));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(&to->mc_ip6_mask, &from->mc_ip6_mask, sizeof(to->mc_ip6_mask));
    to->group_l2 = from->group_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    sal_memcpy(&to->s_ip6_mask, &from->s_ip6_mask, sizeof(to->s_ip6_mask));
    to->priority = from->priority;
}

/*
 * Function:
 *      bcm_compat6515_ipmc_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_add.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_ipmc_add(
    int unit,
    bcm_compat6515_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_add(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_ipmc_addr_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_ipmc_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_find
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_ipmc_find(
    int unit,
    bcm_compat6515_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_find(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_ipmc_addr_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_ipmc_remove
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_remove.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_ipmc_remove(
    int unit,
    bcm_compat6515_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_remove(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_ipmc_addr_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

#endif

/*
 * Function:
 *      _bcm_compat6515in_port_match_info_t
 * Purpose:
 *      Convert the bcm_port_match_info_t datatype from <=6.5.15 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.15 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6515in_port_match_info_t(
    bcm_compat6515_port_match_info_t *from,
    bcm_port_match_info_t *to)
{
    bcm_port_match_info_t_init(to);

    to->match = from->match;
    to->port = from->port;
    to->match_vlan = from->match_vlan;
    to->match_vlan_max = from->match_vlan_max;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_inner_vlan_max = from->match_inner_vlan_max;
    to->match_tunnel_vlan = from->match_tunnel_vlan;
    sal_memcpy(&to->match_tunnel_srcmac, &from->match_tunnel_srcmac,
            sizeof(to->match_tunnel_srcmac));
    to->match_label = from->match_label;
    to->flags = from->flags;
    to->match_pon_tunnel = from->match_pon_tunnel;
    to->match_ethertype = from->match_ethertype;
    to->match_pcp = from->match_pcp;
    to->action = from->action;
    to->extended_port_vid = from->extended_port_vid;
    to->vpn = from->vpn;
    to->niv_port_vif = from->niv_port_vif;
    to->isid = from->isid;
}

/*
 * Function:
 *      _bcm_compat6515out_port_match_info_t
 * Purpose:
 *      Convert the bcm_port_match_info_t datatype from 6.5.15+ to
 *      <=6.5.15
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.15 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6515out_port_match_info_t(
    bcm_port_match_info_t *from,
    bcm_compat6515_port_match_info_t *to)
{
    to->match = from->match;
    to->port = from->port;
    to->match_vlan = from->match_vlan;
    to->match_vlan_max = from->match_vlan_max;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_inner_vlan_max = from->match_inner_vlan_max;
    to->match_tunnel_vlan = from->match_tunnel_vlan;
    sal_memcpy(&to->match_tunnel_srcmac, &from->match_tunnel_srcmac,
            sizeof(to->match_tunnel_srcmac));
    to->match_label = from->match_label;
    to->flags = from->flags;
    to->match_pon_tunnel = from->match_pon_tunnel;
    to->match_ethertype = from->match_ethertype;
    to->match_pcp = from->match_pcp;
    to->action = from->action;
    to->extended_port_vid = from->extended_port_vid;
    to->vpn = from->vpn;
    to->niv_port_vif = from->niv_port_vif;
    to->isid = from->isid;
}

/*
 * Function:
 *      bcm_compat6515_port_match_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_add.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match -(IN) Match criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *match)
{
    int rv = 0;
    bcm_port_match_info_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_match_info_t(match, new_match);
    }

    rv = bcm_port_match_add(unit, port, new_match);

    if (new_match != NULL) {
        sal_free(new_match);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_port_match_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_delete.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match -(IN) Match criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *match)
{
    int rv = 0;
    bcm_port_match_info_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_match_info_t(match, new_match);
    }

    rv = bcm_port_match_delete(unit, port, new_match);

    if (new_match != NULL) {
        sal_free(new_match);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_port_match_replace
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_replace.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      old_match - (IN) Old match criteria
 *      new_match - (IN) New match criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *old_match,
    bcm_compat6515_port_match_info_t *new_match)
{
    int rv = 0;
    bcm_port_match_info_t *new_old_match = NULL;
    bcm_port_match_info_t *new_new_match = NULL;

    if (old_match != NULL) {
        new_old_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (new_old_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_match_info_t(old_match, new_old_match);
    }
    if (new_match != NULL) {
        new_new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (new_new_match == NULL) {
            sal_free(new_old_match);
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_match_info_t(new_match, new_new_match);
    }

    rv = bcm_port_match_replace(unit, port, new_old_match, new_new_match);

    if (new_old_match != NULL) {
        sal_free(new_old_match);
    }
    if (new_new_match != NULL) {
        sal_free(new_new_match);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_port_match_multi_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_multi_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      size - (IN) Number of elements in match array
 *      match_array - (OUT) Match array
 *      count - (OUT) Match count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6515_port_match_info_t *match_array,
    int *count)
{
    int rv = 0, i = 0;
    bcm_port_match_info_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_match_info_t *)
            sal_alloc(size * sizeof(bcm_port_match_info_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
    }

    rv = bcm_port_match_multi_get(unit, port, size, new_match_array, count);

    if (new_match_array != NULL) {
        for (i = 0; i < size; i++) {
            _bcm_compat6515out_port_match_info_t(&new_match_array[i],
                    &match_array[i]);
        }
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_port_match_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      size - (IN) Number of elements in match array
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6515_port_match_info_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_match_info_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_match_info_t *)
            sal_alloc(size * sizeof(bcm_port_match_info_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < size; i++) {
            _bcm_compat6515in_port_match_info_t(&match_array[i],
                    &new_match_array[i]);
        }
    }

    rv = bcm_port_match_set(unit, port, size, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

#endif /* BCM_RPC_SUPPORT */
