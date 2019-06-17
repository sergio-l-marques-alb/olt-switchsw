/** \file ctest_dnx_bfd.c
 * 
 * Tests for BFD
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/bfd.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include "src/bcm/dnx/algo/lif_mngr/global_lif_allocation.h"
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include <include/bcm/stack.h>

#define IPV6_STR_LEN (3 * sizeof(bcm_ip6_t))

/** Value used as a not-so-random seed to generate values   */
#define INIT_GEN_VALUE 0x2A95BD

/** 
 *  Value used to generate a sequence of values that will be the
 *  same every time the test is run
 */
#define GEN_VALUE_DELTA 0xCF

/** 
 *  Value used for upper 14 bits of local discriminator
 *  Lower bits 18 of this number are meaningless and will be
 *  masked
 */
#define FIXED_DISCR_RANGE 0xE9BA38DF

/** This value must be constant for all endpoints */
#define FIXED_UDP_SRC_PORT 50000

/** Constant values used for test   */
#define PWE_VPN_ID           0x28
#define MPLS_PORT_NETWORK_ID 0x652
#define GPORT_INIT_ID        0x7000
#define GPORT_ID_DELTA       0x1000
#define MPLS_INIT_LABEL      0x2000

/** Creation of discriminator for accelerated endpoint */

/** This bit determines: 1/4 entry or self contained? */
#define Q_ENTRY_BIT SAL_BIT(0)

/** Value of discriminator - this bit is LSB */
#define ACC_DISCR_OFFSET 1

/** MSB of 1/4 entry ID mask */
#define Q_ENTRY_MSB 15

/** MSB of full entry ID mask - before shift */
#define FULL_ENTRY_MSB 13

/** Bit shift from entry ID to "oam ID" */
#define FULL_ENTRY_ID_SHIFT 2

/** End of creation of discriminator for accelerated endpoint */

/** Creation of RMEP ID for accelerated endpoints */

/** This bit determines if a full entry or half entry is used */
#define RMEP_H_ENTRY_BIT SAL_BIT(0)

/** Value of RMEP ID: this bit is LSB */
#define REMOTE_ID_OFFSET 1

/** Bit shift from entry ID to "RMEP ID" */
#define FULL_RMEP_ENTRY_SHIFT 1

/** End of creation of RMEP ID for accelerated endpoints */

/** Subnet length: number of bits, and maximum value */
#define DNX_BFD_NOF_SUBNET_LEN_BITS 5
#define DNX_BFD_MAX_SUBNET_LEN SAL_UPTO_BIT(DNX_BFD_NOF_SUBNET_LEN_BITS)

/** BFD period: number of bits, and maximum value */
#define DNX_BFD_NOF_BFD_PERIOD_BITS 10
#define DNX_BFD_MAX_BFD_PERIOD SAL_UPTO_BIT(DNX_BFD_NOF_BFD_PERIOD_BITS)

/** int_pri: number of bits, and maximum value */
#define DNX_BFD_NOF_INT_PRI_BITS 5
#define DNX_BFD_MAX_INT_PRI SAL_UPTO_BIT(DNX_BFD_NOF_INT_PRI_BITS)

/** egress_if: LSB of gen_value determines FEC or out-LIF */
#define FEC_OR_OUT_LIF_BIT SAL_BIT(0)
#define ITF_ID_SHIFT 1

/** TOS: number of bits, and maximum value */
#define DNX_BFD_NOF_TOS_BITS 8
#define DNX_BFD_MAX_TOS SAL_UPTO_BIT(DNX_BFD_NOF_TOS_BITS)

/** TTL: number of bits, and maximum value */
#define DNX_BFD_NOF_TTL_BITS 8
#define DNX_BFD_MAX_TTL SAL_UPTO_BIT(DNX_BFD_NOF_TTL_BITS)

/** MPLS/PWE label - fewer bits for short entry */
#define DNX_BFD_NOF_MPLS_LABEL_BITS 20
#define DNX_BFD_MAX_MPLS_LABEL SAL_UPTO_BIT(DNX_BFD_NOF_MPLS_LABEL_BITS)

/** EXP: number of bits, and maximum value */
#define DNX_BFD_NOF_EXP_BITS 3
#define DNX_BFD_MAX_EXP SAL_UPTO_BIT(DNX_BFD_NOF_EXP_BITS)

/** State: number of bits, and maximum value */
#define DNX_BFD_NOF_STATE_BITS 2
#define DNX_BFD_MAX_STATE SAL_UPTO_BIT(DNX_BFD_NOF_STATE_BITS)

/** Diag: number of allowed values */
#define NOF_DIAG_VALUES 10

/** Flags profile: number of bits, and maximum value */
#define DNX_BFD_NOF_FLAGS_PROFILE_BITS 4
#define DNX_BFD_MAX_FLAGS_PROFILE SAL_UPTO_BIT(DNX_BFD_NOF_FLAGS_PROFILE_BITS)

/** Detection multiplier: number of bits, and maximum value */
#define DNX_BFD_NOF_DETECT_MULT_BITS 8
#define DNX_BFD_MAX_DETECT_MULT SAL_UPTO_BIT(DNX_BFD_NOF_DETECT_MULT_BITS)

/** Explicit detection time: what will be the delta from the default? */

/** If this bit is set, use explicit detection time */
#define EXPLICIT_DETECTION_BIT SAL_BIT(0)

/** Offset starts from this bit */
#define DETECT_OFFSET_START 1

/** Number of bits for the offset */
#define DETECT_OFFSET_NOF_BITS 4

/** Value range for the offset */
#define DETECTION_TIME_RANGE SAL_UPTO_BIT(DETECT_OFFSET_NOF_BITS)

/**
 * Middle of the range - subtracting this allows us to
 * have positive and negative offsets
 */
#define HALF_RANGE SAL_BIT(DETECT_OFFSET_NOF_BITS - 1)

/** loc_clear_thershold: number of bits, and maximum value */
#define DNX_BFD_NOF_LOC_CLEAR_THRESHOLD_BITS 2
#define DNX_BFD_MAX_LOC_CLEAR_THRESHOLD SAL_UPTO_BIT(DNX_BFD_NOF_LOC_CLEAR_THRESHOLD_BITS)

/** Sampling ratio: range of legal values */
#define NOF_SAMPLING_RATIO_VALUES 9

