/*
* $Id: compat_6517.c,v 1.0 2019/11/10
* $Copyright: (c) 2019 Broadcom.
* Broadcom Proprietary and Confidential. All rights reserved.$
*
* RPC Compatibility with sdk-6.5.17 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm/failover.h>
#include <bcm/mirror.h>
#include <bcm/ppp.h>
#include <bcm/tunnel.h>
#include <bcm/switch.h>
#include <bcm_int/compat/compat_6517.h>




/*
 * Function:
 *      _bcm_compat6517in_qos_egress_model_t
 * Purpose:
 *      Convert the bcm_qos_egress_model_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_qos_egress_model_t(
    bcm_compat6517_qos_egress_model_t *from,
    bcm_qos_egress_model_t *to)
{
    to->egress_qos = from->egress_qos;
    to->egress_ttl = from->egress_ttl;
    to->egress_ecn = from->ecn_eligible;
}

/*
 * Function:
 *      _bcm_compat6517out_qos_egress_model_t
 * Purpose:
 *      Convert the bcm_qos_egress_model_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_qos_egress_model_t(
    bcm_qos_egress_model_t *from,
    bcm_compat6517_qos_egress_model_t *to)
{
    to->egress_qos = from->egress_qos;
    to->egress_ttl = from->egress_ttl;
    to->ecn_eligible = (uint8)from->egress_ecn;
}

/*
 * Function:
 *      _bcm_compat6517in_mpls_egress_label_t
 * Purpose:
 *      Convert the bcm_mpls_egress_label_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_mpls_egress_label_t(
    bcm_compat6517_mpls_egress_label_t *from,
    bcm_mpls_egress_label_t *to)
{
    to->flags = from->flags;
    to->label = from->label;
    to->qos_map_id = from->qos_map_id;
    to->exp = from->exp;
    to->ttl = from->ttl;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->tunnel_id = from->tunnel_id;
    to->l3_intf_id = from->l3_intf_id;
    to->action = from->action;
    to->ecn_map_id = from->ecn_map_id;
    to->int_cn_map_id = from->int_cn_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_if_id = from->egress_failover_if_id;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->spl_label_push_type = from->spl_label_push_type;
    to->encap_access = from->encap_access;
    _bcm_compat6517in_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
}

/*
 * Function:
 *      _bcm_compat6517out_mpls_egress_label_t
 * Purpose:
 *      Convert the bcm_mpls_egress_label_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_mpls_egress_label_t(
    bcm_mpls_egress_label_t *from,
    bcm_compat6517_mpls_egress_label_t *to)
{
    to->flags = from->flags;
    to->label = from->label;
    to->qos_map_id = from->qos_map_id;
    to->exp = from->exp;
    to->ttl = from->ttl;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->tunnel_id = from->tunnel_id;
    to->l3_intf_id = from->l3_intf_id;
    to->action = from->action;
    to->ecn_map_id = from->ecn_map_id;
    to->int_cn_map_id = from->int_cn_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_if_id = from->egress_failover_if_id;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->spl_label_push_type = from->spl_label_push_type;
    to->encap_access = from->encap_access;
    _bcm_compat6517out_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_initiator_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_initiator_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) The egress L3 interface<br>The egress L3 interface
 *      num_labels - (IN) Number of labels in the array<br>Number of labels in the array
 *      label_array - (IN) Array of MPLS label and header information<br>(INOUT) Array of MPLS label and header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_compat6517_mpls_egress_label_t *label_array)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *new_label_array = NULL;
    int i = 0;

    if (label_array != NULL && num_labels > 0) {
        new_label_array = (bcm_mpls_egress_label_t *)
                     sal_alloc(num_labels * sizeof(bcm_mpls_egress_label_t),
                     "New label_array");
        if (new_label_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < num_labels; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6517in_mpls_egress_label_t(&label_array[i], &new_label_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_initiator_set(unit, intf, num_labels, new_label_array);


    /* Deallocate memory*/
    sal_free(new_label_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      num_labels - (IN) 
 *      label_array - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_compat6517_mpls_egress_label_t *label_array)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *new_label_array = NULL;
    int i = 0;

    if (label_array != NULL && num_labels > 0) {
        new_label_array = (bcm_mpls_egress_label_t *)
                     sal_alloc(num_labels * sizeof(bcm_mpls_egress_label_t),
                     "New label_array");
        if (new_label_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < num_labels; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6517in_mpls_egress_label_t(&label_array[i], &new_label_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_initiator_create(unit, intf, num_labels, new_label_array);

    for (i = 0; i < num_labels; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_mpls_egress_label_t(&new_label_array[i], &label_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_label_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) The egress L3 interface
 *      label_max - (IN) Number of entries in label_array
 *      label_array - (OUT) MPLS header information
 *      label_count - (OUT) Actual number of labels returned
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_compat6517_mpls_egress_label_t *label_array,
    int *label_count)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *new_label_array = NULL;
    int i = 0;

    if (label_array != NULL && label_max > 0) {
        new_label_array = (bcm_mpls_egress_label_t *)
                     sal_alloc(label_max * sizeof(bcm_mpls_egress_label_t),
                     "New label_array");
        if (new_label_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_initiator_get(unit, intf, label_max, new_label_array, label_count);

    for (i = 0; i < label_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_mpls_egress_label_t(&new_label_array[i], &label_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_label_array);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_mpls_port_t(
    bcm_compat6517_mpls_port_t *from,
    bcm_mpls_port_t *to)
{
    to->mpls_port_id = from->mpls_port_id;
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->if_class = from->if_class;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->service_tpid = from->service_tpid;
    to->port = from->port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_label = from->match_label;
    to->egress_tunnel_if = from->egress_tunnel_if;
    _bcm_compat6517in_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->mtu = from->mtu;
    to->egress_service_vlan = from->egress_service_vlan;
    to->pw_seq_number = from->pw_seq_number;
    to->encap_id = from->encap_id;
    to->ingress_failover_id = from->ingress_failover_id;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->policer_id = from->policer_id;
    to->failover_mc_group = from->failover_mc_group;
    to->pw_failover_id = from->pw_failover_id;
    to->pw_failover_port_id = from->pw_failover_port_id;
    to->vccv_type = from->vccv_type;
    to->network_group_id = from->network_group_id;
    to->match_subport_pkt_vid = from->match_subport_pkt_vid;
    to->tunnel_id = from->tunnel_id;
    to->per_flow_queue_base = from->per_flow_queue_base;
    to->qos_map_id = from->qos_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ecn_map_id = from->ecn_map_id;
    to->class_id = from->class_id;
    to->egress_class_id = from->egress_class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    _bcm_compat6517in_mpls_egress_label_t(&from->egress_tunnel_label, &to->egress_tunnel_label);
    to->nof_service_tags = from->nof_service_tags;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->context_label = from->context_label;
    to->ingress_if = from->ingress_if;
}

/*
 * Function:
 *      _bcm_compat6517out_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_mpls_port_t(
    bcm_mpls_port_t *from,
    bcm_compat6517_mpls_port_t *to)
{
    to->mpls_port_id = from->mpls_port_id;
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->if_class = from->if_class;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->service_tpid = from->service_tpid;
    to->port = from->port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_label = from->match_label;
    to->egress_tunnel_if = from->egress_tunnel_if;
    _bcm_compat6517out_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->mtu = from->mtu;
    to->egress_service_vlan = from->egress_service_vlan;
    to->pw_seq_number = from->pw_seq_number;
    to->encap_id = from->encap_id;
    to->ingress_failover_id = from->ingress_failover_id;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->policer_id = from->policer_id;
    to->failover_mc_group = from->failover_mc_group;
    to->pw_failover_id = from->pw_failover_id;
    to->pw_failover_port_id = from->pw_failover_port_id;
    to->vccv_type = from->vccv_type;
    to->network_group_id = from->network_group_id;
    to->match_subport_pkt_vid = from->match_subport_pkt_vid;
    to->tunnel_id = from->tunnel_id;
    to->per_flow_queue_base = from->per_flow_queue_base;
    to->qos_map_id = from->qos_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ecn_map_id = from->ecn_map_id;
    to->class_id = from->class_id;
    to->egress_class_id = from->egress_class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    _bcm_compat6517out_mpls_egress_label_t(&from->egress_tunnel_label, &to->egress_tunnel_label);
    to->nof_service_tags = from->nof_service_tags;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->context_label = from->context_label;
    to->ingress_if = from->ingress_if;
}

/*
 * Function:
 *      bcm_compat6517_mpls_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      mpls_port - (INOUT) (IN/OUT) MPLS port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6517_mpls_port_t *mpls_port)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (mpls_port != NULL) {
        new_mpls_port = (bcm_mpls_port_t *)
                     sal_alloc(sizeof(bcm_mpls_port_t),
                     "New mpls_port");
        if (new_mpls_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mpls_port_t(mpls_port, new_mpls_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_add(unit, vpn, new_mpls_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_mpls_port_t(new_mpls_port, mpls_port);

    /* Deallocate memory*/
    sal_free(new_mpls_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mpls_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      mpls_port - (INOUT) (IN/OUT) MPLS port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6517_mpls_port_t *mpls_port)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (mpls_port != NULL) {
        new_mpls_port = (bcm_mpls_port_t *)
                     sal_alloc(sizeof(bcm_mpls_port_t),
                     "New mpls_port");
        if (new_mpls_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mpls_port_t(mpls_port, new_mpls_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get(unit, vpn, new_mpls_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_mpls_port_t(new_mpls_port, mpls_port);

    /* Deallocate memory*/
    sal_free(new_mpls_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mpls_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      port_max - (IN) Maximum number of ports in array
 *      port_array - (OUT) Array of MPLS ports
 *      port_count - (OUT) Number of ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6517_mpls_port_t *port_array,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_port_array = NULL;
    int i = 0;

    if (port_array != NULL && port_max > 0) {
        new_port_array = (bcm_mpls_port_t *)
                     sal_alloc(port_max * sizeof(bcm_mpls_port_t),
                     "New port_array");
        if (new_port_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get_all(unit, vpn, port_max, new_port_array, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_mpls_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_port_array);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_mpls_tunnel_encap_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_encap_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_mpls_tunnel_encap_t(
    bcm_compat6517_mpls_tunnel_encap_t *from,
    bcm_mpls_tunnel_encap_t *to)
{
    int i1 = 0;

    to->tunnel_id = from->tunnel_id;
    to->num_labels = from->num_labels;
    for (i1 = 0; i1 < BCM_MPLS_EGRESS_LABEL_MAX; i1++) {
        _bcm_compat6517in_mpls_egress_label_t(&from->label_array[i1], &to->label_array[i1]);
    }
}

/*
 * Function:
 *      _bcm_compat6517out_mpls_tunnel_encap_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_encap_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_mpls_tunnel_encap_t(
    bcm_mpls_tunnel_encap_t *from,
    bcm_compat6517_mpls_tunnel_encap_t *to)
{
    int i1 = 0;

    to->tunnel_id = from->tunnel_id;
    to->num_labels = from->num_labels;
    for (i1 = 0; i1 < BCM_MPLS_EGRESS_LABEL_MAX; i1++) {
        _bcm_compat6517out_mpls_egress_label_t(&from->label_array[i1], &to->label_array[i1]);
    }
}

/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_encap_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_encap_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Operation options
 *      tunnel_encap - (INOUT) (IN/OUT) MPLS tunnel initiator information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_encap_create(
    int unit,
    uint32 options,
    bcm_compat6517_mpls_tunnel_encap_t *tunnel_encap)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_encap_t *new_tunnel_encap = NULL;

    if (tunnel_encap != NULL) {
        new_tunnel_encap = (bcm_mpls_tunnel_encap_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_encap_t),
                     "New tunnel_encap");
        if (new_tunnel_encap == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mpls_tunnel_encap_t(tunnel_encap, new_tunnel_encap);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_encap_create(unit, options, new_tunnel_encap);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_mpls_tunnel_encap_t(new_tunnel_encap, tunnel_encap);

    /* Deallocate memory*/
    sal_free(new_tunnel_encap);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_encap_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tunnel_encap - (INOUT) (IN/OUT) MPLS tunnel initiator information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_encap_get(
    int unit,
    bcm_compat6517_mpls_tunnel_encap_t *tunnel_encap)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_encap_t *new_tunnel_encap = NULL;

    if (tunnel_encap != NULL) {
        new_tunnel_encap = (bcm_mpls_tunnel_encap_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_encap_t),
                     "New tunnel_encap");
        if (new_tunnel_encap == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mpls_tunnel_encap_t(tunnel_encap, new_tunnel_encap);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_encap_get(unit, new_tunnel_encap);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_mpls_tunnel_encap_t(new_tunnel_encap, tunnel_encap);

    /* Deallocate memory*/
    sal_free(new_tunnel_encap);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_mpls_tunnel_switch_t(
    bcm_compat6517_mpls_tunnel_switch_t *from,
    bcm_mpls_tunnel_switch_t *to)
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
    _bcm_compat6517in_mpls_egress_label_t(&from->egress_label, &to->egress_label);
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
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
}

/*
 * Function:
 *      _bcm_compat6517out_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_mpls_tunnel_switch_t(
    bcm_mpls_tunnel_switch_t *from,
    bcm_compat6517_mpls_tunnel_switch_t *to)
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
    _bcm_compat6517out_mpls_egress_label_t(&from->egress_label, &to->egress_label);
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
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
}

/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_switch_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Label (switch) information<br>(INOUT) Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_switch_add(
    int unit,
    bcm_compat6517_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mpls_tunnel_switch_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_add(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_switch_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Label (switch) information<br>(INOUT) Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_switch_create(
    int unit,
    bcm_compat6517_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mpls_tunnel_switch_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_mpls_tunnel_switch_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_switch_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_switch_delete(
    int unit,
    bcm_compat6517_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mpls_tunnel_switch_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_delete(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mpls_tunnel_switch_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mpls_tunnel_switch_get(
    int unit,
    bcm_compat6517_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mpls_tunnel_switch_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_mpls_tunnel_switch_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}
#endif /*INCLUDE_L3*/

/*
 * Function:
 *      _bcm_compat6517in_oam_endpoint_info_t
 * Purpose:
 *      Convert the bcm_oam_endpoint_info_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_oam_endpoint_info_t(
    bcm_compat6517_oam_endpoint_info_t *from,
    bcm_oam_endpoint_info_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->opcode_flags = from->opcode_flags;
    to->lm_flags = from->lm_flags;
    to->id = from->id;
    to->type = from->type;
    to->group = from->group;
    to->name = from->name;
    to->local_id = from->local_id;
    to->level = from->level;
    to->ccm_period = from->ccm_period;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->trunk_index = from->trunk_index;
    to->intf_id = from->intf_id;
    to->mpls_label = from->mpls_label;
    _bcm_compat6517in_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    sal_memcpy(&to->mpls_network_info, &from->mpls_network_info, sizeof(bcm_oam_mpls_network_info_t));
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac_address[i1] = from->dst_mac_address[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_address[i1] = from->src_mac_address[i1];
    }
    to->pkt_pri = from->pkt_pri;
    to->inner_pkt_pri = from->inner_pkt_pri;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid = from->outer_tpid;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    for (i1 = 0; i1 < BCM_OAM_INTPRI_MAX; i1++) {
        to->pri_map[i1] = from->pri_map[i1];
    }
    to->faults = from->faults;
    to->persistent_faults = from->persistent_faults;
    to->clear_persistent_faults = from->clear_persistent_faults;
    to->ing_map = from->ing_map;
    to->egr_map = from->egr_map;
    to->ms_pw_ttl = from->ms_pw_ttl;
    to->port_state = from->port_state;
    to->interface_state = from->interface_state;
    to->vccv_type = from->vccv_type;
    to->vpn = from->vpn;
    to->lm_counter_base_id = from->lm_counter_base_id;
    to->lm_counter_if = from->lm_counter_if;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->timestamp_format = from->timestamp_format;
    to->subport_tpid = from->subport_tpid;
    to->remote_gport = from->remote_gport;
    to->mpls_out_gport = from->mpls_out_gport;
    to->sampling_ratio = from->sampling_ratio;
    to->lm_payload_offset = from->lm_payload_offset;
    to->lm_cos_offset = from->lm_cos_offset;
    to->lm_ctr_type = from->lm_ctr_type;
    to->lm_ctr_sample_size = from->lm_ctr_sample_size;
    to->pri_map_id = from->pri_map_id;
    to->lm_ctr_pool_id = from->lm_ctr_pool_id;
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_base_id[i1] = from->ccm_tx_update_lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_offset[i1] = from->ccm_tx_update_lm_counter_offset[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_action[i1] = from->ccm_tx_update_lm_counter_action[i1];
    }
    to->ccm_tx_update_lm_counter_size = from->ccm_tx_update_lm_counter_size;
    to->session_id = from->session_id;
    to->session_num_entries = from->session_num_entries;
    to->lm_count_profile = from->lm_count_profile;
    to->mpls_exp = from->mpls_exp;
    to->action_reference_id = from->action_reference_id;
    to->acc_profile_id = from->acc_profile_id;
    to->endpoint_memory_type = from->endpoint_memory_type;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->extra_data_index = from->extra_data_index;
}

/*
 * Function:
 *      _bcm_compat6517out_oam_endpoint_info_t
 * Purpose:
 *      Convert the bcm_oam_endpoint_info_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_oam_endpoint_info_t(
    bcm_oam_endpoint_info_t *from,
    bcm_compat6517_oam_endpoint_info_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->opcode_flags = from->opcode_flags;
    to->lm_flags = from->lm_flags;
    to->id = from->id;
    to->type = from->type;
    to->group = from->group;
    to->name = from->name;
    to->local_id = from->local_id;
    to->level = from->level;
    to->ccm_period = from->ccm_period;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->trunk_index = from->trunk_index;
    to->intf_id = from->intf_id;
    to->mpls_label = from->mpls_label;
    _bcm_compat6517out_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    sal_memcpy(&to->mpls_network_info, &from->mpls_network_info, sizeof(bcm_oam_mpls_network_info_t));
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac_address[i1] = from->dst_mac_address[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_address[i1] = from->src_mac_address[i1];
    }
    to->pkt_pri = from->pkt_pri;
    to->inner_pkt_pri = from->inner_pkt_pri;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid = from->outer_tpid;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    for (i1 = 0; i1 < BCM_OAM_INTPRI_MAX; i1++) {
        to->pri_map[i1] = from->pri_map[i1];
    }
    to->faults = from->faults;
    to->persistent_faults = from->persistent_faults;
    to->clear_persistent_faults = from->clear_persistent_faults;
    to->ing_map = from->ing_map;
    to->egr_map = from->egr_map;
    to->ms_pw_ttl = from->ms_pw_ttl;
    to->port_state = from->port_state;
    to->interface_state = from->interface_state;
    to->vccv_type = from->vccv_type;
    to->vpn = from->vpn;
    to->lm_counter_base_id = from->lm_counter_base_id;
    to->lm_counter_if = from->lm_counter_if;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->timestamp_format = from->timestamp_format;
    to->subport_tpid = from->subport_tpid;
    to->remote_gport = from->remote_gport;
    to->mpls_out_gport = from->mpls_out_gport;
    to->sampling_ratio = from->sampling_ratio;
    to->lm_payload_offset = from->lm_payload_offset;
    to->lm_cos_offset = from->lm_cos_offset;
    to->lm_ctr_type = from->lm_ctr_type;
    to->lm_ctr_sample_size = from->lm_ctr_sample_size;
    to->pri_map_id = from->pri_map_id;
    to->lm_ctr_pool_id = from->lm_ctr_pool_id;
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_base_id[i1] = from->ccm_tx_update_lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_offset[i1] = from->ccm_tx_update_lm_counter_offset[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_action[i1] = from->ccm_tx_update_lm_counter_action[i1];
    }
    to->ccm_tx_update_lm_counter_size = from->ccm_tx_update_lm_counter_size;
    to->session_id = from->session_id;
    to->session_num_entries = from->session_num_entries;
    to->lm_count_profile = from->lm_count_profile;
    to->mpls_exp = from->mpls_exp;
    to->action_reference_id = from->action_reference_id;
    to->acc_profile_id = from->acc_profile_id;
    to->endpoint_memory_type = from->endpoint_memory_type;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->extra_data_index = from->extra_data_index;
}

/*
 * Function:
 *      bcm_compat6517_oam_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_endpoint_create.
 * Parameters:
 *      unit - (IN) BCM device number
 *      endpoint_info - (INOUT) (IN/OUT) Pointer to an OAM endpoint info structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_oam_endpoint_create(
    int unit,
    bcm_compat6517_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_oam_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_oam_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_oam_endpoint_info_t(endpoint_info, new_endpoint_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_endpoint_create(unit, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_oam_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_oam_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_endpoint_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      endpoint - (IN) ID of the endpoint to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint info structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_oam_endpoint_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_compat6517_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_oam_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_oam_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_endpoint_get(unit, endpoint, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_oam_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_srv6_srh_base_initiator_info_t
 * Purpose:
 *      Convert the bcm_srv6_srh_base_initiator_info_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_srv6_srh_base_initiator_info_t(
    bcm_compat6517_srv6_srh_base_initiator_info_t *from,
    bcm_srv6_srh_base_initiator_info_t *to)
{
    to->flags = from->flags;
    to->tunnel_id = from->tunnel_id;
    to->nof_sids = from->nof_sids;
    to->qos_map_id = from->qos_map_id;
    to->ttl = from->ttl;
    to->dscp = from->dscp;
    _bcm_compat6517in_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
    to->next_encap_id = from->next_encap_id;
}

/*
 * Function:
 *      _bcm_compat6517out_srv6_srh_base_initiator_info_t
 * Purpose:
 *      Convert the bcm_srv6_srh_base_initiator_info_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_srv6_srh_base_initiator_info_t(
    bcm_srv6_srh_base_initiator_info_t *from,
    bcm_compat6517_srv6_srh_base_initiator_info_t *to)
{
    to->flags = from->flags;
    to->tunnel_id = from->tunnel_id;
    to->nof_sids = from->nof_sids;
    to->qos_map_id = from->qos_map_id;
    to->ttl = from->ttl;
    to->dscp = from->dscp;
    _bcm_compat6517out_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
    to->next_encap_id = from->next_encap_id;
}

/*
 * Function:
 *      bcm_compat6517_srv6_srh_base_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_srv6_srh_base_initiator_create(
    int unit,
    bcm_compat6517_srv6_srh_base_initiator_info_t *info)
{
    int rv = BCM_E_NONE;
    bcm_srv6_srh_base_initiator_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_srv6_srh_base_initiator_info_t *)
                     sal_alloc(sizeof(bcm_srv6_srh_base_initiator_info_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_srv6_srh_base_initiator_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_srv6_srh_base_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_srv6_srh_base_initiator_info_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_srv6_srh_base_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_srv6_srh_base_initiator_get(
    int unit,
    bcm_compat6517_srv6_srh_base_initiator_info_t *info)
{
    int rv = BCM_E_NONE;
    bcm_srv6_srh_base_initiator_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_srv6_srh_base_initiator_info_t *)
                     sal_alloc(sizeof(bcm_srv6_srh_base_initiator_info_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_srv6_srh_base_initiator_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_srv6_srh_base_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_srv6_srh_base_initiator_info_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_srv6_srh_base_initiator_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_srv6_srh_base_initiator_delete(
    int unit,
    bcm_compat6517_srv6_srh_base_initiator_info_t *info)
{
    int rv = BCM_E_NONE;
    bcm_srv6_srh_base_initiator_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_srv6_srh_base_initiator_info_t *)
                     sal_alloc(sizeof(bcm_srv6_srh_base_initiator_info_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_srv6_srh_base_initiator_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_srv6_srh_base_initiator_delete(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_vlan_port_t(
    bcm_compat6517_vlan_port_t *from,
    bcm_vlan_port_t *to)
{
    to->criteria = from->criteria;
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_pcp = from->match_pcp;
    to->match_tunnel_value = from->match_tunnel_value;
    to->match_ethertype = from->match_ethertype;
    to->port = from->port;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_vlan = from->egress_vlan;
    to->egress_inner_vlan = from->egress_inner_vlan;
    to->egress_tunnel_value = from->egress_tunnel_value;
    to->encap_id = from->encap_id;
    to->qos_map_id = from->qos_map_id;
    to->policer_id = from->policer_id;
    to->egress_policer_id = from->egress_policer_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->vlan_port_id = from->vlan_port_id;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id = from->egress_network_group_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->if_class = from->if_class;
    to->tunnel_id = from->tunnel_id;
    to->group = from->group;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->class_id = from->class_id;
    to->match_class_id = from->match_class_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    _bcm_compat6517in_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
}

/*
 * Function:
 *      _bcm_compat6517out_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_vlan_port_t(
    bcm_vlan_port_t *from,
    bcm_compat6517_vlan_port_t *to)
{
    to->criteria = from->criteria;
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_pcp = from->match_pcp;
    to->match_tunnel_value = from->match_tunnel_value;
    to->match_ethertype = from->match_ethertype;
    to->port = from->port;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_vlan = from->egress_vlan;
    to->egress_inner_vlan = from->egress_inner_vlan;
    to->egress_tunnel_value = from->egress_tunnel_value;
    to->encap_id = from->encap_id;
    to->qos_map_id = from->qos_map_id;
    to->policer_id = from->policer_id;
    to->egress_policer_id = from->egress_policer_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->vlan_port_id = from->vlan_port_id;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id = from->egress_network_group_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->if_class = from->if_class;
    to->tunnel_id = from->tunnel_id;
    to->group = from->group;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->class_id = from->class_id;
    to->match_class_id = from->match_class_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    _bcm_compat6517out_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
}

/*
 * Function:
 *      bcm_compat6517_vlan_port_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_port - (INOUT) (IN/OUT) Layer 2 Logical port.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_vlan_port_create(
    int unit,
    bcm_compat6517_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (vlan_port != NULL) {
        new_vlan_port = (bcm_vlan_port_t *)
                     sal_alloc(sizeof(bcm_vlan_port_t),
                     "New vlan_port");
        if (new_vlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_create(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_vlan_port_t(new_vlan_port, vlan_port);

    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_vlan_port_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_port - (INOUT) (IN/OUT) Layer 2 logical port
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_vlan_port_find(
    int unit,
    bcm_compat6517_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (vlan_port != NULL) {
        new_vlan_port = (bcm_vlan_port_t *)
                     sal_alloc(sizeof(bcm_vlan_port_t),
                     "New vlan_port");
        if (new_vlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_find(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_vlan_port_t(new_vlan_port, vlan_port);

    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}


#if defined(INCLUDE_BFD)
/*
 * Function:
 *      _bcm_compat6517in_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_bfd_endpoint_info_t(
    bcm_compat6517_bfd_endpoint_info_t *from,
    bcm_bfd_endpoint_info_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->type = from->type;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->remote_gport = from->remote_gport;
    to->bfd_period = from->bfd_period;
    to->vpn = from->vpn;
    to->vlan_pri = from->vlan_pri;
    to->inner_vlan_pri = from->inner_vlan_pri;
    to->vrf_id = from->vrf_id;
    to->dst_ip_addr = from->dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->dst_ip6_addr[i1] = from->dst_ip6_addr[i1];
    }
    to->src_ip_addr = from->src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->src_ip6_addr[i1] = from->src_ip6_addr[i1];
    }
    to->ip_tos = from->ip_tos;
    to->ip_ttl = from->ip_ttl;
    to->inner_dst_ip_addr = from->inner_dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_dst_ip6_addr[i1] = from->inner_dst_ip6_addr[i1];
    }
    to->inner_src_ip_addr = from->inner_src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_src_ip6_addr[i1] = from->inner_src_ip6_addr[i1];
    }
    to->inner_ip_tos = from->inner_ip_tos;
    to->inner_ip_ttl = from->inner_ip_ttl;
    to->udp_src_port = from->udp_src_port;
    to->label = from->label;
    _bcm_compat6517in_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->egress_if = from->egress_if;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mep_id[i1] = from->mep_id[i1];
    }
    to->mep_id_length = from->mep_id_length;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    to->local_state = from->local_state;
    to->local_discr = from->local_discr;
    to->local_diag = from->local_diag;
    to->local_flags = from->local_flags;
    to->local_min_tx = from->local_min_tx;
    to->local_min_rx = from->local_min_rx;
    to->local_min_echo = from->local_min_echo;
    to->local_detect_mult = from->local_detect_mult;
    to->auth = from->auth;
    to->auth_index = from->auth_index;
    to->tx_auth_seq = from->tx_auth_seq;
    to->rx_auth_seq = from->rx_auth_seq;
    to->remote_flags = from->remote_flags;
    to->remote_state = from->remote_state;
    to->remote_discr = from->remote_discr;
    to->remote_diag = from->remote_diag;
    to->remote_min_tx = from->remote_min_tx;
    to->remote_min_rx = from->remote_min_rx;
    to->remote_min_echo = from->remote_min_echo;
    to->remote_detect_mult = from->remote_detect_mult;
    to->sampling_ratio = from->sampling_ratio;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->ip_subnet_length = from->ip_subnet_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->remote_mep_id[i1] = from->remote_mep_id[i1];
    }
    to->remote_mep_id_length = from->remote_mep_id_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mis_conn_mep_id[i1] = from->mis_conn_mep_id[i1];
    }
    to->mis_conn_mep_id_length = from->mis_conn_mep_id_length;
    to->bfd_detection_time = from->bfd_detection_time;
    to->pkt_vlan_id = from->pkt_vlan_id;
    to->rx_pkt_vlan_id = from->rx_pkt_vlan_id;
    to->gal_label = from->gal_label;
    to->faults = from->faults;
    to->flags2 = from->flags2;
    to->ipv6_extra_data_index = from->ipv6_extra_data_index;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->bfd_period_cluster = from->bfd_period_cluster;
}

/*
 * Function:
 *      _bcm_compat6517out_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_bfd_endpoint_info_t(
    bcm_bfd_endpoint_info_t *from,
    bcm_compat6517_bfd_endpoint_info_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->type = from->type;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->remote_gport = from->remote_gport;
    to->bfd_period = from->bfd_period;
    to->vpn = from->vpn;
    to->vlan_pri = from->vlan_pri;
    to->inner_vlan_pri = from->inner_vlan_pri;
    to->vrf_id = from->vrf_id;
    to->dst_ip_addr = from->dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->dst_ip6_addr[i1] = from->dst_ip6_addr[i1];
    }
    to->src_ip_addr = from->src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->src_ip6_addr[i1] = from->src_ip6_addr[i1];
    }
    to->ip_tos = from->ip_tos;
    to->ip_ttl = from->ip_ttl;
    to->inner_dst_ip_addr = from->inner_dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_dst_ip6_addr[i1] = from->inner_dst_ip6_addr[i1];
    }
    to->inner_src_ip_addr = from->inner_src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_src_ip6_addr[i1] = from->inner_src_ip6_addr[i1];
    }
    to->inner_ip_tos = from->inner_ip_tos;
    to->inner_ip_ttl = from->inner_ip_ttl;
    to->udp_src_port = from->udp_src_port;
    to->label = from->label;
    _bcm_compat6517out_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->egress_if = from->egress_if;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mep_id[i1] = from->mep_id[i1];
    }
    to->mep_id_length = from->mep_id_length;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    to->local_state = from->local_state;
    to->local_discr = from->local_discr;
    to->local_diag = from->local_diag;
    to->local_flags = from->local_flags;
    to->local_min_tx = from->local_min_tx;
    to->local_min_rx = from->local_min_rx;
    to->local_min_echo = from->local_min_echo;
    to->local_detect_mult = from->local_detect_mult;
    to->auth = from->auth;
    to->auth_index = from->auth_index;
    to->tx_auth_seq = from->tx_auth_seq;
    to->rx_auth_seq = from->rx_auth_seq;
    to->remote_flags = from->remote_flags;
    to->remote_state = from->remote_state;
    to->remote_discr = from->remote_discr;
    to->remote_diag = from->remote_diag;
    to->remote_min_tx = from->remote_min_tx;
    to->remote_min_rx = from->remote_min_rx;
    to->remote_min_echo = from->remote_min_echo;
    to->remote_detect_mult = from->remote_detect_mult;
    to->sampling_ratio = from->sampling_ratio;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->ip_subnet_length = from->ip_subnet_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->remote_mep_id[i1] = from->remote_mep_id[i1];
    }
    to->remote_mep_id_length = from->remote_mep_id_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mis_conn_mep_id[i1] = from->mis_conn_mep_id[i1];
    }
    to->mis_conn_mep_id_length = from->mis_conn_mep_id_length;
    to->bfd_detection_time = from->bfd_detection_time;
    to->pkt_vlan_id = from->pkt_vlan_id;
    to->rx_pkt_vlan_id = from->rx_pkt_vlan_id;
    to->gal_label = from->gal_label;
    to->faults = from->faults;
    to->flags2 = from->flags2;
    to->ipv6_extra_data_index = from->ipv6_extra_data_index;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->bfd_period_cluster = from->bfd_period_cluster;
}

/*
 * Function:
 *      bcm_compat6517_bfd_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (INOUT) Pointer to an BFD endpoint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_bfd_endpoint_create(
    int unit,
    bcm_compat6517_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_bfd_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_bfd_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_bfd_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_bfd_endpoint_info_t(endpoint_info, new_endpoint_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_endpoint_create(unit, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_bfd_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an BFD endpoint structure to receive the data.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6517_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_bfd_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_bfd_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_bfd_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_endpoint_get(unit, endpoint, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}


#endif /* defined(INCLUDE_BFD) */
#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6517in_l3_intf_t
 * Purpose:
 *      Convert the bcm_l3_intf_t datatype from <=6.5.17 to
 *      SDK 6.5.17+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.17+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_l3_intf_t(
    bcm_compat6517_l3_intf_t *from,
    bcm_l3_intf_t *to)
{
    int i = 0;

    bcm_l3_intf_t_init(to);
    to->l3a_flags                  = from->l3a_flags;
    to->l3a_flags2                 = from->l3a_flags2;
    to->l3a_vrf                    = from->l3a_vrf;
    to->l3a_intf_id                = from->l3a_intf_id;
    sal_memcpy(to->l3a_mac_addr, from->l3a_mac_addr, sizeof(bcm_mac_t));
    to->l3a_vid                    = from->l3a_vid;
    to->l3a_inner_vlan             = from->l3a_inner_vlan;
    to->l3a_tunnel_idx             = from->l3a_tunnel_idx;
    to->l3a_ttl                    = from->l3a_ttl;
    to->l3a_mtu                    = from->l3a_mtu;
    to->l3a_mtu_forwarding         = from->l3a_mtu_forwarding;
    to->l3a_group                  = from->l3a_group;
    to->vlan_qos                   = from->vlan_qos;
    to->inner_vlan_qos             = from->inner_vlan_qos;
    to->dscp_qos                   = from->dscp_qos;
    to->l3a_intf_class             = from->l3a_intf_class;
    to->l3a_ip4_options_profile_id = from->l3a_ip4_options_profile_id;
    to->l3a_nat_realm_id           = from->l3a_nat_realm_id;
    to->outer_tpid                 = from->outer_tpid;
    to->l3a_intf_flags             = from->l3a_intf_flags;
    to->native_routing_vlan_tags   = from->native_routing_vlan_tags;
    to->l3a_source_vp              = from->l3a_source_vp;
    to->flow_handle                = from->flow_handle;
    to->flow_option_handle         = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i]      = from->logical_fields[i];
    }
    to->num_of_fields              = from->num_of_fields;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    _bcm_compat6517in_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
    to->stat_id                    = from->stat_id;
    to->stat_pp_profile            = from->stat_pp_profile;
    return;
}


/*
 * Function:
 *      _bcm_compat6517out_l3_intf_t
 * Purpose:
 *      Convert the bcm_l3_intf_t datatype from 6.5.17+ to
 *      SDK <=6.5.17
 * Parameters:
 *      from        - (IN) The 6.5.17+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_l3_intf_t(
    bcm_l3_intf_t *from,
    bcm_compat6517_l3_intf_t *to)
{
    int i = 0;

    to->l3a_flags                  = from->l3a_flags;
    to->l3a_flags2                 = from->l3a_flags2;
    to->l3a_vrf                    = from->l3a_vrf;
    to->l3a_intf_id                = from->l3a_intf_id;
    sal_memcpy(to->l3a_mac_addr, from->l3a_mac_addr, sizeof(bcm_mac_t));
    to->l3a_vid                    = from->l3a_vid;
    to->l3a_inner_vlan             = from->l3a_inner_vlan;
    to->l3a_tunnel_idx             = from->l3a_tunnel_idx;
    to->l3a_ttl                    = from->l3a_ttl;
    to->l3a_mtu                    = from->l3a_mtu;
    to->l3a_mtu_forwarding         = from->l3a_mtu_forwarding;
    to->l3a_group                  = from->l3a_group;
    to->vlan_qos                   = from->vlan_qos;
    to->inner_vlan_qos             = from->inner_vlan_qos;
    to->dscp_qos                   = from->dscp_qos;
    to->l3a_intf_class             = from->l3a_intf_class;
    to->l3a_ip4_options_profile_id = from->l3a_ip4_options_profile_id;
    to->l3a_nat_realm_id           = from->l3a_nat_realm_id;
    to->outer_tpid                 = from->outer_tpid;
    to->l3a_intf_flags             = from->l3a_intf_flags;
    to->native_routing_vlan_tags   = from->native_routing_vlan_tags;
    to->l3a_source_vp              = from->l3a_source_vp;
    to->flow_handle                = from->flow_handle;
    to->flow_option_handle         = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i]      = from->logical_fields[i];
    }
    to->num_of_fields              = from->num_of_fields;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    _bcm_compat6517out_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
    to->stat_id                    = from->stat_id;
    to->stat_pp_profile            = from->stat_pp_profile;
    return;
}

int
bcm_compat6517_l3_intf_create(
    int unit,
    bcm_compat6517_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_create(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6517_l3_intf_delete(
    int unit,
    bcm_compat6517_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_delete(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6517_l3_intf_find(
    int unit,
    bcm_compat6517_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_find(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6517_l3_intf_find_vlan(
    int unit,
    bcm_compat6517_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_find_vlan(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6517_l3_intf_get(
    int unit,
    bcm_compat6517_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_get(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}


/*
 * Function:
 *      _bcm_compat6517in_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_tunnel_initiator_t(
    bcm_compat6517_tunnel_initiator_t *from,
    bcm_tunnel_initiator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    _bcm_compat6517in_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
    to->qos_map_id = from->qos_map_id;
    to->ecn = from->ecn;
    to->flow_label_sel = from->flow_label_sel;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
}

/*
 * Function:
 *      _bcm_compat6517out_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_tunnel_initiator_t(
    bcm_tunnel_initiator_t *from,
    bcm_compat6517_tunnel_initiator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    _bcm_compat6517out_qos_egress_model_t(&from->egress_qos_model, &to->egress_qos_model);
    to->qos_map_id = from->qos_map_id;
    to->ecn = from->ecn;
    to->flow_label_sel = from->flow_label_sel;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
}

/*
 * Function:
 *      bcm_compat6517_tunnel_initiator_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      tunnel - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_tunnel_initiator_set(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6517_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            sal_free(new_intf);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_set(unit, new_intf, new_tunnel);


    /* Deallocate memory*/
    sal_free(new_intf);
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (INOUT) 
 *      tunnel - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_tunnel_initiator_create(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6517_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            sal_free(new_intf);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_create(unit, new_intf, new_tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_l3_intf_t(new_intf, intf);
    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_tunnel_initiator_t(new_tunnel, tunnel);

    /* Deallocate memory*/
    sal_free(new_intf);
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_tunnel_initiator_clear
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_clear.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_tunnel_initiator_clear(
    int unit,
    bcm_compat6517_l3_intf_t *intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_clear(unit, new_intf);


    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      tunnel - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_tunnel_initiator_get(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6517_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_intf_t(intf, new_intf);
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            sal_free(new_intf);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_get(unit, new_intf, new_tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_tunnel_initiator_t(new_tunnel, tunnel);

    /* Deallocate memory*/
    sal_free(new_intf);
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_l3_ingress_t
 * Purpose:
 *      Convert the bcm_l3_ingress_t datatype from <=6.5.17 to
 *      SDK 6.5.17+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.17+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_l3_ingress_t(
    bcm_compat6517_l3_ingress_t *from,
    bcm_l3_ingress_t *to)
{
    bcm_l3_ingress_t_init(to);
    to->flags                    = from->flags;
    to->vrf                      = from->vrf;
    to->urpf_mode                = from->urpf_mode;
    to->intf_class               = from->intf_class;
    to->ipmc_intf_id             = from->ipmc_intf_id;
    to->qos_map_id               = from->qos_map_id;
    to->ip4_options_profile_id   = from->ip4_options_profile_id;
    to->nat_realm_id             = from->nat_realm_id;
    to->tunnel_term_ecn_map_id   = from->tunnel_term_ecn_map_id;
    to->intf_class_route_disable = from->intf_class_route_disable;
    to->oam_default_profile      = from->oam_default_profile;
    to->hash_layers_disable      = from->hash_layers_disable;
    return;
}

/*
 * Function:
 *      _bcm_compat6517in_l3_ingress_t
 * Purpose:
 *      Convert the bcm_l3_ingress_t datatype from 6.5.17+ to
 *      SDK <=6.5.17
 * Parameters:
 *      from        - (IN) The 6.5.17+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_l3_ingress_t(
    bcm_l3_ingress_t *from,
    bcm_compat6517_l3_ingress_t *to)
{
    to->flags                    = from->flags;
    to->vrf                      = from->vrf;
    to->urpf_mode                = from->urpf_mode;
    to->intf_class               = from->intf_class;
    to->ipmc_intf_id             = from->ipmc_intf_id;
    to->qos_map_id               = from->qos_map_id;
    to->ip4_options_profile_id   = from->ip4_options_profile_id;
    to->nat_realm_id             = from->nat_realm_id;
    to->tunnel_term_ecn_map_id   = from->tunnel_term_ecn_map_id;
    to->intf_class_route_disable = from->intf_class_route_disable;
    to->oam_default_profile      = from->oam_default_profile;
    to->hash_layers_disable      = from->hash_layers_disable;
    return;
}

int
bcm_compat6517_l3_ingress_create(
    int unit,
    bcm_compat6517_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id)
{
    int rv;
    bcm_l3_ingress_t *new_ing_intf = NULL;

    if (NULL != ing_intf) {
        /* Create from heap to avoid the stack to bloat */
        new_ing_intf = sal_alloc(sizeof(bcm_l3_ingress_t), "New ingress");
        if (NULL == new_ing_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_ingress_t(ing_intf, new_ing_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ingress_create(unit, new_ing_intf, intf_id);
    if (NULL != new_ing_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_ingress_t(new_ing_intf, ing_intf);
        /* Deallocate memory*/
        sal_free(new_ing_intf);
    }

    return rv;
}

int
bcm_compat6517_l3_ingress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6517_l3_ingress_t *ing_intf
    )
{
    int rv;
    bcm_l3_ingress_t *new_ing_intf = NULL;

    if (NULL != ing_intf) {
        /* Create from heap to avoid the stack to bloat */
        new_ing_intf = sal_alloc(sizeof(bcm_l3_ingress_t), "New ingress");
        if (NULL == new_ing_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_ingress_t(ing_intf, new_ing_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ingress_get(unit, intf, new_ing_intf);
    if (NULL != new_ing_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_ingress_t(new_ing_intf, ing_intf);
        /* Deallocate memory*/
        sal_free(new_ing_intf);
    }

    return rv;
}

int
bcm_compat6517_l3_ingress_find(
    int unit,
    bcm_compat6517_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id)
{
    int rv;
    bcm_l3_ingress_t *new_ing_intf = NULL;

    if (NULL != ing_intf) {
        /* Create from heap to avoid the stack to bloat */
        new_ing_intf = sal_alloc(sizeof(bcm_l3_ingress_t), "New ingress");
        if (NULL == new_ing_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_ingress_t(ing_intf, new_ing_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ingress_find(unit, new_ing_intf, intf_id);
    if (NULL != new_ing_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_ingress_t(new_ing_intf, ing_intf);
        /* Deallocate memory*/
        sal_free(new_ing_intf);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from <=6.5.17 to
 *      SDK 6.5.17+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.17+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_l3_egress_t(
    bcm_compat6517_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    int i = 0;

    bcm_l3_egress_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    to->vntag = from->vntag;
    to->vntag_action = from->vntag_action;
    to->etag = from->etag;
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    sal_memcpy(to->src_mac_addr, from->src_mac_addr, sizeof(bcm_mac_t));
    to->hierarchical_gport = from->hierarchical_gport;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->vlan_port_id = from->vlan_port_id;
    to->replication_id = from->replication_id;
    to->mtu = from->mtu;
}

/*
 * Function:
 *      _bcm_compat6517out_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from 6.5.17+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.17+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6517_l3_egress_t *to)
{
    int i = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    to->vntag = from->vntag;
    to->vntag_action = from->vntag_action;
    to->etag = from->etag;
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    sal_memcpy(to->src_mac_addr, from->src_mac_addr, sizeof(bcm_mac_t));
    to->hierarchical_gport = from->hierarchical_gport;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->vlan_port_id = from->vlan_port_id;
    to->replication_id = from->replication_id;
    to->mtu = from->mtu;
}

int
bcm_compat6517_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6517_l3_egress_t *egr,
    bcm_if_t *intf)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_create(unit, flags, new_egr, intf);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6517_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6517_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_get(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6517_l3_egress_find(
    int unit,
    bcm_compat6517_l3_egress_t *egr,
    bcm_if_t *intf)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_find(unit, new_egr, intf);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6517_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6517_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_set(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6517_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6517_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_get(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_l3_ecmp_member_t
 * Purpose:
 *      Convert the bcm_l3_ecmp_member_t datatype from <=6.5.17 to
 *      SDK 6.5.17+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.17+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_l3_ecmp_member_t(
    bcm_compat6517_l3_ecmp_member_t *from,
    bcm_l3_ecmp_member_t *to)
{
    bcm_l3_ecmp_member_t_init(to);
    to->flags                = from->flags;
    to->egress_if            = from->egress_if;
    to->failover_id          = from->failover_id;
    to->failover_egress_if   = from->failover_egress_if;
    to->status               = from->status;
    to->weight               = from->weight;
    return;
}

/*
 * Function:
 *      _bcm_compat6517out_l3_ecmp_member_t
 * Purpose:
 *      Convert the bcm_l3_ecmp_member_t datatype from 6.5.17+ to
 *      SDK <=6.5.17
 * Parameters:
 *      from        - (IN) The 6.5.17+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_l3_ecmp_member_t(
    bcm_l3_ecmp_member_t *from,
    bcm_compat6517_l3_ecmp_member_t *to)
{
    to->flags                = from->flags;
    to->egress_if            = from->egress_if;
    to->failover_id          = from->failover_id;
    to->failover_egress_if   = from->failover_egress_if;
    to->status               = from->status;
    to->weight               = from->weight;
    return;
}

int
bcm_compat6517_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member_array)
{
    int sz, i, rv;
    bcm_l3_ecmp_member_t *new_member_array = NULL;

    if (ecmp_member_array && (ecmp_member_count > 0)) {
        sz = sizeof(bcm_l3_ecmp_member_t) * ecmp_member_count;
        new_member_array = sal_alloc(sz, "New ECMP Member Array");
        if (new_member_array == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < ecmp_member_count; i++) {
            _bcm_compat6517in_l3_ecmp_member_t(&ecmp_member_array[i],
                                               &new_member_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_create(unit, options, ecmp_info, ecmp_member_count,
                            new_member_array);

    if (new_member_array != NULL) {
        sal_free(new_member_array);
    }

    return rv;
}

int
bcm_compat6517_l3_ecmp_get(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count)
{
    int sz, i, rv;
    bcm_l3_ecmp_member_t *new_member_array = NULL;

    if (ecmp_member_array && (ecmp_member_size > 0)) {
        sz = sizeof(bcm_l3_ecmp_member_t) * ecmp_member_size;
        new_member_array = sal_alloc(sz, "New ECMP Member Array");
        if (new_member_array == NULL) {
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_get(unit, ecmp_info, ecmp_member_size, new_member_array,
                         ecmp_member_count);

    if (ecmp_member_array && (ecmp_member_size > 0)) {
        if (BCM_SUCCESS(rv)) {
            /* Transform the entry from the new format to old one */
            for (i = 0; i < *ecmp_member_count; i++) {
                _bcm_compat6517out_l3_ecmp_member_t(&new_member_array[i],
                                                    &ecmp_member_array[i]);
            }
        }
        sal_free(new_member_array);
    }

    return rv;
}

int
bcm_compat6517_l3_ecmp_member_add(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member)
{
    int sz, rv;
    bcm_l3_ecmp_member_t *new_member = NULL;

    if (ecmp_member) {
        /* Create from heap to avoid the stack to bloat */
        sz = sizeof(bcm_l3_ecmp_member_t);
        new_member = sal_alloc(sz, "New ECMP Member");
        if (new_member == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_ecmp_member_t(ecmp_member, new_member);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_member_add(unit, ecmp_group_id, new_member);

    if (new_member != NULL) {
        sal_free(new_member);
    }

    return rv;
}

int
bcm_compat6517_l3_ecmp_member_delete(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member)
{
    int sz, rv;
    bcm_l3_ecmp_member_t *new_member = NULL;

    if (ecmp_member) {
        /* Create from heap to avoid the stack to bloat */
        sz = sizeof(bcm_l3_ecmp_member_t);
        new_member = sal_alloc(sz, "New ECMP Member");
        if (new_member == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_l3_ecmp_member_t(ecmp_member, new_member);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_member_delete(unit, ecmp_group_id, new_member);

    if (new_member != NULL) {
        sal_free(new_member);
    }

    return rv;
}

int
bcm_compat6517_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member_array,
    bcm_l3_egress_ecmp_t *ecmp_info)
{
    int sz, i, rv;
    bcm_l3_ecmp_member_t *new_member_array = NULL;

    if (ecmp_member_array && (ecmp_member_count > 0)) {
        sz = sizeof(bcm_l3_ecmp_member_t) * ecmp_member_count;
        new_member_array = sal_alloc(sz, "New ECMP Member Array");
        if (new_member_array == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < ecmp_member_count; i++) {
            _bcm_compat6517in_l3_ecmp_member_t(&ecmp_member_array[i],
                                               &new_member_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_find(unit, ecmp_member_count, new_member_array, ecmp_info);

    if (new_member_array != NULL) {
        sal_free(new_member_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_vxlan_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_vxlan_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_wlan_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_wlan_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Tunnel initiator structure. \ref bcm_tunnel_initiator_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_wlan_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_wlan_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_wlan_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel initiator structure. \ref bcm_tunnel_initiator_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_wlan_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_l2gre_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_l2gre_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

#endif  /* defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_compat6517in_port_config_t
 * Purpose:
 *      Convert the bcm_port_config_t datatype from <=6.5.17 to
 *      SDK 6.5.17+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.17+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_port_config_t(
    bcm_compat6517_port_config_t *from,
    bcm_port_config_t *to)
{
    int i = 0;
    bcm_port_config_t_init(to);
    BCM_PBMP_ASSIGN(to->fe, from->fe);
    BCM_PBMP_ASSIGN(to->ge, from->ge);
    BCM_PBMP_ASSIGN(to->xe, from->xe);
    BCM_PBMP_ASSIGN(to->ce, from->ce);
    BCM_PBMP_ASSIGN(to->le, from->le);
    BCM_PBMP_ASSIGN(to->cc, from->cc);
    BCM_PBMP_ASSIGN(to->cd, from->cd);
    BCM_PBMP_ASSIGN(to->e, from->e);
    BCM_PBMP_ASSIGN(to->hg, from->hg);
    BCM_PBMP_ASSIGN(to->sci, from->sci);
    BCM_PBMP_ASSIGN(to->sfi, from->sfi);
    BCM_PBMP_ASSIGN(to->spi, from->spi);
    BCM_PBMP_ASSIGN(to->spi_subport, from->spi_subport);
    BCM_PBMP_ASSIGN(to->port, from->port);
    BCM_PBMP_ASSIGN(to->cpu, from->cpu);
    BCM_PBMP_ASSIGN(to->all, from->all);
    BCM_PBMP_ASSIGN(to->stack_int, from->stack_int);
    BCM_PBMP_ASSIGN(to->stack_ext, from->stack_ext);
    BCM_PBMP_ASSIGN(to->tdm, from->tdm);
    BCM_PBMP_ASSIGN(to->pon, from->pon);
    BCM_PBMP_ASSIGN(to->llid, from->llid);
    BCM_PBMP_ASSIGN(to->il, from->il);
    BCM_PBMP_ASSIGN(to->xl, from->xl);
    BCM_PBMP_ASSIGN(to->rcy, from->rcy);
    BCM_PBMP_ASSIGN(to->sat, from->sat);
    BCM_PBMP_ASSIGN(to->ipsec, from->ipsec);
    for (i = 0; i < BCM_PIPES_MAX; i ++) {
        BCM_PBMP_ASSIGN(to->per_pipe[i], from->per_pipe[i]);
    }
    BCM_PBMP_ASSIGN(to->nif, from->nif);
    BCM_PBMP_ASSIGN(to->control, from->control);
    BCM_PBMP_ASSIGN(to->eventor, from->eventor);
    BCM_PBMP_ASSIGN(to->olp, from->olp);
    BCM_PBMP_ASSIGN(to->oamp, from->oamp);
    BCM_PBMP_ASSIGN(to->erp, from->erp);
    BCM_PBMP_ASSIGN(to->roe, from->roe);
    BCM_PBMP_ASSIGN(to->rcy_mirror, from->rcy_mirror);
    return;
}

/*
 * Function:
 *      _bcm_compat6517out_port_config_t
 * Purpose:
 *      Convert the bcm_port_config_t datatype from 6.5.17+ to
 *      SDK <=6.5.17
 * Parameters:
 *      from        - (IN) The 6.5.17+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_port_config_t(
    bcm_port_config_t *from,
    bcm_compat6517_port_config_t *to)
{
    int i = 0;
    BCM_PBMP_ASSIGN(to->fe, from->fe);
    BCM_PBMP_ASSIGN(to->ge, from->ge);
    BCM_PBMP_ASSIGN(to->xe, from->xe);
    BCM_PBMP_ASSIGN(to->ce, from->ce);
    BCM_PBMP_ASSIGN(to->le, from->le);
    BCM_PBMP_ASSIGN(to->cc, from->cc);
    BCM_PBMP_ASSIGN(to->cd, from->cd);
    BCM_PBMP_ASSIGN(to->e, from->e);
    BCM_PBMP_ASSIGN(to->hg, from->hg);
    BCM_PBMP_ASSIGN(to->sci, from->sci);
    BCM_PBMP_ASSIGN(to->sfi, from->sfi);
    BCM_PBMP_ASSIGN(to->spi, from->spi);
    BCM_PBMP_ASSIGN(to->spi_subport, from->spi_subport);
    BCM_PBMP_ASSIGN(to->port, from->port);
    BCM_PBMP_ASSIGN(to->cpu, from->cpu);
    BCM_PBMP_ASSIGN(to->all, from->all);
    BCM_PBMP_ASSIGN(to->stack_int, from->stack_int);
    BCM_PBMP_ASSIGN(to->stack_ext, from->stack_ext);
    BCM_PBMP_ASSIGN(to->tdm, from->tdm);
    BCM_PBMP_ASSIGN(to->pon, from->pon);
    BCM_PBMP_ASSIGN(to->llid, from->llid);
    BCM_PBMP_ASSIGN(to->il, from->il);
    BCM_PBMP_ASSIGN(to->xl, from->xl);
    BCM_PBMP_ASSIGN(to->rcy, from->rcy);
    BCM_PBMP_ASSIGN(to->sat, from->sat);
    BCM_PBMP_ASSIGN(to->ipsec, from->ipsec);
    for (i = 0; i < BCM_PIPES_MAX; i ++) {
        BCM_PBMP_ASSIGN(to->per_pipe[i], from->per_pipe[i]);
    }
    BCM_PBMP_ASSIGN(to->nif, from->nif);
    BCM_PBMP_ASSIGN(to->control, from->control);
    BCM_PBMP_ASSIGN(to->eventor, from->eventor);
    BCM_PBMP_ASSIGN(to->olp, from->olp);
    BCM_PBMP_ASSIGN(to->oamp, from->oamp);
    BCM_PBMP_ASSIGN(to->erp, from->erp);
    BCM_PBMP_ASSIGN(to->roe, from->roe);
    BCM_PBMP_ASSIGN(to->rcy_mirror, from->rcy_mirror);
    return;
}

int
bcm_compat6517_port_config_get(
    int unit,
    bcm_compat6517_port_config_t *config
    )
{
    int rv;
    bcm_port_config_t *new_config = NULL;

    if (NULL != config) {
        /* Create from heap to avoid the stack to bloat */
        new_config = sal_alloc(sizeof(bcm_port_config_t), "New config");
        if (NULL == new_config) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_port_config_t(config, new_config);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_config_get(unit, new_config);
    if (NULL != new_config) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_port_config_t(new_config, config);
        /* Deallocate memory*/
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6517in_mirror_source_t
 * Purpose:
 *   Convert the bcm_mirror_source_t datatype from <=6.5.17 to
 *   SDK 6.5.17+.
 * Parameters:
 *   from        - (IN) The <=6.5.17 version of the datatype.
 *   to          - (OUT) The SDK 6.5.17+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6517in_mirror_source_t(
    bcm_compat6517_mirror_source_t *from,
    bcm_mirror_source_t *to)
{
    bcm_mirror_source_t_init(to);
    to->type = from->type;
    to->flags = from->flags;
    to->port = from->port;
    to->trace_event = from->trace_event;
    to->drop_event = from->drop_event;
}

/*
 * Function:
 *   _bcm_compat6517out_mirror_source_t
 * Purpose:
 *   Convert the bcm_mirror_source_t datatype from 6.5.17+ to
 *   <=6.5.17.
 * Parameters:
 *   from     - (IN) The 6.5.17+ version of the datatype.
 *   to       - (OUT) The <=6.5.17 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6517out_mirror_source_t(
    bcm_mirror_source_t *from,
    bcm_compat6517_mirror_source_t *to)
{
    to->type = from->type;
    to->flags = from->flags;
    to->port = from->port;
    to->trace_event = from->trace_event;
    to->drop_event = from->drop_event;
}

/*
 * Function:
 *   bcm_compat6517_mirror_source_dest_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_source_dest_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   source         - (IN) Mirror source.
 *   mirror_dest_id - (IN) Mirror destination id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_mirror_source_dest_add(
    int unit,
    bcm_compat6517_mirror_source_t *source,
    bcm_gport_t mirror_dest_id)
{
    int rv = 0;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t*)
            sal_alloc(sizeof(bcm_mirror_source_t), "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_source_dest_add(unit, new_source, mirror_dest_id);

    if (rv >= 0) {
        _bcm_compat6517out_mirror_source_t(new_source, source);
    }

    sal_free(new_source);
    return rv;
}

/*
 * Function:
 *   bcm_compat6517_mirror_source_dest_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_source_dest_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   source         - (IN) Mirror source.
 *   mirror_dest_id - (IN) Mirror destination id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_mirror_source_dest_delete(
    int unit,
    bcm_compat6517_mirror_source_t *source,
    bcm_gport_t mirror_dest_id)
{
    int rv = 0;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t*)
            sal_alloc(sizeof(bcm_mirror_source_t), "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_source_dest_delete(unit, new_source, mirror_dest_id);

    if (rv >= 0) {
        _bcm_compat6517out_mirror_source_t(new_source, source);
    }

    sal_free(new_source);
    return rv;
}

/*
 * Function:
 *   bcm_compat6517_mirror_source_dest_delete_all
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_source_dest_delete_all.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   source         - (IN) Mirror source.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6517_mirror_source_t *source)
{
    int rv = 0;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t*)
            sal_alloc(sizeof(bcm_mirror_source_t), "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_source_dest_delete_all(unit, new_source);

    if (rv >= 0) {
        _bcm_compat6517out_mirror_source_t(new_source, source);
    }

    sal_free(new_source);
    return rv;
}

/*
 * Function:
 *   bcm_compat6517_mirror_source_dest_get_all
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_source_dest_get_all.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   source         - (IN) Mirror source.
 *   array_size     - (IN) Pre-allocated array size for mirror_dest.
 *   mirror_dest    - (OUT) The returned mirror destination id array.
 *   count          - (OUT) The actual mirror destination count.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_mirror_source_dest_get_all(
    int unit,
    bcm_compat6517_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count)
{
    int rv = 0;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t*)
            sal_alloc(sizeof(bcm_mirror_source_t), "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_source_dest_get_all(unit, new_source, array_size,
                                        mirror_dest, count);

    if (rv >= 0) {
        _bcm_compat6517out_mirror_source_t(new_source, source);
    }

    sal_free(new_source);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6517in_field_presel_config_t
 * Purpose:
 *   Convert the bcm_field_presel_config_t datatype from <=6.5.17 to
 *   SDK 6.5.17+.
 * Parameters:
 *   from        - (IN) The <=6.5.17 version of the datatype.
 *   to          - (OUT) The SDK 6.5.17+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6517in_field_presel_config_t(
    bcm_compat6517_field_presel_config_t *from,
    bcm_field_presel_config_t *to)
{
    int i = 0;
    bcm_field_presel_config_t_init(to);
	for (i = 0; i < BCM_FIELD_MAX_NAME_LEN; i++) {
        to->name[i] = from->name[i];
    }
}

/*
 * Function:
 *   _bcm_compat6517out_field_presel_config_t
 * Purpose:
 *   Convert the bcm_field_presel_config_t datatype from 6.5.17+ to
 *   <=6.5.17.
 * Parameters:
 *   from     - (IN) The 6.5.17+ version of the datatype.
 *   to       - (OUT) The <=6.5.17 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6517out_field_presel_config_t(
    bcm_field_presel_config_t *from,
    bcm_compat6517_field_presel_config_t *to)
{
    int i = 0;
    for (i = 0; i < BCM_FIELD_MAX_NAME_LEN; i++) {
        to->name[i] = from->name[i];
    }
}

/*
 * Function:
 *   bcm_compat6517_field_presel_config_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_fied_presel_config_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   presel_id      - (IN) Presel Id.
 *   presel_config  - (IN) presel config
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_field_presel_config_get(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_compat6517_field_presel_config_t *presel_config)
{
    int rv;
    bcm_field_presel_config_t *new_config = NULL;

    if (presel_config != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_config = sal_alloc(sizeof(bcm_field_presel_config_t),
                               "New presel cfg");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_field_presel_config_t(presel_config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_presel_config_get(unit, presel_id, new_config);

    if (new_config != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_field_presel_config_t(new_config, presel_config);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6517_mirror_source_dest_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_presel_config_set.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   presel_id      - (IN) Presel Id.
 *   preseel_cfg    - (IN) presel config.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_field_presel_config_set(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_compat6517_field_presel_config_t *presel_cfg)
{
    int rv = 0;
    bcm_field_presel_config_t *new_config = NULL;

    if (presel_cfg != NULL) {
        new_config = (bcm_field_presel_config_t*)
            sal_alloc(sizeof(bcm_field_presel_config_t), "New presel config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_field_presel_config_t(presel_cfg, new_config);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_field_presel_config_set(unit, presel_id, new_config);

    if (new_config != NULL) {
        _bcm_compat6517out_field_presel_config_t(new_config, presel_cfg);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_vlan_block_t
 * Purpose:
 *      Convert the _bcm_compat6517in_vlan_block_t datatype from <=6.5.17
 *      to SDK 6.5.17+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.17+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_vlan_block_t(
    bcm_compat6517_vlan_block_t *from,
    bcm_vlan_block_t *to)
{
    bcm_vlan_block_t_init(to);

    BCM_PBMP_ASSIGN(to->known_multicast, from->known_multicast);
    BCM_PBMP_ASSIGN(to->unknown_multicast, from->unknown_multicast);
    BCM_PBMP_ASSIGN(to->unknown_unicast, from->unknown_unicast);
    BCM_PBMP_ASSIGN(to->broadcast, from->broadcast);
}

/*
 * Function:
 *      _bcm_compat6517out_vlan_block_t
 * Purpose:
 *      Convert the bcm_vlan_block_t datatype from 6.5.17+ to
 *      <=SDK 6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.17+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_vlan_block_t(
    bcm_vlan_block_t *from,
    bcm_compat6517_vlan_block_t *to)
{
    BCM_PBMP_ASSIGN(to->known_multicast, from->known_multicast);
    BCM_PBMP_ASSIGN(to->unknown_multicast, from->unknown_multicast);
    BCM_PBMP_ASSIGN(to->unknown_unicast, from->unknown_unicast);
    BCM_PBMP_ASSIGN(to->broadcast, from->broadcast);
}

/*
 * Function:
 *   bcm_compat6517_vlan_block_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_vlan_block_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   presel_id      - (IN) Presel Id.
 *   presel_config  - (IN) presel config
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_vlan_block_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6517_vlan_block_t *vlan_block)

{
    int rv;
    bcm_vlan_block_t *new_config = NULL;

    if (vlan_block != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_config = (bcm_vlan_block_t*)
            sal_alloc(sizeof(bcm_vlan_block_t), "New vlan block structure");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_vlan_block_t(vlan_block, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_block_get(unit, vlan, new_config);

    if (new_config != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6517out_vlan_block_t(new_config, vlan_block);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6517_vlan_block_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_vlan_block_set.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   presel_id      - (IN) Presel Id.
 *   preseel_cfg    - (IN) presel config.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_vlan_block_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6517_vlan_block_t *vlan_block)

{
    int rv = 0;
    bcm_vlan_block_t *new_config = NULL;

    if (vlan_block != NULL) {
        new_config = (bcm_vlan_block_t*)
            sal_alloc(sizeof(bcm_vlan_block_t), "New vlan block structure");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_vlan_block_t(vlan_block, new_config);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_vlan_block_set(unit, vlan, new_config);

    if (new_config != NULL) {
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6517in_switch_pkt_protocol_match_t
 * Purpose:
 *   Convert the bcm_compat6517_switch_pkt_protocol_match_t datatype from
 *   <=6.5.17 to SDK 6.5.17+.
 * Parameters:
 *   from        - (IN) The <=6.5.17 version of the datatype.
 *   to          - (OUT) The SDK 6.5.17+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6517in_switch_pkt_protocol_match_t(
    bcm_compat6517_switch_pkt_protocol_match_t *from,
    bcm_switch_pkt_protocol_match_t *to)
{
    bcm_switch_pkt_protocol_match_t_init(to);
    to->l2_iif_opaque_ctrl_id            = from->l2_iif_opaque_ctrl_id;
    to->l2_iif_opaque_ctrl_id_mask       = from->l2_iif_opaque_ctrl_id_mask;
    to->vfi                              = from->vfi;
    to->vfi_mask                         = from->vfi_mask;
    to->ethertype                        = from->ethertype;
    to->ethertype_mask                   = from->ethertype_mask;
    to->arp_rarp_opcode                  = from->arp_rarp_opcode;
    to->arp_rarp_opcode_mask             = from->arp_rarp_opcode_mask;
    to->icmp_type                        = from->icmp_type;
    to->icmp_type_mask                   = from->icmp_type_mask;
    to->igmp_type                        = from->igmp_type;
    to->igmp_type_mask                   = from->igmp_type_mask;
    to->l4_valid                         = from->l4_valid;
    to->l4_valid_mask                    = from->l4_valid_mask;
    to->l4_src_port                      = from->l4_src_port;
    to->l4_src_port_mask                 = from->l4_src_port_mask;
    to->l4_dst_port                      = from->l4_dst_port;
    to->l4_dst_port_mask                 = from->l4_dst_port_mask;
    to->l5_bytes_0_1                     = from->l5_bytes_0_1;
    to->l5_bytes_0_1_mask                = from->l5_bytes_0_1_mask;
    to->outer_l5_bytes_0_1               = from->outer_l5_bytes_0_1;
    to->outer_l5_bytes_0_1_mask          = from->outer_l5_bytes_0_1_mask;
    to->ip_last_protocol                 = from->ip_last_protocol;
    to->ip_last_protocol_mask            = from->ip_last_protocol_mask;
    to->fixed_hve_result_set_1           = from->fixed_hve_result_set_1;
    to->fixed_hve_result_set_1_mask      = from->fixed_hve_result_set_1_mask;
    to->fixed_hve_result_set_5           = from->fixed_hve_result_set_5;
    to->fixed_hve_result_set_5_mask      = from->fixed_hve_result_set_5_mask;
    to->flex_hve_result_set_1            = from->flex_hve_result_set_1;
    to->flex_hve_result_set_1_mask       = from->flex_hve_result_set_1_mask;
    to->tunnel_processing_results_1      = from->tunnel_processing_results_1;
    to->tunnel_processing_results_1_mask = from->tunnel_processing_results_1_mask;
    sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
    sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
}

/*
 * Function:
 *   _bcm_compat6517out_switch_pkt_protocol_match_t
 * Purpose:
 *   Convert the bcm_switch_pkt_protocol_match_t datatype from 6.5.17+ to
 *   <=6.5.17.
 * Parameters:
 *   from     - (IN) The 6.5.17+ version of the datatype.
 *   to       - (OUT) The <=6.5.17 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6517out_switch_pkt_protocol_match_t(
    bcm_switch_pkt_protocol_match_t *from,
    bcm_compat6517_switch_pkt_protocol_match_t *to)
{
    to->l2_iif_opaque_ctrl_id            = from->l2_iif_opaque_ctrl_id;
    to->l2_iif_opaque_ctrl_id_mask       = from->l2_iif_opaque_ctrl_id_mask;
    to->vfi                              = from->vfi;
    to->vfi_mask                         = from->vfi_mask;
    to->ethertype                        = from->ethertype;
    to->ethertype_mask                   = from->ethertype_mask;
    to->arp_rarp_opcode                  = from->arp_rarp_opcode;
    to->arp_rarp_opcode_mask             = from->arp_rarp_opcode_mask;
    to->icmp_type                        = from->icmp_type;
    to->icmp_type_mask                   = from->icmp_type_mask;
    to->igmp_type                        = from->igmp_type;
    to->igmp_type_mask                   = from->igmp_type_mask;
    to->l4_valid                         = from->l4_valid;
    to->l4_valid_mask                    = from->l4_valid_mask;
    to->l4_src_port                      = from->l4_src_port;
    to->l4_src_port_mask                 = from->l4_src_port_mask;
    to->l4_dst_port                      = from->l4_dst_port;
    to->l4_dst_port_mask                 = from->l4_dst_port_mask;
    to->l5_bytes_0_1                     = from->l5_bytes_0_1;
    to->l5_bytes_0_1_mask                = from->l5_bytes_0_1_mask;
    to->outer_l5_bytes_0_1               = from->outer_l5_bytes_0_1;
    to->outer_l5_bytes_0_1_mask          = from->outer_l5_bytes_0_1_mask;
    to->ip_last_protocol                 = from->ip_last_protocol;
    to->ip_last_protocol_mask            = from->ip_last_protocol_mask;
    to->fixed_hve_result_set_1           = from->fixed_hve_result_set_1;
    to->fixed_hve_result_set_1_mask      = from->fixed_hve_result_set_1_mask;
    to->fixed_hve_result_set_5           = from->fixed_hve_result_set_5;
    to->fixed_hve_result_set_5_mask      = from->fixed_hve_result_set_5_mask;
    to->flex_hve_result_set_1            = from->flex_hve_result_set_1;
    to->flex_hve_result_set_1_mask       = from->flex_hve_result_set_1_mask;
    to->tunnel_processing_results_1      = from->tunnel_processing_results_1;
    to->tunnel_processing_results_1_mask = from->tunnel_processing_results_1_mask;
    sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
    sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
}

/*
 * Function:
 *   bcm_compat6517_switch_pkt_protocol_control_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Options flags.
 *   match          - (IN) Protocol match structure.
 *   action         - (IN) Packet control action.
 *   priority       - (IN) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6517_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int priority)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_add(unit, options, new_match,
                                             action, priority);

    if (rv >= 0) {
        _bcm_compat6517out_switch_pkt_protocol_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6517_switch_pkt_protocol_control_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Protocol match structure.
 *   action         - (OUT) Packet control action.
 *   priority       - (OUT) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6517_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int *priority)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_get(unit, new_match, action, priority);

    if (rv >= 0) {
        _bcm_compat6517out_switch_pkt_protocol_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6517_switch_pkt_protocol_control_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Protocol match structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6517_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6517_switch_pkt_protocol_match_t *match)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6517in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_delete(unit, new_match);

    if (rv >= 0) {
        _bcm_compat6517out_switch_pkt_protocol_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6517in_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_mirror_pkt_dnx_pp_header_t(
    bcm_compat6517_mirror_pkt_dnx_pp_header_t *from,
    bcm_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
}

/*
 * Function:
 *      _bcm_compat6517out_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_mirror_pkt_dnx_pp_header_t(
    bcm_mirror_pkt_dnx_pp_header_t *from,
    bcm_compat6517_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
}

/*
 * Function:
 *      _bcm_compat6517in_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from <=6.5.17 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.17 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517in_mirror_header_info_t(
    bcm_compat6517_mirror_header_info_t *from,
    bcm_mirror_header_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6517in_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      _bcm_compat6517out_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from SDK 6.5.18+ to
 *      <=6.5.17
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.17 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6517out_mirror_header_info_t(
    bcm_mirror_header_info_t *from,
    bcm_compat6517_mirror_header_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6517out_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      bcm_compat6517_mirror_header_info_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_header_info_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_MIRROR_DEST_* flags
 *      mirror_dest_id - (IN) (IN/OUT) Mirrored destination ID
 *      mirror_header_info - (IN) system header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6517_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6517in_mirror_header_info_t(mirror_header_info, new_mirror_header_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_header_info_set(unit, flags, mirror_dest_id, new_mirror_header_info);


    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6517_mirror_header_info_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_header_info_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mirror_dest_id - (IN) Mirrored destination ID
 *      flags - (INOUT) (IN/OUT) BCM_MIRROR_DEST_* flags
 *      mirror_header_info - (OUT) system header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6517_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6517_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_header_info_get(unit, mirror_dest_id, flags, new_mirror_header_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6517out_mirror_header_info_t(new_mirror_header_info, mirror_header_info);

    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}


#endif /* BCM_RPC_SUPPORT*/
