/** \file ctest_dnx_bfd.c
 * 
 * Tests for BFD
 * 
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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
#include <include/bcm_int/dnx/algo/oam/algo_oam.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include "src/bcm/dnx/algo/lif_mngr/global_lif_allocation.h"
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#ifdef BCM_DNX2_SUPPORT
/* { */
#include <src/bcm/dnx/oam/oamp_v1/bfd_oamp_v1.h>
#include <src/bcm/dnx/oam/oamp_v1/oam_oamp_v1.h>
/* } */
#endif
#include <include/bcm/stack.h>

#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>

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

/** Flags2 that are supported by Jericho 2 */
#define BFD_ACC_FLAGS2 (BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION | BCM_BFD_ENDPOINT_FLAGS2_TX_STATISTICS | BCM_BFD_ENDPOINT_FLAGS2_RX_STATISTICS)

/** Bit that is used to determine whether or not endpoint will be IPv6 in cluster test */
#define CLUSTER_IPV6_BIT 2

/** Bit that is used to determine whether or not endpoint will be BFD ECHO */
#define ECHO_BFD_BIT 4

/**
 * Since these values are limited in number, they
 * are set at the beginning of the test.
 */
uint32 req_ints[NOF_REQ_INTS];

/** Structure to keep current values of parameters   */
typedef struct
{
    /** 
     *  Value that is used for parameters with no numeral
     *  restrictions: local discriminator LSB, parts of IPv6
     *  address and IPv4 address
     */
    uint32 gen_value;

    /** Value of gport for tunneled BFD */
    bcm_gport_t cur_gport;

    /** Label for MPLS encapsulation   */
    bcm_mpls_label_t mpls_label;

    /** Structure for adding MPLS encapsulation   */
    bcm_mpls_port_t mpls_port_ingress;

    /** Specifies whether this is a short entry */
    uint8 is_short_entry;

    /** BFD Tx period cluster parameters */
    uint32 cluster_tx_period;
    uint32 cluster_overall_mep_index;
    uint8 cluster_id;

    /** Specifies whether MEP type in discriminator is used */
    uint8 mep_type_discr;
} diag_dnx_bfd_semantic_test_struct;

const uint16 physical_ports[NOF_PHYS_PORTS] = {
    200,
    201
};

int update_test_value_set = 0;