/** Number of physical ports that can be used for Tx */
#define NOF_PHYS_PORTS 2

/** Maximum number of diffrent required intervals */
#define NOF_REQ_INTS 8

/** Structure to keep current values of parameters   */
typedef struct
{
    /** 
     *  Value that is used for parameters with no numeral
     *  restrictions: local discriminator LSB, parts of IPv6
     *  address and IPv4 address
     */
    uint32 gen_value;

    /**
     * Since these values are limited in number, they
     * are set at the beginning of the test.
     */
    uint32 req_ints[NOF_REQ_INTS];

    /** Value of gport for tunneled BFD */
    bcm_gport_t cur_gport;

    /** Label for MPLS encapsulation   */
    bcm_mpls_label_t mpls_label;

    /** Structure for adding MPLS encapsulation   */
    bcm_mpls_port_t mpls_port_ingress;

    /** Specifies whether this is a short entry */
    uint8 is_short_entry;

    /** Specifies whether MEP type in discriminator is used */
    uint8 mep_type_discr;
} diag_dnx_bfd_semantic_test_struct;

const uint16 physical_ports[NOF_PHYS_PORTS] = {
    200,
    201
};

/** The actual port value; the upper 16 bits are a header */
#define LOCAL_PORT_BITS SAL_UPTO_BIT(16)

