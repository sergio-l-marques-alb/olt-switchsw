/*
 * $Id: compat_6515.h,v 2.0 2018/09/20
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * RPC Compatibility with sdk-6.5.15 routines
*/

#ifndef _COMPAT_6515_H_
#define _COMPAT_6515_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/mpls.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>

#ifdef INCLUDE_L3
typedef struct bcm_compat6515_mpls_tunnel_switch_s {
    uint32 flags;                       /* BCM_MPLS_SWITCH_xxx. */
    uint32 flags2;                      /* BCM_MPLS_SWITCH2_xxx. */
    bcm_mpls_label_t label;             /* Incoming label value. */
    bcm_mpls_label_t second_label;      /* Incoming second label. */
    bcm_gport_t port;                   /* Incoming port. */
    bcm_mpls_switch_action_t action;    /* MPLS label action. */
    bcm_mpls_switch_action_t action_if_bos; /* MPLS label action if BOS. */
    bcm_mpls_switch_action_t action_if_not_bos; /* MPLS label action if not BOS. */
    bcm_multicast_t mc_group;           /* P2MP Multicast group. */
    int exp_map;                        /* EXP-map ID. */
    int int_pri;                        /* Internal priority. */
    bcm_policer_t policer_id;           /* Policer ID to be associated with the
                                           incoming label. */
    bcm_vpn_t vpn;                      /* L3 VPN used if action is POP. */
    bcm_mpls_egress_label_t egress_label; /* Outgoing label information. */
    bcm_if_t egress_if;                 /* Outgoing egress object. */
    bcm_if_t ingress_if;                /* Ingress Interface object. */
    int mtu;                            /* MTU. */
    int qos_map_id;                     /* QOS map identifier. */
    bcm_failover_t failover_id;         /* Failover Object Identifier for
                                           protected tunnel. Used for 1+1
                                           protection also */
    bcm_gport_t tunnel_id;              /* Tunnel ID. */
    bcm_gport_t failover_tunnel_id;     /* Failover Tunnel ID. */
    bcm_if_t tunnel_if;                 /* hierarchical interface, relevant for
                                           when action is
                                           BCM_MPLS_SWITCH_ACTION_POP. */
    bcm_gport_t egress_port;            /* Outgoing egress port. */
    uint16 oam_global_context_id;       /* OAM global context id passed from
                                           ingress to egress XGS chip. */
    uint32 class_id;                    /* Class ID */
    int inlif_counting_profile;         /* In LIF counting profile */
    int ecn_map_id;                     /* ECN map identifier */
    int tunnel_term_ecn_map_id;         /* Tunnel termination ecn map identifier */
} bcm_compat6515_mpls_tunnel_switch_t;

extern int bcm_compat6515_mpls_tunnel_switch_add(
    int unit, bcm_compat6515_mpls_tunnel_switch_t *info);

extern int bcm_compat6515_mpls_tunnel_switch_delete(
    int unit, bcm_compat6515_mpls_tunnel_switch_t *info);

extern int bcm_compat6515_mpls_tunnel_switch_get(
    int unit, bcm_compat6515_mpls_tunnel_switch_t *info);

/* IPMC address type. */
typedef struct bcm_compat6515_ipmc_addr_s {
    bcm_ip_t s_ip_addr;                 /* IPv4 Source address. */
    bcm_ip_t mc_ip_addr;                /* IPv4 Destination address. */
    bcm_ip6_t s_ip6_addr;               /* IPv6 Source address. */
    bcm_ip6_t mc_ip6_addr;              /* IPv6 Destination address. */
    bcm_vlan_t vid;                     /* VLAN identifier. */
    bcm_vrf_t vrf;                      /* Virtual Router Instance. */
    bcm_cos_t cos;                      /* COS based on dst IP multicast addr. */
    int ts;                             /* Source port or TGID bit. */
    int port_tgid;                      /* Source port or TGID. */
    int v;                              /* Valid bit. */
    int mod_id;                         /* Module ID. */
    bcm_multicast_t group;              /* Use this index to program IPMC table
                                           for XGS chips based on flags value.
                                           For SBX chips it is the Multicast
                                           Group index */
    uint32 flags;                       /* See BCM_IPMC_XXX flag definitions. */
    int lookup_class;                   /* Classification lookup class ID. */
    bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
    bcm_if_t l3a_intf;                  /* L3 interface associated with route. */
    int rp_id;                          /* Rendezvous point ID. */
    bcm_ip_t s_ip_mask;                 /* IPv4 Source subnet mask. */
    bcm_if_t ing_intf;                  /* L3 interface associated with this
                                           Entry */
    bcm_ip_t mc_ip_mask;                /* IPv4 Destination subnet mask. */
    bcm_ip6_t mc_ip6_mask;              /* IPv6 Destination subnet mask. */
    bcm_multicast_t group_l2;           /* Use this index to program IPMC table
                                           for l2 recipients if TTL/RPF check
                                           fails. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_ip6_t s_ip6_mask;               /* IPv6 Source subnet mask. */
    int priority;                       /* Entry priority. */
} bcm_compat6515_ipmc_addr_t;

/* Add new IPMC group. */
extern int bcm_compat6515_ipmc_add(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

/* Find info of an IPMC group. */
extern int bcm_compat6515_ipmc_find(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

/* Remove IPMC group. */
extern int bcm_compat6515_ipmc_remove(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

#endif

/* Generic port match attribute structure */
typedef struct bcm_compat6515_port_match_info_s {
    bcm_port_match_t match;             /* Match criteria */
    bcm_gport_t port;                   /* Match port */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match */
    bcm_vlan_t match_vlan_max;          /* Maximum VLAN ID in range to match */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match */
    bcm_vlan_t match_inner_vlan_max;    /* Maximum Inner VLAN ID in range to
                                           match */
    bcm_vlan_t match_tunnel_vlan;       /* B-domain VID */
    bcm_mac_t match_tunnel_srcmac;      /* B-domain source MAC address */
    bcm_mpls_label_t match_label;       /* MPLS label */
    uint32 flags;                       /* BCM_PORT_MATCH_XXX flags */
    bcm_tunnel_id_t match_pon_tunnel;   /* PON Tunnel value to match. */
    bcm_port_ethertype_t match_ethertype; /* Ethernet type value to match */
    int match_pcp;                      /* Outer PCP ID to match */
    bcm_vlan_action_set_t *action;      /* Match action */
    uint16 extended_port_vid;           /* Extender port VID */
    bcm_vpn_t vpn;                      /* VPN ID */
    uint16 niv_port_vif;                /* NIV port VIF */
    uint32 isid;
} bcm_compat6515_port_match_info_t;

/* Add a match to an existing port */
extern int bcm_compat6515_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *match);

/* Remove a match from an existing port */
extern int bcm_compat6515_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *match);

/* Replace an old match with a new one for an existing port */
extern int bcm_compat6515_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_match_info_t *old_match,
    bcm_compat6515_port_match_info_t *new_match);

/* Get all the matches for an existing port */
extern int bcm_compat6515_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6515_port_match_info_t *match_array,
    int *count);

/* Assign a set of matches to an existing port */
extern int bcm_compat6515_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6515_port_match_info_t *match_array);

#endif
#endif /* !_COMPAT_6515_H */