/**
 * \brief - Create a value for the remote_gport field.
 *
 * \param [in] unit - Number of hardware unit used.
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
static void
diag_dnx_bfd_test_generate_remote_gport(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    diag_dnx_bfd_semantic_test_struct * test)
{
    uint32 trap_id, trap_strength, snoop_strength;

    /** Build remote_gport field from its parts   */

    /**
     * Limit trap ID to OAM/BFD range - some traps outside
     * this range are invalid for some devices
     */
    trap_id = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_OAM_REFLECTOR +
        ((test->gen_value) %
         (DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_OAM_CPU_SNOOP - DBAL_ENUM_FVAL_INGRESS_TRAP_ID_OAM_REFLECTOR + 1));
    test->gen_value += GEN_VALUE_DELTA;
    trap_strength = ((test->gen_value) & DNX_OAM_MAX_FORWARDING_STRENGTH);
    test->gen_value += GEN_VALUE_DELTA;
    snoop_strength = ((test->gen_value) & DNX_OAM_MAX_SNOOP_STRENGTH);
    test->gen_value += GEN_VALUE_DELTA;
    _SHR_GPORT_TRAP_SET(endpoint_info->remote_gport, trap_id, trap_strength, snoop_strength);
}

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
    uint32 ip6_index, ip6_segment, discr_high, discr_low = 0;
    uint32 num_fec_ids, num_lif_ids, itf_id, new_bank_offset;
    int explicit_detection_time, mep_db_threshold, avail_full_entries, total_full_entries;
    int rmep_db_threshold, total_full_rmep_entries, avail_full_rmep_entries;

    int nof_mep_db_entries_per_bank =
        DNX_OAMP_IS_V2(unit) ? 0 : dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    int nof_mep_db_sub_entries = DNX_OAMP_IS_V2(unit) ? 0 : dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    int bank_offset_in_sub_entries = nof_mep_db_entries_per_bank * nof_mep_db_sub_entries;

    int nof_entry_banks_for_eps = dnx_data_oam.oamp.nof_mep_db_entry_banks_for_endpoints_get(unit);
    uint8 itf_type, port_index;
    uint32 oamp_rmep_scale_limitataion =
        dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_rmep_scale_limitataion);
    uint32 nof_entries_per_rmep = 0;
    uint32 full_rmep_entry_shift = 0;
    bcm_gport_t gport_id;
    /*
     * 4 mutually exclusive flags for punt profile. The first one is shared between loc_punt_event and
     * state_punt_event, so it isn't marked on the latter 
     */
    uint32 loc_punt_flags[] = { BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE, BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE,
        BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE, 0
    };
    uint32 state_punt_flags[] = { 0, BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE,
        BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE, 0
    };
    uint32 punt_flags =
        BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE | BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE |
        BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE | BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE |
        BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE;
    SHR_FUNC_INIT_VARS(unit);

    /** 
     *  These fields may not be modified after endpoint is
     *  created
     */
    if (for_modification == FALSE)
    {
        /** The discriminator range is set with this value of the MSB */
        discr_high = (FIXED_DISCR_RANGE & UTILEX_BITS_MASK(SAL_UINT32_NOF_BITS - 1, DISCR_RANGE_BIT_SHIFT));

        if (test->mep_type_discr)
        {
            UTILEX_SET_BIT(discr_high, !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP),
                           SAL_UINT32_NOF_BITS - 1);
        }

        /** Is the endpoint accelerated? */
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
        {
            if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
            {
                /** UDP source port is a constant */
                endpoint_info->udp_src_port = FIXED_UDP_SRC_PORT;
            }

            mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
            total_full_entries = dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get(unit);
            avail_full_entries = total_full_entries - (mep_db_threshold >> FULL_ENTRY_ID_SHIFT);
            if ((mep_db_threshold != 0))
            {
#ifdef BCM_DNX2_SUPPORT
                if (DNX_OAMP_IS_V1(unit))
                {
                    uint32 highest_mep_db_index = 0, max_num_of_entries_in_bank, bank;
                    /** Highest mep_db entry in short bank */
                    dnx_oamp_v1_mep_db_highest_index_get(unit, &highest_mep_db_index);

                    /** Max number of entries per bank*/
                    max_num_of_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
                        dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
                    /** bank where entry resides */
                    bank = ((test->gen_value >> ACC_DISCR_OFFSET) % mep_db_threshold) / max_num_of_entries_in_bank;
                    /** Build a discriminator that results in a 1/4 entry endpoint */
                    discr_low =
                        bank * max_num_of_entries_in_bank +
                        ((test->gen_value >> ACC_DISCR_OFFSET) % highest_mep_db_index);
                    test->is_short_entry = TRUE;
                }
#endif
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
            if (endpoint_info->type != bcmBFDTunnelTypeUdp)
            {
                /** local_discr MSB=1 not allowed for MPLS/PWE endpoints */
                endpoint_info->local_discr &= 0x7FFFFFFF;
            }
            endpoint_info->gport = 0;

            /** Also, try explicit remote ID */
            if ((endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_WITH_ID) != 0)
            {
                rmep_db_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
                if ((test->gen_value & RMEP_H_ENTRY_BIT) && (rmep_db_threshold != 0))
                {
                    /** Make sure the RMEP ID is below the threshold */
                    nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 2 : 1;
                    endpoint_info->remote_id =
                        ((test->gen_value >> REMOTE_ID_OFFSET) % (rmep_db_threshold / nof_entries_per_rmep));
                    endpoint_info->remote_id = rmep_db_threshold * nof_entries_per_rmep;
                }
                else
                {
                    /** Make sure the RMEP ID is above the threshold */
                    rmep_db_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
                    full_rmep_entry_shift = FULL_RMEP_ENTRY_SHIFT;
                    nof_entries_per_rmep = 2;
                    if (oamp_rmep_scale_limitataion)
                    {
                        full_rmep_entry_shift += 1;
                        nof_entries_per_rmep = nof_entries_per_rmep * 2;
                    }
                    total_full_rmep_entries =
                        (dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) -
                         rmep_db_threshold) / nof_entries_per_rmep;
                    avail_full_rmep_entries = total_full_rmep_entries - (rmep_db_threshold >> full_rmep_entry_shift);
                    endpoint_info->remote_id = rmep_db_threshold +
                        (((test->gen_value >> REMOTE_ID_OFFSET) % avail_full_rmep_entries) << full_rmep_entry_shift);
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
             /* coverity[Copy-paste error:false]  */
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
    diag_dnx_bfd_test_generate_remote_gport(unit, endpoint_info, test);

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

        if (test->cluster_id != 0)
        {
            endpoint_info->bfd_period_cluster = test->cluster_id;
            endpoint_info->bfd_period = test->cluster_tx_period;

            /** Since the cluster creates more endpoints, use of profiles must be limited */
            endpoint_info->int_pri = ((test->cluster_id) & DNX_BFD_MAX_INT_PRI);

            /** Since endpoints in a cluster share a MEP profile, these values must be consistent */
            endpoint_info->flags2 = (test->cluster_id) & BFD_ACC_FLAGS2;

            /** Different remote_gport values would use up all MP profiles */
            endpoint_info->remote_gport = BCM_GPORT_INVALID;

            /** Some of the endpoints should be IPv6 */
            if ((test->gen_value & CLUSTER_IPV6_BIT) != 0)
            {
                endpoint_info->flags |= BCM_BFD_ENDPOINT_IPV6;
                if (for_modification == FALSE)
                {
                    /** All IPv6 extra data placed in non-MEP banks */
                    new_bank_offset = nof_entry_banks_for_eps * bank_offset_in_sub_entries;

                    /** Offset in bank (in full entries) is index of MEP */
                    endpoint_info->ipv6_extra_data_index =
                        new_bank_offset + test->cluster_overall_mep_index * nof_entry_banks_for_eps;
                }
            }
        }
        else
        {
            /** This number should be a multiple of 167, and may be 0 */
            endpoint_info->bfd_period = (test->gen_value) % (DNX_BFD_MAX_BFD_PERIOD / 167) * 167;
            test->gen_value += GEN_VALUE_DELTA;
            endpoint_info->int_pri = ((test->gen_value) & DNX_BFD_MAX_INT_PRI);
            test->gen_value += GEN_VALUE_DELTA;
            endpoint_info->flags2 = ((test->gen_value) & BFD_ACC_FLAGS2);
            test->gen_value += GEN_VALUE_DELTA;
        }

        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) == 0)
        {
            /** This flag is only allowed for multi-hop endpoints */
            endpoint_info->flags2 &= ~BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION;
        }
        else
        {
            if (((test->gen_value & ECHO_BFD_BIT) != 0) && (test->cluster_id == 0) && !for_modification)
            {
                /** If not multi-hop, maybe echo? */
                endpoint_info->flags |= BCM_BFD_ECHO;
            }
        }

        if ((endpoint_info->flags & BCM_BFD_ECHO) != 0)
        {
            /** Mandatory values for echo */
            endpoint_info->local_state = 0;
            endpoint_info->local_diag = 0;
            endpoint_info->remote_state = 0;
            endpoint_info->remote_diag = 0;
            endpoint_info->remote_discr = 0;
            endpoint_info->remote_gport = BCM_GPORT_INVALID;
            endpoint_info->ip_subnet_length = 0;
        }

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
            {
                num_lif_ids = 0xC0000;
            }
            itf_id = (test->gen_value) % num_lif_ids;
        }
        BCM_L3_ITF_SET(endpoint_info->egress_if, itf_type, itf_id >> ITF_ID_SHIFT);
        test->gen_value += GEN_VALUE_DELTA;

        port_index = (test->gen_value) % NOF_PHYS_PORTS;

        if (itf_type != BCM_L3_ITF_TYPE_FEC)
        {
            BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, physical_ports[port_index]);
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
            endpoint_info->ip_ttl = 255;
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
             && _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP)))
        {
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

        if ((endpoint_info->flags & BCM_BFD_ECHO) == 0)
        {
            endpoint_info->local_state = ((test->gen_value) & DNX_BFD_MAX_STATE);
            test->gen_value += GEN_VALUE_DELTA;

            endpoint_info->local_diag = ((test->gen_value) % NOF_DIAG_VALUES);
            test->gen_value += GEN_VALUE_DELTA;
        }

        endpoint_info->local_flags = BFD_FLAGS_PROFILE_TO_VAL((test->gen_value) & DNX_BFD_MAX_FLAGS_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->local_detect_mult = ((test->gen_value) & DNX_BFD_MAX_DETECT_MULT);
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->local_min_rx = req_ints[test->gen_value % NOF_REQ_INTS];
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->local_min_tx = req_ints[test->gen_value % NOF_REQ_INTS];
        test->gen_value += GEN_VALUE_DELTA;

        endpoint_info->remote_detect_mult = ((test->gen_value) & DNX_BFD_MAX_DETECT_MULT);
        test->gen_value += GEN_VALUE_DELTA;

        if (((test->gen_value & EXPLICIT_DETECTION_BIT) != 0) && (endpoint_info->local_min_rx != 0)
            && (endpoint_info->remote_detect_mult != 0))
                                                       /** To avoid a negative value */
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

        if ((endpoint_info->flags & BCM_BFD_ECHO) == 0)
        {
            endpoint_info->remote_discr = test->gen_value;
            test->gen_value += GEN_VALUE_DELTA;

            endpoint_info->remote_diag = ((test->gen_value) % NOF_DIAG_VALUES);
            test->gen_value += GEN_VALUE_DELTA;
        }

        endpoint_info->remote_flags = BFD_FLAGS_PROFILE_TO_VAL((test->gen_value) & DNX_BFD_MAX_FLAGS_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;

        if ((endpoint_info->flags & BCM_BFD_ECHO) == 0)
        {
            endpoint_info->remote_state = ((test->gen_value) & DNX_BFD_MAX_STATE);
            test->gen_value += GEN_VALUE_DELTA;
        }
        /*
         * Punt profile flags.
         * First clear then set the new ones
         */
        endpoint_info->flags &= ~punt_flags;
        endpoint_info->flags |= loc_punt_flags[test->gen_value % 4];
        endpoint_info->flags |= state_punt_flags[test->gen_value % 4];
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
#ifdef BCM_DNX2_SUPPORT
/* { */
    dnx_bfd_oamp_v1_endpoint_t *mep_oamp_v1_entry_values = NULL;
/* } */
#endif
    dnx_oam_mep_profile_t *mep_profile = NULL;
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
        if (endpoint_info2->gport != endpoint_info->gport)
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
            (endpoint_info2->ip_subnet_length != endpoint_info->ip_subnet_length) && (DNX_OAMP_IS_V1(unit)))
        {
             /** This field is relevant only for IPv4 single-hop, valid only for DNX2 */
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Subnet length read doesn't match Subnet length written to endpoint.  "
                         "Written: 0x%08X. Read: 0x%08X.\n", endpoint_info->ip_subnet_length,
                         endpoint_info2->ip_subnet_length);
        }

        /** Added buffer for the round-up */
        if ((endpoint_info2->bfd_period > (endpoint_info->bfd_period + 5))
            && (endpoint_info2->bfd_period < (endpoint_info->bfd_period - 5)))
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

        if (endpoint_info2->tx_gport != endpoint_info->tx_gport)
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
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) != 0)
        {
            SHR_ALLOC_SET_ZERO(mep_profile, sizeof(dnx_oam_mep_profile_t),
                               "Structure used to call OAMP MEP profile get", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            /** Make sure checking source IP is disabled */
            {
#ifdef BCM_DNX2_SUPPORT
/* { */
                SHR_ALLOC_SET_ZERO(mep_oamp_v1_entry_values, sizeof(dnx_bfd_oamp_v1_endpoint_t),
                                   "Structure used to call OAMP MEP DB get", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
                /** Read from OAMP MEP DB table */
                SHR_IF_ERR_EXIT(dnx_bfd_oamp_v1_mep_db_endpoint_get(unit, endpoint_info->id, mep_oamp_v1_entry_values));

                SHR_CLI_EXIT_IF_ERR(dnx_oam_oamp_v1_mep_profile_get
                                    (unit, mep_oamp_v1_entry_values->mep_profile, mep_profile),
                                    "Test failed.  Could not read MEP profile\n");
                if (mep_profile->src_ip_check_dis != TRUE)
                {
                    SHR_CLI_EXIT(_SHR_E_FAIL,
                                 "Test failed.  EP is IPv6, but source IP check disable is not set in profile");
                }
/* } */
#endif
            }
        }
    }

exit:
    SHR_FREE(ipv6_str);
    SHR_FREE(ipv6_str2);
    SHR_FUNC_EXIT;
}