/**
 * \brief - Write values for the destination IP (if applicable) 
 *        and the MP profile, the values that can be modified in
 *        a BFD endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] for_modification - if this value is true, 
 *        overwrite only fields that may be modified after the
 *        endpoint was created.
 * \param [in,out] endpoint_info - structure from which flags 
 *        are read and to which values are written
 * \param [in,out] test - structure that holds "global" 
 *        variables for test.
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_bfd_test_generate_params(
    int unit,
    int for_modification,
    bcm_bfd_endpoint_info_t * endpoint_info,
    diag_dnx_bfd_semantic_test_struct * test)
{
    uint32 ip6_index, ip6_segment, discr_high, discr_low;
    uint32 trap_id, trap_strength, snoop_strength;
    uint32 num_fec_ids, num_lif_ids, itf_id;
    int explicit_detection_time, mep_db_threshold, avail_full_entries, total_full_entries;
    int rmep_db_threshold, total_full_rmep_entries, avail_full_rmep_entries;
    uint8 itf_type, port_index;
    bcm_gport_t gport_id;
    SHR_FUNC_INIT_VARS(unit);

    /** 
     *  These fields may not be modified after endpoint is
     *  created
     */
    if (for_modification == FALSE)
    {
        /** The discriminator range is set with this value of the MSB */
        discr_high = (FIXED_DISCR_RANGE & UTILEX_BITS_MASK(SAL_UINT32_NOF_BITS - 1, DISCR_RANGE_BIT_SHIFT));

        if(test->mep_type_discr)
        {
            UTILEX_SET_BIT(discr_high, !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP), SAL_UINT32_NOF_BITS - 1);
        }

        /** Is the endpoint accelerated? */
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
        {
            /** UDP source port is a constant */
            endpoint_info->udp_src_port = FIXED_UDP_SRC_PORT;

            mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
            total_full_entries = dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get(unit);
            avail_full_entries = total_full_entries - (mep_db_threshold >> FULL_ENTRY_ID_SHIFT);
            if (((test->gen_value & Q_ENTRY_BIT) != 0) && (mep_db_threshold != 0))
            {
                /** Build a discriminator that results in a 1/4 entry endpoint */
                discr_low = ((test->gen_value >> ACC_DISCR_OFFSET) % mep_db_threshold);
                test->is_short_entry = TRUE;
            }
            else
            {
                /** Build a discriminator that results in a self-contained endpoint */
                discr_low =
                    mep_db_threshold +
                    (((test->gen_value >> ACC_DISCR_OFFSET) % avail_full_entries) << FULL_ENTRY_ID_SHIFT);
                test->is_short_entry = FALSE;
            }
            test->gen_value += GEN_VALUE_DELTA;
            endpoint_info->local_discr = discr_high | discr_low;
            endpoint_info->gport = 0;

            /** Also, try explicit remote ID */
            if ((endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_WITH_ID) != 0)
            {
                rmep_db_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
                if ((test->gen_value & RMEP_H_ENTRY_BIT) && (rmep_db_threshold != 0))
                {
                    /** Make sure the RMEP ID is below the threshold */
                    endpoint_info->remote_id = ((test->gen_value >> REMOTE_ID_OFFSET) % rmep_db_threshold);
                }
                else
                {
                    /** Make sure the RMEP ID is above the threshold */
                    rmep_db_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
                    total_full_rmep_entries =
                        (dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) - rmep_db_threshold) / 2;
                    avail_full_rmep_entries = total_full_rmep_entries - (rmep_db_threshold >> FULL_RMEP_ENTRY_SHIFT);
                    endpoint_info->remote_id = rmep_db_threshold +
                        (((test->gen_value >> REMOTE_ID_OFFSET) % avail_full_rmep_entries) << FULL_RMEP_ENTRY_SHIFT);
                }
            }
        }
        else
        {
            /** Is the endpoint classified by discriminator or LIF?   */
            if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
            {
                                /** Generate discriminator   */
                discr_low = (test->gen_value & UTILEX_BITS_MASK(DISCR_RANGE_BIT_SHIFT - 1, 0));
                test->gen_value += GEN_VALUE_DELTA;
                endpoint_info->local_discr = discr_high | discr_low;
                endpoint_info->gport = 0;
            }
        }

        if (!BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
        {
                        /** Configure MPLS port for endpoints classified by LIF   */
            bcm_mpls_port_t_init(&test->mpls_port_ingress);
            test->mpls_port_ingress.flags = BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
            test->mpls_port_ingress.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
            test->mpls_port_ingress.criteria = BCM_MPLS_PORT_MATCH_LABEL;
            BCM_GPORT_LOCAL_SET(test->mpls_port_ingress.port, test->cur_gport);
            BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, test->cur_gport);
            BCM_GPORT_MPLS_PORT_ID_SET(test->mpls_port_ingress.mpls_port_id, gport_id);
            test->mpls_port_ingress.encap_id = test->cur_gport;
            test->mpls_port_ingress.match_label = test->mpls_label;
            /*
             * Coverity false positives: this line is not copied from anywhere, and is not remotely Similar to
             * "SHR_FUNC_INIT_VARS(unit);"
             */
            /*
             * coverity[Copy-paste error:false]
             */
            SHR_CLI_EXIT_IF_ERR(bcm_mpls_port_add(unit, PWE_VPN_ID, &test->mpls_port_ingress),
                                "Test failed.  Could not create MPLS tunnel\n");

                        /** Generate LIF   */
            endpoint_info->gport = test->mpls_port_ingress.mpls_port_id;
            test->mpls_label += GEN_VALUE_DELTA;
        }
    }

    if ((endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) != 0)
    {
        /** Destination IP only relevant in multi-hop   */
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) != 0)
        {
            /** Generate IPv6 address   */
            for (ip6_index = 0; ip6_index < sizeof(bcm_ip6_t); ip6_index += sizeof(int))
            {
                ip6_segment = test->gen_value;
                test->gen_value += GEN_VALUE_DELTA;
                sal_memcpy(&endpoint_info->src_ip6_addr[ip6_index], &ip6_segment, sizeof(int));
            }
        }
        else
        {
            /** Generate IPv4 address   */
            endpoint_info->src_ip_addr = test->gen_value;
            test->gen_value += GEN_VALUE_DELTA;
        }
    }

    /** Build remote_gport field from its parts   */
    trap_id = ((test->gen_value) % DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES);
    test->gen_value += GEN_VALUE_DELTA;
    trap_strength = ((test->gen_value) & DNX_OAM_MAX_FORWARDING_STRENGTH);
    test->gen_value += GEN_VALUE_DELTA;
    snoop_strength = ((test->gen_value) & DNX_OAM_MAX_SNOOP_STRENGTH);
    test->gen_value += GEN_VALUE_DELTA;
    _SHR_GPORT_TRAP_SET(endpoint_info->remote_gport, trap_id, trap_strength, snoop_strength);

    if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
    {
        /** Fields relevant for locally  accelerated endpoints only */

        if ((bcmBFDTunnelTypeUdp == endpoint_info->type && (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) == 0))
        {
                /** This field is relevant only for IPv4 single-hop */
            endpoint_info->ip_subnet_length = ((test->gen_value) & DNX_BFD_MAX_SUBNET_LEN);
            if (endpoint_info->ip_subnet_length == DNX_BFD_MAX_SUBNET_LEN)
                endpoint_info->ip_subnet_length = DNX_BFD_MAX_SUBNET_LEN + 1;
            test->gen_value += GEN_VALUE_DELTA;
        }

        /** This number should be a multiple of 167, and may be 0 */
        endpoint_info->bfd_period = (test->gen_value) % (DNX_BFD_MAX_BFD_PERIOD / 167) * 167;
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->int_pri = ((test->gen_value) & DNX_BFD_MAX_INT_PRI);
        test->gen_value += GEN_VALUE_DELTA;

        if ((test->gen_value) & FEC_OR_OUT_LIF_BIT)
        {
            itf_type = BCM_L3_ITF_TYPE_FEC;
            num_fec_ids = dnx_data_l3.fec.nof_fecs_get(unit);
            itf_id = (test->gen_value) % num_fec_ids;
            endpoint_info->tx_gport = BCM_GPORT_INVALID;
        }
        else
        {
            itf_type = _SHR_L3_ITF_TYPE_LIF;
            num_lif_ids = dnx_data_lif.global_lif.nof_global_out_lifs_get(unit);
            itf_id = (test->gen_value) % num_lif_ids;
        }
        BCM_L3_ITF_SET(endpoint_info->egress_if, itf_type, itf_id >> ITF_ID_SHIFT);
        test->gen_value += GEN_VALUE_DELTA;

        port_index = (test->gen_value) % NOF_PHYS_PORTS;
        /*
         * SHR_CLI_EXIT_IF_ERR(bcm_stk_sysport_gport_get(unit, physical_ports[port_index], &endpoint_info->tx_gport),
         * "Test failed.  Could not get system port.\n"); 
         */
        if (itf_type != BCM_L3_ITF_TYPE_FEC)
        {
            endpoint_info->tx_gport = physical_ports[port_index];
            test->gen_value += GEN_VALUE_DELTA;
        }

        /** Irrelevant for MPLS/PWE */
        if (bcmBFDTunnelTypeUdp == endpoint_info->type)
        {
            if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) != 0)
            {
                /** Generate IPv6 address   */
                for (ip6_index = 0; ip6_index < sizeof(bcm_ip6_t); ip6_index += sizeof(int))
                {
                    ip6_segment = test->gen_value;
                    test->gen_value += GEN_VALUE_DELTA;
                    sal_memcpy(&endpoint_info->dst_ip6_addr[ip6_index], &ip6_segment, sizeof(int));
                }
            }
            else
            {
                /** Generate IPv4 address   */
                endpoint_info->dst_ip_addr = test->gen_value;
                test->gen_value += GEN_VALUE_DELTA;
            }
        }

        if (((bcmBFDTunnelTypeUdp == endpoint_info->type)
             && _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP))
            || (bcmBFDTunnelTypeMpls == endpoint_info->type))
        {
                /** IP TOS and TTL relevant only for multi-hop and MPLS */
            test->gen_value += GEN_VALUE_DELTA;
            endpoint_info->ip_tos = ((test->gen_value) & DNX_BFD_MAX_TOS);
            test->gen_value += GEN_VALUE_DELTA;
        }

        if (((bcmBFDTunnelTypeUdp == endpoint_info->type)
             && _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP))) {
            endpoint_info->ip_ttl = ((test->gen_value) & DNX_BFD_MAX_TTL);
            test->gen_value += GEN_VALUE_DELTA;
         }
        
        if (endpoint_info->type != bcmBFDTunnelTypeUdp)
        {
             /** Relevant only for MPLS/PWE */
            endpoint_info->label = ((test->gen_value) & DNX_BFD_MAX_MPLS_LABEL);
            test->gen_value += GEN_VALUE_DELTA;

            endpoint_info->egress_label.ttl = ((test->gen_value) & DNX_BFD_MAX_TTL);
            test->gen_value += GEN_VALUE_DELTA;

            endpoint_info->egress_label.exp = ((test->gen_value) & DNX_BFD_MAX_EXP);
            test->gen_value += GEN_VALUE_DELTA;
        }

        endpoint_info->local_state = ((test->gen_value) & DNX_BFD_MAX_STATE);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->local_diag = ((test->gen_value) % NOF_DIAG_VALUES);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->local_flags = BFD_FLAGS_PROFILE_TO_VAL((test->gen_value) & DNX_BFD_MAX_FLAGS_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->local_detect_mult = ((test->gen_value) & DNX_BFD_MAX_DETECT_MULT);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->local_min_rx = test->req_ints[test->gen_value % NOF_REQ_INTS];
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->local_min_tx = test->req_ints[test->gen_value % NOF_REQ_INTS];
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->remote_detect_mult = ((test->gen_value) & DNX_BFD_MAX_DETECT_MULT);
        test->gen_value += GEN_VALUE_DELTA;

        if ((test->gen_value & EXPLICIT_DETECTION_BIT) != 0)
        {
            endpoint_info->flags |= BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME;
            explicit_detection_time =
                ((test->gen_value >> DETECT_OFFSET_START) & DETECTION_TIME_RANGE) - HALF_RANGE +
                (endpoint_info->local_min_rx * endpoint_info->remote_detect_mult);
            endpoint_info->bfd_detection_time = explicit_detection_time;
            test->gen_value += GEN_VALUE_DELTA;
        }

        endpoint_info->loc_clear_threshold = ((test->gen_value) & DNX_BFD_MAX_LOC_CLEAR_THRESHOLD);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->sampling_ratio = ((test->gen_value) % NOF_SAMPLING_RATIO_VALUES);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->remote_discr = test->gen_value;
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->remote_diag = ((test->gen_value) % NOF_DIAG_VALUES);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->remote_flags = BFD_FLAGS_PROFILE_TO_VAL((test->gen_value) & DNX_BFD_MAX_FLAGS_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->remote_state = ((test->gen_value) & DNX_BFD_MAX_STATE);
        test->gen_value += GEN_VALUE_DELTA;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function formats an IPv6 address to a character 
 *        string (xx:xx:xx...)
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [out] result_str - Pointer to start of string
 * \param [in] ipv6_addr - IPv6 address
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_bfd_test_ipv6_addr_to_str(
    int unit,
    char *result_str,
    bcm_ip6_t ipv6_addr)
{
    int index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(result_str, _SHR_E_PARAM, "result_str");
    SHR_NULL_CHECK(ipv6_addr, _SHR_E_PARAM, "ipv6_addr");

    sal_sprintf(result_str, "%02X", ipv6_addr[0]);
    for (index = 1; index < sizeof(bcm_ip6_t); index++)
    {
        sal_sprintf(result_str, "%s:%02X", result_str, ipv6_addr[index]);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function compares two information structure, 
 *        presumably one that contains data to create a or
 *        modify a BFD endpoint, and the other that was read
 *        after the creation/modification occurs, and verifies
 *        that the relevant values are identical.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - Structure that was used for 
 *        creation or modification
 * \param [in] endpoint_info2 - Structure to which the data was 
 *        later read.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_bfd_test_compare_bfd_info_structs(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bcm_bfd_endpoint_info_t * endpoint_info2)
{
    char *ipv6_str = NULL, *ipv6_str2 = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate strings for IPv6 addresses   */
    SHR_ALLOC(ipv6_str, IPV6_STR_LEN, "String for written IPv6 address", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(ipv6_str2, IPV6_STR_LEN, "String for read IPv6 address", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (endpoint_info2->id != endpoint_info->id)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Endpoint ID read doesn't match endpoint ID written to endpoint.  "
                     "Written: 0x%08X. Read: 0x%08X.\n", endpoint_info->id, endpoint_info2->id);
    }
    if (endpoint_info2->type != endpoint_info->type)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Tunnel type read doesn't match type written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                     endpoint_info->type, endpoint_info2->type);
    }

    /** This flag is not saved by the API */
    if (endpoint_info2->flags != (endpoint_info->flags & ~BCM_BFD_ENDPOINT_REMOTE_WITH_ID))
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Flags read don't match flags written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                     endpoint_info->flags, endpoint_info2->flags);
    }

    /** Check source IP only for multi-hop   */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)
    {
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) != 0)
        {
            /** Compare IPv6 addresses   */
            if (sal_memcmp(endpoint_info2->src_ip6_addr, endpoint_info->src_ip6_addr, sizeof(bcm_ip6_t)) != 0)
            {
                SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_ipv6_addr_to_str(unit, ipv6_str, endpoint_info->src_ip6_addr),
                                    "Test failed.  NULL pointer error.\n");
                SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_ipv6_addr_to_str(unit, ipv6_str2, endpoint_info2->src_ip6_addr),
                                    "Test failed.  NULL pointer error.\n");
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  Source IP read doesn't match source IP written to endpoint.  "
                             "Written: %s. Read: %s.\n", ipv6_str, ipv6_str2);
            }
        }
        else
        {
            /** Compare IPv4 addresses   */
            if (endpoint_info2->src_ip_addr != endpoint_info->src_ip_addr)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  Source IP read doesn't match source IP written to endpoint.  "
                             "Written: %d.%d.%d.%d. Read: %d.%d.%d.%d.\n",
                             UTILEX_GET_BYTE_0(endpoint_info->src_ip_addr),
                             UTILEX_GET_BYTE_1(endpoint_info->src_ip_addr),
                             UTILEX_GET_BYTE_2(endpoint_info->src_ip_addr),
                             UTILEX_GET_BYTE_3(endpoint_info->src_ip_addr),
                             UTILEX_GET_BYTE_0(endpoint_info2->src_ip_addr),
                             UTILEX_GET_BYTE_1(endpoint_info2->src_ip_addr),
                             UTILEX_GET_BYTE_2(endpoint_info2->src_ip_addr),
                             UTILEX_GET_BYTE_3(endpoint_info2->src_ip_addr));
            }
        }
    }
    if (endpoint_info2->remote_gport != endpoint_info->remote_gport)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  No match between remote_gport read from endpoint and value "
                     "written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n", endpoint_info->remote_gport,
                     endpoint_info2->remote_gport);
    }
    /** Is the endpoint classified by discriminator or LIF?   */
    if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
    {
        if (endpoint_info2->local_discr != endpoint_info->local_discr)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  No match between local_discr read from endpoint and value "
                         "written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n", endpoint_info->local_discr,
                         endpoint_info2->local_discr);
        }
    }
    else
    {
        if (BCM_GPORT_LOCAL_GET(endpoint_info2->gport) != BCM_GPORT_LOCAL_GET(endpoint_info->gport))
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  No match between gport read from endpoint and value "
                         "written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n", endpoint_info->gport,
                         endpoint_info2->gport);
        }
    }

    if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
    {
        /** Fields relevant for acclerated endpoints */
        if (((bcmBFDTunnelTypeUdp == endpoint_info->type &&
              (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) == 0)) &&
            (endpoint_info2->ip_subnet_length != endpoint_info->ip_subnet_length))
        {
                /** This field is relevant only for IPv4 single-hop */
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Subnet length read doesn't match Subnet length written to endpoint.  "
                         "Written: 0x%08X. Read: 0x%08X.\n", endpoint_info->ip_subnet_length,
                         endpoint_info2->ip_subnet_length);
        }

        if (endpoint_info2->bfd_period != endpoint_info->bfd_period)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  BFD period read doesn't match period written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->bfd_period, endpoint_info2->bfd_period);
        }

        if (endpoint_info2->int_pri != endpoint_info->int_pri)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  int_pri read doesn't match int_pri written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->int_pri, endpoint_info2->int_pri);
        }

        if (endpoint_info2->egress_if != endpoint_info->egress_if)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Egress interface read doesn't match egress interface written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->egress_if, endpoint_info2->egress_if);
        }

        if ((endpoint_info2->tx_gport & LOCAL_PORT_BITS) != (endpoint_info->tx_gport & LOCAL_PORT_BITS))
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  tx_gport read doesn't match tx_gport written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->tx_gport, endpoint_info2->tx_gport);
        }

        if (bcmBFDTunnelTypeUdp == endpoint_info->type)
        {
            if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) != 0)
            {
                /** Compare IPv6 addresses   */
                if (sal_memcmp(endpoint_info2->dst_ip6_addr, endpoint_info->dst_ip6_addr, sizeof(bcm_ip6_t)) != 0)
                {
                    SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_ipv6_addr_to_str(unit, ipv6_str, endpoint_info->dst_ip6_addr),
                                        "Test failed.  NULL pointer error.\n");
                    SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_ipv6_addr_to_str
                                        (unit, ipv6_str2, endpoint_info2->dst_ip6_addr),
                                        "Test failed.  NULL pointer error.\n");
                    SHR_CLI_EXIT(_SHR_E_FAIL,
                                 "Test failed.  Destination IP read doesn't match destination IP written to endpoint.  "
                                 "Written: %s. Read: %s.\n", ipv6_str, ipv6_str2);
                }
            }
            else
            {
                /** Compare IPv4 addresses   */
                if (endpoint_info2->dst_ip_addr != endpoint_info->dst_ip_addr)
                {
                    SHR_CLI_EXIT(_SHR_E_FAIL,
                                 "Test failed.  Destination IP read doesn't match destination IP written to endpoint.  "
                                 "Written: %d.%d.%d.%d. Read: %d.%d.%d.%d.\n",
                                 UTILEX_GET_BYTE_0(endpoint_info->dst_ip_addr),
                                 UTILEX_GET_BYTE_1(endpoint_info->dst_ip_addr),
                                 UTILEX_GET_BYTE_2(endpoint_info->dst_ip_addr),
                                 UTILEX_GET_BYTE_3(endpoint_info->dst_ip_addr),
                                 UTILEX_GET_BYTE_0(endpoint_info2->dst_ip_addr),
                                 UTILEX_GET_BYTE_1(endpoint_info2->dst_ip_addr),
                                 UTILEX_GET_BYTE_2(endpoint_info2->dst_ip_addr),
                                 UTILEX_GET_BYTE_3(endpoint_info2->dst_ip_addr));
                }
            }
        }

        if (((bcmBFDTunnelTypeUdp == endpoint_info->type)
             && _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP))
            || (bcmBFDTunnelTypeMpls == endpoint_info->type))
        {
            if (endpoint_info2->ip_tos != endpoint_info->ip_tos)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  IP TOS read doesn't match IP TOS written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                             endpoint_info->ip_tos, endpoint_info2->ip_tos);
            }

            if (endpoint_info2->ip_ttl != endpoint_info->ip_ttl)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  IP TTL read doesn't match IP TTL written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                             endpoint_info->ip_ttl, endpoint_info2->ip_ttl);
            }
        }

        if (endpoint_info->type != bcmBFDTunnelTypeUdp)
        {
            if (endpoint_info2->label != endpoint_info->label)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  Label read doesn't match label written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                             endpoint_info->label, endpoint_info2->label);
            }

            if (endpoint_info2->egress_label.ttl != endpoint_info->egress_label.ttl)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  Egress label TTL read doesn't match egress label TTL written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                             endpoint_info->egress_label.ttl, endpoint_info2->egress_label.ttl);
            }

            if (endpoint_info2->egress_label.exp != endpoint_info->egress_label.exp)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  Egress label EXP read doesn't match egress label EXP written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                             endpoint_info->egress_label.exp, endpoint_info2->egress_label.exp);
            }
        }

        if (endpoint_info2->local_state != endpoint_info->local_state)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Local BFD state read doesn't match local BFD state written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->local_state, endpoint_info2->local_state);
        }

        if (endpoint_info2->local_diag != endpoint_info->local_diag)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Local diag read doesn't match local diag written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->local_diag, endpoint_info2->local_diag);
        }

        if (endpoint_info2->local_flags != endpoint_info->local_flags)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Local flags read doesn't match local flags written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->local_flags, endpoint_info2->local_flags);
        }

        if (endpoint_info2->local_detect_mult != endpoint_info->local_detect_mult)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Local detect mult read doesn't match local detect mult written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->local_detect_mult, endpoint_info2->local_detect_mult);
        }

        if (endpoint_info2->local_min_rx != endpoint_info->local_min_rx)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Required min Rx inverval read doesn't match required min Rx inverval written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->local_min_rx, endpoint_info2->local_min_rx);
        }

        if (endpoint_info2->local_min_tx != endpoint_info->local_min_tx)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Desired min Tx inverval read doesn't match Desired min Tx inverval written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->local_min_tx, endpoint_info2->local_min_tx);
        }

        if (endpoint_info2->remote_detect_mult != endpoint_info->remote_detect_mult)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Remote detect mult read doesn't match local remote mult written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->remote_detect_mult, endpoint_info2->remote_detect_mult);
        }

        if (endpoint_info2->loc_clear_threshold != endpoint_info->loc_clear_threshold)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  LOC clear threshold read doesn't match LOC clear threshold written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->loc_clear_threshold, endpoint_info2->loc_clear_threshold);
        }

        if (endpoint_info2->sampling_ratio != endpoint_info->sampling_ratio)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Sampling ratio read doesn't match sampling ratio written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->loc_clear_threshold, endpoint_info2->loc_clear_threshold);
        }

        if (endpoint_info2->remote_discr != endpoint_info->remote_discr)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Remote discriminator read doesn't match remote discriminator written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->remote_discr, endpoint_info2->remote_discr);
        }

        if (endpoint_info2->remote_diag != endpoint_info->remote_diag)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Remote diag read doesn't match remote diag written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->remote_diag, endpoint_info2->remote_diag);
        }

        if (endpoint_info2->remote_flags != endpoint_info->remote_flags)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Remote flags read doesn't match remote flags written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->remote_flags, endpoint_info2->remote_flags);
        }

        if (endpoint_info2->remote_state != endpoint_info->remote_state)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Remote state read doesn't match remote state written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->remote_state, endpoint_info2->remote_state);
        }
    }

