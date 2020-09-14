/*
 * ! \file ctest_dnx_apt_object_def.c
 * Contains all of the API performance test objects. New test objects should be added here.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
    dnx_apt_l3_route_ipv4_update_object,
    dnx_apt_l3_route_ipv6_insert_object,
    dnx_apt_l3_route_ipv6_update_object,

    /** L3 Host tests */
    dnx_apt_l3_host_ipv4_insert_object,
    dnx_apt_l3_host_ipv4_update_object,
    dnx_apt_l3_host_ipv6_insert_object,
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
    dnx_apt_mpls_ingress_port_insert_object,
    dnx_apt_mpls_egress_port_insert_object,
    dnx_apt_l3_arp_insert_object,
    dnx_apt_l3_arp_update_object,

    dnx_apt_l3_fec_insert_object,
    dnx_apt_l3_fec_update_object,
    dnx_apt_l3_fec_destroy_object,

    dnx_apt_tunnel_initiator_create_ipv4_object,
    dnx_apt_tunnel_initiator_create_ipv6_object,

    dnx_apt_vlan_port_create_egress_object,
    dnx_apt_vlan_port_create_ingress_object,
    dnx_apt_vlan_port_create_symmetric_object,
    dnx_apt_vlan_port_create_esem_object,
    dnx_apt_vlan_port_create_replace_object,
    dnx_apt_vlan_port_create_native_object;

/***********************************************************************************************************************
 * Definition of the list of API performance test objects.
 **********************************************************************************************************************/
const dnx_apt_object_def_t *dnx_apt_objects[] = {
    /** L3 Route tests */
    &dnx_apt_l3_route_ipv4_insert_object,
    &dnx_apt_l3_route_ipv4_update_object,
    &dnx_apt_l3_route_ipv6_insert_object,
    &dnx_apt_l3_route_ipv6_update_object,

    /** L3 Host tests */
    &dnx_apt_l3_host_ipv4_insert_object,
    &dnx_apt_l3_host_ipv4_update_object,
    &dnx_apt_l3_host_ipv6_insert_object,
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
    &dnx_apt_mpls_ingress_port_insert_object,
    &dnx_apt_mpls_egress_port_insert_object,
    &dnx_apt_l3_arp_insert_object,
    &dnx_apt_l3_arp_update_object,

    &dnx_apt_l3_fec_insert_object,
    &dnx_apt_l3_fec_update_object,
    &dnx_apt_l3_fec_destroy_object,

    &dnx_apt_tunnel_initiator_create_ipv4_object,
    &dnx_apt_tunnel_initiator_create_ipv6_object,

    &dnx_apt_vlan_port_create_egress_object,
    &dnx_apt_vlan_port_create_ingress_object,
    &dnx_apt_vlan_port_create_symmetric_object,
    &dnx_apt_vlan_port_create_esem_object,
    &dnx_apt_vlan_port_create_replace_object,
    &dnx_apt_vlan_port_create_native_object
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