static void
update_test_value(
    diag_dnx_bfd_semantic_test_struct * test)
{
    int req_int_index;
    for (req_int_index = 0; req_int_index < NOF_REQ_INTS; req_int_index++)
    {
        req_ints[req_int_index] = test->gen_value;
        test->gen_value += GEN_VALUE_DELTA;
        test->gen_value &= ~Q_ENTRY_BIT;
    }
    update_test_value_set = 1;
}

static int
is_prime(
    int a)
{
    int c;
    for (c = 2; c <= a - 1; c++)
    {
        if (a % c == 0)
            return 0;
    }
    return 1;
}

static int
lowest_prime_greater_than(
    int num)
{
    while (1)
    {
        ++num;
        if (is_prime(num))
            return num;
    }
}

shr_error_e
free_all_mep_ids(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    int nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    int nof_mep_db_entry_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit);
    int nof_mep_db_sub_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    int nof_iterations = 0;
    uint8 is_allocated = 0;
    uint32 mep_id = 0;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    nof_iterations = nof_mep_db_entry_banks * nof_mep_db_entries_per_bank;
    for (i = 0; i < nof_iterations; i++)
    {
#ifdef BCM_DNX2_SUPPORT
        if (DNX_OAMP_IS_V1(unit))
        {
            SHR_IF_ERR_EXIT(dnx_algo_oamp_v1_mep_id_is_allocated(unit, mep_id, &is_allocated));
            if (is_allocated)
            {
                SHR_IF_ERR_EXIT(dnx_algo_oamp_v1_mep_id_dealloc(unit, mep_id));
            }
        }
        else
#endif
        {
        }
        mep_id += nof_mep_db_sub_entries;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
is_free_tuple(
    int unit,
    uint32 mep_id,
    uint8 *is_free_ret)
{
    uint32 mep_id_iter = 0;
    uint8 is_free = 0;
    uint8 *is_allocated = NULL;
    int i = 0, j = 0;
    int nof_bfd_ipv6_extra_data_entries = DNX_BFD_IPV6_NOF_EXTRA_DATA_ENTRIES;
    int nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    int nof_mep_db_entry_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit);
    int oamp_mep_full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    int bank_index = OAM_ID_TO_MEP_DB_ENTRY(mep_id - oamp_mep_full_entry_threshold) / nof_mep_db_entries_per_bank;
    int nof_iterations = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(is_allocated, nof_bfd_ipv6_extra_data_entries, "Array of tuples allocation indication",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    nof_iterations =
        (nof_mep_db_entry_banks - nof_bfd_ipv6_extra_data_entries + 1) * nof_mep_db_entries_per_bank -
        OAM_ID_TO_MEP_DB_ENTRY(oamp_mep_full_entry_threshold);
    for (i = 0; i < nof_iterations; i++)
    {
        is_free = 1;
        for (j = 0; j < nof_bfd_ipv6_extra_data_entries; j++)
        {
            int current_mep_id =
                mep_id_iter + MEP_DB_ENTRY_TO_OAM_ID(nof_mep_db_entries_per_bank * j) + oamp_mep_full_entry_threshold;

            if (bank_index ==
                OAM_ID_TO_MEP_DB_ENTRY(current_mep_id - oamp_mep_full_entry_threshold) / nof_mep_db_entries_per_bank)
            {
                is_allocated[j] = 1;
            }
            else
            {
#ifdef BCM_DNX2_SUPPORT
                if (DNX_OAMP_IS_V1(unit))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_oamp_v1_mep_id_is_allocated(unit, current_mep_id, &is_allocated[j]));
                }
                else
#endif
                {
                }
            }
            is_free &= !is_allocated[j];
        }
        if (is_free)
        {
            *is_free_ret = 1;
            return _SHR_E_NONE;
        }
        mep_id_iter += MEP_DB_ENTRY_TO_OAM_ID(1);
    }
    *is_free_ret = 0;