exit:
    SHR_FREE(ipv6_str);
    SHR_FREE(ipv6_str2);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the BFD feature in a very basic 
 *        way: a number of BFD endpoints are created and
 *        verified, modified and verified again, and finally
 *        deleted.  It is part of the ctest feature.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line 
 *        arguments were parsed (partially)
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_bfd_semantic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 num_eps, flags, ep_index, half_num_eps, temp, mep_db_lim;
    int rv, req_int_index;
    bcm_bfd_tunnel_type_t ep_type = bcmBFDTunnelTypeUdp;
    bcm_bfd_endpoint_t *endpoint_id = NULL;
    bcm_bfd_endpoint_info_t *endpoint_info = NULL, *endpoint_info2 = NULL;
    diag_dnx_bfd_semantic_test_struct *test = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("Number", num_eps);
    SH_SAND_GET_ENUM("Type", ep_type);
    SH_SAND_GET_ENUM("FLaGs", flags);

    LOG_CLI((BSL_META("Test with %d endpoints\n"), num_eps));
    LOG_CLI((BSL_META("Test with tunnel Type = %d\n"), ep_type));
    LOG_CLI((BSL_META("Test with flags = 0x%08X\n"), flags));

    /** Set values for tunnel-type loop   */
    half_num_eps = (num_eps >> 1);

    SHR_ALLOC(endpoint_info, sizeof(bcm_bfd_endpoint_info_t), "Struct for creating BFD endpoints",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(endpoint_info2, sizeof(bcm_bfd_endpoint_info_t), "Struct for reading BFD endpoints",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(endpoint_id, num_eps * sizeof(bcm_bfd_endpoint_t), "Array of endpoint IDs", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(test, sizeof(diag_dnx_bfd_semantic_test_struct), "Struct for test variables", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);

    bcm_bfd_endpoint_info_t_init(endpoint_info);
    bcm_bfd_endpoint_info_t_init(endpoint_info2);

    test->gen_value = INIT_GEN_VALUE;
    test->cur_gport = GPORT_INIT_ID;
    test->mpls_label = MPLS_INIT_LABEL;

    /**
     * This flag is not in use in the DNX line.
     * Here it is used to avoid adding a new
     * parameter to the the BFD ctest
     */
    if(flags & BCM_BFD_ENDPOINT_KEY_TYPE_USE_YOUR_DISC)
    {
        flags &= ~BCM_BFD_ENDPOINT_KEY_TYPE_USE_YOUR_DISC;
        test->mep_type_discr = TRUE;
        if(dnx_data_bfd.property.discriminator_type_update_get(unit) == 0)
        {
            SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "config add custom_feature_discriminator_type_update=1"), "Error - unable to set property\n");
            /*
             * Run Deinit-Init DNX DATA - the SoC properties loaded upon init
             */
            SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141"), "Error - init-deinit failed\n");
        }
    }
    else
    {
        if(dnx_data_bfd.property.discriminator_type_update_get(unit) == 1)
        {
            SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "config add custom_feature_discriminator_type_update=0"), "Error - unable to set property\n");
            /*
             * Run Deinit-Init DNX DATA - the SoC properties loaded upon init
             */
            SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141"), "Error - init-deinit failed\n");
        }
    }

    if (((flags & BCM_BFD_ENDPOINT_IN_HW) != 0) && ((num_eps & 3) != 0))
    {
        /** Only in case of accelerated MEPs */
        if ((flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
        {
            /** Some tests with remote ID, some without */
            flags |= BCM_BFD_ENDPOINT_REMOTE_WITH_ID;
        }
    }

    /** Fill the structure with the user provided parameters   */
    endpoint_info->flags = flags;
    endpoint_info->id = 0;
    endpoint_info->type = ep_type;

    if ((flags & BCM_BFD_ENDPOINT_IN_HW) != 0) {
        if (ep_type == bcmBFDTunnelTypeUdp &&
                !_SHR_IS_FLAG_SET(flags, BCM_BFD_ENDPOINT_MULTIHOP) &&
                !_SHR_IS_FLAG_SET(flags, BCM_BFD_ECHO)) {
                    endpoint_info->ip_ttl = 255;
        }

        if (ep_type == bcmBFDTunnelTypeMpls) {
            endpoint_info->ip_ttl = 1;
        }
    }

    /** Create VPN for MPLS/PWE tunnel endpoints   */
    if (!BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(ep_type))
    {
        SHR_CLI_EXIT_IF_ERR(bcm_vlan_create(unit, PWE_VPN_ID), "Test failed.  Could not create MPLS VLAN\n");
    }

    /** Create required intervals for accelerated endpoints */
    if ((flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
    {
        for (req_int_index = 0; req_int_index < NOF_REQ_INTS; req_int_index++)
        {
            test->req_ints[req_int_index] = test->gen_value;
            test->gen_value += GEN_VALUE_DELTA;
        }
    }

    /*
     * if the endpoint is IPv6, accelerated and self-containd 
     */
    if ((flags & BCM_BFD_ENDPOINT_IPV6) != 0 && (flags & BCM_BFD_ENDPOINT_IN_HW) != 0 &&
        !((dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit) != 0) && ((test->gen_value & Q_ENTRY_BIT) != 0)))
    {
        endpoint_info->ipv6_extra_data_index = 8195;
    }

    mep_db_lim = dnx_data_oam.oamp.mep_db_version_limitation_get(unit);
    for (ep_index = 0; ep_index < num_eps; ep_index++)
    {
        /** Generate the rest of the parameters   */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_generate_params(unit, FALSE, endpoint_info, test),
                            "Test failed.  Could not generate parameters.\n");

        /** A necessary limitation */
        if ((mep_db_lim == 1) && ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0))
        {
            /** Is this a 1/4 entry that might fail the "leader" test? */
            if ((endpoint_info->local_discr & 0x3) != 0)
            {
                if ((ep_index & 1) == 0)
                {
                    /** Even endpoints are leaders */
                    endpoint_info->local_discr = (endpoint_info->local_discr & ~0x3);
                }
                else
                {
                    /** Odd endpoints are followers */
                    temp = endpoint_id[ep_index - 1] + 1 + ((ep_index / 2) % 3);
                    endpoint_info->local_discr = (endpoint_info->local_discr & 0xFFFF0000) + temp;
                }
            }
        }

        /** Create BFD endpoint   */
        SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, endpoint_info), "Test failed.  Could not create endpoint\n");

        LOG_CLI((BSL_META("Added endpoint #%d\n"), ep_index + 1));

        /** Save endpoint ID for later   */
        endpoint_id[ep_index] = endpoint_info->id;

        /** Read endpoint parameters and compare   */
        SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2),
                            "Test failed.  Endpoint could not be read.\n");
        LOG_CLI((BSL_META("Checking values for endpoint #%d\n"), ep_index + 1));
        SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_compare_bfd_info_structs(unit, endpoint_info, endpoint_info2),
                            "Test failed.  Parameters read did not match parameters written.\n");
        LOG_CLI((BSL_META("Read endpoint #%d\n"), ep_index + 1));

        /** Only accelerated endpoints can be modified */
        if ((flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
        {
            /** Modify BFD endpoint   */
            endpoint_info->flags |= BCM_BFD_ENDPOINT_UPDATE;
            diag_dnx_bfd_test_generate_params(unit, TRUE, endpoint_info, test);

            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, endpoint_info),
                                "Test failed.  Could not modify endpoint\n");
            LOG_CLI((BSL_META("Modify endpoint #%d\n"), ep_index + 1));

            /** Read and compare again   */
            endpoint_info->flags &= ~BCM_BFD_ENDPOINT_UPDATE;
            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2),
                                "Test failed.  Endpoint could not be read.\n");
            LOG_CLI((BSL_META("Checking values for endpoint #%d\n"), ep_index + 1));
            SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_compare_bfd_info_structs(unit, endpoint_info, endpoint_info2),
                                "Test failed.  Parameters read did not match modified parameters written.\n");
            LOG_CLI((BSL_META("Read modified endpoint #%d\n"), ep_index + 1));
        }

        if (!BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(ep_type))
        {
            /** Delete MPLS encapsulation   */
            SHR_CLI_EXIT_IF_ERR(bcm_mpls_port_delete(unit, PWE_VPN_ID, test->mpls_port_ingress.mpls_port_id),
                                "Test failed.  Could not delete MPLS port.");

            /** Use a different port for the next endpoint */
            test->cur_gport += GPORT_ID_DELTA;
        }
        if (endpoint_info->ipv6_extra_data_index != 0)
        {
            endpoint_info->ipv6_extra_data_index++;
        }
    }

    /** Delete half of the endpoints individually   */
    for (ep_index = 0; ep_index < half_num_eps; ep_index++)
    {
        SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_destroy(unit, endpoint_id[ep_index]),
                            "Test failed.  Could not delete endpoint.\n");
        LOG_CLI((BSL_META("Deleted endpoint #%d\n"), ep_index + 1));

        rv = bcm_bfd_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  Endpoint #%d not really deleted.\n", ep_index + 1);
        }
    }

    /** Delete the rest   */
    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_destroy_all(unit), "Test failed.  Could not delete all endpoints.\n");
    LOG_CLI((BSL_META("Deleted all remaining endpoints\n")));

    /** Make sure all endpoints are deleted   */
    for (ep_index = half_num_eps; ep_index < num_eps; ep_index++)
    {
        rv = bcm_bfd_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  Endpoint #%d not really deleted.\n", ep_index + 1);
        }
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    /** Create VPN for MPLS/PWE tunnel endpoints   */
    if (!BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(ep_type))
    {
        SHR_CLI_EXIT_IF_ERR(bcm_vlan_destroy(unit, PWE_VPN_ID), "Test failed.  Could not destroy MPLS VLAN\n");
    }
    SHR_FREE(endpoint_info);
    SHR_FREE(endpoint_info2);
    SHR_FREE(endpoint_id);
    SHR_FREE(test);
    SHR_FUNC_EXIT;
}

