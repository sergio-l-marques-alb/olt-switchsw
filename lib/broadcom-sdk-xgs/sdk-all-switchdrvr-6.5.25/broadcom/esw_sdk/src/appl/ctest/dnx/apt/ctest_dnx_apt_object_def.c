/*
 * ! \file ctest_dnx_apt_object_def.c
 * Contains all of the API performance test objects. New test objects should be added here.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
#include <shared/bsl.h>

/*
 * Include files.
 * {
 */

#include "ctest_dnx_apt_object_def.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * GLOBALs
 * {
 */

/* *INDENT-OFF* */
/***********************************************************************************************************************
 * Declaration of the API performance test objects define in "ctest_dnx_apt_object_xxx_def.c".
 **********************************************************************************************************************/
extern const dnx_apt_object_def_t
    /** L3 Route tests */
    dnx_apt_l3_route_ipv4_insert_object,
    dnx_apt_l3_route_ipv4_insert_perf_object,
    dnx_apt_l3_route_ipv4_insert_dbal_object,
    dnx_apt_l3_route_ipv4_update_object,
    dnx_apt_l3_route_ipv4_update_dbal_object,
    dnx_apt_l3_route_ipv4_delete_object,
    dnx_apt_l3_route_ipv6_insert_object,
    dnx_apt_l3_route_ipv6_insert_perf_object,
    dnx_apt_l3_route_ipv6_insert_dbal_object,
    dnx_apt_l3_route_ipv6_update_object,
    dnx_apt_l3_route_ipv6_update_dbal_object,
    dnx_apt_l3_route_ipv6_delete_object,

    /** L3 Host tests */
    dnx_apt_l3_host_ipv4_insert_object,
    dnx_apt_l3_host_ipv4_insert_dbal_object,
    dnx_apt_l3_host_ipv4_update_object,
    dnx_apt_l3_host_ipv6_insert_object,
    dnx_apt_l3_host_ipv6_insert_dbal_object,
    dnx_apt_l3_host_ipv6_update_object,

    /** IPMC tests */
    dnx_apt_ipmc_ipv4_lpm_insert_object,
    dnx_apt_ipmc_ipv4_lpm_update_object,
    dnx_apt_ipmc_ipv6_lpm_insert_object,
    dnx_apt_ipmc_ipv6_lpm_update_object,

    dnx_apt_ipmc_ipv4_em_insert_object,
    dnx_apt_ipmc_ipv4_em_update_object,
    dnx_apt_ipmc_ipv6_em_insert_object,
    dnx_apt_ipmc_ipv6_em_update_object,

    dnx_apt_ipmc_ipv4_tcam_insert_object,
    dnx_apt_ipmc_ipv4_tcam_update_object,
    dnx_apt_ipmc_ipv6_tcam_insert_object,
    dnx_apt_ipmc_ipv6_tcam_update_object,

    dnx_apt_mpls_ingress_insert_object,
    dnx_apt_mpls_egress_insert_object,
    dnx_apt_mpls_egress_update_object,
    dnx_apt_mpls_egress_update_dbal_object,
    dnx_apt_mpls_ingress_port_insert_object,
    dnx_apt_mpls_egress_port_insert_object,
    dnx_apt_l3_arp_insert_object,
    dnx_apt_l3_arp_update_object,

    dnx_apt_l3_fec_insert_object,
    dnx_apt_l3_fec_update_object,
    dnx_apt_l3_fec_get_object,
    dnx_apt_l3_fec_destroy_object,

    dnx_apt_l3_ecmp_multiply_and_divide_insert_object,
    dnx_apt_l3_ecmp_multiply_and_divide_update_object,
    dnx_apt_l3_ecmp_multiply_and_divide_destroy_object,

    dnx_apt_l3_ecmp_consistent_hashing_small_table_insert_object,
    dnx_apt_l3_ecmp_consistent_hashing_medium_table_insert_object,
    dnx_apt_l3_ecmp_consistent_hashing_large_table_insert_object,

    dnx_apt_l3_ecmp_consistent_hashing_small_table_update_object,
    dnx_apt_l3_ecmp_consistent_hashing_medium_table_update_object,
    dnx_apt_l3_ecmp_multiply_and_divide_get_object,
    dnx_apt_l3_ecmp_consistent_hashing_large_table_update_object,

    dnx_apt_l3_ecmp_consistent_hashing_destroy_object,

    dnx_apt_tunnel_initiator_create_ipv4_object,
    dnx_apt_tunnel_initiator_create_ipv6_object,
    dnx_apt_tunnel_term_ipv4_p2p_insert_object,
    dnx_apt_tunnel_term_ipv4_mp_insert_object,
    dnx_apt_tunnel_term_ipv4_tcam_insert_object,
    dnx_apt_tunnel_term_ipv6_p2p_insert_object,
    dnx_apt_tunnel_term_ipv6_mp_insert_object,
    dnx_apt_tunnel_term_txsci_insert_object,
    dnx_apt_tunnel_vxlan_port_add_object,

    dnx_apt_vlan_port_create_egress_object,
    dnx_apt_vlan_port_create_ingress_object,
    dnx_apt_vlan_port_create_symmetric_object,
    dnx_apt_vlan_port_create_esem_object,
    dnx_apt_vlan_port_create_replace_object,
    dnx_apt_vlan_port_create_native_object,

    dnx_apt_oam_mep_eth_acc_dnmep_insert_object,
    dnx_apt_oam_mep_eth_acc_upmep_insert_object,
    dnx_apt_oam_mep_eth_non_acc_dnmep_insert_object,
    dnx_apt_oam_mep_eth_non_acc_upmep_insert_object,

    /** SWSATATE tests*/
    dnx_apt_swstate_htb_small_insert_object,
    dnx_apt_swstate_htb_small_delete_object,
    dnx_apt_swstate_htb_small_find_object,
    dnx_apt_swstate_htb_large_insert_object,
    dnx_apt_swstate_htb_large_delete_object,
    dnx_apt_swstate_htb_large_find_object,


    /** MACT test */
    dnx_apt_l2_mact_insert_object,

    dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_insert_object,
    dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_insert_object,
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_insert_object,
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_insert_object,
    dnx_apt_bfd_endpoint_pwe_control_word_acc_insert_object,
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_insert_object,
    dnx_apt_bfd_endpoint_pwe_ttl_acc_insert_object,
    dnx_apt_bfd_endpoint_pwe_gal_acc_insert_object,
    
    dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_update_object,
    dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_update_object,
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_object,
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_object,
    dnx_apt_bfd_endpoint_pwe_control_word_acc_update_object,
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_object,
    dnx_apt_bfd_endpoint_pwe_ttl_acc_update_object,
    dnx_apt_bfd_endpoint_pwe_gal_acc_update_object,

    dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_destroy_object,
    dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_destroy_object,
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_destroy_object,
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_destroy_object,
    dnx_apt_bfd_endpoint_pwe_control_word_acc_destroy_object,
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_destroy_object,
    dnx_apt_bfd_endpoint_pwe_ttl_acc_destroy_object,
    dnx_apt_bfd_endpoint_pwe_gal_acc_destroy_object
    ;


/***********************************************************************************************************************
 * Definition of the list of API performance test objects.
 **********************************************************************************************************************/
const dnx_apt_object_def_t *dnx_apt_objects[] = {
    /** L3 Route tests */
    &dnx_apt_l3_route_ipv4_insert_object,
    &dnx_apt_l3_route_ipv4_insert_perf_object,
    &dnx_apt_l3_route_ipv4_insert_dbal_object,
    &dnx_apt_l3_route_ipv4_update_object,
    &dnx_apt_l3_route_ipv4_update_dbal_object,
    &dnx_apt_l3_route_ipv4_delete_object,
    &dnx_apt_l3_route_ipv6_insert_object,
    &dnx_apt_l3_route_ipv6_insert_perf_object,
    &dnx_apt_l3_route_ipv6_insert_dbal_object,
    &dnx_apt_l3_route_ipv6_update_object,
    &dnx_apt_l3_route_ipv6_update_dbal_object,
    &dnx_apt_l3_route_ipv6_delete_object,

    /** L3 Host tests */
    &dnx_apt_l3_host_ipv4_insert_object,
    &dnx_apt_l3_host_ipv4_insert_dbal_object,
    &dnx_apt_l3_host_ipv4_update_object,
    &dnx_apt_l3_host_ipv6_insert_object,
    &dnx_apt_l3_host_ipv6_insert_dbal_object,
    &dnx_apt_l3_host_ipv6_update_object,

    /** IPMC tests */
    &dnx_apt_ipmc_ipv4_lpm_insert_object,
    &dnx_apt_ipmc_ipv4_lpm_update_object,
    &dnx_apt_ipmc_ipv6_lpm_insert_object,
    &dnx_apt_ipmc_ipv6_lpm_update_object,

    &dnx_apt_ipmc_ipv4_em_insert_object,
    &dnx_apt_ipmc_ipv4_em_update_object,
    &dnx_apt_ipmc_ipv6_em_insert_object,
    &dnx_apt_ipmc_ipv6_em_update_object,

    &dnx_apt_ipmc_ipv4_tcam_insert_object,
    &dnx_apt_ipmc_ipv4_tcam_update_object,
    &dnx_apt_ipmc_ipv6_tcam_insert_object,
    &dnx_apt_ipmc_ipv6_tcam_update_object,

    &dnx_apt_mpls_ingress_insert_object,
    &dnx_apt_mpls_egress_insert_object,
    &dnx_apt_mpls_egress_update_object,
    &dnx_apt_mpls_egress_update_dbal_object,
    &dnx_apt_mpls_ingress_port_insert_object,
    &dnx_apt_mpls_egress_port_insert_object,
    &dnx_apt_l3_arp_insert_object,
    &dnx_apt_l3_arp_update_object,

    &dnx_apt_l3_fec_insert_object,
    &dnx_apt_l3_fec_update_object,
    &dnx_apt_l3_fec_get_object,
    &dnx_apt_l3_fec_destroy_object,

    &dnx_apt_l3_ecmp_multiply_and_divide_insert_object,
    &dnx_apt_l3_ecmp_multiply_and_divide_update_object,
    &dnx_apt_l3_ecmp_multiply_and_divide_get_object,
    &dnx_apt_l3_ecmp_multiply_and_divide_destroy_object,

    &dnx_apt_l3_ecmp_consistent_hashing_small_table_insert_object,
    &dnx_apt_l3_ecmp_consistent_hashing_medium_table_insert_object,
    &dnx_apt_l3_ecmp_consistent_hashing_large_table_insert_object,
    &dnx_apt_l3_ecmp_consistent_hashing_small_table_update_object,
    &dnx_apt_l3_ecmp_consistent_hashing_medium_table_update_object,
    &dnx_apt_l3_ecmp_consistent_hashing_large_table_update_object,
    &dnx_apt_l3_ecmp_consistent_hashing_destroy_object,

    &dnx_apt_tunnel_initiator_create_ipv4_object,
    &dnx_apt_tunnel_initiator_create_ipv6_object,
    &dnx_apt_tunnel_term_ipv4_p2p_insert_object,
    &dnx_apt_tunnel_term_ipv4_mp_insert_object,
    &dnx_apt_tunnel_term_ipv4_tcam_insert_object,
    &dnx_apt_tunnel_term_ipv6_p2p_insert_object,
    &dnx_apt_tunnel_term_ipv6_mp_insert_object,
    &dnx_apt_tunnel_term_txsci_insert_object,
    &dnx_apt_tunnel_vxlan_port_add_object,

    &dnx_apt_vlan_port_create_egress_object,
    &dnx_apt_vlan_port_create_ingress_object,
    &dnx_apt_vlan_port_create_symmetric_object,
    &dnx_apt_vlan_port_create_esem_object,
    &dnx_apt_vlan_port_create_replace_object,
    &dnx_apt_vlan_port_create_native_object,

    &dnx_apt_oam_mep_eth_acc_dnmep_insert_object,
    &dnx_apt_oam_mep_eth_acc_upmep_insert_object,
    &dnx_apt_oam_mep_eth_non_acc_dnmep_insert_object,
    &dnx_apt_oam_mep_eth_non_acc_upmep_insert_object,

    /** SWSATATE tests*/
    &dnx_apt_swstate_htb_small_insert_object,
    &dnx_apt_swstate_htb_small_delete_object,
    &dnx_apt_swstate_htb_small_find_object,
    &dnx_apt_swstate_htb_large_insert_object,
    &dnx_apt_swstate_htb_large_delete_object,
    &dnx_apt_swstate_htb_large_find_object,

    /** MACT tests */
    &dnx_apt_l2_mact_insert_object,

    &dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_insert_object,
    &dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_insert_object,
    &dnx_apt_bfd_endpoint_mpls_tp_cc_acc_insert_object,
    &dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_insert_object,
    &dnx_apt_bfd_endpoint_pwe_control_word_acc_insert_object,
    &dnx_apt_bfd_endpoint_pwe_router_alter_acc_insert_object,
    &dnx_apt_bfd_endpoint_pwe_ttl_acc_insert_object,
    &dnx_apt_bfd_endpoint_pwe_gal_acc_insert_object,

    &dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_update_object,
    &dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_update_object,
    &dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_object,
    &dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_object,
    &dnx_apt_bfd_endpoint_pwe_control_word_acc_update_object,
    &dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_object,
    &dnx_apt_bfd_endpoint_pwe_ttl_acc_update_object,
    &dnx_apt_bfd_endpoint_pwe_gal_acc_update_object,

    &dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_destroy_object,
    &dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_destroy_object,
    &dnx_apt_bfd_endpoint_mpls_tp_cc_acc_destroy_object,
    &dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_destroy_object,
    &dnx_apt_bfd_endpoint_pwe_control_word_acc_destroy_object,
    &dnx_apt_bfd_endpoint_pwe_router_alter_acc_destroy_object,
    &dnx_apt_bfd_endpoint_pwe_ttl_acc_destroy_object,
    &dnx_apt_bfd_endpoint_pwe_gal_acc_destroy_object
};

/* *INDENT-ON* */
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/*
 * API for returning the number of test objects.
 * Outside this file "sizeof(dnx_apt_objects)" cannot be resolved.
 */
int
dnx_apt_number_of_objects(
    void)
{
    return sizeof(dnx_apt_objects) / sizeof(dnx_apt_object_def_t *);
}

/*
 * }
 */