exit:
    SHR_FREE(is_allocated);
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dnx_bfd_semantic_test_bfd_ipv6_update(
    int unit,
    int generator)
{
    bcm_bfd_endpoint_info_t *endpoint_info = NULL;
    bcm_bfd_endpoint_info_t *endpoint_info2 = NULL;
    diag_dnx_bfd_semantic_test_struct *test = NULL;
    int nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    int nof_mep_db_entry_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit);
    int oamp_mep_full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    uint32 current_mep_id = 0;
    int i, prime;
    SHR_FUNC_INIT_VARS(unit);
    LOG_CLI((BSL_META("Running BFD over IPv6 update test with generator = %d\n"), generator));
    SHR_ALLOC(endpoint_info, sizeof(bcm_bfd_endpoint_info_t), "Struct for creating BFD endpoints",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(endpoint_info2, sizeof(bcm_bfd_endpoint_info_t), "Struct for creating BFD endpoints",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    bcm_bfd_endpoint_info_t_init(endpoint_info);
    /*
     * save the extra data in the 10'th bank 
     */
    endpoint_info->ipv6_extra_data_index = MEP_DB_ENTRY_TO_OAM_ID(nof_mep_db_entries_per_bank * 9);
    endpoint_info->flags = BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_IN_HW;

    SHR_ALLOC(test, sizeof(diag_dnx_bfd_semantic_test_struct), "Struct for test variables", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);
    test->gen_value = (INIT_GEN_VALUE & ~Q_ENTRY_BIT);

    test->cluster_id = 0;

    diag_dnx_bfd_test_generate_params(unit, FALSE, endpoint_info, test);
    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, endpoint_info), "Test failed.  Could create endpoint\n");

    prime =
        lowest_prime_greater_than(nof_mep_db_entry_banks * nof_mep_db_entries_per_bank -
                                  OAM_ID_TO_MEP_DB_ENTRY(oamp_mep_full_entry_threshold));

    for (i = 0; i < prime; i++)
    {
        int rv = _SHR_E_NONE;
        uint8 is_allocated = 0;
        uint8 _is_free_tuple = 0;
        if (!update_test_value_set)
        {
            update_test_value(test);
        }
        endpoint_info->flags |= BCM_BFD_ENDPOINT_UPDATE;
        diag_dnx_bfd_test_generate_params(unit, TRUE, endpoint_info, test);
        SHR_IF_ERR_EXIT(is_free_tuple(unit, endpoint_info->id, &_is_free_tuple));

        rv = bcm_bfd_endpoint_create(unit, endpoint_info);

        if (_is_free_tuple)
        {
            if (rv != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.\n");
            }
            endpoint_info->flags &= ~BCM_BFD_ENDPOINT_UPDATE;
            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_info->id, endpoint_info2),
                                "Test failed.  Endpoint could not be read.\n");
            SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_compare_bfd_info_structs(unit, endpoint_info, endpoint_info2),
                                "Test failed.  Parameters read did not match modified parameters written.\n");
        }
        else
        {
            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_destroy(unit, endpoint_info->id),
                                "Test failed.  Could not delete endpoint.\n");
            SHR_IF_ERR_EXIT(free_all_mep_ids(unit, endpoint_info));
            if (rv != _SHR_E_RESOURCE)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed - expected resource error.\n");
            }
            LOG_CLI((BSL_META("BFD over IPv6 update test with generator = %d passed\n"), generator));
            SHR_EXIT();
        }

        current_mep_id += oamp_mep_full_entry_threshold;
        if (current_mep_id < MEP_DB_ENTRY_TO_OAM_ID(nof_mep_db_entry_banks * nof_mep_db_entries_per_bank))
        {
#ifdef BCM_DNX2_SUPPORT
            if (DNX_OAMP_IS_V1(unit))
            {
                dnx_oamp_mep_db_memory_type_t mep_id_memory_type;
                dnx_algo_oamp_v1_mep_id_is_allocated(unit, current_mep_id, &is_allocated);

                if (!is_allocated)
                {
                    dnx_algo_oamp_v1_mep_id_alloc(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &mep_id_memory_type,
                                                  &current_mep_id);
                }
            }
            else
#endif
            {
            }
        }
        current_mep_id -= oamp_mep_full_entry_threshold;

        current_mep_id += MEP_DB_ENTRY_TO_OAM_ID(generator);
        current_mep_id = current_mep_id % (MEP_DB_ENTRY_TO_OAM_ID(prime));
    }
    SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.\n");