/** Supported endpoint types   */
static sh_sand_enum_t diag_dnx_bfd_ep_all_type_enum_table[] = {
    {"UDP", bcmBFDTunnelTypeUdp},
    {"MPLS", bcmBFDTunnelTypeMpls},
    {"PWECW", bcmBFDTunnelTypePweControlWord},
    {"PWERA", bcmBFDTunnelTypePweRouterAlert},
    {"PWETTL", bcmBFDTunnelTypePweTtl},
    {"MPLSTPCC", bcmBFDTunnelTypeMplsTpCc},
    {"PWEGAL", bcmBFDTunnelTypePweGal},
    {NULL}
};

/** Supported flag combinations   */
static sh_sand_enum_t diag_dnx_bfd_ep_more_options_enum_table[] = {
    {"NONE", 0},
    {"IPV6", BCM_BFD_ENDPOINT_IPV6},
    {"MULTIHOP", BCM_BFD_ENDPOINT_MULTIHOP},
    {"IPV6MH", (BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_MULTIHOP)},
    {"ACCELERATED", BCM_BFD_ENDPOINT_IN_HW},
    {"ACC_IPV6", BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_IPV6},
    {"ACC_MH", BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP},
    {"ACC_IPV6MH", (BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_MULTIHOP)},
    {"ACC_MEP_TYPE_DISCR", (BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_KEY_TYPE_USE_YOUR_DISC)},
    {"ACC_MHMTD", (BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP | BCM_BFD_ENDPOINT_KEY_TYPE_USE_YOUR_DISC)},
    {"ACC_IPV6MTD", (BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_KEY_TYPE_USE_YOUR_DISC)},
    {"ACC_IPV6MHMTD", (BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_MULTIHOP | BCM_BFD_ENDPOINT_KEY_TYPE_USE_YOUR_DISC)},
    {NULL}
};