exit:

    SHR_FREE(endpoint_info);
    SHR_FREE(endpoint_info2);
    SHR_FREE(test);
    SHR_FUNC_EXIT;

}

shr_error_e
diag_dnx_bfd_semantic_test_bfd_update(
    int unit,
    int ep_index,
    diag_dnx_bfd_semantic_test_struct * test)
{
    bcm_bfd_endpoint_info_t *bfd_endpoint_info = NULL;
    bcm_bfd_endpoint_info_t *bfd_endpoint_info_2 = NULL;
    int i;
    int oamp_mep_full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(bfd_endpoint_info, sizeof(bcm_bfd_endpoint_info_t), "Struct for creating BFD endpoints",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_ERR_EXIT(bfd_endpoint_info_2, sizeof(bcm_bfd_endpoint_info_t), "Struct for reading BFD endpoints",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    bcm_bfd_endpoint_info_t_init(bfd_endpoint_info);
    bcm_bfd_endpoint_info_t_init(bfd_endpoint_info_2);

    for (i = 0; i < 50; i++)
    {
        SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, ep_index, bfd_endpoint_info),
                            "Test failed.  Endpoint could not be read.\n");

        /**
         * IPv6 short entry endpoints may not be modified,
         * because it cannot be done atomically.
         */
        if ((ep_index < oamp_mep_full_entry_threshold) && (bfd_endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) != 0)
        {
            SHR_EXIT();
        }

        bfd_endpoint_info->flags |= BCM_BFD_ENDPOINT_UPDATE;
        diag_dnx_bfd_test_generate_params(unit, TRUE, bfd_endpoint_info, test);

        if (((bfd_endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) != 0) && ((i % 5) == 0))
        {
            /**
             * Since this is a bit tricky, we
             * want to test it thoroughly
             */
            bfd_endpoint_info->bfd_period = 0;
        }

        SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, bfd_endpoint_info),
                            "Test failed.  Could not modify endpoint\n");

        bfd_endpoint_info->flags &= ~BCM_BFD_ENDPOINT_UPDATE;
        SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, bfd_endpoint_info->id, bfd_endpoint_info_2),
                            "Test failed.  Endpoint could not be read.\n");

        LOG_CLI((BSL_META("Checking values for endpoint #%d, step#%d\n"), ep_index, i));
        SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_compare_bfd_info_structs(unit, bfd_endpoint_info, bfd_endpoint_info_2),
                            "Test failed.  Parameters read did not match modified parameters written.\n");
    }