/** Test arguments   */
static sh_sand_option_t diag_dnx_bfd_sem_test_options[] = {
    {"Number", SAL_FIELD_TYPE_INT32, "Number of endpoints", "4"},
    {"Type", SAL_FIELD_TYPE_ENUM, "Type of tunneling", "UDP", (void *) diag_dnx_bfd_ep_all_type_enum_table},
    {"FLaGs", SAL_FIELD_TYPE_ENUM, "EP flags", "NONE", (void *) diag_dnx_bfd_ep_more_options_enum_table},
    {NULL}
};

/** Test manual   */
static sh_sand_man_t diag_dnx_bfd_sem_test_man = {
    /** Brief */
    "Semantic test of basic BFD APIs",
    /** Full */
    "Create BFD endpoints.  Get endpoints and compare."
        " Modify some endpoints.  Get endpoints and compare."
        " Delete half the endpoints individually." " Delete the rest of the endpoints.",
    /** Synopsis   */
    "ctest bfd semantic [Number=<value> Type=<value> FLaGs=<value>]",
    /** Example   */
    "Number=3 Type=UDP FLaGs=MULTIHOP",
};

/** Automatic test list (ctest bfd run)   */
static sh_sand_invoke_t diag_dnx_bfd_sem_tests[] = {
    {"eps_4_ipv4", "Number=4 Type=UDP FLaGs=NONE"},
    {"eps_6_ipv4", "Number=6 Type=UDP FLaGs=NONE"},
    {"eps_4_ipv6", "Number=4 Type=UDP FLaGs=IPV6"},
    {"eps_6_ipv6", "Number=6 Type=UDP FLaGs=IPV6"},
    {"eps_4_ipv4_mh", "Number=4 Type=UDP FLaGs=MULTIHOP"},
    {"eps_6_ipv4_mh", "Number=6 Type=UDP FLaGs=MULTIHOP"},
    {"eps_4_ipv6_mh", "Number=4 Type=UDP FLaGs=IPV6MH"},
    {"eps_6_ipv6_mh", "Number=6 Type=UDP FLaGs=IPV6MH"},
    {"eps_6_ipv4_mpls", "Number=6 Type=MPLS FLaGs=NONE"},
    {"eps_6_pwecw", "Number=6 Type=PWECW FLaGs=NONE"},
    {"eps_6_pwera", "Number=6 Type=PWERA FLaGs=NONE"},
    {"eps_6_pwettl", "Number=6 Type=PWETTL FLaGs=NONE"},
    {"eps_6_mplstpcc", "Number=6 Type=MPLSTPCC FLaGs=NONE"},
    {"eps_6_pwegal", "Number=6 Type=PWEGAL FLaGs=NONE"},
    {"acc_eps_4_ipv4", "Number=4 Type=UDP FLaGs=ACCELERATED"},
    {"acc_eps_6_ipv4", "Number=6 Type=UDP FLaGs=ACCELERATED"},
    {"acc_eps_4_ipv6", "Number=4 Type=UDP FLaGs=ACC_IPV6"},
    {"acc_eps_6_ipv6", "Number=6 Type=UDP FLaGs=ACC_IPV6"},
    {"acc_eps_4_ipv4_mh", "Number=4 Type=UDP FLaGs=ACC_MH"},
    {"acc_eps_6_ipv4_mh", "Number=6 Type=UDP FLaGs=ACC_MH"},
    {"acc_eps_4_ipv6_mh", "Number=4 Type=UDP FLaGs=ACC_IPV6MH"},
    {"acc_eps_6_ipv6_mh", "Number=6 Type=UDP FLaGs=ACC_IPV6MH"},
    {"acc_eps_6_ipv4_mpls", "Number=6 Type=MPLS FLaGs=ACCELERATED"},
    {"acc_eps_6_pwecw", "Number=6 Type=PWECW FLaGs=ACCELERATED"},
    {"acc_eps_6_pwera", "Number=6 Type=PWERA FLaGs=ACCELERATED"},
    {"acc_eps_6_pwettl", "Number=6 Type=PWETTL FLaGs=ACCELERATED"},
    {"acc_eps_6_mplstpcc", "Number=6 Type=MPLSTPCC FLaGs=ACCELERATED"},
    {"acc_eps_6_pwegal", "Number=6 Type=PWEGAL FLaGs=ACCELERATED"},
    {"acc_eps_6_mtd", "Number=6 Type=UDP FLaGs=ACC_MEP_TYPE_DISCR"},
    {"acc_eps_6_mtdhd", "Number=6 Type=UDP FLaGs=ACC_MHMTD"},
    {"acc_eps_6_ipv6_mtd", "Number=6 Type=UDP FLaGs=ACC_IPV6MTD"},
    {"acc_eps_6_ipv6_mtdhd", "Number=6 Type=UDP FLaGs=ACC_IPV6MHMTD"},
    {NULL}
};

/** List of BFD tests   */
sh_sand_cmd_t dnx_bfd_test_cmds[] = {
    {"semantic", diag_dnx_bfd_semantic_test_cmd, NULL, diag_dnx_bfd_sem_test_options, &diag_dnx_bfd_sem_test_man, NULL,
     diag_dnx_bfd_sem_tests},
    {NULL}
};