exit:
    SHR_FREE(bfd_endpoint_info);
    SHR_FREE(bfd_endpoint_info_2);
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
    uint32 num_eps = 0, sequence_number = 0, flags, ep_index, half_num_eps;
    uint32 cluster_ep_index, temp, mep_db_lim;
    int rv, cluster_index, num_clusters = 1;
    bcm_bfd_tunnel_type_t ep_type = bcmBFDTunnelTypeUdp;
    bcm_bfd_endpoint_t *endpoint_id = NULL;
    bcm_bfd_endpoint_info_t *endpoint_info = NULL, *endpoint_info2 = NULL;
    diag_dnx_bfd_semantic_test_struct *test = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("FLaGs", flags);
    if (flags == (BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_UPDATE))
    {
        SH_SAND_GET_INT32("Number", sequence_number);
    }
    else
    {
        SH_SAND_GET_INT32("Number", num_eps);
    }
    SH_SAND_GET_ENUM("Type", ep_type);

    if (flags == (BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_UPDATE))
    {
        int nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
        int nof_mep_db_entry_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit);
        int oamp_mep_full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
        int prime =
            lowest_prime_greater_than(nof_mep_db_entry_banks * nof_mep_db_entries_per_bank -
                                      OAM_ID_TO_MEP_DB_ENTRY(oamp_mep_full_entry_threshold));
        switch (sequence_number)
        {
            case 0:
                SHR_IF_ERR_EXIT(diag_dnx_bfd_semantic_test_bfd_ipv6_update(unit, 1));
                SHR_IF_ERR_EXIT(diag_dnx_bfd_semantic_test_bfd_ipv6_update(unit, prime - 1));
                break;
            case 1:
                SHR_IF_ERR_EXIT(diag_dnx_bfd_semantic_test_bfd_ipv6_update(unit, prime / 2));
                SHR_IF_ERR_EXIT(diag_dnx_bfd_semantic_test_bfd_ipv6_update(unit, 2));
                break;
            case 2:
                SHR_IF_ERR_EXIT(diag_dnx_bfd_semantic_test_bfd_ipv6_update(unit, nof_mep_db_entries_per_bank));
                SHR_IF_ERR_EXIT(diag_dnx_bfd_semantic_test_bfd_ipv6_update(unit, prime / 8));
                break;
            case 3:
                SHR_IF_ERR_EXIT(diag_dnx_bfd_semantic_test_bfd_ipv6_update(unit, nof_mep_db_entries_per_bank * 2));
                SHR_IF_ERR_EXIT(diag_dnx_bfd_semantic_test_bfd_ipv6_update(unit, 7));
                break;
        }
        SHR_EXIT();
    }

    LOG_CLI((BSL_META("Test with %d endpoints\n"), num_eps));
    LOG_CLI((BSL_META("Test with tunnel Type = %d\n"), ep_type));
    LOG_CLI((BSL_META("Test with flags = 0x%08X\n"), flags));

    /** Set values for tunnel-type loop   */
    half_num_eps = (num_eps >> 1);

    SHR_ALLOC_ERR_EXIT(endpoint_info, sizeof(bcm_bfd_endpoint_info_t), "Struct for creating BFD endpoints",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_ERR_EXIT(endpoint_info2, sizeof(bcm_bfd_endpoint_info_t), "Struct for reading BFD endpoints",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(endpoint_id, num_eps * sizeof(bcm_bfd_endpoint_t), "Array of endpoint IDs",
                                "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(test, sizeof(diag_dnx_bfd_semantic_test_struct), "Struct for test variables",
                                "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (!update_test_value_set)
    {
        update_test_value(test);
    }

    bcm_bfd_endpoint_info_t_init(endpoint_info);
    bcm_bfd_endpoint_info_t_init(endpoint_info2);

    test->gen_value = INIT_GEN_VALUE;
    test->cur_gport = GPORT_INIT_ID;
    test->mpls_label = MPLS_INIT_LABEL;

    if (num_eps == 3)
    {

        /** Test for default MEPs */
        for (ep_index = BCM_BFD_ENDPOINT_DEFAULT1; ep_index >= BCM_BFD_ENDPOINT_DEFAULT3; ep_index--)
        {
            endpoint_info->id = ep_index;

            /** Build remote_gport field */
            diag_dnx_bfd_test_generate_remote_gport(unit, endpoint_info, test);

            /** Create default BFD endpoint   */
            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, endpoint_info),
                                "Test failed.  Could not create endpoint\n");
            LOG_CLI((BSL_META("Added endpoint #%d\n"), BCM_BFD_ENDPOINT_DEFAULT0 - ep_index));

            /** Read endpoint parameters and compare   */
            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, ep_index, endpoint_info2),
                                "Test failed.  Endpoint could not be read.\n");
            LOG_CLI((BSL_META("Checking values for endpoint #%d\n"), BCM_BFD_ENDPOINT_DEFAULT0 - ep_index));

            SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_compare_bfd_info_structs(unit, endpoint_info, endpoint_info2),
                                "Test failed.  Endpoint info from get missmatch endpoint info from create!\n");

            if (endpoint_info2->remote_gport != endpoint_info->remote_gport)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  No match between remote_gport read from endpoint and value "
                             "written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n", endpoint_info->remote_gport,
                             endpoint_info2->remote_gport);
            }
        }

        for (ep_index = BCM_BFD_ENDPOINT_DEFAULT1; ep_index >= BCM_BFD_ENDPOINT_DEFAULT3; ep_index--)
        {
            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_destroy(unit, ep_index), "Test failed.  Could not delete endpoint.\n");
            LOG_CLI((BSL_META("Deleted endpoint #%d\n"), BCM_BFD_ENDPOINT_DEFAULT0 - ep_index));

            rv = bcm_bfd_endpoint_get(unit, ep_index, endpoint_info2);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  Endpoint #%d not really deleted.\n",
                             BCM_BFD_ENDPOINT_DEFAULT0 - ep_index);
            }
        }
        SHR_EXIT();
    }

    if (num_eps == 7)
    {
        /** Test for clustered endpoints */
        num_clusters = num_eps;

        /** Used for IPv6 endpoints */
        test->cluster_overall_mep_index = 0;
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

    if ((flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
    {
        if (ep_type == bcmBFDTunnelTypeUdp &&
            !_SHR_IS_FLAG_SET(flags, BCM_BFD_ENDPOINT_MULTIHOP) && !_SHR_IS_FLAG_SET(flags, BCM_BFD_ECHO))
        {
            endpoint_info->ip_ttl = 255;
        }

        if (ep_type == bcmBFDTunnelTypeMpls)
        {
            endpoint_info->ip_ttl = 1;
        }
    }

    /** Create VPN for MPLS/PWE tunnel endpoints   */
    if (!BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(ep_type))
    {
        SHR_CLI_EXIT_IF_ERR(bcm_vlan_create(unit, PWE_VPN_ID), "Test failed.  Could not create MPLS VLAN\n");
    }
    /*
     * if the endpoint is IPv6, accelerated and self-containd 
     */
    if ((flags & BCM_BFD_ENDPOINT_IPV6) != 0 && (flags & BCM_BFD_ENDPOINT_IN_HW) != 0 &&
        !((dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit) != 0) && ((test->gen_value & Q_ENTRY_BIT) != 0)))
    {
        endpoint_info->ipv6_extra_data_index = 0x1000C;
    }

    mep_db_lim = DNX_OAMP_IS_V2(unit) ? 0 : dnx_data_oam.oamp.mep_db_version_limitation_get(unit);
    for (cluster_index = 0; cluster_index < num_clusters; cluster_index++)
    {
        if (num_clusters != 1)
        {
            test->cluster_tx_period = (test->gen_value) % (DNX_BFD_MAX_BFD_PERIOD / 167) * 167;
            test->gen_value += GEN_VALUE_DELTA;

            test->cluster_id = (test->gen_value & 0xFF);
            test->gen_value += GEN_VALUE_DELTA;
        }
        for (ep_index = 0; ep_index < num_eps; ep_index++)
        {
            /** Generate the rest of the parameters   */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_generate_params(unit, FALSE, endpoint_info, test),
                                "Test failed.  Could not generate parameters.\n");

            /** Increment the counter to ensure different IPv6 extra data locations */
            test->cluster_overall_mep_index++;

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
            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, endpoint_info),
                                "Test failed.  Could not create endpoint\n");

            LOG_CLI((BSL_META("Added endpoint #%d\n"), ep_index + 1));

            if (ep_index == 0)
            {
                SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_destroy(unit, endpoint_info->id),
                                    "Test failed.  Could not delete endpoint.\n");

                SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, endpoint_info),
                                    "Test failed.  Could not create endpoint\n");
            }

            /** Save endpoint ID for later   */
            endpoint_id[ep_index] = endpoint_info->id;

            /** Read endpoint parameters and compare   */
            SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2),
                                "Test failed.  Endpoint could not be read.\n");
            LOG_CLI((BSL_META("Read endpoint #%d\n"), ep_index + 1));
            LOG_CLI((BSL_META("Checking values for endpoint #%d\n"), ep_index + 1));
            SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_test_compare_bfd_info_structs(unit, endpoint_info, endpoint_info2),
                                "Test failed.  Parameters read did not match parameters written.\n");

            /** Only accelerated endpoints can be modified */
            if ((flags & BCM_BFD_ENDPOINT_IN_HW) != 0)
            {
                SHR_CLI_EXIT_IF_ERR(diag_dnx_bfd_semantic_test_bfd_update(unit, endpoint_id[ep_index], test),
                                    "Test failed.  Could not modify endpoint\n");
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
                endpoint_info->ipv6_extra_data_index += 4;
            }
        }

        if (num_clusters != 1)
        {
            /** Now modify the bfd_period in each endpoint and make sure all others are affected */
            for (ep_index = 0; ep_index < num_eps; ep_index++)
            {
                LOG_CLI((BSL_META("Read endpoint #%d in cluster #%d\n"), ep_index + 1, cluster_index));
                SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_id[ep_index], endpoint_info),
                                    "Test failed.  Endpoint could not be read.\n");
                LOG_CLI((BSL_META("Modify bfd_period for endpoint #%d in cluster #%d\n"), ep_index + 1, cluster_index));
                endpoint_info->flags |= BCM_BFD_ENDPOINT_UPDATE;
                endpoint_info->bfd_period = ep_index * 167;
                SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, endpoint_info),
                                    "Test failed.  Could not modify endpoint\n");
                for (cluster_ep_index = 0; cluster_ep_index < num_eps; cluster_ep_index++)
                {
                    LOG_CLI((BSL_META("Read endpoint #%d in cluster #%d\n"), cluster_ep_index + 1, cluster_index));
                    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_id[cluster_ep_index], endpoint_info),
                                        "Test failed.  Endpoint could not be read.\n");
                    if (endpoint_info->bfd_period != (ep_index * 167))
                    {
                        SHR_CLI_EXIT(_SHR_E_FAIL,
                                     "Test failed.  bfd_period value read for Endpoint #%d is not correct .\n",
                                     cluster_ep_index + 1);
                    }
                }
            }
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
    {"IPV6_UPDATE", BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_UPDATE},
    {"MULTIHOP", BCM_BFD_ENDPOINT_MULTIHOP},
    {"IPV6MH", (BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_MULTIHOP)},
    {"ACCELERATED", BCM_BFD_ENDPOINT_IN_HW},
    {"ACC_IPV6", BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_IPV6},
    {"ACC_MH", BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP},
    {"ACC_IPV6MH", (BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_MULTIHOP)},
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
    {"eps_3_default", "Number=3 Type=UDP FLaGs=NONE"},
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
    {"acc_eps_7_cluster", "Number=7 Type=UDP FLaGs=ACCELERATED"},
    {NULL}
};

/** List of BFD tests   */
sh_sand_cmd_t dnx_bfd_test_cmds[] = {
    {"semantic", diag_dnx_bfd_semantic_test_cmd, NULL, diag_dnx_bfd_sem_test_options, &diag_dnx_bfd_sem_test_man, NULL,
     diag_dnx_bfd_sem_tests},
    {NULL}
};
